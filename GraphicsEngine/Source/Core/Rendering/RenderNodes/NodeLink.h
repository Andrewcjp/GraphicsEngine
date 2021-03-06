#pragma once
#include "StorageNode.h"
#include "RenderNode.h"

class RenderNode;
class StorageNode;

class NodeLink
{
public:
	NodeLink();
	NodeLink(EStorageType::Type TargetType, const std::string& format, const std::string& InputName = std::string(), RenderNode* Owner = nullptr);
	~NodeLink();
	bool SetStore(StorageNode* Target);
	bool IsValid();
	EStorageType::Type TargetType = EStorageType::Limit;
	RenderNode* OwnerNode = nullptr;
	StorageNode* GetStoreTarget() const;
	template<class T>
	T* GetTarget() const
	{
		return static_cast<T*>(GetStoreTarget());
	}
	//this is a GUIDE not a hard rule
	std::string DataFormat = "";
	EViewMode::Type ViewMode = EViewMode::DontCare;
	std::string GetLinkName() const;
	void SetLinkName(const std::string& val);
	void Validate(RenderGraph::ValidateArgs& args, RenderNode* parent);

	void PushWrongFormat(RenderNode* parent, RenderGraph::ValidateArgs &args, const std::string& badformat);
	EResourceState::Type ResourceState = EResourceState::Undefined;
	void SetOptional();
private:
	bool IsOptional = false;
	StorageNode* StoreTarget = nullptr;
	std::string LinkName = "";

};

