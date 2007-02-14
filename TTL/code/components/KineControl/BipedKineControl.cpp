#include "TTLPCH.h"

#include "BipedKineControl.h"
#include "ViewDriver.h"
#include "BipedLadder.h"
#include "WallMoves.h"
#include "Components/ComponentCreator.h"

#include "EngineHelper/sfx.h"
#include "Interfaces/MoveIF.h"
#include "Interfaces/ViewIF.h"
#include "Interfaces/CollisionIF.h"
#include "Components/InteractionMonitor.h"
#include "Interfaces/GroundHeightIF.h"

#include "data_h_files/players/players_s.h"
#include "avatar/taunt.h"
#include "components/quicktag.h"
#include "weapons/ttlweapinv.h"
#include "weapons/firearm.h"
#include "weapons/zoomsight.h"
#include "weapons/lobbed.h"

#include "Math/sfilter.h"

#include "camera/controlcam/controlcam.h"
#include "InterfacesTTL/ParticipantIF.h"
#include "platform/CMouse.h"
#include "Script/ScriptVM.h"

#include "avatar/pstat.h"

#define COS_VERTICAL_SLOPE     1.0e-6f
#define COS_GROUND_ANGLE       0.95f

// Define creators and add them to the creator list.
STANDARD_COMPONENT_CREATOR(BipedKinematicController, Biped,
		COMPONENT_CONTROLLER_PRIORITY);
STANDARD_COMPONENT_CREATOR(BipedPhysicalControllerDriver,
		BipedPhysicalController, COMPONENT_DRIVER_PRIORITY);
STANDARD_COMPONENT_CREATOR(MoveToPointDriver,
		MoveToPoint, COMPONENT_DRIVER_PRIORITY);
#ifdef _DEBUG
STANDARD_COMPONENT_CREATOR(BipedPointToPointDriver, BipedPointToPoint,
		COMPONENT_DRIVER_PRIORITY);
#endif

typedef struct tag_BipedDefaultParams
{
	float aimDuration;
	float maxSidestepSpeed;
	float maxMove;
	float maxStrafe;
	float maxTurnRate;
	float airAcceleration;
	float airMaxSpeed;
	float gravity;
	float maxPeek;
	float fallVelocityMin;
	float fallVelocityMax;
	float sprintFactor;
} BipedDefaultParams;

BipedDefaultParams BipedDefaults;

void CParam<BipedDefaultParams>::RegisterVariables()
{
	RegisterVariable( d_data.aimDuration, "aimDuration", 1.0f, 0.0f, 30.0f );
	RegisterVariable( d_data.maxSidestepSpeed, "maxSidestepSpeed", 25.5f, 0.0f, 50.0f );
	RegisterVariable( d_data.maxMove, "maxMove", 4.5f, 0.0f, 10.0f );
	RegisterVariable( d_data.maxStrafe, "maxStrafe", 4.5f, 0.0f, 10.0f );
	RegisterVariable( d_data.maxTurnRate, "maxTurnRate", 8.0f, 0.0f, 50.0f );

	RegisterVariable( d_data.airAcceleration, "airAcceleration", 10.0f, 0.0f, 100.0f );
	RegisterVariable( d_data.airMaxSpeed, "airMaxSpeed", 3.0f, 0.0f, 20.0f );
	RegisterVariable( d_data.gravity, "gravity", -14.81f, 0.0f, 50.0f );
	RegisterVariable( d_data.maxPeek, "maxPeek", 0.3f, 0.0f, 2.0f );

	RegisterVariable( d_data.fallVelocityMin, "fallVelocityMin", 13.0f, 0.0f, 100.0f );
	RegisterVariable( d_data.fallVelocityMax, "fallVelocityMax", 26.0f, 0.0f, 100.0f );

	RegisterVariable( d_data.sprintFactor, "sprintFactor", 1.5f, 1.0f, 5.0f );
}

/***************************************************************************/
/***************************************************************************/
void BipedKinematicController::KickHandler::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	ASSERT(&d_ref->actor == CActor::FromHandle(i_sender));
	d_ref->Kick();
}

void BipedKinematicController::Kick( void )
{
	InteractionMonitor *monitor = (InteractionMonitor *)GetActor().GetComponentByName( "InteractionMonitor" );
	if( monitor )
	{
		CActor *target = CActor::FromHandle( interactionActor );
		if( target )
		{
			if( interactionType == InteractionMonitor::INTERACTION_DOOR_KICK_OPEN ||
				interactionType == InteractionMonitor::INTERACTION_DOOR_KICK_CLOSE ||
				interactionType == InteractionMonitor::INTERACTION_DOOR_PULL_CLOSE )
			{
				Vector3 forward;
				ViewIF *viewIF = (ViewIF *)GetActor().GetInterface( ViewIF::GetID() );
				if( viewIF )
					viewIF->GetDirection( forward );
				else
					forward = GetActor().GetBodyToWorld().GetRow2();

				forward *= 20.0f;

				g_messageDispatcher.SendMessageToActor( "SimpleInteraction", &forward, actor.GetHandle(), target->GetHandle() );
			}
		}
	}
}

#ifdef DEBUG_VAULTTRIGGER_SETUP
void BipedKinematicController::VaultTrigger::HandleMessage( void *i_data,
	   ActorHandle i_sender, bool i_posted )
{
	g_messageDispatcher.SendMessageToActor( "TriggeredVault", NULL, i_sender, d_ref->actor.GetHandle() );
}
#endif //DEBUG_VAULTTRIGGER_SETUP

void BipedKinematicController::TriggeredVaultHandler::HandleMessage( void *i_data,
	   ActorHandle i_sender, bool i_posted )
{
	CActor* sender = CActor::FromHandle(i_sender);
	if (sender == NULL)
	{
		ASSERT(false);
		return;
	}

	// use the sender's obb as a query volume, find the first vaultable/climbable static obb colliding
	// with the trigger, and vault over that sucker
	CCollisionList *list = NULL;
	CObbEntity obb;
	CCollisionObject *object = sender->GetCollisionObject();

	bool correct = true;
	if( object == NULL )
		correct = false;
	if( object->GetObjectType() != CCollisionObject::OBB_OBJECT )
		correct = false;
	ASSERT( correct && "The sender of TriggeredVault should be a trigger box.  If you're forwarding the message, please set the sender correctly" );
	if( correct )
	{
		obb.Obb() = *((OBB *)object);
		obb.SetPositiveExclude( TERRAIN_CATEGORY );
		obb.SetIgnoreEntity( &d_ref->actor );
		list = g_collisionSystem.Test( &obb );

		if( list )
		{
			CCollisionEvent* event = list->GetHeadEvent();
			while (event)
			{
				if( event->GetTargetEntity()->IsA( CStaticEntity::ClassType() ) )
				{
					CCollisionPair *pair = event->GetHeadPair();
					while( pair )
					{
						ASSERT( pair->GetIntersectionType() == CCollisionIntersection::OBB_TO_OBB );
						OBBStaticInstance* inst = (OBBStaticInstance*)(pair->GetTargetObject());
						if( inst->Material() == g_surfaceType.GetIndex("vaultable") ||
							inst->Material() == g_surfaceType.GetIndex("vaulttrigger") ||
							inst->Material() == g_surfaceType.GetIndex("climbable") )
						{
							WallVault *vault = (WallVault *)d_ref->GetActor().GetComponentByName( "WallVault" );

							if( vault )
							{
								vault->SetVault( inst );
								vault->Activate();
							}

						}
						pair = pair->GetNext();
					}
				}
				else if( event->GetTargetEntity()->IsA( CActor::ClassType() ) )
				{
					CActor *target = (CActor *)event->GetTargetEntity();
					ScriptVM *script = target->Script( AI_SCRIPT );
					if( script )
					{
						char *type = NULL;
						if( script->HasFunction( "QueryTriggeredInteractionType" ) )
						{
							script->Push( &d_ref->GetActor() );
							script->CallFunction( "QueryTriggeredInteractionType" );
							type = script->PopString();
						}
						else if( script->HasFunction( "QueryInteractionType" ) )
						{
							script->Push( &d_ref->GetActor() );
							script->CallFunction( "QueryInteractionType" );
							type = script->PopString();
						}
						if( type )
						{
							if( strcmp( type, "Vaultable" ) == 0 )
							{
								WallVault *vault = (WallVault *)d_ref->GetActor().GetComponentByName( "WallVault" );

								if( vault )
								{
									vault->SetVault( target );
									vault->Activate();
								}

							}
						}
					}
				}
				event = event->GetNext();
			}
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void BipedKinematicController::MoveToHandler::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	CActor* beacon = g_stage.MasterCast().FindActorByInstanceName( (char *)i_data );
	if( beacon )
	{
		MoveToPointDriver *move = (MoveToPointDriver *)d_ref->GetActor().AttachComponent( "MoveToPoint" );
		if( move )
			move->MoveToPointAndFacing( beacon->GetBodyInWorld(), beacon->GetBodyToWorld(), 1.0f );
	}
}

/***************************************************************************/
/***************************************************************************/
void BipedKinematicController::LaunchHandler::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	ASSERT(&d_ref->actor == CActor::FromHandle(i_sender));
	d_ref->Launch(*(Vector3*)i_data);
}


void BipedKinematicController::Launch(Vector3CRef i_vel)
{
	actor.GetVelocityInWorld() += i_vel * actor.GetBodyToWorld();
	actor.SetProperty( "onGround", false );
	actor.SetDiscontinuityBits(DISCONTINUITY_VELOCITY);
}

/***************************************
/ CONTROLLER
***************************************/

void BipedKinematicController::SetDefaults( void )
{
	BipedDefaultParams *defaults = CParam<BipedDefaultParams>::GetParam( "biped", "var" );

	aimDuration = defaults->aimDuration;
	maxSidestepSpeed = defaults->maxSidestepSpeed;
	axisRange[Biped_Axis_Move] = AxisRange( defaults->maxMove );
	axisRange[Biped_Axis_Strafe] = AxisRange( defaults->maxStrafe );
	axisRange[Biped_Axis_TurnRate] = AxisRange( defaults->maxTurnRate );
	airAcceleration = defaults->airAcceleration;
	airMaxSpeed = defaults->airMaxSpeed;
	gravity = defaults->gravity;
	maxPeek = defaults->maxPeek;
	fallVelocityMin = defaults->fallVelocityMin;
	fallVelocityMax = defaults->fallVelocityMax;
	sprintFactor = defaults->sprintFactor;
}

#pragma warning(disable : 4355)  // Allow this pointer in initializer list
BipedKinematicController::BipedKinematicController(CCompActor &i_actor):
	KinematicController( i_actor ),
	d_moveIF(*this)
{
	int i;

	ASSERT(actor.IsA(CCombatant::ClassType()));

	active = false;

	aimtime = 0.0f;

	for( i = 0; i < Biped_Axis_Count; i++ )
		axisPosition[ i ] = 0.0f;

	SetDefaults();

	actor.AddProperty( "onGround", Property_bool );
	actor.AddProperty( "groundNormal", Property_Vector3 );
	actor.AddProperty( "groundPosition", Property_Vector3 );
	actor.AddProperty( "cappedSpeed", Property_float );
	actor.AddProperty( "aimDuration", Property_float, &aimDuration );
	actor.AddProperty( "peekOffset", Property_Vector3 );
	actor.AddProperty( "moveMultiplier", Property_float );
	actor.AddProperty( "aimTurning", Property_float );
	actor.AddProperty( "incumberance", Property_float );
	actor.AddProperty( "sprint", Property_bool );
	actor.AddProperty( "sprinting", Property_bool );
	actor.AddProperty( "deathWhenBelow", Property_float );
	actor.AddProperty( "aimDirection", Property_Vector3 );

	actor.SetProperty( "onGround", false );
	actor.SetProperty( "groundNormal", Vector3( 0.0f, 1.0f, 0.0f ) );
	actor.SetProperty( "groundPosition", Vector3( 0.0f, 0.0f, 0.0f ) );
	actor.SetProperty( "cappedSpeed", MAXFLOAT );
	actor.SetProperty( "peekOffset", Vector3( 0.0f, 0.0f, 0.0f ) );
	actor.SetProperty( "moveMultiplier", 1.0f );
	actor.SetProperty( "incumberance", 0.0f );
	actor.SetProperty( "sprint", false );
	actor.SetProperty( "sprinting", false );
	actor.SetProperty( "deathWhenBelow", -MAXFLOAT );
	actor.SetProperty( "aimDirection", Vector3( 0.0f, 0.0f, 1.0f ) );

	ts_NamedPosition *lowestPoint = g_loadSOM.FindNamedPoint( "LowestPoint" );
	if( lowestPoint )
		actor.SetProperty( "deathWhenBelow", lowestPoint->vPos.Y() );

	actor.AddProperty( "updateLevel", Property_int );
	actor.SetProperty( "updateLevel", 0 );

	heading = 0.0f;
}
#pragma warning(default : 4355)

BipedKinematicController::~BipedKinematicController()
{
	g_console.DeleteContext( "Biped" );
}

void BipedKinematicController::Initialize( void )
{
	#ifdef DEBUG_VAULTTRIGGER_SETUP
	vaultTrigger.Initialize("VaultTrigger", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	#endif
	triggeredVaultHandler.Initialize("TriggeredVault", MESSAGE_HANDLE_IF_MASTER,
			actor.GetMessageOwner(), this);

	kickHandler.Initialize("KickContact", MESSAGE_HANDLE_IF_MASTER,
			actor.GetMessageOwner(), this);
	moveToHandler.Initialize("MoveTo", MESSAGE_HANDLE_IF_MASTER,
			actor.GetMessageOwner(), this);
	launchHandler.Initialize("Launch", MESSAGE_HANDLE_IF_MASTER,
			actor.GetMessageOwner(), this);
}

void BipedKinematicController::SyncActor( void )
{
	KinematicController::SyncActor();

	// controllers don't actually perform placement, that is handled
	// elsewhere.  This is just a notification that a placement has
	// happened
	ASSERT(actor.IsA(CCombatant::ClassType()));

	if( TestOnGround() )
	{
		actor.GetBodyInWorld().y( actor.GetProperty( "groundPosition" )->GetVector3().y() );
		actor.GetVelocityInWorld().y( 0.0f );
		actor.SetProperty( "onGround", true );
	}
	else
		actor.SetProperty( "onGround", false );

	Vector3 forward = actor.GetBodyToWorld().GetRow2();
	heading = Math::ArcTan2( forward.x(), forward.z() );
}

void BipedKinematicController::Activate( void )
{
	// the actorcomponent version needs to be called so it can track the current activity state
	CActorComponent::Activate();

	ASSERT(actor.IsA(CCombatant::ClassType()));
	actor.SetProperty( "onGround", TestOnGround() );
	determineDesiredVelocity = true;

	groundTestCacheValid = false;

	Reset();
}

void BipedKinematicController::Deactivate(void)
{
	// the actorcomponent version needs to be called so it can track the current activity state
	CActorComponent::Deactivate();

	//actor.GetVelocityInWorld().Clear();
}

// This is called when somebody dumps part of our velocity.  In this case, we care
// because if any of that dumped velocity is downward, we may need to inflict
// falling damage
void BipedKinematicController::VelocityDumped( Vector3CRef velChange )
{
	HealthIF *hIF = (HealthIF *)GetActor().GetInterface( HealthIF::GetID() );
	if( hIF )
	{
		float yvel = velChange.y();
		if( yvel > fallVelocityMin )
		{
			// raise everything to the fourth power to scale a little better
			float y4, min4, max4;
			y4 = yvel * yvel * yvel * yvel;
			min4 = fallVelocityMin * fallVelocityMin * fallVelocityMin * fallVelocityMin;
			max4 = fallVelocityMax * fallVelocityMax * fallVelocityMax * fallVelocityMax;

			float damage = ( y4 - min4 ) / ( max4 - min4 );
			hIF->DoDamage( damage * 100.0f, HealthIF::FALLING, INVALID_PARTICIPANT );
		}
	}
}

void BipedKinematicController::BeginUpdate(void)
{
	ASSERT(actor.IsA(CCombatant::ClassType()));
	if( !IsActive() )
		return;

	Vector3 forward = actor.GetBodyToWorld().GetRow2();
	Vector3 side = actor.GetBodyToWorld().GetRow0();

	if( actor.GetProperty( "onGround" )->GetBool() )
	{
		Vector3 vel( 0.0f, 0.0f, 0.0f );

		if( actor.GetProperty( "health" )->GetFloat() > 0.0f )
		{
			float forwardspeed = axisPosition[Biped_Axis_Move];
			float strafespeed = axisPosition[Biped_Axis_Strafe];

			// don't allow full speed until we're within 25 degrees of our desired
			// facing.  scale to zero at 90 degrees
			float angletotarget = Math::Abs( Math::Rad2Deg( axisPosition[Biped_Axis_Turn] ) );

			vel = forward * forwardspeed;
			vel += -side * strafespeed;

			// cap XZ velocity to maxSpeed
			Vector3 xzvel = vel;
			xzvel.y( 0.0f );
			float mag = xzvel.Length();
			float maxspeed = MaxSpeed();

			if( mag > maxspeed )
			{
				xzvel *= maxspeed / mag;
				vel.x( xzvel.x() );
				vel.z( xzvel.z() );
			}

			// apply xzvel along the ground vector
			if( vel.LengthSquared() > 0.0001f )
			{
				Vector3 ground = actor.GetProperty( "groundNormal" )->GetVector3();
				Vector3 veldir = vel.Normalized();
				Vector3 cross = ground.Cross( veldir ).Normalized();
				veldir = cross.Cross( ground ).Normalized();
				vel = veldir * vel.Length();
			}
		}
		// don't overwrite the desired velocity if it's already been figured out
		if( determineDesiredVelocity )
		{
			desiredVelocity = vel;
		}
	}
}

void BipedKinematicController::AttemptUpdate(float i_deltaSec)
{
	ASSERT(actor.IsA(CCombatant::ClassType()));
	if( !IsActive() )
		return;

	// air acceleration?
	if( !actor.GetProperty( "onGround" )->GetBool() )
	{
		// add air acceleration if necessary
		Vector3 forward = actor.GetBodyToWorld().GetRow2();
		Vector3 side = actor.GetBodyToWorld().GetRow0();
		float forwardspeed = axisPosition[Biped_Axis_Move] / GetAxisRange( Biped_Axis_Move ).Max();
		float sidespeed = -axisPosition[Biped_Axis_Strafe] / GetAxisRange( Biped_Axis_Strafe ).Max();

		Vector3 airAccel;
		airAccel = forward * forwardspeed * airAcceleration * i_deltaSec;
		airAccel += side * sidespeed * airAcceleration * i_deltaSec;

		// don't allow the acceleration to bump the actor's xz velocity beyond airMaxSpeed
		Vector3 newv = actor.GetVelocityInWorld() + airAccel;
		newv.y( 0.0f );
		if( newv.LengthSquared() > (airMaxSpeed * airMaxSpeed) )
		{
			// the acceleration is going to put the velocity over the max.
			// figure out how much acceleration we can allow so that the velocity is at max
			Vector3 xz = actor.GetVelocityInWorld();
			xz.y( 0.0f );
			Vector3 airxz = airAccel;
			airxz.y( 0.0f );

			// if the acceleration is in the same direction as the velocity, we need to disallow
			// all or part of it
			if( xz.Dot( airxz ) >= 0.0f )
			{
				float allowed = 3.0f - xz.Length();
				// if allowed is less than or equal to zero, the velocity started at or beyond max,
				// so don't add any accelertaion
				if( allowed <= 0.0f )
				{
					airAccel.Clear();
				}
				else
				{
					float len = airxz.Length();
					if( len > allowed )
						airAccel = airAccel * allowed / len;
				}
			}
		}
		actor.GetVelocityInWorld() += airAccel;
	}

	if( actor.GetProperty( "onGround" )->GetBool() )
	{
		actor.GetVelocityInWorld() = desiredVelocity;
		((CCombatant*)&actor)->GetAccelerationInWorld().Clear();
	}
	else
	{
		((CCombatant*)&actor)->GetAccelerationInWorld().Set( 0.0f, gravity, 0.0f );
	}
	actor.GetBodyInWorld() += actor.GetVelocityInWorld() * i_deltaSec;
	actor.GetVelocityInWorld() += ((CCombatant*)&actor)->GetAccelerationInWorld() * i_deltaSec;
	// terminal velocity!
	if( actor.GetVelocityInWorld().y() < -20.0f )
		actor.GetVelocityInWorld().y( -20.0f );

	// do we need to turn?  this rotation needs to be subtracted back off from
	// Biped_Axis_Turn in endupdate
	float totarget = axisPosition[Biped_Axis_Turn];
	float dt;

	dt = GetAxisRange( Biped_Axis_TurnRate ).Max() * i_deltaSec;
	if( totarget < 0.0f )
		dt = -dt;

	if( Math::Abs( dt ) > Math::Abs( totarget ) )
		dt = totarget;

	heading = Math::NormalAngle( heading - dt );

	actor.GetBodyToWorld().SetYRotation( heading );

	actor.SyncState();
}

void BipedKinematicController::EndUpdate(float i_deltaSec)
{
	if( !IsActive() )
		return;

	if( TestOnGround() )
	{
		actor.GetBodyInWorld().y( actor.GetProperty( "groundPosition" )->GetVector3().y() );
		if( !actor.GetProperty( "onGround" )->GetBool() )
		{
			Vector3 dV;
			dV.Set( 0.0f, -(actor.GetVelocityInWorld().y()), 0.0f );
			VelocityDumped( dV );
			actor.GetVelocityInWorld().y( 0.0f );
			actor.SetProperty( "onGround", true );
			actor.SetDiscontinuityBits(DISCONTINUITY_VELOCITY);
		}
		actor.SyncState();
	}
	else
	{
		actor.SetProperty( "onGround", false );
		// below the death point?
		if( actor.GetBodyInWorld().Y() < actor.GetProperty( "deathWhenBelow" )->GetFloat() )
		{
			PlayerStats* pStats = (PlayerStats*)actor.GetInterface(PlayerStats::GetID());
			if ( pStats )
				pStats->Kill();
		}
	}

	float turned = 0.0f;

	// do we need to turn?
	float totarget = axisPosition[Biped_Axis_Turn];
	float dt;

	dt = GetAxisRange( Biped_Axis_TurnRate ).Max() * i_deltaSec;
	if( totarget < 0.0f )
		dt = -dt;

	if( Math::Abs( dt ) > Math::Abs( totarget ) )
		dt = totarget;

	turned += dt;

	axisPosition[Biped_Axis_Turn] -= dt;
}

void BipedKinematicController::BeginFrame( void )
{
	// Brute force calculation of turn rate.
	Vector3 forward = actor.GetBodyToWorld().GetRow2();
	beginHeading = Math::ArcTan2( forward.x(), forward.z() );

}

void BipedKinematicController::EndFrame( void )
{
	// set this here for next frame.  Ideally, this would be in beginframe, but
	// the driver does all its calculating in beginframe, which runs before our beginframe
	determineDesiredVelocity = true;

	// Brute force calculation of turn rate.
	Vector3 forward = actor.GetBodyToWorld().GetRow2();
	float endHeading = Math::ArcTan2( forward.x(), forward.z() );
	float turnedThisFrame = Math::NormalAngle( endHeading - beginHeading );

	if (!Math::DivByZero(turnedThisFrame, g_timer.GetFrameSec()) &&
			((actor.GetDiscontinuity() & DISCONTINUITY_ANGULAR_VELOCITY) == 0))
	{
		actor.GetAngVelocityInBody().Set( 0.0f, turnedThisFrame / g_timer.GetFrameSec(), 0.0f );
	}
	else
	{
		actor.GetAngVelocityInBody().Set( 0.0f, 0.0f, 0.0f );
	}

	actor.SetProperty( "sprinting", IsSprinting());
}

bool BipedKinematicController::TestOnGround( void )
{
	ASSERT(actor.IsA(CCombatant::ClassType()));

	if( groundTestCacheValid &&
		Math::Zero( (cachePosition - actor.GetBodyInWorld()).LengthSquared() ) &&
		Math::Zero( (cacheVelocity - actor.GetVelocityInWorld()).LengthSquared() ) )
	{
		CGroundHeightIF* groundHeight = CGroundHeightIF::GetInterface(actor);
		if (groundHeight != NULL)
		{
			CGroundHeightIF::SCheckResults testResults;
			if (cachedOnGround)
			{
				testResults.d_status = CGroundHeightIF::HIT;
				testResults.d_topHeight = 1.5f;
				testResults.d_bottomHeight = cachedGroundPosition.Y() -
						actor.GetBodyInWorld().Y();
				testResults.d_intersection = actor.GetBodyInWorld();
				testResults.d_intersection.Y(cachedGroundPosition.Y());
				testResults.d_normal = cachedGroundNormal;
				testResults.d_color = cachedGroundColor;
				testResults.d_material = cachedGroundMaterial;
			}
			else
			{
				testResults.d_status = CGroundHeightIF::NO_HIT;
				testResults.d_topHeight = 1.5f;
				testResults.d_bottomHeight = -1.5f;
				testResults.d_color = 0;
				testResults.d_material = INVALID_MATERIALINDEX;
			}
			testResults.d_target.MakeInvalid();
			groundHeight->Set(testResults);
		}

		actor.SetProperty( "groundNormal", cachedGroundNormal );
		actor.SetProperty( "groundPosition", cachedGroundPosition );
		return cachedOnGround;
	}

	CLineQuery groundCollision;

	CStageEntity *highentity = NULL;
	Vector3 highintersect, highnormal;
	int highmaterial = 0;
	u16 highcolor = 0xffff;
	float angle, highy = -MAXFLOAT;
	bool foundground = false;
	int i;
	int points = 3;
	if( actor.GetProperty( "updateLevel" )->GetInt() == 0 )
		points = 1;

	Vector3 groundCenter;
	CCollisionObject *cObj = actor.GetCollisionObject();
	if( cObj && cObj->GetObjectType() == CCollisionObject::CAPSULE )
	{
		groundCenter = ((Capsule *)cObj)->Origin();
		groundCenter.y( actor.GetBodyInWorld().y() );
	}
	else
		groundCenter = actor.GetBodyInWorld();

	// scan around a triangle on the ground, finding the highest ground point
	for( i = 0, angle = 0.0f; i < points; i++, angle += (360.0f / ((float)points)) )
	{
		CLineEntity entity;

		float x = 0.0f, z = 0.0f;

		if( points != 1 )
		{
			x = Math::CosDeg( angle ) * 0.07f;
			z = Math::SinDeg( angle ) * 0.07f;
		}

		Vector3 origin, end;
		origin.Set( groundCenter.x() + x, groundCenter.y() + 1.5f, groundCenter.z() + z );
		end.Set( groundCenter.x() + x, groundCenter.y() - 1.5f, groundCenter.z() + z );

		g_collisionSystem.ResetStack();

		entity.SetPositiveExclude( AVATAR_IGNORE_CATEGORIES );
		entity.SetIgnoreEntity( &actor );
		entity.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
		entity.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
		entity.Line().SetOriginEnd( origin, end );

		if( groundCollision.Test( entity ) )
		{
			if( groundCollision.intersection.y() > highy )
			{
				highentity = groundCollision.target;
				highintersect = groundCollision.intersection;
				highnormal = groundCollision.normal;
				highmaterial = (int) groundCollision.material;
				highy = highintersect.y();
				highcolor = groundCollision.color;
				foundground = true;
			}
		}
	}

	if( foundground )
	{
		CGroundHeightIF* groundHeight = CGroundHeightIF::GetInterface(actor);
		if (groundHeight != NULL)
		{
			CGroundHeightIF::SCheckResults testResults;
			testResults.d_status = CGroundHeightIF::HIT;
			testResults.d_topHeight = 1.5f;
			testResults.d_bottomHeight = highy - actor.GetBodyInWorld().Y();
			testResults.d_intersection = actor.GetBodyInWorld();
			testResults.d_intersection.Y(highy);
			testResults.d_normal = highnormal;
			testResults.d_color = highcolor;
			testResults.d_material = highmaterial;
			if (highentity && highentity->IsA(CActor::ClassType()))
				testResults.d_target = CActor::ToHandle((CActor*)highentity);
			else
				testResults.d_target.MakeInvalid();
			groundHeight->Set(testResults);
		}
	}
	else
	{
		CGroundHeightIF* groundHeight = CGroundHeightIF::GetInterface(actor);
		if (groundHeight != NULL)
		{
			CGroundHeightIF::SCheckResults testResults;
			testResults.d_status = CGroundHeightIF::NO_HIT;
			testResults.d_topHeight = 1.5f;
			testResults.d_bottomHeight = -1.5f;
			testResults.d_color = 0xffff;
			testResults.d_material = INVALID_MATERIALINDEX;
			testResults.d_target.MakeInvalid();
			groundHeight->Set(testResults);
		}
	}

	if( foundground )
	{
		Vector3 v = actor.GetVelocityInWorld();

		// on ground if its not too steep, we're close to the ground intersection point, and our velocity
		// doesn't point away from the ground

		// if we're currently on the ground, try really hard to keep us there.  If not,
		// don't suck the actor to the ground from too high a distance
		float suckheight = 0.1f;
		float escapeVelocity = 0.1f;

		if( actor.GetProperty( "onGround" )->GetBool() )
		{
			suckheight = 0.4f;
			escapeVelocity = 3.5f;
		}

		bool tooSteep = false;
		bool tooHigh = false;
		bool tooFast = false;
		if( highnormal.y() < Math::CosDeg( 45.0f ) )
			tooSteep = true;
		if( actor.GetBodyInWorld().y() - suckheight >= (highintersect.y()) )
			tooHigh = true;
		if( v.Dot( highnormal ) > escapeVelocity )
			tooFast = true;


		if( tooSteep || tooHigh || tooFast )
		{
			foundground = false;
		}
		else
		{
			// code for a breakpoint in debug builds:
			#ifdef _DEBUG
				float eVel = v.Dot( highnormal );
				float h = actor.GetBodyInWorld().y() - highintersect.y();
			#endif

			//dbgPrint( "Not on ground.  Height = %0.4f  groundDotVel = %0.4f\n",
			//		actor.GetBodyInWorld().y() - highintersect.y(),
			//		v.Dot( highnormal ) );
		}
	}

	groundTestCacheValid = true;
	if( foundground )
	{
		cachePosition = actor.GetBodyInWorld();
		cacheVelocity = actor.GetVelocityInWorld();
		cachedOnGround = true;
		cachedGroundNormal = highnormal;
		cachedGroundPosition = highintersect;
		cachedGroundMaterial = highmaterial;
		cachedGroundColor = highcolor;
		// don't cache if we're standing on an actor, since they could move or go away.
		if( highentity && highentity->IsA( CActor::ClassType() ) )
			groundTestCacheValid = false;
	}
	else
	{
		cachePosition = actor.GetBodyInWorld();
		cacheVelocity = actor.GetVelocityInWorld();
		cachedOnGround = false;
		cachedGroundNormal.Set( 0.0f, 1.0f, 0.0f );
		cachedGroundPosition.Set( groundCenter );
		cachedGroundMaterial = (int)INVALID_MATERIALINDEX;
		cachedGroundColor = 0xffff;
	}

	actor.SetProperty( "groundNormal", cachedGroundNormal );
	actor.SetProperty( "groundPosition", cachedGroundPosition );

	return foundground;
}

void BipedKinematicController::MoveDirection( Vector3CRef vel )
{
	MoveDirectionNormalized( vel, vel.Length() / GetAxisRange( Biped_Axis_Move ).Max() );
}

void BipedKinematicController::MoveDirection( Vector3CRef dir, float speed )
{
	MoveDirectionNormalized( dir, speed / GetAxisRange( Biped_Axis_Move ).Max() );
}

void BipedKinematicController::MoveDirectionNormalized( Vector3CRef vel )
{
	MoveDirectionNormalized( vel, vel.Length() );
}

void BipedKinematicController::MoveDirectionNormalized( Vector3CRef dir, float speed )
{
	// given a specified direction and normalized speed, figure out how to get us moving that way.
	if( Math::Zero( speed ) )
		return;

	// If a facing direction is specified, the actor will walk backward when walking
	// away from that direction.

	Vector3 actorup = actor.GetBodyToWorld().GetRow1();
	Vector3 actordir = actor.GetBodyToWorld().GetRow2();
	Vector3 actorside = actor.GetBodyToWorld().GetRow0();
	Vector3 vel = actor.GetVelocityInWorld();

	float longSpeed = vel.Dot( actordir );
	float sideSpeed = vel.Dot( actorside );

	float strafe = 0.0f;

	// constrain the direction to our movement plane.  this just makes figuring out the
	// angle we need to turn easier
	Vector3 cross = actorup.Cross( dir );
	Vector3 targetdir = cross.Cross( actorup );
	if (!targetdir.SafeNormalize())
		return;

	determineDesiredVelocity = false;

	float maxspeed = MaxSpeed();
	desiredVelocity = targetdir * (speed * maxspeed);

	if ( speed < 0.0f )
	{
		speed = -speed;
		targetdir.Negate();
	}

	ViewIF *viewIF = (ViewIF *)GetActor().GetInterface( ViewIF::GetID() );
	Vector3 facing, flatfacing, side;
	bool viewLocked = false;
	if( viewIF && viewIF->ViewLocked() )
	{
		viewIF->GetDirection( facing );
		viewLocked = true;
	}
	else
		facing = actordir;

	// constrain the facing to our movement plane as well.
	cross = actorup.Cross( facing );
	flatfacing = cross.Cross( actorup );
	if ( !flatfacing.SafeNormalize() )
		return;
	side = actorup.Cross( flatfacing );

	// if we have a hostage, move differently, so that the facing never changes in relation to the
	// view
	Property *prop = GetActor().QueryProperty( "hostage" );
	if( prop && ((ActorHandle)prop->GetHandle()).Valid() )
	{
		targetdir = flatfacing;

		// restrict movement to the 4 cardinal directions.  no diagonal movement allowed
		// when you've got a human shield, sorry
		float totalspeed = desiredVelocity.Length();
		speed = desiredVelocity.Dot( flatfacing );
		strafe = desiredVelocity.Dot( side );

		if( Math::Abs( speed ) > Math::Abs( strafe ) )
		{
			if( speed < 0.0f )
				desiredVelocity = flatfacing * -totalspeed;
			else
				desiredVelocity = flatfacing * totalspeed;
			speed = totalspeed;
			strafe = 0.0f;
		}
		else
		{
			if( strafe < 0.0f )
				desiredVelocity = side * -totalspeed;
			else
				desiredVelocity = side * totalspeed;
			strafe = totalspeed;
			speed = 0.0f;
		}
	}
	else if( viewLocked )
	{

		// figure out how to move so that we're still facing the same direction, but moving
		// in a new direction

		float backwardangle = 135.0f;
		float forwardangle = 45.0f;
		// can we sidestep?
		if( speed > (maxSidestepSpeed / GetAxisRange( Biped_Axis_Move ).Max()) )
		{
			forwardangle = 90.0f;
			backwardangle = 90.0f;
		}

		// try to reduce flip-flopping transitions by adding a 5 degree buffer on each
		// side of the current state.
		if( Math::Abs( sideSpeed ) > Math::Abs( longSpeed ) && speed <= (maxSidestepSpeed / GetAxisRange( Biped_Axis_Move ).Max()) )
		{
			// moving sideways.
			forwardangle -= 5.0f;
			backwardangle += 5.0f;
		}
		else if( longSpeed >= 0.0f )
		{
			// moving forward or stationary.
			forwardangle += 5.0f;
		}
		else
		{
			// moving backward.
			backwardangle -= 5.0f;
		}

		float dot = targetdir.Dot( flatfacing );
		// less than backwardangle = backward move
		// between backwardangle and forwardangle = sidestep
		// greater than forwardangle = forward move
		if( dot < Math::CosDeg( backwardangle ) )
		{
			targetdir.Negate();
			speed = -speed;
		}
		else if( dot < Math::CosDeg( forwardangle ) )
		{
			targetdir = actorup.Cross( targetdir );
			strafe = speed;
			speed = 0.0f;

			// target should point toward the facing.  if not, we need
			// to flip it
			if( targetdir.Dot( flatfacing ) < 0.0f )
			{
				targetdir.Negate();
				strafe = -strafe;
			}
		}
		else
		{
			// forward move.  nothing to change
		}
	}

	float angletotarget = Math::ArcCos( targetdir.Dot( actordir ) );
	if( targetdir.Dot( actorside ) > 0.0f )
		angletotarget = -angletotarget;
	if( viewLocked )
	{
		// don't allow our rotation to go away from the facing, even if that's the shortest way.
		float heading = Math::ArcTan2( actordir.z(), actordir.x() );
		float faceheading = Math::ArcTan2( facing.z(), facing.x() );
		float toface = Math::NormalAngle( faceheading - heading );

		// if we're trying to turn more than 90 degrees (fudged to 120 because of angle slop above)
		// and we're not trying to turn the same direction we would turn if we were turning towards the facing,
		// then GO THE OTHER WAY.
		if( Math::Abs( angletotarget ) > Math::Deg2Rad( 120.0f ) && !Math::SameSign( toface, angletotarget ) )
		{
			angletotarget += Math::Deg2Rad( 360.0f ) * Math::Sign( toface );
		}
	}

	//dbgPrint( "angletotarget: %5.3f  speed: %5.3f  strafe: %5.3f\n", angletotarget, speed, strafe );
	SetAxis( Biped_Axis_Turn, angletotarget );
	SetAxisNormalized( Biped_Axis_Move, speed );
	SetAxisNormalized( Biped_Axis_Strafe, strafe );
}

void BipedKinematicController::MoveToPoint( Vector3CRef dest, float speed )
{
	MoveToPointNormalized( dest, speed / GetAxisRange( Biped_Axis_Move ).Max() );
}

void BipedKinematicController::MoveToPointNormalized( Vector3CRef dest, float speed )
{
	// don't assert for now.  The speed is capped eventually, so this isn't a fatal error.
	// it was meant as a notification saying "hey, you're trying to go faster than the
	// controller is going to let you go"
	//ASSERTS( speed >= 0.0f && speed <= 1.0f, "speed must be between 0 and 1" );
	Vector3 todest = dest - actor.GetBodyInWorld();
	float length = todest.Length();
	if (Math::DivByZero(1.0f, length))
		return;
	todest *= 1.0f / length;

	// scale speed down to zero as we get within half a meter of the destination
	if( length < 0.5f )
		speed = speed * ( length / 0.5f );

	// within a quarter of a meter, don't move at all
	if( length < 0.25f )
	{
		SetAxis( Biped_Axis_Move, 0.0f );
		SetAxis( Biped_Axis_Strafe, 0.0f );
	}
	else
	{
		MoveDirectionNormalized( todest, speed );
	}
}

float BipedKinematicController::MaxSpeed()
{
	// if we're crouching, limit the speed to walkspeed
	float maxspeed = Math::Min( GetAxisRange( Biped_Axis_Move ).Max(), actor.GetProperty( "cappedSpeed" )->GetFloat() );

	Property *prop = actor.QueryProperty( "moveMultiplier" );
	if( prop )
		maxspeed *= prop->GetFloat();
	prop = actor.QueryProperty( "sprint" );
	if (prop && prop->GetBool())
		maxspeed *= sprintFactor;
	prop = actor.QueryProperty( "incumberance" );
	if (prop)
		maxspeed *= (1.0f - prop->GetFloat());

	return maxspeed;
}

bool BipedKinematicController::IsSprinting()
{
	// if we're crouching, limit the speed to walkspeed
	float maxspeed = Math::Min( GetAxisRange( Biped_Axis_Move ).Max(), actor.GetProperty( "cappedSpeed" )->GetFloat() );
	Property *prop = actor.QueryProperty( "moveMultiplier" );
	if( prop )
		maxspeed *= prop->GetFloat();

	Vector3 speedInWorld = actor.GetVelocityInWorld();
	speedInWorld.Y(0.0f);
	float speedXZ = speedInWorld.LengthSquared();
	bool sprinting = speedXZ > Math::Square(maxspeed);

	return sprinting;
}

/***************************************
***************************************/
void BipedPhysicalControllerDriver::Initialize( void )
{
	apprehendDoneHandler.Initialize("ApprehendDone", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
}

/***************************************
/ DRIVER - Physical Controller
***************************************/
void BipedPhysicalControllerDriver::Attach()
{
	// we also need a view driver, to control the view
	GetActor().AttachComponent( "ViewDriver" );
	GetActor().AttachComponent( "BipedLadder" );
	GetActor().AttachComponent( "LadderPhysicalController" );
	GetActor().AttachComponent( "WallVault" );
}

char *BipedPhysicalControllerDriver::GetInteractionAnim( int type )
{
	switch( type )
	{
		case InteractionMonitor::INTERACTION_DOOR_PULL_CLOSE:
			return "DoorClose";

		case InteractionMonitor::INTERACTION_DOOR_SLIDE_OPEN:
		case InteractionMonitor::INTERACTION_DOOR_SLIDE_CLOSE:
			return "SlidingDoor";

		case InteractionMonitor::INTERACTION_LIGHT_SWITCH_ON:
		case InteractionMonitor::INTERACTION_LIGHT_SWITCH_OFF:
		case InteractionMonitor::INTERACTION_SWITCH_OPEN:
		case InteractionMonitor::INTERACTION_SWITCH_CLOSE:
		case InteractionMonitor::INTERACTION_ELEVATOR_LOCKED:
		case InteractionMonitor::INTERACTION_ELEVATOR_SWITCH:
		case InteractionMonitor::INTERACTION_JUKEBOX_AUDIO_TRAC:
			return "Button";
	}
	return NULL;
}

void BipedPhysicalControllerDriver::Deactivate()
{
	CActorComponent::Deactivate();

	KinematicController *controller = (KinematicController *)GetActor().GetActiveComponentInClass( "KinematicController" );
	if( controller && controller->IsActive() )
	{
		controller->SetAxis( Biped_Axis_Move, 0.0f );
		controller->SetAxis( Biped_Axis_Strafe, 0.0f );
	}

	// stop firing if we're on automatic
	TTLWeapon *weapon = NULL;
	WeaponInventory *inv = (WeaponInventory *)GetActor().GetComponentByName( "WeaponInventory" );
	if( inv )
		weapon = (TTLWeapon *)inv->GetSelected();

	// Is it a firearm?
	Firearm *firearm = NULL;
	if( weapon )
		firearm = (Firearm *)weapon->GetInterface("firearm");
	if( firearm && firearm->GetFiringMode() == Firearm::AUTOMATIC )
	{
		if( firearm->GetState() == Firearm::FIRING )
		{
			bool released = false;
			HealthIF *health = (HealthIF *)actor.GetInterface(HealthIF::GetID());
			bool dead = (health && !health->IsAlive());
			if( weapon && !dead )
				released = weapon->ReleaseTrigger();

			// when releasing the trigger on an auto weapon, reset the aim mode timer
			if (released)
			{
				BipedModel* model = (BipedModel*)actor.GetComponentByName("BipedModel");
				ASSERT_PTR(model);
				model->SetAimTime(actor.GetProperty("aimDuration")->GetFloat());
				model->StopAim(true);
			}
		}
	}
}

void BipedPhysicalControllerDriver::Suspend()
{
	CActorComponent::Suspend();

	KinematicController *controller = (KinematicController *)GetActor().GetActiveComponentInClass( "KinematicController" );
	if( controller && controller->IsActive() )
	{
		controller->SetAxis( Biped_Axis_Move, 0.0f );
		controller->SetAxis( Biped_Axis_Strafe, 0.0f );
	}

	// stop firing if we're on automatic
	TTLWeapon *weapon = NULL;
//	Lobbed *lobbed = NULL;
	WeaponInventory *inv = (WeaponInventory *)GetActor().GetComponentByName( "WeaponInventory" );
	if( inv )
		weapon = (TTLWeapon *)inv->GetSelected();

	// Is it a firearm?
	Firearm *firearm = NULL;
	if( weapon )
		firearm = (Firearm *)weapon->GetInterface("firearm");
	if( firearm && firearm->GetFiringMode() == Firearm::AUTOMATIC )
	{
		if( firearm->GetState() == Firearm::FIRING )
		{
			bool released = false;
			BipedModel* model = (BipedModel*)actor.GetComponentByName("BipedModel");
			ASSERT_PTR(model);

			if( weapon )
				released = weapon->ReleaseTrigger();
			// when releasing the trigger on an auto weapon, reset the aim mode timer
			model->SetAimTime(actor.GetProperty("aimDuration")->GetFloat());
			model->StopAim(true);
		}
	}
}

void BipedPhysicalControllerDriver::BeginFrame()
{
	if (!IsActive())
		return;

	if( skipControllerRead )
	{
		skipControllerRead = false;
		return;
	}

	// Am I dead?
	HealthIF *health = (HealthIF *)actor.GetInterface(HealthIF::GetID());
	ASSERT(health);
	if (health && !health->IsAlive())
		return;

	// do we have a hostage?
	bool haveHostage = false;
	Property *prop = GetActor().QueryProperty( "hostage" );
	if( prop && ((ActorHandle)prop->GetHandle()).Valid() )
	{
		// make sure our hostage is alive (safety check for mp games)
		ActorHandle hostageHandle = GetActor().GetProperty( "hostage" )->GetHandle();
		CCompActor* hostage = CCompActor::FromHandle( hostageHandle );
		if( hostage && hostage->QueryProperty( "health" ) && (hostage->GetProperty( "health" )->GetFloat() > 0.0f) )
			haveHostage = true;
		else
		{
			g_messageDispatcher.SendMessageToActor( "HostageTaker", CMessageDispatcher::HandleToData(INVALID_OBJECT_HANDLE), actor.GetHandle(), actor.GetHandle() );
			g_messageDispatcher.SendMessageToActor( "Hostage", 0, INVALID_OBJECT_HANDLE, hostageHandle );
		}
	}

	// What is our current gun
	TTLWeapon *weapon = NULL;
	Lobbed *lobbed = NULL;
	TTLWeaponInventory *inv = (TTLWeaponInventory *)GetActor().GetComponentByName( "WeaponInventory" );
	if( inv )
	{
		weapon = (TTLWeapon *)inv->GetSelected();
  		lobbed = inv->GetLobbed();
	}

	// Is it a firearm?
	Firearm *firearm = NULL;
	if( weapon )
		firearm = (Firearm *)weapon->GetInterface("firearm");

	// Are we reloading?
	bool isReloading = false;
	if (firearm && firearm->GetState() == Firearm::RELOADING)
		isReloading = true;

	// if there's no active controller, don't do anything
	KinematicController *controller = (KinematicController *)GetActor().GetActiveComponentInClass( "KinematicController" );
	if( controller == NULL || !controller->IsActive() )
		return;

	// Get our model component
	BipedModel* model = (BipedModel*)actor.GetComponentByName("BipedModel");
	ASSERT_PTR(model);

	AvatarControlMapper *avatarControl = (AvatarControlMapper *)GetActor().GetProperty("ControlMapper")->GetPointer();
	ASSERT_PTR(avatarControl);

	if( avatarControl->ButtonPressed( AvatarControlMapper::FIRE ) )
	{
		if (firearm)
		{
			if( firearm->GetFiringMode() == Firearm::AUTOMATIC )
				model->StartAimSnap();
			else
				model->StartAimSnap(actor.GetProperty("aimDuration")->GetFloat());
		}
		if( weapon && weapon->PullTrigger() )
		{
			if( weapon->GetWeaponType() == WEAPONTYPE_MELEE )
			{
				g_messageDispatcher.SendMessageToActor("BeginAttack", NULL, actor.GetHandle(), actor.GetHandle());
			}
		}
		if( model->Tagging() )
			StopTagging();
	}

	if( avatarControl->ButtonReleased( AvatarControlMapper::FIRE ) )
	{
		bool released = false;
		if( weapon )
			released = weapon->ReleaseTrigger();
		// when releasing the trigger on an auto weapon, reset the aim mode timer
		if( firearm && firearm->GetFiringMode() == Firearm::AUTOMATIC )
			model->SetAimTime(actor.GetProperty("aimDuration")->GetFloat());
		model->StopAim(true);
	}

	// Doh!
	if (weapon && weapon->GetState() == Weapon::FIRING && !avatarControl->ButtonDown(AvatarControlMapper::FIRE))
		weapon->ReleaseTrigger();

	if( avatarControl->ButtonReleased( AvatarControlMapper::RELOAD ) && !haveHostage )
	{
		if( weapon && weapon->Reload() )
		{
			model->StopAim();
			if( model->Tagging() )
				StopTagging();
			CancelAim(weapon);
		}
	}

	if( (avatarControl->ButtonPressed( AvatarControlMapper::CHANGEWEAPON ) ||
			avatarControl->ButtonPressed( AvatarControlMapper::SELECTMELEE )) &&
			!haveHostage )
	{
		int selected = inv->GetSelectedIndex();
		bool changed = false;
		if (avatarControl->ButtonPressed( AvatarControlMapper::CHANGEWEAPON ))
			changed = inv->SelectNext();
		else
		{
			if (inv->GetSelectedIndex() == 0)
				changed = inv->SelectBest();
			else if (inv->Get(0) != NULL)
				changed = inv->Select(0);
		}

		if (changed)
		{
			weapon = (TTLWeapon *)inv->GetSelected();
			if (!avatarControl->ButtonDown(AvatarControlMapper::FIRE))
				model->StopAim(false);
			if( model->Tagging() )
				StopTagging();
		}
	}

	// Handle the lobbed state
	if (lobbed)
	{
		// Doh
		// They can't do this
		bool dualWielding = weapon && weapon->IsDualWield();
		if (isReloading || haveHostage || dualWielding)
			lobbed->Cancel();
		else if (lobbed->HasAmmo())
		{
			// Handle the push
			if( avatarControl->ButtonPressed( AvatarControlMapper::HUCK ) )
			{
				lobbed->PullTrigger();
				if( model->Tagging() )
					StopTagging();
			}

			// Handle the release
			if( avatarControl->ButtonReleased( AvatarControlMapper::HUCK ) )
			{
				if (lobbed->ReleaseTrigger())
				{
					g_messageDispatcher.SendMessageToActor("BeginThrow", NULL, actor.GetHandle(), actor.GetHandle());
					CancelAim(weapon);
					if( model->Tagging() )
						StopTagging();
				}
			}
		}

		// Make sure they didn't go into the menus or something
		if (lobbed->GetState() == Lobbed::FIRING && !avatarControl->ButtonDown(AvatarControlMapper::HUCK))
			lobbed->Cancel();
	}

	ZoomSight *aimSight = NULL;
	if( weapon )
		aimSight = weapon->GetZoomSight();

	// Sprint!
	int peekLeft, peekRight;
	peekLeft = GetActor().Script( ANIM1_SCRIPT )->GetInt( "peekleft" );
	peekRight = GetActor().Script( ANIM1_SCRIPT )->GetInt( "peekright" );

	if( avatarControl->ButtonDown( AvatarControlMapper::PEEKLEFT ) )
	{
		if( !peekLeft && !haveHostage )
			g_messageDispatcher.SendMessageToActor( "PeekLeft", NULL, actor.GetHandle(), actor.GetHandle() );
		peekLeft = true;
	}
	else
	{
		if( peekLeft && !haveHostage )
			g_messageDispatcher.SendMessageToActor( "StopPeekLeft", NULL, actor.GetHandle(), actor.GetHandle() );
		peekLeft = false;
	}
	if( avatarControl->ButtonDown( AvatarControlMapper::PEEKRIGHT ) )
	{
		if( !peekRight && !haveHostage )
			g_messageDispatcher.SendMessageToActor( "PeekRight", NULL, actor.GetHandle(), actor.GetHandle() );
		peekRight = true;
	}
	else
	{
		if( peekRight && !haveHostage )
			g_messageDispatcher.SendMessageToActor( "StopPeekRight", NULL, actor.GetHandle(), actor.GetHandle() );
		peekRight = false;
	}


	if (avatarControl->ButtonPressed(AvatarControlMapper::AIM) && aimSight && aimSight->ZoomLevels() != 0)
	{
 		int onLevel = aimSight->ZoomLevel();
		int nextLevel = aimSight->NextZoomLevel();
		if (nextLevel != onLevel && lobbed)
			lobbed->Cancel();
	}

	// Do a taunt?
	if (avatarControl->ButtonPressed( AvatarControlMapper::TAUNT ) &&
			!avatarControl->ButtonDown( AvatarControlMapper::PEEKLEFT) && !avatarControl->ButtonDown( AvatarControlMapper::PEEKRIGHT) )
	{
		//out with the old
		//Taunt *taunt = (Taunt *)actor.GetComponentByName( "Taunt" );
		//if (taunt)
		//	taunt->SayRandom();

		//in with the new
		Say *sayComponent = (Say *)actor.GetComponentByName( "Say" );
		if (sayComponent)
			sayComponent->PlayVoiceType(Say::VOICETYPE_TAUNT,0.0f,0.1f,false);

#ifdef TAUNT_ANIMS_DIDNT_SUCK
		if (!haveHostage)
		{
			g_messageDispatcher.SendMessageToActor( "TauntAnim", NULL, actor.GetHandle(), actor.GetHandle() );
			CancelAim(weapon);
		}
#endif
	}

	// Do a tag?
	if (avatarControl->ButtonPressed( AvatarControlMapper::TAG ) &&
			!avatarControl->ButtonDown( AvatarControlMapper::PEEKLEFT) && !avatarControl->ButtonDown( AvatarControlMapper::PEEKRIGHT) )
	{
		QuickTag *quickTag = (QuickTag *)actor.GetComponentByName( "QuickTag" );
		ViewIF *viewIF = (ViewIF *)actor.GetInterface(ViewIF::GetID());
		if (quickTag && viewIF)
		{
			Vector3 origin, dir;
			viewIF->GetEyepoint(origin);
			viewIF->GetDirection(dir);
			if (quickTag->Tag(origin, dir))
				CancelAim(weapon);
		}
	}

	// init all axes to zero
	controller->SetAxis( Biped_Axis_Move, 0.0f );
	controller->SetAxis( Biped_Axis_Strafe, 0.0f );

	float y = avatarControl->GetAxis(AvatarControlMapper::FORWARDBACK);
	float x = avatarControl->GetAxis(AvatarControlMapper::STRAFE);

	// no movement while peeking or kicking a door open
	if( peekLeft || peekRight || GetActor().AnimAction() == ACTION_ROAM_KICK )
	{
		x = 0.0f;
		y = 0.0f;
	}

	if( Math::Abs( x ) > 0.001f || Math::Abs( y ) > 0.001f )
	{
		Vector3 targetdir;
		ViewIF *viewIF = NULL;

		viewIF = (ViewIF *)GetActor().GetInterface( ViewIF::GetID() );
		if( viewIF )
		{
			DirCos3x3 m;
			m = viewIF->SightMatrix();
			targetdir = ( y * m.GetRow2() ) + ( -x * m.GetRow0() );
		}
		else
		{
			Vector3 sight, side;
			sight.Set( 0.0f, 0.0f, 1.0f );
			side.Set( 1.0f, 0.0f, 0.0f );
			targetdir = ( y * sight ) + ( -x * side );
		}

		((BipedKinematicController *)controller)->MoveDirectionNormalized( targetdir );
	}
	else
	{
		Vector3 targetdir;

		ViewIF *viewIF = (ViewIF *)GetActor().GetInterface( ViewIF::GetID() );
		CMoveIF *moveIF = (CMoveIF *)controller->GetInterface( CMoveIF::GetID() );
		if( moveIF && viewIF )
		{
			viewIF->GetDirection( targetdir );
			moveIF->CommandTurnToPoint( GetActor(), actor.GetBodyInWorld() + targetdir );
		}
	}

	// for some things, such as wall vaults, we want to be able to just use the jump button.
	bool jump = false, use = false;
	if( avatarControl->ButtonPressed( AvatarControlMapper::JUMP ) )
		jump = true;
	if( avatarControl->ButtonPressed( AvatarControlMapper::USE ) )
		use = true;

	// jump?
	if( jump && actor.GetProperty( "onGround" )->GetBool() && !model->Kicking() && !model->Tagging() && !haveHostage )
	{
		if( model->Crouching() )
			model->StopCrouch();

		g_messageDispatcher.SendMessageToActor("Jump", NULL, actor.GetHandle(), actor.GetHandle());
		if( weapon )
			weapon->GetAimSim().TriggerEvent("jump");
		CancelAim(weapon);
	}

	// crouch?
	if( avatarControl->ButtonDown( AvatarControlMapper::CROUCH) &&  !model->Crouching() &&
		!model->Tagging() && !haveHostage && !jump && actor.GetProperty( "onGround" )->GetBool() )
		model->StartCrouch();
	if( model->Crouching() && !avatarControl->ButtonDown( AvatarControlMapper::CROUCH) && !haveHostage )
		model->StopCrouch();

	// use?
	if( use )
	{
		InteractionMonitor *monitor = (InteractionMonitor *)GetActor().GetComponentByName( "InteractionMonitor" );
		if( monitor )
		{

#ifdef DIRECTX_PC
			// if we're interacting with something, and we're in first person, take us out
			if( (monitor->GetInteractionType() == InteractionMonitor::INTERACTION_VAULT ||
				 monitor->GetInteractionType() == InteractionMonitor::INTERACTION_CLIMB ||
				 monitor->GetInteractionType() == InteractionMonitor::INTERACTION_LADDER ||
				 monitor->GetInteractionType() == InteractionMonitor::INTERACTION_TAG_FRAME ||
				 monitor->GetInteractionType() == InteractionMonitor::INTERACTION_FENCE ) &&
				 g_inputPtr && g_inputPtr->GetController(0)->InFirstPerson() )
			{
				g_shooterCam.FirstPersonLock(false);
				lockCamera = true;
			}
#endif

			if( monitor->GetInteractionType() == InteractionMonitor::INTERACTION_HOSTAGE )
			{
				if( weapon )
					weapon->Cancel();
				if( lobbed )
					lobbed->Cancel();
				CancelAim(weapon);

				TTLWeapon *secWeapon = NULL;
				if( inv )
					secWeapon = (TTLWeapon*)(inv->Get(TTLWeapon::SECONDARY));

				// Do we have a secondary weapon needed to hold a hostage?
				if( !model->Jumping() && (secWeapon != NULL) )
				{
					CProtagonist *target = monitor->GetInteractionTargetActor();
					bool takeHostage = true;

					if( target->IsA( CCompActor::ClassType() ) )
					{
						// make sure our target is still alive!
						PlayerStats *pStats = (PlayerStats*)((CCompActor*)target)->GetInterface(PlayerStats::GetID());
						if( pStats && !pStats->IsAlive() )
							takeHostage = false;

						// are they on fire?  If so, fail, and catch on fire ourselves
						Afflictions *afflict = (Afflictions *)(((CCompActor *)target)->GetComponentByName( "Afflictions" ));
						if( afflict && afflict->Afflicted( Afflictions::ONFIRE ) )
						{
							g_messageDispatcher.SendMessageToActor("MunitionOnFire", CMessageDispatcher::IntToData(-1), target->GetHandle(), GetActor().GetHandle());
							takeHostage = false;
						}

					}
					if( takeHostage )
					{
						g_messageDispatcher.SendMessageToActor( "HostageTaker", CMessageDispatcher::HandleToData(target->GetHandle()), actor.GetHandle(), actor.GetHandle() );
						g_messageDispatcher.SendMessageToActor( "Hostage", 0, actor.GetHandle(), target->GetHandle() );
					}
				}
			}
			else if( monitor->GetInteractionType() == InteractionMonitor::INTERACTION_RELEASEHOSTAGE )
			{
				CProtagonist *target = monitor->GetInteractionTargetActor();
				g_messageDispatcher.SendMessageToActor( "HostageTaker", CMessageDispatcher::HandleToData(INVALID_OBJECT_HANDLE), actor.GetHandle(), actor.GetHandle() );
				g_messageDispatcher.SendMessageToActor( "Hostage", 0, INVALID_OBJECT_HANDLE, target->GetHandle() );
				CancelAim(weapon);
			}
			else if( monitor->GetInteractionType() == InteractionMonitor::INTERACTION_APPREHEND )
			{
				CProtagonist *target = monitor->GetInteractionTargetActor();
				g_messageDispatcher.SendMessageToActor( "Apprehend", NULL, target->GetHandle(), actor.GetHandle() );
				g_messageDispatcher.SendMessageToActor( "Apprehended", NULL, INVALID_OBJECT_HANDLE, target->GetHandle() );
				g_messageDispatcher.SendMessageToOwner( "RecordApprehend", (void*)CParticipantIF::GetParticipantIdFromActor(actor.GetHandle()),
						target->GetHandle(), g_referee.GetMessageOwner() );
				if (g_shooterCam.GetFollow() == &actor)
					g_shooterCam.LockToTarget(target->GetBodyInWorld());
				CancelAim(weapon);
			}
			else if( monitor->GetInteractionType() == InteractionMonitor::INTERACTION_LADDER ||
					monitor->GetInteractionType() == InteractionMonitor::INTERACTION_FENCE )
			{
				if( weapon )
					weapon->Cancel();
				CancelAim(weapon);

				BipedLadderController *laddercontroller = (BipedLadderController *)GetActor().GetComponentByName( "BipedLadder" );
				CActorComponent *ladderdriver = GetActor().GetComponentByName( "LadderPhysicalController" );

				GetActor().GetActiveComponentInClass( "KinematicController" )->Deactivate();
				GetActor().GetActiveComponentInClass( "KinematicDriver" )->Deactivate();

				if( laddercontroller )
				{
					if( monitor->GetInteractionObjectType() == InteractionMonitor::OBJECT_STATICOBB )
					{
						bool oneSided = false;
						if( monitor->GetInteractionType() == InteractionMonitor::INTERACTION_LADDER )
							oneSided = true;
						laddercontroller->SetLadder( (OBB *)monitor->GetInteractionTargetObject(), oneSided );
					}
					else if( monitor->GetInteractionObjectType() == InteractionMonitor::OBJECT_ACTOR )
					{
						CProtagonist *target = monitor->GetInteractionTargetActor();
						CCollisionObject *obj = target->GetCollisionObject();
						ASSERT( obj->GetObjectType() == CCollisionObject::OBB_OBJECT );
						bool oneSided = false;
						if( monitor->GetInteractionType() == InteractionMonitor::INTERACTION_LADDER )
							oneSided = true;
						laddercontroller->SetLadder( (OBB *)obj, oneSided );
					}
				}
				if( model->Crouching() )
					model->StopCrouch();
				model->StartLadder();
			}
			else if( !model->Jumping() ) // don't allow door kicks, vaults, climbs, or tagging while jumping
			{
				if( monitor->GetInteractionType() == InteractionMonitor::INTERACTION_VAULT ||
					monitor->GetInteractionType() == InteractionMonitor::INTERACTION_CLIMB )
				{
					if( weapon )
						weapon->Cancel();
					CancelAim(weapon);

					WallVault *vault = (WallVault *)GetActor().GetComponentByName( "WallVault" );

					if( vault )
					{
						if( monitor->GetInteractionObjectType() == InteractionMonitor::OBJECT_STATICOBB )
						{
							vault->SetVault( (OBB *)monitor->GetInteractionTargetObject() );
						}
						else if( monitor->GetInteractionObjectType() == InteractionMonitor::OBJECT_ACTOR )
						{
							CProtagonist *target = monitor->GetInteractionTargetActor();
							vault->SetVault( target );
						}
						vault->Activate();
					}
				}
				else if( monitor->GetInteractionType() == InteractionMonitor::INTERACTION_TAG_FRAME )
				{
					// don't tag if we're firing
					bool allowTag = true;
					if( firearm && firearm->GetFiringMode() == Firearm::AUTOMATIC )
					{
						if( firearm->GetState() == Firearm::FIRING )
							allowTag = false;
					}

					if( allowTag )
					{
						if( weapon )
							weapon->Cancel();
						CancelAim(weapon);

						CProtagonist *target = monitor->GetInteractionTargetActor();
						if( target )
						{
							frameHandle = target->GetHandle();
							g_messageDispatcher.SendMessageToActor( "SimpleInteraction", 0, actor.GetHandle(), target->GetHandle() );
						}
					}
				}
				else
				{
					CProtagonist *target = monitor->GetInteractionTargetActor();
					if( target )
					{
						// This is a hack!
						CancelAim(weapon);
						if( monitor->GetInteractionType() == InteractionMonitor::INTERACTION_DOOR_LOCKED ||
							monitor->GetInteractionType() == InteractionMonitor::INTERACTION_DOOR_SLIDE_OPEN ||
							monitor->GetInteractionType() == InteractionMonitor::INTERACTION_DOOR_SLIDE_CLOSE ||
							monitor->GetInteractionType() == InteractionMonitor::INTERACTION_DOOR_KICK_OPEN ||
							monitor->GetInteractionType() == InteractionMonitor::INTERACTION_DOOR_KICK_CLOSE ||
							monitor->GetInteractionType() == InteractionMonitor::INTERACTION_DOOR_PULL_CLOSE ||
							monitor->GetInteractionType() == InteractionMonitor::INTERACTION_LIGHT_SWITCH_ON ||
							monitor->GetInteractionType() == InteractionMonitor::INTERACTION_LIGHT_SWITCH_OFF ||
							monitor->GetInteractionType() == InteractionMonitor::INTERACTION_SWITCH_OPEN ||
							monitor->GetInteractionType() == InteractionMonitor::INTERACTION_SWITCH_CLOSE ||
							monitor->GetInteractionType() == InteractionMonitor::INTERACTION_ELEVATOR_LOCKED	||
							monitor->GetInteractionType() == InteractionMonitor::INTERACTION_ELEVATOR_SWITCH ||
							monitor->GetInteractionType() == InteractionMonitor::INTERACTION_JUKEBOX_AUDIO_TRAC ||
							monitor->GetInteractionType() == InteractionMonitor::INTERACTION_SWAP_WEAPON ||
							monitor->GetInteractionType() == InteractionMonitor::INTERACTION_SWAP_LOADOUT )
						{

							if( monitor->GetInteractionType() == InteractionMonitor::INTERACTION_DOOR_KICK_OPEN ||
								monitor->GetInteractionType() == InteractionMonitor::INTERACTION_DOOR_KICK_CLOSE )
							{
								CActor *target = monitor->GetInteractionTargetActor();
								if( target )
								{
									((BipedKinematicController *)controller)->SetInteractionActor( target->GetHandle() );
									((BipedKinematicController *)controller)->SetInteractionType( monitor->GetInteractionType() );
								}
								else
								{
									((BipedKinematicController *)controller)->SetInteractionActor( INVALID_OBJECT_HANDLE );
									((BipedKinematicController *)controller)->SetInteractionType( InteractionMonitor::INTERACTION_NONE );
								}

								g_messageDispatcher.SendMessageToActor("Kick", NULL, actor.GetHandle(), actor.GetHandle());
								if( weapon )
									weapon->GetAimSim().TriggerEvent("kick");
							}
							else // need to force the interaction now
							{
								Vector3 forward;
								ViewIF *viewIF = (ViewIF *)GetActor().GetInterface( ViewIF::GetID() );

								char *interactionAnim = GetInteractionAnim( monitor->GetInteractionType() );
								if( interactionAnim )
									g_messageDispatcher.SendMessageToActor( interactionAnim, NULL, actor.GetHandle(), actor.GetHandle() );

								if( viewIF )
									viewIF->GetDirection( forward );
								else
									forward = GetActor().GetBodyToWorld().GetRow2();

								forward *= 20.0f;

								g_messageDispatcher.SendMessageToActor( "SimpleInteraction", &forward, actor.GetHandle(), target->GetHandle() );
							}
							CMoveIF *moveIF = (CMoveIF *)controller->GetInterface( CMoveIF::GetID() );
							if( moveIF )
							{
								CCollisionIF *collisionIF = (CCollisionIF *)target->GetInterface( CCollisionIF::GetID() );
								if( collisionIF )
									moveIF->CommandTurnToPoint( GetActor(), collisionIF->GetCollisionInWorld() );
								else
									moveIF->CommandTurnToPoint( GetActor(), target->GetBodyInWorld() );
							}
						}
					}
				}
			}

			// If we try to interact with something while tagging then cancel the tagging
			if( model->Tagging() && monitor->GetInteractionType() != InteractionMonitor::INTERACTION_TAG_FRAME )
				StopTagging();
		}
	}

#ifdef DIRECTX_PC
	else
	{
		if ( g_inputPtr && g_inputPtr->GetController(0)->InFirstPerson() && lockCamera )
		{
			g_shooterCam.FirstPersonLock(true);
			lockCamera = false;
		}
		else if (g_inputPtr && !g_inputPtr->GetController(0)->InFirstPerson() && !aimSight->ZoomLevel())
		{
			g_shooterCam.FirstPersonLock(false);
			lockCamera = true;
		}
	}
#endif

	// Stop tagging if the use button is released or no longer down
	if ( model->Tagging() && ( avatarControl->ButtonReleased( AvatarControlMapper::USE ) ||
			!avatarControl->ButtonDown( AvatarControlMapper::USE )) )
		StopTagging();

	// Are we reloading, stop aiming
#if 0
	if (weapon->GetState() == Firearm::RELOADING)
		controller->EndAction( Biped_Action_Aim );
#endif

#if 0
	extern void DebugShots();
	DebugShots();
#endif
}

// Utilities
void BipedPhysicalControllerDriver::CancelAim(TTLWeapon *weapon)
{
	if (weapon)
	{
		ZoomSight *aimSight = weapon->GetZoomSight();
	 	aimSight->SelectZoomLevel(0);
	}
}

void BipedPhysicalControllerDriver::StopTagging()
{
	if (frameHandle != INVALID_OBJECT_HANDLE)
	{
		g_messageDispatcher.SendMessageToActor( "StopInteraction", NULL, GetActor().GetHandle(), frameHandle );
		frameHandle = INVALID_OBJECT_HANDLE;
	}
}

void BipedPhysicalControllerDriver::ApprehendDone::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	if (g_shooterCam.GetFollow() == &d_ref->actor)
		g_shooterCam.UnlockTarget();
}


#ifdef _DEBUG
/***************************************
/ DRIVER - Point to point driver
***************************************/
typedef struct tag_PointToPoint
{
	Vector3 point;
	float speed;
	Vector3 aimdir;
}PointToPoint;

PointToPoint points[] =
{
	{ Vector3( 0.0f, 0.0f,  0.0f ), 4.5f, Vector3( 0.0f, 0.0f, 1.0f ) },
	{ Vector3( 0.0f, 0.0f,  3.0f ), 4.5f, Vector3( 0.0f, 0.0f, 0.0f ) },
	{ Vector3( 0.0f, 0.0f,  6.0f ), 4.25f, Vector3( 0.0f, 0.0f, 0.0f ) },
	{ Vector3( 0.0f, 0.0f,  9.0f ), 4.0f, Vector3( 0.0f, 0.0f, 0.0f ) },
	{ Vector3( 0.0f, 0.0f, 12.0f ), 3.75f, Vector3( 0.0f, 0.0f, 0.0f ) },
	{ Vector3( 0.0f, 0.0f, 15.0f ), 3.5f, Vector3( 0.0f, 0.0f, 0.0f ) },
};

BipedPointToPointDriver::BipedPointToPointDriver( CCompActor &i_actor ) :
CActorComponent( i_actor )
{
	movingToPoint = 0;
}

float PointToPointSpeed = 4.5f;
void BipedPointToPointDriver::AttemptUpdate( float dt )
{
	BipedKinematicController *controller = (BipedKinematicController *)GetActor().GetComponentByName( "Biped" );

	Vector3 dir = points[movingToPoint].point - GetActor().GetBodyInWorld();
	float dist = dir.Length();
	dir.SafeNormalize();
	Vector3 *aim = &(points[movingToPoint].aimdir);
	if( aim->Length() < 0.1f )
		aim = NULL;
	ViewIF *viewIF = (ViewIF *)GetActor().GetInterface( ViewIF::GetID() );
	if( viewIF && aim )
		viewIF->LookAt( GetActor().GetBodyInWorld(), GetActor().GetBodyInWorld() + *aim );
	controller->MoveDirection( dir, points[movingToPoint].speed );

	// within a quarter meter of the point?
	if( dist < 0.25f )
	{
		movingToPoint++;
		if( movingToPoint >= ( sizeof( points ) / sizeof( PointToPoint ) ) )
			movingToPoint = 0;
	}

	//ControlCamera *cam = (ControlCamera *)&(g_cameraDirector.CurrentCamera());
	//Vector3 eye = cam->Eye();
	//eye.x( GetActor().GetBodyInWorld().x() - 1.5f);
	//eye.z( GetActor().GetBodyInWorld().z() );
	//eye.y( 0.5f );
	//cam->SetEye( eye );
}
#endif

void MoveToPointDriver::BeginFrame( void )
{
//	if( !IsActive() )
//		return;

	bool done = false;

	CMoveIF *moveIF = (CMoveIF *)GetActor().GetInterface( CMoveIF::GetID() );
	ViewIF *viewIF = (ViewIF *)GetActor().GetInterface( ViewIF::GetID() );
	Vector3 up;
	up.Set( 0.0f, 1.0f, 0.0f );
	if( moveIF )
	{
		float d;

		if( viewIF )
			viewIF->LookAt( destination, up );
		moveIF->CommandMoveToPoint( destination, speed, true );
		d = moveIF->DistanceSquaredToPoint( destination );
		if( d < 0.1f )
		{
			if( viewIF )
				viewIF->LookAt( GetActor().GetBodyInWorld() + GetActor().GetBodyToWorld().GetRow2(), up );
			moveIF->CommandTurnToPoint( GetActor(), GetActor().GetBodyInWorld() + orientation.GetRow2() );
			moveIF->CommandForwardSpeed( 0.0f );
			moveIF->CommandSideSpeed( 0.0f );
			d = GetActor().GetBodyToWorld().GetRow2().Dot( orientation.GetRow2() );
			if( d > 0.9f )
			{
				done = true;
				if( viewIF )
					viewIF->LookAt( GetActor().GetBodyInWorld() + orientation.GetRow2(), up );
			}
		}
	}
	else
		done = true;

	if( done )
		GetActor().DetachComponent( this );
}

void MoveToPointDriver::MoveToPointAndFacing( Vector3CRef point, const DirCos3x3 &i_orientation, float normalizedSpeed )
{
	destination = point;
	speed = normalizedSpeed;
	orientation = i_orientation;
}

CInterface* BipedKinematicController::GetInterface(uint i_id)
{
	if( IsActive() )
	{
		if (i_id == CMoveIF::GetID())
			return(&d_moveIF);
	}
	return(NULL);
}

bool BipedMoveIF::CommandTurnRad(float i_turnRad)
{
	float axis = d_controller.GetAxis( Biped_Axis_Turn );
	d_controller.SetAxis( Biped_Axis_Turn, axis + i_turnRad );
	return(true);
}

bool BipedMoveIF::CommandHeadingRad(float i_headingRad)
{
	d_controller.SetAxis( Biped_Axis_Turn, i_headingRad );
	return(true);
}

bool BipedMoveIF::CommandTurnRateRad(float i_turnRateRad, bool i_normalized)
{
	if (i_normalized)
		d_controller.SetAxisNormalized( Biped_Axis_TurnRate, i_turnRateRad );
	else
		d_controller.SetAxis( Biped_Axis_TurnRate, i_turnRateRad );
	return(true);
}

bool BipedMoveIF::CommandMoveDirection(Vector3CRef i_dir, float i_speed, bool i_normalized)
{
	if (i_normalized)
		d_controller.MoveDirectionNormalized(i_dir, i_speed);
	else
		d_controller.MoveDirection(i_dir, i_speed);
	return(true);
}

bool BipedMoveIF::CommandMoveDirection(Vector3CRef i_velocity, bool i_normalized)
{
	if (i_normalized)
		d_controller.MoveDirectionNormalized(i_velocity);
	else
		d_controller.MoveDirection(i_velocity);
	return(true);
}

bool BipedMoveIF::CommandMoveToPoint(Vector3CRef i_point, float i_speed, bool i_normalized)
{
	if (i_normalized)
		d_controller.MoveToPointNormalized(i_point, i_speed);
	else
		d_controller.MoveToPoint(i_point, i_speed);
	return(true);
}

float BipedMoveIF::DistanceSquaredToPoint( Vector3CRef i_point )
{
	Vector3 toPoint = i_point - d_controller.GetActor().GetBodyInWorld();
	toPoint.Y( 0.0f );
	return toPoint.LengthSquared();
}

bool BipedMoveIF::CommandForwardSpeed(float i_forwardSpeed, bool i_normalized)
{
	if (i_normalized)
		d_controller.SetAxisNormalized( Biped_Axis_Move, i_forwardSpeed );
	else
		d_controller.SetAxis( Biped_Axis_Move, i_forwardSpeed );
	return(true);
}

bool BipedMoveIF::CommandSideSpeed(float i_sideSpeed, bool i_normalized)
{
	if (i_normalized)
		d_controller.SetAxisNormalized( Biped_Axis_Strafe, i_sideSpeed );
	else
		d_controller.SetAxis( Biped_Axis_Strafe, i_sideSpeed );
	return(true);
}

bool BipedMoveIF::Crouch( bool crouch )
{
	BipedModel* model = (BipedModel*)d_controller.GetActor().GetComponentByName("BipedModel");
	if( model )
	{
		if( crouch && !model->Crouching() )
			model->StartCrouch();
		else if( !crouch && model->Crouching() )
			model->StopCrouch();
	}
	return true;
}

bool BipedMoveIF::JumpToTarget( Vector3CRef i_point )
{
	// Figure out the 2d quantities
	CActor *act = &(d_controller.GetActor());
	Vector3 d = i_point - act->GetBodyInWorld();

	d = d / act->GetBodyToWorld();

	float yt = d.Y();
	float xt = Math::Sqrt(Math::Square(d.X()) + Math::Square(d.Z()));
	float launchAngle = Math::Deg2Rad( 45.0f );
	float launchV;

	// what should the launch velocity be?
	float root = 2.0f / -d_controller.GetGravity() * (xt * Math::Tan(launchAngle) - yt);
	Vector3 v;
	if( root < 0.0f )
	{
		// failed to solve it.  try with a 60 degree launch angle
		launchAngle = Math::Deg2Rad( 60.0f );
		root = 2.0f / -d_controller.GetGravity() * (xt * Math::Tan(launchAngle) - yt);
	}

	// if the root is still less than zero, then just jump straight towards the target
	// with enough y velocity that gravity will pull us to zero by the time we get there
	if( root < 0.0f )
	{
		v = d.Normalized();
		if( d.Y() < 0.0f )
			v.Clear(); // no velocity jump, since the point is below us, but too close to jump off an edge too
		else
		{
			v.y( Math::Sqrt( -d_controller.GetGravity() * d.Y() * 2.0f ) );
		}
	}
	else
	{
		launchV = xt / (Math::Cos(launchAngle) * Math::Sqrt(root));

		// construct a vector that is in the same direction as d, but with a 45 degree angle
		d.y( 0.0f );
		d.SafeNormalize();
		v = d * Math::Cos(launchAngle);
		v.y( Math::Sin(launchAngle) );
		v = v * launchV;
	}
	g_messageDispatcher.SendMessageToActor( "JumpWithVelocity", &v, act->GetHandle(), act->GetHandle() );
	return true;
}
