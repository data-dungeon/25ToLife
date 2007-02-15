/*
**
**  File:   AabbToTerrain.cpp
**  Date:   March 11, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between an AABB and terrain.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/AabbToTerrain.cpp $
**  $Revision: 10 $
**      $Date: 7/25/03 11:41a $
**    $Author: Bcollard $
**
*/

#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/AabbToTerrain.h"

#if 0

// Gamelib includes //
#include "collide/StackArray.h"

#endif

// Valid data masks.
const uint CAabbToTerrainIntersection::INTERSECTION_TEST   = 0x1;

/***************************************************************************/
/***************************************************************************/
CAabbToTerrainIntersection::~CAabbToTerrainIntersection()
{
}

/***************************************************************************/
/***************************************************************************/
void CAabbToTerrainIntersection::PerformIntersectionTest(void)
{
	d_validFlags |= INTERSECTION_TEST;

	d_iterator.Bounds() = d_aabb.GetConstAABB();
	if (d_triangleList.Open(d_stack) && d_iterator.Initialize(d_terrain))
	{
		TerrainTriangle* triangle;
		while ((triangle = d_iterator.GetNextTriangle()) != NULL)
		{
			if (d_aabb.TestTriangle(triangle->FaceNormal(),
					triangle->A(),	triangle->B(),	triangle->C()))
			{
				d_triangleList.Add(triangle);
			}
		}
	}
	d_triangleList.Close();
	d_intersects = (d_triangleList.NumTriangles() > 0);

	ASSERT(!d_stack->IsBottomLocked());
}

/***************************************************************************/
/***************************************************************************/
void CAabbToTerrainIntersection::DrawTerrain(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	d_triangleList.Draw();
}

#ifdef DEBUG_STACK

#include <string.h>

/**********************************************************************/
/**********************************************************************/
const char* CAabbToTerrainIntersection::GetName(void)
{
	return("AabbToTerrain");
}

/***************************************************************************/
/***************************************************************************/
void CAabbToTerrainIntersection::AppendDescription(char* io_msg)
{
	char msg[256];
	sprintf(msg, " with %d Triangles", d_triangleList.NumTriangles());
	strcat(io_msg, msg);
}

#endif
