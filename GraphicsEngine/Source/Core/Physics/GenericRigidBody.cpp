#include "GenericRigidBody.h"




GenericRigidBody::GenericRigidBody(EBodyType::Type type)
{
	BodyType = type;
}

GenericRigidBody::~GenericRigidBody()
{}

glm::vec3 GenericRigidBody::GetPosition()
{
	return glm::vec3();

}

glm::quat GenericRigidBody::GetRotation()
{
	return glm::quat();
}

void GenericRigidBody::AddTorque(glm::vec3 torque)
{

}

void GenericRigidBody::AddForce(glm::vec3 force)
{}

glm::vec3 GenericRigidBody::GetLinearVelocity()
{
	return glm::vec3();
}

void GenericRigidBody::AttachCollider(Collider * col)
{}

void GenericRigidBody::SetLockFlags(BodyInstanceData data)
{
	LockData = data;
	UpdateFlagStates();
}

BodyInstanceData GenericRigidBody::GetLockFlags()
{
	return LockData;
}

GenericCollider::GenericCollider()
{}

GenericCollider::~GenericCollider()
{}