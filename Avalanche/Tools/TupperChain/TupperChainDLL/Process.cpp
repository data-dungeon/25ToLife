////////////////////////////////////////////////////////////////////////////
//
// Process
//
// Function to handle processing the tup data called from an external application
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////


#include "TupperChainDLLPch.h"
#include "process.h"
#include "simplelog.h"

extern HINSTANCE hInstance;

class AppOptions
{
public:
	TupString m_inputFile;
	TupString m_configFile;
	TupString m_outputFile;
	OptionOverrides m_optionOverrides;
	bool m_bDependencyFileOnly;
};

void SaveFileDependencies(SimpleLog &log, FilePathArray &filePathArray,const char *pDependencyFileName);
void CheckFileDependencies(SimpleLog &log, FilePathArray &filePathArray);
bool CheckCommandLine(int argc, char ** argv,AppOptions &appOptions);
void PrintCommandLineInfo(SimpleLog &log,AppOptions &appOptions);

bool Process( TupperwareAggregate *pInitialData, int argc, char** argv )
{
//	printf("FilterChainDLL\n");
	AppOptions appOptions;
	if (!CheckCommandLine(argc,argv,appOptions))
	{
		return false;
	}

	int result;
	TupString relativePath,fullPath,name,ext;

	char pathBufferTemp[_MAX_PATH];
	char pathBuffer[_MAX_PATH];

	// set work path
	_getcwd( pathBuffer, _MAX_PATH );
	if (pathBuffer[strlen(pathBuffer)-1]!='\\')
		strcat(pathBuffer,"\\");
	TupString workPath(pathBuffer);

	// set executable path
	GetModuleFileName(hInstance,pathBufferTemp,_MAX_PATH);
	FilePath::GetPath(pathBufferTemp ,pathBuffer);
	if (pathBuffer[strlen(pathBuffer)-1]!='\\')
		strcat(pathBuffer,"\\");
	TupString programPath(pathBuffer);

	// set input file path
	FilePath::GetPathsToFile(relativePath,fullPath,name,ext,appOptions.m_inputFile,workPath);
	TupString inputFile(fullPath);

	// set output file path
	FilePath::GetPathsToFile(relativePath,fullPath,name,ext,appOptions.m_outputFile,workPath);
	TupString outputFile(fullPath);

	// set config file path
	FilePath::GetPathsToFile(relativePath,fullPath,name,ext,appOptions.m_configFile,programPath);
	TupString configFileName(fullPath);

	// set config file directory
	FilePath::GetPath(fullPath,pathBuffer);
	if (pathBuffer[strlen(pathBuffer)-1]!='\\')
		strcat(pathBuffer,"\\");
	TupString configPath(pathBuffer);

//	strcpy(configFileBuffer,fullPath);

	TupString logFileName(outputFile);
	FilePath::ChangeExtension(logFileName,".LOG");

	SimpleLog log(logFileName);
	log.SetVerboseLevel(LogFlags::VERBOSE_LEVEL_HIGH);

	PrintCommandLineInfo(log,appOptions);

   log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Program Path set to %s",(const char *)programPath);


	int nStartTickCount = GetTickCount();

	FilterPluginManager filterPluginManager(log,programPath);
	ImageManager imageManager(log,inputFile,programPath);
	TupperwareAggregate *pConfigAgg=NULL;

	try
	{
		TupString configFilename;

		log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Loading %s Configuration file",(const char *)configFileName);
		result = TupperwareLoadSaveManager::Read(configFileName,&pConfigAgg);
		if (!result)
		{
			throw(PluginException(ERROR_FATAL,"Loading %s Configuration File",(const char *)configFileName));
		}
		ConfigManager configManager(pConfigAgg,appOptions.m_optionOverrides);
		// let the image manager have access to the image reader options
		imageManager.SetImageReaderOptionsManager(configManager.GetImagerOptionsManager());

		// load all the filters
		int totalPasses = configManager.GetNumPasses();
		for (int i=0;i<totalPasses;i++)
		{
			const char *filterName = configManager.GetFilterName(i);
			if (!filterPluginManager.FilterPluginLoaded(filterName))
			{
				log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Loading %s Filter",filterName);
				FilterPlugin *pFilter = filterPluginManager.GetFilterPlugin(filterName);
				if (!pFilter)
				{
					throw(PluginException(ERROR_FATAL,"Loading %s Filter",filterName));
				}
			}
		}

		log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Loading Source File %s",inputFile);
		FilterDataManager filterDataManager(pInitialData,inputFile,outputFile,workPath,programPath,configPath,imageManager);
		if (!filterDataManager.GetSceneRoot())
		{
			throw(PluginException(ERROR_FATAL,"Loading %s file",inputFile));
		}

		totalPasses = configManager.GetNumPasses();
		FilePathArray filePathArray;
		// ask each filter to provide the file dependencies that are needed
		log.Add(LogFlags::VERBOSE_LEVEL_LOW|LogFlags::FONT_STYLE_BOLD,"Getting File Dependencies.");
		for (i=0;i<totalPasses;i++)
		{
			const char *filterName = configManager.GetFilterName(i);
			int pass = configManager.GetFilterPass(i);
			FilterPlugin *pFilter = filterPluginManager.GetFilterPlugin(filterName);
			if (!pFilter)
			{
				throw(PluginException(ERROR_FATAL,"FilterChain - Loading %s Filter.",filterName));
			}
			pFilter->GetFileDependencies(configManager.GetFilterPass(i),configManager.GetFilterOptions(i),filterDataManager,filePathArray);
		}

		// if only doing dependencies
		if (appOptions.m_bDependencyFileOnly)
		{
			log.Add(LogFlags::VERBOSE_LEVEL_LOW|LogFlags::FONT_STYLE_BOLD,"Saving File Dependencies.");
			SaveFileDependencies(log,filePathArray,outputFile);
		}
		else
		{
			log.Add(LogFlags::VERBOSE_LEVEL_LOW|LogFlags::FONT_STYLE_BOLD,"Checking File Dependencies.");
			CheckFileDependencies(log,filePathArray);
			
			// process the filters
			for (i=0;i<totalPasses;i++)
			{
				const char *filterName = configManager.GetFilterName(i);
				int pass = configManager.GetFilterPass(i);
				log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Processing Filter %s",filterName);
				FilterPlugin *pFilter = filterPluginManager.GetFilterPlugin(filterName);
				if (!pFilter)
				{
					throw(PluginException(ERROR_FATAL,"Loading %s Plugin",filterName));
				}
				pFilter->Process(configManager.GetFilterPass(i),configManager.GetFilterOptions(i),filterDataManager);
			}
			// save out the tupperware data if no export happened
			if (0)
			{
				TupperwareAggregate *root = filterDataManager.GetSceneRoot();
				log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Saving File %s",outputFile);
				result = TupperwareLoadSaveManager::Write(TupperwareLoadSaveManager::ASCII,argv[3],root);
				if (!result)
				{
					throw(PluginException(ERROR_FATAL,"Writing %s File",outputFile));
				}
			}
		}
	}
	catch (PluginException e)
	{
		delete pConfigAgg;
		pConfigAgg = NULL;
		log.Add(LogFlags::VERBOSE_LEVEL_NONE,"ERROR - %s",(const char *)e);
		log.Add(LogFlags::VERBOSE_LEVEL_NONE,"Processing Aborted.");
//		printf("ERROR - %s\n",(const char *)e);
//		printf("Processing Aborted\n");
		return false;
	}
	catch (...)
	{
//		printf("Unhandled Error - Processing Aborted\n");
		return false;
	}

	log.Add(LogFlags::VERBOSE_LEVEL_LOW|LogFlags::FONT_STYLE_BOLD,"Processing Completed");
	log.SetProgressText("Done");

	int nTicks = GetTickCount() - nStartTickCount;
	log.Add(LogFlags::VERBOSE_LEVEL_LOW|LogFlags::FONT_STYLE_BOLD,"Total Time: %02d Min %02d Sec", (nTicks / 1000) / 60, (nTicks / 1000) % 60);

	return true;
}

void SaveFileDependencies(SimpleLog &log, FilePathArray &filePathArray,const char *pDependencyFileName)
{
	FILE *pFile = fopen(pDependencyFileName,"wb");
	if (!pFile)
	{
		throw(PluginException(ERROR_FATAL,"Error opening dependency file %s",pDependencyFileName));
	}

	int numPaths = filePathArray.GetNumPaths();
	for (int pathIndex=0;pathIndex<numPaths;pathIndex++)
	{
		const char *pPathFilename = filePathArray.GetFilePathName(pathIndex);
		int pathFlags = filePathArray.GetFilePathFlags(pathIndex);
		// input only
		if ((pathFlags & FilePathArray::FILE_INPUT) && ((pathFlags & FilePathArray::FILE_OUTPUT)==0))
		{
			if (_access(pPathFilename,0)!=-1)
			{
				fprintf(pFile,"\"%s\"\r\n",pPathFilename); // only do valid ones
			}
		}
	}
	fclose(pFile);
}

// make sure files exist for reading or writing and throw if there is a problem
void CheckFileDependencies(SimpleLog &log, FilePathArray &filePathArray)
{
	int numFatals = 0;
	int numWarnings = 0;
	int numPaths = filePathArray.GetNumPaths();
	for (int pathIndex=0;pathIndex<numPaths;pathIndex++)
	{
		const char *pPathFilename = filePathArray.GetFilePathName(pathIndex);
		int pathFlags = filePathArray.GetFilePathFlags(pathIndex);
		// input only
		if ((pathFlags & FilePathArray::FILE_INPUT) && ((pathFlags & FilePathArray::FILE_OUTPUT)==0))
		{
			if (_access(pPathFilename,0)==-1)
			{
				if (pathFlags & FilePathArray::FILE_ACCESS_FATAL)
				{
					log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD|LogFlags::FONT_COLOR_DK_RED,"Cannot find %s", pPathFilename);
					numFatals++;
					continue;
				}
				if (pathFlags & FilePathArray::FILE_ACCESS_WARNING)
				{
					log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD|LogFlags::FONT_COLOR_DK_ORANGE,"Cannot find %s", pPathFilename);
					numWarnings++;
					continue;
				}
			}
			if (_access(pPathFilename,4)==-1)
			{
				if (pathFlags & FilePathArray::FILE_ACCESS_FATAL)
				{
					log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD|LogFlags::FONT_COLOR_DK_RED,"Cannot read %s", pPathFilename);
					numFatals++;
					continue;
				}
				if (pathFlags & FilePathArray::FILE_ACCESS_WARNING)
				{
					log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD|LogFlags::FONT_COLOR_DK_ORANGE,"Cannot read %s", pPathFilename);
					numWarnings++;
					continue;
				}
			}
		}
		// output (or input and output)
		else if (pathFlags & FilePathArray::FILE_OUTPUT)
		{
			// if the file exists
			if (_access(pPathFilename,0)!=-1)
			{
				// check if we can write to it
				if (_access(pPathFilename,2)==-1)
				{
					if (pathFlags & FilePathArray::FILE_ACCESS_FATAL)
					{
						log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD|LogFlags::FONT_COLOR_DK_RED,"Cannot write %s", pPathFilename);
						numFatals++;
						continue;
					}
					if (pathFlags & FilePathArray::FILE_ACCESS_WARNING)
					{
						log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD|LogFlags::FONT_COLOR_DK_ORANGE,"Cannot write %s", pPathFilename);
						numWarnings++;
						continue;
					}
				}
			}
		}
	}
	if (numFatals)
	{
		throw(PluginException(ERROR_FATAL,"File Dependency Check Found %d Errors and %d Warnings",numFatals,numWarnings));
	}
	if (numWarnings)
	{
		log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD|LogFlags::FONT_COLOR_DK_ORANGE,"File Dependency Check Found %d Warnings",numWarnings);
	}
}

bool CheckCommandLine(int argc, char ** argv,AppOptions &appOptions)
{
	if (argc<3)
	{
		return false;
	}
	appOptions.m_inputFile = argv[0];
	appOptions.m_configFile = argv[1];
	appOptions.m_outputFile = argv[2];
	int optionIndex = 3;
	appOptions.m_bDependencyFileOnly = false;
	while (optionIndex<argc)
	{
		// option override
		if (!strcmp(argv[optionIndex],"/O"))
		{
			optionIndex++;
			// make sure we have a string after the option
			if (optionIndex<argc)
			{
				TupString optionString(argv[optionIndex]);
				TupArray<TupString> overrideOption;
				TokenizeString::BuildArray(optionString,"!",overrideOption);
				// if global case
				if (overrideOption.GetSize()==1)
				{
					TokenizeString::BuildArray(optionString,"=",overrideOption);
					// if global clear key
					if (overrideOption.GetSize()==1)
					{
						FilterOption newOption;
						TupString newKey(overrideOption[0]);
						newKey.TrimLeft();
						newKey.TrimRight();
						newOption.SetKey(newKey);
						newOption.SetValue("");
						appOptions.m_optionOverrides.AddGlobalOverrideOption(newOption);
					}
					// global set key
					else if (overrideOption.GetSize()==2)
					{
						FilterOption newOption;
						TupString newKey(overrideOption[0]);
						newKey.TrimLeft();
						newKey.TrimRight();
						newOption.SetKey(newKey);
						TupString newValue(overrideOption[1]);
						newKey.TrimLeft();
						newKey.TrimRight();
						newOption.SetValue(newValue);
						appOptions.m_optionOverrides.AddGlobalOverrideOption(newOption);
					}
				}
				// plugin case
				else if (overrideOption.GetSize()==2)
				{
					TupString pluginName(overrideOption[0]);
					TupString optionSubString(overrideOption[1]);
					TokenizeString::BuildArray(optionSubString,"=",overrideOption);
					// if clear key
					if (overrideOption.GetSize()==1)
					{
						FilterOption newOption;
						TupString newKey(overrideOption[0]);
						newKey.TrimLeft();
						newKey.TrimRight();
						newOption.SetKey(newKey);
						newOption.SetValue("");
						appOptions.m_optionOverrides.AddPluginOverrideOption(pluginName,newOption);
					}
					// set key
					else if (overrideOption.GetSize()==2)
					{
						FilterOption newOption;
						TupString newKey(overrideOption[0]);
						newKey.TrimLeft();
						newKey.TrimRight();
						newOption.SetKey(newKey);
						TupString newValue(overrideOption[1]);
						newKey.TrimLeft();
						newKey.TrimRight();
						newOption.SetValue(newValue);
						appOptions.m_optionOverrides.AddPluginOverrideOption(pluginName,newOption);
					}
				}
				optionIndex++;
			}
		}
		else if (!strcmp(argv[optionIndex],"/D"))
		{
			optionIndex++;
			appOptions.m_bDependencyFileOnly = true;
		}
	}
	return true;
}

void PrintCommandLineInfo(SimpleLog &log,AppOptions &appOptions)
{
	log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Input File Set to %s",(const char *)appOptions.m_inputFile);
	log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Output File Set to %s",(const char *)appOptions.m_outputFile);
	log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Configuration File Set to %s",(const char *)appOptions.m_configFile);
	if (appOptions.m_bDependencyFileOnly)
		log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Outputing Dependency File Only");

	const FilterOptions &globalOverrideOptions = appOptions.m_optionOverrides.GetGlobalOverrideOptions();
	int numGlobalOverrideOptions = globalOverrideOptions.GetNumOptions();
	int optionIndex;
	for (optionIndex=0;optionIndex<numGlobalOverrideOptions;optionIndex++)
	{
		const FilterOption &option = globalOverrideOptions.GetOption(optionIndex);
		if (strlen(option.GetValue()))
		{
			log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Setting Global Option \"%s\" = \"%s\"",(const char *)option.GetKey(),(const char *)option.GetValue());
		}
		else
		{
			log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Clearing Global Option \"%s\"",(const char *)option.GetKey());
		}
	}
	const TupArray<PluginOverrideOptions> &pluginOverrideOptions = appOptions.m_optionOverrides.GetPluginOverrideOptions();
	int numPlugins = pluginOverrideOptions.GetSize();
	int pluginIndex;
	for (pluginIndex=0;pluginIndex<numPlugins;pluginIndex++)
	{
		const PluginOverrideOptions &pluginOptions = pluginOverrideOptions[pluginIndex];
		const FilterOptions &options = pluginOptions.GetOptions();
		int numOptions = options.GetNumOptions();
		for (optionIndex=0;optionIndex<numOptions;optionIndex++)
		{
			const FilterOption &option = options.GetOption(optionIndex);
			if (strlen(option.GetValue()))
			{
				log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Setting Plugin Option for %s \"%s\" = \"%s\"",(const char *)pluginOptions.GetPluginName(),(const char *)option.GetKey(),(const char *)option.GetValue());
			}
			else
			{
				log.Add(LogFlags::VERBOSE_LEVEL_LOW,"Clearing Plugin Option for %s \"%s\"",(const char *)pluginOptions.GetPluginName(),(const char *)option.GetKey());
			}
		}
	}
}
