/*
**
**  File:   RigidBody.h
**  Date:   June 28, 2004
**  By:     Bryant Collard
**  Desc:   Models 6 DOF rigid body dynamics.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Components/RigidBody.h $
**  $Revision: #10 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#pragma once

#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include "Components/Component.h"
#include "Interfaces/RigidBodyIF.h"
#include "platform/BaseType.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"

class CStageEntity;
class CCollisionEvent;

class CRigidBody : public CActorComponent, public CRigidBodyIF
{
  public:
	// Construction
	CRigidBody(CCompActor &i_actor);
	virtual const char* ComponentName(void) {return("RigidBody");}

	// Get at interfaces
	virtual CInterface* GetInterface(uint i_id);

	// Initialization
	void SetMass(float i_mass);
	void SetInertia(const Matrix3x3 &i_inertia);
	void SetInertia(float i_Ixx, float i_Iyy, float i_Izz);
	void SetCG(Vector3CRef i_cgInBody, const DirCos3x3 &i_cgToBody);
	void SetBody(Vector3CRef i_cgInBody, const DirCos3x3 &i_cgToBody);
	void SetBodyAcceleration(Vector3CRef i_bodyAcc, bool i_autoApply = false);
	void SetBodyAccelerationAutoApply(bool i_autoApply)
		{d_autoApply = i_autoApply;}

	// Access state
	const Vector3& GetCGInBody(void) const {return(d_cgInBody);}
	const Vector3& GetCGInWorld(void) const {return(d_cgInWorld);}
	const Vector3& GetCGVelocityInWorld(void) const {return(d_cgVelocityInWorld);}
	Vector3 GetTotalForce(void) const {return(d_forceSum * d_cgToBody);}
	Vector3 GetTotalTorque(void) const {return(d_torqueSum * d_cgToBody);}

	// Set state relative to CG
	void SetCGInWorld(Vector3CRef i_cgInWorld);
	void SetCGToWorld(const DirCos3x3 &i_cgToWorld);
	void SetCGVelocityInWorld(Vector3CRef i_cgVelocityInWorld);
	void SetAngVelocityInCG(Vector3CRef i_omegaInCG);
	float ComputeKineticEnergy(void);

	// Apply forces and torques
	void ApplyForceCG(Vector3CRef i_force);
	void ApplyForce(Vector3CRef i_force, Vector3CRef i_loc);
	void ApplyTorque(Vector3CRef i_torque);

	// Manage component
	virtual void Initialize(void);
	virtual void Activate(void);
	virtual void Reset(void);
	virtual void SyncActor(void);

	// Update
	virtual void BeginUpdate(void);
	virtual void AttemptUpdate(float i_deltaSec);
	virtual void EndUpdate(float i_deltaSec);

	// Collision handling
	virtual bool CoupledCollisionDetect(CStageEntity* i_entity,
			CCollisionEvent* i_event, uint &o_myEffect, uint &o_otherEffect);
	virtual uint CollisionDetect(CStageEntity* i_entity,
			CCollisionEvent* i_event, bool i_mateDisplaced);

	// Rigid Body interface.
	virtual void ApplyExternalForce(Vector3CRef i_forceInWorld,
			Vector3CRef i_locInWorld);
	virtual void ApplyExternalTorque(Vector3CRef i_torqueInWorld);
	virtual void Displace(Vector3CRef i_displacement);
	virtual void DumpDisplacementEnergy(void);
	virtual void CollideWithMassive(Vector3CRef i_point, Vector3CRef i_normal,
			Vector3CRef i_velocity, float i_cor, float i_cog);
	virtual void CollideWithMassiveAsSphere(Vector3CRef i_point,
			Vector3CRef i_velocity, float i_cor, float i_cog);
	virtual Vector3 CollideWithPointMass(Vector3CRef i_point,
			Vector3CRef i_normal, float i_mass, Vector3CRef i_velocity,
			float i_cor, float i_cog);
	virtual Vector3 CollideWithPointMassAsSphere(Vector3CRef i_point,
			float i_mass, Vector3CRef i_velocity,
			float i_cor, float i_cog);
	virtual void CoupledCollideWithRigidBody(CRigidBodyIF* i_mate,
			Vector3CRef i_point, Vector3CRef i_normal, float i_cor, float i_cog);
	virtual void CoupledCollideWithRigidBodyAsSphere(CRigidBodyIF* i_mate,
			Vector3CRef i_point, float i_cor, float i_cog);
	virtual void GetState(float &o_mass, Matrix3x3 &o_inertia,
			Vector3 &o_cgInWorld, DirCos3x3 &o_cgToWorld,
			Vector3 &o_cgVelocityInWorld, Vector3 &o_omegaInCG) const;
	virtual void SetState(Vector3CRef i_cgVelocityInWorld,
			Vector3CRef i_omegaInCG);
	virtual Vector3 GetPointVelocity(Vector3CRef i_pointInWorld) const;

  private:
	// Characteristics
	float d_mass;
	float d_massInv;
	Matrix3x3 d_inertia;
	Matrix3x3 d_inertiaInv;
	Vector3 d_cgInBody;
	DirCos3x3 d_cgToBody;

	// Forces
	Vector3 d_forceSum;
	Vector3 d_torqueSum;
	Vector3 d_externalForceSum;
	Vector3 d_externalTorqueSum;

	// State
	Vector3 d_cgInWorld;
	DirCos3x3 d_cgToWorld;
	Vector3 d_cgVelocityInWorld;
	Vector3 d_omegaInCG;

	// Beginning State
	Vector3 d_beginCGInWorld;
	DirCos3x3 d_beginCGToWorld;
	Vector3 d_beginCGVelocityInWorld;
	Vector3 d_beginOmegaInCG;

	// Displacement Energy
	bool DumpDisplacementEnergy(Vector3 &io_cgVelocityInWorld,
			Vector3 &io_omegaInCG);
	float d_deltaSec;
	float d_displacementEnergy;
	float d_bodyAccMag;
	Vector3 d_bodyAccDir;
	bool d_autoApply;
};

#endif // RIGID_BODY_H
