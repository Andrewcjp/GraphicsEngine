#include "Shader_Deferred.h"
#include "RHI/ShaderProgramBase.h"
#include "RHI/RHICommandList.h"
#include "RHI/RHI.h"
IMPLEMENT_GLOBAL_SHADER(Shader_Deferred);
Shader_Deferred::Shader_Deferred(class DeviceContext* dev) :Shader(dev)
{
	float g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,0.0f,
		1.0f, -1.0f, 0.0f,0.0f,
		-1.0f,  1.0f, 0.0f,0.0f,
		-1.0f,  1.0f, 0.0f,0.0f,
		1.0f, -1.0f, 0.0f,0.0f,
		1.0f,  1.0f, 0.0f,0.0f,
	};
	VertexBuffer = RHI::CreateRHIBuffer(ERHIBufferType::Vertex, dev);
	VertexBuffer->CreateVertexBuffer(sizeof(float) * 4, sizeof(float) * 6 * 4);
	VertexBuffer->UpdateVertexBuffer(&g_quad_vertex_buffer_data, sizeof(float) * 6 * 4);

	//Initialize shader
	m_Shader->ModifyCompileEnviroment(ShaderProgramBase::Shader_Define("MAX_POINT_SHADOWS", std::to_string(std::max(RHI::GetRenderConstants()->MAX_DYNAMIC_POINT_SHADOWS, 1))));
	m_Shader->ModifyCompileEnviroment(ShaderProgramBase::Shader_Define("MAX_DIR_SHADOWS", std::to_string(std::max(RHI::GetRenderConstants()->MAX_DYNAMIC_DIRECTIONAL_SHADOWS, 1))));
	m_Shader->ModifyCompileEnviroment(ShaderProgramBase::Shader_Define("MAX_LIGHTS", std::to_string(RHI::GetRenderConstants()->MAX_LIGHTS)));

	m_Shader->AttachAndCompileShaderFromFile("Deferred_LightingPass_vs", EShaderType::SHADER_VERTEX);
	m_Shader->AttachAndCompileShaderFromFile("Deferred_LightingPass_fs", EShaderType::SHADER_FRAGMENT);

}

Shader_Deferred::~Shader_Deferred()
{
	EnqueueSafeRHIRelease(VertexBuffer);
}

std::vector<ShaderParameter> Shader_Deferred::GetShaderParameters()
{
	std::vector<ShaderParameter> out;
	out.push_back(ShaderParameter(ShaderParamType::CBV, DeferredLightingShaderRSBinds::LightDataCBV, 1));
	out.push_back(ShaderParameter(ShaderParamType::CBV, DeferredLightingShaderRSBinds::MVCBV, 2));

	out.push_back(ShaderParameter(ShaderParamType::SRV, DeferredLightingShaderRSBinds::PosTex, 0));
	out.push_back(ShaderParameter(ShaderParamType::SRV, DeferredLightingShaderRSBinds::NormalTex, 1));
	out.push_back(ShaderParameter(ShaderParamType::SRV, DeferredLightingShaderRSBinds::AlbedoTex, 2));

	ShaderParameter parm = ShaderParameter(ShaderParamType::SRV, DeferredLightingShaderRSBinds::DirShadow, 4, 1);
	parm.NumDescriptors = RHI::GetRenderConstants()->MAX_DYNAMIC_DIRECTIONAL_SHADOWS;
	out.push_back(parm);
	parm = ShaderParameter(ShaderParamType::SRV, DeferredLightingShaderRSBinds::PointShadow, 5, 2);
	parm.NumDescriptors = RHI::GetRenderConstants()->MAX_DYNAMIC_POINT_SHADOWS;
	out.push_back(parm);


	out.push_back(ShaderParameter(ShaderParamType::SRV, DeferredLightingShaderRSBinds::DiffuseIr, 10));
	out.push_back(ShaderParameter(ShaderParamType::SRV, DeferredLightingShaderRSBinds::SpecBlurMap, 11));
	out.push_back(ShaderParameter(ShaderParamType::SRV, DeferredLightingShaderRSBinds::EnvBRDF, 12));
	out.push_back(ShaderParameter(ShaderParamType::SRV, DeferredLightingShaderRSBinds::PreSampleShadows, 13));
	out.push_back(ShaderParameter(ShaderParamType::SRV, DeferredLightingShaderRSBinds::ScreenSpecular, 14));
	out.push_back(ShaderParameter(ShaderParamType::SRV, DeferredLightingShaderRSBinds::LightDataBuffer, 20));
	return out;
}
std::vector<Shader::VertexElementDESC> Shader_Deferred::GetVertexFormat()
{
	std::vector<VertexElementDESC> out;
	out.push_back(VertexElementDESC{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0, INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	out[0].Stride = sizeof(glm::vec4);
	return out;
}

void Shader_Deferred::RenderScreenQuad(RHICommandList * list)
{
	//todo: less than full screen!
	list->SetVertexBuffer(VertexBuffer);
	list->DrawPrimitive(6, 1, 0, 0);

}