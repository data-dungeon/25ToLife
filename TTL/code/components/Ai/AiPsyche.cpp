#include "TTLPCH.h"
#include "components/Ai/AiPsyche.h"
#include "components/Ai/AiPerception.h"
#include "Components/ComponentCreator.h"
#include "components/Ai/AiCombatStrategy.h"

#define MIN_RAMP_DOWN 0.4f //additional stimulus inputs will decrease in impact
#define WATCH_TO_FLEE_FACTOR 0.7f //flee gets a portion of want to watch
#define GUN_POINT_TIMER_TIME 1.0f

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(AiPsyche, AiPsyche, COMPONENT_NEUTRAL_PRIORITY);

//============================================================================
AiPsyche::AiPsyche( CCompActor &i_actor ) :
CActorComponent( i_actor )
{
}

//============================================================================
AiPsyche::~AiPsyche()
{
}

//============================================================================
void AiPsyche::Initialize( void )
{
	aiType = AI_UNDEFINED;

	for(int i=0; i < NUMBER_OF_WANTS; i++)
	{
		rawWants[i] = 0.0f;
		baseWant.sensitivity[i] = 0.0f;
		baseWant.predisp[i] = 0.0f;
	}
	rawWants[WANT_DO_JOB] = 1.0f;
	baseWant.predisp[WANT_DO_JOB] = 1.0f;
	lastHighestWant = WANT_DO_JOB;

	for(int j=0; j < NUMBER_OF_ACCESSORIES; j++)
	{
		baseWant.accessories[j] = 0.0f;
		baseWant.accessorySensitivity[j] = 0.0f;
	}

	d_atGunPointHandler.Initialize("AtGunPoint", MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER, actor.GetMessageOwner(), this);
	SetupBaseWants();
	//LoadVars();
	Reset();
}

//============================================================================
void AiPsyche::Reset( void )
{
	gunPointTimer = 0.0f;

	want = baseWant;
	for(int i=0; i < NUMBER_OF_WANTS; i++)
		rawWants[i] = baseWant.predisp[i];

	NormalizeWants();
}

//============================================================================
void AiPsyche::NormalizeWants( void )
{
	float totalWants = 0.0f;

	for(int i=0; i < NUMBER_OF_WANTS; i++)
	{
		totalWants += rawWants[i];
	}

	if(totalWants <= 0.0f)
	{
		totalWants = 1.0f; //error
		//ASSERT(false);
	}

	for(int j=0; j < NUMBER_OF_WANTS; j++)
	{
		normalizedWants[j] = rawWants[j] / totalWants;
	}
}
//============================================================================
void AiPsyche::Update( float frameSec  )
{
	ProcessDegradation(frameSec);

	Math::MoveToZero(&gunPointTimer,frameSec);
}
//============================================================================
float AiPsyche::GetWant(int aiWant)
{
	if(aiWant < 0 && aiWant >= NUMBER_OF_WANTS)
		return 0.0f;

	return normalizedWants[aiWant];
}
//============================================================================
int AiPsyche::GetHighestWant( void )
{
	int highest = 0;
	float amount = 0.0f;

	for(int i=0; i < NUMBER_OF_WANTS; i++)
	{
		if(normalizedWants[i] > amount)
		{
			highest = i;
			amount = normalizedWants[i];
		}
	}

	//manage linked wants
	//////////////////////////////////////////////
	if(highest == WANT_TO_WATCH && normalizedWants[WANT_TO_FLEE] > normalizedWants[WANT_TO_WATCH] * WATCH_TO_FLEE_FACTOR)
		highest = WANT_TO_FLEE;

	//dbgPrint("wants %f %f %f\n", normalizedWants[0],normalizedWants[1],normalizedWants[4]);

	lastHighestWant = highest;

	return highest;
}
//============================================================================
void AiPsyche::ModifyWant(int aiWant, float amount)
{
	if(aiWant < 0 && aiWant >= NUMBER_OF_WANTS)
		return;

	rawWants[aiWant] += (amount * want.sensitivity[aiWant]) * (1.0f - normalizedWants[aiWant]);
	NormalizeWants();

	//dbgPrint("wants %f %f %f\n", normalizedWants[0],normalizedWants[1],normalizedWants[2]);
}
//============================================================================
void AiPsyche::SetWant(int aiWant, float amount)
{
	if(aiWant < 0 && aiWant >= NUMBER_OF_WANTS)
		return;

	rawWants[aiWant] = amount;
	NormalizeWants();
}
//============================================================================
float AiPsyche::GetWantSensitivity(int aiWant)
{
	if(aiWant < 0 && aiWant >= NUMBER_OF_WANTS)
		return 0.0f;

	return want.sensitivity[aiWant];
}
//============================================================================
void AiPsyche::SetWantSensitivity(int aiWant, float amount)
{
	if(aiWant < 0 && aiWant >= NUMBER_OF_WANTS)
		return;

	want.sensitivity[aiWant] = amount;
}
//============================================================================
void AiPsyche::ProcessDegradation(float frameSec)
{
	//note: return rate as made as a single value to keep it simple
	//behaviors will probably get whacky if the artists start messing with individual return values

	//degrade value is % reduction per second
	for(int i=1; i < NUMBER_OF_WANTS; i++)  //DO JOB NEVER DEGRADES
	{
		rawWants[i] = rawWants[i] - (rawWants[i] * want.returnRate * frameSec);

		if(rawWants[i] < want.predisp[i])
			rawWants[i] = want.predisp[i];
		else if(rawWants[i] < 0.0f)
			rawWants[i] = 0.0f;
	}
	NormalizeWants();

	for(int j=1; j < NUMBER_OF_ACCESSORIES; j++)  //DEFAULT NEVER DEGRADES
	{
		want.accessories[j] = want.accessories[j] - (want.accessories[j] * want.returnRate * frameSec);
	}
}
//============================================================================
float AiPsyche::GetWantAccessory(int wantAccessory)
{
	if(wantAccessory < 0 && wantAccessory >= NUMBER_OF_ACCESSORIES)
		return 0.0f;

	return want.accessories[wantAccessory];
}
//============================================================================
void AiPsyche::SetWantAccessory(int wantAccessory, float amount)
{
	if(wantAccessory < 0 && wantAccessory >= NUMBER_OF_ACCESSORIES)
		return;

	want.accessories[wantAccessory] = amount;
}
//============================================================================
void AiPsyche::ModifyWantAccessory(int accessory, float amount)
{
	if(accessory < 0 && accessory >= NUMBER_OF_ACCESSORIES)
		return;

	want.accessories[accessory] += (amount * want.accessorySensitivity[accessory]);
	//dbgPrint("handsup2 %f\n", want.accessories[WANT_ACCESSORY_HANDSUP]);
}
//============================================================================
void AiPsyche::StimulusInput(int input, float amount)
{
	switch(input)
	{
		case STIMULUS_PAIN:
			ModifyWant(AiPsyche::WANT_TO_FLEE,100.0f * amount); //force it
			ModifyWant(AiPsyche::WANT_TO_FIGHT,100.0f * amount); //force it
			ModifyWant(AiPsyche::WANT_TO_COWER,10.0f * amount); //force it, unless they can fight or flee
			ModifyWantAccessory(AiPsyche::WANT_ACCESSORY_HANDSUP,1.0f * amount); //don;t force it
			break;
		case STIMULUS_FRIENDLY_PAIN:
			ModifyWant(AiPsyche::WANT_TO_FLEE,100.0f * amount); //force it
			ModifyWant(AiPsyche::WANT_TO_FIGHT,100.0f * amount); //force it
			ModifyWant(AiPsyche::WANT_TO_COWER,10.0f * amount); //force it, unless they can fight or flee
			ModifyWantAccessory(AiPsyche::WANT_ACCESSORY_HANDSUP,1.0f * amount); //don;t force it
			break;
		case STIMULUS_ENEMY_PRESENCE:
			ModifyWant(AiPsyche::WANT_TO_WATCH,2.0f * amount);
			ModifyWant(AiPsyche::WANT_TO_FLEE,1.0f * amount);
			ModifyWant(AiPsyche::WANT_TO_COWER,1.0f * amount);
			ModifyWantAccessory(AiPsyche::WANT_ACCESSORY_HANDSUP,4.0f * amount);
			break;
		case STIMULUS_SUCCESS:
			ModifyWant(AiPsyche::WANT_TO_WATCH,-2.0f * amount);
			ModifyWant(AiPsyche::WANT_TO_FLEE,-1.0f * amount);
			ModifyWant(AiPsyche::WANT_TO_FIGHT,1.0f * amount);
			ModifyWant(AiPsyche::WANT_TO_COWER,-1.0f * amount);
			break;
	}

}
//============================================================================

//============================================================================
//============================================================================
//EVENTS
//============================================================================
//============================================================================
void AiPsyche::AtGunPoint::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	ASSERT_PTR(d_ref);

	if(d_ref->gunPointTimer > 0.0f)
		return;

	AiPerception *aiPerception = (AiPerception*)( d_ref->actor.GetComponentByName( "AiPerception" ) );
	if(aiPerception)
	{
		if(aiPerception->VisionCheck() == i_sender)
		{
			d_ref->StimulusInput(AiPsyche::STIMULUS_ENEMY_PRESENCE, 1.0f);
			d_ref->gunPointTimer = GUN_POINT_TIMER_TIME;
		}
	}
}

//============================================================================
void AiPsyche::SetupBaseWants()
{
	//Let's eliminate disk access altogether.
	aiType = actor.Parm().GetInt("aiType");
	//ASSERT(aiType > 0);
	//ASSERT(aiType < AI_TYPE_COUNT);

	baseWant.returnRate = 0.01f;

	//temp
	if(aiType < 1 || aiType >= AI_TYPE_COUNT)
		LoadVars();

	switch(aiType)
	{
		case AI_WATCHER:	
			baseWant.predisp[WANT_DO_JOB] = 0.51f;
			baseWant.predisp[WANT_TO_WATCH] = 0.49f;
			baseWant.sensitivity[WANT_TO_WATCH] = 0.1f;
			baseWant.predisp[WANT_TO_COWER] = 0.0f;
			baseWant.sensitivity[WANT_TO_COWER] = 0.5f;
			baseWant.accessories[WANT_ACCESSORY_HANDSUP] = 0.1f;
			baseWant.accessorySensitivity[WANT_ACCESSORY_HANDSUP] = 2.0f;
			break;
		case AI_WALKER:	
			baseWant.predisp[WANT_DO_JOB] = 0.51f;
			baseWant.predisp[WANT_TO_WATCH] = 0.49f;
			baseWant.sensitivity[WANT_TO_WATCH] = 0.2f;
			baseWant.predisp[WANT_TO_COWER] = 0.0f;
			baseWant.sensitivity[WANT_TO_COWER] = 0.5f;
			baseWant.accessories[WANT_ACCESSORY_HANDSUP] = 0.4f;
			baseWant.accessorySensitivity[WANT_ACCESSORY_HANDSUP] = 0.7f;
			break;
		case AI_RUNNER:	
			baseWant.predisp[WANT_DO_JOB] = 0.51f;
			baseWant.predisp[WANT_TO_FLEE] = 0.2f;
			baseWant.sensitivity[WANT_TO_FLEE] = 0.25f;
			break;
		case AI_HANDSUP:
			baseWant.predisp[WANT_DO_JOB] = 0.51f;
			baseWant.predisp[WANT_TO_WATCH] = 0.49f;
			baseWant.sensitivity[WANT_TO_WATCH] = 0.3f;
			baseWant.accessories[WANT_ACCESSORY_HANDSUP] = 0.1f;
			baseWant.accessorySensitivity[WANT_ACCESSORY_HANDSUP] = 5.0f;
			break;
		case AI_HELDUP:	
			baseWant.predisp[WANT_DO_JOB] = 0.51f;
			baseWant.predisp[WANT_TO_WATCH] = 1.0f;
			baseWant.sensitivity[WANT_TO_WATCH] = 0.1f;
			baseWant.predisp[WANT_TO_COWER] = 0.0f;
			baseWant.sensitivity[WANT_TO_COWER] = 0.4f;
			baseWant.accessories[WANT_ACCESSORY_HANDSUP] = 2.0f;
			baseWant.accessorySensitivity[WANT_ACCESSORY_HANDSUP] = 2.0f;
			break;
		case AI_COWERER:	
			baseWant.predisp[WANT_DO_JOB] = 0.51f;
			baseWant.predisp[WANT_TO_WATCH] = 0.49f;
			baseWant.sensitivity[WANT_TO_WATCH] = 0.15f;
			baseWant.predisp[WANT_TO_COWER] = 0.4f;
			baseWant.sensitivity[WANT_TO_COWER] = 0.4f;
			baseWant.accessories[WANT_ACCESSORY_HANDSUP] = 0.1f;
			baseWant.accessorySensitivity[WANT_ACCESSORY_HANDSUP] = 2.0f;
			break;
		case AI_DANCER:	
			baseWant.predisp[WANT_DO_JOB] = 0.51f;
			baseWant.predisp[WANT_TO_WATCH] = 0.49f;
			baseWant.sensitivity[WANT_TO_WATCH] = 0.1f;
			baseWant.predisp[WANT_TO_COWER] = 0.0f;
			baseWant.sensitivity[WANT_TO_COWER] = 0.5f;
			baseWant.accessories[WANT_ACCESSORY_HANDSUP] = 0.1f;
			baseWant.accessorySensitivity[WANT_ACCESSORY_HANDSUP] = 2.0f;
			break;
		case AI_DJ:	
			baseWant.predisp[WANT_DO_JOB] = 0.51f;
			baseWant.predisp[WANT_TO_WATCH] = 0.49f;
			baseWant.sensitivity[WANT_TO_WATCH] = 0.1f;
			baseWant.predisp[WANT_TO_COWER] = 0.0f;
			baseWant.sensitivity[WANT_TO_COWER] = 0.5f;
			baseWant.accessories[WANT_ACCESSORY_HANDSUP] = 0.1f;
			baseWant.accessorySensitivity[WANT_ACCESSORY_HANDSUP] = 2.0f;
			break;
		case AI_GRUNT:	
			baseWant.predisp[WANT_DO_JOB] = 0.50f;
			baseWant.predisp[WANT_TO_WATCH] = 0.01f;
			baseWant.predisp[WANT_TO_FIGHT] = 0.49f;
			baseWant.sensitivity[WANT_TO_FIGHT] = 0.6f;
			{
				AiCombatStrategy* aiCombatStrategy = (AiCombatStrategy*) actor.GetComponentByName( "AiCombatStrategy" );
				if(aiCombatStrategy)
				{
					aiCombatStrategy->EnableTactic(AiCombatStrategy::TACTIC_CHARGE,true);
					aiCombatStrategy->EnableTactic(AiCombatStrategy::TACTIC_RETREAT,true);
					aiCombatStrategy->EnableTactic(AiCombatStrategy::TACTIC_DODGY,true);
					aiCombatStrategy->EnableTactic(AiCombatStrategy::TACTIC_WANDER,true);
				}
			}
			break;
		case AI_CHARGER:	
			baseWant.predisp[WANT_DO_JOB] = 0.50f;
			baseWant.predisp[WANT_TO_WATCH] = 0.01f;
			baseWant.predisp[WANT_TO_FIGHT] = 0.49f;
			baseWant.sensitivity[WANT_TO_FIGHT] = 0.6f;
			{
				AiCombatStrategy* aiCombatStrategy = (AiCombatStrategy*) actor.GetComponentByName( "AiCombatStrategy" );
				if(aiCombatStrategy)
				{
					aiCombatStrategy->EnableTactic(AiCombatStrategy::TACTIC_CHARGE,true);
					aiCombatStrategy->EnableTactic(AiCombatStrategy::TACTIC_RETREAT,true);
					aiCombatStrategy->EnableTactic(AiCombatStrategy::TACTIC_WANDER,true);
				}
			}
			break;
		case AI_DEFENDER:	
			baseWant.predisp[WANT_DO_JOB] = 0.50f;
			baseWant.predisp[WANT_TO_WATCH] = 0.01f;
			baseWant.predisp[WANT_TO_FIGHT] = 0.49f;
			baseWant.sensitivity[WANT_TO_FIGHT] = 0.6f;
			{
				AiCombatStrategy* aiCombatStrategy = (AiCombatStrategy*) actor.GetComponentByName( "AiCombatStrategy" );
				if(aiCombatStrategy)
				{
					aiCombatStrategy->EnableTactic(AiCombatStrategy::TACTIC_COVER,true);
					aiCombatStrategy->EnableTactic(AiCombatStrategy::TACTIC_WANDER,true);
				}
			}
			break;
		case AI_SNIPER:		
			baseWant.predisp[WANT_DO_JOB] = 0.50f;
			baseWant.predisp[WANT_TO_WATCH] = 0.01f;
			baseWant.predisp[WANT_TO_FIGHT] = 0.49f;
			baseWant.sensitivity[WANT_TO_FIGHT] = 0.6f;
			{
				AiCombatStrategy* aiCombatStrategy = (AiCombatStrategy*) actor.GetComponentByName( "AiCombatStrategy" );
				if(aiCombatStrategy)
				{
					aiCombatStrategy->EnableTactic(AiCombatStrategy::TACTIC_GUARD,true);
					aiCombatStrategy->EnableTactic(AiCombatStrategy::TACTIC_RETREAT,true);
				}
			}
			break;
		case AI_STATIONARY:	
			baseWant.predisp[WANT_DO_JOB] = 0.50f;
			baseWant.predisp[WANT_TO_WATCH] = 0.01f;
			baseWant.predisp[WANT_TO_FIGHT] = 0.49f;
			baseWant.sensitivity[WANT_TO_FIGHT] = 0.6f;
			break;
		case AI_SUICIDAL:	
			baseWant.predisp[WANT_DO_JOB] = 0.50f;
			baseWant.predisp[WANT_TO_WATCH] = 0.01f;
			baseWant.predisp[WANT_TO_FIGHT] = 0.49f;
			baseWant.sensitivity[WANT_TO_FIGHT] = 0.6f;
			{
				AiCombatStrategy* aiCombatStrategy = (AiCombatStrategy*) actor.GetComponentByName( "AiCombatStrategy" );
				if(aiCombatStrategy)
				{
					aiCombatStrategy->EnableTactic(AiCombatStrategy::TACTIC_CHARGE,true);
					aiCombatStrategy->EnableTactic(AiCombatStrategy::TACTIC_WANDER,true);
				}
			}
			break;
		case AI_BUDDY:		
			baseWant.predisp[WANT_DO_JOB] = 0.50f;
			baseWant.predisp[WANT_TO_WATCH] = 0.01f;
			baseWant.predisp[WANT_TO_FIGHT] = 0.49f;
			baseWant.sensitivity[WANT_TO_FIGHT] = 0.6f;
			{
				AiCombatStrategy* aiCombatStrategy = (AiCombatStrategy*) actor.GetComponentByName( "AiCombatStrategy" );
				if(aiCombatStrategy)
				{
					aiCombatStrategy->EnableTactic(AiCombatStrategy::TACTIC_FOLLOW,true);
					aiCombatStrategy->EnableTactic(AiCombatStrategy::TACTIC_WANDER,true);
				}
			}
			break;
		case AI_CHICKEN:		
			baseWant.predisp[WANT_DO_JOB] = 0.50f;
			baseWant.predisp[WANT_TO_WATCH] = 0.01f;
			baseWant.predisp[WANT_TO_FIGHT] = 0.49f;
			baseWant.sensitivity[WANT_TO_FIGHT] = 0.6f;
			{
				AiCombatStrategy* aiCombatStrategy = (AiCombatStrategy*) actor.GetComponentByName( "AiCombatStrategy" );
				if(aiCombatStrategy)
				{
					aiCombatStrategy->EnableTactic(AiCombatStrategy::TACTIC_RETREAT,true);
					aiCombatStrategy->EnableTactic(AiCombatStrategy::TACTIC_WANDER,true);
				}
			}
			break;
		case AI_EVACUATOR:	
			baseWant.predisp[WANT_DO_JOB] = 1.0f;
			break;
		case AI_BAMBI:
			baseWant.predisp[WANT_DO_JOB] = 0.51f;
			baseWant.predisp[WANT_TO_FLEE] = 0.2f;
			baseWant.sensitivity[WANT_TO_FLEE] = 5.0f;
			break;
	}

	//LoadVars();
}

//============================================================================
void AiPsyche::LoadVars()
{
	const char *context = actor.Parm().GetString("ai_var_file");
	if(context == NULL)
		return;
	
	m_aiParams = CParam<AiParams>::GetParam(context, "var");

	baseWant.predisp[WANT_DO_JOB] = m_aiParams->want_dj_predisp;
	baseWant.returnRate = m_aiParams->return_state_speed;
	baseWant.sensitivity[WANT_DO_JOB] = m_aiParams->return_state_speed;
	baseWant.predisp[WANT_TO_WATCH] = m_aiParams->want_watch_predisp;
	baseWant.sensitivity[WANT_TO_WATCH] = m_aiParams->want_watch_sensitivity;
	baseWant.predisp[WANT_TO_FLEE] = m_aiParams->want_flee_predisp;
	baseWant.sensitivity[WANT_TO_FLEE] = m_aiParams->want_flee_sensitivity ;
	baseWant.predisp[WANT_TO_COWER] = m_aiParams->want_cower_predisp;
	baseWant.sensitivity[WANT_TO_COWER] = m_aiParams->want_cower_sensitivity;
	baseWant.predisp[WANT_TO_FIGHT] = m_aiParams->want_fight_predisp;
	baseWant.sensitivity[WANT_TO_FIGHT] = m_aiParams->want_fight_sensitivity;
	baseWant.accessories[WANT_ACCESSORY_HANDSUP] = m_aiParams->want_handsup_predisp;
	baseWant.accessorySensitivity[WANT_ACCESSORY_HANDSUP] = m_aiParams->want_handsup_sensitivity;
}

void CParam<AiParams>::RegisterVariables()
{
	RegisterVariable(d_data.want_dj_predisp, "want_dj_predisp", 0.0f, 0.0f, 100.0f);
	RegisterVariable(d_data.return_state_speed, "return_state_speed", 0.0f, 0.0f, 100.0f);
	RegisterVariable(d_data.want_watch_predisp, "want_watch_predisp", 0.0f, 0.0f, 100.0f);
	RegisterVariable(d_data.want_watch_sensitivity, "want_watch_sensitivity", 0.0f, 0.0f, 100.0f);
	RegisterVariable(d_data.want_flee_predisp, "want_flee_predisp", 0.0f, 0.0f, 100.0f);
	RegisterVariable(d_data.want_flee_sensitivity, "want_flee_sensitivity", 0.0f, 0.0f, 100.0f);
	RegisterVariable(d_data.want_cower_predisp, "want_cower_predisp", 0.0f, 0.0f, 100.0f);
	RegisterVariable(d_data.want_cower_sensitivity, "want_cower_sensitivity", 0.0f, 0.0f, 100.0f);
	RegisterVariable(d_data.want_fight_predisp, "want_fight_predisp", 0.0f, 0.0f, 100.0f);
	RegisterVariable(d_data.want_fight_sensitivity, "want_fight_sensitivity", 0.0f, 0.0f, 100.0f);
	RegisterVariable(d_data.want_handsup_predisp, "want_handsup_predisp", 0.0f, 0.0f, 100.0f);
	RegisterVariable(d_data.want_handsup_sensitivity, "want_handsup_sensitivity", 0.0f, 0.0f, 100.0f);
	RegisterVariable(d_data.tactic_normal, "tactic_normal", 0, 0, 1);
	RegisterVariable(d_data.tactic_dodgy, "tactic_dodgy", 0, 0, 1);
	RegisterVariable(d_data.tactic_charge, "tactic_charge", 0, 0, 1);
	RegisterVariable(d_data.tactic_sneaky, "tactic_sneaky", 0, 0, 1);
	RegisterVariable(d_data.tactic_cover, "tactic_cover", 0, 0, 1);
	RegisterVariable(d_data.tactic_hide, "tactic_hide", 0, 0, 1);
	RegisterVariable(d_data.tactic_retreat, "tactic_retreat", 0, 0, 1);
	RegisterVariable(d_data.tactic_wander, "tactic_wander", 0, 0, 1);
	RegisterVariable(d_data.tactic_follow, "tactic_follow", 0, 0, 1);
}

