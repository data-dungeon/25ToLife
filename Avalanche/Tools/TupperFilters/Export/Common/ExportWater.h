////////////////////////////////////////////////////////////////////////////
//
// ExportWater
//
// Export water into the DBL file
//
////////////////////////////////////////////////////////////////////////////

#ifndef EXPORT_WATER_H
#define EXPORT_WATER_H

class TupScene;
class TupNodePool;

////////////////////////////////////////////////////////////////////////////

class ExportWater
{
public:
	ExportWater(const char *pDBLFileName,
					const FilterOptions &options,
					FilterDataManager &dataManager,
					TupConfigRoot &tupConfigRoot);

	// Write the water
	u32 WriteWater(TupNodePool &tupNodePool);

private:
	const char 				*m_pDBLFileName;
	const FilterOptions 	&m_options;
	FilterDataManager 	&m_dataManager;
	TupConfigRoot 			&m_tupConfigRoot;

	// Make this private
	ExportWater(void);
};

#endif
