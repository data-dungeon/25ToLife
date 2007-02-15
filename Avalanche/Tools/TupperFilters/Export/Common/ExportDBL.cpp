////////////////////////////////////////////////////////////////////////////
//
// ExportDBL
//
// Export the DBL file
//
////////////////////////////////////////////////////////////////////////////

#include "ExportPch.h"
#include "ExportDBL.h"
#if defined(EXPORT_PS2)
#include "..\ExportPS2\ExportPS2SOM.h"
#elif defined(EXPORT_GCN)
#include "..\ExportGCN\ExportGCNSOM.h"
#elif defined(EXPORT_DX)
#include "..\ExportDX\ExportDXSOM.h"
#else
#include "..\ExportPC\ExportPCSOM.h"
#endif
#include "ExportBones.h"
#include "ExportClothSOM.h"
#include "ExportTextureContext.h"
#include "ExportTexture.h"
#include "ExportInstance.h"
#include "ExportSceneTree.h"
#include "ExportSurfaceTypes.h"
#include "ExportPath.h"
#include "ExportNamedPoint.h"
#include "ExportAttachPoint.h"
#include "ExportSound.h"
#include "ExportWater.h"
#include "ExportHalo.h"
#include "ExportStationaryCam.h"
#include "ExportWorldEmitter.h"
#include "ExportInstanceGroup.h"
#include "BoneGroupTable.h"
#include "ExportBoneGroupTable.h"
#include "ExportVisibilityRegion.h"
#include "ExportLighting.h"
#include "ExportInstanceAttributes.h"
#include "ExportSOMTextureIds.h"

ExportDBL::ExportDBL(const char *pFileName,const FilterOptions &options,FilterDataManager &dataManager) :
	m_fileNameBase(pFileName), m_options(options), m_dataManager(dataManager), m_pTupConfigRoot(NULL)
{
	m_pSceneAgg = m_dataManager.GetSceneRoot();
	assert(m_pSceneAgg);
	m_pTupScene = new TupScene(m_pSceneAgg);

	m_pNodePoolAgg = m_pTupScene->GetNodePool();
	assert(m_pNodePoolAgg);
	m_pTupNodePool = new TupNodePool(m_pNodePoolAgg);
	
	m_pObjectPoolAgg = m_pTupScene->GetObjectPool();
	assert(m_pObjectPoolAgg);
	m_pTupObjectPool = new TupObjectPool(m_pObjectPoolAgg);

	m_pModifierPoolAgg = m_pTupScene->GetModifierPool();
	assert(m_pModifierPoolAgg);
	m_pTupModifierPool = new TupModifierPool(m_pModifierPoolAgg);

	m_pMaterialPoolAgg = m_pTupScene->GetMaterialPool();
	assert(m_pMaterialPoolAgg);
	m_pTupMaterialPool = new TupMaterialPool(m_pMaterialPoolAgg);

	m_pMapPoolAgg = m_pTupScene->GetMapPool();
	assert(m_pMapPoolAgg);
	m_pTupMapPool = new TupMapPool(m_pMapPoolAgg);

	m_pImagePoolAgg = m_pTupScene->GetImagePool();
	assert(m_pImagePoolAgg);
	m_pTupImagePool = new TupImagePool(m_pImagePoolAgg);

	m_pTupScene->GetRootNodeReference(m_rootNodeRef);
	assert(m_rootNodeRef!=-1);
	m_pRootNodeAgg = m_pTupNodePool->GetNode(m_rootNodeRef);
	assert(m_pRootNodeAgg);
	m_pTupRootNode = new TupNode(m_pRootNodeAgg);

	m_bBuildPartitions = false;
	const char *pValue;
	pValue = m_options.GetValue("BuildPartitions" );
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bBuildPartitions = true;
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Building Partitions");
	}

	m_bModelsOnly = false;
	pValue = m_options.GetValue("ModelsOnly" );
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bModelsOnly = true;
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Exporting Models Only");
	}

	m_bBoneGroupTableOnly = false;
	pValue = m_options.GetValue("BoneGroupTableOnly" );
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bBoneGroupTableOnly = true;
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Exporting Bone Group Table Only");
	}

	m_bSaveMasterBoneFile = false;
	pValue = m_options.GetValue("SaveMasterBoneFile");
	if (pValue)
	{
		m_bSaveMasterBoneFile = true;
		m_masterBoneFilePath = pValue;
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Going to save master bone file %s",pValue);
	}

	m_bLoadMasterBoneFile = false;
	pValue = m_options.GetValue("LoadMasterBoneFile");
	if (pValue)
	{
		m_bLoadMasterBoneFile = true;
		m_masterBoneFilePath = pValue;
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Going to load master bone file %s",pValue);
	}

	m_bExportXBoxRDFFile = false;
	pValue = m_options.GetValue("ExportXBoxRDFFile");
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bExportXBoxRDFFile = true;
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Going to save an XBoxRDF file");
	}

	m_bExportTextureNames = false;
	pValue = m_options.GetValue("ExportTextureNames");
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bExportTextureNames = true;
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Going to export texture names in .dbl file");
	}

	m_bExportTextureData = true;
	pValue = m_options.GetValue("ExportTextureData");
	if (pValue && !stricmp(pValue,"false"))
	{
		m_bExportTextureData = false;
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Not going to export texture data in .dbl file");
	}

	BuildNodeAttributeTree();
	GetBoneGroupTable();
}

ExportDBL::~ExportDBL()
{
	delete m_pTupScene;
	delete m_pTupNodePool;
	delete m_pTupObjectPool;
	delete m_pTupModifierPool;
	delete m_pTupMaterialPool;
	delete m_pTupMapPool;
	delete m_pTupImagePool;
	delete m_pTupRootNode;

	delete m_pTupConfigRoot;
	delete m_pBoneGroupTable;
}


void ExportDBL::DoExport(void)
{
	FilePath::ChangeExtension(m_fileNameBase,".DBL");

	int numVisiblitySets;
	// delete all .dbl files that share the same part
	if (m_bBuildPartitions)
	{
		m_fileName = m_fileNameBase;
		FilePath::LimitCharactersInFilename(m_fileName,5);
		FilePath::AddStringToFileName(m_fileName,"*");
		FilePath::DeleteFiles(m_fileName);

		numVisiblitySets = m_pSceneAgg->FindListByKey("Annotate_VisibilitySetNodeRefs")->GetLength();

		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting %d file(s): %s...",numVisiblitySets, (const char *) m_fileName);
	}
	// if we are not doing the partition thing then just do 1 set
	else
	{
		numVisiblitySets=1;
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting file %s",m_fileNameBase);
	}

	m_worldStats.m_partitionStats.SetSize(numVisiblitySets);

	for (m_currentPartition=0;m_currentPartition<numVisiblitySets;m_currentPartition++)
	{
		m_fileName = m_fileNameBase;

		FilePath::AddNumberToFileName(m_fileName,m_currentPartition); // don't do if 0

		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting file: %s...",(const char *) m_fileName);
		/* start DBL database file */
#if defined(EXPORT_PS2)
		DBLUtil::StartDBLFile(m_fileName,NULL,false,"PS2",false);
#elif defined(EXPORT_GCN)
		DBLUtil::StartDBLFile(m_fileName,NULL,false,"GCN",true);
#elif defined(EXPORT_PC)
		DBLUtil::StartDBLFile(m_fileName,NULL,false,"GL",false);
#elif defined(EXPORT_DX)
		DBLUtil::StartDBLFile(m_fileName,NULL,false,"DX",false);
#elif defined(EXPORT_XBOX)
		DBLUtil::StartDBLFile(m_fileName,NULL,false,"XBOX",false);
#else
		throw(PluginException(ERROR_FATAL,"ExportDBL::DoExport - Unknown Export Type"));
#endif
		
		ExportScene();
		
		m_worldStats.m_partitionStats[m_currentPartition].m_totalSize = DBLUtil::FinishDBLFile();
		DBLUtil::CleanUpDBLFile();
		
#ifdef __DEBUG_DBL_ENABLE
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"DBL Debug Info...");
		int numEntries = DBLDebug::GetNumEntries();
		for (int entryIndex=0;entryIndex<numEntries;entryIndex++)
		{
			int min = DBLDebug::GetFileWriteMin(entryIndex);
			int max = DBLDebug::GetFileWriteMax(entryIndex);
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Entry %d, Name %s, Index %d, Min 0x%08x, Max 0x%08x NumBytes 0x%08x",
				entryIndex,DBLDebug::GetName(entryIndex),DBLDebug::GetIndex(entryIndex),min,max-1,max-min);
			for (int otherEntryIndex=0;otherEntryIndex<entryIndex;otherEntryIndex++)
			{
				int omin = DBLDebug::GetFileWriteMin(otherEntryIndex);
				int omax = DBLDebug::GetFileWriteMax(otherEntryIndex);
				if (omin<max && omax>min)
				{
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Entry Conflict With %d, Name %s, Index %d, Min 0x%08x, Max 0x%08x NumBytes 0x%08x",
						otherEntryIndex,DBLDebug::GetName(otherEntryIndex),DBLDebug::GetIndex(otherEntryIndex),omin,omax-1,omax-omin);
				}
			}
		}
#endif
	}
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Complete");
	AddWorldStatsToLog();
	if (m_bBuildPartitions)
	{
		AddSortedPartitionStatsToLog();
	}
	ExportVarFile();
}

void ExportDBL::ExportScene(void)
{
	if (m_bBoneGroupTableOnly)
	{
		CheckAbort();
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Bone Group Table");
		ExportBoneGroups();
	}
	else if (m_bModelsOnly)
	{
		CheckAbort();
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Textures");
		ExportTextures();
		
		CheckAbort();
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Texture Contexts");
		ExportTextureContexts();

		CheckAbort();
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Meshes");
		ExportMeshes();

		CheckAbort();
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Mesh Texture Ids");
		ExportMeshTextureIDs();

		CheckAbort();
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Attach Points");
		ExportAttachPoints();
	}
	else // scene
	{
		CheckAbort();
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Textures");
		ExportTextures();
			
		CheckAbort();
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Texture Contexts");
		ExportTextureContexts();
			
		CheckAbort();
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Meshes");
		ExportMeshes();

		CheckAbort();
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Mesh Texture Ids");
		ExportMeshTextureIDs();

		CheckAbort();
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Instances");
		ExportNodes();

		if (m_currentPartition == 0)
		{
			CheckAbort();
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Paths");
			ExportPaths();

			CheckAbort();
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Named Points");
			ExportNamedPoints();

			CheckAbort();
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Attach Points");
			ExportAttachPoints();

			CheckAbort();
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Sound Sources/Fields");
			ExportSounds();

			CheckAbort();
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Halos");
			ExportHalos();

			CheckAbort();
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Stationary Cameras");
			ExportStationaryCams();

			CheckAbort();
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Water");
			ExportWaters();
	
			CheckAbort();
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting World Emitters");
			ExportWorldEmitters();
		}

		CheckAbort();
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Scene Tree");
		ExportSceneDataTree();

		if (m_currentPartition == 0)
		{
			CheckAbort();
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Visibility Regions");
			ExportVisibilityRegions();

			CheckAbort();
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Surface Types");
			ExportSurfaceNames();

			CheckAbort();
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Instance Groups");
			ExportInstanceGroups();

			CheckAbort();
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Lighting Sets");
			ExportLightingSets();
		}

		CheckAbort();
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting Instance Attributes");
		ExportInstanceAttribs();

		CheckAbort();
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Merging Object Bounding Box Database");
		MergeODBFile();
		
		CheckAbort();
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Merging Collision Database");
		MergeCDBFile();
		
		CheckAbort();
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Merging Perimeter Database");
		MergePDBFile();
		
		if (m_currentPartition == 0)
		{
			CheckAbort();
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Merging Partition Database");
			MergeCPHFile();

			CheckAbort();
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Merging Navigation Database");
			MergeNAVFile();
		}
	}

	CheckAbort();
}

void ExportDBL::ExportMeshes(void)
{
#if defined(EXPORT_PS2)
	ExportDMASOM exportSOM(m_fileName,m_options,m_dataManager,*m_pTupNodePool,*m_pTupObjectPool,*m_pTupMaterialPool,*m_pTupMapPool,*m_pTupImagePool,m_currentPartition);
#elif defined(EXPORT_GCN)
	ExportGCNSOM exportSOM(m_fileName,m_options,m_dataManager,*m_pTupObjectPool,*m_pTupMaterialPool,*m_pTupMapPool,*m_pTupImagePool,m_currentPartition);
#elif defined(EXPORT_DX)
	ExportSOM exportSOM(m_fileName,m_options,m_dataManager,*m_pTupMaterialPool,*m_pTupMapPool,*m_pTupImagePool,*m_pTupObjectPool,m_currentPartition);
#else 
	ExportSOM exportSOM(m_fileName,m_options,m_dataManager,*m_pTupMaterialPool,*m_pTupMapPool,*m_pTupImagePool,m_currentPartition);
#endif

	ExportClothSOM exportClothSOM(m_fileName,m_options,m_dataManager,*m_pTupMaterialPool,*m_pTupMapPool,*m_pTupImagePool);


	// get the total number of meshes that will bet saved to the DBL file
	// this is due to a mesh having the potetial to use multiple materials
	try
	{
		// this has the totals for all the soms in all the partitions
		TupArray<int> totalSOMsArray;
		m_pTupScene->GetBaseAggregate()->FindListByKey("Annotate_TotalSOMS")->GetAsInt(totalSOMsArray);
		int totalSOMs = totalSOMsArray[m_currentPartition]; // total in our partition

		if (totalSOMs==0)
			return; // don't do if there is no SOMS in this partition

		// the bone meshes only apply to partition 0
		int totalBoneMeshes=0;
		if (m_currentPartition==0)
			totalBoneMeshes = m_pTupScene->GetBaseAggregate()->FindScalarByKey("Annotate_TotalBoneMeshes")->GetAsInt();

		FILE *pDBLFile = DBLUtil::GetDBLFile();

		exportSOM.StartChunk(totalSOMs,totalBoneMeshes);

		int totalObjects = m_pTupObjectPool->GetNumObjects();
		int SOMIndex = 0;

		for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
		{
			TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

			// if slot empty then continue
			if (!pObjectAgg)
				continue;

			// is this mesh a SOM?
			TupperwareList *pRenderPartitionSOMStartsList = pObjectAgg->FindListByKey("Annotate_RenderPartitionSOMStarts");
			if (!pRenderPartitionSOMStartsList)
				continue;
			TupArray<int> renderPartitionSOMStarts;
			pRenderPartitionSOMStartsList->GetAsInt(renderPartitionSOMStarts);
			if (renderPartitionSOMStarts[m_currentPartition]==-1) // if not used in this partition
				continue;

			long somStartPos = ftell(pDBLFile);

			assert(renderPartitionSOMStarts[m_currentPartition]==SOMIndex);
			SOMIndex++;

			bool bSaveBaseMesh = true;
			bool bSaveClothMeshes = false;

			TupTriMesh tupTriMesh(pObjectAgg);
			TupperwareAggregate *pBoneMeshAgg = pObjectAgg->FindAggregateByKey("Annotate_BoneMeshData");
			TupperwareAggregate *pClothMeshAgg = pObjectAgg->FindAggregateByKey("Annotate_ClothMeshData");
			if (pClothMeshAgg)
			{
				int numClothSOMs = pClothMeshAgg->FindScalarByKey("NumClothSOMs")->GetAsInt();
				int numNonClothFaces = pClothMeshAgg->FindScalarByKey("NumNonClothFaces")->GetAsInt();
				if (numNonClothFaces==0 && !pBoneMeshAgg)
					bSaveBaseMesh = false;
				if (numClothSOMs)
					bSaveClothMeshes = true;
			}
			if (!bSaveBaseMesh && !bSaveClothMeshes)
			{
				throw(PluginException(ERROR_FATAL,"Object does not have any SOMS specified"));
			}
			BoneBuilder *pBoneBuilder = new BoneBuilder(tupTriMesh,*m_pTupNodePool,*m_pTupObjectPool,*m_pBoneGroupTable,m_rootNodeRef);
			if (pBoneBuilder->GetNumBones())
			{
				if (m_bLoadMasterBoneFile)
				{
					TupString relativePath,fullPath,name,ext;
					FilePath::GetPathsToFile(relativePath,fullPath,name,ext,m_masterBoneFilePath,m_dataManager.GetWorkDirectory());

					TupperwareAggregate *pBoneFileAgg = new TupperwareAggregate;
					int result = TupperwareLoadSaveManager::Read(fullPath,&pBoneFileAgg);
					if (!result)
						throw(PluginException(ERROR_FATAL,"Error reading Master Bone File %s",(const char *)fullPath));
					TupperwareList *pNameList = pBoneFileAgg->FindListByKey("MasterBones");
					if (!pNameList)
						throw(PluginException(ERROR_FATAL,"Error reading Master Bone File %s",(const char *)fullPath));
					TupArray<TupString> masterNameStrings;
					pNameList->GetAsString(masterNameStrings);
					pBoneBuilder->RemapBonesToList(masterNameStrings);
					delete pBoneFileAgg;
				}
				if (m_bSaveMasterBoneFile)
				{
					TupString relativePath,fullPath,name,ext;
					FilePath::GetPathsToFile(relativePath,fullPath,name,ext,m_masterBoneFilePath,m_dataManager.GetWorkDirectory());

					TupArray<TupString> masterNameStrings;
					pBoneBuilder->GetBoneNameList(masterNameStrings);

					TupperwareAggregate *pBoneFileAgg = new TupperwareAggregate;
					pBoneFileAgg->AddListString("MasterBones",NULL,masterNameStrings);

					int result = TupperwareLoadSaveManager::Write(TupperwareLoadSaveManager::ASCII,fullPath,pBoneFileAgg);
					if (!result)
						throw(PluginException(ERROR_FATAL,"Error writing Master Bone File %s",(const char *)fullPath));
					delete pBoneFileAgg;
				}
			}
			if (bSaveBaseMesh)
			{
#ifndef EXPORT_DX
				exportSOM.WriteMesh(tupTriMesh,*pBoneBuilder);
#else
				{
					unsigned int		defMeshOptions = ExportSOM::MESH_WRITE_DIFFUSE_COLORS | ExportSOM::MESH_WRITE_EMPTY_TCS;

					bool bALit = pObjectAgg->FindScalarByKey("RealTimeLighting_Ambient")->GetAsInt() ? true : false;
					bool bPLit = pObjectAgg->FindScalarByKey("RealTimeLighting_Point")->GetAsInt() ? true : false;
					bool bDLit = pObjectAgg->FindScalarByKey("RealTimeLighting_Directional")->GetAsInt() ? true : false;
					bool bVLit = pObjectAgg->FindScalarByKey("RealTimeLighting_Vertex")->GetAsInt() ? true : false;

					if (bALit || bPLit || bDLit)
					{
						defMeshOptions |= ExportSOM::MESH_WRITE_NORMALS;
						if (!bVLit)
						{
							// if real time lit but not wanting vertex colors
							defMeshOptions &= ~ExportSOM::MESH_WRITE_DIFFUSE_COLORS;
						}
					}

					if( pBoneBuilder->GetNumBones( ) )
					{
						defMeshOptions |= ExportSOM::MESH_NEEDS_DIFFUSE_COLORS | ExportSOM::MESH_NEEDS_TEXTURE_COORDINATES | ExportSOM::MESH_WRITE_NORMALS;
					}

					exportSOM.WriteMeshArray( tupTriMesh, *pBoneBuilder, defMeshOptions );				
				}
#endif
			}
			if (bSaveClothMeshes)
			{
				exportClothSOM.WriteMeshes(tupTriMesh,*pBoneBuilder);
			}
			delete pBoneBuilder;
			if (PluginLog::GetAbortFlag()) throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));

			long somEndPos = ftell(pDBLFile);

			pObjectAgg->AddScalarInt("ExportMesh_SomDataSize",NULL,(int) somEndPos-somStartPos);

		}
		m_worldStats.m_partitionStats[m_currentPartition].m_meshSize+=exportSOM.FinishChunk();
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportMeshes\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::ExportMeshTextureIDs(void)
{
	ExportSOMTextureIDs exportSOMTextureIDs(m_fileName,m_options,m_dataManager,*m_pTupImagePool,m_currentPartition);

	// get the total number of meshes that will bet saved to the DBL file
	// this is due to a mesh having the potetial to use multiple materials
	try
	{
		// this has the totals for all the soms in all the partitions
		TupArray<int> totalSOMsArray;
		m_pTupScene->GetBaseAggregate()->FindListByKey("Annotate_TotalSOMS")->GetAsInt(totalSOMsArray);
		int totalSOMs = totalSOMsArray[m_currentPartition]; // total in our partition

		if (totalSOMs==0)
			return; // don't do if there is no SOMS in this partition

		FILE *pDBLFile = DBLUtil::GetDBLFile();

		exportSOMTextureIDs.StartChunk(totalSOMs);

		int totalObjects = m_pTupObjectPool->GetNumObjects();
		int SOMIndex = 0;

		for (int objectIndex=0;objectIndex<totalObjects;objectIndex++)
		{
			TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectIndex);

			// if slot empty then continue
			if (!pObjectAgg)
				continue;

			// is this mesh a SOM?
			TupperwareList *pRenderPartitionSOMStartsList = pObjectAgg->FindListByKey("Annotate_RenderPartitionSOMStarts");
			if (!pRenderPartitionSOMStartsList)
				continue;
			TupArray<int> renderPartitionSOMStarts;
			pRenderPartitionSOMStartsList->GetAsInt(renderPartitionSOMStarts);
			if (renderPartitionSOMStarts[m_currentPartition]==-1) // if not used in this partition
				continue;

			assert(renderPartitionSOMStarts[m_currentPartition]==SOMIndex);
			SOMIndex++;

			TupTriMesh tupTriMesh(pObjectAgg);
			exportSOMTextureIDs.WriteTextureIDs(tupTriMesh);

			if (PluginLog::GetAbortFlag()) throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));
		}
		m_worldStats.m_partitionStats[m_currentPartition].m_meshTextureIDsSize+=exportSOMTextureIDs.FinishChunk();
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportMeshTextureIds\r\n  %s",(const char *)(e)));
	}
}


void ExportDBL::ExportTextureContexts(void)
{
	ExportTextureContext exportTextureContext(m_fileName,m_options,m_dataManager,*m_pTupScene,*m_pTupMaterialPool,*m_pTupMapPool,*m_pTupImagePool,m_currentPartition);
	try
	{
		m_worldStats.m_partitionStats[m_currentPartition].m_textureContextSize+=exportTextureContext.WriteTextureContexts();
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportTextureContexts\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::ExportTextures(void)
{
	ExportTexture exportTexture(m_fileName,m_options,m_dataManager,*m_pTupScene,*m_pTupObjectPool,*m_pTupImagePool,m_currentPartition,m_bExportTextureData);
	try
	{
		m_worldStats.m_partitionStats[m_currentPartition].m_textureSize+=exportTexture.WriteTextures();
		m_worldStats.m_partitionStats[m_currentPartition].m_textureSize+=exportTexture.WriteAnimatedTextures();
		if (m_bExportXBoxRDFFile)
		{
			exportTexture.WriteXBoxRDFFile();
		}
		if (m_bExportTextureNames)
		{
			exportTexture.WriteTextureNames();
		}
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportTextures\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::ExportNodes(void)
{
	ExportInstance exportInstance(m_fileName,m_options,m_dataManager,*m_pTupConfigRoot);
	try
	{
		TupArray<int> totalInstancesArray;
		m_pTupScene->GetBaseAggregate()->FindListByKey("Annotate_TotalRenderInstances")->GetAsInt(totalInstancesArray);

		int totalInstances = totalInstancesArray[m_currentPartition];

		if (totalInstances==0)
			return;

		InstanceHolder *pInstanceHolders = new InstanceHolder[totalInstances];

		int totalNodes = m_pTupNodePool->GetNumNodes();
		for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
		{
			TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);

			// if slot empty then continue
			if (!pNodeAgg)
				continue;

			TupperwareScalar *pRenderPartitionIndexScalar = pNodeAgg->FindScalarByKey("Annotate_RenderPartitionIndex");
			// skip ones that don't have this value
			if (!pRenderPartitionIndexScalar)
				continue;
				
			int renderPartitionIndex = pRenderPartitionIndexScalar->GetAsInt();
			// skip ones that are not in the current partition
			if (renderPartitionIndex!=m_currentPartition)
				continue;

			int instanceIndex = pNodeAgg->FindScalarByKey("Annotate_RenderInstanceIndex")->GetAsInt();
			assert(instanceIndex<totalInstances);
			pInstanceHolders[instanceIndex].SetNodeAgg(pNodeAgg);
		}

		/* start chunk */
		DBLUtil::StartChunk(DB_INSTANCE, DB_INSTANCE_VERSION, DBL_CHUNK_NO_IGNORE, "1000");

		/* start record list-- writes out dummy data, to be replaced later */
		DBLUtil::StartRecordList(totalInstances, RECORD_LIST_SAME_SIZE_RECORDS | RECORD_LIST_QUAD_WORD_ALIGNED, sizeof(CInstance));

		int instanceIndex;
		for (instanceIndex = 0; instanceIndex < totalInstances; instanceIndex++)
		{
			exportInstance.WriteInstance(pInstanceHolders,instanceIndex,*m_pTupNodePool,*m_pTupObjectPool);
		}

		DBLUtil::FinishRecordList();

// don't export vertex colors if PS2
#ifndef EXPORT_PS2
		for (instanceIndex = 0; instanceIndex < totalInstances; instanceIndex++)
		{
#ifdef EXPORT_GCN
			exportInstance.WriteGCNInstanceVertexColors(pInstanceHolders,instanceIndex,*m_pTupNodePool,*m_pTupObjectPool);
#else
			exportInstance.WriteInstanceVertexColors(pInstanceHolders,instanceIndex,*m_pTupNodePool,*m_pTupObjectPool);
#endif // EXPORT_GCN
		}
#endif // EXPORT_PS2

		for (instanceIndex = 0; instanceIndex < totalInstances; instanceIndex++)
		{
			exportInstance.LinkInstancePasses(pInstanceHolders,instanceIndex,*m_pTupNodePool,*m_pTupObjectPool);
		}

		// done with instance holders
		delete [] pInstanceHolders;

		m_worldStats.m_partitionStats[m_currentPartition].m_instanceSize+=DBLUtil::FinishChunk(FALSE);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportNodes\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::ExportSceneDataTree(void)
{
	ExportSceneTree exportSceneTree(m_fileName,m_options,m_dataManager,*m_pTupConfigRoot,m_currentPartition);
	try
	{
		if (m_currentPartition == 0)
			m_worldStats.m_partitionStats[m_currentPartition].m_sceneTreeSize+=exportSceneTree.WriteMainSceneTree(*m_pTupScene,*m_pTupNodePool);
		else
			m_worldStats.m_partitionStats[m_currentPartition].m_sceneTreeSize+=exportSceneTree.WritePartitionSceneTree(*m_pTupScene,*m_pTupNodePool);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportSceneDataTree\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::ExportVisibilityRegions(void)
{
	ExportVisibilityRegion exportVisibilityRegion(m_fileName,m_options,m_dataManager);
	try
	{
		m_worldStats.m_visibilityRegionSize+=exportVisibilityRegion.WriteVisibilityRegions(*m_pTupScene,*m_pTupNodePool,m_bBuildPartitions);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportVisibilityRegion\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::ExportSurfaceNames(void)
{
	ExportSurfaceTypes exportSurfaceTypes(m_fileName,m_options,m_dataManager);
	try
	{
		m_worldStats.m_surfaceNamesSize+=exportSurfaceTypes.WriteSurfaceTypes(*m_pTupScene);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportSurfaceTypes\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::ExportPaths(void)
{
	ExportPath exportPath(m_fileName,m_options,m_dataManager);
	try
	{
		m_worldStats.m_pathsSize+=exportPath.WritePaths(*m_pTupScene,*m_pTupNodePool,*m_pTupObjectPool);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportPaths\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::ExportNamedPoints(void)
{
	ExportNamedPoint exportNamedPoint(m_fileName,m_options,m_dataManager,*m_pTupConfigRoot);
	try
	{
		m_worldStats.m_namedPointsSize+=exportNamedPoint.WriteNamedPoints(*m_pTupNodePool);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportNamedPoints\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::ExportAttachPoints(void)
{
	ExportAttachPoint exportAttachPoint(m_fileName,m_options,m_dataManager,*m_pTupConfigRoot);
	try
	{
		m_worldStats.m_attachPointsSize+=exportAttachPoint.WriteAttachPoints(*m_pTupNodePool);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportAttachPoints\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::ExportSounds(void)
{
	ExportSound exportSound(m_fileName,m_options,m_dataManager,*m_pTupConfigRoot);
	try
	{
		m_worldStats.m_soundSize+=exportSound.WriteSources(*m_pTupNodePool);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportSources\r\n  %s",(const char *)(e)));
	}

	try
	{
		m_worldStats.m_soundSize+=exportSound.WriteFields(*m_pTupNodePool);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportSounds\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::ExportHalos(void)
{
	ExportHalo exportHalo(m_fileName,m_options,m_dataManager,*m_pTupConfigRoot);
	try
	{
		m_worldStats.m_haloSize+=exportHalo.WriteHalos(*m_pTupNodePool);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportHalos\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::ExportStationaryCams(void)
{
	ExportStationaryCam exportStationaryCam(m_fileName,m_options,m_dataManager,*m_pTupConfigRoot);
	try
	{
		m_worldStats.m_stationaryCamSize+=exportStationaryCam.WriteStationaryCam(*m_pTupNodePool);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportStationaryCam\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::ExportWaters(void)
{
	ExportWater exportWater(m_fileName,m_options,m_dataManager,*m_pTupConfigRoot);
	try
	{
		m_worldStats.m_waterSize+=exportWater.WriteWater(*m_pTupNodePool);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportWaters\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::ExportWorldEmitters(void)
{
	ExportWorldEmitter exportWorldEmitter(m_fileName,m_options,m_dataManager,*m_pTupConfigRoot);
	try
	{
		m_worldStats.m_worldEmittersSize+=exportWorldEmitter.WriteWorldEmmitters(*m_pTupNodePool);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportWorldEmitters\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::ExportInstanceGroups(void)
{
	ExportInstanceGroup exportInstanceGroup(m_fileName,m_options,m_dataManager,*m_pTupConfigRoot);
	try
	{
		m_worldStats.m_instanceGroupsSize+=exportInstanceGroup.WriteInstanceGroups(*m_pTupNodePool);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportInstanceGroups\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::GetBoneGroupTable(void)
{
	TupString relativePath,fullPath,name,ext;
	TupString filename = "BoneGroupTable.tup"; // default filename
	const char *pValue = m_options.GetValue("BoneGroupTableFile");
	if (pValue)
	{
		filename = pValue;
	}

	FilePath::GetPathsToFile(relativePath,fullPath,name,ext,filename,m_dataManager.GetConfigDirectory());

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Loading Bone Group Table %s",(const char *)fullPath);

	if (_access(fullPath,0)==-1)
		throw(PluginException(ERROR_FATAL,"Bone Group Table file not found %s",(const char *)fullPath));
	if (_access(fullPath,4)==-1)
		throw(PluginException(ERROR_FATAL,"Cannot access Bone Group Table file %s",(const char *)fullPath));

	TupperwareAggregate *pBoneGroupTableAgg;
	int result = TupperwareLoadSaveManager::Read(fullPath,&pBoneGroupTableAgg);
	if (!result)
		throw(PluginException(ERROR_FATAL,"Error reading Bone Group Table file %s",(const char *)fullPath));
	m_pBoneGroupTable = new BoneGroupTable(pBoneGroupTableAgg);
	delete pBoneGroupTableAgg;
	// if there were any errors reading the bone group table do a throw here instead of the constructor
	if (m_pBoneGroupTable->GetNumErrors())
		throw(PluginException(ERROR_FATAL,"%d Errors reading Bone Group Table file %s",m_pBoneGroupTable->GetNumErrors(),(const char *)fullPath));
}

void ExportDBL::BuildNodeAttributeTree(void)
{
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Building Node Attribute Tree");
	m_pTupConfigRoot = new TupConfigRoot;
	// Node Attribute Tree
	TupperwareAggregate *pSceneNodeAttributeTreeAgg = m_pTupScene->GetNodeAttributeTree();
	if (!pSceneNodeAttributeTreeAgg)
	{
		throw(PluginException(ERROR_FATAL,"Node Attribute Tree Does not Exist!"));
	}
	m_pTupConfigRoot->BuildConfigTree(pSceneNodeAttributeTreeAgg);
}


void ExportDBL::ExportBoneGroups(void)
{
	ExportBoneGroupTable exportBoneGroupTable(m_fileName,m_options,m_dataManager);
	try
	{
		m_worldStats.m_boneGroupsSize+=exportBoneGroupTable.WriteBoneGroupTable(*m_pBoneGroupTable);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportBoneGroupTable\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::CheckAbort(void)
{
	if (PluginLog::GetAbortFlag()) throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));
}

void ExportDBL::MergeODBFile(void)
{
	try
	{
		TupString ODBFileName = m_fileName;
		FilePath::ChangeExtension(ODBFileName,".ODB");
		m_worldStats.m_partitionStats[m_currentPartition].m_odbSize+=MergeExternalFile(ODBFileName,DB_ODB,DB_ODB_VERSION,true);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"MergeODBFile\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::MergeCDBFile(void)
{
	try
	{
		TupString CDBFileName = m_fileName;
		FilePath::ChangeExtension(CDBFileName,".CDB");
		m_worldStats.m_partitionStats[m_currentPartition].m_cdbSize+=MergeExternalFile(CDBFileName,DB_CDB,DB_CDB_VERSION,true);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"MergeCDBFile\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::MergePDBFile(void)
{
	try
	{
		TupString PDBFileName = m_fileName;
		FilePath::ChangeExtension(PDBFileName,".PDB");
		m_worldStats.m_partitionStats[m_currentPartition].m_pdbSize+=MergeExternalFile(PDBFileName,DB_PDB,DB_PDB_VERSION,true);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"MergePDBFile\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::MergeCPHFile(void)
{
	try
	{
		TupString CPHFileName = m_fileName;
		FilePath::ChangeExtension(CPHFileName,".CPH");
		m_worldStats.m_cphSize+=MergeExternalFile(CPHFileName,DB_CPH,DB_CPH_VERSION,true);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"MergeCPHFile\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::MergeNAVFile(void)
{
	try
	{
		// add all the .NAV files
		char drive[ _MAX_DRIVE ];
		char dir[ _MAX_DIR ];
		char fname[ _MAX_FNAME ];
		char ext[ _MAX_EXT ];
		char fileName[ _MAX_PATH ];

		_splitpath( m_fileName, drive, dir, fname, ext );
		_makepath( fileName, drive, dir, "*", ".NAV" );

		// loop through all *.nav files
		WIN32_FIND_DATA findData;
		HANDLE hFind = ::FindFirstFile( fileName, &findData );
		if ( hFind != INVALID_HANDLE_VALUE )
		{
			do
			{
				if ( ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
				{	// avoid adding directories
					_splitpath( findData.cFileName, NULL, NULL, fname, ext );
					_makepath( fileName, drive, dir, fname, ext );
					m_worldStats.m_navSize += MergeExternalFile( fileName, DB_NAV, DB_NAV_VERSION, true );
				}
			} while ( ::FindNextFile( hFind, &findData ) );
		}

		::FindClose( hFind );
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"MergeNAVFile\r\n %s",(const char *)(e)));
	}
}

u32 ExportDBL::MergeExternalFile(const char *pFileName,int chunkType,int chunkVersion,bool bDeleteFile)
{
	FILE *pFile = fopen( pFileName,"rb");
	
	if (pFile == NULL) 
	{
//		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"%s does not exist.  Cannot merge into this partition.",pFileName);
		return 0;
	}
	
	// get length of file
	fseek(pFile, 0, SEEK_END);
	int fileLength = ftell(pFile); 
	fseek(pFile, 0, SEEK_SET);
	
	if (fileLength==0)
	{
		fclose( pFile );
//		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"%s has no length.  Cannot merge into this partition.",pFileName);
		return 0;
	}
	
	FILE *pDBLFile = DBLUtil::GetDBLFile();
	
	DBLUtil::StartChunk(chunkType, chunkVersion, DBL_CHUNK_NO_IGNORE, "1000");
	
	// allocate a buffer to hold the entire file
	char *pBuffer = new char[fileLength];
	// read it all in
	int bytesRead = fread(pBuffer,1,fileLength,pFile);
	
	DBLWrite::WriteData(pBuffer,fileLength,pDBLFile);
	DBLUtil::AlignFile();
	
	fclose( pFile );

	delete pBuffer;

#ifndef _DEBUG
	if (bDeleteFile)
	{
		if (!::DeleteFile(pFileName))
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Error deleting file %s.",pFileName);
	}
#endif
	return DBLUtil::FinishChunk(FALSE);
}

void ExportDBL::AddWorldStatsToLog(void)
{
	int numVisiblitySets = m_worldStats.m_partitionStats.GetSize();

	TupArray<int> visibilitySetNodeRefs;
	m_pSceneAgg->FindListByKey("Annotate_VisibilitySetNodeRefs")->GetAsInt(visibilitySetNodeRefs);

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Export Stats for %d Partitions:",numVisiblitySets);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Export Stats Main Partition:");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"        Chunk          Size");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"   Visiblity Region  %8d",m_worldStats.m_visibilityRegionSize);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"   Surface Names     %8d",m_worldStats.m_surfaceNamesSize);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"   Paths             %8d",m_worldStats.m_pathsSize);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"   Named Points      %8d",m_worldStats.m_namedPointsSize);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"   Attach Points     %8d",m_worldStats.m_attachPointsSize);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"   Sound Sources     %8d",m_worldStats.m_soundSize);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"   Halos             %8d",m_worldStats.m_haloSize);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"   Stationary Cams   %8d",m_worldStats.m_stationaryCamSize);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"   World Emitters    %8d",m_worldStats.m_worldEmittersSize);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"   Instance Groups   %8d",m_worldStats.m_instanceGroupsSize);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"   Bone Groups       %8d",m_worldStats.m_boneGroupsSize);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"   Lights            %8d",m_worldStats.m_lightsSize);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"   CPH               %8d",m_worldStats.m_cphSize);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"   Water             %8d",m_worldStats.m_waterSize);

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Export Stats Partitions");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"               Mesh     Texture                                 Scene                                  Vis Region");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW," #     Mesh  TextureIDs Context  Textures Instances  InstAttrib Tree      ODB     CDB    PDB     Total   Name");

	int meshTotalSize = 0;
	int meshTextureIDsTotalSize = 0;
	int textureContextTotalSize = 0;
	int textureTotalSize = 0;
	int instanceTotalSize = 0;
	int instanceAttributesTotalSize = 0;
	int sceneTreeTotalSize = 0;
	int odbTotalSize = 0;
	int cdbTotalSize = 0;
	int pdbTotalSize = 0;
	int totalTotalSize = 0;

	for (int setIndex=0;setIndex<numVisiblitySets;setIndex++)
	{
		int nodeRef = visibilitySetNodeRefs[setIndex];
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeRef);

		TupNode tupNode(pNodeAgg);
		char *pName;
		tupNode.GetName(&pName);

		meshTotalSize += m_worldStats.m_partitionStats[setIndex].m_meshSize;
		meshTextureIDsTotalSize += m_worldStats.m_partitionStats[setIndex].m_meshTextureIDsSize;
		textureContextTotalSize += m_worldStats.m_partitionStats[setIndex].m_textureContextSize;
		textureTotalSize += m_worldStats.m_partitionStats[setIndex].m_textureSize;
		instanceTotalSize += m_worldStats.m_partitionStats[setIndex].m_instanceSize;
		instanceAttributesTotalSize += m_worldStats.m_partitionStats[setIndex].m_instanceAttributesSize;
		sceneTreeTotalSize += m_worldStats.m_partitionStats[setIndex].m_sceneTreeSize;
		odbTotalSize += m_worldStats.m_partitionStats[setIndex].m_odbSize;
		cdbTotalSize += m_worldStats.m_partitionStats[setIndex].m_cdbSize;
		pdbTotalSize += m_worldStats.m_partitionStats[setIndex].m_pdbSize;
		totalTotalSize += m_worldStats.m_partitionStats[setIndex].m_totalSize;
		//                                            #   m  mtid    tc    t   i	    ia  st   o   c   p   t  n
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"%03x %8d   %6d   %6d   %8d %8d     %7d %7d  %7d %7d %6d  %8d %s",
			setIndex,
			m_worldStats.m_partitionStats[setIndex].m_meshSize,
			m_worldStats.m_partitionStats[setIndex].m_meshTextureIDsSize,
			m_worldStats.m_partitionStats[setIndex].m_textureContextSize,
			m_worldStats.m_partitionStats[setIndex].m_textureSize,
			m_worldStats.m_partitionStats[setIndex].m_instanceSize,
			m_worldStats.m_partitionStats[setIndex].m_instanceAttributesSize,
			m_worldStats.m_partitionStats[setIndex].m_sceneTreeSize,
			m_worldStats.m_partitionStats[setIndex].m_odbSize,
			m_worldStats.m_partitionStats[setIndex].m_cdbSize,
			m_worldStats.m_partitionStats[setIndex].m_pdbSize,
			m_worldStats.m_partitionStats[setIndex].m_totalSize,
			pName);
	}
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Totals:");
	//                                                m  mtid tc     t   i      ia  st   o   c   p   t  n
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"    %8d   %6d   %6d   %8d %8d     %7d %7d  %7d %7d %6d  %8d",
						meshTotalSize,
						meshTextureIDsTotalSize,
						textureContextTotalSize,
						textureTotalSize,
						instanceTotalSize,
						instanceAttributesTotalSize,
						sceneTreeTotalSize,
						odbTotalSize,
						cdbTotalSize,
						pdbTotalSize,
						totalTotalSize);
}

class PartitionSortData
{
public:
	int m_size;
	int m_index;
	
	PartitionSortData(void) : m_size(0),m_index(0)
	{
	}
	
	bool PartitionSortData::operator==(const PartitionSortData& other) const
	{
		return (m_size==other.m_size);
	}
	
	bool PartitionSortData::operator<=(const PartitionSortData& other) const
	{
		return (m_size<=other.m_size);
	}
	
	bool PartitionSortData::operator>=(const PartitionSortData& other) const
	{
		return (m_size>=other.m_size);
	}
	
	bool PartitionSortData::operator<(const PartitionSortData& other) const
	{
		return (m_size<other.m_size);
	}
	
	bool PartitionSortData::operator>(const PartitionSortData& other) const
	{
		return (m_size>other.m_size);
	}
};

void ExportDBL::AddSortedPartitionStatsToLog(void)
{
	TupArray<int> visibilitySetNodeRefs;
	m_pSceneAgg->FindListByKey("Annotate_VisibilitySetNodeRefs")->GetAsInt(visibilitySetNodeRefs);

	int numVisibilitySets = visibilitySetNodeRefs.GetSize();

	if (numVisibilitySets==1)
		return;

	TupArray<PartitionSortData> partitionSortData;
	partitionSortData.SetSize(numVisibilitySets-1);

	int setIndex;
	for (setIndex=1;setIndex<numVisibilitySets;setIndex++)
	{
		PartitionSortData &partitionStats = partitionSortData[setIndex-1];
		partitionStats.m_index = setIndex;
		partitionStats.m_size += m_worldStats.m_partitionStats[setIndex].m_totalSize;

		int nodeRef = visibilitySetNodeRefs[setIndex];
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeRef);
		TupArray<int> setLinks;
		pNodeAgg->FindListByKey("Annotate_VisibilitySetLinks")->GetAsInt(setLinks);
		int numLinks = setLinks.GetSize();
		for (int linkIndex=0;linkIndex<numLinks;linkIndex++)
		{
			int linkSet = setLinks[linkIndex];
			// don't worry about link to world
			if (linkSet!=0)
			{
				partitionStats.m_size += m_worldStats.m_partitionStats[linkSet].m_totalSize;
			}
		}
	}
	TupSort<PartitionSortData>::SortDescending(partitionSortData.GetData(),numVisibilitySets-1);

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Partition Link Stats:");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW," #    Total Size       Visibility Set Name                      Links");

	for (setIndex=1;setIndex<numVisibilitySets;setIndex++)
	{
		PartitionSortData &partitionStats = partitionSortData[setIndex-1];
		TupString linkString;

		int nodeRef = visibilitySetNodeRefs[partitionStats.m_index];
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeRef);

		TupNode tupNode(pNodeAgg);
		char *pName;
		tupNode.GetName(&pName);
		TupArray<int> setLinks;
		pNodeAgg->FindListByKey("Annotate_VisibilitySetLinks")->GetAsInt(setLinks);
		int numLinks = setLinks.GetSize();
		for (int linkIndex=0;linkIndex<numLinks;linkIndex++)
		{
			int linkSet = setLinks[linkIndex];
			// don't worry about link to world
			if (linkSet!=0)
			{
				char number[10];
				sprintf(number,"%03x ",linkSet);
				linkString += number;
			}
		}

		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"%03x    %8d   %-32s   %s", 
								partitionStats.m_index,
								partitionStats.m_size,
								pName,
								(const char *) linkString);
	}
}

void ExportDBL::ExportLightingSets(void)
{
	ExportLighting exportLighting( m_fileName, m_options, m_dataManager, *m_pTupConfigRoot );

	try
	{
		m_worldStats.m_lightsSize += exportLighting.WriteLightingSets( *m_pTupScene, *m_pTupNodePool, *m_pTupObjectPool );

	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportLightingSets\r\n  %s",(const char *)(e)));
	}
}


void ExportDBL::ExportInstanceAttribs(void)
{
	ExportInstanceAttributes exportInstanceAttributes( m_fileName, m_options, m_dataManager, *m_pTupConfigRoot, m_currentPartition);
	
	try
	{
		m_worldStats.m_partitionStats[m_currentPartition].m_instanceAttributesSize += exportInstanceAttributes.WriteInstanceAttributes(*m_pTupNodePool);
	}
	catch (PluginException e)
	{
		throw (PluginException(ERROR_FATAL,"ExportInstanceAttributes\r\n  %s",(const char *)(e)));
	}
}

void ExportDBL::ExportVarFile(void)
{
	// export a scene.var file if we found a scenesettings node
	TupperwareScalar *pSceneAttribScalar = m_pSceneAgg->FindScalarByKey("Annotate_SceneSettings");
	if (pSceneAttribScalar)
	{
		char path[_MAX_PATH];
		FilePath::GetPath(m_fileNameBase, path);
		strcat(path, "scene.var");
		TupString fileName(path);

		FILE *pFile = fopen(fileName,"wt");
		if (!pFile)
		{
			throw (PluginException(ERROR_FATAL,"ExportVarFile - error opening file %s\r\n",(const char *)(fileName)));
		}

		fprintf(pFile,"# Filterchain generated var file\n");

		TupString platformString("VarFile");
		const char *pValue = m_options.GetValue("Platform");
		if (pValue)
		{
			platformString+=pValue;
			fprintf(pFile,"# Exporting for %s\n",pValue);
		}

		fprintf(pFile,"\ncontext scene\n\n");

		TupString settings(pSceneAttribScalar->GetAsString());
		TupKeyValue tupKeyValue( settings, BEGIN_BLOCK, END_BLOCK );
		TupKeyValueContext tupKeyValueContext;
		m_pTupConfigRoot->GetKeyValueContexts( tupKeyValue, tupKeyValueContext, true );
		int numKeys = tupKeyValueContext.GetNumAttribs();

		for (int index=0;index<numKeys;index++)
		{
			TupString key(tupKeyValueContext.GetKeyAsString(index));
			TupString value(tupKeyValueContext.GetValueAsString(index));
			bool bKeyValid = false;
			// if key for all platforms
			if (key.Find("VARFileAll")==0)
			{
				bKeyValid = true;
				key = key.Mid(10);
			}
			else if (key.Find(platformString)==0)
			{
				bKeyValid = true;
				key = key.Mid(platformString.GetLength());
			}
			if (bKeyValid)
			{
				fprintf(pFile,"%s %s\n",(const char *)key,(const char *)value);
			}
		}
		fprintf(pFile,"\nend\n");
		fclose(pFile);
	}
}