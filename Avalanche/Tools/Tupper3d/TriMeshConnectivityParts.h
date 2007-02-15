////////////////////////////////////////////////////////////////////////////
//
// MeshConnectivityParts
//
// Class to handle the parts MeshConnectivity class
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_TRI_MESH_CONNECTIVITY_PARTS_H
#define TUPPER3D_TRI_MESH_CONNECTIVITY_PARTS_H

#ifndef TUPPER3D_TRI_MESH_CONNECTIVITY_H
#include "TriMeshConnectivity.h"
#endif

class TupTriMeshConnectivity::Vertex
{
public:
	float x,y,z;
	bool Compare(Vertex &vertex,float threshold);
};

class TupTriMeshConnectivity::Edge
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

class TupTriMeshConnectivity::FaceEdgeLink
{
	TupTriMeshConnectivity::Edge m_nextLink;
public:
	FaceEdgeLink(void);
	FaceEdgeLink(const TupTriMeshConnectivity::FaceEdgeLink &s);
	FaceEdgeLink(const TupTriMeshConnectivity::Edge &nextLink);
	~FaceEdgeLink();
	
	const TupTriMeshConnectivity::Edge &GetNextLink(void) const;
	TupTriMeshConnectivity::Edge &GetNextLink(void);
	void SetNextLink(TupTriMeshConnectivity::Edge &nextLink);

	bool Valid(void) const;
	void SetInvalid(void);

};

class TupTriMeshConnectivity::TriFace
{
	TupTriMeshConnectivity::Edge m_edges[3];
	TupTriMeshConnectivity::FaceEdgeLink m_edgeLinks[3];
public:

	TriFace(void);
	const Edge &GetEdge(int edgeIndex) const;
	Edge &GetEdge(int edgeIndex);
	void SetEdge(TupTriMeshConnectivity::Edge &edge,int edgeIndex);

	const FaceEdgeLink &GetFaceEdgeLink(int edgeIndex) const;
	FaceEdgeLink &GetFaceEdgeLink(int edgeIndex);
	void SetFaceEdgeLink(TupTriMeshConnectivity::FaceEdgeLink &faceEdgeLink,int edgeIndex);
};

class TupTriMeshConnectivity::MeshMap
{
	// just points to this data (does not own it)
	TupTriMeshConnectivity::Vertex *m_pMapVerts;
	int *m_pMapIndices;
	int m_numMapVerts;
	int m_mapChannel;
	int m_mapIndex;

public:
	MeshMap(void);
	~MeshMap(void);
	void AddMapVerts(float *pMapVerts,int numMapVerts);
	void AddMapIndices(int *pMapIndices);
	void SetMapChannel(int mapChannel);
	void SetMapIndex(int mapIndex);

	int *GetMapIndices(void) const;
	TupTriMeshConnectivity::Vertex *GetVerts(void) const;
	int GetNumVerts(void) const;
	int GetMapChannel(void) const;
	int GetMapIndex(void) const;
};

class TupTriMeshConnectivity::GetConnectedFacesStackEntry
{
	int m_faceIndex;
	int m_stage;
public:
	GetConnectedFacesStackEntry(void);
	GetConnectedFacesStackEntry(int faceIndex);

	int GetFaceIndex(void) const;
	int GetStage(void) const;

	void SetFaceIndex(int faceIndex);
	void SetStage(int stage);
};

class TupTriMeshConnectivity::GetConnectedPointsStackEntry
{
	int m_faceIndex;
	int m_pointIndex;
	int m_distance;
public:
	GetConnectedPointsStackEntry(void);
	GetConnectedPointsStackEntry(int faceIndex,int pointIndex,int distance);
	
	int GetFaceIndex(void) const;
	int GetPointIndex(void) const;
	int GetDistance(void) const;
	
	void SetFaceIndex(int faceIndex);
	void SetPointIndex(int pointIndex);
	void SetDistance(int distance);
};


#include "TriMeshConnectivityParts.hpp"

#endif