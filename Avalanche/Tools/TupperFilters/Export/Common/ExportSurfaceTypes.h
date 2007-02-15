////////////////////////////////////////////////////////////////////////////
//
// ExportSurfaceTypes
//
// Export the names of all the surface types
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportSurfaceTypes.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 10:50a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * streaming system updates
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

#ifndef EXPORT_SURFACE_TYPES_H
#define EXPORT_SURFACE_TYPES_H

class FilterOptions;
class FilterDataManager;
struct ts_TextureChunkRecord;

class ExportSurfaceTypes
{
	const char *m_pDBLFileName;
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;

	ExportSurfaceTypes(void); // this is not defined

public:
	ExportSurfaceTypes(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager);

	u32 WriteSurfaceTypes(TupScene &tupScene);
};


#endif