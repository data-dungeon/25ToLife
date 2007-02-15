/***************************************************************************/
// This is a utility class for reporting sony sound errors
//*************************************************************************

/******************* includes ****************************/

#include "Audio/AudioPCH.h"


// The last error we experienced
SOUNDERROR SoundError::lastError = SE_OK;

/***************************************************************************/
/***************************************************************************/
void SoundError::SetLastError(
SOUNDERROR error)
{
	lastError = error;
}

/***************************************************************************/
// Translate the error into a string
/***************************************************************************/
const char *SoundError::TranslateError(
SOUNDERROR errorCode)
{
	switch (errorCode)
	{
		case SE_OK:
			return "The request completed successfully.";

		case SE_CANTINITSOUND:
			return "Can't initialize sound system.";

		case SE_CANTINITIOP:
			return "Can't initialize IOP.";

		case SE_SETTINGCOREVOLUME:
			return "Setting core volume.";

		case SE_CANTINITIOPHEAP:
			return "Can't initialzie IOP heap.";

		default:
			return "Unknown error code.";
	}
}

/***************************************************************************/
// General function to show an error
/***************************************************************************/
void SoundError::Error(
const char *format,
...)
{
#ifndef CDROM
	va_list args;
	va_start(args, format);

   char temp[255];
	strcpy(temp, "**** Sound Error: ");
   vsprintf(temp + strlen(temp), format, args);

	va_end(args);
   conPuts(temp);
#endif
}

