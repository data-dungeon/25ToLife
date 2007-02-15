/*************************************/
/*                                   */
/*   FMVPCH.h                 */
/*************************************/

#ifndef __FMV_PCH_H
#define __FMV_PCH_H

/* this file must be effectively empty for non-visual-studio builds, except for when it is included by the codewarrior
.pch file that creates the pre-compiled header file */

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

/******************* includes ****************************/

/* system includes */

#include <string.h>
#include <stdlib.h>

#ifdef WIN32
#include <typeinfo.h>
#endif //WIN32

/* engine includes */

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

#include "fmv/fmv.h"



#endif // defined(WIN32) || defined(CW_PCH)
#endif // __FMV_PCH_H
