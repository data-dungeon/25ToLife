/*
**
**  File:   Event.cpp
**  Date:   March 6, 2003
**  By:     Bryant Collard
**  Desc:   A pairing of colliding stage entities.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/Event.cpp $
**  $Revision: 8 $
**      $Date: 7/25/03 11:41a $
**    $Author: Bcollard $
**
*/

// Pre-compiled header //
#include "collide/CollidePCH.h"

// My include (always first) //
#include "collide/Event.h"

// Gamelib includes //
#include "collide/Node.h"
#include "collide/Pair.h"

/**********************************************************************/
/**********************************************************************/
CCollisionEvent* CCollisionEvent::TestEntityPair(CStageEntity* i_queryEntity,
		CStageEntity* i_targetEntity, CCollisionStack* i_stack,
		CStageEntity::ESelect i_select)
{
/*
 *	See if the two entities can interact at all.
 */
	if ((i_queryEntity == i_targetEntity) ||
			!i_queryEntity->CanInteract(i_targetEntity->Category()) ||
			!i_targetEntity->CanInteract(i_queryEntity->Category()))
		return(NULL);

	bool accept = false;
	CCollisionEvent* event = NULL;
	switch((i_queryEntity->ComplexBit(i_select) << 1) |
			i_targetEntity->ComplexBit(i_select))
	{
	 case 0:
		accept = TestSimplePair(i_queryEntity, i_targetEntity, event, i_stack);
		break;
	 case 1:
		accept = TestMixedPair(i_targetEntity, i_queryEntity, false, event,
				i_stack, i_select);
		break;
	 case 2:
		accept = TestMixedPair(i_queryEntity, i_targetEntity, true, event,
				i_stack, i_select);
		break;
	 case 3:
		accept = TestComplexPair(i_queryEntity, i_targetEntity, event, i_stack,
				i_select);
		break;
	 default:
		ASSERT(false);
	}
/*
 *	If no event was created, return.
 */
	if (event == NULL)
		return(NULL);
/*
 *	If the event is to be ignored or is empty, delete it and return.
 */
	if (!accept || (event->d_head == NULL))
	{
		CCollisionStackItem::Delete(event);
		return(NULL);
	}
/*
 *	Fill in entities and return the result.
 */
	event->d_queryEntity = i_queryEntity;
	event->d_targetEntity = i_targetEntity;
	return(event);
}

/**********************************************************************/
/**********************************************************************/
bool CCollisionEvent::TestSimplePair(CStageEntity* i_queryEntity,
		CStageEntity* i_targetEntity, CCollisionEvent* &o_event,
		CCollisionStack* i_stack)
{
/*
 *	See if one entity ignores the other.
 */
	if (i_queryEntity->IgnoreEntity(i_targetEntity) ||
			i_targetEntity->IgnoreEntity(i_queryEntity))
		return(false);
/*
 *	Create the event.
 */
	o_event = CCollisionEvent::New(i_stack);
	if (o_event == NULL)
		return(false);
/*
 *	Get the collision objects.
 */
	CCollisionObject* queryObject = i_queryEntity->GetCollisionObject();
	CCollisionObject* targetObject = i_targetEntity->GetCollisionObject();
/*
 *	Test for an intersection.
 */
	CCollisionIntersection* intersection =
			CCollisionPair::TestObjectPair(queryObject, targetObject, i_stack);
	if (intersection != NULL)
	{
/*
 *		Add a pair to the list. Make it seem as if no collision happened if we
 *		run out of memory.
 */
		CCollisionPair* pair = CCollisionPair::New(queryObject,
				targetObject, intersection, true, i_stack);
		if (pair == NULL)
		{
			CCollisionStackItem::Delete(intersection);
			return(false);
		}
		o_event->GetHeadPair() = pair->AddToEvent(o_event, o_event->GetHeadPair(),
				true);
	}
	return(true);
}

/**********************************************************************/
/**********************************************************************/
bool CCollisionEvent::TestMixedPair(CStageEntity* i_complexEntity,
		CStageEntity* i_simpleEntity, bool i_queryComplex,
		CCollisionEvent* &o_event, CCollisionStack* i_stack,
		CStageEntity::ESelect i_select)
{
/*
 *	See if the simple entity ignores the complex.
 */
	if (i_simpleEntity->IgnoreEntity(i_complexEntity))
		return(false);
/*
 *	Get the root collision node from the complex entity.
 */
	CCollisionNode* node = i_complexEntity->BeginCollisionTest(i_simpleEntity,
			i_select);
	if (node == NULL)
		return(false);
/*
 *	Create the event.
 */
	o_event = CCollisionEvent::New(i_stack);
	if (o_event == NULL)
	{
		i_complexEntity->EndCollisionTest(NULL);
		return(false);
	}
/*
 *	Test the pair.
 */
	o_event->Test(node, i_simpleEntity->GetCollisionObject(), i_queryComplex,
			i_stack);
/*
 *	Tell the complex entity that the test is complete and give it a chance
 *	to ignore it.
 */
	return(i_complexEntity->EndCollisionTest(o_event));
}

/**********************************************************************/
/**********************************************************************/
void CCollisionEvent::Test(CCollisionNode* i_node,
		CCollisionObject* i_entityObject, bool i_queryNode,
		CCollisionStack* i_stack)
{
/*
 *	Get the node's collision object. If it is not NULL, perform an intersection
 *	test.
 */
	CCollisionObject* nodeObject = i_node->BeginObjectTest();
	if (nodeObject != NULL)
	{
/*
 *		Test the object pair. Make sure the query object is the first argument
 *		so it can be identified in the intersection object if the two objects
 *		are of the same type.
 */
		CCollisionIntersection* intersection;
		if (i_queryNode)
			intersection = CCollisionPair::TestObjectPair(nodeObject,
					i_entityObject, i_stack);
		else
			intersection = CCollisionPair::TestObjectPair(i_entityObject,
					nodeObject, i_stack);
/*
 *		No intersection.
 */
		if (intersection == NULL)
		{
			i_node->EndObjectTest(CCollisionNode::NO_INTERSECTION);
		}
/*
 *		An intersection was detected.
 */
		else
		{
/*
 *			See if the node wants to ignore this collision. This may be the case,
 *			for example, if the node served an object to cull before serving more
 *			refined objects. If it does, release the intersection object. Also,
 *			tell the query node that an intersection was detected but ignored.
 */
			if (i_node->IgnoreIntersection(intersection))
			{
				CCollisionStackItem::Delete(intersection);
				i_node->EndObjectTest(CCollisionNode::INTERSECTION_IGNORED);
			}
/*
 *			A collision of interest has been detected.
 */
			else
			{
/*
 *				Add a pair to the list. Make it seem as if no collision
 *				happened if we run out of memory.
 */
				CCollisionPair* pair;
				if (i_queryNode)
				{
					pair = CCollisionPair::New(nodeObject, i_entityObject,
							intersection, true, i_stack);
				}
				else
				{
					pair = CCollisionPair::New(i_entityObject, nodeObject,
							intersection, true, i_stack);
				}
				if (pair == NULL)
				{
					CCollisionStackItem::Delete(intersection);
					i_node->EndObjectTest(CCollisionNode::INTERSECTION_IGNORED);
					return;
				}
				d_head = pair->AddToEvent(this, d_head, true);
				i_node->EndObjectTest(CCollisionNode::INTERSECTION_ACCEPTED, pair);
			}
		}
	}
/*
 * Test the entity against each of the node's children.
 */
	CCollisionNode* child;
	while ((child = i_node->GetChild()) != NULL)
	{
		Test(child, i_entityObject, i_queryNode, i_stack);
	}
}

/**********************************************************************/
/**********************************************************************/
bool CCollisionEvent::TestComplexPair(CStageEntity* i_queryEntity,
		CStageEntity* i_targetEntity, CCollisionEvent* &o_event,
		CCollisionStack* i_stack, CStageEntity::ESelect i_select)
{
/*
 *	Get the root collision node from the query entity.
 */
	CCollisionNode* queryNode =
			i_queryEntity->BeginCollisionTest(i_targetEntity, i_select);
	if (queryNode == NULL)
		return(false);
/*
 *	Get the root collision node from the target entity.
 */
	CCollisionNode* targetNode =
			i_targetEntity->BeginCollisionTest(i_queryEntity, i_select);
	if (targetNode == NULL)
	{
		i_queryEntity->EndCollisionTest(NULL);
		return(false);
	}
/*
 *	Create the event.
 */
	o_event = CCollisionEvent::New(i_stack);
	if (o_event == NULL)
	{
		i_queryEntity->EndCollisionTest(NULL);
		i_targetEntity->EndCollisionTest(NULL);
		return(false);
	}
/*
 *	Test the nodes.
 */
	o_event->Test(queryNode, targetNode, i_stack);
/*
 *	Tell each of the entities that the test is complete and give them a chance
 *	to ignore it.
 */
	bool queryAccept = i_queryEntity->EndCollisionTest(o_event);
	bool targetAccept = i_targetEntity->EndCollisionTest(o_event);
	return(queryAccept && targetAccept);
}

/**********************************************************************/
/**********************************************************************/
void CCollisionEvent::Test(CCollisionNode* i_queryNode,
		CCollisionNode* i_targetNode, CCollisionStack* i_stack)
{
/*
 *	Get the collision object for this, the query node. If it is NULL,
 *	no collision object intersection test is to be performed.
 */
	CCollisionObject* queryObject = i_queryNode->BeginObjectTest();
	if (queryObject == NULL)
	{
/*
 * 	The query node does not have an object. Test the target node against each
 *		of the query node's children. If the target node has an object, it is
 *		assumed that the children served by it depend on the results of any
 *		collision checks with its object. It is best to test each of the query
 *		node's children against the target node because this allows tests between
 *		the query node children and the target node children to be avoided if the
 *		target node decides to. If the target node does not have an object, there
 *		is no clear choice for testing order so just do query node childrem
 *		against the target node.
 */
		CCollisionNode* child;
		while ((child = i_queryNode->GetChild()) != NULL)
		{
			Test(child, i_targetNode, i_stack);
		}
		return;
	}
/*
 *	The query node has a collision object.
 *
 *	Get the collision object for the target node. If it is NULL, tell the
 *	query node no test was performed.
 */
	CCollisionObject* targetObject = i_targetNode->BeginObjectTest();
	if (targetObject == NULL)
	{
		i_queryNode->EndObjectTest(CCollisionNode::NO_TEST);
/*
 *		The target node does not have an object. Since the query node has an
 *		object, it is assumed that the children served by it depend on the
 *		results of any collision checks with its object. It is best to test
 *		each of the target node's children against the query node because this
 *		allows tests between the target node children and the query node
 *		children to be avoided if the query node decides to.
 */
		CCollisionNode* child;
		while ((child = i_targetNode->GetChild()) != NULL)
		{
			Test(i_queryNode, child, i_stack);
		}
		return;
	}
/*
 *	Both nodes have collision objects. Test for an intersection. If one is not
 *	detected, tell the nodes.
 */
	CCollisionIntersection* intersection =
			CCollisionPair::TestObjectPair(queryObject, targetObject, i_stack);
	if (intersection == NULL)
	{
		i_queryNode->EndObjectTest(CCollisionNode::NO_INTERSECTION);
		i_targetNode->EndObjectTest(CCollisionNode::NO_INTERSECTION);
	}
/*
 *	An intersection was detected.
 */
	else
	{
/*
 *		See if either node wants to ignore this collision. This may be the case,
 *		for example, if an node served an object to cull before serving more
 *		refined objects. If they do, release the intersection object. Also, tell
 *		the nodes that an intersection was detected but ignored.
 */
		if (i_queryNode->IgnoreIntersection(intersection) ||
				i_targetNode->IgnoreIntersection(intersection))
		{
			CCollisionStackItem::Delete(intersection);
			i_queryNode->EndObjectTest(CCollisionNode::INTERSECTION_IGNORED);
			i_targetNode->EndObjectTest(CCollisionNode::INTERSECTION_IGNORED);
		}
/*
 *		A collision of interest has been detected.
 */
		else
		{
/*
 *			Add a pair to the list. Make it seem as if no collision
 *			happened if we run out of memory.
 */
			CCollisionPair* pair = CCollisionPair::New(queryObject, targetObject,
					intersection, true, i_stack);
			if (pair == NULL)
			{
				CCollisionStackItem::Delete(intersection);
				i_queryNode->EndObjectTest(CCollisionNode::INTERSECTION_IGNORED);
				i_targetNode->EndObjectTest(CCollisionNode::INTERSECTION_IGNORED);
				return;
			}
			d_head = pair->AddToEvent(this, d_head, true);
/*
 *			Tell the nodes an intersection was detected.
 */
			i_queryNode->EndObjectTest(CCollisionNode::INTERSECTION_ACCEPTED,
					pair);
			i_targetNode->EndObjectTest(CCollisionNode::INTERSECTION_ACCEPTED,
					pair);
		}
	}
/*
 *	Both nodes have a collision object and they have been tested for
 *	an intersection. If the query node has children, check each of them
 *	against the target node. If the target node has children, they will get
 *	checked as part of this call.
 */
	CCollisionNode* child = i_queryNode->GetChild();
	if (child != NULL)
	{
		do
		{
			Test(child, i_targetNode, i_stack);
		} while ((child = i_queryNode->GetChild()) != NULL);
		return;
	}
/*
 *	If the query node does not have children, test the query node against
 *	each of the target node's children.
 */
	while ((child = i_targetNode->GetChild()) != NULL)
	{
		Test(i_queryNode, child, i_stack);
	}
}

#ifdef DEBUG_STACK

#include <string.h>

/***************************************************************************/
/***************************************************************************/
const char* CCollisionEvent::GetName(void)
{
	return("Event");
}

/***************************************************************************/
/***************************************************************************/
void CCollisionEvent::AppendDescription(char* io_msg)
{
	char msg[14];
	strcat(io_msg, " (Query: ");
	if (d_queryEntity == NULL) 
		strcat(io_msg, "NULL");
	else if (!CStageEntity::IsValid(d_queryEntity))
		strcat(io_msg, "INVALID");
	else
	{
		strcat(io_msg, d_queryEntity->GetClassName());
		d_queryEntity->AppendDescription(io_msg);
	}
	sprintf(msg, "[0x%08x]", d_queryEntity);
	strcat(io_msg, msg);

	strcat(io_msg, ", Target: ");
	if (d_targetEntity == NULL) 
		strcat(io_msg, "NULL");
	else if (!CStageEntity::IsValid(d_targetEntity))
		strcat(io_msg, "INVALID");
	else
	{
		strcat(io_msg, d_targetEntity->GetClassName());
		d_targetEntity->AppendDescription(io_msg);
	}
	sprintf(msg, "[0x%08x])", d_targetEntity);
	strcat(io_msg, msg);
}

#endif
