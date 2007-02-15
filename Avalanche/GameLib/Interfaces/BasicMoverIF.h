//============================================================================
//=
//= BasicMoverIF.h - Interface for basic movement, such as rotating and
//=                  and translation.
//=
//============================================================================

#ifndef _BASICMOVERIF_H
#define _BASICMOVERIF_H

#include "Interfaces/Interface.h"
#include "GameHelper/Handle.h"

class CActor;
handledef(CActor) ActorHandle;

class CBasicMoverIF : public CInterface
{
public:
	// interface ID
	static uint GetID(void) { return CInterface::GetValidID( s_basicMoverID ); }
	static CBasicMoverIF* GetInterface(CActor* i_actor);
	static CBasicMoverIF* GetInterface(ActorHandle i_handle);

	// translational movement to absolute position
	virtual void MoveTo( Vector3CRef pos, float duration ) = 0;
	virtual void MoveToX( float pos, float duration ) = 0;
	virtual void MoveToY( float pos, float duration ) = 0;
	virtual void MoveToZ( float pos, float duration ) = 0;

	// translational movement to relative position
	virtual void MoveRel( Vector3CRef pos, float duration ) = 0;
	virtual void MoveRelX( float pos, float duration ) = 0;
	virtual void MoveRelY( float pos, float duration ) = 0;
	virtual void MoveRelZ( float pos, float duration ) = 0;

	// absolute rotational movement.
	virtual void RotateTo( Vector3CRef rot, float duration ) = 0;
	virtual void RotateToX( float rot, float duration ) = 0;
	virtual void RotateToY( float rot, float duration ) = 0;
	virtual void RotateToZ( float rot, float duration ) = 0;

	// relative rotational movement.
	virtual void RotateRel( Vector3CRef rot, float duration ) = 0;
	virtual void RotateRelX( float rot, float duration ) = 0;
	virtual void RotateRelY( float rot, float duration ) = 0;
	virtual void RotateRelZ( float rot, float duration ) = 0;

	// apply constant velocities
	virtual void MoveAtConstantVelocity( Vector3CRef vel ) = 0;
	virtual void RotateAtConstantVelocity( Vector3CRef vel ) = 0;

private:
	static uint s_basicMoverID;
};

#endif //_BASICMOVERIF_H

//============================================================================
