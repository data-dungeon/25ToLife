////////////////////////////////////////////////////////////////////////////
//
// LogFile
//
// Class for updating the log files
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: LogFile.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 8/15/03    Time: 10:26a
 * Updated in $/Avalanche/tools/SyncUtils
 * added time update to log on the source file when copying over a newer
 * file
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/28/03    Time: 4:30p
 * Updated in $/Avalanche/tools/SyncUtils
 * view status flags
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/27/03    Time: 4:34p
 * Updated in $/Avalanche/tools/SyncUtils
 * log file output
 */

#ifndef SYNC_UTILS_DLL_LOG_FILE_H
#define SYNC_UTILS_DLL_LOG_FILE_H

//-----------------------------------------------------------------------------
// SyncDBFactory class declaration:

class AFX_EXT_CLASS LogFile
{
public:
	enum Commands
	{
		FILE_CREATE,
		FILE_COPY,
		FILE_DELETE,
		FILE_RENAME,
		FILE_COMMENT,
		FILE_STATUS,
		FILE_TIME_UPDATE,
		FOLDER_CREATE,
		FOLDER_DELETE,
		FOLDER_RENAME,
		FOLDER_COMMENT,
		FOLDER_STATUS,
		NUM_COMMANDS // must be last
	};

	class AFX_EXT_CLASS Entry
	{
		Commands m_command;
		TupString m_fileName;
		TupString m_userName;
		TupString m_computerName;
		CTime m_time;
		TupString m_timeString;
		TupString m_parameter1;
		TupString m_parameter2;

	public:
		Entry(void);

		const char *GetCommand(void) const;
		const char *GetFileName(void) const;
		const char *GetUserName(void) const;
		const char *GetComputerName(void) const;
		const char *GetTime(void) const;
		const char *GetParameter1(void) const;
		const char *GetParameter2(void) const;
	};
	
	class AFX_EXT_CLASS Database
	{
		TupArray<Entry> m_entries;
	};



	LogFile(const char *pLogFileName,const char *pUserName,const char *pComputerName);

	bool AddFileCreate(const char *pDestFileName,const char *pDestPathName,const char *pSourcePathName);
	bool AddFileCopy(const char *pDestFileName,const char *pDestPathName,const char *pSourcePathName);
	bool AddFileDelete(const char *pDestFileName,const char *pDestPathName);
	bool AddFileRename(const char *pDestFileName,const char *pDestPathName,const char *pSourcePathName);
	bool AddFileComment(const char *pDestFileName,const char *pDestPathName,const char *pCommentText);
	bool AddFileStatus(const char *pDestFileName,const char *pDestPathName,const char *pStatusText);
	bool AddFileTimeUpdate(const char *pDestFileName,const char *pDestPathName);

	bool AddFolderCreate(const char *pDestFolderName,const char *pDestPathName,const char *pSourcePathName);
	bool AddFolderCopy(const char *pDestFolderName,const char *pDestPathName,const char *pSourcePathName);
	bool AddFolderRename(const char *pDestFolderName,const char *pDestPathName,const char *pSourcePathName);
	bool AddFolderDelete(const char *pDestFolderName,const char *pDestPathName);
	bool AddFolderComment(const char *pDestFolderName,const char *pDestPathName,const char *pCommentText);
	bool AddFolderStatus(const char *pDestFolderName,const char *pDestPathName,const char *pStatusText);

	const char *GetLogFileName(void) const;

private:
	bool AddToLog(const char *pDestPathName,const char *pText);
	void UpdateCurrentTime(void);

	char m_buffer[8192];
	TupString m_currentTimeString;
	CTime m_currentTime;

	TupString m_userName;
	TupString m_computerName;
	TupString m_logFilenameExtension;
	TupString m_logFileName;
	TupString m_logExtension;
};

//-----------------------------------------------------------------------------

#endif

