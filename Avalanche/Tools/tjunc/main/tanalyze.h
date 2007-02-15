//***************************************************************************
// Class to analyze a triangle soup for tjunction problems.
//***************************************************************************
#ifndef TANALYZE_H
#define TANALYZE_H

#include <math/vector.h>
#include <container/hashlist.h>
#include <container/dimmap.h>

// The uber define....
#define COTOLERENCE	(0.00001f)

class Triangle
{
public:
	uint 	v1, v2, v3;
	uint	e1, e2, e3;

	inline int operator==(const Triangle &t) const
		{ return(v1 == t.v1 && v2 == t.v2 && v3 == t.v3); }
};

class Edge
{
public:
	uint v1, v2;

	inline int operator==(const Edge &e) const
		{ return(v1 == e.v1 && v2 == e.v2); }
};

class TJuncAnalyze
{
public:
	TJuncAnalyze(float vertexMergeToleranceSquared = COTOLERENCE);
	~TJuncAnalyze(void);

	// Pass me in triangles & quads
	bool AddTriangle(Vector3 &v1, Vector3 &v2, Vector3 &v3);
	bool AddQuad(Vector3 &v1, Vector3 &v2, Vector3 &v3, Vector3 &v4);

	// When you are done adding geometry, call this to stats
	typedef struct
	{
		// Input stats
		int		triangles;
		int		edges;
		int		vertices;

		// Redundancies
		int		redundantTriangles;
		int		degenerateTriangles;

		// Number of tjunctions & merged vertices
		int		mergedVertices;
		int		tJunctions;
	} Stats;
	const Stats &Analyze(void);

	// Return true if there is "something bad" in the stats
	bool SomethingBad(const Stats &stats)
		{ return(stats.redundantTriangles > 0 || stats.degenerateTriangles > 0 || stats.mergedVertices > 0 || stats.tJunctions > 0); }

private:
	// How close do vertices need to be to merge them?
	float										vertexMergeSquared;

	// Stats
	Stats										stats;

	// Our tables of traiangle data
	HashList<Vector3, 14>				vertexHash;
	HashList<Triangle, 12>				triangleHash;
	HashList<Edge, 14>					edgeHash;

	// Internals
	inline void Sort3(uint &a, uint &b, uint &c);
	inline void Swap(uint &a, uint &b);
	bool AddVertex(Vector3 &v, uint &index);
	bool AddEdge(uint v1, uint v2, uint &index);
	bool AddTriangle(Triangle &t);

	// Is p on the segment defined by a-b
	static bool Colinear(Vector3 &p, Vector3 &a, Vector3 &b, float toleranceSquared);
};

// Our hash keys for the tables
uint HashKey(Vector3 &v);
uint HashKey(Edge &e);
uint HashKey(Triangle &t);

#endif
