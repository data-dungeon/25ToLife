#include "TTLPCH.h"
#include "components/Ai/AiHeli.h"
#include "Components/ComponentCreator.h"
#include "main/PlayerDB.h"
#include "weapons/seethru.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(AiHeli, AiHeli, COMPONENT_NEUTRAL_PRIORITY);

#define SIN_BASE 0.8f
#define SIN_VARIANCE 0.8f
#define MOVE_DISTANCE_MIN 100000.0f
#define HOVER_FROM_TARGET_MAX 50.0f * 50.0f
#define HELI_SIGHT_DIST_MIN 9.0f * 9.0f
#define HELI_SIGHT_DIST_MAX 300.0f * 300.0f
#define HELI_SIGHT_DOT -0.8f
#define TURNSPEED_MAX 0.4f
#define HEADING_RAND 0.02f
#define TURNAMOUNT_DEADZONE 0.1f
#define DISTANCE_TO_TARGET_MIN 15.0f
#define DISTANCE_OFF_PATH_MAX 200.0f
#define HELI_SPEED 1.0f
#define HITS_TIL_FLEE 3
#define FLEE_RISE 1.0f
#define ALTITUDE_CEILING 400.0f
#define FLEE_FACTOR 0.01f
#define VISUAL_CHECK_TIME 0.5f
#define SEND_EVENT_TIME 2.0f
#define AGGRO_TIME_OUT 30.0f
//#define SIGHT_DEBUG

//============================================================================
AiHeli::AiHeli( CCompActor &i_actor ) :
CActorComponent( i_actor )
{
}
//============================================================================
AiHeli::~AiHeli()
{
	if(heliSoundHandle != NULL)
		Sfx::Stop(heliSoundHandle);
	heliSoundHandle = NULL;
}
//============================================================================
void AiHeli::Initialize( void )
{
	d_munitionImpactHandler.Initialize(MUNITION_IMPACT_ACTOR, MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	pathFollower = (CPathFollower*)( actor.GetComponentByName( "PathFollower" ) );
	ASSERT(pathFollower);
	heliSoundHandle = NULL;
	Reset();
}
//============================================================================
void AiHeli::Reset( void )
{
	heliState = HELI_AT_START;
	destination = actor.GetBodyInWorld();
	startPosition = destination;
	anchor = destination;
	tether = 0.0f;
	sinTime = 0.0f;
	sinFactor = SIN_BASE;
	altitude = actor.GetBodyInWorld().y();
	pathFollower->SetSpeedTarget(0.0f);
	pathFollower->AdjustSpeed(0.0f);
	pathFollower->SetFaceMovementDirection(false);
	pathFollower->Deactivate();
	inSight = false;
	sightTimer = 0.0f;
	lastSeenPosition.Clear();
	rotation = 0.0f;
	velocityVec = 0.0f;
	hitCount = 0;
	timeRise = 0.0f;
	visualCheckTimer = 0.0f;
	sendEventTimer = 0.0f;
	aggroTimeOut = 0.0f;

	Activate(true); //temp
}
//============================================================================
void AiHeli::BeginFrame()
{
	float frameSec = g_timer.GetFrameSec();

	Math::MoveToZero(&visualCheckTimer,frameSec);
	Math::MoveToZero(&sendEventTimer,frameSec);
	if(!inSight && aggroTimeOut > 0.0f)
		aggroTimeOut += frameSec;

	StateControl();
	RunState(frameSec);

	inSight = AvatarInSight();

	if(inSight && sendEventTimer == 0.0f)
	{
		aggroTimeOut += 0.001f;
		sendEventTimer = SEND_EVENT_TIME;
		g_messageDispatcher.SendMessageToAll("AvatarSpotted",NULL,actor.GetHandle());
	}

#ifdef SIGHT_DEBUG
	if (GetHeliTarget())
	{
		if(inSight)
			DrawUtility::Line(actor.GetFocusInWorld(),GetHeliTarget()->GetFocusInWorld(),DrawUtility::CYAN);
		else
			DrawUtility::Line(actor.GetFocusInWorld(),GetHeliTarget()->GetFocusInWorld(),DrawUtility::RED);
	}
#endif

	//if(inSight)
	//	FollowPlayer();

	//dbgPrint(":::::::::altitude %f\n",altitude);
}
//============================================================================
void AiHeli::EndUpdate(float i_deltaSec)
{
	actor.GetBodyInWorld().Set(destination);
	switch(heliState)
	{
		case HELI_SPOTTED_ON_PATH:
		case HELI_SPOTTED_OFF_PATH:
		case HELI_MOVING_TO_SEE:
		case HELI_FLEEING:
			actor.GetBodyToWorld().SetYRotation(rotation);
			break;
	}
}
//============================================================================
void AiHeli::FollowTarget()
{
	if(heliState == HELI_PATHING)
	{
		pathFollower->SetSpeedTarget(0.0f);
		pathFollower->AdjustSpeed(0.0f);
		pathFollower->SetFaceMovementDirection(false);
		pathFollower->Deactivate();
		heliState = HELI_SPOTTED_ON_PATH;
		return;
	}

	if(heliState == HELI_SPOTTED_ON_PATH && !inSight)
		heliState = HELI_MOVING_TO_SEE;

	Vector3 proposedDestination = actor.GetBodyInWorld();

	if(heliState == HELI_MOVING_TO_SEE)
	{
		lastSeenPosition.y(actor.GetBodyInWorld().y());
		Vector3 toTarget = lastSeenPosition - actor.GetBodyInWorld();
		if(toTarget.LengthXZSquared() < MOVE_DISTANCE_MIN)
			return;
		toTarget.y(actor.GetBodyInWorld().y());
		proposedDestination = actor.GetBodyInWorld() + (toTarget * 0.5f);
	}
	//DrawUtility::Line(actor.GetFocusInWorld(),proposedDestination,DrawUtility::BLUE);
}
//============================================================================
void AiHeli::TurnToTarget(float i_frameSec)
{
	if (GetHeliTarget() == NULL)
		return;
		
	Vector3 toTarget =  GetHeliTarget()->GetFocusInWorld() - actor.GetBodyInWorld();
	if( toTarget.SafeNormalize() )
	{
		Vector3 forward = actor.GetBodyToWorld().GetRow2();
		float heading = Math::ArcTan2( forward.x(), forward.z() );
		float newheading = Math::ArcTan2( toTarget.x(), toTarget.z() );
		newheading += g_random.InRange(-1.0f * HEADING_RAND,HEADING_RAND);
		float difference = Math::NormalAngle(newheading - heading);
		if(Math::Abs(difference) > TURNAMOUNT_DEADZONE)
		{
			if(Math::Abs(difference) > (TURNSPEED_MAX * i_frameSec))
				rotation = heading + ((TURNSPEED_MAX * i_frameSec) * Math::Sign(difference));
			else
				rotation = newheading;
		}
		else
			rotation = heading;
	}
}
//============================================================================
void AiHeli::TurnToFoward(float i_frameSec)
{
}
//============================================================================
void AiHeli::MoveToExit()
{
	//up and away
}
//============================================================================
void AiHeli::MoveAlongPath()
{
	if(heliState != HELI_PATHING)
	{
		pathFollower->SetSpeedTarget(pathFollower->GetOrgSpeedTarget());
		pathFollower->SetFaceMovementDirection(true);
		pathFollower->Activate();
		heliState = HELI_PATHING;
	}
}
//============================================================================
bool AiHeli::AvatarInSight()
{
	//if timer not up return inSight
	if(!inSight)
	{
		if(visualCheckTimer > 0.0f)
			return false;
	}
	else
		visualCheckTimer = VISUAL_CHECK_TIME;

	if (GetHeliTarget() == NULL)
		return false;

	//distance check
	Vector3 targetPos = GetHeliTarget()->GetFocusInWorld();
	Vector3 toTarget = targetPos - actor.GetBodyInWorld();
	float xzDist2rd = toTarget.LengthXZSquared();
	if(xzDist2rd < HELI_SIGHT_DIST_MIN || xzDist2rd > HELI_SIGHT_DIST_MAX)
		return false;

	//dot check (can't see through tail
	float dotcheck = actor.GetBodyToWorld().GetRow2().Dot(toTarget.Normalized());
	if(dotcheck < HELI_SIGHT_DOT)
		return false;

	if(CordCheck(g_players.GetPlayer(0)))
		return true;

	return false;
}
//============================================================================
void AiHeli::Altitude(float i_deltaSec)
{
	sinTime += i_deltaSec;
	if(sinTime > Math::TwoPi)
	{
		sinTime -= Math::TwoPi;
		sinFactor = SIN_BASE + g_random.InRange(0.0f,SIN_VARIANCE) - (SIN_VARIANCE * 0.5f);
	}
	altitude = sinFactor * Math::Sin(sinTime);
}
//============================================================================
void AiHeli::Activate(bool i_activate)
{
	Sfx::Play("Helicopter_loop",actor,1.0f,1.0f,&heliSoundHandle);
   if(i_activate)
		MoveAlongPath();
}
//============================================================================
void AiHeli::SetAnchor(Vector3 i_anchor, float i_radius)
{
	anchor = i_anchor;
	tether = i_radius;
}
//============================================================================
Vector3 AiHeli::SinMod(Vector3 i_pos)
{
	return i_pos;
}
//============================================================================
bool AiHeli::IsInBox()
{
	return true;
}
//============================================================================
bool AiHeli::CordCheck(CActor* targetActor)
{
	if(targetActor == NULL)
		return false;

	Vector3 toTarget = targetActor->GetFocusInWorld() - actor.GetBodyInWorld();
	toTarget.Normalize();

	//SeeThru* seeThru;
	SeeThru::LookResult lookResult;
	SeeThru::Look(actor.GetBodyInWorld(),toTarget,200.0f,&actor,lookResult,true);
	if(lookResult.actor == targetActor)
		return true;

	return false;


	CLineEntity line;
	line.Line().SetOriginEnd(actor.GetFocusInWorld(),targetActor->GetFocusInWorld());
	line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	CCollisionStackArray<CStageEntity*> ignoreList;
	ignoreList.InitArray();
	//don't include target
	if(targetActor)
	{
		CStageEntity** entry1 = ignoreList.ExpandArray();
		*entry1 = (CStageEntity*) targetActor;
	}
	//don't include me
	CStageEntity** entry2 = ignoreList.ExpandArray();
	*entry2 = (CStageEntity*) &actor;
	//don't include target's hostage
	if(targetActor->IsA(CCompActor::ClassType()))
	{
		//don't include my hostage
		Property* prop = ((CCompActor*)targetActor)->QueryProperty( "hostage" );
		if(prop)
		{
			CActor* targetshostage = CActor::FromHandle( prop->GetHandle() );
			if(targetshostage)
			{
				CStageEntity** entry3 = ignoreList.ExpandArray();
				*entry3 = (CStageEntity*) targetshostage;
			}
		}
	}
	line.SetIgnoreList(&ignoreList);
	line.SetPositiveInclude(WORLD_CATEGORY | ACTOR_CATEGORY);
	line.PositiveExclude() |= TRANSPARENT_CATEGORY;
	g_collisionSystem.ResetStack();
	CLineQuery test;

	bool blocked = test.Test(line);

	if(blocked)
		return false;
	else
		return true;
}
//============================================================================
void AiHeli::StateControl()
{
	if(aggroTimeOut > AGGRO_TIME_OUT || hitCount > HITS_TIL_FLEE)
	{
		heliState = HELI_FLEEING;
		Sfx::SetPitch(heliSoundHandle,2.0f);
	}

	switch(heliState)
	{
	case HELI_AT_START:
		break;
	case HELI_PATHING:
		if(inSight)
		{
			CommonStreamInterface *csi = g_audio.CreateStream3D("at1fsp24", false, true);
			if (csi && GetHeliTarget())
			{
				SoundStream3D *stream3D = csi->Stream3D();
				ASSERT(stream3D);
				Vector3 pos = GetHeliTarget()->GetBodyInWorld();
				stream3D->SetPosition(pos);
				//Vector3 vel = actor.GetVelocityInWorld();
				//stream3D->SetVelocity(vel);
				stream3D->SetVolume(1.0f);
				stream3D->Play();
			}
			StopOnPath();
			heliState = HELI_SPOTTED_ON_PATH;
		}
		break;
	case HELI_SPOTTED_ON_PATH:
		if(!inSight || DistanceToTargetXZ2rd() > HOVER_FROM_TARGET_MAX)
		{
			RemoveFromPath();
			heliState = HELI_MOVING_TO_SEE;
		}
		break;
	case HELI_MOVING_TO_SEE:
		if(inSight && DistanceToTargetXZ2rd() < HOVER_FROM_TARGET_MAX)
			heliState = HELI_SPOTTED_OFF_PATH;
		if(hitCount > HITS_TIL_FLEE)
			heliState = HELI_FLEEING;
		break;
	case HELI_SPOTTED_OFF_PATH:
		if(!inSight || DistanceToTargetXZ2rd() > HOVER_FROM_TARGET_MAX)
			heliState = HELI_MOVING_TO_SEE;
		if(hitCount > HITS_TIL_FLEE)
			heliState = HELI_FLEEING;
		break;
	case HELI_MOVING_TO_PATH:
		break;
	case HELI_FLEEING:
		break;
	case HELI_DEAD:
		break;
	}
}
//============================================================================
void AiHeli::StopOnPath()
{
	pathFollower->SetSpeedTarget(0.0f);
	pathFollower->SetFaceMovementDirection(false);
}
float AiHeli::DistanceToTargetXZ2rd()
{
	if (GetHeliTarget() == NULL)
		return 0.0f;

	Vector3 toTarget = GetHeliTarget()->GetFocusInWorld() - actor.GetBodyInWorld();
	float xzDist2rd = toTarget.LengthXZSquared();
	return xzDist2rd;
}
void AiHeli::RemoveFromPath()
{
	pathFollower->SetSpeedTarget(0.0f);
	pathFollower->AdjustSpeed(0.0f);
	pathFollower->SetFaceMovementDirection(false);
	pathFollower->Deactivate();
}
//============================================================================
//============================================================================
void AiHeli::RunState(float i_frameSec)
{
	if(heliState != HELI_FLEEING)
	{
		destination = pathFollower->GetPosition();
		Altitude(g_timer.GetFrameSec());
		destination.y(destination.y() + altitude);
	}
	else
	{
		timeRise += i_frameSec;
		altitude = timeRise * timeRise * FLEE_FACTOR;
		destination.y(destination.y() + altitude);
	}

	switch(heliState)
	{
	case HELI_AT_START:
		break;
	case HELI_PATHING:
		break;
	case HELI_SPOTTED_ON_PATH:
		if(inSight)
			TurnToTarget(i_frameSec);
		break;
	case HELI_MOVING_TO_SEE:
		if(inSight)
			TurnToTarget(i_frameSec);
		break;
	case HELI_SPOTTED_OFF_PATH:
		if(inSight)
			TurnToTarget(i_frameSec);
		break;
	case HELI_MOVING_TO_PATH:
		break;
	case HELI_FLEEING:
		if(inSight)
			TurnToTarget(i_frameSec);
		if(altitude > ALTITUDE_CEILING)
		{
			heliState = HELI_DEAD;
			actor.MarkForDeath();
		}
		break;
	case HELI_DEAD:
		break;
	}
}
//============================================================================
Vector3 AiHeli::DeviseVelocity()
{
	if (GetHeliTarget() == NULL)
		return(Vector3(0.0f, 0.0f, 0.0f));

	Vector3 toTarget = GetHeliTarget()->GetBodyInWorld() - actor.GetBodyInWorld();
	toTarget.y(actor.GetBodyInWorld().y());
	toTarget *= 0.5f;
	float xz2rd = toTarget.LengthXZSquared();

	Vector3 newVel;
	newVel.Clear();

	if(xz2rd < DISTANCE_TO_TARGET_MIN)
		return newVel;

	Vector3 newPos = actor.GetBodyInWorld() + toTarget;
	float nearestDist = pathFollower->GetPath()->NearestDistance(newPos);
	Vector3 toPath = pathFollower->GetPath()->PointFromDistance(nearestDist) - newPos;
	if(toPath.LengthXZSquared() > DISTANCE_OFF_PATH_MAX)
		return newVel;

	newVel = toTarget.Normalized() * HELI_SPEED;

	return newVel;
}
//============================================================================
//============================================================================
void AiHeli::MunitionImpact::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	CActor* sender = CActor::FromHandle(i_sender);
	if(sender == NULL)
		return;

	if (Cheats::Enabled(Cheats::STEALTH, sender))
		return;

	d_ref->hitCount++;
}
//============================================================================
