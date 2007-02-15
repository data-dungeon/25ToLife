//****************************************************************************
// A generalized "zone" with helper implementations
//***************************************************************************

// Pre-compiled header //
#include "CollideHelper/CollideHelperPCH.h"

// My header (always first) //
#include "CollideHelper/zone.h"

// Gamelib includes //
#include "EngineHelper/drawutil.h"

//***************************************************************************
//***************************************************************************
ZoneSphere::ZoneSphere(
int id,
Vector3CRef position,
float size) :
Zone(id)
{
	center = position;
	radius = size;
}

//***************************************************************************
// Does this zone intersect this Rect?
//***************************************************************************
XZRect ZoneSphere::Extents()
{
	float corner = radius * 1.4142f;
	return XZRect(center.x() - corner,
						center.z() - corner,
						center.x() + corner,
						center.z() + corner);
}

//***************************************************************************
// Does this zone intersect this rect?
//***************************************************************************
bool ZoneSphere::Intersects(
const XZRect &rect)
{
	return (rect.Inside(center.x(), center.z()) ||
				EdgeIntersects(rect.MinX(), rect.MinZ(), rect.MaxX(), rect.MinZ()) ||
				EdgeIntersects(rect.MaxX(), rect.MinZ(), rect.MaxX(), rect.MaxZ()) ||
				EdgeIntersects(rect.MaxX(), rect.MaxZ(), rect.MinX(), rect.MaxZ()) ||
				EdgeIntersects(rect.MinX(), rect.MaxZ(), rect.MinX(), rect.MinZ()));
}

//***************************************************************************
// Does this zone contain this point?
//***************************************************************************
bool ZoneSphere::Contains(
Vector3CRef p)
{
	Vector3 d = p - center;

	return (d.LengthSquared() < Math::Square(radius));
}

//***************************************************************************
// Debugging draw call
//***************************************************************************
void ZoneSphere::Draw()
{
	if (Active())
		DrawUtility::Sphere(center, DrawUtility::RED, radius);
	else
		DrawUtility::Sphere(center, DrawUtility::BLUE, radius);
}

//***************************************************************************
//***************************************************************************
inline bool ZoneSphere::EdgeIntersects(
float x0,
float z0,
float x1,
float z1)
{
	// Find the vector from the start to the end of the edge.
	float dx = x1 - x0;
	float dz = z1 - z0;

	// Find the vector from the center of the circle to start of the edge.
	float sx = x0 - center.x();
	float sz = z0 - center.z();

	// Find the length of the edge squared.
	float dSquared = dx * dx + dz * dz;

	// Find the dot product of the start vector and the edge and its square.
	float sDotD = sx * dx + sz * dz;
	float sDotDSquared = sDotD * sDotD;

	// The following term appears under the square root of the quadratic formula
	// when you solve for the fractional distance along the edge where it
	// intersects the circle.
	float b = sDotDSquared - dSquared * (sx * sx + sz * sz - radius * radius);

	// If b is negative, the solution is complex indicating that the edge cannot
	// intersect the circle.
	if (b < 0.0f)
		return false;

	// If sDotD is positive, the edge starts after the point of closest
	// approach. If the largest solution of the quadratic formula is less
	// than 0, the edge starts outside the circle and heads away.
	if (sDotD >= 0.0f)
		return sDotDSquared <= b;

	// If sDotD is negative, the edge starts before the point if closest
	// approach. If the smallest solution of the quadratic formula is greater
	// than 1, the edge ends before it reaches the circle.
	float c = dSquared + sDotD;
	return (c >= 0.0f) || ((c * c) <= b);
}

//***************************************************************************
//***************************************************************************
inline bool ZoneSphere::Inside(
float x, 
float z)
{
	float dx = x - center.x();
	float dz = z - center.z();
	return dx * dx + dz * dz < radius * radius;
}

//***************************************************************************
//***************************************************************************
ZoneOBB::ZoneOBB(
int id,
OBB &init) : 
Zone(id)
{
	obb = init;
}

//***************************************************************************
// Does this zone intersect this Rect?
//***************************************************************************
XZRect ZoneOBB::Extents()
{
	XZRect extents;
	obb.ComputeWorldExtents(extents);
	return extents;
}

//***************************************************************************
// Does this zone intersect this rect?
//***************************************************************************
bool ZoneOBB::Intersects(
const XZRect &rect)
{
	// Make the a really tall AABB object for the rect
	CAabbCollisionObject aabbObject;
	aabbObject.GetAABB().minV.Set(rect.MinX(), -10000.0f, rect.MinZ());
	aabbObject.GetAABB().maxV.Set(rect.MaxX(), 10000.0f, rect.MaxZ());

	// Now see if the OBB and AABB intersect
	CAabbToObbIntersection intersection(aabbObject, obb);
	return intersection.SeparatingAxisTest();
}

//***************************************************************************
// Does this zone contain this point?
//***************************************************************************
bool ZoneOBB::Contains(
Vector3CRef p)
{
	return obb.PointInBox(p);
}

//***************************************************************************
// Debugging draw call
//***************************************************************************
void ZoneOBB::Draw()
{
	if (Active())
		obb.Draw(DrawUtility::RED);
	else
		obb.Draw(DrawUtility::BLUE);
}

