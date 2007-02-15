////////////////////////////////////////////////////////////////////////////
//
// ExportDMASOM
//
// Export Simple Ordinary Meshes into the DBL file (for ps2)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportDMASOM.h $
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:34a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 11:48a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * update for streaming system
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 9/17/02    Time: 11:27a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * multi-texturing pass 1 complete
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 2:17p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * use passed bone builder
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 6/03/02    Time: 4:29p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * First pass of bones for ps2
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/07/02    Time: 2:39p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * Has alpha flag for texture contexts set
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 5/06/02    Time: 2:56p
 * Created in $/Avalanche/tools/TupperFilters/ExportPS2
 */

#ifndef EXPORT_DMA_SOM_H
#define EXPORT_DMA_SOM_H

class TupTriMesh;
class BoneGroupTable;
class BoneBuilder;
class TupNodePool;
class TupObjectPool;
class TupMaterialPool;
class TupMapPool;
class TupImagePool;

class ExportDMASOM
{
	const char *m_pDBLFileName;
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;
	TupNodePool &m_tupNodePool;
	TupObjectPool &m_tupObjectPool;
	TupMaterialPool &m_tupMaterialPool;
	TupMapPool &m_tupMapPool;
	TupImagePool &m_tupImagePool;

	BoneBuilder *m_pBoneBuilder;
	bool m_bOutputBones;

	int m_currentPartition;

	ExportDMASOM(void); // this is not defined

public:
	ExportDMASOM(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,
		TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,int currentPartition);
	void StartChunk(int numMeshes,int totalBoneMeshes);
	u32 FinishChunk(void);

	void WriteMesh(TupTriMesh &tupTriMesh,BoneBuilder &boneBuilder);
};

#endif
