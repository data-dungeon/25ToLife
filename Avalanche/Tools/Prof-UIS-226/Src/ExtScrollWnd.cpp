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

#if (!defined __EXT_MFC_NO_SCROLLWND)

#if (!defined __EXT_SCROLLWND_H)
	#include <ExtScrollWnd.h>
#endif 

#if (!defined __AFXPRIV_H__)
	#include <AfxPriv.h>
#endif 

#if (!defined __EXT_TOOLCONTROLBAR_H)
	#include <ExtToolControlBar.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../src/ExtMemoryDC.h>
#endif

#if _MFC_VER < 0x700
	#include <../src/AfxImpl.h>
#else
	#ifndef __AFXSTATE_H__
		#include <../src/mfc/afxstat_.h>
	#endif
	#include <../src/mfc/AfxImpl.h>
#endif

#if (!defined __EXT_LOCALIZATION_H)
	#include <../Src/ExtLocalization.h>
#endif

#include <../profuisdll/resource.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtScrollBar

IMPLEMENT_DYNCREATE( CExtScrollBar, CScrollBar );

CExtScrollBar::CExtScrollBar()
	: m_bAutoDeleteOnPostNcDestroy( false )
	, m_bReflectParentSizing( true )
	, m_eSO( CExtScrollBar::__ESO_AUTO )
	, m_bDisableAutoReserveSpace( false )
	, m_bAutoReserveSpaceBefore( false )
	, m_bAutoReserveSpaceAfter( false )
	, m_bSmoothPainting( true )
	, m_hWndBefore( NULL )
	, m_hWndAfter( NULL )
	, m_bHideHwndBefore( false )
	, m_bHideHwndAfter( false )
{
	_ScanSysMertics();
}

CExtScrollBar::~CExtScrollBar()
{
}

BEGIN_MESSAGE_MAP(CExtScrollBar, CScrollBar)
	//{{AFX_MSG_MAP(CExtScrollBar)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	__EXT_MFC_SAFE_ON_WM_SETTINGCHANGE()
	ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
END_MESSAGE_MAP()

void CExtScrollBar::_ScanSysMertics()
{
	ASSERT_VALID( this );
	m_nHorzBarHeight	= ::GetSystemMetrics( SM_CYHSCROLL );
	m_nVertBarWidth		= ::GetSystemMetrics( SM_CXVSCROLL );
}

void CExtScrollBar::SyncReservedSpace( CExtScrollBar * pOtherBar )
{
	ASSERT_VALID( this );
	ASSERT_VALID( pOtherBar );
	ASSERT( this != pOtherBar );
CWnd * pWndNext = GetWindow( GW_HWNDNEXT );
	if( pWndNext != pOtherBar )
		return;
	m_bAutoReserveSpaceBefore = m_bAutoReserveSpaceAfter = false;
	pOtherBar->m_bAutoReserveSpaceBefore = pOtherBar->m_bAutoReserveSpaceAfter = false;
	if( m_bDisableAutoReserveSpace )
		return;
	if( (GetStyle()&WS_VISIBLE) == 0 || (pOtherBar->GetStyle()&WS_VISIBLE) == 0 )
		return;
eScrollerOrientation_t _eSOown = GetScrollerOrientation();
eScrollerOrientation_t _eSOother = pOtherBar->GetScrollerOrientation();
	switch( _eSOown )
	{
	case __ESO_TOP:
	case __ESO_BOTTOM:
		switch( _eSOother )
		{
		case __ESO_LEFT:
			m_bAutoReserveSpaceBefore = true;
			break;
		case __ESO_RIGHT:
			m_bAutoReserveSpaceAfter = true;
			break;
		case __ESO_TOP:
		case __ESO_BOTTOM:
		case __ESO_NONE:
			return;
#ifdef _DEBUG
		default:
			ASSERT( FALSE );
			return;
#endif // _DEBUG
		} // switch( _eSOother )
		break;
	case __ESO_LEFT:
	case __ESO_RIGHT:
		switch( _eSOother )
		{
		case __ESO_TOP:
			m_bAutoReserveSpaceBefore = true;
			break;
		case __ESO_BOTTOM:
			m_bAutoReserveSpaceAfter = true;
			break;
		case __ESO_LEFT:
		case __ESO_RIGHT:
		case __ESO_NONE:
			return;
#ifdef _DEBUG
		default:
			ASSERT( FALSE );
			return;
#endif // _DEBUG
		} // switch( _eSOother )
		break;
	case __ESO_NONE:
		return;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
		return;
#endif // _DEBUG
	} // switch( _eSOown )
}

CExtScrollBar::eScrollerOrientation_t
	CExtScrollBar::GetScrollerOrientation() const
{
	ASSERT_VALID( this );
	ASSERT( __ESO_MIN_VALUE <= m_eSO && m_eSO <= __ESO_MAX_VALUE );
eScrollerOrientation_t _eSO = m_eSO;
	if( _eSO == __ESO_AUTO )
	{
		DWORD dwWndStyle = CWnd::GetStyle();
		if( ( dwWndStyle & (SBS_TOPALIGN|SBS_VERT) ) == (SBS_TOPALIGN) )
			_eSO = __ESO_TOP;
		else if( ( dwWndStyle & (SBS_BOTTOMALIGN|SBS_VERT) ) == (SBS_BOTTOMALIGN) )
			_eSO = __ESO_BOTTOM;
		else if( ( dwWndStyle & (SBS_LEFTALIGN|SBS_VERT) ) == (SBS_LEFTALIGN|SBS_VERT) )
			_eSO = __ESO_LEFT;
		else if( ( dwWndStyle & (SBS_RIGHTALIGN|SBS_VERT) ) == (SBS_RIGHTALIGN|SBS_VERT) )
			_eSO = __ESO_RIGHT;
		else
			_eSO = __ESO_NONE;
	} // if( _eSO == __ESO_AUTO )
	return _eSO;
}

void CExtScrollBar::PreSubclassWindow()
{
	CScrollBar::PreSubclassWindow();
	_ScanSysMertics();
}

void CExtScrollBar::PostNcDestroy()
{
	ASSERT_VALID( this );
	if( m_bAutoDeleteOnPostNcDestroy )
		delete this;
}

BOOL CExtScrollBar::OnEraseBkgnd(CDC* pDC) 
{
	if( !m_bSmoothPainting )
		return CScrollBar::OnEraseBkgnd( pDC );
	return TRUE;
}

void CExtScrollBar::OnPaint() 
{
	if( !m_bSmoothPainting )
	{
		CScrollBar::OnPaint();
		return;
	} // if( !m_bSmoothPainting )
CRect rcClient;
	GetClientRect( &rcClient );
CPaintDC dcPaint( this );
CExtMemoryDC dc( &dcPaint, &rcClient );
	DefWindowProc( WM_PAINT, (WPARAM)dc.GetSafeHdc(), 0L );
}

void CExtScrollBar::OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection) 
{
	ASSERT_VALID( this );
	CScrollBar::OnSettingChange(uFlags, lpszSection);
	_ScanSysMertics();
}

LRESULT CExtScrollBar::OnSizeParent( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	if( !m_bReflectParentSizing )
		return (LRESULT)0;
	wParam;
DWORD dwWndStyle = CWnd::GetStyle();
	if( (dwWndStyle & WS_VISIBLE) == 0 )
		return 0;
AFX_SIZEPARENTPARAMS * lpLayout = (AFX_SIZEPARENTPARAMS*)lParam;
	ASSERT( lpLayout != NULL );

CRect rcOwnLayout( lpLayout->rect );
eScrollerOrientation_t _eSO = GetScrollerOrientation();
CSize _sizeNeeded( 0, 0 );
CRect	rcWndBeforeSrc( 0, 0, 0, 0 ),
		rcWndBeforeDst( 0, 0, 0, 0 ),
		rcWndAfterSrc( 0, 0, 0, 0 ),
		rcWndAfterDst( 0, 0, 0, 0 );
bool	bHwndBeforeVisible = false, bHwndAfterVisible = false,
		bForceHideBefore = false, bForceHideAfter = false;
HWND	hWndBefore = NULL, hWndAfter = NULL, hWndParent = NULL;
	if( lpLayout->hDWP != NULL )
	{
		if(		m_hWndBefore != NULL
			&&	::IsWindow( m_hWndBefore )
			)
		{
			bHwndBeforeVisible =
				( ::GetWindowLong( m_hWndBefore, GWL_STYLE ) & WS_VISIBLE )
					? true : false;
			if( ! m_bHideHwndBefore )
			{
				::GetWindowRect( m_hWndBefore, &rcWndBeforeSrc );
				if( hWndParent == NULL )
				{
					hWndParent = ::GetParent( m_hWnd );
					ASSERT( hWndParent != NULL && ::IsWindow(hWndParent) );
					ASSERT( hWndParent ==  ::GetParent( m_hWndBefore ) );
				} // if( hWndParent == NULL )
				::ScreenToClient( m_hWndBefore, LPPOINT(&rcWndBeforeSrc) );
				::ScreenToClient( m_hWndBefore, LPPOINT(&rcWndBeforeSrc) + 1 );
				rcWndBeforeDst = rcWndBeforeSrc;
			} // if( !m_bHideHwndBefore )
			hWndBefore = m_hWndBefore;
		}
		if(		m_hWndAfter != NULL
			&&	::IsWindow( m_hWndAfter )
			)
		{
			bHwndAfterVisible =
				( ::GetWindowLong( m_hWndAfter, GWL_STYLE ) & WS_VISIBLE )
					? true : false;
			if( ! m_bHideHwndAfter )
			{
				::GetWindowRect( m_hWndAfter, &rcWndAfterSrc );
				if( hWndParent == NULL )
				{
					hWndParent = ::GetParent( m_hWnd );
					ASSERT( hWndParent != NULL && ::IsWindow(hWndParent) );
					ASSERT( hWndParent ==  ::GetParent( m_hWndAfter ) );
				} // if( hWndParent == NULL )
				::ScreenToClient( m_hWndAfter, LPPOINT(&rcWndAfterSrc) );
				::ScreenToClient( m_hWndAfter, LPPOINT(&rcWndAfterSrc) + 1 );
				rcWndAfterDst = rcWndAfterSrc;
			} // if( !m_bHideHwndAfter )
			hWndAfter = m_hWndAfter;
		}
	} // if( lpLayout->hDWP != NULL )
int nMinExtent;
	switch( _eSO )
	{
	case __ESO_TOP:
		_sizeNeeded.cy = m_nHorzBarHeight;
		lpLayout->rect.top += _sizeNeeded.cy;
		rcOwnLayout.bottom = rcOwnLayout.top + _sizeNeeded.cy;
		lpLayout->sizeTotal.cy += _sizeNeeded.cy;
		if( m_bAutoReserveSpaceBefore )
			rcOwnLayout.left += m_nVertBarWidth;
		if( m_bAutoReserveSpaceAfter )
			rcOwnLayout.right -= m_nVertBarWidth;
		nMinExtent = ::GetSystemMetrics(SM_CXHSCROLL)*2;
		if( hWndAfter != NULL )
		{
			rcWndAfterDst.OffsetRect(
				rcOwnLayout.right - rcWndAfterDst.left - rcWndAfterDst.Width(),
				rcOwnLayout.top - rcWndAfterDst.top
				);
			if( rcWndAfterDst.left <= (rcOwnLayout.left + nMinExtent) )
				bForceHideAfter = true;
			else
			{
				rcWndAfterDst.bottom =
					rcWndAfterDst.top + rcOwnLayout.Height();
				rcOwnLayout.right -= rcWndAfterDst.Width();
			} // if( rcWndAfterDst.left <= (rcOwnLayout.left + nMinExtent) )
		} // if( hWndAfter != NULL )
		if( hWndBefore != NULL )
		{
			rcWndBeforeDst.OffsetRect(
				rcOwnLayout.left - rcWndBeforeDst.left,
				rcOwnLayout.top - rcWndBeforeDst.top
				);
			if( rcWndBeforeDst.right >= (rcOwnLayout.right - nMinExtent) )
				bForceHideBefore = true;
			else
			{
				rcWndBeforeDst.bottom =
					rcWndBeforeDst.top + rcOwnLayout.Height();
				rcOwnLayout.left += rcWndBeforeDst.Width();
			} // else from if( rcWndBeforeDst.right >= (rcOwnLayout.right - nMinExtent) )
		} // if( hWndBefore != NULL )
		break;
	case __ESO_BOTTOM:
		_sizeNeeded.cy = m_nHorzBarHeight;
		lpLayout->rect.bottom -= _sizeNeeded.cy;
		rcOwnLayout.top = rcOwnLayout.bottom - _sizeNeeded.cy;
		lpLayout->sizeTotal.cy += _sizeNeeded.cy;
		if( m_bAutoReserveSpaceBefore )
			rcOwnLayout.left += m_nVertBarWidth;
		if( m_bAutoReserveSpaceAfter )
			rcOwnLayout.right -= m_nVertBarWidth;
		nMinExtent = ::GetSystemMetrics(SM_CXHSCROLL)*2;
		if( hWndAfter != NULL )
		{
			rcWndAfterDst.OffsetRect(
				rcOwnLayout.right - rcWndAfterDst.left - rcWndAfterDst.Width(),
				rcOwnLayout.top - rcWndAfterDst.top
				);
			if( rcWndAfterDst.left <= (rcOwnLayout.left + nMinExtent) )
				bForceHideAfter = true;
			else
			{
				rcWndAfterDst.bottom =
					rcWndAfterDst.top + rcOwnLayout.Height();
				rcOwnLayout.right -= rcWndAfterDst.Width();
			} // if( rcWndAfterDst.left <= (rcOwnLayout.left + nMinExtent) )
		} // if( hWndAfter != NULL )
		if( hWndBefore != NULL )
		{
			rcWndBeforeDst.OffsetRect(
				rcOwnLayout.left - rcWndBeforeDst.left,
				rcOwnLayout.top - rcWndBeforeDst.top
				);
			if( rcWndBeforeDst.right >= (rcOwnLayout.right - nMinExtent) )
				bForceHideBefore = true;
			else
			{
				rcWndBeforeDst.bottom =
					rcWndBeforeDst.top + rcOwnLayout.Height();
				rcOwnLayout.left += rcWndBeforeDst.Width();
			} // else from if( rcWndBeforeDst.right >= (rcOwnLayout.right - nMinExtent) )
		} // if( hWndBefore != NULL )
		break;
	case __ESO_LEFT:
		_sizeNeeded.cx = m_nVertBarWidth;
		lpLayout->rect.left += _sizeNeeded.cx;
		rcOwnLayout.right = rcOwnLayout.left + _sizeNeeded.cx;
		lpLayout->sizeTotal.cx += _sizeNeeded.cx;
		if( m_bAutoReserveSpaceBefore )
			rcOwnLayout.top += m_nHorzBarHeight;
		if( m_bAutoReserveSpaceAfter )
			rcOwnLayout.bottom -= m_nHorzBarHeight;
		break;
	case __ESO_RIGHT:
		_sizeNeeded.cx = m_nVertBarWidth;
		lpLayout->rect.right -= _sizeNeeded.cx;
		rcOwnLayout.left = rcOwnLayout.right - _sizeNeeded.cx;
		lpLayout->sizeTotal.cx += _sizeNeeded.cx;
		if( m_bAutoReserveSpaceBefore )
			rcOwnLayout.top += m_nHorzBarHeight;
		if( m_bAutoReserveSpaceAfter )
			rcOwnLayout.bottom -= m_nHorzBarHeight;
		nMinExtent = ::GetSystemMetrics(SM_CYVSCROLL)*2;
		if( hWndAfter != NULL )
		{
			rcWndAfterDst.OffsetRect(
				rcOwnLayout.left - rcWndAfterDst.left,
				rcOwnLayout.bottom - rcWndAfterDst.top - rcWndAfterDst.Height()
				);
			if( rcWndAfterDst.top <= (rcOwnLayout.top + nMinExtent) )
				bForceHideAfter = true;
			else
			{
				rcWndAfterDst.right =
					rcWndAfterDst.left + rcOwnLayout.Width();
				rcOwnLayout.bottom -= rcWndAfterDst.Height();
			} // if( rcWndAfterDst.top <= (rcOwnLayout.top + nMinExtent) )
		} // if( hWndAfter != NULL )
		if( hWndBefore != NULL )
		{
			rcWndBeforeDst.OffsetRect(
				rcOwnLayout.left - rcWndBeforeDst.left,
				rcOwnLayout.top - rcWndBeforeDst.top
				);
			if( rcWndBeforeDst.bottom >= (rcOwnLayout.bottom - nMinExtent) )
				bForceHideBefore = true;
			else
			{
				rcWndBeforeDst.right =
					rcWndBeforeDst.left + rcOwnLayout.Width();
				rcOwnLayout.top += rcWndBeforeDst.Height();
			} // else from if( rcWndBeforeDst.bottom >= (rcOwnLayout.bottom - nMinExtent) )
		} // if( hWndBefore != NULL )
		break;
	case __ESO_NONE:
		break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
		break;
#endif // _DEBUG
	} // switch( _eSO )

//	ASSERT( ! rcOwnLayout.IsRectEmpty() );
	if( lpLayout->hDWP != NULL )
	{
		::AfxRepositionWindow(
			lpLayout,
			m_hWnd,
			&rcOwnLayout
			);
		if( hWndBefore != NULL )
		{
			if(		(m_bHideHwndBefore && bHwndBeforeVisible)
				||	bForceHideBefore
				)
				lpLayout->hDWP =
					::DeferWindowPos(
						lpLayout->hDWP,
						hWndBefore, NULL, 0, 0, 0, 0,
						SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE
							|SWP_NOZORDER|SWP_NOOWNERZORDER
							|SWP_HIDEWINDOW
						);
			else if( rcWndBeforeDst != rcWndBeforeSrc )
				lpLayout->hDWP =
					::DeferWindowPos(
						lpLayout->hDWP,
						hWndBefore, NULL,
						rcWndBeforeDst.left, rcWndBeforeDst.top,
						rcWndBeforeDst.Width(), rcWndBeforeDst.Height(),
						SWP_NOACTIVATE|SWP_SHOWWINDOW
							|SWP_NOZORDER|SWP_NOOWNERZORDER
						);
		} // if( hWndBefore != NULL )
		if( hWndAfter != NULL )
		{
			if(		(m_bHideHwndAfter && bHwndAfterVisible)
				||	bForceHideAfter
				)
				lpLayout->hDWP =
					::DeferWindowPos(
						lpLayout->hDWP,
						hWndAfter, NULL, 0, 0, 0, 0,
						SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE
							|SWP_NOZORDER|SWP_NOOWNERZORDER
							|SWP_HIDEWINDOW
						);
			else if( rcWndAfterDst != rcWndAfterSrc )
				lpLayout->hDWP =
					::DeferWindowPos(
						lpLayout->hDWP,
						hWndAfter, NULL,
						rcWndAfterDst.left, rcWndAfterDst.top,
						rcWndAfterDst.Width(), rcWndAfterDst.Height(),
						SWP_NOACTIVATE|SWP_SHOWWINDOW
							|SWP_NOZORDER|SWP_NOOWNERZORDER
						);
		} // if( hWndAfter != NULL )
	} // if( lpLayout->hDWP != NULL )
	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
// CExtScrollWnd

IMPLEMENT_DYNCREATE( CExtScrollWnd, CWnd );

CExtScrollWnd::CExtScrollWnd()
	: m_nUpdateScrollBars( 0 )
	, m_bScrollPhysical( false )
	, m_bEatErasing( true )
	, m_bBufferedPainting( true )
	, m_bScrollUpdateWindow( false )
	, m_bScrollInvalidate( true )
	, m_bScrollErase( false )
	, m_bRedrawUpdateWindow( false )
	, m_bRedrawInvalidate( true )
	, m_bRedrawErase( false )
	, m_bUse32BitScrollInfo( true )
{
}

CExtScrollWnd::~CExtScrollWnd()
{
}


BEGIN_MESSAGE_MAP(CExtScrollWnd, CWnd)
	//{{AFX_MSG_MAP(CExtScrollWnd)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_SYSCOLORCHANGE()
	__EXT_MFC_SAFE_ON_WM_SETTINGCHANGE()
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_MESSAGE(__ExtMfc_WM_THEMECHANGED, OnThemeChanged)
END_MESSAGE_MAP()

void CExtScrollWnd::OnSysColorChange() 
{
	ASSERT_VALID( this );
	CWnd::OnSysColorChange();
	g_PaintManager.OnSysColorChange( this );
	g_CmdManager.OnSysColorChange( this );
	Invalidate();
}

LRESULT CExtScrollWnd::OnDisplayChange( WPARAM wParam, LPARAM lParam )
{
LRESULT lResult = CWnd::OnDisplayChange( wParam, lParam );
	g_PaintManager.OnDisplayChange( this, (INT)wParam, CPoint(lParam) );
	g_CmdManager.OnDisplayChange( this, (INT)wParam, CPoint(lParam) );
	return lResult;
}

LRESULT CExtScrollWnd::OnThemeChanged( WPARAM wParam, LPARAM lParam )
{
LRESULT lResult = Default();
	g_PaintManager.OnThemeChanged( this, wParam, lParam );
	g_CmdManager.OnThemeChanged( this, wParam, lParam );
	OnSwRecalcLayout( true );
	return lResult;
}

void CExtScrollWnd::OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection) 
{
	ASSERT_VALID( this );
	CWnd::OnSettingChange(uFlags, lpszSection);
	g_PaintManager.OnSettingChange( this, uFlags, lpszSection );
	g_CmdManager.OnSettingChange( this, uFlags, lpszSection );
	OnSwRecalcLayout( true );
}

CSize CExtScrollWnd::OnSwGetTotalSize() const
{
	ASSERT_VALID( this );
	return CSize( 0, 0 );
}

CSize CExtScrollWnd::OnSwGetPageSize( int nDirection ) const
{
	ASSERT_VALID( this );
	nDirection;
	return CSize( 0, 0 );
}

CSize CExtScrollWnd::OnSwGetLineSize( int nDirection ) const
{
	ASSERT_VALID( this );
	nDirection;
	return CSize( 0, 0 );
}

bool CExtScrollWnd::OnSwScrollInfoAdjust(
	int nBar,
	SCROLLINFO & si,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( nBar == SB_HORZ || nBar == SB_VERT );
CScrollBar * pScrollBarWnd = GetScrollBarCtrl( nBar );
	if( pScrollBarWnd != NULL )
	{
		if( pScrollBarWnd->SetScrollInfo(
				&si,
				bRedraw ? TRUE : FALSE
				)
			)
			return true;
		return false;
	} // if( pScrollBarWnd != NULL )
	if( CWnd::SetScrollInfo(
			nBar,
			&si,
			bRedraw ? TRUE : FALSE
			)
		)
		return true;
	return false;
}

void CExtScrollWnd::OnSwSetScrollRange(
	int nBar,
	LONG nMinPos,
	LONG nMaxPos,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( nBar == SB_HORZ || nBar == SB_VERT );
CScrollBar * pScrollBarWnd = GetScrollBarCtrl( nBar );
	if( pScrollBarWnd != NULL )
	{
		pScrollBarWnd->SetScrollRange(
			(int)nMinPos,
			(int)nMaxPos,
			bRedraw ? TRUE : FALSE
			);
		return;
	} // if( pScrollBarWnd != NULL )
	CWnd::SetScrollRange(
		nBar,
		(int)nMinPos,
		(int)nMaxPos,
		bRedraw ? TRUE : FALSE
		);
}

LONG CExtScrollWnd::ScrollLimit32Get( int nBar ) const
{
	ASSERT_VALID( this );
	ASSERT( nBar == SB_HORZ || nBar == SB_VERT );
CScrollBar * pScrollBarWnd = GetScrollBarCtrl( nBar );
	if( pScrollBarWnd != NULL )
	{
		if( m_bUse32BitScrollInfo )
		{
			SCROLLINFO _scroll_info;
			::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
			_scroll_info.cbSize = sizeof(SCROLLINFO);
			if(	pScrollBarWnd->GetScrollInfo(
					&_scroll_info,
					SIF_RANGE|SIF_PAGE
					)
				)
			{
				LONG nRetVal = (LONG)
					(_scroll_info.nMax
					- _scroll_info.nMin
					- _scroll_info.nPage
					+ 1);
				ASSERT( nRetVal >= 0 );
				return nRetVal;
			}
			//ASSERT( FALSE );
		} // if( m_bUse32BitScrollInfo )
		LONG nRetVal = (LONG)
			pScrollBarWnd->GetScrollLimit();
		return nRetVal;
	} // if( pScrollBarWnd != NULL )
	if( m_bUse32BitScrollInfo )
	{
		SCROLLINFO _scroll_info;
		::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
		_scroll_info.cbSize = sizeof(SCROLLINFO);
		if(	( const_cast < CExtScrollWnd * > ( this ) ) ->
				CWnd::GetScrollInfo(
					nBar,
					&_scroll_info,
					SIF_RANGE|SIF_PAGE
					)
			)
		{
			LONG nRetVal = (LONG)
				(_scroll_info.nMax
				- _scroll_info.nMin
				- _scroll_info.nPage
				+ 1);
			ASSERT( nRetVal >= 0 );
			return nRetVal;
		}
		//ASSERT( FALSE );
	} // if( m_bUse32BitScrollInfo )
LONG nRetVal = (LONG)
		(	( const_cast < CExtScrollWnd * > ( this ) ) ->
				CWnd::GetScrollLimit( nBar )
		);
	return nRetVal;
}

void CExtScrollWnd::ScrollInfo32Get(
	int nBar,
	LONG * p_nMinPos,
	LONG * p_nMaxPos,
	LONG * p_nPageSize,
	LONG * p_nCurrPos,
	LONG * p_nTrackPos
	) const
{
	ASSERT_VALID( this );
	ASSERT( nBar == SB_HORZ || nBar == SB_VERT );
	if( p_nMinPos != NULL )
		*p_nMinPos = 0L;
	if( p_nMaxPos != NULL )
		*p_nMaxPos = 0L;
	if( p_nPageSize != NULL )
		*p_nPageSize = 0L;
	if( p_nCurrPos != NULL )
		*p_nCurrPos = 0L;
	if( p_nTrackPos != NULL )
		*p_nTrackPos = 0L;
CScrollBar * pScrollBarWnd = GetScrollBarCtrl( nBar );
	if( pScrollBarWnd != NULL )
	{
		if( m_bUse32BitScrollInfo )
		{
			SCROLLINFO _scroll_info;
			::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
			_scroll_info.cbSize = sizeof(SCROLLINFO);
			if(	pScrollBarWnd->GetScrollInfo(
					&_scroll_info,
					SIF_RANGE|SIF_PAGE|SIF_POS|SIF_TRACKPOS
					)
				)
			{
				if( p_nMinPos != NULL )
					*p_nMinPos = _scroll_info.nMin;
				if( p_nMaxPos != NULL )
					*p_nMaxPos = _scroll_info.nMax;
				if( p_nPageSize != NULL )
					*p_nPageSize = _scroll_info.nPage;
				if( p_nCurrPos != NULL )
					*p_nCurrPos = _scroll_info.nPos;
				if( p_nTrackPos != NULL )
					*p_nTrackPos = _scroll_info.nTrackPos;
				return;
			}
			//ASSERT( FALSE );
		} // if( m_bUse32BitScrollInfo )
		if( p_nMinPos != NULL || p_nMaxPos != NULL )
		{
			INT nMin = 0, nMax = 0;
			pScrollBarWnd->GetScrollRange( &nMin, &nMax );
			if( p_nMinPos != NULL )
				*p_nMinPos = nMin;
			if( p_nMaxPos != NULL )
				*p_nMaxPos = nMax;
			
		} // if( p_nMinPos != NULL || p_nMaxPos != NULL )
		if( p_nCurrPos != NULL || p_nTrackPos != NULL )
		{
			INT nPos = pScrollBarWnd->GetScrollPos();
			if( p_nCurrPos != NULL )
				*p_nCurrPos = nPos;
			if( p_nTrackPos != NULL )
				*p_nTrackPos = nPos;
		} // if( p_nCurrPos != NULL || p_nTrackPos != NULL )
		return;
	} // if( pScrollBarWnd != NULL )
	if( m_bUse32BitScrollInfo )
	{
		SCROLLINFO _scroll_info;
		::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
		_scroll_info.cbSize = sizeof(SCROLLINFO);
		if(	( const_cast < CExtScrollWnd * > ( this ) ) ->
				CWnd::GetScrollInfo(
					nBar,
					&_scroll_info,
					SIF_RANGE|SIF_PAGE|SIF_POS|SIF_TRACKPOS
					)
			)
		{
			if( p_nMinPos != NULL )
				*p_nMinPos = _scroll_info.nMin;
			if( p_nMaxPos != NULL )
				*p_nMaxPos = _scroll_info.nMax;
			if( p_nPageSize != NULL )
				*p_nPageSize = _scroll_info.nPage;
			if( p_nCurrPos != NULL )
				*p_nCurrPos = _scroll_info.nPos;
			if( p_nTrackPos != NULL )
				*p_nTrackPos = _scroll_info.nTrackPos;
			return;
		}
		//ASSERT( FALSE );
	} // if( m_bUse32BitScrollInfo )
	if( p_nMinPos != NULL || p_nMaxPos != NULL )
	{
		INT nMin = 0, nMax = 0;
		CWnd::GetScrollRange( nBar, &nMin, &nMax );
		if( p_nMinPos != NULL )
			*p_nMinPos = nMin;
		if( p_nMaxPos != NULL )
			*p_nMaxPos = nMax;
		
	} // if( p_nMinPos != NULL || p_nMaxPos != NULL )
	if( p_nCurrPos != NULL || p_nTrackPos != NULL )
	{
		INT nPos = CWnd::GetScrollPos( nBar );
		if( p_nCurrPos != NULL )
			*p_nCurrPos = nPos;
		if( p_nTrackPos != NULL )
			*p_nTrackPos = nPos;
	} // if( p_nCurrPos != NULL || p_nTrackPos != NULL )
	return;
}

LONG CExtScrollWnd::ScrollPos32Get(
	int nBar,
	bool bTrackPos // = false
	) const
{
	ASSERT_VALID( this );
	ASSERT( nBar == SB_HORZ || nBar == SB_VERT );
CScrollBar * pScrollBarWnd = GetScrollBarCtrl( nBar );
	if( pScrollBarWnd != NULL )
	{
		if( m_bUse32BitScrollInfo )
		{
			SCROLLINFO _scroll_info;
			::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
			_scroll_info.cbSize = sizeof(SCROLLINFO);
			if(	pScrollBarWnd->GetScrollInfo(
					&_scroll_info,
					bTrackPos ? SIF_TRACKPOS : SIF_POS
					)
				)
			{
				LONG nRetVal = (LONG)
					(	bTrackPos
							? _scroll_info.nTrackPos
							: _scroll_info.nPos
					);
				return nRetVal;
			}
			//ASSERT( FALSE );
		} // if( m_bUse32BitScrollInfo )
		LONG nRetVal = (LONG)
			pScrollBarWnd->GetScrollPos();
		return nRetVal;
	} // if( pScrollBarWnd != NULL )
DWORD dwWndStyle = CWnd::GetStyle();
	if(		( nBar == SB_HORZ && (dwWndStyle & WS_HSCROLL) == 0 )
		||	( nBar == SB_VERT && (dwWndStyle & WS_VSCROLL) == 0 )
		)
		return 0;
	if( m_bUse32BitScrollInfo )
	{
		SCROLLINFO _scroll_info;
		::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
		_scroll_info.cbSize = sizeof(SCROLLINFO);
		if( ( const_cast < CExtScrollWnd * > ( this ) ) ->
				CWnd::GetScrollInfo(
					nBar,
					&_scroll_info,
					bTrackPos ? SIF_TRACKPOS : SIF_POS
					)
				)
		{
			LONG nRetVal = (LONG)
				(	bTrackPos
						? _scroll_info.nTrackPos
						: _scroll_info.nPos
				);
			return nRetVal;
		}
		//ASSERT( FALSE );
	} // if( m_bUse32BitScrollInfo )
LONG nRetVal = (LONG)
		(	( const_cast < CExtScrollWnd * > ( this ) ) ->
				CWnd::GetScrollPos( nBar )
		);
	return nRetVal;
}

void CExtScrollWnd::ScrollPos32Set(
	int nBar,
	LONG nPos,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( nBar == SB_HORZ || nBar == SB_VERT );
CScrollBar * pScrollBarWnd = GetScrollBarCtrl( nBar );
	if( pScrollBarWnd != NULL )
	{
		if( m_bUse32BitScrollInfo )
		{
			SCROLLINFO _scroll_info;
			::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
			_scroll_info.cbSize = sizeof(SCROLLINFO);
			_scroll_info.fMask = SIF_POS;
			_scroll_info.nPos = (int)nPos;
			if( pScrollBarWnd->SetScrollInfo(
					&_scroll_info,
					bRedraw ? TRUE : FALSE
					)
				)
				return;
			//ASSERT( FALSE );
		} // if( m_bUse32BitScrollInfo )
		pScrollBarWnd->SetScrollPos(
			(int)nPos,
			bRedraw ? TRUE : FALSE
			);
		return;
	} // if( pScrollBarWnd != NULL )
	if( m_bUse32BitScrollInfo )
	{
		SCROLLINFO _scroll_info;
		::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
		_scroll_info.cbSize = sizeof(SCROLLINFO);
		_scroll_info.fMask = SIF_POS;
		_scroll_info.nPos = (int)nPos;
		if( CWnd::SetScrollInfo(
				nBar,
				&_scroll_info,
				bRedraw ? TRUE : FALSE
				)
			)
			return;
		//ASSERT( FALSE );
	} // if( m_bUse32BitScrollInfo )
	CWnd::SetScrollPos(
		nBar,
		(int)nPos,
		bRedraw ? TRUE : FALSE
		);
}

CPoint CExtScrollWnd::OnSwGetScrollPos() const
{
	ASSERT_VALID( this );
CPoint pt(
		ScrollPos32Get( SB_HORZ ),
		ScrollPos32Get( SB_VERT )
		);
	ASSERT( pt.x >= 0 && pt.y >= 0 );
	return pt;
}

CPoint CExtScrollWnd::OnSwGetScrollPaintPos() const
{
	ASSERT_VALID( this );
	return OnSwGetScrollPos();
}

CRect CExtScrollWnd::OnSwRecalcLayout(
	bool bDoLayout,
	LPCRECT pRectClientSrc // = NULL
	)
{
	ASSERT_VALID( this );
	if( bDoLayout )
	{
		CScrollBar * pScrollBarWndH = GetScrollBarCtrl( SB_HORZ );
		CScrollBar * pScrollBarWndV = GetScrollBarCtrl( SB_VERT );
		if( pScrollBarWndH != NULL && pScrollBarWndV != NULL )
		{
			CExtScrollBar * pExtScrollBarWndH = DYNAMIC_DOWNCAST( CExtScrollBar, pScrollBarWndH );
			CExtScrollBar * pExtScrollBarWndV = DYNAMIC_DOWNCAST( CExtScrollBar, pScrollBarWndV );
			if( pExtScrollBarWndH != NULL && pExtScrollBarWndV != NULL )
			{
				pExtScrollBarWndH->SyncReservedSpace( pExtScrollBarWndV );
				pExtScrollBarWndV->SyncReservedSpace( pExtScrollBarWndH );
			} // if( pExtScrollBarWndH != NULL && pExtScrollBarWndV != NULL )
		} // if( pScrollBarWndH != NULL && pScrollBarWndV != NULL )
	} // if( bDoLayout )
CRect rcClient;
	if( pRectClientSrc != NULL )
		rcClient = *pRectClientSrc;
	else
		CWnd::GetClientRect( &rcClient );
	CWnd::RepositionBars(
		0,
		0x0FFFF,
		AFX_IDW_PANE_FIRST,
		bDoLayout ? CWnd::reposDefault : CWnd::reposQuery,
		&rcClient,
		&rcClient,
		TRUE
		);
	return rcClient;
}

CRect CExtScrollWnd::OnSwGetClientRect() const
{
	ASSERT_VALID( this );
CRect rcClient =
		( const_cast < CExtScrollWnd * > ( this ) )
			-> OnSwRecalcLayout( false );
	return rcClient;
}

bool CExtScrollWnd::OnSwHasScrollBar( bool bHorz ) const
{
	ASSERT_VALID( this );
CScrollBar * pScrollBarWnd = GetScrollBarCtrl( bHorz ? SB_HORZ : SB_VERT );
	if( pScrollBarWnd != NULL )
	{
		if( pScrollBarWnd->IsWindowEnabled() )
			return true;
		return false;
	} // if( pScrollBarWnd != NULL )
DWORD dwWndStyle = CWnd::GetStyle();
DWORD dwTestStyle = bHorz ? WS_HSCROLL : WS_VSCROLL;
	if( ( dwWndStyle & dwTestStyle ) != 0 )
		return true;
	return false;
}

bool CExtScrollWnd::OnSwCanAutoHideScrollBar( bool bHorz ) const
{
	ASSERT_VALID( this );
	bHorz;
	return true;
}

void CExtScrollWnd::OnSwEnableScrollBarCtrl( int nBar, bool bEnable )
{
	ASSERT_VALID( this );
	ASSERT( nBar == SB_HORZ || nBar == SB_VERT || nBar == SB_BOTH );
DWORD dwWndStyle = CWnd::GetStyle();
CPoint ptMove = OnSwGetScrollPos();
CScrollBar * pScrollBarWndH = GetScrollBarCtrl( SB_HORZ );
CScrollBar * pScrollBarWndV = GetScrollBarCtrl( SB_VERT );
	if( nBar == SB_HORZ || nBar == SB_BOTH )
	{
		if( (!bEnable) && ptMove.x != 0 )
		{
			ptMove.x = 0;
			OnSwSetScrollPos( ptMove );
		} // if( (!bEnable) && ptMove.x != 0 )
		if( pScrollBarWndH != NULL )
		{
			if( (dwWndStyle & WS_HSCROLL) != 0 )
				CWnd::ShowScrollBar( SB_HORZ, FALSE );
			bool bAreadyEnabled = pScrollBarWndH->IsWindowEnabled() ? true : false;
			if(	!(		( bAreadyEnabled && bEnable )
					||	( (!bAreadyEnabled) && (!bEnable) )
					)
				)
			{
				pScrollBarWndH->EnableWindow( bEnable ? TRUE : FALSE );
				if( OnSwCanAutoHideScrollBar(true) )
					pScrollBarWndH->ShowWindow( bEnable ? SW_SHOW : SW_HIDE );
				else if( bEnable && (pScrollBarWndH->GetStyle()&WS_VISIBLE) == 0 )
					pScrollBarWndH->ShowWindow( SW_SHOW );
			}
		} // if( pScrollBarWndH != NULL )
		else
		{
			if( OnSwCanAutoHideScrollBar(true) )
				CWnd::ShowScrollBar(
					SB_HORZ,
					bEnable ? TRUE : FALSE
					);
			else
				CWnd::EnableScrollBar(
					SB_HORZ,
					bEnable ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH
					);
		} // else from if( pScrollBarWndH != NULL )
	} // if( nBar == SB_HORZ || nBar == SB_BOTH )
	if( nBar == SB_VERT || nBar == SB_BOTH )
	{
		if( (!bEnable) && ptMove.y != 0 )
		{
			ptMove.y = 0;
			OnSwSetScrollPos( ptMove );
		} // if( (!bEnable) && ptMove.y != 0 )
		if( pScrollBarWndV != NULL )
		{
			if( (dwWndStyle & WS_VSCROLL) != 0 )
				CWnd::ShowScrollBar( SB_VERT, FALSE );
			bool bAreadyEnabled = pScrollBarWndV->IsWindowEnabled() ? true : false;
			if(	!(		( bAreadyEnabled && bEnable )
					||	( (!bAreadyEnabled) && (!bEnable) )
					)
				)
			{
				pScrollBarWndV->EnableWindow( bEnable ? TRUE : FALSE );
				if( OnSwCanAutoHideScrollBar(false) )
					pScrollBarWndV->ShowWindow( bEnable ? SW_SHOW : SW_HIDE );
				else if( bEnable && (pScrollBarWndV->GetStyle()&WS_VISIBLE) == 0 )
					pScrollBarWndV->ShowWindow( SW_SHOW );
			}
		} // if( pScrollBarWndV != NULL )
		else
		{
			if( OnSwCanAutoHideScrollBar(false) )
				CWnd::ShowScrollBar(
					SB_VERT,
					bEnable ? TRUE : FALSE
					);
			else
				CWnd::EnableScrollBar(
					SB_VERT,
					bEnable ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH
					);
		} // else from if( pScrollBarWndV != NULL )
	} // if( nBar == SB_VERT || nBar == SB_BOTH )
CExtScrollBar * pExtScrollBarWndH = DYNAMIC_DOWNCAST( CExtScrollBar, pScrollBarWndH );
CExtScrollBar * pExtScrollBarWndV = DYNAMIC_DOWNCAST( CExtScrollBar, pScrollBarWndV );
	if( pExtScrollBarWndH != NULL && pExtScrollBarWndV != NULL )
	{
		pExtScrollBarWndH->SyncReservedSpace( pExtScrollBarWndV );
		pExtScrollBarWndV->SyncReservedSpace( pExtScrollBarWndH );
	} // if( pExtScrollBarWndH != NULL && pExtScrollBarWndV != NULL )
}

void CExtScrollWnd::OnSwSetScrollPos( POINT pt )
{
	ASSERT_VALID( this );
	ASSERT( pt.x >= 0 && pt.y >= 0 );
int xOrgValue = ScrollPos32Get( SB_HORZ );
	ScrollPos32Set( SB_HORZ, pt.x );
int yOrgValue = ScrollPos32Get( SB_VERT );
	ScrollPos32Set( SB_VERT, pt.y );
	OnSwDoScrollWindow(
		xOrgValue - pt.x,
		yOrgValue - pt.y
		);
}

UINT CExtScrollWnd::OnSwCalcMouseScrollLines(
	UINT fFlags,
	short zDelta,
	CPoint point
	)
{
	ASSERT_VALID( this );
	fFlags;
	zDelta;
	point;
	return g_PaintManager.GetMouseWheelScrollLines();
}

bool CExtScrollWnd::OnSwDoMouseWheel(
	UINT fFlags,
	short zDelta,
	CPoint point
	)
{
	ASSERT_VALID( this );
	point;
	if(		(fFlags&(MK_CONTROL)) != 0
		||	CExtToolControlBar::g_bMenuTracking
		||	CExtPopupMenuWnd::IsMenuTracking()
		)
		return false;

	if( ! ( OnSwHasScrollBar(true) || OnSwHasScrollBar(false) ) )
		return false;
bool bResult = false;
UINT uWheelScrollLines =
		OnSwCalcMouseScrollLines(
			fFlags,
			zDelta,
			point
			);
	if(		OnSwHasScrollBar(false)
		&&	( (fFlags&(MK_SHIFT)) == 0 )
		)
	{
		int nDisplacement;
		int nToScroll =
			::MulDiv( -zDelta, uWheelScrollLines, WHEEL_DELTA );
		if(		nToScroll == -1
			||	uWheelScrollLines == WHEEL_PAGESCROLL
			)
		{
			nDisplacement = OnSwGetPageSize( -1 ).cy;
			if( zDelta > 0 )
				nDisplacement = -nDisplacement;
		}
		else
		{
			nDisplacement =
				nToScroll * OnSwGetLineSize( 1 ).cy;
			nDisplacement =
				min(
					nDisplacement,
					OnSwGetPageSize( 1 ).cy
					);
		}
		bResult =
			OnSwDoScrollBy(
				CSize( 0, nDisplacement ),
				true
				);
	}
	else if(
			OnSwHasScrollBar( true )
		&&	( (fFlags&(MK_SHIFT)) != 0 )
		)
	{
		int nDisplacement;
		int nToScroll =
			::MulDiv( -zDelta, uWheelScrollLines, WHEEL_DELTA );
		if(		nToScroll == -1
			||	uWheelScrollLines == WHEEL_PAGESCROLL
			)
		{
			nDisplacement = OnSwGetPageSize( -1 ).cx;
			if( zDelta > 0 )
				nDisplacement = -nDisplacement;
		}
		else
		{
			nDisplacement =
				nToScroll * OnSwGetLineSize( 1 ).cx;
			nDisplacement =
				min(
					nDisplacement,
					OnSwGetPageSize( 1 ).cx
					);
		}
		bResult =
			OnSwDoScrollBy(
				CSize( nDisplacement, 0 ),
				true
				);
	}

	if( bResult )
	{
		OnSwInvalidate( m_bScrollErase );
		if( m_bScrollUpdateWindow )
			CWnd::UpdateWindow();
	}

	return bResult;
}

bool CExtScrollWnd::OnSwDoScroll(
	UINT nScrollCode,
	UINT nPos,
	bool bDoScroll // = true
	)
{
	ASSERT_VALID( this );
LONG xPos, xOrgValue, yPos, yOrgValue;
	xOrgValue = xPos = ScrollPos32Get( SB_HORZ );
	switch( LOBYTE(nScrollCode) )
	{
	case SB_TOP:		xPos  = 0;							break;
	case SB_BOTTOM:		xPos  = __EXT_SCROLL_NUMERIC_MAX;	break;
	case SB_LINEUP:		xPos -= OnSwGetLineSize( -1 ).cx;	break;
	case SB_LINEDOWN:	xPos += OnSwGetLineSize(  1 ).cx;	break;
	case SB_PAGEUP:		xPos -= OnSwGetPageSize( -1 ).cx;	break;
	case SB_PAGEDOWN:	xPos += OnSwGetPageSize(  1 ).cx;	break;
	case SB_THUMBTRACK:
		if( ! OnSwQueryThumbTrackEnabled(true) )
			break;
	case SB_THUMBPOSITION:
			if( m_bUse32BitScrollInfo )
				nPos = ScrollPos32Get( SB_HORZ, true );
			xPos  = nPos;
		break;
	}
	yOrgValue = yPos = ScrollPos32Get( SB_VERT );
	switch( HIBYTE(nScrollCode) )
	{
	case SB_TOP:		yPos  = 0;							break;
	case SB_BOTTOM:		yPos  = __EXT_SCROLL_NUMERIC_MAX;	break;
	case SB_LINEUP:		yPos -= OnSwGetLineSize( -1 ).cy;	break;
	case SB_LINEDOWN:	yPos += OnSwGetLineSize(  1 ).cy;	break;
	case SB_PAGEUP:		yPos -= OnSwGetPageSize( -1 ).cy;	break;
	case SB_PAGEDOWN:	yPos += OnSwGetPageSize(  1 ).cy;	break;
	case SB_THUMBTRACK:
		if( ! OnSwQueryThumbTrackEnabled(false) )
			break;
	case SB_THUMBPOSITION:
			if( m_bUse32BitScrollInfo )
				nPos = ScrollPos32Get( SB_VERT, true );
			yPos  = nPos;
		break;
	}
bool bResult =
		OnSwDoScrollBy(
			CSize( xPos - xOrgValue, yPos - yOrgValue ),
			bDoScroll
			);
	if( bResult && bDoScroll && m_bScrollUpdateWindow )
		CWnd::UpdateWindow();
	return bResult;
}

bool CExtScrollWnd::OnSwDoScrollBy(
	CSize sizeScroll,
	bool bDoScroll // = true
	)
{
	ASSERT_VALID( this );
int xMaxValue, xOrgValue, xPos, yMaxValue, yOrgValue, yPos;
	if( ! OnSwHasScrollBar(false) )
		sizeScroll.cy = 0;
	if( ! OnSwHasScrollBar(true) )
		sizeScroll.cx = 0;
	// adjust current positions
	xOrgValue = xPos = ScrollPos32Get( SB_HORZ );
	xMaxValue = ScrollLimit32Get( SB_HORZ );
	xPos += sizeScroll.cx;
	if( xPos < 0 )
		xPos = 0;
	else if( xPos > xMaxValue )
		xPos = xMaxValue;
	yOrgValue = yPos = ScrollPos32Get( SB_VERT );
	yMaxValue = ScrollLimit32Get( SB_VERT );
	yPos += sizeScroll.cy;
	if( yPos < 0 )
		yPos = 0;
	else if( yPos > yMaxValue )
		yPos = yMaxValue;
	if( xPos == xOrgValue && yPos == yOrgValue )
		return false;
	if( bDoScroll )
	{
		OnSwDoScrollWindow(
			xOrgValue - xPos,
			yOrgValue - yPos
			);
		if( xPos != xOrgValue )
			ScrollPos32Set( SB_HORZ, xPos );
		if( yPos != yOrgValue )
			ScrollPos32Set( SB_VERT, yPos );
	}
	return true;
}

CSize CExtScrollWnd::OnSwGetScrollBarSizes() const
{
	ASSERT_VALID( this );
CSize sizeSb( 0, 0 );
DWORD dwWndStyle = CWnd::GetStyle();
	if( GetScrollBarCtrl(SB_HORZ) == NULL )
	{
		sizeSb.cx = afxData.cxVScroll;
		if( dwWndStyle & WS_BORDER )
			sizeSb.cx -= CX_BORDER;
	} // if( GetScrollBarCtrl(SB_HORZ) == NULL )
	if( GetScrollBarCtrl(SB_VERT) == NULL )
	{
		sizeSb.cy = afxData.cyHScroll;
		if( dwWndStyle & WS_BORDER )
			sizeSb.cy -= CY_BORDER;
	} // if( GetScrollBarCtrl(SB_VERT) == NULL )
	return sizeSb;
}

bool CExtScrollWnd::OnSwCalcClientSizes(
	CSize & size,
	CSize & sizeSb
	)
{
	// return true if enough room to add scrollbars if needed
	ASSERT_VALID( this );
CRect rcClient = OnSwGetClientRect();
	size = rcClient.Size();
	sizeSb = OnSwGetScrollBarSizes();
	if(		sizeSb.cx != 0
		&&	OnSwHasScrollBar( false )
		)
		size.cx += sizeSb.cx;
	if(		sizeSb.cy != 0
		&&	OnSwHasScrollBar( true )
		)
		size.cy += sizeSb.cy;
	return
		( size.cx > sizeSb.cx && size.cy > sizeSb.cy )
			? true : false;
}

bool CExtScrollWnd::OnSwQueryThumbTrackEnabled( bool bHorz ) const
{
	ASSERT_VALID( this );
	bHorz;
	return true;
}

void CExtScrollWnd::OnSwGetScrollBarState(
	CSize sizeClient,
	CSize & sizeNeedSB,
	CSize & sizeRange,
	CPoint & ptMove,
	bool bInsideClient
	) const
{
	ASSERT_VALID( this );
CSize sizeSb = OnSwGetScrollBarSizes();
	sizeRange = OnSwGetTotalSize() - sizeClient;
	ptMove = OnSwGetScrollPos();
bool bNeedH = sizeRange.cx > 0;
	if( !bNeedH )
		ptMove.x = 0;
	else if( bInsideClient )
		sizeRange.cy += sizeSb.cy;
bool bNeedV = sizeRange.cy > 0;
	if( !bNeedV )
		ptMove.y = 0;
	else if ( bInsideClient )
		sizeRange.cx += sizeSb.cx;
	if( bNeedV && (!bNeedH) && sizeRange.cx > 0 )
	{
		ASSERT( bInsideClient );
		bNeedH = true;
		sizeRange.cy += sizeSb.cy;
	} // if( bNeedV && (!bNeedH) && sizeRange.cx > 0 )
	if( sizeRange.cx > 0 && ptMove.x >= sizeRange.cx )
		ptMove.x = sizeRange.cx;
	if( sizeRange.cy > 0 && ptMove.y >= sizeRange.cy )
		ptMove.y = sizeRange.cy;
	sizeNeedSB.cx = bNeedH;
	sizeNeedSB.cy = bNeedV;
}

void CExtScrollWnd::OnSwUpdateScrollBars()
{
	ASSERT_VALID( this );
	if( m_nUpdateScrollBars > 1 )
		return;
	m_nUpdateScrollBars ++;

	ASSERT( OnSwGetTotalSize().cx >= 0 && OnSwGetTotalSize().cy >= 0 );
CRect rcClient;
bool bCalcClient = true;
CWnd * pParentWnd = GetParent();
	if(		pParentWnd != NULL
		&&	(BOOL)pParentWnd->SendMessage(
				WM_RECALCPARENT,
				0,
				(LPARAM)(LPCRECT)&rcClient
				) != 0
		)
		bCalcClient = false;
CSize sizeClient;
CSize sizeSb;
	if( bCalcClient )
	{
		if( !OnSwCalcClientSizes( sizeClient, sizeSb ) )
		{ // no room for scroll bars
			CRect rcClient2 = OnSwGetClientRect();
			if( rcClient2.Width() > 0 && rcClient2.Height() > 0 )
			{
				OnSwEnableScrollBarCtrl( SB_BOTH, false );
				OnSwRecalcLayout( true );
			}
			m_nUpdateScrollBars --;
			return;
		}
	}
	else
	{ // let parent window determine the "client" rect
		sizeSb = OnSwGetScrollBarSizes();
		sizeClient.cx = rcClient.right - rcClient.left;
		sizeClient.cy = rcClient.bottom - rcClient.top;
	}

	// if enough room to add scrollbars
CSize sizeRange;
CPoint ptMove;
CSize sizeNeedSB;
	OnSwGetScrollBarState(
		sizeClient,
		sizeNeedSB,
		sizeRange,
		ptMove,
		bCalcClient
		);
	if( sizeNeedSB.cx )
		sizeClient.cy -= sizeSb.cy;
	if( sizeNeedSB.cy )
		sizeClient.cx -= sizeSb.cx;

	// scroll window + update
	OnSwSetScrollPos( ptMove );

	// the scrollbar page range
SCROLLINFO _scroll_info;
	::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
	_scroll_info.cbSize = sizeof(SCROLLINFO);
	_scroll_info.fMask = SIF_PAGE|SIF_RANGE;
	_scroll_info.nMin = 0;
	// update bars
	OnSwEnableScrollBarCtrl( SB_HORZ, sizeNeedSB.cx ? true : false );
	if( sizeNeedSB.cx )
	{
		_scroll_info.nPage = sizeClient.cx;
		_scroll_info.nMax = OnSwGetTotalSize().cx - 1;
		if( ! OnSwScrollInfoAdjust(
				SB_HORZ,
				_scroll_info,
				true
				)
			)
			OnSwSetScrollRange(
				SB_HORZ,
				0,
				sizeRange.cx,
				true
				);
	} // if( sizeNeedSB.cx )
	OnSwEnableScrollBarCtrl( SB_VERT, sizeNeedSB.cy ? true : false );
	if( sizeNeedSB.cy )
	{
		_scroll_info.nPage = sizeClient.cy;
		_scroll_info.nMax = OnSwGetTotalSize().cy - 1;
		if( ! OnSwScrollInfoAdjust(
				SB_VERT,
				_scroll_info,
				true
				)
			)
			OnSwSetScrollRange(
				SB_VERT,
				0,
				sizeRange.cy,
				true
				);
	} // if( sizeNeedSB.cy )
	OnSwRecalcLayout( true );
	m_nUpdateScrollBars --;
}

void CExtScrollWnd::OnSwDoScrollWindow(
	int xAmount,
	int yAmount,
	LPCRECT lpRect, // = NULL
	LPCRECT lpClipRect // = NULL
	)
{
	ASSERT_VALID( this );
	if( m_bScrollPhysical )
		CWnd::ScrollWindow(
			xAmount,
			yAmount,
			lpRect,
			lpClipRect
			);
	if( m_bScrollInvalidate )
		OnSwInvalidate( m_bScrollErase );
	if( m_bScrollUpdateWindow )
		CWnd::UpdateWindow();
}

void CExtScrollWnd::OnSwInvalidate( bool bErase )
{
	ASSERT_VALID( this );
CScrollBar * pScrollBarWndH = GetScrollBarCtrl( SB_HORZ );
CScrollBar * pScrollBarWndV = GetScrollBarCtrl( SB_VERT );
	if(		pScrollBarWndH != NULL
		&&	( pScrollBarWndH->GetStyle() & WS_VISIBLE ) == 0
		)
		pScrollBarWndH = NULL;
	if(		pScrollBarWndV != NULL
		&&	( pScrollBarWndV->GetStyle() & WS_VISIBLE ) == 0
		)
		pScrollBarWndV = NULL;
	if( pScrollBarWndH == NULL && pScrollBarWndV == NULL )
	{
		CWnd::Invalidate( bErase ? TRUE : FALSE );
		return;
	} // if( pScrollBarWndH == NULL && pScrollBarWndV == NULL )
CRect rcClientReal;
	CWnd::GetClientRect( &rcClientReal );
CRgn rgnInvalidate;
	if( !rgnInvalidate.CreateRectRgnIndirect(&rcClientReal) )
	{
		CWnd::Invalidate( bErase ? TRUE : FALSE );
		return;
	}
	if( pScrollBarWndH != NULL )
	{
		CRect rcBar;
		pScrollBarWndH->GetWindowRect( &rcBar );
		CWnd::ScreenToClient( &rcBar );
		CRgn rgnBar;
		if(		(! rgnBar.CreateRectRgnIndirect(&rcBar) )
			||	rgnInvalidate.CombineRgn(
					&rgnInvalidate,
					&rgnBar,
					RGN_DIFF
					) == ERROR
			)
		{
			CWnd::Invalidate( bErase ? TRUE : FALSE );
			return;
		}
	} // if( pScrollBarWndH != NULL )
	if( pScrollBarWndV != NULL )
	{
		CRect rcBar;
		pScrollBarWndV->GetWindowRect( &rcBar );
		CWnd::ScreenToClient( &rcBar );
		CRgn rgnBar;
		if(		(! rgnBar.CreateRectRgnIndirect(&rcBar) )
			||	rgnInvalidate.CombineRgn(
					&rgnInvalidate,
					&rgnBar,
					RGN_DIFF
					) == ERROR
			)
		{
			CWnd::Invalidate( bErase ? TRUE : FALSE );
			return;
		}
	} // if( pScrollBarWndV != NULL )
	CWnd::InvalidateRgn( &rgnInvalidate, bErase ? TRUE : FALSE );
}

void CExtScrollWnd::OnSwDoRedraw()
{
	ASSERT_VALID( this );
	if( m_bRedrawInvalidate )
		OnSwInvalidate( m_bRedrawErase );
	if( m_bRedrawUpdateWindow )
		CWnd::UpdateWindow();
}

void CExtScrollWnd::OnSwPaint( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
}

/////////////////////////////////////////////////////////////////////////////
// CExtScrollWnd message handlers

void CExtScrollWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if( nType != SIZE_MINIMIZED )
	{
		OnSwRecalcLayout( true );
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	}
}

void CExtScrollWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(		pScrollBar != NULL
		&&	pScrollBar->SendChildNotifyLastMsg()
		)
		return;
	if( pScrollBar != GetScrollBarCtrl(SB_HORZ) )
		return;
	OnSwDoScroll( MAKEWORD(nSBCode, -1), nPos );
}

void CExtScrollWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(		pScrollBar != NULL
		&&	pScrollBar->SendChildNotifyLastMsg()
		)
		return;
	if( pScrollBar != GetScrollBarCtrl(SB_VERT) )
		return;
	OnSwDoScroll( MAKEWORD(-1, nSBCode), nPos );
}

BOOL CExtScrollWnd::OnMouseWheel(UINT fFlags, short zDelta, CPoint point)
{
	return
		OnSwDoMouseWheel( fFlags, zDelta, point )
			? true : false;
}

BOOL CExtScrollWnd::OnEraseBkgnd(CDC* pDC) 
{
	if( m_bEatErasing )
		return TRUE;
	return CWnd::OnEraseBkgnd(pDC);
}

void CExtScrollWnd::OnPaint() 
{
bool bSingleBufferPaint = false;
CPaintDC dcPaint( this );
CExtPaintManager::stat_ExcludeChildAreas(
		dcPaint.m_hDC,
		m_hWnd,
		CExtPaintManager::stat_DefExcludeChildAreaCallback
		);
CPoint ptSp = OnSwGetScrollPaintPos();
	if( m_bBufferedPainting )
	{
		CRect rcRealClient;
		CWnd::GetClientRect( &rcRealClient );
		CExtMemoryDC dcCompat( &dcPaint, &rcRealClient );
		ASSERT( dcCompat.GetSafeHdc() != NULL );
		if( dcCompat.GetSafeHdc() != NULL )
		{
			CPoint ptVpOld( dcCompat.SetViewportOrg(-ptSp) );
			OnSwPaint( dcCompat );
			dcCompat.SetViewportOrg( ptVpOld );
		} // if( dcCompat.GetSafeHdc() != NULL )
		else
			bSingleBufferPaint = true;
	} // if( m_bBufferedPainting )
	else
		bSingleBufferPaint = true;
	if( bSingleBufferPaint )
	{
		CPoint ptVpOld( dcPaint.SetViewportOrg(-ptSp) );
		OnSwPaint( dcPaint );
		dcPaint.SetViewportOrg( ptVpOld );
	} // if( bSingleBufferPaint )
}

BOOL CExtScrollWnd::PreTranslateMessage(MSG* pMsg) 
{
	if(		pMsg->message == WM_MOUSEWHEEL
		&&	GetSafeHwnd() != NULL
		&&	::IsWindow( GetSafeHwnd() )
		&&	CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND(
				GetSafeHwnd()
				)
		)
	{
		CPoint point = pMsg->lParam;
		HWND hWnd = ::WindowFromPoint( point );
		if( hWnd != NULL && hWnd == m_hWnd )
		{
			UINT fFlags = LOWORD(pMsg->wParam);
			short zDelta = HIWORD(pMsg->wParam);
			if( OnSwDoMouseWheel( fFlags, zDelta, point ) )
				return TRUE;
		}
	}
	
	return CWnd::PreTranslateMessage(pMsg);
}

void CExtScrollWnd::OnSwEnsurePointAvail( CPoint point )
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL)
		return;
	if( ! ::IsWindow( GetSafeHwnd() ) )
		return;
	if( ! ( OnSwHasScrollBar(false) || OnSwHasScrollBar(true) ) )
		return;
CRect rcClient;
	CWnd::GetClientRect( &rcClient );
	if( rcClient.PtInRect(point) )
		return;

CPoint ptScrollOrg( OnSwGetScrollPos() );
CPoint ptScroll( ptScrollOrg );
CSize sizeTotal( OnSwGetTotalSize() );

	if( OnSwHasScrollBar(true) )
	{
		if( point.x < 0 )
			ptScroll.x += point.x;
		else if( point.x > rcClient.Width() )
			ptScroll.x += point.x - rcClient.Width();
		if( ptScroll.x < 0 )
			ptScroll.x = 0;
		else if( ptScroll.x > sizeTotal.cx )
			ptScroll.x = sizeTotal.cx;
	} // if( OnSwHasScrollBar(true) )
	if( OnSwHasScrollBar(false) )
	{
		if( point.y < 0 )
			ptScroll.y += point.y;
		else if( point.y > rcClient.Height() )
			ptScroll.y += point.y - rcClient.Height();
		if( ptScroll.y < 0 )
			ptScroll.y = 0;
		else if( ptScroll.y > sizeTotal.cy )
			ptScroll.y = sizeTotal.cy;
	} // if( OnSwHasScrollBar(false) )

	if( ptScrollOrg != ptScroll )
		OnSwSetScrollPos( ptScroll );
}

#if (!defined __EXT_MFC_NO_SCROLLITEMWND)

/////////////////////////////////////////////////////////////////////////////
// CExtScrollItemWnd window & CExtScrollItemCacheInfo helper

CExtScrollItemCacheInfo::CExtScrollItemCacheInfo(
	LONG nPosH,   // = 0L
	LONG nMaxH,   // = 0L
	LONG nPageSizeH, // = 0L
	LONG nPosV,   // = 0L
	LONG nMaxV,   // = 0L
	LONG nPageSizeV, // = 0L
	bool bBeginH,    // = false
	bool bEndH,      // = false
	bool bBeginV,    // = false
	bool bEndV       // = false
	)
	: m_nPosH( nPosH )
	, m_nMaxH( nMaxH )
	, m_nPageSizeH( nPageSizeH )
	, m_nPosV( nPosV )
	, m_nMaxV( nMaxV )
	, m_nPageSizeV( nPageSizeV )
	, m_bBeginH( bBeginH )
	, m_bEndH( bEndH )
	, m_bBeginV( bBeginV )
	, m_bEndV( bEndV )
{
	ASSERT( IsValid() );
}

CExtScrollItemCacheInfo::CExtScrollItemCacheInfo(
	const CExtScrollItemCacheInfo & other
	)
{
	(*this) = other;
	ASSERT( IsValid() );
}

void CExtScrollItemCacheInfo::Assign(
	const CExtScrollItemCacheInfo & other,
	bool bAssignH, // = true
	bool bAssignV // = true
	)
{
	if( bAssignH )
	{
		m_nPosH = other.m_nPosH;
		m_nMaxH = other.m_nMaxH;
		m_nPageSizeH = other.m_nPageSizeH;
		m_bBeginH = other.m_bBeginH;
		m_bEndH = other.m_bEndH;
	} // if( bAssignH )
	if( bAssignV )
	{
		m_nPosV = other.m_nPosV;
		m_nMaxV = other.m_nMaxV;
		m_nPageSizeV = other.m_nPageSizeV;
		m_bBeginV = other.m_bBeginV;
		m_bEndV = other.m_bEndV;
	} // if( bAssignV )
	ASSERT( IsValid() );
}

void CExtScrollItemCacheInfo::AssignH(
	const CExtScrollItemCacheInfo & other
	)
{
	Assign( other, true, false );
}

void CExtScrollItemCacheInfo::AssignV(
	const CExtScrollItemCacheInfo & other
	)
{
	Assign( other, false, true );
}

CExtScrollItemCacheInfo & CExtScrollItemCacheInfo::operator = (
	const CExtScrollItemCacheInfo & other
	)
{
	Assign( other );
	return (*this);
}

bool CExtScrollItemCacheInfo::operator == (
	const CExtScrollItemCacheInfo & other
	) const
{
	return IsEqual( other );
}

bool CExtScrollItemCacheInfo::operator != (
	const CExtScrollItemCacheInfo & other
	) const
{
	return ( ! IsEqual( other ) );
}

void CExtScrollItemCacheInfo::Empty(
	bool bEmptyH, // = true
	bool bEmptyV // = true
	)
{
	ASSERT( IsValid() );
	if( bEmptyH )
	{
		m_nPosH = 0L;
		m_nMaxH = 0L;
		m_nPageSizeH = 0L;
		m_bBeginH = false;
		m_bEndH = false;
	} // if( bEmptyH )
	if( bEmptyV )
	{
		m_nPosV = 0L;
		m_nMaxV = 0L;
		m_nPageSizeV = 0L;
		m_bBeginV = false;
		m_bEndV = false;
	} // if( bEmptyV )
}

void CExtScrollItemCacheInfo::EmptyH()
{
	ASSERT( IsValid() );
	Empty( true, false );
}

void CExtScrollItemCacheInfo::EmptyV()
{
	ASSERT( IsValid() );
	Empty( false, true );
}

bool CExtScrollItemCacheInfo::IsEmpty(
	bool bCheckEmptyH, // = true
	bool bCheckEmptyV // = true
	) const
{
	ASSERT( IsValid() );
	if(		(	bCheckEmptyH
			&&	m_nMaxH != 0L
			)
		||	(	bCheckEmptyV
			&&	m_nMaxV != 0L
			)
		)
		return false;
	return true;
}

bool CExtScrollItemCacheInfo::IsEmptyH() const
{
	ASSERT( IsValid() );
	return IsEmpty( true, false );
}

bool CExtScrollItemCacheInfo::IsEmptyV() const
{
	ASSERT( IsValid() );
	return IsEmpty( false, true );
}

bool CExtScrollItemCacheInfo::IsValid() const
{
	if(		m_nPosH >= 0L
		&&	m_nPosV >= 0L
		&&	m_nMaxH >= 0L
		&&	m_nMaxV >= 0L
		&&	m_nPageSizeH >= 0L
		&&	m_nPageSizeV >= 0L
		&&	0L <= m_nPosH && m_nPosH <= m_nMaxH
		&&	0L <= m_nPosV && m_nPosV <= m_nMaxV
		)
		return true;
	return false;
}

bool CExtScrollItemCacheInfo::IsEqual( // static
	const CExtScrollItemCacheInfo & left,
	const CExtScrollItemCacheInfo & right,
	bool bComparePosH,   // = true
	bool bComparePosV,   // = true
	bool bCompareMaxH,   // = true
	bool bCompareMaxV,   // = true
	bool bComparePageSizeH, // = true
	bool bComparePageSizeV, // = true
	bool bCompareBofH,      // = false
	bool bCompareBofV,      // = false
	bool bCompareEofH,      // = false
	bool bCompareEofV       // = false
	)
{
	ASSERT( left.IsValid() );
	ASSERT( left.IsValid() );
	if(		(	bComparePosH
			&&	left.m_nPosH != right.m_nPosH
			)
		||	(	bComparePosV
			&&	left.m_nPosV != right.m_nPosV
			)
		||	(	bCompareMaxH
			&&	left.m_nMaxH != right.m_nMaxH
			)
		||	(	bCompareMaxV
			&&	left.m_nMaxV != right.m_nMaxV
			)
		||	(	bComparePageSizeH
			&&	left.m_nPageSizeH != right.m_nPageSizeH
			)
		||	(	bComparePageSizeV
			&&	left.m_nPageSizeV != right.m_nPageSizeV
			)
		||	(	bCompareBofH
			&&	left.m_bBeginH != right.m_bBeginH
			)
		||	(	bCompareBofV
			&&	left.m_bBeginV != right.m_bBeginV
			)
		||	(	bCompareEofH
			&&	left.m_bEndH != right.m_bEndH
			)
		||	(	bCompareEofV
			&&	left.m_bEndV != right.m_bEndV
			)
		)
		return false;
	return true;
}

bool CExtScrollItemCacheInfo::IsEqual(
	const CExtScrollItemCacheInfo & other,
	bool bComparePosH,   // = true
	bool bComparePosV,   // = true
	bool bCompareMaxH,   // = true
	bool bCompareMaxV,   // = true
	bool bComparePageSizeH, // = true
	bool bComparePageSizeV, // = true
	bool bCompareBofH,      // = false
	bool bCompareBofV,      // = false
	bool bCompareEofH,      // = false
	bool bCompareEofV       // = false
	) const
{
	return
		IsEqual(
			*this, other,
			bComparePosH, bComparePosV,
			bCompareMaxH, bCompareMaxV,
			bComparePageSizeH, bComparePageSizeV,
			bCompareBofH, bCompareBofV,
			bCompareEofH, bCompareEofV
			);
}

LONG CExtScrollItemCacheInfo::GetExtentH() const
{
	ASSERT( IsValid() );
	return (m_nMaxH + m_nPageSizeH);
}

LONG CExtScrollItemCacheInfo::GetExtentV() const
{
	ASSERT( IsValid() );
	return (m_nMaxV + m_nPageSizeV);
}

LONG CExtScrollItemCacheInfo::GetPosChangingH(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_nPosH - _sciOld.m_nPosH);
}

LONG CExtScrollItemCacheInfo::GetPosChangingV(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_nPosV - _sciOld.m_nPosV);
}

LONG CExtScrollItemCacheInfo::GetMaxChangingH(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_nMaxH - _sciOld.m_nMaxH);
}

LONG CExtScrollItemCacheInfo::GetMaxChangingV(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_nMaxV - _sciOld.m_nMaxV);
}

LONG CExtScrollItemCacheInfo::GetExtentChangingH(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
LONG nExtentThis = GetExtentH();
LONG nExtentOld = _sciOld.GetExtentH();
	return (nExtentThis - nExtentOld);
}

LONG CExtScrollItemCacheInfo::GetExtentChangingV(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
LONG nExtentThis = GetExtentV();
LONG nExtentOld = _sciOld.GetExtentV();
	return (nExtentThis - nExtentOld);
}

LONG CExtScrollItemCacheInfo::GetPageSizeChangingH(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_nPageSizeH - _sciOld.m_nPageSizeH);
}

LONG CExtScrollItemCacheInfo::GetPageSizeChangingV(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_nPageSizeV - _sciOld.m_nPageSizeV);
}

bool CExtScrollItemCacheInfo::IsShiftToBofH(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_bBeginH&&(!_sciOld.m_bBeginH)) ? true : false;
}

bool CExtScrollItemCacheInfo::IsShiftToBofV(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_bBeginV&&(!_sciOld.m_bBeginV)) ? true : false;
}

bool CExtScrollItemCacheInfo::IsShiftToEofH(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_bEndH&&(!_sciOld.m_bEndH)) ? true : false;
}

bool CExtScrollItemCacheInfo::IsShiftToEofV(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_bEndV&&(!_sciOld.m_bEndV)) ? true : false;
}

void CExtScrollItemCacheInfo::FlipHV()
{
	ASSERT( IsValid() );
LONG nTemp;
bool bTemp;
	
	nTemp = m_nPosH;
	m_nPosH = m_nPosV;
	m_nPosV = nTemp;

	nTemp = m_nMaxH;
	m_nMaxH = m_nMaxV;
	m_nMaxV = nTemp;

	nTemp = m_nPageSizeH;
	m_nPageSizeH = m_nPageSizeV;
	m_nPageSizeV = nTemp;

	bTemp = m_bBeginH;
	m_bBeginH = m_bBeginV;
	m_bBeginV = bTemp;

	bTemp = m_bEndH;
	m_bEndH = m_bEndV;
	m_bEndV = bTemp;
	
	ASSERT( IsValid() );
}

IMPLEMENT_DYNCREATE( CExtScrollItemWnd, CExtScrollWnd );

CExtScrollItemWnd::CExtScrollItemWnd()
	: m_bDirectCreateCall( false )
	, m_dwScrollItemWndStyle( 0L )
	, m_dwScrollItemWndStyleEx( 0L )
	, m_bExtractResourceStyles( true )
{
	VERIFY( RegisterScrollItemWndClass() );
}

CExtScrollItemWnd::~CExtScrollItemWnd()
{
}

BEGIN_MESSAGE_MAP(CExtScrollItemWnd,CExtScrollWnd)
	//{{AFX_MSG_MAP(CExtScrollItemWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

bool CExtScrollItemWnd::g_bScrollItemWndClassRegistered = false;

bool CExtScrollItemWnd::RegisterScrollItemWndClass()
{
	if( g_bScrollItemWndClassRegistered )
		return true;

WNDCLASS wndclass;
HINSTANCE hInst = AfxGetInstanceHandle();
	if( ! ::GetClassInfo(
			hInst,
			__EXT_SCROLLITEMWND_CLASS_NAME,
			&wndclass
			)
		)
	{
		wndclass.style = CS_GLOBALCLASS|CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW;
		wndclass.lpfnWndProc = ::DefWindowProc;
		wndclass.cbClsExtra = wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInst;
		wndclass.hIcon = NULL;
		wndclass.hCursor = ::LoadCursor( NULL, IDC_ARROW );
		ASSERT( wndclass.hCursor != NULL );
		wndclass.hbrBackground = NULL; 
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = __EXT_SCROLLITEMWND_CLASS_NAME;
		if( !::AfxRegisterClass( &wndclass ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}

	g_bScrollItemWndClassRegistered = true;
	return true;
}

BOOL CExtScrollItemWnd::Create(
	CWnd * pParentWnd,
	const RECT & rcWnd, // = CRect( 0, 0, 0, 0 )
	UINT nDlgCtrlID, // = UINT( IDC_STATIC )
	DWORD dwScrollItemWndStyle, // = 0L
	DWORD dwWindowStyle, // = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS
	CCreateContext * pContext // = NULL
	)
{
	ASSERT_VALID( this );
	if( !RegisterScrollItemWndClass() )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	m_bDirectCreateCall = true;
	m_dwScrollItemWndStyle = dwScrollItemWndStyle;
	if( ! CWnd::Create(
			__EXT_SCROLLITEMWND_CLASS_NAME,
			NULL,
			dwWindowStyle,
			rcWnd,
			pParentWnd,
			nDlgCtrlID,
			pContext
			)
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}

	if( !_CreateHelper() )
	{
		ASSERT( FALSE );
		AfxThrowMemoryException();
	} // if( !Create() )

	return TRUE;
}

BOOL CExtScrollItemWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	ASSERT_VALID( this );
	if( ( !RegisterScrollItemWndClass() )
		|| ( !CWnd::PreCreateWindow(cs) )
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
	
	cs.lpszClass = __EXT_SCROLLITEMWND_CLASS_NAME;

	return TRUE;
}

bool CExtScrollItemWnd::_CreateHelper()
{
	ASSERT_VALID( this );
	OnSwUpdateScrollBars();
	OnSwDoRedraw();
	return true;
}

void CExtScrollItemWnd::PreSubclassWindow() 
{
	ASSERT_VALID( this );
	CWnd::PreSubclassWindow();

	if( m_bDirectCreateCall )
		return;

	if( m_bExtractResourceStyles )
	{
		DWORD dwStyle = ::GetWindowLong( m_hWnd, GWL_STYLE );
		m_dwScrollItemWndStyle = dwStyle & __EXTMFC_ALL_FORM_MOVABLE_WND_STYLES;
		::SetWindowLong( m_hWnd, GWL_STYLE, dwStyle & (~__EXTMFC_ALL_FORM_MOVABLE_WND_STYLES) );
	} // if( m_bExtractResourceStyles )
	
	if( !_CreateHelper() )
	{
		ASSERT( FALSE );
		AfxThrowMemoryException();
	} // if( !Create() )
}

DWORD CExtScrollItemWnd::SiwGetStyle() const
{
	ASSERT( this != NULL );
	return m_dwScrollItemWndStyle;
}

DWORD CExtScrollItemWnd::SiwModifyStyle(
	DWORD dwStyleAdd,
	DWORD dwStyleRemove, // = 0L
	bool bUpdateWnd // = true
	)
{
	ASSERT( this != NULL );
DWORD dwScrollItemWndStyleOld = SiwGetStyle();
	if( dwStyleAdd == 0 && dwStyleRemove == 0 )
		return dwScrollItemWndStyleOld;
	m_dwScrollItemWndStyle &= ~dwStyleRemove;
	m_dwScrollItemWndStyle |= dwStyleAdd;
	if(		bUpdateWnd
		&&	GetSafeHwnd() != NULL
		&&	::IsWindow( GetSafeHwnd() )
		)
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	}
	return dwScrollItemWndStyleOld;
}

DWORD CExtScrollItemWnd::SiwGetStyleEx() const
{
	ASSERT( this != NULL );
	return m_dwScrollItemWndStyleEx;
}

DWORD CExtScrollItemWnd::SiwModifyStyleEx(
	DWORD dwStyleExAdd,
	DWORD dwStyleExRemove, // = 0L
	bool bUpdateWnd // = true
	)
{
	ASSERT( this != NULL );
DWORD dwScrollItemWndStyleExOld = SiwGetStyle();
	if( dwStyleExAdd == 0 && dwStyleExRemove == 0 )
		return dwScrollItemWndStyleExOld;
	m_dwScrollItemWndStyleEx &= ~dwStyleExRemove;
	m_dwScrollItemWndStyleEx |= dwStyleExAdd;
	if(		bUpdateWnd
		&&	GetSafeHwnd() != NULL
		&&	::IsWindow( GetSafeHwnd() )
		)
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	}
	return dwScrollItemWndStyleExOld;
}

DWORD CExtScrollItemWnd::SiwScrollTypeHGet() const
{
	ASSERT( this != NULL );
DWORD dwScrollItemWndStyle = SiwGetStyle();
	return (dwScrollItemWndStyle&__ESIS_STH_MASK);
}

DWORD CExtScrollItemWnd::SiwScrollTypeVGet() const
{
	ASSERT( this != NULL );
DWORD dwScrollItemWndStyle = SiwGetStyle();
	return ((dwScrollItemWndStyle&__ESIS_STV_MASK) >> __ESIW_ST_BIT_COUNT);
}

DWORD CExtScrollItemWnd::SiwScrollTypeHSet(
	DWORD dwScrollTypeNew,
	bool bUpdateWnd // = true
	)
{
	ASSERT( this != NULL );
	dwScrollTypeNew &= __ESIW_ST_MASK;
DWORD dwScrollTypeOld = SiwScrollTypeHGet();
	if( dwScrollTypeNew == dwScrollTypeOld )
		return dwScrollTypeOld;
	SiwModifyStyle(
		dwScrollTypeNew,
		__ESIS_STH_MASK,
		bUpdateWnd
		);
	return dwScrollTypeOld;
}

DWORD CExtScrollItemWnd::SiwScrollTypeVSet(
	DWORD dwScrollTypeNew,
	bool bUpdateWnd // = true
	)
{
	ASSERT( this != NULL );
	dwScrollTypeNew &= __ESIW_ST_MASK;
DWORD dwScrollTypeOld = SiwScrollTypeVGet();
	if( dwScrollTypeNew == dwScrollTypeOld )
		return dwScrollTypeOld;
	SiwModifyStyle(
		dwScrollTypeNew << __ESIW_ST_BIT_COUNT,
		__ESIS_STV_MASK,
		bUpdateWnd
		);
	return dwScrollTypeOld;
}

bool CExtScrollItemWnd::SiwPreferHorizontalWalkGet() const
{
	ASSERT_VALID( this );
bool bPreferHorizontalWalk =
	( (SiwGetStyle() & __ESIS_PREFER_HORZ_WALK) != 0 )
		? true : false;
	return bPreferHorizontalWalk;
}

bool CExtScrollItemWnd::SiwPreferHorizontalWalkSet(
	bool bPreferHorizontalWalk
	)
{
bool bPreferHorizontalWalkOld = SiwPreferHorizontalWalkGet();
	if(		(bPreferHorizontalWalk && bPreferHorizontalWalkOld)
		||	((!bPreferHorizontalWalk) && (!bPreferHorizontalWalkOld))
		)
		return bPreferHorizontalWalkOld;
	SiwModifyStyle(
		( (bPreferHorizontalWalk) ? __ESIS_PREFER_HORZ_WALK : 0 ),
		__ESIS_PREFER_HORZ_WALK,
		false
		);
	return bPreferHorizontalWalkOld;
}

CSize CExtScrollItemWnd::OnSwGetLineSize( int nDirection ) const
{
	ASSERT_VALID( this );
	nDirection;
static const CSize g_sizePixelScrollStep( 3, 3 );
CSize _sizeLine( 0, 0 );
DWORD dwScrollType = SiwScrollTypeHGet();
	if( dwScrollType == __ESIW_ST_PIXEL )
		_sizeLine.cx = g_sizePixelScrollStep.cx;
	else if(	dwScrollType == __ESIW_ST_ITEM
			||	dwScrollType == __ESIW_ST_VIRTUAL
			)
		_sizeLine.cx = 1;
	dwScrollType = SiwScrollTypeVGet();
	if( dwScrollType == __ESIW_ST_PIXEL )
		_sizeLine.cy = g_sizePixelScrollStep.cy;
	else if(	dwScrollType == __ESIW_ST_ITEM
			||	dwScrollType == __ESIW_ST_VIRTUAL
			)
		_sizeLine.cy = 1;
	return _sizeLine;
}

void CExtScrollItemWnd::OnSwUpdateScrollBars()
{
	ASSERT_VALID( this );
	if( m_nUpdateScrollBars > 1 )
		return;
	m_nUpdateScrollBars ++;

	ASSERT( OnSwGetTotalSize().cx >= 0 && OnSwGetTotalSize().cy >= 0 );
CRect rcClient;
bool bCalcClient = true;
CWnd * pParentWnd = GetParent();
	if(		pParentWnd != NULL
		&&	(BOOL)pParentWnd->SendMessage(
				WM_RECALCPARENT,
				0,
				(LPARAM)(LPCRECT)&rcClient
				) != 0
		)
		bCalcClient = false;
CSize sizeClient;
CSize sizeSb;
	if( bCalcClient )
	{
		if( !OnSwCalcClientSizes( sizeClient, sizeSb ) )
		{ // no room for scroll bars
			CRect rcClient2 = OnSwGetClientRect();
			if( rcClient2.Width() > 0 && rcClient2.Height()> 0 )
			{
				OnSwEnableScrollBarCtrl( SB_BOTH, false );
				OnSwRecalcLayout( true );
			}
			m_nUpdateScrollBars --;
			return;
		}
	}
	else
	{ // let parent window determine the "client" rect
		sizeSb = OnSwGetScrollBarSizes();
		sizeClient.cx = rcClient.right - rcClient.left;
		sizeClient.cy = rcClient.bottom - rcClient.top;
	}

DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	if( dwScrollTypeH == __ESIW_ST_ITEM || dwScrollTypeV == __ESIW_ST_ITEM )
	{
		CSize _sizeVR = OnSiwGetVisibleRange().Size();
		if( dwScrollTypeH == __ESIW_ST_ITEM )
			sizeClient.cx = _sizeVR.cx;
		if( dwScrollTypeV == __ESIW_ST_ITEM )
			sizeClient.cy = _sizeVR.cy;
	} // if( dwScrollTypeH == __ESIW_ST_ITEM || dwScrollTypeV == __ESIW_ST_ITEM )

bool bHasSbH0 = OnSwHasScrollBar( true );
bool bHasSbV0 = OnSwHasScrollBar( false );

	// if enough room to add scrollbars
CSize sizeRange;
CPoint ptMove;
CSize sizeNeedSB;
	OnSwGetScrollBarState(
		sizeClient,
		sizeNeedSB,
		sizeRange,
		ptMove,
		bCalcClient
		);

//	if( bCalcClient )
//	{
//		if(		sizeNeedSB.cx != 0
//			&&	dwScrollTypeV == __ESIW_ST_PIXEL
//			//&&	bHasSbH0
//			)
//			sizeClient.cy -= sizeSb.cy;
//		if(		sizeNeedSB.cy != 0
//			&&	dwScrollTypeH == __ESIW_ST_PIXEL
//			//&&	bHasSbV0
//			)
//			sizeClient.cx -= sizeSb.cx;
//	} // if( bCalcClient )

	// scroll window + update
CPoint ptCurrScrollPos = OnSwGetScrollPos();
	if( ptMove != ptCurrScrollPos )
		OnSwSetScrollPos( ptMove );

	// the scrollbar page range
SCROLLINFO _scroll_info;
	::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
	_scroll_info.cbSize = sizeof(SCROLLINFO);
	_scroll_info.fMask = SIF_PAGE|SIF_RANGE;
	_scroll_info.nMin = 0;

//DWORD dwScrollTypeH = SiwScrollTypeHGet();
//DWORD dwScrollTypeV = SiwScrollTypeVGet();

CSize _sizeTotalISR = OnSwGetTotalSize();
	if( _sizeTotalISR.cx < 0 )
		_sizeTotalISR.cx = __EXT_SCROLL_NUMERIC_MAX;
	if( _sizeTotalISR.cy < 0 )
		_sizeTotalISR.cy = __EXT_SCROLL_NUMERIC_MAX;
CSize _sizePageISR = OnSwGetPageSize( 0 );

	// update bars
	if( dwScrollTypeH == __ESIW_ST_NONE )
		OnSwEnableScrollBarCtrl( SB_HORZ, false );
	else if( dwScrollTypeH == __ESIW_ST_PIXEL )
	{
		OnSwEnableScrollBarCtrl( SB_HORZ, sizeNeedSB.cx ? true : false );
		if( sizeNeedSB.cx )
		{
			_scroll_info.nPage = sizeClient.cx;
			_scroll_info.nMax = OnSwGetTotalSize().cx - 1;
			if( bHasSbH0 && bHasSbV0 )
				_scroll_info.nMax += OnSwGetScrollBarSizes().cx;
			if( ! OnSwScrollInfoAdjust(
					SB_HORZ,
					_scroll_info,
					true
					)
				)
				OnSwSetScrollRange(
					SB_HORZ,
					0,
					sizeRange.cx,
					true
					);
		} // if( sizeNeedSB.cx )
	} // else if( dwScrollTypeH == __ESIW_ST_PIXEL )
	else if( dwScrollTypeH == __ESIW_ST_ITEM )
	{
		if( _sizeTotalISR.cx > _sizePageISR.cx )
		{
			ASSERT( _sizePageISR.cx >= 0 );
			OnSwEnableScrollBarCtrl( SB_HORZ, true );
			_scroll_info.nPage = _sizePageISR.cx;
			_scroll_info.nMax = _sizeTotalISR.cx - 1;
			if( ! OnSwScrollInfoAdjust(
					SB_HORZ,
					_scroll_info,
					true
					)
				)
				OnSwSetScrollRange(
					SB_HORZ,
					0,
					_sizeTotalISR.cx,
					true
					);
		}
		else
			OnSwEnableScrollBarCtrl( SB_HORZ, false );
	} // else if( dwScrollTypeH == __ESIW_ST_ITEM )
	else
	{ // if __ESIW_ST_VIRTUAL
		ASSERT( dwScrollTypeH == __ESIW_ST_VIRTUAL );
		bool bBOF = OnSiwQueryVirtualBOF( true );
		bool bEOF = OnSiwQueryVirtualEOF( true );
		bool bEnableSB = true;
		if( bBOF )
		{
			if( bEOF )
				bEnableSB = false;
			else
				_scroll_info.nPos = 0;
		} // if( bBOF )
		else
		{
			if( bEOF )
				_scroll_info.nPos = __EXT_SCROLL_NUMERIC_MAX - 1;
			else
				_scroll_info.nPos = __EXT_SCROLL_NUMERIC_MAX / 2;
		} // else from if( bBOF )
		if( bEnableSB )
		{
			OnSwEnableScrollBarCtrl( SB_HORZ, true );
			_scroll_info.fMask = SIF_PAGE|SIF_RANGE|SIF_POS;
			_scroll_info.nPage = _sizePageISR.cx;
			_scroll_info.nMax = __EXT_SCROLL_NUMERIC_MAX;
			if( ! OnSwScrollInfoAdjust(
					SB_HORZ,
					_scroll_info,
					true
					)
				)
				OnSwSetScrollRange(
					SB_HORZ,
					0,
					_sizeTotalISR.cx,
					true
					);
		} // if( bEnableSB )
		else
			OnSwEnableScrollBarCtrl( SB_HORZ, false );
	} // if __ESIW_ST_VIRTUAL

	// recalc for vertical parms
	_sizeTotalISR = OnSwGetTotalSize();
	if( _sizeTotalISR.cx < 0 )
		_sizeTotalISR.cx = __EXT_SCROLL_NUMERIC_MAX;
	if( _sizeTotalISR.cy < 0 )
		_sizeTotalISR.cy = __EXT_SCROLL_NUMERIC_MAX;
	_sizePageISR = OnSwGetPageSize( 0 );

	if( dwScrollTypeV == __ESIW_ST_NONE )
		OnSwEnableScrollBarCtrl( SB_VERT, false );
	else if( dwScrollTypeV == __ESIW_ST_PIXEL )
	{
		OnSwEnableScrollBarCtrl( SB_VERT, sizeNeedSB.cy ? true : false );
		if( sizeNeedSB.cy )
		{
			_scroll_info.nPage = sizeClient.cy;
			_scroll_info.nMax = OnSwGetTotalSize().cy - 1;
			if( bHasSbH0 && bHasSbV0 )
				_scroll_info.nMax += OnSwGetScrollBarSizes().cy;
			if( ! OnSwScrollInfoAdjust(
					SB_VERT,
					_scroll_info,
					true
					)
				)
				OnSwSetScrollRange(
					SB_VERT,
					0,
					sizeRange.cy,
					true
					);
		} // if( sizeNeedSB.cy )
	} // else if( dwScrollTypeV == __ESIW_ST_PIXEL )
	else if( dwScrollTypeV == __ESIW_ST_ITEM )
	{
		if( _sizeTotalISR.cy > _sizePageISR.cy )
		{
			ASSERT( _sizePageISR.cy >= 0 );
			OnSwEnableScrollBarCtrl( SB_VERT, true );
			_scroll_info.nPage = _sizePageISR.cy;
			_scroll_info.nMax = _sizeTotalISR.cy - 1;
			if( ! OnSwScrollInfoAdjust(
					SB_VERT,
					_scroll_info,
					true
					)
				)
				OnSwSetScrollRange(
					SB_VERT,
					0,
					_sizeTotalISR.cy,
					true
					);
		}
		else
			OnSwEnableScrollBarCtrl( SB_VERT, false );
	} // else if( dwScrollTypeV == __ESIW_ST_ITEM )
	else
	{ // if __ESIW_ST_VIRTUAL
		ASSERT( dwScrollTypeV == __ESIW_ST_VIRTUAL );
		bool bBOF = OnSiwQueryVirtualBOF( false );
		bool bEOF = OnSiwQueryVirtualEOF( false );
		bool bEnableSB = true;
		if( bBOF )
		{
			if( bEOF )
				bEnableSB = false;
			else
				_scroll_info.nPos = 0;
		} // if( bBOF )
		else
		{
			if( bEOF )
				_scroll_info.nPos = __EXT_SCROLL_NUMERIC_MAX - 1;
			else
				_scroll_info.nPos = __EXT_SCROLL_NUMERIC_MAX / 2;
		} // else from if( bBOF )
		if( bEnableSB )
		{
			OnSwEnableScrollBarCtrl( SB_VERT, true );
			_scroll_info.fMask = SIF_PAGE|SIF_RANGE|SIF_POS;
			_scroll_info.nPage = _sizePageISR.cy;
			_scroll_info.nMax = __EXT_SCROLL_NUMERIC_MAX;
			if( ! OnSwScrollInfoAdjust(
					SB_VERT,
					_scroll_info,
					true
					)
				)
				OnSwSetScrollRange(
					SB_VERT,
					0,
					_sizeTotalISR.cy,
					true
					);
		} // if( bEnableSB )
		else
			OnSwEnableScrollBarCtrl( SB_VERT, false );
	} // if __ESIW_ST_VIRTUAL

// adjust virtual position and cache state
//	if(		dwScrollTypeH == __ESIW_ST_VIRTUAL
//		||	dwScrollTypeV == __ESIW_ST_VIRTUAL
//		)
		OnSwDoScrollBy( CSize(0,0), false );
	OnSwRecalcLayout( true );

bool bHasSbH1 = OnSwHasScrollBar( true );
bool bHasSbV1 = OnSwHasScrollBar( false );
	if(		bHasSbH0 != bHasSbH1
		||	bHasSbV0 != bHasSbV1
//		||	dwScrollTypeH == __ESIW_ST_VIRTUAL
//		||	dwScrollTypeV == __ESIW_ST_VIRTUAL
		)
		OnSwUpdateScrollBars();

	m_nUpdateScrollBars --;
}

bool CExtScrollItemWnd::SiwAutohideScrollBarGet( bool bHorz ) const
{
	ASSERT_VALID( this );
	if( bHorz )
	{
		if( ( SiwGetStyle() & __ESIS_DISABLE_AUTOHIDE_SB_H ) != 0 )
			return false;
	} // if( bHorz )
	else
	{
		if( ( SiwGetStyle() & __ESIS_DISABLE_AUTOHIDE_SB_V ) != 0 )
			return false;
	} // else from if( bHorz )
	return true;
}

bool CExtScrollItemWnd::SiwAutohideScrollBarSet(
	bool bHorz,
	bool bAutoHide // = true
	)
{
	ASSERT_VALID( this );
bool bAutohideScrollBarOld = SiwThumbTrackEnabledGet( bHorz );
	if(		(bAutohideScrollBarOld && bAutoHide)
		||	((!bAutohideScrollBarOld) && (!bAutoHide))
		)
		return bAutohideScrollBarOld;
DWORD dwRemove = bHorz ? __ESIS_DISABLE_AUTOHIDE_SB_H : __ESIS_DISABLE_AUTOHIDE_SB_V;
DWORD dwAdd = bAutoHide ? 0L : dwRemove;
	SiwModifyStyle( dwAdd, dwRemove, false );
	return bAutohideScrollBarOld;
}

bool CExtScrollItemWnd::SiwThumbTrackEnabledGet( bool bHorz ) const
{
	ASSERT_VALID( this );
	if( bHorz )
	{
		if( ( SiwGetStyle() & __ESIS_DISABLE_THUMBTRACK_H ) != 0 )
			return false;
	} // if( bHorz )
	else
	{
		if( ( SiwGetStyle() & __ESIS_DISABLE_THUMBTRACK_V ) != 0 )
			return false;
	} // else from if( bHorz )
	return true;
}

bool CExtScrollItemWnd::SiwThumbTrackEnabledSet(
	bool bHorz,
	bool bEnabled // = true
	)
{
	ASSERT_VALID( this );
bool bThumbTrackEnabledOld = SiwThumbTrackEnabledGet( bHorz );
	if(		(bThumbTrackEnabledOld && bEnabled)
		||	((!bThumbTrackEnabledOld) && (!bEnabled))
		)
		return bThumbTrackEnabledOld;
DWORD dwRemove = bHorz ? __ESIS_DISABLE_THUMBTRACK_H : __ESIS_DISABLE_THUMBTRACK_V;
DWORD dwAdd = bEnabled ? 0L : dwRemove;
	SiwModifyStyle( dwAdd, dwRemove, false );
	return bThumbTrackEnabledOld;
}

bool CExtScrollItemWnd::OnSwQueryThumbTrackEnabled( bool bHorz ) const
{
	ASSERT_VALID( this );
	if( !SiwThumbTrackEnabledGet( bHorz ) )
		return false;
DWORD dwScrollType = bHorz
		? SiwScrollTypeHGet()
		: SiwScrollTypeVGet()
		;
	if( dwScrollType == __ESIW_ST_VIRTUAL )
		return false;
	return true;
}

CSize CExtScrollItemWnd::OnSwGetTotalSize() const
{
	ASSERT_VALID( this );
CSize _sizeTotalISR( 0, 0 );
DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	if( dwScrollTypeH == __ESIW_ST_VIRTUAL )
		_sizeTotalISR.cx = __EXT_SCROLL_NUMERIC_MAX;
	if( dwScrollTypeV == __ESIW_ST_VIRTUAL )
		_sizeTotalISR.cy = __EXT_SCROLL_NUMERIC_MAX;
	return _sizeTotalISR;
}

CSize CExtScrollItemWnd::OnSwGetPageSize( int nDirection ) const
{
	ASSERT_VALID( this );
CRect rcClient = OnSwGetClientRect();
CSize _sizeClient = rcClient.Size();
CSize _sizePage = OnSiwCalcPageMetrics( nDirection );

DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();

	if( dwScrollTypeH == __ESIW_ST_NONE )
		_sizePage.cx = 0;
	else if( dwScrollTypeH == __ESIW_ST_PIXEL )
		_sizePage.cx = _sizeClient.cx;
	else if( dwScrollTypeH == __ESIW_ST_ITEM )
	{
	} // else if( dwScrollTypeH == __ESIW_ST_ITEM )
	else
	{ // if __ESIW_ST_VIRTUAL
		ASSERT( dwScrollTypeH == __ESIW_ST_VIRTUAL );
	} // if __ESIW_ST_VIRTUAL
	
	if( dwScrollTypeV == __ESIW_ST_NONE )
		_sizePage.cy = 0;
	else if( dwScrollTypeV == __ESIW_ST_PIXEL )
		_sizePage.cy = _sizeClient.cy;
	else if( dwScrollTypeV == __ESIW_ST_ITEM )
	{
	} // else if( dwScrollTypeV == __ESIW_ST_ITEM )
	else
	{ // if __ESIW_ST_VIRTUAL
		ASSERT( dwScrollTypeV == __ESIW_ST_VIRTUAL );
	} // if __ESIW_ST_VIRTUAL

	return _sizePage;
}

CSize CExtScrollItemWnd::OnSiwCalcItemSize() const
{
	ASSERT_VALID( this );
	return CSize( 0, 0 );
}

CSize CExtScrollItemWnd::OnSiwCalcPageMetrics( int nDirection ) const
{
	ASSERT_VALID( this );
	nDirection;
CSize _sizeItem = OnSiwCalcItemSize();
	ASSERT( _sizeItem.cx >= 0 && _sizeItem.cy >= 0 );
CRect rcClient = OnSwGetClientRect();
CSize _sizeClient = rcClient.Size();
	if( _sizeClient.cx < 0 )
		_sizeClient.cx = 0;
	if( _sizeClient.cy < 0 )
		_sizeClient.cy = 0;
CSize _sizePageMetrics(
		( _sizeItem.cx == 0 ) ? 0 : ( _sizeClient.cx / _sizeItem.cx ),
		( _sizeItem.cy == 0 ) ? 0 : ( _sizeClient.cy / _sizeItem.cy )
		);
	if( _sizeItem.cx != 0 )
	{
		if( _sizePageMetrics.cx == 0 )
			_sizePageMetrics.cx = 1;
	}
	else
		_sizePageMetrics.cx = 0;
	if( _sizeItem.cy != 0 )
	{
		if( _sizePageMetrics.cy == 0 )
			_sizePageMetrics.cy = 1;
	}
	else
		_sizePageMetrics.cy = 0;
	return _sizePageMetrics;
}

bool CExtScrollItemWnd::OnSiwQueryVirtualBOF( bool bHorz ) const
{
	ASSERT_VALID( this );
	if( bHorz )
	{
		DWORD dwScrollTypeH = SiwScrollTypeHGet();
		if( dwScrollTypeH != __ESIW_ST_VIRTUAL )
			return true;
	} // if( bHorz )
	else
	{
		DWORD dwScrollTypeV = SiwScrollTypeVGet();
		if( dwScrollTypeV != __ESIW_ST_VIRTUAL )
			return true;
	} // else from if( bHorz )
	return false;
}

bool CExtScrollItemWnd::OnSiwQueryVirtualEOF( bool bHorz ) const
{
	ASSERT_VALID( this );
	if( bHorz )
	{
		DWORD dwScrollTypeH = SiwScrollTypeHGet();
		if( dwScrollTypeH != __ESIW_ST_VIRTUAL )
			return true;
	} // if( bHorz )
	else
	{
		DWORD dwScrollTypeV = SiwScrollTypeVGet();
		if( dwScrollTypeV != __ESIW_ST_VIRTUAL )
			return true;
	} // else from if( bHorz )
	return false;
}

INT CExtScrollItemWnd::OnSiwQueryItemExtentH(
	LONG nColNo,
	INT * p_nExtraSpaceBefore, // = NULL
	INT * p_nExtraSpaceAfter // = NULL
	) const
{
	ASSERT_VALID( this );
	ASSERT( nColNo >= 0 );
	nColNo;
	if( p_nExtraSpaceBefore != NULL )
		(*p_nExtraSpaceBefore) = 0;
	if( p_nExtraSpaceAfter != NULL )
		(*p_nExtraSpaceAfter) = 0;
	return 0;
}

INT CExtScrollItemWnd::OnSiwQueryItemExtentV(
	LONG nRowNo,
	INT * p_nExtraSpaceBefore, // = NULL
	INT * p_nExtraSpaceAfter // = NULL
	) const
{
	ASSERT_VALID( this );
	ASSERT( nRowNo >= 0 );
	nRowNo;
	if( p_nExtraSpaceBefore != NULL )
		(*p_nExtraSpaceBefore) = 0;
	if( p_nExtraSpaceAfter != NULL )
		(*p_nExtraSpaceAfter) = 0;
	return 0;
}

bool CExtScrollItemWnd::OnSiwVirtualEndTestH( LONG nColNo, LPARAM lParam ) const
{
	ASSERT_VALID( this );
	ASSERT( nColNo >= 0 );
	nColNo;
	lParam;
	return false;
}

bool CExtScrollItemWnd::OnSiwVirtualEndTestV( LONG nRowNo, LPARAM lParam ) const
{
	ASSERT_VALID( this );
	ASSERT( nRowNo >= 0 );
	nRowNo;
	lParam;
	return false;
}

bool CExtScrollItemWnd::OnSiwWalkCell(
	CDC & dc,
	LPVOID pQueryData,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	const RECT & rcVisibleRange,
	bool & bVirtualRightReached,
	bool & bVirtualBottomReached,
	DWORD dwAreaFlags,
	bool bFocusedControl
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL || pQueryData != NULL );
	ASSERT( nVisibleColNo >= 0 );
	ASSERT( nVisibleRowNo >= 0 );
	ASSERT( nColNo >= 0 );
	ASSERT( nRowNo >= 0 );
	ASSERT( rcCell.right >= rcCell.left );
	ASSERT( rcCell.bottom >= rcCell.top );
	ASSERT( rcCellExtra.right >= rcCellExtra.left );
	ASSERT( rcCellExtra.bottom >= rcCellExtra.top );
	ASSERT( rcCellExtra.left <= rcCell.left );
	ASSERT( rcCellExtra.top <= rcCell.top );
	ASSERT( rcCellExtra.right >= rcCell.right );
	ASSERT( rcCellExtra.bottom >= rcCell.bottom );
	dc;
	pQueryData;
	nVisibleColNo;
	nVisibleRowNo;
	nColNo;
	nRowNo;
	rcCellExtra;
	rcCell;
	rcVisibleRange;
	bVirtualRightReached;
	bVirtualBottomReached;
	dwAreaFlags;
	bFocusedControl;

//	if( pQueryData != NULL )
//		return false; // no default hit-testing implementation
//	bVirtualRightReached = OnSiwVirtualEndTestH( nColNo, LPARAM(dwAreaFlags) );
//	if( bVirtualRightReached )
//		return false;
//	bVirtualBottomReached = OnSiwVirtualEndTestV( nRowNo, LPARAM(dwAreaFlags) );
//	if( bVirtualBottomReached )
//		return false;
//	if(		rcCell.right == rcCell.left
//		||	rcCell.bottom == rcCell.top
//		)
//		return false;
//	if( ! dc.RectVisible( &rcCell ) )
//		return false;
//COLORREF clrLT = OnSiwGetSysColor( COLOR_3DHIGHLIGHT );
//COLORREF clrRB = OnSiwGetSysColor( COLOR_3DSHADOW );
//COLORREF clrFace = OnSiwGetSysColor( COLOR_3DFACE );
//	dc.FillSolidRect( &rcCell, clrFace );
//CString sCellText;
//	sCellText.Format( _T("%08d-%08d"), nColNo, nRowNo );
//	dc.DrawText(
//		sCellText,
//		(LPRECT)&rcCell,
//		DT_SINGLELINE|DT_CENTER|DT_VCENTER
//		);
//	dc.Draw3dRect( &rcCell, clrLT, clrRB );

	return false;
}

bool CExtScrollItemWnd::OnSiwWalkItemsH(
	CDC & dc,
	LPVOID pQueryData,
	const RECT & rcRowExtra,
	const RECT & rcRow,
	LONG nVisibleRowNo,
	LONG nRowNo,
	const RECT & rcVisibleRange,
	bool & bVirtualBottomReached,
	DWORD dwAreaFlags,
	bool bFocusedControl
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL || pQueryData != NULL );
	ASSERT( nVisibleRowNo >= 0 );
	ASSERT( nRowNo >= 0 );
	ASSERT( rcRow.right >= rcRow.left );
	ASSERT( rcRow.bottom >= rcRow.top );
	ASSERT( rcRowExtra.right >= rcRowExtra.left );
	ASSERT( rcRowExtra.bottom >= rcRowExtra.top );
	ASSERT( rcRowExtra.left <= rcRow.left );
	ASSERT( rcRowExtra.top <= rcRow.top );
	ASSERT( rcRowExtra.right >= rcRow.right );
	ASSERT( rcRowExtra.bottom >= rcRow.bottom );
	bVirtualBottomReached = OnSiwVirtualEndTestV( nRowNo, LPARAM(dwAreaFlags) );
	if( bVirtualBottomReached )
		return false;
	if(		rcRowExtra.right == rcRowExtra.left
		||	rcRowExtra.bottom == rcRowExtra.top
		)
		return false;
	if( dc.GetSafeHdc() == NULL )
		return false; // no hit-test like query support
	if( ! dc.RectVisible( &rcRow ) )
		return false;
CRect rcClient = OnSwGetClientRect();
LONG nVisibleColNo = 0;
LONG nColNo = rcVisibleRange.left;
bool bVirtualRightReached = false;
CRect rcCellExtra(
		rcClient.left,
		rcRowExtra.top,
		rcClient.left,
		rcRowExtra.bottom
		);
	for( ; nColNo <= rcVisibleRange.right; nColNo++, nVisibleColNo++ )
	{
		INT nExtraSpaceBefore = 0, nExtraSpaceAfter = 0;
		INT nColWidth =
			OnSiwQueryItemExtentH(
				nColNo,
				&nExtraSpaceBefore,
				&nExtraSpaceAfter
				);
		ASSERT( nColWidth >= 0 );
		ASSERT( nExtraSpaceBefore >= 0 );
		ASSERT( nExtraSpaceAfter >= 0 );
		rcCellExtra.right += nColWidth;
		if(		rcCellExtra.right < rcClient.left
			||	rcCellExtra.left > rcClient.right
			)
			continue;
		CRect rcCell(
			rcCellExtra.left + nExtraSpaceBefore,
			rcRow.top,
			rcCellExtra.right - nExtraSpaceAfter,
			rcRow.bottom
			);
		OnSiwWalkCell(
			dc,
			pQueryData,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			rcCellExtra,
			rcCell,
			rcVisibleRange,
			bVirtualRightReached,
			bVirtualBottomReached,
			dwAreaFlags,
			bFocusedControl
			);
		if( bVirtualRightReached || bVirtualBottomReached )
			break;
		rcCellExtra.left = rcCellExtra.right;
	} // for( ; nColNo <= rcVisibleRange.right; nColNo++, nVisibleColNo++ )
	return false;
}

bool CExtScrollItemWnd::OnSiwWalkItemsV(
	CDC & dc,
	LPVOID pQueryData,
	const RECT & rcColExtra,
	const RECT & rcCol,
	LONG nVisibleColNo,
	LONG nColNo,
	const RECT & rcVisibleRange,
	bool & bVirtualRightReached,
	DWORD dwAreaFlags,
	bool bFocusedControl
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL || pQueryData != NULL );
	ASSERT( nVisibleColNo >= 0 );
	ASSERT( nColNo >= 0 );
	ASSERT( rcCol.right >= rcCol.left );
	ASSERT( rcCol.bottom >= rcCol.top );
	ASSERT( rcColExtra.right >= rcColExtra.left );
	ASSERT( rcColExtra.bottom >= rcColExtra.top );
	ASSERT( rcColExtra.left <= rcCol.left );
	ASSERT( rcColExtra.top <= rcCol.top );
	ASSERT( rcColExtra.right >= rcCol.right );
	ASSERT( rcColExtra.bottom >= rcCol.bottom );
	bVirtualRightReached = OnSiwVirtualEndTestH( nColNo, LPARAM(dwAreaFlags) );
	if( bVirtualRightReached )
		return false;
	if(		rcColExtra.right == rcColExtra.left
		||	rcColExtra.bottom == rcColExtra.top
		)
		return false;
	if( dc.GetSafeHdc() == NULL )
		return false; // no hit-test like query support
	if( ! dc.RectVisible( &rcCol ) )
		return false;
CRect rcClient = OnSwGetClientRect();
LONG nVisibleRowNo = 0;
LONG nRowNo = rcVisibleRange.top;
bool bVirtualBottomReached = false;
CRect rcCellExtra(
		rcColExtra.left,
		rcClient.top,
		rcColExtra.right,
		rcClient.top
		);
	for( ; nRowNo <= rcVisibleRange.bottom; nRowNo++, nVisibleRowNo++ )
	{
		INT nExtraSpaceBefore = 0, nExtraSpaceAfter = 0;
		INT nRowHeight =
			OnSiwQueryItemExtentV(
				nRowNo,
				&nExtraSpaceBefore,
				&nExtraSpaceAfter
				);
		ASSERT( nRowHeight >= 0 );
		ASSERT( nExtraSpaceBefore >= 0 );
		ASSERT( nExtraSpaceAfter >= 0 );
		rcCellExtra.bottom += nRowHeight;
		if(		rcCellExtra.right < rcClient.left
			||	rcCellExtra.left > rcClient.right
			)
			continue;
		CRect rcCell(
			rcCol.left,
			rcCellExtra.top + nExtraSpaceBefore,
			rcCol.right,
			rcCellExtra.bottom - nExtraSpaceAfter
			);
		OnSiwWalkCell(
			dc,
			pQueryData,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			rcCellExtra,
			rcCell,
			rcVisibleRange,
			bVirtualRightReached,
			bVirtualBottomReached,
			dwAreaFlags,
			bFocusedControl
			);
		if( bVirtualRightReached || bVirtualBottomReached )
			break;
		rcCellExtra.top = rcCellExtra.bottom;
	} // for( ; nRowNo <= rcVisibleRange.bottom; nRowNo++, nVisibleRowNo++ )
	return false;
}

bool CExtScrollItemWnd::OnSiwQueryFocusedControlState() const
{
	ASSERT_VALID( this );
	if( m_hWnd == NULL || (! ::IsWindow(m_hWnd) ) )
		return false;
HWND hWndFocus = ::GetFocus();
	if(		m_hWnd == hWndFocus
		||	::IsChild( m_hWnd, hWndFocus )
		)
		return true;
	return false;
}

COLORREF CExtScrollItemWnd::OnSiwGetSysColor( int nIndex ) const
{
	ASSERT_VALID( this );
DWORD dwSiwGetStyleEx = SiwGetStyleEx();
bool bUsedPmColors =
	( (dwSiwGetStyleEx&__EGWS_EX_PM_COLORS) != 0 )
		? true : false;
COLORREF clr = bUsedPmColors
		? g_PaintManager->GetColor( nIndex )
		: ( ::GetSysColor( nIndex ) )
		;
	return clr;
}

// nLighterOrDarker is related to COLOR_3DFACE brush:
// -3 - darkest, -2 - darker, -1 - dark,
// 1 - light, 2 - lighter, 3 - lightest
CBrush & CExtScrollItemWnd::OnSiwGetLighterOrDarkerBrush( int nLighterOrDarker ) const
{
	ASSERT_VALID( this );
DWORD dwSiwGetStyleEx = SiwGetStyleEx();
bool bUsedPmColors =
	( (dwSiwGetStyleEx&__EGWS_EX_PM_COLORS) != 0 )
		? true : false;
	switch( nLighterOrDarker )
	{
	case -3:
		return bUsedPmColors ? g_PaintManager->m_brushDarkestDefault : g_PaintManager->m_brushDarkestSystem;
	case -2:
		return bUsedPmColors ? g_PaintManager->m_brushDarkerDefault : g_PaintManager->m_brushDarkerSystem;
	case -1:
		return bUsedPmColors ? g_PaintManager->m_brushDarkDefault : g_PaintManager->m_brushDarkSystem;
	case 1:
		return bUsedPmColors ? g_PaintManager->m_brushLightDefault: g_PaintManager->m_brushLightSystem;
	case 2:
		return bUsedPmColors ? g_PaintManager->m_brushLighterDefault : g_PaintManager->m_brushLighterSystem;
	default:
		ASSERT( nLighterOrDarker == 3 );
		return bUsedPmColors ? g_PaintManager->m_brushLightestDefault : g_PaintManager->m_brushLightestSystem;
	} // switch( nLighterOrDarker )
}

CFont & CExtScrollItemWnd::OnSiwGetDefaultFont() const
{
	ASSERT_VALID( this );
	return g_PaintManager->m_FontNormal;
}

void CExtScrollItemWnd::OnSwPaint( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
bool bFocusedControl = OnSiwQueryFocusedControlState();
	OnSiwPaintBackground( dc, bFocusedControl );
	OnSiwPaintForeground( dc, bFocusedControl );
}

void CExtScrollItemWnd::OnSiwPaintBackground(
	CDC & dc,
	bool bFocusedControl
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	bFocusedControl;
CRect rcRealClient;
	CWnd::GetClientRect( &rcRealClient );
	if( ! dc.RectVisible(&rcRealClient) )
		return;
CRect rcClient = OnSwGetClientRect();
	if( rcClient != rcRealClient )
	{
		COLORREF clrEntire = OnSiwGetSysColor( COLOR_3DFACE );
		dc.FillSolidRect( &rcRealClient, clrEntire );
	} // if( rcClient != rcRealClient )
	if( dc.RectVisible(&rcClient) )
	{
		COLORREF clrWindow = OnSiwGetSysColor( COLOR_WINDOW );
		dc.FillSolidRect( &rcClient, clrWindow );
	} // if( dc.RectVisible(&rcClient) )
}

void CExtScrollItemWnd::OnSiwPaintForeground(
	CDC & dc,
	bool bFocusedControl
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
CRect rcClient = OnSwGetClientRect();
	if( ! dc.RectVisible(&rcClient) )
		return;
CRect rcVisibleRange = OnSiwGetVisibleRange();
	ASSERT( rcVisibleRange.left <= rcVisibleRange.right );
	ASSERT( rcVisibleRange.top <= rcVisibleRange.bottom );
CSize _sizeItem = OnSiwCalcItemSize();
	ASSERT( _sizeItem.cx >= 0 && _sizeItem.cy >= 0 );
	if(	!(		rcVisibleRange.left <= rcVisibleRange.right
			&&	rcVisibleRange.top <= rcVisibleRange.bottom
			&&	_sizeItem.cx > 0
			&&	_sizeItem.cy > 0
			)
		)
		return;
DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	if(		(	dwScrollTypeH == __ESIW_ST_NONE
			||	dwScrollTypeH == __ESIW_ST_PIXEL
			)
		&&	(	dwScrollTypeV == __ESIW_ST_NONE
			||	dwScrollTypeV == __ESIW_ST_PIXEL
			)
		)
		return;
bool bPreferHorizontalWalk = SiwPreferHorizontalWalkGet();
	if( bPreferHorizontalWalk )
	{
		if(		dwScrollTypeH == __ESIW_ST_NONE
			||	dwScrollTypeH == __ESIW_ST_PIXEL
			)
			bPreferHorizontalWalk = false;
	} // if( bPreferHorizontalWalk )
	else
	{
		if(		dwScrollTypeV == __ESIW_ST_NONE
			||	dwScrollTypeV == __ESIW_ST_PIXEL
			)
			bPreferHorizontalWalk = true;
	} // else from if( bPreferHorizontalWalk )
CRgn rgnClientClip;
	if( rgnClientClip.CreateRectRgnIndirect(&rcClient) )
		dc.SelectClipRgn( &rgnClientClip );
COLORREF clrText = OnSiwGetSysColor( COLOR_BTNTEXT );
int nOldBkMode = dc.SetBkMode( TRANSPARENT );
COLORREF clrTextOld = dc.SetTextColor( clrText );
CFont * pOldFont = dc.SelectObject( &( OnSiwGetDefaultFont() ) );
	if( bPreferHorizontalWalk )
	{
		LONG nColNo = rcVisibleRange.left;
		LONG nVisibleColNo = 0;
		bool bVirtualRightReached = false;
		CRect rcColExtra(
			rcClient.left,
			rcClient.top,
			rcClient.left,
			rcClient.bottom
			);
		for( ; nColNo <= rcVisibleRange.right; nColNo ++, nVisibleColNo++ )
		{
			INT nExtraSpaceBefore = 0, nExtraSpaceAfter = 0;
			INT nColWidth =
				OnSiwQueryItemExtentH(
					nColNo,
					&nExtraSpaceBefore,
					&nExtraSpaceAfter
					);
			ASSERT( nColWidth >= 0 );
			ASSERT( nExtraSpaceBefore >= 0 );
			ASSERT( nExtraSpaceAfter >= 0 );
			if( nColWidth == 0 )
				continue;
			rcColExtra.right += nColWidth;
			CRect rcCol( rcColExtra );
			rcCol.DeflateRect(
				nExtraSpaceBefore,
				0,
				nExtraSpaceAfter,
				0
				);
			ASSERT( rcCol.left <= rcCol.right );
			OnSiwWalkItemsV(
				dc,
				NULL,
				rcColExtra,
				rcCol,
				nVisibleColNo,
				nColNo,
				rcVisibleRange,
				bVirtualRightReached,
				0L,
				bFocusedControl
				);
			if( bVirtualRightReached )
				break;
			rcColExtra.left = rcColExtra.right;
		}
	} // if( bPreferHorizontalWalk )
	else
	{
		LONG nRowNo = rcVisibleRange.top;
		LONG nVisibleRowNo = 0;
		bool bVirtualBottomReached = false;
		CRect rcRowExtra(
			rcClient.left,
			rcClient.top,
			rcClient.right,
			rcClient.top
			);
		for( ; nRowNo <= rcVisibleRange.bottom; nRowNo ++, nVisibleRowNo++ )
		{
			INT nExtraSpaceBefore = 0, nExtraSpaceAfter = 0;
			INT nRowHeight =
				OnSiwQueryItemExtentV(
					nRowNo,
					&nExtraSpaceBefore,
					&nExtraSpaceAfter
					);
			ASSERT( nRowHeight >= 0 );
			ASSERT( nExtraSpaceBefore >= 0 );
			ASSERT( nExtraSpaceAfter >= 0 );
			if( nRowHeight == 0 )
				continue;
			rcRowExtra.bottom += nRowHeight;
			CRect rcRow( rcRowExtra );
			rcRow.DeflateRect(
				0,
				nExtraSpaceBefore,
				0,
				nExtraSpaceAfter
				);
			ASSERT( rcRow.top <= rcRow.bottom );
			OnSiwWalkItemsH(
				dc,
				NULL,
				rcRowExtra,
				rcRow,
				nVisibleRowNo,
				nRowNo,
				rcVisibleRange,
				bVirtualBottomReached,
				0L,
				bFocusedControl
				);
			if( bVirtualBottomReached )
				break;
			rcRowExtra.top = rcRowExtra.bottom;
		}
	} // else from if( bPreferHorizontalWalk )
	dc.SelectObject( pOldFont );
	dc.SetTextColor( clrTextOld );
	dc.SetBkMode( nOldBkMode );
	dc.SelectClipRgn( NULL );
}

bool CExtScrollItemWnd::OnSwDoScroll(
	UINT nScrollCode,
	UINT nPos,
	bool bDoScroll // = true
	)
{
	ASSERT_VALID( this );
DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();
LONG xPos, xOrgValue, yPos, yOrgValue;
	xOrgValue = xPos = ScrollPos32Get( SB_HORZ );
	switch( LOBYTE(nScrollCode) )
	{
	case SB_TOP:		xPos  = 0;							break;
	case SB_BOTTOM:		xPos  = __EXT_SCROLL_NUMERIC_MAX;	break;
	case SB_LINEUP:		xPos -= OnSwGetLineSize( -1 ).cx;	break;
	case SB_LINEDOWN:	xPos += OnSwGetLineSize(  1 ).cx;	break;
	case SB_PAGEUP:		xPos -= OnSwGetPageSize( -1 ).cx;	break;
	case SB_PAGEDOWN:
		{
			LONG nPageSize = OnSwGetPageSize( 1 ).cx;
			if( dwScrollTypeH != __ESIW_ST_VIRTUAL )
				xPos += nPageSize;
			else
			{
				if( (__EXT_SCROLL_NUMERIC_MAX - nPageSize) < xPos )
					xPos = __EXT_SCROLL_NUMERIC_MAX;
				else
					xPos += nPageSize;
			} // else from if( dwScrollTypeH != __ESIW_ST_VIRTUAL )
		}
		break;
	case SB_THUMBTRACK:
		if( ! OnSwQueryThumbTrackEnabled(true) )
			return false;
	case SB_THUMBPOSITION:
		if( dwScrollTypeH != __ESIW_ST_VIRTUAL )
		{
			if( m_bUse32BitScrollInfo )
				nPos = ScrollPos32Get( SB_HORZ, true );
			xPos = nPos;
		} // if( dwScrollTypeH != __ESIW_ST_VIRTUAL )
		else
		{
			if( nPos <= 0 )
				xPos = 0;
			else
				xPos = __EXT_SCROLL_NUMERIC_MAX;
		} // else from if( dwScrollTypeH != __ESIW_ST_VIRTUAL )
		break;
	}
	yOrgValue = yPos = ScrollPos32Get( SB_VERT );
	switch( HIBYTE(nScrollCode) )
	{
	case SB_TOP:		yPos  = 0;							break;
	case SB_BOTTOM:		yPos  = __EXT_SCROLL_NUMERIC_MAX;	break;
	case SB_LINEUP:		yPos -= OnSwGetLineSize( -1 ).cy;	break;
	case SB_LINEDOWN:	yPos += OnSwGetLineSize(  1 ).cy;	break;
	case SB_PAGEUP:		yPos -= OnSwGetPageSize( -1 ).cy;	break;
	case SB_PAGEDOWN:
		{
			LONG nPageSize = OnSwGetPageSize( 1 ).cy;
			if( dwScrollTypeV != __ESIW_ST_VIRTUAL )
				yPos += nPageSize;
			else
			{
				if( (__EXT_SCROLL_NUMERIC_MAX - nPageSize) < yPos )
					yPos = __EXT_SCROLL_NUMERIC_MAX;
				else
					yPos += nPageSize;
			} // else from if( dwScrollTypeV != __ESIW_ST_VIRTUAL )
		}
		break;
	case SB_THUMBTRACK:
		if( ! OnSwQueryThumbTrackEnabled(false) )
			return false;
	case SB_THUMBPOSITION:
		if( dwScrollTypeV != __ESIW_ST_VIRTUAL )
		{
			if( m_bUse32BitScrollInfo )
				nPos = ScrollPos32Get( SB_VERT, true );
			yPos = (int)nPos;
		} // if( dwScrollTypeV != __ESIW_ST_VIRTUAL )
		else
		{
			if( nPos <= 0 )
				yPos = 0;
			else
				yPos = __EXT_SCROLL_NUMERIC_MAX;
		} // else from if( dwScrollTypeV != __ESIW_ST_VIRTUAL )
		break;
	}
bool bResult =
		OnSwDoScrollBy(
			CSize( xPos - xOrgValue, yPos - yOrgValue ),
			bDoScroll
			);
	if( bResult && bDoScroll && m_bScrollUpdateWindow )
		CWnd::UpdateWindow();
	return bResult;
}

bool CExtScrollItemWnd::SiwFireCacheChanging(
	LONG nScrollPosNewX, // = -1L // if <0L - use current
	LONG nScrollPosNewY, // = -1L // if <0L - use current
	bool bUpdate // = true
	)
{
	ASSERT_VALID( this );
LONG	xMaxValue = 0L, xPos = 0L, xPosOld = 0L,
		yMaxValue = 0L, yPos = 0L, yPosOld = 0L;
	if( bUpdate || OnSwHasScrollBar(true) )
	{
		xPos = ScrollPos32Get( SB_HORZ );
		xMaxValue = ScrollLimit32Get( SB_HORZ );
		if( nScrollPosNewX >= 0L )
		{
			xPosOld = xPos;
			xPos = nScrollPosNewX;
		} // if( nScrollPosNewX >= 0L )
		if( xPos < 0 )
			xPos = 0;
		else if( xPos > xMaxValue )
			xPos = xMaxValue;
		if( bUpdate && xPos != xPosOld  )
			ScrollPos32Set( SB_HORZ, xPos, false );
	} // if( bUpdate || OnSwHasScrollBar(true) )
	if( bUpdate || OnSwHasScrollBar(false) )
	{
		yPos = ScrollPos32Get( SB_VERT );
		yMaxValue = ScrollLimit32Get( SB_VERT );
		if( nScrollPosNewY >= 0L )
		{
			yPosOld = yPos;
			yPos = nScrollPosNewY;
		} // if( nScrollPosNewY >= 0L )
		if( yPos < 0 )
			yPos = 0;
		else if( yPos > yMaxValue )
			yPos = yMaxValue;
		if( bUpdate && yPos != yPosOld  )
			ScrollPos32Set( SB_VERT, yPos, false );
	} // if( bUpdate || OnSwHasScrollBar(false) )
bool	bBeginX = ( (xPos == 0) ? true : false ),
		bBeginY = ( (yPos == 0) ? true : false ),
		bEndX = ( (xPos == xMaxValue && xMaxValue > 0) ? true : false ),
		bEndY = ( (yPos == yMaxValue && yMaxValue > 0) ? true : false );
CSize _sizePage = OnSwGetPageSize( 0 );
	if( _sizePage.cx < 0 )
		_sizePage.cx = 0;
	if( _sizePage.cy < 0 )
		_sizePage.cy = 0;
	m_sciLast.Empty();
bool bRetVal =
		_NotifyCacheChanged(
			xPos, xPosOld, xMaxValue, _sizePage.cx,
			yPos, yPosOld, yMaxValue, _sizePage.cy,
			bBeginX, bEndX,
			bBeginY, bEndY
			);
	if( bUpdate )
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	} // if( bUpdate )
	return bRetVal;
}

bool CExtScrollItemWnd::OnSwDoScrollBy(
	CSize sizeScroll,
	bool bDoScroll // = true
	)
{
	ASSERT_VALID( this );
LONG xMaxValue, xOrgValue, xPos, yMaxValue, yOrgValue, yPos;
	if( ! OnSwHasScrollBar(false) )
		sizeScroll.cy = 0;
	if( ! OnSwHasScrollBar(true) )
		sizeScroll.cx = 0;

	xOrgValue = xPos = ScrollPos32Get( SB_HORZ );
	xMaxValue = ScrollLimit32Get( SB_HORZ );
	xPos += sizeScroll.cx;
	if( xPos < 0 )
		xPos = 0;
	else if( xPos > xMaxValue )
		xPos = xMaxValue;

	yOrgValue = yPos = ScrollPos32Get( SB_VERT );
	yMaxValue = ScrollLimit32Get( SB_VERT );
	yPos += sizeScroll.cy;
	if( yPos < 0 )
		yPos = 0;
	else if( yPos > yMaxValue )
		yPos = yMaxValue;
	
bool	bBeginX = ( (xPos == 0) ? true : false ),
		bBeginY = ( (yPos == 0) ? true : false ),
		bEndX = ( (xPos == xMaxValue && xMaxValue > 0) ? true : false ),
		bEndY = ( (yPos == yMaxValue && yMaxValue > 0) ? true : false );
CSize _sizePage = OnSwGetPageSize( 0 );
bool bRetVal =
		_NotifyCacheChanged(
			xPos, xOrgValue, xMaxValue, _sizePage.cx,
			yPos, yOrgValue, yMaxValue, _sizePage.cy,
			bBeginX, bEndX,
			bBeginY, bEndY
			);
	if( !bRetVal )
		return false;

	if(		xPos == xOrgValue
		&&	yPos == yOrgValue
		)
		return false;

	if( bDoScroll )
	{
		CSize sizeItem = OnSiwCalcItemSize();
		int xAmount = xOrgValue - xPos;
		int yAmount = yOrgValue - yPos;
		xAmount *= sizeItem.cx;
		yAmount *= sizeItem.cy;
		OnSwDoScrollWindow(
			xAmount,
			yAmount
			);
		if( xPos != xOrgValue )
			ScrollPos32Set( SB_HORZ, xPos );
		if( yPos != yOrgValue )
			ScrollPos32Set( SB_VERT, yPos );
	} // if( bDoScroll )

	return true;
}

CPoint CExtScrollItemWnd::OnSwGetScrollPaintPos() const
{
	ASSERT_VALID( this );
	return CPoint( 0, 0 );
}

CRect CExtScrollItemWnd::OnSiwGetVisibleRange() const
{
	ASSERT_VALID( this );
DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();
CSize _sizeTotal = OnSwGetTotalSize();
CSize _sizePage = OnSiwCalcPageMetrics( 0 );
CSize _sizeRange(
		(dwScrollTypeH == __ESIW_ST_NONE) ? _sizeTotal.cx : ( min( _sizeTotal.cx, _sizePage.cx ) ),
		(dwScrollTypeV == __ESIW_ST_NONE) ? _sizeTotal.cy : ( min( _sizeTotal.cy, _sizePage.cy ) )
		);
CPoint ptMove = OnSwGetScrollPos();
CRect rcVisibleRange( ptMove, _sizeRange );
	if( ((ULONG)rcVisibleRange.right) >= ((ULONG)_sizeTotal.cx)
		&&	dwScrollTypeH != __ESIW_ST_NONE
		&&	dwScrollTypeH != __ESIW_ST_VIRTUAL
		)
		rcVisibleRange.right = _sizeTotal.cx-1;
	if( ((ULONG)rcVisibleRange.right) < ((ULONG)rcVisibleRange.left) )
		rcVisibleRange.right = rcVisibleRange.left;
	if( ((ULONG)rcVisibleRange.bottom) >= ((ULONG)_sizeTotal.cy)
		&&	dwScrollTypeV != __ESIW_ST_NONE
		&&	dwScrollTypeV != __ESIW_ST_VIRTUAL
		)
		rcVisibleRange.bottom = _sizeTotal.cy-1;
	if( ((ULONG)rcVisibleRange.bottom) < ((ULONG)rcVisibleRange.top) )
		rcVisibleRange.bottom = rcVisibleRange.top;
	if( dwScrollTypeH == __ESIW_ST_VIRTUAL )
		rcVisibleRange.OffsetRect( -rcVisibleRange.left, 0 );
	if( dwScrollTypeV == __ESIW_ST_VIRTUAL )
		rcVisibleRange.OffsetRect( 0, -rcVisibleRange.top );
	return rcVisibleRange;
}

void CExtScrollItemWnd::OnSwGetScrollBarState(
	CSize sizeClient,
	CSize & sizeNeedSB,
	CSize & sizeRange,
	CPoint & ptMove,
	bool bInsideClient
	) const
{
	ASSERT_VALID( this );

DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();

bool bHasSbH0 = OnSwHasScrollBar( true );
bool bHasSbV0 = OnSwHasScrollBar( false );

CSize sizeSb = OnSwGetScrollBarSizes();
	sizeRange = OnSwGetTotalSize() - sizeClient;
	ptMove = OnSwGetScrollPos();
bool bNeedH = sizeRange.cx > 0;
	if( !bNeedH )
		ptMove.x = 0;
	else if(
				bInsideClient
			&&	dwScrollTypeV == __ESIW_ST_PIXEL
			&&	( bHasSbH0 )
			)
		sizeRange.cy += sizeSb.cy;
bool bNeedV = sizeRange.cy > 0;
	if( !bNeedV )
		ptMove.y = 0;
	else if (
				bInsideClient
			&&	dwScrollTypeH == __ESIW_ST_PIXEL
			&&	( bHasSbV0 )
			)
			sizeRange.cx += sizeSb.cx;
	if(		bNeedV
		&&	(!bNeedH)
		&&	sizeRange.cx > 0
		)
	{
		ASSERT( bInsideClient );
		bNeedH = true;
		if( dwScrollTypeV == __ESIW_ST_PIXEL )
			sizeRange.cy += sizeSb.cy;
	} // if( bNeedV ...
	if( sizeRange.cx > 0 && ptMove.x >= sizeRange.cx )
		ptMove.x = sizeRange.cx;
	if( sizeRange.cy > 0 && ptMove.y >= sizeRange.cy )
		ptMove.y = sizeRange.cy;
	sizeNeedSB.cx = bNeedH;
	sizeNeedSB.cy = bNeedV;


CPoint ptMove_ISR = OnSwGetScrollPos();

	if(		dwScrollTypeH == __ESIW_ST_ITEM
		||	dwScrollTypeH == __ESIW_ST_VIRTUAL
		)
		ptMove.x = ptMove_ISR.x;
	else if( dwScrollTypeH == __ESIW_ST_NONE )
		sizeNeedSB.cx = 0;
	
	if(		dwScrollTypeV == __ESIW_ST_ITEM
		||	dwScrollTypeV == __ESIW_ST_VIRTUAL
		)
		ptMove.y = ptMove_ISR.y;
	else if( dwScrollTypeV == __ESIW_ST_NONE )
		sizeNeedSB.cy = 0;
}

bool CExtScrollItemWnd::OnSwCanAutoHideScrollBar( bool bHorz ) const
{
	ASSERT_VALID( this );
	return SiwAutohideScrollBarGet( bHorz );
}

bool CExtScrollItemWnd::_NotifyCacheChanged(
	LONG nPosH,
	LONG nPosOldH,
	LONG nMaxH,
	LONG nPageSizeH,
	LONG nPosV,
	LONG nPosOldV,
	LONG nMaxV,
	LONG nPageSizeV,
	bool bBeginH,
	bool bEndH,
	bool bBeginV,
	bool bEndV
	)
{
	ASSERT_VALID( this );
	nPosOldH;
	nPosOldV;
	ASSERT(
			nPosH >= 0L
		&&	nPosOldH >= 0L
		&&	nPosV >= 0L
		&&	nPosOldV >= 0L
		&&	nMaxH >= 0L
		&&	nMaxV >= 0L
		&&	nPageSizeH >= 0L
		&&	nPageSizeV >= 0L
		);
	ASSERT( 0L <= nPosH && nPosH <= nMaxH );
	ASSERT( 0L <= nPosOldH && nPosOldH <= nMaxH );
	ASSERT( 0L <= nPosV && nPosV <= nMaxV );
	ASSERT( 0L <= nPosOldV && nPosOldV <= nMaxV );

//DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	if(		dwScrollTypeV != __ESIW_ST_NONE
		&&	dwScrollTypeV != __ESIW_ST_PIXEL
		&&	OnSwHasScrollBar( false )
		)
	{
		if( nPosV < nMaxV )
		{
			nPageSizeV++;
		}
	}

CExtScrollItemCacheInfo _sciNew(
		nPosH,
		nMaxH,
		nPageSizeH,
		nPosV,
		nMaxV,
		nPageSizeV,
		bBeginH,
		bEndH,
		bBeginV,
		bEndV
		);
	ASSERT( _sciNew.IsValid() );
	if( m_sciLast == _sciNew )
		return true;
	if( ! OnSiwCacheChanged( _sciNew, m_sciLast ) )
		return false;
	m_sciLast = _sciNew;
	return true;
}

bool CExtScrollItemWnd::OnSiwCacheChanged(
	const CExtScrollItemCacheInfo & _sciNew,
	const CExtScrollItemCacheInfo & _sciOld
	)
{
	ASSERT_VALID( this );
	ASSERT( _sciNew.IsValid() );
	ASSERT( _sciOld.IsValid() );
	_sciNew;
	_sciOld;
///////////////////////////////////////////////////////////////////////
/// DEBUG BLOCK (BEGIN): verify cache notification is valid
///////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
LONG nPosChangingH = _sciNew.m_nPosH - _sciOld.m_nPosH;
LONG nPosChangingV = _sciNew.m_nPosV - _sciOld.m_nPosV;
	ASSERT( nPosChangingH == _sciNew.GetPosChangingH(_sciOld) );
	ASSERT( nPosChangingV == _sciNew.GetPosChangingV(_sciOld) );
LONG nPageSizeChangingH = _sciNew.m_nPageSizeH - _sciOld.m_nPageSizeH;
LONG nPageSizeChangingV = _sciNew.m_nPageSizeV - _sciOld.m_nPageSizeV;
	ASSERT( nPageSizeChangingH == _sciNew.GetPageSizeChangingH(_sciOld) );
	ASSERT( nPageSizeChangingV == _sciNew.GetPageSizeChangingV(_sciOld) );
LONG nMaxChangingH = _sciNew.m_nMaxH - _sciOld.m_nMaxH;
LONG nMaxChangingV = _sciNew.m_nMaxV - _sciOld.m_nMaxV;
	ASSERT( nMaxChangingH == _sciNew.GetMaxChangingH(_sciOld) );
	ASSERT( nMaxChangingV == _sciNew.GetMaxChangingV(_sciOld) );
LONG nExtentH_New = _sciNew.m_nMaxH + _sciNew.m_nPageSizeH;
LONG nExtentH_Old = _sciOld.m_nMaxH + _sciOld.m_nPageSizeH;
LONG nExtentV_New = _sciNew.m_nMaxV + _sciNew.m_nPageSizeV;
LONG nExtentV_Old = _sciOld.m_nMaxV + _sciOld.m_nPageSizeV;
LONG nExtentChangingH = nExtentH_New - nExtentH_Old;
LONG nExtentChangingV = nExtentV_New - nExtentV_Old;
	ASSERT( nExtentChangingH == _sciNew.GetExtentChangingH(_sciOld) );
	ASSERT( nExtentChangingV == _sciNew.GetExtentChangingV(_sciOld) );
	if(			nPosChangingH == 0L
			&&	nPosChangingV == 0L
			&&	nPageSizeChangingH == 0L
			&&	nPageSizeChangingV == 0L
			&&	nMaxChangingH == 0L
			&&	nMaxChangingV == 0L
			&&	nExtentChangingH == 0L
			&&	nExtentChangingV == 0L
		)
	{
		ASSERT( FALSE );
	}
//CString strTrace;
//	strTrace.Format(
//		_T("   --> CExtScrollItemWnd::OnSiwCacheChanged()\n")
//		_T("   -->    nPosNewH=%08ld nMaxH=%08ld PG=%08ld BEGIN=%s END=%s\n")
//		_T("   -->    nPosNewV=%08ld nMaxV=%08ld PG=%08ld BEGIN=%s END=%s\n")
//		,
//		_sciNew.m_nPosH, _sciNew.m_nMaxH, _sciNew.m_nPageSizeH,
//			_sciNew.m_bBeginH ? _T("yes") : _T("not"),
//			_sciNew.m_bEndH ? _T("yes") : _T("not"),
//		_sciNew.m_nPosV, _sciNew.m_nMaxV, _sciNew.m_nPageSizeV,
//			_sciNew.m_bBeginV ? _T("yes") : _T("not"),
//			_sciNew.m_bEndV ? _T("yes") : _T("not")
//		);
//	TRACE0( LPCTSTR(strTrace) );
#endif // _DEBUG
///////////////////////////////////////////////////////////////////////
/// DEBUG BLOCK (END): verify cache notification is valid
///////////////////////////////////////////////////////////////////////
	return true;
}

#endif // (!defined __EXT_MFC_NO_SCROLLITEMWND)

#endif // (!defined __EXT_MFC_NO_SCROLLWND)

