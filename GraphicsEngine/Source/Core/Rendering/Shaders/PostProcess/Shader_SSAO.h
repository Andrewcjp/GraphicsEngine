#pragma once
#include "RHI/Shader.h"
struct ShaderData
{
	glm::mat4x4 projection;
	glm::mat4x4 view;
	float radius = 0.5f;
	float bias = 0.05f;
	int kernelSize = 64;
	float pad = 0;
	glm::vec3 samples[64];
};

class Shader_SSAO :
	public Shader
{
public:
	DECLARE_GLOBAL_SHADER(Shader_SSAO);
	Shader_SSAO(DeviceContext* d);
	void Bind(RHICommandList * list);
	~Shader_SSAO();
	void Resize(int width, int height);
	RHIBuffer* DataBuffer;
	ShaderData CurrentData;
private:
	
	int quad_vertexbuffer;
	BaseTexture* noisetex;
	const int NoiseTextureUnit = 10;
	std::vector<glm::vec3> ssaoKernel;
	int mwidth = 0;
	int mheight = 0;


};
class Shader_SSAO_Merge :
	public Shader
{
public:
	DECLARE_GLOBAL_SHADER(Shader_SSAO_Merge);
	Shader_SSAO_Merge(DeviceContext* d);
};
