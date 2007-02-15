////////////////////////////////////////////////////////////////////////////
//
// ImportMaterial
//
// Class to handle the materials
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImportMaterial.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/24/02   Time: 4:29p
 * Updated in $/Avalanche/tools/Plugins/Max/TupImport
 * materials & maps
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 10/23/02   Time: 9:54a
 * Created in $/Avalanche/tools/Plugins/Max/TupImport
 */

#include "TupImportPch.h"
#include "TupImport.h"

static Class_ID compositematClassID(0x61dc0cd7, 0x13640af6);

Mtl *TupImport::AddMaterial(int materialRef)
{
	// if null reference
	if (materialRef==-1)
		return NULL;
	// if already created
	if ((*m_pMaterialPoolPtrs)[materialRef])
		return (*m_pMaterialPoolPtrs)[materialRef];

	Mtl *pMaterial = NULL;

	TupperwareAggregate *pMaterialAgg = m_pMaterialPool->GetMaterial(materialRef);
	assert(pMaterialAgg);
	TupMaterial tupMaterial(pMaterialAgg);
	switch (tupMaterial.GetMaterialType())
	{
		case TupMaterial::STANDARD_MATERIAL:
			pMaterial = AddStandardMaterial(pMaterialAgg);
			break;
		case TupMaterial::MULTI_SUB_OBJECT_MATERIAL:
			pMaterial = AddMultiSubObjectMaterial(pMaterialAgg);
			break;
		case TupMaterial::BLEND_MATERIAL:
			pMaterial = AddBlendMaterial(pMaterialAgg);
			break;
		case TupMaterial::COMPOSITE_MATERIAL:
			pMaterial = AddCompositeMaterial(pMaterialAgg);
			break;
		case TupMaterial::DOUBLE_SIDED_MATERIAL:
			pMaterial = AddDoubleSidedMaterial(pMaterialAgg);
			break;
	}
	(*m_pMaterialPoolPtrs)[materialRef] = pMaterial;
	return pMaterial;
}

// std2_extended param IDs
enum StandardMaterial2ExtendedParms
{ 
	std2_opacity_type, std2_opacity, std2_filter_color, std2_ep_filter_map,
	std2_falloff_type, std2_falloff_amnt, 
	std2_ior,
	std2_wire_size, std2_wire_units,
	std2_apply_refl_dimming, std2_dim_lvl, std2_refl_lvl,
};

#define NEWSHADERS_CLASS_ID		0x2857f420
#define WARDSHADER_CLASS_ID		0x2857f440
#define STRAUSS_SHADER_CLASS_ID		0x2857f450
#define ANISOSHADER_CLASS_ID		0x2857f460
#define MULTILAYERSHADER_CLASS_ID		0x2857f470
#define TRANSLUCENT_SHADER_CLASS_ID		0x2857f480
#define OREN_NAYAR_BLINN_SHADER_CLASS_ID		0x2857f421


enum validShaders
{
	SHADER_ANISOTROPIC,
	SHADER_BLINN,
	SHADER_METAL,
	SHADER_MULTI_LAYER,
	SHADER_OREN_NAYAR_BLINN,
	SHADER_PHONG,
	SHADER_STRAUSS,
	SHADER_TRANSLUCENT,
	SHADER_WARD,
	SHADERS_TOTAL // must be last
};

static Class_ID ShaderClassIDArray[SHADERS_TOTAL] =
{
	Class_ID(ANISOSHADER_CLASS_ID,0), // SHADER_ANISOTROPIC
	Class_ID(BLINNClassID,0), // SHADER_BLINN
	Class_ID(METALClassID,0), // SHADER_METAL
	Class_ID(MULTILAYERSHADER_CLASS_ID,0), // SHADER_MULTI_LAYER
	Class_ID(OREN_NAYAR_BLINN_SHADER_CLASS_ID,0), // SHADER_OREN_NAYAR_BLINN
	Class_ID(PHONGClassID,0), // SHADER_PHONG
	Class_ID(STRAUSS_SHADER_CLASS_ID,0), // SHADER_STRAUSS
	Class_ID(TRANSLUCENT_SHADER_CLASS_ID,0), // SHADER_TRANSLUCENT
	Class_ID(WARDSHADER_CLASS_ID,0), // SHADER_WARD

};

static char *AnisotropicMapNames[] =
{
	"Ambient Color",
	"Diffuse Color",
	"Specular Color",
	"Diffuse Level",
	"Specular Level",
	"Glossiness",
	"Anisotropy",
	"Orientation",
	"Self-Illumination",
	"Opacity",
	"Filter Color",
	"Bump",
	"Reflection",
	"Refraction",
	"Displacement",
	NULL
};

static char *BlinnMapNames[] =
{
	"Ambient Color",
	"Diffuse Color",
	"Specular Color",
	"Specular Level",
	"Glossiness",
	"Self-Illumination",
	"Opacity",
	"Filter Color",
	"Bump",
	"Reflection",
	"Refraction",
	"Displacement",
	NULL
};

static char *MetalMapNames[] =
{
	"Ambient Color",
	"Diffuse Color",
	"Specular Color",
	"Specular Level",
	"Glossiness",
	"Self-Illumination",
	"Opacity",
	"Filter Color",
	"Bump",
	"Reflection",
	"Refraction",
	"Displacement",
	NULL
};

static char *MultiLayerMapNames[] =
{
	"Ambient Color",
	"Diffuse Color",
	"Specular Color",
	"Diffuse Level",
	"Diff. Roughness",
	"Specular Color 1",
	"Specular Level 1",
	"Glossiness 1",
	"Anisotropy 1",
	"Orientation 1",
	"Specular Color 2",
	"Specular Level 2",
	"Glossiness 2",
	"Anisotropy 2",
	"Orientation 2",
	"Self-Illumination",
	"Opacity",
	"Filter Color",
	"Bump",
	"Reflection",
	"Refraction",
	"Displacement",
	NULL
};

static char *OrenNayarBlinnMapNames[] = 
{
	"Ambient Color",
	"Diffuse Color",
	"Specular Color",
	"Specular Level",
	"Self-Illumination",
	"Opacity",
	"Filter Color",
	"Diffuse Level",
	"Diff. Roughness",
	"Bump",
	"Reflection",
	"Refraction",
	"Displacement",
	NULL
};

static char *PhongMapNames[] = 
{
	"Ambient Color",
	"Diffuse Color",
	"Specular Color",
	"Specular Level",
	"Glossiness",
	"Self-Illumination",
	"Opacity",
	"Filter Color",
	"Diffuse Level",
	"Diff. Roughness",
	"Bump",
	"Reflection",
	"Refraction",
	"Displacement",
	NULL
};

static char *StraussMapNames[] = 
{
	"Color",
	"Glossiness",
	"Metalness",
	"Opacity",
	"Refr. Filter",
	"Bump",
	"Reflection",
	"Refraction",
	"Displacement",
	NULL
};

static char *TransparentMapNames[] =
{
	"Ambient Color",
	"Diffuse Color",
	"Specular Color",
	"Glossiness",
	"Specular Level",
	"Self-Illumination",
	"Opacity",
	"Filter Color",
	"Diffuse Level",
	"Translucent Color",
	"Bump",
	"Reflection",
	"Refraction",
	"Displacement",
	NULL
};

static char *WardMapNames[] =
{
	"Ambient Color",
	"Diffuse Color",
	"Specular Color",
	"Diffuse Level",
	"Specular Level",
	"Glossiness X",
	"Glossiness Y",
	"Opacity",
	"Filter Color",
	"Translucent Color",
	"Reflection",
	"Refraction",
	NULL
};


static char *shaderNamesArray[SHADERS_TOTAL] = 
{
	"Anisotropic",			 // SHADER_ANISOTROPIC
	"Blinn",				 // SHADER_BLINN
	"Metal",				 // SHADER_METAL
	"Multi-Layer",			 // SHADER_MULTI_LAYER
	"Oren-Nayar-Blinn",		 // SHADER_OREN_NAYAR_BLINN
	"Phong",				 // SHADER_PHONG
	"Strauss",				 // SHADER_STRAUSS
	"Translucent Shader",	 // SHADER_TRANSLUCENT
	"Ward Anisotropic",		 // SHADER_WARD
};

static char **ShaderMapNamesArray[SHADERS_TOTAL] = 
{
	AnisotropicMapNames,	 // SHADER_ANISOTROPIC
	BlinnMapNames,			 // SHADER_BLINN
	MetalMapNames,			 // SHADER_METAL
	MultiLayerMapNames,		 // SHADER_MULTI_LAYER
	OrenNayarBlinnMapNames,	 // SHADER_OREN_NAYAR_BLINN
	PhongMapNames,			 // SHADER_PHONG
	StraussMapNames,		 // SHADER_STRAUSS
	TransparentMapNames,	 // SHADER_TRANSLUCENT
	WardMapNames,			 // SHADER_WARD
};

StdMat2 *TupImport::AddStandardMaterial(TupperwareAggregate *pMaterialAgg)
{
	StdMat2 *pMaterial = (StdMat2*)CreateInstance(MATERIAL_CLASS_ID, Class_ID(DMTL_CLASS_ID,0)); 

	IParamBlock2 *pb_extended = pMaterial->GetParamBlock(1); // get extended

	TupStandardMaterial tupStandardMaterial(pMaterialAgg);

	char *pString;
	int intValue;
	float floatValue;
	float *pFloatList;
	char **mapNames = NULL;

	tupStandardMaterial.GetShadingTypeName(&pString);

	// switch to correct shader if found
	for (int shaderIndex=0;shaderIndex<SHADERS_TOTAL;shaderIndex++)
	{
		if (!strcmp(pString,shaderNamesArray[shaderIndex]))
		{
			pMaterial->SwitchShader(ShaderClassIDArray[shaderIndex]); 
			mapNames = ShaderMapNamesArray[shaderIndex];
			break;
		}
	}

	Shader *pShader = pMaterial->GetShader();

	tupStandardMaterial.GetName(&pString); pMaterial->SetName(pString);
	tupStandardMaterial.GetAmbientColor(&pFloatList); pMaterial->SetAmbient(*((Color *)pFloatList),m_pInterface->GetTime());
	tupStandardMaterial.GetDiffuseColor(&pFloatList); pMaterial->SetDiffuse(*((Color *)pFloatList),m_pInterface->GetTime());
	tupStandardMaterial.GetSpecularColor(&pFloatList); pMaterial->SetSpecular(*((Color *)pFloatList),m_pInterface->GetTime());
	tupStandardMaterial.GetFilterColor(&pFloatList); pMaterial->SetFilter(*((Color *)pFloatList),m_pInterface->GetTime());

	tupStandardMaterial.GetGlossiness(floatValue); pMaterial->SetShininess(floatValue,m_pInterface->GetTime());
	tupStandardMaterial.GetSpecularLevel(floatValue); pMaterial->SetShinStr(floatValue,m_pInterface->GetTime());
	tupStandardMaterial.GetSpecularSoften(floatValue); pShader->SetSoftenLevel(floatValue,m_pInterface->GetTime());
//	tupStandardMaterial.GetTransparency(floatValue);
	tupStandardMaterial.GetWireSize(floatValue); pMaterial->SetWireSize(floatValue,m_pInterface->GetTime());
	tupStandardMaterial.GetIndexOfRefraction(floatValue); pMaterial->SetIOR(floatValue,m_pInterface->GetTime());
	tupStandardMaterial.GetReflectionDimIntensity(floatValue); pb_extended->SetValue(std2_dim_lvl,m_pInterface->GetTime(),floatValue);
	tupStandardMaterial.GetReflectionDimMultiplier(floatValue); pb_extended->SetValue(std2_refl_lvl,m_pInterface->GetTime(),floatValue);

	tupStandardMaterial.GetOpacity(floatValue); pMaterial->SetOpacity(floatValue,m_pInterface->GetTime());
	tupStandardMaterial.GetOpacityFalloff(floatValue); pMaterial->SetOpacFalloff(floatValue,m_pInterface->GetTime());
	tupStandardMaterial.GetTransparencyType(intValue); pMaterial->SetTransparencyType(intValue);

	tupStandardMaterial.GetSelfIllumination(floatValue); pMaterial->SetSelfIllum(floatValue,m_pInterface->GetTime());

	int flags;
	tupStandardMaterial.GetFlags(flags);

	pMaterial->SetSoften((flags & TupStandardMaterial::FLAGS_SOFTEN) ? TRUE : FALSE);
	pMaterial->SetFaceMap((flags & TupStandardMaterial::FLAGS_FACE_MAPPING) ? TRUE : FALSE);
	pMaterial->SetWire((flags & TupStandardMaterial::FLAGS_WIRE) ? TRUE : FALSE);
	pMaterial->SetWireUnits((flags & TupStandardMaterial::FLAGS_WIRE_UNITS) ? TRUE : FALSE);
	pMaterial->SetFalloffOut((flags & TupStandardMaterial::FLAGS_FALLOFF_OUT) ? TRUE : FALSE);
	pMaterial->LockAmbDiffTex((flags & TupStandardMaterial::FLAGS_AMBIENT_DIFFUSE_TEXTURE_LOCK) ? TRUE : FALSE);
	pShader->SetLockAD((flags & TupStandardMaterial::FLAGS_AMBIENT_DIFFUSE_COLOR_LOCK) ? TRUE : FALSE);
	pShader->SetLockDS((flags & TupStandardMaterial::FLAGS_DIFFUSE_SPECULAR_COLOR_LOCK) ? TRUE : FALSE);

	pMaterial->SetSelfIllumColorOn((flags & TupStandardMaterial::FLAGS_SELF_ILLUMINATION_COLOR) ? TRUE : FALSE);

	pb_extended->SetValue(std2_apply_refl_dimming,m_pInterface->GetTime(),(flags & TupStandardMaterial::REFLECTION_DIMMING_APPLY) ? TRUE : FALSE);

	if (mapNames) // make sure we have a shader to map to
	{
		int numMaps = tupStandardMaterial.GetNumMaps();
		for (int i=0;i<numMaps;i++)
		{
			TupperwareAggregate *pMapAgg = tupStandardMaterial.GetMap(i);
			TupStandardMaterialMap tupMap(pMapAgg);
			tupMap.GetMapName(&pString);
			int slot=0;
			bool bFoundSlot = false;
			while(mapNames[slot])
			{
				if (!strcmp(pString,mapNames[slot]))
				{
					bFoundSlot = true;
					break;
				}
				slot++;
			}
			if (bFoundSlot)
			{
				tupMap.GetMapReference(intValue);
				Texmap *pMap = AddMap(intValue);
				pMaterial->SetSubTexmap(slot,pMap);
				tupMap.GetAmount(floatValue); pMaterial->SetTexmapAmt(slot,floatValue,m_pInterface->GetTime());
				int mapFlags;
				tupMap.GetFlags(mapFlags);
				pMaterial->EnableMap(slot,(mapFlags & TupStandardMaterialMap::ENABLED) ? TRUE : FALSE);
			}
		}
	}
	return pMaterial;
}

enum MultiSubObjectMaterialParameterBlockEnums
{ 
	multi_mtls,
	multi_ons,
	multi_names,
	multi_ids,
};

MultiMtl * TupImport::AddMultiSubObjectMaterial(TupperwareAggregate *pMaterialAgg)
{
	MultiMtl *pMaterial = (MultiMtl*)CreateInstance(MATERIAL_CLASS_ID, Class_ID(MULTI_CLASS_ID,0)); 

	int numBlocks = pMaterial->NumParamBlocks();
	IParamBlock2 *pblock = pMaterial->GetParamBlock(0);

	TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);

	char *pString;
	tupMultiSubObjectMaterial.GetName(&pString); pMaterial->SetName(pString);

	int numMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials();
	pMaterial->SetNumSubMtls(numMaterials);
	for (int i=0;i<numMaterials;i++)
	{
		TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(i);
		TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
		int subMaterialRef;
		tupSubMaterial.GetMaterialReference(subMaterialRef);
		if (subMaterialRef!=-1)
		{
			Mtl *pSubMat = AddMaterial(subMaterialRef);
			pMaterial->SetSubMtl(i,pSubMat);
			tupSubMaterial.GetSlotName(&pString); 
			TSTR subMtlName = pString;
			pMaterial->SetSubMtlAndName(i,pSubMat,subMtlName);

			// find the material with the id of i within the parameter block table
			int tableIndex;
			for (tableIndex = 0;  tableIndex < numMaterials;  tableIndex++ ) 
			{
				int id;
				pblock->GetValue(multi_ids, m_pInterface->GetTime(), id, FOREVER, tableIndex);
				if ( id == i )
					break;
			}
			assert(tableIndex<numMaterials); // should be found!!!
			int flags;
			tupSubMaterial.GetFlags(flags);
			pblock->SetValue(multi_ons, m_pInterface->GetTime(), (flags & TupMultiSubObjectMaterialSubMaterial::ENABLED) ? TRUE : FALSE,  tableIndex);
		}
	}
	return pMaterial;
}

enum BlendMaterialParameterBlockEnums
{ 
	mixmat_mix, mixmat_curvea, mixmat_curveb, mixmat_usecurve, mixmat_usemat,
	mixmat_map1, mixmat_map2, mixmat_mask,		
	mixmat_map1_on, mixmat_map2_on,  mixmat_mask_on // main grad params 
};

Mtl *TupImport::AddBlendMaterial(TupperwareAggregate *pMaterialAgg)
{
	Mtl *pMaterial = (Mtl*)CreateInstance(MATERIAL_CLASS_ID, Class_ID(MIXMAT_CLASS_ID,0)); 

	int numBlocks = pMaterial->NumParamBlocks();
	IParamBlock2 *pblock = pMaterial->GetParamBlock(0);

	TupBlendMaterial tupBlendMaterial(pMaterialAgg);

	char *pString;
	tupBlendMaterial.GetName(&pString); pMaterial->SetName(pString);
	int intValue;
	TupperwareScalar *pTupScalar;
	pTupScalar = tupBlendMaterial.GetMaterial1Reference(intValue);
	if (pTupScalar)
	{
		Mtl *pSubMaterial1 = AddMaterial(intValue);
		pMaterial->SetSubMtl(0,pSubMaterial1);
	}
	pTupScalar = tupBlendMaterial.GetMaterial2Reference(intValue);
	if (pTupScalar)
	{
		Mtl *pSubMaterial2 = AddMaterial(intValue);
		pMaterial->SetSubMtl(1,pSubMaterial2);
	}
	pTupScalar = tupBlendMaterial.GetMaskReference(intValue);
	if (pTupScalar)
	{
		Texmap *pSubMap = AddMap(intValue);
		pMaterial->SetSubTexmap(0,pSubMap);
	}

	int flags;
	tupBlendMaterial.GetFlags(flags);
	pblock->SetValue( mixmat_map1_on,m_pInterface->GetTime(),(flags&TupBlendMaterial::MATERIAL_1_ENABLE) ? TRUE : FALSE);
	pblock->SetValue( mixmat_map2_on,m_pInterface->GetTime(),(flags&TupBlendMaterial::MATERIAL_2_ENABLE) ? TRUE : FALSE);
	pblock->SetValue( mixmat_mask_on,m_pInterface->GetTime(),(flags&TupBlendMaterial::MASK_ENABLE) ? TRUE : FALSE);
	pblock->SetValue( mixmat_usecurve,m_pInterface->GetTime(),(flags&TupBlendMaterial::USE_CURVE) ? TRUE : FALSE);
	float floatValue;
	tupBlendMaterial.GetMixAmount(floatValue); pblock->SetValue(mixmat_mix,m_pInterface->GetTime(),floatValue);
	tupBlendMaterial.GetLowerTransitionZone(floatValue); pblock->SetValue(mixmat_curvea,m_pInterface->GetTime(),floatValue);
	tupBlendMaterial.GetUpperTransitionZone(floatValue); pblock->SetValue(mixmat_curveb,m_pInterface->GetTime(),floatValue);
	return pMaterial;
}

enum CompositeMaterialParameterBlockEnums
{ 
	compmat_mtls,
	compmat_type, 
	compmat_map_on, 
	compmat_amount
};

Mtl *TupImport::AddCompositeMaterial(TupperwareAggregate *pMaterialAgg)
{
	Mtl *pMaterial = (Mtl*)CreateInstance(MATERIAL_CLASS_ID, compositematClassID); 

	int numBlocks = pMaterial->NumParamBlocks();
	IParamBlock2 *pblock = pMaterial->GetParamBlock(0);

	TupCompositeMaterial tupCompositeMaterial(pMaterialAgg);
	char *pString;
	tupCompositeMaterial.GetName(&pString); pMaterial->SetName(pString);

	int numSubMaterials = tupCompositeMaterial.GetNumSubMaterials();
	int intValue;
	TupperwareScalar *pTupScalar = tupCompositeMaterial.GetBaseMaterialReference(intValue);
	if (pTupScalar)
	{
		Mtl *pBaseMaterial = AddMaterial(intValue);
		pMaterial->SetSubMtl(0,pBaseMaterial);
	}
	for (int i=0;i<numSubMaterials;i++)
	{
		TupperwareAggregate *pSubMaterialAgg = tupCompositeMaterial.GetSubMaterial(i);
		TupCompositeMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
		tupSubMaterial.GetMaterialReference(intValue);
		
		Mtl *subMaterial = AddMaterial(intValue);
		pMaterial->SetSubMtl(i+1,subMaterial);

		int subMaterialFlags;
		tupSubMaterial.GetFlags(subMaterialFlags);
		switch (subMaterialFlags & TupCompositeMaterialSubMaterial::MIXING_MODE_MASK)
		{
			case TupCompositeMaterialSubMaterial::MIXING_MODE_ADDITIVE:
				pblock->SetValue(compmat_type, m_pInterface->GetTime(), 0, i);
				break;
			case TupCompositeMaterialSubMaterial::MIXING_MODE_SUBTRACTIVE:
				pblock->SetValue(compmat_type, m_pInterface->GetTime(), 1, i);
				break;
			case TupCompositeMaterialSubMaterial::MIXING_MODE_MIXED:
				pblock->SetValue(compmat_type, m_pInterface->GetTime(), 2, i);
				break;
		}
		pblock->SetValue(compmat_map_on, m_pInterface->GetTime(),(subMaterialFlags & TupCompositeMaterialSubMaterial::ENABLED) ? TRUE : FALSE, i);
		float floatValue;
		tupSubMaterial.GetMixAmount(floatValue); pblock->GetValue(compmat_amount, m_pInterface->GetTime(), floatValue, FOREVER, i);
	}
	return pMaterial;
}

enum DoubleSidedMaterialParameterBlockEnums
{ 
	doublesided_map1, doublesided_map2,		
	doublesided_map1_on, doublesided_map2_on, // main grad params 
	doublesided_transluency
};

Mtl *TupImport::AddDoubleSidedMaterial(TupperwareAggregate *pMaterialAgg)
{
	Mtl *pMaterial = (Mtl*)CreateInstance(MATERIAL_CLASS_ID, Class_ID(DOUBLESIDED_CLASS_ID,0)); 

	int numBlocks = pMaterial->NumParamBlocks();
	IParamBlock2 *pblock = pMaterial->GetParamBlock(0);

	TupDoubleSidedMaterial tupDoubleSidedMaterial(pMaterialAgg);
	char *pString;
	tupDoubleSidedMaterial.GetName(&pString); pMaterial->SetName(pString);
	int intValue;
	TupperwareScalar *pTupScalar;
	pTupScalar = tupDoubleSidedMaterial.GetFrontMaterialReference(intValue);
	if (pTupScalar)
	{
		Mtl *pFrontMaterial = AddMaterial(intValue);
		pMaterial->SetSubMtl(0,pFrontMaterial);
	}
	pTupScalar = tupDoubleSidedMaterial.GetBackMaterialReference(intValue);
	if (pTupScalar)
	{
		Mtl *pBackMaterial = AddMaterial(intValue);
		pMaterial->SetSubMtl(1,pBackMaterial);
	}
	int flags;
	tupDoubleSidedMaterial.GetFlags(flags);
	pblock->SetValue(doublesided_map1_on,m_pInterface->GetTime(),(flags & TupDoubleSidedMaterial::FRONT_MATERIAL_ENABLE) ? TRUE : FALSE);
	pblock->SetValue(doublesided_map2,m_pInterface->GetTime(),(flags & TupDoubleSidedMaterial::BACK_MATERIAL_ENABLE) ? TRUE : FALSE);
	float floatValue;
	tupDoubleSidedMaterial.GetTranslucency(floatValue); pblock->SetValue(doublesided_transluency,m_pInterface->GetTime(),floatValue);
	return pMaterial;
}
