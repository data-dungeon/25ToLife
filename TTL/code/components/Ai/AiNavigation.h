#ifndef AI_NAVIGATION_H
#define AI_NAVIGATION_H

#include "Components/Component.h"
#include "components/Ai/AiPerception.h"
#include "Navigate/NavManager.h"
#include "Interfaces/MoveIF.h"
#include "Interfaces/ViewIF.h"

#define BEACONS_MAX 5

class AiNavigation : public CActorComponent
{
public:
	AiNavigation( CCompActor &i_actor );
	~AiNavigation();
	const char *ComponentName( void ) { return "AiNavigation"; }
	void Initialize(void);
	void Reset(void);
	void Update(float frameSec);
	void SetType(int type);
	int GetState(){return navState;}
	void SetState(int state){navState = state;}
	void SetMode(int mode, bool force = false);
	int GetMode(){return navMode;}
	void SetSpeed(float speed){navSpeed = speed;}
	void SetCover(int cover){navCover = cover;}
	void SetWanderCenter(Vector3 center){wanderCenter = center;}
	int GetCoverType(){return navCoverType;}
	void SetRunFromNade(Vector3 location);
	int GetCoverState() {return navCoverState;}
	Vector3 GetCoverDir(){return navCoverDir;}
	Vector3 GetGoalPosition(){return goalPosition;}
	Vector3 GetWaypoint(){return waypoint;}
	void SetPushed(Vector3 pusherPos);
	void SetGoalCheckTimer(float i_time){goalCheckTimer = i_time;}
	float GetGoalCheckTimer(){return goalCheckTimer;}
	void NavMesh_CreateNewPath(){navMeshState = NAVMESH_CREATE_PATH;}
	bool ResetToStartPosition();
	bool SetStartPosition(Vector3 pos);

	enum NAV_TYPES
	{
		NAVTYPE_DORMANT,
		NAVTYPE_COMBAT,
		NAVTYPE_NONCOMBAT
	};

	enum NAV_STATE
	{
		NAVSTATE_DISABLED,
		NAVSTATE_NEUTRAL,
		NAVSTATE_STARTING_OFF,
		NAVSTATE_MOVING,
		NAVSTATE_STOPPING,
		NAVSTATE_AT_GOAL,

		NUMBER_OF_NAVSTATES
	};

	enum NAV_MODES
	{
		NAVMODE_DISABLED,
		NAVMODE_STATIONARY,
		NAVMODE_MOVE_TO_TARGET,
		NAVMODE_FLANK_TO_TARGET,
		NAVMODE_RETREAT,
		NAVMODE_DODGY,
		NAVMODE_RETURN_TO_START,
		NAVMODE_GO_TO_BEACON,
		NAVMODE_GO_TO_BEACON_LOCK,
		NAVMODE_WANDER,
		NAVMODE_RUN_HOME,
		NAVMODE_ATTACK_FROM_COVER,
		NAVMODE_MELEE,
		NAVMODE_FOLLOW_AVATAR,
		NAVMODE_RUN_FROM_NADE,
		NAVMODE_PUSHED,
		NAVMODE_SIDESTEP,

		NUMBER_OF_NAVMODES
	};

	enum NAVCOVER_STATES
	{
		NAVCOVER_NOT_IN_USE,
		NAVCOVER_PROCESS_DESIREDGOAL,
		NAVCOVER_PROCESSING,
		NAVCOVER_PROCESS_SUCCESS,
		NAVCOVER_PROCESS_FAILURE
	};

	enum NAVMESH_STATES
	{
		NAVMESH_NO_MESH_FOUND,
		NAVMESH_UNSTARTED,
		NAVMESH_CREATE_PATH,
		NAVMESH_CREATE_PATH_FAILED,
		NAVMESH_GET_SEGMENT,
		NAVMESH_SEGMENT_PROCESSING,
		NAVMESH_GET_NEW_POINT,
		NAVMESH_MOVE_TO_POINT
	};

	//so that behavior can stop the npc when on timeout
	void StopMoving();

private:
	REF_SEND_MESSAGE_HANDLER(NavigationBeacon, AiNavigation) d_navigationBeaconHandler;
	REF_SEND_MESSAGE_HANDLER(CreateHomePoint, AiNavigation) d_createHomePointHandler;
	REF_SEND_MESSAGE_HANDLER(YouAreInMyWay, AiNavigation) d_youAreInMyWayHandler;
	
	void DetermineGoalPosition();
	void StartMoving();
	void Move(float frameSec);
	bool CloseToPosition(Vector3 i_goal, float i_dist, bool xzOnly = false);
	void ProcessGoalPosition(float frameSec);
	bool GetPointOnMesh(Vector3 &position, float randomDistMin = 0.0f, float randomDistMax = 0.0f, int tries = 1);
	bool SetGoalPosition(Vector3 goal);

	void NavMode_MoveToTarget(float speed);
	void NavMode_DodgeTarget();
	void NavMode_Retreat();
	void NavMode_Wander();
	void NavMode_AttackFromCover(bool allowMoveup);
	void NavMode_FollowAvatar();
	void NavMode_RunFromNade();
	void NavMode_Pushed();
	void NavMode_SideStep();
	void NavMode_GoToBeacon();

	bool Technique_MoveToActor(CActor *target, float howClose = 1.0f);
	bool Technique_MoveToPosition(Vector3 i_position);
	bool Technique_MoveAwayFromPosition(Vector3 i_position, float distFromPos, float maxSearchRadius = 5.0f);
	bool Technique_SideStep();
	bool Technique_MoveDirection(Vector3 dir);

	void NavCover_FindCover(Vector3 threatPosition, Vector3 fromPosition, float radius = 10.0f);
	void NavCover_Process();
	bool NavMesh_SetupMesh();
	bool NavMesh_CreatePath(Vector3 beginPoint, Vector3 endPoint);
	bool NavMesh_GetNextPoint(Vector3 &nextPoint);
	void NavMesh_Process();
	bool NavMesh_PointOnMesh(Vector3 &point, float yDist = 2.0f);


	AiPerception *aiPerception;
	CMoveIF *moveIF;
	ViewIF *viewIF;
	int navType;
	int navState;
	int navMode;
	float navSpeed;
	int navCover;
	Vector3 goalPosition;
	Vector3 desiredGoal;
	Vector3 lastGoalPosition;
	Vector3 startPosition;
	float startHeading;
	Vector3 lastKnownTargetPos;
	float goalCheckTimer;
	float goalCheckTimerSetting;
	float returnToStartTimer;
	float wanderTimer;
	Vector3 wanderCenter;
	int navCoverType;
	int lastNavMode;
	float stillTime;
	float pauseTime;
	Vector3 savedLookAt;

	int navMeshState;
	int waypointCur;
	int waypointCount;
	Vector3 waypoint;
	int maxFirstWays;
	Navigate::Mesh	*pNavMesh;
	Navigate::Path	*pNavPath;
	Navigate::PathSegment	*pPathSegment;
	//Navigate::PathConnection *pPathConnection;

	int navCoverState;
	Navigate::CoverIterator *pNavCovIter;
	Vector3 navCoverDir;

	ActorHandle beaconHandle;
	Vector3 savedGoalPosition;
	float freeUseTimer;
};

#endif // AI_NAVIGATION_H



