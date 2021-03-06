#include "DefaultShaderCommon.hlsl"
#include "../Lighting.hlsl"
#include "Shading/ReflectionsTrace.hlsl"
RaytracingAccelerationStructure gRtScene : register(t0);
RWTexture2D<float4> gOutput : register(u0);
Texture2D<float4> Normals : register(t5);
Texture2D<float4> Pos : register(t6);
Texture2D<float4> GBUFFER_BaseSpec : register(t66);
SamplerState g_sampler : register(s0);
SamplerState g_Clampsampler : register(s1);
TextureCube g_Shadow_texture2[MAX_POINT_SHADOWS] : register(t5, space2);
#include "../Shadow.hlsl"
float3 linearToSrgb(float3 c)
{
	// Based on http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
	float3 sq1 = sqrt(c);
	float3 sq2 = sqrt(sq1);
	float3 sq3 = sqrt(sq2);
	float3 srgb = 0.662002687 * sq1 + 0.684122060 * sq2 - 0.323583601 * sq3 - 0.0225411470 * c;
	return srgb;
}
cbuffer RTBufferData : register(b3)
{  
	float RT_RoughnessThreshold;
	float VX_MaxRoughness; 
	float VX_MinRoughness;
	float VX_RT_BlendStart;
	float VX_RT_BlendEnd;
	float VX_RT_BlendFactor;
	int Max_VXRayCount;
	int Max_RTRayCount;
	int Framecount;
};
cbuffer CameraData: register(b0)
{
	float4x4 viewI;
	float4x4 projectionI;
	float3 CameraPos;
}
cbuffer LightBuffer : register(b1)
{
	int LightCount;
};
cbuffer RayData: register(b2)
{
	uint RayFlags;
	uint RayMask;
};
StructuredBuffer<Light> LightList : register(t20);
float3 LaunchSample(float3 origin, uint seed, float SmoothNess, float3 normal, float3 V)
{
	RayDesc ray;
	ray.Origin = origin;

	float3 H = getGGXMicrofacet(seed, 1.0 - SmoothNess, normal);
	ray.Direction = normalize(2.f * dot(V, H) * H - V);

	ray.TMin = 1;
	ray.TMax = 1000;

	RayPayload payload;
	TraceRay(gRtScene, RayFlags /*rayFlags*/, RayMask, 0 /* ray index*/, 0, 0, ray, payload);
	if (!payload.Hit)
	{
		return float3(payload.color);
	}
	float3 OutColor = payload.color *GetAmbient_CONST();
	//float3 CalcColorFromLight(Light light, float3 Diffusecolor, float3 FragPos, float3 normal, float3 CamPos, float roughness, float Metalic)
	for (int i = 0; i < LightCount; i++)
	{
		float3 LightColour = CalcColorFromLight(LightList[i], payload.color, payload.Pos, payload.Normal, CameraPos, 0.0f, 0.0f);
		if (LightList[i].HasShadow && LightList[i].type == 1)
		{
			LightColour *= 1.0 - ShadowCalculationCube(payload.Pos.xyz, LightList[i], g_Shadow_texture2[LightList[i].ShadowID]);
		}
		OutColor += LightColour;
	}
	return OutColor;
}


[shader("raygeneration")]
void rayGen()
{
	uint3 launchIndex = DispatchRaysIndex();
	uint3 launchDim = DispatchRaysDimensions();

	float2 crd = float2(launchIndex.xy);
	float2 dims = float2(launchDim.xy);
	float2 NrmPos = crd / dims;
	float2 d = ((crd / dims) * 2.f - 1.f);
	float aspectRatio = dims.x / dims.y;
	const float SmoothNess = GBUFFER_BaseSpec.SampleLevel(g_sampler, NrmPos, 0).w;

	if (SmoothNess <= RT_RoughnessThreshold)
	{
		gOutput[launchIndex.xy] = float4(0, 0, 0, 0);
		return;
	}
	//const float Metalic = Pos.SampleLevel(g_sampler, NrmPos, 0).w;
	float3 pos = Pos.SampleLevel(g_sampler, NrmPos, 0).xyz;
	float3 ViewDir = normalize(pos - CameraPos);
	float3 normal = Normals.SampleLevel(g_sampler, NrmPos, 0).xyz;
	int SampleCount = Max_RTRayCount;
	if (SmoothNess >= 0.95f)
	{
		SampleCount = 1;
	}
	float3 AccumColour = float3(0, 0, 0);
	for (int i = 0; i < SampleCount; i++)
	{
		uint seed = initRand(launchIndex.x + launchIndex.y * launchDim.x, Framecount+i);
		AccumColour += LaunchSample(pos, seed, SmoothNess, normal, -ViewDir);
	}
	AccumColour /= SampleCount;
	gOutput[launchIndex.xy] = float4(AccumColour, SmoothNess);
}

