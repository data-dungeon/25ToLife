////////////////////////////////////////////////////////////////////////////
//
// ExportDBL
//
// Export the DBL file
//
////////////////////////////////////////////////////////////////////////////

#ifndef EXPORT_DBL_H
#define EXPORT_DBL_H

class FilterOptions;
class FilterDataManager;
class ExportInstance;
class InstanceHolder;
class TupScene;
class TupObjectPool;
class TupperwareAggregate;
class TupConfigRoot;
class BoneGroupTable;
class ExportVisibilityRegion;
class ExportInstanceAttributes;
class ExportSOMTextureIDs;

class ExportDBL
{
public:
	class PartitionStats
	{
	public:
		int m_meshSize;
		int m_meshTextureIDsSize;
		int m_textureContextSize;
		int m_textureSize;
		int m_instanceSize;
		int m_instanceAttributesSize;
		int m_sceneTreeSize;
		int m_totalSize;
		int m_odbSize;
		int m_cdbSize;
		int m_pdbSize;

		PartitionStats(void) : 
		m_meshSize(0),
		m_meshTextureIDsSize(0),
		m_textureContextSize(0),
		m_textureSize(0),
		m_instanceSize(0),
		m_instanceAttributesSize(0),
		m_sceneTreeSize(0),
		m_totalSize(0),
		m_odbSize(0),
		m_cdbSize(0),
		m_pdbSize(0)
		{
		}
	};

	class WorldStats
	{
	public:
		TupArray<PartitionStats> m_partitionStats;
		int m_visibilityRegionSize;
		int m_surfaceNamesSize;
		int m_pathsSize;
		int m_namedPointsSize;
		int m_attachPointsSize;
		int m_soundSize;
		int m_haloSize;
		int m_stationaryCamSize;
		int m_waterSize;
		int m_worldEmittersSize;
		int m_instanceGroupsSize;
		int m_boneGroupsSize;
		int m_lightsSize;
		int m_cphSize;
		int m_navSize;

		WorldStats(void) :
		m_visibilityRegionSize(0),
		m_surfaceNamesSize(0),
		m_pathsSize(0),
		m_namedPointsSize(0),
		m_attachPointsSize(0),
		m_soundSize(0),
		m_haloSize(0),
		m_stationaryCamSize(0),
		m_waterSize(0),
		m_worldEmittersSize(0),
		m_instanceGroupsSize(0),
		m_boneGroupsSize(0),
		m_lightsSize(0),
		m_cphSize(0),
		m_navSize(0)
		{
		}
	};

private:
	TupString m_fileNameBase;
	TupString m_fileName;

	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;

	TupperwareAggregate *m_pSceneAgg;
	TupScene *m_pTupScene;

	TupperwareAggregate *m_pRootNodeAgg;
	TupNode *m_pTupRootNode;
	int m_rootNodeRef;

	TupperwareAggregate *m_pNodePoolAgg;
	TupNodePool *m_pTupNodePool;
	
	TupperwareAggregate *m_pObjectPoolAgg;
	TupObjectPool *m_pTupObjectPool;

	TupperwareAggregate *m_pModifierPoolAgg;
	TupModifierPool *m_pTupModifierPool;

	TupperwareAggregate *m_pMaterialPoolAgg;
	TupMaterialPool *m_pTupMaterialPool;

	TupperwareAggregate *m_pMapPoolAgg;
	TupMapPool *m_pTupMapPool;

	TupperwareAggregate *m_pImagePoolAgg;
	TupImagePool *m_pTupImagePool;

	TupConfigRoot *m_pTupConfigRoot;
	BoneGroupTable *m_pBoneGroupTable;

	int m_currentPartition; // which partition we are currently exporting 0=world
	WorldStats m_worldStats;
	bool m_bBuildPartitions;
	bool m_bModelsOnly;
	bool m_bBoneGroupTableOnly;
	bool m_bSaveMasterBoneFile;
	bool m_bLoadMasterBoneFile;
	TupString m_masterBoneFilePath;
	bool m_bExportXBoxRDFFile;
	bool m_bExportTextureNames;
	bool m_bExportTextureData;

	ExportDBL(void); // this is not defined

public:
	ExportDBL(const char *pFilename,const FilterOptions &options,FilterDataManager &dataManager);
	~ExportDBL();
	void DoExport(void);

private:
	void ExportScene(void);
	void ExportMeshes(void);
	void ExportMeshTextureIDs(void);
	void ExportTextureContexts(void);
	void ExportTextures(void);
	void ExportNodes(void);
	void ExportSceneDataTree(void);
	void ExportVisibilityRegions(void);
	void ExportSurfaceNames(void);
	void ExportPaths(void);
	void ExportNamedPoints(void);
	void ExportAttachPoints(void);
	void ExportHalos(void);
	void ExportStationaryCams(void);
	void ExportSounds(void);
	void ExportWaters(void);
	void ExportWorldEmitters(void);
	void ExportInstanceGroups(void);
	void ExportInstanceAttribs(void);
	void GetBoneGroupTable(void);
	void ExportBoneGroups(void);
	void MergeODBFile(void);
	void MergeCDBFile(void);
	void MergePDBFile(void);
	void MergeCPHFile(void);
	void MergeNAVFile(void);
	void ExportVarFile(void);
	void BuildNodeAttributeTree(void);
	void CheckAbort(void);
	void ExportLightingSets(void);
	u32 MergeExternalFile(const char *pFileName,int chunkType,int chunkVersion,bool bDeleteFile);
	void AddWorldStatsToLog(void);
	void AddSortedPartitionStatsToLog(void);

};

#endif