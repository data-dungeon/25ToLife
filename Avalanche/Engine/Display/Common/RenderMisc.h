/*************************************/
/*                                   */
/*   d_RenderMisc.h                  */
/*   new engine  07/14/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   miscellaneous render api        */
/*                                   */
/*************************************/

#ifndef __D_RENDERMISC_H
#define __D_RENDERMISC_H

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include "Layers/LayersStruct.h"				// low-level include

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

/******************* forward declarations ****************/

struct ts_BoundingBox;		// in g_Bounds.h

/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* global variables ********************/

/******************* global prototypes *******************/

#ifndef CONSUMER_BUILD

// Some debugging stuff

// Throw away any buffered lines
void gfxClearLineBuffer();

// Draw a line segment specified in world coordinates
void gfxDrawLineWC(const Vector3 *a, const Vector3 *b, ts_bRGBA *color);

// Draw a line segment specified in model coordinates, model transform given by m
void gfxDrawLineMC(Graphics4x4 *m, Vector3 *a, Vector3 *b, ts_bRGBA *color);

// Draw a lit & textured quad specified in normalized screen coordinates
void gfxDrawQuadSC(Vector2 *verts, ts_bRGBA *colors, Vector2 *textureUVs, int nTextureIndex);
void gfxDrawQuadSC1(Vector2 *verts, ts_bRGBA *colors, Vector2 *textureUVs, int nTextureIndex);

void gfxDrawTriangleWC(Vector3 *a, Vector3 *b, Vector3 *c, ts_bRGBA *color);
void gfxDrawTriangleMC(Graphics4x4 *m, Vector3 *a, Vector3 *b, Vector3 *c, ts_bRGBA *color);

// Draw a "mark" centered at p
void gfxDrawMarkWC(const Vector3 *p, float radius, ts_bRGBA *color);

// Draw a "mark" centered at p
void gfxDrawMarkMC(Graphics4x4 *m, Vector3 *p, float radius, ts_bRGBA *color);

// Draw a "sphere" centered at p
void gfxDrawSphereWC(const Vector3 *p, float radius, ts_bRGBA *color);

// Draw a "lozenge"
void gfxDrawLozengeWC(const Vector3 *a, const Vector3 *b, float radius, ts_bRGBA *color);

// draw a box in world space
void gfxDrawBox( ts_BoundingBox *pBox);

void gfxDrawTristripMC(Graphics4x4 *m, Vector4 *v, u16 count, ts_bRGBA *color);

#endif //CONSUMER_BUILD

void gfxRenderTriangles2D(t_Handle TextureSetHandle, u32 u32TextureIndex, ts_bRGBA *pColor, u32 u32TriangleCount, Vector4 *pVertexList, Vector2 *pUVList);


#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // __D_RENDERMISC_H





