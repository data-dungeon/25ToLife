////////////////////////////////////////////////////////////////////////////
//
// ExportLighting
//
// Export the lighting sets into the DBL file
//
////////////////////////////////////////////////////////////////////////////
#ifndef EXPORT_LIGHTING_H
#define EXPORT_LIGHTING_H

class TupScene;
class TupNodePool;
class TupObjectPool;

class ExportLighting
{
	const char					*m_pDBLFileName;
	const FilterOptions		&m_options;
	FilterDataManager			&m_dataManager;
	TupConfigRoot				&m_tupConfigRoot;

public:
	ExportLighting( const char *pDBLFileName, const FilterOptions &options, FilterDataManager &dataManager, TupConfigRoot &tupConfigRoot );
	u32 WriteLightingSets( TupScene &tupScene,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool );
};

#endif // EXPORT_LIGHTING_H