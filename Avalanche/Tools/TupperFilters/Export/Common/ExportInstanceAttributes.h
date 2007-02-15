////////////////////////////////////////////////////////////////////////////
//
// ExportInstanceAttributes
//
// Export a bunch of key/value pairs into the DBL file
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportInstanceAttributes.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:20p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * new chunk
 */

#ifndef EXPORT_INSTANCE_ATTRIBUTES_H
#define EXPORT_INSTANCE_ATTRIBUTES_H

class TupScene;
class TupNodePool;

class ExportInstanceAttributes
{
	const char *m_pDBLFileName;
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;
	TupConfigRoot &m_tupConfigRoot;

	int m_currentPartition;

	ExportInstanceAttributes(void); // this is not defined

public:
	ExportInstanceAttributes(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupConfigRoot &tupConfigRoot,int currentPartition);
	u32 WriteInstanceAttributes(TupNodePool &tupNodePool);

private:
	void BuildGroupData(TupKeyValueContext &tupKeyValueContext,TupArray<TupKeyValue> &groupData);
	void WriteGroupData(const TupArray<TupKeyValue> &groupData,int instanceIndex);
};

#endif
