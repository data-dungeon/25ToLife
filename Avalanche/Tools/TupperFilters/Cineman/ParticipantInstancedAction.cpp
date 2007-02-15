////////////////////////////////////////////////////////////////////////////
//
// ParticipantInstancedAction.cpp
//
// Class to keep track of a instanced action for a participant
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ParticipantInstancedAction.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Nmartin      Date: 6/02/03    Time: 5:31p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * tony's cut scene is working!
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/30/03    Time: 1:54p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * group transform connecting to boned mesh transform
 * animation from bones connecting to actions for mesh transform
 * added more camera channels for hooking up
 * added instance clip blends
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/29/03    Time: 4:09p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * first phase of source and instanced actions
 */

////////////////////////////////////////////////////////////////////////////

#include "CinemanPch.h"
#include "ParticipantInstancedAction.h"

ParticipantInstancedAction::ParticipantInstancedAction(void) : m_sourceReference(-1),m_startTime(0.0f),m_cycle(1.0f),m_weight(1.0f),m_scale(1.0f),m_flags(0)
{
}

const char *ParticipantInstancedAction::GetName(void) const
{
	return m_name;
}

void ParticipantInstancedAction::SetName(const char *pName)
{
	m_name = pName;
}

float ParticipantInstancedAction::GetStartTime(void) const
{
	return m_startTime;
}

float ParticipantInstancedAction::GetCycle(void) const
{
	return m_cycle;
}

float ParticipantInstancedAction::GetWeight(void) const
{
	return m_weight;
}

float ParticipantInstancedAction::GetScale(void) const
{
	return m_scale;
}

int ParticipantInstancedAction::GetFlags(void) const
{
	return m_flags;
}

int ParticipantInstancedAction::GetSourceReference(void) const
{
	return m_sourceReference;
}

bool ParticipantInstancedAction::GetProcessed(void) const
{
	return m_bProcessed;
}

void ParticipantInstancedAction::SetStartTime(float startTime)
{
	m_startTime = startTime;
}

void ParticipantInstancedAction::SetCycle(float cycle)
{
	m_cycle = cycle;
}

void ParticipantInstancedAction::SetWeight(float weight)
{
	m_weight = weight;
}

void ParticipantInstancedAction::SetScale(float scale)
{
	m_scale = scale;
}

void ParticipantInstancedAction::SetFlags(int flags)
{
	m_flags = flags;
}

void ParticipantInstancedAction::SetSourceReference(int reference)
{
	m_sourceReference = reference;
}

void ParticipantInstancedAction::SetProcessed(bool bProcessed)
{
	m_bProcessed = bProcessed;
}

