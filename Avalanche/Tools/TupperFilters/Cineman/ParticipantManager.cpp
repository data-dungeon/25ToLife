////////////////////////////////////////////////////////////////////////////
//
// ParticipantManager.cpp
//
// Class to manage the participants in a cutscene
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ParticipantManager.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 6/23/03    Time: 3:39p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * parent and child participants
 * 
 * *****************  Version 3  *****************
 * User: Nmartin      Date: 5/28/03    Time: 7:49p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * processing adam's data-- creates .cut and curve .dbl file
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/23/03    Time: 3:21p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * start of participants classes
 */

////////////////////////////////////////////////////////////////////////////

#include "CinemanPch.h"
#include "ParticipantManager.h"
#include "Participant.h"
#include "ParticipantBuilder.h"

ParticipantManager::ParticipantManager(TupperwareAggregate *pSceneAgg)
{
	m_pParticipants = new TupArray<Participant>;
	m_pParticipantNodeRefs = new Keeper<int>;

	ParticipantBuilder builder(pSceneAgg,m_pParticipants,m_pParticipantNodeRefs); // build the data
}

ParticipantManager::ParticipantManager(TupperwareAggregate *pSceneAgg,CExclusions *exclusions)
{
	m_pParticipants = new TupArray<Participant>;
	m_pParticipantNodeRefs = new Keeper<int>;

	ParticipantBuilder builder(pSceneAgg,m_pParticipants,m_pParticipantNodeRefs,exclusions); // build the data
}

ParticipantManager::~ParticipantManager()
{
	delete m_pParticipants;
	delete m_pParticipantNodeRefs;
}


int ParticipantManager::GetNumParticipants(void) const
{
	return m_pParticipants->GetSize();
}

const Participant &ParticipantManager::GetParticipant(int participantIndex) const
{
	return (*m_pParticipants)[participantIndex];
}

Participant &ParticipantManager::GetParticipant(int participantIndex)
{
	return (*m_pParticipants)[participantIndex];
}

int ParticipantManager::GetParticipantFromNodeRef(int nodeRef) const
{
	return m_pParticipantNodeRefs->GetIndex(nodeRef);
}


