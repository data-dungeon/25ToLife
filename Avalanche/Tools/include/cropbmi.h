/* --------------------------------------------------
 * File    : CROPIMG.H
 * Created : Fri Oct  2 14:34:23 1998
 * Descript:
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#ifndef __CROPIMAGE_H
#define __CROPIMAGE_H

/* --------------------------------------------------
 * Defines
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
BOOL GetImageSize8Bit(BITMAPINFO *bmi, 
								 char *imageData,
								 int pad,
								 int *left, 
								 int *right, 
								 int *bottom, 
								 int *top,
								 int *highPalette);
BOOL GetHighPalette(BITMAPINFO *bmi, 
								 char *imageData,
								 int pad,
								 int *left, 
								 int *right, 
								 int *bottom, 
								 int *top,
								 int *highPalette);
BOOL Update8BitImageData(BITMAPINFO *bmi, 
								 char *imageData, 
								 char *compImage, 
								 int *left, 
								 int *right, 
								 int *bottom, 
								 int *top, 
								 int padSize,
								 int pad,
								 int imageWidth);
BOOL GetImageSize4Bit(BITMAPINFO *bmi,
								 char *imageData, 
								 int left, 
								 int  right, 
								 int bottom, 
								 int top);
char * CropImage		(BITMAPINFO **bmi,
								 char **imageData,
								 int *imageLength,
								 int *highPalette,
								 POINT *ptTopLeft);
char * FlipImage (BITMAPINFO *bmi, char *imageData, int *highPalette, RECT *recCrop);
BOOL FlipBitMap		(BITMAPINFO *bmi, char *bmBits, int rowSize);
BOOL ReadBMP(HANDLE hFile, BITMAPINFO **bmi, char **bmBits);
char * Convert4BitImage(BITMAPINFO *bmi, char *imageData);
void Pack4BitImage( char **pImage, int iWidth, int iHeight);
void PackImage( char **pImage, int iWidth, int iHeight);
void MakeRGBAImage( char **pImage, BITMAPINFO *bmi, int iWidth, int iHeight);
#ifdef __cplusplus
}
#endif

#endif
