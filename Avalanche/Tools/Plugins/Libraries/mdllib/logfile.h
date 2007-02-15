//////////////////////////////////////////////////////////////////////////////////////////
//
//	LOGFILE CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __LOGFILE_H
#define __LOGFILE_H

#define ADDTOLOG(t) LogFile::AddToLog(t); LogFile::AddToLog("\r\n")
#define CLEARLOG LogFile::ClearLog()

class LogFile
{
public:
	static const char *m_pLogFileName;
	static bool AddToLog(const char *pText);
	static bool ClearLog(void);
};

#endif
