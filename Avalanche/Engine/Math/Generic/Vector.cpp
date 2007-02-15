/**************************************************************************/
/* some vector routines that are big enough that they shouldn't be inline */
/**************************************************************************/
#include "Math/MathPCH.h"

// Some useful constants
const Vector3 c_origin(0.0f, 0.0f, 0.0f);
const Vector3 c_up(0.0f, 1.0f, 0.0f);
const Vector3 c_down(0.0f, -1.0f, 0.0f);

/***************************************************************************/
// handy utility (axis should be normalized)
/***************************************************************************/
Vector3 RotatePointAroundAxis(
Vector3CRef p,
Vector3CRef axis,
float theta)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(ROTATE_POINT_AROUND_AXIS);
#endif //MATH_STATS

	float cTheta, sTheta;
	Math::SinCos(theta, sTheta, cTheta);

	return(cTheta * p +
			 (1.0f - cTheta) * axis * Vector3::Dot(axis, p) +
			 sTheta * Vector3::Cross(axis, p));
}

/***************************************************************************/
// NearestPointOnLineSeg
//    Utility to compute the nearest point on a line segment to a point.
//    Two versions are provided each for Vector2 and Vector3.
//    This technique calculates this by doing an orthogonal projection
//    of the point on the line, then making sure the point is on the line
//    segment.  If it's not then the closest endpoint of the line segment
//    is used.
/***************************************************************************/
void NearestPointOnLineSeg(
const Vector2 &p,
const Vector2 &l1,
const Vector2 &l2,
Vector2 &out)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(NEAREST_POINT_ON_LINE_SEG_V2);
#endif //MATH_STATS

	//---------------------------------------------
	//  ..........IMPORTANT NOTE..........
	//  If you make changes to this code you must
	//  seriously consider making the same changes
	//  to the 3D version of this routine, below.
	//---------------------------------------------
	Vector2 A = l2 - l1;
	float dot = A.Dot(p - l1);

	// If the dot product is 0 or less than one, then the vectors are pointing
	// away from each other and we can use (l1);
	if (dot <= .0001f)
	{
		out = l1;
		return;
	}

	out = l1 + A * dot / A.LengthSquared();

	// Do a bounding rectangle check to see if the point is actually on the line.
	if (out.x() < Math::Min(l1.x(), l2.x()) || out.x() > Math::Max(l1.x(), l2.x()) ||
		out.y() < Math::Min(l1.y(), l2.y()) || out.y() > Math::Max(l1.y(), l2.y()))
	{
		// We know the point isn't on the line, and we know that the vectors
		// aren't opposite direction.
		out = l2;
	}
}

/***************************************************************************/
//  3D Version
/***************************************************************************/
void NearestPointOnLineSeg(
Vector3CRef p,
Vector3CRef l1,
Vector3CRef l2,
Vector3 &out)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(NEAREST_POINT_ON_LINE_SEG_V3);
#endif //MATH_STATS

	//---------------------------------------------
	//  ..........IMPORTANT NOTE..........
	//  If you make changes to this code you must
	//  seriously consider making the same changes
	//  to the 2D version of this routine, above.
	//---------------------------------------------
	Vector3 A = l2-l1;
	float dot = A.Dot(p-l1);

	// If the dot product is 0 or less than one, then the vectors are pointing
	// away from each other and we can use (l1);
	if (dot <= .0001f)
	{
		out = l1;
		return;
	}

	out = l1 + A * dot / A.LengthSquared();

	// Do a bounding rectangle check to see if the point is actually on the line.
	if (out.x() < Math::Min(l1.x(), l2.x()) || out.x() > Math::Max(l1.x(), l2.x()) ||
		out.y() < Math::Min(l1.y(), l2.y()) || out.y() > Math::Max(l1.y(), l2.y()) ||
		out.z() < Math::Min(l1.z(), l2.z()) || out.z() > Math::Max(l1.z(), l2.z()))
	{
		// We know the point isn't on the line, and we know that the vectors
		// aren't opposite direction.
		out = l2;
	}
}

/***************************************************************************/
// NearestPointOnLine
//    Find the closest point on an unbounded line by doing an
//    orthogonal projection of the point on the line.
/***************************************************************************/
void NearestPointOnLine(
const Vector2 &p,
const Vector2 &l1,
const Vector2 &l2,
Vector2 &out)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(NEAREST_POINT_ON_LINE_V2);
#endif //MATH_STATS

	//---------------------------------------------
	//  ..........IMPORTANT NOTE..........
	//  If you make changes to this code you must
	//  seriously consider making the same changes
	//  to the 3D version of this routine, below.
	//---------------------------------------------
	Vector2 A = l2 - l1;
	out = l1 + A.Dot(p-l1) * A / A.LengthSquared();
}

/***************************************************************************/
//  3D Version
/***************************************************************************/
void NearestPointOnLine(
Vector3CRef p,
Vector3CRef l1,
Vector3CRef l2,
Vector3 &out)
{
#ifdef MATH_STATS	// defined in Math\MathStats.h
	MATH_STATS_INC(NEAREST_POINT_ON_LINE_V3);
#endif //MATH_STATS

	//---------------------------------------------
	//  ..........IMPORTANT NOTE..........
	//  If you make changes to this code you must
	//  seriously consider making the same changes
	//  to the 2D version of this routine, above.
	//---------------------------------------------
	Vector3 A = l2 - l1;
	out = l1 + A.Dot(p-l1) * A / A.LengthSquared();
}


/***************************************************************************/
 //  Convenience functions
/***************************************************************************/
Vector2 NearestPointOnLineSeg(
const Vector2& p,
const Vector2& l1,
const Vector2& l2)
{
	Vector2 out;
	NearestPointOnLineSeg(p, l1, l2, out);
	return(out);
}

/***************************************************************************/
/***************************************************************************/
Vector3 NearestPointOnLineSeg(
Vector3CRef p,
Vector3CRef l1,
Vector3CRef l2)
{
	Vector3 out;
	NearestPointOnLineSeg(p, l1, l2, out);
	return(out);
}

/***************************************************************************/
/***************************************************************************/
Vector2 NearestPointOnLine(
const Vector2& p,
const Vector2& l1,
const Vector2& l2)
{
	Vector2 out;
	NearestPointOnLine(p, l1, l2, out);
	return(out);
}

/***************************************************************************/
/***************************************************************************/
Vector3 NearestPointOnLine(
Vector3CRef p,
Vector3CRef l1,
Vector3CRef l2)
{
	Vector3 out;
	NearestPointOnLine(p, l1, l2, out);
	return(out);
}


/***************************************************************************/
// Utiltiy for making an orthoganal vector
/***************************************************************************/
Vector3 MakeOrthogonal(
Vector3CRef v)
{
	Vector3 abs = v.Abs();

	int smallest = 2;	
	if (abs.X() < abs.Y())
	{
		if (abs.X() < abs.Z())	
			smallest = 0;
	}
	else // y < x 
	{
		if (abs.Y() < abs.Z())
			smallest = 1;
	}

	int largest = 2;	
	if (abs.X() > abs.Y())
	{
		if (abs.X() > abs.Z())	
			largest = 0;
	}
	else // y > x 
	{
		if (abs.Y() > abs.Z())
			largest = 1;
	}

	Vector3 o;
	if (largest == smallest)
		o = v;
	else if (smallest == 0)
	{ 
		if (largest == 1)
			o.Set(v.Y(), v.X(), v.Z());
		else 
			o.Set(v.Z(), v.Y(), v.X());
	}
	else if (smallest == 1)
	{
		if (largest == 0)
			o.Set(v.Y(), v.X(), v.Z());
		else 
			o.Set(v.X(), v.Z(), v.Y());
	}
	else if (smallest == 2)
	{
		if (largest == 0)
			o.Set(v.Z(), v.Y(), v.X());
		else 
			o.Set(v.X(), v.Z(), v.Y());
	} 

	return o;
}


