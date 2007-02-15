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

#if (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)

#if (!defined __EXTSHORTCUTLISTWND_H)
	#include <ExtShortcutListWnd.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtShortcutListWnd::CItemTextEditWnd

// CEdit syntax of Create() method
BOOL CExtShortcutListWnd::CItemTextEditWnd::Create(
	DWORD dwStyle,
	const RECT & rect,
	CWnd * pParentWnd,
	UINT nID
	)
{
	ASSERT( m_pSII != NULL );
	if( !CEdit::Create(
			dwStyle,
			rect,
			pParentWnd,
			nID
			)
		)
	{
		ASSERT( FALSE );
		delete this;
		return FALSE;
	}
	return TRUE;
}

void CExtShortcutListWnd::CItemTextEditWnd::PostNcDestroy()
{
	delete this;
}

void CExtShortcutListWnd::CItemTextEditWnd::DoEndEdit( bool bUpdateItemText )
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT( m_pSII != NULL );

	if( m_bInEndEdit )
		return;
	m_bInEndEdit = true;

	CExtShortcutListWnd * pWndParent =
		STATIC_DOWNCAST(
			CExtShortcutListWnd,
			GetParent()
			);
	ASSERT( pWndParent != NULL );

CWnd * pWndTopParent = pWndParent->GetTopLevelParent();
	if( pWndParent->OnShortcutListDeactivateEditor(
			m_pSII,
			bUpdateItemText
			)
		&& bUpdateItemText
		)
	{
		CString sWindowText;
		GetWindowText( sWindowText );
		m_pSII->m_sText = sWindowText;
	}

	pWndParent->SendMessage( WM_CANCELMODE );

//	pWndParent->SetFocus();
	if( pWndTopParent->GetSafeHwnd() != NULL )
		pWndTopParent->SetFocus();

	DestroyWindow();
}

LRESULT CExtShortcutListWnd::CItemTextEditWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID( this );
	ASSERT( m_pSII != NULL );

	switch( message )
	{
	case WM_NCCALCSIZE:
	{
		NCCALCSIZE_PARAMS * pNCCSP =
			reinterpret_cast < NCCALCSIZE_PARAMS * > ( lParam );
		ASSERT( pNCCSP != NULL );
		
		CRect rcInBarWnd( pNCCSP->rgrc[0] );
		rcInBarWnd.DeflateRect(
			__EXT_SHORTCUTLISTWND_EXPANDED_TEXT_GAP_X,
			__EXT_SHORTCUTLISTWND_EXPANDED_TEXT_GAP_Y
			);
		::CopyRect( &(pNCCSP->rgrc[0]), rcInBarWnd );
		
		return 0;
	} // case WM_NCCALCSIZE
	case WM_NCPAINT:
	{
		CRect rcInBarWnd, rcInBarClient;
		GetWindowRect( &rcInBarWnd );
		GetClientRect( &rcInBarClient );
		ClientToScreen( &rcInBarClient );
		if( rcInBarWnd == rcInBarClient )
			return 0;
		CPoint ptDevOffset = -rcInBarWnd.TopLeft();
		rcInBarWnd.OffsetRect( ptDevOffset );
		rcInBarClient.OffsetRect( ptDevOffset );

		CWindowDC dc( this );
		ASSERT( dc.GetSafeHdc() != NULL );
		dc.ExcludeClipRect( &rcInBarClient );

		dc.FillSolidRect(
			&rcInBarWnd,
			g_PaintManager->GetColor( COLOR_3DDKSHADOW )
			);

		return 0;
	} // case WM_NCPAINT
	case WM_KILLFOCUS:
	{
		HWND hWndFocus = (HWND)wParam;
		if(	hWndFocus != NULL
			&& hWndFocus == GetParent()->GetSafeHwnd()
			)
			DoEndEdit( true );
		else
			DoEndEdit( false );
		return 0;
	} // case WM_KILLFOCUS
	case WM_ACTIVATEAPP:
		if( !wParam )
			DoEndEdit( false );
		return 0;
	case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS;
	case WM_KEYDOWN:
	{
		switch( int(wParam) )
		{
		case VK_RETURN:
			DoEndEdit( true );
			return 0;
		case VK_ESCAPE:
		{
			DoEndEdit( false );
			return 0;
		} // case VK_ESCAPE
		case VK_TAB:
		{
			HWND hWndParent = GetParent()->GetSafeHwnd();
			if( hWndParent != NULL )
			{
				if( ( ((DWORD)::GetWindowLong(hWndParent,GWL_STYLE)) & WS_POPUP ) == 0 )
				{
					hWndParent = ::GetParent(hWndParent);
					if( hWndParent != NULL )
					{
						::SendMessage(
							hWndParent,
							WM_NEXTDLGCTL,
							CExtPopupMenuWnd::IsKeyPressed(VK_SHIFT),
							0
							);
					}
				}
			}
			return 0;
		} // case VK_TAB
		case VK_UP:
		case VK_DOWN:
		case VK_PRIOR:
		case VK_NEXT:
		case VK_HOME:
		case VK_END:
		{
			HWND hWndParent = GetParent()->GetSafeHwnd();
			if( hWndParent != NULL )
			{
				if(		int(wParam) == VK_HOME
					||	int(wParam) == VK_END
					)
				{
					if( ! CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
						break;
				}
				DoEndEdit( true );
				return
					::SendMessage(hWndParent,WM_KEYDOWN,wParam,lParam);
			}
			return 0;
		} // case VK_...
		} // switch( int(wParam) )
	}
	break; // case WM_KEYDOWN
	} // switch( message )

LRESULT lResult =
		CEdit::WindowProc(message,wParam,lParam);
	return lResult;
}

/////////////////////////////////////////////////////////////////////////////
// CExtShortcutListWnd

IMPLEMENT_DYNCREATE( CExtShortcutListWnd, CWnd );

CExtShortcutListWnd::CExtShortcutListWnd()
	: m_bDirectCreateCall( false )
	, m_bDelayRecalcLayout( true )
	, m_bScrolling( false )
	, m_bCancelingActionsMode( false )
	, m_bInLayoutRecalcProc( false )
	, m_bPressedTracking( false )
	, m_nTrackingHT( __ESLW_HT_NOWHERE )
	, m_dwShortcutListStyle( __ESLW_DEF_STYLES )
	, m_hWndEditor( NULL )
	, m_nScrollPos( 0L )
	, m_nScrollSize( 0L )
	, m_nScrollStep( 0L )
	, m_nIndexVisibleFirst( 0L )
	, m_nIndexVisibleLast( 0L )
	, m_nScrollPeriod( 0L )
	, m_ptStartLeftBtnTrack( -1, -1 )
{
	VERIFY( RegisterShortcutListWndClass() );
}

CExtShortcutListWnd::~CExtShortcutListWnd()
{
	ItemRemoveAll( false );
}

BEGIN_MESSAGE_MAP(CExtShortcutListWnd, CWnd)
	//{{AFX_MSG_MAP(CExtShortcutListWnd)
	ON_WM_MOUSEACTIVATE()
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CANCELMODE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP

	ON_WM_ACTIVATEAPP()
	ON_REGISTERED_MESSAGE(
		CExtContentExpandWnd::g_nMsgPaintItemContent,
		_OnPaintExpanedItemContent
		)

END_MESSAGE_MAP()

bool CExtShortcutListWnd::g_bShortcutListWndClassRegistered = false;

bool CExtShortcutListWnd::RegisterShortcutListWndClass()
{
	if( g_bShortcutListWndClassRegistered )
		return true;

WNDCLASS wndclass;
HINSTANCE hInst = AfxGetInstanceHandle();
	if( ! ::GetClassInfo(
			hInst,
			__EXT_SHORTCUTLISTWND_CLASS_NAME,
			&wndclass
			)
		)
	{
		// otherwise we need to register a new class
		wndclass.style = CS_GLOBALCLASS; // |CS_DBLCLKS;
		wndclass.lpfnWndProc = ::DefWindowProc;
		wndclass.cbClsExtra = wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInst;
		wndclass.hIcon = NULL;
		wndclass.hCursor =
//			( g_hCursor != NULL )
//				? g_hCursor
//				:
				::LoadCursor(
					NULL, //hInst,
					IDC_ARROW
					)
				;
		ASSERT( wndclass.hCursor != NULL );
		wndclass.hbrBackground = NULL; 
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = __EXT_SHORTCUTLISTWND_CLASS_NAME;
		if( !::AfxRegisterClass( &wndclass ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}

	g_bShortcutListWndClassRegistered = true;
	return true;
}

BOOL CExtShortcutListWnd::Create(
	CWnd * pParentWnd,
	const RECT & rcWnd, // = CRect( 0, 0, 0, 0 )
	UINT nDlgCtrlID, // = UINT( IDC_STATIC )
	DWORD dwWindowStyle, // = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS
	DWORD dwShortcutListStyle, // = __ESLW_DEF_STYLES
	CCreateContext * pContext // = NULL
	)
{
	if( !RegisterShortcutListWndClass() )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	m_bDirectCreateCall = true;
	m_dwShortcutListStyle = dwShortcutListStyle;
	if( ! CWnd::Create(
			__EXT_SHORTCUTLISTWND_CLASS_NAME,
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

void CExtShortcutListWnd::UpdateShortcutListWnd( bool bUpdateNow )
{
	ASSERT_VALID( this );

	m_bDelayRecalcLayout = true;

	if( m_hWndEditor != NULL )
	{
		OnShortcutListCancelItemEditor( m_hWndEditor );
		m_hWndEditor = NULL;
	}

	if(		bUpdateNow
		&&	GetSafeHwnd() != NULL
		&&	::IsWindow( GetSafeHwnd() )
		)
	{
		_RecalcLayoutImpl();
		Invalidate();
		UpdateWindow();
	}
}

void CExtShortcutListWnd::_RecalcLayoutImpl()
{
	ASSERT_VALID( this );

	if(		( !m_bDelayRecalcLayout )
		||	m_bInLayoutRecalcProc
		||	GetSafeHwnd() == NULL
		||	( ! ::IsWindow( GetSafeHwnd() ) )
		)
		return;

	m_bInLayoutRecalcProc = true;
	m_bDelayRecalcLayout = false;

	m_nIndexVisibleFirst = m_nIndexVisibleLast = 0L;
CRect rcClient;
bool bHorzLayout = IsHorizontalLayout();
	GetClientRect( &rcClient );
LONG nItemCount = ItemGetCount();
	if( nItemCount > 0 )
	{
		CClientDC dc( this );
		LONG nItemOffset = 0L;
		for( LONG nIndex = 0; nIndex < nItemCount; nIndex++ )
		{
			SHORTCUT_ITEM_INFO * pSII = ItemGetInfo( nIndex );
			ASSERT( pSII != NULL );
			if( ! pSII->m_bVisible )
				continue;
			OnShortcutListItemMeasure(
				dc,
				rcClient,
				*pSII
				);
			if( bHorzLayout )
			{
				pSII->m_rcBoundsIcon.OffsetRect( nItemOffset, 0 );
				pSII->m_rcBoundsText.OffsetRect( nItemOffset, 0 );
				pSII->m_rcBoundsAll.OffsetRect( nItemOffset, 0 );
				LONG nItemExtent = pSII->m_rcBoundsAll.Size().cx;
				nItemOffset += nItemExtent;
			} // if( bHorzLayout )
			else
			{
				pSII->m_rcBoundsIcon.OffsetRect( 0, nItemOffset );
				pSII->m_rcBoundsText.OffsetRect( 0, nItemOffset );
				pSII->m_rcBoundsAll.OffsetRect( 0, nItemOffset );
				LONG nItemExtent = pSII->m_rcBoundsAll.Size().cy;
				nItemOffset += nItemExtent;
			} // else from if( bHorzLayout )
		} // for( LONG nIndex = 0; nIndex < nItemCount; nIndex++ )

		if( bHorzLayout )
		{
			m_nScrollSize = nItemOffset - rcClient.Width();
			if( m_nScrollSize < 0 )
				m_nScrollSize = 0;
		} // if( bHorzLayout )
		else
		{
			m_nScrollSize = nItemOffset - rcClient.Height();
			if( m_nScrollSize < 0 )
				m_nScrollSize = 0;
		} // else from if( bHorzLayout )
		if( m_nScrollPos < 0 )
			m_nScrollPos = 0;
		else if( m_nScrollPos > m_nScrollSize )
			m_nScrollPos = m_nScrollSize;

		LONG nTestClientStart =
			bHorzLayout
				? rcClient.left
				: rcClient.top
				;
		LONG nTestClientEnd =
			bHorzLayout
				? rcClient.right
				: rcClient.bottom
				;
		bool bSetFirst = false;
		for( nIndex = 0; nIndex < nItemCount; nIndex++ )
		{
			SHORTCUT_ITEM_INFO * pSII = ItemGetInfo( nIndex );
			ASSERT( pSII != NULL );
			if( ! pSII->m_bVisible )
				continue;
			CRect rcBoundsItem;
			ItemGetRects( nIndex, NULL, NULL, rcBoundsItem );
			LONG nTestItemStart =
				bHorzLayout
					? rcBoundsItem.left
					: rcBoundsItem.top
					;
			LONG nTestItemEnd =
				bHorzLayout
					? rcBoundsItem.right
					: rcBoundsItem.bottom
					;
			if(		(!bSetFirst)
				&&	nTestItemEnd > nTestClientStart
				)
			{
				bSetFirst = true;
				m_nIndexVisibleFirst = m_nIndexVisibleLast = nIndex;
				continue;
			}
			if( nTestItemStart > nTestClientEnd )
				break;
			m_nIndexVisibleLast = nIndex;
		} // for( nIndex = 0; nIndex < nItemCount; nIndex++ )
	
	} // if( nItemCount > 0 )
	else
	{
		m_nScrollPos = m_nScrollSize = 0L;
	} // else from if( nItemCount > 0 )

	OnShortcutListCalcButtonsPos(
		m_rcButtonUp,
		m_rcButtonDown,
		rcClient,
		bHorzLayout
		);

	m_bInLayoutRecalcProc = false;
}

void CExtShortcutListWnd::OnShortcutListCalcButtonsPos(
	CRect & rcButtonUp,
	CRect & rcButtonDown,
	const CRect & rcClient,
	bool bHorzLayout
	)
{
	ASSERT_VALID( this );

CSize sizeButton(
		::GetSystemMetrics( SM_CXVSCROLL ),
		::GetSystemMetrics( SM_CYHSCROLL )
		);
	if( bHorzLayout )
	{
		rcButtonUp.SetRect(
			rcClient.left + __EXT_SHORTCUTLISTWND_BTN2BORDER_DX,
			rcClient.top + __EXT_SHORTCUTLISTWND_BTN2BORDER_DY,
			rcClient.left + __EXT_SHORTCUTLISTWND_BTN2BORDER_DX + sizeButton.cx,
			rcClient.top + __EXT_SHORTCUTLISTWND_BTN2BORDER_DY + sizeButton.cy
			);
		rcButtonDown.SetRect(
			rcClient.right - __EXT_SHORTCUTLISTWND_BTN2BORDER_DX - sizeButton.cx,
			rcClient.top + __EXT_SHORTCUTLISTWND_BTN2BORDER_DY,
			rcClient.right - __EXT_SHORTCUTLISTWND_BTN2BORDER_DX,
			rcClient.top + __EXT_SHORTCUTLISTWND_BTN2BORDER_DY + sizeButton.cy
			);
	} // if( bHorzLayout )
	else
	{
		rcButtonUp.SetRect(
			rcClient.right - __EXT_SHORTCUTLISTWND_BTN2BORDER_DX - sizeButton.cx,
			rcClient.top + __EXT_SHORTCUTLISTWND_BTN2BORDER_DY,
			rcClient.right - __EXT_SHORTCUTLISTWND_BTN2BORDER_DX,
			rcClient.top + __EXT_SHORTCUTLISTWND_BTN2BORDER_DY + sizeButton.cy
			);
		rcButtonDown.SetRect(
			rcClient.right - __EXT_SHORTCUTLISTWND_BTN2BORDER_DX - sizeButton.cx,
			rcClient.bottom - __EXT_SHORTCUTLISTWND_BTN2BORDER_DY - sizeButton.cy,
			rcClient.right - __EXT_SHORTCUTLISTWND_BTN2BORDER_DX,
			rcClient.bottom - __EXT_SHORTCUTLISTWND_BTN2BORDER_DY
			);
	} // else from if( bHorzLayout )

}

void CExtShortcutListWnd::OnShortcutListItemMeasure(
	CDC & dc,
	const CRect & rcClient,
	CExtShortcutListWnd::SHORTCUT_ITEM_INFO & _sii
	)
{
	ASSERT_VALID( this );
	ASSERT( _sii.m_bVisible );
bool bBigIconView = IsBigIconView();
CExtCmdIcon * pIcon =
		bBigIconView
			? ( & _sii.m_iconBig )
			: ( & _sii.m_iconSmall )
			;
	if( pIcon->IsEmpty() )
	{
		pIcon = ( & _sii.m_iconBig );
		if( pIcon->IsEmpty() )
			pIcon = ( & _sii.m_iconSmall );
	}
CSize sizeText( 0, 0 );
CSize sizeIcon = pIcon->GetSize();
	if( !_sii.m_sText.IsEmpty() )
	{
		CFont * pFont = OnShortcutListItemGetFont( false, _sii );
		ASSERT( pFont != NULL );
		ASSERT( pFont->GetSafeHandle() != NULL );
		CFont * pOldFont = dc.SelectObject( pFont );
		_sii.m_rcBoundsText.SetRect( 0, 0, 0, 0 );
		dc.DrawText(
			_sii.m_sText,
			_sii.m_sText.GetLength(),
			_sii.m_rcBoundsText,
			DT_CALCRECT|DT_SINGLELINE
			);
		dc.SelectObject( pOldFont );
		sizeText = _sii.m_rcBoundsText.Size();
		_sii.m_rcBoundsText.SetRect( 0, 0, 0, 0 );
	} // if( !_sii.m_sText.IsEmpty() )

bool bHorzLayout = IsHorizontalLayout();

CPoint ptClientCenter = rcClient.CenterPoint();
	_sii.m_rcBoundsIcon.SetRect( 0, 0, 0, 0 );
	_sii.m_rcBoundsText.SetRect( 0, 0, 0, 0 );

	if( bHorzLayout )
	{
		if( sizeIcon.cx > 0 && sizeIcon.cy > 0 )
		{
			_sii.m_rcBoundsIcon.SetRect(
				0,
				ptClientCenter.y-sizeIcon.cy/2,
				sizeIcon.cx,
				ptClientCenter.y-sizeIcon.cy/2+sizeIcon.cy
				);
			if( !bBigIconView )
				_sii.m_rcBoundsIcon.OffsetRect(
					0,
					- _sii.m_rcBoundsIcon.top
						- _sii.m_rcBoundsIcon.Height()
						+ rcClient.Height()
						- __EXT_SHORTCUTLISTWND_ITEM_DY_BEFORE
					);
		}
		if( sizeText.cx > 0 && sizeText.cy > 0 )
		{
			sizeText.cx += 10;
			sizeText.cy += 2;
			_sii.m_rcBoundsText.SetRect(
				0,
				ptClientCenter.y-sizeText.cx/2,
				sizeText.cy,
				ptClientCenter.y-sizeText.cx/2+sizeText.cx
				);

			if( !_sii.m_rcBoundsIcon.IsRectEmpty() )
			{
				if( bBigIconView )
					_sii.m_rcBoundsText.OffsetRect(
						_sii.m_rcBoundsIcon.Width() + __EXT_SHORTCUTLISTWND_ICON2TEXT_DY,
						0
						);
				else
					_sii.m_rcBoundsText.OffsetRect(
						0,
						_sii.m_rcBoundsIcon.top
							- _sii.m_rcBoundsText.bottom - __EXT_SHORTCUTLISTWND_ICON2TEXT_DY
						);
			} // if( !_sii.m_rcBoundsIcon.IsRectEmpty() )
		}
		if( bBigIconView )
		{
			_sii.m_rcBoundsAll.SetRect(
				min( _sii.m_rcBoundsIcon.left, _sii.m_rcBoundsText.left),
				min( _sii.m_rcBoundsIcon.top, _sii.m_rcBoundsText.top),
				max( _sii.m_rcBoundsIcon.right, _sii.m_rcBoundsText.right),
				max( _sii.m_rcBoundsIcon.bottom, _sii.m_rcBoundsText.bottom)
				);
		} // if( bBigIconView )
		else
		{
			_sii.m_rcBoundsAll.SetRect(
				min( _sii.m_rcBoundsIcon.left, _sii.m_rcBoundsText.left),
				min( _sii.m_rcBoundsIcon.top, _sii.m_rcBoundsText.top),
				max( _sii.m_rcBoundsIcon.right, _sii.m_rcBoundsText.right),
				max( _sii.m_rcBoundsIcon.bottom, _sii.m_rcBoundsText.bottom)
				);
			if( !_sii.m_rcBoundsIcon.IsRectEmpty() )
				_sii.m_rcBoundsIcon.OffsetRect(
					(_sii.m_rcBoundsAll.Width() - _sii.m_rcBoundsIcon.Width() )/2,
					0
					);
			if( !_sii.m_rcBoundsText.IsRectEmpty() )
				_sii.m_rcBoundsText.OffsetRect(
					(_sii.m_rcBoundsText.Width() - _sii.m_rcBoundsText.Width() )/2,
					0
					);
		} // else from if( bBigIconView )
		if( !_sii.m_rcBoundsAll.IsRectEmpty() )
		{
			_sii.m_rcBoundsAll.left -= __EXT_SHORTCUTLISTWND_ITEM_DY_BEFORE;
			_sii.m_rcBoundsAll.right += __EXT_SHORTCUTLISTWND_ITEM_DX_AFTER;
			LONG nOffset = -(_sii.m_rcBoundsAll.TopLeft().x);
			_sii.m_rcBoundsAll.OffsetRect( nOffset, 0 );
			_sii.m_rcBoundsIcon.OffsetRect( nOffset, 0 );
			_sii.m_rcBoundsText.OffsetRect( nOffset, 0 );
		}
	} // if( bHorzLayout )
	else
	{
		if( sizeIcon.cx > 0 && sizeIcon.cy > 0 )
		{
			_sii.m_rcBoundsIcon.SetRect(
				ptClientCenter.x-sizeIcon.cx/2,
				0,
				ptClientCenter.x-sizeIcon.cx/2+sizeIcon.cx,
				sizeIcon.cx
				);
			if( !bBigIconView )
				_sii.m_rcBoundsIcon.OffsetRect(
					- _sii.m_rcBoundsIcon.left + __EXT_SHORTCUTLISTWND_ITEM_DX_BEFORE,
					0
					);
		}
		if( sizeText.cx > 0 && sizeText.cy > 0 )
		{
			_sii.m_rcBoundsText.SetRect(
				ptClientCenter.x-sizeText.cx/2,
				0,
				ptClientCenter.x-sizeText.cx/2+sizeText.cx,
				sizeText.cy
				);
			if( !_sii.m_rcBoundsIcon.IsRectEmpty() )
			{
				if( bBigIconView )
					_sii.m_rcBoundsText.OffsetRect(
						0,
						_sii.m_rcBoundsIcon.Height() + __EXT_SHORTCUTLISTWND_ICON2TEXT_DY
						);
				else
					_sii.m_rcBoundsText.OffsetRect(
						_sii.m_rcBoundsIcon.right - _sii.m_rcBoundsText.left + __EXT_SHORTCUTLISTWND_ICON2TEXT_DX,
						0
						);
			} // if( !_sii.m_rcBoundsIcon.IsRectEmpty() )
		}
		if( bBigIconView )
		{
			_sii.m_rcBoundsAll.SetRect(
				min( _sii.m_rcBoundsIcon.left, _sii.m_rcBoundsText.left),
				min( _sii.m_rcBoundsIcon.top, _sii.m_rcBoundsText.top),
				max( _sii.m_rcBoundsIcon.right, _sii.m_rcBoundsText.right),
				max( _sii.m_rcBoundsIcon.bottom, _sii.m_rcBoundsText.bottom)
				);
		} // if( bBigIconView )
		else
		{
			_sii.m_rcBoundsAll.SetRect(
				min( _sii.m_rcBoundsIcon.left, _sii.m_rcBoundsText.left),
				min( _sii.m_rcBoundsIcon.top, _sii.m_rcBoundsText.top),
				max( _sii.m_rcBoundsIcon.right, _sii.m_rcBoundsText.right),
				max( _sii.m_rcBoundsIcon.bottom, _sii.m_rcBoundsText.bottom)
				);
			if( !_sii.m_rcBoundsIcon.IsRectEmpty() )
				_sii.m_rcBoundsIcon.OffsetRect(
					0,
					(_sii.m_rcBoundsAll.Height() - _sii.m_rcBoundsIcon.Height() )/2
					);
			if( !_sii.m_rcBoundsText.IsRectEmpty() )
				_sii.m_rcBoundsText.OffsetRect(
					0,
					(_sii.m_rcBoundsText.Height() - _sii.m_rcBoundsText.Height() )/2
					);
		} // else from if( bBigIconView )
		if( !_sii.m_rcBoundsAll.IsRectEmpty() )
		{
			_sii.m_rcBoundsAll.top -= __EXT_SHORTCUTLISTWND_ITEM_DY_BEFORE;
			_sii.m_rcBoundsAll.bottom += __EXT_SHORTCUTLISTWND_ITEM_DY_AFTER;
			LONG nOffset = -(_sii.m_rcBoundsAll.TopLeft().y);
			_sii.m_rcBoundsAll.OffsetRect( 0, nOffset );
			_sii.m_rcBoundsIcon.OffsetRect( 0, nOffset );
			_sii.m_rcBoundsText.OffsetRect( 0, nOffset );
		}
	} // else from if( bHorzLayout )

}

CFont * CExtShortcutListWnd::OnShortcutListItemGetFont(
	bool bHorzLayout,
	CExtShortcutListWnd::SHORTCUT_ITEM_INFO & _sii
	)
{
	ASSERT_VALID( this );
	_sii;
	if( bHorzLayout )
	{
		if( GetShortcutListStyle() & __ESLW_INV_VFONT )
			return &g_PaintManager->m_FontNormalVert;
		return &g_PaintManager->m_FontNormalVertX;
	}
	return &g_PaintManager->m_FontNormal;
}

LONG CExtShortcutListWnd::ItemGetCount() const
{
	ASSERT_VALID( this );
	return m_vItems.GetSize();
}

const CExtShortcutListWnd::SHORTCUT_ITEM_INFO * CExtShortcutListWnd::ItemGetInfo(
	LONG nIndex
	) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtShortcutListWnd * > ( this ) )
			-> ItemGetInfo( nIndex );
}

CExtShortcutListWnd::SHORTCUT_ITEM_INFO * CExtShortcutListWnd::ItemGetInfo(
	LONG nIndex
	)
{
	ASSERT_VALID( this );
	if( nIndex < 0 || nIndex >= m_vItems.GetSize() )
	{
		ASSERT( FALSE );
		return NULL;
	}
SHORTCUT_ITEM_INFO * pSII = m_vItems[ nIndex ];
	ASSERT( pSII != NULL );
	return pSII;
}

CExtShortcutListWnd::SHORTCUT_ITEM_INFO & CExtShortcutListWnd::ItemInsert(
	LONG nIndexInsertBefore, // = -1L, // append
	__EXT_MFC_SAFE_LPCTSTR sText, // = NULL
	HICON hIconBig, // = NULL
	bool bCopyIconBig, // = true
	HICON hIconSmall, // = NULL
	bool bCopyIconSmall, // = true
	bool bUpdateNow // = false
	)
{
	ASSERT_VALID( this );
	
	_CancelActions();

LONG nItemCount = ItemGetCount();
	if( nIndexInsertBefore < 0 || nIndexInsertBefore >= nItemCount )
		nIndexInsertBefore = nItemCount;
SHORTCUT_ITEM_INFO * pSII = new SHORTCUT_ITEM_INFO;
	
	pSII->m_sText = (sText == NULL) ?  _T("") : sText;
	if( hIconBig != NULL )
		pSII->m_iconBig.AssignFromHICON( hIconBig, bCopyIconBig );
	if( hIconSmall != NULL )
		pSII->m_iconSmall.AssignFromHICON( hIconSmall, bCopyIconSmall );

	m_vItems.InsertAt( nIndexInsertBefore, pSII );
	OnShortcutListItemInserted( nIndexInsertBefore, *pSII );
	m_nIndexVisibleFirst = m_nIndexVisibleLast = 0L;
	UpdateShortcutListWnd( bUpdateNow );
	return *pSII;
}

void CExtShortcutListWnd::OnShortcutListItemInserted(
	LONG nIndex,
	CExtShortcutListWnd::SHORTCUT_ITEM_INFO & _sii
	)
{
	ASSERT_VALID( this );
	nIndex;
	_sii;
}

LONG CExtShortcutListWnd::ItemRemove(
	LONG nIndex,
	LONG nCount, // = 1L
	bool bUpdateNow // = false
	)
{
	ASSERT_VALID( this );
LONG nRemovedCount = 0;
LONG nItemCount = ItemGetCount();
LONG nLastIndex = nIndex + nCount - 1;
	if(		nItemCount > 0
		&&	nIndex >= 0
		&&	nIndex <= (nItemCount-1)
		)
	{

		_CancelActions();

		if( nLastIndex > nCount )
			nLastIndex = nCount - 1;
		ASSERT( nLastIndex >= nIndex );
		for( ; true ; )
		{
			SHORTCUT_ITEM_INFO * pSII =
				ItemGetInfo( nLastIndex );
			ASSERT( pSII != NULL );
			if( OnShortcutListItemRemoving( nLastIndex, *pSII ) )
			{
				nRemovedCount++;
				m_vItems.RemoveAt( nLastIndex );
				OnShortcutListItemRemoved( nLastIndex, *pSII );
				delete pSII;
			}
			if( nLastIndex == nIndex )
				break;
			nLastIndex--;
		}
		ASSERT( nRemovedCount <= nCount );
	}

	if( nRemovedCount > 0 )
		m_nIndexVisibleFirst = m_nIndexVisibleLast = 0L;

	UpdateShortcutListWnd( bUpdateNow );
	return nRemovedCount;
}

bool CExtShortcutListWnd::OnShortcutListItemRemoving(
	LONG nIndex,
	CExtShortcutListWnd::SHORTCUT_ITEM_INFO & _sii
	)
{
	ASSERT_VALID( this );
	nIndex;
	_sii;
	return true;
}

void CExtShortcutListWnd::OnShortcutListItemRemoved(
	LONG nIndex,
	CExtShortcutListWnd::SHORTCUT_ITEM_INFO & _sii
	)
{
	ASSERT_VALID( this );
	nIndex;
	_sii;
}

LONG CExtShortcutListWnd::ItemRemoveAll(
	bool bUpdateNow // = false
	)
{
	ASSERT_VALID( this );
LONG nItemCount = ItemGetCount();
	if( nItemCount == 0 )
		return 0;
	return ItemRemove( 0, nItemCount, bUpdateNow );
}

LONG CExtShortcutListWnd::ItemGetIndexOf(
	const CExtShortcutListWnd::SHORTCUT_ITEM_INFO * pSII
	) const
{
	ASSERT_VALID( this );
//	ASSERT( pSII != NULL );
	if( pSII == NULL )
		return -1L;
LONG nItemCount = ItemGetCount();
	for( LONG nIndex = 0; nIndex < nItemCount; nIndex++ )
	{
		const SHORTCUT_ITEM_INFO * pSII2 = ItemGetInfo( nIndex );
		ASSERT( pSII2 != NULL );
		if( pSII == pSII2 )
			return nIndex;
	}
	return -1L;
}

LONG CExtShortcutListWnd::ItemHitTest(
	const POINT & ptClient,
	bool bTestIconArea, // = true
	bool bTestTextArea // = true
	) const
{
	ASSERT_VALID( this );
CRect rcClient;
	GetClientRect( & rcClient );
	if( !rcClient.PtInRect(ptClient) )
		return __ESLW_HT_NOWHERE;

	if( m_nScrollSize > 0 )
	{
		if( m_nScrollPos > 0
			&& ( ! m_rcButtonUp.IsRectEmpty() )
			&& m_rcButtonUp.PtInRect(ptClient)
			)
			return __ESLW_HT_BUTTON_UP;
		if( m_nScrollPos < m_nScrollSize
			&& ( ! m_rcButtonDown.IsRectEmpty() )
			&& m_rcButtonDown.PtInRect(ptClient)
			)
			return __ESLW_HT_BUTTON_DOWN;
	} // if( m_nScrollSize > 0 )

	if( bTestIconArea || bTestTextArea )
	{
		LONG nItemCount = ItemGetCount();
		if( nItemCount > 0 )
		{
			LONG nIndexVisibleFirst, nIndexVisibleLast;
			GetVisibleRange( nIndexVisibleFirst, nIndexVisibleLast );
			ASSERT( 0 <= nIndexVisibleFirst && nIndexVisibleFirst < nItemCount );
			ASSERT( 0 <= nIndexVisibleLast && nIndexVisibleLast < nItemCount );
			for( LONG nIndex = nIndexVisibleFirst; nIndex <= nIndexVisibleLast; nIndex++ )
			{
				const SHORTCUT_ITEM_INFO * pSII = ItemGetInfo( nIndex );
				ASSERT( pSII != NULL );
				if( ! pSII->m_bVisible )
					continue;
				CRect rcBoundsIcon, rcBoundsText, rcBoundsAll;
				ItemGetRects(
					nIndex,
					&rcBoundsIcon,
					&rcBoundsText,
					&rcBoundsAll
					);
				if( rcBoundsAll.IsRectEmpty()
					|| ( ! rcBoundsAll.PtInRect(ptClient) )
					)
					continue;
				if( bTestIconArea
					&& ( ! rcBoundsText.IsRectEmpty() )
					&& rcBoundsText.PtInRect(ptClient)
					)
					return nIndex;
				if( bTestTextArea
					&& ( ! rcBoundsIcon.IsRectEmpty() )
					&& rcBoundsIcon.PtInRect(ptClient)
					)
					return nIndex;
				if( bTestIconArea
					&& bTestTextArea
					&& rcBoundsAll.PtInRect(ptClient)
					)
					return nIndex;
			} // for( LONG nIndex = nIndexVisibleFirst; nIndex <= nIndexVisibleLast; nIndex++ )
		} // if( nItemCount > 0 )

	} // if( bTestIconArea || bTestTextArea )

	return __ESLW_HT_NOWHERE;
}

void CExtShortcutListWnd::ItemGetRects(
	LONG nIndex,
	RECT * pRectBoundsIcon, // = NULL
	RECT * pRectBoundsText, // = NULL
	RECT * pRectBoundsAll // = NULL
	) const
{
	ASSERT_VALID( this );

	if(		pRectBoundsIcon == NULL
		&&	pRectBoundsText == NULL
		&&	pRectBoundsAll == NULL
		)
		return;
	if( pRectBoundsIcon != NULL )
		::memset( pRectBoundsIcon, 0, sizeof(RECT) );
	if( pRectBoundsText != NULL )
		::memset( pRectBoundsText, 0, sizeof(RECT) );
	if( pRectBoundsAll != NULL )
		::memset( pRectBoundsAll, 0, sizeof(RECT) );

	if( nIndex < 0 )
		return;
LONG nItemCount = ItemGetCount();
	if( nIndex >= nItemCount )
		return;
const SHORTCUT_ITEM_INFO * pSII = ItemGetInfo( nIndex );
	ASSERT( pSII != NULL );
	if( ! pSII->m_bVisible )
		return;
bool bHorzLayout = IsHorizontalLayout();
	if( pRectBoundsIcon != NULL )
	{
		::CopyRect(
			pRectBoundsIcon,
			&pSII->m_rcBoundsIcon
			);
		::OffsetRect(
			pRectBoundsIcon,
			bHorzLayout ? (-m_nScrollPos) : 0,
			bHorzLayout ? 0 : (-m_nScrollPos)
			);
	}
	if( pRectBoundsText != NULL )
	{
		::CopyRect(
			pRectBoundsText,
			&pSII->m_rcBoundsText
			);
		::OffsetRect(
			pRectBoundsText,
			bHorzLayout ? (-m_nScrollPos) : 0,
			bHorzLayout ? 0 : (-m_nScrollPos)
			);
	}
	if( pRectBoundsAll != NULL )
	{
		::CopyRect(
			pRectBoundsAll,
			&pSII->m_rcBoundsAll
			);
		::OffsetRect(
			pRectBoundsAll,
			bHorzLayout ? (-m_nScrollPos) : 0,
			bHorzLayout ? 0 : (-m_nScrollPos)
			);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CExtShortcutListWnd message handlers

void CExtShortcutListWnd::PreSubclassWindow() 
{
	CWnd::PreSubclassWindow();

	if( m_bDirectCreateCall )
		return;

DWORD dwStyle = ::GetWindowLong( m_hWnd, GWL_STYLE );
	m_dwShortcutListStyle = dwStyle & __EXTMFC_ALL_FORM_MOVABLE_WND_STYLES;
	::SetWindowLong( m_hWnd, GWL_STYLE, dwStyle & (~__EXTMFC_ALL_FORM_MOVABLE_WND_STYLES) );
	
	if( !_CreateHelper() )
	{
		ASSERT( FALSE );
		AfxThrowMemoryException();
	} // if( !Create() )
}

void CExtShortcutListWnd::_CancelActions()
{
	ASSERT_VALID( this );
	if(		m_bCancelingActionsMode
		||	GetSafeHwnd() == NULL
		||	( ! ::IsWindow(GetSafeHwnd()) )
		)
		return;
	if(		m_bScrolling
		||	m_hWndEditor != NULL
		||	m_nTrackingHT != __ESLW_HT_NOWHERE
		)
		SendMessage( WM_CANCELMODE );
}

bool CExtShortcutListWnd::_CreateHelper()
{
	EnableToolTips( TRUE );

	if( !m_wndToolTip.Create(this) )
	{
		ASSERT( FALSE );
		return false;
	}
	m_wndToolTip.Activate( TRUE );

	UpdateShortcutListWnd( false );
	return true;
}

BOOL CExtShortcutListWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	if( ! CWnd::PreCreateWindow(cs) )
		return FALSE;
	return true;
}

void CExtShortcutListWnd::PostNcDestroy() 
{
	if( m_hWndEditor != NULL )
	{
		OnShortcutListCancelItemEditor( m_hWndEditor );
		m_hWndEditor = NULL;
	}
	m_wndContentExpand.Deactivate();
	ItemRemoveAll( false );
	CWnd::PostNcDestroy();
}

CScrollBar* CExtShortcutListWnd::GetScrollBarCtrl(int nBar) const
{
//	CWnd::GetScrollBarCtrl(nBar);
	nBar;
	return NULL;
}

LRESULT CExtShortcutListWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	
LRESULT lResult =
		CWnd::WindowProc(
			message,
			wParam,
			lParam
			);

	return lResult;
}

int CExtShortcutListWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
//	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
	pDesktopWnd;
	nHitTest;
	message;
	return MA_NOACTIVATE;
}

void CExtShortcutListWnd::OnSetFocus(CWnd* pOldWnd) 
{
//	CWnd::OnSetFocus(pOldWnd);
	if( pOldWnd != NULL )
	{
		if(		pOldWnd->GetSafeHwnd() != NULL
			&&	::IsWindow( pOldWnd->GetSafeHwnd() )
			)
			pOldWnd->SetFocus();
	}
	else
	{
		if( GetStyle() & WS_CHILD )
			GetParent()->SetFocus();
		else
		{
			CWnd * pWnd = ::AfxGetMainWnd();
			if( pWnd != NULL )
				pWnd->SetFocus();
		}
	}
}

LRESULT CExtShortcutListWnd::_OnPaintExpanedItemContent(WPARAM wParam, LPARAM lParam)
{
	lParam;
CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA * p_picd =
		(CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA *)wParam;
	ASSERT( p_picd != NULL );
	ASSERT( p_picd->m_dc.GetSafeHdc() != NULL );
	if( m_nTrackingHT < 0 )
		return 0;
	ASSERT( m_nTrackingHT < ItemGetCount() );
SHORTCUT_ITEM_INFO * pSII = ItemGetInfo( m_nTrackingHT );
	ASSERT( pSII != NULL );

CRect	rcBoundsIcon( pSII->m_rcBoundsIcon ),
		rcBoundsText( pSII->m_rcBoundsText ),
		rcBoundsAll( pSII->m_rcBoundsAll ); // p_picd->m_rcItem
CPoint ptOffsetSrc2 = rcBoundsAll.TopLeft();
CPoint ptOffsetSrc1 = p_picd->m_rcItem.TopLeft();
bool bHorzLayout = IsHorizontalLayout();
CPoint ptOffset(
		ptOffsetSrc1.x - ptOffsetSrc2.x
			+ __EXT_SHORTCUTLISTWND_ITEM_GAP_X
			+ ( bHorzLayout ? __EXT_SHORTCUTLISTWND_EXPANDED_TOTAL_GAP_H_X : __EXT_SHORTCUTLISTWND_EXPANDED_TOTAL_GAP_V_X )
			,
		ptOffsetSrc1.y - ptOffsetSrc2.y
			+ __EXT_SHORTCUTLISTWND_ITEM_GAP_Y
			+ ( bHorzLayout ? __EXT_SHORTCUTLISTWND_EXPANDED_TOTAL_GAP_H_Y : __EXT_SHORTCUTLISTWND_EXPANDED_TOTAL_GAP_V_Y )
		);
	rcBoundsIcon.OffsetRect( ptOffset );
	rcBoundsText.OffsetRect( ptOffset );
	rcBoundsAll.OffsetRect( ptOffset );

//	OnShortcutListEraseEntire( p_picd->m_dc, p_picd->m_rcItem );

//	OnShortcutListEraseEntire( p_picd->m_dc, rcBoundsIcon );
//	OnShortcutListEraseEntire( p_picd->m_dc, rcBoundsText );
	
	OnShortcutListEraseItem(
		p_picd->m_dc,
		*pSII,
		p_picd->m_rcItem,
		rcBoundsIcon,
		rcBoundsText,
		rcBoundsAll,
		true
		);
	OnShortcutListDrawItem(
		p_picd->m_dc,
		*pSII,
		p_picd->m_rcItem,
		rcBoundsIcon,
		rcBoundsText,
		rcBoundsAll,
		true
		);
	return (!0);
}

void CExtShortcutListWnd::OnPaint() 
{
	// recalc delayed layout
	_RecalcLayoutImpl();

CRect rcClient;
	GetClientRect( &rcClient );
CPaintDC dcPaint( this );
	CExtPaintManager::stat_ExcludeChildAreas(
		dcPaint.GetSafeHdc(),
		GetSafeHwnd()
		);
CExtMemoryDC dc( &dcPaint, &rcClient );
	CExtPaintManager::stat_ExcludeChildAreas(
		dc.GetSafeHdc(),
		GetSafeHwnd()
		);

	OnShortcutListEraseEntire( dc, rcClient );
	OnShortcutListDrawEntire( dc, rcClient );
}

BOOL CExtShortcutListWnd::OnEraseBkgnd(CDC* pDC) 
{
//	return CWnd::OnEraseBkgnd(pDC);
	pDC;
	return TRUE;
}

void CExtShortcutListWnd::OnShortcutListEraseEntire(
	CDC & dc,
	const RECT & rcClient
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	g_PaintManager->PaintShortcutListBkgnd(
		dc,
		rcClient,
		this
		);
}

void CExtShortcutListWnd::OnShortcutListDrawEntire(
	CDC & dc,
	const RECT & rcClient
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	if(		rcClient.left >= rcClient.right
		||	rcClient.top >= rcClient.bottom
		||	(! dc.RectVisible( &rcClient ) )
		)
		return;

LONG nItemCount = ItemGetCount();
	if( nItemCount > 0 )
	{
		LONG nIndexVisibleFirst, nIndexVisibleLast;
		GetVisibleRange( nIndexVisibleFirst, nIndexVisibleLast );
		ASSERT( 0 <= nIndexVisibleFirst && nIndexVisibleFirst < nItemCount );
		ASSERT( 0 <= nIndexVisibleLast && nIndexVisibleLast < nItemCount );
		for( LONG nIndex = nIndexVisibleFirst; nIndex <= nIndexVisibleLast; nIndex++ )
		{
			SHORTCUT_ITEM_INFO * pSII = ItemGetInfo( nIndex );
			ASSERT( pSII != NULL );
			if( ! pSII->m_bVisible )
				continue;
			CRect rcBoundsIcon, rcBoundsText, rcBoundsAll;
			ItemGetRects(
				nIndex,
				&rcBoundsIcon,
				&rcBoundsText,
				&rcBoundsAll
				);
			OnShortcutListEraseItem(
				dc,
				*pSII,
				rcClient,
				rcBoundsIcon,
				rcBoundsText,
				rcBoundsAll,
				false
				);
			OnShortcutListDrawItem(
				dc,
				*pSII,
				rcClient,
				rcBoundsIcon,
				rcBoundsText,
				rcBoundsAll,
				false
				);
		} // for( LONG nIndex = nIndexVisibleFirst; nIndex <= nIndexVisibleLast; nIndex++ )
	} // if( nItemCount > 0 )
	
	if( m_nScrollSize > 0 )
	{
		if( m_nScrollPos > 0
			&& ( ! m_rcButtonUp.IsRectEmpty() )
			&& dc.RectVisible( &m_rcButtonUp )
			)
		{
			OnShortcutListDrawButton(
				dc,
				m_rcButtonUp,
				true,
				(m_nTrackingHT == __ESLW_HT_BUTTON_UP) ? true : false,
				(m_nTrackingHT == __ESLW_HT_BUTTON_UP && m_bPressedTracking) ? true : false
				);
		} // if( m_nScrollPos > 0 ...
		if( m_nScrollPos < m_nScrollSize
			&& ( ! m_rcButtonDown.IsRectEmpty() )
			&& dc.RectVisible( &m_rcButtonDown )
			)
		{
			OnShortcutListDrawButton(
				dc,
				m_rcButtonDown,
				false,
				(m_nTrackingHT == __ESLW_HT_BUTTON_DOWN) ? true : false,
				(m_nTrackingHT == __ESLW_HT_BUTTON_DOWN && m_bPressedTracking) ? true : false
				);
		} // if( m_nScrollPos < m_nScrollSize ...
	} // if( m_nScrollSize > 0 )
}

void CExtShortcutListWnd::OnShortcutListEraseItem(
	CDC & dc,
	CExtShortcutListWnd::SHORTCUT_ITEM_INFO & _sii,
	CRect rcClient,
	CRect rcBoundsIcon,
	CRect rcBoundsText,
	CRect rcBoundsAll,
	bool bExpandMode
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

bool bHorzLayout = IsHorizontalLayout();
CFont * pItemFont = OnShortcutListItemGetFont( bHorzLayout, _sii );
	ASSERT( pItemFont != NULL );
	ASSERT( pItemFont->GetSafeHandle() != NULL );
CExtPaintManager::PAINTSHORTCUTLISTITEMSDATA _pslid(
	this,
	m_dwShortcutListStyle,
	(HFONT)pItemFont->GetSafeHandle(),
	&_sii.m_iconBig,
	&_sii.m_iconSmall,
	_sii.m_lParam,
	rcBoundsAll,
	rcBoundsIcon,
	rcBoundsText,
	rcClient,
	_sii.m_sText,
	_sii.m_bHover,
	_sii.m_bPressed,
	_sii.m_bVisible,
	_sii.m_bChecked,
	bExpandMode,
	bHorzLayout,
	IsBigIconView(),
	true
	);
	g_PaintManager->PaintShortcutListItem( dc, _pslid );


/*
	dc;
	_sii;
	rcClient;
	rcBoundsIcon;
	rcBoundsAll;

	if( bExpandMode )
	{
		if(		rcBoundsText.left < rcBoundsText.right
			&&	rcBoundsText.top < rcBoundsText.bottom
			&&	dc.RectVisible( &rcBoundsText )
			)
		{
			CRect rcExpandErase( rcBoundsText );
			rcExpandErase.InflateRect(
				__EXT_SHORTCUTLISTWND_EXPANDED_TEXT_GAP_X,
				__EXT_SHORTCUTLISTWND_EXPANDED_TEXT_GAP_Y
				);
			COLORREF clrDarkBorder = g_PaintManager->GetColor( COLOR_3DDKSHADOW );
			COLORREF clrInfoBk = g_PaintManager->GetColor( COLOR_INFOBK );
			dc.FillSolidRect( &rcExpandErase, clrInfoBk );
			dc.Draw3dRect( &rcExpandErase, clrDarkBorder, clrDarkBorder );
		}
	} // if( bExpandMode )
*/
}

void CExtShortcutListWnd::OnShortcutListDrawItem(
	CDC & dc,
	CExtShortcutListWnd::SHORTCUT_ITEM_INFO & _sii,
	CRect rcClient,
	CRect rcBoundsIcon,
	CRect rcBoundsText,
	CRect rcBoundsAll,
	bool bExpandMode
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	ASSERT( _sii.m_bVisible );

bool bHorzLayout = IsHorizontalLayout();
CFont * pItemFont = OnShortcutListItemGetFont( bHorzLayout, _sii );
	ASSERT( pItemFont != NULL );
	ASSERT( pItemFont->GetSafeHandle() != NULL );
CExtPaintManager::PAINTSHORTCUTLISTITEMSDATA _pslid(
	this,
	m_dwShortcutListStyle,
	(HFONT)pItemFont->GetSafeHandle(),
	&_sii.m_iconBig,
	&_sii.m_iconSmall,
	_sii.m_lParam,
	rcBoundsAll,
	rcBoundsIcon,
	rcBoundsText,
	rcClient,
	_sii.m_sText,
	_sii.m_bHover,
	_sii.m_bPressed,
	_sii.m_bVisible,
	_sii.m_bChecked,
	bExpandMode,
	bHorzLayout,
	IsBigIconView(),
	false
	);
	g_PaintManager->PaintShortcutListItem( dc, _pslid );
	
/*	
	if( rcBoundsAll.IsRectEmpty()
		||	( ! dc.RectVisible( &rcBoundsAll ) )
		)
		return;

	if( (! rcBoundsIcon.IsRectEmpty() )
		&& dc.RectVisible( &rcBoundsIcon )
		)
	{
		CExtCmdIcon & _icon =
			IsBigIconView() ? _sii.m_iconBig : _sii.m_iconSmall;
		HICON hIcon = _icon.GetIcon();
		if( hIcon == NULL )
		{
			hIcon = _sii.m_iconBig.GetIcon();
			if( hIcon == NULL )
				hIcon = _sii.m_iconSmall.GetIcon();
		}
		CRect rcUnderIcon( rcBoundsIcon );
		rcUnderIcon.InflateRect(
			__EXT_SHORTCUTLISTWND_ICON_BORDER_GAP_X,
			__EXT_SHORTCUTLISTWND_ICON_BORDER_GAP_Y
			);
		dc.FillSolidRect(
			&rcUnderIcon,
			g_PaintManager->GetColor( COLOR_3DSHADOW )
			);
		if( hIcon != NULL )
		{
			::DrawIconEx(
				dc.GetSafeHdc(),
				rcBoundsIcon.left,
				rcBoundsIcon.top,
				hIcon,
				rcBoundsIcon.Width(),
				rcBoundsIcon.Height(),
				0,
				(HBRUSH)NULL,
				DI_NORMAL
				);
		}
		if( _sii.m_bPressed || _sii.m_bHover )
		{
			COLORREF clrLT =
				g_PaintManager->GetColor(
					_sii.m_bPressed ? COLOR_3DDKSHADOW : COLOR_3DFACE
					);
			COLORREF clrRB =
				g_PaintManager->GetColor(
					_sii.m_bPressed ? COLOR_3DFACE : COLOR_3DDKSHADOW
					);
			dc.Draw3dRect(
				&rcUnderIcon,
				clrLT,
				clrRB
				);
		}
	}

CExtSafeString sItemText = _sii.m_sText;
	if( (! rcBoundsText.IsRectEmpty() )
		&& dc.RectVisible( &rcBoundsText )
		&& ( ! sItemText.IsEmpty() )
		)
	{
		LONG nItemTextLen = sItemText.GetLength();
		CRect rcCaption( rcBoundsText );
bool bHorzLayout = IsHorizontalLayout();
		CFont * pItemFont = OnShortcutListItemGetFont( bHorzLayout, _sii );
		ASSERT( pItemFont != NULL );
		ASSERT( pItemFont->GetSafeHandle() != NULL );
		CFont * pOldFont = dc.SelectObject( pItemFont );
		COLORREF clrTextOld =
			dc.SetTextColor(
				g_PaintManager->GetColor(
					bExpandMode ? COLOR_INFOTEXT : COLOR_3DHILIGHT
					//bEnabled ? COLOR_BTNTEXT : COLOR_3DHILIGHT
					)
				);
		int nOldBkMode = dc.SetBkMode( TRANSPARENT );
		if( bHorzLayout )
		{
			rcCaption.InflateRect( 3, 0 );
			CSize sizeTextMeasured = rcBoundsText.Size();
			UINT nOldTA = dc.SetTextAlign(
				TA_TOP | TA_BASELINE
				);
			rcCaption.OffsetRect(
				( (m_dwShortcutListStyle & __ESLW_INV_VFONT) == 0 )
					?   sizeTextMeasured.cx/2
					: - sizeTextMeasured.cx/2 + 2
					,
				0
				);
			CPoint ptCenter = rcCaption.CenterPoint();
			if( (m_dwShortcutListStyle & __ESLW_INV_VFONT) == 0 )
				ptCenter.y =
					rcCaption.bottom - 4
					- (rcCaption.Height() - sizeTextMeasured.cy)
					;
			else
				ptCenter.y =
					rcCaption.top + 4
					;
			dc.ExtTextOut(
				ptCenter.x,
				ptCenter.y,
				ETO_CLIPPED,
				&rcCaption,
				sItemText,
				NULL
				);
			dc.SetTextAlign( nOldTA );
		} // if( bHorzLayout )
		else
		{
			rcClient.DeflateRect( 1, 0 );
			CRect rcText(
				max(rcCaption.left, rcClient.left),
				rcCaption.top,
				min(rcCaption.right, rcClient.right),
				rcCaption.bottom
				);
			if( rcText.left < rcText.right )
			{
				UINT nFormat =
					DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
				dc.DrawText(
					sItemText,
					nItemTextLen,
					&rcText,
					nFormat
					);
			} // if( rcText.left < rcText.right )
		} // else from if( bHorzLayout )
		dc.SetBkMode( nOldBkMode );
		dc.SetTextColor( clrTextOld );
		dc.SelectObject( pOldFont );
	}
*/
}

void CExtShortcutListWnd::OnShortcutListDrawButton(
	CDC & dc,
	CRect rcButton,
	bool bUpBtn,
	bool bHover,
	bool bPressed
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	g_PaintManager->PaintShortcutListScrollButton(
		dc,
		IsHorizontalLayout(),
		rcButton,
		bUpBtn,
		bHover,
		bPressed,
		this
		);
}

void CExtShortcutListWnd::OnCancelMode() 
{
	CWnd::OnCancelMode();

	if( m_bCancelingActionsMode )
		return;

	m_bCancelingActionsMode = true;
	
	if( m_hWndEditor != NULL )
	{
		OnShortcutListCancelItemEditor( m_hWndEditor );
		m_hWndEditor = NULL;
	}

	if( m_nTrackingHT != __ESLW_HT_NOWHERE )
	{
		KillTimer( __EXT_SHORTCUTLISTWND_HOVER_TIMER_ID );
		KillTimer( __EXT_SHORTCUTLISTWND_PRESSED_TIMER_ID );

		if( m_nTrackingHT == __ESLW_HT_BUTTON_UP || m_nTrackingHT == __ESLW_HT_BUTTON_DOWN )
		{
			InvalidateRect(
				(m_nTrackingHT == __ESLW_HT_BUTTON_UP)
					? ( & m_rcButtonUp )
					: ( & m_rcButtonDown )
				);
		} // if( nHT == __ESLW_HT_BUTTON_UP || nHT == __ESLW_HT_BUTTON_DOWN )
		else if( m_nTrackingHT >= 0 )
		{
			LONG nItemCount = ItemGetCount();
			ASSERT( m_nTrackingHT < nItemCount );
			CRect rcBoundsItem;
			ItemGetRects( m_nTrackingHT, NULL, NULL, &rcBoundsItem );
			rcBoundsItem.InflateRect(
				__EXT_SHORTCUTLISTWND_ITEM_GAP_X,
				__EXT_SHORTCUTLISTWND_ITEM_GAP_Y
				);
			InvalidateRect( &rcBoundsItem );
			if( m_bPressedTracking )
				OnShortcutListItemPressedStop( m_nTrackingHT, false );
			else
				OnShortcutListItemHoverStop( m_nTrackingHT );
		} // else if( m_nTrackingHT >= 0 )
		
		m_nTrackingHT = __ESLW_HT_NOWHERE;
	}
	m_bPressedTracking = false;

	if(		(m_dwShortcutListStyle & __ESLW_ITEM_TOOLTIP) != 0
		&&	m_wndToolTip.GetSafeHwnd() != NULL
		&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
		)
		m_wndToolTip.DelTool( this, 1 );
	CWnd::CancelToolTips();

	m_wndContentExpand.Deactivate();
	m_ptStartLeftBtnTrack.x = m_ptStartLeftBtnTrack.y = -1;

	if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
		CExtMouseCaptureSink::ReleaseCapture();

	m_bCancelingActionsMode = false;
	
	if( m_bDelayRecalcLayout )
		UpdateShortcutListWnd( true );
	else
		UpdateWindow();
	
}

bool CExtShortcutListWnd::ItemBeginEdit( LONG nIndex )
{
	ASSERT_VALID( this );

	_CancelActions();
	if( nIndex < 0 )
		return false;
LONG nItemCount = ItemGetCount();
	if( nItemCount == 0 )
		return false;
	if( nIndex > nItemCount )
		return false;
	if( m_bDelayRecalcLayout )
		UpdateShortcutListWnd( true );
	if( !ItemEnsureVisible(nIndex, true) )
		return false;
	m_hWndEditor = OnShortcutListStartItemEditor( nIndex );
	return (m_hWndEditor != NULL) ? true : false;
}

HWND CExtShortcutListWnd::OnShortcutListStartItemEditor(
	LONG nIndex
	)
{
	ASSERT_VALID( this );
	ASSERT( nIndex >= 0 && nIndex < ItemGetCount() );

bool bHorzLayout = IsHorizontalLayout();
	if( bHorzLayout )
		return NULL;

SHORTCUT_ITEM_INFO * pSII = ItemGetInfo( nIndex );
	ASSERT( pSII != NULL );
	ASSERT( pSII->m_bVisible );
CRect rcItem;
	ItemGetRects( nIndex, NULL, &rcItem );
CRect rcClient;
	GetClientRect( &rcClient );

	if( rcItem.top < rcClient.top )
		rcItem.OffsetRect(
			0,
			rcClient.top - rcItem.top
			);
	if( rcItem.bottom > rcClient.bottom )
		rcItem.OffsetRect(
			0,
			-(rcItem.bottom - rcClient.bottom)
			);
	if( rcItem.top < rcClient.top )
		rcItem.top = rcClient.top;

DWORD dwAddEditWndStyles = 0L;
	rcItem.right = rcClient.right - __EXT_SHORTCUTLISTWND_EXPANDED_TEXT_GAP_X;
	if( IsBigIconView() )
	{
		rcItem.left = rcClient.left + __EXT_SHORTCUTLISTWND_EXPANDED_TEXT_GAP_X;
		dwAddEditWndStyles |= ES_CENTER;
	}
	else
		rcItem.left -= __EXT_SHORTCUTLISTWND_EXPANDED_TEXT_GAP_X;

	rcItem.InflateRect(
		0,
		__EXT_SHORTCUTLISTWND_EXPANDED_TEXT_GAP_Y
		);

	if( rcItem.left < rcClient.left )
		rcItem.left = rcClient.left;
	if( rcItem.right > rcClient.right )
		rcItem.right = rcClient.right;

	if( (rcItem.right - rcItem.left) < __EXT_SHORTCUTLISTWND_MIN_EDITOR_DX )
		return false;
	if( (rcItem.bottom - rcItem.top) < __EXT_SHORTCUTLISTWND_MIN_EDITOR_DY )
		return false;

CItemTextEditWnd * pWndEdit =
		new CItemTextEditWnd( pSII );
	if( !pWndEdit->Create(
			WS_CHILD|ES_AUTOHSCROLL|dwAddEditWndStyles,
			rcItem,
			this,
			(UINT)IDC_STATIC
			)
		)
		return NULL;

HWND hWndEditor = pWndEdit->GetSafeHwnd();

CFont * pItemFont = OnShortcutListItemGetFont( false, *pSII );
	if( pItemFont != NULL )
		pWndEdit->SetFont( pItemFont );
	else
		::SendMessage(
			hWndEditor,
			WM_SETFONT,
			(WPARAM)::GetStockObject( DEFAULT_GUI_FONT ),
			(LPARAM)TRUE
			);
__EXT_MFC_SAFE_LPCTSTR sItemText = pSII->m_sText;
	if( sItemText == NULL )
		sItemText = _T("");
	pWndEdit->SetWindowText( sItemText );
	pWndEdit->ShowWindow( SW_SHOW );
	pWndEdit->SetFocus();

	return hWndEditor;
}

void CExtShortcutListWnd::OnShortcutListCancelItemEditor(
	HWND hWndEditor
	)
{
	ASSERT_VALID( this );
	ASSERT( hWndEditor != NULL );
	if( !::IsWindow(hWndEditor) )
		return;
	::DestroyWindow( hWndEditor );
}

bool CExtShortcutListWnd::OnShortcutListDeactivateEditor(
	CExtShortcutListWnd::SHORTCUT_ITEM_INFO * pSII,
	bool bUpdateDeactivation
	)
{
	ASSERT_VALID( this );
	ASSERT( pSII != NULL );
	pSII;
	bUpdateDeactivation;
	if( m_hWndEditor != NULL )
		::PostMessage(
			m_hWndEditor,
			WM_CLOSE,
			0,
			0
			);
	m_hWndEditor = NULL;
	return true;
}

void CExtShortcutListWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	if( (GetStyle() & WS_VISIBLE) == 0 )
		return;
	UpdateShortcutListWnd( true );
}

void CExtShortcutListWnd::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
	
	if( (GetStyle() & WS_VISIBLE) == 0 )
		return;
	UpdateShortcutListWnd( true );
}

void CExtShortcutListWnd::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CWnd::OnWindowPosChanged(lpwndpos);
	
	if( (GetStyle() & WS_VISIBLE) == 0 )
		return;
	UpdateShortcutListWnd( true );
}

#if _MFC_VER < 0x700
void CExtShortcutListWnd::OnActivateApp(BOOL bActive, HTASK hTask) 
#else
void CExtShortcutListWnd::OnActivateApp(BOOL bActive, DWORD hTask) 
#endif
{
	CWnd::OnActivateApp(bActive, hTask);
	
	if( !bActive )
		_CancelActions();
	
}

bool CExtShortcutListWnd::_ProcessMouseWheel( short zDelta, bool bDirectMsg )
{
	ASSERT_VALID( this );
	if(		GetSafeHwnd() == NULL
		||	( ! ::IsWindow(GetSafeHwnd()) )
		||	(GetStyle() & WS_VISIBLE) == 0
		||	( ! IsWindowEnabled() )
		)
		return false;

	if( m_bPressedTracking )
		return true;

	if( !bDirectMsg )
	{
		CPoint ptCursor;
		if( ! ::GetCursorPos(&ptCursor) )
			return false;
		HWND hWndFromPoint = ::WindowFromPoint( ptCursor );
		if( hWndFromPoint == NULL )
			return false;
		if( hWndFromPoint != m_hWnd )
		{
			HWND hWndContentExpand = m_wndContentExpand.GetSafeHwnd();
			if( hWndFromPoint != hWndContentExpand )
				return false;
		}
	}
	
	_CancelActions();

LONG nItemCount = ItemGetCount();
	if( nItemCount == 0 )
		return true;
LONG nIndexVisibleFirst, nIndexVisibleLast;
	GetVisibleRange( nIndexVisibleFirst, nIndexVisibleLast );
	ASSERT( 0 <= nIndexVisibleFirst && nIndexVisibleFirst < nItemCount );
	ASSERT( 0 <= nIndexVisibleLast && nIndexVisibleLast < nItemCount );
CRect rcItem;
	ItemGetRects( nIndexVisibleFirst, NULL, NULL, &rcItem );
	if( rcItem.IsRectEmpty() )
		return true;
LONG nStep = ( GetShortcutListStyle() & __ESLW_HORIZONTAL )
		? rcItem.Width()
		: rcItem.Height()
		;
	ASSERT( nStep > 0 );
	if( zDelta > 0 )
		nStep = - nStep;

LONG nScrollPos = m_nScrollPos;
	nScrollPos += nStep;
	if( nScrollPos < 0 )
		nScrollPos = 0;
	else if( nScrollPos > m_nScrollSize )
		nScrollPos = m_nScrollSize;
	if( m_nScrollPos != nScrollPos )
	{
		m_nScrollPos = nScrollPos;
		UpdateShortcutListWnd( true );
	}
	return true;
}

BOOL CExtShortcutListWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
//	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
	nFlags;
	pt;
	return _ProcessMouseWheel( zDelta, true ) ? TRUE : FALSE;
}

void CExtShortcutListWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
//	CWnd::OnMouseMove(nFlags, point);
	nFlags;

	_AnalyzeHover( point );
}

void CExtShortcutListWnd::_AnalyzeHover( CPoint point )
{
	ASSERT_VALID( this );

	if( !OnShortcutCanDoHover() )
		return;

LONG nHT = ItemHitTest( point );

	if( m_bPressedTracking )
	{
		if(		m_nTrackingHT >= 0
			&&	(m_dwShortcutListStyle & __ESLW_START_DRAG) != 0
			)
		{ // analyze stard dragging conditions
			LONG nIndex = m_nTrackingHT;
			ASSERT( nIndex < ItemGetCount() );
			if( OnShortcutListCanStartDrag(
					nIndex,
					m_ptStartLeftBtnTrack,
					point
					)
				)
			{
				_CancelActions();
				OnShortcutListStartItemDrag(
					nIndex,
					m_ptStartLeftBtnTrack,
					point
					);
			}
			return;
		} // analyze stard dragging conditions

		return;
	}

	if( m_nTrackingHT == nHT )
		return;

	if( nHT == __ESLW_HT_BUTTON_UP || nHT == __ESLW_HT_BUTTON_DOWN )
	{
		_CancelActions();
	
		m_nTrackingHT = nHT;
		SetTimer(
			__EXT_SHORTCUTLISTWND_HOVER_TIMER_ID,
			__EXT_SHORTCUTLISTWND_HOVER_TIMER_PERIOD,
			NULL
			);
		InvalidateRect(
			(nHT == __ESLW_HT_BUTTON_UP)
				? ( & m_rcButtonUp )
				: ( & m_rcButtonDown )
			);
		UpdateWindow();
		return;
	} // if( nHT == __ESLW_HT_BUTTON_UP || nHT == __ESLW_HT_BUTTON_DOWN )

	if( nHT >= 0 )
	{
		LONG nItemCount = ItemGetCount();
		ASSERT( nHT < nItemCount );
		_CancelActions();
		if( !OnShortcutListItemHoverStart( nHT, true ) )
			return;
		m_nTrackingHT = nHT;
		SetTimer(
			__EXT_SHORTCUTLISTWND_HOVER_TIMER_ID,
			__EXT_SHORTCUTLISTWND_HOVER_TIMER_PERIOD,
			NULL
			);
		CRect rcBoundsItem;
		ItemGetRects( nHT, NULL, NULL, &rcBoundsItem );
		rcBoundsItem.InflateRect(
			__EXT_SHORTCUTLISTWND_ITEM_GAP_X,
			__EXT_SHORTCUTLISTWND_ITEM_GAP_Y
			);
		InvalidateRect( &rcBoundsItem );
		UpdateWindow();
		OnShortcutListItemHoverStart( nHT, false );
		return;
	}

	if( m_nTrackingHT != __ESLW_HT_NOWHERE )
		_CancelActions();
}

bool CExtShortcutListWnd::OnShortcutListCanStartDrag(
	LONG nIndex,
	CPoint ptClientTrackStart,
	CPoint ptClientTrackCurrent
	)
{
	ASSERT_VALID( this );
	ASSERT( nIndex >= 0 && nIndex < ItemGetCount() );
	nIndex;
	if( (GetShortcutListStyle() & __ESLW_START_DRAG) == 0 )
		return false;
CPoint ptDiff(
		labs( ptClientTrackStart.x - ptClientTrackCurrent.x ),
		labs( ptClientTrackStart.y - ptClientTrackCurrent.y )
		);
	if( ptDiff.x >= 10 || ptDiff.y >= 10 )
		return true;
	return false;
}

void CExtShortcutListWnd::OnShortcutListStartItemDrag(
	LONG nIndex,
	CPoint ptClientTrackStart,
	CPoint ptClientTrackCurrent
	)
{
	ASSERT_VALID( this );
	ASSERT( nIndex >= 0 && nIndex < ItemGetCount() );
	nIndex;
	ptClientTrackStart;
	ptClientTrackCurrent;
}

void CExtShortcutListWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
//	CWnd::OnLButtonDblClk(nFlags, point);
	nFlags;
	point;
}

void CExtShortcutListWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
//	CWnd::OnLButtonDown(nFlags, point);
	nFlags;
	point;

//	SetFocus();
	
	_CancelActions();
LONG nHT = ItemHitTest( point );

	if( nHT == __ESLW_HT_BUTTON_UP || nHT == __ESLW_HT_BUTTON_DOWN )
	{
		OnShortcutListMeasureScrollParms(
			m_nScrollStep,
			m_nScrollPeriod,
			(m_dwShortcutListStyle & __ESLW_SMOOTH_SCROLL) ? true : false
			);

		if( m_nScrollStep > 0 && m_nScrollPeriod > 0 )
		{
			m_nTrackingHT = nHT;
			m_bPressedTracking = true;
			CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );

			InvalidateRect(
				(nHT == __ESLW_HT_BUTTON_UP)
					? ( & m_rcButtonUp )
					: ( & m_rcButtonDown )
				);
			UpdateWindow();

			SetTimer(
				__EXT_SHORTCUTLISTWND_PRESSED_TIMER_ID,
				m_nScrollPeriod,
				NULL
				);
			OnTimer( __EXT_SHORTCUTLISTWND_PRESSED_TIMER_ID );
		} // if( m_nScrollStep > 0 && m_nScrollPeriod > 0 )

		return;
	} // if( nHT == __ESLW_HT_BUTTON_UP || nHT == __ESLW_HT_BUTTON_DOWN )

	if( nHT >= 0 )
	{
		LONG nItemCount = ItemGetCount();
		ASSERT( nHT < nItemCount );
		if( !OnShortcutListItemPressedStart( nHT, true ) )
			return;
		m_nTrackingHT = nHT;
		m_bPressedTracking = true;
		m_ptStartLeftBtnTrack = point;
		CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
		CRect rcBoundsItem;
		ItemGetRects( nHT, NULL, NULL, &rcBoundsItem );
		rcBoundsItem.InflateRect(
			__EXT_SHORTCUTLISTWND_ITEM_GAP_X,
			__EXT_SHORTCUTLISTWND_ITEM_GAP_Y
			);
		InvalidateRect( &rcBoundsItem );
		UpdateWindow();
		OnShortcutListItemPressedStart( nHT, false );
		return;
	}

}

void CExtShortcutListWnd::OnShortcutListMeasureScrollParms(
	LONG & nScrollStep,
	UINT & nScrollPeriod,
	bool bSmoothScroll
	)
{
	ASSERT_VALID( this );

	if( bSmoothScroll )
	{
		nScrollPeriod = __EXT_SHORTCUTLISTWND_PRESSED_TIMER_PERIOD_SMOOTH;
		nScrollStep = __EXT_SHORTCUTLISTWND_SCROLL_STEP_SIZE_SMOOTH;
	} // if( bSmoothScroll )
	else
	{
		nScrollPeriod = __EXT_SHORTCUTLISTWND_PRESSED_TIMER_PERIOD_NORMAL;
		CRect rcClient;
		GetClientRect( &rcClient );
		bool bHorzLayout = IsHorizontalLayout();
		nScrollStep =
			bHorzLayout
				? ( rcClient.Width() - 5 )
				: ( rcClient.Height() - 5 )
				;
		if( nScrollStep < __EXT_SHORTCUTLISTWND_SCROLL_STEP_SIZE_SMOOTH )
			nScrollStep = __EXT_SHORTCUTLISTWND_SCROLL_STEP_SIZE_SMOOTH;
	} // else from if( bSmoothScroll )
}

void CExtShortcutListWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
//	CWnd::OnLButtonUp(nFlags, point);
	nFlags;
	point;

	if( m_hWndEditor != NULL )
		return;

	if( m_nTrackingHT >= 0 && m_bPressedTracking )
	{
		LONG nHT = ItemHitTest( point );
		if( m_nTrackingHT == nHT )
		{
			m_nTrackingHT = __ESLW_HT_NOWHERE;
			m_bPressedTracking = false;

			KillTimer( __EXT_SHORTCUTLISTWND_HOVER_TIMER_ID );
			KillTimer( __EXT_SHORTCUTLISTWND_PRESSED_TIMER_ID );
			ASSERT( nHT < ItemGetCount() );
			CRect rcBoundsItem;
			ItemGetRects( nHT, NULL, NULL, &rcBoundsItem );
			rcBoundsItem.InflateRect(
				__EXT_SHORTCUTLISTWND_ITEM_GAP_X,
				__EXT_SHORTCUTLISTWND_ITEM_GAP_Y
				);
			InvalidateRect( &rcBoundsItem );
			OnShortcutListItemPressedStop(
				nHT,
				true
				);
			UpdateWindow();
			if( m_hWndEditor != NULL )
				return;
		}
		_CancelActions();
		_AnalyzeHover( point );
		return;
	}

	_CancelActions();
	_AnalyzeHover( point );
}

void CExtShortcutListWnd::OnTimer(UINT nIDEvent) 
{
	switch( nIDEvent )
	{
	case __EXT_SHORTCUTLISTWND_DELAY_UPDATE_TIMER_ID:
	{
		KillTimer( nIDEvent );
		if( m_bDelayRecalcLayout )
			UpdateShortcutListWnd( true );
		else
		{
			Invalidate();
			UpdateWindow();
		}
		return;
	} // case __EXT_SHORTCUTLISTWND_DELAY_UPDATE_TIMER_ID
	case __EXT_SHORTCUTLISTWND_HOVER_TIMER_ID:
	{
		if( m_bPressedTracking )
		{
			KillTimer( __EXT_SHORTCUTLISTWND_HOVER_TIMER_ID );
		} // if( m_bPressedTracking )
		else
		{
			CPoint point( 0, 0 );
			if( ! ::GetCursorPos( &point ) )
				return;
			ScreenToClient( &point );
			LONG nHT = ItemHitTest( point );
			if( m_nTrackingHT != nHT )
			{
				KillTimer( __EXT_SHORTCUTLISTWND_HOVER_TIMER_ID );
				_CancelActions();
			}
		} // else from if( m_bPressedTracking )
		return;
	} // case __EXT_SHORTCUTLISTWND_HOVER_TIMER_ID
	case __EXT_SHORTCUTLISTWND_PRESSED_TIMER_ID:
	{
		if(		(	m_nTrackingHT != __ESLW_HT_BUTTON_UP
				&&	m_nTrackingHT != __ESLW_HT_BUTTON_DOWN
				)
			||	m_nScrollSize == 0
			||	( !m_bPressedTracking )
			||	m_nScrollStep == 0
			)
		{
			KillTimer( __EXT_SHORTCUTLISTWND_PRESSED_TIMER_ID );
			return;
		}

		LONG nScrollPos = m_nScrollPos;
		if( m_nTrackingHT == __ESLW_HT_BUTTON_UP )
		{
			nScrollPos -= m_nScrollStep;
			if( nScrollPos < 0 )
				nScrollPos = 0;
		}
		else
		{
			nScrollPos += m_nScrollStep;
			if( nScrollPos > m_nScrollSize )
				nScrollPos = m_nScrollSize;
		}
		if( m_nScrollPos != nScrollPos )
		{
			m_nScrollPos = nScrollPos;
			UpdateShortcutListWnd( true );
		}
		return;
	} // case __EXT_SHORTCUTLISTWND_PRESSED_TIMER_ID
	} // switch( nIDEvent )
	
	CWnd::OnTimer(nIDEvent);
}

void CExtShortcutListWnd::OnCaptureChanged(CWnd *pWnd) 
{
//	CWnd::OnCaptureChanged(pWnd);
	if( pWnd != this )
		_CancelActions();
}

bool CExtShortcutListWnd::OnShortcutCanDoHover() const
{
	ASSERT_VALID( this );
	if( m_hWndEditor != NULL )
		return false;
	return
		CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND(
			GetSafeHwnd()
			);
}

bool CExtShortcutListWnd::OnShortcutListItemHoverStart(
	LONG nIndex,
	bool bPreHoverTest
	)
{
	ASSERT_VALID( this );
	ASSERT( nIndex >= 0 && nIndex < ItemGetCount() );
	if( bPreHoverTest )
	{
		if(		m_bPressedTracking
			||	( !OnShortcutCanDoHover() )
			)
			return false;
		SHORTCUT_ITEM_INFO * pSII = ItemGetInfo( nIndex );
		ASSERT( pSII != NULL );
		ASSERT( pSII->m_bVisible );
		pSII->m_bHover = true;
	} // if( bPreHoverTest )
	else
	{
		if( m_dwShortcutListStyle & (__ESLW_ITEM_EXPANDING|__ESLW_ITEM_TOOLTIP) )
		{ // if item expanding or tooltips are enabled
			if( ItemIsPartiallyVisible(nIndex) )
			{
				CRect rcBoundsItem;
				ItemGetRects( nIndex, NULL, NULL, &rcBoundsItem );
				if( m_dwShortcutListStyle & __ESLW_ITEM_EXPANDING )
				{ // if item expanding is enabled
					bool bHorzLayout = IsHorizontalLayout();
					rcBoundsItem.InflateRect(
						__EXT_SHORTCUTLISTWND_ITEM_GAP_X
							+ ( bHorzLayout ? __EXT_SHORTCUTLISTWND_EXPANDED_TOTAL_GAP_H_X : __EXT_SHORTCUTLISTWND_EXPANDED_TOTAL_GAP_V_X )
							,
						__EXT_SHORTCUTLISTWND_ITEM_GAP_Y
							+ ( bHorzLayout ? __EXT_SHORTCUTLISTWND_EXPANDED_TOTAL_GAP_H_Y : __EXT_SHORTCUTLISTWND_EXPANDED_TOTAL_GAP_V_Y )
						);
					m_wndContentExpand.Deactivate();
					CExtPopupMenuWnd::PassMsgLoop( false );
					if(		GetSafeHwnd() == NULL
						||	( ! ::IsWindow( GetSafeHwnd() ) )
						)
						return false;
					m_wndContentExpand.Activate(
						rcBoundsItem,
						this,
						__ECWAF_DEFAULT|__ECWAF_DRAW_SOURCE
						);
				} // if item expanding is enabled
				else if(		(m_dwShortcutListStyle & __ESLW_ITEM_TOOLTIP) != 0
					&&	m_wndToolTip.GetSafeHwnd() != NULL
					)
				{ // if tooltips are enabled
					SHORTCUT_ITEM_INFO * pSII = ItemGetInfo( nIndex );
					ASSERT( pSII != NULL );
					if( ! pSII->m_sText.IsEmpty() )
						m_wndToolTip.AddTool(
							this,
							pSII->m_sText,
							&rcBoundsItem,
							1
							);
				} // if tooltips are enabled
			} // if( ItemIsPartiallyVisible(nIndex) )
		} // if item expanding or tooltips are enabled
	} // else from 
	return true;
}

void CExtShortcutListWnd::OnShortcutListItemHoverStop(
	LONG nIndex
	)
{
	ASSERT_VALID( this );
	ASSERT( nIndex >= 0 && nIndex < ItemGetCount() );
SHORTCUT_ITEM_INFO * pSII = ItemGetInfo( nIndex );
	ASSERT( pSII != NULL );
	pSII->m_bHover = false;

CFrameWnd * pParentFrame = GetParentFrame();
	if( pParentFrame != NULL
		&& pParentFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd))
		)
		SetTimer(
			__EXT_SHORTCUTLISTWND_DELAY_UPDATE_TIMER_ID,
			__EXT_SHORTCUTLISTWND_DELAY_UPDATE_TIMER_PERIOD,
			NULL
			);
}

LONG CExtShortcutListWnd::ScrollSizeGet() const
{
	ASSERT_VALID( this );
	return m_nScrollSize;
}

LONG CExtShortcutListWnd::ScrollPosGet() const
{
	ASSERT_VALID( this );
	return m_nScrollPos;
}

void CExtShortcutListWnd::ScrollPosSet(
	LONG nScrollPos,
	bool bUpdateNow // = false
	)
{
	ASSERT_VALID( this );
	if( m_nScrollSize == 0 )
		return;
	if( nScrollPos < 0 )
		nScrollPos = 0;
	else if( nScrollPos > m_nScrollSize )
		nScrollPos = m_nScrollSize;
	if( m_nScrollPos != nScrollPos )
	{
		m_nScrollPos = nScrollPos;
		UpdateShortcutListWnd( bUpdateNow );
	}
}

bool CExtShortcutListWnd::ItemIsPartiallyVisible(
	LONG nIndex,
	bool bTestVerticalVisibility, //  = true
	bool bTestHorizontalVisibility, //  = true
	bool bTestIconVisibility, // = true
	bool bTestTextVisibility // = true
	) const
{
	ASSERT_VALID( this );

	if( (!bTestIconVisibility) && (!bTestTextVisibility) )
		return false;

LONG nItemCount = ItemGetCount();
	if( nItemCount == 0 )
		return false;

	if( nIndex < 0 || nIndex >= nItemCount )
		return false;

const SHORTCUT_ITEM_INFO * pSII = ItemGetInfo( nIndex );
	ASSERT( pSII != NULL );
	if( !pSII->m_bVisible )
		return false;

//	if( m_nScrollSize == 0 )
//		return false;

CRect rcBoundsIcon, rcBoundsText, rcBoundsAll;
	ItemGetRects(
		nIndex,
		&rcBoundsIcon,
		&rcBoundsText,
		&rcBoundsAll
		);
CRect rcClient;
	GetClientRect( &rcClient );

	if( bTestVerticalVisibility )
	{
		if( bTestIconVisibility )
		{
			if(		rcBoundsIcon.left < rcClient.left
				||	rcBoundsIcon.right > rcClient.right
				)
				return true;
		}
		if( bTestTextVisibility )
		{
			if(		rcBoundsText.left < rcClient.left
				||	rcBoundsText.right > rcClient.right
				)
				return true;
		}
		if( bTestIconVisibility && bTestTextVisibility )
		{
			if(		rcBoundsAll.left < rcClient.left
				||	rcBoundsAll.right > rcClient.right
				)
				return true;
		}
	} // if( bTestVerticalVisibility )

	if( bTestHorizontalVisibility )
	{
		if( bTestIconVisibility )
		{
			if(		rcBoundsIcon.top < rcClient.top
				||	rcBoundsIcon.bottom > rcClient.bottom
				)
				return true;
		}
		if( bTestTextVisibility )
		{
			if(		rcBoundsText.top < rcClient.top
				||	rcBoundsText.bottom > rcClient.bottom
				)
				return true;
		}
		if( bTestIconVisibility && bTestTextVisibility )
		{
			if(		rcBoundsAll.top < rcClient.top
				||	rcBoundsAll.bottom > rcClient.bottom
				)
				return true;
		}
	} // if( bTestHorizontalVisibility )

	return false;
}

bool CExtShortcutListWnd::ItemEnsureVisible(
	LONG nIndex,
	bool bUpdateNow // = false
	)
{
	ASSERT_VALID( this );

LONG nItemCount = ItemGetCount();
	if( nItemCount == 0 )
		return false;

	if( nIndex < 0 || nIndex >= nItemCount )
		return false;

SHORTCUT_ITEM_INFO * pSII = ItemGetInfo( nIndex );
	ASSERT( pSII != NULL );
	if( !pSII->m_bVisible )
		return false;

	if( m_nScrollSize == 0 )
		return true;

CRect rcBoundsItem;
	ItemGetRects( nIndex, NULL, NULL, &rcBoundsItem );
bool bHorzLayout = IsHorizontalLayout();
CRect rcClient;
	GetClientRect( &rcClient );
LONG nScrollPos = m_nScrollPos;
	if( bHorzLayout )
	{
		LONG nOffset0 = rcBoundsItem.left - rcClient.left;
		LONG nOffset1 = rcClient.right - rcBoundsItem.right;
		if( nOffset1 < 0 )
			nScrollPos -= nOffset1;
		if( nOffset0 < 0 )
			nScrollPos += nOffset0;
	} // if( bHorzLayout )
	else
	{
		LONG nOffset0 = rcBoundsItem.top - rcClient.top;
		LONG nOffset1 = rcClient.bottom - rcBoundsItem.bottom;
		if( nOffset1 < 0 )
			nScrollPos -= nOffset1;
		if( nOffset0 < 0 )
			nScrollPos += nOffset0;
	} // else from if( bHorzLayout )

	if( nScrollPos < 0 )
		nScrollPos = 0;
	else if( nScrollPos > m_nScrollSize )
		nScrollPos = m_nScrollSize;

	if( m_nScrollPos != nScrollPos )
	{
		m_nScrollPos = nScrollPos;
		UpdateShortcutListWnd( bUpdateNow );
	}

	return true;
}


bool CExtShortcutListWnd::OnShortcutListItemPressedStart(
	LONG nIndex,
	bool bPrePressedTest
	)
{
	ASSERT_VALID( this );
	ASSERT( nIndex >= 0 && nIndex < ItemGetCount() );
	if( bPrePressedTest )
	{
		SHORTCUT_ITEM_INFO * pSII = ItemGetInfo( nIndex );
		ASSERT( pSII != NULL );
		pSII->m_bPressed = true;
	} // if( bPrePressedTest )
	else
	{
		ItemEnsureVisible( nIndex, true );
	} // else from if( bPrePressedTest )

	return true;
}

void CExtShortcutListWnd::OnShortcutListItemPressedStop(
	LONG nIndex,
	bool bOnItem
	)
{
	ASSERT_VALID( this );
	ASSERT( nIndex >= 0 && nIndex < ItemGetCount() );
	bOnItem;

SHORTCUT_ITEM_INFO * pSII = ItemGetInfo( nIndex );
	ASSERT( pSII != NULL );
	pSII->m_bPressed = false;

	if( bOnItem
		&& (GetShortcutListStyle() & __ESLW_EDIT_ITEMS) != 0
		)
	{
		ItemBeginEdit( nIndex );
	}
}

BOOL CExtShortcutListWnd::PreTranslateMessage(MSG* pMsg) 
{
	if(		(m_dwShortcutListStyle & __ESLW_ITEM_TOOLTIP) != 0
		&&	m_wndToolTip.GetSafeHwnd() != NULL
		)
		m_wndToolTip.RelayEvent( pMsg );

	if( pMsg->message == WM_MOUSEWHEEL )
	{
		if(	_ProcessMouseWheel(
				short( HIWORD(pMsg->wParam) ),
				false
				)
			)
			return TRUE;
	}
	
	return CWnd::PreTranslateMessage(pMsg);
}

#endif // (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)
