////////////////////////////////////////////////////////////////////////////
//
// CollagePch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: CollagePch.h $
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 3/19/03    Time: 2:55p
 * Updated in $/Avalanche/tools/TupperFilters/Collage
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:34p
 * Updated in $/Avalanche/tools/TupperFilters/Collage
 * File Dependency checks
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 7/29/02    Time: 3:12p
 * Updated in $/Avalanche/tools/TupperFilters/Collage
 * Image Reader Options available to Image Reader Plugins
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 7/25/02    Time: 4:43p
 * Updated in $/Avalanche/tools/TupperFilters/Collage
 * First phase for texture animation
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 4/15/02    Time: 4:37p
 * Updated in $/Avalanche/tools/TupperFilters/Collage
 * Works in hybrid configuration
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 4/15/02    Time: 1:13p
 * Updated in $/Avalanche/tools/TupperFilters/Collage
 * Support 8 bit textures complete
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 4/12/02    Time: 4:35p
 * Updated in $/Avalanche/tools/TupperFilters/Collage
 * Provide dummy texture when it can't find the desired one
 * Support various formats on load (first pass)
 * work with the latest version of paintlib
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/Collage
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/19/02    Time: 5:08p
 * Updated in $/Avalanche/tools/TupperFilters/Collage
 * First stage of gui app complete
 * Log window working
 * Exceptions set up
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/14/02    Time: 11:22a
 * Updated in $/Avalanche/tools/TupperFilters/Collage
 * Puts 32 bpp bitmap data into the Tup tree
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 4:25p
 * Updated in $/Avalanche/tools/TupperFilters/Collage
 * Added Collage Image Reader
 * Uses Paintlib (16 extensions)
 * Can read file and add width and height to tup
 * More to come...
 */

#ifndef __COLLAGE_PCH__H
#define __COLLAGE_PCH__H

#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <io.h>

#include "tupperware/aggregate.h"
#include "tupperware/scalar.h"
#include "tupperware/list.h"
#include "tupperware/keeper.h"

#include "Tupper3d/ImageData.h"
#include "Tupper3d/ImageDataFrame.h"
#include "Tupper3d/ImageDataMap.h"
#include "Tupper3d/ImageDataPalette.h"

#include "TupperFilters/Libraries/Plugins/ImageReaderPluginDesc.h"
#include "TupperFilters/Libraries/Plugins/ImageReaderPlugin.h"
#include "TupperFilters/Libraries/Plugins/PluginError.h"
#include "TupperFilters/Libraries/Plugins/PluginLog.h"
#include "TupperFilters/Libraries/Plugins/AnimationListFile.h"
#include "TupperFilters/Libraries/Plugins/ImageAnalyze.h"
#include "TupperFilters/Libraries/Plugins/ImageUtilities.h"
#include "TupperFilters/Libraries/Plugins/Options.h"
#include "TupperFilters/Libraries/Plugins/FilePathArray.h"

#include "PaintLib/PaintLib/Common/planybmp.h"
#include "PaintLib/PaintLib/Common/planydec.h"
#include "PaintLib/PaintLib/Common/plbitmap.h"
#include "PaintLib/PaintLib/Common/config.h"
#include "PaintLib/PaintLib/Common/pldatasrc.h"
#include "PaintLib/PaintLib/Common/plexcept.h"
#include "PaintLib/PaintLib/Common/plpicdec.h"
#include "PaintLib/PaintLib/win/paintlib/plwinbmp.h"
#include "Paintlib/Paintlib/common/filter/PLFilterQuantize.h"

#endif
