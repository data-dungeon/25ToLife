////////////////////////////////////////////////////////////////////////////
//
// MaterialRemap
//
// Class to handle remapping a material into other materials
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MaterialRemap.h $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 2/07/03    Time: 2:59p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * environment mapping
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 10/01/02   Time: 9:47a
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * build uv manager needs the image pool (for image rotation)
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 9/20/02    Time: 2:40p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * better check of opacity map to diffuse color map
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/03/02    Time: 2:05p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * start of hookup phase
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/28/02    Time: 4:38p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Material Remap Skeleton & MultiTexMesh done
 */

////////////////////////////////////////////////////////////////////////////

#ifndef MATERIAL_REMAP
#define MATERIAL_REMAP

#ifndef MULTI_TEX_PARTS
#include "MultiTexParts.h"
#endif

class FaceLayerEntry
{
public:
	TupArray<int> m_diffuseColorMapRefs;
	TupArray<int> m_reflectionMapRefs;
	TupArray<ApplyAlphaEntry> m_alphaEntries;
};

class MaterialMapRefs
{
public:
	int m_diffuseColorMapRef;
	int m_reflectionMapRef;
};

class MaterialRemapManager
{
	TupMaterialPool &m_tupMaterialPool;
	TupMapPool &m_tupMapPool;
	TupImagePool &m_tupImagePool;
	enum MapType
	{
		DIFFUSE_COLOR_MAP,
		REFLECTION_MAP,
	};
	
public:
	MaterialRemapManager(TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool);
	void SetupMaterialRemap(int materialRef,MaterialRemapEntry &materialRemapEntry);
private:
	bool MapsEqualForAlpha(int diffuseMapRef,int alphaMapRef);

	int FindOrCreateMap(TupArray<int> &mapRefs);
	int FindOrCreateMaterial(const char *pMaterialName,int standardMaterialRef,MaterialMapRefs &materialMapRefs);
	int FindStandardMaterial(int materialRef);

	void AddMaterialMaps(int materialRef,TupArray<ApplyAlphaEntry> &alphaEntries,TupArray<FaceLayerEntry> &faceLayerArray);
	void AddStandardMaterialMaps(TupperwareAggregate *pMaterialAgg,TupArray<ApplyAlphaEntry> &alphaEntries,TupArray<FaceLayerEntry> &faceLayerArray);
	void AddBlendMaterialMaps(TupperwareAggregate *pMaterialAgg,TupArray<ApplyAlphaEntry> &alphaEntries,TupArray<FaceLayerEntry> &faceLayerArray);
	void AddCompositeMaterialMaps(TupperwareAggregate *pMaterialAgg,TupArray<ApplyAlphaEntry> &alphaEntries,TupArray<FaceLayerEntry> &faceLayerArray);
	void AddDoubleSidedMaterialMaps(TupperwareAggregate *pMaterialAgg,TupArray<ApplyAlphaEntry> &alphaEntries,TupArray<FaceLayerEntry> &faceLayerArray);

	void AddMap(int mapRef,MapType mapType,TupArray<ApplyAlphaEntry> &alphaEntries,TupArray<FaceLayerEntry> &faceLayerArray);
	void AddBitmapMap(TupperwareAggregate *pMapAgg,int mapRef,MapType mapType,TupArray<ApplyAlphaEntry> &alphaEntries,TupArray<FaceLayerEntry> &faceLayerArray);
	void AddCompositeMap(TupperwareAggregate *pMapAgg,MapType mapType,TupArray<ApplyAlphaEntry> &alphaEntries,TupArray<FaceLayerEntry> &faceLayerArray);
	void AddMixMap(TupperwareAggregate *pMapAgg,MapType mapType,TupArray<ApplyAlphaEntry> &alphaEntries,TupArray<FaceLayerEntry> &faceLayerArray);

	void AddOpacityMap(int mapRef,float amount,TupArray<ApplyAlphaEntry> &alphaEntries);
	void AddOpacityBitmapMap(TupperwareAggregate *pMapAgg,int mapRef,float amount,TupArray<ApplyAlphaEntry> &alphaEntries);
	void AddOpacityCompositeMap(TupperwareAggregate *pMapAgg,int mapRef,float amount,TupArray<ApplyAlphaEntry> &alphaEntries);
	void AddOpacityMixMap(TupperwareAggregate *pMapAgg,int mapRef,float amount,TupArray<ApplyAlphaEntry> &alphaEntries);

	bool CompareAlphaEntryArrays(TupArray<ApplyAlphaEntry> &s1,TupArray<ApplyAlphaEntry> &s2);
	bool CompareAlphaEntries(ApplyAlphaEntry &s1,ApplyAlphaEntry &s2);

};

#endif
