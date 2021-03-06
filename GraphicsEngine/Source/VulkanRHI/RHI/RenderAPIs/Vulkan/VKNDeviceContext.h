#pragma once
#include "RHI/DeviceContext.h"
#include "VKNRHI.h"

class VKNDescriptorPool;
class VKNDeviceContext :public DeviceContext
{
public:
	VKNDeviceContext();
	~VKNDeviceContext();

	// Inherited via DeviceContext
	virtual void ResetDeviceAtEndOfFrame() override;
	virtual void SampleVideoMemoryInfo() override;
	virtual std::string GetMemoryReport() override;
	virtual void DestoryDevice() override;
	virtual void WaitForGpu() override;
	virtual void WaitForCopy() override;
	virtual void ResetSharingCopyList() override;
	virtual void NotifyWorkForCopyEngine() override;
	virtual void UpdateCopyEngine() override;
	virtual void ResetCopyEngine() override;
	virtual void GPUWaitForOtherGPU(DeviceContext * OtherGPU, DeviceContextQueue::Type WaitingQueue, DeviceContextQueue::Type SignalQueue) override;
	virtual void CPUWaitForAll() override;
	virtual void InsertGPUWait(DeviceContextQueue::Type WaitingQueue, DeviceContextQueue::Type SignalQueue) override;
	virtual RHITimeManager * GetTimeManager() override;
	bool isDeviceSuitable(VkPhysicalDevice device);
	void pickPhysicalDevice();
	void createLogicalDevice();
	void Init();

	static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;
	VKNDescriptorPool* pool = nullptr;
private:

	class VKNTimeManager* TimeManager = nullptr;
};

