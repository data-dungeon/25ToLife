//

#ifdef _MAC
   #define TICKSPERSEC 60.0f
#else
   #include <mmsystem.h>

   #define TICKSPERSEC 1000.0f
#endif

DWORD GetTime();
