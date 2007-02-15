////////////////////////////////////////////////////////////////////////////
//
// ExportSOMTextureIDs
//
// Export Texture IDs used by the meshes
//
////////////////////////////////////////////////////////////////////////////

#ifndef EXPORT_SOM_TEXTURE_IDS_H
#define EXPORT_SOM_TEXTURE_IDS_H

class TupScene;
class TupNodePool;

class ExportSOMTextureIDs
{

	const char *m_pDBLFileName;
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;

	TupTriMesh *m_pTupTriMesh;
	TupImagePool &m_tupImagePool;

	int m_currentPartition;

	ExportSOMTextureIDs(void); // this is not defined

public:
	ExportSOMTextureIDs(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupImagePool &tupImagePool,int currentPartition);
	void StartChunk(int numMeshes);
	u32 FinishChunk(void);

	void WriteTextureIDs(TupTriMesh &tupMesh);
};

#endif
