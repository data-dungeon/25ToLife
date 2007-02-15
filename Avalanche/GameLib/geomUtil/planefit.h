/*
**
**  File:   planefit.h
**  Date:   March 6, 2001
**  By:     Bryant Collard
**  Desc:   Fits a plane to a collection of points using a least squares
**          method.
**
**   $Archive: /Avalanche/GameLibNew/geomUtil/planefit.h $
**  $Revision: 19 $
**      $Date: 6/30/03 3:35p $
**    $Author: Bcollard $
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#ifndef PLANEFIT_H
#define PLANEFIT_H

#include "platform/BaseType.h"
#include "Math/Matrix.h"

class CPlaneFit
{
  public:
	enum EPlaneFitResult
	{
		PLANE_FIT_NO_POINTS,
		PLANE_FIT_COAXIAL,
		PLANE_FIT_COPLANAR,
		PLANE_FIT_SCATTER
	};

	CPlaneFit();
	void Init(void);
	void AddPoint(Vector3CRef i_point);
	EPlaneFitResult Fit(Vector3 &i_hint, Vector3 &o_loc, Vector3 &o_normal);
	static EPlaneFitResult Fit(uint i_numPoints, const Vector3* i_points,
			Vector3CRef i_hint, Vector3 &o_loc, Vector3 &o_normal);
	bool TransformInit(Vector3CRef i_fit);
	void AddTransformPoint(Vector3CRef i_point);
	EPlaneFitResult TransformFit(Vector3 &o_loc, Vector3 &o_normal);
	static EPlaneFitResult TransformFit(uint i_numPoints,
			const Vector3* i_points, Vector3CRef i_fit, Vector3 &o_loc,
			Vector3 &o_normal);
	static EPlaneFitResult TransformFit(uint i_numPoints,
			const Vector3* i_points, Vector3CRef i_fit, Vector3CRef i_plane,
			Vector3 &o_loc, Vector3 &o_normal);
	uint GetNumPoints(void) {return(d_numPoints);};
  private:
	DirCos3x3 d_worldToFit;
	uint d_numPoints;
	Vector3 d_pointSum;
	float d_sumXX;
	float d_sumXY;
	float d_sumXZ;
	float d_sumYY;
	float d_sumYZ;
	float d_sumZZ;
};

#endif
