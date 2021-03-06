#pragma once
#include "RHI/RHI.h"

class Shader;
struct ParticleSystem;
struct DeviceDependentObjects;

#define USE_INDIRECTCOMPUTE 1
#define USE_INDIRECTRENDER 1
#define PARTICLE_STATS 1
class ParticleSystemManager
{
public:
	ParticleSystemManager();
	~ParticleSystemManager();
	void InitCommon();

	void PreRenderUpdate(Camera * c);
	struct ParticleData
	{
		glm::vec4 Pos;
		glm::vec4 Vel;
		float lifetime;
		float Size;
	};
	struct IndirectArgs
	{
		int data;
		IndirectDrawArgs drawArguments;
	};
	struct Counters
	{
		unsigned int aliveCount = 0;
		unsigned int deadCount = 0;
		unsigned int realEmitCount = 0;
		unsigned int aliveCount_afterSimulation = 0;
	};
	struct DispatchArgs
	{
		IndirectDispatchArgs dispatchArgs;
	};
	static ParticleSystemManager* Get();
	static void ShutDown();
	void ShutDown_I();
	void Sync(ParticleSystem * system);
	void Simulate();
	void SimulateSystem(ParticleSystem* system);

	void StartSimulate();

	void StartRender();

	void RenderSystem(ParticleSystem* system, FrameBuffer * BufferTarget, EEye::Type Eye = EEye::Left);
	void Render(FrameBuffer* DDO, FrameBuffer * DepthTexture = nullptr, EEye::Type Eye = EEye::Left);
	void AddSystem(ParticleSystem* system);
	void RemoveSystem(ParticleSystem* system);
	RHICommandList* RenderList = nullptr;
private:
	ParticleSystem* Testsystem = nullptr;
	static ParticleSystemManager* Instance;
	RHICommandList* CmdList = nullptr;
	//rendering

	RHIBuffer* VertexBuffer = nullptr;
	//This is common across all particle systems
	RHIBuffer* ParticleRenderConstants = nullptr;
	struct ParticleConstData
	{
		glm::mat4x4 VPMat;
		glm::vec4 CameraRight_worldspace;
		glm::vec4 CameraUp_worldspace;
	};
	ParticleConstData RenderData[EEye::Limit];
	int emitcount = 0;
	std::vector<ParticleSystem*> ParticleSystems;
	void SubmitCompute();
	void SubmitRender(FrameBuffer* buffer);
	FrameBuffer* DepthBuffer = nullptr;
	RHICommandSignitureDescription RenderSig;
};

