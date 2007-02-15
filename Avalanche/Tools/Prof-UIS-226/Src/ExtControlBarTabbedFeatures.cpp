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

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

#if (!defined __EXT_CONTROLBAR_TABBED_FEATURES_H)
	#include "ExtControlBarTabbedFeatures.h"
#endif // __EXT_CONTROLBAR_TABBED_FEATURES_H

#if _MFC_VER < 0x700
	#include <../src/AfxImpl.h>
#else
	#include <../src/mfc/AfxImpl.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

/////////////////////////////////////////////////////////////////////////
// CExtDockDynTabBar window

IMPLEMENT_DYNCREATE(CExtDockDynTabBar, CExtDockDynBar);

BEGIN_MESSAGE_MAP(CExtDockDynTabBar, CExtDockDynBar)
	//{{AFX_MSG_MAP(CExtDockDynTabBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CExtDockDynTabBar::CExtDockDynTabBar(
	UINT nCircleNo
	)
	: CExtDockDynBar( nCircleNo )
{
}

void CExtDockDynTabBar::_SyncTabbedChilds(
	bool bSyncSizes,
	bool bSyncSwitcher
	)
{

CExtDynTabControlBar * pParentBar =
		STATIC_DOWNCAST(
			CExtDynTabControlBar,
			GetParent()
			);

	if( bSyncSizes )
	{
		for( INT nBar = 1; nBar < m_arrBars.GetSize(); nBar++ )
		{
			ASSERT( m_arrBars[0] == NULL );
			CExtControlBar * pBar = (CExtControlBar *)m_arrBars[nBar];
			if( pBar == NULL )
				continue;
			if( __PLACEHODLER_BAR_PTR(pBar) )
				continue;
			ASSERT( !pBar->IsFixedMode() );
			pBar->m_sizeDockedH = pParentBar->m_sizeDockedH;
			pBar->m_sizeDockedV = pParentBar->m_sizeDockedV;
			pBar->m_sizeFloated = pParentBar->m_sizeFloated;
		}
	}

	if( bSyncSwitcher )
		pParentBar->SyncSwitcher();
}

void CExtDockDynTabBar::OnDynamicLayoutUpdate()
{
	if( m_bLockedOptimize )
		return;
	CExtDockDynBar::OnDynamicLayoutUpdate();
	_SyncTabbedChilds( true, false );
}

void CExtDockDynTabBar::OnDynamicLayoutOptimize()
{
	if( m_bLockedOptimize )
		return;

//	_SyncTabbedChilds( true, false );
HWND hWndOwn = GetSafeHwnd();
	ASSERT( hWndOwn != NULL );
	ASSERT( ::IsWindow(hWndOwn) );
	CExtDockDynBar::OnDynamicLayoutOptimize();
	if( ! ::IsWindow(hWndOwn) )
		return;
	_SyncTabbedChilds( false, true );
}

CSize CExtDockDynTabBar::_RepositionTabbedChilds(
	bool bLockChilds
	)
{
CRect rect;
	GetWindowRect( &rect );
CSize _size = rect.Size();
CRect rcClient;
	GetClientRect( &rcClient );
INT nCount = m_arrBars.GetSize();
	ASSERT( nCount > 0 );
	ASSERT( m_arrBars[0] == NULL );
INT nCountReal = nCount - 1;
INT nBar;
	for( nBar = 1; nBar < nCount; nBar++ )
	{
		CExtControlBar * pBar = (CExtControlBar *)m_arrBars[nBar];
		if( pBar == NULL )
		{
			nCountReal --;
			continue;
		} // if( pBar == NULL )
		if( __PLACEHODLER_BAR_PTR( pBar ) )
		{
			nCountReal --;
			continue;
		} // if( __PLACEHODLER_BAR_PTR( pBar ) )
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CExtControlBar, pBar );
		ASSERT( ! pBar->IsFixedMode() );
		if( bLockChilds )
			pBar->m_bUpdatingChain = true;
	} // for( nBar = 1; nBar < nCount; nBar++ )
	ASSERT( nCountReal >= 0 );
	if( nCountReal == 0 )
		return _size;
HANDLE hDWP = ::BeginDeferWindowPos( nCountReal );
	ASSERT( hDWP != NULL );
	for( nBar = 1; nBar < nCount; nBar++ )
	{
		CExtControlBar * pBar = (CExtControlBar *)m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CExtControlBar, pBar );
		ASSERT( ! pBar->IsFixedMode() );
		hDWP =
			::DeferWindowPos(
				hDWP,
				pBar->GetSafeHwnd(),
				NULL,
				rcClient.left,
				rcClient.top,
				rcClient.Width(),
				rcClient.Height(),
				SWP_NOZORDER
					|SWP_NOOWNERZORDER
					|SWP_NOACTIVATE
					|SWP_FRAMECHANGED
				);
		ASSERT( hDWP != NULL );
		if( hDWP == NULL )
			break;
	} // for( nBar = 1; nBar < nCount; nBar++ )
	ASSERT( hDWP != NULL );
	if( hDWP != NULL )
		::EndDeferWindowPos( hDWP );
	if( bLockChilds )
	{
		for( nBar = 1; nBar < nCount; nBar++ )
		{
			CExtControlBar * pBar = (CExtControlBar *)m_arrBars[nBar];
			if( pBar == NULL )
				continue;
			if( __PLACEHODLER_BAR_PTR(pBar) )
				continue;
			ASSERT_VALID( pBar );
			ASSERT_KINDOF( CExtControlBar, pBar );
			ASSERT( ! pBar->IsFixedMode() );
			//if( ! pBar->IsVisible() )
			//	continue;
			pBar->m_bUpdatingChain = false;
			pBar->OnRepositionSingleChild();
		}
	}

	return _size;
}

CSize CExtDockDynTabBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	bStretch;
	bHorz;

	return _RepositionTabbedChilds( false );
}

void CExtDockDynTabBar::CalcOrderedVector(
	ExtControlBarVector_t & vBars
	)
{
	ASSERT_VALID( this );

CExtDynTabControlBar * pTabbedBar =
		STATIC_DOWNCAST(
			CExtDynTabControlBar,
			GetParent()
			);
	ASSERT_VALID( pTabbedBar );
INT nCount = pTabbedBar->GetSwitcherItemCount();
	for( INT nBar = 0; nBar < nCount; nBar++ )
	{
		CExtControlBar * pBar =
			pTabbedBar->GetBarAt( nBar, true );
		if( pBar == NULL )
			continue;
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CExtControlBar, pBar );
		ASSERT( !pBar->IsFixedMode() );
		vBars.Add( pBar );
	}

#ifdef _DEBUG
INT nRealDockedCount = GetDockedCount();
	ASSERT( nCount == nRealDockedCount );
#endif // _DEBUG

}

BOOL CExtDockDynTabBar::PreTranslateMessage(MSG* pMsg)
{
	if( GetSafeHwnd() != NULL )
	{
		CExtDynTabControlBar * pTabbedBar =
			STATIC_DOWNCAST(
				CExtDynTabControlBar,
				GetParent()
				);
		ASSERT_VALID( pTabbedBar );
		if( pTabbedBar->PreTranslateMessage(pMsg) )
			return TRUE;
	}
	return CExtDockDynBar::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////
// CExtDynTabWnd window

DROPEFFECT CExtDynTabWnd::CDynDropTarget::OnDragOver(
	CWnd * pWnd,
	COleDataObject * pDataObject,
	DWORD dwKeyState,
	CPoint point
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pWndDynTab );
	pWnd;
	pDataObject;
	dwKeyState;
	point;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_pWndDynTab->m_hWnd );
	if( pSite != NULL )
	{
		if( pSite->IsCustomizeMode() )
			return DROPEFFECT_NONE;
		if( pSite->DraggedNodeGet() != NULL )
			return DROPEFFECT_NONE;
	} // if( pSite != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

LONG nIndex = m_pWndDynTab->ItemHitTest( point );
	if( nIndex < 0 )
		return DROPEFFECT_NONE;
	ASSERT( nIndex < m_pWndDynTab->ItemGetCount() );
LONG nSel = m_pWndDynTab->SelectionGet();
	if( nSel != nIndex )
		m_pWndDynTab->SelectionSet( nIndex, true, true );
	return DROPEFFECT_NONE;
}

IMPLEMENT_DYNCREATE( CExtDynTabWnd, CExtTabWnd );

BEGIN_MESSAGE_MAP(CExtDynTabWnd, CExtTabWnd)
	//{{AFX_MSG_MAP(CExtDynTabWnd)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CONTEXTMENU()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CExtDynTabWnd::CExtDynTabWnd()
	: m_nSavedIndex( -1L )
{
	m_oleDropTarget.m_pWndDynTab = this;
}

int CExtDynTabWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( CExtTabWnd::OnCreate(lpCreateStruct) == -1 )
	{
		ASSERT( FALSE );
		return -1;
	}
	if( CExtControlBar::g_bAllowDynamicTabsOleDropTarget )
	{
		_AFX_THREAD_STATE * pState = ::AfxGetThreadState();
		ASSERT( pState != NULL );
		if( pState->m_bNeedTerm )
		{ // if AfxOleInit() was called
			VERIFY( m_oleDropTarget.Register( this ) );
		} // if AfxOleInit() was called
	} // if( CExtControlBar::g_bAllowDynamicTabsOleDropTarget )
	return 0;
}

CExtCmdIcon * CExtDynTabWnd::OnTabWndQueryItemIcon(
	const TAB_ITEM_INFO * pTii
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTii );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT( ::IsWindow(GetSafeHwnd()) );

LPARAM lParam = pTii->LParamGet();
CExtControlBar * pBar =
		reinterpret_cast < CExtControlBar * > ( lParam );
	ASSERT( pBar != NULL );
	ASSERT_KINDOF( CExtControlBar, pBar );

UINT nCmdID = pBar->GetDlgCtrlID();

	return
		g_CmdManager->CmdGetIconPtr(
			g_CmdManager->ProfileNameFromWnd( GetSafeHwnd() ),
			nCmdID
			);
}

__EXT_MFC_SAFE_LPCTSTR CExtDynTabWnd::OnTabWndQueryItemText(
	const TAB_ITEM_INFO * pTii
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTii );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT( ::IsWindow(GetSafeHwnd()) );

LPARAM lParam = pTii->LParamGet();
CExtControlBar * pBar =
		reinterpret_cast < CExtControlBar * > ( lParam );
	ASSERT( pBar != NULL );
	ASSERT_KINDOF( CExtControlBar, pBar );

static CString sText;
	pBar->OnGetBarCaptionText( CExtControlBar::__EBCT_TAB_ITEM_CAPTION, sText );
	return (LPCTSTR)sText;
}

bool CExtDynTabWnd::OnTabWndClickedItem(
	LONG nItemIndex,
	bool bButtonPressed,
	INT nMouseButton, // MK_... values
	UINT nMouseEventFlags
	)
{
	if( nMouseButton != MK_LBUTTON )
		return false;
bool bRetVal =
		CExtTabWnd::OnTabWndClickedItem(
			nItemIndex,
			bButtonPressed,
			nMouseButton,
			nMouseEventFlags
			);

	return bRetVal;
}

bool CExtDynTabWnd::OnTabWndStartDrag( LONG nIndex )
{
	ASSERT_VALID( this );
	ASSERT( ItemGetCount() > 0 );
	ASSERT(
		nIndex >= 0
		&& nIndex < ItemGetCount()
		);

CPoint ptCursor( 0, 0 );
	if( ! ::GetCursorPos( &ptCursor ) )
		return false;
//CPoint ptCursorClient( ptCursor );
//	ScreenToClient( &ptCursorClient );
//INT nHitTest = ItemHitTest( ptCursorClient );
//	if( nIndex != nHitTest )
//		return false;

	OnTabWndMouseTrackingPushedStop();
	OnTabWndMouseTrackingHoverStop();

CExtControlBar * pBar = GetBarAt( nIndex, true );
	ASSERT_VALID( pBar );
	ASSERT_KINDOF( CExtControlBar, pBar );
	ASSERT( !pBar->IsFixedMode() );
	ItemGet( nIndex )->ModifyItemStyle( 0, __DYNTAB_ITEM_STYLE_TEMPORARY );
HWND hWndThis = m_hWnd;
	pBar->_DraggingStart( ptCursor );
	if( ::IsWindow( hWndThis ) )
	{
		ASSERT_VALID( this );
		LONG nItemCount = ItemGetCount();
		for( LONG nItem = 0; nItem < nItemCount; nItem++ )
			ItemGet( nItem )->ModifyItemStyle( __DYNTAB_ITEM_STYLE_TEMPORARY, 0 );
	} // if( ::IsWindow( hWndThis ) )
	return true;
}

bool CExtDynTabWnd::OnTabWndSelectionChange(
	LONG nOldItemIndex,
	LONG nNewItemIndex,
	bool bPreSelectionTest
	)
{
	ASSERT_VALID( this );
bool bRetVal =
		CExtTabWnd::OnTabWndSelectionChange(
			nOldItemIndex,
			nNewItemIndex,
			bPreSelectionTest
			);
	if( bPreSelectionTest
		|| nNewItemIndex < 0
		|| ItemGetVisibleCount() == 0
		)
		return bRetVal;

TAB_ITEM_INFO * pTii = ItemGet( nNewItemIndex );
	ASSERT_VALID( pTii );

LPARAM lParam = pTii->LParamGet();
CExtControlBar * pBar =
		reinterpret_cast < CExtControlBar * > ( lParam );
	ASSERT( pBar != NULL );
	ASSERT_KINDOF( CExtControlBar, pBar );

	if( pBar->GetSafeHwnd() == NULL
		|| (! ::IsWindow(pBar->GetSafeHwnd()) )
		)
		return bRetVal;
	

CExtDockDynTabBar * pDynDocker =
		STATIC_DOWNCAST(
			CExtDockDynTabBar,
			pBar->GetParent()
			);
	ASSERT_VALID( pDynDocker );
	pDynDocker->_LockSequenceOptimization( true );
	::SetWindowPos(
		pBar->GetSafeHwnd(),
		HWND_TOP, 0, 0, 0, 0,
		SWP_NOSIZE|SWP_NOMOVE
			//|SWP_NOACTIVATE|SWP_NOSENDCHANGING
			//|SWP_NOREDRAW
		);
	pBar->SetFocus();
	pDynDocker->_LockSequenceOptimization( false );
//	pBar->RedrawWindow(
//		NULL, NULL,
//		RDW_INVALIDATE //|RDW_UPDATENOW
//			|RDW_ERASE //|RDW_ERASENOW
//			|RDW_ALLCHILDREN|RDW_NOFRAME
//		);

CExtDynTabControlBar * pParentBar =
		STATIC_DOWNCAST(
			CExtDynTabControlBar,
			GetParent()
			);
	ASSERT_VALID( pParentBar );
	// if( nOldItemIndex < 0 ) // (- v.2.24)
	{
		pParentBar->_RecalcNcArea();
		CExtDockDynTabBar * pDynDocker =
		STATIC_DOWNCAST(
			CExtDockDynTabBar,
			pParentBar->m_pWndDynDocker
			);
		pDynDocker->_RepositionTabbedChilds( true );
	}


CString sText;
	pBar->GetWindowText( sText );
	pParentBar->SetWindowText( sText );
	pParentBar->m_pDockBar->SetWindowText( sText );

//CDockBar * pPPD = 
//		STATIC_DOWNCAST(
//			CDockBar,
//			pParentBar->GetParent()
//			);
//CFrameWnd * pFrame =
//		DYNAMIC_DOWNCAST(
//			CMiniFrameWnd,
//			pPPD->GetParent()
//			);

CFrameWnd * pFrame = GetParentFrame();
	if( pFrame != NULL )
	{
		pFrame = DYNAMIC_DOWNCAST( CMiniDockFrameWnd, pFrame );
		if( pFrame != NULL )
			pFrame->SendMessage( WM_NCPAINT );
	}

	return bRetVal;
}

void CExtDynTabWnd::InsertControlBar(
	CExtControlBar * pBar,
	INT nPos, // = -1 // default - append
	bool bTempOnDrag, // = false // is tab item temporary (when dragging)
	bool bUpdateTabWnd // = false
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );

	if( FindControlBar(pBar) >= 0 )
	{
		UpdateTabWnd( bUpdateTabWnd );
		return;
	}

LONG nCount = ItemGetCount();
	if( nCount == 0 )
		nPos = -1;
	else if( nPos < 0 || nPos >= nCount )
		nPos = -1;

DWORD dwItemStyle = 0;
	if( bTempOnDrag )
		dwItemStyle |= __DYNTAB_ITEM_STYLE_TEMPORARY;
	if( !pBar->IsVisible() )
		dwItemStyle |= __ETWI_INVISIBLE;

LPARAM lParam = reinterpret_cast < LPARAM > ( pBar );
	
	ItemInsert(
		NULL, NULL, false,
		dwItemStyle,
		nPos,
		lParam,
		bUpdateTabWnd
		);
}

void CExtDynTabWnd::RemoveTemporaryItems(
	bool bUpdateTabWnd // = false
	)
{
	ASSERT_VALID( this );

LONG nCount = ItemGetCount();
	for( LONG nIndex = 0; nIndex < nCount; )
	{
		TAB_ITEM_INFO * pTii = ItemGet( nIndex );
		ASSERT_VALID( pTii );
		DWORD dwItemStyle = pTii->GetItemStyle();
		if( dwItemStyle & __DYNTAB_ITEM_STYLE_TEMPORARY )
		{
			ItemRemove( nIndex );
			nCount--;
			continue;
		}
		nIndex++;
	}

	UpdateTabWnd( bUpdateTabWnd );
//	CExtPaintManager::stat_PassPaintMessages();
}

CExtControlBar * CExtDynTabWnd::GetBarAt(
	LONG nIndex,
	bool bExcludeTemporary
	)
{
	ASSERT_VALID( this );

	ASSERT( ItemGetCount() > 0 );
	ASSERT( 0 <= nIndex && nIndex < ItemGetCount() );

TAB_ITEM_INFO * pTii = ItemGet( nIndex );
	ASSERT( pTii );

	if( bExcludeTemporary )
	{
		DWORD dwItemStyle = pTii->GetItemStyle();
		if( dwItemStyle & __DYNTAB_ITEM_STYLE_TEMPORARY )
			return NULL;
	}

LPARAM lParam = pTii->LParamGet();

CExtControlBar * pBar =
		reinterpret_cast < CExtControlBar * > ( lParam );
	ASSERT( pBar != NULL );
	ASSERT_KINDOF( CExtControlBar, pBar );

	return pBar;
}

LONG CExtDynTabWnd::ReindexPosCalc( CPoint ptCursor )
{
	ASSERT_VALID( this );
	ScreenToClient( &ptCursor );
LONG nHitTest = ItemHitTest( ptCursor );
	if( nHitTest < 0 )
		return (-1L);

	return nHitTest;
}

void CExtDynTabWnd::OnTabWndDrawItem(
	CDC & dc,
	CRect & rcTabItemsArea,
	LONG nItemIndex,
	TAB_ITEM_INFO * pTii,
	bool bTopLeft,
	bool bHorz,
	bool bSelected,
	bool bCenteredText,
	bool bGroupedMode,
	bool bInGroupActive,
	bool bInvertedVerticalMode,
	const CRect & rcEntireItem,
	CSize sizeTextMeasured,
	CFont * pFont,
	__EXT_MFC_SAFE_LPCTSTR sText,
	CExtCmdIcon * pIcon
	)
{
#ifdef _DEBUG
	if( bSelected )
	{
		ASSERT( m_nSelIndex == nItemIndex );
	}
#endif // _DEBUG

	ASSERT( nItemIndex >= 0 && nItemIndex < ItemGetCount() );
	if( bSelected )
	{
		LONG nFirstTempIndex =
			ItemFindByStyle(
				__DYNTAB_ITEM_STYLE_TEMPORARY,
				0,
				-1
				);
		if( nFirstTempIndex >= 0 )
			bSelected = false;
	}

DWORD dwItemStyle = pTii->GetItemStyle();
	if( dwItemStyle & __DYNTAB_ITEM_STYLE_TEMPORARY )
	{
		bSelected = true;
		TAB_ITEM_INFO * pTiiPrev = pTii->GetPrev();
		if( pTiiPrev != NULL )
		{
			DWORD dwPrevItemStyle = pTiiPrev->GetItemStyle();
			if( dwPrevItemStyle & __DYNTAB_ITEM_STYLE_TEMPORARY )
				bSelected = false;
		}
	}

	CExtTabWnd::OnTabWndDrawItem(
		dc,
		rcTabItemsArea,
		nItemIndex,
		pTii,
		bTopLeft,
		bHorz,
		bSelected,
		bCenteredText,
		bGroupedMode,
		bInGroupActive,
		bInvertedVerticalMode,
		rcEntireItem,
		sizeTextMeasured,
		pFont,
		sText,
		pIcon
		);
}

void CExtDynTabWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CExtTabWnd::OnLButtonDblClk(nFlags, point);
LONG nHitTest = ItemHitTest( point );
	if( nHitTest < 0 )
		return;
CExtControlBar * pBar = GetBarAt( nHitTest, true );
	if( pBar == NULL )
		return;
	ASSERT_VALID( pBar );
	pBar->FloatControlBar();
}

void CExtDynTabWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonDown( nFlags, point );
}

void CExtDynTabWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonUp( nFlags, point );
}

void CExtDynTabWnd::OnContextMenu(CWnd* pWnd, CPoint point)
{
	ASSERT_VALID( this );
	pWnd;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
		return;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if( CExtControlBar::FindHelpMode(this) )
		return;

CExtDynTabControlBar * pParentBar =
	STATIC_DOWNCAST(
		CExtDynTabControlBar,
		GetParent()
		);
	ASSERT_VALID( pParentBar );

CFrameWnd * pFrame = pParentBar->m_pDockSite;
	ASSERT_VALID( pFrame );
HWND hWndTrack = pFrame->GetSafeHwnd();
	ASSERT( hWndTrack != NULL && ::IsWindow(hWndTrack) );
CExtPopupMenuWnd * pPopup = new CExtPopupMenuWnd;
	VERIFY( pPopup->CreatePopupMenu(hWndTrack) );

CExtControlBar::POPUP_MENU_EVENT_DATA _pmed(
		CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_DYNCBCTABS_CTX,
		pPopup,
		this,
		point,
		WM_CONTEXTMENU,
		NULL
		);

	if( !_pmed.NotifyTarget( false ) )
	{

		LONG nItemCount = ItemGetCount();
		for( LONG nBar = 0; nBar < nItemCount; nBar ++ )
		{
			TAB_ITEM_INFO * pTII = ItemGet( nBar );
			ASSERT_VALID( pTII );
			CExtControlBar * pExtBar =
				(CExtControlBar *) pTII->LParamGet();
			ASSERT_VALID( pExtBar );
			ASSERT_KINDOF( CExtControlBar, pExtBar );

			UINT nID = pExtBar->GetDlgCtrlID();

			VERIFY( pPopup->ItemInsert( nID ) );
		}

	} // if( !_pmed.NotifyTarget( false ) )
	_pmed.NotifyTarget( true );
	if( pPopup->ItemGetCount() == 0 )
	{
		delete pPopup;
		return;
	}
	::SetFocus( hWndTrack );
	if(	! pPopup->TrackPopupMenu(
			TPMX_OWNERDRAW_FIXED,
			point.x,
			point.y
			)
		)
	{
		delete pPopup;
	}
}

BOOL CExtDynTabWnd::PreTranslateMessage(MSG* pMsg) 
{
	if(		CExtControlBar::g_bAllowKeyboardTabSwitch
		&&	pMsg->message == WM_KEYDOWN
		&&	(	pMsg->wParam == VK_PRIOR
			||	pMsg->wParam == VK_NEXT
			)
		&&	GetSafeHwnd() != NULL
		&&	ItemGetVisibleCount() > 0
		&&	CExtPopupMenuWnd::IsKeyPressed( VK_CONTROL )
		&&	(! CExtPopupMenuWnd::IsKeyPressed( VK_SHIFT ) )
		&&	(! CExtPopupMenuWnd::IsKeyPressed( VK_MENU ) )
		)
	{ // if Ctrl+PageUp or Ctrl+PageDown keydown event
		LONG nSel = SelectionGet();
		if( nSel >= 0L )
		{
			LONG nStep = (pMsg->wParam == VK_PRIOR) ? -1L : 1L;
			LONG nIdx = nSel + nStep;
			LONG nItemCount = ItemGetCount();
			ASSERT( nItemCount > 0L );
			if( nIdx < 0 )
				nIdx = nItemCount - 1L;
			else if( nIdx >= nItemCount )
				nIdx = 0;
			for( ; nIdx != nSel;  )
			{
				const TAB_ITEM_INFO * pTii = ItemGet( nIdx );
				ASSERT( pTii != NULL );
				if(		pTii->VisibleGet()
					&&	((pTii->GetItemStyle())&__DYNTAB_ITEM_STYLE_TEMPORARY) == 0
					)
				{
					LPARAM lParam = pTii->LParamGet();
					CExtControlBar * pBar =
						reinterpret_cast < CExtControlBar * > ( lParam );
					ASSERT( pBar != NULL );
					ASSERT_KINDOF( CExtControlBar, pBar );
					if( (pBar->GetStyle()&WS_VISIBLE) != 0 )
					{
						CWnd::CancelToolTips();
						SelectionSet( nIdx, true, true );
						break;
					} // if( (pBar->GetStyle()&WS_VISIBLE) != 0 )
				} // if( pTii->IsVisible() ...
				nIdx += nStep;
				if( nIdx < 0 )
					nIdx = nItemCount - 1L;
				else if( nIdx >= nItemCount )
					nIdx = 0;
			} // for( ; nIdx != nSel;  )
			return TRUE;
		} // if( nSel >= 0L )
	} // if Ctrl+PageUp or Ctrl+PageDown keydown event
	return CExtTabWnd::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////
// CExtDynTabControlBar window

IMPLEMENT_DYNCREATE( CExtDynTabControlBar, CExtDynControlBar );

BEGIN_MESSAGE_MAP(CExtDynTabControlBar, CExtDynControlBar)
	//{{AFX_MSG_MAP(CExtDynTabControlBar)
	ON_WM_CREATE()
	ON_WM_NCCALCSIZE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CExtDynTabControlBar::CExtDynTabControlBar()
{
}

bool CExtDynTabControlBar::IsBarWithGripper(
	bool * pbGripperAtTop, // = NULL
	bool * pbTextOnGripper // = NULL
	) const
{
	if( pbGripperAtTop != NULL )
		*pbGripperAtTop = false;
	if( pbTextOnGripper != NULL )
		*pbTextOnGripper = false;

	if( ((CExtControlBar *)this)->_IsSingleVisibleInFloatingPalette() )
		return false;

	if( pbGripperAtTop != NULL )
		*pbGripperAtTop = true;
	if( pbTextOnGripper != NULL )
		*pbTextOnGripper = true;
	return true;
}

CSize CExtDynTabControlBar::_CalcLayoutMinSize() const
{
	ASSERT_VALID( this );
CExtDockDynBar * pDockBar = 
		STATIC_DOWNCAST( CExtDockDynBar, GetWindow(GW_CHILD) );
	ASSERT_VALID( pDockBar );
	ASSERT_KINDOF( CExtDockDynBar, pDockBar );

CSize _size(
		CExtControlBar::_CalcDesiredMinHW(),
		CExtControlBar::_CalcDesiredMinVH()
		);

INT nCountOfBars = pDockBar->m_arrBars.GetSize();
	ASSERT( nCountOfBars > 0 );
	ASSERT( pDockBar->m_arrBars[0] == NULL );
	for( INT nBar = 1; nBar < nCountOfBars; nBar++ )
	{
		CExtControlBar * pBar = (CExtControlBar *)
				pDockBar->m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CExtControlBar, pBar );
		ASSERT( !pBar->IsFixedMode() );
		if( !pBar->IsVisible() )
			continue;
		CSize _sizeBar(
			pBar->_CalcDesiredMinHW(),
			pBar->_CalcDesiredMinVH()
			);
		_size.cx = max( _size.cx, _sizeBar.cx );
		_size.cy = max( _size.cy, _sizeBar.cy );
	}
	return _size;
}

INT CExtDynTabControlBar::_CalcTabHeight() const
{

CRect rcInnerRest;
	GetClientRect( &rcInnerRest );

CRect rcTabWnd =
		((CExtTabWnd *)(&m_wndTabSwitcher))->CalcPreviewLayout( rcInnerRest );

CSize sizeTab = rcTabWnd.Size();
	
	sizeTab.cy +=
		__EXTTAB_MARGIN_BORDER_HY*2
		+ 2
		;

	return sizeTab.cy;
}

INT CExtDynTabControlBar::_CalcDesiredMinHW() const
{
	return _CalcLayoutMinSize().cx;
}

INT CExtDynTabControlBar::_CalcDesiredMinVH() const
{
	return _CalcLayoutMinSize().cy;
}

CSize CExtDynTabControlBar::_CalcDesiredMinFloatedSize() const
{
	return _CalcLayoutMinSize();
}

CSize CExtDynTabControlBar::CalcDynamicLayout(
	int nLength,
	DWORD nMode
	)
{
	return
		CExtDynControlBar::CalcDynamicLayout(
			nLength,
			nMode
			);
}

void CExtDynTabControlBar::OnRepositionSingleChild(
	int cx, // = -1 // if negative - get from client area
	int cy, // = -1
	bool bRedraw // = true
	)
{
	if(		m_wndTabSwitcher.GetSafeHwnd() == NULL
		||	m_pWndDynDocker->GetSafeHwnd() == NULL
		)
		return;
CRect rcClient;
	if( cx < 0 || cy < 0 )
		GetClientRect( &rcClient );
	else
		rcClient.SetRect( 0, 0, cx, cy );
CWnd * pWndParent = GetParent();
	if(		pWndParent->IsKindOf(RUNTIME_CLASS(CDockBar))
		&&	pWndParent->GetDlgCtrlID() == AFX_IDW_DOCKBAR_FLOAT
		)
		rcClient.DeflateRect( 1, 1 );
INT nTabHeight = _CalcTabHeight();
CRect rcTabWnd = rcClient;
	rcTabWnd.top = rcTabWnd.bottom - nTabHeight - 1;
	m_wndTabSwitcher.MoveWindow( &rcTabWnd, FALSE );
bool bTabVisible =
		(m_wndTabSwitcher.GetStyle() & WS_VISIBLE) ? true : false;
	if( m_wndTabSwitcher.ItemGetVisibleCount() < 2 )
	{
		if( bTabVisible )
			m_wndTabSwitcher.ShowWindow( SW_HIDE );
	} // if( m_wndTabSwitcher.ItemGetVisibleCount() < 2 )
	else
	{
		if( ! bTabVisible )
			m_wndTabSwitcher.ShowWindow( SW_SHOW );
		rcClient.bottom -= nTabHeight + 1;
	} // else from if( m_wndTabSwitcher.ItemGetVisibleCount() < 2 )
	m_pWndDynDocker->MoveWindow( rcClient, FALSE );
	( STATIC_DOWNCAST( CExtDockDynTabBar, m_pWndDynDocker ) )
		-> _RepositionTabbedChilds( true );
	m_wndTabSwitcher.UpdateTabWnd( false );
	m_wndTabSwitcher.Invalidate();
	if( ! bRedraw )
		return;
	m_pWndDynDocker->OnDynamicLayoutUpdate();
}

CExtDockDynBar * CExtDynTabControlBar::OnCreateDynamicDockBarObject()
{
	return ( new CExtDockDynTabBar( 0 ) );
}

int CExtDynTabControlBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if( CExtDynControlBar::OnCreate(lpCreateStruct) == -1 )
	{
		ASSERT( FALSE );
		return -1;
	}

	ASSERT( m_pWndDynDocker != NULL );
	ASSERT_KINDOF( CExtDockDynTabBar, m_pWndDynDocker );
	ASSERT( m_pWndDynDocker->GetSafeHwnd() != NULL );
	ASSERT( ::IsWindow(m_pWndDynDocker->GetSafeHwnd()) );

CRect rcTabSwitcher;
	GetClientRect( &rcTabSwitcher );
	if( rcTabSwitcher.right <= rcTabSwitcher.left )
		rcTabSwitcher.right = rcTabSwitcher.left + 10;
	rcTabSwitcher.top =
		rcTabSwitcher.bottom
		- 2
		- __EXTTAB_BTN_MIN_DY
		;

	if( !m_wndTabSwitcher.Create(
			this,
			rcTabSwitcher,
			UINT( IDC_STATIC ),
			WS_CHILD | WS_VISIBLE
				//| WS_CLIPSIBLINGS | WS_CLIPCHILDREN
				,
			__ETWS_ORIENT_BOTTOM | __ETWS_EQUAL_WIDTHS
			)
		)
	{
		ASSERT( FALSE );
		return -1;
	}

	return 0;
}

void CExtDynTabControlBar::OnGetBarCaptionText(
	CExtControlBar::e_bar_caption_text_t eBCT,
	CExtSafeString & strCaptionText
	) const
{
	ASSERT_VALID( this );
	strCaptionText.Empty();
	if( GetSafeHwnd() == NULL )
		return;
CExtControlBar * pBar = NULL;
LONG nSelIdx = GetSwitcherSelection();
	if( nSelIdx < 0 )
	{
		if(		GetSwitcherItemCount() == 0
			||	m_pWndDynDocker->GetSafeHwnd() == NULL
			)
			return;
		CWnd * pWnd = m_pWndDynDocker->GetWindow( GW_CHILD );
		if( pWnd != NULL )
			pBar =
				DYNAMIC_DOWNCAST(
					CExtControlBar,
					pWnd
					);
	} // if( nSelIdx < 0 )
	else
		pBar =
			( const_cast < CExtDynTabControlBar * > ( this ) )
				-> GetBarAt( nSelIdx, false );
	if( pBar == NULL )
		return;
	ASSERT_VALID( pBar );
	if( eBCT == __EBCT_SINGLE_CAPTION_DOCKED )
		eBCT = __EBCT_TAB_GROUP_CAPTION_DOCKED;
	else if( eBCT == __EBCT_SINGLE_CAPTION_FLOATING )
		eBCT = __EBCT_TAB_GROUP_CAPTION_FLOATING;
	pBar->OnGetBarCaptionText(
		eBCT,
		strCaptionText
		);
}

void CExtDynTabControlBar::OnSetFocus(CWnd* pOldWnd) 
{
//	CExtDynControlBar::OnSetFocus(pOldWnd);
	CWnd::OnSetFocus(pOldWnd);
	
	if( m_wndTabSwitcher.ItemGetCount() == 0 )
		return;
LONG nSel = m_wndTabSwitcher.SelectionGet();
	if( nSel < 0 )
		return;
CExtControlBar * pBar = m_wndTabSwitcher.GetBarAt( nSel, true );
	if( pBar == NULL )
		return;
	ASSERT_VALID( pBar );
	if( (pBar->GetStyle() & WS_VISIBLE) == 0 )
		return;

	pBar->SetFocus();
}

void CExtDynTabControlBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	if( IsFloating() )
	{
		CRect & rcClientLoc =
			reinterpret_cast < CRect & > ( lpncsp->rgrc[0] );
		rcClientLoc.DeflateRect( 1, 1 );
		return;
	}
	CExtControlBar::OnNcCalcSize( bCalcValidRects, lpncsp );
}

void CExtDynTabControlBar::InsertTemporaryItemsFrom(
	CExtControlBar * pBar,
	LONG nIndex,
	bool bUpdateTabWnd
	)
{
	ASSERT_VALID( this );
	ASSERT_KINDOF( CExtDynTabControlBar, this );
	ASSERT_VALID( pBar );
	ASSERT( !pBar->IsFixedMode() );

LONG nCount = m_wndTabSwitcher.ItemGetCount();
	if( nIndex < 0 || nIndex >= nCount )
		nIndex = nCount;
	_InsertTemporaryItemsFromImpl( pBar, nIndex );
	m_wndTabSwitcher.UpdateTabWnd( bUpdateTabWnd );
//	CExtPaintManager::stat_PassPaintMessages();
}

void CExtDynTabControlBar::_InsertTemporaryItemsFromImpl(
	CExtControlBar * pBar,
	LONG & nIndex
	)
{
	ASSERT_VALID( this );
	ASSERT_KINDOF( CExtDynTabControlBar, this );
	ASSERT_VALID( pBar );
	ASSERT( !pBar->IsFixedMode() );

	if( pBar->IsKindOf( RUNTIME_CLASS(CExtDynControlBar)) )
	{
		CExtDockDynBar * pDynDocker =
			((CExtDynControlBar *)pBar) ->
				m_pWndDynDocker;

		ASSERT_VALID( pDynDocker );
		ASSERT( pDynDocker->m_arrBars.GetSize() > 0 );
		ASSERT( pDynDocker->m_arrBars[0] == NULL );

		ExtControlBarVector_t vBars;
		pDynDocker->CalcOrderedVector( vBars );
		INT nCount = vBars.GetSize();
		for( INT nBar = 0; nBar < nCount; nBar ++ )
		{
			CExtControlBar * pInnerBar = vBars[nBar];
			ASSERT_VALID( pInnerBar );
			ASSERT( !pInnerBar->IsFixedMode() );

			if( !pInnerBar->IsVisible() )
				continue;
			_InsertTemporaryItemsFromImpl( pInnerBar, nIndex );
		} // for( INT nBar = 0; nBar < nCount; nBar ++ )
	} // if( pBar->IsKindOf( RUNTIME_CLASS(CExtDynControlBar)) )
	else
	{
///		ASSERT( pBar->IsVisible() );
		m_wndTabSwitcher.InsertControlBar(
			pBar,
			nIndex,
			true,
			false
			);
		nIndex ++;
	} // else from if( pBar->IsKindOf( RUNTIME_CLASS(CExtDynControlBar)) )
	
//	CExtPaintManager::stat_PassPaintMessages();
}

void CExtDynTabControlBar::InsertBar( 
	CExtControlBar * pBar,
	LONG nIndex,
	bool bUpdateTabWnd
	)
{
	ASSERT_VALID( this );
	ASSERT_KINDOF( CExtDynTabControlBar, this );
	ASSERT_VALID( pBar );
	ASSERT( !pBar->IsFixedMode() );

	m_wndTabSwitcher.RemoveTemporaryItems( false );
INT nCountOfTabs = m_wndTabSwitcher.ItemGetCount();
	if( nIndex < 0 )
		nIndex = nCountOfTabs;
	else if( nIndex >= nCountOfTabs )
		nIndex = nCountOfTabs;

	_InsertBarImpl( 
		pBar,
		nIndex
		);

	m_wndTabSwitcher.UpdateTabWnd( bUpdateTabWnd );

	if( bUpdateTabWnd )
	{
//		ASSERT_VALID( m_pDockBar );
//		ASSERT_KINDOF( CExtDockBar, m_pDockBar );
//		((CExtDockBar *)m_pDockBar)->_OptimizeCircles();
		ASSERT_VALID( m_pDockSite );
		CExtDockBar::_OptimizeCircles( m_pDockSite );

		OnRepositionSingleChild();
		CExtPaintManager::stat_PassPaintMessages();
	}
}

void CExtDynTabControlBar::_InsertBarImpl( 
	CExtControlBar * pBar,
	LONG & nIndex
	)
{
	ASSERT_VALID( this );
	ASSERT_KINDOF( CExtDynTabControlBar, this );
	ASSERT_VALID( pBar );
	ASSERT( !pBar->IsFixedMode() );

CExtDockBar * pExtDockBar = NULL;
	if( m_pDockBar != NULL )
		pExtDockBar = 
			DYNAMIC_DOWNCAST( CExtDockBar, m_pDockBar );
bool bUnlockDockBarOptimization = false;
	if( pExtDockBar != NULL )
	{
		if( !pExtDockBar->m_bLockedOptimize )
		{
			pExtDockBar->m_bLockedOptimize = true;
			bUnlockDockBarOptimization = true;
		}
	}

	if( pBar->IsKindOf( RUNTIME_CLASS(CExtDynControlBar)) )
	{
		CExtDockDynBar * pDynDocker =
			((CExtDynControlBar *)pBar) ->
				m_pWndDynDocker;

		ASSERT_VALID( pDynDocker );
		ASSERT( pDynDocker->m_arrBars.GetSize() > 0 );
		ASSERT( pDynDocker->m_arrBars[0] == NULL );

		pDynDocker->RemoveAllPlaceHolders();
		ExtControlBarVector_t vBars;
		pDynDocker->CalcOrderedVector( vBars );
		INT nCount = vBars.GetSize();
		for( INT nBar = 0; nBar < nCount; nBar ++ )
		{
			CExtControlBar * pInnerBar = vBars[nBar];
			ASSERT_VALID( pInnerBar );
			ASSERT( !pInnerBar->IsFixedMode() );

//			if( !pInnerBar->IsVisible() )
//				continue;
			
			CExtDockBar * pExtInnerDockBar = NULL;
			if( pInnerBar->m_pDockBar != NULL )
				pExtInnerDockBar = 
					DYNAMIC_DOWNCAST(
						CExtDockBar,
						pInnerBar->m_pDockBar
						);
			if( pExtInnerDockBar != NULL )
				pExtInnerDockBar->m_bLockedOptimize = true;
			
			_InsertBarImpl( pInnerBar, nIndex );

			if( pExtInnerDockBar != NULL )
				pExtInnerDockBar->m_bLockedOptimize = false;

		} // for( INT nBar = 0; nBar < nCount; nBar ++ )
	} // if( pBar->IsKindOf( RUNTIME_CLASS(CExtDynControlBar)) )
	else
	{
		ASSERT_VALID( m_pWndDynDocker );
		BOOL bNewBarHereIsVisible = pBar->IsVisible();
		if( bNewBarHereIsVisible )
			pBar->ShowWindow( SW_HIDE );
		m_pWndDynDocker->DockControlBar( pBar );
		ASSERT( pBar->GetParent() == m_pWndDynDocker );
		if( ! m_pWndDynDocker->m_bLockedOptimize )
		{
			CRect rcBar;
			m_pWndDynDocker->GetClientRect( &rcBar );
			if(		bNewBarHereIsVisible
				&&	(m_wndTabSwitcher.GetStyle()&WS_VISIBLE) == 0
				)
			{
				INT nTabHeight = _CalcTabHeight();
				if( nTabHeight > 0 )
					rcBar.bottom -= nTabHeight + 1;
			}
			::SetWindowPos(
				pBar->GetSafeHwnd(),
				HWND_TOP,
				rcBar.left, rcBar.top,
				rcBar.Width(), rcBar.Height(),
				0
				);
			pBar->OnRepositionSingleChild();
		} // if( ! m_pWndDynDocker->m_bLockedOptimize )
		if( bNewBarHereIsVisible )
			pBar->ShowWindow( SW_SHOWNA );

// temp
CExtDockBar::_RemovePlaceHolder( m_pDockSite, pBar );

		m_wndTabSwitcher.InsertControlBar( pBar, nIndex, false, false );
		nIndex++;
	} // else from if( pBar->IsKindOf( RUNTIME_CLASS(CExtDynControlBar)) )

	if( bUnlockDockBarOptimization )
	{
		ASSERT( pExtDockBar != NULL );
		pExtDockBar->m_bLockedOptimize = false;
	}
	
}

void CExtDynTabControlBar::SyncSwitcher()
{
	ASSERT_VALID( this );
LONG nCount = m_wndTabSwitcher.ItemGetCount();
	if( nCount <= 1 )
		return;
LONG nSel = m_wndTabSwitcher.SelectionGet();
	if( nSel >= 0 )
		return;

CExtDockDynTabBar * pTabbedDockBar =
		STATIC_DOWNCAST(
			CExtDockDynTabBar,
			GetWindow( GW_CHILD )
			);
CWnd * pWndTop = pTabbedDockBar->GetTopWindow();
	if( pWndTop == NULL )
		return;
CExtControlBar * pBar =
		STATIC_DOWNCAST(
			CExtControlBar,
			pWndTop
			);
#ifdef _DEBUG
	nSel = FindControlBar( pBar );
	ASSERT( nSel >= 0 );
#endif // _DEBUG
	SelectControlBar( pBar );
}

void CExtDynTabControlBar::RemoveSelFromSwitcher()
{
	ASSERT_VALID( this );
LONG nCount = m_wndTabSwitcher.ItemGetCount();
	if( nCount == 0 )
		return;
LONG nSel = m_wndTabSwitcher.SelectionGet();
	if( nSel < 0 )
		return;
	m_wndTabSwitcher.ItemRemove( nSel );
}

void CExtDynTabControlBar::UpdateBarVisibility(
	CExtControlBar * pBar
	)
{
	if( m_pDockBar == NULL && (!m_bPresubclassDialogMode) )
		return; // on destruction

	ASSERT_VALID( this );
	ASSERT_KINDOF( CExtDynTabControlBar, this );
	ASSERT_VALID( pBar );
	ASSERT( !pBar->IsFixedMode() );

	BOOL bVisible = pBar->IsVisible();

	ASSERT( m_wndTabSwitcher.ItemGetCount() > 0 );
LONG nBarIndexInTab =
		m_wndTabSwitcher.FindControlBar( pBar );
	ASSERT(
		nBarIndexInTab >= 0
		&& nBarIndexInTab < m_wndTabSwitcher.ItemGetCount()
		);

LONG nSelIndex = m_wndTabSwitcher.SelectionGet();
	if( !bVisible && nSelIndex == nBarIndexInTab )
	{
		CWnd * pNextTestWnd = pBar->GetWindow( GW_HWNDNEXT );
		for(	; pNextTestWnd != NULL;
				pNextTestWnd = pNextTestWnd->GetWindow( GW_HWNDNEXT )
				)
		{
			CExtControlBar * pNextBar =
				STATIC_DOWNCAST( CExtControlBar, pNextTestWnd );
			ASSERT_VALID( pNextBar );
			ASSERT( !pNextBar->IsFixedMode() );
			if( pNextBar->IsVisible() )
				break;
		}
		if( pNextTestWnd == NULL )
		{
			m_wndTabSwitcher.SelectionSet( -1, false, false );
		}
		else
		{
			CExtControlBar * pNextBar =
				STATIC_DOWNCAST( CExtControlBar, pNextTestWnd );
			ASSERT_VALID( pNextBar );
			ASSERT( !pNextBar->IsFixedMode() );
			ASSERT( pNextBar->IsVisible() );
			LONG nIdxNextBar = m_wndTabSwitcher.FindControlBar( pNextBar );
			ASSERT(
				nIdxNextBar >= 0
				&& nIdxNextBar < m_wndTabSwitcher.ItemGetCount()
				);
			m_wndTabSwitcher.SelectionSet( nIdxNextBar, false, false );
		}
	} // if( !bVisible && nSelIndex == nBarIndexInTab )

CExtTabWnd::TAB_ITEM_INFO * pTii =
		m_wndTabSwitcher.ItemGet( nBarIndexInTab );
	ASSERT_VALID( pTii );
bool bAdjustSelection = false;
	if( bVisible && !pTii->VisibleGet() )
		bAdjustSelection = true;

	pTii->ModifyItemStyle(
		bVisible ? __ETWI_INVISIBLE : 0,
		bVisible ? 0 : __ETWI_INVISIBLE
		);

	if( bAdjustSelection )
		m_wndTabSwitcher.SelectionSet( nBarIndexInTab, false, false );
	
	m_wndTabSwitcher.UpdateTabWnd( true );

CFrameWnd * pParentFrame = pBar->GetParentFrame();
	if( pParentFrame->IsKindOf( RUNTIME_CLASS(CMiniFrameWnd) ) )
		pParentFrame->DelayRecalcLayout();

	OnRepositionSingleChild();
}

bool CExtDynTabControlBar::_CanDockToTabbedContainers(
	CExtControlBar * pDestBar
	) const
{
	ASSERT_VALID( this );
	return CExtDynControlBar::_CanDockToTabbedContainers( pDestBar );
}

CExtDynTabControlBar * CExtDynTabControlBar::_GetNearestTabbedContainer()
{
	ASSERT_VALID( this );
	return this;
}

BOOL CExtDynTabControlBar::PreTranslateMessage(MSG* pMsg) 
{
	if( m_wndTabSwitcher.PreTranslateMessage(pMsg) )
		return TRUE;
	return CExtDynControlBar::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////
// CExtDynAutoHideArea window

DROPEFFECT CExtDynAutoHideArea::CDynDropTarget::OnDragOver(
	CWnd * pWnd,
	COleDataObject * pDataObject,
	DWORD dwKeyState,
	CPoint point
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pWndAutoHideArea );
	ASSERT_VALID( m_pWndSlider );
	pWnd;
	pDataObject;
	dwKeyState;
	point;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_pWndAutoHideArea->m_hWnd );
	if( pSite != NULL )
	{
		if( pSite->IsCustomizeMode() )
			return DROPEFFECT_NONE;
		if( pSite->DraggedNodeGet() != NULL )
			return DROPEFFECT_NONE;
	} // if( pSite != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	
//LONG nIndex = m_pWndAutoHideArea->ItemHitTest( point );
//	if( nIndex >= 0 )
//	{
//		ASSERT( nIndex < m_pWndAutoHideArea->ItemGetCount() );
//		LONG nSel = m_pWndAutoHideArea->SelectionGet();
//		if( nSel != nIndex )
//			m_pWndAutoHideArea->SelectionSet( nIndex, true, true );
//		return DROPEFFECT_NONE;
//	} // if( nIndex >= 0 )
//	if( m_pWndSlider->m_bActiveState
//		&& !m_pWndSlider->m_bAnimation
//		)
//		m_pWndSlider->PostMessage( WM_CANCELMODE );
//	return DROPEFFECT_NONE;

LONG nIndex = m_pWndAutoHideArea->ItemHitTest( point );
	if( nIndex < 0 )
		return DROPEFFECT_NONE;
	ASSERT( nIndex < m_pWndAutoHideArea->ItemGetCount() );
LONG nSel = m_pWndAutoHideArea->SelectionGet();
	if( nSel != nIndex )
		m_pWndAutoHideArea->SelectionSet( nIndex, true, true );
	return DROPEFFECT_NONE;
}

IMPLEMENT_DYNCREATE( CExtDynAutoHideArea, CExtTabWnd );

BEGIN_MESSAGE_MAP(CExtDynAutoHideArea, CExtTabWnd)
	//{{AFX_MSG_MAP(CExtDynAutoHideArea)
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
END_MESSAGE_MAP()

CExtDynAutoHideArea::CExtDynAutoHideArea()
	: m_pWndSlider( NULL )
	, m_bActivationLocked( false )
	, m_bInsertMode( false )
	, m_rcLastPos( 0, 0, 0, 0 )
	, m_bCancelNearestActivateAnimation( false )
{
	m_oleDropTarget.m_pWndAutoHideArea = this;
}

BOOL CExtDynAutoHideArea::Create(
	CFrameWnd * pParentWnd,
	CExtDynAutoHideSlider * pWndSlider,
	UINT nDockedDockBarID
	)
{
	ASSERT_VALID( pParentWnd );
	ASSERT_VALID( pWndSlider );
	ASSERT( pWndSlider->GetSafeHwnd() != NULL );
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockedDockBarID );

	m_pWndSlider = pWndSlider;

UINT nOrientStylePart = __ETWS_ORIENT_TOP;
	switch( nDockedDockBarID )
	{
	case AFX_IDW_DOCKBAR_TOP:
		break;
	case AFX_IDW_DOCKBAR_BOTTOM:
		nOrientStylePart = __ETWS_ORIENT_BOTTOM;
		break;
	case AFX_IDW_DOCKBAR_LEFT:
		nOrientStylePart = __ETWS_ORIENT_LEFT;
		break;
	case AFX_IDW_DOCKBAR_RIGHT:
		nOrientStylePart = __ETWS_ORIENT_RIGHT;
		break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
		break;
#endif // _DEBUG
	}; // switch( nDockedDockBarID )
	if( ! CExtTabWnd::Create(
			pParentWnd,
			CRect( 0, 0, 0, 0 ),
			nDockedDockBarID,
			WS_CHILD | WS_VISIBLE, //  | WS_VISIBLE
			__ETWS_GROUPED
				| __ETWS_HOVER_FOCUS
				| __ETWS_AUTOHIDE_SCROLL
				| nOrientStylePart
			)
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}

CExtDockBar * pDockBar =
		STATIC_DOWNCAST(
			CExtDockBar,
			pParentWnd->GetControlBar( nDockedDockBarID )
			);
	ASSERT_VALID( pDockBar );
	ASSERT( pDockBar->_GetCircleNo() == 0 );
	ASSERT( pDockBar->m_pWndAutoHideArea == NULL );

	for( ; pDockBar != NULL; pDockBar = pDockBar->m_pDockBarInner)
	{
		ASSERT_VALID( pDockBar );
		ASSERT_KINDOF( CExtDockBar, pDockBar );
		ASSERT( pDockBar->m_pWndAutoHideArea == NULL );
		pDockBar->m_pWndAutoHideArea = this;
	}

	AdjustOrder();

	if( CExtControlBar::g_bAllowAutohideTabsOleDropTarget )
	{
		_AFX_THREAD_STATE * pState = ::AfxGetThreadState();
		ASSERT( pState != NULL );
		if( pState->m_bNeedTerm )
		{ // if AfxOleInit() was called
			m_oleDropTarget.m_pWndSlider = m_pWndSlider;
			VERIFY( m_oleDropTarget.Register( this ) );
		} // if AfxOleInit() was called
	} // if( CExtControlBar::g_bAllowAutohideTabsOleDropTarget )

	return TRUE;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
void CExtDynAutoHideArea::OnCustomizeModeEnter()
{
	ASSERT_VALID( this );
	if( m_pWndSlider == NULL )
		return;
	ASSERT_VALID( m_pWndSlider );
	m_pWndSlider->OnCustomizeModeEnter( this );
}

void CExtDynAutoHideArea::OnCustomizeModeLeave()
{
	ASSERT_VALID( this );
	if( m_pWndSlider == NULL )
		return;
	ASSERT_VALID( m_pWndSlider );
	m_pWndSlider->OnCustomizeModeLeave( this );
}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

void CExtDynAutoHideArea::AdjustOrder()
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT( ::IsWindow(GetSafeHwnd()) );

UINT nDockedDockBarID = GetDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockedDockBarID );

CFrameWnd * pParentWnd =
		STATIC_DOWNCAST( CFrameWnd, GetParent() );

	switch( nDockedDockBarID )
	{
	case AFX_IDW_DOCKBAR_TOP:
		{
			CExtDockBar * pDockBarRight =
				STATIC_DOWNCAST(
					CExtDockBar,
					pParentWnd->GetControlBar( AFX_IDW_DOCKBAR_RIGHT )
					);
			ASSERT_VALID( pDockBarRight );
			ASSERT( pDockBarRight->_GetCircleNo() == 0 );
			if( pDockBarRight->GetWindow(GW_HWNDNEXT) == this )
				return;
			VERIFY(
				::SetWindowPos(
					m_hWnd,
					pDockBarRight->m_hWnd,
					0, 0, 0, 0,
					SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE
						|SWP_NOCOPYBITS|SWP_NOREDRAW
						|SWP_NOSENDCHANGING
					)
				);
		}
		break;
	case AFX_IDW_DOCKBAR_BOTTOM:
		{
			CExtDockBar * pDockBarTop =
				STATIC_DOWNCAST(
					CExtDockBar,
					pParentWnd->GetControlBar( AFX_IDW_DOCKBAR_TOP )
					);
			ASSERT_VALID( pDockBarTop );
			ASSERT( pDockBarTop->_GetCircleNo() == 0 );
			ASSERT_VALID( pDockBarTop->m_pWndAutoHideArea );
			ASSERT(
				pDockBarTop->m_pWndAutoHideArea->GetSafeHwnd() != NULL
				);
			ASSERT(
				::IsWindow( pDockBarTop->m_pWndAutoHideArea->GetSafeHwnd() )
				);
			ASSERT(
				pDockBarTop->m_pWndAutoHideArea->GetDlgCtrlID()
					== AFX_IDW_DOCKBAR_TOP
				);
			if( pDockBarTop->m_pWndAutoHideArea->GetWindow(GW_HWNDNEXT) == this )
				return;
			VERIFY(
				::SetWindowPos(
					m_hWnd,
					pDockBarTop->m_pWndAutoHideArea->m_hWnd,
					0, 0, 0, 0,
					SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE
						|SWP_NOCOPYBITS|SWP_NOREDRAW
						|SWP_NOSENDCHANGING
					)
				);
		}
		break;
	case AFX_IDW_DOCKBAR_LEFT:
		{
			CExtDockBar * pDockBarBottom =
				STATIC_DOWNCAST(
					CExtDockBar,
					pParentWnd->GetControlBar( AFX_IDW_DOCKBAR_BOTTOM )
					);
			ASSERT_VALID( pDockBarBottom );
			ASSERT( pDockBarBottom->_GetCircleNo() == 0 );
			ASSERT_VALID( pDockBarBottom->m_pWndAutoHideArea );
			ASSERT(
				pDockBarBottom->m_pWndAutoHideArea->GetSafeHwnd() != NULL
				);
			ASSERT(
				::IsWindow( pDockBarBottom->m_pWndAutoHideArea->GetSafeHwnd() )
				);
			ASSERT(
				pDockBarBottom->m_pWndAutoHideArea->GetDlgCtrlID()
					== AFX_IDW_DOCKBAR_BOTTOM
				);
			if( pDockBarBottom->m_pWndAutoHideArea->GetWindow(GW_HWNDNEXT) == this )
				return;
			VERIFY(
				::SetWindowPos(
					m_hWnd,
					pDockBarBottom->m_pWndAutoHideArea->m_hWnd,
					0, 0, 0, 0,
					SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE
						|SWP_NOCOPYBITS|SWP_NOREDRAW
						|SWP_NOSENDCHANGING
					)
				);
		}
		break;
	case AFX_IDW_DOCKBAR_RIGHT:
		{
			CExtDockBar * pDockBarLeft =
				STATIC_DOWNCAST(
					CExtDockBar,
					pParentWnd->GetControlBar( AFX_IDW_DOCKBAR_LEFT )
					);
			ASSERT_VALID( pDockBarLeft );
			ASSERT( pDockBarLeft->_GetCircleNo() == 0 );
			ASSERT_VALID( pDockBarLeft->m_pWndAutoHideArea );
			ASSERT(
				pDockBarLeft->m_pWndAutoHideArea->GetSafeHwnd() != NULL
				);
			ASSERT(
				::IsWindow( pDockBarLeft->m_pWndAutoHideArea->GetSafeHwnd() )
				);
			ASSERT(
				pDockBarLeft->m_pWndAutoHideArea->GetDlgCtrlID()
					== AFX_IDW_DOCKBAR_LEFT
				);
			if( pDockBarLeft->m_pWndAutoHideArea->GetWindow(GW_HWNDNEXT) == this )
				return;
			VERIFY(
				::SetWindowPos(
					m_hWnd,
					pDockBarLeft->m_pWndAutoHideArea->m_hWnd,
					0, 0, 0, 0,
					SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE
						|SWP_NOCOPYBITS|SWP_NOREDRAW
						|SWP_NOSENDCHANGING
					)
				);
		}
		break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
		break;
#endif // _DEBUG
	}; // switch( nDockedDockBarID )

}

CExtCmdIcon * CExtDynAutoHideArea::OnTabWndQueryItemIcon(
	const TAB_ITEM_INFO * pTii
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTii );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT( ::IsWindow(GetSafeHwnd()) );

LPARAM lParam = pTii->LParamGet();
CExtControlBar * pBar =
		reinterpret_cast < CExtControlBar * > ( lParam );
	ASSERT( pBar != NULL );
	ASSERT_KINDOF( CExtControlBar, pBar );

UINT nCmdID = pBar->GetDlgCtrlID();

	return
		g_CmdManager->CmdGetIconPtr(
			g_CmdManager->ProfileNameFromWnd( GetSafeHwnd() ),
			nCmdID
			);
}

__EXT_MFC_SAFE_LPCTSTR CExtDynAutoHideArea::OnTabWndQueryItemText(
	const TAB_ITEM_INFO * pTii
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTii );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT( ::IsWindow(GetSafeHwnd()) );

LPARAM lParam = pTii->LParamGet();
CExtControlBar * pBar =
		reinterpret_cast < CExtControlBar * > ( lParam );
	ASSERT( pBar != NULL );
	ASSERT_KINDOF( CExtControlBar, pBar );

static CExtSafeString sText;
	pBar->OnGetBarCaptionText( CExtControlBar::__EBCT_AUTOHIDE_ITEM_CAPTION, sText );
	return (LPCTSTR)sText;
}

CExtDynAutoHideArea * CExtDynAutoHideArea::stat_FindAreaOfBar(
	CExtControlBar * pBar,
	LONG * p_nSelIdx // = NULL
	)
{
	ASSERT_VALID( pBar );
	ASSERT_KINDOF( CExtControlBar, pBar );
	ASSERT( pBar->GetSafeHwnd() != NULL );
	ASSERT( ::IsWindow( pBar->GetSafeHwnd() ) );
	if( p_nSelIdx != NULL )
		(*p_nSelIdx) = -1L;
CExtDynAutoHideArea
		*pAreaTop, *pAreaBottom, *pAreaLeft, *pAreaRight;
	stat_FindAreas(
		pBar->GetSafeHwnd(),
		&pAreaTop,
		&pAreaBottom,
		&pAreaLeft,
		&pAreaRight
		);
LONG nSelIdx;
	if(		pAreaTop != NULL
		&&	( nSelIdx = pAreaTop->FindControlBar( pBar ) ) >= 0L
		)
	{
		if( p_nSelIdx != NULL )
			(*p_nSelIdx) = nSelIdx;
		return pAreaTop;
	} // if( pAreaTop != NULL ..
	if(		pAreaBottom != NULL
		&&	( nSelIdx = pAreaBottom->FindControlBar( pBar ) ) >= 0L
		)
	{
		if( p_nSelIdx != NULL )
			(*p_nSelIdx) = nSelIdx;
		return pAreaBottom;
	} // if( pAreaBottom != NULL ..
	if(		pAreaLeft != NULL
		&&	( nSelIdx = pAreaLeft->FindControlBar( pBar ) ) >= 0L
		)
	{
		if( p_nSelIdx != NULL )
			(*p_nSelIdx) = nSelIdx;
		return pAreaLeft;
	} // if( pAreaLeft != NULL ..
	if(		pAreaRight != NULL
		&&	( nSelIdx = pAreaRight->FindControlBar( pBar ) ) >= 0L
		)
	{
		if( p_nSelIdx != NULL )
			(*p_nSelIdx) = nSelIdx;
		return pAreaRight;
	} // if( pAreaRight != NULL ..
	return NULL;
}

void CExtDynAutoHideArea::stat_FindAreas(
	CFrameWnd * pDockSite,
	CExtDynAutoHideArea ** ppAreaTop,
	CExtDynAutoHideArea ** ppAreaBottom,
	CExtDynAutoHideArea ** ppAreaLeft,
	CExtDynAutoHideArea ** ppAreaRight
	)
{
	ASSERT_VALID( pDockSite );
	ASSERT_KINDOF( CFrameWnd, pDockSite );
	ASSERT( pDockSite->GetSafeHwnd() != NULL );
	ASSERT( ::IsWindow( pDockSite->GetSafeHwnd() ) );
	if( ppAreaTop != NULL )
		(*ppAreaTop) = NULL;
	if( ppAreaBottom != NULL )
		(*ppAreaBottom) = NULL;
	if( ppAreaLeft != NULL )
		(*ppAreaLeft) = NULL;
	if( ppAreaRight != NULL )
		(*ppAreaRight) = NULL;
HWND hWnd = ::GetWindow( pDockSite->GetSafeHwnd(), GW_CHILD );
	for( ; hWnd != NULL; hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ) )
	{
		UINT nDlgCtrlID = ::GetWindowLong( hWnd, GWL_ID );
		switch( nDlgCtrlID )
		{
		case AFX_IDW_DOCKBAR_TOP:
			if(		ppAreaTop != NULL
				&&	(*ppAreaTop) == NULL
				)
			{
				CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
				if( pWnd == NULL )
					continue;
				(*ppAreaTop) = DYNAMIC_DOWNCAST( CExtDynAutoHideArea, pWnd );
			}
		break;
		case AFX_IDW_DOCKBAR_BOTTOM:
			if(		ppAreaBottom != NULL
				&&	(*ppAreaBottom) == NULL
				)
			{
				CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
				if( pWnd == NULL )
					continue;
				(*ppAreaBottom) = DYNAMIC_DOWNCAST( CExtDynAutoHideArea, pWnd );
			}
		break;
		case AFX_IDW_DOCKBAR_LEFT:
			if(		ppAreaLeft != NULL
				&&	(*ppAreaLeft) == NULL
				)
			{
				CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
				if( pWnd == NULL )
					continue;
				(*ppAreaLeft) = DYNAMIC_DOWNCAST( CExtDynAutoHideArea, pWnd );
			}
		break;
		case AFX_IDW_DOCKBAR_RIGHT:
			if(		ppAreaRight != NULL
				&&	(*ppAreaRight) == NULL
				)
			{
				CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
				if( pWnd == NULL )
					continue;
				(*ppAreaRight) = DYNAMIC_DOWNCAST( CExtDynAutoHideArea, pWnd );
			}
		break;
		}; // switch( nDlgCtrlID )
	} // for( ; hWnd != NULL; hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ) )
}

void CExtDynAutoHideArea::stat_FindAreas(
	HWND hWndStartSearch,
	CExtDynAutoHideArea ** ppAreaTop,
	CExtDynAutoHideArea ** ppAreaBottom,
	CExtDynAutoHideArea ** ppAreaLeft,
	CExtDynAutoHideArea ** ppAreaRight
	)
{
	ASSERT( hWndStartSearch != NULL && ::IsWindow(hWndStartSearch) );
	if( ppAreaTop != NULL )
		(*ppAreaTop) = NULL;
	if( ppAreaBottom != NULL )
		(*ppAreaBottom) = NULL;
	if( ppAreaLeft != NULL )
		(*ppAreaLeft) = NULL;
	if( ppAreaRight != NULL )
		(*ppAreaRight) = NULL;
CWnd * pWnd = CWnd::FromHandle( hWndStartSearch );
CFrameWnd * pDockSite = DYNAMIC_DOWNCAST( CFrameWnd, pWnd );
	if( pDockSite == NULL )
	{
		pDockSite = pWnd->GetParentFrame();
		if( pDockSite == NULL )
			return;
	} // if( pDockSite == NULL )
	if( pDockSite->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
		pDockSite = pDockSite->GetParentFrame();
	if( pDockSite == NULL )
		return;
	stat_FindAreas(
		pDockSite,
		ppAreaTop,
		ppAreaBottom,
		ppAreaLeft,
		ppAreaRight
		);
}

bool CExtDynAutoHideArea::_CanActivate()
{
	if( ( ! CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND( GetSafeHwnd() ) )
		&& ( ! m_bInsertMode )
		)
		return false;

HWND hWndFocus = ::GetFocus();
	if( hWndFocus == NULL )
		return false;
CFrameWnd * pFrame =
		STATIC_DOWNCAST( CFrameWnd, GetParent() );
	if( pFrame->GetSafeHwnd() == hWndFocus
		||	::IsChild( pFrame->GetSafeHwnd(), hWndFocus )
		)
		return true;

CWnd * pWndFocus = CWnd::FromHandlePermanent( hWndFocus );
	if( pWndFocus == NULL )
		return false;

CFrameWnd * pFocusFrame =
		pWndFocus->GetParentFrame();
//	ASSERT( pFocusFrame != pFrame );
	if( pFocusFrame == pFrame )
		return true;

HWND hWndParentOther = pFocusFrame->GetParent()->GetSafeHwnd();
	if( pFrame->GetSafeHwnd() == hWndParentOther
		||	::IsChild( pFrame->GetSafeHwnd(), hWndParentOther )
		)
		return true;

	if( pFrame->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)) )
	{
		CMDIFrameWnd * pMainFrame =
			STATIC_DOWNCAST(
				CMDIFrameWnd,
				pFrame->GetParentFrame()
				);
		CMDIChildWnd * pActiveChild = pMainFrame->MDIGetActive();
		if( pActiveChild == pFrame )
			return true;
	} // if( pFrame->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)) )

	return false;
}

void CExtDynAutoHideArea::OnTabWndItemInsert(
	LONG nItemIndex,
	TAB_ITEM_INFO * pTii
	)
{
	ASSERT_VALID( this );
	CExtTabWnd::OnTabWndItemInsert( nItemIndex, pTii );
	UpdateTabWnd( false );
}


bool CExtDynAutoHideArea::OnTabWndSelectionChange(
	LONG nOldItemIndex,
	LONG nNewItemIndex,
	bool bPreSelectionTest
	)
{
	ASSERT_VALID( this );
	
	ASSERT_VALID( m_pWndSlider );
	ASSERT( m_pWndSlider->GetSafeHwnd() != NULL );
	ASSERT( ::IsWindow(m_pWndSlider->GetSafeHwnd()) );

bool bRetVal =
		CExtTabWnd::OnTabWndSelectionChange(
			nOldItemIndex,
			nNewItemIndex,
			bPreSelectionTest
			);
	if( nOldItemIndex == nNewItemIndex )
	{
		if( (m_pWndSlider->GetStyle()&WS_VISIBLE) != 0 )
			return bRetVal;
	}

	if( bPreSelectionTest )
	{
		if( !_CanActivate() )
		{
			if( nNewItemIndex < 0 )
				return true;
			return false;
		}
		if( nNewItemIndex != -1
			&& !m_bActivationLocked
			)
		{
			if( m_pWndSlider->m_pBar != NULL
				&& m_pWndSlider->m_bActiveState
				)
				m_pWndSlider->Activate(
					m_pWndSlider->m_pBar,
					false,
					false, // true,
					true
					);
		}
		return bRetVal;
	}

	if(	bPreSelectionTest
		|| m_bActivationLocked
		|| nNewItemIndex < 0
		|| ItemGetVisibleCount() == 0
		)
		return bRetVal;

TAB_ITEM_INFO * pTii = ItemGet( nNewItemIndex );
	ASSERT_VALID( pTii );

LPARAM lParam = pTii->LParamGet();
CExtControlBar * pBar =
		reinterpret_cast < CExtControlBar * > ( lParam );
	ASSERT( pBar != NULL );
	ASSERT_KINDOF( CExtControlBar, pBar );

	if(		pBar->GetSafeHwnd() == NULL
		||	(! ::IsWindow(pBar->GetSafeHwnd()) )
		||	pBar->_IsDockSiteCustomizeMode()
		)
		return bRetVal;
	
bool bAnimate = true;
	if( m_bCancelNearestActivateAnimation )
	{
		m_bCancelNearestActivateAnimation = false;
		bAnimate = false;
	} // if( m_bCancelNearestActivateAnimation )
	m_pWndSlider->Activate(
		pBar,
		true,
		bAnimate,
		true
		);

	return bRetVal;
}

void CExtDynAutoHideArea::OnTabWndSyncVisibility()
{
LONG nItemCount = ItemGetCount();
DWORD dwWndStyle = GetStyle();
	if( nItemCount > 0
		&&	(!CExtControlBar::FindPrintPreviewMode(
				STATIC_DOWNCAST( CFrameWnd, GetParent() )
				)
			)
		)
	{
		if( (dwWndStyle & WS_VISIBLE) == 0 )
		{
			::SetWindowPos(
				m_hWnd,
				NULL, 0, 0, 0, 0,
				SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOOWNERZORDER
					|SWP_FRAMECHANGED
					|SWP_SHOWWINDOW
				);
		}
	}
	else
	{
		if( (dwWndStyle & WS_VISIBLE) != 0 )
			::SetWindowPos(
				m_hWnd,
				NULL, 0, 0, 0, 0,
				SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOOWNERZORDER
					|SWP_HIDEWINDOW
				);
	}

//	dwWndStyle = GetStyle();
//	if( (dwWndStyle & WS_VISIBLE) == 0 )
//		m_nIndexVisFirst = m_nIndexVisLast = -1;
}

void CExtDynAutoHideArea::OnTabWndMeasureItemAreaMargins(
	CDC & dcMeasure,
	LONG & nSpaceBefore,
	LONG & nSpaceAfter,
	LONG & nSpaceOver
	)
{
	CExtTabWnd::OnTabWndMeasureItemAreaMargins(
		dcMeasure,
		nSpaceBefore,
		nSpaceAfter,
		nSpaceOver
		);
	
	nSpaceOver = 0;
	
	if( OrientationIsVertical() )
		return;

CFrameWnd * pFrame =
		STATIC_DOWNCAST(
			CFrameWnd,
			GetParent()
			);
CControlBar * pBarTemp =
		pFrame->GetControlBar( AFX_IDW_DOCKBAR_LEFT );
	if( pBarTemp == NULL )
		return; // shutdown mode
	ASSERT_VALID( pBarTemp );
CExtDockBar * pDockBarLeft =
		DYNAMIC_DOWNCAST( CExtDockBar, pBarTemp );
	if(		pDockBarLeft == NULL
		||	pDockBarLeft->_GetCircleNo() != 0
		)
		return; // shutdown mode
	pBarTemp = pFrame->GetControlBar( AFX_IDW_DOCKBAR_RIGHT );
	if( pBarTemp == NULL )
		return; // shutdown mode
	ASSERT_VALID( pBarTemp );
CExtDockBar * pDockBarRight =
		DYNAMIC_DOWNCAST( CExtDockBar, pBarTemp );
	if(		pDockBarRight == NULL
		||	pDockBarRight->_GetCircleNo() != 0
		)
		return; // shutdown mode
CExtDynAutoHideArea * pAreaLeft =
		pDockBarLeft->_GetAutoHideArea();
	if( pAreaLeft == NULL )
		return; // shutdown mode
	ASSERT_VALID( pAreaLeft );
CExtDynAutoHideArea * pAreaRight =
		pDockBarRight->_GetAutoHideArea();
	if( pAreaRight == NULL )
		return; // shutdown mode
	ASSERT_VALID( pAreaLeft );
bool bLeftAreaVisible =
		( pAreaLeft->ItemGetCount() != 0 )
			? true : false;
bool bRightAreaVisible =
		( pAreaRight->ItemGetCount() != 0 )
			? true : false;

	if( !(bLeftAreaVisible || bRightAreaVisible) )
		return;

CRect rcWnd;
	GetWindowRect( &rcWnd );
CSize sizeWnd = rcWnd.Size();
LONG nUpdateMetric = sizeWnd.cy; // + 4;
	if( bLeftAreaVisible )
		nSpaceBefore += nUpdateMetric;
	if( bRightAreaVisible )
		nSpaceAfter += nUpdateMetric;
}

void CExtDynAutoHideArea::PostNcDestroy() 
{
//CExtTabWnd::PostNcDestroy();
	delete this;
}

LRESULT CExtDynAutoHideArea::OnSizeParent(WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID( this );
LRESULT lResult = CExtTabWnd::OnSizeParent(wParam, lParam);
	if( (GetStyle() & WS_VISIBLE) == 0 )
		return lResult;

CFrameWnd * pFrame =
		STATIC_DOWNCAST( CFrameWnd, GetParent() );
	ASSERT_VALID( pFrame );

UINT nDockedDlgCtrlID = GetDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockedDlgCtrlID );
CControlBar * pBarTemp =
		pFrame->GetControlBar( nDockedDlgCtrlID );
	if( pBarTemp == NULL )
		return lResult; // shutdown mode
	ASSERT_VALID( pBarTemp );
CExtDockBar * pDockBar =
		DYNAMIC_DOWNCAST( CExtDockBar, pBarTemp );
	if( pDockBar == NULL )
		return lResult; // shutdown mode
	ASSERT_VALID( pDockBar );
	if( pDockBar->_GetCircleNo() != 0 )
		return lResult; // shutdown mode
	ASSERT( pDockBar->m_pWndAutoHideArea == this );

CRect _rcWnd;
	GetWindowRect( &_rcWnd );
CSize _sizeWnd = _rcWnd.Size();
	switch( nDockedDlgCtrlID )
	{
	case AFX_IDW_DOCKBAR_TOP:
		pDockBar->m_rcLastInvisiblePreCalc.bottom += _sizeWnd.cy;
		break;
	case AFX_IDW_DOCKBAR_BOTTOM:
		pDockBar->m_rcLastInvisiblePreCalc.top -= _sizeWnd.cy;
		break;
	case AFX_IDW_DOCKBAR_LEFT:
		pDockBar->m_rcLastInvisiblePreCalc.right += _sizeWnd.cx;
		break;
	case AFX_IDW_DOCKBAR_RIGHT:
		pDockBar->m_rcLastInvisiblePreCalc.left -= _sizeWnd.cx;
		break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
		break;
#endif // _DEBUG
	} // switch( nDockedDlgCtrlID )

	return lResult;
}

CExtDynAutoHideSlider * CExtDynAutoHideArea::GetAutoHideSlider()
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pWndSlider );
	return m_pWndSlider;
}

void CExtDynAutoHideArea::AppendControlBar(
	CExtControlBar * pBar,
	bool bForceNoShow
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
	ASSERT( !pBar->IsFixedMode() );
	ASSERT( !pBar->AutoHideModeGet() );

	if( pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
	{
		ASSERT( FindControlBar(pBar) < 0 );
		// dynamic control bar should be tabbed
		ASSERT_KINDOF( CExtDynTabControlBar, pBar );
		CExtDynTabControlBar * pTabbedBar =
			STATIC_DOWNCAST( CExtDynTabControlBar, pBar );
		INT nItemCount = pTabbedBar->GetSwitcherItemCount();
		INT nSwitcherSel = pTabbedBar->GetSwitcherSelection();
		if( nSwitcherSel < 0 )
			nSwitcherSel = 0;
		ASSERT( nItemCount > 1 );
		for( INT nBar = 0; nBar < nItemCount; nBar ++ )
		{
			CExtControlBar * pExtBar =
				pTabbedBar->GetBarAt( nBar, true );
			ASSERT_VALID( pExtBar );
			ASSERT( FindControlBar(pExtBar) < 0 );
			ASSERT( !pExtBar->m_bAutoHideMode );
			pExtBar->m_bAutoHideMode = true;
			ItemInsert(
				NULL, NULL, false,
				(nBar == 0) ? __ETWI_GROUP_START : 0,
				-1,
				reinterpret_cast < LPARAM > ( pExtBar ),
				false
				);
		}
		INT nSel =
			ItemGetCount() // - 1
			- nItemCount
			+ nSwitcherSel;
		ASSERT( SelectionGet() != nSel );
		m_bActivationLocked = true;
		m_bInsertMode = true;
		SelectionSet( nSel, true, true );
		SelectionSet( -1 );
		m_bInsertMode = false;
		m_bActivationLocked = false;
		if( !bForceNoShow )
		{
			pBar->m_pDockSite->RecalcLayout();
			m_pWndSlider->Activate(
				((CExtControlBar *)ItemGet(nSel)->LParamGet()),
				true, false, true
				);
		}
	} // if( pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
	else
	{
		ASSERT( FindControlBar(pBar) < 0 );
		ItemInsert(
			NULL, NULL, false,
			__ETWI_GROUP_START,
			-1,
			reinterpret_cast < LPARAM > ( pBar ),
			false
			);
		ASSERT( SelectionGet() != (ItemGetCount() - 1) );
		ASSERT( !pBar->m_bAutoHideMode );
		pBar->m_bAutoHideMode = true;
		m_bActivationLocked = true;
		m_bInsertMode = true;
		SelectionSet( ItemGetCount() - 1, true, true );
		SelectionSet( -1 );
		m_bInsertMode = false;
		m_bActivationLocked = false;
		if( !bForceNoShow )
		{
			pBar->m_pDockSite->RecalcLayout();
			m_pWndSlider->Activate( pBar, true, false, true );
		}
	} // else from if( pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )

}

void CExtDynAutoHideArea::RemoveControlBar(
	CExtControlBar * pBar,
	bool bForceNoShow
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
	ASSERT( !pBar->IsFixedMode() );
	ASSERT( pBar->AutoHideModeGet() );

	if( pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
	{
		ASSERT( FindControlBar(pBar) < 0 );
		// dynamic control bar should be tabbed
		ASSERT_KINDOF( CExtDynTabControlBar, pBar );
		CExtDynTabControlBar * pTabbedBar =
			STATIC_DOWNCAST( CExtDynTabControlBar, pBar );
		INT nItemCount = pTabbedBar->GetSwitcherItemCount();
		INT nSwitcherSel = -1;
		ASSERT( nItemCount > 1 );
		CExtControlBar * pBarDelayRemove = NULL;
		for( INT nBar = nItemCount; nBar > 0; nBar -- )
		{
			CExtControlBar * pExtBar =
				pTabbedBar->GetBarAt(
					nBar - 1,
					true
					);
			ASSERT_VALID( pExtBar );
			ASSERT( pExtBar->m_bAutoHideMode );
			LONG nIndex = FindControlBar(pExtBar);
			ASSERT( nIndex >= 0 );
			if( nSwitcherSel == -1 )
			{
				TAB_ITEM_INFO * pTii = ItemGet( nIndex );
				ASSERT_VALID( pTii );
				if( pTii->GetItemStyle() & __ETWI_IN_GROUP_ACTIVE )
				{
					TAB_ITEM_INFO * pTiiFirst = pTii->GetInGroupFirst();
					ASSERT_VALID( pTiiFirst );
					nSwitcherSel =
						ItemGetIndexOf( pTii )
						- ItemGetIndexOf( pTiiFirst )
						;
					pBarDelayRemove = pExtBar;
					continue;
				}
			} // if( nSwitcherSel == -1 )
			RemoveControlBar( pExtBar, bForceNoShow );
		}
		pTabbedBar->m_bAutoHideMode = false;
		ASSERT( nSwitcherSel >= 0 && nSwitcherSel < nItemCount );

		ASSERT( pBarDelayRemove != NULL );
		pBarDelayRemove->m_bAutoHideMode = false;
		LONG nIndex = FindControlBar(pBarDelayRemove);
		ASSERT( nIndex >= 0 );
		ItemRemove( nIndex, 1, true );
		
		pBarDelayRemove->m_bAutoHideMode = false;
		
		if( !bForceNoShow )
		{
			pTabbedBar->_PreSyncRowMetrics();
			pTabbedBar->m_pDockSite->ShowControlBar( pBarDelayRemove, TRUE, TRUE );
			pTabbedBar->m_pDockSite->ShowControlBar( pTabbedBar, TRUE, FALSE );
			pTabbedBar->SetSwitcherSelection( nSwitcherSel );
//			pTabbedBar->_RecalcNcArea();
		}
		else
		{
			pTabbedBar->m_pDockSite->ShowControlBar( pTabbedBar, TRUE, TRUE );
			pTabbedBar->m_pDockSite->DelayRecalcLayout();
		}
		return;
	}

	ASSERT( pBar->m_bAutoHideMode );
	pBar->m_bAutoHideMode = false;

LONG nIndex = FindControlBar(pBar);
	ASSERT( nIndex >= 0 );

	ItemRemove( nIndex, 1, true );
	
	pBar->m_bAutoHideMode = false;

	if( !bForceNoShow )
	{
		ASSERT_VALID( pBar->m_pDockBar );
		ASSERT_KINDOF( CExtDockBar, pBar->m_pDockBar );
		if( !pBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockDynTabBar)) )
			pBar->_PreSyncRowMetrics();
		pBar->m_pDockSite->ShowControlBar( pBar, TRUE, TRUE );
	}
	else
		pBar->m_pDockSite->DelayRecalcLayout();
}

void CExtDynAutoHideArea::OnSize(UINT nType, int cx, int cy) 
{
	CExtTabWnd::OnSize(nType, cx, cy);

CRect rcLastPos;
	GetWindowRect( &rcLastPos );
	if( m_rcLastPos != rcLastPos )
	{
		m_rcLastPos = rcLastPos;
		ASSERT_VALID( m_pWndSlider );
		if( m_pWndSlider->m_bActiveState
			&& !m_pWndSlider->m_bAnimation
			)
			m_pWndSlider->PostMessage( WM_CANCELMODE );
	} // if( m_rcLastPos != rcLastPos )
}

void CExtDynAutoHideArea::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CExtTabWnd::OnWindowPosChanged(lpwndpos);

CRect rcLastPos;
	GetWindowRect( &rcLastPos );
	if( m_rcLastPos != rcLastPos )
	{
		m_rcLastPos = rcLastPos;
		ASSERT_VALID( m_pWndSlider );
		if( m_pWndSlider->m_bActiveState
			&& !m_pWndSlider->m_bAnimation
			&& m_pWndSlider->m_nResizingMode == HTCLIENT
			)
			m_pWndSlider->PostMessage( WM_CANCELMODE );
	} // if( m_rcLastPos != rcLastPos )
}

void CExtDynAutoHideArea::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonDown( nFlags, point );
}

void CExtDynAutoHideArea::OnRButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonUp( nFlags, point );
}

void CExtDynAutoHideArea::OnContextMenu(CWnd* pWnd, CPoint point)
{
	ASSERT_VALID( this );
	pWnd;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
		return;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if( CExtControlBar::FindHelpMode(this) )
		return;

	ASSERT_VALID( m_pWndSlider );
	ASSERT( m_pWndSlider->GetSafeHwnd() != NULL );
	ASSERT( ::IsWindow(m_pWndSlider->GetSafeHwnd()) );

	if( m_pWndSlider->m_bAnimation )
		return;
	if( CExtTabWnd::m_nDelayedSelIndex >= 0 )
		return;

CFrameWnd * pFrame = STATIC_DOWNCAST( CFrameWnd, GetParent() );
	ASSERT_VALID( pFrame );
HWND hWndTrack = pFrame->GetSafeHwnd();
	ASSERT( hWndTrack != NULL && ::IsWindow(hWndTrack) );
CExtPopupMenuWnd * pPopup = new CExtPopupMenuWnd;
	VERIFY( pPopup->CreatePopupMenu(hWndTrack) );

CExtControlBar::POPUP_MENU_EVENT_DATA _pmed(
		CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_AUTOHIDETABS_CTX,
		pPopup,
		this,
		point,
		WM_CONTEXTMENU,
		NULL
		);

	if( !_pmed.NotifyTarget( false ) )
	{

		LONG nItemCount = ItemGetCount();
		for( LONG nBar = 0; nBar < nItemCount; nBar ++ )
		{
			TAB_ITEM_INFO * pTII = ItemGet( nBar );
			ASSERT_VALID( pTII );
			CExtControlBar * pExtBar =
				(CExtControlBar *) pTII->LParamGet();
			ASSERT_VALID( pExtBar );
			ASSERT_KINDOF( CExtControlBar, pExtBar );
			ASSERT( pExtBar->m_bAutoHideMode );

			UINT nID = pExtBar->GetDlgCtrlID();

			if(		nBar > 0
				&&	(pTII->GetItemStyle() & __ETWI_GROUP_START) != 0
				)
			{
				VERIFY( pPopup->ItemInsert( ID_SEPARATOR ) );
			}

			VERIFY( pPopup->ItemInsert( nID ) );
		}

	} // if( !_pmed.NotifyTarget( false ) )
	_pmed.NotifyTarget( true );
	if( pPopup->ItemGetCount() == 0 )
	{
		delete pPopup;
		return;
	}
	::SetFocus( hWndTrack );
	if(	! pPopup->TrackPopupMenu(
			TPMX_OWNERDRAW_FIXED,
			point.x,
			point.y
			)
		)
	{
		delete pPopup;
	}
}

void CExtDynAutoHideArea::OnPaint()
{
	ASSERT_VALID( this );

DWORD dwTabWndStyle = GetTabWndStyle();
bool bGrpExpCurrent = (dwTabWndStyle&__ETWS_GROUPED_EXPAND_ITEMS) ? true : false;
bool bGrpExpNeeded = false;
CExtControlBar::eAutoHideTabsType_t eAutoHideTabsType =
		CExtControlBar::g_eAutoHideTabsType;
	if( eAutoHideTabsType == CExtControlBar::__AUTOHIDE_TABS_TYPE_BY_THEME )
		eAutoHideTabsType = (CExtControlBar::eAutoHideTabsType_t)
			g_PaintManager->GetAutoHideTabsType();
	ASSERT(
			CExtControlBar::__AUTOHIDE_TABS_TYPE_MIN_VALUE <= eAutoHideTabsType
		&&	eAutoHideTabsType <= CExtControlBar::__AUTOHIDE_TABS_TYPE_MAX_VALUE
		);
	if( eAutoHideTabsType == CExtControlBar::__AUTOHIDE_TABS_TYPE_STUDIO_2005 )
		bGrpExpNeeded = true;
	if( bGrpExpCurrent != bGrpExpNeeded )
	{
		ModifyTabWndStyle(
			bGrpExpNeeded ? 0 : __ETWS_GROUPED_EXPAND_ITEMS,
			bGrpExpNeeded ? __ETWS_GROUPED_EXPAND_ITEMS : 0,
			false
			);
		m_bDelayRecalcLayout = true;
	} // if( bGrpExpCurrent != bGrpExpNeeded )

	CExtTabWnd::OnPaint();
}

BOOL CExtDynAutoHideArea::PreTranslateMessage(MSG* pMsg) 
{
	if(		CExtControlBar::g_bAllowKeyboardAutohideSwitch
		&&	pMsg->message == WM_KEYDOWN
		&&	(	pMsg->wParam == VK_PRIOR
			||	pMsg->wParam == VK_NEXT
			)
		&&	GetSafeHwnd() != NULL
		&&	ItemGetVisibleCount() > 0
		&&	CExtPopupMenuWnd::IsKeyPressed( VK_CONTROL )
		&&	(! CExtPopupMenuWnd::IsKeyPressed( VK_SHIFT ) )
		&&	(! CExtPopupMenuWnd::IsKeyPressed( VK_MENU ) )
		)
	{ // if Ctrl+PageUp or Ctrl+PageDown keydown event
		LONG nSel = SelectionGet();
		if( nSel >= 0L )
		{
			LONG nStep = (pMsg->wParam == VK_PRIOR) ? -1L : 1L;
			LONG nIdx = nSel + nStep;
			LONG nItemCount = ItemGetCount();
			ASSERT( nItemCount > 0L );
			if( nIdx < 0 )
				nIdx = nItemCount - 1L;
			else if( nIdx >= nItemCount )
				nIdx = 0;
			for( ; nIdx != nSel;  )
			{
				const TAB_ITEM_INFO * pTii = ItemGet( nIdx );
				ASSERT( pTii != NULL );
				if(		pTii->VisibleGet()
					&&	((pTii->GetItemStyle())&__DYNTAB_ITEM_STYLE_TEMPORARY) == 0
					)
				{
					LPARAM lParam = pTii->LParamGet();
					CExtControlBar * pBar =
						reinterpret_cast < CExtControlBar * > ( lParam );
					ASSERT( pBar != NULL );
					ASSERT_KINDOF( CExtControlBar, pBar );
					if( (pBar->GetStyle()&WS_VISIBLE) == 0 )
					{
						CWnd::CancelToolTips();
						HWND hWndChild = ::GetWindow( pBar->m_hWnd, GW_CHILD );
						m_bCancelNearestActivateAnimation = true;
						SelectionSet( nIdx, true, true );
						if( hWndChild != NULL && ::IsWindow(hWndChild) )
						{
							CWnd * pWndChild = CWnd::FromHandle(hWndChild);
							if(		pWndChild->GetSafeHwnd() != NULL
								&&	(pWndChild->GetStyle()&WS_VISIBLE) != 0
								&&	pWndChild->IsWindowEnabled()
								)
							{
								CWnd * pWndParent = pWndChild->GetParent();
								if(		pWndParent->GetSafeHwnd() != NULL
									&&	pWndParent->IsKindOf( RUNTIME_CLASS(CExtDynAutoHideSlider) )
									)
									pWndChild->SetFocus();
							} // if( pWndChild->GetSafeHwnd() != NULL ...
						} // if( hWndChild != NULL && ::IsWindow(hWndChild) )
						break;
					} // if( (pBar->GetStyle()&WS_VISIBLE) == 0 )
				} // if( pTii->IsVisible() ...
				nIdx += nStep;
				if( nIdx < 0 )
					nIdx = nItemCount - 1L;
				else if( nIdx >= nItemCount )
					nIdx = 0;
			} // for( ; nIdx != nSel;  )
			return TRUE;
		} // if( nSel >= 0L )
	} // if Ctrl+PageUp or Ctrl+PageDown keydown event
	return CExtTabWnd::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////
// CExtDynAutoHideSlider window

IMPLEMENT_DYNCREATE( CExtDynAutoHideSlider, CWnd );

BEGIN_MESSAGE_MAP(CExtDynAutoHideSlider, CWnd)
	//{{AFX_MSG_MAP(CExtDynAutoHideSlider)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_CONTEXTMENU()
	ON_WM_CANCELMODE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATEAPP()
	ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
END_MESSAGE_MAP()

CExtDynAutoHideSlider::CExtDynAutoHideSlider()
	: m_pWndAnimateSrc( NULL )
	, m_pBar( NULL )
	, m_bActiveState( false )
	, m_bWndActive( false )
	, m_bDeactivateTesting( false )
	, m_bAnimation( false )
	, m_nResizingMode( HTCLIENT )
	, m_pLastHitTestM( NULL )
	, m_pLastHitTestT( NULL )
	, m_pLastHitTestB( NULL )
	, m_rcMax( 0, 0, 0, 0 )
{
	VERIFY( RegisterAutohideSliderWndClass() );
}

static bool g_bAutoHideSliderWndClassRegistered = false;

bool CExtDynAutoHideSlider::RegisterAutohideSliderWndClass()
{
	if( g_bAutoHideSliderWndClassRegistered )
		return true;

WNDCLASS wndclass;
HINSTANCE hInst = AfxGetInstanceHandle();
	if( ! ::GetClassInfo(
			hInst,
			__AUTOHIDE_SLIDER_WNDCLASS,
			&wndclass
			)
		)
	{
		// otherwise we need to register a new class
		wndclass.style = CS_GLOBALCLASS|CS_DBLCLKS;
		wndclass.lpfnWndProc = ::DefWindowProc;
		wndclass.cbClsExtra = wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInst;
		wndclass.hIcon = NULL;
		wndclass.hCursor =
				::LoadCursor(
					NULL, //hInst,
					IDC_ARROW
					)
				;
		ASSERT( wndclass.hCursor != NULL );
		wndclass.hbrBackground = NULL; 
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = __AUTOHIDE_SLIDER_WNDCLASS;
		if( !::AfxRegisterClass( &wndclass ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}

	g_bAutoHideSliderWndClassRegistered = true;
	return true;
}

BOOL CExtDynAutoHideSlider::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !RegisterAutohideSliderWndClass()
		|| !CWnd::PreCreateWindow( cs )
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}

	cs.lpszClass = __AUTOHIDE_SLIDER_WNDCLASS;

	return TRUE;
}

BOOL CExtDynAutoHideSlider::Create(
	CFrameWnd * pParentWnd
	)
{
	ASSERT_VALID( pParentWnd );
	ASSERT_KINDOF( CFrameWnd, pParentWnd );
	ASSERT( !pParentWnd->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) );
	ASSERT( GetSafeHwnd() == NULL );

	if( !RegisterAutohideSliderWndClass() )
	{
		ASSERT( FALSE );
		return FALSE;
	}

	if( ! CWnd::Create(
			__AUTOHIDE_SLIDER_WNDCLASS,
			_T(""),
			WS_CHILD, //|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
			CRect(0,0,0,0),
			pParentWnd,
			UINT(IDC_STATIC)
			)
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}

	EnableToolTips();
	m_wndToolTip.Create( this );
	m_wndToolTip.Activate( TRUE );

	return TRUE;
}

void CExtDynAutoHideSlider::PostNcDestroy()
{
//CWnd::PostNcDestroy();
	delete this;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
void CExtDynAutoHideSlider::OnCustomizeModeEnter(
	CExtDynAutoHideArea * pWndAutoHideArea
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pWndAutoHideArea );
	pWndAutoHideArea;
}

void CExtDynAutoHideSlider::OnCustomizeModeLeave(
	CExtDynAutoHideArea * pWndAutoHideArea
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pWndAutoHideArea );
	pWndAutoHideArea;
}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

LRESULT CExtDynAutoHideSlider::OnSizeParent(WPARAM wParam, LPARAM lParam)
{
	wParam;
	lParam;
//	if( m_bActiveState )
//		SendMessage( WM_CANCELMODE );
	return 0L;
}

BOOL CExtDynAutoHideSlider::OnEraseBkgnd(CDC* pDC) 
{
	pDC;
	return TRUE;
}

void CExtDynAutoHideSlider::OnPaint() 
{
CPaintDC dc(this);
HWND hWndChild = ::GetWindow( GetSafeHwnd(), GW_CHILD );
	if( hWndChild != NULL )
	{
		ASSERT( ::IsWindow(hWndChild) );
		return;
	}
CRect rcClient;
	GetClientRect( &rcClient );
	dc.FillSolidRect(
		&rcClient,
		g_PaintManager->GetColor( COLOR_3DFACE )
		);
}

#define __AH_RESIZER_DX 3
#define __AH_RESIZER_DY 3
#define __AH_RESIZER_NCGAP_DX 2 // 1
#define __AH_RESIZER_NCGAP_DY 2 // 1
#define __AH_RESIZER_GRIPGAP_DX 1
#define __AH_RESIZER_GRIPGAP_DY 1

#define __AH_RESIZER_METRIC ( __AH_RESIZER_DX + __AH_RESIZER_NCGAP_DX )

UINT CExtDynAutoHideSlider::OnNcHitTest(CPoint point) 
{
	point;
	if( !m_bActiveState || m_bAnimation )
		return HTCLIENT;
	
	if( m_nResizingMode != HTCLIENT )
		return m_nResizingMode;

	ASSERT_VALID( m_pWndAnimateSrc );
UINT nID = m_pWndAnimateSrc->GetDlgCtrlID();
CRect rcResizeArea;
	GetWindowRect( &rcResizeArea );
	switch( nID )
	{
	case AFX_IDW_DOCKBAR_TOP:
		rcResizeArea.top =
			rcResizeArea.bottom - __AH_RESIZER_METRIC;
		if( rcResizeArea.PtInRect(point) )
			return HTBOTTOM;
		break;
	case AFX_IDW_DOCKBAR_BOTTOM:
		rcResizeArea.bottom =
			rcResizeArea.top + __AH_RESIZER_METRIC;
		if( rcResizeArea.PtInRect(point) )
			return HTTOP;
		break;
	case AFX_IDW_DOCKBAR_LEFT:
		rcResizeArea.left =
			rcResizeArea.right - __AH_RESIZER_METRIC;
		if( rcResizeArea.PtInRect(point) )
			return HTRIGHT;
		break;
	case AFX_IDW_DOCKBAR_RIGHT:
		rcResizeArea.right =
			rcResizeArea.left + __AH_RESIZER_METRIC;
		if( rcResizeArea.PtInRect(point) )
			return HTLEFT;
		break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
		break;
#endif // _DEBUG
	} // switch( nID )
	return HTCLIENT;
}

void CExtDynAutoHideSlider::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	bCalcValidRects;
	if(		m_pWndAnimateSrc == NULL
		||	m_pBar == NULL
		||	m_pWndAnimateSrc->GetSafeHwnd() == NULL
		||	m_pBar->GetSafeHwnd() == NULL
		||	(! ::IsWindow(m_pWndAnimateSrc->GetSafeHwnd()) )
		||	(! ::IsWindow(m_pBar->GetSafeHwnd()) )
		)
		return;
UINT nID = UINT( m_pWndAnimateSrc->GetDlgCtrlID() );
	lpncsp->rgrc[0].left += __AH_RESIZER_NCGAP_DX;
	lpncsp->rgrc[0].right -= __AH_RESIZER_NCGAP_DX;
	lpncsp->rgrc[0].top += __AH_RESIZER_NCGAP_DY;
	lpncsp->rgrc[0].bottom -= __AH_RESIZER_NCGAP_DY;
	switch( nID )
	{
	case AFX_IDW_DOCKBAR_TOP:
		lpncsp->rgrc[0].bottom -= __AH_RESIZER_DY;
		break;
	case AFX_IDW_DOCKBAR_BOTTOM:
		lpncsp->rgrc[0].top += __AH_RESIZER_DY;
		break;
	case AFX_IDW_DOCKBAR_LEFT:
		lpncsp->rgrc[0].right -= __AH_RESIZER_DX;
		break;
	case AFX_IDW_DOCKBAR_RIGHT:
		lpncsp->rgrc[0].left += __AH_RESIZER_DX;
		break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
		break;
#endif // _DEBUG
	} // switch( nID )
bool bGripperAtTop = false;
	if( m_pBar->IsBarWithGripper(&bGripperAtTop) )
	{
		if( bGripperAtTop )
			lpncsp->rgrc[0].top +=
				m_pBar->m_nGripHeightAtTop
				+ __AH_RESIZER_GRIPGAP_DY;
		else
			lpncsp->rgrc[0].left +=
				m_pBar->m_nGripWidthAtLeft
				+ __AH_RESIZER_GRIPGAP_DX;
	} // if( m_pBar->IsBarWithGripper(&bGripperAtTop) )
}

void CExtDynAutoHideSlider::CalcInnerNcRects(
	RECT * pRcInnerArea,
	RECT * pRcDrawResizer,
	RECT * pRcDrawGripper
	)
{
	ASSERT_VALID( this );

	if(		pRcInnerArea == NULL
		&&	pRcDrawResizer == NULL
		&&	pRcDrawGripper == NULL
		)
		return;
CRect rcEmpty( 0, 0, 0, 0 );
	if( pRcInnerArea != NULL )
		::CopyRect( pRcInnerArea, &rcEmpty );
	if( pRcDrawResizer != NULL )
		::CopyRect( pRcDrawResizer, &rcEmpty );
	if( pRcDrawGripper != NULL )
		::CopyRect( pRcDrawGripper, &rcEmpty );

	if(		m_pWndAnimateSrc == NULL
		||	m_pBar == NULL
		||	m_pWndAnimateSrc->GetSafeHwnd() == NULL
		||	m_pBar->GetSafeHwnd() == NULL
		||	(! ::IsWindow(m_pWndAnimateSrc->GetSafeHwnd()) )
		||	(! ::IsWindow(m_pBar->GetSafeHwnd()) )
		)
		return;

CRect rcInnerArea;
	GetWindowRect( &rcInnerArea );
	rcInnerArea.OffsetRect( -rcInnerArea.TopLeft() );
CRect rcResizer( rcInnerArea );
	rcResizer.InflateRect( 2, 2 );

	rcInnerArea.left += __AH_RESIZER_NCGAP_DX;
	rcInnerArea.right -= __AH_RESIZER_NCGAP_DX;
	rcInnerArea.top += __AH_RESIZER_NCGAP_DY;
	rcInnerArea.bottom -= __AH_RESIZER_NCGAP_DY;

	ASSERT_VALID( m_pWndAnimateSrc );
UINT nID = UINT( m_pWndAnimateSrc->GetDlgCtrlID() );
	
	switch( nID )
	{
	case AFX_IDW_DOCKBAR_TOP:
		rcInnerArea.bottom -= __AH_RESIZER_DY;
		rcResizer.bottom -= 2;
		break;
	case AFX_IDW_DOCKBAR_BOTTOM:
		rcInnerArea.top += __AH_RESIZER_DY;
		rcResizer.top += 2;
		break;
	case AFX_IDW_DOCKBAR_LEFT:
		rcInnerArea.right -= __AH_RESIZER_DX;
		rcResizer.right -= 2;
		break;
	case AFX_IDW_DOCKBAR_RIGHT:
		rcInnerArea.left += __AH_RESIZER_DX;
		rcResizer.left += 2;
		break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
		break;
#endif // _DEBUG
	} // switch( nID )

	if( pRcDrawGripper != NULL )
	{
		bool bGripperAtTop = false;
		if( m_pBar->IsBarWithGripper(&bGripperAtTop) )
		{
			CRect rcGrip( rcInnerArea );
			if( bGripperAtTop )
				rcGrip.bottom = rcGrip.top +
					m_pBar->m_nGripHeightAtTop;
			else
				rcGrip.right = rcGrip.left +
					m_pBar->m_nGripWidthAtLeft;
				::CopyRect( pRcDrawGripper, &rcGrip );
		}
	} // if( pRcDrawGripper != NULL )

	if( pRcInnerArea != NULL )
		::CopyRect( pRcInnerArea, &rcInnerArea );
	if( pRcDrawResizer != NULL )
		::CopyRect( pRcDrawResizer, &rcResizer );
}

void CExtDynAutoHideSlider::CalcNcOffsets(
	RECT * pRcNcSizes
	)
{
	ASSERT_VALID( this );
	ASSERT( pRcNcSizes != NULL );
	::memset( pRcNcSizes, 0, sizeof(RECT) );
	pRcNcSizes->left += __AH_RESIZER_NCGAP_DX;
	pRcNcSizes->right += __AH_RESIZER_NCGAP_DX;
	pRcNcSizes->top += __AH_RESIZER_NCGAP_DY;
	pRcNcSizes->bottom += __AH_RESIZER_NCGAP_DY;

	ASSERT_VALID( m_pWndAnimateSrc );
UINT nID = UINT( m_pWndAnimateSrc->GetDlgCtrlID() );
	
	switch( nID )
	{
	case AFX_IDW_DOCKBAR_TOP:
		pRcNcSizes->bottom += __AH_RESIZER_DY;
		break;
	case AFX_IDW_DOCKBAR_BOTTOM:
		pRcNcSizes->top += __AH_RESIZER_DY;
		break;
	case AFX_IDW_DOCKBAR_LEFT:
		pRcNcSizes->right += __AH_RESIZER_DX;
		break;
	case AFX_IDW_DOCKBAR_RIGHT:
		pRcNcSizes->left += __AH_RESIZER_DX;
		break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
		break;
#endif // _DEBUG
	} // switch( nID )

bool bGripperAtTop = false;
	if( m_pBar->IsBarWithGripper(&bGripperAtTop) )
	{
		if( bGripperAtTop )
			pRcNcSizes->top +=
				m_pBar->m_nGripHeightAtTop;
		else
			pRcNcSizes->left +=
				m_pBar->m_nGripWidthAtLeft;
	}
}

void CExtDynAutoHideSlider::OnNcPaint() 
{
	ASSERT_VALID( this );
CWindowDC dc( this );
CRect rcClient, rcBar;
	GetClientRect( rcClient );
	ClientToScreen( rcClient );
	GetWindowRect( rcBar );
	rcClient.OffsetRect( -rcBar.TopLeft() );
	rcBar.OffsetRect( -rcBar.TopLeft() );
	dc.ExcludeClipRect(rcClient);
CRect rcDraw = rcBar;
CExtMemoryDC mdc( &dc, &rcBar );
	mdc.FillSolidRect(
		&rcDraw,
		g_PaintManager->GetColor(COLOR_3DFACE)
		);

bool bEmptyMode = false;
	if(		m_pWndAnimateSrc == NULL
		||	m_pBar == NULL
		||	m_pWndAnimateSrc->GetSafeHwnd() == NULL
		||	m_pBar->GetSafeHwnd() == NULL
		||	(! ::IsWindow(m_pWndAnimateSrc->GetSafeHwnd()) )
		||	(! ::IsWindow(m_pBar->GetSafeHwnd()) )
		)
		bEmptyMode = true;

	if( !bEmptyMode )
	{
		CRect rcResizer, rcGrip;
		CalcInnerNcRects(
			NULL,
			&rcResizer,
			&rcGrip
			);

		mdc.Draw3dRect(
			&rcResizer,
			g_PaintManager->GetColor(COLOR_3DFACE),
			g_PaintManager->GetColor(COLOR_3DDKSHADOW)
			);
		rcResizer.DeflateRect( 1, 1 );
		mdc.Draw3dRect(
			&rcResizer,
			g_PaintManager->GetColor(COLOR_3DHILIGHT),
			g_PaintManager->GetColor(COLOR_3DSHADOW)
			);

		bool bGripperAtTop = false;
		if( m_pBar->IsBarWithGripper(&bGripperAtTop) )
		{
			m_pBar->OnNcAreaButtonsReposition();
			
			bool bHorz = m_pBar->IsDockedHorizontally();
			CRect rcText( rcGrip );
			if( bHorz && !bGripperAtTop )
			{
				rcText.DeflateRect( 0, 2 );
				rcText.OffsetRect( -2, 0 );
			}
			else
				rcText.DeflateRect( 2, 0 );
			m_pBar->NcButtons_CalcGripperTextLocation( rcText );
			CExtSafeString sCaption;
			m_pBar->OnGetBarCaptionText( CExtControlBar::__EBCT_AUTOHIDE_SLIDER_CAPTION, sCaption );
			CRgn rgnExclude;
			if( rgnExclude.CreateRectRgnIndirect( &rcGrip ) )
				mdc.SelectClipRgn( &rgnExclude );
			CExtPaintManager::PAINTGRIPPERDATA _pgd(
				this,
				rcGrip,
				rcText,
				m_bWndActive,
				false,
				bHorz && !bGripperAtTop,
				true,
				sCaption.IsEmpty() ? LPCTSTR( NULL ) : sCaption
				);
			g_PaintManager->PaintGripper( mdc, _pgd );
			m_pBar->NcButtons_Paint( mdc );
			mdc.SelectClipRgn( NULL );
		}
	} // if( !bEmptyMode )

}

void CExtDynAutoHideSlider::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	ASSERT_VALID( this );
	pWnd;

	if( m_pBar == NULL )
		return;
	ASSERT( m_pBar );
	ASSERT( m_pBar->GetSafeHwnd() );
	ASSERT( ::IsWindow(m_pBar->GetSafeHwnd()) );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
		return;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if( CExtControlBar::FindHelpMode(this) )
		return;

CFrameWnd * pFrame = STATIC_DOWNCAST( CFrameWnd, GetParent() );
	ASSERT_VALID( pFrame );
HWND hWndTrack = pFrame->GetSafeHwnd();
	ASSERT( hWndTrack != NULL && ::IsWindow(hWndTrack) );
CExtPopupMenuWnd * pPopup = new CExtPopupMenuWnd;
	VERIFY( pPopup->CreatePopupMenu(hWndTrack) );

	if(	! CExtDockBar::_ConstructDockSiteControlBarPopupMenu(
			pFrame,
			pPopup,
			CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_AUTOHIDESLIDER_CTX,
			m_pBar,
			WM_CONTEXTMENU,
			NULL
			)
		)
	{
		delete pPopup;
		return;
	}
	if( pPopup->ItemGetCount() == 0 )
	{
		delete pPopup;
		return;
	}
//	::SetFocus( hWndTrack );
	if( ! pPopup->TrackPopupMenu(
			TPMX_OWNERDRAW_FIXED,
			point.x,
			point.y
			)
		)
	{
		delete pPopup;
	}
}

void CExtDynAutoHideSlider::OnCancelMode() 
{
	ASSERT_VALID( this );

	CWnd::OnCancelMode();
	
	if( !m_bActiveState
		|| m_bAnimation
		|| m_nResizingMode != HTCLIENT
		)
		return;

	ASSERT_VALID( this );
	ASSERT_VALID( m_pBar );
	ASSERT_KINDOF( CExtControlBar, m_pBar );
	ASSERT( !m_pBar->IsFixedMode() );
	
	Activate( m_pBar, false, false, true );
}

void CExtDynAutoHideSlider::OnClose() 
{
//	CWnd::OnClose();
}

void CExtDynAutoHideSlider::OnDestroy() 
{
	SendMessage( WM_CANCELMODE );
	CWnd::OnDestroy();
}

void CExtDynAutoHideSlider::OnTimer(UINT nIDEvent) 
{
//static int nn = 0;
//TRACE2( "  %d  -- CExtDynAutoHideSlider::OnTimer(%d)\n", nn++, nIDEvent );

	switch( nIDEvent )
	{
	case 1:
	{
		if( !m_bActiveState || (GetStyle() & WS_VISIBLE) == 0 )
		{
			KillTimer( 1 );
			return;
		}
		if( m_nResizingMode != HTCLIENT
			|| m_bAnimation
			)
			return;
//		if( m_pLastHitTestM != NULL
//			|| m_pLastHitTestB != NULL
//			)
//			_AnalyzeButtonsHover();
		if(		CExtControlBar::_DraggingGetBar() != NULL
			||	CExtControlBar::FindPrintPreviewMode(
					STATIC_DOWNCAST( CFrameWnd, GetParent() )
					)
			)
		{
			SendMessage( WM_CANCELMODE );
			return;
		}
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		CExtCustomizeSite * pSite =
			CExtCustomizeSite::GetCustomizeSite( m_hWnd );
		if(		pSite != NULL
			&&	pSite->IsCustomizeMode()
			)
		{
			SendMessage( WM_CANCELMODE );
			return;
		}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	
		HWND hWndFocus = ::GetFocus();
		bool bActive =
			(	GetSafeHwnd() == hWndFocus
			||	::IsChild( GetSafeHwnd(), hWndFocus )
			) ? true : false;
		if( bActive == m_bWndActive )
		{
			if( m_bWndActive || m_bDeactivateTesting )
				return;
			CPoint ptCursor;
			if( ! ::GetCursorPos(&ptCursor) )
				return;
			CRect rcWnd;
			GetWindowRect( &rcWnd );
			if( rcWnd.PtInRect(ptCursor) )
				return;
			ASSERT_VALID( m_pWndAnimateSrc );
			m_pWndAnimateSrc->GetWindowRect( &rcWnd );
			rcWnd.InflateRect( 2, 2 );
			if( rcWnd.PtInRect(ptCursor) )
				return;
			m_bDeactivateTesting = true;
			SetTimer( 2, 500, NULL );
			return;
		} // if( bActive == m_bWndActive )
		m_bWndActive = bActive;
		OnNcPaint();
		return;
	}
	case 2:
	{
		if( CExtToolControlBar::g_bMenuTracking
			|| CExtPopupMenuWnd::IsMenuTracking()
			)
			return;

		m_bDeactivateTesting = false;
		KillTimer( 2 );
		if( !m_bActiveState || (GetStyle() & WS_VISIBLE) == 0 )
			return;
		CPoint ptCursor;
		if( ! ::GetCursorPos(&ptCursor) )
			return;
		CRect rcWnd;
		GetWindowRect( &rcWnd );
		if( rcWnd.PtInRect(ptCursor) )
			return;
		ASSERT_VALID( m_pWndAnimateSrc );
		m_pWndAnimateSrc->GetWindowRect( &rcWnd );
		rcWnd.InflateRect( 2, 2 );
		if( rcWnd.PtInRect(ptCursor) )
			return;
		ASSERT_VALID( m_pBar );
		Activate( m_pBar, false, true, true );
		return;
	}
	} // switch( nIDEvent )
	
	CWnd::OnTimer(nIDEvent);
}

bool CExtDynAutoHideSlider::_HookMouseClick(
	UINT nMkCode,
	bool bDown,
	bool bNcArea,
	CPoint ptScreen
	)
{
	ASSERT_VALID( this );
	if( !m_bActiveState )
		return false;
	if( (GetStyle() & WS_VISIBLE) == 0 )
		return false;
	ASSERT_VALID( m_pWndAnimateSrc );
	ASSERT_VALID( m_pBar );
	bNcArea;

bool bRetVal = false;
	switch( nMkCode )
	{
	case MK_RBUTTON:

		{ // block: prepare focus for context menu
			HWND hWndFromPoint = ::WindowFromPoint(ptScreen);
			if(		hWndFromPoint != NULL
				&&	(	hWndFromPoint == GetSafeHwnd()
					||	::IsChild( GetSafeHwnd(), hWndFromPoint )
					)
				)
			{
				HWND hWndFocus = ::GetFocus();
				if(		hWndFocus != GetSafeHwnd()
					&&	( ! ::IsChild( GetSafeHwnd(), hWndFocus ) )
					)
				{
					::SetFocus( GetSafeHwnd() );
				}
			} // if( ::WindowFromPoint(ptScreen) == GetSafeHwnd() )
		} // block: prepare focus for context menu

	case MK_MBUTTON:
		m_pLastHitTestB = NULL;
	break;
	case MK_LBUTTON:
	{
		CRect rcWnd;
		GetWindowRect( &rcWnd );
		CPoint ptWnd( ptScreen - rcWnd.TopLeft() );

		HWND hWnd = ::WindowFromPoint( ptScreen );
		bool bHandleThisMsg = false;
		if(		hWnd == GetSafeHwnd()
			||	::IsChild( GetSafeHwnd(), hWnd )
			)
			bHandleThisMsg = true;

		if( bDown )
		{
			if( m_nResizingMode == HTCLIENT )
			{
				if( !bHandleThisMsg )
					return false;
				UINT nNcHitTest = OnNcHitTest( ptScreen );
				switch( nNcHitTest )
				{
				case HTBOTTOM:
				case HTTOP:
				{
					CPoint ptFrame( ptScreen );
					CFrameWnd * pFrame =
						STATIC_DOWNCAST( CFrameWnd, GetParent() );
					pFrame->ScreenToClient( &ptFrame );
					m_nResizingMode = nNcHitTest;
					m_nResizingStart
						= m_nResizingMetric
						= ptFrame.y;
					return true;
				}
				case HTLEFT:
				case HTRIGHT:
				{
					CPoint ptFrame( ptScreen );
					CFrameWnd * pFrame =
						STATIC_DOWNCAST( CFrameWnd, GetParent() );
					pFrame->ScreenToClient( &ptFrame );
					m_nResizingMode = nNcHitTest;
					m_nResizingStart
						= m_nResizingMetric
						= ptFrame.x;
					return true;
				}
				} // switch( nNcHitTest )
			} // if( m_nResizingMode == HTCLIENT )

			if( !bHandleThisMsg || !rcWnd.PtInRect(ptScreen) )
			{
				m_pLastHitTestB = NULL;
				break;
			}

			m_pBar->NcButtons_HitTest( ptScreen, &m_pLastHitTestB );
			if( m_pLastHitTestB == NULL )
			{
				CRect rcGrip;
				CalcInnerNcRects( NULL, NULL, rcGrip );
				if( !rcGrip.IsRectEmpty()
					&& rcGrip.PtInRect(ptWnd)
					)
				{
					HWND hWndFocus = GetSafeHwnd();
					HWND hWndChild = ::GetWindow( hWndFocus, GW_CHILD );
					if( hWndChild != NULL )
						hWndFocus = hWndChild;
					::SetFocus( hWndFocus );
					return true;
				}
				break;
			}
			OnNcPaint();
			return true;
		} // if( bDown )
		else
		{
			if( m_nResizingMode != HTCLIENT )
			{
				m_nResizingMode = HTCLIENT;
				return true;
			}
		
			if( !rcWnd.PtInRect(ptScreen) )
			{
				m_pLastHitTestB = NULL;
				break;
			}
			
			if( m_pLastHitTestB == NULL )
				break;
			CExtBarNcAreaButton * pLastHitTestB = NULL;
			m_pBar->NcButtons_HitTest( ptScreen, &pLastHitTestB );
			if( pLastHitTestB == NULL
				|| pLastHitTestB != m_pLastHitTestB
				)
			{
				if( m_pLastHitTestB != NULL )
				{
					m_pLastHitTestB = NULL;
					OnNcPaint();
				}
				break;
			}
			ASSERT_VALID( pLastHitTestB );
			m_pLastHitTestB = NULL;
			CPoint ptDevOffset = -rcWnd.TopLeft();
			CPoint ptWnd( ptScreen + ptDevOffset );
			Activate( m_pBar, false, false, true );
			pLastHitTestB->OnNcAreaClicked( ptWnd );
			return true;
		} // else from if( bDown )
	}
	break;
	}
	return bRetVal;
}

bool CExtDynAutoHideSlider::_HookMouseMove(
	bool bNcArea,
	CPoint ptScreen
	)
{
	ASSERT_VALID( this );
	if( !m_bActiveState )
		return false;
	if( (GetStyle() & WS_VISIBLE) == 0 )
		return false;
	ASSERT_VALID( m_pWndAnimateSrc );
	ASSERT_VALID( m_pBar );
	bNcArea;

	
	if( m_nResizingMode != HTCLIENT )
	{
		CRect rcWnd;
		GetWindowRect( &rcWnd );
		CRect rcTarget( &rcWnd );;
//		CPoint ptWnd( ptScreen - rcWnd.TopLeft() );

		CPoint ptFrame( ptScreen );
		CFrameWnd * pFrame =
			STATIC_DOWNCAST( CFrameWnd, GetParent() );
		pFrame->ScreenToClient( &ptFrame );
		pFrame->ScreenToClient( &rcTarget );

#define __AH_RP_MIN_GAP_DX 20
#define __AH_RP_MIN_GAP_DY 20
		CRect rcTrackMargin( m_rcMax );
		rcTrackMargin.DeflateRect(
			__AH_RP_MIN_GAP_DX,
			__AH_RP_MIN_GAP_DY
			);

		INT nNewMertic = 0;
		switch( m_nResizingMode )
		{
		case HTTOP:
		case HTBOTTOM:
		{
			nNewMertic = ptFrame.y;
			if( nNewMertic < rcTrackMargin.top )
				nNewMertic = rcTrackMargin.top;
			else if( nNewMertic > rcTrackMargin.bottom )
				nNewMertic = rcTrackMargin.bottom;
			((m_nResizingMode == HTTOP) ? rcTarget.top : rcTarget.bottom) = nNewMertic;
			nNewMertic += (m_nResizingMode == HTTOP) ? (+1) : (-1);
			break;
		}
		case HTLEFT:
		case HTRIGHT:
		{
			nNewMertic = ptFrame.x;
			if( nNewMertic < rcTrackMargin.left )
				nNewMertic = rcTrackMargin.left;
			else if( nNewMertic > rcTrackMargin.right )
				nNewMertic = rcTrackMargin.right;
			((m_nResizingMode == HTLEFT) ? rcTarget.left : rcTarget.right) = nNewMertic;
			nNewMertic += (m_nResizingMode == HTLEFT) ? (+1) : (-1);
			break;
		}
#ifdef _DEBUG
		default:
			ASSERT( FALSE );
			return true;
#endif // _DEBUG
		}
		if( rcTarget != rcWnd )
		{
			SetWindowPos(
				NULL,
				rcTarget.left, rcTarget.top,
				rcTarget.Width(), rcTarget.Height(),
				SWP_NOREPOSITION|SWP_NOZORDER|SWP_NOCOPYBITS
				);
			RedrawWindow(
				NULL, NULL,
				RDW_INVALIDATE|RDW_UPDATENOW
					|RDW_ERASE|RDW_ERASENOW
					|RDW_ALLCHILDREN|RDW_NOFRAME
				);
		}
		return true;
	
	} // if( m_nResizingMode != HTCLIENT )
	else
		_AnalyzeButtonsHover();

	return false;
}

CExtDynAutoHideSlider * CExtDynAutoHideSlider::g_pHookTarget = NULL;
HHOOK CExtDynAutoHideSlider::g_hMouseHook = NULL;

LRESULT CALLBACK CExtDynAutoHideSlider::_HookMouseProc(
	int nCode,      // hook code
	WPARAM wParam,  // message identifier
	LPARAM lParam   // mouse coordinates
	)
{
	__PROF_UIS_MANAGE_STATE;

MOUSEHOOKSTRUCT* lpMS = (MOUSEHOOKSTRUCT*)lParam;
	ASSERT( lpMS != NULL );

	if( g_pHookTarget != NULL
		&& g_pHookTarget->GetSafeHwnd() != NULL
		&& ::IsWindow( g_pHookTarget->GetSafeHwnd() )
		&& g_pHookTarget->m_bActiveState
		&& (g_pHookTarget->GetStyle() & WS_VISIBLE)
		)
	{
		ASSERT_VALID( g_pHookTarget );
		switch( wParam )
		{
		
		case WM_NCLBUTTONDOWN:
		{
			CPoint ptScreen;
			if( ! ::GetCursorPos(&ptScreen) )
				return -1; // eat!
			if(	g_pHookTarget->_HookMouseClick( MK_LBUTTON, true, true, ptScreen ) )
				return -1; // eat!
			break;
		}
		case WM_NCLBUTTONUP:
		{
			CPoint ptScreen;
			if( ! ::GetCursorPos(&ptScreen) )
				return -1; // eat!
			if(	g_pHookTarget->_HookMouseClick( MK_LBUTTON, false, true, ptScreen ) )
				return -1; // eat!
			break;
		}
		case WM_NCMBUTTONDOWN:
		{
			CPoint ptScreen;
			if( ! ::GetCursorPos(&ptScreen) )
				return -1; // eat!
			if(	g_pHookTarget->_HookMouseClick( MK_MBUTTON, true, true, ptScreen ) )
				return -1; // eat!
			break;
		}
		case WM_NCMBUTTONUP:
		{
			CPoint ptScreen;
			if( ! ::GetCursorPos(&ptScreen) )
				return -1; // eat!
			if(	g_pHookTarget->_HookMouseClick( MK_MBUTTON, false, true, ptScreen ) )
				return -1; // eat!
			break;
		}
		case WM_NCRBUTTONDOWN:
		{
			CPoint ptScreen;
			if( ! ::GetCursorPos(&ptScreen) )
				return -1; // eat!
			if(	g_pHookTarget->_HookMouseClick( MK_RBUTTON, true, true, ptScreen ) )
				return -1; // eat!
			break;
		}
		case WM_NCRBUTTONUP:
		{
			CPoint ptScreen;
			if( ! ::GetCursorPos(&ptScreen) )
				return -1; // eat!
			if(	g_pHookTarget->_HookMouseClick( MK_RBUTTON, false, true, ptScreen ) )
				return -1; // eat!
			break;
		}
		case WM_LBUTTONDOWN:
		{
			CPoint ptScreen;
			if( ! ::GetCursorPos(&ptScreen) )
				return -1; // eat!
			if(	g_pHookTarget->_HookMouseClick( MK_LBUTTON, true, false, ptScreen ) )
				return -1; // eat!
			break;
		}
		case WM_LBUTTONUP:
		{
			CPoint ptScreen;
			if( ! ::GetCursorPos(&ptScreen) )
				return -1; // eat!
			if(	g_pHookTarget->_HookMouseClick( MK_LBUTTON, false, false, ptScreen ) )
				return -1; // eat!
			break;
		}
		case WM_MBUTTONDOWN:
		{
			CPoint ptScreen;
			if( ! ::GetCursorPos(&ptScreen) )
				return -1; // eat!
			if(	g_pHookTarget->_HookMouseClick( MK_MBUTTON, true, false, ptScreen ) )
				return -1; // eat!
			break;
		}
		case WM_MBUTTONUP:
		{
			CPoint ptScreen;
			if( ! ::GetCursorPos(&ptScreen) )
				return -1; // eat!
			if(	g_pHookTarget->_HookMouseClick( MK_MBUTTON, false, false, ptScreen ) )
				return -1; // eat!
			break;
		}
		case WM_RBUTTONDOWN:
		{
			CPoint ptScreen;
			if( ! ::GetCursorPos(&ptScreen) )
				return -1; // eat!
			if(	g_pHookTarget->_HookMouseClick( MK_RBUTTON, true, false, ptScreen ) )
				return -1; // eat!
			break;
		}
		case WM_RBUTTONUP:
		{
			CPoint ptScreen;
			if( ! ::GetCursorPos(&ptScreen) )
				return -1; // eat!
			if(	g_pHookTarget->_HookMouseClick( MK_RBUTTON, false, false, ptScreen ) )
				return -1; // eat!
			break;
		}
		
		case WM_MOUSEMOVE:
		{
			CPoint ptScreen;
			if( ! ::GetCursorPos(&ptScreen) )
				return -1; // eat!
			if(	g_pHookTarget->_HookMouseMove( false, ptScreen ) )
				return -1; // eat!
			break;
		}
		case WM_NCMOUSEMOVE:
		{
			CPoint ptScreen;
			if( ! ::GetCursorPos(&ptScreen) )
				return -1; // eat!
			if(	g_pHookTarget->_HookMouseMove( true, ptScreen ) )
				return -1; // eat!
			break;
		}

		} // switch( wParam )
	}

	return
		::CallNextHookEx(
			g_hMouseHook,
			nCode,
			wParam,
			lParam
			);
}

void CExtDynAutoHideSlider::Activate(
	CExtControlBar * pBar,
	bool bActivate,
	bool bAnimate,
	bool bWait
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
	ASSERT_KINDOF( CExtControlBar, pBar );
	// (+ v.2.24)
	// fix for VC 7.10
	if(	pBar->GetSafeHwnd() == NULL
		||	(! ::IsWindow(pBar->GetSafeHwnd()) )
		)
		return;
	ASSERT( !pBar->IsFixedMode() );

	ASSERT( CExtControlBar::g_nAutoHideAnimationStepCount >= 0 );
	ASSERT( CExtControlBar::g_nAutoHideAnimationStepTime >= 0 );
	if(		CExtControlBar::g_nAutoHideAnimationStepCount == 0
		||	CExtControlBar::g_nAutoHideAnimationStepTime == 0
		)
		bAnimate = false;
	if( bAnimate )
	{ // optionally cancel animation
		if( bActivate )
		{
			if( CExtControlBar::g_bDisableAutoHideAnimationExpanding )
				bAnimate = false;
		} // if( bActivate )
		else
		{
			if( CExtControlBar::g_bDisableAutoHideAnimationCollapsing )
				bAnimate = false;
		} // else from if( bActivate )
	} // optionally cancel animation

CFrameWnd * pFrame =
		STATIC_DOWNCAST( CFrameWnd, GetParent() );

	if( bActivate )
	{
		CExtPopupMenuWnd::CancelMenuTracking();
		CWnd::CancelToolTips();

		if( m_pBar == pBar )
			return;
		if( m_bActiveState )
		{
			ASSERT_VALID( m_pBar );
			ASSERT_KINDOF( CExtControlBar, m_pBar );
			ASSERT( !m_pBar->IsFixedMode() );
			Activate( m_pBar, false, bAnimate, true );
			ASSERT( !m_bActiveState );
		} // if( m_bActiveState )
		ASSERT( m_pBar == NULL );

		CExtDockBar * vOuterDockBars[4];
		CExtDynAutoHideArea * vAutoHideAreas[4];
		vOuterDockBars[0] = STATIC_DOWNCAST( CExtDockBar, pFrame->GetControlBar( AFX_IDW_DOCKBAR_TOP ) );
		vOuterDockBars[1] = STATIC_DOWNCAST( CExtDockBar, pFrame->GetControlBar( AFX_IDW_DOCKBAR_BOTTOM ) );
		vOuterDockBars[2] = STATIC_DOWNCAST( CExtDockBar, pFrame->GetControlBar( AFX_IDW_DOCKBAR_LEFT ) );
		vOuterDockBars[3] = STATIC_DOWNCAST( CExtDockBar, pFrame->GetControlBar( AFX_IDW_DOCKBAR_RIGHT ) );
		CRect rcTrack(0,0,0,0), rcMax(0,0,0,0);
		pFrame->GetClientRect( &rcTrack );
//		HWND hWndInsertAfter = NULL;
		for( INT i = 0; i < 4; i++ )
		{
			ASSERT_VALID( vOuterDockBars[i] );
			ASSERT_KINDOF( CExtDockBar, vOuterDockBars[i] );
			ASSERT( vOuterDockBars[i]->_GetCircleNo() == 0 );
			vAutoHideAreas[i] = vOuterDockBars[i]->_GetAutoHideArea();
			ASSERT_VALID( vAutoHideAreas[i] );
			ASSERT_KINDOF( CExtDynAutoHideArea, vAutoHideAreas[i] );
			UINT nID = vOuterDockBars[i]->GetDlgCtrlID();
			ASSERT(
				UINT( vAutoHideAreas[i]->GetDlgCtrlID() )
					== nID
				);
//			ASSERT( hWndInsertAfter == NULL );
//			if( i == 3 )
//			{
//				hWndInsertAfter = vAutoHideAreas[i]->GetSafeHwnd();
//				ASSERT( hWndInsertAfter != NULL );
//				ASSERT( ::IsWindow( hWndInsertAfter ) );
//			}
			CRect rcInnerArea;
			if( vAutoHideAreas[i]->GetStyle() & WS_VISIBLE )
			{
				vAutoHideAreas[i]->GetWindowRect( &rcInnerArea );
				pFrame->ScreenToClient( &rcInnerArea );
				switch( nID )
				{
				case AFX_IDW_DOCKBAR_TOP:
					rcMax.top = rcInnerArea.bottom;
					break;
				case AFX_IDW_DOCKBAR_BOTTOM:
					rcMax.bottom = rcInnerArea.top;
					break;
				case AFX_IDW_DOCKBAR_LEFT:
					rcMax.left = rcInnerArea.right;
					break;
				case AFX_IDW_DOCKBAR_RIGHT:
					rcMax.right = rcInnerArea.left;
					break;
#ifdef _DEBUG
				default:
					ASSERT( FALSE );
					break;
#endif // _DEBUG
				} // switch( nID )
//				if(	!(		rcInnerArea.left >= rcTrack.left
//						&&	rcInnerArea.top >= rcTrack.top
//						&&	rcInnerArea.right <= rcTrack.right
//						&&	rcInnerArea.bottom <= rcTrack.bottom
//						)
//					)
//					return;
			}
			else if( vOuterDockBars[i]->GetStyle() & WS_VISIBLE )
			{
				vOuterDockBars[i]->GetWindowRect( &rcInnerArea );
				pFrame->ScreenToClient( &rcInnerArea );
				switch( nID )
				{
				case AFX_IDW_DOCKBAR_TOP:
					rcMax.top = rcInnerArea.bottom;
					break;
				case AFX_IDW_DOCKBAR_BOTTOM:
					rcMax.bottom = rcInnerArea.top;
					break;
				case AFX_IDW_DOCKBAR_LEFT:
					rcMax.left = rcInnerArea.right;
					break;
				case AFX_IDW_DOCKBAR_RIGHT:
					rcMax.right = rcInnerArea.left;
					break;
#ifdef _DEBUG
				default:
					ASSERT( FALSE );
					break;
#endif // _DEBUG
				} // switch( nID )
//				if(	!(		rcInnerArea.left >= rcTrack.left
//						&&	rcInnerArea.top >= rcTrack.top
//						&&	rcInnerArea.right <= rcTrack.right
//						&&	rcInnerArea.bottom <= rcTrack.bottom
//						)
//					)
//					return;
			}
			else
			{
//				continue;

				switch( nID )
				{
				case AFX_IDW_DOCKBAR_TOP:
					rcMax.top = rcTrack.top;
					break;
				case AFX_IDW_DOCKBAR_BOTTOM:
					rcMax.bottom = rcTrack.bottom;
					break;
				case AFX_IDW_DOCKBAR_LEFT:
					rcMax.left = rcTrack.left;
					break;
				case AFX_IDW_DOCKBAR_RIGHT:
					rcMax.right = rcTrack.right;
					break;
#ifdef _DEBUG
				default:
					ASSERT( FALSE );
					break;
#endif // _DEBUG
				} // switch( nID )

			}
			switch( nID )
			{
			case AFX_IDW_DOCKBAR_TOP:
				rcTrack.top =
					max( rcTrack.top, rcInnerArea.bottom );
				break;
			case AFX_IDW_DOCKBAR_BOTTOM:
				rcTrack.bottom =
					min( rcTrack.bottom, rcInnerArea.top );
				break;
			case AFX_IDW_DOCKBAR_LEFT:
				rcTrack.left =
					max( rcTrack.left, rcInnerArea.right );
				break;
			case AFX_IDW_DOCKBAR_RIGHT:
				rcTrack.right =
					min( rcTrack.right, rcInnerArea.left );
				break;
#ifdef _DEBUG
			default:
				ASSERT( FALSE );
				break;
#endif // _DEBUG
			}
		}
		
		rcTrack.left = max( rcTrack.left, rcMax.left );
		rcTrack.top = max( rcTrack.top, rcMax.top );
		rcTrack.right = min( rcTrack.right, rcMax.right );
		rcTrack.bottom = min( rcTrack.bottom, rcMax.bottom );

		if( (rcTrack.right - rcTrack.left) < 20 )
			return;
		if( (rcTrack.bottom - rcTrack.top) < 20 )
			return;

//		CRect rcBarTmp;
//		pBar->GetWindowRect( &rcBarTmp );
//		CSize sizeControlBar = rcBarTmp.Size();
		
		ASSERT_VALID( pBar->m_pDockBar );
		ASSERT_KINDOF( CExtDockBar, pBar->m_pDockBar );
		CExtDynAutoHideArea *pWndAnimateSrc = 
			((CExtDockBar *)pBar->m_pDockBar)->
				_GetAutoHideArea();
		ASSERT_VALID( pWndAnimateSrc );
		
		UINT nAutoHiderID = pWndAnimateSrc->GetDlgCtrlID();
		ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nAutoHiderID );
		CSize sizeControlBar( 0, 0 );
		if( pBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockDynBar)) )
		{
			CExtControlBar * pOtherBar =
				STATIC_DOWNCAST(
					CExtControlBar,
					pBar->m_pDockBar->GetParent()
					);
			ASSERT_VALID( pOtherBar );
			ASSERT( !pOtherBar->IsFixedMode() );
			sizeControlBar = 
				( nAutoHiderID == AFX_IDW_DOCKBAR_TOP || nAutoHiderID == AFX_IDW_DOCKBAR_BOTTOM )
					? pOtherBar->m_sizeDockedH
					: pOtherBar->m_sizeDockedV
					;
		}
		else
			sizeControlBar = 
				( nAutoHiderID == AFX_IDW_DOCKBAR_TOP || nAutoHiderID == AFX_IDW_DOCKBAR_BOTTOM )
					? pBar->m_sizeDockedH
					: pBar->m_sizeDockedV
					;

		if( sizeControlBar.cx < 20 )
			sizeControlBar.cx = 20;
		if( sizeControlBar.cy < 20 )
			sizeControlBar.cy = 20;
		if( sizeControlBar.cx > rcMax.Width() )
			sizeControlBar.cx = rcMax.Width();
		if( sizeControlBar.cy > rcMax.Height() )
			sizeControlBar.cy = rcMax.Height();
		if(		sizeControlBar.cx < 20 
			||	sizeControlBar.cy < 20
			)
			return;

		m_bActiveState = true;
		m_pBar = pBar;
		m_rcMax = rcMax;
		ASSERT( m_pWndAnimateSrc == NULL );
		m_pWndAnimateSrc = pWndAnimateSrc;
		ASSERT( m_pWndAnimateSrc->m_pWndSlider == this );
		UINT nID = UINT( m_pWndAnimateSrc->GetDlgCtrlID() );
		m_sizeControlBar = sizeControlBar;
		//m_pBar->GetClientRect( &rcBarTmp );
		//CSize sizeNcAdd( m_sizeControlBar - rcBarTmp.Size() );
		RECT rcNcSizes;
		CalcNcOffsets( &rcNcSizes );
		CSize sizeNcAdd( 0, 0 );
		sizeNcAdd.cx += rcNcSizes.left + rcNcSizes.right;
		sizeNcAdd.cy += rcNcSizes.top + rcNcSizes.bottom;

		switch( nID )
		{
		case AFX_IDW_DOCKBAR_TOP:
			m_sizeControlBar.cx = rcTrack.Width();
			rcTrack.bottom =
				rcTrack.top
				+ m_sizeControlBar.cy
				//+ sizeNcAdd.cy
				;
			break;
		case AFX_IDW_DOCKBAR_BOTTOM:
			m_sizeControlBar.cx = rcTrack.Width();
			rcTrack.top =
				rcTrack.bottom
				- m_sizeControlBar.cy
				//- sizeNcAdd.cy
				;
			break;
		case AFX_IDW_DOCKBAR_LEFT:
			m_sizeControlBar.cy = rcTrack.Height();
			rcTrack.right =
				rcTrack.left
				+ m_sizeControlBar.cx
				//+ sizeNcAdd.cx
				;
			break;
		case AFX_IDW_DOCKBAR_RIGHT:
			m_sizeControlBar.cy = rcTrack.Height();
			rcTrack.left =
				rcTrack.right
				- m_sizeControlBar.cx
				//- sizeNcAdd.cx
				;
			break;
#ifdef _DEBUG
		default:
			ASSERT( FALSE );
			break;
#endif // _DEBUG
		}
		
		m_sizeControlBar -= sizeNcAdd;

		HWND hWndChild = ::GetWindow( m_pBar->GetSafeHwnd(), GW_CHILD );

		if( hWndChild != NULL )
		{
			ASSERT( ::IsWindow(hWndChild) );
			::SetParent( hWndChild, GetSafeHwnd() );
//			DWORD dwChildStyles =
//				(DWORD)::GetWindowLong( hWndChild, GWL_STYLE );
//			if( (dwChildStyles & WS_VISIBLE) == 0 )
//				::ShowWindow( hWndChild, SW_SHOWNA );
			::SetWindowPos(
				hWndChild, HWND_TOP, 0, 0, 0, 0,
				SWP_NOSIZE|SWP_NOMOVE|SWP_NOREPOSITION
					|SWP_FRAMECHANGED|SWP_SHOWWINDOW
					|SWP_NOACTIVATE
				);
		} // if( hWndChild != NULL )

		bool bRecalcFrame = true;
		if( bAnimate )
		{
			m_bAnimation = true;
			clock_t nLastAnimTime = clock();
			for( INT nStep = 0; nStep < CExtControlBar::g_nAutoHideAnimationStepCount; nStep++ )
			{
				CRect rcAnim( rcTrack );
				CPoint ptMoveChild( 0, 0 );
				switch( nID )
				{
				case AFX_IDW_DOCKBAR_TOP:
					rcAnim.bottom =
						rcAnim.top
						+ ::MulDiv( rcAnim.Height(), nStep, CExtControlBar::g_nAutoHideAnimationStepCount+1 )
						;
					break;
				case AFX_IDW_DOCKBAR_BOTTOM:
					rcAnim.top =
						rcAnim.bottom
						- ::MulDiv( rcAnim.Height(), nStep, CExtControlBar::g_nAutoHideAnimationStepCount+1 )
						;
					break;
				case AFX_IDW_DOCKBAR_LEFT:
					rcAnim.right =
						rcAnim.left
						+ ::MulDiv( rcAnim.Width(), nStep, CExtControlBar::g_nAutoHideAnimationStepCount+1 )
						;
					break;
				case AFX_IDW_DOCKBAR_RIGHT:
					rcAnim.left =
						rcAnim.right
						- ::MulDiv( rcAnim.Width(), nStep, CExtControlBar::g_nAutoHideAnimationStepCount+1 )
						;
					break;
#ifdef _DEBUG
				default:
					ASSERT( FALSE );
					break;
#endif // _DEBUG
				}
				::SetWindowPos(
					GetSafeHwnd(),
					HWND_TOP,
					rcAnim.left, rcAnim.top,
					rcAnim.Width(), rcAnim.Height(),
					SWP_SHOWWINDOW|SWP_NOCOPYBITS
						| (bRecalcFrame ? SWP_FRAMECHANGED : 0 )
					);
				bRecalcFrame = false;
				if( hWndChild != NULL )
				{
					ASSERT( ::IsWindow(hWndChild) );
					CRect rcMoveChild;
					GetClientRect( &rcMoveChild );
					switch( nID )
					{
					case AFX_IDW_DOCKBAR_TOP:
						rcMoveChild.top =
							rcMoveChild.bottom
							- m_sizeControlBar.cy
							;
						break;
					case AFX_IDW_DOCKBAR_LEFT:
						rcMoveChild.left =
							rcMoveChild.right
							- m_sizeControlBar.cx
							;
						break;
					case AFX_IDW_DOCKBAR_BOTTOM:
					case AFX_IDW_DOCKBAR_RIGHT:
						rcMoveChild.SetRect(
							0,0,m_sizeControlBar.cx,m_sizeControlBar.cy
							);
						break;
#ifdef _DEBUG
					default:
						ASSERT( FALSE );
						break;
#endif // _DEBUG
					}
					::SetWindowPos(
						hWndChild,
						HWND_BOTTOM,
						rcMoveChild.left, rcMoveChild.top,
						rcMoveChild.Width(), rcMoveChild.Height(),
						SWP_SHOWWINDOW|SWP_NOCOPYBITS
							//|SWP_FRAMECHANGED
						);
					RedrawWindow(
						NULL, NULL,
						RDW_INVALIDATE|RDW_UPDATENOW
							|RDW_ERASE|RDW_ERASENOW
							|RDW_ALLCHILDREN|RDW_NOFRAME
						);
				} // if( hWndChild != NULL )

				CExtPaintManager::stat_PassPaintMessages();
				clock_t nNewAnimTime = clock();
				clock_t nDuration =
					nLastAnimTime - nNewAnimTime;
				if( nDuration < CExtControlBar::g_nAutoHideAnimationStepTime )
					::Sleep( CExtControlBar::g_nAutoHideAnimationStepTime - nDuration );
				nLastAnimTime = clock();
			}
			m_bAnimation = false;
		} // if( bAnimate )

		if( hWndChild != NULL )
		{
			ASSERT( ::IsWindow(hWndChild) );
			CRect rcClient;
			GetClientRect( &rcClient );
			::SetWindowPos(
				hWndChild,
				HWND_BOTTOM,
				0, 0,
				m_sizeControlBar.cx, m_sizeControlBar.cy,
				//rcClient.Width(), rcClient.Height(),
				SWP_SHOWWINDOW|SWP_NOCOPYBITS
					//|SWP_FRAMECHANGED
			);
			RedrawWindow(
				NULL, NULL,
				RDW_INVALIDATE|RDW_UPDATENOW
					|RDW_ERASE|RDW_ERASENOW
					|RDW_ALLCHILDREN|RDW_NOFRAME
				);
		} // if( hWndChild != NULL )

//		ASSERT( hWndInsertAfter != NULL );
//		ASSERT( ::IsWindow( hWndInsertAfter ) );
		::SetWindowPos(
			GetSafeHwnd(),
			HWND_TOP, // hWndInsertAfter,
			rcTrack.left, rcTrack.top,
			rcTrack.Width(), rcTrack.Height(),
			SWP_SHOWWINDOW|SWP_NOCOPYBITS
				| (bRecalcFrame ? SWP_FRAMECHANGED : 0 )
			);
		RedrawWindow(
			NULL, NULL,
			RDW_INVALIDATE|RDW_UPDATENOW
				|RDW_ERASE|RDW_ERASENOW
				|RDW_ALLCHILDREN|RDW_NOFRAME
			);

		ASSERT( g_hMouseHook == NULL );
		ASSERT( g_pHookTarget == NULL );
		g_pHookTarget = this;
		g_hMouseHook =
			::SetWindowsHookEx(
				WH_MOUSE,
				_HookMouseProc, 
				0,
				::GetCurrentThreadId()
				);
		ASSERT( g_hMouseHook != NULL );

		//m_pBar->OnNcAreaButtonsReposition();
		m_pBar->m_bWindowActive = false;
		INT nCountOfNcButtons = m_pBar->NcButtons_GetCount();
		for( INT nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )
		{
			CExtBarNcAreaButton * pBtn =
				m_pBar->NcButtons_GetAt( nBtn );
			//pBtn->m_bHover = pBtn->m_bPushed = false;
			pBtn->OnNcAreaHitTest( CPoint(-1,-1) );
		} // for( INT nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )

		SetTimer( 1, 100, NULL );

//		::SetFocus(
//			(hWndChild == NULL) ? GetSafeHwnd() : hWndChild
//			);
	} // if( bActivate )
	else
	{
		if( !m_bActiveState )
			return;

		CExtDynAutoHideArea * pDynAutoHideArea =
			CExtDynAutoHideArea::stat_FindAreaOfBar( pBar );
		if( pDynAutoHideArea != NULL )
			pDynAutoHideArea->SelectionSet( -1L );

		if( g_hMouseHook != NULL )
		{
			ASSERT( g_pHookTarget == this );
			::UnhookWindowsHookEx( g_hMouseHook );
			g_hMouseHook = NULL;
			g_pHookTarget = NULL;
		}

		if( bAnimate )
		{
			m_bAnimation = true;
			ASSERT_VALID( m_pWndAnimateSrc );
			ASSERT( m_pWndAnimateSrc->m_pWndSlider == this );
			UINT nID = UINT( m_pWndAnimateSrc->GetDlgCtrlID() );

			CRect rcTrack;
			GetWindowRect( &rcTrack );
			pFrame->ScreenToClient( &rcTrack );
			HWND hWndChild = ::GetWindow( GetSafeHwnd(), GW_CHILD );
			clock_t nLastAnimTime = clock();
			for( INT nStep = 0; nStep < CExtControlBar::g_nAutoHideAnimationStepCount; nStep++ )
			{
				CRect rcAnim( rcTrack );
				CPoint ptMoveChild( 0, 0 );
				INT nStepCalc = CExtControlBar::g_nAutoHideAnimationStepCount - nStep- 1;
				switch( nID )
				{
				case AFX_IDW_DOCKBAR_TOP:
					rcAnim.bottom =
						rcAnim.top
						+ ::MulDiv( rcAnim.Height(), nStepCalc, CExtControlBar::g_nAutoHideAnimationStepCount+1 )
						;
					break;
				case AFX_IDW_DOCKBAR_BOTTOM:
					rcAnim.top =
						rcAnim.bottom
						- ::MulDiv( rcAnim.Height(), nStepCalc, CExtControlBar::g_nAutoHideAnimationStepCount+1 )
						;
					break;
				case AFX_IDW_DOCKBAR_LEFT:
					rcAnim.right =
						rcAnim.left
						+ ::MulDiv( rcAnim.Width(), nStepCalc, CExtControlBar::g_nAutoHideAnimationStepCount+1 )
						;
					break;
				case AFX_IDW_DOCKBAR_RIGHT:
					rcAnim.left =
						rcAnim.right
						- ::MulDiv( rcAnim.Width(), nStepCalc, CExtControlBar::g_nAutoHideAnimationStepCount+1 )
						;
					break;
#ifdef _DEBUG
				default:
					ASSERT( FALSE );
					break;
#endif // _DEBUG
				}
				::SetWindowPos(
					GetSafeHwnd(),
					HWND_TOP,
					rcAnim.left, rcAnim.top,
					rcAnim.Width(), rcAnim.Height(),
					0 //SWP_SHOWWINDOW
					);
				if( hWndChild != NULL )
				{
					ASSERT( ::IsWindow(hWndChild) );
					CRect rcMoveChild;
					GetClientRect( &rcMoveChild );
					switch( nID )
					{
					case AFX_IDW_DOCKBAR_TOP:
						rcMoveChild.top =
							rcMoveChild.bottom
							- m_sizeControlBar.cy
							;
						break;
					case AFX_IDW_DOCKBAR_LEFT:
						rcMoveChild.left =
							rcMoveChild.right
							- m_sizeControlBar.cx
							;
						break;
					case AFX_IDW_DOCKBAR_BOTTOM:
					case AFX_IDW_DOCKBAR_RIGHT:
						rcMoveChild.SetRect(
							0,0,m_sizeControlBar.cx,m_sizeControlBar.cy
							);
						break;
#ifdef _DEBUG
					default:
						ASSERT( FALSE );
						break;
#endif // _DEBUG
					}
					::SetWindowPos(
						hWndChild,
						HWND_BOTTOM,
						rcMoveChild.left, rcMoveChild.top,
						rcMoveChild.Width(), rcMoveChild.Height(),
						0 //SWP_SHOWWINDOW //|SWP_FRAMECHANGED
						);
					RedrawWindow(
						NULL, NULL,
						RDW_INVALIDATE|RDW_UPDATENOW
							|RDW_ERASE|RDW_ERASENOW
							|RDW_ALLCHILDREN|RDW_NOFRAME
						);
				} // if( hWndChild != NULL )

				CExtPaintManager::stat_PassPaintMessages();
				clock_t nNewAnimTime = clock();
				clock_t nDuration =
					nLastAnimTime - nNewAnimTime;
				if( nDuration < CExtControlBar::g_nAutoHideAnimationStepTime )
					::Sleep( CExtControlBar::g_nAutoHideAnimationStepTime - nDuration );
				nLastAnimTime = clock();
			}
			m_bAnimation = false;
		} // if( bAnimate )

		ASSERT_VALID( m_pBar );
		m_pBar->m_bWindowActive = false;
		INT nCountOfNcButtons = m_pBar->NcButtons_GetCount();
		for( INT nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )
		{
			CExtBarNcAreaButton * pBtn =
				m_pBar->NcButtons_GetAt( nBtn );
			//pBtn->m_bHover = pBtn->m_bPushed = false;
			pBtn->OnNcAreaHitTest( CPoint(-1,-1) );
		} // for( INT nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )

		KillTimer( 1 );
		KillTimer( 2 );
		m_bDeactivateTesting = false;

		ASSERT_VALID( m_pWndAnimateSrc );
		ASSERT( m_pWndAnimateSrc->m_pWndSlider == this );

		HWND hWndChild = ::GetWindow( GetSafeHwnd(), GW_CHILD );
		bool bSetFrameFocus = false;
		if( hWndChild != NULL && pBar->GetSafeHwnd() != NULL )
		{
			HWND hWndFocus = ::GetFocus();
			if( hWndFocus != NULL
				&&	(	hWndFocus == GetSafeHwnd()
					||	::IsChild( GetSafeHwnd(), hWndFocus )
					)
				)
				bSetFrameFocus = true;
			ASSERT( ::IsWindow(hWndChild) );
			//::LockWindowUpdate( hWndChild );
			::SetParent( hWndChild, pBar->GetSafeHwnd() );
			//::LockWindowUpdate( NULL );
			CRect rcBarClient;
			pBar->GetClientRect( &rcBarClient );
			::SetWindowPos(
				hWndChild,
				HWND_BOTTOM,
				0, 0,
				rcBarClient.Width(), rcBarClient.Height(),
				SWP_SHOWWINDOW|SWP_NOCOPYBITS //|SWP_FRAMECHANGED
			);
		}

		SetWindowPos(
			NULL, 0,0,0,0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOREPOSITION
				|SWP_HIDEWINDOW
			);
		m_pWndAnimateSrc->SelectionSet( -1 );
		m_pWndAnimateSrc = NULL;
		m_pBar = NULL;
		m_bActiveState = false;
		m_bWndActive = false;
		m_bDeactivateTesting = false;
		m_nResizingMode = HTCLIENT;
		m_pLastHitTestM
			= m_pLastHitTestT
			= m_pLastHitTestB
			= NULL;
		m_rcMax.SetRectEmpty();
		
		if( bSetFrameFocus )
			pFrame->SetFocus();
	} // else from if( bActivate )

	bWait;
}

void CExtDynAutoHideSlider::_AnalyzeButtonsHover()
{
	ASSERT_VALID( this );

	if( !m_bActiveState
		|| m_pBar == NULL
		|| m_pBar->GetSafeHwnd() == NULL
		|| (! ::IsWindow(m_pBar->GetSafeHwnd()) )
		|| CExtToolControlBar::g_bMenuTracking
		|| CExtPopupMenuWnd::IsMenuTracking()
		|| CExtControlBar::_DraggingGetBar() != NULL
		)
	{
		if( m_pLastHitTestM != NULL )
		{
			m_pLastHitTestM = NULL;
			OnNcPaint();
		}
		return;
	}

CPoint point;
	if( ! ::GetCursorPos(&point) )
		return;

HWND hWnd = ::WindowFromPoint( point );
bool bHandleThisMsg = false;
	if(		hWnd == GetSafeHwnd()
		||	::IsChild( GetSafeHwnd(), hWnd )
		)
		bHandleThisMsg = true;
	if( !bHandleThisMsg )
	{
		if( m_pLastHitTestM != NULL )
		{
			m_pLastHitTestM = NULL;
			OnNcPaint();
		}
		return;
	}

CExtBarNcAreaButton * pBtnHitTest = NULL;
	m_pBar->NcButtons_HitTest( point, &pBtnHitTest );
	
	if( m_pLastHitTestM == pBtnHitTest )
		return;
	m_pLastHitTestM = pBtnHitTest;
	OnNcPaint();
}

// for command hit testing (used for automatic tooltips)
int CExtDynAutoHideSlider::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	ASSERT_VALID( this );

	if( pTI == NULL || m_wndToolTip.GetSafeHwnd() == NULL )
		return -1;

	if( !m_bActiveState
		|| m_pBar == NULL
		|| m_pBar->GetSafeHwnd() == NULL
		|| (! ::IsWindow(m_pBar->GetSafeHwnd()) )
		|| CExtToolControlBar::g_bMenuTracking
		|| CExtPopupMenuWnd::IsMenuTracking()
		|| CExtControlBar::_DraggingGetBar() != NULL
		)
		return -1;

	if( (m_pBar->GetBarStyle() & CBRS_TOOLTIPS) == 0 )
		return -1;

	if( ! ::GetCursorPos(&point) )
		return -1;
	//ScreenToClient( &point );

CExtBarNcAreaButton * pBtnHitTest = NULL;
	( const_cast < CExtControlBar * > ( m_pBar ) ) ->
		NcButtons_HitTest( point, &pBtnHitTest );
	if( pBtnHitTest == NULL )
		return -1;
	ASSERT_VALID( pBtnHitTest );
	if( m_pLastHitTestT == pBtnHitTest )
		return -1;
UINT nHitTest = pBtnHitTest->OnToolHitTest( point, pTI );
	if( nHitTest == HTCLIENT )
		return -1;
CExtDynAutoHideSlider * pDAHA = (CExtDynAutoHideSlider*)this;
	pDAHA->m_pLastHitTestT = pBtnHitTest;
	
	if( pTI->lpszText != NULL )
	{
		VERIFY(
			pDAHA->m_wndToolTip.AddTool(
				pDAHA,
				pTI->lpszText,
				&pTI->rect,
				1
				)
			);
	}

	return -1;
//	return CWnd::OnToolHitTest( point,  pTI );
}

BOOL CExtDynAutoHideSlider::PreTranslateMessage(MSG* pMsg) 
{
	if( m_pWndAnimateSrc != NULL )
	{
		ASSERT_VALID( m_pWndAnimateSrc );
		if( m_pWndAnimateSrc->PreTranslateMessage(pMsg) )
			return TRUE;
	}
	if( m_wndToolTip.GetSafeHwnd() != NULL )
		m_wndToolTip.RelayEvent( pMsg );
	return CWnd::PreTranslateMessage(pMsg);
}

void CExtDynAutoHideSlider::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	
	if( !m_bActiveState
		|| (GetStyle() & WS_VISIBLE) == 0
		)
		return;

HWND hWndChild = ::GetWindow( GetSafeHwnd(), GW_CHILD );
	if( hWndChild == NULL )
		return;
	ASSERT( ::IsWindow(hWndChild) );
	::SetFocus( hWndChild );
}


void CExtDynAutoHideSlider::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	if( !m_bActiveState || m_bAnimation )
		return;

CExtControlBar * pBar = m_pBar;
	ASSERT_VALID( pBar );
	ASSERT( !pBar->IsFixedMode() );
	ASSERT_VALID( pBar->m_pDockBar );
	ASSERT_KINDOF( CExtDockBar, pBar->m_pDockBar );
	if( pBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockDynBar)) )
	{
		pBar =
			STATIC_DOWNCAST(
				CExtControlBar,
				pBar->m_pDockBar->GetParent()
				);
		ASSERT_VALID( pBar );
		ASSERT( !pBar->IsFixedMode() );
	}
	
	ASSERT_VALID( m_pWndAnimateSrc );
UINT nID = m_pWndAnimateSrc->GetDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nID );

RECT rcNcSizes;
CalcNcOffsets( &rcNcSizes );
CSize sizeNcAdd( 0, 0 );
	sizeNcAdd.cx += rcNcSizes.left + rcNcSizes.right;
	sizeNcAdd.cy += rcNcSizes.top + rcNcSizes.bottom;
	
	if( nID == AFX_IDW_DOCKBAR_TOP || nID == AFX_IDW_DOCKBAR_BOTTOM )
		pBar->m_sizeDockedH.cy = cy + sizeNcAdd.cy;
	else
		pBar->m_sizeDockedV.cx = cx + sizeNcAdd.cx;
	
	m_sizeControlBar.cx = cx;
	m_sizeControlBar.cy = cy;
HWND hWndChild = ::GetWindow( GetSafeHwnd(), GW_CHILD );
	if( hWndChild == NULL )
		return;
	ASSERT( ::IsWindow(hWndChild) );
	::MoveWindow( hWndChild, 0, 0, cx, cy, TRUE );
}

#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

