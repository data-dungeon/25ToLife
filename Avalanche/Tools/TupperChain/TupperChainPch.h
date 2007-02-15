////////////////////////////////////////////////////////////////////////////
//
// TupperChainPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupperChainPch.h $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 7/29/02    Time: 3:12p
 * Updated in $/Avalanche/tools/TupperChain
 * Image Reader Options available to Image Reader Plugins
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 7/16/02    Time: 12:47p
 * Updated in $/Avalanche/tools/TupperChain
 * Improved path code - plugin path is specified
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 5/17/02    Time: 11:38a
 * Updated in $/Avalanche/tools/TupperChain
 * renamed tupperware string and array
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperChain
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 12:58p
 * Updated in $/Avalanche/tools/TupperChain
 * Added Precompiled Header
 * Added Chain manager for checking plugin dependencies
 */

#ifndef TUPPERCHAIN_PCH_H
#define TUPPERCHAIN_PCH_H

#include <windows.h>
#include <assert.h>

#include "tupperware/aggregate.h"
#include "tupperware/scalar.h"
#include "tupperware/tuparray.h"
#include "tupperware/tupstring.h"

#include "TupperFilters/Libraries/Plugins/Options.h"
#include "TupperFilters/Libraries/Plugins/FilterPluginDesc.h"
#include "TupperFilters/Libraries/Plugins/FilterPlugin.h"
#include "TupperFilters/Libraries/Plugins/revision.h"
#include "TupperFilters/Libraries/Plugins/pluginerror.h"
#include "TupperFilters/Libraries/Plugins/pluginlog.h"
#include "TupperFilters/Libraries/Plugins/filepath.h"
#include "TupperFilters/Libraries/Plugins/ImageReaderOptions.h"


#endif
