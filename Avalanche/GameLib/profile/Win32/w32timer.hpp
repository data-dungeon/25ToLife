//*************************************************************************
// A "Read Time Stamp Code" class that computes the exact number of
// cpu clocks that have gone by since boot.
//*************************************************************************

//*************************************************************************
// A really handy thing to return the time-stamp on a pentium+
//*************************************************************************
#pragma warning(disable : 4035)
inline CPUTIME CPUTimer::Get(void)
{
	__asm
	{
		xor		eax, eax
		xor		edx, edx
		_emit		0fH
		_emit		031H
	}
}
#pragma warning(default : 4035)
