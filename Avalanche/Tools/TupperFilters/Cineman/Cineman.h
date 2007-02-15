////////////////////////////////////////////////////////////////////////////
//
// Cineman.h
//
// Class to handle the Cineman Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: Cineman.h $
 * 
 * *****************  Version 13  *****************
 * User: Nmartin      Date: 7/22/03    Time: 8:03p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * guard against a single camera that is marked invisible-- produces no
 * datastream
 * 
 * *****************  Version 12  *****************
 * User: Nmartin      Date: 7/18/03    Time: 2:18p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * start of kfm processing (replaces MakeMDB.bat file), always output
 * initial event data
 * 
 * *****************  Version 11  *****************
 * User: Nmartin      Date: 7/09/03    Time: 1:18p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * write out binary data
 * 
 * *****************  Version 10  *****************
 * User: Nmartin      Date: 7/02/03    Time: 12:12p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * process audio file attached to cinematic
 * 
 * *****************  Version 9  *****************
 * User: Nmartin      Date: 7/01/03    Time: 10:00a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * remove ParentPos and ParentRotation
 * 
 * *****************  Version 8  *****************
 * User: Nmartin      Date: 6/26/03    Time: 3:56p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * start dealing with parent/child relationships
 * 
 * *****************  Version 7  *****************
 * User: Nmartin      Date: 6/04/03    Time: 8:36a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * visibility curve activates/deactivates particle emitter, handle
 * transform order flags
 * 
 * *****************  Version 6  *****************
 * User: Nmartin      Date: 6/03/03    Time: 10:44a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * get frames per second, start/end from tup file
 * 
 * *****************  Version 5  *****************
 * User: Nmartin      Date: 6/02/03    Time: 10:07a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * merging multiple participants into a single data stream
 * 
 * *****************  Version 4  *****************
 * User: Nmartin      Date: 5/30/03    Time: 3:27p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * working on merging multiple participants into one cut scene data stream
 * (e.g. all particle effects end up as one stream)
 * 
 * *****************  Version 3  *****************
 * User: Nmartin      Date: 5/28/03    Time: 7:50p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * remove unneeded curve table
 * 
 * *****************  Version 2  *****************
 * User: Nmartin      Date: 5/28/03    Time: 7:49p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * processing adam's data-- creates .cut and curve .dbl file
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 5/16/03    Time: 1:30p
 * Created in $/Avalanche/tools/TupperFilters/Cineman
 */

////////////////////////////////////////////////////////////////////////////

#ifndef CINEMAN_H
#define CINEMAN_H

class TupperwareAggregate;
class ParticipantManager;
class Participant;
#include "participantbuilder.h"

/* a simple structure to handle merging .tup participants into a single cut scene participant. for example, artist
may build cut scene in maya with two cameras, but final cut scene data will only have one camera */

#define DATASTREAM_WAV_EVENT_OUTPUT	0x00000001

#define MAX_PARTICIPANTS_TO_MERGE	100
typedef struct
{
	int nCount;
	int nParticipantIndex[MAX_PARTICIPANTS_TO_MERGE];
	u32 u32Flags;
} ts_MergedDataStream;

class CinemanFilter : public FilterPlugin
{
public:
	CinemanFilter(TupperChainLogBase &log);
	~CinemanFilter();

	int GetTotalPasses(void) const;

	void Process(int pass,const FilterOptions &options,FilterDataManager &dataManager);

protected:

	ts_AnimCurve *BuildAnimCurve(TupAnimCurve *pTupAnimCurve, const char *pCurveName, float fStartTime, int nType);
	int ConvertCurveType(int nType);

	/* output cut scene events for a data stream */

	void OutputInitialEvents(const Participant *pParticipant, const float fTime);
	bool OutputEvents(const int nDataStreamIndex);
	void OutputDiscreteEvents(const int nDataStreamIndex);
	void OutputTransformOrder(const Participant *pParticipant);

	/* output cut scene data */

	void WriteData(const int nKey, const char *pKey, const int nValue, const char *pComment = NULL);
	void WriteData(const int nKey, const char *pKey, const float fValue, const char *pComment = NULL);
	void WriteData(const int nKey, const char *pKey, const char *pValue, const char *pComment = NULL);
	void WriteData(const int nKey, const char *pKey, const char *pValue, const int nValue, const char *pComment = NULL);

	/* find the next active participant in a posibly-merged data stream */

	Participant *FindNextActiveParticipant(const int nDataStreamIndex, float &fTime) const;

	/* keep track of kfm's used in the cut scene, so we can produce anim data */

	void UpdateKFMList(const char *pNewKFMName);

	/* a class to query the tup file for us (thanks, adam) */

	ParticipantManager *m_pParticipantManager;

	/* for merging multiple participants into one cut scene data stream (like all sounds are a single data stream) */

	ts_MergedDataStream *m_pDataStream;
	int m_nDataStreamCount;

	/* length of cut scene in seconds */

	float m_fCutSceneStartTime;
	float m_fCutSceneEndTime;

	/* for writing out data that controls the cut scene */

	char *m_pBinaryData;
	char *m_pBinaryDataStart;
	FILE *m_pCutLogFile;

	/* for collecting kfm names */

	char *m_pKFMBuffer;
	int m_nKFMBufferSize;

	/* for a key/value exclusions list */

	CExclusions	m_exclusions;

};

#endif
