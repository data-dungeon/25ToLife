/****************6***********************************************************/
// A multi-resolution map node.
/***************************************************************************/
#ifndef MRMNODE_H
#define MRMNODE_H

#include "collide/quadtree.h"
#include "container/dllist.h"
#include "container/pool.h"

// Forward
template <class T> class MRMTreeLeaf;

// This is what we hold in the tree leaves
template <class T> class MRMEntity
{
public:
	// Nothing to see here
	inline MRMEntity(void) {}

	XZRect				size;          	// Size of this entity
	MRMTreeLeaf<T>		*leaf;				// Leaf we reside in
  	T						*entity;				// The actual entity
	MRMEntity<T>		*prev, *next;		// We exist in a list
};

// This is what a leaf in the tree looks like
template <class T> class MRMTreeLeaf
{
public:
	// Everything is public here
	inline MRMTreeLeaf(void);

	// Convenient
	typedef MRMEntity<T>		Entity;
	typedef MRMTreeLeaf<T>	Leaf;

	// Set the size of this
	inline void Setup(const XZRect &leafSize)
		{ size = leafSize; child[0] = child[1] = child[2] = child[3] = NULL; }

	// What is our size
	inline const XZRect &Size(void)
		{ return(size); }

	// Does this object fit in this node?
	inline bool Fits(Entity *entity)
		{ return(size.Contains(entity->size)); }

	// Add an entity (calls children)
	inline bool Add(Pool<Leaf> &leafPool, float smallestArea, Entity *entity);

	// Unlink this entity from this leaf
	inline void Unlink(Entity *entity)
		{ ASSERT(entity->leaf == this); entityList.Unlink(entity); entity->leaf = NULL; }

	// List of entities
	inline Entity *EntityHead(void)
		{ return(entityList.Head()); }

	// Update everyone in this node
	inline void Update(Pool<Leaf> &leafPool, float smallestArea, Leaf *parent = NULL);

	// Debugging validation & stats
	#define MAX_MRM_STATS_LEVELS	64
	struct Stats
	{
		int maxDepth;
		int totalLeaves;
		int totalEntities;
		int entitiesAtLevel[MAX_MRM_STATS_LEVELS];
	};
	inline void static ClearStats(Stats &stats);
	inline void Validate(const XZRect &expectedSize, float smallestArea, Stats *stats = NULL, int level = 0);

	// Return an array of T *'s
	// This data is ONLY VALID until another MRM call is made
	inline void Touches(DynamicArray<T *> &touches, const XZRect &test);
	inline void Touches(DynamicArray<T *> &touches, const SemiSegment &test);

private:
	// The size of this node
	XZRect							size;

	// List of elements in this node
	DoublyLinkedList<Entity>	entityList;

	// Any or all can be NULL
	MRMTreeLeaf<T>					*child[QUADTREECHILDREN];

	// What would the rect of the child be?
	inline static XZRect ChildRect(const XZRect &parent, QUADTREECHILD which);

	// What is the best quadrant for this size
	inline QUADTREECHILD BestQuadrant(const XZRect &testRect);

	// Create a child, return NULL if too small
	inline static Leaf *MakeChild(Pool<Leaf> &leafPool, float smallestArea, const XZRect &childRect);

	// If this child is empty and has no children, delete it (not recursive)
	inline static void CheckEmptyChild(Pool<Leaf> &leafPool, Leaf *&child);

	// Push this entity up if we can
	inline bool PushUp(Leaf *parent, Entity *entity);

	// Push it down if we can
	inline void PushDown(Pool<Leaf> &leafPool, float smallestArea, Entity *entity);
};

// Include the implementation
#include "collide/mrmnode.hpp"

#endif
