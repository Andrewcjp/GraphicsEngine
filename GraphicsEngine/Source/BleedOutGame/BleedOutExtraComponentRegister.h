#pragma once
#include "EngineHeader.h"
namespace ComponentTypes
{
	enum TYPE
	{
		TGComp,
		PhysicsThrowerComponent,
		TestPlayer,
		Weapon,
		WeaponManager,
		Projectile,
		Health,
		Limit
	};
}
class BleedOutExtraComponentRegister :
	public ExtraComponentRegister
{
public:
	BleedOutExtraComponentRegister();
	~BleedOutExtraComponentRegister();
	Component* CreateExtraComponent(int id)override;
};
