#pragma once
#include "AI/Core/Behaviour/BehaviourTreeNode.h"
/*!  \addtogroup Game_AI
* @{ */
class MeleeWeapon;
class B_AIBase;
struct BTValue;

class BTMeleeAttackNode :public BehaviourTreeNode
{
public:
	BTMeleeAttackNode();

	virtual void OnAddedToTree() override;
	virtual std::string GetDebugName() override
	{
		return "BTMeleeAttackNode";
	}
protected:
	EBTNodeReturn::Type ExecuteNode() override;//todo: fix!
private:
	MeleeWeapon* weapon = nullptr;
};

class BTRifleAttackNode :public BehaviourTreeNode
{
public:
	BTRifleAttackNode(BTValue* obj)
	{
		Value = obj;
	};
	int RoundCount = 4;
	float DelayBetween = 0.1f;
	float CoolDownTime = 0.4f;
	virtual void OnAddedToTree() override;
	virtual std::string GetDebugName() override
	{
		return "BTRifleAttackNode";
	}
protected:
	void Run();
	void Reset();
	EBTNodeReturn::Type ExecuteNode() override;//todo: fix!
private:
	float CurrentBurstCoolDown = 0.0f;

	float CurrentDelay = 0.0f;
	int RemainingRounds = 0;
	B_AIBase* Controller = nullptr;
	BTValue* Value = nullptr;
};