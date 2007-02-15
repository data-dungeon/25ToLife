/*
**
**  File:   LineQuery.h
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Picks apart queries made with lines.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/CollideHelper/LineQuery.h $
**  $Revision: #9 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#ifndef LINE_QUERY_H
#define LINE_QUERY_H

#include "Math/Vector.h"
#include "platform/BaseType.h"
#include "stage/StageEntity.h"

class CLineEntity;
class CXZEntity;
class CStageEntity;
class CProtagonist;
class CCollisionIntersection;
class CCollisionList;

class CLineQuery
{
  public:
	// Construct and copy
	CLineQuery() {actor = NULL;}
	CLineQuery &operator=(const CLineQuery &i_src);

	// Tests against the global collision system and extracts all data
	bool Test(CLineEntity &i_entity,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);
	bool Test(CXZEntity &i_entity,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);

	// Extracts data from a collision list
	bool Process(CLineEntity &i_entity, CCollisionList* i_list);
	bool Process(CXZEntity &i_entity, CCollisionList* i_list);
	bool Process(CLineCollisionObject &i_line, CCollisionList* i_list);

	// Get specific pieces of information
	static bool Intersection(CLineCollisionObject &i_line, Vector3 &o_point);
	static bool Normal(CLineCollisionObject &i_line, Vector3 &o_normal);
	static bool Contact(CLineCollisionObject &i_line, Vector3 &o_intersection,
			Vector3 &o_normal);
	static u16 Color(CLineCollisionObject &i_line, CCollisionList* i_list);
	static u8 Material(CLineCollisionObject &i_line, CCollisionList* i_list);
	static CCollisionPair* FindPair(CLineCollisionObject &i_line,
			CCollisionList* i_list);

	// Data
	CStageEntity*           target;       // The target of the query
	CProtagonist*           actor;        // Actor that was hit
	Vector3                 intersection; // Point of intersection
	Vector3                 normal;       // Normal of intersected surface
	u16                     color;        // color of intersected triangle or obb
	u8                      material;     // Material at the intersection point
	float                   d;            // distance along test object of hit
	CCollisionIntersection* pair;         // The pair representing the hit
	CCollisionEvent*        event;        // The event representing the hit
};

#define LINE_QUERY_HPP
#include "CollideHelper/LineQuery.hpp"

#endif // LINE_QUERY_H
