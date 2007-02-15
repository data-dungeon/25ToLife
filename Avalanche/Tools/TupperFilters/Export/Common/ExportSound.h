////////////////////////////////////////////////////////////////////////////
//
// ExportSound
//
// Export sound sources and fields into the DBL file
//
////////////////////////////////////////////////////////////////////////////
#ifndef EXPORT_SOUND_H
#define EXPORT_SOUND_H

class TupScene;
class TupNodePool;

class ExportSound
{
public:
	ExportSound(const char *pDBLFileName,
					const FilterOptions &options,
					FilterDataManager &dataManager,
					TupConfigRoot &tupConfigRoot);

	// Write the sources
	u32 WriteSources(TupNodePool &tupNodePool);

	// Write the fields
	u32 WriteFields(TupNodePool &tupNodePool);

private:
	const char 				*m_pDBLFileName;
	const FilterOptions 	&m_options;
	FilterDataManager 	&m_dataManager;
	TupConfigRoot 			&m_tupConfigRoot;

	// Make this private
	ExportSound(void);
};

#endif
