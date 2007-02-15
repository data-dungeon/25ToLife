/***************************************************************************/
// Terrain triangles
/***************************************************************************/
#ifndef OCTTREETRI_H
#define OCTTREETRI_H

#include "platform/BaseType.h"
#include "Math/Vector.h"
#include "collide/tritest.h"
#include "container/u24.h"

// Using barycentric intersection test
#define USE_BARYCENTRIC

// Define this for vertex table based terrain
#if defined(GCN) //|| defined(WIN32)
#define TERRAINTRIANGLE_LEAN
#endif

#pragma pack(1)
// The base triangle
class TerrainTriangleBase
{
public:
	// This can do no work
	TerrainTriangleBase() {}

	// Access the test ID
	inline void ResetTestID();
	inline void SetTestID(u32 newTestID);
	inline bool MatchTestID(u32 test) const;
	inline u32 TestID() const;

	// Return the color
	inline u16 Color() const;
	inline void SetColor(u16 color);

	// Return the material
	inline u8 Material() const;
	inline void SetMaterial(u8 material);

	// What is the "major" plane of projection for this triangle
	typedef enum
	{
		XY = 0,
		XZ = 1,
		YZ = 2
	} MAJORPLANE;
	inline MAJORPLANE MajorPlane() const;
	inline void ComputeMajorPlane(Vector3CRef normal);

	// What is the "major" axis of projection for this triangle
	typedef enum
	{
		POS_Z = 0,
		POS_Y = 1,
		POS_X = 2,
		NEG_Z = 4,
		NEG_Y = 5,
		NEG_X = 6
	} MAJORAXIS;
	inline MAJORAXIS MajorAxis() const;
	inline void ComputeMajorAxis(Vector3CRef normal);

	// Find the major plane and axis given a normal
	static MAJORPLANE MajorPlane(Vector3CRef normal);
	static MAJORAXIS MajorAxis(Vector3CRef normal);

	// Fixup from disk load
	inline void DiskFixup();

private:
	u32					m_testID;
	u16					m_color;						// 5-6-5, lighting color associated with this triangle
	u8						m_material;
	u8						m_plane;
};
#pragma pack()

#pragma pack(1)
// The fat triangle
class TerrainTriangleFat : public TerrainTriangleBase
{
public:
	// This can do no work
	TerrainTriangleFat() {}

	// Return the vertex data
	inline Vector3CRef A() const;
	inline Vector3CRef B() const;
	inline Vector3CRef C() const;
	inline void SetVertices(Vector3CRef a, Vector3CRef b, Vector3CRef c);

	// Return the normal
	inline Vector3CRef FaceNormal() const;
	inline void SetFaceNormal(Vector3CRef normal);

	// Is a point in the triangle?
	bool PointInTriangle(Vector3CRef point) const;

#ifdef USE_BARYCENTRIC
	inline void SetupPointInTriangle(Vector3 &rightEdge, Vector3 &leftEdge, float &area) const;
	inline bool PointInTriangle(Vector3CRef point, Vector3 &rightEdge, Vector3 &leftEdge, float &area) const;
#endif

	// Fixup from loading from disk
	inline void DiskFixup();

private:
	Vector3Packed		m_normal;
	Vector3Packed		m_a, m_b, m_c;
};
#pragma pack()

#pragma pack(1)
class TerrainTriangleLean : public TerrainTriangleBase
{
public:
	// This can do no work
	TerrainTriangleLean() {}

	// Return the vertex data
	inline Vector3CRef A() const;
	inline Vector3CRef B() const;
	inline Vector3CRef C() const;
	inline void SetVertices(uint a, uint b, uint c);

	// Return the normal
	inline Vector3CRef FaceNormal() const;
	inline void SetFaceNormal(Vector3CRef faceNormal);

	// Is a point in the triangle?
	bool PointInTriangle(Vector3CRef point) const;

#ifdef USE_BARYCENTRIC
	inline void SetupPointInTriangle(Vector3 &rightEdge, Vector3 &leftEdge, float &area) const;
	inline bool PointInTriangle(Vector3CRef point, Vector3 &rightEdge, Vector3 &leftEdge, float &area) const;
#endif

	// Fixup from loading from disk
	inline void DiskFixup(u8 vertexGroup);
	inline void SetVertexGroup(u8 vertexGroup);

	// Add/remove vertex groups
	static u8 AddVertexGroup(Vector3Packed *vertexGroup);
	static bool RemoveVertexGroup(Vector3Packed *vertexGroup);
	static bool RemoveVertexGroup(u8 vertexGroup);

private:
	Vector3Packed				m_normal;
	u32							m_a, m_b;
	u24							m_c;
	u8								m_vertexGroup;

	// Vertex group support
	static Vector3Packed		*s_vertexGroup[256];
};
#pragma pack()

// Here is where we trick the run-time code into using the right triangle
#ifdef TERRAINTRIANGLE_LEAN
#define TerrainTriangle TerrainTriangleLean
#else
#define TerrainTriangle TerrainTriangleFat
#endif

// Things come back from the OctTree in these nice little packets
class TerrainTriangleList
{
public:
	TerrainTriangle		**tri;
	int						count;
};

#include "collide/terraintri.hpp"

#endif
