////////////////////////////////////////////////////////////////////////////
//
// ImportMap
//
// Class to handle the maps
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImportMap.cpp $
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
#include "uvgen.h"

Texmap *TupImport::AddMap(int mapRef)
{
	// if null reference
	if (mapRef==-1)
		return NULL;
	// if already created
	if ((*m_pMapPoolPtrs)[mapRef])
		return (*m_pMapPoolPtrs)[mapRef];

	Texmap *pMap = NULL;

	TupperwareAggregate *pMapAgg = m_pMapPool->GetMap(mapRef);
	assert(pMapAgg);
	TupMap tupMap(pMapAgg);
	switch (tupMap.GetMapType())
	{
		case TupMap::BITMAP_MAP:
			pMap = AddBitmapMap(pMapAgg);
			break;
		case TupMap::COMPOSITE_MAP:
			pMap = AddCompositeMap(pMapAgg);
			break;
		case TupMap::MIX_MAP:
			pMap = AddMixMap(pMapAgg);
			break;
	}
	(*m_pMapPoolPtrs)[mapRef] = pMap;
	return pMap;
}

// BitmapTex paramater block enums
enum BitmapTexEnums1
{ 
	bmtex_clipu,bmtex_clipv,bmtex_clipw,bmtex_cliph,
	bmtex_jitter,bmtex_usejitter,
	bmtex_apply,bmtex_crop_place,
	bmtex_filtering,
	bmtex_monooutput,
	bmtex_rgboutput,
	bmtex_alphasource,
	bmtex_premultalpha,
	bmtex_bitmap,
	bmtex_coords,	 // access for UVW mapping
	bmtex_output,	 //output window
	bmtex_filename   // bitmap filename virtual parameter, JBW 2/23/99
};

enum BitmapTexEnums2
{ 
	bmtex_start,bmtex_playbackrate, bmtex_endcondition
};

BitmapTex *TupImport::AddBitmapMap(TupperwareAggregate *pMapAgg)
{
	BitmapTex *pMap = (BitmapTex*)CreateInstance(TEXMAP_CLASS_ID, Class_ID(BMTEX_CLASS_ID,0)); 

	int numBlocks = pMap->NumParamBlocks();
	IParamBlock2 *pblock1 = pMap->GetParamBlock(0);
	IParamBlock2 *pblock2 = pMap->GetParamBlock(1);

	TupBitmapMap tupBitmapMap(pMapAgg);

	char *pString;
	int intValue;
	float floatValue;

	tupBitmapMap.GetName(&pString); pMap->SetName(pString);

	tupBitmapMap.GetImageReference(intValue); pMap->SetMapName(GetImagePathName(intValue));

	StdUV* pUVGen = static_cast<StdUV *>(pMap->GetUVGen());
	assert(pUVGen);
	tupBitmapMap.GetMapping(intValue); SetMapping(pUVGen,intValue);
	tupBitmapMap.GetMapChannel(intValue); pUVGen->SetMapChannel(intValue);
	tupBitmapMap.GetUOffset(floatValue); pUVGen->SetUOffs(floatValue,m_pInterface->GetTime());
	tupBitmapMap.GetVOffset(floatValue); pUVGen->SetVOffs(floatValue,m_pInterface->GetTime());
	tupBitmapMap.GetUTiling(floatValue); pUVGen->SetUScl(floatValue,m_pInterface->GetTime());
	tupBitmapMap.GetVTiling(floatValue); pUVGen->SetVScl(floatValue,m_pInterface->GetTime());
	tupBitmapMap.GetMapAxis(intValue); pUVGen->SetAxis(intValue);
	tupBitmapMap.GetUAngle(floatValue); pUVGen->SetUAng(floatValue,m_pInterface->GetTime());
	tupBitmapMap.GetVAngle(floatValue); pUVGen->SetVAng(floatValue,m_pInterface->GetTime());
	tupBitmapMap.GetWAngle(floatValue); pUVGen->SetWAng(floatValue,m_pInterface->GetTime());
	tupBitmapMap.GetBlur(floatValue); pUVGen->SetBlur(floatValue,m_pInterface->GetTime());
	tupBitmapMap.GetBlurOffset(floatValue); pUVGen->SetBlurOffs(floatValue,m_pInterface->GetTime());
	tupBitmapMap.GetNoiseAmount(floatValue); pUVGen->SetNoiseAmt(floatValue,m_pInterface->GetTime());
	tupBitmapMap.GetNoiseLevels(intValue); pUVGen->SetNoiseLev(intValue,m_pInterface->GetTime());
	tupBitmapMap.GetNoiseSize(floatValue); pUVGen->SetNoiseSize(floatValue,m_pInterface->GetTime());
	tupBitmapMap.GetCroppingPlacementU(floatValue); pblock1->SetValue(bmtex_clipu,m_pInterface->GetTime(),floatValue);
	tupBitmapMap.GetCroppingPlacementV(floatValue); pblock1->SetValue(bmtex_clipv,m_pInterface->GetTime(),floatValue);
	tupBitmapMap.GetCroppingPlacementW(floatValue); pblock1->SetValue(bmtex_clipw,m_pInterface->GetTime(),floatValue);
	tupBitmapMap.GetCroppingPlacementH(floatValue); pblock1->SetValue(bmtex_cliph,m_pInterface->GetTime(),floatValue);
	tupBitmapMap.GetPlacementJitter(floatValue); pblock1->SetValue(bmtex_jitter,m_pInterface->GetTime(),floatValue);
	tupBitmapMap.GetStartFrame(intValue); pblock2->SetValue(bmtex_start,m_pInterface->GetTime(),intValue);
	tupBitmapMap.GetPlaybackRate(floatValue); pblock2->SetValue(bmtex_playbackrate,m_pInterface->GetTime(),floatValue);

	StdTexoutGen *pTexout = static_cast<StdTexoutGen *>(pMap->GetTexout());
	assert(pTexout);
	tupBitmapMap.GetOutputAmount(floatValue); pTexout->SetOutputLevel(m_pInterface->GetTime(),floatValue);
	tupBitmapMap.GetOutputRGBOffset(floatValue); pTexout->SetRGBOff(floatValue,m_pInterface->GetTime());
	tupBitmapMap.GetOutputRGBLevel(floatValue); pTexout->SetRGBAmt(floatValue,m_pInterface->GetTime());
	tupBitmapMap.GetOutputBumpAmount(floatValue); pTexout->SetBumpAmt(floatValue,m_pInterface->GetTime());

	int flags;
	tupBitmapMap.GetFlags(flags);

	pUVGen->SetTextureTiling(flags&(TupBitmapMap::TUP_U_WRAP|TupBitmapMap::TUP_V_WRAP|TupBitmapMap::TUP_U_MIRROR|TupBitmapMap::TUP_V_MIRROR));
	pUVGen->SetFlag(UV_NOISE,flags & TupBitmapMap::TUP_NOISE);
	pUVGen->SetFlag(UV_NOISE_ANI,flags & TupBitmapMap::TUP_NOISE_ANIMATE);
	pUVGen->hideMapBack = (flags & TupBitmapMap::TUP_SHOW_MAP_ON_BACK) ? TRUE : FALSE;
	switch (flags & TupBitmapMap::TUP_FILTER_TYPE_MASK)
	{
		case TupBitmapMap::TUP_FILTER_TYPE_PYRAMID:
			pMap->SetFilterType(FILTER_PYR);
			break;
		case TupBitmapMap::TUP_FILTER_TYPE_SUMMED_AREA:
			pMap->SetFilterType(FILTER_SAT);
			break;
		case TupBitmapMap::TUP_FILTER_TYPE_NONE:
			pMap->SetFilterType(FILTER_NADA);
			break;
	}

	pblock1->SetValue(bmtex_monooutput,m_pInterface->GetTime(),(flags & TupBitmapMap::TUP_MONO_CHANNEL_ALPHA) ? 1 : 0);
	pblock1->SetValue(bmtex_rgboutput,m_pInterface->GetTime(),(flags & TupBitmapMap::TUP_RGB_CHANNEL_ALPHA) ? 1 : 0);
	switch (flags & TupBitmapMap::TUP_ALPHA_SOURCE_MASK)
	{
		case TupBitmapMap::ALPHA_SOURCE_IMAGE_ALPHA:
			pblock1->SetValue(bmtex_alphasource,m_pInterface->GetTime(),ALPHA_FILE);
			break;
		case TupBitmapMap::ALPHA_SOURCE_RGB_INTENSITY:
			pblock1->SetValue(bmtex_alphasource,m_pInterface->GetTime(),ALPHA_RGB);
			break;
		case TupBitmapMap::ALPHA_SOURCE_NONE:
			pblock1->SetValue(bmtex_alphasource,m_pInterface->GetTime(),ALPHA_NONE);
			break;
	}
	pblock1->SetValue(bmtex_premultalpha,m_pInterface->GetTime(),(flags & TupBitmapMap::TUP_PREMULTIPLIED_ALPHA) ? 1 : 0);
	if (flags & TupBitmapMap::CROPPING_PLACEMENT_PLACEMENT)
	{
		pblock1->SetValue(bmtex_apply,m_pInterface->GetTime(),1);
		pblock1->SetValue(bmtex_crop_place,m_pInterface->GetTime(),1);
	}
	else
	{
		pblock1->SetValue(bmtex_apply,m_pInterface->GetTime(),0);
	}
	pblock1->SetValue(bmtex_usejitter,m_pInterface->GetTime(),(flags & TupBitmapMap::TUP_PLACEMENT_JITTER_ENABLED) ? 1 : 0);
	switch (flags & TupBitmapMap::TUP_ANIMATION_END_CONDITION_MASK)
	{
		case TupBitmapMap::ANIMATION_END_CONDITION_LOOP:
			pblock2->SetValue(bmtex_endcondition,m_pInterface->GetTime(),0);
			break;
		case TupBitmapMap::ANIMATION_END_CONDITION_PING_PONG:
			pblock2->SetValue(bmtex_endcondition,m_pInterface->GetTime(),1);
			break;
		case TupBitmapMap::ANIMATION_END_CONDITION_HOLD:
			pblock2->SetValue(bmtex_endcondition,m_pInterface->GetTime(),2);
			break;
	}
	pTexout->SetInvert((flags & TupBitmapMap::TUP_OUTPUT_INVERT) ? TRUE : FALSE);
	pTexout->SetClamp((flags & TupBitmapMap::TUP_OUTPUT_CLAMP) ? TRUE : FALSE);
	pTexout->SetAlphaFromRGB((flags & TupBitmapMap::TUP_OUTPUT_ALPHA_FROM_RGB_INTENSITY) ? TRUE : FALSE);
	pTexout->SetFlag(TEXOUT_COLOR_MAP,flags & TupBitmapMap::TUP_OUTPUT_ENABLE_COLOR_MAP);

	return pMap;
}

MultiTex *TupImport::AddCompositeMap(TupperwareAggregate *pMapAgg)
{
	MultiTex *pMap = (MultiTex*)CreateInstance(TEXMAP_CLASS_ID, Class_ID(COMPOSITE_CLASS_ID,0)); 

	TupCompositeMap tupCompositeMap(pMapAgg);

	char *pString;
	tupCompositeMap.GetName(&pString); pMap->SetName(pString);

	int numMaps = tupCompositeMap.GetNumSubMaps();
	pMap->SetNumSubTexmaps(numMaps);
	for (int i=0;i<numMaps; i++) 
	{
		TupperwareAggregate *subMapAgg = tupCompositeMap.GetSubMap(i);
		TupCompositeMapSubMap tupSubMap(subMapAgg);

		int subMapRef;
		TupperwareScalar *pMapRefScalar = tupSubMap.GetMapReference(subMapRef);
		if (pMapRefScalar)
		{
			Texmap *pTexMap = AddMap(subMapRef);
			pMap->SetSubTexmap(i,pTexMap);
		}
		else
		{
			pMap->SetSubTexmap(i,NULL);
		}
	}
	return pMap;
}

enum 
{ 
	mix_mix, mix_curvea, mix_curveb, mix_usecurve,
	mix_color1, mix_color2,
	mix_map1, mix_map2, mix_mask,		
	mix_map1_on, mix_map2_on,  mix_mask_on, // main grad params 
	mix_output
};

#define TEXOUT_REF 4

MultiTex *TupImport::AddMixMap(TupperwareAggregate *pMapAgg)
{
	MultiTex *pMap = (MultiTex*)CreateInstance(TEXMAP_CLASS_ID, Class_ID(MIX_CLASS_ID,0)); 

	int numBlocks = pMap->NumParamBlocks();
	IParamBlock2 *pblock = pMap->GetParamBlock(0);

	TupMixMap tupMixMap(pMapAgg);
	char *pString;
	tupMixMap.GetName(&pString); pMap->SetName(pString);
	int intValue;
	TupperwareScalar *pMapRef1Scalar = tupMixMap.GetMap1Reference(intValue);
	if (pMapRef1Scalar)
	{
		Texmap *pTexMap = AddMap(intValue);
		pMap->SetSubTexmap(0,pTexMap);
	}
	else
	{
		pMap->SetSubTexmap(0,NULL);
	}
	TupperwareScalar *pMapRef2Scalar = tupMixMap.GetMap2Reference(intValue);
	if (pMapRef2Scalar)
	{
		Texmap *pTexMap = AddMap(intValue);
		pMap->SetSubTexmap(1,pTexMap);
	}
	else
	{
		pMap->SetSubTexmap(1,NULL);
	}
	TupperwareScalar *pMixMapRefScalar = tupMixMap.GetMixMapReference(intValue);
	if (pMixMapRefScalar)
	{
		Texmap *pTexMap = AddMap(intValue);
		pMap->SetSubTexmap(2,pTexMap);
	}
	else
	{
		pMap->SetSubTexmap(2,NULL);
	}

	float floatValue;
	tupMixMap.GetMixAmount(floatValue); pblock->SetValue(mix_mix,m_pInterface->GetTime(),floatValue);
	tupMixMap.GetLowerTransitionZone(floatValue); pblock->SetValue(mix_curvea,m_pInterface->GetTime(),floatValue);
	tupMixMap.GetUpperTransitionZone(floatValue); pblock->SetValue(mix_curveb,m_pInterface->GetTime(),floatValue);

	StdTexoutGen *pTexout = static_cast<StdTexoutGen *>(pMap->GetReference(TEXOUT_REF));
	assert(pTexout);
	tupMixMap.GetOutputAmount(floatValue); pTexout->SetOutputLevel(m_pInterface->GetTime(),floatValue);
	tupMixMap.GetOutputRGBOffset(floatValue); pTexout->SetRGBOff(m_pInterface->GetTime(),floatValue);
	tupMixMap.GetOutputRGBLevel(floatValue); pTexout->SetRGBAmt(m_pInterface->GetTime(),floatValue);
	tupMixMap.GetOutputBumpAmount(floatValue); pTexout->SetBumpAmt(m_pInterface->GetTime(),floatValue);

	int flags;
	tupMixMap.GetFlags(flags);
	pblock->SetValue( mix_map1_on,m_pInterface->GetTime(),(flags&TupMixMap::MAP_1_ENABLE) ? 1 : 0);
	pblock->SetValue( mix_map2_on,m_pInterface->GetTime(),(flags&TupMixMap::MAP_2_ENABLE) ? 1 : 0);
	pblock->SetValue( mix_mask_on,m_pInterface->GetTime(),(flags&TupMixMap::MIX_MAP_ENABLE) ? 1 : 0);
	pblock->SetValue( mix_usecurve,m_pInterface->GetTime(),(flags&TupMixMap::USE_CURVE) ? 1 : 0);
	pTexout->SetInvert((flags & TupMixMap::TUP_OUTPUT_INVERT) ? 1 : 0);
	pTexout->SetClamp((flags & TupMixMap::TUP_OUTPUT_CLAMP) ? 1 : 0);
	pTexout->SetAlphaFromRGB((flags & TupMixMap::TUP_OUTPUT_ALPHA_FROM_RGB_INTENSITY) ? 1 : 0);
	pTexout->SetFlag(TEXOUT_COLOR_MAP,(flags & TupMixMap::TUP_OUTPUT_ENABLE_COLOR_MAP) ? 1 : 0);

	return pMap;
}

void TupImport::SetMapping(StdUV *pUVGen,int mappingType)
{
	int slotType = pUVGen->GetSlotType();
	int mapping = TupBitmapMap::TUP_MAP_EXPLICIT;

	switch(mappingType)
	{
		case TupBitmapMap::TUP_MAP_EXPLICIT:
			pUVGen->SetMapSlotType(MAPSLOT_TEXTURE);
			pUVGen->SetUVWSource(UVWSRC_EXPLICIT);
			break;
		case TupBitmapMap::TUP_MAP_VERTEX_COLOR:
			pUVGen->SetMapSlotType(MAPSLOT_TEXTURE);
			pUVGen->SetUVWSource(UVWSRC_EXPLICIT2);
			break;
		case TupBitmapMap::TUP_MAP_PLANAR_OBJECT:
			pUVGen->SetMapSlotType(MAPSLOT_TEXTURE);
			pUVGen->SetUVWSource(UVWSRC_OBJXYZ);
			break;
		case TupBitmapMap::TUP_MAP_PLANAR_WORLD:
			pUVGen->SetMapSlotType(MAPSLOT_TEXTURE);
			pUVGen->SetUVWSource(UVWSRC_WORLDXYZ);
			break;
		case TupBitmapMap::TUP_MAP_SPHERICAL:
			pUVGen->SetMapSlotType(MAPSLOT_ENVIRON);
			pUVGen->SetUVWSource(UVMAP_SPHERE_ENV);
			break;
		case TupBitmapMap::TUP_MAP_CYLINDRICAL:
			pUVGen->SetMapSlotType(MAPSLOT_ENVIRON);
			pUVGen->SetUVWSource(UVMAP_CYL_ENV);
			break;
		case TupBitmapMap::TUP_MAP_SHRINK_WRAP:
			pUVGen->SetMapSlotType(MAPSLOT_ENVIRON);
			pUVGen->SetUVWSource(UVMAP_SHRINK_ENV);
			break;
		case TupBitmapMap::TUP_MAP_SCREEN:
			pUVGen->SetMapSlotType(MAPSLOT_ENVIRON);
			pUVGen->SetUVWSource(UVMAP_SCREEN_ENV);
			break;
		default:
			assert(FALSE);
	}
}
