//--------------------------------------------------------------//
// PlatformException.cpp
//
// Platform specific init of exception handling
//
//--------------------------------------------------------------//

#ifndef _PLATFORMEXCEPTION_H
#define _PLATFORMEXCEPTION_H

#ifndef DISABLE_EXCEPTIONS

class PlatformException 
{
public:
	static void Init(); // setup platform specific exception handling

	// platform specific funcs
	#if defined (_XBOX)
	static LONG WINAPI Filter( IN struct _EXCEPTION_POINTERS *ExceptionInfo );
	static const char* GetExceptionText( int Code );
	#endif

	#if defined (PS2)
	static void InitDisplay();
	static int  ReadController( float* X1, float* Y1, float* X2, float* Y2 );
	static void DrawRegisterPanel( u_int stat, u_int cause, u_int epc, u_int bva, u_int bpa, u_long128 *gpr, int memUsage, int freeBytes, int largestFree, unsigned int* callStack );
	static void DrawDebugPanel( int butt, int buttEdge, float X1, float Y1, float X2, float Y2 );
	static void EEExceptionHandler( u_int stat, u_int cause, u_int epc, u_int bva, u_int bpa, u_long128 *gpr );
	static void IOPExceptionHandler( void *p,void *d );
	#endif
};

#endif // DISABLE_EXCEPTIONS

#endif // _PLATFORMEXCEPTION_H
