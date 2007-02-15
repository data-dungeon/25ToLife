// stdafx.cpp : source file that includes just the standard includes
//	TupperChainGuiApp.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information
// smartheap stuff
#ifdef _DEBUG
//#include <smartheap\include\shmfc.cpp> // HeapAgent in debug builds
#else
#ifndef _HYBRID
//#include <smartheap\include\shmfc.cpp> // SmartHeap in release builds
#endif // _HYBRID
#endif // _DEBUG
// end smartheap stuff

#include "stdafx.h"

const char *g_pFilterChainDate = __DATE__;
const char *g_pFilterChainTime = __TIME__;


