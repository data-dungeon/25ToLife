#ifndef _PCH_H_
#define _PCH_H_

//////////////////////////////////
// mfc includes

#define OEMRESOURCE

#include <afxwin.h>
#include <afxadv.h>
#include <afxcmn.h>
#include <afxdisp.h>
#include <afxdlgs.h>
#include <afxext.h>

//////////////////////////////////
// windows shell32 includes

#include <shlobj.h>
#include <shlwapi.h>

//////////////////////////////////
// directx includes

#include <d3d9.h>
#include <d3dx9.h>

//////////////////////////////////
// std library includes

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//////////////////////////////////
// game engine includes

#include <hud\hudlib.h>

//////////////////////////////////
// my custom libraries

#include <libwin32.h>
#include <libmfc.h>

//////////////////////////////////
// resource includes

#include "resource.h"

#endif // _PCH_H_