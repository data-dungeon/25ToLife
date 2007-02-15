////////////////////////////////////////////////////////////////////////////
//
// Tupper3dPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: Tupper3dPch.h $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 10:42a
 * Updated in $/Avalanche/tools/Tupper3d
 * moved tri mesh connectivity from plugins library
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 5/24/02    Time: 4:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * faster compile time (I don't know why)
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 5/17/02    Time: 9:31a
 * Updated in $/Avalanche/tools/Tupper3d
 * changed name of array and string files
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/Tupper3d
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 */

#ifndef TUPPER3D_TUPPER3D_PCH_H
#define TUPPER3D_TUPPER3D_PCH_H

#pragma warning (disable: 4786)

#include <windows.h>
#include <stdio.h>
#include <assert.h>

// tupperware general
#include <tupperware/aggregate.h>
#include <tupperware/list.h>
#include <tupperware/scalar.h>
#include <tupperware/loadsave.h>
#include <tupperware/pool.h>
#include <tupperware/tuparray.h>
#include <tupperware/tupstring.h>

#endif
