/*************************************/
/*                                   */
/*   RenderMgr.h                     */
/*   new engine  04/03/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   render processing               */
/*                                   */
/*************************************/

#ifndef __RENDERMGR_H
#define __RENDERMGR_H

/******************* includes *****************************/

/* system includes */

/* engine includes */

#include "Layers/LayersStruct.h"				// low-level include
#include "Display/Common/RenderState.h"	// for RS_BLENDFUNC_DIFFUSE
#include "platform/CompilerControl.h"		// for NAMED_SECTION

/******************* forward declarations ****************/

/******************* defines *****************************/

/* render manager callback registration */

enum
{
	RENDER_START_CALLBACK = 0,		// called at start of render
	RENDER_SCENE_CALLBACK,			// called before the rendering of each scene (e.g. split screen views)
	RENDER_PARTICLE_CALLBACK,		// called before the rendering of each scene's particles
	RENDER_SPRITE_CALLBACK,			// called after sprites are rendered
	RENDER_END_CALLBACK,				// called at end of render.
	RENDER_DEBUG_CALLBACK,			// called at the very end of the frame (for debug overlays).
    RENDER_UIX_CALLBACK,            // called before sprite rendering for UIX (XBox drop-in online UI)
	MAX_RENDER_CALLBACKS,
	INVALID_RENDER_CALLBACK,
};

/* default state for bTranslateAndScale in RenderMgr__ProjectedTextureSetup call */

#ifdef GCN
#define DEFAULT_TRANSLATE_AND_SCALE	false		// gcn takes care of this itself
#else //GCN
#define DEFAULT_TRANSLATE_AND_SCALE	true
#endif //GCN

/* fades-- a fade type is bit mask which always includes a wipe type, and optional special effect(s) */

/* some standard fades */

#define FADE_TO_BLACK_FAST				0				// suitable for legal screen, etc
#define FADE_WARP							1
#define FADE_TO_BLACK_INSTANT			2
#define FADE_TO_BLACK_SLOW				3
#define FADE_STANDARD_COUNT			4

/* pick one of these...wipe type */

#define FADE_STANDARD					0x00000000	// all corners of sprite have same alpha
#define FADE_WIPE_TOP_TO_BOTTOM		0x00000001
#define FADE_WIPE_BOTTOM_TO_TOP		0x00000002
#define FADE_WIPE_LEFT_TO_RIGHT		0x00000003
#define FADE_WIPE_RIGHT_TO_LEFT		0x00000004
#define FADE_WIPE_RANDOM				0x00000005	// will pick one randomly

/* plus one or more of these...special effects */

#define FADE_USE_DISTORTION			0x00000010	// use full screen distortion
#define FADE_USE_MOTION_BLUR			0x00000020	// use motion blur

/* internal use only */

#define FADE_WIPE_MASK					0x00000007
#define FADE_IN							0x00000040	// fade in if set, fade out if clear
#define FADE_OUT_COMPLETED				0x00000080	// faded out, but not started fading in
#define FADE_FIRST_ADVANCE				0x00000100	// first time RenderMgr__AdvanceFade is called for a fade
#define FADE_IN_COMPLETED				0x00000200	// faded in

/******************* macros ******************************/

/******************* structures **************************/

/* provide some callbacks during rendering */

typedef void (*t_LoadScreenCallback)(void);
typedef bool (*t_RenderCallback)(void *);
struct ts_RenderCallback
{
	bool					bActive;
	t_RenderCallback	RenderCallback;
	void					*pCallbackData;
};

class Graphics4x4;
/******************* global variables ********************/

extern ts_RenderCallback RenderCallbackList[MAX_RENDER_CALLBACKS];

/******************* global prototypes *******************/

void RenderMgr__Init();
void RenderMgr__Reset();
void RenderMgr__Shutdown();
void RenderMgr__SetWorldDBHandle(t_Handle WorldDBHandle);
t_Handle RenderMgr__GetWorldDBHandle(void);
void RenderMgr__RegisterCallback(u32 u32Index, t_RenderCallback RenderCallback, void *pCallbackData);
void RenderMgr__KillCallback(u32 u32Index);
#ifdef DIRECTX_PC
void RenderMgr__Render(bool toTexture = false);
#else
void RenderMgr__Render(void);
#endif
void RenderMgr__ProjectedTextureSetup( bool bTranslateAndScale = DEFAULT_TRANSLATE_AND_SCALE);
void RenderMgr__ProjectedTextureRestore();
bool RenderMgr__RenderingProjectedTextures() NAMED_SECTION("text.rend1");
void RenderMgr__LockDiffuseRender(int nBlendFunc = RS_BLENDFUNC_DIFFUSE);
void RenderMgr__ForceClearAtStart(bool bForceClear = true);
#ifdef PS2
void RenderMgr__LockDiffuseRenderSingleBitAlpha(int nAlpha);
#endif //PS2
void RenderMgr__UnlockDiffuseRender();

void RenderMgr__SetNumVBlanksToSyncTo(u8 n);
u8 RenderMgr__GetNumVBlanksToSyncTo(void);

// fades
void RenderMgr__FadeHide(); // temporarily hide fade for off screen rendering
void RenderMgr__FadeShow(); // show it again.
void RenderMgr__FadeOut(int nStandardFade, bool bCanBePaused = false, int sortPriority = 0xffff);
void RenderMgr__FadeIn(int nStandardFade, bool bCanBePaused = false, int sortPriority = 0xffff);
void RenderMgr__FadeOut(int nType, float fSeconds, ts_bRGBA Color, bool bAdditive, float fDistortOpacity, float fDistortAmplitude, bool bCanBePaused = false, int sortPriority = 0xffff);
void RenderMgr__FadeIn(int nType, float fSeconds, bool bAdditive, float fDistortOpacity, float fDistortAmplitude, bool bCanBePaused = false, int sortPriority = 0xffff);
void RenderMgr__FadeOut(int nType, float fSeconds, ts_bRGBA Color1, ts_bRGBA Color2, bool bAdditive, float fDistortOpacity, float fDistortAmplitude, bool bCanBePaused = false, int sortPriority = 0xffff);
void RenderMgr__FadeIn(int nType, float fSeconds, ts_bRGBA Color, bool bAdditive, float fDistortOpacity, float fDistortAmplitude, bool bCanBePaused = false, int sortPriority = 0xffff);
bool RenderMgr__IsFading();
bool RenderMgr__IsFadingOut();
bool RenderMgr__IsFadingIn();
bool RenderMgr__IsFadedOut();
bool RenderMgr__IsFadedIn();

ts_bRGBA RenderMgr__GetFadeColor();
void RenderMgr__FadeCanBePaused(bool bCanBePaused);
void RenderMgr__FadeBlackLock(bool bFadeBlackLock);

/************************ development routines ********************************/
inline bool RenderMgr__WireframeOn(void);

#ifndef CONSUMER_BUILD
void RenderMgr__ToggleLighting(void);
void RenderMgr__ToggleVertexColors(void);
void RenderMgr__ToggleFog(void);
void RenderMgr__ToggleWireframe(void);
void RenderMgr__ToggleBackfaceCullPatch(void);
void RenderMgr__ToggleForceZoom(void);
void RenderMgr__IncForceZoom(void);
void RenderMgr__DecForceZoom(void);
void RenderMgr__ToggleDiffuseMaps(void);
void RenderMgr__ToggleEnvironmentMaps(void);
void RenderMgr__ToggleLODs(void);
void RenderMgr__ForceLODsUp(void);
void RenderMgr__ForceLODsDown(void);
void RenderMgr__ToggleDrawBoxes(void);
void RenderMgr__ToggleOverrideBrightness(void);
void RenderMgr__ToggleVSync(void);
void RenderMgr__SetUseDisplayLists(bool bValue);
void RenderMgr__ToggleUseDisplayLists(void);
void RenderMgr__ToggleVisualizeSceneTree(void);
void RenderMgr__ToggleViewBackBuffer(void);
void RenderMgr__ToggleShadowDebug(void);
void RenderMgr__ToggleBoneRender(void);

inline bool RenderMgr__LightingOn(void);
inline bool RenderMgr__VertexColorsOn(void);
inline bool RenderMgr__FogEnabled(void);
inline bool RenderMgr__BackfaceCullPatchOn(void);
inline bool RenderMgr__ForceZoomOn(void);
inline float RenderMgr__ForceZoomValue(void);
inline bool RenderMgr__DiffuseMapsOn(void);
inline bool RenderMgr__EnvironmentMapsOn(void);
inline bool RenderMgr__LODsOn(void);
inline u8 RenderMgr__LODsForce(void);
inline bool RenderMgr__DrawBoxesOn(void);
inline bool RenderMgr__OverrideBrightnessOn(void);
inline bool RenderMgr__VSyncOn(void);
inline bool RenderMgr__UseDisplayLists(void);
inline bool RenderMgr__VisualizeSceneTree(void);
inline bool RenderMgr__ViewBackBuffer(void);
inline bool RenderMgr__ShadowDebugOn(void);
inline u8 RenderMgr__BoneRender(void);

void RenderMgr__ForceAlphaTestUp(void);
void RenderMgr__ForceAlphaTestDown(void);
void RenderMgr__ToggleForceAlphaTest(void);
bool RenderMgr__ForceAlphaTest(void);
u8 RenderMgr__ForceAlphaTestValue(void);
#endif //CONSUMER_BUILD

// Xbox needs this shit to compile
#if ( defined(_XBOX) || defined(DIRECTX_PC) )&& defined(CONSUMER_BUILD) 
inline bool RenderMgr__LightingOn(void) { return true; }
inline bool RenderMgr__DiffuseMapsOn(void) { return true; }
inline bool RenderMgr__EnvironmentMapsOn(void) { return true; }
#endif

// quick hack up of a cartoon renderer on the ps2... not really great, but still pretty good.
void RenderMgr__ToggleCartoonRenderer(void);
void RenderMgr__ToggleTextures();

inline bool RenderMgr__CartoonRenderer(void);

/************************ end of development routines *************************/

/* the render manager */

struct ts_RenderMgr
{
	t_Handle	WorldDBHandle;
#ifndef CONSUMER_BUILD
	bool		bLighting;
	bool		bFogEnable;
	bool		bVertexColors;
#endif
	bool		bWireframe;
#ifndef CONSUMER_BUILD
	bool		bBackfaceCullPatch;
	bool		bForceZoom;
	float		fForceZoom;
	bool		bDiffuseMaps;
	bool		bEnvironmentMaps;
	bool		bLODs;
	u8			nLODs;
	bool		bDrawBoxes;
	bool		bOverrideBrightness;
	bool		bVSync;
	bool		bForceAlphaTest;
	u8			nForceAlphaTest;
	bool		bUseDisplayLists;			// pc only, ps2 MUST use display lists
	bool		bVisualizeSceneTree;
	bool		bViewBackBuffer;			// currently ps2 only... but could be done on other platforms
	bool		bShadowDebug;				// view shadow frustum, instances re-rendered with shadow, stats
	u8			nBoneRender;				// 0-bones, 1-lines, 2-boxes, 3-don't render
	bool		bDisableTextures;

#endif //CONSUMER_BUILD

	bool		bCartoonRenderer;			// currently ps2 only.  not perfect, but was quick to do...
	bool		bRenderingProjectedTextures;

	u8			nNumVBlanksToSyncTo;
	bool		bForceClear;				// FORCE a screen clear at the start of a frame (normally false, let SceneView handle)

	/* fades */

	int		nFadeType;
	float		fFadeTotalTime;			// how long current fade will take
	float		fFadeTime;					// 0.0 to fFadeTotalTime
	ts_bRGBA	FadeIntermediateColor;
	ts_bRGBA	FadeFinalColor;			// also used as starting color for fade in
	t_Handle	FadeSpriteHandle;			// full screen sprite that handles the fade
	ts_bRGBA	*pFadeSpriteColor;
	u32		u32FadeMilliseconds;		// fade does its own timing
	bool		bFadeCanBePaused;			// in some circumstances screen fade can be paused
	bool		bFadeBlackLock;			// Lock to black (must fade in)

	/* possible load screen callback */

	t_LoadScreenCallback	LoadScreenCallback;
};

extern ts_RenderMgr RenderMgr;

#include "Game/Managers/RenderMgr.hpp"

#endif // __RENDERMGR_H
