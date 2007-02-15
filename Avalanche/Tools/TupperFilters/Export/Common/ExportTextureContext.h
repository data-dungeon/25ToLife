////////////////////////////////////////////////////////////////////////////
//
// ExportTextureContext
//
// Export the texture contexts
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportTextureContext.h $
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 10:50a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * streaming system updates
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 2/07/03    Time: 10:18a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * removed build texture context helper (used from plugin library)
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 9/12/02    Time: 2:41p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Updated build Texture context to use an additional class to build the
 * contexts (so the ps2 can do it using the same code)
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 9/03/02    Time: 2:03p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Texture Context and Textures from only used materials and images
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 5/07/02    Time: 2:36p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * added hasalpha flag for texture context from the image
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/07/02    Time: 4:30p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * UV Offsets and Tiling now altering the uvs within the model
 * Pushed pools up to top of export dbl 
 * Renamed all inserted tupperware atoms to ExportPC - 
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/12/02    Time: 5:11p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Instance, Scene Tree, etc
 */

#ifndef EXPORT_TEXTURE_CONTEXT_H
#define EXPORT_TEXTURE_CONTEXT_H

class FilterOptions;
class FilterDataManager;
class TupScene;
class TupMapPool;
class TupMaterial;
struct ts_FullTextureContext;

class ExportTextureContext
{
	const char *m_pDBLFileName;
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;
	TupScene &m_tupScene;
	TupMaterialPool &m_tupMaterialPool;
	TupMapPool &m_tupMapPool;
	TupImagePool &m_tupImagePool;

	int m_currentPartition;

	ExportTextureContext(void); // this is not defined

public:
	ExportTextureContext(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,
		TupScene &tupScene,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,
		int currentPartition);

	u32 WriteTextureContexts(void);
};

#endif