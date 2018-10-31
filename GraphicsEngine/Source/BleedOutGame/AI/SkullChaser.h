#pragma once
#include "AI/Core/AIBase.h"
class SkullChaser : public AIBase
{
public:
	SkullChaser();
	~SkullChaser();

	// Inherited via Component
	virtual void InitComponent() override;
	virtual void Update(float dt) override;
};

