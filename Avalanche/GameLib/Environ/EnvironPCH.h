//============================================================================
//=
//= EnvironPCH.h - Precompiled headers
//=
//============================================================================

#ifndef __ENVIRON_PCH_H
#define __ENVIRON_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

// system

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdarg.h>

#ifdef WIN32
#include <typeinfo.h>
#endif //WIN32

// engine

#include "platform/CompilerControl.h"
#include "platform/systemi.h"
#include "Math/Vector.h"
#include "Layers/Debug.h"
#include "Game/Database/DataHelpers.h"
#include "Game/Managers/LightMgr.h"
#include "Game/Managers/ParticleMgr.h"
#include "Game/Misc/Bounds.h"
#include "Game/Misc/FastVars.h"
#include "EngineHelper/filestream.h"
#include "GameHelper/Message.h"
#include "GameHelper/loadsom.h"
#include "GameHelper/TableLookup.h"
#include "stage/AnimatedChain.h"
#include "stage/stage.h"
#include "stage/SurfaceType.h"
#include "collide/collsys.h"
#include "stage/EntityCategory.h"
#include "camera/camera.h"
#include "Audio/audiolib.h"
#include "linearAlgebra/rayseg.h"
#include "XML/XML.h"

#if defined(GCN) && defined(CW)
#include "platform/GCN/GCNMWPort.h"
#endif

#endif // defined(WIN32) || defined(CW_PCH)
#endif // __ENVIRON_PCH_H
