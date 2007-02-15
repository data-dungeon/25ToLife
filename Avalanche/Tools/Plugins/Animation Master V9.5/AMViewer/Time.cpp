// KB  3/11/02  \Ken90\AMViewer\Time.cpp

#include "stdafx.h"
#include "Time.h"
#ifdef _MAC
#include "macname1.h"
#include "Events.h"
#include "macname2.h"
#endif

DWORD GetTime()
{
   DWORD starttime;
#ifdef _MAC
   starttime = TickCount();
#else
   #if (_MFC_VER <= 0x0420)
      starttime = clock();
   #else
      starttime = timeGetTime();
   #endif
#endif // MAC
   return starttime;
}

