//============================================================================
//=
//= Teleport.h - Handles a teleport message and applies the teleport during
//=              the appropriate update phase
//=
//============================================================================

#ifndef _TELEPORT_H
#define _TELEPORT_H

#include "Components/Component.h"
#include "GameHelper/Message.h"
#include "GameHelper/MessageDefines.h"

//============================================================================

class CTeleport : public CActorComponent
{
public:
	// component base stuff
	CTeleport( CCompActor& i_actor );
	virtual ~CTeleport();
	const char *ComponentName()  { return "Teleport"; }
	virtual void AttemptUpdate( float i_deltaSec );

private:
	Vector3	d_queuedTarget;
	float		d_queuedHeading;
	bool		d_queuedTeleport;
	float		d_timer;

	REF_BASIC_MESSAGE_HANDLER( TeleportMessage, MESSAGE_ACTOR_HANDLE, CTeleport) d_teleportHandler;
//	REF_BASIC_MESSAGE_HANDLER( TeleportMessage, MESSAGE_VECTOR3_PTR, CTeleport ) d_teleportHandler;
};

//============================================================================

#endif //_TELEPORT_H
