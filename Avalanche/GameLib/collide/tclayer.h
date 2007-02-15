///////////////////////////////////////////////////////////////////////////////
// Terrain collide databases are now packed together by layers
///////////////////////////////////////////////////////////////////////////////
#ifndef TCLAYER_H
#define TCLAYER_H

#include "collide/cdbhdr.h"
#include "collide/otnode.h"
#include "collide/ottestid.h"
#include "collide/tsuspect.h"

// Each layer has a bunch of information...
class TerrainCollideLayer
{
public:
	TerrainCollideLayer();

	// Initialize off disk
	void Initialize(unsigned int mask, void *data);
	static void FixDiskLoad(void* data);
	
	// Terminate 
	void Terminate();

	// What is this layer's mask
	inline unsigned int Mask();
	inline bool MatchMask(unsigned int mask);

	// Get at the bounding info
	inline const AABB &Bounds();

	// Get at the triangle list
	inline int Triangles();
	inline TerrainTriangle *TriangleList();

	// Get at the oct tree info
	inline const Octant &RootOctant();
	inline OctTreeNode *Root();

	// Access the test ID
	inline OctTreeTestID &TestID();
	inline u32 NewTestID();
	inline u32 CurrentTestID();

	// What is the leaf depth of this tree?
	inline uint LeafDepth();

	// Get at the validation stuff
	inline TriangleSuspect &Suspect();

private:
	// The mask
	unsigned int		m_mask;

	// Pointer to the header info
	OctTreeHeader		*m_hdr;

#ifdef TERRAINTRIANGLE_LEAN
	// The vertex array
	Vector3Packed		*m_vertex;
#endif

	// Triangle list
	TerrainTriangle	*m_triangle;

	// The root
	OctTreeNode			*m_root;

	// Our test ID
	OctTreeTestID		m_testID;

	// Suspect list
	TriangleSuspect	m_suspect;
};

#include "collide/tclayer.hpp"

#endif
