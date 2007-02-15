/*
**
**  File:   Event.hpp
**  Date:   March 6, 2003
**  By:     Bryant Collard
**  Desc:   A pairing of colliding stage entities.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/collide/Event.hpp $
**  $Revision: #15 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#ifdef COLLISION_EVENT_HPP
#undef COLLISION_EVENT_HPP

// System includes //
#include <new.h>

// Engine includes //
#include "Layers/Assert/Assert.h"

// Gamelib includes //
#include "collide/Pair.h"

/**********************************************************************/
/**********************************************************************/
inline CCollisionEvent::CCollisionEvent()
{
	d_queryEntity = NULL;
	d_targetEntity = NULL;
	d_head = NULL;
	d_list = NULL;
	d_prev = NULL;
	d_next = NULL;
	d_listOwnsEvent = false;
}

/**********************************************************************/
/**********************************************************************/
inline CCollisionEvent::CCollisionEvent(CCollisionStack &i_stack) :
	CCollisionStackItem(i_stack)
{
	d_queryEntity = NULL;
	d_targetEntity = NULL;
	d_head = NULL;
	d_list = NULL;
	d_prev = NULL;
	d_next = NULL;
	d_listOwnsEvent = false;
}

/**********************************************************************/
/**********************************************************************/
inline CCollisionEvent::~CCollisionEvent()
{
	while(d_head != NULL)
	{
		if (d_head->EventOwnsPair())
		{
			CCollisionPair* pair = d_head;
			d_head = d_head->GetNext();
			CCollisionStackItem::Delete(pair);
		}
		else
			d_head = d_head->GetNext();
	}
}

/**********************************************************************/
/**********************************************************************/
inline CCollisionEvent* CCollisionEvent::New(CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CCollisionEvent* event = (CCollisionEvent*)i_stack->Allocate(
				sizeof(CCollisionEvent));

		if (event)
			new(event) CCollisionEvent(*i_stack);

		return(event);
	}
	return(new CCollisionEvent());
}

/**********************************************************************/
/**********************************************************************/
inline void CCollisionEvent::SetEntities(CStageEntity* i_queryEntity,
		CStageEntity* i_targetEntity)
{
	d_queryEntity = i_queryEntity;
	d_targetEntity = i_targetEntity;
}

/**********************************************************************/
/**********************************************************************/
inline CCollisionEvent* CCollisionEvent::AddToList(CCollisionList* i_list,
		CCollisionEvent* i_head, bool i_listOwnsEvent)
{
	ASSERT((d_list == NULL) && (d_prev == NULL) && (d_next == NULL));

	d_list = i_list;
	d_prev = NULL;
	d_next = i_head;
	d_listOwnsEvent = i_listOwnsEvent;

	if (d_next != NULL)
		d_next->d_prev = this;

	return(this);
}

/**********************************************************************/
/**********************************************************************/
inline bool CCollisionEvent::RemoveFromList(CCollisionList* i_list,
		CCollisionEvent* &i_head)
{
	if (i_list != d_list)
		return(false);

	if (d_next != NULL)
		d_next->d_prev = d_prev;

	if (d_prev != NULL)
		d_prev->d_next = d_next;
	else
		i_head = d_next;

	d_list = NULL;
	d_prev = NULL;
	d_next = NULL;

	return(true);
}

/**********************************************************************/
/**********************************************************************/
inline CCollisionEvent* CCollisionEvent::SetTempListNext(
		CCollisionEvent* i_tempListNext)
{
	d_tempListNext = i_tempListNext;
	return(this);
}

/**********************************************************************/
/**********************************************************************/
inline CCollisionEvent* CCollisionEvent::GetTempListNext(void)
{
	return(d_tempListNext);
}

#endif // COLLISION_EVENT_HPP
