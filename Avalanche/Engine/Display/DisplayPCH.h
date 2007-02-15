//============================================================================
//=
//= DisplayPCH.h
//=
//============================================================================

#ifndef __DISPLAY_PCH_H
#define __DISPLAY_PCH_H

#define DISPLAY_STATE_ACCESS		// get access to DisplayState record. for Display module files only. see comment in DisplayState.h
#define VTXBUFFER_NEED_INLINES	// for Display/Common/VtxBuffer.h

// system includes
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>

#ifdef GCN
//#include <malloc.h>
#include <dolphin.h>
#include <Dolphin/os.h>
#endif //GCN

#if defined (_XBOX)
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include <xgraphics.h>
#include "Math/MathDeprecate.h"
#endif //DIRECTX/XBOX

#ifdef PS2
#include <eeregs.h>
#include <eestruct.h>
#include <libvu0.h>
#endif //PS2

// engine includes
#include "platform/BaseType.h"
#include "Math/MathClass.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Layers/Debug.h"
#include "Layers/Memory.h"
#include "Layers/Assert/Assert.h"
#include "Game/Animation/Bone.h"
#include "Game/Database/Som.h"
#include "Game/Database/DBFile.h"
#include "Game/Database/Instance.h"
#include "Game/Database/World.h"
#include "Game/Database/lightset.h"
#include "Game/Managers/AuraMgr.h"
#include "Game/Managers/DistortionMgr.h"
#include "Game/Managers/LightMgr.h"
#include "Game/Managers/RenderMgr.h"
#include "Game/Managers/SpriteMgr.h"
#include "Game/Misc/FastVars.h"
#include "Game/Misc/Frustum.h"
#include "linearAlgebra/rayseg.h"
#include "stage/cactor.h"
#include "stage/protagonist.h"

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

#include "Game/Managers/TextureMgr/TextureMgr.h"
#include "Display/Common/TextureAnimatorArray.h"

#include "Display/Common/TextureAnimator.h"

#include "Display/Common/StripBuffer.h"
#include "Display/Common/TextureState.h"
#include "Display/Common/VtxBuffer.h"

#ifdef PS2
#include "Display/PS2/PS2.h"
#include "Display/PS2/Renderer/Code/PS2Renderer.h"
#include "Display/PS2/PSDRenderState.h" // for RS_TEX_PSM*
#include "Display/PS2/GSMemManager.h"
#include "Display/PS2/Renderer/Code/MasterList.h"
#include "Display/PS2/GSState.h"
#include "Display/PS2/Renderer/Code/AnimDDMAObject.h"
#include "Display/PS2/Renderer/Common/PRIMReg.h"
#include "Display/PS2/Renderer/Common/GIFTag.h"
#include "Display/PS2/Renderer/Common/VIFInline.h"
#include "Display/PS2/Renderer/Common/GIFInline.h"
using namespace VIFInline;
using namespace GIFInline;
#endif //PS2

#ifdef DIRECTX
#include "Display/DirectX/DirectX.h"
#include "Game/Managers/ShadowMgr.h"
#endif //DIRECTX

#ifdef GCN
#include "Display/GCN/GCN.h"
#include "Display/GCN/Shader.h"
#endif //GCN

//============================================================================

#endif // __DISPLAY_PCH_H
