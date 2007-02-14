#include "TTLPCH.h"
#include "Coach.h"

#define LOUD_SOUND_DISTANCE 30.0f

Coach::Coach(int i_team)
{
	team = i_team;
	Initialize();
}

Coach::~Coach()
{

}

void Coach::Initialize(void)
{
	playersOnTeam = 0;
	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		teamActors[i] = NULL;
	}
}

