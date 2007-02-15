/***************************************************************************/
// Test ID's for oct trees
/***************************************************************************/
#ifndef OTTESTID_H
#define OTTESTID_H

#include "collide/terraintri.h"

class OctTreeTestID
{
public:
	// Doh
	inline OctTreeTestID();

	// Create us!
	inline void Initialize(TerrainTriangle *triList, int triangles);

	// Get a new ID
	inline u32 GetNewID();

	// Inspect the existing ID
	inline u32 CurrentID();

private:
	u32						m_testID;
	TerrainTriangle	 	*m_triangle;
	int						m_triangles;
};

#include "collide/ottestid.hpp"

#endif
