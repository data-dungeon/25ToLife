////////////////////////////////////////////////////////////////////////////
//
// MaterialRemap
//
// Class to handle remapping a material into other materials
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MaterialRemap.cpp $
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 2/07/03    Time: 2:59p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * environment mapping
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 10/02/02   Time: 11:27a
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Changed UV transform classes
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 10/01/02   Time: 9:47a
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * build uv manager needs the image pool (for image rotation)
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 9/30/02    Time: 1:07p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * handle invert alpha
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 9/23/02    Time: 3:23p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * fixed search to find valid material (instead of creating a new one)
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 9/20/02    Time: 2:40p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * better check of opacity map to diffuse color map
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 9/18/02    Time: 4:34p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * using build uv transform manager
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 9/12/02    Time: 4:37p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Handle multi-sub object material with a -1 reference
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 9/04/02    Time: 11:35a
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
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

#include "MultiTexPch.h"
#include "MaterialRemap.h"

MaterialRemapManager::MaterialRemapManager(TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool) :
	m_tupMaterialPool(tupMaterialPool), m_tupMapPool(tupMapPool),m_tupImagePool(tupImagePool)
{
}

void MaterialRemapManager::SetupMaterialRemap(int materialRef,MaterialRemapEntry &materialRemapEntry)
{
	TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial(materialRef);
	char *pMaterialName;
	TupMaterial tupMaterial(pMaterialAgg);
	tupMaterial.GetName(&pMaterialName);
	int standardMaterialRef = FindStandardMaterial(materialRef);

	bool bKeepMaterial = false;

	// start with no layers
	materialRemapEntry.m_layerArray.RemoveAll();

	TupArray<ApplyAlphaEntry> alphaEntries;
	TupArray<FaceLayerEntry> faceLayerArray;
	// set up the layers each with their alpha entries
	AddMaterialMaps(materialRef,alphaEntries,faceLayerArray);
	int numLayers = faceLayerArray.GetSize();
	// 1 layer
	if (numLayers==1)
	{
		FaceLayerEntry &layerEntry = faceLayerArray[0];
		
		// if there is only 1 diffuse color map
		if (layerEntry.m_diffuseColorMapRefs.GetSize()==1)
		{
			// if there is one alpha entry and it is the same map as the diffuse color
			if (layerEntry.m_alphaEntries.GetSize()==1 && 
				layerEntry.m_alphaEntries[0].m_uvTransform.m_mapRef!=-1 && 
				MapsEqualForAlpha(layerEntry.m_diffuseColorMapRefs[0],layerEntry.m_alphaEntries[0].m_uvTransform.m_mapRef))
			{
				layerEntry.m_alphaEntries.RemoveAll(); // don't apply any alpha (already on diffuse color)
			}
		}
	}
	if (numLayers)
	{
		for (int layerIndex=0;layerIndex<numLayers;layerIndex++)
		{
			FaceLayerEntry &layerEntry = faceLayerArray[layerIndex];
			MaterialMapRefs materialMapRefs;
			materialMapRefs.m_diffuseColorMapRef = FindOrCreateMap(layerEntry.m_diffuseColorMapRefs);
			materialMapRefs.m_reflectionMapRef = FindOrCreateMap(layerEntry.m_reflectionMapRefs);

			MaterialLayerEntry newLayerEntry;
			newLayerEntry.m_materialRef = FindOrCreateMaterial(pMaterialName,standardMaterialRef,materialMapRefs);
			newLayerEntry.m_alphaEntries = layerEntry.m_alphaEntries;
			materialRemapEntry.m_layerArray.Add(newLayerEntry);
		}
	}
	// no layers
	else
	{
		MaterialMapRefs materialMapRefs;
		materialMapRefs.m_diffuseColorMapRef = -1;
		materialMapRefs.m_reflectionMapRef = -1;

		MaterialLayerEntry newLayerEntry;
		newLayerEntry.m_materialRef = FindOrCreateMaterial(pMaterialName,standardMaterialRef,materialMapRefs);
		materialRemapEntry.m_layerArray.Add(newLayerEntry);
	}
}

bool MaterialRemapManager::MapsEqualForAlpha(int diffuseMapRef,int alphaMapRef)
{
	if (diffuseMapRef == alphaMapRef)
		return true;
	TupperwareAggregate *pDiffuseMapAgg = m_tupMapPool.GetMap(diffuseMapRef);
	TupMap tupDiffuseMap(pDiffuseMapAgg);
	TupperwareAggregate *pAlphaMapAgg = m_tupMapPool.GetMap(alphaMapRef);
	TupMap tupAlphaMap(pAlphaMapAgg);
	if (tupDiffuseMap.GetMapType()==TupMap::BITMAP_MAP && tupAlphaMap.GetMapType()==TupMap::BITMAP_MAP )
	{
		TupBitmapMap tupDiffuseMap(pDiffuseMapAgg);
		TupBitmapMap tupAlphaMap(pAlphaMapAgg);
		int diffuseImageRef;
		tupDiffuseMap.GetImageReference(diffuseImageRef);
		int alphaImageRef;
		tupAlphaMap.GetImageReference(alphaImageRef);
		if (diffuseImageRef!=alphaImageRef)
			return false;
		float temp1,temp2;
		tupDiffuseMap.GetUOffset(temp1);
		tupAlphaMap.GetUOffset(temp2);
		if (temp1!=temp2)
			return false;
		tupDiffuseMap.GetVOffset(temp1);
		tupAlphaMap.GetVOffset(temp2);
		if (temp1!=temp2)
			return false;
		tupDiffuseMap.GetUTiling(temp1);
		tupAlphaMap.GetUTiling(temp2);
		if (temp1!=temp2)
			return false;
		tupDiffuseMap.GetVTiling(temp1);
		tupAlphaMap.GetVTiling(temp2);
		if (temp1!=temp2)
			return false;
		tupDiffuseMap.GetUAngle(temp1);
		tupAlphaMap.GetUAngle(temp2);
		if (temp1!=temp2)
			return false;
		tupDiffuseMap.GetVAngle(temp1);
		tupAlphaMap.GetVAngle(temp2);
		if (temp1!=temp2)
			return false;
		tupDiffuseMap.GetWAngle(temp1);
		tupAlphaMap.GetWAngle(temp2);
		if (temp1!=temp2)
			return false;

		return true;
	}
	else
	{
		return false;
	}
}


int MaterialRemapManager::FindOrCreateMap(TupArray<int> &mapRefs)
{
	int numMapRefs = mapRefs.GetSize();
	if (numMapRefs==0)
	{
		return -1;
	}
	else if (numMapRefs==1)
	{
		return mapRefs[0]; // if only one then return this one
	}
	else
	{
		int numMaps = m_tupMapPool.GetNumMaps();
		for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
		{
			TupperwareAggregate *pMapAgg = m_tupMapPool.GetMap(mapIndex);
			if (!pMapAgg)
				continue;
			TupMap tupMap(pMapAgg);
			if (tupMap.GetMapType() == TupMap::COMPOSITE_MAP)
			{
				TupCompositeMap tupCompositeMap(pMapAgg);
				int numSubMaps = tupCompositeMap.GetNumSubMaps();
				if (numSubMaps == numMapRefs)
				{
					int subMapIndex;
					for (subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
					{
						TupperwareAggregate *pSubMapAgg = tupCompositeMap.GetSubMap(subMapIndex);
						TupCompositeMapSubMap tupSubMap(pSubMapAgg);
						int subMapRef;
						tupSubMap.GetMapReference(subMapRef);
						if (subMapRef!=mapRefs[subMapIndex])
							break; // stop if there is a difference
					}
					// if all the submaprefs match then use this composite
					if (subMapIndex==numSubMaps)
					{
						return mapIndex;
					}
				}
			}
		}
		// if we are here we didn't find a match
		// create a new composite map
		int newMapIndex = m_tupMapPool.AddMap();
		TupperwareAggregate *pNewMapAgg = m_tupMapPool.GetMap(newMapIndex);
		TupMap tupMap(pNewMapAgg);
		tupMap.SetMapType(TupMap::COMPOSITE_MAP);
		TupCompositeMap tupCompositeMap(pNewMapAgg);
		tupCompositeMap.AddName("Composite");
		// add each submap
		for (int mapRefIndex=0;mapRefIndex<numMapRefs;mapRefIndex++)
		{
			TupperwareAggregate *pNewSubMapAgg = tupCompositeMap.AddSubMap(mapRefIndex);
			TupCompositeMapSubMap tupSubMap(pNewSubMapAgg);
			tupSubMap.AddFlags(TupCompositeMapSubMap::ENABLED); // all are on
			tupSubMap.AddMapReference(mapRefs[mapRefIndex]);
		}
		return newMapIndex;
	}
}

int MaterialRemapManager::FindOrCreateMaterial(const char *pMaterialName,int standardMaterialRef,MaterialMapRefs &materialMapRefs)
{
	int numMaterials = m_tupMaterialPool.GetNumMaterials();
	for (int materialIndex=0;materialIndex<numMaterials;materialIndex++)
	{
		TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial(materialIndex);
		if (!pMaterialAgg)
			continue;
		TupMaterial tupMaterial(pMaterialAgg);
		if (tupMaterial.GetMaterialType() != TupMaterial::STANDARD_MATERIAL)
			continue;
		char *pOtherMaterialName;
		tupMaterial.GetName(&pOtherMaterialName);
		// make sure the name matches
		if (strcmp(pMaterialName,pOtherMaterialName)!=0)
			continue;

		TupStandardMaterial tupStandardMaterial(pMaterialAgg);
		int numMaps = tupStandardMaterial.GetNumMaps();
		int mapIndex;
		bool bFoundMaps = false;
		for (mapIndex=0;mapIndex<numMaps;mapIndex++)
		{
			TupperwareAggregate *pMapAgg = tupStandardMaterial.GetMap(mapIndex);
			TupStandardMaterialMap tupMaterialMap(pMapAgg);

			char *pMapName;
			tupMaterialMap.GetMapName(&pMapName);
			assert(pMapName);
			// if we found a diffuse color and we need one
			if (!stricmp(pMapName,"Diffuse Color") && materialMapRefs.m_diffuseColorMapRef!=-1)
			{
				// use the diffuse map for now
				int mapRef;
				tupMaterialMap.GetMapReference(mapRef);
				if (mapRef!=materialMapRefs.m_diffuseColorMapRef)
				{
					bFoundMaps = false;
					break;
				}
				else
				{
					bFoundMaps = true;
				}
			}
			// if we found a reflection channel and we need one
			else if (!stricmp(pMapName,"Reflection") && materialMapRefs.m_reflectionMapRef!=-1)
			{
				// use the diffuse map for now
				int mapRef;
				tupMaterialMap.GetMapReference(mapRef);
				if (mapRef!=materialMapRefs.m_reflectionMapRef)
				{
					bFoundMaps = false;
					break;
				}
				else
				{
					bFoundMaps = true;
				}
			}
			else // if any others then stop
			{
				bFoundMaps = false;
				break;
			}
		}
		// if we found a match
		if (bFoundMaps)
		{
			return materialIndex;
		}
	}
	// if we are here we didn't find a match
	// create a new standard material
	int newMaterialIndex = m_tupMaterialPool.AddMaterial();
	TupperwareAggregate *pNewMaterialAgg = m_tupMaterialPool.GetMaterial(newMaterialIndex);
	// copy stuff from this one
	if (standardMaterialRef!=-1)
	{
		TupperwareAggregate *pSourceMaterialAgg = m_tupMaterialPool.GetMaterial(standardMaterialRef);
		// copy all the stuff from this one as a start
		for (TupperwareIterator iter = pSourceMaterialAgg->FindAll(); !iter.Finished(); iter.Next()) 
		{
			pNewMaterialAgg->Copy(iter.GetAsAtom());
		}
		TupperwareAggregate *pMapsAgg = pNewMaterialAgg->FindAggregateByKey("Maps");
		// delete the maps (we don't need this part)
		if (pMapsAgg)
		{
			pNewMaterialAgg->DeleteAtom(pMapsAgg);
		}
	}
	else
	{
		TupMaterial tupMaterial(pNewMaterialAgg);
		tupMaterial.SetMaterialType(TupMaterial::STANDARD_MATERIAL);
		// set other stuff later
	}
	// add in the maps
	TupStandardMaterial tupStandardMaterial(pNewMaterialAgg);
	tupStandardMaterial.AddName(pMaterialName);
	if (materialMapRefs.m_diffuseColorMapRef!=-1)
	{
		TupperwareAggregate *pMapAgg = tupStandardMaterial.AddMap(0);
		TupStandardMaterialMap tupMatMap(pMapAgg);
		tupMatMap.AddAmount(1.0);
		tupMatMap.AddFlags(TupStandardMaterialMap::ENABLED);
		tupMatMap.AddMapName("Diffuse Color");
		tupMatMap.AddMapReference(materialMapRefs.m_diffuseColorMapRef);
	}
	if (materialMapRefs.m_reflectionMapRef!=-1)
	{
		TupperwareAggregate *pMapAgg = tupStandardMaterial.AddMap(1);
		TupStandardMaterialMap tupMatMap(pMapAgg);
		tupMatMap.AddAmount(1.0);
		tupMatMap.AddFlags(TupStandardMaterialMap::ENABLED);
		tupMatMap.AddMapName("Reflection");
		tupMatMap.AddMapReference(materialMapRefs.m_reflectionMapRef);
	}
	return newMaterialIndex;
}

int MaterialRemapManager::FindStandardMaterial(int materialRef)
{
	TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial(materialRef);
	assert(pMaterialAgg);
	TupMaterial tupMaterial(pMaterialAgg);
	TupMaterial::MaterialTypes materialType = tupMaterial.GetMaterialType();
	switch (materialType)
	{
		case TupMaterial::STANDARD_MATERIAL:
			return materialRef;
		case TupMaterial::MULTI_SUB_OBJECT_MATERIAL:
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
					int materialRef = FindStandardMaterial(material1Ref);
					if (materialRef!=-1)
						return materialRef;
				}

				int material2Ref;
				TupperwareScalar *pMaterial2RefScalar = tupBlendMaterial.GetMaterial2Reference(material2Ref);
				if (!pMaterial2RefScalar)
					material2Ref = -1;
				if (material2Ref!=-1)
				{
					int materialRef = FindStandardMaterial(material2Ref);
					if (materialRef!=-1)
						return materialRef;
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
					int materialRef = FindStandardMaterial(baseMaterialRef);
					if (materialRef!=-1)
						return materialRef;
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
						int materialRef = FindStandardMaterial(subMaterialRef);
						if (materialRef!=-1)
							return materialRef;
					}
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
					int materialRef = FindStandardMaterial(frontMaterialRef);
					if (materialRef!=-1)
						return materialRef;
				}

			}
			break;
	}
	return -1;
}


void MaterialRemapManager::AddMaterialMaps(int materialRef,TupArray<ApplyAlphaEntry> &alphaEntries,TupArray<FaceLayerEntry> &faceLayerArray)
{
	TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial(materialRef);
	TupMaterial tupMaterial(pMaterialAgg);
	switch(tupMaterial.GetMaterialType())
	{
		case TupMaterial::STANDARD_MATERIAL:
			{
				AddStandardMaterialMaps(pMaterialAgg,alphaEntries,faceLayerArray);
			}
			break;
		case TupMaterial::MULTI_SUB_OBJECT_MATERIAL:
			{
				char *pMaterialName = pMaterialAgg->FindScalarByKey("Name")->GetAsString();
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
					"Warning - Child Multi-Sub Object Material %s Found. - Ignored",pMaterialName);
			}
			break;
		case TupMaterial::BLEND_MATERIAL:
			{
				AddBlendMaterialMaps(pMaterialAgg,alphaEntries,faceLayerArray);
			}
			break;
		case TupMaterial::COMPOSITE_MATERIAL:
			{
				AddCompositeMaterialMaps(pMaterialAgg,alphaEntries,faceLayerArray);
			}
			break;
		case TupMaterial::DOUBLE_SIDED_MATERIAL:
			{
				AddDoubleSidedMaterialMaps(pMaterialAgg,alphaEntries,faceLayerArray);
			}
			break;
		case TupMaterial::UNKNOWN:
			{
			}
			break;
	}
}


void MaterialRemapManager::AddStandardMaterialMaps(TupperwareAggregate *pMaterialAgg,TupArray<ApplyAlphaEntry> &alphaEntries,TupArray<FaceLayerEntry> &faceLayerArray)
{
	// remember the length of this array to restore when finished
	int startAlphaEntriesSize = alphaEntries.GetSize();

	TupStandardMaterial tupStandardMaterial(pMaterialAgg);
	int numMaps = tupStandardMaterial.GetNumMaps();
	int mapIndex;
	// do opacity of material
	float opacity;
	TupperwareScalar *pOpacityScalar = tupStandardMaterial.GetOpacity(opacity);
	if (!pOpacityScalar)
		opacity = 1.0f;
	if (opacity!=1.0f)
	{
		ApplyAlphaEntry newAlphaEntry;
		newAlphaEntry.m_constantAlpha = opacity;
		alphaEntries.Add(newAlphaEntry);
	}
	// scan for opacity maps
	for (mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMatMap = tupStandardMaterial.GetMap(mapIndex);
		TupStandardMaterialMap tupMaterialMap(pMatMap);

		char *pMapName;
		tupMaterialMap.GetMapName(&pMapName);
		assert(pMapName);
		if (!stricmp(pMapName,"Opacity"))
		{
			// use the diffuse map for now
			int mapRef;
			tupMaterialMap.GetMapReference(mapRef);
			float amount;
			TupperwareScalar *pAmountScalar = tupMaterialMap.GetAmount(amount);
			if (!pAmountScalar)
				amount = 1.0f;
			AddOpacityMap(mapRef,amount,alphaEntries);
		}
	}
	// scan for Diffuse Color
	for (mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMatMap = tupStandardMaterial.GetMap(mapIndex);
		TupStandardMaterialMap tupMaterialMap(pMatMap);

		char *pMapName;
		tupMaterialMap.GetMapName(&pMapName);
		assert(pMapName);
		if (!stricmp(pMapName,"Diffuse Color"))
		{
			// use the diffuse map for now
			int mapRef;
			tupMaterialMap.GetMapReference(mapRef);
			AddMap(mapRef,DIFFUSE_COLOR_MAP,alphaEntries,faceLayerArray);
		}
	}
	// scan for Reflection
	for (mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		TupperwareAggregate *pMatMap = tupStandardMaterial.GetMap(mapIndex);
		TupStandardMaterialMap tupMaterialMap(pMatMap);

		char *pMapName;
		tupMaterialMap.GetMapName(&pMapName);
		assert(pMapName);
		if (!stricmp(pMapName,"Reflection"))
		{
			// use the diffuse level map for now
			int mapRef;
			tupMaterialMap.GetMapReference(mapRef);
			AddMap(mapRef,REFLECTION_MAP,alphaEntries,faceLayerArray);
		}
	}
	// restore alpha entries size
	alphaEntries.SetSize(startAlphaEntriesSize);
}

void MaterialRemapManager::AddBlendMaterialMaps(TupperwareAggregate *pMaterialAgg,TupArray<ApplyAlphaEntry> &alphaEntries,TupArray<FaceLayerEntry> &faceLayerArray)
{
	// remember the length of this array to restore when finished
	int startAlphaEntriesSize = alphaEntries.GetSize();

	TupBlendMaterial tupBlendMaterial(pMaterialAgg);
	int material1Ref;
	TupperwareScalar *pMaterial1RefScalar = tupBlendMaterial.GetMaterial1Reference(material1Ref);
	if (!pMaterial1RefScalar)
		material1Ref = -1;

	int material2Ref;
	TupperwareScalar *pMaterial2RefScalar = tupBlendMaterial.GetMaterial2Reference(material2Ref);
	if (!pMaterial2RefScalar)
		material2Ref = -1;

	int maskRef;
	TupperwareScalar *pMaskRefScalar = tupBlendMaterial.GetMaskReference(maskRef);
	if (!pMaskRefScalar)
		maskRef = -1;

	int flags;
	tupBlendMaterial.GetFlags(flags);
	float mixAmount;
	tupBlendMaterial.GetMixAmount(mixAmount);
	if (material1Ref!=-1 && (flags & TupBlendMaterial::MATERIAL_1_ENABLE))
	{
		AddMaterialMaps(material1Ref,alphaEntries,faceLayerArray);
	}
	if (material2Ref!=-1 && (flags & TupBlendMaterial::MATERIAL_2_ENABLE))
	{
		if ((flags & TupBlendMaterial::MASK_ENABLE) && maskRef!=-1)
		{
			AddOpacityMap(maskRef,1.0f,alphaEntries);
		}
		else 
		{
			ApplyAlphaEntry newAlphaEntry;
			newAlphaEntry.m_constantAlpha = mixAmount;
			alphaEntries.Add(newAlphaEntry);
		}
		AddMaterialMaps(material2Ref,alphaEntries,faceLayerArray);
	}
	// restore alpha entries size
	alphaEntries.SetSize(startAlphaEntriesSize);
}


void MaterialRemapManager::AddCompositeMaterialMaps(TupperwareAggregate *pMaterialAgg,TupArray<ApplyAlphaEntry> &alphaEntries,TupArray<FaceLayerEntry> &faceLayerArray)
{
	TupCompositeMaterial tupCompositeMaterial(pMaterialAgg);
	int baseMaterialRef;
	tupCompositeMaterial.GetBaseMaterialReference(baseMaterialRef);
	if (baseMaterialRef!=-1)
	{
		AddMaterialMaps(baseMaterialRef,alphaEntries,faceLayerArray);
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
			float mixAmount;
			subMat.GetMixAmount(mixAmount);
			// do the alpha thing if the mix amount is <100.0
			if (mixAmount<100.0f)
			{
				// remember the length of this array to restore when finished
				int startAlphaEntriesSize = alphaEntries.GetSize();
				ApplyAlphaEntry newAlphaEntry;
				newAlphaEntry.m_constantAlpha = mixAmount;
				alphaEntries.Add(newAlphaEntry);
				AddMaterialMaps(subMaterialRef,alphaEntries,faceLayerArray);
				alphaEntries.SetSize(startAlphaEntriesSize);
			}
			else
			{
				AddMaterialMaps(subMaterialRef,alphaEntries,faceLayerArray);
			}
		}
	}
}

void MaterialRemapManager::AddDoubleSidedMaterialMaps(TupperwareAggregate *pMaterialAgg,TupArray<ApplyAlphaEntry> &alphaEntries,TupArray<FaceLayerEntry> &faceLayerArray)
{
	TupDoubleSidedMaterial tupDoubleSidedMaterial(pMaterialAgg);
	int frontMaterialRef;
	TupperwareScalar *pFrontMaterialRefScalar = tupDoubleSidedMaterial.GetFrontMaterialReference(frontMaterialRef);
	if (pFrontMaterialRefScalar && frontMaterialRef!=-1)
	{
		AddMaterialMaps(frontMaterialRef,alphaEntries,faceLayerArray);
	}
	// ignore the back material
}

void MaterialRemapManager::AddMap(int mapRef,MaterialRemapManager::MapType mapType,TupArray<ApplyAlphaEntry> &alphaEntries,TupArray<FaceLayerEntry> &faceLayerArray)
{
	TupperwareAggregate *pMapAgg = m_tupMapPool.GetMap(mapRef);
	TupMap tupMap(pMapAgg);
	switch(tupMap.GetMapType())
	{
		case TupMap::BITMAP_MAP:
			{
				AddBitmapMap(pMapAgg,mapRef,mapType,alphaEntries,faceLayerArray);
			}
			break;
		case TupMap::COMPOSITE_MAP:
			{
				AddCompositeMap(pMapAgg,mapType,alphaEntries,faceLayerArray);
			}
			break;
		case TupMap::MIX_MAP:
			{
				AddMixMap(pMapAgg,mapType,alphaEntries,faceLayerArray);
			}
			break;
	}
}

void MaterialRemapManager::AddBitmapMap(TupperwareAggregate *pMapAgg,int mapRef,MaterialRemapManager::MapType mapType,TupArray<ApplyAlphaEntry> &alphaEntries,TupArray<FaceLayerEntry> &faceLayerArray)
{
	bool bAddNewLayer = false;
	int layerArraySize = faceLayerArray.GetSize();
	// if there is any layers yet
	if (layerArraySize)
	{
		FaceLayerEntry &lastLayer = faceLayerArray[layerArraySize-1];
		// if the alpha entries match
		if (CompareAlphaEntryArrays(lastLayer.m_alphaEntries,alphaEntries))
		{
			switch (mapType)
			{
				case MaterialRemapManager::DIFFUSE_COLOR_MAP:
					{
						// check if any other ones that should be after me are in
						if (lastLayer.m_reflectionMapRefs.GetSize())
						{
							bAddNewLayer = true;
						}
					}
					break;
				case MaterialRemapManager::REFLECTION_MAP:
					break;
			}
		}
		else
		{
			bAddNewLayer = true;
		}
	}
	else
	{
		bAddNewLayer = true;
	}
	// create a new layer if needed
	if (bAddNewLayer)
	{
		FaceLayerEntry newLayer;
		newLayer.m_alphaEntries = alphaEntries;
		faceLayerArray.Add(newLayer);
		layerArraySize++;
	}
	// add map to approprate array
	switch (mapType)
	{
		case MaterialRemapManager::DIFFUSE_COLOR_MAP:
			{
				FaceLayerEntry &lastLayer = faceLayerArray[layerArraySize-1];
				lastLayer.m_diffuseColorMapRefs.Add(mapRef);
			}
			break;
		case MaterialRemapManager::REFLECTION_MAP:
			{
				FaceLayerEntry &lastLayer = faceLayerArray[layerArraySize-1];
				lastLayer.m_reflectionMapRefs.Add(mapRef);
			}
			break;
	}
}

void MaterialRemapManager::AddCompositeMap(TupperwareAggregate *pMapAgg,MaterialRemapManager::MapType mapType,TupArray<ApplyAlphaEntry> &alphaEntries,TupArray<FaceLayerEntry> &faceLayerArray)
{
	TupCompositeMap tupCompositeMap(pMapAgg);
	int numSubMaps = tupCompositeMap.GetNumSubMaps();
	int subMapIndex;
	for (subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
	{
		TupperwareAggregate *pSubMapAgg = tupCompositeMap.GetSubMap(subMapIndex);
		TupCompositeMapSubMap subMap(pSubMapAgg);
		int flags;
		subMap.GetFlags(flags);
		if (flags & TupCompositeMapSubMap::ENABLED)
		{
			int subMapRef;
			subMap.GetMapReference(subMapRef);
			assert(subMapRef!=-1);
			AddMap(subMapRef,mapType,alphaEntries,faceLayerArray);
		}
	}

}

void MaterialRemapManager::AddMixMap(TupperwareAggregate *pMapAgg,MaterialRemapManager::MapType mapType,TupArray<ApplyAlphaEntry> &alphaEntries,TupArray<FaceLayerEntry> &faceLayerArray)
{
	// remember the length of this array to restore when finished
	int startAlphaEntriesSize = alphaEntries.GetSize();

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

	int flags;
	tupMixMap.GetFlags(flags);
	float mixAmount;
	tupMixMap.GetMixAmount(mixAmount);
	if (map1Ref!=-1 && (flags & TupMixMap::MAP_1_ENABLE))
	{
		AddMap(map1Ref,mapType,alphaEntries,faceLayerArray);
	}
	if (map2Ref!=-1 && (flags & TupMixMap::MAP_2_ENABLE))
	{
		if ((flags & TupMixMap::MIX_MAP_ENABLE) && mixMapRef!=-1)
		{
			AddOpacityMap(mixMapRef,1.0f,alphaEntries);
		}
		else 
		{
			ApplyAlphaEntry newAlphaEntry;
			newAlphaEntry.m_constantAlpha = mixAmount;
			alphaEntries.Add(newAlphaEntry);
		}
		AddMap(map2Ref,mapType,alphaEntries,faceLayerArray);
	}
	// restore alpha entries size
	alphaEntries.SetSize(startAlphaEntriesSize);
}

void MaterialRemapManager::AddOpacityMap(int mapRef,float amount,TupArray<ApplyAlphaEntry> &alphaEntries)
{
	TupperwareAggregate *pMapAgg = m_tupMapPool.GetMap(mapRef);
	TupMap tupMap(pMapAgg);
	switch(tupMap.GetMapType())
	{
		case TupMap::BITMAP_MAP:
			{
				AddOpacityBitmapMap(pMapAgg,mapRef,amount,alphaEntries);
			}
			break;
		case TupMap::COMPOSITE_MAP:
			{
				AddOpacityCompositeMap(pMapAgg,mapRef,amount,alphaEntries);
			}
			break;
		case TupMap::MIX_MAP:
			{
				AddOpacityMixMap(pMapAgg,mapRef,amount,alphaEntries);
			}
			break;
	}
}

void MaterialRemapManager::AddOpacityBitmapMap(TupperwareAggregate *pMapAgg,int mapRef,float amount,TupArray<ApplyAlphaEntry> &alphaEntries)
{
	BuildUVTransformManager buildUVTransformManager(m_tupMaterialPool,m_tupMapPool,m_tupImagePool,true);

	ApplyAlphaEntry newApplyAlphaEntry;
	newApplyAlphaEntry.m_constantAlpha = amount;

	TupBitmapMap tupBitmapMap(pMapAgg);
	int imageRef;
	tupBitmapMap.GetImageReference(imageRef);

	if (imageRef!=-1)
	{
		buildUVTransformManager.BuildUVTransform(mapRef,newApplyAlphaEntry.m_uvTransform);
	}
	alphaEntries.Add(newApplyAlphaEntry);
}

void MaterialRemapManager::AddOpacityCompositeMap(TupperwareAggregate *pMapAgg,int mapRef,float amount,TupArray<ApplyAlphaEntry> &alphaEntries)
{
	TupCompositeMap tupCompositeMap(pMapAgg);
	int numSubMaps = tupCompositeMap.GetNumSubMaps();
	int subMapIndex;
	for (subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
	{
		TupperwareAggregate *pSubMapAgg = tupCompositeMap.GetSubMap(subMapIndex);
		TupCompositeMapSubMap subMap(pSubMapAgg);
		int flags;
		subMap.GetFlags(flags);
		if (flags & TupCompositeMapSubMap::ENABLED)
		{
			int subMapRef;
			subMap.GetMapReference(subMapRef);
			assert(subMapRef!=-1);
			AddOpacityMap(subMapRef,amount,alphaEntries);
		}
	}

}

void MaterialRemapManager::AddOpacityMixMap(TupperwareAggregate *pMapAgg,int mapRef,float amount,TupArray<ApplyAlphaEntry> &alphaEntries)
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

	int flags;
	tupMixMap.GetFlags(flags);
	float mixAmount;
	tupMixMap.GetMixAmount(mixAmount);
	if (map1Ref!=-1 && (flags & TupMixMap::MAP_1_ENABLE))
	{
		AddOpacityMap(map1Ref,amount,alphaEntries);
	}
	if (map2Ref!=-1 && (flags & TupMixMap::MAP_2_ENABLE))
	{
		if ((flags & TupMixMap::MIX_MAP_ENABLE) && mixMapRef!=-1)
		{
			AddOpacityMap(mixMapRef,amount,alphaEntries);
		}
		AddOpacityMap(mixMapRef,amount*mixAmount,alphaEntries);
	}
}

// return true if the arrays are the same
bool MaterialRemapManager::CompareAlphaEntryArrays(TupArray<ApplyAlphaEntry> &s1,TupArray<ApplyAlphaEntry> &s2)
{
	if (s1.GetSize()!=s2.GetSize())
		return false;
	for (int entryIndex=0;entryIndex<s1.GetSize();entryIndex++)
	{
		if (!CompareAlphaEntries(s1[entryIndex],s2[entryIndex]))
			return false;
	}
	return true;
}

bool MaterialRemapManager::CompareAlphaEntries(ApplyAlphaEntry &s1,ApplyAlphaEntry &s2)
{
	return (s1.m_constantAlpha == s2.m_constantAlpha &&
			s1.m_uvTransform == s2.m_uvTransform);
}
