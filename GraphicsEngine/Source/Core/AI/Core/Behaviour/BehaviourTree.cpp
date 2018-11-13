
#include "BehaviourTree.h"
#include "BehaviourTreeNode.h"
#include "BTBlackboard.h"


BehaviourTree::BehaviourTree()
{
	Blackboard = new BTBlackboard();
	RootNode = new BehaviourTreeNode();
	RootNode->ParentTree = this;
}

BehaviourTree::~BehaviourTree()
{}

void BehaviourTree::RunTree(float dt)
{
	if (RootNode == nullptr)
	{
		return;
	}
	CoolDownTime -= dt;
	if (CoolDownTime > 0.0f)
	{
		return;
	}
	CoolDownTime = UpdateRate;
	//if (RunningNode != nullptr)
	//{
	//	EBTNodeReturn::Type res = RunningNode->HandleExecuteNode();
	//	if (res == EBTNodeReturn::Running)
	//	{
	//		return;
	//	}
	//	RunningNode = nullptr;
	//}
	RootNode->HandleExecuteNode();
}

void BehaviourTree::SetupTree()
{
	
}