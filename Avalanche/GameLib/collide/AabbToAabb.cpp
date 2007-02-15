/*
**
**  File:   AabbToAabb.hpp
**  Date:   February 4, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between AABBs.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/AabbToAabb.cpp $
**  $Revision: 2 $
**      $Date: 7/25/03 11:41a $
**    $Author: Bcollard $
**
*/

// My header (always first)
#include "collide/AabbToAabb.h"

// Valid data masks.
const uint CAabbToAabbIntersection::SEPARATING_AXIS_TEST = 0x01;

#ifdef DEBUG_STACK
/**********************************************************************/
/**********************************************************************/
const char* CAabbToAabbIntersection::GetName(void)
{
	return("AabbToAabb");
}
#endif
