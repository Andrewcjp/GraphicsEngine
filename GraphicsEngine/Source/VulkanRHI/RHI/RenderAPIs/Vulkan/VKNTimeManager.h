#pragma once
#include "RHI/RHITimeManager.h"
class VKNTimeManager:public RHITimeManager
{
public:
	VKNTimeManager();
	~VKNTimeManager();

	// Inherited via RHITimeManager
	virtual void UpdateTimers() override;

	void SetTimerName(int index, std::string Name);
	virtual void StartTotalGPUTimer(RHICommandList * ComandList) override;
	virtual void StartTimer(RHICommandList * ComandList, int index) override;
	virtual void EndTimer(RHICommandList * ComandList, int index) override;
	virtual void EndTotalGPUTimer(RHICommandList * ComandList) override;
	virtual float GetTotalTime() override;



	virtual void SetTimerName(int index, std::string Name, ECommandListType::Type type) override;

};

