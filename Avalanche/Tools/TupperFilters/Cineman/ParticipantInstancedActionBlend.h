////////////////////////////////////////////////////////////////////////////
//
// ParticipantInstancedActionBlend.h
//
// Class to keep track a blend between two instanced actions
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ParticipantInstancedActionBlend.h $
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

#ifndef PARTICIPANT_INSTANCED_ACTION_BLEND_H
#define PARTICIPANT_INSTANCED_ACTION_BLEND_H

class ParticipantInstancedActionBlend
{
public:
	ParticipantInstancedActionBlend(void);

	const char *GetName(void) const;
	int GetInstancedClip1Reference(void) const;
	int GetInstancedClip2Reference(void) const;
	TupperwareAggregate *GetCurve(void) const;
	int GetFlags(void) const;

	void SetName(const char *pName);
	void SetInstancedClip1Reference(int ref);
	void SetInstancedClip2Reference(int ref);
	void SetCurve(TupperwareAggregate *pCurveAgg);
	void SetFlags(int flags);

private:

	TupString m_name;
	int m_clip1Ref;
	int m_clip2Ref;
	TupperwareAggregate *m_pCurveAgg;
	int m_flags;
};

#endif
