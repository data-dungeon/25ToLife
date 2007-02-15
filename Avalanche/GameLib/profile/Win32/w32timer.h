//*************************************************************************
// A "Read Time Stamp Code" class that computes the exact number of
// cpu clocks that have gone by since boot.
//
// Only works on Win32 right now, but something like this may exist on
// the other CPU's.
//*************************************************************************
#ifndef W32TIMER_H
#define W32TIMER_H

// 64-bit int
typedef __int64 CPUTIME;

class CPUTimer
{
public:
	// Constructor determines CPU speed
	CPUTimer(void);

	// Get the current ela
	inline CPUTIME Get(void);

	// Convert from clocks to a real time
	inline int ToClockCycles(CPUTIME t)
		{ return((int)t); }
	inline int ToMilliSeconds(CPUTIME t)
		{ return((int)(ToDouble(t) * msDivide)); }
	inline int ToMicroSeconds(CPUTIME t)
		{ return((int)(ToDouble(t) * usDivide)); }
	inline float ToSeconds(CPUTIME t)
		{ return((float)(ToDouble(t) * sDivide)); }

	// Convert from a real time to clocks
	inline CPUTIME FromClockCycles(int t)
		{ return((CPUTIME)t); }
	inline CPUTIME FromMilliSeconds(int t)
		{ return(FromDouble((double)(t) / msDivide)); }
	inline CPUTIME FromMicroSeconds(int t)
		{ return(FromDouble((double)(t) / usDivide)); }
	inline CPUTIME FromSeconds(float t)
		{ return(FromDouble((double)(t) / sDivide)); }

	// Returns the CPU speed in mhz
	inline int CPUSpeed(void)
		{ return(mhz); }

private:
	int			mhz;           		// Processsor speed in MHZ
	double      msDivide;				// 1/(mhz * 1000)
	double      usDivide;				// 1/mhz
	double      sDivide;					// 1/(mhz * 1000000)

	// Function to convert into a double from an int64
	inline double ToDouble(CPUTIME t)
		{ return((double)t); }

	// Function to convert into an int64 from a double
	inline CPUTIME FromDouble(double t)
		{ return((CPUTIME)t); }

	// Cheesy lookup table for valid cpu speeds
	static 		int cpuSpeed[];
	static 		int cpuSpeeds;
};

// Implementation of the functions
#include "w32timer.hpp"

#endif
