/***************************************************************************/
// This is a utility class for reporting direct sound errors
/***************************************************************************/
#ifndef SOUNDERROR_H
#define SOUNDERROR_H

#define WIN32_LEAN_AND_MEAN
#ifdef _XBOX
#else
#include <windows.h>
#include <mmsystem.h>
#endif
#include <dsound.h>

class SoundError
{
public:
	// Set/get the last error
	static void SetLastError(HRESULT error);
	static HRESULT GetLastError()
		{ return lastError; }

   // Translate the error into a string
   static const char * __cdecl TranslateError(HRESULT errorCode);
   static const char * __cdecl TranslateLastError()
		{ return TranslateError(GetLastError()); }

   // General function to show an error
   static void __cdecl Error(const char *format, ...);

private:
	static HRESULT	lastError;
};

#endif
