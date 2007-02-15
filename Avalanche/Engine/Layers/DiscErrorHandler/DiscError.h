//--------------------------------------------------------------//
// DiscError.h
//
// On Screen Disc Error Handler
//
//--------------------------------------------------------------//

#ifndef _DISCERROR_H
#define _DISCERROR_H


class DiscErrorHandler
{
public:
	static	void Init();
	static	void SetLastFilename(char* Fname);
	static	bool DisplayError(char* Fname = NULL, int ErrorType = 0);

	enum {
		OPEN_FILE_FAILED = 1,
		READ_FILE_FAILED,
		NUM_ERRORCODES
	};
};

extern	char*	DiscErrorHandler_LastFilename;

#endif // _DISCERROR_H

