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

#if (!defined __EXT_COMBO_BOX_H)
	#include <ExtComboBox.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __AFXPRIV_H__)
	#include <AfxPriv.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtComboEditCtrlHook

CExtComboEditCtrlHook::CExtComboEditCtrlHook()
{
}
CExtComboEditCtrlHook::~CExtComboEditCtrlHook()
{
}

CExtComboBox * CExtComboEditCtrlHook::GetExtComboBox()
{
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT( ::IsWindow(GetSafeHwnd()) );
HWND hWndParent = ::GetParent( GetSafeHwnd() );
	ASSERT( hWndParent != NULL );
	ASSERT( ::IsWindow(hWndParent) );
CExtComboBox * pCombo = 
		DYNAMIC_DOWNCAST(
			CExtComboBox,
			FromHandlePermanent(hWndParent)
			);
	ASSERT( pCombo != NULL );
	return pCombo;
}

IMPLEMENT_DYNCREATE( CExtComboEditCtrlHook, CExtEditBase );

BEGIN_MESSAGE_MAP(CExtComboEditCtrlHook, CExtEditBase)
	//{{AFX_MSG_MAP(CExtComboEditCtrlHook)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

LRESULT CExtComboEditCtrlHook::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
bool bFlushAutoComplete = false;

	switch( message )
	{
	case WM_CUT:
	case WM_COPY:
	case WM_PASTE:
	case WM_CLEAR:
	case WM_UNDO:
		bFlushAutoComplete = true;
		break;
	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case ID_EDIT_CLEAR:
		case ID_EDIT_CLEAR_ALL:
		case ID_EDIT_COPY:
		case ID_EDIT_CUT:
		case ID_EDIT_FIND:
		case ID_EDIT_PASTE:
		case ID_EDIT_PASTE_LINK:
		case ID_EDIT_PASTE_SPECIAL:
		case ID_EDIT_REPEAT:
		case ID_EDIT_REPLACE:
		case ID_EDIT_SELECT_ALL:
		case ID_EDIT_UNDO:
		case ID_EDIT_REDO:
			bFlushAutoComplete = true;
		break;
		} // switch( LOWORD(wParam) )
		break;
	} // switch( message )
	
	if( bFlushAutoComplete )
		GetExtComboBox()->m_bAutoComplete = false;

	return CExtEditBase::WindowProc(message,wParam,lParam);
}

void CExtComboEditCtrlHook::OnContextMenu(CWnd* pWnd,CPoint pos )
{
	if( ! m_bHandleCtxMenus )
	{
		CExtEditBase::OnContextMenu( pWnd, pos );
		return;
	} // if( ! m_bHandleCtxMenus )
CExtComboBox * pCombo = GetExtComboBox();
	if( pCombo == NULL )
		return;
	m_bHandleCtxMenus = pCombo->m_bHandleCtxMenus;
	SetFocus();

		CExtEditBase::OnContextMenu( pWnd, pos );

	VERIFY(
		pCombo->RedrawWindow(
		NULL,
		NULL,
		RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW
		|RDW_FRAME|RDW_ALLCHILDREN
		)
		);
}
	
/////////////////////////////////////////////////////////////////////////////
// CExtComboBox

CExtComboBox::CExtComboBox()
	: m_bAutoComplete( true )
	, m_bHandleCtxMenus( true )
	, m_bEnableAutoComplete( true )
	, m_bDrawing( false )
	, m_bWatching( false )
	, m_dwUpdateTimer( 10 )
	, m_dwUpdatePeriod( 50 )
{
}

CExtComboBox::~CExtComboBox()
{
}

IMPLEMENT_DYNCREATE( CExtComboBox, CComboBox );

BEGIN_MESSAGE_MAP(CExtComboBox, CComboBox)
	//{{AFX_MSG_MAP(CExtComboBox)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
	ON_CONTROL_REFLECT( CBN_EDITUPDATE, OnEditCtrlUpdate )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtComboBox message handlers

void CExtComboBox::OnTimer(UINT nIDEvent) 
{
	if( nIDEvent != m_dwUpdateTimer )
	{
		CComboBox::OnTimer( nIDEvent );
		return;
	} // if( nIDEvent != m_dwUpdateTimer )

POINT ptCursorPos;
	if( ! ::GetCursorPos( &ptCursorPos) )
		return;
CRect rcItem;
	GetWindowRect( &rcItem );

bool bHover = false;
	if( rcItem.PtInRect(ptCursorPos) )
		bHover = true;
	if( (!bHover) || m_bDrawing )
	{
		m_bDrawing = false;
		Invalidate();
		UpdateWindow();
	}

	if( !bHover )
	{
		m_bWatching = false;
		KillTimer( m_dwUpdateTimer );

		HWND hWndFocus = ::GetFocus();
		if( hWndFocus != NULL )
		{
			CExtComboBox * pCombo = NULL;
			CWnd * pWndAnalyze = CWnd::FromHandlePermanent( hWndFocus );
			if( pWndAnalyze != NULL )
				pCombo =
					DYNAMIC_DOWNCAST(
						CExtComboBox,
						pWndAnalyze
						);
			if( pCombo == NULL )
			{
				pWndAnalyze = CWnd::FromHandlePermanent( ::GetParent(hWndFocus) );
				if( pWndAnalyze != NULL )
					pCombo =
						DYNAMIC_DOWNCAST(
							CExtComboBox,
							pWndAnalyze
							);
			}
			if( pCombo == NULL )
			{
				HWND hWndFromPoint = ::WindowFromPoint(ptCursorPos);
				if( hWndFromPoint != NULL )
				{
					CWnd * pWndAnalyze = CWnd::FromHandlePermanent( hWndFocus );
					if( pWndAnalyze != NULL )
						pCombo =
							DYNAMIC_DOWNCAST(
								CExtComboBox,
								pWndAnalyze
								);
					if( pCombo == NULL )
					{
						pWndAnalyze = CWnd::FromHandlePermanent( ::GetParent(hWndFocus) );
						if( pWndAnalyze != NULL )
							pCombo =
								DYNAMIC_DOWNCAST(
									CExtComboBox,
									pWndAnalyze
									);
					}
				}
			}
			if( pCombo != NULL )
			{
				pCombo->m_bDrawing = true;
				if( !pCombo->m_bWatching )
				{
					pCombo->m_bWatching = true;
					pCombo->SetTimer( pCombo->m_dwUpdateTimer, pCombo->m_dwUpdatePeriod, NULL );
					//pCombo->OnTimer( pCombo->m_dwUpdateTimer );
				} // if( !pCombo->m_bWatching )
			} // if( pCombo != NULL )
		} // if( hWndFocus != NULL )
	} // if( !bHover )
}

void CExtComboBox::_OnPaintImpl(
	bool bPressed,
	bool bHover
	)
{
	if( (GetStyle()&(CBS_OWNERDRAWFIXED|CBS_OWNERDRAWVARIABLE)) != 0 )
	{
		Default();
		CClientDC dc(this);
		_OnDrawComboImpl( bPressed, bHover, &dc );
	}
	else
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		CPaintDC dcPaint( this );
		CExtPaintManager::stat_ExcludeChildAreas(
			dcPaint.GetSafeHdc(),
			GetSafeHwnd()
			);
		CExtMemoryDC dcmm( &dcPaint, &rcClient );
		DefWindowProc( WM_PAINT, (WPARAM)dcmm.GetSafeHdc(), (LPARAM)0 );
		_OnDrawComboImpl( bPressed, bHover, &dcmm );
	}
}

void CExtComboBox::OnPaint() 
{
	if(		(	GetExStyle()
				& (WS_EX_DLGMODALFRAME|WS_EX_CLIENTEDGE|WS_EX_STATICEDGE)
			) != 0
		)
		ModifyStyleEx(
			WS_EX_DLGMODALFRAME|WS_EX_CLIENTEDGE|WS_EX_STATICEDGE,
			0,
			SWP_FRAMECHANGED
			);

CPoint ptCursorPos( 0, 0 );
	::GetCursorPos( &ptCursorPos );
CRect rcItem;
	GetWindowRect( &rcItem );

bool bHover = false;
	if( rcItem.PtInRect(ptCursorPos) )
		bHover = true;

	_OnPaintImpl( false, bHover );
}

void CExtComboBox::_OnDrawComboImpl(
	bool bPressed,
	bool bHover,
	CDC * pDC // = NULL
	)
{
	if(		bHover
		&&	CExtMouseCaptureSink::GetCapture() != NULL
		)
		bHover = false;

CRect rectClient;
	GetClientRect( &rectClient );
bool bCallReleaseDC = false;
	if( pDC == NULL )
	{
		pDC = GetDC();
		ASSERT( pDC != NULL );
		bCallReleaseDC = true;
	}
	
bool bEnabled = IsWindowEnabled() ? true : false;
bool bPushed =
		( bPressed )
		|| GetDroppedState()
		;
	if( !bEnabled )
	{
		bPushed = false;
		bHover = false;
	}
	if( CExtPopupMenuWnd::IsMenuTracking() )
		bHover = false;
	else if( !bHover )
	{
		HWND hWndFocus = ::GetFocus();
		if(		hWndFocus == GetSafeHwnd()
			||	::IsChild( GetSafeHwnd(), hWndFocus )
			)
			bHover = true;
	}

CExtPaintManager::PAINTCOMBOFRAMEDATA _pcfd(
		this,
		rectClient,
		bHover,
		bPushed,
		bEnabled
		);
	g_PaintManager->PaintComboFrame( *pDC, _pcfd );

	if( bCallReleaseDC )
		ReleaseDC( pDC );
}

BOOL CExtComboBox::PreTranslateMessage(MSG* pMsg) 
{
	if( m_bEnableAutoComplete )
	{
		if( pMsg->message == WM_SYSCHAR )
			return TRUE;
		if( pMsg->message == WM_KEYDOWN )
		{
			m_bAutoComplete = true;
			int nVirtKey = (int) pMsg->wParam;
			if(nVirtKey == VK_DELETE || nVirtKey == VK_BACK)
				m_bAutoComplete = false;
		} // if( pMsg->message == WM_KEYDOWN )
	} // if( m_bEnableAutoComplete )
	else
	{
		m_bAutoComplete = false;
	} // else from if( m_bEnableAutoComplete )

	if( pMsg->message == WM_MOUSEMOVE && (!m_bDrawing) && (!m_bWatching) )
	{
		HWND hWndOwn = GetSafeHwnd();
		if(		hWndOwn != NULL
			&&	::IsWindow( hWndOwn )
			&&	(	pMsg->hwnd == hWndOwn
				||	(	(GetStyle() & CBS_DROPDOWN) != 0
					&&	pMsg->hwnd == ::GetWindow(hWndOwn,GW_CHILD)
					)
				)
			&&	CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND(hWndOwn)
			)
		{
			m_bDrawing = true;
			m_bWatching = true;
			SetTimer( m_dwUpdateTimer, m_dwUpdatePeriod, NULL );
			//OnTimer( m_dwUpdateTimer );
		}
	} // if( pMsg->message == WM_MOUSEMOVE && (!m_bDrawing) && (!m_bWatching) )

	return CComboBox::PreTranslateMessage(pMsg);
}

void CExtComboBox::OnEditCtrlUpdate() 
{
	if( !m_bEnableAutoComplete )
	{
		Default();
		return;
	}
	if( !m_bAutoComplete ) 
		return;
CString str;
	GetWindowText( str );
int nLength = str.GetLength();
DWORD dwCurSel = GetEditSel();
WORD dStart = LOWORD( dwCurSel );
WORD dEnd   = HIWORD( dwCurSel );
int nSelectStringResult = SelectString( -1, str );
	if( nSelectStringResult == CB_ERR )
	{
		SetWindowText( str );
		if( dwCurSel != CB_ERR )
			SetEditSel( dStart, dEnd );
	} // if( nSelectStringResult == CB_ERR )
	if( dEnd < nLength && dwCurSel != CB_ERR )
		SetEditSel( dStart, dEnd );
	else
		SetEditSel( nLength, -1 );
}

BOOL CExtComboBox::OnEraseBkgnd(CDC* pDC) 
{
	pDC;
	return TRUE;
}

LRESULT CExtComboBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch( message )
	{
	case WM_ENABLE:
		{
			HWND hWndEdit =
				::GetWindow( GetSafeHwnd(), GW_CHILD );
			if( hWndEdit == NULL || !::IsWindow(hWndEdit) )
				break;
			::EnableWindow( hWndEdit, TRUE );
			::SendMessage( hWndEdit, EM_SETREADONLY, !wParam, 0L );
			Invalidate();
		}
		break;
	case WM_LBUTTONUP:
		Invalidate();
		break;
	case WM_MOUSEMOVE:
		if( CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND(
				GetSafeHwnd()
				)
			)
		{
			m_bDrawing = true;
			if( !m_bWatching )
			{
				m_bWatching = true;
				SetTimer( m_dwUpdateTimer, m_dwUpdatePeriod, NULL );
				OnTimer( m_dwUpdateTimer );
			} // if( !m_bWatching )
		}
		break;
	case WM_SETFOCUS:
		{
			LRESULT lResult =
				CComboBox::WindowProc(message, wParam, lParam);;
			//_OnDrawComboImpl( false, true );
			_OnDrawComboImpl(
				GetDroppedState() ? true : false,
				true
				);
			return lResult;
		}
	case WM_KILLFOCUS:
		{
			LRESULT lResult =
				CComboBox::WindowProc(message, wParam, lParam);;
			if( GetDroppedState() )
				ShowDropDown( FALSE );
			//_OnDrawComboImpl( false, false );
			_OnDrawComboImpl(
				GetDroppedState() ? true : false,
				false
				);
			return lResult;
		}
	case WM_LBUTTONDOWN:
		{
			ASSERT( CBS_SIMPLE       == 0x0001L );
			ASSERT( CBS_DROPDOWN     == 0x0002L );
			ASSERT( CBS_DROPDOWNLIST == 0x0003L );
			DWORD dwStyle = GetStyle();
			DWORD dwType = dwStyle & 0x0003L;
			if( dwType == CBS_DROPDOWN || dwType == CBS_DROPDOWNLIST )
			{
				HWND hWndOwn = GetSafeHwnd();
				ASSERT( hWndOwn != NULL && ::IsWindow(hWndOwn) );

				CWnd * pWndParent = GetParent();
				if( pWndParent != NULL )
				{
					pWndParent->SendMessage( WM_CANCELMODE );
					if( !::IsWindow(hWndOwn) )
						return TRUE;
				}

				BOOL bDropped = GetDroppedState();
				if(		dwType == CBS_DROPDOWNLIST
					&&	m_wndInnerEditHook.GetSafeHwnd() != NULL
					)
					m_wndInnerEditHook.SetFocus();
				else
					SetFocus();
				_OnDrawComboImpl(
					(!bDropped) ? true : false,
					true
					);
				::PostMessage(hWndOwn, CB_SHOWDROPDOWN, !bDropped, 0);
				return TRUE;
			} // if( dwType == CBS_DROPDOWN || dwType == CBS_DROPDOWNLIST )
		}
		break;

		case WM_CREATE:
		{
			LRESULT lResult =
				CComboBox::WindowProc(message, wParam, lParam);;
			_SubclassInnerEdit();
			return lResult;
		}

	} // switch( message )

	return CComboBox::WindowProc(message, wParam, lParam);
}

void CExtComboBox::PreSubclassWindow() 
{
	CComboBox::PreSubclassWindow();
	_SubclassInnerEdit();
}
