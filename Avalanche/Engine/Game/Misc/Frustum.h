/*************************************/
/*                                   */
/*   g_Frustum.h                     */
/*   originally by joe barnes        */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   view frustum processing         */
/*                                   */
/*************************************/

#ifndef __G_FRUSTUM_H
#define __G_FRUSTUM_H

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include "Game/Misc/Bounds.h"				// for WorldBoundingBox field in ts_Frustum
#include "Math/plane.h"

/******************* forward declarations ****************/

/******************* defines *****************************/

/* return values from Frustum__IntersectsWorldSphere */

#define SPHERE_OUT_OF_VIEW				0
#define SPHERE_IN_VIEW					1
#define SPHERE_MAYBE_IN_VIEW			2


/* frustum test state */

#define FRUSTUM_STATE_NORMAL			0
#define FRUSTUM_STATE_NO_FAR_CLIP	1

/* frustum planes. a major goal of the far fog plane is to position it somewhat in front of the far clip plane. */
/* for example, on snowboarding we had boatloads of trees in the world. when they crossed the far clip plane, */
/* they went through the "clipped" rendering path, which is much slower than the unclipped path. by setting the */
/* far fog plane a bit more than a tree's top-down radius in front of the far clip plane, the trees will be */
/* culled before they need to be clipped */

#define NEAR_PLANE		0
#define FAR_PLANE			1
#define TOP_PLANE			2
#define BOTTOM_PLANE		3
#define RIGHT_PLANE		4
#define LEFT_PLANE		5
#define FAR_FOG_PLANE	6	///< parallel to the far plane, used to cull objects when they are completely fogged out
#define NEAR_FOG_PLANE	7	///< parallel to the near plane, used to determine if objects need to go through the "fogged" rendering path
#define FRUSTUM_PLANES	8

/******************* compile control *********************/

/* some platforms need to know fog status of instance before it is rendered so it can use the correct code path */
#ifdef PS2
#define CHECK_FOG_STATUS
#endif //PS2

/******************* macros ******************************/

#define FAR_PLANE_CLIPPED(u8Clipped) (u8Clipped == FAR_PLANE + 1)	///< "+ 1" is what frustum check uses

/******************* structures **************************/

/*! view frustum-- can either be in camera space (where the top of the pyramid is at (0, 0, 0)) or in world space.
in camera space, the bounding box is not typically calculated. "camera space" is used for convenience, it could
be called "light space" instead for a frustum that defines the extent of a light source for shadow casting */

struct ts_Frustum
{
	CPlane			Planes[FRUSTUM_PLANES];			///< the planes that make up the view frustum
	CPlane			WorldPlanes[FRUSTUM_PLANES];	///< the planes that make up the view frustum, transformed to world space
	Vector3			Corners[8];							///< the 8 corners of the truncated pyramid, in frustum space
	Vector3			NoFarClipCorners[4];				///< the 4 corners of the frustum at the very-far plane

	/*! the frustum has two states-- normal and no-far-clip (for instances marked no-far-clip). the no-far-clip state */
	/*! is implemented by having a second far plane very far away from the origin of the frustum. but having a bounding */
	/*! box for that state is still desirable, even though it may be big-- it will still exclude a bunch of stuff */
	/*! behind it */
	/// @{
	ts_BoundingBox	NormalWorldBoundingBox;
	ts_BoundingBox	NoFarClipWorldBoundingBox;
	/// @}

	/*! fog plane status */
	/// @{
	bool				bFarFogPlaneValid;				///< true if fog is on and far fog plane is nearer than far clip plane
	bool				bNearFogPlaneValid;				///< true if fog is on
	/// @}
	

	/*! for processing frustum in normal or no-far-clip state */
	/// @{
	mutable const ts_BoundingBox	*pWorldBoundingBox;		///< box that bounds the 8 corners, once they have been transformed to world space
	mutable u8					u8State;								///< normal or no-far-clip
	mutable bool				bCheckFarFogPlane;				///< want to ignore valid far fog plane for no-far-clip state
	/// @}

#ifdef PS2
	/// TJC - if you chnage the WorldPlanes above, you must rebuild this info
	/// by calling Frustum__RebuildPS2();  This is done in Frustum__TransformToWorld()
	/// TJC - ps2 specific format for storing the planes.  This is to
	/// allow faster checking using the VU.  This is 2 separate 7x4
	/// matrices.  The planes are stored like this:
	/// 
	/// planes[X][0] =  N0x,   N1x,   N2x,   N3x
	/// planes[X][1] =  N0y,   N1y,   N2y,   N3y
	/// planes[X][2] =  N0z,   N1z,   N2z,   N3z
	/// planes[X][3] = |N0x|, |N1x|, |N2x|, |N3x|
	/// planes[X][4] = |N0y|, |N1y|, |N2y|, |N3y|
	/// planes[X][5] = |N0z|, |N1z|, |N2z|, |N3z|
	/// planes[X][6] =  D0,    D1,    D2,    D3
	/// 
	/// where for X == 0, plane# 0 == Near
	///							plane# 1 == Far
	///							plane# 2 == Top
	///							plane# 3 == Bottom
	/// 
	/// and for X == 1,	plane# 0 == Right
	///							plane# 1 == Left
	///							plane# 2 == Far Fog
	///							plane# 3 == Near Fog
	/// 
	Vector4 PS2Planes[2][7];
#endif // PS2
};

/******************* global variables ********************/

/******************* global prototypes *******************/

void Frustum__Build(ts_Frustum *pFrustum, float fNearClip, float fFarClip, bool bFogOn, float fFogStart, float fFogEnd, float fFieldOfViewYDegrees, float fAspectRatio);
void Frustum__SetFog(ts_Frustum *pFrustum, bool bFogOn, float fFogStart, float fFogEnd);
void Frustum__TransformToWorld(ts_Frustum *pFrustum, const Graphics4x4 *pmFrustumToWorld);
bool Frustum__IntersectsWorldBox(ts_Frustum *pFrustum, const ts_BoundingBox *pBox, bool bNoFarClip = false);
bool Frustum__IntersectsWorldSphere(ts_Frustum *pFrustum, const ts_BoundingSphere *pSphere, bool bNoFarClip = false);
bool Frustum__IntersectsWorldBounds(
		ts_Frustum *pFrustum,
		#ifdef PS2
		const ts_Frustum *pGuardBandFrustum,
		#endif //PS2
		const ts_BoundingSphere *pSphere,
		const ts_BoundingBox *pBox,
		u8 *pu8Clipped,
		bool *pbFogged,
		bool bNoFarClip = false);

const ts_Frustum * Frustum__GetCurrentFrustum( void );

/// TJC - This call works together with the
/// Frustum__IntersectsWorldBounds() that doesnt take a frustum
/// pointer to give multiple fast frustum checks against the same
/// frustum on the ps2.
///
/// @param pFrustum view frustum
/// @param pGuardBandFrustum guard band frustum on the ps2.
void Frustum__SetupToCheckFrustum(
	const ts_Frustum *pFrustum
#ifdef PS2
	, const ts_Frustum *pGuardBandFrustum = NULL
#endif
	);

/// TJC - This call works together with the
/// Frustum__SetupToCheckFrustum() to give multiple fast frustum
/// checks against the same frustum on the ps2.
///
/// @param pSphere sphere to check against
/// @param pBox box to check against
/// @param pu8Clipped store which plane was clipped
/// @param pbFogged store if fogging is required
/// @param bNoFarClip do we want to include the far plane in the clip checks
///
/// @return if any part of the sphere or box was in the frustum
bool Frustum__IntersectsWorldBounds(
		const ts_BoundingSphere *pSphere,
		const ts_BoundingBox *pBox,
		u8 *pu8Clipped,
		bool *pbFogged,
		bool bNoFarClip = false);

/// TJC - This call works together with the
/// Frustum__SetupToCheckFrustum() to give multiple fast frustum
/// checks against the same frustum on the ps2.
///
/// @param pBox box to check against
/// @param bNoFarClip do we want to include the far plane in the clip checks
///
/// @return if any part of the sphere or box was in the frustum
bool Frustum__IntersectsWorldBox(const ts_BoundingBox *pBox, bool bNoFarClip = false);

/// TJC - This call works together with the
/// Frustum__SetupToCheckFrustum() to give multiple fast frustum
/// checks against the same frustum on the ps2.
///
/// @param pSphere sphere to check against
/// @param bNoFarClip do we want to include the far plane in the clip checks
///
/// @return if any part of the sphere or box was in the frustum
bool Frustum__IntersectsWorldSphere(const ts_BoundingSphere *pSphere, bool bNoFarClip = false);

#ifdef PS2
/// This function rebuilds the PS2 optimized data for pFrustum
///
/// @param pFrustum The frustum to be rebuilt
///
void Frustum__RebuildPS2(ts_Frustum *pFrustum);
#endif

#endif __G_FRUSTUM_H

