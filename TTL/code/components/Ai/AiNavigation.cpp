#include "TTLPCH.h"
#include "components/Ai/AiNavigation.h"
#include "Components/ComponentCreator.h"
#include "components/KineControl/BipedKineControl.h"
#include "components/KineControl/BipedModel.h"
#include "Navigate/NavManager.h"
#include "Navigate/NavPath.h"
#include "components/Ai/AiWeaponControl.h"
#include "frontend/savegame.h"
#include "components/Ai/AiBodyControl.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(AiNavigation, AiNavigation, COMPONENT_NEUTRAL_PRIORITY);

#define NAVSPEED_STOP 0.0f
#define NAVSPEED_CREEP 1.0f
#define NAVSPEED_WALK 2.0f
#define NAVSPEED_FASTWALK 3.0f
#define NAVSPEED_JOG 3.9f
#define NAVSPEED_TROT 5.0f
#define NAVSPEED_RUN 6.5f

#define GOAL_CHECK_TIME_MELEE 0.4f
#define GOAL_CHECK_TIME_DODGY 2.0f
#define GOAL_CHECK_TIME_NADE 1.0f
#define GOAL_CHECK_DISTANCEXZ2 2.0f
#define WAYPOINT_CHECK_DISTANCEXZ2 0.1f
#define WANDER_CHECK_DISTANCEXZ2 5.0f
#define GLANCE_TIME_BASE 3.0f
#define START_SEARCH_TIME 8.0f
#define STOP_DODGING_TIME 1.5f
#define FIRST_SEEN_WAYPOINT_MAX 6
#define WANDER_TIME 25.0f
#define RUN_FROM_NADE_XZ_SQRD_DIST_MAX 40.0f
#define RUN_FROM_NADE_FACTOR 6.0f
#define PUSHED_BY_AVATAR_MIN 2.0f
#define PUSHED_BY_AVATAR_MAX 5.0f
#define RETREAT_DISTANCE 8.0f
#define POINT_ON_MESH_Y_MIN 1.5f
#define POINT_ON_MESH_Y_MAX 30.0f
#define STILL_TIME_THRESHOLD 2.0f
#define MOVE_TO_SPACING 6.0f
#define COVER_MOVE_UP_DISTXZSQRD 50.0f
#define COVER_MOVE_UP_SCALAR 0.5f
#define COVER_MOVE_UP_TIME 4.0f

#define DISTANCEXZSQRD_VIRTUALY_THERE 1.0f
#define DISTANCEXZSQRD_TINY 3.0f
#define DISTANCEXZSQRD_CLOSE 30.0f
#define DISTANCEXZSQRD_NEAR 150.0f
#define DISTANCEXZSQRD_HELLO 250.0f
#define DISTANCEXZSQRD_MEDIUM 800.0f
#define DISTANCEXZSQRD_LONG 2000.0f
#define DISTANCEXZSQRD_FAR 4000.0f
#define DISTANCEXZSQRD_WOW 8000.0f

#define GOAL_CHECK_TIME_DEFAULT 1.2f
#define GOAL_CHECK_TIME_MOVETO_CONTACT 1.2f
#define GOAL_CHECK_TIME_MOVETO_NOCONTACT 3.0f

//#define DRAW_DEBUGGIN_LINES_BEOTCH

//============================================================================
AiNavigation::AiNavigation( CCompActor &i_actor ) :
CActorComponent( i_actor )
{
}

//============================================================================
AiNavigation::~AiNavigation()
{
	if(pNavPath)
	{
		if(ValidSingleton(Navigate::Manager))
		{
			//pNavPath->ReleaseConnection(pPathConnection);
			gbl_NavManager.DestroyPath( pNavPath );
		}
		pNavPath = NULL;
	}
	pPathSegment = NULL;
	pNavCovIter = NULL;
	//pPathConnection = NULL;
}

//============================================================================
void AiNavigation::Initialize( void )
{
	pNavMesh = NULL;
	pNavPath = NULL;
	pPathSegment = NULL;
	pNavCovIter = NULL;
	//pPathConnection = NULL;

	aiPerception = (AiPerception*)( actor.GetComponentByName( "AiPerception" ) );
	ASSERT(aiPerception);
	moveIF = (CMoveIF *)actor.GetInterface( CMoveIF::GetID() );
	ASSERT( moveIF );
	viewIF = (ViewIF *)actor.GetInterface( ViewIF::GetID() );
	ASSERT( viewIF );
	startPosition = actor.GetBodyInWorld();
	startHeading = Math::ArcTan2(actor.GetBodyToWorld().e20,actor.GetBodyToWorld().e22);

	maxFirstWays = actor.Parm().GetInt("max_first_ways");

	d_navigationBeaconHandler.Initialize("NavigationBeacon",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);

	d_createHomePointHandler.Initialize("CreateHomePoint",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);

	d_youAreInMyWayHandler.Initialize("YouAreInMyWay",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);

	Reset();
}

//============================================================================
void AiNavigation::Reset( void )
{
	navType = NAVTYPE_DORMANT;
	navState = NAVSTATE_NEUTRAL;
	navMode = NAVMODE_STATIONARY;
	lastNavMode = navMode;
	navSpeed = NAVSPEED_JOG;
	navCoverState = NAVCOVER_NOT_IN_USE;
	navCoverDir.Set(1.0f,1.0f,0.0f);
	goalPosition = startPosition;
	lastGoalPosition = goalPosition;
	desiredGoal.Clear();
	lastKnownTargetPos.Clear();
	goalCheckTimerSetting = g_random.InRange(GOAL_CHECK_TIME_DEFAULT, GOAL_CHECK_TIME_DEFAULT + 1.0f);
	goalCheckTimer = goalCheckTimerSetting;
	returnToStartTimer = 0.0f;
	wanderTimer = g_random.InRange(WANDER_TIME,WANDER_TIME * 2.0f);
	wanderCenter = startPosition;
	waypointCur = 0;
	waypointCount = 0;
	waypoint = actor.GetBodyInWorld();
	if(NavMesh_SetupMesh())
		navMeshState = NAVMESH_UNSTARTED;
	else
		navMeshState = NAVMESH_NO_MESH_FOUND;

	actor.Place(startPosition, startHeading);
	g_messageDispatcher.SendMessageToAll("ShoutOutToBeacons",NULL,actor.GetHandle());
	navCoverType = Navigate::TreeNode::NOCOVER_EDGE;

	stillTime = 0.0f;
	pauseTime = 0.0f;
	beaconHandle = INVALID_OBJECT_HANDLE;
	savedLookAt.Clear();
	freeUseTimer = 0.0f;
}
//============================================================================
void AiNavigation::Update(float frameSec)
{
	//DrawUtility::Line(actor.GetBodyInWorld(),desiredGoal,DrawUtility::RED);
	//DrawUtility::Line(actor.GetBodyInWorld(),goalPosition,DrawUtility::WHITE);
#ifdef DRAW_DEBUGGIN_LINES_BEOTCH
	DrawUtility::Line(actor.GetBodyInWorld(),waypoint,DrawUtility::YELLOW);
	DrawUtility::Sphere(waypoint,DrawUtility::YELLOW, 0.15f);
	Vector3 raisedSpot = desiredGoal;
	raisedSpot.y(raisedSpot.y() + 0.5f);
	DrawUtility::Sphere(savedGoalPosition,DrawUtility::GREEN_ALPHA, 0.25f);
	switch(navMode)
	{
	case NAVMODE_MOVE_TO_TARGET:
		DrawUtility::Line(actor.GetBodyInWorld(),raisedSpot,DrawUtility::GREEN);
		DrawUtility::Sphere(raisedSpot,DrawUtility::BLUE, 0.15f);
		break;
	case NAVMODE_MELEE:
		DrawUtility::Line(actor.GetBodyInWorld(),raisedSpot,DrawUtility::CYAN);
		DrawUtility::Sphere(raisedSpot,DrawUtility::CYAN, 0.15f);
		break;
	case NAVMODE_RETREAT:
		DrawUtility::Line(actor.GetBodyInWorld(),raisedSpot,DrawUtility::CYAN);
		DrawUtility::Sphere(raisedSpot,DrawUtility::CYAN, 0.15f);
		break;
	case NAVMODE_DODGY:
		DrawUtility::Line(actor.GetBodyInWorld(),raisedSpot,DrawUtility::YELLOW);
		DrawUtility::Sphere(raisedSpot,DrawUtility::YELLOW, 0.15f);
		break;
	case NAVMODE_GO_TO_BEACON:
		DrawUtility::Line(actor.GetBodyInWorld(),raisedSpot,DrawUtility::BLUE);
		DrawUtility::Sphere(raisedSpot,DrawUtility::BLUE, 0.15f);
		break;
	case NAVMODE_GO_TO_BEACON_LOCK:
		DrawUtility::Line(actor.GetBodyInWorld(),raisedSpot,DrawUtility::BLUE);
		DrawUtility::Sphere(raisedSpot,DrawUtility::BLUE, 0.15f);
		break;
	case NAVMODE_WANDER:
		DrawUtility::Line(actor.GetBodyInWorld(),raisedSpot,DrawUtility::BLACK);
		DrawUtility::Sphere(raisedSpot,DrawUtility::BLACK, 0.15f);
		break;
	case NAVMODE_RUN_HOME:
		DrawUtility::Line(actor.GetBodyInWorld(),raisedSpot,DrawUtility::BLUE);
		DrawUtility::Sphere(raisedSpot,DrawUtility::BLUE, 0.15f);
		break;
	case NAVMODE_ATTACK_FROM_COVER:
		DrawUtility::Line(actor.GetBodyInWorld(),raisedSpot,DrawUtility::MAGENTA);
		DrawUtility::Sphere(raisedSpot,DrawUtility::MAGENTA, 0.15f);
		break;
	case NAVMODE_FOLLOW_AVATAR:
		DrawUtility::Line(actor.GetBodyInWorld(),raisedSpot,DrawUtility::WHITE_ALPHA);
		DrawUtility::Sphere(raisedSpot,DrawUtility::WHITE_ALPHA, 0.15f);
		break;
	case NAVMODE_RUN_FROM_NADE:
		DrawUtility::Line(actor.GetBodyInWorld(),raisedSpot,DrawUtility::RED);
		DrawUtility::Sphere(raisedSpot,DrawUtility::RED, 0.15f);
		break;
	case NAVMODE_PUSHED:
		DrawUtility::Line(actor.GetBodyInWorld(),raisedSpot,DrawUtility::RED);
		DrawUtility::Sphere(raisedSpot,DrawUtility::RED, 0.15f);
		break;
	}
#endif

	if(navType == NAVTYPE_DORMANT || navMode == NAVMODE_DISABLED)
		return;

	if(navMeshState == NAVMESH_NO_MESH_FOUND)
	{
		//borrow timer
		Math::MoveToZero(&goalCheckTimer,frameSec);
		if(goalCheckTimer == 0.0f)
		{
			if(NavMesh_SetupMesh())
			{
				startPosition = actor.GetBodyInWorld();
				navMeshState = NAVMESH_UNSTARTED;
			}
			else
			{
				//if on ground, move forward
				if(actor.GetProperty("onGround")->GetBool() && actor.Parm().GetInt("starts_sitting") != 1)
				{
					Vector3 aheadOfMe = startPosition + (actor.GetBodyToWorld().GetRow2() * 15.0f);
					moveIF->CommandMoveToPoint(aheadOfMe, NAVSPEED_JOG);
				}
				goalCheckTimer = 0.1f;
				return;
			}
		}
		else
			return;
	}

	Math::MoveToZero(&freeUseTimer,frameSec);

	switch(navState)
	{
	case NAVSTATE_NEUTRAL:
		stillTime += frameSec;
		ProcessGoalPosition(frameSec);
		if(navCoverState != NAVCOVER_NOT_IN_USE)
			NavCover_Process();
		NavMesh_Process();
		if(!CloseToPosition(goalPosition, GOAL_CHECK_DISTANCEXZ2))
		{
			if(stillTime > pauseTime)
				navState = NAVSTATE_STARTING_OFF;
		}
		//switch(navMode)
		//{
		//	case NAVMODE_MOVE_TO_TARGET:
		//	case NAVMODE_DODGY:
		//		NavMode_SideStep();
		//		navState = NAVSTATE_STARTING_OFF;
		//		break;
		//}
		//else if(navCoverState == NAVCOVER_PROCESS_FAILURE)
		//	NavMode_SideStep();
		break;
	case NAVSTATE_STARTING_OFF:
		stillTime += frameSec;
		StartMoving();
		break;
	case NAVSTATE_MOVING:
		stillTime = 0.0f;
		ProcessGoalPosition(frameSec);
		if(navCoverState != NAVCOVER_NOT_IN_USE)
			NavCover_Process();
		NavMesh_Process();
		Move(frameSec);
		break;
	case NAVSTATE_STOPPING:
		stillTime += frameSec;
		StopMoving();
		break;
	case NAVSTATE_AT_GOAL:
		stillTime += frameSec;
		navState = NAVSTATE_NEUTRAL;
		break;
	}
}
//============================================================================
void AiNavigation::SetType(int type)
{
	if(type == NAVTYPE_DORMANT)
		StopMoving();
	navType = type;
}
//============================================================================
void AiNavigation::SetMode(int mode, bool force)
{
	if(mode == navMode)
		return;

	//reset default speed
	if(mode != NAVMODE_STATIONARY && mode != NAVMODE_GO_TO_BEACON && mode != NAVMODE_GO_TO_BEACON_LOCK)
		navSpeed = NAVSPEED_JOG;

	float pause = 0.0f;

	//CURRENT MODE
	switch(navMode)
	{
	case NAVMODE_DISABLED:
		if(force)
			navMode = mode;
		else
			return;
		break;
	case NAVMODE_GO_TO_BEACON:
	case NAVMODE_GO_TO_BEACON_LOCK:
		if(force)  //keep going until forced
		{
			lastNavMode = navMode;
			navMode = mode;
		}
		else
			lastNavMode = mode; //record for changing
		break;
	case NAVMODE_RUN_FROM_NADE:
	case NAVMODE_PUSHED:
		if(goalCheckTimer == 0.0f)
			navMode = mode;
		break;
	default:
		lastNavMode = navMode;
		navMode = mode;
		break;
	}

	//after switch, if one was made
	if(navMode != mode)
		return;
	switch(navMode)
	{
	case NAVMODE_STATIONARY:
	case NAVMODE_DISABLED:
		StopMoving();
		goalCheckTimerSetting = GOAL_CHECK_TIME_DEFAULT;
		break;
	case NAVMODE_GO_TO_BEACON:
	case NAVMODE_GO_TO_BEACON_LOCK:
		pause = pauseTime;
		if(pNavCovIter != NULL)
		{
			gbl_NavManager.DestroyCoverIterator(pNavCovIter);
			pNavCovIter = NULL;
		}
		navCoverState = NAVCOVER_NOT_IN_USE;
	case NAVMODE_MELEE:
		goalCheckTimerSetting = GOAL_CHECK_TIME_MELEE;
		break;
	case NAVMODE_DODGY:
		goalCheckTimerSetting = GOAL_CHECK_TIME_DODGY;
		break;
	case NAVMODE_RUN_FROM_NADE:
	case NAVMODE_PUSHED:
		goalCheckTimerSetting = GOAL_CHECK_TIME_NADE;
		freeUseTimer = 3.5f;  //if you can't resolve the push in short order, try to go back to previous goal
		break;
	default:
		goalCheckTimerSetting = GOAL_CHECK_TIME_DEFAULT;
		break;
	}

	pauseTime = pause;
}
//============================================================================
void AiNavigation::ProcessGoalPosition(float frameSec)
{
	Math::MoveToZero(&goalCheckTimer,frameSec);
	if(goalCheckTimer == 0.0f)
	{
		DetermineGoalPosition();  //depending on what kind of approach...find a good goal
		goalCheckTimer = goalCheckTimerSetting;
	}

	if(goalPosition != lastGoalPosition)  //goal changed, reconstruct path
	{
		navMeshState = NAVMESH_CREATE_PATH;
		lastGoalPosition = goalPosition;
	}
	else if (stillTime > STILL_TIME_THRESHOLD)
	{
		switch(navMode)
		{
			case NAVMODE_MOVE_TO_TARGET:
			case NAVMODE_FLANK_TO_TARGET:
			case NAVMODE_RETREAT:
			case NAVMODE_DODGY:
			case NAVMODE_RUN_HOME:
				if(aiPerception && aiPerception->inSight == true)
				{
					NavMode_SideStep();
					goalCheckTimer = goalCheckTimerSetting;
					navMeshState = NAVMESH_CREATE_PATH;
					lastGoalPosition = goalPosition;
					stillTime = 0.0f;
				}
				break;
		}
	}
}
//============================================================================
void AiNavigation::DetermineGoalPosition()
{
	//navMode = NAVMODE_MOVE_TO_TARGET; //testing

	//default speed
	//navSpeed = NAVSPEED_JOG;

	//if I have a hostage, try to escape
	if((ActorHandle)actor.GetProperty( "hostage" )->GetHandle())
		navMode = NAVMODE_RETREAT;

	switch(navMode)
	{
	case NAVMODE_MOVE_TO_TARGET:
		NavMode_MoveToTarget(NAVSPEED_JOG);
		break;
	case NAVMODE_MELEE:
		//if target is off mesh, retreat
		if(aiPerception && aiPerception->GetLastKnownLocation(lastKnownTargetPos) && !NavMesh_PointOnMesh(lastKnownTargetPos))
		{
			navSpeed = NAVSPEED_JOG;
			NavMode_Retreat();
			break;
		}
		NavMode_MoveToTarget(NAVSPEED_JOG);
		break;
	case NAVMODE_RETREAT:
		NavMode_Retreat();
		break;
	case NAVMODE_DODGY:
		NavMode_DodgeTarget();
		break;
	case NAVMODE_GO_TO_BEACON:
	case NAVMODE_GO_TO_BEACON_LOCK:
		NavMode_GoToBeacon();
		break;
	case NAVMODE_WANDER:
		NavMode_Wander();
		break;
	case NAVMODE_RUN_HOME:
		goalPosition = startPosition;
		if(CloseToPosition(goalPosition, GOAL_CHECK_DISTANCEXZ2))
			SetMode(lastNavMode,true);
		break;
	case NAVMODE_ATTACK_FROM_COVER:
		if(aiPerception->GetLastKnownLocation(lastKnownTargetPos))
		{
			if(actor.Parm().GetInt("moveup") == 1)
				NavMode_AttackFromCover(true);
			else
				NavMode_AttackFromCover(false);
			NavCover_FindCover(lastKnownTargetPos, desiredGoal);
		}
		break;
	case NAVMODE_FOLLOW_AVATAR:
		NavMode_FollowAvatar();
		break;
	case NAVMODE_RUN_FROM_NADE:
		SetMode(lastNavMode);
		break;
	case NAVMODE_PUSHED:
		NavMode_Pushed();
		break;
	}
} 
//============================================================================
void AiNavigation::StartMoving()
{
	if(navMeshState == NAVMESH_GET_SEGMENT)
	{
		NavMesh_Process();
		return;
	} 

	if(navMeshState != NAVMESH_MOVE_TO_POINT)
		navState = NAVSTATE_NEUTRAL;
	else if(CloseToPosition(waypoint, WAYPOINT_CHECK_DISTANCEXZ2, true))
		navMeshState = NAVMESH_GET_NEW_POINT;
	else if(aiPerception->BlindMansStick(waypoint))
		navState = NAVSTATE_NEUTRAL;
	else		
	{
		aiPerception->BodyControlPeek(AiPerception::PEEK_NONE);
		g_messageDispatcher.SendMessageToActor("StopFullBodyAnimation",NULL,actor.GetHandle(),actor.GetHandle());
		navState = NAVSTATE_MOVING;
	}
}
//============================================================================
void AiNavigation::Move(float frameSec)
{
	if(navMeshState == NAVMESH_CREATE_PATH_FAILED)
	{
		navState = NAVSTATE_STOPPING;
		//dbgPrint("Can not create Nagivation path");
		return;
	}

	if(CloseToPosition(goalPosition, GOAL_CHECK_DISTANCEXZ2))
		navState = NAVSTATE_STOPPING;
	else if(aiPerception->BlindMansStick(waypoint))
		navState = NAVSTATE_STOPPING;
	else
	{
		if(CloseToPosition(waypoint, WAYPOINT_CHECK_DISTANCEXZ2, true))
		{
			if(navMeshState == NAVMESH_MOVE_TO_POINT)
				navMeshState = NAVMESH_GET_NEW_POINT;
			else
				navState = NAVSTATE_STOPPING; //must be failing to get a legit path
		}
		else
			moveIF->CommandMoveToPoint(waypoint, navSpeed);
	}
}

//============================================================================
bool AiNavigation::CloseToPosition(Vector3 i_goal, float i_dist, bool xzOnly)
{
	Vector3 toGoal = i_goal - actor.GetBodyInWorld();
	float dist;
	if(xzOnly)
		dist = toGoal.LengthXZSquared(); //this is necessary for waypoint accuracy
	else
		dist = toGoal.LengthSquared();

	if(dist < i_dist)
		return true;
	else
		return false;
}
//============================================================================
void AiNavigation::StopMoving()
{
	moveIF->CommandForwardSpeed( 0.0f );
	moveIF->CommandSideSpeed( 0.0f );
	navState = NAVSTATE_AT_GOAL;
}
//============================================================================
void AiNavigation::NavMode_MoveToTarget(float speed)
{
	navSpeed = speed;
	bool tryNext = true;

	CActor* target = NULL;
	if(aiPerception->LastKnownTarget() != INVALID_OBJECT_HANDLE)
		target = CActor::FromHandle(aiPerception->LastKnownTarget());

	float xzsqDist = 0.0f;
	Vector3 toTarget = actor.GetBodyToWorld().GetRow2();
	if(target)
	{
		toTarget = target->GetBodyInWorld() - actor.GetBodyInWorld();
		xzsqDist = toTarget.LengthXZSquared();
	}

	//Hopefully move to their actual location, this isn't cheating since you just saw or heard them
	if(aiPerception->GetRecentContact(10.0f) && target && navMeshState != NAVMESH_CREATE_PATH_FAILED)
	{
		//goalCheckTimerSetting = GOAL_CHECK_TIME_MOVETO_CONTACT;

		//if I am virtually there, stop, otherwise move to target actor.  This is important since your final goal would collide with target
		if(xzsqDist < DISTANCEXZSQRD_VIRTUALY_THERE)
		{
			desiredGoal = actor.GetBodyInWorld();
			SetGoalPosition(desiredGoal);
			tryNext = false;
		}

		if(tryNext)
		{
			AiWeaponControl *aiWeaponControl = (AiWeaponControl*) actor.GetComponentByName( "AiWeaponControl" );
			if(aiWeaponControl->GetSelectedClass() == WEAPONCLASS_MELEE || navMode == NAVMODE_MELEE)
				desiredGoal = target->GetBodyInWorld();
			else
			{
				Vector3 backToMe = toTarget.Normalized();
				float spacing = MOVE_TO_SPACING;
				backToMe *= -spacing;
				desiredGoal = target->GetBodyInWorld() + backToMe;
				if(GetPointOnMesh(desiredGoal))
					tryNext = false;
				else
					desiredGoal = target->GetBodyInWorld();
			}

			if(tryNext && GetPointOnMesh(desiredGoal))
				tryNext = false;
		}

		if(tryNext)
		{
			desiredGoal = target->GetBodyInWorld();
			if(GetPointOnMesh(desiredGoal,0.0f,10.0f,10))
				tryNext = false;
		}
	}

	//Second best, go to lastKnown if you don't have current contact or the above failed
	if(tryNext && aiPerception->GetLastKnownLocation(lastKnownTargetPos))
	{
		//goalCheckTimerSetting = GOAL_CHECK_TIME_MOVETO_NOCONTACT;
		navSpeed = NAVSPEED_WALK;
		desiredGoal = lastKnownTargetPos;
		if(xzsqDist > 100.0f && GetPointOnMesh(desiredGoal,0.0f,10.0f,10))
			tryNext = false;
		else if(GetPointOnMesh(desiredGoal,0.0f,0.0f,10))
			tryNext = false;
	}

	if(tryNext == false) //something succeeded
		SetGoalPosition(desiredGoal);
	//else
	//	dbgPrint("wtf?\n");
}
//============================================================================
void AiNavigation::NavMode_FollowAvatar()
{
	if (g_players.GetPlayer(0) == NULL)
		return;

	desiredGoal = g_players.GetPlayer(0)->GetBodyInWorld();
	Vector3 toAvatar = desiredGoal - actor.GetBodyInWorld();
	float distancesq = toAvatar.LengthXZSquared();

	if(distancesq < 20.0f)
		desiredGoal = actor.GetBodyInWorld();
		
	if(GetPointOnMesh(desiredGoal))
		SetGoalPosition(desiredGoal);

	navSpeed = NAVSPEED_RUN;
}
//============================================================================
void AiNavigation::NavMode_Retreat()
{
	if(aiPerception->GetLastKnownLocation(lastKnownTargetPos))
	{
		Technique_MoveAwayFromPosition(lastKnownTargetPos,RETREAT_DISTANCE);
		NavCover_FindCover(lastKnownTargetPos, desiredGoal);
	}
}
//============================================================================
void AiNavigation::NavMode_DodgeTarget()
{
	if(navCoverState == NAVCOVER_PROCESSING)
		return;
	if(!aiPerception->GetLastKnownLocation(lastKnownTargetPos))
		return;
	if(navCoverState == NAVCOVER_PROCESS_FAILURE || g_timer.GetStartSec() - aiPerception->GetLastHeardTime() > STOP_DODGING_TIME)
		NavMode_MoveToTarget(NAVSPEED_JOG);
	else
	{
		//gather info
		Vector3 toTarget = lastKnownTargetPos - actor.GetBodyInWorld();
		toTarget.Normalize();
		Vector3 up; up.Set(0.0,1.0f,0.0f);
		Vector3 sideFromTarget = toTarget.Cross(up);
		//determine which side to run
		float dotSide = sideFromTarget.Dot(actor.GetBodyToWorld().GetRow2());
		float direction = 1.0f;
		if(dotSide > 0.0f)
			direction = -1.0f;
		//devise desired
		desiredGoal = actor.GetBodyInWorld() + (toTarget * 8.0f) + (sideFromTarget * 5.0f * direction);
	}
	NavCover_FindCover(lastKnownTargetPos, desiredGoal);
}
//============================================================================
void AiNavigation::NavMode_Wander()
{
	//this gets called every GOAL_CHECK_TIME
	Math::MoveToZero(&wanderTimer,1.0f);
	if(wanderTimer == 0.0f)
	{
		if(CloseToPosition(wanderCenter, WANDER_CHECK_DISTANCEXZ2))
			desiredGoal = actor.GetBodyInWorld();
		else
			desiredGoal = wanderCenter;

		if(GetPointOnMesh(desiredGoal,5.0f,10.0f,4))
			SetGoalPosition(desiredGoal);
		wanderTimer = g_random.InRange(WANDER_TIME,WANDER_TIME * 2.0f);
		navSpeed = NAVSPEED_WALK;
	}
}
//============================================================================
void AiNavigation::NavMode_AttackFromCover(bool allowMoveup)
{
	if(navCoverState == NAVCOVER_PROCESSING)
		return;

	if(!aiPerception->GetLastKnownLocation(lastKnownTargetPos))
		return;

	//if we are in a failure state, see if dodging will get us the hell outta here
	if(navCoverState == NAVCOVER_PROCESS_FAILURE && aiPerception->inSight)
	{
		NavMode_MoveToTarget(NAVSPEED_JOG);
		NavCover_FindCover(lastKnownTargetPos, desiredGoal);
		return;
	}

	Vector3 toTarget = lastKnownTargetPos - actor.GetBodyInWorld();

	//if full cover be dodgy
	if(GetCoverType() == Navigate::TreeNode::FULLCOVER_EDGE)
		NavMode_DodgeTarget();
	else if(toTarget.LengthXZSquared() < DISTANCEXZSQRD_CLOSE)
		NavMode_Retreat();
	else if(GetCoverType() == Navigate::TreeNode::NOCOVER_EDGE)
		NavMode_Retreat();
	else
	{
		if(allowMoveup)
		{
			//if you don't see him, move up
			if(aiPerception->GetRecentSighting(COVER_MOVE_UP_TIME))
			{
				//don't set new goal unless in nuetral
				if(navState != NAVSTATE_NEUTRAL)
					return;

				//don't close within minimum distance
				if(toTarget.LengthXZSquared() > COVER_MOVE_UP_DISTXZSQRD)
				{
					Vector3 toCover = toTarget;
					toCover *= COVER_MOVE_UP_SCALAR; //half way to you
					desiredGoal = actor.GetBodyInWorld() + toCover;
				}
				else
					desiredGoal = actor.GetBodyInWorld();
			}
			else
				desiredGoal = actor.GetBodyInWorld();
		}
		else
			desiredGoal = actor.GetBodyInWorld();

		NavCover_FindCover(lastKnownTargetPos, desiredGoal);
	}
}
//============================================================================
void AiNavigation::NavMode_RunFromNade()
{
	navSpeed = NAVSPEED_JOG;
}
//============================================================================
void AiNavigation::NavMode_Pushed()
{
	navSpeed = NAVSPEED_JOG;

	if(navState == NAVSTATE_MOVING || freeUseTimer == 0.0f)
	{
		goalPosition = savedGoalPosition;
		desiredGoal = savedGoalPosition;
		SetMode(lastNavMode);  //we're done
		return;
	}

	//you haven't move so just try to get anywhere but here
	Technique_MoveAwayFromPosition(actor.GetBodyInWorld(),1.0f,4.0f);
	//dbgPrint("stillTime in Pushed >>>>>>>>>>> %f\n",stillTime);
}
//============================================================================
void AiNavigation::NavMode_SideStep()
{
	desiredGoal = actor.GetBodyToWorld().GetRow0() * (g_random.Sign() * 2.5f);
	desiredGoal += actor.GetBodyInWorld();

	if(GetPointOnMesh(desiredGoal,0.0f,0.5f))
		SetGoalPosition(desiredGoal);

	return;
}
//============================================================================
void AiNavigation::NavMode_GoToBeacon()
{
	bool stop = false;

	if(navMode == NAVMODE_GO_TO_BEACON)
	{
		if(aiPerception->GetRecentSighting(1.0f))
			stop = true;
	}

	//in case we were interupted, go back to trying to get to goal
	if(!stop)
		goalPosition = savedGoalPosition;

	if(CloseToPosition(goalPosition, GOAL_CHECK_DISTANCEXZ2))
	{
		stop = true;
		g_messageDispatcher.SendMessageToActor("GetNextBeacon",NULL,actor.GetHandle(),beaconHandle);
	}

	if(stop)
	{
		//turn in direction of beacon
		AiBodyControl *aiBodyControl = (AiBodyControl *)actor.GetComponentByName( "AiBodyControl" );
		if(aiBodyControl && savedLookAt.LengthXZSquared() != 0.0f)
			aiBodyControl->LookAtPosition(savedLookAt);
		SetMode(lastNavMode,true);
	}
}
//============================================================================
bool AiNavigation::NavMesh_SetupMesh()
{
	pNavMesh = gbl_NavManager.FindBestMesh( actor.GetBodyInWorld() );
	if( pNavMesh )
		return true;
	return false;
}
//============================================================================
bool AiNavigation::NavMesh_CreatePath(Vector3 beginPoint, Vector3 endPoint)
{
	if(pNavPath)
	{
		gbl_NavManager.DestroyPath( pNavPath );
		pNavPath = NULL;
	}
	pPathSegment = NULL;

	// create the path
	bool bBeginOnMesh = NavMesh_PointOnMesh( beginPoint );
	bool bEndOnMesh = NavMesh_PointOnMesh( endPoint );

	if( !bBeginOnMesh )
		bBeginOnMesh = NavMesh_PointOnMesh( beginPoint, 10.0f );

	if( !bEndOnMesh )
		bEndOnMesh = NavMesh_PointOnMesh( endPoint, 20.0f );

	if( bBeginOnMesh && bEndOnMesh )
	{
		const Vector3* pointArray[2];
		pointArray[0] = &beginPoint;
		pointArray[1] = &endPoint;

		ASSERT( pNavMesh != NULL );
		pNavPath = gbl_NavManager.CreateOneWayPath( pNavMesh, 2, pointArray );

		ASSERT( pNavPath != NULL );
		bool bFailedInit = !( pNavPath->Initialize() );
		if ( bFailedInit )
		{
			gbl_NavManager.DestroyPath( pNavPath );
			pNavPath = NULL;
			return( false );
		}
		return( true );
	}

	//if(!bBeginOnMesh)
	//	actor.Place(startPosition, startHeading);

	//if(!bBeginOnMesh)
	//{
	//	if(NavMesh_PointOnMesh( waypoint ))
	//		goalPosition = waypoint;
	//}

	//if(!bBeginOnMesh)
	//{
		//turn on terrain collisions for a bit
	//	actor.PositiveExclude() &= ~WORLD_CATEGORY;
	//}

	return false;
}
//============================================================================
bool AiNavigation::NavMesh_GetNextPoint(Vector3 &nextPoint)
{
	if(waypointCur >= waypointCount || pPathSegment == NULL)
		return false;
	
	waypoint = pPathSegment->m_Waypoints[waypointCur];
	waypointCur++;
	
	return true;
}
//============================================================================
bool AiNavigation::NavMesh_PointOnMesh(Vector3 &point, float yDist)
{
	if(navMeshState == NAVMESH_NO_MESH_FOUND)
	{
		if(NavMesh_SetupMesh())
		{
			startPosition = actor.GetBodyInWorld();
			navMeshState = NAVMESH_UNSTARTED;
		}
	}
	Vector3 actualPointOnMesh;
	ASSERT( pNavMesh != NULL );
	bool success =	gbl_NavManager.TestPointInMesh(pNavMesh, point, actualPointOnMesh, yDist);
	if(success)
		point = actualPointOnMesh;
	//else
	//	dbgPrint("Find mesh point failed at: %f %f %f\n",point.x(),point.y(),point.z());
	return success;
}
//============================================================================
void AiNavigation::NavMesh_Process()
{
	switch(navMeshState)
	{
		case NAVMESH_UNSTARTED:
			break;
		case NAVMESH_CREATE_PATH:
		case NAVMESH_CREATE_PATH_FAILED:
			if(NavMesh_CreatePath(actor.GetBodyInWorld(),goalPosition))
				navMeshState = NAVMESH_GET_SEGMENT;
			else
				navMeshState = NAVMESH_CREATE_PATH_FAILED;
				//goalCheckTimer = Math::Clamp(goalCheckTimer,0.0f,0.1f); //better check for a new goal
			break;
		case NAVMESH_GET_SEGMENT:
			if(pNavPath && !pNavPath->HasWorkToDo())
			{
				pPathSegment = pNavPath->GetPathSegment(0);
				//pPathConnection = pNavPath->GetConnection();
				if(pPathSegment)
					navMeshState = NAVMESH_SEGMENT_PROCESSING;
			}
			break;
		case NAVMESH_SEGMENT_PROCESSING:
			waypointCount = pPathSegment->m_Waypoints.Count();
			//don't allow bots to navigate far if they have never seen the avatar
			if(navMode == NAVMODE_GO_TO_BEACON || navMode == NAVMODE_GO_TO_BEACON_LOCK ||navMode == NAVMODE_FOLLOW_AVATAR || waypointCount <= maxFirstWays || aiPerception->GetLastSeenTime() != 0.0f)
			{
				waypointCur = 0; 
				navMeshState = NAVMESH_GET_NEW_POINT;
			}
			break;
		case NAVMESH_GET_NEW_POINT:
			if(NavMesh_GetNextPoint(waypoint))
				navMeshState = NAVMESH_MOVE_TO_POINT;
			else
				navMeshState = NAVMESH_CREATE_PATH;
			break;
		case NAVMESH_MOVE_TO_POINT:
			break;
	};
}
//============================================================================
void AiNavigation::NavCover_FindCover(Vector3 threatPosition, Vector3 fromPosition, float radius)
{
	//if the current iterator failed, better create a new one
	if(navCoverState == NAVCOVER_PROCESS_FAILURE && pNavCovIter != NULL)
	{
		gbl_NavManager.DestroyCoverIterator(pNavCovIter);
		pNavCovIter = NULL;
	}

	if(pNavCovIter == NULL)
	{
		ASSERT( pNavMesh != NULL );
		if ( !pNavMesh )
		{
			navCoverState = NAVCOVER_PROCESS_FAILURE;
			return;
		}
		threatPosition.y(fromPosition.y()); //put on same y
		pNavCovIter = gbl_NavManager.CreateCoverIterator(pNavMesh, threatPosition, fromPosition, radius);
		navCoverDir = threatPosition - fromPosition;
		navCoverDir.SafeNormalize();
		navCoverState = NAVCOVER_PROCESSING;
	}
 }
//============================================================================
void AiNavigation::NavCover_Process()
{
	//is it process?
	if(navCoverState != NAVCOVER_PROCESSING)
		return;
	//is it done processing?
	if(pNavCovIter && pNavCovIter->HasWorkToDo()) //still processing
		return;
	//did it find cover?
	if(pNavCovIter->Count() == 0)
	{
		//send them to the uncovered desired goal
		if(GetPointOnMesh(desiredGoal))
			SetGoalPosition(desiredGoal);

		gbl_NavManager.DestroyCoverIterator(pNavCovIter);
		pNavCovIter = NULL;
		navCoverState = NAVCOVER_PROCESS_FAILURE;
		return;
	}

	if(pNavCovIter == NULL)
		return;

	navCoverType = (*pNavCovIter)[0].m_iCoverType;
	//it shouldn't return this, but it is
	if(navCoverType == Navigate::TreeNode::NOCOVER_EDGE)
	{
		navCoverState = NAVCOVER_PROCESS_FAILURE;
		return;
	}
	//devise a point from the cover info

	Vector3Packed *begPoint = (*pNavCovIter)[0].m_pBegin;
	Vector3Packed *endPoint = (*pNavCovIter)[0].m_pEnd;
	Vector3 toEnd = *endPoint - *begPoint;
	toEnd *= 0.5f;
	desiredGoal = *begPoint + toEnd;
	Vector3 coverOffset = lastKnownTargetPos - desiredGoal;
	coverOffset.SafeNormalize();
	coverOffset *= -2.0f;
	desiredGoal += coverOffset;
	//clean up, cuz we done
	gbl_NavManager.DestroyCoverIterator(pNavCovIter);
	pNavCovIter = NULL;
	//is the point good?
	if(GetPointOnMesh(desiredGoal))
		SetGoalPosition(desiredGoal);
	else
	{
		navCoverState = NAVCOVER_PROCESS_FAILURE;
		return;
	}
	navCoverState = NAVCOVER_PROCESS_SUCCESS;
	return;
}
//============================================================================
void AiNavigation::NavigationBeacon::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	CActor* sender = CActor::FromHandle(i_sender);
	if(sender == NULL)
		return;

	Vector3 navigationBeaconPos = sender->GetBodyInWorld();
	if(d_ref->NavMesh_PointOnMesh(navigationBeaconPos, 2.0f))
	{
		d_ref->goalPosition = navigationBeaconPos;
		d_ref->desiredGoal = navigationBeaconPos;
		int lock = (int) i_data;
		if(lock)
			d_ref->SetMode(NAVMODE_GO_TO_BEACON_LOCK);
		else
			d_ref->SetMode(NAVMODE_GO_TO_BEACON);
	}
	else
	{
		ASSERT(false); //navbeacon needs to be repositioned
	}

	//override speed
	float moveSpeed = sender->Parm().GetFloat("move_speed");
	if(moveSpeed > 0.0f && moveSpeed < NAVSPEED_RUN)
			d_ref->navSpeed = moveSpeed;

	d_ref->pauseTime = sender->Parm().GetFloat("beacon_pause");
	d_ref->beaconHandle = i_sender;

	if(sender->Parm().GetInt("usedir") == 1)
		d_ref->savedLookAt = sender->GetBodyInWorld() + (sender->GetBodyToWorld().GetRow2() * 4.0f);
	else
		d_ref->savedLookAt.Clear();

	d_ref->savedGoalPosition = d_ref->goalPosition;
}
//============================================================================
void AiNavigation::CreateHomePoint::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	CActor* sender = CActor::FromHandle(i_sender);
	if(sender == NULL)
		return;
	Vector3 navigationBeaconPos = sender->GetBodyInWorld();
	if(d_ref->NavMesh_PointOnMesh(navigationBeaconPos, 2.0f))
		d_ref->startPosition = navigationBeaconPos;

	d_ref->pauseTime = sender->Parm().GetFloat("beacon_pause");
	d_ref->beaconHandle = i_sender;

	if(sender->Parm().GetInt("usedir") == 1)
		d_ref->savedLookAt = sender->GetBodyInWorld() + (sender->GetBodyToWorld().GetRow2() * 4.0f);
	else
		d_ref->savedLookAt.Clear();
}
//============================================================================
void AiNavigation::YouAreInMyWay::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	CActor* sender = CActor::FromHandle(i_sender);
	if(sender == NULL)
		return;
	d_ref->SetPushed(sender->GetBodyInWorld());
}
//============================================================================
void AiNavigation::SetRunFromNade(Vector3 location)
{
	Vector3 awayFromNade = actor.GetBodyInWorld() - location;
	if(awayFromNade.LengthXZSquared() > RUN_FROM_NADE_XZ_SQRD_DIST_MAX)
		return;

	if(Technique_MoveAwayFromPosition(location,RUN_FROM_NADE_FACTOR))
		SetMode(NAVMODE_RUN_FROM_NADE);
}
//============================================================================
void AiNavigation::SetPushed(Vector3 pusherPos)
{
	//use old method
	if(navMode == NAVMODE_GO_TO_BEACON || navMode == NAVMODE_GO_TO_BEACON_LOCK)
	{
		//float pushdistance = g_random.InRange(5.0f * 0.5f, 5.0f * 2.0f);
		//Technique_MoveAwayFromPosition(pusherPos,pushdistance);
		return;
	}

	//in push mode already and therefore seeking a new home
	if(navMode == NAVMODE_PUSHED)
		return;

	//initially just try a straight away move
	Technique_MoveAwayFromPosition(pusherPos,g_random.InRange(2.0f,4.0f),0.5f);
   SetMode(NAVMODE_PUSHED);
	goalCheckTimer = 0.5f;
	goalCheckTimerSetting = 0.5f;
}
//============================================================================
bool AiNavigation::GetPointOnMesh(Vector3 &i_position, float randomDistMin, float randomDistMax, int tries)
{
	Vector3 tryPosition;
	bool success = false;

	for(int i = 0; i < tries ; i++)
	{
		tryPosition = i_position;
		tryPosition.x(tryPosition.x() + (g_random.InRange(randomDistMin,randomDistMax)) * g_random.Sign());	
		tryPosition.z(tryPosition.z() + (g_random.InRange(randomDistMin,randomDistMax)) * g_random.Sign());	

		if(NavMesh_PointOnMesh(tryPosition,POINT_ON_MESH_Y_MIN))
		{
			success = true;
			break;
		}
	}
	
	if(!success)
	{
		for(int i = 0; i < tries ; i++)
		{
			tryPosition = i_position;
			tryPosition.x(tryPosition.x() + g_random.InRange(randomDistMin,randomDistMax) * g_random.Sign());	
			tryPosition.z(tryPosition.z() + g_random.InRange(randomDistMin,randomDistMax) * g_random.Sign());	
			if(NavMesh_PointOnMesh(tryPosition,POINT_ON_MESH_Y_MAX))
			{
				success = true;
				break;
			}
		}
	}

	if(success)
		i_position = tryPosition;
	return success;
}
//============================================================================
bool AiNavigation::Technique_MoveToActor(CActor *target, float howClose)
{
	if(target == NULL)
		return false;
	Vector3 toTarget = target->GetBodyInWorld() - actor.GetBodyInWorld();
	desiredGoal = target->GetBodyInWorld();
	if(toTarget.LengthXZSquared() < howClose)
		return false;
	if(GetPointOnMesh(desiredGoal))
	{
		SetGoalPosition(desiredGoal);
		return true;
	}
	return false;
}
//============================================================================
bool AiNavigation::Technique_MoveToPosition(Vector3 i_position)
{
	desiredGoal = i_position;
	if(GetPointOnMesh(desiredGoal))
	{
		SetGoalPosition(desiredGoal);
		return true;
	}
	return false;
}
//============================================================================
bool AiNavigation::Technique_MoveAwayFromPosition(Vector3 i_position, float distFromPos, float maxSearchRadius)
{
	float minSearchRadius = 0.0f;

	if(i_position == actor.GetBodyInWorld())
	{
		desiredGoal = i_position;
		minSearchRadius = 2.0f;
	}
	else
	{
		desiredGoal = actor.GetBodyInWorld() - i_position;
		desiredGoal.SafeNormalize();
		desiredGoal *= distFromPos;
		desiredGoal += actor.GetBodyInWorld();
	}

	if(GetPointOnMesh(desiredGoal,minSearchRadius,maxSearchRadius,5))
	{
		SetGoalPosition(desiredGoal);
		return true;
	}
	return false;
}
//============================================================================
bool AiNavigation::Technique_SideStep()
{
	desiredGoal = actor.GetBodyToWorld().GetRow0() * (g_random.Sign() * 2.5f);
	desiredGoal += actor.GetBodyInWorld();
	if(GetPointOnMesh(desiredGoal,0.0f,0.5f))
	{
		SetGoalPosition(desiredGoal);
		return true;
	}
	return false;
}
//============================================================================
bool AiNavigation::Technique_MoveDirection(Vector3 dir)
{
	return false;
}
//============================================================================
bool AiNavigation::SetGoalPosition(Vector3 goal)
{
	goalPosition = goal;
	return true;
}	
//============================================================================
bool AiNavigation::ResetToStartPosition()
{
	//don't place on avatar
	Vector3 toAvatar = g_players.GetPlayer(0)->GetBodyInWorld() - startPosition;
   if(toAvatar.LengthSquared() < DISTANCEXZSQRD_VIRTUALY_THERE)
		return false;

	if(navState == NAVSTATE_MOVING)
		navState = NAVSTATE_STOPPING;
	desiredGoal = startPosition;
	goalPosition = startPosition;
	actor.Place(startPosition, startHeading);
	return true;
}
//============================================================================
bool AiNavigation::SetStartPosition(Vector3 pos)
{
	if(GetPointOnMesh(pos) || navMode == NAVMODE_STATIONARY)
	{
		startPosition = pos;
		return true;
	}
	else
		return false;
}
//============================================================================