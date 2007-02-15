/* --------------------------------------------------
 * File    : SOM_UTIL.H
 * Created : Fri Sep 25 15:13:12 1998
 * Descript:
 * --------------------------------------------------*/
#ifndef __SOM_UTIL_H
#define __SOM_UTIL_H

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <t_struct.h>
#include <t_som.h>

/* --------------------------------------------------
 * Defines
 * --------------------------------------------------*/

#define SOM_PAK_NO_TEXTURES		0x00000001
#define SOM_PAK_SNGL_TEXTURE		0x00000002
#define SOM_PAK_CLR_WITH_TEX		0x00000004
#define SOM_PAK_NORMALS				0x00000008

#define SOM_PAK_CLR_BYTE			0x00000010
#define SOM_PAK_CLR_FLOAT			0x00000020

#define SOM_PAK_UV_BYTE				0x00000100
#define SOM_PAK_UV_FLOAT			0x00000200

#define SOM_SET_TEXTURE				0x00000001
#define SOM_SET_CLR_SINGLE			0x00000002
#define SOM_SET_CLR_VERTEX			0x00000004
#define SOM_SET_NORMAL				0x00000008


// Sort Flags

#define SOM_SORT_SNGL_TEXTURE		0x00000001

/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
void					somSetup( ts_SOMUP *pModel);
void					somSort( ts_SOMUP *pModel, t_lflagbits);
void					somSortAlpha( ts_SOMUP *pModel, t_lflagbits, ts_RBImage *pTextures);
void					somDupeFaceData( ts_SOMFaceUP *pIn, ts_SOMFaceUP *pOut);
void					somFreeSOMUP( ts_SOMUP *pModel);
void					somFreeSOM( ts_SOM *pModel);
void					somFreeSOMFaceUP( ts_SOMFaceUP *pFace);
void					somBoundingBox( ts_SOMUP *pSOM, t_AABB *pBoundingBox);
void					somBoundingSphere( ts_SOMUP *pSOM, ts_Sphere *pBoundingSphere);
void					somCenter( ts_SOMUP *pSOM, ts_f3DCoord *pCenter);
void					somCenterBB( ts_SOMUP *pSOM, ts_f3DCoord *pCenter);
void					somBondingSphere( ts_SOMUP *pSOM, ts_f3DCoord *pCenter, float *pRadius);
void 					somMove( ts_SOMUP *pModel, float fDeltaX, float fDeltaY, float fDeltaZ);
void 					somScale( ts_SOMUP *pModel, float fScaleX, float fScaleY, float fScaleZ);
void					somNextFace( ts_SOMFace **ppFace);

void					somConvertTri( ts_SOMUP **ppModel);
void					somConvertQuad( ts_SOMUP **ppModel);

ts_SOMUP *			somCopySOMUP( ts_SOMUP *pModel);
ts_SOM *				somPack( ts_SOMUP *pModel, t_lflagbits PackFlags, t_lflagbits SetFlags);

void					somFaceNormal( ts_SOMFaceUP *pFace, ts_f3DCoord *pVtxList, ts_f3DCoord *pNorm);
void					somCalcFaceNormals( ts_SOMUP *pModel);
void					somAddVertexNormals( ts_SOMUP *pModel);
void					somAddFaceNormals( ts_SOMUP *pModel);

unsigned short *	SortTextures( ts_RBImage *pImages, unsigned short usTexCount);
void					somSwapTexID( ts_SOMUP *pModel, unsigned short *pIDs, unsigned short usIDCount);

void					ShowSOM( ts_SOM *pModel);
void					ShowSOMUP( ts_SOMUP *pModel);
#ifdef __cplusplus
}
#endif

#endif __SOM_UTIL_H
