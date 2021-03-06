#pragma once
#include "RHI/ShaderBase.h"
#define CSO_VERSION 3
class ShaderProgramBase;
struct ShaderByteCodeBlob;
struct ShaderCompileItem;
class ShaderCache
{
public:
	ShaderCache();
	~ShaderCache();
	RHI_API static ShaderByteCodeBlob * GetShader(ShaderCompileItem * item);
	ShaderByteCodeBlob * IN_GetShader(ShaderCompileItem * item);
	void MirrorShaderToBuiltPlat(ShaderCompileItem * item, EPlatforms::Type platform);
	RHI_API static ShaderCache* Get();
	RHI_API static const std::string GetShaderInstanceHash(ShaderCompileItem* shader);
	RHI_API static const std::string GetShaderNamestr(const std::string & Shadername, const std::string & InstanceHash, EShaderType::Type type);
	void PrintShaderStats();
private:
	bool TryLoadCachedShader(const std::string & Name, ShaderCompileItem * Blob, const std::string & InstanceHash, EShaderType::Type type,EPlatforms::Type platform  = EPlatforms::Limit);
	void WriteBlobToFile(ShaderCompileItem* item, EPlatforms::Type platform = EPlatforms::Limit);
	void WriteDebugFile(ShaderCompileItem * item, EPlatforms::Type platform);
	static ShaderCache* Instance;
	struct CSOHeader
	{
		uint64 Version = CSO_VERSION;
		uint64 Size = 0;
	};
#if !BUILD_SHIPPING
	struct ShaderStats
	{
		int ShaderComplieCount = 0;
		int TotalShaderCount = 0;
		int ShaderLoadFromCacheCount = 0;
	};
	ShaderStats stats;
#endif
};

