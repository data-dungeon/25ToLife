////////////////////////////////////////////////////////////////////////////
//
// Revision
//
// Base class for ids for filters
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: revision.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 12:57p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Changed Plugin Class (only process pass)
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/09/02    Time: 4:36p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * skeleton filter chain complete
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_REVISION_H
#define TUPPERFILTERS_LIB_PLUGINS_REVISION_H

// Version value that signals old plugins
#define VERSION_FILTER_CHAIN 0x00010000

class Revision
{
	int m_major,m_minor;
public:

	// constructors
	Revision() { m_major = m_minor = 0; }
	Revision(int major,int minor) :  m_major(major), m_minor(minor) {}
	Revision(const Revision& s) { m_major = s.m_major; m_minor = s.m_minor; }

	// operators
	Revision& operator=(const Revision& s)  { m_major = s.m_major; m_minor = s.m_minor; return (*this); }
	bool operator==(const Revision& s) const { return (m_major==s.m_major && m_minor==s.m_minor); }
	bool operator!=(const Revision& s) const { return (m_major!=s.m_major || m_minor!=s.m_minor); }
	bool operator<(const Revision& s) const
	{
		if (m_major<s.m_major || (m_major==s.m_major && m_minor<s.m_minor))
			return true;
		return false;
	}
	bool operator<=(const Revision& s) const
	{
		if (m_major<s.m_major || (m_major==s.m_major && m_minor<=s.m_minor))
			return true;
		return false;
	}
	bool operator>(const Revision& s) const
	{
		if (m_major>s.m_major || (m_major==s.m_major && m_minor>s.m_minor))
			return true;
		return false;
	}
	bool operator>=(const Revision& s) const
	{
		if (m_major>s.m_major || (m_major==s.m_major && m_minor>=s.m_minor))
			return true;
		return false;
	}

	// get methods
	int GetMajor(void) { return m_major; }
	int GetMinor(void) { return m_minor; }

	// set methods
	void SetMajor( int major ) { m_major = major; }
	void SetMinor( int minor ) { m_minor = minor; }

};


#endif
