//============================================================================
//=
//= GamePCH.h - Precompiled header file for the Game project.
//=
//============================================================================

#ifndef _GAME_PCH_H
#define _GAME_PCH_H

//============================================================================

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

#define VTXBUFFER_NEED_INLINES	// for Display/Common/VtxBuffer.h

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

#ifdef WIN32
	#include <float.h>	// for possible fpu exception enabling through _control87
#endif //WIN32

#ifdef PS2
	#include <eetypes.h>
	#include <eestruct.h>
#endif //PS2

#ifdef GCN
	#include <Dolphin/os.h>
	#include <dolphin/gx.h>
#endif //GCN

#include "platform/CompilerControl.h"
#include "platform/BaseType.h"
#include "Math/MathClass.h"
#include "Math/Quatern.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Math/BuildView.h"
#include "Layers/Debug.h"
#include "Layers/DLinkList.h"
#include "Layers/LayersState.h"
#include "Layers/Media.h"
#include "Layers/Memory.h"
#include "Layers/SLinkList.h"
#include "Layers/Timing.h"
#include "Display/Common/DisplayPlatform.h"
#include "Display/Common/DisplayState.h"
#include "Display/Common/Draw.h"
#include "Display/Common/DrawArray.h"
#include "Display/Common/GfxFunc.h"
#include "Display/Common/MatrixStack.h"
#include "Display/Common/PatchDisp.h"
#include "Display/Common/RenderMisc.h"
#include "Display/Common/RenderState.h"
#include "Display/Common/SceneView.h"
#include "Display/Common/StripBuffer.h"
#include "Display/Common/TextureState.h"
#include "Display/Common/VtxBuffer.h"
#include "GameHelper/loadsom.h"
#include "EngineHelper/timer.h"
#include "stage/cactor.h"
#include "stage/cast.h"
#include "stage/stage.h"
#include "WatchClient/WatchClient.h"
#include "container/Darray.h"
#include "linearAlgebra/rayseg.h"
#include "Game/Managers/ParticulateMgr/ParticulateMgr.h"
#ifdef DIRECTX
#include "Display/DirectX/DirectX.h"
#include "Game/Managers/ShadowMgr.h"
#endif //DIRECTX

#ifdef GCN
	#include "Display/GCN/GCN.h"
	#include "platform/GCN/GCNMWPort.h"
#endif	//GCN

#ifdef PS2
	#include "Layers/DMASom.h"
	#include "Display/PS2/PS2.h"
	#include "Display/PS2/Renderer/Code/PS2Renderer.h"
	#include "Display/PS2/GSState.h"		// for dma packet sizes
	#include "Game/Managers/ParticleMgr.h"			// for dma packet sizes
	#include "Game/Managers/SpriteMgr.h"			// for dma packet sizes
	#include "Display/PS2/Renderer/Code/MasterList.h"
	#include "Display/PS2/Renderer/Common/GIFTag.h"
	#include "Display/PS2/Renderer/Common/VIFInline.h"
	#include "Display/PS2/Renderer/Common/GIFInline.h"
	using namespace VIFInline;
	using namespace GIFInline;
#endif //PS2

#include "container/Darray.h"
#include "container/qsort.h"

#include "Game/Animation/ApplyKFM.h"
#include "Game/Animation/Bone.h"
#include "Game/Animation/BoneGroup.h"
#include "Game/Animation/Motion.h"
#include "Game/Animation/MotionChannel.h"
#include "Game/Animation/MotionDB.h"
#include "Game/Animation/MotCtrl.h"
#include "Game/Animation/attachp.h"
#include "Game/Database/Som.h"
#include "Game/Database/MeshInstance.h"
#include "Game/Database/MeshLOD.h"
#include "Game/Database/MeshList.h"
#include "Game/Database/MeshParam.h"
#include "Game/Database/MeshSOM.h"
#include "Game/Database/ChunkLoaders.h"
#include "Game/Database/DataHelpers.h"
#include "Game/Database/DataMgr.h"
#include "Game/Database/DataTypes.h"	
#include "Game/Database/DBFile.h"
#include "Game/Database/DynamicObject.h"
#include "Game/Database/Instance.h"
#include "Game/Database/LOD.h"
#include "Game/Database/SceneNode.h"
#include "Game/Database/VisRegion.h"
#include "Game/Database/lightset.h"
#include "Game/Database/World.h"
#include "Game/Managers/TextureMgr/TextureMgr.h"
#include "Game/Managers/AttachMgr.h"
#include "Game/Managers/AuraMgr.h"
#include "Game/Managers/DistortionMgr.h"
#include "Game/Managers/LightMgr.h"
#include "Game/Managers/MiscPools.h"
#include "Game/Managers/ParticleDB.h"
#include "Game/Managers/ParticleMgr.h"
#include "Game/Managers/ReflectionMgr.h"
#include "Game/Managers/RenderMgr.h"
#include "Game/Managers/SpriteMgr.h"
#include "Game/Managers/SwooshMgr.h"
#include "Game/Managers/IdManager.h"
#include "Game/Managers/replayMgr.h"
#include "Game/Misc/Bounds.h"
#include "Game/Misc/FastVars.h"
#include "Game/Misc/FontInfo.h"
#include "Game/Misc/Frustum.h"
#include "Game/Misc/GameEngine.h"
#include "Game/Misc/ParameterSet.h"

#endif //defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

//============================================================================

#endif //_GAME_PCH_H
