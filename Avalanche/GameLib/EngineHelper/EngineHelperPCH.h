//============================================================================
//=
//= EngineHelper.h - Precompiled headers
//=
//============================================================================

#ifndef __ENGINEHELPER_PCH_H
#define __ENGINEHELPER_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

// system

#include <string.h>
#include <stdarg.h>
#include <ctype.h>

// engine

#include "platform/BaseType.h"
#include "Layers/Memory.h"
#include "Layers/Debug.h"
#include "Layers/LayersStruct.h"
#include "Math/MathClass.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Display/Common/RenderMisc.h"
#include "Display/Common/PatchDisp.h"
#include "Game/Animation/Bone.h"
#include "Game/Database/Som.h"
#include "Game/Database/DataHelpers.h"
#include "Game/Database/Instance.h"
#include "Game/Database/LOD.h"
#include "Game/Database/World.h"
#include "Game/Database/DataMgr.h"
#include "Game/Database/lightset.h"
#include "Game/Managers/DistortionMgr.h"
#include "Game/Misc/Bounds.h"
#include "WatchClient/WatchClient.h"

#ifdef PS2
#include "Display/PS2/Renderer/Code/PS2Renderer.h"
#endif //PS2

#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif //GCN

#include "EngineHelper/drawutil.h"
#include "EngineHelper/filestream.h"
#include "EngineHelper/hog.h"
#include "EngineHelper/Nestle.h"
#include "EngineHelper/pathfile.h"
#include "EngineHelper/sfx.h"
#include "EngineHelper/soundcollide.h"
#include "EngineHelper/simpleinst.h"

#endif // defined(WIN32) || defined(CW_PCH)
#endif // __ENGINEHELPER_PCH_H
