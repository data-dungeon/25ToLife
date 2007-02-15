/*************************************************************************/
// Nice cube class
/*************************************************************************/
#include "collide/CollidePCH.h"
#include "collide/cube.h"
#include "collide/tritest.h"
#include "collide/quadtest.h"

// Array of signs for the octant
const Vector3 Octant::s_sign[OCTANTS] =
{
	Vector3( 1.0f,  1.0f,  1.0f),	// XYZ
	Vector3(-1.0f,  1.0f,  1.0f),	// _XYZ,
	Vector3( 1.0f, -1.0f,  1.0f),	// X_YZ,
	Vector3(-1.0f, -1.0f,  1.0f),	// _X_YZ,
	Vector3( 1.0f,  1.0f, -1.0f),	// XY_Z,
	Vector3(-1.0f,  1.0f, -1.0f),	// _XY_Z,
	Vector3( 1.0f, -1.0f, -1.0f),	// X_Y_Z,
	Vector3(-1.0f, -1.0f, -1.0f)	// _X_Y_Z
};

/*************************************************************************/
// Does this triangle touch this cube?
/*************************************************************************/
bool Cube::Touches(
const Vector3 &normal,
const Vector3 &a,
const Vector3 &b,
const Vector3 &c,
float epsilon) const
{
#if 1
	return !FindSeparatingAxis(normal, a, b, c, epsilon);
#else
	// If any point is inside, we are inside
	if (Inside(a, epsilon) || Inside(b, epsilon) || Inside(c, epsilon))
		return true;

	// Array of signs for the corners
	const static Vector3 corner[8] =
	{
		Vector3( 1.0f,  1.0f,  1.0f),	// XYZ
		Vector3(-1.0f,  1.0f,  1.0f),	// -XYZ,
		Vector3( 1.0f, -1.0f,  1.0f),	// X-YZ,
		Vector3(-1.0f, -1.0f,  1.0f),	// -X-YZ,
		Vector3( 1.0f,  1.0f, -1.0f),	// XY-Z,
		Vector3(-1.0f,  1.0f, -1.0f),	// -XY-Z,
		Vector3( 1.0f, -1.0f, -1.0f),	// X-Y-Z,
		Vector3(-1.0f, -1.0f, -1.0f)	// -X-Y-Z
	};

	// Build up the corners
	Vector3 v[8];
	const float halfSize = m_halfSize + epsilon;
	for (int i = 0; i < 8; i++)
		v[i] = m_center + (v[i] * halfSize);

	// Build up the edges
	const static int edge[12][2] =
	{
		{ 0, 1 }, { 1, 3 }, { 3, 2 }, { 2, 0 },
		{ 4, 5 }, { 5, 7 }, { 7, 6 }, { 6, 4 },
		{ 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }
	};

	// Test all the edges against the triangle
	for (int e = 0; e < 12; e++)
	{
		Segment s(v[edge[e][0]], v[edge[e][1]]);

		if (TriangleTest(s, a, b, c, normal))
			return true;
	}

	// The list of faces
	const static int quad[6][4] =
	{
		{ 0, 1, 3, 2 },
		{ 0, 1, 5, 4 },
		{ 4, 5, 7, 6 },
		{ 2, 3, 7, 6 },
		{ 1, 3, 7, 5 },
		{ 0, 2, 6, 4 }
	};

	// Segment array of triangle legs
	Segment triLeg[3];
	triLeg[0].Set(a, b);
	triLeg[1].Set(b, c);
	triLeg[2].Set(c, a);

	// Test all of the triangle edges against the box faces
	for (int face = 0; face < 6; face++)
	{
		for (int leg = 0; leg < 3; leg++)
		{
			if (QuadTest(triLeg[leg], v[quad[face][0]], v[quad[face][1]], v[quad[face][2]], v[quad[face][3]]))
		 		return true;
		}
	}

	// No intersection here
	return false;
#endif
}

/*************************************************************************/
/*************************************************************************/
bool Cube::TriangleTest(
const Segment &s,
const Vector3 &a,
const Vector3 &b,
const Vector3 &c,
const Vector3 &normal)
{
	// Intersect the segment with the triangle's plane
	Vector3 intersection;
	if (!s.TrueIntersectWithPlane(a, normal, intersection))
		return false;

	// Is it inside?
	return TriangleTest::PointInside(intersection, a, b, c, normal);
}

/*************************************************************************/
/*************************************************************************/
bool Cube::QuadTest(
const Segment &s,
const Vector3 &a,
const Vector3 &b,
const Vector3 &c,
const Vector3 &d)
{
	// Compute the normal
	Vector3 normal = QuadTest::Normal(a, b, c, d);

	// Intersect it with the plane
	Vector3 intersection;
	if (!s.TrueIntersectWithPlane(a, normal, intersection))
		return false;

	return QuadTest::PointInside(intersection, a, b, c, d, normal);
}

/*************************************************************************/
// Clip this segment to the cube, returns false if not in the cube
/*************************************************************************/
bool Cube::Clip(
const Vector3 &start,
const Vector3 &end,
Vector3 &clipStart,
Vector3 &clipEnd) const
{
	// Assume no clipping
	clipStart = start;
	clipEnd = end;

	bool startInside = Inside(start);
	bool endInside = Inside(end);

	// We are safe!
	if (startInside && endInside)
		return true;

	// Is the start inside?
	Vector3 intersection;
	if (!startInside)
	{
		if (!LineEnter(start, end, intersection))
			return false;
		else
			clipStart = intersection;
	}

	// Clip the end
	if (!endInside)
	{
		if (!LineExit(start, end, intersection))
			return false;
		else
			clipEnd = intersection;
	}

	// We are good
	return true;
}

/*************************************************************************/
// Compute the point at which the segment enters the cube
/*************************************************************************/
bool Cube::LineEnter(
Vector3CRef start,
Vector3CRef end,
Vector3 &intersection,
float epsilon) const
{
	// What is the direction of the ray
	Vector3 dir = end - start;

	// Slightly deflate the cube for floating point error
	float halfSize = m_halfSize + epsilon;

	// What are the candidate planes?
	float xPlane = (dir.x() > 0.0f) ? (m_center.x() - halfSize) : (m_center.x() + halfSize);
	float yPlane = (dir.y() > 0.0f) ? (m_center.y() - halfSize) : (m_center.y() + halfSize);
	float zPlane = (dir.z() > 0.0f) ? (m_center.z() - halfSize) : (m_center.z() + halfSize);

	// Intersect each plane
	float tx = Math::Zero(dir.x()) ? -Math::PositiveInfinity : (xPlane - start.x()) / dir.x();
	float ty = Math::Zero(dir.y()) ? -Math::PositiveInfinity : (yPlane - start.y()) / dir.y();
	float tz = Math::Zero(dir.z()) ? -Math::PositiveInfinity : (zPlane - start.z()) / dir.z();

	// What is the maximum?
	if (tx > ty)
	{
		if (tx > tz)
		{
			if (tx > 1.0f)
				return false;
			intersection = start + dir * tx;
			return InsideFaceYZ(intersection);
		}
	}
	else if (ty > tz)
	{
		if (ty > 1.0f)
			return false;
		intersection = start + dir * ty;
		return InsideFaceXZ(intersection);
	}

	if (tz > 1.0f)
		return false;
	intersection = start + dir * tz;
	return InsideFaceXY(intersection);
}

/*************************************************************************/
// Compute the point at which the segment exits cube
/*************************************************************************/
bool Cube::LineExit(
Vector3CRef start,
Vector3CRef end,
Vector3 &intersection,
float epsilon) const
{
	// What is the direction of the ray
	Vector3 dir = end - start;

	// Slightly deflate the cube for floating point error
	float halfSize = m_halfSize + epsilon;

	// What are the candidate planes?
	float xPlane = (dir.x() > 0.0f) ? (m_center.x() + halfSize) : (m_center.x() - halfSize);
	float yPlane = (dir.y() > 0.0f) ? (m_center.y() + halfSize) : (m_center.y() - halfSize);
	float zPlane = (dir.z() > 0.0f) ? (m_center.z() + halfSize) : (m_center.z() - halfSize);

	// Intersect each plane
	float tx = Math::Zero(dir.x()) ? Math::PositiveInfinity : (xPlane - start.x()) / dir.x();
	float ty = Math::Zero(dir.y()) ? Math::PositiveInfinity : (yPlane - start.y()) / dir.y();
	float tz = Math::Zero(dir.z()) ? Math::PositiveInfinity : (zPlane - start.z()) / dir.z();

	// What is the minimum?
	if (tx < ty)
	{
		if (tx < tz)
		{
			if (tx < 0.0f)
				return false;
			intersection = start + dir * tx;
			return InsideFaceYZ(intersection);
		}
	}
	else if (ty < tz)
	{
		if (ty < 0.0f)
			return false;
	 	intersection = start + dir * ty;
		return InsideFaceXZ(intersection);
	}

	if (tz < 0.0f)
		return false;
	intersection = start + dir * tz;
	return InsideFaceXY(intersection);
}

/*************************************************************************/
// The point at which the ray exits the cube
/*************************************************************************/
float Cube::RayExit(
Vector3CRef start,
Vector3CRef end,
float exitEpsilon) const
{
	// Make the difference
	Vector3 d = end - start;

	// Adjust the halfsize
	float halfSize = m_halfSize + exitEpsilon;

	// What are the candidate planes?
	float xPlane = (d.x() > 0.0f) ? (m_center.x() + halfSize) : (m_center.x() - halfSize);
	float yPlane = (d.y() > 0.0f) ? (m_center.y() + halfSize) : (m_center.y() - halfSize);
	float zPlane = (d.z() > 0.0f) ? (m_center.z() + halfSize) : (m_center.z() - halfSize);

	// Intersect each
	float tx = Math::Zero(d.x()) ? Math::PositiveInfinity : (xPlane - start.x()) / d.x();
	float ty = Math::Zero(d.y()) ? Math::PositiveInfinity : (yPlane - start.y()) / d.y();
	float tz = Math::Zero(d.z()) ? Math::PositiveInfinity : (zPlane - start.z()) / d.z();

	// What is the minimum?
	float t = Math::Min(tx, ty, tz);
	ASSERT(t >= 0);
	return t;
}

/*************************************************************************/
// The point at which the ray exits the cube
/*************************************************************************/
Cube::Face Cube::RayFaceExit(
Vector3CRef start,
Vector3CRef end) const
{
	// Make the difference
	Vector3 d = end - start;

	// Comparisons
	const bool xPositive = (d.x() > 0.0f);
	const bool yPositive = (d.y() > 0.0f);
	const bool zPositive = (d.z() > 0.0f);

	// What are the candidate planes?
	float xPlane = xPositive ? (m_center.x() + m_halfSize) : (m_center.x() - m_halfSize);
	float yPlane = yPositive ? (m_center.y() + m_halfSize) : (m_center.y() - m_halfSize);
	float zPlane = zPositive ? (m_center.z() + m_halfSize) : (m_center.z() - m_halfSize);

	// Intersect each
	float tx = Math::Zero(d.x()) ? Math::PositiveInfinity : (xPlane - start.x()) / d.x();
	float ty = Math::Zero(d.y()) ? Math::PositiveInfinity : (yPlane - start.y()) / d.y();
	float tz = Math::Zero(d.z()) ? Math::PositiveInfinity : (zPlane - start.z()) / d.z();

//	ASSERT(tx >= 0.0f);
//	ASSERT(ty >= 0.0f);
//	ASSERT(tz >= 0.0f);

	// What is the minimum?
	if (tx < ty)
	{
		if (tx < tz)
			return xPositive ? POSX : NEGX;
		else
			return zPositive ? POSZ : NEGZ;
	}
	else // tx > ty
	{
		if (ty < tz)
			return yPositive ? POSY : NEGY;
		else
			return zPositive ? POSZ : NEGZ;
	}
}

/*************************************************************************/
// Separating axis test (returns true if it finds a seperating axis)
/*************************************************************************/
bool Cube::FindSeparatingAxis(
const Vector3 &normal,
const Vector3 &a,
const Vector3 &b,
const Vector3 &c,
float epsilon) const
{
	// Catch parallel edges
	const float crossBuffer = 1.0e-4f;

	// Make these aliases
	const float halfSize = m_halfSize + epsilon;
	const Vector3 minV = m_center - halfSize;
	const Vector3 maxV = m_center + halfSize;

	// Check faces.
	if (((a.x() > maxV.x()) && (b.x() > maxV.x()) &&
			(c.x() > maxV.x())) || ((a.x() < minV.x()) &&
			(b.x() < minV.x()) && (c.x() < minV.x())))
		return true;

	if (((a.y() > maxV.y()) && (b.y() > maxV.y()) &&
			(c.y() > maxV.y())) || ((a.y() < minV.y()) &&
			(b.y() < minV.y()) && (c.y() < minV.y())))
		return true;

	if (((a.z() > maxV.z()) && (b.z() > maxV.z()) &&
			(c.z() > maxV.z())) || ((a.z() < minV.z()) &&
			(b.z() < minV.z()) && (c.z() < minV.z())))
		return true;

 	//	Transform into a system whose origin is at the center of the aabb.
	Vector3 extent = 0.5f * (maxV - minV);
	Vector3 v0 = a - m_center;
	Vector3 v1 = b - m_center;
	Vector3 v2 = c - m_center;

	// Check triangle face.
	if (Math::Abs(normal.Dot(v0)) > extent.Dot(Vector3::Abs(normal)))
		return true;

 	// Now check the cross products of the edges.
	Vector3 crossTerm0 = v1.Cross(v0);
	Vector3 crossTerm1 = v2.Cross(v1);
	Vector3 crossTerm2 = v0.Cross(v2);

	// Edge 0.
	Vector3 edge = Vector3::Abs(v1 - v0);
	Vector3 crossSum = crossTerm1 + crossTerm2;

	//	X-axis cross edge 0.
	float aabbDist = extent.Y() * edge.Z() + extent.Z() * edge.Y() + crossBuffer;
	if (((crossTerm0.X() > aabbDist) && (crossSum.X() < -aabbDist)) ||
			((crossTerm0.X() < -aabbDist) && (crossSum.X() > aabbDist)))
		return true;

	// Y-axis cross edge 0.
	aabbDist = extent.X() * edge.Z() + extent.Z() * edge.X() + crossBuffer;
	if (((crossTerm0.Y() > aabbDist) && (crossSum.Y() < -aabbDist)) ||
			((crossTerm0.Y() < -aabbDist) && (crossSum.Y() > aabbDist)))
		return true;

	// Z-axis cross edge 0.
	aabbDist = extent.X() * edge.Y() + extent.Y() * edge.X() + crossBuffer;
	if (((crossTerm0.Z() > aabbDist) && (crossSum.Z() < -aabbDist)) ||
			((crossTerm0.Z() < -aabbDist) && (crossSum.Z() > aabbDist)))
		return true;

	// Edge 1.
	edge = Vector3::Abs(v2 - v1);
	crossSum = crossTerm0 + crossTerm2;

	// X-axis cross edge 1.
	aabbDist = extent.Y() * edge.Z() + extent.Z() * edge.Y() + crossBuffer;
	if (((crossTerm1.X() > aabbDist) && (crossSum.X() < -aabbDist)) ||
			((crossTerm1.X() < -aabbDist) && (crossSum.X() > aabbDist)))
		return true;

	// Y-axis cross edge 1.
	aabbDist = extent.X() * edge.Z() + extent.Z() * edge.X() + crossBuffer;
	if (((crossTerm1.Y() > aabbDist) && (crossSum.Y() < -aabbDist)) ||
			((crossTerm1.Y() < -aabbDist) && (crossSum.Y() > aabbDist)))
		return true;

	// Z-axis cross edge 1.
	aabbDist = extent.X() * edge.Y() + extent.Y() * edge.X() + crossBuffer;
	if (((crossTerm1.Z() > aabbDist) && (crossSum.Z() < -aabbDist)) ||
			((crossTerm1.Z() < -aabbDist) && (crossSum.Z() > aabbDist)))
		return true;

	// Edge 2.
	edge = Vector3::Abs(v0 - v2);
	crossSum = crossTerm0 + crossTerm1;

	// X-axis cross edge 2.
	aabbDist = extent.Y() * edge.Z() + extent.Z() * edge.Y() + crossBuffer;
	if (((crossTerm2.X() > aabbDist) && (crossSum.X() < -aabbDist)) ||
			((crossTerm2.X() < -aabbDist) && (crossSum.X() > aabbDist)))
		return true;

	// Y-axis cross edge 2.
	aabbDist = extent.X() * edge.Z() + extent.Z() * edge.X() + crossBuffer;
	if (((crossTerm2.Y() > aabbDist) && (crossSum.Y() < -aabbDist)) ||
			((crossTerm2.Y() < -aabbDist) && (crossSum.Y() > aabbDist)))
		return true;

	// Z-axis cross edge 2.
	aabbDist = extent.X() * edge.Y() + extent.Y() * edge.X() + crossBuffer;
	if (((crossTerm2.Z() > aabbDist) && (crossSum.Z() < -aabbDist)) ||
			((crossTerm2.Z() < -aabbDist) && (crossSum.Z() > aabbDist)))
		return true;

	// They must intersect
	return false;
}

/*************************************************************************/
// Is this OBB touching this cube in any way
/*************************************************************************/
bool Cube::Touches(
const OBB &obb) const
{
	// compute difference of box centers, D = C1-C0
	const Vector3 &obbCenter = *obb.Center();
	Vector3 d = obbCenter - m_center;

	Matrix3x3 absC;		// |c_{ij}|
	Vector3CRef obbExtents = obb.GetConstAABB().maxV;
	const DirCos3x3 &c = *obb.Matrix();

	// AABB x-axis
	absC.e00 = Math::Abs(c.e00);
	absC.e10 = Math::Abs(c.e10);
	absC.e20 = Math::Abs(c.e20);
	if ((m_halfSize + obbExtents.x() * absC.e00 + obbExtents.y() * absC.e10 +
			obbExtents.z() * absC.e20) < Math::Abs(d.x()))
		return false;

	// AABB y-axis
	absC.e01 = Math::Abs(c.e01);
	absC.e11 = Math::Abs(c.e11);
	absC.e21 = Math::Abs(c.e21);
	if ((m_halfSize + obbExtents.x() * absC.e01 + obbExtents.y() * absC.e11 +
			obbExtents.z() * absC.e21) < Math::Abs(d.y()))
		return false;

	// AABB z-axis
	absC.e02 = Math::Abs(c.e02);
	absC.e12 = Math::Abs(c.e12);
	absC.e22 = Math::Abs(c.e22);
	if ((m_halfSize + obbExtents.x() * absC.e02 + obbExtents.y() * absC.e12 +
			obbExtents.z() * absC.e22) < Math::Abs(d.z()))
		return false;

	// OBB x-axis
	if ((m_halfSize * absC.e00 + m_halfSize * absC.e01 +
			m_halfSize * absC.e02 + obbExtents.x()) <
			Math::Abs(d.x() * c.e00 + d.y() * c.e01 + d.z() * c.e02))
		return false;

	// OBB y-axis
	if ((m_halfSize * absC.e10 + m_halfSize * absC.e11 +
			m_halfSize * absC.e12 + obbExtents.y()) <
			Math::Abs(d.x() * c.e10 + d.y() * c.e11 + d.z() * c.e12))
		return false;

	// OBB z-axis
	if ((m_halfSize * absC.e20 + m_halfSize * absC.e21 +
			m_halfSize * absC.e22 + obbExtents.z()) <
			Math::Abs(d.x() * c.e20 + d.y() * c.e21 + d.z() * c.e22))
		return false;

	// AABB x-axis cross OBB x-axis
	if ((m_halfSize * absC.e02 + m_halfSize * absC.e01 +
			obbExtents.y() * absC.e20 + obbExtents.z() * absC.e10) <
			Math::Abs(d.z() * c.e01 - d.y() * c.e02))
		return false;

	// AABB x-axis cross OBB y-axis
	if ((m_halfSize * absC.e12 + m_halfSize * absC.e11 +
			obbExtents.x() * absC.e20 + obbExtents.z() * absC.e00) <
			Math::Abs(d.z() * c.e11 - d.y() * c.e12))
		return false;

	// AABB x-axis cross OBB z-axis
	if ((m_halfSize * absC.e22 + m_halfSize * absC.e21 +
			obbExtents.x() * absC.e10 + obbExtents.y() * absC.e00) <
			Math::Abs(d.z() * c.e21 - d.y() * c.e22))
		return false;

	// AABB y-axis cross OBB x-axis
	if ((m_halfSize * absC.e02 + m_halfSize * absC.e00 +
			obbExtents.y() * absC.e21 + obbExtents.z() * absC.e11) <
			Math::Abs(d.x() * c.e02 - d.z() * c.e00))
		return false;

	// AABB y-axis cross OBB y-axis
	if ((m_halfSize * absC.e12 + m_halfSize * absC.e10 +
			obbExtents.x() * absC.e21 + obbExtents.z() * absC.e01) <
			Math::Abs(d.x() * c.e12 - d.z() * c.e10))
		return false;

	// AABB y-axis cross OBB z-axis
	if ((m_halfSize * absC.e22 + m_halfSize * absC.e20 +
			obbExtents.x() * absC.e11 + obbExtents.y() * absC.e01) <
			Math::Abs(d.x() * c.e22 - d.z() * c.e20))
		return false;

	// AABB z-axis cross OBB x-axis
	if ((m_halfSize * absC.e01 + m_halfSize * absC.e00 +
			obbExtents.y() * absC.e22 + obbExtents.z() * absC.e12) <
			Math::Abs(d.y() * c.e00 - d.x() * c.e01))
		return false;

	// AABB z-axis cross OBB y-axis
	if ((m_halfSize * absC.e11 + m_halfSize * absC.e10 +
			obbExtents.x() * absC.e22 + obbExtents.z() * absC.e02) <
			Math::Abs(d.y() * c.e10 - d.x() * c.e11))
		return false;

	// AABB z-axis cross OBB z-axis
	if ((m_halfSize * absC.e21 + m_halfSize * absC.e20 +
			obbExtents.x() * absC.e12 + obbExtents.y() * absC.e02) <
			Math::Abs(d.y() * c.e20 - d.x() * c.e21))
		return false;

	return true;
}

/*************************************************************************/
// Make the root octant from an AABB
/*************************************************************************/
Octant Octant::StandardRoot(
const AABB &aabb)
{
	Octant root(aabb);
	return root;
}

/*************************************************************************/
// Make the root octant from an AABB
/*************************************************************************/
Octant Octant::LooseRoot(
const AABB &aabb)
{
	Octant root(aabb);
	root.Inflate(root.Size() * 0.5f);
	return root;
}

/*************************************************************************/
/*************************************************************************/
Octant Octant::Root(
const AABB &aabb)
{
#ifndef LOOSEOCTTREE
	return StandardRoot(aabb);
#else
	return LooseRoot(aabb);
#endif
}

