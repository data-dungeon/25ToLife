////////////////////////////////////////////////////////////////////////////
//
// TextureInfo
//
// Handle any texture information for the PS2
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TextureInfo.cpp $
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 7/03/03    Time: 10:05a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * mip map support
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 3/13/03    Time: 10:09a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * texture id bug
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 4:46p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 4:34p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 2/18/03    Time: 12:57p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * write number of bytes into log
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 9/27/02    Time: 3:23p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * copy only the parent palette
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 9/10/02    Time: 11:38a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * palette merging
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 9/06/02    Time: 3:38p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * Using textureIDs rather than image indexes
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 8/20/02    Time: 3:19p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * Warning on VRAM space overflow
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 8/16/02    Time: 3:35p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * save memory usage in log file
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 6/04/02    Time: 1:22p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/08/02    Time: 1:46p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * Texture sets and vram locations update
 */

#include "ExportPch.h"
#include "textureinfo.h"
#include "Display/PS2/GSMemManager.h"
#include "Display/PS2/PSDRenderState.h" // for RS_TEX_PSM*


PS2TextureInfo::PS2TextureInfo(const FilterOptions &options,FilterDataManager &dataManager,
   							 TupScene &tupScene,TupObjectPool &tupObjectPool,TupImagePool &tupImagePool,int currentPartition) :
	m_options(options),
	m_dataManager(dataManager),
	m_tupScene(tupScene),
	m_tupObjectPool(tupObjectPool),
	m_tupImagePool(tupImagePool),
	m_totalTextures(0),
	m_numTextureSets(0),
	m_currentPartition(currentPartition)
{
}

PS2TextureInfo::~PS2TextureInfo()
{
}


void PS2TextureInfo::AnalyzeTextures(void)
{
	int *totalTexturesArray = m_tupScene.GetBaseAggregate()->FindListByKey("Annotate_TotalTextures")->GetAsInt();
	m_totalTextures = totalTexturesArray[0]; // change later
	if (m_totalTextures==0)
	{
		return;
	}
	m_textureDataArray.SetSize(m_totalTextures);

	TupperwareAggregate *pSceneAgg = m_tupScene.GetBaseAggregate();
	m_numTextureSets = pSceneAgg->FindScalarByKey("Annotate_TotalTextureSets")->GetAsInt();
	assert(m_numTextureSets);

	m_VRAMBlockSizes.SetSize(m_numTextureSets);

	u32 totalVRAMBlockSize = 0;

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"PS2 Texture Set Information");

	for(int subset = 0; subset < m_numTextureSets; subset++)
	{
		// empty out the allocation info for gs mem...
		GSMemManager::Reset();
		int imageIndex;

		// allocate space for textures...
		int numSubSetImages = 0;
		int numImages=m_tupImagePool.GetNumImages();
		for (imageIndex = 0; imageIndex < numImages; imageIndex++)
		{
			TupperwareAggregate *pImageAgg = m_tupImagePool.GetImage(imageIndex);
			if (!pImageAgg)
				continue;

			int *pTextureIDs = pImageAgg->FindListByKey("Annotate_TextureIDs")->GetAsInt();
			int textureID = pTextureIDs[m_currentPartition];  // use first partition for now
			// if -1 or referencing the world partition
			if (textureID==-1 || (textureID&0xff000000)!=0)
				continue;

			int textureSet = pImageAgg->FindScalarByKey("Annotate_TextureSet")->GetAsInt();
			// is this texture in the subset we are processing...
			if(textureSet != subset)
				continue;

			int numMaps = 1;
			TupperwareScalar *pMipMappingScalar = pImageAgg->FindScalarByKey("Annotate_MipMapping");
			// does this one have mip maps
			if (pMipMappingScalar)
			{
				numMaps += pImageAgg->FindScalarByKey("Annotate_MipMapNumSubMaps")->GetAsInt();
			}

			m_textureDataArray[textureID].SetSize(numMaps);

			TupImage tupImage(pImageAgg);
			AllocateVRAMSpaceForTexture(tupImage,textureID);
			numSubSetImages++;
		}

		// allocate space for palettes
		for (imageIndex = 0; imageIndex < numImages; imageIndex++)
		{
			TupperwareAggregate *pImageAgg = m_tupImagePool.GetImage(imageIndex);
			if (!pImageAgg)
				continue;

			int *pTextureIDs = pImageAgg->FindListByKey("Annotate_TextureIDs")->GetAsInt();
			int textureID = pTextureIDs[m_currentPartition];
			// if -1 or referencing the world partition
			if (textureID==-1 || (textureID&0xff000000)!=0)
				continue;

			int textureSet = pImageAgg->FindScalarByKey("Annotate_TextureSet")->GetAsInt();

			// is this texture in the subset we are processing...
			if(textureSet != subset)
				continue;

			TupImage tupImage(pImageAgg);
			AllocateVRAMSpaceForPalette(tupImage,textureID);
		}

		/* record VRAM block size of entire texture subset */
		m_VRAMBlockSizes[subset] = GSMemManager::GetHighestBlockUsed() + 1;
		totalVRAMBlockSize += m_VRAMBlockSizes[subset];
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Texture Set %d of %d uses %d images.  VRAM Block Size = %d (%d bytes)",subset+1,m_numTextureSets,numSubSetImages,m_VRAMBlockSizes[subset],m_VRAMBlockSizes[subset]*256);
	}
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Total VRAM Block Size for Texture Sets = %d (%d bytes)",totalVRAMBlockSize,totalVRAMBlockSize*256);
}

int PS2TextureInfo::GetNumTextures(void) const
{
	return m_totalTextures;
}

int PS2TextureInfo::GetNumMaps(int textureID) const
{
	assert(textureID>=0 && textureID<m_totalTextures);
	return m_textureDataArray[textureID].GetSize();
}

const PS2TextureInfo::AnalyzedTextureData &PS2TextureInfo::GetAnalyzedTextureData(int textureID,int mapIndex) const
{
	assert(textureID>=0 && textureID<m_totalTextures);
	assert(mapIndex>=0 && mapIndex<m_textureDataArray[textureID].GetSize());
	return m_textureDataArray[textureID][mapIndex];
}

int PS2TextureInfo::GetNumTextureSets(void) const
{
	return m_numTextureSets;
}

u32 PS2TextureInfo::GetTextureSetVRAMBlockSize(int setNum) const
{
	assert(setNum>=0 && setNum<m_numTextureSets);
	return m_VRAMBlockSizes[setNum];
}

u32 PS2TextureInfo::GetPS2Format(int formatFlags)
{
	switch(formatFlags&TupImageDataFrame::FORMAT_MASK)
	{
		case TupImageDataFrame::RGBA_8888:
			return RS_TEX_PSMCT32;
		case TupImageDataFrame::RGB_888:
			return RS_TEX_PSMCT24;
		case TupImageDataFrame::RGB_555:
		case TupImageDataFrame::RGB_565:
		case TupImageDataFrame::RGBA_1555:
			return RS_TEX_PSMCT16;
		case TupImageDataFrame::INDEXED_8:
			return RS_TEX_PSMT8;
		case TupImageDataFrame::INDEXED_4:
			return RS_TEX_PSMT4;
		default:
			assert(false);
	}
	return -1;
}

void PS2TextureInfo::AllocateVRAMSpaceForTexture(TupImage &tupImage,int textureID) 
{
	// collect some info about the texture..
	u32 width, height, format;
	int numMaps = m_textureDataArray[textureID].GetSize();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		PS2TextureInfo::AnalyzedTextureData &analyzedData = m_textureDataArray[textureID][mapIndex];

		TupperwareAggregate *pImageAgg = tupImage.GetBaseAggregate();
		AnnotateHelper::ImageInfo imageInfo;
		AnnotateHelper::GetImageInfo(tupImage,0,mapIndex,m_dataManager.GetImageManager(),imageInfo);
		width = imageInfo.m_width;
		height = imageInfo.m_height;

		format = GetPS2Format(imageInfo.m_formatFlags);
		if(format == 0xFFFFFFFF)
			throw(PluginException(ERROR_FATAL,"Bad Texture Export Format"));

		// do the allocation
		GSMemManager::allocationInfo info;
		GSMemManager::GetAllocationInfo(width,height,width,format, false, info);

		analyzedData.m_textureBlock = GSMemManager::BlockAlloc(info);

		if(analyzedData.m_textureBlock == 0xFFFFFFFF)
		{
			char *pFilePathName;
			tupImage.GetFilePathName(&pFilePathName);
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Warning - Out of PS2 VRAM Space Image %s map %d width %d height %d",pFilePathName,mapIndex,width,height);
		}
	}
}

void PS2TextureInfo::AllocateVRAMSpaceForPalette(TupImage &tupImage,int textureID) 
{
	// collect some info about the texture..
	u32 format;
	u32 paletteBlock = 0; // default

	AnnotateHelper::ImageInfo imageInfo;
	AnnotateHelper::GetImageInfo(tupImage,0,0,m_dataManager.GetImageManager(),imageInfo);

	format = GetPS2Format(imageInfo.m_formatFlags);
	if(format == 0xFFFFFFFF)
		throw(PluginException(ERROR_FATAL,"Bad Texture Export Format"));

	if (format == RS_TEX_PSMT4 || format == RS_TEX_PSMT8)
	{
		TupperwareScalar *pParentPaletteReferenceScalar = tupImage.GetBaseAggregate()->FindScalarByKey("Annotate_ParentPaletteReference");
		if (pParentPaletteReferenceScalar)
		{
			int parentPaletteReference = pParentPaletteReferenceScalar->GetAsInt();
			TupperwareAggregate *pParentImageAgg = m_tupImagePool.GetImage(parentPaletteReference);
			// get texture id (this is the index into the pTextureChunkRecordArray
			int *pParentTextureIDs = pParentImageAgg->FindListByKey("Annotate_TextureIDs")->GetAsInt();
			int parentTextureID = pParentTextureIDs[m_currentPartition];  // get parent id from same sartition
			assert(parentTextureID!=-1);
			assert(parentTextureID<textureID); // this better be before me
			paletteBlock = m_textureDataArray[parentTextureID][0].m_paletteBlock; // copy the palette block
		}
		else
		{
			GSMemManager::allocationInfo info;

			// do the allocation
			if(format == RS_TEX_PSMT4)
				GSMemManager::GetAllocationInfo(8, 2, 8, RS_TEX_PSMCT32, false, info);
			else if(format == RS_TEX_PSMT8)
				GSMemManager::GetAllocationInfo(16, 16, 16, RS_TEX_PSMCT32, false, info);

			paletteBlock = GSMemManager::BlockAlloc(info);

			if(paletteBlock == 0xFFFFFFFF)
			{
				char *pFilePathName;
				tupImage.GetFilePathName(&pFilePathName);
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
					"Warning - Out of PS2 VRAM Space Allocating Palette - Image %s",pFilePathName);
			}
		}
	}
	// use for all maps
	int numMaps = m_textureDataArray[textureID].GetSize();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		m_textureDataArray[textureID][mapIndex].m_paletteBlock = paletteBlock;
	}
}

