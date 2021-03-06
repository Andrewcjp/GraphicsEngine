#include "MathUtils.h"

namespace MathUtils
{

	bool AlmostEqual(float a, float b, float epsilon)
	{
		return fabs(a - b) <= epsilon;
	}

	bool AlmostEqual(glm::vec3 a, glm::vec3 b, float epsilon)
	{
		const float SqEpsi = epsilon * epsilon;
		const float distanceSQ = glm::distance2(a, b);

		return distanceSQ <= SqEpsi;
	}

	void MathUtils::CheckNAN_(glm::vec3 value)
	{
		glm::vec3::bool_type t = glm::isnan(value);
		if (t.x == true || t.y == true || t.z == true)
		{
			__debugbreak();
		}
	}

	void MathUtils::CheckNAN_(glm::quat value)
	{
		glm::vec4::bool_type t = glm::isnan(value);
		if (t.x == true || t.y == true || t.z == true || t.w == true)
		{
			__debugbreak();
		}
	}

	glm::vec3 VectorProject(const glm::vec3 length, const glm::vec3 direction)
	{
		float dot = glm::dot(length, direction);
		float magSq = glm::length2(direction);
		return direction * (dot / magSq);
	}

	float MakeSafeForDivide(float value, float MinValue)
	{
		if (MathUtils::AlmostEqual(value, 0.0f, FLT_EPSILON))
		{
			return MinValue;
		}
		return value;
	}

};
