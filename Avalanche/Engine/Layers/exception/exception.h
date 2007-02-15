//--------------------------------------------------------------//
// Exception.h
//
// On Screen Exception Handler
//
//--------------------------------------------------------------//

#ifndef _EXCEPTION_H
#define _EXCEPTION_H

#if   defined( PS2   )
#elif defined( XBOX  )
#elif defined( GCN   )
  #define DISABLE_EXCEPTIONS
#endif

#if defined( WIN32 ) && !defined (_XBOX)
  #define DISABLE_EXCEPTIONS
#endif

#if defined(CONSUMER_BUILD)
#define DISABLE_EXCEPTIONS
#endif

class ExceptionHandler
{
public:
	static void InitExceptions();
	static void SetExceptionMessage(const char* message);
	static const char* GetExceptionMessage();
};

#endif // _EXCEPTION_H
