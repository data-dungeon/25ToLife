////////////////////////////////////////////////////////////////////////////
//
// TupperAttribPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupperAttribPch.h $
 * 
 * *****************  Version 9  *****************
 * User: Dross        Date: 4/29/03    Time: 7:32p
 * Updated in $/Avalanche/tools/TupperAttrib
 * removed include of varargs.h is SN build
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 5/17/02    Time: 9:29a
 * Updated in $/Avalanche/tools/TupperAttrib
 * changed name of array and string files
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 5/15/02    Time: 2:21p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Alternate memory manager hooked in
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 5/09/02    Time: 3:37p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Working with CodeWarrior Compiler
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperAttrib
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 4:27p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Added access classes
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 12:11p
 * Updated in $/Avalanche/tools/TupperAttrib
 * TupperAttrib project first pass complete
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/05/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperAttrib
 * First pass of tupperattrib
 * Build tree from file
 * Get surface types from file
 */

#ifndef TUPPERATTRIB_PCH_H
#define TUPPERATTRIB_PCH_H

#include <Assert.h>
#include <stdio.h>

#ifdef PS2
#if !defined(PS2) || !defined(SN)
#include <varargs.h>
#endif
#endif

// tupperware general
#include "Tupperware/Aggregate.h"
#include "Tupperware/List.h"
#include "Tupperware/Scalar.h"
#include "Tupperware/pool.h"
#include "Tupperware/tuparray.h"
#include "Tupperware/tupstring.h"
#include "Tupperware/memorymanager.h"
#include "Tupperware/keeper.h"

#endif
