////////////////////////////////////////////////////////////////////////////
//
// Process
//
// Class handle the process button
//
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TupperChainGuiApp.h"
#include "TupperChainGuiAppDlg.h"
#include "registry.h"
#include <direct.h>
#include "TupperFilters/Libraries/Plugins/FilePath.h"
#include "log.h"
#include <assert.h>
#include "tupperware/tupstring.h"
#include "tupperware/memorymanager.h"

// TupperAttrib
#include "tupperattrib/AccessRoot.h"
#include "tupperattrib/AccessInstance.h"
#include "tupperattrib/AccessPath.h"
#include "tupperattrib/ConfigRoot.h"
#include "tupperattrib/KeyValue.h"
#include "tupperattrib/KeyValueContext.h"

//static void TestAttributeData(const char *filename);

void CTupperChainGuiAppDlg::OnProcessButton() 
{
#if 1
	if (m_pProcessThread)
		return;
	m_threadRunning = true;
	m_pProcessThread = AfxBeginThread(ProcessProc,this);
#else
	ProcessProc(this);
	EnableControls(TRUE);
	m_stopProcessingButton.EnableWindow(FALSE);
#endif
}

LRESULT CTupperChainGuiAppDlg::OnKickIdle(WPARAM wParam,LPARAM lParam)
{
	if (m_pProcessThread && !m_threadRunning)
	{
		// thread is done
		m_pProcessThread = NULL;
		EnableControls(TRUE);
		m_stopProcessingButton.EnableWindow(FALSE);
	}
	return FALSE;
}


UINT CTupperChainGuiAppDlg::ProcessProc(LPVOID pParam)
{
	CTupperChainGuiAppDlg *pThis = (CTupperChainGuiAppDlg *) pParam;
	return pThis->ProcessThread();
}

UINT CTupperChainGuiAppDlg::ProcessThread(void)
{
	m_stopButtonPressed = false;
	EnableControls(FALSE);
	m_stopProcessingButton.EnableWindow(TRUE);

	WriteSettings();

	CString sourceFileString;
	CString outputFileString;
	CString configurationFileString;

	m_sourceFileCombo.GetWindowText(sourceFileString);
	m_outputFileCombo.GetWindowText(outputFileString);
	m_configurationFileCombo.GetWindowText(configurationFileString);

	TupString logFilename(outputFileString);
	FilePath::ChangeExtension(logFilename,".LOG");

	TupperChainLog log(*this,logFilename);
	PluginLog::SetLog(&log);

	int verboseLevel = m_verboseLevelCombo.GetCurSel();
	log.SetVerboseLevel(verboseLevel);

	log.Clear();
	log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD,"Processing...");
	log.SetProgressText("Processing...");
	int result;
	TupperwareAggregate *pConfigAgg = NULL;

	int nStartTickCount = GetTickCount();

	// moved these out of the try block
	FilterPluginManager filterPluginManager(log,m_pluginPath);
	ImageManager imageManager(log,LPCTSTR(sourceFileString),m_pluginPath);

	try
	{
		if (!sourceFileString.GetLength())
		{
			throw(PluginException(ERROR_FATAL,"Source File Not Specified"));
		}
		if (!outputFileString.GetLength())
		{
			throw(PluginException(ERROR_FATAL,"Output File Not Specified"));
		}
		if (!configurationFileString.GetLength())
		{
			throw(PluginException(ERROR_FATAL,"Configuration File Not Specified"));
		}

		char pathBuffer[_MAX_PATH];
		// set config file directory
		FilePath::GetPath(configurationFileString,pathBuffer);
		if (pathBuffer[strlen(pathBuffer)-1]!='\\')
			strcat(pathBuffer,"\\");
		m_configPath = pathBuffer;

		log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD,"Loading %s Configuration File.",LPCTSTR(configurationFileString));
		result = TupperwareLoadSaveManager::Read(LPCTSTR(configurationFileString),&pConfigAgg);
		if (!result)
		{
			throw(PluginException(ERROR_FATAL,"FilterChain - Reading %s Configuration File.",LPCTSTR(configurationFileString)));
		}

		OptionOverrides optionOverrides;
		// we don't set any of these in the GUI app at this time
		ConfigManager configManager(pConfigAgg,optionOverrides);

		// let the image manager have access to the image reader options
		imageManager.SetImageReaderOptionsManager(configManager.GetImagerOptionsManager());

		log.SetProgressText("Loading Filters...");
		// load all the filters
		int totalPasses = configManager.GetNumPasses();
		for (int i=0;i<totalPasses;i++)
		{
			const char *filterName = configManager.GetFilterName(i);
			if (!filterPluginManager.FilterPluginLoaded(filterName))
			{
				log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD,"Loading %s Filter.",filterName);
				FilterPlugin *pFilter = filterPluginManager.GetFilterPlugin(filterName);
				if (!pFilter)
				{
					throw(PluginException(ERROR_FATAL,"FilterChain - Loading %s Filter.",filterName));
				}
			}
		}
		log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD,"Checking Dependencies.");
		ChainManager chainManager(filterPluginManager,configManager,log);
		chainManager.CheckDependencies();

      // attempt to load a different source file in the same directory with a specific "postfix"
      // handle sourceFilePostfix
      const char *pSourceFilePostfix = configManager.GetGlobalOptions().GetValue("SourceFilePostfix");
      if (pSourceFilePostfix)
      {
         TupString sourceFilePostfix(pSourceFilePostfix);
         TupArray<TupString> sourceFilePostfixStrings;
         TokenizeString::BuildArray(sourceFilePostfix,";",sourceFilePostfixStrings);
         int numSubStrings = sourceFilePostfixStrings.GetSize();
         for (int subStringIndex=0;subStringIndex<numSubStrings;subStringIndex++)
         {
            TupString replacePath = sourceFileString;
            FilePath::AddStringToFileName(replacePath,sourceFilePostfixStrings[subStringIndex]);
            if (_access(replacePath,0)==0)
            {
               log.Add(LogFlags::VERBOSE_LEVEL_NONE,"Using source file postfix %s to load alternate source file.",
                  LPCTSTR(sourceFilePostfixStrings[subStringIndex]));
               sourceFileString = replacePath;
               break;
            }
         }
      }

		log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD,"Loading %s Source File.",LPCTSTR(sourceFileString));
		log.SetProgressText("Loading %s",LPCTSTR(sourceFileString));
		FilterDataManager filterDataManager(LPCTSTR(sourceFileString),LPCTSTR(outputFileString),LPCTSTR(m_workPath),LPCTSTR(m_pluginPath),LPCTSTR(m_configPath),imageManager);
		if (!filterDataManager.GetSceneRoot())
		{
			throw(PluginException(ERROR_FATAL,"FilterChain - Loading %s Source file.",LPCTSTR(sourceFileString)));
		}
		totalPasses = configManager.GetNumPasses();
		FilePathArray filePathArray;
		// ask each filter to provide the file dependencies that are needed
		log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD,"Getting File Dependencies.");
		for (i=0;i<totalPasses;i++)
		{
			if (log.GetAbortFlag())
			{
				throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));
			}
			const char *filterName = configManager.GetFilterName(i);
			int pass = configManager.GetFilterPass(i);
			FilterPlugin *pFilter = filterPluginManager.GetFilterPlugin(filterName);
			if (!pFilter)
			{
				throw(PluginException(ERROR_FATAL,"FilterChain - Loading %s Filter.",filterName));
			}
			pFilter->GetFileDependencies(pass,configManager.GetFilterOptions(i),filterDataManager,filePathArray);
		}
		if (m_outputTupperwareFile.GetCheck())
		{
			TupString tempStr = LPCTSTR(outputFileString);
			FilePath::ChangeExtension(tempStr,".tup");
			filePathArray.AddFilePath(tempStr,FilePathArray::FILE_OUTPUT|FilePathArray::FILE_ACCESS_FATAL);
		}
		log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD,"Checking File Dependencies.");

		CheckFileDependencies(log,filePathArray);

		// process the filters
		for (i=0;i<totalPasses;i++)
		{
			if (log.GetAbortFlag())
			{
				throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));
			}
			const char *filterName = configManager.GetFilterName(i);
			int pass = configManager.GetFilterPass(i);
			FilterPlugin *pFilter = filterPluginManager.GetFilterPlugin(filterName);
			if (!pFilter)
			{
				throw(PluginException(ERROR_FATAL,"FilterChain - Loading %s Filter.",filterName));
			}
			log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD,"Processing Filter %s - Pass %d",filterName,pass);
			log.SetProgressText("Processing Filter %s",filterName);
			pFilter->Process(pass,configManager.GetFilterOptions(i),filterDataManager);
		}
		// save out the tupperware data if no export happened
		if (m_outputTupperwareFile.GetCheck())
		{
			log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD|LogFlags::FONT_COLOR_YELLOW,"Post Process Tupperware File Output Requested");
			TupperwareAggregate *root = filterDataManager.GetSceneRoot();
			TupString tempStr = LPCTSTR(outputFileString);
			FilePath::ChangeExtension(tempStr,".tup");
			log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD|LogFlags::FONT_COLOR_YELLOW,"Saving %s Tupperware File",tempStr);
			log.SetProgressText("Writing %s",tempStr);
			result = TupperwareLoadSaveManager::Write(TupperwareLoadSaveManager::ASCII,tempStr,root);
			if (!result)
			{
				throw(PluginException(ERROR_FATAL,"FilterChain - Saving %s file.",tempStr));
			}
		}
	}
	catch (PluginException e)
	{
		delete pConfigAgg;
		pConfigAgg = NULL;
		log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_RED|LogFlags::FONT_STYLE_BOLD,"ERROR - %s",(const char *)(e));
		log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_RED|LogFlags::FONT_STYLE_BOLD,"Processing Aborted.");
		m_threadRunning = false;
		PostMessage(WM_KICKIDLE);
		return 1;
	}

	log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD,"Processing Completed");
	log.SetProgressText("Done");

	int nTicks = GetTickCount() - nStartTickCount;
	log.Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_STYLE_BOLD,"Total Time: %02d Min %02d Sec", (nTicks / 1000) / 60, (nTicks / 1000) % 60);

	delete pConfigAgg;

	m_threadRunning = false;
	PostMessage(WM_KICKIDLE);
	return 0;
}

void CTupperChainGuiAppDlg::OnStopProcessingButton() 
{
	m_stopButtonPressed = true;
}

bool CTupperChainGuiAppDlg::StopButtonPressed(void)
{
	return m_stopButtonPressed;
}

// make sure files exist for reading or writing and throw if there is a problem
void CTupperChainGuiAppDlg::CheckFileDependencies(TupperChainLog &log,FilePathArray &filePathArray)
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
//			log.Add(LogFlags::VERBOSE_LEVEL_HIGH,"Checking file %s for read access", pPathFilename);
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
//			log.Add(LogFlags::VERBOSE_LEVEL_HIGH,"Checking file %s for write access", pPathFilename);
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
