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

#include <pshpack1.h>
typedef struct PCXPalette_tag
{
	BYTE pcxRed,
		  pcxGreen,
		  pcxBlue;
} PCXPaletteDef;

typedef struct PCXHeader_tag
{
	BYTE pcxId,
		  pcxVersion,
		  pcxEncoding,
		  pcxBitsPerPixel;
	WORD pcxXMin,
		  pcxYMin,
		  pcxXMax,
		  pcxYMax,
		  pcxXRes,
		  pcxYRes;
	PCXPaletteDef pcxHeaderPalette[16];
	BYTE pcxReserved,
		  pcxPlanes;
	WORD pcxBytesPerLine,
		  pcxPaletteInterp,
		  pcxXScreenSize,
		  pcxYScreenSize;
	BYTE pcxFiller[54];
} PCXHeaderDef;
#include <poppack.h>

/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
int DetermineImageType (HANDLE);
BOOL ReadAnim (HANDLE,short *,BITMAPINFO **,char *(**)[]);
BOOL ReadImage (HANDLE,BITMAPINFO **,char **);
#ifdef __cplusplus
}
#endif

#endif