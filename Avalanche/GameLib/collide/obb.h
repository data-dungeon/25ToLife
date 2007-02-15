/***************************************************************************/
// An OBB is an object oriented bounding box. An OBB is an AxisBBox (AABB)
// with a local transformation matrix.
//
// OBB's have 8 vertices, and these vertices are identified in the following
// way:
//
// Vertex			Coordinate
// ------         ----------
//   0				minx, miny, minz
//   1				maxx, miny, minz
//   2				minx, miny, maxz
//   3				maxx, miny, maxz
//   4            minx, maxy, minz
//   5            maxx, maxy, minz
//   6            minx, maxy, maxz
//   7            maxx, maxy, maxz
//
// Edges are defined as the following:
//
// Edge				Vertices
// ----           --------
//   0				0, 1
//   1				1, 3
//   2            3, 2
//   3				2, 0
//   4				5, 4
//   5				4, 6
//   6           	6, 7
//   7            7, 5
//   8				0, 4
//   9            1, 5
//   10           2, 6
//   11				3, 7
//
// And finally the faces appear:
//
//	Face				Vertices    	Edges       		Normal
// ---- 				--------  		-----             ------
//   0				0, 2, 6, 4		-3, 10, -5. -8		  -x
//   1				1, 5, 7, 3		9, -7, -11, -1		  +x
//   2				0, 1, 3. 2		0, 1, 2, 3			  -y
//   3				4, 6, 7, 5		5, 6, 7, 4			  +y
//   4				0, 4, 5, 1		8, -4, -9, -0		  -z
//   5				2, 3, 7, 6		-2, 11, -6, -10	  +z
//
// OBB's have a a parent matrix, which typically points to the parent's
// instance matrix.
/***************************************************************************/

#ifndef OBB_H
#define OBB_H

#include "collide/Object.h"
#include "collide/aabb.h"
#include "EngineHelper/drawutil.h"

#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif

class OBB : public CCollisionObject
{
public:
	// Given a matrix that may have scaling and a model AABB that may not be
	// "symmetric" return a proper dirCos matrix, center and trueAABB
	// that can be used to create an OBB
	// Note: This function will be pretty fast, but not super fast.
	static void ComputeBasis(const Graphics4x4 &m, const AABB &modelAABB,
									DirCos3x3 &dirCos, Vector3 &center, AABB &trueAABB);

	// Nothing to see here, move along
	OBB() : CCollisionObject(OBB_OBJECT) {}
	OBB(const AABB *modelAABB);

	// Set the sphere and box pointers (this copies now)
	// the AABB MUST BE SYMMETTRIC!!! i.e. abs(min.x) == abs(max.x) ...
	inline void SetBoundingVolume(const AABB *modelAABB,
			bool forceSymmetric = false);

	// Normalize (Note parent & AABB must be set!!!!)
	// Make the parent orientation all unit vectors and adjusts aabb to
	// compensate for scaling
	// Returns true if the obb had to be normalized
	bool Normalize(Vector3 *scale = NULL);

	// Modify the aabb (make sure you call UpdateRadius after you monkey with it)
	inline AABB &GetAABB()
		{ return(aabb); }

	// If you monkey with the AABB, call this!
	inline void UpdateRadius();

	// Look at the AABB
	inline const AABB &GetConstAABB() const
		{ return(aabb); }

	// Access the other juicy bits
	inline const DirCos3x3 *Matrix() const
		{ return(parent); }
	inline const Vector3Packed *Center(void) const
		{ return(pos); }
	inline float Radius() const
		{ return(radius); }

	// What is our volume?
	inline float Volume() const
		{ return aabb.Volume(); }

	inline void ApplyTranslation(Vector3CRef t);

	// Is this OBB totally inside of me?
	inline bool TotallyInside(OBB &test);

	// Get a vertex of the OBB
	inline Vector3 GetVertex(uint index) const;

	// Get the vertex indices for an edge
	inline static void GetEdge(uint edge, uint &vec1, uint &vec2);

	// Get the vertex indices for a face CCW order
	inline static void GetFace(uint face, uint &vec1, uint &vec2, uint &vec3, uint &vec4);

	// Get the vertex indices for a face CCW order
	inline void GetFace(uint face, uint &v1, uint &v2, uint &v3, uint &v4, Vector3 &normal) const;

	// Get a normal
	inline void GetFaceNormal(uint face, Vector3 &normal) const;
	inline Vector3 GetFaceNormal(uint face) const;

	// Requires an 8 entry vertex array
	inline void Transform(const DirCos3x3 &m, Vector3CRef pos, Vector3 *vList) const;
	inline void Transform(const Cartesian4x4 &m, Vector3 *vList) const;
	inline void Transform(Vector3 *vList) const
		{ Transform(*parent, *pos, vList); }

	// Are these OBB's equal?
	int operator==(const OBB &obb);

	// Find world extents.
	inline void ComputeWorldExtents(XZRect &o_worldRect) const;
	inline void ComputeWorldExtents(AABB &o_worldAABB) const;

	inline void ComputeWorldCoords( Vector3 &o_center, Vector3 &x, Vector3 &y, Vector3 &z );

	// Find intersections of a line with the OBB.
	bool IntersectLineInWorld(Vector3CRef i_originInWorld,
			Vector3CRef i_basisInWorld, uint &o_entryFace, float &o_entryPoint,
			uint &o_exitFace, float &o_exitPoint) const;

	// Find distances between a plane and OBB corners.
	void ComputeWorldPlaneDistance(Vector3CRef i_planePointInWorld,
			Vector3CRef i_planeNormalInWorld, float &o_frontDistance,
			float &o_backDistance) const;
	void ComputeOBBPlaneDistance(Vector3CRef i_planePointInOBB,
			Vector3CRef i_planeNormalInOBB, float &o_frontDistance,
			float &o_backDistance) const;

	// Find furthest corners from a plane.
	Vector3 GetOBBFrontCorner(Vector3CRef i_planeNormalInOBB);
	Vector3 GetOBBBackCorner(Vector3CRef i_planeNormalInOBB);
	int GetOBBFrontCornerIndex( Vector3CRef i_planeNormalInOBB );
	Vector3 GetWorldFrontCorner(Vector3CRef i_planeNormalInWorld);
	Vector3 GetWorldBackCorner(Vector3CRef i_planeNormalInWorld);
	int GetWorldFrontCornerIndex( Vector3CRef i_planeNormalInWorld );

	// return the leading faces, edges, and corners, given a leading corner
	static void GetLeadingFaces( int leadingCorner, int &face1, int &face2, int &face3 );
	static void GetLeadingEdges( int leadingCorner, int &edge1, int &edge2, int &edge3 );
	static void GetLeadingCorners( int leadingCorner, int &corner1, int &corner2, int &corner3 );
	static void GetAdjacentFaces( int edge, int &face1, int &face2 );

	// Plane intersections.
	enum EPlaneIntersectPath
	{
		PATH_XYZ = 0,
		PATH_YXZ,
		PATH_YZX,
		PATH_ZYX,
		PATH_ZXY,
		PATH_XZY,
		NUM_PATH
	};
	static const uint8 MASK_XYZ;
	static const uint8 MASK_YXZ;
	static const uint8 MASK_YZX;
	static const uint8 MASK_ZYX;
	static const uint8 MASK_ZXY;
	static const uint8 MASK_XZY;
	struct SObbPlaneIntersections
	{
		Vector3 d_pointInObb[NUM_PATH];
		uint8 d_intersects;
		uint8 d_master;
	};
	bool ComputeWorldPlaneIntersections(Vector3CRef i_planePointInWorld,
			Vector3CRef i_planeNormalInWorld,
			SObbPlaneIntersections &o_intersections) const;
	bool ComputeObbPlaneIntersections(Vector3CRef i_planePointInOBB,
			Vector3CRef i_planeNormalInOBB,
			SObbPlaneIntersections &o_intersections) const;

	// Triangle intersections
	bool TestTriangle(Vector3CRef i_normal, Vector3CRef i_v0,
			Vector3CRef i_v1, Vector3CRef i_v2, Vector3 &o_normalInObb,
			Vector3 &o_v0InObb, Vector3 &o_v1InObb, Vector3 &o_v2InObb) const;

	// Is this point in the box?
	bool PointInBox(Vector3CRef test);

	// Test if in AABB.
	bool TestAABB(Vector3 &i_center, Vector3 &i_corner, float i_radiusSquared);

	// Debugging
#ifndef CONSUMER_BUILD
	void QueueDraw(DrawUtility::COLOR color = DrawUtility::RED, float lifetime = 0.0f) const;
	void Draw(DrawUtility::COLOR color = DrawUtility::RED) const;
	void Draw(Cartesian4x4 &m, DrawUtility::COLOR color) const;
#endif

#ifdef GCN
	inline void EndianSwap()
	{
		CCollisionObject::EndianSwap();
		aabb.EndianSwap();
		ByteSwap(radius);
	}
#endif

protected:
	// We are fundamentally constructed from an AABB
	AABB				aabb;
	float				radius;

	// Point into the parents stuff
	DirCos3x3		*parent;
	Vector3Packed	*pos;

	// Stuff for accessing the obb
	static uint vertexIndex[8][3];
	static uint edgeVertexIndex[12][2];
	static uint faceVertexIndex[6][4];

private:
	bool FindPlanePenetration(float i_negOrigin, float i_basis, float i_face,
			uint i_axis, uint &io_entryFace, float &io_entryPoint,
			uint &io_exitFace, float &io_exitPoint) const;
	void PlaneIntersectionX(SObbPlaneIntersections &io_intersections,
			enum EPlaneIntersectPath i_path, uint8 i_mask,
			Vector3CRef i_planeNormalInObb, float i_planeDistance,
			float i_lengthY, float i_lengthZ, float i_extentY,
			float i_extentZ) const;
	void PlaneIntersectionY(SObbPlaneIntersections &io_intersections,
			enum EPlaneIntersectPath i_path, uint8 i_mask,
			Vector3CRef i_planeNormalInObb, float i_planeDistance,
			float i_lengthX, float i_lengthZ, float i_extentX,
			float i_extentZ) const;
	void PlaneIntersectionZ(SObbPlaneIntersections &io_intersections,
			enum EPlaneIntersectPath i_path, uint8 i_mask,
			Vector3CRef i_planeNormalInObb, float i_planeDistance,
			float i_lengthX, float i_lengthY, float i_extentX,
			float i_extentY) const;

	// Compute the radius
	inline static float ComputeRadius(const AABB &aabb);
};

// An interface for setting up the pointers to an OBB
class OBBReference : public OBB
{
public:
	// Setup the obb (pos/o can be null on construction)
	OBBReference(DirCos3x3 *orientation, Vector3Packed *position,
			AABB *modelAABB = NULL);

	OBBReference(DirCos3x3 *orientation, Vector3Padded *position,
			AABB *modelAABB = NULL);

	// Set the pointer to the parent and position (CANNOT BE NULL!!!)
	// Note, these are stored as pointers in the class
	inline void SetTransform(DirCos3x3 *orientation, Vector3Packed *position);
	inline void SetTransform(DirCos3x3 *orientation, Vector3Padded *position);
};

// This OBB stores the matrix/position data locally
class OBBInstance : public OBB
{
public:
	inline OBBInstance()
		{ parent = &orientation; pos = &position; }
	inline OBBInstance(const AABB *modelAABB) : OBB(modelAABB)
		{ parent = &orientation; pos = &position; }

	// Setup the position/orientation from the passed values
	// These are stored locally
	inline void SetTransform(const Cartesian4x4 &m);
	inline void SetTransform(const DirCos3x3 &o, Vector3CRef p);
	inline void SetOffsetTransform(const Cartesian4x4 &m, Vector3CRef origin);
	inline void SetOffsetTransform(const DirCos3x3 &o, Vector3CRef p,
			Vector3CRef origin);

	inline OBBInstance &operator=(const OBB &o);
	inline OBBInstance &operator=(const OBBReference &o);
	inline OBBInstance &operator=(const OBBInstance &o);

	// Expose data.
	inline DirCos3x3& Orientation()
		{ return(orientation); }
	inline Vector3Packed& Position(void)
		{ return(position); }

	// align base class to point to the data (call this after moving the data around)
	inline void AlignBase()
		{ parent = &orientation; pos = &position; }

#ifdef GCN
	inline void EndianSwap()
	{
		position.EndianSwap();
		orientation.EndianSwap();
		OBB::EndianSwap();
	}
#endif

protected:
	DirCos3x3		orientation;
	Vector3Packed	position;
};

class OBBStaticInstance : public OBBInstance
{
public:
	inline OBBStaticInstance() {}

	// Fixup (for when loaded of disk)
	inline void Fixup()
	{
#ifdef GCN
		ByteSwap(color);
		OBBInstance::EndianSwap();
#endif
		parent = &orientation;
		pos = &position;
	}

	// Access the color
	inline u16 Color()
		{ return(color); }
	inline void SetColor(u16 newColor);

	// Access the material
	inline u8 Material()
		{ return(material); }
	inline void SetMaterial(u8 newMaterial);

private:
	// 5-6-5, lighting color associated with this obb ("top," standable surface)
	u16		color;
	u8			material;
};

// Include the implementation
#include "collide/obb.hpp"

#endif
