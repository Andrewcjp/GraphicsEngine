#pragma once
#include <map>
#include <iomanip>
#include <time.h>

#include "Core/Utils/MovingAverage.h"
#include <unordered_map>

class BenchMarker;
class NVAPIManager;
#define SCOPE_CYCLE_COUNTER(name) PerfManager::ScopeCycleCounter PREPROCESSOR_JOIN(CYCLECOUNTER,__LINE__)(name);
#define SCOPE_CYCLE_COUNTER_GROUP(name,group) PerfManager::ScopeCycleCounter PREPROCESSOR_JOIN(CYCLECOUNTER,__LINE__)(name,group);
#define SCOPE_STARTUP_COUNTER(name) PerfManager::ScopeStartupCounter PREPROCESSOR_JOIN(STARTUPCOUNTER,__LINE__)(name);
#define DECLARE_TIMER_GROUP(name,GroupName) int name = PerfManager::Get()->GetGroupId(GroupName); 
#define DECLARE_TIMER(name,Group)int name =  PerfManager::Get()->AddTimer(#name, Group);
struct TimerData
{
	float Time = 0.0f;
	float MaxTime = 0.0f;
	MovingAverage* AVG = nullptr;
	std::string name = "";
	int GroupId = 0;
	std::vector<int> GroupIds;
	void AddToGroup(int id)
	{
		for (int i = 0; i < GroupIds.size(); i++)
		{
			if (GroupIds[i] == id)
			{
				return;
			}
		}
		GroupIds.push_back(id);
	}
	bool IsInGroup(int id)
	{
		for (int i = 0; i < GroupIds.size(); i++)
		{
			if (GroupIds[i] == id)
			{
				return true;
			}
		}
		return false;
	}
	bool Active = false;
	bool DirectUpdate = false;
	int CallCount = 0;
	int LastCallCount = 0;
	bool IsGPUTimer = false;
	float GPUStartOffset = 0.0f;//Offset in MS from start of frame (GPU main timestamp)
	ECommandListType::Type TimerType = ECommandListType::Graphics;
	bool HiddenFromDisplay = false;
	int LastFrameUsed = 0;
	bool IsCountTimer = false;
	MovingAverage* MAXAVG = nullptr;
	~TimerData()
	{
		SafeDelete(AVG);
		SafeDelete(MAXAVG);
	}
	TimerData(const TimerData& t)
	{
		*this = t;
		AVG = new MovingAverage(50);
		MAXAVG = new MovingAverage(50);
	}
	TimerData(std::string kname, int groupid)
	{
		name = kname;
	
		AddToGroup(groupid);
		AVG = new MovingAverage(50);
		MAXAVG = new MovingAverage(50);
	}

};
class PerfManager
{
public:
	struct RHI_API ScopeCycleCounter
	{
		ScopeCycleCounter(const char* Name);
		ScopeCycleCounter(const char* Name, const char* group);
		~ScopeCycleCounter();
	private:
		int StatId = -1;
		int GroupID = -1;
	};
	struct RHI_API ScopeStartupCounter
	{
		ScopeStartupCounter(const char* name);
		~ScopeStartupCounter();
	private:
		const char* Name = "";
	};
	CORE_API static unsigned long get_nanos();
	long GetSeconds();
	static PerfManager* Instance;
	CORE_API static PerfManager* Get();
	CORE_API static float EndAndLogTimer(std::string name);
	static void StartPerfManager();
	static void ShutdownPerfManager();
	PerfManager();
	CORE_API void Test();
	~PerfManager();
	CORE_API int AddTimer(const char * countername, const char * group);
	CORE_API int AddGPUTimer(const char * countername, int group);
	CORE_API int AddTimer(const char * countername, int groupId);
	CORE_API int AddTimer(int id, int groupid);

	CORE_API static void StartTimer(const char * countername);
	CORE_API static void EndTimer(const char * countername);
	static void StartTimer(int Counterid);
	static void EndTimer(int Counterid);
	static void RenderGpuData(int x, int y);
	float GetAVGFrameRate();
	float GetAVGFrameTime()const;
	std::string GetAllTimers();
	void StartCPUTimer();
	void EndCPUTimer();
	void StartFrameTimer();
	void EndFrameTimer();
	static float GetGPUTime();
	static float GetCPUTime();
	static float GetDeltaTime();
	static float GetDeltaTimeRaw();
	static void SetDeltaTime(float Time);
	static void NotifyEndOfFrame(bool Final = false);
	bool ShowAllStats = false;
	CORE_API TimerData* GetTimerData(int id);
	void DrawAllStats(int x, int y, bool IncludeGPUStats = false);

	void UpdateStats();
	void SampleSlowStats();
	void ClearStats();
	//Resets all AVGs
	void ResetStats();
	void DrawStatsGroup(int x, int & y, std::string GroupFilter, bool IncludeGPU,bool ParentOnly = false);
	CORE_API void UpdateStat(int id, float newtime, float GPUOffsetToMain, bool Direct = false);
	CORE_API int GetTimerIDByName(std::string name);
	CORE_API int GetGroupId(std::string name);

	CORE_API void StartSingleActionTimer(std::string Name);
	CORE_API float EndSingleActionTimer(std::string Name);
	void FlushSingleActionTimers();
	void LogSingleActionTimers();
	void LogSingleActionTimer(std::string name);
	CORE_API void FlushSingleActionTimer(std::string name,bool Log = false);
	void WriteLogStreams(bool UseRaw);
	std::vector<TimerData*> GetAllGPUTimers(std::string group);
	CORE_API static void StartBenchMark(std::string name = std::string());
	CORE_API static void EndBenchMark();
	CORE_API static void AddToCountTimer(std::string name, int amout);
private:
	void Internal_NotifyEndOfFrame();
	void InStartTimer(int targetTimer);
	void InEndTimer(int targetTimer);

	NVAPIManager* NVApiManager = nullptr;
	std::string GetTimerName(int id);
	std::unordered_map<std::string, unsigned long> SingleActionTimers;
	std::unordered_map<std::string, float> SingleActionTimersAccum;
	void UpdateStatsTimer();

	std::unordered_map<int, long> TimersStartStamps;
	std::unordered_map<int, long> TimersEndStamps;

	std::unordered_map<std::string, int> TimerIDs;
	std::unordered_map<std::string, int> GroupIDS;
	std::unordered_map<int, float> TimerOutput;
	std::unordered_map<int, TimerData> AVGTimers;
	int NextId = 0;
	int NextGroupId = 0;
	const float TimeMS = 1e6f;
	static bool PerfActive;
	float FrameTime = 1.0f;
	float DeltaTime = 0.0f;
	float ClampedDT = 0.0f;
	float CPUTime = 0;
	float GPUTime = 0;
	long FrameStart = 0;
	long CPUstart = 0;
	int CurrentFrame = 0;
	int FrameAccum = 0;
	float fpsnexttime = 0;
	float FpsPollSpeed = 0.25f;
	float CurrentAVGFps = 0.0f;
	float AVGFrameTime = 0;
	float FrameTimeAccum = 0.0;
	const bool LockStatsRate = true;
	const float StatsTickRate = (1.0f / 60.0f);
	const int AvgCount = 50;
	float StatAccum = 0;
	bool Capture = true;
	MovingAverage CPUAVG = MovingAverage(50);
	std::vector<const TimerData*> SortedTimers;
	//stats UI
	const float TextSize = 0.4f;
	const int Height = 20;
	const int ColWidth = 250;
	const float SlowStatsUpdateRate = (1.0f / 10.0f);
	float CurrentSlowStatsUpdate = 0.0f;

	BenchMarker* Bencher = nullptr;
	bool DidJustReset = false;
};

