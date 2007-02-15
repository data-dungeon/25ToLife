////////////////////////////////////////////////////////////////////////////
//
// Exception
//
// class for errors
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#define EXCEPTION_H

enum ErrorLevels
{
	ERROR_MINOR_WARNING,
	ERROR_WARNING,
	ERROR_FATAL_BUT_CONTINUE,
	ERROR_FATAL
};

class Exception
{
public:
	//! Creates an exception.
	Exception(int code, const char * pFormatString,...);

	//! Copy constructor.
	Exception (const Exception& s );

	//! Destroys an exception
	~Exception();

	int GetCode(void) const;

	operator const char *() const;

private:
	int m_code;
	char *m_pErrorString;
};


