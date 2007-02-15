//=============================================================================//
//  Copyright ©2001 Avalanche Software.  All Rights Reserved.                  //
//=============================================================================//
//      File: timer.cpp                                                        //
//    Author: Daniel Silver                                                    //
//   Created: 04/05/2001                                                       //
//=============================================================================//

#include "pch.h"
#include "timer.h"

double Timer( void )
{
	static double s = 0.0;

	LARGE_INTEGER c, f;

	QueryPerformanceCounter( &c );
	QueryPerformanceFrequency( &f );

	if( s == 0.0 )
		s = (double) c.QuadPart / (double) f.QuadPart;

	return (double) c.QuadPart / (double) f.QuadPart - s;
}

static double timer = 0.0;

void StartTimer()
{
	timer = Timer();
}

double EndTimer( void )
{
	return Timer() - timer;
}

double CpuTimer( void )
{
	__int64 timer;

	__asm
	{
		rdtsc;
		mov		DWORD PTR timer+0, eax;
		mov		DWORD PTR timer+4, edx;
	}

	return (double) timer;
}

__int64 CpuTimeStamp( void )
{
	__int64 c;

	__asm
	{
		rdtsc
		mov		DWORD PTR c+0, eax
		mov		DWORD PTR c+4, edx;
	}

	return c;
}