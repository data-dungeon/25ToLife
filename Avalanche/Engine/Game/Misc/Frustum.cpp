/*************************************/
/*                                   */
/*   Frustum.cpp                     */
/*   originally by joe barnes        */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   view frustum processing         */
/*                                   */
/*************************************/

/* routines to handle the view frustum and culling tests. the culling routines are called a lot, and need to be fast */

/******************* includes ****************************/

#include "Game/GamePCH.h"





/******************* defines *****************************/

/******************* macros ******************************/

/* for Frustum__IntersectsWorldSphere */

#define SPHERE_CHECK_WITH_CLIP(nIndex, Planes, Label)																				\
																																					\
	{																																				\
		fDistance = Planes[nIndex].Dist( pSphere->Center );																		\
		if (fDistance > fRadius)				/* more than <radius> outside of plane? */									\
		{																																			\
			SceneMgr::GetStats().IncSphereCull( 1 );																\
			return(SPHERE_OUT_OF_VIEW);																									\
		}																																			\
		if (fDistance > fNegRadius)			/* check if center is within radius of plane */								\
		{																																			\
			if (fDistance > 0.0f)				/* outside of plane? */																\
				bInsideAllPlanes = FALSE;		/* can't guarantee it's visible without box check */						\
			*pu8Clipped = nIndex + 1;			/* within radius of plane so it clips, "+ 1" so it's non-zero */		\
			goto Label;								/* no need for further clip checks */											\
		}																																			\
	}

#define SPHERE_CHECK_NO_CLIP(nIndex, Planes)																							\
																																					\
	{																																				\
		fDistance = Planes[nIndex].Dist( pSphere->Center );																		\
		if (fDistance > fRadius)				/* more than <radius> outside of plane? */									\
		{																																			\
			SceneMgr::GetStats().IncSphereCull( 1 );																\
			return(SPHERE_OUT_OF_VIEW);																									\
		}																																			\
		if (fDistance > 0.0f)					/* outside of plane? */																\
			bInsideAllPlanes = FALSE;			/* can't guarantee it's visible without box check */						\
	}

/* a sphere check that just checks whether the sphere is outside the plane */

#define SPHERE_OUTSIDE_PLANE(nIndex, Planes)																							\
	Planes[nIndex].IsFacing( *pSphere )

/* for Frustum__IntersectsWorldBox */

#define BOX_CHECK_WITH_CLIP(nIndex, Planes, Label)																					\
																																					\
	{																																				\
		i = 0;																																	\
		j = 0;																																	\
		while ((i != 3) && (j < 8))																										\
		{																																			\
		if ( Planes[nIndex].IsFacing( WorkVector[j] ) )                                                          \
				i |= 1;	/* outside of clip plane (outside frustum) */															\
			else																																	\
				i |= 2;	/* inside clip plane (inside frustum) */																	\
			j++;																																	\
		}																																			\
		if (i == 1)		/* no points inside frustum-- cull */																		\
		{																																			\
			SceneMgr::GetStats().IncBoxCull( 1 );																	\
			return(FALSE);																														\
		}																																			\
		if (i == 3)		/* if both bits 1 and 2 set, bounding box had corners on both sides of the plane */			\
		{																																			\
			*pu8Clipped = nIndex + 1;	/* "+ 1" so it's non-zero */																\
			goto Label;																															\
		}																																			\
	}

#define BOX_CHECK_NO_CLIP(nIndex, Planes)																								\
																																					\
	{																																				\
		j = 0;																																	\
		while ((j < 8) && Planes[nIndex].IsFacing( WorkVector[j] ) )                                             \
			j++;																																	\
		if (j == 8)	/* all corners on outside of clip plane, entire box is too */											\
		{																																			\
			SceneMgr::GetStats().IncBoxCull( 1 );																	\
			return(FALSE);																														\
		}																																			\
	}

/******************* structures **************************/

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

/******************* local prototypes ********************/

/* these are called as part of Frustum__IntersectsWorldBounds, and shouldn't be called directly. there are simpler
versions available for outside use that don't check fog or clip status-- just the intersection result. the issues:

	1.	these routines don't do a simple bounding box intersection check between passed box and view frustum box--
		they expect that to have been done already by the versions available to the public. so they can be slow
	2. fog status is not set by the static version of Frustum__IntersectsWorldBox

*/

int Frustum__IntersectsWorldSphere(
		const ts_Frustum *pFrustum,
		#ifdef PS2
		const ts_Frustum *pGuardBandFrustum,
		#endif //PS2
		const ts_BoundingSphere *pSphere,
		u8 *pu8Clipped,
		bool *pbFogged);
bool Frustum__IntersectsWorldBox2(
		const ts_Frustum *pFrustum,
		#ifdef PS2
		const ts_Frustum *pGuardBandFrustum,
		#endif //PS2
		const ts_BoundingBox *pBox,
		u8 *pu8Clipped);
bool Standard_Frustum__IntersectsWorldBounds(
		const ts_BoundingSphere *pSphere,
		const ts_BoundingBox *pBox,
		u8 *pu8Clipped,
		bool *pbFogged,
		bool bNoFarClip /*= false*/);
int Frustum__IntersectsWorldSphere(
		const ts_BoundingSphere *pSphere,
		u8 *pu8Clipped,
		bool *pbFogged);
bool Frustum__IntersectsWorldBox(
		const ts_BoundingBox *pBox,
		u8 *pu8Clipped);

static const ts_Frustum* gCurrFrustum;
static const ts_Frustum* gCurrGuardBandFrustum;

/******************* functions ***************************/

/* calculate the 6 planes that enclose the frustum in "frustum" space. each plane is represented by a unit normal and
a "distance" to the origin. distance is negative if normal points away from origin, positive if toward. normal and distance
are the A, B, C, D values in the plane equation: Ax + By + Cz + D = 0. plug any point into plane equation to get the
distance of that point from the plane. a positive "distance" means that the point is on the "outside" of the plane (the
side to which the normal points), negative means "inside" the plane */

void Frustum__Build(ts_Frustum *pFrustum, float fNearClip, float fFarClip, bool bFogOn, float fFogStart, float fFogEnd, float fFieldOfViewYDegrees, float fAspectRatio)
{
	float fHalfViewXAngleRadians;
	float fHalfViewYAngleRadians = Math::Deg2Rad(fFieldOfViewYDegrees / 2.0f);

	/* near plane. camera looks down negative z-axis, so plane's normal is down positive z-axis (points toward origin) */

	pFrustum->Planes[NEAR_PLANE].Set( Vector3(0.0f, 0.0f, 1.0f), fNearClip );

	/* far plane. camera looks down negative z-axis, so plane's normal is also down negative z-axis */

	pFrustum->Planes[FAR_PLANE].Set( Vector3( 0.0f, 0.0f, -1.0f), -fFarClip );

	/* top plane, i think */
	
	pFrustum->Planes[TOP_PLANE].Set( Vector3( 0.0f,
												         Math::Sin( Math::HalfPi - fHalfViewYAngleRadians),
											 	         Math::Cos( Math::HalfPi - fHalfViewYAngleRadians) ),
												0.0f );

	/* bottom plane, i think */

	pFrustum->Planes[BOTTOM_PLANE].Set( Vector3( 0.0f,
													         -pFrustum->Planes[TOP_PLANE].Normal().y(),
													         pFrustum->Planes[TOP_PLANE].Normal().z() ),
													0.0f );

	/* right side plane, i think. you can't just multiply fovy by aspect ratio to get fovx-- for example, */
	/* if fovy is 45 degrees, and the x-to-y aspect ratio is 2, then the angle that includes twice as */
	/* many pixels at the same distance isn't 90. 45 = arctan(1), arctan(2) = 63.4 */

	fHalfViewXAngleRadians = Math::ArcTan(Math::Tan(fHalfViewYAngleRadians) * fAspectRatio);
	pFrustum->Planes[RIGHT_PLANE].Set( Vector3( Math::Sin( Math::HalfPi - fHalfViewXAngleRadians),
													        0.0f,
													        Math::Cos( Math::HalfPi - fHalfViewXAngleRadians) ),
												  0.0f );

	/* left side plane, i think */

	pFrustum->Planes[LEFT_PLANE].Set( Vector3( -pFrustum->Planes[RIGHT_PLANE].Normal().x(),
												          0.0f,
												          pFrustum->Planes[RIGHT_PLANE].Normal().z() ),
												 0.0f );

	/* far and near fog planes */

	Frustum__SetFog(pFrustum, bFogOn, fFogStart, fFogEnd);

	/* calculate corner points of frustum in camera space-- first points on the near plane */

	float fXTan = Math::Tan(fHalfViewYAngleRadians);
	float fYTan = Math::Tan(fHalfViewXAngleRadians);
	pFrustum->Corners[0].Set(fYTan * -fNearClip,	//tan = opp/adj, so opp = tan * adj
									 fXTan * -fNearClip,	//tan = opp/adj, so opp = tan * adj
									 -fNearClip);	// camera looks down negative z-axis, so z-value should be negative

	pFrustum->Corners[1].Set( -pFrustum->Corners[0].x(),
									  pFrustum->Corners[0].y(),
									  pFrustum->Corners[0].z());

	pFrustum->Corners[2].Set( -pFrustum->Corners[0].x(),
									  -pFrustum->Corners[0].y(),
									  pFrustum->Corners[0].z());

	pFrustum->Corners[3].Set( pFrustum->Corners[0].x(),
									  -pFrustum->Corners[0].y(),
									  pFrustum->Corners[0].z());

	/* points on the far clip plane, or on the far fog plane if valid (since things are culled if they are past the */
	/* far fog plane) */

	int nIndex = pFrustum->bFarFogPlaneValid ? FAR_FOG_PLANE : FAR_PLANE;
	pFrustum->Corners[4].Set(fYTan * pFrustum->Planes[nIndex].D(),	//tan = opp/adj, so opp = tan * adj
									 fXTan * pFrustum->Planes[nIndex].D(),	//tan = opp/adj, so opp = tan * adj
									 pFrustum->Planes[nIndex].D());	// camera looks down negative z-axis, so z-value should be negative
	pFrustum->Corners[5].Set(-pFrustum->Corners[4].x(),
									 pFrustum->Corners[4].y(),
									 pFrustum->Corners[4].z());
	pFrustum->Corners[6].Set( -pFrustum->Corners[4].x(),
									  -pFrustum->Corners[4].y(),
									  pFrustum->Corners[4].z());
	pFrustum->Corners[7].Set( pFrustum->Corners[4].x(),
									  -pFrustum->Corners[4].y(),
									  pFrustum->Corners[4].z());

	/* points on the very far clip plane */

	pFrustum->NoFarClipCorners[0].Set(fYTan * -10000.0f,	//tan = opp/adj, so opp = tan * adj
												 fXTan * -10000.0f,	//tan = opp/adj, so opp = tan * adj
												 -10000.0f);	// camera looks down negative z-axis, so z-value should be negative
	pFrustum->NoFarClipCorners[1].Set( -pFrustum->NoFarClipCorners[0].x(),
												  pFrustum->NoFarClipCorners[0].y(),
												  pFrustum->NoFarClipCorners[0].z());
	pFrustum->NoFarClipCorners[2].Set( -pFrustum->NoFarClipCorners[0].x(),
												  -pFrustum->NoFarClipCorners[0].y(),
												  pFrustum->NoFarClipCorners[0].z());
	pFrustum->NoFarClipCorners[3].Set( pFrustum->NoFarClipCorners[0].x(),
												  -pFrustum->NoFarClipCorners[0].y(),
												  pFrustum->NoFarClipCorners[0].z());
}

/* set up the fog planes in frustum */

void Frustum__SetFog(ts_Frustum *pFrustum, bool bFogOn, float fFogStart, float fFogEnd)
{
	if (bFogOn)	// don't check RenderState__Get(RS_FOG) because not all frustums care about fog
	{
		float fFarClip = -pFrustum->Planes[FAR_PLANE].D();
		if (fFogEnd < fFarClip)	// far fog plane only valid if it is closer than far clip
		{
			pFrustum->Planes[FAR_FOG_PLANE].Set( Vector3( 0.0f, 0.0f, -1.0f ), -fFogEnd );
			pFrustum->bFarFogPlaneValid = TRUE;
		}
		else
			pFrustum->bFarFogPlaneValid = FALSE;

		/* when fog is on, near fog plane is always valid since it is used to determine whether */
		/* instance needs to be rendered through the fog path. don't really need this unless */
		/* CHECK_FOG_STATUS is defined, but the cost of setting it up is low, and it might be useful */
		/* elsewhere */

		pFrustum->Planes[NEAR_FOG_PLANE].Set( Vector3( 0.0f, 0.0f, 1.0f ), fFogStart );
		pFrustum->bNearFogPlaneValid = TRUE;
	}
	else
	{
		pFrustum->bFarFogPlaneValid = FALSE;
		pFrustum->bNearFogPlaneValid = FALSE;
	}
}

const ts_Frustum * Frustum__GetCurrentFrustum( void )
{
	return gCurrFrustum;
}

/* transform the camera space view volume to world space for use in culling groups/instances. typically called once
per frame per scene/camera */

void Frustum__TransformToWorld(ts_Frustum *pFrustum, const Graphics4x4 *pmFrustumToWorld)
{
	int i;
	Vector3 PointOnPlane;

	/* transform each plane according to camera-to-world matrix */

	for (i = 0; i < FRUSTUM_PLANES; i++)
	{
		// calculate a point on the current plane, transform it
		PointOnPlane = pFrustum->Planes[i].Normal() * -pFrustum->Planes[i].D();
		PointOnPlane = PointOnPlane * *pmFrustumToWorld;

		// build the new plane
		Vector3 newNormal;
		newNormal.FastBasisTransform( pFrustum->Planes[ i ].Normal(), *pmFrustumToWorld );
		pFrustum->WorldPlanes[ i ].Set( newNormal, PointOnPlane );
	}

	/* calculate world-space bounding box for view frustum by transforming frustum corner points into world space */
	/* and calculating bounding box that includes them */

	for (i = 0; i < 8; i++)
		WorkVector[i] = pFrustum->Corners[i] * *pmFrustumToWorld;
	BoundingBox__Clear(&pFrustum->NormalWorldBoundingBox);
	BoundingBox__IncludeCount(&pFrustum->NormalWorldBoundingBox, WorkVector, 8);

	/* now repeat the calculation for the no-far-clip bounding box-- much bigger */

	for (i = 0; i < 4; i++)
		WorkVector[4 + i] = pFrustum->NoFarClipCorners[i] * *pmFrustumToWorld;
	BoundingBox__Clear(&pFrustum->NoFarClipWorldBoundingBox);
	BoundingBox__IncludeCount(&pFrustum->NoFarClipWorldBoundingBox, WorkVector, 8);

#if defined(PS2) && defined(SN)
	// TJC -  update the ps2 version of the planes
	Frustum__RebuildPS2(pFrustum);
#endif

#ifdef _DEBUG
	/* make sure someone sets this properly-- it needs to point to one of the bounding boxes */

	pFrustum->pWorldBoundingBox = NULL;
#endif //_DEBUG
}

/* check if passed bounding sphere intersects frustum in world space. for ps2, pSphere must be quad-word aligned 
so we can use vu0 */

int Frustum__IntersectsWorldSphere(
		const ts_Frustum *pFrustum,
#ifdef PS2
		const ts_Frustum *pGuardBandFrustum,
#endif //PS2
		const ts_BoundingSphere *pSphere,
		u8 *pu8Clipped,
		bool *pbFogged)
{
	register float fDistance;
	register float fRadius = pSphere->Radius;
	register float fNegRadius = -pSphere->Radius;
	register bool bInsideAllPlanes;

	/* first do a no-clip check against the far fog plane, if valid. objects outside the far fog plane */
	/* get tossed (since they're completely fogged out). this is to prevent things like trees from having */
	/* to be clipped against the far plane, since the clipped rendering path is much slower than the */
	/* non-clipped path-- the far fog plane should be enough in front of the far clip plane that common */
	/* smaller objects (like trees) get culled before reaching the far clip plane */

	if ((pFrustum->bCheckFarFogPlane) && (SPHERE_OUTSIDE_PLANE(FAR_FOG_PLANE, pFrustum->WorldPlanes)))
	{
		SceneMgr::GetStats().IncSphereCull( 1 );
		return(SPHERE_OUT_OF_VIEW);
	}

	/* if sphere center more than radius distance outside any clip plane, sphere is outside frustum. */
	/* if center is inside all clip planes, sphere is definitely in view. a positive "distance" is outside the */
	/* clip plane, negative is inside */

	bInsideAllPlanes = TRUE;

	/* interested in clip status? */

	if (pu8Clipped == NULL)
		goto CheckFarNoClip;	// skip clip checks

	/* run checks with clipping status check. if sphere is found to be clipped, switch to no-clip check */
	/*** do far plane check first. if we intersect far plane, we assume we are far enough away ***/
	/*** that we can run through cull microcode (where applicable) instead of clip microcode   ***/

	if (pFrustum->u8State == FRUSTUM_STATE_NORMAL)
		SPHERE_CHECK_WITH_CLIP(FAR_PLANE, pFrustum->WorldPlanes, CheckNearNoClip);
	SPHERE_CHECK_WITH_CLIP(NEAR_PLANE, pFrustum->WorldPlanes, CheckTopNoClip);
	SPHERE_CHECK_WITH_CLIP(TOP_PLANE, pFrustum->WorldPlanes, CheckBottomNoClip);
	SPHERE_CHECK_WITH_CLIP(BOTTOM_PLANE, pFrustum->WorldPlanes, CheckRightNoClip);
	SPHERE_CHECK_WITH_CLIP(RIGHT_PLANE, pFrustum->WorldPlanes, CheckLeftNoClip);
	SPHERE_CHECK_WITH_CLIP(LEFT_PLANE, pFrustum->WorldPlanes, Clipped);

	/* made it through all checks without finding a clip situation */

	*pu8Clipped = 0;

	/* interested in fog status? */

#ifdef CHECK_FOG_STATUS
	if ((pbFogged) && (pFrustum->bNearFogPlaneValid))
		*pbFogged = (SPHERE_OUTSIDE_PLANE(NEAR_FOG_PLANE, pFrustum->WorldPlanes) == FALSE);	// not fogged if sphere is completely on camera side of near fog plane
#endif //CHECK_FOG_STATUS

	/* sphere is completely inside frustum */

	return(SPHERE_IN_VIEW);

	/* run checks without looking at clipping status. this is slightly simpler than clip checks, so */
	/* once we find the sphere clipped against any plane, we jump into these checks for performance */

CheckFarNoClip:
	if (pFrustum->u8State == FRUSTUM_STATE_NORMAL)
		SPHERE_CHECK_NO_CLIP(FAR_PLANE, pFrustum->WorldPlanes);
CheckNearNoClip:
	SPHERE_CHECK_NO_CLIP(NEAR_PLANE, pFrustum->WorldPlanes);
CheckTopNoClip:
	SPHERE_CHECK_NO_CLIP(TOP_PLANE, pFrustum->WorldPlanes);
CheckBottomNoClip:
	SPHERE_CHECK_NO_CLIP(BOTTOM_PLANE, pFrustum->WorldPlanes);
CheckRightNoClip:
	SPHERE_CHECK_NO_CLIP(RIGHT_PLANE, pFrustum->WorldPlanes);
CheckLeftNoClip:
	SPHERE_CHECK_NO_CLIP(LEFT_PLANE, pFrustum->WorldPlanes);
Clipped:

	/* interested in fog status? */

#ifdef CHECK_FOG_STATUS
	if ((pbFogged) && (pFrustum->bNearFogPlaneValid))
		*pbFogged = (SPHERE_OUTSIDE_PLANE(NEAR_FOG_PLANE, pFrustum->WorldPlanes) == FALSE);	// not fogged if sphere is completely on camera side of near fog plane
#endif //CHECK_FOG_STATUS

#ifdef PS2
	/* on ps2, the unclipped rendering path is much faster than the clipped path. if the bounding sphere is within */
	/* the guard band frustum (which is much wider than the view frustum) then it goes through the unclipped path. */
	/* so check against guard frustum here */

	if (pu8Clipped)	// only if we are interested in clip status */
	{
		/* guard band near and far planes are same as view frustum. if clipped by either, then clip status won't change */

		ASSERT(*pu8Clipped);	// should always be non-0 if we get here
		if ((*pu8Clipped == NEAR_PLANE + 1) || (*pu8Clipped == FAR_PLANE + 1))	// clipped value is plane that clipped + 1
			return(TRUE);	// render it

		/* check against guard band frustum side planes. if box intersects any of side planes */

		ASSERT(*pu8Clipped >= TOP_PLANE + 1);	// must have been clipped against one of view frustum side planes. far plane never sets clipped status
		SPHERE_CHECK_WITH_CLIP(TOP_PLANE, pGuardBandFrustum->WorldPlanes, ClippedReturn);
		SPHERE_CHECK_WITH_CLIP(BOTTOM_PLANE, pGuardBandFrustum->WorldPlanes, ClippedReturn);
		SPHERE_CHECK_WITH_CLIP(RIGHT_PLANE, pGuardBandFrustum->WorldPlanes, ClippedReturn);
		SPHERE_CHECK_WITH_CLIP(LEFT_PLANE, pGuardBandFrustum->WorldPlanes, ClippedReturn);

		/* made it through all checks without finding a clip situation, so sphere is completely within */
		/* guard band frustum, so it goes through unclipped rendering path (even though it was clipped */
		/* by view frustum) */

		*pu8Clipped = 0;
		return(SPHERE_IN_VIEW);
	}

ClippedReturn:
#endif //PS2

	/* sphere is at least partially inside frustum. if we are interested in clip status, getting here means */
	/* that sphere is clipped */

	return(bInsideAllPlanes ? SPHERE_IN_VIEW : SPHERE_MAYBE_IN_VIEW);
}

/* check if passed bounding box intersects frustum in world space. this routine does NOT check fogged status.
the logic is that a call to Frustum__IntersectsWorldBounds that calls for the fogged status to be checked goes
through Frustum__IntersectsWorldSphere first, which will set the fog status. refining the fog status by
checking the bounding box against the near fog plane is not as important as refining the clip status, so
we won't */

bool Frustum__IntersectsWorldBox2(
		const ts_Frustum *pFrustum,
#ifdef PS2
		const ts_Frustum *pGuardBandFrustum,
#endif //PS2
		const ts_BoundingBox *pBox,
		u8 *pu8Clipped)
{
	int i;
	int j;

	/* get the bounding boxes 8 corner points */

	WorkVector[0] = pBox->Min;
	
	WorkVector[1] = pBox->Min;
	WorkVector[1].z(pBox->Max.z());
	
	WorkVector[2] = pBox->Min;
	WorkVector[2].y(pBox->Max.y());
	
	WorkVector[3] = pBox->Max;
	WorkVector[3].x(pBox->Min.x());
	
	WorkVector[4] = pBox->Min;
	WorkVector[4].x(pBox->Max.x());
	
	WorkVector[5] = pBox->Max;
	WorkVector[5].y(pBox->Min.y());
	
	WorkVector[6] = pBox->Max;
	WorkVector[6].z(pBox->Min.z());
	
	WorkVector[7] = pBox->Max;

	/* first do a no-clip check against the far fog plane, if valid. objects outside the far fog plane */
	/* get tossed (since they're completely fogged out). this is to prevent things like trees from having */
	/* to be clipped against the far plane, since the clipped rendering path is much slower than the */
	/* non-clipped path-- the far fog plane should be enough in front of the far clip plane that common */
	/* smaller objects (like trees) get culled before reaching the far clip plane */

	if (pFrustum->bCheckFarFogPlane)
		BOX_CHECK_NO_CLIP(FAR_FOG_PLANE, pFrustum->WorldPlanes);

	/* interested in clip status? */

	if (pu8Clipped == NULL)
		goto CheckFarNoClip;	// skip clip checks

	/* run checks with clipping status check. if box is found to be clipped, switch to no-clip check */
	/*** do far plane check first. if we intersect far plane, we assume we are far enough away ***/
	/*** that we can run through cull microcode (where applicable) instead of clip microcode   ***/
 
	if (pFrustum->u8State == FRUSTUM_STATE_NORMAL)
		BOX_CHECK_WITH_CLIP(FAR_PLANE, pFrustum->WorldPlanes, CheckNearNoClip);
	BOX_CHECK_WITH_CLIP(NEAR_PLANE, pFrustum->WorldPlanes, CheckTopNoClip);
	BOX_CHECK_WITH_CLIP(TOP_PLANE, pFrustum->WorldPlanes, CheckBottomNoClip);
	BOX_CHECK_WITH_CLIP(BOTTOM_PLANE, pFrustum->WorldPlanes, CheckRightNoClip);
	BOX_CHECK_WITH_CLIP(RIGHT_PLANE, pFrustum->WorldPlanes, CheckLeftNoClip);
	BOX_CHECK_WITH_CLIP(LEFT_PLANE, pFrustum->WorldPlanes, Clipped);

	/* made it through all checks without finding a clip situation */

	*pu8Clipped = 0;

	/* box is completely inside frustum */

	return(TRUE);

	/* run checks without looking at clipping status. this is slightly simpler than clip checks, so */
	/* once we find the box clipped against any plane, we jump into these checks for performance */

CheckFarNoClip:
	if (pFrustum->u8State == FRUSTUM_STATE_NORMAL)
		BOX_CHECK_NO_CLIP(FAR_PLANE, pFrustum->WorldPlanes);
CheckNearNoClip:
	BOX_CHECK_NO_CLIP(NEAR_PLANE, pFrustum->WorldPlanes);
CheckTopNoClip:
	BOX_CHECK_NO_CLIP(TOP_PLANE, pFrustum->WorldPlanes);
CheckBottomNoClip:
	BOX_CHECK_NO_CLIP(BOTTOM_PLANE, pFrustum->WorldPlanes);
CheckRightNoClip:
	BOX_CHECK_NO_CLIP(RIGHT_PLANE, pFrustum->WorldPlanes);
CheckLeftNoClip:
	BOX_CHECK_NO_CLIP(LEFT_PLANE, pFrustum->WorldPlanes);
Clipped:

#ifdef PS2
	/* on ps2, the unclipped rendering path is much faster than the clipped path. if the bounding box is within */
	/* the guard band frustum (which is much wider than the view frustum) then it goes through the unclipped path. */
	/* so check against guard frustum here */

	if (pu8Clipped)	// only if we are interested in clip status */
	{
		/* guard band near and far planes are same as view frustum. if clipped by either, then clip status won't change */

		ASSERT(*pu8Clipped);	// should always be non-0 if we get here
		if ((*pu8Clipped == NEAR_PLANE + 1) || (*pu8Clipped == FAR_PLANE + 1))	// clipped value is plane that clipped + 1
			return(TRUE);	// render it

		/* check against guard band frustum side planes. if box intersects any of side planes */

		ASSERT(*pu8Clipped >= TOP_PLANE + 1);	// must have been clipped against one of view frustum side planes
		BOX_CHECK_WITH_CLIP(TOP_PLANE, pGuardBandFrustum->WorldPlanes, ClippedReturn);
		BOX_CHECK_WITH_CLIP(BOTTOM_PLANE, pGuardBandFrustum->WorldPlanes, ClippedReturn);
		BOX_CHECK_WITH_CLIP(RIGHT_PLANE, pGuardBandFrustum->WorldPlanes, ClippedReturn);
		BOX_CHECK_WITH_CLIP(LEFT_PLANE, pGuardBandFrustum->WorldPlanes, ClippedReturn);

		/* made it through all checks without finding a clip situation, so box is completely within */
		/* guard band frustum, so it goes through unclipped rendering path (even though it was clipped */
		/* by view frustum) */

		*pu8Clipped = 0;
		return(TRUE);
	}

ClippedReturn:
#endif //PS2

	/* box is within frustum. if we are interested in clip status, getting here means */
	/* that box is clipped */

	return(TRUE);
}

/* simple version available to outside callers-- just the intersection result, no clip or fog status */

bool Standard_Frustum__IntersectsWorldBox(const ts_BoundingBox *pBox, bool bNoFarClip /*= false*/)
{
	/* set up state-- normal or no-far-clip */

	if (bNoFarClip)
	{
		gCurrFrustum->u8State = FRUSTUM_STATE_NO_FAR_CLIP;
		gCurrFrustum->pWorldBoundingBox = &gCurrFrustum->NoFarClipWorldBoundingBox;
		gCurrFrustum->bCheckFarFogPlane = false;	// override far fog plane even if valid
	}
	else
	{
		gCurrFrustum->u8State = FRUSTUM_STATE_NORMAL;
		gCurrFrustum->pWorldBoundingBox = &gCurrFrustum->NormalWorldBoundingBox;
		gCurrFrustum->bCheckFarFogPlane = gCurrFrustum->bFarFogPlaneValid;
	}

	/* first check against view frustum's world-space bounding box */

	if (BoundingBox__IntersectBox(gCurrFrustum->pWorldBoundingBox, pBox) == FALSE)
		return(FALSE);

	/* check the box more carefully */

	return(Frustum__IntersectsWorldBox2(
				gCurrFrustum,
				#ifdef PS2
				NULL,	// don't worry about guard band frustum unless interested in clipping status
				#endif //PS2
				pBox,
				(u8 *) NULL));	// call correct overloaded function
}

/* simple version available to outside callers-- just the intersection result, no clip or fog status */

bool Standard_Frustum__IntersectsWorldSphere(const ts_BoundingSphere *pSphere, bool bNoFarClip /*= false*/)
{
	/* set up state-- normal or no-far-clip */

	if (bNoFarClip)
	{
		gCurrFrustum->u8State = FRUSTUM_STATE_NO_FAR_CLIP;
		gCurrFrustum->pWorldBoundingBox = &gCurrFrustum->NoFarClipWorldBoundingBox;
		gCurrFrustum->bCheckFarFogPlane = false;	// override far fog plane even if valid
	}
	else
	{
		gCurrFrustum->u8State = FRUSTUM_STATE_NORMAL;
		gCurrFrustum->pWorldBoundingBox = &gCurrFrustum->NormalWorldBoundingBox;
		gCurrFrustum->bCheckFarFogPlane = gCurrFrustum->bFarFogPlaneValid;
	}

	/* first check against view frustum's world-space bounding box */

	if (BoundingBox__IntersectSphere(gCurrFrustum->pWorldBoundingBox, pSphere) == FALSE)
		return(FALSE);

	/* check the sphere more carefully */

	int nSphereResult = Frustum__IntersectsWorldSphere(
									gCurrFrustum,
									#ifdef PS2
									NULL,	// don't worry about guard band frustum unless interested in clipping status
									#endif //PS2
									pSphere,
									NULL,
									NULL);
	return(nSphereResult != SPHERE_OUT_OF_VIEW);	// have to consider SPHERE_MAYBE_IN_VIEW as intersection
}

#if defined(PS2) && defined(SN)
bool PS2_Frustum__IntersectsWorldSphere(const ts_BoundingSphere *pSphere, bool bNoFarClip /*= false*/)
{
	register u128 Sphere asm("vf18");

	Sphere = (Vector4(*(Vector4Packed*)pSphere)).vec();

	int ignoremask1 = 0xFF0F;
	int ignoremask2 = 0xFF1F;

	if(bNoFarClip){
		ignoremask1 |= 0x0040;
		ignoremask2 |= 0x0020;
	}
	if(!gCurrFrustum->bFarFogPlaneValid)
	{
		ignoremask2 |= 0x0020;
	}

	int outside;
	int fog;
	int clipon;
	int farclip;

	register u128 JunkRegisterToKeepOptimizerHappy asm("vf0");

	asm(
		".set noreorder										\n\t"
		"ctc2		%1, vi01										\n\t"
		"ctc2		%2, vi02										\n\t"
		: "=j" (JunkRegisterToKeepOptimizerHappy)
		: "r" (ignoremask1), "r" (ignoremask2));

	asm(
		"vcallms	VU0_Frustum__SphereIntersectsWorldBounds	\n\t"
		: "+j" (Sphere),
		  "+j" (JunkRegisterToKeepOptimizerHappy));

	asm(
		"cfc2.i	%0, vi01										\n\t"
		".set reorder											\n\t"
		: "=r" (outside)
		: "j" (JunkRegisterToKeepOptimizerHappy));

	// TJC - interpret the results
	return !outside;
}

bool PS2_Frustum__IntersectsWorldBox(const ts_BoundingBox *pBox, bool bNoFarClip /*= false*/)
{
	register u128 BoxMin asm("vf16");
	register u128 BoxMax asm("vf17");

	Vector3 vBoxMin = pBox->Min;
	Vector3 vBoxMax = pBox->Max;
	BoxMin = vBoxMin.vec();
	BoxMax = vBoxMax.vec();

	int ignoremask1 = 0xFF0F;
	int ignoremask2 = 0xFF1F;

	if(bNoFarClip){
		ignoremask1 |= 0x0040;
		ignoremask2 |= 0x0020;
	}
	if(!gCurrFrustum->bFarFogPlaneValid)
	{
		ignoremask2 |= 0x0020;
	}

	int outside;
	int fog;
	int clipon;
	int farclip;

	register u128 JunkRegisterToKeepOptimizerHappy asm("vf0");

	asm(
		".set noreorder										\n\t"
		"ctc2		%1, vi01										\n\t"
		"ctc2		%2, vi02										\n\t"
		: "=j" (JunkRegisterToKeepOptimizerHappy)
		: "r" (ignoremask1), "r" (ignoremask2));

	asm(
		"vcallms	VU0_Frustum__BoxIntersectsWorldBounds	\n\t"
		: "+j" (BoxMin), "+j" (BoxMax),
		  "+j" (JunkRegisterToKeepOptimizerHappy));

	asm(
		"cfc2.i	%0, vi01										\n\t"
		".set reorder											\n\t"
		: "=r" (outside)
		: "j" (JunkRegisterToKeepOptimizerHappy));

	// TJC - interpret the results
	return !outside;
}
#endif

/* simple version available to outside callers-- just the intersection result, no clip or fog status */
bool Frustum__IntersectsWorldSphere(const ts_BoundingSphere *pSphere, bool bNoFarClip /*= false*/)
{
//ned says: PS2_Frustum__IntersectsWorldSphere doesn't work 
#if defined(PS2) && defined(SN)
	return PS2_Frustum__IntersectsWorldSphere(pSphere, bNoFarClip);
#else
	return Standard_Frustum__IntersectsWorldSphere(pSphere, bNoFarClip);
#endif
}

/* simple version available to outside callers-- just the intersection result, no clip or fog status */
bool Frustum__IntersectsWorldBox(const ts_BoundingBox *pBox, bool bNoFarClip /*= false*/)
{
#if defined(PS2) && defined(SN)
	return PS2_Frustum__IntersectsWorldBox(pBox, bNoFarClip);
#else
	return Standard_Frustum__IntersectsWorldBox(pBox, bNoFarClip);
#endif
}

/* simple version available to outside callers-- just the intersection result, no clip or fog status */
bool Frustum__IntersectsWorldSphere(ts_Frustum *pFrustum, const ts_BoundingSphere *pSphere, bool bNoFarClip /*= false*/)
{
	/* set up state-- normal or no-far-clip */
	Frustum__SetupToCheckFrustum(pFrustum);

	return Frustum__IntersectsWorldSphere(pSphere, bNoFarClip);
}

/* simple version available to outside callers-- just the intersection result, no clip or fog status */
bool Frustum__IntersectsWorldBox(ts_Frustum *pFrustum, const ts_BoundingBox *pBox, bool bNoFarClip /*= false*/)
{
	/* set up state-- normal or no-far-clip */
	Frustum__SetupToCheckFrustum(pFrustum);

	return Frustum__IntersectsWorldBox(pBox, bNoFarClip);
}

// TJC - rebuild the ps2 specific info in the frustum
#if defined(PS2) && defined(SN)
void Frustum__RebuildPS2(ts_Frustum *pFrustum)
{
	pFrustum->PS2Planes[0][0].Set(
		pFrustum->WorldPlanes[NEAR_PLANE].Normal().X(),
		pFrustum->WorldPlanes[FAR_PLANE].Normal().X(),
		pFrustum->WorldPlanes[TOP_PLANE].Normal().X(),
		pFrustum->WorldPlanes[BOTTOM_PLANE].Normal().X());
	pFrustum->PS2Planes[0][1].Set(
		pFrustum->WorldPlanes[NEAR_PLANE].Normal().Y(),
		pFrustum->WorldPlanes[FAR_PLANE].Normal().Y(),
		pFrustum->WorldPlanes[TOP_PLANE].Normal().Y(),
		pFrustum->WorldPlanes[BOTTOM_PLANE].Normal().Y());
	pFrustum->PS2Planes[0][2].Set(
		pFrustum->WorldPlanes[NEAR_PLANE].Normal().Z(),
		pFrustum->WorldPlanes[FAR_PLANE].Normal().Z(),
		pFrustum->WorldPlanes[TOP_PLANE].Normal().Z(),
		pFrustum->WorldPlanes[BOTTOM_PLANE].Normal().Z());
	pFrustum->PS2Planes[0][3] = pFrustum->PS2Planes[0][0].Abs();
	pFrustum->PS2Planes[0][4] = pFrustum->PS2Planes[0][1].Abs();
	pFrustum->PS2Planes[0][5] = pFrustum->PS2Planes[0][2].Abs();
	pFrustum->PS2Planes[0][6].Set(
		pFrustum->WorldPlanes[NEAR_PLANE].D(),
		pFrustum->WorldPlanes[FAR_PLANE].D(),
		pFrustum->WorldPlanes[TOP_PLANE].D(),
		pFrustum->WorldPlanes[BOTTOM_PLANE].D());

	pFrustum->PS2Planes[1][0].Set(
		pFrustum->WorldPlanes[RIGHT_PLANE].Normal().X(),
		pFrustum->WorldPlanes[LEFT_PLANE].Normal().X(),
		pFrustum->WorldPlanes[FAR_FOG_PLANE].Normal().X(),
		pFrustum->WorldPlanes[NEAR_FOG_PLANE].Normal().X());
	pFrustum->PS2Planes[1][1].Set(
		pFrustum->WorldPlanes[RIGHT_PLANE].Normal().Y(),
		pFrustum->WorldPlanes[LEFT_PLANE].Normal().Y(),
		pFrustum->WorldPlanes[FAR_FOG_PLANE].Normal().Y(),
		pFrustum->WorldPlanes[NEAR_FOG_PLANE].Normal().Y());
	pFrustum->PS2Planes[1][2].Set(
		pFrustum->WorldPlanes[RIGHT_PLANE].Normal().Z(),
		pFrustum->WorldPlanes[LEFT_PLANE].Normal().Z(),
		pFrustum->WorldPlanes[FAR_FOG_PLANE].Normal().Z(),
		pFrustum->WorldPlanes[NEAR_FOG_PLANE].Normal().Z());
	pFrustum->PS2Planes[1][3] = pFrustum->PS2Planes[1][0].Abs();
	pFrustum->PS2Planes[1][4] = pFrustum->PS2Planes[1][1].Abs();
	pFrustum->PS2Planes[1][5] = pFrustum->PS2Planes[1][2].Abs();
	pFrustum->PS2Planes[1][6].Set(
		pFrustum->WorldPlanes[RIGHT_PLANE].D(),
		pFrustum->WorldPlanes[LEFT_PLANE].D(),
		pFrustum->WorldPlanes[FAR_FOG_PLANE].D(),
		pFrustum->WorldPlanes[NEAR_FOG_PLANE].D());
}


/***************************************************************************/
// PS2 Microcode declarations
/***************************************************************************/
extern "C"
{
	extern void VU0_Frustum__SetPlanes(void) __attribute__ ((section(".vutext")));
	extern void VU0_Frustum__IntersectsWorldBounds(void) __attribute__ ((section(".vutext")));
	extern void VU0_Frustum__BoxIntersectsWorldBounds(void) __attribute__ ((section(".vutext")));
	extern void VU0_Frustum__SphereIntersectsWorldBounds(void) __attribute__ ((section(".vutext")));
};

void Frustum__UpdateVUMem(const ts_Frustum *pFrustum, int offset)
{
	register u128 plane0 asm("vf16");
	register u128 plane1 asm("vf17");
	register u128 plane2 asm("vf18");
	register u128 plane3 asm("vf19");
	register u128 plane4 asm("vf20");
	register u128 plane5 asm("vf21");
	register u128 plane6 asm("vf22");
	int junkRegister;
	
	plane0 = pFrustum->PS2Planes[0][0].vec();
	plane1 = pFrustum->PS2Planes[0][1].vec();
	plane2 = pFrustum->PS2Planes[0][2].vec();
	plane3 = pFrustum->PS2Planes[0][3].vec();
	plane4 = pFrustum->PS2Planes[0][4].vec();
	plane5 = pFrustum->PS2Planes[0][5].vec();
	plane6 = pFrustum->PS2Planes[0][6].vec();

	asm(
		"ctc2 	%1, vi1							\n\t"
		"vcallms	VU0_Frustum__SetPlanes		\n\t"
		"cfc2.i	%0, vi1							\n\t"
		: "=r" (junkRegister)
		: "r" (offset), "j" (plane0), "j" (plane1), "j" (plane2), "j" (plane3), "j" (plane4), "j" (plane5), "j" (plane6));

	plane0 = pFrustum->PS2Planes[1][0].vec();
	plane1 = pFrustum->PS2Planes[1][1].vec();
	plane2 = pFrustum->PS2Planes[1][2].vec();
	plane3 = pFrustum->PS2Planes[1][3].vec();
	plane4 = pFrustum->PS2Planes[1][4].vec();
	plane5 = pFrustum->PS2Planes[1][5].vec();
	plane6 = pFrustum->PS2Planes[1][6].vec();

	asm(
		"ctc2		%1, vi1							\n\t"
		"vcallms	VU0_Frustum__SetPlanes		\n\t"
		"cfc2.i	%0, vi1							\n\t"
		: "+r" (junkRegister)
		: "r" (offset+7), "j" (plane0), "j" (plane1), "j" (plane2), "j" (plane3), "j" (plane4), "j" (plane5), "j" (plane6));

	// TJC -  make the optimizer not strip the entire function clean...
	asm("" : : "r" (junkRegister));
}

bool PS2_Frustum__IntersectsWorldBounds(
		const ts_BoundingSphere *pSphere,
		const ts_BoundingBox *pBox,
		u8 *pu8Clipped,
		bool *pbFogged,
		bool bNoFarClip /*= false*/)
{
	register u128 BoxMin asm("vf16");
	register u128 BoxMax asm("vf17");
	register u128 Sphere asm("vf18");

	BoxMin = (Vector3(pBox->Min)).vec();
	BoxMax = (Vector3(pBox->Max)).vec();
	Sphere = (Vector4(*(Vector4Packed*)pSphere)).vec();

	int ignoremask1 = 0xFF0F;
	int ignoremask2 = 0xFF1F;

	if(bNoFarClip){
		ignoremask1 |= 0x0040;
		ignoremask2 |= 0x0020;
	}
	if(!gCurrFrustum->bFarFogPlaneValid)
	{
		ignoremask2 |= 0x0020;
	}

	int outside;
	int fog;
	int clipon;
	int farclip;

	register u128 JunkRegisterToKeepOptimizerHappy asm("vf0");

	asm(
		".set noreorder										\n\t"
		"ctc2		%1, vi01										\n\t"
		"ctc2		%2, vi02										\n\t"
		"ctc2		%3, vi03										\n\t"
		"ctc2		%4, vi04										\n\t"
		: "=j" (JunkRegisterToKeepOptimizerHappy)
		: "r" (ignoremask1), "r" (ignoremask2), "r" (0xFF0F), "r" (0xFF3F));

	asm(
		"vcallms	VU0_Frustum__IntersectsWorldBounds	\n\t"
		: "+j" (BoxMin), "+j" (BoxMax), "+j" (Sphere),
		  "+j" (JunkRegisterToKeepOptimizerHappy));

	asm(
		"cfc2.i	%0, vi01										\n\t"
		"cfc2		%1, vi02										\n\t"
		"cfc2		%2, vi03										\n\t"
		"cfc2		%3, vi04										\n\t"
		".set reorder											\n\t"
		: "=r" (outside), "=r" (fog), "=r" (clipon), "=r" (farclip)
		: "j" (JunkRegisterToKeepOptimizerHappy));

	bool result;
	// TJC - interpret the results
	if(outside)
		return false;
	else
	{
		if(pbFogged)
		{
			*pbFogged = fog;
		}
		if(pu8Clipped)
		{
			if(clipon)
			{
				if(farclip)
					*pu8Clipped = FAR_PLANE+1;
				else
					*pu8Clipped = TOP_PLANE+1;
			}
			else
				*pu8Clipped = 0;
		}
		return true;
	}
}	
#endif // defined(PS2) && defined(SN)

bool Standard_Frustum__IntersectsWorldBounds(
		const ts_BoundingSphere *pSphere,
		const ts_BoundingBox *pBox,
		u8 *pu8Clipped,
		bool *pbFogged,
		bool bNoFarClip /*= false*/)
{
	int nSphereResult;

	/* set up state-- normal or no-far-clip */

	if (bNoFarClip)
	{
		gCurrFrustum->u8State = FRUSTUM_STATE_NO_FAR_CLIP;
		gCurrFrustum->pWorldBoundingBox = &gCurrFrustum->NoFarClipWorldBoundingBox;
		gCurrFrustum->bCheckFarFogPlane = false;	// override far fog plane even if valid
	}
	else
	{
		gCurrFrustum->u8State = FRUSTUM_STATE_NORMAL;
		gCurrFrustum->pWorldBoundingBox = &gCurrFrustum->NormalWorldBoundingBox;
		gCurrFrustum->bCheckFarFogPlane = gCurrFrustum->bFarFogPlaneValid;
	}

	/* first check against view frustum's world-space bounding box */

	if (BoundingBox__IntersectBox(gCurrFrustum->pWorldBoundingBox, pBox) == FALSE)
		return(FALSE);

#if defined(WIN32) || defined(GCN)
	/* we never care about clip status for pc */

	if (pu8Clipped)
		*pu8Clipped = FALSE;
	pu8Clipped = NULL;
#endif //WIN32

//xxx may want to copy passed frustum to a fast frustum for rest of testing... check performance

	/* then check passed sphere against view frustum's planes */

	if ((nSphereResult = Frustum__IntersectsWorldSphere(
									gCurrFrustum,
#ifdef PS2
									gCurrGuardBandFrustum,
#endif
									pSphere,
									pu8Clipped,
									pbFogged)) == SPHERE_OUT_OF_VIEW)
		return(FALSE);

	/* if we don't care about clipped status, things are pretty simple at this point */

	if (pu8Clipped == NULL)
	{
		/* if the center of the sphere is in the view frustum, we are good to go */

		if (nSphereResult == SPHERE_IN_VIEW)
			return(TRUE);

		/* sphere test was not definitive (center of sphere is outside frustum but sphere extends into frustum), */
		/* check passed box against view frustum's planes */

		return (Frustum__IntersectsWorldBox2(
					gCurrFrustum,
#ifdef PS2
					gCurrGuardBandFrustum,
#endif //PS2
					pBox,
					(u8 *) NULL));	// call correct overloaded function
	}

	/* if bounding sphere is in view with no clipping, we're set */

	if (*pu8Clipped == 0)
	{
		ASSERT(nSphereResult == SPHERE_IN_VIEW);
		return(TRUE);
	}

	/* ok, at this point we know that the bounding sphere intersects the view frustum. since we are interested in */
	/* clipping, we must want to refine the clipping status. the box check is tighter, so do that now */

	return (Frustum__IntersectsWorldBox2(
				gCurrFrustum,
#ifdef PS2
				gCurrGuardBandFrustum,
#endif //PS2
				pBox,
				pu8Clipped));
}

void Frustum__SetupToCheckFrustum(
	const ts_Frustum *pFrustum
#ifdef PS2
	, const ts_Frustum *pGuardBandFrustum
#endif
	)
{
#if defined(PS2) && defined(SN)

	Frustum__UpdateVUMem(pFrustum, 0);
	if(pGuardBandFrustum)
		Frustum__UpdateVUMem(pGuardBandFrustum, 14);
#endif
#ifdef PS2
	gCurrGuardBandFrustum = pGuardBandFrustum;
#endif

	gCurrFrustum = pFrustum;
}

bool Frustum__IntersectsWorldBounds(
		const ts_BoundingSphere *pSphere,
		const ts_BoundingBox *pBox,
		u8 *pu8Clipped,
		bool *pbFogged,
		bool bNoFarClip /*= false*/)
{
#if defined(PS2) && defined(SN)
	return PS2_Frustum__IntersectsWorldBounds(
		pSphere,pBox,
		pu8Clipped,pbFogged,bNoFarClip);
#else
	return Standard_Frustum__IntersectsWorldBounds(
			pSphere,pBox,
			pu8Clipped,pbFogged,bNoFarClip);
#endif
}

bool Frustum__IntersectsWorldBounds(
		ts_Frustum *pFrustum,
#ifdef PS2
		const ts_Frustum *pGuardBandFrustum,
#endif
		const ts_BoundingSphere *pSphere,
		const ts_BoundingBox *pBox,
		u8 *pu8Clipped,
		bool *pbFogged,
		bool bNoFarClip /*= false*/)
{
#ifdef PS2
	Frustum__SetupToCheckFrustum(pFrustum, pGuardBandFrustum);
#else
	Frustum__SetupToCheckFrustum(pFrustum);
#endif

	return Frustum__IntersectsWorldBounds(
			pSphere,pBox,
			pu8Clipped,pbFogged,bNoFarClip);
}

