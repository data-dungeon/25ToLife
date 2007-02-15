/*
**
**  File:   TriangleList.h
**  Date:   June 12, 2003
**  By:     Bryant Collard
**  Desc:   A collision object that is a list of terrain triangles. This object
**          is odd because it is transient in nature and uses a collision stack
**          to store information. Care must be taken in its use.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/TriangleList.h $
**  $Revision: 6 $
**      $Date: 7/01/03 11:54a $
**    $Author: Bcollard $
**
*/

#ifndef TRIANGLE_LIST_H
#define TRIANGLE_LIST_H

// Engine include //
#include "Math/Vector.h"

// Gamelib includes //
#include "collide/Object.h"
#include "collide/terraintri.h"
#include "EngineHelper/drawutil.h"

class CCollisionStack;
template<class T> class CCollisionStackArray;

struct STriangleEntry
{
	TerrainTriangle* d_triangle;
	Vector3 d_transformedNormal;
	Vector3 d_transformedVertex[3];
	int d_vertexIndex[3];
	int d_edgeIndex[3];
};

class CTriangleListCollisionObject : public CCollisionObject
{
  public:
	CTriangleListCollisionObject();
	~CTriangleListCollisionObject();

	bool Open(CCollisionStack* i_stack);
	void Close(void);
	bool Add(TerrainTriangle* i_triangle);
	bool Add(TerrainTriangle* i_triangle, Vector3CRef i_transformedNormal,
			Vector3CRef i_transformedVertexA, Vector3CRef i_transformedVertexB,
			Vector3CRef i_transformedVertexC);

	int NumTriangles(void);
	TerrainTriangle* GetTriangle(int i_index);
	TerrainTriangle* UnsafeGetTriangle(int i_index);
	STriangleEntry* GetTriangleEntry(int i_index);
	STriangleEntry* UnsafeGetTriangleEntry(int i_index);
	bool GetCentroid(int i_index, Vector3 &i_centroid);
	Vector3 UnsafeGetCentroid(int i_index);

	bool BuildVertexList(void);
	int NumVertices(void);
	Vector3* GetVertex(int i_index);
	Vector3* UnsafeGetVertex(int i_index);
	Vector3* GetTransformedVertex(int i_index);
	Vector3* UnsafeGetTransformedVertex(int i_index);

	bool BuildEdgeList(void);
	int NumEdges(void);
	bool GetEdge(int i_index, Vector3 &o_start, Vector3 &o_end);
	bool UnsafeGetEdge(int i_index, Vector3 &o_start, Vector3 &o_end);
	bool GetTransformedEdge(int i_index, Vector3 &o_start, Vector3 &o_end);
	bool UnsafeGetTransformedEdge(int i_index, Vector3 &o_start, Vector3 &o_end);

	bool BuildGroups(void);
	int NumGroups(void);

	class CGroupIterator
	{
	  public:
		CGroupIterator(CTriangleListCollisionObject &i_list);
		CGroupIterator(CTriangleListCollisionObject &i_list, int i_group);
		bool SetGroup(int i_group);
		int GetGroup(void);
		bool EndReached(void);

	  protected:
		CGroupIterator();
		CTriangleListCollisionObject &d_list;
		int d_group;
		int d_index;
		bool d_atEnd;
	};

	class CTriangleGroupIterator : public CGroupIterator
	{
	  public:
		CTriangleGroupIterator(CTriangleListCollisionObject &i_list);
		CTriangleGroupIterator(CTriangleListCollisionObject &i_list, int i_group);
		bool GotoStart(void);
		bool operator++();
		TerrainTriangle* Triangle(void);
		STriangleEntry* TriangleEntry(void);
		bool Centroid(Vector3 &o_centroid);

	  private:
		CTriangleGroupIterator();
	};

	class CVertexGroupIterator : public CGroupIterator
	{
	  public:
		CVertexGroupIterator(CTriangleListCollisionObject &i_list);
		CVertexGroupIterator(CTriangleListCollisionObject &i_list, int i_group);
		bool GotoStart(void);
		bool operator++();
		Vector3* Vertex(void);
		Vector3* TransformedVertex(void);

	  private:
		CVertexGroupIterator();
	};

	class CEdgeGroupIterator : public CGroupIterator
	{
	  public:
		CEdgeGroupIterator(CTriangleListCollisionObject &i_list);
		CEdgeGroupIterator(CTriangleListCollisionObject &i_list, int i_group);
		bool GotoStart(void);
		bool operator++();
		bool Edge(Vector3 &o_start, Vector3 &o_end);
		bool TransformedEdge(Vector3 &o_start, Vector3 &o_end);

	  private:
		CEdgeGroupIterator();
	};

	void Draw(DrawUtility::COLOR i_color = DrawUtility::GREEN);
	void DrawNormals(DrawUtility::COLOR i_color = DrawUtility::YELLOW);

  private:
	int AddVertex(Vector3CRef i_vertex, Vector3CRef i_transformedVertex);
	int AddEdge(int i_index1, int i_index2);

	struct SVertex
	{
		Vector3 d_vertex;
		Vector3 d_transformedVertex;
		int d_group;
	};
	struct SEdge
	{
		int d_start;
		int d_end;
	};

	CCollisionStack* d_stack;
	bool d_open;
	CCollisionStackArray<STriangleEntry>* d_triangleList;
	CCollisionStackArray<SVertex>* d_vertexList;
	CCollisionStackArray<SEdge>* d_edgeList;
	int d_numGroups;

	friend class CTriangleListCollisionObject::CGroupIterator;
	friend class CTriangleListCollisionObject::CTriangleGroupIterator;
	friend class CTriangleListCollisionObject::CVertexGroupIterator;
	friend class CTriangleListCollisionObject::CEdgeGroupIterator;
};

#define TRIANGLE_LIST_HPP
#include "collide/TriangleList.hpp"

#endif // TRIANGLE_LIST_H
