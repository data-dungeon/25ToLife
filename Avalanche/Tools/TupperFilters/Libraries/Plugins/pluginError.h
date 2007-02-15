////////////////////////////////////////////////////////////////////////////
//
// PluginError
//
// class for all plugin errors
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: pluginError.h $
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
  
#ifndef __PLUGIN_ERROR_H
#define __PLUGIN_ERROR_H

enum ErrorLevels
{
	ERROR_MINOR_WARNING,
	ERROR_WARNING,
	ERROR_FATAL_BUT_CONTINUE,
	ERROR_FATAL
};

class PluginException
{
public:
	//! Creates an exception.
	PluginException(int code, const char * pFormatString,...);

	//! Copy constructor.
	PluginException (const PluginException& s );

	//! Destroys an exception
	~PluginException();

	int GetCode(void) const;

	operator const char *() const;

private:
	int m_code;
	char *m_pErrorString;
};


#endif // __PLUGIN_ERROR_H
