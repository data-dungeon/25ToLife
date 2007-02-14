#include "TTLPCH.h"

#include "components/Ai/AiBehavior.h"
#include "Components/ComponentCreator.h"
#include "Interfaces/MoveIF.h"
#include "Interfaces/ViewIF.h"
#include "InterfacesTTL/ParticipantIF.h"
#include "components/KineControl/BipedKineControl.h"
#include "components/KineControl/BipedModel.h"
#include "Environ/Munition.h"
#include "data_h_files/players/players_s.h"
#include "stage/protagonist.h"
#include "avatar/pstat.h"
#include "weapons/lobbed.h"
#include "CollideHelper/AabbEntity.h"
#include "components/affliction.h"
#include "environ/ConfigMgr.h"
  
#define RUN_SPEED 6.0f
#define STATE_CONTROL_TIME 2.0f
#define HANDSUP_THRESHOLD 1.0f
#define MIN_HANDSUP_TIME 6.0f
#define REACTION_TIME 0.1f
#define CULL_XZSQR_LENGTH 10000.0f
#define VOICE_FIRST_SEE_CHANCE 1.0f
#define VOICE_IDLE_TIME_LOW 20.0f
#define VOICE_IDLE_TIME_HIGH 40.0f
#define VOICE_BATTLE_TIME_LOW 5.0f
#define VOICE_BATTLE_TIME_HIGH 25.0f
#define VOICE_LOSS_CONTACT 20.0f
#define SIGHT_CHECK_DELAY 5
#define TERMINATOR_TIME 50.0f

#define DISTXZSQRD_VIRTUALY_THERE 1.0f
#define DISTXZSQRD_TINY 5.0f
#define DISTXZSQRD_CLOSE 30.0f
#define DISTXZSQRD_NEAR 150.0f
#define DISTXZSQRD_HELLO 250.0f
#define DISTXZSQRD_MEDIUM 800.0f
#define DISTXZSQRD_LONG 2000.0f
#define DISTXZSQRD_FAR 4000.0f
#define DISTXZSQRD_WOW 8000.0f

#define HOSTAGE_ANCHOR_DIST2RD 50.0f
#define INTROTIME_MIN 1.0f
#define INTROTIME_MAX 5.0f

#define MAX_NPC_DELETE_DIST2RD 200.0f
#define MAX_NPCS 12
#define AFFLICTION_DELAY 1.3f

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(AiBehavior, AiBehavior, COMPONENT_NEUTRAL_PRIORITY);

//============================================================================
AiBehavior::AiBehavior( CCompActor &i_actor ) :
CActorComponent( i_actor )
{
	npcCount = 0;
}
//============================================================================
AiBehavior::~AiBehavior()
{
	if(IsInitialized() && aiWeaponControl)
		g_messageDispatcher.SendMessageToAll( "NpcDied", NULL, actor.GetHandle());
	aiPerception = NULL;
	aiWeaponControl = NULL;
	aiPsyche = NULL;
}
//============================================================================
void AiBehavior::Initialize( void )
{
	aiPerception = (AiPerception*)( actor.GetComponentByName( "AiPerception" ) );
	if(aiPerception == NULL)
	{
		actor.AttachComponent("AiPerception");
		aiPerception = (AiPerception*)( actor.GetComponentByName( "AiPerception" ) );
	}
	aiWeaponControl = (AiWeaponControl*)( actor.GetComponentByName( "AiWeaponControl" ) );
	pStats = (PlayerStats*)actor.GetInterface(PlayerStats::GetID());
	// This can happen in multiplayer games.
	if (!pStats)
	{
		actor.AttachComponent("PlayerStatsMP");
		pStats = (PlayerStats*)actor.GetInterface(PlayerStats::GetID());
		pStats->Initialize();
	}
	aiPsyche = (AiPsyche*)( actor.GetComponentByName( "AiPsyche" ) );
	pathFollower = (CPathFollower*)( actor.GetComponentByName( "PathFollower" ) );
	aiNavigation = (AiNavigation*)( actor.GetComponentByName( "AiNavigation" ) );
	aiBodyControl = (AiBodyControl*)( actor.GetComponentByName( "AiBodyControl" ) );
	aiCombatStrategy = (AiCombatStrategy*)( actor.GetComponentByName( "AiCombatStrategy" ) );
	afflictions = (Afflictions*)( actor.GetComponentByName( "Afflictions" ) );
	sayComponent = (Say*)( actor.GetComponentByName( "Say" ) );

	//must have's
	//ASSERT(aiBodyControl);
	//ASSERT(aiPsyche);
	ASSERT(pStats);
	ASSERT(aiPerception);

	CParticipantIF* myParticipant =
			(CParticipantIF*)actor.GetInterface(CParticipantIF::GetID());
	if (myParticipant == NULL)
		participantId = -1;
	else
		participantId = myParticipant->GetParticipantId();

	if(pathFollower)
		normalPathSpeed = pathFollower->GetSpeedTarget();
	else
		normalPathSpeed = 0.0f;

	//events
	d_respawnNPCHandler.Initialize("RespawnNPC", MESSAGE_HANDLE_IF_MASTER |
		MESSAGE_ROUTE_TO_MASTER, actor.GetMessageOwner(), this);
	d_manDownHandler.Initialize("ManDown",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_npcDownHandler.Initialize("NpcDown", MESSAGE_HANDLE_IF_DUPLICA |
		MESSAGE_ROUTE_TO_DUPLICAS, actor.GetMessageOwner(), this);
	d_hostageTaker.Initialize("HostageTaker",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_hostage.Initialize("Hostage", MESSAGE_HANDLE_IF_MASTER |
		MESSAGE_HANDLE_IF_DUPLICA | MESSAGE_ROUTE_TO_DUPLICAS,
		actor.GetMessageOwner(), this);
	d_hostageGrabbedHandler.Initialize("HostageGrabbed",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_hostageGoneFarHandler.Initialize("HostageGoneFar",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_startaffliction.Initialize("startaffliction",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_endaffliction.Initialize("endaffliction",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_killInside.Initialize("KillInside",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_RemoveFromGame.Initialize("RemoveFromGame",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_timeout.Initialize("NPCTimeout",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_timein.Initialize("NPCTimein",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_vaultTriggerHandler.Initialize("VaultTrigger",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_jumpHereHandler.Initialize("JumpHere",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_trespassHandler.Initialize("Trespass",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_playerSpawningHandler.Initialize("PlayerSpawning",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_enemyGunfireHandler.Initialize("EnemyGunfire",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_npcSpawnedHandler.Initialize("NpcSpawned",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_npcDiedHandler.Initialize("NpcDied",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	startCinematicHandler.Initialize("StartCinematic", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	endCinematicHandler.Initialize("EndCinematic", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	approveSoundHandler.Initialize("ApproveSound", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	killedHostageHandler.Initialize("KilledHostage", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	hostageSavedHandler.Initialize("HostageSaved", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	performHandler.Initialize("Perform", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	resetCheckpointHandler.Initialize("KillAllAtCheckpoint", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	setCheckpointHandler.Initialize("SetCheckpoint", MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	
	handsup = false;
	stateControlTime = STATE_CONTROL_TIME;

	//decide general type
	if(actor.Parm().GetInt("dancer") == 1)
		aiType = AI_DANCER;
	else if (actor.Parm().GetInt("dj") == 1)
		aiType = AI_DJ;
	else if (strcmpi(actor.Parm().GetString("name"), "bistander") == 0)
		aiType = AI_STATIONARY;
	else if (strcmpi(actor.Parm().GetString("name"), "pedestrian") == 0)
		aiType = AI_PATHFOLLOWER;
	else if (strcmpi(actor.Parm().GetString("name"), "bambi") == 0)
		aiType = AI_BAMBI;
	else
		aiType = AI_COMBATANT;

	isGang = false;
	int npc_team = actor.Parm().GetInt("npc_team");
	switch(npc_team)
	{
		case OBJNPCTEAM_ENEMY:
		case OBJNPCTEAM_ENEMY2:
			{
				char *role = actor.Parm().GetString( "role" );
				if( role && strcmp( role, "gang" ) == 0 )
					isGang = true;
				aiType = AI_COMBATANT;
				desiredState = STATE_FIGHT;
			}
			break;
		case OBJNPCTEAM_FRIENDLY:
			aiType = AI_COMBATANT;
			desiredState = STATE_FIGHT;
			break;
		default:
			break;
	}

	npcCount = 0;
	if(aiWeaponControl)
		g_messageDispatcher.SendMessageToAll( "NpcSpawned", NULL, actor.GetHandle());

	Reset();
}
//============================================================================
void AiBehavior::Reset( void )
{
	aiState = STATE_DO_JOB;

	if(actor.Parm().GetInt("starts_sitting") == 1)
		desiredState = STATE_SITTING;
	else if(aiType != AI_COMBATANT)
		desiredState = STATE_DO_JOB;
	else
		desiredState = STATE_FIGHT;
	stateWithinState = AI_STATE_ENTERING;
	stateTimer = 0.0f;
	handsupTimer = 0.0f;
	runWhenYouCan = false;
	GetActor().SyncActor();
	//reset pstats hitpoints....this is also necessary because pstats is created before objattr data available
	float maxHealth = actor.Parm().GetFloat("hitpoints");
	pStats->SetMaxHealth(Math::Clamp(maxHealth,1.0f,1000.0f));
	pStats->SetHealthToFull();
	if(actor.Parm().GetInt("invincible") == 1)
		pStats->SetInvincible(true);
	//tauntTimer = -1.0f;
	tauntTimer = g_random.InRange(VOICE_IDLE_TIME_LOW,VOICE_IDLE_TIME_HIGH);
	varianceTimer = -1.0f;
	rangeSquared = actor.Parm().GetFloat("range") * actor.Parm().GetFloat("range");
	sightCheckDelay = g_random.InRange(SIGHT_CHECK_DELAY,SIGHT_CHECK_DELAY * 2);
	m_startedAction = false;
	m_fightState = FIGHTSTATE_IDLE;
	m_affliction = -1;
	vaultTimer = 0.0f;
	timeout = false;
	hostageAnchor.Clear();
	targetActorHandle = INVALID_OBJECT_HANDLE;
	if(aiWeaponControl)
		introAnimTime = g_timer.GetStartSec() + g_random.InRange(INTROTIME_MIN,INTROTIME_MAX);
	else
		introAnimTime = 0.0f;

	freeUseTimer = 0.0f;

	terminatorTimer = TERMINATOR_TIME;

	hostageTakerHandle = INVALID_OBJECT_HANDLE;
	canBeHostage = true;
	m_afflictionDelay = -1.0f;
	playVoice = Say::VOICETYPE_NONE;
}
//============================================================================
void AiBehavior::BeginFrame()
{
	if(aiState == STATE_DEAD)
		return;

	if (pStats && !pStats->IsAlive())
	{
		SetDesiredState(STATE_DEAD);
		return;
	}

	if(RangeCulling() || timeout)
		return;

	float frameSec = g_timer.GetFrameSec();
	Math::MoveToZero(&vaultTimer,frameSec);

	if(m_afflictionDelay != -1.0f)
	{
		Math::MoveToZero(&m_afflictionDelay,frameSec);
		if(m_afflictionDelay == 0.0f)
		{
			BeginAfflication();
			m_afflictionDelay = -1.0f;
		}
	}

	if(introAnimTime != 0.0f && introAnimTime < g_timer.GetStartSec())
	{
		if(aiPerception->GetLastSeenTime() == 0.0f)
		{
			if(aiPerception->AvatarCanSeeMe())
			{
				aiBodyControl->InroAnim();
				introAnimTime = 0.0f;
			}
			else
				introAnimTime = g_timer.GetStartSec() + g_random.InRange(INTROTIME_MIN,INTROTIME_MAX);
		}
		else
			introAnimTime = 0.0f;
	}

	switch(aiState)
	{
	case STATE_HOSTAGE:
	case STATE_STUNNED:
	case STATE_ON_FIRE:
	case STATE_VAULTING:
	case STATE_SITTING:
		RunState(frameSec);
		return;
		break;
	case STATE_SMOKED:
	case STATE_HOLDING_FIRE:
		RunState(frameSec);
		break;
	default:
		if(stateTimer == 0.0f)
		{
			StateControl();
			stateTimer = stateControlTime;
		}
		else
			Math::MoveToZero(&stateTimer,frameSec);
		RunState(frameSec);
		Math::MoveToZero(&terminatorTimer,frameSec);
		if(terminatorTimer <= 0.0f && aiCombatStrategy && aiCombatStrategy->GetTactic() != AiCombatStrategy::TACTIC_COVER && aiPerception->GetLastSeenTime() != 0.0f)
		{
			aiPerception->SeedAvatarPosition();
			aiCombatStrategy->SetSuicidal();
			terminatorTimer = TERMINATOR_TIME;
		}
		break;
	}

	if(aiPsyche)
		aiPsyche->Update(frameSec);

	if(aiPerception)
		aiPerception->Update(frameSec);

	if(aiBodyControl)
		aiBodyControl->Update(frameSec);

	if(aiNavigation)
		aiNavigation->Update(frameSec);

	//voices in response to messages handled in update
	if(sayComponent && playVoice != Say::VOICETYPE_NONE)
	{
		switch(playVoice)
		{
			case Say::VOICETYPE_BUDDY_DIED:
				sayComponent->PlayVoiceType(Say::VOICETYPE_BUDDY_DIED,0.1f,1.5f,false);
				break;
			case Say::VOICETYPE_HOSTAGE_KILLED:
				sayComponent->PlayVoiceType(Say::VOICETYPE_HOSTAGE_KILLED,0.0f,2.5f);
				break;
			case Say::VOICETYPE_HOSTAGE_SAVED:
				sayComponent->PlayVoiceType(Say::VOICETYPE_HOSTAGE_SAVED,0.5f,3.0f,false);
				break;
			case	Say::VOICETYPE_BATTLE_TALK:
				sayComponent->PlayVoiceType(Say::VOICETYPE_BATTLE_TALK,0.5f,1.5f);
				break;
		}
		playVoice = Say::VOICETYPE_NONE;
	}
}
//============================================================================
void AiBehavior::SetDesiredState(int i_state)
{
	if(aiState == STATE_DEAD || desiredState == STATE_DEAD)
		return;
	desiredState = i_state;
	if(desiredState != aiState)
		stateWithinState = AI_STATE_EXITING;
	StatePaperWork();
}
//============================================================================
void AiBehavior::SetDesiredStateToHighestWant(void)
{
	if(aiType == AI_COMBATANT || aiPsyche == NULL)
	{
		SetDesiredState(STATE_FIGHT);
		return;
	}

	int highestWant = aiPsyche->GetHighestWant();
	int highestWantState = desiredState;

	switch(highestWant)
	{
	case AiPsyche::WANT_DO_JOB:
		if(aiState != STATE_SITTING)
			highestWantState = STATE_DO_JOB;
		break;
	case AiPsyche::WANT_TO_WATCH:
		highestWantState = STATE_WATCH;
		break;
	case AiPsyche::WANT_TO_FLEE:
		highestWantState = STATE_FLEE;
		break;
	case AiPsyche::WANT_TO_COWER:
		highestWantState = STATE_COWER;
		break;
	case AiPsyche::WANT_TO_FIGHT:
		highestWantState = STATE_FIGHT;
		break;
	}

	SetDesiredState(highestWantState);
}
//============================================================================
void AiBehavior::StateControl()
{
	switch(aiState)
	{
		case STATE_DO_JOB:
		case STATE_WATCH:
		case STATE_FLEE:
		case STATE_COWER:
		case STATE_FIGHT:
		case STATE_SITTING:
			if(desiredState != aiState)
				SetDesiredState(desiredState);
			else
				SetDesiredStateToHighestWant();
			break;
	}
}
//============================================================================
void AiBehavior::StatePaperWork()
{
	if(stateWithinState == AI_STATE_EXITING)
	{
		switch(aiState)
		{
		case STATE_DO_JOB:
			ExitState_DoJob();
			break;
		case STATE_WATCH:
			ExitState_Watch();
			break;
		case STATE_FLEE:
			ExitState_Flee();
			break;
		case STATE_COWER:
			ExitState_Cower();
			break;
		case STATE_FIGHT:
			ExitState_Fight();
			break;
		case STATE_DEAD:
			ExitState_Dead();
			break;
		case STATE_SITTING:
			ExitState_Sitting();
			break;
		case STATE_STUNNED:
			ExitState_Stunned();
			break;
		case STATE_SMOKED:
			ExitState_Smoked();
			break;
		case STATE_ON_FIRE:
			ExitState_OnFire();
			break;
		case STATE_VAULTING:
			ExitState_Vaulting();
			break;
		case STATE_HOSTAGE:
			ExitState_Hostage();
			break;
		case STATE_HOLDING_FIRE:
			ExitState_HoldingFire();
			break;
		}
		stateWithinState = AI_STATE_ENTERING;
	}

	if(stateWithinState == AI_STATE_ENTERING)
	{
		aiState = desiredState;

		switch(aiState)
		{
		case STATE_DO_JOB:
			EnterState_DoJob();
			break;
		case STATE_WATCH:
			EnterState_Watch();
			break;
		case STATE_FLEE:
			EnterState_Flee();
			break;
		case STATE_COWER:
			EnterState_Cower();
			break;
		case STATE_FIGHT:
			EnterState_Fight();
			break;
		case STATE_DEAD:
			EnterState_Dead();
			break;
		case STATE_SITTING:
			EnterState_Sitting();
			break;
		case STATE_STUNNED:
			EnterState_Stunned();
			break;
		case STATE_SMOKED:
			EnterState_Smoked();
			break;
		case STATE_ON_FIRE:
			EnterState_OnFire();
			break;
		case STATE_VAULTING:
			EnterState_Vaulting();
			break;
		case STATE_HOLDING_FIRE:
			EnterState_HoldingFire();
			break;
		case STATE_HOSTAGE:
			EnterState_Hostage();
			break;
		}
		stateWithinState = AI_STATE_RUNNING;
	}
}
//============================================================================
void AiBehavior::RunState(float framesec)
{
	switch(aiState)
	{
	case STATE_DO_JOB:
		RunState_DoJob(framesec);
		break;
	case STATE_WATCH:
		RunState_Watch(framesec);
		break;
	case STATE_FLEE:
		RunState_Flee(framesec);
		break;
	case STATE_COWER:
		RunState_Cower(framesec);
		break;
	case STATE_FIGHT:
		RunState_Fight(framesec);
		break;
	case STATE_DEAD:
		RunState_Dead(framesec);
		break;
	case STATE_SITTING:
		RunState_Sitting(framesec);
		break;
	case STATE_ON_FIRE:
		RunState_OnFire(framesec);
		break;
	case STATE_STUNNED:
		RunState_Stunned(framesec);
		break;
	case STATE_SMOKED:
		RunState_Smoked(framesec);
		break;
	case STATE_VAULTING:
		RunState_Vaulting(framesec);
		break;
	case STATE_HOLDING_FIRE:
		RunState_HoldingFire(framesec);
		break;
	case STATE_HOSTAGE:
		RunState_Hostage(framesec);
		break;
	}
}
//============================================================================
void AiBehavior::Targeting(int type)
{
	if(aiPerception)
	{
		if(targetActorHandle == INVALID_OBJECT_HANDLE)
		{
			if(sightCheckDelay)
			{
				sightCheckDelay--;
				return;
			}
         sightCheckDelay = SIGHT_CHECK_DELAY;
		}

		targetActorHandle = aiPerception->VisionCheck();

		//cheats
		CActor* targetActor = CActor::FromHandle(targetActorHandle);
		if (Cheats::Enabled(Cheats::STEALTH, targetActor))
			targetActorHandle = INVALID_OBJECT_HANDLE;
	}
}
//============================================================================
void AiBehavior::Aiming(float frameSec)
{
	//aiming
	if(aiWeaponControl)
	{
		if(targetActorHandle != INVALID_OBJECT_HANDLE)
		{
			aiWeaponControl->AimAtTarget();
			aiWeaponControl->FireAtWill(true);
		}
		else
			aiWeaponControl->FireAtWill(false);
	}
}
//============================================================================
void AiBehavior::FaceStimulus(void)
{
	if(aiBodyControl)
		return;

	if(aiPerception)
	{
		Vector3 location; 
		location.Clear();
		if(aiPerception->GetSoundHeard())
		{
			if(aiPerception->GetLastSoundLocation(location))
			{
				if(aiBodyControl)
					aiBodyControl->TurnToPoint(location);
			}
		}
	}
}
//============================================================================
void AiBehavior::ProcessHandsUp(float frameSec)
{
	Math::MoveToZero(&handsupTimer,frameSec);

	if(handsupTimer > 0.0f)
		return;

	//unsyncronize from fellow bots
	if(varianceTimer == -1.0f)
		varianceTimer = g_random.Unit();
	Math::MoveToZero(&varianceTimer,frameSec);
	if(varianceTimer > 0.0f)
		return;
	varianceTimer = -1.0f;

	float desire = aiPsyche->GetWantAccessory(AiPsyche::WANT_ACCESSORY_HANDSUP);
	if(!handsup && desire > HANDSUP_THRESHOLD)
	{
		BipedModel* model = (BipedModel*)actor.GetComponentByName( "BipedModel" );
		if( model )
			model->StartHandsUp();
		handsup = true;
		handsupTimer = MIN_HANDSUP_TIME;
	}
	else if(handsup && desire < HANDSUP_THRESHOLD)
	{
		BipedModel* model = (BipedModel*)actor.GetComponentByName( "BipedModel" );
		if( model )
			model->StopHandsUp();
		handsup = false;
		handsupTimer = MIN_HANDSUP_TIME;
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATES
//============================================================================
void AiBehavior::EnterState_DoJob(void)
{
	switch(aiType)
	{
	case AI_DANCER:
		if(actor.Parm().GetInt("female_dance") == 1)
			g_messageDispatcher.SendMessageToActor( "DanceFemale", NULL, actor.GetHandle(), actor.GetHandle() );
		else if(actor.Parm().GetInt("male_dance") == 1)
			g_messageDispatcher.SendMessageToActor( "DanceMale", NULL, actor.GetHandle(), actor.GetHandle() );
		else
			g_messageDispatcher.SendMessageToActor( "Dance", NULL, actor.GetHandle(), actor.GetHandle() );
		break;
	case AI_DJ:
		g_messageDispatcher.SendMessageToActor( "DJTurntable", NULL, actor.GetHandle(), actor.GetHandle() );
		break;
	}

	if(pathFollower)
	{
		pathFollower->SetState((uint8)STATE_DO_JOB);
		if(actor.Parm().GetInt("evacuator") == 1)
		{
			pathFollower->SetSpeedTarget(0.0f);
			pathFollower->AdjustSpeed(0.0f);
			pathFollower->SetFaceMovementDirection(false);
			pathFollower->Deactivate();
		}
		else
		{
			if(pathFollower->GetSpeedCurrent() < 0.0f)
				pathFollower->SetSpeedTarget(normalPathSpeed * -1.0f);
			else
				pathFollower->SetSpeedTarget(normalPathSpeed);
			pathFollower->Activate();
		}
	}
	else if(aiNavigation)
	{
		aiNavigation->SetType(AiNavigation::NAVTYPE_NONCOMBAT);
	}
	if(aiBodyControl)
		aiBodyControl->SetBodyControlState(AiBodyControl::BODYCONTROL_DISABLED);

	if(aiWeaponControl)
		aiWeaponControl->SelectNoWeapon();
}
//============================================================================
void AiBehavior::RunState_DoJob(float frameSec)
{
	if(pathFollower)
	{
		if((pathFollower->IsAtEndpoint() || pathFollower->GetPathDistanceFromT() >= 1.0f))
		{
			if(actor.Parm().GetInt("remove_at_path_end") == 1)
				actor.MarkForDeath();
			else
				aiPsyche->SetWant(AiPsyche::WANT_TO_COWER,1000.0f);
		}

		if((actor.Parm().GetInt("evacuator") == 1) && (g_players.GetPlayer(0) != NULL))
		{
			Vector3 toAvatar = g_players.GetPlayer(0)->GetBodyInWorld() - actor.GetBodyInWorld();
			if(toAvatar.LengthSquared() < rangeSquared)
			{
				if(sayComponent)
					sayComponent->PlayVoiceType(Say::VOICETYPE_BATTLE_TALK,0.5f,1.5f);

				if(pathFollower->GetSpeedCurrent() < 0.0f)
					pathFollower->SetSpeedTarget(normalPathSpeed * -1.0f);
				else
					pathFollower->SetSpeedTarget(normalPathSpeed);
				pathFollower->Activate();
			}
		}
	}
}
//============================================================================
void AiBehavior::ExitState_DoJob(void)
{
	if(pathFollower)
	{
		pathFollower->SetSpeedTarget(0.0f);
		pathFollower->AdjustSpeed(0.0f);
		pathFollower->SetFaceMovementDirection(false);
		pathFollower->Deactivate();
	}
	BipedMoveIF *moveIF = (BipedMoveIF *)GetActor().GetInterface( BipedMoveIF::GetID() );
	if( moveIF )
	{
		moveIF->CommandForwardSpeed( 0.0f );
		moveIF->CommandSideSpeed( 0.0f );
	}
}
//============================================================================
void AiBehavior::EnterState_Watch(void)
{
	if(aiBodyControl)
		aiBodyControl->SetBodyControlState(AiBodyControl::BODYCONTROL_AMBIENT);

	if(pathFollower)
		pathFollower->SetState((uint8)STATE_WATCH);
}
//============================================================================
void AiBehavior::RunState_Watch(float frameSec)
{
	Targeting(AiPerception::CHECK_FOV_AND_RANGE);  //face aquired player
	if(targetActorHandle == INVALID_OBJECT_HANDLE)
		FaceStimulus();

	ProcessHandsUp(frameSec);
}
//============================================================================
void AiBehavior::ExitState_Watch(void)
{
	targetActorHandle = INVALID_OBJECT_HANDLE;
	if(handsup)
	{
		BipedModel* model = (BipedModel*)actor.GetComponentByName( "BipedModel" );
		if( model )
			model->StopHandsUp();
		handsup = false;
	}
	if(aiPsyche)
		aiPsyche->SetWantAccessory(AiPsyche::WANT_ACCESSORY_HANDSUP,0.0f); //zero it out
}
//============================================================================
void AiBehavior::EnterState_Flee(void)
{
	runWhenYouCan = true;
	if(pathFollower && aiBodyControl)
		aiBodyControl->SetBodyControlState(AiBodyControl::BODYCONTROL_DISABLED);
	if(pathFollower)
		pathFollower->SetState((uint8)STATE_FLEE);
}
//============================================================================
void AiBehavior::RunState_Flee(float frameSec)
{
	ProcessHandsUp(frameSec);
	Targeting(AiPerception::CHECK_FOV_AND_RANGE);

	if(pathFollower == NULL || (pathFollower && (pathFollower->IsAtEndpoint() == true || pathFollower->IsReverseAtEnd() == false)))
	{
		aiPsyche->SetWant(AiPsyche::WANT_TO_FLEE,0.0f);
		aiPsyche->SetWant(AiPsyche::WANT_TO_COWER,1000.0f);
	}
	
	if(runWhenYouCan && actor.AnimAction()!=ACTION_ROAM_COWER)
	{
		if(aiNavigation)
			aiNavigation->SetMode(AiNavigation::NAVMODE_RETREAT);
		else if (pathFollower)
		{
			pathFollower->SetSpeedTarget(RUN_SPEED);  //make maya inputable
			pathFollower->SetFaceMovementDirection(true);
			pathFollower->Activate();

			if(actor.Parm().GetInt("remove_at_path_end") == 1 && (pathFollower->IsAtEndpoint() || pathFollower->GetPathDistanceFromT() >= 1.0f))
				actor.MarkForDeath();
			else if(targetActorHandle != INVALID_OBJECT_HANDLE)
			{
				CActor* targetActor = CActor::FromHandle(targetActorHandle);
				ASSERT(targetActor);
				Vector3 toTarget = 	targetActor->GetBodyInWorld() - actor.GetBodyInWorld();
				float dist = pathFollower->GetPathDistance();
				Vector3 currentPathDir = pathFollower->GetPath()->PointFromDistance(dist - pathFollower->GetSpeedTarget()) - pathFollower->GetPath()->PointFromDistance(dist);

				float dotToTarget = toTarget.Dot(currentPathDir);
				if(dotToTarget < 0.0f)
					pathFollower->SetSpeedTarget(pathFollower->GetSpeedTarget() * -1.0f);
			}
		}

		runWhenYouCan = false;
	}
}
//============================================================================
void AiBehavior::ExitState_Flee(void)
{
	if(handsup)
	{
		BipedModel* model = (BipedModel*)actor.GetComponentByName( "BipedModel" );
		if( model )
			model->StopHandsUp();
		handsup = false;
	}
	
	if(pathFollower)
	{
		pathFollower->SetSpeedTarget(0.0f);
		pathFollower->AdjustSpeed(0.0f);
		pathFollower->SetFaceMovementDirection(false);
		pathFollower->Deactivate();
	}
	BipedMoveIF *moveIF = (BipedMoveIF *)GetActor().GetInterface( BipedMoveIF::GetID() );
	if( moveIF )
	{
		moveIF->CommandForwardSpeed( 0.0f );
		moveIF->CommandSideSpeed( 0.0f );
	}

	if(aiPsyche)
		aiPsyche->SetWantAccessory(AiPsyche::WANT_ACCESSORY_HANDSUP,0.0f); //zero it out
}
//============================================================================
void AiBehavior::EnterState_Cower(void)
{
	if(aiBodyControl)
		aiBodyControl->SetBodyControlState(AiBodyControl::BODYCONTROL_DISABLED);

	freeUseTimer = 0.5f;

	if(pathFollower)
		pathFollower->SetState((uint8)STATE_COWER);
}
//============================================================================
void AiBehavior::RunState_Cower(float frameSec)
{	
	if(freeUseTimer != 69.0f)
      Math::MoveToZero(&freeUseTimer,frameSec);
	if(freeUseTimer == 0.0f)
	{
		if(((CCompActor*)&actor)->GetProperty("hostageTaker")->GetHandle() == INVALID_OBJECT_HANDLE)
		{
			BipedModel* model = (BipedModel*)actor.GetComponentByName( "BipedModel" );
			if( model )
				model->StartCower();
			freeUseTimer = 69.0f;
		}
		else
			freeUseTimer = 0.5f;
	}
}
//============================================================================
void AiBehavior::ExitState_Cower(void)
{
	BipedModel* model = (BipedModel*)actor.GetComponentByName( "BipedModel" );
	if( model )
		model->StopCower();
}
//============================================================================
void AiBehavior::EnterState_Fight(void)
{
	ASSERT(aiWeaponControl);
	if(aiNavigation)
		aiNavigation->SetType(AiNavigation::NAVTYPE_COMBAT);

	if(aiBodyControl)
	{
		aiBodyControl->SetBodyControlState(AiBodyControl::BODYCONTROL_COMBAT);
		aiBodyControl->SetCrouchEnabled(true);
	}
	if(aiWeaponControl)
		aiWeaponControl->SetWeaponControlEnabled(true);

	if(g_referee.GetTeamingModel().IsPlayerFriend(participantId))
		aiPerception->SetHearingRadius(200.0f); //make big so they don't get left behind

	if(pathFollower)
		pathFollower->SetState((uint8)STATE_FIGHT);

	//if nuetral, make enemy
	//const CTeamingModel &teamingModel = g_referee.GetTeamingModel();
	//teamingModel.

}
//============================================================================
void AiBehavior::RunState_Fight(float frameSec)
{
	Targeting(AiPerception::CHECK_FOR_PARTIAL_VISABILITY);
	Aiming(frameSec);
	if(aiWeaponControl)
		aiWeaponControl->Update(frameSec);
	if(aiCombatStrategy)
		aiCombatStrategy->Update(frameSec);
	TauntBehavior(frameSec);
}
//============================================================================
void AiBehavior::ExitState_Fight(void)
{
	if(aiWeaponControl)
		aiWeaponControl->FireAtWill(false);
	if(aiNavigation)
		aiNavigation->SetType(AiNavigation::NAVTYPE_DORMANT);

	if(aiBodyControl)
		aiBodyControl->SetCrouchEnabled(false);

	if(aiWeaponControl)
		aiWeaponControl->SetWeaponControlEnabled(false);

	//*****************************************
	//*****************************************
	//verify that combat strategy agrees we can stop fighting **********TBD*************
	//*****************************************
	//*****************************************
}
//============================================================================
void AiBehavior::EnterState_Dead(void)
{
	if(aiBodyControl)
		aiBodyControl->SetBodyControlState(AiBodyControl::BODYCONTROL_DISABLED);

	if(aiWeaponControl)
	{
		aiWeaponControl->FireAtWill(false);
		aiWeaponControl->SetTrailingTimer(0.0f);
	}

	if(aiNavigation)
	{
		aiNavigation->SetMode(AiNavigation::NAVMODE_STATIONARY);
		aiNavigation->SetType(AiNavigation::NAVTYPE_DORMANT);
	}

	if(aiWeaponControl)
		aiWeaponControl->SetWeaponControlEnabled(false);

	if(pathFollower)
		pathFollower->SetState((uint8)STATE_DEAD);

	//if(sayComponent)
	//{
	//	sayComponent->ClearPending();
	//	sayComponent->PlayVoiceType(Say::VOICETYPE_DEATH,0.1f,0.5f,true);
	//}
}
//============================================================================
void AiBehavior::RunState_Dead(float frameSec)
{
}
//============================================================================
void AiBehavior::ExitState_Dead(void)
{
}
//============================================================================
void AiBehavior::EnterState_Sitting(void)
{
	g_messageDispatcher.SendMessageToActor( "SitDown", NULL, actor.GetHandle(), actor.GetHandle() );

	if(pathFollower)
		pathFollower->SetState((uint8)STATE_SITTING);
}
//============================================================================
void AiBehavior::RunState_Sitting(float frameSec)
{
	Targeting(AiPerception::CHECK_FOR_PARTIAL_VISABILITY);

	if(aiPerception)
		aiPerception->Update(frameSec);

	if(aiPerception && aiPerception->GetLastSeenTime() > 0.0f)
	{
		if(aiPsyche)
			desiredState = STATE_DO_JOB;
		else
			desiredState = STATE_FIGHT;
		StateControl();
		return;
	}

	if(aiPerception->GetAtLeastOneStimulis())
	{
		desiredState = STATE_DO_JOB;
		StateControl();
	}
}
//============================================================================
void AiBehavior::ExitState_Sitting(void)
{
	g_messageDispatcher.SendMessageToActor( "StopSuperCrouch", NULL, actor.GetHandle(), actor.GetHandle() );
}
//============================================================================
void AiBehavior::EnterState_Stunned(void)
{
	if(aiNavigation)
		aiNavigation->SetType(AiNavigation::NAVTYPE_DORMANT);

	BipedMoveIF *moveIF = (BipedMoveIF *)GetActor().GetInterface( BipedMoveIF::GetID() );
	if( moveIF )
	{
		moveIF->CommandForwardSpeed( 0.0f );
		moveIF->CommandSideSpeed( 0.0f );
	}
	//temp until get real animation
	g_messageDispatcher.SendMessageToActor( "OnFire", NULL, actor.GetHandle(), actor.GetHandle() );

	if(pathFollower)
		pathFollower->SetState((uint8)STATE_STUNNED);

	//if(sayComponent)
	//{
	//	sayComponent->PlayVoiceType(Say::VOICETYPE_BLINDED,0.0f,0.2f,true);
		tauntTimer = Math::Clamp(tauntTimer,VOICE_IDLE_TIME_LOW,1000.0f);
	//}
}
//============================================================================
void AiBehavior::RunState_Stunned(float frameSec)
{
}
//============================================================================
void AiBehavior::ExitState_Stunned(void)
{
}
//============================================================================
void AiBehavior::EnterState_Smoked(void)
{
	if(aiNavigation)
	{
		aiNavigation->SetType(AiNavigation::NAVTYPE_COMBAT);
		aiNavigation->SetMode(AiNavigation::NAVMODE_RETREAT);
	}

	if(sayComponent)
	{
		switch(m_affliction)
		{
			case Afflictions::BLIND:
			case Afflictions::TEARGAS:
			sayComponent->PlayVoiceType(Say::VOICETYPE_BLINDED,0.0f,0.2f,true);
			freeUseTimer = g_random.InRange(5.0f,10.0f);
		}
	}
	else
		freeUseTimer = 0.0f;

	if(aiBodyControl)
	{
		aiBodyControl->SetBodyControlState(AiBodyControl::BODYCONTROL_COMBAT);
		aiBodyControl->SetCrouchEnabled(true);
	}

	if(aiPerception)
	{
		aiPerception->inSight = false;
		aiPerception->BodyControlPeek(AiPerception::PEEK_NONE);
	}

	if(aiWeaponControl)
		aiWeaponControl->SetWeaponControlEnabled(true);

	//if(actor.GetProperty( "hostage" )->GetHandle() != INVALID_OBJECT_HANDLE)
	///{
	//	g_messageDispatcher.SendMessageToActor( "HostageTaker", CMessageDispatcher::HandleToData(INVALID_OBJECT_HANDLE), actor.GetProperty( "hostage" )->GetHandle(), actor.GetProperty( "hostage" )->GetHandle() );	
	//	g_messageDispatcher.SendMessageToActor( "Hostage", 0, INVALID_OBJECT_HANDLE, actor.GetHandle());
	//}
}
//============================================================================
void AiBehavior::RunState_Smoked(float frameSec)
{
	if(aiWeaponControl)
		aiWeaponControl->Update(frameSec);

	Math::MoveToZero(&freeUseTimer,frameSec);
	if(freeUseTimer == 0.0f)
	{
		if(sayComponent)
		{
			switch(m_affliction)
			{
			case Afflictions::BLIND:
			case Afflictions::TEARGAS:
				sayComponent->PlayVoiceType(Say::VOICETYPE_BLINDED,0.0f,0.2f,true);
				freeUseTimer = g_random.InRange(5.0f,10.0f);
			}
		}
	}
}
//============================================================================
void AiBehavior::ExitState_Smoked(void)
{
	if(aiNavigation)
		aiNavigation->SetType(AiNavigation::NAVTYPE_DORMANT);

	if(aiBodyControl)
		aiBodyControl->SetCrouchEnabled(false);

	if(aiWeaponControl)
		aiWeaponControl->SetWeaponControlEnabled(false);
	tauntTimer = Math::Clamp(tauntTimer,VOICE_IDLE_TIME_LOW,1000.0f);
}
//============================================================================
void AiBehavior::EnterState_OnFire(void)
{
	if(aiNavigation)
	{
		aiNavigation->StopMoving();
		aiNavigation->SetType(AiNavigation::NAVTYPE_DORMANT);
	}
	//BipedMoveIF *moveIF = (BipedMoveIF *)GetActor().GetInterface( BipedMoveIF::GetID() );
	//if( moveIF )
	//{
	//	moveIF->CommandForwardSpeed( 0.0f );
	//	moveIF->CommandSideSpeed( 0.0f );
	//}


	g_messageDispatcher.SendMessageToActor( "OnFire", NULL, actor.GetHandle(), actor.GetHandle() );

	if(pathFollower)
		pathFollower->SetState((uint8)STATE_ON_FIRE);

	if(sayComponent)
	{
		sayComponent->PlayVoiceType(Say::VOICETYPE_ON_FIRE,0.0f,0.2f,true);
		tauntTimer = Math::Clamp(tauntTimer,VOICE_IDLE_TIME_LOW,1000.0f);
	}
}
//============================================================================
void AiBehavior::RunState_OnFire(float frameSec)
{
}
//============================================================================
void AiBehavior::ExitState_OnFire(void)
{
}
//============================================================================
void AiBehavior::EnterState_Vaulting(void)
{
	if(aiWeaponControl)
		aiWeaponControl->CeaseFire();

	if(aiNavigation)
		aiNavigation->SetType(AiNavigation::NAVTYPE_DORMANT);

	if(aiBodyControl)
		aiBodyControl->SetCrouchEnabled(false);

	if(pathFollower)
		pathFollower->SetState((uint8)STATE_VAULTING);
}
//============================================================================
void AiBehavior::RunState_Vaulting(float frameSec)
{
	if( actor.AnimAction() == ACTION_ROAM_VAULT )
		m_startedAction = true;

	if( m_startedAction && actor.AnimAction() != ACTION_ROAM_VAULT )
	{
		m_startedAction = false;
		SetDesiredStateToHighestWant();
	}
}
//============================================================================
void AiBehavior::ExitState_Vaulting(void)
{
	if(aiNavigation)
	{
		float goalTimer = aiNavigation->GetGoalCheckTimer();
		aiNavigation->SetGoalCheckTimer(Math::Clamp(goalTimer,1.0f,1000.0f)); //don't be wiggin'
		aiNavigation->NavMesh_CreateNewPath();
		actor.PositiveExclude() &= ~WORLD_CATEGORY;
	}
	vaultTimer = 1.0f;
}
//============================================================================
void AiBehavior::EnterState_Hostage(void)
{
	//play a don't hurt me sound
	sayComponent->PlayVoiceType(Say::VOICETYPE_PLEED,0.0f,0.7f,false);
	
	//set anchor point unless it has been set prior
	if(hostageAnchor.x() == 0.0f && hostageAnchor.y() == 0.0f && hostageAnchor.z() == 0.0f)
		hostageAnchor = actor.GetBodyInWorld();

	freeUseTimer = 0.0f;
	return;
}
//============================================================================
void AiBehavior::RunState_Hostage(float frameSec)
{
	//check anchor distance, so player can't be invuln through level
	Vector3 dist = hostageAnchor - actor.GetBodyInWorld();
	Math::MoveToZero(&freeUseTimer,frameSec);
	if(freeUseTimer == 0.0f && dist.LengthSquared() > HOSTAGE_ANCHOR_DIST2RD)
	{
		g_messageDispatcher.SendMessageToAll( "HostageGoneFar", NULL, actor.GetHandle());
		freeUseTimer = 5.0f; //just a bit of a delay so we don't spam
		if(sayComponent)
			sayComponent->PlayVoiceType(Say::VOICETYPE_BATTLE_TALK,0.5f,5.5f,false);
	}
}
//============================================================================
void AiBehavior::ExitState_Hostage(void)
{
	aiPsyche->SetWant(AiPsyche::WANT_TO_FLEE,0.0f);
	aiPsyche->SetWant(AiPsyche::WANT_TO_COWER,1000.0f);

	if(desiredState != STATE_DEAD)
	{
		CCompActor* hostageTaker = CCompActor::FromHandle(hostageTakerHandle);
		if(hostageTaker && hostageTaker->GetProperty("health")->GetFloat() <= 0.0f)
			g_messageDispatcher.SendMessageToAll("HostageSaved",NULL,actor.GetHandle());
	}
}
//============================================================================
void AiBehavior::EnterState_HoldingFire(void)
{
	if(aiNavigation)
		aiNavigation->SetType(AiNavigation::NAVTYPE_COMBAT);

	if(aiWeaponControl)
		aiWeaponControl->SetWeaponControlEnabled(true);

	BipedModel* model = (BipedModel*)actor.GetComponentByName( "BipedModel" );
	if( model )
		model->StartAim();

	sayComponent->PlayVoiceType(Say::VOICETYPE_HOSTAGE_TAKEN,0.0f,2.5f,false);
	freeUseTimer = g_random.InRange(4.0f,8.0f);
}
//============================================================================
void AiBehavior::RunState_HoldingFire(float frameSec)
{
	Targeting(AiPerception::CHECK_FOR_PARTIAL_VISABILITY);
	Aiming(frameSec);
	if(aiCombatStrategy)
		aiCombatStrategy->Update(frameSec);

	//lost target exit state
	if(targetActorHandle == INVALID_OBJECT_HANDLE)
	{
		//SetDesiredStateToHighestWant();
		return;
	}

	//target no longer has hostage exit state
	CCompActor* targetActor = CCompActor::FromHandle(targetActorHandle);
	ASSERT(targetActor);
	if(targetActor->GetProperty("hostage")->GetHandle() == INVALID_OBJECT_HANDLE)
	{
		SetDesiredStateToHighestWant();
		return;
	}

	Math::MoveToZero(&freeUseTimer,frameSec);
	if(freeUseTimer <= 0.0f)
	{
		freeUseTimer = g_random.InRange(4.0f,8.0f);
		sayComponent->PlayVoiceType(Say::VOICETYPE_HOSTAGE_TAKEN,0.0f,2.5f,false);
	}
}
//============================================================================
void AiBehavior::ExitState_HoldingFire(void)
{
	if(aiWeaponControl)
		aiWeaponControl->SetWeaponControlEnabled(false);

	if(aiNavigation)
		aiNavigation->SetType(AiNavigation::NAVTYPE_DORMANT);

	sayComponent->PlayVoiceType(Say::VOICETYPE_BATTLE_TALK,0.0f,2.5f,false);

}
//============================================================================

/////////////////////////////////////////////////////////////////
//EVENTS
/////////////////////////////////////////////////////////////////
void AiBehavior::RespawnNPC::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);
	d_ref->Respawn();
}

bool AiBehavior::RangeCulling()
{
	//very simple range check to cull ai stuff
	CActor *avatar = g_players.GetPlayer(0);
	if(avatar == NULL)
		return true;

	Vector3 toAvatar = avatar->GetBodyInWorld() - actor.GetBodyInWorld();

	if(toAvatar.LengthXZSquared() > CULL_XZSQR_LENGTH)
		return true;

	return false;
}

CInterface *AiBehavior::GetInterface( uint i_id )
{
	if( i_id == InteractionIF::GetID() )
	{
		// we're not interactable if we're dead
		if( pStats && pStats->IsAlive() )
		{
			return (InteractionIF *)this;
		}
	}
	
	return NULL;
}

InteractionMonitor::InteractionType AiBehavior::QueryInteractionType( CCompActor *whoWantsToKnow )
{
	if( GetType() != AI_COMBATANT )
	{
		if( !canBeHostage )
			return InteractionMonitor::INTERACTION_NONE;

		Property *prop = whoWantsToKnow->QueryProperty( "hostage" );
		if( prop && ((ActorHandle)prop->GetHandle()).Valid() )
		{
			// greedy fucker!  there can be only one
			return InteractionMonitor::INTERACTION_NONE;
		}
		char *role = whoWantsToKnow->Parm().GetString( "role" );
		if( role && strcmp( role, "cop" ) == 0 )
			return InteractionMonitor::INTERACTION_NONE; // cops may not take hostages!
		
		if( GetActor().Parm().GetInt( "disable_hostage" ) == 0 )
			return InteractionMonitor::INTERACTION_NONE;

		// if I'm already a hostage, don't let somebody else take me
		prop = GetActor().QueryProperty( "hostageTaker" );
		if( prop && ((ActorHandle)prop->GetHandle()).Valid() )
			return InteractionMonitor::INTERACTION_NONE;

		return InteractionMonitor::INTERACTION_HOSTAGE;
	}
	return InteractionMonitor::INTERACTION_NONE;
}
//============================================================================
void AiBehavior::TauntBehavior(float frameSec)
{
	if(sayComponent)
	{
		Math::MoveToZero(&tauntTimer,frameSec);

		switch(m_fightState)
		{
		case FIGHTSTATE_IDLE:
			if(aiPerception->inSight)
			{
				if(aiPerception->LastKnownTarget())
				{
					if(g_random.Unit() < VOICE_FIRST_SEE_CHANCE)
					{
						sayComponent->PlayVoiceType(Say::VOICETYPE_FIRST_SEE_PLAYER,0.0f,1.2f,false);
					}
					tauntTimer = g_random.InRange(VOICE_BATTLE_TIME_LOW,VOICE_BATTLE_TIME_HIGH);
					m_fightState = FIGHTSTATE_FIGHTING;
				}
			}
			else if(aiPerception->GetLastHeardTime() != 0.0f)
				m_fightState = FIGHTSTATE_ALERTED;
			else if(tauntTimer == 0.0f)
			{
				sayComponent->PlayVoiceType(Say::VOICETYPE_PRECONTACT_IDLE,0.0f,0.0f,false);
				tauntTimer = g_random.InRange(VOICE_IDLE_TIME_LOW,VOICE_IDLE_TIME_HIGH);
			}
			break;
		case FIGHTSTATE_ALERTED:
			if(aiPerception->inSight)
			{
				if(g_random.Unit() < VOICE_FIRST_SEE_CHANCE)
				{
					sayComponent->PlayVoiceType(Say::VOICETYPE_FIRST_SEE_PLAYER,0.0f,0.0f,false);
				}
				tauntTimer = g_random.InRange(VOICE_BATTLE_TIME_LOW,VOICE_BATTLE_TIME_HIGH);
				m_fightState = FIGHTSTATE_FIGHTING;
			}
			break;
		case FIGHTSTATE_FIGHTING:
			if(!aiPerception->GetRecentSighting(VOICE_LOSS_CONTACT))
			{
				if(tauntTimer < VOICE_IDLE_TIME_LOW)
					sayComponent->PlayVoiceType(Say::VOICETYPE_CONTACT_LOST,0.0f,0.0f,false);
				m_fightState = FIGHTSTATE_LOSS_CONTACT;
				tauntTimer = g_random.InRange(VOICE_IDLE_TIME_LOW,VOICE_IDLE_TIME_HIGH);
			}
			else if(tauntTimer == 0.0f)
			{
				sayComponent->PlayVoiceType(Say::VOICETYPE_BATTLE_TALK,0.0f,0.0f,false);
				tauntTimer = g_random.InRange(VOICE_BATTLE_TIME_LOW,VOICE_BATTLE_TIME_HIGH);
			}
			break;
		case FIGHTSTATE_LOSS_CONTACT:
			if(aiPerception->inSight)
				m_fightState = FIGHTSTATE_FIGHTING;
			else if(tauntTimer == 0.0f)
			{
				tauntTimer = g_random.InRange(VOICE_IDLE_TIME_LOW,VOICE_IDLE_TIME_HIGH);
				m_fightState = FIGHTSTATE_ALERTED;
			}
			break;
		}
	}
}
//============================================================================
void AiBehavior::ActorInMyWay(CActor *i_actorInWay)
{
	ASSERT(i_actorInWay);

	if(i_actorInWay->IsA(CCombatant::ClassType()))
		g_messageDispatcher.SendMessageToActor("YouAreInMyWay", NULL, i_actorInWay->GetHandle(), actor.GetHandle());

	if(i_actorInWay->Parm().GetString("name"))
	{
		//send a message to the door to open if closed, or maybe close when open
		if( strcmpi (i_actorInWay->Parm().GetString("name"), "door") == 0 )
		{
			Vector3 forward = GetActor().GetBodyToWorld().GetRow2();
			g_messageDispatcher.SendMessageToActor( "AiOpenDoor", &forward, actor.GetHandle(), i_actorInWay->GetHandle());
		}
		else if( strcmpi (i_actorInWay->Parm().GetString("name"), "pedestrian") == 0 
			||	 strcmpi (i_actorInWay->Parm().GetString("name"), "bistander") == 0 )
		{
			//if you are not a gangster, don't grab hostage
			if(!isGang)
				return;

			//if you already have a hostage, or you are a hostage, don't grab one
			if((ActorHandle)actor.GetProperty( "hostage" )->GetHandle())
				return;
			if((ActorHandle)actor.GetProperty( "hostageTaker" )->GetHandle())
				return;

			//if the hostage is dead, held, or holding someone, don't take it
			CCompActor* hostage = (CCompActor*) i_actorInWay;
			ASSERT_PTR( hostage );
			ASSERT( hostage->IsA( CCompActor::ClassType() ) );
			if(hostage->GetProperty( "health" )->GetFloat() <= 0.0f)
				return;
			if((ActorHandle)hostage->GetProperty( "hostageTaker" )->GetHandle())
				return;
			if((ActorHandle)hostage->GetProperty( "hostage" )->GetHandle())
				return;
			if(hostage->Parm().GetInt("disable_hostage") == 0)
				return;

			//select a weapon that work...or don't hold hostage
			AiWeaponControl *aiWeaponControl = (AiWeaponControl *)actor.GetComponentByName( "AiWeaponControl" );
			if (aiWeaponControl)
			{
				if(!aiWeaponControl->SelectHostageWeapon(true))
					return;
			}
			else
				return ;

			g_messageDispatcher.SendMessageToActor( "HostageTaker", CMessageDispatcher::HandleToData(i_actorInWay->GetHandle()), actor.GetHandle(), actor.GetHandle() );
			g_messageDispatcher.SendMessageToActor( "Hostage", 0, actor.GetHandle(), i_actorInWay->GetHandle() );
			
			if(aiBodyControl)
				aiBodyControl->SetCrouchEnabled(false);
		}
		//else if( strcmpi (i_actorInWay->Parm().GetString("name"), "glass") == 0 )
		//{
		//	AiBodyControl *aiBodyControl = (AiBodyControl *)actor.GetComponentByName( "AiBodyControl" );
		//	if (aiBodyControl)
		//		aiBodyControl->TurnToPoint(i_actorInWay->GetFocusInWorld());

		//	AiWeaponControl *aiWeaponControl = (AiWeaponControl *)actor.GetComponentByName( "AiWeaponControl" );
		//	if (aiWeaponControl)
		//		aiWeaponControl->ForceSomeFiring(3.0f);
		//}
	}
}
void AiBehavior::BeginAfflication()
{
	switch(m_affliction)
	{
	case Afflictions::ONFIRE:
		SetDesiredState(STATE_ON_FIRE);
		break;
	case Afflictions::DEAF:
		if(sayComponent)
		{
			sayComponent->PlayVoiceType(Say::VOICETYPE_DEAFENED,0.0f,0.2f,true);
			tauntTimer = Math::Clamp(tauntTimer,VOICE_IDLE_TIME_LOW,1000.0f);
		}
		break;
	case Afflictions::STUNNED:
	case Afflictions::SHOCK:
		if(aiState != STATE_HOSTAGE)
			SetDesiredState(STATE_STUNNED);
		break;
	case Afflictions::BLIND:
	case Afflictions::TEARGAS:
	case Afflictions::INSMOKE:
		if(aiState != STATE_HOSTAGE)
			SetDesiredState(STATE_SMOKED);
		break;
	}
}
//============================================================================
//============================================================================
//============================================================================
//Message Handlers
//============================================================================
//============================================================================
//============================================================================
void AiBehavior::ManDown::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);

	//clear insight, if it was your target
	if(d_ref->aiPerception->inSight && i_sender == d_ref->aiPerception->LastKnownTarget())
		d_ref->aiPerception->ClearInsight();

	//did we just die in a multi-player game?
	if(!g_configmgr.TestOptionValue("gamesModes", "sp") && (d_ref->actor.GetHandle() == i_sender))
	{
		//let our duplicas know we're dead
		g_messageDispatcher.SendMessageToActor( "NpcDown", 0, INVALID_OBJECT_HANDLE, d_ref->actor.GetHandle() );
		d_ref->canBeHostage = false;
	}

	CActor* sender = CActor::FromHandle(i_sender);
	if (sender == NULL || sender == &d_ref->actor)
		return;

	Vector3 toSender = sender->GetBodyInWorld() - d_ref->actor.GetBodyInWorld();
	float xzdist = toSender.LengthXZSquared();

	if(g_referee.GetTeamingModel().AreFriends(CParticipantIF::GetParticipantIdFromActor(&d_ref->actor),CParticipantIF::GetParticipantIdFromActor(CActor::FromHandle(i_sender))))
	{
		if(d_ref->sayComponent)
		{
			if(xzdist > DISTXZSQRD_HELLO)
				return;
			if(d_ref->m_fightState != FIGHTSTATE_ALERTED && d_ref->m_fightState != FIGHTSTATE_FIGHTING)
				return;
			if(d_ref->desiredState == STATE_DEAD)
				return;
			switch(d_ref->aiState)
			{
			case STATE_STUNNED:
			case STATE_ON_FIRE:
			case STATE_DEAD:
			case STATE_SMOKED:
				return;
				break;
			}
			d_ref->playVoice = Say::VOICETYPE_BUDDY_DIED;
			return;
		}
	}

	//if you were near by, scream, this helps in having target switching when someone dies
	if(xzdist < DISTXZSQRD_HELLO)
		g_messageDispatcher.SendMessageToAll("DoorKicked",d_ref->actor.GetHandle(),d_ref->actor.GetHandle());

	//Avatar died stuff.........................................
   //were you the avatar?
	if(sender != g_players.GetPlayer(0))
		return;

	//if(g_referee.GetTeamingModel().IsPlayerFriend(d_ref->participantId))
	//	return;

	//remove hostility
	//if(d_ref->aiPerception)
	//	d_ref->aiPerception->hostileToAvatar = false;

	if(xzdist > DISTXZSQRD_CLOSE)
		return;

	if (d_ref->sayComponent)
	{
		d_ref->sayComponent->PlayVoiceType(Say::VOICETYPE_KILLED_PLAYER,0.0f,1.0f,false);
		d_ref->tauntTimer = Math::Clamp(d_ref->tauntTimer,VOICE_IDLE_TIME_LOW,1000.0f);
		d_ref->m_fightState = FIGHTSTATE_IDLE;
	}
}
//============================================================================
void AiBehavior::NpcDown::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	d_ref->canBeHostage = false;
}
//============================================================================
void AiBehavior::HostageTaker::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);
	if(d_ref->aiState == STATE_DEAD)
		return;
	if(d_ref->aiWeaponControl == NULL)
		return;
	//if you are releasing a hostage, you can stop using only the hostage certified weapon
	ActorHandle hostage = CMessageDispatcher::DataToHandle(i_data);
	if(hostage == INVALID_OBJECT_HANDLE)
	{
		d_ref->aiWeaponControl->SelectHostageWeapon(false);
		if(d_ref->aiBodyControl)
			d_ref->aiBodyControl->SetCrouchEnabled(false);
	}
}
//============================================================================
void AiBehavior::Hostage::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	//only someone qualifying as a hostage will get this
	ASSERT_PTR(d_ref);

	if(d_ref->aiState == STATE_DEAD)
		return;

	//who has us hostage if anybody
	if(i_sender)
		d_ref->hostageTakerHandle = i_sender;

	if (g_configmgr.TestOptionValue("gameModes", "sp") || ((CActor*)&d_ref->actor)->IsNetMaster())
	{
		if(i_sender)
		{
			d_ref->SetDesiredState(STATE_HOSTAGE);
			g_messageDispatcher.SendMessageToAll( "HostageGrabbed", NULL, d_ref->actor.GetHandle());
		}
		else
			d_ref->SetDesiredStateToHighestWant();
	}
	else
	{
		//our duplica only cares if it is a hostage or not
		if(i_sender)
			d_ref->aiState = STATE_HOSTAGE;
		else
			d_ref->aiState = STATE_DO_JOB;
	}
}
//============================================================================
void AiBehavior::HostageGrabbed::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);
	if(d_ref->aiState == STATE_DEAD)
		return;
	if(d_ref->aiWeaponControl == NULL)
		return;
	char *role = d_ref->actor.Parm().GetString( "role" );
	if( role && strcmp( role, "cop" ) == 0 )
	{
		d_ref->targetActorHandle = i_sender;
		CActor* sender = CActor::FromHandle(i_sender);
		if(i_data != NULL)
		{
			CActor* target = (CActor*) i_data;
			if(!target->IsA(CActor::ClassType()))
				return;  //make sure it is an actor
			d_ref->hostageAnchor = target->GetBodyInWorld();
			d_ref->aiPerception->AddSoundIncident(AiPerception::ALERT_SCREAM,target->GetFocusInWorld());
		}
		else if(sender)
		{
			d_ref->hostageAnchor = sender->GetBodyInWorld();
			d_ref->aiPerception->AddSoundIncident(AiPerception::ALERT_SCREAM,sender->GetFocusInWorld());
		}
		d_ref->SetDesiredState(STATE_HOLDING_FIRE);

	}
}
//============================================================================
void AiBehavior::HostageGoneFar::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);
	if(d_ref->aiState == STATE_HOLDING_FIRE)
		d_ref->SetDesiredStateToHighestWant();
}
void AiBehavior::EnemyGunfire::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);
	if(d_ref->aiState == STATE_HOLDING_FIRE)
		d_ref->SetDesiredStateToHighestWant();
}
void AiBehavior::KilledHostage::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	//if I am a cop play sound
	if(!d_ref->isGang && d_ref->sayComponent)
		d_ref->playVoice = Say::VOICETYPE_HOSTAGE_KILLED;
}
void AiBehavior::ApproveSound::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);
	bool playSound = true;

	switch(d_ref->aiState)
	{
	case STATE_STUNNED:
	case STATE_ON_FIRE:
	case STATE_DEAD:
	case STATE_COWER:
		playSound = false;
		break;
	}

	switch(d_ref->desiredState)
	{
	case STATE_STUNNED:
	case STATE_ON_FIRE:
	case STATE_DEAD:
	case STATE_COWER:
		playSound = false;
		break;
	}

	if(playSound)
		g_messageDispatcher.SendMessageToActor( "SoundApproved", NULL, d_ref->actor.GetHandle(), i_sender );
}

void AiBehavior::NpcSpawned::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);

	//non combatants don't count
	if(d_ref->aiWeaponControl == NULL)
		return;

	d_ref->npcCount++;
	if(d_ref->npcCount < MAX_NPCS)
		return;
	if(d_ref->aiPerception && d_ref->aiPerception->inSight)
		return;
	Vector3 toAvatar = g_players.GetPlayer(0)->GetBodyInWorld() - d_ref->actor.GetBodyInWorld();
	float xzdist = toAvatar.LengthXZSquared();
	if(xzdist < DISTXZSQRD_LONG)
		return;

	char* deathmessage = d_ref->actor.Parm().GetString( "ai/deathMessage" );
	if(deathmessage)
		g_messageDispatcher.SendMessageToAll( deathmessage, NULL, d_ref->actor.GetHandle());		// send out death message
	d_ref->actor.MarkForDeath();
	//dbgPrint("Too many combatants!!!!! You have %i\n",d_ref->npcCount);
}
//============================================================================
void AiBehavior::NpcDied::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);

	//non combatants don't count
	if(d_ref->aiWeaponControl == NULL)
		return;

	d_ref->npcCount--;
}
//============================================================================
void AiBehavior::startaffliction::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);
	int i_affliction = (int) i_data;

	//check to see for higher priority affltion
	switch(d_ref->m_affliction)
	{
	case Afflictions::ONFIRE:
		break;
	default:
		d_ref->m_affliction = i_affliction;
		d_ref->m_afflictionDelay = g_random.InRange(0.0f,AFFLICTION_DELAY);
	}
}
//============================================================================
void AiBehavior::endaffliction::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);
	int affliction = (int) i_data;

	if((int)affliction != d_ref->m_affliction)
		return; //can't clean up what you aren't already in

	d_ref->SetDesiredStateToHighestWant();
}
//============================================================================
void AiBehavior::KillInside::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);
	//check to see if the sender and myself would collide, if so, kill me

	CActor* sender = CActor::FromHandle(i_sender);
	if(sender && sender->GetAABB().Contains(d_ref->actor.GetAABB()))
		d_ref->actor.MarkForDeath();
}
//============================================================================
void AiBehavior::RemoveFromGame::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);
	d_ref->actor.MarkForDeath();
}
//============================================================================
void AiBehavior::NPCTimeout::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);
	d_ref->timeout = true;
	if(d_ref->aiNavigation)
		d_ref->aiNavigation->StopMoving();
}
//============================================================================
void AiBehavior::NPCTimein::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);
	d_ref->timeout = false;
}
//============================================================================
void AiBehavior::VaultTrigger::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	if(d_ref->aiNavigation == NULL)
		return;

	if(d_ref->aiState != STATE_FIGHT)
		return;

	if(d_ref->vaultTimer > 0.0f)
		return;

	/*
	//prevent vaulting backwards from direction of travel
	Vector3 toWaypoint = d_ref->aiNavigation->GetWaypoint() - d_ref->actor.GetBodyInWorld();
	toWaypoint.y(0.0f);
	toWaypoint.SafeNormalize();
	CActor* sender = CActor::FromHandle(i_sender);
	Vector3 toMe = d_ref->actor.GetBodyInWorld() - sender->GetBodyInWorld();
	toMe.y(0.0f);
	toMe.SafeNormalize();
	Vector3 senderDir = sender->GetBodyToWorld().GetRow2();
   float dot = toMe.Dot(senderDir);
	if(dot < 0.0f)
		senderDir *= -1.0f; //flip to be right
	dot = toWaypoint.Dot(senderDir);
  	if(dot > 0.0f) //this means they are both going the same direction...which is away from the vault object
		return;
		*/

	g_messageDispatcher.SendMessageToActor( "TriggeredVault", NULL, i_sender, d_ref->actor.GetHandle() );
	d_ref->SetDesiredState(STATE_VAULTING);
}
//============================================================================
void AiBehavior::JumpHere::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	CActor* sender = CActor::FromHandle(i_sender);
	if(d_ref->aiNavigation == NULL || sender == NULL)
		return;

	//unless you are going toward the center, you must be going away
	Vector3 toSender = sender->GetBodyInWorld() - d_ref->actor.GetBodyInWorld();
	toSender.y(0.0f);
	toSender.SafeNormalize();
	Vector3 toWaypoint = d_ref->aiNavigation->GetWaypoint() - d_ref->actor.GetBodyInWorld();
	toWaypoint.y(0.0f);
	toWaypoint.SafeNormalize();
	if(toWaypoint.Dot(toSender) < 0.0f)
	return;

	g_messageDispatcher.SendMessageToActor( "Jump", NULL, d_ref->actor.GetHandle(), d_ref->actor.GetHandle() );
}
//============================================================================
void AiBehavior::Trespass::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	d_ref->stateTimer = 0.0f; //change state soon
	if(d_ref->aiPsyche)
		d_ref->aiPsyche->StimulusInput(AiPsyche::STIMULUS_PAIN, 1.0f);
}
//============================================================================
void AiBehavior::PlayerSpawning::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	if(d_ref->aiState == STATE_DEAD || d_ref->desiredState == STATE_DEAD)
		return;

	if(g_referee.GetTeamingModel().IsPlayerEnemy(d_ref->participantId))
		d_ref->Respawn();
}
//============================================================================
void AiBehavior::Respawn()
{
	if (actor.IsA(CCombatant::ClassType()))
		((CCombatant*)(&actor))->Respawn();

	if ( pStats )				pStats->Reset();
	if ( aiPsyche )			aiPsyche->Reset();
	if ( aiNavigation )		aiNavigation->Reset();
	if ( aiPerception )		aiPerception->Reset();
	if ( aiWeaponControl )	aiWeaponControl->Reset();

	Reset();

	KinematicController* controller =
		(KinematicController*)actor.GetActiveComponentInClass( "KinematicController" );
	if( controller )
		controller->Reset();
}
//============================================================================

void AiBehavior::StartCinematic::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	d_ref->tauntTimer = 1000.0f;
}
//============================================================================
void AiBehavior::EndCinematic::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	d_ref->tauntTimer = g_random.InRange(VOICE_IDLE_TIME_LOW,VOICE_IDLE_TIME_HIGH);
}
//============================================================================
void AiBehavior::HostageSaved::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	if(d_ref->isGang)
		return;

	d_ref->playVoice = Say::VOICETYPE_HOSTAGE_SAVED;
}
//============================================================================
void AiBehavior::Perform::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	if(d_ref->actor.Parm().GetInt("evacuator") == 1)
	{
		d_ref->playVoice = Say::VOICETYPE_BATTLE_TALK;

		if(d_ref->pathFollower)
		{
			if(d_ref->pathFollower->GetSpeedCurrent() < 0.0f)
				d_ref->pathFollower->SetSpeedTarget(d_ref->normalPathSpeed * -1.0f);
			else
				d_ref->pathFollower->SetSpeedTarget(d_ref->normalPathSpeed);
			d_ref->pathFollower->Activate();
		}
	}
}
//============================================================================
void AiBehavior::CheckpointSet::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	if(d_ref->aiPsyche == NULL)
		d_ref->aiPsyche = (AiPsyche *)d_ref->actor.GetComponentByName( "AiPsyche" );

	if(d_ref->aiPsyche)
	{
		if(d_ref->aiPsyche->GetAiType() != AiPsyche::AI_BUDDY)
			return;

		//save his position
		if(d_ref->aiNavigation)
			d_ref->aiNavigation->SetStartPosition(d_ref->actor.GetBodyInWorld());
	}
}
//============================================================================
void AiBehavior::CheckpointReset::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	if(d_ref->aiPsyche == NULL)
		d_ref->aiPsyche = (AiPsyche *)d_ref->actor.GetComponentByName( "AiPsyche" );

	if(d_ref->aiPsyche)
	{
		if(d_ref->aiPsyche->GetAiType() != AiPsyche::AI_BUDDY)
			return;

		//place him at saved position
		if(d_ref->aiNavigation)
			d_ref->aiNavigation->ResetToStartPosition();
	}
}
//============================================================================
void AiBehavior::PathfollowerStop()
{
	if(pathFollower == NULL)
		return;

	aiPsyche->SetWant(AiPsyche::WANT_TO_FLEE,0.0f);
	aiPsyche->SetWant(AiPsyche::WANT_TO_COWER,1000.0f);

	pathFollower->SetState((uint8)STATE_COWER);
	if(actor.Parm().GetInt("evacuator") == 1)
	{
		pathFollower->SetSpeedTarget(0.0f);
		pathFollower->AdjustSpeed(0.0f);
		pathFollower->SetFaceMovementDirection(false);
		pathFollower->Deactivate();
	}
}
//============================================================================
//============================================================================
