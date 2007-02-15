/*************************************/
/*                                   */
/*   ShadowMgr.c                     */
/*   new engine  4/13/01             */
/*   ned martin, avalanche software  */
/*   visual c++ 6.0                  */
/*   shadow manager                  */
/*                                   */
/*************************************/

/* handle real-time shadow casters. each entry in the manager is an instance that casts a shadow. each instance has one or
more associated light sources. each light casts a separate shadow into a separate texture. the shadow is cast by treating
the light source as a camera, and rendering the instance from the light's point of view. to get the smallest possible
texture, you manipulate the light so that the "camera" frustum just contains the instance. this is pretty easy for
infinite directional lights, since you can move the light's "location" anywhere */

/******************* includes ****************************/

#include "Game/GamePCH.h"
#include "Game/Managers/ShadowMgr.h"

#ifdef GCN
#include "Display/GCN/Shader.h"
#endif

#ifdef DIRECTX
#include "Display/DirectX/PatchHelper.h"
#endif

#ifdef PS2
//#define DISABLE_SHADOWS
#endif

#define MAX_LIGHTS_PER_SHADOW_CASTER		4

/******************* macros ******************************/

#define SHADOW_GET_RECORD(Handle)				&ShadowMgr.pShadowAlloc[HANDLE_INDEX(Handle)]

/******************* structures **************************/

/* shadow caster record */

struct ts_Shadow
{
	ts_Shadow			*pNext;	// linked list pointer-- does not have to be first

	/* instance that is casting the shadow */

	CInstance			*pInst;

	/* Some overrides for the instance */
	Vector3				*pCenterOverride;
	float					*pRadius;

	/* light(s) that cast the shadow(s) */

	ts_Light				*pLights[MAX_LIGHTS_PER_SHADOW_CASTER];
	int					nLightCount;

	/* for each light, we need a frustum and data on how to position it to render the shadow. we position the frustum */
	/* so that the shadow fills the target texture as much as possible. so for an infinite directional light, we */
	/* always start at the center of the instance that is casting the shadow and move back a set amount so that the */
	/* instance fills the frustum */

	ts_PoolFrustum		*pLightFrustum[MAX_LIGHTS_PER_SHADOW_CASTER];
	Vector3				LightDelta[MAX_LIGHTS_PER_SHADOW_CASTER];	// relative to center of instance
	ts_PoolMatrix		*pFrustumProjection[MAX_LIGHTS_PER_SHADOW_CASTER];
	ts_PoolMatrix		*pWorldToLight[MAX_LIGHTS_PER_SHADOW_CASTER];
  	Texture*				nRealtimeShadowTexture[MAX_LIGHTS_PER_SHADOW_CASTER];

#if defined(DIRECTX)
	unsigned int		uInstanceShadowCount;
	bool					bInstanceShadowedByLight[MAX_LIGHTS_PER_SHADOW_CASTER];
	ts_PoolMatrix		*pWorldToTexture[MAX_LIGHTS_PER_SHADOW_CASTER];
#endif

	/* realtime shadow texture to use */

	/* assigned handle for invalid handle checking */

	t_Handle				Handle;
};


/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

/* the shadow manager */

ts_ShadowMgr ShadowMgr;

#if defined(DIRECTX)
unsigned int		InstanceShadowCount = 0;

Graphics4x4			LightProjToTexture;
#endif

static u16 uSize;
static u16 vSize;

/******************* local prototypes ********************/

void Shadow__Create(ts_Shadow *pShadow);
ts_Shadow *ShadowMgr__AllocShadowRecord(void);
void Shadow__Free(ts_Shadow *pShadow, int nIndex);

/******************* functions ***************************/

/* allocate records for shadow manager */

void ShadowMgr__Init(int nMaxShadows)
{
	ts_Shadow *pShadow;

	/* clear shadow manager */

	memset(&ShadowMgr, 0, sizeof(ts_ShadowMgr));

	/* set up pool of shadows */

	ASSERT(nMaxShadows < MAX_HANDLES);	// see definition of MAX_HANDLES
	ShadowMgr.nMaxShadows = nMaxShadows;
	MEM_SET_ALLOC_NAME("ShadowMgr shadow list");
	ShadowMgr.pShadowAlloc = (ts_Shadow *) memAlloc(ShadowMgr.nMaxShadows * sizeof(ts_Shadow));
	SLinkList__Init(&ShadowMgr.ShadowFree, offsetof(ts_Shadow, pNext));
	SLinkList__LinkBlock(&ShadowMgr.ShadowFree, ShadowMgr.pShadowAlloc, ShadowMgr.nMaxShadows, sizeof(ts_Shadow));
	SLinkList__Init(&ShadowMgr.ShadowActive, offsetof(ts_Shadow, pNext));	// no shadows registered yet

	/* clear all handles */

	pShadow = (ts_Shadow *) ShadowMgr.ShadowFree.pHead;
	while (pShadow)
	{
		pShadow->Handle = INVALID_HANDLE;
		pShadow = pShadow->pNext;
	}

#if defined(DIRECTX)
	Graphics4x4	mScale, mBias;

	mScale.Identity( );
	mScale.e00 = -.5f;
	mScale.e11 = .5f;
	mScale.e22 = -1.0f;

	mBias.Identity( );
	mBias.e20 = .5f;
	mBias.e21 = .5f;

	LightProjToTexture = mScale * mBias;
#endif

	// Resaonable defaults
#if !defined(DIRECTX)
	uSize = vSize = 128;
#else
	uSize = vSize = 256;
#endif

	/* default active */

#ifdef DISABLE_SHADOWS
	ShadowMgr.bActive = FALSE;
#else
	ShadowMgr.bActive = TRUE;
#endif
}

/* reset */

void ShadowMgr__Reset(void)
{
	ts_Shadow *pShadow;

	/* free allocated stuff */

	pShadow = (ts_Shadow *) ShadowMgr.ShadowActive.pHead;
	while (pShadow)
	{
		for (int i = 0; i < pShadow->nLightCount; i++)
			Shadow__Free(pShadow, i);
		pShadow = pShadow->pNext;
	}

	/* reset pool of shadows */

	SLinkList__Init(&ShadowMgr.ShadowFree, offsetof(ts_Shadow, pNext));
	SLinkList__LinkBlock(&ShadowMgr.ShadowFree, ShadowMgr.pShadowAlloc, ShadowMgr.nMaxShadows, sizeof(ts_Shadow));
	SLinkList__Init(&ShadowMgr.ShadowActive, offsetof(ts_Shadow, pNext));	// no shadows registered yet

	/* clear all handles */

	pShadow = (ts_Shadow *) ShadowMgr.ShadowFree.pHead;
	while (pShadow)
	{
		pShadow->Handle = INVALID_HANDLE;
		pShadow = pShadow->pNext;
	}

	/* default active */

#ifdef DISABLE_SHADOWS
	ShadowMgr.bActive = FALSE;
#else
	ShadowMgr.bActive = TRUE;
#endif
}

/* shutdown */

void ShadowMgr__Shutdown(void)
{
	/* free allocated stuff */

	ts_Shadow *pShadow = (ts_Shadow *) ShadowMgr.ShadowActive.pHead;
	while (pShadow)
	{
		for (int i = 0; i < pShadow->nLightCount; i++)
			Shadow__Free(pShadow, i);
		pShadow = pShadow->pNext;
	}

	/* shut down */

	if (ShadowMgr.pShadowAlloc)
		memFree(ShadowMgr.pShadowAlloc);
	memset(&ShadowMgr, 0, sizeof(ts_ShadowMgr));
}

void ShadowMgr__SetShadowRez(u16 u, u16 v)
{
	uSize = u;
	vSize = v;
}

/* enable/disable shadow manager */

void ShadowMgr__Enable(void)
{
#ifdef DISABLE_SHADOWS
	ShadowMgr.bActive = FALSE;
#else
	ShadowMgr.bActive = TRUE;
#endif
}

void ShadowMgr__Disable(void)
{
	ShadowMgr.bActive = FALSE;
}

/* confirm shadow manager is empty when you think it should be */

#ifndef CONSUMER_BUILD
bool ShadowMgr__IsEmpty(void)
{
	if (SLinkList__Check(&ShadowMgr.ShadowFree) != (signed) ShadowMgr.nMaxShadows)
		return(FALSE);

	/* ok, shadow manager is empty */

	return(TRUE);
}
#endif //CONSUMER_BUILD

/* register a new shadow caster, and optionally a light to cast the shadow */

t_Handle ShadowMgr__RegisterShadow(CInstance *pInst, ts_Light *pLight, float fRadiusAdjust, Vector3 *centerOverride, float *radiusOverride )
{
	/* shadow manager must have been initialized */

	ASSERT(ShadowMgr.pShadowAlloc);		// if this hits, ShadowMgr__Init hasn't been called

	/* grab an instance record from free list */

	ts_Shadow *pShadow = ShadowMgr__AllocShadowRecord();
	if (pShadow == NULL)
		return(INVALID_HANDLE);

	/* add to head of active list */

	SLinkList__AddLinkToHead(&ShadowMgr.ShadowActive, pShadow);

	/* fill in record */

	pShadow->pInst = pInst;
	pShadow->pCenterOverride = centerOverride;
	pShadow->pRadius = radiusOverride ? radiusOverride : &pInst->BoundingSphere().Radius;

	pShadow->nLightCount = 0;
	if (pLight)
		ShadowMgr__AddLight(pShadow->Handle, pLight, fRadiusAdjust);

	/* return handle */

	return(pShadow->Handle);
}

/* remove a shadow from manager */

void ShadowMgr__KillShadow(t_Handle ShadowHandle)
{
	ts_Shadow *pShadow;

	/* point to shadow, error if invalid id */

	ASSERT(ShadowHandle != INVALID_HANDLE);
	pShadow = SHADOW_GET_RECORD(ShadowHandle);
	ASSERT(pShadow->Handle == ShadowHandle);			// error if id does not match. this means that record got deallocated (and maybe allocated again) somewhere else
	if (pShadow->Handle != ShadowHandle)
		return;

	/* free allocated stuff */

	for (int i = 0; i < pShadow->nLightCount; i++)
		Shadow__Free(pShadow, i);

	/* clear shadow record */

	pShadow->Handle = INVALID_HANDLE;
	SLinkList__RemoveLink(&ShadowMgr.ShadowActive, pShadow);
	SLinkList__AddLinkToHead(&ShadowMgr.ShadowFree, pShadow);
}

/* find a shadow record given an instance */

void ShadowMgr__ReplaceInstance(CInstance *pOldInst, CInstance *pNewInst)
{
	ts_Shadow *pShadow;

	/* shadow manager must have been initialized, and be active */

	ASSERT(ShadowMgr.pShadowAlloc);	// if this hits, ShadowMgr__Init hasn't been called
	if (ShadowMgr.bActive == FALSE)
		return;

	pShadow = (ts_Shadow *) ShadowMgr.ShadowActive.pHead;
	while (pShadow)
	{
		if (pShadow->pInst == pOldInst)
			pShadow->pInst = pNewInst;

		/* to next shadow */

		pShadow = pShadow->pNext;
	}
}

/* add a light to a shadow record */

bool ShadowMgr__AddLight(t_Handle ShadowHandle, ts_Light *pLight, float fRadiusAdjust)
{
	int i;
	ts_Shadow *pShadow;

	/* only handling infinite directional lights currently */

	if ((pLight->u32Flags & LIGHT_DIRECTIONAL) == 0)
	{
		ASSERT(FALSE);
		return(FALSE);
	}

	/* point to shadow, error if invalid id */

	ASSERT(ShadowHandle != INVALID_HANDLE);
	pShadow = SHADOW_GET_RECORD(ShadowHandle);
	ASSERT(pShadow->Handle == ShadowHandle);			// error if id does not match. this means that record got deallocated (and maybe allocated again) somewhere else
	if (pShadow->Handle != ShadowHandle)
		return(FALSE);

	/* bail if light already in shadow record */

	for (i = 0; i < pShadow->nLightCount; i++)
	{
		if (pShadow->pLights[i] == pLight)
			return(TRUE);
	}

	/* add light if room */

	ASSERT(pShadow->nLightCount < MAX_LIGHTS_PER_SHADOW_CASTER);
	if (pShadow->nLightCount == MAX_LIGHTS_PER_SHADOW_CASTER)
		return(FALSE);

	/* fill in light record. initialize alloc'd stuff for possible call to Shadow__Free in case of error */

	pShadow->pLights[pShadow->nLightCount] = pLight;
	pShadow->nRealtimeShadowTexture[pShadow->nLightCount] = NULL;
	pShadow->pLightFrustum[pShadow->nLightCount] = NULL;
	pShadow->pFrustumProjection[pShadow->nLightCount] = NULL;
	pShadow->pWorldToLight[pShadow->nLightCount] = NULL;
#if defined(DIRECTX)
	pShadow->pWorldToTexture[pShadow->nLightCount] = NULL;
#endif
	/* grab a realtime shadow texture */

	pShadow->nRealtimeShadowTexture[pShadow->nLightCount] = TextureMgr::CreateSpecialTexture(
		0,
#if defined(DIRECTX)
		Image::RENDER_TARGET,
		Image::SIZE_32_BIT | Image::TYPE_RGBA,
#else
		Image::SHADOW|Image::LOCAL,
		Image::SIZE_16_BIT|Image::TYPE_RGBA,
#endif
		uSize,
		vSize, "ShadowTexture");
	if (pShadow->nRealtimeShadowTexture[pShadow->nLightCount] == NULL)
	{
		Shadow__Free(pShadow, pShadow->nLightCount);
		return(FALSE);
	}
	/* malloc frustum */

	pShadow->pLightFrustum[pShadow->nLightCount] = FrustumPool__Alloc();
	if (pShadow->pLightFrustum[pShadow->nLightCount] == NULL)
	{
		ASSERT(FALSE);	// shouldn't run out of frustums in pool? if this happens, either live with it (light won't be added, comment out assert), or boost u32MaxFrustums parameter to FrustumPool__Init
		Shadow__Free(pShadow, pShadow->nLightCount);
		return(FALSE);	// no matrix available
	}

	/* malloc matrices */

	pShadow->pFrustumProjection[pShadow->nLightCount] = MatrixPool__Alloc(pShadow);
	if (pShadow->pFrustumProjection[pShadow->nLightCount] == NULL)
	{
		ASSERT(FALSE);	// shouldn't run out of matrices in pool? if this happens, either live with it (light won't be added, comment out assert), or boost u32MaxMatrices parameter to MatrixPool__Init
		Shadow__Free(pShadow, pShadow->nLightCount);
		return(FALSE);	// no matrix available
	}
	pShadow->pWorldToLight[pShadow->nLightCount] = MatrixPool__Alloc(pShadow);
	if (pShadow->pWorldToLight[pShadow->nLightCount] == NULL)
	{
		ASSERT(FALSE);	// shouldn't run out of matrices in pool? if this happens, either live with it (light won't be added, comment out assert), or boost u32MaxMatrices parameter to MatrixPool__Init
		Shadow__Free(pShadow, pShadow->nLightCount);
		return(FALSE);	// no matrix available
	}

#if defined(DIRECTX)
	pShadow->pWorldToTexture[pShadow->nLightCount] = MatrixPool__Alloc(pShadow);
	if (pShadow->pWorldToTexture[pShadow->nLightCount] == NULL)
	{
		ASSERT(FALSE);	// shouldn't run out of matrices in pool? if this happens, either live with it (light won't be added, comment out assert), or boost u32MaxMatrices parameter to MatrixPool__Init
		Shadow__Free(pShadow, pShadow->nLightCount);
		return(FALSE);	// no matrix available
	}
#endif
	/* construct frustum. figure out how much the camera needs to be pulled back so that it closely contains the instance. */
	/* currently hardwired */

#define SHADOW_PULLBACK					25.0f		// distance to pull back from instance so that it fills frustum-- calculate from bounding sphere?
#define SHADOW_FAR_CLIP_DELTA			4.5f		// long enough so that shadow reaches potential receivers, e.g. if caster is in air
#define SHADOW_ASPECT_RATIO			1.0f		// render into square texture
	float ShadowNearClip = SHADOW_PULLBACK - *pShadow->pRadius;	// SHADOW_PULLBACK cuts through origin of instance
	float ShadowFarClip = SHADOW_PULLBACK + SHADOW_FAR_CLIP_DELTA;
	float ShadowFOVDegrees = Math::ArcTanDeg( *pShadow->pRadius * 2.0f * fRadiusAdjust / SHADOW_PULLBACK);
	Frustum__Build(
		&pShadow->pLightFrustum[pShadow->nLightCount]->Frustum,
		ShadowNearClip,
		ShadowFarClip,
		FALSE,									// fog not a factor for shadow frustum
		0.0f,										// fog start
		0.0f,										// fog end
		ShadowFOVDegrees,						// degrees
		SHADOW_ASPECT_RATIO);

	/* low-angle lights cause problems with shadows, since they get cast over a long distance. ensure light angle */
	/* isn't too shallow */

	Vector3 LightDir = pLight->Pos;	// points from origin to light, so y-value is typically positive
	float fLengthXZ = LightDir.LengthXZ();
	float fRadians = Math::ArcTan2(LightDir.y(), fLengthXZ);
#define MIN_LIGHT_ANGLE	60.0f	// in degrees, noon sun is 90
	if (fRadians < Math::Deg2Rad(MIN_LIGHT_ANGLE))
	{
		/* light is too shallow, adjust it. this means that shadow will not match real-time lighting, but if it comes */
		/* from roughly the same direction, it should be ok. tan(angle) = opposite/adjacent. in this case, adjacent */
		/* is x/z-length, and opposite is y-length. so we can calculate desired y-length as opposite = adjacent * tan */

		LightDir.y(fLengthXZ * Math::Tan(Math::Deg2Rad(MIN_LIGHT_ANGLE)));
		LightDir.Normalize();
	}
	pShadow->LightDelta[pShadow->nLightCount] = LightDir * SHADOW_PULLBACK;

	/* calculate projection matrix for this frustum shape */

	MatrixStack__Mode( MS_PROJECTION);
	MatrixStack__Push( );
	MatrixStack__Identity( );
#if defined(DIRECTX)
	MatrixStack__Perspective( ShadowFOVDegrees, SHADOW_ASPECT_RATIO, 1.0f, 200.0f );
#else
	MatrixStack__Perspective(ShadowFOVDegrees, SHADOW_ASPECT_RATIO, ShadowNearClip, ShadowFarClip);
#endif
	memcpy( &pShadow->pFrustumProjection[pShadow->nLightCount]->mMatrix, MatrixStack__Get(MS_PROJECTION), sizeof(Graphics4x4));
	MatrixStack__Pop( );

	/* another light added to this shadow caster */

	pShadow->nLightCount++;
	return(TRUE);
}

/* remove a light from shadow record */

void ShadowMgr__RemoveLight(t_Handle ShadowHandle, ts_Light *pLight)
{
	int i;
	ts_Shadow *pShadow;

	/* point to shadow, error if invalid id */

	ASSERT(ShadowHandle != INVALID_HANDLE);
	pShadow = SHADOW_GET_RECORD(ShadowHandle);
	ASSERT(pShadow->Handle == ShadowHandle);			// error if id does not match. this means that record got deallocated (and maybe allocated again) somewhere else
	if (pShadow->Handle != ShadowHandle)
		return;

	/* find light */

	for (i = 0; i < pShadow->nLightCount; i++)
	{
		if (pShadow->pLights[i] == pLight)
			break;
	}
	if (i == pShadow->nLightCount)
		return;	// not found, nothing to do

	/* free matrices */

	Shadow__Free(pShadow, i);

	// free texture
	// DAS 11-24-04 this is deleted in Shadow__Free so why do it twice?
	//delete pShadow->nRealtimeShadowTexture[i];
	//pShadow->nRealtimeShadowTexture[i] = NULL;

	/* copy other lights down */

	if (i < pShadow->nLightCount - 1)
		memcpy(&pShadow->pLights[i], &pShadow->pLights[i + 1], (pShadow->nLightCount - 1 - i) * sizeof(ts_Light *));
	pShadow->nLightCount--;
}

/* add a light to all shadow casters */

void ShadowMgr__AddLightAll(ts_Light *pLight)
{
	ts_Shadow *pShadow;

	/* go through active list */

	pShadow = (ts_Shadow *) ShadowMgr.ShadowActive.pHead;
	while (pShadow)
	{
		ShadowMgr__AddLight(pShadow->Handle, pLight, 1.0f);
		pShadow = pShadow->pNext;
	}
}

/* remove a light from all shadow casters */

void ShadowMgr__RemoveLightAll(ts_Light *pLight)
{
	ts_Shadow *pShadow;

	/* go through active list */

	pShadow = (ts_Shadow *) ShadowMgr.ShadowActive.pHead;
	while (pShadow)
	{
		ShadowMgr__RemoveLight(pShadow->Handle, pLight);
		pShadow = pShadow->pNext;
	}
}

/* create shadows for all active shadow casters */
void ShadowMgr__CreateShadows(void)
{
	ts_Shadow *pShadow;

	/* shadow manager must have been initialized, and be active */

	ASSERT(ShadowMgr.pShadowAlloc);	// if this hits, ShadowMgr__Init hasn't been called
	if (ShadowMgr.bActive == FALSE)
		return;

	/* not if no sun */

	if ( !g_lightSet.sunLightExists() )
		return;

	/* no shadows created yet */

	ShadowMgr.bShadowsCreated = false;

	/* quick exit */

	if (ShadowMgr.ShadowActive.pHead == NULL)
		return;

	/* save current projection matrix */

	MatrixStack__Mode( MS_PROJECTION );
	MatrixStack__Push();

	/* turn z-buffer off, it doesn't matter-- might be a bit faster */

	RenderState__Request(RS_ZTEST, RS_FALSE);
	RenderState__Lock(RS_ZTEST);
	RenderState__Request(RS_ZWRITE, RS_FALSE);
	RenderState__Lock(RS_ZWRITE);

	/* lighting off */

	RenderState__Request(RS_LIGHTING, RS_FALSE);
	RenderState__Lock(RS_LIGHTING);

	/* for pc, we want to disable all textures and render with a chosen color to get a solid-color shadow */

#if defined(WIN32) || defined(GCN)
	for (int i = TextureState__GetNumUnits() - 1; i >= 0; i--)
	{
		TextureState__Request(i, TS_TYPE, TS_TYPE_NONE);	// no texturing
		TextureState__Lock(i, TS_TYPE);
		TextureState__Request(i, TS_METHOD, TS_METHOD_PASSTHRU);	// use fragment color-- base color in this case
		TextureState__Lock(i, TS_METHOD);
	}

#ifdef PS2
#define SHADOW_RED	(0xd0 << 0)		// closer to white, dimmer the shadow
#define SHADOW_GREEN	(0xd0 << 8)
#define SHADOW_BLUE	(0xd0 << 16)
#define SHADOW_ALPHA	(0xff << 24)	// always full alpha-- no alpha channel in frame buffer, alpha value doesn't affect shadow result, can safely use RS_BLENDFUNC_DEFAULT
#else
#define SHADOW_RED	(0xa0 << 0)		// closer to white, dimmer the shadow
#define SHADOW_GREEN	(0xa0 << 8)
#define SHADOW_BLUE	(0xa0 << 16)
#define SHADOW_ALPHA	(0xff << 24)	// always full alpha-- no alpha channel in frame buffer, alpha value doesn't affect shadow result, can safely use RS_BLENDFUNC_DEFAULT
#endif
	RenderState__Request(RS_BASECOLOR, SHADOW_ALPHA | SHADOW_BLUE | SHADOW_GREEN | SHADOW_RED);
	RenderState__Lock(RS_BASECOLOR);
	RenderState__Request(RS_BLENDFUNC, RS_BLENDFUNC_DEFAULT);
	RenderState__Lock(RS_BLENDFUNC);

#ifdef GCN
	GXColor			gxcolor;

	unsigned char	aucColor[4];

	RenderState__Unpackubv( aucColor, RenderState__Get( RS_BASECOLOR));

	gxcolor.r = aucColor[0];
	gxcolor.g = aucColor[1];
	gxcolor.b = aucColor[2];
	gxcolor.a = aucColor[3];

	GXSetTevColor( GX_TEVREG2, gxcolor);

	g_ShaderMgr.UseShader( g_ShaderMgr.GetRTShadowCreateShader( ));
	g_ShaderMgr.Lock( );
	TextureBind__Lock( );
#endif

#ifdef DIRECTX
	StandardTCMaterial::setRenderMode( StandardTCMaterial::MODE_SHADOW_CAST );
#endif

#endif //WIN32

	/* process all active shadows */

	pShadow = (ts_Shadow *) ShadowMgr.ShadowActive.pHead;
	while (pShadow)
	{
		Shadow__Create(pShadow);

		/* to next shadow */

		pShadow = pShadow->pNext;
	}

#ifdef DIRECTX
	StandardTCMaterial::setRenderMode( StandardTCMaterial::MODE_DEFAULT );
#endif
	/* restore projection matrix */

	MatrixStack__Mode( MS_PROJECTION );
	MatrixStack__Pop();

	/* back to full screen viewport */

	MatrixStack__FullScreenViewport();

	/* unlock render and texture states */

	RenderState__Unlock(RS_ZTEST);
	RenderState__Unlock(RS_ZWRITE);
	RenderState__Unlock(RS_LIGHTING);
	RenderState__Unlock(RS_BASECOLOR);
	RenderState__Unlock(RS_BLENDFUNC);
	TextureState__UnlockAll();
#ifdef GCN
	g_ShaderMgr.Unlock( );
	TextureBind__Unlock( );
#endif
}

/* render a shadow caster to create a shadow map */

void Shadow__Create(ts_Shadow *pShadow)
{
	int i;
	u32 u32LODLevel = 0;	// currently hard-wired to highest LOD
	ts_PatchSOM__RenderFlags ucFlags = {0,0,0};	// not currently used-- always 0
	ts_PatchSOM *pSOM;
	ts_Light *pLight;
	ASSERT(pShadow->pInst);

	/* quick exit */

	if (pShadow->nLightCount == 0)
		return;

	/* nothing to do if caster is hidden */
	if ( pShadow->pInst->Hidden() )
		return;

	/* give global access to instance being rendered */

	pRenderInst = pShadow->pInst;
	ShadowMgr.bCreatingShadows = TRUE;

	/* get model to render */
	pSOM = pShadow->pInst->Model();

	/* before rendering this object, make sure that it's textures are render-ready */

#ifdef PS2
	Texture* tex = TextureMgr::GetWhiteTexture();
	tex->Lock();
	tex->Select();
#endif

	/* What is our center of projection */
	Vector3 center;
	if (pShadow->pCenterOverride)
		center = *pShadow->pCenterOverride;
	else
		center = pShadow->pInst->BoundingSphere().Center;

	/* loop through lights */
	for (i = 0; i < pShadow->nLightCount; i++)
	{
		/* does light reach caster? */

		pLight = pShadow->pLights[i];

		/* calculate world-to-light matrix. the "camera" in this case is the light */

		Vector3 LightPosition;
		LightPosition = center + pShadow->LightDelta[i];
		BuildCTM(LightPosition, center, vUp, &g4WorldToCamera);

		/* save a copy, it will be needed later */

		//memcpy(&pShadow->pWorldToLight[i]->mMatrix, &g4WorldToCamera, sizeof(Graphics4x4));
		// Sad news, memcpy is not faster, not more readable nor does it make any more sense.
		pShadow->pWorldToLight[i]->mMatrix = g4WorldToCamera;

		/* calculate local-to-camera matrix */

		g4LocalToCamera.FastMultiply(pShadow->pInst->Matrix(), g4WorldToCamera);

		/* projection matrix is determined by shape of light frustum */

		MatrixStack__Mode(MS_PROJECTION);
		MatrixStack__Load(&pShadow->pFrustumProjection[i]->mMatrix);

#if defined(WIN32) || defined(GCN)
		/* for pc, clear frame buffer to white, and render shadow in a solid color that will be multiplied into the */
		/* frame buffer. non-shadow texels will be white, and the frame buffer will be unaffected. shadow color is set */
		/* in ShadowMgr__CreateShadows, above. the disadvantage of using a constant color and ignoring the textures is */
		/* that we lose possible alpha channels in the textures-- each whole polygon casts a solid shadow */

		static ts_bRGBA ShadowClearColor = {255, 255, 255, 255};
#else if defined(PS2)
		/* for ps2, clear frame buffer to black. the important thing is that the alpha value is 0. render the */
		/* shadow in normal colors (textured), but the shadow texels will have their alpha set. this alpha will */
		/* then be multiplied into the frame buffer (rgb is ignored). we use a 16-bit texture for the shadow texture */
		/* so that we can use the TEXA gs register to set the alpha that is used for 0/1 alpha bits. using a 32-bit */
		/* texture won't work */

		static ts_bRGBA ShadowClearColor = { 0, 0, 0, 0};
#endif //WIN32/PS2

		/* set up for rendering into correct spot, and clear frame */
		u16 u16Width;
		u16 u16Height;

		pShadow->nRealtimeShadowTexture[i]->Lock();
		pShadow->nRealtimeShadowTexture[i]->LockForRender();
		u16Width = pShadow->nRealtimeShadowTexture[i]->GetImage()->GetWidth();
		u16Height = pShadow->nRealtimeShadowTexture[i]->GetImage()->GetHeight();

#ifdef GCN

		ts_Viewport		clearVP;

		GXRenderModeObj	*pGXRenderMode;

		pGXRenderMode = DisplayState.pGXRenderMode;

		clearVP.fX = 0.0f;
		clearVP.fY = 0.0f;
		clearVP.fWidth = (float)( u16Width) / (float)pGXRenderMode->fbWidth;
		clearVP.fHeight = (float)( u16Height) / (float)pGXRenderMode->efbHeight;
		gfxClearFrameBuffer(RS_COLOR_BUFFER_BIT, &ShadowClearColor, &clearVP);
#else
		gfxClearFrameBuffer(RS_COLOR_BUFFER_BIT, &ShadowClearColor);
#endif

#ifdef PS2
		/* set viewport to protect outer border of pixels, in case frustum is close enough to caster that the shadow */
		/* would touch the edge of the shadow texture. since shadows are rendered with wrap == clamp (they get rendered */
		/* over a large area, but don't repeat), they need an outside edge of transparent pixels */
		GSState__Set(SCE_GS_SCISSOR_1, SCE_GS_SET_SCISSOR(1, u16Width - 2, 1, u16Height - 2));

		// This sets the shadow render color
		GSState__Set( SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ( 0x18, 0x18, 0x18, 0x80, 0x3f800000 ));
		GSState__Send( );	//yuch-- not needed in master list mode? PatchSOM__Render will flush???
#else
		/* set viewport to protect outer border of pixels, in case frustum is close enough to caster that the shadow */
		/* would touch the edge of the shadow texture. since shadows are rendered with wrap == clamp (they get rendered */
		/* over a large area, but don't repeat), they need an outside edge of transparent pixels */
		MatrixStack__ViewportPixels(1, 1, u16Width - 2, u16Height - 2);

		/* face sense */
		RenderState__Request( RS_FRONTFACE, ( pRenderInst->ClockwiseVertices() ) ? RS_FRONTFACE_CW : RS_FRONTFACE_CCW );
#endif

#if defined(DIRECTX)
		/* render instance */
		pShadow->pInst->RenderShadow( & pShadow->pInst->Matrix(), 0 );

		// if this instance is on an actor, render each of the actors instances
		// (attached weapons, for example)
		if ( pShadow->pInst->Owner() )
		{
			CActor* act = (CActor*) pShadow->pInst->Owner();
			ASSERT( act->IsA( CActor::ClassType() ) );
			for ( int j = 1 ; j < MAX_ACTOR_INSTANCES ; j++ )
			{
				if ( act->Instance( j ) && !act->Instance( j )->Hidden() )
					act->Instance( j )->RenderShadow( & act->Instance( j )->Matrix( ), 0 );
			}
		}

		Graphics4x4 mLightToWorld;

		pShadow->pWorldToLight[i]->mMatrix.Invert( mLightToWorld );
		Frustum__TransformToWorld( &pShadow->pLightFrustum[i]->Frustum, &mLightToWorld );

		Graphics4x4 CameraToLProj = pShadow->pWorldToLight[i]->mMatrix * pShadow->pFrustumProjection[i]->mMatrix;

		pShadow->pWorldToTexture[i]->mMatrix = CameraToLProj * LightProjToTexture;
#else
		/* render instance */
		pShadow->pInst->RenderShadow( &g4LocalToCamera, 0 );

		// if this instance is on an actor, render each of the actors instances
		// (attached weapons, for example)
		if ( pShadow->pInst->Owner() )
		{
			CActor* act = (CActor*) pShadow->pInst->Owner();
			ASSERT( act->IsA( CActor::ClassType() ) );
			for ( int j = 1 ; j < MAX_ACTOR_INSTANCES ; j++ )
			{
				if ( act->Instance( j ) && !act->Instance( j )->Hidden() )
				{
					g4LocalToCamera.FastMultiply( act->Instance( j )->Matrix(), g4WorldToCamera );
					act->Instance( j )->RenderShadow( &g4LocalToCamera, 0 );
				}
			}
		}
#endif
		/* finish render */
		pShadow->nRealtimeShadowTexture[i]->UnlockForRender();
		pShadow->nRealtimeShadowTexture[i]->Unlock();
	}

	/* we have at least one shadow created */

	ShadowMgr.bShadowsCreated = true;

	/* TJC - start next dynamic texture transfering */
#ifdef PS2
	tex->Unselect();
	tex->Unlock();
#endif

#ifdef _DEBUG
	/* check for errors */

	RenderState__CheckForErrors();
#endif //_DEBUG
	ShadowMgr.bCreatingShadows = FALSE;
}

/* render shadows into scene for all shadow casters. this re-renders the instances in the scene that intersect the
light frustum, with uv coordinate generation based on how the instance appears from the light */

void ShadowMgr__RenderShadows(CScene *pScene)
{
	/* shadow manager must have been initialized, and be active */

	ASSERT(ShadowMgr.pShadowAlloc);	// if this hits, ShadowMgr__Init hasn't been called
	if (ShadowMgr.bActive == FALSE)
		return;

	/* quick exit */

	if (ShadowMgr.ShadowActive.pHead == NULL)
		return;
	if (ShadowMgr.bShadowsCreated == false)	// shadow casters may have been hidden
		return;
	if ( !g_lightSet.sunLightExists() )
		return;

	/* mark status */

	ShadowMgr.bRenderingShadows = TRUE;

#ifdef GCN
	int	oldFogging = RenderState__Get( RS_FOG);

	RenderState__Unlock( RS_FOG);
	RenderState__Request( RS_FOG, RS_FALSE);
	RenderState__Lock( RS_FOG);
#endif

	/* set up for projected texture-- convert vertex positions into texture coordinates */

#ifdef PS2
	RenderMgr__ProjectedTextureSetup(false);	// ps2 shadow rendering microcode takes care of translation/scale itself
#else //PS2
	RenderMgr__ProjectedTextureSetup(  );
#endif //PS2

	/* shadows are rendered as a diffuse texture-- multiply frame buffer by texture */

#ifdef PS2
	RenderMgr__LockDiffuseRender(RS_BLENDFUNC_SUBTRACTIVE);	// shadow rendering on ps2 uses subtractive rendering
#else //PS2
	RenderMgr__LockDiffuseRender();
#endif //PS2

	/* render all previously-created shadows */

	ts_Shadow *pNextShadow = (ts_Shadow *) ShadowMgr.ShadowActive.pHead;
	while (pNextShadow)
	{
		/* pre-load pNextShadow so i can use continue */

		ts_Shadow *pShadow = pNextShadow;
		pNextShadow = pNextShadow->pNext;

		/* nothing to do if caster is hidden */
		if ( pShadow->pInst->Hidden() )
			continue;

		/* loop through shadows cast by this caster (one for each light) */

		for (int i = 0; i < pShadow->nLightCount; i++)
		{
			/* set and lock shadow texture to render with. use replace to get shadow pixel directly from shadow texture. */
			/* use TS_WRAP_CLAMP so that shadow doesn't repeat across large swaths of ground that it may be rendering over */
			/* (outer border of pixels of shadow texture must be transparent) */

			Texture* realtimeTexture = pShadow->nRealtimeShadowTexture[i];
			realtimeTexture->Lock();

			TextureState__RequestAll(0, TS_TYPE_2D, realtimeTexture, TS_METHOD_MODULATE, TS_WRAP_CLAMP, TS_WRAP_CLAMP, TS_FILTER_BILINEAR);

			TextureState__Lock(0, TS_BIND);
			TextureState__Lock(0, TS_TYPE);
			TextureState__Lock(0, TS_METHOD);
			TextureState__Lock(0, TS_WRAPU);
			TextureState__Lock(0, TS_WRAPV);
			TextureState__Lock(0, TS_FILTER);

			/* project world-space coordinates onto the viewing plane defined by the light */

			TextureState__RequestUnit(0);	// shadow texture is on unit 0
			MatrixStack__Mode(MS_TEXTURE);
			MatrixStack__Push();
#ifdef GCN
			LOCKUP();
			// TJC - Not sure what to do here.  I think that the correct solution is the
			// idea of adding another layer above textures which replace the texture context
			// which is called a Shader... :)... Then it manages all this stuff in a platform
			// independent way.  Until then, something should be done... but what?

/////////////////////////////////////////////////////////////////////////////////////////////////////
#define SHADOW_PULLBACK					25.0f		// distance to pull back from instance so that it fills frustum-- calculate from bounding sphere?
#define SHADOW_FAR_CLIP_DELTA			4.5f		// long enough so that shadow reaches potential receivers, e.g. if caster is in air
#define SHADOW_ASPECT_RATIO			1.0f		// render into square texture

			float ShadowNearClip = SHADOW_PULLBACK - *pShadow->pRadius;	// SHADOW_PULLBACK cuts through origin of instance

			float ShadowFarClip = SHADOW_PULLBACK + SHADOW_FAR_CLIP_DELTA;

			float fRadiusAdjust = 1.0f;

			float ShadowFOV = Math::ArcTanDeg( *pShadow->pRadius * 2.0f * fRadiusAdjust / SHADOW_PULLBACK);

			Matrix4x4		m;

			Mtx				mtx;

			MTXLightPerspective(	mtx,
										ShadowFOV,
										SHADOW_ASPECT_RATIO,
										0.5f,
										-0.5f,
										0.5f,
										0.5f);

			m.Identity( );

			m.e00 = mtx[0][0];
			m.e10 = mtx[0][1];
			m.e20 = mtx[0][2];
			m.e30 = mtx[0][3];

			m.e01 = mtx[1][0];
			m.e11 = mtx[1][1];
			m.e21 = mtx[1][2];
			m.e31 = mtx[1][3];

			m.e02 = mtx[2][0];
			m.e12 = mtx[2][1];
			m.e22 = mtx[2][2];
			m.e32 = mtx[2][3];

			MatrixStack__Multiply( &m);
/////////////////////////////////////////////////////////////////////////////////////////////
#else
			MatrixStack__Multiply( &pShadow->pFrustumProjection[i]->mMatrix);
#endif
			MatrixStack__Multiply( &pShadow->pWorldToLight[i]->mMatrix);

#ifdef DIRECTX
			MatrixStack__Multiply( &pScene->d_cameraToWorld );
			DirectX_Device->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 | D3DTTFF_PROJECTED );
			DirectX_Device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
#endif
			/* calculate world-space frustum view volume for instance culling in SceneMgr::RerenderScene */

			Graphics4x4 mLightToWorld;
			pShadow->pWorldToLight[i]->mMatrix.Invert(mLightToWorld);
			Frustum__TransformToWorld(&pShadow->pLightFrustum[i]->Frustum, &mLightToWorld);

#ifndef CONSUMER_BUILD
			/* maybe visualize the light frustum */

			if (RenderMgr__ShadowDebugOn())
			{
				static ts_bRGBA Blue = {0, 0, 255, 255};
				static ts_bRGBA TransparentBlue = {0, 0, 255, 20};
				for (int j = 0; j < 8; j++)
					WorkVector[j] = pShadow->pLightFrustum[i]->Frustum.Corners[j] * mLightToWorld;

#if 0
	  			/* i spent a bit of time trying to draw the sides of the box as solid quads so you could see */
				/* poly intersections better, but i couldn't get it working easily. first, the alpha didn't seem */
				/* to be effective, and second, if you rendered the triangles in pairs they worked ok, but if */
				/* you rendered all of them triangles dropped out. not worth pursuing, but here are the calls */
				/* that should render the inside of the box */

				gfxDrawTriangleWC(&WorkVector[0], &WorkVector[1], &WorkVector[2], &TransparentBlue);
				gfxDrawTriangleWC(&WorkVector[0], &WorkVector[2], &WorkVector[3], &TransparentBlue);

				gfxDrawTriangleWC(&WorkVector[4], &WorkVector[5], &WorkVector[6], &TransparentBlue);
				gfxDrawTriangleWC(&WorkVector[4], &WorkVector[6], &WorkVector[7], &TransparentBlue);

				gfxDrawTriangleWC(&WorkVector[0], &WorkVector[1], &WorkVector[4], &TransparentBlue);
				gfxDrawTriangleWC(&WorkVector[1], &WorkVector[4], &WorkVector[5], &TransparentBlue);

				gfxDrawTriangleWC(&WorkVector[1], &WorkVector[2], &WorkVector[5], &TransparentBlue);
				gfxDrawTriangleWC(&WorkVector[2], &WorkVector[5], &WorkVector[6], &TransparentBlue);

				gfxDrawTriangleWC(&WorkVector[2], &WorkVector[3], &WorkVector[6], &TransparentBlue);
				gfxDrawTriangleWC(&WorkVector[3], &WorkVector[6], &WorkVector[7], &TransparentBlue);

				gfxDrawTriangleWC(&WorkVector[3], &WorkVector[0], &WorkVector[7], &TransparentBlue);
				gfxDrawTriangleWC(&WorkVector[0], &WorkVector[7], &WorkVector[4], &TransparentBlue);
#endif //0

				/* draw lines that represent the frustum */

				gfxDrawLineWC(&WorkVector[0], &WorkVector[1], &Blue);
				gfxDrawLineWC(&WorkVector[1], &WorkVector[2], &Blue);
				gfxDrawLineWC(&WorkVector[2], &WorkVector[3], &Blue);
				gfxDrawLineWC(&WorkVector[3], &WorkVector[0], &Blue);

				gfxDrawLineWC(&WorkVector[4], &WorkVector[5], &Blue);
				gfxDrawLineWC(&WorkVector[5], &WorkVector[6], &Blue);
				gfxDrawLineWC(&WorkVector[6], &WorkVector[7], &Blue);
				gfxDrawLineWC(&WorkVector[7], &WorkVector[4], &Blue);

				gfxDrawLineWC(&WorkVector[0], &WorkVector[4], &Blue);
				gfxDrawLineWC(&WorkVector[1], &WorkVector[5], &Blue);
				gfxDrawLineWC(&WorkVector[2], &WorkVector[6], &Blue);
				gfxDrawLineWC(&WorkVector[3], &WorkVector[7], &Blue);
			}
#endif //CONSUMER_BUILD

			/* re-render instances that intersect light frustum */

#ifdef _DEBUG
			ShadowMgr.pCurrentFrustum = &pShadow->pLightFrustum[i]->Frustum;
#endif //_DEBUG


			SceneMgr::RerenderScene(pScene, &pShadow->pLightFrustum[i]->Frustum, pShadow->pInst);

			/* pop texture matrix */

			TextureState__RequestUnit(0);	// shadow texture is on unit 0
			MatrixStack__Mode(MS_TEXTURE);
			MatrixStack__Pop();

#ifdef DIRECTX
	DirectX_Device->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE  );
	DirectX_Device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
#endif
			/* unlock texture states */

			realtimeTexture->Unselect();
			realtimeTexture->Unlock();
			TextureState__UnlockAll();

#ifdef GCN
			// TJC - not sure what to do here... see the lock function...
			LOCKUP();
#endif
		}
	}

	/* restore */

	RenderMgr__ProjectedTextureRestore();
	RenderMgr__UnlockDiffuseRender();

#ifdef GCN
	RenderState__Unlock( RS_FOG);
	RenderState__Request( RS_FOG, oldFogging);
	RenderState__Lock( RS_FOG);
#endif

	/* clear status */

	ShadowMgr.bRenderingShadows = FALSE;
}

#if defined(DIRECTX)
void ShadowMgr__DrawFrustum( ts_Frustum & Frustum, Graphics4x4 & FrustumToWorld, Graphics4x4 & InvFrustumProjection );

void ShadowMgr__ClearInstanceBits( ts_Shadow * pShadow )
{
	for( int i = 0; i < pShadow->nLightCount; i++ )
		pShadow->bInstanceShadowedByLight[i] = false;

	pShadow->uInstanceShadowCount = 0;
}

void ShadowMgr__ClearAllInstanceBits( void )
{
	ts_Shadow * pShadow = (ts_Shadow *) ShadowMgr.ShadowActive.pHead;

	while( pShadow )
	{
		ShadowMgr__ClearInstanceBits( pShadow );
		pShadow = pShadow->pNext;
	}

	InstanceShadowCount = 0;
}

void ShadowMgr__SetInstanceBits( CInstance * pInst )
{
	ASSERT( ShadowMgr.pShadowAlloc );	// if this hits, ShadowMgr__Init hasn't been called
	ASSERT( pInst );

	/* shadow manager must have been initialized, and be active */
	if( (ShadowMgr.bActive == FALSE)  ||  (ShadowMgr.ShadowActive.pHead == NULL)  ||
		(ShadowMgr.bShadowsCreated == false))
	{
		ShadowMgr__ClearAllInstanceBits( );
		return;
	}

	const ts_Frustum * pOldFrustum = Frustum__GetCurrentFrustum( );

	InstanceShadowCount = 0;

	/* check against all shadows */
	ts_Shadow * pShadow = (ts_Shadow *) ShadowMgr.ShadowActive.pHead;

	while( pShadow )
	{

		/* nothing to do if caster is hidden */
		if( pShadow->pInst->Hidden( )  ||  (pShadow->pInst == pInst) )
		{
			ShadowMgr__ClearInstanceBits( pShadow );
		}
		else
		{
			pShadow->uInstanceShadowCount = 0;

			for( int i = 0; i < pShadow->nLightCount; i++)
			{
				Graphics4x4 mLightToWorld, mInvProj;

				pShadow->pWorldToLight[i]->mMatrix.Invert( mLightToWorld );
				pShadow->pFrustumProjection[i]->mMatrix.Invert( mInvProj );

				pShadow->bInstanceShadowedByLight[i] = Frustum__IntersectsWorldBounds(
							&pShadow->pLightFrustum[i]->Frustum,
							&pInst->BoundingSphere( ),
							&pInst->BoundingBox( ),
							NULL,
							NULL,
							pInst->FarClipDisabled( ) );

				if( pShadow->bInstanceShadowedByLight[i] )
					pShadow->uInstanceShadowCount++;

			}

			InstanceShadowCount += pShadow->uInstanceShadowCount;
		}

		pShadow = pShadow->pNext;
	}

	Frustum__SetupToCheckFrustum( pOldFrustum );
}

static float fScaleX = -.5f;
static float fScaleY = .5f;
static float fScaleZ = -1.0f;

static float fBiasX = .5f;
static float fBiasY = .5f;

extern D3DXMATRIX _MatrixStack_RHProjectionToTexture;

void ShadowMgr__ApplyShadows( unsigned int & startTextureUnit )
{
	if( (InstanceShadowCount == 0)  || (startTextureUnit == 0) )
		return;

	ts_Shadow * pShadow = (ts_Shadow *) ShadowMgr.ShadowActive.pHead;
	CScene * pScene = SceneMgr::GetCurrentScene( );
	while( pShadow )
	{
		if( pShadow->uInstanceShadowCount )
		{
			for( int i = 0; i < pShadow->nLightCount; i++ )
			{
				if( pShadow->bInstanceShadowedByLight[i] )
				{
					Image * pImage = pShadow->nRealtimeShadowTexture[i]->GetImage( );
					if( pImage )
					{
						HRESULT result = D3D_OK;

						result = DirectX_Device->SetTextureStageState( startTextureUnit, D3DTSS_COLOROP, D3DTOP_MODULATE );
						ASSERT( result == D3D_OK );
						result = DirectX_Device->SetTextureStageState( startTextureUnit, D3DTSS_COLORARG1, D3DTA_TEXTURE );
						ASSERT( result == D3D_OK );
						result = DirectX_Device->SetTextureStageState( startTextureUnit, D3DTSS_COLORARG2, D3DTA_CURRENT );
						ASSERT( result == D3D_OK );

						result = DirectX_Device->SetTextureStageState( startTextureUnit, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
						ASSERT( result == D3D_OK );
						result = DirectX_Device->SetTextureStageState( startTextureUnit, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
						ASSERT( result == D3D_OK );

						result = DirectX_Device->SetTextureStageState( startTextureUnit, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
						ASSERT( result == D3D_OK );
						result = DirectX_Device->SetTextureStageState( startTextureUnit, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
						ASSERT( result == D3D_OK );
						result = DirectX_Device->SetTextureStageState( startTextureUnit, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
						ASSERT( result == D3D_OK );
						result = DirectX_Device->SetTextureStageState( startTextureUnit, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
						ASSERT( result == D3D_OK );

						Graphics4x4 CameraToTexture = pScene->d_cameraToWorld * pShadow->pWorldToTexture[i]->mMatrix;

						result = DirectX_Device->SetTransform( (D3DTRANSFORMSTATETYPE) (D3DTS_TEXTURE0 + startTextureUnit), (D3DXMATRIX *) & CameraToTexture );
						ASSERT( result == D3D_OK );
						result = DirectX_Device->SetTextureStageState( startTextureUnit, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );
						ASSERT( result == D3D_OK );
						result = DirectX_Device->SetTextureStageState( startTextureUnit, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
						ASSERT( result == D3D_OK );

						TextureState__RequestUnit( startTextureUnit );

						pImage->Select( );

						startTextureUnit++;
					}
				}
			}
		}

		pShadow = pShadow->pNext;
	}

}

#if defined(_XBOX)

extern D3DXMATRIX _MatrixStack_Identity;

void ShadowMgr__DrawFrustum( ts_Frustum & Frustum, Graphics4x4 & FrustumToWorld, Graphics4x4 & InvFrustumProjection )
{
	struct
	{
		D3DXVECTOR3	Pos;
		D3DCOLOR		RGBs;
	} Verts[8];

	Vector3 worldCorners[8];

	for( unsigned int i = 0; i < 8; i++ )
		worldCorners[i] = Frustum.Corners[i] * FrustumToWorld;

	HRESULT result = D3D_OK;
	result = DirectX_Device->SetRenderState( D3DRS_LIGHTING, FALSE );
	result = DirectX_Device->SetRenderState( D3DRS_FOGENABLE, FALSE );

	D3DBaseTexture * pOldTexture;

	result = DirectX_Device->GetTexture( 0, & pOldTexture );
	result = DirectX_Device->SetTexture( 0, NULL );

	result = DirectX_Device->SetVertexShader( D3DFVF_XYZ | D3DFVF_DIFFUSE );
	assert( result == D3D_OK );

	DirectX_Device->SetTransform( D3DTS_WORLD, & _MatrixStack_Identity );

	DirectX_Device->SetDebugMarker( 3 );

	Verts[0].RGBs = D3DCOLOR_XRGB( 0xff, 0, 0 );
	Verts[1].RGBs = D3DCOLOR_XRGB( 0xff, 0, 0 );
	Verts[2].RGBs = D3DCOLOR_XRGB( 0xff, 0, 0 );
	Verts[3].RGBs = D3DCOLOR_XRGB( 0xff, 0, 0 );

	Verts[0].Pos.x = worldCorners[0].x( );
	Verts[0].Pos.y = worldCorners[0].y( );
	Verts[0].Pos.z = worldCorners[0].z( );

	Verts[1].Pos.x = worldCorners[1].x( );
	Verts[1].Pos.y = worldCorners[1].y( );
	Verts[1].Pos.z = worldCorners[1].z( );

	Verts[2].Pos.x = worldCorners[2].x( );
	Verts[2].Pos.y = worldCorners[2].y( );
	Verts[2].Pos.z = worldCorners[2].z( );

	Verts[3].Pos.x = worldCorners[3].x( );
	Verts[3].Pos.y = worldCorners[3].y( );
	Verts[3].Pos.z = worldCorners[3].z( );

	result = DirectX_Device->DrawVerticesUP( D3DPT_LINELOOP, 4, Verts, sizeof( Verts[0] ) );

	Verts[0].Pos.x = worldCorners[3].x( );
	Verts[0].Pos.y = worldCorners[3].y( );
	Verts[0].Pos.z = worldCorners[3].z( );

	Verts[1].Pos.x = worldCorners[2].x( );
	Verts[1].Pos.y = worldCorners[2].y( );
	Verts[1].Pos.z = worldCorners[2].z( );

	Verts[2].Pos.x = worldCorners[6].x( );
	Verts[2].Pos.y = worldCorners[6].y( );
	Verts[2].Pos.z = worldCorners[6].z( );

	Verts[3].Pos.x = worldCorners[7].x( );
	Verts[3].Pos.y = worldCorners[7].y( );
	Verts[3].Pos.z = worldCorners[7].z( );

	result = DirectX_Device->DrawVerticesUP( D3DPT_LINELOOP, 4, Verts, sizeof( Verts[0] ) );

	Verts[0].Pos.x = worldCorners[4].x( );
	Verts[0].Pos.y = worldCorners[4].y( );
	Verts[0].Pos.z = worldCorners[4].z( );

	Verts[1].Pos.x = worldCorners[5].x( );
	Verts[1].Pos.y = worldCorners[5].y( );
	Verts[1].Pos.z = worldCorners[5].z( );

	Verts[2].Pos.x = worldCorners[6].x( );
	Verts[2].Pos.y = worldCorners[6].y( );
	Verts[2].Pos.z = worldCorners[6].z( );

	Verts[3].Pos.x = worldCorners[7].x( );
	Verts[3].Pos.y = worldCorners[7].y( );
	Verts[3].Pos.z = worldCorners[7].z( );

	result = DirectX_Device->DrawVerticesUP( D3DPT_LINELOOP, 4, Verts, sizeof( Verts[0] ) );

	Verts[0].Pos.x = worldCorners[4].x( );
	Verts[0].Pos.y = worldCorners[4].y( );
	Verts[0].Pos.z = worldCorners[4].z( );

	Verts[1].Pos.x = worldCorners[5].x( );
	Verts[1].Pos.y = worldCorners[5].y( );
	Verts[1].Pos.z = worldCorners[5].z( );

	Verts[2].Pos.x = worldCorners[1].x( );
	Verts[2].Pos.y = worldCorners[1].y( );
	Verts[2].Pos.z = worldCorners[1].z( );

	Verts[3].Pos.x = worldCorners[0].x( );
	Verts[3].Pos.y = worldCorners[0].y( );
	Verts[3].Pos.z = worldCorners[0].z( );

	result = DirectX_Device->DrawVerticesUP( D3DPT_LINELOOP, 4, Verts, sizeof( Verts[0] ) );

	Verts[0].RGBs = D3DCOLOR_XRGB( 0, 0xff, 0 );
	Verts[1].RGBs = D3DCOLOR_XRGB( 0, 0xff, 0 );
	Verts[2].RGBs = D3DCOLOR_XRGB( 0, 0xff, 0 );
	Verts[3].RGBs = D3DCOLOR_XRGB( 0, 0xff, 0 );
	Verts[4].RGBs = D3DCOLOR_XRGB( 0, 0xff, 0 );
	Verts[5].RGBs = D3DCOLOR_XRGB( 0, 0xff, 0 );
	Verts[6].RGBs = D3DCOLOR_XRGB( 0, 0xff, 0 );
	Verts[7].RGBs = D3DCOLOR_XRGB( 0, 0xff, 0 );

	for( int p = 0; p < 8; p++ )
    {
        D3DXVECTOR4 vT( 0.0f, 0.0f, 0.0f, 1.0f );

        vT.x = (p%2) * ( p&0x2 ? -1.0f : 1.0f );
        vT.y = (p%2) * ( p&0x4 ? -1.0f : 1.0f );
        vT.z = (p%2) * ( 1.0f );

        D3DXVec4Transform( &vT, &vT, (D3DXMATRIX *) &InvFrustumProjection );
        D3DXVec4Transform( &vT, &vT, (D3DXMATRIX *) &FrustumToWorld );

        Verts[p].Pos.x = vT.x / vT.w;
        Verts[p].Pos.y = vT.y / vT.w;
        Verts[p].Pos.z = vT.z / vT.w;
    }

	DirectX_Device->DrawPrimitiveUP( D3DPT_LINELIST, 4, Verts, sizeof( Verts[0] ) );

	result = DirectX_Device->SetTexture( 0, pOldTexture );

	if( pOldTexture )
		pOldTexture->Release( );

}
#endif
#endif

/* grab a record from free pool, assign handle, return handle */

ts_Shadow *ShadowMgr__AllocShadowRecord(void)
{
	/* grab a record from free pool */

	ts_Shadow *pShadow = (ts_Shadow *) SLinkList__GetAndRemoveHead(&ShadowMgr.ShadowFree);
	ASSERT(pShadow);	// shouldn't run out of records. if so, increase nMaxShadows count passed to scnInit
	if (pShadow == NULL)
		return(NULL);

	/* build handle */

	ASSERT(pShadow->Handle == INVALID_HANDLE);			// record should not have a handle assigned
	HANDLE_INDEX(pShadow->Handle) = pShadow - ShadowMgr.pShadowAlloc;	// index into allocated array of records
	HANDLE_ID(pShadow->Handle) = ShadowMgr.u16ID++;
	if (pShadow->Handle == INVALID_HANDLE)
		HANDLE_ID(pShadow->Handle) = ShadowMgr.u16ID++;	// do it again to avoid INVALID_HANDLE
	ASSERT(pShadow->Handle != INVALID_HANDLE);			// record should have a valid handle

	/* clear record */

	pShadow->pInst = NULL;
	pShadow->nLightCount = 0;

	/* success */

	return(pShadow);
}

/* free alloc'd stuff from shadow */

void Shadow__Free(ts_Shadow *pShadow, int nIndex)
{
	ASSERT(nIndex < pShadow->nLightCount);
	if (pShadow->nRealtimeShadowTexture[nIndex] != NULL)
	{
		delete pShadow->nRealtimeShadowTexture[nIndex];
		pShadow->nRealtimeShadowTexture[nIndex] = NULL;
	}
	if (pShadow->pLightFrustum[nIndex])
	{
		FrustumPool__Free(pShadow->pLightFrustum[nIndex]);
		pShadow->pLightFrustum[nIndex] = NULL;
	}
	if (pShadow->pFrustumProjection[nIndex])
	{
		MatrixPool__Free(pShadow->pFrustumProjection[nIndex]);
		pShadow->pFrustumProjection[nIndex] = NULL;
	}
	if (pShadow->pWorldToLight[nIndex])
	{
		MatrixPool__Free(pShadow->pWorldToLight[nIndex]);
		pShadow->pWorldToLight[nIndex] = NULL;
	}

#if defined(DIRECTX)
	if (pShadow->pWorldToTexture[nIndex])
	{
		MatrixPool__Free( pShadow->pWorldToTexture[nIndex]);
		pShadow->pWorldToTexture[nIndex] = NULL;
	}
#endif
}


// ----------------------------------------------------------------------------
// --- update the shadow projection if the light direction changed
// ----------------------------------------------------------------------------
bool ShadowMgr__UpdateLight( t_Handle ShadowHandle, ts_Light *pLight, float fRadiusAdjust )
{
	int i;
	int light=0;
	ts_Shadow *pShadow;

	/* point to shadow, error if invalid id */

	ASSERT(ShadowHandle != INVALID_HANDLE);
	pShadow = SHADOW_GET_RECORD(ShadowHandle);

	ASSERT(pShadow->Handle == ShadowHandle);			// error if id does not match. this means that record got deallocated (and maybe allocated again) somewhere else
	if (pShadow->Handle != ShadowHandle)
		return(FALSE);

	// --- find the light
	for (i = 0; i < pShadow->nLightCount; i++)
	{
		if (pShadow->pLights[i] == pLight)
		{
			light=i;
			break;
		}
	}

	// --- construct the frustum again
	float ShadowNearClip = SHADOW_PULLBACK - *pShadow->pRadius;	// SHADOW_PULLBACK cuts through origin of instance
	float ShadowFarClip = SHADOW_PULLBACK + SHADOW_FAR_CLIP_DELTA;
	float ShadowFOVDegrees = Math::ArcTanDeg( *pShadow->pRadius * 2.0f * fRadiusAdjust / SHADOW_PULLBACK);
	Frustum__Build(
		&pShadow->pLightFrustum[light]->Frustum,
		ShadowNearClip,
		ShadowFarClip,
		FALSE,									// fog not a factor for shadow frustum
		0.0f,										// fog start
		0.0f,										// fog end
		ShadowFOVDegrees,						// degrees
		SHADOW_ASPECT_RATIO);

	/* low-angle lights cause problems with shadows, since they get cast over a long distance. ensure light angle */
	/* isn't too shallow */

	Vector3 LightDir = pLight->Pos;	// points from origin to light, so y-value is typically positive
	float fLengthXZ = LightDir.LengthXZ();
	float fRadians = Math::ArcTan2(LightDir.y(), fLengthXZ);
#define MIN_LIGHT_ANGLE	60.0f	// in degrees, noon sun is 90
	if (fRadians < Math::Deg2Rad(MIN_LIGHT_ANGLE))
	{
		/* light is too shallow, adjust it. this means that shadow will not match real-time lighting, but if it comes */
		/* from roughly the same direction, it should be ok. tan(angle) = opposite/adjacent. in this case, adjacent */
		/* is x/z-length, and opposite is y-length. so we can calculate desired y-length as opposite = adjacent * tan */

		LightDir.y(fLengthXZ * Math::Tan(Math::Deg2Rad(MIN_LIGHT_ANGLE)));
		LightDir.Normalize();
	}
	pShadow->LightDelta[light] = LightDir * SHADOW_PULLBACK;

	/* calculate projection matrix for this frustum shape */

	MatrixStack__Mode( MS_PROJECTION);
	MatrixStack__Push( );
	MatrixStack__Identity( );
#if defined(DIRECTX)
	MatrixStack__Perspective( ShadowFOVDegrees, SHADOW_ASPECT_RATIO, 1.0f, 200.0f );
#else
	MatrixStack__Perspective(ShadowFOVDegrees, SHADOW_ASPECT_RATIO, ShadowNearClip, ShadowFarClip);
#endif
	memcpy( &pShadow->pFrustumProjection[light]->mMatrix, MatrixStack__Get(MS_PROJECTION), sizeof(Graphics4x4));
	MatrixStack__Pop( );
	return(TRUE);
}
