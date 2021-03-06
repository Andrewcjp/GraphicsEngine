#pragma once
#if TDSIM_ENABLED
#include "Physics/Physics_fwd.h"
#include "Physics/GenericRigidBody.h"
#include "core/Transform.h"
#include "TDPhysicsAPI.h"

namespace TD
{
	class TDMesh;
}

class TDRigidBody :public GenericRigidBody
{
public:
	TDRigidBody(EBodyType::Type type, Transform T);
	~TDRigidBody();
	glm::vec3 GetPosition()const;
	glm::quat GetRotation()const;
	CORE_API void AddTorque(glm::vec3);
	CORE_API void AddForce(glm::vec3 force, EForceMode::Type Mode = EForceMode::AsForce);
	glm::vec3 GetLinearVelocity() const;
	void AttachCollider(Collider* col);
	TD::TDMesh * GenerateTriangleMesh(std::string Filename, glm::vec3 scale);
	CORE_API void SetLinearVelocity(glm::vec3 velocity);
	CORE_API void InitBody();
	void SetPositionAndRotation(glm::vec3 pos, glm::quat rot);
	CORE_API void SetGravity(bool state);
	CORE_API TD::TDActor* GetActor();
	CORE_API float GetMass();
	void SetPhysicsMaterial(PhysicalMaterial* Mat);
	void UpdateBodyState();
private:
	std::vector<Collider*> AttachedColliders;
	Transform m_transform;
	TD::TDRigidDynamic* Actor = nullptr;
	TD::TDRigidStatic* StaticActor = nullptr;
	TD::TDActor* CommonActorPTr = nullptr;
	std::vector<TD::TDShape*> shapes;
	TD::TDPhysicalMaterial* TDMaterial = nullptr;
};

#endif