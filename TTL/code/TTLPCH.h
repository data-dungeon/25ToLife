//============================================================================
//=
//= TTLPCH.h - Precompiled headers
//=
//============================================================================

#ifndef __TTL_PCH_H
#define __TTL_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

// system

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <new.h>

#if defined (_XBOX)
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#endif

// engine

#include "Math/MathClass.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "platform/BaseType.h"
#include "platform/gameapp.h"
#include "platform/memcard.h"
#include "platform/cfocus.h"
#include "platform/systemi.h"
#include "Layers/Assert/Assert.h"
#include "Layers/Debug.h"
#include "Layers/LayersState.h"
#include "Layers/Media.h"
#include "Layers/Memory.h"
#include "Layers/SLinkList.h"
#include "Layers/Tasking.h"
#include "Game/Animation/MotionDB.h"
#include "Game/Animation/MotionChannel.h"
#include "Game/Database/DBFile.h"
#include "Game/Database/MeshLOD.h"
#include "Game/Database/lightset.h"
#include "Game/Managers/AttachMgr.h"
#include "Game/Managers/DistortionMgr.h"
#include "Game/Managers/LightMgr.h"
#include "Game/Managers/ParticleMgr.h"
#include "Game/Managers/RenderMgr.h"
#include "Game/Managers/SpriteMgr.h"
#include "Game/Managers/IdManager.h"
#include "Game/Misc/FastVars.h"
#include "Game/Misc/GameEngine.h"
#include "Display/Common/DisplayPlatform.h"
#include "Display/Common/DisplayState.h"
#include "Display/Common/Draw.h"
#include "Display/Common/GfxFunc.h"
#include "Display/Common/MatrixStack.h"
#include "Display/Common/PatchDisp.h"
#include "Display/Common/RenderState.h"
#include "Display/Common/SceneView.h"
#include "Display/Common/TextureState.h"
#include "camera/director.h"
#include "camera/camera.h"
#include "camera/orbitcam/orbitcam.h"
#include "camera/targetcam/targetcam.h"
#include "EngineHelper/drawutil.h"
#include "EngineHelper/filestream.h"
#include "EngineHelper/timer.h"
#include "GameHelper/loadsom.h"
#include "GameHelper/lod.h"
#include "GameHelper/SimpleType.h"
#include "GameHelper/ParamMgr.h"
#include "GameHelper/Message.h"
#include "GameHelper/DebugOutput.h"
#include "Effects/ahfmgr.h"
#include "Effects/anihf.h"
#include "Effects/collisionparticle.h"
#include "Effects/footstep.h"
#include "Effects/widget.h"
#include "Effects/ttsys.h"
#include "stage/AnimatedChain.h"
#include "stage/cast.h"
#include "stage/cactor.h"
#include "stage/stage.h"
#include "stage/SurfaceType.h"
#include "linearAlgebra/rayseg.h"
#include "mathUtil/Ballistic.h"
#include "mathUtil/mathcons.h"
#include "path/PathCollection.h"
#include "collide/aabb.h"
#include "collide/collsys.h"
#include "CollideHelper/ObbEntity.h"
#include "CollideHelper/SphereEntity.h"
#include "CollideHelper/LineEntity.h"
#include "CollideHelper/XZEntity.h"
#include "CollideHelper/LineQuery.h"
#include "CollideHelper/EventMiner.h"
#include "CollideHelper/ObjectList.h"
#include "CollideHelper/obbchild.h"
#include "stage/EntityCategory.h"
#include "Audio/audiolib.h"
#include "path/SplinePath.h"
#include "WatchClient/WatchClient.h"
#include "XML/XML.h"
#include "Network/Session.h"
#include "Interfaces/Interface.h"
#include "Environ/ActorInitializer.h"
#include "Environ/World.h"
#include "Components/Component.h"

#include "hud/ttlhud.h"

#include "camera/shootercam.h"
#include "camera/ghostcam.h"
#include "camera/deathcam.h"

#if defined(PS2)
#include "Display/PS2/PS2.h"
#include "platform/PS2/ps2demo.h"
#endif

#if defined(GCN) && defined(CW)
#include "platform/GCN/GCNMWPort.h"
#endif

// game

#include "main/GameEnv.h"
#include "main/GameVersion.h"
#include "main/TTLConstants.h"
#include "main/Players.h"
#include "main/TTLMessages.h"
#include "main/PlayerProfile.h"
#include "main/cheats.h"

#include "Effects/tracer.h"
#include "Effects/dtracer.h"
#include "Effects/decal/decal.h"
#include "effects/lasersight.h"
#include "effects/blood.h"
#include "effects/customtag.h"
#include "effects/modetag.h"
#include "effects/fsdsys.h"

#include "sim/GameEntityCategory.h"
#include "sim/GameElements.h"
#include "sim/Referee.h"

#include "fluff/sprite3d.h"

#include "avatar/avcontrolmap.h"
#include "avatar/avatarfx.h"

#include "weapons/ttlmunition.h"

#include "ActorsTTL/Combatant.h"

#endif // defined(WIN32) || defined(CW_PCH)
#endif // __TTL_PCH_H
