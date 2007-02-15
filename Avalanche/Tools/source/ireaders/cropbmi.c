#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <ctype.h>
#include <string.h>

#include "cropbmi.h"

char * CropImage(BITMAPINFO **bmi, char **imageData, int *imageLength, int *highPalette, POINT *ptTopLeft)
{
	int	imageSize = 0,
		   imageWidth,
			padSize = 0,
			pad = 0,
			left = (*bmi)->bmiHeader.biWidth,
			right = 0,
			bottom = 0,
			top = -1,
			result = 0,
			width4Bit = 0,
			rowSize;
	char  *compImage,
			*newImageData;

	imageWidth = (*bmi)->bmiHeader.biWidth;
	
  	switch ((*bmi)->bmiHeader.biBitCount)
	{
		case 4:
			rowSize = ((*bmi)->bmiHeader.biWidth + 1) / 2;
			break;

		case 8:
			rowSize = (*bmi)->bmiHeader.biWidth;
			break;

	}

	pad = (rowSize % 4);
	if (pad)
	{
		pad = (4 - pad);
	}

	result = FlipBitMap(*bmi, *imageData, (rowSize + pad));

	switch ((*bmi)->bmiHeader.biBitCount)
	{
		case 8:
			result = GetImageSize8Bit(*bmi, 
											  *imageData,
											  pad,
											  &left,
											  &right,
											  &bottom,
											  &top,
											  highPalette);
			ptTopLeft->x = left;
			ptTopLeft->y = top;
			padSize = (((*bmi)->bmiHeader.biWidth) % 4);
		  	if (padSize)
			{
				padSize = (4 - padSize);
			}
			imageSize = ((*bmi)->bmiHeader.biWidth + padSize) * ((*bmi)->bmiHeader.biHeight);
			compImage = (char *) malloc(imageSize);
			if (compImage == NULL)
			{
				return FALSE;
			}
			memset (compImage,0,imageSize);

			result = Update8BitImageData (*bmi, 
													*imageData, 
													compImage, 
													&left, 
													&right, 
													&bottom, 
													&top, 
													padSize,
													pad,
													imageWidth);
			*imageLength = imageSize;
			
			return compImage;
			break;

		case 4:
			newImageData = Convert4BitImage(*bmi, *imageData);
			rowSize = (*bmi)->bmiHeader.biWidth;
			pad = (rowSize % 4);
			if (pad)
			{
				pad = (4 - pad);
			}

			result = GetImageSize8Bit(*bmi,
											  newImageData,
											  pad,
											  &left,
											  &right,
											  &bottom,
											  &top,
											  highPalette);
			ptTopLeft->x = left;
			ptTopLeft->y = top;
			padSize = (((*bmi)->bmiHeader.biWidth) % 4);
		  	if (padSize)
			{
				padSize = (4 - padSize);
			}
			imageSize = ((*bmi)->bmiHeader.biWidth + padSize) * ((*bmi)->bmiHeader.biHeight);
			compImage = (char *) malloc(imageSize);
			if (compImage == NULL)
			{
				return FALSE;
			}
			memset(compImage, '\0', sizeof(compImage));

			result = Update8BitImageData (*bmi, 
													newImageData, 
													compImage, 
													&left, 
													&right, 
													&bottom, 
													&top, 
													padSize,
													pad,
													imageWidth);
			
			free (newImageData);
			*imageLength = imageSize;
			return compImage;
			break;

		default:
			return 0;
			break;
	}
}

char * FlipImage (BITMAPINFO *bmi, char *imageData, int *highPalette, RECT *recCrop)
{

	int pad = 0,
		 left = bmi->bmiHeader.biWidth,
		 right = 0,
		 bottom = 0,
		 top = -1,
		 rowSize;

	char *newImageData;

	if (bmi->bmiHeader.biBitCount == 4)
	{
		newImageData = Convert4BitImage (bmi,imageData);
	}
	else
	{
		newImageData = (char *) malloc (((bmi->bmiHeader.biWidth + 3) & ~3) * bmi->bmiHeader.biHeight);
		memcpy (newImageData,imageData,((bmi->bmiHeader.biWidth + 3) & ~3) * bmi->bmiHeader.biHeight);
	}

	rowSize = bmi->bmiHeader.biWidth;

	pad = (rowSize % 4);
	if (pad)
	{
		pad = (4 - pad);
	}

	FlipBitMap (bmi,newImageData,(rowSize + pad));

	*highPalette = 0;
	GetHighPalette (bmi,newImageData,pad,&left,&right,&bottom,&top,highPalette);
	recCrop->left = left;
	recCrop->right = right;
	recCrop->top = top;
	recCrop->bottom = bottom;
	return newImageData;
}

void PackImage( char **pImage, int iWidth, int iHeight)
{

	int	srcRowSize,
			destRowSize,
			row,
			col;
	
	char	*pSrcImage,
			*pDestImage,
			*pSrc,
			*pDest;

	srcRowSize = (iWidth + 3) & ~3;
	destRowSize = iWidth;

	pSrcImage = *pImage;

	pDestImage = (char *)malloc( destRowSize * iHeight);

	for (row = 0; row < iHeight; row++)
	{
		pSrc = pSrcImage + (srcRowSize * row);
		pDest = pDestImage + (destRowSize * row);
		for (col = 0; col < iWidth; col++)
		{
			*pDest = *pSrc;
			pDest++;
			pSrc++;
		}
	}
	free( pSrcImage);
	*pImage = pDestImage;
}

void Pack4BitImage( char **pImage, int iWidth, int iHeight)
{

	int	srcRowSize,
			destRowSize,
			row,
			col;
	
	char	*pSrcImage,
			*pDestImage,
			*pSrc,
			*pDest;

	srcRowSize = (iWidth + 3) & ~3;
	destRowSize = (iWidth + 1) >> 1;

	pSrcImage = *pImage;

	pDestImage = (char *)malloc( destRowSize * iHeight);

	for (row = 0; row < iHeight; row++)
	{
		pSrc = pSrcImage + (srcRowSize * row);
		pDest = pDestImage + (destRowSize * row);
		for (col = 0; col < ((iWidth + 1) >> 1); col++)
		{
			*pDest = (pSrc[0] << 4) | pSrc[1];
			pDest++;
			pSrc += 2;
		}
	}
	free( pSrcImage);
	*pImage = pDestImage;
}

void MakeRGBAImage( char **pImage, BITMAPINFO *bmi, int iWidth, int iHeight)
{

	int	srcRowSize,
			destRowSize,
			row,
			col;
	
	unsigned char	*pSrcImage,
			*pDestImage,
			*pSrc,
			*pDest;

	RGBQUAD	*pRGB;

	srcRowSize = (iWidth + 3) & ~3;
	destRowSize = iWidth * 4;

	pSrcImage = *pImage;

	pDestImage = (char *)malloc( destRowSize * iHeight);

	for (row = 0; row < iHeight; row++)
	{
		pSrc = pSrcImage + (srcRowSize * row);
		pDest = pDestImage + (destRowSize * row);
		for (col = 0; col < iWidth; col++)
		{
			pRGB = bmi->bmiColors + *pSrc;
			pDest[0] = pRGB->rgbRed;
			pDest[1] = pRGB->rgbGreen;
			pDest[2] = pRGB->rgbBlue;
			pDest[3] = (char)0xFF;
			pDest += 4;
			pSrc++;
		}
	}
	free( pSrcImage);
	*pImage = pDestImage;
}

BOOL Update8BitImageData(BITMAPINFO *bmi, 
								 char *imageData, 
								 char *compImage, 
								 int *left, 
								 int *right, 
								 int *bottom, 
								 int *top, 
								 int padSize,
								 int pad,
								 int imageWidth)
{
	int	i = 0,
			currentLine;

	currentLine = ((imageWidth + pad) * (*top)) + *left;
  	for (i; i < bmi->bmiHeader.biHeight; i++)
	{
		memcpy(compImage, imageData + currentLine, bmi->bmiHeader.biWidth);
		compImage = compImage + (bmi->bmiHeader.biWidth + padSize);
		currentLine = currentLine + (imageWidth + pad);
	}

	return TRUE;
}


BOOL GetImageSize8Bit(BITMAPINFO *bmi,
							 char *imageData,
							 int pad,
							 int *left,
							 int *right,
							 int *bottom,
							 int *top,
							 int *highPalette)
{
	BYTE	backGround = 0;
	int	x,
			y,
			offset;
	
	*highPalette = 0;
	for (y = 0; y < bmi->bmiHeader.biHeight; y++)
	{
		offset = y * (bmi->bmiHeader.biWidth + pad);
		for (x = 0; x < bmi->bmiHeader.biWidth; x++)
		{
			backGround = (BYTE)(imageData[offset + x]);
			if (backGround > *highPalette)
			{
				*highPalette = backGround;
			}
			if(backGround)
			{
				if (*top == -1)
				{
					*top = y;
				}
				*bottom = y;
				if (x < *left )
				{
					*left = x;
				}
				if (x > *right)
				{
					*right = x;
				}
			}
		}
	}
	bmi->bmiHeader.biHeight = (*bottom - *top) + 1;
	bmi->bmiHeader.biWidth = (*right - *left) + 1;
	return TRUE;
}

BOOL GetHighPalette(BITMAPINFO *bmi,
							 char *imageData,
							 int pad,
							 int *left,
							 int *right,
							 int *bottom,
							 int *top,
							 int *highPalette)
{
	BYTE	backGround = 0;
	int	x,
			y,
			offset;
	
	for (y = 0; y < bmi->bmiHeader.biHeight; y++)
	{
		offset = y * (bmi->bmiHeader.biWidth + pad);
		for (x = 0; x < bmi->bmiHeader.biWidth; x++)
		{
			backGround = imageData[offset + x];
			if (backGround > *highPalette)
			{
				*highPalette = backGround;
			}
			if(backGround)
			{
				if (*top == -1)
				{
					*top = y;
				}
				*bottom = y;
				if (x < *left )
				{
					*left = x;
				}
				if (x > *right)
				{
					*right = x;
				}
			}
		}
	}
	return TRUE;
}

BOOL FlipBitMap(BITMAPINFO *bmi,char *bmBits,int rowSize)
{

	int i;

	char *tempBits,
		  *srcBits,
		  *destBits;

	tempBits = (char *)malloc (bmi->bmiHeader.biHeight * rowSize);
	if (tempBits == NULL)
	{
		return 0;
	}

	memcpy (tempBits,bmBits,bmi->bmiHeader.biHeight * rowSize);
	srcBits = tempBits + ((bmi->bmiHeader.biHeight - 1) * rowSize);
	destBits = bmBits;
	for (i = 0; i < bmi->bmiHeader.biHeight; i++)
	{
		memcpy (destBits,srcBits,rowSize);
		destBits += rowSize;
		srcBits -= rowSize;
	}
	free (tempBits);
	return TRUE;
}


char * Convert4BitImage (BITMAPINFO *bmi, char *imageData)
{

	char *newImageData,
		  *imagePtr;

	BYTE high = 0,
		  low = 0,
		  currentByte;

	int  srcRowSize,
		  destRowSize;
	
	int x,
		 y,
		 count = 0,
		 offset;

	srcRowSize = (((bmi->bmiHeader.biWidth + 1) / 2) + 3) & ~3;
	destRowSize = (bmi->bmiHeader.biWidth + 3) & ~3;

	newImageData = (char *) malloc (destRowSize * bmi->bmiHeader.biHeight);
	
	memset (newImageData,0,destRowSize * bmi->bmiHeader.biHeight);

	imagePtr = newImageData;

	for (y = 0; y < bmi->bmiHeader.biHeight; y++)
	{
		offset = y * srcRowSize;
		for (x = 0; x < ((bmi->bmiHeader.biWidth + 1) / 2); x++)
		{
			currentByte = imageData[offset + x];
			high = (currentByte>>4) & 0x0f;
			low = currentByte & 0x0f;

			imagePtr[x * 2] = high;
			imagePtr[(x * 2) + 1] = low;
		}
		imagePtr += destRowSize;
	}
	bmi->bmiHeader.biBitCount = 8;
	return newImageData;
}