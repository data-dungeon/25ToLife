//------------------------------------------------------------------------------
// PatchCommandThread.h
//
// Class declaration for platform-specific thread and message handling.
//
// Copyright (c) 2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------
#ifndef __PATCHCOMMANDTHREAD_H__
#define __PATCHCOMMANDTHREAD_H__ 1

#include "PatcherCommon/patchplatform.h"

typedef void PatchThreadFunc(void*);

#ifdef PATCH_COMPILER_SN_PS2
	#include <eekernel.h>
	typedef int PCT_ThreadHandle;
	typedef int PCT_SemaHandle;
#else
	#include <windows.h>
	typedef HANDLE PCT_ThreadHandle;
	typedef HANDLE PCT_SemaHandle;
#endif

class PatchCommandThread {
public:
	PatchCommandThread();
	~PatchCommandThread();

	// Main thread: Delay the currently running thread or otherwise kick the work thread so it can run
	void Delay(int delay_ms);

	// Main thread: Start the work thread with the given thread function and argument
	bool Start(PatchThreadFunc* func, void* handle, int priority);

	// Main thread: Kill the work thread if it is still running
	void Kill(void);

	// Main thread: Send a signal to the work thread that a message is ready
	void SignalMessage(void);

	// Work thread: wait for a signal to arrive
	bool WaitMessage(void);

	// Main thread: Wait for the work thread to exit (assuming a quit message was sent)
	void WaitComplete(int timeout_ms);

	// Extra public members for changing around the thread function for windows
#ifdef PATCH_COMPILER_MSVC_W32
	PatchThreadFunc*	m_tfunc;
	void*				m_tfuncParam;
#endif

private:
	PCT_ThreadHandle	m_threadHandle;
	PCT_SemaHandle		m_semaHandle;
};

#endif //__PATCHCOMMANDTHREAD_H__