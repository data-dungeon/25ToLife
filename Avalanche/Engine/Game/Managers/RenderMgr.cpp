/*************************************/
/*                                   */
/*   RenderMgr.c                     */
/*   new engine  04/03/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   render-process manager          */
/*                                   */
/*************************************/

/* the render manager is responsible for calling all the routines involved in the render process. it also handles
registering the 3d scenes to render */

/******************* includes ****************************/

#include "Game/GamePCH.h"



#include "Layers/Timing.h"
#include "EngineHelper/timer.h"
#include "EngineHelper/LoadScreen.h"
#include "Math/fastrand.h"
#include "Game/Managers/ShadowMgr.h"

// this is icky.. see the comment at the top of this header for info
#include "Effects/fullscreendistort.h"

#ifdef GCN
#include "Display/GCN/Shader.h"
#endif


/******************* defines *****************************/

/* define VC_MUTEX if you want vertex colors and lighting to be mutually exclusive (right now, Eric wants to turn */
/* them on/off independently) */

//#define VC_MUTEX

/******************* macros ******************************/

/******************* structures **************************/

typedef struct
{
	int nType;
	float fTime;
	ts_bRGBA IntermediateColor;
	ts_bRGBA FinalColor;
	bool bAdditive;
	float fDistortOpacity;
	float fDistortAmplitude;
} ts_StandardFade;

/******************* externs *****************************/

extern t_Handle TextureSetHandle;	// handle of main texture set, for sprite manager to use //yuch-- temporary

/******************* global variables ********************/

ts_RenderCallback RenderCallbackList[MAX_RENDER_CALLBACKS];

/******************* local variables *********************/

/* the render manager */

ts_RenderMgr RenderMgr;
static ts_RenderMgr RenderMgrInit = {

//yuch-- this doesn't belong in the render manager!!!	maybe the gamestate???
	INVALID_HANDLE,		// t_Handle	WorldDBHandle;
#ifndef CONSUMER_BUILD
	true,						// bool		bLighting;
	true,						// bool		bFogEnable;
	true,						// bool		bVertexColors;
#endif
	false,					// bool		bWireframe;
#ifndef CONSUMER_BUILD
	false,					// bool		bBackfaceCullPatch;
	false,					// bool		bForceZoom;
	0.0f,						// float		fForceZoom;
	true,						// bool		bDiffuseMaps;
	true,						// bool		bEnvironmentMaps;
	true,						// bool		bLODs;
	0,							// u8			nLODs;
	false,					// bool		bDrawBoxes;
	false,					// bool		bOverrideBrightness;
	true,						// bool		bVSync;
	false,					//	bool		bForceAlphaTest;
	0,							//	u8			nForceAlphaTest;
#if defined ( DIRECTX ) // && ! defined ( _DEBUG )
	true,						// bool		bUseDisplayLists;	// pc only, ps2 MUST use display lists
#else
	false,					// bool		bUseDisplayLists;	// pc only, ps2 MUST use display lists
#endif
	false,					// bool		bVisualizeSceneTree
	false,					// bool		bViewBackBuffer // ps2 only
	false,					// bool		bShadowDebug
	0,							// u8			nBoneRender
	false,					// bool		bDisableTextures
#endif // CONSUMER_BUILD
	false,					// bool		bCartoonRenderer
	false,					// bool		bRenderingProjectedTextures

#ifdef GCN
	2,							// u8			nNumVBlanksToSyncTo;
#else
	1,							// u8			nNumVBlanksToSyncTo;
#endif
	false,					// bool		bForceClear;				// FORCE a screen clear at the start of a frame (normally false, let SceneView handle)

	FADE_IN_COMPLETED,	// int		nFadeType;					// default to screen ON
	0.0f,						// float		fFadeTotalTime;			// how long current fade will take
	0.0f,						// float		fFadeTime;					// 0.0 to fFadeTotalTime
	{0, 0, 0, 0},			// ts_bRGBA	FadeIntermediateColor;
	{0, 0, 0, 0},			// ts_bRGBA	FadeFinalColor;			// also used as starting color for fade in
	INVALID_HANDLE,		// t_Handle	FadeSpriteHandle;			// full screen sprite that handles the fade
	NULL,						// ts_bRGBA	*pFadeSpriteColor;
	0,							// u32		u32FadeMilliseconds;		// fade does its own timing
	false,					// bool		bFadeCanBePaused;			// in some circumstances screen fade can be paused
	false,					// bool     bFadeBlackLock				// once faded stay black until faded in

	NULL,						// t_LoadScreenCallback	LoadScreenCallback;
};

static ts_StandardFade StandardFadeTable[FADE_STANDARD_COUNT] = {

/*   flags                       time     int. color        final color          additive    opacity     amplitude  */
	{ FADE_STANDARD,					0.5f,		{0, 0, 0, 0},		{0, 0, 0, 0},			0,				0.0f,			0.0f },		// FADE_TO_BLACK_FAST
	{ FADE_WIPE_TOP_TO_BOTTOM,		2.0f,		{0, 0, 0, 0},		{255, 255, 255, 0},	1,				0.0f,			0.0f },		// FADE_WARP
	{ FADE_STANDARD,					0.0f,		{0, 0, 0, 0},		{0, 0, 0, 0},			0,				0.0f,			0.0f },		// FADE_TO_BLACK_INSTANT
	{ FADE_STANDARD,					1.0f,		{0, 0, 0, 0},		{0, 0, 0, 0},			0,				0.0f,			0.0f },		// FADE_TO_BLACK_SLOW
};

/******************* local prototypes ********************/

void RenderMgr__Fade(int nType, float fSeconds, ts_bRGBA IntermediateColor, ts_bRGBA FinalColor, bool bAdditive, float fDistortOpacity, float fDistortAmplitude, bool bCanBePaused, int sortPriority);
void RenderMgr__AdvanceFade();

/******************* functions ***************************/

/* init manager */

void RenderMgr__Init()
{
	/* copy over original values */

	memcpy(&RenderMgr, &RenderMgrInit, sizeof(ts_RenderMgr));

	/* clear render manager callbacks */

	memset(RenderCallbackList, 0, MAX_RENDER_CALLBACKS * sizeof(ts_RenderCallback));
}

/* reset */

void RenderMgr__Reset()
{
	/* fade final color has to survive reset. sorry for the hack */

	ts_bRGBA SaveFadeFinalColor = RenderMgr.FadeFinalColor;
	bool goddammitthisissostupid = RenderMgr.bFadeBlackLock;

	/* no mallocs, so can just call init */

	RenderMgr__Init();
	RenderMgr.FadeFinalColor = SaveFadeFinalColor;
	RenderMgr.bFadeBlackLock = goddammitthisissostupid;

	/* on pc there may be some buffer clean-up */

	PatchDisp__Cleanup();
}

/* shutdown */

void RenderMgr__Shutdown()
{
	/* on pc there may be some buffer clean-up */

	PatchDisp__Cleanup();


}

/* force a frame buffer clear at the start of a frame */
/* this is usually handled by the scene, but when we dont */
/* have a scene, we need this */
void RenderMgr__ForceClearAtStart(bool bForceClear)
{
	RenderMgr.bForceClear = bForceClear;
}

/* identify the world database so render manager has access to models and other chunks */

void RenderMgr__SetWorldDBHandle(t_Handle WorldDBHandle)
{
	RenderMgr.WorldDBHandle = WorldDBHandle;
}
t_Handle RenderMgr__GetWorldDBHandle(void)
{
	ASSERT(RenderMgr.WorldDBHandle != INVALID_HANDLE);	// no sense requesting it if it hasn't been set yet
	return(RenderMgr.WorldDBHandle);
}

/* register a callback routine */

void RenderMgr__RegisterCallback(u32 u32Index, t_RenderCallback RenderCallback, void *pCallbackData)
{
	ASSERT(u32Index < MAX_RENDER_CALLBACKS);
	ASSERT(RenderCallbackList[u32Index].bActive == false);
	RenderCallbackList[u32Index].bActive = true;
	RenderCallbackList[u32Index].RenderCallback = RenderCallback;
	RenderCallbackList[u32Index].pCallbackData = pCallbackData;
}

/* kill a callback routine */

void RenderMgr__KillCallback(u32 u32Index)
{
	ASSERT(u32Index < MAX_RENDER_CALLBACKS);
	if ( RenderCallbackList[u32Index].bActive )
	{
		RenderCallbackList[u32Index].bActive = false;
		RenderCallbackList[u32Index].RenderCallback = NULL;
		RenderCallbackList[u32Index].pCallbackData = NULL;
	}
}

/* run the render process */
#ifdef DIRECTX_PC
void RenderMgr__Render(bool toTexture)
{
	// do whatever is necessary to set-up for rendering
	if ( !gfxStartRender(toTexture) )
		return;

	// start-of-render callback
	if (RenderCallbackList[RENDER_START_CALLBACK].bActive)
		RenderCallbackList[RENDER_START_CALLBACK].RenderCallback(RenderCallbackList[RENDER_START_CALLBACK].pCallbackData);

	// update fade, if any
	if ( RenderMgr.fFadeTotalTime != 0.0f)
		RenderMgr__AdvanceFade();

	// create shadows for shadow casters
	Profiler__Start(PROFILE_CREATE_SHADOWS);
	ShadowMgr__CreateShadows();
	Profiler__Stop(PROFILE_CREATE_SHADOWS);

	// setup frame buffer for rendering-- this is what will be displayed next frame buffer swap
	#if defined(WIN32) || defined(PS2)

	Display__SetFrameBuffer();
	#endif

	// if force screen clear is on, do it
	if ( RenderMgr.bForceClear )
		gfxClearFrameBuffer(Display__GetClearMode(), Display__GetClearColor());

	// possibly disable textures for the scene render
	#ifndef CONSUMER_BUILD
	if ( RenderMgr.bDisableTextures )
		TextureMgr::DisableTextureTransfers();
	#endif //CONSUMER_BUILD

	// render scene(s) (which includes particles and shadows
	SceneMgr::Render();


	#ifndef CONSUMER_BUILD
	if ( RenderMgr.bDisableTextures )
		TextureMgr::EnableTextureTransfers();
	#endif //CONSUMER_BUILD

	// callback before sprite rendering
	if (RenderCallbackList[RENDER_SPRITE_CALLBACK].bActive)
		RenderCallbackList[RENDER_SPRITE_CALLBACK].RenderCallback(RenderCallbackList[RENDER_SPRITE_CALLBACK].pCallbackData);

	if (RenderToTexture::renderingToTexture)
		gfxRenderMotionBlur();

	if (RenderToTexture::renderingToTexture)
		RenderToTexture::BlitTextureToScreen();

	FullScreenDistort::UpdateAll();
	FullScreenDistort::RenderAll();

	// purge texture ram so we have access to the full texture window
	TextureMgr::Cache().Unlock();
	TextureMgr::Pause();
	TextureMgr::Resume();

	Profiler__Start(PROFILE_SPRITE_RENDER);
	SpriteMgr__Render();
	Profiler__Stop(PROFILE_SPRITE_RENDER);

	// Callback for UIX rendering
    if (RenderCallbackList[RENDER_UIX_CALLBACK].bActive)
        RenderCallbackList[RENDER_UIX_CALLBACK].RenderCallback(RenderCallbackList[RENDER_UIX_CALLBACK].pCallbackData);

	// end-of-render callback
	if (RenderCallbackList[RENDER_END_CALLBACK].bActive)
		RenderCallbackList[RENDER_END_CALLBACK].RenderCallback(RenderCallbackList[RENDER_END_CALLBACK].pCallbackData);

	// debug-render callback
	if (RenderCallbackList[RENDER_DEBUG_CALLBACK].bActive)
		RenderCallbackList[RENDER_DEBUG_CALLBACK].RenderCallback(RenderCallbackList[RENDER_DEBUG_CALLBACK].pCallbackData);

	// done
	//if(!renderingToTexture)
		gfxEndRender();
	//else
	//	DirectX_Device->EndScene();
}
#else
void RenderMgr__Render(void)
{	
	// do whatever is necessary to set-up for rendering
	if ( !gfxStartRender() )
		return;
	// start-of-render callback
	if (RenderCallbackList[RENDER_START_CALLBACK].bActive)
		RenderCallbackList[RENDER_START_CALLBACK].RenderCallback(RenderCallbackList[RENDER_START_CALLBACK].pCallbackData);

	// update fade, if any
	if ( RenderMgr.fFadeTotalTime != 0.0f)
		RenderMgr__AdvanceFade();

	// create shadows for shadow casters
	Profiler__Start(PROFILE_CREATE_SHADOWS);
	ShadowMgr__CreateShadows();
	Profiler__Stop(PROFILE_CREATE_SHADOWS);

	// setup frame buffer for rendering-- this is what will be displayed next frame buffer swap
	#if defined(WIN32) || defined(PS2)

	Display__SetFrameBuffer();
	#endif

	// if force screen clear is on, do it
	if ( RenderMgr.bForceClear )
		gfxClearFrameBuffer(Display__GetClearMode(), Display__GetClearColor());

	// possibly disable textures for the scene render
	#ifndef CONSUMER_BUILD
	if ( RenderMgr.bDisableTextures )
		TextureMgr::DisableTextureTransfers();
	#endif //CONSUMER_BUILD

	// render scene(s) (which includes particles and shadows)
	SceneMgr::Render();

	#ifndef CONSUMER_BUILD
	if ( RenderMgr.bDisableTextures )
		TextureMgr::EnableTextureTransfers();
	#endif //CONSUMER_BUILD

	// callback before sprite rendering
	if (RenderCallbackList[RENDER_SPRITE_CALLBACK].bActive)
		RenderCallbackList[RENDER_SPRITE_CALLBACK].RenderCallback(RenderCallbackList[RENDER_SPRITE_CALLBACK].pCallbackData);

	// motion blur before sprites
	gfxRenderMotionBlur();

	// ask the full screen distortion manager to render itself out, before sprites...
	FullScreenDistort::UpdateAll();
	FullScreenDistort::RenderAll();

	// purge texture ram so we have access to the full texture window
	TextureMgr::Cache().Unlock();
	TextureMgr::Pause();
	TextureMgr::Resume();

	// render sprites
	Profiler__Start(PROFILE_SPRITE_RENDER);
	SpriteMgr__Render();
	Profiler__Stop(PROFILE_SPRITE_RENDER);

    // Callback for UIX rendering
    if (RenderCallbackList[RENDER_UIX_CALLBACK].bActive)
        RenderCallbackList[RENDER_UIX_CALLBACK].RenderCallback(RenderCallbackList[RENDER_UIX_CALLBACK].pCallbackData);

	// end-of-render callback
	if (RenderCallbackList[RENDER_END_CALLBACK].bActive)
		RenderCallbackList[RENDER_END_CALLBACK].RenderCallback(RenderCallbackList[RENDER_END_CALLBACK].pCallbackData);

	// debug-render callback
	if (RenderCallbackList[RENDER_DEBUG_CALLBACK].bActive)
		RenderCallbackList[RENDER_DEBUG_CALLBACK].RenderCallback(RenderCallbackList[RENDER_DEBUG_CALLBACK].pCallbackData);

	// done
	gfxEndRender();
}
#endif

void RenderMgr__SetNumVBlanksToSyncTo(u8 n)
{
	RenderMgr.nNumVBlanksToSyncTo = n;
}

/* set up for projected texture rendering-- convert vertex camera positions into texture coordinates */

void RenderMgr__ProjectedTextureSetup( bool bTranslateAndScale /*= DEFAULT_TRANSLATE_AND_SCALE*/)
{
	/* mark status */

	RenderMgr.bRenderingProjectedTextures = true;

	// Build a texture matrix to transform world-space coordinates
	// into the projective texture "screen" space (i.e., onto the
	// viewing plane as seen from the point of view of the light).
	// Scale & translate to go from [-1,1] normalized device
	// coordinates to [0,1] texture coordinates.

	TextureState__RequestUnit(0);	// shadow texture is on unit 0
	MatrixStack__Mode(MS_TEXTURE);
	MatrixStack__Identity();
	if (bTranslateAndScale)
	{
#ifdef DIRECTX
		MatrixStack__Translate(0.5f, 0.5f, 0.0f);
		MatrixStack__Scale(0.50f, -0.50f, 1.0f);
#else
		MatrixStack__Translate(0.5f, 0.5f, 0.0f);
		MatrixStack__Scale(0.50f, 0.50f, 1.0f);
#endif
	}


#ifdef GCN
	g_ShaderMgr.UseShader( g_ShaderMgr.GetRTShadowShader( ));
	g_ShaderMgr.Lock( );

	GXColor	materialColor;

	materialColor.r = 255;
	materialColor.g = 255;
	materialColor.b = 255;
	materialColor.a = 255;

	g_ShaderMgr.SetupLightingChannel( 0, materialColor);
	g_ShaderMgr.LightingChannelLock( );
#endif //GCN
}

/* restore after projected texture */

void RenderMgr__ProjectedTextureRestore()
{
	/* mark status */

	RenderMgr.bRenderingProjectedTextures = false;

	/* restore texture matrix */

	TextureState__RequestUnit(0);	// shadow texture is on unit 0
	MatrixStack__Mode(MS_TEXTURE);
	MatrixStack__Identity();
#ifdef GCN
	g_ShaderMgr.Unlock( );
	g_ShaderMgr.LightingChannelUnlock( );
#endif //GCN
}

/* status check */

bool RenderMgr__RenderingProjectedTextures()
{
	return(RenderMgr.bRenderingProjectedTextures);
}

/* lock the renderer into a diffuse-texture mode, for rendering shadows and other things that multiply against the frame
buffer */

void RenderMgr__LockDiffuseRender(int nBlendFunc /*= RS_BLENDFUNC_DIFFUSE*/)
{
	/* modify the existing frame buffer-- typically, we use the RS_BLENDFUNC_DIFFUSE to multiply the frame */
	/* buffer by the source texels, but RS_BLENDFUNC_SUBTRACT or RS_BLENDFUNC_ADD might be reasonable too */

	RenderState__Request(RS_BLENDFUNC, nBlendFunc);
	RenderState__Request(RS_TEXTURE_BRIGHTNESS, RenderState__ftoi(1.0f));
	RenderState__Request(RS_LIGHTING, RS_FALSE);
  	RenderState__Force(RS_BASECOLOR);	// override any vertex coloring that might have been done via DA_COLOR
	RenderState__Request(RS_BASECOLOR, 0xFFFFFFFF);
#ifndef CONSUMER_BUILD
	if (RenderMgr__ShadowDebugOn())
		RenderState__Request(RS_BASECOLOR, 0xff8080ff);	// draw instances re-rendered with shadow with a reddish overall color
#endif //CONSUMER_BUILD

	/* z-buffer */

	RenderState__Request(RS_ZTEST, RS_TRUE);
	RenderState__Request(RS_ZWRITE, RS_FALSE);		// no need to update the z buffer again
#ifdef PS2
	RenderState__Request(RS_ZFUNC, RS_ZFUNC_GEQUAL);
#endif

	/* lock it down */

	RenderState__Lock(RS_BLENDFUNC);
	RenderState__Lock(RS_TEXTURE_BRIGHTNESS);
	RenderState__Lock(RS_LIGHTING);
	RenderState__Lock(RS_BASECOLOR);
	RenderState__Lock(RS_ZTEST);
	RenderState__Lock(RS_ZWRITE);
	RenderState__Lock(RS_ZFUNC);

	/* texture units into default state. for one thing, this sets all texture units to no-texture state. */
	/* we had a bug where the last model rendered into the scene before shadows was multi-textured, so */
	/* texture unit 1 was set up with a texture, and then shadows were rendered. shadows should only render */
	/* one texture layer per poly, but the second texture unit was still active */

	TextureState__Default();

	/* draw array settings-- always one layer of texture (the projected texture) */

	DrawArray__Enable(DA_VECTOR);
	DrawArray__Disable(DA_NORMAL);
	DrawArray__Disable(DA_COLOR);
	DrawArray__TextureUnit(1);
	DrawArray__Disable(DA_TEXTURE);
	DrawArray__TextureUnit(0);
	DrawArray__Enable(DA_TEXTURE);
}

#ifdef PS2
/* the ps2 uses the alpha channel for the multiply-frame-buffer source. for a 1-bit alpha, we just have 2 values-- on and
off. we can specify what multiply values to use for these 2 options */

void RenderMgr__LockDiffuseRenderSingleBitAlpha(int nAlpha)
{
	/* first do normal setup */

	RenderMgr__LockDiffuseRender();

	/* when using RS_BLENDFUNC_DIFFUSE, alpha of source texture is used as a multiplier into frame buffer. the shadow */
	/* texture is a 16-bit texture, so there is a single bit of alpha. it should be 0 outside the shadow, and 1 */
	/* inside the shadow. we use the TEXA gs register to specify the values to use in either case-- 0x80 for 0 */
	/* means that the outside of the shadow has no effect on the frame buffer. some other value for 1 represents */
	/* the shadow color (grayscale). the lower the value, the darker the shadow (0 would be black). with bilinear */
	/* filtering on, we get some filtering between completely transparent and the shadow color */

	GSState__Set(
		SCE_GS_TEXA,
		SCE_GS_SET_TEXA(
			0,						// ta0-- alpha value to use when a = 0 in rgba16 or in rgb24
			0,						// aem-- if 1, alpha always 0 if r = g = b = 0
			nAlpha));			// ta1-- alpha value to use when a = 1 in rgba16
	GSState__Send();
}
#endif //PS2

/* unlock render mode after diffuse render */

void RenderMgr__UnlockDiffuseRender()
{
	/* unlock render states */

	RenderState__Unlock(RS_BLENDFUNC);
	RenderState__Unlock(RS_TEXTURE_BRIGHTNESS);
	RenderState__Unlock(RS_LIGHTING);
	RenderState__Unlock(RS_BASECOLOR);
	RenderState__Unlock(RS_ZTEST);
	RenderState__Unlock(RS_ZWRITE);
	RenderState__Unlock(RS_ZFUNC);
	RenderState__Default();

#ifdef PS2
	/* restore SCISSOR_1, TEXA values */

	GSState__Set(SCE_GS_SCISSOR_1, GSState.SCISSOR_1);	// scissor is changed for shadow creation, necessary here?
	GSState__Set(SCE_GS_TEXA, GSState.TEXA);
	GSState__Send();
#endif //PS2
}

/* request a fade */

void RenderMgr__FadeOut(int nStandardFade, bool bCanBePaused /*= false*/, int sortPriority /*= MAX_INT*/)
{
	ASSERT(nStandardFade < FADE_STANDARD_COUNT);
	RenderMgr__FadeOut(StandardFadeTable[nStandardFade].nType,
							 StandardFadeTable[nStandardFade].fTime,
							 StandardFadeTable[nStandardFade].IntermediateColor,
							 StandardFadeTable[nStandardFade].FinalColor,
							 StandardFadeTable[nStandardFade].bAdditive,
							 StandardFadeTable[nStandardFade].fDistortOpacity,
							 StandardFadeTable[nStandardFade].fDistortAmplitude,
							 bCanBePaused, sortPriority);
}
void RenderMgr__FadeIn(int nStandardFade, bool bCanBePaused /*= false*/, int sortPriority /*= MAX_INT*/)
{
	ASSERT(nStandardFade < FADE_STANDARD_COUNT);
	RenderMgr__FadeIn(StandardFadeTable[nStandardFade].nType,
							StandardFadeTable[nStandardFade].fTime,
							StandardFadeTable[nStandardFade].IntermediateColor,
							StandardFadeTable[nStandardFade].bAdditive,
							StandardFadeTable[nStandardFade].fDistortOpacity,
							StandardFadeTable[nStandardFade].fDistortAmplitude,
							bCanBePaused, sortPriority);
}
void RenderMgr__FadeOut(int nType, float fSeconds, ts_bRGBA FinalColor, bool bAdditive, float fDistortOpacity, float fDistortAmplitude, bool bCanBePaused /*= false*/, int sortPriority /*= MAX_INT*/)
{
	RenderMgr__Fade(nType, fSeconds, FinalColor, FinalColor, bAdditive, fDistortOpacity, fDistortAmplitude, bCanBePaused, sortPriority);
}
void RenderMgr__FadeIn(int nType, float fSeconds, bool bAdditive, float fDistortOpacity, float fDistortAmplitude, bool bCanBePaused /*= false*/, int sortPriority /*= MAX_INT*/)
{
	RenderMgr__Fade(nType | FADE_IN, fSeconds, RenderMgr.FadeFinalColor, RenderMgr.FadeFinalColor, bAdditive, fDistortOpacity, fDistortAmplitude, bCanBePaused, sortPriority);	// fade back in from last fade-out color
}
void RenderMgr__FadeOut(int nType, float fSeconds, ts_bRGBA IntermediateColor, ts_bRGBA FinalColor, bool bAdditive, float fDistortOpacity, float fDistortAmplitude, bool bCanBePaused /*= false*/, int sortPriority /*= MAX_INT*/)
{
	RenderMgr__Fade(nType, fSeconds, IntermediateColor, FinalColor, bAdditive, fDistortOpacity, fDistortAmplitude, bCanBePaused, sortPriority);
}
void RenderMgr__FadeIn(int nType, float fSeconds, ts_bRGBA FinalColor, bool bAdditive, float fDistortOpacity, float fDistortAmplitude, bool bCanBePaused /*= false*/, int sortPriority /*= MAX_INT*/)
{
	RenderMgr__Fade(nType | FADE_IN, fSeconds, RenderMgr.FadeFinalColor, FinalColor, bAdditive, fDistortOpacity, fDistortAmplitude, bCanBePaused, sortPriority);	// fade back in from last fade-out color
}
bool RenderMgr__IsFadedOut()
{
	return((RenderMgr.nFadeType & FADE_OUT_COMPLETED) != 0);
}
bool RenderMgr__IsFadedIn()
{
	return((RenderMgr.nFadeType & FADE_IN_COMPLETED) != 0);
}
bool RenderMgr__IsFading()
{
	return(RenderMgr.fFadeTotalTime != 0.0f);
}
bool RenderMgr__IsFadingOut()
{
	return(((RenderMgr.nFadeType & FADE_IN) == 0) && (RenderMgr__IsFading()));
}
bool RenderMgr__IsFadingIn()
{
	return(((RenderMgr.nFadeType & FADE_IN) != 0) && (RenderMgr__IsFading()));
}
ts_bRGBA RenderMgr__GetFadeColor()
{
	return(RenderMgr.FadeFinalColor);
}
void RenderMgr__FadeCanBePaused(bool bCanBePaused)
{
	RenderMgr.bFadeCanBePaused = bCanBePaused;
}
void RenderMgr__FadeBlackLock(bool bFadeBlackLock)
{
	RenderMgr.bFadeBlackLock = bFadeBlackLock;
}

/* the common set-up routine */

void RenderMgr__Fade(int nType, float fSeconds, ts_bRGBA IntermediateColor, ts_bRGBA FinalColor, bool bAdditive, float fDistortOpacity, float fDistortAmplitude, bool bCanBePaused, int sortPriority)
{
	if (LoadScreen::IsRunning())
		return;

	/* randomly pick a wipe? */

	RenderMgr.nFadeType = nType;
	if ((RenderMgr.nFadeType & FADE_WIPE_MASK) == FADE_WIPE_RANDOM)
	{
		RenderMgr.nFadeType &= ~FADE_WIPE_MASK;
		RenderMgr.nFadeType |= g_random.InRange(FADE_STANDARD, FADE_WIPE_RIGHT_TO_LEFT);
	}

	/* create a sprite if necessary */

	if (RenderMgr.FadeSpriteHandle == INVALID_HANDLE)
	{
		// one vert color must be different so it doesn't auto set SPRITE_ALL_ONE_COLOR
		static Vector3 Vertices[4] = { Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)};
		static ts_bRGBA VertColors[4] = { {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,1} };

		RenderMgr.FadeSpriteHandle = SpriteMgr__RegisterSprite(TS_NO_TEXTURE, SPRITE_DEFAULT_GROUP, (u16)sortPriority, bAdditive ? SPRITE_BLENDMODE_ENABLED : 0, Vertices, NULL, VertColors );

		// now get some info back
		SpriteMgr__GetDataPointers(RenderMgr.FadeSpriteHandle, NULL, NULL, NULL, &RenderMgr.pFadeSpriteColor);
	}

	/* fail if no sprite */

	if (RenderMgr.FadeSpriteHandle == INVALID_HANDLE)
	{
		ASSERTS(false, "No sprite available for fade!");
		return;
	}

	/* set up possible full screen distortion */

	if (nType & FADE_USE_DISTORTION)
	{
		float fAttack = 0.0f;
		float fDecay = 0.0f;
		if (nType & FADE_IN)
			fDecay = fSeconds;	// get weaker as screen fades in
		else
			fAttack = fSeconds;	// get stronger as screen fades out
		FullScreenDistort::Init(20, 15, fDistortOpacity, FullScreenDistort::NORMAL);
		FullScreenDistort::Add(FullScreenDistort::X, 0.5f, 0.5f, 0.0f, 0.5f, false, fAttack, 0.0f, fDecay, true, false, new SigGen(SigGen::SIN, 0.8f, fDistortAmplitude));
		FullScreenDistort::Add(FullScreenDistort::Y, 0.5f, 0.5f, 0.0f, 0.5f, false, fAttack, 0.0f, fDecay, true, false, new SigGen(SigGen::SIN, 0.8f, fDistortAmplitude));
	}

	/* set up time, color, pause status */

	if (fSeconds == 0.0f)
		fSeconds = 0.001f;	// ok to request an immediate fade, but RenderMgr__AdvanceFade is only called when fFadeTotalTime is non-0
	RenderMgr.fFadeTotalTime = fSeconds;
	RenderMgr.fFadeTime = 0.0f;
	if (RenderMgr.nFadeType & FADE_IN)
		RenderMgr.fFadeTime = -0.1f;	// for fade-in, delay a tiny bit before starting fade-in to hide glitches
	RenderMgr.FadeIntermediateColor = IntermediateColor;
	RenderMgr.FadeFinalColor = FinalColor;
	RenderMgr.u32FadeMilliseconds = EngineTimer__Time();
	RenderMgr.bFadeCanBePaused = bCanBePaused;
	RenderMgr.nFadeType |= FADE_FIRST_ADVANCE;
}

// temporarily hide fade for off screen rendering
void RenderMgr__FadeHide()
{
	if ( RenderMgr.FadeSpriteHandle != INVALID_HANDLE )
		SpriteMgr__DisableSprite( RenderMgr.FadeSpriteHandle );
}

// show it again.
void RenderMgr__FadeShow()
{
	if ( RenderMgr.FadeSpriteHandle != INVALID_HANDLE )
      SpriteMgr__EnableSprite( RenderMgr.FadeSpriteHandle );
}

/* move the fade along */

void RenderMgr__AdvanceFade()
{
	/* we don't get time from g_timer for two reasons: fades can't be paused, and fades may happen in load screen, which */
	/* does not update g_timer */

	ASSERT(RenderMgr.fFadeTotalTime != 0.0f || RenderMgr.bFadeBlackLock);	// should only be called if fade is active
	u32 u32NewTime = EngineTimer__Time();
	float fDeltaSec = (u32NewTime - RenderMgr.u32FadeMilliseconds) / 1000.0f;
	RenderMgr.u32FadeMilliseconds = u32NewTime;

	/* well, sometimes fades can be paused */

	if (RenderMgr.bFadeCanBePaused)
	{
		fDeltaSec = g_timer.GetFrameSec();
		if (fDeltaSec == 0.0f)
			return;
	}

	/* first advance should always be 0 seconds. this fixes a bug where the first run of AdvanceSimulation takes */
	/* a long time (building display lists, etc), and the delta time that the fade sees is already past its end */
	/* time */

	if (RenderMgr.nFadeType & FADE_FIRST_ADVANCE)
	{
		fDeltaSec = 0.0f;
		RenderMgr.nFadeType &= ~FADE_FIRST_ADVANCE;
	}

	/* advance time */

	RenderMgr.fFadeTime += fDeltaSec;

	/* wipes work by having the first edge alpha out over half the time, and then the other edge uses the other half. */
	/* standard fade is a sub-case where both edges go at the same rate, over the full fade time */

	float fTotalTime = RenderMgr.fFadeTotalTime;
	int nPasses = 1;
	if ((RenderMgr.nFadeType & FADE_WIPE_MASK) != FADE_STANDARD)
	{
		/* wipe */

		fTotalTime *= 0.5f;
		nPasses = 2;
	}
	for (int nPass = 0; nPass < nPasses; nPass++)
	{
		float fFraction = Math::Clamp(RenderMgr.fFadeTime / fTotalTime);
		if (nPass == 1)
			fFraction -= 1.0f;	// must be a wipe
		fFraction = Math::Max(fFraction, 0.0f);
		fFraction = Math::Min(fFraction, 1.0f);

		/* interpolate between possibly-different colors */

		ts_bRGBA Color;
		Color.R = RenderMgr.FadeIntermediateColor.R + (int) (fFraction * (float) (RenderMgr.FadeFinalColor.R - RenderMgr.FadeIntermediateColor.R));
		Color.G = RenderMgr.FadeIntermediateColor.G + (int) (fFraction * (float) (RenderMgr.FadeFinalColor.G - RenderMgr.FadeIntermediateColor.G));
		Color.B = RenderMgr.FadeIntermediateColor.B + (int) (fFraction * (float) (RenderMgr.FadeFinalColor.B - RenderMgr.FadeIntermediateColor.B));

		/* move sprite color alpha's along */

		u8 u8Alpha = (int) ((float) BYTE_FULL_ALPHA * fFraction);
		if (RenderMgr.nFadeType & FADE_IN)
			u8Alpha = BYTE_FULL_ALPHA - u8Alpha;

		/* choose edges to support wipe */

		bool bValid[4] = {true, true, true, true};
		switch(RenderMgr.nFadeType & FADE_WIPE_MASK)
		{
		case FADE_WIPE_TOP_TO_BOTTOM:
			if (nPass == 0)
				bValid[2] = bValid[3] = false;
			else
				bValid[0] = bValid[1] = false;
			break;
		case FADE_WIPE_BOTTOM_TO_TOP:
			if (nPass == 0)
				bValid[0] = bValid[1] = false;
			else
				bValid[2] = bValid[3] = false;
			break;
		case FADE_WIPE_RIGHT_TO_LEFT:
			if (nPass == 0)
				bValid[0] = bValid[3] = false;
			else
				bValid[1] = bValid[2] = false;
			break;
		case FADE_WIPE_LEFT_TO_RIGHT:
			if (nPass == 0)
				bValid[1] = bValid[2] = false;
			else
				bValid[0] = bValid[3] = false;
			break;
		}

		/* set colors for valid vertices */

		Color.A = u8Alpha;
		for (int i = 0; i < 4; i++)
		{
			if (bValid[i])
				RenderMgr.pFadeSpriteColor[i] = Color;
		}
	}

	/* maybe done */

	if (RenderMgr.fFadeTime >= RenderMgr.fFadeTotalTime)
	{
		if ((RenderMgr.nFadeType & FADE_IN) || !RenderMgr.bFadeBlackLock)
			RenderMgr.fFadeTotalTime = 0.0f;

		/* maybe shutdown full screen distortion */

		if (RenderMgr.nFadeType & FADE_USE_DISTORTION)
			FullScreenDistort::Shutdown();

		/* get rid of the sprite if this is a fade-in (it's gone to totally transparent). it might be possible */
		/* get get rid of the sprite after a fade out. there are two cases: we stop rendering, so the screen stays */
		/* at the last framebuffer state; or we reset and render some more, and we can change the background color */
		/* to match the fade out color. there might be a one-frame glitch in the first case. as it stands, it we */
		/* reset the game engine after the fade is done, the sprite will be killed by SpriteMgr__Reset, and */
		/* RenderMgr.FadeSpriteHandle will be reset to INVALID_HANDLE by RenderMgr__Reset. i think we are ok leaving */
		/* the sprite around */

		if (RenderMgr.nFadeType & FADE_IN)
		{
			/* done with sprite */

			SpriteMgr__KillSprite(RenderMgr.FadeSpriteHandle);
			RenderMgr.FadeSpriteHandle = INVALID_HANDLE;
			RenderMgr.pFadeSpriteColor = NULL;
			RenderMgr.nFadeType |= FADE_IN_COMPLETED;	
		}
		else
			RenderMgr.nFadeType |= FADE_OUT_COMPLETED;	// record that fade out is completed-- gets cleared once a new fade-in starts
	}
}

/************************ development routines ********************************/

void RenderMgr__ToggleWireframe(void)
{
	RenderMgr.bWireframe ^= true;
}

#ifndef CONSUMER_BUILD

/* toggle render control variables */

void RenderMgr__ToggleLighting(void)
{
	RenderMgr.bLighting ^= 1;
#ifdef VC_MUTEX
	if (RenderMgr.bLighting)
		RenderMgr.bVertexColors = false;
#endif //VC_MUTEX

	/* have to rebuild display lists for new lighting state to take effect */

	DisplayList__ResetDisplayLists(false);
}
void RenderMgr__ToggleVertexColors(void)
{
	RenderMgr.bVertexColors ^= 1;
#ifdef VC_MUTEX
	if (RenderMgr.bVertexColors)
		RenderMgr.bLighting = false;
#endif	//VC_MUTEX

	/* have to rebuild display lists for new lighting state to take effect */

	DisplayList__ResetDisplayLists(false);

}
void RenderMgr__ToggleFog(void)
{
	RenderMgr.bFogEnable ^= true;

	/* if we turned it off, make sure fog is currently off (and after this, it will be prevented from being */
	/* turned back on) */

	if (RenderMgr.bFogEnable == false)
		RenderState__Request(RS_FOG, RS_FALSE);
}
void RenderMgr__ToggleBoneRender(void)
{
	++RenderMgr.nBoneRender &= 3;
}
void RenderMgr__ToggleBackfaceCullPatch(void)
{
	RenderMgr.bBackfaceCullPatch ^= true;
//yuch-- crossfade test
//DisplayList__ResetDisplayLists(false);
}
void RenderMgr__ToggleForceZoom(void)
{
	RenderMgr.bForceZoom ^= true;
}
void RenderMgr__IncForceZoom(void)
{
	RenderMgr.fForceZoom += 0.01f;
	if (RenderMgr.fForceZoom > 5.0f)
		RenderMgr.fForceZoom = 5.0f;
}
void RenderMgr__DecForceZoom(void)
{
	RenderMgr.fForceZoom -= 0.01f;
	if (RenderMgr.fForceZoom < 0.0f)
		RenderMgr.fForceZoom = 0.0f;
}
void RenderMgr__ToggleDiffuseMaps(void)
{
	RenderMgr.bDiffuseMaps ^= true;
	DisplayList__ResetDisplayLists(false);
}
void RenderMgr__ToggleEnvironmentMaps(void)
{
	RenderMgr.bEnvironmentMaps ^= true;
	DisplayList__ResetDisplayLists(false);
}
void RenderMgr__ToggleLODs(void)
{
	RenderMgr.bLODs ^= true;
}
void RenderMgr__ForceLODsUp(void)
{
	//force an LOD level: 0 -> normal behavior, n -> display N-1'th LOD level (or nearest one to it that is available)
	if(RenderMgr.nLODs < MAX_LODS)
		RenderMgr.nLODs++;
}
void RenderMgr__ForceLODsDown(void)
{
	//force an LOD level: 0 -> normal behavior, n -> display N-1'th LOD level (or nearest one to it that is available)
	if(RenderMgr.nLODs > 0)
		RenderMgr.nLODs--;
}
void RenderMgr__ForceAlphaTestUp(void)
{
	//force alpha test value higher
	if(RenderMgr.nForceAlphaTest < 255)
		RenderMgr.nForceAlphaTest++;
}
void RenderMgr__ForceAlphaTestDown(void)
{
	//force alpha test value higher
	if(RenderMgr.nForceAlphaTest > 0)
		RenderMgr.nForceAlphaTest--;
}
void RenderMgr__ToggleForceAlphaTest(void)
{
	RenderMgr.bForceAlphaTest ^= true;
}
void RenderMgr__ToggleDrawBoxes(void)
{
	RenderMgr.bDrawBoxes ^= true;
}
void RenderMgr__ToggleOverrideBrightness(void)
{
	RenderMgr.bOverrideBrightness ^= true;
}
void RenderMgr__ToggleVSync(void)
{
	RenderMgr.bVSync ^= true;
#ifdef PS2
	MasterList::DisplayVSynced(RenderMgr.bVSync);
#endif
}
void RenderMgr__SetUseDisplayLists(bool bValue)
{
	RenderMgr.bUseDisplayLists = bValue;
}
void RenderMgr__ToggleUseDisplayLists(void)
{
	RenderMgr.bUseDisplayLists ^= true;
	DisplayList__ResetDisplayLists(false);
}
void RenderMgr__ToggleVisualizeSceneTree(void)
{
	RenderMgr.bVisualizeSceneTree ^= true;
}

void RenderMgr__ToggleViewBackBuffer(void)
{
	RenderMgr.bViewBackBuffer ^= true;
}

void RenderMgr__ToggleShadowDebug(void)
{
	RenderMgr.bShadowDebug ^= true;
}

#endif //CONSUMER_BUILD

// quick hack up of a cartoon renderer on the ps2... not really great, but still pretty good.
void RenderMgr__ToggleCartoonRenderer(void)
{
	RenderMgr.bCartoonRenderer ^= 1;
}

void RenderMgr__ToggleTextures()
{
	#ifndef CONSUMER_BUILD
	RenderMgr.bDisableTextures ^= 1;
	#endif //CONSUMER_BUILD
}

/************************ end of development routines ********************************/

/************************ local routines ********************************/

