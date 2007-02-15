////////////////////////////////////////////////////////////////////////////
//
// TriMesh
//
// Class to handle triangle meshes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: trimesh.h $
 * 
 * *****************  Version 23  *****************
 * User: Adam Clayton Date: 9/02/03    Time: 2:59p
 * Updated in $/Avalanche/tools/Tupper3d
 * setting for clearing two sided faces (for ps2)
 * check # of bones for no receive shadow check for gamecube
 * 
 * *****************  Version 22  *****************
 * User: Adam Clayton Date: 8/26/03    Time: 1:05p
 * Updated in $/Avalanche/tools/Tupper3d
 * no receive shadows for GCN
 * 
 * *****************  Version 21  *****************
 * User: Adam Clayton Date: 8/18/03    Time: 3:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * multi sub object material converter
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 10:56a
 * Updated in $/Avalanche/tools/Tupper3d
 * move mesh to trimesh
 * 
 * *****************  Version 19  *****************
 * User: Dwain Skinner Date: 3/25/03    Time: 1:08p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 1/24/03    Time: 4:59p
 * Updated in $/Avalanche/tools/Tupper3d
 * added 8 bit texture alpha flag
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 8/16/02    Time: 3:42p
 * Updated in $/Avalanche/tools/Tupper3d
 * added stage flag
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 7/02/02    Time: 4:14p
 * Updated in $/Avalanche/tools/Tupper3d
 * Mesh flags renamed (conflict with Max)
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 6/26/02    Time: 2:41p
 * Updated in $/Avalanche/tools/Tupper3d
 * More Mesh Flags
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 6/25/02    Time: 4:17p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 6/25/02    Time: 3:32p
 * Updated in $/Avalanche/tools/Tupper3d
 * added face flags (only using 4 bits for now)
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 3/22/02    Time: 12:54p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added class references for import export
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 2/27/02    Time: 2:24p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Triangle Strip classes
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 2/12/02    Time: 5:12p
 * Updated in $/Avalanche/tools/Tupper3d
 * Remove Bounding Box 
 * Moved to process step in ExportPC filter
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 2/07/02    Time: 3:01p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 2/07/02    Time: 2:14p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Bounding Box and Bounding Sphere
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 1/16/02    Time: 3:44p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added constructors that use the parent as a parameter
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/28/01   Time: 4:30p
 * Updated in $/Avalanche/tools/Tupper3d
 * spelled indices correctly
 * 
 * *****************  Version 4  *****************
 * User: Shon         Date: 12/18/01   Time: 3:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/06/01   Time: 1:51p
 * Updated in $/Avalanche/tools/Tupper3d
 * Add and Get functions complete
 * Added new files for handling various parts of the graphics format
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/05/01   Time: 4:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * Level A
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_TRI_MESH_H
#define TUPPER3D_TRI_MESH_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;
class TupperwareList;

class TupObject;

class TupTriMesh
{
	// Private Default constructor so no one can use it
	TupTriMesh(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		POSITION_VERTS,
		POSITION_INDICES,
		NORMAL_VERTS,
		NORMAL_INDICES,
		FACE_FLAGS,
		FACE_MATERIAL_INDICES,
		FACE_SHADING_GROUPS,
		FACE_NORMAL_VERTS,
		FACE_NORMAL_INDICES,
		MESH_MAPS,
		TRIANGLE_STRIPS,
		FACE_MATERIAL_REFS,
		NUM_CHILDREN_ATOMS, // last entry
	};
// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:

	enum FaceFlags
	{
		FLAGS_EDGE_0_VISIBLITY					= 0x00000001, // these 3 flags are for constructing a poly from a series of triangles
		FLAGS_EDGE_1_VISIBLITY					= 0x00000002, // and are currently ignored
		FLAGS_EDGE_2_VISIBLITY					= 0x00000004,
		FLAGS_FACE_HIDDEN							= 0x00000008, // if this is set it will not be part of any tristrip
		// the following bits are not set in TupExport but are used in the filter chain
		FLAGS_FACE_Z_BIAS_MASK					= 0x000000f0, // comes from the face attributes to change the drawing order of the faces
		FLAGS_FACE_Z_BIAS_SHIFT					= 4,
		FLAGS_FACE_TWO_SIDED						= 0x00000100, // if the faces if backfaced culled or not
		FLAGS_FACE_HAS_TEXTURE					= 0x00000200, // if this face is using a texture
		FLAGS_FACE_HAS_TEXTURE_ALPHA			= 0x00000400, // if this face has a texture with alpha
		FLAGS_FACE_HAS_TEXTURE_8BIT_ALPHA	= 0x00000800, // if this face has a texture with 8 bit alpha
		FLAGS_FACE_USES_CLOTH					= 0x00001000, // if this face is marked as having cloth
		FLAGS_FACE_NO_RECEIVE_SHADOWS			= 0x00002000, // if this face is marked to not recieve shadows
		FLAGS_FACE_NO_Z_WRITE					= 0x00004000, // if this face is not going to write to the z buffer
		FLAGS_FACE_STAGE_MASK					= 0x000f0000, // which stage this face is drawn used when drawing multiple passes
		FLAGS_FACE_STAGE_SHIFT					= 16,
		FLAGS_FACE_LIGHTING_SHIFT				= 21,
	};

	TupTriMesh(TupperwareAggregate *pObject);
	TupTriMesh(TupObject &objectBase);

	void BuildAtomCache(void);
  
  TupperwareAggregate *GetBaseAggregate( void );

// add methods
	TupperwareList *AddPositionVerts(const float *pVerts,int numVerts);
	TupperwareList *AddPositionIndices(const int *pIndices,int numFaces);
	TupperwareList *AddNormalVerts(const float *pVerts,int numVerts);
	TupperwareList *AddNormalIndices(const int *pIndices,int numFaces);
	TupperwareList *AddFaceNormalVerts(const float *pVerts,int numVerts);
	TupperwareList *AddFaceNormalIndices(const int *pIndices,int numFaces);
	TupperwareList *AddFaceFlags(const int *pFaceFlags,int numFaces);
	TupperwareList *AddFaceMaterialIndices(const int *pFaceMaterialIndices,int numFaces); // Obsolete
	TupperwareList *AddFaceShadingGroups(const int *pFaceShadingGroups,int numFaces); // Obsolete
	TupperwareList *AddFaceMaterialRefs(const int *pFaceMaterialRefs,int numFaces);
	TupperwareAggregate *AddMeshMap(int channelNum);
	TupperwareAggregate *AddTriangleStrips(void);

// get methods
	TupperwareList *GetPositionVerts(float **pVerts,int &numVerts);
	TupperwareList *GetPositionIndices(int **pIndices,int &numFaces);
	TupperwareList *GetNormalVerts(float **pVerts,int &numVerts);
	TupperwareList *GetNormalIndices(int **pIndices,int &numFaces);
	TupperwareList *GetFaceNormalVerts(float **pVerts,int &numVerts);
	TupperwareList *GetFaceNormalIndices(int **pIndices,int &numFaces);
	TupperwareList *GetFaceFlags(int **pFaceFlags,int &numFaces);
	TupperwareList *GetFaceMaterialIndices(int **pFaceMaterialIndices,int &numFaces); // Obsolete
	TupperwareList *GetFaceShadingGroups(int **pFaceShadingGroups,int &numFaces); // Obsolete
	TupperwareList *GetFaceMaterialRefs(int **pFaceMaterialRefs,int &numFaces);
	int GetNumMeshMaps(void);
	TupperwareAggregate *GetMeshMap(int mapIndex);
	TupperwareAggregate *GetTriangleStrips(void);

private:
// add methods
	TupperwareList *AddFloatList(const float *pfloats,int numFloats,TupTriMesh::ChildrenAtoms index);
	TupperwareList *AddIntList(const int *pInts,int numInts,TupTriMesh::ChildrenAtoms index);

// get methods
	TupperwareList *GetFloatList(float **pFloats,int &numFloats,TupTriMesh::ChildrenAtoms index);
	TupperwareList *GetIntList(int **pInts,int &numInts,TupTriMesh::ChildrenAtoms index);
};

#endif
