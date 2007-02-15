////////////////////////////////////////////////////////////////////////////
//
// ProcessTup
//
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "ExportTexturesPch.h"
#include "ProcessTup.h"
#include "PS2TextureInfo.h"

u32 ProcessTup::WriteTextures(void)
{
	FILE *pDBLFile = DBLUtil::GetDBLFile();

	int numTextures = m_textureEntries.GetSize();
	if (numTextures==0)
	{
		return 0;
	}

	ts_TextureChunkHeader textureChunkHeader;
	memset(&textureChunkHeader, 0, sizeof(ts_TextureChunkHeader));
	textureChunkHeader.u32TextureCount = numTextures;
	strncpy(textureChunkHeader.Name, m_outputFileName, TEXTURE_SET_NAME_LENGTH);
	textureChunkHeader.Name[TEXTURE_SET_NAME_LENGTH - 1] = 0;	// make sure it's null-terminated

	DBLUtil::StartChunk(DB_TEXTURE_SET, DB_TEXTURE_SET_VERSION, DBL_CHUNK_NO_IGNORE, "1000");

	/* header contains one texture chunk record (an array of one entry), write out header without that record. */
	/* this is placeholder only, header will be re-written when complete */

	DBLWrite::WriteRecord(&textureChunkHeader, pDBLFile);

	TupArray<TupArray<ts_TextureChunkRecord> > textureChunkRecordArray;
	TupArray<TupArray<int> > textureHeaderChunkOffsetArray;

	textureChunkRecordArray.SetSize(numTextures);
	textureHeaderChunkOffsetArray.SetSize(numTextures);

	PluginLog::SetProgressText("Writing Textures");

	int textureIndex;
	for (textureIndex = 0; textureIndex < numTextures; textureIndex++)
	{
		TextureEntry &textureEntry = m_textureEntries[textureIndex];
		TupperwareAggregate *pTextureAgg = textureEntry.m_pTextureAgg;

		int numMaps = 1; // start with the base map
		float mipMapDistance = 0.0f;

		TupperwareScalar *pMipMappingScalar = pTextureAgg->FindScalarByKey("ExportTextures_MipMapping");
		// does this one have mip maps
		if (pMipMappingScalar)
		{
			numMaps += pTextureAgg->FindScalarByKey("ExportTextures_MipMapNumSubMaps")->GetAsInt();
			mipMapDistance = pTextureAgg->FindScalarByKey("ExportTextures_MipMapDistance")->GetAsFloat();
		}
		textureChunkRecordArray[textureIndex].SetSize(numMaps);
		textureHeaderChunkOffsetArray[textureIndex].SetSize(numMaps);
		textureChunkRecordArray[textureIndex][0].u16MipmapDistance = (u16)mipMapDistance;
	}

	// write out empty texture chunk records-- placeholder only, these will be re-written when complete 
	for (textureIndex = 0; textureIndex < numTextures; textureIndex++)
	{
		// add position of the start of the chunk record 
		textureHeaderChunkOffsetArray[textureIndex][0] = ftell(pDBLFile) - DBLUtil::GetChunkDataStart();
		DBLWrite::WriteRecord(&textureChunkRecordArray[textureIndex][0], pDBLFile);
	}

	// write out empty texture chunk records for the submaps and link the parents through
	for (textureIndex = 0; textureIndex < numTextures; textureIndex++)
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

	if (m_platform == PLATFORM_PS2)
	{
		PS2TextureInfo ps2TextureInfo(m_textureEntries,m_pRootAgg);
		ps2TextureInfo.AnalyzeTextures();
		for (textureIndex = 0; textureIndex < numTextures; textureIndex++)
		{
			TextureEntry &textureEntry = m_textureEntries[textureIndex];
			TupperwareAggregate *pTextureAgg = textureEntry.m_pTextureAgg;

			int numMaps = textureChunkRecordArray[textureIndex].GetSize();

			for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
			{
				const PS2TextureInfo::AnalyzedTextureData &textureData = ps2TextureInfo.GetAnalyzedTextureData(textureIndex,mapIndex);
				textureChunkRecordArray[textureIndex][mapIndex].s16TextureBlock = (s16) textureData.m_textureBlock;
				textureChunkRecordArray[textureIndex][mapIndex].s16PaletteBlock = (s16) textureData.m_paletteBlock;
				textureChunkRecordArray[textureIndex][mapIndex].s16ImageBlocks = 0;
				textureChunkRecordArray[textureIndex][mapIndex].u32SubSetNumber = 0;
			}
		}
		textureChunkHeader.u32NumTexSubSets = 1;
		textureChunkHeader.u32VRAMInfoTableOffset = ftell(pDBLFile) - DBLUtil::GetChunkDataStart(); 
		// write vram info
		u32 vramBlockSize = ps2TextureInfo.GetTextureSetVRAMBlockSize();
		DBLWrite::WriteRecord(&vramBlockSize, pDBLFile);
	}
   else
	{
		textureChunkHeader.u32VRAMInfoTableOffset = 0;
		textureChunkHeader.u32NumTexSubSets = 1;
	}

	// output textures 
	int recordCount = 0;
	for (textureIndex = 0; textureIndex < numTextures; textureIndex++)
	{
		PluginLog::SetProgressBar(0,numTextures-1,textureIndex);

		if (PluginLog::GetAbortFlag()) throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));

		TextureEntry &textureEntry = m_textureEntries[textureIndex];
		TupperwareAggregate *pTextureAgg = textureEntry.m_pTextureAgg;

		int numMaps = textureChunkRecordArray[textureIndex].GetSize();
		for (int mapIndex=0;mapIndex<numMaps;mapIndex++)
		{
			switch (m_platform)
			{
			case PLATFORM_PC:
			case PLATFORM_PS2:
			case PLATFORM_XBOX:
			case PLATFORM_DX:
				SetPS2TextureRecord(textureEntry,textureChunkRecordArray,textureIndex,mapIndex,false);
				WritePS2Texture(textureEntry,textureChunkRecordArray,textureIndex,mapIndex,false);
				break;
			case PLATFORM_GCN:
				SetGCNTextureRecord(textureEntry,textureChunkRecordArray,textureIndex,mapIndex,false);
				WriteGCNTexture(textureEntry,textureChunkRecordArray,textureIndex,mapIndex,false);
				break;
			}
			recordCount++;
		}
	}

	/* rewrite texture chunk header and records now that they are filled in */
	fseek(pDBLFile, DBLUtil::GetChunkDataStart(), SEEK_SET);
	DBLWrite::WriteRecord(&textureChunkHeader, pDBLFile);
	for (textureIndex = 0; textureIndex < numTextures; textureIndex++)
	{
		/* write out record */
		assert(textureHeaderChunkOffsetArray[textureIndex][0] == ftell(pDBLFile) - DBLUtil::GetChunkDataStart());	// must be in same place in file
		DBLWrite::WriteRecord(&textureChunkRecordArray[textureIndex][0], pDBLFile);
	}
	// sub maps
	for (textureIndex = 0; textureIndex < numTextures; textureIndex++)
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

u32 ProcessTup::WriteAnimatedTextures(void)
{
	FILE *pDBLFile = DBLUtil::GetDBLFile();
	u32 chunkSize = 0;

	PluginLog::SetProgressText("Writing Animated Textures");

	int numTextures = m_textureEntries.GetSize();

	for (int textureIndex=0;textureIndex<numTextures;textureIndex++)
	{
		PluginLog::SetProgressBar(0,numTextures-1,textureIndex);

		TextureEntry &textureEntry = m_textureEntries[textureIndex];
		TupperwareAggregate *pTextureAgg = textureEntry.m_pTextureAgg;

		// get image data from image manager
		ImageManager &imageManager = m_dataManager.GetImageManager();
		TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(textureEntry.m_filePathName);
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

			animatedTextureChunkHeader.u32TextureSlot = textureIndex;

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
				switch (m_platform)
				{
					case PLATFORM_PC:
					case PLATFORM_PS2:
					case PLATFORM_XBOX:
					case PLATFORM_DX:
						SetPS2TextureRecord(textureEntry,textureChunkRecordArray,frameIndex,0,true);
						WritePS2Texture(textureEntry,textureChunkRecordArray,frameIndex,0,true);
						break;
					case PLATFORM_GCN:
						SetGCNTextureRecord(textureEntry,textureChunkRecordArray,frameIndex,0,true);
						WriteGCNTexture(textureEntry,textureChunkRecordArray,frameIndex,0,true);
						break;
				}
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
	return chunkSize;
}

