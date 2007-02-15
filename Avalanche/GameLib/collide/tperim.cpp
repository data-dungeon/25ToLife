//***************************************************************************
//***************************************************************************

/******************* includes ****************************/

#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/tperim.h"

const u24 TrianglePerimeter::BOUNDARY_INDEX = 0x00ffffff;

#ifdef _DEBUG
bool TrianglePerimeter::Dump(
FILE *fh)
{
	fprintf(fh, "verts: %d\n", (int)ca + 1);
	fprintf(fh, "ab, bc, ca: %d %d %d\n", (int)ab, (int)bc, (int)ca);
	u24 *v = VertexIndex();
	for (uint i = 0; i <= ca; i++)
		fprintf(fh, "%d = %d ", i, (u32)v[i]);
	fprintf(fh, "\n");

	return(true);
}
#endif
