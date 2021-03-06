#pragma once
#include "BT_Knight.h"
#include "AI/Core/Behaviour/BehaviourTreeNode.h"
#include "AI/Core/Behaviour/BTBlackboard.h"
#include "AI/Core/Behaviour/BaseDecorator.h"
#include "AI/Core/Services/Service_PlayerCheck.h"
#include "AI/BleedOutBTNodes.h"


BT_Knight::BT_Knight()
{}


BT_Knight::~BT_Knight()
{}

void BT_Knight::SetupTree()
{
	BTValue* posptr = Blackboard->AddValue(EBTBBValueType::Vector);
	posptr->Vector = glm::vec3(1, 1, 1);
	BTValue* obj = Blackboard->AddValue(EBTBBValueType::Object);
	BTValue* Distance = Blackboard->AddValue(EBTBBValueType::Float);
	RootNode->ReturnOnFailure = false;
	BTSelectorNode* selector = RootNode->AddChildNode<BTSelectorNode>(new BTSelectorNode());
	selector->AddDecorator(new BaseDecorator(Distance, EDecoratorTestType::LessThanEqual, 5));
	selector->AddService(new Service_PlayerCheck(obj, Distance));
	selector->AddChildNode<BTMeleeAttackNode>(new BTMeleeAttackNode());
	selector->AddChildNode<BTWaitNode>(new BTWaitNode(3.0f));
	//sub node
	selector = RootNode->AddChildNode<BTSelectorNode>(new BTSelectorNode());
	selector->AddDecorator(new BaseDecorator(obj, EDecoratorTestType::NotNull));
	selector->AddService(new Service_PlayerCheck(obj));
	selector->AddChildNode<BTMoveToNode>(new BTMoveToNode(obj));
	//selector->AddChildNode<BTWaitNode>(new BTWaitNode(0.2f));
}
