////////////////////////////////////////////////////////////////////////////
//
// SimpleLog
//
// class to handle the log within a command line app
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: SimpleLog.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 7/16/02    Time: 12:47p
 * Updated in $/Avalanche/tools/TupperChain/TupperChainApp
 * Command line version is now working
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 1:27p
 * Updated in $/Avalanche/tools/TupperChain
 * Made command line version compile (did not test it completely yet)
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:49p
 * Updated in $/Avalanche/tools/TupperChain
 * Exceptions working properly
 * Log working with all plugins
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "simplelog.h"

SimpleLog::SimpleLog(const char *pLogFilename) : m_verboseLevel(0)
{
	if (pLogFilename)
	{
		m_pFile = fopen(pLogFilename,"wb");
		if (!m_pFile)
		{
			printf("Error opening log file %s",pLogFilename);
			m_pFile = NULL;
		}
	}
	else
	{
		m_pFile = NULL;
	}
}

SimpleLog::~SimpleLog(void)
{
	if (m_pFile)
	{
		fclose(m_pFile);
	}
}

void SimpleLog::Add(int flags,const char *pcFmt, ...)
{
	int verboseLevel = (flags>>LogFlags::VERBOSE_LEVEL_SHIFT)&LogFlags::VERBOSE_LEVEL_MASK;

	if (verboseLevel<=m_verboseLevel)
	{
		int nLen;
		static char acMessage[800];
		va_list PrintArgs;

		/* create message */

		va_start(PrintArgs, pcFmt);
		vsprintf(acMessage, pcFmt, PrintArgs);
		va_end(PrintArgs);

		/* anything to do? */
		if ((nLen = strlen(acMessage)) == 0)
			return;

		/* write to window if it exists */
		if (m_pFile)
		{
			fprintf(m_pFile,"%s\r\n",acMessage);
			// print the low messages
			if (verboseLevel==0)
				printf("%s\n",acMessage);
		}
		else
		{
			printf("%s\n",acMessage);
		}
	}
}

void SimpleLog::Clear(void)
{
}

int SimpleLog::GetVerboseLevel(void)
{
	return m_verboseLevel;
}

void SimpleLog::SetVerboseLevel(int verboseLevel)
{
	m_verboseLevel = verboseLevel;
}


void SimpleLog::SetProgressBar(int min,int max, int value)
{
}

void SimpleLog::SetProgressText(const char *pFormatString, ...)
{
}

bool SimpleLog::GetAbortFlag(void)
{
	return false;
}

