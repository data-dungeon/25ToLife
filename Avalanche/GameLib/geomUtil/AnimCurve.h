/**********************************************/
/*                                            */
/*   AnimCurve.h                              */
/*   big juju  5/14/03                        */
/*   ned martin  avalanche software           */
/*   visual c++ 6.0                           */
/*   animation curves for cut scenes          */
/*                                            */
/**********************************************/

/* animation curve data for cut scenes. in addition to being able to play existing motion data animations, cut scenes can contain
new animation data, in the form of animation (keyframe) curves. each curve represents one channel-- e.g. the camera's x-rotation.
each curve is a chain of linked 2D splines. unlike the SplineChain class in GeomUtil/Spline.h, the endpoints of an animation
curve spline chain do not have to be equally spaced. this class is intended to be fast in processing a spline chain in a forward
direction */

#ifndef __ANIMCURVE_H
#define __ANIMCURVE_H

/* system includes */

/* engine includes */

#include "Math/Vect2.h"

/******************* forward declarations ****************/

/******************* defines *****************************/

#define ANIM_CURVE_NAME_LENGTH	16
#define ANIM_CURVE_SAMPLE_POINTS	10

/******************* macros ******************************/

/******************* structures **************************/

/* disk data */

struct ts_AnimCurve
{
	char Name[ANIM_CURVE_NAME_LENGTH];	// so cut scene can hook up to named curves
	int nSplineCount;							// each spline is a start point, two tangents, and an end point

	/* a curve has a start time and an end time. outside this range, the return value is a flat line that connects */
	/* to the first point is before the start time, or to the last point if after the end time */

	float fStartTime;
	float fEndTime;

	/* allow room for at least one spline, typically there will be more. total number of Vector2's is */
	/* (nSplineCount * 3) + 1. the x-value of each Vector2 is a time value, the y is a value in the correct */
	/* units for the type of curve, e.g. radians for a rotation curve, meters for a translation curve */

	/*** must be last-- variable length array ***/

	Vector2 SplineData[4];

#ifdef GCN
	void EndianSwap( void)
	{
		ByteSwap( nSplineCount);
		ByteSwap( fStartTime);
		ByteSwap( fEndTime);

		Vector2	*pVec = SplineData;

		for( int iVec = 0; iVec < ( ( nSplineCount * 3)) + 1; iVec++, pVec++)
		{
			pVec->EndianSwap( );
		}
	}
#endif
};

/******************* global variables ********************/

/******************* global prototypes *******************/

/******************* class *******************************/

class CAnimCurve
{
public:

	/* Constructor/destructor */

	CAnimCurve() { d_pData = NULL; }
	~CAnimCurve() {}

	/* initialize */

	void Initialize(ts_AnimCurve *pData);

	/* the main workhorse */

	float Evaluate(float fTime);

	/* access */

	int GetSplineCount() const								{ if (d_pData) return(d_pData->nSplineCount); return(0); }
	int GetSplinePointCount() const						{ if (d_pData) return((d_pData->nSplineCount * 3) + 1); return(0); }
	const Vector2 *GetSplinePoint(int nIndex) const	{ ASSERT(nIndex < GetSplinePointCount()); if (d_pData) return(&d_pData->SplineData[nIndex]); return(NULL); }

protected:

	/* for evaluating current spline (indices into SplineData array) */

	enum
	{
		START_POINT = 0,
		START_TANGENT,
		END_TANGENT,
		END_POINT,
	};

	/* data */

	ts_AnimCurve *d_pData;
	Vector2 *d_pCurrentSpline;	// pointer into ts_AnimCurve's spline data

	/* for sampling current spline segment more accurately-- see notes in Evaluate */

	float d_fSampleX[ANIM_CURVE_SAMPLE_POINTS];
	int d_nSampleIndex;
	bool d_bSampleValid;
};

#endif // __ANIMCURVE_H

