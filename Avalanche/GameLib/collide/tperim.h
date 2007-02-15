//***************************************************************************
// Triangle perimeter information
//***************************************************************************
#ifndef TPERIM_H
#define TPERIM_H

#ifdef _DEBUG
#include <stdio.h>
#endif

#include "Math/Vector.h"
#include "container/u24.h"

// Forwards
class SectorizerTrianglePerimeter;

class TrianglePerimeter
{
public:
	TrianglePerimeter(void) {}

	// The vertex index pointer
	inline u24 *VertexIndex(void)
		{ return((u24 *)((uint)this + sizeof(TrianglePerimeter))); }

	// These are all public!
	u8    ab, bc, ca, pad;

	static const u24 BOUNDARY_INDEX;

	friend class SectorizerTrianglePerimeter;

#ifdef _DEBUG
	bool Dump(FILE *fh);
#endif
};

#endif
