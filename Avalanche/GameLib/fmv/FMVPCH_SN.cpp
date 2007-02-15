/*************************************/
/*                                   */
/*   FMVPCH_SN.cpp               */
/*   new engine  06/12/02            */
/*************************************/
#ifdef CW
#define CW_PCH	// enable contents of .h file, see comments in .h file
#endif //CW
#include "FMVPCH.h"

/* For SN, we include all the source files from the projects here */
/* this is done to reduce compile times under SN (which currently */
/* does not support precompiled headers) */
#if defined(PS2) && defined(SN)
#include "fmv.cpp"
#include "FMVScriptFunc.cpp"
#include "ps2/audiodec.cpp"
#include "ps2/fmvdriver.cpp"
#include "ps2/ldimage.cpp"
#include "ps2/playpss.cpp"
#include "ps2/strfile.cpp"
#endif // defined(PS2) && defined(SN)
