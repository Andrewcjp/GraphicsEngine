
#include "Shader_Bloom.h"
IMPLEMENT_GLOBAL_SHADER(Shader_Bloom);
//Handles Threshold Pass for Bloom!
Shader_Bloom::Shader_Bloom(DeviceContext* device) :Shader(device)
{
	m_Shader->AttachAndCompileShaderFromFile("PostProcess\\BloomThreshold_CS", EShaderType::SHADER_COMPUTE);
}

Shader_Bloom::~Shader_Bloom()
{}

bool Shader_Bloom::IsComputeShader()
{
	return true;
}

std::vector<ShaderParameter> Shader_Bloom::GetShaderParameters()
{
	std::vector<ShaderParameter> Output;
	Output.push_back(ShaderParameter(ShaderParamType::SRV, 0, 0));
	Output.push_back(ShaderParameter(ShaderParamType::UAV, 1, 0));
	return Output;
}
IMPLEMENT_GLOBAL_SHADER(Shader_Bloom_Compost);
Shader_Bloom_Compost::Shader_Bloom_Compost(DeviceContext * device) :Shader(device)
{
	m_Shader->AttachAndCompileShaderFromFile("PostProcess\\BloomMergeCS", EShaderType::SHADER_COMPUTE);
}

Shader_Bloom_Compost::~Shader_Bloom_Compost()
{}

std::vector<ShaderParameter> Shader_Bloom_Compost::GetShaderParameters()
{
	std::vector<ShaderParameter> Output;
	Output.push_back(ShaderParameter(ShaderParamType::SRV, 0, 0));
	Output.push_back(ShaderParameter(ShaderParamType::UAV, 1, 0));
	return Output;
}