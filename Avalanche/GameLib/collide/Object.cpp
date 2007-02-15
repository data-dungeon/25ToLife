/*
**
**  File:   Object.cpp
**  Date:   January 31, 2003
**  By:     Bryant Collard
**  Desc:   A base class for geometrical objects (OBBs, Segments, Terrain, etc.)
**          that can collide with each other.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/collide/Object.cpp $
**  $Revision: #3 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

// Pre-compiled header //
#include "collide/CollidePCH.h"

// My include (always first) //
#include "collide/Object.h"

// Gamelib includes //
#include "collide/Line.h"
#include "collide/obb.h"
#include "collide/Sphere.h"
#include "collide/TriangleList.h"
#include "collide/AabbObject.h"

/**********************************************************************/
// Perform Draw() this way to avoid vtable in CCollisionObject.
/**********************************************************************/
void CCollisionObject::Draw(DrawUtility::COLOR i_color)
{
	switch(d_type)
	{
	 case LINE:
		((CLineCollisionObject*)this)->Draw(i_color);
		break;
	 case OBB_OBJECT:
		((OBB*)this)->Draw(i_color);
		break;
	 case SPHERE:
		((CSphereCollisionObject*)this)->Draw(i_color);
		break;
	 case TRIANGLE_LIST:
		((CTriangleListCollisionObject*)this)->Draw(i_color);
		break;
	 case AABB_OBJECT:
		 ((CAabbCollisionObject*)this)->Draw(i_color);
		 break;
	 default:
		ASSERT(false);
		break;
	}
}
