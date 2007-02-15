#include "Layers/Debug.h"

/***************************************************************************/
// Create the initial tree
/***************************************************************************/
template <class T>
inline MultiResolutionMap<T>::MultiResolutionMap(
const XZRect &totalSize,
float smallestLeaf) :
leafPool(32, 32),
entityPool(64, 32),
entityList(64, 32, false),
touches(64, 64)
{
	// Make the top node square
	XZRect realSize = totalSize;
	realSize = totalSize;
	realSize.Normalize();
	realSize.MakeSquare();

#ifdef MRM_LOOSEQUADTREE
	// Inflate the size of the root node
	realSize.Inflate(realSize.Width() * 0.5f, realSize.Height() * 0.5f);
#else
	// Make this just a titch bigger
	realSize.Inflate(realSize.Width() * 0.1f, realSize.Height() * 0.1f);
#endif

	// Setup the top node
	top.Setup(realSize);

	// Save the smallest allowed area
	smallestArea = smallestLeaf * smallestLeaf;

#ifdef MRM_PROFILE
	totalChecks = 0;
#endif
}

/***************************************************************************/
// Add an object to the tree
/***************************************************************************/
template <class T>
inline bool MultiResolutionMap<T>::Add(
T *entity)
{
	// Create a new entity
	Entity *newEntity = entityPool.New();
	ASSERT(newEntity != NULL);
	if (!newEntity)
		return(false);

	// Set this bad boy up
	newEntity->size.Set(0.0f, 0.0f, 0.0f, 0.0f);
	newEntity->leaf = NULL;
	newEntity->entity = entity;

	// Put it into the flat list
	entityList.Add(newEntity);
	return(true);
}

/***************************************************************************/
// Remove an entity from the tree (this one is slow)
/***************************************************************************/
template <class T>
inline bool MultiResolutionMap<T>::Remove(
T *killMe)
{
	// Find them in the list
	int count = entityList.Count();
	for (int i = 0; i < count; i++)
	{
		Entity *entity = entityList.Get(i);
		if (entity->entity == killMe)
		{
			// Does this entity exist in the tree
			if (entity->leaf)
				entity->leaf->Unlink(entity);	// Unlink them from the leaf

			//	Whack him in the pool
			entityPool.Delete(entity);

			// Delete them from the flat list
			entityList.Delete(i);
			return(true);
		}
	}

	return(false);
}

/***************************************************************************/
// Mark an entity as active or inactive
/***************************************************************************/
template <class T>
inline void MultiResolutionMap<T>::Activate(
Entity *entity,
bool active)
{
	// Do we have to add this the tree?
	if (active && !entity->leaf)
	{
		// Push it down into the tree
		top.Add(leafPool, smallestArea, entity);
		return;
	}

	// Coming out of the tree?
	if (!active && entity->leaf)
		entity->leaf->Unlink(entity);
}

/***************************************************************************/
// Update the contents of the map using sizes (that you've updated)
/***************************************************************************/
template <class T>
inline void MultiResolutionMap<T>::Update(void)
{
	// Update everyone, this may cause the tree to grow/shrink
	top.Update(leafPool, smallestArea);

#ifdef MRM_VERIFY
	// Let's check it out
	top.Validate(top.Size(), smallestArea, NULL);
#endif
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline void MultiResolutionMap<T>::ShowStats(void)
{
	// Let's check it out
	Leaf::Stats stats;
	Leaf::ClearStats(stats);
	top.Validate(top.Size(), smallestArea, &stats);

	// Dump the stats
	TRACE("Total tree depth, leaves, entities: %d %d %d\n", stats.maxDepth + 1, stats.totalLeaves, stats.totalEntities);
	TRACE("Tree memory size: %dk\n", (stats.totalLeaves * sizeof(Leaf) + stats.totalEntities * sizeof(Entity) + 1023) / 1024);
	TRACE("Root entities %d\n", stats.entitiesAtLevel[0]);

	float avgDepth = (float)stats.entitiesAtLevel[0];
	float avgDiv = avgDepth;
	for (int i = 1; i <= stats.maxDepth; i++)
	{
		if (i < MAX_MRM_STATS_LEVELS && stats.entitiesAtLevel[i])
		{
			TRACE("Level %d entities %d\n", i, stats.entitiesAtLevel[i]);
			avgDepth += (float)(stats.entitiesAtLevel[i] * (i + 1));
			avgDiv += (float)stats.entitiesAtLevel[i];
		}
	}
	if (avgDiv != 0.0f)
		avgDepth /= avgDiv;
	TRACE("Average entity depth: %.3f\n", avgDepth - 1.0f);
}

/***************************************************************************/
// Return an array of T *'s
// This data is ONLY VALID until another MRM call is made
/***************************************************************************/
template <class T>
inline T **MultiResolutionMap<T>::Touches(
const XZRect &size,
int &count)
{
	// Get ready to build the list
	touches.Clear();

	// Build the list
	top.Touches(touches, size);

	// Return the count
	count = touches.Count();
#ifdef MRM_PROFILE
	totalChecks += count;
#endif
	if (count)
		return(touches.GetPtr());
	else
		return(NULL);
}

/***************************************************************************/
// Return an array of T *'s
// This data is ONLY VALID until another MRM call is made
/***************************************************************************/
template <class T>
inline T **MultiResolutionMap<T>::Touches(
const Vector3 &start,
const Vector3 &end,
int &count)
{
	SemiSegment test(start, end);

	// Get ready to build the list
	touches.Clear();

	// Build the list
	top.Touches(touches, test);

	// Return the count
	count = touches.Count();
#ifdef MRM_PROFILE
	totalChecks += count;
#endif
	if (count)
		return(touches.GetPtr());
	else
		return(NULL);
}


