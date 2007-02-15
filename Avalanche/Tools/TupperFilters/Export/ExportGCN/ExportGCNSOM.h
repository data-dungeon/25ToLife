////////////////////////////////////////////////////////////////////////////
//
// ExportGCNSOM
//
// Export Simple Ordinary Meshes into the DBL file (GAMECUBE)
//
////////////////////////////////////////////////////////////////////////////

#ifndef EXPORT_GCN_SOM_H
#define EXPORT_GCN_SOM_H

class TupTriMesh;
class BoneBuilder;
class BoneGroupTable;

class ExportGCNSOM
{
	class RGBAColor
	{
	public:
		u8				R;
		u8				G;
		u8				B;
		u8				A;
		
		bool operator==(const RGBAColor &s) { return (R==s.R && G==s.G && B==s.B && A==s.A); }
	};

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
	int m_modelPositionFileOffset;
	int m_modelNormalFileOffset;
	int m_modelUVFileOffset;
	int m_modelPositionCount;
	int m_modelNormalCount;
	int m_modelUVCount;

	int m_modelSOMFlags;

	const char *m_pDBLFileName;
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;

	TupObjectPool &m_tupObjectPool;
	TupMaterialPool &m_tupMaterialPool;
	TupMapPool &m_tupMapPool;
	TupImagePool &m_tupImagePool;

	// material and uv coordinate data
	TupArray<int> m_textureContextIDArray;
	TupArray<TupArray<UVTransform> > m_UVTransformArray;
	TupArray<int> m_materialFlagsArray;
	TupArray<ts_FullTextureContext> m_textureContextArray;

	BoneBuilder *m_pBoneBuilder;
	TupTriMesh *m_pTupTriMesh;

	bool m_bOutputBones; // whether we output bones at all

	int m_currentPartition;

	ExportGCNSOM(void); // this is not defined

public:
	ExportGCNSOM(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupObjectPool &tupObjectPool,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,int currentPartition);
	void StartChunk(int numMeshes,int numBoneMeshes);
	u32 FinishChunk(void);

	void WriteMesh(TupTriMesh &tupMesh,BoneBuilder &boneBuilder);
private:
	void StartModel(void);
	void FinishModel(void);
	void WriteTriStrips(void);
	void BuildTextureContextIDs(int materialReference);
	void BuildUVTransformArray(int materialReference);
	void BuildTextureContexts(int materialReference);
	void BuildFlagsArray(int materialReference);
	void WriteBoneData(void);
	void GetConsolidatedColorIndices(TupArray<int> &indices,int colorRef,int alphaRef,bool b32bitColors);

};

#endif
