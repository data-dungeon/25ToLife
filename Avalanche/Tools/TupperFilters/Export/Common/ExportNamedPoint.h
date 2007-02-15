////////////////////////////////////////////////////////////////////////////
//
// ExportNamedPoint
//
// Export Named Points into the DBL file
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportNamedPoint.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 10:50a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * streaming system updates
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/02/02    Time: 11:25a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Named Points Exporting
 */

#ifndef EXPORT_NAMED_POINT_H
#define EXPORT_NAMED_POINT_H

class TupScene;
class TupNodePool;

class ExportNamedPoint
{
	const char *m_pDBLFileName;
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;
	TupConfigRoot &m_tupConfigRoot;

	ExportNamedPoint(void); // this is not defined

public:
	ExportNamedPoint(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupConfigRoot &tupConfigRoot);
	u32 WriteNamedPoints(TupNodePool &tupNodePool);
};

#endif
