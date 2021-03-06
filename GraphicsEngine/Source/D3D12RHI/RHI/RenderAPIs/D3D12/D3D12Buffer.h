#pragma once
#include "D3D12RHI.h"
class D3D12Buffer : public RHIBuffer
{
public:
	D3D12Buffer(ERHIBufferType::Type type, DeviceContext* Device = nullptr);
	virtual ~D3D12Buffer();
	virtual void CreateConstantBuffer(int StructSize, int Elementcount, bool ReplicateToAllDevices = false) override;
	virtual void CreateVertexBuffer(int Stride, int ByteSize, EBufferAccessType::Type Accesstype = EBufferAccessType::Static) override;
	virtual void UpdateConstantBuffer(void * data, int offset) override;
	virtual void UpdateIndexBuffer(void* data, size_t length) override;
	virtual void UpdateBufferData(void * data, size_t length, EBufferResourceState::Type state) override;
	virtual void CreateIndexBuffer(int Stride, int ByteSize) override;
	virtual void CreateBuffer(RHIBufferDesc desc) override;
	virtual void UpdateVertexBuffer(void* data, size_t length, int VertexCount = -1) override;
	virtual void SetBufferState(class RHICommandList* list, EBufferResourceState::Type State) override;
	RHI_VIRTUAL void CopyToStagingResource(RHIInterGPUStagingResource* Res, RHICommandList* List) override;
	RHI_VIRTUAL void CopyFromStagingResource(RHIInterGPUStagingResource* Res, RHICommandList* List)override;
	bool CheckDevice(int index);
	void EnsureResouceInFinalState(D3D12CommandList* list);
	D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	void InitCBV(int StructSize, int Elementcount);
	void SetConstantBufferView(int offset, ID3D12GraphicsCommandList * list, int Slot, bool IsCompute, int Deviceindex);
	GPUResource* GetResource();
	DXDescriptor* GetDescriptor(const RHIViewDesc& desc, DescriptorHeap* heap = nullptr);

	virtual void* MapReadBack();
	

	RHI_VIRTUAL void SetResourceState(RHICommandList* list, EResourceState::Type State) override;

	void UpdateBufferDataGPU(void * data, size_t length, D3D12CommandList * list);
	void PushDataUpLoad(D3D12CommandList * list);
protected:
	void UpdateData(void * data, size_t length, D3D12_RESOURCE_STATES EndState);
	void Release() override;

	friend class D3D12RHIUAV;
private:
	GPUResource* GetDoubleBuffer();
	void MapBuffer(void** Data);
	void UnMap();
	void CreateStaticBuffer(int ByteSize);
	void CreateDynamicBuffer(int ByteSize);
	EBufferAccessType::Type BufferAccesstype;
	GPUResource * m_UploadBuffer = nullptr;
	GPUResource* m_DataBuffer = nullptr;
	GPUResource* m_DataBufferDouble[2] = {  };
	int ElementCount = 0;
	int ElementSize = 0;
	D3D12DeviceContext* Device = nullptr;
	D3D12_RESOURCE_STATES PostUploadState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
	UINT8* m_pCbvDataBegin[2];
};
