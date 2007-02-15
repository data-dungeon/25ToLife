////////////////////////////////////////////////////////////////////////////
//
// SimpleLog
//
// class to handle the log for a command line
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: SimpleLog.h $
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
 
#ifndef SIMPLE_LOG_H
#define SIMPLE_LOG_H

#include "TupperFilters/Libraries/Plugins/PluginLog.h"

class SimpleLog : public TupperChainLogBase
{
   TupString m_logFilename;
   bool m_bValidLogName;
	int m_verboseLevel;
	bool m_abortFlag;

public:
	SimpleLog(const char *pLogFilename);
	~SimpleLog();

	void Add(int flags,const char *pcFmt, ...);
	void Clear(void);
	int GetVerboseLevel(void);
	void SetVerboseLevel(int verboseLevel);

	void SetProgressBar(int min,int max, int value);
	void SetProgressText(const char *pFormatString, ...);
	bool GetAbortFlag(void);

};

#endif
