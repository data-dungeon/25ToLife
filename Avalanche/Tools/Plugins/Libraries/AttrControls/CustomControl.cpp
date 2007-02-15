//
//	CustomControl
//	

/*
 * $History: CustomControl.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/21/02   Time: 4:07p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * spinner, sliders, interfaces, and implemtations
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/20/02   Time: 4:20p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * working on custom control port
 */

#include "stdafx.h"
#include "CustomControl.h"
#include "ICustomEditBox.h"
#include "ICustomSlider.h"
#include "ICustomSpinner.h"

namespace AttrControls 
{

CustomControl::CustomControl(HWND hWnd) : m_hWnd(hWnd)
{
	::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this );
}

HWND CustomControl::GetHWnd(void)
{
	return m_hWnd;
}

void CustomControl::Enable(BOOL onOff)
{
	::EnableWindow(m_hWnd,onOff);
}

void CustomControl::Disable(void)
{
	::EnableWindow(m_hWnd,FALSE);
}

BOOL CustomControl::IsEnabled(void)
{
	return ::IsWindowEnabled(m_hWnd);
}

LRESULT CustomControl::OnWMNextDlgCtl( WORD wParam)
{
	return 0;
}

LRESULT CustomControl::OnLButDown( int x, int y, WORD keys )
{
	return 0;
}

LRESULT CustomControl::OnMButDown( int x, int y, WORD keys )
{
	return 0;
}

LRESULT CustomControl::OnRButDown( int x, int y, WORD keys )
{
	return 0;
}

LRESULT CustomControl::OnMouseMove( int x, int y, WORD keys )
{
	return 0;
}

LRESULT CustomControl::OnMouseWheel( int x, int y, int delta, WORD keys )
{
	return 0;
}

LRESULT CustomControl::OnVScroll( WORD code, int pos, HWND hwnd)
{
	return 0;
}

LRESULT CustomControl::OnLButUp( int x, int y, WORD keys )
{
	return 0;
}

LRESULT CustomControl::OnMButUp( int x, int y, WORD keys )
{
	return 0;
}

LRESULT CustomControl::OnCaptureReleased(HWND hWnd)
{
	return 0;
}

LRESULT	CustomControl::OnCommand( WORD id, WORD notify, HWND hCtrl )
{
	return 0;
}

LRESULT	CustomControl::OnSize( int x, int y)
{
	return 0;
}

LRESULT	CustomControl::OnPaint(void)
{
	return 0;
}

LRESULT	CustomControl::OnDroppedFiles(HDROP hDrop)
{
	return 0;
}

LRESULT	CustomControl::OnDrawItem(UINT idCtl, LPDRAWITEMSTRUCT lpdis)
{
	return 0;
}

LRESULT	CustomControl::OnTimer( UINT nIDEvent )
{
	return 0;
}

LRESULT	CustomControl::OnSetFocus( HWND hWnd )
{
	return 0;
}

LRESULT	CustomControl::OnKillFocus( HWND hWnd )
{
	return 0;
}

LRESULT	CustomControl::OnActivate( BOOL onOff )
{
	return 0;
}

LRESULT	CustomControl::OnWMEnable( BOOL enabled )
{
	return 0;
}

LRESULT	CustomControl::OnWMNotify(LPNMHDR hdr)
{
	return 0;
}

LRESULT	CustomControl::OnCTBRelay( MSG *msg )
{
	return 0;
}

LRESULT	CustomControl::OnSpinnerChange(int id, BOOL drag)
{
	return 0;
}

LRESULT	CustomControl::OnSpinnerButtonDown(WPARAM wParam,LPARAM lParam)
{
	return 0;
}

LRESULT	CustomControl::OnSpinnerButtonUp(WPARAM wParam,LPARAM lParam)
{
	return 0;
}

LRESULT	CustomControl::OnSliderChange(int id, BOOL drag)
{
	return 0;
}

LRESULT	CustomControl::OnSliderButtonDown(WPARAM wParam,LPARAM lParam)
{
	return 0;
}

LRESULT	CustomControl::OnSliderButtonUp(WPARAM wParam,LPARAM lParam)
{
	return 0;
}

LRESULT	CustomControl::OnWMCustEdit(int id,HWND hEdit)
{
	return 0;
}

LRESULT	CustomControl::OnWMMenuSelect(WPARAM wParam,LPARAM lParam)
{
	return 0;
}

LRESULT CustomControl::OnColorEdit(HDC hdc,HWND hCtrl)
{
	return 0;
}

LRESULT	CustomControl::OnKeyDown(int key, LPARAM keyData)
{
	return 0;
}

LRESULT	CustomControl::OnKeyUp(int key, LPARAM keyData)
{
	return 0;
}

LRESULT	CustomControl::OnWMChar(TCHAR ch, LPARAM keyData)
{
	return 0;
}

LRESULT CustomControl::OnDestroy(void)
{
	return 0;
}

LRESULT CustomControl::OnEraseBkgnd( HDC hDC)
{
	return 0;
}


LRESULT CALLBACK CustomControl::WindowProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{	
	LRESULT result = 0;
	switch ( message ) 
	{
		case WM_NEXTDLGCTL:
			result = OnWMNextDlgCtl(wParam);
			break;

		case WM_LBUTTONDOWN:
			result = OnLButDown( (short)LOWORD(lParam), (short)HIWORD(lParam), wParam );
			break;

		case WM_MBUTTONDOWN:
			result = OnMButDown( (short)LOWORD(lParam), (short)HIWORD(lParam), wParam );
			break;

		case WM_RBUTTONDOWN:
			result = OnRButDown( (short)LOWORD(lParam), (short)HIWORD(lParam), wParam );
			break;
			
		case WM_MOUSEMOVE:			
			result = OnMouseMove( (short)LOWORD(lParam), (short)HIWORD(lParam), wParam );
			break;

		case 0x020A:	// WM_MOUSEWHEEL 
			result = OnMouseWheel( (short)LOWORD(lParam), (short)HIWORD(lParam), (short)HIWORD(wParam), LOWORD(wParam) );
			break;

		case WM_VSCROLL:
			result = OnVScroll( LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
			break;

		case WM_LBUTTONUP:
			result = OnLButUp( (short)LOWORD(lParam), (short)HIWORD(lParam), wParam );
			break;

		case WM_MBUTTONUP:
			result = OnMButUp( (short)LOWORD(lParam), (short)HIWORD(lParam), wParam );
			break;

		case WM_CAPTURECHANGED:
			result = OnCaptureReleased((HWND) lParam );
			break;

		case WM_COMMAND:
			result = OnCommand( LOWORD(wParam), HIWORD(wParam), (HWND)lParam );
			break;

		case WM_SIZE:
			result = OnSize( (int)LOWORD(lParam), (int)HIWORD(lParam) );
			break;

		case WM_PAINT:
			result = OnPaint();
			break;

		case WM_DROPFILES:
			result = OnDroppedFiles(reinterpret_cast<HDROP>(wParam));
			break;

		case WM_DRAWITEM:
			result = OnDrawItem((UINT)wParam, (LPDRAWITEMSTRUCT)lParam);
			break;

		case WM_TIMER:
			result = OnTimer((UINT)wParam);
			break;

		case WM_SETFOCUS:
			result = OnSetFocus((HWND)wParam);
			break;

		case WM_KILLFOCUS:
			result = OnKillFocus((HWND)wParam);			
			break;

		case WM_ACTIVATE: 
			result = OnActivate(LOWORD(wParam));
			break;
		
		case WM_ENABLE:
			result = OnWMEnable((BOOL)wParam);
			break;

		case WM_NOTIFY:
			result = OnWMNotify((LPNMHDR)lParam);
			break;

		case CC_ATSPINNER_CHANGE:		
			result = OnSpinnerChange(LOWORD(wParam),HIWORD(wParam));
			break;

		case CC_ATSPINNER_BUTTONDOWN:
			result = OnSpinnerButtonDown(wParam,lParam);
			break;
		
		case CC_ATSPINNER_BUTTONUP:
			result = OnSpinnerButtonUp(wParam,lParam);
			break;

		case CC_ATSLIDER_CHANGE:		
			result = OnSliderChange(LOWORD(wParam),HIWORD(wParam));
			break;

		case CC_ATSLIDER_BUTTONDOWN:
			result = OnSliderButtonDown(wParam,lParam);
			break;
		
		case CC_ATSLIDER_BUTTONUP:
			result = OnSliderButtonUp(wParam,lParam);
			break;

		case WM_ATCUSTOMEDIT_ENTER:
			result = OnWMCustEdit((int)wParam,(HWND)lParam);
			break;

		case WM_MENUSELECT:
			result = OnWMMenuSelect(wParam,lParam);
			break;

		case WM_CTLCOLOREDIT:
			result = OnColorEdit((HDC)wParam,(HWND)lParam);
			break;
		
		case WM_KEYDOWN:
			result = OnKeyDown((int)wParam,lParam);
			break;

		case WM_KEYUP:
			result = OnKeyUp((int)wParam,lParam);
			break;

		case WM_CHAR:
			result = OnWMChar((TCHAR)wParam,lParam);
			break;

		case WM_CTLCOLORLISTBOX:
			return 0;

		case WM_DESTROY:
			result = OnDestroy();
			break;

		case WM_ERASEBKGND:
			result = OnEraseBkgnd((HDC)wParam);
			break;
        
	}	
	if (!result)
		return DefWindowProc(hwnd,message,wParam,lParam);
	else
		return result;
}

} // AttrControls
