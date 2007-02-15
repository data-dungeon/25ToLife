/***************************************************************************/
// Fast axis-aligned cube stuff
/***************************************************************************/
#ifndef CUBE_H
#define CUBE_H

#include "collide/aabb.h"
#include "collide/obb.h"
#include "linearAlgebra/semiseg.h"
#include "linearAlgebra/rayseg.h"

// For clipping lines to the cube
#define CUBE_LINE_EPSILON		(-0.01f)

class Cube
{
public:
	// Constructors
	inline Cube() {}
	inline Cube(const Vector3 &center, float halfSize);
	inline Cube(const AABB &aabb);

	// Clear this to an empty Cube
	inline void Clear();

	// Setup the cube
	inline void Set(const Vector3 &center, float halfSize);

	// Make this AABB into a cube using the largest dimension
	inline void Fit(const AABB &aabb);

	// Expand by a point
	inline void ExpandByPoint(const Vector3 &p);

	// Get at the center
	inline Vector3 Center() const;

	// Get at the extents
	inline Vector3 Max() const;
	inline Vector3 Min() const;

	// Get at the sizes
	inline float HalfSize() const;
	inline float Size() const;

	// The volume
	inline float Volume() const;

	// Inflate the Cube by the passed amount
	inline void Inflate(float inflate);

	// Point in the cube?
	inline bool Inside(const Vector3 &p) const;
	inline bool Inside(const Vector3 &p, float epsilon) const;

	// Point inside of face?
	inline bool InsideFaceXY(const Vector3 &p) const;
	inline bool InsideFaceXZ(const Vector3 &p) const;
	inline bool InsideFaceYZ(const Vector3 &p) const;

	// Is this AABB touching this cube in any way
	inline bool Touches(const AABB &aabb) const;

	// Is this AABB contianed by the cube?
	inline bool Contains(const AABB &aabb) const;

	// Is this OBB touching this cube in any way
	bool Touches(const OBB &obb) const;

	// Is this OBB contianed by the cube?
	inline bool Contains(const Vector3 *pList, int count) const;

	// Is this line touching this cube?
	inline bool Touches(const SemiSegment &s) const;
	inline bool Touches(const SemiSegment &s, float fEpsilon) const;

	// Is this line contained by the cube?
	inline bool Contains(const SemiSegment &s) const;

	// Does this triangle touch this cube?
	bool Touches(const Vector3 &normal, const Vector3 &a, const Vector3 &b, const Vector3 &c, float epsilon = 0.0f) const;

	// Does this triangle fit inside of this cube
	inline bool Contains(const Vector3 &a, const Vector3 &b, const Vector3 &c) const;
	inline bool Contains(const Vector3 &a, const Vector3 &b, const Vector3 &c, float epsilon) const;

	// Clip this segment to the cube, returns false if not in the cube
	bool Clip(const Vector3 &start, const Vector3 &end, Vector3 &clipStart, Vector3 &clipEnd) const;

	// The point at which the segment exits the plane
	float RayExit(Vector3CRef start, Vector3CRef end, float exitEpsilon = 0.0f) const;

	// Clip a line to the cube
	bool LineEnter(Vector3CRef start, Vector3CRef end,
							Vector3 &intersection, float epsilon = CUBE_LINE_EPSILON) const;
	bool LineExit(Vector3CRef start, Vector3CRef end,
							Vector3 &intersection, float epsilon = CUBE_LINE_EPSILON) const;

	// What face does this ray exit?
	enum Face
	{
		POSX = 0,
		POSY = 1,
		POSZ = 2,
		NEGX = 3,
		NEGY = 4,
		NEGZ = 5,

		FACEEXITS
	};
	Face RayFaceExit(Vector3CRef start, Vector3CRef end) const;

	// Convert to an AABB
	inline AABB ToAABB() const;

	// Allow copies
	inline Cube &operator=(const Cube &c);

#ifdef GCN
	inline void EndianSwap()
	{
		m_center.EndianSwap();
		ByteSwap(m_halfSize);
	}
#endif

protected:
	// The data
	Vector3Packed	m_center;
	float				m_halfSize;

	// Test stuff
	static bool TriangleTest(const Segment &s,	const Vector3 &a,	const Vector3 &b,	const Vector3 &c,	const Vector3 &normal);
	static bool QuadTest(const Segment &s, const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &d);

	// Separating axis test (returns true if it finds a seperating axis)
	bool FindSeparatingAxis(const Vector3 &normal,
									const Vector3 &a, const Vector3 &b, const Vector3 &c,
									float epsilon = 0.0f) const;
};


class Octant : public Cube
{
public:
	inline Octant();
	inline Octant(const Vector3 &center, float halfSize);
	inline Octant(const AABB &aabb);

	// Way of naming the 8 octants
	// Notice the bits are the negative signs
	enum OCTANT
	{
		XYZ = 0,			// 000
		_XYZ = 1,    	// 001
		X_YZ = 2,    	// 010
		_X_YZ = 3,    	// 011
		XY_Z = 4,  	  	// 100
		_XY_Z = 5,    	// 101
		X_Y_Z = 6,    	// 110
		_X_Y_Z = 7,    // 111
		OCTANTS
	};

	// Make the root octant from an AABB
	static Octant StandardRoot(const AABB &aabb);
	static Octant LooseRoot(const AABB &aabb);

	// Make the bounding box for this specified child
	inline Octant StandardChild(OCTANT octant) const;
	inline Octant LooseChild(OCTANT octant) const;

	// Return the bounds of the parent, give the octant ID
	inline Octant StandardParent(OCTANT myOctant) const;
	inline Octant LooseParent(OCTANT myOctant) const;

	// Nice calls that isolate out the fact if  we are loose or not
	static Octant Root(const AABB &aabb);
	inline Octant Child(OCTANT octant) const;
	inline Octant Parent(OCTANT myOctant) const;

	// What is the best octant for this point?
	inline OCTANT BestChild(const Vector3 &p) const;

	// What is the size of an octant n levels below myself
	inline float SizeAtDepth(int depth);

	// What is the center of the octant that contains the point
	inline Vector3 CenterAtDepth(int depth, Vector3CRef forPoint);

	// I hate you C++
	inline Octant &operator=(const Octant &o);

#ifdef GCN
	inline void EndianSwap()
		{ Cube::EndianSwap(); }
#endif

private:
	// Array of signs for the octant
	static const Vector3			s_sign[OCTANTS];
};

// Include the implementation
#include "collide/cube.hpp"

#endif
