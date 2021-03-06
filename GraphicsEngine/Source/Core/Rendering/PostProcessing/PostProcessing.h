#pragma once

#include "PostProcessEffectBase.h"

struct DeviceDependentObjects;
class PP_FXAA;
class PostProcessing
{

public:
	static PostProcessing* Instance;
	static void StartUp();
	static void ShutDown();
	PostProcessing();
	~PostProcessing();
	void Update();
	std::vector<PostProcessEffectBase*> Effects;
	void AddEffect(PostProcessEffectBase* effect);
	void ExecPPStack(FrameBuffer* Target, RHICommandList* CommandList);
	void Init(FrameBuffer* Target);
	void Resize(FrameBuffer * Target);
	static PostProcessing* Get();
	//void AddCompostPass(FrameBuffer* buffer);
	class PP_ColourCorrect* ColourCorrect = nullptr;
	class PP_Blur* Blur = nullptr;
	class PP_Bloom* Bloom = nullptr;
	class PP_SSAO* SSAO = nullptr;
	class PP_Debug* Debug = nullptr;
	FrameBuffer* Gbuffer = nullptr;
private:
	PP_FXAA* FXAA = nullptr;
};
