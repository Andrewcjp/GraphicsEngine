#include "VRXEngine.h"
#include "RHI\Shader.h"
#include "FrameBuffer.h"
#include "..\Shaders\VRX\Shader_VRRResolve.h"


VRXEngine::VRXEngine()
{}


VRXEngine::~VRXEngine()
{}

VRXEngine * VRXEngine::Get()
{
	return nullptr;
}

void VRXEngine::ResolveVRRFramebuffer(RHICommandList* list, FrameBuffer* Target)
{
	//return;
	//#VRX: todo
	ensure(list->IsComputeList());
	RHIPipeLineStateDesc Desc = RHIPipeLineStateDesc::CreateDefault(ShaderComplier::GetShader<Shader_VRRResolve>());
	list->SetPipelineStateDesc(Desc);
	list->SetUAV(Target->GetUAV(), "DstTexture");
	list->SetFrameBufferTexture(Target, "SRCLevel1");
	list->Dispatch(Target->GetWidth() / 4, Target->GetHeight() / 4, 1);
	list->UAVBarrier(Target->GetUAV());
}

void VRXEngine::ResolveVRSFramebuffer(RHICommandList* list, FrameBuffer* Target)
{
	//#VRX: todo


}

void VRXEngine::SetVRSShadingRate(RHICommandList * List, VRS_SHADING_RATE::type Rate)
{
	//#VRX: todo
}

void VRXEngine::SetVRRShadingRate(RHICommandList * List, int FactorIndex)
{
	//#VRX: todo
	int slot = 13;// List->GetCurrnetPSO()->GetDesc().ShaderInUse->GetSlotForName("VRRData");
	List->SetSingleRootConstant(slot, FactorIndex);
}

void VRXEngine::SetVRXShadingRateImage(RHICommandList * List, FrameBuffer * Target)
{
	//#VRX: todo
}

void VRXEngine::SetupVRRShader(Shader * S)
{
#if TEST_VRR
	S->GetShaderProgram()->ModifyCompileEnviroment(ShaderProgramBase::Shader_Define("SUPPORT_VRR", "1"));
#endif
}

void VRXEngine::AddVRRToRS(std::vector<ShaderParameter>& S, int lastindex /*= 0*/)
{
#if !TEST_VRR
	return;
#endif
	ShaderParameter Sp = ShaderParameter(ShaderParamType::RootConstant, lastindex, 65);
	Sp.Name = "VRRData";
	S.push_back(Sp);
}
