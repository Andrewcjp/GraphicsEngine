
#include "TestBTTree.h"
#include "AI/Core/Behaviour/BehaviourTreeNode.h"
#include "AI/Core/Behaviour/BTBlackboard.h"
#include "AI/Core/Behaviour/BaseDecorator.h"


TestBTTree::TestBTTree()
{}


TestBTTree::~TestBTTree()
{}
  
void TestBTTree::SetupTree()
{
	BTValue* posptr = Blackboard->AddValue(EBTBBValueType::Vector);
	posptr->Vector = glm::vec3(1, 1, 1);
	BTValue* obj = Blackboard->AddValue(EBTBBValueType::Object);
	obj->ObjectPtr = (void*)0x1;
	BTSelectorNode* selector = RootNode->AddChildNode<BTSelectorNode>(new BTSelectorNode());
	selector->Decorators.push_back(new BaseDecorator(obj, EDecoratorTestType::NotNull)); 
	selector->AddChildNode<BTMoveToNode>(new BTMoveToNode(posptr));
}