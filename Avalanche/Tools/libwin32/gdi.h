#ifndef __GDI_H__
#define __GDI_H__

#include <windows.h>

void GdiLine( HDC hdc, int x1, int y1, int x2, int y2, COLORREF c );
void GdiFrame( HDC hdc, int x, int y, int Width, int Height, COLORREF c );
void GdiFrame( HDC hdc, RECT &rc, COLORREF c );
void GdiSetRect( LPRECT pRect, int x, int y, int Width, int Height );
RECT GdiMakeRect( int x, int y, int Width, int Height );
BOOL GdiPointInRect( int x, int y, RECT &rc );
void GdiFillRect( HDC hdc, int x, int y, int Width, int Height, COLORREF Color );
void GdiFillRect( HDC hdc, RECT &rc, COLORREF Color );

//////////////////////////////////////////////////////////////////////////

COLORREF AssBackwardsColor( COLORREF c );

//////////////////////////////////////////////////////////////////////////

HFONT GdiCreateSimilarFontEx( HFONT hFont, int Height, int Width, int Escapement, int Orientation, int Weight, DWORD Italic, DWORD Underline,
	DWORD Strikeout, DWORD CharacterSet, DWORD OutputPrecision, DWORD ClipPrecesion, DWORD Quality, DWORD PitchAndFamily );

//////////////////////////////////////////////////////////////////////////

HFONT GdiCreateSimilarFont( HFONT hFont, int Height, int Weight, DWORD Itatlic, DWORD Underline );
HFONT GdiCreateFont( int Height, int Weight, DWORD Italic, DWORD Underline, LPCTSTR pFaceName );
TEXTMETRIC GdiTextMetrics( HFONT hFont );
int GdiGetFontHeight( HFONT hFont );

//////////////////////////////////////////////////////////////////////////

void GdiCenterText( HDC hdc, int x1, int y1, int x2, int y2, LPCTSTR pText );
void GdiTextInRect( HDC hdc, int x1, int y1, int x2, int y2, LPCTSTR pText, float tx, float ty );
RECT GdiMakeTextRect( HDC hdc, int x, int y, LPCTSTR pText );

//////////////////////////////////////////////////////////////////////////

void GdiCopyColorTable( HBITMAP hbmDst, HBITMAP hbmSrc );
void GdiGetColorTable( HBITMAP hbm, RGBQUAD *pPalette, int PaletteSize );
void GdiSetColorTable( HBITMAP hbm, RGBQUAD *pPalette, int PaletteSize );

//////////////////////////////////////////////////////////////////////////

HBITMAP GdiLoadBitmap( HINSTANCE hInstance, int Resource, DWORD Flags );

#endif // __GDI_H__