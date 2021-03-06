
#include "VKNCommandlist.h"
#include "VKNBuffers.h"
#include "RHI\RHITypes.h"
#include "VKNRHI.h"
#include "VKNDeviceContext.h"
#include "VKNPipeLineStateObject.h"

#include "Core/Platform/PlatformCore.h"
#include "VKNDescriptorPool.h"
#include "VKNTexture.h"
#include "RHI\RHIRenderPassCache.h"
#include "VKNFramebuffer.h"
#include "VKNRenderPass.h"
VKNCommandlist::VKNCommandlist(ECommandListType::Type type, DeviceContext * context) :RHICommandList(type, context)
{
	Device = context;
	for (int i = 0; i < RHI::CPUFrameCount; i++)
	{
		Pools[i].Pool = VKNRHI::RHIinstance->createCommandPool();

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = Pools[i].Pool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(VKNRHI::GetVDefaultDevice()->device, &allocInfo, &Pools[i].Buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}
	ResetList();
}

VKNCommandlist::~VKNCommandlist()
{}

void VKNCommandlist::ResetList()
{
	CommandBuffer = Pools[VKNRHI::RHIinstance->currentFrame].Buffer;
	vkResetCommandPool(VKNRHI::GetVDefaultDevice()->device, Pools[VKNRHI::RHIinstance->currentFrame].Pool, 0);
	vkResetCommandBuffer(Pools[VKNRHI::RHIinstance->currentFrame].Buffer, 0);
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	if (vkBeginCommandBuffer(CommandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}
	IsOpen = true;
	Rootsig.Invalidate();
}

void VKNCommandlist::SetViewport(int MinX, int MinY, int MaxX, int MaxY, float MaxZ, float MinZ)
{

}

void VKNCommandlist::DrawPrimitive(int VertexCountPerInstance, int InstanceCount, int StartVertexLocation, int StartInstanceLocation)
{
	ensure(IsOpen);
	VKNRHI::VKConv(Device)->pool->AllocateAndBind(this);
	ensure(IsInRenderPass);
	vkCmdDraw(CommandBuffer, VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}

void VKNCommandlist::DrawIndexedPrimitive(int IndexCountPerInstance, int InstanceCount, int StartIndexLocation, int BaseVertexLocation, int StartInstanceLocation)
{
	ensure(IsOpen);
	VKNRHI::VKConv(Device)->pool->AllocateAndBind(this);
	ensure(IsInRenderPass);
	vkCmdDrawIndexed(CommandBuffer, IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}

void VKNCommandlist::SetVertexBuffer(RHIBuffer * buffer)
{
	ensure(IsOpen);
	VKNBuffer* vb = VKNRHI::VKConv(buffer);
	VkBuffer vertexBuffers[] = { vb->vertexbuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(CommandBuffer, 0, 1, vertexBuffers, offsets);
}

void VKNCommandlist::SetIndexBuffer(RHIBuffer * buffer)
{
	ensure(IsOpen);
	VKNBuffer* vb = VKNRHI::VKConv(buffer);
	vkCmdBindIndexBuffer(CommandBuffer, vb->vertexbuffer, 0, VK_INDEX_TYPE_UINT16);
}

void VKNCommandlist::SetConstantBufferView(RHIBuffer * buffer, int offset, int Register)
{
	ensure(IsOpen);
	Rootsig.SetConstantBufferView(Register, buffer, offset);
}

void VKNCommandlist::SetTexture(BaseTextureRef texture, int slot)
{
	ensure(IsOpen);
	Rootsig.SetTexture(slot, texture);
}

VkCommandBuffer* VKNCommandlist::GetCommandBuffer()
{
	return &Pools[VKNRHI::RHIinstance->currentFrame].Buffer;
}

void VKNCommandlist::ClearFrameBuffer(FrameBuffer * buffer)
{}


void VKNCommandlist::Dispatch(int ThreadGroupCountX, int ThreadGroupCountY, int ThreadGroupCountZ)
{
	ensure(IsOpen);
	VKNRHI::VKConv(Device)->pool->AllocateAndBind(this);
	vkCmdDispatch(CommandBuffer, ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}

void VKNCommandlist::Execute(DeviceContextQueue::Type Target /*= DeviceContextQueue::LIMIT*/)
{
	if (vkEndCommandBuffer(CommandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &CommandBuffer;

	submitInfo.signalSemaphoreCount = 0;


	if (vkQueueSubmit(VKNRHI::VKConv(Device)->graphicsQueue, 1, &submitInfo, nullptr) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}
	IsOpen = false;
}

void VKNCommandlist::BeginRenderPass(const RHIRenderPassDesc& RenderPassInfo)
{
	ensure(IsOpen);
	RHICommandList::BeginRenderPass(RenderPassInfo);
	CurrnetRenderPass = VKNRHI::RHIinstance->Pass;
	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	ensureMsgf(RenderPassInfo.DepthSourceBuffer == nullptr, "Vulkan does not support depth source buffer binding - Use shared depth stencil framebuffers");
	if (RenderPassInfo.TargetSwapChain)
	{
		renderPassInfo.renderPass = VKNRHI::VKConv(RHIRenderPassCache::Get()->GetOrCreatePass(RenderPassInfo))->RenderPass;
		renderPassInfo.framebuffer = VKNRHI::RHIinstance->swapChainFramebuffers[VKNRHI::RHIinstance->currentFrame];
	}
	else
	{
		VKNFramebuffer* FB = VKNRHI::VKConv(RenderPassInfo.TargetBuffer);
		FB->TryInitBuffer(RenderPassInfo, this);
		renderPassInfo.framebuffer = FB->Buffer;
		CurrnetRenderPass = VKNRHI::VKConv(RHIRenderPassCache::Get()->GetOrCreatePass(RenderPassInfo));
		renderPassInfo.renderPass = VKNRHI::VKConv(RHIRenderPassCache::Get()->GetOrCreatePass(RenderPassInfo))->RenderPass;
		//ensure(VKanRHI::VKConv(CurrentPso->GetDesc().RenderPass)->RenderPass == renderPassInfo.renderPass);
		if (CurrentPso != nullptr)
		{
			if (VKNRHI::VKConv(CurrentPso->GetDesc().RenderPass)->RenderPass != renderPassInfo.renderPass)
			{
				RHIPipeLineStateDesc NewDesc = CurrentPso->GetDesc();
				NewDesc.RenderPass = RHIRenderPassCache::Get()->GetOrCreatePass(RenderPassInfo);
				SetPipelineStateDesc(NewDesc);
				CurrnetRenderPass = VKNRHI::VKConv(NewDesc.RenderPass);
			}
		}
	}
	renderPassInfo.renderArea.offset = { 0, 0 };
	if (RenderPassInfo.TargetSwapChain)
	{
		renderPassInfo.renderArea.extent = VKNRHI::RHIinstance->swapChainExtent;
	}
	else
	{
		renderPassInfo.renderArea.extent.width = RenderPassInfo.TargetBuffer->GetWidth();
		renderPassInfo.renderArea.extent.height = RenderPassInfo.TargetBuffer->GetHeight();
	}

	const int Count = 4;
	VkClearValue clearColor[Count];
	for (int i = 0; i < Count; i++)
	{
		clearColor[i] = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearColor[i].depthStencil = { 1.0f,0 };
	}
	renderPassInfo.clearValueCount = Count;
	renderPassInfo.pClearValues = &clearColor[0];

	vkCmdBeginRenderPass(CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VKNCommandlist::EndRenderPass()
{
	RHICommandList::EndRenderPass();
	vkCmdEndRenderPass(CommandBuffer);
	if (CurrnetRenderPass != nullptr && CurrnetRenderPass->Desc.TargetBuffer != nullptr)
	{
		VKNRHI::VKConv(CurrnetRenderPass->Desc.TargetBuffer)->UpdateStateTrackingFromRP(CurrnetRenderPass->Desc);
	}
}

void VKNCommandlist::SetPipelineStateObject(RHIPipeLineStateObject* Object)
{
	VKNPipeLineStateObject* VObject = VKNRHI::VKConv(Object);
	Rootsig.SetRootSig(VObject->Parms);
	CurrentPso = VObject;
	CurrentPSO = CurrentPso;
	ensure(CommandBuffer != nullptr);
	vkCmdBindPipeline(CommandBuffer, IsComputeList() ? VK_PIPELINE_BIND_POINT_COMPUTE : VK_PIPELINE_BIND_POINT_GRAPHICS, VObject->Pipeline);
}

void VKNCommandlist::SetRootConstant(int SignitureSlot, int ValueNum, void * Data, int DataOffset)
{
	vkCmdPushConstants(CommandBuffer, CurrentPso->PipelineLayout, VK_SHADER_STAGE_ALL, DataOffset, sizeof(float)*ValueNum, Data);
}

void VKNCommandlist::SetFrameBufferTexture(FrameBuffer * buffer, int slot, int Resourceindex/* = 0*/)
{
	ensure(Resourceindex >= 0);
	VKNFramebuffer* V = VKNRHI::VKConv(buffer);
	V->WasTexture = true;
	Rootsig.SetFrameBufferTexture(slot, buffer, Resourceindex);
}


void VKNCommandlist::BindSRV(FrameBuffer* Buffer, int slot, RHIViewDesc Desc)
{
	throw std::logic_error("The method or operation is not implemented.");
}

VkPipelineBindPoint VKNCommandlist::GetBindPoint()
{
	if (IsComputeList())
	{
		return VK_PIPELINE_BIND_POINT_COMPUTE;
	}
	return VK_PIPELINE_BIND_POINT_GRAPHICS;
}

void VKNCommandlist::SetDepthBounds(float Min, float Max)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void VKNCommandlist::SetHighLevelAccelerationStructure(HighLevelAccelerationStructure* Struct)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void VKNCommandlist::TraceRays(const RHIRayDispatchDesc& desc)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void VKNCommandlist::SetStateObject(RHIStateObject* Object)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void VKNCommandlist::ExecuteIndiect(int MaxCommandCount, RHIBuffer * ArgumentBuffer, int ArgOffset, RHIBuffer * CountBuffer, int CountBufferOffset)
{}



void VKNCommandlist::SetPipelineStateDesc(const RHIPipeLineStateDesc& Desc)
{
	RHIPipeLineStateDesc TargetDesc = Desc;
	if (IsInRenderPass && CurrnetRenderPass != nullptr)
	{
		//if (Desc.FrameBufferTarget == CurrnetRenderPass->Desc.TargetBuffer)
		{
			TargetDesc.RenderPassDesc = CurrnetRenderPass->Desc;
			TargetDesc.RenderPass = CurrnetRenderPass;
		}
	}
	SetPipelineStateObject(Device->GetPSOCache()->GetFromCache(TargetDesc));
}

//void VknUAV::Bind(RHICommandList * list, int slot)
//{
//	VKNCommandlist* VKList = VKNRHI::VKConv(list);
//	VKList->Rootsig.SetUAV(slot, this);
//}



//void VknUAV::CreateUAVFromRHIBuffer(RHIBuffer * target)
//{
//	VKNBuffer* VBuffer = VKNRHI::VKConv(target);
//	TargetSize = VBuffer->GetSize();
//	TargetBuffer =  &VBuffer->vertexbuffer;
//}



void VkanTextureArray::AddFrameBufferBind(FrameBuffer * Buffer, int slot)
{
	if (slot == 0)
	{
		Tmp = Buffer;
	}
}

void VkanTextureArray::BindToShader(RHICommandList * list, int slot)
{
	if (Tmp != nullptr)
	{
		list->SetFrameBufferTexture(Tmp, slot);
	}
}

void VkanTextureArray::SetIndexNull(int TargetIndex, FrameBuffer* Buffer /*= nullptr*/)
{

}

void VkanTextureArray::Clear()
{

}

void VkanTextureArray::SetFrameBufferFormat(const RHIFrameBufferDesc & desc)
{
	//throw std::logic_error("The method or operation is not implemented.");
}



