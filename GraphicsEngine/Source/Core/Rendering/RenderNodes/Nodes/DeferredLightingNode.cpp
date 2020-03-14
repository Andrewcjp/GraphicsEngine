#include "DeferredLightingNode.h"
#include "Core/Assets/ShaderComplier.h"
#include "Rendering/Core/FrameBuffer.h"
#include "Rendering/Core/LightCulling/LightCullingEngine.h"
#include "Rendering/Core/ReflectionEnviroment.h"
#include "Rendering/Core/Screen.h"
#include "Rendering/RenderNodes/NodeLink.h"
#include "Rendering/RenderNodes/StorageNodeFormats.h"
#include "Rendering/RenderNodes/StoreNodes/FrameBufferStorageNode.h"
#include "Rendering/RenderNodes/StoreNodes/ShadowAtlasStorageNode.h"
#include "Rendering/Shaders/Shader_Deferred.h"
#include "Rendering/Shaders/Shader_Pair.h"
#include "Rendering/Shaders/Shader_Skybox.h"
#include "RHI/RHITimeManager.h"
#include "../../Core/VRXEngine.h"
#include "../../RayTracing/VoxelTracingEngine.h"
#include "../../Renderers/Terrain/TerrainRenderer.h"
#include "RHI/RHITexture.h"

DeferredLightingNode::DeferredLightingNode()
{
	ViewMode = EViewMode::PerView;
	UseScreenSpaceReflection = true;
	OnNodeSettingChange();
}

DeferredLightingNode::~DeferredLightingNode()
{
	SafeRHIRelease(List);
}

void DeferredLightingNode::OnSetupNode()
{
	List = RHI::CreateCommandList(ECommandListType::Graphics, Context);
}

void DeferredLightingNode::OnExecute()
{
	NodeLink* VRXImage = GetInputLinkByName("VRX Image");
	FrameBuffer* GBuffer = GetFrameBufferFromInput(0);
	FrameBuffer* MainBuffer = GetFrameBufferFromInput(1);
	Scene* MainScene = GetSceneDataFromInput(2);
	ensure(MainScene);
	DeferredShader = ShaderComplier::GetShader<Shader_Deferred, int>(Context, MainBuffer->GetDescription().VarRateSettings.BufferMode);
	List->ResetList();

	SetBeginStates(List);
	if (VRXImage != nullptr && VRXImage->IsValid())
	{
		List->SetVRXShadingRateImage(StorageNode::NodeCast<FrameBufferStorageNode>(VRXImage->GetStoreTarget())->GetFramebuffer()->GetRenderTexture());
		//List->PrepareFramebufferForVRR(List->GetShadingRateImage(), MainBuffer);
	}
	List->StartTimer(EGPUTIMERS::DeferredLighting);

	RHIPipeLineStateDesc desc = RHIPipeLineStateDesc();
	desc.InitOLD(false, false, false);
	desc.ShaderInUse = DeferredShader;
	desc.RenderTargetDesc = MainBuffer->GetPiplineRenderDesc();
	if (RHI::GetRenderSettings()->GetVRXSettings().UseVRR())
	{
		desc.DepthStencilState.StencilEnable = false;
		desc.DepthStencilState.BackFace.StencilFunc = COMPARISON_FUNC_EQUAL;
		desc.DepthStencilState.FrontFace.StencilFunc = COMPARISON_FUNC_EQUAL;
	}
	List->SetPipelineStateDesc(desc);
	if (VRXImage != nullptr && VRXImage->IsValid() )
	{
		RHITexture* Text = StorageNode::NodeCast<FrameBufferStorageNode>(VRXImage->GetStoreTarget())->GetFramebuffer()->GetRenderTexture();
		List->SetVRXShadingRateImage(Text);
		List->SetTexture2(Text, DeferredLightingShaderRSBinds::RateImage);
#if USEPS_VRR
		if (VRXEngine::Get()->TempTexture == nullptr)
		{
			RHITextureDesc2 Desc2 = MainBuffer->GetDescription().RenderTargets[0]->GetDescription();
			VRXEngine::Get()->TempTexture = RHI::GetRHIClass()->CreateTexture2();
			Desc2.AllowUnorderedAccess = true;
			VRXEngine::Get()->TempTexture->Create(Desc2);
		}
		VRXEngine::Get()->TempTexture->SetState(List, EResourceState::UAV);
		List->SetUAV(VRXEngine::Get()->TempTexture, DeferredLightingShaderRSBinds::TempVRRBuffer, RHIViewDesc::DefaultUAV());
#endif
	}
	RHIRenderPassDesc D = RHIRenderPassDesc(MainBuffer, RHI::GetRenderSettings()->GetVRXSettings().UseVRR() ? ERenderPassLoadOp::Load : ERenderPassLoadOp::Clear);
	List->BeginRenderPass(D);
	List->SetFrameBufferTexture(GBuffer, DeferredLightingShaderRSBinds::PosTex, 0);
	List->SetFrameBufferTexture(GBuffer, DeferredLightingShaderRSBinds::NormalTex, 1);
	List->SetFrameBufferTexture(GBuffer, DeferredLightingShaderRSBinds::AlbedoTex, 2);
	if (UseScreenSpaceReflection && GetInput(4)->IsValid())
	{
		FrameBuffer* ScreenSpaceData = GetFrameBufferFromInput(4);
		List->SetFrameBufferTexture(ScreenSpaceData, DeferredLightingShaderRSBinds::ScreenSpecular);
	}
	SceneRenderer::Get()->GetLightCullingEngine()->BindLightBuffer(List, true);
	SceneRenderer::Get()->GetReflectionEnviroment()->BindStaticSceneEnivoment(List, true);
	//SceneRenderer::Get()->GetReflectionEnviroment()->BindDynamicReflections(List, true);
	SceneRenderer::Get()->BindLightsBufferB(List, DeferredLightingShaderRSBinds::LightDataCBV);
	SceneRenderer::Get()->BindMvBufferB(List, DeferredLightingShaderRSBinds::MVCBV, GetEye());
	if (GetInput(3)->IsValid() && RHI::IsD3D12())
	{
		GetShadowDataFromInput(3)->BindPointArray(List, 6);	
	}
	SceneRenderer::DrawScreenQuad(List);
	List->EndRenderPass();


#if !TEST_VRR
	Shader_Skybox* SkyboxShader = ShaderComplier::GetShader<Shader_Skybox>();
	SkyboxShader->Render(SceneRenderer::Get(), List, MainBuffer, GBuffer);
#endif
	List->EndTimer(EGPUTIMERS::DeferredLighting);
#if USEPS_VRR
	if (VRXImage != nullptr)
	{
		RHITexture* Text = StorageNode::NodeCast<FrameBufferStorageNode>(VRXImage->GetStoreTarget())->GetFramebuffer()->GetRenderTexture();
		VRXEngine::ResolveVRRFramebuffer_PS(List, MainBuffer, Text);
	}
#endif-
	SetEndStates(List);
	List->Execute();
	GetInput(1)->GetStoreTarget()->DataFormat = StorageFormats::LitScene;

}

void DeferredLightingNode::OnNodeSettingChange()
{
	AddResourceInput(EStorageType::Framebuffer, EResourceState::PixelShader, StorageFormats::GBufferData, "GBuffer");
	AddResourceInput(EStorageType::Framebuffer, EResourceState::RenderTarget, StorageFormats::DefaultFormat, "Main buffer");
	AddInput(EStorageType::SceneData, StorageFormats::DefaultFormat, "Scene Data");
	AddInput(EStorageType::ShadowData, StorageFormats::ShadowData, "Shadow Maps");
	AddOutput(EStorageType::Framebuffer, StorageFormats::LitScene, "Lit scene");
	if (UseScreenSpaceReflection)
	{
		AddResourceInput(EStorageType::Framebuffer, EResourceState::PixelShader, StorageFormats::ScreenReflectionData, "SSR Data");
	}
	if (RHI::GetRenderSettings()->GetVRXSettings().UseVRX())
	{
		AddResourceInput(EStorageType::Framebuffer, EResourceState::PixelShader, StorageFormats::ShadingImage, "VRX Image");
	}
	AddResourceInput(EStorageType::Framebuffer, EResourceState::PixelShader, StorageFormats::PreSampleShadowData, "ShadowMask");
	GetOutput(0)->SetLink(GetInput(1));
}
