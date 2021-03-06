#include "VoxelTracingEngine.h"
#include "../../Shaders/Shader_Pair.h"
#include "RHI/RHITexture.h"
#include "../../Core/Mesh/MeshBatch.h"
#include "../../Core/SceneRenderer.h"
#include "RHI/RHITimeManager.h"
#include "../../Core/Mesh/MeshPipelineController.h"
VoxelTracingEngine* VoxelTracingEngine::Instance = nullptr;

void VoxelTracingEngine::VoxliseTest(RHICommandList * list)
{
	//if (!RHI::GetRenderSettings()->GetVoxelSet().Enabled)
	{
		return;
	}
	VoxelMap->SetState(list, EResourceState::UAV);
	RHIPipeLineStateDesc desc = RHIPipeLineStateDesc::CreateDefault(voxeliseShader);
	desc.DepthStencilState.DepthEnable = false;
	desc.RasterizerState.ConservativeRaster = true;
	desc.RasterizerState.Cull = false;
	desc.Cull = false;
	list->SetPipelineStateDesc(desc);
	list->SetViewport(0, 0, size, size, 0, 1.0f);
	list->SetUAV(VoxelMap, "voxelTex");
	MeshPassRenderArgs Args;
	Args.PassType = ERenderPass::VoxelPass;
	Args.PassData = this;
	SceneRenderer::Get()->MeshController->RenderPass(Args, list);
	VoxelMap->SetState(list, EResourceState::Non_PixelShader);
	//list->UAVBarrier(VoxelMap);
	//uav barrier?
}

void VoxelTracingEngine::RenderVoxelDebug(RHICommandList* list, FrameBuffer* buffer, RHITexture* Texture,RHIBuffer* CBV)
{
	if (!RHI::GetRenderSettings()->GetVoxelSet().Enabled)
	{
		return;
	}
	DECALRE_SCOPEDGPUCOUNTER(list, "RenderVoxelDebug");
	Texture->SetState(list, EResourceState::ComputeUse);
	RHIPipeLineStateDesc desc = RHIPipeLineStateDesc::CreateDefault(DebugvoxeliseShader, buffer);
	desc.RasterizerState.Cull = false;
	desc.RasterMode = PRIMITIVE_TOPOLOGY_TYPE_POINT;
	desc.DepthStencilState.DepthEnable = true;
	desc.Cull = false;
	list->SetPipelineStateDesc(desc);
	list->BeginRenderPass(RHIRenderPassDesc(buffer, ERenderPassLoadOp::Load));
	RHIViewDesc d = RHIViewDesc::DefaultSRV();
	d.Dimension = DIMENSION_TEXTURE3D;
//	d.Mip = 4;
	list->SetTexture2(Texture, list->GetCurrnetPSO()->GetDesc().ShaderInUse->GetSlotForName("voxelTex"), d);
	SceneRenderer::Get()->BindMvBuffer(list);
	list->SetConstantBufferView(CBV, 0, "VoxelDataBuffer");
	
	list->DrawPrimitive(Texture->GetDescription().Width*Texture->GetDescription().Height*Texture->GetDescription().Depth, 1, 0, 0);
	list->EndRenderPass();
	Texture->SetState(list, EResourceState::Non_PixelShader);
}

VoxelTracingEngine::VoxelTracingEngine()
{
	if (!RHI::GetRenderSettings()->GetVoxelSet().Enabled)
	{
		return;
	}
	voxeliseShader = new Shader_Pair(RHI::GetDefaultDevice(), { "Voxel\\Voxelise_VS","Voxel\\Voxelise_GS","Voxel\\Voxelise_PS" },
		{ EShaderType::SHADER_VERTEX,EShaderType::SHADER_GEOMETRY,EShaderType::SHADER_FRAGMENT }, { ShaderProgramBase::Shader_Define("MAX_INSTANCES", std::to_string(RHI::GetRenderConstants()->MAX_MESH_INSTANCES)) });
	DebugvoxeliseShader = new Shader_Pair(RHI::GetDefaultDevice(), { "Voxel\\VoxelDebugOut_VS","Voxel\\VoxelDebugOut_GS","Voxel\\VoxelDebugOut_PS" },
		{ EShaderType::SHADER_VERTEX,EShaderType::SHADER_GEOMETRY,EShaderType::SHADER_FRAGMENT });
	VoxelMap = RHI::GetRHIClass()->CreateTexture2();
	RHITextureDesc2 Desc = {};
	size = 250;
	Desc.Width = size;
	Desc.Height = size;
	Desc.Depth = size;
	Desc.clearcolour = glm::vec4(0, 0, 0, 0);
	Desc.AllowUnorderedAccess = true;
	Desc.Dimension = DIMENSION_TEXTURE3D;
	Desc.Format = R32G32B32A32_UINT;
	Desc.InitalState = EResourceState::UAV;
	Desc.Name = "Voxel Struct";
	VoxelMap->Create(Desc);

	VxControlData = RHI::CreateRHIBuffer(ERHIBufferType::Vertex);
	VxControlData->CreateConstantBuffer(sizeof(VXData), 1, true);

	ControlData.VoxelGridCenter = glm::vec3(0, 0, 0);
	ControlData.VoxelSize = 1;
	ControlData.VoxelSize_INV = 1.0 / ControlData.VoxelSize;
	ControlData.VoxelRes = glm::ivec3(50, 50, 50);
	ControlData.Update();
	VxControlData->UpdateConstantBuffer(&ControlData);
}

VoxelTracingEngine* VoxelTracingEngine::Get()
{
	if (Instance == nullptr)
	{
		Instance = new VoxelTracingEngine();
	}
	return Instance;
}
