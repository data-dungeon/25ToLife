/***************************************************************************/
// Simple classes for ray's and segments
/***************************************************************************/
#ifndef RAYSEG_H
#define RAYSEG_H

#include "Math/Matrix.h"

// Forward declare
class Ray;

// Two points
class Segment
{
public:
	inline Segment(void) {}
	inline Segment(Vector3CRef a, Vector3CRef b);
	inline Segment(const Ray &r, float scale = 1.0f);
	inline ~Segment(void) {}

	// Setup
	inline void Set(Vector3CRef a, Vector3CRef b);

	// Handy
	inline float Length(void) const;
	inline float LengthXZ(void) const;
	inline float LengthSquared(void) const;
	inline float LengthXZSquared(void) const;

	// Operators
	inline int operator==(const Segment &s) const;
	inline int operator!=(const Segment &s) const;
	inline Segment &operator=(const Segment &s);

	// Give distance to the LINE defined by the segment
	inline float DistanceSquaredToPoint(Vector3CRef p) const;
	inline float DistanceToPoint(Vector3CRef p) const;

	// Give distance to the actual segment
	inline float TrueDistanceSquaredToPoint(Vector3CRef p) const;
	inline float TrueDistanceToPoint(Vector3CRef p) const;

	// Closest point to defined line
	inline Vector3 ClosestPoint(Vector3CRef p) const;

	// Closest point to the segment
	inline Vector3 TrueClosestPoint(Vector3CRef p) const;
	inline Vector3 TrueClosestPoint(Vector3CRef p, float fLength, float &t) const;

	// Find intersection point with a plane (does line, not segment)
	// returns false if no intersection, otherwise true with i being the intersection point
	// t is the normalized 0 - 1 of intersection (not normalized unless using True call)
	inline bool IntersectWithPlane(Vector3CRef planeP, Vector3CRef planeN, Vector3 &i) const;
	inline bool IntersectWithPlane(Vector3CRef planeP, Vector3CRef planeN, Vector3 &i, float &t) const;

	// Find intersection point with a plane (does true segment)
	// returns false if no intersection, otherwise true with i being the intersection point
	// t is the normalized 0 - 1 of intersection (not normalized unless using True call)
	inline bool TrueIntersectWithPlane(Vector3CRef planeP, Vector3CRef planeN, Vector3 &i) const;
	inline bool TrueIntersectWithPlane(Vector3CRef planeP, Vector3CRef planeN, Vector3 &i, float &t) const;
	inline bool TrueIntersectWithPlaneFrontSide(Vector3CRef planeP, Vector3CRef planeN, Vector3 &i, float &t) const;
	inline bool TrueIntersectWithPlaneBackSide(Vector3CRef planeP, Vector3CRef planeN, Vector3 &i, float &t) const;

	// These are public now!
	Vector3	start, end;
};

// Point with normalized direction
class Ray
{
public:
	inline Ray(void) {}
	inline Ray(Vector3CRef origin, Vector3CRef direction);
	inline Ray(const Segment &s);

	// Setup
	inline void Set(Vector3CRef origin, Vector3CRef direction);

	// Normalize the direction
	inline void Normalize(void);

	// Operators
	inline int operator==(const Ray &r) const;
	inline int operator!=(const Ray &r) const;
	inline Ray &operator=(const Ray &r);

	// Give distance to the LINE defined by the ray
	inline float DistanceSquaredToPoint(Vector3CRef p) const;
	inline float DistanceToPoint(Vector3CRef p) const;

	// Give distance to the actual ray
	inline float TrueDistanceSquaredToPoint(Vector3CRef p) const;
	inline float TrueDistanceToPoint(Vector3CRef p) const;

	// Closest point to defined line
	inline Vector3 ClosestPoint(Vector3CRef p) const;

	// Closest point to defined ray
	inline Vector3 TrueClosestPoint(Vector3CRef p) const;

	// Find intersection point with a plane (does line, not segment)
	// returns false if no intersection, otherwise true with i being the
	// intersection point
	// t is the normalized 0 - 1 of intersection (not normalized unless using True call)
	inline bool IntersectWithPlane(Vector3CRef planeP, Vector3CRef planeN, Vector3 &i) const;
	inline bool IntersectWithPlane(Vector3CRef planeP, Vector3CRef planeN, Vector3 &i, float &t) const;

	// Find intersection point with a plane (does true ray)
	// returns false if no intersection, otherwise true with i being the
	// intersection point
	// t is the normalized 0 - 1 of intersection (not normalized unless using True call)
	inline bool TrueIntersectWithPlane(Vector3CRef planeP, Vector3CRef planeN, Vector3 &i) const;
	inline bool TrueIntersectWithPlane(Vector3CRef planeP, Vector3CRef planeN, Vector3 &i, float &t) const;
	inline bool TrueIntersectWithPlaneFrontSide(Vector3CRef planeP, Vector3CRef planeN, Vector3 &i, float &t) const;
	inline bool TrueIntersectWithPlaneBackSide(Vector3CRef planeP, Vector3CRef planeN, Vector3 &i, float &t) const;

	// These are public now!
	Vector3	o, dir;
};

// ---------------------
// Some handy operators
// ---------------------

// Get a point from scaling a ray
inline Vector3 operator*(const Ray &r, float scale);
inline Vector3 operator*(float scale, const Ray &r);

// Obtain a point on the segment using t = [0 - 1]
inline Vector3 operator*(const Segment &s, float t);
inline Vector3 operator*(float t, const Segment &s);

// Transform a ray or segment (does not re-normalize ray, you must do it yourself)
inline Ray operator*(const Ray &r, const DirCos3x3 &m);
inline Ray operator*(const Ray &r, const Graphics4x4 &m);

inline Segment operator*(const Segment &s, const DirCos3x3 &m);
inline Segment operator*(const Segment &s, const Graphics4x4 &m);

// Include the implementations
#include "linearAlgebra/rayseg.hpp"

#endif
