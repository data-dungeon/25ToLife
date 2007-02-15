/***************************************************************************/
// A class for creating a loadable quadtree of objects
/***************************************************************************/

/******************* includes ****************************/

#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/ObjectQT.h"

#if 0

// Engine includes //
#include "Math/Vector.h"

// Gamelib includes //
#include "collide/StackArray.h"

#endif

/***************************************************************************/
// Build a list of Objects
/***************************************************************************/
void ObjectQuadTreeNode::BuildList(
CCollisionStackArray<CStaticCollisionObject*>* io_list,
Vector3CRef i_start,
Vector3CRef i_chord) const
{
	// Does the line fall in our rect?
	float enter, exit;
	if (!rect.Crosses(i_start, i_chord, enter, exit) || (enter > 1.0f) ||
			(exit < 0.0f))
		return;

	if (objects > 0)
	{
		CStaticCollisionObject** array = io_list->ExpandArray(objects);
		if (array == NULL)
			return;

		for (uint i = 0; i < objects; i++)
			array[i] = &objectList[i];
	}

	// check our children
	if (child[QUADTREECHILD_NE])
		child[QUADTREECHILD_NE]->BuildList(io_list, i_start, i_chord);
	if (child[QUADTREECHILD_NW])
		child[QUADTREECHILD_NW]->BuildList(io_list, i_start, i_chord);
	if (child[QUADTREECHILD_SW])
		child[QUADTREECHILD_SW]->BuildList(io_list, i_start, i_chord);
	if (child[QUADTREECHILD_SE])
		child[QUADTREECHILD_SE]->BuildList(io_list, i_start, i_chord);
}

/***************************************************************************/
// Build a list of Objects
/***************************************************************************/
void ObjectQuadTreeNode::BuildList(
CCollisionStackArray<CStaticCollisionObject*>* io_list,
const XZRect &i_rect) const
{
	// Does the line fall in our rect?
	if (!rect.Intersects(i_rect))
		return;

	if (objects > 0)
	{
		CStaticCollisionObject** array = io_list->ExpandArray(objects);
		if (array == NULL)
			return;

		for (uint i = 0; i < objects; i++)
			array[i] = &objectList[i];
	}

	// check our children
	if (child[QUADTREECHILD_NE])
		child[QUADTREECHILD_NE]->BuildList(io_list, i_rect);
	if (child[QUADTREECHILD_NW])
		child[QUADTREECHILD_NW]->BuildList(io_list, i_rect);
	if (child[QUADTREECHILD_SW])
		child[QUADTREECHILD_SW]->BuildList(io_list, i_rect);
	if (child[QUADTREECHILD_SE])
		child[QUADTREECHILD_SE]->BuildList(io_list, i_rect);
}

/***************************************************************************/
// Fixup the data (after a load)
/***************************************************************************/
void ObjectQuadTreeNode::Fixup(void)
{
	uint baseAddr = (uint)this;

#ifdef GCN
	EndianSwap( );
#endif

	// Fixup all the addresses
	if (objectList != NULL)
	{
		objectList = (CStaticCollisionObject *)(baseAddr + (uint)objectList);
		for (uint o = 0; o < objects; o++)
			objectList[o].Fixup();
	}

	// Do the children
	if (child[QUADTREECHILD_NE])
	{
		child[QUADTREECHILD_NE] = (ObjectQuadTreeNode *)(baseAddr + (uint)child[QUADTREECHILD_NE]);
		child[QUADTREECHILD_NE]->Fixup();
	}

	if (child[QUADTREECHILD_NW])
	{
		child[QUADTREECHILD_NW] = (ObjectQuadTreeNode *)(baseAddr + (uint)child[QUADTREECHILD_NW]);
		child[QUADTREECHILD_NW]->Fixup();
	}

	if (child[QUADTREECHILD_SW])
	{
		child[QUADTREECHILD_SW] = (ObjectQuadTreeNode *)(baseAddr + (uint)child[QUADTREECHILD_SW]);
		child[QUADTREECHILD_SW]->Fixup();
	}

	if (child[QUADTREECHILD_SE])
	{
		child[QUADTREECHILD_SE] = (ObjectQuadTreeNode *)(baseAddr + (uint)child[QUADTREECHILD_SE]);
		child[QUADTREECHILD_SE]->Fixup();
	}
}

/***************************************************************************/
/***************************************************************************/
void ObjectQuadTreeNode::Validate(
XZRect *parentRect,
QUADTREECHILD which)
{
	// If parent rect is null, we are the root
	if (parentRect != NULL)
	{
		// What should the rect be?
		XZRect childRect = ChildRect(*parentRect, which);
		if (!RectsSame(childRect, rect))
		{
			ASSERTS(false, "Object Quad Tree: Invalid child rect (safe to ignore).\n");
			return;
		}
	}

	// Now check through the objects
	for (uint o = 0; o < objects; o++)
	{
		if (!objectList[o].Validate(rect))
			return;
	}

	// Check our children
	if (child[QUADTREECHILD_NE])
		child[QUADTREECHILD_NE]->Validate(&rect, QUADTREECHILD_NE);
	if (child[QUADTREECHILD_NW])
		child[QUADTREECHILD_NW]->Validate(&rect, QUADTREECHILD_NW);
	if (child[QUADTREECHILD_SW])
		child[QUADTREECHILD_SW]->Validate(&rect, QUADTREECHILD_SW);
	if (child[QUADTREECHILD_SE])
		child[QUADTREECHILD_SE]->Validate(&rect, QUADTREECHILD_SE);
}

/***************************************************************************/
/***************************************************************************/
XZRect ObjectQuadTreeNode::ChildRect(
XZRect &parentRect,
QUADTREECHILD which)
{
	XZRect childRect;

	float childSize = parentRect.Width() * 0.5f;
	float centerX = 0.0f;
	float centerZ = 0.0f;

	switch (which)
	{
		case QUADTREECHILD_NE:
			centerX = parentRect.CenterX() + childSize * 0.25f;
			centerZ = parentRect.CenterZ() + childSize * 0.25f;
			break;
		case QUADTREECHILD_NW:
			centerX = parentRect.CenterX() - childSize * 0.25f;
			centerZ = parentRect.CenterZ() + childSize * 0.25f;
			break;
		case QUADTREECHILD_SW:
			centerX = parentRect.CenterX() - childSize * 0.25f;
			centerZ = parentRect.CenterZ() - childSize * 0.25f;
			break;
		case QUADTREECHILD_SE:
			centerX = parentRect.CenterX() + childSize * 0.25f;
			centerZ = parentRect.CenterZ() - childSize * 0.25f;
			break;
	}

	float childHalf = childSize * 0.5f;
	childRect.Set(centerX - childHalf, centerZ - childHalf, centerX + childHalf, centerZ + childHalf);

	return(childRect);
}

/***************************************************************************/
/***************************************************************************/
bool ObjectQuadTreeNode::RectsSame(
XZRect &a,
XZRect &b)
{
	float mindif = (a.MinX() - b.MinX()) * (a.MinX() - b.MinX()) +
						(a.MinZ() - b.MinZ()) * (a.MinZ() - b.MinZ());

	float maxdif = (a.MaxX() - b.MaxX()) * (a.MaxX() - b.MaxX()) +
						(a.MaxZ() - b.MaxZ()) * (a.MaxZ() - b.MaxZ());

	return(Math::Zero(mindif) && Math::Zero(maxdif));
}

#ifdef OBJECT_QT_WRITE_FILE
/***************************************************************************/
/***************************************************************************/
int ObjectQuadTreeNode::PrintTree(int i_level, const char* i_dir, FILE* i_fp)
{
	int childLevel = i_level + 1;
	int numChildObject = 0;

	if (child[QUADTREECHILD_NE])
		numChildObject += child[QUADTREECHILD_NE]->PrintTree(childLevel, "NE", i_fp);
	if (child[QUADTREECHILD_NW])
		numChildObject += child[QUADTREECHILD_NW]->PrintTree(childLevel, "NW", i_fp);
	if (child[QUADTREECHILD_SW])
		numChildObject += child[QUADTREECHILD_SW]->PrintTree(childLevel, "SW", i_fp);
	if (child[QUADTREECHILD_SE])
		numChildObject += child[QUADTREECHILD_SE]->PrintTree(childLevel, "SE", i_fp);

	int numObject = numChildObject + objects;

	if (numChildObject == 0)
		fprintf(i_fp, "\n");

	if (i_level > 0)
	{
		fprintf(i_fp, "%*c%d%s: %d (%d)\n", i_level, ' ', i_level, i_dir, numObject,
				objects);
	}
	else
		fprintf(i_fp, "Root: %d (%d)\n", numObject, objects);

	return(numObject);
}
#endif

/***************************************************************************/
/***************************************************************************/
void CStaticCollisionObject::Fixup(void)
{
#ifdef GCN
	int	type = Type( );

	ByteSwap( type);
	switch( type)
#else
	switch(Type())
#endif
	{
	 case CCollisionObject::OBB_OBJECT:
		 Obb().Fixup();
		break;
	 default:
		ASSERTS(false, "Unsupported object type fixup");
	}
}

/***************************************************************************/
/***************************************************************************/
uint16 CStaticCollisionObject::Color(void)
{
	switch(Type())
	{
	 case CCollisionObject::OBB_OBJECT:
		return(Obb().Color());
	 default:
		ASSERTS(false, "Unsupported object type color");
	}
	return(0xffff);
}

/***************************************************************************/
/***************************************************************************/
uint8 CStaticCollisionObject::Material(void)
{
	switch(Type())
	{
	 case CCollisionObject::OBB_OBJECT:
		return(Obb().Material());
	 default:
		ASSERTS(false, "Unsupported object type material");
	}
	return(0xff);
}

/***************************************************************************/
/***************************************************************************/
void CStaticCollisionObject::Draw(DrawUtility::COLOR i_color)
{
	switch(Type())
	{
	 case CCollisionObject::OBB_OBJECT:
		Obb().Draw(i_color);
		break;
	 default:
		ASSERTS(false, "Unsupported object type draw");
	}
}

/***************************************************************************/
/***************************************************************************/
bool CStaticCollisionObject::Validate(XZRect &i_rect)
{
	switch(Type())
	{
	 case CCollisionObject::OBB_OBJECT:
	 {
		if (!i_rect.Inside(Obb().Center()->x(), Obb().Center()->z()))
		{
			ASSERTS(false, "Object Quad Tree: Object not in rect (safe to ignore)");
			return(false);
		}

		if (!Obb().Matrix()->Validate(1.0e-5f))
		{
			ASSERTS(false, "Object Quad Tree: Object matrix not orthonormal (safe to ignore)");
			return(false);
		}
		return(true);
	 }
	 default:
		ASSERTS(false, "Unsupported object type validate");
	}
	return(false);
}
