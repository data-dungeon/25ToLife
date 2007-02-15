////////////////////////////////////////////////////////////////////////////
//
// Log
//
// class to handle the log within the dialog
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: log.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 7/16/02    Time: 12:48p
 * Updated in $/Avalanche/tools/TupperChain/TupperChainGuiApp
 * better path support - the work directory can be different from the exe
 * file
 * also a log file is now saved out all the time
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/18/02    Time: 12:33p
 * Updated in $/Avalanche/tools/TupperChain/TupperChainGuiApp
 * Worker thread for processing
 * Stop Processing Button
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/15/02    Time: 3:16p
 * Updated in $/Avalanche/tools/TupperChain/TupperChainGuiApp
 * Progress Bar & Text
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/01/02    Time: 12:05p
 * Updated in $/Avalanche/tools/TupperChain/TupperChainGuiApp
 * Rich Edit Control for the Log.  Added additional flags for font styles
 * and colors.
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:49p
 * Updated in $/Avalanche/tools/TupperChain/TupperChainGuiApp
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/19/02    Time: 5:08p
 * Updated in $/Avalanche/tools/TupperChain/TupperChainGuiApp
 * First stage of gui app complete
 * Log window working
 * Exceptions set up
 */

#include "stdafx.h"
#include "TupperChainGuiApp.h"
#include "TupperChainGuiAppDlg.h"
#include "log.h"



TupperChainLog::TupperChainLog(CTupperChainGuiAppDlg &dialog,const char *pLogFilename) :
	TupperChainLogBase() , m_dialog(dialog), m_verboseLevel(0), m_abortFlag(false)
{
	if (pLogFilename)
	{
		m_pFile = fopen(pLogFilename,"wb");
	}
	else
	{
		m_pFile = NULL;
	}
}

TupperChainLog::~TupperChainLog(void)
{
	if (m_pFile)
	{
		fclose(m_pFile);
	}
}

void TupperChainLog::Add(int flags,const char *pFormatString, ...)
{
	int verboseLevel = (flags>>LogFlags::VERBOSE_LEVEL_SHIFT)&LogFlags::VERBOSE_LEVEL_MASK;

	int nLen;
	static char acMessage[800];
	va_list PrintArgs;

	/* create message */

	va_start(PrintArgs, pFormatString);
	vsprintf(acMessage, pFormatString, PrintArgs);
	va_end(PrintArgs);

	if ((nLen = strlen(acMessage)) == 0)
		return;

	if (m_pFile)
	{
		fprintf(m_pFile,"%s\r\n",acMessage);
	}

	if (verboseLevel<=m_verboseLevel)
	{
		m_dialog.WriteLog(flags,acMessage);
	}
}

void TupperChainLog::Clear(void)
{
	m_dialog.ClearLog();
}

int TupperChainLog::GetVerboseLevel(void)
{
	return m_verboseLevel;
}

void TupperChainLog::SetVerboseLevel(int verboseLevel)
{
	m_verboseLevel = verboseLevel;
}

void TupperChainLog::SetProgressBar(int min,int max, int value)
{
	m_dialog.SetProgressBar(min,max,value);
}

void TupperChainLog::SetProgressText(const char *pFormatString, ...)
{
	int nLen;
	static char acMessage[800];
	va_list PrintArgs;

	/* create message */

	va_start(PrintArgs, pFormatString);
	vsprintf(acMessage, pFormatString, PrintArgs);
	va_end(PrintArgs);

	/* anything to do? */
	if ((nLen = strlen(acMessage)) == 0)
		return;

	/* write to window if it exists */
	m_dialog.SetProgressText(acMessage);
}

bool TupperChainLog::GetAbortFlag(void)
{
	return m_dialog.StopButtonPressed();
}

