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

#if (!defined __EXT_RESIZABLE_DIALOG_H)
	#include <ExtResizableDialog.h>
#endif

#if (!defined __EXT_CONTROLBAR_H)
	#include <ExtControlBar.h>
#endif

#if (!defined __ExtCmdManager_H)
	#include <ExtCmdManager.h>
#endif

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	#if (!defined __EXT_CONTROLBAR_TABBED_FEATURES_H)
		#include "ExtControlBarTabbedFeatures.h"
	#endif // __EXT_CONTROLBAR_TABBED_FEATURES_H
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtResizableDialog

IMPLEMENT_DYNCREATE( CExtResizableDialog, CExtResDlg )

CExtResizableDialog::CExtResizableDialog()
	: m_bEnabledControlBarUpdate( false )
	, m_bInConrolBarUpdate( false )
{
	m_bShowResizingGripper = true;
}

CExtResizableDialog::CExtResizableDialog(
	UINT nIDTemplate,
	CWnd * pParentWnd
	)
	: CExtWA < CExtWS < __BASEOF_CExtResizableDialog__ > >( nIDTemplate, pParentWnd )
	, m_bEnabledControlBarUpdate( false )
	, m_bInConrolBarUpdate( false )
{
	m_bShowResizingGripper = true;
}

CExtResizableDialog::CExtResizableDialog(
	__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
	CWnd * pParentWnd
	)
	: CExtWA < CExtWS < __BASEOF_CExtResizableDialog__ > >( lpszTemplateName, pParentWnd )
	, m_bEnabledControlBarUpdate( false )
	, m_bInConrolBarUpdate( false )
{
	m_bShowResizingGripper = true;
}

BEGIN_MESSAGE_MAP(CExtResizableDialog, CExtResDlg)
	//{{AFX_MSG_MAP(CExtResizableDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtResizableDialog message handlers

void CExtResizableDialog::OnOK()
{
DWORD dwStyle = GetStyle();
	if( (dwStyle & WS_CHILD) == 0 )
		CExtWA < CExtWS < __BASEOF_CExtResizableDialog__ > >::OnOK();
}

void CExtResizableDialog::OnCancel()
{
DWORD dwStyle = GetStyle();
	if( (dwStyle & WS_CHILD) == 0 )
		CExtWA < CExtWS < __BASEOF_CExtResizableDialog__ > >::OnCancel();
}


LRESULT CExtResizableDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_DESTROY )
		m_bEnabledControlBarUpdate = false;

bool bUpdateBarsOnThisMsg = false;
	if( m_bEnabledControlBarUpdate )
	{
		CWinApp * pApp = AfxGetApp();
			ASSERT( pApp != NULL );
		BOOL bIdleMsg = pApp->IsIdleMessage(
#if _MFC_VER < 0x700
			&pApp->m_msgCur
#else
			&(::AfxGetThreadState()->m_msgCur)
#endif
			);
		if( bIdleMsg )
			bUpdateBarsOnThisMsg = true;
	}

HWND hWndThis = m_hWnd;
LRESULT lResult =
		CExtWA < CExtWS < __BASEOF_CExtResizableDialog__ > >::WindowProc(message, wParam, lParam);

	if(		hWndThis == NULL
		||	( ! ::IsWindow(hWndThis) )
		)
		bUpdateBarsOnThisMsg = false;

	if(		bUpdateBarsOnThisMsg
		&&	( ! m_bInConrolBarUpdate )
		)
	{
		m_bInConrolBarUpdate = true;
		CExtControlBar::DoCustomModeUpdateControlBars( this );
		m_bInConrolBarUpdate = false;
	}

	return lResult;
}

void CExtResizableDialog::PreSubclassWindow() 
{
	CExtWA < CExtWS < __BASEOF_CExtResizableDialog__ > >::PreSubclassWindow();
	m_bEnabledControlBarUpdate = true;
	EnableToolTips();
}

BOOL CExtResizableDialog::PreTranslateMessage(MSG* pMsg) 
{
	if(		WM_KEYFIRST <= pMsg->message
		&&	pMsg->message <= WM_KEYLAST
		&&	GetSafeHwnd() != NULL
		&&	(GetStyle()&(WS_VISIBLE|WS_CHILD)) == (WS_VISIBLE|WS_CHILD)
		)
	{ // if child/visible dialog
		HWND hWndParent = ::GetParent( m_hWnd );
		CWnd * pWndParentPermanent = CWnd::FromHandlePermanent( hWndParent );
		if(		pWndParentPermanent != NULL
			&&	(	pWndParentPermanent->IsKindOf( RUNTIME_CLASS(CExtControlBar) )
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
				||	pWndParentPermanent->IsKindOf( RUNTIME_CLASS(CExtDynAutoHideSlider) )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
				)
			&&	pWndParentPermanent->PreTranslateMessage(pMsg)
			)
			return TRUE;
	} // if child/visible dialog
	if(	CExtControlBar::DoCustomModePreTranslateMessage(
			this,
			pMsg
			)
		)
		return TRUE;
	return CExtWA < CExtWS < __BASEOF_CExtResizableDialog__ > >::PreTranslateMessage(pMsg);
}

//////////////////////////////////////////////////////////////////////////
// CExtResizablePropertyPage

IMPLEMENT_DYNCREATE( CExtResizablePropertyPage, CExtResPP )

CExtResizablePropertyPage::CExtResizablePropertyPage()
	: m_bEnabledControlBarUpdate( false )
	, m_bInConrolBarUpdate( false )
{
	m_bShowResizingGripper = false;
}

CExtResizablePropertyPage::CExtResizablePropertyPage(
	UINT nIDTemplate,
	UINT nIDCaption // = 0 
	)
	: CExtWA < CExtWS < __BASEOF_CExtResizablePropertyPage__ > >( nIDTemplate, nIDCaption )
	, m_bEnabledControlBarUpdate( false )
	, m_bInConrolBarUpdate( false )
{
	m_bShowResizingGripper = false;
}

CExtResizablePropertyPage::CExtResizablePropertyPage(
	__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
	UINT nIDCaption // = 0 
	)
	: CExtWA < CExtWS < __BASEOF_CExtResizablePropertyPage__ > >( lpszTemplateName, nIDCaption )
	, m_bEnabledControlBarUpdate( false )
	, m_bInConrolBarUpdate( false )
{
	m_bShowResizingGripper = false;
}

BEGIN_MESSAGE_MAP(CExtResizablePropertyPage, CExtResPP)
	//{{AFX_MSG_MAP(CExtResizablePropertyPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtResizablePropertyPage message handlers

void CExtResizablePropertyPage::OnOK()
{
DWORD dwStyle = GetStyle();
	if( (dwStyle & WS_CHILD) == 0 )
		CExtWA < CExtWS < __BASEOF_CExtResizablePropertyPage__ > >::OnOK();
}

void CExtResizablePropertyPage::OnCancel()
{
DWORD dwStyle = GetStyle();
	if( (dwStyle & WS_CHILD) == 0 )
		CExtWA < CExtWS < __BASEOF_CExtResizablePropertyPage__ > >::OnCancel();
}


LRESULT CExtResizablePropertyPage::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
//TRACE1( "--CExtResizablePropertyPage::WindowProc(0x%08X)\n", message );
	if( message == WM_DESTROY )
		m_bEnabledControlBarUpdate = false;

bool bUpdateBarsOnThisMsg = false;
	if( m_bEnabledControlBarUpdate )
	{
		CWinApp * pApp = AfxGetApp();
			ASSERT( pApp != NULL );
		BOOL bIdleMsg = pApp->IsIdleMessage(
#if _MFC_VER < 0x700
			&pApp->m_msgCur
#else
			&(::AfxGetThreadState()->m_msgCur)
#endif
			);
		if( bIdleMsg )
			bUpdateBarsOnThisMsg = true;
	}

LRESULT lResult =
		CExtWA < CExtWS < __BASEOF_CExtResizablePropertyPage__ > >::WindowProc(message, wParam, lParam);

	switch( message )
	{
	//case WM_NCCREATE:
	case WM_INITDIALOG:
		{
			CWnd * pWndParent = GetParent();
			ASSERT_VALID( pWndParent );
			CExtResizablePropertySheet * pRPS =
				DYNAMIC_DOWNCAST( CExtResizablePropertySheet, pWndParent );
			if(		pRPS != NULL
				&&	pRPS->GetActiveIndex() != 0
				)
				pRPS->_SyncActivePage( this );
		}
		break;
	case WM_SHOWWINDOW:
	case WM_WINDOWPOSCHANGED:
			if( GetStyle() & WS_VISIBLE )
			{
				CWnd * pWndParent = GetParent();
				if(		pWndParent != NULL
					&&	pWndParent->IsKindOf(
							RUNTIME_CLASS(CExtResizablePropertySheet)
							)
					)
					((CExtResizablePropertySheet*)pWndParent)->ArrangeLayout();
				ArrangeLayout();
			}
		break;
	} // switch( message )

	if( bUpdateBarsOnThisMsg && (!m_bInConrolBarUpdate) )
	{
		m_bInConrolBarUpdate = true;
		CExtControlBar::DoCustomModeUpdateControlBars( this );
		m_bInConrolBarUpdate = false;
	}

	return lResult;
}

void CExtResizablePropertyPage::PreSubclassWindow() 
{
	CExtWA < CExtWS < __BASEOF_CExtResizablePropertyPage__ > >::PreSubclassWindow();
	m_bEnabledControlBarUpdate = true;
	EnableToolTips();
}

BOOL CExtResizablePropertyPage::PreTranslateMessage(MSG* pMsg) 
{
	if( CExtControlBar::DoCustomModePreTranslateMessage(
			this,
			pMsg
			)
		)
		return TRUE;
	return CExtWA < CExtWS < __BASEOF_CExtResizablePropertyPage__ > >::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CExtResizablePropertySheet

IMPLEMENT_DYNCREATE( CExtResizablePropertySheet, CExtResPS )

CExtResizablePropertySheet::CExtResizablePropertySheet()
	: m_bSheetInitialized( false )
	//, m_bAllowTabSwitchingLoop( false )
{
	m_bShowResizingGripper = true;
}

CExtResizablePropertySheet::CExtResizablePropertySheet(
	UINT nIDCaption,
	CWnd *pParentWnd, // = NULL
	UINT iSelectPage  // = 0
	)
	: CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > >( nIDCaption, pParentWnd, iSelectPage  )
	, m_bSheetInitialized( false )
	//, m_bAllowTabSwitchingLoop( false )
{
	m_bShowResizingGripper = true;
}

CExtResizablePropertySheet::CExtResizablePropertySheet(
	__EXT_MFC_SAFE_LPCTSTR pszCaption,
	CWnd *pParentWnd, // = NULL
	UINT iSelectPage  // = 0
	)
	: CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > >( pszCaption, pParentWnd, iSelectPage  )
	, m_bSheetInitialized( false )
	//, m_bAllowTabSwitchingLoop( false )
{
	m_bShowResizingGripper = true;
}

BEGIN_MESSAGE_MAP(CExtResizablePropertySheet, CExtResPS)
	//{{AFX_MSG_MAP(CExtResizablePropertySheet)
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtResizablePropertySheet message handlers

void CExtResizablePropertySheet::PreSubclassWindow() 
{
	CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > >::PreSubclassWindow();
	EnableToolTips();
}

void CExtResizablePropertySheet::_SyncActivePage(
	CPropertyPage * pActivePage // = NULL
	)
{
CPropertyPage * pWnd =
		(pActivePage->GetSafeHwnd() != NULL)
			? pActivePage
			: CExtResPS::GetActivePage()
			;
	if(		pWnd != NULL
		&&	pWnd->GetSafeHwnd() != NULL
		&&	::IsWindow( pWnd->GetSafeHwnd() )
		)
	{
		RDI_t _val;
		if( ! m_mapRDI.Lookup(
				pWnd->GetSafeHwnd(),
				_val
				)
			)
		{
			for( POSITION pos = m_mapRDI.GetStartPosition(); pos != NULL; )
			{
				HWND _hWnd;
				RDI_t _rdi;
				m_mapRDI.GetNextAssoc( pos, _hWnd, _rdi );
				ASSERT( _hWnd != NULL );
				if( ! ::IsWindow( _hWnd ) )
					continue;
				CWnd * _pWnd = CWnd::FromHandlePermanent( _hWnd );
				if( _pWnd == NULL )
					continue;
				if( _pWnd->IsKindOf(RUNTIME_CLASS(CPropertyPage)) )
				{
					RECT rcMove;
					_pWnd->GetWindowRect( &rcMove );
					ScreenToClient( &rcMove );
					pWnd->MoveWindow( &rcMove );
					break;
				} // if( _pWnd->IsKindOf(RUNTIME_CLASS(CPropertyPage)) )
			} // for( POSITION pos = m_mapRDI.GetStartPosition(); pos != NULL; )
			AddAnchor(
				pWnd->GetSafeHwnd(),
				__RDA_LT,
				__RDA_RB
				);
		}
	}
	ArrangeLayout();
}

LRESULT CExtResizablePropertySheet::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
CPropertyPage * pWndPrevActivePage = NULL;
	switch( message )
	{
	case WM_CLOSE:
		if( m_bEnableSaveRestore )
			SaveWindowRect();
	break;
	case WM_SYSCOMMAND:
		if( wParam == SC_CLOSE )
		{
			if( m_bEnableSaveRestore )
				SaveWindowRect();
		}
	break;
	case WM_COMMAND:
		if( wParam == IDOK || wParam == IDCANCEL )
		{
			if( m_bEnableSaveRestore )
				SaveWindowRect();
		}
		else if( wParam == ID_WIZBACK || wParam == ID_WIZNEXT )
		{
			pWndPrevActivePage = CExtResPS::GetActivePage();
		}
	break;
	case WM_DESTROY:
	case WM_NCDESTROY:
		m_bSheetInitialized = false;
	break;
	case WM_SIZE:
		_SyncActivePage();
	break;
	} // switch( message )

LRESULT lResult =
		m_bSheetInitialized
		? CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > > :: WindowProc(message,wParam,lParam)
		: CExtResPS :: WindowProc(message,wParam,lParam)
		;
	
	switch( message )
	{
	case WM_CREATE:
		{
			// window should be resizable
			ModifyStyle(
				0,
				WS_THICKFRAME | WS_CLIPCHILDREN
				);

			::PostMessage( m_hWnd, (WM_USER+10), 0, 0 );
		}
	break;
	case (WM_USER+10):
		{
			if( m_bSheetInitialized )
				break;
			m_bSheetInitialized = true;

			CRect rcWnd;
			GetWindowRect( &rcWnd );
			CSize sizeWnd = rcWnd.Size();
			SetMinTrackSize( sizeWnd );
			::SetWindowPos(
				m_hWnd, NULL, 0,0,0,0,
				SWP_NOSIZE|SWP_NOMOVE
					|SWP_NOZORDER|SWP_NOOWNERZORDER
					|SWP_NOREPOSITION
					|SWP_FRAMECHANGED
				);

			CTabCtrl * pTabCtrl = CExtResPS::GetTabControl();
			CWnd * pWnd = GetWindow( GW_CHILD );
			for( ; pWnd != NULL; pWnd = pWnd->GetWindow(GW_HWNDNEXT)  )
			{
				if( pTabCtrl == pWnd )
				{
					pTabCtrl->ModifyStyle( 0, WS_CLIPSIBLINGS );
					AddAnchor( pTabCtrl->GetSafeHwnd(), __RDA_LT, __RDA_RB );
					continue;
				}
				
				CExtSafeString sClassName;
				::GetClassName(
					pWnd->GetSafeHwnd(),
					sClassName.GetBuffer( _MAX_PATH+1 ),
					_MAX_PATH
					);
				sClassName.ReleaseBuffer();
				sClassName.MakeLower();
				if( sClassName == LPCTSTR( _T("static") ) )
				{
					pWnd->ModifyStyle( 0, WS_CLIPSIBLINGS );
					
#if _MFC_VER >= 0x700
					if( (m_psh.dwFlags&PSH_WIZARD97) != 0 )
					{
						if( pWnd->GetDlgCtrlID() == 0x3027 )
						{
							AddAnchor(
								pWnd->GetSafeHwnd(),
								__RDA_LT,
								__RDA_RT
								);
							continue;
						}
					}
#endif // #if _MFC_VER >= 0x700
					AddAnchor(
						pWnd->GetSafeHwnd(),
						__RDA_LB,
						__RDA_RB
						);
					continue;
				}
				if( sClassName == LPCTSTR( _T("button") ) )
				{
					CRect rcWndButton;
					pWnd->GetWindowRect( &rcWndButton );
					ScreenToClient( &rcWndButton );
					rcWndButton.OffsetRect( -10, 0 );
					pWnd->MoveWindow( &rcWndButton );

					pWnd->ModifyStyle( 0, WS_CLIPSIBLINGS );
					AddAnchor( pWnd->GetSafeHwnd(), __RDA_RB );
					
					CButtonInSheetHook * pBtn = new CButtonInSheetHook;
					VERIFY(
						pBtn->SubclassWindow(
							pWnd->GetSafeHwnd()
							)
						);
					continue;
				}
				
			} // for( ; pWnd != NULL; pWnd = pWnd->GetWindow(GW_HWNDNEXT)  )

			_SyncActivePage();

			CExtPaintManager::monitor_parms_t _mp;
			CExtPaintManager::stat_GetMonitorParms( _mp, this );

			SetMaximizedRect( &_mp.m_rcWorkArea );
			SetMaxTrackSize( _mp.m_rcWorkArea.Size() );

			CRect _rcWnd;
			GetWindowRect( &_rcWnd );;
			CSize _size = _rcWnd.Size();
			::SendMessage(
				m_hWnd, WM_SIZE,
				WPARAM(SIZE_RESTORED),
				MAKELPARAM( _size.cx, _size.cy )
				);
			if( GetStyle() & WS_VISIBLE )
				RedrawWindow();

			OnResizablePropertySheetInitialized();
		}
	break;
	case WM_COMMAND:
		if( wParam == ID_WIZBACK || wParam == ID_WIZNEXT )
		{
			if( !m_bSheetInitialized )
				break;

			if( pWndPrevActivePage != NULL )
			{
				CPropertyPage * pWndNewActivePage = CExtResPS::GetActivePage();
				if( pWndPrevActivePage != pWndNewActivePage )
				{
					CRect rcWndPage;
					pWndPrevActivePage->GetWindowRect( &rcWndPage );
					ScreenToClient( &rcWndPage );
					pWndNewActivePage->MoveWindow( &rcWndPage );
				}
			}
			_SyncActivePage();
			ArrangeLayout();
		}
	break;
	} // switch( message )

	return lResult;
}

BOOL CExtResizablePropertySheet::PreCreateWindow(CREATESTRUCT& cs)
{
	if( ! ( CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > > :: PreCreateWindow(cs) ) )
	{
		ASSERT( FALSE );
		return FALSE;
	}

	return TRUE;
}

BOOL CExtResizablePropertySheet::PreTranslateMessage(MSG* pMsg)
{
	if(		GetSafeHwnd() != NULL
		&&	::IsWindow( GetSafeHwnd() )
		//&&	( GetStyle() & WS_CHILD ) != 0
		)
	{ // if valid sheet
		if(		pMsg->message == WM_KEYDOWN
			&&	pMsg->wParam == VK_TAB
			)
		{ // if WM_KEYDOWN with VK_TAB
			HWND hWndFocus = ::GetFocus();
			if(		hWndFocus != NULL
				&&	(	hWndFocus == GetSafeHwnd()
					||	::IsChild( GetSafeHwnd(), hWndFocus )
					)
				)
			{
				BOOL bShift = CExtPopupMenuWnd::IsKeyPressed(VK_SHIFT);
				BOOL bCtrl = CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL);
				if( !bCtrl )
				{
					SendMessage( WM_NEXTDLGCTL, bShift, 0 );
					return TRUE;
				}
// + (2.25)
				if( bShift )
				{
					if( GetActiveIndex() == 0 )
						return TRUE;
					SendMessage( WM_COMMAND, ID_WIZBACK );
				} // if( bShift )
				else
				{
					if( GetActiveIndex() == (GetPageCount() - 1) )
						return TRUE;
					SendMessage( WM_COMMAND, ID_WIZNEXT );
				} // else from if( bShift )
				return TRUE;
// - (2.25)
//				CTabCtrl * pTabCtrl = CExtResPS::GetTabControl();
//				if( pTabCtrl != NULL )
//				{
//					int nItemCount = pTabCtrl->GetItemCount();
//					if( nItemCount > 1 )
//					{
//						int nCurSel = pTabCtrl->GetCurSel();
//						nCurSel += bShift ? -1 : 1;
//						if( nCurSel < 0 || nCurSel == nItemCount )
//						{
//							if( ! m_bAllowTabSwitchingLoop )
//								return TRUE;
//							if( nCurSel < 0 )
//								nCurSel = nItemCount - 1;
//							else
//							{
//								ASSERT( nCurSel == nItemCount );
//								nCurSel = 0;
//							}
//						} // if( nCurSel == nItemCount )
//						if( (pTabCtrl->GetStyle()&WS_VISIBLE) != 0 )
//						{
//							//pTabCtrl->SetCurSel( nCurSel );
//							pTabCtrl->SetFocus();
//						} // if( (pTabCtrl->GetStyle()&WS_VISIBLE) != 0 )
//						SetActivePage( nCurSel );
//						_SyncActivePage();
//					}
//					_TrickSyncActviveChild();
//				} // valid sheet
//				return TRUE;
			}
		} // if WM_KEYDOWN with VK_TAB
	} // if valid sheet
	return CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > > :: PreTranslateMessage(pMsg);
}

BOOL CExtResizablePropertySheet::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(		GetSafeHwnd() != NULL
		&&	::IsWindow( GetSafeHwnd() )
		)
	{ // if valid sheet
		if( nID == ID_WIZNEXT || nID == ID_WIZBACK )
		{
			BOOL bRetVal =
				CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > >
					:: OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
			if( nCode == CN_COMMAND )
				_SyncActivePage();
			return bRetVal;
		} // if( nID == ID_WIZNEXT || nID == ID_WIZBACK )
		if( ( GetStyle() & WS_CHILD ) != 0 )
		{ // if valid child sheet
			if( CWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) )
				return TRUE;
			if(		(	nCode != CN_COMMAND
					&& nCode != CN_UPDATE_COMMAND_UI
					)
					||	( ((nID) & 0x8000) == 0 )
					||	nID >= 0xf000
					)
				return FALSE;
			// last crack goes to the current CWinThread object
			CWinThread* pThread = AfxGetThread();
			if( pThread != NULL )
			{
#ifdef _DEBUG
				if (afxTraceFlags & traceCmdRouting)
					TRACE1("Routing command id 0x%04X to app.\n", nID);
#endif
				if (pThread->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
					return TRUE;
			}
#ifdef _DEBUG
			if (afxTraceFlags & traceCmdRouting)
			{
				TRACE2("IGNORING command id 0x%04X sent to %hs dialog.\n", nID,
						GetRuntimeClass()->m_lpszClassName);
			}
#endif
			return FALSE;
		} // if valid child sheet
	} // if valid sheet
	return
		CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > >
			:: OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

void CExtResizablePropertySheet::OnSize(UINT nType, int cx, int cy) 
{
	CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > > :: OnSize(nType, cx, cy);

	if(		GetSafeHwnd() != NULL
		&&	::IsWindow( GetSafeHwnd() )
		&&	( GetStyle() & WS_CHILD ) != 0
		)
	{ // if valid child sheet
		CTabCtrl * pTabCtrl = GetTabControl();
		if( pTabCtrl != NULL )
		{
			pTabCtrl->MoveWindow( 0, 0, cx, cy, FALSE );
			ArrangeLayout();
			_TrickSyncActviveChild();
		} // if( pTabCtrl != NULL )
	} // if valid child sheet
}

void CExtResizablePropertySheet::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > > :: OnWindowPosChanged(lpwndpos);
	
	if(		GetSafeHwnd() != NULL
		&&	::IsWindow( GetSafeHwnd() )
		&&	( GetStyle() & (WS_CHILD|WS_VISIBLE) ) == (WS_CHILD|WS_VISIBLE)
		)
	{ // if valid visible child sheet
		CTabCtrl * pTabCtrl = GetTabControl();
		if( pTabCtrl != NULL )
		{
			CRect rcClient;
			GetClientRect( &rcClient );
			pTabCtrl->MoveWindow( 0, 0, rcClient.Width(), rcClient.Height(), FALSE );
			ArrangeLayout();
			_TrickSyncActviveChild();
		} // if( pTabCtrl != NULL )
	} // if valid visible child sheet
}

void CExtResizablePropertySheet::OnResizablePropertySheetInitialized()
{
	if( ( GetStyle() & WS_CHILD ) != 0 )
	{
		_TrickSyncActviveChild();
	} // if( ( GetStyle() & WS_CHILD ) != 0 )
	else
	{
		// + (2.25)
		ModifyStyle( 0, WS_CLIPCHILDREN );
		if( m_bEnableSaveRestore )
			LoadWindowRect();
	} // else from if( ( GetStyle() & WS_CHILD ) != 0 )
	SetWindowPos(
		NULL, 0, 0, 0, 0,
		SWP_NOSIZE|SWP_NOMOVE
			|SWP_NOCOPYBITS
			|SWP_NOZORDER|SWP_NOOWNERZORDER
			|SWP_FRAMECHANGED
		);
	ArrangeLayout();
}

void CExtResizablePropertySheet::_TrickSyncActviveChild()
{
}
