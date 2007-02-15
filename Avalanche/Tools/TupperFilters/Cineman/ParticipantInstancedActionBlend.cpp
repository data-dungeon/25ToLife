////////////////////////////////////////////////////////////////////////////
//
// ParticipantInstancedActionBlend.cpp
//
// Class to keep track a blend between two instanced actions
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ParticipantInstancedActionBlend.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/30/03    Time: 1:54p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * group transform connecting to boned mesh transform
 * animation from bones connecting to actions for mesh transform
 * added more camera channels for hooking up
 * added instance clip blends
 */

////////////////////////////////////////////////////////////////////////////

#include "CinemanPch.h"
#include "ParticipantInstancedActionBlend.h"

ParticipantInstancedActionBlend::ParticipantInstancedActionBlend(void) : 
	m_clip1Ref(-1),m_clip2Ref(-1),m_pCurveAgg(NULL),m_flags(0)
{
}

const char *ParticipantInstancedActionBlend::GetName(void) const
{
	return m_name;
}

int ParticipantInstancedActionBlend::GetInstancedClip1Reference(void) const
{
	return m_clip1Ref;
}

int ParticipantInstancedActionBlend::GetInstancedClip2Reference(void) const
{
	return m_clip2Ref;
}

TupperwareAggregate *ParticipantInstancedActionBlend::GetCurve(void) const
{
	return m_pCurveAgg;
}

int ParticipantInstancedActionBlend::GetFlags(void) const
{
	return m_flags;
}

void ParticipantInstancedActionBlend::SetName(const char *pName)
{
	m_name = pName;
}

void ParticipantInstancedActionBlend::SetInstancedClip1Reference(int ref)
{
	m_clip1Ref = ref;
}

void ParticipantInstancedActionBlend::SetInstancedClip2Reference(int ref)
{
	m_clip2Ref = ref;
}

void ParticipantInstancedActionBlend::SetCurve(TupperwareAggregate *pCurveAgg)
{
	m_pCurveAgg = pCurveAgg;
}

void ParticipantInstancedActionBlend::SetFlags(int flags)
{
	m_flags = flags;
}
