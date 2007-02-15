//===================================================================
//  File:  PointToLine.h
//  Description:
//     A fast point to line routine.
//===================================================================

#ifndef __POINTTOLINE_H__
#define __POINTTOLINE_H__

#include "platform/BaseType.h"
#include "Math/MathClass.h"
#include "Math/Vector.h"

const float ptl_infinity = 1.0e30f;
inline float sqr(float x) {return x*x;}

inline void PointToLineSeg2D(float x, float y, float x1, float y1, float x2, float y2,
						     float& xout, float& yout)
{
	float Ax = x2-x1;
	float Ay = y2-y1;
	float dot = Ax*(x-x1) + Ay*(y-y1);

	// If the dot product is 0 or less than one, then the vectors are pointing
	// away from each other and we can use (x1, y1);
	if (dot <= .0001f)
	{
		xout = x1;
		yout = y1;
		return;
	}

	float tmp = dot / (Ax*Ax + Ay*Ay);
	xout = x1 + tmp*Ax;
	yout = y1 + tmp*Ay;

	// Do a bounding rectangle check to see if the point is actually on the line.
	if (xout < Math::Min(x1, x2) || xout > Math::Max(x1, x2) ||
		yout < Math::Min(y1, y2) || yout > Math::Max(y1, y2))
	{
		// We know the point isn't on the line, and we know that the vectors
		// aren't opposite direction.
		xout = x2;
		yout = y2;
	}
}

inline float Slope(float x1, float y1, float x2, float y2)
{
	if (Math::Abs(x2-x1) < .0001f)
		return ptl_infinity;

	return (y2-y1)/(x2-x1);
}

inline void UnboundedLineIntersect2D(float x11, float y11, float x12, float y12,  // First line
									 float x21, float y21, float x22, float y22,  // Second line
									 float& xout, float& yout) // Intersection point
{
	float m1 = Slope(x11, y11, x12, y12);
	float m2 = Slope(x21, y21, x22, y22);

	// Are the lines parallel?
	if (Math::Abs(m1-m2) < .001f)
	{
		xout = ptl_infinity;
		yout = ptl_infinity;
		return;
	}

	// Is the first line vertical?
	if (m1 == ptl_infinity)
	{
		xout = x11;
		yout = m2*(xout-x21) + y21;
		return;
	}

	// Is the second line vertical?
	if (m2 == ptl_infinity)
	{
		xout = x21;
		yout = m1*(xout-x11) + y11;
		return;
	}

	xout = (y21 - y11 + m1*x11 - m2*x21) / (m1 - m2);
	yout = m1 * (xout-x11) + y11;

	ASSERT(m1 * (xout-x11) + y11 == m2 * (xout-x21) + y21);
	ASSERT((yout-y11) / m1 + x11 == (yout-y21) / m2 + x21);
}

inline bool Equal(float a, float b)
{
	const float threshhold = 0.001f;
	if (Math::Abs(a - b) < threshhold)
		return true;
	return false;
}

inline bool LessThan(float a, float b)
{
	if (a < b && !Equal(a, b))
		return true;
	return false;
}

inline bool GreaterThan(float a, float b)
{
	if (a > b && !Equal(a, b))
		return true;
	return false;
}


//----------------------------------------------------------------------
// ::LineSegIntersect2D
//    This is a modification of a fast line segment intersection
//    routine by Franklin Antonio (Graphics Gems III pp. 199)
//----------------------------------------------------------------------
inline bool LineSegIntersect2D(float x1, float y1, float x2, float y2,
										 float x3, float y3, float x4, float y4)
//										 float& x, float& y)
{
	float Ax,Bx,Cx,Ay,By,Cy,d,e,f; //num,offset;
	float x1lo,x1hi,y1lo,y1hi;

	Ax = x2-x1;
	Bx = x3-x4;

	//---------------------------------------------
	//  x direction bounding box test.
	//---------------------------------------------
	if (LessThan(Ax,0.0f))
	{
		x1lo = x2;
		x1hi = x1;
	}
	else
	{
		x1hi = x2;
		x1lo = x1;
	}
	if (GreaterThan(Bx,0.0f))
	{
		if (LessThan(x1hi,x4) || LessThan(x3,x1lo))
			return false;
	}
	else
	{
		if (LessThan(x1hi, x3) || LessThan(x4, x1lo))
			return false;
	}

	Ay = y2-y1;
	By = y3-y4;

	//---------------------------------------------
	//  y direction bounding box test.
	//---------------------------------------------
	if (LessThan(Ay, 0.0f))
	{
		y1lo = y2;
		y1hi = y1;
	}
	else
	{
		y1hi = y2;
		y1lo = y1;
	}
	if (GreaterThan(By, 0.0f))
	{
		if (LessThan(y1hi, y4) || LessThan(y3, y1lo))
			return false;
	}
	else
	{
		if (LessThan(y1hi, y3) || LessThan(y4, y1lo))
			return false;
	}

	Cx = x1-x3;
	Cy = y1-y3;
	d = By*Cx - Bx*Cy;					/* alpha numerator*/
	f = Ay*Bx - Ax*By;					/* both denominator*/
	if (GreaterThan(f, 0.0f))						/* alpha tests*/
	{
		if (LessThan(d, 0.0f) || GreaterThan(d, f))
			return false;
	}
	else
	{
		if (GreaterThan(d, 0.0f) || LessThan(d, f))
			return false;
	}

	e = Ax*Cy - Ay*Cx;					/* beta numerator*/
	if (GreaterThan(f, 0.0f))						/* beta tests*/
	{
		if (LessThan(e, 0.0f) || GreaterThan(e, f))
			return false;
	}
	else
	{
		if (GreaterThan(e, 0.0f) || LessThan(e, f))
			return false;
	}

	if (Equal(f, 0.0f)) // Co-linear case.
		return false;

//  	num = d*Ax;						/* numerator */
//  	offset = SAME_SIGNS(num,f) ? f/2 : -f/2;		/* round direction*/
//  	x = x1 + (num+offset) / f;				/* intersection x */

//  	num = d*Ay;
//  	offset = SAME_SIGNS(num,f) ? f/2 : -f/2;
//  	y = y1 + (num+offset) / f;				/* intersection y */

	return true;
}


inline float TurnAngle(const Vector3& oldDir, const Vector3& newDir)
{
	float velangle = Math::ArcTan2(oldDir.z, oldDir.x);
	float idealangle = Math::ArcTan2(newDir.z, newDir.x);

	float anglediff = idealangle - velangle;

	if (anglediff < 0)
		anglediff += Math::TwoPi;

	if (anglediff > Math::Pi)
		anglediff -= Math::TwoPi;

	// The result will probably be between pi/2 and -pi/2.  If it's more than that,
	// we're in trouble.
	return anglediff;
}

inline float FastDist2D(float x1, float y1, float x2, float y2)
{
	float dx, dy;
	dx = Math::Abs(x2-x1);
	dy = Math::Abs(y2-y1);

	if (dx < dy)
		return dy + dx*0.335f;
	return dx + dy*0.335f;
}

#endif // __POINTTOLINE_H__
