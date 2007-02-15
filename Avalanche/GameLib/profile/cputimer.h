//*************************************************************************
// A "Read Time Stamp Code" class that computes the exact number of
// cpu clocks that have gone by since boot.
//*************************************************************************
#ifndef CPUTIMER_H
#define CPUTIMER_H

//*************************************************************************
// All the platforms implement a class that looks just like this:
// The type CPUTIME is platform specific...and may be a float or int :(
/**************************************************************************
typedef long int CPUTIME;

class CPUTimer
{
public:
	CPUTimer(void);

	// Get the current time
	inline CPUTIME Get(void);

	// Convert from clocks to a real time
	inline int ToClockCycles(CPUTIME t);
	inline int ToMilliSeconds(CPUTIME t);
	inline int ToMicroSeconds(CPUTIME t);
	inline float ToSeconds(CPUTIME t);

	// Convert from a real time to clocks
	inline CPUTIME FromClockCycles(int t);
	inline CPUTIME FromMilliSeconds(int t);
	inline CPUTIME FromMicroSeconds(int t);
	inline CPUTIME FromSeconds(float t);

	// Returns the CPU speed in mhz
	inline int CPUSpeed(void)
};
*/

// Include the appropriate file
#if defined(WIN32)
#if defined(_XBOX)
#include "Xbox/xboxtimer.h"
#else
#include "Win32/w32timer.h"
#endif
#elif defined(PS2)
#include "PS2/ps2timer.h"
#elif defined(GCN)
#include "profile/GCN/gcntimer.h"
#else
// Oh shit, they killed Kenny!   You are probably fuming at the
// compile error right now...sorry
#endif

// -----------------------
// A global for all to use
// -----------------------
extern CPUTimer g_CPUTimer;

#endif
