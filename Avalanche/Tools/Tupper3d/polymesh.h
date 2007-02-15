////////////////////////////////////////////////////////////////////////////
//
// PolyMesh
//
// Class to handle polygon meshes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: polymesh.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 8/18/03    Time: 3:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * multi sub object material converter
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/05/03    Time: 1:58p
 * Updated in $/Avalanche/tools/Tupper3d
 * added new flags
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/28/03    Time: 3:10p
 * Updated in $/Avalanche/tools/Tupper3d
 * polymesh and start of object converters
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_POLYMESH_H
#define TUPPER3D_POLYMESH_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;
class TupperwareList;

class TupObject;

class TupPolyMesh
{
	// Private Default constructor so no one can use it
	TupPolyMesh(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		POSITION_VERTS,
		POSITION_INDICES,
		NORMAL_VERTS,
		NORMAL_INDICES,
		FACE_VERTEX_COUNTS,
		FACE_FLAGS,
		FACE_MATERIAL_INDICES, // Obsolete
		FACE_NORMAL_VERTS,
		FACE_NORMAL_INDICES,
		MESH_MAPS,
		FACE_TRIANGLE_COUNTS,
		TRIANGLE_POINT_INDICES,
		FACE_MATERIAL_REFS,
		NUM_CHILDREN_ATOMS, // last entry
	};
// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:

	enum FaceFlags
	{
		FLAGS_FACE_HIDDEN							= 0x00000001,
		FLAGS_FACE_PLANAR							= 0x00000002,
		FLAGS_FACE_CONVEX							= 0x00000004,
		FLAGS_FACE_ZERO_AREA						= 0x00000008,
		FLAGS_FACE_HAS_HOLES						= 0x00000010,
	};

	TupPolyMesh(TupperwareAggregate *pObject);
	TupPolyMesh(TupObject &objectBase);

	void BuildAtomCache(void);
  
  TupperwareAggregate *GetBaseAggregate( void );

// add methods
	TupperwareList *AddPositionVerts(const float *pVerts,int numVerts);
	TupperwareList *AddPositionIndices(const int *pIndices,int numIndices);
	TupperwareList *AddNormalVerts(const float *pVerts,int numVerts);
	TupperwareList *AddNormalIndices(const int *pIndices,int numIndices);
	TupperwareList *AddFaceVertexCounts(const int *pCounts,int numFaces);
	TupperwareList *AddFaceNormalVerts(const float *pVerts,int numVerts);
	TupperwareList *AddFaceNormalIndices(const int *pIndices,int numFaces);
	TupperwareList *AddFaceFlags(const int *pFaceFlags,int numFaces);
	TupperwareList *AddFaceMaterialIndices(const int *pFaceMaterialIndices,int numFaces); // Obsolete
	TupperwareList *AddFaceTriangleCounts(const int *pCounts,int numFaces);
	TupperwareList *AddTrianglePointIndices(const int *pIndices,int numIndices);
	TupperwareList *AddFaceMaterialRefs(const int *pFaceMaterialRefs,int numFaces);

	TupperwareAggregate *AddMeshMap(int channelNum);

// get methods
	TupperwareList *GetPositionVerts(float **pVerts,int &numVerts);
	TupperwareList *GetPositionIndices(int **pIndices,int &numFaces);
	TupperwareList *GetNormalVerts(float **pVerts,int &numVerts);
	TupperwareList *GetNormalIndices(int **pIndices,int &numFaces);
	TupperwareList *GetFaceVertexCounts(int **pCounts,int &numFaces);
	TupperwareList *GetFaceNormalVerts(float **pVerts,int &numVerts);
	TupperwareList *GetFaceNormalIndices(int **pIndices,int &numFaces);
	TupperwareList *GetFaceFlags(int **pFaceFlags,int &numFaces);
	TupperwareList *GetFaceMaterialIndices(int **pFaceMaterialIndices,int &numFaces); // Obsolete
	TupperwareList *GetFaceTriangleCounts(int **pCounts,int &numFaces);
	TupperwareList *GetTrianglePointIndices(int **pIndices,int &numIndices);
	TupperwareList *GetFaceMaterialRefs(int **pFaceMaterialRefs,int &numFaces);

	int GetNumMeshMaps(void);
	TupperwareAggregate *GetMeshMap(int mapIndex);
	
private:
// add methods
	TupperwareList *AddFloatList(const float *pfloats,int numFloats,TupPolyMesh::ChildrenAtoms index);
	TupperwareList *AddIntList(const int *pInts,int numInts,TupPolyMesh::ChildrenAtoms index);

// get methods
	TupperwareList *GetFloatList(float **pFloats,int &numFloats,TupPolyMesh::ChildrenAtoms index);
	TupperwareList *GetIntList(int **pInts,int &numInts,TupPolyMesh::ChildrenAtoms index);
};

#endif
