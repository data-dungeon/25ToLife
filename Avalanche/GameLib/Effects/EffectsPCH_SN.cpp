//============================================================================
//=
//= EffectsPCH_SN.cpp - Precompiled header CPP
//=
//============================================================================

// codewarrior flag
#ifdef CW
#define CW_PCH
#endif

#include "EffectsPCH.h"

#if defined(PS2) && defined(SN)

#include "ahfmgr.cpp"
#include "anihf.cpp"
#include "canihf.cpp"
#include "collisionparticle.cpp"
#include "dropshad.cpp"
#include "fastnoise.cpp"
#include "footstep.cpp"
#include "halo.cpp"
#include "haloele.cpp"
#include "leaf.cpp"
#include "leafblow.cpp"
#include "lensele.cpp"
#include "lensflare.cpp"
#include "bloom.cpp"
#include "pnoise.cpp"
#include "shatter.cpp"
#include "siggen.cpp"
#include "simplerb.cpp"
#include "splinefog.cpp"
#include "sseffect.cpp"
#include "ssemgr.cpp"
#include "tracer.cpp"
#include "dtracer.cpp"
#include "ttemit.cpp"
#include "ttrack.cpp"
#include "ttread.cpp"
#include "ttsys.cpp"
#include "scenesetup.cpp"
#include "fullscreendistort.cpp"
#include "worldspacedistort.cpp"
#include "widget.cpp"
#include "damageanim.cpp"
#include "Water/Water.cpp"
#include "Water/WaterCollide.cpp"
#include "Water/WaterEquation.cpp"
#include "Water/WaterMgr.cpp"
#include "Water/WaterPhysics.cpp"
#include "Water/WaterPool.cpp"
#include "Water/WaterRender.cpp"
#include "Water/WaterRenderPS2.cpp"
#include "Water/WaterSprite.cpp"
#include "Water/WaterSubdivision.cpp"
#include "Water/WaterUpdate.cpp"
#include "Water/WaterSpriteEffect.cpp"
#include "decal/decal.cpp"
#include "decal/adecal.cpp"
#include "decal/rdecal.cpp"
#include "decal/decalemit.cpp"
#include "decal/staticdecal.cpp"
#include "decal/fadedecal.cpp"
#include "decal/decalset.cpp"
#include "sfx/sfxsys.cpp"
#include "sfx/specialfx.cpp"
#include "sfx/sfxpemit.cpp"
#include "sfx/sfxlight.cpp"
#include "sfx/sfxdistort.cpp"
#include "sfx/sfxtrigger.cpp"
#include "sfx/sfxfss.cpp"

#endif // defined(PS2) && defined(SN)
