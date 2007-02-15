////////////////////////////////////////////////////////////////////////////
//
// CompareTupObjects
//
// Class to deal with comparing tup objects
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: CompareTupObjects.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/20/03    Time: 4:11p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * moved map and material comparison to separate class
 * fixed memory leak
 */

#include "TupImpExpPch.h"
#include "CompareTupObjects.h"
#include "CRC.h"

unsigned long CompareTupObjects::GetMaterialCRC(TupperwareAggregate *pMaterialAgg)
{
	CRC CRCGenerator;
	char *pName;
	int intValue;
	float floatValue;
	float *pFloatList;

	TupMaterial tupMaterial(pMaterialAgg);

	switch(tupMaterial.GetMaterialType())
	{
		case TupMaterial::STANDARD_MATERIAL:
			{
				CRCGenerator.AnalyzeData((int)TupMaterial::STANDARD_MATERIAL);
				TupStandardMaterial tupStandardMaterial(pMaterialAgg);
//				tupStandardMaterial.GetName(&pName); CRCGenerator.AnalyzeData(pName);
				tupStandardMaterial.GetAmbientColor(&pFloatList); CRCGenerator.AnalyzeData(pFloatList,sizeof(float)*3);
				tupStandardMaterial.GetDiffuseColor(&pFloatList); CRCGenerator.AnalyzeData(pFloatList,sizeof(float)*3);
				tupStandardMaterial.GetSpecularColor(&pFloatList); CRCGenerator.AnalyzeData(pFloatList,sizeof(float)*3);
				tupStandardMaterial.GetFilterColor(&pFloatList); CRCGenerator.AnalyzeData(pFloatList,sizeof(float)*3);
				tupStandardMaterial.GetGlossiness(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetSpecularLevel(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetSpecularSoften(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetTransparency(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetWireSize(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetIndexOfRefraction(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetReflectionDimIntensity(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetReflectionDimMultiplier(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetOpacity(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetOpacityFalloff(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetTransparencyType(intValue); CRCGenerator.AnalyzeData(intValue);
				tupStandardMaterial.GetShadingTypeName(&pName); CRCGenerator.AnalyzeData(pName);
				tupStandardMaterial.GetSelfIllumination(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupStandardMaterial.GetFlags(intValue); CRCGenerator.AnalyzeData(intValue);
				int numSubMaps = tupStandardMaterial.GetNumMaps(); CRCGenerator.AnalyzeData(numSubMaps);
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupStandardMaterial.GetMap(subMapIndex);
					TupStandardMaterialMap tupSubMap(pSubMapAgg);
//					tupSubMap.GetMapName(&pName); CRCGenerator.AnalyzeData(pName);
					tupSubMap.GetMapReference(intValue); CRCGenerator.AnalyzeData(intValue);
					tupSubMap.GetAmount(floatValue); CRCGenerator.AnalyzeData(floatValue);
					tupSubMap.GetFlags(intValue); CRCGenerator.AnalyzeData(intValue);
				}
			}
			break;
		case TupMaterial::MULTI_SUB_OBJECT_MATERIAL:
			{
				CRCGenerator.AnalyzeData((int)TupMaterial::MULTI_SUB_OBJECT_MATERIAL);
				TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);
//				tupMultiSubObjectMaterial.GetName(&pName); CRCGenerator.AnalyzeData(pName);
				int numSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials(); CRCGenerator.AnalyzeData(numSubMaterials);
				int subMaterialIndex;
				for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
				{
					TupperwareAggregate *pSubObjectMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(subMaterialIndex);
					TupMultiSubObjectMaterialSubMaterial tupMultiSubObjectMaterialSubMaterial(pSubObjectMaterialAgg);
					tupMultiSubObjectMaterialSubMaterial.GetMaterialReference(intValue); CRCGenerator.AnalyzeData(intValue);
//					tupMultiSubObjectMaterialSubMaterial.GetSlotName(&pName); CRCGenerator.AnalyzeData(pName);
					tupMultiSubObjectMaterialSubMaterial.GetFlags(intValue); CRCGenerator.AnalyzeData(intValue);
				}
			}
			break;
		case TupMaterial::BLEND_MATERIAL:
			{
				CRCGenerator.AnalyzeData((int)TupMaterial::BLEND_MATERIAL);
				TupBlendMaterial tupBlendMaterial(pMaterialAgg);
//				tupBlendMaterial.GetName(&pName); CRCGenerator.AnalyzeData(pName);
				tupBlendMaterial.GetMaterial1Reference(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBlendMaterial.GetMaterial2Reference(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBlendMaterial.GetMaskReference(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBlendMaterial.GetFlags(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBlendMaterial.GetMixAmount(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBlendMaterial.GetUpperTransitionZone(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBlendMaterial.GetLowerTransitionZone(floatValue); CRCGenerator.AnalyzeData(floatValue);
			}
			break;
		case TupMaterial::COMPOSITE_MATERIAL:
			{
				CRCGenerator.AnalyzeData((int)TupMaterial::COMPOSITE_MATERIAL);
				TupCompositeMaterial tupCompositeMaterial(pMaterialAgg);
//				tupCompositeMaterial.GetName(&pName); CRCGenerator.AnalyzeData(pName);
				tupCompositeMaterial.GetBaseMaterialReference(intValue); CRCGenerator.AnalyzeData(intValue);
				int numSubMaterials = tupCompositeMaterial.GetNumSubMaterials(); CRCGenerator.AnalyzeData(numSubMaterials);
				int subMaterialIndex;
				for (subMaterialIndex=0;subMaterialIndex<numSubMaterials;subMaterialIndex++)
				{
					TupperwareAggregate *pSubMaterialAgg = tupCompositeMaterial.GetSubMaterial(subMaterialIndex);
					TupCompositeMaterialSubMaterial subMat(pSubMaterialAgg);
					subMat.GetFlags(intValue); CRCGenerator.AnalyzeData(intValue);
					subMat.GetMaterialReference(intValue); CRCGenerator.AnalyzeData(intValue);
					subMat.GetMixAmount(floatValue); CRCGenerator.AnalyzeData(floatValue);
				}
			}
			break;
		case TupMaterial::DOUBLE_SIDED_MATERIAL:
			{
				CRCGenerator.AnalyzeData((int)TupMaterial::DOUBLE_SIDED_MATERIAL);
				TupDoubleSidedMaterial tupDoubleSidedMaterial(pMaterialAgg);
//				tupDoubleSidedMaterial.GetName(&pName); CRCGenerator.AnalyzeData(pName);
				tupDoubleSidedMaterial.GetFrontMaterialReference(intValue);	CRCGenerator.AnalyzeData(intValue);
				tupDoubleSidedMaterial.GetBackMaterialReference(intValue); CRCGenerator.AnalyzeData(intValue);
				tupDoubleSidedMaterial.GetFlags(intValue); CRCGenerator.AnalyzeData(intValue);
				tupDoubleSidedMaterial.GetTranslucency(floatValue); CRCGenerator.AnalyzeData(floatValue);
			}
			break;
	}
	return CRCGenerator.GetResult();
}

bool CompareTupObjects::CompareFloats(float *pFloat1,float *pFloat2,int total)
{
	for (int index=0;index<total;index++)
	{
		if (pFloat1[index]!=pFloat2[index])
			return false;
	}
	return true;
}

bool CompareTupObjects::CompareMaterials(TupperwareAggregate *pMaterial1Agg,TupperwareAggregate *pMaterial2Agg)
{
	char *pName1,*pName2;
	int intValue1,intValue2;
	float floatValue1,floatValue2;
	float *pFloatList1,*pFloatList2;

	TupMaterial tupMaterial1(pMaterial1Agg);
	TupMaterial tupMaterial2(pMaterial1Agg);
	if (tupMaterial1.GetMaterialType()!=tupMaterial2.GetMaterialType()) return false;
	switch(tupMaterial1.GetMaterialType())
	{
		case TupMaterial::STANDARD_MATERIAL:
			{
				TupStandardMaterial tupStandardMaterial1(pMaterial1Agg);
				TupStandardMaterial tupStandardMaterial2(pMaterial2Agg);

//				tupStandardMaterial1.GetName(&pName1); tupStandardMaterial2.GetName(&pName2); if (strcmp(pName1,pName2)) return false;
				tupStandardMaterial1.GetAmbientColor(&pFloatList1); tupStandardMaterial2.GetAmbientColor(&pFloatList2);	if (!CompareFloats(pFloatList1,pFloatList2,3)) return false;
				tupStandardMaterial1.GetDiffuseColor(&pFloatList1); tupStandardMaterial2.GetDiffuseColor(&pFloatList2); if (!CompareFloats(pFloatList1,pFloatList2,3)) return false;
				tupStandardMaterial1.GetSpecularColor(&pFloatList1); tupStandardMaterial2.GetSpecularColor(&pFloatList2); if (!CompareFloats(pFloatList1,pFloatList2,3)) return false;
				tupStandardMaterial1.GetFilterColor(&pFloatList1); tupStandardMaterial2.GetFilterColor(&pFloatList2); if (!CompareFloats(pFloatList1,pFloatList2,3)) return false;
				tupStandardMaterial1.GetGlossiness(floatValue1); tupStandardMaterial2.GetGlossiness(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetSpecularLevel(floatValue1); tupStandardMaterial2.GetSpecularLevel(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetSpecularSoften(floatValue1); tupStandardMaterial2.GetSpecularSoften(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetTransparency(floatValue1); tupStandardMaterial2.GetTransparency(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetWireSize(floatValue1); tupStandardMaterial2.GetWireSize(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetIndexOfRefraction(floatValue1); tupStandardMaterial2.GetIndexOfRefraction(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetReflectionDimIntensity(floatValue1); tupStandardMaterial2.GetReflectionDimIntensity(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetReflectionDimMultiplier(floatValue1); tupStandardMaterial2.GetReflectionDimMultiplier(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetOpacity(floatValue1); tupStandardMaterial2.GetOpacity(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetOpacityFalloff(floatValue1); tupStandardMaterial2.GetOpacityFalloff(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetTransparencyType(intValue1); tupStandardMaterial2.GetTransparencyType(intValue2); if (intValue1!=intValue2) return false;
				tupStandardMaterial1.GetShadingTypeName(&pName1); tupStandardMaterial2.GetShadingTypeName(&pName2); if (strcmp(pName1,pName2)) return false;
				tupStandardMaterial1.GetSelfIllumination(floatValue1); tupStandardMaterial2.GetSelfIllumination(floatValue2); if (floatValue1!=floatValue2) return false;
				tupStandardMaterial1.GetFlags(intValue1); tupStandardMaterial2.GetFlags(intValue2); if (intValue1!=intValue2) return false;
				int numSubMaps1 = tupStandardMaterial1.GetNumMaps();
				int numSubMaps2 = tupStandardMaterial2.GetNumMaps();
				if (numSubMaps1!=numSubMaps2) return false;
				for (int subMapIndex=0;subMapIndex<numSubMaps1;subMapIndex++)
				{
					TupperwareAggregate *pSubMap1Agg = tupStandardMaterial1.GetMap(subMapIndex);
					TupperwareAggregate *pSubMap2Agg = tupStandardMaterial1.GetMap(subMapIndex);
					TupStandardMaterialMap tupSubMap1(pSubMap1Agg);
					TupStandardMaterialMap tupSubMap2(pSubMap2Agg);
//					tupSubMap1.GetMapName(&pName1); tupSubMap2.GetMapName(&pName2); if (strcmp(pName1,pName2)) return false;
					tupSubMap1.GetMapReference(intValue1); tupSubMap2.GetMapReference(intValue2); if (intValue1!=intValue2) return false;
					tupSubMap1.GetAmount(floatValue1); tupSubMap2.GetAmount(floatValue2); if (floatValue1!=floatValue2) return false;
					tupSubMap1.GetFlags(intValue1); tupSubMap2.GetFlags(intValue2); if (intValue1!=intValue2) return false;
				}
			}
			break;
		case TupMaterial::MULTI_SUB_OBJECT_MATERIAL:
			{
				TupMultiSubObjectMaterial tupMultiSubObjectMaterial1(pMaterial1Agg);
				TupMultiSubObjectMaterial tupMultiSubObjectMaterial2(pMaterial2Agg);
//				tupMultiSubObjectMaterial1.GetName(&pName1); tupMultiSubObjectMaterial2.GetName(&pName2); if (strcmp(pName1,pName2)) return false;
				int numSubMaterials1 = tupMultiSubObjectMaterial1.GetNumSubMaterials();
				int numSubMaterials2 = tupMultiSubObjectMaterial2.GetNumSubMaterials();
				int subMaterialIndex;
				if (numSubMaterials1!=numSubMaterials1) return false;
				for (subMaterialIndex=0;subMaterialIndex<numSubMaterials1;subMaterialIndex++)
				{
					TupperwareAggregate *pSubObjectMaterial1Agg = tupMultiSubObjectMaterial1.GetSubMaterial(subMaterialIndex);
					TupperwareAggregate *pSubObjectMaterial2Agg = tupMultiSubObjectMaterial2.GetSubMaterial(subMaterialIndex);
					TupMultiSubObjectMaterialSubMaterial tupMultiSubObjectMaterialSubMaterial1(pSubObjectMaterial1Agg);
					TupMultiSubObjectMaterialSubMaterial tupMultiSubObjectMaterialSubMaterial2(pSubObjectMaterial2Agg);
					tupMultiSubObjectMaterialSubMaterial1.GetMaterialReference(intValue1); tupMultiSubObjectMaterialSubMaterial2.GetMaterialReference(intValue2); if (intValue1!=intValue2) return false;
//					tupMultiSubObjectMaterialSubMaterial1.GetSlotName(&pName1);	tupMultiSubObjectMaterialSubMaterial2.GetSlotName(&pName2); if (strcmp(pName1,pName2)) return false;
					tupMultiSubObjectMaterialSubMaterial1.GetFlags(intValue1); tupMultiSubObjectMaterialSubMaterial2.GetFlags(intValue2); if (intValue1!=intValue2) return false;
				}
			}
			break;
		case TupMaterial::BLEND_MATERIAL:
			{
				TupBlendMaterial tupBlendMaterial1(pMaterial1Agg);
				TupBlendMaterial tupBlendMaterial2(pMaterial2Agg);
//				tupBlendMaterial1.GetName(&pName1); tupBlendMaterial2.GetName(&pName2); if (strcmp(pName1,pName2)) return false;
				tupBlendMaterial1.GetMaterial1Reference(intValue1); tupBlendMaterial2.GetMaterial1Reference(intValue2); if (intValue1!=intValue2) return false;
				tupBlendMaterial1.GetMaterial2Reference(intValue1); tupBlendMaterial2.GetMaterial2Reference(intValue2); if (intValue1!=intValue2) return false;
				tupBlendMaterial1.GetMaskReference(intValue1); tupBlendMaterial2.GetMaskReference(intValue2); if (intValue1!=intValue2) return false;
				tupBlendMaterial1.GetFlags(intValue1); tupBlendMaterial2.GetFlags(intValue2); if (intValue1!=intValue2) return false;
				tupBlendMaterial1.GetMixAmount(floatValue1); tupBlendMaterial2.GetMixAmount(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBlendMaterial1.GetUpperTransitionZone(floatValue1); tupBlendMaterial2.GetUpperTransitionZone(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBlendMaterial1.GetLowerTransitionZone(floatValue1); tupBlendMaterial2.GetLowerTransitionZone(floatValue2); if (floatValue1!=floatValue2) return false;
			}
			break;
		case TupMaterial::COMPOSITE_MATERIAL:
			{
				TupCompositeMaterial tupCompositeMaterial1(pMaterial1Agg);
				TupCompositeMaterial tupCompositeMaterial2(pMaterial2Agg);
//				tupCompositeMaterial1.GetName(&pName1); tupCompositeMaterial2.GetName(&pName2); if (strcmp(pName1,pName2)) return false;
				tupCompositeMaterial1.GetBaseMaterialReference(intValue1); tupCompositeMaterial2.GetBaseMaterialReference(intValue2); if (intValue1!=intValue2) return false;
				int numSubMaterials1 = tupCompositeMaterial1.GetNumSubMaterials();
				int numSubMaterials2 = tupCompositeMaterial2.GetNumSubMaterials();
				if (numSubMaterials1!=numSubMaterials2)	return false;
				int subMaterialIndex;
				for (subMaterialIndex=0;subMaterialIndex<numSubMaterials1;subMaterialIndex++)
				{
					TupperwareAggregate *pSubMaterial1Agg = tupCompositeMaterial1.GetSubMaterial(subMaterialIndex);
					TupperwareAggregate *pSubMaterial2Agg = tupCompositeMaterial2.GetSubMaterial(subMaterialIndex);
					TupCompositeMaterialSubMaterial subMat1(pSubMaterial1Agg);
					TupCompositeMaterialSubMaterial subMat2(pSubMaterial2Agg);
					subMat1.GetFlags(intValue1); subMat2.GetFlags(intValue2); if (intValue1!=intValue2) return false;
					subMat1.GetMaterialReference(intValue1); subMat2.GetMaterialReference(intValue2); if (intValue1!=intValue2) return false;
					subMat1.GetMixAmount(floatValue1); subMat2.GetMixAmount(floatValue2); if (floatValue1!=floatValue2) return false;
				}
			}
			break;
		case TupMaterial::DOUBLE_SIDED_MATERIAL:
			{
				TupDoubleSidedMaterial tupDoubleSidedMaterial1(pMaterial1Agg);
				TupDoubleSidedMaterial tupDoubleSidedMaterial2(pMaterial2Agg);
//				tupDoubleSidedMaterial1.GetName(&pName1); tupDoubleSidedMaterial2.GetName(&pName2); if (strcmp(pName1,pName2)) return false;
				tupDoubleSidedMaterial1.GetFrontMaterialReference(intValue1); tupDoubleSidedMaterial2.GetFrontMaterialReference(intValue2); if (intValue1!=intValue2) return false;
				tupDoubleSidedMaterial1.GetBackMaterialReference(intValue1); tupDoubleSidedMaterial2.GetBackMaterialReference(intValue2); if (intValue1!=intValue2) return false;
				tupDoubleSidedMaterial1.GetFlags(intValue1); tupDoubleSidedMaterial2.GetFlags(intValue2); if (intValue1!=intValue2) return false;
				tupDoubleSidedMaterial1.GetTranslucency(floatValue1); tupDoubleSidedMaterial2.GetTranslucency(floatValue2); if (floatValue1!=floatValue2) return false;
			}
			break;
	}
	return true;
}

unsigned long CompareTupObjects::GetMapCRC(TupperwareAggregate *pMapAgg)
{
	CRC CRCGenerator;
	//	char *pName;
	int intValue;
	float floatValue;
	
	TupMap tupMap(pMapAgg);
	switch(tupMap.GetMapType())
	{
		case TupMap::BITMAP_MAP:
			{
				CRCGenerator.AnalyzeData((int)TupMap::BITMAP_MAP);
				TupBitmapMap tupBitmapMap(pMapAgg);
//				tupBitmapMap.GetName(&pName); CRCGenerator.AnalyzeData(pName);
				tupBitmapMap.GetImageReference(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBitmapMap.GetMapping(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBitmapMap.GetMapChannel(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBitmapMap.GetUOffset(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetVOffset(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetUTiling(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetVTiling(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetMapAxis(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBitmapMap.GetUAngle(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetVAngle(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetWAngle(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetBlur(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetBlurOffset(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetNoiseAmount(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetNoiseLevels(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBitmapMap.GetNoiseSize(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetCroppingPlacementU(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetCroppingPlacementV(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetCroppingPlacementW(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetCroppingPlacementH(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetPlacementJitter(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetStartFrame(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBitmapMap.GetPlaybackRate(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetOutputAmount(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetOutputRGBOffset(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetOutputRGBLevel(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetOutputBumpAmount(floatValue); CRCGenerator.AnalyzeData(floatValue);
				tupBitmapMap.GetBlendMode(intValue); CRCGenerator.AnalyzeData(intValue);
				tupBitmapMap.GetFlags(intValue); CRCGenerator.AnalyzeData(intValue);
				char *pUserProperties;
				tupBitmapMap.GetUserProperties(&pUserProperties); CRCGenerator.AnalyzeData(pUserProperties);
			}
			break;
		case TupMap::COMPOSITE_MAP:
			{
				CRCGenerator.AnalyzeData((int)TupMap::COMPOSITE_MAP);
				TupCompositeMap tupCompositeMap(pMapAgg);
//				tupCompositeMap.GetName(&pName); CRCGenerator.AnalyzeData(pName);
				int numSubMaps = tupCompositeMap.GetNumSubMaps(); CRCGenerator.AnalyzeData(numSubMaps);
				for (int subMapIndex=0;subMapIndex<numSubMaps;subMapIndex++)
				{
					TupperwareAggregate *pSubMapAgg = tupCompositeMap.GetSubMap(subMapIndex);
					TupCompositeMapSubMap tupSubMap(pSubMapAgg);
					tupSubMap.GetMapReference(intValue); CRCGenerator.AnalyzeData(intValue);
					tupSubMap.GetFlags(intValue); CRCGenerator.AnalyzeData(intValue);
				}
			}
			break;
		case TupMap::MIX_MAP:
			{
				CRCGenerator.AnalyzeData((int)TupMap::MIX_MAP);
				TupMixMap tupMixMap(pMapAgg);
//				tupMixMap.GetName(&pName); CRCGenerator.AnalyzeData(pName);
				tupMixMap.GetMap1Reference(intValue); CRCGenerator.AnalyzeData(intValue);
				tupMixMap.GetMap2Reference(intValue); CRCGenerator.AnalyzeData(intValue);
				tupMixMap.GetMixMapReference(intValue); CRCGenerator.AnalyzeData(intValue);
				tupMixMap.GetFlags(intValue); CRCGenerator.AnalyzeData(intValue);
				tupMixMap.GetMixAmount(floatValue);	CRCGenerator.AnalyzeData(floatValue);
				
				tupMixMap.GetLowerTransitionZone(floatValue);	CRCGenerator.AnalyzeData(floatValue);
				tupMixMap.GetLowerTransitionZone(floatValue);	CRCGenerator.AnalyzeData(floatValue);
				tupMixMap.GetOutputAmount(floatValue);	CRCGenerator.AnalyzeData(floatValue);
				tupMixMap.GetOutputRGBOffset(floatValue);	CRCGenerator.AnalyzeData(floatValue);
				tupMixMap.GetOutputRGBLevel(floatValue);	CRCGenerator.AnalyzeData(floatValue);
				tupMixMap.GetOutputBumpAmount(floatValue);	CRCGenerator.AnalyzeData(floatValue);
			}
			break;
	}
	return CRCGenerator.GetResult();
}


bool CompareTupObjects::CompareMaps(TupperwareAggregate *pMap1Agg,TupperwareAggregate *pMap2Agg)
{
	//	char *pName1,*pName2;
	int intValue1,intValue2;
	float floatValue1,floatValue2;
	
	TupMap tupMap1(pMap1Agg);
	TupMap tupMap2(pMap2Agg);
	if (tupMap1.GetMapType()!=tupMap2.GetMapType()) return false;
	switch(tupMap1.GetMapType())
	{
		case TupMap::BITMAP_MAP:
			{
				TupBitmapMap tupBitmapMap1(pMap1Agg);
				TupBitmapMap tupBitmapMap2(pMap2Agg);
//				tupBitmapMap1.GetName(&pName1); tupBitmapMap2.GetName(&pName2); if (strcmp(pName1,pName2)) return false;
				tupBitmapMap1.GetImageReference(intValue1); tupBitmapMap2.GetImageReference(intValue2); if (intValue1!=intValue2) return false;
				tupBitmapMap1.GetMapping(intValue1); tupBitmapMap2.GetMapping(intValue2); if (intValue1!=intValue2) return false;
				tupBitmapMap1.GetMapChannel(intValue1); tupBitmapMap2.GetMapChannel(intValue2); if (intValue1!=intValue2) return false;
				tupBitmapMap1.GetUOffset(floatValue1); tupBitmapMap2.GetUOffset(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetVOffset(floatValue1); tupBitmapMap2.GetVOffset(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetUTiling(floatValue1); tupBitmapMap2.GetUTiling(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetVTiling(floatValue1); tupBitmapMap2.GetVTiling(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetMapAxis(intValue1); tupBitmapMap2.GetMapAxis(intValue2); if (intValue1!=intValue2) return false;
				tupBitmapMap1.GetUAngle(floatValue1); tupBitmapMap2.GetUAngle(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetVAngle(floatValue1); tupBitmapMap2.GetVAngle(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetWAngle(floatValue1); tupBitmapMap2.GetWAngle(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetBlur(floatValue1); tupBitmapMap2.GetBlur(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetBlurOffset(floatValue1); tupBitmapMap2.GetBlurOffset(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetNoiseAmount(floatValue1); tupBitmapMap2.GetNoiseAmount(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetNoiseLevels(intValue1); tupBitmapMap2.GetNoiseLevels(intValue2); if (intValue1!=intValue2) return false;
				tupBitmapMap1.GetNoiseSize(floatValue1); tupBitmapMap2.GetNoiseSize(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetCroppingPlacementU(floatValue1); tupBitmapMap2.GetCroppingPlacementU(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetCroppingPlacementV(floatValue1); tupBitmapMap2.GetCroppingPlacementV(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetCroppingPlacementW(floatValue1); tupBitmapMap2.GetCroppingPlacementW(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetCroppingPlacementH(floatValue1); tupBitmapMap2.GetCroppingPlacementH(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetPlacementJitter(floatValue1); tupBitmapMap2.GetPlacementJitter(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetStartFrame(intValue1); tupBitmapMap2.GetStartFrame(intValue2); if (intValue1!=intValue2) return false;
				tupBitmapMap1.GetPlaybackRate(floatValue1); tupBitmapMap2.GetPlaybackRate(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetOutputAmount(floatValue1); tupBitmapMap2.GetOutputAmount(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetOutputRGBOffset(floatValue1); tupBitmapMap2.GetOutputRGBOffset(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetOutputRGBLevel(floatValue1); tupBitmapMap2.GetOutputRGBLevel(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetOutputBumpAmount(floatValue1); tupBitmapMap2.GetOutputBumpAmount(floatValue2); if (floatValue1!=floatValue2) return false;
				tupBitmapMap1.GetBlendMode(intValue1); tupBitmapMap2.GetBlendMode(intValue2); if (intValue1!=intValue2) return false;
				tupBitmapMap1.GetFlags(intValue1); tupBitmapMap2.GetFlags(intValue2); if (intValue1!=intValue2) return false;
				char *pUserProperties1,*pUserProperties2;
				tupBitmapMap1.GetUserProperties(&pUserProperties1); tupBitmapMap2.GetUserProperties(&pUserProperties2); 
				// return false if they are different (taking into account NULL string pointers)
				if (pUserProperties1)
				{
					if (pUserProperties2)
					{
						if (strcmp(pUserProperties1,pUserProperties2))
							return false;
					}
					else
					{
						return false;
					}
				}
				else
				{
					if (pUserProperties2)
					{
						return false;
					}
				}
			}
			break;
		case TupMap::COMPOSITE_MAP:
			{
				TupCompositeMap tupCompositeMap1(pMap1Agg);
				TupCompositeMap tupCompositeMap2(pMap2Agg);
//				tupCompositeMap1.GetName(&pName1); tupCompositeMap2.GetName(&pName2); if (strcmp(pName1,pName2)) return false;
				int numSubMaps1 = tupCompositeMap1.GetNumSubMaps();
				int numSubMaps2 = tupCompositeMap2.GetNumSubMaps();
				if (numSubMaps1!=numSubMaps2) return false;
				for (int subMapIndex=0;subMapIndex<numSubMaps1;subMapIndex++)
				{
					TupperwareAggregate *pSubMap1Agg = tupCompositeMap1.GetSubMap(subMapIndex);
					TupperwareAggregate *pSubMap2Agg = tupCompositeMap2.GetSubMap(subMapIndex);
					TupCompositeMapSubMap tupSubMap1(pSubMap1Agg);
					TupCompositeMapSubMap tupSubMap2(pSubMap2Agg);
					tupSubMap1.GetMapReference(intValue1); tupSubMap2.GetMapReference(intValue2); if (intValue1!=intValue2) return false;
					tupSubMap1.GetFlags(intValue1); tupSubMap2.GetFlags(intValue2); if (intValue1!=intValue2) return false;
				}
			}
			break;
		case TupMap::MIX_MAP:
			{
				TupMixMap tupMixMap1(pMap1Agg);
				TupMixMap tupMixMap2(pMap2Agg);
//				tupMixMap1.GetName(&pName1); tupMixMap1.GetName(&pName2); if (strcmp(pName1,pName2)) return false;
				tupMixMap1.GetMap1Reference(intValue1); tupMixMap2.GetMap1Reference(intValue2); if (intValue1!=intValue2) return false;
				tupMixMap1.GetMap2Reference(intValue1); tupMixMap2.GetMap2Reference(intValue2); if (intValue1!=intValue2) return false;
				tupMixMap1.GetMixMapReference(intValue1); tupMixMap2.GetMixMapReference(intValue2); if (intValue1!=intValue2) return false;
				tupMixMap1.GetFlags(intValue1); tupMixMap2.GetFlags(intValue2); if (intValue1!=intValue2) return false;
				tupMixMap1.GetMixAmount(floatValue1); tupMixMap2.GetMixAmount(floatValue2); if (floatValue1!=floatValue2) return false;
				tupMixMap1.GetLowerTransitionZone(floatValue1); tupMixMap2.GetLowerTransitionZone(floatValue2); if (floatValue1!=floatValue2) return false;
				tupMixMap1.GetLowerTransitionZone(floatValue1); tupMixMap2.GetLowerTransitionZone(floatValue2); if (floatValue1!=floatValue2) return false;
				tupMixMap1.GetOutputAmount(floatValue1); tupMixMap2.GetOutputAmount(floatValue2); if (floatValue1!=floatValue2) return false;
				tupMixMap1.GetOutputRGBOffset(floatValue1); tupMixMap2.GetOutputRGBOffset(floatValue2); if (floatValue1!=floatValue2) return false;
				tupMixMap1.GetOutputRGBLevel(floatValue1); tupMixMap2.GetOutputRGBLevel(floatValue2); if (floatValue1!=floatValue2) return false;
				tupMixMap1.GetOutputBumpAmount(floatValue1); tupMixMap2.GetOutputBumpAmount(floatValue2); if (floatValue1!=floatValue2) return false;
			}
			break;
	}
	return true;
}
