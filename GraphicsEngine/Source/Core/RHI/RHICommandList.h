#pragma once

#include "RHITypes.h"
class DeviceContext;

namespace EBufferAccessType
{
	enum Type
	{
		Static,
		Dynamic,
		GPUOnly
	};
};
struct RHIBufferDesc
{
	int ElementCount = 0;
	int Stride = 0;
	int CounterSize = 0;
	EBufferAccessType::Type Accesstype;
	bool AllowUnorderedAccess = false;
	bool CreateSRV = false;
};
class RHI_API RHIBuffer : public IRHIResourse
{
public:
	enum BufferType
	{
		Vertex,
		Index,
		Constant,
		GPU
	};
	RHIBuffer::BufferType CurrentBufferType;
	RHIBuffer(RHIBuffer::BufferType type)
	{
		CurrentBufferType = type;
	}
	virtual void CreateVertexBuffer(int Stride, int ByteSize, EBufferAccessType::Type Accesstype = EBufferAccessType::Static) = 0;
	virtual void CreateBuffer(RHIBufferDesc Desc) = 0;
	virtual void CreateIndexBuffer(int Stride, int ByteSize) = 0;
	virtual void CreateConstantBuffer(int StructSize, int Elementcount, bool ReplicateToAllDevices = false) = 0;
	virtual void UpdateConstantBuffer(void * data, int offset) = 0;
	virtual void UpdateVertexBuffer(void* data, size_t length) = 0;
	virtual void UpdateIndexBuffer(void* data, size_t length) = 0;
	virtual void BindBufferReadOnly(class RHICommandList* list, int RSSlot) = 0;
	virtual void SetBufferState(class RHICommandList* list, EBufferResourceState::Type State) = 0;
	virtual void UpdateBufferData(void * data, size_t length, EBufferResourceState::Type state) = 0;
	virtual ~RHIBuffer() {}
	size_t GetVertexCount() { return VertexCount; }
	int GetCounterOffset()
	{
		return CounterOffset;
	}
protected:
	size_t VertexCount = 0;
	int CounterOffset = 0;
	int TotalByteSize = 0;
};

class RHIUAV : public IRHIResourse
{
public:
	RHIUAV()
	{}
	virtual ~RHIUAV() {};
	virtual void Bind(class RHICommandList* list, int slot) = 0;
	virtual void CreateUAVFromFrameBuffer(class FrameBuffer* target) = 0;
	virtual void CreateUAVFromTexture(class BaseTexture* target) = 0;
	virtual void CreateUAVFromRHIBuffer(class RHIBuffer* target) = 0;
};



class FrameBuffer;
class RHI_API RHICommandList : public IRHIResourse
{
public:
	RHICommandList(ECommandListType::Type type = ECommandListType::Graphics);
	virtual ~RHICommandList();
	virtual void ResetList() = 0;
	virtual void SetRenderTarget(FrameBuffer* target, int SubResourceIndex = 0) = 0;

	virtual void SetViewport(int MinX, int MinY, int MaxX, int MaxY, float MaxZ, float MinZ) = 0;
	virtual void Execute(DeviceContextQueue::Type Target = DeviceContextQueue::LIMIT) = 0;
	virtual void WaitForCompletion() = 0;
	//drawing
	virtual void DrawPrimitive(int VertexCountPerInstance, int InstanceCount, int StartVertexLocation, int StartInstanceLocation) = 0;
	virtual void DrawIndexedPrimitive(int IndexCountPerInstance, int InstanceCount, int StartIndexLocation, int BaseVertexLocation, int StartInstanceLocation) = 0;
	virtual void SetVertexBuffer(RHIBuffer* buffer) = 0;
	virtual void SetIndexBuffer(RHIBuffer* buffer) = 0;
	//If frame buffer is null the screen will be the render target!
	virtual void CreatePipelineState(class Shader* shader, class FrameBuffer* Buffer = nullptr) = 0;
	//uses the Cached PiplineStates
	virtual void SetPipelineStateObject(class Shader* shader, class FrameBuffer* Buffer = nullptr) = 0;

	virtual void SetPipelineState(PipeLineState state) = 0;
	//virtual void SetConstantBuffer(RHIBuffer* buffer) = 0;
	virtual void UpdateConstantBuffer(void * data, int offset) = 0;
	virtual void SetConstantBufferView(RHIBuffer * buffer, int offset, int Slot) = 0;
	virtual void SetTexture(class BaseTexture* texture, int slot) = 0;
	virtual void SetFrameBufferTexture(class FrameBuffer* buffer, int slot, int Resourceindex = 0) = 0;
	virtual void SetScreenBackBufferAsRT() = 0;
	virtual void ClearScreen() = 0;
	virtual void ClearFrameBuffer(FrameBuffer* buffer) = 0;
	//todo: complete this
	virtual void SetUAVParamter() {};
	virtual void UAVBarrier(RHIUAV* target) = 0;
	virtual void Dispatch(int ThreadGroupCountX, int ThreadGroupCountY, int ThreadGroupCountZ) = 0;
	//Indirect
	virtual void SetUpCommandSigniture(int commandSize, bool Dispatch) = 0;
	virtual void ExecuteIndiect(int MaxCommandCount, RHIBuffer* ArgumentBuffer, int ArgOffset, RHIBuffer* CountBuffer, int CountBufferOffset) = 0;
	virtual void SetRootConstant(int SignitureSlot, int ValueNum, void* Data, int DataOffset) = 0;
	DeviceContext* GetDevice();
	int GetDeviceIndex();
	void StartTimer(int TimerId);
	void EndTimer(int TimerId);
	void ResolveTimers();
	//MultiGPU
	virtual void CopyResourceToSharedMemory(FrameBuffer* Buffer) {};
	virtual void CopyResourceFromSharedMemory(FrameBuffer* Buffer) {};
	bool IsGraphicsList()const;
	bool IsCopyList() const;
	bool IsComputeList() const;

protected:
	DeviceContext * Device = nullptr;
	FrameBuffer * CurrentRenderTarget = nullptr;//todo: multiple!
	ECommandListType::Type ListType = ECommandListType::Graphics;
};

//Used to Bind Buffers or textures to a single Descriptor heap/set for shader arrays
class RHI_API RHITextureArray : public IRHIResourse
{
public:
	RHITextureArray(DeviceContext* device, int inNumEntries)
	{
		NumEntries = inNumEntries;
	};
	virtual ~RHITextureArray() {};
	virtual void AddFrameBufferBind(FrameBuffer* Buffer, int slot) = 0;
	virtual void BindToShader(RHICommandList* list, int slot) = 0;
	virtual void SetIndexNull(int TargetIndex) = 0;
protected:
	int NumEntries = 1;

};