/***************************************************************************/
// Simple classes for ray's and segments
/***************************************************************************/
#include "Math/MathClass.h"

/***************************************************************************/
// Setup
/***************************************************************************/
inline void Segment::Set(
Vector3CRef a,
Vector3CRef b)
{
	start = a;
	end = b;
}

/***************************************************************************/
/***************************************************************************/
inline Segment::Segment(
Vector3CRef a,
Vector3CRef b)
{
	Set(a, b);
}

/***************************************************************************/
/***************************************************************************/
inline Segment::Segment(
const Ray &r,
float scale)
{
	Set(r.o, r.o + r.dir * scale);
}

/***************************************************************************/
/***************************************************************************/
inline float Segment::LengthXZ(void) const
{
	return((end - start).LengthXZ());
}


/***************************************************************************/
/***************************************************************************/
inline float Segment::LengthSquared(void) const
{
	return((end - start).LengthSquared());
}

/***************************************************************************/
/***************************************************************************/
inline float Segment::LengthXZSquared(void) const
{
	return((end - start).LengthXZSquared());
}

/***************************************************************************/
// Handy
/***************************************************************************/
inline float Segment::Length(void) const
{
	return(Math::Sqrt(LengthSquared()));
}

/***************************************************************************/
// Operators
/***************************************************************************/
inline int Segment::operator==(const Segment &s) const
{
	return(start == s.start && end == s.end);
}

/***************************************************************************/
/***************************************************************************/
inline int Segment::operator!=(const Segment &s) const
{
	return(start != s.start || end != s.end);
}

/***************************************************************************/
/***************************************************************************/
inline Segment &Segment::operator=(
const Segment &s)
{
	start = s.start;
	end = s.end;
	return(*this);
}

/***************************************************************************/
// Closest point to defined line
/***************************************************************************/
inline Vector3 Segment::ClosestPoint(
Vector3CRef p) const
{
	// Make segment into a ray
	Vector3 v = end - start;

	// Find the perpendicular point
	Vector3 perp = start + v * (Vector3::Dot(p - start, v) / Vector3::Dot(v, v));
	return(perp);
}

/***************************************************************************/
// Closest point to defined line
/***************************************************************************/
inline Vector3 Segment::TrueClosestPoint(
Vector3CRef p) const
{
	// Compute the direction vector
	Vector3 w = end - start;

	// Normalize, but we also need length, so do it ourselves
	float fLength = w.Length();
	if (fLength == 0.0f)
		return(start);
	float fOneOverLength = 1.0f / fLength;
	Vector3 v = w * fOneOverLength;

	// Find the length of the projection of (p - start) onto v
	float t = Vector3::Dot(p - start, v);

	// test against segment
	Vector3 perp;
	if (t < 0.0f)
		perp = start;
	else if (t > fLength)
		perp = end;
	else
		perp = start + v * t;
	return(perp);
}

/***************************************************************************/
// Closest point to defined line
/***************************************************************************/
inline Vector3 Segment::TrueClosestPoint(
	Vector3CRef p,
	float fLength,
	float &t) const
{
	// Compute the direction vector
	Vector3 w = end - start;

	// Normalize, but we also need length, so do it ourselves
	if (fLength == 0.0f)
		return(start);
	float fOneOverLength = 1.0f / fLength;
	Vector3 v = w * fOneOverLength;

	// Find the length of the projection of (p - start) onto v
	t = Vector3::Dot(p - start, v);

	// test against segment
	Vector3 perp;
	if (t < 0.0f)
		perp = start;
	else if (t > fLength)
		perp = end;
	else
		perp = start + v * t;

	// normalize t
	t /= fLength;

	// return the point
	return(perp);
}

/***************************************************************************/
// Give distance to the LINE defined by the segment
/***************************************************************************/
inline float Segment::DistanceSquaredToPoint(
Vector3CRef p) const
{
	return((p - ClosestPoint(p)).LengthSquared());
}

/***************************************************************************/
// Give distance to the LINE defined by the segment
/***************************************************************************/
inline float Segment::DistanceToPoint(
Vector3CRef p) const
{
	return((p - ClosestPoint(p)).Length());
}

/***************************************************************************/
// Give distance to the actual segment
/***************************************************************************/
inline float Segment::TrueDistanceSquaredToPoint(
Vector3CRef p) const
{
	return((p - TrueClosestPoint(p)).LengthSquared());
}

/***************************************************************************/
// Give distance to the actual segment
/***************************************************************************/
inline float Segment::TrueDistanceToPoint(
Vector3CRef p) const
{
	return((p - TrueClosestPoint(p)).Length());
}

/***************************************************************************/
// Find intersection point with a plane (does line, not segment)
// returns false if no intersection, otherwise true with i being the
// intersection point
/***************************************************************************/
inline bool Segment::IntersectWithPlane(
Vector3CRef planeP,
Vector3CRef planeN,
Vector3 &i,
float &t) const
{
	// Make segment into a ray
	Vector3 w = end - start;

	// Is the line paralell to the plane?
	float vDotN = Vector3::Dot(w, planeN);
	if (Math::Zero(vDotN))
		return(false);

	// What is the intersection?
	t = Vector3::Dot((planeP - start), planeN) / vDotN;

	// Turn back into a point
	i = start + w * t;
	return(true);
}

/***************************************************************************/
// Find intersection point with a plane (does line, not segment)
// returns false if no intersection, otherwise true with i being the
// intersection point
/***************************************************************************/
inline bool Segment::IntersectWithPlane(
Vector3CRef planeP,
Vector3CRef planeN,
Vector3 &i) const
{
	float t;
	return(IntersectWithPlane(planeP, planeN, i, t));
}

/***************************************************************************/
// Find intersection point with a plane (does true ray)
// returns false if no intersection, otherwise true with i being the
// intersection point
// If you modify this function, be aware that the ps2 has an optimized
// assembly version in rayseg.s
/***************************************************************************/
#if 1 //!defined(PS2) || !( defined(PS2) && defined(SN) )
inline bool Segment::TrueIntersectWithPlane(
Vector3CRef planeP,
Vector3CRef planeN,
Vector3 &i,
float &t) const
{
	// Figure out parametric coefficients
	Vector3 w = end - start;

	// Is the line paralell to the plane?
	float vDotN = Vector3::Dot(w, planeN);
	if (Math::Zero(vDotN))
		return(false);

	// What is the intersection?
	t = Vector3::Dot((planeP - start), planeN) / vDotN;

	// On the segment?
	if (t < 0.0f || t > 1.0f)
		return(false);

	// Turn back into a point
	i = start + w * t;

	return(true);
}
#endif //!defined(PS2) || !( defined(PS2) && defined(SN) )

/***************************************************************************/
// Find intersection point with a plane (does true ray)
// returns false if no intersection, otherwise true with i being the
// intersection point
/***************************************************************************/
inline bool Segment::TrueIntersectWithPlane(
Vector3CRef planeP,
Vector3CRef planeN,
Vector3 &i) const
{
	float t;
	return(TrueIntersectWithPlane(planeP, planeN, i, t));
}

/***************************************************************************/
// Find intersection point with a plane (does true ray)
// returns false if no intersection, otherwise true with i being the
// intersection point
/***************************************************************************/
inline bool Segment::TrueIntersectWithPlaneFrontSide(
Vector3CRef planeP,
Vector3CRef planeN,
Vector3 &i,
float &t) const
{
	// Figure out parametric coefficients
	Vector3 w = end - start;

	// Does the segment point away from the plane?
	float vDotN = Vector3::Dot(w, planeN);
	if (vDotN > -Math::Epsilon)
		return(false);

	// What is the intersection?
	t = Vector3::Dot((planeP - start), planeN) / vDotN;

	// On the segment?
	if (t < 0.0f || t > 1.0f)
		return(false);

	// Turn back into a point
	i = start + w * t;
	return(true);
}

/***************************************************************************/
// Find intersection point with a plane (does true ray)
// returns false if no intersection, otherwise true with i being the
// intersection point
/***************************************************************************/
inline bool Segment::TrueIntersectWithPlaneBackSide(
Vector3CRef planeP,
Vector3CRef planeN,
Vector3 &i,
float &t) const
{
	// Figure out parametric coefficients
	Vector3 w = end - start;

	// Does the segment point into the plane?
	float vDotN = Vector3::Dot(w, planeN);
	if (vDotN < Math::Epsilon)
		return(false);

	// What is the intersection?
	t = Vector3::Dot((planeP - start), planeN) / vDotN;

	// On the segment?
	if (t < 0.0f || t > 1.0f)
		return(false);

	// Turn back into a point
	i = start + w * t;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
inline void Ray::Set(
Vector3CRef origin,
Vector3CRef direction)
{
	o = origin;
	dir = direction;
}

/***************************************************************************/
/***************************************************************************/
inline Ray::Ray(
Vector3CRef origin,
Vector3CRef direction)
{
	Set(origin, direction);
}

/***************************************************************************/
/***************************************************************************/
inline Ray::Ray(
const Segment &s)
{
	Set(s.start, (s.end - s.start).Normalized());
}

/***************************************************************************/
// Normalize the direction
/***************************************************************************/
inline void Ray::Normalize(void)
{
	dir.Normalize();
}

/***************************************************************************/
// Operators
/***************************************************************************/
inline int Ray::operator==(const Ray &r) const
{
	return(o == r.o && dir == r.dir);
}

/***************************************************************************/
/***************************************************************************/
inline int Ray::operator!=(const Ray &r) const
{
	return(o != r.o || dir != r.dir);
}

/***************************************************************************/
/***************************************************************************/
inline Ray &Ray::operator=(const Ray &r)
{
	o = r.o;
	dir = r.dir;
	return(*this);
}

/***************************************************************************/
// Get a point from scaling a ray
/***************************************************************************/
inline Vector3 operator*(
const Ray &r,
float scale)
{
	Vector3 temp = r.o + r.dir * scale;
	return(temp);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3 operator*(
float scale,
const Ray &r)
{
	return(r * scale);
}

/***************************************************************************/
// Obtain a point on the segment using t = [0 - 1]
/***************************************************************************/
inline Vector3 operator*(
const Segment &s,
float t)
{
	return(s.start + (s.end - s.start) * t);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3 operator*(
float t,
const Segment &s)
{
	return(s * t);
}

/***************************************************************************/
// Transform a ray or segment
// (does not re-normalize ray, you must do it yourself)
/***************************************************************************/
inline Ray operator*(
const Ray &r,
const DirCos3x3 &m)
{
	Ray temp;

	temp.Set(r.o * m, r.dir * m);
	return(temp);
}

/***************************************************************************/
/***************************************************************************/
inline Ray operator*(
Ray &r,
Graphics4x4 &m)
{
	Ray temp;

	temp.Set(r.o * m, r.dir.BasisTransform(m));
	return(temp);
}

/***************************************************************************/
/***************************************************************************/
inline Segment operator*(
const Segment &s,
const DirCos3x3 &m)
{
	Segment temp;

	temp.Set(s.start * m, s.end * m);
	return(temp);

}

/***************************************************************************/
/***************************************************************************/
inline Segment operator*(
const Segment &s,
const Graphics4x4 &m)
{
	Segment temp;

	temp.Set(s.start * m, s.end * m);
	return(temp);
}

/***************************************************************************/
// Closest point to defined line
/***************************************************************************/
inline Vector3 Ray::ClosestPoint(
Vector3CRef p) const
{
	// Find the perpendicular point
	Vector3 perp = o + dir * Vector3::Dot(p - o, dir);
	return(perp);
}

/***************************************************************************/
// Closest point to actual ray
/***************************************************************************/
inline Vector3 Ray::TrueClosestPoint(
Vector3CRef p) const
{
	// Find the perpendicular point
	float t = Vector3::Dot(p - o, dir);
	Vector3 perp;
	if (t < 0.0f)
		perp = o;
	else
		perp = o + dir * t;
	return(perp);
}

/***************************************************************************/
// Give distance to the LINE defined by the ray
/***************************************************************************/
inline float Ray::DistanceSquaredToPoint(
Vector3CRef p) const
{
	return((p - ClosestPoint(p)).LengthSquared());
}

/***************************************************************************/
// Give distance to the LINE defined by the ray
/***************************************************************************/
inline float Ray::DistanceToPoint(
Vector3CRef p) const
{
	return((p - ClosestPoint(p)).Length());
}

/***************************************************************************/
// Give distance to the actual ray
/***************************************************************************/
inline float Ray::TrueDistanceSquaredToPoint(
Vector3CRef p) const
{
	return((p - TrueClosestPoint(p)).LengthSquared());
}

/***************************************************************************/
// Give distance to the actual ray
/***************************************************************************/
inline float Ray::TrueDistanceToPoint(
Vector3CRef p) const
{
	return((p - TrueClosestPoint(p)).Length());
}

/***************************************************************************/
// Find intersection point with a plane (does line, not segment)
// returns false if no intersection, otherwise true with i being the
// intersection point
/***************************************************************************/
inline bool Ray::IntersectWithPlane(
Vector3CRef planeP,
Vector3CRef planeN,
Vector3 &i,
float &t) const
{
	// Is the line paralell to the plane?
	float dirDotN = Vector3::Dot(dir, planeN);
	if (Math::Zero(dirDotN))
		return(false);

	// What is the intersection?
	t = Vector3::Dot((planeP - o), planeN) / dirDotN;

	// Turn back into a point
	i = o + dir * t;
	return(true);
}

/***************************************************************************/
// Find intersection point with a plane (does true ray)
// returns false if no intersection, otherwise true with i being the
// intersection point
/***************************************************************************/
inline bool Ray::IntersectWithPlane(
Vector3CRef planeP,
Vector3CRef planeN,
Vector3 &i) const
{
	float t;
	return(IntersectWithPlane(planeP, planeN, i, t));
}

/***************************************************************************/
// Find intersection point with a plane (does true ray)
// returns false if no intersection, otherwise true with i being the
// intersection point
// If you modify this function, be aware that the ps2 has an optimized
// assembly version in rayseg.s
/***************************************************************************/
#if 1 //!defined(PS2) || !( defined(PS2) && defined(SN) )
inline bool Ray::TrueIntersectWithPlane(
Vector3CRef planeP,
Vector3CRef planeN,
Vector3 &i,
float &t) const
{
	// Is the line paralell to the plane?
	float dirDotN = Vector3::Dot(dir, planeN);
	if (Math::Zero(dirDotN))
		return(false);

	// What is the intersection?
	t = Vector3::Dot((planeP - o), planeN) / dirDotN;

	// On the ray?
	if (t < 0.0f)
		return(false);

	// Turn back into a point
	i = o + dir * t;
	return(true);
}

/***************************************************************************/
// Find intersection point with a plane (does true ray)
// returns false if no intersection, otherwise true with i being the
// intersection point
/***************************************************************************/
inline bool Ray::TrueIntersectWithPlane(
Vector3CRef planeP,
Vector3CRef planeN,
Vector3 &i) const
{
	float t;
	return(TrueIntersectWithPlane(planeP, planeN, i, t));
}

inline bool Ray::TrueIntersectWithPlaneFrontSide(
Vector3CRef planeP,
Vector3CRef planeN,
Vector3 &i,
float &t) const
{
	// Does the ray point away from the plane?
	float dirDotN = Vector3::Dot(dir, planeN);
	if (dirDotN > -Math::Epsilon)
		return(false);

	// What is the intersection?
	t = Vector3::Dot((planeP - o), planeN) / dirDotN;

	// On the ray?
	if (t < 0.0f)
		return(false);

	// Turn back into a point
	i = o + dir * t;
	return(true);
}

inline bool Ray::TrueIntersectWithPlaneBackSide(
Vector3CRef planeP,
Vector3CRef planeN,
Vector3 &i,
float &t) const
{
	// Does the ray point into the plane?
	float dirDotN = Vector3::Dot(dir, planeN);
	if (dirDotN < Math::Epsilon)
		return(false);

	// What is the intersection?
	t = Vector3::Dot((planeP - o), planeN) / dirDotN;

	// On the ray?
	if (t < 0.0f)
		return(false);

	// Turn back into a point
	i = o + dir * t;
	return(true);
}

#endif //!defined(PS2) || !( defined(PS2) && defined(SN) )
