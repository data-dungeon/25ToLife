////////////////////////////////////////////////////////////////////////////
//
// ExportClothSOM
//
// Export Cloth SOMS into the DBL file (dynamic meshes)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportClothSOM.h $
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:26a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 10:39a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * moved mesh connectivity to tupper3d
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 2/07/03    Time: 11:25a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * set up max uv channels to come from the tupmeshmap class
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 10/01/02   Time: 9:32a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * build uv transforms additions (rotated images)
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 9/17/02    Time: 11:27a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Multi Texturing pass 1 complete
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 8/07/02    Time: 4:23p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 7/22/02    Time: 2:47p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Cloth Working (first time)
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 7/10/02    Time: 1:27p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * cloth code complete (not tested yet)
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 7/09/02    Time: 3:51p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * cloth SOM part 2
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/08/02    Time: 4:00p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * cloth SOM start
 */

#ifndef EXPORT_CLOTH_SOM_H
#define EXPORT_CLOTH_SOM_H

class TupTriMesh;
class BoneGroupTable;
class BoneBuilder;

class ExportClothSOM
{

	const char *m_pDBLFileName;
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;

	TupMaterialPool &m_tupMaterialPool;
	TupMapPool &m_tupMapPool;
	TupImagePool &m_tupImagePool;

	int m_currentSOMIndex;

	// general mesh data
	TupTriMesh *m_pTupTriMesh;
	int m_numFaces;
	int m_numVerts;
	Vector3 *m_pPositionVerts;
	int *m_pPositionIndices;
	int *m_pMaterialIndices,*m_pFaceFlags;

	class UVMap
	{
	public:
		int m_mapChannel;
		int m_uvVerts;
		Vector3 *m_pUVVerts;
		int *m_pUVIndices;
	};

	TupArray<UVMap> m_uvMaps;
	Keeper<int> m_uvChannelToIndexKeeper;

	TupTriMeshConnectivity *m_pTupTriMeshConnectivity; // class that will connect the faces together
	
	// cloth data
	int *m_pClothPointFlags;
	int *m_pClothConnectionPoints;
	int m_numConnections;
	int *m_pFaceClothSOMIndices;
	int *m_pVertClothSOMIndices;

	// bone data
	BoneBuilder *m_pBoneBuilder;

	// material and uv coordinate data
	TupArray<int> m_textureContextIDArray;
	TupArray<TupArray<UVTransform> > m_UVTransformArray;
	TupArray<int> m_materialFlagsArray;

	// data that is being built to do into the DBL file
	SMeshSOM m_meshSOM;
	TupArray<SMeshSOM::SNodeSOM> m_nodeArray;
	TupArray<SMeshSOM::SIndexPair> m_constraintArray;
	TupArray<SMeshSOM::SIndexTrio> m_collisionFaceArray;
	TupArray<SMeshSOM::SIndexPair> m_collisionEdgeArray;
	TupArray<uintCount> m_collisionPointArray;
	TupArray<uintCount> m_neighborArray;
	// tempdata used while building the above data
	TupArray<uintCount> m_positionNodeRemapArray;
	// node used to as the position of the entire mesh
	uintCount m_positionNode;
	// a non attached node that is the closest to the center of all non attached nodes
	uintCount m_dragNode;

	class SubMeshVertex
	{
	public:
		uintCount m_nodeIndex;
		Vector2 m_textureCoordArray[MAX_SOM_TEXTURES_PER_PATCH];
		SubMeshVertex(void);
		bool operator==(const SubMeshVertex& rhs);
	};

	class SubMesh
	{
	public:
		uintOffset m_textureContextIndex;
		uintOffset m_numTextureLayers;
		Keeper<SubMeshVertex> m_vertexKeeper;
		TupArray<uint16> m_triStripArray;
		TupArray<uintCount> m_triStripHeadArray;
	};

	TupArray<SubMesh> m_subMeshArray;

	ExportClothSOM(void); // this is not defined

public:
	ExportClothSOM(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool);

	void WriteMeshes(TupTriMesh &tupMesh,BoneBuilder &boneBuilder);

private:
	void WriteMesh(int SOMIndex);
	void BuildTextureContextIDs(int materialReference);
	void BuildUVTransformArray(int materialReference);
	void BuildFlagsArray(int materialReference);
	void BuildNodes(void);
	void BuildConstraints(void);
	void BuildCollisions(void);
	void BuildNodeNeighbors(void);
	void BuildSubMeshes(void);
	void SetDragAndPositionNodes(void);
};

#endif
