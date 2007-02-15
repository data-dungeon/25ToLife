//=============================================================================//
//  Copyright ©2001 Avalanche Software.  All Rights Reserved.                  //
//=============================================================================//
//      File: graphics.h                                                       //
//    Author: Daniel Silver                                                    //
//   Created: 05/02/2001                                                       //
//=============================================================================//

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <windows.h>

typedef DWORD COLOR32;

typedef enum
{
	PIXELFMT_UNKNOWN,
	PIXELFMT_A8R8G8B8,
	PIXELFMT_A8B8G8R8,
	PIXELFMT_X8R8G8B8,
	PIXELFMT_X8B8G8R8,
	PIXELFMT_R8G8B8,
	PIXELFMT_B8G8R8,
	PIXELFMT_A4R4G4B4,
	PIXELFMT_A4B4G4R4,
	PIXELFMT_X4R4G4B4,
	PIXELFMT_X4B4G4R4,
	PIXELFMT_A1R5G5B5,
	PIXELFMT_A1B5G5R5,
	PIXELFMT_X1R5G5B5,
	PIXELFMT_X1B5G5R5,
	PIXELFMT_R5G6B5,
	PIXELFMT_B5G6R5,
	PIXELFMT_PALETTE8,
	PIXELFMT_PALETTE4,
	PIXELFMT_GRAY8,
	PIXELFMT_GRAY4,
	PIXELFMT_MONOCHROME,
	PIXELFMT_INDEX32,
	PIXELFMT_INDEX24,
	PIXELFMT_INDEX16,
	PIXELFMT_INDEX8,
	PIXELFMT_INDEX4,
}
PIXELFORMAT;

typedef enum
{
	BLENDMODE_MODULATE,
	BLENDMODE_ADD,
	BLENDMODE_SUBTRACT,
}
BLENDMODE;

typedef struct
{
	int Width;
	int Height;
	PIXELFORMAT Format;
	int Stride;
	COLOR32 Palette[256];
	BYTE *pBits;
}
BITMAP2;

#ifndef FASTCALL
#ifdef _MSC_VER
#define FASTCALL __fastcall
#else
#define FASTCALL
#endif // _MSC_VER
#endif // FASTCALL

typedef void (FASTCALL *SETPIXELFUNCTION)(BITMAP2 &bm, int x, int y, COLOR32 c );
typedef COLOR32 (FASTCALL *GETPIXELFUNCTION)(BITMAP2 &bm, int x, int y);

typedef void (*SETSCANLINEFUNCTION)(int ch, COLOR32 *pScanlineBits, int x, int y, int Count );
typedef void (*GETSCANLINEFUNCTION)(int ch, COLOR32 *pScanlineBits, int x, int y, int Count );

extern SETPIXELFUNCTION SetPixelFn[16];
extern GETPIXELFUNCTION GetPixelFn[16];

extern SETSCANLINEFUNCTION SetScanlineFn[16];
extern GETSCANLINEFUNCTION GetScanlineFn[16];

int PixelFormatToBPP( PIXELFORMAT Format );
PIXELFORMAT PixelFormatFromBPP( int BPP );
char * PixelFormatToString( PIXELFORMAT Format );
PIXELFORMAT PixelFormatWithAlpha( PIXELFORMAT Format );
PIXELFORMAT PixelFormatWithoutAlpha( PIXELFORMAT Format );
int GetImageStride( int Width, int BPP );
int GetImageStride( int Width, PIXELFORMAT Format );

BITMAP2 GetBitmap2Struct( HBITMAP hbm, BOOL Flipped );
void BeginBitmap( int channel, int Width, int Height, PIXELFORMAT Format, int Stride, void *pBits );
void BeginBitmap( int channel, BITMAP2 *pBitmap );
void BeginDIB( int channel, HBITMAP hbm, PIXELFORMAT Format = PIXELFMT_UNKNOWN );
void FinishBitmap( int channel );
void SetPixelFormat( int channel, PIXELFORMAT Format );
void SetPalette( int channel, COLOR32 *pPalette, int PaletteSize );
void GetPalette( int channel, COLOR32 *pPalette, int PaletteSize );
BITMAP2 *CreateBitmap2( int Width, int Height, PIXELFORMAT Format, COLOR32 *pPalette, int PaletteSize );
BITMAP2 *DeleteBitmap2( BITMAP2 *pBitmap );
void SetColorTable( BITMAP2 *pBitmap, COLOR32 *pPalette, int BeginIndex, int PaletteSize );

//////////////////////////////////////////////////////////////////////////

void ReadScanline( COLOR32 *pScanlineBits, int x, int y, int Count, void* pSrc, int Sw, int Sh, PIXELFORMAT SrcFormat, int SrcStride );
void WriteScanline( COLOR32 *pScanlineBits, int x, int y, int Count, void* pDst, int Dw, int Dh, PIXELFORMAT DstFormat, int DstStride );
void BitmapToBitmap( BITMAP2 *pDst, int Dx, int Dy, int Dw, int Dh, BITMAP2 *pSrc, int Sx, int Sy, int Sw, int Sh );
void BitmapToBitmap( void *pDst, int DstWidth, int DstHeight, PIXELFORMAT DstFormat, int DstStride, int Dx, int Dy, int Dw, int Dh,
							void *pSrc, int SrcWidth, int SrcHeight, PIXELFORMAT SrcFormat, int SrcStride, int Sx, int Sy, int Sw, int Sh );
void BitmapToDC( HDC hdcDst, int Dx, int Dy, int Dw, int Dh, BITMAP2 *pSrc, int Sx, int Sy, int Sw, int Sh );

//////////////////////////////////////////////////////////////////////////

void FastLine( int channel, int x1, int y1, int x2, int y2, COLORREF c );
void FastRect( int channel, int x1, int y1, int x2, int y2, COLORREF c );
void DrawEllipse( int channel, int x1, int y1, int x2, int y2, COLORREF c );

void TransparentLine( int channel, int x1, int y1, int x2, int y2, COLORREF c, BLENDMODE BlendMode );

//////////////////////////////////////////////////////////////////////////

COLORREF ColorBlend( COLORREF crDst, COLORREF crSrc, int A );
COLORREF ColorMultiply( COLORREF crDst, COLORREF crSrc );
COLORREF ColorAdd( COLORREF crDst, COLORREF crSrc );
COLORREF ColorSub( COLORREF crDst, COLORREF crSrc );
HBITMAP CreateDIB( int nWidth, int nHeight, int nBPP );
HBITMAP CloneBitmap2( BITMAP2 *pbm );
HBITMAP ReleaseDIB( HBITMAP hbm );
HBITMAP CloneDIB( HBITMAP hbmSrc );
HBITMAP CloneDIB( HBITMAP hbmSrc, BOOL bBlank );
HBITMAP ResampleDIB( HBITMAP hbmSrc, int Sx, int Sy, int Sw, int Sh, int Dw, int Dh );
void HighQualityStretchBlt( HDC hdcDst, int Dx, int Dy, int Dw, int Dh, HBITMAP hbmSrc, int Sx, int Sy, int Sw, int Sh );
void HighQualityCopyBitmap( HBITMAP hbmDst, int Dx, int Dy, int Dw, int Dh, HBITMAP hbmsrc, int Sx, int Sy, int Sw, int Sh );
void StretchBltPreserveAspectRatio( HDC hdcDst, int Dx, int Dy, int Dw, int Dh, HBITMAP hbmSrc, int Sx, int Sy, int Sw, int Sh );
void ClearDIB( HBITMAP hbm, COLORREF c );
BITMAP GetBitmapStruct( HBITMAP hbm );
int GetBitmapWidth( HBITMAP hbm );
int GetBitmapHeight( HBITMAP hbm );
int GetBitmapBPP ( HBITMAP hbm );
BYTE *GetBitmapData( HBITMAP hbm );
int GetBitmapStride( HBITMAP hbm );
void BltBitmap( HDC hdcDst, int Dx, int Dy, int Dw, int Dh, HBITMAP hbmSrc, int Sx, int Sy, int Sw, int Sh );
void BltBitmapTransparent( HDC hdcDst, int Dx, int Dy, int Dw, int Dh, HBITMAP hbmSrc, int Sx, int Sy, int Sw, int Sh, COLORREF TransparentColor );
void CopyBitmap( HBITMAP hbmDst, int Dx, int Dy, int Dw, int Dh, HBITMAP hbmSrc, int Sx, int Sy, int Sw, int Sh );
void CopyBitmapPreserveAspectRatio( HBITMAP hbmDst, int Dx, int Dy, int Dw, int Dh, HBITMAP hbmSrc, int Sx, int Sy, int Sw, int Sh );
void CopyDC( HBITMAP hbmDst, int Dx, int Dy, int Dw, int Dh, HDC hdcSrc, int Sx, int Sy, int Sw, int Sh );
COLORREF GetSubPixel( int nChannel, int x, int y );
void SetSubPixel( int nChannel, int x, int y, COLORREF c );
BOOL WriteDIB( HANDLE hFile, HBITMAP hbm, HPALETTE hpal );
BOOL SaveDIB( LPCTSTR lpszFileName, HBITMAP hbm, HPALETTE hpal );
HBITMAP LoadDIB( LPCTSTR lpszFileName );
HBITMAP LoadTGA( LPCTSTR lpszFileName );
BOOL WriteTGA( HANDLE hFile, HBITMAP hbm, HPALETTE hpal );

//////////////////////////////////////////////////////////////////////////

BITMAPINFO *CreateBitmapHeader( HBITMAP hbm );
BITMAPINFO *DeleteBitmapHeader( BITMAPINFO *pbmi );
DWORD GetBitmapHeaderSize( BITMAPINFO *pbmi );

//////////////////////////////////////////////////////////////////////////

void BlendPixel( int Channel, int x, int y, COLORREF c, BLENDMODE Blend );

//////////////////////////////////////////////////////////////////////////

extern BITMAP2 GxBitmap[16];

inline void GxSetPixel( int ch, int x, int y, COLOR32 c )
{
	SetPixelFn[ch]( GxBitmap[ch], x, y, c );
}

inline COLOR32 GxGetPixel( int ch, int x, int y )
{
	return GetPixelFn[ch]( GxBitmap[ch], x, y );
}

inline void SetPixelF( int ch, int x, int y, COLOR32 c )
{
	GxSetPixel( ch, x, y, c );
}

inline void SetPixelU( int ch, int x, int y, COLOR32 c )
{
	GxSetPixel( ch, x, y, c );
}

inline COLOR32 GetPixelF( int ch, int x, int y )
{
	return GxGetPixel( ch, x, y );
}

inline COLOR32 GetPixelU( int ch, int x, int y )
{
	return GxGetPixel( ch, x, y );
}

#endif // __GRAPHICS_H__