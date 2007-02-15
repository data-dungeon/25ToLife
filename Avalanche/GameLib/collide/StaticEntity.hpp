/*
**
**  File:   StaticEntity.hpp
**  Date:   March 6, 2003
**  By:     Bryant Collard
**  Desc:   A stage entity that manages all static collision objects in the
**          world.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/StaticEntity.hpp $
**  $Revision: 7 $
**      $Date: 6/11/03 2:33p $
**    $Author: Tyler Colbert $
**
*/

#ifdef STATIC_ENTITY_HPP
#undef STATIC_ENTITY_HPP

#include "collide/ObjectQT.h"
#include "collide/Cull.h"
#include "collide/StackArray.h"

/***************************************************************************/
/***************************************************************************/
inline CStaticChild::CStaticChild()
{
	d_list = NULL;
}

/***************************************************************************/
/***************************************************************************/
inline void CStaticChild::Initialize(CCollisionStack* i_stack)
{
	d_stack = i_stack;
}

/***************************************************************************/
/***************************************************************************/
inline bool CStaticChild::InitCollisionTest(CCollisionCull &i_cull,
		ObjectLayerList* i_layerList)
{
	d_list = CCollisionStackArray<CStaticCollisionObject*>::New(d_stack);
	if (d_list == NULL)
		return(false);
	d_list->InitArray();

	if (i_cull.GetType() == CCollisionCull::CULL_CHORD)
	{
		while (i_layerList != NULL)
		{
			i_layerList->BuildList(d_list, i_cull.Start(), i_cull.Chord());
			i_layerList = i_layerList->next;
		}
	}
	else
	{
		if (i_cull.GetType() != CCollisionCull::CULL_XZ_RECT)
			i_cull.GetRect();
		while (i_layerList != NULL)
		{
			i_layerList->BuildList(d_list, i_cull.Rect());
			i_layerList = i_layerList->next;
		}
	}

	d_index = -1;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
inline void CStaticChild::TerminateCollisionTest(void)
{
	CCollisionStackItem::Delete(d_list);
	d_list = NULL;
}

/***************************************************************************/
/***************************************************************************/
inline CCollisionNode* CStaticChild::GetCurrentChild(void)
{
	ASSERT(d_list != NULL);
	if (++d_index < d_list->NumEntries())
		return(this);
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
inline void CStaticEntity::Initialize(CCollisionStack* i_stack)
{
	d_child.Initialize(i_stack);
}

/***************************************************************************/
/***************************************************************************/
inline CStaticEntity::CStaticEntity(unsigned int &i_layerMask) :
	CStageEntity(s_staticEntityClass),
	d_layerMask(i_layerMask),
	d_dbList(true)
{
	d_category |= STATIC_CATEGORY;
	Initialize(NULL);
}

/***************************************************************************/
/***************************************************************************/
inline const CEntityType &CStaticEntity::ClassType(void)
{
	return(s_staticEntityClass);
}

/***************************************************************************/
/***************************************************************************/
inline void CStaticEntity::Terminate(void)
{
	d_dbList.RemoveAll();
}

/***************************************************************************/
/***************************************************************************/
inline ObjectLayerList* CStaticEntity::Activate(t_Handle i_handle)
{
	ObjectLayerList* layerList = ObjectLayerList::Create(i_handle, d_layerMask);
	if (layerList != NULL)
		d_dbList.AddTail(layerList);
	return(layerList);
}

/***************************************************************************/
/***************************************************************************/
inline void CStaticEntity::Deactivate(ObjectLayerList* i_layerList)
{
	if (i_layerList != NULL)
		d_dbList.Remove(i_layerList);
}

#endif // STATIC_ENTITY_HPP
