/* --------------------------------------------------
 * File    : READIMAGE.H
 * Created : Fri Oct  2 14:34:23 1998
 * Descript:
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#ifndef __READIMAGE_H
#define __READIMAGE_H

/* --------------------------------------------------
 * Defines
 * --------------------------------------------------*/
#define TYPE_BMP 0
#define TYPE_TGA 1
#define TYPE_PCX 2
#define TYPE_ANM 3
#define TYPE_UNKNOWN -1

/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
int		DetermineImageType( HANDLE, short *);
BOOL		ReadAnim( HANDLE, short *, BITMAPINFO **, char *(**)[]);
BOOL		ReadImage( HANDLE, BITMAPINFO **, char **);
#ifdef __cplusplus
}
#endif

#endif