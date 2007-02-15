/*************************************/
/*                                   */
/*   l_Debug.h                       */
/*   new engine                      */
/*   avalanche software              */
/*   originally by joe barnes        */
/*   visual c++ 6.0                  */
/*   layers debug routines           */
/*                                   */
/*************************************/

#ifndef __L_DEBUG_H
#define __L_DEBUG_H

/******************* includes ****************************/

/* system includes */

#if defined (WIN32) && !defined (_XBOX)
#include <windows.h>	// for OutputDebugString
#endif //WIN32

#if defined (_XBOX)
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#endif //XBOX

#include <stdio.h>

#ifdef GCN
#include <Dolphin/os.h>
#endif

/* engine includes */
#include "platform/BaseType.h"

#if !defined(CONSUMER_BUILD)

	#define CRT_ERROR(pString)		{ conPuts(pString); LOCKUP( ); }

	#ifndef TRACE	// don't override windows-based tool trace declaration
	#define TRACE		dbgPrint
	#endif //TRACE

	#if defined(WIN32) && defined(_XBOX)
		#define conPuts(pString)	OutputDebugString(pString)
		#define ERROR_BOX(pString)	CRT_ERROR(pString)
	#elif defined(WIN32) && !defined(_XBOX)
		#define conPuts(pString)	OutputDebugString(pString)
		#define ERROR_BOX(pString)	{ MessageBox(Display__GetWnd(), pString, "Fatal Error", MB_ICONSTOP); exit(EXIT_FAILURE); }
	#elif defined(PS2)
		extern void SendToRemoteTerminal(const char* string);
		#define conPuts(pString)	SendToRemoteTerminal(pString)
		#define ERROR_BOX(pString)	CRT_ERROR(pString)
	#elif defined(GCN)
		#define conPuts(pString)	OSReport("%s", pString)
		#define ERROR_BOX(pString)	CRT_ERROR(pString)
	#endif //WIN32/PS2

#else //CONSUMER BUILD

	//#define LOCKUP()			((void)0)
	#define conPuts(pString)	((void)0)
	#define ErrorBox(pString)	((void)0)
	#define CRT_ERROR(pString)	((void)0)
#ifndef _XBOX
	#define TRACE(...)			((void)0)
#else
	inline void TRACE(...) {}
#endif

#endif //CONSUMER_BUILD

/******************* structures **************************/

/******************* global variables ********************/

/******************* global prototypes *******************/

#ifndef CONSUMER_BUILD
void 		dbgInit(u8 u8StartLevel, bool bOn);

void 		dbgPrint(const char *pcFmt, ...);
void 		dbgPrintT(u32 u32ErrorType, const char *pFmt, ...);

void 		dbgTypeOn(u32 u32ErrorType);
void 		dbgTypeOff(u32 u32ErrorType);
void 		dbgTypeToggle(u32 u32ErrorType);
bool 		dbgTypeStatus(u32 u32ErrorType);

void 		dbgLevelSet(u8 u8Level);
u8			dbgLevelGet(void);
#else
inline void	dbgInit(u8 u8StartLevel, bool bOn) {}

inline void	dbgPrint(const char *pcFmt, ...) {}
inline void	dbgPrintT(u32 u32ErrorType, const char *pFmt, ...) {}

inline void dbgTypeOn(u32 u32ErrorType) {}
inline void dbgTypeOff(u32 u32ErrorType) {}
inline void dbgTypeToggle(u32 u32ErrorType) {}
inline bool dbgTypeStatus(u32 u32ErrorType) { return false; }

inline void dbgLevelSet(u8 u8Level) {}
inline u8 dbgLevelGet(void) { return 0; }
#endif

#endif // __L_DEBUG_H
