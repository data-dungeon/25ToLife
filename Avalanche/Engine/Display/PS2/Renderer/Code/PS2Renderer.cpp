//
// PS2Renderer.cpp
// Copyright (C) 2001 by Avalanche Software, All Rights Reserved
//
// Author: Tyler Colbert
// Creation Date: Nov 7, 2001
// 

// TJC - temp removed
//#ifndef PS2
//#error MasterList module is PS2 only.. please remove it from your project
//#endif

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// headers
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

#include "PS2Renderer.h"
#include "MasterList.h"

#include "renderassert.h"
#include <malloc.h>
#include <eeregs.h>
#include <math.h>

#include "DMAObject.h"
#include "DDMAObjectClassIDs.h"
#include "DDMAObject.h"
#include "VUCodeManager.h"
#include "VUCodeSetupHelper.h"
#include "VUDataClass.h"
#include "VUDataHeaders.h"
#include "GIFTag.h"

#include "DDMAObject.h"
#include "StaticDDMAObject.h"
#include "PatchDDMAObject.h"
#include "AnimDDMAObject.h"

typedef u_long128 u128;

extern void* pRenderInst;

// TJC -  MAJOR hack... wish I had another simple way to do this.
namespace MasterList
{
	extern void DeleteAfter2Frames(DDMAObject* obj);
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// defines
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

#define FORCE_LOCAL_TO_CAMERA_INPUT_SPACE
#define FORCE_LOCAL_TO_CAMERA_INPUT_LIGHTS

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// types
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

static inline void my_sceVu0CopyVector(sceVu0FVECTOR v0, const sceVu0FVECTOR v1)
{
	asm __volatile__("
		lq    $6,0x0(%1)
		sq    $6,0x0(%0)
		": : "r" (v0) , "r" (v1):"$6");
}

static inline void my_sceVu0TransposeMatrix(PS2Matrix4x4 m0, const PS2Matrix4x4 m1)
{
	asm __volatile__("
		lq $8,0x0000(%1)
		lq $9,0x0010(%1)
		lq $10,0x0020(%1)
		lq $11,0x0030(%1)

		pextlw     $12,$9,$8
		pextuw     $13,$9,$8
		pextlw     $14,$11,$10
		pextuw     $15,$11,$10

		pcpyld     $8,$14,$12
		pcpyud     $9,$12,$14
		pcpyld     $10,$15,$13
		pcpyud     $11,$13,$15

		sq $8,0x0000(%0)
		sq $9,0x0010(%0)
		sq $10,0x0020(%0)
		sq $11,0x0030(%0)
		": : "r" (m0) , "r" (m1));
}

// class to represent a vu code.
struct VUHandler
{
	VUHandler* mNext;

	VUDataClass* mHandler;

	VUHandler(VUDataClass* handler)
		: mNext(NULL), mHandler(handler)
	{
	}
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// static variables
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

static VURenderInfo RenderInfo;

// these are backups for when lighting is off... 
static bool sLightsAreOn = TRUE;
static PS2Lights sBackupLights;

////////////////////////////////////////////
// others
////////////////////////////////////////////

// vu code manager (I wouldnt make these pointers, except that the need init data)
static VUCodeManager* gCodeManager = NULL;

// helper
static VUCodeSetupHelper* gHelper = NULL;
// list of handlers
static VUHandler* gVUHandlers = NULL;

// flags for which matricies need rebuilding (all need it at first...)
// if you mark a matrix dirty, you MUST also mark all matrices that
// depend on it as dirty.
static unsigned int gDirtyMatrices = 0xFFFFFFFF;

#ifndef SHIP_IT
#define ENABLE_DEBUGGING
#endif

// debugging structure... 
#ifdef ENABLE_DEBUGGING
struct {
	unsigned int drawOnlyClassID;
	unsigned int drawSkipClassID;
	unsigned int drawFlagFilterAND;
	unsigned int drawFlagFilterNAND;
	unsigned int drawStartObject;
	unsigned int drawEndObject;
	unsigned int drawOnlyObject;
	unsigned int forceFlagAnd;
	unsigned int forceFlagOr;
	unsigned int dirtyMatrixAnd;
	unsigned int dirtyMatrixOr;
	unsigned int drawObjectCount;
	unsigned int mostRecentDrawnObject;
} PS2Renderer_Dbg = {
	0xFFFFFFFF,	// drawOnlyClassID
	0xFFFFFFFF,	// drawSkipClassID
	0,				// drawFlagFilterAND
	0,				// drawFlagFilterNAND
	0xFFFFFFFF, // drawStartObject
	0xFFFFFFFF, // drawEndObject
	0,				// drawOnlyObject
	0xFFFFFFFF, // dirtyMatrixAnd
	0,				// dirtyMatrixOr
	0xFFFFFFFF, // forceFlagAnd
	0,				// forceFlagOr
	0,				// drawObjectCount
	0,				// mostRecentDrawnObject
};
#endif // ENABLE_DEBUGGING

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// begining of public namespace
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

namespace PS2Renderer {

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// private internal stuff (not static because other internal modules access them)
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////

	// these are used by the AddVUHandlers() function outside here...
	void AddHandlers(void);
	
	void AddHandler(VUDataClass* handler);

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// private function declarations
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////

	VUHandler* FindVUHandler(u_int classID);
	void InverseScaleTranslate(PS2Matrix4x4& dest, const PS2Matrix4x4& src);
	inline void BuildIdentity4x4(PS2Matrix4x4& dest);
	inline void UpdateMatrices(register unsigned int whichMatrices, const PS2Matrix4x4 ModelMatrix = RenderInfo.mIdentity, u_int flags = 0);
	void HandleUpdateMatrices(register unsigned int whichMatrices, const PS2Matrix4x4 ModelMatrix, u_int flags) NAMED_SECTION("text.rend1");
	void ToggleLightsOnOffFlag();
	void CopyMatrix(PS2Matrix4x4& dest, PS2Matrix4x4& src);
	void HandleLighting(u_int flags, const PS2Matrix4x4& ModelMatrix, int tempMatrixType, PS2Matrix4x4& tempMatrix);
	
	
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// private static inline functions
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	
	inline void BuildIdentity4x4(PS2Matrix4x4& dest)
	{
#ifdef A_COMPILER_SN
		u_long128 m1;
		u_long128 m2;
		u_long128 m3;
		u_long128 m4;
		asm ("vmr32.xyzw %0, vf0" : "=j" (m3));
		asm ("vmr32.xyzw %0, %1"  : "=j" (m2) : "j" (m3));
		asm ("vmr32.xyzw %0, %1"  : "=j" (m1) : "j" (m2));
		asm ("vmove.xyzw %0, vf0" : "=j" (m4));
		asm ("sqc2 %2, 0x00(%1)"  : "=m" (dest) : "r" (&dest), "j" (m1));
		asm ("sqc2 %2, 0x10(%1)"  : "=m" (dest) : "r" (&dest), "j" (m2));
		asm ("sqc2 %2, 0x20(%1)"  : "=m" (dest) : "r" (&dest), "j" (m3));
		asm ("sqc2 %2, 0x30(%1)"  : "=m" (dest) : "r" (&dest), "j" (m4));
#else
		dest[0][0] = dest[1][1] = dest[2][2] = dest[3][3] = 1.0f;
		dest[0][1] = dest[0][2] = dest[0][3] = 0.0f;
		dest[1][0] = dest[0][2] = dest[0][3] = 0.0f;
		dest[2][0] = dest[0][1] = dest[0][3] = 0.0f;
		dest[3][0] = dest[0][1] = dest[0][2] = 0.0f;
#endif
	}

	void InverseScaleTranslate(PS2Matrix4x4& dest, const PS2Matrix4x4& src)
	{
		BuildIdentity4x4(dest);

		// TJC - this function is suspect, I just kind of hacked it out...
		for(int i = 0;i < 4;i++)
		{
			dest[i][i] = 1.0f/src[i][i];
		}

		for(int j = 0; j < 3; j++)
			dest[3][j] = -src[3][j] * dest[j][j];
	}

	inline void CopyMatrix(PS2Matrix4x4& dest, PS2Matrix4x4& src)
	{
#ifdef A_COMPILER_SN
		u_long128 temp;
		asm("lqc2 %0, 0x00(%1)" : "=j" (temp) : "r" (&src), "m" (src));
		asm("sqc2 %2, 0x00(%1)" : "=m" (dest) : "r" (&dest), "j" (temp));
		asm("lqc2 %0, 0x10(%1)" : "=j" (temp) : "r" (&src), "m" (src));
		asm("sqc2 %2, 0x10(%1)" : "=m" (dest) : "r" (&dest), "j" (temp));
		asm("lqc2 %0, 0x20(%1)" : "=j" (temp) : "r" (&src), "m" (src));
		asm("sqc2 %2, 0x20(%1)" : "=m" (dest) : "r" (&dest), "j" (temp));
		asm("lqc2 %0, 0x30(%1)" : "=j" (temp) : "r" (&src), "m" (src));
		asm("sqc2 %2, 0x30(%1)" : "=m" (dest) : "r" (&dest), "j" (temp));
#else
		memcpy(&dest, &src, sizeof(PS2Matrix4x4));
#endif
	}

	inline void UpdateMatrices(register unsigned int whichMatrices, const PS2Matrix4x4 ModelMatrix, u_int flags)
	{
		// there are a lot of interdependancies, so all this needs to be 
		// done carefully and in the correct order...

		// eliminate those matrices which are not dirty...
		whichMatrices &= gDirtyMatrices;

		// quick out...
		if(whichMatrices == 0x00000000)
			return;

		HandleUpdateMatrices(whichMatrices, ModelMatrix, flags);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// Public interface
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	
	// functions for setting/getting light info
	void SetLight(int number, const PS2Vector4& dirOrPos, const PS2Vector4& ambientcolor, const PS2Vector4& diffusecolor, const PS2Vector4& specularcolor, u_int flags, u_int type, float attenuation )
	{
		DBGASSERT(number >= 0 && number < MAX_LIGHTS);
		
		if(sLightsAreOn && RenderInfo.mLights.mLight[number].mEnabled)
		{
			memcpy(&RenderInfo.mLights.mLight[number].mAmbientColor, &ambientcolor, sizeof(PS2Vector4));
			memcpy(&RenderInfo.mLights.mLight[number].mDiffuseColor, &diffusecolor, sizeof(PS2Vector4));
			memcpy(&RenderInfo.mLights.mLight[number].mSpecularColor, &specularcolor, sizeof(PS2Vector4));
			memcpy(&RenderInfo.mLights.mLight[number].mOrigVec.mOriginalDirection, &dirOrPos, sizeof(PS2Vector4));
			RenderInfo.mLights.mLight[number].mFlags = flags;
			RenderInfo.mLights.mLight[number].mCurrentSpace = LIGHT_SPACE_NONE;
			RenderInfo.mLights.mLight[number].mType = type;
			RenderInfo.mLights.mLight[number].mQuadraticAttenuation = attenuation;
		}
		else
		{
			memcpy(&sBackupLights.mLight[number].mAmbientColor, &ambientcolor, sizeof(PS2Vector4));
			memcpy(&sBackupLights.mLight[number].mDiffuseColor, &diffusecolor, sizeof(PS2Vector4));
			memcpy(&sBackupLights.mLight[number].mSpecularColor, &specularcolor, sizeof(PS2Vector4));
			memcpy(&sBackupLights.mLight[number].mOrigVec.mOriginalDirection, &dirOrPos, sizeof(PS2Vector4));
			sBackupLights.mLight[number].mFlags = flags;
			sBackupLights.mLight[number].mCurrentSpace = LIGHT_SPACE_NONE;
			sBackupLights.mLight[number].mType = type;
			sBackupLights.mLight[number].mQuadraticAttenuation = attenuation;
		}
	}
	void EnableLight(int number, bool Enabled, u_int type)
	{
		if(sLightsAreOn)
		{
			if(Enabled == RenderInfo.mLights.mLight[number].mEnabled)
				return;

			if(Enabled)
				memcpy(&RenderInfo.mLights.mLight[number], &sBackupLights.mLight[number], sizeof(PS2LightData));
			else
			{
				memcpy(&sBackupLights.mLight[number], &RenderInfo.mLights.mLight[number], sizeof(PS2LightData));
				RenderInfo.mLights.mLight[number].mAmbientColor[0] =
					RenderInfo.mLights.mLight[number].mAmbientColor[1] =
					RenderInfo.mLights.mLight[number].mAmbientColor[2] =
					RenderInfo.mLights.mLight[number].mAmbientColor[3] = 0.0f;
				RenderInfo.mLights.mLight[number].mDiffuseColor[0] =
					RenderInfo.mLights.mLight[number].mDiffuseColor[1] =
					RenderInfo.mLights.mLight[number].mDiffuseColor[2] =
					RenderInfo.mLights.mLight[number].mDiffuseColor[3] = 0.0f;
				RenderInfo.mLights.mLight[number].mSpecularColor[0] =
					RenderInfo.mLights.mLight[number].mSpecularColor[1] =
					RenderInfo.mLights.mLight[number].mSpecularColor[2] =
					RenderInfo.mLights.mLight[number].mSpecularColor[3] = 0.0f;
			}
		}
		RenderInfo.mLights.mLight[number].mEnabled = Enabled;
	}
	void SetAmbientLight(const PS2Vector4& color)
	{
		if(sLightsAreOn)
		{
			memcpy(&RenderInfo.mLights.mAmbientLight.mColor, &color, sizeof(PS2Vector4));
		}
		else
		{
			memcpy(&sBackupLights.mAmbientLight.mColor, &color, sizeof(PS2Vector4));
		}
		RenderInfo.mLights.mAmbientLight.mEnabled = TRUE;
	}

	// functions for setting/getting matrices

	void SetViewToScreen(const PS2Matrix4x4& mtx)
	{
		gDirtyMatrices |= 0
			| MATRIX_CAMERA_TO_SCREEN
			| MATRIX_WORLD_TO_SCREEN
			| MATRIX_CLIP_TO_SCREEN
			| MATRIX_SCREEN_TO_CLIP
			| MATRIX_CULL_TO_SCREEN
			| MATRIX_SCREEN_TO_CULL;
		gDirtyMatrices &= (u_int)~MATRIX_VIEW_TO_SCREEN;

		CopyMatrix(RenderInfo.mViewToScreen, *(PS2Matrix4x4*)&mtx);
	}

	void SetWorldToCamera(const PS2Matrix4x4& mtx)
	{
		gDirtyMatrices |= 0
			| MATRIX_WORLD_TO_SCREEN
			| MATRIX_WORLD_TO_CLIP
			| MATRIX_WORLD_TO_CULL
			| MATRIX_CAMERA_TO_WORLD
			| MATRIX_WORLD_TO_VIEW
			| MATRIX_VIEW_TO_WORLD;
		gDirtyMatrices &= (u_int)~MATRIX_WORLD_TO_CAMERA;
		
		CopyMatrix(RenderInfo.mWorldToCamera, *(PS2Matrix4x4*)&mtx);
	}

	void SetCameraToView(const PS2Matrix4x4& mtx)
	{
		gDirtyMatrices |= 0
			| MATRIX_CAMERA_TO_CLIP
			| MATRIX_CAMERA_TO_CULL
			| MATRIX_CAMERA_TO_SCREEN
			| MATRIX_WORLD_TO_SCREEN
			| MATRIX_WORLD_TO_CLIP
			| MATRIX_WORLD_TO_CULL
			| MATRIX_WORLD_TO_VIEW
			| MATRIX_VIEW_TO_CAMERA
			| MATRIX_VIEW_TO_WORLD;
		gDirtyMatrices &= (u_int)~MATRIX_CAMERA_TO_VIEW;

		CopyMatrix(RenderInfo.mCameraToView, *(PS2Matrix4x4*)&mtx);
	}

	void SetViewToClip(const PS2Matrix4x4& mtx)
	{
		gDirtyMatrices |= 0
			| MATRIX_CAMERA_TO_CLIP
			| MATRIX_CLIP_TO_VIEW
			| MATRIX_CLIP_TO_SCREEN
			| MATRIX_SCREEN_TO_CLIP
			| MATRIX_WORLD_TO_CLIP;
		gDirtyMatrices &= (u_int)~MATRIX_VIEW_TO_CLIP;

		CopyMatrix(RenderInfo.mViewToClip, *(PS2Matrix4x4*)&mtx);
	}

	void SetViewToCull(const PS2Matrix4x4& mtx)
	{
		gDirtyMatrices |= 0
			| MATRIX_CAMERA_TO_CULL
			| MATRIX_CULL_TO_VIEW
			| MATRIX_CULL_TO_SCREEN
			| MATRIX_SCREEN_TO_CULL
			| MATRIX_WORLD_TO_CULL;
		gDirtyMatrices &= (u_int)~MATRIX_VIEW_TO_CULL;

		CopyMatrix(RenderInfo.mViewToCull, *(PS2Matrix4x4*)&mtx);
	}

	void EnableEnvMapping( unsigned int uiBind, bool bSphericalNormals, EnvMapSpace evSpace, const PS2Matrix4x4 * pPostTransform )
	{
		ASSERT( (evSpace == ENVMAP_SPACE_CAMERA)  ||  (evSpace == ENVMAP_SPACE_VIEW) );

		RenderInfo.mEnvData.mEVMapTextureBind = uiBind;
		RenderInfo.mEnvData.mSpace = evSpace;
		RenderInfo.mEnvData.mPostTransform = pPostTransform;

		if( bSphericalNormals )
			RenderInfo.mEnvData.mType = ENVMAP_TYPE_STD_SPHERE_MAP;
		else
			RenderInfo.mEnvData.mType = ENVMAP_TYPE_STD_ENV_MAP;
	}

	void EnableSphereMapping( unsigned int uiBind, bool bSphericalNormals, const PS2Vector4 & eyeWorld, const PS2Matrix4x4 * pWorldToMap )
	{
		RenderInfo.mEnvData.mEVMapTextureBind = uiBind;
		RenderInfo.mEnvData.mPostTransform = 0;

		if( pWorldToMap )
		{
         RenderInfo.mEnvData.mWorldToMap = pWorldToMap;
			RenderInfo.mEnvData.mSpace = ENVMAP_SPACE_MAP;
		}
		else
		{
			RenderInfo.mEnvData.mWorldToMap = 0;
			RenderInfo.mEnvData.mSpace = ENVMAP_SPACE_WORLD;
		}

		memcpy( RenderInfo.mEnvData.mEyePosition, eyeWorld, sizeof( PS2Vector4) );

		if( bSphericalNormals )
			RenderInfo.mEnvData.mType = ENVMAP_TYPE_REF_SPHERE_MAP;
		else
			RenderInfo.mEnvData.mType = ENVMAP_TYPE_REF_ENV_MAP;
		
	}

	void DisableEnvMapping( void )
	{
		RenderInfo.mEnvData.mEVMapTextureBind = 0;
	}


	void SetShadowRGB( u_char R, u_char G, u_char B)
	{
		RenderInfo.ShadowR = R;
		RenderInfo.ShadowG = G;
		RenderInfo.ShadowB = B;
	}
		
	void SetTexture(const PS2Matrix4x4& mtx)
	{
		CopyMatrix(RenderInfo.mTexture, *(PS2Matrix4x4*)&mtx);
	}

	const PS2Matrix4x4& GetViewToScreen(void) 
	{
		return RenderInfo.mViewToScreen;
	}

	const PS2Matrix4x4& GetWorldToCamera(void) 
	{
		return RenderInfo.mWorldToCamera;
	}

	const PS2Matrix4x4& GetCameraToView(void) 
	{
		return RenderInfo.mCameraToView;
	}

	const PS2Matrix4x4& GetViewToClip(void) 
	{
		return RenderInfo.mViewToClip;
	}

	const PS2Matrix4x4& GetViewToCull(void) 
	{
		return RenderInfo.mViewToCull;
	}

	const PS2Matrix4x4& GetTexture(void) 
	{
		return RenderInfo.mTexture;
	}

	// get generated matrices
	const PS2Matrix4x4& GetWorldToClip(void)
	{
		UpdateMatrices(MATRIX_WORLD_TO_CLIP);

		return RenderInfo.mWorldToClip;
	}

	const PS2Matrix4x4& GetCameraToClip(void)
	{
		UpdateMatrices(MATRIX_CAMERA_TO_CLIP);

		return RenderInfo.mCameraToClip;
	}
	
	const PS2Matrix4x4& GetScreenToClip(void)
	{
		UpdateMatrices(MATRIX_SCREEN_TO_CLIP);

		return RenderInfo.mScreenToClip;
	}

	const PS2Matrix4x4& GetWorldToCull(void)
	{
		UpdateMatrices(MATRIX_WORLD_TO_CULL);

		return RenderInfo.mWorldToCull;
	}

	const PS2Matrix4x4& GetCameraToCull(void)
	{
		UpdateMatrices(MATRIX_CAMERA_TO_CULL);

		return RenderInfo.mCameraToCull;
	}
	
	const PS2Matrix4x4& GetScreenToCull(void)
	{
		UpdateMatrices(MATRIX_SCREEN_TO_CULL);

		return RenderInfo.mScreenToCull;
	}

	const PS2Matrix4x4& GetWorldToScreen(void)
	{
		UpdateMatrices(MATRIX_WORLD_TO_SCREEN);

		return RenderInfo.mWorldToScreen;
	}

	const PS2Matrix4x4& GetCameraToScreen(void)
	{
		UpdateMatrices(MATRIX_CAMERA_TO_SCREEN);

		return RenderInfo.mCameraToScreen;
	}

	const PS2Matrix4x4& GetClipToScreen(void)
	{
		UpdateMatrices(MATRIX_CLIP_TO_SCREEN);

		return RenderInfo.mClipToScreen;
	}

	const PS2Matrix4x4& GetClipToView(void)
	{
		UpdateMatrices(MATRIX_CLIP_TO_VIEW);

		return RenderInfo.mClipToView;
	}

	const PS2Matrix4x4& GetCullToScreen(void)
	{
		UpdateMatrices(MATRIX_CULL_TO_SCREEN);

		return RenderInfo.mCullToScreen;
	}

	const PS2Matrix4x4& GetCullToView(void)
	{
		UpdateMatrices(MATRIX_CULL_TO_VIEW);

		return RenderInfo.mCullToView;
	}

	float GetConstantAlphaMultiplier( void )
	{
		return RenderInfo.alphaMultiplier;
	}

	void SetConstantAlphaMultiplier( float alphaMultiplier )
	{
		RenderInfo.alphaMultiplier = alphaMultiplier;
	}

	void GetFog( float &fogNear, float &fogFar)
	{
		fogNear = RenderInfo.fogNear;
		fogFar = RenderInfo.fogFar;
	}

	void SetFog( float fogNear, float fogFar)
	{
		RenderInfo.fogNear = fogNear;
		RenderInfo.fogFar = fogFar;
	}

	// helpers

	// full screen support guardband
	void SetViewToClip(u_int /*frameBufferWidth*/, u_int /*frameBufferHeight*/)
	{
		gDirtyMatrices |= 0
			| MATRIX_CAMERA_TO_CLIP
			| MATRIX_CLIP_TO_VIEW
			| MATRIX_CLIP_TO_SCREEN
			| MATRIX_SCREEN_TO_CLIP
			| MATRIX_WORLD_TO_CLIP;
		gDirtyMatrices &= (u_int)~MATRIX_VIEW_TO_CLIP;

		BuildIdentity4x4(RenderInfo.mViewToClip);
//		RenderInfo.mViewToClip[0][0] = frameBufferWidth * (0.5f / 2047.0f);
//		RenderInfo.mViewToClip[1][1] = frameBufferHeight * (0.5f / 2047.0f);
	}

	// full screen support guardband
	void SetViewToCull(u_int frameBufferWidth, u_int frameBufferHeight)
	{
		gDirtyMatrices |= 0
			| MATRIX_CAMERA_TO_CULL
			| MATRIX_CULL_TO_VIEW
			| MATRIX_CULL_TO_SCREEN
			| MATRIX_SCREEN_TO_CULL
			| MATRIX_WORLD_TO_CULL;
		gDirtyMatrices &= (u_int)~MATRIX_VIEW_TO_CULL;

		BuildIdentity4x4(RenderInfo.mViewToCull);
		RenderInfo.mViewToCull[0][0] = frameBufferWidth * (0.5f / 2047.0f);
		RenderInfo.mViewToCull[1][1] = frameBufferHeight * (0.5f / 2047.0f);
	}

	void HandleLighting(u_int flags, const PS2Matrix4x4& ModelMatrix, int tempMatrixType, PS2Matrix4x4& tempMatrix)
	{
		// deal with lighting on/off flag...
		bool UseLights = flags & (MICROCODE_ENABLE_LIGHTING) ? true : false;

		// the compiler generates a call to memset when we use = { NULL };
		PS2Matrix4x4* lightMtx[MAX_LIGHTS];
		// I want to avoid it so here I loop and set them myself...
		for(int i = 0; i < MAX_LIGHTS; i++)
			lightMtx[i] = NULL;

			
		if(sLightsAreOn != UseLights)
		{
			ToggleLightsOnOffFlag();
		}
			
			// set default lights into RenderInfo
		if((flags & CARTOON_LIGHTS) == (u_int) CARTOON_LIGHTS)
		{
			for(int i = 0; i < MAX_LIGHTS; i++)
			{
				RenderInfo.mLights.mLight[i].mDiffuseColor[0]	=
				RenderInfo.mLights.mLight[i].mDiffuseColor[1]	=
				RenderInfo.mLights.mLight[i].mDiffuseColor[2]	=
				RenderInfo.mLights.mLight[i].mSpecularColor[0]	=
				RenderInfo.mLights.mLight[i].mSpecularColor[1]	=
				RenderInfo.mLights.mLight[i].mSpecularColor[2]	=
				RenderInfo.mLights.mLight[i].mSpecularColor[3]	=
				RenderInfo.mLights.mLight[i].mAmbientColor[0]	=
				RenderInfo.mLights.mLight[i].mAmbientColor[1]	=
				RenderInfo.mLights.mLight[i].mAmbientColor[2]	=
				RenderInfo.mLights.mLight[i].mAmbientColor[3]	= 0.0f;
				RenderInfo.mLights.mLight[i].mDiffuseColor[3]	= 64.0f;
			}
			RenderInfo.mLights.mAmbientLight.mColor[0] =
			RenderInfo.mLights.mAmbientLight.mColor[1] =
			RenderInfo.mLights.mAmbientLight.mColor[2] =
			RenderInfo.mLights.mAmbientLight.mColor[3] = 0.0f;
		}

		if(sLightsAreOn)
		{
			u_int wantedSpace = RenderInfo.lightSpaceRequested;
			// determine which matricies we will need to build the lights into the correct space..
			for(int light = 0; light < (MAX_LIGHTS); light++)
			{
				u_int currentSpace = RenderInfo.mLights.mLight[light].mCurrentSpace;
				
				if(currentSpace == wantedSpace && currentSpace != LIGHT_SPACE_MODEL)
					continue;
					
				if( !RenderInfo.mLights.mLight[light].mEnabled)
					continue;
	
				currentSpace = RenderInfo.mLights.mLight[light].mFlags & LIGHT_SPACE_MASK;

					// need to transform original space to new space...
				switch( currentSpace)
				{
				case LIGHT_SPACE_WORLD:
					switch( wantedSpace)
					{
					case LIGHT_SPACE_MODEL:
						RenderInfo.validMatricies |= MATRIX_WORLD_TO_MODEL;
						lightMtx[light] = &RenderInfo.mWorldToModel;
						break;
					case LIGHT_SPACE_WORLD:
						RenderInfo.validMatricies |= MATRIX_IDENTITY;
						lightMtx[light] = &RenderInfo.mIdentity;
						break;
					case LIGHT_SPACE_CAMERA:
						RenderInfo.validMatricies |= MATRIX_WORLD_TO_CAMERA;
						lightMtx[light] = &RenderInfo.mWorldToCamera;
						break;
					case LIGHT_SPACE_VIEW:
						RenderInfo.validMatricies |= MATRIX_WORLD_TO_VIEW;
						lightMtx[light] = &RenderInfo.mWorldToView;
						break;
					}
					break;
				case LIGHT_SPACE_CAMERA:
					switch( wantedSpace)
					{
					case LIGHT_SPACE_MODEL:
						RenderInfo.validMatricies |= MATRIX_CAMERA_TO_MODEL;
						lightMtx[light] = &RenderInfo.mCameraToModel;
						break;
					case LIGHT_SPACE_WORLD:
						RenderInfo.validMatricies |= MATRIX_CAMERA_TO_WORLD;
						lightMtx[light] = &RenderInfo.mCameraToWorld;
						break;
					case LIGHT_SPACE_CAMERA:
						RenderInfo.validMatricies |= MATRIX_IDENTITY;
						lightMtx[light] = &RenderInfo.mIdentity;
						break;
					case LIGHT_SPACE_VIEW:
						RenderInfo.validMatricies |= MATRIX_CAMERA_TO_VIEW;
						lightMtx[light] = &RenderInfo.mCameraToView;
						break;
					}
					break;
				case LIGHT_SPACE_VIEW:
					switch( wantedSpace)
					{
					case LIGHT_SPACE_MODEL:
						RenderInfo.validMatricies |= MATRIX_VIEW_TO_MODEL;
						lightMtx[light] = &RenderInfo.mViewToModel;
						break;
					case LIGHT_SPACE_WORLD:
						RenderInfo.validMatricies |= MATRIX_VIEW_TO_WORLD;
						lightMtx[light] = &RenderInfo.mViewToWorld;
						break;
					case LIGHT_SPACE_CAMERA:
						RenderInfo.validMatricies |= MATRIX_VIEW_TO_CAMERA;
						lightMtx[light] = &RenderInfo.mViewToCamera;
							break;
					case LIGHT_SPACE_VIEW:
						RenderInfo.validMatricies |= MATRIX_IDENTITY;
						lightMtx[light] = &RenderInfo.mIdentity;
						break;
					}
					break;
				}
					// pretend like the transform just happened and update the light mCurrentSpace (we transform just below)
				RenderInfo.mLights.mLight[light].mCurrentSpace = wantedSpace;
			}
		}

		if(tempMatrixType == MATRIX_CAMERA_TO_WORLD)
		{
			// generate LOCAL_TO_WORLD matrix...
			sceVu0MulMatrix(tempMatrix, RenderInfo.mCameraToWorld, *(PS2Matrix4x4*)&ModelMatrix);
		}
		// make sure our matricies are up to date.
		UpdateMatrices(RenderInfo.validMatricies, ModelMatrix, flags);

		// transform lights that need it...
		for(int light = 0; light < MAX_LIGHTS; light++)
		{
			// skip ones that dont need it...
			if(lightMtx[light] == 0)
				continue;

			if(!lightMtx[light])
				continue;

			// --- transform the light into the proper space, note lights have a type now
			switch( RenderInfo.mLights.mLight[ light ].mType )
			{
				case LIGHT_TYPE_DIRECTIONAL:
					sceVu0ApplyMatrix(RenderInfo.mLights.mLight[light].mCurVec.mCurrentDirection,
										*lightMtx[light], RenderInfo.mLights.mLight[light].mOrigVec.mOriginalDirection);
					break;

				case LIGHT_TYPE_POINT:
					sceVu0ApplyMatrix(RenderInfo.mLights.mLight[light].mCurVec.mCurrentPosition,
										*lightMtx[light], RenderInfo.mLights.mLight[light].mOrigVec.mOriginalPosition);

					// --- pass attenuation coefficient along in the w component since it is unused
					RenderInfo.mLights.mLight[light].mCurVec.mCurrentPosition[3] = RenderInfo.mLights.mLight[light].mQuadraticAttenuation;
					break;
			}				
		}		

		// sum up the ambient values...
		if(flags & CARTOON_LIGHTS)
		{
				RenderInfo.mLights.mTotalAmbientLight[0] = 
				RenderInfo.mLights.mTotalAmbientLight[1] = 
				RenderInfo.mLights.mTotalAmbientLight[2] = 64.0f;
				RenderInfo.mLights.mTotalAmbientLight[3] = 96.0f;
		}
		else
		{
			if(sLightsAreOn)
			{
				if(RenderInfo.mLights.mAmbientLight.mEnabled)
				{
					RenderInfo.mLights.mTotalAmbientLight[0] = RenderInfo.mLights.mAmbientLight.mColor[0];
					RenderInfo.mLights.mTotalAmbientLight[1] = RenderInfo.mLights.mAmbientLight.mColor[1];
					RenderInfo.mLights.mTotalAmbientLight[2] = RenderInfo.mLights.mAmbientLight.mColor[2];
					RenderInfo.mLights.mTotalAmbientLight[3] = RenderInfo.mLights.mAmbientLight.mColor[3];
				}
				else
				{
					RenderInfo.mLights.mTotalAmbientLight[0] = 
					RenderInfo.mLights.mTotalAmbientLight[1] = 
					RenderInfo.mLights.mTotalAmbientLight[2] = 
					RenderInfo.mLights.mTotalAmbientLight[3] = 0.0f;
				}
				for(int light = 0; light < MAX_LIGHTS; light++)
				{
					if(RenderInfo.mLights.mLight[light].mEnabled)
					{
						RenderInfo.mLights.mTotalAmbientLight[0] += RenderInfo.mLights.mLight[light].mAmbientColor[0];
						RenderInfo.mLights.mTotalAmbientLight[1] += RenderInfo.mLights.mLight[light].mAmbientColor[1];
						RenderInfo.mLights.mTotalAmbientLight[2] += RenderInfo.mLights.mLight[light].mAmbientColor[2];
						RenderInfo.mLights.mTotalAmbientLight[3] += RenderInfo.mLights.mLight[light].mAmbientColor[3];
					}
				}
			}
			else
			{
				RenderInfo.mLights.mTotalAmbientLight[0] = 
				RenderInfo.mLights.mTotalAmbientLight[1] = 
				RenderInfo.mLights.mTotalAmbientLight[2] = 
				RenderInfo.mLights.mTotalAmbientLight[3] = 128.0f;
			}
		}
	}

//#define TEST_LIGHTING
#ifdef TEST_LIGHTING
volatile unsigned int hackFlag = 0x0122;
volatile float			 pAtten = 50.0f;
#endif
	void HackRenderFlags( DDMAObject & object, u_int & flags)
	{
#ifdef TEST_LIGHTING
		if( object.d_Flags & DDMAObject::OBJ_FLAG_VERTEX_NORMALS)
		{	
			if( hackFlag & 0x0001)
			{
				RenderInfo.mLights.mLight[0].mEnabled = true;
				RenderInfo.mLights.mLight[0].mType = LIGHT_TYPE_DIRECTIONAL;
				
				RenderInfo.mLights.mLight[0].mDiffuseColor[0] = 0.0f;
				RenderInfo.mLights.mLight[0].mDiffuseColor[1] = 128.0f;
				RenderInfo.mLights.mLight[0].mDiffuseColor[2] = 0.0f;
					
					
				RenderInfo.mLights.mLight[0].mOrigVec.mOriginalDirection[0] = 0.0f;
				RenderInfo.mLights.mLight[0].mOrigVec.mOriginalDirection[1] = -1.0f;
				RenderInfo.mLights.mLight[0].mOrigVec.mOriginalDirection[2] = 0.0f;

				flags |= MICROCODE_DIRECTIONAL_LIGHTS;
			}
			else if( hackFlag & 0x0002)
			{
				RenderInfo.mLights.mLight[0].mEnabled = true;
				RenderInfo.mLights.mLight[0].mType = LIGHT_TYPE_POINT;
				RenderInfo.mLights.mLight[0].mQuadraticAttenuation = pAtten;			
				
				RenderInfo.mLights.mLight[0].mOrigVec.mOriginalPosition[0] = 0.0f;
				RenderInfo.mLights.mLight[0].mOrigVec.mOriginalPosition[1] = 10.0f;
				RenderInfo.mLights.mLight[0].mOrigVec.mOriginalPosition[2] = 0.0f;

				RenderInfo.mLights.mLight[0].mDiffuseColor[0] = 0.0f;
				RenderInfo.mLights.mLight[0].mDiffuseColor[1] = 128.0f;
				RenderInfo.mLights.mLight[0].mDiffuseColor[2] = 128.0f;

				flags |= MICROCODE_POINT_LIGHTS;
			}
					
			
			if( hackFlag & 0x0010)
			{
				RenderInfo.mLights.mLight[1].mEnabled = true;
				RenderInfo.mLights.mLight[1].mType = LIGHT_TYPE_DIRECTIONAL;

				RenderInfo.mLights.mLight[1].mDiffuseColor[0] = 0.0f;
				RenderInfo.mLights.mLight[1].mDiffuseColor[1] = 0.0f;
				RenderInfo.mLights.mLight[1].mDiffuseColor[2] = 128.0f;
					
				RenderInfo.mLights.mLight[1].mOrigVec.mOriginalDirection[0] = 0.0f;
				RenderInfo.mLights.mLight[1].mOrigVec.mOriginalDirection[1] = 1.0f;
				RenderInfo.mLights.mLight[1].mOrigVec.mOriginalDirection[2] = 0.0f;

				flags |= MICROCODE_DIRECTIONAL_LIGHTS;
			}
			else if( hackFlag & 0x0020)
			{
				RenderInfo.mLights.mLight[1].mEnabled = true;
				RenderInfo.mLights.mLight[1].mType = LIGHT_TYPE_POINT;
				RenderInfo.mLights.mLight[1].mQuadraticAttenuation = pAtten * 2;			
				
				RenderInfo.mLights.mLight[1].mOrigVec.mOriginalPosition[0] = 0.0f;
				RenderInfo.mLights.mLight[1].mOrigVec.mOriginalPosition[1] = -10.0f;
				RenderInfo.mLights.mLight[1].mOrigVec.mOriginalPosition[2] = 0.0f;

				RenderInfo.mLights.mLight[1].mDiffuseColor[0] = 128.0f;
				RenderInfo.mLights.mLight[1].mDiffuseColor[1] = 0.0f;
				RenderInfo.mLights.mLight[1].mDiffuseColor[2] = 0.0f;

				flags |= MICROCODE_POINT_LIGHTS;
			}
			
		
			if( hackFlag & 0x0100)
			{	
				flags |= MICROCODE_CLIP;
				flags &= ~MICROCODE_CULL;
			}
			else if (hackFlag & 0x0200)
			{
				flags |= MICROCODE_CULL;
				flags &= ~MICROCODE_CLIP;
			}
		}

		if( vuAllocated == 48  &&  (hackFlag & 0x8000))
		{
			if( !(RenderInfo.drawFlags & MICROCODE_SHADOWED) )
			{
				flags |= MICROCODE_SPHERE_MAP;
				
			}
		}
#endif		
	}
			
	void ValidateRenderFlags( DDMAObject & object, u_int & flags)
	{
		unsigned int vuAllocated = (object.d_Flags >> 24) & 0xff;

		if( (object.d_Flags & DDMAObject::OBJ_FLAG_ENV_MAPPED)  &&  (RenderInfo.mEnvData.mEVMapTextureBind != 0)  &&
				(vuAllocated == 48) && !(flags & (MICROCODE_SHADOWER | MICROCODE_SHADOWED)) )
		{
			if( flags & MICROCODE_CLIP)
			{
				flags &= ~MICROCODE_CLIP;
				flags |= MICROCODE_CULL;
			}
			
			flags |= MICROCODE_ENV_MAP;
		}
		else
			flags &= ~MICROCODE_ENV_MAP;

		
		if( ! (object.d_Flags & DDMAObject::OBJ_FLAG_VERTEX_NORMALS) )
		{	
			flags &= ~(MICROCODE_ENABLE_LIGHTING);
			return;
		}
	
		if( flags & MICROCODE_ENABLE_LIGHTING)
		{
			if( !sLightsAreOn)
				ToggleLightsOnOffFlag( );

			unsigned int numPointLights, numDirectionalLights;
	
			RenderInfo.countLights( numPointLights, numDirectionalLights);
	
			if( numPointLights )
				flags |= MICROCODE_POINT_LIGHTS;

			if( numDirectionalLights )
				flags |= MICROCODE_DIRECTIONAL_LIGHTS;
		}

		if( (flags & MICROCODE_SHADOWED)  &&  (flags & MICROCODE_CLIP)  &&  (RenderInfo.mEnvData.mEVMapTextureBind != 0))
		{
			flags &= ~MICROCODE_CLIP;
			flags |= MICROCODE_CULL;
		}
	
	}
			

	// functions for drawing objects...

	void DrawDMAObject(DDMAObject& object, const PS2Matrix4x4& ModelMatrix, u_int flags)
	{
#ifdef ENABLE_DEBUGGING
		gDirtyMatrices &= PS2Renderer_Dbg.dirtyMatrixAnd;
		gDirtyMatrices |= PS2Renderer_Dbg.dirtyMatrixOr;
		flags &= PS2Renderer_Dbg.forceFlagAnd;
		flags |= PS2Renderer_Dbg.forceFlagOr;

		if(PS2Renderer_Dbg.drawOnlyObject && PS2Renderer_Dbg.drawOnlyObject != (unsigned int)&object) return;
		if(PS2Renderer_Dbg.drawOnlyClassID != 0xFFFFFFFF && object.getClassID() != PS2Renderer_Dbg.drawOnlyClassID) return;
		if(PS2Renderer_Dbg.drawSkipClassID != 0xFFFFFFFF && object.getClassID() == PS2Renderer_Dbg.drawSkipClassID) return;
		if(PS2Renderer_Dbg.drawFlagFilterAND && (flags & PS2Renderer_Dbg.drawFlagFilterAND)) return;
		if(PS2Renderer_Dbg.drawFlagFilterNAND && !(flags & PS2Renderer_Dbg.drawFlagFilterNAND)) return;

		static u_int lastframe = 0;
		static u_int currobject = 0;

		if(lastframe != MasterList::GetFrameBeingGenerated())
		{
			PS2Renderer_Dbg.drawObjectCount = currobject;
			currobject = 0;
			lastframe = MasterList::GetFrameBeingGenerated();
//			scePrintf( "-\n" );
		}
		currobject++;
		if(PS2Renderer_Dbg.drawStartObject != 0xFFFFFFFF && 
			PS2Renderer_Dbg.drawEndObject != 0xFFFFFFFF && 
			(currobject <= PS2Renderer_Dbg.drawStartObject || currobject > PS2Renderer_Dbg.drawEndObject+1))
			return;
		else if(PS2Renderer_Dbg.drawStartObject != 0xFFFFFFFF && 
				  PS2Renderer_Dbg.drawEndObject == 0xFFFFFFFF && 
				  currobject != PS2Renderer_Dbg.drawStartObject+1)
			return;
		PS2Renderer_Dbg.mostRecentDrawnObject = (unsigned int)&object;
//		scePrintf( "I:%08X\n", pRenderInst );
#endif

		// all model matricies are dirty each call...
		gDirtyMatrices |= MATRIX_VIEW_TO_MODEL | MATRIX_CAMERA_TO_MODEL | MATRIX_WORLD_TO_MODEL;
		
		// find correct handler...
// 		VUHandler* handler = FindVUHandler(object.GetObjectClassID());
		VUDataClass * handler = (VUDataClass *) object.handler( );
// 		VUDataClass* handler = FindVUHandler(object.GetObjectClassID())->mHandler;
		DBGASSERT(handler);
		
		unsigned int validFlags = flags;

		HackRenderFlags( object, validFlags);
		ValidateRenderFlags( object, validFlags);
		
		// ask the handler what it needs...
		RenderInfo.drawFlags = validFlags;
		RenderInfo.validMatricies = 0;
		RenderInfo.requirementsFlags = 0;
		RenderInfo.lightSpaceRequested = LIGHT_SPACE_NONE;
		handler->getRequirementsFlags(object, RenderInfo);

		// figure out what we need to do to get it the matricies its requested...
		// first build M1...
		const PS2Matrix4x4* M1 = NULL;
		const PS2Matrix4x4* M2 = NULL;
		PS2Matrix4x4 tempMatrix;
		int tempMatrixType = MATRIX_IDENTITY;
		u_int M1Space = validFlags & LOCAL_MATRIX_MASK;
		switch(RenderInfo.requirementsFlags & REQ_M1_SPACE_MASK)
		{
		case REQ_M1_SPACE_DONT_CARE:
			M1 = &ModelMatrix;
			break;
		case REQ_M1_SPACE_WORLD:
#ifdef FORCE_LOCAL_TO_CAMERA_INPUT_SPACE
			M1 = &tempMatrix;
			tempMatrixType = MATRIX_CAMERA_TO_WORLD;
			RenderInfo.validMatricies |= MATRIX_CAMERA_TO_WORLD;
			M1Space = LOCAL_TO_WORLD;
#else // FORCE_LOCAL_TO_CAMERA_INPUT_SPACE
			switch(M1Space)
			{
			case LOCAL_TO_WORLD:
				M1 = &ModelMatrix;
				break;
			case LOCAL_TO_CAMERA:
				M1 = &tempMatrix;
				tempMatrixType = MATRIX_CAMERA_TO_WORLD;
				RenderInfo.validMatricies |= MATRIX_CAMERA_TO_WORLD;
				M1Space = LOCAL_TO_WORLD;
				break;
			case LOCAL_TO_VIEW:
			case LOCAL_TO_CLIP:
			case LOCAL_TO_SCREEN:
				// code not written for this set of options yet!
			default:
				// unknown input matrix type specified.
				RENDER_ASSERT(FALSE);
				break;
			}
#endif // FORCE_LOCAL_TO_CAMERA_INPUT_SPACE
			break;
		case REQ_M1_SPACE_CAMERA:
#ifdef FORCE_LOCAL_TO_CAMERA_INPUT_SPACE
			M1 = &ModelMatrix;
#else // FORCE_LOCAL_TO_CAMERA_INPUT_SPACE

			switch(M1Space)
			{
			case LOCAL_TO_CAMERA:
				M1 = &ModelMatrix;
				break;
			case LOCAL_TO_WORLD:
			case LOCAL_TO_VIEW:
			case LOCAL_TO_CLIP:
			case LOCAL_TO_SCREEN:
				// code not written for this set of options yet!
			default:
				// unknown input matrix type specified.
				RENDER_ASSERT(FALSE);
				break;
			}
#endif // FORCE_LOCAL_TO_CAMERA_INPUT_SPACE
			break;
		case REQ_M1_SPACE_VIEW:
#ifdef FORCE_LOCAL_TO_CAMERA_INPUT_SPACE
			// code not written for this set of options yet!
			RENDER_ASSERT(FALSE);
#else // FORCE_LOCAL_TO_CAMERA_INPUT_SPACE
			switch(M1Space)
			{
			case LOCAL_TO_VIEW:
				M1 = &ModelMatrix;
				break;
			case LOCAL_TO_WORLD:
			case LOCAL_TO_CAMERA:
			case LOCAL_TO_CLIP:
			case LOCAL_TO_SCREEN:
				// code not written for this set of options yet!
			default:
				// unknown input matrix type specified.
				RENDER_ASSERT(FALSE);
				break;
			}
#endif // FORCE_LOCAL_TO_CAMERA_INPUT_SPACE
			break;
		case REQ_M1_SPACE_SCREEN:
#ifdef FORCE_LOCAL_TO_CAMERA_INPUT_SPACE
			// code not written for this set of options yet!
			RENDER_ASSERT(FALSE);
#else // FORCE_LOCAL_TO_CAMERA_INPUT_SPACE
			switch(M1Space)
			{
			case LOCAL_TO_SCREEN:
				M1 = &ModelMatrix;
				break;
			case LOCAL_TO_WORLD:
			case LOCAL_TO_CAMERA:
			case LOCAL_TO_VIEW:
			case LOCAL_TO_CLIP:
				// code not written for this set of options yet!
			default:
				// unknown input matrix type specified.
				RENDER_ASSERT(FALSE);
				break;
			}
#endif // FORCE_LOCAL_TO_CAMERA_INPUT_SPACE
			break;
		case REQ_M1_SPACE_CULL:
#ifdef FORCE_LOCAL_TO_CAMERA_INPUT_SPACE
			// code not written for this set of options yet!
			RENDER_ASSERT(FALSE);
#else // FORCE_LOCAL_TO_CAMERA_INPUT_SPACE
			switch(M1Space)
			{
			case LOCAL_TO_WORLD:
			case LOCAL_TO_CAMERA:
			case LOCAL_TO_VIEW:
			case LOCAL_TO_CLIP:
			case LOCAL_TO_SCREEN:
				// code not written for this set of options yet!
			default:
				// unknown input matrix type specified.
				RENDER_ASSERT(FALSE);
				break;
			}
#endif // FORCE_LOCAL_TO_CAMERA_INPUT_SPACE
			break;
		case REQ_M1_SPACE_CLIP:
#ifdef FORCE_LOCAL_TO_CAMERA_INPUT_SPACE
			// code not written for this set of options yet!
			RENDER_ASSERT(FALSE);
#else // FORCE_LOCAL_TO_CAMERA_INPUT_SPACE
			switch(M1Space)
			{
			case LOCAL_TO_CLIP:
				M1 = &ModelMatrix;
				break;
			case LOCAL_TO_WORLD:
			case LOCAL_TO_CAMERA:
			case LOCAL_TO_VIEW:
			case LOCAL_TO_SCREEN:
				// code not written for this set of options yet!
			default:
				// unknown input matrix type specified.
				RENDER_ASSERT(FALSE);
				break;
			}
#endif // FORCE_LOCAL_TO_CAMERA_INPUT_SPACE
			break;
		default:
			// unknown M1 space requirement specified.
			RENDER_ASSERT(FALSE);
			break;
		}

		// now do M2
		switch(RenderInfo.requirementsFlags & REQ_M1M2_SPACE_MASK)
		{
		case REQ_M1M2_SPACE_VIEW:
			switch(M1Space)
			{
			case LOCAL_TO_CAMERA:
				RenderInfo.validMatricies |= MATRIX_CAMERA_TO_VIEW;
				M2 = &RenderInfo.mCameraToView;
				break;
			case LOCAL_TO_VIEW:
				RenderInfo.validMatricies |= MATRIX_IDENTITY;
				M2 = &RenderInfo.mIdentity;
				break;
			case LOCAL_TO_CLIP:
				RenderInfo.validMatricies |= MATRIX_CLIP_TO_VIEW;
				M2 = &RenderInfo.mClipToView;
				break;
			case LOCAL_TO_WORLD:
				//RenderInfo.validMatricies |= MATRIX_WORLD_TO_VIEW;
				//M2 = &RenderInfo.mWorldToView;
				// code not written for this set of options yet!
				RENDER_ASSERT(FALSE);
				break;
			case LOCAL_TO_SCREEN:
				//RenderInfo.validMatricies |= MATRIX_SCREEN_TO_VIEW;
				//M2 = &RenderInfo.mScreenToView;
				// code not written for this set of options yet!
				RENDER_ASSERT(FALSE);
				break;
			}
			break;
		case REQ_M1M2_SPACE_CULL:
			switch(M1Space)
			{
			case LOCAL_TO_CAMERA:
				RenderInfo.validMatricies |= MATRIX_CAMERA_TO_CULL;
				M2 = &RenderInfo.mCameraToCull;
				break;
			case LOCAL_TO_VIEW:
				RenderInfo.validMatricies |= MATRIX_VIEW_TO_CULL;
				M2 = &RenderInfo.mViewToCull;
				break;
			case LOCAL_TO_CLIP:
				//RenderInfo.validMatricies |= MATRIX_CLIP_TO_CULL;
				//M2 = &RenderInfo.mClipToCull;
				// code not written for this set of options yet!
				RENDER_ASSERT(FALSE);
				break;
			case LOCAL_TO_WORLD:
				RenderInfo.validMatricies |= MATRIX_WORLD_TO_CULL;
				M2 = &RenderInfo.mWorldToCull;
				break;
			case LOCAL_TO_SCREEN:
				RenderInfo.validMatricies |= MATRIX_SCREEN_TO_CULL;
				M2 = &RenderInfo.mScreenToCull;
				break;
			}
			break;
		case REQ_M1M2_SPACE_CLIP:
			switch(M1Space)
			{
			case LOCAL_TO_CAMERA:
				RenderInfo.validMatricies |= MATRIX_CAMERA_TO_CLIP;
				M2 = &RenderInfo.mCameraToClip;
				break;
			case LOCAL_TO_VIEW:
				RenderInfo.validMatricies |= MATRIX_VIEW_TO_CLIP;
				M2 = &RenderInfo.mViewToClip;
				break;
			case LOCAL_TO_CLIP:
				RenderInfo.validMatricies |= MATRIX_IDENTITY;
				M2 = &RenderInfo.mIdentity;
				break;
			case LOCAL_TO_WORLD:
				RenderInfo.validMatricies |= MATRIX_WORLD_TO_CLIP;
				M2 = &RenderInfo.mWorldToClip;
				break;
			case LOCAL_TO_SCREEN:
				RenderInfo.validMatricies |= MATRIX_SCREEN_TO_CLIP;
				M2 = &RenderInfo.mScreenToClip;
				break;
			}
			break;
		case REQ_M1M2_SPACE_SCREEN:
			switch(M1Space)
			{
			case LOCAL_TO_CAMERA:
				RenderInfo.validMatricies |= MATRIX_CAMERA_TO_SCREEN;
				M2 = &RenderInfo.mCameraToScreen;
				break;
			case LOCAL_TO_VIEW:
				RenderInfo.validMatricies |= MATRIX_VIEW_TO_SCREEN;
				M2 = &RenderInfo.mViewToScreen;
				break;
			case LOCAL_TO_CLIP:
				RenderInfo.validMatricies |= MATRIX_CLIP_TO_SCREEN;
				M2 = &RenderInfo.mClipToScreen;
				break;
			case LOCAL_TO_WORLD:
				RenderInfo.validMatricies |= MATRIX_WORLD_TO_SCREEN;
				M2 = &RenderInfo.mWorldToScreen;
				break;
			case LOCAL_TO_SCREEN:
				RenderInfo.validMatricies |= MATRIX_IDENTITY;
				M2 = &RenderInfo.mIdentity;
				break;
			}
			break;
		}

		// now do misc
		if( RenderInfo.requirementsFlags & REQ_CAMERA_TO_WORLD )
		{
			ASSERT( (gDirtyMatrices & MATRIX_WORLD_TO_CAMERA) == 0 );
	
			RenderInfo.validMatricies |= MATRIX_CAMERA_TO_WORLD | MATRIX_WORLD_TO_MODEL;
		}

		if((RenderInfo.lightSpaceRequested & LIGHT_SPACE_MASK) != LIGHT_SPACE_NONE)
		{
			// this call calls UpdateMatrices when its ready...
			HandleLighting(validFlags, ModelMatrix, tempMatrixType, tempMatrix);
		}
		else
		{
			// make sure our matricies are up to date.
			UpdateMatrices(RenderInfo.validMatricies, ModelMatrix, validFlags);
		}

		// record which matricies are up to date...
		RenderInfo.validMatricies = ~gDirtyMatrices;

		// add object to dma chain
		handler->prepDMA(object, *M1, *M2, RenderInfo);

		// recurse into children...
		if( object.next( ))
			DrawDMAObject( * object.next( ), ModelMatrix, flags);
	}

	// init & cleanup the renderer

	void Init(u_int vuMemBase, u_int vuMemSize)
	{
		// TJC - allow this to be called multiple times in a row without
		// affect.
		if(gCodeManager) return;

		// just to make sure... this has bitten me before and this module
		// can be moved into games without the rest of the avalanche engine
		DBGASSERT(sizeof(u_char) == 1);
		DBGASSERT(sizeof(u_int) == 4);
		DBGASSERT(sizeof(u_long) == 8);

		// create vu code manager
		gCodeManager = new VUCodeManager(vuMemBase, vuMemSize);
		DBGASSERT(gCodeManager);
		gHelper = new VUCodeSetupHelper(*gCodeManager);
		DBGASSERT(gHelper);
		
		VUDataClass::setCodeManager(*gCodeManager);

		// add the handlers
		AddHandlers();

		// finish setting up vu code stuff...
		gHelper->setupVUCode();

		// init RenderInfo
		RenderInfo.drawFlags = 0;
		RenderInfo.lclFlags = 0;

		RenderInfo.alphaMultiplier = 1.0f;
		RenderInfo.fogFar = 0.0f;
		RenderInfo.fogNear = 0.0f;
		
		RenderInfo.ShadowR = RenderInfo.ShadowG = RenderInfo.ShadowB = 0x80;
		
		BuildIdentity4x4( RenderInfo.mIdentity);
		BuildIdentity4x4( RenderInfo.mWorldToCamera);
		BuildIdentity4x4( RenderInfo.mCameraToView);
		BuildIdentity4x4( RenderInfo.mTexture);
		BuildIdentity4x4( RenderInfo.mViewToCull);
		BuildIdentity4x4( RenderInfo.mViewToClip);
		BuildIdentity4x4( RenderInfo.mViewToScreen);
	}

	void Cleanup(void)
	{
		// delete everything we alloced..
		delete(gHelper);
		delete(gCodeManager);
		while(gVUHandlers)
		{
			VUHandler* curr = gVUHandlers;
			gVUHandlers = curr->mNext;
			delete curr;
		}
		gCodeManager = NULL;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// This routine converts a raw data buffer from a file
	// into a DDMAObject.  Some types of objects take some
	// additional information, hence the patchSet and patchData
	// values.  For normal static objects, these can be
	// 0 and NULL respectively.
	//
	// The creation routine is not generalized.  There is a
	// different patch routine for each handler (VUDataClass)
	// that supports it.  This routine finds the handler and
	// asks it to patch the dma object.
	// This results in a DDMAObject which
	// can be directly drawn using the call above.
	// The new patched object can refer to the data block's data.  
	// So dont free the data block until your done with all the 
	// objects.
	//
	// The patchSet parameter allows you to select which patch
	// set you want, if there are different patch sets for the
	// model.  Currently patches are sets of vertex colors
	// so a model has a specific number of them preburnt into it.
	//
	// There is also a generic void* patchData that can be used
	// by the patch routine for additional data.  The DMAObject
	// flags determine what that data should be.
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	unsigned int getDMAObjectVertices( DDMAObject * pObject) 
	{ 
		unsigned int verts = 0;

		while( pObject )
		{
			verts += pObject->getVertCount( );

			pObject = pObject->next( );
		}

		return verts; 
	}

	bool checkDMAObjectFlag( DDMAObject * pObject, unsigned int flag )
	{
		while( pObject )
		{
			if( pObject->d_Flags & flag )
				return true;

			pObject = pObject->next( );
		}

		return false;
	}

	void fixupDDMAObjectTextures( void * DMAModelHdrs, unsigned int numPtrs)
	{
	DMAModelHdr **		ppHeaders = (DMAModelHdr **) DMAModelHdrs;

		for( unsigned int i = 0; i < numPtrs; i++)
			DDMAObject::standardFixup( ppHeaders[i] );
	}
	
	DDMAObject * createDDMAObject( void * DMAModelHdrs, unsigned int numPtrs, unsigned int patchSet)
	{

	DMAModelHdr ** 	ppHeaders = (DMAModelHdr **) DMAModelHdrs;
	DDMAObject *		pCurrent = 0;

		for( unsigned int i = numPtrs; i; i--)
		{
			DMAModelHdr *			pHdr = ppHeaders[i - 1];
			VUHandler *				pVUHandler = FindVUHandler( pHdr->mDataClassID );
			VUDataClass *			pDMAHandler = pVUHandler ? pVUHandler->mHandler : 0;

#ifdef _DEBUG
			unsigned int			VUMemAlloc = (pHdr->mFlags >> 24) & 0xff;
			RENDER_ASSERT( VUMemAlloc >= VUDataClass::d_VUMemBase);
#endif

			DDMAObject::standardFixup( pHdr);
			
			switch( pHdr->mFixupClassID)
			{
			case FIXUP_STATIC:
				pCurrent = new StaticDDMAObject( *pHdr, pDMAHandler, pCurrent );
				break;
			case FIXUP_PATCH_SETS:
				pCurrent = new PatchDDMAObject( *( (PatchDMAModelHdr *) pHdr), patchSet, pDMAHandler, pCurrent );
				break;
			case FIXUP_ANIMATED:	
				pCurrent = new AnimDDMAObject( *( (AnimDMAModelHdr *) pHdr), pDMAHandler, pCurrent);
				break;
				
			default:				
				break;
			}
		}

		return pCurrent;
	}

	void destroyDDMAObject( DDMAObject * object)
	{
#if 0
		do
		{
			DDMAObject * deleteMe = object;
		
			object = object->next( );
					
			delete deleteMe;
			
		} while( object);
#else
		// TJC - this is really a bad thing to do.. but for a quick fix,
		// its probably best to just hack it into masterlist instead of
		// putting in a proper "masterlist tells us when frames go by"
		// type fix.
		// TJC - tell master list to delete this for us soon...
		MasterList::DeleteAfter2Frames(object);
#endif
	}

	// this returns the number of patch sets available for this DMA Object.
	// if its 0, then calling PatchDMAObject on this object will simply
	// return the a new object identical to the one you passed (it will refer
	// to the others dma data just like a patched object)...
	u_int getPatchSetCount( void * DMAModelHdrs)
	{
		DMAModelHdr *	pHdr = ((DMAModelHdr **) DMAModelHdrs)[0];
		
		if( ((DMAModelHdr * ) pHdr)->mFixupClassID == FIXUP_PATCH_SETS)
			return ((PatchDMAModelHdr *) pHdr)->mPatchSetCount;
		else
			return 0;
	}


	
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// the patch routines need to be able to resolve texture IDs into binds
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	void SetTextureBindCallback(TextureBindCallback* callback)
	{
		DDMAObject::setTextureBindCallback(callback);
	}

	
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// the render routines need to be able to resolve texture binds into GS settings.
	// this is how that gets done.
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	void SetTextureDataCallback(TextureDataCallback* callback)
	{
		VUDataClass::setTextureDataCallback(callback);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// use this to tell the renderer that you just trashed a chunk of vu instruction mem...
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	void JustTrashedVUInstructionMem(u_int startAddr, u_int instructionCount)
	{
		startAddr = startAddr; instructionCount = instructionCount;
		gCodeManager->trashVUMemBlock(startAddr, instructionCount);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// private functions that are accessed by other modules that make up the renderer
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////

	// add a single handler to the renderer
	void AddHandler(VUDataClass* handler)
	{
		VUHandler* newHandler = new VUHandler(handler);
		
		// let it get its vu memory
		handler->init(*gHelper);
		
		// link it in
		newHandler->mNext = gVUHandlers;
		gVUHandlers = newHandler;
	}


	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// private static functions
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	
	// find the correct handler for a particluar object.
	VUHandler* FindVUHandler(u_int classID)
	{
		// find correct handler...
		VUHandler* handler = gVUHandlers;
		
		while(handler)
		{
			if(handler->mHandler->classID() == (int)classID) break;
			handler = handler->mNext;
		}

		// couldn't find the appropriate microcode handler for this object.
		DBGASSERT(handler);
		
#ifdef _DEBUG
		// make sure there are no more handlers capable of handling this object...
		// not really sure what to do if there are, but this could be pretty
		// confusing to someone trying to figure out why their code changes didnt
		// work and why it wont stop at their breakpoints...
		VUHandler* temp = handler->mNext;
		while(temp)
		{
			DBGASSERT(temp->mHandler->classID() != (int)classID);
			temp = temp->mNext;
		}
#endif
		
		return handler;
	}

	void HandleUpdateMatrices(register unsigned int whichMatrices, const PS2Matrix4x4 ModelMatrix, u_int flags)
	{
		// there are a lot of interdependancies, so all this needs to be 
		// done carefully and in the correct order...
		flags &= LOCAL_MATRIX_MASK;

		// include dependant matrices for model matrices...
		if(whichMatrices & (MATRIX_WORLD_TO_MODEL
								  | MATRIX_CAMERA_TO_MODEL
								  | MATRIX_VIEW_TO_MODEL))
		{
			switch(flags){
			case LOCAL_TO_WORLD:
				whichMatrices |= MATRIX_WORLD_TO_MODEL;
				if(whichMatrices & MATRIX_CAMERA_TO_MODEL)
					whichMatrices |= MATRIX_CAMERA_TO_WORLD;
				if(whichMatrices & MATRIX_VIEW_TO_MODEL)
					whichMatrices |= MATRIX_VIEW_TO_WORLD;
				break;
			case LOCAL_TO_CAMERA:
				whichMatrices |= MATRIX_CAMERA_TO_MODEL;
				if(whichMatrices & MATRIX_WORLD_TO_MODEL)
					whichMatrices |= MATRIX_WORLD_TO_CAMERA;
				if(whichMatrices & MATRIX_VIEW_TO_MODEL)
					whichMatrices |= MATRIX_VIEW_TO_CAMERA;
				break;
			case LOCAL_TO_VIEW:
				whichMatrices |= MATRIX_VIEW_TO_MODEL;
				if(whichMatrices & MATRIX_CAMERA_TO_MODEL)
					whichMatrices |= MATRIX_CAMERA_TO_VIEW;
				if(whichMatrices & MATRIX_WORLD_TO_MODEL)
					whichMatrices |= MATRIX_WORLD_TO_VIEW;
				break;
			case LOCAL_TO_SCREEN:
				whichMatrices |= MATRIX_VIEW_TO_MODEL;
				if(whichMatrices & MATRIX_CAMERA_TO_MODEL)
					whichMatrices |= MATRIX_CAMERA_TO_SCREEN;
				if(whichMatrices & MATRIX_CAMERA_TO_MODEL)
					whichMatrices |= MATRIX_CAMERA_TO_VIEW;
				if(whichMatrices & MATRIX_WORLD_TO_MODEL)
					whichMatrices |= MATRIX_WORLD_TO_VIEW;
				break;
			case LOCAL_TO_CLIP:
				whichMatrices |= MATRIX_VIEW_TO_MODEL;
				if(whichMatrices & MATRIX_CAMERA_TO_MODEL)
					whichMatrices |= MATRIX_CAMERA_TO_CLIP;
				if(whichMatrices & MATRIX_CAMERA_TO_MODEL)
					whichMatrices |= MATRIX_CAMERA_TO_VIEW;
				if(whichMatrices & MATRIX_WORLD_TO_MODEL)
					whichMatrices |= MATRIX_WORLD_TO_VIEW;
				break;
			case LOCAL_TO_CULL:
				whichMatrices |= MATRIX_VIEW_TO_MODEL;
				if(whichMatrices & MATRIX_CAMERA_TO_MODEL)
					whichMatrices |= MATRIX_CAMERA_TO_CULL;
				if(whichMatrices & MATRIX_CAMERA_TO_MODEL)
					whichMatrices |= MATRIX_CAMERA_TO_VIEW;
				if(whichMatrices & MATRIX_WORLD_TO_MODEL)
					whichMatrices |= MATRIX_WORLD_TO_VIEW;
				break;
			}
		}

		// re-eliminate those matrices which are not dirty...
		whichMatrices &= gDirtyMatrices;

		// include dependant matrices for other matrices
		if(whichMatrices & (0
								  | MATRIX_WORLD_TO_SCREEN
								  | MATRIX_SCREEN_TO_CLIP
								  | MATRIX_CLIP_TO_SCREEN
								  | MATRIX_WORLD_TO_CLIP
								  | MATRIX_SCREEN_TO_CULL
								  | MATRIX_CULL_TO_SCREEN
								  | MATRIX_WORLD_TO_CULL
								  | MATRIX_VIEW_TO_WORLD
								  ))
		{
			if(whichMatrices & MATRIX_VIEW_TO_WORLD)
				whichMatrices |= MATRIX_WORLD_TO_VIEW;
			if(whichMatrices & MATRIX_WORLD_TO_SCREEN)
				whichMatrices |= MATRIX_CAMERA_TO_SCREEN;
			if(whichMatrices & MATRIX_SCREEN_TO_CLIP)
				whichMatrices |= MATRIX_CLIP_TO_SCREEN;
			if(whichMatrices & MATRIX_CLIP_TO_SCREEN)
				whichMatrices |= MATRIX_CLIP_TO_VIEW;
			if(whichMatrices & MATRIX_WORLD_TO_CLIP)
				whichMatrices |= MATRIX_CAMERA_TO_CLIP;
			if(whichMatrices & MATRIX_SCREEN_TO_CULL)
				whichMatrices |= MATRIX_CULL_TO_SCREEN;
			if(whichMatrices & MATRIX_CULL_TO_SCREEN)
				whichMatrices |= MATRIX_CULL_TO_VIEW;
			if(whichMatrices & MATRIX_WORLD_TO_CULL)
				whichMatrices |= MATRIX_CAMERA_TO_CULL;

		}

		// re-eliminate those matrices which are not dirty...
		whichMatrices &= gDirtyMatrices;

		// now most common group
		if(whichMatrices & (MATRIX_CAMERA_TO_CLIP | MATRIX_CAMERA_TO_CULL | MATRIX_CAMERA_TO_SCREEN | MATRIX_WORLD_TO_SCREEN))
		{
			if(whichMatrices & (MATRIX_CAMERA_TO_CLIP))
				sceVu0MulMatrix(RenderInfo.mCameraToClip, RenderInfo.mViewToClip, RenderInfo.mCameraToView);
			if(whichMatrices & (MATRIX_CAMERA_TO_CULL))
				sceVu0MulMatrix(RenderInfo.mCameraToCull, RenderInfo.mViewToCull, RenderInfo.mCameraToView);
			if(whichMatrices & (MATRIX_CAMERA_TO_SCREEN))
				sceVu0MulMatrix(RenderInfo.mCameraToScreen, RenderInfo.mViewToScreen, RenderInfo.mCameraToView);
			if(whichMatrices & (MATRIX_WORLD_TO_SCREEN))
				sceVu0MulMatrix(RenderInfo.mWorldToScreen, RenderInfo.mCameraToScreen, RenderInfo.mWorldToCamera);
		}

		// clip related matrices
		if(whichMatrices & (MATRIX_CLIP_TO_SCREEN|MATRIX_CLIP_TO_VIEW|MATRIX_SCREEN_TO_CLIP|MATRIX_WORLD_TO_CLIP))
		{
			if(whichMatrices & (MATRIX_CLIP_TO_VIEW))
				InverseScaleTranslate(RenderInfo.mClipToView, RenderInfo.mViewToClip);
			if(whichMatrices & (MATRIX_CLIP_TO_SCREEN))
				sceVu0MulMatrix(RenderInfo.mClipToScreen, RenderInfo.mViewToScreen, RenderInfo.mClipToView);
			if(whichMatrices & (MATRIX_SCREEN_TO_CLIP))
				InverseScaleTranslate(RenderInfo.mScreenToClip, RenderInfo.mClipToScreen);
			if(whichMatrices & (MATRIX_WORLD_TO_CLIP))
				sceVu0MulMatrix(RenderInfo.mWorldToClip, RenderInfo.mCameraToClip, RenderInfo.mWorldToCamera);
		}
		
		// cull related matrices
		if(whichMatrices & (MATRIX_CULL_TO_SCREEN|MATRIX_CULL_TO_VIEW|MATRIX_SCREEN_TO_CULL|MATRIX_WORLD_TO_CULL))
		{
			if(whichMatrices & (MATRIX_CULL_TO_VIEW))
				InverseScaleTranslate(RenderInfo.mCullToView, RenderInfo.mViewToCull);
			if(whichMatrices & (MATRIX_CULL_TO_SCREEN))
				sceVu0MulMatrix(RenderInfo.mCullToScreen, RenderInfo.mViewToScreen, RenderInfo.mCullToView);
			if(whichMatrices & (MATRIX_SCREEN_TO_CULL))
				InverseScaleTranslate(RenderInfo.mScreenToCull, RenderInfo.mCullToScreen);
			if(whichMatrices & (MATRIX_WORLD_TO_CULL))
				sceVu0MulMatrix(RenderInfo.mWorldToCull, RenderInfo.mCameraToCull, RenderInfo.mWorldToCamera);
		}

		// some intermediate matricies used for lighting usually...
		if(whichMatrices & (MATRIX_CAMERA_TO_WORLD|MATRIX_WORLD_TO_VIEW|MATRIX_VIEW_TO_WORLD|MATRIX_VIEW_TO_CAMERA))
		{
			if(whichMatrices & MATRIX_CAMERA_TO_WORLD)
				Invert( RenderInfo.mCameraToWorld, RenderInfo.mWorldToCamera );	
//				sceVu0InversMatrix(RenderInfo.mCameraToWorld, RenderInfo.mWorldToCamera);
			if(whichMatrices & MATRIX_WORLD_TO_VIEW)
				sceVu0MulMatrix(RenderInfo.mWorldToView, RenderInfo.mCameraToView, RenderInfo.mWorldToCamera);
			if(whichMatrices & MATRIX_VIEW_TO_CAMERA)
				Invert( RenderInfo.mViewToCamera, RenderInfo.mCameraToView );	
//				sceVu0InversMatrix(RenderInfo.mViewToCamera, RenderInfo.mCameraToView);
			if(whichMatrices & MATRIX_VIEW_TO_WORLD)
				Invert( RenderInfo.mViewToWorld, RenderInfo.mWorldToView );	
//				sceVu0InversMatrix(RenderInfo.mViewToWorld, RenderInfo.mWorldToView);
		}

		// to-model matricies
		if(whichMatrices & (MATRIX_WORLD_TO_MODEL|MATRIX_CAMERA_TO_MODEL|MATRIX_VIEW_TO_MODEL))
		{
			// stupid compiler/stupid sony..
			PS2Matrix4x4 stupidTemporaryBecauseOfSony;
			// stupid compiler
			memcpy(stupidTemporaryBecauseOfSony, ModelMatrix, sizeof(PS2Matrix4x4));
			
			// we just do them all if we do one... its easier that way
			switch(flags)
			{
			case LOCAL_TO_WORLD:
				if(whichMatrices & MATRIX_WORLD_TO_MODEL)
					Invert( RenderInfo.mWorldToModel, stupidTemporaryBecauseOfSony );	
//					sceVu0InversMatrix(RenderInfo.mWorldToModel, stupidTemporaryBecauseOfSony);
				if(whichMatrices & MATRIX_CAMERA_TO_MODEL)
					sceVu0MulMatrix(RenderInfo.mCameraToModel, RenderInfo.mWorldToModel, RenderInfo.mCameraToWorld);
				if(whichMatrices & MATRIX_VIEW_TO_MODEL)
					sceVu0MulMatrix(RenderInfo.mViewToModel, RenderInfo.mWorldToModel, RenderInfo.mViewToWorld);
				break;
			case LOCAL_TO_CAMERA:
				if(whichMatrices & MATRIX_CAMERA_TO_MODEL)
					Invert( RenderInfo.mCameraToModel, stupidTemporaryBecauseOfSony );	
//					sceVu0InversMatrix(RenderInfo.mCameraToModel, stupidTemporaryBecauseOfSony);
				if(whichMatrices & MATRIX_WORLD_TO_MODEL)
					sceVu0MulMatrix(RenderInfo.mWorldToModel, RenderInfo.mCameraToModel, RenderInfo.mWorldToCamera);
				if(whichMatrices & MATRIX_VIEW_TO_MODEL)
					sceVu0MulMatrix(RenderInfo.mViewToModel, RenderInfo.mCameraToModel, RenderInfo.mViewToCamera);
				break;
			case LOCAL_TO_VIEW:
				if(whichMatrices & MATRIX_VIEW_TO_MODEL)
					Invert( RenderInfo.mViewToModel, stupidTemporaryBecauseOfSony );	
//					sceVu0InversMatrix(RenderInfo.mViewToModel, stupidTemporaryBecauseOfSony);
				if(whichMatrices & MATRIX_CAMERA_TO_MODEL)
					sceVu0MulMatrix(RenderInfo.mCameraToModel, RenderInfo.mViewToModel, RenderInfo.mCameraToView);
				if(whichMatrices & MATRIX_WORLD_TO_MODEL)
					sceVu0MulMatrix(RenderInfo.mWorldToModel, RenderInfo.mViewToModel, RenderInfo.mWorldToView);
				break;
			case LOCAL_TO_SCREEN:
				if(whichMatrices & MATRIX_CAMERA_TO_MODEL)
				{
					Invert( RenderInfo.mCameraToModel, stupidTemporaryBecauseOfSony );	
//					sceVu0InversMatrix(RenderInfo.mCameraToModel, stupidTemporaryBecauseOfSony);
					sceVu0MulMatrix(RenderInfo.mCameraToModel, RenderInfo.mCameraToModel, RenderInfo.mCameraToScreen);
				}
				if(whichMatrices & MATRIX_CAMERA_TO_MODEL)
					sceVu0MulMatrix(RenderInfo.mCameraToModel, RenderInfo.mViewToModel, RenderInfo.mCameraToView);
				if(whichMatrices & MATRIX_WORLD_TO_MODEL)
					sceVu0MulMatrix(RenderInfo.mWorldToModel, RenderInfo.mViewToModel, RenderInfo.mWorldToView);
				break;
			case LOCAL_TO_CLIP:
				if(whichMatrices & MATRIX_CAMERA_TO_MODEL)
				{
					Invert( RenderInfo.mCameraToModel, stupidTemporaryBecauseOfSony );	
//					sceVu0InversMatrix(RenderInfo.mCameraToModel, stupidTemporaryBecauseOfSony);
					sceVu0MulMatrix(RenderInfo.mCameraToModel, RenderInfo.mCameraToModel, RenderInfo.mCameraToClip);
				}
				if(whichMatrices & MATRIX_CAMERA_TO_MODEL)
					sceVu0MulMatrix(RenderInfo.mCameraToModel, RenderInfo.mViewToModel, RenderInfo.mCameraToView);
				if(whichMatrices & MATRIX_WORLD_TO_MODEL)
					sceVu0MulMatrix(RenderInfo.mWorldToModel, RenderInfo.mViewToModel, RenderInfo.mWorldToView);
				break;
			case LOCAL_TO_CULL:
				if(whichMatrices & MATRIX_CAMERA_TO_MODEL)
				{
					Invert( RenderInfo.mCameraToModel, stupidTemporaryBecauseOfSony );	
//					sceVu0InversMatrix(RenderInfo.mCameraToModel, stupidTemporaryBecauseOfSony);
					sceVu0MulMatrix(RenderInfo.mCameraToModel, RenderInfo.mCameraToModel, RenderInfo.mCameraToCull);
				}
				if(whichMatrices & MATRIX_CAMERA_TO_MODEL)
					sceVu0MulMatrix(RenderInfo.mCameraToModel, RenderInfo.mViewToModel, RenderInfo.mCameraToView);
				if(whichMatrices & MATRIX_WORLD_TO_MODEL)
					sceVu0MulMatrix(RenderInfo.mWorldToModel, RenderInfo.mViewToModel, RenderInfo.mWorldToView);
				break;
			}
		}
		
		// mark clean...
		gDirtyMatrices &= ~whichMatrices;
	}
	
	void ToggleLightsOnOffFlag()
	{
		sLightsAreOn ^= 1;
		
		if(sLightsAreOn)
		{
			// copy lights into RenderInfo...
			// set default lights into RenderInfo
			for(int i = 0; i < MAX_LIGHTS; i++)
			{
				if(RenderInfo.mLights.mLight[i].mEnabled)
				{
					memcpy(&RenderInfo.mLights.mLight[i], &sBackupLights.mLight[i], sizeof(PS2LightData));
					RenderInfo.mLights.mLight[i].mEnabled = TRUE;
				}
			}
			if(RenderInfo.mLights.mAmbientLight.mEnabled)
			{
				memcpy(&RenderInfo.mLights.mAmbientLight, &sBackupLights.mAmbientLight, sizeof(PS2AmbientLightData));
				RenderInfo.mLights.mAmbientLight.mEnabled = TRUE;
			}
		}
		else
		{
			// copy lights into RenderInfo...
			// set default lights into RenderInfo
			for(int i = 0; i < MAX_LIGHTS; i++)
				if(RenderInfo.mLights.mLight[i].mEnabled)
					memcpy(&sBackupLights.mLight[i], &RenderInfo.mLights.mLight[i], sizeof(PS2LightData));
			memcpy(&sBackupLights.mAmbientLight, &RenderInfo.mLights.mAmbientLight, sizeof(PS2AmbientLightData));
				
			
			for(int i = 0; i < MAX_LIGHTS; i++)
			{
				RenderInfo.mLights.mLight[i].mDiffuseColor[0]	=
				RenderInfo.mLights.mLight[i].mDiffuseColor[1]	=
				RenderInfo.mLights.mLight[i].mDiffuseColor[2]	=
				RenderInfo.mLights.mLight[i].mDiffuseColor[3]	=
				RenderInfo.mLights.mLight[i].mSpecularColor[0]	=
				RenderInfo.mLights.mLight[i].mSpecularColor[1]	=
				RenderInfo.mLights.mLight[i].mSpecularColor[2]	=
				RenderInfo.mLights.mLight[i].mSpecularColor[3]	=
				RenderInfo.mLights.mLight[i].mAmbientColor[0]	=
				RenderInfo.mLights.mLight[i].mAmbientColor[1]	=
				RenderInfo.mLights.mLight[i].mAmbientColor[2]	=
				RenderInfo.mLights.mLight[i].mAmbientColor[3]	= 0.0f;
			}
			RenderInfo.mLights.mAmbientLight.mColor[0] =
			RenderInfo.mLights.mAmbientLight.mColor[1] =
			RenderInfo.mLights.mAmbientLight.mColor[2] =
			RenderInfo.mLights.mAmbientLight.mColor[3] = 128.0f;
		}
	}

static inline float length( PS2Vector4 &vec )
{
	return( sqrtf( ( vec[ 0 ] * vec[ 0 ] ) + ( vec[ 1 ] * vec[ 1 ] ) + ( vec[ 2 ] * vec[ 2 ] ) ) );
}

static inline void normalize( PS2Vector4 &vec )
{
	float len = length( vec );
	float invlen = 0.0f;
	
	if( len != 0 )
		invlen = 1.0f / len;
		
	vec[0] *= invlen;
	vec[1] *= invlen;
	vec[2] *= invlen;
	vec[3] *= invlen;
}
		
static inline float dot( PS2Vector4 &vec1, PS2Vector4 &vec2 )
{
	return( ( ( vec1[ 0 ] * vec2[ 0 ] ) + ( vec1[ 1 ] * vec2[ 1 ] ) + ( vec1[ 2 ] * vec2[ 2 ] ) ) );
}

static void cross( PS2Vector4 &dest, PS2Vector4 &vec1, PS2Vector4 &vec2 )
{
	dest[ 0 ] = vec1[ 1 ] * vec2[ 2 ] - vec2[ 1 ] * vec1[ 2 ];
	dest[ 1 ] = vec2[ 0 ] * vec1[ 2 ] - vec1[ 0 ] * vec2[ 2 ];
	dest[ 2 ] = vec1[ 0 ] * vec2[ 1 ] - vec2[ 0 ] * vec1[ 1 ];
	dest[ 3 ] = 0.0f;	
}

/***************************************************************************/
// Return the det of a 2x2
/***************************************************************************/
inline float Determinant( float _11, float _12, float _21, float _22 )
{
	return( _11 * _22 - _21 * _12);
}

/***************************************************************************/
// Return the det of a 3x3
/***************************************************************************/
inline float Determinant( 	float _11, float _12, float _13, 
									float _21, float _22, float _23, 
									float _31, float _32, float _33)
{
	return(_11 * Determinant(_22, _23, _32, _33) -
			 _12 * Determinant(_21, _23, _31, _33) +
			 _13 * Determinant(_21, _22, _31, _32));
}

/***************************************************************************/
// Return the det of a 4x4
/***************************************************************************/
inline float Determinant( 	float _11, float _12, float _13, float _14, 
							float _21, float _22, float _23, float _24, 
							float _31, float _32, float _33, float _34, 
							float _41, float _42, float _43, float _44)
{
	return(_11 * Determinant(_22, _23, _24, _32, _33, _34, _42, _43, _44) -
			 _12 * Determinant(_21, _23, _24, _31, _33, _34, _41, _43, _44) +
			 _13 * Determinant(_21, _22, _24, _31, _32, _34, _41, _42, _44) -
			 _14 * Determinant(_21, _22, _23, _31, _32, _33, _41, _42, _43));
}

inline float Determinant( const PS2Matrix4x4 &Src)
{
	return( Determinant( Src[0][0], Src[0][1], Src[0][2], Src[0][3],
								Src[1][0], Src[1][1], Src[1][2], Src[1][3],
								Src[2][0], Src[2][1], Src[2][2], Src[2][3],
								Src[3][0], Src[3][1], Src[3][2], Src[3][3]));
}
							
/***************************************************************************/
// Return the Inverse of a matrix -- this is a specialized invert of a 4x4
// that assumes the 4th column is ( 0, 0, 0, 1 ), derived using cramer's rule.
/***************************************************************************/
// The vu macro mode code was generated based on the following "c" code.
//const register float v0x = m[0][0];	const register float v0y = m[0][1];	const register float v0z = m[0][2];
//const register float v1x = m[1][0];	const register float v1y = m[1][1];	const register float v1z = m[1][2];
//const register float v2x = m[2][0];	const register float v2y = m[2][1];	const register float v2z = m[2][2];
//const register float v3x = m[3][0];	const register float v3y = m[3][1];	const register float v3z = m[3][2];
//register float det = v0x*( v1y*v2z - v2y*v1z ) + v0y*( v1z*v2x - v2z*v1x ) + v0z*( v1x*v2y - v2x*v1y )
//det = 1.0f / det;
//r[0][0] =  ( v1y*v2z - v1z*v2y ) * det;
//r[0][1] = -( v0y*v2z - v2y*v0z ) * det;
//r[0][2] =  ( v0y*v1z - v1y*v0z ) * det;
//r[0][3] =  0.0f;
//r[1][0] =  ( v1z*v2x - v2z*v1x) * det;
//r[1][1] = -( v0z*v2x - v0x*v2z ) * det;
//r[1][2] =  ( v0z*v1x - v1z*v0x ) * det;
//r[1][3] =  0.0f;
//r[2][0] =  ( v1x*v2y - v1y*v2x ) * det;
//r[2][1] = -( v0x*v2y - v2x*v0y ) * det;
//r[2][2] =  ( v0x*v1y - v1x*v0y ) * det;
//r[2][3] =  0.0f;
//r[3][0] = -( v1x*( v2y*v3z - v3y*v2z ) + v1y*( v2z*v3x - v3z*v2x ) + v1z*( v2x*v3y - v3x*v2y ) ) * det
//r[3][1] =  ( v0x*( v2y*v3z - v3y*v2z ) + v0y*( v2z*v3x - v3z*v2x ) + v0z*( v2x*v3y - v3x*v2y ) ) * det;
//r[3][2] = -( v0x*( v1y*v3z - v3y*v1z ) + v0y*( v1z*v3x - v3z*v1x ) + v0z*( v1x*v3y - v3x*v1y ) ) * det;
//r[3][3] = 1.0f;
//
// Basically by reorganizing and factoring we can compute the inverse on the vu by performaning a number
// of cross prducts and vector multiplies
//
#define MICROMODE 1

#if 1
#if MICROMODE
bool Invert( PS2Matrix4x4& r, const PS2Matrix4x4& mtx )
{
	ASSERT( mtx[0][3] == 0.0f );
	ASSERT( mtx[1][3] == 0.0f );
	ASSERT( mtx[2][3] == 0.0f );
	ASSERT( mtx[3][3] == 1.0f );
		register u128 row0 asm("vf1");
		register u128 row1 asm("vf2");
		register u128 row2 asm("vf3");
		register u128 row3 asm("vf4");
		register u128 r0 asm("vf5");
		register u128 r1 asm("vf6");
		register u128 r2 asm("vf7");
		register u128 r3 asm("vf8");

		asm __volatile__("
			lqc2		row0, 0x00(mtx)
			lqc2		row1, 0x10(mtx)
			lqc2		row2, 0x20(mtx)
			lqc2		row3, 0x30(mtx)
			vcallms	PS2_Invert
			qmfc2.i	t0, r0
			qmfc2		t1, r1
			qmfc2		t2, r2
			qmfc2		t3, r3
			" : "=j row0" (row0),"=j row1" (row1),"=j row2" (row2),"=j row3" (row3) : "r mtx" (mtx), "j r0" (r0), "j r1" (r1), "j r2" (r2), "j r3" (r3) );
		asm __volatile__("sq					t0, 0x00(r)						" : : "r r" ( r ) : "t0" );
		asm __volatile__("sq					t1, 0x10(r)						" : : "r r" ( r ) : "t1" );
		asm __volatile__("sq					t2, 0x20(r)						" : : "r r" ( r ) : "t2" );
		asm __volatile__("sq					t3, 0x30(r)						" : : "r r" ( r ) : "t3" );

#else
bool Invert( PS2Matrix4x4& r, const PS2Matrix4x4& m )
{
	u128 det, v0, v1, v2, v3, vone;
	u128 r0, r1, r2;
	u128 temp1, temp2, temp3;

	// --load matrix into vectors
	typedef struct { u128 row0; u128 row1; u128 row2; u128 row3; } mtx;
	mtx* mm = (mtx*)&m;
	v0 = mm->row0;
	v1 = mm->row1;
	v2 = mm->row2;
	v3 = mm->row3;

	// --- set vone to ( 1.0, 1.0, 1.0, -1.0 )
	// --- start determinant
	asm __volatile__("
								vmulx.w			vone, vf00, vf00x
								vaddw.xyz		vone, vf00, vf00w
								vopmula.xyz		ACCxyz, v1, v2
								vopmsub.xyz		det, v2, v1			
								vsubw.w			vone, vone, vf00w				" : "=&j vone" ( vone ), "=&j det" ( det ) : "j v1" ( v1 ), "j v2" ( v2 ) );

	// --- start computing cross products and continue determinant
	asm __volatile__("
								vopmula.xyz		ACCxyz, v1, v2
								vopmsub.xyz		r0, v2, v1
								vmul.xyz			det, det, v0
								vopmula.xyz		ACCxyz, v0, v2
								vopmsub.xyz		r1, v2, v0
								vopmula.xyz		ACCxyz, v0, v1
								vopmsub.xyz		r2, v1, v0
								vmulax.x			ACC, vone, detx
								vmadday.x		ACC, vone, dety	
								vmaddz.x			det, vone, detz				" :"+&j det" ( det ), "=&j r0" ( r0 ), "=&j r1" ( r1 ), "=&j r2" ( r2 ) : "j vone" ( vone ), "j v0" ( v0 ), "j v1" ( v1 ), "j v2" ( v2 ));
			
		// --- more cross products and start divide for 1.0f / determinant
		asm __volatile__("
								vopmula.xyz		ACCxyz, v2, v3
								vopmsub.xyz		temp1, v3, v2
								vopmula.xyz		ACCxyz, v2, v3
								vdiv				Q, vf00w, detx					" : "+&j det" ( det ), "=&j temp1" ( temp1 ) : "j v2" ( v2 ), "j v3" ( v3 ) );

		asm __volatile__("vopmsub.xyz		temp2, v3, v2					" : "=&j temp2" ( temp2 ) : "j v2" ( v2 ), "j v3" ( v3 ) );
		asm __volatile__("vopmula.xyz		ACCxyz, v1, v3					" : : "j v1" ( v1 ), "j v3" ( v3 ) );
		asm __volatile__("vopmsub.xyz		temp3, v3, v1					" : "=&j temp3" ( temp3 ) : "j v1" ( v1 ), "j v3" ( v3 ) );
		asm __volatile__("vmul.xyz			temp1, temp1, v1				" : "+&j temp1" ( temp1 ) : "j v1" ( v1 ) );
		asm __volatile__("vmul.xyz			temp2, temp2, v0				" : "+&j temp2" ( temp2 ) : "j v0" ( v0 ) );
		asm __volatile__("vmul.xyz			temp3, temp3, v0				" : "+&j temp3" ( temp3 ) : "j v0" ( v0 ) );
		asm __volatile__("vaddq.xyz		det, vf00, Q					" : "+&j det" ( det ) : );
		asm __volatile__("vmul.xyz			r0, r0, det						" : "+&j det" ( det ), "+&j r0" ( r0 ) );
		asm __volatile__("vmul.xyz			r1, r1, det						" : "+&j det" ( det ), "+&j r1" ( r1 ) );
		asm __volatile__("vmul.xyz			r2, r2, det						" : "+&j det" ( det ), "+&j r2" ( r2 ) );
		asm __volatile__("vmulw.xyz		r1, r1, vone					" : "+&j r1" ( r1 ) : "j vone" ( vone ) );
		asm __volatile__("vmul.xyz			temp1, temp1, det				" : "+&j det" ( det ), "+&j temp1" ( temp1 ) );
		asm __volatile__("vmul.xyz			temp2, temp2, det				" : "+&j det" ( det ), "+&j temp2" ( temp2 ) );
		asm __volatile__("vmul.xyz			temp3, temp3, det				" : "+&j det" ( det ), "+&j temp3" ( temp3 ) );
		asm __volatile__("vmulw.w			temp1, vf00, vf00w			" : "+&j temp1" ( temp1 ) );
		asm __volatile__("vmulax.x			ACC, vone, temp1x				" : : "j vone" ( vone ), "j temp1" ( temp1 ) );
		asm __volatile__("vmadday.x		ACC, vone, temp1y				" : : "j vone" ( vone ), "j temp1" ( temp1 ) );
		asm __volatile__("vmaddaz.x		ACC, vone, temp1z				" : : "j vone" ( vone ), "j temp1" ( temp1 ) );
		asm __volatile__("vmulax.y			ACC, vone, temp2x				" : : "j vone" ( vone ), "j temp2" ( temp2 ) );
		asm __volatile__("vmadday.y		ACC, vone, temp2y				" : : "j vone" ( vone ), "j temp2" ( temp2 ) );
		asm __volatile__("vmaddaz.y		ACC, vone, temp2z				" : : "j vone" ( vone ), "j temp2" ( temp2 ) );
		asm __volatile__("vmulax.z			ACC, vone, temp3x				" : : "j vone" ( vone ), "j temp3" ( temp3 ) );
		asm __volatile__("vmadday.z		ACC, vone, temp3y				" : : "j vone" ( vone ), "j temp3" ( temp3 ) );
		asm __volatile__("vmaddaz.z		ACC, vone, temp3z				" : : "j vone" ( vone ), "j temp3" ( temp3 ) );
		asm __volatile__("vmaddx.xyz		temp1, vf00, vf00x			" : : "j temp1" ( temp1 ) );

		// --- finally transpose the resultant vectors ( r0, r1, r2, temp1 ) and save into r
		asm __volatile__("qmfc2				t0, r0							" : : "j r0" ( r0 ) : "t0" );
		asm __volatile__("qmfc2				t1, r1							" : : "j r1" ( r1 ) : "t1" );
		asm __volatile__("qmfc2				t2, r2							" : : "j r2" ( r2 ) : "t2" );
		asm __volatile__("vmulw.xz			temp1, temp1, vone			" : : "j temp1" ( temp1 ), "j vone" ( vone ) );
		asm __volatile__("qmfc2				t3, vf00							" );
		asm __volatile__("pextlw			t4, t1, t0						" : : : "t4", "t1", "t0" );
		asm __volatile__("pextuw			t5,t1,t0							" : : : "t5", "t1", "t0" );
		asm __volatile__("pextlw			t6, t3, t2						" : : : "t6", "t3", "t2" );
		asm __volatile__("pextuw			t7,t3,t2							" : : : "t7", "t3", "t2" );
		asm __volatile__("pcpyld			t0,t6,t4							" : : : "t0", "t6", "t4" );
		asm __volatile__("pcpyud			t1,t4,t6							" : : : "t1", "t4", "t6" );
		asm __volatile__("pcpyld			t2,t7,t5							" : : : "t2", "t7", "t5" );
		asm __volatile__("qmfc2				t3, temp1						" : : "j temp1" ( temp1 ) : "t3" );
		asm __volatile__("sq					t0, 0x00(r)						" : : "r r" ( r ) : "t0" );
		asm __volatile__("sq					t1, 0x10(r)						" : : "r r" ( r ) : "t1" );
		asm __volatile__("sq					t2, 0x20(r)						" : : "r r" ( r ) : "t2" );
		asm __volatile__("sq					t3, 0x30(r)						" : : "r r" ( r ) : "t3" );
#endif
	return( true );
}
#else
bool Invert( PS2Matrix4x4& r, const PS2Matrix4x4& m )
{
	const register float v0x = m[0][0];	const register float v0y = m[0][1];	const register float v0z = m[0][2];
	const register float v1x = m[1][0];	const register float v1y = m[1][1];	const register float v1z = m[1][2];
	const register float v2x = m[2][0];	const register float v2y = m[2][1];	const register float v2z = m[2][2];
	const register float v3x = m[3][0];	const register float v3y = m[3][1];	const register float v3z = m[3][2];

	register float det = v0x*v1y*v2z - v0x*v1z*v2y - v0y*v1x*v2z + v0y*v1z*v2x + v0z*v1x*v2y - v0z*v1y*v2x;
//	if ( det == 0.0f )
//		return false;
	det = 1.0f / det;

	r[0][0] =  ( v1y*v2z - v1z*v2y ) * det;
	r[0][1] = -( v0y*v2z - v0z*v2y ) * det;
	r[0][2] =  ( v0y*v1z - v0z*v1y ) * det;
	r[0][3] = 0.0f;

	r[1][0] = -( v1x*v2z - v1z*v2x ) * det;
	r[1][1] =  ( v0x*v2z - v0z*v2x ) * det;
	r[1][2] = -( v0x*v1z - v0z*v1x ) * det;
	r[1][3] = 0.0f;

	r[2][0] =  ( v1x*v2y - v1y*v2x ) * det;
	r[2][1] = -( v0x*v2y - v0y*v2x ) * det;
	r[2][2] =  ( v0x*v1y - v0y*v1x ) * det;
	r[2][3] = 0.0f;

	r[3][0] = -( v1x*v2y*v3z - v1x*v2z*v3y - v1y*v2x*v3z + v1y*v2z*v3x + v1z*v2x*v3y - v1z*v2y*v3x ) * det;
	r[3][1] =  ( v0x*v2y*v3z - v0x*v2z*v3y - v0y*v2x*v3z + v0y*v2z*v3x + v0z*v2x*v3y - v0z*v2y*v3x ) * det;
	r[3][2] = -( v0x*v1y*v3z - v0x*v1z*v3y - v0y*v1x*v3z + v0y*v1z*v3x + v0z*v1x*v3y - v0z*v1y*v3x ) * det;
	r[3][3] = 1.0f;

	return true;
}
#endif
/***************************************************************************/
void Normalize( PS2Matrix4x4 &Dest, const PS2Matrix4x4 &Src )
{
	PS2Vector4 			X, Y, Z;
	PS2Vector4 			Scale;
	
// Copy these into some nicer vectors
// Vector3 x, y, z;
// localToWorld.GetRow0(x);
// localToWorld.GetRow1(y);
// localToWorld.GetRow2(z);
	my_sceVu0CopyVector(X, Src[0]);
	my_sceVu0CopyVector(Y, Src[1]);
	my_sceVu0CopyVector(Z, Src[2]);
	
	X[3] = 0.0f;
	Y[3] = 0.0f;
	Z[3] = 0.0f;
	
// compute the scale
// Vector3 scale;
// scale.x = x.Length();
// x.Normalize();
	Scale[0] = length( X ); normalize( X );

// Figure out the XY shear and fix Y
// float shearXY = Vector3::Dot(x, y);
// y -= x * shearXY;
// scale.y = y.Length();
// y.Normalize();
// shearXY /= scale.y;
	float shearXY = dot( X, Y );

	Y[0] -= X[0] * shearXY;
	Y[1] -= X[1] * shearXY;
	Y[2] -= X[2] * shearXY;
		
	Scale[1] = length( Y ); normalize( Y );

	shearXY /= Scale[1];

// Figure out the XZ & YZ shears
// float shearXZ = Vector3::Dot(x, z);
// z -= x * shearXZ;
// float shearYZ = Vector3::Dot(y, z);
// z -= y * shearYZ;
 	float shearXZ = dot( X, Z );

	Z[0] -= X[0] * shearXZ;
	Z[1] -= X[1] * shearXZ;
	Z[2] -= X[2] * shearXZ;

 	float shearYZ = dot( Y, Z );

	Z[0] -= Y[0] * shearYZ;
	Z[1] -= Y[1] * shearYZ;
	Z[2] -= Y[2] * shearYZ;
 
// Next, get Z scale and normalize 3rd row. 
// scale.z = z.Length();
// z.Normalize();
// shearXZ /= scale.z;
// shearYZ /= scale.z;
	Scale[2] = length( Z ); normalize( Z );
	shearXZ /= Scale[2];
	shearYZ /= Scale[2];
	
// Check for a coordinate system flip.
// if (Vector3::Dot(x, Vector3::Cross(y, z)) < 0)
// {
//  scale *= -1.0f;
//  x *= -1.0f;
//  y *= -1.0f;
//  z *= -1.0f;
// }
	PS2Vector4			YCrossZ;
	cross( YCrossZ, Y, Z );
	
	if( dot( X, YCrossZ ) < 0.0f )
	{
		Scale[0] *= -1.0f; Scale[1] *= -1.0f; Scale[2] *= -1.0f;

		X[0] *= -1.0f; X[1] *= -1.0f; X[2] *= -1.0f;
		Y[0] *= -1.0f; Y[1] *= -1.0f; Y[2] *= -1.0f;
		Z[0] *= -1.0f; Z[1] *= -1.0f; Z[2] *= -1.0f;
	}
	
// Fix this bastard up!
// z = Vector3::Cross(x, y);
// z.Normalize();
// y = Vector3::Cross(z, x);
	cross( Z, X, Y );
	normalize( Z );
	
	cross( Y, Z, X );

// Set dest
	Dest[0] = X;
	Dest[1] = Y;
	Dest[2] = Z;
	Dest[3][0] = 0.0f;
	Dest[3][1] = 0.0f;
	Dest[3][2] = 0.0f;
	Dest[3][3] = 1.0f;
}	
			
bool deconstruct( const PS2Matrix4x4 &src, PS2Matrix4x4 &rot, PS2Vector4 &scaleXYZW )
{

	float temp = Determinant( src );
	if ((temp < 1.0e-6f) && (temp > -1.0e-6f))
		return ( false );

	PS2Vector4  row0, row1, row2;
	float		scaleX, scaleY, scaleZ;
	float		shearXY, shearXZ, shearYZ;

	row0 = *( ( PS2Vector4 * )&src[ 0 ][ 0 ] );
	row1 = *( ( PS2Vector4 * )&src[ 1 ][ 0 ] );
	row2 = *( ( PS2Vector4 * )&src[ 2 ][ 0 ] );

	scaleX = length( row0 );
	row0[ 0 ] /= scaleX;
	row0[ 1 ] /= scaleX;
	row0[ 2 ] /= scaleX;

	shearXY = dot( row0, row1 );
//	row1 -= shearXY * row0;
	row1[ 0 ] -= ( shearXY * row0[ 0 ] );
	row1[ 1 ] -= ( shearXY * row0[ 1 ] );
	row1[ 2 ] -= ( shearXY * row0[ 2 ] );

	scaleY = length( row1 );
	temp = 1.0f / scaleY;
//	row1 *= temp;
	row1[ 0 ] *= temp;
	row1[ 1 ] *= temp;
	row1[ 2 ] *= temp;
	shearXY *= temp;

	shearXZ = dot( row0, row2 );
	shearYZ = dot( row1, row2 );
//	row2 -= shearXZ * row0 + shearYZ * row1;
	row2[ 0 ] -= ( shearXZ * row0[ 0 ] + shearYZ * row1[ 0 ] );
	row2[ 1 ] -= ( shearXZ * row0[ 1 ] + shearYZ * row1[ 1 ] );
	row2[ 2 ] -= ( shearXZ * row0[ 2 ] + shearYZ * row1[ 2 ] );

	scaleZ = length( row2 );
	temp = 1.0f / scaleZ;
//	row2 *= temp;
	row2[ 0 ] *= temp;
	row2[ 1 ] *= temp;
	row2[ 2 ] *= temp;
	shearXZ *= temp;
	shearYZ *= temp;

#if 0
	rot.SetRow0(row0);
	rot.SetRow1(row1);
	rot.SetRow2(row2);

  if (rot.Det() < 0.0f)
	{
		 (Matrix3x3)rot *= -1.0f;
		 scaleX = -scaleX;
		 scaleY = -scaleY;
		 scaleZ = -scaleZ;
	}
#endif
	scaleXYZW[ 0 ] = scaleX;
	scaleXYZW[ 1 ] = scaleY;
	scaleXYZW[ 2 ] = scaleZ;
	return (true);
}

} // namespace PS2Renderer

int renderassert( const char* expr, const char* file, int line )
{
	scePrintf( "RENDER ASSERT FAILED: %s:%d (%s)\n", file, line, expr );
	asm( "break 1" );
	return 0;
}
