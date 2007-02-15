/***************************************************************************/
// Implementation file for the OBB's
/***************************************************************************/
#include "Game/Database/Som.h"
#include "collide/xzrect.h"

/***************************************************************************/
// setup the radius
/***************************************************************************/
inline float OBB::ComputeRadius(
const AABB &aabb)
{
	return aabb.maxV.Length();
}

/***************************************************************************/
// If you monkey with the AABB, call this!
/***************************************************************************/
inline void OBB::UpdateRadius()
{
	radius = ComputeRadius(aabb);
}

/***************************************************************************/
// Setup the box
/***************************************************************************/
inline void OBB::SetBoundingVolume(
const AABB *modelAABB,
bool forceSymmetric)
{
	if (modelAABB)
	{
		// Force it to be symmetric?
		if (!forceSymmetric)
			aabb = *modelAABB;
		else
		{
			/* note-- this centers the box at (0, 0, 0), but does so by expanding the box so that */
			/* maxV.x = max(abs(maxV.x), abs(minV.x)), etc. it does NOT move the box so that it is */
			/* the same size but centered at (0, 0, 0) */

			Vector3 absMin = Vector3::Abs(modelAABB->minV);
			Vector3 absMax = Vector3::Abs(modelAABB->maxV);

			aabb.maxV = Vector3::Max(absMin, absMax);
			aabb.minV = -aabb.maxV;
		}

		// has to be symmetric
		ASSERT(Math::Zero((aabb.maxV + aabb.minV).LengthSquared()));

		// setup the radius
		radius = ComputeRadius(aabb);
	}
}

/***************************************************************************/
/***************************************************************************/
inline void OBB::ApplyTranslation(
Vector3CRef t)
{
	ASSERT(pos);
	*pos += t;
}

/***************************************************************************/
// Is this OBB totally inside of me?
/***************************************************************************/
inline bool OBB::TotallyInside(
OBB &test)
{
	DirCos3x3 testToObb = (*test.parent) / (*parent);
	Vector3 testInObb = ((*test.pos) - (*pos)) / (*parent);

	float extent = test.aabb.maxV.x() * Math::Abs(testToObb.e00) +
			test.aabb.maxV.y() * Math::Abs(testToObb.e01) +
			test.aabb.maxV.z() * Math::Abs(testToObb.e02);
	if (((testInObb.x() + extent) > aabb.maxV.x()) ||
			((testInObb.x() - extent) < aabb.minV.x()))
		return(false);

	extent = test.aabb.maxV.x() * Math::Abs(testToObb.e10) +
			test.aabb.maxV.y() * Math::Abs(testToObb.e11) +
			test.aabb.maxV.z() * Math::Abs(testToObb.e12);
	if (((testInObb.y() + extent) > aabb.maxV.y()) ||
			((testInObb.y() - extent) < aabb.minV.y()))
		return(false);

	extent = test.aabb.maxV.x() * Math::Abs(testToObb.e20) +
			test.aabb.maxV.y() * Math::Abs(testToObb.e21) +
			test.aabb.maxV.z() * Math::Abs(testToObb.e22);
	if (((testInObb.z() + extent) > aabb.maxV.z()) ||
			((testInObb.z() - extent) < aabb.minV.z()))
		return(false);

	return(true);
}

/***************************************************************************/
// Get a vertex of the OBB
/***************************************************************************/
inline Vector3 OBB::GetVertex(
uint index) const
{
	ASSERT(index < 8);

	// This is crazy cheating
	float *p = (float *)&aabb;
	Vector3 vertex( p[vertexIndex[index][0]],
						 p[vertexIndex[index][1]],
						 p[vertexIndex[index][2]]);
	return(vertex);
}

/***************************************************************************/
// Get the vertex indices for an edge
/***************************************************************************/
inline void OBB::GetEdge(
uint edge,
uint &vec1,
uint &vec2)
{
	ASSERT(edge < 12);
	vec1 = edgeVertexIndex[edge][0];
	vec2 = edgeVertexIndex[edge][1];
}

/***************************************************************************/
// Get the vertex indices for a face CCW order
/***************************************************************************/
inline void OBB::GetFace(
uint face,
uint &vec1,
uint &vec2,
uint &vec3,
uint &vec4)
{
	ASSERT(face < 6);
	vec1 = faceVertexIndex[face][0];
	vec2 = faceVertexIndex[face][1];
	vec3 = faceVertexIndex[face][2];
	vec4 = faceVertexIndex[face][3];
}

/***************************************************************************/
// Get a normal
/***************************************************************************/
inline void OBB::GetFaceNormal(
uint face,
Vector3 &normal) const
{
	ASSERT(face < 6);
	ASSERT(parent);

	if ((face & 1) == 0)
		normal = parent->Row(face >> 1).Negated();
	else
		normal = parent->Row(face >> 1);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3 OBB::GetFaceNormal(
uint face) const
{
	Vector3 normal;
	GetFaceNormal(face, normal);
	return(normal);
}

/***************************************************************************/
// Get the vertex indices for a face CCW order
/***************************************************************************/
inline void OBB::GetFace(
uint face,
uint &v1,
uint &v2,
uint &v3,
uint &v4,
Vector3 &normal) const
{
	OBB::GetFace(face, v1, v2, v3, v4);
	GetFaceNormal(face, normal);
}

/***************************************************************************/
// Requires an 8 entry vertex array
/***************************************************************************/
inline void OBB::Transform(
const DirCos3x3 &m,
Vector3CRef pos,
Vector3 *vList) const
{
	for (uint i = 0; i < 8; i++)
		vList[i] = (GetVertex(i) * m) + pos;
}

/***************************************************************************/
// Requires an 8 entry vertex array
/***************************************************************************/
inline void OBB::Transform(
const Cartesian4x4 &m,
Vector3 *vList) const
{
	for (uint i = 0; i < 8; i++)
		vList[i] = GetVertex(i) * m;
}

/***************************************************************************/
/***************************************************************************/
inline void OBB::ComputeWorldExtents(XZRect &o_worldRect) const
{
	ASSERT(pos);
	ASSERT(parent);

	float xSize = (Vector3::Abs(parent->GetColumn0()) * Vector3(aabb.maxV)).SumXYZ();

	o_worldRect.MinX(pos->x() - xSize);
	o_worldRect.MaxX(pos->x() + xSize);

	float zSize = (Vector3::Abs(parent->GetColumn2()) * Vector3(aabb.maxV)).SumXYZ();
	o_worldRect.MinZ(pos->z() - zSize);
	o_worldRect.MaxZ(pos->z() + zSize);
}

/***************************************************************************/
/***************************************************************************/
inline void OBB::ComputeWorldExtents(AABB &o_worldAABB) const
{
#if 0
	ASSERT(pos);
	ASSERT(parent);

	Vector3 inWorld[8];
	Transform(*parent, *pos, inWorld);
	o_worldAABB.Set(inWorld[0], inWorld[0]);
	for (int i = 1; i < 8; i++)
		o_worldAABB.ExpandByPoint(inWorld[i]);
#else
	ASSERT(pos);
	ASSERT(parent);

	Vector3 size;
	size.x( aabb.maxV.x() * Math::Abs(parent->e00) +	aabb.maxV.y() * Math::Abs(parent->e10) +	aabb.maxV.z() * Math::Abs(parent->e20));
	size.y( aabb.maxV.x() * Math::Abs(parent->e01) +	aabb.maxV.y() * Math::Abs(parent->e11) +	aabb.maxV.z() * Math::Abs(parent->e21));
	size.z( aabb.maxV.x() * Math::Abs(parent->e02) +	aabb.maxV.y() * Math::Abs(parent->e12) +	aabb.maxV.z() * Math::Abs(parent->e22));

	o_worldAABB.CenterHalfSize(*pos, size);
#endif
}

// get the center and the x, y, and z vectors in world coordinates.
inline void OBB::ComputeWorldCoords( Vector3 &o_center, Vector3 &x, Vector3 &y, Vector3 &z )
{
	ASSERT( pos );
	ASSERT( parent );

	o_center = *pos;

	x = (parent->GetRow0() * GetConstAABB().maxV.x());
	y = (parent->GetRow1() * GetConstAABB().maxV.y());
	z = (parent->GetRow2() * GetConstAABB().maxV.z());
}

/***************************************************************************/
/***************************************************************************/
inline bool OBB::IntersectLineInWorld(Vector3CRef i_originInWorld,
		Vector3CRef i_basisInWorld, uint &o_entryFace, float &o_entryPoint,
		uint &o_exitFace, float &o_exitPoint) const
{
	ASSERT(pos);
	ASSERT(parent);

	Vector3 originInOBB = (i_originInWorld - (*pos)) / (*parent);
	Vector3 basisInOBB = i_basisInWorld / (*parent);
	return(aabb.IntersectLine(originInOBB, basisInOBB, o_entryFace, o_entryPoint,
			o_exitFace, o_exitPoint));
}

/***************************************************************************/
// Find distances between a plane (described in the OBB) and OBB corners.
//
// The front distance is the distance along the plane normal from the plane
// to the corner of the OBB the furthest from the OBB center in the
// direction of the plane normal.
//
// The back distance is the distance along the plane normal from the plane
// to the corner of the OBB the furthest from the OBB center in the
// direction opposite the plane normal.
//
// A positive value for a distance means the corresponding corner is in
// front of the plane. A negative value means the corresponcing corner is
// behind the plane.
/***************************************************************************/
inline void OBB::ComputeOBBPlaneDistance(Vector3CRef i_planePointInOBB,
		Vector3CRef i_planeNormalInOBB, float &o_frontDistance,
		float &o_backDistance) const
{
	Vector3 planeNormalAbs = Vector3::Abs(i_planeNormalInOBB);
	float centerDistance = aabb.maxV.Dot(planeNormalAbs);
	float planeDistance = i_planeNormalInOBB.Dot(i_planePointInOBB);
	o_frontDistance = centerDistance - planeDistance;
	o_backDistance = -centerDistance - planeDistance;
}

/***************************************************************************/
// Find distances between a plane (described in world) and OBB corners.
/***************************************************************************/
inline void OBB::ComputeWorldPlaneDistance(Vector3CRef i_planePointInWorld,
		Vector3CRef i_planeNormalInWorld, float &o_frontDistance,
		float &o_backDistance) const
{
	ASSERT(pos);
	ASSERT(parent);

	ComputeOBBPlaneDistance((i_planePointInWorld - (*pos)) / (*parent),
			i_planeNormalInWorld / (*parent), o_frontDistance, o_backDistance);
}

/***************************************************************************/
// Find furthest corner from the front side of a plane.
/***************************************************************************/
inline Vector3 OBB::GetOBBFrontCorner(Vector3CRef i_planeNormalInOBB)
{
	Vector3 corner = aabb.maxV;
	if (i_planeNormalInOBB.X() < 0.0f)
		corner.X(-corner.X());
	if (i_planeNormalInOBB.Y() < 0.0f)
		corner.Y(-corner.Y());
	if (i_planeNormalInOBB.Z() < 0.0f)
		corner.Z(-corner.Z());
	return(corner);
}

/***************************************************************************/
// Find furthest corner from the back side of a plane.
/***************************************************************************/
inline Vector3 OBB::GetOBBBackCorner(Vector3CRef i_planeNormalInOBB)
{
	Vector3 corner = aabb.maxV;
	if (i_planeNormalInOBB.X() > 0.0f)
		corner.X(-corner.X());
	if (i_planeNormalInOBB.Y() > 0.0f)
		corner.Y(-corner.Y());
	if (i_planeNormalInOBB.Z() > 0.0f)
		corner.Z(-corner.Z());
	return(corner);
}

/***************************************************************************/
// Find furthest corner from the front side of a plane, returning the corner's index
/***************************************************************************/
inline int OBB::GetOBBFrontCornerIndex( Vector3CRef i_planeNormalInOBB )
{
	// this all makes sense if you look at the order of the corners.  I swear it.
	int corner = 0;
	if( i_planeNormalInOBB.y() > 0.0f ) // switch 0 to 4 for maxy
		corner += 4;	
	if( i_planeNormalInOBB.z() > 0.0f ) // switch from 0 or 4 to 2 or 6 for maxz
		corner += 2;
	if( i_planeNormalInOBB.x() > 0.0f ) // switch from 0, 2, 4, or 6, to 1, 3, 5, or 7 for maxx
		corner++;

	return corner;
}

/***************************************************************************/
// Find furthest corner from the front side of a plane.
/***************************************************************************/
inline Vector3 OBB::GetWorldFrontCorner(Vector3CRef i_planeNormalInWorld)
{
	return(GetOBBFrontCorner(i_planeNormalInWorld / (*parent)) * (*parent) +
			(*pos));
}

/***************************************************************************/
// Find furthest corner from the back side of a plane.
/***************************************************************************/
inline Vector3 OBB::GetWorldBackCorner(Vector3CRef i_planeNormalInWorld)
{
	return(GetOBBBackCorner(i_planeNormalInWorld / (*parent)) * (*parent) +
			(*pos));
}

inline int OBB::GetWorldFrontCornerIndex( Vector3CRef i_planeNormalInWorld )
{
	return GetOBBFrontCornerIndex(i_planeNormalInWorld / (*parent));
}

/***************************************************************************/
/***************************************************************************/
inline void OBB::PlaneIntersectionX(SObbPlaneIntersections &io_intersections,
		enum EPlaneIntersectPath i_path, uint8 i_mask,
		Vector3CRef i_planeNormalInObb, float i_planeDistance,
		float i_lengthY, float i_lengthZ, float i_extentY, float i_extentZ) const
{
	io_intersections.d_pointInObb[i_path].Set(i_planeDistance - i_lengthY -
			i_lengthZ, i_extentY, i_extentZ);
	if (!Math::DivByZero(io_intersections.d_pointInObb[i_path].x(),
				i_planeNormalInObb.x()))
	{
		io_intersections.d_pointInObb[i_path].x(
			io_intersections.d_pointInObb[i_path].x() / i_planeNormalInObb.x());
	}
	else
		io_intersections.d_pointInObb[i_path].x(0.0f);
	io_intersections.d_intersects |= i_mask;
	io_intersections.d_master |= i_mask;
}

/***************************************************************************/
/***************************************************************************/
inline void OBB::PlaneIntersectionY(SObbPlaneIntersections &io_intersections,
		enum EPlaneIntersectPath i_path, uint8 i_mask,
		Vector3CRef i_planeNormalInObb, float i_planeDistance,
		float i_lengthX, float i_lengthZ, float i_extentX, float i_extentZ) const
{
	io_intersections.d_pointInObb[i_path].Set(i_extentX,
			i_planeDistance - i_lengthX - i_lengthZ, i_extentZ);
	if (!Math::DivByZero(io_intersections.d_pointInObb[i_path].y(),
				i_planeNormalInObb.y()))
		io_intersections.d_pointInObb[i_path].y(
			io_intersections.d_pointInObb[i_path].y() / 
			i_planeNormalInObb.y());
	else
		io_intersections.d_pointInObb[i_path].y(0.0f);
	io_intersections.d_intersects |= i_mask;
	io_intersections.d_master |= i_mask;
}

/***************************************************************************/
/***************************************************************************/
inline void OBB::PlaneIntersectionZ(SObbPlaneIntersections &io_intersections,
		enum EPlaneIntersectPath i_path, uint8 i_mask,
		Vector3CRef i_planeNormalInObb, float i_planeDistance,
		float i_lengthX, float i_lengthY, float i_extentX, float i_extentY) const
{
	io_intersections.d_pointInObb[i_path].Set(i_extentX, i_extentY,
			i_planeDistance - i_lengthX - i_lengthY);
	if (!Math::DivByZero(io_intersections.d_pointInObb[i_path].z(),
				i_planeNormalInObb.z()))
		io_intersections.d_pointInObb[i_path].z(
			io_intersections.d_pointInObb[i_path].z() / i_planeNormalInObb.z());
	else
		io_intersections.d_pointInObb[i_path].z(0.0f);
	io_intersections.d_intersects |= i_mask;
	io_intersections.d_master |= i_mask;
}

/***************************************************************************/
/***************************************************************************/
inline bool OBB::ComputeObbPlaneIntersections(
		Vector3CRef i_planePointInObb, Vector3CRef i_planeNormalInObb,
		SObbPlaneIntersections &o_intersections) const
{
/*
 *	Clear the flags.
 */
	o_intersections.d_intersects = 0;
	o_intersections.d_master = 0;
/*
 *	Find extents with the signs of the normal.
 */
	Vector3 extent = aabb.maxV;
	if (i_planeNormalInObb.x() < 0.0f)
		extent.x(-extent.x());
	if (i_planeNormalInObb.y() < 0.0f)
		extent.y(-extent.y());
	if (i_planeNormalInObb.z() < 0.0f)
		extent.z(-extent.z());
/*
 *	Find the edge lengths projected into the plane normal.
 */
	Vector3 edgeLength = extent.Diag(i_planeNormalInObb);
/*
 *	Find the distance to the plane.
 */
	float planeDistance = i_planeNormalInObb.Dot(i_planePointInObb);
/*
 *	Find the distance to the corners in the direction of the normal (the "peak"
 *	corners. See if the plane falls between the peaks.
 */
	float cornerDistance = edgeLength.x() + edgeLength.y() + edgeLength.z();
	if ((planeDistance > cornerDistance) || (planeDistance < -cornerDistance))
		return(false);
/*
 *	Find the distance to the corners connected by the x-axis to the peaks.
 */
	cornerDistance = edgeLength.y() + edgeLength.z() - edgeLength.x();
/*
 *	See if the plane intersects the x-edge connected to the front peak. If so,
 *	find the intersection point and mark the paths the go through it as
 *	intersected.
 */
	if (planeDistance > cornerDistance)
	{
		PlaneIntersectionX(o_intersections, PATH_XZY, MASK_XZY,
				i_planeNormalInObb, planeDistance, edgeLength.y(), edgeLength.z(),
				extent.y(), extent.z());
		o_intersections.d_pointInObb[PATH_XYZ] =
				o_intersections.d_pointInObb[PATH_XZY];
		o_intersections.d_intersects |= MASK_XYZ;
	}
/*
 *	See if the plane intersects the x-edge connected to the back peak. If so,
 *	find the intersection point and mark the paths the go through it as
 *	intersected.
 */
	if (planeDistance < -cornerDistance)
	{
		PlaneIntersectionX(o_intersections, PATH_YZX, MASK_YZX,
				i_planeNormalInObb, planeDistance, -edgeLength.y(), -edgeLength.z(),
				-extent.y(), -extent.z());
		o_intersections.d_pointInObb[PATH_ZYX] =
				o_intersections.d_pointInObb[PATH_YZX];
		o_intersections.d_intersects |= MASK_ZYX;
	}
/*
 *	See if any paths going through the y-edges connected to the peaks.
 */
	if (((o_intersections.d_intersects & (MASK_YXZ | MASK_YZX)) == 0) ||
			((o_intersections.d_intersects & (MASK_ZXY | MASK_XZY)) == 0))
	{
/*
 *		Find the distance to the corners connected by the y-axis to the peaks.
 */
		cornerDistance = edgeLength.x() - edgeLength.y() + edgeLength.z();
/*
 *		See if the plane intersects the y-edge connected to the front peak. If so,
 *		find the intersection point and mark the paths the go through it as
 *		intersected.
 */
		if (planeDistance > cornerDistance)
		{
			ASSERT((o_intersections.d_intersects & (MASK_YXZ | MASK_YZX)) == 0);
			PlaneIntersectionY(o_intersections, PATH_YXZ, MASK_YXZ,
					i_planeNormalInObb, planeDistance, edgeLength.x(), edgeLength.z(),
					extent.x(), extent.z());
			o_intersections.d_pointInObb[PATH_YZX] =
					o_intersections.d_pointInObb[PATH_YXZ];
			o_intersections.d_intersects |= MASK_YZX;
		}
/*
 *		See if the plane intersects the y-edge connected to the back peak. If so,
 *		find the intersection point and mark the paths the go through it as
 *		intersected.
 */
		if (planeDistance < -cornerDistance)
		{
			ASSERT((o_intersections.d_intersects & (MASK_ZXY | MASK_XZY)) == 0);
			PlaneIntersectionY(o_intersections, PATH_ZXY, MASK_ZXY,
					i_planeNormalInObb, planeDistance, -edgeLength.x(), -edgeLength.z(),
					-extent.x(), -extent.z());
			o_intersections.d_pointInObb[PATH_XZY] =
					o_intersections.d_pointInObb[PATH_ZXY];
			o_intersections.d_intersects |= MASK_XZY;
		}
	}
/*
 *	See if any paths going through the z-edges connected to the peaks.
 */
	if (((o_intersections.d_intersects & (MASK_ZYX | MASK_ZXY)) == 0) ||
			((o_intersections.d_intersects & (MASK_XYZ | MASK_YXZ)) == 0))
	{
/*
 *		Find the distance to the corners connected by the z-axis to the peaks.
 */
		cornerDistance = edgeLength.x() + edgeLength.y() - edgeLength.z();
/*
 *		See if the plane intersects the z-edge connected to the front peak. If so,
 *		find the intersection point and mark the paths the go through it as
 *		intersected.
 */
		if (planeDistance > cornerDistance)
		{
			ASSERT((o_intersections.d_intersects & (MASK_ZYX | MASK_ZXY)) == 0);
			PlaneIntersectionZ(o_intersections, PATH_ZYX, MASK_ZYX,
					i_planeNormalInObb, planeDistance, edgeLength.x(), edgeLength.y(),
					extent.x(), extent.y());
			o_intersections.d_pointInObb[PATH_ZXY] =
					o_intersections.d_pointInObb[PATH_ZYX];
			o_intersections.d_intersects |= MASK_ZXY;
		}
/*
 *		See if the plane intersects the z-edge connected to the back peak. If so,
 *		find the intersection point and mark the paths the go through it as
 *		intersected.
 */
		if (planeDistance < -cornerDistance)
		{
			ASSERT((o_intersections.d_intersects & (MASK_XYZ | MASK_YXZ)) == 0);
			PlaneIntersectionZ(o_intersections, PATH_XYZ, MASK_XYZ,
					i_planeNormalInObb, planeDistance, -edgeLength.x(), -edgeLength.y(),
					-extent.x(), -extent.y());
			o_intersections.d_pointInObb[PATH_YXZ] =
					o_intersections.d_pointInObb[PATH_XYZ];
			o_intersections.d_intersects |= MASK_YXZ;
		}
	}
/*
 *	Check each of the paths to see if they have been intersected. If not, they
 *	must be intersected on the edges not connected to either peak.
 */
	if ((o_intersections.d_intersects & MASK_XYZ) == 0)
	{
		PlaneIntersectionY(o_intersections, PATH_XYZ, MASK_XYZ,
				i_planeNormalInObb, planeDistance, -edgeLength.x(), edgeLength.z(),
				-extent.x(), extent.z());
	}
	if ((o_intersections.d_intersects & MASK_YXZ) == 0)
	{
		PlaneIntersectionX(o_intersections, PATH_YXZ, MASK_YXZ,
				i_planeNormalInObb, planeDistance, -edgeLength.y(), edgeLength.z(),
				-extent.y(), extent.z());
	}
	if ((o_intersections.d_intersects & MASK_YZX) == 0)
	{
		PlaneIntersectionZ(o_intersections, PATH_YZX, MASK_YZX,
				i_planeNormalInObb, planeDistance, edgeLength.x(), -edgeLength.y(),
				extent.x(), -extent.y());
	}
	if ((o_intersections.d_intersects & MASK_ZYX) == 0)
	{
		PlaneIntersectionY(o_intersections, PATH_ZYX, MASK_ZYX,
				i_planeNormalInObb, planeDistance, edgeLength.x(), -edgeLength.z(),
				extent.x(), -extent.z());
	}
	if ((o_intersections.d_intersects & MASK_ZXY) == 0)
	{
		PlaneIntersectionX(o_intersections, PATH_ZXY, MASK_ZXY,
				i_planeNormalInObb, planeDistance, edgeLength.y(), -edgeLength.z(),
				extent.y(), -extent.z());
	}
	if ((o_intersections.d_intersects & MASK_XZY) == 0)
	{
		PlaneIntersectionZ(o_intersections, PATH_XZY, MASK_XZY,
				i_planeNormalInObb, planeDistance, -edgeLength.x(), edgeLength.y(),
				-extent.x(), extent.y());
	}

	return(true);
}

/***************************************************************************/
/***************************************************************************/
inline bool OBB::ComputeWorldPlaneIntersections(
		Vector3CRef i_planePointInWorld, Vector3CRef i_planeNormalInWorld,
		SObbPlaneIntersections &o_intersections) const
{
	ASSERT(pos);
	ASSERT(parent);

	return(ComputeObbPlaneIntersections((i_planePointInWorld - (*pos)) /
			(*parent), i_planeNormalInWorld / (*parent), o_intersections));
}

/***************************************************************************/
/***************************************************************************/

#define CROSS_BUFFER  1.0e-4f   // Catch parallel edges.

inline bool OBB::TestTriangle(Vector3CRef i_normal, Vector3CRef i_v0,
		Vector3CRef i_v1, Vector3CRef i_v2, Vector3 &o_normalInObb,
		Vector3 &o_v0InObb, Vector3 &o_v1InObb, Vector3 &o_v2InObb) const
{
/*
 *	Start by essentially doing a sphere/triangle separating axis test.
 *
 *	See if the distance from the triangle plane to the center of the OBB
 *	is greater than the radius of the OBB.
 */
	o_v0InObb = i_v0 - (*pos);
	float height = Math::Abs(i_normal.Dot(o_v0InObb));
	if (height > radius)
		return(false);
/*
 *	Find the square of the radius of the circle resulting from the intersection
 *	of the OBB sphere and the triangle plane.
 */
	float radiusSq = radius * radius - height * height;
/*
 *	Now test the intersection circle against the edges of the triangle. Note:
 *	this assumes that the triangles are right-handed.
 */
	o_v1InObb = i_v1 - (*pos);
	Vector3 edge = o_v1InObb - o_v0InObb;
	float centerDistTerm = o_v0InObb.Dot(i_normal.Cross(edge));
	if ((centerDistTerm > 0.0f) && ((centerDistTerm * centerDistTerm) >
			(edge.LengthSquared() * radiusSq)))
		return(false);

	o_v2InObb = i_v2 - (*pos);
	edge = o_v2InObb - o_v1InObb;
	centerDistTerm = o_v1InObb.Dot(i_normal.Cross(edge));
	if ((centerDistTerm > 0.0f) && ((centerDistTerm * centerDistTerm) >
			(edge.LengthSquared() * radiusSq)))
		return(false);

	edge = o_v0InObb - o_v2InObb;
	centerDistTerm = o_v2InObb.Dot(i_normal.Cross(edge));
	if ((centerDistTerm > 0.0f) && ((centerDistTerm * centerDistTerm) >
			(edge.LengthSquared() * radiusSq)))
		return(false);
/*
 *	Now do the full-on separating axis test.
 *
 *	Find vertices in OBB coords.
 */
	o_v0InObb = o_v0InObb / (*parent);
	o_v1InObb = o_v1InObb / (*parent);
	o_v2InObb = o_v2InObb / (*parent);
/*
 *	Check OBB faces.
 */
	if (((o_v0InObb.x() > aabb.maxV.x()) && (o_v1InObb.x() > aabb.maxV.x()) &&
			(o_v2InObb.x() > aabb.maxV.x())) || ((o_v0InObb.x() < -aabb.maxV.x()) &&
			(o_v1InObb.x() < -aabb.maxV.x()) && (o_v2InObb.x() < -aabb.maxV.x())))
		return(false);

	if (((o_v0InObb.y() > aabb.maxV.y()) && (o_v1InObb.y() > aabb.maxV.y()) &&
			(o_v2InObb.y() > aabb.maxV.y())) || ((o_v0InObb.y() < -aabb.maxV.y()) &&
			(o_v1InObb.y() < -aabb.maxV.y()) && (o_v2InObb.y() < -aabb.maxV.y())))
		return(false);

	if (((o_v0InObb.z() > aabb.maxV.z()) && (o_v1InObb.z() > aabb.maxV.z()) &&
			(o_v2InObb.z() > aabb.maxV.z())) || ((o_v0InObb.z() < -aabb.maxV.z()) &&
			(o_v1InObb.z() < -aabb.maxV.z()) && (o_v2InObb.z() < -aabb.maxV.z())))
		return(false);

/*
 *	Check triangle face.
 */
	o_normalInObb = i_normal / (*parent);
	Vector3 absNormal = Vector3::Abs(o_normalInObb);
	if (height > absNormal.Dot(aabb.maxV))
		return(false);
/*
 *	Now check the cross products of the edges.
 */
	Vector3 crossTerm[3];
	crossTerm[0] = o_v1InObb.Cross(o_v0InObb);
	crossTerm[1] = o_v2InObb.Cross(o_v1InObb);
	crossTerm[2] = o_v0InObb.Cross(o_v2InObb);

/*
 * Genarate sums of cross components
 */
	Vector3 sum12 = crossTerm[1] + crossTerm[2];
	Vector3 sum01 = crossTerm[0] + crossTerm[1];
	Vector3 sum02 = crossTerm[0] + crossTerm[2];

/*
 *	X-axis cross edge 0.
 */
	edge = Vector3::Abs(o_v1InObb - o_v0InObb);
	float obbDist;
	obbDist = aabb.maxV.y() * edge.z() + aabb.maxV.z() * edge.y() + CROSS_BUFFER;
	float crossSum = sum12.x();
	if (((crossTerm[0].x() > obbDist) && (crossSum < -obbDist)) ||
			((crossTerm[0].x() < -obbDist) && (crossSum > obbDist)))
		return(false);
/*
 *	Y-axis cross edge 0.
 */
	obbDist = aabb.maxV.x() * edge.z() + aabb.maxV.z() * edge.x() + CROSS_BUFFER;
	crossSum = sum12.y();
	if (((crossTerm[0].y() > obbDist) && (crossSum < -obbDist)) ||
			((crossTerm[0].y() < -obbDist) && (crossSum > obbDist)))
		return(false);
/*
 *	Z-axis cross edge 0.
 */
	obbDist = aabb.maxV.x() * edge.y() + aabb.maxV.y() * edge.x() + CROSS_BUFFER;
	crossSum = sum12.z();
	if (((crossTerm[0].z() > obbDist) && (crossSum < -obbDist)) ||
			((crossTerm[0].z() < -obbDist) && (crossSum > obbDist)))
		return(false);
/*
 *	X-axis cross edge 1.
 */
	edge = Vector3::Abs(o_v2InObb - o_v1InObb);
	obbDist = aabb.maxV.y() * edge.z() + aabb.maxV.z() * edge.y() + CROSS_BUFFER;
	crossSum = sum02.x();
	if (((crossTerm[1].x() > obbDist) && (crossSum < -obbDist)) ||
			((crossTerm[1].x() < -obbDist) && (crossSum > obbDist)))
		return(false);
/*
 *	Y-axis cross edge 1.
 */
	obbDist = aabb.maxV.x() * edge.z() + aabb.maxV.z() * edge.x() + CROSS_BUFFER;
	crossSum = sum02.y();
	if (((crossTerm[1].y() > obbDist) && (crossSum < -obbDist)) ||
			((crossTerm[1].y() < -obbDist) && (crossSum > obbDist)))
		return(false);
/*
 *	Z-axis cross edge 1.
 */
	obbDist = aabb.maxV.x() * edge.y() + aabb.maxV.y() * edge.x() + CROSS_BUFFER;
	crossSum = sum02.z();
	if (((crossTerm[1].z() > obbDist) && (crossSum < -obbDist)) ||
			((crossTerm[1].z() < -obbDist) && (crossSum > obbDist)))
		return(false);
/*
 *	X-axis cross edge 2.
 */
	edge = Vector3::Abs(o_v0InObb - o_v2InObb);
	obbDist = aabb.maxV.y() * edge.z() + aabb.maxV.z() * edge.y() + CROSS_BUFFER;
	crossSum = sum01.x();
	if (((crossTerm[2].x() > obbDist) && (crossSum < -obbDist)) ||
			((crossTerm[2].x() < -obbDist) && (crossSum > obbDist)))
		return(false);
/*
 *	Y-axis cross edge 2.
 */
	obbDist = aabb.maxV.x() * edge.z() + aabb.maxV.z() * edge.x() + CROSS_BUFFER;
	crossSum = sum01.y();
	if (((crossTerm[2].y() > obbDist) && (crossSum < -obbDist)) ||
			((crossTerm[2].y() < -obbDist) && (crossSum > obbDist)))
		return(false);
/*
 *	Z-axis cross edge 2.
 */
	obbDist = aabb.maxV.x() * edge.y() + aabb.maxV.y() * edge.x() + CROSS_BUFFER;
	crossSum = sum01.z();
	if (((crossTerm[2].z() > obbDist) && (crossSum < -obbDist)) ||
			((crossTerm[2].z() < -obbDist) && (crossSum > obbDist)))
		return(false);

	return(true);
}

/***************************************************************************/
// Is this point in the box?
/***************************************************************************/
inline bool OBB::PointInBox(
Vector3CRef test)
{
	Vector3 oTest = (test - *pos) / *parent;
	return(aabb.PointIn(oTest));
}

/***************************************************************************/
/***************************************************************************/
inline void OBBReference::SetTransform(
DirCos3x3 *orientation,
Vector3Packed *position)
{
	parent = orientation;
	pos = position;
}

/***************************************************************************/
/***************************************************************************/
inline void OBBReference::SetTransform(
DirCos3x3 *orientation,
Vector3Padded *position)
{
	parent = orientation;
	pos = (Vector3Packed*)position;
}

/***************************************************************************/
// Setup the position/orientation from the passed values
// These are stored locally. SetTransform works for an obb that is
// "naturally" centered around (0, 0, 0), that is, when the bounding
// box is first calculated it's center is (0, 0, 0) without any adjustment
/***************************************************************************/
inline void OBBInstance::SetTransform(
const Cartesian4x4 &m)
{
	orientation = m;
	position = m.GetTranslation();
}

/***************************************************************************/
// Setup the position/orientation from the passed values
// These are stored locally. SetTransform works for an obb that is
// "naturally" centered around (0, 0, 0), that is, when the bounding
// box is first calculated it's center is (0, 0, 0) without any adjustment
/***************************************************************************/
inline void OBBInstance::SetTransform(
const DirCos3x3 &o,
Vector3CRef p)
{
	orientation = o;
	position = p;
}

/***************************************************************************/
// SetOffsetTransform works for an obb that was not initially centered on
// (0, 0, 0). obb's have to be symmetric (centered on (0, 0, 0)) to work,
// so the obb has to be adjusted before it can be used. the origin parameter
// is the original center of the obb before it was made symmetric
/***************************************************************************/
inline void OBBInstance::SetOffsetTransform(
const Cartesian4x4 &m,
Vector3CRef origin)
{
	orientation = m;
	position = m.GetTranslation() + (origin * orientation);
}

/***************************************************************************/
// SetOffsetTransform works for an obb that was not initially centered on
// (0, 0, 0). obb's have to be symmetric (centered on (0, 0, 0)) to work,
// so the obb has to be adjusted before it can be used. the origin parameter
// is the original center of the obb before it was made symmetric
/***************************************************************************/
inline void OBBInstance::SetOffsetTransform(
const DirCos3x3 &o,
Vector3CRef p,
Vector3CRef origin)
{
	orientation = o;
	position = p + (origin * o);
}

/***************************************************************************/
/***************************************************************************/
inline OBBInstance &OBBInstance::operator=(
const OBB &o)
{
	aabb = o.GetConstAABB();
	radius = o.Radius();

	ASSERT(o.Matrix() != NULL);
	orientation = *o.Matrix();

	ASSERT(o.Center() != NULL);
	position = *o.Center();

	return(*this);
}

/***************************************************************************/
/***************************************************************************/
inline OBBInstance &OBBInstance::operator=(
const OBBReference &o)
{
	aabb = o.GetConstAABB();
	radius = o.Radius();

	ASSERT(o.Matrix() != NULL);
	orientation = *o.Matrix();

	ASSERT(o.Center() != NULL);
	position = *o.Center();

	return(*this);
}

/***************************************************************************/
/***************************************************************************/
inline OBBInstance &OBBInstance::operator=(
const OBBInstance &o)
{
	aabb = o.aabb;
	radius = o.radius;
	orientation = o.orientation;
	position = o.position;

	return(*this);
}

/***************************************************************************/
/***************************************************************************/
inline void OBBStaticInstance::SetMaterial(
u8 newMaterial)
{
	if (material <= INVALID_MATERIALINDEX)
		material = newMaterial;
	else
		material = INVALID_MATERIALINDEX;
}

/***************************************************************************/
/***************************************************************************/
inline void OBBStaticInstance::SetColor(
u16 newColor)
{
	color = newColor;
}
