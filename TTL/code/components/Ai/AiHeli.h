#ifndef AI_HELI_H
#define AI_HELI_H

#include "Components/Component.h"

class AiHeli : public CActorComponent
{
public:
	AiHeli( CCompActor &i_actor );
	~AiHeli();
	const char *ComponentName( void ) { return "AiHeli"; }
	void Initialize(void);
	void Reset(void);

	virtual void BeginFrame();
	virtual void EndUpdate(float i_deltaSec);

	enum HELI_MODES
	{
		HELI_AT_START,
		HELI_PATHING,
		HELI_SPOTTED_ON_PATH,
		HELI_MOVING_TO_SEE,
		HELI_SPOTTED_OFF_PATH,
		HELI_MOVING_TO_PATH,
		HELI_FLEEING,
		HELI_DEAD
	};

	void Activate(bool i_activate);
	void SetAnchor(Vector3 i_anchor, float i_radius);

private:
	REF_SEND_MESSAGE_HANDLER(MunitionImpact, AiHeli) d_munitionImpactHandler;

	void StateControl();
	CPathFollower *pathFollower;
   Vector3 DecideNewDestination();
	Vector3 SinMod(Vector3 i_pos);
	bool IsInBox();
	void Altitude(float i_deltaSec);
	bool AvatarInSight();
	bool CordCheck(CActor* targetActor);
	void FollowTarget();
	void TurnToTarget(float i_frameSec);
	void TurnToFoward(float i_frameSec);
	void MoveToExit();
	void MoveAlongPath();
	void StopOnPath();
	float DistanceToTargetXZ2rd();
	void RemoveFromPath();
	void RunState(float i_frameSec);
	CActor* GetHeliTarget() {return g_players.GetPlayer(0);}
	Vector3 DeviseVelocity();

	int heliState;
	Vector3 destination;
	Vector3 startPosition;
	float sinTime;
	float sinFactor;
	float altitude;
	Vector3 anchor;
	float tether;
	bool inSight;
	float sightTimer;
	Vector3 lastSeenPosition;
	float rotation;
	Vector3 velocityVec;
	int hitCount;
	float timeRise;
	float visualCheckTimer;
	float sendEventTimer;
	float aggroTimeOut;
	SoundHandle heliSoundHandle;
};

#endif // AI_HELI_H



