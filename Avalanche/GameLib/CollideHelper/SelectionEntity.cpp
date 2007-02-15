/*
**
**  File:   SelectionEntity.cpp
**  Date:   September 2, 2005
**  By:     Bryant Collard
**  Desc:   A query entity that uses a provided entity to "select" collision
**          objects to be used in additional tests.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

// Pre-compiled header
#include "CollideHelper/CollideHelperPCH.h"

// My header (always first)
#include "CollideHelper/SelectionEntity.h"

// Gamelib includes
#include "collide/List.h"
#include "collide/Event.h"
#include "collide/Pair.h"
#include "collide/Object.h"
#include "collide/ObbToTerrain.h"
#include "collide/AabbToTerrain.h"
#include "collide/SphereToTerrain.h"

///////////////////////////// CSelectionChildNode //////////////////////////////

/******************************************************************************/
/******************************************************************************/
CSelectionChildNode::CSelectionChildNode()
{
	d_list = NULL;
	d_event = NULL;
	d_pair = NULL;
}

/******************************************************************************/
/******************************************************************************/
void CSelectionChildNode::Setup(CCollisionList* i_list)
{
	d_list = i_list;
	Reset();
}

/******************************************************************************/
/******************************************************************************/
void CSelectionChildNode::Reset(void)
{
	if (d_list != NULL)
		d_event = d_list->GetHeadEvent();
	else
		d_event = NULL;

	d_pair = NULL;
}

/******************************************************************************/
/******************************************************************************/
CCollisionNode* CSelectionChildNode::Next(void)
{
	while (d_event != NULL)
	{
		if (d_pair == NULL)
			d_pair = d_event->GetHeadPair();
		else
			d_pair = d_pair->GetNext();

		if (d_pair != NULL)
			return(this);

		d_event = d_event->GetNext();
	}
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
CCollisionObject* CSelectionChildNode::BeginObjectTest(void)
{
	if (d_pair != NULL)
	{
		CCollisionObject* object = d_pair->GetTargetObject();
		if ((object != NULL) &&
				(object->GetObjectType() == CCollisionObject::TERRAIN))
		{
			switch (d_pair->GetIntersectionType())
			{
			 case CCollisionIntersection::OBB_TO_TERRAIN:
				return(&((CObbToTerrainIntersection*)d_pair->GetIntersection(
						))->GetTriangleList());
			 case CCollisionIntersection::AABB_TO_TERRAIN:
				return(&((CAabbToTerrainIntersection*)d_pair->GetIntersection(
						))->GetTriangleList());
			 case CCollisionIntersection::SPHERE_TO_TERRAIN:
				return(&((CSphereToTerrainIntersection*)d_pair->GetIntersection(
						))->GetTriangleList());
			}
		}
		return(object);
	}
	return(NULL);
}

//////////////////////////////// CSelectionNode ////////////////////////////////

/******************************************************************************/
/******************************************************************************/
void CSelectionNode::Setup(CCollisionList* i_list)
{
	d_child.Setup(i_list);
	d_cullObject = NULL;
}

/******************************************************************************/
/******************************************************************************/
void CSelectionNode::BeginNodeTest(CCollisionObject* i_object)
{
	d_cullObject = i_object;
	d_child.Reset();
}

/******************************************************************************/
/******************************************************************************/
CCollisionNode* CSelectionNode::GetChild(void)
{
	return(d_child.Next());
}

/////////////////////////////// CSelectionEntity ///////////////////////////////

/******************************************************************************/
/******************************************************************************/
CSelectionEntity::CSelectionEntity()
{
	d_selector = NULL;
	d_useRootSelectorObject = false;
}

/******************************************************************************/
/******************************************************************************/
bool CSelectionEntity::Select(CStageEntity* i_selector, ESelect i_select,
		bool i_useRootSelectorObject)
{
	Clear();
	if (i_selector != NULL)
	{
		d_selector = i_selector;
		d_useRootSelectorObject = i_useRootSelectorObject;
		g_collisionSystem.ResetStack();
		CCollisionList* list = g_collisionSystem.Test(i_selector, i_select);
		if (list != NULL)
		{
			CCollisionEvent* event = list->GetHeadEvent();
			while (event != NULL)
			{
				d_category |= event->GetTargetEntity()->Category();
				event = event->GetNext();
			}
			d_lock.Set(g_collisionSystem.GetStack());
			d_node.Setup(list);
			return(true);
		}
	}
	return(false);
}

/******************************************************************************/
/******************************************************************************/
void CSelectionEntity::Clear(void)
{
	d_category = NO_CATEGORY;
	d_lock.Release();
	d_selector = NULL;
	d_useRootSelectorObject = false;
	d_node.Setup(NULL);
}

/******************************************************************************/
/******************************************************************************/
bool CSelectionEntity::GetCollisionCull(CCollisionCull &o_cull)
{
	if (d_selector != NULL)
		return(d_selector->GetCollisionCull(o_cull));
	return(false);
}

/******************************************************************************/
/******************************************************************************/
CCollisionNode* CSelectionEntity::BeginCollisionTest(CStageEntity* i_entity,
		ESelect i_select)
{
	if (d_selector != NULL)
	{
		CCollisionObject* rootObject = NULL;
		if (d_useRootSelectorObject)
		{
			CCollisionNode* rootNode = d_selector->BeginCollisionTest(i_entity,
					i_select);
			if (rootNode != NULL)
				rootObject = rootNode->BeginObjectTest();
		}
		d_node.BeginNodeTest(rootObject);
		return(&d_node);
	}
	return(NULL);
}
