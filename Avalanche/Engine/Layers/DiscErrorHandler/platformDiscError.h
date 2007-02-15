//--------------------------------------------------------------//
// PlatformDiscError.h
//
// Platform specific init of DiscErrorHandler
//
//--------------------------------------------------------------//

#ifndef _PLATFORMDISCERROR_H
#define _PLATFORMDISCERROR_H

class PlatformDiscError
{
public:
	static	void Init();												// setup platform specific exception handling
	static	bool DisplayError(char* Fname, int ErrorType);	// Display the disc error

	// platform specific funcs
	#if defined (_XBOX)
	#endif

	#if defined (PS2)
	#endif

	#if defined (GCN)
	#endif
};

#endif //_PLATFORMDISCERROR_H


