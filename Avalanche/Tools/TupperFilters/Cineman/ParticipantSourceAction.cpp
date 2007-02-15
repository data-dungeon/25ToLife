////////////////////////////////////////////////////////////////////////////
//
// ParticipantSourceAction.cpp
//
// Class to keep track of a source action for a participant
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ParticipantSourceAction.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/29/03    Time: 4:09p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * first phase of source and instanced actions
 */

////////////////////////////////////////////////////////////////////////////

#include "CinemanPch.h"
#include "ParticipantSourceAction.h"

ParticipantSourceAction::ParticipantSourceAction(void) : m_startTime(0.0f),m_duration(0.0f),m_flags(0)
{
}

const char *ParticipantSourceAction::GetName(void) const
{
	return m_name; 
}

void ParticipantSourceAction::SetName(const char *pName)
{
	m_name = pName;
}

float ParticipantSourceAction::GetStartTime(void) const
{
	return m_startTime;
}

float ParticipantSourceAction::GetDuration(void) const
{
	return m_duration;
}

int ParticipantSourceAction::GetFlags(void) const
{
	return m_flags;
}

void ParticipantSourceAction::SetStartTime(float startTime)
{
	m_startTime = startTime;
}

void ParticipantSourceAction::SetDuration(float duration)
{
	m_duration = duration;
}

void ParticipantSourceAction::SetFlags(int flags)
{
	m_flags = flags;
}
