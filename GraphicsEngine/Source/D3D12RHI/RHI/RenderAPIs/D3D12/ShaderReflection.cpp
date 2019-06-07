#include "ShaderReflection.h"
#include <d3d12shader.h>
#include <d3dcompiler.h>
#include "Core\Utils\VectorUtils.h"
#define DXIL_FOURCC(ch0, ch1, ch2, ch3) (                            \
  (uint32_t)(uint8_t)(ch0)        | (uint32_t)(uint8_t)(ch1) << 8  | \
  (uint32_t)(uint8_t)(ch2) << 16  | (uint32_t)(uint8_t)(ch3) << 24   \
  )
void ShaderReflection::GatherRSBinds(IDxcBlob * target, std::vector<ShaderParameter> & shaderbinds, bool & iscompute)
{
	ID3D12ShaderReflection* REF = nullptr;
#if 0
	ThrowIfFailed(D3DReflect(target->GetBufferPointer(), target->GetBufferSize(), IID_PPV_ARGS(&REF)));
#else
	IDxcContainerReflection* pReflection;
	UINT32 shaderIdx;
	DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(&pReflection));
	ThrowIfFailed(pReflection->Load(target));
	ThrowIfFailed(pReflection->FindFirstPartKind(DXIL_FOURCC('D', 'X', 'I', 'L'), &shaderIdx));
	ensure(shaderIdx != 0);
	ThrowIfFailed(pReflection->GetPartReflection(shaderIdx, __uuidof(ID3D12ShaderReflection), (void**)&REF));

#endif
	if (REF == nullptr)
	{
		return;
	}
	D3D12_SHADER_DESC desc;
	REF->GetDesc(&desc);
	int ProgramTYpe = (desc.Version & 0xFFFF0000) >> 16;
	if (ProgramTYpe == D3D12_SHADER_VERSION_TYPE::D3D12_SHVER_COMPUTE_SHADER)
	{
		iscompute = true;
	}
	for (unsigned int i = 0; i < desc.BoundResources; i++)
	{
		D3D12_SHADER_INPUT_BIND_DESC TMPdesc;
		REF->GetResourceBindingDesc(i, &TMPdesc);
		//todo: restrict visibility and then detect duplicates and adjust visibility
		ShaderParameter p = ConvertParm(TMPdesc);
		if (p.Type != ShaderParamType::Limit)
		{
			VectorUtils::AddUnique(shaderbinds, p);
		}
	}

	//__debugbreak();
}


ShaderParameter ShaderReflection::ConvertParm(D3D12_SHADER_INPUT_BIND_DESC& desc)
{
	ShaderParameter S = {};
	S.NumDescriptors = desc.BindCount;
	S.RegisterSlot = desc.BindPoint;
	S.Name = desc.Name;
	switch (desc.Type)
	{
	case D3D_SIT_CBUFFER:
		S.Type = ShaderParamType::CBV;
		break;
	case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_CONSUME_STRUCTURED:
	case D3D_SHADER_INPUT_TYPE::D3D11_SIT_STRUCTURED:
	case D3D_SHADER_INPUT_TYPE::D3D11_SIT_BYTEADDRESS:
	case D3D_SIT_TEXTURE:
		S.Type = ShaderParamType::SRV;
		break;

	case D3D_SIT_UAV_RWSTRUCTURED:
	case D3D11_SIT_UAV_RWTYPED:
	case D3D_SIT_UAV_RWBYTEADDRESS:
		S.Type = ShaderParamType::UAV;
		break;
	case D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER:
		return ShaderParameter();
		break;
	default:
		__debugbreak();
		break;
	}

	return S;
}