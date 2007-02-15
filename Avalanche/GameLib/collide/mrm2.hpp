

/***************************************************************************/
// Create the initial tree
/***************************************************************************/
template <class T>
inline MultiResolutionMap<T>::MultiResolutionMap(
const XZRect &totalSize,
float smallestLeaf) :
entityPool(64, 32),
nodePool(32, 32),
treeWork(64, 64),
entityList(64, 32, false),
activeEntityList(false),
touches(64, 64)
{
#ifdef MRM_PROFILE
	totalChecks = 0;
#endif
}

/***************************************************************************/
// Add an object to the tree
/***************************************************************************/
// Add an entity to the tree (doesn't really get added until you 
// set size and call Activate)
template <class T>
inline bool MultiResolutionMap<T>::Add(T *entity)
{
	// Create a new entity
	Entity *newEntity = entityPool.New();
	ASSERT(newEntity != NULL);
	if (!newEntity)
		return(false);

	// Set this bad boy up
	newEntity->size.Set(0.0f, 0.0f, 0.0f, 0.0f);
	newEntity->entity = entity;

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
	ASSERT_PTR(killMe);

	// Find them in the list
	int count = entityList.Count();
	for (int i = 0; i < count; i++)
	{
		Entity *entity = entityList.Get(i);
		ASSERT_PTR(entity);
		if (entity->entity == killMe)
		{
			// Does this entity exist in the tree
			if (entity->leaf)
			{
				ASSERT_PTR(entity->leaf);
				if(entity->leaf->left.entity == entity)
					entity->leaf->left.entity = NULL;
				else if(entity->leaf->right.entity == entity)
					entity->leaf->right.entity = NULL;
			}

			// get him out of the sorted list.
			if(entity->next)
			{
				ASSERT_PTR(entity->next);
				activeEntityList.Remove(entity);
			}
			
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
	ASSERT_PTR(entity);
	ASSERT_PTR(entity->entity);

	// Do we have to add this the tree?
	if (active && entity->leaf == NULL)
	{
		// TJC - insert sorted into active list... I wish the sort was
		// as easy as a single compare this against another object, but
		// it takes 3 to tango...  thats because we want to fit it
		// inbetween its two closest relatives...  Except if its closest
		// to the ends, then we always put it on the head or tail..
		// That gives us more distance between head and tail.
		Entity* curr = activeEntityList.Head();
		Entity* best = curr;
		float prevDist = Math::NegativeInfinity;
		float bestDist = Math::PositiveInfinity;
		float nextDist = Math::PositiveInfinity;
		float priorDist = Math::NegativeInfinity;
		while(curr)
		{
			ASSERT_PTR(curr);
			float testDist = curr->DistanceSquared(entity);

			if(bestDist == priorDist)
				nextDist = testDist;
				
			if(testDist < bestDist)
			{
				bestDist = testDist;
				prevDist = priorDist;
				nextDist = 0.0f;
				best = curr;
			}

			priorDist = testDist;

			curr = curr->next;
		}
		if(bestDist != Math::PositiveInfinity)
		{
			if(prevDist < nextDist)
				activeEntityList.InsertBefore(best, entity);
			else
				activeEntityList.InsertAfter(best, entity);
		}
		else
		{
			activeEntityList.AddHead(entity);
		}

		return;
	}

	// Coming out of the tree?
	if ((!active) && entity->leaf != NULL)
	{
		ASSERT_PTR(entity->leaf);
		activeEntityList.Remove(entity);
	}
}

/***************************************************************************/
// Update the contents of the map using sizes (that you've updated)
/***************************************************************************/
template <class T>
inline void MultiResolutionMap<T>::SortList(void)
{
	// TJC - this is just a bubble since the list should be near
	// correct.  On top of that, the algorithm doesnt require a correct
	// sort (its just faster that way), so we spread the bubble sort
	// out across frames and therefore, major changes will take a few
	// passes to get correct..  This helps to save us time if the nodes
	// are all really close to each other.  The sort is kindof
	// pointless if there is < 3 nodes, which is fortunate, because
	// it requres at least 3 nodes.  
	Entity* prior = activeEntityList.Head();
	if(!prior)
		return;

	Entity* curr = prior->next;
	if(!curr)
		return;

	Entity* next = curr->next;
	if(!next)
		return;

	// TJC - first, determine which node is furthest from the others...

	while(next){
		ASSERT_PTR(curr);
		ASSERT_PTR(next);
		ASSERT_PTR(prior);
		ASSERT_PTR(curr->entity);
		ASSERT_PTR(next->entity);
		ASSERT_PTR(prior->entity);

		float currDist = curr->DistanceSquared(next);
		float priorDist = prior->DistanceSquared(next);

		if(currDist > priorDist){
			// TJC - swap curr & prior...
			activeEntityList.Remove(curr);
			activeEntityList.InsertBefore(prior, curr);
		}
		else
		{
			// TJC - we dont need this if we swapped prior & curr in the
			// list since that means that prior is pointing at the place
			// I would want it to be.
			prior = curr;
		}

		// TJC - move along
		curr = next;
		next = next->next;
	}

	// TJC - now to make this work, I run down the list backwards as
	// well currently.  Im not sure if this is necessary, but it makes
	// some sense to me, and since this is just test code right now,
	// I'll go ahead and do it.
	
	prior = activeEntityList.Tail();
	if(!prior)
		return;

	curr = prior->prev;
	if(!curr)
		return;

	next = curr->prev;

	while(next){
		ASSERT_PTR(curr);
		ASSERT_PTR(next);
		ASSERT_PTR(prior);
		ASSERT_PTR(curr->entity);
		ASSERT_PTR(next->entity);
		ASSERT_PTR(prior->entity);

		float currDist = curr->DistanceSquared(next);
		float priorDist = prior->DistanceSquared(next);

		if(currDist > priorDist){
			// TJC - swap curr & prior...
			activeEntityList.Remove(curr);
			activeEntityList.InsertAfter(prior, curr);
		}
		else
		{
			// TJC - we dont need this if we swapped prior & curr in the
			// list since that means that prior is pointing at the place
			// I would want it to be.
			prior = curr;
		}

		// TJC - move along
		curr = next;
		next = next->prev;
	}
}

/***************************************************************************/
// Update the contents of the map using sizes (that you've updated)
/***************************************************************************/
template <class T>
inline void MultiResolutionMap<T>::Clear(Node* node)
{
	ASSERT_PTR(node);
	if(node->type == Node::NODE_NODE)
	{
		if(node->left.node)
		{
			ASSERT_PTR(node->left.node);
			Clear(node->left.node);
			nodePool.Delete(node->left.node);
			node->left.node = NULL;
		}
		if(node->right.node)
		{
			ASSERT_PTR(node->right.node);
			Clear(node->right.node);
			nodePool.Delete(node->right.node);
			node->right.node = NULL;
		}
	}
	else if(node->type == Node::ENTITY_ENTITY)
	{
		if(node->left.entity)
		{
			ASSERT_PTR(node->left.entity);
			ASSERT_PTR(node->left.entity->leaf);
			node->left.entity->leaf = NULL;
			node->left.entity = NULL;
		}
		if(node->right.entity)
		{
			ASSERT_PTR(node->right.entity);
			ASSERT_PTR(node->right.entity->leaf);
			node->right.entity->leaf = NULL;
			node->right.entity = NULL;
		}
	}
	else
	{
		ASSERTS(false, "Invalid node in MRMTree");
	}
}

/***************************************************************************/
// Update the contents of the map using sizes (that you've updated)
/***************************************************************************/
template <class T>
inline void MultiResolutionMap<T>::Update(void)
{
	// TJC - ok, this is easy, we build a tree bottom up by joining
	// adjacent obbs...  first we need to sort the list though... }
	SortList();

	// TJC - clear old tree
	Clear(&top);
	
	top.left.node = NULL;
	top.right.node = NULL;

	// TJC - ok, now we need to build the tree on top of this, bottom
	// up... breath first.
	Entity* curr = activeEntityList.Head();

	// TJC - this pass builds the very bottom of the tree...
	while(curr)
	{
		Entity* next = curr->next;

		ASSERT_PTR(curr);
		ASSERT_PTR(curr->entity);
		if(next){
			ASSERT_PTR(next);
			ASSERT_PTR(next->entity);
		}
		
		// TJC - join curr & next
		// Create a new node
		Node *newNode = nodePool.New();
		ASSERT_PTR(newNode);
		if (!newNode)
			return;
		
		newNode->left.entity = curr;
		newNode->size = curr->size;
		newNode->type = Node::ENTITY_ENTITY;
		ASSERT(curr->leaf == NULL);
		curr->leaf = newNode;
		if(next)
		{
			newNode->right.entity = next;
			newNode->size += next->size;
			ASSERT(next->leaf == NULL);
			next->leaf = newNode;
		} else {
			newNode->right.entity = NULL;
		}
		treeWork.Add(newNode);
		
		curr = next;
		if(curr) curr = curr->next;
	}

	DynamicArray<Node*> *currTree = &treeWork;
	DynamicArray<Node*> *nextTree = &newTreeWork;

	ASSERT_PTR(currTree);
	ASSERT_PTR(nextTree);

	// TJC - while we dont have a single node.
	int count = currTree->Count();
	while(count > 1)
	{
		// TJC - collapse the entire breath of the tree by one layer.
		for(int index = 0; index < count; index += 2)
		{
			Node *currNode;
			Node *nextNode;
			currNode = (*currTree)[index];
			ASSERT_PTR(currNode);
			ASSERT(currNode->type == Node::NODE_NODE || currNode->type == Node::ENTITY_ENTITY);
			
			// TJC - join curr & next
			// Create a new node
			Node *newNode = nodePool.New();
		if((int)newNode == 0x32CA980)
			LOCKUP();
			ASSERT_PTR(newNode);
			if (!newNode)
				return;
		
			newNode->left.node = currNode;
			newNode->size = currNode->size;
			newNode->type = Node::NODE_NODE;

			if(index + 1 < count)
			{
				nextNode = (*currTree)[index+1];
				ASSERT_PTR(nextNode);
				ASSERT(nextNode->type == Node::NODE_NODE || nextNode->type == Node::ENTITY_ENTITY);
				newNode->right.node = nextNode;
				newNode->size += nextNode->size;
			} else {
				newNode->right.node = NULL;
			}
			nextTree->Add(newNode);
		}
		Math::Swap(currTree, nextTree);
		nextTree->Clear();
		count = currTree->Count();
	}
	if(count)
	{
		ASSERT_PTR(currTree->Get(0));
		ASSERT_PTR(*currTree->Get(0));
		top = **currTree->Get(0);
		if(top.type == Node::ENTITY_ENTITY){
			if(top.left.entity)
				top.left.entity->leaf = &top;
			if(top.right.entity)
				top.right.entity->leaf = &top;
		}
		currTree->Clear();
		Validate(&top);
	}
	else
	{
		top.left.entity = NULL;
		top.right.entity = NULL;
		top.type = Node::ENTITY_ENTITY;
		top.size.Clear();
	}

	currTree->Clear();
}

template <class T>
inline void MultiResolutionMap<T>::Validate(Node* node)
{
	ASSERT_PTR(node);
	switch(node->type)
	{
	case Node::NODE_NODE:
		if(node->left.node)
		{
			ASSERT_PTR(node->left.node);
			Validate(node->left.node);
		}
		if(node->right.node)
		{
			ASSERT_PTR(node->right.node);
			Validate(node->right.node);
		}
		break;
	case Node::ENTITY_ENTITY:
		if(node->left.entity)
		{
			ASSERT_PTR(node->left.entity);
			ASSERT_PTR(node->left.entity->entity);
			ASSERT(node->left.entity->leaf == node);
		}
		if(node->right.entity)
		{
			ASSERT_PTR(node->right.entity);
			ASSERT_PTR(node->right.entity->entity);
			ASSERT(node->right.entity->leaf == node);
		}
		break;
	default:
		ASSERTS(false, "Invalid node in MultiResolutionMap");
	}
}


/***************************************************************************/
/***************************************************************************/
template <class T>
inline void MultiResolutionMap<T>::ShowStats(void)
{
//	dbgPrint("Entites: %d\n", entityPool->Count());
// dbgPrint("Nodes: %d\n", nodePool->Count());
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

template <class T>
inline void MRMNode<T>::Touches(DynamicArray<T *> &touches, const XZRect &test)
{
	if(size.Intersects(test))
	{
		if(type == NODE_NODE)
		{
			if(left.node)
			{
				ASSERT_PTR(left.node);
				left.node->Touches(touches, test);
			}
			if(right.node)
			{
				ASSERT_PTR(right.node);
				right.node->Touches(touches, test);
			}
		}
		else
		{
			if(left.entity)
			{
				ASSERT_PTR(left.entity);
				ASSERT_PTR(left.entity->entity);
				touches.Add(left.entity->entity);
			}
			if(right.entity)
			{
				ASSERT_PTR(right.entity);
				ASSERT_PTR(right.entity->entity);
				touches.Add(right.entity->entity);
			}
		}
	}
}

