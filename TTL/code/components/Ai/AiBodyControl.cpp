#include "TTLPCH.h"
#include "components/Ai/AiBodyControl.h"
#include "Components/ComponentCreator.h"
#include "components/KineControl/BipedKineControl.h"
#include "components/KineControl/BipedModel.h"
#include "components/Ai/AiWeaponControl.h"
#include "data_h_files/players/players_s.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(AiBodyControl, AiBodyControl, COMPONENT_NEUTRAL_PRIORITY);

#define SCAN_PAUSE_TIME 2.2f
#define START_SCANNING_TIME_LOW 8.0f
#define START_SCANNING_TIME_HIGH 10.0f
//scared me crouch stuff
#define CROUCH_TIME_MIN 1.0f
#define CROUCH_TIME_MAX 5.0f
#define CROUCH_INTERLUDE_TIME_MIN 1.0f
#define CROUCH_INTERLUDE_TIME_MAX 17.0f
#define CROUCH_FOR_A_LONG_TIME 1000.0f
#define CROUCH_MOVING_TIME 0.1f
//standing cover stuff

#define CROUCH_TIME_MIN_STANDCOVER 1.0f
#define CROUCH_TIME_MAX_STANDCOVER 3.0f
#define CROUCH_INTERLUDE_TIME_MIN_STANDCOVER 2.0f
#define CROUCH_INTERLUDE_TIME_MAX_STANDCOVER 4.0f
#define LOOK_AT_TIME_MIN 0.5f
#define LOOK_AT_TIME_MAX 1.0f
#define LONG_SCAN_FACTOR 2.5f

//#define DRAW_LASTKNOWN

//============================================================================
AiBodyControl::AiBodyControl( CCompActor &i_actor ) :
CActorComponent( i_actor )
{
}

//============================================================================
AiBodyControl::~AiBodyControl()
{
}

//============================================================================
void AiBodyControl::Initialize( void )
{
	d_enemyGunfireHandler.Initialize("EnemyGunfire",
		MESSAGE_HANDLE_IF_MASTER | MESSAGE_HANDLE_IF_DUPLICA |
		MESSAGE_ROUTE_TO_NEIGHBORS, actor.GetMessageOwner(), this);
	moveIF = (CMoveIF *)actor.GetInterface( CMoveIF::GetID() );
	ASSERT( moveIF );
	viewIF = (ViewIF *)actor.GetInterface( ViewIF::GetID() );
	ASSERT( viewIF );
	pStats = (PlayerStats*)actor.GetInterface( PlayerStats::GetID() );
	aiPerception = (AiPerception*)( actor.GetComponentByName( "AiPerception" ) );
	aiNavigation = (AiNavigation*)( actor.GetComponentByName( "AiNavigation" ) );
	Reset();
}

//============================================================================
void AiBodyControl::Reset( void )
{
	lookPosition.Clear();
	lastKnownTargetLocation.Clear();
	scanLeft = 1.0f;
	scanMax = g_random.InRange(0.4f,0.7f);
	scanSpeed = g_random.InRange(0.005f,0.02f);
	scanSwitchReady = true;
	scanPauseTimer = 0.0f;
	scanLengthTimer = 0.0f;
	crouchEnabled = false;
	crouching = false;
	crouchTimer = g_random.InRange(CROUCH_TIME_MIN,CROUCH_TIME_MAX);
	crouchInterludeTimer = g_random.InRange(CROUCH_INTERLUDE_TIME_MIN,CROUCH_INTERLUDE_TIME_MAX);
	crouchType = CROUCH_DEFAULT;
	bodyControlState = BODYCONTROL_DISABLED;
	lookAtTimer = 0.0f;
	lastLookAtPos.Clear();
	lookType = LOOK_UNLOCKED;
	startScanningTime = g_random.InRange(START_SCANNING_TIME_LOW,START_SCANNING_TIME_HIGH);
	stationaryCover = 0;
	if(actor.Parm().GetInt("stat_cover") == 1)
		stationaryCover = 1;
	else if(actor.Parm().GetInt("stat_cover") == 2)
		stationaryCover = 2;
	timeAlive = 0.0f;
	crouchNextUpdate = false;
}
//============================================================================
void AiBodyControl::Update(float frameSec)
{
	timeAlive += g_timer.GetFrameSec();

	switch(bodyControlState)
	{
	case BODYCONTROL_DISABLED:
		break;
	case BODYCONTROL_AMBIENT:
		{
			if (pStats && !pStats->IsAlive())
				return;
			UpdateLookPosition(frameSec);
		}
		break;
	case BODYCONTROL_COMBAT:
		{
			if (pStats && !pStats->IsAlive())
				return;
			UpdateLookPosition(frameSec);
			UpdateCrouchPosition(frameSec);
		}
		break;
	}

   //Vector3 dir, eye;
	//viewIF->GetDirection(dir);
	//dir *= 10.0f;
	//viewIF->GetEyepoint(eye);
	//dir += eye;
	//DrawUtility::Line(eye,dir,DrawUtility::RED);
	////dbgPrint("look dir %f %f %f\n",dir.x(),dir.y(),dir.z());
	////dbgPrint("look eye %f %f %f\n",eye.x(),eye.y(),eye.z());
}
//============================================================================
void AiBodyControl::SetBodyControlState(int state)
{
	bodyControlState = state;

	switch(bodyControlState)
	{
	case BODYCONTROL_DISABLED:
		viewIF->UnlockView();
		break;
	}
}
//============================================================================
void AiBodyControl::TurnToPoint(Vector3 location)
{
	moveIF->CommandTurnToPoint( actor, location );
}
//============================================================================
void AiBodyControl::UpdateLookPosition(float frameSec)
{
	bool contact = aiPerception->GetLastKnownLocation(lastKnownTargetLocation);
	bool startQuickScan = false;
	bool stopQuickScan = false;
	bool startLookScan = false;
	bool stopLookScan = false;
	bool tryAgain = true;
	Math::MoveToZero(&lookAtTimer,frameSec);

#ifdef DRAW_LASTKNOWN
	DrawUtility::Line(actor.GetFocusInWorld(),lastKnownTargetLocation,DrawUtility::WHITE_ALPHA);
	if(aiPerception->inSight)
		DrawUtility::Sphere(lastKnownTargetLocation,DrawUtility::GREEN, 0.25f);
	else
		DrawUtility::Sphere(lastKnownTargetLocation,DrawUtility::RED, 0.25f);
#endif

	//Target is in sight
	if(aiPerception->inSight)
	{
		//clear up any scanning animations
		if(lookType == LOOK_SCANNING)
			stopLookScan = true;
		else if(lookType == LOOK_SCAN_QUICK)
			stopQuickScan = true;

		CActor* target = NULL;
		if(aiPerception->LastKnownTarget() != INVALID_OBJECT_HANDLE)
			target = CActor::FromHandle(aiPerception->LastKnownTarget());

		if(target)
		{
			//look at target, if you aren't moving, TurnTo as well
			if(aiNavigation && aiNavigation->GetState() == AiNavigation::NAVSTATE_MOVING)
				LookAtLastKnownPosition(false,true);
			else
				LookAtLastKnownPosition(true,true);
		}
		lookType = LOOK_INSIGHT;
		tryAgain = false;
	}

	//moving npc, target not insight
	if(tryAgain && aiNavigation && aiNavigation->GetState() == AiNavigation::NAVSTATE_MOVING)
	{
		//clear scanning animations
		if(lookType == LOOK_SCANNING)
			stopLookScan = true;
		else if(lookType == LOOK_SCAN_QUICK)
			stopQuickScan = true;

		if(contact && aiPerception->GetRecentSighting(30.0f))
		{
			LookAtLastKnownPosition(false);
			lookType = LOOK_LASTKNOWN;
			return;
		}
		else
		{
			viewIF->UnlockView();
			lookType = LOOK_UNLOCKED;
			return;
		}
	}

	//if standing around
	if(tryAgain && (aiNavigation == NULL || aiNavigation->GetState() == AiNavigation::NAVSTATE_NEUTRAL || aiNavigation->GetState() == AiNavigation::NAVSTATE_STARTING_OFF ))
	{
		float contactTime = aiPerception->GetLastSeenOrHeardTime();
		float timedif = g_timer.GetStartSec() - contactTime;
		if((contactTime == 0.0f && timeAlive > startScanningTime) || (contactTime != 0.0f && timedif > startScanningTime))
		{
			if(timedif > startScanningTime * LONG_SCAN_FACTOR)
			{
				if(lookType == LOOK_SCAN_QUICK)
					stopQuickScan = true;
				if(lookType != LOOK_SCANNING)
					startLookScan = true;
				lookType = LOOK_SCANNING;
			}
			else
			{
				if(lookType == LOOK_SCANNING)
					stopLookScan = true;
				if(lookType != LOOK_SCAN_QUICK)
					startQuickScan = true;
				lookType = LOOK_SCAN_QUICK;
			}
		}
		else if(contact)
		{
			if(lookType == LOOK_SCANNING)
			{
				stopLookScan = true;
				startScanningTime = g_random.InRange(START_SCANNING_TIME_LOW,START_SCANNING_TIME_HIGH);
			}
			else if(lookType == LOOK_SCAN_QUICK)
			{
				stopQuickScan = true;
				startScanningTime = g_random.InRange(START_SCANNING_TIME_LOW,START_SCANNING_TIME_HIGH);
			}
			LookAtLastKnownPosition();
			lookType = LOOK_LASTKNOWN;
		}
	}

	//don't do stuff while you have hostage
	if((ActorHandle)actor.GetProperty( "hostage" )->GetHandle())
		return;

	if(stopQuickScan)
		g_messageDispatcher.SendMessageToActor( "StopQuickScan", NULL, actor.GetHandle(), actor.GetHandle() );
	if(stopLookScan)
		g_messageDispatcher.SendMessageToActor( "StopLookScan", NULL, actor.GetHandle(), actor.GetHandle() );
	if(startQuickScan)
		g_messageDispatcher.SendMessageToActor( "StartQuickScan", NULL, actor.GetHandle(), actor.GetHandle() );
	if(startLookScan)
		g_messageDispatcher.SendMessageToActor( "StartLookScan", NULL, actor.GetHandle(), actor.GetHandle() );
}
//============================================================================
void AiBodyControl::LookAtPosition(Vector3 position, bool turnTo, bool force)
{
	if(actor.GetBodyInWorld().x() == position.x()
		&& actor.GetBodyInWorld().z() == position.z())
		return;

	if(position.LengthSquared() == 0.0f)
		return; //don't look at origin

	if(force)
		lastLookAtPos = position;
	else if(lookAtTimer == 0.0f)
	{
		lastLookAtPos = position;
		lookAtTimer = g_random.InRange(LOOK_AT_TIME_MIN,LOOK_AT_TIME_MAX);
	}

	Vector3 up; up.Set(0.0f, 1.0f, 0.0f);
	viewIF->LookAt(lastLookAtPos,up);
	viewIF->LockView();

	if(turnTo)
		TurnToPoint(lastLookAtPos);
}
//============================================================================
void AiBodyControl::LookAtLastKnownPosition(bool turnTo, bool force)
{
	//use last sighted position unless there is a large time or distance difference

	if(aiPerception->GetLastKnownLocation(lastKnownTargetLocation))
		LookAtPosition(lastKnownTargetLocation,turnTo,force);
}
//============================================================================
void AiBodyControl::LookAtActor(CActor *pActor, bool turnTo, bool force)
{
	LookAtPosition(pActor->GetFocusInWorld(),turnTo,force);
}
//============================================================================
void AiBodyControl::LookAtScanning(float speed, bool force)
{
	if(!force)
	{
		//intermitent pauses
		if(scanPauseTimer == 0.0f)
		{
			if(g_random.Unit() < 0.03f)
				scanPauseTimer = (g_random.Unit() * SCAN_PAUSE_TIME) + 0.2f;
		}
		else if(scanLengthTimer == 0.0f)
			return;
	}

	Vector3 lookingDir;
	viewIF->GetDirection(lookingDir);
	Matrix3x3 rot;
	rot.Identity();
	rot.CatYRotation(scanSpeed * scanLeft * speed);
	lookingDir = lookingDir * rot;
	Vector3 up;
	up.Set(0.0f,1.0f,0.0f);
	viewIF->LookDirection(lookingDir, up);
	viewIF->LockView();

	if(scanSwitchReady && lookingDir.Dot(actor.GetBodyToWorld().GetRow2()) < scanMax)
	{
		scanLeft *= -1.0f;
		scanMax = g_random.InRange(0.1f,0.6f);
		scanSpeed = g_random.InRange(0.02f,0.04f);
		scanSwitchReady = false;
	}
	else if
		(
		(scanLeft == 1.0f && lookingDir.Dot(actor.GetBodyToWorld().GetRow0()) > 0.0f)
		||
		(scanLeft == -1.0f && lookingDir.Dot(actor.GetBodyToWorld().GetRow0()) < 0.0f)
		)
			scanSwitchReady = true;
}
//============================================================================
void AiBodyControl::UpdateCrouchPosition(float frameSec)
{
	if(crouchNextUpdate)
	{
      Crouch(CROUCH_SCARED, g_random.InRange(CROUCH_TIME_MIN,CROUCH_TIME_MAX));
		crouchNextUpdate = false;
		return;
	}

	//if I am a navigator, do this stuff
	if(aiNavigation)
	{
		if(aiNavigation->GetState() == AiNavigation::NAVSTATE_MOVING)
			crouchTimer = Math::Clamp(crouchTimer,0.0f,CROUCH_MOVING_TIME);
		else
		{
			//if they are crouching due to nav crouch cover, make sure still valid cover
			bool checkIfValid = false;
			if(crouching && (crouchType == CROUCH_COVER_HALF || crouchType == CROUCH_COVER_STANDING))
				checkIfValid = true;

			//if I am not crouching and navcover thinks I should probably crouch
			if(!crouching && aiNavigation->GetCoverState() == AiNavigation::NAVCOVER_PROCESS_SUCCESS)
				checkIfValid = true;

			if(checkIfValid)
			{
				//let's see if you are close to your cover position
				Vector3 toCover = aiNavigation->GetGoalPosition() - actor.GetBodyInWorld();
				float xzsq = toCover.LengthXZSquared();
				if(xzsq < 2.0f)
				{
					//let's see if you are looking over your crouch cover
					Vector3 lookingDir;
					viewIF->GetDirection(lookingDir);
					Vector3 coverDir = aiNavigation->GetCoverDir();
					coverDir.y(lookingDir.y());
					if(lookingDir.Dot(coverDir) < 0.7f)
						crouchTimer = 0.0f; //not looking over cover, stop covering
					else
					{
						//if not crouching, navcover crouch must want me to crouch
						if(!crouching)
						{
							if(aiNavigation->GetCoverType() == Navigate::TreeNode::CROUCHCOVER_EDGE)
								Crouch(CROUCH_COVER_HALF, CROUCH_FOR_A_LONG_TIME);
							else if(aiNavigation->GetCoverType() == Navigate::TreeNode::STANDCOVER_EDGE)
							{
								if(crouchInterludeTimer == 0.0f && aiPerception->inSight)
								{
									//stop firing
									AiWeaponControl* aiWeaponControl = (AiWeaponControl*) actor.GetComponentByName( "AiWeaponControl" );
									if(aiWeaponControl)
										aiWeaponControl->CeaseFire();
									Crouch(CROUCH_COVER_STANDING, g_random.InRange(CROUCH_TIME_MIN_STANDCOVER,CROUCH_TIME_MAX_STANDCOVER));
								}
							}
						}
					}
				}
				else
					crouchTimer = 0.0f; //to far from cover, stop covering
			}
		}
	}
	else
	{
		if(stationaryCover == 1 && !crouching && crouchInterludeTimer == 0.0f && aiPerception->inSight)
			Crouch(CROUCH_COVER_STANDING, g_random.InRange(CROUCH_TIME_MIN_STANDCOVER,CROUCH_TIME_MAX_STANDCOVER));
		else if(stationaryCover == 2 && !crouching) //crouches forever
			Crouch(CROUCH_TO_AIM,CROUCH_FOR_A_LONG_TIME);
	}
	//clean up, uncrouch when appropriate
	Math::MoveToZero(&crouchInterludeTimer,frameSec);
	if(crouchTimer > 0.0f)
		Math::MoveToZero(&crouchTimer,frameSec);
	else if(crouching)
		Uncrouch();
}
//============================================================================
void AiBodyControl::Crouch(int type, float time)
{
	crouchType = type;
	if(crouchEnabled)
	{
		if(!crouching)
		{
			if(type == CROUCH_COVER_STANDING)
				g_messageDispatcher.SendMessageToActor( "StartSuperCrouch", NULL, actor.GetHandle(), actor.GetHandle() );
			else
         	moveIF->Crouch(true);
		}
		crouching = true;
		crouchTimer = time;
	}
}
//============================================================================
void AiBodyControl::Uncrouch()
{
	if(crouching)
	{
		if(crouchType == CROUCH_COVER_STANDING)
		{
			g_messageDispatcher.SendMessageToActor( "StopSuperCrouch", NULL, actor.GetHandle(), actor.GetHandle() );
			crouchInterludeTimer = g_random.InRange(CROUCH_INTERLUDE_TIME_MIN_STANDCOVER,CROUCH_INTERLUDE_TIME_MAX_STANDCOVER);
		}
		else
		{
			moveIF->Crouch(false);
			crouchInterludeTimer = g_random.InRange(CROUCH_INTERLUDE_TIME_MIN,CROUCH_INTERLUDE_TIME_MAX);
		}
		crouching = false;
		crouchTimer = 0.0f;
	}
}
//============================================================================
void AiBodyControl::SetCrouchEnabled(bool canCrouch)
{
	if(canCrouch == crouchEnabled)
		return;
	crouchEnabled = canCrouch;
	if(!canCrouch && crouching)
		Uncrouch();
}
//============================================================================
void AiBodyControl::InroAnim()
{
	if(actor.AnimAction() != ACTION_ROAM_IDLE)
		return;

	AiWeaponControl *aiWeaponControl = (AiWeaponControl*) actor.GetComponentByName( "AiWeaponControl" );
	if(aiWeaponControl == NULL)
		return;

	char *role = actor.Parm().GetString( "role" );
	if( role && strcmp( role, "cop" ) == 0 )
	{
		int intro = g_random.InRange(0,4);
		switch(intro)
		{
		case 0:
			g_messageDispatcher.SendMessageToActor("FullBodyAnimation", CMessageDispatcher::CharToData("G_INTRO_WIPEHEAD"), actor.GetHandle(), actor.GetHandle());
			break;
		case 1:
			g_messageDispatcher.SendMessageToActor("FullBodyAnimation", CMessageDispatcher::CharToData("G_INTRO_SCRATCH"), actor.GetHandle(), actor.GetHandle());
			break;
		case 2:
			g_messageDispatcher.SendMessageToActor("FullBodyAnimation", CMessageDispatcher::CharToData("P_INTRO_SCRATCHHEAD"), actor.GetHandle(), actor.GetHandle());
			break;
		case 3:
			g_messageDispatcher.SendMessageToActor("FullBodyAnimation", CMessageDispatcher::CharToData("P_INTRO_SCRATCHSTOMACH"), actor.GetHandle(), actor.GetHandle());
			break;
		case 4:
			g_messageDispatcher.SendMessageToActor("FullBodyAnimation", CMessageDispatcher::CharToData("P_INTRO_MIC"), actor.GetHandle(), actor.GetHandle());
			break;
		default:
			ASSERT(false);
			break;
		}
	}
	else if( role && strcmp( role, "gang" ) == 0 )
	{
		int intro = g_random.InRange(0,3);
		switch(intro)
		{
		case 0:
			g_messageDispatcher.SendMessageToActor("FullBodyAnimation", CMessageDispatcher::CharToData("G_INTRO_WIPEHEAD"), actor.GetHandle(), actor.GetHandle());
			break;
		case 1:
			g_messageDispatcher.SendMessageToActor("FullBodyAnimation", CMessageDispatcher::CharToData("G_INTRO_SCRATCH"), actor.GetHandle(), actor.GetHandle());
			break;
		case 2:
			g_messageDispatcher.SendMessageToActor("FullBodyAnimation", CMessageDispatcher::CharToData("P_INTRO_SCRATCHHEAD"), actor.GetHandle(), actor.GetHandle());
			break;
		case 3:
			g_messageDispatcher.SendMessageToActor("FullBodyAnimation", CMessageDispatcher::CharToData("P_INTRO_SCRATCHSTOMACH"), actor.GetHandle(), actor.GetHandle());
			break;
		default:
			ASSERT(false);
			break;
		}
	}

	timeAlive = 0.0f;
}
//============================================================================
//============================================================================
void AiBodyControl::EnemyGunfire::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	//only navmesh guys
	if(d_ref->aiNavigation == NULL)
		return;

	if (d_ref->pStats && !d_ref->pStats->IsAlive())
		return;

	if(!d_ref->crouching
		&& !d_ref->aiPerception->inSight
		&& d_ref->crouchInterludeTimer == 0.0f)
		d_ref->crouchNextUpdate = true;
}
//============================================================================
int AiBodyControl::PeekToSee(int peekType, int oldPeekType)
{
	if(!crouchEnabled)
		return oldPeekType;

	if(aiNavigation && aiNavigation->GetState() != AiNavigation::NAVSTATE_NEUTRAL)
		peekType = AiPerception::PEEK_NONE;

	switch(peekType)
	{
	case AiPerception::PEEK_NONE:
		if(oldPeekType == AiPerception::PEEK_RIGHT)
			g_messageDispatcher.SendMessageToActor( "StopPeekRight", NULL, actor.GetHandle(), actor.GetHandle() );
		else if(oldPeekType == AiPerception::PEEK_LEFT)
			g_messageDispatcher.SendMessageToActor( "StopPeekLeft", NULL, actor.GetHandle(), actor.GetHandle() );
		Uncrouch();
		break;
	case AiPerception::PEEK_LEFT:
		g_messageDispatcher.SendMessageToActor( "PeekLeft", NULL, actor.GetHandle(), actor.GetHandle() );
		Uncrouch();
		break;
	case AiPerception::PEEK_RIGHT:
		g_messageDispatcher.SendMessageToActor( "PeekRight", NULL, actor.GetHandle(), actor.GetHandle() );
		Uncrouch();
		break;
	case AiPerception::PEEK_CROUCH:
		if(oldPeekType == AiPerception::PEEK_RIGHT)
			g_messageDispatcher.SendMessageToActor( "StopPeekRight", NULL, actor.GetHandle(), actor.GetHandle() );
		else if(oldPeekType == AiPerception::PEEK_LEFT)
			g_messageDispatcher.SendMessageToActor( "StopPeekLeft", NULL, actor.GetHandle(), actor.GetHandle() );
		Crouch(CROUCH_TO_AIM,CROUCH_FOR_A_LONG_TIME);
		break;
	}

	return peekType;
}
//============================================================================
//============================================================================
//============================================================================
//============================================================================
//============================================================================
//============================================================================
//============================================================================
