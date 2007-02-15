// ProcessWatchDogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessList.h"
#include "Watchdog.h"

#include <tlhelp32.h>
#pragma comment(lib, "kernel32.lib")

#include <psapi.h>
#pragma comment(lib, "psapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool GetProcessModule (DWORD           dwPID,
							  LPCTSTR         pstrModule,
							  LPMODULEENTRY32 lpMe32,
							  DWORD           cbMe32)
{
	BOOL			bRet;
	bool			bFound      = false;
	HANDLE			hModuleSnap = NULL;
	MODULEENTRY32	me32        = {0};
	int				nLen = (int)_tcslen(pstrModule);

	if(!nLen)
	{
		return false;
	}

	//
	// for simplicity we do not adjust our security token 
	// which would allow access to all system processes.
	//

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hModuleSnap == INVALID_HANDLE_VALUE) 
	{
#ifdef _DEBUG
		DWORD dwRet = GetLastError();
#endif
		return (false);
	}

	me32.dwSize = sizeof(MODULEENTRY32);

	bRet = Module32First(hModuleSnap, &me32);
	while (bRet && !bFound) 
	{
		//
		// locate the given filename in the modulelist (usually its the first anyway)
		//

		if(_tcsnicmp(me32.szModule, pstrModule, nLen) == 0) 
		{
			CopyMemory(lpMe32, &me32, cbMe32);
			bFound = true;
		}
		bRet = Module32Next(hModuleSnap, &me32);
	}
	CloseHandle (hModuleSnap);

	return (bFound);
}

ProcessManager::ProcessManager(void)
{
}

void ProcessManager::Lock(void)
{
	m_criticalSection.Lock();
}

void ProcessManager::Unlock(void)
{
	m_criticalSection.Unlock();
}

int ProcessManager::GetNumProcesses(void)
{
	return m_processes.GetSize();
}

const ProcessManager::ProcessData &ProcessManager::GetProcess(int index) const
{
	return m_processes[index];
}

ProcessManager::ProcessData &ProcessManager::GetProcess(int index)
{
	return m_processes[index];
}

// this counts the number of children windows
BOOL CALLBACK ProcessListChildWindowsEnum( HWND hwnd, LPARAM lParam )
{
	int *pCount = (int *)lParam;
	(*pCount)++;
	return TRUE;
}

BOOL CALLBACK ProcessListWindowsEnum( HWND hwnd, LPARAM lParam )
{
	ProcessManager *pThis = (ProcessManager *)lParam;

	DWORD dwPID ;
	GetWindowThreadProcessId(hwnd, &dwPID) ;
	int count=0;
	::EnumChildWindows(hwnd,ProcessListChildWindowsEnum,(LPARAM) &count);
	if (count)
	{
		pThis->AddProcessHWND(dwPID,hwnd);
	}
	return TRUE ;
}

void ProcessManager::RefreshProcesses(WatchdogManager *pWatchdogManager)
{
	m_processes.RemoveAll();

	HANDLE hProcessSnap = NULL; 
	bool bRet = false; 

	//  Take a snapshot of all processes in the system. 
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

	if (hProcessSnap == INVALID_HANDLE_VALUE) 
		return; 

	PROCESSENTRY32	processEntry;	// see tlhelp32.h

	processEntry.dwSize = sizeof(PROCESSENTRY32); 

	//  Walk the snapshot of the processes, and for each process, 
	//  display information. 

	if (Process32First(hProcessSnap, &processEntry)) 
	{ 
		do 
		{ 
			ProcessData newProcessData;
			newProcessData.m_processID = processEntry.th32ProcessID;
			newProcessData.m_processName = processEntry.szExeFile;

			HANDLE			hProcess; 
			bool bGotTimes = false;

			hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processEntry.th32ProcessID); 
			if((hProcess != NULL) && (hProcess != INVALID_HANDLE_VALUE))
			{
				FILETIME creationTime,exitTime,kernelTime,userTime;

				if (::GetProcessTimes(hProcess,&creationTime,&exitTime,&kernelTime,&userTime))
				{
					newProcessData.m_creationTime = creationTime;
					bGotTimes = true;
				}

				newProcessData.m_priorityClassBase = ::GetPriorityClass(hProcess);

				::CloseHandle(hProcess);
			}

			m_processes.Add(newProcessData);

			processEntry.dwSize = sizeof(PROCESSENTRY32); 
		} 
		while (Process32Next(hProcessSnap, &processEntry)); 
	} 

	// clean up the snapshot object. 
	CloseHandle (hProcessSnap); 

	EnumWindows((WNDENUMPROC)ProcessListWindowsEnum, (LPARAM) this);

} 

void ProcessManager::AddProcessHWND(DWORD processID,HWND hwnd)
{
	int numProcesses = m_processes.GetSize();
	for (int processIndex=0;processIndex<numProcesses;processIndex++)
	{
		ProcessData &processData = m_processes[processIndex];
		if (processData.m_processID == processID)
		{
			processData.m_topWindows.Add(hwnd);
		}
	}
}

void ProcessManager::UpdateOld(void)
{
	m_oldProcesses = m_processes;
}

bool ProcessManager::CompareToOld(void) const
{
	int numProcesses = m_processes.GetSize();
	if (m_oldProcesses.GetSize()!=numProcesses)
		return false;

	for (int processIndex=0;processIndex<numProcesses;processIndex++)
	{
		const ProcessData &processData = m_processes[processIndex];
		const ProcessData &oldProcessData = m_oldProcesses[processIndex];
		if (processData.m_processID!=oldProcessData.m_processID) return false;
		if (processData.m_processName!=oldProcessData.m_processName) return false;
		if (processData.m_priorityClassBase!=oldProcessData.m_priorityClassBase) return false;
		if (processData.m_watchdogProcessStatus!=oldProcessData.m_watchdogProcessStatus) return false;
	}
	return true;
}
