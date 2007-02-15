//------------------------------------------------------------------------------
// PatchCommandThread_ps2.cpp
//
// Platform-specific thread and message handling.
//
// Copyright (c) 2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

#include "PatchCommandThread.h"

#define PATCHER_STACK_SIZE 0x2000
static char s_PatcherStack[PATCHER_STACK_SIZE] __attribute__ ((aligned(16)));

//------------------------------------------------------------------------------
PatchCommandThread::PatchCommandThread()
	: m_threadHandle(-2)
	, m_semaHandle(-1)
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
	DelayThread(delay_ms * 1000);
}

//------------------------------------------------------------------------------
// Start the thread with the given thread function and argument
bool PatchCommandThread::Start(void func(void*), void* fparam, int priority)
{
	// Create the thread
	// Set up the information for starting the worker thread
	struct ThreadParam tparam;
	tparam.status = 0;						// Current status (not needed)
	tparam.entry = func;					// Address of entry function
	tparam.stack = s_PatcherStack;			// Address of thread stack
	tparam.stackSize = PATCHER_STACK_SIZE;	// Size of stack in bytes
	tparam.gpReg = &_gp;					// Address of global variable base pointer
	tparam.initPriority = priority;			// Initial priority
	tparam.currentPriority = 0;				// Current priority (not needed)
	tparam.attr = 0;						// System reserved (not needed)
	tparam.option = 0;						// System reserved (not needed)
	tparam.waitType = 0;					// Wait type (not needed)
	tparam.waitId = 0;						// Semaphore ID if wait type indicates semaphore (not needed)
	tparam.wakeupCount = 0;					// Wake up count (not needed)

	// Create the thread
	m_threadHandle = CreateThread(&tparam);
	if (m_threadHandle < 0)
		goto InitFailure;

	// Create a semaphore for sending messages to the thread
	struct SemaParam sParam;
	sParam.currentCount = 0;	// Not needed
	sParam.maxCount = 1;
	sParam.initCount = 0;
	sParam.numWaitThreads = 0;	// Not needed
	sParam.attr = 0;			// Not needed
	sParam.option = 0;			// Not needed

	m_semaHandle = CreateSema(&sParam);
	if (m_semaHandle < 0)
		goto InitFailure;

	// The thread and semaphores were all created successfully, so start
	// the worker thread.
	StartThread(m_threadHandle, fparam);

	return true;

InitFailure:
	if (m_semaHandle >= 0)
		DeleteSema(m_semaHandle);
	if (m_threadHandle >= 0)
		DeleteThread(m_threadHandle);
	m_threadHandle = -1;	// Don't try to restart
	m_semaHandle = -1;
	return false;
}

//------------------------------------------------------------------------------
// Kill the thread if it is still running
void PatchCommandThread::Kill(void)
{
	if (m_threadHandle >= 0)
	{
		TerminateThread(m_threadHandle);
		DeleteThread(m_threadHandle);
		m_threadHandle = -1;
	}
	if (m_semaHandle >= 0)
	{
		DeleteSema(m_semaHandle);
		m_semaHandle = -1;
	}
}

//------------------------------------------------------------------------------
// Send a signal to the thread that a message is ready
void PatchCommandThread::SignalMessage(void)
{
	SignalSema(m_semaHandle);
}

//------------------------------------------------------------------------------
// Work thread: wait for a signal to arrive
bool PatchCommandThread::WaitMessage(void)
{
	return (WaitSema(m_semaHandle) == m_semaHandle);
}

//------------------------------------------------------------------------------
// Main thread: Wait for the work thread to exit (assuming a quit message was sent)
// For PS2: ReferThreadStatus is recommended against in the manual, and there is no
// apparent way to wait for a thread to complete. Just do nothing, and let the
// thread get killed with TerminateThread later.
void PatchCommandThread::WaitComplete(int timeout_ms)
{
}
