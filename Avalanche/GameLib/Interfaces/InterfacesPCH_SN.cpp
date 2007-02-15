#ifdef CW
#define CW_PCH	// enable contents of .h file, see comments in .h file
#endif //CW
#include "InterfacesPCH.h"

/* For SN, we include all the source files from the projects here */
/* this is done to reduce compile times under SN (which currently */
/* does not support precompiled headers) */
#if defined(PS2) && defined(SN)
#include "AnimActionIF.cpp"
#include "Interface.cpp"
#include "InterfacesScriptFunc.cpp"
#include "MoveIF.cpp"

#endif // defined(PS2) && defined(SN)
