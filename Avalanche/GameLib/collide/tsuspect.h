/***************************************************************************/
// Triangle suspect support
/***************************************************************************/
#ifndef TSUSPECT_H
#define TSUSPECT_H

#include "collide/terraintri.h"
#include "collide/otnode.h"

#define SUSPECT_RIGHT_HANDED 0x01
#define SUSPECT_UNIT_NORMAL  0x02
#define SUSPECT_CORNER1      0x04
#define SUSPECT_MULTI1       0x08
#define SUSPECT_CORNER2      0x10
#define SUSPECT_MULTI2       0x20
#define SUSPECT_CORNER3      0x40
#define SUSPECT_MULTI3       0x80

#define SUSPECT_CORNER       0x1
#define SUSPECT_MULTI        0x2
#define NEIGHBOR1_SHIFT        2
#define NEIGHBOR2_SHIFT        4
#define NEIGHBOR3_SHIFT        6

class TriangleSuspect
{
public:
	TriangleSuspect();

	// Setup from the tree
	void Initialize(TerrainTriangle *i_triList, int i_triangles);

	struct STriangle
	{
		TerrainTriangle* d_triangle;
		u32 d_reason;
	};

	void Validate(STriangle* o_suspectList, u32 &io_numSuspects, u32 i_maxSuspects);
	u32 TestNeighbor(u32 i_tester, const Vector3 &i_v1, const Vector3 &i_v2);

private:
	TerrainTriangle		*triangle;
	uint						triangles;

	// Two vertices equal?
	inline bool Equal(const Vector3 &a, const Vector3 &b)
		{ return Math::Zero((a - b).LengthSquared()); }
};

#endif
