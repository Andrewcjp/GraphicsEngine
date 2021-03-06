#include "D3D12RHITexture.h"
#include "RHI\RHITypes.h"
#include "D3D12Helpers.h"
#include "D3D12RHI.h"
#include "D3D12Types.h"
#include "D3D12DeviceContext.h"
#include "DXMemoryManager.h"
#include "DXDescriptor.h"
#include "DescriptorHeap.h"
#include "DescriptorHeapManager.h"
#include "GPUResource.h"
#include "D3D12Framebuffer.h"
#include "Core\Performance\PerfManager.h"
#include "RHI\RHICommandList.h"
#include "RHI\RHIInterGPUStagingResource.h"
#include "D3D12InterGPUStagingResource.h"
#include "D3D12CommandList.h"


D3D12RHITexture::D3D12RHITexture()
{
}


D3D12RHITexture::~D3D12RHITexture()
{
}

void D3D12RHITexture::Release()
{
	EnqueueSafeRHIRelease(Resource);
}

DXDescriptor* D3D12RHITexture::GetDescriptor(const RHIViewDesc & desc, DescriptorHeap* heap)
{
	if (heap == nullptr)
	{
		heap = DContext->GetHeapManager()->GetMainHeap();
	}
	ensure(desc.ViewType != EViewType::Limit);
	DXDescriptor* Descriptor = heap->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
	WriteToDescriptor(Descriptor, desc);
	Descriptor->Recreate();
	return Descriptor;
}

void D3D12RHITexture::Create(const RHITextureDesc2& inputDesc, DeviceContext* inContext)
{
	Desc = inputDesc;
	Context = inContext;
	if (Context == nullptr)
	{
		Context = RHI::GetDefaultDevice();
	}
	DContext = D3D12RHI::DXConv(Context);

	ResourceDesc.Width = Desc.Width;
	ResourceDesc.Height = Desc.Height;
	ResourceDesc.DepthOrArraySize = Desc.Depth;
	ResourceDesc.Dimension = D3D12Helpers::ConvertToResourceDimension(Desc.Dimension);
	ResourceDesc.Format = D3D12Helpers::ConvertFormat(Desc.Format);
	ResourceDesc.MipLevels = Desc.MipCount;
	ResourceDesc.SampleDesc.Count = 1;
	if (Desc.IsRenderTarget)
	{
		ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	}
	if (Desc.IsDepthStencil)
	{
		ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	}
	if (Desc.AllowUnorderedAccess)
	{
		ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}
	if (Desc.AllowCrossGPU)
	{
		ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER;
	}
	D3D12_CLEAR_VALUE ClearValue = {};
	ClearValue.Format = D3D12Helpers::ConvertFormat(Desc.GetRenderformat());
	if (Desc.IsDepthStencil)
	{
		if (Desc.DepthRenderFormat != ETextureFormat::FORMAT_UNKNOWN)
		{
			ClearValue.Format = D3D12Helpers::ConvertFormat(Desc.DepthRenderFormat);
		}
		ClearValue.DepthStencil.Depth = Desc.DepthClearValue;
		ClearValue.DepthStencil.Stencil = 0;
	}
	else
	{
		ClearValue.Color[0] = Desc.clearcolour.r;
		ClearValue.Color[1] = Desc.clearcolour.g;
		ClearValue.Color[2] = Desc.clearcolour.b;
		ClearValue.Color[3] = Desc.clearcolour.a;
	}
	AllocDesc d;
	d.ResourceDesc = ResourceDesc;
	d.ClearValue = ClearValue;
	d.Name = Desc.Name;
	d.InitalState = D3D12Helpers::ConvertRHIState_Safe(Desc.InitalState, Desc.IsDepthStencil);
	if (Desc.InitalState == EResourceState::RenderTarget && Desc.IsDepthStencil)
	{
		d.InitalState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	}
	if (UseReservedResouce)
	{
		d.AllowPerTileResidency = true;
		DContext->GetMemoryManager()->CreateReservedResouce(d, &Resource);
	}
	else
	{
		DContext->GetMemoryManager()->AllocResource(d, &Resource);
	}
}

GPUResource * D3D12RHITexture::GetResource() const
{
	return Resource;
}

void D3D12RHITexture::WriteToDescriptor(DXDescriptor * Descriptor, const RHIViewDesc& desc)
{
	
	WriteToItemDesc(Descriptor->GetItemDesc(), desc);
	/*Descriptor->SetItemDesc(GetItemDesc(desc));*/
}

void D3D12RHITexture::CopyToStagingResource(RHIInterGPUStagingResource* Res, RHICommandList* List, const RHICopyRect & rect)
{
	List->StartTimer(EGPUCOPYTIMERS::MGPUCopy);
	ensure(List->GetDeviceIndex() == Context->GetDeviceIndex());
	D3D12InterGPUStagingResource* DXres = D3D12RHI::DXConv(Res);
	D3D12CommandList* list = D3D12RHI::DXConv(List);
	D3D12DeviceContext* CurrentDevice = D3D12RHI::DXConv(Context);
	PerfManager::StartTimer("CopyToDevice");
	// Copy the intermediate render target into the shared buffer using the
	// memory layout prescribed by the render target.
	ID3D12Device* Host = CurrentDevice->GetDevice();
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT renderTargetLayout;
	GPUResource* TargetResource = GetResource();

	TargetResource->SetResourceState(list, D3D12_RESOURCE_STATE_COPY_SOURCE);
	DXres->GetViewOnDevice(CurrentDevice->GetDeviceIndex())->SetResourceState(list, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST);
	DXGI_FORMAT readFormat = D3D12Helpers::ConvertFormat(Desc.Format);
	list->FlushBarriers();
	CD3DX12_RESOURCE_DESC renderTargetDesc = CD3DX12_RESOURCE_DESC::Tex2D(readFormat, Desc.Width, Desc.Height, Desc.Depth, 1, 1, 0, D3D12_RESOURCE_FLAG_NONE, D3D12_TEXTURE_LAYOUT_UNKNOWN);

	CD3DX12_BOX box(rect.Left, rect.Top, rect.Right, rect.Bottom);
	const int count = Desc.Depth;
	for (int i = 0; i < count; i++)
	{
		Host->GetCopyableFootprints(&renderTargetDesc, 0, 1, 0, &renderTargetLayout, nullptr, nullptr, nullptr);
		CD3DX12_TEXTURE_COPY_LOCATION dest(DXres->GetViewOnDevice(CurrentDevice->GetDeviceIndex())->GetResource(), renderTargetLayout);
		CD3DX12_TEXTURE_COPY_LOCATION src(TargetResource->GetResource(), i);
		list->GetCommandList()->CopyTextureRegion(&dest, rect.Left, rect.Top, 0, &src, &box);
	}
	GetResource()->SetResourceState(list, D3D12_RESOURCE_STATE_COMMON);
	list->FlushBarriers();
	PerfManager::EndTimer("CopyToDevice");
	//DidTransferLastFrame = true;
	List->EndTimer(EGPUCOPYTIMERS::MGPUCopy);
}

void D3D12RHITexture::CopyFromStagingResource(RHIInterGPUStagingResource* Res, RHICommandList* List, const RHICopyRect & rect)
{
	List->StartTimer(EGPUCOPYTIMERS::MGPUCopy);
	ensure(List->GetDeviceIndex() == Context->GetDeviceIndex());
	D3D12InterGPUStagingResource* DXres = D3D12RHI::DXConv(Res);
	D3D12CommandList* list = D3D12RHI::DXConv(List);
	PerfManager::StartTimer("MakeReadyOnTarget");
	D3D12DeviceContext* CurrentDevice = D3D12RHI::DXConv(Context);
	ID3D12Device* Host = CurrentDevice->GetDevice();
	// Copy the buffer in the shared heap into a texture that the secondary
	// adapter can sample from.	
	// Copy the shared buffer contents into the texture using the memory
	// layout prescribed by the texture.

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT textureLayout;
	GetResource()->SetResourceState(list, D3D12_RESOURCE_STATE_COPY_DEST);
	DXres->GetViewOnDevice(CurrentDevice->GetDeviceIndex())->SetResourceState(list, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_SOURCE);
	GPUResource* TargetResource = GetResource();
	const int count = Desc.Depth;
	DXGI_FORMAT readFormat = D3D12Helpers::ConvertFormat(Desc.Format);
	list->FlushBarriers();
	CD3DX12_RESOURCE_DESC renderTargetDesc = CD3DX12_RESOURCE_DESC::Tex2D(readFormat, Desc.Width, Desc.Height, Desc.Depth, 1, 1, 0, D3D12_RESOURCE_FLAG_NONE, D3D12_TEXTURE_LAYOUT_UNKNOWN);
	for (int i = 0; i < count; i++)
	{
		int offset = i;
		CD3DX12_TEXTURE_COPY_LOCATION dest(TargetResource->GetResource(), offset);
		Host->GetCopyableFootprints(&renderTargetDesc, offset, 1, 0, &textureLayout, nullptr, nullptr, nullptr);
		CD3DX12_TEXTURE_COPY_LOCATION src(DXres->GetViewOnDevice(CurrentDevice->GetDeviceIndex())->GetResource(), textureLayout);
		CD3DX12_BOX box(rect.Left, rect.Top, rect.Right, rect.Bottom);
		list->GetCommandList()->CopyTextureRegion(&dest, rect.Left, rect.Top, 0, &src, &box);
	}
	//int Pixelsize = (BufferDesc.SFR_FullWidth - (int)BufferDesc.ScissorRect.x)*m_height;
	//CrossGPUBytes = Pixelsize * (int)D3D12Helpers::GetBytesPerPixel(secondaryAdapterTexture.Format);
	GetResource()->SetResourceState(list, D3D12_RESOURCE_STATE_COMMON);
	list->FlushBarriers();
	PerfManager::EndTimer("MakeReadyOnTarget");
	//DidTransferLastFrame = true;
	List->EndTimer(EGPUCOPYTIMERS::MGPUCopy);
}

void D3D12RHITexture::CreateWithUpload(const TextureDescription & idesc, DeviceContext * iContext)
{
	UploadDesc = idesc;
	GPUResource* textureUploadHeap;

	RHITextureDesc2 ImageDesc;
	ImageDesc.Width = UploadDesc.Width;
	ImageDesc.Height = UploadDesc.Height;
	ImageDesc.Format = UploadDesc.Format;
	ImageDesc.Dimension = DIMENSION_TEXTURE2D;
	ImageDesc.InitalState = EResourceState::CopyDst;
	ImageDesc.Depth = idesc.Faces;
	ImageDesc.MipCount = idesc.MipLevels;
	ImageDesc.Name = idesc.Name;
	//ImageDesc.AllowUnorderedAccess = true;
	Create(ImageDesc, iContext);

	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(Resource->GetResource(), 0, idesc.MipLevels*idesc.Faces);

	AllocDesc D = AllocDesc();
	D.ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
	D.InitalState = D3D12_RESOURCE_STATE_GENERIC_READ;
	DContext->GetMemoryManager()->AllocUploadTemporary(D, &textureUploadHeap);
	D3D12Helpers::NameRHIObject(textureUploadHeap, this, "(UPLOAD)");

	std::vector<D3D12_SUBRESOURCE_DATA> SubResourceDesc;
	uint64_t offset = 0;
	for (int face = 0; face < UploadDesc.Faces; face++)
	{
		for (int mip = 0; mip < UploadDesc.MipLevels; mip++)
		{
			D3D12_SUBRESOURCE_DATA textureData = {};
			textureData.pData = (void*)(offset + (UINT64)UploadDesc.PtrToData);
			textureData.RowPitch = UploadDesc.MipExtents(mip).x*UploadDesc.BitDepth;
			//textureData.SlicePitch = textureData.RowPitch  * UploadDesc.MipExtents(mip).y;
			SubResourceDesc.push_back(textureData);
			offset += UploadDesc.Size(mip);
		}
	}
	GPUUploadRequest Request;
	Request.SubResourceDesc = SubResourceDesc;
	Request.UploadBuffer = textureUploadHeap;
	Request.Target = Resource;
	Request.DataPtr = UploadDesc.PtrToData;
	Request.DataPtrSize = uploadBufferSize;
	DContext->EnqueueUploadRequest(Request);

	Resource->SetName(L"Texture");
	textureUploadHeap->SetName(L"Upload");
}

void D3D12RHITexture::WriteToItemDesc(DescriptorItemDesc& ItemDesc, const RHIViewDesc & viewDesc) const
{
	if (viewDesc.ViewType == EViewType::SRV)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC d = {};
		d.Format = D3D12Helpers::ConvertFormat(Desc.Format);
		if (Desc.IsDepthStencil)
		{
			d.Format = D3D12Helpers::ConvertFormat(Desc.RenderFormat);
		}
		d.ViewDimension = D3D12Helpers::ConvertDimension(Desc.Dimension);
		if (Desc.Depth > 1 && d.ViewDimension != D3D12_SRV_DIMENSION_TEXTURE3D)
		{
			d.ViewDimension = D3D12Helpers::ConvertDimension(eTextureDimension::DIMENSION_TEXTURECUBE);
		}
		d.Texture2D.MipLevels = Desc.MipCount;
		d.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		d.TextureCube.MostDetailedMip = viewDesc.Mip;
		d.TextureCube.MipLevels = Math::Min(viewDesc.MipLevels, Desc.MipCount);
		if (viewDesc.Dimension != DIMENSION_UNKNOWN)
		{
			d.ViewDimension = D3D12Helpers::ConvertDimension(viewDesc.Dimension);
		}
		d.Texture2DArray.ArraySize = Desc.Depth;
		d.Texture2DArray.FirstArraySlice = viewDesc.ArraySlice;
		//Descriptor->CreateShaderResourceView(Resource->GetResource(), &d, desc.OffsetInDescriptor);
		ItemDesc.CreateShaderResourceView(Resource->GetResource(), &d, viewDesc.OffsetInDescriptor);
	}
	else
	{
		ensureMsgf(Desc.AllowUnorderedAccess, "Attempt to create a UAV on a framebuffer without AllowUnorderedAccess set");
		D3D12_UNORDERED_ACCESS_VIEW_DESC destTextureUAVDesc = {};
		destTextureUAVDesc.ViewDimension = D3D12Helpers::ConvertDimensionUAV(Desc.Dimension);
		destTextureUAVDesc.Format = D3D12Helpers::ConvertFormat(GetDescription().Format);
		if (!viewDesc.UseResourceFormat)
		{
			destTextureUAVDesc.Format = D3D12Helpers::ConvertFormat(viewDesc.Format);
		}
		destTextureUAVDesc.Texture2D.MipSlice = viewDesc.Mip;
		if (GetDescription().Depth > 1 && GetDescription().Dimension != DIMENSION_TEXTURE3D)
		{
			destTextureUAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
			destTextureUAVDesc.Texture2DArray.ArraySize = Desc.Depth;
			destTextureUAVDesc.Texture2DArray.FirstArraySlice = viewDesc.ArraySlice;
		}
		ItemDesc.CreateUnorderedAccessView(Resource->GetResource(), nullptr, &destTextureUAVDesc, viewDesc.OffsetInDescriptor);
	}
}

DescriptorItemDesc D3D12RHITexture::GetItemDesc(const RHIViewDesc & viewDesc) const
{
	DescriptorItemDesc ItemDesc;
	WriteToItemDesc(ItemDesc, viewDesc);
	return ItemDesc;
}

void D3D12RHITexture::SetState(RHICommandList* list, EResourceState::Type State, int SubResource)
{
	GetResource()->SetResourceState(D3D12RHI::DXConv(list), D3D12Helpers::ConvertRHIState_Safe(State,Desc.IsDepthStencil), EResourceTransitionMode::Direct, SubResource);
}


