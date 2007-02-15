//============================================================================
//=
//= LayersPCH.h - Precompiled headers
//=
//============================================================================

#ifndef __LAYERS_PCH_H
#define __LAYERS_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

// system

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <ctype.h>

#ifdef GCN
#include <dolphin/dvd.h>
#include <Dolphin/types.h>
#include <Dolphin/os.h>
#include "platform/GCN/GCNMWPort.h"
#include <new>
#endif //GCN

#ifndef GCN
#include <malloc.h>
#endif //GCN

#ifdef PS2
#include <sifdev.h>
#include <eeregs.h>
#include <libpc.h>
#include <libcdvd.h>
#endif //PS2

#ifdef WIN32
#ifdef _XBOX
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#else
#include <windows.h>
#endif
#include <Time.h>
#endif //WIN32

#ifdef PS2
#include "Layers/mstream/mstream.h"
#endif

// engine

#include "platform/BaseType.h"
#include "Layers/Assert/Assert.h"
#include "container/dllist.h"
#include "Math/MathClass.h"
#include "Layers/Context.h"
#include "Layers/Memory.h"
#include "Layers/Debug.h"
#include "Layers/SLinkList.h"
#include "Layers/DLinkList.h"
#include "Layers/hog.h"
#include "Layers/Media.h"
#include "Layers/Joystick.h"
#include "Layers/LayersState.h"
#include "Layers/Timing.h"
#include "Layers/Stack.h"
#include "Layers/Tasking.h"
#include "Layers/mediahal.h"
#include "Layers/loadprofile.h"

#endif // defined(WIN32) || defined(CW_PCH)
#endif // __LAYERS_PCH_H
