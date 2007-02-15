////////////////////////////////////////////////////////////////////////////
//
// PluginError
//
// class for all plugin errors
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Exception.h"		// error codes

Exception::Exception(int code, const char * pFormatString, ...)
{
	// file system error checking was disabled.
	static char acMessage[800];
	va_list PrintArgs;

	/* create message */

	va_start(PrintArgs, pFormatString);
	vsprintf(acMessage, pFormatString, PrintArgs);
	va_end(PrintArgs);

	/* anything to do? */
	m_pErrorString = new char[strlen(acMessage)+1];
	strcpy (m_pErrorString, acMessage);
	m_code = code;
}

//! Copy constructor.
Exception::Exception (const Exception& s )
{
	m_code = s.GetCode();
	m_pErrorString = new char [strlen((const char *)s)+1];
	strcpy (m_pErrorString, (const char *)s);
}

Exception::~Exception()
{
	delete m_pErrorString;
}

int Exception::GetCode(void) const
{
	return m_code;
}

Exception::operator const char *() const
{
	return m_pErrorString;
}

