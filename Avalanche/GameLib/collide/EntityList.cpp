/*
**
**  File:   EntityList.cpp
**  Date:   March 14, 2003
**  By:     Bryant Collard
**  Desc:   Build a list of entites.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/becCollide/EntityList.cpp $
**  $Revision: 3 $
**      $Date: 4/24/03 11:21a $
**    $Author: Bcollard $
**
*/

#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/EntityList.h"

// System includes //
#include <stdarg.h>

#if 0

// Gamelib includes //
#include "collide/Stack.h"
#include "collide/StackArray.h"

#endif

/***************************************************************************/
// Create the list using the global collision system stack. Fails if a
// list already exists or if it cannot be created.
/***************************************************************************/
inline bool CEntityList::Create(void)
{
	if (d_list != NULL)
		return(false);

	d_list = CCollisionStackArray<CStageEntity*>::New(&g_collisionStack);
	if (d_list == NULL)
		return(false);

	d_list->InitArray();
	return(true);
}

/***************************************************************************/
// Build a list using the global collision system stack. Terminate list
// with a NULL entry.
/***************************************************************************/
CCollisionStackArray<CStageEntity*>* CEntityList::Build(
		CStageEntity* i_entity, ...)
{
	CCollisionStackArray<CStageEntity*>* list =
			CCollisionStackArray<CStageEntity*>::New(&g_collisionStack);
	if (list == NULL)
		return(NULL);

	list->InitArray();

	va_list arg;
	va_start(arg, i_entity);
	CStageEntity* entity = i_entity;
	while (entity != NULL)
	{
		CStageEntity** entry = list->ExpandArray();
		if (entry == NULL)
			break;
		*entry = entity;

		entity = va_arg(arg, CStageEntity*);
	}
	va_end(arg);

	if (list->NumEntries() <= 0)
	{
		CCollisionStackItem::Delete(list);
		return(NULL);
	}

	return(list);
}

/***************************************************************************/
// Build a list using a specified stack (NULL uses dynamic memory).
// Terminate list with a NULL entry.
/***************************************************************************/
CCollisionStackArray<CStageEntity*>* CEntityList::Build(
		CCollisionStack* i_stack, ...)
{
	CCollisionStackArray<CStageEntity*>* list =
			CCollisionStackArray<CStageEntity*>::New(i_stack);
	if (list == NULL)
		return(NULL);

	list->InitArray();

	va_list arg;
	va_start(arg, i_stack);
	CStageEntity* entity;
	while ((entity = va_arg(arg, CStageEntity*)) != NULL)
	{
		CStageEntity** entry = list->ExpandArray();
		if (entry == NULL)
			break;
		*entry = entity;
	}
	va_end(arg);

	if (list->NumEntries() <= 0)
	{
		CCollisionStackItem::Delete(list);
		return(NULL);
	}

	return(list);
}
