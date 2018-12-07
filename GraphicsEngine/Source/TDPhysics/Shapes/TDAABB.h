#pragma once
#include "TDShape.h"
namespace TD
{
	class TDAABB : public TDShape
	{
	public:
		TDAABB();
		TDAABB(glm::vec3 Pos,glm::vec3 size);
		~TDAABB();
		glm::vec3 GetMin()const;
		glm::vec3 GetMax()const;
		void DebugRender(glm::vec3 colour = glm::vec3(1));

		glm::vec3 HalfExtends = glm::vec3(size, size, size);
		glm::vec3 Position = glm::vec3(0,0,0);
		TDActor* Owner = nullptr;
	private:		
		const float size = 10.0f;
	};
}

