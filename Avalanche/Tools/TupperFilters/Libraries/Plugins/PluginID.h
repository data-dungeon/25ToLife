////////////////////////////////////////////////////////////////////////////
//
// PluginID
//
// Base class for ids for filters
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: PluginID.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 1/10/02    Time: 2:52p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Configuration file working
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/09/02    Time: 4:36p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * skeleton filter chain complete
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_PLUGIN_ID_H
#define TUPPERFILTERS_LIB_PLUGINS_PLUGIN_ID_H

class PluginID
{
	ULONG a,b;
public:

	PluginID() { a = b = 0xffffffff; }
	PluginID(const PluginID& cid) { a = cid.a; b = cid.b;	}
	PluginID(ULONG aa, ULONG bb) { a = aa; b = bb; }
	ULONG PartA() const { return a; }
	ULONG PartB() const { return b; }
	void SetPartA( ULONG aa ) { a = aa; }
	void SetPartB( ULONG bb ) { b = bb; }
	int operator==(const PluginID& cid) const { return (a==cid.a&&b==cid.b); }
	int operator!=(const PluginID& cid) const { return (a!=cid.a||b!=cid.b); }
	PluginID& operator=(const PluginID& cid)  { a=cid.a; b = cid.b; return (*this); }
};

#endif
