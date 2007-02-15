////////////////////////////////////////////////////////////////////////////
//
// ExportMaterial
//
// Class to handle the materials
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportMaterial.cpp $
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 9/12/02    Time: 4:18p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 9/12/02    Time: 4:17p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * multi-sub object material empty slot fix
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 5/21/02    Time: 1:55p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Fixed due to changes in keeper
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 9:37a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Fixed problems with Shader channels (moved away from enums and now
 * queries the material and shader for names)
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 12/27/01   Time: 4:30p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * xrefs & derived objects (skeleton)
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/21/01   Time: 9:47a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Made null maps, materials, object, etc use reference #-1
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/18/01   Time: 11:34a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Materials, Maps complete - saving all information (except for plugin
 * shaders)
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/17/01   Time: 11:07a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Materials & maps exporting (others still to do)
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/14/01   Time: 4:39p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Start of standard material exporting
 */

#include "TupExportPch.h"
#include "TupExport.h"

static Class_ID compositematClassID(0x61dc0cd7, 0x13640af6);

int TupExport::AddMaterial(Mtl *pMaterial)
{
	if (!pMaterial) return -1;
	int index = m_pMaterialPoolPtrs->GetIndex(pMaterial);
	if (index==-1) // if it is not in the list
	{
		index = m_pMaterialPoolPtrs->Add(pMaterial);
		TupperwareAggregate *pMaterialAgg = m_pMaterialPool->AddMaterial(index);
		TupMaterial tupMaterial(pMaterialAgg);

		if (pMaterial->ClassID() == Class_ID(DMTL_CLASS_ID, 0)) 
		{
			tupMaterial.SetMaterialType(TupMaterial::STANDARD_MATERIAL);
			AddStandardMaterial(pMaterialAgg,(StdMat2*)pMaterial);
		}
		else if (pMaterial->ClassID() == Class_ID(MULTI_CLASS_ID, 0)) 
		{
			tupMaterial.SetMaterialType(TupMaterial::MULTI_SUB_OBJECT_MATERIAL);
			AddMultiSubObjectMaterial(pMaterialAgg,(MultiMtl*)pMaterial);
		}
		else if (pMaterial->ClassID() == Class_ID(MIXMAT_CLASS_ID, 0)) 
		{
			tupMaterial.SetMaterialType(TupMaterial::BLEND_MATERIAL);
			AddBlendMaterial(pMaterialAgg,pMaterial);
		}
		else if (pMaterial->ClassID() == compositematClassID) 
		{
			tupMaterial.SetMaterialType(TupMaterial::COMPOSITE_MATERIAL);
			AddCompositeMaterial(pMaterialAgg,pMaterial);
		}
		else if (pMaterial->ClassID() == Class_ID(DOUBLESIDED_CLASS_ID,0)) 
		{
			tupMaterial.SetMaterialType(TupMaterial::DOUBLE_SIDED_MATERIAL);
			AddDoubleSidedMaterial(pMaterialAgg,pMaterial);
		}
		else
		{
			tupMaterial.SetMaterialType(TupMaterial::UNKNOWN);
		}
	}
	return index;
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

#if 0
	enum TextureChannels
	{
		TEXTURE_CHANNEL_AMBIENT,
		TEXTURE_CHANNEL_DIFFUSE,
		TEXTURE_CHANNEL_SPECULAR,
		TEXTURE_CHANNEL_GLOSSINESS,
		TEXTURE_CHANNEL_SPECULAR_LEVEL,
		TEXTURE_CHANNEL_SELF_ILLUMINATION,
		TEXTURE_CHANNEL_OPACITY,
		TEXTURE_CHANNEL_FILTER_COLOR,
		TEXTURE_CHANNEL_BUMP,
		TEXTURE_CHANNEL_REFLECTION,
		TEXTURE_CHANNEL_REFRACTION,
		TEXTURE_CHANNEL_DISPLACEMENT,
		NUM_TEXTURE_CHANNELS, // last entry
	};

	static char *m_textureChannelNames[NUM_TEXTURE_CHANNELS];
char *TupStandardMaterial::m_textureChannelNames[TupStandardMaterial::NUM_TEXTURE_CHANNELS] = 
{
	"Ambient", // TEXTURE_CHANNEL_AMBIENT
	"Diffuse", // TEXTURE_CHANNEL_DIFFUSE
	"Specular", // TEXTURE_CHANNEL_SPECULAR
	"Glossiness", // TEXTURE_CHANNEL_GLOSSINESS
	"SpecularLevel", // TEXTURE_CHANNEL_SPECULAR_LEVEL
	"SelfIllumination", // TEXTURE_CHANNEL_SELF_ILLUMINATION
	"Opacity", // TEXTURE_CHANNEL_OPACITY
	"FilterColor", // TEXTURE_CHANNEL_FILTER_COLOR
	"Bump", // TEXTURE_CHANNEL_BUMP
	"Reflection", // TEXTURE_CHANNEL_REFLECTION
	"Refraction", // TEXTURE_CHANNEL_REFRACTION
	"Displacement", // TEXTURE_CHANNEL_DISPLACEMENT
};

#endif
void TupExport::AddStandardMaterial(TupperwareAggregate *pMaterialAgg,StdMat2 *pMaterial)
{
	IParamBlock2 *pb_extended = pMaterial->GetParamBlock(1); // get extended
	Shader *pShader = pMaterial->GetShader();

	TupStandardMaterial tupStandardMaterial(pMaterialAgg);

	tupStandardMaterial.AddName(pMaterial->GetName());
	tupStandardMaterial.AddAmbientColor((float *)pMaterial->GetAmbient(m_pInterface->GetTime()));
	tupStandardMaterial.AddDiffuseColor((float *)pMaterial->GetDiffuse(m_pInterface->GetTime()));
	tupStandardMaterial.AddSpecularColor((float *)pMaterial->GetSpecular(m_pInterface->GetTime()));
	tupStandardMaterial.AddFilterColor((float *)pMaterial->GetFilter(m_pInterface->GetTime()));

	tupStandardMaterial.AddGlossiness(pMaterial->GetShininess(m_pInterface->GetTime()));
	tupStandardMaterial.AddSpecularLevel(pMaterial->GetShinStr(m_pInterface->GetTime()));
	tupStandardMaterial.AddSpecularSoften(pShader->GetSoftenLevel(m_pInterface->GetTime()));
	tupStandardMaterial.AddTransparency(pMaterial->GetXParency());
	tupStandardMaterial.AddWireSize(pMaterial->GetWireSize(m_pInterface->GetTime()));
	tupStandardMaterial.AddIndexOfRefraction(pMaterial->GetIOR(m_pInterface->GetTime()));
	tupStandardMaterial.AddReflectionDimIntensity(pb_extended->GetFloat(std2_dim_lvl,m_pInterface->GetTime()));
	tupStandardMaterial.AddReflectionDimMultiplier(pb_extended->GetFloat(std2_refl_lvl,m_pInterface->GetTime()));

	tupStandardMaterial.AddOpacity(pMaterial->GetOpacity(m_pInterface->GetTime()));
	tupStandardMaterial.AddOpacityFalloff(pMaterial->GetOpacFalloff(m_pInterface->GetTime()));
	tupStandardMaterial.AddTransparencyType(pMaterial->GetTransparencyType());

	tupStandardMaterial.AddShadingTypeName(pShader->GetName());
	tupStandardMaterial.AddSelfIllumination(pMaterial->GetSelfIllum(m_pInterface->GetTime()));

	int flags = 0;
	if (pMaterial->GetSoften()) flags |= TupStandardMaterial::FLAGS_SOFTEN;
	if (pMaterial->GetFaceMap()) flags |= TupStandardMaterial::FLAGS_FACE_MAPPING;
	if (pMaterial->GetTwoSided()) flags |= TupStandardMaterial::FLAGS_TWO_SIDED;
	if (pMaterial->GetWire()) flags |= TupStandardMaterial::FLAGS_WIRE;
	if (pMaterial->GetWireUnits()) flags |= TupStandardMaterial::FLAGS_WIRE_UNITS;
	if (pMaterial->GetFalloffOut()) flags |= TupStandardMaterial::FLAGS_FALLOFF_OUT;
	if (pMaterial->GetAmbDiffTexLock()) flags |= TupStandardMaterial::FLAGS_AMBIENT_DIFFUSE_TEXTURE_LOCK;
	if (pShader->GetLockAD()) flags |= TupStandardMaterial::FLAGS_AMBIENT_DIFFUSE_COLOR_LOCK;
	if (pShader->GetLockDS()) flags |= TupStandardMaterial::FLAGS_DIFFUSE_SPECULAR_COLOR_LOCK;

	if (pMaterial->GetSelfIllumColorOn()) flags |= TupStandardMaterial::FLAGS_SELF_ILLUMINATION_COLOR;
	if (pb_extended->GetInt(std2_apply_refl_dimming,m_pInterface->GetTime())) flags |= TupStandardMaterial::REFLECTION_DIMMING_APPLY;

	tupStandardMaterial.AddFlags(flags);

	int numMaps = pMaterial->NumSubTexmaps();
	for (int i=0;i<numMaps;i++)
	{
		int mapState = pMaterial->GetMapState(i);
		if (mapState > 0)
		{
			Texmap *texMap = pMaterial->GetSubTexmap(i);
			if (texMap)
			{
				TupperwareAggregate *pMapAgg = tupStandardMaterial.AddMap(i);
				TupStandardMaterialMap tupMap(pMapAgg);
				TupString slotName = pMaterial->GetSubTexmapSlotName(i);
				tupMap.AddMapName(slotName);
				tupMap.AddAmount(pMaterial->GetTexmapAmt(i,m_pInterface->GetTime()));
				int mapFlags = 0;
				if (mapState==2) mapFlags |= TupStandardMaterialMap::ENABLED;
				tupMap.AddFlags(mapFlags);
				tupMap.AddMapReference(AddMap(texMap));
			}
		}
	}
}

enum MultiSubObjectMaterialParameterBlockEnums
{ 
	multi_mtls,
	multi_ons,
	multi_names,
	multi_ids,
};

void TupExport::AddMultiSubObjectMaterial(TupperwareAggregate *pMaterialAgg,MultiMtl *pMaterial)
{
	int numBlocks = pMaterial->NumParamBlocks();
	IParamBlock2 *pblock = pMaterial->GetParamBlock(0);

	TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);
	tupMultiSubObjectMaterial.AddName(pMaterial->GetName());

	int numMaterials = pMaterial->NumSubMtls();
	for (int i=0;i<numMaterials;i++)
	{
		Mtl *subMaterial = pMaterial->GetSubMtl(i);
		if (subMaterial)
		{
			TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.AddSubMaterial(i);
			TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
			TSTR slotName;
			pMaterial->GetSubMtlName(i,slotName);
			tupSubMaterial.AddSlotName(slotName);
			int subMaterialFlags = 0;

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
			int enabled;
			pblock->GetValue(multi_ons, m_pInterface->GetTime(), enabled, FOREVER, tableIndex);
			if (enabled) subMaterialFlags |= TupMultiSubObjectMaterialSubMaterial::ENABLED;
			tupSubMaterial.AddFlags(subMaterialFlags);
			tupSubMaterial.AddMaterialReference(AddMaterial(subMaterial));
		}
		else // if this slot is empty
		{
			TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.AddSubMaterial(i);
			TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
			TSTR slotName;
			tupSubMaterial.AddSlotName(slotName);
			int subMaterialFlags = 0;
			tupSubMaterial.AddFlags(subMaterialFlags);
			tupSubMaterial.AddMaterialReference(-1);
		}
	}
}

enum BlendMaterialParameterBlockEnums
{ 
	mixmat_mix, mixmat_curvea, mixmat_curveb, mixmat_usecurve, mixmat_usemat,
	mixmat_map1, mixmat_map2, mixmat_mask,		
	mixmat_map1_on, mixmat_map2_on,  mixmat_mask_on // main grad params 
};

void TupExport::AddBlendMaterial(TupperwareAggregate *pMaterialAgg,Mtl *pMaterial)
{
	int numBlocks = pMaterial->NumParamBlocks();
	IParamBlock2 *pblock = pMaterial->GetParamBlock(0);

	TupBlendMaterial tupBlendMaterial(pMaterialAgg);
	tupBlendMaterial.AddName(pMaterial->GetName());
	Mtl *subMaterial1 = pMaterial->GetSubMtl(0);
	if (subMaterial1)
	{
		tupBlendMaterial.AddMaterial1Reference(AddMaterial(subMaterial1));
	}
	Mtl *subMaterial2 = pMaterial->GetSubMtl(1);
	if (subMaterial2)
	{
		tupBlendMaterial.AddMaterial2Reference(AddMaterial(subMaterial2));
	}
	Texmap *maskMap = pMaterial->GetSubTexmap(0);
	if (maskMap)
	{
		tupBlendMaterial.AddMaskReference(AddMap(maskMap));
	}
	int flags = 0;
	if (pblock->GetInt( mixmat_map1_on,m_pInterface->GetTime())) flags |= TupBlendMaterial::MATERIAL_1_ENABLE;
	if (pblock->GetInt( mixmat_map2_on,m_pInterface->GetTime())) flags |= TupBlendMaterial::MATERIAL_2_ENABLE;
	if (pblock->GetInt( mixmat_mask_on,m_pInterface->GetTime())) flags |= TupBlendMaterial::MASK_ENABLE;
	if (pblock->GetInt( mixmat_usecurve,m_pInterface->GetTime())) flags |= TupBlendMaterial::USE_CURVE;
	tupBlendMaterial.AddFlags(flags);

	tupBlendMaterial.AddMixAmount(pblock->GetFloat(mixmat_mix,m_pInterface->GetTime()));
	tupBlendMaterial.AddLowerTransitionZone(pblock->GetFloat(mixmat_curvea,m_pInterface->GetTime()));
	tupBlendMaterial.AddUpperTransitionZone(pblock->GetFloat(mixmat_curveb,m_pInterface->GetTime()));
}

enum CompositeMaterialParameterBlockEnums
{ 
	compmat_mtls,
	compmat_type, 
	compmat_map_on, 
	compmat_amount
};

void TupExport::AddCompositeMaterial(TupperwareAggregate *pMaterialAgg,Mtl *pMaterial)
{
	int numBlocks = pMaterial->NumParamBlocks();
	IParamBlock2 *pblock = pMaterial->GetParamBlock(0);

	TupCompositeMaterial tupCompositeMaterial(pMaterialAgg);
	tupCompositeMaterial.AddName(pMaterial->GetName());

	int numMaterials = pMaterial->NumSubMtls();
	for (int i=0;i<numMaterials;i++)
	{
		Mtl *subMaterial = pMaterial->GetSubMtl(i);
		if (subMaterial)
		{
			if (i==0) // base
			{
				tupCompositeMaterial.AddBaseMaterialReference(AddMaterial(subMaterial));
			}
			else
			{
				TupperwareAggregate *pSubMaterialAgg = tupCompositeMaterial.AddSubMaterial(i);
				TupCompositeMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);

				int subMaterialFlags = 0;
				int type;
				pblock->GetValue(compmat_type, m_pInterface->GetTime(), type, FOREVER, i-1);
				switch (type)
				{
					case 0:
						subMaterialFlags |= TupCompositeMaterialSubMaterial::MIXING_MODE_ADDITIVE;
						break;
					case 1:
						subMaterialFlags |= TupCompositeMaterialSubMaterial::MIXING_MODE_SUBTRACTIVE;
						break;
					case 2:
						subMaterialFlags |= TupCompositeMaterialSubMaterial::MIXING_MODE_MIXED;
						break;
				}
				int enabled;
				pblock->GetValue(compmat_map_on, m_pInterface->GetTime(), enabled, FOREVER, i-1);
				if (enabled) subMaterialFlags|= TupCompositeMaterialSubMaterial::ENABLED;
				tupSubMaterial.AddFlags(subMaterialFlags);
				float amount;
				pblock->GetValue(compmat_amount, m_pInterface->GetTime(), amount, FOREVER, i-1);
				tupSubMaterial.AddMixAmount(amount);
				tupSubMaterial.AddMaterialReference(AddMaterial(subMaterial));
			}
		}
	}
}

enum DoubleSidedMaterialParameterBlockEnums
{ 
	doublesided_map1, doublesided_map2,		
	doublesided_map1_on, doublesided_map2_on, // main grad params 
	doublesided_transluency
};

void TupExport::AddDoubleSidedMaterial(TupperwareAggregate *pMaterialAgg,Mtl *pMaterial)
{
	int numBlocks = pMaterial->NumParamBlocks();
	IParamBlock2 *pblock = pMaterial->GetParamBlock(0);

	TupDoubleSidedMaterial tupDoubleSidedMaterial(pMaterialAgg);
	tupDoubleSidedMaterial.AddName(pMaterial->GetName());
	Mtl *frontMaterial = pMaterial->GetSubMtl(0);
	if (frontMaterial)
	{
		tupDoubleSidedMaterial.AddFrontMaterialReference(AddMaterial(frontMaterial));
	}
	Mtl *backMaterial = pMaterial->GetSubMtl(1);
	if (backMaterial)
	{
		tupDoubleSidedMaterial.AddBackMaterialReference(AddMaterial(backMaterial));
	}
	int flags = 0;
	if (pblock->GetInt( doublesided_map1_on,m_pInterface->GetTime())) flags |= TupDoubleSidedMaterial::FRONT_MATERIAL_ENABLE;
	if (pblock->GetInt( doublesided_map2,m_pInterface->GetTime())) flags |= TupDoubleSidedMaterial::BACK_MATERIAL_ENABLE;
	tupDoubleSidedMaterial.AddFlags(flags);

	tupDoubleSidedMaterial.AddTranslucency(pblock->GetFloat(doublesided_transluency,m_pInterface->GetTime()));
}
