////////////////////////////////////////////////////////////////////////////
//
// ExportTextureContext
//
// Export the texture contexts
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportTextureContext.cpp $
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 10:50a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * streaming system updates
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 9/20/02    Time: 10:04a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * moved build texture contexts into plugin library
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 9/12/02    Time: 2:41p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Updated build Texture context to use an additional class to build the
 * contexts (so the ps2 can do it using the same code)
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 9/03/02    Time: 2:03p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Texture Context and Textures from only used materials and images
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 5/24/02    Time: 8:16a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 5/10/02    Time: 4:10p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * if the texture bind is -1 we need to have the TEXTURE_WHITE_TEXTURE set
 * for the flags
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 5/10/02    Time: 3:42p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * Texture context issues
 * 
 * *****************  Version 13  *****************
 * User: Nmartin      Date: 5/10/02    Time: 2:59p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * working on texture context chunk
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 5/10/02    Time: 2:10p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * PS2_EXPORT define changed to EXPORT_PS2 
 * LIghting checked on meshes
 * can handle node with no material
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 5/07/02    Time: 2:36p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * added hasalpha flag for texture context from the image
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 1:00p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Use common name for precompiled header
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 3/25/02    Time: 12:41p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * hybrid configuration
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 12:56p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Fixed to use Annotate filter
 * Added boundign and annotate helper references
 * Removed export pass
 * Added Dependencies
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/08/02    Time: 12:08p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Offsets, Tiling, and W Rotation of map working
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/07/02    Time: 4:30p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * UV Offsets and Tiling now altering the uvs within the model
 * Pushed pools up to top of export dbl 
 * Renamed all inserted tupperware atoms to ExportPC - 
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 11:05a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Fixed to handle standard material map changes
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/18/02    Time: 1:10p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Fixes for Juju
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/12/02    Time: 5:11p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Instance, Scene Tree, etc
 */

#include "ExportPch.h"
#include "ExportTextureContext.h"

ExportTextureContext::ExportTextureContext(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,
										   TupScene &tupScene,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,
											int currentPartition) :
	m_pDBLFileName(pDBLFileName),
	m_options(options),
	m_dataManager(dataManager),
	m_tupScene(tupScene),
	m_tupMaterialPool(tupMaterialPool),
	m_tupMapPool(tupMapPool),
	m_tupImagePool(tupImagePool),
	m_currentPartition(currentPartition)
{
}

/* collect and write out texture context data */

u32 ExportTextureContext::WriteTextureContexts(void)
{
	TupperwareAggregate *pSceneAgg = m_tupScene.GetBaseAggregate();
	TupArray<int> totalTextureContextsArray;
	pSceneAgg->FindListByKey("Annotate_TotalTextureContexts")->GetAsInt(totalTextureContextsArray);

	int totalTextureContexts = totalTextureContextsArray[m_currentPartition];
	if (totalTextureContexts==0)
		return 0; // no textures in this partition

	ts_FullTextureContext fullTextureContext;

	// get the total number of texture contexts that we need to write

	FILE *pDBLFile = DBLUtil::GetDBLFile();

	/* start chunk */

	DBLUtil::StartChunk(DB_TEXTURE_CONTEXT, DB_TEXTURE_CONTEXT_VERSION, DBL_CHUNK_NO_IGNORE, "1000");

	/* start record list-- writes out dummy data, to be replaced later */

	DBLUtil::StartRecordList(totalTextureContexts, RECORD_LIST_HAS_OFFSET_TABLE, 0);
#if EXPORT_PS2
	bool bPs2 = true;
#else
	bool bPs2 = false;
#endif
	BuildTextureContextManager buildTextureContextManager(m_dataManager,m_tupMaterialPool,m_tupMapPool,m_tupImagePool,m_currentPartition,bPs2);

	int recordCount = 0; // start with no records
	if (m_currentPartition==0)
	{
		// first context is a dummy
		buildTextureContextManager.BuildDummyTextureContext(fullTextureContext);
		DBLUtil::RecordListEntry();
		DBLWrite::WriteRecord((ts_TextureContext *) &fullTextureContext, pDBLFile);	
		// write out the texture layers if there are any
		DBLWrite::WriteRecords(fullTextureContext.TextureLayer, pDBLFile, fullTextureContext.TextureCount);
		
		DBLUtil::AlignFile();

		recordCount++;
	}

	int totalMaterials = m_tupMaterialPool.GetNumMaterials();
	for (int materialIndex=0;materialIndex<totalMaterials;materialIndex++)
	{
		TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial(materialIndex);

		// if pointing to empty slot then continue
		if (!pMaterialAgg)
			continue;

		TupperwareList *pTextureContextIDList = pMaterialAgg->FindListByKey("Annotate_TextureContextIDs");
		if (!pTextureContextIDList)
			continue;

		int *pTextureContextIDData = pTextureContextIDList->GetAsInt();
		int textureContextID = pTextureContextIDData[m_currentPartition];

		// if we are not used in this partition or point to the world one
		if (textureContextID==-1 || (textureContextID&0xff000000)!=0)
			continue;

		// this better be the same
		assert(recordCount==textureContextID);

		TupMaterial tupMaterial(pMaterialAgg);
		
		buildTextureContextManager.BuildTextureContext(tupMaterial,fullTextureContext);

		DBLUtil::RecordListEntry();

		DBLWrite::WriteRecord((ts_TextureContext *) &fullTextureContext, pDBLFile);	// doesn't write out first texture layer
		DBLWrite::WriteRecords(fullTextureContext.TextureLayer, pDBLFile, fullTextureContext.TextureCount);

		DBLUtil::AlignFile();

		recordCount++;
	}

	DBLUtil::AlignFile();

	/* finish record list */
	DBLUtil::FinishRecordList();

	return DBLUtil::FinishChunk(FALSE);
}

