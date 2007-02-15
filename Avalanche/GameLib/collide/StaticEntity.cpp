/*
**
**  File:   StaticEntity.cpp
**  Date:   March 6, 2003
**  By:     Bryant Collard
**  Desc:   A stage entity that manages all static collision objects in the
**          world.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/collide/StaticEntity.cpp $
**  $Revision: #8 $
**  $DateTime: 2005/06/08 16:04:21 $
**    $Author: Bryant_Collard $
**
*/

#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/StaticEntity.h"

#include "collide/Event.h"

/***************************************************************************/
/***************************************************************************/
CCollisionNode* CStaticEntity::BeginCollisionTest(CStageEntity* i_entity,
		ESelect i_select)
{
	CCollisionCull cull;
	if ((i_entity == NULL) || !i_entity->GetCollisionCull(cull))
		return(NULL);

	if (d_child.InitCollisionTest(cull, d_dbList.Head()))
		return((CCollisionNode*)this);
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
bool CStaticEntity::EndCollisionTest(CCollisionEvent* i_event)
{
	d_child.TerminateCollisionTest();
	return(true);
}

/***************************************************************************/
/***************************************************************************/
CCollisionNode* CStaticEntity::GetChild(void)
{
	return(d_child.GetCurrentChild());
}

/***************************************************************************/
/***************************************************************************/
CCollisionObject* CStaticChild::BeginObjectTest(void)
{
	ASSERT(d_list != NULL);
	return((CCollisionObject*)((*d_list)[d_index]));
}

/***************************************************************************/
/***************************************************************************/
uint16 CStaticEntity::Color(CCollisionPair* i_pair)
{
	return(((CStaticCollisionObject*)i_pair->GetTargetObject())->Color());
}

/***************************************************************************/
/***************************************************************************/
uint8 CStaticEntity::Material(CCollisionPair* i_pair)
{
	return(((CStaticCollisionObject*)i_pair->GetTargetObject())->Material());
}

/***************************************************************************/
/***************************************************************************/
void CStaticEntity::Draw(CCollisionEvent* i_event)
{
	CCollisionPair* pair = i_event->GetHeadPair();
	while (pair != NULL)
	{
		((CStaticCollisionObject*)pair->GetTargetObject())->Draw(
				DrawUtility::YELLOW);
		pair = pair->GetNext();
	}
}

/***************************************************************************/
/***************************************************************************/
const char* CStaticEntity::GetName(void)
{
	return("Static");
}
