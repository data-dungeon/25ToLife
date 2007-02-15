//--------------------------------------------------------------//
// DiscError.cpp
//
// On Screen DiscError Handler
//
//--------------------------------------------------------------//

#include "Layers/LayersPCH.h"

#include "Layers/DiscErrorHandler/DiscError.h"
#include "Layers/DiscErrorHandler/platformDiscError.h"

char*	DiscErrorHandler_LastFilename;

//--------------------------------------------------------------//

void DiscErrorHandler::Init()
{
	DiscErrorHandler_LastFilename = NULL;
	PlatformDiscError::Init();
}

//--------------------------------------------------------------//

bool DiscErrorHandler::DisplayError(char* Fname, int ErrorType)
{
	DiscErrorHandler_LastFilename = Fname;
	return (PlatformDiscError::DisplayError(Fname, ErrorType));
}

//--------------------------------------------------------------//

void DiscErrorHandler::SetLastFilename(char* Fname)
{
	DiscErrorHandler_LastFilename = Fname;
}
