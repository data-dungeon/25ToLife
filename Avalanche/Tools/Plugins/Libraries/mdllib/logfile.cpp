

#include "Stdafx.h"
#include "logfile.h"

const char *LogFile::m_pLogFileName = "c:\\mdllib.log";

bool LogFile::AddToLog(const char *pText)
{
	if (!pText)
		return false;
	int stringLength = strlen(pText);

	HANDLE hFile = INVALID_HANDLE_VALUE;
	
	int retryAttempts = 10;
	
	for (int attemptCount=0;attemptCount<retryAttempts;attemptCount++)
	{
		hFile = CreateFile(m_pLogFileName,   // open log file
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

bool LogFile::ClearLog(void)
{
	::DeleteFile(m_pLogFileName);
	return true;
}
