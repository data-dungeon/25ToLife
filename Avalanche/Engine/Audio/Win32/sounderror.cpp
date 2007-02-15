/***************************************************************************/
// This is a utility class for reporting direct sound errors
//*************************************************************************

/******************* includes ****************************/

#include "Audio/AudioPCH.h"





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
			return "The request completed successfully.";

#ifndef _XBOX
		case DSERR_ALLOCATED:
			return "The request failed because resources, such as a priority level, were already in use by another caller.";

		case DSERR_ALREADYINITIALIZED:
			return "The object is already initialized.";

		case DSERR_BADFORMAT:
			return "The specified wave format is not supported.";

		case DSERR_BUFFERLOST:
			return "The buffer memory has been lost and must be restored.";

		case DSERR_CONTROLUNAVAIL:
			return "The control (volume, pan, and so forth) requested by the caller is not available.";

		case DSERR_GENERIC:
			return "An undetermined error occurred inside the DirectSound subsystem.";

		case DSERR_INVALIDCALL:
			return "This function is not valid for the current state of this object.";

		case DSERR_INVALIDPARAM:
			return "An invalid parameter was passed to the returning function.";

		case DSERR_NOAGGREGATION:
			return "The object does not support aggregation.";

		case DSERR_NODRIVER:
			return "No sound driver is available for use.";

		case DSERR_NOINTERFACE:
			return "The requested COM interface is not available.";

		case DSERR_OTHERAPPHASPRIO:
			return "Another application has a higher priority level, preventing this call from succeeding.";

		case DSERR_OUTOFMEMORY:
			return "The DirectSound subsystem could not allocate sufficient memory to complete the caller's request.";

		case DSERR_PRIOLEVELNEEDED:
			return "The caller does not have the priority level required for the function to succeed.";

		case DSERR_UNINITIALIZED:
			return "The IDirectSound::Initialize method has not been called or has not been called successfully before other methods were called.";

#endif
		case DSERR_UNSUPPORTED:
			return "The function called is not supported at this time.";

		default:
			return "Unknown error code.";
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
#else
   //MessageBox(NULL, temp, "Sound Error", MB_OK | MB_ICONHAND);
#endif
}

