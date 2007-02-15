/***************************************************************************/
// A simple implementation of a realtime multi-resolution map.
//
// This implementation uses an "expanded" quad tree.  The implementation
// is not really hidden from the user...as this is meant to be fast.
/***************************************************************************/
#ifndef MRM_H
#define MRM_H

// Define this for a "loose" quadtree
// Loose has proven in tests to be significantly better
#define MRM_LOOSEQUADTREE

// Define this for very anal verification asserts
//#define MRM_VERIFY

// Define this for test stats
//#define MRM_PROFILE

#include "collide/xzrect.h"
#include "collide/mrmnode.h"
#include "container/dynlist.h"
#include "container/pool.h"

template <class T> class MultiResolutionMap
{
public:
	// Create the map
	inline MultiResolutionMap(const XZRect &totalSize, float smallestLeaf = 4.0f);
	inline ~MultiResolutionMap(void) {}

	// Convenient types
	typedef MRMEntity<T>		Entity;
	typedef MRMTreeLeaf<T>  Leaf;

	// Add an entity to the tree (doesn't really get added until you
	// set size and call Activate)
	inline bool Add(T *entity);

	// Remove an entity from the tree (this is pretty slow, it has to search)
	inline bool Remove(T *entity);

	// Get at the top leaf
	inline Leaf &Top(void)
		{ return(top); }

	// Access a "flat" array of all the entities in the world
	inline DynList<Entity> &GetEntityList(void)
		{ return(entityList); }

	// Mark an entity as active or inactive
	inline void Activate(Entity *entity, bool active);

	// Update the contents of the map using sizes in the MRMEntity
	// (that you've updated manually)
	inline void Update(void);

	// Log some stats to debug output
	inline void ShowStats(void);

	// Return an array of T *'s
	// This data is ONLY VALID until another MRM call is made
	inline T **Touches(const XZRect &size, int &count);
	inline T **Touches(const Vector3 &start, const Vector3 &end, int &count);

#ifdef MRM_PROFILE
	inline int TotalChecks(void)
		{ return(totalChecks); }
	inline void ClearChecks(void)
		{ totalChecks = 0; }
#endif

private:
	// The root node always exists
	Leaf						top;

	// Our internal data storage
	Pool<Leaf>				leafPool;
	Pool<Entity>			entityPool;

	// A dynamic array of entities, for doing updates
	DynList<Entity>		entityList;

	// What is the smallest allowed area?
	float						smallestArea;

	// The "static" array for the return of Touches()
	DynamicArray<T *>		touches;

	// Profiling stuff
#ifdef MRM_PROFILE
	int						totalChecks;
#endif
};

// Include the implementation
#include "collide/mrm.hpp"

#endif







