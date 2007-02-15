////////////////////////////////////////////////////////////////////////////
//
// Parts
//
// Class to handle the parts of the StripBuilder class
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef STRIP_BUILDER_PARTS_H
#define STRIP_BUILDER_PARTS_H

#ifndef STRIP_BUILDER_H
#include "stripbuilder.h"
#endif

class StripBuilder::Vertex
{
public:
	float x,y,z;
	bool Compare(Vertex &vertex,float threshold);
};

class StripBuilder::Edge
{
	int m_faceIndex;
	int m_pointIndex;
public:
	Edge(void);
	Edge(int faceIndex,int pointIndex);
	int GetFaceIndex(void) const;
	int GetPointIndex(void) const;
	void SetFaceIndex(int faceIndex);
	void SetPointIndex(int pointIndex);
};

class StripBuilder::TriFace
{
	StripBuilder::Edge m_edges[3];
	int m_flags;
	int m_pathNumSplits; // number of descendant faces that have 2 children
	int m_pathSplitCredit; // number to add to the numsplits from the descendants that have been peeled off
	int m_pathParentFace;
	int m_stripIndex; // which strip this face belongs to
public:
	enum Flags
	{
		PATH_0		= 1<<0,
		PATH_1		= 1<<1,
		PATH_2		= 1<<2,
		PATHS		= 7<<0,
		VISITED		= 1<<4,
		SPLIT		= 1<<5,
	};

	TriFace(void);
	Edge &GetEdge(int edgeIndex);
	int GetFlags(void) const;
	bool PathEnabled(int edgeIndex) const;
	int GetPathNumSplits(void) const;
	int GetPathSplitCredit(void) const; 
	int GetPathParentFace(void) const;
	int GetStripIndex(void) const;
	int GetPathChildFace(void) const;
	int GetPathChildEdge(void) const;

	void SetEdge(StripBuilder::Edge &edge,int edgeIndex);
	void SetFlags(int flags);
	void EnablePath(int edgeIndex);
	void DisablePath(int edgeIndex);
	void ClearPaths(void);
	void SetPathNumSplits(int pathNumSplits);
	void SetPathSplitCredit(int pathSplitCredit);
	void SetPathParentFace(int pathParentFace);
	void SetStripIndex(int stripIndex);
};

class StripBuilder::MeshMap
{
	// just points to this data (does not own it)
	StripBuilder::Vertex *m_pMapVerts;
	int *m_pMapIndices;
	int m_numMapVerts;

public:
	MeshMap(void);
	~MeshMap(void);
	void AddMapVerts(float *pMapVerts,int numMapVerts);
	void AddMapIndices(int *pMapIndices);

	int *GetMapIndices(void);
	StripBuilder::Vertex *GetMapVerts(void);
	int GetNumMapVerts(void);
};

class StripBuilder::VertexColorMap
{
	// just points to this data (does not own it)
	StripBuilder::Vertex *m_pColorVerts;
	int *m_pColorIndices;
	int m_numColorVerts;

public:
	VertexColorMap(void);
	~VertexColorMap(void);
	void AddColorVerts(float *pColorVerts,int numColorVerts);
	void AddColorIndices(int *pColorIndices);

	int *GetColorIndices(void);
	StripBuilder::Vertex *GetColorVerts(void);
	int GetNumColorVerts(void);
};


class StripBuilder::FaceEdgeLink
{
	StripBuilder::Edge m_edge;
	StripBuilder::FaceEdgeLink *m_pNextLink; // does not own this but points to it
	int m_endPointIndex;
public:
	FaceEdgeLink(void);
	FaceEdgeLink(const StripBuilder::Edge &edge,StripBuilder::FaceEdgeLink *nextLink,int endPointIndex);
	~FaceEdgeLink();

	StripBuilder::Edge &GetEdge(void);
	StripBuilder::FaceEdgeLink *GetNextLink(void);
	int GetEndPointIndex(void);

	void SetEdge(StripBuilder::Edge &edge);
	void SetNextLink(StripBuilder::FaceEdgeLink *pNextLink);
	void SetEndPointIndex(int endPointIndex);
};

class StripBuilder::BuildPathStackEntry
{
	int m_faceIndex;
	int m_nextPointIndex;
	int m_prevPointIndex;
	int m_stage;
	bool m_parity;
public:
	BuildPathStackEntry(void);
	BuildPathStackEntry(int faceIndex,int nextPointIndex,int prevPointIndex,bool parity);

	int GetFaceIndex(void) const;
	int GetNextPointIndex(void) const;
	int GetPrevPointIndex(void) const;
	bool GetParity(void) const;
	int GetStage(void) const;

	void SetFaceIndex(int faceIndex);
	void SetNextPointIndex(int nextPointIndex);
	void SetPrevPointIndex(int prevPointIndex);
	void SetParity(bool parity);
	void SetStage(int stage);
};

class StripBuilder::PeelPathStackEntry
{
	int m_faceIndex;
	int m_stage;
public:
	PeelPathStackEntry(void);
	PeelPathStackEntry(int faceIndex);

	int GetFaceIndex(void) const;
	int GetStage(void) const;

	void SetFaceIndex(int faceIndex);
	void SetStage(int stage);
};

class StripBuilder::Strip
{
	int m_startFace;
	int m_endFace;
	int m_numFaces;
	int m_flags;
	int m_numPoints;
public:
	enum Flags
	{
		REMOVED	= 1<<0,
	};

	Strip(void);
	Strip(int startFace,int endFace,int numFaces);

	int GetStartFace(void) const;
	int GetEndFace(void) const;
	int GetNumFaces(void) const;
	int GetFlags(void) const;
	int GetNumPoints(void) const;

	void SetStartFace(int startFace);
	void SetEndFace(int endFace);
	void SetNumFaces(int numFaces);
	void SetFlags(int flags);
	void SetNumPoints(int numPoints);
};

class StripBuilder::PointStrip
{
	TupArray<int> m_faceIndices;
	TupArray<int> m_pointIndices;

	// todo add uv offsets

	int m_numTriangles;
public:
	PointStrip(void);

	int GetNumPoints(void) const;
	int GetFaceIndex(int index) const;
	int GetPointIndex(int index) const;
	const int *GetFaceIndices(void) const;
	const int *GetPointIndices(void) const;
	const int GetNumTriangles(void) const;

	void SetNumPoints(int numPoints);
	void SetFaceIndex(int index,int faceIndex);
	void SetPointIndex(int index,int pointIndex);
	int AddFaceIndex(int faceIndex);
	int AddPointIndex(int pointIndex);
	void SetNumTriangles(int numTriangles);
	void Init(void);
};

class StripBuilder::StripSortData
{
public:
	int m_stripIndex;
	TupCompareator<int> m_compareData;
	StripSortData(void);
	StripSortData(const StripSortData &src);
	const StripSortData& operator=(const StripSortData& src);
	bool operator==(const StripSortData& other) const;
	bool operator<=(const StripSortData& other) const;
	bool operator>=(const StripSortData& other) const;
	bool operator<(const StripSortData& other) const;
	bool operator>(const StripSortData& other) const;
};

class StripBuilder::MinMaxUV
{
public:
	float m_minU,m_minV;
	float m_maxU,m_maxV;

	MinMaxUV(void);
	bool Test(const Vector3 &limitUV) const;
	bool TestCoor(const Vector3 &coor,const Vector3 &limitUV) const;
	void AddCoor(const Vector3 &coor);
	void Init(void);
	void Combine(const MinMaxUV &minMaxUV);
	bool TestCombine(const MinMaxUV &minMaxUV,const Vector3 &limitUV) const;
};

class StripBuilder::SavedStrip
{
	TupArray<int> m_faceIndices;
	TupArray<StripBuilder::TriFace> m_savedFaces;
	StripBuilder::Strip m_savedStrip;
public:
	SavedStrip(void);
	void SetStrip(const StripBuilder::Strip &strip);
	void AddFace(int faceIndex,const StripBuilder::TriFace &triFace);

	const StripBuilder::Strip &GetStrip(void) const;
	int GetNumFaces(void) const;
	const StripBuilder::TriFace &GetFace(int index) const;
	int GetFaceIndex(int index) const;
};

class StripBuilder::Stats
{
public:
	Stats(void);
	void Init(void);

	// face connection stats
	int m_totalFaceConnectionGroups;
	int m_totalConnections;
	int m_connectionBreakMaterialIndex;
	int m_connectionBreakFaceFlags;
	int m_connectionBreakVertexNormals;
	int m_connectionBreakMeshMaps;
	int m_connectionBreakVertexColors;
	int m_connectionBreakVertexAlphas;
	int m_connectionBreakStages;

	// build path stats
	int m_buildPathBreaksBones;
	int m_buildPathBreaksUVLimit;

	// build strips from paths
	int m_buildStripFromPathOneFace;
	int m_buildStripFromPathTwoFace;
	int m_buildStripFromPathBadEdge;
	int m_buildStripFromPathBadEvenStrip;
	int m_buildStripFromPathBadEvenStripEdge;

	// combine strips
	int m_combineStrips;

	// sorted strip stats
	int m_sortedStripsBreakMaterialIndex;
	int m_sortedStripsBreakFaceFlags;

   // vertex cache stats
   int m_vertexCacheMisses;
   int m_vertexCacheHits;

};

class StripBuilder::VertexCache
{
   VertexCache(void); // must use other constructor
   TupArray<int> m_verts;
   int m_nextPos;
   int m_count;
public:
   VertexCache(int maxEntries);
   VertexCache(const VertexCache &src);
   const VertexCache &operator=(const VertexCache &src);
   bool InCache(int value) const;
   bool AddValue(int value);
   int GetMaxEntries(void) const;
   int GetNumValues(void) const;
};

#include "parts.hpp"

#endif
