/*************************************/
/*                                   */
/*   LightMgr.c                      */
/*   new engine  12/07/00 (rename)   */
/*   original file created 11/6/98   */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   light manager                   */
/*                                   */
/*************************************/

/* the light manager manages lights at a high level. it typically passes off the active (and maybe judges "importance")
lights to the "hardware" light manager-- that is, the graphics subsystem light manager, like OpenGL or DirectX. the light
manager can also do lighting itself in "software" mode if necessary */

/******************* includes ****************************/

#include "Game/GamePCH.h"



/******************* defines *****************************/

/******************* macros ******************************/

#define LIGHT_MGR_GET_RECORD(Handle)	&LightMgr.pLightAlloc[HANDLE_INDEX(Handle)]

/******************* structures **************************/

/* entry in the light manager's free and active lists */

struct ts_LightRecord
{
	ts_LightRecord			*pNext;	// linked list pointer-- does not have to be first

	/* light data. we keep this as a separate record within the light record because it gets passed to other routines */

	ts_Light					Light;

	/* assigned handle for invalid handle checking */

	t_Handle					Handle;

	/* for texture-projection spotlight effect. i figure that the light manager is the best place for this */

	ts_PoolFrustum			*pLightFrustum;	// frustum that matches shape of requested texture spotlight (fov and far clip)
	ts_PoolMatrix			*pProjection;		// projection matrix for texture projection
	ts_PoolMatrix			*pWorldToLight;	// just like world-to-camera-- convert from world into light space
	ts_PoolMatrix			*pLightToWorld;	// inverse of world-to-light, for transforming frustum into world coords
	Vector3					vDirection;

	Texture*					pTexture;
};

/* light manager is basically just an active list and a free list of light */

struct ts_LightMgr
{
	/* record passed initialization values */

	int						nMaxLights;

	/* light records */

	ts_LightRecord			*pLightAlloc;	// original alloc'd buffer
	ts_SLinkList			FreeList;
	ts_SLinkList			ActiveList;
	ts_SLinkList			SaveDirectionalList;

	/* for handle assignment (id portion of handle) */

	u16						u16ID;

	/* ambient light */

	ts_fRGB					AmbientLight;
	ts_fRGB					SaveAmbientLight;

	/* adjustment multiplier for light colors */

	float						fLightColorMult;
	bool						bLightColorMultDirty;

	/* any active effect lights in the list? */

	bool						bActiveEffectLights;

	/* sun-- the brightest infinite directional light */

	ts_Light					*pSun;
};

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

/* the light manager */

static ts_LightMgr LightMgr;
static ts_Light	sunLight;

/******************* local prototypes ********************/

#ifndef CONSUMER_BUILD
bool LightMgr__IsEmpty(void);
#endif //CONSUMER_BUILD
void LightMgr__UpdateTextureSpotlight(ts_LightRecord *pLightRecord, Vector3 &vPos, Vector3 &vDirection);
ts_LightRecord *LightMgr__AllocRecord(void);
void LightMgr__SetSun(void);
void Color__Clamp(ts_fRGB *pColor);
void LightMgr__RenderTextureSpotlight(ts_LightRecord *pLightRecord, CScene *pScene);
void LightMgr__CheckForActiveEffectLights();

/******************* functions ***************************/

/* initialize the manager */

void LightMgr__Init(int nMaxLights)
{
	ts_LightRecord *pLightRecord;

	/* clear manager */

	memset(&LightMgr, 0, sizeof(ts_LightMgr));

	/* linked lists */

	ASSERT(nMaxLights < MAX_HANDLES);	// see definition of MAX_HANDLES
	LightMgr.nMaxLights = nMaxLights;
	MEM_SET_ALLOC_NAME("LightMgr list");
	LightMgr.pLightAlloc = (ts_LightRecord *) memAlloc(LightMgr.nMaxLights * sizeof( ts_LightRecord));
	SLinkList__Init(&LightMgr.FreeList, offsetof(ts_LightRecord, pNext));
	SLinkList__LinkBlock(&LightMgr.FreeList, LightMgr.pLightAlloc, LightMgr.nMaxLights, sizeof(ts_LightRecord));
	SLinkList__Init(&LightMgr.ActiveList, offsetof(ts_LightRecord, pNext));
	SLinkList__Init(&LightMgr.SaveDirectionalList, offsetof(ts_LightRecord, pNext));

	/* clear all handles */

	pLightRecord = (ts_LightRecord *) LightMgr.FreeList.pHead;
	while (pLightRecord)
	{
		pLightRecord->Handle = INVALID_HANDLE;
		pLightRecord = pLightRecord->pNext;
	}

	/* default ambient light */

	LightMgr.AmbientLight.R = 0.2f;
	LightMgr.AmbientLight.G = 0.2f;
	LightMgr.AmbientLight.B = 0.2f;

	/* light color multiplier */

	LightMgr.fLightColorMult = 1.0f;
	LightMgr.bLightColorMultDirty = FALSE;
}

/* reset */

void LightMgr__Reset(void)
{
	/* restore ambient light to default value */

	ts_fRGB DefaultColor = {RS_DEFAULT_AMBIENT_RED, RS_DEFAULT_AMBIENT_GREEN, RS_DEFAULT_AMBIENT_BLUE};
	RenderState__RegisterHardwareAmbientLight(DefaultColor);

	/* remove all lights */

	LightMgr__RemoveAllLights();
}

/* shutdown */

void LightMgr__Shutdown(void)
{
	/* maybe not active */

	if (LightMgr.pLightAlloc == NULL)
		return;

	/* remove all lights so hardware light mnager is clean */

	LightMgr__Reset();

	/* free alloc'd buffer */

	memFree(LightMgr.pLightAlloc);

	/* clear the manager */

	memset(&LightMgr, 0, sizeof(ts_LightMgr));
}

#ifndef CONSUMER_BUILD
bool LightMgr__IsEmpty(void)
{
	return(SLinkList__Check(&LightMgr.FreeList) == (signed) LightMgr.nMaxLights);
}
#endif //CONSUMER_BUILD

void LightMgr__SetAmbient(ts_fRGB *pColor)
{
	LightMgr.AmbientLight = *pColor;

//yuch-- brighten realtime lighting for ps2
#ifdef PS2
	LightMgr.AmbientLight.R *= 2.0f;
	LightMgr.AmbientLight.G *= 2.0f;
	LightMgr.AmbientLight.B *= 2.0f;
#endif //PS2

	Color__Clamp(&LightMgr.AmbientLight);

	/* set hardware ambient light */

	RenderState__RegisterHardwareAmbientLight(LightMgr.AmbientLight);
}

ts_fRGB *LightMgr__GetAmbient(void)
{
	return(&LightMgr.AmbientLight);
}

void LightMgr__AdjustAmbient(ts_fRGB *pColor)
{
	LightMgr.AmbientLight.R += pColor->R;
	LightMgr.AmbientLight.G += pColor->G;
	LightMgr.AmbientLight.B += pColor->B;

	Color__Clamp(&LightMgr.AmbientLight);

	/* set hardware ambient light */

	RenderState__RegisterHardwareAmbientLight(LightMgr.AmbientLight);
}

/* temporarily modify lights. if you use this, use LightMgr__RestoreLights to restore it */

void LightMgr__AdjustLights(ts_fRGB Color, float fLightColorMult)
{
	RenderState__RegisterHardwareAmbientLight(Color);
	LightMgr__SetLightColorMult(fLightColorMult);
}
void LightMgr__RestoreLights(void)
{
	RenderState__RegisterHardwareAmbientLight(LightMgr.AmbientLight);
	LightMgr__SetLightColorMult(1.0f);
}

/* keep track of light color multiplier status */

void LightMgr__SetLightColorMult(float fLightColorMult)
{
	if (fLightColorMult != LightMgr.fLightColorMult)
	{
		LightMgr.fLightColorMult = fLightColorMult;
		LightMgr.bLightColorMultDirty = TRUE;
	}
}
void LightMgr__GetLightColorMult(bool *pbLightColorMultDirty, float *pfLightColorMult)
{
	if (pbLightColorMultDirty)
		*pbLightColorMultDirty = LightMgr.bLightColorMultDirty;
	if (pfLightColorMult)
		*pfLightColorMult = LightMgr.fLightColorMult;
}
void LightMgr__ClearLightColorMultDirty(void)
{
 	LightMgr.bLightColorMultDirty = FALSE;
}

t_Handle LightMgr__RegisterLight(ts_Light *pLight)
{
	/* light manager must have been initialized */

	ASSERT(LightMgr.pLightAlloc != NULL);			// if this hits, LightMgr__Init hasn't been called
	ASSERT(pLight->u32Flags & LIGHT_LIGHTING_TYPE);	// check type

	/* grab an instance record from free list */

	ts_LightRecord *pLightRecord = LightMgr__AllocRecord();
	if (pLightRecord == NULL)
		return(INVALID_HANDLE);

	/* add to head of active list */

	SLinkList__AddLinkToHead(&LightMgr.ActiveList, pLightRecord);

	/* copy in passed light */

	pLightRecord->Light = *pLight;

	/* set-up */

	if (pLightRecord->Light.u32Flags & LIGHT_DIRECTIONAL)		//directional lights: position is lighting direction - so normalize
		pLightRecord->Light.Pos.Normalize();

//yuch-- brighten realtime lighting for ps2
#ifdef PS2
	pLightRecord->Light.Color.R *= 2.0f;
	pLightRecord->Light.Color.G *= 2.0f;
	pLightRecord->Light.Color.B *= 2.0f;
#endif //PS2

	/* register for rendering */

	RenderState__RegisterHardwareLight(&pLightRecord->Light);

	/* for instance-in-shadow detection, keep track of the opposite of the direction of the strongest infinite */
	/* directional light */

	LightMgr__SetSun();

	/* done */

	return(pLightRecord->Handle);
}

/* update a light's position (direction for infinite directional lights) */

bool LightMgr__UpdateLightPos(t_Handle Handle, Vector3 *pvPos)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_LightRecord *pLightRecord = LIGHT_MGR_GET_RECORD(Handle);
	if (pLightRecord->Handle != Handle)
		return(false);

	/* update light position */

	pLightRecord->Light.Pos = *pvPos;
	if (pLightRecord->Light.u32Flags & LIGHT_DIRECTIONAL)		//directional lights: position is lighting direction - so normalize
		pLightRecord->Light.Pos.Normalize();

	/* success */

	return(true);
}

/* update a light's color */

bool LightMgr__UpdateLightColor(t_Handle Handle, ts_fRGB *pColor)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_LightRecord *pLightRecord = LIGHT_MGR_GET_RECORD(Handle);
	if (pLightRecord->Handle != Handle)
		return(false);

	/* update light color */

	if (memcmp(&pLightRecord->Light.Color, pColor, sizeof(pLightRecord->Light.Color)) != 0)
	{
		pLightRecord->Light.Color = *pColor;
		LightMgr.bLightColorMultDirty = true;	// this tells RenderState__SendHardwareLights that color has changed
	}

	/* success */

	return(true);
}

/* the texture-projection spotlight is sort of a light...this is an effect where a texture is projected onto an already-
rendered scene, a lot like rendering realtime shadows-- the vertex positions are transformed into texture coordinates.
if the texture is a white circle on a black background, the result looks like a spotlight shining on the world */

t_Handle	LightMgr__RegisterTextureSpotlight(Vector3 &vPos, Vector3 &vDirection, float fNearClip, float fFarClip, float fFieldOfViewDegrees, const char *pTextureFile)
{
	/* light manager must have been initialized */

	ASSERT(LightMgr.pLightAlloc != NULL);			// if this hits, LightMgr__Init hasn't been called

	/* load texture file-- we're looking for the texture to use to create the spotlight */

	t_Handle TextureHandle = dblLoad(pTextureFile);
	if (TextureHandle == INVALID_HANDLE)
	{
		ASSERT(false);	// couldn't find texture set file
		return(INVALID_HANDLE);
	}
	TextureDatabase* tdb = (TextureDatabase*)dblGetChunkData(DB_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, TextureHandle);
	// TODO: detect 'no textures in file' condition.

	/* grab a record from free list */

	ts_LightRecord *pLightRecord = LightMgr__AllocRecord();
	if (pLightRecord == NULL)
		return(INVALID_HANDLE);

	/* add to head of active list */

	SLinkList__AddLinkToHead(&LightMgr.ActiveList, pLightRecord);

	/* fill in record */

	pLightRecord->Light.u32Flags = LIGHT_ON | LIGHT_TEXTURE_SPOTLIGHT;
	pLightRecord->Light.Pos = vPos;
	pLightRecord->vDirection = vDirection;
	LOCKUP();
	// get the texture here..., but pLightRecord has no member pTexture yet...
	pLightRecord->pTexture = tdb->GetTexture(0);
//pLightRecord->nBind = 2;
	pLightRecord->pLightFrustum = FrustumPool__Alloc();
	ASSERT(pLightRecord->pLightFrustum);
	pLightRecord->pProjection = MatrixPool__Alloc(pLightRecord);
	ASSERT(pLightRecord->pProjection);
	pLightRecord->pWorldToLight = MatrixPool__Alloc(pLightRecord);
	ASSERT(pLightRecord->pWorldToLight);
	pLightRecord->pLightToWorld = MatrixPool__Alloc(pLightRecord);
	ASSERT(pLightRecord->pLightToWorld);

	/* set up frustum */

	#define TEXTURE_SPOTLIGHT_ASPECT_RATIO	1.0f	// square
	Frustum__Build(
		&pLightRecord->pLightFrustum->Frustum,	// ts_Frustum *pFrustum
		fNearClip,									// float fNearClip
		fFarClip,										// float fFarClip
		false,											// bool bFogOn-- fog not a factor for shadow frustum
		0.0f,												// float fFogStart
		0.0f,												// float fFogEnd
		fFieldOfViewDegrees,							// float fFieldOfViewYDegrees
		TEXTURE_SPOTLIGHT_ASPECT_RATIO);			// float fAspectRatio

	/* calculate projection matrix for this frustum shape */

	MatrixStack__Mode( MS_PROJECTION);
	MatrixStack__Push();
	MatrixStack__Identity();
	MatrixStack__Perspective(fFieldOfViewDegrees, TEXTURE_SPOTLIGHT_ASPECT_RATIO, fNearClip, fFarClip);
	memcpy(&pLightRecord->pProjection->mMatrix, MatrixStack__Get(MS_PROJECTION), sizeof(Graphics4x4));
	MatrixStack__Pop();

	/* fill in world-to-light matrix */

	LightMgr__UpdateTextureSpotlight(pLightRecord, vPos, vDirection);

	/* done */

	LightMgr.bActiveEffectLights = true;
	return(pLightRecord->Handle);
}

/* update position/direction of a texture spotlight light */

void LightMgr__UpdateTextureSpotlight(ts_LightRecord *pLightRecord, Vector3 &vPos, Vector3 &vDirection)
{
	pLightRecord->Light.Pos = vPos;
	pLightRecord->vDirection = vDirection;

	/* calculate world-to-light matrix-- transform from world coords to light space-- just like a world-to-camera matrix */

	Vector3 vFocal = vPos + vDirection;
	BuildCTM(vPos, vFocal, vUp, &pLightRecord->pWorldToLight->mMatrix);

	/* and we need inverse too */

	pLightRecord->pWorldToLight->mMatrix.Invert(pLightRecord->pLightToWorld->mMatrix);
}
bool LightMgr__UpdateTextureSpotlight(t_Handle Handle, Vector3 &vPos, Vector3 &vDirection)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_LightRecord *pLightRecord = LIGHT_MGR_GET_RECORD(Handle);
	if (pLightRecord->Handle != Handle)
		return(false);
	LightMgr__UpdateTextureSpotlight(pLightRecord, vPos, vDirection);
	return(true);
}

/* remove a light from list */

bool LightMgr__RemoveLight(t_Handle Handle)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_LightRecord *pLightRecord = LIGHT_MGR_GET_RECORD(Handle);
	if (pLightRecord->Handle != Handle)
		return(FALSE);

	/* clear handle */

	pLightRecord->Handle = INVALID_HANDLE;

	/* move to free list */

	RenderState__RemoveHardwareLight(&pLightRecord->Light);
	SLinkList__RemoveLink(&LightMgr.ActiveList, pLightRecord);
	SLinkList__AddLinkToHead(&LightMgr.FreeList, pLightRecord);

	/* for instance-in-shadow detection, keep track of the opposite of the direction of the strongest infinite */
	/* directional light */

	LightMgr__SetSun();

	/* any active effect lights? */

	LightMgr__CheckForActiveEffectLights();

	/* success */

	return(TRUE);
}

/* remove all lights */
void LightMgr__RemoveAllLights( void )
{
  	while( LightMgr.ActiveList.pHead )
  		LightMgr__RemoveLight( ((ts_LightRecord *) LightMgr.ActiveList.pHead)->Handle );

	ASSERT( LightMgr__IsEmpty( ) );

  	/* hardware light list should be empty once all lights are removed */
  	ASSERT( RenderState__HardwareLightsEmpty( ) );
}

/* remove all lights except effect lights */
void LightMgr__RemoveAlmostAllLights( void )
{
	ts_LightRecord **				ppList;

	ppList = (ts_LightRecord **) & LightMgr.ActiveList.pHead;

	while( *ppList )
	{
		ts_LightRecord * pRecord = *ppList;

		if( (pRecord->Light.u32Flags & LIGHT_EFFECT_TYPE) == LIGHT_TEXTURE_SPOTLIGHT )
			ppList = & pRecord->pNext;
		else
			LightMgr__RemoveLight( pRecord->Handle );
	}
}

/* turn on/off requested light */

bool LightMgr__EnableLight(t_Handle Handle, bool bEnable)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_LightRecord *pLightRecord = LIGHT_MGR_GET_RECORD(Handle);
	if (pLightRecord->Handle != Handle)
		return(false);

	/* on/off */

	if (bEnable) 
	{
		/* nothing to do if already on */

		if (pLightRecord->Light.u32Flags & LIGHT_ON)
			return(true);

		/* turn on */

		pLightRecord->Light.u32Flags |= LIGHT_ON;
		RenderState__RegisterHardwareLight(&pLightRecord->Light);
	}
	else
	{
		/* nothing to do if already off */

		if ((pLightRecord->Light.u32Flags & LIGHT_ON) == 0)
			return(true);

		/* turn off */

		pLightRecord->Light.u32Flags &= ~LIGHT_ON;
		RenderState__RemoveHardwareLight(&pLightRecord->Light);
	}

	/* for shadow generation, keep track of strongest directional light */

	LightMgr__SetSun();

	/* any active effect lights? */

	LightMgr__CheckForActiveEffectLights();

	/* success */

	return(true);
}

/* for cut scene support, remove ambient light and save it off */

void LightMgr__SaveAmbientLight()
{
	LightMgr.SaveAmbientLight = LightMgr.AmbientLight;
}

/* for cut scene support, restore saved ambient light */

void LightMgr__RestoreAmbientLight()
{
	LightMgr__SetAmbient(&LightMgr.SaveAmbientLight);
}

/* for cut scene support, remove all registered directional lights and save them off */

void LightMgr__SaveDirectionalLights()
{
	ts_LightRecord *pNextLightRecord = (ts_LightRecord *) LightMgr.ActiveList.pHead;
	while (pNextLightRecord)
	{
		ts_LightRecord *pLightRecord = pNextLightRecord;
		pNextLightRecord = pLightRecord->pNext;
		if (pLightRecord->Light.u32Flags & LIGHT_DIRECTIONAL)
		{
			/* remove hardware light */
	
			RenderState__RemoveHardwareLight(&pLightRecord->Light);

			/* move to saved list */

			SLinkList__RemoveLink(&LightMgr.ActiveList, pLightRecord);
			SLinkList__AddLinkToHead(&LightMgr.SaveDirectionalList, pLightRecord);
		}
	}

	/* for instance-in-shadow detection, keep track of the opposite of the direction of the strongest infinite */
	/* directional light */

	LightMgr__SetSun();

	/* any active effect lights? */

	LightMgr__CheckForActiveEffectLights();
}

/* for cut scene support, restore saved directional lights */

void LightMgr__RestoreDirectionalLights()
{
	/* remove any directional lights that may have been added */

	ts_LightRecord *pNextLightRecord = (ts_LightRecord *) LightMgr.ActiveList.pHead;
	while (pNextLightRecord)
	{
		ts_LightRecord *pLightRecord = pNextLightRecord;
		pNextLightRecord = pLightRecord->pNext;
		if (pLightRecord->Light.u32Flags & LIGHT_DIRECTIONAL)
			LightMgr__RemoveLight(pLightRecord->Handle);
	}

	/* move saved directional lights back to active list */

	while (LightMgr.SaveDirectionalList.pHead)
	{
		ts_LightRecord *pLightRecord = (ts_LightRecord *) LightMgr.SaveDirectionalList.pHead;
		{
			/* move to active list */

			SLinkList__RemoveLink(&LightMgr.SaveDirectionalList, pLightRecord);
			SLinkList__AddLinkToHead(&LightMgr.ActiveList, pLightRecord);

			/* register for rendering */

			RenderState__RegisterHardwareLight(&pLightRecord->Light);
		}
	}

	/* recheck for sun */

	LightMgr__SetSun();
}

/* grab a record from free pool, assign handle, return handle */

ts_LightRecord *LightMgr__AllocRecord(void)
{
	ts_LightRecord *pLightRecord;

	/* grab a record from free pool */

	pLightRecord = (ts_LightRecord *) SLinkList__GetAndRemoveHead(&LightMgr.FreeList);
	ASSERT(pLightRecord);	// shouldn't run out of records. if so, increase handle count passed to LightMgr__Init
	if (pLightRecord == NULL)
		return(NULL);

	/* build handle */

	ASSERT(pLightRecord->Handle == INVALID_HANDLE);				// record should not have a handle assigned
	HANDLE_INDEX(pLightRecord->Handle) = pLightRecord - LightMgr.pLightAlloc;	// index into allocated array of records
	HANDLE_ID(pLightRecord->Handle) = LightMgr.u16ID++;
	if (pLightRecord->Handle == INVALID_HANDLE)
		HANDLE_ID(pLightRecord->Handle) = LightMgr.u16ID++;	// do it again to avoid INVALID_HANDLE
	ASSERT(pLightRecord->Handle != INVALID_HANDLE);				// record should have a valid handle

	/* success */

	return(pLightRecord);
}

/* find the brightest infinite directional light and consider it the sun */

void LightMgr__SetSun(void)
{
	LightMgr.pSun = NULL;
	float fMaxLightSquared = 0.0f;
	ts_LightRecord	*pLightRecord = (ts_LightRecord *) LightMgr.ActiveList.pHead;
	while (pLightRecord)
	{
		if ((pLightRecord->Light.u32Flags & (LIGHT_ON | LIGHT_DIRECTIONAL)) == (LIGHT_ON | LIGHT_DIRECTIONAL))
		{
			float fLightSquared = (*(Vector3Packed *) &pLightRecord->Light.Color).LengthSquared();	// color is ts_fRGB
			if (fLightSquared > fMaxLightSquared)
			{
				fMaxLightSquared = fLightSquared;
				LightMgr.pSun = &pLightRecord->Light;
			}
		}
		pLightRecord = pLightRecord->pNext;
	}

	if( LightMgr.pSun )
		sunLight = *( LightMgr.pSun );
}

/* return pointer to sun (brightest infinite directional light) */

ts_Light *LightMgr__GetSun(void)
{
	return( &sunLight );
//	return(LightMgr.pSun);
}

void Color__Clamp(ts_fRGB *pColor)
{
	if (pColor->R > 1.0f)
		pColor->R = 1.0f;
	if (pColor->G > 1.0f)
		pColor->G = 1.0f;
	if (pColor->B > 1.0f)
		pColor->B = 1.0f;
}

/* lights can be "effect" lights-- that is, not used for actual lighting, but used to add an effect to the frame, like a
texture-based spotlight */

void LightMgr__RenderSpecialEffects(CScene *pScene)
{
	/* maybe nothing to do */

	if (LightMgr.bActiveEffectLights == false)
		return;

	/* find 'em and render 'em */

	ts_LightRecord	*pLightRecord = (ts_LightRecord *) LightMgr.ActiveList.pHead;
	while (pLightRecord)
	{
		if (pLightRecord->Light.u32Flags & LIGHT_ON)
		{
			switch(pLightRecord->Light.u32Flags & LIGHT_EFFECT_TYPE)
			{
			case 0:
				break;	// not an effect light
			case LIGHT_TEXTURE_SPOTLIGHT:
				LightMgr__RenderTextureSpotlight(pLightRecord, pScene);
				break;
			default:
				ASSERT(false);	// unknown type
				break;
			}
		}

		/* to next light */

		pLightRecord = pLightRecord->pNext;
	}
}

/* render a texture spotlight effect light. this is very similar to the projected texture code, but the texture is not
generated in realtime */

void LightMgr__RenderTextureSpotlight(ts_LightRecord *pLightRecord, CScene *pScene)
{
	/* set up for projected texture-- convert vertex positions into texture coordinates */

#ifndef TEST_PROJECTED
	RenderMgr__ProjectedTextureSetup();
#else //TEST_PROJECTED
if (RenderMgr__FogEnabled())
	RenderMgr__ProjectedTextureSetup();
else
	RenderMgr__ProjectedTextureSetup(false);
#endif //TEST_PROJECTED

	/* shadows are rendered as a diffuse texture-- multiply frame buffer by texture */
	RenderMgr__LockDiffuseRender( RS_BLENDFUNC_ADD );

	/* set and lock shadow texture to render with. use replace to get pixel directly from texture */
	ASSERT( pLightRecord->pTexture );
	pLightRecord->pTexture->Lock( );
	pLightRecord->pTexture->Select( );

	TextureState__RequestAll(0, TS_TYPE_2D, 0, TS_METHOD_REPLACE, TS_WRAP_CLAMP, TS_WRAP_CLAMP, TS_FILTER_BILINEAR);
	TextureState__Lock(0, TS_TYPE);
	TextureState__Lock(0, TS_METHOD);
	TextureState__Lock(0, TS_WRAPU);
	TextureState__Lock(0, TS_WRAPV);
	TextureState__Lock(0, TS_FILTER);

	/* project world-space coordinates onto the viewing plane defined by the light. texture matrix has already had */
	/* some set-up done by RenderMgr__ProjectedTextureSetup */

	TextureState__RequestUnit(0);	// shadow texture is on unit 0
	MatrixStack__Mode(MS_TEXTURE);
	MatrixStack__Push();
	MatrixStack__Multiply(&pLightRecord->pProjection->mMatrix);
	MatrixStack__Multiply(&pLightRecord->pWorldToLight->mMatrix);

	/*** we don't check instance against the frustum, so no need to transform it to world coords.  ***/
	/*** if needed, see Frustum__TransformToWorld call in ShadowMgr__RenderShadows for example. if ***/
	/*** we have a texture spotlight that only brightens, then we should resurrect frustum culling ***/
	/*** (since we won't have to worry about darkening all instances outside of the light frustum) ***/

#if 0
	/* to visualize frustum */

	static ts_bRGBA Blue = {0, 0, 255, 255};
	static ts_bRGBA TransparentBlue = {0, 0, 255, 20};
	for (int j = 0; j < 8; j++)
		WorkVector[j] = pLightRecord->pLightFrustum->Frustum.Corners[j] * pLightRecord->pLightToWorld->mMatrix;

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
#endif //0

	/* re-render all instance in scene */

	SceneMgr::RerenderScene(pScene, NULL, NULL);

	/* pop texture matrix */

	TextureState__RequestUnit(0);	// shadow texture is on unit 0
	MatrixStack__Mode(MS_TEXTURE);
	MatrixStack__Pop();

	/* unlock texture states */
	pLightRecord->pTexture->Unselect( );
	pLightRecord->pTexture->Unlock( );

	TextureState__UnlockAll();

	/* restore */

	RenderMgr__ProjectedTextureRestore();
	RenderMgr__UnlockDiffuseRender();
}

/* check whether there are any active "effect" lights in the list */

void LightMgr__CheckForActiveEffectLights()
{
	LightMgr.bActiveEffectLights = false;
	ts_LightRecord	*pLightRecord = (ts_LightRecord *) LightMgr.ActiveList.pHead;
	while (pLightRecord)
	{
		if ((pLightRecord->Light.u32Flags & LIGHT_EFFECT_TYPE) && (pLightRecord->Light.u32Flags & LIGHT_ON))
		{
			LightMgr.bActiveEffectLights = true;
			return;	// found one, no need to check any longer
		}

		/* to next light */

		pLightRecord = pLightRecord->pNext;
	}
}
