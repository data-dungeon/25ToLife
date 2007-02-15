////////////////////////////////////////////////////////////////////////////
//
// BuildTextureContext
//
// build the texture contexts
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: BuildTextureContext.h $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 7/03/03    Time: 10:08a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * use map attributes for point sampling and mip mapping
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 11:48a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * update for streaming system
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 2/07/03    Time: 2:59p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * environment mapping
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 1/29/03    Time: 12:09p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * new blending mode stuff (for multitexturing)
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 9/20/02    Time: 12:55p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * moved texture context manager to plugins library
 * added ps2 flag
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/12/02    Time: 4:21p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/12/02    Time: 2:40p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * build texture context class
 */

#ifndef BUILD_TEXTURE_CONTEXT_H
#define BUILD_TEXTURE_CONTEXT_H

class FilterDataManager;
class TupMaterialPool;
class TupMaterial;
class TupMapPool;
class TupImagePool;
struct ts_FullTextureContext;

class BuildTextureContextManager
{
	FilterDataManager &m_dataManager;
	TupMaterialPool &m_tupMaterialPool;
	TupMapPool &m_tupMapPool;
	TupImagePool &m_tupImagePool;
	bool m_bPs2; // flag for indicating ps2 (builds this differently)
	int m_currentPartition;

	BuildTextureContextManager(void); // this is not defined

public:
	enum MapTypes
	{
		MAP_TYPE_DIFFUSE_COLOR,
		MAP_TYPE_REFLECTION,
	};
	BuildTextureContextManager(FilterDataManager &dataManager,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,int currentPartition,bool bPs2);
	void BuildTextureContext(TupMaterial &tupMaterial,ts_FullTextureContext &fullTextureContext);
	void BuildDummyTextureContext(ts_FullTextureContext &fullTextureContext);
private:
	void AddMap(MapTypes mapType,int mapReference,ts_FullTextureContext &fullTextureContext);
	int GetBlend(MapTypes mapType,int blendMode);
	int GetFilter(bool bPointSampling,bool bMipMapping,bool bCrossFade);
	int GetMethod(MapTypes mapType,int blendMode);
		
};

#endif