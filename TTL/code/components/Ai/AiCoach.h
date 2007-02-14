#ifndef AICOACH_H
#define AICOACH_H

//only one active coach per team...it doesn't matter if coach is dead, the coach functionality will continue
//to direct the team.  If the combatant is going to be deleted...it needs to transfer coachship to another bot

#include "Components/Component.h"

class AiCoach : public CActorComponent
{
public:
	typedef enum
	{
		TEAM_STATE_NORMAL,
		TEAM_STATE_OFFENSIVE,
		TEAM_STATE_DEFENSIVE,
		TEAM_STATE_HUNTING,
		TEAM_STATE_OBJECTIVE,
		TEAM_STATE_DISABLED
	} TEAM_STATES;


	AiCoach( CCompActor &i_actor );
	~AiCoach();

	const char *ComponentName( void ) { return "AiCoach"; }

	void Initialize(void);
	void Reset(void);
	void Update(float frameSec);
	void SetActive(bool toggle) {active = toggle;}
	bool GetActive() {return active;}

private:
	void GatherData();
	void MacroStrategy();
	void SendStateChange();
	bool active;
	int teamState;
	float decisionTimer;
	int members;
	int memberPartId[MAX_PARTICIPANTS];
	float healthRatio;
};

#endif

/* OLD THOUGHTS....
class Coach
{
public:
Coach(int i_team = 0);
~Coach();
void Initialize(void);

void SetTeam(int i_team){team = i_team;} //one coach per team
int GetTeam(void) {return team;}

void RegisterPlayer(CActor *actor);
void RemovePlayer(CActor *actor);

int AddTeamGoal(Vector3 location, int type);
void UpdateTeamGoal(int goal, int status);

void SetTeamState(int i_teamState) {teamState = i_teamState;}
int GetTeamState(void){return teamState;}

typedef enum
{
TEAM_STATE_NUETRAL,
TEAM_STATE_OFFENSIVE,
TEAM_STATE_DEFENSIVE,
TEAM_STATE_HUNTING,
TEAM_STATE_FAILED
} TEAM_STATES;

protected:

private:
int team;
int playersOnTeam;
CActor *teamActors[MAX_PLAYERS];
int teamState;

struct TeamGoal
{
Vector3 location;
int type;
int status;
TeamGoal *prev, *next;
};

DoublyLinkedList<TeamGoal>	teamGoalList;
};
*/