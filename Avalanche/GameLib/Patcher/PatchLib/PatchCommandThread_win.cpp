//------------------------------------------------------------------------------
// PatchCommandThread_win.cpp
//
// Platform-specific thread and message handling.
//
// Copyright (c) 2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

#include <windows.h>
#include "PatchCommandThread.h"

//------------------------------------------------------------------------------
// Static function to use as the Windows thread proc, that just calls our thread
// proc. Only difference between ours and windows is that windows uses a return 
// value.
DWORD WINAPI MyThreadProc(LPVOID lpParameter)
{
	PatchCommandThread* pct = (PatchCommandThread*)lpParameter;
	pct->m_tfunc(pct->m_tfuncParam);
	return 0;
}


//------------------------------------------------------------------------------
PatchCommandThread::PatchCommandThread()
	: m_threadHandle(NULL)
	, m_semaHandle(NULL)
	, m_tfunc(NULL)
	, m_tfuncParam(NULL)
{
}

//------------------------------------------------------------------------------
PatchCommandThread::~PatchCommandThread()
{
	Kill();
}

//------------------------------------------------------------------------------
// Delay the currently running thread or otherwise kick the patch thread so it can run
void PatchCommandThread::Delay(int delay_ms)
{
	Sleep(delay_ms);
}

//------------------------------------------------------------------------------
// Start the thread with the given thread function and argument
bool PatchCommandThread::Start(PatchThreadFunc* func, void* fparam, int priority)
{
	m_semaHandle = CreateSemaphore(NULL, 0, 1, NULL);
	if (m_semaHandle == NULL)
		return false;

	// Create and start the thread
	DWORD threadID;
	m_tfunc = func;
	m_tfuncParam = fparam;
	m_threadHandle = CreateThread(NULL, 0, MyThreadProc, this, 0, &threadID);
	if (m_threadHandle == NULL)
	{
		Kill();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
// Kill the thread if it is still running
void PatchCommandThread::Kill(void)
{
	if (m_threadHandle)
	{
		TerminateThread(m_threadHandle, 1);
		m_threadHandle = NULL;
	}
	if (m_semaHandle)
	{
		CloseHandle(m_semaHandle);
		m_semaHandle = NULL;
	}
}

//------------------------------------------------------------------------------
// Main thread: Send a signal to the thread that a message is ready
void PatchCommandThread::SignalMessage(void)
{
	ReleaseSemaphore(m_semaHandle, 1, NULL);
}

//------------------------------------------------------------------------------
// Work thread: wait for a signal to arrive
bool PatchCommandThread::WaitMessage(void)
{
	return (WaitForSingleObject(m_semaHandle, INFINITE) == WAIT_OBJECT_0);
}

//------------------------------------------------------------------------------
// Main thread: Wait for the work thread to exit (assuming a quit message was sent)
void PatchCommandThread::WaitComplete(int timeout_ms)
{
	if (m_threadHandle)
	{
		if (WaitForSingleObject(m_threadHandle, (DWORD)timeout_ms) != WAIT_TIMEOUT)
			m_threadHandle = NULL;
	}
}
