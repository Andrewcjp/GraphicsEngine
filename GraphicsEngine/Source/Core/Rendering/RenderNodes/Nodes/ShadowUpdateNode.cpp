#include "ShadowUpdateNode.h"
#include "Core/Assets/Scene.h"
#include "Rendering/Core/SceneRenderer.h"
#include "Rendering/Core/ShadowRenderer.h"
#include "Rendering/RenderNodes/StorageNodeFormats.h"
#include "Rendering/RenderNodes/StoreNodes/ShadowAtlasStorageNode.h"
#include "../../RayTracing/Voxel/VoxelTracingEngine.h"
#include "../../RayTracing/Voxel/VoxelScene.h"

ShadowUpdateNode::ShadowUpdateNode()
{
	OnNodeSettingChange();
	//if (RHI::IsVulkan())
	//{
	//	SetNodeActive(false);
	//}
}


ShadowUpdateNode::~ShadowUpdateNode()
{}

void ShadowUpdateNode::OnExecute()
{
	//ShadowAtlasStorageNode* Node = GetShadowDataFromInput(0);
	//#Shadows: does this node need atlas
	ShadowList->ResetList();
	SetBeginStates(ShadowList);
	ShadowRenderer::RenderPointShadows(ShadowList);
	ShadowRenderer::RenderDirectionalShadows(ShadowList);
	VoxelTracingEngine::Get()->VoxliseTest(ShadowList);
	if (SceneRenderer::Get()->GetVoxelScene())
	{
		SceneRenderer::Get()->GetVoxelScene()->ProcessUpdatesToAcclerationStuctures(ShadowList);
	}
	SetEndStates(ShadowList);
	ShadowList->Execute();
}

void ShadowUpdateNode::OnNodeSettingChange()
{
	AddInput(EStorageType::ShadowData, StorageFormats::ShadowData);
}

void ShadowUpdateNode::OnSetupNode()
{
	ShadowList = RHI::CreateCommandList(ECommandListType::Graphics, Context);
}
