////////////////////////////////////////////////////////////////////////////
//
// ExportTexturePS2
//
// Export the texture data for the PS2
//
////////////////////////////////////////////////////////////////////////////

#include "ExportTexturesPch.h"
#include "ProcessTup.h"
#include "Ps2TextureInfo.h"

#define PS2_ALPHA(Alpha)	((Alpha + 1) / 2)

static int PS2PaletteRemap[256] = 
{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
		0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
		0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
		0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,

		0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
		0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
		0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
		0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
		0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
		0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
		0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,

		0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
		0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
		0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
		0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
		0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
		0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
		0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
		0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,

		0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
		0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
		0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
		0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
		0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
		0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
		0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
		0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
};


void ProcessTup::WritePS2Texture(TextureEntry &textureEntry,TupArray<TupArray<ts_TextureChunkRecord> > &textureChunkRecordArray,int frameNum,int mapIndex,bool bAnimation)
{
   bool bPS2Texture = true;
   const char *pPS2Texture = m_options.GetValue("PS2Texture");
   if (pPS2Texture && !stricmp(pPS2Texture,"false"))
   {
      bPS2Texture = false;
   }

	ts_TextureChunkRecord &textureChunkRecord = textureChunkRecordArray[frameNum][mapIndex];

	FILE *pDBLFile = DBLUtil::GetDBLFile();

	ImageManager &imageManager = m_dataManager.GetImageManager();
	TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(textureEntry.m_filePathName);
	assert(pImageDataAgg);
	// get the first frame from the image data
	TupImageData tupImageData(pImageDataAgg);
	int numFrames = tupImageData.GetNumImageDataFrames();
	assert(numFrames>0);

	// if this is not when we save animations and it is one then get out
	if (!bAnimation && numFrames>1)
		return;

	// if this image needs to point to another
	TupperwareScalar *pRemapImageReferenceScalar = textureEntry.m_pTextureAgg->FindScalarByKey("ExportTextures_RemapImageRef");
	if (pRemapImageReferenceScalar)
	{
		int parentImageReference = pRemapImageReferenceScalar->GetAsInt();
		ts_TextureChunkRecord &parentTextureChunkRecord = textureChunkRecordArray[parentImageReference][mapIndex];
		// use the same date as my parent
		textureChunkRecord.s16TextureWidth = parentTextureChunkRecord.s16TextureWidth;
		textureChunkRecord.s16TextureHeight = parentTextureChunkRecord.s16TextureHeight;
		textureChunkRecord.u32TextureOffset = parentTextureChunkRecord.u32TextureOffset;
		textureChunkRecord.u32PaletteOffset = parentTextureChunkRecord.u32PaletteOffset;
		return;
	}

	// which frame to get from the image (depends on if we are doing animation or not)
	int animationFrameNum = bAnimation ? frameNum : 0;

	TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(animationFrameNum);
	assert(pImageDataFrameAgg);
	TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);
	// get first map from frame
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	assert(numMaps>0);
	TupperwareAggregate *pImageDataMap = tupImageDataFrame.GetImageDataMap(mapIndex);
	assert(pImageDataMap);
	TupImageDataMap tupImageDataMap(pImageDataMap);
	// get the bitmap data
	int width;
	tupImageDataMap.GetWidth(width);
	int height;
	tupImageDataMap.GetHeight(height);

	if (m_platform == PLATFORM_PS2)
	{
		DBLUtil::AlignChunkDataQuadWord();
	}

	textureChunkRecord.s16TextureWidth = (s16) width;
	textureChunkRecord.s16TextureHeight = (s16) height;
	int imageSize;
	void *pImageData;

	switch(formatFlags&TupImageDataFrame::FORMAT_MASK)
	{
		case TupImageDataFrame::RGBA_8888:
			{
				imageSize = width*height*4;
				assert(imageSize);
				int actualSize;
				tupImageDataMap.GetData(&pImageData,actualSize);
				assert(imageSize==actualSize);
				// fix the alpha channel for ps2
				int numPixels = width*height;
				ts_bRGBA *pTempImageData = new ts_bRGBA[numPixels];
				memcpy(pTempImageData,pImageData,imageSize);
            int i;
            if (bPS2Texture)
            {
               for (i=0;i<numPixels;i++)
               {
                  pTempImageData[i].A = PS2_ALPHA(pTempImageData[i].A);
               }
            }
            else // dx texture
            {
               for (i=0;i<numPixels;i++)
               {
                  u8 red = pTempImageData[i].R;
                  pTempImageData[i].R = pTempImageData[i].B;
                  pTempImageData[i].B = red;
               }
            }
				/* write out image data */
				textureChunkRecord.u32TextureOffset = ftell(pDBLFile) - DBLUtil::GetChunkDataStart();
				if (m_bExportTextureData)
				{
					DBLWrite::WriteData(pTempImageData, imageSize, pDBLFile);
				}
				DBLUtil::AlignFile();
				delete pTempImageData;
			}
		break;
		case TupImageDataFrame::RGB_888:
			{
				imageSize = width*height*3;
				assert(imageSize);
				int actualSize;
				tupImageDataMap.GetData(&pImageData,actualSize);
				assert(imageSize==actualSize);
            int numPixels = width*height;
            ts_bRGB *pTempImageData = new ts_bRGB[numPixels];
            memcpy(pTempImageData,pImageData,imageSize);
            int i;
            if (!bPS2Texture) // dx texture
            {
               for (i=0;i<numPixels;i++)
               {
                  u8 red = pTempImageData[i].R;
                  pTempImageData[i].R = pTempImageData[i].B;
                  pTempImageData[i].B = red;
               }
            }

            /* write out image data */
				textureChunkRecord.u32TextureOffset = ftell(pDBLFile) - DBLUtil::GetChunkDataStart();
				if (m_bExportTextureData)
				{
					DBLWrite::WriteData(pTempImageData, imageSize, pDBLFile);
				}
				DBLUtil::AlignFile();
            delete pTempImageData;
			}
		break;
		case TupImageDataFrame::RGB_555:
			{
				imageSize = width*height*2;
				assert(imageSize);
				int actualSize;
				tupImageDataMap.GetData(&pImageData,actualSize);
				assert(imageSize==actualSize);
				int numPixels = imageSize>>1;
				unsigned short *pTempImageData = new unsigned short[numPixels];
				memcpy(pTempImageData,pImageData,imageSize);
            int index;
            if (bPS2Texture)
            {
               // convert to 555 for PS2
               for (index = 0;index<numPixels;index++)
               {
                  unsigned short pixel = pTempImageData[index];
                  // from 0RRR RRGG GGGB BBBB
                  // to   1BBB BBGG GGGR RRRR
                  pTempImageData[index] = 0x8000 | (pixel&0x03e0) | ((pixel&0x001f)<<10) | ((pixel&0x7c00)>>10) ;
               }
            }
				/* write out image data */
				textureChunkRecord.u32TextureOffset = ftell(pDBLFile) - DBLUtil::GetChunkDataStart();
				if (m_bExportTextureData)
				{
					DBLWrite::WriteData(pTempImageData, imageSize, pDBLFile);
				}
				DBLUtil::AlignFile();
				delete pTempImageData;

			}
		break;
		case TupImageDataFrame::RGB_565:
			{
				imageSize = width*height*2;
				assert(imageSize);
				int actualSize;
				tupImageDataMap.GetData(&pImageData,actualSize);
				assert(imageSize==actualSize);
				int numPixels = imageSize>>1;
				unsigned short *pTempImageData = new unsigned short[numPixels];
				memcpy(pTempImageData,pImageData,imageSize);
            int index;
            if (bPS2Texture)
            {
               // convert to 555 for PS2
               for (index = 0;index<numPixels;index++)
               {
                  unsigned short pixel = pTempImageData[index];
                  // from RRRR RGGG GGGB BBBB
                  // to   1BBB BBGG GGGR RRRR
                  pTempImageData[index] = 0x8000 | ((pixel&0x001f)<<10) | ((pixel&0xf800)>>11) | ((pixel&0x07c0)>>1) ;
               }
            }
				/* write out image data */
				textureChunkRecord.u32TextureOffset = ftell(pDBLFile) - DBLUtil::GetChunkDataStart();
				if (m_bExportTextureData)
				{
					DBLWrite::WriteData(pTempImageData, imageSize, pDBLFile);
				}
				DBLUtil::AlignFile();
				delete pTempImageData;
			}
		break;
		case TupImageDataFrame::RGBA_1555:
			{
				imageSize = width*height*2;
				assert(imageSize);
				int actualSize;
				tupImageDataMap.GetData(&pImageData,actualSize);
				assert(imageSize==actualSize);
				int numPixels = imageSize>>1;
				unsigned short *pTempImageData = new unsigned short[numPixels];
				memcpy(pTempImageData,pImageData,imageSize);
            int index;
            if (bPS2Texture)
            {
				   for (index = 0;index<numPixels;index++)
				   {
					   unsigned short pixel = pTempImageData[index];
					   // from ARRR RRGG GGGB BBBB
					   // to   ABBB BBGG GGGR RRRR
					   pTempImageData[index] = (pixel&0x83e0) | ((pixel&0x001f)<<10) | ((pixel&0x7c00)>>10) ;
				   }
            }
				/* write out image data */
				textureChunkRecord.u32TextureOffset = ftell(pDBLFile) - DBLUtil::GetChunkDataStart();
				if (m_bExportTextureData)
				{
					DBLWrite::WriteData(pTempImageData, imageSize, pDBLFile);
				}
				DBLUtil::AlignFile();
				delete pTempImageData;

			}
		break;
	case TupImageDataFrame::INDEXED_8:
		{
			imageSize = width*height;

			assert(imageSize);
			int actualSize;
			tupImageDataMap.GetData(&pImageData,actualSize);
			assert(imageSize==actualSize);
			bool bSavePalette = true;

			if (mapIndex!=0) // if we are a mip map
			{
				bSavePalette = false; // we are using a parent palette
				// use the same offset as my first map
				textureChunkRecord.u32PaletteOffset = textureChunkRecordArray[frameNum][0].u32PaletteOffset;
				assert(textureChunkRecord.u32PaletteOffset); // this better be set
			}
			else // first map
			{
				// check if we are doing palette sharing
				if (bAnimation)
				{
					TupperwareScalar *pParentPaletteFrameScalar = pImageDataFrameAgg->FindScalarByKey("ExportTextures_ParentPaletteFrame");
					if (pParentPaletteFrameScalar)
					{
						bSavePalette = false; // we are using a parent palette
						int parentPaletteFrame = pParentPaletteFrameScalar->GetAsInt();
						assert(parentPaletteFrame<frameNum); // this better be before me
						// use the same offset as my parent
						textureChunkRecord.u32PaletteOffset = textureChunkRecordArray[parentPaletteFrame][0].u32PaletteOffset;
						assert(textureChunkRecord.u32PaletteOffset); // this better be set
					}
				}
				else
				{
					TupperwareScalar *pParentPaletteReferenceScalar = textureEntry.m_pTextureAgg->FindScalarByKey("ExportTextures_ParentPaletteReference");
					if (pParentPaletteReferenceScalar)
					{
						bSavePalette = false; // we are using a parent palette
						int parentPaletteReference = pParentPaletteReferenceScalar->GetAsInt();
						assert(parentPaletteReference<frameNum); // this better be before me
						// use the same offset as my parent
						textureChunkRecord.u32PaletteOffset = textureChunkRecordArray[parentPaletteReference][0].u32PaletteOffset;
						assert(textureChunkRecord.u32PaletteOffset); // this better be set
					}
				}
			}

			if (bSavePalette)
			{
				// save out palette
				textureChunkRecord.u32PaletteOffset = ftell(pDBLFile) - DBLUtil::GetChunkDataStart();
				TupperwareAggregate *pImageDataPaletteAgg = tupImageDataFrame.GetImageDataPalette();
				assert(pImageDataPaletteAgg);
				TupImageDataPalette tupImageDataPalette(pImageDataPaletteAgg);
				ts_bRGBA *pPaletteData;
				int numBytes;
				tupImageDataPalette.GetData((void **)&pPaletteData,numBytes);
				assert(pPaletteData);
				assert(numBytes==256*4);

				// swizzle the palette
				ts_bRGBA swizzledPalette[256];
				for (int i = 0; i < 256; i++)
				{
					swizzledPalette[PS2PaletteRemap[i]] = pPaletteData[i];
					swizzledPalette[PS2PaletteRemap[i]].A = PS2_ALPHA(pPaletteData[i].A);
				}

				if (m_bExportTextureData)
				{
					DBLWrite::WriteRecords(swizzledPalette, pDBLFile, 256);
				}
				DBLUtil::AlignFile();
			}
			/* write out image data */
			textureChunkRecord.u32TextureOffset = ftell(pDBLFile) - DBLUtil::GetChunkDataStart();
			if (m_bExportTextureData)
			{
				DBLWrite::WriteData(pImageData, imageSize, pDBLFile);
			}
			DBLUtil::AlignFile();
		}
		break;
	case TupImageDataFrame::INDEXED_4:
		{
			imageSize = (width/2)*height;

			assert(imageSize);
			int actualSize;
			tupImageDataMap.GetData(&pImageData,actualSize);
			assert(imageSize==actualSize);
			bool bSavePalette = true;

			if (mapIndex!=0) // if we are a mip map
			{
				bSavePalette = false; // we are using a parent palette
				// use the same offset as my first map
				textureChunkRecord.u32PaletteOffset = textureChunkRecordArray[frameNum][0].u32PaletteOffset;
				assert(textureChunkRecord.u32PaletteOffset); // this better be set
			}
			else // first map
			{
				if (bAnimation)
				{
					TupperwareScalar *pParentPaletteFrameScalar = pImageDataFrameAgg->FindScalarByKey("ExportTextures_ParentPaletteFrame");
					if (pParentPaletteFrameScalar)
					{
						bSavePalette = false; // we are using a parent palette
						int parentPaletteFrame = pParentPaletteFrameScalar->GetAsInt();
						assert(parentPaletteFrame<frameNum); // this better be before me
						// use the same offset as my parent
						textureChunkRecord.u32PaletteOffset = textureChunkRecordArray[parentPaletteFrame][0].u32PaletteOffset;
						assert(textureChunkRecord.u32PaletteOffset); // this better be set
					}
				}
				else
				{
					TupperwareScalar *pParentPaletteReferenceScalar = textureEntry.m_pTextureAgg->FindScalarByKey("ExportTextures_ParentPaletteReference");
					if (pParentPaletteReferenceScalar)
					{
						bSavePalette = false; // we are using a parent palette
						int parentPaletteReference = pParentPaletteReferenceScalar->GetAsInt();
						assert(parentPaletteReference<frameNum); // this better be before me
						// use the same offset as my parent
						textureChunkRecord.u32PaletteOffset = textureChunkRecordArray[parentPaletteReference][0].u32PaletteOffset;
						assert(textureChunkRecord.u32PaletteOffset); // this better be set
					}
				}
			}
			if (bSavePalette)
			{
				// save out palette
				textureChunkRecord.u32PaletteOffset = ftell(pDBLFile) - DBLUtil::GetChunkDataStart();
				TupperwareAggregate *pImageDataPaletteAgg = tupImageDataFrame.GetImageDataPalette();
				assert(pImageDataPaletteAgg);
				TupImageDataPalette tupImageDataPalette(pImageDataPaletteAgg);
				ts_bRGBA *pPaletteData;
				int numBytes;
				tupImageDataPalette.GetData((void **)&pPaletteData,numBytes);
				assert(pPaletteData);
				assert(numBytes==16*4);

				for (int i = 0; i < 16; i++)
				{
					pPaletteData[i].A = PS2_ALPHA(pPaletteData[i].A);
				}

				if (m_bExportTextureData)
				{
					DBLWrite::WriteRecords(pPaletteData, pDBLFile, 16);
				}
				DBLUtil::AlignFile();
			}
			/* write out image data */
			textureChunkRecord.u32TextureOffset = ftell(pDBLFile) - DBLUtil::GetChunkDataStart();
			if (m_bExportTextureData)
			{
				DBLWrite::WriteData(pImageData, imageSize, pDBLFile);
			}
			DBLUtil::AlignFile();
		}
		break;
	default:
		assert(false);
	}

	if (m_platform == PLATFORM_PS2)
	{
		if (imageSize >= (1024 * 1024 / 2))
		{
			int bytesPerPixel = imageSize/(width*height);
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Warning - Texture too big for PS2 %s - width = %d, height = %d, bytesPerPixel = %d",
				(const char *)textureEntry.m_filePathName,width,height,bytesPerPixel);
		}
	}
}

void ProcessTup::SetPS2TextureRecord(TextureEntry &textureEntry,TupArray<TupArray<ts_TextureChunkRecord> > &textureChunkRecordArray,int frameNum,int mapIndex,bool bAnimation)
{
	ts_TextureChunkRecord &textureChunkRecord = textureChunkRecordArray[frameNum][mapIndex];

	FILE *pDBLFile = DBLUtil::GetDBLFile();

	// if this image needs to point to another
	TupperwareScalar *pRemapImageReferenceScalar = textureEntry.m_pTextureAgg->FindScalarByKey("ExportTextures_RemapImageRef");
	if (pRemapImageReferenceScalar)
	{
		int parentImageReference = pRemapImageReferenceScalar->GetAsInt();
		ts_TextureChunkRecord &parentTextureChunkRecord = textureChunkRecordArray[parentImageReference][mapIndex];
		// use the same date as my parent
		textureChunkRecord.s16TextureWidth = parentTextureChunkRecord.s16TextureWidth;
		textureChunkRecord.s16TextureHeight = parentTextureChunkRecord.s16TextureHeight;
		textureChunkRecord.u32Flags = parentTextureChunkRecord.u32Flags;
		textureChunkRecord.s16PaletteWidth = parentTextureChunkRecord.s16PaletteWidth;
		textureChunkRecord.s16PaletteHeight = parentTextureChunkRecord.s16PaletteHeight;

		// copy name for easier debugging in game engine 
		// copy right part of name
		int startingNameIndex = strlen(textureEntry.m_filePathName)-(TEXTURE_NAME_LENGTH-1);
		if (startingNameIndex<0)
			startingNameIndex=0;

		strncpy(textureChunkRecord.acTextureName, &((const char *)textureEntry.m_filePathName)[startingNameIndex], TEXTURE_NAME_LENGTH);

		textureChunkRecord.acTextureName[TEXTURE_NAME_LENGTH - 1] = 0;	// make sure it's null-terminated

		return;
	}

	ImageManager &imageManager = m_dataManager.GetImageManager();
	TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(textureEntry.m_filePathName);
	assert(pImageDataAgg);
	// get the first frame from the image data
	TupImageData tupImageData(pImageDataAgg);
	int numFrames = tupImageData.GetNumImageDataFrames();
	assert(numFrames>0);
	// mark texture as animated if using more than 1 frame and not part of an animated texture set)
	if (!bAnimation && numFrames>1)
		textureChunkRecord.u32Flags |= TEX_FLAG_ANIMATED_TEXTURE;

	// set flag if uv is reversed
	TupperwareScalar *pRotatedImageScalar = textureEntry.m_pTextureAgg->FindScalarByKey("ExportTextures_ImageReversed");
	if (pRotatedImageScalar)
	{
		textureChunkRecord.u32Flags |= TEX_FLAG_UVREVERSED;
	}

	TupperwareAggregate *pImageDataFrame = tupImageData.GetImageDataFrame(0);
	assert(pImageDataFrame);
	TupImageDataFrame tupImageDataFrame(pImageDataFrame);
	int formatFlags;
	tupImageDataFrame.GetFormatFlags(formatFlags);
	// get first map from frame
	int numMaps = tupImageDataFrame.GetNumImageDataMaps();
	assert(numMaps>0);
	TupperwareAggregate *pImageDataMap = tupImageDataFrame.GetImageDataMap(mapIndex);
	assert(pImageDataMap);
	TupImageDataMap tupImageDataMap(pImageDataMap);
	// get the bitmap data
	int width;
	tupImageDataMap.GetWidth(width);
	int height;
	tupImageDataMap.GetHeight(height);

	textureChunkRecord.s16TextureWidth = (s16) width;
	textureChunkRecord.s16TextureHeight = (s16) height;

	switch(formatFlags&TupImageDataFrame::FORMAT_MASK)
	{
	case TupImageDataFrame::RGBA_8888:
		textureChunkRecord.u32Flags |= TEX_FLAG_SIZE_32_BIT | TEX_FLAG_FMT_RGBA;
		break;
	case TupImageDataFrame::RGB_888:
		textureChunkRecord.u32Flags |= TEX_FLAG_SIZE_24_BIT | TEX_FLAG_FMT_RGB;
		break;
	case TupImageDataFrame::RGB_555:
	case TupImageDataFrame::RGB_565:
		textureChunkRecord.u32Flags |= TEX_FLAG_SIZE_16_BIT | TEX_FLAG_FMT_RGB;
		break;
	case TupImageDataFrame::RGBA_1555:
		textureChunkRecord.u32Flags |= TEX_FLAG_SIZE_16_BIT | TEX_FLAG_FMT_RGBA;
		break;
	case TupImageDataFrame::INDEXED_8:
		textureChunkRecord.u32Flags |= TEX_FLAG_SIZE_8_BIT | TEX_FLAG_FMT_CI;
		textureChunkRecord.s16PaletteWidth = 16;
		textureChunkRecord.s16PaletteHeight = 16;
		break;
	case TupImageDataFrame::INDEXED_4:
		textureChunkRecord.u32Flags |= TEX_FLAG_SIZE_4_BIT | TEX_FLAG_FMT_CI;
		textureChunkRecord.s16PaletteWidth = 8;
		textureChunkRecord.s16PaletteHeight = 2;
		break;
	default:
		assert(false);
	}

	// mark diffuse textures 
	TupperwareScalar *pDiffuseMapScalar = textureEntry.m_pTextureAgg->FindScalarByKey("ExportTextures_DiffuseMap");
	if (pDiffuseMapScalar && pDiffuseMapScalar->GetAsInt() == TRUE)
		textureChunkRecord.u32Flags |= TEX_FLAG_DIFFUSE;

	// copy name for easier debugging in game engine 
	// copy right part of name
	int startingNameIndex = strlen(textureEntry.m_filePathName)-(TEXTURE_NAME_LENGTH-1);
	if (startingNameIndex<0)
		startingNameIndex=0;

	strncpy(textureChunkRecord.acTextureName, &((const char *)textureEntry.m_filePathName)[startingNameIndex], TEXTURE_NAME_LENGTH);

	textureChunkRecord.acTextureName[TEXTURE_NAME_LENGTH - 1] = 0;	// make sure it's null-terminated
}
