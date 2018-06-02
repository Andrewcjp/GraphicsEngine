#pragma once
#include "RHI/BaseTexture.h"
class Material
{
public:
	struct MaterialProperties
	{
		bool UseMainShader = true;
		bool IsReflective = false;
		bool DoesShadow = true;
	};
	CORE_API Material(BaseTexture* Diff, MaterialProperties props = MaterialProperties());
	~Material();
	void SetMaterialActive(class RHICommandList * list);

	void SetShadow(bool state);
	bool GetDoesShadow();
	const MaterialProperties* GetProperties();
	void SetDisplacementMap(BaseTexture* tex);
	void SetNormalMap(BaseTexture * tex);
	void SetDiffusetexture(BaseTexture* tex);
	BaseTexture* GetDisplacementMap();
	BaseTexture * GetNormalMap();
	BaseTexture* GetDiffusetexture();


private:
	BaseTexture * Diffusetexture = nullptr;
	BaseTexture* Metallictex = nullptr;
	BaseTexture* glosstex = nullptr;
	BaseTexture* NormalMap = nullptr;
	BaseTexture* DisplacementMap = nullptr;
	MaterialProperties Properties;

	//bind to null
	BaseTexture* NullTexture2D = nullptr;
};

