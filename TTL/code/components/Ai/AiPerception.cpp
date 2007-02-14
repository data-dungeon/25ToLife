#include "TTLPCH.h"
#include "components/Ai/AiBehavior.h"
#include "components/Ai/AiPerception.h"
#include "components/Ai/AiPsyche.h"
#include "Components/ComponentCreator.h"
#include "Environ/Munition.h"
#include "InterfacesTTL/ParticipantIF.h"
#include "components/Ai/AiBodyControl.h"
#include "components/Ai/AiWeaponControl.h"
#include "components/Pickup.h"
#include "environ/ConfigMgr.h"
#include "weapons/seethru.h"

#define HIGHLEVEL_DOT_QUALIFIER 0.0f
#define SIDE_OFFSET 1.0f
#define TEMP_Y_OFFSET 0.7f
#define FOOTSTEP_POLL_INTERVAL 8.5f
#define FOOTSTEP_MAX_SOUND_DISTSQ 80.0f
#define FOOTSTEP_VEL_MIN 20.0f
#define TEAMMATE_SHOT_DISTANCE_SQRD 500.0f
#define STOP_LEAN_CHECK 2.0f
#define STOP_CROUCH_CHECK 1.0f
#define STAY_NO_PEEK_TIME 0.5f
#define TIME_FRAME_CURRENT 5.0f
#define FOV_IDLE_DOT 0.4f
#define FOV_ALERTED_DOT -0.1f
//#define SIGHT_DEBUG
#define GUNSHOT_STUN_DURATION 1.0f
#define SIGNIFICANT_SOUND_DELAY 1.0f
#define FOV_LARGE_CONE_DIST2RD 150.0f
#define MAX_VIEW_DIST_SRQD 500.0f //for intros

#define DISTSQRD_VIRTUALY_THERE 1.0f
#define DISTSQRD_TINY 5.0f
#define DISTSQRD_CLOSE 30.0f
#define DISTSQRD_NEAR 150.0f
#define DISTSQRD_HELLO 250.0f
#define DISTSQRD_MEDIUM 800.0f
#define DISTSQRD_LONG 2000.0f
#define DISTSQRD_FAR 4000.0f
#define DISTSQRD_WOW 8000.0f

#define SPHERE_SIZE 0.4f
#define SPHERE_Y_OFFSET 1.3f
#define SPHERE_DIRECTION_SCALAR 0.5f
#define IN_FOCUS_TIME 2.5f
#define TRANSPARENT_OFFSET 0.02f
#define SOUND_CORDCHECK_TIME 3.0f
//#define SIGHTED_Y_OFFSET 0.4f

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(AiPerception, AiPerception,
		COMPONENT_NEUTRAL_PRIORITY);

CCollisionNode* PerceptionEntity::BeginCollisionTest(CStageEntity* i_entity, ESelect i_select)
{
	// Control the entities that can be perceived. For example, just participants
	// on another team.

	// Only actors should be detected.
	ASSERT(i_entity->IsA(CActor::ClassType()));

	// Get the entit's participant interface. Since only participants should be
	// detected, the interface should always be available.
	CParticipantIF* entityParticipant = (CParticipantIF*)
			((CActor*)i_entity)->GetInterface(CParticipantIF::GetID());
	ASSERT(entityParticipant != NULL);

	// Get my actor's participant interface. Check for team membership if it
	// is available. (Shouldn't it always be available?)
	CParticipantIF* myParticipant =
			(CParticipantIF*)Actor()->GetInterface(CParticipantIF::GetID());
	if ((myParticipant != NULL) &&
			(myParticipant->GetTeamId() == entityParticipant->GetTeamId()))
		return(NULL);

	// Let the base class have a chance. It ignores my actor as well as any
	// other actors that I have been set to ignore.
	return(CSightEntity::BeginCollisionTest(i_entity, i_select));
}
//============================================================================
AiPerception::AiPerception( CCompActor &i_actor ) :
CActorComponent( i_actor )
{
}
//============================================================================
AiPerception::~AiPerception()
{
}
//============================================================================
void AiPerception::Initialize()
{
	d_munitionFiredHandler.Initialize(MUNITION_FIRED,
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);

	d_munitionImpactHandler.Initialize(MUNITION_IMPACT_ACTOR, MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);

	d_teammateShotHandler.Initialize("TeammateShot",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);

	d_teammateAlertedHandler.Initialize("TeammateAlerted",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);

	d_avatarSpottedHandler.Initialize("AvatarSpotted",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);

	d_doorKickedHandler.Initialize("DoorKicked",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);

	d_lobbedExplosionWarning.Initialize("LobbedExplosionWarning",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);

	d_actorDiedHandler.Initialize( "ActorDied", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this );
	d_shootAtInstanceStart.Initialize( "ShootInstStart", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this );
	d_shootAtInstanceStop.Initialize( "ShootInstStop", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this );
	d_tauntedHandler.Initialize( "Taunted", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this );

	visualRadiusSqrd = actor.Parm().GetFloat("visual_radius");
	hearingRadiusSqrd = actor.Parm().GetFloat("hearing_radius");

	if(visualRadiusSqrd < 1.0f)
		visualRadiusSqrd = 50.0f;
	else if(visualRadiusSqrd > 1000.0f)
		visualRadiusSqrd = 1000.0f;
	visualRadiusSqrd *= visualRadiusSqrd;

	if(hearingRadiusSqrd < 1.0f)
		hearingRadiusSqrd = 50.0f;
	else if(hearingRadiusSqrd > 1000.0f)
		hearingRadiusSqrd = 1000.0f;
	hearingRadiusSqrd *= hearingRadiusSqrd;

	// Setup the perception entity.
	perceptionEntity.SetRange(Math::Sqrt(visualRadiusSqrd));
	perceptionEntity.SetFov(Math::ArcCos(HIGHLEVEL_DOT_QUALIFIER));
	Vector3 focusOffset = actor.GetFocusInWorld() - actor.GetBodyInWorld();
	perceptionEntity.SetActor(&actor, focusOffset, Vector3(0.0f, 0.0f, 1.0f));

	// Only detect participants.
	perceptionEntity.SetPositiveInclude(PARTICIPANT_CATEGORY);
	aiBehavior = NULL;

	transparentMaterials[0] = g_surfaceType.GetIndex("chainlink");
	transparentMaterials[1] = g_surfaceType.GetIndex("glass");
	transparentMaterials[TRANSPARENT_SURFACES - 1] = g_surfaceType.GetIndex("metalgate");

	Reset();
}
//============================================================================
void AiPerception::Reset()
{
	inSight = false;
	percentVisible = 0.0f;
   //lastPos.Clear();
	lastPosTime = 0.0f;
	lastKnownTarget = INVALID_OBJECT_HANDLE;
	lastSeenTime = 0.0f;
	lastHeardTime = 0.0f;
	lastCentroid.Clear();
	footstepTimer = FOOTSTEP_POLL_INTERVAL;
	//hostileToAvatar = false;
	currentPeekType = PEEK_NONE;
	desiredPeekType = PEEK_NONE;
	peekFrameDelay = 0;
	peekTimer = 0.0f;

	int i;
	for(i = 0; i < SOUND_INCIDENT_TABLE_SIZE; i++)
	{
		SoundIncidentTable[i].type = EMPTY_ROW;
		SoundIncidentTable[i].intensity = 0.0f;
		SoundIncidentTable[i].gameTime = 0.0f;
		SoundIncidentTable[i].location.Clear();
		SoundIncidentTable[i].participantId = 0;
		SoundIncidentTable[i].teamId = 0;
	}
	soundIncidentCursor = 0;

	for(int p = i; i < MAX_STORED_LAST_POSITIONS; i++)
	{
		lastPosGroup[i].Clear();
	}
	lastPosCounter = 0;
	lastPosGroupFilledOnce = false;

	CParticipantIF* myParticipant =
			(CParticipantIF*)actor.GetInterface(CParticipantIF::GetID());
	if (myParticipant != NULL)
		myId = myParticipant->GetParticipantId();
	else
		myId = INVALID_PARTICIPANT;
	attemptedTargetId = INVALID_PARTICIPANT;

	peekCounter = -1;
	peekPercentVisible = 0.0f;
	sightState = SIGHTSTATE_NOT_IN_SIGHT;
	fov = FOV_IDLE;

	if(actor.Parm().GetInt("born_knowing") == 1 && g_referee.GetTeamingModel().IsPlayerEnemy(myId) && g_players.GetPlayer(0))
		SeedAvatarPosition();

	inFocusTime = 0.0f;
	setEchoLastSeen = true;
	soundPosTimer = 0.0f;
}
//============================================================================
void AiPerception::SeedAvatarPosition()
{
	AddSoundIncident(ALERT_SCREAM, g_players.GetPlayer(0)->GetFocusInWorld());
	AiPsyche* aiPsyche = (AiPsyche*) actor.GetComponentByName( "AiPsyche" );
	if(aiPsyche)
		aiPsyche->StimulusInput(AiPsyche::STIMULUS_ENEMY_PRESENCE, 1.0f);
	Sighted(g_players.GetPlayer(0),g_players.GetPlayer(0)->GetFocusInWorld(),true);
}
//============================================================================
ActorHandle AiPerception::DetermineTarget()
{
	if(inSight)
	{
		if(lastKnownTarget == CActor::ToHandle(g_players.GetPlayer(0))) //avatar is highest priority
			return lastKnownTarget;

		//if you are a avatar enemy see if avatar is closer than lastknown
		if(g_referee.GetTeamingModel().IsPlayerEnemy(myId) && g_players.GetPlayer(0))
		{
			CActor *target = 	g_referee.GetActor(attemptedTargetId);
			if(target)
			{
				Vector3 toTarget = target->GetBodyInWorld() - actor.GetBodyInWorld();
				Vector3 toAvatar = g_players.GetPlayer(0)->GetBodyInWorld() - actor.GetBodyInWorld();
				if(toTarget.LengthSquared() < toAvatar.LengthSquared())
					return lastKnownTarget;
				else
					return g_players.GetPlayer(0)->GetHandle();
			}
		}
		return lastKnownTarget;
	}

	//for ambients, neutrals, and pissed off neutrals, the target is always the avatar
	if(g_referee.GetTeamingModel().IsAmbient(myId) )//g_referee.GetTeamingModel().IsPlayerNeutral(myId))
		return (CActor::ToHandle(g_players.GetPlayer(0)));

	//while sight it is rotating peek spots, stay on last attemptedTargetId
	if(desiredPeekType == PEEK_NONE && sightState == SIGHTSTATE_NOT_IN_SIGHT)
		attemptedTargetId = g_referee.GetTeamingModel().GetNextEnemy(myId, attemptedTargetId, false);

	//int myteam = g_referee.GetTeam(myId);
	//int histeam = g_referee.GetTeam(attemptedTargetId);
	//dbgPrint("^^^^^^^myId, attemptedTargetId, myteam, histeam %i %i %i %i\n", myId, attemptedTargetId, myteam, histeam);

	if(myId == attemptedTargetId || myId == INVALID_PARTICIPANT || attemptedTargetId == INVALID_PARTICIPANT)
		return INVALID_OBJECT_HANDLE;

	CActor *target = 	g_referee.GetActor(attemptedTargetId);
	if(target)
      return target->GetHandle();
	else
		return INVALID_OBJECT_HANDLE;
}
//============================================================================
void AiPerception::SetupPerceptionEntity(int level)
{
	switch(level)
	{
	case CHECK_FOV_AND_RANGE:
		perceptionEntity.Obscure().SetPositiveInclude(NO_CATEGORY);
		perceptionEntity.SetConfiguration(CSightEntity::TEST_ONLY);
		break;
	case CHECK_FOR_PARTIAL_VISABILITY_NO_PROCESSING:
		perceptionEntity.Obscure().SetPositiveInclude(WORLD_CATEGORY | ACTOR_CATEGORY);
		perceptionEntity.Obscure().PositiveExclude() |= TRANSPARENT_CATEGORY;
		perceptionEntity.SetConfiguration(CSightEntity::TEST_ONLY);
		break;
	case CHECK_FOR_PARTIAL_VISABILITY:
	default:
		perceptionEntity.Obscure().SetPositiveInclude(WORLD_CATEGORY | ACTOR_CATEGORY);
		perceptionEntity.Obscure().PositiveExclude() |= TRANSPARENT_CATEGORY;
		perceptionEntity.SetConfiguration(CSightEntity::INTERSECT_ALL | CSightEntity::DESCRIBE_VISIBLE);
		break;
	}
	perceptionEntity.SyncWithActor();
}

void AiPerception::BodyControlPeek(int newPeek)
{
	if(currentPeekType == newPeek)
		return;
	desiredPeekType = newPeek;

	AiBodyControl *aiBodyControl = (AiBodyControl *)actor.GetComponentByName( "AiBodyControl" );
	if(aiBodyControl)
	{
		desiredPeekType = aiBodyControl->PeekToSee(desiredPeekType, currentPeekType);
		currentPeekType = desiredPeekType;
		peekTimer = 0.0f;
	}
}
//============================================================================
bool AiPerception::GetLastDistanceSquared(float &distance)
{
	if(!GetAtLeastOneStimulis())
		return false;

	//Vector3 toTarget = lastPos - actor.GetFocusInWorld();
	Vector3 toTarget = GetLaggedLastPos() - actor.GetFocusInWorld();
	distance = toTarget.LengthSquared();
	return true;
}
//============================================================================
void AiPerception::Update(float deltaSec)
{
#define ECHOLASTSEEN_DISTXZSQRD 20.0f
	if(setEchoLastSeen && !inSight && lastKnownTarget != INVALID_OBJECT_HANDLE && GetRecentSighting(20.0f))
	{
		CActor* targetActor = CActor::FromHandle(lastKnownTarget);
		ASSERT(targetActor);
		Vector3 toTarget = targetActor->GetBodyInWorld() - lastCentroid;
		if(toTarget.LengthXZSquared() > ECHOLASTSEEN_DISTXZSQRD)
		{
			//place a sound
			AddSoundIncident(ALERT_SCREAM,targetActor->GetFocusInWorld());
			setEchoLastSeen = false;
		}
	}

	if(soundPosTimer != -1.0f)
		Math::MoveToZero(&soundPosTimer,deltaSec);

	ListenForFootsteps(deltaSec);
}
//============================================================================
void AiPerception::ListenForFootsteps(float deltaSec)
{
	if(inSight || GetRecentSighting(TIME_FRAME_CURRENT))
		return;

	//only avatar enemies hear footsteps
	if(!g_referee.GetTeamingModel().IsPlayerEnemy(myId))
		return;

	Math::MoveToZero(&footstepTimer,deltaSec);
	if(footstepTimer > 0.0f)
		return;
	footstepTimer = FOOTSTEP_POLL_INTERVAL;

	CActor* avatar = g_players.GetPlayer(0);
	if(avatar == NULL)
		return;

	Vector3 toTarget = avatar->GetFocusInWorld() - actor.GetFocusInWorld();
	float distSq = toTarget.LengthSquared();
	if(distSq > FOOTSTEP_MAX_SOUND_DISTSQ)
		return;

	float targetVelSq = avatar->GetVelocityInWorld().LengthSquared();
	if(targetVelSq < FOOTSTEP_VEL_MIN)
		return;

	AddSoundIncident(FOOTSTEP_SOUND, avatar->GetFocusInWorld());

	AiPsyche* aiPsyche = (AiPsyche*) actor.GetComponentByName( "AiPsyche" );
	if(aiPsyche)
		aiPsyche->StimulusInput(AiPsyche::STIMULUS_ENEMY_PRESENCE, 1.0f);
}
//============================================================================
bool AiPerception::GetLastSoundLocation(Vector3 &location, int type)
{
	if(lastHeardTime == 0.0f)
		return false; //no sounds in table

	if(type == -1) //any type
	{
		location = SoundIncidentTable[soundIncidentCursor].location;
		return true;
	}

	int cur = soundIncidentCursor;
	// add particular type
	for(int i = 0; i < SOUND_INCIDENT_TABLE_SIZE; i++)
	{
		if(SoundIncidentTable[cur].type == type)
		{
			location = SoundIncidentTable[soundIncidentCursor].location;
			return true;
			break;
		}
		else
		{
			cur--;
			if(cur < 0)
				cur = SOUND_INCIDENT_TABLE_SIZE - 1;
		}
	}
	return false;
}
//============================================================================
bool AiPerception::GetSoundTypeHeard(int i_soundType)
{
	if(lastHeardTime == 0.0f)
		return false; //no sounds in table

	int cur = soundIncidentCursor;
	// add particular type
	for(int i = 0; i < SOUND_INCIDENT_TABLE_SIZE; i++)
	{
		if(SoundIncidentTable[cur].type == i_soundType)
		{
			return true;
			break;
		}
		else
		{
			cur--;
			if(cur < 0)
				cur = SOUND_INCIDENT_TABLE_SIZE - 1;
		}
	}
	return false;
}
//============================================================================
bool AiPerception::GetSoundHeard()
{
	if(lastHeardTime > 0.0f)
		return false;
	return true;
}
//============================================================================
bool AiPerception::GetSoundLaterThanSight()
{
	if(inSight)
		return false;

	if(lastHeardTime > lastSeenTime)
		return true;

	return false;
}
//============================================================================
float AiPerception::GetLastSeenOrHeardTime()
{
	if(lastSeenTime > lastHeardTime)
		return lastSeenTime;
	else
		return lastHeardTime;
}
//============================================================================
bool AiPerception::GetLastKnownLocation(Vector3 &location)
{
	if(!GetAtLeastOneStimulis())
		return false;

	if(GetSoundLaterThanSight())
	{
		//if no sight yet, for sure send sound location
		if(GetLastSeenTime() == 0.0f)
		{
			location = SoundIncidentTable[soundIncidentCursor].location;
			//DrawUtility::Sphere(location,DrawUtility::CYAN,0.15f);  //Lastknowntest
			return true;
		}

		//if it is a current thing, look where the sound is, otherwise turn back to where you last saw player
		if((g_timer.GetStartSec() - GetLastHeardTime() < TIME_FRAME_CURRENT && GetLastHeardTime() - GetLastSeenTime() > SIGNIFICANT_SOUND_DELAY))
		{
			location = SoundIncidentTable[soundIncidentCursor].location;
			//DrawUtility::Sphere(location,DrawUtility::RED,0.15f);  //Lastknowntest
			soundPosTimer = 0.0f;
			return true;
		}

		//if I can see where the sound is, look there
		if(soundPosTimer == 0.0f)
		{
			if(CordCheck(actor.GetFocusInWorld(),SoundIncidentTable[soundIncidentCursor].location))
			{
				location = SoundIncidentTable[soundIncidentCursor].location;
				//DrawUtility::Sphere(location,DrawUtility::BLUE,0.15f);  //Lastknowntest
				soundPosTimer = -1.0f; //success, disabled until set to zero
				return true;
			}
			else
				soundPosTimer = SOUND_CORDCHECK_TIME;
		}
		else if (soundPosTimer == -1.0f)
		{
			location = SoundIncidentTable[soundIncidentCursor].location;
			//DrawUtility::Sphere(location,DrawUtility::BLUE,0.15f);  //Lastknowntest
			return true;
		}
	}

	location = GetLaggedLastPos();
	//DrawUtility::Sphere(location,DrawUtility::YELLOW,0.15f);  //Lastknowntest
	return true;
}
//============================================================================
bool AiPerception::GetAtLeastOneStimulis()
{
	if(lastSeenTime != 0.0f || lastHeardTime != 0.0f)
		return true;
	return false;
}
//============================================================================
void AiPerception::SetCurrentPos(Vector3 curPos)
{
	lastPosGroup[lastPosCounter] = curPos;
	lastPosCounter++;

   if(lastPosCounter >= MAX_STORED_LAST_POSITIONS)
	{
		lastPosGroupFilledOnce = true;
		lastPosCounter = 0;
	}
}
//============================================================================
Vector3 AiPerception::GetLaggedLastPos()
{
	//default use current
	Vector3 latestLastPos;
	if(lastPosCounter - 1 < 0)
	{
		if(lastPosGroupFilledOnce)
			latestLastPos = lastPosGroup[MAX_STORED_LAST_POSITIONS - 1];
		else
      	latestLastPos = lastPosGroup[0];
	}
	else
		latestLastPos = lastPosGroup[lastPosCounter - 1];

	//use lagged if insight and haven't been infocus for a long time
	if(inSight && GetTimeInSight() < IN_FOCUS_TIME)
	{
		//Vector3 toTarget = latestLastPos - actor.GetFocusInWorld();
		
		//if(toTarget.LengthXZSquared() > DISTSQRD_TINY)
		//{
			//use oldest in list
			if(lastPosGroupFilledOnce)
				return lastPosGroup[lastPosCounter];
			else
				return lastPosGroup[0]; //oldest if it hasn't been filled once
		//}
	}

	return latestLastPos;

	/*
	if(inSight && lastPosGroupFilledOnce)
		return lastPosGroup[lastPosCounter];
	else
	{
		int slot = lastPosCounter - 1;		
		if(slot < 0)
		{
			if(lastPosGroupFilledOnce)
				slot = MAX_STORED_LAST_POSITIONS - 1;
			else
				slot = 0;
		}
		return lastPosGroup[slot];
	}
	*/
}
//============================================================================
Vector3 AiPerception::GetLastCentroid()
{
	return GetLaggedLastPos();  //centralizing position so modified Y is ok

	/*
	//we need to introduce inaccuracy
	Vector3 centroid = GetLaggedLastPos();
	centroid.y(lastCentroid.y());
	return centroid;
	*/
}
//============================================================================
bool AiPerception::BlindMansStick(Vector3 destination)
{
	//create a sphere position that is toward the destination
	Vector3 toDestination = destination - actor.GetBodyInWorld();
	toDestination.y(0.0f);
	toDestination.SafeNormalize();
	toDestination *= SPHERE_DIRECTION_SCALAR;  //shrink a bit so it is closer to body
	toDestination.y(SPHERE_Y_OFFSET);
	Vector3 spherePosition = actor.GetBodyInWorld() + toDestination;

	CSphereEntity sphere;
	sphere.Sphere().Set(spherePosition, SPHERE_SIZE);
	sphere.SetIgnoreEntity(&actor);
	sphere.SetPositiveInclude(ACTOR_CATEGORY);
	//sphere.Sphere().SetCenterTest(true);  //this is a performance enhancing thing (distance check only) odd sized boxes are b.a.d. bad
	g_collisionSystem.ResetStack();
	CCollisionList *list = g_collisionSystem.Test(&sphere);
	bool bCollision = false;
	if(list)
		bCollision = true;

	//DrawUtility::Sphere(spherePosition,DrawUtility::GREEN,SPHERE_SIZE);

	//let's find out what it was, this code shouldn't really go here....maybe move it later
	if(bCollision)
	{
		if(list->GetHeadEvent()->GetTargetEntity() == NULL || !list->GetHeadEvent()->GetTargetEntity()->IsA(CActor::ClassType()))
			return bCollision;

		CActor* actorInWay = (CActor*)list->GetHeadEvent()->GetTargetEntity();

		if(actorInWay == CActor::FromHandle(lastKnownTarget))
			return false;

		//if(actorInWay->IsA(CCompActor::ClassType()))
		//{
			//is it an enemy?
			//CParticipantIF* participant = (CParticipantIF*)((CCompActor*)actorInWay)->GetInterface(CParticipantIF::GetID());
			//if (participant != NULL)
			//{
				if(g_referee.GetTeamingModel().AreEnemies(myId,CParticipantIF::GetParticipantIdFromActor(actorInWay)))
					Sighted(actorInWay,actorInWay->GetFocusInWorld());
			//}
		//}

		//don't collide with pickups
		if(actorInWay->Parm().GetHasParameter("pickupType"))
			return false;

		if(aiBehavior == NULL)  //grab this component permanently, but wait til you need it first time
			aiBehavior = (AiBehavior*)( actor.GetComponentByName( "AiBehavior" ) );

		if(aiBehavior)
			aiBehavior->ActorInMyWay((CActor*)actorInWay);
	}

	return bCollision;
}
//============================================================================
void AiPerception::CollisionAccept( CStageEntity* i_entity, CCollisionEvent* i_event )
{
	if(!i_entity->IsA(CActor::ClassType()))
		return;  //make sure it is an actor

	CActor *collider = (CActor*) i_entity;
	CParticipantIF* participant = (CParticipantIF*)collider->GetInterface(CParticipantIF::GetID());
	if (participant == NULL)
		return;

	uint8 testId = CParticipantIF::GetParticipantIdFromActor(&actor);

	if(testId != myId)
		int messedup = 1;

	uint8 yourId = CParticipantIF::GetParticipantIdFromActor(collider);

	if(g_referee.GetTeamingModel().AreEnemies(myId,yourId))
	{
		lastKnownTarget = ((CActor*)i_entity)->GetHandle();
		if(g_players.GetPlayer(0) == collider)
		{
			AddSoundIncident(AVATAR_COLLISION, collider->GetFocusInWorld(), 1.0f, g_timer.GetStartSec(), participant->GetParticipantId(), participant->GetTeamId());
			//if a pathfollower, stop and cower
			{
				if(aiBehavior == NULL)  //grab this component permanently, but wait til you need it first time
					aiBehavior = (AiBehavior*)( actor.GetComponentByName( "AiBehavior" ) );
				if(aiBehavior)
					aiBehavior->PathfollowerStop();
			}
		}
		AiPsyche* aiPsyche = (AiPsyche*) actor.GetComponentByName( "AiPsyche" );
		if(aiPsyche)
			aiPsyche->StimulusInput(AiPsyche::STIMULUS_ENEMY_PRESENCE, 1.0f);
	}
	else
	{
		AiNavigation* aiNavigation = (AiNavigation*) actor.GetComponentByName( "AiNavigation" );
		if(aiNavigation)
			aiNavigation->SetPushed(collider->GetBodyInWorld());
	}
}
//============================================================================
void AiPerception::MunitionFired::HandleMessage(void* i_data, ActorHandle i_sender,
																bool i_posted)
{
	CActor* sender = CActor::FromHandle(i_sender);
	if (Cheats::Enabled(Cheats::STEALTH, sender))
		return;

	if (!sender)
		return;

	//if silenced weapon
	//CMunitionFired *fired = (CMunitionFired*) i_data;
	//TTLWeaponProperties *prop = (TTLWeaponProperties *)g_weaponLib.GetProperties( fired->d_weaponID );
	//int weaponClass = WEAPONCLASS_NONE;
	//if( prop )
	//	weaponClass = prop->m_weaponClass;

	CParticipantIF* myParticipant = (CParticipantIF*)d_ref->actor.GetInterface(CParticipantIF::GetID());
	if (myParticipant == NULL)
		return;

	CParticipantIF* shooterParticipant = (CParticipantIF*)sender->GetInterface(CParticipantIF::GetID());
	if (shooterParticipant == NULL)
		return;

	//if I am on the same team, ignore your shots
	if(g_referee.GetTeamingModel().AreFriends(myParticipant->GetParticipantId(),shooterParticipant->GetParticipantId()))
		return;

	ASSERT(sender!= &d_ref->actor);

	Vector3 toSender = sender->GetFocusInWorld() - d_ref->actor.GetFocusInWorld();
	float intensity = d_ref->hearingRadiusSqrd - toSender.LengthSquared();
	if(intensity < 0.0f)
		return;

	g_messageDispatcher.SendMessageToActor("EnemyGunfire", NULL, d_ref->actor.GetHandle(), d_ref->actor.GetHandle());

	d_ref->AddSoundIncident(WEAPON_FIRED, sender->GetFocusInWorld(), intensity, g_timer.GetStartSec(), shooterParticipant->GetParticipantId(), shooterParticipant->GetTeamId());

	AiPsyche* aiPsyche = (AiPsyche*) d_ref->actor.GetComponentByName( "AiPsyche" );
	if(aiPsyche)
		aiPsyche->StimulusInput(AiPsyche::STIMULUS_ENEMY_PRESENCE, 1.0f);
}
//============================================================================
void AiPerception::MunitionImpact::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	if(d_ref->inSight)
		return;

	CActor* sender = CActor::FromHandle(i_sender);
	if(sender == NULL)
		return;

	if (Cheats::Enabled(Cheats::STEALTH, sender))
		return;

	AiPsyche* aiPsyche = (AiPsyche*) d_ref->actor.GetComponentByName( "AiPsyche" );
	if(aiPsyche)
		aiPsyche->StimulusInput(AiPsyche::STIMULUS_PAIN, 1.0f);

	AiWeaponControl* aiWeaponControl = (AiWeaponControl*) d_ref->actor.GetComponentByName( "AiWeaponControl" );
	if(aiWeaponControl)
		aiWeaponControl->CeaseFire(GUNSHOT_STUN_DURATION);

	if(sender == &d_ref->actor)
		return;

	CParticipantIF* myParticipant = (CParticipantIF*)d_ref->actor.GetInterface(CParticipantIF::GetID());
	if (myParticipant == NULL)
		return;

	CParticipantIF* shooterParticipant = (CParticipantIF*)sender->GetInterface(CParticipantIF::GetID());
	if (shooterParticipant == NULL)
		return;

	//if friends and not already hostile, see if friendly fire is on, if so...go hostile...otherwise exit
	//if(g_referee.GetTeamingModel().AreFriends(myParticipant->GetParticipantId(), shooterParticipant->GetParticipantId())
	//	&& !d_ref->hostileToAvatar)
	//{
	//	if(g_configmgr.TestOptionValue("friendlyFire", "1"))
	//		d_ref->hostileToAvatar = true;
	//	else
	//		return;
	//}

	g_messageDispatcher.SendMessageToAll("TeammateShot",(void*)myParticipant->GetTeamId(),d_ref->actor.GetHandle());

	if(g_referee.GetTeamingModel().AreEnemies(myParticipant->GetParticipantId(), shooterParticipant->GetParticipantId()))
	{
		d_ref->Sighted(sender,sender->GetFocusInWorld());
		if(!d_ref->inSight)
			d_ref->AddSoundIncident(WEAPON_IMPACT, sender->GetFocusInWorld(), 1.0f, g_timer.GetStartSec(), shooterParticipant->GetParticipantId(), shooterParticipant->GetTeamId());
	}
}
//============================================================================
void AiPerception::TeammateShot::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	if(d_ref->inSight)
		return;

	CActor* sender = CActor::FromHandle(i_sender);
	if(sender == &d_ref->actor || sender == NULL)
		return;

	CParticipantIF* myParticipant = (CParticipantIF*)d_ref->actor.GetInterface(CParticipantIF::GetID());
	if (myParticipant == NULL)
		return;

	//check to see if you are on my team
	if(myParticipant->GetTeamId() != (int) i_data)
		return;

	//check range
	Vector3 toSender = sender->GetFocusInWorld() - d_ref->actor.GetFocusInWorld();
	if(toSender.LengthSquared() > TEAMMATE_SHOT_DISTANCE_SQRD)
		return;

	CParticipantIF* senderParticipant = (CParticipantIF*)sender->GetInterface(CParticipantIF::GetID());
	if (senderParticipant == NULL)
		return;

	//d_ref->AddSoundIncident(WEAPON_IMPACT, sender->GetFocusInWorld());  //getting shot will give them a sight and they will send a teammate alerted which gives position of shooter

	AiPsyche* aiPsyche = (AiPsyche*) d_ref->actor.GetComponentByName( "AiPsyche" );
	if(aiPsyche)
		aiPsyche->StimulusInput(AiPsyche::STIMULUS_PAIN, 1.0f);
}
//============================================================================
void AiPerception::TeammateAlerted::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	if(d_ref->inSight)
		return;

	//this is me
	CActor* sender = CActor::FromHandle(i_sender);
	if(sender == &d_ref->actor || sender == NULL)
		return;

	CParticipantIF* myParticipant = (CParticipantIF*)d_ref->actor.GetInterface(CParticipantIF::GetID());
	if (myParticipant == NULL)
		return;

	CParticipantIF* senderParticpant = (CParticipantIF*)sender->GetInterface(CParticipantIF::GetID());
	if (senderParticpant == NULL)
		return;

	//check to see if you are on my team
	if(myParticipant->GetTeamId() != senderParticpant->GetTeamId())
		return;

	//check range
	Vector3 toSender = sender->GetFocusInWorld() - d_ref->actor.GetFocusInWorld();
	if(toSender.LengthSquared() > TEAMMATE_SHOT_DISTANCE_SQRD)
		return;

	CActor* target = (CActor*) i_data;
	if(!target->IsA(CActor::ClassType()))
		return;  //make sure it is an actor

	d_ref->AddSoundIncident(ALERT_SCREAM, target->GetFocusInWorld());

	AiPsyche* aiPsyche = (AiPsyche*) d_ref->actor.GetComponentByName( "AiPsyche" );
	if(aiPsyche)
		aiPsyche->StimulusInput(AiPsyche::STIMULUS_ENEMY_PRESENCE, 1.0f);
}
//============================================================================
void AiPerception::DoorKicked::HandleMessage(void* i_data, ActorHandle i_sender,
																bool i_posted)
{
	if(d_ref->inSight)
		return;

	CActor* sender = CActor::FromHandle(i_sender);
	if(sender == NULL)
		return;

	if (Cheats::Enabled(Cheats::STEALTH))
		return;

	//only avatar enemies
	if(!g_referee.GetTeamingModel().IsPlayerEnemy(d_ref->myId))
		return;

	CActor *playerActor = CActor::FromHandle((uint32)i_data);

	if(!playerActor->IsA(CActor::ClassType()))
		return;  //make sure it is an actor

	CParticipantIF* kickerParticipant = (CParticipantIF*)playerActor->GetInterface(CParticipantIF::GetID());
	if (kickerParticipant == NULL)
		return;

	if(g_referee.GetTeamingModel().AreTeammates(d_ref->myId, kickerParticipant->GetParticipantId()))
		return;

	Vector3 toSender = sender->GetFocusInWorld() - d_ref->actor.GetFocusInWorld();

	float intensity = d_ref->hearingRadiusSqrd - toSender.LengthSquared();
	if(intensity < 0.0f)
		return;

	d_ref->AddSoundIncident(DOOR_KICKED, sender->GetFocusInWorld(), intensity, g_timer.GetStartSec(), kickerParticipant->GetParticipantId(), kickerParticipant->GetTeamId());

	AiPsyche* aiPsyche = (AiPsyche*) d_ref->actor.GetComponentByName( "AiPsyche" );
	if(aiPsyche)
		aiPsyche->StimulusInput(AiPsyche::STIMULUS_ENEMY_PRESENCE, 1.0f);
}
//============================================================================
void AiPerception::LobbedExplosionWarning::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);

	CActor* sender = CActor::FromHandle(i_sender);
	if(sender == NULL)
		return;

	Vector3 toMe = d_ref->actor.GetFocusInWorld() - sender->GetFocusInWorld();
	if(toMe.LengthSquared() > DISTSQRD_CLOSE)
		return;

	//d_ref->AddSoundIncident(EXPLOSION, sender->GetFocusInWorld());

	AiPsyche* aiPsyche = (AiPsyche*) d_ref->actor.GetComponentByName( "AiPsyche" );
	if(aiPsyche)
		aiPsyche->StimulusInput(AiPsyche::STIMULUS_ENEMY_PRESENCE, 1.0f);

	AiNavigation* aiNavigation = (AiNavigation*) d_ref->actor.GetComponentByName( "AiNavigation" );
	if(aiNavigation)
		aiNavigation->SetRunFromNade(sender->GetFocusInWorld());
}
//============================================================================
bool AiPerception::CordCheck(Vector3 fromPos, Vector3 toPos, CActor* targetActor, int type)
{
   CLineEntity line;
#define CAPSULERAYONLY
#ifdef CAPSULERAYONLY
	if(fromPos == toPos)
		return false;
	Vector3 dir =  toPos - fromPos;
	float range = dir.Length();
	dir /= range;
	line.Line().SetOriginBasis(fromPos, dir);
	line.Line().SetMaxBoundary(range);
#else // this doesn't
	line.Line().SetOriginEnd(fromPos,toPos);
#endif
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
	//don't include my hostage
	Property* prop = actor.QueryProperty( "hostage" );
	if(prop)
	{
		CActor* hostage = CActor::FromHandle( prop->GetHandle() );
		if(hostage)
		{
			CStageEntity** entry3 = ignoreList.ExpandArray();
			*entry3 = (CStageEntity*) hostage;
		}
	}
	//don't include target's hostage
	if(targetActor && targetActor->IsA(CCompActor::ClassType()))
	{
		prop = ((CCompActor*)targetActor)->QueryProperty( "hostage" );
		if(prop)
		{
			CActor* targetshostage = CActor::FromHandle( prop->GetHandle() );
			if(targetshostage)
			{
				CStageEntity** entry4 = ignoreList.ExpandArray();
				*entry4 = (CStageEntity*) targetshostage;
			}
		}
	}

	line.SetIgnoreList(&ignoreList);

	switch(type)
	{
	case CORD_WORLD_AND_ACTORS:
		line.SetPositiveInclude(WORLD_CATEGORY | ACTOR_CATEGORY);
		line.PositiveExclude() |= TRANSPARENT_CATEGORY;
		break;
	case CORD_WORLD:
		line.SetPositiveInclude(WORLD_CATEGORY );
		break;
	case CORD_ACTOR:
		line.SetPositiveInclude(ACTOR_CATEGORY);
		line.PositiveExclude() |= TRANSPARENT_CATEGORY;
		break;
	}

	g_collisionSystem.ResetStack();
	CLineQuery test;

	bool blocked = test.Test(line);

//#define AIPERCEPTIONDEBUG
#ifdef AIPERCEPTIONDEBUG
	if (!blocked)
		DrawUtility::Line(fromPos, toPos, DrawUtility::WHITE);
	else
	{
		DrawUtility::COLOR color = test.actor ? DrawUtility::RED : DrawUtility::GREEN;
		DrawUtility::Line(fromPos, test.intersection, color);
		DrawUtility::Sphere(test.intersection, color);
	}
#endif

	if(blocked)
	{
		u8 material = test.material;//test.event->GetTargetEntity()->Material(test.event->GetHeadPair());
		for(int i = 0; i < TRANSPARENT_SURFACES; i++)
		{
			if(SeeThru::Transparent(material))//transparentMaterials[i] == material)
			{
				//re-cast
				Vector3 toToPos = toPos - fromPos;
				toToPos.SafeNormalize();
				toToPos *= TRANSPARENT_OFFSET;
				fromPos = test.intersection + toToPos;
				line.Line().SetOriginEnd(fromPos,toPos);
				g_collisionSystem.ResetStack();
				CLineQuery test;
				if(!test.Test(line))
				{
					blocked = false;
					//DrawUtility::Line(toPos, fromPos, DrawUtility::CYAN);
					//DrawUtility::Sphere(test.intersection, DrawUtility::GREEN);
					//DrawUtility::Sphere(fromPos, DrawUtility::RED);
				}
			}
		}
	}

#ifdef SIGHT_DEBUG
	if(blocked)
		DrawUtility::Line(fromPos,toPos,DrawUtility::RED);
	else if(fov == FOV_IDLE)
		DrawUtility::Line(fromPos,toPos,DrawUtility::BLUE);
	else if(fov == FOV_ALERT)
		DrawUtility::Line(fromPos,toPos,DrawUtility::CYAN);
	else //long range
		DrawUtility::Line(fromPos,toPos,DrawUtility::MAGENTA);
#endif

	if(blocked)
		return false;
	else
		return true;
}
//============================================================================
ActorHandle AiPerception::VisionCheck()
{
	//determine peekType
	if(sightState == SIGHTSTATE_NOT_IN_SIGHT)
	{
		desiredPeekType++;
		if(desiredPeekType > PEEK_CROUCH)
			desiredPeekType = PEEK_NONE;
	}

	CActor *target = CActor::FromHandle(DetermineTarget());

	if(target == NULL)
	{
		inSight = false;
		sightState = SIGHTSTATE_NOT_IN_SIGHT;
		return INVALID_OBJECT_HANDLE;
	}

	//check max range
	Vector3 toTarget = target->GetBodyInWorld() - actor.GetBodyInWorld();
	float distToTargetSqrd = toTarget.LengthSquared();
	if(distToTargetSqrd > visualRadiusSqrd)
	{
		inSight = false;
		sightState = SIGHTSTATE_NOT_IN_SIGHT;
		return INVALID_OBJECT_HANDLE;
	}

	if(fov != FOV_IDLE)
	{
		if(distToTargetSqrd > FOV_LARGE_CONE_DIST2RD)
			fov = FOV_LONGRANGE;
		else
			fov = FOV_ALERT;
	}

	//check fov
	toTarget.SafeNormalize();
	float fovDot = toTarget.Dot(actor.GetBodyToWorld().GetRow2());
	if((fov == FOV_ALERT && fovDot < FOV_ALERTED_DOT)||
		(fov == FOV_IDLE && fovDot < FOV_IDLE_DOT)||
		(fov == FOV_LONGRANGE && fovDot < FOV_IDLE_DOT))
	{
		inSight = false;
		sightState = SIGHTSTATE_NOT_IN_SIGHT;
		return INVALID_OBJECT_HANDLE;
	}

	//set up to and from positions
	Vector3 toPos = GetToPos(target);
	Vector3 fromPos = GetFromPos();

	switch(sightState)
	{
	case SIGHTSTATE_NOT_IN_SIGHT:
		if(CordCheck(fromPos,toPos,target,CORD_WORLD))
			sightState = SIGHTSTATE_NO_TERRAIN_OBSCURING;
		break;
	case SIGHTSTATE_NO_TERRAIN_OBSCURING:
		if(CordCheck(fromPos,toPos,target,CORD_WORLD_AND_ACTORS))
		{
			if(desiredPeekType == PEEK_NONE)
				sightState = SIGHTSTATE_IN_SIGHT_NORMAL;
			else
				sightState = SIGHTSTATE_PEEK_FIRST_POSITIVE;
		}
		else
			sightState = SIGHTSTATE_NOT_IN_SIGHT;
		break;
	case SIGHTSTATE_PEEK_FIRST_POSITIVE:
		if(CordCheck(fromPos,toPos,target,CORD_WORLD_AND_ACTORS))
			sightState = SIGHTSTATE_IN_SIGHT_NORMAL;
		else
			sightState = SIGHTSTATE_IN_SIGHT_PEEK;
		break;
	case SIGHTSTATE_IN_SIGHT_NORMAL:
		if(!CordCheck(fromPos,toPos,target,CORD_WORLD_AND_ACTORS))
			sightState = SIGHTSTATE_NOT_IN_SIGHT;
		break;
	case SIGHTSTATE_IN_SIGHT_PEEK:
		if(!CordCheck(fromPos,toPos,target,CORD_WORLD_AND_ACTORS))
			sightState = SIGHTSTATE_NOT_IN_SIGHT;
		break;
	case SIGHTSTATE_PEEKING_TRY_NORM:
		if(CordCheck(fromPos,toPos,target,CORD_WORLD_AND_ACTORS))
		{
			sightState = SIGHTSTATE_IN_SIGHT_NORMAL;
			desiredPeekType = PEEK_NONE;
			BodyControlPeek(desiredPeekType);
		}
		else
			sightState = SIGHTSTATE_IN_SIGHT_PEEK;
		break;
	}

	switch(sightState)
	{
	case SIGHTSTATE_NOT_IN_SIGHT:
		if(inSight)
		{
			desiredPeekType = PEEK_NONE;
			BodyControlPeek(desiredPeekType);
			inSight = false;
		}
	case SIGHTSTATE_NO_TERRAIN_OBSCURING:
	case SIGHTSTATE_PEEK_FIRST_POSITIVE:
		break;
	case SIGHTSTATE_IN_SIGHT_NORMAL:
		Sighted(target, toPos);
		break;
	case SIGHTSTATE_IN_SIGHT_PEEK:
		if(!inSight)
			BodyControlPeek(desiredPeekType);
		Sighted(target, toPos);
		peekTimer += g_timer.GetFrameSec();
		if(peekTimer > STOP_LEAN_CHECK)
		{
			sightState = SIGHTSTATE_PEEKING_TRY_NORM;
			peekTimer = 0.0f;
		}
		break;
	}

	if(inSight)
		return target->GetHandle();
	else
		return INVALID_OBJECT_HANDLE;
}
//============================================================================
void AiPerception::Sighted(CActor* target, Vector3 toPos, bool frameOnly)
{
	CParticipantIF* particIF = (CParticipantIF*)target->GetInterface(CParticipantIF::GetID());
	if (particIF == NULL)
		return;

	if(aiBehavior == NULL)  //grab this component permanently, but wait til you need it first time
		aiBehavior = (AiBehavior*)( actor.GetComponentByName( "AiBehavior" ) );


	if(((CCompActor*)target)->GetProperty( "health" )->GetFloat() <= 0.0f)
	{
		Vector3 deathPosition = target->GetFocusInWorld();
		deathPosition.y(target->GetBodyInWorld().y());
		SetCurrentPos(deathPosition);

		//cops don't double tap, only double tap avatar
		if(!aiBehavior->isGang || target != g_players.GetPlayer(0))
		{
			inSight = false;
			return;
		}
		else
		{
			//if you have a nade, don't throw
			AiWeaponControl* aiWeaponControl = (AiWeaponControl*) actor.GetComponentByName( "AiWeaponControl" );
			if(aiWeaponControl && aiWeaponControl->GetSelectedClass() == WEAPONCLASS_LOBBED)
			{
				inSight = false;
				return;
			}
		}
	}
	else
	{
		Vector3 curpos = target->GetFocusInWorld();
		//curpos.y(curpos.y() - SIGHTED_Y_OFFSET);
		SetCurrentPos(curpos);
	}

	if(lastKnownTarget != target->GetHandle() && lastKnownTarget != INVALID_OBJECT_HANDLE)
		int interesting  = 1;
	lastKnownTarget = target->GetHandle();

	percentVisible = 1.0f;
	lastCentroid = toPos;
	//if new acquisition, communicate with body for peeking etc.
	//if(!inSight && sightState == SIGHTSTATE_IN_SIGHT_PEEK)
	//		BodyControlPeek(desiredPeekType);

	if(!inSight)
	{
		if(lastSeenTime == 0.0f || g_timer.GetStartSec() - lastSeenTime > TIME_FRAME_CURRENT)
		{
			g_messageDispatcher.SendMessageToAll("TeammateAlerted", (void*)target, actor.GetHandle());
			if(((CCompActor*)target)->GetProperty( "hostage" )->GetHandle() != INVALID_OBJECT_HANDLE)
				g_messageDispatcher.SendMessageToAll( "HostageGrabbed", (void*)target, actor.GetHandle());
		}
	}

	lastSeenTime = g_timer.GetStartSec();
	if(frameOnly)
		inSight = false;
	else
	{
		if(!inSight)
			inFocusTime = g_timer.GetStartSec();
      inSight = true;
		setEchoLastSeen = true;
	}
	fov = FOV_ALERT;
}
//============================================================================
Vector3 AiPerception::GetFromPos()
{
	switch(sightState)
	{
	case SIGHTSTATE_IN_SIGHT_NORMAL:
	case SIGHTSTATE_PEEK_FIRST_POSITIVE:
			return actor.GetFocusInWorld();
			break;
	case SIGHTSTATE_PEEKING_TRY_NORM:
		if(desiredPeekType == PEEK_CROUCH)
		{
			//fake a standing height, since the current focus has him crouched
			Vector3 standingPos = actor.GetBodyInWorld();
			standingPos.y(standingPos.y() + 1.5f);
			return standingPos;
		}
		else
			return actor.GetFocusInWorld();
		break;
	}

	Vector3 peekOffset;
	peekOffset.Clear();

	switch(desiredPeekType)
	{
	default:
	case PEEK_NONE:
		break;
	case PEEK_RIGHT:
		peekOffset = actor.GetBodyToWorld().GetRow0();
		peekOffset *= -0.35f;
		peekOffset.y(peekOffset.y() - 0.1f);
		break;
	case PEEK_LEFT:
		peekOffset = actor.GetBodyToWorld().GetRow0();
		peekOffset *= 0.35f;
		peekOffset.y(peekOffset.y() - 0.1f);
		break;
	case PEEK_CROUCH:
		peekOffset.y(1.1f);
		break;
	}

	Vector3 fromPos;
	if(desiredPeekType == PEEK_CROUCH)
		fromPos = actor.GetBodyInWorld() + peekOffset;
	else
		fromPos = actor.GetFocusInWorld() + peekOffset;

	return fromPos;
}
//============================================================================
Vector3 AiPerception::GetToPos(CActor* target)
{
	Vector3 toPos;
	if(target->IsA( CCompActor::ClassType() ) )
		toPos = (((CCompActor*)target)->GetProperty("headPosition"))->GetVector3();
	else
		toPos = target->GetFocusInWorld();

	switch(sightState)
	{
	case SIGHTSTATE_IN_SIGHT_NORMAL:
	case SIGHTSTATE_PEEK_FIRST_POSITIVE:
	case SIGHTSTATE_PEEKING_TRY_NORM:
			return toPos;
		break;
	}

	switch(desiredPeekType)
	{
	default:
	case PEEK_NONE:
		break;
	case PEEK_RIGHT:
		{
			toPos.y(toPos.y() - 0.2f);
			Vector3 peekOffset = actor.GetBodyToWorld().GetRow0();
			peekOffset *= -0.2f;
			toPos += peekOffset;
		}
		break;
	case PEEK_LEFT:
		{
			toPos.y(toPos.y() - 0.2f);
			Vector3 peekOffset = actor.GetBodyToWorld().GetRow0();
			peekOffset *= 0.2f;
			toPos += peekOffset;
		}
		break;
	case PEEK_CROUCH:
		toPos = target->GetBodyInWorld();
		toPos.y(toPos.y() + 1.1f);
		break;
	}

	return toPos;
}
//============================================================================
void AiPerception::AddSoundIncident(int type, Vector3 location, float intensity, float gameTime, int participantId, int teamId)
{
	if(lastHeardTime != 0.0f)
		soundIncidentCursor++;

	if(soundIncidentCursor >= SOUND_INCIDENT_TABLE_SIZE)
		soundIncidentCursor = 0;

	SoundIncidentTable[soundIncidentCursor].type = type;
	SoundIncidentTable[soundIncidentCursor].intensity = intensity;
	SoundIncidentTable[soundIncidentCursor].gameTime = gameTime;
	SoundIncidentTable[soundIncidentCursor].location = location;
	SoundIncidentTable[soundIncidentCursor].participantId = participantId;
	SoundIncidentTable[soundIncidentCursor].teamId = teamId;

	lastHeardTime = gameTime;
	fov = FOV_ALERT;
	ASSERT(lastHeardTime != 0.0f);  //just in case, let's see if this can hit
}
//============================================================================
bool AiPerception::GetRecentContact(float seconds)
{
	if( (g_timer.GetStartSec() - GetLastSeenOrHeardTime()) > seconds)
		return false;
	return true;
}
//============================================================================
bool AiPerception::GetRecentSighting(float seconds)
{
	if( (g_timer.GetStartSec() - GetLastSeenTime()) > seconds)
		return false;
	return true;
}
//============================================================================
void AiPerception::ActorDied::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	if(i_sender == d_ref->lastKnownTarget)
	{
		d_ref->lastKnownTarget = INVALID_OBJECT_HANDLE;
		d_ref->inSight = false;
	}
}
//============================================================================
void AiPerception::ShootInstStart::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	CActor* sender = CActor::FromHandle(i_sender);
	if (sender == NULL)
		return;

	Vector3 distVec = sender->GetBodyInWorld() - d_ref->actor.GetBodyInWorld();
	//dbgPrint("dist %f\n",distVec.LengthSquared());
	if(distVec.LengthSquared() > 1000.0f)
		return;
	d_ref->Sighted(sender,sender->GetFocusInWorld());
	d_ref->lastKnownTarget = i_sender;
	d_ref->SetCurrentPos(sender->GetFocusInWorld());
	d_ref->percentVisible = 1.0f;
	d_ref->lastCentroid = sender->GetFocusInWorld();
	d_ref->lastSeenTime = g_timer.GetStartSec();
	d_ref->inSight = true;

	AiNavigation *aiNavigation = (AiNavigation *)d_ref->actor.GetComponentByName( "AiNavigation" );
	if(aiNavigation)
	{
		aiNavigation->SetGoalCheckTimer(10.0f);
	}
}
//============================================================================
void AiPerception::ShootInstStop::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	if(i_sender == d_ref->lastKnownTarget)
	{
		d_ref->lastKnownTarget = INVALID_OBJECT_HANDLE;
		d_ref->inSight = false;
	}

	AiNavigation *aiNavigation = (AiNavigation *)d_ref->actor.GetComponentByName( "AiNavigation" );
	if(aiNavigation)
		aiNavigation->SetGoalCheckTimer(Math::Clamp(aiNavigation->GetGoalCheckTimer(),0.0f,1.5f));
}
//============================================================================
void AiPerception::Taunted::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	CActor* sender = CActor::FromHandle(i_sender);
	if(sender == NULL)
		return;

	if(!g_referee.GetTeamingModel().AreEnemies(CParticipantIF::GetParticipantIdFromActor(&d_ref->actor),CParticipantIF::GetParticipantIdFromActor(sender)))
		return;

	Vector3 toTarget = sender->GetFocusInWorld() - d_ref->actor.GetFocusInWorld();
	if(toTarget.LengthXZSquared() > DISTSQRD_MEDIUM)
		return;

	d_ref->AddSoundIncident(ALERT_SCREAM, sender->GetFocusInWorld());
	if(d_ref->aiBehavior)
		d_ref->aiBehavior->SetTauntTimer(g_random.InRange(1.0f,2.0f));
}
//============================================================================
bool AiPerception::AvatarCanSeeMe()
{
	CActor* playerActor = g_players.GetPlayer(0);  //0 is avatar
	if(playerActor)
	{
		Vector3 toTarget = actor.GetFocusInWorld() - g_cameraDirector.CurrentCamera().Eye();
		float distSquared = toTarget.LengthSquared();

		if(distSquared > MAX_VIEW_DIST_SRQD)
			return false;

		float dotCheck = g_cameraDirector.CurrentCamera().Dir().Dot(toTarget);
		if(dotCheck <= 0.0f)
			return false;

		CLineEntity line;
		line.Line().SetOriginEnd(g_cameraDirector.CurrentCamera().Eye(), actor.GetFocusInWorld() );
		//		line.SetIgnoreEntity(toActor);
		//		line.SetIgnoreEntity(&actor);

		CCollisionStackArray<CStageEntity*> ignoreList;
		ignoreList.InitArray();
		CStageEntity** entry1 = ignoreList.ExpandArray();
		*entry1 = (CStageEntity*) playerActor;
		CStageEntity** entry2 = ignoreList.ExpandArray();
		*entry2 = (CStageEntity*) &actor;
		line.SetIgnoreList(&ignoreList);

		// Now cast it
		g_collisionSystem.ResetStack();

		CLineQuery test;
		if (!test.Test(line))
			return true;
		return false;
	}
	return false;
}
//============================================================================
//============================================================================
void AiPerception::AvatarSpotted::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	if(d_ref->inSight)
		return;

	if(!g_referee.GetTeamingModel().IsPlayerEnemy(d_ref->myId) || (g_players.GetPlayer(0) == NULL))
		return;

	d_ref->AddSoundIncident(ALERT_SCREAM, g_players.GetPlayer(0)->GetFocusInWorld());

	AiPsyche* aiPsyche = (AiPsyche*) d_ref->actor.GetComponentByName( "AiPsyche" );
	if(aiPsyche)
		aiPsyche->StimulusInput(AiPsyche::STIMULUS_ENEMY_PRESENCE, 1.0f);
}
//============================================================================
//============================================================================
//============================================================================
//============================================================================
//============================================================================