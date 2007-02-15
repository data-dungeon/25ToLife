////////////////////////////////////////////////////////////////////////////
//
// ExportTexture
//
// Export the textures
//
////////////////////////////////////////////////////////////////////////////

#include "ExportPch.h"
#include "ExportTexture.h"
#ifdef EXPORT_PS2
#include "..\ExportPS2\textureinfo.h"
#endif

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

ExportTexture::ExportTexture(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,
							 TupScene &tupScene,TupObjectPool &tupObjectPool,TupImagePool &tupImagePool,
							 int currentPartition,bool bExportTextureData) :
	m_pDBLFileName(pDBLFileName),
	m_options(options),
	m_dataManager(dataManager),
	m_tupScene(tupScene),
	m_tupObjectPool(tupObjectPool),
	m_tupImagePool(tupImagePool),
	m_currentPartition(currentPartition),
	m_bExportTextureData(bExportTextureData)
{
}

/* collect and write out texture data */

u32 ExportTexture::WriteTextures(void)
{
	FILE *pDBLFile = DBLUtil::GetDBLFile();

	TupperwareAggregate *pSceneAgg = m_tupScene.GetBaseAggregate();

	int *totalTexturesArray = m_tupScene.GetBaseAggregate()->FindListByKey("Annotate_TotalTextures")->GetAsInt();
	int totalTextures = totalTexturesArray[m_currentPartition];
	if (totalTextures==0)
	{
		return 0;
	}

	ts_TextureChunkHeader textureChunkHeader;
	memset(&textureChunkHeader, 0, sizeof(ts_TextureChunkHeader));
	textureChunkHeader.u32TextureCount = totalTextures;
	strncpy(textureChunkHeader.Name, m_pDBLFileName, TEXTURE_SET_NAME_LENGTH);
	textureChunkHeader.Name[TEXTURE_SET_NAME_LENGTH - 1] = 0;	// make sure it's null-terminated

	DBLUtil::StartChunk(DB_TEXTURE_SET, DB_TEXTURE_SET_VERSION, DBL_CHUNK_NO_IGNORE, "1000");

	/* header contains one texture chunk record (an array of one entry), write out header without that record. */
	/* this is placeholder only, header will be re-written when complete */

	DBLWrite::WriteRecord(&textureChunkHeader, pDBLFile);

	TupArray<TupArray<ts_TextureChunkRecord> > textureChunkRecordArray;
	TupArray<TupArray<int> > textureHeaderChunkOffsetArray;

	textureChunkRecordArray.SetSize(totalTextures);
	textureHeaderChunkOffsetArray.SetSize(totalTextures);

	int numImages = m_tupImagePool.GetNumImages();
	int imageIndex;
	int textureIndex;
	for (textureIndex = 0,imageIndex = 0; imageIndex < numImages; imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_tupImagePool.GetImage(imageIndex);
		if (!pImageAgg)
			continue;
		TupperwareList *pTextureIDList = pImageAgg->FindListByKey("Annotate_TextureIDs");
		if (pTextureIDList)
		{
			int *pTextureIDs = pTextureIDList->GetAsInt();
			int textureID = pTextureIDs[m_currentPartition];
			// if not used or referencing the world partition
			if (textureID==-1 || (textureID&0xff000000)!=0)
				continue;

			int numMaps = 1; // start with the base map
			float mipMapDistance = 0.0f;

			TupperwareScalar *pMipMappingScalar = pImageAgg->FindScalarByKey("Annotate_MipMapping");
			// does this one have mip maps
			if (pMipMappingScalar)
			{
				numMaps += pImageAgg->FindScalarByKey("Annotate_MipMapNumSubMaps")->GetAsInt();
				mipMapDistance = pImageAgg->FindScalarByKey("Annotate_MipMapDistance")->GetAsFloat();
			}
			textureChunkRecordArray[textureIndex].SetSize(numMaps);
			textureHeaderChunkOffsetArray[textureIndex].SetSize(numMaps);
			textureChunkRecordArray[textureIndex][0].u16MipmapDistance = (u16)mipMapDistance;
			textureIndex++;
		}
	}

	// write out empty texture chunk records-- placeholder only, these will be re-written when complete 
	for (textureIndex = 0; textureIndex < totalTextures; textureIndex++)
	{
		// add position of the start of the chunk record 
		textureHeaderChunkOffsetArray[textureIndex][0] = ftell(pDBLFile) - DBLUtil::GetChunkDataStart();
		DBLWrite::WriteRecord(&textureChunkRecordArray[textureIndex][0], pDBLFile);
	}

	// write out empty texture chunk records for the submaps and link the parents through
	for (textureIndex = 0; textureIndex < totalTextures; textureIndex++)
	{
		int numMaps = textureHeaderChunkOffsetArray[textureIndex].GetSize();
		for (int mapIndex=1;mapIndex<numMaps;mapIndex++)
		{
			int fileOffset = ftell(pDBLFile) - DBLUtil::GetChunkDataStart();
			textureHeaderChunkOffsetArray[textureIndex][mapIndex] = fileOffset;
			textureChunkRecordArray[textureIndex][mapIndex-1].u32NextOffset = fileOffset; // set parent (0 default)
			DBLWrite::WriteRecord(&textureChunkRecordArray[textureIndex][mapIndex], pDBLFile);
		}
	}

	textureChunkHeader.u32PieceOffset = 0; // this is not a merged texture set

#ifdef EXPORT_PS2
	PS2TextureInfo ps2TextureInfo(m_options,m_dataManager,m_tupScene,m_tupObjectPool,m_tupImagePool,m_currentPartition);
	ps2TextureInfo.AnalyzeTextures();
	for (imageIndex = 0; imageIndex < numImages; imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_tupImagePool.GetImage(imageIndex);
		if (!pImageAgg)
			continue;
		TupperwareList *pTextureIDList = pImageAgg->FindListByKey("Annotate_TextureIDs");
		if (pTextureIDList)
		{
			int *pTextureIDs = pTextureIDList->GetAsInt();
			int textureID = pTextureIDs[m_currentPartition];
			// if not used or referencing the world partition
			if (textureID==-1 || (textureID&0xff000000)!=0)
				continue;

			int textureSet = pImageAgg->FindScalarByKey("Annotate_TextureSet")->GetAsInt();

			int numMaps = textureChunkRecordArray[textureID].GetSize();
			for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
			{
				const PS2TextureInfo::AnalyzedTextureData &textureData = ps2TextureInfo.GetAnalyzedTextureData(textureID,mapIndex);
				textureChunkRecordArray[textureID][mapIndex].s16TextureBlock = (s16) textureData.m_textureBlock;
				textureChunkRecordArray[textureID][mapIndex].s16PaletteBlock = (s16) textureData.m_paletteBlock;
				textureChunkRecordArray[textureID][mapIndex].s16ImageBlocks = 0;
				textureChunkRecordArray[textureID][mapIndex].u32SubSetNumber = (u32) textureSet;
			}
		}
	}
	textureChunkHeader.u32NumTexSubSets = (u32) ps2TextureInfo.GetNumTextureSets();
	textureChunkHeader.u32VRAMInfoTableOffset = ftell(pDBLFile) - DBLUtil::GetChunkDataStart(); 
	// write vram info
	for (u32 subSet=0;subSet<textureChunkHeader.u32NumTexSubSets;subSet++)
	{
		u32 vramBlockSize = ps2TextureInfo.GetTextureSetVRAMBlockSize(subSet);
		DBLWrite::WriteRecord(&vramBlockSize, pDBLFile);
	}
#else
	textureChunkHeader.u32VRAMInfoTableOffset = 0;
	textureChunkHeader.u32NumTexSubSets = 1;
#endif

	// output textures 
	int recordCount = 0;
	for (imageIndex = 0; imageIndex < numImages; imageIndex++)
	{
		if (PluginLog::GetAbortFlag()) throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));

		TupperwareAggregate *pImageAgg = m_tupImagePool.GetImage(imageIndex);
		if (!pImageAgg)
			continue;
		TupperwareList *pTextureIDList = pImageAgg->FindListByKey("Annotate_TextureIDs");
		if (pTextureIDList)
		{
			int *pTextureIDs = pTextureIDList->GetAsInt();
			int textureID = pTextureIDs[m_currentPartition];
			if (textureID==-1 || (textureID&0xff000000)!=0)
				continue;
			assert(textureID==recordCount);
			int numMaps = textureChunkRecordArray[textureID].GetSize();
			for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
			{
#ifdef EXPORT_GCN
				SetGCNTextureRecord(pImageAgg,textureChunkRecordArray,textureID,mapIndex,false);
				WriteGCNTexture(pImageAgg,textureChunkRecordArray,textureID,mapIndex,false);
#else
				SetTextureRecord(pImageAgg,textureChunkRecordArray,textureID,mapIndex,false);
				WriteTexture(pImageAgg,textureChunkRecordArray,textureID,mapIndex,false);
#endif
			}
			recordCount++;
		}
	}

	/* rewrite texture chunk header and records now that they are filled in */
	fseek(pDBLFile, DBLUtil::GetChunkDataStart(), SEEK_SET);
	DBLWrite::WriteRecord(&textureChunkHeader, pDBLFile);
	for (textureIndex = 0; textureIndex < totalTextures; textureIndex++)
	{
		/* write out record */
		assert(textureHeaderChunkOffsetArray[textureIndex][0] == ftell(pDBLFile) - DBLUtil::GetChunkDataStart());	// must be in same place in file
		DBLWrite::WriteRecord(&textureChunkRecordArray[textureIndex][0], pDBLFile);
	}
	// sub maps
	for (textureIndex = 0; textureIndex < totalTextures; textureIndex++)
	{
		int numMaps = textureHeaderChunkOffsetArray[textureIndex].GetSize();
		for (int mapIndex=1;mapIndex<numMaps;mapIndex++)
		{
			assert(textureHeaderChunkOffsetArray[textureIndex][mapIndex] == ftell(pDBLFile) - DBLUtil::GetChunkDataStart());	// must be in same place in file
			DBLWrite::WriteRecord(&textureChunkRecordArray[textureIndex][mapIndex], pDBLFile);
		}
	}

	/* back to end of file */
	fseek(pDBLFile, 0, SEEK_END);

	/* chunk is done-- move it to front of dbl file (needs to come before geometry and texture context chunks) */
	u32 chunkSize = DBLUtil::FinishChunk(FALSE);

	return chunkSize;
}

u32 ExportTexture::WriteAnimatedTextures(void)
{
	FILE *pDBLFile = DBLUtil::GetDBLFile();
	u32 chunkSize = 0;

	int *totalTexturesArray = m_tupScene.GetBaseAggregate()->FindListByKey("Annotate_TotalTextures")->GetAsInt();
	int totalTextures = totalTexturesArray[m_currentPartition];
	if (totalTextures==0)
	{
		return 0;
	}

	int numImages = m_tupImagePool.GetNumImages();
	for (int imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_tupImagePool.GetImage(imageIndex);
		if (!pImageAgg)
			continue;

		TupperwareList *pTextureIDList = pImageAgg->FindListByKey("Annotate_TextureIDs");
		if (pTextureIDList)
		{
			int *pTextureIDs = pTextureIDList->GetAsInt();
			int textureID = pTextureIDs[m_currentPartition];
			// if not used or referencing the world partition
			if (textureID==-1 || (textureID&0xff000000)!=0)
				continue;

			TupImage tupImage(pImageAgg);
			char *pImageFileName;
			// get filename for image
			tupImage.GetFilePathName(&pImageFileName);
			// get image data from image manager
			ImageManager &imageManager = m_dataManager.GetImageManager();
			TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(pImageFileName);
			assert(pImageDataAgg);
			// get the first frame from the image data
			TupImageData tupImageData(pImageDataAgg);
			int numFrames = tupImageData.GetNumImageDataFrames();
			if (numFrames>1)
			{
				ts_AnimatedTextureChunkHeader animatedTextureChunkHeader;

				DBLUtil::StartChunk(DB_ANIMATED_TEXTURE_SET, DB_ANIMATED_TEXTURE_SET_VERSION, DBL_CHUNK_NO_IGNORE, "1000");

				/* clear and fill in header */
				memset(&animatedTextureChunkHeader, 0, sizeof(ts_AnimatedTextureChunkHeader));
				animatedTextureChunkHeader.u16TextureIndex = 0;	// start at first image
				animatedTextureChunkHeader.u16TextureCount = numFrames;

				/* record texture set index-- this is so animated texture can get to bind number assigned during */
				/* texture set loading at runtime */

				animatedTextureChunkHeader.u32TextureSlot = textureID;

				/* animation speed, in milliseconds */

				animatedTextureChunkHeader.s16TimerInit = 33; //GetFPS();
				animatedTextureChunkHeader.s16Timer = animatedTextureChunkHeader.s16TimerInit;
				
				/* header contains one texture chunk record (an array of one entry), write out header without that record */

				DBLWrite::WriteRecord(&animatedTextureChunkHeader, pDBLFile);

				TupArray<TupArray<ts_TextureChunkRecord> > textureChunkRecordArray;
				textureChunkRecordArray.SetSize(numFrames);

				TupArray<TupArray<int> > textureHeaderChunkOffsetArray;
				textureHeaderChunkOffsetArray.SetSize(numFrames);

				/* write out texture chunk records */
				int frameIndex;
				for (frameIndex=0;frameIndex<numFrames;frameIndex++)
				{
					textureChunkRecordArray[frameIndex].SetSize(1);
					textureHeaderChunkOffsetArray[frameIndex].SetSize(1);

					// add position of the start of the chunk record 
					textureHeaderChunkOffsetArray[frameIndex][0] = ftell(pDBLFile) - DBLUtil::GetChunkDataStart();
					memset(&textureChunkRecordArray[frameIndex][0], 0, sizeof(ts_TextureChunkRecord));
					DBLWrite::WriteRecord(&textureChunkRecordArray[frameIndex][0], pDBLFile);
				}

				// output textures 
				for (frameIndex = 0; frameIndex < numFrames; frameIndex++)
				{
					if (PluginLog::GetAbortFlag()) throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));
#ifdef EXPORT_GCN
					SetGCNTextureRecord(pImageAgg,textureChunkRecordArray,frameIndex,0,true); 
					WriteGCNTexture(pImageAgg,textureChunkRecordArray,frameIndex,0,true);
#else
					SetTextureRecord(pImageAgg,textureChunkRecordArray,frameIndex,0,true);
					WriteTexture(pImageAgg,textureChunkRecordArray,frameIndex,0,true);
#endif
				}

				/* rewrite texture chunk header and records now that they are filled in */
				fseek(pDBLFile, DBLUtil::GetChunkDataStart(), SEEK_SET);
				DBLWrite::WriteRecord(&animatedTextureChunkHeader, pDBLFile);
				for (frameIndex = 0; frameIndex < numFrames; frameIndex++)
				{
					/* write out record */
					assert(textureHeaderChunkOffsetArray[frameIndex][0] == ftell(pDBLFile) - DBLUtil::GetChunkDataStart());	// must be in same place in file
					DBLWrite::WriteRecord(&textureChunkRecordArray[frameIndex][0], pDBLFile);
				}
				/* back to end of file */
				fseek(pDBLFile, 0, SEEK_END);

				/* chunk is done-- move it to front of dbl file (needs to come before geometry and texture context chunks) */
				chunkSize += DBLUtil::FinishChunk(FALSE);
			}
		}
	}
	return chunkSize;
}



void ExportTexture::WriteTexture(TupperwareAggregate *pImageAgg,TupArray<TupArray<ts_TextureChunkRecord> > &textureChunkRecordArray,int frameNum,int mapIndex,bool bAnimation)
{
   bool bPS2Texture = true;
   const char *pPS2Texture = m_options.GetValue("PS2Texture");
   if (pPS2Texture && !stricmp(pPS2Texture,"false"))
   {
      bPS2Texture = false;
   }

	ts_TextureChunkRecord &textureChunkRecord = textureChunkRecordArray[frameNum][mapIndex];

	FILE *pDBLFile = DBLUtil::GetDBLFile();

	TupImage tupImage(pImageAgg);
	char *pImageFileName;
	// get filename for image
	tupImage.GetFilePathName(&pImageFileName);
	// get image data from image manager
	ImageManager &imageManager = m_dataManager.GetImageManager();
	TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(pImageFileName);
	assert(pImageDataAgg);
	// get the first frame from the image data
	TupImageData tupImageData(pImageDataAgg);
	int numFrames = tupImageData.GetNumImageDataFrames();
	assert(numFrames>0);

	// if this is not when we save animations and it is one then get out
	if (!bAnimation && numFrames>1)
		return;
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

	// check if animated frame is remapped to another in the animation
	if (bAnimation && animationFrameNum > 0)
	{
		TupperwareScalar *pRemapAnimatedFrameIndexScalar = pImageDataFrameAgg->FindScalarByKey("Annotate_RemapAnimatedFrameIndex");
		if (pRemapAnimatedFrameIndexScalar)
		{
			int remapAnimationFrameNum = pRemapAnimatedFrameIndexScalar->GetAsInt();
			textureChunkRecord.s16TextureWidth = (s16) width;
			textureChunkRecord.s16TextureHeight = (s16) height;
			textureChunkRecord.u32TextureOffset = textureChunkRecordArray[remapAnimationFrameNum][mapIndex].u32TextureOffset;
			textureChunkRecord.u32PaletteOffset = textureChunkRecordArray[remapAnimationFrameNum][mapIndex].u32PaletteOffset;
			return;
		}
	}

#ifdef EXPORT_PS2
	DBLUtil::AlignChunkDataQuadWord();
#endif

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
						TupperwareScalar *pParentPaletteFrameScalar = pImageDataFrameAgg->FindScalarByKey("Annotate_ParentPaletteFrame");
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
						TupperwareScalar *pParentPaletteReferenceScalar = pImageAgg->FindScalarByKey("Annotate_ParentPaletteReference");
						if (pParentPaletteReferenceScalar)
						{
							bSavePalette = false; // we are using a parent palette
							int parentPaletteReference = pParentPaletteReferenceScalar->GetAsInt();
							TupperwareAggregate *pParentImageAgg = m_tupImagePool.GetImage(parentPaletteReference);
							// get texture id (this is the index into the pTextureChunkRecordArray
							int parentTextureID = (pParentImageAgg->FindListByKey("Annotate_TextureIDs")->GetAsInt())[m_currentPartition];
							assert(parentTextureID<frameNum); // this better be before me
							// use the same offset as my parent
							textureChunkRecord.u32PaletteOffset = textureChunkRecordArray[parentTextureID][0].u32PaletteOffset;
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
						TupperwareScalar *pParentPaletteFrameScalar = pImageDataFrameAgg->FindScalarByKey("Annotate_ParentPaletteFrame");
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
						TupperwareScalar *pParentPaletteReferenceScalar = pImageAgg->FindScalarByKey("Annotate_ParentPaletteReference");
						if (pParentPaletteReferenceScalar)
						{
							bSavePalette = false; // we are using a parent palette
							int parentPaletteReference = pParentPaletteReferenceScalar->GetAsInt();
							TupperwareAggregate *pParentImageAgg = m_tupImagePool.GetImage(parentPaletteReference);
							// get texture id (this is the index into the pTextureChunkRecordArray
							int parentTextureID = (pParentImageAgg->FindListByKey("Annotate_TextureIDs")->GetAsInt())[m_currentPartition];
							assert(parentTextureID<frameNum); // this better be before me
							// use the same offset as my parent
							textureChunkRecord.u32PaletteOffset = textureChunkRecordArray[parentTextureID][0].u32PaletteOffset;
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
#ifdef EXPORT_PS2
	if (imageSize >= (1024 * 1024 / 2))
	{
		int bytesPerPixel = imageSize/(width*height);
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_ORANGE|LogFlags::FONT_STYLE_BOLD,"Warning - Texture too big for PS2 %s - width = %d, height = %d, bytesPerPixel = %d",pImageFileName,width,height,bytesPerPixel);
	}
#endif
}

void ExportTexture::SetTextureRecord(TupperwareAggregate *pImageAgg,TupArray<TupArray<ts_TextureChunkRecord> > &textureChunkRecordArray,int frameNum,int mapIndex,bool bAnimation)
{
	ts_TextureChunkRecord &textureChunkRecord = textureChunkRecordArray[frameNum][mapIndex];

	FILE *pDBLFile = DBLUtil::GetDBLFile();

	TupImage tupImage(pImageAgg);
	char *pImageFileName;
	// get filename for image
	tupImage.GetFilePathName(&pImageFileName);
	// get image data from image manager
	ImageManager &imageManager = m_dataManager.GetImageManager();
	TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(pImageFileName);
	assert(pImageDataAgg);
	// get the first frame from the image data
	TupImageData tupImageData(pImageDataAgg);
	int numFrames = tupImageData.GetNumImageDataFrames();
	assert(numFrames>0);
	// mark texture as animated if using more than 1 frame and not part of an animated texture set)
	if (!bAnimation && numFrames>1)
		textureChunkRecord.u32Flags |= TEX_FLAG_ANIMATED_TEXTURE;
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
	TupperwareScalar *pDiffuseMapScalar = pImageAgg->FindScalarByKey("Annotate_DiffuseMap");
	if (pDiffuseMapScalar && pDiffuseMapScalar->GetAsInt() == TRUE)
		textureChunkRecord.u32Flags |= TEX_FLAG_DIFFUSE;

	// mark Reflects textures
	TupperwareScalar *pReflectsMapScalar = pImageAgg->FindScalarByKey("Annotate_Reflects");
	if (pReflectsMapScalar && pReflectsMapScalar->GetAsInt() == TRUE)
		textureChunkRecord.u32Flags |= TEX_FLAG_REFLECTS;

	// mark Aperture textures
	TupperwareScalar *pApertureMapScalar = pImageAgg->FindScalarByKey("Annotate_Aperture");
	if (pApertureMapScalar && pApertureMapScalar->GetAsInt() == TRUE)
		textureChunkRecord.u32Flags |= TEX_FLAG_APERTURE;
	
	// copy name for easier debugging in game engine 
	// copy right part of name
	int startingNameIndex = strlen(pImageFileName)-(TEXTURE_NAME_LENGTH-1);
	if (startingNameIndex<0)
		startingNameIndex=0;

	strncpy(textureChunkRecord.acTextureName, &pImageFileName[startingNameIndex], TEXTURE_NAME_LENGTH);

	textureChunkRecord.acTextureName[TEXTURE_NAME_LENGTH - 1] = 0;	// make sure it's null-terminated
}

void ExportTexture::WriteXBoxRDFFile(void)
{
	TupString RDFFileName = m_pDBLFileName;
	FilePath::ChangeExtension(RDFFileName,".RDF");

	FILE* fp;
	fp = fopen( RDFFileName, "wb" );
	if ( !fp )
		return;

	Keeper<TupString> nameKeeper;

	int numImages = m_tupImagePool.GetNumImages();
	int imageIndex;
	for (imageIndex = 0; imageIndex < numImages; imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_tupImagePool.GetImage(imageIndex);
		if (!pImageAgg)
			continue;
		TupperwareList *pTextureIDList = pImageAgg->FindListByKey("Annotate_TextureIDs");
		if (pTextureIDList)
		{
			int *pTextureIDs = pTextureIDList->GetAsInt();
			int textureID = pTextureIDs[m_currentPartition];
			// if not used or referencing the world partition
			if (textureID==-1 || (textureID&0xff000000)!=0)
				continue;

			char fileName[_MAX_PATH];
			char *pActualFileName = pImageAgg->FindScalarByKey("Annotate_ActualFilePath")->GetAsString();
			_splitpath(pActualFileName,NULL,NULL,fileName,NULL);

			int oldKeeperSize = nameKeeper.GetSize();
			int newKeeperIndex = nameKeeper.Add(fileName);
			if (newKeeperIndex==oldKeeperSize)
			{
				TupString formatName;
				int formatFlags = pImageAgg->FindScalarByKey("Annotate_ImageFlags")->GetAsInt();
				int imageHasAlpha = pImageAgg->FindScalarByKey("Annotate_ImageHasAlpha")->GetAsInt();
				int imageHas8BitAlpha = pImageAgg->FindScalarByKey("Annotate_ImageHas8BitAlpha")->GetAsInt();
				switch (formatFlags & TupImageDataFrame::FORMAT_MASK)
				{
					case TupImageDataFrame::RGBA_8888:
						formatName = "D3DFMT_A8R8G8B8";
						break;
					case TupImageDataFrame::RGB_888:
						formatName = "D3DFMT_X8R8G8B8";
						break;
					case TupImageDataFrame::RGB_565:
					case TupImageDataFrame::RGB_555:
						formatName = "D3DFMT_R5G6B5";
						break;
					case TupImageDataFrame::RGBA_1555:
						formatName = "D3DFMT_A1R5G5B5";
						break;
					case TupImageDataFrame::INDEXED_8:
						formatName = "D3DFMT_A8R8G8B8";			// PAB changed since bundler doesnt like P8
//						formatName = "D3DFMT_P8";					// may need to revisit this later
						break;
					case TupImageDataFrame::INDEXED_4:
						formatName = "D3DFMT_A8R8G8B8";			// PAB changed since bundler doesnt like P8
//						formatName = "D3DFMT_P8";
						break;
				}

				// fix issues with bad names.. bundler doenst like spaces,-,[,]
				TupString	fname;
				fname = fileName;
				fname.Replace("-", "_");
				fname.Replace(" ", "_");
				fname.Replace("[", "_");
				fname.Replace("]", "");

				fprintf( fp, "Texture X_%s\r\n", fname );
				fprintf( fp, "{\r\n");
				fprintf( fp, "\tSource \"%s\"\r\n", pActualFileName);
				fprintf( fp, "\tFormat \"%s\"\r\n", (const char *)formatName);
				fprintf( fp, "\tWidth %d\r\n",pImageAgg->FindScalarByKey("Annotate_ImageWidth")->GetAsInt());
				fprintf( fp, "\tHeight %d\r\n",pImageAgg->FindScalarByKey("Annotate_ImageHeight")->GetAsInt());
				fprintf( fp, "}\r\n");
			}
		}
	}

	// our make process needs a file or it quits out..
	// so try to make a dummy file here
	if (numImages == 0)
	{
		fprintf ( fp, "\n");
	}


	fclose( fp );
}

u32 ExportTexture::WriteTextureNames(void)
{
	FILE *pDBLFile = DBLUtil::GetDBLFile();

	Keeper<TupString> nameKeeper;
	TupArray<int> nameIds;
	TupArray<TupString> filePathNames;

	int numImages = m_tupImagePool.GetNumImages();
	int imageIndex;
	int textureIndex;
	for (textureIndex = 0,imageIndex = 0; imageIndex < numImages; imageIndex++)
	{
		TupperwareAggregate *pImageAgg = m_tupImagePool.GetImage(imageIndex);
		if (!pImageAgg)
			continue;
		TupperwareList *pTextureIDList = pImageAgg->FindListByKey("Annotate_TextureIDs");
		if (pTextureIDList)
		{
			int *pTextureIDs = pTextureIDList->GetAsInt();
			int textureID = pTextureIDs[m_currentPartition];
			// if not used or referencing the world partition
			if (textureID==-1 || (textureID&0xff000000)!=0)
				continue;

			char fileName[_MAX_PATH];
			char *pActualFileName = pImageAgg->FindScalarByKey("Annotate_ActualFilePath")->GetAsString();
			_splitpath(pActualFileName,NULL,NULL,fileName,NULL);

			int oldKeeperSize = nameKeeper.GetSize();
			int newKeeperIndex = nameKeeper.Add(fileName);
			if (newKeeperIndex==oldKeeperSize)
			{
				nameIds.Add(textureIndex);
				filePathNames.Add(pActualFileName);
			}
			else
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
					"Warning Name %s used for more than one texture",(const char *)fileName);
			}
			textureIndex++;
		}
	}

	int numNames = nameIds.GetSize();
	if (numNames==0)
		return 0;

	DBLUtil::StartChunk(DB_TEXTURE_NAMES, DB_TEXTURE_NAMES_VERSION, DBL_CHUNK_NO_IGNORE, "1000");
	DBLUtil::StartRecordList(numNames, RECORD_LIST_HAS_OFFSET_TABLE, 0);

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Texture Names");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Index  ID       Name                              FilePath");

	for (int nameIndex = 0; nameIndex < numNames; nameIndex++)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"%4d  %4d   %-30s  %s",nameIndex,nameIds[nameIndex],nameKeeper[nameIndex],filePathNames[nameIndex]);

		DBLUtil::RecordListEntry();

		int textureID = nameIds[nameIndex];
		DBLWrite::WriteRecord(&textureID,pDBLFile);
		const char *pName = nameKeeper[nameIndex];
		DBLWrite::WriteData(pName,nameKeeper[nameIndex].GetLength()+1,pDBLFile);
		DBLUtil::AlignFile();
	}
	DBLUtil::FinishRecordList();
	return DBLUtil::FinishChunk(FALSE);
}

