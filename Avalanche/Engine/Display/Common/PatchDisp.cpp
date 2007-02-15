//============================================================================
//=
//= PatchDisp.cpp -
//=    This module is responsible for passing tri-strips off to the
//=    hardware renderers for each platform.
//=
//============================================================================

#include "Display/DisplayPCH.h"
#include "Layers/Timing.h"
#include "Game/Managers/ShadowMgr.h"

//============================================================================

// when pc uses ps2 data, it gets 4-component vertices (mainly so that the
// w-component can hold the adc bit for splicing strips together. the
// w-component is not needed for opengl rendering, so ignore it

#if defined(WIN32) && (TRI_STRIP_VERTEX_TYPE == Vector4)
#define TRI_STRIP_VERTEX_SIZE		3
#define TRI_STRIP_VERTEX_STRIDE	(4 * sizeof(float))
#else //WIN32/Vector4
#define TRI_STRIP_VERTEX_SIZE		(sizeof(TRI_STRIP_VERTEX_TYPE) / sizeof(float))
#define TRI_STRIP_VERTEX_STRIDE	0
#endif //WIN32/Vector4

// for analyzing AttachMgr calculations

#if !defined( CDROM )
#define STICK_FIGURES
#endif //

#if defined( PS2 ) && defined( _DEBUG )
#define PROFILE_RENDERER
#endif

#if defined( PS2 ) && !defined( CDROM )
#define PS2_LABEL_MODELS
#endif

#define MAX_OBJECT_BONES 256

//============================================================================

/* patch processing */

static u32 u32RenderFlags;								// render flags for patches

#if defined(DIRECTX)
	static Matrix4x4	BoneLocal2CamMatrix[MAX_OBJECT_BONES];
#endif

//============================================================================

#if defined( WIN32 ) && !defined( CONSUMER_BUILD )
static void PatchSOM__UpdateShadowStats(ts_PatchSOM *pSOM, ts_TriStripHeader *pTriStripHeader);
#endif

#ifdef STICK_FIGURES
static bool PatchSOM__DrawStickFigure(ts_PatchSOM *pPatchSOM, ts_Body *pBody);
#endif //STICK_FIGURES

//============================================================================

#if defined( PS2 )
#include "Display/PS2/PatchDisp_PS2.cpp"
#elif defined( GCN )
#include "Display/GCN/PatchDisp_GCN.cpp"
#elif defined( DIRECTX )
#include "Display/DirectX/PatchDisp_DX.cpp"
#endif

//============================================================================

/* render triangle strips in a SOM model */

//============================================================================

/* flush the current display lists. the display list buffer is double-buffered for compatibility with the master
display list. see comment at declaration of DisplayListBuffer  */

void DisplayList__ResetDisplayLists(bool bCopy)
{
#ifdef WIN32
	SceneMgr::ResetDisplayLists( );
#endif //WIN32
}

//============================================================================

/* build up a display list that represents the geometry. only directx does this at this point. ps2 display lists are
pre-built, display lists are not a win on opengl, and gcn does not use display lists */

void PatchSOM__BuildDisplayList(ts_PatchSOM *pPatchSOM, ts_DisplayListRecord *pOrigDisplayListRecord, void *pVertexColors, ts_PatchSOM__RenderFlags ucFlags, ts_Body *pBody)
{

}

//============================================================================

#if defined( WIN32 ) && !defined( CONSUMER_BUILD )
static void PatchSOM__UpdateShadowStats(ts_PatchSOM *pSOM, ts_TriStripHeader *pTriStripHeader) 
{
	/* total polys being rendered */
	SceneMgr::GetStats().IncShadowTriangles( pTriStripHeader->u16PointCount - 2 );

	/* no intersect-light-frustum calculation for boned models-- we don't have local-to-world transform for bones */

	if (pSOM->dh.u16Flags & SOMM_HAS_BONES)
		return;

	/* number that intersect light frustum */

	Vector4 *pVertices = (Vector4 *) TRI_STRIP_SKIP_HEADER(pTriStripHeader);
	for (int i = 0; i < pTriStripHeader->u16PointCount - 2; i++)
	{
		/* to world coords */

		Vector3 v1, v2, v3;
		v1.FastMultiply(*(Vector3 *) &pVertices[i + 0], pRenderInst->Matrix());
		v2.FastMultiply(*(Vector3 *) &pVertices[i + 1], pRenderInst->Matrix());
		v3.FastMultiply(*(Vector3 *) &pVertices[i + 2], pRenderInst->Matrix());

		/* create a bounding box */

		ts_BoundingBox TriangleBox;
		BoundingBox__Clear(&TriangleBox);
		BoundingBox__Include(&TriangleBox, v1);
		BoundingBox__Include(&TriangleBox, v2);
		BoundingBox__Include(&TriangleBox, v3);

		/* does it intersect light frustum? */

#if _DEBUG
		if (Frustum__IntersectsWorldBox(ShadowMgr__GetCurrentFrustum(), &TriangleBox))
			SceneMgr::GetStats().IncShadowTrianglesInFrustum( 1 );
#endif
	}
}
#endif //WIN32 && _DEBUG

//============================================================================

#ifdef STICK_FIGURES
/* draw boned models as stick figures for AttachMgr analysis */

static bool PatchSOM__DrawStickFigure(ts_PatchSOM *pPatchSOM, ts_Body *pBody)
{
	ASSERT(pBody);	// som is marked as HAS_BONES, better have them
	
	if(RenderMgr__BoneRender() == 3)
		return true;

	/* loop through attach bone list */

	for (int i = 1; i < pBody->usBones; i++)	// skip model bone
	{
		ts_Bone* pBone = &pBody->pBones[ i ];
		bool bDrawBox = false;

		/* skip orient-like bones */

		if (pBone->pModelBone->OrientBone != 0)
			continue;

		/* skip possible "main" bone */

		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "main") == 0)
			continue;

#if 0
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "pelvis") == 0)
			goto Yes;
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "spine1") == 0)
			goto Yes;
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "spine2") == 0)
			goto Yes;
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "spine3") == 0)
			goto Yes;
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "neck1") == 0)
			goto Yes;
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "head") == 0)
			goto Yes;
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "bicepright") == 0)
			goto Yes;
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "forearmright") == 0)
			goto Yes;
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "handright") == 0)
			goto Yes;
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "thighright") == 0)
			goto Yes;
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "calfright") == 0)
			goto Yes;
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "footright") == 0)
			goto Yes;
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "toesright") == 0)
			goto Yes;
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "bicepleft") == 0)
			goto Yes;
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "forearmleft") == 0)
			goto Yes;
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "handleft") == 0)
			goto Yes;
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "thighleft") == 0)
			goto Yes;
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "calfleft") == 0)
			goto Yes;
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "footleft") == 0)
			goto Yes;
		if (stricmp(pBone->pBody->pMBones[i].cBoneName, "toesleft") == 0)
			goto Yes;

		/* don't want this bone */

		bDrawBox = false;
		goto Ok;

//		continue;

Yes:
		
		bDrawBox = true;
		
Ok:
#else
		bDrawBox = true;//allow all bones to be viewed as a box
#endif


		/* pBone->Matrix holds bone-space-to-local transform. calculate bone-space-to-world */

		Graphics4x4 mBoneToWorld;
		mBoneToWorld.FastMultiply(pBone->Matrix, pRenderInst->Matrix());

		/* vPivot is bone-space start of bone, vBoneEnd is bone-space end of bone. get world-space start and end position */

		Vector3 vStart = pBone->pModelBone->vPivot * mBoneToWorld;
		Vector3 vEnd = pBone->pModelBone->vBoneEnd * mBoneToWorld;

		/* draw line that represents bone */

		ts_bRGBA White = {255, 255, 255, 255};
		gfxDrawLineWC(&vStart, &vEnd, &White);

		if(bDrawBox && RenderMgr__BoneRender() == 2)
		{
		/* convert bone bounding box to world space */

		mBoneToWorld.FastMultiply(pBone->pModelBone->boneToModel, pBone->Matrix);
		mBoneToWorld.FastMultiply(mBoneToWorld, pRenderInst->Matrix());

		Vector3 WorkVector[8];

		WorkVector[0] = pBone->pModelBone->mBoneBox.Min;

		WorkVector[1] = pBone->pModelBone->mBoneBox.Min;
		WorkVector[1].z(pBone->pModelBone->mBoneBox.Max.z());

		WorkVector[2] = pBone->pModelBone->mBoneBox.Min;
		WorkVector[2].y(pBone->pModelBone->mBoneBox.Max.y());

		WorkVector[3] = pBone->pModelBone->mBoneBox.Max;
		WorkVector[3].x(pBone->pModelBone->mBoneBox.Min.x());

		WorkVector[4] = pBone->pModelBone->mBoneBox.Min;
		WorkVector[4].x(pBone->pModelBone->mBoneBox.Max.x());

		WorkVector[5] = pBone->pModelBone->mBoneBox.Max;
		WorkVector[5].y(pBone->pModelBone->mBoneBox.Min.y());

		WorkVector[6] = pBone->pModelBone->mBoneBox.Max;
		WorkVector[6].z(pBone->pModelBone->mBoneBox.Min.z());

		WorkVector[7] = pBone->pModelBone->mBoneBox.Max;

		for (int j = 0; j < 8; j++)
			WorkVector[j] = WorkVector[j] * mBoneToWorld;

		/* draw box. points 0..3 are min-x face, in z-order */

		ts_bRGBA LightBlue = {0, 255, 255, 255};
		gfxDrawLineWC(&WorkVector[0], &WorkVector[1], &LightBlue);
		gfxDrawLineWC(&WorkVector[1], &WorkVector[3], &LightBlue);
		gfxDrawLineWC(&WorkVector[3], &WorkVector[2], &LightBlue);
		gfxDrawLineWC(&WorkVector[2], &WorkVector[0], &LightBlue);

		/* draw box. points 4..7 are max-x face, in z-order */

		gfxDrawLineWC(&WorkVector[4], &WorkVector[5], &LightBlue);
		gfxDrawLineWC(&WorkVector[5], &WorkVector[7], &LightBlue);
		gfxDrawLineWC(&WorkVector[7], &WorkVector[6], &LightBlue);
		gfxDrawLineWC(&WorkVector[6], &WorkVector[4], &LightBlue);

		/* points in min-x and max-x faces correspond for remaining edges */

		gfxDrawLineWC(&WorkVector[0], &WorkVector[4], &LightBlue);
		gfxDrawLineWC(&WorkVector[1], &WorkVector[5], &LightBlue);
		gfxDrawLineWC(&WorkVector[2], &WorkVector[6], &LightBlue);
		gfxDrawLineWC(&WorkVector[3], &WorkVector[7], &LightBlue);
		}
	}

	/* we drew a stick figure! */

	return(true);
}
#endif //STICK_FIGURES

//============================================================================
