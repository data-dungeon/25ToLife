//--------------------------------------------------------------//
// DiscError_gcn.cpp
//
// Display message onscreen that disc failed
//
//--------------------------------------------------------------//

#include "Layers/LayersPCH.h"
#include "Layers/DiscErrorHandler/DiscError.h"
#include "Layers/DiscErrorHandler/platformDiscError.h"

#if defined (PSP)

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
#endif	// PSP
