////////////////////////////////////////////////////////////////////////////
//
// Watchdog
//
// Class hold information about a watchdog
//
////////////////////////////////////////////////////////////////////////////

/*
* $History: Watchdog.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 10/10/03   Time: 8:46a
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * better ability to close processes (window needs children)
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 10/03/03   Time: 12:06p
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * more robust kill and check if responding code
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 10/02/03   Time: 12:37p
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * looks like we are here!
*/

#include "stdafx.h"
#include "Watchdog.h"
#include "ProcessWatchDogDlg.h"
#include "ProcessList.h"

static int CompareTimes(FILETIME time1,FILETIME time2,float addSecTo1,float addSecTo2)
{
	__int64 oneValue = *((__int64*)&time1);
	__int64 twoValue = *((__int64*)&time2);
	oneValue += (__int64)(10000000.0 * addSecTo1);
	twoValue += (__int64)(10000000.0 * addSecTo2);
	__int64 diff = oneValue-twoValue;
	if (diff<0)
	{
		return -1; // one is older than two
	}
	else if (diff>0)
	{
		return 1; // two is older than one
	}
	else
	{
		return 0; // one is equal to two
	}
}

WatchdogManager::WatchdogManager(CProcessWatchDogDlg &parentDialog,ProcessManager &processManager) :
	m_parentDialog(parentDialog),m_processManager(processManager),m_bEndManagerThread(false)
{
	m_pManagerThread = AfxBeginThread(UpdateWatchdogProc,this); // start the thread for handling the watchdogs
	m_pManagerThread->m_bAutoDelete = FALSE; // we need access to this after the thread terminates
	m_lockCount = 0;
}

WatchdogManager::~WatchdogManager()
{
	// tell the manager thread we are going away
	Lock();
	m_bEndManagerThread = true;
	Unlock();
	while(1)
	{
		Sleep(1000);
		DWORD retCode;
		if (!GetExitCodeThread(m_pManagerThread->m_hThread,&retCode))
		{
			ASSERT(FALSE);
			break;
		}
		// if thread is not running then we are done
		if (retCode!=STILL_ACTIVE)
		{
			break;
		}
	}
	delete m_pManagerThread; // autodelete is false...
}

void WatchdogManager::Lock(void)
{
	m_criticalSection.Lock();
	m_lockCount++;
}

void WatchdogManager::Unlock(void)
{
	m_lockCount--;
	m_criticalSection.Unlock();
}

void WatchdogManager::EnableAddThreads(void)
{
	m_bManagerAddsThreads = true;
}

void WatchdogManager::DisableAddThreads(void)
{
	m_bManagerAddsThreads = false;
}


int WatchdogManager::GetNumWatchdogs(void) const
{
	return m_watchDogs.GetSize();
}

const Watchdog &WatchdogManager::GetWatchdog(int index) const
{
	return m_watchDogs[index];
}

Watchdog &WatchdogManager::GetWatchdog(int index)
{
	return m_watchDogs[index];
}

void WatchdogManager::AddWatchdog(const Watchdog &watchdog)
{
	m_watchDogs.Add(watchdog);
}

void WatchdogManager::DeleteWatchdog(int index)
{
	m_watchDogs.RemoveAt(index);
}

TupString WatchdogManager::StripExtensionFromString(const TupString &string)
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

int WatchdogManager::FindWatchdogByName(const char *pName) const
{
	TupString name(pName);
	name = StripExtensionFromString(name);

	int numWatchdogs = m_watchDogs.GetSize();
	for (int watchdogIndex=0;watchdogIndex<numWatchdogs;watchdogIndex++)
	{
		TupString testName = StripExtensionFromString(m_watchDogs[watchdogIndex].m_name);
		if (!stricmp(name,testName))
		{
			return watchdogIndex;
		}
	}
	return -1;
}

bool WatchdogManager::FindWatchdogAndProcessByID(DWORD processID,int &watchdogIndex,int &processIndex)
{
	int numWatchdogs = m_watchDogs.GetSize();
	for (watchdogIndex=0;watchdogIndex<numWatchdogs;watchdogIndex++)
	{
		Watchdog &watchdog = m_watchDogs[watchdogIndex];
		int numProcesses = watchdog.m_processes.GetSize();
		for (processIndex=0;processIndex<numProcesses;processIndex++)
		{
			Watchdog::Process &process = watchdog.m_processes[processIndex];
			if (process.m_processID==processID)
			{
				return true;
			}
		}
	}
	return false;
}

UINT WatchdogManager::UpdateWatchdogProc(LPVOID pParam)
{
	WatchdogManager *pWatchdogManager = (WatchdogManager *) pParam;
	pWatchdogManager->UpdateWatchdogThread();
	return 0;
}

void WatchdogManager::UpdateWatchdogThread(void)
{
	while(1)
	{
		int processIndex;
		int watchdogIndex;
		bool bRefresh = false;

		Lock();

		if (!m_bEndManagerThread)
		{
			m_processManager.Lock();
			m_processManager.RefreshProcesses(this);

			// add any processes to the watchdog as needed
			int numProcesses = m_processManager.GetNumProcesses();
			int numWatchdogs = m_watchDogs.GetSize();
			for (processIndex=0;processIndex<numProcesses;processIndex++)
			{
				ProcessManager::ProcessData &process = m_processManager.GetProcess(processIndex);
				TupString processName = StripExtensionFromString(process.m_processName);

				for (watchdogIndex=0;watchdogIndex<numWatchdogs;watchdogIndex++)
				{
					Watchdog &watchdog = m_watchDogs[watchdogIndex];
					TupString watchdogName = StripExtensionFromString(watchdog.m_name);
					// if we found a watchdog that matches the name
					if (!stricmp(processName,watchdogName))
					{
						bool bFoundWatchdogProcess = false;
						int numWatchdogProcesses = watchdog.m_processes.GetSize();
						for (int watchdogProcessIndex=0;watchdogProcessIndex<numWatchdogProcesses;watchdogProcessIndex++)
						{
							Watchdog::Process &watchdogProcess = watchdog.m_processes[watchdogProcessIndex];
							if (process.m_processID == watchdogProcess.m_processID)
							{
								process.m_watchdogProcessStatus = watchdogProcess.m_status; // set the status from the watchdog
								watchdogProcess.m_topWindows = process.m_topWindows; // update the top windows for the watchdog
								bFoundWatchdogProcess = true;
								break;
							}
						}
						// if one not found
						if (!bFoundWatchdogProcess)
						{
							process.m_watchdogProcessStatus = Watchdog::Process::PROCESS_STATUS_NOT_CONNECTED;
							// if we can add a new watchdog thread
							if (m_bManagerAddsThreads)
							{
								process.m_watchdogProcessStatus = Watchdog::Process::PROCESS_STATUS_CONNECTING;

								m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_BLACK,
									"Connecting Watchdog %s to Process %d\r\n",
									(const char *)watchdog.m_name,process.m_processID);

								WatchdogManager::ThreadData *pThreadData = new WatchdogManager::ThreadData;
								pThreadData->m_processID = process.m_processID;
								pThreadData->m_pWatchdogManager = this;

								Watchdog::Process newProcess;
								newProcess.m_processID = process.m_processID;
								newProcess.m_creationTime = process.m_creationTime;
								newProcess.m_topWindows = process.m_topWindows;
								newProcess.m_status = Watchdog::Process::PROCESS_STATUS_CONNECTING;
								newProcess.m_bEndThread = false;
								newProcess.m_pThread = 	AfxBeginThread(UpdateWatchdogProcessProc,pThreadData); // start the thread for the process

								watchdog.m_processes.Add(newProcess);

								bRefresh = true;
							}
						}
					}
				}
			}

			// remove any processes from the watchdogs
			for (watchdogIndex=0;watchdogIndex<numWatchdogs;watchdogIndex++)
			{
				Watchdog &watchdog = m_watchDogs[watchdogIndex];

				int numWatchdogProcesses = watchdog.m_processes.GetSize();
				if (numWatchdogProcesses)
				{
					for (int watchdogProcessIndex=0;watchdogProcessIndex<numWatchdogProcesses;watchdogProcessIndex++)
					{
						bool bFoundProcess = false;
						for (processIndex=0;processIndex<numProcesses;processIndex++)
						{
							const ProcessManager::ProcessData &process = m_processManager.GetProcess(processIndex);
							if (watchdog.m_processes[watchdogProcessIndex].m_processID ==process.m_processID)
							{
								bFoundProcess = true;
								break;
							}
						}
						// if not found and we have not told it to go away previously
						if (!bFoundProcess && !watchdog.m_processes[watchdogProcessIndex].m_bEndThread)
						{
							m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_BLACK,
								"Disconnecting Watchdog %s from Process %d.\r\n",
								(const char *)watchdog.m_name,watchdog.m_processes[watchdogProcessIndex].m_processID);

							// tell the thread we are done
							watchdog.m_processes[watchdogProcessIndex].m_bEndThread = true;
							bRefresh = true;
						}
					}
					SYSTEMTIME currentSystemTime;
					::GetSystemTime(&currentSystemTime);		// gets current time
					::SystemTimeToFileTime(&currentSystemTime,&watchdog.m_noProcessTime);	// converts to file time format
				}
				else
				{
					if ((watchdog.m_flags & Watchdog::NO_PROCESSES_STATE_MASK) == Watchdog::NO_PROCESSES_STATE_OPEN &&
						watchdog.m_targetPath.GetLength())
					{
						FILETIME currentFileTime;
						SYSTEMTIME currentSystemTime;
						::GetSystemTime(&currentSystemTime);		// gets current time
						::SystemTimeToFileTime(&currentSystemTime,&currentFileTime);	// converts to file time format
						// if we have waited long enough
						if (CompareTimes(watchdog.m_noProcessTime,currentFileTime,watchdog.m_waitForNoProcesses,0.0f)<0)
						{
							watchdog.m_noProcessTime = currentFileTime;
							int runType;
							switch (watchdog.m_runType)
							{
								case Watchdog::RUN_TYPE_NORMAL_WINDOW:
									runType = SW_SHOWNORMAL;
									break;
								case Watchdog::RUN_TYPE_MINIMIZED:
									runType = SW_SHOWMINIMIZED;
									break;
								case Watchdog::RUN_TYPE_MAXIMIZED:
									runType = SW_SHOWMAXIMIZED;
									break;
							}
							m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_BLACK,
								"Watchdog Opening File\n\tFile: %s\n\tParameters: %s\n\tStart Directory: %s.\r\n",
								(const char *)watchdog.m_targetPath,
								(const char *)watchdog.m_parameters,
								(const char *)watchdog.m_startInPath);

							__int64 retValue = (__int64) ShellExecute(NULL, // hwnd
																		  NULL, // operation
																		  watchdog.m_targetPath, // filepath
																		  watchdog.m_parameters, // parameters
																		  watchdog.m_startInPath, // start directory
																		  runType); // show command
							if (retValue<32)
							{
								switch (retValue)
								{
									case 0:
										m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_DK_RED,
											"Error Opening File %s\n\tThe operating system is out of memory or resources.\r\n",
											(const char *)watchdog.m_targetPath);
										break;
									case ERROR_BAD_FORMAT:
										m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_DK_RED,
											"Error Opening File %s\n\tThe .exe file is invalid.\r\n",
											(const char *)watchdog.m_targetPath);
										break;
									case SE_ERR_ACCESSDENIED:
										m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_DK_RED,
											"Error Opening File %s\n\tThe operating system denied access to the specified file.\r\n",
											(const char *)watchdog.m_targetPath);
										break;
									case SE_ERR_ASSOCINCOMPLETE:
										// AddToLog("The file name association is incomplete or invalid.");
										m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_DK_RED,
											"Error Opening File %s\n\tThe file name association is incomplete or invalid.\r\n",
											(const char *)watchdog.m_targetPath);
										break;
									case SE_ERR_DDEBUSY:
									case SE_ERR_DDEFAIL:
									case SE_ERR_DDETIMEOUT:
										m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_DK_RED,
											"Error Opening File %s\n\tThe Dynamic Data Exchange (DDE) error.\r\n",
											(const char *)watchdog.m_targetPath);
										break;
									case SE_ERR_DLLNOTFOUND:
										m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_DK_RED,
											"Error Opening File %s\n\tThe specified dynamic-link library (DLL) was not found.\r\n",
											(const char *)watchdog.m_targetPath);
										break;
									case SE_ERR_FNF:
										m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_DK_RED,
											"Error Opening File %s\n\tThe specified file was not found.\r\n",
											(const char *)watchdog.m_targetPath);
										break;
									case SE_ERR_NOASSOC:
										m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_DK_RED,
											"Error Opening File %s\n\tThere is no application associated with the given file name extension.\r\n",
											(const char *)watchdog.m_targetPath);
										break;
									case SE_ERR_OOM:
										m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_DK_RED,
											"Error Opening File %s\n\tThere was not enough memory to complete the operation.\r\n",
											(const char *)watchdog.m_targetPath);
										break;
									case SE_ERR_PNF:
										m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_DK_RED,
											"Error Opening File %s\n\tThe specified path was not found.\r\n",
											(const char *)watchdog.m_targetPath);
										break;
									case SE_ERR_SHARE:
										m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_DK_RED,
											"Error Opening File %s\n\tSharing violation.\r\n",
											(const char *)watchdog.m_targetPath);
										break;
								}
							}
							bRefresh = true;
						}
					}
				}
			}

			m_processManager.Unlock();
			Unlock();
		}
		else // we need to tell them all of the watchdog processes to go away
		{
			Unlock();
			PurgeWatchdogProcessThreads(false);
			break; // we are done...
		}
		if (bRefresh)
		{
			m_parentDialog.PostRefresh();
		}
		Sleep(1000);
	}
}

void WatchdogManager::PurgeWatchdogProcessThreads(bool bLeaveLocked)
{
	while(1)
	{
		int numProcesses = 0;
		Lock();
		int numWatchdogs = m_watchDogs.GetSize();
		for (int watchdogIndex=0;watchdogIndex<numWatchdogs;watchdogIndex++)
		{
			Watchdog &watchdog = m_watchDogs[watchdogIndex];

			int numWatchdogProcesses = watchdog.m_processes.GetSize();
			for (int watchdogProcessIndex=0;watchdogProcessIndex<numWatchdogProcesses;watchdogProcessIndex++)
			{
				watchdog.m_processes[watchdogProcessIndex].m_bEndThread = true;
				numProcesses++;
			}
		}
		if (numProcesses)
		{
			Unlock();
			Sleep(1000);
		}
		else
		{
			if (!bLeaveLocked)
				Unlock();
			break;
		}
	}
}


UINT WatchdogManager::UpdateWatchdogProcessProc(LPVOID pParam)
{
	WatchdogManager::ThreadData *pThreadData = (WatchdogManager::ThreadData *) pParam;
	WatchdogManager *pWatchdogManager = pThreadData->m_pWatchdogManager;
	DWORD processID = pThreadData->m_processID;
	delete pParam; // don't need this memory anymore
	pWatchdogManager->UpdateWatchdogProcessThread(processID);
	return 0;
}

// this thread is used for a specific process under a watchdog
void WatchdogManager::UpdateWatchdogProcessThread(DWORD processID)
{
	bool bCreationWaitMet = false;
	int watchdogIndex,processIndex;
	Watchdog watchdog;
	bool bOldWatchFileDateValid = false;
	FILETIME oldWatchFileDate;
	bool bValidOldWatchdogName = false;
	TupString oldWatchdogName;
	int status = Watchdog::Process::PROCESS_STATUS_CONNECTING;

	do
	{
		bool bFoundWatchdog = false;
		Lock(); // lock watchdogs
		if (FindWatchdogAndProcessByID(processID,watchdogIndex,processIndex))
		{
			if (!bValidOldWatchdogName)
			{
				oldWatchdogName = m_watchDogs[watchdogIndex].m_name;
				bValidOldWatchdogName = true;
			}
			// if the name of the watchdog changed or we are told to get out
			else if (oldWatchdogName!=m_watchDogs[watchdogIndex].m_name ||
						m_watchDogs[watchdogIndex].m_processes[processIndex].m_bEndThread)
			{
				m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_BLACK,
					"Disconnecing Watchdog %s from Process %d\r\n",
					(const char *)m_watchDogs[watchdogIndex].m_name,
					m_watchDogs[watchdogIndex].m_processes[processIndex].m_processID);

				// this thread is the only one that can remove this entry
				m_watchDogs[watchdogIndex].m_processes.RemoveAt(processIndex);
				Unlock();
				m_parentDialog.PostRefresh();
				return;
			}
			m_watchDogs[watchdogIndex].m_processes[processIndex].m_status = status;
			// copy to other object
			watchdog = m_watchDogs[watchdogIndex];
			bFoundWatchdog = true;
		}
		Unlock();

		// if we are no longer controlled by a watchdog exit
		if (!bFoundWatchdog)
		{
			ASSERT(FALSE); // this should not happen
			break; 
		}

		if (!bCreationWaitMet)
		{
			FILETIME currentFileTime;
			SYSTEMTIME currentSystemTime;

			::GetSystemTime(&currentSystemTime);		// gets current time
			::SystemTimeToFileTime(&currentSystemTime, &currentFileTime);	// converts to file time format

			if (CompareTimes(watchdog.m_processes[processIndex].m_creationTime,currentFileTime,watchdog.m_waitAfterCreation,0.0f)<0)
			{
				bCreationWaitMet = true;
			}
		}
		if (bCreationWaitMet)
		{
			HANDLE hProcess;
			hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_SET_INFORMATION, FALSE, processID); 
			if((hProcess != NULL) && (hProcess != INVALID_HANDLE_VALUE))
			{
				DWORD dwExitCode;
				BOOL bResult = ::GetExitCodeProcess(hProcess,&dwExitCode);
				// make sure the process is still running
				if (bResult && dwExitCode==STILL_ACTIVE)
				{
					bool bResponding = true;
					int waitForResponse = (int)(watchdog.m_waitForResponse*1000.0f);
					int numWindows = watchdog.m_processes[processIndex].m_topWindows.GetSize();
					for (int windowIndex=0;windowIndex<numWindows;windowIndex++)
					{
						HWND hwnd =  watchdog.m_processes[processIndex].m_topWindows[windowIndex];
						// make sure the window is still valid
						if (::IsWindow(hwnd))
						{
							DWORD dwResult;
							// see if the window is receiving any messages
							LRESULT lResult = SendMessageTimeout(hwnd,WM_NULL, 0, 0, SMTO_ABORTIFHUNG|SMTO_BLOCK, waitForResponse, &dwResult);
							if (!lResult)
							{
								DWORD lastError = GetLastError();
								if (lastError == 0 || lastError == ERROR_TIMEOUT) // this is a timeout
								{
									bResponding = false;
								}
								else
								{
									m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_DK_RED,
										"Error checking status of Process %s PID: %d.\r\nSendMessageTimeout error %d\r\n",
										(const char *)watchdog.m_name,processID,lastError);
									m_parentDialog.PostRefresh();
								}
							}
						}
					}
					if (bResponding)
					{
						status = Watchdog::Process::PROCESS_STATUS_ACTIVE;
						UpdateStatus(processID,status);

						switch (watchdog.m_flags & Watchdog::ACTIVE_STATE_MASK)
						{
						case Watchdog::ACTIVE_STATE_IGNORE:
							break;
						case Watchdog::ACTIVE_STATE_KILL:
							m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_BLACK,
								"Attempting to Kill Active Process %s PID: %d\r\n",
								(const char *)watchdog.m_name,processID);

							if (KillProcess(processID,watchdog.m_name,waitForResponse))
							{
								m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_BLACK,
									"Process %s PID: %d Closed successfully.\r\n",
									(const char *)watchdog.m_name,processID);
								m_parentDialog.PostRefresh();
								return;
							}
							else
							{
								m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_DK_RED,
									"Error Killing Process %s PID: %d.\r\n",
									(const char *)watchdog.m_name,processID);
								m_parentDialog.PostRefresh();
							}
							break;
						case Watchdog::ACTIVE_STATE_SET_PRIORITY:
							{
								int priorityToSet;
								switch (watchdog.m_priority)
								{
									case Watchdog::PRIORITY_REALTIME:
										priorityToSet = REALTIME_PRIORITY_CLASS;
										break;
									case Watchdog::PRIORITY_HIGH:
										priorityToSet = HIGH_PRIORITY_CLASS;
										break;
									case Watchdog::PRIORITY_ABOVE_NORMAL:
										priorityToSet = ABOVE_NORMAL_PRIORITY_CLASS;
										break;
									case Watchdog::PRIORITY_NORMAL:
										priorityToSet = NORMAL_PRIORITY_CLASS;
										break;
									case Watchdog::PRIORITY_BELOW_NORMAL:
										priorityToSet = BELOW_NORMAL_PRIORITY_CLASS;
										break;
									case Watchdog::PRIORITY_LOW:
										priorityToSet = IDLE_PRIORITY_CLASS;
										break;
								}

								int previousPriority = ::GetPriorityClass(hProcess);
								if (priorityToSet!=previousPriority)
								{
									::SetPriorityClass(hProcess,priorityToSet);

									switch (watchdog.m_priority)
									{
									case Watchdog::PRIORITY_REALTIME:
										m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_BLACK,
											"Process %s PID: %d Priority set to Realtime.\r\n",
											(const char *)watchdog.m_name,processID);
										break;
									case Watchdog::PRIORITY_HIGH:
										m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_BLACK,
											"Process %s PID: %d Priority set to High.\r\n",
											(const char *)watchdog.m_name,processID);
										break;
									case Watchdog::PRIORITY_ABOVE_NORMAL:
										m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_BLACK,
											"Process %s PID: %d Priority set to Above Normal.\r\n",
											(const char *)watchdog.m_name,processID);
										break;
									case Watchdog::PRIORITY_NORMAL:
										m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_BLACK,
											"Process %s PID: %d Priority set to Normal.\r\n",
											(const char *)watchdog.m_name,processID);
										break;
									case Watchdog::PRIORITY_BELOW_NORMAL:
										m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_BLACK,
											"Process %s PID: %d Priority set to Below Normal.\r\n",
											(const char *)watchdog.m_name,processID);
										break;
									case Watchdog::PRIORITY_LOW:
										m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_BLACK,
											"Process %s PID: %d Priority set to Low.\r\n",
											(const char *)watchdog.m_name,processID);
										break;
									}
									m_parentDialog.PostRefresh();
								}
							}
						}
					}
					else // not responding
					{
						status = Watchdog::Process::PROCESS_STATUS_NOT_RESPONDING;
						UpdateStatus(processID,status);

						switch (watchdog.m_flags & Watchdog::NOT_RESPONDING_STATE_MASK)
						{
						case Watchdog::NOT_RESPONDING_STATE_IGNORE:
							break;
						case Watchdog::NOT_RESPONDING_STATE_KILL:
							m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_BLACK,
								"Attempting to Kill Not Responding Process %s PID: %d.\r\n",
								(const char *)watchdog.m_name,processID);
							m_parentDialog.PostRefresh();
							if (KillProcess(processID,watchdog.m_name,waitForResponse))
							{
								m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_BLACK,
									"Process %s PID: %d Closed successfully.\r\n",
									(const char *)watchdog.m_name,processID);
								m_parentDialog.PostRefresh();
								return;
							}
							else
							{
								m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_DK_RED,
									"Error Killing Process %s PID: %d.\r\n",
									(const char *)watchdog.m_name,processID);
								m_parentDialog.PostRefresh();
							}
						}
					}
					::CloseHandle(hProcess);
				}
			}

			// check watchfile
			if ((watchdog.m_flags & Watchdog::WATCH_FILE_MASK) == Watchdog::WATCH_FILE_KILL &&
				  watchdog.m_watchFilePath.GetLength())
			{
				HANDLE hFile = ::CreateFile(watchdog.m_watchFilePath,  // open path
						0,										// access only
						FILE_SHARE_READ,					// share for reading 
						NULL,									// no security 
						OPEN_EXISTING,						// existing file only 
						FILE_ATTRIBUTE_NORMAL,			// normal file 
						NULL);								// no attr. template 

				if (hFile != INVALID_HANDLE_VALUE) 
				{
					bool bTimesMatch = true;
					FILETIME currentFileTime;
					if (::GetFileTime(hFile,(LPFILETIME) NULL,(LPFILETIME) NULL,&currentFileTime))
					{
						if (!bOldWatchFileDateValid)
						{
							oldWatchFileDate = currentFileTime;
							bOldWatchFileDateValid = true;
						}
						else
						{
							if (CompareTimes(oldWatchFileDate,currentFileTime,0.0f,0.0f)!=0)
								bTimesMatch = false;
						}
					}
					CloseHandle(hFile); 
					int waitForResponse = (int)(watchdog.m_waitForResponse*1000.0f);

					if (!bTimesMatch)
					{
						m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_BLACK,
							"Attempting to Kill Process %s PID: %d due to watch file %s.\r\n",
							(const char *)watchdog.m_name,processID,
							(const char *)watchdog.m_watchFilePath);
						m_parentDialog.PostRefresh();
						if (KillProcess(processID,watchdog.m_name,waitForResponse))
						{
							m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_BLACK,
								"Process %s PID: %d Closed successfully.\r\n",
								(const char *)watchdog.m_name,processID);
							m_parentDialog.PostRefresh();
							return;
						}
						else
						{
							m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_DK_RED,
								"Error Killing Process %s PID: %d.\r\n",
								(const char *)watchdog.m_name,processID);
							m_parentDialog.PostRefresh();
						}
					}
				}
			}
		}
		else
		{
			status = Watchdog::Process::PROCESS_STATUS_WAITING_AFTER_CREATION;
		}
		UpdateStatus(processID,status);

		int sleepValue = (int)(watchdog.m_checkInterval*1000.0f);
		if (sleepValue<1) sleepValue = 1;

		Sleep(sleepValue);

	} while(1);
}

void WatchdogManager::UpdateStatus(DWORD processID,int status)
{
	int oldStatus = status;
	int watchdogIndex,processIndex;

	Lock(); // lock watchdogs
	if (FindWatchdogAndProcessByID(processID,watchdogIndex,processIndex))
	{
		oldStatus = m_watchDogs[watchdogIndex].m_processes[processIndex].m_status;
		if (status!=oldStatus)
		{
			m_watchDogs[watchdogIndex].m_processes[processIndex].m_status = status;
		}
	}
	Unlock();
	if (oldStatus!=status)
		m_parentDialog.PostRefresh();
}

class SendMessageData
{
public:
	DWORD m_processID;
	int m_timeoutValue;
	UINT m_message;
	WPARAM m_wparam;
	LPARAM m_lparam;
};

BOOL CALLBACK WatchdogManager::SendMessageTimeoutEnum( HWND hwnd, LPARAM lParam )
{
	DWORD dwID ;
	SendMessageData *pSendMessageData = (SendMessageData *)lParam;

	::GetWindowThreadProcessId(hwnd, &dwID) ;

	if(dwID == pSendMessageData->m_processID)
	{
		DWORD dwResult;
		LRESULT lResult = SendMessageTimeout(
			hwnd,
			pSendMessageData->m_message,
			pSendMessageData->m_wparam,
			pSendMessageData->m_lparam,
			SMTO_ABORTIFHUNG|SMTO_BLOCK,
			pSendMessageData->m_timeoutValue,
			&dwResult);
	}
	return TRUE ;
}


bool WatchdogManager::KillProcess(DWORD processID,const char *pName,int waitForResponse)
{
	HANDLE hProcess;
	bool bProcessKilled;

	// If we can't open the process with PROCESS_TERMINATE rights,
	// then we give up immediately.
	hProcess = ::OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, FALSE,	processID);

	if(hProcess == NULL)
	{
		return false;
	}
	SendMessageData messageData;
	messageData.m_processID = processID;
	messageData.m_timeoutValue = waitForResponse;
	messageData.m_wparam = SC_CLOSE;
	messageData.m_lparam = 0;

	messageData.m_message = WM_SYSCOMMAND;

	// send a syscommand with a close to tell the app to close (this may not work)
	EnumWindows((WNDENUMPROC)SendMessageTimeoutEnum, (LPARAM) &messageData) ;

	// Wait on the handle. If it signals, great. If it times out,
	// then send a endsession message
	if(WaitForSingleObject(hProcess, waitForResponse)!=WAIT_OBJECT_0)
	{
		messageData.m_wparam = 0;
		messageData.m_lparam = 0;

		messageData.m_message = WM_ENDSESSION;

		// send a end session command (this should work except when the app is frozen or crashed)
		EnumWindows((WNDENUMPROC)SendMessageTimeoutEnum, (LPARAM) &messageData) ;
		if(WaitForSingleObject(hProcess, waitForResponse)!=WAIT_OBJECT_0)
		{
			bProcessKilled = (TerminateProcess(hProcess,0) ? true : false);
			if (!bProcessKilled)
			{
				DWORD lastError = GetLastError();
				m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_DK_RED,
					"Error Terminating process %s PID: %d.\r\nTerminateProcess error %d\r\n",
					pName,processID,lastError);
				m_parentDialog.PostRefresh();
			}
			else
			{
				m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_BLACK,
					"Process %s PID: %d Closed successfully using TerminateProcess.\r\n",
					pName,processID);
			}
		}
		else
		{
			bProcessKilled = true;
			m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_BLACK,
				"Process %s PID: %d Closed successfully using EndSession Message.\r\n",
				pName,processID);
		}
	}
	else
	{
		bProcessKilled = true;
		m_parentDialog.AddToLog(CProcessWatchDogDlg::LogFlags::FONT_COLOR_BLACK,
			"Process %s PID: %d Closed successfully using Close Message.\r\n",
			pName,processID);
	}

	CloseHandle(hProcess);

	if (bProcessKilled)
	{
		Lock(); // lock watchdogs
		int watchdogIndex,processIndex;
		if (FindWatchdogAndProcessByID(processID,watchdogIndex,processIndex))
		{
			// delete entry from process array on watchdog
			m_watchDogs[watchdogIndex].m_processes.RemoveAt(processIndex);
		}
		Unlock();
	}

	return bProcessKilled;
}

bool WatchdogManager::Load(const char *pFilename)
{
	TupperwareAggregate *pTupRoot;
	int result = TupperwareLoadSaveManager::Read(pFilename,&pTupRoot);
	if (result)
	{
		assert(pTupRoot);
		TupperwareAggregate *pWatchdogsAgg = pTupRoot->FindAggregateByKey("Watchdogs");
		assert(pWatchdogsAgg);

		m_watchDogs.RemoveAll();
		TupperwarePoolValueString tupPool(pWatchdogsAgg,"Watchdog");
		int numWatchdogs = tupPool.GetPoolSize();
		for (int watchdogIndex=0;watchdogIndex<numWatchdogs;watchdogIndex++)
		{
			TupperwareAggregate *pWatchdogAgg = tupPool.FindAggregate(watchdogIndex);
			if (!pWatchdogAgg)
				continue;
			Watchdog newWatchdog;

			newWatchdog.m_name = pWatchdogAgg->FindScalarByKey("Name")->GetAsString();
			newWatchdog.m_targetPath = pWatchdogAgg->FindScalarByKey("TargetPath")->GetAsString();
			newWatchdog.m_parameters = pWatchdogAgg->FindScalarByKey("Parameters")->GetAsString();
			newWatchdog.m_startInPath = pWatchdogAgg->FindScalarByKey("StartInPath")->GetAsString();
			newWatchdog.m_watchFilePath = pWatchdogAgg->FindScalarByKey("WatchFilePath")->GetAsString();
			newWatchdog.m_checkInterval = pWatchdogAgg->FindScalarByKey("CheckInterval")->GetAsFloat();
			newWatchdog.m_waitForResponse = pWatchdogAgg->FindScalarByKey("WaitForResponse")->GetAsFloat();
			newWatchdog.m_waitAfterCreation = pWatchdogAgg->FindScalarByKey("WaitAfterCreation")->GetAsFloat();
			newWatchdog.m_waitForNoProcesses = pWatchdogAgg->FindScalarByKey("WaitForNoProcesses")->GetAsFloat();
			newWatchdog.m_flags = pWatchdogAgg->FindScalarByKey("Flags")->GetAsInt();
			newWatchdog.m_priority = pWatchdogAgg->FindScalarByKey("Priority")->GetAsInt();
			newWatchdog.m_runType = pWatchdogAgg->FindScalarByKey("RunType")->GetAsInt();

			SYSTEMTIME currentSystemTime;
			::GetSystemTime(&currentSystemTime);		// gets current time
			::SystemTimeToFileTime(&currentSystemTime,&newWatchdog.m_noProcessTime);	// converts to file time format

			m_watchDogs.Add(newWatchdog);
		}
		delete pTupRoot;
	}
	return (result!=0);
}

bool WatchdogManager::Save(const char *pFilename)
{
	TupperwareAggregate *pTupRoot = new TupperwareAggregate;

	TupperwareAggregate *pWatchdogsAgg = pTupRoot->AddAggregate("Watchdogs",NULL);
	TupperwarePoolValueString tupPool(pWatchdogsAgg,"Watchdog");
	int numWatchdogs = m_watchDogs.GetSize();
	for (int watchdogIndex=0;watchdogIndex<numWatchdogs;watchdogIndex++)
	{
		TupperwareAggregate *pWatchdogAgg = tupPool.AddAggregate(watchdogIndex);
		Watchdog &watchdog = m_watchDogs[watchdogIndex];

		pWatchdogAgg->AddScalarString("Name",NULL,watchdog.m_name);
		pWatchdogAgg->AddScalarString("TargetPath",NULL,watchdog.m_targetPath);
		pWatchdogAgg->AddScalarString("Parameters",NULL,watchdog.m_parameters);
		pWatchdogAgg->AddScalarString("StartInPath",NULL,watchdog.m_startInPath);
		pWatchdogAgg->AddScalarString("WatchFilePath",NULL,watchdog.m_watchFilePath);
		pWatchdogAgg->AddScalarFloat("CheckInterval",NULL,watchdog.m_checkInterval);
		pWatchdogAgg->AddScalarFloat("WaitForResponse",NULL,watchdog.m_waitForResponse);
		pWatchdogAgg->AddScalarFloat("WaitAfterCreation",NULL,watchdog.m_waitAfterCreation);
		pWatchdogAgg->AddScalarFloat("WaitForNoProcesses",NULL,watchdog.m_waitForNoProcesses);
		pWatchdogAgg->AddScalarInt("Flags",NULL,watchdog.m_flags);
		pWatchdogAgg->AddScalarInt("Priority",NULL,watchdog.m_priority);
		pWatchdogAgg->AddScalarInt("RunType",NULL,watchdog.m_runType);
	}

	int result = TupperwareLoadSaveManager::Write(TupperwareLoadSaveManager::ASCII,pFilename,pTupRoot);
	delete pTupRoot;

	return (result!=0);
}
