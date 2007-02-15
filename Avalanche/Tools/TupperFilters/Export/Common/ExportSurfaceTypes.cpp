////////////////////////////////////////////////////////////////////////////
//
// ExportSurfaceTypes
//
// Export the names of all the surface types
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportSurfaceTypes.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 10:50a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * streaming system updates
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 1:00p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Use common name for precompiled header
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 4/05/02    Time: 4:32p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Surface Type strings now read from node attribute tree
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 4:28p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Surface types chunk
 */

#include "ExportPch.h"
#include "ExportSurfaceTypes.h"

ExportSurfaceTypes::ExportSurfaceTypes(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager) :
	m_pDBLFileName(pDBLFileName), m_options(options), m_dataManager(dataManager)
{
}

/* collect and write out texture data */

u32 ExportSurfaceTypes::WriteSurfaceTypes(TupScene &tupScene)
{
	FILE *pDBLFile = DBLUtil::GetDBLFile();
	TupperwareAggregate *pNodeAttributesAgg = tupScene.GetNodeAttributeTree();
	if (!pNodeAttributesAgg)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_YELLOW|LogFlags::FONT_STYLE_BOLD,"Node Attribute Tree not available");
		return 0;
	}
	TupConfigRoot tupConfigRoot;
	tupConfigRoot.BuildConfigTree(pNodeAttributesAgg);
	TupKeyValue pathToSurfaceTypes;
	pathToSurfaceTypes.AddKeyValue("CollisionType","TerrainOnly");
	TupKeySelection surfaceTypes;
	if (!tupConfigRoot.GetKeySelections(pathToSurfaceTypes,"SurfaceType",surfaceTypes))
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_YELLOW|LogFlags::FONT_STYLE_BOLD,"Surface Types not found in Node Attribute Tree");
		return 0;
	}

	int numSurfaceTypes = surfaceTypes.GetNumSelections();
	if (numSurfaceTypes == 0)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_YELLOW|LogFlags::FONT_STYLE_BOLD,"No Surface types to export");
		return 0;
	}

	DBLUtil::StartChunk(DB_TERRAIN_LIST, DB_TERRAIN_LIST_VERSION, DBL_CHUNK_NO_IGNORE, "1000");
	DBLUtil::StartRecordList(numSurfaceTypes, RECORD_LIST_HAS_OFFSET_TABLE, 0);

	/* write out surface type names */
	for (int surfaceIndex = 0; surfaceIndex < numSurfaceTypes; surfaceIndex++)
	{
		/* mark new record list entry */
		DBLUtil::RecordListEntry();
		/* write data-- include terminating 0 */
		const char *pSurfaceType = surfaceTypes.GetSelection(surfaceIndex);
		DBLWrite::WriteData(pSurfaceType, strlen(pSurfaceType) + 1, pDBLFile);

		DBLUtil::AlignFile();
	}
	DBLUtil::FinishRecordList();
	return DBLUtil::FinishChunk(FALSE);
}

