//============================================================================
//=
//= GamePCH_SN.cpp - Precompiled header CPP
//=
//============================================================================

#include "GamePCH.h"

#if defined(PS2) && defined(SN)

#include "Misc/Bounds.cpp"
#include "Misc/FastVars.cpp"
#include "Misc/fontinfo.cpp"
#include "Misc/Frustum.cpp"
#include "Misc/GameEngine.cpp"
#include "Database/DataHelpers.cpp"
#include "Database/DataMgr.cpp"
#include "Database/DBFile.cpp"
#include "Database/Instance.cpp"
#include "Database/lightset.cpp"
#include "Database/MeshInstance.cpp"
#include "Database/MeshList.cpp"
#include "Database/MeshLOD.cpp"
#include "Database/MeshParam.cpp"
#include "Database/mot_load.cpp"
#include "Database/SceneNode.cpp"
#include "Database/som_load.cpp"
#include "Database/VisRegion.cpp"
#include "Database/world_load.cpp"
#include "Managers/AttachMgr.cpp"
#include "Managers/AuraMgr.cpp"
#include "Managers/DistortionMgr.cpp"
#include "Managers/LightMgr.cpp"
#include "Managers/MiscPools.cpp"
#include "Managers/ParticleMgr.cpp"
#include "Managers/ParticulateMgr/ParticulateMgr.cpp"
#include "Managers/ParticulateMgr/ParticulateMgr_ps2.cpp"
#include "Managers/ParticulateMgr/ParticulateMgr_System.cpp"
#include "Managers/ParticulateMgr/ParticulateMgr_Effect.cpp"
#include "Managers/ParticulateMgr/ParticulateMgr_Emitter.cpp"
#ifndef CDROM
#include "Managers/ParticulateMgr/ParticulateMgr_Test.cpp"
#endif
#include "Managers/ReflectionMgr.cpp"
#include "Managers/RenderMgr.cpp"
#include "Managers/SceneMgr.cpp"
#include "Managers/ShadowMgr.cpp"
#include "Managers/SpriteMgr.cpp"
#include "Managers/SwooshMgr.cpp"
#include "Managers/idmanager.cpp"
#include "Managers/TextureMgr/TextureMgr.cpp"
#include "Managers/TextureMgr/TextureArray.cpp"
#include "Managers/TextureMgr/TextureDatabase.cpp"
#include "Managers/TextureMgr/Texture.cpp"
#include "Managers/TextureMgr/Image.cpp"
#include "Managers/TextureMgr/PS2/PlatformImage.cpp"
#include "Animation/ApplyKFM.cpp"
#include "Animation/bone.cpp"
#include "Animation/bonegroup.cpp"
#include "Animation/motion.cpp"
#include "Animation/motionchannel.cpp"

#endif // defined(PS2) && defined(SN)
