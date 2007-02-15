//============================================================================
//=
//= PathPCH.h - Precompiled headers
//=
//============================================================================

#ifndef __PATH_PCH_H
#define __PATH_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

// system

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#if defined(PS2) || defined(GCN)
#include "ctype.h"
#endif

// engine

#include "Layers/Debug.h"
#include "Display/Common/Draw.h"
#include "Display/Common/MatrixStack.h"
#include "Display/Common/RenderState.h"
#include "Display/Common/TextureState.h"
#include "EngineHelper/drawutil.h"
#include "path/Path.h"
#include "path/PathCollection.h"
#include "path/SegmentPath.h"
#include "path/ShortcutPath.h"
#include "path/SplinePath.h"

#endif // defined(WIN32) || defined(CW_PCH)
#endif // __PATH_PCH_H
