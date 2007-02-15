// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <direct.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <iostream>
#include <tchar.h>
#include <math.h>
#include <stdarg.h>
#include <conio.h>

// tupperware general
#include <tupperware/aggregate.h>
#include <tupperware/list.h>
#include <tupperware/scalar.h>
#include <tupperware/loadsave.h>
#include <tupperware/TupArray.h>
#include <tupperware/TupString.h>
#include <tupperware/TupSort.h>
#include <Tupperware/keeper.h>

// Game Engine files
#include <Game/Database/DBFile.h>

#include "Exception.h"

extern const char *g_pDXDDate;
extern const char *g_pDXDTime;
extern const char *g_pDXDVersion;
