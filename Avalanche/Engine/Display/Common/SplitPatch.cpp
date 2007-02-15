/***********************************/
/*                                 */
/* SplitPatch.c                    */
/* big juju prototype  09/20/99    */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* patch tessellation              */
/*                                 */
/***********************************/

/* this routine contains the code to tesselate a hash patch into a number of polygons, based on curvature of the spline edges.
the module is descended from splitpat.cpp from Dan Tiedy at Hash, but i've done some optimizing and reworked it into straight
c code for better translation to one of the ps2's vector units (keep your fingers crossed)

the parametric equation for a spline edge is:

  (1 - t)^3 * p0  +  3 * (1 - t)^2 * t * p1  +  3 * (1 - t) * t ^ 2 * p2  +  t^3 * p3

where p0 is the first endpoint, p1 is the first control vertex, p2 is the second control vertex, and p3 is the second endpoint:

   p0       p1          p2       p3
   +-------->............<--------+

typically, of course, the spline is not a straight line

the 1st and 2nd derivatives are:

	p0:	1st:	-3  +  6 * t  -  3 * t^2
			2nd:	6  -  6 * t
	p1:	1st:	3  -  12 * t  +  9 * t^2
			2nd:	-12  +  18 * t
	p2:	1st:	6 * t  -  9 * t^2
			2nd:	6  -  18 * t
	p3:	1st:	3 * t^2
			2nd:	6t

	at t = 0:

	1st:	-3 * p0  +  3 * p1
	2nd:	6 * p0  -  12 * p1  +  6 * p2

	at t = 1/3:

	1st:	-4/3 * p0  +  0 * p1  +  1 * p2  +  1/3 * p3
	2nd:	4 * p0  -  6 * p1  +  0 * p2  +  2 * p3

	at t = 1/2:

	1st:	-3/4 * p0  -  3/4 * p1  +  3/4 * p2  +  3/4 * p3
	2nd:	3 * p0  -  3 * p1  -  3 * p2  +  3 * p3

	at t = 2/3:

	1st:	-1/3 * p0  -  1 * p1  +  0 * p2  +  4/3 * p3
	2nd:	2 * p0  +  0 * p1  -  6 * p2  +  4 * p3

	at t = 1:

	1st:	-3 * p2  +  3 * p3
	2nd:	6 * p1  -  12 * p2  +  6 * p3

patch layout for SubSplitVertical and SubSplitHorizontal:

       uv0                uv1              uv0                uv1				  uv0         uv1      uv0         uv1
     v0 +----->......<-----+ v1          v0 +----->......<-----+ v1			v0 +--->...<---+ v1  v0 +--->...<---+ v1
        |    d0      d1    |                |    d0      d1    |				   |  d0   d1  |	      |  d0   d1  |
        |                  |	              v d2            d5 v				   |           |	      |           |
        |                  |                .      Patch1      .				   |           |        |           |
        v d2            d5 v                ^ d6            d9 ^				   v d2     d5 v        v d2     d5 v
        .                  .		           |    d10    d11    |				   .           .	      .           .
        .                  .		        v3 +----->......<-----+ v2			   .           .	      .           .
        .                  .		          uv3                uv2				   .           .	      .           .
        .                  .																   .  Patch1   .         .  Patch2   .
        .                  .		          uv0                uv1				   .           .	      .           .
        .                  .             v0 +----->......<-----+ v1			   .           .         .           .
        .                  .		           |    d0      d1    |				   .           .	      .           .
        ^ d6            d9 ^                v d2            d5 v				   ^ d6     d9 ^        ^ d6     d9 ^
        |                  |	              .      Patch2      .				   |           |	      |           |
        |                  |	              ^ d6            d9 ^				   |           |	      |           |
        |    d10    d11    |                |    d10    d11    |				   |  d10 d11  |        |  d10 d11  |
     v3 +----->......<-----+ v2        v3   +----->......<-----+ v2			v3 +--->...<---+ v2  v3 +--->...<---+ v2
       uv3                uv2              uv3                uv2				  uv3         uv2      uv3         uv2

           passed patch                       SubSplitVertical					           SubSplitHorizontal

SubSplitVertical: for a 3-point patch, v0 == v3, so sub-dividing the left edge always yields the same point,
and 2 more 3-point patches:

     v0 +----->......<-----+ v1
         \ \               |
           \   \           |
             \	    \       |
               \	     \   |
                 \	      + subdivision point
                   \		   |
                     \     |
                       \   |
                         \ |
                           + v2

SubSplitHorizontal: for a 3-point patch, v0 == v3, so the result is a triangle and a quad:

     v0 +--------+---------+ v1
         \       |         |
           \     |         |
             \	  |         |
               \ |	      |
                 +	      |
                   \		   |
                     \     |
                       \   |
                         \ |
                           + v2

*/

/******************* includes ****************************/

#include "Display/DisplayPCH.h"



#ifdef TOOLS
#include <math.h>
#include <crtdbg.h>
#include <tessellation.h>
#endif //TOOLS


/******************* defines *****************************/

/* default straightness tolerance */

#define DEFAULT_STRAIGHT_TOLERANCE	0.100f //djr: was 10.0 when DB was in centimeters, changed to .1 now that DB is in meters

/******************* externs *****************************/

#ifdef WIN32
extern Vector3 *pRenderVertex;
#elif defined(PS2)
extern Vector4 *pRenderVertex;
#elif defined(GCN)
extern Vector3 *pRenderVertex;
#endif //WIN32/PS2
extern Vector2 *pRenderUV[MAX_SOM_TEXTURES_PER_PATCH];
extern ts_fRGBA *pRenderColor;
#ifdef TOOLS
/* sometimes useful */

void WriteLogFile(const char *pcFmt, ...);
#endif //TOOL

/******************* global variables ********************/

/* tessellation environment variables */

int nPatchTextureCount;

/* original render-ready patch to be tessellated (pointed to by pPatch when SplitPatch is called) */

ts_RenderPatch RenderPatch;

#ifdef TOOLS
/* for calculating a patch's bounding box during export */

ts_BoundingBox PatchBoundingBox;
bool bCollisionPatchSplit;
u32 u32CollisionTriangleCount;
u32 u32CollisionVertexCount;
#endif //TOOLS

/* pre-calculated colors for s-shaped edges */

ts_fRGBA SShapedColor[S_SHAPED_MAX_ENTRIES];

#ifdef TOOLS
/* for data export, pre-calculated normals for s-shaped edges */

Vector3 SShapedNormal[S_SHAPED_MAX_ENTRIES];

/* default color uv values for non-tessellated patch */

Vector2 DefaultColorUVs[4] = {Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f), Vector2(0.0f, 1.0f)};
#endif //TOOLS

/******************* local variables *********************/

static float fStraightToleranceSquared = DEFAULT_STRAIGHT_TOLERANCE * DEFAULT_STRAIGHT_TOLERANCE;

/* for forced-zoom tessellation (e.g collision tessellation) */

static bool bForceZoom = FALSE;
static float fForceZoomSquared;

/* near clip plane comparison value for tesselator */

static float fNegativeNearClip = 0.0f;

/* for non-recursive processing, i need a stack where i can put Patch2 until it is time to divide it */

#define MAX_STACK_DEPTH	40
static ts_RenderPatch PatchStack[MAX_STACK_DEPTH];
static u32 u32FlagsStack[MAX_STACK_DEPTH];
static int nStackIndex;

#ifdef TOOLS
/* for uv dividing */

static float fDeltaX;
static float fDeltaY;
static float fDeltaZ;
static float fLength1;
static float fLength2;
static float fMult;
#endif //TOOLS

/******************* local prototypes ********************/

#ifdef TOOLS
static void SetProportionalUVFlags(ts_RenderPatch *pPatch, u32 *pu32Flags);
#endif //TOOLS

/******************* functions ***************************/

/* calculate "zoom" factor for edge. this is a multiplier based on the edge's distance to the camera. it is critical that
shared edges between patches use the same soom factor, because this is what determines when an edge meets the straightness
tolerance test. in order to avoid cracks between patches, shared edges have to tessellate exactly the same. pV0 and pV1 are
the endpoints of the edge */

#define NEAR_ZOOM		0.30f		//yuch-- was 0.4f
#define FAR_ZOOM		0.0f
#define NEAR_LOD		2.5f		//meters
#define FAR_LOD		50.00f	//meters yuch-- was 10000.0f
#define NEAR_LOD_X2	(NEAR_LOD * 2.0f)
#define FAR_LOD_X2	(FAR_LOD * 2.0f)
#define LOD_DELTA_X2	((FAR_LOD - NEAR_LOD) * 2.0f)
#define LOD_MULT		(1.0f / (FAR_LOD - NEAR_LOD))

//yuch-- old version-- new version is slightly simpler
#define SET_ZOOM_SQUARED_OLD(V0, V1)																					\
{																																	\
	if (bForceZoom)																											\
			fZoomedStraightSquared = fForceZoomSquared;																\
	else																															\
	{																																\
		/* camera looks down negative z-axis, so z-values are typically negative */						\
																																	\
		fTemp = -0.5f * (V0.z() + V1.z());																				\
		if (fTemp < NEAR_LOD)																								\
			fZoomedStraightSquared = NEAR_ZOOM * NEAR_ZOOM;															\
		else if (fTemp > FAR_LOD)																							\
			fZoomedStraightSquared = FAR_ZOOM * FAR_ZOOM;															\
		else																														\
		{																															\
			fZoomedStraightSquared = NEAR_ZOOM * (1 - ((fTemp - NEAR_LOD) * LOD_MULT));					\
			fZoomedStraightSquared *= fZoomedStraightSquared;														\
		}																															\
	}																																\
}
#define SET_ZOOM_SQUARED(V0, V1)																							\
{																																	\
	if (bForceZoom)																											\
			fZoomedStraightSquared = fForceZoomSquared;																\
	else																															\
	{																																\
		/* camera looks down negative z-axis, so z-values are typically negative */						\
																																	\
		fTemp = -(V0.z() + V1.z());	/* make it positive */									  					\
		if (fTemp < NEAR_LOD_X2)																							\
			fZoomedStraightSquared = NEAR_ZOOM * NEAR_ZOOM;															\
		else if (fTemp > FAR_LOD_X2)																						\
			fZoomedStraightSquared = FAR_ZOOM * FAR_ZOOM;															\
		else																														\
		{																															\
			/* take range from X1..x2 to Y..0. easiest way to think of it is: */								\
			/* A' = Y * ((X2 - A) / (X2 - X1)). easiest to implement is: */									\
			/* A' = (A - X2) * (Y / (X1 - X2)) */																		\
																																	\
			fZoomedStraightSquared = (fTemp - FAR_LOD_X2) * (NEAR_ZOOM / (NEAR_LOD_X2 - FAR_LOD_X2));	\
			fZoomedStraightSquared *= fZoomedStraightSquared;														\
		}																															\
	}																																\
}



/* calculate "straightness" of edge. "curvy" edges yield a higher value. for central differencing, use sum of magnitudes
of 2nd derivatives at endpoints */

#define STRAIGHTNESS(D0, D1) (D0.LengthSquared() + D1.LengthSquared())

/* divide an edge into two sub-edges. if the edge is straight, this is pretty easy (the midpoint is the average of the
endpoints). if the edge is curved, use central differencing to calculate new midpoint and 2nd derivatives. edge is:

 V0, D0               V1, D1           V0, D0               V1, D1      V0, D0               V1, D1
   *--------------------*     ---->      *--------------------*           *--------------------*
       original patch                          new patch 1                     new patch 2

*/

#define CENTRAL_DIFF_STRAIGHT_EDGES	// assuming straight edges have 2nd derivative == 0, central difference code will divide them exactly in half, don't bother testing the flag bit
#ifdef CENTRAL_DIFF_STRAIGHT_EDGES
#define DIVIDE_EDGE(V0, V1, D0, D1)																								\
{																																			\
	/* midpoint of edge is (average of midpoints) - (0.125 * average of 2nd derivatives) */					\
																																			\
	/* patch2's 2nd derivative at right endpoint comes from original patch, but grab it before */			\
	/* it gets written over. reduce it for recursion purposes (see "du squared" comment, below) */			\
																																			\
	pPatch2->D1 = 0.25f * pPatch->D1;																							\
																																			\
	/* first calculate 2nd derivative at midpoint. it is the average of 2nd derivatives at endpoints */	\
	/* because 2nd derivative of a cubic is a linear equation. also divide it by 4 for recursion since */	\
	/* by splitting the curve in half we are reducing du by half, and the 2nd derivative of the new */		\
	/* half-curve is based on du squared */																					\
																																			\
	pPatch->D1 = (0.5f * 0.25f) * (pPatch->D0 + pPatch->D1);																\
	pPatch2->D0 = pPatch->D1;																										\
																																			\
	/* 2nd derivative at first endpoint of original edge needs to be reduced for recursion, too */			\
																																			\
	pPatch->D0 *= 0.25f;																												\
																																			\
	/* since the average of the 2nd derivatives at the endpoints has already been divided by 4, just */	\
	/* divide by 2 more to get the .125 factor */																			\
																																			\
/*	pPatch->V1.x = (0.5f * (pPatch->V0.x + pPatch->V1.x)) - (0.5f * pPatch->D1.x); */							\
/*	pPatch->V1.y = (0.5f * (pPatch->V0.y + pPatch->V1.y)) - (0.5f * pPatch->D1.y); */							\
/*	pPatch->V1.z = (0.5f * (pPatch->V0.z + pPatch->V1.z)) - (0.5f * pPatch->D1.z); */							\
	pPatch->V1 = 0.5f * (pPatch->V0 + pPatch->V1 - pPatch->D1);															\
	pPatch2->V0 = pPatch->V1;																										\
}
#else //CENTRAL_DIFF_STRAIGHT_EDGES
//yuch-- i think that there's a potential bug here. since 2nd derivatives are used in FINISH_NEW_EDGE,
// i think we get into trouble by not reducing the straight edge's 2nd derivatives...may always need
// to set them to 0 if the edge passes the straightness test...not a big deal now, since we're using
// the above code, not the following code
#define DIVIDE_EDGE(u32Flags, EDGE_STRAIGHT_FLAG, V0, V1, D0, D1)															\
{																																				\
	if (u32Flags & EDGE_STRAIGHT_FLAG)																							  	\
	{																																		  	\
		/* edge is straight, just divide it exactly in half */															  	\
																																			  	\
		pPatch->V1 = pPatch2->V0 = 0.5f * (pPatch->V0 + pPatch->V1);													  	\
																																			  	\
		/* second derivatives are 0 for straight lines */																	  	\
																																			  	\
		pPatch->D0.Clear();																											  	\
		pPatch->D1.Clear();																											  	\
		pPatch2->D0.Clear();																											  	\
		pPatch2->D1.Clear();																											  	\
	}																																		  	\
	else																																	  	\
	{																																			\
		/* midpoint of edge is (average of midpoints) - (0.125 * average of 2nd derivatives) */					\
																																				\
		/* patch2's 2nd derivative at right endpoint comes from original patch, but grab it before */			\
		/* it gets written over. reduce it for recursion purposes (see "du squared" comment, below) */			\
																																				\
		pPatch2->D1 = 0.25f * pPatch->D1;																							\
																																				\
		/* first calculate 2nd derivative at midpoint. it is the average of 2nd derivatives at endpoints */	\
		/* because 2nd derivative of a cubic is a linear equation. also divide it by 4 for recursion since */	\
		/* by splitting the curve in half we are reducing du by half, and the 2nd derivative of the new */		\
		/* half-curve is based on du squared */																					\
																																				\
		pPatch->D1 = (0.5f * 0.25f) * (pPatch->D0 + pPatch->D1);																\
		pPatch2->D0 = pPatch->D1;																										\
																																				\
		/* since the average of the 2nd derivatives at the endpoints has already been divided by 4, just */	\
		/* divide by 2 more to get the .125 factor */																			\
																																				\
/*		pPatch->V1.x = (0.5f * (pPatch->V0.x + pPatch->V1.x)) - (0.5f * pPatch->D1.x); */							\
/*		pPatch->V1.y = (0.5f * (pPatch->V0.y + pPatch->V1.y)) - (0.5f * pPatch->D1.y); */							\
/*		pPatch->V1.z = (0.5f * (pPatch->V0.z + pPatch->V1.z)) - (0.5f * pPatch->D1.z); */							\
		pPatch->V1 = 0.5f * (pPatch->V0 + pPatch->V1 - pPatch->D1);															\
		pPatch2->V0 = pPatch->V1;																										\
																																				\
		/* 2nd derivative at first endpoint of original edge needs to be reduced for recursion, too */			\
																																				\
		pPatch->D0 *= 0.25f;																												\
	}																																			\
}
#endif //CENTRAL_DIFF_STRAIGHT_EDGES

/* after dividing two of the patch's edges, we have created a shared edge between the two new patches. this shared edge
needs to have its 2nd derivatives set in both patches. the new 2nd derivatives are the averages of the 2nd derivatives
along the un-divided edges of the original patch:

   D0                  D1
   *--------------------*
     original patch edge

   D2                  D3
   *--------------------*
     patch1 shared edge

     patch2 shared edge
   *--------------------*
   D0                  D1

     original patch edge
   *--------------------*
   D2                  D3

 */

#define FINISH_NEW_EDGE(D0, D1, D2, D3)						\
{																			\
	pPatch->D2 = 0.5f * (pPatch->D0 + pPatch->D2);			\
	pPatch2->D0 = pPatch->D2;										\
	pPatch->D3 = 0.5f * (pPatch->D1 + pPatch->D3);			\
	pPatch2->D1 = pPatch->D3;										\
}

/* determine the new vertex color at the midpoint of an edge. typically, this is just the average of the colors at the
endpoints. but for s-shaped edges i have pre-calculated the correct midpoint color, since the normal at the midpoint of
an s-shaped edge is not the average of the normals at the endpoints. there is also a pre-calculated color at the midpoint
of the patch for use by the shared edge created by the first division of a patch-- that new shared edge may be s-shaped
as well. edge is:

   C0                  C1                C0                  C1           C0                  C1
   *--------------------*     ---->      *--------------------*           *--------------------*
       original patch                          new patch 1                     new patch 2

 */

#ifndef TOOLS

/* game version-- just divides colors */

#define DIVIDE_EDGE_COLOR(u32Flags, EDGE_S_SHAPED_FLAG, EDGE_COLOR_INDEX, EDGE_PATCH_MIDPOINT_FLAG, C0, C1)	\
{																																				\
	if (u32Flags & EDGE_S_SHAPED_FLAG)																								\
	{																																			\
		/* s-shaped-- use pre-calculated vertex color */																		\
																																				\
		if (u32Flags & EDGE_PATCH_MIDPOINT_FLAG)																					\
		{																																		\
			/* s-shaped interior edge-- use pre-calculated patch midpoint color */										\
																																				\
			pPatch->Color[C1] = SShapedColor[S_SHAPED_PATCH_MIDPOINT];														\
			u32Flags &= ~EDGE_PATCH_MIDPOINT_FLAG;																					\
		}																																		\
		else																																	\
			pPatch->Color[C1] = SShapedColor[EDGE_COLOR_INDEX];																\
	}																																			\
	else																																		\
	{																																			\
		/* c-shaped-- take average of endpoint vertex colors */																\
																																				\
		pPatch->Color[C1].R = 0.5f * (pPatch->Color[C0].R + pPatch->Color[C1].R);										\
		pPatch->Color[C1].G = 0.5f * (pPatch->Color[C0].G + pPatch->Color[C1].G);										\
		pPatch->Color[C1].B = 0.5f * (pPatch->Color[C0].B + pPatch->Color[C1].B);										\
		pPatch->Color[C1].A = 0.5f * (pPatch->Color[C0].A + pPatch->Color[C1].A);										\
	}																																			\
	pPatch2->Color[C0] = pPatch->Color[C1];	/* same color */																\
}
#else //TOOLS

/* data-export version-- divides normals instead of colors, but decision flow is the same */

#define DIVIDE_EDGE_COLOR(u32Flags, EDGE_S_SHAPED_FLAG, EDGE_COLOR_INDEX, EDGE_PATCH_MIDPOINT_FLAG, C0, C1)	\
{																																				\
	if (u32Flags & EDGE_S_SHAPED_FLAG)																								\
	{																																			\
		/* s-shaped-- use pre-calculated vertex color */																		\
																																				\
		if (u32Flags & EDGE_PATCH_MIDPOINT_FLAG)																					\
		{																																		\
			/* s-shaped interior edge-- use pre-calculated patch midpoint color */										\
																																				\
			pPatch->Color[C1] = SShapedColor[S_SHAPED_PATCH_MIDPOINT];														\
			pPatch->Normal[C1] = SShapedNormal[S_SHAPED_PATCH_MIDPOINT];													\
			u32Flags &= ~EDGE_PATCH_MIDPOINT_FLAG;																					\
		}																																		\
		else																																	\
		{																																		\
			pPatch->Color[C1] = SShapedColor[EDGE_COLOR_INDEX];																\
			pPatch->Normal[C1] = SShapedNormal[EDGE_COLOR_INDEX];																\
		}																																		\
	}																																			\
	else																																		\
	{																																			\
		/* c-shaped-- take average of endpoint vertex colors */																\
																																				\
		pPatch->Color[C1].R = 0.5f * (pPatch->Color[C0].R + pPatch->Color[C1].R);										\
		pPatch->Color[C1].G = 0.5f * (pPatch->Color[C0].G + pPatch->Color[C1].G);										\
		pPatch->Color[C1].B = 0.5f * (pPatch->Color[C0].B + pPatch->Color[C1].B);										\
		pPatch->Color[C1].A = 0.5f * (pPatch->Color[C0].A + pPatch->Color[C1].A);										\
																																				\
		/* and normal is average of normals, then re-normalized */															\
																																				\
		pPatch->Normal[C1] = 0.5f * (pPatch->Normal[C0] + pPatch->Normal[C1]);											\
																																				\
		/* normalize the averaged normal */																							\
																																				\
		fTemp = pPatch->Normal[C1].Length();																						\
		if (fTemp != 0.0f)																												\
		{																																		\
			fTemp = 1.0f / fTemp;																										\
			pPatch->Normal[C1] *= fTemp;																								\
		}																																		\
		else																																	\
			_ASSERT(FALSE);	/* zero-length normal-- what happened? */														\
	}																																			\
	pPatch2->Color[C0] = pPatch->Color[C1];	/* same color */																\
	pPatch2->Normal[C0] = pPatch->Normal[C1];	/* same normal */																\
}
#endif //TOOLS

/* uv coords just get averaged along edge. layout is:

  UV0                  UV1
   *--------------------*
     original patch edge

  UV3                  UV2
   *--------------------*
     patch1 shared edge

     patch2 shared edge
   *--------------------*
  UV0                  UV1

     original patch edge
   *--------------------*
  UV3                  UV2

*/

#ifndef TOOLS
/* real-time version just takes average of uv coords */

#define DIVIDE_UV_COORDS(UV0, UV1, UV2, UV3)														\
{																												\
	if (nPatchTextureCount)																				\
	{																											\
		switch(nPatchTextureCount)																		\
		{																										\
		default:																								\
			ASSERT(FALSE);																					\
			/* fall through */																			\
		case 6:																								\
			/* copy values from original patch that new patch2 will use directly */		\
																												\
			pPatch2->UV2[5] = pPatch->UV2[5];														\
			pPatch2->UV3[5] = pPatch->UV3[5];														\
																												\
			/* average uv's at divided edges */														\
																												\
			pPatch->UV3[5].x = 0.5f * (pPatch->UV0[5].x + pPatch->UV3[5].x);				\
			pPatch->UV3[5].y = 0.5f * (pPatch->UV0[5].y + pPatch->UV3[5].y);				\
			pPatch2->UV0[5] = pPatch->UV3[5];														\
			pPatch->UV2[5].x = 0.5f * (pPatch->UV1[5].x + pPatch->UV2[5].x);				\
			pPatch->UV2[5].y = 0.5f * (pPatch->UV1[5].y + pPatch->UV2[5].y);				\
			pPatch2->UV1[5] = pPatch->UV2[5];														\
																												\
			/* fall through */																			\
		case 5:																								\
			/* copy values from original patch that new patch2 will use directly */		\
																												\
			pPatch2->UV2[4] = pPatch->UV2[4];														\
			pPatch2->UV3[4] = pPatch->UV3[4];														\
																												\
			/* average uv's at divided edges */														\
																												\
			pPatch->UV3[4].x = 0.5f * (pPatch->UV0[4].x + pPatch->UV3[4].x);				\
			pPatch->UV3[4].y = 0.5f * (pPatch->UV0[4].y + pPatch->UV3[4].y);				\
			pPatch2->UV0[4] = pPatch->UV3[4];														\
			pPatch->UV2[4].x = 0.5f * (pPatch->UV1[4].x + pPatch->UV2[4].x);				\
			pPatch->UV2[4].y = 0.5f * (pPatch->UV1[4].y + pPatch->UV2[4].y);				\
			pPatch2->UV1[4] = pPatch->UV2[4];														\
																												\
			/* fall through */																			\
		case 4:																								\
			/* copy values from original patch that new patch2 will use directly */		\
																												\
			pPatch2->UV2[3] = pPatch->UV2[3];														\
			pPatch2->UV3[3] = pPatch->UV3[3];														\
																												\
			/* average uv's at divided edges */														\
																												\
			pPatch->UV3[3].x = 0.5f * (pPatch->UV0[3].x + pPatch->UV3[3].x);				\
			pPatch->UV3[3].y = 0.5f * (pPatch->UV0[3].y + pPatch->UV3[3].y);				\
			pPatch2->UV0[3] = pPatch->UV3[3];														\
			pPatch->UV2[3].x = 0.5f * (pPatch->UV1[3].x + pPatch->UV2[3].x);				\
			pPatch->UV2[3].y = 0.5f * (pPatch->UV1[3].y + pPatch->UV2[3].y);				\
			pPatch2->UV1[3] = pPatch->UV2[3];														\
																												\
			/* fall through */																			\
		case 3:																								\
			/* copy values from original patch that new patch2 will use directly */		\
																												\
			pPatch2->UV2[2] = pPatch->UV2[2];														\
			pPatch2->UV3[2] = pPatch->UV3[2];														\
																												\
			/* average uv's at divided edges */														\
																												\
			pPatch->UV3[2].x = 0.5f * (pPatch->UV0[2].x + pPatch->UV3[2].x);				\
			pPatch->UV3[2].y = 0.5f * (pPatch->UV0[2].y + pPatch->UV3[2].y);				\
			pPatch2->UV0[2] = pPatch->UV3[2];														\
			pPatch->UV2[2].x = 0.5f * (pPatch->UV1[2].x + pPatch->UV2[2].x);				\
			pPatch->UV2[2].y = 0.5f * (pPatch->UV1[2].y + pPatch->UV2[2].y);				\
			pPatch2->UV1[2] = pPatch->UV2[2];														\
																												\
			/* fall through */																			\
		case 2:																								\
			/* copy values from original patch that new patch2 will use directly */		\
																												\
			pPatch2->UV2[1] = pPatch->UV2[1];														\
			pPatch2->UV3[1] = pPatch->UV3[1];														\
																												\
			/* average uv's at divided edges */														\
																												\
			pPatch->UV3[1].x = 0.5f * (pPatch->UV0[1].x + pPatch->UV3[1].x);				\
			pPatch->UV3[1].y = 0.5f * (pPatch->UV0[1].y + pPatch->UV3[1].y);				\
			pPatch2->UV0[1] = pPatch->UV3[1];														\
			pPatch->UV2[1].x = 0.5f * (pPatch->UV1[1].x + pPatch->UV2[1].x);				\
			pPatch->UV2[1].y = 0.5f * (pPatch->UV1[1].y + pPatch->UV2[1].y);				\
			pPatch2->UV1[1] = pPatch->UV2[1];														\
																												\
			/* fall through */																			\
		case 1:																								\
			/* copy values from original patch that new patch2 will use directly */		\
																												\
			pPatch2->UV2[0] = pPatch->UV2[0];														\
			pPatch2->UV3[0] = pPatch->UV3[0];														\
																												\
			/* average uv's at divided edges */														\
																												\
			pPatch->UV3[0].x = 0.5f * (pPatch->UV0[0].x + pPatch->UV3[0].x);				\
			pPatch->UV3[0].y = 0.5f * (pPatch->UV0[0].y + pPatch->UV3[0].y);				\
			pPatch2->UV0[0] = pPatch->UV3[0];														\
			pPatch->UV2[0].x = 0.5f * (pPatch->UV1[0].x + pPatch->UV2[0].x);				\
			pPatch->UV2[0].y = 0.5f * (pPatch->UV1[0].y + pPatch->UV2[0].y);				\
			pPatch2->UV1[0] = pPatch->UV2[0];														\
																												\
			/* fall through */																			\
		}																										\
	}																											\
}

#else //TOOLS

/* just takes average of uv coords, but a single edge at a time */

#define DIVIDE_UV_COORDS(UV0, UV3)																	\
{																												\
	if (nPatchTextureCount)																				\
	{																											\
		switch(nPatchTextureCount)																		\
		{																										\
		default:																								\
			ASSERT(FALSE);																					\
			/* fall through */																			\
		case 6:																								\
			/* copy values from original patch that new patch2 will use directly */		\
																												\
			pPatch2->UV3[5] = pPatch->UV3[5];														\
																												\
			/* average uv's at divided edges */														\
																												\
			pPatch->UV3[5] = 0.5f * (pPatch->UV0[5] + pPatch->UV3[5]);						\
			pPatch2->UV0[5] = pPatch->UV3[5];														\
																												\
			/* fall through */																			\
		case 5:																								\
			/* copy values from original patch that new patch2 will use directly */		\
																												\
			pPatch2->UV3[4] = pPatch->UV3[4];														\
																												\
			/* average uv's at divided edges */														\
																												\
			pPatch->UV3[4] = 0.5f * (pPatch->UV0[4] + pPatch->UV3[4]);						\
			pPatch2->UV0[4] = pPatch->UV3[4];														\
																												\
			/* fall through */																			\
		case 4:																								\
			/* copy values from original patch that new patch2 will use directly */		\
																												\
			pPatch2->UV3[3] = pPatch->UV3[3];														\
																												\
			/* average uv's at divided edges */														\
																												\
			pPatch->UV3[3] = 0.5f * (pPatch->UV0[3] + pPatch->UV3[3]);						\
			pPatch2->UV0[3] = pPatch->UV3[3];														\
																												\
			/* fall through */																			\
		case 3:																								\
			/* copy values from original patch that new patch2 will use directly */		\
																												\
			pPatch2->UV3[2] = pPatch->UV3[2];														\
																												\
			/* average uv's at divided edges */														\
																												\
			pPatch->UV3[2] = 0.5f * (pPatch->UV0[2] + pPatch->UV3[2]);						\
			pPatch2->UV0[2] = pPatch->UV3[2];														\
																												\
			/* fall through */																			\
		case 2:																								\
			/* copy values from original patch that new patch2 will use directly */		\
																												\
			pPatch2->UV3[1] = pPatch->UV3[1];														\
																												\
			/* average uv's at divided edges */														\
																												\
			pPatch->UV3[1] = 0.5f * (pPatch->UV0[1] + pPatch->UV3[1]);						\
			pPatch2->UV0[1] = pPatch->UV3[1];														\
																												\
			/* fall through */																			\
		case 1:																								\
			/* copy values from original patch that new patch2 will use directly */		\
																												\
			pPatch2->UV3[0] = pPatch->UV3[0];														\
																												\
			/* average uv's at divided edges */														\
																												\
			pPatch->UV3[0] = 0.5f * (pPatch->UV0[0] + pPatch->UV3[0]);						\
			pPatch2->UV0[0] = pPatch->UV3[0];														\
																												\
			/* fall through */																			\
		}																										\
	}																											\
}

/* tool also has a version that looks at where edge midpoint lies to determine proportionate division */

#define DIVIDE_UV_COORDS_PROPORTIONAL(UV0, UV3, V0, V3)														\
{																																\
	if (nPatchTextureCount)																								\
	{																															\
		/* calculate proportion multiplier */																		\
																																\
		fLength1 = (pPatch->V0 - pPatch->V3).Length();															\
		fLength2 = (pPatch2->V0 - pPatch2->V3).Length();														\
		fMult = fLength1 / (fLength1 + fLength2);																	\
																																\
		switch(nPatchTextureCount)																						\
		{																														\
		default:																												\
			ASSERT(FALSE);																									\
			/* fall through */																							\
		case 6:																												\
			/* copy values from original patch that new patch2 will use directly */						\
																																\
			pPatch2->UV3[5] = pPatch->UV3[5];																		\
																																\
			/* average uv's at divided edges */																		\
																																\
			pPatch->UV3[5] = pPatch->UV0[5] + (fMult * (pPatch->UV3[5] - pPatch->UV0[5]));			\
			pPatch2->UV0[5] = pPatch->UV3[5];																		\
																																\
			/* fall through */																							\
		case 5:																												\
			/* copy values from original patch that new patch2 will use directly */						\
																																\
			pPatch2->UV3[4] = pPatch->UV3[4];																		\
																																\
			/* average uv's at divided edges */																		\
																																\
			pPatch->UV3[4] = pPatch->UV0[4] + (fMult * (pPatch->UV3[4] - pPatch->UV0[4]));			\
			pPatch2->UV0[4] = pPatch->UV3[4];																		\
																																\
			/* fall through */																							\
		case 4:																												\
			/* copy values from original patch that new patch2 will use directly */						\
																																\
			pPatch2->UV3[3] = pPatch->UV3[3];																		\
																																\
			/* average uv's at divided edges */																		\
																																\
			pPatch->UV3[3] = pPatch->UV0[3] + (fMult * (pPatch->UV3[3] - pPatch->UV0[3]));			\
			pPatch2->UV0[3] = pPatch->UV3[3];																		\
																																\
			/* fall through */																							\
		case 3:																												\
			/* copy values from original patch that new patch2 will use directly */						\
																																\
			pPatch2->UV3[2] = pPatch->UV3[2];																		\
																																\
			/* average uv's at divided edges */																		\
																																\
			pPatch->UV3[2] = pPatch->UV0[2] + (fMult * (pPatch->UV3[2] - pPatch->UV0[2]));			\
			pPatch2->UV0[2] = pPatch->UV3[2];																		\
																																\
			/* fall through */																							\
		case 2:																												\
			/* copy values from original patch that new patch2 will use directly */						\
																																\
			pPatch2->UV3[1] = pPatch->UV3[1];																		\
																																\
			/* average uv's at divided edges */																		\
																																\
			pPatch->UV3[1] = pPatch->UV0[1] + (fMult * (pPatch->UV3[1] - pPatch->UV0[1]));			\
			pPatch2->UV0[1] = pPatch->UV3[1];																		\
																																\
			/* fall through */																							\
		case 1:																												\
			/* copy values from original patch that new patch2 will use directly */						\
																																\
			pPatch2->UV3[0] = pPatch->UV3[0];																		\
																																\
			/* average uv's at divided edges */																		\
																																\
			pPatch->UV3[0] = pPatch->UV0[0] + (fMult * (pPatch->UV3[0] - pPatch->UV0[0]));			\
			pPatch2->UV0[0] = pPatch->UV3[0];																		\
																																\
			/* fall through */																							\
		}																														\
	}																															\
}

#endif //TOOLS

#ifdef TOOLS
/* same as DIVIDE_UV_COORDS for color uv, used for vertex color interpolation at export */

#define DIVIDE_COLOR_UV_COORDS(ColorUV0, ColorUV3)										\
{																										\
	/* copy values from original patch that new patch2 will use directly */		\
																										\
	pPatch2->ColorUV3 = pPatch->ColorUV3;													\
																										\
	/* average uv's at divided edges */														\
																										\
	pPatch->ColorUV3 = 0.5f * (pPatch->ColorUV0 + pPatch->ColorUV3);				\
	pPatch2->ColorUV0 = pPatch->ColorUV3;	   											\
}

#define DIVIDE_COLOR_UV_COORDS_PROPORTIONAL(ColorUV0, ColorUV3, V0, V3)										\
{																																	\
	/* calculate proportion multipliers */																				\
																																	\
	fLength1 = (pPatch->V0 - pPatch->V3).Length();																	\
	fLength2 = (pPatch2->V0 - pPatch2->V3).Length();																\
	fMult = fLength1 / (fLength1 + fLength2);																			\
																																	\
	/* copy values from original patch that new patch2 will use directly */									\
																																	\
	pPatch2->ColorUV3 = pPatch->ColorUV3;																				\
																																	\
	/* average uv's at divided edges */																					\
																																	\
	pPatch->ColorUV3 = pPatch->ColorUV0 + (fMult * (pPatch->ColorUV3 - pPatch->ColorUV0));				\
	pPatch2->ColorUV0 = pPatch->ColorUV3;	   																		\
}

#endif //TOOLS

/* the patch tessellation routine. take incoming original patch and tessellate it until all sub-patches pass the
straightness test */

void SplitPatch(ts_RenderPatch *pPatch, int nLevel, u32 u32Flags)
{
	u32 u32NewFlags;
	float fTemp;
	float fHorizontalStraightnessSquared;	// greater of unzoomed height of top, bottom edges, to decide tessellation order
	float fVerticalStraightnessSquared;		// greater of unzoomed height of left, right edges, to decide tessellation order
	float fZoomedStraightSquared;				// zoomed straightness value, for straightness tolerance test for each edge
	ts_RenderPatch *pPatch2;					// pointer into patch stack

#ifdef TOOLS
	SetProportionalUVFlags(pPatch, &u32Flags);

	/* tessellation doesn't work for polys-- they have no control vertices! */

	ASSERT((u32Flags & PATCH_IS_POLY) == 0);
#endif //TOOLS

	/* start at top of stack. pPatch points to original incoming patch. during processing, it always holds the patch */
	/* that needs to be sub-divided. when a patch is sub-divided into two sub-patches, the first patch is written over */
	/* the original patch, and the second one is placed in the patch stack until it is time to use it as the patch that */
	/* needs checking */

	nStackIndex = 0;
	while(nStackIndex >= 0)
	{
/*************************** check edges for straightness **********************************************/

		/* if there's no more room on the stack, force to done */

		if (nStackIndex == MAX_STACK_DEPTH)
		{
			u32Flags |= PATCH_ANY_EDGE_STRAIGHT;	// mark all edges straight
			ASSERT(FALSE);							// shouldn't reach bottom of stack for pc?
			pPatch2 = NULL;							// shouldn't need stack pointer, so crash if it gets referenced
		}
		else
		{
			/* left edge straightness check */

			if (u32Flags & PATCH_LEFT_EDGE_STRAIGHT)
				fVerticalStraightnessSquared = 0.0f;
			else
			{
				SET_ZOOM_SQUARED(pPatch->v0, pPatch->v3);

				if (fZoomedStraightSquared != 0.0f)
				{
					fVerticalStraightnessSquared = STRAIGHTNESS(pPatch->d2, pPatch->d6);
					fZoomedStraightSquared *= fVerticalStraightnessSquared;
				}

				/* check against straightness tolerance. use zoom factor based on edge's distance to screen. this is */
				/* important to make sure the shared edge of adjacent patches subdivide the same to prevent gaps */

				if (fZoomedStraightSquared < fStraightToleranceSquared)
				{
					/* consider it perfectly straight-- note that it is possible for an edge to be marked straight */
					/* and s-shaped at the same time, so that it can subdivide as a straight edge but still do the */
					/* s-shaped color check */

					u32Flags |= PATCH_LEFT_EDGE_STRAIGHT;
					fVerticalStraightnessSquared = 0.0f;
#ifdef CENTRAL_DIFF_STRAIGHT_EDGES
					pPatch->d2.Clear();
					pPatch->d6.Clear();
#endif //CENTRAL_DIFF_STRAIGHT_EDGES
				}
			}

			/* right edge straightness check */

			if ((u32Flags & PATCH_RIGHT_EDGE_STRAIGHT) == 0)
			{
				SET_ZOOM_SQUARED(pPatch->v1, pPatch->v2);
				if (fZoomedStraightSquared != 0.0f)
				{
					fTemp = STRAIGHTNESS(pPatch->d5, pPatch->d9);
					fZoomedStraightSquared *= fTemp;
				}

				/* check against straightness tolerance. use zoom factor based on edge's distance to screen. this is */
				/* important to make sure the shared edge of adjacent patches subdivide the same to prevent gaps */

				if (fZoomedStraightSquared < fStraightToleranceSquared)
				{
					/* consider it perfectly straight-- note that it is possible for an edge to be marked straight */
					/* and s-shaped at the same time, so that it can subdivide as a straight edge but still do the */
					/* s-shaped color check */

					u32Flags |= PATCH_RIGHT_EDGE_STRAIGHT;
#ifdef CENTRAL_DIFF_STRAIGHT_EDGES
					pPatch->d5.Clear();
					pPatch->d9.Clear();
#endif //CENTRAL_DIFF_STRAIGHT_EDGES

					/* don't need to clear fTemp since next check only fires if edge wasn't straight */
				}
				else if (fTemp > fVerticalStraightnessSquared)
				{
					/* less straight than other edge, use this value */

					fVerticalStraightnessSquared = fTemp;
				}
			}

			/* top edge straightness check */

			if (u32Flags & PATCH_TOP_EDGE_STRAIGHT)
				fHorizontalStraightnessSquared = 0.0f;
			else
			{
				SET_ZOOM_SQUARED(pPatch->v0, pPatch->v1);
				if (fZoomedStraightSquared != 0.0f)
				{
					fHorizontalStraightnessSquared = STRAIGHTNESS(pPatch->d0, pPatch->d1);
					fZoomedStraightSquared *= fHorizontalStraightnessSquared;
				}

				/* check against straightness tolerance. use zoom factor based on edge's distance to screen. this is */
				/* important to make sure the shared edge of adjacent patches subdivide the same to prevent gaps */

				if (fZoomedStraightSquared < fStraightToleranceSquared)
				{
					/* consider it perfectly straight-- note that it is possible for an edge to be marked straight */
					/* and s-shaped at the same time, so that it can subdivide as a straight edge but still do the */
					/* s-shaped color check */

					u32Flags |= PATCH_TOP_EDGE_STRAIGHT;
					fHorizontalStraightnessSquared = 0.0f;
#ifdef CENTRAL_DIFF_STRAIGHT_EDGES
					pPatch->d0.Clear();
					pPatch->d1.Clear();
#endif //CENTRAL_DIFF_STRAIGHT_EDGES
				}
			}

			/* bottom edge straightness check */

			if ((u32Flags & PATCH_BOTTOM_EDGE_STRAIGHT) == 0)
			{
				SET_ZOOM_SQUARED(pPatch->v3, pPatch->v2);
				if (fZoomedStraightSquared != 0.0f)
				{
					fTemp = STRAIGHTNESS(pPatch->d10, pPatch->d11);
					fZoomedStraightSquared *= fTemp;
				}

				/* check against straightness tolerance. use zoom factor based on edge's distance to screen. this is */
				/* important to make sure the shared edge of adjacent patches subdivide the same to prevent gaps */

				if (fZoomedStraightSquared < fStraightToleranceSquared)
				{
					/* consider it perfectly straight-- note that it is possible for an edge to be marked straight */
					/* and s-shaped at the same time, so that it can subdivide as a straight edge but still do the */
					/* s-shaped color check */

					u32Flags |= PATCH_BOTTOM_EDGE_STRAIGHT;
#ifdef CENTRAL_DIFF_STRAIGHT_EDGES
					pPatch->d10.Clear();
					pPatch->d11.Clear();
#endif //CENTRAL_DIFF_STRAIGHT_EDGES

					/* don't need to clear fTemp since next check only fires if edge wasn't straight */
				}
				else if (fTemp > fHorizontalStraightnessSquared)
				{
					/* less straight than other edge, use this value */

					fHorizontalStraightnessSquared = fTemp;
				}
			}
		}

		/* do i split vertically or horizontally? if all edges straight, i'm done recursing. by using non-zoomed horizontal/vertical */
		/* straightness values, the patch tessellates in roughly the same order no matter what direction it is viewed from. */
		/* the tessellation order can still be affected by an edge being marked as straight, but i don't see an easy fix. */
		/* this mainly affects large patches that have a far edge past the FAR_LOD distance */

		if ((u32Flags & PATCH_ANY_EDGE_STRAIGHT) != PATCH_ANY_EDGE_STRAIGHT)
		{
			/* point into patch stack */

			pPatch2 = &PatchStack[nStackIndex];
			ASSERT((fVerticalStraightnessSquared != 0.0f) || (fHorizontalStraightnessSquared != 0.0f));

			/* decide which way to split */

			if (fVerticalStraightnessSquared > fHorizontalStraightnessSquared)
			{
/**************** SubSplitVertical-- split into a top and bottom patch ***************************************/

				/* shouldn't get here if both edges are both straight-- should divide other way or stop recursing */

				ASSERT(((u32Flags & PATCH_LEFT_EDGE_STRAIGHT) == 0) || ((u32Flags & PATCH_RIGHT_EDGE_STRAIGHT) == 0));

				/* left edge of 3-point patches should always be straight (since the points coincide) */

				ASSERT(((u32Flags & PATCH_3_POINT) == 0) || (u32Flags & PATCH_LEFT_EDGE_STRAIGHT));								// if this hits tell ned!
				ASSERT(((u32Flags & PATCH_3_POINT) == 0) || ((u32Flags & PATCH_LEFT_EDGE_S_SHAPED) == 0));	// if this hits tell ned! (affects normal calculation)
//				ASSERT(((u32Flags & PATCH_3_POINT) == 0) || (memcmp(&pPatch->v0, &pPatch->v3, sizeof(Vector3)) == 0));	// if this hits tell ned!

				/* copy values from original patch that new patch2 will use directly */

				pPatch2->v2 = pPatch->v2;		// bottom right
				pPatch2->v3 = pPatch->v3;		// bottom left
				pPatch2->d10 = pPatch->d10;	// bottom left
				pPatch2->d11 = pPatch->d11;	// bottom right

				/* subdivide left and right edges, calculate 2nd derivatives for new shared edge */

#ifdef CENTRAL_DIFF_STRAIGHT_EDGES
				DIVIDE_EDGE(v0, v3, d2, d6);
				DIVIDE_EDGE(v1, v2, d5, d9);
#else //CENTRAL_DIFF_STRAIGHT_EDGES
				DIVIDE_EDGE(u32Flags, PATCH_LEFT_EDGE_STRAIGHT, v0, v3, d2, d6);
				DIVIDE_EDGE(u32Flags, PATCH_RIGHT_EDGE_STRAIGHT, v1, v2, d5, d9);
#endif //CENTRAL_DIFF_STRAIGHT_EDGES
				FINISH_NEW_EDGE(d0, d1, d10, d11);

				/**************** vertex colors ******************************************************************************/

				/* copy values from original patch that new patch2 will use directly */

				pPatch2->Color[2] = pPatch->Color[2];	// bottom right
				pPatch2->Color[3] = pPatch->Color[3];	// bottom left
#ifdef TOOLS
				/* for data export, subdivide normals */

				pPatch2->Normal[2] = pPatch->Normal[2];	// bottom right-- copy original patch's normal into patch2
				pPatch2->Normal[3] = pPatch->Normal[3];	// bottom left-- copy original patch's normal into patch2
#endif //TOOLS

				/* subdivide left and right edges */

				DIVIDE_EDGE_COLOR(u32Flags, PATCH_LEFT_EDGE_S_SHAPED, S_SHAPED_LEFT_EDGE, PATCH_BOTTOM_LEFT_MIDPOINT_COLOR, 0, 3);
				DIVIDE_EDGE_COLOR(u32Flags, PATCH_RIGHT_EDGE_S_SHAPED, S_SHAPED_RIGHT_EDGE, PATCH_TOP_RIGHT_MIDPOINT_COLOR, 1, 2);

				/**************** uv coordinates *****************************************************************************/

				/* uv coordinates are just averages */

#ifndef TOOLS
				DIVIDE_UV_COORDS(uv0, uv1, uv2, uv3);
#else //TOOLS
				/* for export tool, we divide some edges more accurately */

				if (u32Flags & PATCH_LEFT_EDGE_PROPORTIONAL_UV)
				{
					DIVIDE_UV_COORDS_PROPORTIONAL(uv0, uv3, v0, v3);
				}
				else
				{
					DIVIDE_UV_COORDS(uv0, uv3);
				}
				if (u32Flags & PATCH_RIGHT_EDGE_PROPORTIONAL_UV)
				{
					DIVIDE_UV_COORDS_PROPORTIONAL(uv1, uv2, v1, v2);
				}
				else
				{
					DIVIDE_UV_COORDS(uv1, uv2);
				}
#endif //TOOLS

#ifdef TOOLS
				/**************** color uv coordinates *****************************************************************************/

				/* this is for vertex color interpolation at export time */

				if (u32Flags & PATCH_LEFT_EDGE_PROPORTIONAL_UV)
				{
					DIVIDE_COLOR_UV_COORDS_PROPORTIONAL(ColorUV0, ColorUV3, v0, v3);
				}
				else
				{
					DIVIDE_COLOR_UV_COORDS(ColorUV0, ColorUV3);
				}
				if (u32Flags & PATCH_RIGHT_EDGE_PROPORTIONAL_UV)
				{
					DIVIDE_COLOR_UV_COORDS_PROPORTIONAL(ColorUV1, ColorUV2, v1, v2);
				}
				else
				{
					DIVIDE_COLOR_UV_COORDS(ColorUV1, ColorUV2);
				}
#endif //TOOLS

				/**************** update flags *******************************************************************************/

				/* we have split left and right edges. they are no longer s-shaped (if they ever were) */

				u32Flags &= ~(PATCH_LEFT_EDGE_S_SHAPED | PATCH_RIGHT_EDGE_S_SHAPED);

				/* and recurse for 2 new patches. if both top and bottom edges of original patch are straight, i know new edge is */
   			/* straight too (which skips all s-shaped/pre-calculated height checks) */

				if ((u32Flags & PATCH_TOP_EDGE_STRAIGHT) && (u32Flags & PATCH_BOTTOM_EDGE_STRAIGHT))
				{
					/* record flags that go with Patch2 we just created, update stack index */

					u32FlagsStack[nStackIndex++] = u32Flags;
   			}
				else
   			{
					/* record flags that go with Patch2 we just created, update stack index. we don't know state of patch's */
					/* new top edge */

					u32NewFlags = u32Flags & ~(PATCH_TOP_EDGE_STRAIGHT | PATCH_TOP_EDGE_S_SHAPED);
					if (u32Flags & (PATCH_TOP_EDGE_S_SHAPED | PATCH_BOTTOM_EDGE_S_SHAPED))
   					u32NewFlags |= (PATCH_TOP_EDGE_S_SHAPED | PATCH_TOP_RIGHT_MIDPOINT_COLOR);
					u32FlagsStack[nStackIndex++] = u32NewFlags;

					/* update flags for processing top patch. we don't know state of patch's new bottom edge. we can guess */
					/* as to it's s-shape state, though-- if either original top/bottom edge is s-shaped, have to assume */
					/* new edge is s-shaped. this isn't strictly accurate-- can generate a s-shaped edge by averaging */
					/* control points from two c-shaped edges-- but we'll ignore this case */

   				u32Flags &= ~(PATCH_BOTTOM_EDGE_STRAIGHT | PATCH_BOTTOM_EDGE_S_SHAPED);
					if (u32Flags & (PATCH_TOP_EDGE_S_SHAPED | PATCH_BOTTOM_EDGE_S_SHAPED))
   					u32Flags |= (PATCH_BOTTOM_EDGE_S_SHAPED | PATCH_BOTTOM_LEFT_MIDPOINT_COLOR);
				}

#ifdef TOOLS
				/* update proportional uv flags. if one edge was marked for proportional and other wasn't, new */
				/* shared edge gets marked for proportional */

				if (u32Flags & (PATCH_TOP_EDGE_PROPORTIONAL_UV | PATCH_BOTTOM_EDGE_PROPORTIONAL_UV))
				{
					u32Flags |= PATCH_BOTTOM_EDGE_PROPORTIONAL_UV;
					u32FlagsStack[nStackIndex - 1] |= PATCH_TOP_EDGE_PROPORTIONAL_UV;
				}
#endif //TOOLS

				/* now continue in while loop to process new top patch (which was overwritten into pPatch) */
			}
			else
   		{
/**************** SubSplitHorizonal-- split into a left and right patch *************************************/

				/* shouldn't get here if both edges are both straight-- should divide other way or stop recursing */

				ASSERT(((u32Flags & PATCH_TOP_EDGE_STRAIGHT) == 0) || ((u32Flags & PATCH_BOTTOM_EDGE_STRAIGHT) == 0));

				/* copy values from original patch that new patch2 will use directly */

				pPatch2->v1 = pPatch->v1;		// top right
   			pPatch2->v2 = pPatch->v2;		// bottom right
				pPatch2->d5 = pPatch->d5;		// top right
   			pPatch2->d9 = pPatch->d9;		// bottom right

				/* subdivide top and bottom edges, calculate 2nd derivatives for new shared edge */

#ifdef CENTRAL_DIFF_STRAIGHT_EDGES
				DIVIDE_EDGE(v0, v1, d0, d1);
				DIVIDE_EDGE(v3, v2, d10, d11);
#else //CENTRAL_DIFF_STRAIGHT_EDGES
				DIVIDE_EDGE(u32Flags, PATCH_TOP_EDGE_STRAIGHT, v0, v1, d0, d1);
				DIVIDE_EDGE(u32Flags, PATCH_BOTTOM_EDGE_STRAIGHT, v3, v2, d10, d11);
#endif //CENTRAL_DIFF_STRAIGHT_EDGES
				FINISH_NEW_EDGE(d2, d6, d5, d9);

				/**************** vertex colors ******************************************************************************/

				/* copy values from original patch that new patch2 will use directly */

				pPatch2->Color[1] = pPatch->Color[1];	// top right
   			pPatch2->Color[2] = pPatch->Color[2];	// bottom right
#ifdef TOOLS
				/* for data export, subdivide normals */

				pPatch2->Normal[1] = pPatch->Normal[1];	// top right-- copy original patch's normal into patch2
				pPatch2->Normal[2] = pPatch->Normal[2];	// bottom right-- copy original patch's normal into patch2
#endif //TOOLS

   			/* subdivide top and bottom edges */

				DIVIDE_EDGE_COLOR(u32Flags, PATCH_TOP_EDGE_S_SHAPED, S_SHAPED_TOP_EDGE, PATCH_TOP_RIGHT_MIDPOINT_COLOR, 0, 1);
				DIVIDE_EDGE_COLOR(u32Flags, PATCH_BOTTOM_EDGE_S_SHAPED, S_SHAPED_BOTTOM_EDGE, PATCH_BOTTOM_LEFT_MIDPOINT_COLOR, 3, 2);

				/**************** uv coordinates *****************************************************************************/

				/* uv coordinates are just averages */

#ifndef TOOLS
				DIVIDE_UV_COORDS(uv3, uv0, uv1, uv2);
#else //TOOLS
				/* for export tool, we divide some edges more accurately */

				if (u32Flags & PATCH_TOP_EDGE_PROPORTIONAL_UV)
				{
					DIVIDE_UV_COORDS_PROPORTIONAL(uv0, uv1, v0, v1);
				}
				else
				{
					DIVIDE_UV_COORDS(uv0, uv1);
				}
				if (u32Flags & PATCH_BOTTOM_EDGE_PROPORTIONAL_UV)
				{
					DIVIDE_UV_COORDS_PROPORTIONAL(uv3, uv2, v3, v2);
				}
				else
				{
					DIVIDE_UV_COORDS(uv3, uv2);
				}
#endif //TOOLS

#ifdef TOOLS
				/**************** color uv coordinates *****************************************************************************/

				/* this is for vertex color interpolation at export time */

				if (u32Flags & PATCH_TOP_EDGE_PROPORTIONAL_UV)
				{
					DIVIDE_COLOR_UV_COORDS_PROPORTIONAL(ColorUV0, ColorUV1, v0, v1);
				}
				else
				{
					DIVIDE_COLOR_UV_COORDS(ColorUV0, ColorUV1);
				}
				if (u32Flags & PATCH_BOTTOM_EDGE_PROPORTIONAL_UV)
				{
					DIVIDE_COLOR_UV_COORDS_PROPORTIONAL(ColorUV3, ColorUV2, v3, v2);
				}
				else
				{
					DIVIDE_COLOR_UV_COORDS(ColorUV3, ColorUV2);
				}
#endif //TOOLS

				/**************** update flags *******************************************************************************/

				/* we have split top and bottom edges. they are no longer s-shaped (if they ever were) */

				u32Flags &= ~(PATCH_TOP_EDGE_S_SHAPED | PATCH_BOTTOM_EDGE_S_SHAPED);

				/* and recurse for 2 new patches. if both top and bottom edges of original patch are straight, i know new edge is */
				/* straight too (which skips all s-shaped/pre-calculated height checks) */

   			if ((u32Flags & PATCH_LEFT_EDGE_STRAIGHT) && (u32Flags & PATCH_RIGHT_EDGE_STRAIGHT))
				{
					/* record flags that go with Patch2 we just created, update stack index */

					u32FlagsStack[nStackIndex++] = (u32Flags & ~PATCH_3_POINT);	// right patch is no longer 3-point if it was
   			}
				else
   			{
					/* record flags that go with Patch2 we just created, update stack index. we don't know state of patch's */
					/* new top edge */

					u32NewFlags = u32Flags & ~(PATCH_LEFT_EDGE_STRAIGHT | PATCH_LEFT_EDGE_S_SHAPED | PATCH_3_POINT);
					if (u32Flags & (PATCH_LEFT_EDGE_S_SHAPED | PATCH_RIGHT_EDGE_S_SHAPED))
						u32NewFlags |= (PATCH_LEFT_EDGE_S_SHAPED | PATCH_BOTTOM_LEFT_MIDPOINT_COLOR);
					u32FlagsStack[nStackIndex++] = u32NewFlags;

					/* update flags for processing left patch. we don't know state of patch's new right edge. we can guess */
					/* as to it's s-shape state, though-- if either original left/right edge is s-shaped, have to assume */
					/* new edge is s-shaped. this isn't strictly accurate-- can generate a s-shaped edge by averaging */
					/* control points from two c-shaped edges-- but we'll ignore this case */

					u32Flags &= ~(PATCH_RIGHT_EDGE_STRAIGHT | PATCH_RIGHT_EDGE_S_SHAPED);
					if (u32Flags & (PATCH_LEFT_EDGE_S_SHAPED | PATCH_RIGHT_EDGE_S_SHAPED))
						u32Flags |= (PATCH_RIGHT_EDGE_S_SHAPED | PATCH_TOP_RIGHT_MIDPOINT_COLOR);
				}

#ifdef TOOLS
				/* update proportional uv flags. if one edge was marked for proportional and other wasn't, new */
				/* shared edge gets marked for proportional */

				if (u32Flags & (PATCH_LEFT_EDGE_PROPORTIONAL_UV | PATCH_RIGHT_EDGE_PROPORTIONAL_UV))
				{
					u32Flags |= PATCH_RIGHT_EDGE_PROPORTIONAL_UV;
					u32FlagsStack[nStackIndex - 1] |= PATCH_LEFT_EDGE_PROPORTIONAL_UV;
				}
#endif //TOOLS

				/* now continue in while loop to process new left patch (which was overwritten into pPatch) */
			}
   	}
		else
		{
/*************************** all edges are straight, done with this branch of tessellation ***********************/

#ifdef TOOLS
			/* may be pre-tessellating for high-poly models */

			if (pTessPatch != NULL)	// if buffer is active, we are tessellating for high-poly triangle strips
			{
				/* room? */

				if (nTessQuadCount + nTessTriCount >= nMaxPolysPerPatch)
				{
					_ASSERT(FALSE);
					bTessellationError = TRUE;
					return;	// no point continuing
				}
				else
				{
					/* record patch data for later triangle stripping. typically, this patch will be treated as */
					/* a polygon from here on out, but record patch data for possible midpoint checks */

					memcpy(&pTessPatch->RenderPatch, pPatch, sizeof(ts_RenderPatch));
					pTessPatch->u16Flags = (u16) u32Flags;	// upper 16 bits are tessellator-only flags. see SOM.h
					pTessPatch++;
					if (u32Flags & PATCH_3_POINT)
						nTessTriCount++;
					else
						nTessQuadCount++;
				}
			}
			else
			{
				/* record bounds for patch bounding box calculations for data export */

				BoundingBox__Include(&PatchBoundingBox, pPatch->v0);
	   		BoundingBox__Include(&PatchBoundingBox, pPatch->v1);
				BoundingBox__Include(&PatchBoundingBox, pPatch->v2);
	   		if ((u32Flags & PATCH_3_POINT) == 0)
					BoundingBox__Include(&PatchBoundingBox, pPatch->v3);

				/* record if patch got tessellated at small collision zoom value */

				if (nStackIndex != 0)
	   			bCollisionPatchSplit = TRUE;

				/* count how many triangles and vertices are generated by collision tessellation */

				if (u32Flags & PATCH_3_POINT)
	   		{
					u32CollisionTriangleCount++;
	   			u32CollisionVertexCount += 3;
				}
	   		else
				{
	   			u32CollisionTriangleCount += 2;
					u32CollisionVertexCount += 4;
	   		}
			}
#else //TOOLS
			/* Cull patches behind camera. camera points down negative z-axis, so clip plane z value is negative */

			if ((pPatch->v0.z > fNegativeNearClip) && (pPatch->v1.z > fNegativeNearClip) && (pPatch->v3.z > fNegativeNearClip) && (pPatch->v2.z > fNegativeNearClip))
				goto RenderDone; // vertices have not been projected so valid z's are less than -1;

			/* record poly for later rendering. treat all polys as quads-- it's simpler, and just as fast */

#ifdef WIN32
			memcpy(pRenderVertex, &pPatch->v0, 4 * sizeof(Vector3));
#elif defined(PS2)
			memcpy(pRenderVertex, &pPatch->v0, 4 * sizeof(Vector4));
#elif defined(GCN)
			memcpy(pRenderVertex, &pPatch->v0, 4 * sizeof(Vector3));
#endif //WIN32/PS2
			pRenderVertex += 4;
			{
				int i;
				for (i = 0; i < nPatchTextureCount; i++)
				{
					memcpy(pRenderUV[i]++, &pPatch->uv0[i], sizeof(Vector2));
					memcpy(pRenderUV[i]++, &pPatch->uv1[i], sizeof(Vector2));
					memcpy(pRenderUV[i]++, &pPatch->uv2[i], sizeof(Vector2));
					memcpy(pRenderUV[i]++, &pPatch->uv3[i], sizeof(Vector2));
				}
			}
			if (u32Flags & (PATCH_RENDER_DO_LIGHTING | PATCH_RENDER_HAS_VERTEXCOLORS))
			{
//yuch-- ps2 color range is 0..255 for glDrawArrays
#ifdef yuchPS2	//yuch-- only do this if we get glDrawArrays working
				pPatch->Color[0].R *= 255.0f;
				pPatch->Color[0].G *= 255.0f;
				pPatch->Color[0].B *= 255.0f;
				pPatch->Color[0].A *= 255.0f;
				pPatch->Color[1].R *= 255.0f;
				pPatch->Color[1].G *= 255.0f;
				pPatch->Color[1].B *= 255.0f;
				pPatch->Color[1].A *= 255.0f;
				pPatch->Color[2].R *= 255.0f;
				pPatch->Color[2].G *= 255.0f;
				pPatch->Color[2].B *= 255.0f;
				pPatch->Color[2].A *= 255.0f;
				pPatch->Color[3].R *= 255.0f;
				pPatch->Color[3].G *= 255.0f;
				pPatch->Color[3].B *= 255.0f;
				pPatch->Color[3].A *= 255.0f;
#endif //PS2
				memcpy(pRenderColor++, &pPatch->Color[0], sizeof(ts_fRGBA));
				memcpy(pRenderColor++, &pPatch->Color[1], sizeof(ts_fRGBA));
				memcpy(pRenderColor++, &pPatch->Color[2], sizeof(ts_fRGBA));
				memcpy(pRenderColor++, &pPatch->Color[3], sizeof(ts_fRGBA));
			}

RenderDone:
#endif //TOOLS

			/* once we have reached end of tessellation branch, decrement stack index and proceed down other path */


#if 0
			/* this version represents the PS2 flow, since we will want to load the next patch to process into registers, */
			/* and also use the stack most efficiently */

			if (--nStackIndex >= 0)
			{
				*pPatch = PatchStack[nStackIndex];		// copy from stack into working patch
				u32Flags = u32FlagsStack[nStackIndex];	// restore flags for this patch
			}
#else //0
			/* this version is for the pc-- just process the patch on place in the stack */

			nStackIndex--;

			/* if we have already processed this patch, decrement again */

			while ((nStackIndex >= 0) && (u32FlagsStack[nStackIndex] == 0xffffffff))
				nStackIndex--;
			if (nStackIndex >= 0)
			{
				pPatch = &PatchStack[nStackIndex];		// copy from stack into working patch
				u32Flags = u32FlagsStack[nStackIndex];	// restore flags for this patch

				/* mark this patch as having been processed, and increment the stack pointer */

				u32FlagsStack[nStackIndex] = 0xffffffff;
				nStackIndex++;
			}
#endif //0
		}
	}
}

#ifdef TOOLS
/* a very simplified version of the tessellator to determine a patch's midpoint. since the patch surface is implicit
(it depends on the order of subdivision), let the tessellator do the first subdivision as normal to match the
tessellation of the patch into a high poly model, then force one of the resulting patches to subdivide the other way
so that we get the midpoint */

void GetPatchMidpoint(ts_RenderPatch *pPatch, u32 u32Flags, Vector3 *pMidpoint)
{
	float fTemp;
	float fHorizontalStraightnessSquared;	// greater of unzoomed height of top, bottom edges, to decide tessellation order
	float fVerticalStraightnessSquared;		// greater of unzoomed height of left, right edges, to decide tessellation order
	float fZoomedStraightSquared;				// zoomed straightness value, for straightness tolerance test for each edge
	ts_RenderPatch *pPatch2;					// pointer into patch stack

	/* copy passed patch onto patch stack so we don't overwrite it */

	memcpy(&PatchStack[0], pPatch, sizeof(ts_RenderPatch));
	pPatch = &PatchStack[0];	// new location of original patch
	pPatch2 = &PatchStack[1];	// for subdivision

/*************************** check edges for straightness **********************************************/

	/* left edge straightness check */

	if (u32Flags & PATCH_LEFT_EDGE_STRAIGHT)
		fVerticalStraightnessSquared = 0.0f;
	else
	{
		SET_ZOOM_SQUARED(pPatch->v0, pPatch->v3);

		if (fZoomedStraightSquared != 0.0f)
		{
			fVerticalStraightnessSquared = STRAIGHTNESS(pPatch->d2, pPatch->d6);
			fZoomedStraightSquared *= fVerticalStraightnessSquared;
		}

		/* check against straightness tolerance. use zoom factor based on edge's distance to screen. this is */
		/* important to make sure the shared edge of adjacent patches subdivide the same to prevent gaps */

		if (fZoomedStraightSquared < fStraightToleranceSquared)
		{
			/* consider it perfectly straight-- note that it is possible for an edge to be marked straight */
			/* and s-shaped at the same time, so that it can subdivide as a straight edge but still do the */
			/* s-shaped color check */

			u32Flags |= PATCH_LEFT_EDGE_STRAIGHT;
			fVerticalStraightnessSquared = 0.0f;
#ifdef CENTRAL_DIFF_STRAIGHT_EDGES
			pPatch->d2.Clear();
			pPatch->d6.Clear();
#endif //CENTRAL_DIFF_STRAIGHT_EDGES
		}
	}

	/* right edge straightness check */

	if ((u32Flags & PATCH_RIGHT_EDGE_STRAIGHT) == 0)
	{
		SET_ZOOM_SQUARED(pPatch->v1, pPatch->v2);
		if (fZoomedStraightSquared != 0.0f)
		{
			fTemp = STRAIGHTNESS(pPatch->d5, pPatch->d9);
			fZoomedStraightSquared *= fTemp;
		}

		/* check against straightness tolerance. use zoom factor based on edge's distance to screen. this is */
		/* important to make sure the shared edge of adjacent patches subdivide the same to prevent gaps */

		if (fZoomedStraightSquared < fStraightToleranceSquared)
		{
			/* consider it perfectly straight-- note that it is possible for an edge to be marked straight */
			/* and s-shaped at the same time, so that it can subdivide as a straight edge but still do the */
			/* s-shaped color check */

			u32Flags |= PATCH_RIGHT_EDGE_STRAIGHT;
#ifdef CENTRAL_DIFF_STRAIGHT_EDGES
			pPatch->d5.Clear();
			pPatch->d9.Clear();
#endif //CENTRAL_DIFF_STRAIGHT_EDGES

			/* don't need to clear fTemp since next check only fires if edge wasn't straight */
		}
		else if (fTemp > fVerticalStraightnessSquared)
		{
			/* less straight than other edge, use this value */

			fVerticalStraightnessSquared = fTemp;
		}
	}

	/* top edge straightness check */

	if (u32Flags & PATCH_TOP_EDGE_STRAIGHT)
		fHorizontalStraightnessSquared = 0.0f;
	else
	{
		SET_ZOOM_SQUARED(pPatch->v0, pPatch->v1);
		if (fZoomedStraightSquared != 0.0f)
		{
			fHorizontalStraightnessSquared = STRAIGHTNESS(pPatch->d0, pPatch->d1);
			fZoomedStraightSquared *= fHorizontalStraightnessSquared;
		}

		/* check against straightness tolerance. use zoom factor based on edge's distance to screen. this is */
		/* important to make sure the shared edge of adjacent patches subdivide the same to prevent gaps */

		if (fZoomedStraightSquared < fStraightToleranceSquared)
		{
			/* consider it perfectly straight-- note that it is possible for an edge to be marked straight */
			/* and s-shaped at the same time, so that it can subdivide as a straight edge but still do the */
			/* s-shaped color check */

			u32Flags |= PATCH_TOP_EDGE_STRAIGHT;
			fHorizontalStraightnessSquared = 0.0f;
#ifdef CENTRAL_DIFF_STRAIGHT_EDGES
			pPatch->d0.Clear();
			pPatch->d1.Clear();
#endif //CENTRAL_DIFF_STRAIGHT_EDGES
		}
	}

	/* bottom edge straightness check */

	if ((u32Flags & PATCH_BOTTOM_EDGE_STRAIGHT) == 0)
	{
		SET_ZOOM_SQUARED(pPatch->v3, pPatch->v2);
		if (fZoomedStraightSquared != 0.0f)
		{
			fTemp = STRAIGHTNESS(pPatch->d10, pPatch->d11);
			fZoomedStraightSquared *= fTemp;
		}

		/* check against straightness tolerance. use zoom factor based on edge's distance to screen. this is */
		/* important to make sure the shared edge of adjacent patches subdivide the same to prevent gaps */

		if (fZoomedStraightSquared < fStraightToleranceSquared)
		{
			/* consider it perfectly straight-- note that it is possible for an edge to be marked straight */
			/* and s-shaped at the same time, so that it can subdivide as a straight edge but still do the */
			/* s-shaped color check */

			u32Flags |= PATCH_BOTTOM_EDGE_STRAIGHT;
#ifdef CENTRAL_DIFF_STRAIGHT_EDGES
			pPatch->d10.Clear();
			pPatch->d11.Clear();
#endif //CENTRAL_DIFF_STRAIGHT_EDGES

			/* don't need to clear fTemp since next check only fires if edge wasn't straight */
		}
		else if (fTemp > fHorizontalStraightnessSquared)
		{
			/* less straight than other edge, use this value */

			fHorizontalStraightnessSquared = fTemp;
		}
	}

	if ((u32Flags & PATCH_ANY_EDGE_STRAIGHT) == PATCH_ANY_EDGE_STRAIGHT)
	{
/*************************** all edges of original patch are straight ***********************/
		if (u32Flags & PATCH_3_POINT)
			*pMidpoint = (pPatch->v0 + pPatch->v1 + pPatch->v2) / 3.0f;
		else
			*pMidpoint = (pPatch->v0 + pPatch->v1 + pPatch->v2 + pPatch->v3) / 4.0f;
		return;
	}

	/* split vertically or horizontally */

	ASSERT((fVerticalStraightnessSquared != 0.0f) || (fHorizontalStraightnessSquared != 0.0f));

	/* decide which way to split */

	if (fVerticalStraightnessSquared > fHorizontalStraightnessSquared)
	{
/**************** SubSplitVertical-- split into a top and bottom patch ***************************************/

		/* shouldn't get here if both edges are both straight-- should divide other way or stop recursing */

		ASSERT(((u32Flags & PATCH_LEFT_EDGE_STRAIGHT) == 0) || ((u32Flags & PATCH_RIGHT_EDGE_STRAIGHT) == 0));

		/* left edge of 3-point patches should always be straight (since the points coincide) */

		ASSERT(((u32Flags & PATCH_3_POINT) == 0) || (u32Flags & PATCH_LEFT_EDGE_STRAIGHT));								// if this hits tell ned!
		ASSERT(((u32Flags & PATCH_3_POINT) == 0) || ((u32Flags & PATCH_LEFT_EDGE_S_SHAPED) == 0));	// if this hits tell ned! (affects normal calculation)
//		ASSERT(((u32Flags & PATCH_3_POINT) == 0) || (memcmp(&pPatch->v0, &pPatch->v3, sizeof(Vector3)) == 0));	// if this hits tell ned!

		/* copy values from original patch that new patch2 will use directly */

		pPatch2->v2 = pPatch->v2;		// bottom right
		pPatch2->v3 = pPatch->v3;		// bottom left
		pPatch2->d10 = pPatch->d10;	// bottom left
		pPatch2->d11 = pPatch->d11;	// bottom right

		/* subdivide left and right edges, calculate 2nd derivatives for new shared edge */

#ifdef CENTRAL_DIFF_STRAIGHT_EDGES
		DIVIDE_EDGE(v0, v3, d2, d6);
		DIVIDE_EDGE(v1, v2, d5, d9);
#else //CENTRAL_DIFF_STRAIGHT_EDGES
		DIVIDE_EDGE(u32Flags, PATCH_LEFT_EDGE_STRAIGHT, v0, v3, d2, d6);
		DIVIDE_EDGE(u32Flags, PATCH_RIGHT_EDGE_STRAIGHT, v1, v2, d5, d9);
#endif //CENTRAL_DIFF_STRAIGHT_EDGES
		FINISH_NEW_EDGE(d0, d1, d10, d11);

		/* then subdivide shared edge-- bottom edge of patch1. midpoint is original patch's midpoint */

		DIVIDE_EDGE(v3, v2, d10, d11);
		*pMidpoint = pPatch->v2;	// midpoint is new bottom right vertex
	}
	else
	{
/**************** SubSplitHorizonal-- split into a left and right patch *************************************/

		/* shouldn't get here if both edges are both straight-- should divide other way or stop recursing */

		ASSERT(((u32Flags & PATCH_TOP_EDGE_STRAIGHT) == 0) || ((u32Flags & PATCH_BOTTOM_EDGE_STRAIGHT) == 0));

		/* copy values from original patch that new patch2 will use directly */

		pPatch2->v1 = pPatch->v1;		// top right
		pPatch2->v2 = pPatch->v2;		// bottom right
		pPatch2->d5 = pPatch->d5;		// top right
		pPatch2->d9 = pPatch->d9;		// bottom right

		/* subdivide top and bottom edges, calculate 2nd derivatives for new shared edge */

#ifdef CENTRAL_DIFF_STRAIGHT_EDGES
		DIVIDE_EDGE(v0, v1, d0, d1);
		DIVIDE_EDGE(v3, v2, d10, d11);
#else //CENTRAL_DIFF_STRAIGHT_EDGES
		DIVIDE_EDGE(u32Flags, PATCH_TOP_EDGE_STRAIGHT, v0, v1, d0, d1);
		DIVIDE_EDGE(u32Flags, PATCH_BOTTOM_EDGE_STRAIGHT, v3, v2, d10, d11);
#endif //CENTRAL_DIFF_STRAIGHT_EDGES
		FINISH_NEW_EDGE(d2, d6, d5, d9);

		/* then subdivide shared edge-- right edge of patch1. midpoint is original patch's midpoint */

		DIVIDE_EDGE(v1, v2, d5, d9);
		*pMidpoint = pPatch->v2;	// midpoint is new bottom right vertex
	}
}
#endif //TOOLS

/* set the straightness tolerance value for this tesselation. passing 1.0f uses default value. increasing bias multiplier
yields fewer tessellations (since splines pass the straightness test earlier). decreasing multiplier yields more
tessellations */

void SetStraightTolerance(float fBiasMultiplier)
{
	fStraightToleranceSquared = (DEFAULT_STRAIGHT_TOLERANCE * DEFAULT_STRAIGHT_TOLERANCE) * (fBiasMultiplier * fBiasMultiplier);
}

/* turn forced zoom on/off */

void SetForcedZoom(bool bOnOff, float fValue)
{
	bForceZoom = bOnOff;
	fForceZoomSquared = fValue * fValue;
}

/* set negative near clip plane value for tessellator */

void SetNegativeNearClip(float fNearClip)
{
	fNegativeNearClip = -fNearClip;
}

/* fill in a self-contained, render-ready patch structure. patch layout at this point (v = vertex, cv = control vertex):

               edge 0
     v0 +----->......<-----+ v1
        |    cv0    cv1    |
        |                  |
        |                  |
        v cv2          cv5 v
        .                  .
 edge 3 .                  . edge 1
        .                  .
        ^ cv6          cv9 ^
        |                  |
        |                  |
        |    cv10   cv11   |
     v3 +----->......<-----+ v2
               edge 2

control vertices 3, 4, 7, 8 are unused */

//yuch-- should be inline?
void CRenderPatch__Fill(ts_RenderPatch *pRenderPatch, u32 u32RenderFlags, ts_Patch *pPatch, VCT_TYPE *pVertexList, VCT_TYPE *pControlVertexList, VCT_TYPE *pNormalList, Vector2 *pUVList)
{
	int i;

	/* vertices */

//yuch-- fix this
//	pRenderPatch->v0 = pVertexList[pPatch->usVertexIndex[0]];
//	pRenderPatch->v1 = pVertexList[pPatch->usVertexIndex[1]];
//	pRenderPatch->v2 = pVertexList[pPatch->usVertexIndex[2]];
//	pRenderPatch->v3 = pVertexList[pPatch->usVertexIndex[3]];
	memcpy(&pRenderPatch->v0, &pVertexList[pPatch->usVertexIndex[0]], sizeof(Vector3));
	memcpy(&pRenderPatch->v1, &pVertexList[pPatch->usVertexIndex[1]], sizeof(Vector3));
	memcpy(&pRenderPatch->v2, &pVertexList[pPatch->usVertexIndex[2]], sizeof(Vector3));
	memcpy(&pRenderPatch->v3, &pVertexList[pPatch->usVertexIndex[3]], sizeof(Vector3));

	/* 3-point patch should always have v0 == v3 */
	
	ASSERT(((pPatch->usFlags & PATCH_3_POINT) == 0) || (pPatch->usVertexIndex[0] == pPatch->usVertexIndex[3]));
	
	/* calculate 2nd derivatives for central differencing. equations are:

	at t = 0:

	2nd:	6 * p0  -  12 * p1  +  6 * p2

	at t = 1:

	2nd:	6 * p1  -  12 * p2  +  6 * p3 */

#define SECOND_DERIVATIVE(D, P0, P1, P2) D = 6.0f * (((Vector3) P0 - (2.0f * (Vector3) P1)) + (Vector3) P2);
#ifdef CENTRAL_DIFF_STRAIGHT_EDGES
	if (u32RenderFlags & PATCH_ANY_EDGE_STRAIGHT)
	{
		/* one or more straight edges, test each edge */

		if (u32RenderFlags & PATCH_TOP_EDGE_STRAIGHT)
		{
			pRenderPatch->d0.Clear();
			pRenderPatch->d1.Clear();
		}
		else
		{
			SECOND_DERIVATIVE(pRenderPatch->d0, pRenderPatch->v0, pControlVertexList[pPatch->usControlVertexIndex[CV0]], pControlVertexList[pPatch->usControlVertexIndex[CV1]]);
			SECOND_DERIVATIVE(pRenderPatch->d1, pControlVertexList[pPatch->usControlVertexIndex[CV0]], pControlVertexList[pPatch->usControlVertexIndex[CV1]], pRenderPatch->v1);
		}
		if (u32RenderFlags & PATCH_RIGHT_EDGE_STRAIGHT)
		{
			pRenderPatch->d5.Clear();
			pRenderPatch->d9.Clear();
		}
		else
		{
			SECOND_DERIVATIVE(pRenderPatch->d5, pRenderPatch->v1, pControlVertexList[pPatch->usControlVertexIndex[CV5]], pControlVertexList[pPatch->usControlVertexIndex[CV9]]);
			SECOND_DERIVATIVE(pRenderPatch->d9, pControlVertexList[pPatch->usControlVertexIndex[CV5]], pControlVertexList[pPatch->usControlVertexIndex[CV9]], pRenderPatch->v2);
		}
		if (u32RenderFlags & PATCH_BOTTOM_EDGE_STRAIGHT)
		{
			pRenderPatch->d10.Clear();
			pRenderPatch->d11.Clear();
		}
		else
		{
			SECOND_DERIVATIVE(pRenderPatch->d11, pRenderPatch->v2, pControlVertexList[pPatch->usControlVertexIndex[CV11]], pControlVertexList[pPatch->usControlVertexIndex[CV10]]);
			SECOND_DERIVATIVE(pRenderPatch->d10, pControlVertexList[pPatch->usControlVertexIndex[CV11]], pControlVertexList[pPatch->usControlVertexIndex[CV10]], pRenderPatch->v3);
		}
		if (u32RenderFlags & PATCH_LEFT_EDGE_STRAIGHT)
		{
			pRenderPatch->d2.Clear();
			pRenderPatch->d6.Clear();
		}
		else
		{
			SECOND_DERIVATIVE(pRenderPatch->d6, pRenderPatch->v3, pControlVertexList[pPatch->usControlVertexIndex[CV6]], pControlVertexList[pPatch->usControlVertexIndex[CV2]]);
			SECOND_DERIVATIVE(pRenderPatch->d2, pControlVertexList[pPatch->usControlVertexIndex[CV6]], pControlVertexList[pPatch->usControlVertexIndex[CV2]], pRenderPatch->v0);
		}
	}
	else
#endif //CENTRAL_DIFF_STRAIGHT_EDGES
	{
		/* no straight edges, calculate all 2nd derivatives */

		SECOND_DERIVATIVE(pRenderPatch->d0, pRenderPatch->v0, pControlVertexList[pPatch->usControlVertexIndex[CV0]], pControlVertexList[pPatch->usControlVertexIndex[CV1]]);
		SECOND_DERIVATIVE(pRenderPatch->d1, pControlVertexList[pPatch->usControlVertexIndex[CV0]], pControlVertexList[pPatch->usControlVertexIndex[CV1]], pRenderPatch->v1);
		SECOND_DERIVATIVE(pRenderPatch->d5, pRenderPatch->v1, pControlVertexList[pPatch->usControlVertexIndex[CV5]], pControlVertexList[pPatch->usControlVertexIndex[CV9]]);
		SECOND_DERIVATIVE(pRenderPatch->d9, pControlVertexList[pPatch->usControlVertexIndex[CV5]], pControlVertexList[pPatch->usControlVertexIndex[CV9]], pRenderPatch->v2);
		SECOND_DERIVATIVE(pRenderPatch->d11, pRenderPatch->v2, pControlVertexList[pPatch->usControlVertexIndex[CV11]], pControlVertexList[pPatch->usControlVertexIndex[CV10]]);
		SECOND_DERIVATIVE(pRenderPatch->d10, pControlVertexList[pPatch->usControlVertexIndex[CV11]], pControlVertexList[pPatch->usControlVertexIndex[CV10]], pRenderPatch->v3);
		SECOND_DERIVATIVE(pRenderPatch->d6, pRenderPatch->v3, pControlVertexList[pPatch->usControlVertexIndex[CV6]], pControlVertexList[pPatch->usControlVertexIndex[CV2]]);
		SECOND_DERIVATIVE(pRenderPatch->d2, pControlVertexList[pPatch->usControlVertexIndex[CV6]], pControlVertexList[pPatch->usControlVertexIndex[CV2]], pRenderPatch->v0);
	}

	/* fill in uv coords */
	
	for (i = 0; i < nPatchTextureCount; i++)
	{
  		/* fill in uv's from list */

  		pRenderPatch->uv0[i] = pUVList[pPatch->usUV0Index[i]];
  		pRenderPatch->uv1[i] = pUVList[pPatch->usUV1Index[i]];
  		pRenderPatch->uv2[i] = pUVList[pPatch->usUV2Index[i]];
  		pRenderPatch->uv3[i] = pUVList[pPatch->usUV3Index[i]];
	}
}

#ifdef TOOLS
void CRenderPatch__FillVct3f(ts_RenderPatch *pRenderPatch, u32 u32RenderFlags, ts_BuildPatch *pBuildPatch, Vector3 *pVertexList, Vector3 *pControlVertexList, Vector3 *pNormalList, Vector2 *pUVList)
{
	int i;
	static Vector3 DummyNormal = Vector3(1.0f, 0.0f, 0.0f);

	/* vertices */

//yuch-- fix this
//	pRenderPatch->v0 = pVertexList[pPatch->usVertexIndex[0]];
//	pRenderPatch->v1 = pVertexList[pPatch->usVertexIndex[1]];
//	pRenderPatch->v2 = pVertexList[pPatch->usVertexIndex[2]];
//	pRenderPatch->v3 = pVertexList[pPatch->usVertexIndex[3]];
	memcpy(&pRenderPatch->v0, &pVertexList[pBuildPatch->lVertexID[0]], sizeof(Vector3));
	memcpy(&pRenderPatch->v1, &pVertexList[pBuildPatch->lVertexID[1]], sizeof(Vector3));
	memcpy(&pRenderPatch->v2, &pVertexList[pBuildPatch->lVertexID[2]], sizeof(Vector3));
	memcpy(&pRenderPatch->v3, &pVertexList[pBuildPatch->lVertexID[3]], sizeof(Vector3));

	/* 3-point patch should always have v0 == v3 */
	
	ASSERT(((pBuildPatch->ulFlags & PATCH_3_POINT) == 0) || (pBuildPatch->lVertexID[0] == pBuildPatch->lVertexID[3]));

	/* control vertices-- useful for data export (currently used to set proportional uv flags) */

	if ((pBuildPatch->ulFlags & PATCH_IS_POLY) == 0)	// no control vertices for polys
	{
		memcpy(&pRenderPatch->cv0, &pControlVertexList[pBuildPatch->lControlVertexID[CV0]], sizeof(Vector3));
		memcpy(&pRenderPatch->cv1, &pControlVertexList[pBuildPatch->lControlVertexID[CV1]], sizeof(Vector3));
		memcpy(&pRenderPatch->cv2, &pControlVertexList[pBuildPatch->lControlVertexID[CV2]], sizeof(Vector3));
		memcpy(&pRenderPatch->cv5, &pControlVertexList[pBuildPatch->lControlVertexID[CV5]], sizeof(Vector3));
		memcpy(&pRenderPatch->cv6, &pControlVertexList[pBuildPatch->lControlVertexID[CV6]], sizeof(Vector3));
		memcpy(&pRenderPatch->cv9, &pControlVertexList[pBuildPatch->lControlVertexID[CV9]], sizeof(Vector3));
		memcpy(&pRenderPatch->cv10, &pControlVertexList[pBuildPatch->lControlVertexID[CV10]], sizeof(Vector3));
		memcpy(&pRenderPatch->cv11, &pControlVertexList[pBuildPatch->lControlVertexID[CV11]], sizeof(Vector3));
	}

	/* normals-- for data export, we want to subdivide normals while tessellating patches that will be tri-stripped */

	if (pNormalList)
	{
		memcpy(&pRenderPatch->Normal[0], &pNormalList[pBuildPatch->lNormalID[0]], sizeof(Vector3));
		memcpy(&pRenderPatch->Normal[1], &pNormalList[pBuildPatch->lNormalID[1]], sizeof(Vector3));
		memcpy(&pRenderPatch->Normal[2], &pNormalList[pBuildPatch->lNormalID[2]], sizeof(Vector3));
		memcpy(&pRenderPatch->Normal[3], &pNormalList[pBuildPatch->lNormalID[3]], sizeof(Vector3));

		/* and fill in pre-calculated s-shaped normals too */

		for (i = 0; i < S_SHAPED_MAX_ENTRIES; i++)
		{
			if (pBuildPatch->lSShapedNormalID[i] != -1)
				memcpy(&SShapedNormal[i], &pNormalList[pBuildPatch->lSShapedNormalID[i]], sizeof(Vector3));
			else
				memcpy(&SShapedNormal[i], &DummyNormal, sizeof(Vector3));
		}
	}
	else
	{
		memcpy(&pRenderPatch->Normal[0], &DummyNormal, sizeof(Vector3));
		memcpy(&pRenderPatch->Normal[1], &DummyNormal, sizeof(Vector3));
		memcpy(&pRenderPatch->Normal[2], &DummyNormal, sizeof(Vector3));
		memcpy(&pRenderPatch->Normal[3], &DummyNormal, sizeof(Vector3));

		/* and fill in pre-calculated s-shaped normals too */

		for (i = 0; i < S_SHAPED_MAX_ENTRIES; i++)
			memcpy(&SShapedNormal[i], &DummyNormal, sizeof(Vector3));
	}

	/* calculate 2nd derivatives for central differencing. equations are:

	at t = 0:

	2nd:	6 * p0  -  12 * p1  +  6 * p2

	at t = 1:

	2nd:	6 * p1  -  12 * p2  +  6 * p3 */

#define SECOND_DERIVATIVE(D, P0, P1, P2) D = 6.0f * (((Vector3) P0 - (2.0f * (Vector3) P1)) + (Vector3) P2);
#ifdef CENTRAL_DIFF_STRAIGHT_EDGES
	if (u32RenderFlags & PATCH_ANY_EDGE_STRAIGHT)
	{
		/* one or more straight edges, test each edge */

		if (u32RenderFlags & PATCH_TOP_EDGE_STRAIGHT)
		{
			pRenderPatch->d0.Clear();
			pRenderPatch->d1.Clear();
		}
		else
		{
			SECOND_DERIVATIVE(pRenderPatch->d0, pRenderPatch->v0, pControlVertexList[pBuildPatch->lControlVertexID[CV0]], pControlVertexList[pBuildPatch->lControlVertexID[CV1]]);
			SECOND_DERIVATIVE(pRenderPatch->d1, pControlVertexList[pBuildPatch->lControlVertexID[CV0]], pControlVertexList[pBuildPatch->lControlVertexID[CV1]], pRenderPatch->v1);
		}
		if (u32RenderFlags & PATCH_RIGHT_EDGE_STRAIGHT)
		{
			pRenderPatch->d5.Clear();
			pRenderPatch->d9.Clear();
		}
		else
		{
			SECOND_DERIVATIVE(pRenderPatch->d5, pRenderPatch->v1, pControlVertexList[pBuildPatch->lControlVertexID[CV5]], pControlVertexList[pBuildPatch->lControlVertexID[CV9]]);
			SECOND_DERIVATIVE(pRenderPatch->d9, pControlVertexList[pBuildPatch->lControlVertexID[CV5]], pControlVertexList[pBuildPatch->lControlVertexID[CV9]], pRenderPatch->v2);
		}
		if (u32RenderFlags & PATCH_BOTTOM_EDGE_STRAIGHT)
		{
			pRenderPatch->d10.Clear();
			pRenderPatch->d11.Clear();
		}
		else
		{
			SECOND_DERIVATIVE(pRenderPatch->d11, pRenderPatch->v2, pControlVertexList[pBuildPatch->lControlVertexID[CV11]], pControlVertexList[pBuildPatch->lControlVertexID[CV10]]);
			SECOND_DERIVATIVE(pRenderPatch->d10, pControlVertexList[pBuildPatch->lControlVertexID[CV11]], pControlVertexList[pBuildPatch->lControlVertexID[CV10]], pRenderPatch->v3);
		}
		if (u32RenderFlags & PATCH_LEFT_EDGE_STRAIGHT)
		{
			pRenderPatch->d2.Clear();
			pRenderPatch->d6.Clear();
		}
		else
		{
			SECOND_DERIVATIVE(pRenderPatch->d6, pRenderPatch->v3, pControlVertexList[pBuildPatch->lControlVertexID[CV6]], pControlVertexList[pBuildPatch->lControlVertexID[CV2]]);
			SECOND_DERIVATIVE(pRenderPatch->d2, pControlVertexList[pBuildPatch->lControlVertexID[CV6]], pControlVertexList[pBuildPatch->lControlVertexID[CV2]], pRenderPatch->v0);
		}
	}
	else
#endif //CENTRAL_DIFF_STRAIGHT_EDGES
	{
		/* no straight edges, calculate all 2nd derivatives */

		SECOND_DERIVATIVE(pRenderPatch->d0, pRenderPatch->v0, pControlVertexList[pBuildPatch->lControlVertexID[CV0]], pControlVertexList[pBuildPatch->lControlVertexID[CV1]]);
		SECOND_DERIVATIVE(pRenderPatch->d1, pControlVertexList[pBuildPatch->lControlVertexID[CV0]], pControlVertexList[pBuildPatch->lControlVertexID[CV1]], pRenderPatch->v1);
		SECOND_DERIVATIVE(pRenderPatch->d5, pRenderPatch->v1, pControlVertexList[pBuildPatch->lControlVertexID[CV5]], pControlVertexList[pBuildPatch->lControlVertexID[CV9]]);
		SECOND_DERIVATIVE(pRenderPatch->d9, pControlVertexList[pBuildPatch->lControlVertexID[CV5]], pControlVertexList[pBuildPatch->lControlVertexID[CV9]], pRenderPatch->v2);
		SECOND_DERIVATIVE(pRenderPatch->d11, pRenderPatch->v2, pControlVertexList[pBuildPatch->lControlVertexID[CV11]], pControlVertexList[pBuildPatch->lControlVertexID[CV10]]);
		SECOND_DERIVATIVE(pRenderPatch->d10, pControlVertexList[pBuildPatch->lControlVertexID[CV11]], pControlVertexList[pBuildPatch->lControlVertexID[CV10]], pRenderPatch->v3);
		SECOND_DERIVATIVE(pRenderPatch->d6, pRenderPatch->v3, pControlVertexList[pBuildPatch->lControlVertexID[CV6]], pControlVertexList[pBuildPatch->lControlVertexID[CV2]]);
		SECOND_DERIVATIVE(pRenderPatch->d2, pControlVertexList[pBuildPatch->lControlVertexID[CV6]], pControlVertexList[pBuildPatch->lControlVertexID[CV2]], pRenderPatch->v0);
	}

	/* fill in uv coords */

	if (pUVList)
	{
		for (i = 0; i < nPatchTextureCount; i++)
		{
	  		/* fill in uv's from list */

	  		pRenderPatch->uv0[i] = pUVList[pBuildPatch->lUVID[i][0]];
	  		pRenderPatch->uv1[i] = pUVList[pBuildPatch->lUVID[i][1]];
	  		pRenderPatch->uv2[i] = pUVList[pBuildPatch->lUVID[i][2]];
	  		pRenderPatch->uv3[i] = pUVList[pBuildPatch->lUVID[i][3]];

			/* 3-point patches should have vertex0 == vertex3 */ 

			ASSERT(((pBuildPatch->ulFlags & PATCH_3_POINT) == 0) || (pBuildPatch->lUVID[i][0] == pBuildPatch->lUVID[i][3]));
		}
	}

	/* fill in default color uv values */

	pRenderPatch->ColorUV0 = DefaultColorUVs[0];
	pRenderPatch->ColorUV1 = DefaultColorUVs[1];
	pRenderPatch->ColorUV2 = DefaultColorUVs[2];
	pRenderPatch->ColorUV3 = DefaultColorUVs[3];
}
#endif //TOOLS

#ifdef TOOLS

/* this routine sets the proportional uv flags for a patch by comparing the magnitude of the bias handles for each edge

               edge 0
     v0 +----->......<-----+ v1
        |    cv0    cv1    |
        |                  |
        |                  |
        v cv2          cv5 v
        .                  .
 edge 3 .                  . edge 1
        .                  .
        ^ cv6          cv9 ^
        |                  |
        |                  |
        |    cv10   cv11   |
     v3 +----->......<-----+ v2
               edge 2

*/

#define PROPORTIONAL_THESHHOLD (5.0f / 100.0f)	// e.g. if one magnitude > 55% or < 45% of sum of magnitudes
static void SetProportionalUVFlags(ts_RenderPatch *pPatch, u32 *pu32Flags)
{
	float fLength1;
	float fLength2;
	float fMult;

//uncomment to turn off proportional uv calculations
//return;

	_ASSERT((*pu32Flags & (PATCH_TOP_EDGE_PROPORTIONAL_UV | PATCH_RIGHT_EDGE_PROPORTIONAL_UV | PATCH_BOTTOM_EDGE_PROPORTIONAL_UV | PATCH_LEFT_EDGE_PROPORTIONAL_UV)) == 0);	// shouldn't be any flags set yet
	if ((*pu32Flags & PATCH_TOP_EDGE_STRAIGHT) == 0)	// no proportional division for straight edges
	{
		fLength1 = (pPatch->cv0 - pPatch->v0).Length();
		fLength2 = (pPatch->cv1 - pPatch->v1).Length();
		fMult = fLength1 / (fLength1 + fLength2);
		if ((fMult > 0.5f + PROPORTIONAL_THESHHOLD) || (fMult < 0.5f - PROPORTIONAL_THESHHOLD))
			 *pu32Flags |= PATCH_TOP_EDGE_PROPORTIONAL_UV;
	}
	if ((*pu32Flags & PATCH_RIGHT_EDGE_STRAIGHT) == 0)	// no proportional division for straight edges
	{
		fLength1 = (pPatch->cv5 - pPatch->v1).Length();
		fLength2 = (pPatch->cv9 - pPatch->v2).Length();
		fMult = fLength1 / (fLength1 + fLength2);
		if ((fMult > 0.5f + PROPORTIONAL_THESHHOLD) || (fMult < 0.5f - PROPORTIONAL_THESHHOLD))
			 *pu32Flags |= PATCH_RIGHT_EDGE_PROPORTIONAL_UV;
	}
	if ((*pu32Flags & PATCH_BOTTOM_EDGE_STRAIGHT) == 0)	// no proportional division for straight edges
	{
		fLength1 = (pPatch->cv10 - pPatch->v3).Length();
		fLength2 = (pPatch->cv11 - pPatch->v2).Length();
		fMult = fLength1 / (fLength1 + fLength2);
		if ((fMult > 0.5f + PROPORTIONAL_THESHHOLD) || (fMult < 0.5f - PROPORTIONAL_THESHHOLD))
			 *pu32Flags |= PATCH_BOTTOM_EDGE_PROPORTIONAL_UV;
	}
	if ((*pu32Flags & PATCH_LEFT_EDGE_STRAIGHT) == 0)	// no proportional division for straight edges
	{
		fLength1 = (pPatch->cv2 - pPatch->v0).Length();
		fLength2 = (pPatch->cv6 - pPatch->v3).Length();
		fMult = fLength1 / (fLength1 + fLength2);
		if ((fMult > 0.5f + PROPORTIONAL_THESHHOLD) || (fMult < 0.5f - PROPORTIONAL_THESHHOLD))
			 *pu32Flags |= PATCH_LEFT_EDGE_PROPORTIONAL_UV;
	}
}
#endif //TOOLS

