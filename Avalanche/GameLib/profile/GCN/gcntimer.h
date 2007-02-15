//*************************************************************************
// A "Read Time Stamp Code" class that computes the exact number of
// cpu clocks that have gone by since boot.
//*************************************************************************
#ifndef GCNTIMER_H
#define GCNTIMER_H

// 64-bit int, but we manipulate as a double
typedef long CPUTIME;

class CPUTimer
{
public:
	// Constructor determines CPU speed
	CPUTimer(void);

	// Get the current ela
	inline CPUTIME Get(void) { return(0); }

	// Convert from clocks to a real time
	inline int ToClockCycles(CPUTIME t)
		{ return((int)t); }
	inline int ToMilliSeconds(CPUTIME t)
		{ return((int)(t / msDivide)); }
	inline int ToMicroSeconds(CPUTIME t)
		{ return((int)(t / usDivide)); }
	inline float ToSeconds(CPUTIME t)
		{ return((float)t / sDivide); }

	// Convert from a real time to clocks
	inline CPUTIME FromClockCycles(int t)
		{ return((CPUTIME)t); }
	inline CPUTIME FromMilliSeconds(int t)
		{ return((CPUTIME)(t) * msDivide); }
	inline CPUTIME FromMicroSeconds(int t)
		{ return((CPUTIME)(t) * usDivide); }
	inline CPUTIME FromSeconds(float t)
		{ return((CPUTIME)(t * sDivide)); }

	// Returns the CPU speed in mhz
	inline int CPUSpeed(void)
		{ return(400); }

private:
	CPUTIME		msDivide;				// 1/(mhz * 1000)
	CPUTIME		usDivide;				// 1/mhz
	float			sDivide;					// 1/(mhz * 1000000)
};

#endif
