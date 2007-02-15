////////////////////////////////////////////////////////////////////////////
//
// OptionOverrides
//
// Class to handle the option overrides
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: OptionOverrides.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/25/02   Time: 12:40p
 * Updated in $/Avalanche/tools/TupperChain
 * option override code
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERCHAIN_OPTION_OVERRIDES_H
#define TUPPERCHAIN_OPTION_OVERRIDES_H



class PluginOverrideOptions
{
	TupString m_pluginName;
	FilterOptions m_options;

public:
	PluginOverrideOptions(void);
	PluginOverrideOptions(const char *pluginName,const FilterOptions &options);
	const char *GetPluginName(void) const;
	const FilterOptions &GetOptions(void) const;
	void AddOption(const FilterOption &option);
	void AddOption(const char *key,const char *value);
};

class OptionOverrides
{
	// these are not defined
	OptionOverrides &operator=(const OptionOverrides &s); 
	OptionOverrides(const OptionOverrides &s);

	FilterOptions m_globalOverrideOptions;
	TupArray<PluginOverrideOptions> m_pluginOverrideOptions;
public:
	OptionOverrides(void);
	
	void AddGlobalOverrideOption(const FilterOption &option);
	void AddPluginOverrideOption(const char *pluginName,const FilterOption &option);

	void OverridePluginOptions(const char *pluginName,FilterOptions &options);
	void OverrideGlobalOptions(FilterOptions &options);

	const FilterOptions &GetGlobalOverrideOptions(void) const;
	const TupArray<PluginOverrideOptions> &GetPluginOverrideOptions(void) const;
};

#endif
