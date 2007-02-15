////////////////////////////////////////////////////////////////////////////
//
// ExportHalo
//
// Export halo sources and fields into the DBL file
//
////////////////////////////////////////////////////////////////////////////
#ifndef EXPORT_HALO_H
#define EXPORT_HALO_H

class TupScene;
class TupNodePool;

class ExportHalo
{
public:
	ExportHalo(const char *pDBLFileName,
					const FilterOptions &options,
					FilterDataManager &dataManager,
					TupConfigRoot &tupConfigRoot);

	// Write the sources
	u32 WriteHalos(TupNodePool &tupNodePool);

private:
	const char 				*m_pDBLFileName;
	const FilterOptions 	&m_options;
	FilterDataManager 	&m_dataManager;
	TupConfigRoot 			&m_tupConfigRoot;

	// Make this private
	ExportHalo(void);
};

#endif
