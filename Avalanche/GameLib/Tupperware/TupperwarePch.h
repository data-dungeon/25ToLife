////////////////////////////////////////////////////////////////////////////
//
// TupperwarePch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWARE_PCH_H
#define TUPPERWARE_PCH_H


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <Assert.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <Time.h>
#ifndef GCN
#include <malloc.h>
#endif

#ifdef GCN
#define _MAX_PATH   260 /* max. length of full pathname */
#define _MAX_DRIVE  3   /* max. length of drive component */
#define _MAX_DIR    256 /* max. length of path component */
#define _MAX_FNAME  256 /* max. length of file name component */
#define _MAX_EXT    256 /* max. length of extension component */
#endif

#ifdef PS2
#define _MAX_PATH   260 /* max. length of full pathname */
#define _MAX_DRIVE  3   /* max. length of drive component */
#define _MAX_DIR    256 /* max. length of path component */
#define _MAX_FNAME  256 /* max. length of file name component */
#define _MAX_EXT    256 /* max. length of extension component */
#if !defined(PS2) || !defined(SN)
#include <varargs.h>
#endif
#endif

#ifdef WIN32
#include <direct.h>	// for include additions
#endif

#ifdef TUP_EXT
#define TRACK_LINE_NUMBERS
#endif

#endif
