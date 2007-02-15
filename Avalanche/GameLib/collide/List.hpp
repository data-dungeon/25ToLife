/*
**
**  File:   List.hpp
**  Date:   March 6, 2003
**  By:     Bryant Collard
**  Desc:   A pairing of colliding stage entities.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/collide/List.hpp $
**  $Revision: #4 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#ifdef COLLISION_LIST_HPP
#undef COLLISION_LIST_HPP

// System includes //
#include <new.h>

// Engine includes //
#include "Layers/Assert/Assert.h"

// Gamelib includes //
#include "collide/Event.h"

/**********************************************************************
 **********************************************************************/
inline CCollisionList::CCollisionList()
{
	d_head = NULL;
}

/**********************************************************************
 **********************************************************************/
inline CCollisionList::CCollisionList(CCollisionStack &i_stack) :
	CCollisionStackItem(i_stack)
{
	d_head = NULL;
}

/**********************************************************************
 **********************************************************************/
inline CCollisionList::~CCollisionList()
{
	while(d_head != NULL)
	{
		if (d_head->ListOwnsEvent())
		{
			CCollisionEvent* event = d_head;
			d_head = d_head->GetNext();
			CCollisionStackItem::Delete(event);
		}
		else
			d_head = d_head->GetNext();
	}
}

/**********************************************************************
 **********************************************************************/
inline CCollisionList* CCollisionList::New(CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CCollisionList* list = (CCollisionList*)i_stack->Allocate(
				sizeof(CCollisionList));

		if (list)
			new(list) CCollisionList(*i_stack);

		return(list);
	}
	return(new CCollisionList);
}

/**********************************************************************
 **********************************************************************/
inline void CCollisionList::AddEvent(CCollisionEvent* i_event,
		bool i_listOwnsEvent)
{
	ASSERT(i_event != NULL);

	d_head = i_event->AddToList(this, d_head, i_listOwnsEvent);
}

/**********************************************************************
 **********************************************************************/
inline bool CCollisionList::RemoveEvent(CCollisionEvent* i_event)
{
	ASSERT(i_event != NULL);

	bool success = i_event->RemoveFromList(this, d_head);
	if (success && i_event->ListOwnsEvent())
		CCollisionStackItem::Delete(i_event);

	return(success);
}

#endif // COLLISION_LIST_HPP
