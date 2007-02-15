/***********************************/
/*                                 */
/* d_SplitPatch.h                  */
/* big juju prototype  09/20/99    */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* patch tessellation              */
/*                                 */
/***********************************/

#ifndef __D_SPLITPATCH_H
#define __D_SPLITPATCH_H

/******************* includes *****************************/

/* system includes */

/* engine includes */

#include "math/mathstruct.h"			// low-level include
#include "Layers/LayersStruct.h"		// low-level include
#include "Game/Database/Som.h"

/* tool includes */

#ifdef TOOLS
#include "buildpatch.h"
#endif //TOOLS

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/******************* forward declarations ****************/

/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/* ready-for-render patch structure-- self-contained for tessellation, though it's missing some texture info-- number of
textures and texture indices. these are contained in the original ts_Patch record, which is where this structure was
filled in from. typically the vertices, control vertices, and normals have already been transformed */

struct ts_RenderPatch
{
#ifdef WIN32
	Vector3Packed	v0;			// patch vertices
	Vector3Packed	v1;
	Vector3Packed	v2;
	Vector3Packed	v3;
#elif defined(PS2)
	Vector4	v0;			// patch vertices
	Vector4	v1;
	Vector4	v2;
	Vector4	v3;
#elif defined(GCN)
	Vector3Packed	v0;			// patch vertices
	Vector3Packed	v1;
	Vector3Packed	v2;
	Vector3Packed	v3;
#endif //WIN32/PS2
	Vector3Packed	d0;			// patch edge 2nd derivatives
	Vector3Packed	d1;
	Vector3Packed	d2;
	Vector3Packed	d5;
	Vector3Packed	d6;
	Vector3Packed	d9;
	Vector3Packed	d10;
	Vector3Packed	d11;
	ts_fRGBA	Color[4];	// vertex colors
	Vector2	uv0[MAX_SOM_TEXTURES_PER_PATCH];	// uv's
	Vector2	uv1[MAX_SOM_TEXTURES_PER_PATCH];
	Vector2	uv2[MAX_SOM_TEXTURES_PER_PATCH];
	Vector2	uv3[MAX_SOM_TEXTURES_PER_PATCH];
#ifdef TOOLS
	Vector3Packed	cv0;			// patch control vertices
	Vector3Packed	cv1;
	Vector3Packed	cv2;
	Vector3Packed	cv5;
	Vector3Packed	cv6;
	Vector3Packed	cv9;
	Vector3Packed	cv10;
	Vector3Packed	cv11;
	Vector3Packed	Normal[4];	// patch normals, so i can subdivide normals during tessellation into tri-strips
	Vector2	ColorUV0;	// for vertex color interpolation at export time
	Vector2	ColorUV1;
	Vector2	ColorUV2;
	Vector2	ColorUV3;
#endif //TOOLS
};

/******************* global variables ********************/

/* tessellation environment variables */

extern int nPatchTextureCount;

/* original render-ready patch to be tessellated */

extern ts_RenderPatch RenderPatch;

#ifdef TOOLS
/* for calculating a patch's bounding box during export */

extern ts_BoundingBox PatchBoundingBox;
extern bool bCollisionPatchSplit;
extern u32 u32CollisionTriangleCount;
extern u32 u32CollisionVertexCount;
#endif //TOOLS

/* pre-calculated colors for s-shaped edges */

extern ts_fRGBA SShapedColor[S_SHAPED_MAX_ENTRIES];


#ifdef TOOLS
/* for data export, pre-calculated normals for s-shaped edges */

extern Vector3 SShapedNormal[S_SHAPED_MAX_ENTRIES];

/* default color uv values for non-tessellated patch */

extern Vector2 DefaultColorUVs[4];
#endif //TOOLS

/******************* global prototypes *******************/

void SplitPatch(ts_RenderPatch *pPatch, int nLevel, u32 u32Flags);
#ifdef TOOLS
void GetPatchMidpoint(ts_RenderPatch *pPatch, u32 u32Flags, Vector3 *pMidpoint);
#endif //TOOLS
void SetStraightTolerance(float fBiasMultiplier);
void SetForcedZoom(bool bOnOff, float fValue);
void SetNegativeNearClip(float fNearClip);
void GetSplineMidpoint(Vector3 *p0, Vector3 *p1, Vector3 *p2, Vector3 *p3, Vector3 *pMidpoint);
void CRenderPatch__Fill(ts_RenderPatch *pRenderPatch, u32 u32RenderFlags, ts_Patch *pPatch, VCT_TYPE *pVertexList, VCT_TYPE *pControlVertexList, VCT_TYPE *pNormalList, Vector2 *pUVList);
#ifdef TOOLS
void CRenderPatch__FillVct3f(ts_RenderPatch *pRenderPatch, u32 u32RenderFlags, ts_BuildPatch *pBuildPatch, Vector3 *pVertexList, Vector3 *pControlVertexList, Vector3 *pNormalList, Vector2 *pUVList);
#endif //TOOLS


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __D_SPLITPATCH_H




