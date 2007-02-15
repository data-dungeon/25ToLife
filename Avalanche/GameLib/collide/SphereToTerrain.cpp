/*
**
**  File:   SphereToTerrain.cpp
**  Date:   May 1, 2003
**  By:     Bryant Collard
**  Desc:   Tests intersections between a sphere and terrain.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/SphereToTerrain.cpp $
**  $Revision: 6 $
**      $Date: 7/25/03 11:41a $
**    $Author: Bcollard $
**
*/

#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/SphereToTerrain.h"

#if 0

// Engine include //
#include "Math/Vector.h"

// Gamelib includes //
#include "collide/StackArray.h"
#include "geomUtil/planefit.h"

#endif

// Valid data masks.
const uint CSphereToTerrainIntersection::INTERSECTION_TEST   = 0x1;

/***************************************************************************/
/***************************************************************************/
CSphereToTerrainIntersection::~CSphereToTerrainIntersection()
{
}

/***************************************************************************/
/***************************************************************************/
void CSphereToTerrainIntersection::PerformIntersectionTest(void)
{
	d_validFlags |= INTERSECTION_TEST;

	d_sphere.ComputeWorldExtents(d_iterator.Bounds());
	if (d_triangleList.Open(d_stack) && d_iterator.Initialize(d_terrain))
	{
		TerrainTriangle* triangle;
		while ((triangle = d_iterator.GetNextTriangle()) != NULL)
		{
			if (d_sphere.TestTriangle(triangle->FaceNormal(),
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
void CSphereToTerrainIntersection::DrawTerrain(void)
{
	if ((d_validFlags & INTERSECTION_TEST) == 0)
		PerformIntersectionTest();
	d_triangleList.Draw();
}

#ifdef DEBUG_STACK

#include <string.h>

/**********************************************************************/
/**********************************************************************/
const char* CSphereToTerrainIntersection::GetName(void)
{
	return("SphereToTerrain");
}

/***************************************************************************/
/***************************************************************************/
void CSphereToTerrainIntersection::AppendDescription(char* io_msg)
{
	char msg[256];
	sprintf(msg, " with %d Triangles",
			d_triangleList.NumTriangles());
	strcat(io_msg, msg);
}

#endif
