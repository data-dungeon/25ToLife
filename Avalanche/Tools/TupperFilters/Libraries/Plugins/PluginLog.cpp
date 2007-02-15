////////////////////////////////////////////////////////////////////////////
//
// PluginLog
//
// class for all plugin logs
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: PluginLog.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 7/26/02    Time: 12:55p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * pass all text through
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 4/12/02    Time: 1:34p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * flags - verboseLevel bug
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/18/02    Time: 12:33p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Abort checks within system
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/15/02    Time: 3:16p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Progress Bar
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Exceptions working properly
 * Log working with all plugins
 */
  
#include "PluginsPch.h"
#include "PluginLog.h"

TupperChainLogBase *PluginLog::m_pLog = 0;

void PluginLog::SetLog(TupperChainLogBase *pLog)
{
	m_pLog = pLog;
}

void PluginLog::Add(int flags,const char *pFormatString, ...)
{
	assert(m_pLog);
	static char acMessage[800];
	va_list PrintArgs;

	/* create message */

	va_start(PrintArgs, pFormatString);
	vsprintf(acMessage, pFormatString, PrintArgs);
	va_end(PrintArgs);

	m_pLog->Add(flags,acMessage);
}

void PluginLog::Clear(void)
{
	assert(m_pLog);
	m_pLog->Clear();
}

int PluginLog::GetVerboseLevel(void)
{
	assert(m_pLog);
	return m_pLog->GetVerboseLevel();
}

void PluginLog::SetProgressBar(int min,int max, int value)
{
	assert(m_pLog);
	m_pLog->SetProgressBar(min,max,value);
}

void PluginLog::SetProgressText(const char *pFormatString, ...)
{
	static char acMessage[800];
	va_list PrintArgs;

	/* create message */

	va_start(PrintArgs, pFormatString);
	vsprintf(acMessage, pFormatString, PrintArgs);
	va_end(PrintArgs);

	m_pLog->SetProgressText(acMessage);
}

bool PluginLog::GetAbortFlag(void)
{
	return m_pLog->GetAbortFlag();
}


