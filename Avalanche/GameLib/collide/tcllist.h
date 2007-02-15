///////////////////////////////////////////////////////////////////////////////
// Terrain collide databases are now packed together by layers
///////////////////////////////////////////////////////////////////////////////
#ifndef TCLAYERLIST_H
#define TCLAYERLIST_H

#include "collide/tclayer.h"
#include "collide/octtree.h"
#include "collide/Stack.h"

class TerrainCollideLayerList
{
public:
	// Clean up stuff
	~TerrainCollideLayerList();

	// Load in a full TerrainCollide and all the TerrainSectors from disk
	static TerrainCollideLayerList *Create(t_Handle dblHandle, unsigned int &layerMask);
	static void FixDiskLoad(void* data);

	// What the hell does this do?
	void Validate(TriangleSuspect::STriangle *suspectList, u32 &numSuspects, u32 maxSuspects);

private:
	// Construct us!
	TerrainCollideLayerList(unsigned int &mask);
	TerrainCollideLayerList(unsigned int &mask, CDBHeader &hdr, void *data, t_Handle dblHandle);

	// The current mask for testing
	unsigned int			&m_layerMask;

	// An array of layer's and their mask's
	// This array is not SPARSE, it is packed
	TerrainCollideLayer	m_layer[MAX_COLLIDE_LAYERS];
	int						m_layers;

public:
	TerrainCollideLayerList *prev, *next;

	friend class CTerrainLayerAABBIterator;
	friend class CTerrainLayerOBBIterator;
	friend class CTerrainLayerLineIterator;
};

///////////////////////// CTerrainLayerAABBIterator /////////////////////////
class CTerrainLayerAABBIterator
{
public:
	inline CTerrainLayerAABBIterator(CCollisionStack *i_stack);

	inline AABB &Bounds();

	inline bool Initialize(TerrainCollideLayerList &i_list);

	inline void Terminate() {d_iterator.Terminate();}

	inline TerrainTriangle *GetNextTriangle();

private:
	TerrainCollideLayerList			*d_list;
	CCollisionStack					*d_stack;
	int 									d_layerIndex;
	OctTreeAABBIterator 				d_iterator;
};

///////////////////////// CTerrainLayerOBBIterator /////////////////////////
class CTerrainLayerOBBIterator
{
public:
	inline CTerrainLayerOBBIterator(CCollisionStack *i_stack);

	inline OBB &Bounds()	{ return d_obb; }

	inline bool Initialize(TerrainCollideLayerList &i_list);

	inline void Terminate() {d_iterator.Terminate();}

	inline TerrainTriangle *GetNextTriangle();

private:
	TerrainCollideLayerList			*d_list;
	CCollisionStack					*d_stack;
	int 									d_layerIndex;
	OBB									d_obb;
	AABB									d_aabb;
	Vector3								d_corner[8];
	OctTreeOBBIterator 				d_iterator;
};

///////////////////////// CTerrainLayerLineIterator /////////////////////////
class CTerrainLayerLineIterator
{
public:
	inline CTerrainLayerLineIterator(CCollisionStack *i_stack);

	inline void Setup(const Vector3 &i_start, const Vector3 &i_end);

	inline bool Initialize(TerrainCollideLayerList &i_list);

	inline void Terminate() {d_iterator.Terminate();}

	inline void FoundEnd(const Vector3 &i_intersection);

	inline TerrainTriangle *GetNextTriangle();

private:
	CCollisionStack					*d_stack;
	TerrainCollideLayerList			*d_list;
	int 									d_layerIndex;
	OctTreeLineIterator 				d_iterator;
};

#include "collide/tcllist.hpp"

#endif
