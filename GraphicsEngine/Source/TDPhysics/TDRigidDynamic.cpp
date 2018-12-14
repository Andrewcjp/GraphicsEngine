#include "TDRigidDynamic.h"
#include "TDPhysics.h"
#include "TDSimConfig.h"
#include "TDSolver.h"

namespace TD
{
	TDRigidDynamic::TDRigidDynamic()
	{
		ActorType = TDActorType::RigidDynamic;
		CachedsqSleepZeroThreshold = TDPhysics::GetCurrentSimConfig()->BodySleepZeroThreshold;
		CachedsqSleepZeroThreshold = CachedsqSleepZeroThreshold * CachedsqSleepZeroThreshold;//All compares are done Squared
		ComputeInertiaTensor();
	}

	TDRigidDynamic::~TDRigidDynamic()
	{}

	bool TDRigidDynamic::IsEffectedByGravity()
	{
		return UseGravity;
	}

	glm::vec3 TDRigidDynamic::GetLinearVelocityDelta()
	{
		return DeltaLinearVel;
	}

	glm::vec3 TDRigidDynamic::GetAngularVelocityDelta()
	{
		return DeltaAngularVel;
	}

	void TDRigidDynamic::AddForce(glm::vec3 Force, bool AsForce)
	{
		glm::vec3 Addition = glm::vec3();
		if (AsForce)
		{
			Addition = Force * GetInvBodyMass();
		}
		else
		{
			Addition = Force;
		}
		DeltaLinearVel += Addition;
	}

	bool TDRigidDynamic::CheckSleep(glm::vec3 & value)
	{
		if (glm::length2(value) <= CachedsqSleepZeroThreshold)
		{
			value = glm::vec3();
			return true;
		}
		return false;
	}

	void TDRigidDynamic::UpdateSleepTimer(float DT)
	{
		if (CheckSleep(DeltaLinearVel) && CheckSleep(DeltaAngularVel) && CheckSleep(LinearVelocity) && CheckSleep(AngularVel))
		{
			SleepTimer += DT;
		}
		else
		{
			SleepTimer = 0.0f;
		}
	}

	bool TDRigidDynamic::IsBodyAsleep() const
	{
		return SleepTimer > SleepMaxTimer;
	}

#if VALIDATE_KE
	float TDRigidDynamic::Compute_KE()
	{
		return 0.5f* (GetBodyMass() * glm::length(GetLinearVelocity())*glm::length(GetLinearVelocity()));
	}

	void TDRigidDynamic::ComputeKE()
	{
		PreSimKE = Compute_KE();
	}

	void TDRigidDynamic::ValidateKE()
	{
		const float postsim = Compute_KE();
		if (AttachedShapes.size() != 2)
		{
			DebugEnsure(postsim == PreSimKE);
		}
	}
#endif

	void TDRigidDynamic::ResetForceThisFrame()
	{
		DeltaLinearVel = glm::vec3();
		DeltaAngularVel = glm::vec3();
	}

	void TDRigidDynamic::SetGravity(bool enabled)
	{
		UseGravity = enabled;
	}

	float TDRigidDynamic::GetBodyMass()
	{
		return BodyMass;
	}

	float TDRigidDynamic::GetInvBodyMass()
	{//todo: cache this
		if (BodyMass <= FLT_EPSILON)
		{
			return 0.0f;
		}
		return 1.0f / BodyMass;
	}

	glm::vec3 TDRigidDynamic::GetLinearVelocity()
	{
		return LinearVelocity;
	}

	void TDRigidDynamic::SetLinearVelocity(glm::vec3 newvel)
	{
		LinearVelocity = newvel;
	}

	glm::vec3 TDRigidDynamic::GetAngularVelocity()
	{
		return AngularVel;
	}

	void TDRigidDynamic::ComputeInertiaTensor()
	{
		if (BodyMass == 0.0f)
		{
			InertaTensor = glm::mat4(0);
		}
		float ix = 0.0f;
		float iy = 0.0f;
		float iz = 0.0f;
		float iw = 0.0f;

		if (true)//todo: check this
		{
			float r2 = 1.0;
			float fraction = (2.0f / 5.0f);

			ix = r2 * BodyMass * fraction;
			iy = r2 * BodyMass * fraction;
			iz = r2 * BodyMass * fraction;
			iw = 1.0f;
		}
		InertaTensor = glm::inverse(glm::mat4x4(
			ix, 0, 0, 0,
			0, iy, 0, 0,
			0, 0, iz, 0,
			0, 0, 0, iw));
	}

	glm::mat4x4 TDRigidDynamic::GetInertiaTensor()
	{
		return InertaTensor;
	}

	void TDRigidDynamic::SetInertiaTensor(glm::mat4x4 tensor)
	{
		InertaTensor = tensor;
	}

	void TDRigidDynamic::SetAngularVelocity(glm::vec3 velocity)
	{
		AngularVel = velocity;
	}

	void TDRigidDynamic::AddTorque(glm::vec3 Torque)
	{
		glm::vec3 angAccel = glm::vec4(Torque, 0.0f) * GetInertiaTensor();
		DeltaAngularVel += angAccel;
	}

	void TDRigidDynamic::AddForceAtPosition(glm::vec3 pos, glm::vec3 force)
	{
		glm::vec3 centerOfMass = GetTransfrom()->GetPos();
		glm::vec3 torque = glm::cross(pos - centerOfMass, force);
		AddTorque(torque);
		AddForce(force, true);
	}

	void TDRigidDynamic::SetBodyMass(float Mass)
	{
		BodyMass = Mass;
		ComputeInertiaTensor();
	}

}