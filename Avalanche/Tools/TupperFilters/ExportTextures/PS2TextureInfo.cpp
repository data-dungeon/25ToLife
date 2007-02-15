////////////////////////////////////////////////////////////////////////////
//
// TextureInfo
//
// Handle any texture information for the PS2
//
////////////////////////////////////////////////////////////////////////////

#include "ExportTexturesPch.h"
#include "ProcessTup.h"
#include "PS2TextureInfo.h"
#include "Display/PS2/GSMemManager.h"
#include "Display/PS2/PSDRenderState.h" // for RS_TEX_PSM*

PS2TextureInfo::PS2TextureInfo(TupArray<TextureEntry> &textureEntries,TupperwareAggregate *pRootAgg) :
	m_textureEntries(textureEntries),
	m_pRootAgg(pRootAgg)
{
}

PS2TextureInfo::~PS2TextureInfo()
{
}

void PS2TextureInfo::AnalyzeTextures(void)
{
	int numTextures = m_textureEntries.GetSize();
	if (numTextures == 0)
	{
		return;
	}

	m_textureDataArray.SetSize(m_totalTextures);

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"PS2 Texture Set Information");

	// empty out the allocation info for gs mem...
	GSMemManager::Reset();
	int textureIndex;

	for (textureIndex = 0; textureIndex < numTextures; textureIndex++)
	{
		TextureEntry &textureEntry = m_textureEntries[textureIndex];
      TupperwareAggregate *pTextureAgg = textureEntry.m_pTextureAgg;

		int numMaps = 1;
		TupperwareScalar *pMipMappingScalar = pTextureAgg->FindScalarByKey("ExportTextures_MipMapping");
		// does this one have mip maps
		if (pMipMappingScalar)
		{
			numMaps += pTextureAgg->FindScalarByKey("ExportTextures_MipMapNumSubMaps")->GetAsInt();
		}

		m_textureDataArray[textureIndex].SetSize(numMaps);

		AllocateVRAMSpaceForTexture(textureEntry,textureIndex);
	}

	// allocate space for palettes
	for (textureIndex = 0; textureIndex < numTextures; textureIndex++)
	{
		TextureEntry &textureEntry = m_textureEntries[textureIndex];
		TupperwareAggregate *pTextureAgg = textureEntry.m_pTextureAgg;

		AllocateVRAMSpaceForPalette(textureEntry,textureIndex);
	}

	/* record VRAM block size of entire texture subset */
	m_VRAMBlockSize = GSMemManager::GetHighestBlockUsed() + 1;
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Texture Set uses %d Textures.  VRAM Block Size = %d (%d bytes)",m_totalTextures,m_VRAMBlockSize,m_VRAMBlockSize*256);
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

u32 PS2TextureInfo::GetTextureSetVRAMBlockSize(void) const
{
	return m_VRAMBlockSize;
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

void PS2TextureInfo::AllocateVRAMSpaceForTexture(TextureEntry &textureEntry,int textureIndex) 
{
	// collect some info about the texture..
	u32 width, height, format;
	int numMaps = m_textureDataArray[textureIndex].GetSize();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		PS2TextureInfo::AnalyzedTextureData &analyzedData = m_textureDataArray[textureIndex][mapIndex];

		TupperwareScalar *pRemapImageReferenceScalar = textureEntry.m_pTextureAgg->FindScalarByKey("ExportTextures_RemapImageRef");
		if (pRemapImageReferenceScalar)
		{
			int parentImageReference = pRemapImageReferenceScalar->GetAsInt();
			PS2TextureInfo::AnalyzedTextureData &parentAnalyzedData = m_textureDataArray[parentImageReference][mapIndex];
			// use the same date as my parent
			analyzedData.m_textureBlock = parentAnalyzedData.m_textureBlock;
			continue;
		}

		width = textureEntry.m_pTextureAgg->FindScalarByKey("ExportTextures_TextureWidth")->GetAsInt();
		height = textureEntry.m_pTextureAgg->FindScalarByKey("ExportTextures_TextureHeight")->GetAsInt();
		int formatFlags = textureEntry.m_pTextureAgg->FindScalarByKey("ExportTextures_TextureFormat")->GetAsInt();

		format = GetPS2Format(formatFlags);
		if(format == 0xFFFFFFFF)
			throw(PluginException(ERROR_FATAL,"Bad Texture Export Format"));

		// do the allocation
		GSMemManager::allocationInfo info;
		GSMemManager::GetAllocationInfo(width,height,width,format, false, info);

		analyzedData.m_textureBlock = GSMemManager::BlockAlloc(info);

		if(0) //analyzedData.m_textureBlock == 0xFFFFFFFF)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Warning - Out of PS2 VRAM Space Image %s map %d width %d height %d",(const char *)textureEntry.m_filePathName,mapIndex,width,height);
		}
	}
}

void PS2TextureInfo::AllocateVRAMSpaceForPalette(TextureEntry &textureEntry,int textureIndex) 
{
	// collect some info about the texture..
	u32 format;
	u32 paletteBlock = 0; // default

	int formatFlags = textureEntry.m_pTextureAgg->FindScalarByKey("ExportTextures_TextureFormat")->GetAsInt();

	format = GetPS2Format(formatFlags);
	if(format == 0xFFFFFFFF)
		throw(PluginException(ERROR_FATAL,"Bad Texture Export Format"));

	// if using a different texture
	TupperwareScalar *pRemapImageReferenceScalar = textureEntry.m_pTextureAgg->FindScalarByKey("ExportTextures_RemapImageRef");
	if (pRemapImageReferenceScalar)
	{
		int parentImageReference = pRemapImageReferenceScalar->GetAsInt();
		PS2TextureInfo::AnalyzedTextureData &parentAnalyzedData = m_textureDataArray[parentImageReference][0];
		// use the same data as my parent
		paletteBlock = parentAnalyzedData.m_paletteBlock;
	}
	else
	{
		// if 8 or 4 bit
		if (format == RS_TEX_PSMT4 || format == RS_TEX_PSMT8)
		{
			// if palette is remapped to another one
			TupperwareScalar *pParentPaletteReferenceScalar = textureEntry.m_pTextureAgg->FindScalarByKey("ExportTextures_ParentPaletteReference");
			if (pParentPaletteReferenceScalar)
			{
				int parentPaletteReference = pParentPaletteReferenceScalar->GetAsInt();
				assert(parentPaletteReference!=-1);
				assert(parentPaletteReference<textureIndex); // this better be before me
				paletteBlock = m_textureDataArray[parentPaletteReference][0].m_paletteBlock; // copy the palette block
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

				if(0) //paletteBlock == 0xFFFFFFFF)
				{
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
						"Warning - Out of PS2 VRAM Space Allocating Palette - Image %s",(const char *)textureEntry.m_filePathName);
				}
			}
		}
	}
	// use for all maps
	int numMaps = m_textureDataArray[textureIndex].GetSize();
	for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
	{
		m_textureDataArray[textureIndex][mapIndex].m_paletteBlock = paletteBlock;
	}
}

