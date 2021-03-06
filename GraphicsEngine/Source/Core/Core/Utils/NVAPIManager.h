#pragma once
#ifdef PLATFORM_WINDOWS
//This might not be supported by every Compiler version
//detect if present as NVAPI is not distributable?
#if __has_include("nvapi.h")
#define NVAPI_PRESENT 1
#else 
#define NVAPI_PRESENT 0
#endif

#if NVAPI_PRESENT 
#include "d3d12.h"
#include "nvapi.h"
#endif
struct ID3D12Device;
class NVAPIManager
{ 
public:
	NVAPIManager();
	~NVAPIManager();
	void RenderGPUStats(int x, int y);
	void SampleClocks();
private:
	enum Stats
	{
		GPU0_GRAPHICS_PC,
		GPU0_GRAPHICS_CLOCK,
		Limit
	};
	bool IsOnline = false;
	int StaticProps = 2;
	int DynamicProps = 5+2;
	std::string SampleData;
	std::vector<std::vector<std::string>> GpuData;
#if NVAPI_PRESENT 
	NvU32 GPUCount = 0;
	NvPhysicalGpuHandle GPUHandles[NVAPI_MAX_PHYSICAL_GPUS];
	glm::vec3 Colours[NVAPI_MAX_PHYSICAL_GPUS] = {glm::vec3(1)};
#endif
	int StatIds[MAX_GPU_DEVICE_COUNT][Stats::Limit] = { 0 };
};
#else
#define NVAPI_PRESENT 0
#endif
