#include "ForwardRenderer.h"
#include "RHI/RHI.h"
#include "Core/Components/MeshRendererComponent.h"
#include "EngineGlobals.h"
#include "Rendering/PostProcessing/PostProcessing.h"
#include "Core/Engine.h"
#include "RHI/DeviceContext.h"

//todo:RHI
#include "RHI/RenderAPIs/D3D12/D3D12TimeManager.h"

#if USED3D12DebugP
#include "D3D12/D3D12Plane.h"
#endif
ForwardRenderer::ForwardRenderer(int width, int height) :RenderEngine(width, height)
{	
#if USED3D12DebugP
		debugplane = new D3D12Plane(1);
#endif
}

void ForwardRenderer::Resize(int width, int height)
{
	m_width = width;
	m_height = height;
	FilterBuffer->Resize(GetScaledWidth(), GetScaledHeight());	
	if (MainCamera != nullptr)
	{
		MainCamera->UpdateProjection((float)GetScaledWidth() / (float)GetScaledHeight());
	}
	RenderEngine::Resize(width, height);
}

ForwardRenderer::~ForwardRenderer()
{}

void ForwardRenderer::OnRender()
{
	ShadowPass();
	MainPass();
	RenderSkybox();
	PostProcessPass();	
}

#include "../Rendering/Shaders/Generation/Shader_Convolution.h"
#include "../Rendering/Shaders/Generation/Shader_EnvMap.h"
void ForwardRenderer::PostInit()
{
	MainShader = new Shader_Main();
	RHIFrameBufferDesc Desc = RHIFrameBufferDesc::CreateColourDepth(GetScaledWidth(), GetScaledHeight());
	Desc.AllowUnordedAccess = true;
	Desc.RTFormats[0] = eTEXTURE_FORMAT::FORMAT_R32G32B32A32_FLOAT;
	FilterBuffer = RHI::CreateFrameBuffer(RHI::GetDeviceContext(0), Desc);
	if (MainScene == nullptr)
	{
		MainScene = new Scene();
	}
	SkyBox = new Shader_Skybox();
	SkyBox->Init(FilterBuffer, nullptr);
	MainCommandList = RHI::CreateCommandList();
	//finally init the pipeline!
	MainCommandList->CreatePipelineState(MainShader,FilterBuffer);

#if DEBUG_CUBEMAPS
	SkyBox->test = Conv->CubeBuffer;
#endif
	
}


void ForwardRenderer::RenderDebugPlane()
{
#if (_DEBUG) && USED3D12DebugP
	MainList->SetGraphicsRootSignature(((D3D12Shader*)outshader->GetShaderProgram())->m_Shader.m_rootSignature);
	MainList->SetPipelineState(((D3D12Shader*)outshader->GetShaderProgram())->m_Shader.m_pipelineState);
	debugplane->Render(MainList);
#endif

}
void ForwardRenderer::MainPass()
{
	MainCommandList->ResetList();
	
	MainCommandList->GetDevice()->GetTimeManager()->StartTotalGPUTimer(MainCommandList);
	MainCommandList->GetDevice()->GetTimeManager()->StartTimer(MainCommandList, D3D12TimeManager::eGPUTIMERS::MainPass);
	MainCommandList->SetScreenBackBufferAsRT();
	MainCommandList->ClearScreen();	
	MainShader->UpdateMV(MainCamera);	
	MainShader->BindLightsBuffer(MainCommandList);

	if (mShadowRenderer != nullptr)
	{
		mShadowRenderer->BindShadowMapsToTextures(MainCommandList);
	}
	MainShader->UpdateMV(MainCamera);
	MainCommandList->SetRenderTarget(FilterBuffer);
	MainCommandList->ClearFrameBuffer(FilterBuffer);
	MainCommandList->SetFrameBufferTexture(Conv->CubeBuffer, 7);
	if (MainScene->GetLightingData()->SkyBox != nullptr)
	{
		MainCommandList->SetTexture(MainScene->GetLightingData()->SkyBox, 8);
	}
	MainCommandList->SetFrameBufferTexture(envMap->EnvBRDFBuffer, 9);
	for (size_t i = 0; i < (*MainScene->GetObjects()).size(); i++)
	{
		MainShader->SetActiveIndex(MainCommandList, (int)i);		
		(*MainScene->GetObjects())[i]->Render(false, MainCommandList);
	}	
	MainCommandList->SetRenderTarget(nullptr);	
	MainCommandList->GetDevice()->GetTimeManager()->EndTimer(MainCommandList, D3D12TimeManager::eGPUTIMERS::MainPass);
	MainCommandList->Execute();
}

void ForwardRenderer::RenderSkybox()
{
	SkyBox->Render(MainShader, FilterBuffer,nullptr);	
}

void ForwardRenderer::DestoryRenderWindow()
{
	delete MainCommandList;
}

void ForwardRenderer::FinaliseRender()
{

}

void ForwardRenderer::OnStaticUpdate()
{}

