////////////////////////////////////////////////////////////////////////////
//
// ProcessTup
//
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "ExportTexturesPch.h"
#include "ProcessTup.h"
#include "LoadSCFiles.h"

ProcessTup::ProcessTup(const FilterOptions &options,FilterDataManager &dataManager) : m_options(options), m_dataManager(dataManager),
	m_platform(PLATFORM_UNDEFINED),m_bLoadSCFiles(false),m_bExportXBoxRDFFile(false),m_bExportTextureData(true),
	m_scaleTextureHeight(100),m_scaleTextureWidth(100)
{
	m_pRootAgg = m_dataManager.GetSceneRoot();
	assert(m_pRootAgg);
}

ProcessTup::~ProcessTup()
{
}

void ProcessTup::GetFileDependencies(FilePathArray &filePathArray)
{
	// add output file
	TupString outputFilename(m_dataManager.GetOutputFilename());
	FilePath::ChangeExtension(outputFilename,".DBL");
	filePathArray.AddFilePath(outputFilename,FilePathArray::FILE_OUTPUT|FilePathArray::FILE_ACCESS_FATAL);

	// add texture files
	ImageManager &imageManager = m_dataManager.GetImageManager();
	GetTextureEntries();
	if (m_bLoadSCFiles)
	{
		AddSCFileTextureEntries(false);
	}

	int numTextureEntries = m_textureEntries.GetSize();
	for (int textureEntryIndex=0;textureEntryIndex<numTextureEntries;textureEntryIndex++)
	{
		TextureEntry &textureEntry	= m_textureEntries[textureEntryIndex];
		imageManager.GetImageFileDependencies(textureEntry.m_filePathName,filePathArray);
	}
}

void ProcessTup::Process(void)
{
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD,"Platform Set to %s",m_platformString);

	GetTextureEntries();
	if (m_bLoadSCFiles)
	{
		AddSCFileTextureEntries(true);
	}
	
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Loading Textures and converting to optimal formats and original sizes");
	AddTextureData();
	PluginLog::SetProgressBar(0,1,0);
	PluginLog::SetProgressText("Scaling Textures");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Changing Texture Sizes to specified amounts");
	ChangeTextureSize();

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Rotating any tall images");
	RotateTallImages();

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Changing Texture formats to specified values");
	PluginLog::SetProgressText("Changing Texture Formats");
	ChangeTextureFormats();
	AddTextureInformation();

	PluginLog::SetProgressText("Remapping Duplicate Textures");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Remapping duplicate textures");
	RemapDuplicateTextures();

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Merging texture Palettes");
	MergeTexturePalettes();
	AnalyzePaletteColors();
	AddSortedTexturesToLog();

	ExportTextures();
	if (m_bExportXBoxRDFFile)
	{
		WriteXBoxRDFFile();
	}
}

void ProcessTup::ExportTextures(void)
{
	m_outputFileName = m_dataManager.GetOutputFilename();
	FilePath::ChangeExtension(m_outputFileName,".DBL");

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting file: %s...",(const char *) m_outputFileName);

	const char *pPlatformFileString;
	bool bSwapEndan = false;
	switch (m_platform)
	{
		case PLATFORM_PC:
			pPlatformFileString = "WIN32";
			break;
		case PLATFORM_PS2:
			pPlatformFileString = "PS2";
			break;
		case PLATFORM_GCN:
			pPlatformFileString = "GCN";
			bSwapEndan = true;
			break;
		case PLATFORM_XBOX:
			pPlatformFileString = "XBOX";
			break;
		case PLATFORM_DX:
			pPlatformFileString = "DX";
			break;
		default:
		case PLATFORM_UNDEFINED:
			pPlatformFileString = "WIN32";
			m_platform = PLATFORM_PC;
			break;
	}

	int fileSize = 0;
	DBLUtil::StartDBLFile(m_outputFileName,NULL,false,pPlatformFileString,bSwapEndan);
	WriteTextureNames();
	WriteTextures();
	WriteAnimatedTextures();
	fileSize = DBLUtil::FinishDBLFile();
	DBLUtil::CleanUpDBLFile();
}

void ProcessTup::SetPlatform(const char *pPlatform)
{
	if (!stricmp(pPlatform,"PC"))
	{
		m_platform = PLATFORM_PC;
		m_platformString = pPlatform;
	}
	else if (!stricmp(pPlatform,"PS2"))
	{
		m_platform = PLATFORM_PS2;
		m_platformString = pPlatform;
	}
	else if (!stricmp(pPlatform,"GCN"))
	{
		m_platform = PLATFORM_GCN;
		m_platformString = pPlatform;
	}
	else if (!stricmp(pPlatform,"XBOX"))
	{
		m_platform = PLATFORM_XBOX;
		m_platformString = pPlatform;
	}
	else if (!stricmp(pPlatform,"DX"))
	{
		m_platform = PLATFORM_DX;
		m_platformString = pPlatform;
	}
	else
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Undefined Platform %s Defaulting to PC",(const char *) pPlatform);
		m_platform = PLATFORM_UNDEFINED;
		m_platformString = "Undefined";
	}
}

void ProcessTup::SetLoadSCFiles(bool bLoadSCFiles)
{
	m_bLoadSCFiles = bLoadSCFiles;
}

void ProcessTup::SetExportXBoxRDFFile(bool bExportXBoxRDFFile)
{
	m_bExportXBoxRDFFile = bExportXBoxRDFFile;
}

void ProcessTup::SetExportTextureData(bool bExportTextureData)
{
	m_bExportTextureData = bExportTextureData;
}

void ProcessTup::SetScaleTextureWidth(int scaleTextureWidth)
{
	m_scaleTextureWidth = scaleTextureWidth;
}

void ProcessTup::SetScaleTextureHeight(int scaleTextureHeight)
{
	m_scaleTextureHeight = scaleTextureHeight;
}


void ProcessTup::GetTextureEntries(void)
{
	// set defaults 
	TextureSettings globalSettings;
	globalSettings.m_bBestFitFormat = true;
	globalSettings.m_height = 0;
	globalSettings.m_width = 0;

	for (TupperwareIterator iter = m_pRootAgg->FindAll(); !iter.Finished(); iter.Next()) 
	{
		TupperwareAtom *pAtom = iter.GetAsAtom();
		const char *pKey =  pAtom->GetKey();
		if (!pKey) 
			continue;
		const char *pName = pAtom->GetName();
		if (!ValidPlatform(pName))
			continue;

		if (!stricmp(pKey,"Texture"))
		{
			if (pAtom->GetAtomType()!=TupperwareAtom::ATOM_AGGREGATE)
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Texture should be an aggregate");
			}
			else
			{
				TextureEntry newEntry;
				if (GetTextureEntry((TupperwareAggregate *)pAtom,newEntry,globalSettings))
				{
					int oldKeeperIndex = m_textureEntryUniqueNames.GetSize();
					int newKeeperIndex = m_textureEntryUniqueNames.Add(newEntry.m_name);
					if (oldKeeperIndex==newKeeperIndex)
					{
						m_textureEntries.Add(newEntry);
					}
					else
					{
						PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Skipping Texture %s due previous one with same name",(const char *)newEntry.m_name);
					}
				}
				else
				{
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Skipping Texture due to no filename");
				}
			}
		}
		else
		{
			UpdateSettings(pAtom,globalSettings);
		}
	}
}

bool ProcessTup::GetTextureEntry(TupperwareAggregate *pAgg,TextureEntry &entry,const TextureSettings &globalSettings)
{
	entry.m_pTextureAgg = pAgg;

	bool bSetFileName = false;
	bool bSetName = false;
	entry.m_filePathName.Empty();
	entry.m_settings = globalSettings;

	for (TupperwareIterator iter = pAgg->FindAll(); !iter.Finished(); iter.Next()) 
	{
		TupperwareAtom *pAtom = iter.GetAsAtom();
		const char *pKey =  pAtom->GetKey();
		if (!pKey) 
			continue;
		const char *pName = pAtom->GetName();
		if (!ValidPlatform(pName))
			continue;

		if (!stricmp(pKey,"FilePathName"))
		{
			if (pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
			{
				TupperwareScalar *pScalar = (TupperwareScalar*) pAtom;
				if (pScalar->GetDataType()!=TupperwareScalar::DATA_STRING)
				{
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"FilePathName Scalar should be a string");
				}
				const char *pFilePathName = pScalar->GetAsString();
				if (!pFilePathName)
				{
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"FilePathName Scalar should be a valid string");
				}
				else
				{
					if (bSetFileName)
					{
						PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"FilePathName already set ignoring %s",pFilePathName);
					}
					else
					{
						bSetFileName = true;
						entry.m_filePathName = pFilePathName;
					}
				}
			}
			else
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"FilePathName should be a scalar string");
			}
		}
		else if (!stricmp(pKey,"Name"))
		{
			if (pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
			{
				TupperwareScalar *pScalar = (TupperwareScalar*) pAtom;
				if (pScalar->GetDataType()!=TupperwareScalar::DATA_STRING)
				{
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Name Scalar should be a string");
				}
				const char *pName = pScalar->GetAsString();
				if (!pName)
				{
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Name Scalar should be a valid string");
				}
				else
				{
					if (bSetName)
					{
						PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Name already set ignoring %s",pName);
					}
					else
					{
						bSetName = true;
						entry.m_name = pName;
					}
				}
			}
			else
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Name should be a scalar string");
			}
		}
      else
		{
			UpdateSettings(pAtom,entry.m_settings);
		}
	}
	// set the name from the filename if not explicitly set
	if (!bSetName && bSetFileName)
	{
		char fname[_MAX_FNAME];
		FilePath::GetFileName(entry.m_filePathName,fname);
		entry.m_name = fname;
	}
	return bSetFileName; // return true if the name was set
}

void ProcessTup::UpdateSettings(TupperwareAtom *pAtom,TextureSettings &settings)
{
	const char *pKey = pAtom->GetKey();

	if (!stricmp(pKey,"Format"))
	{
		if (pAtom->GetAtomType()!=TupperwareAtom::ATOM_SCALAR)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Format should be a string scalar");
		}
		else
		{
			TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
			if (pScalar->GetDataType()!=TupperwareScalar::DATA_STRING)
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Format should be a string scalar");
			}
			else
			{
				const char *pFormat = pScalar->GetAsString();
				if (!pFormat)
				{
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Format should be a valid string");
				}
				else
				{
					if (!UpdateFormat(pFormat,settings.m_bBestFitFormat,settings.m_format))
					{
						PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Format %s is not a valid format",pFormat);
					}
				}
			}
		}
	}
	else if (!stricmp(pKey,"Width"))
	{
		if (pAtom->GetAtomType()!=TupperwareAtom::ATOM_SCALAR)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Width should be a scalar");
		}
		else
		{
			TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
			if (pScalar->GetDataType()!=TupperwareScalar::DATA_INT)
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Width should be a int scalar");
			}
			else
			{
				int newWidth = pScalar->GetAsInt();
				if (newWidth<0 || newWidth>4096)
				{
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Width should be between 0 and 4096 - (0 indicating use original size)");
					newWidth = 0;
				}
				settings.m_width = newWidth;
			}
		}
	}
	else if (!stricmp(pKey,"Height"))
	{
		if (pAtom->GetAtomType()!=TupperwareAtom::ATOM_SCALAR)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Height should be a scalar");
		}
		else
		{
			TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
			if (pScalar->GetDataType()!=TupperwareScalar::DATA_INT)
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Height should be a int scalar");
			}
			else
			{
				int newHeight = pScalar->GetAsInt();
				if (newHeight<0 || newHeight>4096)
				{
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Height should be between 0 and 4096 - (0 indicating use original size)");
					newHeight = 0;
				}
				settings.m_height = newHeight;
			}
		}
	}
}

bool ProcessTup::UpdateFormat(const char *pFormat,bool &bBestFitFormat,int &format)
{
	if (!stricmp(pFormat,"BestFit"))
	{
		bBestFitFormat = true;
		format = 0;
		return true;
	}
	else if (!stricmp(pFormat,"RGBA8888"))
	{
		bBestFitFormat = false;
		format = TupImageDataFrame::RGBA_8888;
		return true;
	}
	else if (!stricmp(pFormat,"RGB888"))
	{
		bBestFitFormat = false;
		format = TupImageDataFrame::RGB_888;
		return true;
	}
	else if (!stricmp(pFormat,"RGB555"))
	{
		bBestFitFormat = false;
		format = TupImageDataFrame::RGB_555;
		return true;
	}
	else if (!stricmp(pFormat,"RGB565"))
	{
		bBestFitFormat = false;
		format = TupImageDataFrame::RGB_565;
		return true;
	}
	else if (!stricmp(pFormat,"RGBA1555"))
	{
		bBestFitFormat = false;
		format = TupImageDataFrame::RGBA_1555;
		return true;
	}
	else if (!stricmp(pFormat,"8bit"))
	{
		bBestFitFormat = false;
		format = TupImageDataFrame::INDEXED_8;
		return true;
	}
	else if (!stricmp(pFormat,"4bit"))
	{
		bBestFitFormat = false;
		format = TupImageDataFrame::INDEXED_4;
		return true;
	}
	return false;
}

void ProcessTup::AddSCFileTextureEntries(bool bAddTexturesToRootAgg)
{
	// set defaults 
	TextureSettings globalSettings;
	globalSettings.m_bBestFitFormat = true;
	globalSettings.m_height = 0;
	globalSettings.m_width = 0;

	for (TupperwareIterator iter = m_pRootAgg->FindAll(); !iter.Finished(); iter.Next()) 
	{
		TupperwareAtom *pAtom = iter.GetAsAtom();
		const char *pKey =  pAtom->GetKey();
		if (!pKey) 
			continue;
		const char *pName = pAtom->GetName();
		if (!ValidPlatform(pName))
			continue;

		if (!stricmp(pKey,"SCFile"))
		{
			if (pAtom->GetAtomType()!=TupperwareAtom::ATOM_AGGREGATE)
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"SCFile should be an aggregate");
			}
			else
			{
				TupArray<TextureEntry> newEntries;
				if (GetSCFileTextureEntries((TupperwareAggregate *)pAtom,newEntries,globalSettings,bAddTexturesToRootAgg))
				{
					int numEntries = newEntries.GetSize();
					for (int entryIndex=0;entryIndex<numEntries;entryIndex++)
					{
						int oldKeeperIndex = m_textureEntryUniqueNames.GetSize();
						int newKeeperIndex = m_textureEntryUniqueNames.Add(newEntries[entryIndex].m_name);
						if (oldKeeperIndex==newKeeperIndex)
						{
							m_textureEntries.Add(newEntries[entryIndex]);
						}
						else
						{
							PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Skipping Texture %s due previous one with same name",(const char *)newEntries[entryIndex].m_name);
						}
					}
				}
				else
				{
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Skipping SCFile");
				}
			}
		}
		else
		{
			UpdateSettings(pAtom,globalSettings);
		}
	}
}

bool ProcessTup::GetSCFileTextureEntries(TupperwareAggregate *pAgg,TupArray<TextureEntry> &entries,const TextureSettings &globalSettings,bool bAddTexturesToRootAgg)
{
	bool bSetFileName = false;
	bool bSetName = false;
	TupString filePathName;
	TupString name;
	TextureSettings settings(globalSettings);

	for (TupperwareIterator iter = pAgg->FindAll(); !iter.Finished(); iter.Next()) 
	{
		TupperwareAtom *pAtom = iter.GetAsAtom();
		const char *pKey =  pAtom->GetKey();
		if (!pKey) 
			continue;
		const char *pName = pAtom->GetName();
		if (!ValidPlatform(pName))
			continue;

		if (!stricmp(pKey,"FilePathName"))
		{
			if (pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
			{
				TupperwareScalar *pScalar = (TupperwareScalar*) pAtom;
				if (pScalar->GetDataType()!=TupperwareScalar::DATA_STRING)
				{
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"FilePathName Scalar should be a string");
				}
				const char *pFilePathName = pScalar->GetAsString();
				if (!pFilePathName)
				{
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"FilePathName Scalar should be a valid string");
				}
				else
				{
					if (bSetFileName)
					{
						PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"FilePathName already set ignoring %s",pFilePathName);
					}
					else
					{
						bSetFileName = true;
						filePathName = pFilePathName;
					}
				}
			}
			else
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"FilePathName should be a scalar string");
			}
		}
		else if (!stricmp(pKey,"Name"))
		{
			if (pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
			{
				TupperwareScalar *pScalar = (TupperwareScalar*) pAtom;
				if (pScalar->GetDataType()!=TupperwareScalar::DATA_STRING)
				{
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Name Scalar should be a string");
				}
				const char *pName = pScalar->GetAsString();
				if (!pName)
				{
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Name Scalar should be a valid string");
				}
				else
				{
					if (bSetName)
					{
						PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Name already set ignoring %s",pName);
					}
					else
					{
						bSetName = true;
						name = pName;
					}
				}
			}
			else
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Name should be a scalar string");
			}
		}
		else
		{
			UpdateSettings(pAtom,settings);
		}
	}
	// set the name from the filename if not explicitly set
	if (!bSetName && bSetFileName)
	{
		char fname[_MAX_FNAME];
		FilePath::GetFileName(filePathName,fname);
		name = fname;
	}
	LoadSCFiles loadSCFiles;

	TupArray<TupString> textureFileNames;
	loadSCFiles.ScanSCFile(filePathName,textureFileNames);
	int numTextures = textureFileNames.GetSize();
	for (int textureIndex=0;textureIndex<numTextures;textureIndex++)
	{
		TextureEntry newEntry;
		newEntry.m_pTextureAgg = NULL;

		char fname[_MAX_FNAME];
		FilePath::GetFileName(textureFileNames[textureIndex],fname);
		newEntry.m_name.Format("%s.%s",(const char *)name,fname);
		newEntry.m_filePathName = textureFileNames[textureIndex];
		newEntry.m_settings = settings;

		if (bAddTexturesToRootAgg)
		{
			newEntry.m_pTextureAgg = m_pRootAgg->AddAggregate("Texture",NULL);
			TupString comment;
			comment.Format("This texture was provided by %s, entry %d of %d",(const char *)filePathName,textureIndex+1,numTextures);
			newEntry.m_pTextureAgg->AddComment("Comment",NULL,comment);
			newEntry.m_pTextureAgg->AddScalarString("Name",NULL,newEntry.m_name);
			newEntry.m_pTextureAgg->AddScalarString("FilePathName",NULL,newEntry.m_filePathName);
			if (!newEntry.m_settings.m_bBestFitFormat)
			{
				newEntry.m_pTextureAgg->AddScalarString("Format",NULL,GetFormatString(newEntry.m_settings.m_format));
			}
			if (newEntry.m_settings.m_width)
			{
				newEntry.m_pTextureAgg->AddScalarInt("Width",NULL,newEntry.m_settings.m_width);
			}
			if (newEntry.m_settings.m_height)
			{
				newEntry.m_pTextureAgg->AddScalarInt("Height",NULL,newEntry.m_settings.m_height);
			}
		}
		entries.Add(newEntry);
	}
	return bSetFileName; // return true if the name was set
}


bool ProcessTup::ValidPlatform(const char *pPlatform)
{
	// if NULL then it is a go
	if (!pPlatform)
	{
		return true;
	}
	else
	{
		TupArray<TupString> platformStrings;
		TokenizeString::BuildArray(pPlatform,"|",platformStrings);
		int numPlatforms = platformStrings.GetSize();
		for (int platformIndex=0;platformIndex<numPlatforms;platformIndex++)
		{
			if (!stricmp(m_platformString,platformStrings[platformIndex]))
			{
				return true;
			}
			else if (!stricmp(platformStrings[platformIndex],"All"))
			{
				return true;
			}
		}
	}
	return false;
}

const char *ProcessTup::GetFormatString(int formatFlags)
{
	const char *pFormatString = "Undefined";
	switch (formatFlags & TupImageDataFrame::FORMAT_MASK)
	{
		case TupImageDataFrame::RGBA_8888:
			pFormatString = "RGBA8888";
			break;
		case TupImageDataFrame::RGB_888:
			pFormatString = "RGB888";
			break;
		case TupImageDataFrame::RGB_565:
			pFormatString = "RGB565";
			break;
		case TupImageDataFrame::RGB_555:
			pFormatString = "RGB555";
			break;
		case TupImageDataFrame::RGBA_1555:
			pFormatString = "RGBA1555";
			break;
		case TupImageDataFrame::INDEXED_8:
			pFormatString = "8Bit";
			break;
		case TupImageDataFrame::INDEXED_4:
			pFormatString = "4Bit";
			break;
	}
	return pFormatString;
}



// add width,height, etc to the image data (so everyone does not need to scan so much)
// also prints out a sorted list of images in the log
void ProcessTup::AddTextureData(void)
{
	ImageManager &imageManager = m_dataManager.GetImageManager();

	PluginLog::SetProgressText("Loading Textures...");

	int numTextures = m_textureEntries.GetSize();

	for (int textureIndex=0;textureIndex<numTextures;textureIndex++)
	{
		PluginLog::SetProgressBar(0,numTextures-1,textureIndex);

		TextureEntry &textureEntry = m_textureEntries[textureIndex];

		// get image data from image manager
		int imageManagerIndex = imageManager.GetImageIndex(textureEntry.m_filePathName);

		TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(imageManagerIndex);
		assert(pImageDataAgg);
	}
}

void ProcessTup::ChangeTextureFormats(void)
{
	ImageManager &imageManager = m_dataManager.GetImageManager();
	int numTextures = m_textureEntries.GetSize();
	for (int textureIndex=0;textureIndex<numTextures;textureIndex++)
	{
		TextureEntry &textureEntry = m_textureEntries[textureIndex];
		if (!textureEntry.m_settings.m_bBestFitFormat)
		{
			// get image data from image manager
			int imageManagerIndex = imageManager.GetImageIndex(textureEntry.m_filePathName);

			TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(imageManagerIndex);
			assert(pImageDataAgg);

			TupImageData tupImageData(pImageDataAgg);
			int numFrames = tupImageData.GetNumImageDataFrames();
			assert(numFrames>0);
			TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(0); // first frame only (for now)
			assert(pImageDataFrameAgg);
			TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);
			int formatFlags;
			tupImageDataFrame.GetFormatFlags(formatFlags);
			if ((formatFlags&TupImageDataFrame::FORMAT_MASK)!=textureEntry.m_settings.m_format)
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Converting format of %s from %s to %s",
					(const char *)textureEntry.m_filePathName,GetFormatString(formatFlags),GetFormatString(textureEntry.m_settings.m_format));
				ImageUtilities::ConvertToFormat(tupImageData,textureEntry.m_settings.m_format);
			}
		}
	}
}

void ProcessTup::ChangeTextureSize(void)
{
	ImageManager &imageManager = m_dataManager.GetImageManager();
	int numTextures = m_textureEntries.GetSize();
	for (int textureIndex=0;textureIndex<numTextures;textureIndex++)
	{
		TextureEntry &textureEntry = m_textureEntries[textureIndex];
		if (m_scaleTextureHeight != 100 || m_scaleTextureWidth != 100 ||
			textureEntry.m_settings.m_width!=0 || textureEntry.m_settings.m_height!=0)
		{
			// get image data from image manager
			int imageManagerIndex = imageManager.GetImageIndex(textureEntry.m_filePathName);

			TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(imageManagerIndex);
			assert(pImageDataAgg);

			// get the first frame from the image data
			TupImageData tupImageData(pImageDataAgg);
			int numFrames = tupImageData.GetNumImageDataFrames();
			assert(numFrames>0);
			TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(0); // first frame only (for now)
			assert(pImageDataFrameAgg);
			TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);

			int numMaps = tupImageDataFrame.GetNumImageDataMaps();
			assert(numMaps>0);
			TupperwareAggregate *pImageDataMapAgg = tupImageDataFrame.GetImageDataMap(0);
			TupImageDataMap tupImageDataMap(pImageDataMapAgg);
			int imageWidth;
			tupImageDataMap.GetWidth(imageWidth);
			int imageHeight;
			tupImageDataMap.GetHeight(imageHeight);

			int newImageWidth = imageWidth;
			int newImageHeight = imageHeight;

			bool bSizeImage = false;
			if (textureEntry.m_settings.m_width!=0 && textureEntry.m_settings.m_width!=imageWidth)
			{
				newImageWidth = textureEntry.m_settings.m_width;
				bSizeImage = true;
			}

			if (textureEntry.m_settings.m_height!=0 && textureEntry.m_settings.m_height!=imageHeight)
			{
				newImageHeight = textureEntry.m_settings.m_height;
				bSizeImage = true;
			}

			if (m_scaleTextureWidth != 100)
			{
				newImageWidth = (newImageWidth * m_scaleTextureWidth) / 100;
				if (newImageWidth < 8)
					newImageWidth = 8;
				bSizeImage = true;
			}

			if (m_scaleTextureHeight != 100)
			{
				newImageHeight = (newImageHeight * m_scaleTextureHeight) / 100;
				if (newImageHeight < 8)
					newImageHeight = 8;
				bSizeImage = true;
			}

			if (bSizeImage)
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Scaling image %s from %d,%d to %d,%d",
					(const char *)textureEntry.m_filePathName,
					imageWidth,imageHeight,newImageWidth,newImageHeight);

				ImageUtilities::Scale(tupImageData,newImageWidth,newImageHeight);
			}
		}
	}
}

// method will rotate any images that are taller than wide and mark the image data so the uv will be rotated as well
void ProcessTup::RotateTallImages(void)
{
	ImageManager &imageManager = m_dataManager.GetImageManager();
	int numTextures = m_textureEntries.GetSize();
	for (int textureIndex=0;textureIndex<numTextures;textureIndex++)
	{
		TextureEntry &textureEntry = m_textureEntries[textureIndex];

		// get image data from image manager
		int imageManagerIndex = imageManager.GetImageIndex(textureEntry.m_filePathName);

		TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(imageManagerIndex);
		assert(pImageDataAgg);

		// get the first frame from the image data
		TupImageData tupImageData(pImageDataAgg);
		int numFrames = tupImageData.GetNumImageDataFrames();
		assert(numFrames>0);
		TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(0); // first frame only (for now)
		assert(pImageDataFrameAgg);
		TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);

		int numMaps = tupImageDataFrame.GetNumImageDataMaps();
		assert(numMaps>0);
		TupperwareAggregate *pImageDataMapAgg = tupImageDataFrame.GetImageDataMap(0);
		TupImageDataMap tupImageDataMap(pImageDataMapAgg);
		int imageWidth;
		tupImageDataMap.GetWidth(imageWidth);
		int imageHeight;
		tupImageDataMap.GetHeight(imageHeight);

		if (imageHeight>imageWidth)
		{
			// rotate the image
			ImageUtilities::Reverse(tupImageData);

			TupperwareAggregate *m_pTextureAgg = m_textureEntries[textureIndex].m_pTextureAgg;
			assert(m_pTextureAgg);
			m_pTextureAgg->AddScalarInt("ExportTextures_ImageReversed",NULL,TRUE);
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Reversed image %s due to height = %d > width = %d",
				(const char *)textureEntry.m_filePathName,imageHeight,imageWidth);
		}
	}
}


// add width,height, etc to the image data (so everyone does not need to scan so much)
void ProcessTup::AddTextureInformation(void)
{
	ImageManager &imageManager = m_dataManager.GetImageManager();
	int numTextures = m_textureEntries.GetSize();
	for (int textureIndex=0;textureIndex<numTextures;textureIndex++)
	{
		TextureEntry &textureEntry = m_textureEntries[textureIndex];
		TupperwareAggregate *pTextureAgg = textureEntry.m_pTextureAgg;

		int imageManagerIndex = imageManager.GetImageIndex(textureEntry.m_filePathName);
		TupString actualImagePathName = imageManager.GetImageFilename(imageManagerIndex);

		TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(imageManagerIndex);
		assert(pImageDataAgg);

		int dummyImage = 0;
		TupperwareScalar *pDummyImageScalar = pImageDataAgg->FindScalarByKey("Dummy");
		if (pDummyImageScalar)
		{
			dummyImage = pDummyImageScalar->GetAsInt();
		}

		// get the first frame from the image data
		TupImageData tupImageData(pImageDataAgg);
		int numFrames = tupImageData.GetNumImageDataFrames();
		assert(numFrames>0);
		TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(0); // first frame only (for now)
		assert(pImageDataFrameAgg);
		TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);
		int formatFlags;
		int hasAlpha = FALSE;
		int has8BitAlpha = FALSE;
		tupImageDataFrame.GetFormatFlags(formatFlags);
		if (formatFlags&TupImageDataFrame::HAS_ALPHA)
		{
			hasAlpha = TRUE;
			if (formatFlags&TupImageDataFrame::HAS_8BITALPHA)
				has8BitAlpha = TRUE;
		}
		int numMaps = tupImageDataFrame.GetNumImageDataMaps();
		assert(numMaps>0);
		TupperwareAggregate *pImageDataMapAgg = tupImageDataFrame.GetImageDataMap(0);
		TupImageDataMap tupImageDataMap(pImageDataMapAgg);
		int imageWidth;
		tupImageDataMap.GetWidth(imageWidth);
		int imageHeight;
		tupImageDataMap.GetHeight(imageHeight);
		pTextureAgg->AddScalarInt("ExportTextures_NumFrames",NULL,numFrames);
		pTextureAgg->AddScalarInt("ExportTextures_TextureHasAlpha",NULL,hasAlpha);
		pTextureAgg->AddScalarInt("ExportTextures_TextureHas8BitAlpha",NULL,has8BitAlpha);
		pTextureAgg->AddScalarInt("ExportTextures_TextureWidth",NULL,imageWidth);
		pTextureAgg->AddScalarInt("ExportTextures_TextureHeight",NULL,imageHeight);
		pTextureAgg->AddScalarInt("ExportTextures_TextureFormat",NULL,formatFlags);
		pTextureAgg->AddScalarString("ExportTextures_ActualFilePath",NULL,actualImagePathName);
		pTextureAgg->AddScalarInt("ExportTextures_DummyImage",NULL,dummyImage);
	}
}


void ProcessTup::RemapDuplicateTextures(void)
{
	ImageManager &imageManager = m_dataManager.GetImageManager();

	int numTextureEntries = m_textureEntries.GetSize();

	TupArray<u32> CRCArray;
	CRCArray.SetSize(numTextureEntries);
	int textureIndex;
	// set the CRC values for each
	for (textureIndex=0;textureIndex<numTextureEntries;textureIndex++)
	{
		TextureEntry &textureEntry = m_textureEntries[textureIndex];

		TupperwareAggregate *m_pTextureAgg = textureEntry.m_pTextureAgg;
		// get image data from image manager
		TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(textureEntry.m_filePathName);
		assert(pImageDataAgg);
		// get the first frame from the image data
		TupImageData tupImageData(pImageDataAgg);
		CRCArray[textureIndex] = ImageUtilities::GetCRC(tupImageData);
	}

	TupArray<int> textureRemapArray;
	textureRemapArray.SetSize(numTextureEntries);
	int totalRemapped = 0;
	for (textureIndex=0;textureIndex<numTextureEntries;textureIndex++)
	{
		textureRemapArray[textureIndex] = -1;

		u32 CRCValue = CRCArray[textureIndex];
		for (int testTextureIndex=0;testTextureIndex<textureIndex;testTextureIndex++)
		{
			u32 testCRCValue = CRCArray[testTextureIndex];
			if (CRCValue == testCRCValue)
			{
				// re found a match
				textureRemapArray[textureIndex] = testTextureIndex;

				PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Remapping Duplicate Texture %s to %s",
					m_textureEntries[textureIndex].m_filePathName,
					m_textureEntries[testTextureIndex].m_filePathName);
				totalRemapped++;
				break;
			}
		}
	}
	if (totalRemapped)
	{
		// add remap scalars to images that are remapped
		for (textureIndex=0;textureIndex<numTextureEntries;textureIndex++)
		{
			if (textureRemapArray[textureIndex]!=-1)
			{
				TupperwareAggregate *m_pTextureAgg = m_textureEntries[textureIndex].m_pTextureAgg;
				assert(m_pTextureAgg);
				m_pTextureAgg->AddScalarInt("ExportTextures_RemapImageRef",NULL,textureRemapArray[textureIndex]);
			}
		}
	}
}

class PaletteInfo
{
public:
	TupArray<int> m_usedColors;
	TupperwareAggregate *m_pImageDataFrameAgg;
	TupString m_name;
	bool m_bActive;
	bool m_bChild;
};

void ProcessTup::MergeTexturePalettes(void)
{
	ImageManager &imageManager = m_dataManager.GetImageManager();

	int numPalettes = 0;
	int numMergedPalettes = 0;
	int numTextureEntries = m_textureEntries.GetSize();
	int textureIndex;
	TupArray<PaletteInfo> paletteInfoArray;
	paletteInfoArray.SetSize(numTextureEntries);

	PluginLog::SetProgressText("Merging Texture Palettes");

	for (textureIndex=0;textureIndex<numTextureEntries;textureIndex++)
	{
		PluginLog::SetProgressBar(0,textureIndex-1,textureIndex);

		TextureEntry &textureEntry = m_textureEntries[textureIndex];

		PaletteInfo &paletteInfo = paletteInfoArray[textureIndex];
		paletteInfo.m_bActive = false;

		TupperwareAggregate *m_pTextureAgg = textureEntry.m_pTextureAgg;

		// if remapped to another ignore
		if (m_pTextureAgg->FindScalarByKey("ExportTextures_RemapImageRef")!=NULL)
			continue;

		// get image data from image manager
		TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(textureEntry.m_filePathName);
		assert(pImageDataAgg);
		// get the first frame from the image data
		TupImageData tupImageData(pImageDataAgg);
		int numFrames = tupImageData.GetNumImageDataFrames();
		if (numFrames>1)
		{
			MergeAnimatedImagePalettes(textureEntry.m_filePathName,tupImageData,numPalettes,numMergedPalettes);
		}
		else
		{
			assert (numFrames==1);
			TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(0);
			TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);
			int formatFlags;
			tupImageDataFrame.GetFormatFlags(formatFlags);

			// is it a paletted frame?
			if ((formatFlags & TupImageDataFrame::FORMAT_MASK) == TupImageDataFrame::INDEXED_8 ||
				(formatFlags & TupImageDataFrame::FORMAT_MASK) == TupImageDataFrame::INDEXED_4)
			{
				numPalettes++;
				paletteInfo.m_bActive = true;
				paletteInfo.m_bChild = false;
				paletteInfo.m_pImageDataFrameAgg = pImageDataFrameAgg;
				paletteInfo.m_name = textureEntry.m_filePathName;
				ImageUtilities::GetUsedColors(tupImageDataFrame,paletteInfo.m_usedColors);

				int parentImageIndex;
				int bestParentImageIndex = -1;
				int bestAddedColors;
				int addedColors;
				// look at potential parents
				for (parentImageIndex=0;parentImageIndex<textureIndex;parentImageIndex++)
				{
					PaletteInfo &parentPaletteInfo = paletteInfoArray[parentImageIndex];
					// is this parent a potental canidate for a palette merge
					if (parentPaletteInfo.m_bActive && 
						!parentPaletteInfo.m_bChild &&
						parentPaletteInfo.m_usedColors.GetSize() == paletteInfo.m_usedColors.GetSize())
					{
						TupImageDataFrame tupParentImageDataFrame(parentPaletteInfo.m_pImageDataFrameAgg);
						TupArray<int> remapArray; // not used for now
						// check if these can merge
						if (ImageUtilities::MergePalettes(tupParentImageDataFrame,parentPaletteInfo.m_usedColors,
							tupImageDataFrame,paletteInfo.m_usedColors,
							remapArray,addedColors,true))
						{
							if (bestParentImageIndex==-1 || addedColors<bestAddedColors)
							{
								bestAddedColors = addedColors;
								bestParentImageIndex = parentImageIndex;
							}
						}
					}
				}
				// if a parent was found
				if (bestParentImageIndex!=-1)
				{
					numMergedPalettes++;
					PaletteInfo &parentPaletteInfo = paletteInfoArray[bestParentImageIndex];
					TupImageDataFrame tupParentImageDataFrame(parentPaletteInfo.m_pImageDataFrameAgg);
					TupArray<int> remapArray; 
					int priorUsedColors = ImageUtilities::GetUsedColors(parentPaletteInfo.m_usedColors);
					// merge the palettes, build the remap array, & update the used color array for parent
					ImageUtilities::MergePalettes(tupParentImageDataFrame,parentPaletteInfo.m_usedColors,
						tupImageDataFrame,paletteInfo.m_usedColors,
						remapArray,addedColors,false);
					int afterUsedColors = ImageUtilities::GetUsedColors(parentPaletteInfo.m_usedColors);
					// alter the pixels to fit the parent's palette
					ImageUtilities::RemapPalettedFrame(tupImageDataFrame,remapArray);
					// copy the parents palette to the child
					ImageUtilities::CopyPalette(tupImageDataFrame,tupParentImageDataFrame);
					paletteInfo.m_bChild = true; // I am a child (no one can use me as a parent)
					// I now have a parent palette to another image
					m_pTextureAgg->AddScalarInt("ExportTextures_ParentPaletteReference",NULL,bestParentImageIndex);
					PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Merging Palettes from %s to %s. Added %d colors, was %d now %d",
						paletteInfo.m_name,parentPaletteInfo.m_name,afterUsedColors-priorUsedColors,priorUsedColors,afterUsedColors);
				}
			}
		}
	}
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"%d palettes cut down to %d palettes",numPalettes,numPalettes-numMergedPalettes);
}

void ProcessTup::MergeAnimatedImagePalettes(const char *pImageFileName,TupImageData &tupImageData,int &numPalettes,int &numMergedPalettes)
{
	int numFrames = tupImageData.GetNumImageDataFrames();
	assert(numFrames>1);
	int frameIndex;
	TupArray<PaletteInfo> paletteInfoArray;
	paletteInfoArray.SetSize(numFrames);

	for (frameIndex=0;frameIndex<numFrames;frameIndex++)
	{
		PaletteInfo &paletteInfo = paletteInfoArray[frameIndex];
		TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(frameIndex);
		TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);
		int formatFlags;
		tupImageDataFrame.GetFormatFlags(formatFlags);

		// is it a paletted frame?
		if ((formatFlags & TupImageDataFrame::FORMAT_MASK) == TupImageDataFrame::INDEXED_8 ||
			(formatFlags & TupImageDataFrame::FORMAT_MASK) == TupImageDataFrame::INDEXED_4)
		{
			numPalettes++;
			paletteInfo.m_bActive = true;
			paletteInfo.m_bChild = false;
			paletteInfo.m_pImageDataFrameAgg = pImageDataFrameAgg;

			ImageUtilities::GetUsedColors(tupImageDataFrame,paletteInfo.m_usedColors);

			int parentFrameIndex;
			int bestParentFrameIndex = -1;
			int bestAddedColors;
			int addedColors;
			// look at potential parents
			for (parentFrameIndex=0;parentFrameIndex<frameIndex;parentFrameIndex++)
			{
				PaletteInfo &parentPaletteInfo = paletteInfoArray[parentFrameIndex];
				// is this parent a potental canidate for a palette merge
				if (parentPaletteInfo.m_bActive && 
					!parentPaletteInfo.m_bChild &&
					parentPaletteInfo.m_usedColors.GetSize() == paletteInfo.m_usedColors.GetSize())
				{
					TupImageDataFrame tupParentImageDataFrame(parentPaletteInfo.m_pImageDataFrameAgg);
					TupArray<int> remapArray; // not used for now
					// check if these can merge
					if (ImageUtilities::MergePalettes(tupParentImageDataFrame,parentPaletteInfo.m_usedColors,
						tupImageDataFrame,paletteInfo.m_usedColors,
						remapArray,addedColors,true))
					{
						if (bestParentFrameIndex==-1 || addedColors<bestAddedColors)
						{
							bestAddedColors = addedColors;
							bestParentFrameIndex = parentFrameIndex;
						}
					}
				}
			}
			// if a parent was found
			if (bestParentFrameIndex!=-1)
			{
				numMergedPalettes++;
				PaletteInfo &parentPaletteInfo = paletteInfoArray[bestParentFrameIndex];
				TupImageDataFrame tupParentImageDataFrame(parentPaletteInfo.m_pImageDataFrameAgg);
				TupArray<int> remapArray; 
				int priorUsedColors = ImageUtilities::GetUsedColors(parentPaletteInfo.m_usedColors);
				// merge the palettes, build the remap array, & update the used color array for parent
				ImageUtilities::MergePalettes(tupParentImageDataFrame,parentPaletteInfo.m_usedColors,
					tupImageDataFrame,paletteInfo.m_usedColors,
					remapArray,addedColors,false);
				int afterUsedColors = ImageUtilities::GetUsedColors(parentPaletteInfo.m_usedColors);
				// alter the pixels to fit the parent's palette
				ImageUtilities::RemapPalettedFrame(tupImageDataFrame,remapArray);
				// copy the parents palette to the child
				ImageUtilities::CopyPalette(tupImageDataFrame,tupParentImageDataFrame);
				paletteInfo.m_bChild = true; // I am a child (no one can use me as a parent)
				// I now have a parent palette to another image
				pImageDataFrameAgg->AddScalarInt("ExportTextures_ParentPaletteFrame",NULL,bestParentFrameIndex);
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Merging Palettes From %s - Frame %d to Frame %d. Added %d colors, was %d now %d",
					pImageFileName,frameIndex,bestParentFrameIndex,afterUsedColors-priorUsedColors,priorUsedColors,afterUsedColors);
			}
		}
	}
}

void ProcessTup::AnalyzePaletteColors(void)
{
	int numTextureEntries = m_textureEntries.GetSize();
	for (int textureIndex=0;textureIndex<numTextureEntries;textureIndex++)
	{
		TextureEntry &textureEntry = m_textureEntries[textureIndex];
		TupperwareAggregate *m_pTextureAgg = textureEntry.m_pTextureAgg;

		if (m_pTextureAgg->FindScalarByKey("ExportTextures_RemapImageRef")!=NULL)
			continue;

		// get image data from image manager
		ImageManager &imageManager = m_dataManager.GetImageManager();
		TupperwareAggregate *pImageDataAgg = imageManager.GetImageData(textureEntry.m_filePathName);
		assert(pImageDataAgg);
		// get the first frame from the image data
		TupImageData tupImageData(pImageDataAgg);
		int numFrames = tupImageData.GetNumImageDataFrames();
		if (numFrames>1)
		{
			bool bFoundIndexedImage = false;
			int maxAlphasUsed = 0;
			int frameIndex;
			for (frameIndex=0;frameIndex<numFrames;frameIndex++)
			{
				TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(frameIndex);
				TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);
				int formatFlags;
				tupImageDataFrame.GetFormatFlags(formatFlags);

				// is it a paletted frame?
				if ((formatFlags & TupImageDataFrame::FORMAT_MASK) == TupImageDataFrame::INDEXED_8 ||
					(formatFlags & TupImageDataFrame::FORMAT_MASK) == TupImageDataFrame::INDEXED_4)
				{
					bFoundIndexedImage = true;
					TupperwareScalar *pParentPaletteFrameScalar = m_pTextureAgg->FindScalarByKey("ExportTextures_ParentPaletteFrame");
					// we are not referencing a parent
					if (!pParentPaletteFrameScalar)
					{
						TupperwareAggregate *pPaletteAgg = tupImageDataFrame.GetImageDataPalette();
						TupImageDataPalette tupPalette(pPaletteAgg);
						u32 *pPaletteData;
						int numSourceBytes;
						tupPalette.GetData((void **)&pPaletteData,numSourceBytes);
						int numColors = numSourceBytes>>2;
						Keeper<int> alphasUsed;
						for (int paletteIndex=0;paletteIndex<numColors;paletteIndex++)
						{
							alphasUsed.Add(pPaletteData[paletteIndex]>>24);
						}
						if (alphasUsed.GetSize()>maxAlphasUsed)
							maxAlphasUsed = alphasUsed.GetSize();
					}
				}
			}
			if (bFoundIndexedImage)
			{
				m_pTextureAgg->AddScalarInt("ExportTextures_PaletteAlphasUsed",NULL,maxAlphasUsed);
			}
		}
		else
		{
			assert (numFrames==1);
			TupperwareAggregate *pImageDataFrameAgg = tupImageData.GetImageDataFrame(0);
			TupImageDataFrame tupImageDataFrame(pImageDataFrameAgg);
			int formatFlags;
			tupImageDataFrame.GetFormatFlags(formatFlags);

			// is it a paletted frame?
			if ((formatFlags & TupImageDataFrame::FORMAT_MASK) == TupImageDataFrame::INDEXED_8 ||
				(formatFlags & TupImageDataFrame::FORMAT_MASK) == TupImageDataFrame::INDEXED_4)
			{
				TupperwareScalar *pParentPaletteReferenceScalar = m_pTextureAgg->FindScalarByKey("ExportTextures_ParentPaletteReference");
				// we are not referencing a parent
				if (!pParentPaletteReferenceScalar)
				{
					TupperwareAggregate *pPaletteAgg = tupImageDataFrame.GetImageDataPalette();
					TupImageDataPalette tupPalette(pPaletteAgg);
					u32 *pPaletteData;
					int numSourceBytes;
					tupPalette.GetData((void **)&pPaletteData,numSourceBytes);
					int numColors = numSourceBytes>>2;
					Keeper<int> alphasUsed;
					for (int paletteIndex=0;paletteIndex<numColors;paletteIndex++)
					{
						alphasUsed.Add(pPaletteData[paletteIndex]>>24);
					}
					m_pTextureAgg->AddScalarInt("ExportTextures_PaletteAlphasUsed",NULL,alphasUsed.GetSize());
				}
			}
		}
	}
}

class TextureSortData
{
public:
	int m_width;
	int m_height;
	int m_numFrames;
	int m_size;
	TupString m_originalPath;
	TupString m_actualPath;
	int m_dummy;
	char *m_pFormatString;
	int m_textureIndex;
	int m_useTextureIndex;

	TextureSortData(void) : m_width(0),m_height(0),m_numFrames(0),m_size(0),m_dummy(0),m_pFormatString(NULL),m_textureIndex(-1)
	{
	}

	bool TextureSortData::operator==(const TextureSortData& other) const
	{
		return (m_size==other.m_size &&
			strcmp(m_originalPath,other.m_originalPath)==0);
	}

	bool TextureSortData::operator<=(const TextureSortData& other) const
	{
		if (m_size>other.m_size)
			return true;
		else if (m_size<other.m_size)
			return false;
		// size ==
		// return true if path is less than or equal other
		return (strcmp(m_originalPath,other.m_originalPath)<1);
	}

	bool TextureSortData::operator>=(const TextureSortData& other) const
	{
		if (m_size<other.m_size)
			return true;
		else if (m_size>other.m_size)
			return false;
		// size ==
		// return true if path is greater than or equal other
		return (strcmp(m_originalPath,other.m_originalPath)>-1);
	}

	bool TextureSortData::operator<(const TextureSortData& other) const
	{
		if (m_size>other.m_size)
			return true;
		else if (m_size<other.m_size)
			return false;
		// size ==
		// return true if path is less than other
		return (strcmp(m_originalPath,other.m_originalPath)<0);
	}

	bool TextureSortData::operator>(const TextureSortData& other) const
	{
		if (m_size<other.m_size)
			return true;
		else if (m_size>other.m_size)
			return false;
		// size ==
		// return true if path is greater than other
		return (strcmp(m_originalPath,other.m_originalPath)>0);
	}
};

void ProcessTup::AddSortedTexturesToLog(void)
{
	int numTextures = m_textureEntries.GetSize();

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Texture List:");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"  Size    Width Height TextureID UseTexture Frames Format  Path (*=Remapped X=Dummy Texture)");

	TupArray<TextureSortData> sortedTextures;
	int textureIndex;
	for (textureIndex=0;textureIndex<numTextures;textureIndex++)
	{
		TextureEntry &textureEntry = m_textureEntries[textureIndex];

		TupperwareAggregate *m_pTextureAgg = textureEntry.m_pTextureAgg;

		TextureSortData textureSortData;

		textureSortData.m_textureIndex = textureIndex;
		textureSortData.m_originalPath = textureEntry.m_filePathName;
		textureSortData.m_actualPath = m_pTextureAgg->FindScalarByKey("ExportTextures_ActualFilePath")->GetAsString();
		textureSortData.m_numFrames = m_pTextureAgg->FindScalarByKey("ExportTextures_NumFrames")->GetAsInt();
		textureSortData.m_width = m_pTextureAgg->FindScalarByKey("ExportTextures_TextureWidth")->GetAsInt();
		textureSortData.m_height = m_pTextureAgg->FindScalarByKey("ExportTextures_TextureHeight")->GetAsInt();
		textureSortData.m_dummy = m_pTextureAgg->FindScalarByKey("ExportTextures_DummyImage")->GetAsInt();
		textureSortData.m_useTextureIndex = textureIndex;
		TupperwareScalar *pRemapImageReferenceScalar = m_pTextureAgg->FindScalarByKey("ExportTextures_RemapImageRef");
		if (pRemapImageReferenceScalar)
		{
			textureSortData.m_useTextureIndex = pRemapImageReferenceScalar->GetAsInt();
		}

		int formatFlags = m_pTextureAgg->FindScalarByKey("ExportTextures_TextureFormat")->GetAsInt();

		int numPixels = textureSortData.m_width*textureSortData.m_height*textureSortData.m_numFrames;
		switch (formatFlags & TupImageDataFrame::FORMAT_MASK)
		{
		case TupImageDataFrame::RGBA_8888:
			textureSortData.m_size = numPixels*4;
			textureSortData.m_pFormatString = "RGB8888";
			break;
		case TupImageDataFrame::RGB_888:
			textureSortData.m_size = numPixels*3;
			textureSortData.m_pFormatString = "RGB888 ";
			break;
		case TupImageDataFrame::RGB_565:
			textureSortData.m_pFormatString = "RGB565 ";
			textureSortData.m_size = numPixels*2;
			break;
		case TupImageDataFrame::RGB_555:
			textureSortData.m_pFormatString = "RGB555 ";
			textureSortData.m_size = numPixels*2;
			break;
		case TupImageDataFrame::RGBA_1555:
			textureSortData.m_pFormatString = "RGB1555";
			textureSortData.m_size = numPixels*2;
			break;
		case TupImageDataFrame::INDEXED_8:
			textureSortData.m_pFormatString = "8 Bit  ";
			textureSortData.m_size = numPixels;
			break;
		case TupImageDataFrame::INDEXED_4:
			textureSortData.m_pFormatString = "4 Bit  ";
			textureSortData.m_size = (numPixels)>>1;
			break;
		}
		m_pTextureAgg->AddScalarInt("ExportTextures_TextureSize",NULL,textureSortData.m_size);

		char dummyChar = textureSortData.m_dummy ? 'X' : ' ';
		char pathPrefix = (stricmp(textureSortData.m_originalPath,textureSortData.m_actualPath)==0) ? ' ' : '*';

		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"%8d  %4d  %4d      %3d      %3d       %2d    %s  %c%c%s",
			textureSortData.m_size,textureSortData.m_width,textureSortData.m_height,textureSortData.m_textureIndex,textureSortData.m_useTextureIndex,textureSortData.m_numFrames,
			textureSortData.m_pFormatString,dummyChar,pathPrefix,(const char *)textureSortData.m_actualPath);

		sortedTextures.Add(textureSortData);
	}
	int numSortedImages = sortedTextures.GetSize();
	TupSort<TextureSortData>::SortAscending(sortedTextures.GetData(),numSortedImages);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Sorted Texture List:");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"  Size    Width Height TextureID UseTexture Frames Format  Path (*=Remapped X=Dummy Texture)");
	int totalSize=0;
	for (textureIndex=0;textureIndex<numSortedImages;textureIndex++)
	{
		TextureSortData &textureSortData = sortedTextures[textureIndex];
		char dummyChar = textureSortData.m_dummy ? 'X' : ' ';
		char pathPrefix = (stricmp(textureSortData.m_originalPath,textureSortData.m_actualPath)==0) ? ' ' : '*';

		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"%8d  %4d  %4d      %3d      %3d       %2d    %s  %c%c%s",
			textureSortData.m_size,textureSortData.m_width,textureSortData.m_height,textureSortData.m_textureIndex,textureSortData.m_useTextureIndex,textureSortData.m_numFrames,
			textureSortData.m_pFormatString,dummyChar,pathPrefix,(const char *)textureSortData.m_actualPath);

		totalSize += textureSortData.m_size;
	}
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Total Textures: %d",numSortedImages);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Total texture size: %d bytes.",totalSize);
}

void ProcessTup::WriteXBoxRDFFile(void)
{
	TupString RDFFileName = m_dataManager.GetOutputFilename();;
	FilePath::ChangeExtension(RDFFileName,".RDF");

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Exporting file: %s...",(const char *) RDFFileName);

	FILE* fp;
	fp = fopen( RDFFileName, "wb" );
	if ( !fp )
		return;

	int numTextures = m_textureEntries.GetSize();

	int textureIndex;
	for (textureIndex = 0; textureIndex < numTextures; textureIndex++)
	{
		TextureEntry &textureEntry = m_textureEntries[textureIndex];
		TupperwareAggregate *pTextureAgg = textureEntry.m_pTextureAgg;

		TupString formatName;
		int formatFlags = pTextureAgg->FindScalarByKey("ExportTextures_TextureFormat")->GetAsInt();
		int imageHasAlpha = pTextureAgg->FindScalarByKey("ExportTextures_TextureHasAlpha")->GetAsInt();
		int imageHas8BitAlpha = pTextureAgg->FindScalarByKey("ExportTextures_TextureHas8BitAlpha")->GetAsInt();

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
		TupString fname(textureEntry.m_name);
		fname.Replace("-", "_");
		fname.Replace(" ", "_");
		fname.Replace("[", "_");
		fname.Replace("]", "");

		fprintf( fp, "Texture X_%s\r\n", (const char *)fname);
		fprintf( fp, "{\r\n");
		fprintf( fp, "\tSource \"%s\"\r\n", (const char *)textureEntry.m_filePathName);
		fprintf( fp, "\tFormat \"%s\"\r\n", (const char *)formatName);
		fprintf( fp, "\tWidth %d\r\n",pTextureAgg->FindScalarByKey("ExportTextures_TextureWidth")->GetAsInt());
		fprintf( fp, "\tHeight %d\r\n",pTextureAgg->FindScalarByKey("ExportTextures_TextureHeight")->GetAsInt());
		fprintf( fp, "}\r\n");
	}

	// our make process needs a file or it quits out..
	// so try to make a dummy file here
	if (numTextures == 0)
	{
		fprintf ( fp, "\n");
	}


	fclose( fp );
}
