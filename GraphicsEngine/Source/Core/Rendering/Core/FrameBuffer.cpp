#include "FrameBuffer.h"
#include "RHI/DeviceContext.h"
#include "Core/Performance/PerfManager.h"
FrameBuffer::FrameBuffer(DeviceContext * device, const RHIFrameBufferDesc & Desc)
{
	BufferDesc = Desc;
	const RHIFrameBufferDesc& T = Desc;
	BufferDesc = T;
	m_width = BufferDesc.Width;
	m_height = BufferDesc.Height;
}

FrameBuffer::~FrameBuffer()
{}

RHIFrameBufferDesc & FrameBuffer::GetDescription()
{
	return BufferDesc;
}

void FrameBuffer::Resize(int width, int height)
{}

void FrameBuffer::CopyHelper(FrameBuffer * Target, DeviceContext * TargetDevice)
{
	PerfManager::StartTimer("RunOnSecondDevice");
	DeviceContext* HostDevice = Target->GetDevice();
	if (TargetDevice == HostDevice)
	{
		return;
	}
	HostDevice->InsertGPUWait(DeviceContextQueue::Graphics, DeviceContextQueue::InterCopy);
	HostDevice->InsertGPUWait(DeviceContextQueue::InterCopy, DeviceContextQueue::Graphics);
	RHICommandList* CopyList = HostDevice->GetInterGPUCopyList();
	CopyList->ResetList();
	CopyList->StartTimer(EGPUCOPYTIMERS::MGPUCopy);
	CopyList->CopyResourceToSharedMemory(Target);
	CopyList->EndTimer(EGPUCOPYTIMERS::MGPUCopy);
	CopyList->ResolveTimers();
	CopyList->Execute(DeviceContextQueue::InterCopy);
	HostDevice->InsertGPUWait(DeviceContextQueue::Graphics, DeviceContextQueue::InterCopy);
	RHI::GetDeviceContext(1)->GPUWaitForOtherGPU(RHI::GetDeviceContext(0), DeviceContextQueue::InterCopy, DeviceContextQueue::InterCopy);

	//TargetDevice->InsertGPUWait(DeviceContextQueue::InterCopy, DeviceContextQueue::Graphics);
	CopyList = TargetDevice->GetInterGPUCopyList();
	CopyList->ResetList();

	CopyList->StartTimer(EGPUCOPYTIMERS::MGPUCopy);
	CopyList->CopyResourceFromSharedMemory(Target);
	CopyList->EndTimer(EGPUCOPYTIMERS::MGPUCopy);
	CopyList->ResolveTimers();
	CopyList->Execute(DeviceContextQueue::InterCopy);
	TargetDevice->InsertGPUWait(DeviceContextQueue::Graphics, DeviceContextQueue::InterCopy);
	PerfManager::EndTimer("RunOnSecondDevice");
}

void FrameBuffer::BindDepthWithColourPassthrough(RHICommandList * list, FrameBuffer * PassThrough)
{}

void FrameBuffer::CopyHelper_Async(FrameBuffer * Target, DeviceContext * TargetDevice)
{
	PerfManager::StartTimer("RunOnSecondDevice");
	DeviceContext* HostDevice = Target->GetDevice();
	if (TargetDevice == HostDevice)
	{
		return;
	}
	HostDevice->InsertGPUWait(DeviceContextQueue::Graphics, DeviceContextQueue::InterCopy);
	HostDevice->InsertGPUWait(DeviceContextQueue::InterCopy, DeviceContextQueue::Graphics);
	RHICommandList* CopyList = HostDevice->GetInterGPUCopyList();
	CopyList->ResetList();
	CopyList->StartTimer(EGPUCOPYTIMERS::MGPUCopy);
	CopyList->CopyResourceToSharedMemory(Target);
	CopyList->EndTimer(EGPUCOPYTIMERS::MGPUCopy);
	CopyList->ResolveTimers();
	CopyList->Execute(DeviceContextQueue::InterCopy);
	HostDevice->InsertGPUWait(DeviceContextQueue::Graphics, DeviceContextQueue::InterCopy);
	RHI::GetDeviceContext(1)->GPUWaitForOtherGPU(RHI::GetDeviceContext(0), DeviceContextQueue::InterCopy, DeviceContextQueue::InterCopy);

	//TargetDevice->InsertGPUWait(DeviceContextQueue::InterCopy, DeviceContextQueue::Graphics);
	CopyList = TargetDevice->GetInterGPUCopyList();
	CopyList->ResetList();
	CopyList->StartTimer(EGPUCOPYTIMERS::MGPUCopy);
	CopyList->CopyResourceFromSharedMemory(Target);
	CopyList->EndTimer(EGPUCOPYTIMERS::MGPUCopy);
	CopyList->ResolveTimers();
	CopyList->Execute(DeviceContextQueue::InterCopy);
	TargetDevice->InsertGPUWait(DeviceContextQueue::Graphics, DeviceContextQueue::InterCopy);
	PerfManager::EndTimer("RunOnSecondDevice");
}

void FrameBuffer::CopyHelper_Async_OneFrame(FrameBuffer * Target, DeviceContext * TargetDevice)
{
	//PerfManager::StartTimer("RunOnSecondDevice");
	DeviceContext* HostDevice = Target->GetDevice();
	if (TargetDevice == HostDevice)
	{
		return;
	}
	RHICommandList* CopyList = nullptr;
#if 1
	
	HostDevice->InsertGPUWait(DeviceContextQueue::Graphics, DeviceContextQueue::InterCopy);
	HostDevice->InsertGPUWait(DeviceContextQueue::InterCopy, DeviceContextQueue::Graphics);
	CopyList = HostDevice->GetInterGPUCopyList();
	CopyList->ResetList();
	CopyList->StartTimer(EGPUCOPYTIMERS::MGPUCopy);
	CopyList->CopyResourceToSharedMemory(Target);
	CopyList->EndTimer(EGPUCOPYTIMERS::MGPUCopy);
	CopyList->ResolveTimers();
	CopyList->Execute(DeviceContextQueue::InterCopy);
	HostDevice->InsertGPUWait(DeviceContextQueue::Graphics, DeviceContextQueue::InterCopy);
	RHI::GetDeviceContext(1)->GPUWaitForOtherGPU(RHI::GetDeviceContext(0), DeviceContextQueue::InterCopy, DeviceContextQueue::InterCopy);
#endif
	//TargetDevice->InsertGPUWait(DeviceContextQueue::InterCopy, DeviceContextQueue::Graphics);
	CopyList = TargetDevice->GetInterGPUCopyList();
	CopyList->ResetList();

	CopyList->StartTimer(EGPUCOPYTIMERS::MGPUCopy);
	CopyList->CopyResourceFromSharedMemory(Target);
	CopyList->EndTimer(EGPUCOPYTIMERS::MGPUCopy);
	CopyList->ResolveTimers();
	CopyList->Execute(DeviceContextQueue::InterCopy);
	TargetDevice->InsertGPUWait(DeviceContextQueue::Graphics, DeviceContextQueue::InterCopy);
	//PerfManager::EndTimer("RunOnSecondDevice");
}