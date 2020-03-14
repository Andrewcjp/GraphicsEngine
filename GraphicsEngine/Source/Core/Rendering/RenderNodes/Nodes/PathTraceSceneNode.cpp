#include "PathTraceSceneNode.h"
#include "Core/BaseWindow.h"
#include "Rendering/Core/SceneRenderer.h"
#include "Rendering/RayTracing/RayTracingCommandList.h"
#include "Rendering/RayTracing/RayTracingEngine.h"
#include "Rendering/RayTracing/RHIStateObject.h"
#include "Rendering/RayTracing/ShaderBindingTable.h"
#include "Rendering/RenderNodes/StorageNodeFormats.h"
#include "../../RayTracing/Tables/PathTraceBindingTable.h"
#include "../../Core/Camera.h"
#include "RHI/RHICommandList.h"


PathTraceSceneNode::PathTraceSceneNode()
{
	OnNodeSettingChange();
	NodeEngineType = ECommandListType::Compute;
}


PathTraceSceneNode::~PathTraceSceneNode()
{}

void PathTraceSceneNode::OnExecute()
{
	FrameBuffer* Target = GetFrameBufferFromInput(0);
	StateObject->RebuildShaderTable();
	FLAT_COMPUTE_START(RTList->GetRHIList()->GetDevice());
	
	Data.IProj = glm::inverse(SceneRenderer::Get()->GetCurrentCamera()->GetProjection());
	Data.IView = glm::inverse(SceneRenderer::Get()->GetCurrentCamera()->GetView());
	Data.CamPos = SceneRenderer::Get()->GetCurrentCamera()->GetPosition();
	CBV->UpdateConstantBuffer(&Data, 0);

	RTList->ResetList();
	SetBeginStates(RTList->GetRHIList());
	RTList->GetRHIList()->StartTimer(EGPUTIMERS::RT_Trace);
	RTList->SetStateObject(StateObject);

	RTList->SetHighLevelAccelerationStructure(RayTracingEngine::Get()->GetHighLevelStructure());
	RTList->GetRHIList()->SetConstantBufferView(CBV, 0, 2);
	RTList->TraceRays(RHIRayDispatchDesc(Target));
	RTList->GetRHIList()->EndTimer(EGPUTIMERS::RT_Trace);
	SetEndStates(RTList->GetRHIList());
	RTList->Execute();

	FLAT_COMPUTE_END(RTList->GetRHIList()->GetDevice());
	PassNodeThough(0, StorageFormats::LitScene);
}


void PathTraceSceneNode::OnNodeSettingChange()
{
	AddResourceInput(EStorageType::Framebuffer,EResourceState::UAV, StorageFormats::DefaultFormat);
	AddOutput(EStorageType::Framebuffer, StorageFormats::LitScene);
}

void PathTraceSceneNode::OnSetupNode()
{
	RTList = RayTracingEngine::CreateRTList(RHI::GetDefaultDevice());
	DefaultTable = new PathTraceBindingTable();

	DefaultTable->InitTable();
	RHIStateObjectDesc Desc = {};
	Desc.AttibuteSize = sizeof(glm::vec2);// float2 barycentrics
	Desc.PayloadSize =  sizeof(glm::vec4) * 3;    // float4 pixelColor
	StateObject = RHI::GetRHIClass()->CreateStateObject(RHI::GetDefaultDevice(),Desc);
	StateObject->ShaderTable = DefaultTable;
	StateObject->Build();
	RayTracingEngine::Get()->AddHitTable(DefaultTable);
	CBV = RHI::CreateRHIBuffer(ERHIBufferType::Constant);
	CBV->CreateConstantBuffer(sizeof(Data), 1);
}
