///////////////////////////////////////////////////////////////////////////////
// This is a simple implementation of a platform independent thread
///////////////////////////////////////////////////////////////////////////////
#ifndef OSTHREAD_H
#define OSTHREAD_H

class OSThread
{
public:
	// this should not make any os calls
	OSThread();

	// Don't delete unless you KIll(true)
	virtual ~OSThread();

	// The priorities
	enum Priority
	{
		VERY_LOW,
		LOW,
		NORMAL,
		HIGH,
		VERY_HIGH
	};

	// start this thread up.
	bool Start(Priority pri);

	// pause the starting thread until this thread terminates.
	void WaitForExit();

	// kill this thread (from the starting thread)
	void Kill(bool wait = false);

	// see if this thread is running
	bool IsRunning();

protected:
	// You write these
	virtual void Startup() = 0;

	// Return false to exit
	virtual bool Run() = 0;

	// Called after run
	virtual void Cleanup() = 0;

	// If this is set, the thread should die!
	bool TimeToDie();
	bool SignalDeath();

	// Call this to sleep/yield
	void Sleep(unsigned int milliseconds);

	// Include the rest of the definitions
#if defined(PS2)
#include "OSThread_ps2.h"
#elif defined(GCN)
#include "OSThread_gcn.h"
#elif defined (WIN32)
#include "OSThread_win.h"
#endif

#endif
