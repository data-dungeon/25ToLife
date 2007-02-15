/*
**
**  File:   List.h
**  Date:   March 6, 2003
**  By:     Bryant Collard
**  Desc:   A pairing of colliding stage entities.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/collide/List.h $
**  $Revision: #7 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#ifndef COLLISION_LIST_H
#define COLLISION_LIST_H

// Gamelib includes //
#include "collide/Stack.h"

class CCollisionEvent;

class CCollisionList : public CCollisionStackItem
{
  public:
	CCollisionList();
	~CCollisionList();

	// Allocate a new list object.
	static CCollisionList* New(CCollisionStack* i_stack);

	void AddEvent(CCollisionEvent* i_event, bool i_listOwnsEvent = false);
	bool RemoveEvent(CCollisionEvent* i_event);

	bool Empty(void) {return(d_head == NULL);}
	CCollisionEvent* GetHeadEvent(void) {return(d_head);}

#ifdef DEBUG_STACK
	virtual const char* GetName(void) {return("List");}
#endif

  private:
	CCollisionList(CCollisionStack &i_stack);

	CCollisionEvent* d_head;
};

#define COLLISION_LIST_HPP
#include "collide/List.hpp"

#endif // COLLISION_LIST_H
