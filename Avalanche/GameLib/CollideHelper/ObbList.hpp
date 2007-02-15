/*
**
**  File:   ObbList.hpp
**  Date:   March 15, 2003
**  By:     Bryant Collard
**  Desc:   Extract OBBs from a collision event list.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/CollideHelper/ObbList.hpp $
**  $Revision: 1 $
**      $Date: 3/20/03 8:11p $
**    $Author: Bcollard $
**
*/

#ifdef OBB_LIST_HPP
#undef OBB_LIST_HPP

#include "collide/Event.h"
#include "collide/Pair.h"
#include "collide/Object.h"
#include "collide/obb.h"

/**********************************************************************
 **********************************************************************/
inline CObbList::CObbList()
{
	d_event = NULL;
	d_pair = NULL;
}

/**********************************************************************
 **********************************************************************/
inline OBB* CObbList::GetHead(CCollisionList* i_list)
{
	if (i_list == NULL)
		return(NULL);

	d_event = i_list->GetHeadEvent();
	while (d_event != NULL)
	{
		d_pair = d_event->GetHeadPair();
		while (d_pair != NULL)
		{
			CCollisionObject* object = d_pair->GetTargetObject();
			if (object->GetObjectType() == CCollisionObject::OBB_OBJECT)
				return((OBB*)object);
			d_pair = d_pair->GetNext();
		}
		d_event = d_event->GetNext();
	}

	return(NULL);
}

/**********************************************************************
 **********************************************************************/
inline OBB* CObbList::GetNext(void)
{
	ASSERT((d_event != NULL) && (d_pair != NULL));

	while ((d_pair = d_pair->GetNext()) != NULL)
	{
		CCollisionObject* object = d_pair->GetTargetObject();
		if (object->GetObjectType() == CCollisionObject::OBB_OBJECT)
			return((OBB*)object);
	}

	while ((d_event = d_event->GetNext()) != NULL)
	{
		d_pair = d_event->GetHeadPair();
		while (d_pair != NULL)
		{
			CCollisionObject* object = d_pair->GetTargetObject();
			if (object->GetObjectType() == CCollisionObject::OBB_OBJECT)
				return((OBB*)object);
		}
	}

	return(NULL);
}

#endif // OBBLIST_HPP
