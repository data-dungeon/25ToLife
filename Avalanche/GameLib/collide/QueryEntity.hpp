/*
**
**  File:   QueryEntity.hpp
**  Date:   April 3, 2003
**  By:     Bryant Collard
**  Desc:   A base class for entities used to perform collision queries.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/collide/QueryEntity.hpp $
**  $Revision: #7 $
**  $DateTime: 2005/04/01 17:02:54 $
**    $Author: Bryant_Collard $
**
*/

#ifdef QUERY_ENTITY_HPP
#undef QUERY_ENTITY_HPP

#include "stage/EntityCategory.h"
#include "collide/EntityList.h"

/***************************************************************************/
// Construct
/***************************************************************************/
inline void CQueryEntity::Construct(void)
{
	d_category |= QUERY_CATEGORY;
	d_ignoreList = NULL;
	d_ownList = false;
	d_collisionConfiguration = COMPLEX;
	d_ignoreEntity = NULL;
	d_manageCollisionComplexity = false;
}

/***************************************************************************/
// Derived type does not have its own entity type.
/***************************************************************************/
inline CQueryEntity::CQueryEntity() :
	CStageEntity(s_queryEntityClass)
{
	Construct();
}

/***************************************************************************/
// Derived type has its own entity type.
/***************************************************************************/
inline CQueryEntity::CQueryEntity(const CEntityType &i_type) :
	CStageEntity(i_type)
{
	Construct();
}

/***************************************************************************/
/***************************************************************************/
inline CQueryEntity::~CQueryEntity()
{
	if (d_ownList)
		CCollisionStackItem::Delete(d_ignoreList);
}

/***************************************************************************/
/***************************************************************************/
inline const CEntityType &CQueryEntity::ClassType(void)
{
	return(s_queryEntityClass);
}

/***************************************************************************/
// Set the ignore list. Note that no action will be taken on any previously
// set list that the entity does not own and that the pointer to it will be
// forgotten.
/***************************************************************************/
inline void CQueryEntity::SetIgnoreList(
		CCollisionStackArray<CStageEntity*>* i_ignoreList, bool i_ownList)
{
	if (d_ownList)
	{
		d_lock.Release();
		CCollisionStackItem::Delete(d_ignoreList);
	}

	d_ignoreList = i_ignoreList;

	if (d_ignoreList == NULL)
	{
		d_ownList = false;
		if (d_manageCollisionComplexity)
		{
			d_collisionConfiguration = SIMPLE;
			d_ignoreEntity = NULL;
		}
	}
	else
	{
		d_ownList = i_ownList;
		if (d_ownList && (d_ignoreList->GetStack() != NULL))
		{
			d_lock.Set(*(d_ignoreList->GetStack()));
			SET_LOCK_NAME(d_lock, "Query Ignore List");
		}
		if (d_manageCollisionComplexity)
			d_collisionConfiguration = COMPLEX;
	}
}

/***************************************************************************/
// Build a list with one entry using the global stack. Note that no action
// will be taken on any previously set list that the entity does not own and
// that the pointer to it will be forgotten.
/***************************************************************************/
inline void CQueryEntity::SetIgnoreEntity(CStageEntity* i_entity)
{
	if (d_ownList)
	{
		d_lock.Release();
		CCollisionStackItem::Delete(d_ignoreList);
	}

	d_ignoreEntity = i_entity;
	d_ignoreList = NULL;
	d_ownList = false;

	if (d_manageCollisionComplexity)
	{
		d_collisionConfiguration = SIMPLE;
		return;
	}

	if (i_entity != NULL)
	{
		d_ignoreList = CEntityList::Build(&g_collisionStack, i_entity, NULL);
		d_ownList = true;
		d_lock.Set(g_collisionStack);
		SET_LOCK_NAME(d_lock, "Query Ignore Entity");
	}
}

/***************************************************************************/
// Delete any ignore list whether or not the entity owns it.
/***************************************************************************/
inline void CQueryEntity::DeleteIgnoreList(void)
{
	d_lock.Release();
	CCollisionStackItem::Delete(d_ignoreList);
	d_ignoreList = NULL;
	d_ownList = false;
}

/***************************************************************************/
/***************************************************************************/
inline bool CQueryEntity::IsIgnoredEntity(CStageEntity* i_entity)
{
	return(CEntityList::IsMember(d_ignoreList, i_entity));
}

/***************************************************************************/
/***************************************************************************/
inline void CQueryEntity::ManageCollisionComplexity(CCollisionObject* i_object)
{
	d_manageCollisionComplexity = true;
	d_collisionObject = i_object;
	d_node.Set(i_object);
	if (d_ignoreList == NULL)
	{
		d_collisionConfiguration = SIMPLE;
		d_ignoreEntity = NULL;
	}
	else
	{
		int numEntries = d_ignoreList->NumEntries();
		if (numEntries > 1)
			d_collisionConfiguration = COMPLEX;
		else
		{
			d_collisionConfiguration = SIMPLE;
			if (numEntries == 1)
				d_ignoreEntity = (*d_ignoreList)[0];
			else
				d_ignoreEntity = NULL;
		}
	}
}

/***************************************************************************/
/***************************************************************************/
inline void CQueryEntity::DoNotManageCollisionComplexity(void)
{
	d_manageCollisionComplexity = false;
}

#endif // QUERY_ENTITY_HPP
