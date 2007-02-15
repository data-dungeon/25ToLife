////////////////////////////////////////////////////////////////////////////
//
// ExportStationaryCAm
//
// Export StationaryCam info
//
////////////////////////////////////////////////////////////////////////////
#ifndef EXPORT_STATIONARYCAM_H
#define EXPORT_STATIONARYCAM_H

class TupScene;
class TupNodePool;

class ExportStationaryCam
{
public:
	ExportStationaryCam(const char *pDBLFileName,
					const FilterOptions &options,
					FilterDataManager &dataManager,
					TupConfigRoot &tupConfigRoot);

	// Write the sources
	u32 WriteStationaryCam(TupNodePool &tupNodePool);

private:
	const char 				*m_pDBLFileName;
	const FilterOptions 	&m_options;
	FilterDataManager 	&m_dataManager;
	TupConfigRoot 			&m_tupConfigRoot;

	// Make this private
	ExportStationaryCam(void);
};

#endif
