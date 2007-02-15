#ifdef CW
#define CW_PCH	// enable contents of .h file, see comments in .h file
#endif //CW
#include "LoopPCH.h"

/* For SN, we include all the source files from the projects here */
/* this is done to reduce compile times under SN (which currently */
/* does not support precompiled headers) */
#if defined(PS2) && defined(SN)
#include "ActorLoop.cpp"
#include "BaseLoop.cpp"
#include "Environment.cpp"
#include "LoopScenes.cpp"
#endif // defined(PS2) && defined(SN)
