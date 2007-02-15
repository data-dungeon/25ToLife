//============================================================================
//=
//= PlatformPCH.h - Precompiled headers
//=
//============================================================================

#ifndef __PLATFORM_PCH_H
#define __PLATFORM_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

// system

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#if defined  (WIN32) && !defined (_XBOX)
#include <windows.h>
#include <windowsx.h>
#include <zmouse.h>
#include <direct.h>
#endif

#if defined (_XBOX)
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#endif 

#ifdef PS2
// OS dependent header
#include <eetypes.h>
#include <libpad.h>
#include <libpc.h>
#include <libmc.h>
#include <sifrpc.h>
#include <sifcmd.h>
#include <eekernel.h>
#include <eeregs.h>
#include <sifdev.h>
#include <devvu0.h>
#include <devvu1.h>
// STANDARD header
#include <stdlib.h>
#include <string.h>
#ifdef CDROM
#include <libcdvd.h>
#endif //CDROM
#endif //PS2

#ifdef GCN
#include <dolphin.h>
#endif //GCN

// engine

#include "Math/MathClass.h"
#include "WatchClient/WatchClient.h"
#include "Layers/Memory.h"
#include "Layers/LayersState.h"
#include "Layers/Debug.h"
#include "Layers/Media.h"
#include "Display/Common/DisplayState.h"
#include "Display/Common/GfxFunc.h"
#include "Display/Common/DisplayPlatform.h"
#include "Game/Misc/GameEngine.h"

// General includes
#include "WatchClient/WatchClient.h"
#include "profile/textwin.h"
#include "profile/codetimer.h"
#include "profile/log.h"

// PS2 Includes
#ifdef PS2
#include "Display/PS2/PS2.h"
#endif //PS2

// Windows specific includes
#if defined (WIN32) && !defined (_XBOX)
#include "platform/Win32/Winapp.h"
#include "platform/Win32/resource.h"
#include "profile/Win32/textscrw32.h"
#endif //WIN32

#include "platform/game.h"
#include "platform/ctroller.h"
#include "platform/gameapp.h"
#include "platform/keyctrl.h"
#include "platform/memcard.h"
#include "platform/systemi.h"
#include "platform/cfocus.h"
#include "platform/ctrolmap.h"
#include "platform/defcontrolmap.h"

#if defined (WIN32) && !defined (_XBOX)
#include "platform/Win32/wincard.h"
#include "platform/Win32/winccd.h"
#include "platform/Win32/conrcids.h"
#include "platform/Win32/winctrol.h"
#include "platform/Win32/wininput.h"
#endif //WIN32

#if defined (_XBOX)
#include "platform/Xbox/xboxcard.h"
#include "platform/Xbox/xboxccd.h"
#include "platform/Xbox/xboxctrol.h"
#include "platform/Xbox/xboxinput.h"
#endif //XBOX

#ifdef PS2
#include "platform/PS2/mwUtils.h"
#include "platform/PS2/mwUtils_PS2.h"
#include "platform/PS2/PS2app.h"
#include "platform/PS2/ps2demo.h"
#include "platform/PS2/ps2card.h"
#include "platform/PS2/ps2ccd.h"
#include "platform/PS2/ps2ctrol.h"
#include "platform/PS2/ps2input.h"
#endif //PS2

#ifdef GCN
#include "platform/GCN/gcnapp.h"
#include "platform/GCN/gcncard.h"
#include "platform/GCN/gcnccd.h"
#include "platform/GCN/gcnctrol.h"
#include "platform/GCN/gcninput.h"
#include "platform/GCN/GCNMWPort.h"
#endif //GCN

#endif // defined(WIN32) || defined(CW_PCH)
#endif // __PLATFORM_PCH_H
