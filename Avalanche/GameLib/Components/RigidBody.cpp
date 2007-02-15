/*
**
**  File:   RigidBody.cpp
**  Date:   June 28, 2004
**  By:     Bryant Collard
**  Desc:   Models 6 DOF rigid body dynamics.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Components/RigidBody.cpp $
**  $Revision: #14 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#include "Components/ComponentsPCH.h"
#include "Components/RigidBody.h"
#include "Components/ComponentCreator.h"
#include "Components/RigidBodyTable.h"
#include "mathUtil/Cholesky.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(CRigidBody, RigidBody, COMPONENT_CONTROLLER_PRIORITY);

/******************************************************************************/
/******************************************************************************/
CRigidBody::CRigidBody(CCompActor &i_actor) : CActorComponent(i_actor)
{
	d_mass = 1.0f;
	d_massInv = 1.0f;
	d_inertia.Identity();
	d_inertiaInv.Identity();
	d_cgInBody.Clear();
	d_cgToBody.Identity();
	d_bodyAccMag = 0.0f;
	d_bodyAccDir.Set(0.0f, -1.0f, 0.0f);
	d_autoApply = false;
	Reset();

	actor.AddProperty("bodyAcc", Property_Vector3);
	actor.SetProperty("bodyAcc", d_bodyAccMag * d_bodyAccDir);
}

/******************************************************************************/
/******************************************************************************/
CInterface* CRigidBody::GetInterface(uint i_id)
{
	if (i_id == CRigidBodyIF::GetID())
		return((CInterface*)this);
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::SetMass(float i_mass)
{
	ASSERT(!Math::DivByZero(1.0f, i_mass));
	d_mass = i_mass;
	d_massInv = 1.0f / i_mass;
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::SetInertia(const Matrix3x3 &i_inertia)
{
	d_inertia = i_inertia;
	bool invertable = d_inertia.Invert(d_inertiaInv);
	ASSERT (invertable);
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::SetInertia(float i_Ixx, float i_Iyy, float i_Izz)
{
	ASSERT(!Math::DivByZero(1.0f, i_Ixx) && !Math::DivByZero(1.0f, i_Iyy) &&
			!Math::DivByZero(1.0f, i_Izz));
	d_inertia.Set(i_Ixx, 0.0f, 0.0f, 0.0f, i_Iyy, 0.0f,
			0.0f, 0.0f, i_Izz);
	d_inertiaInv.Set(1.0f / i_Ixx, 0.0f, 0.0f, 0.0f,
			1.0f / i_Iyy, 0.0f, 0.0f, 0.0f, 1.0f / i_Izz);
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::SetCG(Vector3CRef i_cgInBody, const DirCos3x3 &i_cgToBody)
{
	d_cgInBody = i_cgInBody;
	d_cgToBody = i_cgToBody;

	d_cgInWorld = actor.d_bodyInWorld + (d_cgInBody * actor.d_bodyToWorld);
	d_cgToWorld = d_cgToBody * actor.d_bodyToWorld;
	d_cgVelocityInWorld = actor.d_velocityInWorld +
			actor.d_omegaInBody.Cross(d_cgInBody) * actor.d_bodyToWorld;
	d_omegaInCG = actor.d_omegaInBody / d_cgToBody;
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::SetBody(Vector3CRef i_cgInBody, const DirCos3x3 &i_cgToBody)
{
	d_cgInBody = i_cgInBody;
	d_cgToBody = i_cgToBody;

	if (IsActive())
	{
		actor.d_bodyToWorld = d_cgToBody.InvertTimes(d_cgToWorld);
		actor.d_bodyInWorld = d_cgInWorld - d_cgInBody * actor.d_bodyToWorld;
		actor.d_omegaInBody = d_omegaInCG * d_cgToBody;
		actor.d_velocityInWorld = d_cgVelocityInWorld -
				actor.d_omegaInBody.Cross(d_cgInBody) * actor.d_bodyToWorld;
		actor.SyncState();
	}
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::SetBodyAcceleration(Vector3CRef i_bodyAcc, bool i_autoApply)
{
	d_bodyAccMag = i_bodyAcc.Length();
	if (!Math::DivByZero(1.0f, d_bodyAccMag))
		d_bodyAccDir = (1.0f / d_bodyAccMag) * i_bodyAcc;
	else
	{
		d_bodyAccMag = 0.0f;
		d_bodyAccDir.Set(0.0f, -1.0f, 0.0f);
	}
	d_autoApply = i_autoApply;
	actor.SetProperty("bodyAcc", i_bodyAcc);
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::SetCGInWorld(Vector3CRef i_cgInWorld)
{
	d_cgInWorld = i_cgInWorld;
	actor.d_bodyInWorld = d_cgInWorld - d_cgInBody * actor.d_bodyToWorld;
	actor.SyncState();
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::SetCGToWorld(const DirCos3x3 &i_cgToWorld)
{
	d_cgToWorld = i_cgToWorld;
	actor.d_bodyToWorld = d_cgToBody.InvertTimes(d_cgToWorld);
	actor.d_bodyInWorld = d_cgInWorld - d_cgInBody * actor.d_bodyToWorld;
	actor.d_velocityInWorld = d_cgVelocityInWorld -
			actor.d_omegaInBody.Cross(d_cgInBody) * actor.d_bodyToWorld;
	actor.SyncState();
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::SetCGVelocityInWorld(Vector3CRef i_cgVelocityInWorld)
{
	d_cgVelocityInWorld = i_cgVelocityInWorld;
	actor.d_velocityInWorld = d_cgVelocityInWorld -
			actor.d_omegaInBody.Cross(d_cgInBody) * actor.d_bodyToWorld;
	actor.SyncState();
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::SetAngVelocityInCG(Vector3CRef i_omegaInCG)
{
	d_omegaInCG = i_omegaInCG;
	actor.d_omegaInBody = d_omegaInCG * d_cgToBody;
	actor.d_velocityInWorld = d_cgVelocityInWorld -
			actor.d_omegaInBody.Cross(d_cgInBody) * actor.d_bodyToWorld;
	actor.SyncState();
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::ApplyExternalForce(Vector3CRef i_forceInWorld,
		Vector3CRef i_locInWorld)
{
	Vector3 force = i_forceInWorld / d_cgToWorld;

	d_externalForceSum += force;
	d_externalTorqueSum += ((i_locInWorld - d_cgInWorld) / d_cgToWorld).Cross(
			force);
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::ApplyExternalTorque(Vector3CRef i_torqueInWorld)
{
	d_externalTorqueSum += i_torqueInWorld / d_cgToWorld;
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::ApplyForceCG(Vector3CRef i_force)
{
	d_forceSum += (i_force / d_cgToBody);
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::ApplyForce(Vector3CRef i_force, Vector3CRef i_loc)
{
	Vector3 force = i_force / d_cgToBody;

	d_forceSum += force;
	d_torqueSum += ((i_loc - d_cgInBody) / d_cgToBody).Cross(force);
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::ApplyTorque(Vector3CRef i_torque)
{
	d_torqueSum += (i_torque / d_cgToBody);
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::Initialize(void)
{
	SyncActor();
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::Activate(void)
{
	SyncActor();
	CActorComponent::Activate();
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::Reset(void)
{
	d_forceSum.Clear();
	d_torqueSum.Clear();
	d_externalForceSum.Clear();
	d_externalTorqueSum.Clear();
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::SyncActor(void)
{
	d_cgInWorld = actor.d_bodyInWorld + (d_cgInBody * actor.d_bodyToWorld);
	d_cgToWorld = d_cgToBody * actor.d_bodyToWorld;
	d_cgVelocityInWorld = actor.d_velocityInWorld +
			actor.d_omegaInBody.Cross(d_cgInBody) * actor.d_bodyToWorld;
	d_omegaInCG = actor.d_omegaInBody / d_cgToBody;
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::BeginUpdate(void)
{
	// Only update if active.
	if (!IsActive())
		return;

	// Save starting state.
	d_beginCGInWorld = d_cgInWorld;
	d_beginCGToWorld = d_cgToWorld;
	d_beginCGVelocityInWorld = d_cgVelocityInWorld;
	d_beginOmegaInCG = d_omegaInCG;

	// Make sure the displacement energy is zero.
	d_displacementEnergy = 0.0f;

	// Auto apply acceleration
	if (d_autoApply)
		d_forceSum += d_mass * d_bodyAccMag * d_bodyAccDir / d_cgToWorld;
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::AttemptUpdate(float i_deltaSec)
{
	// Only update if active.
	if (!IsActive())
		return;

	// Perform translational integration.

	// Find (1/2)*a*dt expressed in world coordinate and save in accTerm. Note
	// that a = F/m.
	Vector3 accTerm = (0.5f * i_deltaSec * d_massInv) *
			((d_forceSum + d_externalForceSum) * d_beginCGToWorld);

	// Find (1/2)*a*dt + v0 and save in velTerm.
	Vector3 velTerm = accTerm + d_beginCGVelocityInWorld;

	// Add (1/2)*a*dt to velTerm to get v1 = a*dt + v0.
	d_cgVelocityInWorld = accTerm + velTerm;

	// Multiply velTerm by dt and add to x0 to get
	// x1 = (1/2)*a*dt**2 + v0*dt + x0.
	d_cgInWorld = d_beginCGInWorld + velTerm * i_deltaSec;

	// Perform rotational integration.

	// Find the new angular velocity.
	accTerm = ((d_torqueSum + d_externalTorqueSum) -
			d_beginOmegaInCG.Cross(d_beginOmegaInCG * d_inertia)) * d_inertiaInv;
	d_omegaInCG = d_beginOmegaInCG + accTerm * i_deltaSec;

	// Find a new orientation by behaving as if the angular velocity is constant
	// during the time step, using the average of the old and new angular
	// velocities as the constant angular velocity.
	Vector3 axis = 0.5f * (d_beginOmegaInCG + d_omegaInCG);
	float mag;
	if (axis.ReallySafeNormalize(mag))
	{
		DirCos3x3 newToOld;
		newToOld.SetAngleAxis(mag * i_deltaSec, axis);
		d_cgToWorld = newToOld * d_beginCGToWorld;
	}
	else
		d_cgToWorld = d_beginCGToWorld;

	// Set the actor state.
	actor.d_bodyToWorld = d_cgToBody.InvertTimes(d_cgToWorld);
	actor.d_bodyInWorld = d_cgInWorld - d_cgInBody * actor.d_bodyToWorld;
	actor.d_omegaInBody = d_omegaInCG * d_cgToBody;
	actor.d_velocityInWorld = d_cgVelocityInWorld -
			actor.d_omegaInBody.Cross(d_cgInBody) * actor.d_bodyToWorld;
	actor.SyncState();

	// Save delta sec to determine if displacement energy should be dumped.
	d_deltaSec = i_deltaSec;
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::EndUpdate(float i_deltaSec)
{
	// Only update if active.
	if (!IsActive())
		return;

	d_forceSum.Clear();
	d_torqueSum.Clear();
	d_externalForceSum.Clear();
	d_externalTorqueSum.Clear();
}

/******************************************************************************/
/******************************************************************************/
bool CRigidBody::CoupledCollisionDetect(CStageEntity* i_entity,
		CCollisionEvent* i_event, uint &o_myEffect, uint &o_otherEffect)
{
	// Only update if active.
	if (!IsActive())
		return(false);

	// Can only do a coupled response if hitting another rigid body.
	if (!i_entity->IsA(CActor::ClassType()))
		return(false);

	CRigidBodyIF* rigidBodyIF =
			(CRigidBodyIF*)((CActor*)i_entity)->GetInterface(CRigidBodyIF::GetID());
	if (rigidBodyIF == NULL)
		return(false);

	// Characterize the collision as a point and a normal.
	Vector3 point, normal;
	if (!CEventMiner::PlaneFit(i_event, point, normal))
	{
		o_myEffect = CProtagonist::EFFECT_NONE;
		o_otherEffect = CProtagonist::EFFECT_NONE;
		return(true);
	}

	// Find the collision parameters.
	u8 queryType, targetType;
	CEventMiner::SurfaceType(i_event, point, normal, queryType, targetType);
	SRigidBodyData* surface = g_rigidBodyTable.GetValidEntry(queryType,
			targetType);

	// Find the penetration depths.
	float queryDepth, targetDepth;
	CEventMiner::Depths(i_event, point, normal, queryDepth, targetDepth);

	// Match the rigid body interfaces to the query and target in the collision
	// event.
	CRigidBodyIF* query;
	CRigidBodyIF* target;
	if (i_entity == i_event->GetQueryEntity())
	{
		query = rigidBodyIF;
		target = (CRigidBodyIF*)this;
	}
	else
	{
		query = (CRigidBodyIF*)this;
		target = rigidBodyIF;
	}

	// Displace each body.
	query->Displace(queryDepth * normal);
	target->Displace(-targetDepth * normal);

	// Reduce kinetic energy to account for any potential energy added by
	// displacement.
	query->DumpDisplacementEnergy();
	target->DumpDisplacementEnergy();

	// Adjust velocities in response to the collision.
	query->CoupledCollideWithRigidBody(target, point, normal,
			surface->d_cor, surface->d_cog);

	o_myEffect = CProtagonist::EFFECT_DISPLACED;
	o_otherEffect = CProtagonist::EFFECT_DISPLACED;
	return(true);
}

/******************************************************************************/
/******************************************************************************/
uint CRigidBody::CollisionDetect(CStageEntity* i_entity,
		CCollisionEvent* i_event, bool i_mateDisplaced)
{
	// Only update if active.
	if (!IsActive())
		return(CProtagonist::EFFECT_NONE);

	// Don't react to a collision if the other entity says it does not affect
	// this actor.
	uint hint = 0;
	if (i_entity->IsA(CProtagonist::ClassType()))
	{
		hint = ((CProtagonist*)i_entity)->GetTailoredInteractionHint(&actor);
		if ((hint & CProtagonist::HINT_IMPOTENT) != 0)
			return(CProtagonist::EFFECT_NONE);
	}

	// Retest the pair of entities if the other entity has already displaced
	// in response to this collision.
	if (i_mateDisplaced)
	{
		i_event = CCollisionEvent::TestEntityPair((CStageEntity*)&actor,
				i_entity, &g_collisionSystem.GetStack());
		if (i_event == NULL)
			return(CProtagonist::EFFECT_NONE);
	}

	// Characterize the collision as a point and a normal.
	Vector3 point, normal;
	float depth, targetDepth;
	if (!CEventMiner::PlaneFit(i_event, point, normal))
		return(CProtagonist::EFFECT_NONE);

	// Find the collision parameters.
	u8 queryType, targetType;
	CEventMiner::SurfaceType(i_event, point, normal, queryType, targetType);
	SRigidBodyData* surface = g_rigidBodyTable.GetValidEntry(queryType,
			targetType);

	// Find the penetration depths. If the other entity has already displaced in
	// response to this collision or if the other entity will not displace, make
	// this entity perform all of the displacement needed to prevent
	// interpenetration.
	CEventMiner::Depths(i_event, point, normal, depth, targetDepth);
	if (!i_mateDisplaced || ((hint & CProtagonist::HINT_JUGGERNAUT) != 0))
		depth += targetDepth;

	// Make sure the normal points into this entity then displace.
	if ((CStageEntity*)&actor == i_event->GetTargetEntity())
		normal.Negate();
	Displace(depth * normal);

	// Reduce kinetic energy to account for any potential energy added by
	// displacement.
	DumpDisplacementEnergy();

	// Put a sticky on the other entity at the impact point.
	CSticky sticky;
	sticky.SetInWorld(i_entity, point);

	// Adjust velocities in response to the collision.
	CollideWithMassive(point, normal, sticky.GetVelocity(),
			surface->d_cor, surface->d_cog);

	return(CProtagonist::EFFECT_DISPLACED);
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::Displace(Vector3CRef i_displacement)
{
	d_cgInWorld += i_displacement;
	actor.d_bodyInWorld += i_displacement;
	actor.SyncState();

	// Find the amount of energy added to the system by displacement.
	d_displacementEnergy += -d_mass * d_bodyAccMag *
			i_displacement.Dot(d_bodyAccDir);
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::DumpDisplacementEnergy(void)
{
	if (DumpDisplacementEnergy(d_cgVelocityInWorld, d_omegaInCG))
	{
		actor.d_omegaInBody = d_omegaInCG * d_cgToBody;
		actor.d_velocityInWorld = d_cgVelocityInWorld -
				actor.d_omegaInBody.Cross(d_cgInBody) * actor.d_bodyToWorld;
		actor.SyncState();
	}
}

/******************************************************************************/
/******************************************************************************/
bool CRigidBody::DumpDisplacementEnergy(Vector3 &io_cgVelocityInWorld,
		Vector3 &io_omegaInCG)
{
	// If the model is running fast, don't dump energy.
	if (d_deltaSec < 0.01f)
	{
		d_displacementEnergy = 0.0f;
		return (false);
	}

	// If no energy is added or if it is subtracted, exit indicating that no
	// change in state occurred.
	if (d_displacementEnergy <= 0.0f)
	{
		d_displacementEnergy = 0.0f;
		return (false);
	}

	// Determine the component of velocity in the direction of the body
	// acceleration. If it is positive, remove the energy associated with it.
	float availableEnergy;
	float downVelocity = io_cgVelocityInWorld.Dot(d_bodyAccDir);
	if (downVelocity > 0.0f)
	{
		// Determine the amount of energy assoicated with movement in the
		// direction of the body acceleration.
		availableEnergy = 0.5f * d_mass * downVelocity * downVelocity;

		// If there is more energy available than needs to be removed, remove what
		// is needed and exit indicating a change in state.
		if (availableEnergy >= d_displacementEnergy)
		{
			io_cgVelocityInWorld -= (downVelocity - Math::Sqrt(2.0f *
					(availableEnergy - d_displacementEnergy) / d_mass)) *
					d_bodyAccDir;
			d_displacementEnergy = 0.0f;
			return (true);
		}

		// If there is less energy available than needs to be removed, remove all
		// that is avaliable, reduce the excess energy value by the corresponding
		// amount and continue.
		io_cgVelocityInWorld -= downVelocity * d_bodyAccDir;
		d_displacementEnergy -= availableEnergy;
	}

	// Determine the amount of kinetic energy available.
	availableEnergy = 0.5f * (d_mass *
			io_cgVelocityInWorld.Dot(io_cgVelocityInWorld) +
			io_omegaInCG.Dot(io_omegaInCG * d_inertia));

	// If there is more energy available than needs to be removed, remove what is
	// needed.
	if (availableEnergy >= d_displacementEnergy)
	{
		float f = Math::Sqrt((availableEnergy - d_displacementEnergy) /
				availableEnergy);
		io_cgVelocityInWorld *= f;
		io_omegaInCG *= f;
	}

	// If there is less energy available than needs to be removed, remove all
	// that is avaliable. There is no more kinetic energy available.
	else
	{
		io_cgVelocityInWorld.Clear();
		io_omegaInCG.Clear();
	}

	// Exit indicating a change in state.
	d_displacementEnergy = 0.0f;
	return (true);
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::CollideWithMassive(Vector3CRef i_point, Vector3CRef i_normal,
		Vector3CRef i_velocity, float i_cor, float i_cog)
{
	// Find the relative velocity at the impact point before the impact. Don't
	// do anything if moving away from the impact.
	Vector3 relVelocity = GetPointVelocity(i_point) - i_velocity;
	float normalVelocity = i_normal.Dot(relVelocity);
	if (normalVelocity >= 0.0f)
		return;

	// Find the velocity of the impact point after the impact.
	Vector3 pointVelocity = i_cog * relVelocity +
			(i_cor - i_cog) * normalVelocity * i_normal + i_velocity;

	// Find the location of the CG relative to the impact point in the CG frame.
	Vector3 cgInPoint = (d_cgInWorld - i_point) / d_cgToWorld;

	// Find the moment of inertia about the impact point.
	float collisionInertia[6];
	collisionInertia[0] = d_inertia.e00 + d_mass * (cgInPoint.Y() * cgInPoint.Y()
			+ cgInPoint.Z() * cgInPoint.Z());
	collisionInertia[1] = d_inertia.e01 - d_mass * cgInPoint.X() * cgInPoint.Y();
	collisionInertia[2] = d_inertia.e02 - d_mass * cgInPoint.X() * cgInPoint.Z();
	collisionInertia[3] = d_inertia.e11 + d_mass * (cgInPoint.X() * cgInPoint.X()
			+ cgInPoint.Z() * cgInPoint.Z());
	collisionInertia[4] = d_inertia.e12 - d_mass * cgInPoint.Y() * cgInPoint.Z();
	collisionInertia[5] = d_inertia.e22 + d_mass * (cgInPoint.X() * cgInPoint.X()
			+ cgInPoint.Y() * cgInPoint.Y());

	// Find a momentum-like term.
	Vector3 momentum = d_omegaInCG * d_inertia + d_mass * cgInPoint.Cross(
			(d_cgVelocityInWorld - pointVelocity) / d_cgToWorld);

	// Solve for the new angular velocity.
	float omega[3];
	omega[0] = momentum.X();
	omega[1] = momentum.Y();
	omega[2] = momentum.Z();
	bool result = CCholesky::Solve(collisionInertia, omega, 3);
	ASSERT (result);

	// Update the affected state variables.
	d_omegaInCG.Set(omega);
	d_cgVelocityInWorld = pointVelocity -
			(cgInPoint.Cross(d_omegaInCG) * d_cgToWorld);
	actor.d_omegaInBody = d_omegaInCG * d_cgToBody;
	actor.d_velocityInWorld = d_cgVelocityInWorld -
			actor.d_omegaInBody.Cross(d_cgInBody) * actor.d_bodyToWorld;
	actor.SyncState();
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::CollideWithMassiveAsSphere(Vector3CRef i_point,
		Vector3CRef i_velocity, float i_cor, float i_cog)
{
	Vector3 normal = i_point - d_cgInWorld;
	if (!normal.SafeNormalize())
		normal.Set(0.0f, 1.0f, 0.0f);
	CollideWithMassive(i_point, normal, i_velocity, i_cor, i_cog);
}

/******************************************************************************/
/******************************************************************************/
Vector3 CRigidBody::CollideWithPointMass(Vector3CRef i_point,
		Vector3CRef i_normal, float i_mass, Vector3CRef i_velocity,
		float i_cor, float i_cog)
{
	// Find the relative velocity at the impact point. If the bodies are moving
	// away from each other, no collision is taking place.
	Vector3 deltaVelocity = GetPointVelocity(i_point) - i_velocity;
	float normalVelocity = i_normal.Dot(deltaVelocity);
	if (normalVelocity >= 0.0f)
		return(i_velocity);

	// Compute the relative velocity after impact.
	deltaVelocity = i_cog * deltaVelocity +
			(i_cor - i_cog) * normalVelocity * i_normal;

	// Compute the impact point to CG vector.
	Vector3 radius = d_cgInWorld - i_point;

	// Compute the total translational momentum in world coordinates.
	Vector3 translationalMomentum = d_mass * d_cgVelocityInWorld +
			i_mass * i_velocity;

	// Angular momentum is made up from two parts; the rotation of the body about
	// its center of mass and the rotation of the center of mass about the impact
	// point. The first part is the product of angular velocity and moment of
	// inertia. Those two components are stored in body coordinates. If Wb is
	// angular velocity in body coordinates, Ib is moment of inertia in body
	// coordinates, bCw is the transformation from body to world coordinates, and
	// wCb is the transformation from world to body coordinates, the component of
	// angular momentum due to rotation about the center of mass, expressed in
	// world coordinates is:
	//
	//    Hw = (Wb x bCw) * (wCb x Ib x bCw)
	//
	// which can be reduced to:
	//
	//    Hw = Wb x Ib x bCw
	// 
	// Compute the angular momentum and moment of inertia in world coordinates.
	Matrix3x3 inertia = d_inertia * d_cgToWorld;
	Vector3 angularMomentum = d_omegaInCG * inertia +
			d_mass * radius.Cross(d_cgVelocityInWorld);
	inertia = d_cgToWorld.InvertTimes(inertia);

	// Form the right hand side vector.
	float omega[3];
	float massSum = d_mass + i_mass;
	float collisionMassInv = massSum / (d_mass * i_mass);
	Vector3 omegaInWorld = angularMomentum * collisionMassInv -
			radius.Cross((translationalMomentum / i_mass) + deltaVelocity);
	omega[0] = omegaInWorld.X();
	omega[1] = omegaInWorld.Y();
	omega[2] = omegaInWorld.Z();

	// Form the collision inertia matrix.
	float collisionInertia[6];

	collisionInertia[0] = collisionMassInv * inertia.e00 +
			radius.Y() * radius.Y() + radius.Z() * radius.Z();
	collisionInertia[1] = collisionMassInv * inertia.e01 -
			radius.X() * radius.Y();
	collisionInertia[2] = collisionMassInv * inertia.e02 -
			radius.X() * radius.Z();

	collisionInertia[3] = collisionMassInv * inertia.e11 +
			radius.X() * radius.X() + radius.Z() * radius.Z();
	collisionInertia[4] = collisionMassInv * inertia.e12 -
			radius.Y() * radius.Z();

	collisionInertia[5] = collisionMassInv * inertia.e22 +
			radius.X() * radius.X() + radius.Y() * radius.Y();

	// Solve for the angular velocity after impact.
	bool result = CCholesky::Solve(collisionInertia, omega, 3);
	omegaInWorld.Set(omega);
	ASSERT (result);

	// Compute and set the new velocities.
	deltaVelocity = (deltaVelocity + omegaInWorld.Cross(radius)) / massSum;
	translationalMomentum /= massSum;

	SetState(i_mass * deltaVelocity + translationalMomentum,
			omegaInWorld / d_cgToWorld);
	return(-d_mass * deltaVelocity + translationalMomentum);
}

/******************************************************************************/
/******************************************************************************/
Vector3 CRigidBody::CollideWithPointMassAsSphere(Vector3CRef i_point,
		float i_mass, Vector3CRef i_velocity, float i_cor, float i_cog)
{
	Vector3 normal = i_point - d_cgInWorld;
	if (!normal.SafeNormalize())
		normal.Set(0.0f, 1.0f, 0.0f);
	return(CollideWithPointMass(i_point, normal, i_mass, i_velocity, i_cor,
			i_cog));
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::CoupledCollideWithRigidBody(CRigidBodyIF* i_mate,
		Vector3CRef i_point, Vector3CRef i_normal, float i_cor, float i_cog)
{
	// Find the relative velocity at the impact point. If the bodies are moving
	// away from each other, no collision is taking place.
	Vector3 deltaVelocity = GetPointVelocity(i_point) -
			i_mate->GetPointVelocity(i_point);
	float normalVelocity = i_normal.Dot(deltaVelocity);
	if (normalVelocity >= 0.0f)
		return;

	// Compute the relative velocity after impact.
	deltaVelocity = i_cog * deltaVelocity +
			(i_cor - i_cog) * normalVelocity * i_normal;

	// Get the state of the other body before impact.
	float mateMass;
	Matrix3x3 mateInertia;
	Vector3 mateCGInWorld, mateCGVelocityInWorld, mateOmegaInCG;
	DirCos3x3 mateCGToWorld;
	i_mate->GetState(mateMass, mateInertia, mateCGInWorld, mateCGToWorld,
			mateCGVelocityInWorld, mateOmegaInCG);

	// Compute the impact point to CG vectors.
	Vector3 thisRadius = d_cgInWorld - i_point;
	Vector3 mateRadius = mateCGInWorld - i_point;

	// Compute the total translational momentum in world coordinates.
	Vector3 translationalMomentum = d_mass * d_cgVelocityInWorld +
			mateMass * mateCGVelocityInWorld;

	// Angular momentum is made up from two parts; the rotation of the body about
	// its center of mass and the rotation of the center of mass about the impact
	// point. The first part is the product of angular velocity and moment of
	// inertia. Those two components are stored in body coordinates. If Wb is
	// angular velocity in body coordinates, Ib is moment of inertia in body
	// coordinates, bCw is the transformation from body to world coordinates, and
	// wCb is the transformation from world to body coordinates, the component of
	// angular momentum due to rotation about the center of mass, expressed in
	// world coordinates is:
	//
	//    Hw = (Wb x bCw) * (wCb x Ib x bCw)
	//
	// which can be reduced to:
	//
	//    Hw = Wb x Ib x bCw
	// 
	// Compute the angular momentum and moment of inertia for each body in world
	// coordinates.
	Matrix3x3 thisInertia = d_inertia * d_cgToWorld;
	Vector3 thisAngularMomentum = d_omegaInCG * thisInertia +
			d_mass * thisRadius.Cross(d_cgVelocityInWorld);
	thisInertia = d_cgToWorld.InvertTimes(thisInertia);

	mateInertia = mateInertia * mateCGToWorld;
	Vector3 mateAngularMomentum = mateOmegaInCG * mateInertia +
			mateMass * mateRadius.Cross(mateCGVelocityInWorld);
	mateInertia = mateCGToWorld.InvertTimes(mateInertia);

	// Form the right hand side vector.
	float omega[6];
	float massSum = d_mass + mateMass;
	float collisionMassInv = massSum / (d_mass * mateMass);
	Vector3 thisOmegaInWorld = thisAngularMomentum * collisionMassInv -
			thisRadius.Cross((translationalMomentum / mateMass) + deltaVelocity);
	Vector3 mateOmegaInWorld = mateAngularMomentum * collisionMassInv -
			mateRadius.Cross((translationalMomentum / d_mass) - deltaVelocity);
	omega[0] = thisOmegaInWorld.X();
	omega[1] = thisOmegaInWorld.Y();
	omega[2] = thisOmegaInWorld.Z();
	omega[3] = mateOmegaInWorld.X();
	omega[4] = mateOmegaInWorld.Y();
	omega[5] = mateOmegaInWorld.Z();

	// Form the collision inertia matrix.
	float collisionInertia[21];

	collisionInertia[0] = collisionMassInv * thisInertia.e00 +
			thisRadius.Y() * thisRadius.Y() + thisRadius.Z() * thisRadius.Z();
	collisionInertia[1] = collisionMassInv * thisInertia.e01 -
			thisRadius.X() * thisRadius.Y();
	collisionInertia[2] = collisionMassInv * thisInertia.e02 -
			thisRadius.X() * thisRadius.Z();
	collisionInertia[3] = -thisRadius.Y() * mateRadius.Y() -
			thisRadius.Z() * mateRadius.Z();
	collisionInertia[4] = thisRadius.Y() * mateRadius.X();
	collisionInertia[5] = thisRadius.Z() * mateRadius.X();

	collisionInertia[6] = collisionMassInv * thisInertia.e11 +
			thisRadius.X() * thisRadius.X() + thisRadius.Z() * thisRadius.Z();
	collisionInertia[7] = collisionMassInv * thisInertia.e12 -
			thisRadius.Y() * thisRadius.Z();
	collisionInertia[8] = thisRadius.X() * mateRadius.Y();
	collisionInertia[9] = -thisRadius.X() * mateRadius.X() -
			thisRadius.Z() * mateRadius.Z();
	collisionInertia[10] = thisRadius.Z() * mateRadius.Y();

	collisionInertia[11] = collisionMassInv * thisInertia.e22 +
			thisRadius.X() * thisRadius.X() + thisRadius.Y() * thisRadius.Y();
	collisionInertia[12] = thisRadius.X() * mateRadius.Z();
	collisionInertia[13] = thisRadius.Y() * mateRadius.Z();
	collisionInertia[14] = -thisRadius.X() * mateRadius.X() -
			thisRadius.Y() * mateRadius.Y();

	collisionInertia[15] = collisionMassInv * mateInertia.e00 +
			mateRadius.Y() * mateRadius.Y() + mateRadius.Z() * mateRadius.Z();
	collisionInertia[16] = collisionMassInv * mateInertia.e01 -
			mateRadius.X() * mateRadius.Y();
	collisionInertia[17] = collisionMassInv * mateInertia.e02 -
			mateRadius.X() * mateRadius.Z();

	collisionInertia[18] = collisionMassInv * mateInertia.e11 +
			mateRadius.X() * mateRadius.X() + mateRadius.Z() * mateRadius.Z();
	collisionInertia[19] = collisionMassInv * mateInertia.e12 -
			mateRadius.Y() * mateRadius.Z();

	collisionInertia[20] = collisionMassInv * mateInertia.e22 +
			mateRadius.X() * mateRadius.X() + mateRadius.Y() * mateRadius.Y();

	// Solve for the angular velocity after impact.
	bool result = CCholesky::Solve(collisionInertia, omega, 6);
	thisOmegaInWorld.Set(&omega[0]);
	mateOmegaInWorld.Set(&omega[3]);
	ASSERT (result);

	// Compute and set the new velocities.
	deltaVelocity = (deltaVelocity + thisOmegaInWorld.Cross(thisRadius) -
			mateOmegaInWorld.Cross(mateRadius)) / massSum;
	translationalMomentum /= massSum;

	SetState(mateMass * deltaVelocity + translationalMomentum,
			thisOmegaInWorld / d_cgToWorld);
	i_mate->SetState(-d_mass * deltaVelocity + translationalMomentum,
			mateOmegaInWorld / mateCGToWorld);
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::CoupledCollideWithRigidBodyAsSphere(CRigidBodyIF* i_mate,
		Vector3CRef i_point, float i_cor, float i_cog)
{
	Vector3 normal = i_point - d_cgInWorld;
	if (!normal.SafeNormalize())
		normal.Set(0.0f, 1.0f, 0.0f);
	CoupledCollideWithRigidBody(i_mate, i_point, normal, i_cor, i_cog);
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::GetState(float &o_mass, Matrix3x3 &o_inertia,
		Vector3 &o_cgInWorld, DirCos3x3 &o_cgToWorld,
		Vector3 &o_cgVelocityInWorld, Vector3 &o_omegaInCG) const
{
	o_mass = d_mass;
	o_inertia = d_inertia;
	o_cgInWorld = d_cgInWorld;
	o_cgToWorld = d_cgToWorld;
	o_cgVelocityInWorld = d_cgVelocityInWorld;
	o_omegaInCG = d_omegaInCG;
}

/******************************************************************************/
/******************************************************************************/
void CRigidBody::SetState(Vector3CRef i_cgVelocityInWorld,
		Vector3CRef i_omegaInCG)
{
	d_cgVelocityInWorld = i_cgVelocityInWorld;
	d_omegaInCG = i_omegaInCG;

	actor.d_omegaInBody = d_omegaInCG * d_cgToBody;
	actor.d_velocityInWorld = d_cgVelocityInWorld -
			actor.d_omegaInBody.Cross(d_cgInBody) * actor.d_bodyToWorld;
	actor.SyncState();
}

/******************************************************************************/
/******************************************************************************/
Vector3 CRigidBody::GetPointVelocity(Vector3CRef i_pointInWorld) const
{
	return(d_cgVelocityInWorld +
			(d_cgInWorld - i_pointInWorld).Cross(d_omegaInCG * d_cgToWorld));
}


/******************************************************************************/
/******************************************************************************/
float CRigidBody::ComputeKineticEnergy(void)
{
	return 0.5f * (d_mass *	d_cgVelocityInWorld.Dot(d_cgVelocityInWorld) +
			d_omegaInCG.Dot(d_omegaInCG * d_inertia));
}

