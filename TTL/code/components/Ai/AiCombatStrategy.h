#ifndef AI_COMBATSTRATEGY_H
#define AI_COMBATSTRATEGY_H

#include "Components/Component.h"
#include "components/Ai/AiNavigation.h"
#include "components/Ai/AiPerception.h"
#include "components/Ai/AiPsyche.h"
#include "components/Ai/AiWeaponControl.h"

class AiCombatStrategy : public CActorComponent
{
public:
	AiCombatStrategy( CCompActor &i_actor );
	~AiCombatStrategy();

	const char *ComponentName( void ) { return "AiCombatStrategy"; }
	void Initialize(void);
	void Reset(void);
	void Update(float frameSec);
	int GetStrategyMode(){return mode;}
	void SetStrategyMode(int iMode) {mode = iMode;}
	int GetTactic() {return tactic;} 
	void SetTactic(int iTactic){tactic = iTactic;}
	void InfluenceTactic(int iTactic, bool forceIt = false, bool lockIt = false);
	void SetLock(bool lockStrategy);
	void SetTeamState(int state){teamState = state;}
	void EnableTactic(int iTactic, bool enable) {tacticEnabled[iTactic] = enable;}
	void SetDecisionTimer(float iTime){decisionTimer = iTime;}
	void SetSuicidal();

	enum STRATEGY_MODES
	{
		STRATEGY_MODE_AGGRESSIVE,
		STRATEGY_MODE_NORMAL,
		STRATEGY_MODE_DEFENSIVE
	};

	enum TACTICS
	{
		TACTIC_GUARD,     //this is an idle, pretty much
		TACTIC_NORMAL,		//move to firing position, if cover near, take it, keep firing even if uncovered
		TACTIC_DODGY,		//fire, hide, fire... use side-to-side strafing if not near cover
		TACTIC_CHARGE,		//move to firing point and open fire, chase
		TACTIC_SNEAKY,		//flank move
		TACTIC_COVER,		//use firing cover until exposed, then move to other cover
		TACTIC_HIDE,		//stay behind cover until discovered, fire a volley, then switch to RETREAT
		TACTIC_RETREAT,	//retreat a certain distance and take cover
		TACTIC_WANDER,		//simple idle movement, snipers won't want to do this
		TACTIC_RUN_HOME,  //runners run home
		TACTIC_FOLLOW,    //follows the avatar, only a buddy uses this
		TACTIC_MELEE,

		NUMBER_OF_TACTICS
	};

	enum ASSET_CONDITIONS
	{
		ASSET_NORMAL,
		ASSET_ADVANTAGE,
		ASSET_DISADVANTAGE
	};

	enum DISTANCE_CONDITIONS
	{
		DISTANCE_CLOSE,
		DISTANCE_NORMAL,
		DISTANCE_FAR,
		DISTANCE_CULL
	};

private:
	REF_SEND_MESSAGE_HANDLER(CreateHomePoint, AiCombatStrategy) d_createHomePointHandler;
	REF_SEND_MESSAGE_HANDLER(ForceCharge, AiCombatStrategy) d_forceChargeHandler;
	REF_SEND_MESSAGE_HANDLER(ForceRetreat, AiCombatStrategy) d_forceRetreatHandler;

	void DataGrab();
	void ComputeStrategy();
	void ProcessTactic(float frameSec);
	void LoadVars();

	AiNavigation *aiNavigation;
	AiPerception *aiPerception;
	AiPsyche *aiPsyche;
	AiWeaponControl *aiWeaponControl;
	int mode;
	int tactic;
	bool tacticEnabled[NUMBER_OF_TACTICS];
	bool tacticLocked;
	int suggestedTactic;
	float health;
	float playerHealth;
	float decisionTimer;
	int assetCondition;
	int distanceCondition;
	int teamState;

	//known data
	float targetsHealth; //I know I shot you...we can change this later so that self inflicted wounds don't count
	float targetsDistance; //last known distance
};

#endif // AI_COMBATSTRATEGY_H