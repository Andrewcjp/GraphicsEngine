#pragma once
#include "AI\Core\AnimationController.h"
/*!  \addtogroup Game_AI
* @{ */
class Possesed_Anim_Controller : public AnimationController
{
public:
	Possesed_Anim_Controller();
	~Possesed_Anim_Controller();


	virtual void OnTick(float dt) override;

};

