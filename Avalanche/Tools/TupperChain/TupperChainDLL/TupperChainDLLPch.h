////////////////////////////////////////////////////////////////////////////
//
// TupperChainDLLPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __TUPPERCHAINDLL_PCH__H
#define __TUPPERCHAINDLL_PCH__H

#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <direct.h>
#include <io.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "tupperware/aggregate.h"
#include "tupperware/loadsave.h"
#include "tupperware/tuparray.h"
#include "tupperware/tupstring.h"
#include "tupperware/keeper.h"

#include "TupperFilters/Libraries/Plugins/FilterPlugin.h"
#include "TupperFilters/Libraries/Plugins/Options.h"
#include "TupperFilters/Libraries/Plugins/DataManager.h"
#include "TupperFilters/Libraries/Plugins/imageManager.h"
#include "TupperFilters/Libraries/Plugins/PluginError.h"
#include "TupperFilters/Libraries/Plugins/PluginLog.h"
#include "TupperFilters/Libraries/Plugins/FilePath.h"
#include "TupperFilters/Libraries/Plugins/FilePathArray.h"
#include "TupperFilters/Libraries/Plugins/ImageReaderOptions.h"
#include "TupperFilters/Libraries/Plugins/TokenizeString.h"

#include "TupperChain/FilterPluginManager.h"
#include "TupperChain/ConfigManager.h"
#include "TupperChain/OptionOverrides.h"

#endif
