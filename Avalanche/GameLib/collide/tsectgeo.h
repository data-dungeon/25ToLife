/***************************************************************************/
// Terrain geometry definitions
/***************************************************************************/
#ifndef TSECTGEO_H
#define TSECTGEO_H

#include "platform/BaseType.h"
#include "Math/Vector.h"
#include "container/u24.h"
#include "collide/tritest.h"

#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif

//#define USE_BARYCENTRIC
#error "You probably shouldn't be compiling this file..."
#if !defined(PS2) || !defined(SN)
#pragma pack(1)
#endif
class TerrainTriangle
{
public:
	// This can do no work
	TerrainTriangle(void) { testID = 0; }

	// Return the vertex data
	inline Vector3Packed &V1(Vector3Packed *vertexTable)
		{ return(vertexTable[(u32)v1]); }
	inline Vector3Packed &V2(Vector3Packed *vertexTable)
		{ return(vertexTable[(u32)v2]); }
	inline Vector3Packed &V3(Vector3Packed *vertexTable)
		{ return(vertexTable[(u32)v3]); }
	inline u32 V1(void)
		{ return((u32)v1); }
	inline u32 V2(void)
		{ return((u32)v2); }
	inline u32 V3(void)
		{ return((u32)v3); }

	// Return the normal
	inline Vector3Packed &FaceNormal(void)
		{ return(faceNormal); }

	// Return the color
	inline u16 Color(void)
		{ return(color); }

	// Return the material
	inline u8 Material(void)
		{ return(material); }

	// These are used by the converter
	inline void SetVertices(uint vertex1, uint vertex2, uint vertex3)
		{ v1 = vertex1; v2 = vertex2; v3 = vertex3; }
	inline void SetFaceNormal(Vector3PackedCRef normal)
		{ faceNormal = normal; }
	inline void SetColor(u16 colorValue)
		{ color = colorValue; }
	inline void SetMaterial(u8 materialValue)
		{ material = materialValue; }

	// What is the "major" plane of projection for this triangle
	typedef enum
	{
		XY = 0,
		XZ = 1,
		YZ = 2
	} MAJORPLANE;
	inline MAJORPLANE MajorPlane(void)
		{ return((MAJORPLANE)(plane & 0x3)); }
	inline void SetMajorPlane(MAJORPLANE majorPlane)
		{ plane = (u8)majorPlane; }

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
	inline MAJORAXIS MajorAxis(void)
		{ return((MAJORAXIS)plane); }
	inline void SetMajorAxis(MAJORAXIS majorAxis)
		{ plane = (u8)majorAxis; }

	// Find the major plane and axis given a normal
	inline static MAJORPLANE MajorPlane(Vector3 &normal);
	inline static MAJORAXIS MajorAxis(Vector3 &normal);

	// Access the test ID
	inline void ResetTestID(void)
		{ testID = 0; }
	inline void SetTestID(u32 newTestID)
		{ testID = newTestID; }
	inline bool MatchTestID(u32 test)
		{ return(testID == test); }
	inline u32 TestID(void)
		{ return(testID); }

	// Are the two triangles equal?
	inline int operator==(const TerrainTriangle &t) const
		{ return(v1 == t.v1 && v2 == t.v2 && v3 == t.v3 && faceNormal == t.faceNormal && material == t.material); }

	// Is a point in the triangle?
	inline bool PointInTriangle(Vector3CRef point, Vector3Packed *vertexTable) const;
#ifdef USE_BARYCENTRIC
	inline void SetupPointInTriangle(Vector3Packed *vertexTable, Vector3 &rightEdge,
			Vector3 &leftEdge, float &area) const;
	inline bool PointInTriangle(Vector3CRef point, Vector3Packed *vertexTable,
			Vector3 &rightEdge, Vector3 &leftEdge, float &area) const;
#endif

#ifdef GCN
	inline void EndianSwap( void)
	{
		ByteSwap( testID);
		faceNormal.EndianSwap( );
		ByteSwap( color);
		ByteSwap( v1);
		ByteSwap( v2);
		ByteSwap( v3);
	}
#endif

	// ned-- this structure originally used the "u24" type for v1, v2, and v3. before i added the color field, total
	// size was 27 bytes, with one pad byte (u32's must be 4-byte aligned on ps2). adding color made it 29 bytes, with
	// 3 pad bytes. it would be possible to combine the material and plane fields into a single byte (the plane field
	// just needs 2 bits). this would have knocked the size back down to 28 bytes, so we would be 4-byte aligned.
	// however, the u24 type needs to be assembled byte-by-byte on the ps2, and dave and i figured that we'd be better
	// off sacrificing data size for a potential speed increase by changing to u32's, since collision detection is one
	// of the major time eaters. also, a 32-byte record might be more data cache friendly. i haven't tried to measure
	// any performance difference from this change
private:
	u32			testID;
	Vector3Packed		faceNormal;
	u16			color;						// 5-6-5, lighting color associated with this triangle
	u8				material, plane;
	u32 			v1, v2, v3;
};
#if !defined(PS2) || !defined(SN)
#pragma pack()
#endif

// And edge is defined by two vertices
#if !defined(PS2) || !defined(SN)
#pragma pack(1)
#endif
class TerrainEdge
{
public:
	// This can do no work
	inline TerrainEdge(void) {}

	// Return the vertex data
	inline Vector3Packed &V1(Vector3Packed *vertexTable)
		{ return(vertexTable[(u32)v1]); }
	inline Vector3Packed &V2(Vector3Packed *vertexTable)
		{ return(vertexTable[(u32)v2]); }
	inline u32 V1(void)
		{ return((u32)v1); }
	inline u32 V2(void)
		{ return((u32)v2); }

	// These are used by the converter
	inline void SetVertices(uint vertex1, uint vertex2);

	inline int operator==(const TerrainEdge &e) const
		{ return(v1 == e.v1 && v2 == e.v2); }

#ifdef GCN
	inline void EndianSwap( void)
	{
		ByteSwap( v1);
		ByteSwap( v2);
	}
#endif
private:
	u32			v1, v2;
};
#if !defined(PS2) || !defined(SN)
#pragma pack()
#endif


/***************************************************************************/
// These are used by the converter
/***************************************************************************/
inline void TerrainEdge::SetVertices(
uint vertex1,
uint vertex2)
{
	if (vertex1 < vertex2)
	{
		v1 = vertex1;
		v2 = vertex2;
	}
	else
	{
		v1 = vertex2;
		v2 = vertex1;
	}
}

/***************************************************************************/
/***************************************************************************/
inline bool TerrainTriangle::PointInTriangle(
Vector3CRef point,
Vector3Packed *vertexTable) const
{
#ifndef USE_BARYCENTRIC
	switch ((MAJORPLANE)(plane & 0x3))
	{
	 case XY:
		return(TriangleTest::PointInTriangleXY(point, vertexTable[(u32)v1],
				vertexTable[(u32)v2], vertexTable[(u32)v3]));
	 case XZ:
		return(TriangleTest::PointInTriangleXZ(point, vertexTable[(u32)v1],
				vertexTable[(u32)v2], vertexTable[(u32)v3]));
	default:
		return(TriangleTest::PointInTriangleYZ(point, vertexTable[(u32)v1],
				vertexTable[(u32)v2], vertexTable[(u32)v3]));
	}
#else
	// This assumes that the point is in the triangle plane.
	Vector3 localPoint = point - vertexTable[v1];
	Vector3 rightEdge = vertexTable[v2] - vertexTable[v1];
	float rightArea = faceNormal.Dot(rightEdge.Cross(localPoint));
	if (rightArea < 0.0f)
		return(false);

	Vector3 leftEdge = vertexTable[v3] - vertexTable[v1];
	float leftArea = faceNormal.Dot(localPoint.Cross(leftEdge));
	return((leftArea >= 0.0f) &&
			((rightArea + leftArea) <= faceNormal.Dot(rightEdge.Cross(leftEdge))));
#endif
}

#ifdef USE_BARYCENTRIC

/***************************************************************************/
/***************************************************************************/
inline void TerrainTriangle::SetupPointInTriangle(
Vector3Packed *vertexTable,
Vector3 &rightEdge,
Vector3 &leftEdge,
float &area) const
{
	rightEdge = vertexTable[v2] - vertexTable[v1];
	leftEdge = vertexTable[v3] - vertexTable[v1];
	area = faceNormal.Dot(rightEdge.Cross(leftEdge));
}

/***************************************************************************/
/***************************************************************************/
inline bool TerrainTriangle::PointInTriangle(
Vector3CRef point,
Vector3Packed *vertexTable,
Vector3 &rightEdge,
Vector3 &leftEdge,
float &area) const
{
	// This assumes that the point is in the triangle plane.
	Vector3 localPoint = point - vertexTable[v1];
	float rightArea = faceNormal.Dot(rightEdge.Cross(localPoint));
	if (rightArea < 0.0f)
		return(false);

	float leftArea = faceNormal.Dot(localPoint.Cross(leftEdge));
	return((leftArea >= 0.0f) && ((rightArea + leftArea) <= area));
}

#endif

/***************************************************************************/
// What is the major plane for this normal?
/***************************************************************************/
inline TerrainTriangle::MAJORPLANE TerrainTriangle::MajorPlane(
Vector3 &normal)
{
	Vector3 absN = Vector3::Abs(normal);

	// Which plane is best to project into?
	if (absN.x() > absN.y())
	{
		if (absN.x() > absN.z())
   		return(YZ);
		return(XY);
	}
	else if (absN.y() > absN.z())
		return(XZ);
	return(XY);
}

/***************************************************************************/
// What is the major axis for this normal?
/***************************************************************************/
inline TerrainTriangle::MAJORAXIS TerrainTriangle::MajorAxis(
Vector3 &normal)
{
	Vector3 absN = Vector3::Abs(normal);

	// Which plane is best to project into?
	if (absN.x() > absN.y())
	{
		if (absN.x() > absN.z())
		{
			if (normal.x() >= 0.0f)
				return(POS_X);
			return(NEG_X);
		}
		else if (normal.z() >= 0.0f)
   		return(POS_Z);
		return(NEG_Z);
	}
	else if (absN.y() > absN.z())
	{
		if (normal.y() >= 0.0f)
   		return(POS_Y);
		return(NEG_Y);
	}
	else if (normal.z() >= 0.0f)
		return(POS_Z);
	return(NEG_Z);
}

#endif
