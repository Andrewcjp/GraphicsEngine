#pragma once
#include "Rendering/Core/Camera.h"
#include "Core/GameObject.h"
#include "Rendering/Core/Light.h"
#include "Rendering\Shaders\Shader_Main.h"
#include <vector>
#include "Rendering/Core/FrameBuffer.h"

#include "RenderSettings.h"
class OGLWindow;
class Scene;
class RenderEngine
{
public:
	RenderEngine(int width, int height) {
		m_width = width;
		m_height = height;
	}
	virtual ~RenderEngine();
	void Render();
	void PreRender();
	virtual void OnRender() = 0;
	virtual void FinaliseRender() = 0;
	void Init();
	void ProcessScene();
	void PrepareData();
	virtual void PostInit() =0;
	virtual void Resize(int width, int height);
	virtual void DestoryRenderWindow() {};
	//called on play start and in editor when statics are changed
	void StaticUpdate();
	virtual void OnStaticUpdate() =0;
	virtual void SetRenderSettings(RenderSettings settings);
	virtual void SetScene(Scene* sc);
	virtual void SetEditorCamera(class Editor_Camera* cam);

	//getters
	Shader* GetMainShader();
	Camera* GetMainCam();
	int GetScaledWidth();
	int GetScaledHeight();
protected:
	class Shader_Convolution * Conv = nullptr;
	class Shader_EnvMap* envMap = nullptr;
	void ShadowPass();
	void PostProcessPass();
	int			m_width = 0;
	int			m_height = 0;
	RenderSettings CurrentRenderSettings = {};
	Scene* MainScene = nullptr; 
	Camera* MainCamera = nullptr;
	Editor_Camera* EditorCam = nullptr;
	Shader_Main* MainShader = nullptr;
	class ShadowRenderer* mShadowRenderer = nullptr;
	class PostProcessing* Post = nullptr;
	FrameBuffer* FilterBuffer = nullptr;
	class Shader_Skybox* SkyBox = nullptr;
	bool once = true;
};
