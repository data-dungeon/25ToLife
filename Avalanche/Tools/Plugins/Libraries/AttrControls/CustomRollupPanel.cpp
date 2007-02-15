//
//	CustomRollupPanel
//	

/*
 * $History: CustomRollupPanel.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/14/03    Time: 2:15p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * use of new window class prefixes
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/24/02   Time: 12:57p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * key val first start
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/13/02   Time: 12:15p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * +- text
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 4:04p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * Rollups in
 */

#include "stdafx.h"
#include "ICustomRollupPanel.h"
#include "CustomRollupPanel.h"
#include "CustomRollupWindow.h"
#include "CustomControls.h"
#include "WinUtil.h"

namespace AttrControls
{

#define WM_CUSTROLLUP_RECALCLAYOUT WM_USER+876

// Flags passed to AppendRollup
#define APPENDROLL_CLOSED		(1<<0)	// Starts the page out rolled up.
#define DONTAUTOCLOSE    		(1<<1)	// Dont close this rollup when doing Close All

// Rollup panel metrics
#define HORIZ_MARGIN 	4
#define TITLE_MARGIN	2
#define VERT_MARGIN		4
#define TITLE_HEIGHT	16

#define CLOSE_STEPS		30
#define OPEN_STEPS		15

#define PT_TITLEPRESS	WM_USER + 10

#define MAKEPOINT( lparam, pt ) { pt.x = (short)LOWORD(lparam); pt.y = (short)HIWORD(lparam); }

//-----------------------------------------------------------------
// Rollup Panel

BOOL ICustomRollupPanel::IsRollupPanelOpen(HWND hDlg)
{
	CustomRollupPanel *pPanel = (CustomRollupPanel*)GetWindowLongPtr(GetParent(hDlg),GWLP_USERDATA);
	if (pPanel) return pPanel->m_bOpen;
	else return FALSE;
}

INT_PTR CALLBACK RollupPanelDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	CustomRollupPanel *pPanel;
	if (msg==WM_INITDIALOG) 
	{
		pPanel = (CustomRollupPanel*)lParam;
		SetWindowLongPtr(hwndDlg,DWLP_USER,(LONG_PTR)pPanel);
		INT_PTR res = pPanel->m_panelDlgProc(hwndDlg,msg,wParam,pPanel->m_userParam);
		CustomRollupPanel *rp = (CustomRollupPanel *)GetWindowLongPtr(hwndDlg, DWLP_USER);
		// make sure developers aren't using DWLP_USER
		if (rp!=pPanel) 
		{
			MessageBox(hwndDlg,_T("Rollup Window Procs must use GWLP_USERDATA, not DWLP_USER"),_T("Plug-in Implementation Error"),MB_OK|MB_ICONEXCLAMATION);
		}
		assert(rp==pPanel); 
		return res;
	}
	else 
	{
	    if ( (pPanel = (CustomRollupPanel *)GetWindowLongPtr(hwndDlg, DWLP_USER) ) == NULL )
			return FALSE; 
	}
	switch (msg) 
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			::SetFocus(pPanel->m_pRollupWindow->GetHWnd());
		case WM_MOUSEMOVE:
			pPanel->m_pRollupWindow->DlgMouseMessage(hwndDlg,msg,wParam,lParam); 
			return FALSE;
		case WM_CTLCOLORDLG:		
			SetBkColor( (HDC)wParam, CustomControls::GetColor(COLOR_BTNFACE) );
			SetTextColor( (HDC)wParam, CustomControls::GetColor(COLOR_BTNTEXT) );
			return (LRESULT)CustomControls::GetColorBrush(COLOR_BTNFACE);
		case WM_CTLCOLORSTATIC:		
			SetBkColor( (HDC)wParam, CustomControls::GetColor(COLOR_BTNFACE) );
			SetTextColor( (HDC)wParam, CustomControls::GetColor(COLOR_BTNTEXT) );
			return (LRESULT)CustomControls::GetColorBrush(COLOR_BTNFACE);
	}
	return  pPanel->m_panelDlgProc(hwndDlg,msg,wParam,lParam);
}

CustomRollupPanel::CustomRollupPanel( HINSTANCE hInst, TCHAR *dlgTemplate, HWND hParent, DLGPROC dlgProc, LPARAM param, TCHAR *title, int index, CustomRollupWindow *rw)
{
	int width=0, height=0;
	RECT rect;
	
	m_panelDlgProc = dlgProc;
	m_userParam = param;

	m_bIn = FALSE;
	m_bLBDown = FALSE;
	m_bOpen = TRUE;
	m_pRollupWindow = rw;
	m_scrollDist = 0;
	m_flags	= 0;
	m_hInst = hInst;
	m_resID	= (DWORD_PTR) dlgTemplate;

	m_hPanel = CreateWindow( 
				CustomControls::GetWindowClassName(CustomControls::ROLLUP_PANEL_WINDOW_CLASS),
				NULL,
				WS_CHILD,
				0, 0, 0 ,0 ,
				hParent,
				NULL,
				hInst,
				NULL );	

	SetWindowLongPtr( m_hPanel, GWLP_USERDATA, (LONG_PTR)this );
			// WIN64 Cleanup: Shuler

	m_hPanelTitle = CreateWindow( 
				CustomControls::GetWindowClassName(CustomControls::ROLLUP_PANEL_TITLE_WINDOW_CLASS),
				title,
				WS_CHILD,
				0, 0, 0, 0,
				m_hPanel,
				NULL,
				hInst,
				NULL );	

	SetWindowLongPtr( m_hPanelTitle, GWLP_USERDATA, (LONG_PTR)this );
			// WIN64 Cleanup: Shuler
	
	m_hDlg = CreateDialogParam( hInst, dlgTemplate, m_hPanel, RollupPanelDlgProc, (LONG_PTR)this );	
	if (!m_hDlg)
	{
		DWORD error = GetLastError();
	}
	SetClassLongPtr( m_hDlg, GCLP_HCURSOR, 0 );

	GetWindowRect( m_hDlg, &rect );
	width  = rect.right-rect.left;
	height = rect.bottom-rect.top;

	SetWindowPos( m_hDlg, NULL, HORIZ_MARGIN, TITLE_HEIGHT, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
	MoveWindow( m_hPanelTitle, HORIZ_MARGIN + TITLE_MARGIN, 0, width - 2*TITLE_MARGIN, TITLE_HEIGHT, FALSE );
	MoveWindow( m_hPanel, 0, 0, width + 2*HORIZ_MARGIN, height + VERT_MARGIN + TITLE_HEIGHT, FALSE );
	
	m_panelHeight = height + VERT_MARGIN + TITLE_HEIGHT;
	m_dlgHeight   = height;
}

CustomRollupPanel::CustomRollupPanel( HINSTANCE hInst, DLGTEMPLATE *dlgTemplate, HWND hParent,
	DLGPROC dlgProc, LPARAM param, TCHAR *title, int index, CustomRollupWindow *rw)
{
	int width=0, height=0;
	RECT rect;
	
	m_panelDlgProc = dlgProc;
	m_userParam = param;

	m_bIn = FALSE;
	m_bLBDown = FALSE;
	m_bOpen = TRUE;
	m_pRollupWindow = rw;
	m_scrollDist = 0;
	m_flags	= 0;
	m_hInst = hInst;
	m_resID	= (DWORD_PTR) dlgTemplate;

	m_hPanel = CreateWindow( 
		CustomControls::GetWindowClassName(CustomControls::ROLLUP_PANEL_WINDOW_CLASS),
				NULL,
				WS_CHILD,
				0, 0, 0 ,0 ,
				hParent,
				NULL,
				hInst,
				NULL );	

	SetWindowLongPtr( m_hPanel, GWLP_USERDATA, (LONG_PTR)this );
			// WIN64 Cleanup: Shuler

	m_hPanelTitle = CreateWindow( 
				CustomControls::GetWindowClassName(CustomControls::ROLLUP_PANEL_TITLE_WINDOW_CLASS),
				title,
				WS_CHILD,
				0, 0, 0, 0,
				m_hPanel,
				NULL,
				hInst,
				NULL );	

	SetWindowLongPtr( m_hPanelTitle, GWLP_USERDATA, (LONG_PTR)this );
	
	m_hDlg = CreateDialogIndirectParam( hInst, dlgTemplate, m_hPanel, RollupPanelDlgProc, (LONG_PTR)this );	
	SetClassLongPtr( m_hDlg, GCLP_HCURSOR, 0 );

	GetWindowRect( m_hDlg, &rect );
	width  = rect.right-rect.left;
	height = rect.bottom-rect.top;

	MoveWindow( m_hPanel, 0, 0, width + 2*HORIZ_MARGIN, height + VERT_MARGIN + TITLE_HEIGHT, FALSE );
	MoveWindow( m_hDlg, HORIZ_MARGIN, TITLE_HEIGHT, width, height,  FALSE);
	MoveWindow( m_hPanelTitle, HORIZ_MARGIN + TITLE_MARGIN, 0, width - 2*TITLE_MARGIN, TITLE_HEIGHT, FALSE );
	
	m_panelHeight = height + VERT_MARGIN + TITLE_HEIGHT;
	m_dlgHeight   = height;
}

HINSTANCE CustomRollupPanel::GetHInst(void)
{
	return m_hInst;
}

DWORD_PTR CustomRollupPanel::GetResID(void)
{
	return m_resID;
}

HWND CustomRollupPanel::GetHWnd(void)
{
	return m_hPanel;
}

HWND CustomRollupPanel::GetTitleWnd(void)
{
	return m_hPanelTitle;
}

HWND CustomRollupPanel::GetPanelWnd(void)
{
	return m_hPanel; 
}

HWND CustomRollupPanel::GetDlgWnd(void)
{
	return m_hDlg; 
}


void CustomRollupPanel::Show(void)
{
	ShowWindow( m_hPanel, SW_SHOW );
	ShowWindow( m_hPanelTitle, SW_SHOW );
	if ( m_bOpen ) ShowWindow( m_hDlg, SW_SHOW );
}

void CustomRollupPanel::Hide(void)
{
	ShowWindow( m_hPanel, SW_HIDE );
	ShowWindow( m_hPanelTitle, SW_HIDE );
	ShowWindow( m_hDlg, SW_HIDE );
}

void CustomRollupPanel::SetPos( int x, int y , int w )
{
	RECT crect;
	GetClientRect(m_hPanel, &crect);
	MoveWindow(m_hPanel, x, y, w, crect.bottom-crect.top, TRUE);
}

int CustomRollupPanel::GetTop(void)
{
	RECT rect;
	WinUtil::GetClientRectP( m_hPanel, &rect );
	return rect.top;
}

int CustomRollupPanel::GetBottom(void)
{
	RECT rect;
	WinUtil::GetClientRectP( m_hPanel, &rect );
	return rect.bottom;
}

int CustomRollupPanel::GetHeight(void)
{
	if ( m_bOpen ) 
	{
		return m_panelHeight;
	}
	else 
	{
		return TITLE_HEIGHT;
	}
}

void CustomRollupPanel::DrawPanelInset( HDC hdc )
{
	RECT rect;

	GetClientRect( m_hPanel, &rect );
	rect.left   += HORIZ_MARGIN-2;
	rect.right  -= HORIZ_MARGIN-1;
	if ( !m_bOpen ) 
	{
		rect.top = TITLE_HEIGHT / 4;
	}
	else 
	{	   
		rect.top += TITLE_HEIGHT / 2;
	}
	rect.bottom -= VERT_MARGIN-1;

	if ( rect.bottom < rect.top+4 ) rect.bottom = rect.top+4;

	WinUtil::ShadedRect( hdc, rect );
}


void CustomRollupPanel::ExpandCollapse(void)
{
	BOOL bCollapse;
	RECT rect;
	int w, sy=0;
	
	// int h, i, dy;

	if ( m_bOpen ) 
	{
		bCollapse = TRUE;
		m_bOpen = FALSE;
	}
	else 
	{
		bCollapse = FALSE;
		m_bOpen = TRUE;
	}

	GetWindowRect( m_hPanel, &rect );
	w = rect.right-rect.left;

	
	if ( bCollapse ) 
	{
/*
		//NS: 8/2/00 removed rollup animation 

		dy = panelHeight/CLOSE_STEPS;		

		for ( i = 0; i < CLOSE_STEPS; i++ ) {
			ScrollWindow( hDlg, 0, -dy, NULL, NULL );
			sy += dy;

			h = TITLE_HEIGHT + int( (float(CLOSE_STEPS-i)/float(CLOSE_STEPS)) 
				* float( panelHeight-TITLE_HEIGHT ) );
			SetWindowPos( hPanel, NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER );

			UpdateWindow( hDlg );
			UpdateWindow( hPanel );	
			} 
*/		
		SetWindowPos( m_hPanel, NULL, 0, 0, w, TITLE_HEIGHT, SWP_NOMOVE | SWP_NOZORDER );
		//ScrollWindow( hDlg, 0, -(panelHeight-sy), NULL, NULL );
		//scrollDist = panelHeight;

		InvalidateRect( m_hPanel, NULL, TRUE );		
		if ( m_pRollupWindow ) 
		{
			m_pRollupWindow->PanelOpenClose(m_pRollupWindow->GetPanelIndex(m_hDlg),TITLE_HEIGHT-m_panelHeight,TRUE);			
			UpdateWindow( m_pRollupWindow->GetPanelWnd() );		
		}
		else 
		{
			UpdateWindow( m_hPanel );		
		}

		ShowWindow( m_hDlg, SW_HIDE );		
	}
	else 
	{
		InvalidateRect( m_hPanel, NULL, TRUE );
		if ( m_pRollupWindow ) 
		{
			m_pRollupWindow->PanelOpenClose(m_pRollupWindow->GetPanelIndex(m_hDlg),m_panelHeight-TITLE_HEIGHT,TRUE);			
			UpdateWindow( m_pRollupWindow->GetPanelWnd() );
		}
		else 
		{
			UpdateWindow( m_hPanel );		
		}
		ShowWindow( m_hDlg, SW_SHOW );
/*
		//NS: 8/2/00 removed rollup animation 

		dy = scrollDist/OPEN_STEPS;

		for ( i = 0; i < OPEN_STEPS; i++ ) {
			h = TITLE_HEIGHT + int( (float(i)/float(OPEN_STEPS)) 
				* float( panelHeight-TITLE_HEIGHT ) );
			SetWindowPos( hPanel, NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER );

			ScrollWindow( hDlg, 0, dy, NULL, NULL );
			sy += dy;

			if ( i==1 ) InvalidateRect( hPanel, NULL, TRUE );
			
			UpdateWindow( hPanel );			
			UpdateWindow( hDlg );						
			} 
		
*/		
		SetWindowPos( m_hPanel, NULL, 0, 0, w, m_panelHeight, SWP_NOMOVE | SWP_NOZORDER );
		//ScrollWindow( hDlg, 0, scrollDist-sy, NULL, NULL );

		InvalidateRect(m_hPanel,NULL,TRUE);
		UpdateWindow( m_hPanel );		
		UpdateWindow( m_hDlg );				
	}
	
	SendMessage(GetParent(m_pRollupWindow->GetHWnd()),WM_CUSTROLLUP_RECALCLAYOUT, bCollapse, (LPARAM) m_pRollupWindow);
}

void CustomRollupPanel::SetDlgHeight(int height)
{
	RECT rect;
	int width, oldh = m_panelHeight;

	if ( m_dlgHeight == height ) return;

	GetWindowRect( m_hDlg, &rect );
	width  = rect.right-rect.left;

	RECT clientRect;
	GetClientRect(m_hDlg,&clientRect);
	int clientWidth = clientRect.right-clientRect.left;
	
	m_panelHeight = height + VERT_MARGIN + TITLE_HEIGHT;
	m_dlgHeight   = height;
	WinUtil::StretchWindow( m_hDlg, width, m_dlgHeight );
	InvalidateRect( m_hDlg, NULL, TRUE );

	if ( m_bOpen ) 
	{
		WinUtil::StretchWindow( m_hPanel, width + 2*HORIZ_MARGIN, m_panelHeight );
		m_pRollupWindow->PanelOpenClose(m_pRollupWindow->GetPanelIndex(m_hDlg),m_panelHeight-oldh,FALSE);
	}
}

BOOL CustomRollupPanel::Open(void)
{
	return m_bOpen;
}

BOOL CustomRollupPanel::operator==(const ICustomRollupPanel& id)
{ 
	CustomRollupPanel &panl = (CustomRollupPanel&) id;
	return (m_hInst == panl.m_hInst && m_resID == panl.m_resID);
}

BOOL CustomRollupPanel::OnTWLButtonDown(HWND hTitle,LPARAM lParam, WPARAM fwKeys)
{
	POINT pt;
	MAKEPOINT(lParam, pt);
	m_bLBDown = TRUE;
	SetCapture(hTitle);
	::SetFocus(hTitle);
	m_bIn = ChildWindowFromPoint( GetParent(hTitle), pt ) == hTitle;
	InvalidateRect( hTitle, NULL, TRUE );			
	return FALSE;
}

BOOL CustomRollupPanel::OnTWLButtonUp(HWND hTitle,POINT pt, WPARAM fwKeys)
{
	if ( m_bLBDown ) 
	{
		m_bIn = ChildWindowFromPoint( GetParent(hTitle), pt ) == hTitle;				
		if ( m_bIn ) 
		{
			SendMessage( GetParent(hTitle), PT_TITLEPRESS, 0, 0 );
		}
		m_bLBDown = FALSE;
		m_bIn = FALSE;
		InvalidateRect( hTitle, NULL, TRUE );
		ReleaseCapture();
	}
	return FALSE;
}

BOOL CustomRollupPanel::OnTWMouseMove(HWND hTitle,LPARAM lParam, WPARAM fwKeys)
{
	static bool bBeginDrag = false;
	POINT pt;
	MAKEPOINT(lParam, pt);
	if ( m_bLBDown ) 
	{
		BOOL bPIn = m_bIn;
		
		m_bIn = ChildWindowFromPoint( GetParent(hTitle), pt ) == hTitle;
		
		if ( bPIn != m_bIn )
		{
			InvalidateRect( hTitle, NULL, TRUE );
		}
	}
	else
		bBeginDrag = true;
	return FALSE;
}


static LONG_PTR CALLBACK ScrollChildren( HWND hwnd, LPARAM lParam )
{
	RECT rect;
	WinUtil::GetClientRectP( hwnd, &rect );
	SetWindowPos( hwnd, NULL, rect.left, rect.top + int(lParam), 0, 0, SWP_NOSIZE | SWP_NOZORDER );
	return TRUE;
}

void CustomRollupPanel::SetState( BOOL bOpenState )
{
	if ( !bOpenState ) 
	{
		RECT rect;
		int w;

		m_bOpen = FALSE;

		GetWindowRect( m_hPanel, &rect );
		w = rect.right-rect.left;

		ShowWindow( m_hDlg, SW_SHOW );

		SetWindowPos( m_hPanel, NULL, 0, 0, w, TITLE_HEIGHT, SWP_NOMOVE | SWP_NOZORDER );
		//ScrollWindowEx(hDlg,0,-panelHeight,NULL,NULL,NULL,NULL,SW_SCROLLCHILDREN);
		//EnumChildWindows(hDlg,ScrollChildren,(LPARAM)(int)-panelHeight);

		if ( m_pRollupWindow ) 
		{
			m_pRollupWindow->PanelOpenClose(m_pRollupWindow->GetPanelIndex(m_hDlg),TITLE_HEIGHT-m_panelHeight,FALSE);
		}
			 
		ShowWindow( m_hDlg, SW_HIDE );		
	}
	else 
	{
		if (!m_bOpen)
			ExpandCollapse();
	}
}

HWND CustomRollupPanel::GetRollupWindowHWND(void)
{
	return m_pRollupWindow->GetPanelWnd(); 
}

void CustomRollupPanel::PaintTitle(HDC hdc)
{
	RECT rect;
	SIZE size;
	
	TCHAR *buf = new TCHAR[GetWindowTextLength(m_hPanelTitle)+1];
	
	GetWindowText( m_hPanelTitle, buf, GetWindowTextLength(m_hPanelTitle)+1 );
	GetClientRect( m_hPanelTitle, &rect );
	
	SetBkColor( hdc, CustomControls::GetColor(CustomControls::COLOR_ROLLUP_TITLE_FACE) );
	SetBkMode( hdc, TRANSPARENT );
	
	HBRUSH hbr = CreateSolidBrush(CustomControls::GetColor(CustomControls::COLOR_ROLLUP_TITLE_FACE));
	FillRect(hdc, &rect, hbr);
	DeleteObject(hbr);
	
	SetTextColor( hdc, CustomControls::GetColor(CustomControls::COLOR_ROLLUP_TITLE_TEXT) );

	// +- sign
	if ( !m_bOpen ) 
	{
		TextOut( hdc, 5, 0, _T("+"), 1 );
	}
	else 
	{
		TextOut( hdc, 6, 0, _T("-"), 1 );
	}
	
	SelectObject( hdc, CustomControls::GetFixedFont());
	GetTextExtentPoint( hdc, buf, _tcslen(buf), &size );
	
	TextOut( hdc, rect.right/2-size.cx/2, 1, buf, _tcslen(buf) );
	
	int topLeft = m_bIn ? CustomControls::GetColor(CustomControls::COLOR_ROLLUP_TITLE_SHADOW) :
		CustomControls::GetColor(CustomControls::COLOR_ROLLUP_TITLE_HILIGHT);
	int bottomRight = m_bIn ? CustomControls::GetColor(CustomControls::COLOR_ROLLUP_TITLE_HILIGHT) :
		CustomControls::GetColor(CustomControls::COLOR_ROLLUP_TITLE_SHADOW);
	
	HPEN tl = CreatePen(PS_SOLID, 0, topLeft);
	HPEN br = CreatePen(PS_SOLID, 0, bottomRight);
	
	HPEN op = (HPEN)SelectObject(hdc, tl);
	MoveToEx(hdc, rect.left, rect.bottom-1, NULL);
	LineTo(hdc, rect.left, rect.top);
	LineTo(hdc, rect.right-1, rect.top);
	SelectObject(hdc, br);
	MoveToEx(hdc, rect.right-1, rect.top, NULL);
	LineTo(hdc, rect.right-1, rect.bottom-1);
	LineTo(hdc, rect.left, rect.bottom-1);
	
	SelectObject(hdc, op);
	DeleteObject(tl);
	DeleteObject(br);
	
	delete buf; 
}

LRESULT CALLBACK RollupPanelWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	CustomRollupPanel *pPanel = (CustomRollupPanel *)GetWindowLongPtr( hwnd, GWLP_USERDATA );
	if ( !pPanel ) 
		return DefWindowProc(hwnd,message,wParam,lParam);

	switch ( message ) 
	{
		case WM_CREATE:
			break;

		case WM_DESTROY:
			SendMessage(pPanel->m_hDlg,WM_CLOSE,0,0);
		    SetWindowLongPtr(pPanel->GetDlgWnd(), DWLP_USER,0);	// clear this one too...
			SetWindowLongPtr( pPanel->GetTitleWnd(), GWLP_USERDATA, 0 );
			delete pPanel;
			SetWindowLongPtr( hwnd, GWLP_USERDATA, 0 );
			break;

		case WM_SIZE:			
			{
				RECT rect;
				GetClientRect( pPanel->GetPanelWnd(), &rect );
				int width  = (rect.right-rect.left);
				int height  = (rect.bottom-rect.top);

				MoveWindow( pPanel->m_hDlg, HORIZ_MARGIN, TITLE_HEIGHT, width - 2*HORIZ_MARGIN, height-(VERT_MARGIN +TITLE_HEIGHT),  FALSE);
				MoveWindow( pPanel->GetTitleWnd(), HORIZ_MARGIN + TITLE_MARGIN, 0, width - (2*(TITLE_MARGIN+HORIZ_MARGIN)), TITLE_HEIGHT, FALSE );
			}
			break;

		case WM_PAINT: 
			{
				PAINTSTRUCT ps;
				HDC hdc;			
				hdc = BeginPaint( hwnd, &ps );
				SelectObject(ps.hdc,CustomControls::GetColorBrush(COLOR_BTNFACE));
				SelectObject(ps.hdc,GetStockObject(NULL_PEN));
				Rectangle(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom);
				EndPaint( hwnd, &ps );
				break;
			}

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			::SetFocus(hwnd);
			//fall thru
		case 0x020A:
		case WM_MOUSEMOVE:
			pPanel->m_pRollupWindow->DlgMouseMessage(hwnd,message,wParam,lParam);
			break;
		case PT_TITLEPRESS:			
			pPanel->ExpandCollapse();
			break;
		case WM_CTLCOLORDLG:		
			SetBkColor( (HDC)wParam, CustomControls::GetColor(COLOR_BTNFACE) );
			return (LRESULT)CustomControls::GetColorBrush(COLOR_BTNFACE);
	}
	return DefWindowProc(hwnd,message,wParam,lParam);
}

LRESULT CALLBACK RollupPanelTitleWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{		
	POINT pt;
	CustomRollupPanel *pPanel = (CustomRollupPanel *)GetWindowLongPtr( hwnd, GWLP_USERDATA );
	if ( !pPanel )
		return DefWindowProc(hwnd,message,wParam,lParam);

	switch ( message ) 
	{
		case WM_CREATE:
			break;

		case WM_DESTROY:
			break;

		case WM_PAINT: 
			{
				PAINTSTRUCT ps;
				HDC hdc;			
				hdc = BeginPaint( hwnd, &ps );
				pPanel->PaintTitle(hdc);
				EndPaint( hwnd, &ps );
				break;
			}

		case WM_RBUTTONDOWN:
			::SetFocus(hwnd);
			break;

		case WM_LBUTTONDOWN:
			if(pPanel->OnTWLButtonDown(hwnd,lParam, wParam) )
				return TRUE;
			break;

		case WM_LBUTTONUP:
			MAKEPOINT(lParam, pt);
			if(pPanel->OnTWLButtonUp(hwnd,pt, wParam))
				return TRUE;
			break;

		case WM_MOUSEMOVE:
			if(pPanel->OnTWMouseMove(hwnd,lParam, wParam))
				return TRUE;
			break;

		case WM_CTLCOLORSTATIC:		
			SetBkColor( (HDC)wParam, CustomControls::GetColor(COLOR_BTNFACE) );
			return (LRESULT)CustomControls::GetColorBrush(COLOR_BTNFACE);
		}
	return DefWindowProc(hwnd,message,wParam,lParam);
}

} // AttrControls