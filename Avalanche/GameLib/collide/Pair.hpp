/*
**
**  File:   Pair.hpp
**  Date:   March 6, 2003
**  By:     Bryant Collard
**  Desc:   A pairing of collision objects.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/collide/Pair.hpp $
**  $Revision: #11 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#ifdef COLLISION_PAIR_HPP
#undef COLLISION_PAIR_HPP

#include "collide/Object.h"
#include "collide/LineToTerrain.h"
#include "collide/LineToObb.h"
#include "collide/LineToAabb.h"
#include "collide/LineToSphere.h"
#include "collide/ObbToTerrain.h"
#include "collide/ObbToObb.h"
#include "collide/AabbToTerrain.h"
#include "collide/AabbToObb.h"
#include "collide/AabbToAabb.h"
#include "collide/SphereToTerrain.h"
#include "collide/SphereToObb.h"
#include "collide/LineToTriangleList.h"

/**********************************************************************
 **********************************************************************/
inline CCollisionPair::CCollisionPair(CCollisionObject* i_queryObject,
		CCollisionObject* i_targetObject, CCollisionIntersection* i_intersection,
		bool i_pairOwnsIntersection)
{
	d_queryObject = i_queryObject;
	d_targetObject = i_targetObject;
	d_intersection = i_intersection;
	d_event = NULL;
	d_prev = NULL;
	d_next = NULL;
	d_pairOwnsIntersection = i_pairOwnsIntersection;
	d_eventOwnsPair = false;
}

/**********************************************************************
 **********************************************************************/
inline CCollisionPair::CCollisionPair(CCollisionObject* i_queryObject,
		CCollisionObject* i_targetObject, CCollisionIntersection* i_intersection,
		bool i_pairOwnsIntersection, CCollisionStack &i_stack) :
	CCollisionStackItem(i_stack)
{
	d_queryObject = i_queryObject;
	d_targetObject = i_targetObject;
	d_intersection = i_intersection;
	d_event = NULL;
	d_prev = NULL;
	d_next = NULL;
	d_pairOwnsIntersection = i_pairOwnsIntersection;
	d_eventOwnsPair = false;
}

/**********************************************************************
 **********************************************************************/
inline CCollisionPair::~CCollisionPair()
{
	if (d_pairOwnsIntersection)
		CCollisionStackItem::Delete(d_intersection);
}

/**********************************************************************
 **********************************************************************/
inline CCollisionPair* CCollisionPair::New(CCollisionObject* i_queryObject,
		CCollisionObject* i_targetObject, CCollisionIntersection* i_intersection,
		bool i_pairOwnsIntersection, CCollisionStack* i_stack)
{
	CCollisionPair* pair;

	if (i_stack != NULL)
	{
		pair = (CCollisionPair*)i_stack->Allocate(sizeof(CCollisionPair));

		if (pair)
			new(pair) CCollisionPair(i_queryObject, i_targetObject, i_intersection,
					i_pairOwnsIntersection, *i_stack);
	}
	else
		pair = new CCollisionPair(i_queryObject, i_targetObject, i_intersection,
			i_pairOwnsIntersection);

	return(pair);
}


/**********************************************************************
 **********************************************************************/
inline CCollisionPair* CCollisionPair::AddToEvent(CCollisionEvent* i_event,
		CCollisionPair* i_head, bool i_eventOwnsPair)
{
	ASSERT((d_event == NULL) && (d_prev == NULL) && (d_next == NULL));

	d_event = i_event;
	d_prev = NULL;
	d_next = i_head;
	d_eventOwnsPair = i_eventOwnsPair;
	return(this);
}

/**********************************************************************
 **********************************************************************/
inline CCollisionPair* CCollisionPair::MoveToEvent(CCollisionEvent* i_event,
		CCollisionPair* &o_oldHead, CCollisionPair* i_newHead)
{
	ASSERT((d_prev != NULL) || (o_oldHead == this));

	if (d_prev != NULL)
		d_prev->d_next = d_next;
	else
		o_oldHead = d_next;

	if (d_next != NULL)
		d_next->d_prev = d_prev;

	d_event = i_event;
	d_prev = NULL;
	d_next = i_newHead;
	return(this);
}

/**********************************************************************
 **********************************************************************/
inline CCollisionPair* CCollisionPair::MoveAllToEvent(CCollisionEvent* i_event,
		CCollisionPair* &o_oldHead, CCollisionPair* i_newHead)
{
	ASSERT((d_prev != NULL) || (o_oldHead == this));

	d_event = i_event;
	CCollisionPair* pair = this;
	while (pair->d_next != NULL)
	{
		pair = pair->d_next;
		pair->d_event = i_event;
	}

	if (d_prev != NULL)
		d_prev->d_next = NULL;
	else
		o_oldHead = NULL;

	d_prev = NULL;
	pair->d_next = i_newHead;
	return(this);
}

#endif // COLLISION_PAIR_HPP
