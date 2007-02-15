/***************************************************************************/
// Implementation file for the triangle stuff
/***************************************************************************/
#include "Math/MathClass.h"

/***************************************************************************/
// Determine which side of the line p is on in the X Y plane
/***************************************************************************/
inline float TriangleTest::SideOfLine(
float px,
float py,
float v1x,
float v1y,
float v2x,
float v2y)
{
	float ax = v2x - v1x;
	float ay = v2y - v1y;
	float bx = px - v2x;
	float by = py - v2y;

   return(ax * by - ay * bx);
}

/***************************************************************************/
// Return the normal for the triangle
// If you look perpendicular to the triangle from above and the verticies
// are in clockwise order, your normal will be negative (down).  If they
// are give counterclockwise the normal will be positive (up).
/***************************************************************************/
inline Vector3 TriangleTest::Normal(
Vector3CRef v0,
Vector3CRef v1,
Vector3CRef v2)
{
	// Shorthand
	Vector3 normal = Vector3::Cross(v2 - v1, v0 - v1);
	normal.SafeNormalize();
	
	return normal;
}

/***************************************************************************/
// Return true if the point p is in the triangle defined by v0, v1, v2
// Right now I am assuming p is co-planar with the triangle.
// This test works by projecting the triangle into a plane defined by
// two coordinate axes
/***************************************************************************/
inline bool TriangleTest::PointInsideXY(
Vector3CRef p,
Vector3CRef v0,
Vector3CRef v1,
Vector3CRef v2)
{
	// Do the tests
   SIGN s1 = Sign(SideOfLine(p.X(), p.Y(), v0.X(), v0.Y(), v1.X(), v1.Y()));
   SIGN s2 = Sign(SideOfLine(p.X(), p.Y(), v1.X(), v1.Y(), v2.X(), v2.Y()));
   SIGN s3 = Sign(SideOfLine(p.X(), p.Y(), v2.X(), v2.Y(), v0.X(), v0.Y()));

   // If they are all the same sign or 0's, it's okay
   return(SignTest(s1, s2, s3));
}

/***************************************************************************/
// Return true if the point p is in the triangle defined by v0, v1, v2
// Right now I am assuming p is co-planar with the triangle.
// This test works by projecting the triangle into a plane defined by
// two coordinate axes
/***************************************************************************/
inline bool TriangleTest::PointInsideYZ(
Vector3CRef p,
Vector3CRef v0,
Vector3CRef v1,
Vector3CRef v2)
{
	// Do the tests
   SIGN s1 = Sign(SideOfLine(p.Y(), p.Z(), v0.Y(), v0.Z(), v1.Y(), v1.Z()));
   SIGN s2 = Sign(SideOfLine(p.Y(), p.Z(), v1.Y(), v1.Z(), v2.Y(), v2.Z()));
   SIGN s3 = Sign(SideOfLine(p.Y(), p.Z(), v2.Y(), v2.Z(), v0.Y(), v0.Z()));

   // If they are all the same sign or 0's, it's okay
   return(SignTest(s1, s2, s3));
}

/***************************************************************************/
// Return true if the point p is in the triangle defined by v0, v1, v2
// Right now I am assuming p is co-planar with the triangle.
// This test works by projecting the triangle into a plane defined by
// two coordinate axes
/***************************************************************************/
inline bool TriangleTest::PointInsideXZ(
Vector3CRef p,
Vector3CRef v0,
Vector3CRef v1,
Vector3CRef v2)
{
	// Do the tests
   SIGN s1 = Sign(SideOfLine(p.X(), p.Z(), v0.X(), v0.Z(), v1.X(), v1.Z()));
   SIGN s2 = Sign(SideOfLine(p.X(), p.Z(), v1.X(), v1.Z(), v2.X(), v2.Z()));
   SIGN s3 = Sign(SideOfLine(p.X(), p.Z(), v2.X(), v2.Z(), v0.X(), v0.Z()));

   // If they are all the same sign or 0's, it's okay
   return(SignTest(s1, s2, s3));
}

/***************************************************************************/
// Is the point in the triangle projected into he specified plane?
/***************************************************************************/
inline bool TriangleTest::PointInsideXY(
float x,
float y,
Vector3CRef v0,
Vector3CRef v1,
Vector3CRef v2)
{
	// Do the tests
   SIGN s1 = Sign(SideOfLine(x, y, v0.X(), v0.Y(), v1.X(), v1.Y()));
   SIGN s2 = Sign(SideOfLine(x, y, v1.X(), v1.Y(), v2.X(), v2.Y()));
   SIGN s3 = Sign(SideOfLine(x, y, v2.X(), v2.Y(), v0.X(), v0.Y()));

   // If they are all the same sign or 0's, it's okay
   return(SignTest(s1, s2, s3));
}

/***************************************************************************/
// Is the point in the triangle projected into he specified plane?
/***************************************************************************/
inline bool TriangleTest::PointInsideYZ(
float y,
float z,
Vector3CRef v0,
Vector3CRef v1,
Vector3CRef v2)
{
	// Do the tests
   SIGN s1 = Sign(SideOfLine(y, z, v0.Y(), v0.Z(), v1.Y(), v1.Z()));
   SIGN s2 = Sign(SideOfLine(y, z, v1.Y(), v1.Z(), v2.Y(), v2.Z()));
   SIGN s3 = Sign(SideOfLine(y, z, v2.Y(), v2.Z(), v0.Y(), v0.Z()));

   // If they are all the same sign or 0's, it's okay
   return(SignTest(s1, s2, s3));
}

/***************************************************************************/
// Is the point in the triangle projected into he specified plane?
/***************************************************************************/
inline bool TriangleTest::PointInsideXZ(
float x,
float z,
Vector3CRef v0,
Vector3CRef v1,
Vector3CRef v2)
{
	// Do the tests
   SIGN s1 = Sign(SideOfLine(x, z, v0.X(), v0.Z(), v1.X(), v1.Z()));
   SIGN s2 = Sign(SideOfLine(x, z, v1.X(), v1.Z(), v2.X(), v2.Z()));
   SIGN s3 = Sign(SideOfLine(x, z, v2.X(), v2.Z(), v0.X(), v0.Z()));

   // If they are all the same sign or 0's, it's okay
   return(SignTest(s1, s2, s3));
}

/***************************************************************************/
// Given the triangle v0, v1, v2 and the co-planar point p, is p
// inside the triangle?  If p is on an edge, it counts.
// If you don't specify a normal, one will have to be computed
/***************************************************************************/
inline bool TriangleTest::PointInside(
Vector3CRef p,
Vector3CRef v0,
Vector3CRef v1,
Vector3CRef v2,
Vector3CRef normal)
{
	Vector3 absN = Vector3::Abs(normal);

   // Which plane is best to project into?
   if (absN.X() > absN.Y() && absN.X() > absN.Z())
      return(PointInsideYZ(p, v0, v1, v2));
   else if (absN.Y() > absN.X() && absN.Y() > absN.Z())
      return(PointInsideXZ(p, v0, v1, v2));
   else
      return(PointInsideXY(p, v0, v1, v2));
}

/***************************************************************************/
// Given the triangle v0, v1, v2 and the co-planar point p, is p
// inside the triangle?  If p is on an edge, it counts
/***************************************************************************/
inline bool TriangleTest::PointInside(
Vector3CRef p,
Vector3CRef v0,
Vector3CRef v1,
Vector3CRef v2)
{
	Vector3 normal = Normal(v0, v1, v2);
	return(PointInside(p, v0, v1, v2, normal));
}

/***************************************************************************/
// Return 1 it t > 0.0, -1 if t < 0.0 and 0 if t == 0.0
/***************************************************************************/
inline TriangleTest::SIGN TriangleTest::Sign(
float t)
{
   if (t < -TRIANGLE_EDGETHICKNESS)
      return(SIGN_NEG);
   else if (t > TRIANGLE_EDGETHICKNESS)
      return(SIGN_POS);
   else
      return(SIGN_ZERO);
}

/***************************************************************************/
// Sign test
/***************************************************************************/
inline bool TriangleTest::SignTest(
SIGN a,
SIGN b,
SIGN c)
{
	uint32 testBit = 1 << (a + b * 3 + c * 9);
	return((signTest & testBit) != 0);
}

/************************************************************************/
/* Discover the triangle's centroid in 3D                               */
/************************************************************************/ 
inline Vector3 TriangleTest::Centroid( Vector3CRef v0, Vector3CRef v1, Vector3CRef v2 )
{
	return( ( ( ( v1 - v0 ) + ( v2 - v0 ) ) * 0.333334f ) + v0 );
}

/************************************************************************/
/* Determine if two lines intersect - not where they do, just if        */
/************************************************************************/
inline bool TriangleTest::LinesIntersectXZ( Vector3CRef vP1, Vector3CRef vP2, Vector3CRef vQ1, Vector3CRef vQ2 )
{
	// taken from the discussion of line intersections in Real-Time Rendering 2e by Tomas Akenine-Moller and Eric Haines, pg.616

	float fD, fE, fF;
	Vector3 a, b, c, aBar, bBar;
	a = ( vQ2 - vQ1 ); a.y( 0.0f );
	b = ( vP2 - vP1 ); b.y( 0.0f );
	c = ( vP1 - vQ1 ); c.y( 0.0f );
	aBar.Set( -a.z(), a.y(), a.x() );	// perpendicular dot product in 2D
	bBar.Set( -b.z(), b.y(), b.x() );	// perpendicular dot product in 2D
	fD = c.Dot( aBar );
	fE = c.Dot( bBar );
	fF = a.Dot( bBar );

	// testing for t
	if ( fF > 0.0f )
	{
		if ( ( fD < 0.0f ) || ( fD > fF ) )
		{
			return( false );
		}
	}
	else
	{
		if ( ( fD > 0.0f ) || ( fD < fF ) )
		{
			return( false );
		}
	}

	// testing for s
	if ( fF > 0.0f )
	{
		if ( ( fE < 0.0f ) || ( fE > fF ) )
		{
			return( false );
		}
	}
	else
	{
		if ( ( fE > 0.0f ) || ( fE < fF ) )
		{
			return( false );
		}
	}

	// they must intersect if we get here
	return( true );
}

#define TRIANGLETEST_RAYINTERSECT_EPSILON	( 0.0001f )

inline bool TriangleTest::RayIntersection( Vector3CRef vOrigin, Vector3CRef vNormalDirection, Vector3CRef v0, Vector3CRef v1, Vector3CRef v2, float &fT, float &fU, float &fV )
{
	Vector3 edge1, edge2, tvec, pvec, qvec;
	float det, inv_det;

	edge1 = v1 - v0;
	edge2 = v2 - v0;
	pvec = vNormalDirection.Cross( edge2 );
	det = edge1.Dot( pvec );

#if defined( TRIANGLETEST_RAYINTERSECT_USECULLTEST )

	if ( det < TRIANGLETEST_RAYINTERSECT_EPSILON )
	{
		return( false );
	}

	tvec = vOrigin - v0;

	fU = tvec.Dot( pvec );
	if ( ( fU < 0.0f ) || ( fU > det ) )
	{
		return( false );
	}

	qvec = tvec.Cross( edge1 );
	fV = vNormalDirection.Dot( qvec );
	if ( ( fV < 0.0f ) || ( ( fU + fV ) > det ) )
	{
		return( false );
	}

	fT = edge2.Dot( qvec );

	inv_det = 1.0f / det;
	fT *= inv_det;
	fU *= inv_det;
	fV *= inv_det;

#else

	if ( ( det > -TRIANGLETEST_RAYINTERSECT_EPSILON ) && ( det < TRIANGLETEST_RAYINTERSECT_EPSILON ) )
	{
		return( false );
	}

	inv_det = 1.0f / det;

	tvec = vOrigin - v0;
	fU = tvec.Dot( pvec ) * inv_det;
	if ( ( fU < 0.0f ) || ( fU > 1.0f ) )
	{
		return( false );
	}

	qvec = tvec.Cross( edge1 );
	fV = vNormalDirection.Dot( qvec ) * inv_det;
	if ( ( fV < 0.0f ) || ( ( fU + fV ) > 1.0f ) )
	{
		return( false );
	}

	fT = edge2.Dot( qvec ) * inv_det;

#endif

	return( true );
}
