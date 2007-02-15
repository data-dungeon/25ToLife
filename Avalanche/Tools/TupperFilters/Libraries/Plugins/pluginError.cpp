////////////////////////////////////////////////////////////////////////////
//
// PluginError
//
// class for all plugin errors
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: pluginError.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/19/02    Time: 5:08p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * First stage of gui app complete
 * Log window working
 * Exceptions set up
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/07/02    Time: 5:13p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * ported from import export (good place to start)
 */

/* system includes */


#include "PluginsPch.h"
#include "PluginError.h"		// error codes
#include <Platform/BaseType.h>

PluginException::PluginException(int code, const char * pFormatString, ...)
{
	SetErrorMode (0);	// Restore normal error handling just in case
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
PluginException::PluginException (const PluginException& s )
{
	m_code = s.GetCode();
	m_pErrorString = new char [strlen((const char *)s)+1];
	strcpy (m_pErrorString, (const char *)s);
}

PluginException::~PluginException()
{
	delete m_pErrorString;
}

int PluginException::GetCode(void) const
{
	return m_code;
}

PluginException::operator const char *() const
{
	return m_pErrorString;
}

