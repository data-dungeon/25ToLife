////////////////////////////////////////////////////////////////////////////
//
// ExportSOM
//
// Export Simple Ordinary Meshes into the DBL file
//
////////////////////////////////////////////////////////////////////////////

#ifndef EXPORT_SOM_H
#define EXPORT_SOM_H

class TupTriMesh;
class BoneGroupTable;
class BoneBuilder;

class ExportSOM
{
	class UVMap
	{
	public:
		int m_mapChannel;
		int m_uvVerts;
		Vector3 *m_pUVVerts;
		int *m_pUVIndices;
	};

	int m_totalTriStripCount;
	int m_totalTriangleCount;
	int m_totalStripDataSize;

	int m_modelHeaderFilePos;
	int m_modelTriStripCount;
	int m_modelTriStripPointCount;
	int m_modelTriStripFaceCount;
	int m_modelTriStripVertexColorCount;
	int m_modelTriStripFileOffset;
	int m_modelEndStripsFileOffset;
	int m_modelSOMFlags;

	const char *m_pDBLFileName;
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;

	BoneBuilder *m_pBoneBuilder;
	TupTriMesh *m_pTupTriMesh;
	TupMaterialPool &m_tupMaterialPool;
	TupMapPool &m_tupMapPool;
	TupImagePool &m_tupImagePool;

	TupArray<int> m_textureContextIDArray;
	TupArray<TupArray<UVTransform> > m_UVTransformArray;
	TupArray<int> m_materialFlagsArray;

	bool m_bOutputBones; // whether we output bones at all

	int m_currentPartition;

	ExportSOM(void); // this is not defined

public:
	ExportSOM(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,int currentPartition);
	void StartChunk(int numMeshes,int numBoneMeshes);
	u32 FinishChunk(void);

	void WriteMesh(TupTriMesh &tupMesh,BoneBuilder &boneBuilder);

private:
	void StartModel(void);
	void FinishModel(void);
	void WriteTriStrips(void);
	void BuildTextureContextIDs(int materialReference);
	void BuildUVTransformArray(int materialReference);
	void BuildFlagsArray(int materialReference);
	void WriteBoneData(void);
};

#endif
