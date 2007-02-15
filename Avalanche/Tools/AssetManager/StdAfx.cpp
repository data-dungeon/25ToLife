// stdafx.cpp : source file that includes just the standard includes
//	TupperChainGuiApp.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information
// smartheap stuff
#ifdef _DEBUG
//#include <smartheap\include\shmfc.cpp> // HeapAgent in debug builds
#else
//#include <smartheap\include\shmfc.cpp> // SmartHeap in release builds
#endif // _DEBUG
// end smartheap stuff

#include "stdafx.h"

const char *g_pAssetManagerDate = __DATE__;
const char *g_pAssetManagerTime = __TIME__;
const char *g_pAssetManagerVersion = "1.4.013";

