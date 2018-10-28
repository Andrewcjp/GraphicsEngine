#pragma once
#include "TDTypes.h"
namespace TD
{
	class TDSimConfig
	{
	public:
		TD_API TDSimConfig();
		//!Time Taken to sleep a body once still
		float BodySleepTime = 1.0f;		
		//! Body values below this threshold will be zeroed off,Lower values will be more accurate but less performant in scenes with a large number of bodies
		float BodySleepZeroThreshold = 0.001f;
		//!Number of Iterations used to resolve collisions, higher is more accurate but require more CPU time
		int SolverIterationCount = 5;
		
		typedef void(*PrefCounterCallBack)(bool/*Is start of timer*/, TDPerfCounters::Type/*Timer ID*/);
		//! Callback Used for performance tracking in profile builds 
		PrefCounterCallBack PerfCounterCallBack;
	};
};
