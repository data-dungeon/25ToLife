/*
**
**  File:   Intersection.h
**  Date:   January 31, 2003
**  By:     Bryant Collard
**  Desc:   A base class for intersection testing.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/collide/Intersection.h $
**  $Revision: #9 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "collide/Stack.h"

class CCollisionIntersection : public CCollisionStackItem
{
  public:
	enum ECollisionIntersectionType
	{
		LINE_TO_TERRAIN,
		LINE_TO_OBB,
		OBB_TO_TERRAIN,
		OBB_TO_OBB,
		LINE_TO_AABB,
		AABB_TO_TERRAIN,
		AABB_TO_OBB,
		AABB_TO_AABB,
		LINE_TO_SPHERE,
		SPHERE_TO_TERRAIN,
		SPHERE_TO_OBB,
		SPHERE_TO_AABB,
		SPHERE_TO_SPHERE,
		LINE_TO_TRIANGLE_LIST,
		TRIANGLE_LIST_TO_TERRAIN,
		TRIANGLE_LIST_TO_OBB,
		TRIANGLE_LIST_TO_AABB,
		TRIANGLE_LIST_TO_SPHERE,
		TRIANGLE_LIST_TO_TRIANGLE_LIST,
		LINE_TO_CAPSULE,
		CAPSULE_TO_TERRAIN,
		CAPSULE_TO_OBB,
		CAPSULE_TO_AABB,
		SPHERE_TO_CAPSULE,
		CAPSULE_TO_CAPSULE,
	};
	ECollisionIntersectionType GetIntersectionType(void) {return(d_type);}

#ifdef DEBUG_STACK
	virtual const char* GetName(void) {return("Intersection");}
#endif

  protected:
	CCollisionIntersection(ECollisionIntersectionType i_type);
	CCollisionIntersection(ECollisionIntersectionType i_type,
			CCollisionStack &i_stack);

  private:
	CCollisionIntersection();

	const ECollisionIntersectionType d_type;
};

#define INTERSECTION_HPP
#include "collide/Intersection.hpp"

#endif // INTERSECTION_H
