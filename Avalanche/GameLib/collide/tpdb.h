//***************************************************************************
// The triangle perimeter database
//***************************************************************************
#ifndef TPDB_H
#define TPDB_H

#include "collide/tperim.h"

#define TRIANGLEPERIMETER_VERSION		0x00010000

class TrianglePerimeterDatabase
{
public:
	TrianglePerimeterDatabase(void) {}
	~TrianglePerimeterDatabase(void) {}

	// Load us
	static TrianglePerimeterDatabase *Create(t_Handle dblHandle);
	static void Destroy(TrianglePerimeterDatabase *killMe);

	// Get a triangles perimeter info
	inline TrianglePerimeter *Get(uint triangle)
		{ ASSERT(triangle < count); return(list[triangle]); }

	// Get the maximum verticies surrounding a triangle
	inline u32 GetMaxVerts(void) { return(maxVerts); }

#ifdef _DEBUG	
	bool Dump(char *filename);
#endif

private:
	// Version
	u32					version;

	// maximum verts
	u32					maxVerts;

	// The list of the data
	TrianglePerimeter	**list;
	u32					count;

	// fixup everything
	void DiskFixup(void);

	// Our friends
	friend class SectorizerTrianglePerimeterDatabase;
};

#endif
