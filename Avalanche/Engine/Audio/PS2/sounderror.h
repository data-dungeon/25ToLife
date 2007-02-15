/***************************************************************************/
// This is a utility class for reporting sony sound errors
/***************************************************************************/
#ifndef SOUNDERROR_H
#define SOUNDERROR_H

typedef enum
{
	SE_OK = 0,
	SE_CANTINITSOUND,
	SE_CANTINITIOP,
	SE_SETTINGCOREVOLUME,
	SE_CANTINITIOPHEAP,
	SE_MISSINGFILE,
	SE_BADFILE,
	SE_OUTOFSPUMEMORY,
	SE_OUTOFEEMEMORY,
	SE_OUTOFIOPMEMORY,
	SE_IOPTRANSFERFAIL,
	SE_SPUTRANSFERFAIL,
	SE_SETTINGVOLLEFT,
	SE_SETTINGVOLRIGHT,
	SE_SETTINGPITCH,
	SE_SETTINGADDR,
	SE_SETTINGADSR1,
	SE_SETTINGADSR2
} SOUNDERROR;

class SoundError
{
public:
	// Set/get the last error
	static void SetLastError(SOUNDERROR error);
	static SOUNDERROR GetLastError()
		{ return lastError; }

   // Translate the error into a string
   static const char *TranslateError(SOUNDERROR errorCode);
   static const char *TranslateLastError()
		{ return TranslateError(GetLastError()); }

   // General function to show an error
   static void Error(const char *format, ...);

private:
	static SOUNDERROR lastError;
};

#endif
