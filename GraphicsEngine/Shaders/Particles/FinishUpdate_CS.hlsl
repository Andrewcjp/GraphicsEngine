#include "Particle_Common.hlsl"
StructuredBuffer<uint>	 Indexs: register(t0);	// UAV
RWByteAddressBuffer  IndirectCommandBuffer: register(u1);
RWByteAddressBuffer	 Counter: register(u2);
[numthreads(1, 1, 1)]
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
	uint PostsimCount = Counter.Load(PARTICLECOUNTER_OFFSET_ALIVECOUNT_AFTERSIMULATION);
	if (DTid.x < PostsimCount)
	{
		const int Stride = 20;
		const int index = DTid.x;
		IndirectCommandBuffer.Store(Stride*index, Indexs[index]);
	}
}

