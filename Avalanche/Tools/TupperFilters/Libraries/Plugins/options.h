////////////////////////////////////////////////////////////////////////////
//
// Options
//
// Class to handle options for plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: options.h $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 5/17/02    Time: 11:38a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * renamed tupperware string and array
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/10/02    Time: 2:52p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Configuration file working
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/09/02    Time: 4:36p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * skeleton filter chain complete
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_OPTIONS_H
#define TUPPERFILTERS_LIB_PLUGINS_OPTIONS_H

#ifndef TUPPERWARE_STRING_H
#include <tupperware/tupstring.h>
#endif

#ifndef TUPPERWARE_ARRAY_H
#include <tupperware/tuparray.h>
#endif

class FilterOption
{
	TupString m_key;
	TupString m_value;

public:
	FilterOption(void);
	FilterOption(const char *key,const char *value);

	const char *GetKey(void) const;
	const char *GetValue(void) const;

	void SetKey(const char *key);
	void SetValue(const char *value);
};

class FilterOptions
{
	TupArray<FilterOption> m_optionArray;
public:

	FilterOptions(void);
	FilterOptions(const FilterOptions &s);
	FilterOptions &operator=(const FilterOptions &s);

	void Copy(const FilterOptions &s);

	void AddOption(const FilterOption &option);
	void AddOption(const char *key,const char *value);
	int GetNumOptions(void) const;
	const FilterOption &GetOption(int index) const;
	const char *GetKey(int index) const;
	const char *GetValue(int index) const;
	const char *GetValue(const char *key) const;
	bool RemoveOption(const char *key);
	bool RemoveOption(int index);
};

#endif
