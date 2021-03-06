#pragma once
#include "Core/Utils/MovingAverage.h"
#include "RHI/RHITimeManager.h"
#include "RHI/RHITypes.h"
#define AVGTIME 50
#define ENABLE_GPUTIMERS 1


class DeviceContext;
class D3D12DeviceContext;
class D3D12Query;
class D3D12CommandList;
class D3D12TimeManager : public RHITimeManager
{
public:

	D3D12TimeManager(DeviceContext* context);
	~D3D12TimeManager();
	void UpdateTimers() override;
	float ConvertTimeStampToMS(UINT64 Time);
	void SetTimerName(int index, std::string Name, ECommandListType::Type type = ECommandListType::Graphics) override;

	void StartTotalGPUTimer(RHICommandList * ComandList) override;
	void StartTimer(RHICommandList * ComandList, int index) override;
	void EndTimer(RHICommandList * ComandList, int index) override;
	void EndTotalGPUTimer(RHICommandList * ComandList) override;

	float GetTotalTime() override;
	void StartPipelineStatCapture(RHICommandList * commandlist) override;
	void EndPipelineStatCapture(RHICommandList * commandlist) override;
	void StartTimer(D3D12CommandList * ComandList, int index, bool IsCopy);
	void EndTimer(D3D12CommandList * ComandList, int index, bool IsCopy);

	void ResolveHeaps(RHICommandList* list) override;

private:

	float AVGgpuTimeMS = 0;
	

	int StatsGroupId = -1;
	bool TimerStarted = false;
#if GPUTIMERS_FULL
	const int MaxTimerCount = EGPUTIMERS::LIMIT+100;
	static const int TotalMaxTimerCount = EGPUTIMERS::LIMIT + EGPUCOPYTIMERS::LIMIT;
	const int MaxTimeStamps = MaxTimerCount * 2;
	const bool EnableCopyTimers = true;
#else 
	const int MaxTimerCount = 1;
	static const int TotalMaxTimerCount = 1;
	const int MaxTimeStamps = 2;
	const bool EnableCopyTimers = false;
#endif
	
	int MaxIndexInUse = 0;
	void Init(DeviceContext * context);

	void UpdateTimeStampFreq();


	UINT64 m_directCommandQueueTimestampFrequencies = 1;
	UINT64 m_ComputeQueueFreqency = 1;
	UINT64 m_copyCommandQueueTimestampFrequencies = 1;
	D3D12DeviceContext* Device = nullptr;
	//contains all timestamps for a timer
	struct TimerQ
	{
		std::string name = "";
		std::vector<D3D12Query*> TimerQueries;
		float TotalTime = 0.0f;
		void Resolve(UINT64 freqnecy);
	};
	UINT64 StartTimeStamp = 0;
	std::vector<TimerQ> TimerQueries;
	TimerQ* GetTimer(std::string name);
	void ResolveAllTimers();
	D3D12Query* PSQuery = nullptr;
};

