/*
**
**  File:   Pair.h
**  Date:   March 6, 2003
**  By:     Bryant Collard
**  Desc:   A pairing of collision objects.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/collide/Pair.h $
**  $Revision: #9 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#ifndef COLLISION_PAIR_H
#define COLLISION_PAIR_H

#include "collide/Stack.h"
#include "collide/Intersection.h"

class CCollisionObject;
class CCollisionEvent;

class CCollisionPair : public CCollisionStackItem
{
  public:
	CCollisionPair(CCollisionObject* i_queryObject,
			CCollisionObject* i_targetObject,
			CCollisionIntersection* i_intersection, bool i_pairOwnsIntersection);
	~CCollisionPair();

	static CCollisionPair* New(CCollisionObject* i_queryObject,
			CCollisionObject* i_targetObject,
			CCollisionIntersection* i_intersection,
			bool i_pairOwnsIntersection, CCollisionStack* i_stack);

	static CCollisionIntersection* TestObjectPair(
			CCollisionObject* i_queryObject, CCollisionObject* i_targetObject,
			CCollisionStack* i_stack);

	CCollisionPair* AddToEvent(CCollisionEvent* i_event, CCollisionPair* i_head,
			bool i_eventOwnsPair = false);
	CCollisionPair* MoveToEvent(CCollisionEvent* i_event,
			CCollisionPair* &o_oldHead, CCollisionPair* i_newHead);
	CCollisionPair* MoveAllToEvent(CCollisionEvent* i_event,
			CCollisionPair* &o_oldHead, CCollisionPair* i_newHead);
	CCollisionEvent* GetEvent(void) const {return(d_event);}
	bool EventOwnsPair(void) const {return(d_eventOwnsPair);}
	CCollisionPair* GetPrev(void) const {return(d_prev);}
	CCollisionPair* GetNext(void) const {return(d_next);}

	CCollisionObject* GetQueryObject(void) const {return(d_queryObject);}
	CCollisionObject* GetTargetObject(void) const {return(d_targetObject);}

	CCollisionIntersection* GetIntersection(void) const {return(d_intersection);}
	CCollisionIntersection::ECollisionIntersectionType
			GetIntersectionType(void) const
		{return(d_intersection->GetIntersectionType());}

#ifdef DEBUG_STACK
	virtual const char* GetName(void) {return("Pair");}
#endif

  private:
	CCollisionPair();
	CCollisionPair(CCollisionObject* i_queryObject,
			CCollisionObject* i_targetObject,
			CCollisionIntersection* i_intersection, bool i_pairOwnsIntersection,
			CCollisionStack &i_stack);

	CCollisionObject* d_queryObject;
	CCollisionObject* d_targetObject;
	CCollisionIntersection* d_intersection;
	CCollisionEvent* d_event;
	CCollisionPair* d_prev;
	CCollisionPair* d_next;
	bool d_pairOwnsIntersection;
	bool d_eventOwnsPair;
};

#define COLLISION_PAIR_HPP
#include "collide/Pair.hpp"

#endif // COLLISION_PAIR_H
