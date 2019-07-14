#include "Material.h"
#include "Core/Assets/Archive.h"
#include "Core/Assets/Asset_Shader.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Assets/ImageIO.h"
#include "Core/Assets/SerialHelpers.h"
#include "Defaults.h"
#include "../Shaders/Shader_NodeGraph.h"

void Material::UpdateShaderData()
{
	if (MaterialDataBuffer != nullptr)
	{
		MaterialDataBuffer->UpdateConstantBuffer(ParmbindSet.GetDataPtr());
	}
	ShaderInterface->SetShader(MaterialCData);
}

Material::Material(Asset_Shader * shader)
{
	ShaderInterface = new MaterialShader(shader);
	MaterialCData.Shader = shader;
	NeedsUpdate = true;
	Init();
	CurrentBindSet = ShaderInterface->GetBinds();
	ParmbindSet = ShaderInterface->GetParamBinds();
	ParmbindSet.AllocateMemeory();
	MaterialDataBuffer = RHI::CreateRHIBuffer(ERHIBufferType::Constant);
	MaterialDataBuffer->CreateConstantBuffer((int)ParmbindSet.GetSize(), 1);
}

Material::~Material()
{
	EnqueueSafeRHIRelease(MaterialDataBuffer);
	SafeDelete(CurrentBindSet);
}

void Material::SetMaterialActive(RHICommandList* RESTRICT list, const MeshPassRenderArgs& Pass)
{
	//SetReceiveShadow(Pass.UseShadows);
	if (NeedsUpdate)
	{
		ShaderInterface->SetShader(MaterialCData);
		NeedsUpdate = false;
	}
	Shader_NodeGraph* CurrentShader = nullptr;
	if (Pass.PassType == ERenderPass::BasePass_Cubemap || Pass.PassType == ERenderPass::TransparentPass)
	{
		CurrentShader = ShaderInterface->GetShader(EMaterialPassType::Forward);
	}
	else
	{
		CurrentShader = ShaderInterface->GetShader(Pass.UseDeferredShaders ? EMaterialPassType::Deferred : EMaterialPassType::Forward);
	}
	if (!CurrentShader->IsValid())//stack overflow!
	{
		Defaults::GetDefaultMaterial()->SetMaterialActive(list, Pass);
		return;
	}
	RHIPipeLineStateDesc desc;
	desc.DepthStencilState.DepthEnable = true;
	desc.DepthCompareFunction = COMPARISON_FUNC_LESS_EQUAL;
	desc.Cull = true;
	if (Pass.ReadDepth)
	{
		desc.DepthStencilState.DepthWrite = false;
	}
	if (GetRenderPassType() == EMaterialRenderType::Transparent)
	{
		desc.Blending = true;
		desc.Cull = false;
		desc.Mode = Full;
	}

	desc.ShaderInUse = (Shader*)CurrentShader;
	list->SetPipelineStateDesc(desc);
	list->SetConstantBufferView(MaterialDataBuffer, 0, MainShaderRSBinds::MaterialData);
	for (auto const& Pair : CurrentBindSet->BindMap)
	{
		if (Pair.second.TextureObj == nullptr)
		{
			list->SetTexture(ImageIO::GetDefaultTexture(), Pair.second.RootSigSlot);
		}
		else
		{
			list->SetTexture(Pair.second.TextureObj.Get(), Pair.second.RootSigSlot);
		}
	}
}

void Material::Init()
{
	//debug
	SetReceiveShadow(true);
	ShaderInterface->GetOrComplie(MaterialCData);
}

Shader* Material::GetShaderInstance(EMaterialPassType::Type pass)
{
	return ShaderInterface->GetShader(pass);
}

void Material::SetRenderType(EMaterialRenderType::Type t)
{
	NeedsUpdate = true;
	MaterialCData.MaterialRenderType = t;
}

void Material::UpdateBind(std::string Name, BaseTextureRef NewTex)
{
	if (CurrentBindSet->BindMap.find(Name) != CurrentBindSet->BindMap.end())
	{
		if (CurrentBindSet->BindMap.at(Name).TextureObj != NewTex)
		{
			CurrentBindSet->BindMap.at(Name).TextureObj = NewTex;
		}
	}
	else
	{
		LogEnsureMsgf(false, "Failed to Find Bind");
	}
}

BaseTexture * Material::GetTexturebind(std::string Name)
{
	if (CurrentBindSet->BindMap.find(Name) != CurrentBindSet->BindMap.end())
	{
		return CurrentBindSet->BindMap.at(Name).TextureObj.Get();
	}
	return nullptr;
}

void Material::SetDisplacementMap(BaseTexture * tex)
{
	if (tex != nullptr)
	{
		//UpdateBind("DisplacementMAP", tex);
	}
}

void Material::SetNormalMap(BaseTexture * tex)
{
	if (tex != nullptr)
	{
		UpdateBind("NORMALMAP", tex);
	}
}

void Material::SetDiffusetexture(BaseTextureRef tex)
{
	if (tex != nullptr)
	{
		UpdateBind("DiffuseMap", tex);
	}
}

bool Material::HasNormalMap()
{
	return GetTexturebind("NORMALMAP") != nullptr;
}

Material* Material::GetDefaultMaterial()
{
	return Defaults::GetDefaultMaterial();
}


void SerialTextureBind(Archive * A, TextureBindData* object)
{
	ArchiveProp(object->RegisterSlot);
	ArchiveProp(object->RootSigSlot);
	if (A->IsReading())
	{
		std::string Name;
		ArchiveProp_Alias(Name, object->TextureObj->TexturePath);
		object->TextureObj = AssetManager::DirectLoadTextureAsset(Name);
	}
	else
	{
		ArchiveProp(object->TextureObj->TexturePath);
	}
}

void Material::ProcessSerialArchive(Archive * A)
{
	//#Editor Save this
	//ArchiveProp(ShaderProperties.Metallic);
	//ArchiveProp(ShaderProperties.Roughness);
	if (A->IsReading())
	{
		std::string ShaderName = "";
		//ArchiveProp_Alias(ShaderName, Properties.ShaderInUse->GetName());
		//TEMP
		Asset_Shader* NewShader = nullptr;
		if (ShaderName == "Test")
		{
			NewShader = new Asset_Shader();
			NewShader->SetupTestMat();
		}
		else if (ShaderName == "Test2")
		{
			NewShader = new Asset_Shader();
			NewShader->SetupSingleColour();
		}
		else
		{
			//Properties.ShaderInUse = Material::GetDefaultMaterialShader();
		}
		if (NewShader != nullptr)
		{
			//NewShader->GetMaterialInstance(this);
			//CurrentBindSet->BindMap = Properties.TextureBinds;
		}
	}
	else
	{
		std::string tmp = "";
		/*if (Properties.ShaderInUse != nullptr)
		{
			tmp = Properties.ShaderInUse->GetName();
		}*/
		//ArchiveProp_Alias(tmp, Properties.ShaderInUse->GetName());
	}
	if (A->IsReading())
	{
		CurrentBindSet->BindMap.clear();
	}
	A->LinkPropertyMap<std::string, TextureBindData>(CurrentBindSet->BindMap, "CurrentBindSet->BindMap", &SerialTextureBind);

}

EMaterialRenderType::Type Material::GetRenderPassType()
{
	return MaterialCData.MaterialRenderType;
}

Shader * Material::GetDefaultMaterialShader()
{
	return (Shader*)GetDefaultMaterial()->GetShader();
}

Material * Material::CreateDefaultMaterialInstance()
{
	return new Material(new Asset_Shader(true));
}

Shader_NodeGraph * Material::GetShader()
{
	return ShaderInterface->GetShader(EMaterialPassType::Forward);
}

void Material::SetFloat(std::string name, float value)
{
	ParmbindSet.SetFloat(name, value);
	UpdateShaderData();
}

bool Material::IsComplied()
{
	return GetShader()->IsValid();
}

void Material::SetReceiveShadow(bool state)
{
	if (CurrnetShadowState == state)
	{
		return;
	}
	if (state)
	{
#if !NOSHADOW
		MaterialCData.ShaderKeyWords.push_back(ShadowShaderstring);
#endif
	}
	else
	{
		VectorUtils::Remove(MaterialCData.ShaderKeyWords, ShadowShaderstring);
	}
	NeedsUpdate = true;
	CurrnetShadowState = state;
	UpdateShaderData();
}

void Material::SetupDefaultBinding(TextureBindSet* TargetSet)
{
	TargetSet->BindMap.clear();
	TargetSet->BindMap.emplace("ALBEDOMAP", TextureBindData{ nullptr, ALBEDOMAP });
	TargetSet->BindMap.emplace("NORMALMAP", TextureBindData{ nullptr, NORMALMAP });
}

std::string Material::ShadowShaderstring = std::string("WITH_SHADOW");

