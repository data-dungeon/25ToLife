//--------------------------------------------------------------//
// Thread.h
//
// Cross platform Thread API
// Author: Paul Blagay
// Date:   06/08/04 
//
// Note: This is the simple functions necessary to create and
// execute a thread. Semaphores and messaging havent been added
// yet. If this is needed please add or let me know what you
// need. Currently PS2/PC/XBOX are supported. 
//--------------------------------------------------------------//

#include "Layers/LayersPCH.h"
#include "thread.h"

//--------------------------------------------------------------//
// Thread Priorities (platform specific)
#if defined (WIN32)
int	ThreadPriorities[] = { 	THREAD_PRIORITY_LOWEST,					// -2		// CTHREAD_PRIORITY_LOWEST,
										THREAD_PRIORITY_BELOW_NORMAL,			// -1		// CTHREAD_PRIORITY_BELOW_NORMAL,
										THREAD_PRIORITY_NORMAL,					// 0		// CTHREAD_PRIORITY_NORMAL,
										THREAD_PRIORITY_ABOVE_NORMAL,			// 1		// CTHREAD_PRIORITY_ABOVE_NORMAL,
										THREAD_PRIORITY_HIGHEST,				// 2		// CTHREAD_PRIORITY_HIGHEST,
										THREAD_PRIORITY_TIME_CRITICAL,		// 15		// CTHREAD_PRIORITY_TIME_CRITICAL,
										THREAD_PRIORITY_IDLE,					// -15	// CTHREAD_PRIORITY_IDLE,
};

// PS2 uses priorities 1-127 (so we lets make arbitrary vals)
// assuming 127 is lowest pri
#elif defined (PS2)			
int	ThreadPriorities[] = { 
										90,	// CTHREAD_PRIORITY_LOWEST,
										60,	// CTHREAD_PRIORITY_BELOW_NORMAL,
										 9,	// CTHREAD_PRIORITY_NORMAL
										30,	// CTHREAD_PRIORITY_ABOVE_NORMAL,
										1,		// CTHREAD_PRIORITY_HIGHEST,
										0,		// CTHREAD_PRIORITY_TIME_CRITICAL,
										0,		// CTHREAD_PRIORITY_IDLE,
};
#endif

//--------------------------------------------------------------//

CThread::CThread()
{
	Handle = 0;
}

//--------------------------------------------------------------//

CThread::~CThread()
{
}

//--------------------------------------------------------------//
// Default stack on PS2      = 4K
// Default stack on WIN32    = 64K
// Defauly Priority          = NORMAL
//
#if defined (PS2)
bool CThread::Create(void ThreadFunction(void*), int stacksize, int Priority, int arg)
#elif defined (WIN32)
bool CThread::Create(int* ThreadFunction, int stacksize, int Priority, int arg)
#endif
{
	#if defined (PS2)
	m_Arg = arg;
	m_ThreadParams.entry = ThreadFunction;
	m_ThreadParams.stack = m_ThreadStack;
	m_ThreadParams.stackSize = sizeof(m_ThreadStack);
	memset(m_ThreadParams.stack, 0xab, m_ThreadParams.stackSize);
	m_ThreadParams.gpReg = &_gp;					
	m_ThreadParams.initPriority = ThreadPriorities[Priority];
	m_ThreadParams.option = 0;													// not used
	Handle = CreateThread(&m_ThreadParams);

	if (Handle == -1)
		return false;

	#elif defined (WIN32)
	m_Arg = arg;
	DWORD dwThreadId, dwThrdParam = 1; 
	Handle = CreateThread(NULL, stacksize, (LPTHREAD_START_ROUTINE)ThreadFunction, &arg, 0, &dwThreadId);
	if (!Handle)
		return false;
	SetThreadAffinityMask(Handle, 1<<0);
	BOOL result = SetThreadPriority(Handle, ThreadPriorities[Priority]);
	if (!result)
		return false;

	if (!Stop())
		return false;

	return true;
	#endif
}

//--------------------------------------------------------------//
bool CThread::Destroy()
{
	#if defined (PS2)
	int result = DeleteThread(Handle);
	if (result == -1)
		return false;
	#elif defined (WIN32)
	BOOL result = CloseHandle(Handle);
	if (!result)
		return false;
	#endif

	return true;
	Handle = 0;
}

//--------------------------------------------------------------//
void CThread::SleepTime (int time)
{
	#if defined (PS2)
	SleepThread();
	#elif defined (WIN32)
	for (int i=0; i<time; i++)
		DWORD  result = SuspendThread(Handle);
	#endif
}

//--------------------------------------------------------------//
void CThread::YieldThread(int time)
{
	#if defined (PS2)
	DelayThread(time * 1000);
	#elif defined (WIN32)
	Sleep(time);
	#endif
}

//--------------------------------------------------------------//
int CThread::GetStatus()
{
	#if defined (PS2)
	return (m_ThreadParams.status);
	#elif defined (WIN32)
	DWORD	ExitCode;
	BOOL result = GetExitCodeThread (Handle, &ExitCode);
	return (ExitCode);
	#endif
}
//--------------------------------------------------------------//
int CThread::GetWaitStatus()
{
	#if defined (PS2)
	return (m_ThreadParams.waitType);
	#elif defined (WIN32)
	return (0);
	#endif
}

//--------------------------------------------------------------//
void CThread::Exit()
{
	#if defined (PS2)
	ExitThread();
	#elif defined (WIN32)
	DWORD	ExitCode = 0;
	ExitThread(ExitCode);
	#endif
}
//--------------------------------------------------------------//
void CThread::ExitDelete()
{
	#if defined (PS2)
	ExitDeleteThread();
	#elif defined (WIN32)
	DWORD	ExitCode = 0;
	GetExitCodeThread(Handle, &ExitCode);
	ExitThread(ExitCode);
	#endif
}
//--------------------------------------------------------------//
bool CThread::Start()
{
	#if defined (PS2)
	int result = StartThread(Handle, &m_Arg); 
	#elif defined (WIN32)
	DWORD	result = ResumeThread(Handle);
	#endif

	if (result == -1)
		return false;

	return true;
}
//--------------------------------------------------------------//
bool CThread::Stop()
{
	#if defined (PS2)
	int result = SuspendThread(Handle); 
	#elif defined (WIN32)
	DWORD	result = SuspendThread(Handle);
	#endif

	if (result == -1)
		return false;

	return true;
}
//--------------------------------------------------------------//
bool CThread::SetPriority(int Priority)
{
	#if defined (PS2)
	int result = ChangeThreadPriority(Handle, ThreadPriorities[Priority]);
	if (result == -1)
		return false;
	#elif defined (WIN32)
	BOOL result = SetThreadPriority(Handle, ThreadPriorities[Priority]);
	if (!result)
		return false;
	#endif

	return true;
}
//--------------------------------------------------------------//
int CThread::GetPriority()
{
	#if defined (PS2)
	return (m_ThreadParams.currentPriority);
	#elif defined (WIN32)
	int pri = GetThreadPriority(Handle);
	if (pri == THREAD_PRIORITY_ERROR_RETURN)
		return -1;
	else
		return pri;
	#endif
}
//--------------------------------------------------------------//
bool CThread::Execute()
{
	#if defined (PS2)
	int result = ReleaseWaitThread(Handle);
	if (result == -1)
		return false;
	#elif defined (WIN32)
	DWORD	count = 0;
	while (count != 0)
	{
		count = ResumeThread(Handle);
		if (count == -1)
			return false;
	}
	Sleep(1);												// system sleep, will execute a thread on PC
	#endif

	return true;
}
//--------------------------------------------------------------//
//--------------------------------------------------------------//
//--------------------------------------------------------------//
//--------------------------------------------------------------//


