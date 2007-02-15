////////////////////////////////////////////////////////////////////////////
//
// ConvertObjectsPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ConvertObjectsPch.h $
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 7/29/03    Time: 2:58p
 * Created in $/Avalanche/tools/TupperFilters/ConvertObjects
 */

#ifndef __CONVERT_OBJECTS_PCH__H
#define __CONVERT_OBJECTS_PCH__H

#include <windows.h>
#include <stdio.h>
#include <assert.h>

// tupperware general
#include <tupperware/aggregate.h>
#include <tupperware/list.h>
#include <tupperware/scalar.h>
#include <tupperware/loadsave.h>
#include <tupperware/TupArray.h>
#include <tupperware/TupString.h>
#include <tupperware/TupSort.h>
#include <Tupperware/keeper.h>

// image data (for image reader)
#include <Tupper3d/imageData.h>
#include <Tupper3d/imageDataFrame.h>
#include <Tupper3d/imageDataMap.h>
#include <Tupper3d/imageDataPalette.h>

// DBLUtils library
#include <TupperFilters/Libraries/DBLUtils/DBLUtil.h>
#include <TupperFilters/Libraries/DBLUtils/DBLWrite.h>
#include <TupperFilters/Libraries/DBLUtils/DBLSwap.h>
#include <TupperFilters/Libraries/DBLUtils/DBLDebug.h>

// Plugins library
#include <TupperFilters/Libraries/Plugins/FilterPluginDesc.h>
#include <TupperFilters/Libraries/Plugins/FilterPlugin.h>
#include <TupperFilters/Libraries/Plugins/Options.h>
#include <TupperFilters/Libraries/Plugins/DataManager.h>
#include <TupperFilters/Libraries/Plugins/filepath.h>
#include <TupperFilters/Libraries/Plugins/filepatharray.h>
#include <TupperFilters/Libraries/Plugins/PluginError.h>
#include <TupperFilters/Libraries/Plugins/PluginLog.h>
#include <TupperFilters/Libraries/Plugins/ImageManager.h>
#include <TupperFilters/Libraries/Plugins/ImageUtilities.h>
#include <TupperFilters/Libraries/Plugins/annotatehelper.h>
#include <TupperFilters/Libraries/Plugins/TokenizeString.h>

// DBL game headers
#include <Platform/BaseType.h>
#include <Display/Common/TextureState.h>
#include <Display/Common/RenderState.h>
#include <Game/Managers/TextureMgr/TextureDbl.h>

#endif
