//*************************************************************************
// A "Read Time Stamp Code" class that computes the exact number of
// cpu clocks that have gone by since boot.
//*************************************************************************

/******************* includes ****************************/

#include "profile/ProfilePCH.h"





// A global for all to use
CPUTimer g_CPUTimer;

//*************************************************************************
// Compute the MHZ of the machine
//*************************************************************************
CPUTimer::CPUTimer(void)
{
	// Setup the conversion factors
	msDivide = 300 * 1000;
	usDivide = 300;
	sDivide = 1.0f / (300.0f * 1000000.0f);
}

