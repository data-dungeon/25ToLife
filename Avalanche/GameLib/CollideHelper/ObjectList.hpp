/*
**
**  File:   ObjectList.hpp
**  Date:   March 15, 2003
**  By:     Bryant Collard
**  Desc:   Extract OBBs from a collision event list.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/CollideHelper/ObjectList.hpp $
**  $Revision: 3 $
**      $Date: 6/20/03 2:38p $
**    $Author: Bcollard $
**
*/

#ifdef OBJECT_LIST_HPP
#undef OBJECT_LIST_HPP

#include "collide/Event.h"
#include "collide/Pair.h"

/**********************************************************************/
/**********************************************************************/
inline CObjectList::CObjectList()
{
	d_numObjects = 0;
	d_maxObjects = 0;
	d_objects = NULL;
}

/**********************************************************************/
/**********************************************************************/
inline void CObjectList::Clear(void)
{
	d_numObjects = 0;
}

/**********************************************************************/
/**********************************************************************/
inline bool CObjectList::Add(CCollisionEvent* i_event)
{
	if (i_event != NULL)
	{
		CCollisionPair* pair = i_event->GetHeadPair();
		while (pair != NULL)
		{
			if (d_numObjects >= d_maxObjects)
				return(false);
			d_objects[d_numObjects++] = pair->GetTargetObject();
			pair = pair->GetNext();
		}
	}
	return(true);
}

/**********************************************************************/
/**********************************************************************/
inline bool CObjectList::Add(CCollisionEvent* i_event, CStageEntity* i_subject)
{
	if (i_event != NULL)
	{
		if (i_subject == i_event->GetQueryEntity())
		{
			CCollisionPair* pair = i_event->GetHeadPair();
			while (pair != NULL)
			{
				if (d_numObjects >= d_maxObjects)
					return(false);
				d_objects[d_numObjects++] = pair->GetTargetObject();
				pair = pair->GetNext();
			}
		}
		else if (i_subject == i_event->GetTargetEntity())
		{
			CCollisionPair* pair = i_event->GetHeadPair();
			while (pair != NULL)
			{
				if (d_numObjects >= d_maxObjects)
					return(false);
				d_objects[d_numObjects++] = pair->GetQueryObject();
				pair = pair->GetNext();
			}
		}
	}
	return(true);
}

/**********************************************************************/
/**********************************************************************/
inline bool CObjectList::Add(CCollisionList* i_list)
{
	if (i_list == NULL)
		return(true);

	CCollisionEvent* event = i_list->GetHeadEvent();
	while (event != NULL)
	{
		CCollisionPair* pair = event->GetHeadPair();
		while (pair != NULL)
		{
			if (d_numObjects >= d_maxObjects)
				return(false);
			d_objects[d_numObjects++] = pair->GetTargetObject();
			pair = pair->GetNext();
		}
		event = event->GetNext();
	}

	return(true);
}

/**********************************************************************/
/**********************************************************************/
inline bool CObjectList::Add(CCollisionList* i_list, CStageEntity* i_subject)
{
	if (i_list == NULL)
		return(true);

	CCollisionEvent* event = i_list->GetHeadEvent();
	while (event != NULL)
	{
		if (i_subject == event->GetQueryEntity())
		{
			CCollisionPair* pair = event->GetHeadPair();
			while (pair != NULL)
			{
				if (d_numObjects >= d_maxObjects)
					return(false);
				d_objects[d_numObjects++] = pair->GetTargetObject();
				pair = pair->GetNext();
			}
		}
		else if (i_subject == event->GetTargetEntity())
		{
			CCollisionPair* pair = event->GetHeadPair();
			while (pair != NULL)
			{
				if (d_numObjects >= d_maxObjects)
					return(false);
				d_objects[d_numObjects++] = pair->GetQueryObject();
				pair = pair->GetNext();
			}
		}
		event = event->GetNext();
	}

	return(true);
}

/**********************************************************************/
/**********************************************************************/
inline bool CObjectList::Add(CCollisionObject* i_object)
{
	if (i_object == NULL)
		return(true);
	if (d_numObjects >= d_maxObjects)
		return(false);
	d_objects[d_numObjects++] = i_object;
	return(true);
}

/**********************************************************************/
/**********************************************************************/
inline CCollisionObject* CObjectList::GetNext(void)
{
	if (++d_currObject < d_numObjects)
		return(d_objects[d_currObject]);
	return(NULL);
}

/**********************************************************************/
/**********************************************************************/
inline CCollisionObject* CObjectList::GetNext(int i_type)
//		CCollisionObject::ECollisionObjectType i_type)
{
	while (++d_currObject < d_numObjects)
	{
		if (d_objects[d_currObject]->GetObjectType() ==
				(CCollisionObject::ECollisionObjectType)i_type)
			return(d_objects[d_currObject]);
	}
	return(NULL);
}

/**********************************************************************/
/**********************************************************************/
inline CCollisionObject* CObjectList::GetHead(void)
{
	d_currObject = -1;
	return(GetNext());
}

/**********************************************************************/
/**********************************************************************/
inline CCollisionObject* CObjectList::GetHead(int i_type)
//		CCollisionObject::ECollisionObjectType i_type)
{
	d_currObject = -1;
	return(GetNext(i_type));
}

/***************************************************************************/
/***************************************************************************/
template <int SIZE>
inline CObjectListArray<SIZE>::CObjectListArray()
{
	d_maxObjects = SIZE;
	d_objects = d_objectArray;
}

#endif // OBJECT_LIST_HPP
