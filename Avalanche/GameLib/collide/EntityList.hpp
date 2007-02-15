/*
**
**  File:   EntityList.hpp
**  Date:   March 14, 2003
**  By:     Bryant Collard
**  Desc:   Build a list of entites.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/becCollide/EntityList.hpp $
**  $Revision: 3 $
**      $Date: 4/11/03 4:01p $
**    $Author: Bcollard $
**
*/

#ifdef ENTITY_LIST_HPP
#undef ENTITY_LIST_HPP

#include "collide/StackArray.h"

/***************************************************************************/
// Create the list using a specified stack (NULL uses dynamic memory).
// Fails if a list already exists or if it cannot be created.
/***************************************************************************/
inline bool CEntityList::Create(CCollisionStack* i_stack)
{
	if (d_list != NULL)
		return(false);

	d_list = CCollisionStackArray<CStageEntity*>::New(i_stack);
	if (d_list == NULL)
		return(false);

	d_list->InitArray();
	return(true);
}

/***************************************************************************/
// Add an entity to the list. All entities should be added after the
// list is created and before any other routine uses the specified stack.
// Fails if no list exists, the existing list is closed or a new item
// cannot be added.
/***************************************************************************/
inline bool CEntityList::Add(CStageEntity* i_entity)
{
	if (d_list == NULL)
		return(false);

	CStageEntity** entity = d_list->ExpandArray();
	if (entity == NULL)
		return(false);

	*entity = i_entity;
	return(true);
}

/***************************************************************************/
// Close the list. The list will be destroyed if it is empty.
/***************************************************************************/
inline CCollisionStackArray<CStageEntity*>* CEntityList::Close(void)
{
	if ((d_list != NULL) && (d_list->NumEntries() <= 0))
	{
		CCollisionStackItem::Delete(d_list);
		d_list = NULL;
	}
	return(d_list);
}

/***************************************************************************/
// Destroy the list.
/***************************************************************************/
inline void CEntityList::Destroy(void)
{
	CCollisionStackItem::Delete(d_list);
	d_list = NULL;
}

/***************************************************************************/
// See if an entity is a member of a list.
/***************************************************************************/
inline bool CEntityList::IsMember(CCollisionStackArray<CStageEntity*>* i_list,
		CStageEntity* i_entity)
{
	if ((i_list != NULL) && (i_entity != NULL))
	{
		int numEntries = i_list->NumEntries();
		for (int i = 0; i < numEntries; i++)
		{
			if (i_entity == (*i_list)[i])
				return(true);
		}
	}
	return(false);
}

/***************************************************************************/
// See if an entity is a member of the list.
/***************************************************************************/
inline bool CEntityList::IsMember(CStageEntity* i_entity)
{
	return(IsMember(d_list, i_entity));
}

/***************************************************************************/
/***************************************************************************/
inline CEntityList::~CEntityList()
{
	Destroy();
}

#endif // ENTITY_LIST_HPP
