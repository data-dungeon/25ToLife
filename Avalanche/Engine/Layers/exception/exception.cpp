//--------------------------------------------------------------//
// Exception.cpp
//
// On Screen Exception Handler
//
//--------------------------------------------------------------//

#include "Layers/LayersPCH.h"
#include "Layers/exception/exception.h"
#include "Layers/exception/platformexception.h"

#ifndef DISABLE_EXCEPTIONS
static char s_exceptionMessage[256];
#endif

//--------------------------------------------------------------//

void ExceptionHandler::InitExceptions()
{
	#ifndef DISABLE_EXCEPTIONS
	s_exceptionMessage[0] = 0;
	PlatformException::Init();
	#endif
}

//--------------------------------------------------------------//

void ExceptionHandler::SetExceptionMessage(const char* message)
{
	#ifndef DISABLE_EXCEPTIONS
	if (message)
	{
		strncpy(s_exceptionMessage, message, sizeof(s_exceptionMessage) - 1);
		s_exceptionMessage[sizeof(s_exceptionMessage) - 1] = 0;
	}
	else
	{
		s_exceptionMessage[0] = 0;
	}
	#endif DISABLE_EXCEPTIONS
}

//--------------------------------------------------------------//

const char* ExceptionHandler::GetExceptionMessage()
{
	#ifndef DISABLE_EXCEPTIONS
	return s_exceptionMessage;
	#else
	return "";
	#endif
}

//--------------------------------------------------------------//
