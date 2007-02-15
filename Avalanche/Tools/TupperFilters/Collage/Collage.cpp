////////////////////////////////////////////////////////////////////////////
//
// Collage
//
// Class to handle the Collage Image Reader Plugin
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "CollagePch.h"
#include "Collage.h"
#include "resource.h"

char *CollageImageReader::m_extensionStrings[TOTAL_EXTENSIONS] = 
{
	".BMP",".DIB",".RLE",	// BMP FILE
	".JPG",".JPEG",".JIF",	// JPEG FILE
	".PNG",					// PNG FILE
	".PCT",					// PICT FILE
	".TIF",".EPS",			// TIFF FILE
	".TGA",					// TGA FILE
	".WMF",".EMF",			// WINDOW METAFILE
	".PCX",					// PCX
	".PGM",					// PORTABLE GREYMAP FILE
	".PPM",					// PORTABLE PIXMAP FILE
	".PSD",					// PHOTOSHOP FILE
	".GIF",					// GIF
};

int CollageImageReader::GetTotalSupportedExtensions(void)
{
	return TOTAL_EXTENSIONS;
}

const char *CollageImageReader::GetExtension(int extensionIndex)
{
	return m_extensionStrings[extensionIndex];
}

void CollageImageReader::GetImageFileDependencies(const char *filename,const FilterOptions &options,FilePathArray &filePathArray)
{
	PluginLog::SetLog(&m_log);

	const char *pPlatform = options.GetValue("Platform");
	TupString fileString(filename);
	fileString.MakeLower();

	if (strstr(fileString, "_atx") != NULL)
	{
		char InfoFileName[256];
		try
		{
			char FileDrive[_MAX_DRIVE];
			char FileDir[_MAX_DIR];
			char FileRoot[_MAX_FNAME];
			_splitpath(filename, FileDrive, FileDir, FileRoot, NULL);
			strcat(FileDir, "anim\\");
			_makepath(InfoFileName, FileDrive, FileDir, FileRoot, ".lst");
			filePathArray.AddFilePath(InfoFileName,FilePathArray::FILE_INPUT|FilePathArray::FILE_ACCESS_WARNING);

			if (_access(InfoFileName,0)==-1)
				throw(PluginException(ERROR_FATAL,"List File Not Found",InfoFileName));
			if (_access(InfoFileName,4)==-1)
				throw(PluginException(ERROR_FATAL,"List File Access Denied",InfoFileName));

			TupArray<TupString> fileList;
			GetImagesNamesFromListFile(filename,InfoFileName,fileList);
			int numImages = fileList.GetSize();
			for (int imageIndex=0;imageIndex<numImages;imageIndex++)
			{
				filePathArray.AddFilePath(fileList[imageIndex],FilePathArray::FILE_INPUT|FilePathArray::FILE_ACCESS_WARNING);
			}
		}
		catch (PluginException e)
		{
		}
	}
	else // single frame
	{
		filePathArray.AddFilePath(filename,FilePathArray::FILE_INPUT|FilePathArray::FILE_ACCESS_WARNING);
	}
}

TupperwareAggregate *CollageImageReader::GetImageData(const char *filename,const FilterOptions &options,const char *pAttributeString,TupConfigRoot *pConfigRoot)
{
	PluginLog::SetLog(&m_log);

	if (!pConfigRoot)
	{
//		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_ORANGE|LogFlags::FONT_STYLE_BOLD,"Warning GetImageData for %s called without attribute tree",filename);
	}

	bool bUse32BitImages = false;
	const char *pUse32BitImages = options.GetValue("Use24BitImages");
	if (pUse32BitImages && !stricmp(pUse32BitImages,"true"))
	{
		bUse32BitImages = true;
	}
	pUse32BitImages = options.GetValue("Use32BitImages");
	if (pUse32BitImages && !stricmp(pUse32BitImages,"true"))
	{
		bUse32BitImages = true;
	}

	bool bForceAnimatedTo32Bit = false;
	const char *pForceAnimatedTo32Bit = options.GetValue("ForceAnimatedTo32Bit");
	if (pForceAnimatedTo32Bit && !stricmp(pForceAnimatedTo32Bit,"true"))
	{
		bForceAnimatedTo32Bit = true;
	}

   bool bForceImagesTo32Bit = false;
   const char *pForceImagesTo32Bit = options.GetValue("ForceImagesTo32Bit");
   if (pForceImagesTo32Bit && !stricmp(pForceImagesTo32Bit,"true"))
   {
      bForceImagesTo32Bit = true;
   }

   bool bAllowPalettedImages = true;
   const char *pAllowPalettedImages = options.GetValue("AllowPalettedImages");
   if (pAllowPalettedImages && !stricmp(pAllowPalettedImages,"false"))
   {
      bAllowPalettedImages = false;
   }

	// allocate aggregate to hold the image data
	TupperwareAggregate *pRootAgg = new TupperwareAggregate;
	TupImageData tupImageData(pRootAgg);

	TupString fileString(filename);
	fileString.MakeLower();

	// if it is more than 1 frame
	if (strstr(fileString, "_atx") != NULL)
	{
		char InfoFileName[256];
		try
		{
			char FileDrive[_MAX_DRIVE];
			char FileDir[_MAX_DIR];
			char FileRoot[_MAX_FNAME];
			_splitpath(filename, FileDrive, FileDir, FileRoot, NULL);
			strcat(FileDir, "anim\\");
			_makepath(InfoFileName, FileDrive, FileDir, FileRoot, ".lst");

			if (_access(InfoFileName,0)==-1)
				throw(PluginException(ERROR_FATAL,"List File Not Found",InfoFileName));
			if (_access(InfoFileName,4)==-1)
				throw(PluginException(ERROR_FATAL,"List File Access Denied",InfoFileName));

			TupArray<TupString> fileList;
			GetImagesNamesFromListFile(filename,InfoFileName,fileList);
			int numImages = fileList.GetSize();
			for (int imageIndex=0;imageIndex<numImages;imageIndex++)
			{
				GetImageFromFile(fileList[imageIndex],tupImageData);
			}
		}
		catch (PluginException e)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_ORANGE|LogFlags::FONT_STYLE_BOLD,"Warning Cannot Load %s - %s, Substituting Dummy Texture",InfoFileName,(const char *)(e));
			try
			{
				PLAnyPicDecoder decoder;
				PLWinBmp image;
				decoder.MakeBmpFromResource(hInstance,IDR_BADFILE,&image,0);
				TupperwareAggregate *pFrameAgg = AddImageDataFrame(tupImageData,image);
				pFrameAgg->AddScalarString("Collage_SourceFileName",NULL,"Dummy");
				tupImageData.GetBaseAggregate()->AddScalarInt("Dummy",NULL,1);
			}
			catch (PLTextException e)
			{
				throw(PluginException(ERROR_FATAL,"Cannot use dummy texture"));
			}
		}
	}
	else // single frame
	{
		GetImageFromFile(filename,tupImageData);
	}
	// analyze image data and get stats on frames
	ImageAnalyze imageAnalyze;
	imageAnalyze.AddImageData(tupImageData);
	ImageAnalyze::Stats stats;
	imageAnalyze.CalculateStats(stats);
	if (stats.m_maxWidthUsed!=stats.m_minWidthUsed || 
		stats.m_maxHeightUsed!=stats.m_minHeightUsed)
	{
		// make uniform scale
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW|LogFlags::FONT_COLOR_DK_GREEN|LogFlags::FONT_STYLE_ITALIC,
			"%d Bitmaps from %s min size = %d,%d max size %d,%d scaling to min size",
			stats.m_numMaps,filename,
			stats.m_minWidthUsed,stats.m_minHeightUsed,
			stats.m_maxWidthUsed,stats.m_maxHeightUsed);
		ImageUtilities::Scale(tupImageData,stats.m_minWidthUsed,stats.m_minHeightUsed);
	}
	int bestFormat;
	// determine the best format for the frame(s)
	TupString formatString;
	if (bForceImagesTo32Bit || 
       (stats.m_numMaps>1 && bForceAnimatedTo32Bit))
	{
		bestFormat = TupImageDataFrame::RGBA_8888;
		formatString = "32 bit RGBA 8888";
	}
	else if (bUse32BitImages)
	{
		if (stats.m_minColorsUsed<=16 && stats.m_maxColorsUsed<=32 && bAllowPalettedImages)
		{
			bestFormat = TupImageDataFrame::INDEXED_4;
			formatString = "4 bit Indexed";
		}
		else if (stats.m_minColorsUsed<=256 && stats.m_maxColorsUsed<=512 && bAllowPalettedImages)
		{
			bestFormat = TupImageDataFrame::INDEXED_8;
			formatString = "8 bit Indexed";
		}
		else //if (stats.m_minAlphaUsed>1)
		{
			bestFormat = TupImageDataFrame::RGBA_8888;
			formatString = "32 bit RGBA 8888";
		}
//		else
//		{
//			bestFormat = TupImageDataFrame::RGB_888;
//			formatString = "24 bit RGB 888";
//		}
	}
	else
	{
		if (stats.m_minColorsUsed<=16 && stats.m_maxColorsUsed<=32 && bAllowPalettedImages)
		{
			bestFormat = TupImageDataFrame::INDEXED_4;
			formatString = "4 bit Indexed";
		}
		else if (stats.m_minColorsUsed<=256 && stats.m_maxColorsUsed<=512 && bAllowPalettedImages)
		{
			bestFormat = TupImageDataFrame::INDEXED_8;
			formatString = "8 bit Indexed";
		}
      // clear alpha across entire image
      else if (stats.m_maxAlphaUsed==1 && stats.m_minAlphaValueUsed == 0 && stats.m_maxAlphaValueUsed == 0)
      {
         bestFormat = TupImageDataFrame::RGBA_1555;
         formatString = "16 bit RGBA 1555";
      }
      // uses 2 alpha values (0 & 255)
		else if (stats.m_maxAlphaUsed==2 && stats.m_minAlphaValueUsed == 0 && stats.m_maxAlphaValueUsed == 255)
		{
			bestFormat = TupImageDataFrame::RGBA_1555;
			formatString = "16 bit RGBA 1555";
		}
      // uses more than 1 alpha value or the lowest alpha is not fully opaque
      else if (stats.m_maxAlphaUsed!=1 || stats.m_minAlphaValueUsed != 255)
      {
         bestFormat = TupImageDataFrame::RGBA_8888;
         formatString = "32 bit RGBA 8888";
      }
      // uses 1 alpha that is fully opaque
      else
      {
         bestFormat = TupImageDataFrame::RGB_565;
         formatString = "16 bit RGB 565";
      }
	}
	if (stats.m_numMaps==1)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW|LogFlags::FONT_COLOR_DK_GREEN|LogFlags::FONT_STYLE_ITALIC,
			"Bitmap from %s using %d colors and %d alpha values (%d->%d) - Converting to %s",
			filename,stats.m_minColorsUsed,stats.m_minAlphaUsed,stats.m_minAlphaValueUsed,stats.m_maxAlphaValueUsed,(const char *)formatString);
	}
	else
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW|LogFlags::FONT_COLOR_DK_GREEN|LogFlags::FONT_STYLE_ITALIC,
			"%d Bitmaps from %s using from %d to %d colors and from %d to %d alpha values (%d->%d) - Converting to %s",
			stats.m_numMaps,filename,
			stats.m_minColorsUsed,stats.m_maxColorsUsed,
			stats.m_minAlphaUsed,stats.m_maxAlphaUsed,
			stats.m_minAlphaValueUsed,stats.m_maxAlphaValueUsed,
			(const char *) formatString);
	}

	// change format of images to best format
	ImageUtilities::ConvertToFormat(tupImageData,bestFormat);

	return pRootAgg;
}

TupperwareAggregate *CollageImageReader::GetDummyImageData(const char *filename,const FilterOptions &options,const char *pAttributeString,TupConfigRoot *pConfigRoot)
{
	PluginLog::SetLog(&m_log);

	bool bUse32BitImages = false;
	const char *pUse32BitImages = options.GetValue("Use24BitImages");
	if (pUse32BitImages && !stricmp(pUse32BitImages,"true"))
	{
		bUse32BitImages = true;
	}
	pUse32BitImages = options.GetValue("Use32BitImages");
	if (pUse32BitImages && !stricmp(pUse32BitImages,"true"))
	{
		bUse32BitImages = true;
	}

	bool bForceAnimatedTo32Bit = false;
	const char *pForceAnimatedTo32Bit = options.GetValue("ForceAnimatedTo32Bit");
	if (pForceAnimatedTo32Bit && !stricmp(pForceAnimatedTo32Bit,"true"))
	{
		bForceAnimatedTo32Bit = true;
	}

   bool bForceImagesTo32Bit = false;
   const char *pForceImagesTo32Bit = options.GetValue("ForceImagesTo32Bit");
   if (pForceImagesTo32Bit && !stricmp(pForceImagesTo32Bit,"true"))
   {
      bForceImagesTo32Bit = true;
   }

   bool bAllowPalettedImages = true;
   const char *pAllowPalettedImages = options.GetValue("AllowPalettedImages");
   if (pAllowPalettedImages && !stricmp(pAllowPalettedImages,"false"))
   {
      bAllowPalettedImages = false;
   }

	// allocate aggregate to hold the image data
	TupperwareAggregate *pRootAgg = new TupperwareAggregate;
	TupImageData tupImageData(pRootAgg);

	TupString fileString(filename);
	fileString.MakeLower();

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_ORANGE|LogFlags::FONT_STYLE_BOLD,"Unable to read %s - unrecognized format - Substituting Dummy Texture",(const char *)fileString);
	try
	{
		PLAnyPicDecoder decoder;
		PLWinBmp image;
		decoder.MakeBmpFromResource(hInstance,IDR_BADFILE,&image,0);
		TupperwareAggregate *pFrameAgg = AddImageDataFrame(tupImageData,image);
		pFrameAgg->AddScalarString("Collage_SourceFileName",NULL,"Dummy");
		tupImageData.GetBaseAggregate()->AddScalarInt("Dummy",NULL,1);
	}
	catch (PLTextException e)
	{
		throw(PluginException(ERROR_FATAL,"Cannot use dummy texture"));
	}

	// analyze image data and get stats on frames
	ImageAnalyze imageAnalyze;
	imageAnalyze.AddImageData(tupImageData);
	ImageAnalyze::Stats stats;
	imageAnalyze.CalculateStats(stats);
	if (stats.m_maxWidthUsed!=stats.m_minWidthUsed || 
		stats.m_maxHeightUsed!=stats.m_minHeightUsed)
	{
		// make uniform scale
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW|LogFlags::FONT_COLOR_DK_GREEN|LogFlags::FONT_STYLE_ITALIC,
			"%d Bitmaps from %s min size = %d,%d max size %d,%d scaling to min size",
			stats.m_numMaps,filename,
			stats.m_minWidthUsed,stats.m_minHeightUsed,
			stats.m_maxWidthUsed,stats.m_maxHeightUsed);
		ImageUtilities::Scale(tupImageData,stats.m_minWidthUsed,stats.m_minHeightUsed);
	}
	int bestFormat;
	// determine the best format for the frame(s)
	TupString formatString;
	if (bForceImagesTo32Bit ||
       (stats.m_numMaps>1 && bForceAnimatedTo32Bit))
	{
		bestFormat = TupImageDataFrame::RGBA_8888;
		formatString = "32 bit RGBA 8888";
	}
	else if (bUse32BitImages)
	{
		if (stats.m_minColorsUsed<=16 && stats.m_maxColorsUsed<=32 && bAllowPalettedImages)
		{
			bestFormat = TupImageDataFrame::INDEXED_4;
			formatString = "4 bit Indexed";
		}
		else if (stats.m_minColorsUsed<=256 && stats.m_maxColorsUsed<=512 && bAllowPalettedImages)
		{
			bestFormat = TupImageDataFrame::INDEXED_8;
			formatString = "8 bit Indexed";
		}
		else //if (stats.m_minAlphaUsed>1)
		{
			bestFormat = TupImageDataFrame::RGBA_8888;
			formatString = "32 bit RGBA 8888";
		}
		//		else
		//		{
		//			bestFormat = TupImageDataFrame::RGB_888;
		//			formatString = "24 bit RGB 888";
		//		}
	}
	else
	{
		if (stats.m_minColorsUsed<=16 && stats.m_maxColorsUsed<=32 && bAllowPalettedImages)
		{
			bestFormat = TupImageDataFrame::INDEXED_4;
			formatString = "4 bit Indexed";
		}
		else if (stats.m_minColorsUsed<=256 && stats.m_maxColorsUsed<=512 && bAllowPalettedImages)
		{
			bestFormat = TupImageDataFrame::INDEXED_8;
			formatString = "8 bit Indexed";
		}
      // clear alpha across entire image
      else if (stats.m_maxAlphaUsed==1 && stats.m_minAlphaValueUsed == 0 && stats.m_maxAlphaValueUsed == 0)
      {
         bestFormat = TupImageDataFrame::RGBA_1555;
         formatString = "16 bit RGBA 1555";
      }
      // uses 2 alpha values (0 & 255)
      else if (stats.m_maxAlphaUsed==2 && stats.m_minAlphaValueUsed == 0 && stats.m_maxAlphaValueUsed == 255)
      {
         bestFormat = TupImageDataFrame::RGBA_1555;
         formatString = "16 bit RGBA 1555";
      }
      // uses more than 1 alpha value or the lowest alpha is not fully opaque
      else if (stats.m_maxAlphaUsed!=1 || stats.m_minAlphaValueUsed != 255)
      {
         bestFormat = TupImageDataFrame::RGBA_8888;
         formatString = "32 bit RGBA 8888";
      }
      // uses 1 alpha that is fully opaque
      else
      {
         bestFormat = TupImageDataFrame::RGB_565;
         formatString = "16 bit RGB 565";
      }
	}
	if (stats.m_numMaps==1)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW|LogFlags::FONT_COLOR_DK_GREEN|LogFlags::FONT_STYLE_ITALIC,
			"Bitmap from %s using %d colors and %d alpha values (%d->%d) - Converting to %s",
			filename,stats.m_minColorsUsed,stats.m_minAlphaUsed,stats.m_minAlphaValueUsed,stats.m_maxAlphaValueUsed,(const char *)formatString);
	}
	else
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW|LogFlags::FONT_COLOR_DK_GREEN|LogFlags::FONT_STYLE_ITALIC,
			"%d Bitmaps from %s using from %d to %d colors and from %d to %d alpha values (%d->%d) - Converting to %s",
			stats.m_numMaps,filename,
			stats.m_minColorsUsed,stats.m_maxColorsUsed,
			stats.m_minAlphaUsed,stats.m_maxAlphaUsed,
			stats.m_minAlphaValueUsed,stats.m_maxAlphaValueUsed,
			(const char *) formatString);
	}

	// change format of images to best format
	ImageUtilities::ConvertToFormat(tupImageData,bestFormat);

	return pRootAgg;
}


void CollageImageReader::GetImagesNamesFromListFile(const char *pBaseFilename,const char *pListFilename,TupArray<TupString> &fileList)
{
	char FileDrive[_MAX_DRIVE];
	char FileDir[_MAX_DIR];
	char FileRoot[_MAX_FNAME];
	char FileExt[_MAX_EXT];

	/* open .lst file that describes animated texture set */
	AnimationListFile InfoFile;
	InfoFile.Load(pListFilename);
	InfoFile.Lowercase();

#define ANIM_INFO_VERSION 1

	if ((InfoFile.FindFirstToken("version") == FALSE) && (InfoFile.GetTokenIntValue() != ANIM_INFO_VERSION))
		throw(PluginException(ERROR_FATAL,"List File %s Wrong Version",pListFilename));

	/* count files, maybe nothing to do */

	int nAnimatedTextureCount = InfoFile.GetTokenCount("file");
	if (nAnimatedTextureCount == 0)
	{
		throw(PluginException(ERROR_FATAL,"List File %s Contains No Files",pListFilename));
	}

	/* load up file names, in animation order */

	bool bFound = InfoFile.FindFirstToken("file");
	assert(bFound);	// we've already confirmed that there are files in list file, there had better still be
	char pImageFile[_MAX_PATH];
	// load each of the images
	while (bFound)
	{

		/* copy file name */
		strcpy(pImageFile, InfoFile.GetTokenStringValue());

		/* typically just a file name, assumed to be in the same directory as the original animated texture */
		if (strchr(pImageFile, '\\') == NULL)
		{
			_splitpath(pBaseFilename, FileDrive, FileDir, NULL, NULL);
			_splitpath(pImageFile, NULL, NULL, FileRoot, FileExt);
			_makepath(pImageFile, FileDrive, FileDir, FileRoot, FileExt);
		}
		TupString newFilename(pImageFile);
		fileList.Add(newFilename);
		bFound = InfoFile.FindNextToken();
	}
}

void CollageImageReader::GetImageFromFile(const char *filename,TupImageData &tupImageData)
{
	PLAnyPicDecoder decoder;

	// Tell the decoder to fill the bitmap object with data.
	try
	{
		if (_access(filename,0)==-1)
			throw(PLTextException(PL_ERRFILE_NOT_FOUND,"File Not Found"));
		if (_access(filename,4)==-1)
			throw(PLTextException(PL_ERRACCESS_DENIED,"Access Denied"));
		// read in the image
		PLWinBmp image;
		decoder.MakeBmpFromFile (filename, &image,0); // read in native format
		TupperwareAggregate *pFrameAgg = AddImageDataFrame(tupImageData,image);
		pFrameAgg->AddScalarString("Collage_SourceFileName",NULL,filename);
	}
	catch (PLTextException e)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_ORANGE|LogFlags::FONT_STYLE_BOLD,"Warning Cannot Load %s - %s, Substituting Dummy Texture",filename,(const char *)(e));
		try
		{
			PLWinBmp image;
			decoder.MakeBmpFromResource(hInstance,IDR_BADFILE,&image,0);
			TupperwareAggregate *pFrameAgg = AddImageDataFrame(tupImageData,image);
			pFrameAgg->AddScalarString("Collage_SourceFileName",NULL,"Dummy");
			tupImageData.GetBaseAggregate()->AddScalarInt("Dummy",NULL,1);
		}
		catch (PLTextException e)
		{
			throw(PluginException(ERROR_FATAL,"Cannot use dummy texture"));
		}
	}
}


// add a 32 bit or 8 bit image data frame to the image data object
// we will do the analysis later to determine the final image format
TupperwareAggregate *CollageImageReader::AddImageDataFrame(TupImageData &tupImageData,PLWinBmp &image)
{
	int frameToAdd = tupImageData.GetNumImageDataFrames();
	// add 1 frame 
	TupperwareAggregate *pFrameAgg = tupImageData.AddImageDataFrame(frameToAdd);
	TupImageDataFrame tupImageDataFrame(pFrameAgg);

	int auxilaryFlags = 0;
	// set auxiliary flags
	tupImageDataFrame.AddAuxilaryFlags(auxilaryFlags);

	// only 1 map (no mip maps)
	TupperwareAggregate *pMapAgg = tupImageDataFrame.AddImageDataMap(0);
	TupImageDataMap tupImageDataMap(pMapAgg);
	int width = image.GetWidth();
	tupImageDataMap.AddWidth(width);
	int height = image.GetHeight();
	tupImageDataMap.AddHeight(height);

	int bitsPerPixel = image.GetBitsPerPixel();

	bool hasAlpha = image.HasAlpha();
	bool has8BitAlpha = false;
	int formatFlags = 0;
	if (bitsPerPixel==32)
	{
		PLPixel32 **pPixel32LineArray = (PLPixel32 **)image.GetLineArray();
		PLPixel32 *pPixel32Line;
		int x,y;
		// if the image has no alpha then set the alpha to 255
		if (!hasAlpha)
		{
			for (y=0; y<height; y++)
			{
				pPixel32Line = pPixel32LineArray[y];
				// Iterate through the pixels
				for (x=0; x<width; x++)
				{
					pPixel32Line[x].SetA(255);
				}
			}
		}
		// if the image has alpha- clear the hasAlpha flag if there is no alpha in the channel
		else 
		{
			bool foundTransparentAlpha = false;
			for (y=0; y<height && !foundTransparentAlpha; y++)
			{
				pPixel32Line = pPixel32LineArray[y];
				// Iterate through the pixels
				for (x=0; x<width; x++)
				{
					PLBYTE alphaValue = pPixel32Line[x].GetA();
					if (alphaValue!=255)
					{
						foundTransparentAlpha = true;
						if (alphaValue!=0)
							has8BitAlpha = true;
						break;
					}
				}
				if (foundTransparentAlpha)
					break;
			}
			if (!foundTransparentAlpha)
			{
				hasAlpha = false;
			}
		}
		formatFlags |= TupImageDataFrame::RGBA_8888;
		BYTE ** pLineArray = image.GetLineArray();

		int size = width*height*4;
		BYTE *pData = new BYTE[size];
		BYTE *pDest = pData;
		BYTE *pLine;

		for (y=0; y<height; y++)
		{
			pLine = pLineArray[y];

			// Iterate through the pixels
			for (x=0; x<width; x++)
			{
				pDest[2]=pLine[0];
				pDest[1]=pLine[1];
				pDest[0]=pLine[2];
				pDest[3]=pLine[3];
				pDest+=4;
				pLine+=4;
			}
		}
		tupImageDataMap.AddData(pData,size);
		delete pData;
	}
	// if palettized
	else if (bitsPerPixel==8)
	{
		assert(!hasAlpha); // this better not be set

		formatFlags |= TupImageDataFrame::INDEXED_8;

		// save out the palette
		BYTE *pPaletteSrcData = (BYTE *)image.GetPalette();
		BYTE *pPaletteSrc = pPaletteSrcData;
		int paletteSize = 256*4;
		BYTE *pPaletteDstData = new BYTE[paletteSize];
		BYTE *pPaletteDest = pPaletteDstData;

		for (int i=0; i<256; i++)
		{
			pPaletteDest[2]=pPaletteSrc[0];
			pPaletteDest[1]=pPaletteSrc[1];
			pPaletteDest[0]=pPaletteSrc[2];
			pPaletteDest[3]=pPaletteSrc[3];
			pPaletteSrc+=4;
			pPaletteDest+=4;
		}
		TupperwareAggregate *pPaletteAgg = tupImageDataFrame.AddImageDataPalette();
		TupImageDataPalette tupImageDataPalette(pPaletteAgg);
		int paletteFlags = 0;
		tupImageDataPalette.AddFlags(paletteFlags);
		tupImageDataPalette.AddData(pPaletteDstData,paletteSize);

		hasAlpha = false;
		BYTE ** pIndexLineArray = image.GetLineArray();

		int size = width*height;
		BYTE *pData = new BYTE[size];
		BYTE *pDest = pData;
		BYTE *pIndexLine;

		int x,y;
		// Iterate through the lines and add the data
		for (y=0; y<height; y++)
		{
			pIndexLine = pIndexLineArray[y];
			// Iterate through the pixels
			for (x=0; x<width; x++)
			{
				BYTE paletteIndex = pIndexLine[x];
				BYTE paletteAlpha = pPaletteSrcData[(paletteIndex<<2)+3];
				// check if image using any palette entries with alpha
				if (paletteAlpha!=255)
				{
					hasAlpha = true;
					if (paletteAlpha!=0)
						has8BitAlpha = true;
				}
				*pDest=paletteIndex;
				pDest++;
			}
		}
		tupImageDataMap.AddData(pData,size);
		delete pData;
		delete pPaletteDstData;
	}
	// set this flag if the image actually has alpha within it
	if (hasAlpha)
		formatFlags |= TupImageDataFrame::HAS_ALPHA;
	if (has8BitAlpha)
		formatFlags |= TupImageDataFrame::HAS_8BITALPHA;
	
	tupImageDataFrame.AddFormatFlags(formatFlags);
	return pFrameAgg;
}
