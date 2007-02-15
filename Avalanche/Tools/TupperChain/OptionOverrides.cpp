////////////////////////////////////////////////////////////////////////////
//
// OptionOverrides
//
// Class to handle the option overrides
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: OptionOverrides.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/25/02   Time: 12:40p
 * Updated in $/Avalanche/tools/TupperChain
 * option override code
 */

////////////////////////////////////////////////////////////////////////////

#include "TupperChainPch.h"
#include "OptionOverrides.h"

// PluginOverrideOptions methods
PluginOverrideOptions::PluginOverrideOptions(void)
{
}

PluginOverrideOptions::PluginOverrideOptions(const char *pPluginName,const FilterOptions &options) : 
	m_pluginName(pPluginName),
	m_options(options)
{
}

const char *PluginOverrideOptions::GetPluginName(void) const
{
	return m_pluginName;
}

const FilterOptions &PluginOverrideOptions::GetOptions(void) const
{
	return m_options;
}

void PluginOverrideOptions::AddOption(const FilterOption &option)
{
	m_options.AddOption(option);
}

void PluginOverrideOptions::AddOption(const char *key,const char *value)
{
	m_options.AddOption(key,value);
}


// OptionOverrides methods
OptionOverrides::OptionOverrides(void)
{
}
	
void OptionOverrides::AddGlobalOverrideOption(const FilterOption &option)
{
	m_globalOverrideOptions.AddOption(option);
}

void OptionOverrides::AddPluginOverrideOption(const char *pPluginName,const FilterOption &option)
{
	int totalPlugins = m_pluginOverrideOptions.GetSize();
	int pluginIndex;
	for (pluginIndex=0;pluginIndex<totalPlugins;pluginIndex++)
	{
		PluginOverrideOptions &pluginOverrideOptions = m_pluginOverrideOptions[pluginIndex];
		// if we found the plugin then add the option to the array
		if (!stricmp(pPluginName,pluginOverrideOptions.GetPluginName()))
		{
			pluginOverrideOptions.AddOption(option);
			return;
		}
	}
	// if the plugin was not found then create a new plugin with the option
	FilterOptions newOptions;
	newOptions.AddOption(option);
	PluginOverrideOptions newPluginOverrideOptions(pPluginName,newOptions);
	m_pluginOverrideOptions.Add(newPluginOverrideOptions);
}

void OptionOverrides::OverridePluginOptions(const char *pPluginName,FilterOptions &options)
{
	int totalPlugins = m_pluginOverrideOptions.GetSize();
	int pluginIndex;
	for (pluginIndex=0;pluginIndex<totalPlugins;pluginIndex++)
	{
		PluginOverrideOptions &pluginOverrideOptions = m_pluginOverrideOptions[pluginIndex];
		// if we found the plugin then add the plugin options to the passed options object
		if (!stricmp(pPluginName,pluginOverrideOptions.GetPluginName()))
		{
			const FilterOptions &pluginOptions = pluginOverrideOptions.GetOptions();
			int totalOptions = pluginOptions.GetNumOptions();
			int optionIndex;
			for (optionIndex=0;optionIndex<totalOptions;optionIndex++)
			{
				const FilterOption &option = pluginOptions.GetOption(optionIndex);
				if (strlen(option.GetValue()))
					options.AddOption(option);
				else
					options.RemoveOption(option.GetKey());
			}
			return; // we are done
		}
	}
}

void OptionOverrides::OverrideGlobalOptions(FilterOptions &options)
{
	int totalOptions = m_globalOverrideOptions.GetNumOptions();
	int optionIndex;
	for (optionIndex=0;optionIndex<totalOptions;optionIndex++)
	{
		const FilterOption &option = m_globalOverrideOptions.GetOption(optionIndex);
		if (strlen(option.GetValue()))
			options.AddOption(option);
		else
			options.RemoveOption(option.GetKey());
	}
}

const FilterOptions &OptionOverrides::GetGlobalOverrideOptions(void) const
{
	return m_globalOverrideOptions;
}

const TupArray<PluginOverrideOptions> &OptionOverrides::GetPluginOverrideOptions(void) const
{
	return m_pluginOverrideOptions;
}


