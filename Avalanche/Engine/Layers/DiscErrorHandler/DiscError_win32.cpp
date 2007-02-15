//--------------------------------------------------------------//
// DiscError_win32.cpp
//
// Display message onscreen that disc failed
//
//--------------------------------------------------------------//

#include "Layers/LayersPCH.h"
#include "Layers/DiscErrorHandler/DiscError.h"
#include "Layers/DiscErrorHandler/platformDiscError.h"

#if defined (WIN32) && !defined (_XBOX)

//--------------------------------------------------------------//

void PlatformDiscError::Init(void)
{
}

//--------------------------------------------------------------//
bool	PlatformDiscError::DisplayError(char* Fname, int ErrorType)
{
	return true;
}

//--------------------------------------------------------------//
#endif	// WIN32
