//
//	WinUtil.h
//	

/*
 * $History: WinUtil.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/22/02   Time: 1:18p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * all compiling
 */

#ifndef ATTR_CONTROLS_WIN_UTIL_H
#define ATTR_CONTROLS_WIN_UTIL_H

namespace AttrControls
{

class WinUtil
{
public:
	static float GetWindowFloat(HWND hwnd,BOOL *valid=NULL);
	static int GetWindowInt(HWND hwnd,BOOL *valid=NULL);
	static BOOL SetWindowTextInt( HWND hwnd, int i );
	static BOOL SetWindowTextFloat( HWND hwnd, float f, const char *pFormat);
	static BOOL SetDlgItemFloat( HWND hwnd, int idControl, float val );
	static float GetDlgItemFloat( HWND hwnd, int idControl, BOOL *valid=NULL );
	static void SetDlgFont( HWND hDlg, HFONT hFont );
	static void SlideWindow( HWND hwnd, int x, int y );
	static void StretchWindow( HWND hwnd, int w, int h );
	static BOOL CenterWindow(HWND hWndChild, HWND hWndParent);
	static void GetClientRectP( HWND hwnd, RECT *rect );
	static void DrawIconButton( HDC hdc, HBITMAP hBitmap, RECT& wrect, RECT& brect, BOOL in );
	static int GetListHieght( HWND hList );
	static void ShadedVertLine( HDC hdc, int x, int y0, int y1, BOOL in );
	static void ShadedHorizLine( HDC hdc, int y, int x0, int x1, BOOL in );
	static void ShadedRect( HDC hdc, RECT& rect );
	static void Rect3D( HDC hdc, RECT& rect, BOOL in );
	static void WhiteRect3D( HDC hdc, RECT rect, BOOL in );
	static void DrawButton( HDC hdc, RECT rect, BOOL in );
	static void RemoveBorder( HDC hdc, RECT rect, COLORREF bgcolor );
	static void XORRect(HDC hdc, RECT& r, int border=1);
	static void MakeButton2State(HWND hCtrl);
	static void MakeButton3State(HWND hCtrl);
	static int GetCheckBox(HWND hw, int id);
	static void SetCheckBox(HWND hw, int id, BOOL b);
	static int GetBitsPerPixel();
	static void DrawTriangle( HDC hdc, int x, int y, HPEN hText, HPEN hDark, int dir);
};

} // AttrControls
#endif