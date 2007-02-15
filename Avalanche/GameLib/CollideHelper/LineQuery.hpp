/*
**
**  File:   LineQuery.hpp
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Stage entities that serve up line collision objects.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/CollideHelper/LineQuery.hpp $
**  $Revision: #15 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#ifdef LINE_QUERY_HPP
#undef LINE_QUERY_HPP

#include "collide/collsys.h"
#include "collide/LineToCapsule.h"
#include "CollideHelper/LineEntity.h"
#include "CollideHelper/XZEntity.h"
#include "stage/protagonist.h"
#include "collide/StaticEntity.h"
#include "Game/Database/Som.h"     // For INVALID_MATERIALINDEX

/**********************************************************************/
/**********************************************************************/
inline CLineQuery &CLineQuery::operator=(const CLineQuery &i_src)
{
	actor = i_src.actor;
	intersection = i_src.intersection;
	normal = i_src.normal;
	color = i_src.color;
	material = i_src.material;
	d = i_src.d;
	pair = i_src.pair;

	return(*this);
}

/**********************************************************************/
/**********************************************************************/
inline CCollisionPair* CLineQuery::FindPair(CLineCollisionObject &i_line,
		CCollisionList* i_list)
{
	CCollisionEvent* event = i_list->GetHeadEvent();
	while(event != NULL)
	{
		CCollisionPair* pair = event->GetHeadPair();
		while(pair != NULL)
		{
			if (pair->GetIntersection() == i_line.Modifier())
				return(pair);
			pair = pair->GetNext();
		}
		event = event->GetNext();
	}
	ASSERT(false);
	return(NULL);
}

/**********************************************************************/
/**********************************************************************/
inline bool CLineQuery::Intersection(CLineCollisionObject &i_line,
		Vector3 &o_point)
{
	if (!i_line.FindPoint())
		return(false);

	CCollisionIntersection* pair = i_line.Modifier();
	switch(pair->GetIntersectionType())
	{
	 case CCollisionIntersection::LINE_TO_TERRAIN:
		o_point =
				((CLineToTerrainIntersection*)pair)->GetFoundPoint()->Intersection();
		break;
	 case CCollisionIntersection::LINE_TO_OBB:
		o_point = ((CLineToObbIntersection*)pair)->FoundPoint();
		break;
	 case CCollisionIntersection::LINE_TO_AABB:
		o_point = ((CLineToAabbIntersection*)pair)->FoundPoint();
		break;
	 case CCollisionIntersection::LINE_TO_CAPSULE:
		 o_point = ((CLineToCapsuleIntersection*)pair)->FoundPoint();
		 break;
	 default:
		ASSERT(false);
		return(false);
		break;
	}
	return(true);
}

/**********************************************************************/
/**********************************************************************/
inline bool CLineQuery::Normal(CLineCollisionObject &i_line, Vector3 &o_normal)
{
	if (!i_line.FindPoint())
		return(false);

	CCollisionIntersection* pair = i_line.Modifier();
	switch(pair->GetIntersectionType())
	{
	 case CCollisionIntersection::LINE_TO_TERRAIN:
		o_normal = ((CLineToTerrainIntersection*)pair)->GetFoundPoint()->Normal();
		break;
	 case CCollisionIntersection::LINE_TO_OBB:
		o_normal = ((CLineToObbIntersection*)pair)->FoundNormal();
		break;
	 case CCollisionIntersection::LINE_TO_AABB:
		o_normal = ((CLineToAabbIntersection*)pair)->FoundNormal();
		break;
	 case CCollisionIntersection::LINE_TO_CAPSULE:
		 o_normal = ((CLineToCapsuleIntersection*)pair)->FoundNormal();
		 break;
	 default:
		ASSERT(false);
		return(false);
		break;
	}
	return(true);
}

/**********************************************************************/
/**********************************************************************/
inline bool CLineQuery::Contact(CLineCollisionObject &i_line,
		Vector3 &o_intersection, Vector3 &o_normal)
{
	if (!i_line.FindPoint())
		return(false);

	CCollisionIntersection* pair = i_line.Modifier();
	switch(pair->GetIntersectionType())
	{
	 case CCollisionIntersection::LINE_TO_TERRAIN:
		o_intersection =
				((CLineToTerrainIntersection*)pair)->GetFoundPoint()->Intersection();
		o_normal = ((CLineToTerrainIntersection*)pair)->GetFoundPoint()->Normal();
		break;
	 case CCollisionIntersection::LINE_TO_OBB:
		o_intersection = ((CLineToObbIntersection*)pair)->FoundPoint();
		o_normal = ((CLineToObbIntersection*)pair)->FoundNormal();
		break;
	 case CCollisionIntersection::LINE_TO_AABB:
		o_intersection = ((CLineToAabbIntersection*)pair)->FoundPoint();
		o_normal = ((CLineToAabbIntersection*)pair)->FoundNormal();
		break;
	 case CCollisionIntersection::LINE_TO_CAPSULE:
		 o_intersection = ((CLineToCapsuleIntersection*)pair)->FoundPoint();
		 o_normal = ((CLineToCapsuleIntersection*)pair)->FoundNormal();
		 break;
	 default:
		ASSERT(false);
		return(false);
		break;
	}
	return(true);
}

/**********************************************************************/
/**********************************************************************/
inline u16 CLineQuery::Color(CLineCollisionObject &i_line,
		CCollisionList* i_list)
{
	if ((i_list != NULL) && i_line.FindPoint())
	{
		CCollisionPair* pair = FindPair(i_line, i_list);
		return(pair->GetEvent()->GetTargetEntity()->Color(pair));
	}
	return(0xffff);
}

/**********************************************************************/
/**********************************************************************/
inline u8 CLineQuery::Material(CLineCollisionObject &i_line,
		CCollisionList* i_list)
{
	if ((i_list != NULL) && i_line.FindPoint())
	{
		CCollisionPair* pair = FindPair(i_line, i_list);
		return(pair->GetEvent()->GetTargetEntity()->Material(pair));
	}
	return(INVALID_MATERIALINDEX);
}

/**********************************************************************/
/**********************************************************************/
inline bool CLineQuery::Process(CLineCollisionObject &i_line,
		CCollisionList* i_list)
{
	if (i_list == NULL)
	{
		actor = NULL;
		return(false);
	}

	if (i_line.FindMode() == CLineCollisionObject::FIND_ANY)
	{
		actor = NULL;
		return(i_line.Modifier() != NULL);
	}

	if (i_line.FindMode() == CLineCollisionObject::FIND_ALL)
	{
		actor = NULL;
		return(true);
	}

	if (i_line.Modifier() == NULL)
	{
		actor = NULL;
		return(false);
	}

	CCollisionPair* collisionPair = FindPair(i_line, i_list);
	event = collisionPair->GetEvent();
	target = event->GetTargetEntity();
	d = i_line.FoundPoint();
	pair = i_line.Modifier();
	color = target->Color(collisionPair);
	material = target->Material(collisionPair);
	actor = NULL;

	switch(pair->GetIntersectionType())
	{
	 case CCollisionIntersection::LINE_TO_TERRAIN:
	 {
		CLineToTerrainPoint* terrainPoint =
				((CLineToTerrainIntersection*)pair)->GetFoundPoint();
		ASSERT(terrainPoint != NULL);
		intersection = terrainPoint->Intersection();
		normal = terrainPoint->Normal();
		break;
	 }
	 case CCollisionIntersection::LINE_TO_OBB:
	 {
		CLineToObbIntersection* obbHit = (CLineToObbIntersection*)pair;
		intersection = obbHit->FoundPoint();
		normal = obbHit->FoundNormal();
		if (target->IsA(CProtagonist::ClassType()))
			actor = (CProtagonist*)target;
		break;
	 }
	 case CCollisionIntersection::LINE_TO_AABB:
	 {
		CLineToAabbIntersection* aabbHit = (CLineToAabbIntersection*)pair;
		intersection = aabbHit->FoundPoint();
		normal = aabbHit->FoundNormal();
		break;
	 }
	 case CCollisionIntersection::LINE_TO_CAPSULE:
	 {
		CLineToCapsuleIntersection* capsuleHit = (CLineToCapsuleIntersection*)pair;
		intersection = capsuleHit->FoundPoint();
		normal = capsuleHit->FoundNormal();
		if (target->IsA(CProtagonist::ClassType()))
			actor = (CProtagonist*)target;
		break;
	 }
	 default:
		ASSERT(false);
		break;
	}
	return(true);
}

/**********************************************************************/
/**********************************************************************/
inline bool CLineQuery::Process(CLineEntity &i_entity, CCollisionList* i_list)
{
	return(Process(i_entity.Line(), i_list));
}

/**********************************************************************/
/**********************************************************************/
inline bool CLineQuery::Process(CXZEntity &i_entity, CCollisionList* i_list)
{
	return(Process(i_entity.Line(), i_list));
}

/**********************************************************************/
/**********************************************************************/
inline bool CLineQuery::Test(CLineEntity &i_entity,
		CStageEntity::ESelect i_select)
{
	return(Process(i_entity.Line(), g_collisionSystem.Test(&i_entity, i_select)));
}

/**********************************************************************/
/**********************************************************************/
inline bool CLineQuery::Test(CXZEntity &i_entity,
		CStageEntity::ESelect i_select)
{
	return(Process(i_entity.Line(), g_collisionSystem.Test(&i_entity, i_select)));
}

#endif // LINE_QUERY_HPP
