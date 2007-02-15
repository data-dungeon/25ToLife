////////////////////////////////////////////////////////////////////////////
//
// ExportInstanceGroup
//
// Export instance groups into the DBL file
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportInstanceGroup.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 10:50a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * streaming system updates
 * 
 * *****************  Version 2  *****************
 * User: Nmartin      Date: 10/30/02   Time: 9:17a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/02/02    Time: 11:25a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Named Points Exporting
 */

#ifndef EXPORT_INSTANCE_GROUP_H
#define EXPORT_INSTANCE_GROUP_H

class TupScene;
class TupNodePool;

class ExportInstanceGroup
{
	const char *m_pDBLFileName;
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;
	TupConfigRoot &m_tupConfigRoot;

	ExportInstanceGroup(void); // this is not defined

public:
	ExportInstanceGroup(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupConfigRoot &tupConfigRoot);
	u32 WriteInstanceGroups(TupNodePool &tupNodePool);
};

#endif
