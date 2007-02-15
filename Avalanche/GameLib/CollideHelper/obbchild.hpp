/***************************************************************************/
// A simple class to manage a list of OBB's
/***************************************************************************/

#include "collide/obb.h"
#include "collide/Cull.h"
#include "collide/Pair.h"

/***************************************************************************/
// Get the parent of the list
/***************************************************************************/
inline OBB &OBBChildList::Parent(void)
{
	return(parent);
}

/***************************************************************************/
// How many are there?
/***************************************************************************/
inline uint OBBChildList::Children(void)
{
	return(children);
}

/***************************************************************************/
/***************************************************************************/
inline void OBBChildList::SetChildren(uint newChildren)
{
	/* may have malloc'd more children than actually end up valid */

	ASSERT(newChildren <= maxChildren);	// can only reduce, not expand
	children = newChildren;
}

/***************************************************************************/
// Get a specific child
/***************************************************************************/
inline OBBChild &OBBChildList::operator[](uint index)
{
	ASSERT(index < children);
	return(child[index]);
}

/***************************************************************************/
// Setup from a list of OBB instances
/***************************************************************************/
inline void OBBChildList::Setup(
OBBInstance *obb,
uint count)
{
	ASSERT(count <= maxChildren);
	children = count;
	for (uint i = 0; i < children; i++)
	{
		child[i] = obb[i];
	}
}

/***************************************************************************/
// Apply a translation to all the children (not the parent)
/***************************************************************************/
inline void OBBChildList::ApplyChildTranslation(Vector3CRef t)
{
	for (uint i = 0; i < children; i++)
	{
		OBB &childOBB = child[i].GetOBB();
		childOBB.ApplyTranslation(t);
	}
}

/***************************************************************************/
/***************************************************************************/
inline bool OBBChildList::GetCollisionCull(CCollisionCull &o_cull)
{
	parent.ComputeWorldExtents(o_cull.SelectAABB());
	return(true);
}

/***************************************************************************/
/***************************************************************************/
inline int OBBChildList::FindChild(OBBInstance* i_obb)
{
	for (uint i = 0; i < children; i++)
	{
		if (i_obb == &child[i].GetOBB())
			return(i);
	}
	return(-1);
}
