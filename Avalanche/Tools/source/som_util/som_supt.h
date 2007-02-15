/* --------------------------------------------------
 * File    : SOM_SUPT.H
 * Created : Fri Oct  2 14:34:23 1998
 * Descript:
 * --------------------------------------------------*/
#ifndef __SOM_SUPT_H
#define __SOM_SUPT_H

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/

#include <t_som.h>

/* --------------------------------------------------
 * Defines
 * --------------------------------------------------*/
#define SOM_HAS_TRIS				0x00000001
#define SOM_HAS_QUADS			0x00000002

#define SOM_HAS_TEXTURES		0x00000004
#define SOM_HAS_COLORS        0x00000008

#define SOM_HAS_ALL				0x0000000F

/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

t_lflagbits       somGetHasFlags( ts_SOMUP *pModel);
void					somNextFace( ts_SOMFace **ppFace);
bool 					somTriFace( ts_SOMFaceUP *pFace);

#ifdef __cplusplus
}
#endif

#endif __SOM_SUPT_H
