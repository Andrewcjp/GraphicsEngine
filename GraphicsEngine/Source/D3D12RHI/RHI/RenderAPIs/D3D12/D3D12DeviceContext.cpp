#include "Stdafx.h"
#include "D3D12DeviceContext.h"
#include "Core/Asserts.h"
#include "D3D12Helpers.h"
#include "D3D12TimeManager.h"
#include "D3D12CommandList.h"
#include "D3D12RHI.h"
#include "Core/Performance/PerfManager.h"
#if defined(_DEBUG)
#define DEVICE_NAME_OBJECT(x) NameObject(x,L#x, this->GetDeviceIndex())
void NameObject(ID3D12Object* pObject, std::wstring name, int id)
{
	name.append(L"_");
	name.append(std::to_wstring(id));
	NAME_D3D12_SetName(pObject, name.c_str());
}
#else
#define DEVICE_NAME_OBJECT(x);
#endif
D3D12DeviceContext::D3D12DeviceContext()
{}

D3D12DeviceContext::~D3D12DeviceContext()
{
	SafeRelease(m_commandQueue);
	
	for (int i = 0; i < RHI::CPUFrameCount; i++)
	{
		SafeRelease(m_commandAllocator[i]);
		SafeRelease(m_SharedCopyCommandAllocator[i]);
	}
	SafeRelease(m_CopyList);
	SafeRelease(m_CopyCommandAllocator);	
	delete TimeManager;
	SafeRHIRelease(GPUCopyList);
	SafeRHIRelease(InterGPUCopyList);
	SafeRelease(m_IntraCopyList);
	SafeRelease(m_SharedCopyCommandQueue);
	SafeRelease(m_ComputeCommandQueue);
	SafeRelease(m_CopyCommandQueue);
	SafeRelease(m_Device);
	SafeRelease(pDXGIAdapter);
	/*if (pDXGIAdapter != nullptr)
	{
		pDXGIAdapter->UnregisterVideoMemoryBudgetChangeNotification(m_BudgetNotificationCookie);
	}*/
}

void D3D12DeviceContext::CheckFeatures()
{
	D3D12_FEATURE_DATA_D3D12_OPTIONS3  FeatureData;
	ZeroMemory(&FeatureData, sizeof(FeatureData));
	HRESULT hr = m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS3, &FeatureData, sizeof(FeatureData));
	if (SUCCEEDED(hr))
	{
		Caps_Data.SupportsCopyTimeStamps = FeatureData.CopyQueueTimestampQueriesSupported;
	}
}

void D3D12DeviceContext::CreateDeviceFromAdaptor(IDXGIAdapter1 * adapter, int index)
{
	pDXGIAdapter = (IDXGIAdapter3*)adapter;
	HRESULT result = D3D12CreateDevice(
		pDXGIAdapter,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&m_Device)
	);
	ensureFatalMsgf(!(result == DXGI_ERROR_UNSUPPORTED), "D3D_FEATURE_LEVEL_11_0 is required to run this engine");
	ThrowIfFailed(result);

	D3D_FEATURE_LEVEL MaxLevel = D3D12RHI::GetMaxSupportedFeatureLevel(m_Device);
	if (MaxLevel != D3D_FEATURE_LEVEL_11_0)
	{
		m_Device->Release();
		ThrowIfFailed(D3D12CreateDevice(
			pDXGIAdapter,
			MaxLevel,
			IID_PPV_ARGS(&m_Device)
		));
	}
	DEVICE_NAME_OBJECT(m_Device);

	if (LogDeviceDebug)
	{
		std::stringstream ss;
		ss << "Device Created With Feature level " << D3D12Helpers::StringFromFeatureLevel(MaxLevel);
		Log::LogMessage(ss.str());
	}
	DeviceIndex = index;
	CheckFeatures();
#if 0
	pDXGIAdapter->RegisterVideoMemoryBudgetChangeNotificationEvent(m_VideoMemoryBudgetChange, &m_BudgetNotificationCookie);
#endif

	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
	m_commandQueue->SetName(L"Core Device Command Queue");
	for (int i = 0; i < RHI::CPUFrameCount; i++)
	{
		ThrowIfFailed(m_Device->CreateCommandAllocator(queueDesc.Type, IID_PPV_ARGS(&m_commandAllocator[i])));
		//m_commandAllocator[i]->SetName(L"Core Device Allocator");
		DEVICE_NAME_OBJECT(m_commandAllocator[i]);
	}
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	ThrowIfFailed(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_ComputeCommandQueue)));

	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
	ThrowIfFailed(m_Device->CreateCommandAllocator(queueDesc.Type, IID_PPV_ARGS(&m_CopyCommandAllocator)));
	ThrowIfFailed(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CopyCommandQueue)));
	ThrowIfFailed(m_Device->CreateCommandList(0, queueDesc.Type, m_CopyCommandAllocator, nullptr, IID_PPV_ARGS(&m_CopyList)));
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
	ThrowIfFailed(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_SharedCopyCommandQueue)));

	DEVICE_NAME_OBJECT(m_SharedCopyCommandQueue);
	DEVICE_NAME_OBJECT(m_ComputeCommandQueue);
	DEVICE_NAME_OBJECT(m_CopyCommandQueue);
	DEVICE_NAME_OBJECT(m_CopyCommandAllocator);
	for (int i = 0; i < RHI::CPUFrameCount; i++)
	{
		ThrowIfFailed(m_Device->CreateCommandAllocator(queueDesc.Type, IID_PPV_ARGS(&m_SharedCopyCommandAllocator[i])));
	}
	ThrowIfFailed(m_Device->CreateCommandList(0, queueDesc.Type, m_SharedCopyCommandAllocator[0], nullptr, IID_PPV_ARGS(&m_IntraCopyList)));
	m_SharedCopyCommandQueue->SetName(L"m_SharedCopyCommandQueue");
	m_IntraCopyList->Close();

	GraphicsQueueSync.Init(GetDevice());
	CopyQueueSync.Init(GetDevice());
	ComputeQueueSync.Init(GetDevice());
	GpuWaitSyncPoint.InitGPUOnly(GetDevice());
	D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
	ThrowIfFailed(GetDevice()->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, reinterpret_cast<void*>(&options), sizeof(options)));
	//todo: validate the device capablities 
	//GetDevice()->SetStablePowerState(false);

	TimeManager = new D3D12TimeManager(this);
	GPUCopyList = new D3D12CommandList(this, ECommandListType::Copy);
	InterGPUCopyList = new D3D12CommandList(this, ECommandListType::Copy);
	((D3D12CommandList*)GPUCopyList)->CreateCommandList();
	GPUCopyList->ResetList();
	GraphicsSync.Init(GetCommandQueueFromEnum(DeviceContextQueue::Graphics), GetDevice());
	CopySync.Init(GetCommandQueueFromEnum(DeviceContextQueue::Copy), GetDevice());
	InterGPUSync.Init(GetCommandQueueFromEnum(DeviceContextQueue::InterCopy), GetDevice());
	ComputeSync.Init(GetCommandQueueFromEnum(DeviceContextQueue::Compute), GetDevice());
}

void D3D12DeviceContext::LinkAdaptors(D3D12DeviceContext* other)
{
	CrossAdaptorSync.Init(GetDevice(), other->GetDevice());
}

ID3D12Device * D3D12DeviceContext::GetDevice()
{
	return m_Device;
}

ID3D12CommandAllocator * D3D12DeviceContext::GetCommandAllocator()
{
	return m_commandAllocator[CurrentFrameIndex];
}

ID3D12CommandAllocator * D3D12DeviceContext::GetComputeCommandAllocator()
{
	return nullptr;
}

ID3D12CommandAllocator * D3D12DeviceContext::GetCopyCommandAllocator()
{
	return nullptr;
}
ID3D12CommandAllocator * D3D12DeviceContext::GetSharedCommandAllocator()
{
	return m_SharedCopyCommandAllocator[CurrentFrameIndex];
}

ID3D12CommandQueue * D3D12DeviceContext::GetCommandQueue()
{
	return m_commandQueue;
}

void D3D12DeviceContext::MoveNextFrame(int SyncIndex)
{
	GraphicsSync.MoveNextFrame(SyncIndex);
	CopySync.MoveNextFrame(SyncIndex);
	InterGPUSync.MoveNextFrame(SyncIndex);
	ComputeSync.MoveNextFrame(SyncIndex);
	CurrentFrameIndex = SyncIndex;
}

void D3D12DeviceContext::ResetDeviceAtEndOfFrame()
{
	if (CurrentFrameIndex == 0)
	{
		GetTimeManager()->UpdateTimers();
	}
	GetCommandAllocator()->Reset();
	GetSharedCommandAllocator()->Reset();
	ResetCopyEngine();
	//compute work could run past the end of a frame?
}

void D3D12DeviceContext::SampleVideoMemoryInfo()
{
	pDXGIAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &CurrentVideoMemoryInfo);

	usedVRAM = CurrentVideoMemoryInfo.CurrentUsage / 1024 / 1024;
	totalVRAM = CurrentVideoMemoryInfo.Budget / 1024 / 1024;
}

std::string D3D12DeviceContext::GetMemoryReport()
{
	std::string output = "VMEM: ";
	output.append(std::to_string(usedVRAM));
	output.append("MB / ");
	output.append(std::to_string(totalVRAM));
	output.append("MB");
	return output;
}
void D3D12DeviceContext::MoveNextFrame()
{
	/*CurrentFrameIndex++;
	if (CurrentFrameIndex == RHI::CPUFrameCount - 1)
	{
		CurrentFrameIndex = 0;
	}*/
}
void D3D12DeviceContext::DestoryDevice()
{

}

void D3D12DeviceContext::WaitForGpu()
{
	GraphicsQueueSync.CreateSyncPoint(m_commandQueue);
}
void D3D12DeviceContext::WaitForCopy()
{
	CopyQueueSync.CreateSyncPoint(m_SharedCopyCommandQueue);
}
ID3D12GraphicsCommandList * D3D12DeviceContext::GetCopyList()
{
	return ((D3D12CommandList*)GPUCopyList)->GetCommandList();
}

ID3D12GraphicsCommandList * D3D12DeviceContext::GetSharedCopyList()
{
	return m_IntraCopyList;
}

void D3D12DeviceContext::ResetSharingCopyList()
{
	ThrowIfFailed(m_IntraCopyList->Reset(GetSharedCommandAllocator(), nullptr));
}

void D3D12DeviceContext::NotifyWorkForCopyEngine()
{
	CopyEngineHasWork = true;
}

void D3D12DeviceContext::UpdateCopyEngine()
{	
	if (true)
	{				
		//CopyEngineHasWork = false;
		GPUCopyList->Execute();
	}
	
}

void D3D12DeviceContext::ResetCopyEngine()
{
	if (/*CopyEngineHasWork*/true)
	{
		GPUCopyList->ResetList();
	}
}

void D3D12DeviceContext::ExecuteComputeCommandList(ID3D12GraphicsCommandList * list)
{
	ID3D12CommandList* ppCommandLists[] = { list };
	m_ComputeCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	if (RHI::BlockCommandlistExec())
	{
		ComputeQueueSync.CreateSyncPoint(m_ComputeCommandQueue);
	}
}

void D3D12DeviceContext::ExecuteCopyCommandList(ID3D12GraphicsCommandList * list)
{
	ID3D12CommandList* ppCommandLists[] = { list };
	m_CopyCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	if (RHI::BlockCommandlistExec())
	{
		CopyQueueSync.CreateSyncPoint(m_CopyCommandQueue);
	}
}

void D3D12DeviceContext::ExecuteInterGPUCopyCommandList(ID3D12GraphicsCommandList * list, bool forceblock)
{
	ID3D12CommandList* ppCommandLists[] = { list };
	m_SharedCopyCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	if (RHI::BlockCommandlistExec() || forceblock)
	{
		GraphicsQueueSync.CreateSyncPoint(m_SharedCopyCommandQueue);
	}

}
void D3D12DeviceContext::ExecuteCommandList(ID3D12GraphicsCommandList * list)
{
	ID3D12CommandList* ppCommandLists[] = { list };
	GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	if (RHI::BlockCommandlistExec())
	{
		WaitForGpu();
	}
}

int D3D12DeviceContext::GetDeviceIndex()
{
	return DeviceIndex;
}

RHITimeManager * D3D12DeviceContext::GetTimeManager()
{
	return TimeManager;
}

int D3D12DeviceContext::GetCpuFrameIndex()
{
	return CurrentFrameIndex;
}

void D3D12DeviceContext::GPUWaitForOtherGPU(DeviceContext * OtherGPU, DeviceContextQueue::Type WaitingQueue, DeviceContextQueue::Type SignalQueue)
{
	CrossAdaptorSync.CrossGPUCreateSyncPoint(GetCommandQueueFromEnum(SignalQueue), ((D3D12DeviceContext*)OtherGPU)->GetCommandQueueFromEnum(WaitingQueue));
}

void D3D12DeviceContext::CPUWaitForAll()
{
	GraphicsQueueSync.CreateSyncPoint(m_commandQueue);
	CopyQueueSync.CreateSyncPoint(m_SharedCopyCommandQueue);
	CopyQueueSync.CreateSyncPoint(m_CopyCommandQueue);
	ComputeQueueSync.CreateSyncPoint(m_ComputeCommandQueue);
}

ID3D12CommandQueue* D3D12DeviceContext::GetCommandQueueFromEnum(DeviceContextQueue::Type value)
{
	switch (value)
	{
	case DeviceContextQueue::Graphics:
		return m_commandQueue;
		break;
	case DeviceContextQueue::Compute:
		return m_ComputeCommandQueue;
		break;
	case DeviceContextQueue::Copy:
		return m_CopyCommandQueue;
		break;
	case DeviceContextQueue::InterCopy:
		return m_SharedCopyCommandQueue;
		break;
	}
	return nullptr;
}

void D3D12DeviceContext::InsertGPUWait(DeviceContextQueue::Type WaitingQueue, DeviceContextQueue::Type SignalQueue)
{
	SCOPE_CYCLE_COUNTER_GROUP("InsertGPUWait", "RHI");
	GpuWaitSyncPoint.GPUCreateSyncPoint(GetCommandQueueFromEnum(SignalQueue), GetCommandQueueFromEnum(WaitingQueue));
}

void D3D12DeviceContext::WaitForGPU(DeviceContextQueue::Type WaitingQueue, DeviceContextQueue::Type SignalQueue)
{

}

RHICommandList * D3D12DeviceContext::GetInterGPUCopyList()
{
	return InterGPUCopyList;
}

GPUSyncPoint::~GPUSyncPoint()
{
	SafeRelease(m_fence);
	SafeRelease(secondaryFence);
}

void GPUSyncPoint::Init(ID3D12Device * device, ID3D12Device* SecondDevice)
{
	//Fence types
	//  D3D12_FENCE_FLAG_NONE
	//  D3D12_FENCE_FLAG_SHARED
	//	D3D12_FENCE_FLAG_SHARED_CROSS_ADAPTER
	//	D3D12_FENCE_FLAG_NON_MONITORED
	ThrowIfFailed(device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_SHARED | D3D12_FENCE_FLAG_SHARED_CROSS_ADAPTER, IID_PPV_ARGS(&m_fence)));
	m_fenceEvent = CreateEvent(nullptr, false, false, nullptr);
	if (m_fenceEvent == nullptr)
	{
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
	}
	m_fenceValue++;
	// Create a shared handle to the cross adapter fence
	HANDLE fenceHandle = nullptr;
	device->CreateSharedHandle(
		m_fence,
		nullptr,
		GENERIC_ALL,
		nullptr,
		&fenceHandle);

	// Open shared handle to fence on secondaryDevice GPU
	SecondDevice->OpenSharedHandle(fenceHandle, IID_PPV_ARGS(&secondaryFence));

	CloseHandle(fenceHandle);
}
void GPUSyncPoint::InitGPUOnly(ID3D12Device * device)
{
	ThrowIfFailed(device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
	m_fenceValue++;
}
void GPUSyncPoint::Init(ID3D12Device * device)
{
	//Fence types
	//  D3D12_FENCE_FLAG_NONE
	//  D3D12_FENCE_FLAG_SHARED
	//	D3D12_FENCE_FLAG_SHARED_CROSS_ADAPTER
	//	D3D12_FENCE_FLAG_NON_MONITORED
	ThrowIfFailed(device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
	m_fenceEvent = CreateEvent(nullptr, false, false, nullptr);
	if (m_fenceEvent == nullptr)
	{
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
	}
	m_fenceValue++;
}

void GPUSyncPoint::CreateSyncPoint(ID3D12CommandQueue * queue)
{
	// Schedule a Signal command in the queue.
	ThrowIfFailed(queue->Signal(m_fence, m_fenceValue));

	// Wait until the fence has been processed.
	ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
	DWORD status = WaitForSingleObject(m_fenceEvent, INFINITE);
	if (status == WAIT_OBJECT_0)
	{
		// Increment the fence value for the current frame.
		m_fenceValue++;
		return;
	}
	ensure(status == WAIT_OBJECT_0);
}
void GPUSyncPoint::CrossGPUCreateSyncPoint(ID3D12CommandQueue * queue, ID3D12CommandQueue* otherDeviceQeue)
{
	// Schedule a Signal command in the queue.
	ThrowIfFailed(queue->Signal(m_fence, m_fenceValue));

	otherDeviceQeue->Wait(secondaryFence, m_fenceValue);
	m_fenceValue++;
}
void GPUSyncPoint::GPUCreateSyncPoint(ID3D12CommandQueue * queue, ID3D12CommandQueue * targetqueue)
{
	// Schedule a Signal command in the queue.
	ThrowIfFailed(queue->Signal(m_fence, m_fenceValue));

	targetqueue->Wait(m_fence, m_fenceValue);
	m_fenceValue++;
}

void GPUFenceSync::Init(ID3D12CommandQueue * TargetQueue, ID3D12Device* device)
{
	Queue = TargetQueue;
	ThrowIfFailed(device->CreateFence(m_fenceValues[m_frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
	m_fenceValues[m_frameIndex] = 1;
	// Create an event handle to use for frame synchronization
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
#if !BUILD_SHIPPING
	if (m_fenceEvent == nullptr)
	{
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
	}
#endif
#if _DEBUG
	m_fence->SetName(L"GPUFenceSync Fence");
#endif
}

void GPUFenceSync::MoveNextFrame(int SyncIndex)
{
	if (Queue == nullptr)
	{
		return;
	}
	// Schedule a Signal command in the queue.
	const UINT64 currentFenceValue = m_fenceValues[m_frameIndex];
	ThrowIfFailed(Queue->Signal(m_fence, currentFenceValue));
	// Update the frame index.
	m_frameIndex = SyncIndex;

	// If the next frame is not ready to be rendered yet, wait until it is ready.
	const UINT64 value = m_fence->GetCompletedValue();
	if (value < m_fenceValues[m_frameIndex])
	{
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
		WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
	}

	// Set the fence value for the next frame.
	m_fenceValues[m_frameIndex] = currentFenceValue + 1;
}
