/**********************************************/
/*                                            */
/*   AnimCurve.cpp                            */
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

/******************* includes ****************************/

#include "geomUtil/GeomUtilPCH.h"



/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

/******************* local prototypes ********************/

/******************* functions ***************************/

/* point to data */

void CAnimCurve::Initialize(ts_AnimCurve *pData)
{
	/* it is possible for this routine to be called again to attach to a different curve data of the same type. */
	/* currently there is no clean up necessary, but if there ever is,this is where it goes */

	if (d_pData)
	{
		/* cleanup... */
	}

	/* record data */

	ASSERT(pData);
	d_pData = pData;
	d_pCurrentSpline = d_pData->SplineData;
	d_bSampleValid = false;

	/* make sure start and end times are correct */

	ASSERT(d_pData->fStartTime == d_pData->SplineData[0].x());
	ASSERT(d_pData->fEndTime == d_pData->SplineData[d_pData->nSplineCount * 3].x());
}

/* evaluate at passed time */

float CAnimCurve::Evaluate(float fTime)
{
	if (fTime <= d_pData->fStartTime)
		return(d_pData->SplineData[0].y());
	if (fTime >= d_pData->fEndTime)
		return(d_pData->SplineData[d_pData->nSplineCount * 3].y());

	/* d_pCurrentSpline tracks the current spline that we are evaluating to get the return value. the passed time */
	/* should be monotonically increasing, so this shouldn't happen, but we need to check */

	if (fTime < d_pCurrentSpline[START_POINT].x())
	{
		ASSERT(false);
		d_pCurrentSpline = d_pData->SplineData;
		d_bSampleValid = false;
	}

	/* find spline that contains passed time */

	while (fTime > d_pCurrentSpline[END_POINT].x())
	{
		d_pCurrentSpline += 3;
		d_bSampleValid = false;
		ASSERT(d_pCurrentSpline - d_pData->SplineData < d_pData->nSplineCount * 3);	// should never run past data since we have the fEndTime check
	}

	/* need to sample this spline segment? the issue is that evaluating a 2D spline gives x and y as a function of t */
	/* typically, the resulting x != t. for example, you can set up endpoints and tangents so that the resulting curve */
	/* is a quarter circle (or at least approximately):


      ^
      |
      +--------->
      |      +
      |          +
      |            *
      |
      |              + ^
      |                |
      |                |
      |               +|
      |                |
      |                |
      +----------------+--->

	/*	or something like that. anyway, evaluating the spline at t = 0.5 gives the point marked by the asterisk-- */
	/* at the 45 degree angle. at this point, x = cos(45), or 0.7. */

	/* the issue is, when this spline is presented to the artist in maya, the x-axis is presented as the t-axis-- */
	/* the user can adjust the tangents any way he wants, but t will always move smoothly along the horizontal axis. */
	/* so what we want at t = 0.5 is really to find the value of t that yields x = 0.5, then use that value of t to */
	/* calculate y. to do this, we'll sample the spline at known values of t and record the resulting x values. these */
	/* values should be monotically increasing-- the spline shouldn't loop or go backwards. then when we get a request */
	/* to evaluate the spline at t = 0.5, we'll look at the x sample values, find where 0.5 fits, linearly interpolate */
	/* to find the value of t that yields x = 0.5, and use that value of t to calculate the y return value */

#define SAMPLE_DELTA_T	(1.0f / (ANIM_CURVE_SAMPLE_POINTS - 1))
	if (d_bSampleValid == false)
	{
		/* we know the start and end values */

		d_fSampleX[0] = d_pCurrentSpline[START_POINT].x();
		d_fSampleX[ANIM_CURVE_SAMPLE_POINTS - 1] = d_pCurrentSpline[END_POINT].x();
		float fT = SAMPLE_DELTA_T;
		for (int i = 1; i < ANIM_CURVE_SAMPLE_POINTS - 1; i++, fT += SAMPLE_DELTA_T)
		{
			float fTSquared = fT * fT;
			float fOneMinusT = 1.0f - fT;
			float fOneMinusTSquared = fOneMinusT * fOneMinusT;
			d_fSampleX[i] = (fOneMinusTSquared * fOneMinusT * d_pCurrentSpline[START_POINT].x()) + 
								 (3.0f * fOneMinusTSquared * fT * d_pCurrentSpline[START_TANGENT].x()) +
								 (3.0f * fOneMinusT * fTSquared * d_pCurrentSpline[END_TANGENT].x()) +
								 (fTSquared * fT * d_pCurrentSpline[END_POINT].x());

			/* must be monotonically increasing */

			ASSERT(d_fSampleX[i] >= d_fSampleX[i - 1]);
		}

		/* test end point for monotonically increasing */

		ASSERT(d_fSampleX[ANIM_CURVE_SAMPLE_POINTS - 1] >= d_fSampleX[ANIM_CURVE_SAMPLE_POINTS - 2]);

		/* samples are valid for this spline segment */

		d_bSampleValid = true;
		d_nSampleIndex = 0;
	}

	/* find passed time in x-sample array */

	while (fTime > d_fSampleX[d_nSampleIndex + 1])
		d_nSampleIndex++;
	ASSERT(d_nSampleIndex < ANIM_CURVE_SAMPLE_POINTS - 1);

	/* calculate corresponding value of t-- we know the value of t at each sample point, just interpolate */

	float fRatio = (fTime - d_fSampleX[d_nSampleIndex]) / (d_fSampleX[d_nSampleIndex + 1] - d_fSampleX[d_nSampleIndex]);
	float fT = ((float) d_nSampleIndex * SAMPLE_DELTA_T) + (fRatio * SAMPLE_DELTA_T);

	/* evalute spline at that value of t */

	float fTSquared = fT * fT;
	float fOneMinusT = 1.0f - fT;
	float fOneMinusTSquared = fOneMinusT * fOneMinusT;
	float fY = (fOneMinusTSquared * fOneMinusT * d_pCurrentSpline[START_POINT].y()) + 
				  (3.0f * fOneMinusTSquared * fT * d_pCurrentSpline[START_TANGENT].y()) +
				  (3.0f * fOneMinusT * fTSquared * d_pCurrentSpline[END_TANGENT].y()) +
				  (fTSquared * fT * d_pCurrentSpline[END_POINT].y());
	return(fY);
}

