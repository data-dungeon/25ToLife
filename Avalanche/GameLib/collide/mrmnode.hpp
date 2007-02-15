/***************************************************************************/
// We do nothing
/***************************************************************************/
template <class T>
inline MRMTreeLeaf<T>::MRMTreeLeaf(void) :
entityList(false)
{
}

/***************************************************************************/
// What would the rect of the child be?
/***************************************************************************/
template <class T>
inline XZRect MRMTreeLeaf<T>::ChildRect(
const XZRect &parent,
QUADTREECHILD which)
{
	XZRect childRect;

#ifdef MRM_LOOSEQUADTREE
	float childSize = parent.Width() * 0.5f;
	float centerX = 0.0f;
	float centerZ = 0.0f;

	switch (which)
	{
		case QUADTREECHILD_NE:
			centerX = parent.CenterX() + childSize * 0.25f;
			centerZ = parent.CenterZ() + childSize * 0.25f;
			break;
		case QUADTREECHILD_NW:
			centerX = parent.CenterX() - childSize * 0.25f;
			centerZ = parent.CenterZ() + childSize * 0.25f;
			break;
		case QUADTREECHILD_SW:
			centerX = parent.CenterX() - childSize * 0.25f;
			centerZ = parent.CenterZ() - childSize * 0.25f;
			break;
		case QUADTREECHILD_SE:
			centerX = parent.CenterX() + childSize * 0.25f;
			centerZ = parent.CenterZ() - childSize * 0.25f;
			break;
	}

	float childHalf = childSize * 0.5f;
	childRect.Set(centerX - childHalf, centerZ - childHalf, centerX + childHalf, centerZ + childHalf);

#else
	float centerx = parent.CenterX();
	float centerz = parent.CenterZ();

	switch (which)
	{
		case QUADTREECHILD_NE:
			childRect.Set(centerx, centerz, parent.MaxX(), parent.MaxZ());
			break;
		case QUADTREECHILD_NW:
			childRect.Set(parent.MinX(), centerz, centerx, parent.MaxZ());
			break;
		case QUADTREECHILD_SW:
			childRect.Set(parent.MinX(), parent.MinZ(), centerx, centerz);
			break;
		case QUADTREECHILD_SE:
			childRect.Set(centerx, parent.MinZ(), parent.MaxX(), centerz);
			break;
	}
#endif

	return(childRect);
}

/***************************************************************************/
// What is the best quadrant to try to add too...
/***************************************************************************/
template <class T>
inline QUADTREECHILD MRMTreeLeaf<T>::BestQuadrant(
const XZRect &testRect)
{
	const float myX = size.CenterX();
	const float myZ = size.CenterZ();
	const float testX = testRect.CenterX();
	const float testZ = testRect.CenterZ();

	if (testX > myX)
	{
		if (testZ > myZ)
			return(QUADTREECHILD_NE);
		else
			return(QUADTREECHILD_SE);
	}
	else
	{
		if (testZ > myZ)
			return(QUADTREECHILD_NW);
		else
			return(QUADTREECHILD_SW);
	}
}

/***************************************************************************/
// Create a child, return NULL if too small
/***************************************************************************/
template <class T>
inline MRMTreeLeaf<T> *MRMTreeLeaf<T>::MakeChild(
Pool<Leaf> &leafPool,
float smallestArea,
const XZRect &childRect)
{
	// Is this leaf going to be too small?
	if (childRect.Area() < smallestArea)
		return(NULL);

	// Create a new leaf
	Leaf *leaf = leafPool.New();
	ASSERT(leaf);
	if (!leaf)
		return(NULL);

	// Set it up
	leaf->Setup(childRect);
	return(leaf);
}

/***************************************************************************/
// If this child is empty and has no children, delete it (not recursive)
/***************************************************************************/
template <class T>
inline void MRMTreeLeaf<T>::CheckEmptyChild(
Pool<Leaf> &leafPool,
Leaf *&leaf)
{
	// Do we have any children, or entities?
	if (leaf->child[QUADTREECHILD_NE] == NULL &&
			leaf->child[QUADTREECHILD_NW] == NULL &&
			leaf->child[QUADTREECHILD_SW] == NULL &&
			leaf->child[QUADTREECHILD_SE] == NULL &&
			leaf->entityList.Head() == NULL)
	{
		// Get rid of it
		leafPool.Delete(leaf);
		leaf = NULL;
	}
}

/***************************************************************************/
// Add an entity (calls children)
/***************************************************************************/
template <class T>
inline bool MRMTreeLeaf<T>::Add(
Pool<Leaf> &leafPool,
float smallestArea,
Entity *entity)
{
	// We only get called if this does fit
	ASSERT(size.Contains(entity->size));

	// Try to push this down to our best child
	QUADTREECHILD best = BestQuadrant(entity->size);
	XZRect childSize = ChildRect(size, best);
	if (childSize.Contains(entity->size))
	{
		// Push it down into the child
		if (!child[best])
			child[best] = MakeChild(leafPool, smallestArea, childSize);

		// If we made a child, this will not fail
		if (child[best])
			return(child[best]->Add(leafPool, smallestArea, entity));
	}

	// Didn't fit anywhere lower, put it in our list
	entity->leaf = this;
	entityList.AddTail(entity);
	return(true);
}

/***************************************************************************/
// Push it down if we can
/***************************************************************************/
template <class T>
inline void MRMTreeLeaf<T>::PushDown(
Pool<Leaf> &leafPool,
float smallestArea,
Entity *entity)
{
	// We only get called if this does fit
	ASSERT(size.Contains(entity->size));

	// Can we push this down even more?
	QUADTREECHILD best = BestQuadrant(entity->size);
	XZRect childSize = ChildRect(size, best);

	// Couldn't push add it here
	if (childSize.Contains(entity->size))
	{
		// Push it down into the child
		if (!child[best])
			child[best] = MakeChild(leafPool, smallestArea, childSize);

		// Push them again, entity is already unlinked
		if (child[best])
		{
			child[best]->PushDown(leafPool, smallestArea, entity);
			return;
		}
	}

	// Couldn't push add it here
	entity->leaf = this;
	entityList.AddTail(entity);
}

/***************************************************************************/
// Update everyone in this node
/***************************************************************************/
template <class T>
inline void MRMTreeLeaf<T>::Update(
Pool<Leaf> &leafPool,
float smallestArea,
Leaf *parent)
{
	// update children first
	if (child[QUADTREECHILD_NE])
		child[QUADTREECHILD_NE]->Update(leafPool, smallestArea, this);
	if (child[QUADTREECHILD_NW])
		child[QUADTREECHILD_NW]->Update(leafPool, smallestArea, this);
	if (child[QUADTREECHILD_SW])
		child[QUADTREECHILD_SW]->Update(leafPool, smallestArea, this);
	if (child[QUADTREECHILD_SE])
		child[QUADTREECHILD_SE]->Update(leafPool, smallestArea, this);

	// Update all of my guys
	Entity *entity = entityList.Head();
	while (entity)
	{
		// Save this next
  		Entity *next = entity->next;

		// Can we push up?
		if (!PushUp(parent, entity))
		{
			// Try to push down
			QUADTREECHILD best = BestQuadrant(entity->size);
			XZRect childSize = ChildRect(size, best);
			if (childSize.Contains(entity->size))
			{
				// Create the new leaf if we have to
				if (!child[best])
					child[best] = MakeChild(leafPool, smallestArea, childSize);

				// Unlink the entity from us, pushing them down
				if (child[best])
				{
					entityList.Unlink(entity);
					child[best]->PushDown(leafPool, smallestArea, entity);
				}
			}
		}

		// Go to the next guy
		entity = next;
	}

	// Garbage collect all of our children
 	if (child[QUADTREECHILD_NE])
		CheckEmptyChild(leafPool, child[QUADTREECHILD_NE]);
 	if (child[QUADTREECHILD_NW])
		CheckEmptyChild(leafPool, child[QUADTREECHILD_NW]);
 	if (child[QUADTREECHILD_SW])
		CheckEmptyChild(leafPool, child[QUADTREECHILD_SW]);
 	if (child[QUADTREECHILD_SE])
		CheckEmptyChild(leafPool, child[QUADTREECHILD_SE]);
}

/***************************************************************************/
// Debugging validation & stats
/***************************************************************************/
template <class T>
inline void MRMTreeLeaf<T>::ClearStats(
Stats &stats)
{
	stats.maxDepth = 0;
	stats.totalLeaves = 0;
	stats.totalEntities = 0;
	for (int i = 0; i < 64; i++)
		stats.entitiesAtLevel[i] = 0;
}

/***************************************************************************/
// Validate this leaf is correct
/***************************************************************************/
template <class T>
inline void MRMTreeLeaf<T>::Validate(
const XZRect &expectedSize,
float smallestArea,
Stats *stats,
int level)
{
	// Does our size match?
	ASSERT(Math::Equal(expectedSize.MinX(), size.MinX()));
	ASSERT(Math::Equal(expectedSize.MinZ(), size.MinZ()));
	ASSERT(Math::Equal(expectedSize.MaxX(), size.MaxX()));
	ASSERT(Math::Equal(expectedSize.MaxZ(), size.MaxZ()));

	// This node had better not be empty
	ASSERT(level == 0 ||
				child[0] || child[1] || child[2] || child[3] ||
				entityList.Head());

	// Check all my entities
	Entity *entity = entityList.Head();
	while (entity)
	{
		// We better fit
		ASSERT(size.Contains(entity->size));

		// We better NOT fit in any of our children
		QUADTREECHILD best = BestQuadrant(entity->size);
		for (int i = 0; i < 4; i++)
		{
			// Do we fit in a child?
			XZRect childSize = ChildRect(size, (QUADTREECHILD)i);
			if (childSize.Area() > smallestArea)
			{
				ASSERT(!childSize.Contains(entity->size));
			}
		}

		// Next
		entity = entity->next;
	}

	// Update the stats
	if (stats)
	{
		if (level > stats->maxDepth)
			stats->maxDepth = level;
		++stats->totalLeaves;
		stats->totalEntities += entityList.Items();
		if (level < MAX_MRM_STATS_LEVELS)
			stats->entitiesAtLevel[level] += entityList.Items();
	}

	// Validate all children
	for (int i = 0; i < 4; i++)
	{
		if (child[i])
		{
			// What is the expected size
			XZRect childSize = ChildRect(size, (QUADTREECHILD)i);
			child[i]->Validate(childSize, smallestArea, stats, level + 1);
		}
	}
}

/***************************************************************************/
// Return an array of T *'s
// This data is ONLY VALID until another MRM call is made
/***************************************************************************/
template <class T>
inline void MRMTreeLeaf<T>::Touches(
DynamicArray<T *> &touches,
const XZRect &test)
{
	// Only do something if they touch us
	if (size.Intersects(test))
	{
		// Add our entities
		Entity *entity = entityList.Head();
		while (entity)
		{
			touches.Add(entity->entity);
			entity = entity->next;
		}

		// Try the children
		if (child[QUADTREECHILD_NE])
			child[QUADTREECHILD_NE]->Touches(touches, test);
		if (child[QUADTREECHILD_NW])
			child[QUADTREECHILD_NW]->Touches(touches, test);
		if (child[QUADTREECHILD_SW])
			child[QUADTREECHILD_SW]->Touches(touches, test);
		if (child[QUADTREECHILD_SE])
			child[QUADTREECHILD_SE]->Touches(touches, test);
	}
}

/***************************************************************************/
// Return an array of T *'s
// This data is ONLY VALID until another MRM call is made
/***************************************************************************/
template <class T>
inline void MRMTreeLeaf<T>::Touches(
DynamicArray<T *> &touches,
const SemiSegment &test)
{
	// Only do something if they touch us
	if (size.Touches(test))
	{
		// Add our entities
		Entity *entity = entityList.Head();
		while (entity)
		{
			touches.Add(entity->entity);
			entity = entity->next;
		}

		// Try the children
		if (child[QUADTREECHILD_NE])
			child[QUADTREECHILD_NE]->Touches(touches, test);
		if (child[QUADTREECHILD_NW])
			child[QUADTREECHILD_NW]->Touches(touches, test);
		if (child[QUADTREECHILD_SW])
			child[QUADTREECHILD_SW]->Touches(touches, test);
		if (child[QUADTREECHILD_SE])
			child[QUADTREECHILD_SE]->Touches(touches, test);
	}
}


/***************************************************************************/
// Update this entity
/***************************************************************************/
template <class T>
inline bool MRMTreeLeaf<T>::PushUp(
Leaf *parent,
Entity *entity)
{
	// Do we have to push them up?
	if (!size.Contains(entity->size))
	{
		// This would be bad, the top node is too small
		ASSERT(parent != NULL);
		if (parent)
		{
			// Pop this off our list
			entityList.Unlink(entity);

			// Push this guy up
			entity->leaf = parent;
			parent->entityList.AddTail(entity);
			return(true);
		}
	}

	// We did not move the entity
	return(false);
}


