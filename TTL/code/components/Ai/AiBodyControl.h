#ifndef AI_BODYCONTROL_H
#define AI_BODYCONTROL_H

#include "Components/Component.h"
#include "Interfaces/MoveIF.h"
#include "Interfaces/ViewIF.h"
#include "components/Ai/AiPerception.h"
#include "components/Ai/AiNavigation.h"
#include "avatar/pstat.h"

class AiBodyControl : public CActorComponent
{
public:
	AiBodyControl( CCompActor &i_actor );
	~AiBodyControl();
	const char *ComponentName( void ) { return "AiBodyControl"; }
	void Initialize(void);
	void Reset(void);
	void Update(float frameSec);
	void SetBodyControlState(int state);
	void UpdateLookPosition(float frameSec);
	void TurnToPoint(Vector3 location);
	void LookAtLastKnownPosition(bool turnTo = true, bool force = false);
	void LookAtPosition(Vector3 position, bool turnTo = true, bool force = false);
	void LookAtScanning(float speed = 1.0f, bool force = false);
	void LookAtActor(CActor *pActor, bool turnTo = true, bool force = false);
	void SetCrouchEnabled(bool canCrouch);
	void UpdateCrouchPosition( float frameSec );
	int PeekToSee(int peekType, int oldPeekType);
	void InroAnim();

	enum BODYCONTROL_STATES
	{
		BODYCONTROL_DISABLED,
		BODYCONTROL_AMBIENT,
		BODYCONTROL_COMBAT
	};

	enum CROUCH_TYPES
	{
		CROUCH_DEFAULT,
		CROUCH_COVER_HALF,
		CROUCH_COVER_STANDING,
		CROUCH_SCARED,
		CROUCH_TO_AIM
	};

	enum LOOK_TYPES
	{
		LOOK_UNLOCKED,
		LOOK_LASTKNOWN,
		LOOK_INSIGHT,
		LOOK_SCANNING,
		LOOK_SCAN_QUICK
	};

private:
	void Crouch(int type, float time);
	void Uncrouch();
	REF_SEND_MESSAGE_HANDLER(EnemyGunfire, AiBodyControl) d_enemyGunfireHandler;

	CMoveIF *moveIF;
	ViewIF *viewIF;
	AiPerception *aiPerception;
	AiNavigation *aiNavigation;
	PlayerStats *pStats;
	int bodyControlState;
	Vector3 lookPosition;
	Vector3 lastKnownTargetLocation;
	float scanLeft;
	float scanMax;
	float scanSpeed;
	bool scanSwitchReady;
	float scanPauseTimer;
	float scanLengthTimer;
	bool crouchEnabled;
	bool crouching;
	float crouchTimer;
	float crouchInterludeTimer;
	int crouchType;
	float lookAtTimer;
	Vector3 lastLookAtPos;
	int lookType;
	float startScanningTime;
	int stationaryCover;
	float timeAlive;
	bool crouchNextUpdate;
};

#endif // AI_BODYCONTROL_H



