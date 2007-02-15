//=============================================================================//
//  Copyright ©2001 Avalanche Software.  All Rights Reserved.                  //
//=============================================================================//
//      File: graphics.cpp                                                     //
//    Author: Daniel Silver                                                    //
//   Created: 05/02/2001                                                       //
//=============================================================================//

#include "pch.h"
#include "3dmath.h"
#include "fileio.h"
#include "gdi.h"
#include "graphics.h"
#include "tstring.h"

static HBITMAP hbmSrc;
static HBITMAP hbmDst;
static BITMAP bmSrc;
static BITMAP bmDst;

void SetPixel0 ( int x, int y, COLORREF c );
void SetPixel8 ( int x, int y, COLORREF c );
void SetPixel16( int x, int y, COLORREF c );
void SetPixel24( int x, int y, COLORREF c );
void SetPixel32( int x, int y, COLORREF c );

COLORREF GetPixel0 ( int x, int y );
COLORREF GetPixel8 ( int x, int y );
COLORREF GetPixel16( int x, int y );
COLORREF GetPixel24( int x, int y );
COLORREF GetPixel32( int x, int y );

//=============================================================================
// PixelFormatToBPP
//=============================================================================

int PixelFormatToBPP( PIXELFORMAT Format )
{
	switch( Format )
	{
	case PIXELFMT_A8R8G8B8:
	case PIXELFMT_A8B8G8R8:
	case PIXELFMT_X8R8G8B8:
	case PIXELFMT_X8B8G8R8:
	case PIXELFMT_INDEX32:
		return 32;
	case PIXELFMT_R8G8B8:
	case PIXELFMT_B8G8R8:
	case PIXELFMT_INDEX24:
		return 24;
	case PIXELFMT_A4R4G4B4:
	case PIXELFMT_A4B4G4R4:
	case PIXELFMT_X4R4G4B4:
	case PIXELFMT_X4B4G4R4:
	case PIXELFMT_A1R5G5B5:
	case PIXELFMT_A1B5G5R5:
	case PIXELFMT_X1R5G5B5:
	case PIXELFMT_X1B5G5R5:
	case PIXELFMT_R5G6B5:
	case PIXELFMT_B5G6R5:
	case PIXELFMT_INDEX16:
		return 16;
	case PIXELFMT_PALETTE8:
	case PIXELFMT_GRAY8:
	case PIXELFMT_INDEX8:
		return 8;
	case PIXELFMT_PALETTE4:
	case PIXELFMT_GRAY4:
	case PIXELFMT_INDEX4:
		return 4;
	}

	return 0;
}

//=============================================================================
// PixelFormatFromBPP
//=============================================================================

PIXELFORMAT PixelFormatFromBPP( int BPP )
{
	switch( BPP )
	{
	case 32:
		return PIXELFMT_A8R8G8B8;
	case 24:
		return PIXELFMT_R8G8B8;
	case 16:
		return PIXELFMT_X1R5G5B5; // TODO: hmm should this be PIXELFMT_A1R5G5B5??
	case 8:
		return PIXELFMT_PALETTE8;
	case 4:
		return PIXELFMT_PALETTE4;
	case 1:
		return PIXELFMT_MONOCHROME;
	}

	return PIXELFMT_UNKNOWN;
}

//=============================================================================
// PixelFormatToString
//=============================================================================

char * PixelFormatToString( PIXELFORMAT Format )
{
	switch( Format )
	{
	case PIXELFMT_A8R8G8B8:
		return "PIXELFMT_A8R8G8B8";

	case PIXELFMT_A8B8G8R8:
		return "PIXELFMT_A8B8G8R8";

	case PIXELFMT_X8R8G8B8:
		return "PIXELFMT_X8R8G8B8";

	case PIXELFMT_X8B8G8R8:
		return "PIXELFMT_X8B8G8R8";

	case PIXELFMT_R8G8B8:
		return "PIXELFMT_R8G8B8";

	case PIXELFMT_B8G8R8:
		return "PIXELFMT_B8G8R8";

	case PIXELFMT_A4R4G4B4:
		return "PIXELFMT_A4R4G4B4";

	case PIXELFMT_A4B4G4R4:
		return "PIXELFMT_A4B4G4R4";

	case PIXELFMT_X4R4G4B4:
		return "PIXELFMT_X4R4G4B4";

	case PIXELFMT_X4B4G4R4:
		return "PIXELFMT_X4B4G4R4";

	case PIXELFMT_A1R5G5B5:
		return "PIXELFMT_A1R5G5B5";

	case PIXELFMT_A1B5G5R5:
		return "PIXELFMT_A1B5G5R5";

	case PIXELFMT_X1R5G5B5:
		return "PIXELFMT_X1R5G5B5";

	case PIXELFMT_X1B5G5R5:
		return "PIXELFMT_X1B5G5R5";

	case PIXELFMT_R5G6B5:
		return "PIXELFMT_R5G6B5";

	case PIXELFMT_B5G6R5:
		return "PIXELFMT_B5G6R5";

	case PIXELFMT_PALETTE8:
		return "PIXELFMT_PALETTE8";

	case PIXELFMT_PALETTE4:
		return "PIXELFMT_PALETTE4";

	case PIXELFMT_GRAY8:
		return "PIXELFMT_GRAY8";

	case PIXELFMT_GRAY4:
		return "PIXELFMT_GRAY4";

	case PIXELFMT_MONOCHROME:
		return "PIXELFMT_MONOCHROME";

	case PIXELFMT_INDEX32:
		return "PIXELFMT_INDEX32";

	case PIXELFMT_INDEX24:
		return "PIXELFMT_INDEX24";

	case PIXELFMT_INDEX16:
		return "PIXELFMT_INDEX16";

	case PIXELFMT_INDEX8:
		return "PIXELFMT_INDEX8";

	case PIXELFMT_INDEX4:
		return "PIXELFMT_INDEX4";
	}

	return "PIXELFMT_UNKNOWN";
}

//=============================================================================
// PixelFormatWithAlpha
//=============================================================================

PIXELFORMAT PixelFormatWithAlpha( PIXELFORMAT Format )
{
	switch( Format )
	{
	case PIXELFMT_A8R8G8B8:
	case PIXELFMT_X8R8G8B8:
		return PIXELFMT_A8R8G8B8;

	case PIXELFMT_A8B8G8R8:
	case PIXELFMT_X8B8G8R8:
		return PIXELFMT_A8B8G8R8;

	case PIXELFMT_A4R4G4B4:
	case PIXELFMT_X4R4G4B4:
		return PIXELFMT_A4R4G4B4;

	case PIXELFMT_A4B4G4R4:
	case PIXELFMT_X4B4G4R4:
		return PIXELFMT_A4B4G4R4;

	case PIXELFMT_A1R5G5B5:
	case PIXELFMT_X1R5G5B5:
		return PIXELFMT_A1R5G5B5;

	case PIXELFMT_A1B5G5R5:
	case PIXELFMT_X1B5G5R5:
		return PIXELFMT_A1B5G5R5;

	case PIXELFMT_PALETTE8:
		return PIXELFMT_PALETTE8;

	case PIXELFMT_PALETTE4:
		return PIXELFMT_PALETTE4;
	}

	return PIXELFMT_UNKNOWN;
}

//=============================================================================
// PixelFormatWithoutAlpha
//=============================================================================

PIXELFORMAT PixelFormatWithoutAlpha( PIXELFORMAT Format )
{
	switch( Format )
	{
	case PIXELFMT_A8R8G8B8:
		return PIXELFMT_X8R8G8B8;

	case PIXELFMT_A8B8G8R8:
		return PIXELFMT_X8B8G8R8;

	case PIXELFMT_A4R4G4B4:
		return PIXELFMT_X4R4G4B4;

	case PIXELFMT_A4B4G4R4:
		return PIXELFMT_X4B4G4R4;

	case PIXELFMT_A1R5G5B5:
		return PIXELFMT_X1R5G5B5;

	case PIXELFMT_A1B5G5R5:
		return PIXELFMT_X1B5G5R5;
	}

	return Format;
}

//=============================================================================
// GetImageStride
//=============================================================================

int GetImageStride( int Width, int BPP )
{
	return (Width * BPP + 7) / 8;
}

//=============================================================================
// GetImageStride
//=============================================================================

int GetImageStride( int Width, PIXELFORMAT Format )
{
	return GetImageStride( Width, PixelFormatToBPP( Format ) );
}

//=============================================================================
// GetBitmap2Struct
//=============================================================================

BITMAP2 GetBitmap2Struct( HBITMAP hbm, BOOL Flipped )
{
	BITMAP bm = GetBitmapStruct( hbm );
	BITMAP2 bm2;

	bm2.Width = bm.bmWidth;
	bm2.Height = bm.bmHeight;
	bm2.Format = PixelFormatFromBPP( bm.bmBitsPixel );
	bm2.Stride = GetBitmapStride( hbm );
	bm2.pBits = (BYTE *) bm.bmBits;

	if( Flipped )
	{
		bm2.pBits = &bm2.pBits[(bm2.Height - 1) * bm2.Stride];
		bm2.Stride = -bm2.Stride;
	}

	GdiGetColorTable( hbm, (RGBQUAD *) bm2.Palette, 256 );

	return bm2;
}

//=============================================================================
// BeginBitmap
//=============================================================================

void BeginBitmap( int channel, BITMAP2 *pBitmap )
{
	if( pBitmap )
	{
		BeginBitmap( channel, pBitmap->Width, pBitmap->Height, pBitmap->Format, pBitmap->Stride, pBitmap->pBits );
		SetPalette( channel, pBitmap->Palette, 256 );
	}

	return;
}

//=============================================================================
// BeginDIB
//=============================================================================

void BeginDIB( int channel, HBITMAP hbm, PIXELFORMAT Format )
{
	BITMAP bm = GetBitmapStruct( hbm );

	if( Format == PIXELFMT_UNKNOWN )
		Format = PixelFormatFromBPP( bm.bmBitsPixel );

	BeginBitmap( channel, bm.bmWidth, bm.bmHeight, Format, GetBitmapStride( hbm ), bm.bmBits );

	RGBQUAD ColorTable[256];
	GdiGetColorTable( hbm, ColorTable, 256 );

	SetPalette( channel, (COLOR32 *) ColorTable, 256 );
}

//=============================================================================
// CreateBitmap2
//=============================================================================

BITMAP2 *CreateBitmap2( int Width, int Height, PIXELFORMAT Format, COLOR32 *pPalette, int PaletteSize )
{
	int BPP = PixelFormatToBPP( Format );
	int Stride = (Width * BPP + 7) / 8;
	int BitmapSize = Stride * Height;

	BITMAP2 *pBitmap = (BITMAP2 *) new BYTE[sizeof(BITMAP2)+BitmapSize];

	if( pBitmap )
	{
		pBitmap->Width = Width;
		pBitmap->Height = Height;
		pBitmap->Format = Format;
		pBitmap->Stride = Stride;
		pBitmap->pBits = (BYTE *) &pBitmap[1];

		if( pPalette )
			SetColorTable( pBitmap, pPalette, 0, PaletteSize );
	}

	return pBitmap;
}

//=============================================================================
// DeleteBitmap2
//=============================================================================

BITMAP2 *DeleteBitmap2( BITMAP2 *pBitmap )
{
	if( pBitmap )
		delete [] pBitmap;

	return 0;
}

//=============================================================================
// SetColorTable
//=============================================================================

void SetColorTable( BITMAP2 *pBitmap, COLOR32 *pPalette, int BeginIndex, int PaletteSize )
{
	if( pBitmap && pPalette )
	{
		if( PaletteSize > 256 - BeginIndex )
			PaletteSize = 256 - BeginIndex;

		for( int i = 0; i < PaletteSize; i++ )
			pBitmap->Palette[BeginIndex+i] = pPalette[i];
	}

	return;
}

//=============================================================================
// ReadScanline
//=============================================================================

void ReadScanline( COLOR32* pScanlineBits, int x, int y, int Count, void *pSrc, int Sw, int Sh, PIXELFORMAT SrcFormat, int SrcStride )
{
	BeginBitmap( 0, Sw, Sh, SrcFormat, SrcStride, pSrc );

	if( GetScanlineFn[0] )
		GetScanlineFn[0]( 0, pScanlineBits, x, y, Count );

	FinishBitmap(0);
}

//=============================================================================
// WriteScanline
//=============================================================================

void WriteScanline( COLOR32* pScanlineBits, int x, int y, int Count, void* pDst, int Dw, int Dh, PIXELFORMAT DstFormat, int DstStride )
{
	BeginBitmap( 0, Dw, Dh, DstFormat, DstStride, pDst );

	if( SetScanlineFn[0] )
		SetScanlineFn[0]( 0, pScanlineBits, x, y, Count );

	FinishBitmap(0);
}

//=============================================================================
// BitmapToBitmap
//=============================================================================

void BitmapToBitmap( BITMAP2 *pDst, int Dx, int Dy, int Dw, int Dh, BITMAP2 *pSrc, int Sx, int Sy, int Sw, int Sh )
{
	BitmapToBitmap( pDst->pBits, pDst->Width, pDst->Height, pDst->Format, pDst->Stride, Dx, Dy, Dw, Dh,
						 pSrc->pBits, pSrc->Width, pSrc->Height, pSrc->Format, pSrc->Stride, Sx, Sy, Sw, Sh );
}

//=============================================================================
// BitmapToBitmap
//=============================================================================

void BitmapToBitmap( void *pDst, int DstWidth, int DstHeight, PIXELFORMAT DstFormat, int DstStride, int Dx, int Dy, int Dw, int Dh, void *pSrc, int SrcWidth, int SrcHeight, PIXELFORMAT SrcFormat, int SrcStride, int Sx, int Sy, int Sw, int Sh )
{
	BeginBitmap( 0, DstWidth, DstHeight, DstFormat, DstStride, pDst );
	BeginBitmap( 1, SrcWidth, SrcHeight, SrcFormat, SrcStride, pSrc );

	int x, y;
	int n, c;

	if( Dw == Sw && Dh == Sh )
	{
	//	for( y = 0; y < Dh; y++ )
	//		for( x = 0; x < Dw; x++ )
	//			SetPixelF( 0, Dx+x, Dy+y, GetPixelF( 1, Sx+x, Sy+y ) );

		COLOR32 ScanlineBits[1024];

		c = sizeof(ScanlineBits)/sizeof(ScanlineBits[0]);

		for( y = 0; y < Dh; y++ )
		{
			for( x = 0; x < Dw; x += n )
			{
				if( c > Dw - x )
					n = Dw - x;
				else
					n = c;

				ReadScanline( ScanlineBits, Sx+x, Sy+y, n, pSrc, Sw, Sh, SrcFormat, SrcStride );
				WriteScanline( ScanlineBits, Dx+x, Dy+y, n, pDst, Dw, Dh, DstFormat, DstStride );
			}
		}
	}
	else
	{
		int Tx = (Sw<<16) / Dw;
		int Ty = (Sh<<16) / Dh;
		int Px = Sx<<16;
		int Py = Sy<<16;

		int x2 = Dx + Dw;
		int y2 = Dy + Dh;

		for( y = Dy; y < y2; y++ )
		{
			Px = Sx << 16;

			for( x = Dx; x < x2; x++ )
			{
				SetPixelF( 0, x, y, GetPixelF( 1, Px>>16, Py>>16 ) );

				Px = Px + Tx;
			}

			Py = Py + Ty;
		}
	}

	FinishBitmap(0);
	FinishBitmap(1);
}

//=============================================================================
// BitmapToDC
//=============================================================================

void BitmapToDC( HDC hdcDst, int Dx, int Dy, int Dw, int Dh, BITMAP2 *pSrc, int Sx, int Sy, int Sw, int Sh )
{
	if( pSrc )
	{
		BITMAPINFO bmi;

		bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
		bmi.bmiHeader.biWidth = pSrc->Width;
		bmi.bmiHeader.biHeight = -pSrc->Height;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = PixelFormatToBPP( pSrc->Format );
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = 0;
		bmi.bmiHeader.biXPelsPerMeter = 0;
		bmi.bmiHeader.biYPelsPerMeter = 0;
		bmi.bmiHeader.biClrUsed = 0;
		bmi.bmiHeader.biClrImportant = 0;

		SetStretchBltMode( hdcDst, COLORONCOLOR );
		StretchDIBits( hdcDst, Dx, Dy, Dw, Dh, Sx, Sy, Sw, Sh, pSrc->pBits, &bmi, DIB_RGB_COLORS, SRCCOPY );
	}

	return;
}

//=============================================================================
// FastLine
//=============================================================================

void FastLine( int channel, int x1, int y1, int x2, int y2, COLORREF c )
{
	if( x1 > x2 )
		FastLine( channel, x2, y2, x1, y1, c );
	else
	{
		int Dx = x2 - x1;
		int Dy = y2 - y1;
		int Height;
		int Zx, Zy;
		int x, y;

		if( Dy < 0 )
		{
			Height = -Dy;

			if( Dx > Height )
			{
				y = (y1 << 16) + 32768;
				Zy = (Dy << 16) / Dx;

				for( x = x1; x <= x2; x++ )
				{
					SetPixelF( channel, x, y >> 16, c );

					y = y + Zy;
				}
			}
			else if( Dx < Height )
			{
				x = (x1 << 16) + 32768;
				Zx = (Dx << 16) / Height;

				for( y = y1; y >= y2; y-- )
				{
					SetPixelF( channel, x >> 16, y, c );

					x = x + Zx;
				}
			}
			else
			{
				for( x = 0; x <= Dx; x++ )
					SetPixelF( channel, x1+x, y1-x, c );
			}
		}
		else if( Dy > 0 )
		{
			Height = Dy;

			if( Dx > Height )
			{
				y = (y1 << 16) + 32768;
				Zy = (Dy << 16) / Dx;

				for( x = x1; x <= x2; x++ )
				{
					SetPixelF( channel, x, y >> 16, c );

					y = y + Zy;
				}
			}
			else if( Dx < Height )
			{
				x = (x1 << 16) + 32768;
				Zx = (Dx << 16) / Dy;

				for( y = y1; y <= y2; y++ )
				{
					SetPixelF( channel, x >> 16, y, c );

					x = x + Zx;
				}
			}
			else
			{
				for( x = 0; x <= Dx; x++ )
					SetPixelF( channel, x1+x, y1+x, c );
			}
		}
		else
		{
			for( x = x1; x <= x2; x++ )
				SetPixelF( channel, x, y1, c );
		}
	}

	return;
}

//=============================================================================
// TransparentLine
//=============================================================================

void TransparentLine( int channel, int x1, int y1, int x2, int y2, COLORREF c, BLENDMODE BlendMode )
{
	if( x1 > x2 )
		TransparentLine( channel, x2, y2, x1, y1, c, BlendMode );
	else
	{
		int Dx = x2 - x1;
		int Dy = y2 - y1;
		int Height;
		int Zx, Zy;
		int x, y;

		if( Dy < 0 )
		{
			Height = -Dy;

			if( Dx > Height )
			{
				y = (y1 << 16) + 32768;
				Zy = (Dy << 16) / Dx;

				for( x = x1; x <= x2; x++ )
				{
					BlendPixel( channel, x, y >> 16, c, BlendMode );

					y = y + Zy;
				}
			}
			else if( Dx < Height )
			{
				x = (x1 << 16) + 32768;
				Zx = (Dx << 16) / Height;

				for( y = y1; y >= y2; y-- )
				{
					BlendPixel( channel, x >> 16, y, c, BlendMode );

					x = x + Zx;
				}
			}
			else
			{
				for( x = 0; x <= Dx; x++ )
					BlendPixel( channel, x1+x, y1-x, c, BlendMode );
			}
		}
		else if( Dy > 0 )
		{
			Height = Dy;

			if( Dx > Height )
			{
				y = (y1 << 16) + 32768;
				Zy = (Dy << 16) / Dx;

				for( x = x1; x <= x2; x++ )
				{
					BlendPixel( channel, x, y >> 16, c, BlendMode );

					y = y + Zy;
				}
			}
			else if( Dx < Height )
			{
				x = (x1 << 16) + 32768;
				Zx = (Dx << 16) / Dy;

				for( y = y1; y <= y2; y++ )
				{
					BlendPixel( channel, x >> 16, y, c, BlendMode );

					x = x + Zx;
				}
			}
			else
			{
				for( x = 0; x <= Dx; x++ )
					BlendPixel( channel, x1+x, y1+x, c, BlendMode );
			}
		}
		else
		{
			for( x = x1; x <= x2; x++ )
				BlendPixel( channel, x, y1, c, BlendMode );
		}
	}

	return;
}

//=============================================================================
// FastRect
//=============================================================================

void FastRect( int channel, int x1, int y1, int x2, int y2, COLORREF c )
{
	FastLine( channel, x1, y1, x2, y1, c );
	FastLine( channel, x2, y1, x2, y2, c );
	FastLine( channel, x2, y2, x1, y2, c );
	FastLine( channel, x1, y2, x1, y1, c );
}

//=============================================================================
// DrawEllipse
//=============================================================================

#define DEG2RAD(d) ((d) * (3.14159f/180.0f))

void DrawEllipse( int channel, int x1, int y1, int x2, int y2, COLORREF c )
{
	int rx = (x2 - x1) / 2;
	int ry = (y2 - y1) / 2;
	int cx = x1 + rx;
	int cy = y1 + ry;
	int x3, y3;
	int x4, y4;

	int Segments = 128;

	float T = 0.0f;
	float N = 360.0f / Segments;

	x3 = cx + (int) (sinf(0.0f) * rx);
	y3 = cy + (int) (cosf(0.0f) * ry);

	for( int i = 1; i <= Segments; i++ )
	{
		x4 = cx + (int) (sinf(DEG2RAD(T)) * rx);
		y4 = cy + (int) (cosf(DEG2RAD(T)) * ry);

		FastLine( channel, x3, y3, x4, y4, c );

		T = T + N;

		x3 = x4;
		y3 = y4;
	}

	return;
}

//=============================================================================
// ColorBlend
//=============================================================================

COLORREF ColorBlend( COLORREF crDst, COLORREF crSrc, int A )
{
	return (((crSrc >> 8 & 0xff0000) * A + (crDst >> 8 & 0xff0000) * (256 - A)) >> 0 & 0xff000000)|
			 (((crSrc >> 0 & 0xff0000) * A + (crDst >> 0 & 0xff0000) * (256 - A)) >> 8 & 0x00ff0000)|
			 (((crSrc >> 0 & 0x00ff00) * A + (crDst >> 0 & 0x00ff00) * (256 - A)) >> 8 & 0x0000ff00)|
			 (((crSrc >> 0 & 0x0000ff) * A + (crDst >> 0 & 0x0000ff) * (256 - A)) >> 8 & 0x000000ff);
}

//=============================================================================
// ColorMultiply
//=============================================================================

COLORREF ColorMultiply( COLORREF crDst, COLORREF crSrc )
{
	return ((crSrc >> 8 & 0xff0000) * (crDst >> 24 & 0xff) >> 0 & 0xff000000)|
			 ((crSrc >> 0 & 0xff0000) * (crDst >> 16 & 0xff) >> 8 & 0x00ff0000)|
		    ((crSrc >> 0 & 0x00ff00) * (crDst >>  8 & 0xff) >> 8 & 0x0000ff00)|
			 ((crSrc >> 0 & 0x0000ff) * (crDst >>  0 & 0xff) >> 8 & 0x000000ff);
}

//=============================================================================
// ColorAdd
//=============================================================================

COLORREF ColorAdd( COLORREF crColor1, COLORREF crColor2 )
{
	unsigned int A = (crColor1 >> 24 & 0xff) + (crColor2 >> 24 & 0xff);
	unsigned int R = (crColor1 >> 16 & 0xff) + (crColor2 >> 16 & 0xff);
	unsigned int G = (crColor1 >>  8 & 0xff) + (crColor2 >>  8 & 0xff);
	unsigned int B = (crColor1 >>  0 & 0xff) + (crColor2 >>  0 & 0xff);

	if( A > 255 ) A = 255;
	if( R > 255 ) R = 255;
	if( G > 255 ) G = 255;
	if( B > 255 ) B = 255;

	return A<<24|R<<16|G<<8|B;
}

//=============================================================================
// ColorSub
//=============================================================================

COLORREF ColorSub( COLORREF crColor1, COLORREF crColor2 )
{
	unsigned int A = (crColor1 >> 24 & 0xff) - (crColor2 >> 24 & 0xff);
	unsigned int R = (crColor1 >> 16 & 0xff) - (crColor2 >> 16 & 0xff);
	unsigned int G = (crColor1 >>  8 & 0xff) - (crColor2 >>  8 & 0xff);
	unsigned int B = (crColor1 >>  0 & 0xff) - (crColor2 >>  0 & 0xff);

	if( A > 255 ) A = 0;
	if( R > 255 ) R = 0;
	if( G > 255 ) G = 0;
	if( B > 255 ) B = 0;

	return A<<24|R<<16|G<<8|B;
}

//=============================================================================
// CreateDIB
//=============================================================================

HBITMAP CreateDIB( int nWidth, int nHeight, int nBPP )
{
	BITMAPINFO bmi;

	bmi.bmiHeader.biSize				= sizeof( bmi.bmiHeader );
	bmi.bmiHeader.biWidth			= nWidth;
	bmi.bmiHeader.biHeight			= -nHeight;
	bmi.bmiHeader.biPlanes			= 1;
	bmi.bmiHeader.biBitCount		= nBPP;
	bmi.bmiHeader.biCompression	= BI_RGB;
	bmi.bmiHeader.biSizeImage		= 0;
	bmi.bmiHeader.biXPelsPerMeter	= 0;
	bmi.bmiHeader.biYPelsPerMeter	= 0;
	bmi.bmiHeader.biClrUsed			= 0;
	bmi.bmiHeader.biClrImportant	= 0;

	return CreateDIBSection( NULL, &bmi, DIB_RGB_COLORS, (VOID **) &bmi.bmiColors[0], NULL, NULL );
}

//=============================================================================
// CloneBitmap2
//=============================================================================

HBITMAP CloneBitmap2( BITMAP2 *pbm )
{
	HBITMAP hbm = 0;

	if( pbm )
	{
		int Width = pbm->Width;
		int Height = pbm->Height;

		hbm = CreateDIB( Width, Height, PixelFormatToBPP( pbm->Format ) );

		if( hbm )
		{
			BITMAP2 dst = GetBitmap2Struct(hbm, FALSE);

			BitmapToBitmap( &dst, 0, 0, Width, Height, pbm, 0, 0, Width, Height );
			
			GdiSetColorTable( hbm, (RGBQUAD*) pbm->Palette, 256 );
		}
	}

	return hbm;
}

//=============================================================================
// ReleaseDIB
//=============================================================================

HBITMAP ReleaseDIB( HBITMAP hbm )
{
	if( DeleteObject( hbm ) )
		return NULL;

	return hbm;
}

//=============================================================================
// CloneDIB
//=============================================================================

HBITMAP CloneDIB( HBITMAP hbmSrc )
{
	return CloneDIB( hbmSrc, FALSE );
}

//=============================================================================
// CloneDIB
//=============================================================================

HBITMAP CloneDIB( HBITMAP hbmSrc, BOOL bBlank )
{
	int nWidth  = GetBitmapWidth( hbmSrc );
	int nHeight = GetBitmapHeight( hbmSrc );
	int nBPP    = GetBitmapBPP( hbmSrc );

	HBITMAP hbmDst = CreateDIB( nWidth, nHeight, nBPP );

	if( hbmDst )
	{
		if( nBPP <= 8 )
			GdiCopyColorTable( hbmDst, hbmSrc );

		if( !bBlank )
			CopyBitmap( hbmDst, 0, 0, nWidth, nHeight,
							hbmSrc, 0, 0, nWidth, nHeight );
	}

	return hbmDst;
}

//=============================================================================
// HighQualityStretchBlt
//=============================================================================

void HighQualityStretchBlt( HDC hdcDst, int Dx, int Dy, int Dw, int Dh, HBITMAP hbmSrc, int Sx, int Sy, int Sw, int Sh )
{
	if( Dw == Sw && Sh == Dh )
		BltBitmap( hdcDst, Dx, Dy, Dw, Dh, hbmSrc, Sx, Sy, Sw, Sh );
	else
	{
		HBITMAP hbmTemp = ResampleDIB( hbmSrc, Sx, Sy, Sw, Sh, Dw, Dh );

		BltBitmap( hdcDst, Dx, Dy, Dw, Dh, hbmTemp, 0, 0, Dw, Dh );

		ReleaseDIB( hbmTemp );
	}

	return;
}

//=============================================================================
// HighQualityCopyBitmap
//=============================================================================

void HighQualityCopyBitmap( HBITMAP hbmDst, int Dx, int Dy, int Dw, int Dh, HBITMAP hbmSrc, int Sx, int Sy, int Sw, int Sh )
{
	HDC hdcDst = CreateCompatibleDC( NULL );

	hbmDst = (HBITMAP) SelectObject( hdcDst, hbmDst );

	HighQualityStretchBlt( hdcDst, Dx, Dy, Dw, Dh, hbmSrc, Sx, Sy, Sw, Sh );

	hbmDst = (HBITMAP) SelectObject( hdcDst, hbmDst );

	DeleteDC( hdcDst );
}

//=============================================================================
// ResampleDIB
//=============================================================================

HBITMAP ResampleDIB( HBITMAP hbmSrc, int Sx, int Sy, int Sw, int Sh, int Dw, int Dh )
{
	if( Sw == 0 ) Sw = GetBitmapWidth( hbmSrc );
	if( Sh == 0 ) Sh = GetBitmapHeight( hbmSrc );

	HBITMAP hbmTemp = CreateDIB( Sw, Sh, GetBitmapBPP( hbmSrc ) );

	CopyBitmap( hbmTemp, 0, 0, Sw, Sh, hbmSrc, Sx, Sy, Sw, Sh );

	BeginDIB( 0, hbmTemp );

	int bmWidth = GetBitmapWidth( hbmTemp );
	int bmHeight = GetBitmapHeight( hbmTemp );

	while( (bmWidth>>1) >= Dw )
	{
		bmWidth >>= 1;

		for( int y = 0; y < bmHeight; y++ )
		{
			for( int x = 0; x < bmWidth; x++ )
			{
				SetPixelU( 0, x, y, ColorBlend( GetPixelU( 0, (x<<1)+0, y ),
														  GetPixelU( 0, (x<<1)+1, y ), 128 ) );
			}
		}
	}

	while( (bmHeight>>1) >= Dh )
	{
		bmHeight >>= 1;

		for( int y = 0; y < bmHeight; y++ )
		{
			for( int x = 0; x < bmWidth; x++ )
			{
				SetPixelU( 0, x, y, ColorBlend( GetPixelU( 0, x, (y<<1)+0 ),
														  GetPixelU( 0, x, (y<<1)+1 ), 128 ) );
			}
		}
	}

	FinishBitmap(0);

	HBITMAP hbmResult = CreateDIB( Dw, Dh, GetBitmapBPP( hbmSrc ) );

	BeginDIB( 0, hbmResult );
	BeginDIB( 1, hbmTemp );

	int X;
	int Y;
	int U;
	int V;
	int StepU = bmWidth * 65536 / Dw;
	int StepV = bmHeight * 65536 / Dh;

	for( V = Y = 0; Y < Dh; Y++, V += StepV )
		for( U = X = 0; X < Dw; X++, U += StepU )
			SetPixelU( 0, X, Y, GetSubPixel( 1, U, V ) );

	FinishBitmap(0);
	FinishBitmap(1);

	ReleaseDIB( hbmTemp );

	return hbmResult;
}

//=============================================================================
// StretchBltPreserveAspectRatio
//=============================================================================

void StretchBltPreserveAspectRatio( HDC hdcDst, int Dx, int Dy, int Dw, int Dh, HBITMAP hbmSrc, int Sx, int Sy, int Sw, int Sh )
{
	float AspectRatio = Sh / (float) Sw;
	int Cx, Cy;
	int Cw, Ch;

	if( AspectRatio < 1.0f )
	{
		Cw = Dw;
		Ch = (int) (Dw * AspectRatio);
	}
	else
	{
		Ch = Dh;
		Cw = (int) (Dh / AspectRatio);
	}

	Cx = Dx + (Dw - Cw) / 2;
	Cy = Dy + (Dh - Ch) / 2;

	BltBitmap( hdcDst, Cx, Cy, Cw, Ch, hbmSrc, Sx, Sy, Sw, Sh );
}

//=============================================================================
// ClearDIB
//=============================================================================

void ClearDIB( HBITMAP hbm, COLORREF c )
{
	BITMAP bm = GetBitmapStruct( hbm );
	HDC hdc = CreateCompatibleDC( NULL );
	HBRUSH hbr = CreateSolidBrush( (c >> 16 & 0x0000ff) | (c & 0x00ff00) | (c << 16 & 0xff0000) );

	RECT rc;
	
	rc.left   = 0;
	rc.top    = 0;
	rc.right  = bm.bmWidth;
	rc.bottom = bm.bmHeight;

	hbm = (HBITMAP) SelectObject( hdc, hbm );

	FillRect( hdc, &rc, hbr );

	hbm = (HBITMAP) SelectObject( hdc, hbm );

	DeleteObject( hbr );
	DeleteDC( hdc );
}

//=============================================================================
// GetBitmapStruct
//=============================================================================

BITMAP GetBitmapStruct( HBITMAP hbm )
{
	BITMAP bm;

	if( GetObject( hbm, sizeof(bm), &bm ) )
		return bm;

	bm.bmType		= 0;
	bm.bmWidth		= 0;
	bm.bmHeight		= 0;
	bm.bmPlanes		= 0;
	bm.bmBitsPixel	= 0;
	bm.bmBits		= 0;

	return bm;
}

//=============================================================================
// GetBitmapWidth
//=============================================================================

int GetBitmapWidth( HBITMAP hbm )
{
	BITMAP bm = GetBitmapStruct( hbm );

	return bm.bmWidth;
}

//=============================================================================
// GetBitmapHeight
//=============================================================================

int GetBitmapHeight( HBITMAP hbm )
{
	BITMAP bm = GetBitmapStruct( hbm );

	return bm.bmHeight;
}

//=============================================================================
// GetBitmapBPP
//=============================================================================

int GetBitmapBPP( HBITMAP hbm )
{
	BITMAP bm = GetBitmapStruct( hbm );

	return bm.bmBitsPixel;
}

//=============================================================================
// GetBitmapData
//=============================================================================

BYTE *GetBitmapData( HBITMAP hbm )
{
	BITMAP bm = GetBitmapStruct( hbm );

	return (BYTE *) bm.bmBits;
}

//=============================================================================
// GetBitmapStride()
//=============================================================================

int GetBitmapStride( HBITMAP hbm )
{
	BITMAP bm = GetBitmapStruct( hbm );

	return bm.bmWidthBytes;
}

//=============================================================================
// BltBitmap
//=============================================================================

void BltBitmap( HDC hdcDst, int Dx, int Dy, int Dw, int Dh, HBITMAP hbmSrc, int Sx, int Sy, int Sw, int Sh )
{
	if( Sw == 0 ) Sw = GetBitmapWidth( hbmSrc );
	if( Sh == 0 ) Sh = GetBitmapHeight( hbmSrc );
	if( Dw == 0 ) Dw = Sw;
	if( Dh == 0 ) Dh = Sh;

	HDC hdcSrc = CreateCompatibleDC( hdcDst );

	hbmSrc = (HBITMAP) SelectObject( hdcSrc, hbmSrc );

	SetStretchBltMode( hdcDst, COLORONCOLOR );

	StretchBlt( hdcDst, Dx, Dy, Dw, Dh, hdcSrc, Sx, Sy, Sw, Sh, SRCCOPY );

	hbmSrc = (HBITMAP) SelectObject( hdcSrc, hbmSrc );

	DeleteDC( hdcSrc );
}

//=============================================================================
// BltBitmapTransparent
//=============================================================================

void BltBitmapTransparent( HDC hdcDst, int Dx, int Dy, int Dw, int Dh, HBITMAP hbmSrc, int Sx, int Sy, int Sw, int Sh, COLORREF TransparentColor )
{
	if( Sw == 0 ) Sw = GetBitmapWidth( hbmSrc );
	if( Sh == 0 ) Sh = GetBitmapHeight( hbmSrc );
	if( Dw == 0 ) Dw = Sw;
	if( Dh == 0 ) Dh = Sh;

	HDC hdcSrc = CreateCompatibleDC( hdcDst );

	hbmSrc = (HBITMAP) SelectObject( hdcSrc, hbmSrc );

	SetStretchBltMode( hdcDst, COLORONCOLOR );

	TransparentBlt( hdcDst, Dx, Dy, Dw, Dh, hdcSrc, Sx, Sy, Sw, Sh, TransparentColor );

	hbmSrc = (HBITMAP) SelectObject( hdcSrc, hbmSrc );

	DeleteDC( hdcSrc );
}

//=============================================================================
// CopyBitmap
//=============================================================================

void CopyBitmap( HBITMAP hbmDst, int Dx, int Dy, int Dw, int Dh, HBITMAP hbmSrc, int Sx, int Sy, int Sw, int Sh )
{
	HDC hdcDst = CreateCompatibleDC( NULL );

	hbmDst = (HBITMAP) SelectObject( hdcDst, hbmDst );

	SetStretchBltMode( hdcDst, COLORONCOLOR );

	BltBitmap( hdcDst, Dx, Dy, Dw, Dh, hbmSrc, Sx, Sy, Sw, Sh );

	hbmDst = (HBITMAP) SelectObject( hdcDst, hbmDst );

	DeleteDC( hdcDst );
}

//=============================================================================
// CopyBitmapPreserveAspectRatio
//=============================================================================

void CopyBitmapPreserveAspectRatio( HBITMAP hbmDst, int Dx, int Dy, int Dw, int Dh, HBITMAP hbmSrc, int Sx, int Sy, int Sw, int Sh )
{
	HDC hdcDst = CreateCompatibleDC( NULL );

	hbmDst = (HBITMAP) SelectObject( hdcDst, hbmDst );

	StretchBltPreserveAspectRatio( hdcDst, Dx, Dy, Dw, Dh, hbmSrc, Sx, Sy, Sw, Sh );

	hbmDst = (HBITMAP) SelectObject( hdcDst, hbmDst );

	DeleteDC( hdcDst );
}

//=============================================================================
// CopyDC
//=============================================================================

void CopyDC( HBITMAP hbmDst, int Dx, int Dy, int Dw, int Dh, HDC hdcSrc, int Sx, int Sy, int Sw, int Sh )
{
	HDC hdcDst = CreateCompatibleDC( hdcSrc );

	hbmDst = (HBITMAP) SelectObject( hdcDst, hbmDst );

	StretchBlt( hdcDst, Dx, Dy, Dw, Dh, hdcSrc, Sx, Sy, Sw, Sh, SRCCOPY );

	hbmDst = (HBITMAP) SelectObject( hdcDst, hbmDst );

	DeleteObject( hdcDst );
}

//=============================================================================
// CreateBitmapHeader
//=============================================================================

BITMAPINFO *CreateBitmapHeader( HBITMAP hbm )
{
	BITMAP bm = GetBitmapStruct( hbm );
	DWORD dwBPP = bm.bmBitsPixel;
	DWORD dwSize = sizeof(BITMAPINFOHEADER);

	if( dwBPP <= 8 )
		dwSize += sizeof(RGBQUAD) * (1 << bm.bmBitsPixel);

	BITMAPINFO *pbmi = (BITMAPINFO *) LocalAlloc( LPTR, dwSize );

	if( pbmi )
	{
		pbmi->bmiHeader.biSize				= sizeof(pbmi->bmiHeader);
		pbmi->bmiHeader.biWidth				= bm.bmWidth;
		pbmi->bmiHeader.biHeight			= bm.bmHeight;
		pbmi->bmiHeader.biPlanes			= bm.bmPlanes;
		pbmi->bmiHeader.biBitCount			= (WORD) dwBPP;
		pbmi->bmiHeader.biCompression		= BI_RGB;
		pbmi->bmiHeader.biSizeImage		= (pbmi->bmiHeader.biWidth * pbmi->bmiHeader.biBitCount / 8 + 3 & 0xfffffffc) * pbmi->bmiHeader.biHeight;
		pbmi->bmiHeader.biXPelsPerMeter	= 0;
		pbmi->bmiHeader.biYPelsPerMeter	= 0;
		pbmi->bmiHeader.biClrUsed			= 0;
		pbmi->bmiHeader.biClrImportant	= 0;

		if( pbmi->bmiHeader.biBitCount <= 8 )
			pbmi->bmiHeader.biClrUsed = 1 << pbmi->bmiHeader.biBitCount;
	}

	return pbmi;
}

//=============================================================================
// DeleteBitmapHeader
//=============================================================================

BITMAPINFO *DeleteBitmapHeader( BITMAPINFO *pbmi )
{
	if( pbmi )
		pbmi = (BITMAPINFO *) LocalFree( pbmi );

	return pbmi;
}

//=============================================================================
// GetBitmapHeaderSize
//=============================================================================

DWORD GetBitmapHeaderSize( BITMAPINFO *pbmi )
{
	return pbmi->bmiHeader.biSize + pbmi->bmiHeader.biClrUsed * sizeof(RGBQUAD);
}

//=============================================================================
// WriteDIB
//=============================================================================

BOOL WriteDIB( HANDLE hFile, HBITMAP hbm, HPALETTE hpal )
{
	BOOL bResult = FALSE;
	HDC hdc = GetDC( NULL );
	DWORD dwFilePos = GetFilePointer( hFile );
	BITMAPINFO *pbmi = CreateBitmapHeader( hbm );

	if( pbmi )
	{
		if( pbmi->bmiHeader.biClrUsed > 0 )
		{
			//GetSystemPaletteEntries( hdc, 0, pbmi->bmiHeader.biClrUsed, (PALETTEENTRY *) pbmi->bmiColors );

			GdiGetColorTable( hbm, pbmi->bmiColors, pbmi->bmiHeader.biClrUsed );

			if( hpal )
				GetPaletteEntries( hpal, 0, pbmi->bmiHeader.biClrUsed, (PALETTEENTRY *) pbmi->bmiColors );
		}

		DWORD dwWidthBytes = pbmi->bmiHeader.biSizeImage / pbmi->bmiHeader.biHeight;
		BYTE *pBits = (BYTE *) GlobalAlloc( GMEM_FIXED, dwWidthBytes );

		if( pBits )
		{
			BITMAPFILEHEADER bf;

			bf.bfType		= 0x4d42;
			bf.bfSize		= 0;
			bf.bfReserved1	= 0;
			bf.bfReserved2	= 0;
			bf.bfOffBits	= 0;

			if( FileWrite( hFile, &bf, sizeof(bf) ) )
			{
				if( FileWrite( hFile, pbmi, GetBitmapHeaderSize( pbmi ) ) )
				{
					bf.bfOffBits = GetFilePointer( hFile );

					for( int y = 0; y < pbmi->bmiHeader.biHeight; y++ )
					{
						GetDIBits( hdc, hbm, y, 1, pBits, pbmi, DIB_RGB_COLORS );

						if( !FileWrite( hFile, pBits, dwWidthBytes ) )
							break;
					}

					if( y == pbmi->bmiHeader.biHeight )
					{
						bf.bfSize = GetFilePointer( hFile );

						SetFilePointer( hFile, dwFilePos, 0, FILE_BEGIN );

						if( FileWrite( hFile, &bf, sizeof(bf) ) )
							bResult = TRUE;
					}
				}
			}
		}
		pbmi = DeleteBitmapHeader( pbmi );
	}

	ReleaseDC( NULL, hdc );

	return bResult;
}

//=============================================================================
// SaveDIB
//=============================================================================

BOOL SaveDIB( LPCTSTR lpszFileName, HBITMAP hbm, HPALETTE hpal )
{
	FileCreateDirectory( lpszFileName, TRUE );

	HANDLE hFile = CreateFile( lpszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	TCHAR szType[5];

	strcpy( szType, &lpszFileName[strlen(lpszFileName)-4] );

	szType[0] = tlower( szType[0] );
	szType[1] = tlower( szType[1] );
	szType[2] = tlower( szType[2] );
	szType[3] = tlower( szType[3] );

	BOOL bResult = FALSE;

	if( szType[0] == '.' &&
		szType[1] == 'b' &&
		szType[2] == 'm' &&
		szType[3] == 'p' )
	{
		bResult = WriteDIB( hFile, hbm, hpal );
	}

	if( szType[0] == '.' &&
		szType[1] == 't' &&
		szType[2] == 'g' &&
		szType[3] == 'a' )
	{
		bResult = WriteTGA( hFile, hbm, hpal );
	}

	CloseHandle( hFile );

	return bResult;
}

//=============================================================================
// LoadDIB
//=============================================================================

HBITMAP LoadDIB( LPCTSTR lpszFileName )
{
	TCHAR szType[5];

	strcpy( szType, &lpszFileName[strlen(lpszFileName)-4] );

	szType[0] = tlower(szType[0]);
	szType[1] = tlower(szType[1]);
	szType[2] = tlower(szType[2]);
	szType[3] = tlower(szType[3]);

	if( szType[0] == '.' &&
		szType[1] == 'b' &&
		szType[2] == 'm' &&
		szType[3] == 'p' )
	{
		return (HBITMAP) LoadImage( NULL, lpszFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION );
	}

	if( szType[0] == '.' &&
		szType[1] == 't' &&
		szType[2] == 'g' &&
		szType[3] == 'a' )
	{
		return (HBITMAP) LoadTGA( lpszFileName );
	}

	return NULL;
}

//=============================================================================
// WriteTGA
//=============================================================================

#pragma pack(push)
#pragma pack(1)

typedef struct
{
	BYTE tgaImageIDLength;
	BYTE tgaColorMapType;
	BYTE tgaImageType;
	WORD tgaColorMapBegin;
	WORD tgaColorMapLength;
	BYTE tgaColorMapBPP;
	WORD tgaOriginX;
	WORD tgaOriginY;
	WORD tgaImageWidth;
	WORD tgaImageHeight;
	BYTE tgaImageBPP;
	BYTE tgaDescriptor;
}
TGAINFOHEADER, *LPTGAINFOHEADER;

typedef struct
{
	DWORD tgaExtensionOffset;
	DWORD tgaDeveloperOffset;
	BYTE tgaSignature[18];
}
TGAFILEFOOTER, *LPTGAFILEFOOTER;

#pragma pack(pop)

typedef struct
{
	BYTE	tgaBlue;
	BYTE	tgaGreen;
	BYTE	tgaRed;
}
TGATRIPLE;

typedef struct
{
	BYTE	tgaBlue;
	BYTE	tgaGreen;
	BYTE	tgaRed;
	BYTE	tgaAlpha;
}
TGAQUAD;

HBITMAP ReadTGA( HANDLE hFile )
{
	DWORD dwFilePos = GetFilePointer( hFile );

	HBITMAP hbm = 0;
	TGAINFOHEADER header;
	TGAFILEFOOTER footer;

	if( FileRead( hFile, &header, sizeof(header) ) )
	{
		hbm = CreateDIB( header.tgaImageWidth, -header.tgaImageHeight, header.tgaImageBPP );

		if( hbm )
		{
			FileRead( hFile, &footer, sizeof(footer), GetFileSize( hFile, 0 ) - sizeof(footer) );

			if( 0 == memcmp( footer.tgaSignature, "TRUEVISION-XFILE", sizeof(footer.tgaSignature) ) )
			{
			}

			if( header.tgaImageType == 1 || header.tgaImageType == 2 )
			{
				RGBQUAD Palette[256];

				union 
				{
					RGBQUAD Palette32[256];
					RGBTRIPLE Palette24[256];
					WORD Palette16[256];
				};

				if( header.tgaColorMapType == 1 )
				{
					int ColorMapLength = header.tgaColorMapLength;

					if( ColorMapLength > 256 )
						ColorMapLength = 256;

					int BufferSize = ColorMapLength * ((header.tgaColorMapBPP+7) / 8);

					FileRead( hFile, Palette32, BufferSize, GetFilePointer( hFile ) );

					switch( header.tgaColorMapBPP )
					{
					case 32:
						GdiSetColorTable( hbm, Palette32, ColorMapLength );
						break;
					case 24:
						{
							for( int i = 0; i < ColorMapLength; i++ )
							{
								Palette[i].rgbRed   = Palette24[i].rgbtRed;
								Palette[i].rgbGreen = Palette24[i].rgbtGreen;
								Palette[i].rgbBlue  = Palette24[i].rgbtBlue;
								Palette[i].rgbReserved = 0;
							}

							GdiSetColorTable( hbm, Palette, ColorMapLength );
						}
						break;
					case 16:
					case 15:
						{
							WORD C;

							for( int i = 0; i < ColorMapLength; i++ )
							{
								C = Palette16[i];

								Palette[i].rgbRed   = (C&0x7c00) >>13;
								Palette[i].rgbGreen = (C&0x03e0) >> 5;
								Palette[i].rgbBlue  = (C&0x001f) << 3;
								Palette[i].rgbReserved = 0;
							}

							GdiSetColorTable( hbm, Palette, ColorMapLength );
						}
						break;
					}
				}

				SetFilePointer( hFile, header.tgaImageIDLength + (header.tgaColorMapBPP+7)/8 * header.tgaColorMapLength, 0, FILE_CURRENT );

				BYTE *pBitmap = GetBitmapData( hbm );
				int Stride = GetBitmapStride( hbm );
				int Length = header.tgaImageWidth * ((header.tgaImageBPP+7)/8);

				if( header.tgaDescriptor & 0x20 )
				{
					for( int y = 0; y < header.tgaImageHeight; y++ )
						FileRead( hFile, &pBitmap[(header.tgaImageHeight - y) * Stride - Stride], Length );
				}
				else
				{
					for( int y = 0; y < header.tgaImageHeight; y++ )
						FileRead( hFile, &pBitmap[y * Stride], Length );
				}
			}
		}
	}

	return hbm;
}

HBITMAP LoadTGA( LPCTSTR lpszFileName )
{
	HANDLE hFile = CreateFile( lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL );

	if( !hFile )
		return FALSE;

	HBITMAP hbmResult = ReadTGA( hFile );

	CloseHandle( hFile );

	return hbmResult;
}

BOOL WriteTGA( HANDLE hFile, HBITMAP hbm, HPALETTE hpal )
{
	DWORD dwFilePos = GetFilePointer( hFile );

	TGAINFOHEADER header;

	header.tgaImageIDLength = 0;
	header.tgaColorMapType = 0;
	header.tgaImageType = 2;
	header.tgaColorMapBegin = 0;
	header.tgaColorMapLength = 0;
	header.tgaColorMapBPP = 0;
	header.tgaOriginX = 0;
	header.tgaOriginY = 0;
	header.tgaImageWidth = GetBitmapWidth( hbm );
	header.tgaImageHeight = GetBitmapHeight( hbm );
	header.tgaImageBPP = GetBitmapBPP( hbm );
	header.tgaDescriptor = 0x24;	// bit field: 00 (reserved) 00 (origin lower-left) 0100 (8-bit alpha);

	if( header.tgaImageBPP <= 8 )
	{
		header.tgaColorMapType = 1;
		header.tgaImageType = 1;
		header.tgaColorMapBegin = 0;
		header.tgaColorMapLength = 1 << header.tgaImageBPP;
		header.tgaColorMapBPP = 32;
		header.tgaDescriptor = 0;
	}

	if( !FileWrite( hFile, &header, sizeof( header ) ) )
		return FALSE;

	BeginDIB( 0, hbm );

	BOOL bResult = FALSE;
	int x, y;

	if( header.tgaImageBPP == 8 )
	{
		COLOR32 Palette[256];
		GetPalette( 0, Palette, 256 );

		if( FileWrite( hFile, Palette, header.tgaColorMapLength * (header.tgaColorMapBPP/8) ) )
		{
			BYTE *scanline = new BYTE[header.tgaImageWidth];

			if( scanline )
			{
				SetPixelFormat( 0, PIXELFMT_INDEX8 );

				for( y = 0; y < header.tgaImageHeight; y++ )
				{
					for( x = 0; x < header.tgaImageWidth; x++ )
						scanline[x] = (BYTE) GetPixelU( 0, x, y );

					bResult = FileWrite( hFile, scanline, sizeof(BYTE) * header.tgaImageWidth );

					if( !bResult )
						break;
				}

				delete [] scanline;
			}
		}
	}
	else if( header.tgaImageBPP == 16 )
	{
		WORD *scanline = new WORD[header.tgaImageWidth];

		if( scanline )
		{
			COLORREF c;

			for( y = 0; y < header.tgaImageHeight; y++ )
			{
				for( x = 0; x < header.tgaImageWidth; x++ )
				{
					c = GetPixelU( 0, x, y );

					scanline[x] = (WORD)((c >> 9 & 0x7800)|(c >> 6 & 0x03e0)|(c >> 3 & 0x001f));
				}

				bResult = FileWrite( hFile, scanline, sizeof(WORD) * header.tgaImageWidth );

				if( !bResult )
					break;
			}
	
			delete [] scanline;
		}
	}
	else if( header.tgaImageBPP == 24 )
	{
		TGATRIPLE *scanline = new TGATRIPLE[header.tgaImageWidth];

		if( scanline )
		{
			COLORREF c;

			for( y = 0; y < header.tgaImageHeight; y++ )
			{
				for( x = 0; x < header.tgaImageWidth; x++ )
				{
					c = GetPixelU( 0, x, y );

					scanline[x].tgaRed   = (BYTE) (c >> 16);
					scanline[x].tgaGreen = (BYTE) (c >>  8);
					scanline[x].tgaBlue  = (BYTE) (c >>  0);
				}

				bResult = FileWrite( hFile, scanline, sizeof(TGATRIPLE) * header.tgaImageWidth );

				if( !bResult )
					break;
			}

			delete [] scanline;
		}
	}
	else if( header.tgaImageBPP == 32 )
	{
		TGAQUAD *scanline = new TGAQUAD[header.tgaImageWidth];

		if( scanline )
		{
			COLORREF c;

			for( y = 0; y < header.tgaImageHeight; y++ )
			{
				for( x = 0; x < header.tgaImageWidth; x++ )
				{
					c = GetPixelU( 0, x, y );

					scanline[x].tgaAlpha = (BYTE) (c >> 24);
					scanline[x].tgaRed   = (BYTE) (c >> 16);
					scanline[x].tgaGreen = (BYTE) (c >>  8);
					scanline[x].tgaBlue  = (BYTE) (c >>  0);
				}

				bResult = FileWrite( hFile, scanline, sizeof(TGAQUAD) * header.tgaImageWidth );

				if( !bResult )
					break;
			}

			delete [] scanline;
		}
	}

	FinishBitmap(0);

	return bResult;
}

void BlendPixel( int Channel, int x, int y, COLORREF c, BLENDMODE Blend )
{
	COLORREF a, b;

	switch( Blend )
	{
	case BLENDMODE_MODULATE:
		{
			b = GetPixelF(Channel, x, y);
			
			__asm
			{
				movd	mm0, b;
				paddusb	mm0, c;
				movd	a, mm0;
				emms;
			}

			SetPixelF(Channel, x, y, a);
		}
		break;
	case BLENDMODE_ADD:
		{
			b = GetPixelF(Channel, x, y);
			
			__asm
			{
				movd	mm0, b;
				paddusb	mm0, c;
				movd	a, mm0;
				emms;
			}

			SetPixelF(Channel, x, y, a);
		}
		break;
	case BLENDMODE_SUBTRACT:
		{
			b = GetPixelF(Channel, x, y);
			
			__asm
			{
				movd	mm0, b;
				psubusb	mm0, c;
				movd	a, mm0;
				emms;
			}

			SetPixelF(Channel, x, y, a);
		}
		break;
	}
}