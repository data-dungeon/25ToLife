#include "pch.h"
#include "gdi.h"

void GdiLine( HDC hdcDst, int x1, int y1, int x2, int y2, COLORREF c )
{
	HPEN hpen = (HPEN) SelectObject( hdcDst, CreatePen( PS_SOLID, 1, AssBackwardsColor(c) ) );

	MoveToEx( hdcDst, x1, y1, 0 );
	LineTo( hdcDst, x2, y2 );

	DeleteObject( SelectObject( hdcDst, hpen ) );
}

void GdiFrame( HDC hdc, int x, int y, int Width, int Height, COLORREF c )
{
	HPEN hpen = (HPEN) SelectObject( hdc, CreatePen( PS_SOLID, 1, AssBackwardsColor(c) ) );

	int x1 = x;
	int y1 = y;
	int x2 = x + Width - 1;
	int y2 = y + Height - 1;

	MoveToEx( hdc, x1, y1, 0 );
	LineTo( hdc, x2, y1 );
	LineTo( hdc, x2, y2 );
	LineTo( hdc, x1, y2 );
	LineTo( hdc, x1, y1 );

	DeleteObject( SelectObject( hdc, hpen ) );
}

void GdiFrame( HDC hdc, RECT &rc, COLORREF c )
{
	GdiFrame( hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, c );
}

void GdiSetRect( LPRECT pRect, int x, int y, int Width, int Height )
{
	if( pRect )
	{
		pRect->left = x;
		pRect->right = x + Width;
		pRect->top = y;
		pRect->bottom = y + Height;
	}

	return;
}

RECT GdiMakeRect( int x, int y, int Width, int Height )
{
	RECT rc;

	GdiSetRect( &rc, x, y, Width, Height );

	return rc;
}

BOOL GdiPointInRect( int x, int y, RECT &rc )
{
	if( x >= rc.left && x < rc.right && y >= rc.top && y < rc.bottom )
		return TRUE;

	return FALSE;
}

void GdiFillRect( HDC hdc, int x, int y, int Width, int Height, COLORREF Color )
{
	RECT rc = GdiMakeRect( x, y, Width, Height );
	HBRUSH hBrush = CreateSolidBrush( AssBackwardsColor(Color) );

	FillRect( hdc, &rc, hBrush );

	DeleteObject( hBrush );
}

void GdiFillRect( HDC hdc, RECT &rc, COLORREF Color )
{
	GdiFillRect( hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, Color );
}

COLORREF AssBackwardsColor( COLORREF c )
{
	return (c&0xff0000)>>16|
		    (c&0x00ff00)>> 0|
			 (c&0x0000ff)<<16;
}

HFONT GdiCreateSimilarFontEx( HFONT hFont, int Height, int Width, int Escapement, int Orientation, int Weight, DWORD Italic, DWORD Underline, DWORD Strikeout, DWORD CharacterSet, DWORD OutputPrecision, DWORD ClipPrecesion, DWORD Quality, DWORD PitchAndFamily )
{
	HDC hdc = CreateCompatibleDC(0);
	HFONT hOldFont = (HFONT) SelectObject( hdc, hFont );

	hOldFont = (HFONT) SelectObject( hdc, hFont );

	TEXTMETRIC tm;
	GetTextMetrics( hdc, &tm );

	TCHAR szFaceName[256];
	GetTextFace( hdc, sizeof(szFaceName)/sizeof(szFaceName[0]), szFaceName );

	if( Height == 0 )
		Height = tm.tmHeight;

	if( Weight == FW_DONTCARE )
		Weight = tm.tmWeight;

	hOldFont = (HFONT) SelectObject( hdc, hFont );

	DeleteDC( hdc );

	return CreateFont( Height, Width, Escapement, Orientation, Weight, Italic, Underline, Strikeout, CharacterSet, OutputPrecision, ClipPrecesion, Quality, PitchAndFamily, szFaceName );
}

HFONT GdiCreateSimilarFont( HFONT hFont, int Height, int Weight, DWORD Itatlic, DWORD Underline )
{
	TEXTMETRIC tm = GdiTextMetrics( hFont );

	DWORD CharacterSet = (DWORD) tm.tmCharSet;

	return GdiCreateSimilarFontEx( hFont, Height, 0, 0, 0, Weight, Itatlic, Underline, 0, CharacterSet, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_DONTCARE );
}

HFONT GdiCreateFont( int Height, int Weight, DWORD Italic, DWORD Underline, LPCTSTR pFaceName )
{
	return CreateFont( Height, 0, 0, 0, Weight, Italic, Underline, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_DONTCARE, pFaceName );
}

TEXTMETRIC GdiTextMetrics( HFONT hFont )
{
	TEXTMETRIC tm;

	HDC hdc = CreateCompatibleDC(0);

	hFont = (HFONT) SelectObject( hdc, hFont );

	GetTextMetrics( hdc, &tm );

	hFont = (HFONT) SelectObject( hdc, hFont );

	DeleteDC( hdc );

	return tm;
}

int GdiGetFontHeight( HFONT hFont )
{
	TEXTMETRIC tm = GdiTextMetrics( hFont );

	return tm.tmHeight;
}

void GdiCenterText( HDC hdc, int x1, int y1, int x2, int y2, LPCTSTR pText )
{
	GdiTextInRect( hdc, x1, y1, x2, y2, pText, 0.5f, 0.5f );
}

void GdiTextInRect( HDC hdc, int x1, int y1, int x2, int y2, LPCTSTR pText, float tx, float ty )
{
	if( pText )
	{
		SIZE TextSize;
		GetTextExtentPoint32( hdc, pText, (int) strlen(pText), &TextSize );

		float cx = (x2 - x1) * tx;
		float cy = (y2 - y1) * ty;

		float rx = TextSize.cx * tx;
		float ry = TextSize.cy * ty;

		int dx = x1 + (int) (cx-rx);
		int dy = y1 + (int) (cy-ry);

		TextOut( hdc, dx, dy, pText, (int) strlen(pText) );
	}

	return;
}

RECT GdiMakeTextRect( HDC hdc, int x, int y, LPCTSTR pText )
{
	if( pText )
	{
		SIZE TextSize;
		GetTextExtentPoint32( hdc, pText, (int) strlen(pText), &TextSize );

		return GdiMakeRect( x, y, TextSize.cx, TextSize.cy );
	}

	return GdiMakeRect( 0, 0, 0, 0 );
}

void GdiCopyColorTable( HBITMAP hbmDst, HBITMAP hbmSrc )
{
	RGBQUAD ColorTable[256];
	GdiGetColorTable( hbmSrc, ColorTable, 256 );
	GdiSetColorTable( hbmDst, ColorTable, 256 );
}

void GdiGetColorTable( HBITMAP hbm, RGBQUAD *pPalette, int PaletteSize )
{
	HDC hdc = CreateCompatibleDC(0);

	hbm = (HBITMAP) SelectObject( hdc, hbm );

	int i;
	int c = (int) GetDIBColorTable( hdc, 0, PaletteSize, pPalette );

	if( c != PaletteSize )
	{
		for( i = c; i < PaletteSize; i++ )
		{
			pPalette[i].rgbRed = 0xff;
			pPalette[i].rgbGreen = 0x00;
			pPalette[i].rgbBlue = 0xff;
			pPalette[i].rgbReserved = 0xff;
		}
	}

	hbm = (HBITMAP) SelectObject( hdc, hbm );

	DeleteDC( hdc );

	for( i = 0; i < PaletteSize; i++ )
		pPalette[i].rgbReserved = 0xff;

	return;
}

void GdiSetColorTable( HBITMAP hbm, RGBQUAD *pPalette, int PaletteSize )
{
	HDC hdc = CreateCompatibleDC(0);

	hbm = (HBITMAP) SelectObject( hdc, hbm );

	SetDIBColorTable( hdc, 0, PaletteSize, pPalette );

	hbm = (HBITMAP) SelectObject( hdc, hbm );

	DeleteDC( hdc );
}

HBITMAP GdiLoadBitmap( HINSTANCE hInstance, int Resource, DWORD Flags )
{
	return (HBITMAP) LoadImage( hInstance, MAKEINTRESOURCE(Resource), IMAGE_BITMAP, 0, 0, Flags );
}