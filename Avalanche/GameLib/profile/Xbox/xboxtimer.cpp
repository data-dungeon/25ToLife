//*************************************************************************
// A "Read Time Stamp Code" class that computes the exact number of
// cpu clocks that have gone by since boot.
//
// Only works on Win32 right now, but something like this may exist on
// the other CPU's.
//*************************************************************************

#include "profile/ProfilePCH.h"

#include "xboxtimer.h"

// A global for all to use
CPUTimer g_CPUTimer;

//*************************************************************************
// Compute the MHZ of the machine
//*************************************************************************
CPUTimer::CPUTimer(void)
{
	mhz = 733;

	// Setup the conversion factors
	msDivide = 1.0 / ((CPUTIME)mhz * 1000.0);
	usDivide = 1.0 / (CPUTIME)mhz;
	sDivide = 1.0 / ((CPUTIME)mhz * 1000000.0);
}
