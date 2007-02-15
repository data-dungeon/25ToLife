////////////////////////////////////////////////////////////////////////////
//
// ExportBoneGroupTable
//
// Export the names of the bones, their groups and any mirror bones
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportBoneGroupTable.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 10:50a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * streaming system updates
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 6/03/02    Time: 3:28p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * First Pass at bone export
 */

#ifndef EXPORT_BONE_GROUP_TABLES_H
#define EXPORT_BONE_GROUP_TABLES_H

class BoneGroupTable;

class ExportBoneGroupTable
{
	const char *m_pDBLFileName;
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;

	ExportBoneGroupTable(void); // this is not defined

public:
	ExportBoneGroupTable(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager);

	u32 WriteBoneGroupTable(BoneGroupTable &boneGroupTable);
};


#endif