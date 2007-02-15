/////////////////////////////////////////////////////////////////////////////
// Simple utility to log time
/////////////////////////////////////////////////////////////////////////////
#ifndef LOGTIME_H
#define LOGTIME_H

#include "profile/cputimer.h"
#include "profile/log.h"

class LogTime
{
public:
	// Start the time on initialization
	LogTime(void)
		{ total = 0; calls = 0; }

	inline void Enter(void)
		{ start = g_CPUTimer.Get(); }
	inline void Exit(void)
		{ total += g_CPUTimer.Get() - start; ++calls; }

	// Output the elapsed time in seconds
	inline float TotalSeconds(void)
		{ return(g_CPUTimer.ToSeconds(total)); }
	inline float AverageSeconds(void)
		{ return(g_CPUTimer.ToSeconds(total / calls)); }
	inline int Calls(void)
		{ return((int)calls); }

	// Output the elapsed seconds to the log
	void LogElapsed(char *title);

private:
	CPUTIME	start;
	CPUTIME	total;
	CPUTIME	calls;
};

#endif
