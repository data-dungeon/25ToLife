// This is part of the Professional User Interface Suite library.
// Copyright (C) 2001-2004 FOSS Software, Inc.
// All rights reserved.
//
// http://www.prof-uis.com
// http://www.fossware.com
// mailto:support@fossware.com
//
// This source code can be used, modified and redistributed
// under the terms of the license agreement that is included
// in the Professional User Interface Suite package.
//
// Warranties and Disclaimers:
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND
// INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
// IN NO EVENT WILL FOSS SOFTWARE INC. BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES,
// INCLUDING DAMAGES FOR LOSS OF PROFITS, LOSS OR INACCURACY OF DATA,
// INCURRED BY ANY PERSON FROM SUCH PERSON'S USAGE OF THIS SOFTWARE
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

#include "stdafx.h"
#include "ExtButton.h"

#include "ExtCmdManager.h"
#include "ExtPaintManager.h"
#include "ExtMemoryDC.h"
#include "ExtPopupMenuWnd.h"
#include "ExtColorDlg.h"
#include "ExtToolControlBar.h"

#if (!defined __AFXPRIV_H__)
	#include <AfxPriv.h>
#endif

#include <intshcut.h>

#include <../profuisdll/resource.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CExtButton::CExtButton()
	: m_nButtonType( BS_PUSHBUTTON )
	, m_nCheck( BST_UNCHECKED )
	, m_hIconIn( NULL )
	, m_hIconOut( NULL )
	, m_hCursor( NULL )
	, m_bDrawBorder( true )
	, m_bDrawFocusRect( true )
	, m_bSeparatedDropDown( false )
	, m_bAutoChangeID( false )
	, m_bAutoChangeCmdIcon( false )
	, m_bMouseOver( false )
	, m_bPushed( false )
	, m_bPushTracking( false )
	, m_bKeyTracking( false )
	, m_nTrackingHT( 0 )
	, m_nAlign( CExtPaintManager::__ALIGN_HORIZ_CENTER|CExtPaintManager::__ALIGN_VERT )
	, m_dwMenuOpt( TPMX_TOPALIGN )
	, m_nMenuLeftAreaWidth( 0 )
	, m_clrTextDisabled( ((COLORREF)-1L) )
	, m_clrTextNormal( ((COLORREF)-1L) )
	, m_clrTextHover( ((COLORREF)-1L) )
	, m_clrTextPressed( ((COLORREF)-1L) )
{
}

CExtButton::~CExtButton()
{
	_FreeWinObjects();
}

void CExtButton::_FreeWinObjects()
{
	if( m_hIconIn != NULL )
		::DestroyIcon( m_hIconIn );
	if( m_hIconOut != NULL )
		::DestroyIcon( m_hIconOut );
	if( m_hCursor != NULL )
		::DestroyCursor(m_hCursor);
}

IMPLEMENT_DYNCREATE( CExtButton, CButton );

BEGIN_MESSAGE_MAP(CExtButton, CButton)
    //{{AFX_MSG_MAP(CExtButton)
	ON_WM_CAPTURECHANGED()
	ON_WM_SETCURSOR()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(
		CExtToolControlBar::g_nMsgPutToPopupMenu,
		_OnPutToPopupMenuImpl
		)
	ON_WM_SYSCOLORCHANGE()
	__EXT_MFC_SAFE_ON_WM_SETTINGCHANGE()
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_MESSAGE(__ExtMfc_WM_THEMECHANGED, OnThemeChanged)
END_MESSAGE_MAP()

LRESULT CExtButton::_OnPutToPopupMenuImpl(WPARAM wParam, LPARAM lParam)
{
	lParam;
CExtPopupMenuWnd * pPopup = reinterpret_cast < CExtPopupMenuWnd * > (wParam);
	ASSERT_VALID( pPopup );
	return OnPutToPopupMenu( pPopup ) ? TRUE : FALSE;
}

bool CExtButton::OnPutToPopupMenu( CExtPopupMenuWnd * pPopup )
{
	ASSERT_VALID( pPopup );

//bool bInBar = false;
//CWnd * pWnd = GetParent();
//	ASSERT_VALID( pWnd );
//	if( pWnd->IsKindOf(RUNTIME_CLASS(CControlBar)) )
//	{
//		bInBar = true;
//		pWnd = pWnd->GetParentFrame();
//		ASSERT_VALID( pWnd );
//		if( pWnd->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
//		{
//			pWnd = pWnd->GetParentFrame();
//			ASSERT_VALID( pWnd );
//		}
//	}
	
	if( m_menu.GetSafeHmenu() != NULL )
	{
		ASSERT( ::IsMenu( m_menu.GetSafeHmenu() ) );
		CExtPopupMenuWnd * pPopupIns = new CExtPopupMenuWnd;
		VERIFY(
			pPopupIns->CreatePopupMenu(
				//pWnd->GetSafeHwnd()
				pPopup->GetCmdReceiverHWND()
				)
			);
		VERIFY(
			pPopupIns->UpdateFromMenu(
				pPopup->GetCmdReceiverHWND(),
				&m_menu
				)
			);
		HICON hIcon = NULL;
		if( m_hIconIn != NULL )
		{
			CExtCmdIcon _icon( m_hIconIn, true );
			hIcon = _icon.DetachAsHICON();
		}
		CString sPopupText( _T("") );
		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
				g_CmdManager->ProfileNameFromWnd( GetSafeHwnd() ),
				GetDlgCtrlID()
				);
		if( pCmdItem != NULL )
			sPopupText = pCmdItem->m_sMenuText;
		if( sPopupText.IsEmpty() )
			GetWindowText( sPopupText );
		VERIFY(
			pPopup->ItemInsertSpecPopup(
				pPopupIns,
				-1,
				(LPCTSTR)sPopupText,
				hIcon
				)
			);
		return true;
	}

	return false;
}

void CExtButton::SetIcon(int nIconInId, int nIconOutId)
{
HICON hIconIn;
HICON hIconOut;
HINSTANCE hInstResource =
		AfxFindResourceHandle(
			MAKEINTRESOURCE(nIconInId),
			RT_GROUP_ICON
			);

	hIconIn = (HICON)::LoadImage(
		hInstResource,
		MAKEINTRESOURCE(nIconInId),
		IMAGE_ICON,
		0,
		0,
		0
		);
	hIconOut =
		(nIconOutId == NULL) ?
			NULL
			:
			(HICON)::LoadImage(
				hInstResource,
				MAKEINTRESOURCE(nIconOutId),
				IMAGE_ICON,
				0,
				0,
				0
				);
	SetIcon(hIconIn, hIconOut);
}

void CExtButton::SetIcon(HICON hIconIn, HICON hIconOut)
{
	if( m_hIconIn != NULL)
		::DestroyIcon( m_hIconIn );
	if( m_hIconOut != NULL)
		::DestroyIcon( m_hIconOut );

	m_hIconIn = hIconIn;
	m_hIconOut =
		(hIconOut == NULL) ?
			m_hIconIn : hIconOut;

	if(		GetSafeHwnd() != NULL
		&&	::IsWindow( GetSafeHwnd() )
		)
	{
		Invalidate();
		//UpdateWindow();
	}
} 

HICON CExtButton::GetIconIn()
{
	return m_hIconIn;
}

HICON CExtButton::GetIconOut()
{
	return m_hIconOut;
}

BOOL CExtButton::SetBtnCursor(int nCursorId)
{
HINSTANCE hInstResource;
	if( m_hCursor != NULL )
		::DestroyCursor(m_hCursor);
	m_hCursor = NULL;
	if( nCursorId != -1 )
	{
		hInstResource =
			AfxFindResourceHandle(
				MAKEINTRESOURCE(nCursorId),
				RT_GROUP_CURSOR
				);
		m_hCursor =
			(HCURSOR)::LoadImage(
				hInstResource,
				MAKEINTRESOURCE(nCursorId),
				IMAGE_CURSOR,
				0,
				0,
				0
				);
		if( m_hCursor == NULL )
		{
			m_hCursor =
				::LoadCursor(
					NULL,
					MAKEINTRESOURCE(nCursorId)
					);
			if( m_hCursor == NULL )
				return FALSE;
		}
	}
	return TRUE;
}

void CExtButton::SetFlat( BOOL bState )
{
	if(		GetSafeHwnd() != NULL
		&&	::IsWindow( GetSafeHwnd() )
		)
	{
		ModifyStyle(
			bState ? 0 : BS_FLAT,
			bState ? BS_FLAT : 0,
			SWP_FRAMECHANGED
			);
		Invalidate();
		//UpdateWindow();
	}
}

BOOL CExtButton::GetFlat() const
{
	return ( (GetStyle()&BS_FLAT) != 0 ) ? TRUE : FALSE;
}

void CExtButton::SetAlign( int nAlign )
{
	m_nAlign = nAlign;
	
//	switch( nAlign )
//	{    
//	case CExtPaintManager::__ALIGN_HORIZ_CENTER:
//		m_nAlign = CExtPaintManager::__ALIGN_HORIZ_CENTER;
//	break;
//	case CExtPaintManager::__ALIGN_HORIZ_RIGHT:
//		m_nAlign = CExtPaintManager::__ALIGN_HORIZ_RIGHT;
//	break;
//	case CExtPaintManager::__ALIGN_VERT:
//		m_nAlign = CExtPaintManager::__ALIGN_VERT;
//	break;
//	case CExtPaintManager::__ALIGN_VERT|CExtPaintManager::__ALIGN_HORIZ_CENTER:
//		m_nAlign = CExtPaintManager::__ALIGN_VERT|CExtPaintManager::__ALIGN_HORIZ_CENTER;
//	break;
//	} // switch( nAlign )

	if(		GetSafeHwnd() != NULL
		&&	::IsWindow( GetSafeHwnd() )
		)
	{
		Invalidate();
		//UpdateWindow();
	}
}

int CExtButton::GetAlign() const
{
	return m_nAlign;
}

BOOL CExtButton::GetDrawBorder() const
{
	return m_bDrawBorder ? TRUE : FALSE;
}

void CExtButton::SetDrawBorder( BOOL bEnable )
{
	m_bDrawBorder = bEnable ? true : false;
	if(		GetSafeHwnd() != NULL
		&&	::IsWindow( GetSafeHwnd() )
		)
	{
		Invalidate();
		//UpdateWindow();
	}
}

BOOL CExtButton::GetDrawFocusRect() const
{
	return m_bDrawFocusRect ? TRUE : FALSE;
}

void CExtButton::SetDrawFocusRect( BOOL bEnable )
{
	m_bDrawFocusRect = bEnable ? true : false;
	if(		GetSafeHwnd() != NULL
		&&	::IsWindow( GetSafeHwnd() )
		)
	{
		Invalidate();
		//UpdateWindow();
	}
}

BOOL CExtButton::GetSeparatedDropDown() const
{
	return m_bSeparatedDropDown ? TRUE : FALSE;
}

void CExtButton::SetSeparatedDropDown(
	BOOL bSeparatedDropDown // = TRUE
	)
{
	m_bSeparatedDropDown = bSeparatedDropDown ? true : false;
	if(		GetSafeHwnd() != NULL
		&&	::IsWindow( GetSafeHwnd() )
		)
	{
		Invalidate();
		//UpdateWindow();
	}
}

BOOL CExtButton::GetAutoChangeID() const
{
	return m_bAutoChangeID;
}

void CExtButton::SetAutoChangeID(
	BOOL bAutoChangeID // = TRUE
	)
{
	m_bAutoChangeID = bAutoChangeID ? true : false;
	if(		GetSafeHwnd() != NULL
		&&	::IsWindow( GetSafeHwnd() )
		)
	{
		Invalidate();
		//UpdateWindow();
	}
}

BOOL CExtButton::GetAutoChangeCmdIcon() const
{
	return m_bAutoChangeCmdIcon ? TRUE : FALSE;
}

void CExtButton::SetAutoChangeCmdIcon(
	BOOL bAutoChangeCmdIcon // = TRUE
	)
{
	m_bAutoChangeCmdIcon = bAutoChangeCmdIcon ? true : false;
}

void CExtButton::_CalcClientRects(
	CRect & rcMainArea,
	CRect & rcDropDown
	) const
{
	GetClientRect( &rcMainArea );
	rcDropDown = rcMainArea;
	rcDropDown.left = rcDropDown.right;
	if( !m_bSeparatedDropDown )
		return;
	rcDropDown.left -= 
			__DROPDOWN_ARROW_GAP*2
			+ CExtPaintManager::g_glyph_btn_expand_bottom.Size().cx
			;
	rcMainArea.right = rcDropDown.left;
}

INT CExtButton::_HtMouseOnButton( const POINT & ptClient ) const // 0 - not, 1-main area, 2-dropdown area
{
CRect rcMainArea, rcDropDown;
	_CalcClientRects( rcMainArea, rcDropDown );
	if( rcMainArea.PtInRect(ptClient) )
		return 1;
	if( !m_bSeparatedDropDown )
		return 0;
	if( rcDropDown.PtInRect(ptClient) )
		return 2;
	return 0;
}

INT CExtButton::_HtMouseOnButton() const // 0 - not, 1-main area, 2-dropdown area
{
POINT point;
	if( ! ::GetCursorPos( &point ) )
		return 0;
HWND hWndUnderMouse = ::WindowFromPoint( point );
	if( hWndUnderMouse == NULL || hWndUnderMouse != m_hWnd )
		return false;
	if( !m_bSeparatedDropDown )
		return 1;
	ScreenToClient( &point );
	return _HtMouseOnButton( point );
}

void CExtButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bKeyTracking )
		return;

	CButton::OnMouseMove(nFlags, point);

INT nTrackingHT = _HtMouseOnButton();
	if(	(	((!m_bPushTracking) && nTrackingHT!=0)
		||	( m_bPushTracking && nTrackingHT==m_nTrackingHT )
		)

		&&	CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND( GetSafeHwnd() )
		)
	{ // if mouse is really on button
		if( CExtMouseCaptureSink::GetCapture() != GetSafeHwnd() )
			CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
		if( m_bMouseOver )
			return;
		m_bMouseOver = TRUE;
		m_bPushed =
			((nFlags & MK_LBUTTON) && m_bPushTracking )
				? TRUE : FALSE;
		Invalidate();
		//UpdateWindow();
	} // if mouse is really on button
	else
	{ // if mouse is really NOT on button
		if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() && ( !m_bPushTracking ) )
			CExtMouseCaptureSink::ReleaseCapture();
		if( !m_bMouseOver )
			return;
		m_bMouseOver = m_bPushed = FALSE;
		Invalidate();
		//UpdateWindow();
	} // if mouse is really NOT on button
}

void CExtButton::OnKillFocus(CWnd * pNewWnd)
{
	CButton::OnKillFocus(pNewWnd);

	if( m_bMouseOver || m_bPushed )
	{
		if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
			CExtMouseCaptureSink::ReleaseCapture();
		m_bMouseOver = m_bPushed = FALSE;
		Invalidate();
		//UpdateWindow();
	}
}

void CExtButton::OnCaptureChanged(CWnd *pWnd) 
{
	if( (m_bMouseOver || m_bPushed)
		&& pWnd != this
		)
	{
//		CExtMouseCaptureSink::ReleaseCapture();
		m_bMouseOver = m_bPushed = FALSE;
		Invalidate();
		//UpdateWindow();
	}
	CButton::OnCaptureChanged(pWnd);
}

void CExtButton::PreSubclassWindow() 
{
	CButton::PreSubclassWindow();
DWORD dwWndStyle = GetStyle();
DWORD dwWndType = dwWndStyle&0x0F;
	m_nButtonType = INT(dwWndType);
	SetButtonStyle( BS_OWNERDRAW );
	SetFont(
		(m_nButtonType == BS_DEFPUSHBUTTON)
			? (&g_PaintManager->m_FontBold)
			: (&g_PaintManager->m_FontNormal)
			,
		FALSE
		);
//int CExtPaintManager::__ALIGN_HORIZ_CENTER|CExtPaintManager::__ALIGN_VERT
	m_nAlign = CExtPaintManager::__ALIGN_VERT;
	if( (dwWndStyle&BS_LEFT) != 0 )
		m_nAlign |= 0;
	else if( (dwWndStyle&BS_RIGHT) != 0 )
		m_nAlign |= CExtPaintManager::__ALIGN_HORIZ_RIGHT;
	else if( (dwWndStyle&BS_CENTER) != 0 )
		m_nAlign |= CExtPaintManager::__ALIGN_HORIZ_CENTER;
	else if( SupportsCommand() || (dwWndStyle&BS_PUSHLIKE) != 0 )
		m_nAlign |= CExtPaintManager::__ALIGN_HORIZ_CENTER;
		
	PostMessage( // delayed repainting
		WM_ENABLE,
		(WPARAM)IsWindowEnabled()
		);
}

BOOL CExtButton::PreTranslateMessage(MSG* pMsg) 
{
	if( !CExtPopupMenuWnd::IsMenuTracking() )
	{
		InitToolTip();
		m_wndToolTip.RelayEvent(pMsg);
	}

//	if( CExtPopupMenuWnd::IsMenuTracking() )
//		return TRUE;
	if(		(	pMsg->message == WM_KEYDOWN
				|| pMsg->message == WM_KEYUP
			)
		&&
			(	pMsg->wParam == VK_RETURN
				|| pMsg->wParam == VK_SPACE
			)
		)
	{
		if( IsWindowEnabled() && IsWindowVisible() && (!m_bPushTracking) )
		{
			if(	(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
				||
				(pMsg->message == WM_KEYUP && pMsg->wParam == VK_SPACE)
				)
			{
				if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
					CExtMouseCaptureSink::ReleaseCapture();
				m_bMouseOver = m_bPushed = m_bPushTracking = m_bKeyTracking = FALSE;
				Invalidate();
				//UpdateWindow();
				_OnClick( true, false );
				return TRUE;
			}
			if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_SPACE)
			{
				m_bMouseOver = m_bPushed = m_bKeyTracking = TRUE;
				CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
				Invalidate();
				//UpdateWindow();
				return TRUE;
			}
		} // if( IsWindowEnabled() && IsWindowVisible() && (!m_bPushTracking) )
		return TRUE;
	}

	return CButton::PreTranslateMessage(pMsg);
}

LRESULT CExtButton::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
bool bInvalidate = false;
	switch( message )
	{
	case WM_LBUTTONDBLCLK:
		message = WM_LBUTTONDOWN;
	break;
	case BM_SETCHECK:
	case BM_SETSTATE:
	case BM_SETSTYLE:
#if (WINVER >= 0x0400)
	case BM_CLICK:
	case BM_SETIMAGE:
#endif
	case WM_SETTEXT:
	case WM_SIZE:
	case WM_WINDOWPOSCHANGED:
		bInvalidate = true;
	break;
	} // switch( message )

LRESULT lResult =
		CButton::DefWindowProc(message, wParam, lParam);
	if( bInvalidate )
	{
		Invalidate();
		//UpdateWindow();
	}
	return lResult;
}

BOOL CExtButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if( m_hCursor != NULL )
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}
	return CButton::OnSetCursor(pWnd, nHitTest, message);
}

void CExtButton::SetTooltipText(CExtSafeString* spText, BOOL bActivate)
{
	if( spText == NULL )
		return;
	InitToolTip();
	if( m_wndToolTip.GetToolCount() == 0 )
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_wndToolTip.AddTool( this, *spText, rectBtn, 1 );
	}
	m_wndToolTip.UpdateTipText( *spText, this, 1 );
	m_wndToolTip.Activate(bActivate);
}

void CExtButton::SetTooltipText(CExtSafeString & sText, BOOL bActivate)
{
	if( sText.IsEmpty() )
		return;
	InitToolTip();
	if( m_wndToolTip.GetToolCount() == 0 )
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_wndToolTip.AddTool( this, sText, rectBtn, 1 );
	}
	m_wndToolTip.UpdateTipText( sText, this, 1 );
	m_wndToolTip.Activate( bActivate );
}

void CExtButton::SetTooltipText(__EXT_MFC_SAFE_LPCTSTR sText, BOOL bActivate)
{
	if( sText == NULL
		|| _tcslen( sText ) == 0
		)
		return;
	InitToolTip();
	if( m_wndToolTip.GetToolCount() == 0 )
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_wndToolTip.AddTool(this,sText,rectBtn,1);
	}
	m_wndToolTip.UpdateTipText(sText,this,1);
	m_wndToolTip.Activate(bActivate);
}


void CExtButton::SetTooltipText(int nId, BOOL bActivate)
{
CExtSafeString sText;
	g_ResourceManager->LoadString( sText, nId );
	if( ! sText.IsEmpty() )
		SetTooltipText( &sText, bActivate );
}

void CExtButton::ActivateTooltip(BOOL bActivate)
{
	if( m_wndToolTip.GetToolCount() == 0 )
		return;
	m_wndToolTip.Activate(bActivate);
}

BOOL CExtButton::GetDefault() const
{
	return (m_nButtonType == BS_DEFPUSHBUTTON) ? TRUE : FALSE;
}

BOOL CExtButton::SupportsCommand() const
{
	if(		m_nButtonType == BS_PUSHBUTTON
		||	m_nButtonType == BS_DEFPUSHBUTTON
		||	m_nButtonType == BS_USERBUTTON
		||	m_nButtonType == BS_OWNERDRAW
		)
		return TRUE;
	return FALSE;
}

BOOL CExtButton::SupportsCheck() const
{
	if(		m_nButtonType == BS_CHECKBOX
		||	m_nButtonType == BS_AUTOCHECKBOX
		||	m_nButtonType == BS_3STATE
		||	m_nButtonType == BS_AUTO3STATE
		||	m_nButtonType == BS_RADIOBUTTON
		||	m_nButtonType == BS_AUTORADIOBUTTON 
		)
		return TRUE;
	return FALSE;
}

INT CExtButton::GetType() const
{
	return m_nButtonType;
}

BOOL CExtButton::Is3StateType() const
{
BOOL bIs3State =
		(GetType() == BS_AUTO3STATE) || (GetType() == BS_3STATE)
			? TRUE : FALSE;
	return bIs3State;
}

void CExtButton::InitToolTip()
{
	if( m_wndToolTip.m_hWnd == NULL )
	{
		m_wndToolTip.Create(this);
		m_wndToolTip.Activate(FALSE);
	}
}

void CExtButton::OnSysColorChange() 
{
	CButton::OnSysColorChange();
	g_PaintManager.OnSysColorChange( this );
	g_CmdManager.OnSysColorChange( this );
}

void CExtButton::OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection) 
{
	CButton::OnSettingChange( uFlags, lpszSection );
	g_PaintManager.OnSettingChange( this, uFlags, lpszSection );
	g_CmdManager.OnSettingChange( this, uFlags, lpszSection );
}

LRESULT CExtButton::OnDisplayChange( WPARAM wParam, LPARAM lParam )
{
LRESULT lResult = CButton::OnDisplayChange( wParam, lParam );
	g_PaintManager.OnDisplayChange( this, (INT)wParam, CPoint(lParam) );
	g_CmdManager.OnDisplayChange( this, (INT)wParam, CPoint(lParam) );
	return lResult;
}

LRESULT CExtButton::OnThemeChanged( WPARAM wParam, LPARAM lParam )
{
LRESULT lResult = Default();
	g_PaintManager.OnThemeChanged( this, wParam, lParam );
	g_CmdManager.OnThemeChanged( this, wParam, lParam );
	return lResult;
}

void CExtButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( m_bKeyTracking )
		return;

	CButton::OnLButtonDown( nFlags, point );

	m_nTrackingHT = _HtMouseOnButton();
	if( m_nTrackingHT == 0 )
		return;
	m_bMouseOver = m_bPushed = m_bPushTracking = TRUE;
	CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
	
	Invalidate();
	//UpdateWindow();
}

void CExtButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	nFlags;
	point;

	if( m_bKeyTracking )
		return;

INT nTrackingHT = _HtMouseOnButton();
BOOL bWasPushed = m_bPushed && m_bPushTracking && (nTrackingHT == m_nTrackingHT);
	m_bMouseOver = m_bPushed = m_bPushTracking = false;
	m_nTrackingHT = 0;

	if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
		CExtMouseCaptureSink::ReleaseCapture();
	Invalidate();
	//UpdateWindow();

	if( !bWasPushed )
		return;

	//CButton::OnLButtonDown(nFlags, point);
	if( IsWindowEnabled()
		&& IsWindowVisible()
		)
		_OnClick(
			false,
			(m_bSeparatedDropDown && nTrackingHT == 2) ? true : false
			);
}

void CExtButton::_OnClick(
	bool bSelectAny,
	bool bSeparatedDropDownClicked
	)
{
CWnd * pWnd = GetParent();
	if( pWnd->GetSafeHwnd() == NULL )
		return;
	ASSERT_VALID( pWnd );
	
	if( SupportsCheck() )
	{
		SetCheck( (GetCheck() == 0) ? 1 : 0 );
		HWND hWndThis = GetSafeHwnd();
		int nDlgCtrlID = GetDlgCtrlID();
		pWnd->SendMessage(
			WM_COMMAND,
			MAKEWPARAM(nDlgCtrlID,BN_CLICKED),
			(LPARAM)m_hWnd
			);
		if( ::IsWindow(hWndThis) )
		{
			Invalidate();
			//UpdateWindow();
		}
		return;
	} // if( SupportsCheck() )

	if( !SupportsCommand() )
		return;

	if(		(!m_bSeparatedDropDown)
		||	bSeparatedDropDownClicked
		)
	{
		if( _OnTrackPopup( bSelectAny ) )
			return;
	}
	if(		m_menu.GetSafeHmenu() == NULL
		|| (m_bSeparatedDropDown && (!bSeparatedDropDownClicked))
		)
	{
		HWND hWndThis = GetSafeHwnd();
		int nDlgCtrlID = GetDlgCtrlID();
		pWnd->SendMessage(
			WM_COMMAND,
			MAKEWPARAM(nDlgCtrlID,BN_CLICKED),
			(LPARAM)m_hWnd
			);
		if( ::IsWindow(hWndThis) )
		{
			Invalidate();
			//UpdateWindow();
		}
		return;
	}
}

bool CExtButton::_IsMenuAvail()
{
	if( m_menu.GetSafeHmenu() == NULL )
		return false;
	return true;
}

bool CExtButton::_OnTrackPopup(
	bool bSelectAny
	)
{
	if( m_menu.GetSafeHmenu() == NULL )
		return false;

	ASSERT( ::IsMenu(m_menu.GetSafeHmenu()) );
	VERIFY(
		g_CmdManager->UpdateFromMenu(
			g_CmdManager->ProfileNameFromWnd( GetSafeHwnd() ),
			m_menu.GetSafeHmenu()
			)
		);
CExtPopupMenuWnd * pPopup = new CExtPopupMenuWnd;
	pPopup->SetLeftAreaWidth( m_nMenuLeftAreaWidth );
CWnd * pWnd = GetParent();
	ASSERT_VALID( pWnd );
DWORD dwAlignOpt = 0;
DWORD dwMenuOpt = m_dwMenuOpt;
	while( pWnd->IsKindOf(RUNTIME_CLASS(CControlBar)) )
	{
		if( dwAlignOpt == 0 && ((CControlBar *)pWnd)->m_pDockBar != NULL )
		{
			ASSERT_VALID( ((CControlBar *)pWnd)->m_pDockBar );
			dwMenuOpt &= ~TPMX_ALIGN_MASK;
			UINT nDockBarID = ((CControlBar *)pWnd)->m_pDockBar->GetDlgCtrlID();
			switch( nDockBarID )
			{
			case AFX_IDW_DOCKBAR_TOP:
				dwAlignOpt = TPMX_TOPALIGN;
				break;
			case AFX_IDW_DOCKBAR_BOTTOM:
				dwAlignOpt = TPMX_BOTTOMALIGN;
				break;
			case AFX_IDW_DOCKBAR_LEFT:
				dwAlignOpt = TPMX_LEFTALIGN;
				break;
			case AFX_IDW_DOCKBAR_RIGHT:
				dwAlignOpt = TPMX_RIGHTALIGN;
				break;
			case AFX_IDW_DOCKBAR_FLOAT:
				dwAlignOpt = TPMX_TOPALIGN;
				break;
#ifdef _DEBUG
			default:
				ASSERT( FALSE );
				break;
#endif // _DEBUG
			} // switch( nDockBarID )
		} // if( dwAlignOpt == 0 && ((CControlBar *)pWnd)->m_pDockBar != NULL )
		pWnd = pWnd->GetParent();
		if( pWnd->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
		{
			pWnd = pWnd->GetParentFrame();
			ASSERT_VALID( pWnd );
			break;
		}
	}
	ASSERT( pWnd != NULL );
	ASSERT_VALID( pWnd );
	VERIFY(
		pPopup->UpdateFromMenu( pWnd->GetSafeHwnd(), &m_menu )
		);
CRect wr;
	GetWindowRect( &wr );
CPoint pt = wr.CenterPoint();
	SetFocus();

	if( m_bAutoChangeID )
	{
		UINT nResultCmdID = 0;
		if( !pPopup->TrackPopupMenu(
				dwMenuOpt
					| dwAlignOpt
					| ( (bSelectAny) ? TPMX_SELECT_ANY : 0 )
					| TPMX_DO_MESSAGE_LOOP | TPMX_NO_WM_COMMAND
					| TPMX_COMBINE_DEFAULT,
				pt.x,
				pt.y,
				&wr,
				this,
				_CbPaintCombinedContent,
				& nResultCmdID,
				true
				)
			)
		{
			delete pPopup;
			ASSERT( FALSE );
		}
		else
		{
			if( nResultCmdID != 0 )
			{
				SetDlgCtrlID( nResultCmdID );
				if( m_bAutoChangeCmdIcon )
				{
					__EXT_MFC_SAFE_LPCTSTR sProfileName =
						g_CmdManager->ProfileNameFromWnd(m_hWnd);
					if( sProfileName != NULL )
					{
						CExtCmdItem * pCmdItem =
							g_CmdManager->CmdGetPtr(
								sProfileName,
								nResultCmdID
								);
						if( pCmdItem != NULL )
						{
							CExtSafeString sText =
								pCmdItem->m_sToolbarText.IsEmpty()
									? pCmdItem->m_sMenuText
									: pCmdItem->m_sToolbarText
									;
							SetWindowText( sText );
							HICON hIcon = 
								g_CmdManager->CmdGetHICON(
									sProfileName,
									nResultCmdID
									);
							if( hIcon == NULL )
								SetIcon( NULL );
							{
								CExtCmdIcon _icon( hIcon, true );
								SetIcon( _icon.DetachAsHICON() );
							}
						}
					}
				}
				CWnd * pWnd = GetParent();
				ASSERT( pWnd != NULL );
				ASSERT_VALID( pWnd );
				HWND hWndThis = GetSafeHwnd();
				pWnd->SendMessage(
					WM_COMMAND,
					MAKEWPARAM(nResultCmdID,BN_CLICKED),
					(LPARAM)m_hWnd
					);
				if( ::IsWindow(hWndThis) )
				{
					Invalidate();
					//UpdateWindow();
				}
			}
		}
	} // if( m_bAutoChangeID )
	else
	{
		if( !pPopup->TrackPopupMenu(
				dwMenuOpt
					| dwAlignOpt
					| ( (bSelectAny) ? TPMX_SELECT_ANY : 0 )
					//| TPMX_DO_MESSAGE_LOOP
					| TPMX_COMBINE_DEFAULT,
				pt.x,
				pt.y,
				&wr,
				this,
				_CbPaintCombinedContent,
				NULL,
				true
				)
			)
		{
			delete pPopup;
			ASSERT( FALSE );
		}
	} // else from if( m_bAutoChangeID )

	return true;
}

void CExtButton::_CbPaintCombinedContent(
	LPVOID pCookie,
	CDC & dc,
	const CWnd & refWndMenu,
	const CRect & rcExcludeArea, // in screen coords
	int eCombineAlign // CExtPopupMenuWnd::e_combine_align_t values
	)
{
	pCookie;
	dc;
	refWndMenu;
	rcExcludeArea;
	eCombineAlign;
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( refWndMenu.GetSafeHwnd() != NULL );

CExtButton * pBtn = (CExtButton *)pCookie;
	ASSERT( pBtn != NULL );
	ASSERT_VALID( pBtn );
	ASSERT( pBtn->IsKindOf(RUNTIME_CLASS(CExtButton)) );
	
	ASSERT( eCombineAlign != CExtPopupMenuWnd::__CMBA_NONE );

	if( rcExcludeArea.IsRectEmpty() )
		return;

CRect rcClientBar,rcClientExcludeArea;
	pBtn->GetClientRect( &rcClientBar );
	pBtn->ClientToScreen( &rcClientBar );
	refWndMenu.GetClientRect( &rcClientExcludeArea );
	refWndMenu.ClientToScreen( &rcClientExcludeArea );
CPoint ptOffset =
		rcClientBar.TopLeft()
		- rcClientExcludeArea.TopLeft()
		;
CPoint ptViewportOrg = dc.GetViewportOrg();
	dc.SetViewportOrg( ptOffset );
	pBtn->_RenderImpl(dc,true);
	dc.SetViewportOrg( ptViewportOrg );
}

void CExtButton::_RenderImpl(
	CDC & dc,
	bool bTransparent // = false
	)
{
	ASSERT_VALID( this );
CRect rectClient;
	GetClientRect( &rectClient );
CString sWindowText( _T("") );
	GetWindowText( sWindowText );
bool bPB = true;
bool bDrawFocusRect = (GetFocus() == this && m_bDrawFocusRect) ? true : false;
bool bDefault = GetDefault() ? true : false;
bool bPushed = m_bPushed ? true : false;

//bool bEnabled = IsWindowEnabled() ? true : false;
bool bEnabled = OnQueryWindowEnabledState();

bool bFlat = GetFlat() ? true : false;
bool bHover =
		( m_bMouseOver
		&& !CExtPopupMenuWnd::IsMenuTracking() )
			? true : false;
HICON hIcon = (bHover) ? m_hIconIn : m_hIconOut;
CExtCmdIcon icon( hIcon, false );
bool bDetachIcon = true;
bool bSupportsCheck = SupportsCheck() ? true : false;
bool bChecked = false;
	if( bSupportsCheck )
	{
		bChecked = (GetCheck() == 0) ? false : true;
		if( (GetStyle()&BS_PUSHLIKE) == 0 )
			bPB = false;
		if( ! bPB )
		{
			bDrawFocusRect = false;
			if( icon.IsEmpty() || bDetachIcon )
			{
				if( bDetachIcon )
					icon.DetachAsHICON();
				if( bChecked )
				{
					if(		m_nButtonType == BS_RADIOBUTTON
						||	m_nButtonType == BS_AUTORADIOBUTTON 
						)
					{
						VERIFY( icon.CreateRadioMarkIcon( COLORREF(-1), COLORREF(-1), CSize(24,16) ) );
						bDetachIcon = false;
					}
					else
					{
						VERIFY( icon.CreateCheckMarkIcon( COLORREF(-1), COLORREF(-1), CSize(24,16) ) );
						bDetachIcon = false;
					}
				} // if( bChecked )
				else
				{
						VERIFY( icon.CreateEmptyIcon( COLORREF(-1), CSize(24,16) ) );
						bDetachIcon = false;
				} // else from if( bChecked )
			} // if( icon.IsEmpty() || bDetachIcon )
			else if( bChecked )
				bPushed = true;
			bFlat = true;
		} // if( ! bPB )
		else
		{
			if( bChecked )
				bPushed = true;
		} // if( ! bPB )
	} // if( bSupportsCheck )
	if( GetParent()->IsKindOf(RUNTIME_CLASS(CControlBar)) )
	{
		bDrawFocusRect = false;
		bFlat = true;
	}
	if(		(! bTransparent )
		&&	(! (bHover || bPushed) )
		&&	g_PaintManager->GetCb2DbTransparentMode(this)
		)
	{
		CExtPaintManager::stat_ExcludeChildAreas(
			dc,
			GetSafeHwnd(),
			CExtPaintManager::stat_DefExcludeChildAreaCallback
			);
		if( g_PaintManager->PaintDockerBkgnd( dc, this ) )
			bTransparent = true;
	}
	if( !bTransparent )
		dc.FillSolidRect( 
		&rectClient, 
		g_PaintManager->GetColor( CExtPaintManager::CLR_3DFACE_OUT) 
		);
	
CExtPaintManager::PAINTPUSHBUTTONDATA _ppbd(
		this,										   
		true, rectClient, (LPCTSTR)sWindowText,
		&icon, bFlat, bHover, bPushed,
		false, bEnabled,
		m_bDrawBorder ? true : false,
		bDrawFocusRect, bDefault, m_nAlign,
		OnQueryFont(), 
//		NULL,
		_IsMenuAvail(), 0, bTransparent
		);
	_ppbd.m_bSeparatedDropDown = m_bSeparatedDropDown ? true : false;
	if( _ppbd.m_bSeparatedDropDown
		&& m_nTrackingHT == 2
		&& _ppbd.m_bPushed
		)
	{
		_ppbd.m_bPushed = false;
		_ppbd.m_bPushedDropDown = true;
//		_ppbd.m_bHover = false;
	}

	_ppbd.m_clrForceTextDisabled = GetTextColorDisabled();
	_ppbd.m_clrForceTextNormal = GetTextColorNormal();
	_ppbd.m_clrForceTextHover = GetTextColorHover();
	_ppbd.m_clrForceTextPressed = GetTextColorPressed();

	g_PaintManager->PaintPushButton( dc, _ppbd );
	if( bDetachIcon )
		icon.DetachAsHICON();
}

void CExtButton::_DrawImpl(
	CDC & dc,
	bool bTransparent // = false
	)
{
CRect rectClient;
	GetClientRect( &rectClient );
CExtMemoryDC dcmm( &dc, &rectClient );
	_RenderImpl(dcmm,bTransparent);
}

void CExtButton::OnPaint()
{
DWORD dwWndStyle = GetStyle();
DWORD dwWndType = dwWndStyle&0x0F;
bool bOwnerDraw =
		(dwWndType == BS_OWNERDRAW || dwWndType == BS_USERBUTTON)
			? true : false;
	if( bOwnerDraw )
	{
		CButton::OnPaint();
		return;
	} // if( bOwnerDraw )
CPaintDC dcPaint( this );
	_DrawImpl( dcPaint );
}

BOOL CExtButton::OnEraseBkgnd(CDC* pDC) 
{
//DWORD dwWndStyle = GetStyle();
//DWORD dwWndType = dwWndStyle&0x0F;
//bool bOwnerDraw =
//		(dwWndType == BS_OWNERDRAW || dwWndType == BS_USERBUTTON)
//			? true : false;
//	if( bOwnerDraw )
//		return CButton::OnEraseBkgnd(pDC);
//	return FALSE;
	pDC;
	return TRUE;
}

void CExtButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT( lpDrawItemStruct != NULL );
	ASSERT( lpDrawItemStruct->hDC != NULL );
	_DrawImpl( * CDC::FromHandle(lpDrawItemStruct->hDC) );
}

void CExtButton::OnEnable(BOOL bEnable) 
{
	CButton::OnEnable(bEnable);
CClientDC dc(this);
	_DrawImpl( dc );
}

CExtColorButton::CExtColorButton()
{
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	m_bUseStdColorDlg = false;
#else
	m_bUseStdColorDlg = true;
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	m_bEnableBtnColorDefault = true;
	m_bEnableBtnColorCustom = true;
	m_clrDefault = (COLORREF)(-1); // unexisting
	m_clrSelected = RGB(0,0,0);
	m_lParamCookie = 0;

	m_sBtnTextColorDefault = _T("");
	m_sBtnTextColorCustom = _T("");
}

CExtColorButton::~CExtColorButton()
{
	_FreeWinObjects();
}

IMPLEMENT_DYNCREATE(CExtColorButton, CExtButton);

BEGIN_MESSAGE_MAP(CExtColorButton, CExtButton)
    //{{AFX_MSG_MAP(CExtColorButton)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(
		CExtPopupColorMenuWnd::g_nMsgNotifyColorChanged,
		OnColorChanged
		)
	ON_REGISTERED_MESSAGE(
		CExtPopupColorMenuWnd::g_nMsgNotifyColorChangedFinally,
		OnColorChangedFinally
		)
	ON_REGISTERED_MESSAGE(
		CExtPopupColorMenuWnd::g_nMsgNotifyCustColor,
		OnColorSelectCustom
		)
END_MESSAGE_MAP()

bool CExtColorButton::_IsMenuAvail()
{
	return true;
}

bool CExtColorButton::_OnTrackPopup(
	bool bSelectAny
	)
{

bool bInBar = false;
CWnd * pWnd = GetParent();
	ASSERT_VALID( pWnd );
DWORD dwAlignOpt = 0;
DWORD dwMenuOpt = m_dwMenuOpt;
	while( pWnd->IsKindOf(RUNTIME_CLASS(CControlBar)) )
	{
		bInBar = true;
		if( dwAlignOpt == 0 && ((CControlBar *)pWnd)->m_pDockBar != NULL )
		{
			ASSERT_VALID( ((CControlBar *)pWnd)->m_pDockBar );
			dwMenuOpt &= ~TPMX_ALIGN_MASK;
			UINT nDockBarID = ((CControlBar *)pWnd)->m_pDockBar->GetDlgCtrlID();
			switch( nDockBarID )
			{
			case AFX_IDW_DOCKBAR_TOP:
				dwAlignOpt = TPMX_TOPALIGN;
				break;
			case AFX_IDW_DOCKBAR_BOTTOM:
				dwAlignOpt = TPMX_BOTTOMALIGN;
				break;
			case AFX_IDW_DOCKBAR_LEFT:
				dwAlignOpt = TPMX_LEFTALIGN;
				break;
			case AFX_IDW_DOCKBAR_RIGHT:
				dwAlignOpt = TPMX_RIGHTALIGN;
				break;
			case AFX_IDW_DOCKBAR_FLOAT:
				dwAlignOpt = TPMX_TOPALIGN;
				break;
#ifdef _DEBUG
			default:
				ASSERT( FALSE );
				break;
#endif // _DEBUG
			} // switch( nDockBarID )
		} // if( dwAlignOpt == 0 && ((CControlBar *)pWnd)->m_pDockBar != NULL )
		pWnd = pWnd->GetParent();
		if( pWnd->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
		{
			pWnd = pWnd->GetParentFrame();
			ASSERT_VALID( pWnd );
			break;
		}
	}

//	if( ! (	CExtPopupMenuSite ::g_DefPopupMenuSite.IsEmpty()
//			|| CExtPopupMenuSite ::g_DefPopupMenuSite.IsShutdownMode()
//			)
//		)
//	{
//		CExtPopupMenuWnd * pPopupCurr =
//			CExtPopupMenuSite ::g_DefPopupMenuSite.GetInstance();
//		ASSERT_VALID( pPopupCurr );
//		if(		pPopupCurr->GetSafeHwnd() != NULL
//			&&	( ::IsWindow(pPopupCurr->GetSafeHwnd()) )
//			&&	pPopupCurr->GetParent()->GetSafeHwnd() ==
//					( bInBar ? pWnd->GetSafeHwnd() : GetSafeHwnd() )
//			)
//			return true;
//	}

CExtPopupColorMenuWnd * pPopup = new CExtPopupColorMenuWnd;
	pPopup->SetLeftAreaWidth( m_nMenuLeftAreaWidth );
	pPopup->m_lParamCookie = m_lParamCookie;

	VERIFY(
		pPopup->CreatePopupMenu(
			bInBar ? pWnd->GetSafeHwnd() : GetSafeHwnd()
			)
		);
	pPopup->m_bEnableBtnColorDefault =
		m_bEnableBtnColorDefault;
	pPopup->m_bEnableBtnColorCustom =
		m_bEnableBtnColorCustom;
	pPopup->m_clrDefault =
		m_clrDefault;
	pPopup->m_clrInitial =
		m_clrSelected;
	if( !m_sBtnTextColorDefault.IsEmpty() )
		pPopup->m_sBtnTextColorDefault = m_sBtnTextColorDefault;
	if( !m_sBtnTextColorCustom.IsEmpty() )
		pPopup->m_sBtnTextColorCustom = m_sBtnTextColorCustom;

CRect wr;
	GetWindowRect( &wr );
CPoint pt = wr.CenterPoint();
	if( bInBar)
		pWnd->SetFocus();
	else
		SetFocus();
bool bToolMenuTracking = false;
	if( bInBar )
	{
		CExtToolControlBar::_CloseTrackingMenus();
		bToolMenuTracking = true;
	}
	if( !pPopup->TrackPopupMenu(
			dwMenuOpt
				| dwAlignOpt
				| ( (bSelectAny) ? TPMX_SELECT_ANY : 0 )
				//| TPMX_DO_MESSAGE_LOOP
				| TPMX_COMBINE_DEFAULT
				,
			pt.x,
			pt.y,
			&wr,
			this,
			_CbPaintCombinedContent,
			NULL,
			true
			)
		)
	{
		delete pPopup;
		ASSERT( FALSE );
	}
	else
	{
		if( bToolMenuTracking )
			CExtToolControlBar::g_bMenuTracking = true;
	}
	return true;
}

void CExtColorButton::OnGenerateColorBtnIcon(
	HICON * p_hIcon,
	bool * p_bDestroyIcon
	)
{
	ASSERT( p_hIcon != NULL );
	ASSERT( p_bDestroyIcon != NULL );
	*p_bDestroyIcon = false;
	*p_hIcon =
		CExtPaintManager::stat_GenerateColorIconBox(
			m_clrSelected
			);
	ASSERT( *p_hIcon != NULL );
	if( *p_hIcon != NULL )
		*p_bDestroyIcon = true;
}

void CExtColorButton::_RenderImpl( // non-buffered paint
	CDC & dc,
	bool bTransparent // = false
	)
{
HICON hIcon = NULL;
bool bDestroyIcon = false;
	OnGenerateColorBtnIcon( &hIcon, &bDestroyIcon );

CRect rectClient;
	GetClientRect( &rectClient );
CString sWindowText( _T("") );
	GetWindowText( sWindowText );
bool bDrawFocusRect = (GetFocus() == this && m_bDrawFocusRect) ? true : false;
bool bDefault = GetDefault() ? true : false;
bool bPushed = m_bPushed;

//bool bEnabled = IsWindowEnabled() ? true : false;
bool bEnabled = OnQueryWindowEnabledState();

bool bFlat = GetFlat() ? true : false;
bool bHover =
		( m_bMouseOver
		&& !CExtPopupMenuWnd::IsMenuTracking() )
			? true : false;
	if( GetParent()->IsKindOf(RUNTIME_CLASS(CControlBar)) )
	{
		bDrawFocusRect = false;
		bFlat = true;
	}
	if(		(! bTransparent )
		&&	(! (bHover || bPushed) )
		&&	g_PaintManager->GetCb2DbTransparentMode(this)
		)
	{
		CExtPaintManager::stat_ExcludeChildAreas(
			dc,
			GetSafeHwnd(),
			CExtPaintManager::stat_DefExcludeChildAreaCallback
			);
		if( g_PaintManager->PaintDockerBkgnd( dc, this ) )
			bTransparent = true;
	}
CExtCmdIcon icon( hIcon, false );
CExtPaintManager::PAINTPUSHBUTTONDATA _ppbd(
		this,
		true, rectClient, (LPCTSTR)sWindowText,
		&icon, bFlat, bHover, bPushed,
		false, bEnabled,
		(m_bDrawBorder && (!CExtPopupMenuWnd::IsMenuTracking()))
			? true : false,
		(bDrawFocusRect && (!CExtPopupMenuWnd::IsMenuTracking())
			&& (!sWindowText.IsEmpty()) )
			? true : false,
		bDefault, m_nAlign,
		NULL, _IsMenuAvail(), 0, bTransparent
		);
	_ppbd.m_bSeparatedDropDown = m_bSeparatedDropDown ? true : false;
	if( _ppbd.m_bSeparatedDropDown
		&& m_nTrackingHT == 2
		&& _ppbd.m_bPushed
		)
	{
		_ppbd.m_bPushed = false;
		_ppbd.m_bPushedDropDown = true;
//		_ppbd.m_bHover = false;
	}

	_ppbd.m_clrForceTextDisabled = GetTextColorDisabled();
	_ppbd.m_clrForceTextNormal = GetTextColorNormal();
	_ppbd.m_clrForceTextHover = GetTextColorHover();
	_ppbd.m_clrForceTextPressed = GetTextColorPressed();

	g_PaintManager->PaintPushButton( dc, _ppbd );
	icon.DetachAsHICON();

	if( hIcon != NULL && bDestroyIcon )
	{
		::DestroyIcon( hIcon );
	}
}

LRESULT CExtColorButton::OnColorChanged(WPARAM wParam, LPARAM lParam)
{
	wParam;
	lParam;

//	m_clrSelected = (COLORREF)wParam;
//	Invalidate();
//	//UpdateWindow();
//CExtPopupMenuWnd * pPopup =
//		CExtPopupMenuWnd::GetTrackingMenu();
//	if( pPopup != NULL
//		&& pPopup->GetSafeHwnd() != NULL
//		)
//		pPopup->Invalidate();

	return
		GetParent()->SendMessage(
			CExtPopupColorMenuWnd::g_nMsgNotifyColorChanged,
			wParam,
			lParam
			);
}

LRESULT CExtColorButton::OnColorChangedFinally(WPARAM wParam, LPARAM lParam)
{
	lParam;
COLORREF clr = (COLORREF)wParam;
	m_clrSelected = (clr == ((COLORREF)(-1))) ? RGB(0,0,0) : clr;
	Invalidate();
	//UpdateWindow();

	return
		GetParent()->SendMessage(
			CExtPopupColorMenuWnd::g_nMsgNotifyColorChangedFinally,
			wParam,
			lParam
			);
}

LRESULT CExtColorButton::OnColorSelectCustom(WPARAM wParam, LPARAM lParam)
{
	wParam;
	lParam;
	if( m_bUseStdColorDlg )
	{
		CColorDialog dlg(m_clrSelected,CC_FULLOPEN,this);
		if( dlg.DoModal() != IDOK )
			return 0;
		m_clrSelected = dlg.m_cc.rgbResult;
	} // if( m_bUseStdColorDlg )
	else
	{
		CExtColorDlg dlg( m_clrSelected,m_clrSelected,this );
		if( dlg.DoModal() != IDOK )
			return 0;
		m_clrSelected = dlg.m_clrNew;
	} // else from if( m_bUseStdColorDlg )
	GetParent()->SendMessage(
		CExtPopupColorMenuWnd::g_nMsgNotifyColorChangedFinally,
		WPARAM( m_clrSelected ),
		lParam
		);
	Invalidate();
	//UpdateWindow();
	return 0;
}

LRESULT CExtButton::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == BM_GETCHECK )
		return m_nCheck;
	if( message == BM_SETCHECK )
	{
		m_nCheck = (INT)wParam;
		Invalidate();
		//UpdateWindow();
		return 0L;
	}
	if( message == BM_SETSTYLE )
	{
		m_nButtonType = (INT)wParam;
		m_nButtonType &= 0x0F;
		Invalidate();
		//UpdateWindow();
		return 0L;
	}
	if( message == CExtToolControlBar::g_nMsgContinueTrackMenu )
	{
		_OnTrackPopup( lParam ? true : false );
		return (!0L);
	}
	if(		message == WM_NOTIFY
		&&	m_wndToolTip.GetSafeHwnd() != NULL
		&&	IsWindow( m_wndToolTip.GetSafeHwnd() )
		&&	((LPNMHDR)lParam) != NULL
		&&	((LPNMHDR)lParam)->hwndFrom == m_wndToolTip.GetSafeHwnd()
		&&	((LPNMHDR)lParam)->code == TTN_SHOW
		)
		::SetWindowPos(
			m_wndToolTip.GetSafeHwnd(),
			HWND_TOP,
			0,0,0,0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE
			);
	
	if(
			message == CExtPopupMenuWnd::g_nMsgPrepareMenu
		||	message == CExtPopupMenuWnd::g_nMsgPopupNext
		||	message == CExtPopupMenuWnd::g_nMsgPopupPrev
		||	message == CExtPopupMenuWnd::g_nMsgNotifyMenuExpanded
		||	message == CExtPopupMenuWnd::g_nMsgNotifyMenuClosed
		||	message == CExtPopupMenuWnd::g_nMsgPopupDrawItem
		||	message == CExtPopupMenuWnd::g_nMsgPopupDrawLeftArea
		)
	{
		CWnd * pParent = GetParent();
		if( pParent )
			return pParent->SendMessage( message, wParam, lParam );
		else
			return 0L;
	}

bool bInvalidate = false;
	switch( message )
	{
	case WM_MOUSEMOVE:
		if( CExtToolControlBar::g_bMenuTracking )
		{
			CWnd * pWndParent = GetParent();
			if(		pWndParent != NULL
				&&	pWndParent->IsKindOf(RUNTIME_CLASS(CExtToolControlBar))
				)
			{
				_OnTrackPopup( false );
				return 0L;
			}
		}
		break;
	case WM_LBUTTONDBLCLK:
			return 0L;
		//message = WM_LBUTTONDOWN;
	//break;
	case BM_SETSTATE:
	case BM_SETSTYLE:
#if (WINVER >= 0x0400)
	case BM_CLICK:
	case BM_SETIMAGE:
#endif
	case WM_SETTEXT:
	case WM_SIZE:
	case WM_WINDOWPOSCHANGED:
		bInvalidate = true;
	break;
	} // switch( message )

	
LRESULT lResult =
		CButton::WindowProc( message, wParam, lParam );

//	if(
//			CExtPopupColorMenuWnd::g_nMsgNotifyColorChanged
//		||	CExtPopupColorMenuWnd::g_nMsgNotifyColorChangedFinally
//		||	CExtPopupColorMenuWnd::g_nMsgNotifyCustColor
//		)
//	{
//		CWnd * pParent = GetParent();
//		if( pParent )
//			pParent->SendMessage( message, wParam, lParam );
//	}

	if( bInvalidate )
	{
		Invalidate();
		//UpdateWindow();
	}
	return lResult;
}

void CExtColorButton::_OnClick(
	bool bSelectAny,
	bool bSeparatedDropDownClicked
	)
{
	if(		(!m_bSeparatedDropDown)
		||	bSeparatedDropDownClicked
		)
	{
		if( _OnTrackPopup( bSelectAny ) )
			return;
	}

CWnd * pWnd = GetParent();
	ASSERT( pWnd != NULL );
	ASSERT_VALID( pWnd );
	pWnd->SendMessage(
		CExtPopupColorMenuWnd::g_nMsgNotifyColorChangedFinally,
		(WPARAM)m_clrSelected,
		m_lParamCookie
		);
}

/////////////////////////////////////////////////////////////////////////////
// CExtCheckBox
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE( CExtCheckBox, CExtButton );

CExtCheckBox::CExtCheckBox()
	: m_bMouseLButtonDown( false )
	, m_bSpaceDown( false )
{
}

CExtCheckBox::~CExtCheckBox()
{
}

BEGIN_MESSAGE_MAP(CExtCheckBox, CExtButton)
//{{AFX_MSG_MAP(CExtCheckBox)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CExtCheckBox::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->hwnd == m_hWnd )
	{
		switch(pMsg->message) 
		{
		case WM_LBUTTONDBLCLK:
			pMsg->message = WM_LBUTTONDOWN;
			break;
			
		case WM_KEYDOWN:
			m_bSpaceDown = (pMsg->wParam == VK_SPACE) ? true : false;
			break;
			
		case WM_KEYUP:
			m_bSpaceDown = false;
			break;
			
		case WM_LBUTTONDOWN:
			m_bMouseLButtonDown = true;
			break;
			
		case WM_LBUTTONUP:
			m_bMouseLButtonDown = false;
			break;
		}
	}
	return CExtButton::PreTranslateMessage(pMsg);
}

void CExtCheckBox::_OnClick(
	bool bSelectAny,
	bool bSeparatedDropDownClicked
	)
{
	ASSERT_VALID( this );
	if( !SupportsCheck() )
	{
		CExtButton::_OnClick(
			bSelectAny,
			bSeparatedDropDownClicked
			);
		return;
	} // if( ! SupportsCheck() )
INT nCheck = GetCheck();
	nCheck = ((nCheck + 1 ) % (Is3StateType() ? 3 : 2));
	SetCheck( nCheck );
	
CWnd * pWnd = GetParent();
	if( pWnd->GetSafeHwnd() == NULL )
		return;
	ASSERT_VALID( pWnd );
HWND hWndThis = GetSafeHwnd();
int nDlgCtrlID = GetDlgCtrlID();
	pWnd->SendMessage(
		WM_COMMAND,
		MAKEWPARAM(nDlgCtrlID,BN_CLICKED),
		(LPARAM)m_hWnd
		);
	if( ::IsWindow(hWndThis) ){
		Invalidate();
	}
}

//////////////////////////////////////////////////////////////////////////

void CExtCheckBox::_RenderImpl(
	CDC & dc,
	bool bTransparent // = false
	)
{
	ASSERT_VALID( this );
CRect rectClient;
	GetClientRect( &rectClient );
CString sWindowText( _T("") );
	GetWindowText( sWindowText );
bool bDrawFocusRect = (GetFocus() == this && m_bDrawFocusRect) ? true : false;
bool bDefault = GetDefault() ? true : false;
bool bPushed = (m_bPushed || GetCheck()) ? true : false;

//	bool bEnabled = IsWindowEnabled() ? true : false;
bool bEnabled = OnQueryWindowEnabledState();

bool bFlat = GetFlat() ? true : false;
bool bHover =
		( m_bMouseOver
		&& !CExtPopupMenuWnd::IsMenuTracking() )
		? true : false;
	if( GetParent()->IsKindOf(RUNTIME_CLASS(CControlBar)) )
	{
		bDrawFocusRect = false;
		bFlat = true;
	}

	bTransparent = false;
 	if(	g_PaintManager->GetCb2DbTransparentMode(this) )
	{
		CExtPaintManager::stat_ExcludeChildAreas(
			dc,
			GetSafeHwnd(),
			CExtPaintManager::stat_DefExcludeChildAreaCallback
			);
		if( g_PaintManager->PaintDockerBkgnd( dc, this ) )
			bTransparent = true;
	}
	if( !bTransparent )
		dc.FillSolidRect( &rectClient, g_PaintManager->GetColor( CExtPaintManager::CLR_3DFACE_OUT) );

	CExtPaintManager::PAINTPUSHBUTTONDATA _ppbd(
		this,										   
		true, rectClient, (LPCTSTR)sWindowText,
		NULL, bFlat, bHover, bPushed,
		false, bEnabled,
		m_bDrawBorder ? true : false,
		bDrawFocusRect, bDefault, m_nAlign,
		OnQueryFont(),
		false, 0, bTransparent
		);
	
	_ppbd.m_clrForceTextDisabled = GetTextColorDisabled();
	_ppbd.m_clrForceTextNormal = GetTextColorNormal();
	_ppbd.m_clrForceTextHover = GetTextColorHover();
	_ppbd.m_clrForceTextPressed = GetTextColorPressed();

	PaintPushButton( dc, _ppbd );
	
	g_PaintManager->OnPaintSessionComplete( this );
}

//////////////////////////////////////////////////////////////////////////

void CExtCheckBox::PaintPushButton(
	CDC & dc,
	CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
	)
{
	ASSERT_VALID( this );
	if( _ppbd.m_rcClient.IsRectEmpty() )
		return;
	if( !_ppbd.m_bEnabled )
	{
		_ppbd.m_bHover = false;
		_ppbd.m_bDrawFocusRect = false;
		_ppbd.m_bIndeterminate = false;
	} // if( !_ppbd.m_bEnabled )
bool bLeftText = false;
DWORD dwStyle = GetStyle();
DWORD dwExStyle = GetExStyle();
	if(		(dwStyle&BS_LEFTTEXT) != 0
		||	(dwStyle&BS_RIGHTBUTTON) != 0
		||	(dwExStyle&WS_EX_RIGHT) != 0
		)
		bLeftText = true;
COLORREF clrOldBk = dc.GetBkColor();
int nOldBkMode = dc.SetBkMode( TRANSPARENT );
CRect	rcTextArea( _ppbd.m_rcClient ),
		rcTextLocation( 0, 0, 0, 0),
		rcBoxLocation(
			_ppbd.m_rcClient.left,
			_ppbd.m_rcClient.top,
			_ppbd.m_rcClient.left + __EXT_DEFAULT_CHECK_BOX_SIZE,
			_ppbd.m_rcClient.top + __EXT_DEFAULT_CHECK_BOX_SIZE
			);
	// pre-calculate box coordinates
	if( bLeftText )
	{
		rcBoxLocation.OffsetRect(
			_ppbd.m_rcClient.Width() - rcBoxLocation.Width(),
			0
			);
		rcTextArea.right = rcBoxLocation.left - 2;
		rcTextArea.left += 1;
	} // if( bLeftText )
	else
		rcTextArea.left = rcBoxLocation.right + 4;
	rcTextArea.DeflateRect( 1, 1 );
	
	// draw text
int nTextLength =
		(_ppbd.m_sText == NULL)
			? 0
			: _tcslen(_ppbd.m_sText);
	if(		nTextLength > 0
		&&	rcTextArea.bottom > rcTextArea.top 
		&&	rcTextArea.right > rcTextArea.left
		)
	{ // if have valid area, text & focus rect
		ASSERT( _ppbd.m_sText != NULL );

		COLORREF clrText;
		if( _ppbd.m_bEnabled )
		{
			if( _ppbd.m_bHover || _ppbd.m_bPushed )
			{
				if(			_ppbd.m_bPushed
						&&	_ppbd.m_clrForceTextPressed != ((COLORREF)-1L)
						)
						clrText = _ppbd.m_clrForceTextPressed;
				else if(	_ppbd.m_bHover
						&&	_ppbd.m_clrForceTextHover != ((COLORREF)-1L)
						)
						clrText = _ppbd.m_clrForceTextHover;
				else if( _ppbd.m_clrForceTextNormal != ((COLORREF)-1L) ) 
						clrText = _ppbd.m_clrForceTextNormal;
				else
						clrText = g_PaintManager->GetColor(COLOR_BTNTEXT);
			} // if( _ppbd.m_bHover || _ppbd.m_bPushed )
			else
				clrText = 
				( _ppbd.m_clrForceTextNormal == ((COLORREF)-1L) )
					? g_PaintManager->GetColor( COLOR_BTNTEXT )
					: _ppbd.m_clrForceTextNormal;
		} // if( _ppbd.m_bEnabled )
		else
		{
			clrText = 
				( _ppbd.m_clrForceTextDisabled == ((COLORREF)-1L) )
					? g_PaintManager->GetColor( COLOR_3DSHADOW )
					: _ppbd.m_clrForceTextDisabled;
		} // else from if( _ppbd.m_bEnabled )

		COLORREF clrOldText = dc.SetTextColor( clrText );

		CFont *pOldBtnFont = NULL, *pCurrFont = NULL;
		if( _ppbd.m_hFont != NULL )
			pCurrFont = CFont::FromHandle( _ppbd.m_hFont );
		else
			pCurrFont = &g_PaintManager->m_FontNormal;
		ASSERT( pCurrFont != NULL );
		pOldBtnFont = dc.SelectObject( pCurrFont );
		
//		CString sMeasureText( _ppbd.m_sText );
//		int nAmpIndex = sMeasureText.Find( _T('&') );
//		if( nAmpIndex >= 0 )
//		{
//			sMeasureText.Delete( nAmpIndex, 1 );
//			//ASSERT( sMeasureText.Find( _T('&') ) < 0 );
//		} // if( nAmpIndex >= 0 )
		UINT nDtMeasureFlags =
			DT_LEFT|DT_TOP|DT_CALCRECT;
		if( (dwStyle&BS_MULTILINE) != 0 )
		{
			rcTextLocation = rcTextArea;
			rcTextLocation.OffsetRect( -rcTextLocation.TopLeft() );
			rcTextLocation.bottom = rcTextLocation.top;
			nDtMeasureFlags |= DT_WORDBREAK;
		} // if( (dwStyle&BS_MULTILINE) != 0 )
		else
			nDtMeasureFlags |= DT_SINGLELINE;
		dc.DrawText(
			_ppbd.m_sText, nTextLength, //sMeasureText,
			rcTextLocation,
			nDtMeasureFlags
			);
		rcTextLocation.OffsetRect(
			rcTextArea.TopLeft() - rcTextLocation.TopLeft()
			);
		
		const __BS_H_MASK = (BS_LEFT|BS_CENTER|BS_RIGHT);
		UINT nDtDrawFlags = 0;
		if( (dwStyle&__BS_H_MASK) == BS_RIGHT )
		{
			nDtDrawFlags |= DT_RIGHT;
			rcTextLocation.OffsetRect(
				rcTextArea.Width() - rcTextLocation.Width(),
				0
				);
		}
		else if( (dwStyle&__BS_H_MASK) == BS_CENTER )
		{
			nDtDrawFlags |= DT_CENTER;
			rcTextLocation.OffsetRect(
				( rcTextArea.Width() - rcTextLocation.Width() ) / 2,
				0
				);
		}
		else
		{
			nDtDrawFlags |= DT_LEFT;
		}
		
		const __BS_V_MASK = (BS_TOP|BS_VCENTER|BS_BOTTOM);
		if( (dwStyle&__BS_V_MASK) == BS_BOTTOM )
		{
			nDtDrawFlags |= DT_BOTTOM;
			rcTextLocation.OffsetRect(
				0,
				rcTextArea.Height() - rcTextLocation.Height()
				);
			rcBoxLocation.OffsetRect(
				0,
				( _ppbd.m_rcClient.bottom - __EXT_DEFAULT_CHECK_BOX_SIZE )
				);
		}
		else if( (dwStyle&__BS_V_MASK) == BS_TOP )
		{
			nDtDrawFlags |= DT_TOP;
		}
		else
		{
			nDtDrawFlags |= DT_VCENTER;
			rcTextLocation.OffsetRect(
				0,
				( rcTextArea.Height() - rcTextLocation.Height() ) / 2
				);
			rcBoxLocation.OffsetRect(
				0,
				( _ppbd.m_rcClient.Height() - rcBoxLocation.Height() ) / 2 //+ 1
				);
		}
		if( (dwStyle&BS_MULTILINE) != 0 )
			nDtDrawFlags |= DT_WORDBREAK;
		else
			nDtDrawFlags |= DT_SINGLELINE;
		dc.DrawText(
			_ppbd.m_sText,
			nTextLength,
			rcTextLocation,
			nDtDrawFlags
			);
		
		dc.SelectObject( pOldBtnFont );
		dc.SetTextColor( clrOldText );

		if( _ppbd.m_bDrawFocusRect )
		{
			CRect rcFocus( rcTextLocation );
			rcFocus.InflateRect( 2, 2 );
			rcFocus.left = max( rcFocus.left, rcTextArea.left );
			rcFocus.top = max( rcFocus.top, rcTextArea.top );
			rcFocus.right = min( rcFocus.right, rcTextArea.right );
			rcFocus.bottom = min( rcFocus.bottom, rcTextArea.bottom );
			rcFocus.InflateRect(
				bLeftText ? 0 : 1,
				0,
				bLeftText ? 1 : 0,
				0
				);
			dc.DrawFocusRect( &rcFocus );
		} // if( _ppbd.m_bDrawFocusRect )

	} // if have valid area, text & focus rect
	
	// draw box
	PaintBox( dc, rcBoxLocation, _ppbd );
	dc.SetBkMode( nOldBkMode );
	dc.SetBkColor( clrOldBk );
}

//////////////////////////////////////////////////////////////////////////

void CExtCheckBox::PaintBox(
	CDC & dc,
	CRect & rcBox,
	CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
	)
{
	ASSERT_VALID( this );
	int nCheck = GetCheck();
	eCheckBoxState check_box_state = BOX_UNCHECKED;
	
	switch( nCheck ) 
	{
	case BST_UNCHECKED:
		if( _ppbd.m_bEnabled )
		{
			if( m_bSpaceDown )
			{
				check_box_state = BOX_LDOWN_UNCHECKED;
			}
			else
			{
				if(		( _ppbd.m_bHover && !m_bMouseLButtonDown) 
					||  (!_ppbd.m_bHover &&  m_bMouseLButtonDown) )
				{
					check_box_state = BOX_MOUSE_HOVER_UNCHECKED;
				}
				else if( _ppbd.m_bHover && m_bMouseLButtonDown )
				{
					check_box_state = BOX_LDOWN_UNCHECKED;
				}
				else
				{
					check_box_state = BOX_UNCHECKED;
				}
			}
		}
		else
		{
			check_box_state = BOX_DISABLED_UNCHECKED;
		}
		break;
		
	case BST_CHECKED:
		if( _ppbd.m_bEnabled )
		{
			if( m_bSpaceDown )
			{
				check_box_state = BOX_LDOWN_CHECKED;
			}
			else
			{
				if(		( _ppbd.m_bHover && !m_bMouseLButtonDown) 
					||  (!_ppbd.m_bHover &&  m_bMouseLButtonDown) )
				{
					check_box_state = BOX_MOUSE_HOVER_CHECKED;
				}
				else if( _ppbd.m_bHover && m_bMouseLButtonDown )
				{
					check_box_state = BOX_LDOWN_CHECKED;
				}
				else
				{
					check_box_state = BOX_CHECKED;
				}
			}
		}
		else
		{
			check_box_state = BOX_DISABLED_CHECKED;
		}
		break;
		
	case BST_INDETERMINATE:
		if( _ppbd.m_bEnabled )
		{
			if( m_bSpaceDown )
			{
				check_box_state = BOX_LDOWN_INDETERMINATE;
			}
			else
			{
				if(		( _ppbd.m_bHover && !m_bMouseLButtonDown) 
					||  (!_ppbd.m_bHover &&  m_bMouseLButtonDown) )
				{
					check_box_state = BOX_MOUSE_HOVER_INDETERMINATE;
				}
				else if( _ppbd.m_bHover && m_bMouseLButtonDown )
				{
					check_box_state = BOX_LDOWN_INDETERMINATE;
				}
				else
				{
					check_box_state = BOX_INDETERMINATE;
				}
			}
		}
		else
		{
			check_box_state = BOX_DISABLED_INDETERMINATE;
		}
		break;
	}
	
	if( !OnPaintBox(dc, rcBox, check_box_state ) )
	{
		// state of check box
		UINT nState = 0;
		if(	!_ppbd.m_bEnabled )
			nState |= DFCS_INACTIVE;
		
		if(		( _ppbd.m_bHover && m_bMouseLButtonDown )	
			||	m_bSpaceDown
			)
			nState |= DFCS_PUSHED;
		
		if( nCheck != 0 )
			nState |= DFCS_CHECKED;
		if( GetStyle() & BS_FLAT )
			nState |= DFCS_FLAT;
		
		dc.DrawFrameControl(&rcBox, DFC_BUTTON, DFCS_BUTTONCHECK|nState);
	}
}

//////////////////////////////////////////////////////////////////////////

bool CExtCheckBox::OnPaintBox(
	CDC & dc,
	CRect & rcBox,
	eCheckBoxState eState
	)
{
	ASSERT_VALID( this );
	dc;
	rcBox;
	eState;
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CExtRadioButton
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE( CExtRadioButton, CExtCheckBox );

CExtRadioButton::CExtRadioButton()
{
}

CExtRadioButton::~CExtRadioButton()
{
}

BEGIN_MESSAGE_MAP(CExtRadioButton, CExtCheckBox)
//{{AFX_MSG_MAP(CExtRadioButton)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

BOOL CExtRadioButton::PreTranslateMessage(MSG* pMsg) 
{
	if(		pMsg->hwnd == m_hWnd && GetParent() != NULL 
		&&	pMsg->message == WM_KEYDOWN
		)
	{
		switch(pMsg->wParam) 
		{
		case VK_UP:
		case VK_LEFT:
			if( _CheckNextPrevRadioButton(false) )
				return TRUE;
			break;
			
		case VK_DOWN:
		case VK_RIGHT:
			if( _CheckNextPrevRadioButton(true) )
				return TRUE;
			break;
		}
		
	} // if( pMsg->hwnd == m_hWnd )
	return CExtCheckBox::PreTranslateMessage(pMsg);
}

LRESULT CExtRadioButton::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_GETDLGCODE )
	{
		LRESULT lResult =
			CExtCheckBox::WindowProc( message, wParam, lParam );
		lResult |= DLGC_RADIOBUTTON;
		return lResult;
	} // if( message == WM_GETDLGCODE )
	if( message == BM_SETCHECK )
		_UncheckRadioButtonsInGroup();
	return
		CExtCheckBox::WindowProc( message, wParam, lParam );;
}

bool CExtRadioButton::_CheckNextPrevRadioButton( bool bNext )
{
	ASSERT_VALID (this);
	
CWnd* pWndParent = GetParent();
	ASSERT_VALID( pWndParent );
	
CExtButton* pBtn = NULL;
	
	for( 
		CWnd* pWnd = pWndParent->GetNextDlgGroupItem( this, !bNext ); 
		pWnd != this; 
		pWnd = pWndParent->GetNextDlgGroupItem( pWnd, !bNext )
		)
	{
		if(		(pBtn = DYNAMIC_DOWNCAST(CExtRadioButton, pWnd)) != NULL 
			&&	(pBtn->GetType() == BS_AUTORADIOBUTTON || pBtn->GetType() == BS_RADIOBUTTON) 
			&&	(pBtn->GetStyle() & (WS_DISABLED | WS_VISIBLE)) == WS_VISIBLE)
		{
			break;
		}
	}
	
	if( pBtn != NULL && pBtn != this && !pBtn->GetCheck() )
	{
		ASSERT_VALID( pBtn );

		pBtn->SetCheck( BST_CHECKED );
		pBtn->ModifyStyle(0, WS_TABSTOP);
		pBtn->SetFocus();
		
		::SendMessage(
			pBtn->GetParent()->GetSafeHwnd(), 
			WM_COMMAND,
			MAKELONG (::GetWindowLong(pBtn->m_hWnd, GWL_ID), BN_CLICKED),
			(LPARAM) pBtn->m_hWnd
			);
		return true;
	}

	return false;
}

void CExtRadioButton::_UncheckRadioButtonsInGroup()
{
	ASSERT_VALID (this);
CWnd* pWndParent = GetParent();
	if (pWndParent == NULL)
		return;
	ASSERT_VALID( pWndParent );
	
CWnd *pWnd = pWndParent->GetNextDlgGroupItem( this );
	if( !this->IsWindowEnabled() )
		return;

	while( pWnd != this && pWnd != NULL )
	{
		CExtRadioButton *pBtn = DYNAMIC_DOWNCAST( CExtRadioButton, pWnd );
		if( pBtn != NULL )
		{
			ASSERT_VALID( pBtn );
			
			BOOL bIsRadioButton =
				(pBtn->GetType() == BS_AUTORADIOBUTTON || pBtn->GetType() == BS_RADIOBUTTON)
				? true : false;

			if( pBtn != NULL && pBtn != this && bIsRadioButton && pBtn->GetCheck() == BST_CHECKED ) 
			{
				pBtn->SetCheck( BST_UNCHECKED );
				pBtn->ModifyStyle(WS_TABSTOP,0);
				pBtn->Invalidate();
			}
		}
		pWnd = pWndParent->GetNextDlgGroupItem( pWnd );
	}
}

void CExtRadioButton::_OnClick(
	bool bSelectAny,
	bool bSeparatedDropDownClicked
	)
{
 	ASSERT_VALID( this );
	if( !SupportsCheck() )
	{
		CExtCheckBox::_OnClick(
			bSelectAny,
			bSeparatedDropDownClicked
			);
		return;
	} // if( ! SupportsCheck() )


INT nCheck = GetCheck();
	if( !nCheck ){
		nCheck = !nCheck;
		_UncheckRadioButtonsInGroup ();
	}
	SetCheck( nCheck );
	ModifyStyle(0, WS_TABSTOP);

CWnd * pWnd = GetParent();
	if( pWnd->GetSafeHwnd() == NULL )
		return;
	ASSERT_VALID( pWnd );
HWND hWndThis = GetSafeHwnd();
int nDlgCtrlID = GetDlgCtrlID();
	pWnd->SendMessage(
		WM_COMMAND,
		MAKEWPARAM(nDlgCtrlID,BN_CLICKED),
		(LPARAM)m_hWnd
		);
	if( ::IsWindow(hWndThis) ){
		Invalidate();
	}
}

//////////////////////////////////////////////////////////////////////////

void CExtRadioButton::PaintBox(
	CDC & dc,
	CRect & rcBox,
	CExtPaintManager::PAINTPUSHBUTTONDATA & _ppbd
	)
{
	ASSERT_VALID( this );
	int nCheck = GetCheck();
	eRadioBoxState radio_box_state = BOX_UNCHECKED;
	
	switch( nCheck ) 
	{
	case BST_UNCHECKED:
		if( _ppbd.m_bEnabled )
		{
			if( m_bSpaceDown )
			{
				radio_box_state = BOX_LDOWN_UNCHECKED;
			}
			else
			{
				if(		( _ppbd.m_bHover && !m_bMouseLButtonDown) 
					||  (!_ppbd.m_bHover &&  m_bMouseLButtonDown) )
				{
					radio_box_state = BOX_MOUSE_HOVER_UNCHECKED;
				}
				else if( _ppbd.m_bHover && m_bMouseLButtonDown )
				{
					radio_box_state = BOX_LDOWN_UNCHECKED;
				}
				else
				{
					radio_box_state = BOX_UNCHECKED;
				}
			}
		}
		else
		{
			radio_box_state = BOX_DISABLED_UNCHECKED;
		}
		break;
		
	case BST_CHECKED:
		if( _ppbd.m_bEnabled )
		{
			if( m_bSpaceDown )
			{
				radio_box_state = BOX_LDOWN_CHECKED;
			}
			else
			{
				if(		( _ppbd.m_bHover && !m_bMouseLButtonDown) 
					||  (!_ppbd.m_bHover &&  m_bMouseLButtonDown) )
				{
					radio_box_state = BOX_MOUSE_HOVER_CHECKED;
				}
				else if( _ppbd.m_bHover && m_bMouseLButtonDown )
				{
					radio_box_state = BOX_LDOWN_CHECKED;
				}
				else
				{
					radio_box_state = BOX_CHECKED;
				}
			}
		}
		else
		{
			radio_box_state = BOX_DISABLED_CHECKED;
		}
		break;
	}
	
	if( !OnPaintBox(dc, rcBox, radio_box_state ) )
	{
		// state of check box
		UINT nState = 0;
		if(	!_ppbd.m_bEnabled )
			nState |= DFCS_INACTIVE;

		if(		( _ppbd.m_bHover && m_bMouseLButtonDown )	
			||	m_bSpaceDown
			)
			nState |= DFCS_PUSHED;
		
		if( nCheck != 0 )
			nState |= DFCS_CHECKED;
		if( GetStyle() & BS_FLAT )
			nState |= DFCS_FLAT;
		
		dc.DrawFrameControl(&rcBox, DFC_BUTTON, DFCS_BUTTONRADIO|nState);
	}
}

bool CExtRadioButton::OnPaintBox(
	CDC & dc,
	CRect & rcBox,
	eRadioBoxState eState
	)
{
	ASSERT_VALID( this );
	dc;
	rcBox;
	eState;
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CExtHyperLinkButton
/////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE( CExtHyperLinkButton, CExtButton );

CExtHyperLinkButton::CExtHyperLinkButton()
	: m_bUseStdCommandNotification( false )
	, m_bAllowVisited( true )
	, m_bVisited( false )
	, m_nHorzTextAlign( DT_LEFT )
	, m_clrTextVisited( RGB( 128, 0, 128 ) )
	, m_bTextUnderline( false )
{
	m_clrTextNormal = RGB( 0, 0, 255 );
	m_clrTextHover = RGB( 255, 0, 0 );
	
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	SetBtnCursor( IDC_EXT_HAND );
#else
	SetBtnCursor( 32649 );
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
}

CExtHyperLinkButton::~CExtHyperLinkButton()
{
}

BEGIN_MESSAGE_MAP(CExtHyperLinkButton, CExtButton)
//{{AFX_MSG_MAP(CExtHyperLinkButton)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CExtHyperLinkButton::_OnClick(
	bool bSelectAny,
	bool bSeparatedDropDownClicked
	)
{
	ASSERT_VALID( this );
	if( m_bUseStdCommandNotification )
	{
		CExtButton::_OnClick(
			bSelectAny,
			bSeparatedDropDownClicked
			);
		m_bVisited = true;
		Invalidate();
		return;
	} // if( m_bUseStdCommandNotification )
	if( m_strURL.IsEmpty() )
	{
		CString strURL;
		GetWindowText( strURL );
		strURL.TrimLeft();
		strURL.TrimRight();
		VERIFY( OnHyperLinkOpen( LPCTSTR(strURL) ) );
	} // if( m_strURL.IsEmpty() )
	else
		VERIFY( OnHyperLinkOpen( LPCTSTR(m_strURL) ) );
}

void CExtHyperLinkButton::_RenderImpl( // non-buffered paint
	CDC & dc,
	bool bTransparent // = false
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	bTransparent;
CRect rcClient;
GetClientRect( &rcClient );
	if(		(! g_PaintManager->GetCb2DbTransparentMode(this) )
		||	(! g_PaintManager->PaintDockerBkgnd( dc, this ) )
		)
	{
		COLORREF clrSolidBk =
			g_PaintManager->GetColor(
				CExtPaintManager::CLR_3DFACE_OUT
				);
		dc.FillSolidRect( &rcClient, clrSolidBk );
	}
bool bFocus = (GetFocus() == this) ? true : false;
bool bPushed = m_bPushed ? true : false;

//bool bEnabled = IsWindowEnabled() ? true : false;
bool bEnabled = OnQueryWindowEnabledState();

bool bHover =
		( m_bMouseOver
		&& !CExtPopupMenuWnd::IsMenuTracking() )
			? true : false;
bool bVisited = (m_bVisited && m_bAllowVisited) ? true : false;
	OnPaintHyperLink(
		dc,
		rcClient,
		bVisited,
		bEnabled,
		m_bDrawFocusRect,
		bFocus,
		bHover,
		bPushed
		);
}

COLORREF CExtHyperLinkButton::OnQueryHyperLinkTextColor(
	bool bVisited,
	bool bEnabled,
	bool bDrawFocusRect,
	bool bFocus,
	bool bHover,
	bool bPushed
	) const
{
	ASSERT_VALID( this );
	bDrawFocusRect;
COLORREF clrText = m_clrTextNormal;
	if( bEnabled )
	{
		if( bHover || bPushed || bFocus )
			clrText = m_clrTextHover;
		else if( bVisited )
			clrText = m_clrTextVisited;
		if( clrText == COLORREF(-1L) )
			clrText =
				g_PaintManager->GetColor(
					COLOR_BTNTEXT
					);
	} // if( bEnabled )
	else
	{
		if( m_clrTextDisabled == COLORREF(-1L) )
			clrText =
				g_PaintManager->GetColor(
					COLOR_3DSHADOW
					);
		else
			clrText = m_clrTextDisabled;
	} // else from if( bEnabled )
	return clrText;
}

void CExtHyperLinkButton::OnPaintHyperLink(
	CDC & dc,
	CRect rcText,
	bool bVisited,
	bool bEnabled,
	bool bDrawFocusRect,
	bool bFocus,
	bool bHover,
	bool bPushed
	) const
{
	ASSERT_VALID( this );
	ASSERT(
			m_nHorzTextAlign == DT_LEFT
		||	m_nHorzTextAlign == DT_RIGHT
		||	m_nHorzTextAlign == DT_CENTER
		);
	ASSERT( dc.GetSafeHdc() != NULL );
CExtSafeString strHyperLinkURL = OnQueryHyperLinkURL( true );
	if( strHyperLinkURL.IsEmpty() )
		return;
CRect rcInitial( rcText );
int nOldBkMode = dc.SetBkMode( TRANSPARENT );
CFont * pOldFont = NULL, * pFontNormal = &g_PaintManager->m_FontNormal, fontTemp;
	if( bHover || bPushed || bFocus || m_bTextUnderline )
	{
		LOGFONT _lf;
		::memset( &_lf, 0, sizeof(LOGFONT) );
		pFontNormal->GetLogFont( &_lf );
		_lf.lfUnderline = 1;
		fontTemp.CreateFontIndirect( &_lf );
		pOldFont = dc.SelectObject( &fontTemp );
	} // if( bHover || bPushed || bFocus )
	else
		pOldFont = dc.SelectObject( pFontNormal );
COLORREF clrText =
			OnQueryHyperLinkTextColor(
				bVisited,
				bEnabled,
				bDrawFocusRect,
				bFocus,
				bHover,
				bPushed
				);
COLORREF clrTextOld = dc.SetTextColor( clrText );
int nTextLen = strHyperLinkURL.GetLength();
	rcText.DeflateRect( 2, 2 );
CRect rcFocus( rcText );
	if( bDrawFocusRect )
	{
		CRect rcAlign( 0, 0, 0, 0 );
		dc.DrawText(
			LPCTSTR(strHyperLinkURL),
			nTextLen,
			&rcAlign,
			DT_SINGLELINE|DT_LEFT|DT_TOP|DT_CALCRECT
			);
		CSize sizeAlign = rcAlign.Size();
		CSize sizeReal = rcText.Size();
		if( sizeAlign.cx > sizeReal.cx )
			sizeAlign.cx = sizeReal.cx;
		if( sizeAlign.cy > sizeReal.cy )
			sizeAlign.cy = sizeReal.cy;
		rcFocus.right = rcFocus.left + sizeAlign.cx;
		rcFocus.bottom = rcFocus.top + sizeAlign.cy;
		rcFocus.OffsetRect(
			0,
			( rcInitial.Height() - rcFocus.Height() ) / 2
			);
		if( m_nHorzTextAlign == DT_RIGHT )
			rcFocus.OffsetRect(
				rcInitial.Width() - rcAlign.Width() - 4,
				0
				);
		else if( m_nHorzTextAlign == DT_CENTER )
			rcFocus.OffsetRect(
				( rcInitial.Width() - rcAlign.Width() - 4 ) / 2,
				0
				);
		rcFocus.InflateRect( 2, 2, 2, 1 );
	} // if( bDrawFocusRect )
	dc.DrawText(
		LPCTSTR(strHyperLinkURL),
		nTextLen,
		&rcText,
		m_nHorzTextAlign|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS
		);
	dc.SetTextColor( clrTextOld );
	dc.SelectObject( pOldFont );
	dc.SetBkMode( nOldBkMode );
	if( bDrawFocusRect && bFocus )
		dc.DrawFocusRect( &rcFocus );
}

CExtSafeString CExtHyperLinkButton::OnQueryHyperLinkURL(
	bool bDisplayURL // true - display URL, false - navigate URL
	) const
{
	ASSERT_VALID( this );
	bDisplayURL;
CExtSafeString strHyperLinkURL;
int nTextLen = GetWindowTextLength();
	if( nTextLen > 0 )
	{
		GetWindowText( strHyperLinkURL.GetBuffer( nTextLen+2 ), nTextLen+1 );
		strHyperLinkURL.ReleaseBuffer();
	} // if( nTextLen > 0 )
	return strHyperLinkURL;
}

bool CExtHyperLinkButton::OnHyperLinkOpen(
	__EXT_MFC_SAFE_LPCTSTR strURL,
	CExtHyperLinkButton::e_OpenMode_t eOM, // = CExtHyperLinkButton::__EOM_OPEN
	bool bAllowErrorAlert // = false
	) const
{
	if(		_tcsnicmp( strURL, _T("http:////"), 7 ) == 0
		||	_tcsnicmp( strURL, _T("file:////"), 7 ) == 0
		||	_tcsnicmp( strURL, _T("www."), 4 ) == 0
		||	_tcsnicmp( strURL, _T("web."), 4 ) == 0
		)
	{
		if( ! _OpenVia_RegInfo( strURL, eOM, bAllowErrorAlert ) )
		{
			if( ! _OpenVia_COM( strURL, eOM, bAllowErrorAlert ) )
			{
				if( ! _OpenVia_Shell( strURL, eOM, bAllowErrorAlert ) )
				{
						TRACE0( "*** Failed to open hyper link (CExtHyperLinkButton::OnHyperLinkOpen) ***\n" );
						return false;
				} // if( ! _OpenVia_Shell( strURL, eOM, bAllowErrorAlert ) )
			} // if( ! _OpenVia_COM( strURL, eOM, bAllowErrorAlert ) )
		} // if( ! _OpenVia_RegInfo( strURL, eOM, bAllowErrorAlert ) )
	}
	else
	{
		if( ! _OpenVia_COM( strURL, eOM ) )
		{
			if( ! _OpenVia_Shell( strURL, eOM ) )
			{
				if( ! _OpenVia_RegInfo( strURL, eOM ) )
				{
					TRACE0( "*** Failed to open hyper link (CExtHyperLinkButton::OnHyperLinkOpen) ***\n" );
					return false;
				} // if( ! _OpenVia_RegInfo( strURL, eOM ) )
			} // if( ! _OpenVia_Shell( strURL, eOM ) )
		} // if( ! _OpenVia_COM( strURL, eOM ) )
	}
	m_bVisited = true;
	((CWnd *)this)->Invalidate();
	return true;
}

bool CExtHyperLinkButton::_OpenVia_COM(
	__EXT_MFC_SAFE_LPCTSTR strURL,
	CExtHyperLinkButton::e_OpenMode_t eOM, // = CExtHyperLinkButton::__EOM_OPEN
	bool bAllowErrorAlert // = false
	) const
{
	ASSERT_VALID( this );
	ASSERT( strURL != NULL );
	ASSERT( _tcslen( strURL ) > 0 );
	bAllowErrorAlert;
IUniformResourceLocator *pURL;
HRESULT hr =
		::CoCreateInstance(
			CLSID_InternetShortcut,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IUniformResourceLocator,
			(LPVOID*)&pURL
			);
	if( FAILED(hr) )
	{
		//ASSERT( FALSE );
		return false;
	}
	hr =
		pURL->SetURL(
			LPCTSTR(strURL),
			IURL_SETURL_FL_GUESS_PROTOCOL
			);
	if( FAILED(hr) )
	{
		//ASSERT( FALSE );
		pURL->Release();
		return false;
	}
URLINVOKECOMMANDINFO _uici;
	::memset( &_uici, 0, sizeof(URLINVOKECOMMANDINFO) );
	_uici.dwcbSize = sizeof(URLINVOKECOMMANDINFO);
	_uici.dwFlags = IURL_INVOKECOMMAND_FL_ALLOW_UI;
	_uici.hwndParent = ::GetDesktopWindow(); // GetParent()->GetSafeHwnd();
	switch( eOM )
	{
	case __EOM_OPEN:
		_uici.pcszVerb = _T("open");
	break;
	case __EOM_EDIT:
		_uici.pcszVerb = _T("edit");
	break;
	case __EOM_PRINT:
		_uici.pcszVerb = _T("print");
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( eOM )
	hr = pURL->InvokeCommand(&_uici);
	if( FAILED(hr) )
	{
		//ASSERT( FALSE );
		pURL->Release();
		return false;
	}
	pURL->Release();
	return true;
}

bool CExtHyperLinkButton::_OpenVia_Shell(
	__EXT_MFC_SAFE_LPCTSTR strURL,
	CExtHyperLinkButton::e_OpenMode_t eOM, // = CExtHyperLinkButton::__EOM_OPEN
	bool bAllowErrorAlert // = false
	) const
{
	ASSERT_VALID( this );
	ASSERT( strURL != NULL );
	ASSERT( _tcslen( strURL ) > 0 );
LPCTSTR strOpenMode = NULL;
	switch( eOM )
	{
	case __EOM_OPEN:
		strOpenMode = _T("open");
	break;
	case __EOM_EDIT:
		strOpenMode = _T("edit");
	break;
	case __EOM_PRINT:
		strOpenMode = _T("print");
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( eOM )
	ASSERT( strOpenMode != NULL );
HINSTANCE hInstance =
		::ShellExecute(
			::GetDesktopWindow(), // GetParent()->GetSafeHwnd(),
			strOpenMode,
			LPCTSTR(strURL),
			NULL,
			NULL,
			SW_SHOW
			);
	if( LONG(hInstance) <= LONG(HINSTANCE_ERROR) )
	{
		if( bAllowErrorAlert )
		{
			CExtSafeString strErrorDescription;
			switch( LONG(hInstance) )
			{
			case 0:
				strErrorDescription =
					_T("OS is out of memory or resources.");
			break;
			case SE_ERR_PNF:
				strErrorDescription = _T("Path was not found.");
			break;
			case SE_ERR_FNF:
				strErrorDescription =
					_T("File was not found.");
			break;
			case ERROR_BAD_FORMAT:
				strErrorDescription =
					_T("EXE file is invalid (must run on other platform or error in its image).");
			break;
			case SE_ERR_ACCESSDENIED:
				strErrorDescription =
					_T("OS denied access to the file.");
			break;
			case SE_ERR_ASSOCINCOMPLETE:
				strErrorDescription =
					_T("The filename association is incomplete or invalid.");
			break;
			case SE_ERR_DDEBUSY:
				strErrorDescription =
					_T("DDE transaction was not completed due to other DDE transactions.");
			break;
			case SE_ERR_DDEFAIL:
				strErrorDescription =
					_T("DDE transaction failed.");
			break;
			case SE_ERR_DDETIMEOUT:
				strErrorDescription =
					_T("DDE transaction could not be completed due to request timed out.");
			break;
			case SE_ERR_DLLNOTFOUND:
				strErrorDescription =
					_T("DLL was not found.");
			break;
			case SE_ERR_NOASSOC:
				strErrorDescription =
					_T("There is no application associated with the given filename extension.");
			break;
			case SE_ERR_OOM:
				strErrorDescription =
					_T("Not enough memory.");
			break;
			case SE_ERR_SHARE:
				strErrorDescription =
					_T("A sharing violation occurred.");
			break;
			default:
				strErrorDescription.Format(
					_T("Unknown Error (%d=0x%X) occurred."), int(hInstance), int(hInstance) );
			break;
			} // switch( LONG(hInstance) )
			CExtSafeString strErrorMessage;
			strErrorMessage.Format(
				_T("Unable to open hyper link:\n%s"),
				LPCTSTR(strErrorDescription)
				);
			::AfxMessageBox(
				LPCTSTR(strErrorDescription),
				MB_OK|MB_ICONERROR
				);
		} // if( bAllowErrorAlert )
		return false;
	} // if( LONG(hInstance) <= LONG(HINSTANCE_ERROR) )
	return true;
}

bool CExtHyperLinkButton::_OpenVia_RegInfo(
	__EXT_MFC_SAFE_LPCTSTR strURL,
	CExtHyperLinkButton::e_OpenMode_t eOM, // = CExtHyperLinkButton::__EOM_OPEN
	bool bAllowErrorAlert // = false
	) const
{
	ASSERT_VALID( this );
	ASSERT( strURL != NULL );
	ASSERT( _tcslen( strURL ) > 0 );
TCHAR strKey[ MAX_PATH + MAX_PATH + 2];
HINSTANCE hInstance = NULL;
LPTSTR strExt = _T(".htm");
HKEY hRegKey;
//	if( GetRegKey(HKEY_CLASSES_ROOT, strExt, strKey) != ERROR_SUCCESS )
//		return false;
	if( ::RegOpenKeyEx( HKEY_CLASSES_ROOT, strExt, 0, KEY_QUERY_VALUE, &hRegKey ) != ERROR_SUCCESS )
		return false;
	{ // BLOCK
		long nSize = MAX_PATH;
		TCHAR strData[MAX_PATH];
		::RegQueryValue( hRegKey, NULL, strData, &nSize );
		_tcscpy( strKey, strData );
		::RegCloseKey( hRegKey );
	} // BLOCK
LPCTSTR strOpenMode = NULL;
	switch( eOM )
	{
	case __EOM_OPEN:
		strOpenMode = _T("\\shell\\open\\command");
	break;
	case __EOM_EDIT:
		strOpenMode = _T("\\shell\\edit\\command");
	break;
	case __EOM_PRINT:
		strOpenMode = _T("\\shell\\print\\command");
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( eOM )
	ASSERT( strOpenMode != NULL );
	_tcscat( strKey, strOpenMode );
//	if( GetRegKey(HKEY_CLASSES_ROOT, strKey, strKey) != ERROR_SUCCESS )
//		return false;
	if( ::RegOpenKeyEx( HKEY_CLASSES_ROOT, strKey, 0, KEY_QUERY_VALUE, &hRegKey ) != ERROR_SUCCESS )
		return false;
	{ // BLOCK
		long nSize = MAX_PATH;
		TCHAR strData[MAX_PATH];
		::RegQueryValue( hRegKey, NULL, strData, &nSize );
		_tcscpy( strKey, strData );
		::RegCloseKey( hRegKey );
	} // BLOCK
LPTSTR strPos = _tcsstr( strKey, _T("\"%1\"") );
	if( strPos == NULL )
	{
		strPos = _tcsstr( strKey, _T("%1") );
		if( strPos == NULL )
			strPos = strKey + _tcslen(strKey) - 1;
		else
			*strPos = _T('\0');
	}
	else
		*strPos = _T('\0');
	_tcscat( strPos, _T(" ") );
	_tcscat( strPos, LPCTSTR(strURL) );
USES_CONVERSION;
	hInstance = (HINSTANCE)::WinExec(T2A(strKey), SW_SHOW);
	if( LONG(hInstance) <= LONG(HINSTANCE_ERROR) )
	{
		if( bAllowErrorAlert )
		{
			CExtSafeString strErrorDescription;
			switch( LONG(hInstance) )
			{
			case 0:
				strErrorDescription =
					_T("OS is out of memory or resources.");
			break;
			case SE_ERR_PNF:
				strErrorDescription = _T("Path was not found.");
			break;
			case SE_ERR_FNF:
				strErrorDescription =
					_T("File was not found.");
			break;
			case ERROR_BAD_FORMAT:
				strErrorDescription =
					_T("EXE file is invalid (must run on other platform or error in its image).");
			break;
			case SE_ERR_ACCESSDENIED:
				strErrorDescription =
					_T("OS denied access to the file.");
			break;
			case SE_ERR_ASSOCINCOMPLETE:
				strErrorDescription =
					_T("The filename association is incomplete or invalid.");
			break;
			case SE_ERR_DDEBUSY:
				strErrorDescription =
					_T("DDE transaction was not completed due to other DDE transactions.");
			break;
			case SE_ERR_DDEFAIL:
				strErrorDescription =
					_T("DDE transaction failed.");
			break;
			case SE_ERR_DDETIMEOUT:
				strErrorDescription =
					_T("DDE transaction could not be completed due to request timed out.");
			break;
			case SE_ERR_DLLNOTFOUND:
				strErrorDescription =
					_T("DLL was not found.");
			break;
			case SE_ERR_NOASSOC:
				strErrorDescription =
					_T("There is no application associated with the given filename extension.");
			break;
			case SE_ERR_OOM:
				strErrorDescription =
					_T("Not enough memory.");
			break;
			case SE_ERR_SHARE:
				strErrorDescription =
					_T("A sharing violation occurred.");
			break;
			default:
				strErrorDescription.Format(
					_T("Unknown Error (%d=0x%X) occurred."), int(hInstance), int(hInstance) );
			break;
			} // switch( LONG(hInstance) )
			CExtSafeString strErrorMessage;
			strErrorMessage.Format(
				_T("Unable to open hyper link:\n%s"),
				LPCTSTR(strErrorDescription)
				);
			::AfxMessageBox(
				LPCTSTR(strErrorDescription),
				MB_OK|MB_ICONERROR
				);
		} // if( bAllowErrorAlert )
		return false;
	} // if( LONG(hInstance) <= LONG(HINSTANCE_ERROR) )
	return true;
}
