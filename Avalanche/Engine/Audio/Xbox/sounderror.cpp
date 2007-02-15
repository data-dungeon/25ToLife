/***************************************************************************/
// This is a utility class for reporting direct sound errors
//*************************************************************************
#include "Audio/AudioPCH.h"

#include "Audio/Xbox/sounderror.h"

// The last error we experienced
HRESULT SoundError::lastError = DS_OK;

/***************************************************************************/
/***************************************************************************/
void SoundError::SetLastError(
HRESULT error)
{
	lastError = error;

#ifdef _DEBUG
	if (lastError != DS_OK)
		Error(TranslateLastError());
#endif
}

/***************************************************************************/
// Translate the error into a string
/***************************************************************************/
const char * __cdecl SoundError::TranslateError(
HRESULT errorCode)
{
	switch (errorCode)
	{
		case DS_OK:
			return "The request completed successfully";

		case DSERR_CONTROLUNAVAIL:
			return "The control (vol, pan, etc.) requested by the caller is not available";

		case DSERR_INVALIDCALL:
			return "This call is not valid for the current state of this object";

		case DSERR_GENERIC:
			return "An undetermined error occurred inside the DirectSound subsystem";

		case DSERR_OUTOFMEMORY:
			return "Not enough free memory is available to complete the operation";

		case DSERR_UNSUPPORTED:
			return "The function called is not supported at this time";

		case DSERR_NODRIVER:
			return "No sound driver is available for use";

		case DSERR_NOAGGREGATION:
			return "This object does not support aggregation";

        case 0x80070002:
        case 0x80070003:
            return "Audio stream file not found";

		default:
			return "Unknown error code";
	}
}

/***************************************************************************/
// General function to show an error
/***************************************************************************/
void __cdecl SoundError::Error(
const char *format,
...)
{
   char temp[255];
   wvsprintf(temp, format, (char *)(&format + 1));
#ifdef _XBOX
	OutputDebugString(temp);
    OutputDebugString("\n");
#else
   MessageBox(NULL, temp, "Sound Error", MB_OK | MB_ICONHAND);
#endif
}

