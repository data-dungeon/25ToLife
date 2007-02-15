//============================================================================
//=
//= HudPCH.h - Precompiled headers
//=
//============================================================================

#ifndef __HUD_PCH_H
#define __HUD_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

// system

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#if defined (_XBOX)
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#endif

// engine

#include "Math/MathClass.h"
#include "Display/Common/DisplayState.h"
#include "Display/Common/TextureState.h"
#include "EngineHelper/filestream.h"
#include "Game/Database/DBFile.h"
#include "Layers/Media.h"
#include "Layers/Debug.h"
#include "Layers/Memory.h"
#include "Layers/LayersStruct.h"

#endif // defined(WIN32) || defined(CW_PCH)
#endif // __HUD_PCH_H
