
#include "AssetTypes.h"
#include "Core/Engine.h"
#include "Core/Utils/StringUtil.h"
AssetPathRef::AssetPathRef(std::string Filename)
{
	if (Filename.find("\\") == 0)
	{
		Filename.erase(0, 1);//trim leading whitespace
	}

	std::vector<std::string> split = StringUtils::Split(Filename, '.');
	if (split.size() == 2)
	{
		Name = split[0];
		Extention = split[1];
		FileType = ParseAssetFileType(Extention);
	}
	else
	{
		Name = Filename;
	}
	split = StringUtils::Split(Name, '\\');
	if (split.size() > 0)
	{
		BaseName = split[split.size() - 1];
	}
}
#include "Core//Assets/AssetManager.h"
const std::string AssetPathRef::GetFullPathToAsset() const
{
	if (IsDDC)
	{
		return AssetManager::GetRootDir() + DDCPath/* + Name + "." + Extention*/;
	}
	return AssetManager::GetContentPath() + Name + "." + Extention;
}

const std::string AssetPathRef::GetNoExtPathToAsset() const
{
	return AssetManager::GetContentPath() + Name;
}

const std::string AssetPathRef::GetRelativePathToAsset() const
{
	return Name + "." + Extention;
}

const std::string AssetPathRef::GetExtention() const
{
	return Extention;
}

const std::string AssetPathRef::GetBaseNameExtention()
{
	return BaseName + "." + Extention;
}

const AssetFileType::Type AssetPathRef::GetFileType() const
{
	return FileType;
}

AssetFileType::Type ParseAssetFileType(std::string Data)
{
	if (Data.find("dds") != -1 || Data.find("DDS") != -1)
	{
		return AssetFileType::DDS;
	}
	if (Data.find("jpg") != -1 || Data.find("png") != -1)
	{
		return AssetFileType::Image;
	}
	if (Data.find("obj") != -1 || Data.find("fbx") != -1)
	{
		return AssetFileType::Mesh;
	}
	return AssetFileType::Unknown;
}
