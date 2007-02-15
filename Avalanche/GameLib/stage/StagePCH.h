//============================================================================
//=
//= StagePCH.h - Precompiled headers
//=
//============================================================================

#ifndef __STAGE_PCH_H
#define __STAGE_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

#define SINGLE_MESH	// for Game/Database/MeshList.h

// system

#include <string.h>
#include <stdio.h>
#include <new.h>

// engine

#include "platform/BaseType.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Layers/Memory.h"
#include "Layers/Assert/Assert.h"
#include "Layers/Debug.h"
#include "Game/Animation/Bone.h"
#include "Game/Animation/BoneGroup.h"
#include "Game/Animation/Motion.h"
#include "Game/Animation/MotionChannel.h"
#include "Game/Database/Som.h"
#include "Game/Database/DataHelpers.h"
#include "Game/Database/DataTypes.h"
#include "Game/Database/MeshList.h"
#include "Game/Database/lightset.h"
#include "Game/Managers/DistortionMgr.h"
#include "Game/Managers/AttachMgr.h"
#include "Display/Common/PatchDisp.h"
#include "EngineHelper/drawutil.h"
#include "EngineHelper/filestream.h"
#include "GameHelper/loadsom.h"
#include "GameHelper/TableToken.h"
#include "GameHelper/Message.h"
#include "collide/collsys.h"
#include "collide/obb.h"
#include "collide/Object.h"
#include "CollideHelper/obbchild.h"
#include "linearAlgebra/rayseg.h"
#include "linearAlgebra/cquat.h"
#include "mathUtil/mathcons.h"
#include "profile/codetimer.h"
#include "WatchClient/WatchClient.h"
#include "EngineHelper/Singleton.h"
#include "Network/Session.h"
#include "Network/NetActor.h"

#if defined(GCN) && defined(CW)
#include "platform/GCN/GCNMWPort.h"
#endif

#include "stage/AnimatedChain.h"
#include "stage/cactor.h"
#include "stage/CCompActor.h"
#include "stage/cast.h"
#include "stage/castmember.h"
#include "stage/CastMemberPool.h"
#include "stage/CastProfile.h"
#include "stage/celist.h"
#include "stage/cluster.h"
#include "stage/clustere.h"
#include "stage/clustersys.h"
#include "stage/DynamicChain.h"
#include "stage/MacroChain.h"
#include "stage/protagonist.h"
#include "stage/scatter.h"
#include "stage/stage.h"
#include "stage/StageProfile.h"
#include "stage/SurfaceLookup.h"
#include "stage/SurfaceType.h"

#ifdef ENABLE_RECORD
#include "stage/record.h"
#endif //ENABLE_RECORD

#endif // defined(WIN32) || defined(CW_PCH)
#endif // __STAGE_PCH_H
