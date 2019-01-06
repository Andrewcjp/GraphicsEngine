#pragma once
#include "AI/Core/AIBase.h"
#include "Source/BleedOutGame/Components/BleedOutPlayer.h"
/*! \defgroup Game_AI Game AI Classes  
*\addtogroup Game_AI
* @{ */
class Weapon;
class BleedOutGameMode;
class RigidbodyComponent;
class B_AIBase : public AIBase
{
public:
	B_AIBase();
	~B_AIBase();
	bool FireAt(glm::vec3 pos);
	void OnDead();
	Weapon* MainWeapon = nullptr;

	virtual void SceneInitComponent() override;

protected:
	BleedOutGameMode* GameMode = nullptr;

	virtual void Update(float dt) override;

private:
	virtual void InitComponent() override;
	WalkAudio Walk;
	RigidbodyComponent* RB = nullptr;
};

