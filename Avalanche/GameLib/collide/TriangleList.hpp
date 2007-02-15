/*
**
**  File:   TriangleList.hpp
**  Date:   June 12, 2003
**  By:     Bryant Collard
**  Desc:   A collision object that is a list of terrain triangles. This object
**          is odd because it is transient in nature and uses a collision stack
**          to store information. Care must be taken in its use.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/TriangleList.hpp $
**  $Revision: 4 $
**      $Date: 7/01/03 11:54a $
**    $Author: Bcollard $
**
*/

#ifdef TRIANGLE_LIST_HPP
#undef TRIANGLE_LIST_HPP

#include "collide/Stack.h"
#include "collide/StackArray.h"

/***************************************************************************/
/***************************************************************************/
inline CTriangleListCollisionObject::CTriangleListCollisionObject() :
	CCollisionObject(TRIANGLE_LIST)
{
	d_stack = NULL;
	d_open = false;
	d_triangleList = NULL;
	d_vertexList = NULL;
	d_edgeList = NULL;
	d_numGroups = 0;
}

/***************************************************************************/
/***************************************************************************/
inline CTriangleListCollisionObject::~CTriangleListCollisionObject()
{
	CCollisionStackItem::Delete(d_edgeList);
	CCollisionStackItem::Delete(d_vertexList);
	CCollisionStackItem::Delete(d_triangleList);
}

/***************************************************************************/
/***************************************************************************/
inline bool CTriangleListCollisionObject::Open(CCollisionStack* i_stack)
{
	ASSERT(!d_open);

	CCollisionStackItem::Delete(d_edgeList);
	CCollisionStackItem::Delete(d_vertexList);
	CCollisionStackItem::Delete(d_triangleList);

	d_stack = i_stack;
	d_vertexList = NULL;
	d_edgeList = NULL;
	d_numGroups = 0;

	d_triangleList = CCollisionStackArray<STriangleEntry>::New(d_stack);
	if (d_triangleList == NULL)
		return(false);

	d_open = true;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
inline void CTriangleListCollisionObject::Close(void)
{
	if (d_open)
	{
		int count = d_triangleList->NumEntries();
		if (count <= 0)
		{
			CCollisionStackItem::Delete(d_triangleList);
			d_triangleList = NULL;
		}
		else
		{
			count *= 3;
			d_vertexList = CCollisionStackArray<SVertex>::New(d_stack);
			if ((d_vertexList != NULL) && !d_vertexList->AllocateArray(count))
			{
				CCollisionStackItem::Delete(d_vertexList);
				d_vertexList = NULL;
			}
			d_edgeList = CCollisionStackArray<SEdge>::New(d_stack);
			if ((d_edgeList != NULL) && !d_edgeList->AllocateArray(count))
			{
				CCollisionStackItem::Delete(d_edgeList);
				d_edgeList = NULL;
			}
		}
		d_open = false;
	}
}

/***************************************************************************/
/***************************************************************************/
inline bool CTriangleListCollisionObject::Add(TerrainTriangle* i_triangle)
{
	ASSERT(d_open);

	STriangleEntry* entry = d_triangleList->ExpandArray();
	if (entry == NULL)
		return(false);

	entry->d_triangle = i_triangle;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
inline bool CTriangleListCollisionObject::Add(TerrainTriangle* i_triangle,
		Vector3CRef i_transformedNormal, Vector3CRef i_transformedVertexA,
		Vector3CRef i_transformedVertexB, Vector3CRef i_transformedVertexC)
{
	ASSERT(d_open);

	STriangleEntry* entry = d_triangleList->ExpandArray();
	if (entry == NULL)
		return(false);

	entry->d_triangle = i_triangle;
	entry->d_transformedNormal = i_transformedNormal;
	entry->d_transformedVertex[0] = i_transformedVertexA;
	entry->d_transformedVertex[1] = i_transformedVertexB;
	entry->d_transformedVertex[2] = i_transformedVertexC;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
inline int CTriangleListCollisionObject::NumTriangles(void)
{
	return((d_triangleList == NULL) ? 0 : d_triangleList->NumEntries());
}

/***************************************************************************/
/***************************************************************************/
inline TerrainTriangle* CTriangleListCollisionObject::GetTriangle(int i_index)
{
	if ((d_triangleList == NULL) || (i_index < 0) ||
			(i_index >= d_triangleList->NumEntries()))
		return(NULL);
	return((*d_triangleList)[i_index].d_triangle);
}

/***************************************************************************/
/***************************************************************************/
inline TerrainTriangle* CTriangleListCollisionObject::UnsafeGetTriangle(
		int i_index)
{
	return((*d_triangleList)[i_index].d_triangle);
}

/***************************************************************************/
/***************************************************************************/
inline STriangleEntry* CTriangleListCollisionObject::GetTriangleEntry(
		int i_index)
{
	if ((d_triangleList == NULL) || (i_index < 0) ||
			(i_index >= d_triangleList->NumEntries()))
		return(NULL);
	return(d_triangleList->EntryPtr(i_index));
}

/***************************************************************************/
/***************************************************************************/
inline STriangleEntry* CTriangleListCollisionObject::UnsafeGetTriangleEntry(
		int i_index)
{
	return(d_triangleList->EntryPtr(i_index));
}

/***************************************************************************/
/***************************************************************************/
inline int CTriangleListCollisionObject::NumVertices(void)
{
	return((d_vertexList == NULL) ? 0 : d_vertexList->NumEntries());
}

/***************************************************************************/
/***************************************************************************/
inline Vector3* CTriangleListCollisionObject::GetVertex(int i_index)
{
	if ((d_vertexList == NULL) || (i_index < 0) ||
			(i_index >= d_vertexList->NumEntries()))
		return(NULL);
	return(&(*d_vertexList)[i_index].d_vertex);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3* CTriangleListCollisionObject::UnsafeGetVertex(int i_index)
{
	return(&(*d_vertexList)[i_index].d_vertex);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3* CTriangleListCollisionObject::GetTransformedVertex(int i_index)
{
	if ((d_vertexList == NULL) || (i_index < 0) ||
			(i_index >= d_vertexList->NumEntries()))
		return(NULL);
	return(&(*d_vertexList)[i_index].d_transformedVertex);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3* CTriangleListCollisionObject::UnsafeGetTransformedVertex(
		int i_index)
{
	return(&(*d_vertexList)[i_index].d_transformedVertex);
}

/***************************************************************************/
/***************************************************************************/
inline int CTriangleListCollisionObject::NumEdges(void)
{
	return((d_edgeList == NULL) ? 0 : d_edgeList->NumEntries());
}

/***************************************************************************/
/***************************************************************************/
inline bool CTriangleListCollisionObject::GetEdge(int i_index,
		Vector3 &o_start, Vector3 &o_end)
{
	if ((d_edgeList == NULL) || (i_index < 0) ||
			(i_index >= d_edgeList->NumEntries()))
		return(false);

	SEdge &edge = d_edgeList->EntryRef(i_index);
	o_start = (*d_vertexList)[edge.d_start].d_vertex;
	o_end = (*d_vertexList)[edge.d_end].d_vertex;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
inline bool CTriangleListCollisionObject::UnsafeGetEdge(int i_index,
		Vector3 &o_start, Vector3 &o_end)
{
	SEdge &edge = d_edgeList->EntryRef(i_index);
	o_start = (*d_vertexList)[edge.d_start].d_vertex;
	o_end = (*d_vertexList)[edge.d_end].d_vertex;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
inline bool CTriangleListCollisionObject::GetTransformedEdge(int i_index,
		Vector3 &o_start, Vector3 &o_end)
{
	if ((d_edgeList == NULL) || (i_index < 0) ||
			(i_index >= d_edgeList->NumEntries()))
		return(false);

	SEdge &edge = d_edgeList->EntryRef(i_index);
	o_start = (*d_vertexList)[edge.d_start].d_transformedVertex;
	o_end = (*d_vertexList)[edge.d_end].d_transformedVertex;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
inline bool CTriangleListCollisionObject::UnsafeGetTransformedEdge(int i_index,
		Vector3 &o_start, Vector3 &o_end)
{
	SEdge &edge = d_edgeList->EntryRef(i_index);
	o_start = (*d_vertexList)[edge.d_start].d_transformedVertex;
	o_end = (*d_vertexList)[edge.d_end].d_transformedVertex;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
inline int CTriangleListCollisionObject::NumGroups(void)
{
	return(d_numGroups);
}

/***************************************************************************/
/***************************************************************************/
inline CTriangleListCollisionObject::CGroupIterator::CGroupIterator(
		CTriangleListCollisionObject &i_list) :
	d_list(i_list)
{
	d_group = -2;
	d_atEnd = true;
}

/***************************************************************************/
/***************************************************************************/
inline CTriangleListCollisionObject::CGroupIterator::CGroupIterator(
		CTriangleListCollisionObject &i_list, int i_group) :
	d_list(i_list)
{
	ASSERT((i_group >= 0) && (i_group < d_list.d_numGroups));
	if ((i_group >= 0) && (i_group < d_list.d_numGroups))
		d_group = i_group;
	else
		d_group = -2;
	d_atEnd = true;
}

/***************************************************************************/
/***************************************************************************/
inline bool CTriangleListCollisionObject::CGroupIterator::SetGroup(int i_group)
{
	ASSERT((i_group >= 0) && (i_group < d_list.d_numGroups));
	if ((i_group != d_group) && (i_group >= 0) && (i_group < d_list.d_numGroups))
	{
		d_group = i_group;
		d_atEnd = true;
		return(true);
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
inline int CTriangleListCollisionObject::CGroupIterator::GetGroup(void)
{
	return(d_group);
}

/***************************************************************************/
/***************************************************************************/
inline bool CTriangleListCollisionObject::CGroupIterator::EndReached(void)
{
	return(d_atEnd);
}

/***************************************************************************/
/***************************************************************************/
inline CTriangleListCollisionObject::CTriangleGroupIterator::CTriangleGroupIterator(
		CTriangleListCollisionObject &i_list) :
	CGroupIterator(i_list)
{
}

/***************************************************************************/
/***************************************************************************/
inline CTriangleListCollisionObject::CTriangleGroupIterator::CTriangleGroupIterator(
		CTriangleListCollisionObject &i_list, int i_group) :
	CGroupIterator(i_list, i_group)
{
}

/***************************************************************************/
/***************************************************************************/
inline TerrainTriangle*
		CTriangleListCollisionObject::CTriangleGroupIterator::Triangle(void)
{
	if (!d_atEnd)
		return(d_list.UnsafeGetTriangle(d_index));
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
inline STriangleEntry*
		CTriangleListCollisionObject::CTriangleGroupIterator::TriangleEntry(void)
{
	if (!d_atEnd)
		return(d_list.UnsafeGetTriangleEntry(d_index));
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
inline bool CTriangleListCollisionObject::CTriangleGroupIterator::Centroid(
		Vector3 &o_centroid)
{
	if (!d_atEnd)
	{
		o_centroid = d_list.UnsafeGetCentroid(d_index);
		return(true);
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
inline CTriangleListCollisionObject::CVertexGroupIterator::CVertexGroupIterator(
		CTriangleListCollisionObject &i_list) :
	CGroupIterator(i_list)
{
}

/***************************************************************************/
/***************************************************************************/
inline CTriangleListCollisionObject::CVertexGroupIterator::CVertexGroupIterator(
		CTriangleListCollisionObject &i_list, int i_group) :
	CGroupIterator(i_list, i_group)
{
}

/***************************************************************************/
/***************************************************************************/
inline Vector3* CTriangleListCollisionObject::CVertexGroupIterator::Vertex(void)
{
	if (!d_atEnd)
		return(d_list.UnsafeGetVertex(d_index));
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3*
		CTriangleListCollisionObject::CVertexGroupIterator::TransformedVertex(void)
{
	if (!d_atEnd)
		return(d_list.UnsafeGetTransformedVertex(d_index));
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
inline CTriangleListCollisionObject::CEdgeGroupIterator::CEdgeGroupIterator(
		CTriangleListCollisionObject &i_list) :
	CGroupIterator(i_list)
{
}

/***************************************************************************/
/***************************************************************************/
inline CTriangleListCollisionObject::CEdgeGroupIterator::CEdgeGroupIterator(
		CTriangleListCollisionObject &i_list, int i_group) :
	CGroupIterator(i_list, i_group)
{
}

/***************************************************************************/
/***************************************************************************/
inline bool CTriangleListCollisionObject::CEdgeGroupIterator::Edge(
		Vector3 &o_start, Vector3 &o_end)
{
	if (!d_atEnd)
		return(d_list.UnsafeGetEdge(d_index, o_start, o_end));
	return(false);
}

/***************************************************************************/
/***************************************************************************/
inline bool CTriangleListCollisionObject::CEdgeGroupIterator::TransformedEdge(
		Vector3 &o_start, Vector3 &o_end)
{
	if (!d_atEnd)
		return(d_list.UnsafeGetTransformedEdge(d_index, o_start, o_end));
	return(false);
}

#endif // TRIANGLE_LIST_HPP
