#pragma warning(disable:4786)

//#include "max.h"
#include <windows.h>
#include <windowsx.h>
//#include <ctl3d.h>
#include <commctrl.h>

#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

// Some standard library includes
#include <stdio.h>
#include <math.h>
#include <time.h>

#include <Tupperware/Aggregate.h>
#include <Tupperware/Scalar.h>
#include <Tupperware/List.h>
#include <Tupperware/Iterator.h>
#include <Tupperware/TupArray.h>
#include <Tupperware/TupString.h>
#include <Tupperware/Keeper.h>

#include <assert.h>

#ifndef STRICT
#define STRICT
#endif
#include <windows.h>
#include <tchar.h>

#include "resource.h"

// attrcontrols
#include <plugins/libraries/attrcontrols/debugprint.h>
#include <plugins/libraries/attrcontrols/CustomControls.h>
#include <plugins/libraries/attrcontrols/ICustomControl.h>
#include <plugins/libraries/attrcontrols/ICustomEditBox.h>
#include <plugins/libraries/attrcontrols/ICustomSpinner.h>
#include <plugins/libraries/attrcontrols/ICustomSlider.h>
#include <plugins/libraries/attrcontrols/ICustomToolTip.h>

