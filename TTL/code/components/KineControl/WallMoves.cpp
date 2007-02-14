#include "TTLPCH.h"

#include "BipedKineControl.h"
#include "WallMoves.h"
#include "Components/ComponentCreator.h"
#include "Components/InteractionMonitor.h"
#include "Script/ScriptVM.h"

#include "data_h_files/players/players_s.h"
#include "Math/sfilter.h"

// Define creators and add them to the creator list.
STANDARD_COMPONENT_CREATOR(WallVault, WallVault, COMPONENT_DRIVER_PRIORITY);

void WallGetHeights( Vector3CRef center, Vector3CRef x, Vector3CRef y, Vector3CRef z, Vector3CRef target, CActor *ignore, float &o_nearHeight, float &o_farHeight, bool &farFall )
{
	Vector3 pointInBox, xN, yN, zN;
	float dX;
	bool inFront = false;

	xN = x.Normalized(); yN = y.Normalized(); zN = z.Normalized();
	dX = (target - center).Dot( xN );

	// get the point along the x axis of the box relative to the target, at the top of the box
	pointInBox = center + (dX * xN) + y;

	if( (pointInBox - target).Dot( z ) < 0.0f )
		inFront = true;

	float nearsign, farsign;
	if( inFront )
		nearsign = 1.0f;
	else
		nearsign = -1.0f;
	farsign = nearsign * -1.0f;

	// figure out how high the wall is on either side.

	// on the near side, just use the difference between the target height and the top of the wall
	Vector3 top = center + y;
	o_nearHeight = top.y() - target.y();

	CLineEntity line;
	CLineQuery query;

	top = pointInBox + ( ( (z.Length() + 0.1f) * farsign ) * zN );
	line.Line().SetOriginBasis( top, -yN );
	line.Line().SetMaxBoundary( 4.0f );
	line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	line.SetIgnoreEntity( ignore );

//	g_collisionSystem.ResetStack();

	if( query.Test( line ) )
	{
		farFall = false;
		o_farHeight = ( query.intersection - top ).Length();
	}
	else
	{
		farFall = true;
		o_farHeight = o_nearHeight;
	}
}

InteractionMonitor::InteractionType WallInteractionQuery( CCompActor &queryActor, void *targetObject, 
		InteractionMonitor::InteractionObjectType objType, InteractionMonitor::InteractionType type )
{
	// check to see if interaction with this object should be allowed
	OBB *obb = NULL;
	if( objType == InteractionMonitor::OBJECT_ACTOR )
	{
		CActor *targetActor = (CActor *)targetObject;
		CCollisionObject *obj = targetActor->GetCollisionObject();
		ASSERT( obj->GetObjectType() == CCollisionObject::OBB_OBJECT );
		obb = (OBB *)obj;
	}
	else if( objType == InteractionMonitor::OBJECT_STATICOBB )
	{
		obb = (OBB *)targetObject;
	}
	ASSERT( type == InteractionMonitor::INTERACTION_VAULT ); // we should only be checking for vault interactions
	
	if( obb == NULL )
		return InteractionMonitor::INTERACTION_NONE;

	// only allowed if the actor is not past the end of our side axis
	Vector3 center, x, y, z, xN;
	obb->ComputeWorldCoords( center, x, y, z );

	float maxX = x.LengthSquared();
	xN = x.Normalized();
	float distToActor;
	Vector3 toActor = queryActor.GetBodyInWorld() - center;
	// disallow interacting with the outside .5 meters too
	distToActor = Math::Abs( toActor.Dot( xN ) ) + 0.5f;
	distToActor *= distToActor;
	
	if( distToActor > maxX )
		return InteractionMonitor::INTERACTION_NONE;

	// is the near side too high?
	float nearHeight, farHeight;
	bool farFall;
	WallGetHeights( center, x, y, z, queryActor.GetBodyInWorld(), &queryActor, nearHeight, farHeight, farFall );
	if( nearHeight >= 2.0f )
		return InteractionMonitor::INTERACTION_CLIMB;

	return InteractionMonitor::INTERACTION_VAULT;
}

void WallVault::ManDown::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	if( d_ref->IsActive() && i_sender == d_ref->GetActor().GetHandle() )
		d_ref->AbortVault();
}

WallVault::WallVault( CCompActor &i_actor ) : CActorComponent( i_actor )
{
	actor.AddProperty( "heightOffset", Property_float );
	actor.SetProperty( "heightOffset", 0.0f );
	actor.AddProperty( "stayUpright", Property_bool );
	actor.SetProperty( "stayUpright", false );

	currentVault = NULL;
	state = MOVING_TO_VAULT;
}

WallVault::~WallVault()
{
}

void WallVault::SetVault( CActor *vaultActor )
{
	CCollisionObject *obj = vaultActor->GetCollisionObject();
	ASSERT( obj->GetObjectType() == CCollisionObject::OBB_OBJECT );
	SetVault( (OBB *)obj );

	
	ScriptVM *vm = vaultActor->Script( AI_SCRIPT );
	if( vm )
		animIndex = vm->GetInt( "AnimIndex" );

}

void WallVault::SetVault( OBB *vault )
{ 
	animIndex = -1;
	currentVault = vault;

	Vector3 center, x, y, z;
	currentVault->ComputeWorldCoords( center, x, y, z );

	WallGetHeights( center, x, y, z, GetActor().GetBodyInWorld(), &(GetActor()), nearHeight, farHeight, farFall );
	if( nearHeight >= 2.0f )
	{
		if( z.LengthSquared() > ( 0.2f * 0.2f ) )
			vaultType = CLIMBUP;
		else
			vaultType = CLIMBOVER;
		baseHeight = 2.0f;
	}
	else
	{
		vaultType = VAULT;
		baseHeight = 1.0f;
	}
}

void WallVault::Initialize( void )
{
	CActorComponent::Initialize();
	InteractionMonitor *monitor = (InteractionMonitor *)GetActor().GetComponentByName( "InteractionMonitor" );
	if( monitor )
		monitor->SetInteractionQueryCallback( InteractionMonitor::INTERACTION_VAULT, WallInteractionQuery );
	d_manDownHandler.Initialize("ManDown", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
}

void WallVault::Activate( void )
{
	if( IsActive() )
		return;

	g_messageDispatcher.SendMessageToActor( "StopPeekLeft", NULL, actor.GetHandle(), actor.GetHandle() );
	g_messageDispatcher.SendMessageToActor( "StopPeekRight", NULL, actor.GetHandle(), actor.GetHandle() );

	GetActor().SetPositiveExclude( TERRAIN_BIT | STATIC_BIT | ACTOR_BIT );

	reactivateController = NULL;
	reactivateDriver = GetActor().GetActiveComponentInClass( "KinematicDriver" );
	if( reactivateDriver )
		reactivateDriver->Deactivate();

	CActorComponent::Activate();

	ASSERT( currentVault && "Must have a vault assigned" );
	state = MOVING_TO_VAULT;
	speed = GetActor().GetVelocityInWorld().Length();
}

void WallVault::Deactivate( void )
{
	// need to differentiate between being deactivated because we're inactive by default,
	// or being deactivated because we were active and now need to not be.
	if( IsActive() )
	{
		// we need to offset the animation position
		Vector3 face = actor.GetBodyToWorld().GetRow2();
		Vector3 up = actor.GetBodyToWorld().GetRow1();
		face *= facingTravel;
		up *= upTravel + GetActor().GetProperty( "heightOffset" )->GetFloat();
		actor.GetBodyInWorld() += face;
		actor.GetBodyInWorld() += up;

		actor.SetDiscontinuity( DISCONTINUITY_POSITION );
		actor.SyncState();
		actor.PlaceGeometry();
		
		if( actor.GetProperty( "updateLevel" )->GetInt() == 0 )
			actor.SetPositiveExclude( TERRAIN_CATEGORY | STATIC_CATEGORY );
		else if( actor.GetProperty( "updateLevel" )->GetInt() == 1 )
			actor.SetPositiveExclude( NO_CATEGORY );

		if( reactivateDriver )
			reactivateDriver->Activate();
		if( reactivateController )
			reactivateController->Activate();
	}
	GetActor().SetProperty( "heightOffset", 0.0f );
	GetActor().SetProperty( "stayUpright", true );

	CActorComponent::Deactivate();
}

void WallVault::AbortVault( void )
{
	Vector3 offset = actor.GetStartPosition( (BoneID) 0 );
	Vector3 pos = actor.GetBodyInWorld() + offset;
	actor.GetBodyInWorld() = pos;
	Deactivate();
	actor.GetBodyInWorld() = pos;
	actor.SetProperty( "onGround", false );
	actor.SyncState();
	actor.PlaceGeometry();
	actor.SyncActor();
}

void WallVault::BeginFrame( void )
{
	if (!IsActive())
		return;

	if( state == MOVING_TO_VAULT )
	{
		// are we close enough to vault?
		if( 1 )
		{
			reactivateController = GetActor().GetActiveComponentInClass( "KinematicController" );
			if( reactivateController )
				reactivateController->Deactivate();
			if( vaultType == VAULT )
				g_messageDispatcher.SendMessageToActor( "Vault", (void *)animIndex, actor.GetHandle(), actor.GetHandle() );
			else if( vaultType == CLIMBUP )
				g_messageDispatcher.SendMessageToActor( "ClimbUp", NULL, actor.GetHandle(), actor.GetHandle() );
			else if( vaultType == CLIMBOVER )
				g_messageDispatcher.SendMessageToActor( "ClimbOver", NULL, actor.GetHandle(), actor.GetHandle() );
			GetActor().SetProperty( "heightOffset", 0.0f );
			// need to wait one frame before actually locking onto the vault, so we can get the correct
			// values out of the motion header
			state = LOCK_TO_VAULT;
		}
	}
	else if( state == LOCK_TO_VAULT && GetActor().AnimAction() == ACTION_ROAM_VAULT )
	{
		LockToVault();
		state = VAULTING;
	}
	else if( state == VAULTING && GetActor().AnimAction() == ACTION_ROAM_VAULT )
	{
		static float filterconstant = 0.2f;
		int f0, f1, f2, f3, totalframes;
		float time, t0, t1, t2, t3, totaltime;
		float heightOffset = GetActor().GetProperty( "heightOffset" )->GetFloat();

		f0 = GetActor().GetMotionInt( "StartUp" );
		f1 = GetActor().GetMotionInt( "EndUp" );
		f2 = GetActor().GetMotionInt( "StartDown" );
		f3 = GetActor().GetMotionInt( "EndDown" );
		totalframes = CMotion__GetAnimFrames( GetActor().GetMotnInf( MOTION1 )->pMotionHeader );

		time = CMotion__GetCurrentTime( GetActor().GetMotnInf( MOTION1 ) );
		totaltime = CMotion__GetAnimTime( GetActor().GetMotnInf( MOTION1 )->pMotionHeader );
		t0 = totaltime * ( (float)f0 / totalframes );
		t1 = totaltime * ( (float)f1 / totalframes );
		t2 = totaltime * ( (float)f2 / totalframes );
		t3 = totaltime * ( (float)f3 / totalframes );

		if( time < t0 )
		{
			heightOffset = 0.0f;
		}
		else if( time < t1 )
		{
			float t = 1.0f - (time / t1);
			float p0, p1, a;

			p0 = 0.0f;
			p1 = nearHeight - baseHeight;

			a = (p1 - p0);
			heightOffset = p1 - (a * t * t);
		}
		else
		{
			if( vaultType == CLIMBUP ) // we're done with height offsets
				heightOffset = nearHeight - baseHeight;
			else if( time >= t1 && time < t2 )
			{
				heightOffset = nearHeight - baseHeight;
			}
			else if( time >= t2 && time < t3 )
			{
				float t = (time - t2) / (t3 - t2);
				float p0, p1, a;

				p0 = nearHeight - baseHeight;
				p1 = nearHeight - farHeight;
				
				a = (p1 - p0);
				heightOffset = p0 + (a * t * t);
			}
			else // time > t3
			{
				heightOffset = nearHeight - farHeight;
			}
		}

		GetActor().SetProperty( "heightOffset", heightOffset );
	}
}

Vector3 WallVault::GetPointInBox( Vector3CRef center, Vector3CRef x, Vector3CRef y, Vector3CRef z, Vector3CRef target )
{
	Vector3 toTarget, xN, yN, zN;
	float dX, dY;
	xN = x.Normalized(); yN = y.Normalized(); zN = z.Normalized();

	toTarget = target - center;
	dX = toTarget.Dot( xN );
	dY = toTarget.Dot( yN );

	Vector3 posInBox = center + (xN * dX) + (yN * dY);
	
	return posInBox;
}

void WallVault::LockToVault( void )
{
	ASSERT( GetActor().AnimAction() == ACTION_ROAM_VAULT );
	ASSERT( currentVault );

	Vector3 center, x, y, z, xN, yN, zN;
	float dZ;
	currentVault->ComputeWorldCoords( center, x, y, z );

	ASSERT( x.Length() >= 0.5f && y.Length() >= 0.6125f && z.Length() >= 0.05f && 
			"OBBs marked as Vaultable must be at least 1 meter long along their sideways axis, should be "
			"over 1.25 meters high, and must have some significant depth (over 0.05 meters)" );
	
	xN = x.Normalized(); yN = y.Normalized(); zN = z.Normalized();

	Vector3 posInBox = GetPointInBox( center, x, y, z, GetActor().GetBodyInWorld() );
	Vector3 boxZ, toBox;
	bool inFront = false;
	toBox = posInBox - GetActor().GetBodyInWorld();
	boxZ = currentVault->Matrix()->GetRow2();
	if( boxZ.Dot( toBox ) < 0.0f )
		inFront = true;
	
	if( !inFront )
		zN = -zN;

	Vector3 posOnBox = posInBox + (z.Length() * zN);

	dZ = GetActor().GetMotionFloat( "DistFromWall" );
	GetActor().GetBodyInWorld() = posOnBox + (zN * dZ);

	GetActor().GetBodyToWorld() = *(currentVault->Matrix());
	if( inFront )
	{
		// we're on the front side of the box, which means we need to turn
		// around in order to be facing it
		GetActor().GetBodyToWorld().CatYRotation( Math::Deg2Rad( 180.0f ) );
	}

	WallGetHeights( center, x, y, z, GetActor().GetBodyInWorld(), &(GetActor()), nearHeight, farHeight, farFall );

	facingTravel = GetActor().GetMotionFloat( "FacingTravel" );
	baseHeight = GetActor().GetMotionFloat( "WallHeight" );
	upTravel = GetActor().GetMotionFloat( "UpTravel" );
	float motionHeightOffset = GetActor().GetMotionFloat( "HeightOffset" );
	nearHeight += motionHeightOffset;
	farHeight += motionHeightOffset;
}
