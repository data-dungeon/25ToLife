////////////////////////////////////////////////////////////////////////////
//
// ProcessList
//
// Class to query the current processes in the system
//
////////////////////////////////////////////////////////////////////////////

/*
* $History: ProcessList.h $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 10/03/03   Time: 12:06p
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * more robust kill and check if responding code
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 10/02/03   Time: 12:37p
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * looks like we are here!
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 9/26/03    Time: 3:11p
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * watchdog threads
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/24/03    Time: 3:23p
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * updating of watchdogs
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/22/03    Time: 3:37p
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * watchdog property page
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 9/18/03    Time: 10:34a
 * Created in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchdog
*/

#ifndef PROCESS_WATCHDOG_PROCESS_LIST_H
#define PROCESS_WATCHDOG_PROCESS_LIST_H

class WatchdogManager;

class ProcessManager
{
public:
	class ProcessData
	{
	public:
		DWORD m_processID;
		TupString m_processName;
		int m_priorityClassBase;
		FILETIME m_creationTime;
		int m_watchdogProcessStatus;
		TupArray<HWND> m_topWindows;
	};

	ProcessManager(void);

	void Lock(void);
	void Unlock(void);

	void RefreshProcesses(WatchdogManager *pWatchdogManager);
	int GetNumProcesses(void);
	const ProcessData &GetProcess(int index) const;
	ProcessData &GetProcess(int index);
	void AddProcessHWND(DWORD processID,HWND hwnd);

	void UpdateOld(void);
	bool CompareToOld(void) const;

private:
	TupArray<ProcessData> m_processes;
	TupArray<ProcessData> m_oldProcesses;

	CCriticalSection m_criticalSection;
};

#endif

