//=============================================================================//
//  Copyright ©2001 Avalanche Software.  All Rights Reserved.                  //
//=============================================================================//
//      File: graphics2.cpp                                                    //
//    Author: Daniel Silver                                                    //
//   Created: 06/20/2001                                                       //
//=============================================================================//

#include "pch.h"
#include "graphics.h"

struct BITMAPLIST : public BITMAP2
{
	BITMAPLIST *pNext;
	BITMAPLIST *pPrev;
};

BITMAPLIST *pBitmapList[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
BITMAP2 GxBitmap[16];

SETPIXELFUNCTION SetPixelFn[16];
GETPIXELFUNCTION GetPixelFn[16];
SETSCANLINEFUNCTION SetScanlineFn[16];
GETSCANLINEFUNCTION GetScanlineFn[16];

#define ARGBFLIP(c) (((c) & 0xff00ff00)|((c)>>16&0xff)|((c)<<16&0xff0000))
#define XRGBFLIP(c) (((c) & 0x0000ff00)|((c)>>16&0xff)|((c)<<16&0xff0000))

//=============================================================================
// PIXELFMT_UNKNOWN
//=============================================================================

void FASTCALL SetPixel_Unknown( BITMAP2 &bm, int x, int y, COLORREF c )
{
	return;
}

COLOR32 FASTCALL GetPixel_Unknown( BITMAP2 &bm, int x, int y )
{
	return 0;
}

void SetScanline_Unknown( int ch, COLOR32* p, int x, int y, int c )
{
	for( int i = 0; i < c; i++ )
		GxSetPixel( ch, x+i, y, p[i] );

	return;
}

void GetScanline_Unknown( int ch, COLOR32* p, int x, int y, int c )
{
	for( int i = 0; i < c; i++ )
		p[i] = GxGetPixel( ch, x+i, y );

	return;
}

//=============================================================================
// PIXELFMT_INDEX32
//=============================================================================

static __inline void FASTCALL SetPixel_Index32( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < bm.Width && y < bm.Height )
	{
		((COLOR32 *) &bm.pBits[y * bm.Stride])[x] = c;
	}

	return;
}

static __inline COLOR32 FASTCALL GetPixel_Index32( BITMAP2 &bm, int x, int y )
{	
	if( (x|y) >= 0 && x < bm.Width && y < bm.Height )
	{
		return ((COLOR32 *) &bm.pBits[y * bm.Stride])[x];
	}

	return 0;
}

static __inline void FASTCALL SetPixel_Fast_Index32( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	((COLOR32*) &bm.pBits[x * bm.Stride])[x] = c;
}

static __inline COLOR32 FASTCALL GetPixel_Fast_Index32( BITMAP2 &bm, int x, int y )
{
	return ((COLOR32*) &bm.pBits[y * bm.Stride])[x];
}

//=============================================================================
// PIXELFMT_INDEX24
//=============================================================================

static __inline void FASTCALL SetPixel_Index24( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < bm.Width && y < bm.Height )
	{
		RGBTRIPLE *p = (RGBTRIPLE *) &bm.pBits[y * bm.Stride];

		p[x].rgbtRed   = (BYTE) (c >>16);
		p[x].rgbtGreen = (BYTE) (c >> 8);
		p[x].rgbtBlue  = (BYTE) (c >> 0);
	}

	return;
}

static __inline COLOR32 FASTCALL GetPixel_Index24( BITMAP2 &bm, int x, int y )
{
	if( (x|y) >= 0 && x < bm.Width && y < bm.Height )
	{
		RGBTRIPLE *p = (RGBTRIPLE *) &bm.pBits[y * bm.Stride];

		int R, G, B;

		R = (int) p[x].rgbtRed   << 16;
		G = (int) p[x].rgbtGreen << 8;
		B = (int) p[x].rgbtBlue  << 0;

		return (R|G|B);
	}

	return 0;
}

static __inline void SetScanline_Index24( BITMAP2 &bm, COLOR32* p, int x, int y, int c )
{
	RGBTRIPLE* scanline = &((RGBTRIPLE*) &bm.pBits[y * bm.Stride])[x];
	COLOR32 color;

	for( int i = 0; i < c; i++ )
	{
		color = p[i];

		scanline[i].rgbtRed   = (BYTE) (color >> 16);
		scanline[i].rgbtGreen = (BYTE) (color >> 8 );
		scanline[i].rgbtBlue  = (BYTE) (color >> 0 );
	}

	return;
}

static __inline void GetScanline_Index24( BITMAP2 &bm, COLOR32* p, int x, int y, int c )
{
	RGBTRIPLE* scanline = &((RGBTRIPLE*) &bm.pBits[y * bm.Stride])[x];
	int R, G, B;

	for( int i = 0; i < c; i++ )
	{
		R = (int) scanline[i].rgbtRed   << 16;
		G = (int) scanline[i].rgbtGreen << 8;
		B = (int) scanline[i].rgbtBlue  << 0;

		p[i] = (R|G|B);
	}

	return;
}

//=============================================================================
// PIXELFMT_INDEX16
//=============================================================================

static __inline void FASTCALL SetPixel_Index16( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < bm.Width && y < bm.Height )
	{
		((WORD *) &bm.pBits[y * bm.Stride])[x] = (WORD) c;
	}

	return;
}

static __inline COLOR32 FASTCALL GetPixel_Index16( BITMAP2 &bm, int x, int y )
{
	if( (x|y) >= 0 && x < bm.Width && y < bm.Height )
	{
		return (COLOR32) ((WORD *) &bm.pBits[y * bm.Stride])[x];
	}

	return 0;
}

//=============================================================================
// PIXELFMT_INDEX8
//=============================================================================

static __inline void FASTCALL SetPixel_Index8( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < bm.Width && y < bm.Height )
	{
		((BYTE *) &bm.pBits[y * bm.Stride])[x] = (BYTE) c;
	}

	return;
}

COLOR32 FASTCALL GetPixel_Index8( BITMAP2 &bm, int x, int y )
{
	if( (x|y) >= 0 && x < bm.Width && y < bm.Height )
	{
		return (COLOR32) ((BYTE *) bm.pBits)[x + y * bm.Stride];
	}

	return 0;
}

//=============================================================================
// PIXELFMT_INDEX4
//=============================================================================

void FASTCALL SetPixel_Index4( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < bm.Width && y < bm.Height )
	{
		BYTE *p = (BYTE *) &bm.pBits[y * bm.Stride];

		if( x&1 )
			p[x>>1] = p[x>>1] & 0xf0 | (BYTE) ((c&0xf)<<4);
		else
			p[x>>1] = p[x>>1] & 0x0f | (BYTE) ((c&0xf)<<0);
	}

	return;
}

COLOR32 FASTCALL GetPixel_Index4( BITMAP2 &bm, int x, int y )
{
	if( (x|y) >= 0 && x < bm.Width && y < bm.Height )
	{
		BYTE *p = (BYTE *) &bm.pBits[y * bm.Stride];

		if( x&1 )
			return (COLOR32) p[x>>1] >> 4 & 0x0f;
		else
			return (COLOR32) p[x>>1] >> 0 & 0x0f;
	}

	return 0;
}

//=============================================================================
// PIXELFMT_A8R8G8B8
//=============================================================================

void FASTCALL SetPixel_A8R8G8B8( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	SetPixel_Index32( bm, x, y, c );
}

COLOR32 FASTCALL GetPixel_A8R8G8B8( BITMAP2 &bm, int x, int y )
{
	return GetPixel_Index32( bm, x, y );
}

void SetScanline_A8R8G8B8( int ch, COLOR32* p, int x, int y, int c )
{
	BITMAP2 &bm = GxBitmap[ch];
	COLOR32* scanline = &((COLOR32*) &bm.pBits[y * bm.Stride])[x];

	for( int i = 0; i < c; i++ )
		scanline[i] = p[i];

	return;
}

void GetScanline_A8R8G8B8( int ch, COLOR32* p, int x, int y, int c )
{
	BITMAP2 &bm = GxBitmap[ch];
	COLOR32* scanline = &((COLOR32*) &bm.pBits[y * bm.Stride])[x];

	for( int i = 0; i < c; i++ )
		p[i] = scanline[i];

	return;
}

//=============================================================================
// PIXELFMT_A8B8G8R8
//=============================================================================

void FASTCALL SetPixel_A8B8G8R8( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	SetPixel_Index32( bm, x, y, ARGBFLIP(c) );
}

COLOR32 FASTCALL GetPixel_A8B8G8R8( BITMAP2 &bm, int x, int y )
{
	COLOR32 c = GetPixel_Index32( bm, x, y );

	return ARGBFLIP(c);
}

void SetScanline_A8B8G8R8( int ch, COLOR32* p, int x, int y, int c )
{
	BITMAP2 &bm = GxBitmap[ch];
	COLOR32* scanline = &((COLOR32*) &bm.pBits[y * bm.Stride])[x];
	COLOR32 color;

	for( int i = 0; i < c; i++ )
	{
		color = p[i];

		scanline[i] = ARGBFLIP(color);
	}

	return;
}

void GetScanline_A8B8G8R8( int ch, COLOR32* p, int x, int y, int c )
{
	BITMAP2 &bm = GxBitmap[ch];
	COLOR32* scanline = &((COLOR32*) &bm.pBits[y * bm.Stride])[x];
	COLOR32 color;

	for( int i = 0; i < c; i++ )
	{
		color = scanline[i];

		p[i] = ARGBFLIP(color);
	}

	return;
}

//=============================================================================
// PIXELFMT_X8R8G8B8
//=============================================================================

void FASTCALL SetPixel_X8R8G8B8( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	SetPixel_Index32( bm, x, y, c & 0x00ffffff );
}

COLOR32 FASTCALL GetPixel_X8R8G8B8( BITMAP2 &bm, int x, int y )
{
	return 0xff000000 | GetPixel_Index32( bm, x, y );
}

//=============================================================================
// PIXELFMT_X8B8G8R8
//=============================================================================

void FASTCALL SetPixel_X8B8G8R8( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	SetPixel_Index32( bm, x, y, XRGBFLIP(c) );
}

COLOR32 FASTCALL GetPixel_X8B8G8R8( BITMAP2 &bm, int x, int y )
{
	COLOR32 c = GetPixel_Index32( bm, x, y );

	return XRGBFLIP(c);
}

//=============================================================================
// PIXELFMT_R8G8B8
//=============================================================================

void FASTCALL SetPixel_R8G8B8( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	SetPixel_Index24( bm, x, y, c );
}

COLOR32 FASTCALL GetPixel_R8G8B8( BITMAP2 &bm, int x, int y )
{
	return 0xff000000 | GetPixel_Index24( bm, x, y );
}

//=============================================================================
// PIXELFMT_B8G8R8
//=============================================================================

void FASTCALL SetPixel_B8G8R8( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	SetPixel_Index24( bm, x, y, XRGBFLIP(c) );
}

COLOR32 FASTCALL GetPixel_B8G8R8( BITMAP2 &bm, int x, int y )
{
	COLOR32 c = GetPixel_Index24( bm, x, y );

	return 0xff000000 | XRGBFLIP(c);
}

//=============================================================================
// PIXELFMT_A4R4G4B4
//=============================================================================

void FASTCALL SetPixel_A4R4G4B4( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	c = (c>>16 & 0xf000)|
		 (c>>12 & 0x0f00)|
		 (c>> 8 & 0x00f0)|
		 (c>> 4 & 0x000f);

	SetPixel_Index16( bm, x, y, c );
}

COLOR32 FASTCALL GetPixel_A4R4G4B4( BITMAP2 &bm, int x, int y )
{
	COLOR32 c = GetPixel_Index16( bm, x, y );

	return ((c&0xf000)<<16)|
			 ((c&0x0f00)<<12)|
			 ((c&0x00f0)<< 8)|
			 ((c&0x000f)<< 4);
}


//=============================================================================
// PIXELFMT_A4B4G4R4
//=============================================================================

void FASTCALL SetPixel_A4B4G4R4( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	c = (c>>16 & 0xf000)|
		 (c>>20 & 0x000f)|
		 (c>> 8 & 0x00f0)|
		 (c<< 4 & 0x0f00);

	SetPixel_Index16( bm, x, y, c );
}

COLOR32 FASTCALL GetPixel_A4B4G4R4( BITMAP2 &bm, int x, int y )
{
	COLOR32 c = GetPixel_Index16( bm, x, y );

	return ((c&0xf000)<<16)|
			 ((c&0x000f)<<20)|
			 ((c&0x00f0)<< 8)|
			 ((c&0x0f00)>> 4);
}

//=============================================================================
// PIXELFMT_X4R4G4B4
//=============================================================================

void FASTCALL SetPixel_X4R4G4B4( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	c = (c>>12 & 0x0f00)|
		 (c>> 8 & 0x00f0)|
		 (c>> 4 & 0x000f);

	SetPixel_Index16( bm, x, y, c );
}

COLOR32 FASTCALL GetPixel_X4R4G4B4( BITMAP2 &bm, int x, int y )
{
	COLOR32 c = GetPixel_Index16( bm, x, y );

	return ((c&0x0f00)<<12)|
			 ((c&0x00f0)<< 8)|
			 ((c&0x000f)<< 4)|
			 (0xff000000);
}

//=============================================================================
// PIXELFMT_X4B4G4R4
//=============================================================================

void FASTCALL SetPixel_X4B4G4R4( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	c = (c>>20 & 0x000f)|
		 (c>> 8 & 0x00f0)|
		 (c<< 4 & 0x0f00);

	SetPixel_Index16( bm, x, y, c );
}

COLOR32 FASTCALL GetPixel_X4B4G4R4( BITMAP2 &bm, int x, int y )
{
	COLOR32 c = GetPixel_Index16( bm, x, y );

	return ((c&0x000f)<<20)|
			 ((c&0x00f0)<< 8)|
			 ((c&0x0f00)>> 4)|
			 (0xff000000);
}

//=============================================================================
// PIXELFMT_A1R5G5B5
//=============================================================================

void FASTCALL SetPixel_A1R5G5B5( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	c = (c>>16 & 0x8000)|
		 (c>> 9 & 0x7c00)|
		 (c>> 6 & 0x03e0)|
		 (c>> 3 & 0x001f);

	SetPixel_Index16( bm, x, y, c );
}

COLOR32 FASTCALL GetPixel_A1R5G5B5( BITMAP2 &bm, int x, int y )
{
	COLOR32 c = GetPixel_Index16( bm, x, y );

	return ((c&0x8000)<<16)|
			 ((c&0x7c00)<< 9)|
			 ((c&0x03e0)<< 6)|
			 ((c&0x001f)<< 3);
}

//=============================================================================
// PIXELFMT_A1B5G5R5
//=============================================================================

void FASTCALL SetPixel_A1B5G5R5( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	c = (c>>16 & 0x8000)|
		 (c>>19 & 0x001f)|
		 (c>> 6 & 0x03e0)|
		 (c<< 7 & 0x7c00);

	SetPixel_Index16( bm, x, y, c );
}

COLOR32 FASTCALL GetPixel_A1B5G5R5( BITMAP2 &bm, int x, int y )
{
	COLOR32 c = GetPixel_Index16( bm, x, y );

	return ((c&0x8000)<<16)|
			 ((c&0x001f)<<19)|
			 ((c&0x0e30)<< 6)|
			 ((c&0x7c00)>> 7);
}

//=============================================================================
// PIXELFMT_X1R5G5B5
//=============================================================================

void FASTCALL SetPixel_X1R5G5B5( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	c = (c>> 9 & 0x7c00)|
		 (c>> 6 & 0x03e0)|
		 (c>> 3 & 0x001f);

	SetPixel_Index16( bm, x, y, c );
}

COLOR32 FASTCALL GetPixel_X1R5G5B5( BITMAP2 &bm, int x, int y )
{
	COLOR32 c = GetPixel_Index16( bm, x, y );

	return ((c&0x7c00)<< 9)|
		    ((c&0x03e0)<< 6)|
			 ((c&0x001f)<< 3)|
			 (0xff000000);
}

//=============================================================================
// PIXELFMT_X1B5G5R5
//=============================================================================

void FASTCALL SetPixel_X1B5G5R5( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	c = (c>>19 & 0x001f)|
		 (c>> 6 & 0x03e0)|
		 (c<< 7 & 0x7c00);

	SetPixel_Index16( bm, x, y, c );
}

COLOR32 FASTCALL GetPixel_X1B5G5R5( BITMAP2 &bm, int x, int y )
{
	COLOR32 c = GetPixel_Index16( bm, x, y );

	return ((c&0x001f)<<19)|
			 ((c&0x0e30)<< 6)|
			 ((c&0x7c00)>> 7)|
			 (0xff000000);
}

//=============================================================================
// PIXELFMT_R5G6B5
//=============================================================================

void FASTCALL SetPixel_R5G6B5( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	c = (c>>8 & 0xf800)|
		 (c>>5 & 0x07e0)|
		 (c>>3 & 0x001f);

	SetPixel_Index16( bm, x, y, c );
}

COLOR32 FASTCALL GetPixel_R5G6B5( BITMAP2 &bm, int x, int y )
{
	COLOR32 c = GetPixel_Index16( bm, x, y );

	return ((c&0xf800)<< 8)|
			 ((c&0x07e0)<< 5)|
			 ((c&0x001f)<< 3)|
			 (0xff000000);
}

//=============================================================================
// PIXELFMT_B5G6R5
//=============================================================================

void FASTCALL SetPixel_B5G6R5( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	c = (c>>19 & 0x001f)|
		 (c>> 5 & 0x07e0)|
		 (c<< 8 & 0xf800);

	SetPixel_Index16( bm, x, y, c );
}

COLOR32 FASTCALL GetPixel_B5G6R5( BITMAP2 &bm, int x, int y )
{
	COLOR32 c = GetPixel_Index16( bm, x, y );

	return ((c&0x001f)<<19)|
			 ((c&0x07e0)<< 5)|
			 ((c&0xf800)>> 8)|
			 (0xff000000);
}

//=============================================================================
// PIXELFMT_PALETTE8
//=============================================================================

void FASTCALL SetPixel_Palette8( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	if( (x|y) >= 0 && x < bm.Width && y < bm.Height )
	{
		BYTE *p = (BYTE *) &bm.pBits[y * bm.Stride];

		p[x] = (BYTE) c;
	}

	return;
}

COLOR32 FASTCALL GetPixel_Palette8( BITMAP2 &bm, int x, int y )
{
	return bm.Palette[GetPixel_Index8( bm, x, y )];
}

//=============================================================================
// PIXELFMT_PALETTE4
//=============================================================================

void FASTCALL SetPixel_Palette4( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	SetPixel_Index4( bm, x, y, c );
}

COLOR32 FASTCALL GetPixel_Palette4( BITMAP2 &bm, int x, int y )
{
	return bm.Palette[GetPixel_Index4( bm, x, y )];
}

//=============================================================================
// PIXELFMT_MONOCHROME
//=============================================================================

void FASTCALL SetPixel_Monochrome( BITMAP2 &bm, int x, int y, COLOR32 c )
{
	static const int  on[8] = { 1, 2, 4, 8, 16, 32, 64, 128};
	static const int off[8] = {~1,~2,~4,~8,~16,~32,~64,~128};

	if( (x|y) >= 0 && x < bm.Width && y < bm.Height )
	{
		BYTE *p = (BYTE *) &bm.pBits[y * bm.Stride];

		int n = x >> 3;

		if( c )
			p[n] = p[n] | on[x&7];
		else
			p[n] = p[n] & off[x&7];
	}

	return;
}

COLOR32 FASTCALL GetPixel_Monochrome( BITMAP2 &bm, int x, int y )
{
	static const int bit[8]  = {128, 64, 32, 16, 8, 4, 2, 1};

	if( (x|y) >= 0 && x < bm.Width && y < bm.Height )
	{
		BYTE *p = (BYTE *) &bm.pBits[y * bm.Stride];

		if( p[x>>3] & bit[x&7] )
			return bm.Palette[1];
	}

	return bm.Palette[0];
}

//=============================================================================
// BeginBitmap
//=============================================================================

void BeginBitmap( int ch, int Width, int Height, PIXELFORMAT Format, int Stride, void *pBits )
{
	static BOOL Initialized = FALSE;

	if( !Initialized )
	{
		for( int i = 0; i < 16; i++ )
		{
			GxBitmap[i].Width = 0;
			GxBitmap[i].Height = 0;
			GxBitmap[i].Format = PIXELFMT_UNKNOWN;
			GxBitmap[i].Stride = 0;
			GxBitmap[i].pBits = 0;
		}

		Initialized = TRUE;
	}

	if( pBits == 0 )
		Format = PIXELFMT_UNKNOWN;

	BITMAPLIST *pBitmap = new BITMAPLIST;

	if( pBitmap )
	{
		*(BITMAP2*) pBitmap = GxBitmap[ch];

		pBitmap->pNext = pBitmapList[ch];

		if( pBitmapList[ch] )
			pBitmapList[ch]->pPrev = pBitmap;

		pBitmapList[ch] = pBitmap;

		GxBitmap[ch].Width = Width;
		GxBitmap[ch].Height = Height;
		GxBitmap[ch].Format = Format;
		GxBitmap[ch].Stride = Stride;
		GxBitmap[ch].pBits = (BYTE *) pBits;

		SetPixelFormat( ch, Format );
	}

	return;
}

//=============================================================================
// FinishBitmap
//=============================================================================

void FinishBitmap( int ch )
{
	if( pBitmapList[ch] )
	{
		BITMAPLIST *pNextBitmap = pBitmapList[ch]->pNext;

		if( pNextBitmap )
			pNextBitmap->pPrev = 0;

		GxBitmap[ch] = *pBitmapList[ch];

		delete pBitmapList[ch];

		pBitmapList[ch] = pNextBitmap;
	}

	return;
}

//=============================================================================
// SetPixelFormat
//=============================================================================

void SetPixelFormat( int channel, PIXELFORMAT Format )
{
	GxBitmap[channel].Format = Format;

	SetPixelFn[channel] = SetPixel_Unknown;
	GetPixelFn[channel] = GetPixel_Unknown;
	SetScanlineFn[channel] = SetScanline_Unknown;
	GetScanlineFn[channel] = GetScanline_Unknown;

	switch( Format )
	{
	case PIXELFMT_A8R8G8B8:
		SetPixelFn[channel] = SetPixel_A8R8G8B8;
		GetPixelFn[channel] = GetPixel_A8R8G8B8;
		SetScanlineFn[channel] = SetScanline_A8R8G8B8;
		GetScanlineFn[channel] = GetScanline_A8R8G8B8;
		break;
	case PIXELFMT_A8B8G8R8:
		SetPixelFn[channel] = SetPixel_A8B8G8R8;
		GetPixelFn[channel] = GetPixel_A8B8G8R8;
		SetScanlineFn[channel] = SetScanline_A8B8G8R8;
		GetScanlineFn[channel] = GetScanline_A8B8G8R8;
		break;
	case PIXELFMT_X8R8G8B8:
		SetPixelFn[channel] = SetPixel_X8R8G8B8;
		GetPixelFn[channel] = GetPixel_X8R8G8B8;
		break;
	case PIXELFMT_X8B8G8R8:
		SetPixelFn[channel] = SetPixel_X8B8G8R8;
		GetPixelFn[channel] = GetPixel_X8B8G8R8;
		break;
	case PIXELFMT_R8G8B8:
		SetPixelFn[channel] = SetPixel_R8G8B8;
		GetPixelFn[channel] = GetPixel_R8G8B8;
		break;
	case PIXELFMT_B8G8R8:
		SetPixelFn[channel] = SetPixel_B8G8R8;
		GetPixelFn[channel] = GetPixel_B8G8R8;
		break;
	case PIXELFMT_A4R4G4B4:
		SetPixelFn[channel] = SetPixel_A4R4G4B4;
		GetPixelFn[channel] = GetPixel_A4R4G4B4;
		break;
	case PIXELFMT_A4B4G4R4:
		SetPixelFn[channel] = SetPixel_A4B4G4R4;
		GetPixelFn[channel] = GetPixel_A4B4G4R4;
		break;
	case PIXELFMT_X4R4G4B4:
		SetPixelFn[channel] = SetPixel_X4R4G4B4;
		GetPixelFn[channel] = GetPixel_X4R4G4B4;
		break;
	case PIXELFMT_X4B4G4R4:
		SetPixelFn[channel] = SetPixel_X4B4G4R4;
		GetPixelFn[channel] = GetPixel_X4B4G4R4;
		break;
	case PIXELFMT_A1R5G5B5:
		SetPixelFn[channel] = SetPixel_A1R5G5B5;
		GetPixelFn[channel] = GetPixel_A1R5G5B5;
		break;
	case PIXELFMT_A1B5G5R5:
		SetPixelFn[channel] = SetPixel_A1B5G5R5;
		GetPixelFn[channel] = GetPixel_A1B5G5R5;
		break;
	case PIXELFMT_X1R5G5B5:
		SetPixelFn[channel] = SetPixel_X1R5G5B5;
		GetPixelFn[channel] = GetPixel_X1R5G5B5;
		break;
	case PIXELFMT_X1B5G5R5:
		SetPixelFn[channel] = SetPixel_X1B5G5R5;
		GetPixelFn[channel] = GetPixel_X1B5G5R5;
		break;
	case PIXELFMT_R5G6B5:
		SetPixelFn[channel] = SetPixel_R5G6B5;
		GetPixelFn[channel] = GetPixel_R5G6B5;
		break;
	case PIXELFMT_B5G6R5:
		SetPixelFn[channel] = SetPixel_B5G6R5;
		GetPixelFn[channel] = GetPixel_B5G6R5;
		break;
	case PIXELFMT_PALETTE8:
		SetPixelFn[channel] = SetPixel_Palette8;
		GetPixelFn[channel] = GetPixel_Palette8;
		break;
	case PIXELFMT_PALETTE4:
		SetPixelFn[channel] = SetPixel_Palette4;
		GetPixelFn[channel] = GetPixel_Palette4;
		break;
	case PIXELFMT_INDEX32:
		SetPixelFn[channel] = SetPixel_Index32;
		GetPixelFn[channel] = GetPixel_Index32;
		break;
	case PIXELFMT_INDEX24:
		SetPixelFn[channel] = SetPixel_Index24;
		GetPixelFn[channel] = GetPixel_Index24;
		break;
	case PIXELFMT_INDEX16:
		SetPixelFn[channel] = SetPixel_Index16;
		GetPixelFn[channel] = GetPixel_Index16;
		break;
	case PIXELFMT_INDEX8:
		SetPixelFn[channel] = SetPixel_Index8;
		GetPixelFn[channel] = GetPixel_Index8;
		break;
	case PIXELFMT_INDEX4:
		SetPixelFn[channel] = SetPixel_Index4;
		GetPixelFn[channel] = GetPixel_Index4;
		break;
	case PIXELFMT_MONOCHROME:
		SetPixelFn[channel] = SetPixel_Monochrome;
		GetPixelFn[channel] = GetPixel_Monochrome;
		break;
	}

	return;
}

//=============================================================================
// SetPalette()
//=============================================================================

void SetPalette( int channel, COLOR32 *pPalette, int PaletteSize )
{
	int i;

	if( !pPalette )
		PaletteSize = 0;

	for( i = 0; i < PaletteSize; i++ )
		GxBitmap[channel].Palette[i] = pPalette[i];

	for( i = PaletteSize; i < 256; i++ )
		GxBitmap[channel].Palette[i] = 0;

	return;
}

//=============================================================================
// GetPalette()
//=============================================================================

void GetPalette( int channel, COLOR32 *pPalette, int PaletteSize )
{
	if( pPalette )
	{
		if( PaletteSize > 256 )
			PaletteSize = 256;

		for( int i = 0; i < PaletteSize; i++ )
			pPalette[i] = GxBitmap[channel].Palette[i];
	}

	return;
}

COLORREF GetSubPixel( int nChannel, int x, int y )
{
	const int h = (x & 0xff00) / 255;
	const int i = (y & 0xff00) / 255;

	x = x >> 16;
	y = y >> 16;

	const int a = (0x100 - h) * (0x100 - i) >> 8;
	const int b = (0x000 + h) * (0x100 - i) >> 8;
	const int c = (0x000 + h) * (0x000 + i) >> 8;
	const int d = (0x100 - h) * (0x000 + i) >> 8;

	const COLORREF cr1 = GxGetPixel(nChannel, x + 0, y + 0 );
	const COLORREF cr2 = GxGetPixel(nChannel, x + 1, y + 0 );
	const COLORREF cr3 = GxGetPixel(nChannel, x + 1, y + 1 );
	const COLORREF cr4 = GxGetPixel(nChannel, x + 0, y + 1 );

	const unsigned int A = 0xff000000 & (((cr1 >> 8 & 0xff0000) * a) + ((cr2 >> 8 & 0xff0000) * b) + ((cr3 >> 8 & 0xff0000) * c) + ((cr4 >> 8 & 0xff0000) * d));
	const unsigned int R = 0xff000000 & (((cr1 >> 0 & 0xff0000) * a) + ((cr2 >> 0 & 0xff0000) * b) + ((cr3 >> 0 & 0xff0000) * c) + ((cr4 >> 0 & 0xff0000) * d));
	const unsigned int G = 0x00ff0000 & (((cr1 >> 0 & 0x00ff00) * a) + ((cr2 >> 0 & 0x00ff00) * b) + ((cr3 >> 0 & 0x00ff00) * c) + ((cr4 >> 0 & 0x00ff00) * d));
	const unsigned int B = 0x0000ff00 & (((cr1 >> 0 & 0x0000ff) * a) + ((cr2 >> 0 & 0x0000ff) * b) + ((cr3 >> 0 & 0x0000ff) * c) + ((cr4 >> 0 & 0x0000ff) * d));

	return A|(R|G|B)>>8;
}

void SetSubPixel( int nChannel, int x, int y, COLORREF c )
{
	const int u = (x - 32768 & 0xff00) / 255;
	const int v = (y - 32768 & 0xff00) / 255;

	x = (x - 32768) >> 16;
	y = (y - 32768) >> 16;

	const int A = (0x100 - u) * (0x100 - v) >> 8;
	const int B = (0x000 + u) * (0x100 - v) >> 8;
	const int C = (0x000 + u) * (0x000 + v) >> 8;
	const int D = (0x100 - u) * (0x000 + v) >> 8;

	GxSetPixel(nChannel, x + 0, y + 0, ColorBlend( GxGetPixel(nChannel, x + 0, y + 0 ), c, A) );
	GxSetPixel(nChannel, x + 1, y + 0, ColorBlend( GxGetPixel(nChannel, x + 1, y + 0 ), c, B) );
	GxSetPixel(nChannel, x + 1, y + 1, ColorBlend( GxGetPixel(nChannel, x + 1, y + 1 ), c, C) );
	GxSetPixel(nChannel, x + 0, y + 1, ColorBlend( GxGetPixel(nChannel, x + 0, y + 1 ), c, D) );
}