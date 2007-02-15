//
//	CustomControl
//	

/*
 * $History: CustomControls.cpp $
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 2/14/03    Time: 2:15p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * use of new window class prefixes
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 12/20/02   Time: 10:39a
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 12/19/02   Time: 3:47p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 12/13/02   Time: 12:03p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * added new cursors
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 11/27/02   Time: 10:21a
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * callbacks setup and focus bugs
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 4:04p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * Rollups in
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 9:58a
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * added new controls to numeric type.
 * Fixed color issues with Max
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 11/22/02   Time: 1:18p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * all compiling
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
#include "CustomControls.h"
#include "CustomControl.h"
#include "CustomEditBox.h"
#include "CustomSpinner.h"
#include "CustomSlider.h"
#include "CustomRollupPanel.h"
#include "CustomRollupWindow.h"
#include "CustomTooltip.h"
#include "resource.h"

namespace AttrControls 
{

const char *CustomControls::m_windowClassNames[NUM_WINDOW_CLASSES] = 
{
	"AttrGray",
	"AttrLtGray",
	"AttrWhite",
	"AttrEditDummy",
	"AttrSpinner",
	"AttrSlider",
	"AttrRollupPanel",
	"AttrRollupPanelTitle",
	"AttrRollupWindow",
	"AttrEdit",
	"AttrToolTip",
};

HFONT CustomControls::m_hFixedFont = NULL;
HFONT CustomControls::m_hFixedFontBold = NULL;
HFONT CustomControls::m_hFixedFontItalic = NULL;
HFONT CustomControls::m_hFixedFontBoldItalic = NULL;
HBRUSH CustomControls::m_hLTGrayBrush = NULL;
HBRUSH CustomControls::m_hDKGrayBrush = NULL;
HBRUSH CustomControls::m_hBrushWindow = NULL;
HBRUSH CustomControls::m_hBrushBtnShadow = NULL;
HBRUSH CustomControls::m_hBrushBtnFace = NULL;
HBRUSH CustomControls::m_hBrushBtnText = NULL;
HCURSOR CustomControls::m_hPanCur = NULL;
HCURSOR CustomControls::m_hSpinCur = NULL;
HCURSOR CustomControls::m_hDragOK = NULL;
HCURSOR CustomControls::m_hDragNO = NULL;
HCURSOR CustomControls::m_hSliderCur = NULL;
HCURSOR CustomControls::m_hDockCur = NULL;
HCURSOR CustomControls::m_hFloatCur = NULL;
HCURSOR CustomControls::m_hCUICur = NULL;
DWORD CustomControls::m_colorWindow = RGB(0,0,0);
DWORD CustomControls::m_colorWindowText = RGB(0,0,0);
DWORD CustomControls::m_colorBtnFace = RGB(0,0,0);
DWORD CustomControls::m_colorBtnHighlight = RGB(0,0,0);
DWORD CustomControls::m_colorBtnShadow = RGB(0,0,0);
DWORD CustomControls::m_colorBtnText = RGB(0,0,0);
DWORD CustomControls::m_colorRollupTitleFace = RGB(0,0,0);
DWORD CustomControls::m_colorRollupTitleText = RGB(0,0,0);
DWORD CustomControls::m_colorRollupTitleHiLight = RGB(0,0,0);
DWORD CustomControls::m_colorRollupTitleShadow = RGB(0,0,0);

WNDPROC CustomControls::m_editWndProc = NULL;
void (CALLBACK *CustomControls::m_pEditSetFocusCallback)(void) = NULL;
void (CALLBACK *CustomControls::m_pEditKillFocusCallback)(void) = NULL;

HINSTANCE CustomControls::m_hInstance = NULL;

void CustomControls::InitVariables( HINSTANCE hInst)
{
	m_hInstance = hInst;

	if ( !m_hFixedFont ) 
	{
		LOGFONT lf;
		GetObject(GetStockObject(SYSTEM_FONT), sizeof(lf), &lf);
		lf.lfWeight = 400;
		lf.lfHeight = 14;
		lf.lfWidth = 0;
		strcpy((char *)&lf.lfFaceName, "MS Sans Serif");
		m_hFixedFont = CreateFontIndirect(&lf);		
	}
	if ( !m_hFixedFontBold ) 
	{
		LOGFONT lf;
		GetObject(GetStockObject(SYSTEM_FONT), sizeof(lf), &lf);
		lf.lfWeight = 700;
		lf.lfHeight = 14;
		lf.lfWidth = 0;
		strcpy((char *)&lf.lfFaceName, "MS Sans Serif");
		m_hFixedFontBold = CreateFontIndirect(&lf);		
	}
	if ( !m_hFixedFontItalic) 
	{
		LOGFONT lf;
		GetObject(GetStockObject(SYSTEM_FONT), sizeof(lf), &lf);
		lf.lfWeight = 400;
		lf.lfHeight = 14;
		lf.lfWidth = 0;
		lf.lfItalic = TRUE;
		strcpy((char *)&lf.lfFaceName, "MS Sans Serif");
		m_hFixedFontItalic = CreateFontIndirect(&lf);		
	}
	if ( !m_hFixedFontBoldItalic) 
	{
		LOGFONT lf;
		GetObject(GetStockObject(SYSTEM_FONT), sizeof(lf), &lf);
		lf.lfWeight = 700;
		lf.lfHeight = 14;
		lf.lfWidth = 0;
		lf.lfItalic = TRUE;
		strcpy((char *)&lf.lfFaceName, "MS Sans Serif");
		m_hFixedFontBoldItalic = CreateFontIndirect(&lf);		
	}
	
	if ( !m_hLTGrayBrush ) 
	{		
		m_hLTGrayBrush  = CreateSolidBrush(RGB(225,225,225));
	}
	if ( !m_hDKGrayBrush ) 
	{
		m_hDKGrayBrush  = CreateSolidBrush(RGB(150,150,150));
	}
	if ( !m_hBrushBtnFace ) 
	{
		m_hBrushBtnFace  = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	}
	if ( !m_hBrushBtnShadow ) 
	{
		m_hBrushBtnShadow  = CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
	}
	if ( !m_hBrushWindow ) 
	{
		m_hBrushWindow  = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	}
	if ( !m_hBrushBtnText ) 
	{
		m_hBrushBtnText  = CreateSolidBrush(GetSysColor(COLOR_BTNTEXT));
	}
	if (!m_hPanCur) 
	{
		m_hPanCur = LoadCursor(hInst, MAKEINTRESOURCE(IDC_ATPANHAND)); //LoadCursor(hInstance,MAKEINTRESOURCE(IDC_PANHAND));
	}
	if (!m_hCUICur) 
	{
		m_hCUICur = LoadCursor(NULL, IDC_ARROW); //LoadCursor(hInstance,MAKEINTRESOURCE(IDC_CUI_CUR));
	}
	if (!m_hDockCur) 
	{
		m_hDockCur = LoadCursor(NULL, IDC_ARROW); //LoadCursor(hInstance,MAKEINTRESOURCE(IDC_DOCK_CUR));
	}
	if (!m_hFloatCur) 
	{
		m_hFloatCur = LoadCursor(NULL, IDC_ARROW); //LoadCursor(hInstance,MAKEINTRESOURCE(IDC_FLT_CUR));
	}
	if (!m_hSpinCur) 
	{
		m_hSpinCur = LoadCursor(hInst, MAKEINTRESOURCE(IDC_ATSPINNERCUR)); //LoadCursor( hInstance, MAKEINTRESOURCE(IDC_SPINNERCUR) );
	}
	if (!m_hSliderCur) 
	{
		m_hSliderCur = LoadCursor(hInst, MAKEINTRESOURCE(IDC_ATSLIDERCUR)); //LoadCursor( hInstance, MAKEINTRESOURCE(IDC_SLIDER_CUR) );
	}
	if (!m_hDragOK) 
	{
		m_hDragOK = LoadCursor(NULL, IDC_ARROW); //LoadCursor(hInstance,MAKEINTRESOURCE(IDC_DRAGOKARROW));
	}
	if (!m_hDragNO) 
	{
		m_hDragNO = LoadCursor(NULL, IDC_ARROW); //LoadCursor(hInstance,MAKEINTRESOURCE(IDC_DRAGNOARROW));
	}

	m_colorWindow = GetSysColor(COLOR_WINDOW);
	m_colorWindowText = GetSysColor(COLOR_WINDOWTEXT);
	m_colorBtnFace = GetSysColor(COLOR_BTNFACE);
	m_colorBtnHighlight = GetSysColor(COLOR_BTNHIGHLIGHT);
	m_colorBtnShadow = GetSysColor(COLOR_BTNSHADOW);
	m_colorBtnText = GetSysColor(COLOR_BTNTEXT);
	m_colorRollupTitleFace = GetSysColor(COLOR_BTNFACE);
	m_colorRollupTitleText = GetSysColor(COLOR_BTNTEXT);
	m_colorRollupTitleHiLight = GetSysColor(COLOR_BTNHIGHLIGHT);
	m_colorRollupTitleShadow = GetSysColor(COLOR_BTNSHADOW);
}

void CustomControls::InitCustomControls( HINSTANCE hInst,const char *pPrefixString)
{
	static bool bClassesRegistered = false;
	WNDCLASS  wc;

	if (!bClassesRegistered) 
	{
		bClassesRegistered = true;

		wc.style         = 0;
		wc.hInstance     = hInst;
		wc.hIcon         = NULL;
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = m_hBrushBtnFace;
		wc.lpszMenuName  = NULL;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.lpfnWndProc   = DefWindowProc;
		wc.lpszClassName = m_windowClassNames[GRAY_WINDOW_CLASS];

		RegisterClass(&wc);

		wc.style         = 0;
		wc.hInstance     = hInst;
		wc.hIcon         = NULL;
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = m_hLTGrayBrush;
		wc.lpszMenuName  = NULL;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.lpfnWndProc   = DefWindowProc;
		wc.lpszClassName = m_windowClassNames[LTGRAY_WINDOW_CLASS];

		RegisterClass(&wc);

		wc.style         = 0;
		wc.hInstance     = hInst;
		wc.hIcon         = NULL;
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.lpfnWndProc   = DefWindowProc;
		wc.lpszClassName = m_windowClassNames[WHITE_WINDOW_CLASS];

		RegisterClass(&wc);

		wc.style         = 0;
		wc.hInstance     = hInst;
		wc.hIcon         = NULL;
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName  = NULL;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.lpfnWndProc   = EditDummyWndProc;
		wc.lpszClassName = m_windowClassNames[EDIT_DUMMY_WINDOW_CLASS];

		RegisterClass(&wc);

		wc.style         = 0;
		wc.hInstance     = hInst;
		wc.hIcon         = NULL;
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = m_hBrushBtnFace;
		wc.lpszMenuName  = NULL;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.lpfnWndProc   = ControlWndProc;
		wc.lpszClassName = m_windowClassNames[EDIT_WINDOW_CLASS];


		RegisterClass(&wc);

		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.hInstance     = hInst;
		wc.hIcon         = NULL;
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = m_hBrushBtnFace;
		wc.lpszMenuName  = NULL;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.lpfnWndProc   = ControlWndProc;
		wc.lpszClassName = m_windowClassNames[SPINNER_WINDOW_CLASS];

		RegisterClass(&wc);

		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.hInstance     = hInst;
		wc.hIcon         = NULL;
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = m_hBrushBtnFace;
		wc.lpszMenuName  = NULL;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.lpfnWndProc   = ControlWndProc;
		wc.lpszClassName = m_windowClassNames[SLIDER_WINDOW_CLASS];

		RegisterClass(&wc);

		wc.style         = 0;
		wc.hInstance     = hInst;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;
		wc.lpszMenuName  = NULL;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.lpfnWndProc   = RollupPanelWndProc;
		wc.lpszClassName = m_windowClassNames[ROLLUP_PANEL_WINDOW_CLASS];

		RegisterClass(&wc);

		wc.style         = 0;
		wc.hInstance     = hInst;
		wc.hIcon         = NULL;
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL; 
		wc.lpszMenuName  = NULL;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.lpfnWndProc   = RollupPanelTitleWndProc;
		wc.lpszClassName = m_windowClassNames[ROLLUP_PANEL_TITLE_WINDOW_CLASS];

		RegisterClass(&wc);	

		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.hInstance     = hInst;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = m_hBrushBtnFace;
		wc.lpszMenuName  = NULL;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.lpfnWndProc   = ControlWndProc;
		wc.lpszClassName = m_windowClassNames[ROLLUP_WINDOW_CLASS];

		RegisterClass(&wc);

		wc.style         = CS_SAVEBITS;
		wc.hInstance     = hInst;
		wc.hIcon         = NULL;
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName  = NULL;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.lpfnWndProc   = ControlWndProc;
		wc.lpszClassName = m_windowClassNames[TOOLTIP_WINDOW_CLASS];

		RegisterClass(&wc);

	}
}


LRESULT CALLBACK CustomControls::ControlWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	CustomControl *pControl = (CustomControl *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

	switch ( message ) 
	{
		case WM_CREATE:
			{
				TCHAR buf[256];
				GetClassName( hwnd, buf, 256 );

				if ( !_tcscmp( buf, m_windowClassNames[EDIT_WINDOW_CLASS] ) ) 
				{
					pControl = new CustomEditBox( hwnd );
				}
				else if ( !_tcscmp( buf, m_windowClassNames[SPINNER_WINDOW_CLASS] ) ) 
				{
					pControl = new CustomSpinner( hwnd );
				}	
				else if ( !_tcscmp( buf, m_windowClassNames[SLIDER_WINDOW_CLASS] ) ) 
				{
					pControl = new CustomSlider( hwnd );
				}	
				else if ( !_tcscmp( buf, m_windowClassNames[ROLLUP_WINDOW_CLASS] ) ) 
				{
					pControl = new CustomRollupWindow( hwnd );
				}
				else if ( !_tcscmp( buf, m_windowClassNames[TOOLTIP_WINDOW_CLASS] ) ) 
				{
					pControl = new CustomToolTip( hwnd );
				}
			}
			break;
			
		case WM_DESTROY:
			if ( pControl )
			{
				pControl->WindowProc(hwnd,message,wParam,lParam);
				delete pControl;
			}
			pControl = NULL;
			::SetWindowLongPtr( hwnd, GWLP_USERDATA, 0 );
			break;

		case WM_CTLCOLORSTATIC:
			SetBkColor((HDC)wParam, m_colorBtnFace);
			SetTextColor((HDC)wParam, m_colorBtnText);
			return (LRESULT)m_hBrushBtnFace;

		case WM_CTLCOLOREDIT:
			SetBkColor((HDC)wParam, m_colorWindow);
			SetTextColor((HDC)wParam, m_colorWindowText);
			return (LRESULT) m_hBrushWindow;

		case WM_CTLCOLORLISTBOX:
			SetBkColor((HDC)wParam, m_colorWindow);
			SetTextColor((HDC)wParam, m_colorWindowText);
			return (LRESULT) m_hBrushWindow;

		case WM_CTLCOLORSCROLLBAR:
			SetBkColor((HDC)wParam, m_colorBtnFace);
			return (LRESULT)m_hBrushBtnFace;
	}
 	if (!pControl) 
	{
		return DefWindowProc(hwnd,message,wParam,lParam);
	}
	else 
	{
		return pControl->WindowProc(hwnd,message,wParam,lParam);
	}
}



LRESULT CALLBACK CustomControls::EditSubWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message) 
	{
		case 0x020A: 
			{
				CustomEditBox *pControl = (CustomEditBox*)GetWindowLongPtr(GetParent(GetParent(hwnd)),GWLP_USERDATA);			
				int delta = (short)HIWORD(wParam);			
				if (delta<0 && pControl->m_pSpinner) 
				{
					pControl->m_pSpinner->OnLButDown(0,12,0);
					pControl->m_pSpinner->OnLButUp(0,12,0);
				}
				if (delta>0 && pControl->m_pSpinner)
				{
					pControl->m_pSpinner->OnLButDown(0,0,0);
					pControl->m_pSpinner->OnLButUp(0,0,0);
				}
			}
			break;
		
		case WM_CHAR: 
			{
				CustomEditBox *pControl = (CustomEditBox*)GetWindowLongPtr(GetParent(GetParent(hwnd)),GWLP_USERDATA);
				assert(pControl);
				pControl->m_bGotReturn = FALSE;
				if (wParam == VK_ESCAPE) 
				{				
					// Escape now sets focus to the top level window that is an ancestor of the edit field.
					// This is either the main window (for a spinner in the command panel) or a
					// pop-up window (in the case of a spinner in a dialog box).
					HWND hTop = hwnd;
					while (GetParent(hTop)) 
					{
						// If a window is not a child of its parent
						// then it is a pop-up.
						if (IsChild(GetParent(hTop),hTop)) 
						{
							hTop = GetParent(hTop);
						}
						else 
						{
							break;
						}
					}
					SetFocus(hTop);
					return 0;
				}
				if (wParam == 13/*enter*/) 
				{
					if(pControl->m_bWantReturn) 
					{
						pControl->m_bGotReturn = TRUE;
						HWND h = GetParent(GetParent(hwnd));
						SendMessage(GetParent(h), WM_COMMAND, 
									MAKEWPARAM(GetWindowLongPtr(h, GWL_ID), EN_CHANGE), 0);
					}
					SendMessage( GetParent(GetParent(hwnd)), WM_COMMAND, ID_ATUPDATE_SPINNER, 0 );
					SendMessage( GetParent(GetParent(hwnd)), WM_COMMAND, ID_ATUPDATE_SLIDER, 0 );
					SendMessage( hwnd, EM_SETSEL, 0, (WPARAM)((INT)-1) );
					return 0;
				}

				if (wParam==VK_TAB && pControl->m_bWantDlgNextCtrl) 
				{
					// swallow tabs
					return 0;
				}
				// If this edit field is tied to a spinner, parse out non numerics
				if (!pControl->IsValidCharacter(wParam)) 
				{
					return 0;
				}
				return  CallWindowProc(m_editWndProc,hwnd,message,wParam,lParam);		
			}

		case WM_KEYDOWN: 
			{
				CustomEditBox *pControl = (CustomEditBox*)GetWindowLongPtr(GetParent(GetParent(hwnd)),GWLP_USERDATA);
				switch ( (INT)wParam ) 
				{
					case VK_ESCAPE:
						return 0;

					case VK_UP:
						if (pControl->m_bWantDlgNextCtrl) 
						{
							PostMessage( GetParent(GetParent(GetParent(hwnd))), WM_NEXTDLGCTL, 1, 0 );
						} 
						else 
						{
							SetFocus(GetNextDlgTabItem(GetParent(GetParent(GetParent(hwnd))),hwnd,1));
						}
						return 0;

					case VK_DOWN:
						if (pControl->m_bWantDlgNextCtrl) 
						{
							PostMessage( GetParent(GetParent(GetParent(hwnd))), WM_NEXTDLGCTL, 0, 0 );
						} 
						else 
						{
							SetFocus(GetNextDlgTabItem(GetParent(GetParent(GetParent(hwnd))),hwnd,0));
						}
						return 0;
						
					case VK_TAB:
						if (pControl->m_bWantDlgNextCtrl) 
						{
							PostMessage( GetParent(GetParent(GetParent(hwnd))), WM_NEXTDLGCTL, GetKeyState(VK_SHIFT)<0, 0 );
						} 
						else 
						{					
							SetFocus(GetNextDlgTabItem(GetParent(GetParent(GetParent(hwnd))),hwnd,GetKeyState(VK_SHIFT)<0));
						}
						return 1;						
				}
				return CallWindowProc(m_editWndProc,hwnd,message,wParam,lParam);
			}

		case WM_SETFOCUS:
            // We no longer select all text when getting focus SCM 6/19/00
//			SendMessage(hwnd, EM_SETSEL, 0, (LPARAM)-1);	// highlight all text on click
			if (m_pEditSetFocusCallback)
			{
				m_pEditSetFocusCallback();
			}
			break;
		
		case WM_KILLFOCUS:
			if (m_pEditKillFocusCallback)
			{
				m_pEditKillFocusCallback();
			}
			// Update when we lose the keyboard focus.
			SendMessage( GetParent(GetParent(hwnd)), WM_COMMAND, ID_ATUPDATE_SPINNER, 0 );
			SendMessage( GetParent(GetParent(hwnd)), WM_COMMAND, ID_ATUPDATE_SLIDER, 0 );
			break;
	}	
	return CallWindowProc(m_editWndProc,hwnd,message,wParam,lParam);
}


WNDPROC CustomControls::GetEditWndProc(void)
{
	return m_editWndProc;
}

CustomControls::SetEditWndProc(WNDPROC editWndProc)
{
	m_editWndProc = editWndProc;
}

HFONT CustomControls::GetFixedFont(void)
{
	return m_hFixedFont;
}

HFONT CustomControls::GetFixedFontBold(void)
{
	return m_hFixedFontBold;
}

HFONT CustomControls::GetFixedFontItalic(void)
{
	return m_hFixedFontItalic;
}

HFONT CustomControls::GetFixedFontBoldItalic(void)
{
	return m_hFixedFontBoldItalic;
}

HCURSOR CustomControls::GetSpinCur(void)
{
	return m_hSpinCur;
}

HCURSOR CustomControls::GetSliderCur(void)
{
	return m_hSliderCur;
}

HCURSOR CustomControls::GetPanCur(void)
{
	return m_hPanCur;
}

// This just forwards messages from the edit field to the parent of the cust edit control.
LRESULT CALLBACK CustomControls::EditDummyWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message) 
	{		
    case WM_PAINT: 
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd,&ps);
        
			HBRUSH oldBrush = (HBRUSH)SelectObject(hdc,m_hBrushWindow);
			SelectObject(hdc,GetStockObject(NULL_PEN));
			Rectangle(hdc,ps.rcPaint.left,ps.rcPaint.top,ps.rcPaint.right+1,ps.rcPaint.bottom+1);			
			SelectObject(hdc, oldBrush);
			EndPaint(hwnd,&ps);
			break;
	    }
    case WM_CTLCOLOREDIT:
		SetBkColor((HDC)wParam, m_colorWindow);
        SetTextColor((HDC)wParam, m_colorWindowText);
        return (LRESULT) m_hBrushWindow;
        
    case WM_COMMAND:
        return SendMessage(GetParent(GetParent(hwnd)),message,MAKEWPARAM(GetWindowLongPtr(GetParent(hwnd),GWL_ID),HIWORD(wParam)),lParam);			
    }
	return DefWindowProc(hwnd,message,wParam,lParam);
}

HBRUSH CustomControls::GetColorBrush(int index)
{
	switch (index)
	{
		case COLOR_WINDOW:
			return m_hBrushWindow;
		case COLOR_BTNSHADOW:
			return m_hBrushBtnShadow;
		case COLOR_BTNFACE:
			return m_hBrushBtnFace;
		case COLOR_BTNTEXT:
			return m_hBrushBtnText;
		default:
			assert(false);
			return ::GetSysColorBrush(index);
	}
}

DWORD CustomControls::GetColor(int index)
{
	switch (index)
	{
		case COLOR_WINDOW:
			return m_colorWindow;
		case COLOR_WINDOWTEXT:
			return m_colorWindowText;
		case COLOR_BTNFACE:
			return m_colorBtnFace;
		case COLOR_BTNHIGHLIGHT:
			return m_colorBtnHighlight;
		case COLOR_BTNSHADOW:
			return m_colorBtnShadow;
		case COLOR_BTNTEXT:
			return m_colorBtnText;
		case COLOR_ROLLUP_TITLE_FACE:
			return m_colorRollupTitleFace;
		case COLOR_ROLLUP_TITLE_TEXT:
			return m_colorRollupTitleText;
		case COLOR_ROLLUP_TITLE_HILIGHT:
			return m_colorRollupTitleHiLight;
		case COLOR_ROLLUP_TITLE_SHADOW:
			return m_colorRollupTitleShadow;
		default:
			assert(false);
			return ::GetSysColor(index);
	}
}

void CustomControls::SetColorBrush(int index,COLORREF color)
{
	switch (index)
	{
		case COLOR_WINDOW:
			DeleteObject(m_hBrushWindow);
			m_hBrushWindow = CreateSolidBrush(color);
			break;
		case COLOR_BTNSHADOW:
			DeleteObject(m_hBrushBtnShadow);
			m_hBrushBtnShadow = CreateSolidBrush(color);
			break;
		case COLOR_BTNFACE:
			DeleteObject(m_hBrushBtnFace);
			m_hBrushBtnFace = CreateSolidBrush(color);
			break;
		case COLOR_BTNTEXT:
			DeleteObject(m_hBrushBtnText);
			m_hBrushBtnText = CreateSolidBrush(color);
			break;
		default:
			assert(false);
	}
}

void CustomControls::SetColor(int index,COLORREF color)
{
	switch (index)
	{
		case COLOR_WINDOW:
			m_colorWindow = color;
			break;
		case COLOR_WINDOWTEXT:
			m_colorWindowText = color;
			break;
		case COLOR_BTNFACE:
			m_colorBtnFace = color;
			break;
		case COLOR_BTNHIGHLIGHT:
			m_colorBtnHighlight = color;
			break;
		case COLOR_BTNSHADOW:
			m_colorBtnShadow = color;
			break;
		case COLOR_BTNTEXT:
			m_colorBtnText = color;
			break;
		case COLOR_ROLLUP_TITLE_FACE:
			m_colorRollupTitleFace = color;
			break;
		case COLOR_ROLLUP_TITLE_TEXT:
			m_colorRollupTitleText = color;
			break;
		case COLOR_ROLLUP_TITLE_HILIGHT:
			m_colorRollupTitleHiLight = color;
			break;
		case COLOR_ROLLUP_TITLE_SHADOW:
			m_colorRollupTitleShadow = color;
			break;
		default:
			assert(false);
	}
}

void CustomControls::SetEditSetFocusCallback(void (CALLBACK *editSetFocusCallback)(void))
{
	m_pEditSetFocusCallback = editSetFocusCallback;
}

void CustomControls::SetEditKillFocusCallback(void (CALLBACK *editKillFocusCallback)(void))
{
	m_pEditKillFocusCallback = editKillFocusCallback;
}

const char *CustomControls::GetWindowClassName(int index)
{
	return m_windowClassNames[index];
}

HINSTANCE CustomControls::GetHInstance(void)
{
	return m_hInstance;
}

} // AttrControls
