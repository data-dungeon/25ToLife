////////////////////////////////////////////////////////////////////////////
//
// MaterialImageRefs
//
// Class to build up an array of images references by a material
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MaterialImageRefs.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/07/03    Time: 3:03p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * look for reflection channel
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 9/25/02    Time: 11:35a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * don't assert on multi-sub object material (just ignore it)
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/17/02    Time: 2:25p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * added bitmap map refs
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/17/02    Time: 1:07p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * material image refs added
 */

////////////////////////////////////////////////////////////////////////////

#include "PluginsPch.h"
#include "MaterialImageRefs.h"

MaterialImageRefsManager::MaterialImageRefsManager(TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool) :
	m_tupMaterialPool(tupMaterialPool),m_tupMapPool(tupMapPool)
{
}

void MaterialImageRefsManager::BuildImageRefs(int materialReference,TupArray<MaterialImageRefsManager::MaterialImageData> &imageRefsArray)
{
	imageRefsArray.RemoveAll();
	AddMaterial(materialReference,imageRefsArray);
}

void MaterialImageRefsManager::AddMaterial(int materialReference,TupArray<MaterialImageRefsManager::MaterialImageData> &imageRefsArray)
{
	if (materialReference!=-1)
	{
		TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial(materialReference);
		if (pMaterialAgg)
		{
			TupMaterial tupMaterial(pMaterialAgg);
			switch (tupMaterial.GetMaterialType())
			{
				// these have no images or maps (we handle the materials these point to in different places)
				case TupMaterial::MULTI_SUB_OBJECT_MATERIAL:
					break;
				case TupMaterial::STANDARD_MATERIAL:
					{
						TupStandardMaterial tupStandardMaterial(tupMaterial);

						// look for diffuse color
						int totalMaps = tupStandardMaterial.GetNumMaps();
						int mapIndex;
						for(mapIndex=0;mapIndex<totalMaps;mapIndex++ )
						{
							TupperwareAggregate *pMapAgg = tupStandardMaterial.GetMap(mapIndex);
							TupStandardMaterialMap tupMaterialMap(pMapAgg);
							char *mapName;
							tupMaterialMap.GetMapName(&mapName);
							assert(mapName);
							if (!stricmp(mapName,"Diffuse Color"))
							{
								int mapRef;
								tupMaterialMap.GetMapReference(mapRef);
								AddMap(mapRef,imageRefsArray);
							}
						}
						// look for reflection
						for(mapIndex=0;mapIndex<totalMaps;mapIndex++ )
						{
							TupperwareAggregate *pMapAgg = tupStandardMaterial.GetMap(mapIndex);
							TupStandardMaterialMap tupMaterialMap(pMapAgg);
							char *mapName;
							tupMaterialMap.GetMapName(&mapName);
							assert(mapName);
							if (!stricmp(mapName,"Reflection"))
							{
								int mapRef;
								tupMaterialMap.GetMapReference(mapRef);
								AddMap(mapRef,imageRefsArray);
							}
						}
					}
					break;
				case TupMaterial::COMPOSITE_MATERIAL:
					{
						TupCompositeMaterial tupCompositeMaterial(pMaterialAgg);
						int baseMaterialRef;
						tupCompositeMaterial.GetBaseMaterialReference(baseMaterialRef);
						if (baseMaterialRef!=-1)
						{
							AddMaterial(baseMaterialRef,imageRefsArray);
						}
						int numSubMaterials = tupCompositeMaterial.GetNumSubMaterials();
						int subMaterialIndex;
						for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
						{
							TupperwareAggregate *pSubMaterialAgg = tupCompositeMaterial.GetSubMaterial(subMaterialIndex);
							TupCompositeMaterialSubMaterial subMat(pSubMaterialAgg);
							int flags;
							subMat.GetFlags(flags);
							if (flags & TupCompositeMaterialSubMaterial::ENABLED)
							{
								int subMaterialRef;
								subMat.GetMaterialReference(subMaterialRef);
								assert(subMaterialRef!=-1);
								AddMaterial(subMaterialRef,imageRefsArray);
							}
						}
					}
					break;
				case TupMaterial::BLEND_MATERIAL:
					{
						TupBlendMaterial tupBlendMaterial(pMaterialAgg);
						int material1Ref;
						TupperwareScalar *pMaterial1RefScalar = tupBlendMaterial.GetMaterial1Reference(material1Ref);
						if (!pMaterial1RefScalar)
							material1Ref = -1;
						if (material1Ref!=-1)
						{
							AddMaterial(material1Ref,imageRefsArray);
						}

						int material2Ref;
						TupperwareScalar *pMaterial2RefScalar = tupBlendMaterial.GetMaterial2Reference(material2Ref);
						if (!pMaterial2RefScalar)
							material2Ref = -1;
						if (material2Ref!=-1)
						{
							AddMaterial(material2Ref,imageRefsArray);
						}

						int maskMapRef;
						TupperwareScalar *pMaskMapRefScalar = tupBlendMaterial.GetMaskReference(maskMapRef);
						if (!pMaskMapRefScalar)
							maskMapRef = -1;
						if (maskMapRef!=-1)
						{
							AddMap(maskMapRef,imageRefsArray);
						}
					}
					break;
				case TupMaterial::DOUBLE_SIDED_MATERIAL:
					{
						TupDoubleSidedMaterial tupDoubleSidedMaterial(pMaterialAgg);
						int frontMaterialRef;
						TupperwareScalar *pFrontMaterialRefScalar = tupDoubleSidedMaterial.GetFrontMaterialReference(frontMaterialRef);
						if (pFrontMaterialRefScalar && frontMaterialRef!=-1)
						{
							AddMaterial(frontMaterialRef,imageRefsArray);
						}
						// ignore the other side for now
					}
					break;
				default:
//					assert(false);
					break;
			}
		}
	}
}

void MaterialImageRefsManager::AddMap(int mapReference,TupArray<MaterialImageRefsManager::MaterialImageData> &imageRefsArray)
{
	assert(mapReference!=-1);
	// if we can't add any more then stop
	if (imageRefsArray.GetSize()==MAX_SOM_TEXTURES_PER_PATCH)
		return;

	TupperwareAggregate *pMapAgg = m_tupMapPool.GetMap(mapReference);
	TupMap tupMap(pMapAgg);
	switch (tupMap.GetMapType())
	{
		case TupMap::BITMAP_MAP:
			{
				MaterialImageRefsManager::MaterialImageData newMaterialImageData;
				newMaterialImageData.m_bitmapMapRef = mapReference;

				TupBitmapMap tupBitmapMap(tupMap);
				tupBitmapMap.GetImageReference(newMaterialImageData.m_imageRef);
				assert(newMaterialImageData.m_imageRef!=-1);
				imageRefsArray.Add(newMaterialImageData);
			}
			break;
		case TupMap::COMPOSITE_MAP:
			{
				TupCompositeMap tupCompositeMap(tupMap);
				int numSubMaps = tupCompositeMap.GetNumSubMaps();
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupCompositeMap.GetSubMap(subMapIndex);
					TupCompositeMapSubMap tupSubMap(pSubMapAgg);
					int mapRef;
					tupSubMap.GetMapReference(mapRef);
					AddMap(mapRef,imageRefsArray);
				}
			}
			break;
		case TupMap::MIX_MAP:
			{
				TupMixMap tupMixMap(pMapAgg);
				int map1Ref;
				TupperwareScalar *pMap1RefScalar = tupMixMap.GetMap1Reference(map1Ref);
				if (!pMap1RefScalar)
					map1Ref = -1;
				int map2Ref;
				TupperwareScalar *pMap2RefScalar = tupMixMap.GetMap2Reference(map2Ref);
				if (!pMap2RefScalar)
					map2Ref = -1;
				int mixMapRef;
				TupperwareScalar *pMixMapRefScalar = tupMixMap.GetMixMapReference(mixMapRef);
				if (!pMixMapRefScalar)
					mixMapRef = -1;
				if (map1Ref!=-1)
					AddMap(map1Ref,imageRefsArray);
				if (map2Ref!=-1)
					AddMap(map2Ref,imageRefsArray);
				if (mixMapRef!=-1)
					AddMap(mixMapRef,imageRefsArray);
			}
			break;
		default:
//			assert(false);
			break;
	}
}
