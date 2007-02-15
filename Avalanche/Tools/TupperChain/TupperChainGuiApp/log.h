////////////////////////////////////////////////////////////////////////////
//
// Log
//
// class to handle the log within the dialog
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: log.h $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 7/16/02    Time: 12:48p
 * Updated in $/Avalanche/tools/TupperChain/TupperChainGuiApp
 * better path support - the work directory can be different from the exe
 * file
 * also a log file is now saved out all the time
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/18/02    Time: 12:33p
 * Updated in $/Avalanche/tools/TupperChain/TupperChainGuiApp
 * Worker thread for processing
 * Stop Processing Button
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/15/02    Time: 3:16p
 * Updated in $/Avalanche/tools/TupperChain/TupperChainGuiApp
 * Progress Bar & Text
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

#ifndef LOG_H
#define LOG_H

class CTupperChainGuiAppDlg;

class TupperChainLog : public TupperChainLogBase
{
	CTupperChainGuiAppDlg &m_dialog;
	int m_verboseLevel;
	bool m_abortFlag;

	FILE *m_pFile;

public:
	TupperChainLog(CTupperChainGuiAppDlg &dialog,const char *pLogFilename);
	~TupperChainLog();

	void Add(int verboseLevel,const char *pcFmt, ...);
	void Clear(void);
	int GetVerboseLevel(void);
	void SetVerboseLevel(int verboseLevel);
	void SetProgressBar(int min,int max, int value);
	void SetProgressText(const char *pFormatString, ...);
	bool GetAbortFlag(void);
};

#endif
