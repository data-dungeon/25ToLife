#ifndef COACH_H
#define COACH_H

/* system includes */

/* engine includes */

/* game includes */

class CActor;

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

#endif //COACH_H
