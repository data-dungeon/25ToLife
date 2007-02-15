

#ifndef TYTEST_H
#define TYTEST_H

#ifdef MRMNODE_H
#error Dont use mrmnode.h
#define MRMNODE_H
#endif
#ifdef MRM_H
#error dont use mrm.h
#define MRM_H
#endif

#include "collide/xzrect.h"
#include "container/dynlist.h"
#include "container/pool.h"

template <class T> class MRMNode;

// This is what we hold in the tree leaves
template <class T> class MRMEntity
{
	typedef MRMEntity<T> Entity;
public:
	// Nothing to see here
	inline MRMEntity(void) { next = NULL; leaf = NULL; }

	XZRect				size;          	// Size of this entity
  	T						*entity;				// The actual entity
	MRMEntity<T>		*prev, *next;		// We exist in a list
	MRMNode<T>			*leaf;

	float DistanceSquared(Entity* e){
		float distX = size.CenterX() - e->size.CenterX();
		float distZ = size.CenterZ() - e->size.CenterZ();
		return distX*distX + distZ*distZ;
	}
};

template <class T> class MRMNode
{
	typedef MRMEntity<T> Entity;
	typedef MRMNode<T> Node;
public:
	MRMNode(){
		left.entity = NULL; 
		right.entity = NULL; 
		type = ENTITY_ENTITY;
		size.Clear();
	}

	union Pointer {
		Node* node;
		Entity* entity;
	};

	enum {
		NODE_NODE,
		ENTITY_ENTITY,
	} type;

	Pointer left;
	Pointer right;

	XZRect size;

	// Return an array of T *'s
	// This data is ONLY VALID until another MRM call is made
	void Touches(DynamicArray<T *> &touches, const XZRect &test);
};

template <class T> class MultiResolutionMap
{
public:
	// Create the map
	inline MultiResolutionMap(const XZRect &totalSize, float smallestLeaf = 4.0f);
	inline ~MultiResolutionMap(void) {}

	// Convenient types
	typedef MRMEntity<T>		Entity;
	typedef MRMNode<T>		Node;

	// Add an entity to the tree (doesn't really get added until you 
	// set size and call Activate)
	inline bool Add(T *entity);

	// Remove an entity from the tree (this is pretty slow, it has to search)
	inline bool Remove(T *entity);

	// Access a "flat" array of all the entities in the world
	inline DynList<Entity> &GetEntityList(void)
		{ return(entityList); }

	// Access a "flat" array of all the entities in the world
	inline DynList<Entity> &GetActiveEntityList(void)
		{ return(activeEntityList); }

	// Mark an entity as active or inactive
	inline void Activate(Entity *entity, bool active);

	// Update the contents of the map using sizes in the MRMEntity
	// (that you've updated manually)
	inline void Update(void);

	// Return an array of T *'s
	// This data is ONLY VALID until another MRM call is made
	inline T **Touches(const XZRect &size, int &count);

private:

#ifdef MRM_PROFILE
	inline int TotalChecks(void)
		{ return(totalChecks); }
	inline void ClearChecks(void)
		{ totalChecks = 0; }
#endif
	// Log some stats to debug output
	inline void ShowStats(void);

	// TJC - clear a tree
	inline void Clear(Node* node);

	// TJC - sort routine
	inline void SortList(void);

	void Validate(Node* node);

	// Our internal data storage
	Pool<Entity>			entityPool;
	Pool<Node>				nodePool;

	// TJC - the top of the tree..
	Node						top;

	// TJC - a work array for building the tree...
	DynamicArray<Node*>	treeWork;

	// TJC - a work array for building the tree...
	DynamicArray<Node*>	newTreeWork;

	// A dynamic array of entities, for doing updates
	DynList<Entity>		entityList;

	// TJC - the currently sorted list of entities.
	DoublyLinkedList<Entity>		activeEntityList;

	// The "static" array for the return of Touches()
	DynamicArray<T *>		touches;

	// Profiling stuff
#ifdef MRM_PROFILE
	int						totalChecks;
#endif
};

// Include the implementation
#include "collide/mrm2.hpp"

#endif // TYTEST_H
