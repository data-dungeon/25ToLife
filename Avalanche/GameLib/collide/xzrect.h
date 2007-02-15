/*************************************************************************/
// A simple Rect class that does everything
// + has been overloaded to be union
// - has been overloaded to be intersection
/*************************************************************************/
#ifndef XZRECT_H
#define XZRECT_H

#include "Math/Matrix.h"
#include "linearAlgebra/semiseg.h"
#include "enginehelper/drawutil.h"

#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif

class AABB;

class XZRect
{
public:
	// The constructors/destructor
	XZRect(void) {}
	XZRect(float minx, float minz, float maxx, float maxz);
	XZRect(const AABB &from);
	XZRect(Vector3CRef i_start, Vector3CRef i_chord);

	// Get at the coordinates
	float MinX(void) const
		{ return(minx); }
	float MinZ(void) const
		{ return(minz); }
	float MaxX(void) const
		{ return(maxx); }
	float MaxZ(void) const
		{ return(maxz); }

	// Special quantities
	float Width(void) const
		{ return(maxx - minx); }
	float Height(void) const
		{ return(maxz - minz); }
	float Area(void) const
		{ return(Width() * Height()); }
	float CenterX(void) const
		{ return((minx + maxx) * 0.5f); }
	float CenterZ(void) const
		{ return((minz + maxz) * 0.5f); }

	// Setting
	void MinX(float newMinX)
		{ minx = newMinX; }
	void MinZ(float newMinZ)
		{ minz = newMinZ; }
	void MaxX(float newMaxX)
		{ maxx = newMaxX; }
	void MaxZ(float newMaxZ)
		{ maxz = newMaxZ; }

	inline void Set(float newMinX, float newMinZ, float newMaxX, float newMaxZ);
	void Clear(void)
		{ minx = 0.0f; minz = 0.0f; maxx = 0.0f; maxz = 0.0f; }

	void SetMin(float newMinZ, float newMinX)
		{ minx = newMinX; minz = newMinZ; }
	void SetMax(float newMaxZ, float newMaxX)
		{ maxx = newMaxX; maxz = newMaxZ; }

	// Build from chord
	inline void Set(Vector3CRef i_start, Vector3CRef i_chord);

	// Build from AABB
	void Set(const AABB &i_aabb);

	// Sepecial stuff
	inline void Normalize(void);
	inline void Inflate(float x, float z);

	// Make the rect square
	void MakeSquare(void);

	// Inflate the XZRect so it contains the point
	void InFlateForPoint(float x, float z);

	// Comparisions & tests
	bool IsEmpty(void)
		{ return(minx == maxx && minz == maxz); }
	bool IsNormal(void)
		{ return(minx <= maxx && minz <= maxz); }
	inline bool Contains(const XZRect &r) const;
	inline bool Inside(float x, float z) const;
	inline bool Intersects(const XZRect &r) const;
	inline bool Crosses(Vector3CRef o, Vector3CRef b, float &enter, float &exit) const;
	inline bool Touches(const SemiSegment &s) const;

	// Intersects just in x or z
	inline bool IntersectsX(const XZRect &r) const;
	inline bool IntersectsZ(const XZRect &r) const;

	// Clipping
	inline void Clip(const XZRect &clipBy);

	// debugging
	void Draw(DrawUtility::COLOR color, float y = 0.0f);

	// Overloaded operators
	inline int operator==(const XZRect &r);
	inline int operator!=(const XZRect &r);
	inline XZRect &operator=(const XZRect &r);

	// Find the union
	inline XZRect &operator+=(const XZRect &r);
	inline XZRect operator+(const XZRect &r);

	// Find the intersection
	inline XZRect &operator-=(const XZRect &r);
	inline XZRect operator-(const XZRect &r);

#ifdef GCN
	inline void EndianSwap( void)
	{
		ByteSwap( minx);
		ByteSwap( minz);
		ByteSwap( maxx);
		ByteSwap( maxz);
	}
#endif

private:
	// The rect coordinates (names don't really hold unless normalized)
	float   minx;
	float   minz;
	float   maxx;
	float   maxz;

	// Swap two floats
	static inline void Swap(float &a, float &b)
		{ float t; t = b; b = a; a = t; }

	// Sort a and b into min/max
	static inline void Sort(float a, float b, float &smaller, float &bigger);
};

// inline functions must be defined in the header!
#include "collide/xzrect.hpp"

#endif
