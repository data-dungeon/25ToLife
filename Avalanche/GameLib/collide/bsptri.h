/***************************************************************************/
// BSP triangle definition
/***************************************************************************/
#ifndef BSPTRI_H
#define BSPTRI_H

#include "platform/BaseType.h"
#include "Math/Vector.h"

#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif

#pragma pack(1)
class BSPTriangle
{
public:
	// This can do no work
	BSPTriangle() {}

	// Return the vertex data
	inline Vector3Packed &V1(Vector3Packed *vertexTable)
		{ return(vertexTable[(u32)v1]); }
	inline Vector3Packed &V2(Vector3Packed *vertexTable)
		{ return(vertexTable[(u32)v2]); }
	inline Vector3Packed &V3(Vector3Packed *vertexTable)
		{ return(vertexTable[(u32)v3]); }
	inline u32 V1()
		{ return((u32)v1); }
	inline u32 V2()
		{ return((u32)v2); }
	inline u32 V3()
		{ return((u32)v3); }

	// Return the normal
	inline Vector3Packed &FaceNormal()
		{ return(faceNormal); }

	// Return the color
	inline u16 Color()
		{ return(color); }

	// Return the material
	inline u8 Material()
		{ return(material); }

	// These are used by the converter
	inline void SetVertices(uint vertex1, uint vertex2, uint vertex3)
		{ v1 = vertex1; v2 = vertex2; v3 = vertex3; }
	inline void SetFaceNormal(Vector3CRef normal)
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
	inline MAJORPLANE MajorPlane()
		{ return((MAJORPLANE)plane); }
	inline void SetMajorPlane(MAJORPLANE majorPlane)
		{ plane = (u8)majorPlane; }

	// Are the two triangles equal?
	inline int operator==(const BSPTriangle &t) const
		{ return(v1 == t.v1 && v2 == t.v2 && v3 == t.v3 && faceNormal == t.faceNormal && material == t.material); }

#ifdef GCN
	inline void EndianSwap()
	{
		faceNormal.EndianSwap();
		ByteSwap(color);
		ByteSwap(v1);
		ByteSwap(v2);
		ByteSwap(v3);
	}
#endif

private:
	Vector3Packed		faceNormal;
	u16			color;						// 5-6-5, lighting color associated with this triangle
	u8				material, plane;
	u32 			v1, v2, v3;
};
#pragma pack()

#endif
