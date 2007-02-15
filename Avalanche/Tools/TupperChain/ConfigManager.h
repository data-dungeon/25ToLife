////////////////////////////////////////////////////////////////////////////
//
// ConfigManager
//
// Class handle the configuration file
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ConfigManager.h $
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 11/25/02   Time: 12:40p
 * Updated in $/Avalanche/tools/TupperChain
 * option override code
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 7/29/02    Time: 3:12p
 * Updated in $/Avalanche/tools/TupperChain
 * Image Reader Options available to Image Reader Plugins
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperChain
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 12:58p
 * Updated in $/Avalanche/tools/TupperChain
 * Added Precompiled Header
 * Added Chain manager for checking plugin dependencies
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 2:19p
 * Updated in $/Avalanche/tools/TupperChain
 * Added Image plugin support, Image manager, and renamed the filer
 * classes to not confuse anyone
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/10/02    Time: 2:52p
 * Updated in $/Avalanche/tools/TupperChain
 * Configuration file working
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERCHAIN_CONFIG_MANAGER_H
#define TUPPERCHAIN_CONFIG_MANAGER_H

class FilterPlugin;
class TupperwareAggregate;
class ImageReaderOptionsManager;
class OptionOverrides;

class FilterPass
{
	TupString m_filterName;
	FilterOptions m_options;
	int m_pass;

public:
	FilterPass(void);
	FilterPass(const char *filterName,const FilterOptions &options,int pass);
	const char *GetFilterName(void) const;
	const FilterOptions &GetOptions(void) const;
	int GetPass(void) const;
};

class ConfigManager
{
	// these are not defined
	ConfigManager(void);
	ConfigManager &operator=(const ConfigManager &s); 
	ConfigManager(const ConfigManager &s);

	FilterOptions m_globalOptions;
	TupArray<FilterPass> m_filterPassArray;
	ImageReaderOptionsManager m_imageReaderOptionsManager;
public:
	ConfigManager(TupperwareAggregate *configAgg,OptionOverrides &optionOverrides);

	int GetNumPasses(void) const;
	const char *GetFilterName(int index) const;
	int GetFilterPass(int index) const;
	const FilterOptions &GetFilterOptions(int index) const;
   const FilterOptions &GetGlobalOptions(void) const;
	const ImageReaderOptionsManager &GetImagerOptionsManager(void) const;
};

#endif
