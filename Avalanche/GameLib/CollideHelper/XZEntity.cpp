/*
**
**  File:   XZEntity.cpp
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Stage entities that serve up line collision objects.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/CollideHelper/XZEntity.cpp $
**  $Revision: 6 $
**      $Date: 7/24/03 4:41p $
**    $Author: Bcollard $
**
*/

#include "CollideHelper/CollideHelperPCH.h"

// My header (always first) //
#include "CollideHelper/XZEntity.h"

// Default for really large world
#define REALLYLARGEY	100000.0f

float CXZEntity::s_ceiling = REALLYLARGEY;
float CXZEntity::s_floor = -REALLYLARGEY; 
float CXZEntity::s_height = 2.0f * REALLYLARGEY;

/**********************************************************************/
// Setup the world extents
/**********************************************************************/
void CXZEntity::SetWorldExtents(
float i_floorY,
float i_ceilingY)
{
	ASSERT(i_ceilingY > i_floorY);

	s_floor = i_floorY;
	s_ceiling = i_ceilingY;
	s_height = s_ceiling - s_floor;
}

/**********************************************************************/
/**********************************************************************/
void CXZEntity::BeginQuery(void)
{
	d_line.InitializeFindMode();
}

/**********************************************************************/
/**********************************************************************/
void CXZEntity::EndQuery(void)
{
	d_line.TerminateFindMode();
	d_line.ComputePoint(d_line.FoundPoint(), d_intersection);
}

/**********************************************************************/
/**********************************************************************/
bool CXZEntity::GetCollisionCull(CCollisionCull &o_cull)
{
	o_cull.SelectRect().Set(d_line.Origin().x(), d_line.Origin().z(),
			d_line.Origin().x(), d_line.Origin().z());
	return(true);
}

/***************************************************************************/
/***************************************************************************/
const char* CXZEntity::GetClassName(void)
{
	return("XZ");
}
