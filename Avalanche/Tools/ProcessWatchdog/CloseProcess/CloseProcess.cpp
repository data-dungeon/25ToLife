////////////////////////////////////////////////////////////////////////////
//
// CloseProcess
//
// Class close a process
//
////////////////////////////////////////////////////////////////////////////

/*
* $History: CloseProcess.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 10/10/03   Time: 8:47a
 * Created in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/CloseProcess
 * added new console application to close a process
*/

#include "stdafx.h"
#include "CloseProcess.h"

#include <tlhelp32.h>
#pragma comment(lib, "kernel32.lib")

#include <psapi.h>
#pragma comment(lib, "psapi.lib")


// this counts the number of children windows
BOOL CALLBACK ProcessListChildWindowsEnum( HWND hwnd, LPARAM lParam )
{
	int *pCount = (int *)lParam;
	(*pCount)++;
	return TRUE;
}

BOOL CALLBACK CloseProcess::SendMessageTimeoutEnum( HWND hwnd, LPARAM lParam )
{
	DWORD dwID ;
	SendMessageData *pSendMessageData = (SendMessageData *)lParam;

	::GetWindowThreadProcessId(hwnd, &dwID) ;

	if(dwID == pSendMessageData->m_processID)
	{
		// make sure the window has some children
		int count=0;
		::EnumChildWindows(hwnd,ProcessListChildWindowsEnum,(LPARAM) &count);
		if (count)
		{
			BOOL bPosted = ::PostMessage(
				hwnd,
				pSendMessageData->m_message,
				pSendMessageData->m_wparam,
				pSendMessageData->m_lparam);
			if (bPosted)
				pSendMessageData->m_postedCount++;
		}
	}
	return TRUE ;
}

bool CloseProcess::KillProcess(const char *pProcessNameToKill,int waitForResponse)
{
	bool bClosedAProcess = false;

	TupString processNameToKill(pProcessNameToKill);
	processNameToKill = StripExtensionFromString(pProcessNameToKill);

	HANDLE hProcessSnap = NULL; 
	bool bRet = false; 

	//  Take a snapshot of all processes in the system. 
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 


	if (hProcessSnap == INVALID_HANDLE_VALUE) 
	{
		printf("Error enumerating processes\n");
		return false; 
	}

	PROCESSENTRY32	processEntry;	// see tlhelp32.h

	processEntry.dwSize = sizeof(PROCESSENTRY32); 

	//  Walk the snapshot of the processes, and for each process, 
	//  display information. 

	if (Process32First(hProcessSnap, &processEntry)) 
	{ 
		do 
		{ 
			TupString processName = processEntry.szExeFile;
			processName = StripExtensionFromString(processName);
			if (stricmp(processName,processNameToKill)==0)
			{
				DWORD processID = processEntry.th32ProcessID;
				printf("Attempting to close process %d,%s\n",processID,(const char *)processName);
				bool bResult = KillProcess(processID,processName,waitForResponse);
				if (bResult)
					bClosedAProcess = true;
			}
		} 
		while (Process32Next(hProcessSnap, &processEntry)); 
	} 

	// clean up the snapshot object. 
	CloseHandle (hProcessSnap); 
	// return true if a process was closed
	return bClosedAProcess;
} 

bool CloseProcess::KillProcess(DWORD processID,const char *pName,int waitForResponse)
{
	HANDLE hProcess;
	bool bProcessKilled;

	// If we can't open the process with PROCESS_TERMINATE rights,
	// then we give up immediately.
	hProcess = ::OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, FALSE,	processID);

	if(hProcess == NULL)
	{
		printf("Error opening process %d,%s\n",processID,pName);
		return false;
	}
	SendMessageData messageData;
	messageData.m_processID = processID;
	messageData.m_message = WM_SYSCOMMAND;
	messageData.m_wparam = SC_CLOSE;
	messageData.m_lparam = 0;
	messageData.m_postedCount = 0;

	// send a syscommand with a close to tell the app to close (this may not work)
	EnumWindows((WNDENUMPROC)SendMessageTimeoutEnum, (LPARAM) &messageData) ;

	// Wait on the handle. If it signals, great. If it times out,
	// then send a endsession message
	if(messageData.m_postedCount==0 || WaitForSingleObject(hProcess, waitForResponse)!=WAIT_OBJECT_0)
	{
		messageData.m_wparam = 0;
		messageData.m_lparam = 0;
		messageData.m_postedCount = 0;

		messageData.m_message = WM_ENDSESSION;

		// send a end session command (this should work except when the app is frozen or crashed)
		EnumWindows((WNDENUMPROC)SendMessageTimeoutEnum, (LPARAM) &messageData) ;
		if(messageData.m_postedCount==0 || WaitForSingleObject(hProcess, waitForResponse)!=WAIT_OBJECT_0)
		{
			bProcessKilled = false; //(TerminateProcess(hProcess,0) ? true : false);
			if (!bProcessKilled)
			{
				DWORD lastError = GetLastError();
				printf("Error Terminating process %s PID: %d.\nTerminateProcess error %d\n",
					pName,processID,lastError);
			}
			else
			{
				printf("Process %s PID: %d Closed successfully using TerminateProcess.\n",
					pName,processID);
			}
		}
		else
		{
			bProcessKilled = true;
			printf("Process %s PID: %d Closed successfully using EndSession Message.\n",
				pName,processID);
		}
	}
	else
	{
		bProcessKilled = true;
		printf("Process %s PID: %d Closed successfully using Close Message.\n",
			pName,processID);
	}

	CloseHandle(hProcess);

	return bProcessKilled;
}

TupString CloseProcess::StripExtensionFromString(const TupString &string)
{
	int foundPeriod = string.Find('.');
	if (foundPeriod!=-1)
	{
		return string.Left(foundPeriod);
	}
	else
	{
		return string;
	}
}
