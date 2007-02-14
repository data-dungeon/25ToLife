#ifndef AI_BEHAVIOR_H
#define AI_BEHAVIOR_H

#include "components/Ai/AiPerception.h"
#include "components/Ai/AiWeaponControl.h"
#include "components/Ai/AiPsyche.h"
#include "components/Ai/AiNavigation.h"
#include "components/Ai/AiBodyControl.h"
#include "components/Ai/AiCombatStrategy.h"
#include "Components/InteractionMonitor.h"
#include "avatar/taunt.h"
#include "components/affliction.h"
#include "components/Say.h"

class AiBehavior : public CActorComponent, public InteractionIF
{
public:
	AiBehavior( CCompActor &i_actor );
	~AiBehavior();

	const char *ComponentName( void ) { return "AiBehavior"; }
	
	void Initialize(void);
	void Reset(void);
	virtual void BeginFrame();
	int GetState(){return aiState;}
	void SetDesiredState(int i_state);
	int GetType(){return aiType;}
	void SetType(int type){aiType = type;}
	int GetAfflication(){return m_affliction;}
	void ActorInMyWay(CActor *i_actorInWay);
	void PathfollowerStop();

	void EnterState_DoJob(void);
	void RunState_DoJob(float frameSec);
	void ExitState_DoJob(void);
	void EnterState_Watch(void);
	void RunState_Watch(float frameSec);
	void ExitState_Watch(void);
	void EnterState_Flee(void);
	void RunState_Flee(float frameSec);
	void ExitState_Flee(void);
	void EnterState_Cower(void);
	void RunState_Cower(float frameSec);
	void ExitState_Cower(void);
	void EnterState_Fight(void);
	void RunState_Fight(float frameSec);
	void ExitState_Fight(void);
	void EnterState_Hostage(void);
	void RunState_Hostage(float frameSec);
	void ExitState_Hostage(void);
	void EnterState_Dead(void);
	void RunState_Dead(float frameSec);
	void ExitState_Dead(void);
	void EnterState_Sitting(void);
	void RunState_Sitting(float frameSec);
	void ExitState_Sitting(void);
	void EnterState_Stunned(void);
	void RunState_Stunned(float frameSec);
	void ExitState_Stunned(void);
	void EnterState_OnFire(void);
	void RunState_OnFire(float frameSec);
	void ExitState_OnFire(void);
	void EnterState_Smoked(void);
	void RunState_Smoked(float frameSec);
	void ExitState_Smoked(void);
	void EnterState_Vaulting(void);
	void RunState_Vaulting(float frameSec);
	void ExitState_Vaulting(void);
	void EnterState_HoldingFire();
	void RunState_HoldingFire(float frameSec);
	void ExitState_HoldingFire();
	int isGang;
	int npcCount;
	CInterface *GetInterface( uint i_id );
	void SetTauntTimer(float time){tauntTimer = time;}

	typedef enum
	{
		STATE_DO_JOB,
		STATE_WATCH,
		STATE_FLEE,
		STATE_COWER,
		STATE_FIGHT,
		STATE_HOSTAGE,
		STATE_SITTING,
		STATE_STUNNED,
		STATE_ON_FIRE,
		STATE_VAULTING,
		STATE_DEAD,
		STATE_HOLDING_FIRE,
		STATE_SMOKED,
	} AI_BEHAVIOR_STATES;

	typedef enum
	{
		AI_STATE_ENTERING,
		AI_STATE_RUNNING,
		AI_STATE_EXITING
	} AI_STATE_WITHIN_STATE;

	typedef enum
	{
		AI_UNTYPED,
		AI_PATHFOLLOWER,
		AI_STATIONARY,
		AI_DANCER,
		AI_DJ,
		AI_COMBATANT,
		AI_BAMBI
	} AI_TYPES;

	typedef enum
	{
		FIGHTSTATE_IDLE,
		FIGHTSTATE_ALERTED,
		FIGHTSTATE_FIGHTING,
		FIGHTSTATE_LOSS_CONTACT
	} FIGHT_STATES;

	typedef enum
	{
		OBJNPCTEAM_ENEMY,
		OBJNPCTEAM_FRIENDLY,
		OBJNPCTEAM_NEUTRAL,
		OBJNPCTEAM_ENEMY2,
		OBJNPCTEAM_AMBIENT
	} TEAM_NUMBERS_FROM_OBJATTR;

	InteractionMonitor::InteractionType QueryInteractionType( CCompActor *whoWantsToKnow );

private:
	void StateControl();
	void StatePaperWork();
	void Targeting(int type);
	void FaceStimulus();
	void Aiming(float frameSec);
	void SetDesiredStateToHighestWant();
	void RunState(float frameSec);
	void ProcessHandsUp(float frameSec);
	bool RangeCulling();
	void TauntBehavior(float frameSec);
	void BeginAfflication();
	void Respawn();

	int aiState;
	int stateWithinState;
	int desiredState;
	float normalPathSpeed;
	float stateTimer;
	float stateControlTime;
	bool handsup;
	float handsupTimer;
	bool runWhenYouCan;
	int aiType;
	float tauntTimer;
	float varianceTimer;  //this is so that two bots don't have syncronized reactions
	float rangeSquared;
	bool m_startedAction;
	int m_fightState;
	int m_affliction;
	float m_afflictionDelay;
	float vaultTimer;
	Vector3 hostageAnchor;
   
	AiBodyControl *aiBodyControl;
	AiPsyche *aiPsyche;
	AiPerception *aiPerception;
	AiWeaponControl *aiWeaponControl;
	PlayerStats *pStats;
	CPathFollower *pathFollower;
	AiNavigation *aiNavigation;
	AiCombatStrategy *aiCombatStrategy;
	Afflictions *afflictions;
	Say *sayComponent;

	ActorHandle targetActorHandle;
	int participantId;
	int sightCheckDelay;
	bool timeout;
	float introAnimTime;
	float freeUseTimer;
	float terminatorTimer;

	ActorHandle hostageTakerHandle;
	bool canBeHostage;
	int playVoice;

	//events
	REF_SEND_MESSAGE_HANDLER(RespawnNPC, AiBehavior) d_respawnNPCHandler;
	REF_SEND_MESSAGE_HANDLER(ManDown, AiBehavior) d_manDownHandler;
	REF_SEND_MESSAGE_HANDLER(NpcDown, AiBehavior) d_npcDownHandler;
	REF_BASIC_MESSAGE_HANDLER(HostageTaker, MESSAGE_ACTOR_HANDLE, AiBehavior) d_hostageTaker;
	REF_SEND_MESSAGE_HANDLER(Hostage, AiBehavior) d_hostage;
	REF_SEND_MESSAGE_HANDLER(HostageGrabbed, AiBehavior) d_hostageGrabbedHandler;
	REF_SEND_MESSAGE_HANDLER(HostageGoneFar, AiBehavior) d_hostageGoneFarHandler;
	REF_SEND_MESSAGE_HANDLER(startaffliction, AiBehavior) d_startaffliction;
	REF_SEND_MESSAGE_HANDLER(endaffliction, AiBehavior) d_endaffliction;
	REF_SEND_MESSAGE_HANDLER(KillInside, AiBehavior) d_killInside;
	REF_SEND_MESSAGE_HANDLER(RemoveFromGame, AiBehavior) d_RemoveFromGame;
	REF_SEND_MESSAGE_HANDLER(NPCTimeout, AiBehavior) d_timeout;
	REF_SEND_MESSAGE_HANDLER(NPCTimein, AiBehavior) d_timein;
	REF_SEND_MESSAGE_HANDLER(VaultTrigger, AiBehavior) d_vaultTriggerHandler;
	REF_SEND_MESSAGE_HANDLER(JumpHere, AiBehavior) d_jumpHereHandler;
	REF_SEND_MESSAGE_HANDLER(Trespass, AiBehavior) d_trespassHandler;
	REF_SEND_MESSAGE_HANDLER(PlayerSpawning, AiBehavior) d_playerSpawningHandler;
	REF_SEND_MESSAGE_HANDLER(EnemyGunfire, AiBehavior) d_enemyGunfireHandler;
	REF_SEND_MESSAGE_HANDLER(NpcSpawned, AiBehavior) d_npcSpawnedHandler;
	REF_SEND_MESSAGE_HANDLER(NpcDied, AiBehavior) d_npcDiedHandler;
	REF_SEND_MESSAGE_HANDLER(StartCinematic, AiBehavior) startCinematicHandler;
	REF_SEND_MESSAGE_HANDLER(EndCinematic, AiBehavior) endCinematicHandler;
	REF_SEND_MESSAGE_HANDLER(ApproveSound, AiBehavior) approveSoundHandler;
	REF_SEND_MESSAGE_HANDLER(KilledHostage, AiBehavior) killedHostageHandler;
	REF_SEND_MESSAGE_HANDLER(HostageSaved, AiBehavior) hostageSavedHandler;
	REF_SEND_MESSAGE_HANDLER(Perform, AiBehavior) performHandler;
	REF_SEND_MESSAGE_HANDLER(CheckpointReset, AiBehavior) resetCheckpointHandler;
	REF_SEND_MESSAGE_HANDLER(CheckpointSet, AiBehavior) setCheckpointHandler;
	//REF_BASIC_MESSAGE_HANDLER( CSetCheckpoint , MESSAGE_VALUE , SecondaryObjective ) d_setCheckpointHandler;
	
};

#endif // AI_BEHAVIOR_H
