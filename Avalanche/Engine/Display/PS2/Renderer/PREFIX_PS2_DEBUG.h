//	===========================================================================
//	PREFIX_PS2_DEBUG.h			©1999 Metrowerks Inc. All rights reserved.
//	===========================================================================
//
//	10/11/1999	kashima,	for SDK0.55
//	10/08/1999	kashima,	add pragma "divbyzerocheck"
//	09/14/1999	kashima

#include <prefix_ps2.h>

#pragma	divbyzerocheck		on	/*	break if divided by zero	*/
#define  PS2
#define	DEBUG					/*	just for debugging	*/
#define	COMPILING_RENDERER /* TJC - to differentiate between renderer & engine in headers */

