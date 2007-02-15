/***************************************************************************/
// A class to help compute different quantities about chords intersecting
// each other
/***************************************************************************/
#ifndef CHORD_H
#define CHORD_H

#include "platform/BaseType.h"
#include "Math/Vector.h"

class ChordIntersect
{
public:
	// Do these two chords intersect?
	inline static bool IntersectXY(Vector3 &a, Vector3 &b, Vector3 &i, Vector3 &j);
	inline static bool IntersectXZ(Vector3 &a, Vector3 &b, Vector3 &i, Vector3 &j);
	inline static bool IntersectYZ(Vector3 &a, Vector3 &b, Vector3 &i, Vector3 &j);

	// Solve for the intersection of the chords formed by these sets of points
	// (v1-v2) (v3-v4) and return in i.
	// You must be sure that the chords intersect.
	inline static void IntersectionXZ(Vector3 &v1, Vector3 &v2, Vector3 &v3, Vector3 &v4, Vector3 &i);

private:
   // Internals for fast testing
	static bool testCrossGrid[3][3];

   // Return 1 it t > 0.0, -1 if t < 0.0 and 0 if t == 0.0
   inline static int Sign(float t);

	// Determine which side of the chord p is on in the X Y plane
	inline static float SideOfLineXY(Vector3 &p, Vector3 &v1, Vector3 &v2);

	// Determine which side of the chord p is on in the X Z plane
	inline static float SideOfLineXZ(Vector3 &p, Vector3 &v1, Vector3 &v2);

	// Determine which side of the chord p is on in the Y Z plane
	inline static float SideOfLineYZ(Vector3 &p, Vector3 &v1, Vector3 &v2);
};

// Include the implementation
#include "chord.hpp"

#endif
