/*
**
**  File:   TriangleList.cpp
**  Date:   June 12, 2003
**  By:     Bryant Collard
**  Desc:   A collision object that is a list of terrain triangles. This object
**          is odd because it is transient in nature and uses a collision stack
**          to store information. Care must be taken in its use.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/TriangleList.cpp $
**  $Revision: 5 $
**      $Date: 7/01/03 11:54a $
**    $Author: Bcollard $
**
*/

// Precompiled header //
#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/TriangleList.h"

// Collide headers //
#include "collide/terraintri.h"

#define VERTEX_THRESHOLD_SQ  0.0001f

/***************************************************************************/
/***************************************************************************/
bool CTriangleListCollisionObject::BuildVertexList(void)
{
	if (d_vertexList == NULL)
		return(false);

	if (d_vertexList->NumEntries() > 0)
		return(true);

	int numTriangles = d_triangleList->NumEntries();
	for (int i = 0; i < numTriangles; i++)
	{
		STriangleEntry& entry = d_triangleList->EntryRef(i);
		entry.d_vertexIndex[0] =
				AddVertex(entry.d_triangle->A(), entry.d_transformedVertex[0]);
		entry.d_vertexIndex[1] =
				AddVertex(entry.d_triangle->B(), entry.d_transformedVertex[1]);
		entry.d_vertexIndex[2] =
				AddVertex(entry.d_triangle->C(), entry.d_transformedVertex[2]);
	}
	return(true);
}

/***************************************************************************/
/***************************************************************************/
int CTriangleListCollisionObject::AddVertex(Vector3CRef i_vertex,
		Vector3CRef i_transformedVertex)
{
	int numVertices = d_vertexList->NumEntries();
	for (int i = 0; i < numVertices; i++)
	{
		if ((i_vertex - (*d_vertexList)[i].d_vertex).LengthSquared() <
				VERTEX_THRESHOLD_SQ)
			return(i);
	}

	SVertex* entry = d_vertexList->ExpandArray();
	if (entry == NULL)
		return(-1);

	entry->d_vertex = i_vertex;
	entry->d_transformedVertex = i_transformedVertex;
	entry->d_group = -1;
	return(numVertices);
}

/***************************************************************************/
/***************************************************************************/
bool CTriangleListCollisionObject::BuildEdgeList(void)
{
	if (d_edgeList == NULL)
		return(false);

	if (d_edgeList->NumEntries() > 0)
		return(true);

	int numTriangles = d_triangleList->NumEntries();
	for (int i = 0; i < numTriangles; i++)
	{
		STriangleEntry &entry = d_triangleList->EntryRef(i);
		entry.d_edgeIndex[0] =
				AddEdge(entry.d_vertexIndex[0], entry.d_vertexIndex[1]);
		entry.d_edgeIndex[1] =
				AddEdge(entry.d_vertexIndex[1], entry.d_vertexIndex[2]);
		entry.d_edgeIndex[2] =
				AddEdge(entry.d_vertexIndex[2], entry.d_vertexIndex[0]);
	}
	return(true);
}

/***************************************************************************/
/***************************************************************************/
int CTriangleListCollisionObject::AddEdge(int i_index1, int i_index2)
{
/*
 *	See if the stack filled up while finding vertices or if vertices were
 *	collapsed incorrectly.
 */
	if ((i_index1 < 0) || (i_index2 < 0) || (i_index1 == i_index2))
		return(-1);
/*
 *	Order the vertices to make searching easier.
 */
	int start, end;
	if (i_index1 < i_index2)
	{
		start = i_index1;
		end = i_index2;
	}
	else
	{
		start = i_index2;
		end = i_index1;
	}
/*
 *	See if this edge has already been found.
 */
	int numEdges = d_edgeList->NumEntries();
	for (int i = 0; i < numEdges; i++)
	{
		SEdge &entry = d_edgeList->EntryRef(i);
		if ((start == entry.d_start) && (end == entry.d_end))
			return(i);
	}
/*
 *	Add the edge to the list.
 */
	SEdge* entry = d_edgeList->ExpandArray();
	if (entry == NULL)
		return(-1);

	entry->d_start = start;
	entry->d_end = end;
	return(numEdges);
}

/***************************************************************************/
/***************************************************************************/
bool CTriangleListCollisionObject::BuildGroups(void)
{
	if (d_vertexList == NULL)
		return(false);

	if (d_numGroups <= 0)
	{
		int numTriangles = d_triangleList->NumEntries();
		int numVertices = d_vertexList->NumEntries();
		for (int i = 0; i < numVertices; i++)
		{
			int group = (*d_vertexList)[i].d_group;
			if (group < 0)
			{
				group = d_numGroups++;
				(*d_vertexList)[i].d_group = group;
			}

			for (int j = 0; j < numTriangles; j++)
			{
				STriangleEntry &entry = d_triangleList->EntryRef(j);
				if (entry.d_vertexIndex[0] == i)
				{
					ASSERT((((*d_vertexList)[entry.d_vertexIndex[1]].d_group < 0) ||
							((*d_vertexList)[entry.d_vertexIndex[1]].d_group == group)) &&
							(((*d_vertexList)[entry.d_vertexIndex[2]].d_group < 0) ||
							((*d_vertexList)[entry.d_vertexIndex[2]].d_group == group)));
					(*d_vertexList)[entry.d_vertexIndex[1]].d_group = group;
					(*d_vertexList)[entry.d_vertexIndex[2]].d_group = group;
				}
				else if (entry.d_vertexIndex[1] == i)
				{
					ASSERT((((*d_vertexList)[entry.d_vertexIndex[0]].d_group < 0) ||
							((*d_vertexList)[entry.d_vertexIndex[0]].d_group == group)) &&
							(((*d_vertexList)[entry.d_vertexIndex[2]].d_group < 0) ||
							((*d_vertexList)[entry.d_vertexIndex[2]].d_group == group)));
					(*d_vertexList)[entry.d_vertexIndex[0]].d_group = group;
					(*d_vertexList)[entry.d_vertexIndex[2]].d_group = group;
				}
				else if (entry.d_vertexIndex[2] == i)
				{
					ASSERT((((*d_vertexList)[entry.d_vertexIndex[0]].d_group < 0) ||
							((*d_vertexList)[entry.d_vertexIndex[0]].d_group == group)) &&
							(((*d_vertexList)[entry.d_vertexIndex[1]].d_group < 0) ||
							((*d_vertexList)[entry.d_vertexIndex[1]].d_group == group)));
					(*d_vertexList)[entry.d_vertexIndex[0]].d_group = group;
					(*d_vertexList)[entry.d_vertexIndex[1]].d_group = group;
				}
			}
		}
	}
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool CTriangleListCollisionObject::CTriangleGroupIterator::GotoStart(void)
{
	if (d_list.d_vertexList != NULL)
	{
		int numTriangles = d_list.d_triangleList->NumEntries();
		for (d_index = 0; d_index < numTriangles; d_index++)
		{
			if ((*d_list.d_vertexList)[(*d_list.d_triangleList)[
					d_index].d_vertexIndex[0]].d_group == d_group)
			{
				d_atEnd = false;
				return(true);
			}
		}
	}
	d_atEnd = true;
	return(false);
}

/***************************************************************************/
/***************************************************************************/
bool CTriangleListCollisionObject::CTriangleGroupIterator::operator++()
{
	if (!d_atEnd && (d_list.d_vertexList != NULL))
	{
		int numTriangles = d_list.d_triangleList->NumEntries();
		for (d_index++; d_index < numTriangles; d_index++)
		{
			if ((*d_list.d_vertexList)[(*d_list.d_triangleList)[
					d_index].d_vertexIndex[0]].d_group == d_group)
				return(true);
		}
		d_atEnd = true;
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
bool CTriangleListCollisionObject::CVertexGroupIterator::GotoStart(void)
{
	if (d_list.d_vertexList != NULL)
	{
		int numVertices = d_list.d_vertexList->NumEntries();
		for (d_index = 0; d_index < numVertices; d_index++)
		{
			if ((*d_list.d_vertexList)[d_index].d_group == d_group)
			{
				d_atEnd = false;
				return(true);
			}
		}
	}
	d_atEnd = true;
	return(false);
}

/***************************************************************************/
/***************************************************************************/
bool CTriangleListCollisionObject::CVertexGroupIterator::operator++()
{
	if (!d_atEnd && (d_list.d_vertexList != NULL))
	{
		int numVertices = d_list.d_vertexList->NumEntries();
		for (d_index++; d_index < numVertices; d_index++)
		{
			if ((*d_list.d_vertexList)[d_index].d_group == d_group)
				return(true);
		}
		d_atEnd = true;
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
bool CTriangleListCollisionObject::CEdgeGroupIterator::GotoStart(void)
{
	if ((d_list.d_vertexList != NULL) && (d_list.d_edgeList != NULL))
	{
		int numEdges = d_list.d_edgeList->NumEntries();
		for (d_index = 0; d_index < numEdges; d_index++)
		{
			if ((*d_list.d_vertexList)[(*d_list.d_edgeList)[
					d_index].d_start].d_group == d_group)
			{
				d_atEnd = false;
				return(true);
			}
		}
	}
	d_atEnd = true;
	return(false);
}

/***************************************************************************/
/***************************************************************************/
bool CTriangleListCollisionObject::CEdgeGroupIterator::operator++()
{
	if (!d_atEnd && (d_list.d_vertexList != NULL) && (d_list.d_edgeList != NULL))
	{
		int numEdges = d_list.d_edgeList->NumEntries();
		for (d_index++; d_index < numEdges; d_index++)
		{
			if ((*d_list.d_vertexList)[(*d_list.d_edgeList)[
					d_index].d_start].d_group == d_group)
				return(true);
		}
		d_atEnd = true;
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
void CTriangleListCollisionObject::Draw(DrawUtility::COLOR i_color)
{
	if (!BuildVertexList() || !BuildEdgeList())
		return;

	int numEdges = d_edgeList->NumEntries();
	for (int i = 0; i < numEdges; i++)
	{
		SEdge &edge = d_edgeList->EntryRef(i);
		DrawUtility::Line((*d_vertexList)[edge.d_start].d_vertex,
				(*d_vertexList)[edge.d_end].d_vertex, i_color);
	}
}

/***************************************************************************/
/***************************************************************************/
void CTriangleListCollisionObject::DrawNormals(DrawUtility::COLOR i_color)
{
	if (d_triangleList == NULL)
		return;

	Vector3 centroid;
	int numTriangles = d_triangleList->NumEntries();
	for (int i = 0; i < numTriangles; i++)
	{
		TerrainTriangle* triangle = (*d_triangleList)[i].d_triangle;
		centroid = (1.0f / 3.0f) * (triangle->A() + triangle->B() + triangle->C());
		DrawUtility::Normal(centroid, triangle->FaceNormal(), i_color);
	}
}

/***************************************************************************/
/***************************************************************************/
bool CTriangleListCollisionObject::GetCentroid(int i_index, Vector3 &i_centroid)
{
	if ((d_triangleList == NULL) || (i_index < 0) ||
			(i_index >= d_triangleList->NumEntries()))
		return(false);

	TerrainTriangle* triangle = (*d_triangleList)[i_index].d_triangle;
	i_centroid = (1.0f / 3.0f) * (triangle->A() + triangle->B() + triangle->C());
	return(true);
}

/***************************************************************************/
/***************************************************************************/
Vector3 CTriangleListCollisionObject::UnsafeGetCentroid(int i_index)
{
	TerrainTriangle* triangle = (*d_triangleList)[i_index].d_triangle;
	Vector3 centroid = (1.0f / 3.0f) * (triangle->A() + triangle->B() +
			triangle->C());
	return(centroid);
}
