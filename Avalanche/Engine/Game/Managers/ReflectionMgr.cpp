/*************************************/
/*                                   */
/*   ReflectionMgr.c                 */
/*   new engine  12/3/01             */
/*   ned martin, avalanche software  */
/*   visual c++ 6.0                  */
/*   reflection manager              */
/*                                   */
/*************************************/

/* handle real-time reflections. each entry in the manager is an instance that reflects it's surroundings. each instance
has an associated plane equation that defines the reflection plane, and a rectangular area that is the reflector. the
reflection is rendered by taking the camera view of the scene that contains the reflector, and creating a view frustum
that looks through the reflecting rectangle from the back, rendering the visible scene on the front side of the reflector
as viewed through the frustum. this generated image is used as a texture to render onto the reflection rectangle. each
reflector requires a scene in the scene manager per shown-to-the-user scene in which it appears */

/******************* includes ****************************/

#include "Game/GamePCH.h"
#include "Math/plane.h"



/******************* defines *****************************/

/******************* macros ******************************/

#define REFLECTION_GET_RECORD(Handle)				&ReflectionMgr.pReflectionAlloc[HANDLE_INDEX(Handle)]

/******************* structures **************************/

/* reflection record */

struct ts_Reflection
{
	/* linked list pointer-- does not have to be first */

	ts_Reflection		*pNext;

	/* instance that contains the reflection geometry */

	CInstance			*pInst;

	/* texture to render into */
	Texture				*pTexture;

	/* center of reflection geometry-- where to point the camera when rendering reflection */

	Vector3				vReflectWorldLookAt;

	/* extent of mirror geometry, as seen when looking at mirror from back (e.g. from side that reflected */
	/* camera position will be on), with mirror "upright". top of mirror is determined by where uv.y == 0.0 is. */
	/* this is for determining how to set up the reflection frustum. the extents are used to calculate the */
	/* field of view in width and height. at this point, left and right are interchangeable, so are top and */
	/* bottom, since thsy are just used to calculate the angle between 2 vectors. they might come in handy */
	/* somewhere down the line, though, so calculate them correctly (just takes one more sign check of */
	/* reflection plane's normal's major axis) */

	Vector3				vMirrorTop;
	Vector3				vMirrorBottom;
	Vector3				vMirrorLeft;
	Vector3				vMirrorRight;

	/* plane that corresponds to reflective surface, in world coords */

	CPlane				ReflectPlane;

	/* camera for generating the reflection map */

	Graphics4x4			mWorldToCamera;	// camera is world camera reflected across reflection plane, calculated each frame

	/* size of texture to render into */

	int					nTextureWidth;
	int					nTextureHeight;
	
	/* assigned handle for invalid handle checking */

	t_Handle				Handle;

};

/* reflection manager is a list of registered reflection instances */

struct ts_ReflectionMgr
{
	/* pool of reflection records */

	ts_Reflection		*pReflectionAlloc;		// original alloc'd buffer
	ts_SLinkList		ReflectionFree;
	ts_SLinkList		ReflectionActive;
	u16					u16ID;						// for assigning handles
	int					nMaxReflections;			// record passed value for debugging, not really necessary otherwise

	/* status */

	bool					bActive;						// will not render registered reflection(s) if false

#ifndef CONSUMER_BUILD
	/* for debugging */

	ts_Reflection		*pCurrentReflection;		// current reflection being rendered
#endif //CONSUMER_BUILD
};

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

/* the reflection manager */

static ts_ReflectionMgr ReflectionMgr;

/******************* local prototypes ********************/

void Reflection__Create(ts_Reflection *pReflection, CScene *pScene);
ts_Reflection *ReflectionMgr__AllocReflectionRecord(void);

/******************* functions ***************************/

/* allocate records for reflection manager */

void ReflectionMgr__Init(int nMaxReflections)
{
	/* clear reflection manager */

	memset(&ReflectionMgr, 0, sizeof(ts_ReflectionMgr));

	/* set up pool of reflections */

	ASSERT(nMaxReflections < MAX_HANDLES);	// see definition of MAX_HANDLES
	ReflectionMgr.nMaxReflections = nMaxReflections;
	MEM_SET_ALLOC_NAME("ReflectionMgr reflection list");
	ReflectionMgr.pReflectionAlloc = (ts_Reflection *) memAlloc(ReflectionMgr.nMaxReflections * sizeof(ts_Reflection));
	SLinkList__Init(&ReflectionMgr.ReflectionFree, offsetof(ts_Reflection, pNext));
	SLinkList__LinkBlock(&ReflectionMgr.ReflectionFree, ReflectionMgr.pReflectionAlloc, ReflectionMgr.nMaxReflections, sizeof(ts_Reflection));
	SLinkList__Init(&ReflectionMgr.ReflectionActive, offsetof(ts_Reflection, pNext));	// no reflections registered yet

	/* clear all handles */

	ts_Reflection *pReflection = (ts_Reflection *) ReflectionMgr.ReflectionFree.pHead;
	while (pReflection)
	{
		pReflection->Handle = INVALID_HANDLE;
		pReflection = pReflection->pNext;
	}

	/* default active */

	ReflectionMgr.bActive = TRUE;
}

/* reset */

void ReflectionMgr__Reset(void)
{
	/* kill all active reflections */

	ReflectionMgr__KillAllReflections();

	/* clear all handles */

	ts_Reflection *pReflection = (ts_Reflection *) ReflectionMgr.ReflectionFree.pHead;
	while (pReflection)
	{
		pReflection->Handle = INVALID_HANDLE;
		pReflection = pReflection->pNext;
	}

	/* default active */

	ReflectionMgr.bActive = TRUE;
}

/* shutdown */

void ReflectionMgr__Shutdown(void)
{
	/* kill all active reflections */

	ReflectionMgr__KillAllReflections();

	/* shut down */

	if (ReflectionMgr.pReflectionAlloc)
		memFree(ReflectionMgr.pReflectionAlloc);
	memset(&ReflectionMgr, 0, sizeof(ts_ReflectionMgr));
}

/* enable/disable reflection manager */

void ReflectionMgr__Enable(void)
{
	ReflectionMgr.bActive = TRUE;
}
void ReflectionMgr__Disable(void)
{
	ReflectionMgr.bActive = FALSE;
}

/* confirm reflection manager is empty when you think it should be */

#ifndef CONSUMER_BUILD
bool ReflectionMgr__IsEmpty(void)
{
	if (SLinkList__Check(&ReflectionMgr.ReflectionFree) != (signed) ReflectionMgr.nMaxReflections)
		return(FALSE);

	/* ok, reflection manager is empty */

	return(TRUE);
}
#endif //CONSUMER_BUILD

/* register a new reflection instance */

t_Handle ReflectionMgr__RegisterReflection(CInstance *pInst, Texture* pTexture)
{
	/* reflection manager must have been initialized */

	ASSERT(ReflectionMgr.pReflectionAlloc);		// if this hits, ReflectionMgr__Init hasn't been called

	/* grab a record from free list */

	ts_Reflection *pReflection = ReflectionMgr__AllocReflectionRecord();
	if (pReflection == NULL)
		return(INVALID_HANDLE);

	/* fill in record */

	pReflection->pInst = pInst;
	pReflection->pTexture = pTexture;

	/* figure out plane that reflection surface lies in. by convention, mirrors are built so that the mirror plane */
	/* has a normal (0, 0, 1). that is, you look into the mirror by looking down the negative z-axis, as if looking */
	/* with an unrotated camera. so to get the mirror normal, just put (0, 0, 1) through the local to world matrix */
	/* and normalize. also by convention, the model origin of the instance is on the reflection plane, so a point on */
	/* the plane is just the instance's translation vector. mirrors should be built such that they are thin in the */
	/* z-axis so that the look-at point and the mirror extents are closely related to the mirror plane */

	Vector3 vPositiveZ(0.0f, 0.0f, 1.0f);
	Vector3 vPlaneNormal = vPositiveZ * pInst->Matrix();
	pReflection->ReflectPlane.Set( vPlaneNormal, pInst->Matrix().GetTranslation() );

	/* the center of the instance is the world space look-at point for calculating the view frustum to render with to */
	/* generate a reflection */

#ifdef _DEBUG
	Vector3 vWorldCenter = pInst->Model()->BoundingSphere.Center * pInst->Matrix();
	ASSERT(Math::Zero((vWorldCenter - pInst->BoundingSphere().Center).Length(), 0.0001f));
	vWorldCenter = BoundingBox__GetCenter(&pInst->BoundingBox());
	ASSERT(Math::Zero((vWorldCenter - pInst->BoundingSphere().Center).Length(), 0.0001f));
#endif //_DEBUG
	pReflection->vReflectWorldLookAt = pInst->BoundingSphere().Center;

	/* we know that model was built with the mirror plane's normal pointing along the positive z-axis. we want the */
	/* extent of the mirror geometry when viewed from behind the mirror, so the left extent is positive x, the right */
	/* extent is negative x, the top extent is positive y, and the bottom extent is negative y */

	pReflection->vMirrorLeft = pInst->Model()->BoundingSphere.Center;
	pReflection->vMirrorLeft.x( pInst->Model()->BoundingBox.Max.x());		// left extent is highest x
	pReflection->vMirrorRight = pInst->Model()->BoundingSphere.Center;
	pReflection->vMirrorRight.x( pInst->Model()->BoundingBox.Min.x());		// right extent is lowest x
	pReflection->vMirrorTop = pInst->Model()->BoundingSphere.Center;
	pReflection->vMirrorTop.y( pInst->Model()->BoundingBox.Max.y());			// top extent is highest y
	pReflection->vMirrorBottom = pInst->Model()->BoundingSphere.Center;
	pReflection->vMirrorBottom.y( pInst->Model()->BoundingBox.Min.y());		// bottom extent is lowest y

	/* and transform to world coords */

	pReflection->vMirrorLeft = pReflection->vMirrorLeft * pInst->Matrix();
	pReflection->vMirrorRight = pReflection->vMirrorRight * pInst->Matrix();
	pReflection->vMirrorTop = pReflection->vMirrorTop * pInst->Matrix();
	pReflection->vMirrorBottom = pReflection->vMirrorBottom * pInst->Matrix();

	/* record texture size. currently only for calculation of guard band frustum of ps2 */

	pReflection->nTextureWidth = pTexture->GetImage()->GetWidth();
	pReflection->nTextureHeight = pTexture->GetImage()->GetHeight();

	/* add to head of active list */

	SLinkList__AddLinkToHead(&ReflectionMgr.ReflectionActive, pReflection);

	/* return handle */

	return(pReflection->Handle);
}

/* remove a reflection from manager */

void ReflectionMgr__KillReflection(t_Handle ReflectionHandle)
{
	/* point to reflection, error if invalid id */

	ASSERT(ReflectionHandle != INVALID_HANDLE);
	ts_Reflection *pReflection = REFLECTION_GET_RECORD(ReflectionHandle);
	ASSERT(pReflection->Handle == ReflectionHandle);			// error if id does not match. this means that record got deallocated (and maybe allocated again) somewhere else
	if (pReflection->Handle != ReflectionHandle)
		return;

	/* clear record */

	pReflection->Handle = INVALID_HANDLE;

	/* move from active to free list */

	SLinkList__RemoveLink(&ReflectionMgr.ReflectionActive, pReflection);
	SLinkList__AddLinkToHead(&ReflectionMgr.ReflectionFree, pReflection);
}

/* kill all active reflections */

void ReflectionMgr__KillAllReflections(void)
{
	/* loop through active reflections */

	while (ReflectionMgr.ReflectionActive.pHead)
	{
		/* kill it */

		ReflectionMgr__KillReflection(((ts_Reflection *) ReflectionMgr.ReflectionActive.pHead)->Handle);
	}

	/* all reflections should be in free list now, ready to go */

	ASSERT(SLinkList__Check(&ReflectionMgr.ReflectionFree) == (signed) ReflectionMgr.nMaxReflections);
}

/* create reflection textures for all active reflection instances */

void ReflectionMgr__CreateReflections(CScene *pScene)
{
	/* reflection manager must have been initialized, and be active */

	ASSERT(ReflectionMgr.pReflectionAlloc);	// if this hits, ReflectionMgr__Init hasn't been called
	if (ReflectionMgr.bActive == FALSE)
		return;

	/* quick exit */

	if (ReflectionMgr.ReflectionActive.pHead == NULL)
		return;

	/* get scene frustum's world bounds for Reflection__Create bounds check */

	Frustum__TransformToWorld(pScene->d_sceneView.GetViewFrustum(), &pScene->d_cameraToWorld);

	/* process all active reflection instances */

	ts_Reflection *pReflection = (ts_Reflection *) ReflectionMgr.ReflectionActive.pHead;
	while (pReflection)
	{
		Reflection__Create(pReflection, pScene);

		/* to next reflection */

		pReflection = pReflection->pNext;
	}
}

/* render a view from back of reflection instance to create a reflection map */

extern "C" void gfxDrawLineWC(const Vector3 *a, const Vector3 *b, ts_bRGBA *color);

void Reflection__Create(ts_Reflection *pReflection, CScene *pScene)
{
	/* is instance that contains reflection geometry in view? */

	bool bInView = Frustum__IntersectsWorldBounds(
							pScene->d_sceneView.GetViewFrustum(),
							#ifdef PS2
							NULL,	// don't worry about guard band frustum unless interested in clipping status
							#endif //PS2
							&pReflection->pInst->BoundingSphere(),
							&pReflection->pInst->BoundingBox(),
							NULL,
							NULL);
	if (bInView == FALSE)
		return;

	/* bail if camera is on wrong side of reflect plane */

	if ( !pReflection->ReflectPlane.IsFacing( pScene->d_cameraWorldPos ) )
		 return;

	/* save scene record so i can mess with it to generate reflection scene (new camera matrix, new scene view) */

	static CScene SaveScene;
	SaveScene = *pScene;

	/* reflect camera to other side of reflection plane */

	pScene->d_cameraWorldPos = pReflection->ReflectPlane.Reflect( pScene->d_cameraWorldPos );

	/* calculate world-to-camera matrix for this new camera view, point scene off to it */

	static Graphics4x4 g4ReflectionWorldToCamera;
	BuildCTM(pScene->d_cameraWorldPos, pReflection->vReflectWorldLookAt, vUp, &g4ReflectionWorldToCamera);
	pScene->d_worldToCamera = &g4ReflectionWorldToCamera;

	/* and invert it for scene processing */

	g4ReflectionWorldToCamera.Invert(*(Matrix4x4 *) &pScene->d_cameraToWorld);

	/* this scene view is pixel-based (size of texture to render into), not ts_Viewport-based */

	pScene->d_sceneView.SetViewport(pReflection->nTextureWidth, pReflection->nTextureHeight);

	/* calculate the view frustum that extends from the reflected camera position through the corners of the */
	/* reflection geometry. here's the layout (top view, in world space):

                 A---------C---------B
                /                  /
               /                /
              /              /
             /            /
            /          /
           /        /
          / fov  /
         /    /
        /  /
       //
      E

	/* E is eyepoint (camera position). C is the center of the reflection geometry-- to render a reflection, */
	/* the camera looks at the center point. A and B are the endpoints of reflection geometry. we want angle a. */
	/* we have cos(a) = (EA dot EB) * length(EA) * length(EB) */

	Vector3 vLineEA;
	Vector3 vLineEB;
	vLineEA = pReflection->vMirrorLeft - pScene->d_cameraWorldPos;
	vLineEB = pReflection->vMirrorRight - pScene->d_cameraWorldPos;
	float fFieldOfViewXRadians = Math::ArcCos(Vector3::Dot(vLineEA, vLineEB) / (vLineEA.Length() * vLineEB.Length()));

	/* again for vertical field of view */

	vLineEA = pReflection->vMirrorTop - pScene->d_cameraWorldPos;
	vLineEB = pReflection->vMirrorBottom - pScene->d_cameraWorldPos;
	float fFieldOfViewYRadians = Math::ArcCos(Vector3::Dot(vLineEA, vLineEB) / (vLineEA.Length() * vLineEB.Length()));

	/* override field of view, aspect ratio for scene view. for aspecta ratio, see note in Frustum.cpp where */
	/* fHalfViewXAngleRadians is calculated */

	pScene->d_sceneView.SetFieldOfViewY(Math::Rad2Deg(fFieldOfViewYRadians));
	pScene->d_sceneView.SetAspectRatio(Math::Tan(fFieldOfViewXRadians / 2.0f) / Math::Tan(fFieldOfViewYRadians / 2.0f));

	/* set up for rendering into reflection texture */

	pReflection->pTexture->Lock();
	pReflection->pTexture->LockForRender();

	/* render reflected scene */

	pReflection->pInst->Hide( true );
	pScene->Render( true );
	pReflection->pInst->Hide( false );

	/* finish rendering into texture */

	pReflection->pTexture->UnlockForRender();
	pReflection->pTexture->Unlock();

#if 0
// let's see the reflection view frustum (lines will be buffered and rendered when the game view scene is rendered)
static ts_bRGBA Blue = {0, 0, 255, 255};
static ts_bRGBA Green = {0, 255, 0, 255};
static ts_bRGBA Red = {255, 0, 0, 255};
for (int i = 0; i < 8; i++)
	WorkVector[i] = pScene->ViewFrustum.Corners[i] * pScene->g4CameraToWorld;
//gfxDrawLineWC(&WorkVector[0], &WorkVector[1], &Green);
//gfxDrawLineWC(&WorkVector[1], &WorkVector[2], &Green);
//gfxDrawLineWC(&WorkVector[2], &WorkVector[3], &Green);
//gfxDrawLineWC(&WorkVector[3], &WorkVector[0], &Green);
//gfxDrawLineWC(&WorkVector[4], &WorkVector[5], &Blue);
//gfxDrawLineWC(&WorkVector[5], &WorkVector[6], &Blue);
//gfxDrawLineWC(&WorkVector[6], &WorkVector[7], &Blue);
//gfxDrawLineWC(&WorkVector[7], &WorkVector[4], &Blue);
//gfxDrawLineWC(&WorkVector[0], &WorkVector[4], &Blue);
//gfxDrawLineWC(&WorkVector[1], &WorkVector[5], &Blue);
//gfxDrawLineWC(&WorkVector[2], &WorkVector[6], &Blue);
//gfxDrawLineWC(&WorkVector[3], &WorkVector[7], &Blue);

// get intersection of frustum with reflection plane

Vector3 vPointOnPlane = {0.0f, 0.0f, 0.0f};
Vector3 vIntersect0;
Vector3 vIntersect1;
Vector3 vIntersect2;
Vector3 vIntersect3;
Segment segment;
segment.Set(WorkVector[0], WorkVector[4]);
segment.IntersectWithPlane(vPointOnPlane, pReflection->ReflectPlane.N, vIntersect0);
segment.Set(WorkVector[1], WorkVector[5]);
segment.IntersectWithPlane(vPointOnPlane, pReflection->ReflectPlane.N, vIntersect1);
segment.Set(WorkVector[2], WorkVector[6]);
segment.IntersectWithPlane(vPointOnPlane, pReflection->ReflectPlane.N, vIntersect2);
segment.Set(WorkVector[3], WorkVector[7]);
segment.IntersectWithPlane(vPointOnPlane, pReflection->ReflectPlane.N, vIntersect3);

//gfxDrawLineWC(&WorkVector[0], &vIntersect0, &Blue);
//gfxDrawLineWC(&WorkVector[1], &vIntersect1, &Blue);
//gfxDrawLineWC(&WorkVector[2], &vIntersect2, &Blue);
//gfxDrawLineWC(&WorkVector[3], &vIntersect3, &Blue);
//gfxDrawLineWC(&vIntersect0, &vIntersect1, &Blue);
//gfxDrawLineWC(&vIntersect1, &vIntersect2, &Blue);
//gfxDrawLineWC(&vIntersect2, &vIntersect3, &Blue);
//gfxDrawLineWC(&vIntersect3, &vIntersect0, &Blue);

static Vector3 vSave[8];
if (RenderMgr__VertexColorsOn())
{
	vSave[0] = WorkVector[0];
	vSave[1] = WorkVector[1];
	vSave[2] = WorkVector[2];
	vSave[3] = WorkVector[3];
	vSave[4] = vIntersect0;
	vSave[5] = vIntersect1;
	vSave[6] = vIntersect2;
	vSave[7] = vIntersect3;
}
gfxDrawLineWC(&vSave[0], &vSave[1], &Green);
gfxDrawLineWC(&vSave[1], &vSave[2], &Green);
gfxDrawLineWC(&vSave[2], &vSave[3], &Green);
gfxDrawLineWC(&vSave[3], &vSave[0], &Green);
gfxDrawLineWC(&vSave[0], &vSave[4], &Blue);
gfxDrawLineWC(&vSave[1], &vSave[5], &Blue);
gfxDrawLineWC(&vSave[2], &vSave[6], &Blue);
gfxDrawLineWC(&vSave[3], &vSave[7], &Blue);
gfxDrawLineWC(&vSave[4], &vSave[5], &Blue);
gfxDrawLineWC(&vSave[5], &vSave[6], &Blue);
gfxDrawLineWC(&vSave[6], &vSave[7], &Blue);
gfxDrawLineWC(&vSave[7], &vSave[4], &Blue);

#endif //0

	/* restore scene record */

	*pScene = SaveScene;
}

/* grab a record from free pool, assign handle, return handle */

ts_Reflection *ReflectionMgr__AllocReflectionRecord(void)
{
	/* grab a record from free pool */

	ts_Reflection *pReflection = (ts_Reflection *) SLinkList__GetAndRemoveHead(&ReflectionMgr.ReflectionFree);
	ASSERT(pReflection);	// shouldn't run out of records. if so, increase nMaxReflections count passed to scnInit
	if (pReflection == NULL)
		return(NULL);

	/* build handle */

	ASSERT(pReflection->Handle == INVALID_HANDLE);					// record should not have a handle assigned
	HANDLE_INDEX(pReflection->Handle) = pReflection - ReflectionMgr.pReflectionAlloc;	// index into allocated array of records
	HANDLE_ID(pReflection->Handle) = ReflectionMgr.u16ID++;
	if (pReflection->Handle == INVALID_HANDLE)
		HANDLE_ID(pReflection->Handle) = ReflectionMgr.u16ID++;	// do it again to avoid INVALID_HANDLE
	ASSERT(pReflection->Handle != INVALID_HANDLE);					// record should have a valid handle

	/* clear record */

	pReflection->pInst = NULL;

	/* success */

	return(pReflection);
}

#ifdef ADD_TEST_MIRROR
/* add a test mirror to world */

void ReflectionMgr__AddTestMirror(void)
{
	/* ok, build a model and instance from scratch! */

 	static ts_PatchSOM MirrorSOM;
	memset(&MirrorSOM, 0, sizeof(MirrorSOM));
	MirrorSOM.dh.u8DataType = DATA_SOM;
	MirrorSOM.dh.u16Flags |= SOMM_HAS_NAME;
	strcpy(MirrorSOM.Name, "Mirror");
	MirrorSOM.u32TriStripCount = 1;

	/* build a texture context */

	static ts_TextureContext MirrorTextureContext;
	memset(&MirrorTextureContext, 0, sizeof(ts_TextureContext));
	MirrorTextureContext.TextureCount = 1;
	MirrorTextureContext.BaseColor.R = 255;
	MirrorTextureContext.BaseColor.G = 255;
	MirrorTextureContext.BaseColor.B = 255;
	MirrorTextureContext.BaseColor.A = 255;
	MirrorTextureContext.TextureLayer[0].u8Blend = RS_BLENDFUNC_DEFAULT;
	MirrorTextureContext.TextureLayer[0].u8Filter = TS_FILTER_BILINEAR;
	MirrorTextureContext.TextureLayer[0].u8Method = TS_METHOD_MODULATE;
	MirrorTextureContext.TextureLayer[0].u8WrapU = TS_WRAP_CLAMP;
	MirrorTextureContext.TextureLayer[0].u8WrapV = TS_WRAP_CLAMP;
	MirrorTextureContext.TextureLayer[0].u8Flags |= TEXTURE_REFLECTS;
	strcpy(MirrorTextureContext.TextureLayer[0].TextureName, "Mirror");

	/* hook it up to reflection texture */

	t_Handle Handle = TextureMgr__AllocRealtimeTexture(REALTIME_REFLECTION);
	u16 u16Bind;
	TextureMgr__GetRealtimeTextureRenderFromInfo(REALTIME_REFLECTION, Handle, &u16Bind, NULL, NULL, NULL);
	MirrorTextureContext.TextureLayer[0].s16TextureBind = u16Bind;

	/* build a tri strip. by convention, mirrors are built so that the mirror's plane normal is (0, 0, 1). */
	/* by default, strips start out counter-clockwise */

	u8 *pTriData = (u8 *) memAlloc(1000, VU_ALIGN);
	MirrorSOM.pTriStripBuffer = (ts_TriStripHeader *) pTriData;
	memset(pTriData, 0, 1000);
	ts_TriStripHeader *pTriStripHeader = (ts_TriStripHeader *) pTriData;
	pTriStripHeader->pTextureContext = &MirrorTextureContext;
	pTriStripHeader->u16PointCount = 4;
	pTriStripHeader->u16SkipPointCount = pTriStripHeader->u16PointCount;
	pTriStripHeader->u8Opacity = 100;
	pTriData = (u8 *) TRI_STRIP_SKIP_HEADER(pTriData);
	Vector4 *pVertices = (Vector4 *) pTriData;
	pVertices[0].x = 2.0f;
	pVertices[0].y = 5.0f;
	pVertices[0].z = 0.0f;
	pVertices[1].x = -2.0f;
	pVertices[1].y = 5.0f;
	pVertices[1].z = 0.0f;
	pVertices[2].x = 2.0f;
	pVertices[2].y = 0.0f;
	pVertices[2].z = 0.0f;
	pVertices[3].x = -2.0f;
	pVertices[3].y = 0.0f;
	pVertices[3].z = 0.0f;
	pTriData = (u8 *) TRI_STRIP_SKIP_VERTICES(pTriData, pTriStripHeader->u16SkipPointCount);
	Vector2 *pUVs = (Vector2 *) pTriData;
	pUVs[0].x = 0.0f;
	pUVs[0].y = 1.0f;
	pUVs[1].x = 1.0f;
	pUVs[1].y = 1.0f;
	pUVs[2].x = 0.0f;
	pUVs[2].y = 0.0f;
	pUVs[3].x = 1.0f;
	pUVs[3].y = 0.0f;

	/* build bounding data */

	BoundingBox__Clear(&MirrorSOM.BoundingBox);
	int i;
	for (i = 0; i < pTriStripHeader->u16PointCount; i++)
		BoundingBox__Include(&MirrorSOM.BoundingBox, (Vector3 *) &pVertices[i]); 
	BoundingBox__GetCenter(&MirrorSOM.BoundingBox, &MirrorSOM.BoundingSphere.Center);
	Vector3 vTemp = MirrorSOM.BoundingBox.Max - MirrorSOM.BoundingSphere.Center;
	MirrorSOM.BoundingSphere.Radius = vTemp.Length();

	/* build an instance */

	static CInstance MirrorInstance;
	MirrorInstance.Reset();
	MirrorInstance.SetName( "Mirror" );
	MirrorInstance.Matrix().SetYRotation(Math::Pi);
	MirrorInstance.pSOM = &MirrorSOM;

	/* instance bounding box-- transform model points */

	BoundingBox__Clear(&MirrorInstance.BoundingBox);
	for (i = 0; i < pTriStripHeader->u16PointCount; i++)
	{
		Vector4 v4 = pVertices[i] * MirrorInstance.mLocalToWorld; 
		BoundingBox__Include(&MirrorInstance.BoundingBox, (Vector3 *) &v4); 
	}
	BoundingBox__GetCenter(&MirrorInstance.BoundingBox, &MirrorInstance.BoundingSphere.Center);
	vTemp = MirrorInstance.BoundingBox.Max - MirrorInstance.BoundingSphere.Center;
	MirrorInstance.BoundingSphere.Radius = vTemp.Length();

	/* add instance to scenes */

	SceneMgr::AddInstance(&MirrorInstance);

	/* register it as a reflector */

	ReflectionMgr__RegisterReflection(&MirrorInstance);
}
#endif //ADD_TEST_MIRROR
