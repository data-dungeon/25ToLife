////////////////////////////////////////////////////////////////////////////
//
// ExportVisibilityRegion
//
// Export the visibility regions
//
////////////////////////////////////////////////////////////////////////////

#ifndef EXPORT_VISIBILITY_REGION_H
#define EXPORT_VISIBILITY_REGION_H

////////////////////////////////////////////////////////////////////////////

class ExportVisibilityRegion
{
	const char *m_pDBLFileName;
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;

public:
	ExportVisibilityRegion(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager);
	u32 WriteVisibilityRegions(TupScene &tupScene,TupNodePool &tupNodePool,bool bBuildPartitions);
};

////////////////////////////////////////////////////////////////////////////

#endif // EXPORT_VISIBILITY_REGION_H