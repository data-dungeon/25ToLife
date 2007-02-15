//============================================================================
//=
//= Teleport.cpp - A component to handle rudamentary things such as rotating,
//=                  scaling, point to point movement, etc.
//=
//============================================================================

#include "Components/ComponentsPCH.h"
#include "Components/Teleport.h"
#include "Components/ComponentCreator.h"
#include "CollideHelper/LineQuery.h"
#include "Game/Managers/RenderMgr.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(CTeleport, Teleport, COMPONENT_NEUTRAL_PRIORITY);

//============================================================================

CTeleport::CTeleport( CCompActor& i_actor ) :
	CActorComponent( i_actor )
{
	d_queuedTarget.Set( 0.0f, 0.0f, 0.0f );
	d_queuedHeading  = 0.0f;
	d_queuedTeleport = false;
	d_timer = 0.0f;

	d_teleportHandler.Initialize( "Teleport",
											MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER,
											i_actor.GetMessageOwner(), this );
}

//============================================================================

CTeleport::~CTeleport()
{
}

//============================================================================

void CTeleport::AttemptUpdate( float i_deltaSec )
{
	if ( d_timer > 0.0f )				// if set
	{
		d_timer -= i_deltaSec;			// - wait for timer countdown before doing Teleport (allows time for fadeout)
	}
	else if ( d_queuedTeleport )
	{
		GetActor().Place( d_queuedTarget, d_queuedHeading );
		GetActor().GetVelocityInWorld().Set( 0.0f, 0.0f, 0.0f );
		d_queuedTeleport = false;

		RenderMgr__FadeIn( FADE_TO_BLACK_FAST );
	}
}

//============================================================================

void CTeleport::TeleportMessage::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	d_ref->d_timer = 0.1f;
	RenderMgr__FadeOut( FADE_TO_BLACK_FAST );

	CActor *destination = CActor::FromHandle((uint32)i_data);

	Vector3	target = destination->GetBodyInWorld();
	Vector3  facing = destination->GetBodyToWorld().GetRow2();

	d_ref->d_queuedTarget   = target;
	d_ref->d_queuedHeading  = Math::ArcTan2( facing.x(), facing.z() );
	d_ref->d_queuedTeleport = true;

	// adjust target to nearby ground if we can find it
	CLineEntity line;
	Vector3 start = Vector3( target.x(), target.y() + 0.2f, target.z() );
	Vector3 end   = Vector3( target.x(), target.y() - 2.0f, target.z() );
	line.Line().SetOriginEnd( start, end );
	line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	g_collisionSystem.ResetStack();
	CLineQuery test;
	if ( test.Test(line) )
	{
		d_ref->d_queuedTarget.y( test.intersection.y() );
	}
}

/*void CTeleport::TeleportMessage::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	Vector3* target = (Vector3*) i_data;
	Vector3  facing = d_ref->GetActor().GetBodyToWorld().GetRow2();
	d_ref->d_queuedTarget   = *target;
	d_ref->d_queuedHeading  = Math::ArcTan2( facing.x(), facing.z() );
	d_ref->d_queuedTeleport = true;

	// adjust target to nearby ground if we can find it
	CLineEntity line;
	Vector3 start = Vector3( target->x(), target->y() + 0.2f, target->z() );
	Vector3 end   = Vector3( target->x(), target->y() - 2.0f, target->z() );
	line.Line().SetOriginEnd( start, end );
	line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	g_collisionSystem.ResetStack();
	CLineQuery test;
	if ( test.Test(line) )
	{
		d_ref->d_queuedTarget.y( test.intersection.y() );
	}
}
//*/

//============================================================================
