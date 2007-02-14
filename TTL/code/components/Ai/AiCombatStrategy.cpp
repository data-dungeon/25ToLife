#include "TTLPCH.h"
#include "components/Ai/AiCombatStrategy.h"
#include "components/Ai/AiPsyche.h"
#include "components/Ai/AiBodyControl.h"
#include "Components/ComponentCreator.h"
#include "avatar/pstat.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(AiCombatStrategy, AiCombatStrategy, COMPONENT_NEUTRAL_PRIORITY);

#define DECISION_INTERVAL 1.0f
#define START_GUARD_TIME 50.0f
#define DISTANCE_CLOSE_AMOUNT 4.0f
#define DISTANCE_NORMAL_AMOUNT 20.0f
#define DISTANCE_FAR_AMOUNT  40.0f
#define DISTANCE_CULL_AMOUNT 100.0f

//============================================================================
AiCombatStrategy::AiCombatStrategy( CCompActor &i_actor ) :
CActorComponent( i_actor )
{
}

//============================================================================
AiCombatStrategy::~AiCombatStrategy()
{
}

//============================================================================
void AiCombatStrategy::Initialize( void )
{
	for(int i = 0; i < NUMBER_OF_TACTICS; i++)
	{
		tacticEnabled[i] = false;
	}

	d_createHomePointHandler.Initialize("CreateHomePoint",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_forceChargeHandler.Initialize("ForceCharge",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	d_forceRetreatHandler.Initialize("ForceRetreat",
		MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);

	aiNavigation = (AiNavigation*) actor.GetComponentByName( "AiNavigation" );
	ASSERT(aiNavigation);
	aiPerception = (AiPerception*) actor.GetComponentByName( "AiPerception" );
	ASSERT(aiPerception);
	aiPsyche = (AiPsyche*) actor.GetComponentByName( "AiPsyche" );
	ASSERT(aiPsyche);
	aiPsyche->SetupBaseWants();
	//temp
	if(aiPsyche->GetAiType() <= 0 || aiPsyche->GetAiType() > AiPsyche::AI_TYPE_COUNT)
		LoadVars();
	aiWeaponControl = (AiWeaponControl*) actor.GetComponentByName( "AiWeaponControl" );
	ASSERT(aiWeaponControl);
	Reset();
}

//============================================================================
void AiCombatStrategy::Reset( void )
{
	tactic = TACTIC_GUARD;
	mode = STRATEGY_MODE_NORMAL;
	tacticLocked = false;
	suggestedTactic = tactic;
	playerHealth = 1.0f;
	decisionTimer = g_random.InRange(DECISION_INTERVAL, DECISION_INTERVAL + 1.0f);
	targetsHealth = 1.0f;
	targetsDistance = 100.0f;
	DataGrab();
	distanceCondition = DISTANCE_CULL;
	assetCondition = ASSET_NORMAL;
}

//============================================================================
//============================================================================
void AiCombatStrategy::Update(float frameSec)
{
	Math::MoveToZero(&decisionTimer,frameSec);
	if(decisionTimer == 0.0f)
	{
		DataGrab();
		ComputeStrategy();
		decisionTimer = DECISION_INTERVAL;
		ProcessTactic(frameSec);
	}
}
//============================================================================
void AiCombatStrategy::DataGrab()
{
	float targetPercentHealth = 0.0f;
	float targetDistance;

	//THIS NEEDS TO BE CHANGED IF MORE THAN ONE HUMAN PLAYER
	CActor* avatar = g_players.GetPlayer(0);
	if (!avatar)
	{
		targetPercentHealth = 1.0f;
		targetDistance = 100000.0f;
	}
	else
	{
		ASSERT( avatar->IsA( CCompActor::ClassType()  ));
		PlayerStats* pStats = (PlayerStats*)avatar->GetInterface(PlayerStats::GetID());
		if (pStats)
			targetPercentHealth = pStats->GetHealthPercentage();

		Vector3 toTarget = avatar->GetBodyInWorld() - actor.GetBodyInWorld();
		targetsDistance = toTarget.Length();
	}

	float myPercentHealth;
	PlayerStats* pStats = (PlayerStats*)actor.GetInterface(PlayerStats::GetID());
	if (pStats)
		myPercentHealth = pStats->GetHealthPercentage(); 
	else
		myPercentHealth = 1.0f;

	//determine asset ratio
	assetCondition = ASSET_NORMAL;
	float assetRatio = 2.0f;
	if(targetPercentHealth > 0.0f)
		assetRatio = myPercentHealth/targetPercentHealth;
	if(assetRatio < 0.7f)
		assetCondition = ASSET_DISADVANTAGE;
	else if(assetRatio > 1.3f)
		assetCondition = ASSET_ADVANTAGE;

	//set distance condition
	distanceCondition = DISTANCE_NORMAL;
	if(targetsDistance < DISTANCE_CLOSE_AMOUNT)
		distanceCondition = DISTANCE_CLOSE;
	else if(targetsDistance > DISTANCE_FAR_AMOUNT)
		distanceCondition = DISTANCE_FAR;
	else if(targetsDistance > DISTANCE_CULL_AMOUNT)
		distanceCondition = DISTANCE_CULL;
}
//============================================================================
void AiCombatStrategy::ComputeStrategy()
{
	if(tacticEnabled[TACTIC_FOLLOW]) //must be a buddy that saw the avatar
	{
		aiNavigation->SetMode(AiNavigation::NAVMODE_FOLLOW_AVATAR);
		return;
	}

	//if should be idle, do that
	if(aiPerception->GetLastSeenOrHeardTime() == 0.0f || distanceCondition == DISTANCE_CULL || g_timer.GetStartSec() - aiPerception->GetLastSeenOrHeardTime() > START_GUARD_TIME)  //or lost contact for a long time
	{
		//only set this once
		if(tacticEnabled[TACTIC_WANDER] && tactic != TACTIC_GUARD)
		{
			aiNavigation->SetWanderCenter(actor.GetBodyInWorld());
			aiNavigation->SetMode(AiNavigation::NAVMODE_WANDER);
		}
		SetStrategyMode(STRATEGY_MODE_NORMAL);
		SetTactic(TACTIC_GUARD);
		return;
	}

	//must have been seen, run home...once home this tactic is disabled
	if(tacticEnabled[TACTIC_RUN_HOME])
	{
		aiNavigation->SetMode(AiNavigation::NAVMODE_RUN_HOME);
		return;
	}

	// melee
	if(aiWeaponControl->GetSelectedClass() == WEAPONCLASS_MELEE)
	{
		AiBodyControl *aiBodyControl = (AiBodyControl *)actor.GetComponentByName( "AiBodyControl" );
		if(aiBodyControl)
			aiBodyControl->SetCrouchEnabled(false);
		SetTactic(TACTIC_MELEE);
		return;
	}

	switch(assetCondition)
	{
	case ASSET_NORMAL:
		SetStrategyMode(STRATEGY_MODE_NORMAL);
		break;
	case ASSET_DISADVANTAGE:
		SetStrategyMode(STRATEGY_MODE_DEFENSIVE);
		break;
	case ASSET_ADVANTAGE:
		SetStrategyMode(STRATEGY_MODE_AGGRESSIVE);
		break;
	}

	//determine tactic  *list most desirable to least desirable per mode
	switch(mode)
	{
	case STRATEGY_MODE_NORMAL:
		if(tacticEnabled[TACTIC_NORMAL])
			tactic = TACTIC_NORMAL;
		else if(tacticEnabled[TACTIC_COVER])
			tactic = TACTIC_COVER;
		else if(tacticEnabled[TACTIC_DODGY])
			tactic = TACTIC_DODGY;
		else if(tacticEnabled[TACTIC_SNEAKY])
			tactic = TACTIC_SNEAKY;
		else if(tacticEnabled[TACTIC_CHARGE])
			tactic = TACTIC_CHARGE;
		else if(tacticEnabled[TACTIC_RETREAT])
			tactic = TACTIC_RETREAT;
		else if(tacticEnabled[TACTIC_HIDE])
			tactic = TACTIC_HIDE;
		break;
	case STRATEGY_MODE_AGGRESSIVE:
		if(tacticEnabled[TACTIC_CHARGE])
			tactic = TACTIC_CHARGE;
		else if(tacticEnabled[TACTIC_SNEAKY])
			tactic = TACTIC_SNEAKY;
		else if(tacticEnabled[TACTIC_NORMAL])
			tactic = TACTIC_NORMAL;
		else if(tacticEnabled[TACTIC_DODGY])
			tactic = TACTIC_DODGY;
		else if(tacticEnabled[TACTIC_COVER])
			tactic = TACTIC_COVER;
		else if(tacticEnabled[TACTIC_RETREAT])
			tactic = TACTIC_RETREAT;
		else if(tacticEnabled[TACTIC_HIDE])
			tactic = TACTIC_HIDE;
		break;
	case STRATEGY_MODE_DEFENSIVE:
		if(tacticEnabled[TACTIC_HIDE])
			tactic = TACTIC_HIDE;
		else if(tacticEnabled[TACTIC_RETREAT])
			tactic = TACTIC_RETREAT;
		else if(tacticEnabled[TACTIC_COVER])
			tactic = TACTIC_COVER;
		else if(tacticEnabled[TACTIC_DODGY])
			tactic = TACTIC_DODGY;
		else if(tacticEnabled[TACTIC_SNEAKY])
			tactic = TACTIC_SNEAKY;
		else if(tacticEnabled[TACTIC_NORMAL])
			tactic = TACTIC_NORMAL;
		else if(tacticEnabled[TACTIC_CHARGE])
			tactic = TACTIC_CHARGE;
		break;
	}
}
//============================================================================
void AiCombatStrategy::ProcessTactic(float frameSec)
{
	switch(tactic)
	{
	case TACTIC_GUARD:
		break;
	case TACTIC_NORMAL:
		aiNavigation->SetMode(AiNavigation::NAVMODE_DODGY); //temporary....
		break;
	case TACTIC_DODGY:
		aiNavigation->SetMode(AiNavigation::NAVMODE_DODGY); 
		break;
	case TACTIC_CHARGE:
		aiNavigation->SetMode(AiNavigation::NAVMODE_MOVE_TO_TARGET);
		break;
	case TACTIC_SNEAKY:
		aiNavigation->SetMode(AiNavigation::NAVMODE_DODGY); //temporary....
		break;
	case TACTIC_COVER:
		aiNavigation->SetMode(AiNavigation::NAVMODE_ATTACK_FROM_COVER);
		break;
	case TACTIC_HIDE:
		aiNavigation->SetMode(AiNavigation::NAVMODE_RETREAT); //temporary....
		break;
	case TACTIC_RETREAT:
		aiNavigation->SetMode(AiNavigation::NAVMODE_RETREAT);
		break;
	case TACTIC_MELEE:
		aiNavigation->SetMode(AiNavigation::NAVMODE_MELEE);
		break;
	}
}
//============================================================================
void AiCombatStrategy::LoadVars()
{
	//must happen after aiPsyche

	tacticEnabled[TACTIC_GUARD] = true; //you can always guard
	if(aiPsyche->m_aiParams->tactic_normal)
		tacticEnabled[TACTIC_NORMAL] = true;
	if(aiPsyche->m_aiParams->tactic_dodgy)
		tacticEnabled[TACTIC_DODGY] = true;
	if(aiPsyche->m_aiParams->tactic_charge)
		tacticEnabled[TACTIC_CHARGE] = true;
	if(aiPsyche->m_aiParams->tactic_sneaky)
		tacticEnabled[TACTIC_SNEAKY] = true;
	if(aiPsyche->m_aiParams->tactic_cover)
		tacticEnabled[TACTIC_COVER] = true;
	if(aiPsyche->m_aiParams->tactic_hide)
		tacticEnabled[TACTIC_HIDE] = true;
	if(aiPsyche->m_aiParams->tactic_retreat)
		tacticEnabled[TACTIC_RETREAT] = true;
	if(aiPsyche->m_aiParams->tactic_wander)
		tacticEnabled[TACTIC_WANDER] = true;
	if(aiPsyche->m_aiParams->tactic_follow)
		tacticEnabled[TACTIC_FOLLOW] = true;
}
//============================================================================
void AiCombatStrategy::CreateHomePoint::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	d_ref->tacticEnabled[TACTIC_RUN_HOME] = true;
}
//============================================================================
void AiCombatStrategy::ForceCharge::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	d_ref->SetSuicidal();
}
//============================================================================
void AiCombatStrategy::ForceRetreat::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	if(d_ref->tacticEnabled[TACTIC_FOLLOW]) //buddy don't go
		return;

	for(int i = 0; i < NUMBER_OF_TACTICS; i++)
	{
		d_ref->tacticEnabled[i] = false;
	}

	d_ref->tacticEnabled[TACTIC_RETREAT] = true;
	d_ref->decisionTimer = 0.0f;
	if(d_ref->aiNavigation)
		d_ref->aiNavigation->SetGoalCheckTimer(0.0f);
}
//============================================================================
void AiCombatStrategy::SetSuicidal()
{
	if(tacticEnabled[TACTIC_FOLLOW]) //buddy don't go
		return;

   for(int i = 0; i < NUMBER_OF_TACTICS; i++)
	{
		tacticEnabled[i] = false;
	}

	tacticEnabled[TACTIC_CHARGE] = true;
	decisionTimer = 0.0f;
	if(aiNavigation)
		aiNavigation->SetGoalCheckTimer(0.0f);
}
