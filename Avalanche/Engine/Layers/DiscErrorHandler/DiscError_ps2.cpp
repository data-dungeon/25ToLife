//--------------------------------------------------------------//
// DiscError_ps2.cpp
//
// Display message onscreen that disc failed
//
//--------------------------------------------------------------//

#include "Layers/LayersPCH.h"
#include "Layers/DiscErrorHandler/DiscError.h"
#include "Layers/DiscErrorHandler/platformDiscError.h"

#if defined (PS2)


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
#endif	// PS2
