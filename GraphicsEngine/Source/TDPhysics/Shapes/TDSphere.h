#pragma once
#include "TDShape.h"
namespace TD 
{
	class TD_API TDSphere :public TDShape
	{
	public:
		 TDSphere();
		 ~TDSphere();
		float Radius = 1;
		virtual glm::vec3 GetBoundBoxHExtents() override;

	};
}