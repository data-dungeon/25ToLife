/***************************************************************************/
// Fast axis-aligned bounding box stuff
/***************************************************************************/
#ifndef AABB_H
#define AABB_H

#include "Game/Misc/Bounds.h"
#include "Math/Matrix.h"
#include "collide/xzrect.h"

class AABB
{
public:
	// Constructors
	inline AABB() {}
	inline AABB(const ts_BoundingBox &bbox)
		{ minV = bbox.Min; maxV = bbox.Max; }

	// Access the data
	inline void Set(Vector3CRef newMinV, Vector3CRef newMaxV);

	// Dunno if these corners are normalized
	inline void SetNormalized(Vector3CRef a, Vector3CRef b);

	// Make this aabb into a cube with a side length of size
	inline void MakeCube(Vector3CRef center, float size);

	// Set a center and halfsize
	inline void CenterHalfSize(Vector3CRef center, Vector3CRef halfSize);

	// From a sphere
	inline void FromSphere(const Vector3 &center, float radius);

	// Clear this to an empty AABB
	inline void Clear();

	// Normalize the box (make sure min is min and max is max)
	inline void Normalize();

	// Expand by a point
	inline void ExpandByPoint(const Vector3 &p);

	// Get at the centers
	inline Vector3 Center() const
		{ return ((maxV + minV) * 0.5f); }
	inline float XCenter() const
		{ return ((maxV.x() + minV.x()) * 0.5f); }
	inline float YCenter() const
		{ return ((maxV.y() + minV.y()) * 0.5f); }
	inline float ZCenter() const
		{ return ((maxV.z() + minV.z()) * 0.5f); }

	// Get at the sizes
	inline float XSize() const
		{ return (maxV.x() - minV.x()); }
	inline float YSize() const
		{ return (maxV.y() - minV.y()); }
	inline float ZSize() const
		{ return (maxV.z() - minV.z()); }
	inline Vector3 Size() const
		{ return (maxV - minV); }

	inline Vector3Packed GetMaxV() const
	{ return (maxV); }

	inline Vector3Packed GetMinV() const
	{ return (minV); }

	// The volume
	inline float Volume() const;

	// Make this AABB into a cube using the largest dimension
	inline void FitCube();

	// Inflate the AABB by the passed amount
	inline void Inflate(const Vector3 &inflate);

	// Get the XZ Rect
	inline XZRect ToRect();

	// Point in box?
	inline bool PointIn(Vector3CRef p) const;

	// Intersect a line (uses same face ID as OBBs)
	inline bool IntersectLine(Vector3CRef i_origin, Vector3CRef i_basis,
			uint &o_entryFace, float &o_entryPoint,
			uint &o_exitFace, float &o_exitPoint) const;

	// Segment crosses box
	inline bool SegmentCrosses(Vector3 &start, Vector3 &end);

	// Does test intersect us in any way?
	inline bool Intersects(const AABB &test) const;

	// Is test contained by us?
	inline bool Contains(const AABB &test) const;

	// Does this triangle touch us?
	inline bool TriangleTouches(const Vector3 &i_normal,
											const Vector3 &i_v0,
											const Vector3 &i_v1,
											const Vector3 &i_v2) const;

	// Turn a face id into a normal
	inline static void GetFaceNormal(uint face, Vector3 &normal);
	inline static Vector3 GetFaceNormal(uint face);

	// Allow copies
	inline AABB &operator=(const AABB &aabb);

	// Allow union
	inline AABB &operator+=(const AABB &aabb);
	inline AABB operator+(const AABB &aabb);

	// Allow intersection
	inline AABB &operator-=(const AABB &aabb);
	inline AABB operator-(const AABB &aabb);

	// the min/max data
	Vector3Packed		minV;
	Vector3Packed		maxV;

#ifdef GCN
	inline void EndianSwap()
	{
		minV.EndianSwap();
		maxV.EndianSwap();
	}
#endif

private:

	// stuff for ray/plane testing
	enum EPlaneAxis
	{
		PLANE_AXIS_X = 0,
		PLANE_AXIS_Y = 2,
		PLANE_AXIS_Z = 4
	};
	static inline bool FindPlanePenetration(float i_negOrigin, float i_basis,
			float i_face, EPlaneAxis i_axis, uint &io_entryFace,
			float &io_entryPoint, uint &io_exitFace, float &io_exitPoint);
};

// Make the c struct into the class
inline AABB &ToAABB(ts_BoundingBox &bb);

// Transform the box
inline AABB operator*(AABB &b, DirCos3x3 &m);
inline AABB operator*(AABB &b, Cartesian4x4 &m);

// Transform my the transpose
inline AABB operator/(AABB &b, DirCos3x3 &m);

// Include the implementation
#include "collide/aabb.hpp"

#endif
