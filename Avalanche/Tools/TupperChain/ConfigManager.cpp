////////////////////////////////////////////////////////////////////////////
//
// ConfigManager
//
// Class handle the configuration file
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ConfigManager.cpp $
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

#include "TupperChainPch.h"
#include "ConfigManager.h"
#include "OptionOverrides.h"

// FilterPass methods
FilterPass::FilterPass(void)
{
}

FilterPass::FilterPass(const char *filterName,const FilterOptions &options,int pass) :
	m_filterName(filterName),m_options(options),m_pass(pass)
{
}

const char *FilterPass::GetFilterName(void) const
{
	return m_filterName;
}

const FilterOptions &FilterPass::GetOptions(void) const
{
	return m_options;
}

int FilterPass::GetPass(void) const
{
	return m_pass;
}

// ConfigManager methods
ConfigManager::ConfigManager(TupperwareAggregate *configAgg,OptionOverrides &optionOverrides) : m_imageReaderOptionsManager(m_globalOptions)
{
	int pass=1; // default pass value

	for (TupperwareIterator i=configAgg->FindAll();!i.Finished();i.Next())
	{
		TupperwareAtom *pAtom = i.GetAsAtom();
		const char *pKey = pAtom->GetKey();
		// if this is a plugin
		if (!stricmp(pKey,"FILTER"))
		{
			const char *pName = pAtom->GetName();
			if (pName && strlen(pName) && pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
			{
				// start with the global options that have been defined thus far
				FilterOptions localOptions(m_globalOptions);
				// override any of these options with the global overrides
				optionOverrides.OverrideGlobalOptions(localOptions);

				int localPass(pass);
				TupperwareAggregate *pFilterAgg = (TupperwareAggregate *)pAtom;

				for (TupperwareIterator locali=pFilterAgg->FindAll();!locali.Finished();locali.Next())
				{
					TupperwareAtom *pAtom = locali.GetAsAtom();
					const char *pKey = pAtom->GetKey();
					// if this is an option
					if (!stricmp(pKey,"OPTION"))
					{
						const char *pName = pAtom->GetName();
						if (pName && strlen(pName) && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
						{
							TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
							if (pScalar->GetDataType()==TupperwareScalar::DATA_STRING)
							{
								const char *string = pScalar->GetAsString();
								if (strlen(string))
									localOptions.AddOption(pName,pScalar->GetAsString());
								else
									localOptions.RemoveOption(pName); // kill it if the string is empty
							}
						}
					}
					// if this is the pass keyword
					else if (!stricmp(pKey,"PASS"))
					{
						if (pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
						{
							TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
							if (pScalar->GetDataType()==TupperwareScalar::DATA_INT)
							{
								localPass = pScalar->GetAsInt();
							}
						}
					}
				}
				// override options at this point with the plugin override options
				optionOverrides.OverridePluginOptions(pName,localOptions);
				FilterPass filterPass(pName,localOptions,localPass);
				m_filterPassArray.Add(filterPass);
			}
			else
			{
				// TODO - Flag error due to wrong type of filter atom
			}
		}
		if (!stricmp(pKey,"IMAGE_READER"))
		{
			const char *pName = pAtom->GetName();
			if (pName && strlen(pName) && pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
			{
				// start with the global options that have been defined thus far
				FilterOptions localOptions(m_globalOptions);
				// override any of these options with the global overrides
				optionOverrides.OverrideGlobalOptions(localOptions);

				TupperwareAggregate *pFilterAgg = (TupperwareAggregate *)pAtom;

				for (TupperwareIterator locali=pFilterAgg->FindAll();!locali.Finished();locali.Next())
				{
					TupperwareAtom *pAtom = locali.GetAsAtom();
					const char *pKey = pAtom->GetKey();
					// if this is an option
					if (!stricmp(pKey,"OPTION"))
					{
						const char *pName = pAtom->GetName();
						if (pName && strlen(pName) && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
						{
							TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
							if (pScalar->GetDataType()==TupperwareScalar::DATA_STRING)
							{
								const char *string = pScalar->GetAsString();
								if (strlen(string))
									localOptions.AddOption(pName,pScalar->GetAsString());
								else
									localOptions.RemoveOption(pName); // kill it if the string is empty
							}
						}
					}
				}
				// override options at this point with the plugin override options
				optionOverrides.OverridePluginOptions(pName,localOptions);

				m_imageReaderOptionsManager.AddOptions(pName,localOptions);
			}
			else
			{
				// TODO - Flag error due to wrong type of filter atom
			}
		}
		// if this is an option
		else if (!stricmp(pKey,"OPTION"))
		{
			const char *pName = pAtom->GetName();
			if (pName && strlen(pName) && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
			{
				TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
				if (pScalar->GetDataType()==TupperwareScalar::DATA_STRING)
				{
					const char *string = pScalar->GetAsString();
					if (strlen(string))
						m_globalOptions.AddOption(pName,pScalar->GetAsString());
					else
						m_globalOptions.RemoveOption(pName); // kill it if the string is empty
				}
			}
		}
		// if this is the pass keyword
		else if (!stricmp(pKey,"PASS"))
		{
			if (pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
			{
				TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
				if (pScalar->GetDataType()==TupperwareScalar::DATA_INT)
				{
					pass = pScalar->GetAsInt();
				}
			}
		}
	}
	// override options at this point with the global override options
	optionOverrides.OverrideGlobalOptions(m_globalOptions);
}

int ConfigManager::GetNumPasses(void) const
{
	return m_filterPassArray.GetSize();
}

const char *ConfigManager::GetFilterName(int index) const
{
	assert(index>=0 && index<m_filterPassArray.GetSize());
	return m_filterPassArray[index].GetFilterName();
}

int ConfigManager::GetFilterPass(int index) const
{
	assert(index>=0 && index<m_filterPassArray.GetSize());
	return m_filterPassArray[index].GetPass();
}

const FilterOptions &ConfigManager::GetFilterOptions(int index) const
{
	assert(index>=0 && index<m_filterPassArray.GetSize());
	return m_filterPassArray[index].GetOptions();
}

const FilterOptions &ConfigManager::GetGlobalOptions(void) const
{
   return m_globalOptions;
}

const ImageReaderOptionsManager &ConfigManager::GetImagerOptionsManager(void) const
{
	return m_imageReaderOptionsManager;
}
