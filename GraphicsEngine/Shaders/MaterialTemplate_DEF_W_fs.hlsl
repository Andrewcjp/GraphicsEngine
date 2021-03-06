Texture2D texColour : register(t0);
SamplerState defaultSampler: register (s0);
cbuffer GOConstantBuffer : register(b0)
{
	row_major matrix Model;
	int HasNormalMap;
};
#include "Core/Common.hlsl"
//BufferPoint
#define USE_FEEDBACK
#ifdef USE_FEEDBACK
#include "Core/SamplerFeedBack.hlsl"
FEEDBACKTEXTURE(SFS_TYPE_MINMIP) g_feedback : register(u0);
FEEDBACK_BIND;
Texture2D<float> MipClamp:register(t100);
#endif

struct PSInput
{
	float4 position : SV_POSITION;
	float4 Normal :NORMAL0;
	float2 uv : TEXCOORD;
	float4 WorldPos:TANGENT0;
	row_major float3x3 TBN:TANGENT1;
	INSTANCINGPSDATA
};

struct FS_OUTPUT
{
	float4 Gpos: SV_Target0;
	float4 GNormal: SV_Target1;
	float4 GAlbedoSpec: SV_Target2;
	float4 GTangent: SV_Target3;
};
//Declares
[earlydepthstencil]
FS_OUTPUT main(PSInput input)
{
	FS_OUTPUT output = (FS_OUTPUT)0;
	output.Gpos = input.WorldPos;
	float MetallicVar =GetMatData(Metallic);
	float RoughnessVar = GetMatData(Roughness);
	float3 texturecolour = float3(0, 0, 0);
	float3 Normal = input.Normal.xyz;
	//Insert Marker
	texturecolour = Diffuse;
	output.GNormal = float4(Normal.xyz, MetallicVar);
	output.GAlbedoSpec = float4(texturecolour, RoughnessVar);
	return output;
}
