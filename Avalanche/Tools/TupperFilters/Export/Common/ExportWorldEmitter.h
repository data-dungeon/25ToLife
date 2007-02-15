////////////////////////////////////////////////////////////////////////////
//
// ExportWorldEmitter
//
// Export World Emitters into the DBL file
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportWorldEmitter.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 10:50a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * streaming system updates
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/15/02    Time: 3:45p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * World Emitters
 * Config tree change for named points
 */

#ifndef EXPORT_WORLD_EMITTER_H
#define EXPORT_WORLD_EMITTER_H

class TupScene;
class TupNodePool;

class ExportWorldEmitter
{
	const char *m_pDBLFileName;
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;
	TupConfigRoot &m_tupConfigRoot;

	ExportWorldEmitter(void); // this is not defined

public:
	ExportWorldEmitter(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupConfigRoot &tupConfigRoot);
	u32 WriteWorldEmmitters(TupNodePool &tupNodePool);
};

#endif
