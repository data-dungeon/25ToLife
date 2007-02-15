//============================================================================
//=
//= GameHelperPCH.h - Precompiled headers
//=
//============================================================================

#ifndef __GAMEHELPER_PCH_H
#define __GAMEHELPER_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

// system

#include <string.h>
#include <stdlib.h>

#ifdef WIN32
#include <typeinfo.h>
#endif //WIN32

// engine

#include "platform/BaseType.h"
#include "Layers/Memory.h"
#include "Layers/Debug.h"
#include "Game/Animation/MotionDB.h"
#include "Game/Database/DBFile.h"
#include "Game/Database/DataHelpers.h"
#include "Game/Managers/RenderMgr.h"
#include "Game/Misc/FastVars.h"
#include "Display/Common/TextureState.h"
#include "Display/Common/RenderState.h"
#include "collide/aabb.h"
#include "EngineHelper/drawutil.h"
#include "EngineHelper/timer.h"
#include "EngineHelper/filestream.h"
#include "collide/tritest.h"
#include "Math/MathClass.h"
#include "Math/Matrix.h"
#include "stage/stage.h"
#include "WatchClient/WatchClient.h"

#ifdef GCN
#include "Display/GCN/GCN.h"
#endif

#include "GameHelper/ctristrip.h"
#include "GameHelper/disphf.h"
#include "GameHelper/disphfn.h"
#include "GameHelper/loadsom.h"
#include "GameHelper/lod.h"
#include "GameHelper/SimpleType.h"
#include "GameHelper/TableLabel.h"
#include "GameHelper/TableLookup.h"
#include "GameHelper/TableToken.h"
#include "GameHelper/actorspeech.h"

#endif // defined(WIN32) || defined(CW_PCH)
#endif // __GAMEHELPER_PCH_H
