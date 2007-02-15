//*************************************************************************
// A "Read Time Stamp Code" class that computes the exact number of
// cpu clocks that have gone by since boot.
//
// Only works on Win32 right now, but something like this may exist on
// the other CPU's.
//*************************************************************************

/******************* includes ****************************/

#include "profile/ProfilePCH.h"





// A global for all to use
CPUTimer g_CPUTimer;

// List of "valid" or common CPU speeds
int CPUTimer::cpuSpeed[] =
{
	90, 100, 133, 150, 166,
	200, 233, 250, 266,
	300, 333, 350, 366,
	400, 433, 450, 466, 475,
	500, 533, 550, 566,
	600, 633, 650, 667,
	700, 733, 750, 766,
	800, 833, 850, 866,
	900, 933, 950, 966,
	1000, 1100, 1130, 1200, 1300, 1330, 1400, 1500, 1600, 1700, 1800, 1900,
	2000, 2100, 2130, 2200, 2300, 2330, 2400, 2500, 2600, 2700, 2800, 2900
};
int CPUTimer::cpuSpeeds = sizeof(CPUTimer::cpuSpeed) / sizeof(int);

//*************************************************************************
// Compute the MHZ of the machine
//*************************************************************************
CPUTimer::CPUTimer(void)
{
	// Do a loop
	float speed = 0.0f;
	int test = 0;
	timeBeginPeriod(1);
	for (; test < 5; test++)
	{
		float dummy = 0.0f;
		DWORD deltaT;
		DWORD startTime = timeGetTime();
		CPUTIME startTick = Get();
		while (1)
		{
			dummy += Math::Sin(dummy);
			deltaT = timeGetTime() - startTime;
			if (deltaT > 50)
				break;
		}
		CPUTIME endTick = Get();
		
		// What is the approximate mhz?
		float delta = (float)(endTick - startTick) / (float)deltaT;
		speed = speed + delta * 0.001f;
	}
	timeEndPeriod(1);
	speed /= (float)test;

	// Get the approximate mhz
	mhz = (int)Math::Floor(speed + 0.5f);

	// Scan the list for a better speed
	int best = 0;
	for (int i = 1; i < cpuSpeeds; i++)
	{
		if (abs(mhz - cpuSpeed[i]) <= abs(mhz - cpuSpeed[best]))
			best = i;
	}

	// Set the best one, if they are off the scale, use raw mhz
	if (mhz - cpuSpeed[best] < 100)
		mhz = cpuSpeed[best];

	// Setup the conversion factors
	msDivide = 1.0 / ((CPUTIME)mhz * 1000.0);
	usDivide = 1.0 / (CPUTIME)mhz;
	sDivide = 1.0 / ((CPUTIME)mhz * 1000000.0);
}

