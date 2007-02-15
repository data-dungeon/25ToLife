////////////////////////////////////////////////////////////////////////////
//
// ExportNamedPoint
//
// Export Named Points into the DBL file
//
////////////////////////////////////////////////////////////////////////////

#ifndef EXPORT_ATTACH_POINT_H
#define EXPORT_ATTACH_POINT_H

class TupScene;
class TupNodePool;

#define MAX_ATTACHPOINTNAME_LEN 16

class ExportAttachPoint
{
	struct ts_AttachPoint
	{
		Graphics4x4 m_matrix;
		char m_cAttachPointName[MAX_ATTACHPOINTNAME_LEN];
		char m_cParentBoneName[MAX_BONENAME_LEN];
		char pad[16];
	};

	const char *m_pDBLFileName;
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;
	TupConfigRoot &m_tupConfigRoot;

	ExportAttachPoint(void); // this is not defined

public:
	ExportAttachPoint(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupConfigRoot &tupConfigRoot);
	u32 WriteAttachPoints(TupNodePool &tupNodePool);
};

#endif
