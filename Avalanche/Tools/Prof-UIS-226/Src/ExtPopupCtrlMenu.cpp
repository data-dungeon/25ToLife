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

#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)

#if (!defined __EXT_POPUP_CTRL_MENU_H)
	#include <ExtPopupCtrlMenu.h>
#endif

#if (!defined __EXT_MENUCONTROLBAR_H)
	#include <ExtMenuControlBar.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtPopupControlMenuWnd

IMPLEMENT_DYNCREATE(CExtPopupControlMenuWnd, CExtPopupMenuWnd)

BEGIN_MESSAGE_MAP(CExtPopupControlMenuWnd, CExtPopupMenuWnd)
	//{{AFX_MSG_MAP(CExtPopupControlMenuWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

UINT CExtPopupControlMenuWnd::g_nMsgControlInputRetranslate =
	::RegisterWindowMessage(
		_T("CExtPopupControlMenuWnd::g_nMsgControlInputRetranslate")
		);
bool CExtPopupControlMenuWnd::g_bControlMenuWithShadows = false;

CExtPopupControlMenuWnd::CExtPopupControlMenuWnd()
	: m_sizeChildControl( 150, 100 )
	, m_rcChildControl( 0, 0, 0, 0 )
	, m_hWndChildControl( NULL )
{
}

CExtPopupControlMenuWnd::~CExtPopupControlMenuWnd()
{
}

bool CExtPopupControlMenuWnd::IsAllItemsRarelyUsed() const
{
	ASSERT_VALID( this );
	return false;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
// CExtCustomizeSite::ICustomizeDropTarget
DROPEFFECT CExtPopupControlMenuWnd::OnCustomizeTargetOver(
	CExtCustomizeSite::CCmdDragInfo & _dragInfo,
	CPoint point,
	DWORD dwKeyState
	)
{
	ASSERT_VALID( this );
	ASSERT( !_dragInfo.IsEmpty() );
	_dragInfo;
	point;
	dwKeyState;
	return DROPEFFECT_NONE;
}
void CExtPopupControlMenuWnd::OnCustomizeTargetLeave()
{
	ASSERT_VALID( this );
}
bool CExtPopupControlMenuWnd::OnCustomizeTargetDrop(
	CExtCustomizeSite::CCmdDragInfo & _dragInfo,
	CPoint point,
	DROPEFFECT de
	)
{
	ASSERT_VALID( this );
	ASSERT( !_dragInfo.IsEmpty() );;
	_dragInfo;
	point;
	de;
	return false;
}
// CExtCustomizeSite::ICustomizeDropSource
void CExtPopupControlMenuWnd::OnCustomizeSourceDragComplete(
	DROPEFFECT de,
	bool bCanceled,
	bool * p_bNoResetActiveItem
	)
{
	ASSERT_VALID( this );
	ASSERT( p_bNoResetActiveItem != NULL );
	de;
	bCanceled;
	p_bNoResetActiveItem;
}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

bool CExtPopupControlMenuWnd::_CanStartLevelTracking()
{
	ASSERT_VALID( this );
	if( _FindHelpMode() )
		return false;
	if( _FindCustomizeMode() )
		return false;
	return true;
}

bool CExtPopupControlMenuWnd::_OnMouseWheel(
	WPARAM wParam,
	LPARAM lParam,
	bool & bNoEat
	)
{
	ASSERT_VALID( this );
	wParam;
	lParam;
	bNoEat;
	if( GetSafeHwnd() == NULL )
		return true;
CExtPopupMenuSite & _site = GetSite();
	if(		_site.GetAnimated() != NULL
		||	_site.IsShutdownMode()
		||	_site.IsEmpty()
		||	_site.GetAnimated() != NULL
		)
		return true;

	if(		m_hWndChildControl != NULL
		&&	::IsWindow( m_hWndChildControl )
		)
	{
		CONTROLINPUTRETRANSLATEINFO _ciri(
			this,
			wParam,
			lParam,
			bNoEat
			);
		bool bRetVal = 
			( ::SendMessage(
				m_hWndChildControl,
				g_nMsgControlInputRetranslate,
				(WPARAM)(&_ciri),
				(LPARAM)0L
				) != 0 )
			? true : false;
		return bRetVal;
	}

CPoint ptScreenClick;
	if( ! ::GetCursorPos( &ptScreenClick ) )
		return true;
HWND hWndFromPoint = ::WindowFromPoint( ptScreenClick );
	if(		hWndFromPoint != NULL
		&&	(::GetWindowLong(hWndFromPoint,GWL_STYLE)&WS_CHILD) != 0
		&&	(	m_hWndChildControl == hWndFromPoint
			||	::IsChild( m_hWndChildControl, hWndFromPoint )
			)
		)
	{
		bNoEat = true;
		return false;
	}
HWND hWndFocus = ::GetFocus();
	if(		hWndFocus != NULL
		&&	(::GetWindowLong(hWndFocus,GWL_STYLE)&WS_CHILD) != 0
		&&	(	m_hWndChildControl == hWndFocus
			||	::IsChild( m_hWndChildControl, hWndFocus )
			)
		)
	{
		bNoEat = true;
		return false;
	}
HWND hWndCapture = ::GetCapture();
	if(		hWndCapture != NULL
		&&	(::GetWindowLong(hWndCapture,GWL_STYLE)&WS_CHILD) != 0
		&&	(	m_hWndChildControl == hWndCapture
			||	::IsChild( m_hWndChildControl, hWndCapture )
			)
		)
	{
		bNoEat = true;
		return false;
	}
	return true;
}

bool CExtPopupControlMenuWnd::_OnMouseMove(
	UINT nFlags,
	CPoint point,
	bool & bNoEat
	)
{
	ASSERT_VALID( this );

	if( GetSafeHwnd() == NULL )
		return false;

	if( GetSite().GetAnimated() != NULL )
		return true;

CExtPopupMenuSite & _site = GetSite();
	if(	_site.IsShutdownMode()
		|| _site.IsEmpty()
		|| _site.GetAnimated() != NULL
		)
		return true;

CPoint ptScreenClick( point );
	ClientToScreen( &ptScreenClick );

HWND hWndFromPoint = ::WindowFromPoint( ptScreenClick );
	if(		hWndFromPoint != NULL
		&&	(::GetWindowLong(hWndFromPoint,GWL_STYLE)&WS_CHILD) != 0
		&&	(	m_hWndChildControl == hWndFromPoint
			||	::IsChild( m_hWndChildControl, hWndFromPoint )
			)
		)
	{
		bNoEat = true;
		return false;
	}
HWND hWndFocus = ::GetFocus();
	if(		hWndFocus != NULL
		&&	(::GetWindowLong(hWndFocus,GWL_STYLE)&WS_CHILD) != 0
		&&	(	m_hWndChildControl == hWndFocus
			||	::IsChild( m_hWndChildControl, hWndFocus )
			)
		)
	{
		bNoEat = true;
		return false;
	}
HWND hWndCapture = ::GetCapture();
	if(		hWndCapture != NULL
		&&	(::GetWindowLong(hWndCapture,GWL_STYLE)&WS_CHILD) != 0
		&&	(	m_hWndChildControl == hWndCapture
			||	::IsChild( m_hWndChildControl, hWndCapture )
			)
		)
	{
		bNoEat = true;
		return false;
	}

	if( !_PtInWndArea(point) )
	{
		if( m_pWndParentMenu != NULL
			&& m_pWndParentMenu->GetSafeHwnd() != NULL
			)
		{
			ASSERT_VALID( m_pWndParentMenu );
			ClientToScreen( &point );
			m_pWndParentMenu->ScreenToClient( &point );
			if( m_pWndParentMenu->_OnMouseMove(
					nFlags,
					point,
					bNoEat
					)
				)
			{
				if( bNoEat )
					return false;
				_OnCancelMode();
				return true;
			}
		}
		return false;
	}

bool bRetVal = false;
	if(		m_hWndChildControl != NULL
		&&	::IsWindow( m_hWndChildControl )
		)
	{
		CONTROLINPUTRETRANSLATEINFO _ciri(
			this,
			true,
			nFlags,
			point,
			bNoEat
			);
		bRetVal = 
			( ::SendMessage(
				m_hWndChildControl,
				g_nMsgControlInputRetranslate,
				(WPARAM)(&_ciri),
				(LPARAM)0L
				) != 0 )
			? true : false;
	}

	return bRetVal;
}

bool CExtPopupControlMenuWnd::_OnMouseClick(
	UINT nFlags,
	CPoint point,
	bool & bNoEat
	)
{
	ASSERT_VALID( this );
	bNoEat;

	if( GetSafeHwnd() == NULL )
		return false;

	if( GetSite().GetAnimated() != NULL )
		return true;

CExtPopupMenuSite & _site = GetSite();
	if(	_site.IsShutdownMode()
		|| _site.IsEmpty()
		|| _site.GetAnimated() != NULL
		)
		return true;

CPoint ptScreenClick( point );
	ClientToScreen( &ptScreenClick );

HWND hWndFromPoint = ::WindowFromPoint( ptScreenClick );
	if(		hWndFromPoint != NULL
		&&	(::GetWindowLong(hWndFromPoint,GWL_STYLE)&WS_CHILD) != 0
		&&	(	m_hWndChildControl == hWndFromPoint
			||	::IsChild( m_hWndChildControl, hWndFromPoint )
			)
		)
	{
		bNoEat = true;
		return false;
	}
HWND hWndFocus = ::GetFocus();
	if(		hWndFocus != NULL
		&&	(::GetWindowLong(hWndFocus,GWL_STYLE)&WS_CHILD) != 0
		&&	(	m_hWndChildControl == hWndFocus
			||	::IsChild( m_hWndChildControl, hWndFocus )
			)
		)
	{
		bNoEat = true;
		return false;
	}
HWND hWndCapture = ::GetCapture();
	if(		hWndCapture != NULL
		&&	(::GetWindowLong(hWndCapture,GWL_STYLE)&WS_CHILD) != 0
		&&	(	m_hWndChildControl == hWndCapture
			||	::IsChild( m_hWndChildControl, hWndCapture )
			)
		)
	{
		bNoEat = true;
		return false;
	}

	if( !_PtInWndArea(point) ) 
	{
		if( m_pWndParentMenu != NULL
			&& m_pWndParentMenu->GetSafeHwnd() != NULL
			)
		{
			ASSERT_VALID( m_pWndParentMenu );
			ClientToScreen( &point );
			m_pWndParentMenu->ScreenToClient( &point );

			HWND hWndOwn = GetSafeHwnd();
			ASSERT( hWndOwn != NULL );
			ASSERT( ::IsWindow(hWndOwn) );
			CExtPopupMenuWnd * pWndParentMenu = m_pWndParentMenu;

			bool bInplaceControlArea = false, bInplaceDropDownArea = false;
			int nHtTemp =
				pWndParentMenu->_HitTest(
					point,
					&bInplaceControlArea,
					&bInplaceDropDownArea
					);
			if( nHtTemp >= 0 )
			{
				MENUITEMDATA & mi = pWndParentMenu->ItemGetInfo( nHtTemp );
				if(		mi.IsPopup()
					&&	mi.GetPopup() == this
					)
				{
					if( bInplaceControlArea )
					{
						pWndParentMenu->_SetCapture();
						pWndParentMenu->_ItemFocusCancel( FALSE );
						pWndParentMenu->_ItemFocusSet( nHtTemp, FALSE, TRUE );
						if( mi.IsAllowInplaceEditActivation() )
						{
							CWnd * pWndInplace = mi.GetInplaceEditPtr();
							if( pWndInplace != NULL )
							{
								ASSERT_VALID( pWndInplace );
								ASSERT( pWndInplace->GetSafeHwnd() != NULL && (::IsWindow(pWndInplace->GetSafeHwnd())) );
								if( (pWndInplace->GetStyle() & WS_VISIBLE) == 0 )
								{
									CRect rcInplaceEdit;
									pWndParentMenu->_GetItemRect( nHtTemp, rcInplaceEdit );
									rcInplaceEdit = mi.AdjustInplaceEditRect( rcInplaceEdit );
									pWndInplace->SetWindowPos(
										NULL,
										rcInplaceEdit.left, rcInplaceEdit.top,
										rcInplaceEdit.Width(), rcInplaceEdit.Height(),
										SWP_NOZORDER|SWP_NOOWNERZORDER
											|SWP_NOACTIVATE|SWP_SHOWWINDOW
										);
								} // if( (pWndInplace->GetStyle() & WS_VISIBLE) == 0 )
								pWndInplace->SetFocus();
							} // if( pWndInplace != NULL )
						} // if( mi.IsAllowInplaceEditActivation() )
					} // if( bInplaceControlArea )
					return true;
				}
			} // if( nHtTemp >= 0 )

			if(	pWndParentMenu->_OnMouseClick(
					nFlags,
					point,
					bNoEat
					)
				)
			{
				if( bNoEat )
					return false;
				if( ::IsWindow(hWndOwn) )
				{
					//_OnCancelMode();
					CancelMenuTracking();
				} // CancelMenuTracking();
				return true;
			}
			return false;
		}

		// fixed in v. 2.20
		if( nFlags == WM_RBUTTONUP
			|| nFlags == WM_LBUTTONUP
			)
		{
			return true;
		}

		_OnCancelMode();
		return false;
	} // if( !_PtInWndArea(point) ) 

bool bRetVal = false;
	if(		m_hWndChildControl != NULL
		&&	::IsWindow( m_hWndChildControl )
		)
	{
		CONTROLINPUTRETRANSLATEINFO _ciri(
			this,
			false,
			nFlags,
			point,
			bNoEat
			);
		bRetVal = 
			( ::SendMessage(
				m_hWndChildControl,
				g_nMsgControlInputRetranslate,
				(WPARAM)(&_ciri),
				(LPARAM)0L
				) != 0 )
			? true : false;
	}

	return bRetVal;
}

bool CExtPopupControlMenuWnd::_OnKeyDown(
	UINT nChar,
	UINT nRepCnt,
	UINT nFlags,
	bool & bNoEat
	)
{
	ASSERT_VALID( this );

	nRepCnt;
	nFlags;
	bNoEat;
	if( GetSafeHwnd() == NULL )
		return true; //false;

	if( GetSite().GetAnimated() != NULL )
		return true;

bool bRetVal = false;
	if(		m_hWndChildControl != NULL
		&&	::IsWindow( m_hWndChildControl )
		)
	{
		CONTROLINPUTRETRANSLATEINFO _ciri(
			this,
			nChar,
			nRepCnt,
			nFlags,
			bNoEat
			);
		bRetVal =
			( ::SendMessage(
				m_hWndChildControl,
				g_nMsgControlInputRetranslate,
				(WPARAM)(&_ciri),
				(LPARAM)0L
				) != 0 )
			? true : false;
	}

	return bRetVal;
}

CRect CExtPopupControlMenuWnd::_CalcTrackRect()
{
	ASSERT_VALID( this );
	return CExtPopupMenuWnd::_CalcTrackRect();
}

CSize CExtPopupControlMenuWnd::_CalcTrackSize()
{
	ASSERT_VALID( this );
int nMenuBorderSize = OnQueryMenuBorderSize();
int nMenuShadowSize = _GetPopupShadowSize();
CSize _size(
		m_sizeChildControl.cx + nMenuBorderSize*2 + nMenuShadowSize,
		m_sizeChildControl.cy + nMenuBorderSize*2 + nMenuShadowSize
		);
	_size.cx += m_nLeftAreaWidth;
	return _size;
}

bool CExtPopupControlMenuWnd::_IsPopupWithShadows() const
{
	if( !g_bControlMenuWithShadows )
		return false;
	return CExtPopupMenuWnd::_IsPopupWithShadows();
}

void CExtPopupControlMenuWnd::_AdjustAnimation(
	CExtPopupBaseWnd::e_animation_type_t & eAT
	)
{
	ASSERT_VALID( this );
	eAT = __AT_CONTENT_DISPLAY;
}

bool CExtPopupControlMenuWnd::_CreateHelper(
	CWnd * pWndCmdReceiver
	)
{
	ASSERT_VALID( this );

	if( !CExtPopupMenuWnd::_CreateHelper(
			pWndCmdReceiver
			)
		)
		return false;

	m_rcChildControl = _RecalcControlRect();
	ASSERT( m_hWndChildControl == NULL );
	m_hWndChildControl = OnCreateChildControl( m_rcChildControl );
/*
	if( m_hWndChildControl == NULL || (! ::IsWindow(m_hWndChildControl) ) )
	{
		ASSERT( FALSE );
		return false;
	}
*/
	return true;
}

CRect CExtPopupControlMenuWnd::_RecalcControlRect()
{
CRect rcClient;
	_GetClientRect( &rcClient );
CRect rcControl;
	rcControl.left = rcControl.top = 0;
	rcControl.right = m_sizeChildControl.cx;
	rcControl.bottom = m_sizeChildControl.cy;
	rcControl.OffsetRect( rcClient.TopLeft() );
	rcControl.OffsetRect( m_nLeftAreaWidth, 0 );
int nMenuBorderSize = OnQueryMenuBorderSize();
	rcControl.OffsetRect( nMenuBorderSize, nMenuBorderSize );
	return rcControl;
}

void CExtPopupControlMenuWnd::_RecalcLayoutImpl()
{
	ASSERT_VALID( this );
	CExtPopupMenuWnd::_RecalcLayoutImpl();
	if( m_hWndChildControl == NULL )
		return;

	ASSERT_VALID( this );

//	ASSERT( ::IsWindow(m_hWndChildControl) );

CRect rcControl = _RecalcControlRect();
	if( rcControl == m_rcChildControl )
		return;
	m_rcChildControl = rcControl;
	::SetWindowPos(
		m_hWndChildControl, NULL,
		m_rcChildControl.left, m_rcChildControl.top,
		m_rcChildControl.Width(), m_rcChildControl.Height(),
		SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOACTIVATE
		);
	if( IsWindowVisible() )
	{
		CClientDC dc(this);
		_DoPaint(dc);
	}
}

HWND CExtPopupControlMenuWnd::OnCreateChildControl(
	const RECT & rcChildControl
	)
{
	ASSERT_VALID( this );
	rcChildControl;
	return NULL;
}

LRESULT CExtPopupControlMenuWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(		message == WM_CLOSE
		||	message == WM_DESTROY
		)
		m_hWndChildControl = NULL;
	return CExtPopupMenuWnd::WindowProc(message,wParam,lParam);
}

/////////////////////////////////////////////////////////////////////////////
// CExtPopupInplaceListBox

IMPLEMENT_DYNCREATE(CExtPopupInplaceListBox, CListBox)

BEGIN_MESSAGE_MAP(CExtPopupInplaceListBox, CListBox)
	//{{AFX_MSG_MAP(CExtPopupInplaceListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

UINT CExtPopupInplaceListBox::g_nMsgPopupListBoxInitContent =
	::RegisterWindowMessage(
		_T("CExtPopupInplaceListBox::g_nMsgPopupListBoxInitContent")
		);
UINT CExtPopupInplaceListBox::g_nMsgPopupListBoxItemMeasure =
	::RegisterWindowMessage(
		_T("CExtPopupInplaceListBox::g_nMsgPopupListBoxItemMeasure")
		);
UINT CExtPopupInplaceListBox::g_nMsgPopupListBoxItemDraw =
	::RegisterWindowMessage(
		_T("CExtPopupInplaceListBox::g_nMsgPopupListBoxItemDraw")
		);
UINT CExtPopupInplaceListBox::g_nMsgPopupListBoxSelEndOK =
	::RegisterWindowMessage(
		_T("CExtPopupInplaceListBox::g_nMsgPopupListBoxSelEndOK")
		);
UINT CExtPopupInplaceListBox::g_nMsgPopupListBoxSelEndCancel =
	::RegisterWindowMessage(
		_T("CExtPopupInplaceListBox::g_nMsgPopupListBoxSelEndCancel")
		);
UINT CExtPopupInplaceListBox::g_nMsgPopupListBoxSelChange =
	::RegisterWindowMessage(
		_T("CExtPopupInplaceListBox::g_nMsgPopupListBoxSelChange")
		);

CExtPopupInplaceListBox::CExtPopupInplaceListBox()
	: m_pCbInitListBoxContent( NULL )
	, m_pInitListBoxCoockie( NULL )
	, m_pCbListBoxSelection( NULL )
	, m_pSelectionCoockie( NULL )
	, m_pCbListBoxItemDraw( NULL )
	, m_pCbListBoxItemMeasure( NULL )
	, m_pListBoxItemCoockie( NULL )
	, m_lParamCookie( 0L )
	, m_bSelEndNotificationPassed( false )
{
}

CExtPopupInplaceListBox::~CExtPopupInplaceListBox()
{
}

void CExtPopupInplaceListBox::DrawItem(LPDRAWITEMSTRUCT pDIS)
{
	ASSERT_VALID( this );
	ASSERT( pDIS != NULL );
	ASSERT( pDIS->hDC != NULL );
	if(		m_pCbListBoxItemDraw != NULL
		&&	m_pCbListBoxItemDraw(
				*this,
				m_pListBoxItemCoockie,
				pDIS
				)
		)
		return;
CExtPopupControlMenuWnd * pPopup =
		STATIC_DOWNCAST( CExtPopupControlMenuWnd, GetParent() );
	ASSERT_VALID( pPopup );
	ASSERT( pPopup->m_hWnd != NULL && ::IsWindow(pPopup->m_hWnd) );
HWND hWndCmdReceiver = pPopup->GetCmdReceiverHWND();
	ASSERT( hWndCmdReceiver != NULL && ::IsWindow(hWndCmdReceiver) );
POPUPLISTBOXITEMDRAWINFO plbdii(
		this,
		pDIS
		);
	::SendMessage(
		hWndCmdReceiver,
		g_nMsgPopupListBoxItemDraw,
		(WPARAM)(&plbdii),
		(LPARAM)m_lParamCookie
		);
}

void CExtPopupInplaceListBox::MeasureItem(LPMEASUREITEMSTRUCT pMIS)
{
	ASSERT_VALID( this );
	ASSERT( pMIS != NULL );
	if(		m_pCbListBoxItemMeasure != NULL
		&&	m_pCbListBoxItemMeasure(
				*this,
				m_pListBoxItemCoockie,
				pMIS
				)
		)
		return;
CExtPopupControlMenuWnd * pPopup =
		STATIC_DOWNCAST( CExtPopupControlMenuWnd, GetParent() );
	ASSERT_VALID( pPopup );
	ASSERT( pPopup->m_hWnd != NULL && ::IsWindow(pPopup->m_hWnd) );
HWND hWndCmdReceiver = pPopup->GetCmdReceiverHWND();
	ASSERT( hWndCmdReceiver != NULL && ::IsWindow(hWndCmdReceiver) );
POPUPLISTBOXITEMMEASUREINFO plbmii(
		this,
		pMIS
		);
	::SendMessage(
		hWndCmdReceiver,
		g_nMsgPopupListBoxItemMeasure,
		(WPARAM)(&plbmii),
		(LPARAM)m_lParamCookie
		);
}

void CExtPopupInplaceListBox::PostNcDestroy()
{
	delete this;
}

LRESULT CExtPopupInplaceListBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_CREATE )
	{
		m_bSelEndNotificationPassed = false;
		LRESULT lResult = CListBox::WindowProc(message,wParam,lParam);
		SetFont( &g_PaintManager->m_FontNormal );
		bool bCbInitSucceeded = false;
		if( m_pCbInitListBoxContent != NULL )
			bCbInitSucceeded =
				m_pCbInitListBoxContent(
					*this,
					m_pInitListBoxCoockie
					);
		if( !bCbInitSucceeded )
		{
			CExtPopupControlMenuWnd * pPopup =
				STATIC_DOWNCAST( CExtPopupControlMenuWnd, GetParent() );
			ASSERT_VALID( pPopup );
			ASSERT( pPopup->m_hWnd != NULL && ::IsWindow(pPopup->m_hWnd) );
			HWND hWndCmdReceiver = pPopup->GetCmdReceiverHWND();
			ASSERT( hWndCmdReceiver != NULL && ::IsWindow(hWndCmdReceiver) );
			::SendMessage(
				hWndCmdReceiver,
				g_nMsgPopupListBoxInitContent,
				(WPARAM)this,
				(LPARAM)m_lParamCookie
				);
		} // if( !bCbInitSucceeded )
		return lResult;
	} // if( message == WM_CREATE )
	if( message == WM_DESTROY )
	{
		_DoSelEndCancel( true );
		return CListBox::WindowProc(message,wParam,lParam);
	} // if( message == WM_DESTROY )
	if( message == WM_MOUSEACTIVATE )
		return MA_NOACTIVATE;
	if( message == WM_MOUSEMOVE )
	{
		INT nCount = CListBox::GetCount();
		if( nCount == 0 )
			return 0;
		BOOL bOutside = TRUE;
		CPoint point( (DWORD)lParam );
		INT nHitTest = (INT)CListBox::ItemFromPoint( point, bOutside );
		if( bOutside )
		{
			if( GetCapture() != this )
				return 0;
			INT nTop = CListBox::GetTopIndex();
			if( nHitTest <= nTop )
			{
				if( nTop > 0 )
					nHitTest--;
			} // if( nHitTest <= nTop )
			else
			{
				if( nCount > 0 && nHitTest >= (nCount-2) )
					nHitTest++;
			} // else from if( nHitTest <= nTop )
		} // if( bOutside )
		INT nCurSel = CListBox::GetCurSel();
		if( nCurSel == nHitTest )
			return 0;
		CListBox::SetCurSel( nHitTest );
		_DoSelChange();
		return 0;
	} // if( message == WM_MOUSEMOVE )
	if(		message == WM_RBUTTONUP
		||	message == WM_RBUTTONDOWN
		||	message == WM_RBUTTONDBLCLK
		||	message == WM_MBUTTONUP
		||	message == WM_MBUTTONDOWN
		||	message == WM_MBUTTONDOWN
//		||	message == WM_LBUTTONUP
//		||	message == WM_LBUTTONDOWN
		||	message == WM_LBUTTONDBLCLK
		)
		return 0;
	if( message == WM_LBUTTONDOWN )
	{
		INT nCount = CListBox::GetCount();
		if( nCount == 0 )
			return 0;
		SetCapture();
		return 0;
	}
	if( message == WM_LBUTTONUP )
	{
		if( GetCapture() == this )
			ReleaseCapture();
		INT nCount = CListBox::GetCount();
		if( nCount > 0 )
		{
			BOOL bOutside = TRUE;
			CPoint point( (DWORD)lParam );
			INT nHitTest = (INT)CListBox::ItemFromPoint( point, bOutside );
			nHitTest;
			if( !bOutside )
			{
				_DoSelEndOK();
			} // if( !bOutside )
		} // if( nCount > 0 )
		return 0;
	} // if( message == WM_LBUTTONUP )
	if( message == WM_CANCELMODE )
	{
		if( GetCapture() == this )
			ReleaseCapture();
		_DoSelEndCancel();
		return 0;
	} // if( message == WM_CANCELMODE )
	if( message == CExtPopupControlMenuWnd::g_nMsgControlInputRetranslate )
	{
		ASSERT( wParam != 0 );
		CExtPopupControlMenuWnd::CONTROLINPUTRETRANSLATEINFO & _ciri =
			*((CExtPopupControlMenuWnd::CONTROLINPUTRETRANSLATEINFO*)wParam);
		if( _ciri.m_eRTT == CExtPopupControlMenuWnd::CONTROLINPUTRETRANSLATEINFO::__ERTT_MOUSE_WHEEL )
		{
			if( ! g_PaintManager.m_bIsWin2000orLater )
				return 1;
			DWORD dwWndStyle = GetStyle();
			if( (dwWndStyle & WS_VSCROLL) == 0 )
				return 1; // only vertical scrolling is supported
			int nItemCount = CListBox::GetCount();
			if( nItemCount == 0 )
				return 1;
			struct __SAME_AS_MOUSEHOOKSTRUCTEX
			{
				MOUSEHOOKSTRUCT mhs;
				DWORD mouseData;
			};
			__SAME_AS_MOUSEHOOKSTRUCTEX * pMHEX =
				reinterpret_cast
					< __SAME_AS_MOUSEHOOKSTRUCTEX * >
						( _ciri.m_lParam );
			ASSERT( pMHEX != NULL );
			DWORD dwWheelDeltaAndZeroFlags =
				DWORD( pMHEX->mouseData ) & 0xFFFF0000;
			if( dwWheelDeltaAndZeroFlags == 0 )
				return 1;
			int yAmount =
				( int(short(dwWheelDeltaAndZeroFlags>>16)) > 0 )
					? (-1) : 1;
			int nMouseWheelScrollLines =
				(int)g_PaintManager.GetMouseWheelScrollLines();
			if( nMouseWheelScrollLines > 2 )
				nMouseWheelScrollLines--; // offset is 1 less
			yAmount *= nMouseWheelScrollLines;
			int nTopIdxSrc = CListBox::GetTopIndex();
			int nTopIdxDst = nTopIdxSrc + yAmount;
			if( nTopIdxDst < 0 )
				nTopIdxDst = 0;
			else if( nTopIdxDst >= nItemCount )
				nTopIdxDst = nItemCount - 1;
			if( nTopIdxDst == nTopIdxSrc )
				return 0;
			CListBox::SetTopIndex( nTopIdxDst );
			_ciri.m_bNoEat = true;
			return 0;
		} // if( _ciri.m_eRTT == CExtPopupControlMenuWnd::CONTROLINPUTRETRANSLATEINFO::__ERTT_MOUSE_WHEEL )
		if( _ciri.m_eRTT != CExtPopupControlMenuWnd::CONTROLINPUTRETRANSLATEINFO::__ERTT_KEYBOARD )
			return 0;
		bool bAlt =
			( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 )
				? true : false;
		if( bAlt )
		{
			_DoSelEndCancel();
			return 1;
		}
		bool bCtrl =
			( (::GetAsyncKeyState(VK_CONTROL)&0x8000) != 0 )
				? true : false;
		bool bShift =
			( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 )
				? true : false;
		if( bAlt || bCtrl || bShift )
			return 0;
		if( _ciri.m_nChar == VK_ESCAPE )
		{
			_DoSelEndCancel();
			return 1;
		} // if( _ciri.m_nChar == VK_ESCAPE )
		if( _ciri.m_nChar == VK_RETURN )
		{
			_DoSelEndOK();
			return 1;
		} // if( _ciri.m_nChar == VK_ESCAPE )
		if( _ciri.m_nChar == VK_RIGHT )
			return 1;
		if( _ciri.m_nChar == VK_LEFT )
		{
			CExtPopupControlMenuWnd * pPopup =
				STATIC_DOWNCAST( CExtPopupControlMenuWnd, GetParent() );
			ASSERT_VALID( pPopup );
			ASSERT( pPopup->m_hWnd != NULL && ::IsWindow(pPopup->m_hWnd) );
			CExtPopupMenuWnd * pWndParentMenu = pPopup->GetParentMenuWnd();
			if( pWndParentMenu == NULL )
				return 1;
			int nParentCurIndex = pWndParentMenu->_GetCurIndex();
			ASSERT(
					nParentCurIndex >= 0
				&&	nParentCurIndex <= pWndParentMenu->ItemGetCount()
				);
			pWndParentMenu->_ItemFocusCancel( FALSE );
			pWndParentMenu->_ItemFocusSet(
				nParentCurIndex,
				FALSE,
				TRUE
				);
			pWndParentMenu->_SetCapture();
			return 1;
		} // if( _ciri.m_nChar == VK_LEFT )
		INT nCurSelOld = CListBox::GetCurSel();
		DefWindowProc( WM_KEYDOWN, _ciri.m_nChar, _ciri.m_nFlags );
		INT nCurSelNew = CListBox::GetCurSel();
		if( nCurSelOld != nCurSelNew )
			_DoSelChange();
		return 0;
	} // if( message == CExtPopupControlMenuWnd::g_nMsgControlInputRetranslate )
	return CListBox::WindowProc(message,wParam,lParam);
}

void CExtPopupInplaceListBox::_DoSelEndOK()
{
	ASSERT_VALID( this );
	if( m_bSelEndNotificationPassed )
		return;
	m_bSelEndNotificationPassed = true;
HWND hWndThis = m_hWnd;
	ASSERT( hWndThis != NULL && ::IsWindow(hWndThis) );
	if(		m_pCbListBoxSelection != NULL
		&&	m_pCbListBoxSelection(
				*this,
				m_pSelectionCoockie,
				__SAT_SELENDOK
				)
		)
		return;
	if( ::IsWindow( hWndThis ) )
	{
		CExtPopupControlMenuWnd * pPopup =
			STATIC_DOWNCAST( CExtPopupControlMenuWnd, GetParent() );
		ASSERT_VALID( pPopup );
		ASSERT( pPopup->m_hWnd != NULL && ::IsWindow(pPopup->m_hWnd) );
		HWND hWndCmdReceiver = pPopup->GetCmdReceiverHWND();
		ASSERT( hWndCmdReceiver != NULL && ::IsWindow(hWndCmdReceiver) );
		POPUPLISTBOXITEMSELENDINFO _plbsei( this );
		::SendMessage(
			hWndCmdReceiver,
			g_nMsgPopupListBoxSelEndOK,
			(WPARAM)(&_plbsei),
			(LPARAM)m_lParamCookie
			);
		pPopup->SendMessage( WM_CANCELMODE );
	} // if( ::IsWindow( hWndThis ) )
	CExtToolControlBar::_CloseTrackingMenus();
}

void CExtPopupInplaceListBox::_DoSelEndCancel(
	bool bFinalDestroyMode // = false
	)
{
	ASSERT_VALID( this );
	if( m_bSelEndNotificationPassed )
		return;
	m_bSelEndNotificationPassed = true;
HWND hWndThis = m_hWnd;
	ASSERT( hWndThis != NULL && ::IsWindow(hWndThis) );
	if(		m_pCbListBoxSelection != NULL
		&&	m_pCbListBoxSelection(
				*this,
				m_pSelectionCoockie,
				__SAT_SELENDCANCEL
				)
		)
		return;
	if( bFinalDestroyMode )
		return;
	if( ::IsWindow( hWndThis ) )
	{
		CExtPopupControlMenuWnd * pPopup =
			STATIC_DOWNCAST( CExtPopupControlMenuWnd, GetParent() );
		ASSERT_VALID( pPopup );
		ASSERT( pPopup->m_hWnd != NULL && ::IsWindow(pPopup->m_hWnd) );
		HWND hWndCmdReceiver = pPopup->GetCmdReceiverHWND();
		ASSERT( hWndCmdReceiver != NULL && ::IsWindow(hWndCmdReceiver) );
		POPUPLISTBOXITEMSELENDINFO _plbsei( this );
		::SendMessage(
			hWndCmdReceiver,
			g_nMsgPopupListBoxSelEndCancel,
			(WPARAM)(&_plbsei),
			(LPARAM)m_lParamCookie
			);
		pPopup->SendMessage( WM_CANCELMODE );
	} // if( ::IsWindow( hWndThis ) )
	CExtToolControlBar::_CloseTrackingMenus();
}

void CExtPopupInplaceListBox::_DoSelChange()
{
	ASSERT_VALID( this );
HWND hWndThis = m_hWnd;
	ASSERT( hWndThis != NULL && ::IsWindow(hWndThis) );
	if(		m_pCbListBoxSelection != NULL
		&&	m_pCbListBoxSelection(
				*this,
				m_pSelectionCoockie,
				__SAT_SELCHANGE
				)
		)
		return;
	if( ! ::IsWindow( hWndThis ) )
		return;
CExtPopupControlMenuWnd * pPopup =
		STATIC_DOWNCAST( CExtPopupControlMenuWnd, GetParent() );
	ASSERT_VALID( pPopup );
	ASSERT( pPopup->m_hWnd != NULL && ::IsWindow(pPopup->m_hWnd) );
CExtPopupMenuWnd * pWndParentMenu = pPopup->GetParentMenuWnd();
	if( pWndParentMenu != NULL )
	{
		int nParentCurIndex = pWndParentMenu->_GetCurIndex();
		ASSERT(
				nParentCurIndex >= 0
			&&	nParentCurIndex <= pWndParentMenu->ItemGetCount()
			);
		CExtPopupMenuWnd::MENUITEMDATA & mi =
			pWndParentMenu->ItemGetInfo( nParentCurIndex );
		bool bResetTempSelText = true;
		if( GetStyle() & LBS_HASSTRINGS )
		{
			INT nCurSel = CListBox::GetCurSel();
			if(		CListBox::GetCount() > 0
				&&	nCurSel >= 0
				)
			{
				CExtSafeString sTempSelText;
				CListBox::GetText( nCurSel, *((CString*)&sTempSelText) );
				if( !sTempSelText.IsEmpty() )
				{
					mi.SetTempSelectedInplaceEditText( sTempSelText );
					bResetTempSelText = false;
				}
			}
		} // if( GetStyle() & LBS_HASSTRINGS )
		if( bResetTempSelText )
			mi.SetTempSelectedInplaceEditText( NULL );
		CClientDC dc( pWndParentMenu );
		pWndParentMenu->_DoPaint( dc );
	} // if( pWndParentMenu != NULL )


HWND hWndCmdReceiver = pPopup->GetCmdReceiverHWND();
	ASSERT( hWndCmdReceiver != NULL && ::IsWindow(hWndCmdReceiver) );
POPUPLISTBOXITEMSELENDINFO _plbsei( this );
	::SendMessage(
		hWndCmdReceiver,
		g_nMsgPopupListBoxSelChange,
		(WPARAM)(&_plbsei),
		(LPARAM)m_lParamCookie
		);
}

/////////////////////////////////////////////////////////////////////////////
// CExtPopupListBoxMenuWnd

IMPLEMENT_DYNCREATE(CExtPopupListBoxMenuWnd, CExtPopupControlMenuWnd)

BEGIN_MESSAGE_MAP(CExtPopupListBoxMenuWnd, CExtPopupControlMenuWnd)
	//{{AFX_MSG_MAP(CExtPopupListBoxMenuWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CExtPopupListBoxMenuWnd::CExtPopupListBoxMenuWnd(
	LPARAM lParamListCookie, // = 0L
	DWORD dwListBoxStyles // = WS_CHILD|WS_VISIBLE|WS_VSCROLL|LBS_NOINTEGRALHEIGHT|LBS_HASSTRINGS|LBS_OWNERDRAWVARIABLE
	)
	: m_lParamListCookie( lParamListCookie )
	, m_dwListBoxStyles( dwListBoxStyles )
	, m_pCbInitListBoxContent( NULL )
	, m_pInitListBoxCoockie( NULL )
	, m_pCbListBoxSelection( NULL )
	, m_pSelectionCoockie( NULL )
	, m_pCbListBoxItemDraw( NULL )
	, m_pCbListBoxItemMeasure( NULL )
	, m_pListBoxItemCoockie( NULL )
{
}

CExtPopupListBoxMenuWnd::~CExtPopupListBoxMenuWnd()
{
}

HWND CExtPopupListBoxMenuWnd::OnCreateChildControl(
	const RECT & rcChildControl
	)
{
	ASSERT_VALID( this );
CExtPopupInplaceListBox * pListBox = new CExtPopupInplaceListBox;
	pListBox->m_pCbListBoxSelection = m_pCbListBoxSelection;
	pListBox->m_pSelectionCoockie = m_pSelectionCoockie;
	pListBox->m_pCbInitListBoxContent = m_pCbInitListBoxContent;
	pListBox->m_pInitListBoxCoockie = m_pInitListBoxCoockie;
	pListBox->m_lParamCookie = m_lParamListCookie;
	pListBox->m_pCbListBoxItemDraw = m_pCbListBoxItemDraw;
	pListBox->m_pCbListBoxItemMeasure = m_pCbListBoxItemMeasure;
	pListBox->m_pListBoxItemCoockie = m_pListBoxItemCoockie;
	if(	!pListBox->Create(
			m_dwListBoxStyles,
			rcChildControl,
			this,
			(UINT)(IDC_STATIC)
			)
		)
	{
		ASSERT( FALSE );
		delete pListBox;
		return NULL;
	}
	return pListBox->m_hWnd;
}

/////////////////////////////////////////////////////////////////////////////
// CExtBarTextFieldButton::CInPlaceEditWnd

CExtBarTextFieldButton::CInPlaceEditWnd::CInPlaceEditWnd(
	CExtBarTextFieldButton * pTextFieldTBB,
	CExtSafeString * pStr,
	CExtBarTextFieldButton::pCbVerifyTextInput pCbVerify, // = NULL
	CExtBarTextFieldButton::pCbInplaceEditWndProc pCbWndProc, // = NULL
	LPVOID pCbCoockie // = NULL
	)
	: m_pTbbTextField( pTextFieldTBB )
	, m_pStr( pStr )
	, m_pCbVerifyTextInput( pCbVerify )
	, m_pCbWndProc( pCbWndProc )
	, m_pCbCoockie( pCbCoockie )
	, m_bCanceling( false )
{
	ASSERT_VALID( m_pTbbTextField );
	ASSERT( m_pStr != NULL );
	m_pBar = m_pTbbTextField->GetBar();
	ASSERT_VALID( m_pBar );
	m_pBtnRTC = m_pTbbTextField->GetRuntimeClass();
	g_pWndInplaceEditor = this;
}

CExtBarTextFieldButton::CInPlaceEditWnd::~CInPlaceEditWnd()
{
	if( g_pWndInplaceEditor == this )
		g_pWndInplaceEditor = NULL;
}

#ifdef _DEBUG
void CExtBarTextFieldButton::CInPlaceEditWnd::AssertValid() const
{
	CEdit::AssertValid();
	ASSERT_VALID( m_pTbbTextField );
	ASSERT( m_pStr != NULL );
}
#endif // _DEBUG

bool CExtBarTextFieldButton::CInPlaceEditWnd::Create(
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pTbbTextField );
CExtToolControlBar * pBar = m_pTbbTextField->GetBar();
	ASSERT_VALID( pBar );
	ASSERT( pBar->GetSafeHwnd() != NULL && ::IsWindow(pBar->GetSafeHwnd()) );
CRect rc = m_pTbbTextField->OnInplaceControlCalcRect( m_pTbbTextField->Rect() );
UINT nDlgCtrlID = m_pTbbTextField->GetCmdID( false );
	if( ! CEdit::Create(
			WS_CHILD|WS_VISIBLE|ES_LEFT|ES_AUTOHSCROLL,
			rc,
			pBar,
			nDlgCtrlID
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	SetFont( &g_PaintManager->m_FontNormal );
	SetWindowText( *m_pStr );
	SetSel( 0, -1 ); // SetSel( 0, 0 );
	SetFocus();
	return true;
}

bool CExtBarTextFieldButton::CInPlaceEditWnd::_IsValidState()
{
	ASSERT_VALID( m_pBar );
int nBtnIdx = m_pBar->_GetIndexOf( m_pTbbTextField );
	if( nBtnIdx < 0 )
		return false;
CExtBarButton * pTBB = m_pBar->GetButton( nBtnIdx );
	if( ! pTBB->IsKindOf( m_pBtnRTC ) )
		return false;
	return true;
}

BOOL CExtBarTextFieldButton::CInPlaceEditWnd::PreTranslateMessage( MSG * pMsg )
{
	ASSERT_VALID( m_pBar );
	if( ! _IsValidState() )
		return CEdit::PreTranslateMessage( pMsg );
	if( !m_bCanceling )
	{
		if( pMsg->message == WM_KEYDOWN )
		{
			bool bAlt =
				( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 )
					? true : false;
			if( !bAlt )
			{
				bool bCtrl =
					( (::GetAsyncKeyState(VK_CONTROL)&0x8000) != 0 )
						? true : false;
				bool bShift =
					( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 )
						? true : false;
				if(		bCtrl
					&&	(!bShift)
					&&	(	int(pMsg->wParam) == VK_INSERT
						||	int(pMsg->wParam) == int( _T('C') )
						)
					)
				{
					SendMessage( WM_COPY, 0, 0 );
					return TRUE;
				} 
				if(		( bCtrl && (!bShift) && int(pMsg->wParam) == int( _T('V') ) )
					||	( (!bCtrl) && bShift && int(pMsg->wParam) == VK_INSERT )
					)
				{
					SendMessage( WM_PASTE, 0, 0 );
					return TRUE;
				} 
				if(		( bCtrl && (!bShift) && int(pMsg->wParam) == int( _T('X') ) )
					||	( (!bCtrl) && bShift && int(pMsg->wParam) == VK_DELETE )
					)
				{
					SendMessage( WM_CUT, 0, 0 );
					return TRUE;
				} 
				if(	bCtrl && (!bShift) && int(pMsg->wParam) == int( _T('A') ) ) 
				{
					SetSel( 0, -1 );
					return TRUE;
				}
			} // if( !bAlt )
		} // if( pMsg->message == WM_KEYDOWN )
		else if(
				pMsg->message == WM_LBUTTONDOWN
			||	pMsg->message == WM_MBUTTONDOWN
			||	pMsg->message == WM_RBUTTONDOWN
			)
		{
			if( pMsg->hwnd != m_hWnd )
			{
				m_bCanceling = true;
				PostMessage( (WM_USER+0x666) );
			}
		}
	} // if( !m_bCanceling )
	
	// HASH Added START
	// (Allows single key accelerators)
	if(		(	pMsg->message == WM_KEYDOWN
			||	pMsg->message == WM_CHAR
			)
		&&	pMsg->hwnd == m_hWnd
		)
	{
		::TranslateMessage( pMsg );
		::DispatchMessage( pMsg );
		return TRUE;
	}
	// HASH Added END
	
	return CEdit::PreTranslateMessage( pMsg );
}

LRESULT CExtBarTextFieldButton::CInPlaceEditWnd::WindowProc(
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
{
	ASSERT_VALID( m_pBar );
	if( ! _IsValidState() )
		return CEdit::WindowProc( message, wParam, lParam );
	if( m_pCbWndProc != NULL )
	{
		LRESULT lResult = 0L;
		if( m_pCbWndProc(
				lResult,
				message,
				wParam,
				lParam,
				*this,
				m_pCbCoockie
				)
			)
			return lResult;
	} // if( m_pCbWndProc != NULL )
	if( !m_bCanceling )
	{
		if(		CExtPopupMenuWnd::IsMenuTracking()
			||	CExtControlBar::_DraggingGetBar() != NULL
			)
		{
			m_bCanceling = true;
			PostMessage( (WM_USER+0x666) );
		}
	}
	if( message == (WM_USER+0x666) )
	{
		GetBarTextFieldButton()->OnInplaceControlSessionCancel();
		return 0;
	}
	if( message == WM_NCCALCSIZE )
	{
		NCCALCSIZE_PARAMS * pNCCSP =
			reinterpret_cast < NCCALCSIZE_PARAMS * > ( lParam );
		ASSERT( pNCCSP != NULL );
		CRect rcInBarWnd( pNCCSP->rgrc[0] );
		rcInBarWnd.DeflateRect( 2, 2, 0, 2 );
		::CopyRect( &(pNCCSP->rgrc[0]), rcInBarWnd );
		return 0;
	} // if( message == WM_NCCALCSIZE )
	if( message == WM_NCPAINT )
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
			rcInBarWnd,
			g_PaintManager->GetColor( COLOR_WINDOW )
			);
		return 0;
	} // if( message == WM_NCPAINT )
	if( message == WM_GETDLGCODE )
		return DLGC_WANTALLKEYS|DLGC_WANTCHARS|DLGC_WANTTAB;
	if(		message == WM_RBUTTONDOWN
		||	message == WM_RBUTTONUP
		||	message == WM_RBUTTONDBLCLK
		||	message == WM_CONTEXTMENU
		)
		return 0;
	
	if( message == WM_KEYDOWN )
	{
		if(		int(wParam) == VK_MENU
			||	int(wParam) == VK_ESCAPE
			)
		{
			GetBarTextFieldButton()->OnInplaceControlSessionCancel();
			return 0;
		}
		if(		int(wParam) == VK_DOWN
			||	int(wParam) == VK_UP
			)
		{
			CExtBarTextFieldButton * pTextFieldTBB =
				GetBarTextFieldButton();
			if( pTextFieldTBB->IsComboTextField() )
			{
				pTextFieldTBB->OnInplaceControlSessionEnd();
				pTextFieldTBB->OnTrackPopup( CPoint(0,0), true );
				return 0;
			}
		}

		if( int(wParam) == VK_RETURN )
		{
			ASSERT( m_pStr != NULL );
			CExtSafeString sText;
			int nTextLength = GetWindowTextLength();
			if( nTextLength > 0 )
			{
				GetWindowText( sText.GetBuffer(nTextLength+2), nTextLength+1 );
				sText.ReleaseBuffer();
			}
			if( m_pCbVerifyTextInput != NULL )
			{
				if(	m_pCbVerifyTextInput(
						*this,
						m_pCbCoockie,
						sText.IsEmpty() ? _T("") : sText,
						sText.IsEmpty() ? _T("") : sText
						)
					)
					*m_pStr = sText;
			} // if( m_pCbVerifyTextInput != NULL )
			else
				*m_pStr = sText;
			GetBarTextFieldButton()->OnInplaceControlSessionEnd();
			return 0;
		}
		bool bAlt =
			( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 )
				? true : false;
		if( bAlt )
		{
			GetBarTextFieldButton()->OnInplaceControlSessionCancel();
			return 0;
		}

		ASSERT( m_pStr != NULL );
		CString sTextOld;
		GetWindowText( sTextOld );
		DWORD dwSelSaved = CEdit::GetSel();
		CEdit::SetRedraw( FALSE );
		LRESULT lResult = CEdit::WindowProc( message, wParam, lParam );
		CString sTextNew;
		GetWindowText( sTextNew );
		if( m_pCbVerifyTextInput != NULL )
		{
			if(	m_pCbVerifyTextInput(
					*this,
					m_pCbCoockie,
					sTextOld.IsEmpty() ? _T("") : (LPCTSTR)sTextOld,
					sTextNew.IsEmpty() ? _T("") : (LPCTSTR)sTextNew
					)
				)
				*m_pStr = sTextNew;
			else
			{
				CEdit::SetSel( 0, -1 );
				CEdit::ReplaceSel( sTextOld );
				CEdit::SetSel( dwSelSaved );
			}
		} // if( m_pCbVerifyTextInput != NULL )
		else
			*m_pStr = sTextNew;
		CEdit::SetRedraw( TRUE );
		Invalidate();
		UpdateWindow();
		
		return lResult;
	} // if( message == WM_KEYDOWN )
	else if( message == WM_CHAR )
	{
		ASSERT( m_pStr != NULL );
		CString sTextOld;
		GetWindowText( sTextOld );
		DWORD dwSelSaved = CEdit::GetSel();
		CEdit::SetRedraw( FALSE );
		LRESULT lResult = CEdit::WindowProc( message, wParam, lParam );
		CString sTextNew;
		GetWindowText( sTextNew );
		if( m_pCbVerifyTextInput != NULL )
		{
			if(	m_pCbVerifyTextInput(
					*this,
					m_pCbCoockie,
					sTextOld.IsEmpty() ? _T("") : (LPCTSTR)sTextOld,
					sTextNew.IsEmpty() ? _T("") : (LPCTSTR)sTextNew
					)
				)
				*m_pStr = sTextNew;
			else
			{
				CEdit::SetSel( 0, -1 );
				CEdit::ReplaceSel( sTextOld );
				CEdit::SetSel( dwSelSaved );
			}
		} // if( m_pCbVerifyTextInput != NULL )
		else
			*m_pStr = sTextNew;
		CEdit::SetRedraw( TRUE );
		Invalidate();
		UpdateWindow();

		return lResult;
	} // else if( message == WM_CHAR )
	else if( message == WM_KILLFOCUS || message == WM_CANCELMODE )
	{
		GetBarTextFieldButton()->OnInplaceControlSessionCancel();
		return 0;
	} // else if( message == WM_KILLFOCUS || message == WM_CANCELMODE )

	return CEdit::WindowProc( message, wParam, lParam );
}

void CExtBarTextFieldButton::CInPlaceEditWnd::PostNcDestroy()
{
	ASSERT_VALID( this );
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
// CExtBarTextFieldButton

IMPLEMENT_DYNCREATE(CExtBarTextFieldButton, CExtBarButton)

CExtBarTextFieldButton::CExtBarTextFieldButton(
	bool bComboField, // = false
	INT nTextFieldWidth, // = __EXT_MENU_DEF_INPLACE_EDIT_WIDTH
	CExtToolControlBar * pBar, // = NULL
	UINT nCmdID, // = ID_SEPARATOR
	UINT nStyle // = 0
	)
	: CExtBarButton( pBar, nCmdID, nStyle )
	, m_nTextFieldWidth( nTextFieldWidth )
	, m_bComboField( bComboField )
	, m_bComboPopupDropped( false )
	, m_lParamCookie( 0L )
	, m_dwListBoxStyles( WS_CHILD|WS_VISIBLE|WS_VSCROLL|LBS_NOINTEGRALHEIGHT|LBS_HASSTRINGS|LBS_OWNERDRAWVARIABLE )
	, m_bHelperFindListInitialItem( false )
	, m_nDropDownWidth( -2 ) // (-1) - auto calc, (-2) - same as button area
	, m_nDropDownHeightMax( 250 )
	, m_bTextFieldIsNotEditable( false )
{
	ASSERT( m_nTextFieldWidth >= 0 );
}

CExtBarTextFieldButton::~CExtBarTextFieldButton()
{
//	OnInplaceControlSessionCancel();
}

CExtBarTextFieldButton::CInPlaceEditWnd *
	CExtBarTextFieldButton::CInPlaceEditWnd::g_pWndInplaceEditor = NULL;

void CExtBarTextFieldButton::_CancelInplaceEditor()
{
	if( CExtBarTextFieldButton::CInPlaceEditWnd::g_pWndInplaceEditor->GetSafeHwnd() != NULL )
	{
		ASSERT_VALID( CExtBarTextFieldButton::CInPlaceEditWnd::g_pWndInplaceEditor );
		CExtBarTextFieldButton::CInPlaceEditWnd::g_pWndInplaceEditor->
			GetBarTextFieldButton()->OnInplaceControlSessionEnd();
		if( CExtBarTextFieldButton::CInPlaceEditWnd::g_pWndInplaceEditor->GetSafeHwnd() != NULL )
			CExtBarTextFieldButton::CInPlaceEditWnd::g_pWndInplaceEditor->DestroyWindow();
	} // if( CExtBarTextFieldButton::CInPlaceEditWnd::g_pWndInplaceEditor->GetSafeHwnd() != NULL )
}

INT CExtBarTextFieldButton::GetTextFieldWidth() const
{
	ASSERT_VALID( this );
	ASSERT( m_nTextFieldWidth > 0 );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = ((CExtBarTextFieldButton*)this)->GetCmdNode( false );
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		return pNode->TextFieldWidthGet();
	} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return m_nTextFieldWidth;
}

INT CExtBarTextFieldButton::SetTextFieldWidth( INT nTextFieldWidth )
{
	ASSERT_VALID( this );
	ASSERT( nTextFieldWidth > 0 );
	OnInplaceControlSessionCancel();
INT nTextFieldWidthOld = GetTextFieldWidth();
	m_nTextFieldWidth = nTextFieldWidth;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = GetCmdNode( false );
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		pNode->TextFieldWidthSet( nTextFieldWidth );
	} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return nTextFieldWidthOld;
}

__EXT_MFC_SAFE_LPCTSTR CExtBarTextFieldButton::GetFieldText() const
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if( pSite != NULL )
	{
		pSite->OnTextFieldInplaceTextGet(
			this,
			((CExtBarButton *)this)->GetCmdNode( false ),
			*( (CExtSafeString *)&m_sTextField ) // emulate C++ property mutability
			);
		CExtCustomizeCmdTreeNode * pNode =
			((CExtBarButton*)this)->GetCmdNode( false );
		if( pNode != NULL )
		{
			ASSERT_VALID( pNode );
			pNode->m_sDefInplaceEditBuffer = m_sTextField;
		} // m_sTextField
	} // if( pSite != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return m_sTextField;
}

bool CExtBarTextFieldButton::SetFieldText(
	__EXT_MFC_SAFE_LPCTSTR sText,
	bool bVerify // = true
	)
{
	ASSERT_VALID( this );
	if(		bVerify
		&&	(!	OnInplaceControlVerifyTextInput(
					NULL,
					( GetFieldText() == NULL ) ? _T("") : GetFieldText(),
					( sText == NULL ) ? _T("") : sText
				)
			)
		)
		return false;
LPCTSTR sNewText = ( sText == NULL ) ? _T("") : sText;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if( pSite != NULL )
	{
		CExtCustomizeCmdTreeNode * pNode = GetCmdNode( false );
		if( pNode != NULL )
		{
			ASSERT_VALID( pNode );
			pNode->m_sDefInplaceEditBuffer = m_sTextField;
		} // m_sTextField
		pSite->OnTextFieldInplaceTextSet(
			this,
			GetCmdNode( false ),
			m_sTextField,
			sNewText
			);
	}
	else
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
		m_sTextField = sNewText;
	return true;
}

CRect CExtBarTextFieldButton::OnCalcComboDropRect( const RECT & rcBtnArea ) const
{
	ASSERT_VALID( this );
	if( !IsComboTextField() )
		return CRect( 0, 0, 0, 0 );
CRect rcComboDropRect( rcBtnArea );
	rcComboDropRect.left = rcComboDropRect.right - 
			(INT) ::GetSystemMetrics(SM_CXHTHUMB);
	return rcComboDropRect;
}

CRect CExtBarTextFieldButton::OnInplaceControlCalcRect( const RECT & rcBtnArea ) const
{
	ASSERT_VALID( this );
CRect rcCtrl( rcBtnArea );
CRect rcControlMarginSizes = OnInplaceControlCalcMarginSizes();
	rcCtrl.DeflateRect(
		rcControlMarginSizes.left,
		rcControlMarginSizes.top,
		rcControlMarginSizes.right,
		rcControlMarginSizes.bottom
		);
	return rcCtrl;
}

CRect CExtBarTextFieldButton::OnInplaceControlCalcMarginSizes() const
{
	ASSERT_VALID( this );
CRect rcControlMarginSizes( 2, 2, 2, 2 );
	if( IsComboTextField() )
	{
		rcControlMarginSizes.right +=
			(INT) ::GetSystemMetrics(SM_CXHTHUMB);
	} // if( IsComboTextField() )
	return rcControlMarginSizes;
}

CWnd * CExtBarTextFieldButton::OnInplaceControlCreate()
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
CInPlaceEditWnd * pEdit =
		new CInPlaceEditWnd(
			this,
			&m_sTextInplaceBuffer,
			(CExtBarTextFieldButton::pCbVerifyTextInput)stat_CbVerifyTextInput,
			(CExtBarTextFieldButton::pCbInplaceEditWndProc)stat_CbInplaceEditWndProc,
			this
			);
	if( !pEdit->Create() )
	{
		ASSERT( FALSE );
		m_sTextInplaceBuffer.Empty();
		delete pEdit;
		return NULL;
	}
	return pEdit;
}

void CExtBarTextFieldButton::OnInplaceControlRun()
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
CExtToolControlBar * pBar = m_pBar;
CExtBarTextFieldButton * pThis = this;
	OnInplaceControlSessionCancel();
__EXT_MFC_SAFE_LPCTSTR sText = GetFieldText();
	m_sTextInplaceBuffer = (sText == NULL) ? _T("") : sText;
CWnd * pEdit = OnInplaceControlCreate();
	if( pEdit == NULL )
		return;
	ASSERT_VALID( pEdit );
	ASSERT(
			pEdit->GetSafeHwnd() != NULL
		&&	::IsWindow( pEdit->GetSafeHwnd() )
		);
	CtrlSet( pEdit, true );
	RedrawButton();
HWND hWndCtrl = pEdit->m_hWnd;
	for( MSG msg; ::IsWindow(hWndCtrl); )
	{
		// Process all the messages in the message queue
		while( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		{
			bool bStop = false;
			switch( msg.message )
			{
			//case WM_CONTEXTMENU:
			case WM_ACTIVATEAPP:
			case WM_CANCELMODE:
				bStop = true;
			break;
			case WM_NCLBUTTONDOWN:
			case WM_NCRBUTTONDOWN:
			case WM_NCMBUTTONDOWN:
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
				if( msg.hwnd != hWndCtrl )
					bStop = true;
			break;
			default:
				if( msg.hwnd != hWndCtrl )
				{
					if(		WM_KEYFIRST <= msg.message
						&&	msg.message <= WM_KEYLAST
						)
					{
						bStop = true;
						break;
					}
					if(		WM_MOUSEFIRST <= msg.message
						&&	msg.message <= WM_MOUSELAST
						)
					{
						PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
						continue;
					}
				} // if( msg.hwnd != hWndCtrl )
			} // switch( msg.message )
			if( bStop )
			{
				if( pBar->_GetIndexOf(pThis) < 0 )
					break;
				OnInplaceControlSessionCancel();
				break;
			} // if( bStop )
			if( !AfxGetThread()->PumpMessage() )
			{
				PostQuitMessage(0);
				break; // Signal WM_QUIT received
			} // if( !AfxGetThread()->PumpMessage() )
		} // while( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
	} // for( MSG msg; ::IsWindow(hWndCtrl); )
}

void CExtBarTextFieldButton::OnInplaceControlSessionEnd()
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
CExtToolControlBar * pBar = m_pBar;
CExtBarTextFieldButton * pThis = this;
	SetFieldText( m_sTextInplaceBuffer );
	if( pBar->_GetIndexOf(pThis) < 0 )
		return;
	CtrlSet( NULL, true );
	m_sTextInplaceBuffer.Empty();
	if(		m_pBar->m_hWnd != NULL
		&&	::IsWindow( m_pBar->m_hWnd )
		)
		RedrawButton();
	if( m_pBar->m_pDockSite->GetSafeHwnd() != NULL )
		m_pBar->m_pDockSite->SetFocus();
	else
		m_pBar->GetParent()->SetFocus();
}

void CExtBarTextFieldButton::OnInplaceControlSessionCancel()
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	CtrlSet( NULL, true );
	m_sTextInplaceBuffer.Empty();
	if(		m_pBar->m_hWnd != NULL
		&&	::IsWindow( m_pBar->m_hWnd )
		)
		RedrawButton();
	if( m_pBar->m_pDockSite->GetSafeHwnd() != NULL )
		m_pBar->m_pDockSite->SetFocus();
	else
		m_pBar->GetParent()->SetFocus();
}

bool CExtBarTextFieldButton::OnInplaceControlVerifyTextInput(
	CEdit * pEdit,
	__EXT_MFC_SAFE_LPCTSTR sTextOld,
	__EXT_MFC_SAFE_LPCTSTR sTextNew
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	ASSERT( sTextOld != NULL );
	ASSERT( sTextNew != NULL );
	pEdit;
	sTextOld;
	sTextNew;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if( pSite != NULL )
		return
			pSite->OnTextFieldVerify(
				this,
				GetCmdNode( false ),
				sTextOld,
				sTextNew
				);
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	return true;
}

void CExtBarTextFieldButton::OnInplaceControlPutTextInputResult(
	CEdit * pEdit,
	__EXT_MFC_SAFE_LPCTSTR sTextNew
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	ASSERT( sTextNew != NULL );
	pEdit;
	VERIFY( SetFieldText( sTextNew, false ) );
}

bool CExtBarTextFieldButton::OnInplaceControlWndProcCall(
	LRESULT & lResult,
	UINT message,
	WPARAM wParam,
	LPARAM lParam,
	CEdit & wndEdit
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if( pSite != NULL )
		return
			pSite->OnTextFieldWndProcHook(
				lResult,
				message,
				wParam,
				lParam,
				wndEdit,
				this,
				GetCmdNode( false )
				);
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	lResult;
	message;
	wParam;
	lParam;
	wndEdit;
	return false;
}

void CExtBarTextFieldButton::_UpdateCtrl()
{
	ASSERT_VALID( this );
//	CExtBarButton::_UpdateCtrl();
CWnd * pWndInplaceControl = CtrlGet();
	if( pWndInplaceControl == NULL )
		return;
	ASSERT_VALID( pWndInplaceControl );
CRect rcButton = Rect();
CRect rcCtrl = OnInplaceControlCalcRect( rcButton );
	pWndInplaceControl->SetWindowPos(
		NULL,
		rcCtrl.left, rcCtrl.top,
		rcCtrl.Width(), rcCtrl.Height(),
		SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOACTIVATE
			|	(	(	IsVisible()
					&&	(GetStyle() & TBBS_HIDDEN) == 0
					&&	(!m_bVertDocked || GetCtrlVisibleVertically())
					)
					? SWP_SHOWWINDOW
					: SWP_HIDEWINDOW
				)
		);
	pWndInplaceControl->EnableWindow( IsEnabled() );
	if( pWndInplaceControl->IsKindOf(RUNTIME_CLASS(CButton)) )
	{
		int nCheck = 0;
		if( IsPressed() )
			nCheck = 1;
		else if( IsIndeterminate() )
			nCheck = 2;
		((CButton *)(pWndInplaceControl))->SetCheck( nCheck );
	}
//	if( m_bVisible && m_bHover )
//		pWndInplaceControl->SetFocus();
}

CExtSafeString CExtBarTextFieldButton::GetText() const
{
	ASSERT_VALID( this );
	//return CExtSafeString( _T("") );
	return CExtBarButton::GetText();
}

void CExtBarTextFieldButton::SetMenu(
	HMENU hMenu,
	bool bPopupMenu,
	bool bAutoDestroyMenu
	)
{
	ASSERT_VALID( this );
	ASSERT( FALSE ); // should not be used with text-field button
	hMenu;
	bPopupMenu;
	bAutoDestroyMenu;
}

HMENU CExtBarTextFieldButton::GetMenu()
{
	ASSERT_VALID( this );
	return NULL;
}

//bool CExtBarTextFieldButton::IsLargeIcon() const
//{
//	ASSERT_VALID( this );
//	return false;
//}
//
//bool CExtBarTextFieldButton::IsDisplayScreenTip() const
//{
//	ASSERT_VALID( this );
//	return false;
//}
//
//bool CExtBarTextFieldButton::IsDisplayShortcutKeysOnScreenTip() const
//{
//	ASSERT_VALID( this );
//	return false;
//}

void CExtBarTextFieldButton::SetHover(
	bool bHover // = true
	)
{
	ASSERT_VALID( this );
	CExtBarButton::SetHover( bHover );
}

void CExtBarTextFieldButton::SetRect( const RECT & rectButton )
{
	ASSERT_VALID( this );
	m_ActiveRect = rectButton;
CWnd * pWndInplaceControl = CtrlGet();
	if( (pWndInplaceControl != NULL) && (!m_bVertDocked || GetCtrlVisibleVertically()) )
	{
		ASSERT_VALID( pWndInplaceControl );
		CRect rcCtrl = OnInplaceControlCalcRect( rectButton );
		pWndInplaceControl->MoveWindow( &rcCtrl, FALSE );
	} // if( (pWndInplaceControl != NULL) && (!m_bVertDocked || GetCtrlVisibleVertically()) )
	_UpdateCtrl();
}

bool CExtBarTextFieldButton::IsAbleToTrackMenu(
	bool bCustomizeMode // = false
	)
{
	ASSERT_VALID( this );
	bCustomizeMode;
	return false;
}

bool CExtBarTextFieldButton::IsContainerOfPopupLikeMenu()
{
	ASSERT_VALID( this );
	return false;
}

//CExtCmdIcon * CExtBarTextFieldButton::GetIconPtr()
//{
//	ASSERT_VALID( this );
//	return NULL; // no icon at all for text-fields
//}

CSize CExtBarTextFieldButton::CalculateLayout(
	CDC & dc,
	CSize sizePreCalc,
	BOOL bHorz
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	ASSERT_VALID( (&dc) );

	if(		(! bHorz )
		&&	(! GetCtrlVisibleVertically() )
		)
		return CExtBarButton::CalculateLayout( dc, sizePreCalc, bHorz );

static CExtSafeString g_sTestText( _T("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789;[]{}\\/=+-_*&^%$#@!~") );
CRect rcTestText =
		CExtPaintManager::stat_CalcTextDimension(
			dc,
			g_PaintManager->m_FontNormal,
			g_sTestText
			);
	m_ActiveSize.cx = GetTextFieldWidth();
	m_ActiveSize.cy = rcTestText.Height() + 4;
CRect rcControlMarginSizes = OnInplaceControlCalcMarginSizes();
	m_ActiveSize.cx +=
		rcControlMarginSizes.left + rcControlMarginSizes.right;
	m_ActiveSize.cy +=
		rcControlMarginSizes.top + rcControlMarginSizes.bottom;
	if( IsComboTextField() )
	{
		m_ActiveSize.cx -=
			(INT) ::GetSystemMetrics(SM_CXHTHUMB);
	} // if( IsComboTextField() )
	return m_ActiveSize;
}

BOOL CExtBarTextFieldButton::PutToPopupMenu(
	CExtPopupMenuWnd * pPopup
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );

	OnInplaceControlSessionCancel();
	if( IsComboTextField() )
	{
		CExtPopupMenuWnd * pSub = OnCreateDropPopup( true );
		if( pSub == NULL )
		{
			ASSERT( FALSE );
			return FALSE;
		}
		ASSERT_VALID( pSub );
		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
				g_CmdManager->ProfileNameFromWnd( m_pBar->GetSafeHwnd() ),
				GetCmdID( false )
				);
		ASSERT( pCmdItem != NULL );
		CExtSafeString sText = pCmdItem->m_sMenuText;
		if( sText.IsEmpty() )
			sText = pCmdItem->m_sToolbarText;
		HICON hIcon =
			g_CmdManager->CmdGetHICON(
				g_CmdManager->ProfileNameFromWnd( m_pBar->GetSafeHwnd() ),
				GetCmdID( false )
				);
		if( !pPopup->ItemInsertSpecPopup( pSub, -1, sText, hIcon ) )
		{
			ASSERT( FALSE );
			return FALSE;
		}
	} // if( IsComboTextField() )
	else
	{
		if( !pPopup->ItemInsert( GetCmdID(false) ) )
		{
			ASSERT( FALSE );
			return FALSE;
		}
	} // else from if( IsComboTextField() )
CExtPopupMenuWnd::MENUITEMDATA & mi =
		pPopup->ItemGetInfo( pPopup->ItemGetCount() - 1 );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if( pSite != NULL )
		pSite->OnTextFieldInplaceTextGet(
			this,
			GetCmdNode( false ),
			m_sTextField
			);
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
int nInplaceEditWidth = m_nTextFieldWidth;
	mi.SetInplaceEdit(
		&m_sTextField,
		(CExtPopupMenuWnd::pCbVerifyTextInput)stat_CbVerifyTextInput,
		(CExtPopupMenuWnd::pCbPutTextInputResult)stat_CbPutTextInputResult,
		(CExtPopupMenuWnd::pCbInplaceEditWndProc)stat_CbInplaceEditWndProc,
		this,
		nInplaceEditWidth
		);
bool bEnabled = IsDisabled() ? false : true;
	mi.Enable( bEnabled );
	mi.AllowInplaceEditActivation( (!m_bTextFieldIsNotEditable) && bEnabled );

	mi.MeasureItem();
	pPopup->_SyncItems();
	return TRUE;
}

bool CExtBarTextFieldButton::stat_CbVerifyTextInput(
	CInPlaceEditWnd & edit,
	CExtBarTextFieldButton * pTextFieldTBB,
	__EXT_MFC_SAFE_LPCTSTR sTextOld,
	__EXT_MFC_SAFE_LPCTSTR sTextNew
	)
{
	ASSERT_VALID( pTextFieldTBB );
	ASSERT_KINDOF( CExtBarTextFieldButton, pTextFieldTBB );
	return
		pTextFieldTBB->OnInplaceControlVerifyTextInput(
			&edit,
			sTextOld,
			sTextNew
			);
}

void CExtBarTextFieldButton::stat_CbPutTextInputResult(
	CInPlaceEditWnd & edit,
	CExtBarTextFieldButton * pTextFieldTBB,
	__EXT_MFC_SAFE_LPCTSTR sTextNew
	)
{
	ASSERT_VALID( pTextFieldTBB );
	ASSERT_KINDOF( CExtBarTextFieldButton, pTextFieldTBB );
	ASSERT( sTextNew != NULL );
	pTextFieldTBB->OnInplaceControlPutTextInputResult(
		&edit,
		sTextNew
		);
}

bool CExtBarTextFieldButton::stat_CbInplaceEditWndProc(
	LRESULT & lResult,
	UINT message,
	WPARAM wParam,
	LPARAM lParam,
	CEdit & wndEdit,
	CExtBarTextFieldButton * pTextFieldTBB
	)
{
	ASSERT_VALID( pTextFieldTBB );
	ASSERT_KINDOF( CExtBarTextFieldButton, pTextFieldTBB );
	return
		pTextFieldTBB->OnInplaceControlWndProcCall(
			lResult,
			message,
			wParam,
			lParam,
			wndEdit
			);
}

bool CExtBarTextFieldButton::stat_CbInitListBoxContent(
	CExtPopupInplaceListBox & wndListBox,
	CExtBarTextFieldButton * pTextFieldTBB
	)
{
	ASSERT_VALID( pTextFieldTBB );
	ASSERT_KINDOF( CExtBarTextFieldButton, pTextFieldTBB );
	return
		pTextFieldTBB->OnPopupListBoxInitContent(
			wndListBox
			);
}

bool CExtBarTextFieldButton::stat_CbListBoxSelection(
	CExtPopupInplaceListBox & wndListBox,
	CExtBarTextFieldButton * pTextFieldTBB,
	int eSAT // CExtPopupInplaceListBox::e_sel_action_t
	)
{
	ASSERT_VALID( pTextFieldTBB );
	ASSERT_KINDOF( CExtBarTextFieldButton, pTextFieldTBB );
	switch( ((CExtPopupInplaceListBox::e_sel_action_t)eSAT) )
	{
	case CExtPopupInplaceListBox::__SAT_SELCHANGE:
		return
			pTextFieldTBB->OnPopupListBoxSelChange(
				wndListBox
				);
	case CExtPopupInplaceListBox::__SAT_SELENDOK:
		return
			pTextFieldTBB->OnPopupListBoxSelEndOK(
				wndListBox
				);
	case CExtPopupInplaceListBox::__SAT_SELENDCANCEL:
		return
			pTextFieldTBB->OnPopupListBoxSelEndCancel(
				wndListBox
				);
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( ((CExtPopupInplaceListBox::e_sel_action_t)eSAT) )
	return false;
}

bool CExtBarTextFieldButton::stat_CbListBoxItemDraw(
	CExtPopupInplaceListBox & wndListBox,
	CExtBarTextFieldButton * pTextFieldTBB,
	LPDRAWITEMSTRUCT pDIS
	)
{
	ASSERT_VALID( pTextFieldTBB );
	ASSERT_KINDOF( CExtBarTextFieldButton, pTextFieldTBB );
	ASSERT( pDIS != NULL && pDIS->hDC != NULL );
	return
		pTextFieldTBB->OnPopupListBoxItemDraw(
			wndListBox,
			pDIS
			);
}

bool CExtBarTextFieldButton::stat_CbListBoxItemMeasure(
	CExtPopupInplaceListBox & wndListBox,
	CExtBarTextFieldButton * pTextFieldTBB,
	LPMEASUREITEMSTRUCT pMIS
	)
{
	ASSERT_VALID( pTextFieldTBB );
	ASSERT_KINDOF( CExtBarTextFieldButton, pTextFieldTBB );
	ASSERT( pMIS != NULL );
	return
		pTextFieldTBB->OnPopupListBoxItemMeasure(
			wndListBox,
			pMIS
			);
}

UINT CExtBarTextFieldButton::OnTrackPopup(
	CPoint point,
	bool bSelectAny
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	point;
bool bDockSiteCustomizeMode =
		m_pBar->_IsDockSiteCustomizeMode();
	if( bDockSiteCustomizeMode )
		return UINT(-1L);
	if( !IsEnabled() )
		return UINT(-1L);
	if( !IsComboTextField() )
		return UINT(-1L);
	if(		m_bVertDocked
		&&	(! GetCtrlVisibleVertically() )
		)
		return UINT(-1L);
	if( IsComboPopupDropped() )
		return UINT(-1L);
	CExtToolControlBar::_CloseTrackingMenus();
	OnInplaceControlSessionCancel();
	if( m_pBar->IsFloating() )
	{
		m_pBar->ActivateTopParent();
		CFrameWnd * pFrame =
			m_pBar->GetDockingFrame();
		ASSERT_VALID( pFrame );
		pFrame->BringWindowToTop();
	}
//CWnd * pWndCmdTarget = GetCmdTargetWnd();
//	ASSERT_VALID( pWndCmdTarget );

CExtPopupMenuWnd * pPopup = OnCreateDropPopup( false );
	if( pPopup == NULL )
	{
		ASSERT( FALSE );
		return UINT(-1L);
	}
	ASSERT_VALID( pPopup );
CRect rcButton = OnInplaceControlCalcRect( Rect() );
int nInflateMetric = pPopup->OnQueryMenuBorderSize() / 2;
	rcButton.InflateRect( nInflateMetric, nInflateMetric );
	rcButton.right +=
		::GetSystemMetrics( SM_CXHTHUMB )
		- nInflateMetric;
DWORD dwTrackFlags =
		OnGetTrackPopupFlags()
		| TPMX_COMBINE_NONE
		| TPMX_OWNERDRAW_FIXED
		| TPMX_DO_MESSAGE_LOOP
		;
	m_pBar->ClientToScreen( &rcButton );
	m_pBar->ClientToScreen( &point );
	ModifyStyle( TBBS_PRESSED, 0 );
	m_bComboPopupDropped = true;
	m_sTextInplaceBuffer = GetFieldText();
	RedrawButton( true );
	m_bHelperFindListInitialItem = bSelectAny;

CExtToolControlBar * pBar = m_pBar;
HWND hWndBar = pBar->m_hWnd;
	ASSERT( hWndBar != NULL && ::IsWindow(hWndBar) );
CExtBarTextFieldButton * pThis = this;
	if( !pPopup->TrackPopupMenu(
			dwTrackFlags,
			point.x,point.y,
			&rcButton,
			NULL, //m_pBar
			NULL, //CExtToolControlBar::_CbPaintCombinedContent
			NULL,
			false // true
			)
		)
	{
		delete pPopup;
//		CExtToolControlBar::_CloseTrackingMenus();
//		return UINT(-1L);
	}
	if(		(! ::IsWindow(hWndBar) )
		||	CWnd::FromHandlePermanent(hWndBar) != pBar
		||	pBar->_GetIndexOf( pThis ) < 0
		||	(! pThis->IsKindOf(RUNTIME_CLASS(CExtBarTextFieldButton)) )
		)
		return UINT(-1L);

	ModifyStyle( 0, TBBS_PRESSED );
	m_bComboPopupDropped = false;
	RedrawButton( false );

//	CExtToolControlBar::g_bMenuTracking = true;
//	m_pBar->_SwitchMenuTrackingIndex(
//		m_pBar->_GetIndexOf( this )
//		);
	return UINT(-1L);
}

CExtPopupMenuWnd * CExtBarTextFieldButton::OnCreateDropPopup(
	bool bContentExpand // true - inserting to content expand menu, false - tracking dropped popup
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	bContentExpand;

CWnd * pWndCmdTarget = GetCmdTargetWnd();
	ASSERT_VALID( pWndCmdTarget );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if( pSite != NULL )
	{
		CExtPopupControlMenuWnd * pExtCrtPopup =
			pSite->OnTextFieldCreateDropPopup(
				this,
				GetCmdNode( false ),
				pWndCmdTarget->m_hWnd,
				bContentExpand
				);
		if( pExtCrtPopup != NULL )
			return pExtCrtPopup;
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

CExtPopupListBoxMenuWnd * pPopup = new CExtPopupListBoxMenuWnd;
	
	pPopup->m_lParamListCookie = m_lParamCookie;

	pPopup->m_pCbListBoxSelection = 
		(CExtPopupInplaceListBox::pCbListBoxSelection)
			stat_CbListBoxSelection;
	pPopup->m_pSelectionCoockie = (LPVOID)this;
	
	pPopup->m_pCbInitListBoxContent =
		(CExtPopupInplaceListBox::pCbInitListBoxContent)
			stat_CbInitListBoxContent;
	pPopup->m_pInitListBoxCoockie = (LPVOID)this;

	pPopup->m_pCbListBoxItemDraw =
		(CExtPopupInplaceListBox::pCbListBoxItemDraw)
			stat_CbListBoxItemDraw;
	pPopup->m_pCbListBoxItemMeasure =
		(CExtPopupInplaceListBox::pCbListBoxItemMeasure)
			stat_CbListBoxItemMeasure;
	pPopup->m_pListBoxItemCoockie = (LPVOID)this;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if(		pSite == NULL
		||	(!	pSite->OnPopupListBoxGetStyles(
					this,
					GetCmdNode( false ),
					pPopup->m_dwListBoxStyles
					)
			)
		)
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
		pPopup->m_dwListBoxStyles = m_dwListBoxStyles;

CSize sizeMeasure = OnPopupListBoxMeasureTrackSize();
	pPopup->m_sizeChildControl = sizeMeasure;

	VERIFY( pPopup->CreatePopupMenu( pWndCmdTarget->m_hWnd ) );
	return pPopup;
}

CSize CExtBarTextFieldButton::OnPopupListBoxMeasureTrackSize() const
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if( pSite != NULL )
	{
		CSize _size =
			pSite->OnPopupListBoxMeasureTrackSize(
				this,
				((CExtBarButton*)this)->GetCmdNode( false )
				);
		if( _size.cx > 0 && _size.cy > 0 )
			return _size;
	} // if( pSite != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

int nCount = m_arrLbItems.GetSize();
	if(		(m_dwListBoxStyles & LBS_HASSTRINGS) == 0
		||	nCount == 0
		)
	{
		CSize sizeCalc( 150, 100 );
		if( m_nDropDownWidth > 0 )
			sizeCalc.cx = m_nDropDownWidth;
		if( m_nDropDownHeightMax > 0 )
			sizeCalc.cy = m_nDropDownHeightMax;
//		if( m_bComboField )
//			sizeCalc.cx +=
//				//::GetSystemMetrics( SM_CXSIZEFRAME )
//				+ ::GetSystemMetrics( SM_CXHTHUMB )
//				;
		return sizeCalc;
	}

CWindowDC dc( NULL );
CSize sizeCalc( 0, 0 );
	for( int i = 0; i < nCount; i++ )
	{
		const CExtSafeString & sItem =
			((CExtSafeStringArray*)&m_arrLbItems)->ElementAt( i );
		CExtSafeString sMeasure(
			sItem.IsEmpty() ? _T("AaWwPpQqRrWwZz") : sItem
			);
		CRect rcMeasure =
			CExtPaintManager::stat_CalcTextDimension(
				dc,
				g_PaintManager->m_FontNormal,
				sMeasure
				);
		if( m_dwListBoxStyles & (LBS_OWNERDRAWVARIABLE|LBS_OWNERDRAWFIXED) )
			rcMeasure.InflateRect( OnPopupListBoxCalcItemExtraSizes() );
		CSize sizeMeasure = rcMeasure.Size();
		if( m_nDropDownWidth == -1 )
			sizeCalc.cx = max( sizeCalc.cx, sizeMeasure.cx );
		sizeCalc.cy += sizeMeasure.cy;
	} // for( int i = 0; i < nCount; i++ )

	if( m_nDropDownWidth == -1 )
		sizeCalc.cx += ::GetSystemMetrics( SM_CXSIZEFRAME )*2;
	sizeCalc.cy += 2;
	ASSERT( m_nDropDownHeightMax > 0 );
	if( sizeCalc.cy > m_nDropDownHeightMax )
	{
		sizeCalc.cy = m_nDropDownHeightMax;
		if( m_nDropDownWidth == -1 )
			sizeCalc.cx +=
				::GetSystemMetrics( SM_CXSIZEFRAME )
				+ ::GetSystemMetrics( SM_CXHTHUMB )
				;
	} // if( sizeCalc.cy > m_nDropDownHeightMax )
	else if( m_dwListBoxStyles & LBS_DISABLENOSCROLL )
	{
		if( m_nDropDownWidth == -1 )
			sizeCalc.cx +=
				::GetSystemMetrics( SM_CXSIZEFRAME )
				+ ::GetSystemMetrics( SM_CXHTHUMB )
				;
	} // else if( m_dwListBoxStyles & LBS_DISABLENOSCROLL )

	if( m_nDropDownWidth == -2 )
	{
//		CRect rcButton = Rect();
//		CRect rcTextField = OnInplaceControlCalcRect( rcButton );
//		sizeCalc.cx = rcTextField.Width();
		sizeCalc.cx = m_nTextFieldWidth;
		sizeCalc.cx += ::GetSystemMetrics( SM_CXHTHUMB );
	}
	else if( m_nDropDownWidth > 0 )
		sizeCalc.cx = m_nDropDownWidth;

	return sizeCalc;
}

bool CExtBarTextFieldButton::OnPopupListBoxInitContent(
	CExtPopupInplaceListBox & wndListBox
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if(		pSite != NULL
		&&	pSite->OnPopupListBoxInitContent(
				this,
				GetCmdNode( false ),
				wndListBox
				)
		)
		return true;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if( (wndListBox.GetStyle() & LBS_HASSTRINGS) != 0 )
	{
		int nCount = m_arrLbItems.GetSize();
		for( int i = 0; i < nCount; i++ )
		{
			const CExtSafeString & sItem = m_arrLbItems.ElementAt( i );
			wndListBox.AddString(
				sItem.IsEmpty() ? _T("") : sItem
				);
		} // for( int i = 0; i < nCount; i++ )
		if(		m_bHelperFindListInitialItem
			&&	wndListBox.GetCount() > 0
			)
		{
			__EXT_MFC_SAFE_LPCTSTR sText = GetFieldText();
			if(		sText != NULL
				&&	_tcslen( sText ) > 0
				)
				wndListBox.SelectString( -1, sText );
		} // if( m_bHelperFindListInitialItem ...
	} // if( (wndListBox.GetStyle() & LBS_HASSTRINGS) != 0 )

	return true;
}

bool CExtBarTextFieldButton::OnPopupListBoxSelChange(
	CExtPopupInplaceListBox & wndListBox
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if(		pSite != NULL
		&&	pSite->OnPopupListBoxSelChange(
				this,
				GetCmdNode( false ),
				wndListBox
				)
		)
		return true;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if(		(wndListBox.GetStyle() & LBS_HASSTRINGS) != 0
		&&	wndListBox.GetCount() > 0
		)
	{
		int nCurSel = wndListBox.GetCurSel();
		if( nCurSel >= 0 )
		{
			wndListBox.GetText( nCurSel, *((CString*)&m_sTextInplaceBuffer) );
			RedrawButton( true );
		}
	}
	return false;
}

bool CExtBarTextFieldButton::OnPopupListBoxSelEndOK(
	CExtPopupInplaceListBox & wndListBox
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
HWND hWndThis = wndListBox.m_hWnd;
	ASSERT( hWndThis != NULL && ::IsWindow(hWndThis) );
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if(		pSite != NULL
		&&	pSite->OnPopupListBoxSelEndOK(
				this,
				GetCmdNode( false ),
				wndListBox
				)
		)
		return true;
	if( ! ::IsWindow( hWndThis ) )
		return false;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if(		(wndListBox.GetStyle() & LBS_HASSTRINGS) != 0
		&&	wndListBox.GetCount() > 0
		)
	{
		int nCurSel = wndListBox.GetCurSel();
		if( nCurSel >= 0 )
		{
			wndListBox.GetText( nCurSel, *((CString*)&m_sTextInplaceBuffer) );
			SetFieldText( m_sTextInplaceBuffer );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
			if( ! ::IsWindow( hWndThis ) )
				return false;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
			RedrawButton( false );
		}
	}
	return false;
}

bool CExtBarTextFieldButton::OnPopupListBoxSelEndCancel(
	CExtPopupInplaceListBox & wndListBox
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	wndListBox;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if(		pSite != NULL
		&&	pSite->OnPopupListBoxSelEndCancel(
				this,
				GetCmdNode( false ),
				wndListBox
				)
		)
		return true;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	RedrawButton( false );
	return false;
}

bool CExtBarTextFieldButton::OnPopupListBoxItemDraw(
	CExtPopupInplaceListBox & wndListBox,
	LPDRAWITEMSTRUCT pDIS
	)
{
	ASSERT_VALID( this );
	ASSERT( pDIS != NULL && pDIS->hDC != NULL );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if(		pSite != NULL
		&&	pSite->OnPopupListBoxItemDraw(
				this,
				GetCmdNode( false ),
				wndListBox,
				pDIS
				)
		)
		return true;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if( (wndListBox.GetStyle() & LBS_HASSTRINGS) == 0 )
		return false;
	if( ((INT)pDIS->itemID) < 0 || ((INT)pDIS->itemID) >= wndListBox.GetCount() )
		return true;
CDC dc;
	dc.Attach( pDIS->hDC );

	dc.FillSolidRect(
		&pDIS->rcItem,
		g_PaintManager->GetColor(
			( pDIS->itemState & ODS_SELECTED )
				? COLOR_HIGHLIGHT
				: COLOR_WINDOW
			)
		);

CString sText;
	wndListBox.GetText( (INT)pDIS->itemID, sText );
	if( !sText.IsEmpty() )
	{
		COLORREF clrTextOld =
			dc.SetTextColor(
				g_PaintManager->GetColor(
					( pDIS->itemState & ODS_SELECTED )
						? COLOR_HIGHLIGHTTEXT
						: COLOR_BTNTEXT
					)
				);
		int nOldBkMode = dc.SetBkMode( TRANSPARENT );
		CRect rcText( pDIS->rcItem );
		rcText.DeflateRect( OnPopupListBoxCalcItemExtraSizes() );
		dc.DrawText(
			sText,
			&rcText,
			DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_NOPREFIX
			);
		dc.SetBkMode( nOldBkMode );
		dc.SetTextColor( clrTextOld );
	} // if( !sText.IsEmpty() )

	if( pDIS->itemState & ODS_SELECTED )
		dc.DrawFocusRect( &pDIS->rcItem );

	dc.Detach();
	return true;
}

bool CExtBarTextFieldButton::OnPopupListBoxItemMeasure(
	CExtPopupInplaceListBox & wndListBox,
	LPMEASUREITEMSTRUCT pMIS
	)
{
	ASSERT_VALID( this );
	ASSERT( pMIS != NULL );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if(		pSite != NULL
		&&	pSite->OnPopupListBoxItemMeasure(
				this,
				GetCmdNode( false ),
				wndListBox,
				pMIS
				)
		)
		return true;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if( (wndListBox.GetStyle() & LBS_HASSTRINGS) == 0 )
		return false;
	pMIS->itemWidth = pMIS->itemHeight = 10;
	if( ((INT)pMIS->itemID) < 0 || ((INT)pMIS->itemID) >= wndListBox.GetCount() )
		return true;
CExtSafeString sMeasure;
	wndListBox.GetText( (INT)pMIS->itemID, *((CString*)&sMeasure) );
	if( sMeasure.IsEmpty() )
		return true;
CWindowDC dc( NULL );
CRect rcMeasure =
		CExtPaintManager::stat_CalcTextDimension(
			dc,
			g_PaintManager->m_FontNormal,
			sMeasure
			);
	rcMeasure.InflateRect( OnPopupListBoxCalcItemExtraSizes() );
	pMIS->itemWidth = rcMeasure.Width();
	pMIS->itemHeight = rcMeasure.Height();
	return true;
}

CSize CExtBarTextFieldButton::OnPopupListBoxCalcItemExtraSizes() const
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if( pSite != NULL )
	{
		CSize _size =
			pSite->OnPopupListBoxCalcItemExtraSizes(
				this,
				((CExtBarButton*)this)->GetCmdNode( false )
				);
		if( _size.cx > 0 && _size.cy > 0 ) 
			return _size;
	} // if( pSite != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	return CSize( 2, 1 );
}

void CExtBarTextFieldButton::OnHover(
	CPoint point,
	bool bOn
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );

bool bDockSiteCustomizeMode =
		m_pBar->_IsDockSiteCustomizeMode();
	if( bDockSiteCustomizeMode )
		return;

	if( bOn )
	{
		if( CExtToolControlBar::g_bMenuTracking )
			OnTrackPopup( point, false );
		else
		{
			CExtControlBar::stat_SetMessageString(
				GetCmdTargetWnd(),
				(UINT)(
					(		( ! m_pBar->IsKindOf(RUNTIME_CLASS(CExtMenuControlBar)) )
						&&	( ! IsKindOf(RUNTIME_CLASS(CExtBarContentExpandButton)) )
						&&	CExtCmdManager::IsCommand( GetCmdID(false) )
					)
						? GetCmdID(true)
						: AFX_IDS_IDLEMESSAGE
					)
				);
		}
		CWnd * pCtrl = CtrlGet();
		if( pCtrl == NULL
			|| (pCtrl->GetStyle() & WS_VISIBLE) == 0
			)
			((CExtMouseCaptureSink *)m_pBar)->SetCapture( m_pBar->GetSafeHwnd() );
	} // if( bOn )
	else
	{
		CExtControlBar::stat_SetMessageString( GetCmdTargetWnd() );
		CExtMouseCaptureSink::ReleaseCapture();
	} // else from if( bOn )
}

void CExtBarTextFieldButton::OnClick(
	CPoint point,
	bool bDown
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );

bool bDockSiteCustomizeMode =
		m_pBar->_IsDockSiteCustomizeMode();
	if( bDockSiteCustomizeMode )
		return;

	if(		m_bVertDocked
		&&	(! GetCtrlVisibleVertically() )
		)
	{
		CExtBarButton::OnClick( point, bDown );
		return;
	}

	if( bDown )
	{
		CExtToolControlBar::_CloseTrackingMenus();

		CExtControlBar::stat_SetMessageString(
			GetCmdTargetWnd(),
			(WPARAM)GetCmdID(false)
			);

		return;
	} // if( bDown )

	CExtControlBar::stat_SetMessageString( GetCmdTargetWnd() );

CRect rcButton = Rect();
	if( !m_bTextFieldIsNotEditable )
	{
		CRect rcTextField = OnInplaceControlCalcRect( rcButton );
		if( rcTextField.PtInRect(point) )
		{
			OnInplaceControlRun();
			return;
		}
	} // if( !m_bTextFieldIsNotEditable )
	if( !IsComboTextField() )
		return;
bool bTrackComboPopup = m_bTextFieldIsNotEditable;
	if( !bTrackComboPopup )
	{
		CRect rcComboDropRect = OnCalcComboDropRect( rcButton );
		if( rcComboDropRect.PtInRect(point) )
			bTrackComboPopup = true;
	}
	if( bTrackComboPopup )
		OnTrackPopup( point, false );
}

void CExtBarTextFieldButton::OnDeliverCmd()
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	CExtBarButton::OnDeliverCmd();
}

int CExtBarTextFieldButton::OnToolHitTest(
	CPoint point,
	TOOLINFO * pTI
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	return CExtBarButton::OnToolHitTest( point, pTI );
}

LRESULT CExtBarTextFieldButton::OnHelpHitTest(
	CPoint point
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	return CExtBarButton::OnHelpHitTest( point );
}

void CExtBarTextFieldButton::OnUpdateCmdUI(
	CWnd * pTarget,
	BOOL bDisableIfNoHndler,
	int nIndex
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	CExtBarButton::OnUpdateCmdUI(
		pTarget,
		bDisableIfNoHndler,
		nIndex
		);
}

bool CExtBarTextFieldButton::OnSetCursor( CPoint point )
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	if( m_pBar->_IsDockSiteCustomizeMode() )
		return CExtBarButton::OnSetCursor( point );
	if(		IsDisabled()
		||	m_bTextFieldIsNotEditable
		)
		return false;
	if(		m_bVertDocked
		&&	(! GetCtrlVisibleVertically() )
		)
		return false;
CRect rcButton = Rect();
CRect rcTextField = OnInplaceControlCalcRect( rcButton );
	if( !rcTextField.PtInRect(point) )
		return false;
	::SetCursor( ::LoadCursor(NULL,IDC_IBEAM) );
	return true;
}

void CExtBarTextFieldButton::Paint(
	CDC & dc,
	bool bHorz
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	if(		(! bHorz )
		&&	(! GetCtrlVisibleVertically() )
		)
	{
		CExtBarButton::Paint( dc, bHorz );
		return;
	}
CRect rcButton = Rect();
CRect rcTextField = OnInplaceControlCalcRect( rcButton );
bool bDockSiteCustomizeMode =
		m_pBar->_IsDockSiteCustomizeMode();
bool bPushed =
		(	(!bDockSiteCustomizeMode)
		&&	(	IsComboPopupDropped()
			||	(		CtrlGet() != NULL
				&&	( ! IsComboTextField() )
				)
			)
		)
			? true : false;
bool bEnabled =
		( IsDisabled() && (!bDockSiteCustomizeMode) )
			? false : true;
bool bHover =
		(	(!bDockSiteCustomizeMode)
		&&	(	IsHover()
			||	CtrlGet() != NULL
			||	IsComboPopupDropped()
			)
		)
			? true : false;
	if(		(! bEnabled )
		||	(	CExtPopupMenuWnd::IsMenuTracking()
			&&	(! IsComboPopupDropped() )
			)
		)
		bPushed = bHover = false;

CExtSafeString sText;
	if( !IsComboPopupDropped() )
		sText = (GetFieldText() == NULL) ? _T("") : GetFieldText();
	else
		sText = m_sTextInplaceBuffer;

CExtPaintManager::PAINTTOOLBARTEXTFIELDDATA _ptbtfd(
		this,
		sText,
		rcButton,
		rcTextField,
		IsComboTextField(),
		bHover,
		bPushed,
		bEnabled
		);
	g_PaintManager->PaintToolbarTextField( dc, _ptbtfd );
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
int CExtBarTextFieldButton::GetInitialResizingStateH( // -1 - left side resizing, 1 - right side resizing, 0 - no resizing at specified point (in bar's client coord)
	CPoint point,
	int * p_nWidthMin, // = NULL
	int * p_nWidthMax // = NULL
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
CRect rcButton = Rect();
	if( !rcButton.PtInRect(point) )
		return 0;
CExtCustomizeSite * pSite = m_pBar->GetCustomizeSite();
	if( pSite == NULL )
		return 0;
CExtCustomizeCmdTreeNode * pNode = GetCmdNode( false );
	if( pNode == NULL )
		return 0;
	ASSERT_VALID( pNode );
	if( (pNode->GetFlags() & __ECTN_TBB_RESIZABLE) == 0 )
		return 0;
int nDdWidth = g_PaintManager->GetDropDividerMerics().cx / 2;
	if( nDdWidth < 2 )
		nDdWidth = 2;
int nRetVal = 0;
CRect rcH( rcButton );
	rcH.right = rcH.left + nDdWidth;
	if( rcH.PtInRect(point) )
		nRetVal = -1;
	else
	{
		rcH = rcButton;
		rcH.left = rcH.right - nDdWidth;
		if( rcH.PtInRect(point) )
			nRetVal = 1;
	} // else from if( rcH.PtInRect(point) )
	if( nRetVal != 0 )
		pSite->OnGetCmdItemMinMaxSizeH(
			this,
			pNode,
			p_nWidthMin,
			p_nWidthMax
			);
	return nRetVal;
}
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
