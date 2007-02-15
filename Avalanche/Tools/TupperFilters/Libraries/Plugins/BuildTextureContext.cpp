////////////////////////////////////////////////////////////////////////////
//
// BuildTextureContext
//
// Build the texture contexts
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: BuildTextureContext.cpp $
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 7/03/03    Time: 10:08a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * use map attributes for point sampling and mip mapping
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 6/23/03    Time: 10:24a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 6/20/03    Time: 1:39p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 11:48a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * update for streaming system
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 3/11/03    Time: 11:49a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * oops on reverse
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 3/10/03    Time: 12:26p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * correct wrap flags on reversed image
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 2/07/03    Time: 2:59p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * environment mapping
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 1/30/03    Time: 10:51a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * diffuse used for multiply
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 1/29/03    Time: 12:09p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * new blending mode stuff (for multitexturing)
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 9/25/02    Time: 4:32p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * duplicate images remapped rather than removed
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 9/25/02    Time: 11:14a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Color init problem
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 9/20/02    Time: 12:55p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * moved texture context manager to plugins library
 * added ps2 flag
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 9/17/02    Time: 10:00a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * Multi-texturing first pass complete
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

#include "PluginsPch.h"
#include "BuildTextureContext.h"
#include "DataManager.h"
#include "ImageManager.h"
#include "AnnotateHelper.h"


BuildTextureContextManager::BuildTextureContextManager(FilterDataManager &dataManager,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,
																		 int currentPartition,bool bPs2) :
	m_dataManager(dataManager),
	m_tupMaterialPool(tupMaterialPool),
	m_tupMapPool(tupMapPool),
	m_tupImagePool(tupImagePool),
	m_bPs2(bPs2),
	m_currentPartition(currentPartition)
{
}

void BuildTextureContextManager::BuildTextureContext(TupMaterial &tupMaterial,ts_FullTextureContext &fullTextureContext)
{
	// only handle a standard material for now
	if (tupMaterial.GetMaterialType() == TupMaterial::STANDARD_MATERIAL)
	{
		// simply add it if it's a diffuse map
		TupStandardMaterial tupStandardMaterial(tupMaterial);

		// start with white base color
		fullTextureContext.BaseColor.R = 255;
		fullTextureContext.BaseColor.G = 255;
		fullTextureContext.BaseColor.B = 255;
		fullTextureContext.BaseColor.A = 255;
		fullTextureContext.TextureCount = 0; // start with no textures

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
				AddMap(MAP_TYPE_DIFFUSE_COLOR,mapRef,fullTextureContext);
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
				AddMap(MAP_TYPE_REFLECTION,mapRef,fullTextureContext);
			}
		}

		// if no textures then set to a dummy
		if (fullTextureContext.TextureCount == 0)
		{
			BuildDummyTextureContext(fullTextureContext);
		}
	}
	else
	{
		BuildDummyTextureContext(fullTextureContext);
	}
}

void BuildTextureContextManager::BuildDummyTextureContext(ts_FullTextureContext &fullTextureContext)
{
	fullTextureContext.BaseColor.R = 255;
	fullTextureContext.BaseColor.G = 255;
	fullTextureContext.BaseColor.B = 255;
	fullTextureContext.BaseColor.A = 255;
	// ps2 needs a texture even if there is not one
	if (m_bPs2)
	{
		fullTextureContext.TextureCount = 1;
		fullTextureContext.TextureLayer[0].s16TextureIndex = -1; // default to no bitmap
		fullTextureContext.TextureLayer[0].u8Flags = TEXTURE_USE_WHITE_TEXTURE; 
		strcpy(fullTextureContext.TextureLayer[0].TextureName, "unknown");
		fullTextureContext.TextureLayer[0].TextureName[TEXTURE_LAYER_NAME_LENGTH - 1] = 0;	// make sure it's null-terminated
		fullTextureContext.TextureLayer[0].u8Blend = RS_BLENDFUNC_DEFAULT;
		fullTextureContext.TextureLayer[0].u8Filter = TS_FILTER_BILINEAR;
		fullTextureContext.TextureLayer[0].u8Method = TS_METHOD_PASSTHRU;
		fullTextureContext.TextureLayer[0].u8WrapU = TS_WRAP_REPEAT;
		fullTextureContext.TextureLayer[0].u8WrapV = TS_WRAP_REPEAT;
	}
	// the other platforms can handle a texture context with no textures
	else
	{
		fullTextureContext.TextureCount = 0;
	}
}


void BuildTextureContextManager::AddMap(BuildTextureContextManager::MapTypes mapType,int mapReference,ts_FullTextureContext &fullTextureContext)
{
	assert(mapReference!=-1);
	// if we can't add any more then stop
	if (fullTextureContext.TextureCount==MAX_SOM_TEXTURES_PER_PATCH)
		return;

	TupperwareAggregate *pMapAgg = m_tupMapPool.GetMap(mapReference);
	TupMap tupMap(pMapAgg);
	switch (tupMap.GetMapType())
	{
		case TupMap::BITMAP_MAP:
			{
				int layerIndex = fullTextureContext.TextureCount;

				TupBitmapMap tupBitmapMap(tupMap);

				char *pMapName;
				tupBitmapMap.GetName(&pMapName);
				strncpy(fullTextureContext.TextureLayer[layerIndex].TextureName, pMapName, TEXTURE_LAYER_NAME_LENGTH);
				fullTextureContext.TextureLayer[layerIndex].TextureName[TEXTURE_LAYER_NAME_LENGTH - 1] = 0;	// make sure it's null-terminated

				int imageRef;
				tupBitmapMap.GetImageReference(imageRef);
				TupperwareAggregate *pImageAgg = m_tupImagePool.GetImage(imageRef);
				assert(pImageAgg);
				// remap to other image ref (duplicate image data)
				TupperwareScalar *pRemapImageRefScalar = pImageAgg->FindScalarByKey("Annotate_RemapImageRef");
				if (pRemapImageRefScalar)
				{
					imageRef = pRemapImageRefScalar->GetAsInt();
					pImageAgg = m_tupImagePool.GetImage(imageRef);
				}
				bool bReversedImage = false;
				TupperwareScalar *pRotatedImageScalar = pImageAgg->FindScalarByKey("Annotate_ImageReversed");
				if (pRotatedImageScalar)
					bReversedImage = true;

				bool bPointSampling = false;
				TupperwareScalar *pPointSampledScalar = pMapAgg->FindScalarByKey("Annotate_PointSampled"); 
				if (pPointSampledScalar)
					bPointSampling = true;

				bool bMipMapping = false;
				TupperwareScalar *pMipMappingScalar = pImageAgg->FindScalarByKey("Annotate_MipMapping");
				if (pMipMappingScalar)
					bMipMapping = true;

				bool bCrossFade = false;
				TupperwareScalar *pCrossFadeScalar = pImageAgg->FindScalarByKey("Annotate_MipMapCrossFade");
				if (pCrossFadeScalar)
					bCrossFade = pCrossFadeScalar->GetAsInt() ? true : false;

				// get blend mode 
				int blendMode;
				TupperwareScalar *pBlendModeScalar = tupBitmapMap.GetBlendMode(blendMode);
				// if we can't find this set to default
				if (!pBlendModeScalar)
					blendMode = TupBitmapMap::BLEND_MODE_OVER; // default				

				TupperwareList *pTextureIDList = pImageAgg->FindListByKey("Annotate_TextureIDs");
				if (pTextureIDList)
				{
					// get id for partition
					int *pTextureIDs = pTextureIDList->GetAsInt();
					int textureID = pTextureIDs[m_currentPartition];
					assert(textureID!=-1);

					int mapFlags;
					tupBitmapMap.GetFlags(mapFlags);

					fullTextureContext.TextureLayer[layerIndex].u8Blend = (u8) GetBlend(mapType,blendMode);
					fullTextureContext.TextureLayer[layerIndex].u8Filter = (u8) GetFilter(bPointSampling,bMipMapping,bCrossFade);
					fullTextureContext.TextureLayer[layerIndex].u8Method = (u8) GetMethod(mapType,blendMode);
					fullTextureContext.TextureLayer[layerIndex].s16TextureIndex = textureID; // use the texture ID from the annotate filter
					fullTextureContext.TextureLayer[layerIndex].u8Flags = 0; // no white texture

					// mark texture as diffuse map if using the diffuse blend
					if (fullTextureContext.TextureLayer[layerIndex].u8Blend ==  RS_BLENDFUNC_DIFFUSE)
						fullTextureContext.TextureLayer[layerIndex].u8Flags |= TEXTURE_IS_DIFFUSE_MAP;

					// mark texture as environment map if using the reflection map type
					if (mapType == BuildTextureContextManager::MAP_TYPE_REFLECTION)
						fullTextureContext.TextureLayer[layerIndex].u8Flags |= TEXTURE_IS_ENVIRONMENT_MAP;

					if (bReversedImage) // flip up wrap flags around due to rotated image
					{
						if (mapFlags&TupBitmapMap::TUP_U_WRAP)
							fullTextureContext.TextureLayer[layerIndex].u8WrapV = TS_WRAP_REPEAT;
						else
							fullTextureContext.TextureLayer[layerIndex].u8WrapV = TS_WRAP_CLAMP;
						if (mapFlags&TupBitmapMap::TUP_V_WRAP)
							fullTextureContext.TextureLayer[layerIndex].u8WrapU = TS_WRAP_REPEAT;
						else
							fullTextureContext.TextureLayer[layerIndex].u8WrapU = TS_WRAP_CLAMP;
					}
					else
					{
						if (mapFlags&TupBitmapMap::TUP_U_WRAP)
							fullTextureContext.TextureLayer[layerIndex].u8WrapU = TS_WRAP_REPEAT;
						else
							fullTextureContext.TextureLayer[layerIndex].u8WrapU = TS_WRAP_CLAMP;
						if (mapFlags&TupBitmapMap::TUP_V_WRAP)
							fullTextureContext.TextureLayer[layerIndex].u8WrapV = TS_WRAP_REPEAT;
						else
							fullTextureContext.TextureLayer[layerIndex].u8WrapV = TS_WRAP_CLAMP;
					}

					// get has alpha flag on the texture image
					TupImage tupImage(pImageAgg);
					ImageManager &imageManager = m_dataManager.GetImageManager();
					AnnotateHelper::ImageInfo imageInfo;
					AnnotateHelper::GetImageInfo(tupImage,0,0,imageManager,imageInfo);
					if (imageInfo.m_formatFlags & TupImageDataFrame::HAS_ALPHA)
					{
						fullTextureContext.TextureLayer[layerIndex].u8Flags |= TEXTURE_HAS_ALPHA;
					}
				}
				else
				{
					fullTextureContext.TextureLayer[layerIndex].s16TextureIndex = -1; // default to no bitmap
					fullTextureContext.TextureLayer[layerIndex].u8Flags = TEXTURE_USE_WHITE_TEXTURE; 
					strcpy(fullTextureContext.TextureLayer[layerIndex].TextureName, "unknown");
					fullTextureContext.TextureLayer[layerIndex].TextureName[TEXTURE_LAYER_NAME_LENGTH - 1] = 0;	// make sure it's null-terminated
					fullTextureContext.TextureLayer[layerIndex].u8Blend = (u8) GetBlend(mapType,blendMode);
					fullTextureContext.TextureLayer[layerIndex].u8Filter = (u8) GetFilter(false,false,false);
					fullTextureContext.TextureLayer[layerIndex].u8Method = (u8) GetMethod(mapType,blendMode);
					fullTextureContext.TextureLayer[layerIndex].u8WrapU = TS_WRAP_REPEAT;
					fullTextureContext.TextureLayer[layerIndex].u8WrapV = TS_WRAP_REPEAT;
				}
				fullTextureContext.TextureCount++;
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
					AddMap(mapType,mapRef,fullTextureContext);
				}
			}
			break;
		default:
			assert(false);
			break;
	}
}

int BuildTextureContextManager::GetBlend(BuildTextureContextManager::MapTypes mapType,int blendMode)
{
	int blend = RS_BLENDFUNC_DEFAULT;

	switch (blendMode)
	{
		case TupBitmapMap::BLEND_MODE_NONE:
			blend = RS_BLENDFUNC_NONE;
			break;
		case TupBitmapMap::BLEND_MODE_OVER:
			blend = RS_BLENDFUNC_DEFAULT;
			break;
		case TupBitmapMap::BLEND_MODE_IN:
			blend = RS_BLENDFUNC_DEFAULT;
			break;
		case TupBitmapMap::BLEND_MODE_OUT:
			blend = RS_BLENDFUNC_DEFAULT;
			break;
		case TupBitmapMap::BLEND_MODE_ADD:
			blend = RS_BLENDFUNC_ADDITIVE;
			break;
		case TupBitmapMap::BLEND_MODE_SUBTRACT:
			blend = RS_BLENDFUNC_SUBTRACTIVE;
			break;
		case TupBitmapMap::BLEND_MODE_MULTIPLY:
			blend = RS_BLENDFUNC_DIFFUSE;
			break;
		case TupBitmapMap::BLEND_MODE_DIFFERENCE:
			blend = RS_BLENDFUNC_DEFAULT;
			break;
		case TupBitmapMap::BLEND_MODE_LIGHTEN:
			blend = RS_BLENDFUNC_DEFAULT;
			break;
		case TupBitmapMap::BLEND_MODE_DARKEN:
			blend = RS_BLENDFUNC_DEFAULT;
			break;
		case TupBitmapMap::BLEND_MODE_SATURATE:
			blend = RS_BLENDFUNC_SATURATE;
			break;
		case TupBitmapMap::BLEND_MODE_DESATURATE:
			blend = RS_BLENDFUNC_DEFAULT;
			break;
		case TupBitmapMap::BLEND_MODE_ILLUMINATE:
			blend = RS_BLENDFUNC_DEFAULT;
			break;
	}
	return blend;
}

int BuildTextureContextManager::GetFilter(bool bPointSampling,bool bMipMapping,bool bCrossFade)
{
	if (bMipMapping)
	{
		if (bCrossFade)
		{
			if (bPointSampling)
				return TS_FILTER_LINEAR_MIPMAP_NEAREST;	// point-sampled, choose nearest mipmap texture
			else
				return TS_FILTER_LINEAR_MIPMAP_LINEAR;
		}
		else
		{
			if (bPointSampling)
				return TS_FILTER_NEAREST_MIPMAP_NEAREST;	// point-sampled, choose nearest mipmap texture
			else
				return TS_FILTER_NEAREST_MIPMAP_LINEAR;
		}
	}
	else
	{
		if (bPointSampling)
			return TS_FILTER_POINTSAMPLE;	// point-sampled, choose nearest mipmap texture
		else
			return TS_FILTER_BILINEAR;
	}
}

int BuildTextureContextManager::GetMethod(BuildTextureContextManager::MapTypes mapType,int blendMode)
{
	return TS_METHOD_PASSTHRU;
}


