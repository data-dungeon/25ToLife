/*
**
**  File:   Terrain.cpp
**  Date:   January 31, 2003
**  By:     Bryant Collard
**  Desc:   A terrain collision object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/Terrain.cpp $
**  $Revision: 12 $
**      $Date: 6/11/03 2:33p $
**    $Author: Tyler Colbert $
**
*/

#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/Terrain.h"

// Gamelib includes //
#include "collide/Stack.h"
#include "collide/StackArray.h"

#ifdef TERRAIN_TRI_TEST_STATS
int CTerrainAABBIterator::triangleCount;
int CTerrainOBBIterator::triangleCount;
int CTerrainLineIterator::triangleCount;
#endif
	

/***************************************************************************/
/***************************************************************************/
void CTerrainCollisionObject::InitializeDraw(CCollisionStack* i_stack,
		SDrawState &o_drawState)
{
	o_drawState.d_stack = i_stack;
	o_drawState.d_edgeList = NULL;
}

/***************************************************************************/
/***************************************************************************/
bool CTerrainCollisionObject::DrawTriangle(TerrainTriangle* i_triangle,
		SDrawState &io_state)
{
/*
 *	All triangles in a TerrainCollide object use a common set of
 *	verticies. Detect a change in TerrainCollide objects and set up
 *	an array to track the edges in the object that have been drawn.
 */
	if (io_state.d_edgeList != NULL)
		CCollisionStackItem::Delete(io_state.d_edgeList);
	io_state.d_edgeList = CCollisionStackArray<SDrawState::SEdge>::New(io_state.d_stack);
	if (io_state.d_edgeList == NULL)
		return(false);
	io_state.d_edgeList->InitArray();
/*
 *	Draw each edge.
 */
	if (!DrawEdge(i_triangle->A(), i_triangle->B(), io_state) ||
			!DrawEdge(i_triangle->B(), i_triangle->C(), io_state) ||
			!DrawEdge(i_triangle->C(), i_triangle->A(), io_state))
	{
		CCollisionStackItem::Delete(io_state.d_edgeList);
		io_state.d_edgeList = NULL;
		return(false);
	}
#if 0
/*
 *	Draw normals.
 */
	Vector3 centroid = (1.0f / 3.0f) *
								(i_triangle->A() +	i_triangle->B() + i_triangle->C());
	DrawUtility::Normal(centroid, i_triangle->FaceNormal(),
			DrawUtility::YELLOW);
#endif
	return(true);
}

/***************************************************************************/
/***************************************************************************/
void CTerrainCollisionObject::TerminateDraw(SDrawState &i_state)
{
	if (i_state.d_edgeList != NULL)
		CCollisionStackItem::Delete(i_state.d_edgeList);
}

/***************************************************************************/
/***************************************************************************/
bool CTerrainCollisionObject::DrawEdge(Vector3CRef i_start,
		Vector3CRef i_end, SDrawState &i_state)
{
/*
 *	See if this edge has already been drawn.
 * Holy God, this is ugly, sorry Bryant.
 */
	for (int i = 0; i < i_state.d_edgeList->NumEntries(); i++)
	{
		if (((i_start == i_state.d_edgeList->EntryRef(i).d_start) &&
				(i_end == i_state.d_edgeList->EntryRef(i).d_end)) ||
				((i_end == i_state.d_edgeList->EntryRef(i).d_start) &&
				(i_start == i_state.d_edgeList->EntryRef(i).d_end)))
			return(true);
	}
/*
 *	Add the edge to the list.
 */
	SDrawState::SEdge* edge = i_state.d_edgeList->ExpandArray();
	if (edge == NULL)
		return(false);
	edge->d_start = i_start;
	edge->d_end = i_end;
/*
 *	Draw it.
 */
	DrawUtility::Line(i_start, i_end, DrawUtility::GREEN);
	return(true);
}

/***************************************************************************/
/***************************************************************************/
#include "cmdcon/cmdcon.h"
#include "EngineHelper/filestream.h"
void CTerrainCollisionObject::Validate(FileStream* i_fs)
{
	static char marks[4][4] = {"   ", "  *", " **", "***"};
	d_numSuspectTriangles = 0;

	TerrainCollideLayerList* layerList = d_dbList.Head();
	while (layerList != NULL)
	{
		layerList->Validate(d_suspectList, d_numSuspectTriangles,
				MAX_SUSPECT_TRIANGLES);
		layerList = layerList->next;
	}

	if (d_numSuspectTriangles > 0)
	{
		if (i_fs != NULL)
			i_fs->Print("LocX\tLocY\tLocZ\tRH\tUN\tCnr\tSrd\n");

		for (u32 i = 0; i < d_numSuspectTriangles; i++)
		{
			Vector3 center = (1.0f / 3.0f) *
					(d_suspectList[i].d_triangle->A() +
					d_suspectList[i].d_triangle->B() +
					d_suspectList[i].d_triangle->C());

			g_console.Echo("Suspect triangle at [%f, %f, %f]\n",
					center.x(), center.y(), center.z());
			if (d_suspectList[i].d_reason & SUSPECT_RIGHT_HANDED)
				g_console.Echo("\tRight Handed\n");
			if (d_suspectList[i].d_reason & SUSPECT_UNIT_NORMAL)
				g_console.Echo("\tUnit Normal\n");
			if (d_suspectList[i].d_reason & SUSPECT_CORNER1)
				g_console.Echo("\tCorner 1\n");
			if (d_suspectList[i].d_reason & SUSPECT_MULTI1)
				g_console.Echo("\tMulti 1\n");
			if (d_suspectList[i].d_reason & SUSPECT_CORNER2)
				g_console.Echo("\tCorner 2\n");
			if (d_suspectList[i].d_reason & SUSPECT_MULTI2)
				g_console.Echo("\tMulti 2\n");
			if (d_suspectList[i].d_reason & SUSPECT_CORNER3)
				g_console.Echo("\tCorner 3\n");
			if (d_suspectList[i].d_reason & SUSPECT_MULTI3)
				g_console.Echo("\tMulti 3\n");

			if (i_fs != NULL)
			{
				int corners = 0;
				if (d_suspectList[i].d_reason & SUSPECT_CORNER1)
					corners++;
				if (d_suspectList[i].d_reason & SUSPECT_CORNER2)
					corners++;
				if (d_suspectList[i].d_reason & SUSPECT_CORNER3)
					corners++;

				int shared = 0;
				if (d_suspectList[i].d_reason & SUSPECT_MULTI1)
					shared++;
				if (d_suspectList[i].d_reason & SUSPECT_MULTI2)
					shared++;
				if (d_suspectList[i].d_reason & SUSPECT_MULTI3)
					shared++;

				i_fs->Print("%f\t%f\t%f\t%c\t%c\t%s\t%s\n",
						center.x(), center.y(), center.z(),
						(d_suspectList[i].d_reason & SUSPECT_RIGHT_HANDED) ? '*' : ' ',
						(d_suspectList[i].d_reason & SUSPECT_UNIT_NORMAL) ? '*' : ' ',
						marks[corners], marks[shared]);
			}
		}

		g_console.Echo("\nSuspect Drawing Legend\n");
		g_console.Echo("  Normal:\n");
		g_console.Echo("    Right Handed - BLACK\n");
		g_console.Echo("    Unit Normal  - RED\n");
		g_console.Echo("    Both         - MAGENTA\n");
		g_console.Echo("    Neither      - WHITE\n");
		g_console.Echo("  Edge:\n");
		g_console.Echo("    Corner       - BLUE\n");
		g_console.Echo("    Shared       - YELLOW\n");
		g_console.Echo("    Both         - CYAN\n");
		g_console.Echo("    Neither      - GREEN\n");
	}
	else
	{
		g_console.Echo("No suspect triangles.\n");
		if (i_fs != NULL)
			i_fs->Print("No suspect triangles.\n");
	}

}

/***************************************************************************/
/***************************************************************************/
void CTerrainCollisionObject::DrawSuspects(void)
{
	static DrawUtility::COLOR normalColor[] =
	{
		DrawUtility::WHITE,
		DrawUtility::BLACK,
		DrawUtility::RED,
		DrawUtility::MAGENTA
	};
	static DrawUtility::COLOR edgeColor[] =
	{
		DrawUtility::GREEN,
		DrawUtility::BLUE,
		DrawUtility::YELLOW,
		DrawUtility::CYAN
	};

	for (u32 i = 0; i < d_numSuspectTriangles; i++)
	{
		TerrainTriangle* triangle = d_suspectList[i].d_triangle;
		u32 reason = d_suspectList[i].d_reason;

		DrawUtility::Line(triangle->A(), triangle->B(),
				edgeColor[(reason >> NEIGHBOR1_SHIFT) & 0x3]);
		DrawUtility::Line(triangle->B(), triangle->C(),
				edgeColor[(reason >> NEIGHBOR2_SHIFT) & 0x3]);
		DrawUtility::Line(triangle->C(), triangle->A(),
				edgeColor[(reason >> NEIGHBOR3_SHIFT) & 0x3]);
		Vector3 centroid = (1.0f / 3.0f) * (triangle->A() +
				triangle->B() + triangle->C());
		DrawUtility::Normal(centroid, triangle->FaceNormal(),
				normalColor[reason & 0x3]);
	}
}
