#pragma once
#include "Core/Transform.h"


class Light
{
public:
	static int LastId;
	enum LightType { Directional, Point, Spot };
	Light(glm::vec3 positon, float intesity, LightType type = LightType::Point, glm::vec3 Lightcolor = glm::vec3(1, 1, 1), bool shadow = true);
	~Light();
	glm::vec3 GetPosition() const {
		return m_position;
	}
	glm::vec3 GetColor() const {
		return m_lightColor *m_intesity;
	}
	LightType GetType() const {
		return m_type;
	}
	void SetPostion(glm::vec3 pos) {
		m_position = pos;
	}
	void SetIntesity(float value) {
		m_intesity = value;
	}
	void SetLightType(LightType value)
	{
		m_type = value;
	}
	void SetShadow(bool state) {
		DoesShadow = state;
	}
	void SetDirection(const glm::vec3 &value) {
		m_direction = glm::normalize(value);
	}
	bool GetDoesShadow() const { return DoesShadow; }
	void SetShadowId(int id) { ShadowId = id; }
	int GetShadowId() const { return ShadowId; }
	int DirectionalShadowid = -1;
	glm::vec3 GetDirection()const { return m_direction; }
	float GetIntesity()
	{
		return m_intesity;
	}
	glm::mat4 DirView;
	glm::mat4 Projection;
	glm::vec3 m_lightColor = glm::vec3(1, 1, 1);
private:
	float m_intesity = 10;
	glm::vec3 m_position = glm::vec3(0, 0, 0);
	
	glm::vec3 m_direction = glm::vec3(0, 1, 0);
	LightType m_type;
	bool DoesShadow = true;
	int ShadowId = -1;

};

