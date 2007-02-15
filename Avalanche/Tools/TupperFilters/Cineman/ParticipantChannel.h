////////////////////////////////////////////////////////////////////////////
//
// ParticipantChannel.h
//
// Class to keep track of the channel data for a participant
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ParticipantChannel.h $
 * 
 * *****************  Version 12  *****************
 * User: Nmartin      Date: 9/15/03    Time: 10:04a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * calculate splines from maya data instead of using linear curves
 * 
 * *****************  Version 11  *****************
 * User: Nmartin      Date: 7/30/03    Time: 11:37a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * process light participants
 * 
 * *****************  Version 10  *****************
 * User: Nmartin      Date: 6/26/03    Time: 3:56p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * start dealing with parent/child relationships
 * 
 * *****************  Version 9  *****************
 * User: Nmartin      Date: 6/02/03    Time: 5:31p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * tony's cut scene is working!
 * 
 * *****************  Version 8  *****************
 * User: Nmartin      Date: 6/02/03    Time: 2:55p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * merging cameras works, particle effect works some
 * 
 * *****************  Version 7  *****************
 * User: Nmartin      Date: 6/02/03    Time: 11:07a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * convert static values to engine-friendly values before export to .cut
 * file
 * 
 * *****************  Version 6  *****************
 * User: Nmartin      Date: 6/02/03    Time: 10:07a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * merging multiple participants into a single data stream
 * 
 * *****************  Version 5  *****************
 * User: Nmartin      Date: 5/30/03    Time: 3:28p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * working on merging multiple participants into one cut scene data stream
 * (e.g. all particle effects end up as one stream)
 * 
 * *****************  Version 4  *****************
 * User: Nmartin      Date: 5/28/03    Time: 7:49p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * processing adam's data-- creates .cut and curve .dbl file
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/27/03    Time: 10:29a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * first pass of participant manager
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/23/03    Time: 3:21p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * start of participants classes
 */

////////////////////////////////////////////////////////////////////////////

#ifndef PARTICIPANT_CHANNEL_H
#define PARTICIPANT_CHANNEL_H

class ParticipantChannel
{
	class SourceCurve
	{
	public:

		enum Flags
		{
			FLAGS_CONVERTED					= 1<<0,	// has been through ConvertSourceCurveData
		};

		float m_startTime;
		float m_duration;
		int m_flags;
		TupperwareAggregate *m_pCurveData;

		SourceCurve(void);

	};
	
	class InstancedCurve
	{
	public:
		int m_sourceReference;
		float m_startTime;
		float m_cycle;
		float m_weight;
		float m_scale;
		int m_flags;
		bool m_bProcessed;
		ts_AnimCurve *m_pAnimCurveData;	// curve data that is suitable for evaluation or export

		InstancedCurve(void);
		~InstancedCurve(void);

	};

public:

	enum Types
	{
		// general properties
		TYPE_VISIBILITY,
		// transformation properties
		TYPE_TRANSLATION_X,
		TYPE_TRANSLATION_Y,
		TYPE_TRANSLATION_Z,
		TYPE_ROTATION_X,
		TYPE_ROTATION_Y,
		TYPE_ROTATION_Z,
		TYPE_SCALE_X,
		TYPE_SCALE_Y,
		TYPE_SCALE_Z,
		TYPE_SHEAR_X,
		TYPE_SHEAR_Y,
		TYPE_SHEAR_Z,
		TYPE_SCALE_PIVOT_X,
		TYPE_SCALE_PIVOT_Y,
		TYPE_SCALE_PIVOT_Z,
		TYPE_ROTATION_PIVOT_X,
		TYPE_ROTATION_PIVOT_Y,
		TYPE_ROTATION_PIVOT_Z,
		TYPE_SCALE_PIVOT_TRANSLATION_X,
		TYPE_SCALE_PIVOT_TRANSLATION_Y,
		TYPE_SCALE_PIVOT_TRANSLATION_Z,
		TYPE_ROTATION_PIVOT_TRANSLATION_X,
		TYPE_ROTATION_PIVOT_TRANSLATION_Y,
		TYPE_ROTATION_PIVOT_TRANSLATION_Z,
		TYPE_ROTATION_ORIENTATION_X,
		TYPE_ROTATION_ORIENTATION_Y,
		TYPE_ROTATION_ORIENTATION_Z,
		// CAMERA PROPERTIES
		TYPE_FOCAL_LENGTH,
		TYPE_CAMERA_SCALE,
		TYPE_NEAR_CLIP_PLANE,
		TYPE_FAR_CLIP_PLANE,
		TYPE_HORIZONTAL_FILM_APERTURE,
		TYPE_VERTICAL_FILM_APERTURE,
		TYPE_LENS_SQUEEZE_RATIO,
		// LIGHT PROPERTIES
		TYPE_COLOR_RED,
		TYPE_COLOR_GREEN,
		TYPE_COLOR_BLUE,
		// UNKNOWN
		TYPE_UNKNOWN,
		NUM_TYPES // must be last
	};

	static char *m_typeNames[NUM_TYPES]; // names for the types

	ParticipantChannel(void);
	Types GetType(void) const;
	void SetType(Types type);
	const char *GetTypeName(void) const;

	float GetStaticValue(void) const;
	void SetStaticValue(float value);

	// source curves
	int GetNumSourceCurves(void) const;
	float GetSourceStartTime(int sourceCurveIndex) const;
	float GetSourceDuration(int sourceCurveIndex) const;
	int GetSourceFlags(int sourceCurveIndex) const;
	TupperwareAggregate *GetSourceCurveData(int sourceCurveIndex) const;

	int AddSourceCurve(void);
	void SetSourceStartTime(int sourceCurveIndex,float startTime);
	void SetSourceDuration(int sourceCurveIndex,float duration);
	void SetSourceFlags(int sourceCurveIndex,int flags);
	void SetSourceCurveData(int sourceCurveIndex,TupperwareAggregate *pSourceCurveAgg);

	// instanced curves
	int GetNumInstancedCurves(void) const;
	int GetInstanceSourceCurveReference(int instancedCurveIndex) const;
	float GetInstanceStartTime(int instancedCurveIndex) const;
	float GetInstanceCycle(int instancedCurveIndex) const;
	float GetInstanceWeight(int instancedCurveIndex) const;
	float GetInstanceScale(int instancedCurveIndex) const;
	int GetInstanceFlags(int instancedCurveIndex) const;
	bool GetInstanceProcessed(int instancedCurveIndex) const;
	ts_AnimCurve *GetInstanceAnimCurveData(int instancedCurveIndex) const;

	int AddInstancedCurve(void);
	void SetInstanceSourceCurveReference(int instancedCurveIndex,int sourceCurveReference);
	void SetInstanceStartTime(int instancedCurveIndex,float startTime);
	void SetInstanceCycle(int instancedCurveIndex,float cycle);
	void SetInstanceWeight(int instancedCurveIndex,float weight);
	void SetInstanceScale(int instancedCurveIndex,float scale);
	void SetInstanceFlags(int instancedCurveIndex,int flags);
	void SetInstanceProcessed(int instancedCurveIndex,bool bProcessed);
	void SetInstanceAnimCurveData(int instancedCurveIndex,ts_AnimCurve *pData);

	/* some routines to be used in collecting data while preparing for output into a .cut file */

	void SetValidForExport(bool bValid)	{ m_bValidForExport = bValid; }
	bool IsValidForExport() const			{ return(m_bValidForExport); }
	void SetValidForEval(bool bValid)	{ m_bValidForEval = bValid; }
	bool IsValidForEval() const			{ return(m_bValidForEval); }

	/* adjust all time values to seconds */

	void ConvertTimeToSeconds(float fFramesPerSecond);

	/* evaluate at a given time */
	/*** note: result is in converted, game-compatible format ***/

	float Evaluate(float fTime) const;

	/* find point at which visibility state matches passed state, starting at passed time */

	float FindVisibleTime(const bool bVisible, float fTime, const float fMaxTime) const;

	/* possible data conversion to account for differences in maya and engine */
	
	void ConvertSourceCurveData(const int nParticipantType);

private:
	Types m_type;
	TupArray<SourceCurve> m_sourceCurves;
	TupArray<InstancedCurve> m_instancedCurves;
	float m_staticValue;

	/* some variables to be used in collecting data while preparing for output into a .cut file */

	bool m_bValidForExport;	// whether channel type is valid for participant type (e.g. focal length not valid for actors)
	bool m_bValidForEval;	// whether channel type is valid for evaluation during channel processing (e.g. visibility for particle effects)
};

#endif
