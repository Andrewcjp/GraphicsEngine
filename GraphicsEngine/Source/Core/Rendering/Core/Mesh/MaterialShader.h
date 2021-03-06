#pragma once
#include "Rendering/Core/Material.h"
#include "MaterialTypes.h"
#include <unordered_map>

class Shader_NodeGraph;
class Asset_Shader;
//class that a material creates to manage the node graph and any shader permutations 
class MaterialShader
{
public:
	MaterialShader(Asset_Shader* Shader);
	~MaterialShader();

	Shader_NodeGraph* GetOrComplie(MaterialShaderCompileData& data);
	void SetShader(MaterialShaderCompileData & data);
	Shader_NodeGraph * GetShader(EMaterialPassType::Type type);
	//

	ParmeterBindSet GetParamBinds();
	int GetParamterSize();
	
private:
	MaterialShaderCompileData CurrentData;
	Shader_NodeGraph* CurrentShader = nullptr;
	//holds all shaders created for this material with different option e.g. decal on, forward or deferred etc.
#if USEHASH
	std::unordered_map<int, Shader_NodeGraph*> ShaderPermutations;
#else
	std::map<std::string, Shader_NodeGraph*> ShaderPermutations;
#endif

};

