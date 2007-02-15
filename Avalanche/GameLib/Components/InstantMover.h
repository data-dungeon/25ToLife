//============================================================================
//=
//= InstantMover.h - A component to handle rudamentary things such as rotating,
//=                scaling, point to point movement, etc.
//=
//============================================================================

#ifndef _INSTANT_MOVER_H
#define _INSTANT_MOVER_H

#include "Components/Component.h"
#include "Interfaces/BasicMoverIF.h"

//============================================================================

class CInstantMover : public CActorComponent, public CBasicMoverIF
{
public:
	// component base stuff
	CInstantMover(CCompActor& i_actor);
	virtual ~CInstantMover();
	const char *ComponentName() {return("InstantMover");}
	virtual void BeginUpdate(void);
	virtual void AttemptUpdate(float i_deltaSec);
	virtual void EndUpdate(float i_deltaSec);

	// Get at interfaces
	virtual CInterface* GetInterface(uint i_id);

	// translational movement to absolute position
	virtual void MoveTo(Vector3CRef pos, float duration);
	virtual void MoveToX(float pos, float duration);
	virtual void MoveToY(float pos, float duration);
	virtual void MoveToZ(float pos, float duration);

	// translational movement to relative position
	virtual void MoveRel(Vector3CRef pos, float duration);
	virtual void MoveRelX(float pos, float duration);
	virtual void MoveRelY(float pos, float duration);
	virtual void MoveRelZ(float pos, float duration);

	// rotate to absolute position
	virtual void RotateTo(Vector3CRef rot, float duration);
	virtual void RotateToX(float rot, float duration);
	virtual void RotateToY(float rot, float duration);
	virtual void RotateToZ(float rot, float duration);

	// rotate to relative position
	virtual void RotateRel(Vector3CRef rot, float duration);
	virtual void RotateRelX(float rot, float duration);
	virtual void RotateRelY(float rot, float duration);
	virtual void RotateRelZ(float rot, float duration);

	// apply constant velocities
	virtual void MoveAtConstantVelocity(Vector3CRef vel);
	virtual void RotateAtConstantVelocity(Vector3CRef vel);

private:
	uint d_flags;
	Vector3 d_beginBodyInWorld;
	DirCos3x3 d_beginBodyToWorld;
	Vector3 d_bodyInWorld;
	Vector3 d_euler;
	Vector3 d_velocityInWorld;
	Vector3 d_omegaInBody;
};

//============================================================================

#endif //_INSTANT_MOVER_H
