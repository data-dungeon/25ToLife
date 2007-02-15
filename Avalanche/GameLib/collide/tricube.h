/***************************************************************************/
// Code adapted from Graphics Gems III to test a triangle for intersection
// against a unit cube.
/***************************************************************************/
#ifndef TRICUBE_H
#define TRICUBE_H

#include "Math/Vector.h"
#include "Math/MathClass.h"

class TriangleCubeTest
{
public:
	// Everything is static here
	TriangleCubeTest() {}

	// Does it intersect?
	static bool Intersects(const Vector3 &cubeCenter, float cubeHalfSize,
									const Vector3 &v1, const Vector3 &v2, const Vector3 &v3,
									float expandCube = 0.0f);

private:
	// Inside/outside
	enum
	{
		INSIDE = 0,
		OUTSIDE = 1
	};

	// Internal triangle
	struct Triangle
	{
		Vector3		v1, v2, v3;
	};

	// Internal point
	struct Point
	{
		float x, y, z;
	};

	// Define an epsilon for all comparisons
	const static float epsilon;

	// Give the sign 3
	inline static long Sign3(const Vector3 &v);

	// Lerp
	inline static float LERP(float a, float b, float c);

	// Which of the six face-plane(s) is point P outside of?
	inline static long FacePlane(const Vector3 &p);

	// Which of the twelve edge plane(s) is point P outside of?
	inline static long Bevel2d(const Vector3 &p);

	// Which of the eight corner plane(s) is point P outside of?
	inline static long Bevel3d(const Vector3 &p);

	// Test the point "alpha" of the way from P1 to P2
	// See if it is on a face of the cube
	// Consider only faces in "mask"
	inline static long CheckPoint(const Vector3 &p1, const Vector3 &p2, float alpha, long mask);

	// Compute intersection of P1 --> P2 line segment with face planes
	// Then test intersection point to see if it is on cube face
	// Consider only face planes in "outcode_diff"
	// Note: Zero bits in "outcodeDiff" means face line is outside of
	inline static long CheckLine(const Vector3 &p1, const Vector3 &p2, long outcodeDiff);

	// Test if 3D point is inside 3D triangle
	static long PointTriangleIntersection(const Point &p, const Triangle &t);

	// The main test
	static long Intersection(const Triangle &t);
};

#endif
