/*
**
**  File:   RigidBodyIF.h
**  Date:   July 2, 2004
**  By:     Bryant Collard
**  Desc:   An interface to a rigid body.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Interfaces/RigidBodyIF.h $
**  $Revision: #7 $
**  $DateTime: 2005/03/29 12:30:12 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef RIGID_BODY_IF_H
#define RIGID_BODY_IF_H

#include "Interfaces/Interface.h"
#include "Math/Vector.h"
#include "GameHelper/Handle.h"

class CActor;
handledef(CActor) ActorHandle;

class CRigidBodyIF : public CInterface
{
  public:
	static uint GetID(void) {return(CInterface::GetValidID(s_rigidBodyID));}
	static CRigidBodyIF* GetInterface(CActor* i_actor);
	static CRigidBodyIF* GetInterface(ActorHandle i_handle);

	// Initialization
	virtual void SetMass(float i_mass) {}
	virtual void SetInertia(const Matrix3x3 &i_inertia) {}
	virtual void SetInertia(float i_Ixx, float i_Iyy, float i_Izz) {}
	virtual void SetCG(Vector3CRef i_cgInBody, const DirCos3x3 &i_cgToBody) {}
	virtual void SetBody(Vector3CRef i_cgInBody, const DirCos3x3 &i_cgToBody) {}
	virtual void SetBodyAcceleration(Vector3CRef i_bodyAcc,
			bool i_autoApply = false) {}
	virtual void SetCGVelocityInWorld(Vector3CRef i_bodyVel) {}
	virtual void SetAngVelocityInCG(Vector3CRef i_bodyVel) {}
	virtual float ComputeKineticEnergy() { return 0.0f; }
	
	// Interactons
	virtual void ApplyExternalForce(Vector3CRef i_forceInWorld,
			Vector3CRef i_locInWorld) {}
	virtual void ApplyExternalTorque(Vector3CRef i_torqueInWorld) {}

	// Respond to collisions. Generally called in the context of
	// CollisionDetect() but can be called directly to "hit" a rigid body if
	// careful.
	//
	// In order to respond to any collision, a rigid body needs to know where the
	// impact occured and the orientation of the plane (defined by a vector
	// normal to the plane pointing towards the rigid body) that is tangent to
	// the surfaces of both participants in the collision at the impact
	// point. The point and normal are both expected to be in world
	// coordinates. The "AsSphere" variants do not take a normal as an
	// argument. They compute a normal by behaving as if the rigid body is a
	// sphere centered at the center of mass and the impact point lies on its
	// surface; in other words, the normal points from the impact point to the
	// center of mass.
	//
	// Respond to a collision with an infinitely massive body. The velocity is
	// the velocity of a point on the massive body at the impact point.
	virtual void CollideWithMassive(Vector3CRef i_point, Vector3CRef i_normal,
			Vector3CRef i_velocity, float i_cor, float i_cog) {}
	virtual void CollideWithMassiveAsSphere(Vector3CRef i_point,
			Vector3CRef i_velocity, float i_cor, float i_cog) {}

	// Respond to a collision with a point mass located at the impact point
	// with the provided velocity. The velocity of the point mass after the
	// impact is returned.
	virtual Vector3 CollideWithPointMass(Vector3CRef i_point,
			Vector3CRef i_normal, float i_mass, Vector3CRef i_velocity,
			float i_cor, float i_cog) {return(i_velocity);}
	virtual Vector3 CollideWithPointMassAsSphere(Vector3CRef i_point,
			float i_mass, Vector3CRef i_velocity,
			float i_cor, float i_cog) {return(i_velocity);}

	// Perform a coupled response to a collision with another rigid body. The
	// states of both bodies are modified.
	virtual void CoupledCollideWithRigidBody(CRigidBodyIF* i_mate,
			Vector3CRef i_point, Vector3CRef i_normal, float i_cor, float i_cog) {}
	virtual void CoupledCollideWithRigidBodyAsSphere(CRigidBodyIF* i_mate,
			Vector3CRef i_point, float i_cor, float i_cog) {}

	// Coupled response support.
	virtual void Displace(Vector3CRef i_displacement) {}
	virtual void DumpDisplacementEnergy(void) {}
	virtual void GetState(float &o_mass, Matrix3x3 &o_inertia,
			Vector3 &o_cgInWorld, DirCos3x3 &o_cgToWorld,
			Vector3 &o_cgVelocityInWorld, Vector3 &o_omegaInCG) const {}
	virtual void SetState(Vector3CRef i_cgVelocityInWorld,
			Vector3CRef i_omegaInCG) {}
	virtual Vector3 GetPointVelocity(Vector3CRef i_pointInWorld) const
		{return(Vector3(0.0f, 0.0f, 0.0f));}

  private:
	static uint s_rigidBodyID;
};

#endif // RIGID_BODY_IF_H
