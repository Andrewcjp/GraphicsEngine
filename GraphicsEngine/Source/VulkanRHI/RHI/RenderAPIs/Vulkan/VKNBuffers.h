#pragma once
#include "RHI/RHICommandList.h"
#include "vulkan/vulkan_core.h"

class Descriptor;
#if BUILD_VULKAN
class VKNBuffer : public RHIBuffer
{
public:
	VKNBuffer(ERHIBufferType::Type type, DeviceContext* device) :RHIBuffer(type) {}
	virtual ~VKNBuffer() {};

	// Inherited via RHIBuffer
	virtual void CreateVertexBuffer(int Stride, int ByteSize, EBufferAccessType::Type Accesstype = EBufferAccessType::Static) override;
	virtual void CreateBuffer(RHIBufferDesc Desc) override;
	virtual void CreateIndexBuffer(int Stride, int ByteSize) override;
	virtual void CreateConstantBuffer(int StructSize, int Elementcount, bool ReplicateToAllDevices = false) override;
	virtual void UpdateConstantBuffer(void * data, int offset) override;
	virtual void UpdateVertexBuffer(void * data, size_t length) override;
	virtual void UpdateIndexBuffer(void * data, size_t length) override;
	virtual void BindBufferReadOnly(RHICommandList * list, int RSSlot) override;
	virtual void SetBufferState(RHICommandList * list, EBufferResourceState::Type State) override;
	virtual void UpdateBufferData(void * data, size_t length, EBufferResourceState::Type state) override;
	Descriptor GetDescriptor(int slot,int offset);
//private:
	VkBuffer vertexbuffer;
	VkDeviceMemory vertexBufferMemory;

};
#endif
