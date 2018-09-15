#pragma once
#include "RHI/Shader.h"
class Shader_ColourCorrect : public Shader
{
public:
	Shader_ColourCorrect(class DeviceContext* dev);
	~Shader_ColourCorrect();
	std::vector<Shader::ShaderParameter> GetShaderParameters();
	std::vector<Shader::VertexElementDESC> GetVertexFormat();
};

