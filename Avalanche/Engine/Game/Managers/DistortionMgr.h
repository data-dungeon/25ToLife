/***********************************/
/*                                 */
/* g_DistortionMgr.h               */
/* big juju prototype  05/24/99    */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* spacial distortion handling     */
/*                                 */
/***********************************/

#ifndef __G_DISTORTMGR_H
#define __G_DISTORTMGR_H

/******************* includes ****************************/

/* system includes */

/* engine includes */

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/******************* forward declarations ****************/

struct ts_Bounds;	// from g_Bounds.h

/******************* defines *****************************/

/* distortion types */

enum
{
	DISTORTION_POINT_FORCE = 0,
	DISTORTION_GLOBAL_WIND,
	DISTORTION_VORTEX_WIND,
	DISTORTION_WIND_GUST,
	DISTORTION_UPDRAFT,
	DISTORTION_FORCEFIELD,
	DISTORTION_RIPPLE,
	DISTORTION_POINT_MULTIPLIER,	// currently for normals
	DISTORTION_POINT_ROTATE,		// currently for normals

	MAX_DISTORTION_TYPES		// must be last
};
#define ALL_DISTORTION_TYPES			-1

/* flags (for ts_Distortion.dh.u16Flags) */

//yuch-- not implemented correctly yet. separate lists? pass a flag when adding distortion?
#define DISTORTION_AFFECTS_PARTICLES		0x0001
#define FORCEFIELD_AFFECTS_PARTICLES		0x0002	// forcefield distortions affect a subset of particles
#define DISTORTION_AFFECTS_VERTICES			0x0004
#define DISTORTION_AFFECTS_NORMALS			0x0008
#define DISTORTION_AFFECTS_CHAINS			0x0010
#define DISTORTION_AFFECTS_MESHES			0x0020
#define DISTORTION_NO_TRANSFORM				0x0040	// for distortions already in camera space-- can't be used by particles
#define DISTORTION_NEEDS_ADVANCEMENT		0x0080	// for AdvanceDistortion use
#define DISTORTION_STATIONARY					0x0100	// distortion does not move (applies to distortions that have a position)
#define DISTORTION_ENABLED						0x0200	// distortion is active

#define DISTORTION_COMPATIBILITY_FLAGS		(DISTORTION_AFFECTS_VERTICES| DISTORTION_AFFECTS_NORMALS | DISTORTION_AFFECTS_PARTICLES | FORCEFIELD_AFFECTS_PARTICLES | DISTORTION_AFFECTS_CHAINS | DISTORTION_AFFECTS_MESHES)

/* some distortions are not allowed to transform into camera space-- they must be in world space */

#define DISTORTION_CANNOT_TRANSFORM			(DISTORTION_AFFECTS_PARTICLES | FORCEFIELD_AFFECTS_PARTICLES | DISTORTION_AFFECTS_CHAINS | DISTORTION_AFFECTS_MESHES)

/* default value for distortion group */

#define DISTORTION_DEFAULT_GROUP	0

/******************* macros ******************************/

/******************* structures **************************/

/******************* global variables ********************/

#ifndef CONSUMER_BUILD
extern char *DistortionNames[MAX_DISTORTION_TYPES];
#endif //CONSUMER_BUILD

/******************* global prototypes *******************/

int DistortionMgr__Init(int nMaxDistortions);
void DistortionMgr__Reset(void);
void DistortionMgr__Shutdown(void);
int DistortionMgr__GetDistortionCountByType(int nType);
void DistortionMgr__KillAllDistortions(void);
void DistortionMgr__KillDistortionsByType(int nType);
void DistortionMgr__KillGroup(u16 u16Group);
u16 DistortionMgr__RequestReservedGroupID(void);
void DistortionMgr__ApplyDistortions(Vector3 *pvPos, Vector3 *pvDest, int nCompatibilityFlags);
t_Handle DistortionMgr__AddPointForce(
	Vector3 *vPos,
	Vector3 *vVelocity,
	Vector3 *vAcceleration,
	Vector3 *vStrength,
	float Lifetime,
	u16 u16DistortionFlags,
	u8 u8Flags,
	u16 u16Group = DISTORTION_DEFAULT_GROUP);
t_Handle DistortionMgr__AddGlobalWind(
	Vector3 *vStrength,
	float Randomness,
	float SinAngleStart,
	float SinAngleInc,
	float SinMin,
	float SinMax,
	float Damping,
	float Lifetime,
	u16 u16DistortionFlags,
	u8 u8Flags,
	u16 u16Group = DISTORTION_DEFAULT_GROUP);
t_Handle DistortionMgr__AddVortexWind(
	Vector3 *vPos,
	float StartRadius,
	float Range,
	float RadiusVelocity,
	float RadiusAcceleration,
	float Amplitude,
	float Damping,
	float Lifetime,
	u16 u16DistortionFlags,
	u8 u8Flags,
	u16 u16Group = DISTORTION_DEFAULT_GROUP);
t_Handle DistortionMgr__AddWindGust(
	Vector3 *vPos,
	Vector3 *vVelocity,
	Vector3 *vAcceleration,
	Vector3 *vStrength,
	float Radius,
	float Delay,
	float Lifetime,
	float Deathtime,
	u16 u16DistortionFlags,
	u8 u8Flags,
	u16 u16Group = DISTORTION_DEFAULT_GROUP);
t_Handle DistortionMgr__AddUpdraft(
	Vector3 *vPos,
	float fYVelocity,
	float Height,
	float Radius,
	float Lifetime,
	u16 u16DistortionFlags,
	u8 u8Flags,
	u16 u16Group = DISTORTION_DEFAULT_GROUP);
t_Handle DistortionMgr__AddForcefield(
	Vector3 *pvPos,
	float Radius,
	float Lifetime,
	u16 u16DistortionFlags,
	u8 u8Flags,
	u16 u16Group = DISTORTION_DEFAULT_GROUP);
t_Handle DistortionMgr__AddRipple(
	Vector3 *vPos,
	float StartRadius,
	float Range,
	float RadiusVelocity,
	float RadiusAcceleration,
	float Amplitude,
	float Damping,
	float Lifetime,
	u16 u16DistortionFlags,
	u8 u8Flags,
	u16 u16Group = DISTORTION_DEFAULT_GROUP);
t_Handle DistortionMgr__AddPointMultiplier(
	Vector3 *vPos,
	Vector3 *vVelocity,
	Vector3 *vAcceleration,
	float MultiplierAtCenter,
	float MultiplierAtRadius,
	float Radius,
	float Lifetime,
	u16 u16DistortionFlags,
	u8 u8Flags,
	u16 u16Group = DISTORTION_DEFAULT_GROUP);
t_Handle DistortionMgr__AddPointRotate(
	Vector3 *vPos,
	Vector3 *vVelocity,
	Vector3 *vAcceleration,
	Vector3 *vRotate,
	float Radius,
	float Lifetime,
	u16 u16DistortionFlags,
	u8 u8Flags,
	u16 u16Group = DISTORTION_DEFAULT_GROUP);
void DistortionMgr__Advance(void);
int DistortionMgr__BuildApplyList(ts_Bounds *pBounds, int nCompatibilityFlags);
void DistortionMgr__TransformDistortions(Graphics4x4 *pmTransform, Graphics4x4 *pmTransformRotationOnly);
Vector3PackedData* DistortionMgr__GetGlobalWindStrength();
Vector3PackedData* DistortionMgr__GetGlobalWindBaseStrength();

/* distortion routines */

void Distortion__Kill(t_Handle DistortionHandle);
void Distortion__Enable(t_Handle DistortionHandle, bool bEnable);

void Distortion__ToggleDistortion(void);

//yuch-- temporary. or should be generalized to a distortion emitter, somehow
void WindGustEmitters__Add(Vector3 *pvPos, Vector3 *pvVel, Vector3 *pvStrength, float fRadius, float fDelay, float fLifetime, float fDeathtime = 0.0f, u8 u8Flags = 0);

#define WIND_GUST_FULL_STRENGTH				0x08	// does not alter strength based on distance from center


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __G_DISTORTMGR_H
