////////////////////////////////////////////////////////////////////////////
//
// Watchdog
//
// Class hold information about a watchdog
//
////////////////////////////////////////////////////////////////////////////

/*
* $History: Watchdog.h $
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
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 9/30/03    Time: 3:59p
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * Load/save - Just need to work on the refresh and the log syncronization
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 9/29/03    Time: 2:44p
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * starting applications - killing processes
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/26/03    Time: 3:11p
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * watchdog threads
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/24/03    Time: 3:23p
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * updating of watchdogs
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 9/22/03    Time: 3:37p
 * Created in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * watchdog property page
*/

#ifndef PROCESS_WATCHDOG_WATCHDOG_H
#define PROCESS_WATCHDOG_WATCHDOG_H

class CProcessWatchDogDlg;
class ProcessManager;

class Watchdog
{
public:
	class Process
	{
	public:
		enum Status
		{
			PROCESS_STATUS_NOT_CONNECTED,
			PROCESS_STATUS_CONNECTING,
			PROCESS_STATUS_ACTIVE,
			PROCESS_STATUS_WAITING_AFTER_CREATION,
			PROCESS_STATUS_NOT_RESPONDING,
		};
		DWORD m_processID;
		FILETIME m_creationTime;
		TupArray<HWND> m_topWindows;
		int m_status;
		CWinThread *m_pThread;
		bool m_bEndThread;
	};

	enum WatchdogFlags
	{
		ACTIVE_STATE_MASK						= 3<<0,
		ACTIVE_STATE_IGNORE					= 0<<0,
		ACTIVE_STATE_KILL						= 1<<0,
		ACTIVE_STATE_SET_PRIORITY			= 2<<0,

		NO_PROCESSES_STATE_MASK				= 3<<2,
		NO_PROCESSES_STATE_IGNORE			= 0<<2,
		NO_PROCESSES_STATE_OPEN				= 1<<2,

		NOT_RESPONDING_STATE_MASK			= 3<<4,
		NOT_RESPONDING_STATE_IGNORE		= 0<<4,
		NOT_RESPONDING_STATE_KILL			= 1<<4,

		WATCH_FILE_MASK						= 3<<6,
		WATCH_FILE_IGNORE						= 0<<6,
		WATCH_FILE_KILL						= 1<<6,
	};

	enum Priorities
	{
		PRIORITY_REALTIME,
		PRIORITY_HIGH,
		PRIORITY_ABOVE_NORMAL,
		PRIORITY_NORMAL,
		PRIORITY_BELOW_NORMAL,
		PRIORITY_LOW
	};

	enum RunType
	{
		RUN_TYPE_NORMAL_WINDOW,
		RUN_TYPE_MINIMIZED,
		RUN_TYPE_MAXIMIZED,
	};

	TupString m_name;
	TupString m_targetPath;
	TupString m_parameters;
	TupString m_startInPath;
	TupString m_watchFilePath;
	float m_checkInterval;
	float m_waitForResponse;
	float m_waitAfterCreation;
	float m_waitForNoProcesses;
	int m_flags;
	int m_priority;
	int m_runType;

	FILETIME m_noProcessTime;
	
	TupArray<Process> m_processes;
};

class WatchdogManager
{
	CProcessWatchDogDlg &m_parentDialog;
	ProcessManager &m_processManager;

	TupArray<Watchdog> m_watchDogs;
	CCriticalSection m_criticalSection;
	int m_lockCount;

	CWinThread *m_pManagerThread;

	bool m_bEndManagerThread;
	bool m_bManagerAddsThreads;

public:

	class ThreadData
	{
		public:
			WatchdogManager *m_pWatchdogManager;
			DWORD m_processID;
	};

	WatchdogManager(CProcessWatchDogDlg &parentDialog,ProcessManager &processManager);
	~WatchdogManager();

	void Lock(void);
	void Unlock(void);

	void EnableAddThreads(void);
	void DisableAddThreads(void);

	int GetNumWatchdogs(void) const;
	const Watchdog &GetWatchdog(int index) const;
	Watchdog &GetWatchdog(int index);

	void AddWatchdog(const Watchdog &watchdog);
	void DeleteWatchdog(int index);

	int FindWatchdogByName(const char *pName) const;
	bool FindWatchdogAndProcessByID(DWORD processID,int &watchdogIndex,int &processIndex);

	static UINT UpdateWatchdogProc(LPVOID pParam);
	void UpdateWatchdogThread(void);

	static UINT UpdateWatchdogProcessProc(LPVOID pParam);
	void UpdateWatchdogProcessThread(DWORD processID);

	void UpdateStatus(DWORD processID,int status);

	static BOOL CALLBACK SendMessageTimeoutEnum( HWND hwnd, LPARAM lParam );
	bool KillProcess(DWORD processID,const char *pName,int waitForResponse);

	bool Load(const char *pFilename);
	bool Save(const char *pFilename);

	void PurgeWatchdogProcessThreads(bool bLeaveLocked);

	static TupString StripExtensionFromString(const TupString &string);

};

#endif

