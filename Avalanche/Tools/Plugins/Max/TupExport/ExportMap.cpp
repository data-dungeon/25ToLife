////////////////////////////////////////////////////////////////////////////
//
// ExportMap
//
// Class to handle the maps
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportMap.cpp $
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 10/24/02   Time: 4:29p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * map fix
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 8/13/02    Time: 3:19p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * added mix map
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 5/21/02    Time: 1:55p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Fixed due to changes in keeper
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
#include "uvgen.h"

int TupExport::AddMap(Texmap *pMap)
{
	if (!pMap) return -1;
	int index = m_pMapPoolPtrs->GetIndex(pMap);
	Class_ID classID = pMap->ClassID();
	if (index==-1) // if it is not in the list
	{
		index = m_pMapPoolPtrs->Add(pMap);
		TupperwareAggregate *pMapAgg = m_pMapPool->AddMap(index);
		TupMap tupMap(pMapAgg);

		if (classID == Class_ID(BMTEX_CLASS_ID, 0)) 
		{
			tupMap.SetMapType(TupMap::BITMAP_MAP);
			AddBitmapMap(pMapAgg,(BitmapTex*)pMap);
		}
		else if (classID == Class_ID(COMPOSITE_CLASS_ID, 0)) 
		{
			tupMap.SetMapType(TupMap::COMPOSITE_MAP);
			AddCompositeMap(pMapAgg,(MultiTex*)pMap);
		}
		else if (classID == Class_ID(MIX_CLASS_ID, 0))
		{
			tupMap.SetMapType(TupMap::MIX_MAP);
			AddMixMap(pMapAgg,(MultiTex*)pMap);
		}
		else
		{
			tupMap.SetMapType(TupMap::UNKNOWN);
		}
	}
	return index;
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

void TupExport::AddBitmapMap(TupperwareAggregate *pMapAgg,BitmapTex *pMap)
{
	int numBlocks = pMap->NumParamBlocks();
	IParamBlock2 *pblock1 = pMap->GetParamBlock(0);
	IParamBlock2 *pblock2 = pMap->GetParamBlock(1);

	TupBitmapMap tupBitmapMap(pMapAgg);

	tupBitmapMap.AddName(pMap->GetName());
	tupBitmapMap.AddImageReference(AddImage(pMap->GetMapName()));

	StdUV* pUVGen = static_cast<StdUV *>(pMap->GetUVGen());
	assert(pUVGen);
	tupBitmapMap.AddMapping(GetMapping(pUVGen));
	tupBitmapMap.AddMapChannel(pUVGen->GetMapChannel());
	tupBitmapMap.AddUOffset(pUVGen->GetUOffs(m_pInterface->GetTime()));
	tupBitmapMap.AddVOffset(pUVGen->GetVOffs(m_pInterface->GetTime()));
	tupBitmapMap.AddUTiling(pUVGen->GetUScl(m_pInterface->GetTime()));
	tupBitmapMap.AddVTiling(pUVGen->GetVScl(m_pInterface->GetTime()));
	tupBitmapMap.AddMapAxis(pUVGen->GetAxis());
	tupBitmapMap.AddUAngle(pUVGen->GetUAng(m_pInterface->GetTime()));
	tupBitmapMap.AddVAngle(pUVGen->GetVAng(m_pInterface->GetTime()));
	tupBitmapMap.AddWAngle(pUVGen->GetWAng(m_pInterface->GetTime()));
	tupBitmapMap.AddBlur(pUVGen->GetBlur(m_pInterface->GetTime()));
	tupBitmapMap.AddBlurOffset(pUVGen->GetBlurOffs(m_pInterface->GetTime()));
	tupBitmapMap.AddNoiseAmount(pUVGen->GetNoiseAmt(m_pInterface->GetTime()));
	tupBitmapMap.AddNoiseLevels(pUVGen->GetNoiseLev(m_pInterface->GetTime()));
	tupBitmapMap.AddNoiseSize(pUVGen->GetNoiseSize(m_pInterface->GetTime()));
	tupBitmapMap.AddCroppingPlacementU(pblock1->GetFloat(bmtex_clipu,m_pInterface->GetTime()));
	tupBitmapMap.AddCroppingPlacementV(pblock1->GetFloat(bmtex_clipv,m_pInterface->GetTime()));
	tupBitmapMap.AddCroppingPlacementW(pblock1->GetFloat(bmtex_clipw,m_pInterface->GetTime()));
	tupBitmapMap.AddCroppingPlacementH(pblock1->GetFloat(bmtex_cliph,m_pInterface->GetTime()));
	tupBitmapMap.AddPlacementJitter(pblock1->GetFloat(bmtex_jitter,m_pInterface->GetTime()));
	tupBitmapMap.AddStartFrame(pblock2->GetInt(bmtex_start,m_pInterface->GetTime()));
	tupBitmapMap.AddPlaybackRate(pblock2->GetFloat(bmtex_playbackrate,m_pInterface->GetTime()));

	StdTexoutGen *pTexout = static_cast<StdTexoutGen *>(pMap->GetTexout());
	assert(pTexout);
	tupBitmapMap.AddOutputAmount(pTexout->GetOutputLevel(m_pInterface->GetTime()));
	tupBitmapMap.AddOutputRGBOffset(pTexout->GetRGBOff(m_pInterface->GetTime()));
	tupBitmapMap.AddOutputRGBLevel(pTexout->GetRGBAmt(m_pInterface->GetTime()));
	tupBitmapMap.AddOutputBumpAmount(pTexout->GetBumpAmt(m_pInterface->GetTime()));

	int flags = 0;
	if (pUVGen->GetTextureTiling()&U_WRAP) flags |= TupBitmapMap::TUP_U_WRAP;
	if (pUVGen->GetTextureTiling()&V_WRAP) flags |= TupBitmapMap::TUP_V_WRAP;
	if (pUVGen->GetTextureTiling()&U_MIRROR) flags |= TupBitmapMap::TUP_U_MIRROR;
	if (pUVGen->GetTextureTiling()&V_MIRROR) flags |= TupBitmapMap::TUP_V_MIRROR;
	if (pUVGen->GetFlag(UV_NOISE)) flags |= TupBitmapMap::TUP_NOISE;
	if (pUVGen->GetFlag(UV_NOISE_ANI)) flags |= TupBitmapMap::TUP_NOISE_ANIMATE;
	if (pUVGen->hideMapBack) flags |= TupBitmapMap::TUP_SHOW_MAP_ON_BACK;
	switch (pMap->GetFilterType())
	{
		case FILTER_PYR:
			flags |= TupBitmapMap::TUP_FILTER_TYPE_PYRAMID;
			break;
		case FILTER_SAT:
			flags |= TupBitmapMap::TUP_FILTER_TYPE_SUMMED_AREA;
			break;
		case FILTER_NADA:
			flags |= TupBitmapMap::TUP_FILTER_TYPE_NONE;
			break;
		default:
			assert(FALSE);
			break;
	}
	if (pblock1->GetInt(bmtex_monooutput,m_pInterface->GetTime())) flags |= TupBitmapMap::TUP_MONO_CHANNEL_ALPHA;
	if (pblock1->GetInt(bmtex_rgboutput,m_pInterface->GetTime())) flags |= TupBitmapMap::TUP_RGB_CHANNEL_ALPHA;
	switch (pblock1->GetInt(bmtex_alphasource,m_pInterface->GetTime()))
	{
		case ALPHA_FILE:
			flags |= TupBitmapMap::ALPHA_SOURCE_IMAGE_ALPHA;
			break;
		case ALPHA_RGB:
			flags |= TupBitmapMap::ALPHA_SOURCE_RGB_INTENSITY;
			break;
		case ALPHA_NONE:
			flags |= TupBitmapMap::ALPHA_SOURCE_NONE;
			break;
	}
	if (pblock1->GetInt(bmtex_premultalpha,m_pInterface->GetTime())) flags |= TupBitmapMap::TUP_PREMULTIPLIED_ALPHA;
	if (pblock1->GetInt(bmtex_apply,m_pInterface->GetTime()))
	{
		if (pblock1->GetInt(bmtex_crop_place,m_pInterface->GetTime())) flags |= TupBitmapMap::CROPPING_PLACEMENT_PLACEMENT;
		else flags |= TupBitmapMap::CROPPING_PLACEMENT_CROPPING;
	}
	else flags |= TupBitmapMap::CROPPING_PLACEMENT_DISABLED;
	if (pblock1->GetInt(bmtex_usejitter,m_pInterface->GetTime())) flags |= TupBitmapMap::TUP_PLACEMENT_JITTER_ENABLED;
	switch (pblock2->GetInt(bmtex_endcondition,m_pInterface->GetTime()))
	{
		case 0:
			flags |= TupBitmapMap::ANIMATION_END_CONDITION_LOOP;
			break;
		case 1: 
			flags |= TupBitmapMap::ANIMATION_END_CONDITION_PING_PONG;
			break;
		case 2:
			flags |= TupBitmapMap::ANIMATION_END_CONDITION_HOLD;
			break;
	}
	if (pTexout->GetInvert()) flags |= TupBitmapMap::TUP_OUTPUT_INVERT;
	if (pTexout->GetClamp()) flags |= TupBitmapMap::TUP_OUTPUT_CLAMP;
	if (pTexout->GetAlphaFromRGB()) flags |= TupBitmapMap::TUP_OUTPUT_ALPHA_FROM_RGB_INTENSITY;
	if (pTexout->GetFlag(TEXOUT_COLOR_MAP)) flags |= TupBitmapMap::TUP_OUTPUT_ENABLE_COLOR_MAP;

	tupBitmapMap.AddFlags(flags);
}

void TupExport::AddCompositeMap(TupperwareAggregate *pMapAgg,MultiTex *pMap)
{
//	int numBlocks = pMap->NumParamBlocks();
//	IParamBlock2 *pblock1 = pMap->GetParamBlock(0);
//	IParamBlock2 *pblock2 = pMap->GetParamBlock(1);
	TupCompositeMap tupCompositeMap(pMapAgg);

	tupCompositeMap.AddName(pMap->GetName());

	int numMaps = pMap->NumSubTexmaps();
	for (int i=0;i<numMaps; i++) 
	{
		Texmap *texMap = pMap->GetSubTexmap(i);
		if (texMap)
		{
			TupperwareAggregate *pSubMapAgg = tupCompositeMap.AddSubMap(i);
			TupCompositeMapSubMap tupSubMap(pSubMapAgg);
			int mapFlags = 0;
//			if (pMap->mapOn[i]) mapFlags |= TupCompositeMapSubMap::ENABLED;
			tupSubMap.AddFlags(mapFlags);
			tupSubMap.AddMapReference(AddMap(texMap));
		}
	}
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

void TupExport::AddMixMap(TupperwareAggregate *pMapAgg,MultiTex *pMap)
{
	int numBlocks = pMap->NumParamBlocks();
	IParamBlock2 *pblock = pMap->GetParamBlock(0);

	TupMixMap tupMixMap(pMapAgg);
	tupMixMap.AddName(pMap->GetName());
	Texmap *texMap1 = pMap->GetSubTexmap(0);
	if (texMap1)
	{
		tupMixMap.AddMap1Reference(AddMap(texMap1));
	}
	Texmap *texMap2 = pMap->GetSubTexmap(1);
	if (texMap2)
	{
		tupMixMap.AddMap2Reference(AddMap(texMap2));
	}
	Texmap *texMixMap = pMap->GetSubTexmap(2);
	if (texMixMap)
	{
		tupMixMap.AddMixMapReference(AddMap(texMixMap));
	}

	tupMixMap.AddMixAmount(pblock->GetFloat(mix_mix,m_pInterface->GetTime()));
	tupMixMap.AddLowerTransitionZone(pblock->GetFloat(mix_curvea,m_pInterface->GetTime()));
	tupMixMap.AddUpperTransitionZone(pblock->GetFloat(mix_curveb,m_pInterface->GetTime()));

	StdTexoutGen *pTexout = static_cast<StdTexoutGen *>(pMap->GetReference(TEXOUT_REF));
	assert(pTexout);
	tupMixMap.AddOutputAmount(pTexout->GetOutputLevel(m_pInterface->GetTime()));
	tupMixMap.AddOutputRGBOffset(pTexout->GetRGBOff(m_pInterface->GetTime()));
	tupMixMap.AddOutputRGBLevel(pTexout->GetRGBAmt(m_pInterface->GetTime()));
	tupMixMap.AddOutputBumpAmount(pTexout->GetBumpAmt(m_pInterface->GetTime()));

	int flags = 0;
	if (pblock->GetInt( mix_map1_on,m_pInterface->GetTime())) flags |= TupMixMap::MAP_1_ENABLE;
	if (pblock->GetInt( mix_map2_on,m_pInterface->GetTime())) flags |= TupMixMap::MAP_2_ENABLE;
	if (pblock->GetInt( mix_mask_on,m_pInterface->GetTime())) flags |= TupMixMap::MIX_MAP_ENABLE;
	if (pblock->GetInt( mix_usecurve,m_pInterface->GetTime())) flags |= TupMixMap::USE_CURVE;
	if (pTexout->GetInvert()) flags |= TupMixMap::TUP_OUTPUT_INVERT;
	if (pTexout->GetClamp()) flags |= TupMixMap::TUP_OUTPUT_CLAMP;
	if (pTexout->GetAlphaFromRGB()) flags |= TupMixMap::TUP_OUTPUT_ALPHA_FROM_RGB_INTENSITY;
	if (pTexout->GetFlag(TEXOUT_COLOR_MAP)) flags |= TupMixMap::TUP_OUTPUT_ENABLE_COLOR_MAP;

	tupMixMap.AddFlags(flags);

}

int TupExport::GetMapping(StdUVGen *pUVGen)
{
	int slotType = pUVGen->GetSlotType();
	int mapping = TupBitmapMap::TUP_MAP_EXPLICIT;

	switch (slotType)
	{
		case MAPSLOT_TEXTURE:
		case MAPSLOT_DISPLACEMENT:
			switch (pUVGen->GetUVWSource())
			{
				case UVWSRC_EXPLICIT: 
					mapping = TupBitmapMap::TUP_MAP_EXPLICIT;
					break;
				case UVWSRC_EXPLICIT2:
					mapping = TupBitmapMap::TUP_MAP_VERTEX_COLOR;
					break;
				case UVWSRC_OBJXYZ:
					mapping = TupBitmapMap::TUP_MAP_PLANAR_OBJECT;
					break;
				case UVWSRC_WORLDXYZ:
//				case UVWSRC_GEOXYZ: // same as world for now
					mapping = TupBitmapMap::TUP_MAP_PLANAR_WORLD;
					break;
				default:
					assert(FALSE); // assert if different
			}
			break;
		case MAPSLOT_ENVIRON:
		case MAPSLOT_BACKGROUND:
			switch (pUVGen->GetCoordMapping(0))
			{
				case UVMAP_EXPLICIT:
					mapping = TupBitmapMap::TUP_MAP_EXPLICIT;
					break;
				case UVMAP_SPHERE_ENV:
					mapping = TupBitmapMap::TUP_MAP_SPHERICAL;
					break;
				case UVMAP_CYL_ENV:
					mapping = TupBitmapMap::TUP_MAP_CYLINDRICAL;
					break;
				case UVMAP_SHRINK_ENV:
					mapping = TupBitmapMap::TUP_MAP_SHRINK_WRAP;
					break;
				case UVMAP_SCREEN_ENV:
					mapping = TupBitmapMap::TUP_MAP_SCREEN;
					break;
			}
	}
	return mapping;
}
