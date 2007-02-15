////////////////////////////////////////////////////////////////////////////
//
// ParticipantSourceAction.h
//
// Class to keep track of a source action for a participant
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ParticipantSourceAction.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/29/03    Time: 4:09p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * first phase of source and instanced actions
 */

////////////////////////////////////////////////////////////////////////////

#ifndef PARTICIPANT_SOURCE_ACTION_H
#define PARTICIPANT_SOURCE_ACTION_H

class ParticipantSourceAction
{
public:
	ParticipantSourceAction(void);
	const char *GetName(void) const;
	void SetName(const char *pName);

	float GetStartTime(void) const;
	float GetDuration(void) const;
	int GetFlags(void) const;

	void SetStartTime(float startTime);
	void SetDuration(float duration);
	void SetFlags(int flags);

private:
	TupString m_name;
	float m_startTime;
	float m_duration;
	int m_flags;
};

#endif
