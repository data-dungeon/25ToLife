/***************************************************************************/
// A class to help compute different quantities about points lying inside of
// triangles
/***************************************************************************/
#ifndef TRITEST_H
#define TRITEST_H

#include "platform/BaseType.h"
#include "Math/Vector.h"

// Define the "thickness" of the triangle edges
#define TRIANGLE_EDGETHICKNESS	(0.001f)

class TriangleTest
{
public:
	// These do nothing
   TriangleTest() {}

	// Return the normal for the triangle
	// If you look perpendicular to the triangle from above and the verticies
	// are in clockwise order, your normal will be negative (down).  If they
	// are give counterclockwise the normal will be positive (up).
	inline static Vector3 Normal(Vector3CRef v0, Vector3CRef v1, Vector3CRef v2);

   // Given the triangle v1, v2, v3 and the co-planar point p, is p
   // inside the triangle?  If p is on an edge, it counts.
	// If you don't specify a normal, one will have to be computed
   inline static bool PointInside(Vector3CRef p, Vector3CRef v0, Vector3CRef v1, Vector3CRef v2);
   inline static bool PointInside(Vector3CRef p, Vector3CRef v0, Vector3CRef v1, Vector3CRef v2, Vector3CRef normal); // fastest

   // Is the point in the triangle projected into he specified plane?
   inline static bool PointInsideXY(Vector3CRef p, Vector3CRef v0, Vector3CRef v1, Vector3CRef v2);
   inline static bool PointInsideYZ(Vector3CRef p, Vector3CRef v0, Vector3CRef v1, Vector3CRef v2);
   inline static bool PointInsideXZ(Vector3CRef p, Vector3CRef v0, Vector3CRef v1, Vector3CRef v2);

   // Is the point in the triangle projected into he specified plane?
   inline static bool PointInsideXY(float x, float y, Vector3CRef v0, Vector3CRef v1, Vector3CRef v2);
   inline static bool PointInsideYZ(float y, float z, Vector3CRef v0, Vector3CRef v1, Vector3CRef v2);
   inline static bool PointInsideXZ(float x, float z, Vector3CRef v0, Vector3CRef v1, Vector3CRef v2);

   // Discover the triangle's centroid in the XZ plane
   inline static Vector3 Centroid( Vector3CRef v0, Vector3CRef v1, Vector3CRef v2 );

   // Test if two lines intersect - not where, but if
   inline static bool LinesIntersectXZ( Vector3CRef vP1, Vector3CRef vP2, Vector3CRef vQ1, Vector3CRef vQ2 );

   // Test if a ray intersects the given triangle, returns wether there was an intersection, the distance to the intersection (fT), and the Baricentric coordinates of the intersection (fU, fV).
   inline static bool RayIntersection( Vector3CRef vOrigin, Vector3CRef vNormalDirection, Vector3CRef v0, Vector3CRef v1, Vector3CRef v2, float &fT, float &fU, float &fV );

protected:
	// What side of the line are we on?
	inline static float SideOfLine(float px, float py, float v1x, float v1y, float v2x, float v2y);

   // Return SIGN_POS it t > 0.0, SIGN_NEG if t < 0.0 and SIGN_ZERO if t == 0.0
	typedef enum
	{
		SIGN_NEG = 0,
		SIGN_ZERO = 1,
		SIGN_POS = 2
	} SIGN;
   inline static SIGN Sign(float t);

   // Internals for fast testing
	static uint32 signTest;
	inline static bool SignTest(SIGN a, SIGN b, SIGN c);
};

// implementation
#include "collide/tritest.hpp"

#endif
