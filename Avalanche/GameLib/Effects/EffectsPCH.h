//============================================================================
//=
//= EffectsPCH.h - Precompiled headers
//=
//============================================================================

#ifndef __EFFECTS_PCH_H
#define __EFFECTS_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

#define VTXBUFFER_NEED_INLINES

// system

#include <string.h>
#include <stdio.h>
#include <new.h>
#include <stdlib.h>
#include <math.h>

// engine

#include "platform/BaseType.h"
#include "platform/CompilerControl.h"
#include "Layers/Memory.h"
#include "Layers/Timing.h"
#include "EngineHelper/timer.h"
#include "Layers/Debug.h"
#include "Math/MathClass.h"
#include "Math/Vector.h"
#include "Display/Common/GfxFunc.h"
#include "Display/Common/RenderState.h"
#include "Display/Common/DisplayState.h"
#include "Display/Common/SceneView.h"
#include "Display/Common/TextureState.h"
#include "Display/Common/VtxBuffer.h"
#include "Display/Common/DisplayPlatform.h"
#include "path/PathCollection.h"
#include "path/SplinePath.h"
#include "collide/collsys.h"
#include "CollideHelper/LineEntity.h"
#include "CollideHelper/LineQuery.h"
#include "stage/EntityCategory.h"
#include "Game/Database/DBFile.h"
#include "Game/Managers/RenderMgr.h"
#include "Game/Managers/ParticleMgr.h"
#include "Game/Misc/FastVars.h"
#include "EngineHelper/drawutil.h"
#include "EngineHelper/filestream.h"
#include "profile/log.h"

#if defined(GCN) && defined(CW)
#include "platform/GCN/GCNMWPort.h"
#endif

#include "Effects/anihf.h"
#include "Effects/ahfmgr.h"
#include "Effects/ahfnode.h"
#include "Effects/canihf.h"
#include "Effects/collisionparticle.h"
#include "Effects/dropshad.h"
#include "Effects/fastnoise.h"
#include "Effects/footstep.h"
#include "Effects/halo.h"
#include "Effects/Halo/Halodbl.h"
#include "Effects/haloauto.h"
#include "Effects/haloele.h"
#include "Effects/leaf.h"
#include "Effects/leafblow.h"
#include "Effects/widget.h"
#include "Effects/lensele.h"
#include "Effects/lensflare.h"
#include "Effects/pnoise.h"
#include "Effects/siggen.h"
#include "Effects/simplerb.h"
#include "Effects/splinefog.h"
#include "Effects/sseffect.h"
#include "Effects/ssemgr.h"
//#include "effects/ttdisplay.h"
#include "Effects/ttemit.h"
#include "Effects/ttrack.h"
#include "Effects/ttread.h"
#include "Effects/ttsys.h"
#include "Effects/scenesetup.h"
#include "Effects/decal/decal.h"
#include "Effects/decal/staticdecal.h"
#include "Effects/sfx/sfxsys.h"

#endif // defined(WIN32) || defined(CW_PCH)
#endif // __EFFECTS_PCH_H
