//
//	CustomRollupWindow
//	

/*
 * $History: CustomRollupWindow.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/14/03    Time: 2:15p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * use of new window class prefixes
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/14/03    Time: 4:00p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * release build problems with using member variables as parameters in
 * constructor
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 4:04p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * Rollups in
 */

#include "stdafx.h"
#include "CustomRollupPanel.h"
#include "CustomRollupWindow.h"
#include "ICustomControl.h"
#include "CustomControls.h"
#include "CustomEditBox.h"
#include "CustomSpinner.h"
#include "WinUtil.h"

namespace AttrControls
{
#define APPENDROLL_CLOSED		(1<<0)	// Starts the page out rolled up.
#define DONTAUTOCLOSE    		(1<<1)	// Dont close this rollup when doing Close All

#define MAKEPOINT( lparam, pt ) { pt.x = (short)LOWORD(lparam); pt.y = (short)HIWORD(lparam); }

CustomRollupWindow::CustomRollupWindow( HWND hWnd )	 : CustomControl(hWnd) 
{
	RECT rect;
	int w,h;

	GetClientRect( m_hWnd, &rect );
	w =	rect.right-rect.left-1-LMARGIN-RMARGIN;
	h = rect.bottom-rect.top-BMARGIN-TMARGIN;
	w = max( w, 0 );
	h = max( h, 0 );
	m_panelWidth = w;
	m_hPanelWnd = CreateWindow( 
				CustomControls::GetWindowClassName(CustomControls::GRAY_WINDOW_CLASS),
				NULL,
				WS_CHILD | WS_VISIBLE,
				LMARGIN, TMARGIN, 
				w,
				h,
				m_hWnd,
				NULL,
				(HINSTANCE)GetWindowLongPtr(m_hWnd,GWLP_HINSTANCE),
				NULL );	
	
	
	SetWindowLongPtr( m_hPanelWnd , GWLP_USERDATA, (LONG_PTR)this );
	m_bLBDown        = FALSE;
	m_omy           = 0;
	m_virtualHeight = 0;
	m_scrollPos     = 0;
}

CustomRollupWindow::~CustomRollupWindow()
{
}

int CustomRollupWindow::AppendRollup( HINSTANCE hInst, TCHAR *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param, DWORD flags)
{
	AppendRollupPreCreate(hInst, dlgProc, title, param, flags);

	CustomRollupPanel *rp = new CustomRollupPanel( hInst, dlgTemplate, m_hPanelWnd, 
							dlgProc, param, title, m_panels.GetSize(), this);
	
	return AppendRollupPostCreate(rp, hInst, dlgProc, title, param, flags);
}


int CustomRollupWindow::AppendRollup( HINSTANCE hInst, DLGTEMPLATE *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param, DWORD flags)
{
	AppendRollupPreCreate(hInst, dlgProc, title, param, flags);

	CustomRollupPanel *pRollupPanel = new CustomRollupPanel( hInst, dlgTemplate, m_hPanelWnd, 
							dlgProc, param, title, m_panels.GetSize(), this);
	
	return AppendRollupPostCreate(pRollupPanel, hInst, dlgProc, title, param, flags);

}

int CustomRollupWindow::AppendRollupPreCreate(HINSTANCE hInst,DLGPROC dlgProc, TCHAR *title, LPARAM param,DWORD flags)
{
	return 0;
}

int CustomRollupWindow::AppendRollupPostCreate(CustomRollupPanel *rp,HINSTANCE hInst,DLGPROC dlgProc, TCHAR *title, LPARAM param,DWORD flags)
{
	if ( !rp ) return -1;
	
	rp->m_flags = flags;

	m_panels.Add(rp);
	
	RecalcLayout();
	
	UpdateVirtualHeight();

	return m_panels.GetSize()-1;
}

void CustomRollupWindow::RecalcLayout(void)
{
	int cnt = m_panels.GetSize();

	int nextheight = VIRTUAL_TMARGIN - m_scrollPos;

	for(int i = 0 ; i < cnt ; i++)
	{
		m_panels[i]->SetPos(0,nextheight,m_panelWidth);
		nextheight += (PANEL_SPACING + m_panels[i]->GetHeight());
	}

}

int CustomRollupWindow::ReplaceRollup( int index, HINSTANCE hInst, TCHAR *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param, DWORD flags)
{
	ReplaceRollupPreCreate(index, hInst, dlgProc, title, param,flags);

	CustomRollupPanel *pRollupPanel = new CustomRollupPanel( hInst, dlgTemplate, m_hPanelWnd, dlgProc, param, title, index, this);
	
	return ReplaceRollupPostCreate(pRollupPanel,index, hInst, dlgProc, title, param,flags);
	
}

int CustomRollupWindow::ReplaceRollup( int index, HINSTANCE hInst, DLGTEMPLATE *dlgTemplate, DLGPROC dlgProc, TCHAR *title, LPARAM param, DWORD flags)
{

	ReplaceRollupPreCreate(index, hInst, dlgProc, title, param,flags);

	CustomRollupPanel *pRollupPanel = new CustomRollupPanel( hInst, dlgTemplate, m_hPanelWnd,	dlgProc, param, title, index, this);
	
	return ReplaceRollupPostCreate(pRollupPanel,index, hInst, dlgProc, title, param,flags);	

}

int CustomRollupWindow::ReplaceRollupPreCreate(int index, HINSTANCE hInst, DLGPROC dlgProc, TCHAR *title, LPARAM param,DWORD flags)
{
	assert(index >= 0 && index < m_panels.GetSize());
	return 0;

}

int CustomRollupWindow::ReplaceRollupPostCreate(CustomRollupPanel *rp,int index, HINSTANCE hInst, DLGPROC dlgProc, TCHAR *title, LPARAM param,DWORD flags)
{
	rp->m_flags = flags;

	if ( !rp ) return -1;

	// Get rid of the one that was here	and remember its height
	CustomRollupPanel *drp = m_panels[index];
	int deletedHeight = drp->GetHeight();
	m_panels.RemoveAt(index);
	DestroyWindow( drp->GetPanelWnd() );
	
	m_panels.InsertAt(index,rp);	

	RecalcLayout();
	
	UpdateVirtualHeight();

	InvalidateRect(m_hPanelWnd,NULL,TRUE);	
	UpdateWindow(m_hPanelWnd);

	return index;
}

// Before destroying a rollup window, update the spinner
// with keyboard focus.
void CustomRollupWindow::UpdateFocusEdit(void)
{
	HWND hf = GetFocus();
	if (!hf) return;
	hf = GetParent(hf);
	if (!hf) return;
	hf = GetParent(hf);
	if (!hf) return;

	TCHAR buf[256];
	GetClassName(hf, buf, 256);
	if (!_tcscmp(buf, CustomControls::GetWindowClassName(CustomControls::EDIT_WINDOW_CLASS))) 
	{
		CustomEditBox *pControl = (CustomEditBox*)GetWindowLongPtr(hf,GWLP_USERDATA);
			// WIN64 Cleanup: Shuler
		if (pControl && pControl->m_pSpinner) 
		{
			pControl->m_pSpinner->UpdateFromEdit();
		}
	}
}

void CustomRollupWindow::DeleteRollup( int index, int count )
{
	if (count==0) return;
	assert( index >= 0 && index+count <= m_panels.GetSize() );
	int yoff = 0;

	for ( int i = 0; i < count; i++ ) 
	{
		CustomRollupPanel *pRollupPanel = m_panels[index];
		yoff -= pRollupPanel->GetHeight() + PANEL_SPACING;
		m_panels.RemoveAt(index);		
		DestroyWindow( pRollupPanel->GetPanelWnd() );
	}
	if ( m_panels.GetSize() > index && yoff ) 
	{
		PanelOpenClose(index-1,yoff,FALSE);
	}
	else
	{
		UpdateVirtualHeight();
	}
}

void CustomRollupWindow::SetPageDlgHeight(int index,int height)
{
	m_panels[index]->SetDlgHeight(height);
}

void CustomRollupWindow::Show(void)
{
	for ( int i = 0; i < m_panels.GetSize(); i++ ) 
	{
		m_panels[i]->Show();
	}
}

void CustomRollupWindow::Hide(void)
{
	for ( int i = 0; i < m_panels.GetSize(); i++ ) 
	{
		m_panels[i]->Hide();
	}
}

void CustomRollupWindow::Show(int index)
{
	m_panels[index]->Show();
}

void CustomRollupWindow::Hide(int index)
{
	m_panels[index]->Hide();
}

HWND CustomRollupWindow::GetPanelDlg(int index)
{
	return m_panels[index]->GetDlgWnd();
}

int CustomRollupWindow::GetPanelIndex(HWND m_hWnd)
{	
	for ( int i = 0; i < m_panels.GetSize(); i++ ) 
	{
		if ( m_panels[i] && m_panels[i]->GetDlgWnd() == m_hWnd || m_panels[i] && m_panels[i]->GetPanelWnd() == m_hWnd ) 
		{
			return i;
		}
	}
	return -1;
}

void CustomRollupWindow::SetPanelTitle(int index,TCHAR *title)
{
	SetWindowText(m_panels[index]->m_hPanelTitle,title);
	InvalidateRect(m_panels[index]->m_hPanelTitle,NULL,TRUE);
}

void CustomRollupWindow::PanelOpenClose(int index,int yoff,BOOL scrollTo)
{		
	if ( yoff < 0 ) 
	{
		for ( int i = index + 1; i < m_panels.GetSize(); i++ ) 
		{
			m_panels[i]->SetPos( 0, m_panels[i]->GetTop() + yoff, m_panelWidth);
		}
	} 
	else if ( yoff > 0 ) 
	{
		for ( int i = m_panels.GetSize()-1; i > index; i-- ) 
		{
			m_panels[i]->SetPos( 0, m_panels[i]->GetTop() + yoff, m_panelWidth);
		}
	}
	if ( yoff != 0 ) 
	{
		m_virtualHeight = CalcVirtualHeight();
		VirtualHeightChange(scrollTo ? index : -1);
	}
}

int CustomRollupWindow::CalcVirtualHeight(void)	
{
	int vh = VIRTUAL_TMARGIN + VIRTUAL_BMARGIN;
	for ( int i = 0; i < m_panels.GetSize(); i++ ) 
	{
		vh += m_panels[i]->GetHeight() + PANEL_SPACING;
	}
	return vh;
}

void CustomRollupWindow::VirtualHeightChange(int index)
{
   	int h = GetClientHeight();
	int oldPos = m_scrollPos, dy;

	if (m_virtualHeight <= h) 
	{
		m_scrollPos = 0;
		DrawScroll();			
	} 
	else 
	{
		CalcThumbSize();
		DrawScroll();
	}
	
	if (index >= 0) 
	{
		int tp = m_panels[index]->GetTop();
		int b = tp + m_panels[index]->GetHeight();
		if (b > h) 
		{
			m_scrollPos -= h-b;
			// DS 5/1/97:	keep the title from being scrolled off the top
			int a = m_panels[index]->GetHeight()-GetClientHeight();
			if (a>0)
				m_scrollPos -= a;
			if (m_scrollPos < 0) 
			{
				m_scrollPos = 0;
			}
		}
	}

	dy = m_scrollPos - oldPos;
	if (dy) 
	{
		ScrollWindow( m_hPanelWnd, 0, -dy, NULL, NULL );
		InvalidateRgn(m_hPanelWnd, NULL, TRUE);
		UpdateWindow( m_hPanelWnd );
		DrawScroll();
	}
}

int CustomRollupWindow::GetClientHeight(void)
{
	RECT rect;	
	GetClientRect( m_hWnd, &rect );
	return ((rect.bottom-rect.top)+1)-TMARGIN-BMARGIN;
}

void CustomRollupWindow::CalcThumbSize(void)
{
	int h = GetClientHeight();
	
	if ( m_scrollPos < 0 ) 
	{
		m_scrollPos = 0;
	}
	if ( m_scrollPos + h > m_virtualHeight+1 ) 
	{
		m_scrollPos = m_virtualHeight+1 - h;
		if( m_scrollPos < 0 ) 
		{
			m_scrollPos = 0;
		}
	}
}

void CustomRollupWindow::DrawScroll(void)
{
	int h = GetClientHeight();

	SCROLLINFO scrollInfo;
	scrollInfo.cbSize = sizeof(SCROLLINFO);
	scrollInfo.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
	scrollInfo.nMin = 0;
	if (m_virtualHeight <= h) 
	{
		scrollInfo.nMax = 100;
		scrollInfo.nPage = 101;
		scrollInfo.nPos = 0;
	}
	else
	{
		scrollInfo.nMax = m_virtualHeight-1;
		scrollInfo.nPage = h;
		scrollInfo.nPos = m_scrollPos;
	}
	::SetScrollInfo(m_hWnd,SB_VERT,&scrollInfo,TRUE);
}

LRESULT CustomRollupWindow::OnSize( int w, int h )
{	
	w =	w-2-LMARGIN-RMARGIN;
	h = h-1-BMARGIN-TMARGIN;
	w = max( w, 0 );
	h = max( h, 0 );
	
	SetWindowPos( m_hPanelWnd, NULL, 
		LMARGIN, TMARGIN, w, h,
		SWP_NOZORDER );	 

	m_panelWidth = w;
	UpdateLayout();
	
	if ( m_virtualHeight > 0 ) 
	{
		VirtualHeightChange();
	}
	return 0;
}

LRESULT CustomRollupWindow::OnPaint(void)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;

	hdc = BeginPaint( m_hWnd, &ps );

	GetClientRect( m_hWnd, &rect );
	rect.right--;
	rect.bottom--;
	WinUtil::Rect3D( hdc,rect, TRUE ); 		

	EndPaint( m_hWnd, &ps );
	return 0;
}

LRESULT	CustomRollupWindow::OnLButDown( int x, int y, WORD keys )
{
	m_omy    = y;
	SetCursor( CustomControls::GetPanCur() );
	m_bLBDown = TRUE;
	SetCapture( m_hWnd );
	return 0;
}

LRESULT	CustomRollupWindow::OnMButDown( int x, int y, WORD keys )
{
	return OnLButDown(x,y,keys);
}


LRESULT	CustomRollupWindow::OnMouseMove( int x, int y, WORD keys )
{
	if ( m_bLBDown ) 
	{
		int oldPos = m_scrollPos;
		int dy = y-m_omy;
		m_omy = y;

		// scroll faster if control is down
		if(GetKeyState(VK_CONTROL) & 0x8000)
		{
			dy *= 4;
		}

		if (dy)
		{
			m_scrollPos -= dy;
			CalcThumbSize();
			dy = m_scrollPos - oldPos;
			ScrollWindow( m_hPanelWnd, 0, -dy, NULL, NULL );
			UpdateWindow( m_hPanelWnd );
			DrawScroll();
		}
	} 
	else 
	{
		if ((m_virtualHeight > GetClientHeight()) )  
		{
				SetCursor( CustomControls::GetPanCur());
		} 
		else 
		{
			SetCursor( LoadCursor( NULL, IDC_ARROW ) );
		}			
	}

	return 0;
}

LRESULT	CustomRollupWindow::OnLButUp( int x, int y, WORD keys )
{
	if ( m_bLBDown ) 
	{
		ReleaseCapture();
		m_bLBDown = FALSE;
	}
	return 0;
}

LRESULT	CustomRollupWindow::OnMButUp( int x, int y, WORD keys )
{
	return OnLButUp(x,y,keys);
}

LRESULT	CustomRollupWindow::OnMouseWheel( int x, int y, int zDelta, WORD keys )
{
	int dy = zDelta/4;
	int oldPos = m_scrollPos;

	if(GetKeyState(VK_CONTROL) & 0x8000)
		dy *= 4;

	if ( !dy ) return 0;
	m_scrollPos -= dy;
	CalcThumbSize();
	dy = m_scrollPos-oldPos;
	ScrollWindow( m_hPanelWnd, 0, -dy, NULL, NULL );
	UpdateWindow( m_hPanelWnd );
	DrawScroll();

	return 0;
}

LRESULT CustomRollupWindow::OnVScroll( WORD code,int pos, HWND hwnd)
{
	int oldPos = m_scrollPos;
	int dy=0;
	int h = GetClientHeight();

	switch (code)
	{
		case SB_LINEUP:
			dy = SCROLL_LINE;
			break;
 		case SB_LINEDOWN:
			dy = -SCROLL_LINE;
			break;
		case SB_PAGEUP:
			dy = h;
			break;
		case SB_PAGEDOWN:
			dy = -h;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			dy = m_scrollPos-pos;
			break;
		case SB_TOP:
			dy = 0;
			break;
		case SB_BOTTOM:
			dy = m_scrollPos-(m_virtualHeight-h);
			break;
		case SB_ENDSCROLL:
			break;
	}
	if (dy) 
	{
		m_scrollPos -= dy;
		CalcThumbSize();
		dy = m_scrollPos - oldPos;
		ScrollWindow( m_hPanelWnd, 0, -dy, NULL, NULL );
		UpdateWindow( m_hPanelWnd );
		DrawScroll();
	}
	return 0;
}


void CustomRollupWindow::SetScrollPos(int spos) 
{ 
	int oldPos,dy;
	oldPos = m_scrollPos;
	m_scrollPos = spos;
	CalcThumbSize();
	dy = -(m_scrollPos - oldPos);
	if ( dy < 0 ) 
	{
		for ( int i = 0; i < m_panels.GetSize(); i++ ) 
		{
			m_panels[i]->SetPos( 0, m_panels[i]->GetTop() + dy, m_panelWidth );
		}
	} 
	else if ( dy > 0 ) 
	{
		for ( int i = m_panels.GetSize()-1; i >= 0; i-- ) 
		{
			m_panels[i]->SetPos( 0, m_panels[i]->GetTop() + dy, m_panelWidth );
		}
	}
	DrawScroll();
}

BOOL CustomRollupWindow::IsPanelOpen(int index)  
{
	return m_panels[index]->Open();
}

void CustomRollupWindow::SetPanelOpen(int index, BOOL isOpen, BOOL ingoreFlags )  
{
	if(isOpen)
	{
		if(!m_panels[index]->Open())
			m_panels[index]->ExpandCollapse();
	}
	else
	{
		if(m_panels[index]->Open() && !(m_panels[index]->m_flags&DONTAUTOCLOSE) )
			m_panels[index]->ExpandCollapse();
	}
}


void CustomRollupWindow::DlgMouseMessage( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	POINT pt;
	MAKEPOINT( lParam, pt );
	MapWindowPoints( hDlg, m_hWnd, &pt, 1 );
	lParam = MAKELPARAM( (WORD)pt.x, (WORD)pt.y );
	SendMessage( m_hWnd, message, wParam, lParam );
}

void CustomRollupWindow::MoveRollupPanelFrom(CustomRollupWindow *from, HWND hPanel, BOOL top)
{
	if(from == this)
		return;

	int i = from->GetPanelIndex(hPanel);
	assert(i>-1);
	if(i <= -1)
		return;

	int iNew = -1;

	if(top > m_panels.GetSize())
		top = m_panels.GetSize();
	m_panels.InsertAt(top,from->m_panels[i]);
	iNew = top;

	from->m_panels.RemoveAt(i);
	m_panels[iNew]->m_pRollupWindow = this;
	SetParent(m_panels[iNew]->m_hPanel,m_hPanelWnd);
	ShowWindow( m_panels[iNew]->m_hPanel, SW_SHOW );
	ShowWindow( m_panels[iNew]->m_hPanelTitle, SW_SHOW );

	UpdateVirtualHeight();
	from->UpdateVirtualHeight();
}

int CustomRollupWindow::GetScrollHeight(void)
{
	UpdateVirtualHeight();
	return m_virtualHeight - GetClientHeight();
}

int CustomRollupWindow::GetPanelHeight(int index)
{
	if( index < m_panels.GetSize() )
		return m_panels[index]->GetHeight();
	else
		return -1;
}

void CustomRollupWindow::UpdateLayout(void)
{
	RecalcLayout();
	InvalidateRect(m_hPanelWnd,NULL,TRUE);	
}

void CustomRollupWindow::UpdateVirtualHeight(void)
{
	m_virtualHeight = CalcVirtualHeight();
	VirtualHeightChange();
}

ICustomRollupPanel *CustomRollupWindow::GetPanel(HWND m_hWnd)
{
	int i = GetPanelIndex(m_hWnd);
	if( i >= 0)
		return (ICustomRollupPanel *) m_panels[i];
	else
		return NULL;
}

HWND CustomRollupWindow::GetPanelWnd(void)
{
	return m_hPanelWnd;
}

int CustomRollupWindow::GetNumPanels(void)
{
	return m_panels.GetSize();
}

int CustomRollupWindow::GetScrollPos(void)
{
	return m_scrollPos;
}

} // AttrControls