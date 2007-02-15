/*************************************/
/*                                   */
/*   LightMgr.h                      */
/*   new engine  12/07/00 (rename)   */
/*   original file created 11/6/98   */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   light manager                   */
/*                                   */
/*************************************/

#ifndef __LIGHTMGR_H
#define __LIGHTMGR_H

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include "Layers/LayersStruct.h"			// low-level include
#include "Math/Vector.h"

/******************* forward declarations ****************/

class CScene;		// from Managers/SceneMgr.h

/******************* defines *****************************/

#define LIGHT_ON							0x00000001

/* known types */

#define LIGHT_POINT						0x00000002
#define LIGHT_DIRECTIONAL				0x00000004
#define LIGHT_TEXTURE_SPOTLIGHT		0x00000008

/* categories */

#define LIGHT_LIGHTING_TYPE			(LIGHT_POINT | LIGHT_DIRECTIONAL)		// light contributes lights to rendered objects
#define LIGHT_EFFECT_TYPE				(LIGHT_TEXTURE_SPOTLIGHT)					// light is a special effect

/******************* macros ******************************/

/******************* structures **************************/

struct ts_Light
{
	u32						u32Flags;

	Vector3Packed			Pos;
	float						fFalloffStart;
	float						fFalloff;
	ts_fRGB					Color;

	/* transformed from world space to camera space, with w available for opengl infinite/local flag */

	Vector4					CameraPos;
};

/******************* global variables ********************/

/******************* global prototypes *******************/

void		LightMgr__Init(int nMaxLights);
void		LightMgr__Reset(void);
void		LightMgr__Shutdown(void);
void		LightMgr__SetAmbient(ts_fRGB *pColor);
ts_fRGB	*LightMgr__GetAmbient(void);
void		LightMgr__AdjustAmbient(ts_fRGB *pColor);
void		LightMgr__AdjustLights(ts_fRGB Color, float fLightColorMult);
void		LightMgr__RestoreLights(void);
void		LightMgr__SetLightColorMult(float fLightColorMult);
void		LightMgr__GetLightColorMult(bool *pbLightColorMultDirty, float *pfLightColorMult);
void		LightMgr__ClearLightColorMultDirty(void);

t_Handle	LightMgr__RegisterLight(ts_Light *pLight);
bool		LightMgr__UpdateLightPos(t_Handle Handle, Vector3 *pvPos);
bool		LightMgr__UpdateLightColor(t_Handle Handle, ts_fRGB *pColor);
t_Handle	LightMgr__RegisterTextureSpotlight(Vector3 &vPos, Vector3 &vDirection, float fNearClip, float fFarClip, float fFieldOfViewDegrees, const char *pTextureFile);
bool		LightMgr__UpdateTextureSpotlight(t_Handle Handle, Vector3 &vPos, Vector3 &vDirection);
bool		LightMgr__RemoveLight(t_Handle Handle);
void		LightMgr__RemoveAllLights(void);
void		LightMgr__RemoveAlmostAllLights(void);
bool		LightMgr__EnableLight(t_Handle Handle, bool bEnable);

/* for cut scene support */

void		LightMgr__SaveAmbientLight();
void		LightMgr__RestoreAmbientLight();
void		LightMgr__SaveDirectionalLights();
void		LightMgr__RestoreDirectionalLights();

void		LightMgr__RenderSpecialEffects(CScene *pScene);

ts_Light *LightMgr__GetSun(void);



#endif // __LIGHTMGR_H

