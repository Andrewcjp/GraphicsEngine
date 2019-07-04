#pragma once
#include "StorageNode.h"
#include "RenderGraph.h"
namespace EViewMode
{
	enum Type
	{
		PerView,//node must run per eye in VR
		MultiView,//node can take a VR framebuffer input
		DontCare,//Things like Shadows that Don't care.
		Limit
	};
};

namespace ENodeQueueType
{
	enum Type
	{
		Graphics,
		Compute,
		RT,
		Copy,
		Limit
	};
};

class NodeLink;
class SceneDataNode;
class RenderNode
{
public:
	RenderNode();
	virtual ~RenderNode();
	//Called to update the inputs for this node etc.
	void UpdateSettings();

	//Implemented By every node to run It should be able to be invoked N times without issue
	virtual void OnExecute() = 0;


	//This is called when a setting is updated
	virtual void RefreshNode() {};

	//sets the next node to execute after this one.
	void LinkToNode(RenderNode* NextNode);
	void ExecuteNode();
	//Data Nodes
	NodeLink* GetInput(int index);
	NodeLink* GetOutput(int index);
	NodeLink* GetRefrence(int index);
	uint GetNumInputs() const;
	uint GetNumOutput() const;
	uint GetNumRefrences() const;
	//Node Type
	//For VR Only
	EViewMode::Type GetViewMode() const;
	ENodeQueueType::Type GetNodeQueueType() const;
	RenderNode* GetNextNode()const;
	virtual std::string GetName()const;
	void ValidateNode(RenderGraph::ValidateArgs & args);
	void SetupNode();

	bool IsNodeDeferred() const;
	void SetNodeDeferredMode(bool val);
protected:
	//is this node configured for a deferred pipeline or a forward one
	//This is a special case as many nodes need the depth from the Gbuffer in deferred 
	//all other conditions should be handled with Node conditionals.
	bool IsNodeInDeferredMode = false;

	//helpers:
	FrameBuffer* GetFrameBufferFromInput(int index);
	Scene* GetSceneDataFromInput(int index);
	virtual void OnNodeSettingChange();

	//Creates all data need to run a node on X device should NOT call functions on external objects (excluding Render systems like the MeshPipline).
	virtual void OnSetupNode() {};
	virtual void OnValidateNode(RenderGraph::ValidateArgs & args);
	void AddInput(EStorageType::Type TargetType, std::string format, std::string InputName = std::string());
	void AddOutput(EStorageType::Type TargetType, std::string format, std::string InputName = std::string());
	void AddOutput(NodeLink* Input, std::string format, std::string InputName = std::string());
	void AddRefrence(EStorageType::Type TargetType, std::string format, std::string InputName);
	void PassNodeThough(int inputindex, std::string newformat = std::string(), int outputinput = -1);
	RenderNode* Next = nullptr;
	RenderNode* LastNode = nullptr;
	ENodeQueueType::Type NodeEngineType = ENodeQueueType::Graphics;
	EViewMode::Type ViewMode = EViewMode::DontCare;
	std::vector<NodeLink*> Inputs;
	std::vector<NodeLink*> Outputs;
	std::vector<NodeLink*> Refrences;
	DeviceContext* Context = nullptr;
};
