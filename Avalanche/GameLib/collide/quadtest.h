/***************************************************************************/
// A class to help compute different quantities about points lying inside of
// quads
/***************************************************************************/
#ifndef QUADTEST_H
#define QUADTEST_H

#include "collide/tritest.h"

class QuadTest : public TriangleTest
{
public:
	// These do nothing
   QuadTest() {}

	// Return the normal for the quad
	inline static Vector3 Normal(Vector3CRef v0, Vector3CRef v1, Vector3CRef v2, Vector3CRef v3);

   // Given the quad v1, v2, v3 and the co-planar point p, is p
   // inside the quad?  If p is on an edge, it counts.
	// If you don't specify a normal, one will have to be computed
   inline static bool PointInside(Vector3CRef p, Vector3CRef v0, Vector3CRef v1, Vector3CRef v2, Vector3CRef v3);
   inline static bool PointInside(Vector3CRef p, Vector3CRef v0, Vector3CRef v1, Vector3CRef v2, Vector3CRef v3, Vector3CRef normal); // fastest

   // Is the point in the triangle projected into he specified plane?
   inline static bool PointInsideXY(Vector3CRef p, Vector3CRef v0, Vector3CRef v1, Vector3CRef v2, Vector3CRef v3);
   inline static bool PointInsideYZ(Vector3CRef p, Vector3CRef v0, Vector3CRef v1, Vector3CRef v2, Vector3CRef v3);
   inline static bool PointInsideXZ(Vector3CRef p, Vector3CRef v0, Vector3CRef v1, Vector3CRef v2, Vector3CRef v3);

private:
	// Test the signs
	inline static bool SignTest(SIGN s[4]);
};

// implementation
#include "collide/quadtest.hpp"

#endif
