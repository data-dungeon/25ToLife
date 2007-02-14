#include "TTLPCH.h"
#include "components/Ai/AiCoach.h"
#include "Components/ComponentCreator.h"
#include "InterfacesTTL/ParticipantIF.h"
#include "components/Ai/AiCombatStrategy.h"
#include "avatar/pstat.h"

//============================================================================
// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(AiCoach, AiCoach,
		COMPONENT_NEUTRAL_PRIORITY);

#define DECISION_TIME 2.0f
//============================================================================
AiCoach::AiCoach( CCompActor &i_actor ) :
CActorComponent( i_actor )
{
}
//============================================================================
AiCoach::~AiCoach()
{
}
//============================================================================
void AiCoach::Initialize() 
{
	active = true;
	members = 0;
	for(int i = 0; i < MAX_PARTICIPANTS; i++)
	{
		memberPartId[i] = -1;
	}

	CParticipantIF* myParticipant =
		(CParticipantIF*)actor.GetInterface(CParticipantIF::GetID());
	ASSERT(myParticipant);
	int participantId = myParticipant->GetParticipantId();

	for(int budPartId=0;budPartId<MAX_PARTICIPANTS;budPartId++)
	{
		if(!g_referee.GetTeamingModel().AreFriends(budPartId, participantId))
			continue;  //not a buddy
		members++;
		ASSERT( members < ( sizeof(memberPartId) / sizeof(memberPartId[0]) ) );
		memberPartId[members] = budPartId;
		if(budPartId == participantId)
			continue;  //not myself;
		CCompActor *buddy = (CCompActor*) g_referee.GetActor(budPartId);
		if(buddy)
		{
			AiCoach* aiCoach = (AiCoach*) buddy->GetComponentByName( "AiCoach" );
			if(aiCoach)
			{
				if(aiCoach->GetActive())
				{
					active = false; //another teammate is already coach
				}
			}
		}
	}
	Reset();
}
//============================================================================
void AiCoach::Reset()
{
	decisionTimer = g_random.InRange(DECISION_TIME, DECISION_TIME + 1.0f);
	teamState = TEAM_STATE_NORMAL;
	healthRatio = 1.0f;
}
//============================================================================
void AiCoach::Update(float frameSec)
{
	Math::MoveToZero(&decisionTimer,frameSec);
	if(decisionTimer == 0.0f)
	{
		decisionTimer = DECISION_TIME;
		GatherData();
		MacroStrategy();
	}
}
//============================================================================
void AiCoach::GatherData()
{
	float teamHealth = 0.0f;
	float teamMaxHealth = 0.0f;

	for(int i = 0; i < members; i++)
	{
		if(g_referee.IsAlive(memberPartId[i]))
		{
			CCompActor *buddy = (CCompActor*) g_referee.GetActor(memberPartId[i]);
			if(buddy)
			{
				PlayerStats* pStats = (PlayerStats*) buddy->GetComponentByName( "PlayerStats" );
				if(pStats)
				{
					teamHealth += pStats->GetHealth();
					teamMaxHealth += pStats->GetMaxHealth();
				}
			}
		}
	}
	if(teamMaxHealth > 0.0f)
		healthRatio = teamHealth/teamMaxHealth;
}
//============================================================================
void AiCoach::MacroStrategy()
{
#define TEMPSTUPIDSIMPLECRAP 0.6f

	if(healthRatio < TEMPSTUPIDSIMPLECRAP && teamState != TEAM_STATE_DEFENSIVE)
	{
		teamState = TEAM_STATE_DEFENSIVE;
		SendStateChange();
	}
	else if(healthRatio >= TEMPSTUPIDSIMPLECRAP && teamState != TEAM_STATE_NORMAL) //TEMPORARY
	{
		teamState = TEAM_STATE_NORMAL;
		SendStateChange();
	}
}
//============================================================================
void AiCoach::SendStateChange()
{

	for(int i = 0; i < members; i++)
	{
		CCompActor *buddy = (CCompActor*) g_referee.GetActor(memberPartId[i]);
		if(buddy)
		{
			AiCombatStrategy* aiCombatStrategy = (AiCombatStrategy*) buddy->GetComponentByName( "AiCombatStrategy" );
			if(aiCombatStrategy)
			{
				aiCombatStrategy->SetTeamState(teamState);
			}
		}
	}
}
//============================================================================
//============================================================================
//============================================================================
//============================================================================
//============================================================================


