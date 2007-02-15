/*
**
**  File:   planefit.cpp
**  Date:   March 6, 2001
**  By:     Bryant Collard
**  Desc:   Fits a plane to a collection of points using a least squares
**          method.
**
**   $Archive: /Avalanche/GameLibNew/geomUtil/planefit.cpp $
**  $Revision: 39 $
**      $Date: 7/24/03 3:12p $
**    $Author: Bcollard $
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#if 0 // Problem code - need to fix.
	Vector3 pointList[2];
	Vector3 normalSum, point, normal;
	pointList[0].Set(-37.796799f, -2.4650283f, -66.488235f);
	pointList[1].Set(-37.795586f, -2.4649868f, -66.523018f);
	normalSum.Set(-0.0011902219f, 2.9999084f, 0.0047914060f);
	CPlaneFit::EPlaneFitResult result = CPlaneFit::TransformFit(2,
			pointList, normalSum, point, normal);
#endif

/******************* includes ****************************/

#include "geomUtil/GeomUtilPCH.h"

#if 0

// My header (always first) //
#include "geomUtil/planefit.h"

// Engine includes //
#include "Layers/Assert/Assert.h"
#include "Math/MathClass.h"

// Gamelib includes //
#include "platform/BaseType.h"

#endif

/**********************************************************************
 *
 *  Class:       CPlaneFit
 *  Function:    CPlaneFit (constructor)
 *  Access:      public
 *  Description: Initializes variables.
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
CPlaneFit::CPlaneFit()
{
	d_worldToFit.Identity();
	d_numPoints = 0;
}

/**********************************************************************
 *
 *  Class:       CPlaneFit
 *  Function:    Init
 *  Access:      public
 *  Description: Prepares to fit a plane to a collection of points.
 *               Points will be accumulated then a plane will be fit.
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
void CPlaneFit::Init(void)
{
/*
 *	Clear the transform matrix just in case.
 */
	d_worldToFit.Identity();
/*
 *	Initialize the point counter a accumulation buffers.
 */
	d_numPoints = 0;
	d_pointSum.Clear();
	d_sumXX = 0.0f;
	d_sumXY = 0.0f;
	d_sumXZ = 0.0f;
	d_sumYY = 0.0f;
	d_sumYZ = 0.0f;
	d_sumZZ = 0.0f;
}

/**********************************************************************
 *
 *  Class:       CPlaneFit
 *  Function:    AddPoint
 *  Access:      public
 *  Description: Adds a point to the collection to be fit.
 *  Arguments:
 *      i_point     - The point.
 *  Return:      None
 *
 **********************************************************************/
void CPlaneFit::AddPoint(Vector3CRef i_point)
{
	ASSERT(!Math::IsNaN(i_point.x()) && !Math::IsNaN(i_point.y()) &&
			!Math::IsNaN(i_point.z()));

	d_pointSum += i_point;
	d_sumXX += i_point.x() * i_point.x();
	d_sumXY += i_point.x() * i_point.y();
	d_sumXZ += i_point.x() * i_point.z();
	d_sumYY += i_point.y() * i_point.y();
	d_sumYZ += i_point.y() * i_point.z();
	d_sumZZ += i_point.z() * i_point.z();
	d_numPoints++;
}

/**********************************************************************
 *
 *  Class:       CPlaneFit
 *  Function:    Fit
 *  Access:      public
 *  Description: Uses a least squares method to fit a plane to a
 *               collection of points.
 *  Arguments:
 *      i_hint   - A vector that indicates the probable direction of
 *                 the normal to the plane.
 *      o_loc    - A point on the plane. It is the average of the
 *                 input points.
 *      o_normal - The normal to the plane.
 *  Return:      An enumeration indicating the nature of the input
 *               data and the resulting fit.
 *
 *               PLANE_FIT_NO_POINTS - There are no points to fit.
 *
 *               PLANE_FIT_COAXIAL - The points are coincident.
 *
 *               PLANE_FIT_COPLANAR - The points all lie on line.
 *
 *               PLANE_FIT_SCATTER - The typical case where three or
 *               more points are arranged such that a plane can be
 *               fit to them.
 *
 **********************************************************************/
CPlaneFit::EPlaneFitResult CPlaneFit::Fit(Vector3 &i_hint, Vector3 &o_loc,
		Vector3 &o_normal)
{
/*
 *	If there are no points, set the location to the zero vector and the
 *	normal to the hint vector.
 */
	if (d_numPoints == 0)
	{
		o_loc.Clear();
		o_normal = i_hint;
/*
 *		Normalize the hint vector. If it is zero-length, arbitrarily use the
 *		y-axis.
 */
		if (!o_normal.SafeNormalize())
			o_normal.Set(0.0f, 1.0f, 0.0f);
		return(PLANE_FIT_NO_POINTS);
	}
/*
 *	Find the inverse of the number of points.
 */
	float numPointsInverse;
	 numPointsInverse = 1.0f / (float)d_numPoints;
/*
 *	The average of the collection of points will lie on the plane.
 */
	o_loc = numPointsInverse * d_pointSum;
	ASSERT(!Math::IsNaN(o_loc.x()) && !Math::IsNaN(o_loc.y()) &&
			!Math::IsNaN(o_loc.z()));
/*
 *	Express the sums in a coordinate system aligned with the fit system
 *	and centered at the average point.
 */
	float Rxx = d_sumXX - numPointsInverse * d_pointSum.x() * d_pointSum.x();
	float Rxy = d_sumXY - numPointsInverse * d_pointSum.x() * d_pointSum.y();
	float Rxz = d_sumXZ - numPointsInverse * d_pointSum.x() * d_pointSum.z();
	float Ryy = d_sumYY - numPointsInverse * d_pointSum.y() * d_pointSum.y();
	float Ryz = d_sumYZ - numPointsInverse * d_pointSum.y() * d_pointSum.z();
	float Rzz = d_sumZZ - numPointsInverse * d_pointSum.z() * d_pointSum.z();
	ASSERT ((Rxx >= 0.0f) && (Ryy >= 0.0f) && (Rzz >= 0.0f));
/*
 *	Find the component of the normal in each axis, if that axis were used
 *	to fit.
 */
	float fit[3];
	fit[0] = Ryy * Rzz - Ryz * Ryz;
	fit[1] = Rxx * Rzz - Rxz * Rxz;
	fit[2] = Rxx * Ryy - Rxy * Rxy;
/*
 *	Fit to the axis with the largest component. (It can be shown that all of
 *	the values are >= 0.)
 */
	int fitAxis;
	if (fit[0] > fit[1])
	{
		if (fit[0] > fit[2])
			fitAxis = 0;
		else
			fitAxis = 2;
	}
	else if (fit[1] > fit[2])
		fitAxis = 1;
	else
		fitAxis = 2;
/*
 *	Check to see if all the points fall on a line.
 */
	if (fit[fitAxis] < 1.0e-4f)
	{
/*
 *		If the magnitude of the vector is 0, all points are coincident.
 *		Use the hint vector.
 */
		if ((Rxx + Ryy + Rzz) < 1.0e-4f)
		{
			o_normal = i_hint.Normalized();
			ASSERT(!Math::IsNaN(o_normal.x()) && !Math::IsNaN(o_normal.y()) &&
					!Math::IsNaN(o_normal.z()));
			return(PLANE_FIT_COAXIAL);
		}
/*
 *		Find the line then set the normal to be perpendicular to the line
 *		and oriented such that the hint vector is in the plane defined by the
 *		normal and the line. If l is the line and h is the hint vector:
 *
 *			lx(hxl)
 *
 *		will yield such a vector.
 */
		Vector3 line;
		line.x( Math::Sqrt(Rxx));
		if (Rxy >= 0.0f)
			line.y( Math::Sqrt(Ryy));
		else
			line.y( -Math::Sqrt(Ryy));
		if (Rxz >= 0.0f)
			line.z( Math::Sqrt(Rzz));
		else
			line.z( -Math::Sqrt(Rzz));
		o_normal = (line.Cross(i_hint.Cross(line))).Normalized();

		ASSERT(!Math::IsNaN(o_normal.x()) && !Math::IsNaN(o_normal.y()) &&
				!Math::IsNaN(o_normal.z()));
		return(PLANE_FIT_COPLANAR);
	}
/*
 *	Form the non-unit normal vector.
 */
	switch(fitAxis)
	{
	case 0:
		 o_normal.Set(fit[0],
						  Rxz * Ryz - Rzz * Rxy,
						  Ryz * Rxy - Ryy * Rxz);
		break;
	case 1:
		 o_normal.Set(Rxz * Ryz - Rzz * Rxy,
						  fit[1],
						  Rxz * Rxy - Ryz * Rxx);
		break;
	default:
		 o_normal.Set(Rxy * Ryz - Rxz * Ryy,
						  Rxz * Rxy - Ryz * Rxx,
						  fit[2]);
		break;
	}
/*
 *	Normalize and make sure the normal points in the direction of the
 *	hint vector.
 */
	o_normal.Normalize();
	if (o_normal.Dot(i_hint) < 0.0f)
		o_normal.Negate();

	ASSERT(!Math::IsNaN(o_normal.x()) && !Math::IsNaN(o_normal.y()) &&
			!Math::IsNaN(o_normal.z()));
	return(PLANE_FIT_SCATTER);
}

/**********************************************************************
 *
 *  Class:       CPlaneFit
 *  Function:    Fit (static)
 *  Access:      public
 *  Description: Uses a least squares method to fit a plane to a
 *               collection of points.
 *  Arguments:
 *      i_numPoints - The number of points to fit.
 *      i_points    - The array of points.
 *      i_hint      - A guess at the direction of the normal.
 *      o_loc       - A point on the plane. It is the average of the
 *                    input points.
 *      o_normal    - The normal to the plane.
 *  Return:      An enumeration indicating the nature of the input
 *               data and the resulting fit.
 *
 *               PLANE_FIT_NO_POINTS - There are no points to fit.
 *
 *               PLANE_FIT_COAXIAL - The points are coincident.
 *
 *               PLANE_FIT_COPLANAR - The points all lie on line.
 *
 *               PLANE_FIT_SCATTER - The typical case where three or
 *               more points are arranged such that a plane can be
 *               fit to them.
 *
 **********************************************************************/
CPlaneFit::EPlaneFitResult CPlaneFit::Fit(uint i_numPoints,
		const Vector3* i_points, Vector3CRef i_hint, Vector3 &o_loc,
		Vector3 &o_normal)
{
/*
 *	If there are no points, set the location to the zero vector and the
 *	normal to the hint vector.
 */
	if (i_numPoints == 0)
	{
		o_loc.Clear();
		o_normal = i_hint;
/*
 *		Normalize the hint vector. If it is zero-length, arbitrarily use the
 *		y-axis.
 */
		if (!o_normal.SafeNormalize())
			o_normal.Set(0.0f, 1.0f, 0.0f);
		return(PLANE_FIT_NO_POINTS);
	}
/*
 *	Find the average point. It lies on the fit plane.
 */
	uint i;
	o_loc.Clear();
	for (i = 0; i < i_numPoints; i++)
		o_loc += i_points[i];
	o_loc *= 1.0f / (float)i_numPoints;
	ASSERT(!Math::IsNaN(o_loc.x()) && !Math::IsNaN(o_loc.y()) &&
			!Math::IsNaN(o_loc.z()));
/*
 *	Find sums of products in the fit system.
 */
	float Rxx = 0.0f;
	float Rxy = 0.0f;
	float Rxz = 0.0f;
	float Ryy = 0.0f;
	float Ryz = 0.0f;
	float Rzz = 0.0f;
	for (i = 0; i < i_numPoints; i++)
	{
		Rxx += i_points[i].x() * i_points[i].x();
		Rxy += i_points[i].x() * i_points[i].y();
		Rxz += i_points[i].x() * i_points[i].z();
		Ryy += i_points[i].y() * i_points[i].y();
		Ryz += i_points[i].y() * i_points[i].z();
		Rzz += i_points[i].z() * i_points[i].z();
	}
	ASSERT ((Rxx >= 0.0f) && (Ryy >= 0.0f) && (Rzz >= 0.0f));
/*
 *	Find the component of the normal in each axis, if that axis were used
 *	to fit.
 */
	float fit[3];
	fit[0] = Ryy * Rzz - Ryz * Ryz;
	fit[1] = Rxx * Rzz - Rxz * Rxz;
	fit[2] = Rxx * Ryy - Rxy * Rxy;
/*
 *	Fit to the axis with the largest component. (It can be shown that all of
 *	the values are >= 0.)
 */
	int fitAxis;
	if (fit[0] > fit[1])
	{
		if (fit[0] > fit[2])
			fitAxis = 0;
		else
			fitAxis = 2;
	}
	else if (fit[1] > fit[2])
		fitAxis = 1;
	else
		fitAxis = 2;
/*
 *	Check to see if all the points fall on a line.
 */
	if (fit[fitAxis] < 1.0e-4f)
	{
/*
 *		If the magnitude of the vector is 0, all points are coincident.
 *		Use the hint vector.
 */
		if ((Rxx + Ryy + Rzz) < 1.0e-4f)
		{
			o_normal = i_hint.Normalized();
			ASSERT(!Math::IsNaN(o_normal.x()) && !Math::IsNaN(o_normal.y()) &&
					!Math::IsNaN(o_normal.z()));
			return(PLANE_FIT_COAXIAL);
		}
/*
 *		Find the line then set the normal to be perpendicular to the line
 *		and oriented such that the hint vector is in the plane defined by the
 *		normal and the line. If l is the line and h is the hint vector:
 *
 *			lx(hxl)
 *
 *		will yield such a vector.
 */
		Vector3 line;
		line.x( Math::Sqrt(Rxx));
		if (Rxy >= 0.0f)
			line.y( Math::Sqrt(Ryy));
		else
			line.y( -Math::Sqrt(Ryy));
		if (Rxz >= 0.0f)
			line.z( Math::Sqrt(Rzz));
		else
			line.z( -Math::Sqrt(Rzz));
		o_normal = (line.Cross(i_hint.Cross(line))).Normalized();

		ASSERT(!Math::IsNaN(o_normal.x()) && !Math::IsNaN(o_normal.y()) &&
				!Math::IsNaN(o_normal.z()));
		return(PLANE_FIT_COPLANAR);
	}
/*
 *	Form the non-unit normal vector.
 */
	switch(fitAxis)
	{
	case 0:
		 o_normal.Set(fit[0],
						  Rxz * Ryz - Rzz * Rxy,
						  Ryz * Rxy - Ryy * Rxz);
		break;
	case 1:
		 o_normal.Set(Rxz * Ryz - Rzz * Rxy,
						  fit[1],
						  Rxz * Rxy - Ryz * Rxx);
		break;
	default:
		 o_normal.Set(Rxy * Ryz - Rxz * Ryy,
						  Rxz * Rxy - Ryz * Rxx,
						  fit[2]);
		break;
	}
/*
 *	Normalize and make sure the normal points in the direction of the
 *	hint vector.
 */
	o_normal.Normalize();
	if (o_normal.Dot(i_hint) < 0.0f)
		o_normal.Negate();

	ASSERT(!Math::IsNaN(o_normal.x()) && !Math::IsNaN(o_normal.y()) &&
			!Math::IsNaN(o_normal.z()));
	return(PLANE_FIT_SCATTER);
}

/**********************************************************************
 *
 *  Class:       CPlaneFit
 *  Function:    TransformInit
 *  Access:      public
 *  Description: Prepares to fit a plane to a collection of points.
 *               Points will be accumulated then a plane will be fit.
 *  Arguments:
 *      i_fit  - The fit direction. The algorithm finds a plane that
 *               minimizes the sum of the squares of the distances from
 *               the points to the plane along the fit vector.
 *  Return:      true if the fit vector could be normalized; otherwise,
 *               false. Valid initialization is performed in either
 *               case. If the fit vector could not be normalized,
 *               the world y-axis is used to fit.
 *
 **********************************************************************/
bool CPlaneFit::TransformInit(Vector3CRef i_fit)
{
	ASSERT(!Math::IsNaN(i_fit.x()) && !Math::IsNaN(i_fit.y()) &&
			!Math::IsNaN(i_fit.z()));
/*
 *	Make sure i_fit is a unit vector.
 */
	bool result;
	Vector3 fit;
	float len = i_fit.Length();
	if (Math::DivByZero(1.0f, len))
	{
		fit.Set(0.0f, 1.0f, 0.0f);
		result = false;
	}
	else
	{
		fit = (1.0f / len) * i_fit;
		result = true;
	}
/*
 *	Form a transformation to a system where the fit vector is the y-axis.
 *	Let the x-axis of the system be the cross product of the fit vector and
 *	one of the principle axes of the world.
 *
 *	Select the world axis that gives the most stable results.
 */
	int xIndex = 0;
	float xMag = Math::Abs(fit.x());
	float mag = Math::Abs(fit.y());
	if (mag < xMag)
	{
		xMag = mag;
		xIndex = 1;
	}
	if (Math::Abs(fit.z()) < xMag)
		xIndex = 2;
	Vector3 xAxis;
	switch(xIndex)
	{
	case 0:
		xAxis.Set(0.0f, fit.z(), -fit.y());
		break;
	case 1:
		xAxis.Set(-fit.z(), 0.0f, fit.x());
		break;
	default:
		xAxis.Set(fit.y(), -fit.x(), 0.0f);
		break;
	}
/*
 *	Build the transformation.
 */
	xAxis.Normalize();
	d_worldToFit.SetColumn0(xAxis);
	d_worldToFit.SetColumn1(fit);
	d_worldToFit.SetColumn2(xAxis.Cross(fit));
/*
 *	Initialize the point counter a accumulation buffers.
 */
	d_numPoints = 0;
	d_pointSum.Clear();
	d_sumXX = 0.0f;
	d_sumXY = 0.0f;
	d_sumXZ = 0.0f;
	d_sumYY = 0.0f;  // Just in case.
	d_sumYZ = 0.0f;
	d_sumZZ = 0.0f;

	return(result);
}

/**********************************************************************
 *
 *  Class:       CPlaneFit
 *  Function:    AddTransformPoint
 *  Access:      public
 *  Description: Adds a point to the collection to be fit.
 *  Arguments:
 *      i_point     - The point.
 *  Return:      None
 *
 **********************************************************************/
void CPlaneFit::AddTransformPoint(Vector3CRef i_point)
{
	ASSERT(!Math::IsNaN(i_point.x()) && !Math::IsNaN(i_point.y()) &&
			!Math::IsNaN(i_point.z()));

	Vector3 fitPoint = i_point * d_worldToFit;
	d_pointSum += fitPoint;
	d_sumXX += fitPoint.x() * fitPoint.x();
	d_sumXY += fitPoint.x() * fitPoint.y();
	d_sumXZ += fitPoint.x() * fitPoint.z();
	d_sumYY += fitPoint.y() * fitPoint.y();  // Just in case.
	d_sumYZ += fitPoint.y() * fitPoint.z();
	d_sumZZ += fitPoint.z() * fitPoint.z();
	d_numPoints++;
}

/**********************************************************************
 *
 *  Class:       CPlaneFit
 *  Function:    TransformFit
 *  Access:      public
 *  Description: Uses a least squares method to fit a plane to a
 *               collection of points.
 *  Arguments:
 *      o_loc    - A point on the plane. It is the average of the
 *                 input points.
 *      o_normal - The normal to the plane.
 *  Return:      An enumeration indicating the nature of the input
 *               data and the resulting fit.
 *
 *               PLANE_FIT_NO_POINTS - There are no points to fit.
 *               The origin is used as the plane point and the fit
 *               vector is used as the normal vector.
 *
 *               PLANE_FIT_COAXIAL - The points all lie on a line
 *               aligned with the fit vector. This includes the case
 *               where there is only one point. The fit vector is
 *               used as the normal vector.
 *
 *               PLANE_FIT_COPLANAR - The points all lie on a plane
 *               that contains the fit vector. This includes the two
 *               point case. The normal vector lies in the plane and
 *               is perpendicular to a line fit to the points.
 *
 *               PLANE_FIT_SCATTER - The typical case where three or
 *               more points are arranged such that a plane can be
 *               fit to them.
 *
 **********************************************************************/
CPlaneFit::EPlaneFitResult CPlaneFit::TransformFit(Vector3 &o_loc,
		Vector3 &o_normal)
{
/*
 *	If there are no points, set the location to the zero vector and the
 *	normal to the fit vector.
 */
	if (d_numPoints == 0)
	{
		o_loc.Clear();
		  o_normal = d_worldToFit.GetColumn1();
		return(PLANE_FIT_NO_POINTS);
	}
/*
 *	Find the inverse of the number of points.
 */
	float numPointsInverse;
	 numPointsInverse = 1.0f / (float)d_numPoints;
/*
 *	The average of the collection of points will lie on the plane.
 */
	o_loc = numPointsInverse * (d_pointSum / d_worldToFit);
	ASSERT(!Math::IsNaN(o_loc.x()) && !Math::IsNaN(o_loc.y()) &&
			!Math::IsNaN(o_loc.z()));
/*
 *	Express the sums in a coordinate system aligned with the fit system
 *	and centered at the average point.
 */
	float Rxx = d_sumXX - numPointsInverse * d_pointSum.x() * d_pointSum.x();
	float Rxy = d_sumXY - numPointsInverse * d_pointSum.x() * d_pointSum.y();
	float Rxz = d_sumXZ - numPointsInverse * d_pointSum.x() * d_pointSum.z();
	float Ryz = d_sumYZ - numPointsInverse * d_pointSum.y() * d_pointSum.z();
	float Rzz = d_sumZZ - numPointsInverse * d_pointSum.z() * d_pointSum.z();
/*
 *	If Rxx and Rzz are zero, all the points lie on the y-axis (fit vector)
 *	(or there are zero points). Use the fit vector as the normal.
 *	Note that since Rxx and Rzz are sums of squares, they must be positive.
 */
	 if (Rxx < 0.0f)
	 {
		 ASSERT(Rxx > -0.02f);
		 Rxx = 0.0f;
	 }
	 if (Rzz < 0.0f)
	 {
		 ASSERT(Rzz > -0.02f);
		 Rzz = 0.0f;
	 }
	if (((Rxx < 1.0e-4f) && (Rzz < 1.0e-4f)) || (d_numPoints == 1))
	{
		o_normal = d_worldToFit.GetColumn1();
		ASSERT(!Math::IsNaN(o_normal.x()) && !Math::IsNaN(o_normal.y()) &&
				!Math::IsNaN(o_normal.z()));
		return(PLANE_FIT_COAXIAL);
	}
/*
 *	Form the non-unit normal vector in fit coordinates and find its length.
 */
	o_normal.Set(Rxz * Ryz - Rzz * Rxy,
					  Rxx * Rzz - Rxz * Rxz,
					  Rxz * Rxy - Ryz * Rxx);
	float len = o_normal.Length();
/*
 *	If the magnitude of the computed normal is zero, all points are
 *	coplanar with the fit vector.
 */
	if (Math::DivByZero(1.0f, len) || (d_numPoints == 2))
	{
/*
 *		Fit a line to the points using the larger of Rxx and Rzz (at least
 *		one is non-zero) then set the normal to be perpendicular to the line
 *		and oriented such that the fit vector is in the plane defined by the
 *		normal and the line. If l is the line and f is the fit vector:
 *
 *			lx(fxl)
 *
 *		will yield such a vector. Since the fit vector is [0 1 0] in the
 *		fit system, the operation can be worked out in advance.
 */
		if (Rxx > Rzz)
			o_normal.Set (-Rxx * Rxy, Rxx * Rxx + Rxz * Rxz, -Rxy * Rxz);
		else
			o_normal.Set (-Rxz * Ryz, Rxz * Rxz + Rzz * Rzz, -Ryz * Rzz);
		o_normal = o_normal.Normalized() / d_worldToFit;

		ASSERT(!Math::IsNaN(o_normal.x()) && !Math::IsNaN(o_normal.y()) &&
				!Math::IsNaN(o_normal.z()));
		return(PLANE_FIT_COPLANAR);
	}
/*
 *	Make the normal a unit vector in world coordinates.
 */
	o_normal = ((1.0f / len) * o_normal) / d_worldToFit;

	ASSERT(!Math::IsNaN(o_normal.x()) && !Math::IsNaN(o_normal.y()) &&
			!Math::IsNaN(o_normal.z()));
	return(PLANE_FIT_SCATTER);
}

/**********************************************************************
 *
 *  Class:       CPlaneFit
 *  Function:    TransformFit (static)
 *  Access:      public
 *  Description: Uses a least squares method to fit a plane to a
 *               collection of points.
 *  Arguments:
 *      i_numPoints - The number of points to fit.
 *      i_points    - The array of points.
 *      i_fit       - The fit direction. The algorithm finds a plane
 *                    that minimizes the sum of the squares of the
 *                    distances from the points to the plane along
 *                    the fit vector.
 *      o_loc       - A point on the plane. It is the average of the
 *                    input points.
 *      o_normal    - The normal to the plane.
 *  Return:      An enumeration indicating the nature of the input
 *               data and the resulting fit.
 *
 *               PLANE_FIT_NO_POINTS - There are no points to fit.
 *               The origin is used as the plane point and the fit
 *               vector is used as the normal vector.
 *
 *               PLANE_FIT_COAXIAL - The points all lie on a line
 *               aligned with the fit vector. This includes the case
 *               where there is only one point. The zero point case is
 *               treated as if there were one point at the origin. The
 *               fit vector is used as the normal vector.
 *
 *               PLANE_FIT_COPLANAR - The points all lie on a plane
 *               that contains the fit vector. This includes the two
 *               point case. The normal vector lies in the plane and
 *               is perpendicular to a line fit to the points.
 *
 *               PLANE_FIT_SCATTER - The typical case where three or
 *               more points are arranged such that a plane can be
 *               fit to them.
 *
 **********************************************************************/
CPlaneFit::EPlaneFitResult CPlaneFit::TransformFit(uint i_numPoints,
		const Vector3* i_points, Vector3CRef i_fit, Vector3 &o_loc,
		Vector3 &o_normal)
{
/*
 *	Make sure i_fit is a unit vector.
 */
	Vector3 fit;
	float len = i_fit.Length();
	if (Math::DivByZero(1.0f, len))
		fit.Set(0.0f, 1.0f, 0.0f);
	else
		fit = (1.0f / len) * i_fit;
/*
 *	Form a transformation to a system where the fit vector is the y-axis.
 *	Let the x-axis of the system be the cross product of the fit vector and
 *	one of the principle axes of the world.
 *
 *	Select the world axis that gives the most stable results.
 */
	int xIndex = 0;
	float xMag = Math::Abs(fit.x());
	float mag = Math::Abs(fit.y());
	if (mag < xMag)
	{
		xMag = mag;
		xIndex = 1;
	}
	if (Math::Abs(fit.z()) < xMag)
		xIndex = 2;
	Vector3 xAxis;
	switch(xIndex)
	{
	case 0:
		xAxis.Set(0.0f, fit.z(), -fit.y());
		break;
	case 1:
		xAxis.Set(-fit.z(), 0.0f, fit.x());
		break;
	default:
		xAxis.Set(fit.y(), -fit.x(), 0.0f);
		break;
	}
/*
 *	If there are no points, set the location to the zero vector and the
 *	normal to the fit vector.
 */
	if (i_numPoints == 0)
	{
		o_loc.Clear();
		o_normal = fit;
		return(PLANE_FIT_NO_POINTS);
	}
/*
 *	Build the transformation.
 */
	xAxis.Normalize();
	DirCos3x3 worldToFit;
	worldToFit.SetColumn0(xAxis);
	worldToFit.SetColumn1(fit);
	worldToFit.SetColumn2(xAxis.Cross(fit));
/*
 *	Find the average point. It lies on the fit plane.
 */
	uint i;
	o_loc.Clear();
	for (i = 0; i < i_numPoints; i++)
		o_loc += i_points[i];
	o_loc *= 1.0f / (float)i_numPoints;
	ASSERT(!Math::IsNaN(o_loc.x()) && !Math::IsNaN(o_loc.y()) &&
			!Math::IsNaN(o_loc.z()));
/*
 *	Find sums of products in the fit system.
 */
	float Rxx = 0.0f;
	float Rxy = 0.0f;
	float Rxz = 0.0f;
	float Ryz = 0.0f;
	float Rzz = 0.0f;
	Vector3 temp;
	for (i = 0; i < i_numPoints; i++)
	{
		temp = (i_points[i] - o_loc) * worldToFit;
		Rxx += temp.x() * temp.x();
		Rxy += temp.x() * temp.y();
		Rxz += temp.x() * temp.z();
		Ryz += temp.y() * temp.z();
		Rzz += temp.z() * temp.z();
	}
/*
 *	If Rxx and Rzz are zero, all the points lie on the y-axis (fit vector)
 *	(or there are zero points). Use the fit vector as the normal.
 *	Note that since Rxx and Rzz are sums of squares, they must be positive.
 */
	ASSERT ((Rxx >= 0.0f) && (Rzz >= 0.0f));
	if ((Rxx < 1.0e-4f) && (Rzz < 1.0e-4f))
	{
		o_normal = fit;
		ASSERT(!Math::IsNaN(o_normal.x()) && !Math::IsNaN(o_normal.y()) &&
				!Math::IsNaN(o_normal.z()));
		return(PLANE_FIT_COAXIAL);
	}
/*
 *	Form the non-unit normal vector in fit coordinates and find its length.
 */
	o_normal.Set(Rxz * Ryz - Rzz * Rxy, Rxx * Rzz - Rxz * Rxz,
			Rxz * Rxy - Ryz * Rxx);
	if (o_normal.Y() < 0.0f)
		o_normal.Negate();
	len = o_normal.Length();
/*
 *	If the magnitude of the computed normal is zero, all points are
 *	coplanar with the fit vector.
 */
	if (Math::DivByZero(1.0f, len))
	{
/*
 *		Fit a line to the points using the larger of Rxx and Rzz (at least
 *		one is non-zero) then set the normal to be perpendicular to the line
 *		and oriented such that the fit vector is in the plane defined by the
 *		normal and the line. If l is the line and f is the fit vector:
 *
 *			lx(fxl)
 *
 *		will yield such a vector. Since the fit vector is [0 1 0] in the
 *		fit system, the operation can be worked out in advance.
 */
		if (Rxx > Rzz)
			o_normal.Set (-Rxx * Rxy, Rxx * Rxx + Rxz * Rxz, -Rxy * Rxz);
		else
			o_normal.Set (-Rxz * Ryz, Rxz * Rxz + Rzz * Rzz, -Ryz * Rzz);
		if (o_normal.Y() < 0.0f)
			o_normal.Negate();
		o_normal = o_normal.Normalized() / worldToFit;

		ASSERT(!Math::IsNaN(o_normal.x()) && !Math::IsNaN(o_normal.y()) &&
				!Math::IsNaN(o_normal.z()));
		return(PLANE_FIT_COPLANAR);
	}
/*
 *	Make the normal a unit vector in world coordinates.
 */
	o_normal = ((1.0f / len) * o_normal) / worldToFit;

	ASSERT(!Math::IsNaN(o_normal.x()) && !Math::IsNaN(o_normal.y()) &&
			!Math::IsNaN(o_normal.z()));
	return(PLANE_FIT_SCATTER);
}

/**********************************************************************
 *
 *  Class:       CPlaneFit
 *  Function:    TransformFit (static)
 *  Access:      public
 *  Description: Uses a least squares method to fit a plane to a
 *               collection of points, constaining the normal to lie
 *               on a plane.
 *  Arguments:
 *      i_numPoints - The number of points to fit.
 *      i_points    - The array of points.
 *      i_fit       - The fit direction. The algorithm finds a plane
 *                    that minimizes the sum of the squares of the
 *                    distances from the points to the plane along
 *                    the fit vector.
 *      i_plane     - Normal to the plane the fit normal must lie in.
 *      o_loc       - A point on the plane. It is the average of the
 *                    input points.
 *      o_normal    - The normal to the plane.
 *  Return:      An enumeration indicating the nature of the input
 *               data and the resulting fit.
 *
 *               PLANE_FIT_NO_POINTS - There are no points to fit.
 *               The origin is used as the plane point and the fit
 *               vector is used as the normal vector.
 *
 *               PLANE_FIT_COAXIAL - The points all lie on a line
 *               aligned with the fit vector. This includes the case
 *               where there is only one point. The zero point case is
 *               treated as if there were one point at the origin. The
 *               fit vector is used as the normal vector.
 *
 *               PLANE_FIT_SCATTER - The typical case where three or
 *               more points are arranged such that a plane can be
 *               fit to them.
 *
 **********************************************************************/
CPlaneFit::EPlaneFitResult CPlaneFit::TransformFit(uint i_numPoints,
		const Vector3* i_points, Vector3CRef i_fit, Vector3CRef i_plane,
		Vector3 &o_loc, Vector3 &o_normal)
{
/*
 *	Setup as if there are no points.
 */
	EPlaneFitResult result = PLANE_FIT_NO_POINTS;
	o_loc.Clear();
/*
 *	Find the side vector, then the fit vector.
 */
	Vector3 side = i_plane.Cross(i_fit);
	if (side.LengthSquared() < 1.0e-4f)
	{
		Vector3 absPlane = i_plane.Abs();
		if (absPlane.X() < absPlane.Y())
		{
			if (absPlane.X() < absPlane.Z())
				side.Set(0.0f, i_plane.Z(), -i_plane.Y());
			else
				side.Set(i_plane.Y(), -i_plane.X(), 0.0f);
		}
		else if (absPlane.Y() < absPlane.Z())
			side.Set(-i_plane.Z(), 0.0f, i_plane.X());
		else
			side.Set(i_plane.Y(), -i_plane.X(), 0.0f);
	}
	o_normal = side.Cross(i_plane);
/*
 *	Process the points.
 */
	if (i_numPoints > 0)
	{
/*
 *		Find the average point. It lies on the fit plane.
 */
		uint i;
		for (i = 0; i < i_numPoints; i++)
			o_loc += i_points[i];
		o_loc *= 1.0f / (float)i_numPoints;
		ASSERT(!Math::IsNaN(o_loc.x()) && !Math::IsNaN(o_loc.y()) &&
				!Math::IsNaN(o_loc.z()));
/*
 *		Find sums of products in the fit system.
 */
		float locY = o_normal.Dot(o_loc);
		float locZ = side.Dot(o_loc);
		float Ryz = 0.0f;
		float Rzz = 0.0f;
		for (i = 0; i < i_numPoints; i++)
		{
			float Ry = o_normal.Dot(i_points[i]) - locY;
			float Rz = side.Dot(i_points[i]) - locZ;
			Ryz += Ry * Rz;
			Rzz += Rz * Rz;
		}
/*
 *		If Rzz is zero, bad control vectors were passed in or all the points
 *		lie on a line perpendicular to the side vector.
 */
		if (!Math::DivByZero(Ryz, Rzz))
		{
			o_normal -= (Ryz / Rzz) * side;
			result = PLANE_FIT_SCATTER;
		}
		else
			result = PLANE_FIT_COAXIAL;
	}
/*
 *	If the normal is bad, bad control vectors were passed in. Use the no point
 *	result to call attention to the failure.
 */
	if (!o_normal.SafeNormalize())
	{
		ASSERTF(false, ("Constrained plane fit:\n\tPoints: %d\n\t"
				"Fit: [%f %f %f]\n\tPlane: [%f %f %f]", i_numPoints,
				(float)i_fit.X(), (float)i_fit.Y(), (float)i_fit.Z(),
				(float)i_plane.X(), (float)i_plane.Y(), (float)i_plane.Z()));
		result = PLANE_FIT_NO_POINTS;
/*
 *		Find a vector perpendicular to the plane normal by crossing it with
 *		the principal axis that yields the largest magnitude result.
 */
		Vector3 absPlane = i_plane.Abs();
		if (absPlane.X() < absPlane.Y())
		{
			if (absPlane.X() < absPlane.Z())
				o_normal.Set(0.0f, i_plane.Z(), -i_plane.Y());
			else
				o_normal.Set(i_plane.Y(), -i_plane.X(), 0.0f);
		}
		else if (absPlane.Y() < absPlane.Z())
			o_normal.Set(-i_plane.Z(), 0.0f, -i_plane.X());
		else
			o_normal.Set(i_plane.Y(), -i_plane.X(), 0.0f);
/*
 *		If this vector has zero length, the plane normal is zero length. Try
 *		to use the fit vector. If that fails, just pick something.
 */
		if (!o_normal.SafeNormalize())
		{
			o_normal = i_fit;
			if (!o_normal.SafeNormalize())
				o_normal.Set(0.0f, 1.0f, 0.0f);
		}
	}
	return(result);
}
