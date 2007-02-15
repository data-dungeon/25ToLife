////////////////////////////////////////////////////////////////////////////
//
// ExportPath
//
// Export Paths into the DBL file
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportPath.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 10:50a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * streaming system updates
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/03/02    Time: 2:36p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Paths Exporting
 */

#ifndef EXPORT_PATH_H
#define EXPORT_PATH_H

class TupScene;
class TupNodePool;
class TupObjectPool;

class ExportPath
{

	const char *m_pDBLFileName;
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;

	ExportPath(void); // this is not defined

public:
	ExportPath(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager);
	u32 WritePaths(TupScene &tupScene,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool);
};

#endif
