//
//	WinUtil.cpp
//	

/*
 * $History: WinUtil.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 9:58a
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * added new controls to numeric type.
 * Fixed color issues with Max
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/22/02   Time: 1:18p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * all compiling
 */

#include "StdAfx.h"
#include "WinUtil.h"
#include "CustomControls.h"

namespace AttrControls
{

int WinUtil::GetCheckBox(HWND hw, int id) 
{
	return (int)SendMessage( GetDlgItem(hw, id), BM_GETCHECK, 0, 0L);
}

void WinUtil::SetCheckBox(HWND hw, int id, BOOL b) 
{
	SendMessage( GetDlgItem(hw, id), BM_SETCHECK, b, 0L);
} 

float WinUtil::GetWindowFloat(HWND hwnd,BOOL *valid)
{
	TCHAR buf[64], *endPtr=NULL;	
	GetWindowText(hwnd, buf, 64);
	float res = (float)_tcstod(buf, &endPtr);
	if (valid) 
	{
		if (!endPtr || *endPtr!='\0') *valid = FALSE;
		else *valid = TRUE;
	}
	return res;
}

int WinUtil::GetWindowInt(HWND hwnd,BOOL *valid)
{
	TCHAR buf[64], *endPtr=NULL;
	GetWindowText(hwnd, buf, 64);
	int res = (int)_tcstol(buf, &endPtr, 10);
	if (valid) 
	{
		if (!endPtr || *endPtr!='\0') *valid = FALSE;
		else *valid = TRUE;
	}
	return res;
}

BOOL WinUtil::SetWindowTextInt( HWND hwnd, int i )
{
	TCHAR buf[64];
	_sntprintf( buf, 64, _T("%d"), i );
	return SetWindowText( hwnd, buf );
}

// strip off all but one trailing zero 
static void StripTrailingZeros(TCHAR* buf) 
{
	int n = strlen(buf);
	for (int i=0; i<n; i++) {
		if (buf[i] == _T('.') || buf[i] == _T(',')) 
		{
			for (int j = n-1; j>i+1; j--) 
			{
				if (buf[j]==_T('0')) 
					buf[j] = 0;
				else 
					break;
			}
			break;
		}
	}
}

BOOL WinUtil::SetWindowTextFloat( HWND hwnd, float f, const char *pFormat)
{
	TCHAR buf[64];
	TCHAR format[64];
	_sntprintf( format, 64, _T("%%%sf"), pFormat );
	_sntprintf( buf, 64, format, f );	
	StripTrailingZeros(buf);
	return SetWindowText( hwnd, buf );
}

BOOL WinUtil::SetDlgItemFloat( HWND hwnd, int idControl, float val ) 
{
	TCHAR buf[64];
	_sntprintf( buf, 64, _T("%f"), val );
	StripTrailingZeros(buf);
	return SetDlgItemText( hwnd, idControl, buf );
}

float WinUtil::GetDlgItemFloat( HWND hwnd, int idControl, BOOL *valid )
{
	TCHAR buf[64], *endPtr=NULL;
	GetDlgItemText(hwnd, idControl, buf, 64);
	return (float)_tcstod( buf, NULL );	
}

static BOOL CALLBACK SetFontEnum( HWND hwnd, LPARAM lParam )
{	
	SendMessage( hwnd, WM_SETFONT, (WPARAM)lParam, MAKELONG(0, 0));
	return TRUE;
}

void WinUtil::SetDlgFont( HWND hDlg, HFONT hFont )
{
	EnumChildWindows( hDlg, SetFontEnum, (LPARAM)hFont );
}

void WinUtil::SlideWindow( HWND hwnd, int x, int y )
{
	SetWindowPos( hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
}

void WinUtil::StretchWindow( HWND hwnd, int w, int h )
{
	SetWindowPos( hwnd, NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER );
}

BOOL WinUtil::CenterWindow(HWND hWndChild, HWND hWndParent)
{
	RECT	rChild, rParent;
	int		wChild, hChild, wParent, hParent;
	int		wScreen, hScreen, xNew, yNew;
	HDC		hdc = GetDC(hWndChild);

	// get the display limits
	wScreen = GetDeviceCaps(hdc, HORZRES);
	hScreen = GetDeviceCaps(hdc, VERTRES);

	// Get the Height and Width of the parent window
	if(hWndParent)
		GetWindowRect(hWndParent, &rParent);
	else 
	{
		rParent.left = 0;
		rParent.right = wScreen;
		rParent.top = 0;
		rParent.bottom = hScreen;
	}
	wParent = rParent.right - rParent.left;
	hParent = rParent.bottom - rParent.top;

	// get the Height and Width of the child window
	GetWindowRect(hWndChild, &rChild);
	wChild = rChild.right - rChild.left;
	hChild = rChild.bottom - rChild.top;

	// calculate new X position, then adjust for screen
	xNew = rParent.left + ((wParent - wChild) / 2);
	if(xNew < 0)
		xNew = 0;
	else if ((xNew + wChild) > wScreen)
		xNew = wScreen - wChild;

	// calculate new Y position, then adjust for screen
	yNew = rParent.top + ((hParent - hChild) / 2);
	if(yNew < 0)
		yNew = 0;
	else if((yNew + hChild) > hScreen)
		yNew = hScreen - hChild;

	ReleaseDC(hWndChild, hdc);
	// set it, and return
	return SetWindowPos(hWndChild, NULL, xNew, yNew, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void WinUtil::GetClientRectP( HWND hwnd, RECT *rect )
{
	POINT  pt[2];
	GetClientRect( hwnd, rect );
	pt[0].x = rect->left;
	pt[0].y = rect->top;
	pt[1].x = rect->right;
	pt[1].y = rect->bottom;
	MapWindowPoints( hwnd, GetParent(hwnd), pt, 2 );
	rect->left   = pt[0].x;
	rect->top    = pt[0].y;
	rect->right  = pt[1].x;
	rect->bottom = pt[1].y;
}

void WinUtil::DrawIconButton( HDC hdc, HBITMAP hBitmap, RECT& wrect, RECT& brect, BOOL in )
{
    HBRUSH hBrush = CustomControls::GetColorBrush(COLOR_BTNFACE);
	HPEN hLight = CreatePen( PS_SOLID, 0, CustomControls::GetColor( COLOR_BTNHIGHLIGHT ) );
	HPEN hDark  = CreatePen( PS_SOLID, 0, CustomControls::GetColor( COLOR_BTNSHADOW ) );	
	HDC hdcMem;
	int x, y;
	int wrectX = min(wrect.left,wrect.right);
	int wrectY = min(wrect.top,wrect.bottom);
	int wrectW = abs(wrect.right-wrect.left)+1;
	int wrectH = abs(wrect.bottom-wrect.top)+1;

	int brectX = min(brect.left,brect.right);
	int brectY = min(brect.top,brect.bottom);
	int brectW = abs(brect.right-brect.left)+1;
	int brectH = abs(brect.bottom-brect.top)+1;

	x = wrectX + wrectW/2 - brectW/2;
	y = wrectY + wrectH/2 - brectH/2;

	SelectObject( hdc, in?hDark:hLight );		
	SelectObject( hdc, hBrush );
	SetBkColor( hdc, CustomControls::GetColor(COLOR_BTNFACE) );
	Rectangle( hdc, wrect.left,wrect.top,wrect.right,wrect.bottom);
	SelectObject( hdc, GetStockObject(BLACK_PEN) );
	SelectObject( hdc, in?hLight:hDark );	
	MoveToEx( hdc, wrect.right-1, wrect.top, NULL );
	LineTo( hdc, wrect.right-1, wrect.bottom-1 );
	LineTo( hdc, wrect.left, wrect.bottom-1 );
	
	hdcMem = CreateCompatibleDC( hdc );
	SelectObject( hdcMem, hBitmap );
	BitBlt( hdc, x, y, brectW-1, brectH-1, hdcMem, brectX, brectY, SRCCOPY );
	DeleteDC( hdcMem );

	SelectObject( hdc, GetStockObject( WHITE_BRUSH ) );
	SelectObject( hdc, GetStockObject( NULL_PEN ) );
	DeleteObject( hLight );
	DeleteObject( hDark );	
}

int WinUtil::GetListHieght( HWND hList )
{
	int sy = GetSystemMetrics(SM_CYSCREEN);
	LRESULT count   = SendMessage( hList, CB_GETCOUNT, 0, 0 ) + 1;
	LRESULT height  = SendMessage( hList, CB_GETITEMHEIGHT, 0, 0 );
	LRESULT sheight = SendMessage( hList, CB_GETITEMHEIGHT, (WPARAM)-1, 0 );
	int res = (int)(count * height + sheight);
	return sy<res?sy:res;
}


void WinUtil::ShadedVertLine( HDC hdc, int x, int y0, int y1, BOOL in )
{
	HPEN hLight = CreatePen( PS_SOLID, 0, CustomControls::GetColor( COLOR_BTNHIGHLIGHT ) );
	HPEN hDark  = CreatePen( PS_SOLID, 0, CustomControls::GetColor( COLOR_BTNSHADOW ) );
	
	SelectObject( hdc, in ? hDark : hLight);
	MoveToEx( hdc, x, y0, NULL );
	LineTo( hdc, x, y1 );
	SelectObject( hdc, GetStockObject(BLACK_PEN) );
	SelectObject( hdc, in ? hLight : hDark );
	MoveToEx( hdc, x+1, y0, NULL );
	LineTo( hdc, x+1, y1 );
	
	SelectObject( hdc, GetStockObject(BLACK_PEN) );
	DeleteObject( hLight );
	DeleteObject( hDark );	
}


void WinUtil::ShadedHorizLine( HDC hdc, int y, int x0, int x1, BOOL in )
{
	HPEN hLight = CreatePen( PS_SOLID, 0, CustomControls::GetColor( COLOR_BTNHIGHLIGHT ) );
	HPEN hDark  = CreatePen( PS_SOLID, 0, CustomControls::GetColor( COLOR_BTNSHADOW ) );
	
	SelectObject( hdc, in?hDark:hLight );
	MoveToEx( hdc, x0, y, NULL );
	LineTo( hdc, x1, y );
	SelectObject( hdc, GetStockObject(BLACK_PEN) );
	SelectObject( hdc, in?hLight:hDark );
	MoveToEx( hdc, x0, y+1, NULL );
	LineTo( hdc, x1, y+1 );
	
	SelectObject( hdc, GetStockObject(BLACK_PEN) );
	DeleteObject( hLight );
	DeleteObject( hDark );
}

void WinUtil::ShadedRect( HDC hdc, RECT& rect )
{
	HPEN hLight = CreatePen( PS_SOLID, 0, CustomControls::GetColor( COLOR_BTNHIGHLIGHT ) );
	HPEN hDark  = CreatePen( PS_SOLID, 0, CustomControls::GetColor( COLOR_BTNSHADOW ) );

	SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
	SelectObject( hdc, hDark );
	Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
	SelectObject( hdc, GetStockObject(BLACK_PEN) );
	SelectObject( hdc, hLight );
	Rectangle( hdc, rect.left+1, rect.top+1, rect.right+1, rect.bottom+1 );

	SelectObject( hdc, GetStockObject(BLACK_PEN) );
	DeleteObject( hLight );
	DeleteObject( hDark );
}

void WinUtil::Rect3D( HDC hdc, RECT& rect, BOOL in )
{
	HPEN hLight = CreatePen( PS_SOLID, 0, CustomControls::GetColor( COLOR_BTNHIGHLIGHT ) );
	HPEN hDark = CreatePen( PS_SOLID, 0, CustomControls::GetColor( COLOR_BTNSHADOW ) );
	
	SelectObject( hdc, in ? hDark : hLight );
	MoveToEx( hdc, rect.left, rect.bottom, NULL );
	LineTo( hdc, rect.left, rect.top );
	LineTo( hdc, rect.right, rect.top );
	SelectObject( hdc, in ? hLight : hDark );
	LineTo( hdc, rect.right, rect.bottom );
	LineTo( hdc, rect.left, rect.bottom );
	SelectObject( hdc, GetStockObject(BLACK_PEN) );
	DeleteObject( hLight );
	DeleteObject( hDark );
}

void WinUtil::WhiteRect3D( HDC hdc, RECT rect, BOOL in )
{
	HPEN hLight = CreatePen( PS_SOLID, 0, CustomControls::GetColor( COLOR_BTNHIGHLIGHT ) );
	HPEN hDark  = CreatePen( PS_SOLID, 0, CustomControls::GetColor( COLOR_BTNSHADOW ) );
	HPEN hGray  = CreatePen( PS_SOLID, 0, CustomControls::GetColor(COLOR_BTNFACE) );
	HPEN hBlack = (HPEN)GetStockObject( BLACK_PEN );

	rect.right--;
	rect.bottom--;

	SelectObject( hdc, in ? hDark : hLight );
	MoveToEx( hdc, rect.left, rect.bottom, NULL );
	LineTo( hdc, rect.left, rect.top );
	LineTo( hdc, rect.right, rect.top );
	SelectObject( hdc, GetStockObject(BLACK_PEN) );
	SelectObject( hdc, in ? hLight : hDark );
	LineTo( hdc, rect.right, rect.bottom );
	LineTo( hdc, rect.left, rect.bottom );

	rect.left++;
	rect.right--;
	rect.top++;
	rect.bottom--;

	SelectObject( hdc, GetStockObject(BLACK_PEN) );
	SelectObject( hdc, in ? hBlack : hGray );
	MoveToEx( hdc, rect.left, rect.bottom, NULL );
	LineTo( hdc, rect.left, rect.top );
	LineTo( hdc, rect.right, rect.top );
	SelectObject( hdc, GetStockObject(BLACK_PEN) );
	SelectObject( hdc, in ? hGray : hBlack );
	LineTo( hdc, rect.right, rect.bottom );
	LineTo( hdc, rect.left, rect.bottom );

	SelectObject( hdc, GetStockObject(BLACK_PEN) );
	DeleteObject( hLight );
	DeleteObject( hDark );
	DeleteObject( hGray );
}

void WinUtil::DrawButton( HDC hdc, RECT rect, BOOL in )
	{
	HPEN hLight;
	HPEN hDark;

	hLight = CreatePen( PS_SOLID, 0, CustomControls::GetColor( COLOR_BTNHILIGHT ) );
	hDark  = CreatePen( PS_SOLID, 0, CustomControls::GetColor( COLOR_BTNSHADOW ) );

	rect.right -= 1;
	rect.bottom -= 1;

	// top and left sides
	SelectObject( hdc, in ? hDark : hLight );
	MoveToEx( hdc, rect.left, rect.bottom, NULL );
	LineTo( hdc, rect.left, rect.top );
	LineTo( hdc, rect.right, rect.top );

	// bottom and right sides
	SelectObject( hdc, in ? hLight : hDark);
	MoveToEx( hdc, rect.right, rect.top, NULL );
	LineTo( hdc, rect.right, rect.bottom );
	LineTo( hdc, rect.left, rect.bottom );

	SelectObject( hdc, GetStockObject(BLACK_PEN) );
	SelectObject( hdc, GetStockObject(NULL_PEN) );
	DeleteObject( hLight );
	DeleteObject( hDark );
}

void WinUtil::RemoveBorder( HDC hdc, RECT rect, COLORREF bgcolor )
{
	HPEN hPen = CreatePen( PS_SOLID, 0, bgcolor );

	rect.right -= 1;
	rect.bottom -= 1;

	// top and left sides
	SelectObject( hdc, hPen );
	MoveToEx( hdc, rect.left, rect.bottom, NULL );
	LineTo( hdc, rect.left, rect.top );
	LineTo( hdc, rect.right, rect.top );

	// bottom and right sides
	MoveToEx( hdc, rect.right, rect.top, NULL );
	LineTo( hdc, rect.right, rect.bottom );
	LineTo( hdc, rect.left, rect.bottom );

	rect.left++;
	rect.top++;
	rect.right--;
	rect.bottom--;

	MoveToEx( hdc, rect.right, rect.top, NULL );
	LineTo( hdc, rect.right, rect.bottom );
	LineTo( hdc, rect.left, rect.bottom );
	SelectObject( hdc, GetStockObject(BLACK_PEN) );

	DeleteObject( hPen );
}


void WinUtil::MakeButton2State(HWND hCtrl)
{
	LONG_PTR style = GetWindowLongPtr(hCtrl,GWL_STYLE);
	style &= ~(BS_AUTO3STATE);
	style |= BS_AUTOCHECKBOX;
	SetWindowLongPtr(hCtrl,GWL_STYLE,style);
}

void WinUtil::MakeButton3State(HWND hCtrl)
{
	LONG_PTR style = GetWindowLongPtr(hCtrl,GWL_STYLE);
	style &= ~(BS_AUTOCHECKBOX);
	style |= BS_AUTO3STATE;
	SetWindowLongPtr(hCtrl,GWL_STYLE,style);
}


void WinUtil::XORRect(HDC hdc, RECT& r, int border) 
{
	HPEN svPen = (HPEN)SelectObject(hdc,GetStockObject(WHITE_PEN));
	HBRUSH svBrush = (HBRUSH)SelectObject(hdc,GetStockObject(NULL_BRUSH));
	int svDrMode = GetROP2(hdc);
	SetROP2(hdc, R2_XORPEN);
	for (int i=0; i<border; i++) 
	{
		Rectangle(hdc, r.left+i, r.top+i, r.right-i, r.bottom-i);
	}
	SetROP2(hdc, svDrMode);
	SelectObject(hdc,svPen);
	SelectObject(hdc,svBrush);
}

int WinUtil::GetBitsPerPixel()
{
	HDC hdc = GetDC(GetDesktopWindow());
	int bits = GetDeviceCaps(hdc,BITSPIXEL);
	ReleaseDC(GetDesktopWindow(),hdc);
	return bits;
}


void WinUtil::DrawTriangle( HDC hdc, int x, int y, HPEN hText, HPEN hDark, int dir)
{
	SelectObject( hdc, hText );
	if(dir == 0) 
	{
		MoveToEx(hdc, x, y+2, NULL);
		LineTo(hdc, x+2, y);
		LineTo(hdc, x+4, y+2);
		LineTo(hdc, x, y+2);
		MoveToEx(hdc, x+2, y, NULL);
		LineTo(hdc, x+2, y+2);
	}
	else 
	{
		MoveToEx(hdc, x, y, NULL);
		LineTo(hdc, x+4, y);
		LineTo(hdc, x+2, y+2);
		LineTo(hdc, x, y);
		MoveToEx(hdc, x+2, y, NULL);
		LineTo(hdc, x+2, y+2);
	}
}

} // AttrControls

