////////////////////////////////////////////////////////////////////////////
//
// ExportSOM
//
// Export Simple Ordinary Meshes into the DBL file
//
////////////////////////////////////////////////////////////////////////////

#ifndef EXPORT_SOM_H
#define EXPORT_SOM_H

#include "DX\XPRBundler.h"

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

	int m_sizeMatrixCache;

	int m_lengthCurrentModelArray;

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
	TupObjectPool &m_tupObjectPool;

	TupArray<int> m_textureContextIDArray;
	TupArray<TupArray<UVTransform> > m_UVTransformArray;
	TupArray<int> m_materialFlagsArray;
	TupArray<int> m_modelOffsetsArray;

	bool m_bOutputBones; // whether we output bones at all

	int m_currentPartition;

	bool m_Xbox;

	XPRBundler	m_XPRBundler;

	ExportSOM(void); // this is not defined

public:
	enum
	{
		MESH_WRITE_DIFFUSE_COLORS			= 0x00000001,
		MESH_WRITE_NORMALS					= 0x00000002,
		MESH_WRITE_EMPTY_TCS					= 0x00000004,

		MESH_NEEDS_DIFFUSE_COLORS			= 0x00000008,
		MESH_NEEDS_TEXTURE_COORDINATES	= 0x00000010
	} ;

	ExportSOM(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,TupObjectPool &tupObjectPool,int currentPartition);
	~ExportSOM(void);
	void StartChunk(int numMeshes,int numBoneMeshes);
	u32 FinishChunk(void);

	void WriteMeshArray( TupTriMesh &tupTriMesh, BoneBuilder &boneBuilder, unsigned int defFVF);

private:
	void StartModel(int lengthModelArray);
	void FinishModel(void);
	void WriteTriStrips( TupTriMeshMap *pTupTriColorMeshMap,TupTriMeshMap *pTupTriAlphaMeshMap, unsigned int defFVF);
	void BuildTextureContextIDs(int materialReference);
	void BuildUVTransformArray(int materialReference);
	void BuildFlagsArray(int materialReference);
	void WriteBoneData(void);
};

#endif
