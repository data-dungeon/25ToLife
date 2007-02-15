//============================================================================
//=
//= PatchDisp.h -
//=    This module is responsible for passing tri-strips off to the
//=    hardware renderers for each platform.
//=
//============================================================================

#ifndef _PATCHDISP_H
#define _PATCHDISP_H

#if defined (_XBOX)
#include "platform/Xbox/dxdmanager.h"
#endif

//============================================================================

#include "platform/CompilerControl.h"

//============================================================================

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

//============================================================================

struct ts_Patch;						// in Game/Database/SOM.h
struct ts_PatchSOM;					// in Game/Database/SOM.h
struct ts_DisplayListRecord;		// in Game/Database/SOM.h
class CInstance;					// in Game/World.h
struct ts_Body;						// in Game/Bone.h
struct ts_bRGBA;
struct ts_RecordList;

//============================================================================

//yuch-- currently ps2 requires vertex buffers for use with glVertexPointer to be xyzw format. for simplicity,
// pc is also currently using xyzw so it can use the same data file
//#ifdef WIN32
//#define TRI_STRIP_VERTEX_TYPE Vector3
//#elif defined(PS2)
#define TRI_STRIP_VERTEX_TYPE Vector4
//#endif //WIN32/PS2

//#define TEST_PROJECTED	// for projected texture spotlight test

//============================================================================

/* some macros for navigating tri strips */

#define TRI_STRIP_SKIP_HEADER(pData)						((char *) pData + sizeof(ts_TriStripHeader))
#define TRI_STRIP_SKIP_VERTICES(pData, PointCount)		((char *) pData + (PointCount * sizeof(TRI_STRIP_VERTEX_TYPE)))
#define TRI_STRIP_SKIP_NORMALS(pData, PointCount)		((char *) pData + (PointCount * sizeof(TRI_STRIP_VERTEX_TYPE)))
#define TRI_STRIP_SKIP_UVS(pData, PointCount)			((char *) pData + (((PointCount + 1) & ~1) * sizeof(Vector2)))
//#endif //WIN32/PS2

/* this one is a bit different, since the vertex colors aren't actually in the tri strip data, but are in a */
/* separate buffer. but on the ps2 they are subject to quad-word alignment */

#define TRI_STRIP_INC_VERTEX_COLORS(pInst, Count)	((pInst->VertexColor32Bit()) ? ((Count + (COLORS32_PER_QUAD_WORD - 1)) & ~(COLORS32_PER_QUAD_WORD - 1)) : ((Count + (COLORS16_PER_QUAD_WORD - 1)) & ~(COLORS16_PER_QUAD_WORD - 1)))
//#endif //WIN32/PS2

//============================================================================

/* for listing patches that contain alpha that need to be sorted internally as the model that contains them is being rendered */

struct ts_AlphaPatch
{
	ts_Patch *pCurrentPatch;
	float fZ;
};

struct ts_PatchSOM__RenderFlags
{
	unsigned char	bFogAlpha	: 1,
						bIsLODModel : 1,
						Pad			: 6;
};

//============================================================================

extern float AuraDepth;

//============================================================================

void PatchDisp__Cleanup();

#if defined( GCN )
	void GCNPatchSOM2__RenderTriStrips( ts_PatchSOM *pPatchSOM, Graphics4x4 *pmLocalToCamera, void *pVertexColors, ts_Body *pBody);
	#define PatchSOM__Render(pPatchSOM, pmLocalToCamera, pDisplayListRecord, pVertexColors, ucFlags, pBody) \
		GCNPatchSOM2__RenderTriStrips( pPatchSOM, pmLocalToCamera, pVertexColors, pBody)
#elif defined( DIRECTX )
	void PatchSOM__RenderDX( ts_PatchSOM** pPatchSOMList, int numSOMs, Graphics4x4 *pmLocalToCamera, ts_DisplayListRecord * pDisplayListRecord, ts_PatchSOM__RenderFlags ucFlags, ts_Body * pBody);
	void * PatchSOM__BuildMeshData( void * pMeshDataBlock, ts_RecordList * pTextureContextList, ts_RecordList * pWorldTextureContextList, unsigned int & numMeshesInBlock );
	void PatchSOM__ReleaseMeshData( void * pMeshDataBlock, unsigned int numMeshesInBlock );
	void * PatchSOM__GetIndexedMesh( void * pMeshData, unsigned int meshIndex );
	void PatchSOM__ReleaseIndexedMesh( void * pMeshInterface );
#define PatchSOM__Render(pPatchSOMList, numSOMs, pmLocalToCamera, pDisplayListRecord, pVertexColors, ucFlags, pBody) \
		PatchSOM__RenderDX( pPatchSOMList, numSOMs, pmLocalToCamera, pDisplayListRecord, ucFlags, pBody)

#if defined(_XBOX)
void PatchSOM__CheckHWDataDXD( CDXD_Data* Dx );
void PatchSOM__ReleaseHWDataDXD( CDXD_Data* Dx);
void PatchSOM__ReleaseAllHWData( void );
#endif

#else
	void PatchSOM__Render(ts_PatchSOM** pPatchSOMList, int numSOMs, Graphics4x4 *pmLocalToCamera, ts_DisplayListRecord *pDisplayListRecord, void *pVertexColors, ts_PatchSOM__RenderFlags ucFlags, ts_Body *pBody) NAMED_SECTION("text.rend1");
	void PatchSOM__BuildDisplayList(ts_PatchSOM *pPatchSOM, ts_DisplayListRecord *pOrigDisplayListRecord, void *pVertexColors, ts_PatchSOM__RenderFlags ucFlags, ts_Body *pBody);
#endif

#if defined( PS2 )
	void PatchSOM__RenderPS2ShadowModel( ts_PatchSOM **pPatchSOMList, int numSOMs, Graphics4x4 *pmLocalToCamera, ts_DisplayListRecord *pDisplayListRecord, ts_Body *pBody);
	void DynamicDisplayList__Reset(void);
	void DynamicDisplayList__RecordInstance(CInstance *pInst);
#endif //PS2

void DisplayList__ResetDisplayLists(bool bCopy);

//============================================================================

#ifdef __cplusplus
}
#endif //__cplusplus

//============================================================================

#endif //_PATCHDISP_H
