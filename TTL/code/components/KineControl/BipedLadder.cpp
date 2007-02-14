#include "TTLPCH.h"

#include "BipedLadder.h"
#include "Components/InteractionMonitor.h"
#include "data_h_files/players/players_s.h"
#include "Components/ComponentCreator.h"
#include "Components/GroundHeight.h"

// Define creators and add them to the creator list.
STANDARD_COMPONENT_CREATOR(BipedLadderController, BipedLadder,
		COMPONENT_CONTROLLER_PRIORITY);
STANDARD_COMPONENT_CREATOR(LadderPhysicalControllerDriver,
		LadderPhysicalController, COMPONENT_DRIVER_PRIORITY);

typedef struct tag_BipedLadderDefaultParams
{
	float maxClimb;
	float distFromLadder;
	float topClimbOff;
	float bottomClimbOff;
} BipedLadderDefaultParams;

BipedLadderDefaultParams BipedLadderDefaults;

void CParam<BipedLadderDefaultParams>::RegisterVariables()
{
	RegisterVariable( d_data.maxClimb, "maxClimb", 2.0f, 0.0f, 20.0f );
	RegisterVariable( d_data.distFromLadder, "distFromLadder", 0.5f, 0.0f, 2.0f );
	RegisterVariable( d_data.topClimbOff, "topClimbOff", 2.0f, 0.0f, 5.0f );
	RegisterVariable( d_data.bottomClimbOff, "bottomClimbOff", 0.1f, 0.0f, 1.0f );
}

void BipedLadderController::OffLadder::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	d_ref->GetOffLadder( (int)i_data );
}

void BipedLadderController::ManDown::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	bool inLadderMode = false;

	if( ACTION_TYPE_MASK( d_ref->GetActor().AnimAction() ) == ACTION_ROAM_LADDER )
		inLadderMode = true;
	if( d_ref->IsActive() )
		inLadderMode = true;

	if( inLadderMode && i_sender == d_ref->GetActor().GetHandle() )
		d_ref->GetOffLadder( LadderDismount_Jump );
}

void BipedLadderController::SetDefaults( void )
{
	BipedLadderDefaultParams *defaults = CParam<BipedLadderDefaultParams>::GetParam( "Ladder", "var" );
	
	axisRange[Ladder_Axis_Climb] = AxisRange( defaults->maxClimb );
	distFromLadder = defaults->distFromLadder;
	topClimbOff = defaults->topClimbOff;
	bottomClimbOff = defaults->bottomClimbOff;
}

BipedLadderController::BipedLadderController(CCompActor &i_actor): KinematicController( i_actor )
{
	int i;

	for( i = 0; i < Ladder_Axis_Count; i++ )
		axisPosition[ i ] = 0.0f;

	SetDefaults();

	ladderMaterial = g_surfaceType.GetIndex("ladder");
}

BipedLadderController::~BipedLadderController()
{
	g_console.DeleteContext( "Ladder" );
}

void BipedLadderController::Initialize(void)
{
	d_offLadderHandler.Initialize("OffLadder", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_manDownHandler.Initialize("ManDown", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
}

bool BipedLadderController::CommandVerticalSpeed( float i_speed, bool i_normalized )
{
	if( !IsActive() )
		return false;

	if( i_normalized )
		i_speed = GetAxisRange( Ladder_Axis_Climb ).DeNormalize( i_speed );
	SetAxis( Ladder_Axis_Climb, i_speed );
	return true;
}

void BipedLadderController::Activate( void )
{
	if( IsActive() )
		return;

	CActorComponent::Activate();

	GetActor().SetPositiveExclude( TERRAIN_BIT | STATIC_BIT );

	// figure out where to place the player on the ladder box.
	ASSERT( currentLadder );

	// figure out where to lock them on at
	Vector3 center, x, y, z;
	Vector3 normal, pos, toP;
	currentLadder->ComputeWorldCoords( center, x, y, z );

	normal = y;
	normal.SafeNormalize();
	toP = actor.GetBodyInWorld() - center;
	ladderPosition = toP.Dot( normal );

	float len, top, bottom;
	len = y.Length();
	bottom = -len + bottomClimbOff;
	top = len - topClimbOff;

	if( ladderPosition > top ) // starting over the top, climb down now
		g_messageDispatcher.SendMessageToActor( "LadderClimbDown", NULL, actor.GetHandle(), actor.GetHandle() );

	ladderPosition = Math::Clamp( ladderPosition, bottom, top );

	SetAxis( Ladder_Axis_Climb, 0.0f );

	//LockToLadder();
	//actor.SetDiscontinuity( DISCONTINUITY_POSITION );
}

void BipedLadderController::Deactivate( void )
{
	if( !IsActive() )
		return;

	CActorComponent::Deactivate();

	GetActor().SetPositiveExclude( NO_CATEGORY );
}

void BipedLadderController::AttemptUpdate( float i_deltaSec )
{
	if( !IsActive() )
		return;

	float ladderVel = 0.0f;
	if( GetActor().AnimAction() == ACTION_ROAM_LADDER_SLIDE )
	{
		slideVel += -14.0f * i_deltaSec;
		
		ladderVel = slideVel;
	}
	else
	{
		ladderVel = GetAxis( Ladder_Axis_Climb );
		
		slideVel = 0.0f;
	}
	Vector3 up = GetActor().GetBodyToWorld().GetRow1();

	GetActor().GetVelocityInWorld() = up * ladderVel;
	GetActor().GetBodyInWorld() += GetActor().GetVelocityInWorld() * i_deltaSec;

	ladderPosition += ladderVel * i_deltaSec;
	Vector3 center, x, y, z;
	currentLadder->ComputeWorldCoords( center, x, y, z );
	float len = y.Length();
	if( ladderPosition > len )
	{
		ladderPosition = len;
		GetActor().GetVelocityInWorld().Clear();
	}
	else if( ladderPosition < -len )
	{
		ladderPosition = -len;
		GetActor().GetVelocityInWorld().Clear();
	}
}

void BipedLadderController::LockToLadder( void )
{
	// figure out where to place the player on the ladder box.
	// unless we're playing the ontop of ladder animation, in which case we need
	// to let the BipedModel control the position
	if( actor.AnimAction() != ACTION_ROAM_LADDER_ONTOP )
	{
		ASSERT( currentLadder );

		bool inFront = true;

		Vector3 center, x, y, z;
		Vector3 normal, pos, toP, right;
		currentLadder->ComputeWorldCoords( center, x, y, z );

		toP = actor.GetBodyInWorld() - center;

		normal = -z;
		normal.SafeNormalize();

		// which side should we be on?  
		if( !oneSided )
		{
			right = x;
			right.SafeNormalize();
			right = right * right.Dot( toP );

			if( normal.Dot( toP ) < 0.0f )
			{
				inFront = false;
				normal.Negate();
				z.Negate();
			}
		}
		else
		{
			right.Clear();
		}

		CGroundHeightIF* groundHeight = CGroundHeightIF::GetInterface(actor);
		CGroundHeightIF::SCheckResults* currResults;
		if ((groundHeight != NULL) && groundHeight->Get(currResults))
		{
			CGroundHeightIF::SCheckResults modResults = *currResults;
			modResults.d_normal = normal;
			groundHeight->Set(modResults);
		}

		actor.SetProperty( "groundNormal", normal );
		pos = center - z + ( distFromLadder * normal );
		normal = y;
		normal.SafeNormalize();
		pos += ladderPosition * normal;
		
		pos += right;

		actor.GetBodyInWorld() = pos;

		actor.GetBodyToWorld() = *(currentLadder->Matrix());
		if( !inFront )
			actor.GetBodyToWorld().CatYRotation( Math::Pi );
	}
}

void BipedLadderController::EndUpdate( float i_deltaSec )
{
	if( !IsActive() )
		return;

	CGroundHeightIF* groundHeight = CGroundHeightIF::GetInterface(actor);
	CGroundHeightIF::SCheckResults* currResults;
	if ((groundHeight != NULL) && groundHeight->Get(currResults))
	{
		CGroundHeightIF::SCheckResults modResults = *currResults;
		modResults.d_status = CGroundHeightIF::NO_HIT;
		modResults.d_material = ladderMaterial;
		groundHeight->Set(modResults);
	}

	actor.SetProperty( "onGround", false );
	
	LockToLadder();
}

void BipedLadderController::EndFrame( void )
{
	if( !IsActive() )
		return;

	// Get our model component
	BipedModel* model = (BipedModel*)actor.GetComponentByName("BipedModel");
	ASSERT_PTR(model);

	Vector3 center, x, y, z;
	currentLadder->ComputeWorldCoords( center, x, y, z );

	float len, top, bottom;
	len = y.Length();
	bottom = -len + bottomClimbOff;
	top = len - topClimbOff;


	if( GetActor().AnimAction() == ACTION_ROAM_LADDER_SLIDE )
	{
		if( ladderPosition < bottom )
			model->StopLadderSlide();
	}
	else if( !Math::Zero( GetAxis( Ladder_Axis_Climb ) ) )
	{
		// are we moving down and past the point where we want to automatically get off at the bottom?
		if( GetAxis( Ladder_Axis_Climb ) < 0.0f && ladderPosition < bottom )
		{
			g_messageDispatcher.SendMessageToActor( "LadderOffBottom", NULL, actor.GetHandle(), actor.GetHandle() );
		}
		else if( GetAxis( Ladder_Axis_Climb ) > 0.0f && ladderPosition > top && 
				(GetActor().AnimAction() != ACTION_ROAM_LADDER_ONTOP) )
		{
			SetAxis( Ladder_Axis_Climb, 0.0f );
			// turn off controls while we climb off the top
			GetActor().GetActiveComponentInClass( "KinematicDriver" )->Deactivate();
			int data = 0;
			if( z.LengthSquared() > ( 0.2f * 0.2f ) )
				data = 0;
			else
				data = 1;
			g_messageDispatcher.SendMessageToActor( "LadderOnTop", (void *)data, actor.GetHandle(), actor.GetHandle() );
		}
	}
}

CInterface* BipedLadderController::GetInterface(uint i_id)
{
	if( IsActive() )
	{
		if (i_id == CMoveIF::GetID())
			return( this );
	}
	return(NULL);
}

void BipedLadderController::GetOffLadder( int reason )
{
	// Get our model component
	BipedModel* model = (BipedModel*)actor.GetComponentByName("BipedModel");
	ASSERT_PTR(model);

	Vector3 center, x, y, z;
	currentLadder->ComputeWorldCoords( center, x, y, z );

	model->StopLadder();

	CActorComponent *laddercontroller = GetActor().GetActiveComponentInClass( "KinematicController" );
	CActorComponent *ladderdriver = GetActor().GetActiveComponentInClass( "KinematicDriver" );
	if( laddercontroller )
		laddercontroller->Deactivate();
	if( ladderdriver )
		ladderdriver->Deactivate();

	laddercontroller = GetActor().GetDefaultComponentInClass( "KinematicController" );
	ladderdriver = GetActor().GetDefaultComponentInClass( "KinematicDriver" );
	
	if( laddercontroller )
		laddercontroller->Activate();
	if( ladderdriver )
		ladderdriver->Activate();

	CActorComponent *col = GetActor().GetDefaultComponentInClass( "CollisionBehavior" );
	if( col )
		col->Activate();

	if( reason == LadderDismount_Jump )	
		actor.d_bodyInWorld += actor.GetStartPosition( (BoneID)0 );

	actor.SyncState();
	actor.SetDiscontinuity( DISCONTINUITY_POSITION );

	if( reason != LadderDismount_Jump && z.LengthSquared() > (0.2f * 0.2f) )
		actor.SetProperty( "onGround", true );
}

void LadderPhysicalControllerDriver::BeginFrame( void )
{
	if( !IsActive() )
		return;

	if( skipControllerRead )
	{
		skipControllerRead = false;
		return;
	}

	if( GetActor().AnimAction() == ACTION_ROAM_LADDER_CLIMBDOWN )
		return;

	// Get our model component
	BipedModel* model = (BipedModel*)actor.GetComponentByName("BipedModel");
	ASSERT_PTR(model);

	AvatarControlMapper *avatarControl = (AvatarControlMapper *)GetActor().GetProperty("ControlMapper")->GetPointer();
	ASSERT_PTR(avatarControl);

	if( avatarControl->ButtonPressed( AvatarControlMapper::USE ) && model->OnLadder() && !model->LadderSlide() )
	{
		pressOnUs = true;
		model->StartLadderSlide();
	}
	if( avatarControl->ButtonPressed( AvatarControlMapper::JUMP ) && model->OnLadder() && !model->LadderSlide() )
	{
		g_messageDispatcher.SendMessageToActor( "Jump", NULL, actor.GetHandle(), actor.GetHandle() );
	}

	float climb = avatarControl->GetAxis(AvatarControlMapper::FORWARDBACK);
	// if the view is not pointing the same direction as the actor's facing,
	// invert the climb axis, so that pushing up goes down the ladder, since
	// this feels more natural.

	// ok, sure, it feels more natural to me, but everybody else hates it.
	// bye-bye.  leaving the code just in case though, because I really did think
	// it felt better with this in.
	#if 0
		ViewIF *viewIF = (ViewIF *)GetActor().GetInterface( ViewIF::GetID() );
		if( viewIF )
		{
			Vector3 forward = GetActor().GetBodyToWorld().GetRow2();
			Vector3 viewdir;
			viewIF->GetDirection( viewdir );
			if( viewdir.Dot( forward ) < Math::CosDeg( 100.0f ) )
				climb = -climb;
		}
	#endif
	CMoveIF *moveIF = (CMoveIF *)GetActor().GetInterface( CMoveIF::GetID() );
	if( moveIF )
		moveIF->CommandVerticalSpeed( climb, true );

}
