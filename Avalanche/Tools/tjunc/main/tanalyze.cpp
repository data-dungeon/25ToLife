/***************************************************************************/
// Triangle cluster
/***************************************************************************/
#include <layers/debug.h>

#include "tanalyze.h"

/***************************************************************************/
/***************************************************************************/
TJuncAnalyze::TJuncAnalyze(
float vertexMergeToleranceSquared) 
{
	vertexMergeSquared = vertexMergeToleranceSquared;
	memset(&stats, '\0', sizeof(stats));
}

/***************************************************************************/
/***************************************************************************/
TJuncAnalyze::~TJuncAnalyze(void)
{
}

/***************************************************************************/
// Add a triangle to the cluster, return false if the triangle cannot
// be added.  If it is added, fill out it's index
/***************************************************************************/
bool TJuncAnalyze::AddTriangle(
Vector3 &a,
Vector3 &b,
Vector3 &c)
{
	Triangle t;

	// Add the vertices
	if (!AddVertex(a, t.v1))
		return(false);
	if (!AddVertex(b, t.v2))
		return(false);
	if (!AddVertex(c, t.v3))
		return(false);

	// Degenerate triangle check
	if (t.v1 == t.v2 || t.v2 == t.v3 || t.v1 == t.v3)
	{
		++stats.degenerateTriangles;
		return(false);
	}

	// Add the edges
	if (!AddEdge(t.v1, t.v2, t.e1))
		return(false);
	if (!AddEdge(t.v2, t.v3, t.e2))
		return(false);
	if (!AddEdge(t.v3, t.v1, t.e3))
		return(false);

	// Make sure the triangle is uniform
	Sort3(t.v1, t.v2, t.v3);
	Sort3(t.e1, t.e2, t.e3);

	// this should not fail
	if (!AddTriangle(t))
		return(false);

	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool TJuncAnalyze::AddQuad(
Vector3 &v1,
Vector3 &v2,
Vector3 &v3,
Vector3 &v4)
{
	bool ret = AddTriangle(v1, v2, v3);
	ret = AddTriangle(v2, v3, v4) && ret;
	return(ret);
}

/***************************************************************************/
/***************************************************************************/
const TJuncAnalyze::Stats &TJuncAnalyze::Analyze(void)
{
	// Scan for vertices that are co-linear with an existing edge
	uint verts = vertexHash.Count();
	Vector3 *vertexList = vertexHash.Array();
	uint edges = edgeHash.Count();
	Edge *edgeList = edgeHash.Array();
	for (uint v = 0; v < verts; v++)
	{
		for (uint e = 0; e < edges; e++)
		{
			if (v != edgeList[e].v1 && v != edgeList[e].v2)
			{
				if (Colinear(vertexList[v], vertexList[edgeList[e].v1], vertexList[edgeList[e].v2], vertexMergeSquared))
				{
#ifdef _DEBUG
					TRACE("\tVertex %d t-junc's with edge [%d %d]\n", v, edgeList[e].v1, edgeList[e].v2);
#endif
					++stats.tJunctions;
				}
			}
		}
	}

	// Give them back the results
	return(stats);
}

/***************************************************************************/
/***************************************************************************/
inline void TJuncAnalyze::Sort3(
uint &a,
uint &b,
uint &c)
{
	if (a > b)
		Swap(a, b);
	if (b > c)
	{
		Swap(b, c);
		if (a > b)
			Swap(a, b);
	}
}

/***************************************************************************/
/***************************************************************************/
inline void TJuncAnalyze::Swap(
uint &a,
uint &b)
{
	uint temp = a;
	a = b;
	b = temp;
}

/***************************************************************************/
// add a vertex, return it's index.  Return false
// if we are full.
/***************************************************************************/
bool TJuncAnalyze::AddVertex(
Vector3 &v,
uint &index)
{
	// Is it already in the list?
	unsigned long fool;
	if (vertexHash.Find(v, fool))
	{
		index = fool;
		return(true);
	}

	// Not in the list, lets try to merge
	uint count = vertexHash.Count();
	Vector3 *vertexList = vertexHash.Array();
	for (uint i = 0; i < count; i++)
	{
		float d = (vertexList[i] - v).LengthSquared();
		if (d < vertexMergeSquared)
		{
			index = i;
			++stats.mergedVertices;
			return(true);
		}
	}

	// Didn't find it, add it
	index = vertexHash.Add(v);
	ASSERT(index == vertexHash.Count() - 1);
	++stats.vertices;

	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool TJuncAnalyze::AddEdge(
uint v1,
uint v2,
uint &index)
{
	// make the edge
	Edge e;
	if (v1 < v2)
	{
		e.v1 = v1;
		e.v2 = v2;
	}
	else
	{
		e.v1 = v2;
		e.v2 = v1;
	}

	// Does it already exist?
	index = edgeHash.Add(e);
	if (index == edgeHash.Count() - 1)
	{
		// One more edge
		++stats.edges;
	}

	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool TJuncAnalyze::AddTriangle(
Triangle &t)
{
	// Does it already exist?
	uint index = triangleHash.Add(t);
	if (index == triangleHash.Count() - 1)
		++stats.triangles;
	else
		++stats.redundantTriangles;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool TJuncAnalyze::Colinear(
Vector3 &p,
Vector3 &a,
Vector3 &b,
float toleranceSquared)
{
	// Make segment into a true ray
	Vector3 v = (b - a).Normalized();

	// Find the perpendicular point (v is unit)
	float t = Vector3::Dot(p - a, v);
	if (t < 0.0f || t > 1.0f)
		return(false);
	Vector3 perp = a + v * t;

	// What is the distance?
	float d = (p - perp).LengthSquared();
	return(d < toleranceSquared);
}

/***************************************************************************/
// hash key for vector
/***************************************************************************/
uint HashKey(
Vector3 &v)
{
	return((uint)((Math::Abs(v.x) + Math::Abs(v.z)) * 100.0f));
}

/***************************************************************************/
// hash key for edge
/***************************************************************************/
uint HashKey(
Edge &e)
{
	return(e.v1 + e.v2);
}

/***************************************************************************/
// hash key for triangle
/***************************************************************************/
uint HashKey(
Triangle &t)
{
	return(t.v1 + t.v2 + t.v3);
}


