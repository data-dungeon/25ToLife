////////////////////////////////////////////////////////////////////////////
//
// MeshConnectivity
//
// Class to handle build connectivity information for the TupTriMesh
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_TRI_MESH_CONNECTIVITY_H
#define TUPPER3D_TRI_MESH_CONNECTIVITY_H

#ifndef TUPPERWARE_KEEPER_H
#include "tupperware\keeper.h"
#endif

class TupTriMesh;

class TupTriMeshConnectivity
{
public:

class Vertex;
class MeshMap;
class Edge;
class TriFace;
class FaceEdgeLink;
class GetConnectedFacesStackEntry;
class GetConnectedPointsStackEntry;

private:

// just points to this data (does not own it)
	Vertex *m_pPositionVerts;
	Vertex *m_pNormalVerts;
	int *m_pPositionIndices;
	int *m_pNormalIndices;
	int *m_pFaceMaterialIndices;
	int *m_pFaceFlags;
	int *m_pFaceOpacityValues;
	int *m_pBoneVerts;

	int m_numPositionVerts;
	int m_numNormalVerts;
	int m_numFaces;


// owns this data
	TupArray<MeshMap> m_meshMaps;
	TupArray<TriFace> m_faces;

	TupArray<FaceEdgeLink> m_faceEdgeLinks;

	int m_faceFlagMask;
	int m_faceCheckFlags;

public:

	enum FaceCheckFlags
	{
		FLAGS_FACE_CHECK_MATERIAL_INDEX		= 0x0001,
		FLAGS_FACE_CHECK_FACE_FLAGS			= 0x0002,
		FLAGS_FACE_CHECK_FACE_OPACITY		= 0x0004,
		FLAGS_FACE_CHECK_NORMALS			= 0x0008,
		FLAGS_FACE_CHECK_MESHMAPS			= 0x0010,
	};

	TupTriMeshConnectivity(void);
	~TupTriMeshConnectivity();

	void SetMesh(TupTriMesh &tupTriMesh,int faceCheckFlags);
	void SetMeshFaceFlagMask(int faceFlagMask);

	int GetNumFaces(void) const;
	const TriFace &GetFace(int faceIndex) const;
	TriFace &GetFace(int faceIndex);
	int GetNumMeshMaps(void) const;
	MeshMap &GetMeshMap(int meshMapIndex);
	const MeshMap &GetMeshMap(int meshMapIndex) const;

	void AddAllConnectedFaces(int seedFaceIndex,Keeper<int> &connectedFaces) const;

	void GetAllConnectedPositionDistances(int seedPositionIndex,TupArray<int> &pointDistanceValues,bool bAddFacePoints=false,int maxDistance=1) const;
	void AddAllConnectedPositionIndices(int seedPositionIndex,TupArray<int> &connectedPositionIndices,bool bAddFacePoints=false,int maxDistance=1) const;
	void GetAllConnectedPositionFaceDistances(int seedPositionIndex,TupArray<int> &faceDistanceValues,int maxDistance=1) const; 

private:

	void ConnectFaces(void);
	void CheckFaces(void);
	void AddMeshMap(float *pMapVerts,int numMapVerts,int *pMapIndices,int mapChannel,int mapIndex);
	
};

#endif
