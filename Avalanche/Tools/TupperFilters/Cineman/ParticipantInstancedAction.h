////////////////////////////////////////////////////////////////////////////
//
// ParticipantInstancedAction.h
//
// Class to keep track of a instanced action for a participant
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ParticipantInstancedAction.h $
 * 
 * *****************  Version 3  *****************
 * User: Nmartin      Date: 6/02/03    Time: 5:31p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * tony's cut scene is working!
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/29/03    Time: 4:09p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * first phase of source and instanced actions
 */

////////////////////////////////////////////////////////////////////////////

#ifndef PARTICIPANT_INSTANCED_ACTION_H
#define PARTICIPANT_INSTANCED_ACTION_H

class ParticipantInstancedAction
{
public:
	ParticipantInstancedAction(void);
	const char *GetName(void) const;
	void SetName(const char *pName);

	float GetStartTime(void) const;
	float GetCycle(void) const;
	float GetWeight(void) const;
	float GetScale(void) const;
	int GetFlags(void) const;
	int GetSourceReference(void) const;
	bool GetProcessed() const;

	void SetStartTime(float startTime);
	void SetCycle(float cycle);
	void SetWeight(float weight);
	void SetScale(float scale);
	void SetFlags(int flags);
	void SetSourceReference(int reference);
	void SetProcessed(bool bProcessed);

private:
	TupString m_name;
	int m_sourceReference;
	float m_startTime;
	float m_cycle;
	float m_weight;
	float m_scale;
	int m_flags;
	bool m_bProcessed;
};

#endif
