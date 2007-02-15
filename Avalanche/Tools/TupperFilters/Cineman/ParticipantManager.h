////////////////////////////////////////////////////////////////////////////
//
// ParticipantManager.h
//
// Class to manage the participants in a cutscene
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ParticipantManager.h $
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

#ifndef PARTICIPANT_MANAGER_H
#define PARTICIPANT_MANAGER_H

class Participant;
class CExclusions;

class ParticipantManager
{
public:

	ParticipantManager(TupperwareAggregate *pSceneAgg);
	ParticipantManager(TupperwareAggregate *pSceneAgg,CExclusions *exclusions);
	~ParticipantManager();

	int GetNumParticipants(void) const;
	const Participant &GetParticipant(int participantIndex) const;
	Participant &GetParticipant(int participantIndex);

	int GetParticipantFromNodeRef(int nodeRef) const;

private:
	TupArray<Participant> *m_pParticipants;
	Keeper<int> *m_pParticipantNodeRefs;

};

#endif
