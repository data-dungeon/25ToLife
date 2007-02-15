////////////////////////////////////////////////////////////////////////////
//
// LogFile
//
// Class for updating the log files
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: LogFile.cpp $
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

#include "Stdafx.h"
#include "LogFile.h"


static char *commandText[LogFile::NUM_COMMANDS] =
{
	"FileCreate", // FILE_CREATE
	"FileCopy", // FILE_COPY
	"FileDelete", // FILE_DELETE
	"FileRename", // FILE_RENAME
	"FileComment", // FILE_COMMENT
	"FileStatus", // FILE_STATUS
	"FileTimeUpdate", // FILE_TIME_UPDATE
	"FolderCreate", // FOLDER_CREATE
	"FolderDelete", // FOLDER_DELETE
	"FolderRename", // FOLDER_RENAME
	"FolderComment", // FOLDER_COMMENT
	"FolderStatus", // FOLDER_STATUS
};

LogFile::Entry::Entry(void)
{
}
		
const char *LogFile::Entry::GetCommand(void) const
{
	return commandText[m_command];
}

const char *LogFile::Entry::GetFileName(void) const
{
	return m_fileName;
}

const char *LogFile::Entry::GetUserName(void) const
{
	return m_userName;
}

const char *LogFile::Entry::GetComputerName(void) const
{
	return m_computerName;
}

const char *LogFile::Entry::GetTime(void) const
{
	return m_timeString;
}

const char *LogFile::Entry::GetParameter1(void) const
{
	return m_parameter1;
}

const char *LogFile::Entry::GetParameter2(void) const
{
	return m_parameter2;
}


LogFile::LogFile(const char *pLogFileName,const char *pUserName,const char *pComputerName) :
	m_userName(pUserName),
	m_computerName(pComputerName),
	m_currentTime(0)
{
	m_logFilenameExtension = pLogFileName;
	char fname[_MAX_FNAME];
	char fext[_MAX_EXT];
	_splitpath(pLogFileName,NULL,NULL,fname,fext);
	m_logFileName = fname;
	m_logExtension = fext;
	UpdateCurrentTime();
}

bool LogFile::AddFileCreate(const char *pDestFileName,const char *pDestPathName,const char *pSourcePathName)
{
	UpdateCurrentTime();
	sprintf(m_buffer,"%s,%s,%s,%s,%s,%s\r\n",
		commandText[FILE_CREATE],
		pDestFileName,
		(const char *)m_computerName,
		(const char *)m_userName,
		(const char *)m_currentTimeString,
		pSourcePathName);
	
	return AddToLog(pDestPathName,m_buffer);
}

bool LogFile::AddFileCopy(const char *pDestFileName,const char *pDestPathName,const char *pSourcePathName)
{
	UpdateCurrentTime();
	sprintf(m_buffer,"%s,%s,%s,%s,%s,%s\r\n",
		commandText[FILE_COPY],
		pDestFileName,
		(const char *)m_computerName,
		(const char *)m_userName,
		(const char *)m_currentTimeString,
		pSourcePathName);
	
	return AddToLog(pDestPathName,m_buffer);
}

bool LogFile::AddFileDelete(const char *pDestFileName,const char *pDestPathName)
{
	UpdateCurrentTime();
	sprintf(m_buffer,"%s,%s,%s,%s,%s\r\n",
		commandText[FILE_DELETE],
		pDestFileName,
		(const char *)m_computerName,
		(const char *)m_userName,
		(const char *)m_currentTimeString);
	
	return AddToLog(pDestPathName,m_buffer);
}

bool LogFile::AddFileRename(const char *pDestFileName,const char *pDestPathName,const char *pSourcePathName)
{
	UpdateCurrentTime();
	sprintf(m_buffer,"%s,%s,%s,%s,%s,%s\r\n",
		commandText[FILE_RENAME],
		pDestFileName,
		(const char *)m_computerName,
		(const char *)m_userName,
		(const char *)m_currentTimeString,
		(const char *)pSourcePathName);
	
	return AddToLog(pDestPathName,m_buffer);
}

bool LogFile::AddFileComment(const char *pDestFileName,const char *pDestPathName,const char *pCommentText)
{
	UpdateCurrentTime();
	sprintf(m_buffer,"%s,%s,%s,%s,%s,%s\r\n",
		commandText[FILE_COMMENT],
		pDestFileName,
		(const char *)m_computerName,
		(const char *)m_userName,
		(const char *)m_currentTimeString,
		pCommentText);
	
	return AddToLog(pDestPathName,m_buffer);
}

bool LogFile::AddFileStatus(const char *pDestFileName,const char *pDestPathName,const char *pStatusText)
{
	UpdateCurrentTime();
	sprintf(m_buffer,"%s,%s,%s,%s,%s,%s\r\n",
		commandText[FILE_STATUS],
		pDestFileName,
		(const char *)m_computerName,
		(const char *)m_userName,
		(const char *)m_currentTimeString,
		pStatusText);
	
	return AddToLog(pDestPathName,m_buffer);
}

bool LogFile::AddFileTimeUpdate(const char *pDestFileName,const char *pDestPathName)
{
	UpdateCurrentTime();
	sprintf(m_buffer,"%s,%s,%s,%s,%s\r\n",
		commandText[FILE_TIME_UPDATE],
		pDestFileName,
		(const char *)m_computerName,
		(const char *)m_userName,
		(const char *)m_currentTimeString);

	return AddToLog(pDestPathName,m_buffer);
}


bool LogFile::AddFolderCreate(const char *pDestFolderName,const char *pDestPathName,const char *pSourcePathName)
{
	UpdateCurrentTime();
	sprintf(m_buffer,"%s,%s,%s,%s,%s,%s\r\n",
		commandText[FOLDER_CREATE],
		pDestFolderName,
		(const char *)m_computerName,
		(const char *)m_userName,
		(const char *) m_currentTimeString,
		pSourcePathName);
	
	return AddToLog(pDestPathName,m_buffer);
}

bool LogFile::AddFolderDelete(const char *pDestFolderName,const char *pDestPathName)
{
	UpdateCurrentTime();
	sprintf(m_buffer,"%s,%s,%s,%s,%s\r\n",
		commandText[FOLDER_DELETE],
		pDestFolderName,
		(const char *)m_computerName,
		(const char *)m_userName,
		(const char *) m_currentTimeString);
	
	return AddToLog(pDestPathName,m_buffer);
}

bool LogFile::AddFolderRename(const char *pDestFolderName,const char *pDestPathName,const char *pSourcePathName)
{
	UpdateCurrentTime();
	sprintf(m_buffer,"%s,%s,%s,%s,%s,%s\r\n",
		commandText[FOLDER_RENAME],
		pDestFolderName,
		(const char *)m_computerName,
		(const char *)m_userName,
		(const char *) m_currentTimeString,
		pSourcePathName);
	
	return AddToLog(pDestPathName,m_buffer);
}

bool LogFile::AddFolderComment(const char *pDestFolderName,const char *pDestPathName,const char *pCommentText)
{
	UpdateCurrentTime();
	sprintf(m_buffer,"%s,%s,%s,%s,%s,%s\r\n",
		commandText[FOLDER_COMMENT],
		pDestFolderName,
		(const char *)m_computerName,
		(const char *)m_userName,
		(const char *) m_currentTimeString,
		pCommentText);
	
	return AddToLog(pDestPathName,m_buffer);
}

bool LogFile::AddFolderStatus(const char *pDestFolderName,const char *pDestPathName,const char *pStatusText)
{
	UpdateCurrentTime();
	sprintf(m_buffer,"%s,%s,%s,%s,%s,%s\r\n",
		commandText[FOLDER_STATUS],
		pDestFolderName,
		(const char *)m_computerName,
		(const char *)m_userName,
		(const char *) m_currentTimeString,
		pStatusText);
	
	return AddToLog(pDestPathName,m_buffer);
}

bool LogFile::AddToLog(const char *pDestPathName,const char *pText)
{
	char pathBuffer[_MAX_PATH];
	
	char fdrive[_MAX_DRIVE];
	char fdir[_MAX_DIR];
	
	_splitpath(pDestPathName,fdrive,fdir,NULL,NULL);
	_makepath(pathBuffer,fdrive,fdir,m_logFileName,m_logExtension);

	int stringLength = strlen(pText);

	HANDLE hFile = INVALID_HANDLE_VALUE;
	
	int retryAttempts = 10;

	for (int attemptCount=0;attemptCount<retryAttempts;attemptCount++)
	{
		hFile = CreateFile(pathBuffer,   // open log file
			GENERIC_WRITE,                // open for writing 
			0,                            // do not share 
			NULL,                         // no security 
			OPEN_ALWAYS,                  // open or create 
			FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM, // system & hidden file 
			NULL);                        // no attr. template 

		if (hFile != INVALID_HANDLE_VALUE)
		{
			break;
		}
		Sleep(100); // wait a little bit before trying again
	}

	// if we not able to do it then get out
	if (hFile == INVALID_HANDLE_VALUE) 
	{ 
		return false;
	} 
	DWORD bytesWritten;

	DWORD dwPos = SetFilePointer(hFile, 0, NULL, FILE_END); 
	LockFile(hFile, dwPos, 0, dwPos + stringLength, 0); 
	WriteFile(hFile, pText, stringLength, &bytesWritten, NULL); 
	UnlockFile(hFile, dwPos, 0, dwPos + stringLength, 0); 
	
	CloseHandle(hFile); 
	return true;
}

void LogFile::UpdateCurrentTime(void)
{
	CTime newCurrentTime = CTime::GetCurrentTime();
	if (newCurrentTime != m_currentTime)
	{
		m_currentTimeString = newCurrentTime.Format("%m/%d/%y,%I:%M:%S %p");
		m_currentTime = newCurrentTime;
	}
}

const char *LogFile::GetLogFileName(void) const
{
	return m_logFilenameExtension;
}
