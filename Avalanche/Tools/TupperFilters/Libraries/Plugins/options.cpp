////////////////////////////////////////////////////////////////////////////
//
// Options
//
// Class to handle options for plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: options.cpp $
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

#include "PluginsPch.h"
#include "options.h"

// FilterOption methods
FilterOption::FilterOption(void)
{
}

FilterOption::FilterOption(const char *key,const char *value) : m_key(key) , m_value(value)
{
}

const char *FilterOption::GetKey(void) const
{
	return m_key;
}
	
const char *FilterOption::GetValue(void) const
{
	return m_value;
}

void FilterOption::SetKey(const char *key)
{
	m_key = key;
}

void FilterOption::SetValue(const char *value)
{
	m_value = value;
}

// FilterOptions methods
FilterOptions::FilterOptions(void)
{
}

FilterOptions::FilterOptions(const FilterOptions &s)
{
	m_optionArray = s.m_optionArray;
}

FilterOptions &FilterOptions::operator=(const FilterOptions &s)
{
	m_optionArray = s.m_optionArray;
	return (*this);
}

void FilterOptions::Copy(const FilterOptions &s)
{
	int total = s.GetNumOptions();
	for (int i=0;i<total;i++)
	{
		const FilterOption &option = s.GetOption(i);
		AddOption(s.GetOption(i));
	}
}

void FilterOptions::AddOption(const FilterOption &option)
{
	int total = m_optionArray.GetSize();
	for (int i=0;i<total;i++)
	{
		FilterOption &entry = m_optionArray[i];
		// replace entry if the key matches
		if (!strcmp(option.GetKey(),entry.GetKey()))
		{
			entry = option;
			return;
		}
	}
	// if we finished we must not have replace one
	m_optionArray.Add(option);
}

void FilterOptions::AddOption(const char *key,const char *value)
{
	FilterOption option(key,value);
	AddOption(option);
}

int FilterOptions::GetNumOptions(void) const
{
	return m_optionArray.GetSize();
}

const FilterOption &FilterOptions::GetOption(int index) const
{
	assert(index>=0 && index<m_optionArray.GetSize());
	return m_optionArray[index];
}

const char *FilterOptions::GetKey(int index) const
{
	assert(index>=0 && index<m_optionArray.GetSize());
	return m_optionArray[index].GetKey();
}

const char *FilterOptions::GetValue(int index) const
{
	assert(index>=0 && index<m_optionArray.GetSize());
	return m_optionArray[index].GetValue();
}

const char *FilterOptions::GetValue(const char *key) const
{
	int total = m_optionArray.GetSize();
	for (int i=0;i<total;i++)
	{
		const FilterOption &entry = m_optionArray[i];
		// replace entry if the key matches
		if (!strcmp(entry.GetKey(),key))
		{
			return entry.GetValue();
		}
	}
	return NULL;
}

bool FilterOptions::RemoveOption(const char *key)
{
	int total = m_optionArray.GetSize();
	for (int i=0;i<total;i++)
	{
		FilterOption &entry = m_optionArray[i];
		// replace entry if the key matches
		if (!strcmp(entry.GetKey(),key))
		{
			m_optionArray.RemoveAt(i);
			return true;
		}
	}
	return false;	
}

bool FilterOptions::RemoveOption(int index)
{
	assert(index>=0 && index<m_optionArray.GetSize());
	m_optionArray.RemoveAt(index);
	return true;
};
