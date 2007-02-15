////////////////////////////////////////////////////////////////////////////
//
// ParticipantChannel.cpp
//
// Class to keep track of the channel data for a participant
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ParticipantChannel.cpp $
 * 
 * *****************  Version 15  *****************
 * User: Nmartin      Date: 9/15/03    Time: 10:04a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * calculate splines from maya data instead of using linear curves
 * 
 * *****************  Version 14  *****************
 * User: Nmartin      Date: 7/30/03    Time: 11:37a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * process light participants
 * 
 * *****************  Version 13  *****************
 * User: Nmartin      Date: 7/22/03    Time: 3:26p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * 
 * *****************  Version 12  *****************
 * User: Nmartin      Date: 7/09/03    Time: 1:18p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * write out binary data
 * 
 * *****************  Version 11  *****************
 * User: Nmartin      Date: 6/26/03    Time: 3:56p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * start dealing with parent/child relationships
 * 
 * *****************  Version 10  *****************
 * User: Nmartin      Date: 6/25/03    Time: 9:25a
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * add more curves, check if any instanced curve is valid before counting
 * it, add model_name to particpant
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

#include "CinemanPch.h"
#include "ParticipantChannel.h"
#include "Participant.h"

char *ParticipantChannel::m_typeNames[ParticipantChannel::NUM_TYPES] = 
{
	"Visibility", // TYPE_VISIBILITY,
	"TranslationX", // TYPE_TRANSLATION_X
	"TranslationY", // TYPE_TRANSLATION_Y
	"TranslationZ", // TYPE_TRANSLATION_Z
	"RotationX", // TYPE_ROTATION_X
	"RotationY", // TYPE_ROTATION_Y
	"RotationZ", // TYPE_ROTATION_Z
	"ScaleX", // TYPE_SCALE_X
	"ScaleY", // TYPE_SCALE_Y
	"ScaleZ", // TYPE_SCALE_Z
	"ShearX", // TYPE_SHEAR_X
	"ShearY", // TYPE_SHEAR_Y
	"ShearZ", // TYPE_SHEAR_Z
	"ScalePivotX", // TYPE_SCALE_PIVOT_X
	"ScalePivotY", // TYPE_SCALE_PIVOT_Y
	"ScalePivotZ", // TYPE_SCALE_PIVOT_Z
	"RotationPivotX", // TYPE_ROTATION_PIVOT_X
	"RotationPivotY", // TYPE_ROTATION_PIVOT_Y
	"RotationPivotZ", // TYPE_ROTATION_PIVOT_Z
	"ScalePivotTranslationX", // TYPE_SCALE_PIVOT_TRANSLATION_X
	"ScalePivotTranslationY", // TYPE_SCALE_PIVOT_TRANSLATION_Y
	"ScalePivotTranslationZ", // TYPE_SCALE_PIVOT_TRANSLATION_Z
	"RotationPivotTranslationX", // TYPE_ROTATION_PIVOT_TRANSLATION_X
	"RotationPivotTranslationY", // TYPE_ROTATION_PIVOT_TRANSLATION_Y
	"RotationPivotTranslationZ", // TYPE_ROTATION_PIVOT_TRANSLATION_Z
	"RotationOrientationX", // TYPE_ROTATION_ORIENTATION_X
	"RotationOrientationY", // TYPE_ROTATION_ORIENTATION_Y
	"RotationOrientationZ", // TYPE_ROTATION_ORIENTATION_Z

	// CAMERA PROPERTIES
	"FocalLength", // TYPE_FOCAL_LENGTH
	"CameraScale", // TYPE_CAMERA_SCALE
	"NearClipPlane", // TYPE_NEAR_CLIP_PLANE
	"FarClipPlane", // TYPE_FAR_CLIP_PLANE
	"HorizontalFilmAperture", // TYPE_HORIZONTAL_FILM_APERTURE
	"VerticalFilmAperture", // TYPE_VERTICAL_FILM_APERTURE
	"LensSqueezeRatio", // TYPE_LENS_SQUEEZE_RATIO

	// LIGHT PROPERTIES
	"ColorRed", //TYPE_COLOR_RED,
	"ColorGreen", //TYPE_COLOR_GREEN,
	"ColorBlue", //TYPE_COLOR_BLUE,
	
	// UNKNOWN
	"Unknown", // TYPE_UNKNOWN
};

ParticipantChannel::SourceCurve::SourceCurve(void) :
	m_startTime(0.0f),
	m_duration(0.0f),
	m_flags(0),
	m_pCurveData(NULL)
{
}
	
ParticipantChannel::InstancedCurve::InstancedCurve(void) :
	m_sourceReference(-1),
	m_startTime(0.0f),
	m_cycle(1.0f),
	m_weight(1.0f),
	m_scale(1.0f),
	m_flags(0),
	m_pAnimCurveData(NULL)
{
}

ParticipantChannel::InstancedCurve::~InstancedCurve(void)
{
	if (m_pAnimCurveData)
		free(m_pAnimCurveData);
}

ParticipantChannel::ParticipantChannel(void) :
	m_type(TYPE_UNKNOWN),
	m_staticValue(0.0f)
{
	m_bValidForExport = false;
	m_bValidForEval = false;
}

ParticipantChannel::Types ParticipantChannel::GetType(void) const
{
	return m_type;
}

void ParticipantChannel::SetType(ParticipantChannel::Types type) 
{
	m_type = type;
}

const char *ParticipantChannel::GetTypeName(void) const
{
	return m_typeNames[m_type];
}


float ParticipantChannel::GetStaticValue(void) const
{
	return m_staticValue;
}

void ParticipantChannel::SetStaticValue(float value)
{
	m_staticValue = value;
}

// source curves
int ParticipantChannel::GetNumSourceCurves(void) const
{
	return m_sourceCurves.GetSize();
}

float ParticipantChannel::GetSourceStartTime(int sourceCurveIndex) const
{
	return m_sourceCurves[sourceCurveIndex].m_startTime;
}

float ParticipantChannel::GetSourceDuration(int sourceCurveIndex) const
{
	return m_sourceCurves[sourceCurveIndex].m_duration;
}

int ParticipantChannel::GetSourceFlags(int sourceCurveIndex) const
{
	return m_sourceCurves[sourceCurveIndex].m_flags;
}

TupperwareAggregate *ParticipantChannel::GetSourceCurveData(int sourceCurveIndex) const
{
	return m_sourceCurves[sourceCurveIndex].m_pCurveData;
}

int ParticipantChannel::AddSourceCurve(void)
{
	int newIndex = m_sourceCurves.GetSize();
	m_sourceCurves.SetSize(newIndex+1);
	return newIndex;
}

void ParticipantChannel::SetSourceStartTime(int sourceCurveIndex,float startTime)
{
	m_sourceCurves[sourceCurveIndex].m_startTime = startTime;
}

void ParticipantChannel::SetSourceDuration(int sourceCurveIndex,float duration)
{
	m_sourceCurves[sourceCurveIndex].m_duration = duration;
}

void ParticipantChannel::SetSourceFlags(int sourceCurveIndex,int flags)
{
	m_sourceCurves[sourceCurveIndex].m_flags = flags;
}

void ParticipantChannel::SetSourceCurveData(int sourceCurveIndex,TupperwareAggregate *pSourceCurveAgg)
{
	m_sourceCurves[sourceCurveIndex].m_pCurveData = pSourceCurveAgg;
}

// instanced curves
int ParticipantChannel::GetNumInstancedCurves(void) const
{
	return m_instancedCurves.GetSize();
}

int ParticipantChannel::GetInstanceSourceCurveReference(int instancedCurveIndex) const
{
	return m_instancedCurves[instancedCurveIndex].m_sourceReference;
}

float ParticipantChannel::GetInstanceStartTime(int instancedCurveIndex) const
{
	return m_instancedCurves[instancedCurveIndex].m_startTime;
}

float ParticipantChannel::GetInstanceCycle(int instancedCurveIndex) const
{
	return m_instancedCurves[instancedCurveIndex].m_cycle;
}

float ParticipantChannel::GetInstanceWeight(int instancedCurveIndex) const
{
	return m_instancedCurves[instancedCurveIndex].m_weight;
}

float ParticipantChannel::GetInstanceScale(int instancedCurveIndex) const
{
	return m_instancedCurves[instancedCurveIndex].m_scale;
}

int ParticipantChannel::GetInstanceFlags(int instancedCurveIndex) const
{
	return m_instancedCurves[instancedCurveIndex].m_flags;
}

bool ParticipantChannel::GetInstanceProcessed(int instancedCurveIndex) const
{
	return m_instancedCurves[instancedCurveIndex].m_bProcessed;
}

ts_AnimCurve *ParticipantChannel::GetInstanceAnimCurveData(int instancedCurveIndex) const
{
	return m_instancedCurves[instancedCurveIndex].m_pAnimCurveData;
}

int ParticipantChannel::AddInstancedCurve(void)
{
	int newIndex = m_instancedCurves.GetSize();
	m_instancedCurves.SetSize(newIndex+1);
	return newIndex;
}

void ParticipantChannel::SetInstanceSourceCurveReference(int instancedCurveIndex,int sourceCurveReference)
{
	m_instancedCurves[instancedCurveIndex].m_sourceReference = sourceCurveReference;
}

void ParticipantChannel::SetInstanceStartTime(int instancedCurveIndex,float startTime)
{
	m_instancedCurves[instancedCurveIndex].m_startTime = startTime;
}

void ParticipantChannel::SetInstanceCycle(int instancedCurveIndex,float cycle)
{
	m_instancedCurves[instancedCurveIndex].m_cycle = cycle;
}

void ParticipantChannel::SetInstanceWeight(int instancedCurveIndex,float weight)
{
	m_instancedCurves[instancedCurveIndex].m_weight = weight;
}

void ParticipantChannel::SetInstanceScale(int instancedCurveIndex,float scale)
{
	m_instancedCurves[instancedCurveIndex].m_scale = scale;
}

void ParticipantChannel::SetInstanceFlags(int instancedCurveIndex,int flags)
{
	m_instancedCurves[instancedCurveIndex].m_flags = flags;
}

void ParticipantChannel::SetInstanceProcessed(int instancedCurveIndex,bool bProcessed)
{
	m_instancedCurves[instancedCurveIndex].m_bProcessed = bProcessed;
}

void ParticipantChannel::SetInstanceAnimCurveData(int instancedCurveIndex,ts_AnimCurve *pData)
{
	m_instancedCurves[instancedCurveIndex].m_pAnimCurveData = pData;
}

/* adjust all time values to seconds */

void ParticipantChannel::ConvertTimeToSeconds(float fFramesPerSecond)
{
	/* source curves */

	int i;
	for (i = 0; i < GetNumSourceCurves(); i++)
	{
		/* start time and duration */

		SetSourceStartTime(i, GetSourceStartTime(i) / fFramesPerSecond);
		SetSourceDuration(i, GetSourceDuration(i) / fFramesPerSecond);

		/* get to curve data */

		TupperwareAggregate *pAnimCurveAgg = GetSourceCurveData(i);
		TupAnimCurve *pTupAnimCurve = new TupAnimCurve( pAnimCurveAgg );

		/* input values */

		float *pfTimeValues;
		int nEndpointCount;
		pTupAnimCurve->GetInputValues(&pfTimeValues, nEndpointCount);
		ASSERT(pfTimeValues);
		for (int j = 0; j < nEndpointCount; j++)
			pfTimeValues[j] /= fFramesPerSecond;

		/* the tangent angles in maya appear to be tan(angle) of the tangent. that is, the delta output value divided */
		/* by the delta time value. since we are adjusting the time values, we have to adjust the tangent angles as well */

		float *pfInTangentAngles;
		int nConfirmEndpointCount;
		pTupAnimCurve->GetInTangentAngles(&pfInTangentAngles, nConfirmEndpointCount);
		ASSERT(pfInTangentAngles);
		ASSERT(nConfirmEndpointCount == nEndpointCount);
		for (j = 0; j < nEndpointCount; j++)
			pfInTangentAngles[j] *= fFramesPerSecond;

		float *pfOutTangentAngles;
		pTupAnimCurve->GetOutTangentAngles(&pfOutTangentAngles, nConfirmEndpointCount);
		ASSERT(pfOutTangentAngles);
		ASSERT(nConfirmEndpointCount == nEndpointCount);
		for (j = 0; j < nEndpointCount; j++)
			pfOutTangentAngles[j] *= fFramesPerSecond;

		/* clean up */

		delete pTupAnimCurve;
	}

	/* instance curves */

	for (i = 0; i < GetNumInstancedCurves(); i++)
	{
		/* start time */

		SetInstanceStartTime(i, GetInstanceStartTime(i) / fFramesPerSecond);
	}
}

/* evaluate channel at a given time. channel may have more than one instanced curve. if so, find earliest curve that
includes passed time. later, we'll take blending into account */

float ParticipantChannel::Evaluate(float fTime) const
{
	/* if no curves, return static value */

	if (GetNumInstancedCurves() == 0)
		return(m_staticValue);

	/* may not be any valid curves */

	int i;
	for (i = 0; i < GetNumInstancedCurves(); i++)
	{
		if (GetInstanceAnimCurveData(i) != NULL)
			break;
	}
	if (i == GetNumInstancedCurves())
		return(m_staticValue);

	/* channel may have multiple instance curves. find the earliest one that includes the passed time. or if no */
	/* curve contains time, find the one that's closest */

	int nIndex = -1;
	float fEarliestStartTime = MAXFLOAT;
	int nFollowingIndex = -1;
	float fFollowingTime = MAXFLOAT;
	int nPrecedingIndex = -1;
	float fPrecedingTime = MAXFLOAT;
	for (i = 0; i < GetNumInstancedCurves(); i++)
	{
		/* skip if curve has no data (maybe only 1 endpoint?) */

		if (GetInstanceAnimCurveData(i) == NULL)
			continue;

		/* check out this curve */

		float fStartTime = GetInstanceStartTime(i);
		float fDuration = GetSourceDuration(GetInstanceSourceCurveReference(i));
		float fEndTime = fStartTime + fDuration;

		/* contains passed time and earlier? */

		if ((fStartTime <= fTime) && (fEndTime >= fTime) && (fStartTime < fEarliestStartTime))
		{
			nIndex = i;
			fEarliestStartTime = fStartTime;
		}

		/* comes closer after passed time than other following curves? */

		else if (fStartTime >= fTime)
		{
			float fDeltaTime = fStartTime - fTime;
			if (fDeltaTime < fFollowingTime)
			{
				nFollowingIndex = i;
				fFollowingTime = fDeltaTime;
			}
		}

		/* ends closer before passed time than other preceding curves? */

		else if (fEndTime <= fTime)
		{
			float fDeltaTime = fTime - fEndTime;
			if (fDeltaTime < fPrecedingTime)
			{
				nPrecedingIndex = i;
				fPrecedingTime = fDeltaTime;
			}
		}
	}

	/* first choice is a curve that contains passed time. second choice is a curve that starts after passed time. last */
	/* choice is a curve that ends before passed time (e.g. passed time is after all curves have ended) */

	if (nIndex == -1)
		nIndex = nFollowingIndex;
	if (nIndex == -1)
		nIndex = nPrecedingIndex;

	/* it's possible to have no valid curves (all curves have only 1 endpoint or something weird) */

	if (nIndex == -1)
		return(m_staticValue);

	/* evaluate at passed time */

	CAnimCurve *pAnimCurve = new CAnimCurve;
	pAnimCurve->Initialize(GetInstanceAnimCurveData(nIndex));
	float fReturnValue = pAnimCurve->Evaluate(fTime);
	delete pAnimCurve;
	return(fReturnValue);
}

/* find change in visible state. this is used to determine who is the active participant in a merged data stream-- it's the
one who's visible. typically, at the passed time the participant is in the opposite state of bVisible */

float ParticipantChannel::FindVisibleTime(const bool bVisible, float fTime, const float fMaxTime) const
{
	ASSERT(GetType() == TYPE_VISIBILITY);
#define VISIBLE_THRESHHOLD 0.99f	// allow values very close to 1.0f but not quite

	/* if no instanced curves, use static value */

	if (GetNumInstancedCurves() == 0)
	{
		if (m_staticValue >= VISIBLE_THRESHHOLD)
			return(bVisible ? fTime : fMaxTime + 1.0f);	// visible now, visible forever
		else
			return(bVisible ? fMaxTime + 1.0f : fTime);	// will never be visible, not visible now
	}

	/* have to step through the curve */

	while (fTime < fMaxTime)
	{
		float fVisibility = Evaluate(fTime);
		if (bVisible == (fVisibility >= VISIBLE_THRESHHOLD))
			return(fTime);
		fTime += 0.01f;
	}

	/* never reached requested state */

	return(fMaxTime + 1.0f);
}

/* maya has a different interpretation of camera rotation than the engine (in maya, identity rotation matrix looks down the
negative z-axis, positive x-rotation points camera up, positive z-rotation leans camera left. in engine, identity
matrix looks down positive z-axis, positive x-rotation looks down, and positive z-rotation leans right */

void ParticipantChannel::ConvertSourceCurveData(const int nParticipantType)
{
	/* set up for conversion */

	float fScale = 1.0f;
	float fTranslate = 0.0f;
	float fTangentScale = 1.0f;
	switch(m_type)
	{
	case TYPE_ROTATION_X:
		if (nParticipantType == Participant::TYPE_CAMERA)
			fScale = -1.0f;	// camera is special case, see comment above

		/* output value has already been converted from degrees to radians (i'm not sure if adam does it at load */
		/* time or if it comes out of maya that way). tangents have to be converted as well */

		fTangentScale = Math::TwoPi / 360.0f;
		break;
	case TYPE_ROTATION_Y:
		if (nParticipantType == Participant::TYPE_CAMERA)
			fTranslate = Math::Pi;	// camera is special case, see comment above
		fTangentScale = Math::TwoPi / 360.0f;	// see comment above
		break;
	case TYPE_ROTATION_Z:
		fTangentScale = Math::TwoPi / 360.0f;	// see comment above
		break;

	default:
		/* nothing to do */

		return;
	}

	/* convert static value */

	m_staticValue = ((m_staticValue * fScale) + fTranslate);

	/* loop through source curves */

	for (int i = 0; i < GetNumSourceCurves(); i++)
	{
		/* already processed? */

		if (GetSourceFlags(i) & SourceCurve::FLAGS_CONVERTED)
			continue;
		SetSourceFlags(i, GetSourceFlags(i) | SourceCurve::FLAGS_CONVERTED);

		/* get to curve data */

		TupperwareAggregate *pAnimCurveAgg = GetSourceCurveData(i);
		TupAnimCurve *pTupAnimCurve = new TupAnimCurve( pAnimCurveAgg );

		/* get curve output data */

		float *pfOutputValues;
		int nEndpointCount;
		pTupAnimCurve->GetOutputValues(&pfOutputValues, nEndpointCount);
		ASSERT(pfOutputValues);
		float *pfInTangentAngles;
		int nConfirmEndpointCount;
		pTupAnimCurve->GetInTangentAngles(&pfInTangentAngles, nConfirmEndpointCount);
		ASSERT(pfInTangentAngles);
		ASSERT(nConfirmEndpointCount == nEndpointCount);
		float *pfOutTangentAngles;
		pTupAnimCurve->GetOutTangentAngles(&pfOutTangentAngles, nConfirmEndpointCount);
		ASSERT(pfOutTangentAngles);
		ASSERT(nConfirmEndpointCount == nEndpointCount);

		/* convert */

		for (int j = 0; j < nEndpointCount; j++)
		{
			pfOutputValues[j] = ((pfOutputValues[j] * fScale) + fTranslate);
			pfInTangentAngles[j] *= fTangentScale * fScale;
			pfOutTangentAngles[j] *= fTangentScale * fScale;
		}

		/* clean up */

		delete pTupAnimCurve;
	}
}

