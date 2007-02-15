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

#if (!defined __EXT_MFC_NO_TAB_CTRL)

	#if (!defined __AFXPRIV_H__)
		#include <AfxPriv.h>
	#endif 

	#if _MFC_VER < 0x700
		#include <../src/AfxImpl.h>
	#else
		#include <../src/mfc/AfxImpl.h>
	#endif

	#if (!defined __EXT_TABWND_H)
		#include <ExtTabWnd.h>
	#endif

	#if (!defined __EXT_PAINT_MANAGER_H)
		#include <ExtPaintManager.h>
	#endif

	#if (!defined __EXT_MEMORY_DC_H)
		#include <../Src/ExtMemoryDC.h>
	#endif

	#if (!defined __EXT_CONTROLBAR_H)
		#include <ExtControlBar.h>
	#endif

	#if (!defined __EXT_POPUP_MENU_WND_H)
		#include <ExtPopupMenuWnd.h>
	#endif

	#if (!defined __EXT_LOCALIZATION_H)
		#include <../Src/ExtLocalization.h>
	#endif

	#include <../profuisdll/resource.h>

	#if (!defined __EXT_CONTROLBAR_H)
		#include <ExtControlBar.h>
	#endif

#endif // (!defined __EXT_MFC_NO_TAB_CTRL )

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	#if (!defined __EXTCUSTOMIZE_H)
		#include <ExtCustomize.h>
	#endif
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#if (!defined __EXT_MFC_NO_TAB_CTRL)

/////////////////////////////////////////////////////////////////////////////
// CExtTabWnd

CExtTabWnd::TAB_ITEM_INFO::TAB_ITEM_INFO(
	CExtTabWnd * pWndTab,  // = NULL
	TAB_ITEM_INFO * pPrev, // = NULL
	TAB_ITEM_INFO * pNext, // = NULL
	__EXT_MFC_SAFE_LPCTSTR sText, // = NULL
	HICON hIcon, // = NULL
	bool bCopyIcon, // = true
	DWORD dwStyle, // = 0 // visible & non-group
	LPARAM lParam // = 0
	)
	: m_pWndTab( pWndTab )
	, m_pPrev( pPrev )
	, m_pNext( pNext )
	, m_sText( ( sText == NULL ) ? _T("") : sText )
	, m_icon( hIcon, bCopyIcon )
	, m_dwItemStyle( dwStyle )
	, m_sizeLastMeasuredItem( __EXTTAB_MIN_HORZ_WIDTH, __EXTTAB_MIN_HORZ_HEIGHT )
	, m_sizeLastMeasuredText( 0, 0 )
	, m_sizeLastMeasuredIcon( 0, 0 )
	, m_rcItem( 0, 0, __EXTTAB_MIN_HORZ_WIDTH, __EXTTAB_MIN_HORZ_HEIGHT )
	, m_lParam( lParam )
{
	if( m_pPrev != NULL )
		m_pPrev->m_pNext = this;
	if( m_pNext != NULL )
		m_pNext->m_pPrev = this;
}

CExtTabWnd::TAB_ITEM_INFO::TAB_ITEM_INFO(
	const TAB_ITEM_INFO & other
	)
	: m_pWndTab( NULL )
	, m_pPrev( NULL )
	, m_pNext( NULL )
	, m_sText( _T("") )
	, m_dwItemStyle( 0 )
	, m_sizeLastMeasuredItem( __EXTTAB_MIN_HORZ_WIDTH, __EXTTAB_MIN_HORZ_HEIGHT )
	, m_sizeLastMeasuredText( 0, 0 )
	, m_sizeLastMeasuredIcon( 0, 0 )
	, m_rcItem( 0, 0, __EXTTAB_MIN_HORZ_WIDTH, __EXTTAB_MIN_HORZ_HEIGHT )
	, m_lParam( 0 )
	, m_bHelperToolTipAvail( false )
{
	_AssignFromOther( other );
}

CExtTabWnd::TAB_ITEM_INFO::~TAB_ITEM_INFO()
{
}

void CExtTabWnd::TAB_ITEM_INFO::_AssignFromOther(
	const TAB_ITEM_INFO & other
	)
{
	ASSERT_VALID( (&other) );

	m_pWndTab = other.m_pWndTab;
	m_dwItemStyle = other.m_dwItemStyle;
	m_sText = other.m_sText;
	m_icon = other.m_icon;
	m_pPrev = other.m_pPrev;
	m_pNext = other.m_pNext;
	m_sizeLastMeasuredItem = other.m_sizeLastMeasuredItem;
	m_sizeLastMeasuredText = other.m_sizeLastMeasuredText;
	m_sizeLastMeasuredIcon = other.m_sizeLastMeasuredIcon;
	m_rcItem = other.m_rcItem;
	m_lParam = other.m_lParam;
	m_bHelperToolTipAvail = other.m_bHelperToolTipAvail;

	ASSERT_VALID( this );
}

__EXT_MFC_SAFE_LPCTSTR CExtTabWnd::TAB_ITEM_INFO::TextGet() const
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pWndTab );

__EXT_MFC_SAFE_LPCTSTR sExternItemText =
		m_pWndTab->OnTabWndQueryItemText( this );
	if( sExternItemText != NULL )
		return sExternItemText;

	return m_sText;
}

CSize CExtTabWnd::TAB_ITEM_INFO::IconGetSize() const
{
	ASSERT_VALID( this );

CExtCmdIcon * pExternIcon = m_pWndTab->OnTabWndQueryItemIcon( this );
	if( pExternIcon != NULL && (! pExternIcon->IsEmpty()) )
	{
		CSize _sizeIcon = pExternIcon->GetSize();
		return _sizeIcon;
	}

	return m_icon.GetSize();
}

CExtCmdIcon * CExtTabWnd::TAB_ITEM_INFO::IconGetPtr() const
{
	ASSERT_VALID( this );
	ASSERT_VALID( this );
	ASSERT_VALID( m_pWndTab );

CExtCmdIcon * pExternIcon = m_pWndTab->OnTabWndQueryItemIcon( this );
	if( pExternIcon != NULL && (! pExternIcon->IsEmpty()) )
		return pExternIcon;

	if( m_icon.IsEmpty() )
		return NULL;

	return (CExtCmdIcon *)&m_icon;
}

CSize CExtTabWnd::TAB_ITEM_INFO::Measure(
	CDC * pDcMeasure // = NULL
	)
{
	ASSERT_VALID( this );

CDC * pDC = pDcMeasure;
	if( pDcMeasure == NULL )
	{
		pDC = new CWindowDC( NULL );
		ASSERT( pDC != NULL );
	}
	ASSERT( pDC->GetSafeHdc() != NULL );

CExtTabWnd * pWndTab = GetTabWnd();
	ASSERT_VALID( pWndTab );

bool bSelected = SelectedGet();
//bool bSelected = pWndTab->SelectionBoldGet();

CFont & font =
		pWndTab->_GetTabWndFont(
			bSelected,
			__ETWS_ORIENT_TOP
			);
	ASSERT( font.GetSafeHandle() != NULL );

CExtSafeString sText = TextGet();
CRect rcMeasureText =
		CExtPaintManager::stat_CalcTextDimension(
			*pDC,
			font,
			sText
			);
	m_sizeLastMeasuredText = rcMeasureText.Size();
	m_sizeLastMeasuredIcon = IconGetSize();

CSize _sizeTab( 0, 0 );
bool bVertical = pWndTab->OrientationIsVertical();
	if( bVertical )
	{
		m_sizeLastMeasuredText.cx += 10;
		m_sizeLastMeasuredText.cy += 2;
		_sizeTab.cx =
			max(  m_sizeLastMeasuredIcon.cx, m_sizeLastMeasuredText.cy );
		_sizeTab.cy =
			m_sizeLastMeasuredIcon.cy
			+ ((m_sizeLastMeasuredIcon.cy > 0) ? __EXTTAB_MARGIN_ICON2TEXT_Y : 0)
			+ m_sizeLastMeasuredText.cx
			+ __EXTTAB_SEPARATOR_AREA_Y
			;

		_sizeTab.cx += __EXTTAB_MARGIN_BORDER_VX * 2;
		_sizeTab.cy += __EXTTAB_MARGIN_BORDER_VY * 2;

		_sizeTab.cx =
			max( _sizeTab.cx, __EXTTAB_MIN_VERT_WIDTH );
		_sizeTab.cy =
			max( _sizeTab.cy, __EXTTAB_MIN_VERT_HEIGHT );
	} // if( bVertical )
	else
	{
		_sizeTab.cx =
			m_sizeLastMeasuredIcon.cx
			+ ((m_sizeLastMeasuredIcon.cx > 0) ? __EXTTAB_MARGIN_ICON2TEXT_X : 0)
			+ m_sizeLastMeasuredText.cx
			+ __EXTTAB_SEPARATOR_AREA_X
			;
		_sizeTab.cy =
			max(  m_sizeLastMeasuredIcon.cy, m_sizeLastMeasuredText.cy );

		_sizeTab.cx += __EXTTAB_MARGIN_BORDER_HX * 2;
		_sizeTab.cy += __EXTTAB_MARGIN_BORDER_HY * 2;

		_sizeTab.cx =
			max( _sizeTab.cx, __EXTTAB_MIN_HORZ_WIDTH );
		_sizeTab.cy =
			max( _sizeTab.cy, __EXTTAB_MIN_HORZ_HEIGHT );
	} // else from if( bVertical )

	pWndTab->OnTabWndUpdateItemMeasure(
		this,
		*pDC,
		_sizeTab
		);

	if( pDcMeasure == NULL )
	{
		ASSERT( pDC != NULL );
		delete pDC;
	}

	m_sizeLastMeasuredItem = _sizeTab;
	return _sizeTab;
}

bool CExtTabWnd::TAB_ITEM_INFO::HitTest(
	const POINT & ptClient
	) const
{
	if( !VisibleGet() )
		return false;
const CRect & rcItem = ItemRectGet();
	if( rcItem.IsRectEmpty() )
		return false;
	if( !rcItem.PtInRect(ptClient) )
		return false;
	return true;
}

#ifdef _DEBUG

void CExtTabWnd::TAB_ITEM_INFO::AssertValid() const
{
	CObject::AssertValid();

	ASSERT( m_pWndTab != NULL );

	if( m_pPrev != NULL )
	{
		ASSERT( m_pPrev->m_pNext == this );
		ASSERT( m_pPrev->m_pWndTab == m_pWndTab );
	}
	
	if( m_pNext != NULL )
	{
		ASSERT( m_pNext->m_pPrev == this );
		ASSERT( m_pNext->m_pWndTab == m_pWndTab );
	}

}

void CExtTabWnd::TAB_ITEM_INFO::Dump(CDumpContext& dc) const
{
	CObject::Dump( dc );
}

#endif

IMPLEMENT_DYNCREATE(CExtTabWnd, CWnd)

CExtTabWnd::CExtTabWnd()
	: m_bDirectCreateCall( false )
	, m_bDelayRecalcLayout( true )
	, m_bReflectQueryRepositionCalcEffect( false )
	, m_bReflectParentSizing( true )
	, m_bTrackingButtonPushed( false )
	, m_bTrackingButtonHover( false )
	, m_dwTabWndStyle( 0 )
	, m_nSelIndex( -1 )
	, m_nDelayedSelIndex( -1 )
	, m_nIndexVisFirst( 0 )
	, m_nIndexVisLast( 0 )
	, m_nVisibleItemCount( 0 )
	, m_rcTabItemsArea( 0, 0, 0, 0 )
	, m_rcTabNearBorderArea( 0, 0, 0, 0 )
	, m_rcBtnUp( 0, 0, 0, 0 )
	, m_rcBtnDown( 0, 0, 0, 0 )
	, m_rcBtnClose( 0, 0, 0, 0 )
	, m_rcBtnHelp( 0, 0, 0, 0 )
	, m_rcBtnTabList( 0, 0, 0, 0 )
	, m_nPushedTrackingButton( (-1L) )
	, m_nPushedTrackingHitTest( __ETWH_NOWHERE )
	, m_nHoverTrackingHitTest( __ETWH_NOWHERE )
	, m_nItemsExtent( 0 )
	, m_nScrollPos( 0 )
	, m_nScrollMaxPos( 0 )
	, m_nScrollDirectionRest( 0 )
	, m_rcRecalcLayout( 0, 0, 0, 0 )
	, m_ptStartDrag( -1, -1 )
	, m_bEnableTrackToolTips( false )
	, m_bPushedUp( false )
	, m_bPushedDown( false )
	, m_bPushedHelp( false )
	, m_bPushedClose( false )
	, m_bPushedTabList( false )
	, m_pHelperLastPaintManagerRTC( NULL )
{
	VERIFY( RegisterTabWndClass() );
}

CExtTabWnd::~CExtTabWnd()
{
	_RemoveAllItemsImpl();
}

BEGIN_MESSAGE_MAP(CExtTabWnd, CWnd)
	//{{AFX_MSG_MAP(CExtTabWnd)
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_SETFOCUS()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_CAPTURECHANGED()
	ON_WM_CANCELMODE()
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
	ON_WM_SYSCOLORCHANGE()
	__EXT_MFC_SAFE_ON_WM_SETTINGCHANGE()
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_MESSAGE(__ExtMfc_WM_THEMECHANGED, OnThemeChanged)
	ON_REGISTERED_MESSAGE(
		CExtControlBar::g_nMsgQueryRepositionCalcEffect,
		_OnQueryRepositionCalcEffect
		)
END_MESSAGE_MAP()


#ifdef _DEBUG

void CExtTabWnd::AssertValid() const
{
	CWnd::AssertValid();
INT nItemCount = m_arrItems.GetSize();
	ASSERT(
			m_nVisibleItemCount >= 0
		&&	m_nVisibleItemCount <= nItemCount
		);
}

void CExtTabWnd::Dump(CDumpContext& dc) const
{
	CWnd::Dump( dc );
}

#endif

CFont & CExtTabWnd::_GetTabWndFont(
	bool bSelected,
	DWORD dwOrientation // = DWORD(-1) // default orientation
	) const
{
	ASSERT_VALID( this );
	if( dwOrientation == DWORD(-1) )
		dwOrientation = OrientationGet();
bool bBold = bSelected && SelectionBoldGet();
	if( dwOrientation == __ETWS_ORIENT_TOP
		|| dwOrientation == __ETWS_ORIENT_BOTTOM
		)
		return
			bBold
				? g_PaintManager->m_FontBold
				: g_PaintManager->m_FontNormal
				;
	if( GetTabWndStyle() & __ETWS_INVERT_VERT_FONT )
		return
			bBold
				? g_PaintManager->m_FontBoldVertX
				: g_PaintManager->m_FontNormalVertX
				;
	return
			bBold
				? g_PaintManager->m_FontBoldVert
				: g_PaintManager->m_FontNormalVert
				;
}

void CExtTabWnd::_RemoveAllItemsImpl()
{
	ASSERT_VALID( this );

LONG nCount = m_arrItems.GetSize();
	
	if( nCount > 0 )
	{
		OnTabWndRemoveAllItems( true );

		for( LONG nItem = 0; nItem < nCount; nItem ++ )
		{
			TAB_ITEM_INFO * pTII = m_arrItems[ nItem ];
			delete pTII;
		}
		m_arrItems.RemoveAll();
		m_arrItems.FreeExtra();

		m_nVisibleItemCount = 0;
		m_nSelIndex = -1;
		m_bDelayRecalcLayout = true;

		ASSERT_VALID( this );

		OnTabWndRemoveAllItems( false );
	} // if( nCount > 0 )
}

CExtTabWnd::TAB_ITEM_INFO * CExtTabWnd::ItemInsert(
	__EXT_MFC_SAFE_LPCTSTR sText, // = NULL
	HICON hIcon, // = NULL
	bool bCopyIcon, // = true
	DWORD dwItemStyle, // = 0 // visible & non-group
	LONG nIndex, // = -1 // default - append
	LPARAM lParam, // = 0
	bool bUpdateTabWnd // = false
	)
{
	ASSERT_VALID( this );
LONG nCount = ItemGetCount();
	if( nIndex < 0 || nIndex > nCount )
		nIndex = nCount;

TAB_ITEM_INFO * pTiiPrev = NULL;
	if( nIndex > 0 )
	{
		pTiiPrev = ItemGet( nIndex - 1 );
		ASSERT_VALID( pTiiPrev );
	}

TAB_ITEM_INFO * pTiiNext = NULL;
	if( nIndex < nCount )
	{
		pTiiNext = ItemGet( nIndex );
		ASSERT_VALID( pTiiNext );
	}

TAB_ITEM_INFO * pTii =
		new TAB_ITEM_INFO(
			this,
			pTiiPrev,
			pTiiNext,
			sText,
			hIcon,
			bCopyIcon,
			dwItemStyle,
			lParam
			);
	m_arrItems.InsertAt( nIndex, pTii );
	ASSERT_VALID( pTii );
	if( m_nSelIndex >= nIndex ) 
		m_nSelIndex++;

	if( pTii->VisibleGet() )
		m_nVisibleItemCount ++;

	ASSERT_VALID( this );

	OnTabWndItemInsert(
		nIndex,
		pTii
		);

	UpdateTabWnd( bUpdateTabWnd );
	return pTii;
}

LONG CExtTabWnd::ItemRemove( // returns count of removed items
	LONG nIndex,
	LONG nCountToRemove, // = 1
	bool bUpdateTabWnd // = false
	)
{
	ASSERT_VALID( this );

	if( nCountToRemove > 0 )
	{
		LONG nCount = ItemGetCount();
		if( nCount > 0 )
		{
			ASSERT( nIndex >= 0 && nIndex < nCount );
			LONG nAvailToRemove = nCount - nIndex;
			if( nCountToRemove > nAvailToRemove )
				nCountToRemove = nAvailToRemove;

			TAB_ITEM_INFO * pTiiPrev = NULL;
			TAB_ITEM_INFO * pTiiNext = NULL;

			if( m_nSelIndex >= (nIndex+nCountToRemove) )
				m_nSelIndex -= nCountToRemove;

			for( LONG n = 0; n < nCountToRemove; n++ )
			{
				TAB_ITEM_INFO * pTii = ItemGet( nIndex );
				if( n == 0 )
					pTiiPrev = pTii->GetPrev();
				if( n == ( nCountToRemove - 1  ) )
					pTiiNext = pTii->GetNext();
				if( pTii->VisibleGet() )
					m_nVisibleItemCount --;
				if( pTii->SelectedGet() )
					m_nSelIndex = -1;
				delete pTii;
			}

			
			OnTabWndRemoveItem( nIndex, nCountToRemove, true );
			
			m_arrItems.RemoveAt( nIndex, nCountToRemove );

			if( pTiiPrev != NULL )
				pTiiPrev->m_pNext = pTiiNext;
			if( pTiiNext != NULL )
				pTiiNext->m_pPrev = pTiiPrev;
			
			OnTabWndRemoveItem( nIndex, nCountToRemove, false );
			
		} // if( nCount > 0 )
		else
			nCountToRemove = 0;
	}

	ASSERT_VALID( this );

	UpdateTabWnd( bUpdateTabWnd );
	return nCountToRemove;
}

LONG CExtTabWnd::ItemHitTest(
	const POINT & ptClient
	) const
{
	ASSERT_VALID( this );
	if(	GetSafeHwnd() == NULL
		||	( ! ::IsWindow( GetSafeHwnd() ) )
		)
		return __ETWH_NOWHERE;
CRect rcClient;
	GetClientRect( &rcClient );
	if( rcClient.IsRectEmpty()
		|| (! rcClient.PtInRect(ptClient) )
		)
		return __ETWH_NOWHERE;
	
	if( !m_rcTabNearBorderArea.IsRectEmpty()
		&& m_rcTabNearBorderArea.PtInRect(ptClient)
		)
		return __ETWH_BORDER_AREA;

	if( !m_rcBtnUp.IsRectEmpty()
		&& m_rcBtnUp.PtInRect(ptClient)
		)
		return __ETWH_BUTTON_LEFTUP;
	if( !m_rcBtnDown.IsRectEmpty()
		&& m_rcBtnDown.PtInRect(ptClient)
		)
		return __ETWH_BUTTON_RIGHTDOWN;
	if( !m_rcBtnHelp.IsRectEmpty()
		&& m_rcBtnHelp.PtInRect(ptClient)
		)
		return __ETWH_BUTTON_HELP;
	if( !m_rcBtnClose.IsRectEmpty()
		&& m_rcBtnClose.PtInRect(ptClient)
		)
		return __ETWH_BUTTON_CLOSE;
	if( !m_rcBtnTabList.IsRectEmpty()
		&& m_rcBtnTabList.PtInRect(ptClient)
		)
		return __ETWH_BUTTON_TAB_LIST;
	if( m_rcTabItemsArea.IsRectEmpty()
		|| !m_rcTabItemsArea.PtInRect(ptClient)
		)
		return __ETWH_NOWHERE;

	if( ItemGetVisibleCount() == 0 )
		return __ETWH_ITEMS_AREA;

bool bHorz = OrientationIsHorizontal();

//CPoint ptClientTest(
//		ptClient.x + ( bHorz ? m_nScrollPos : 0 ),
//		ptClient.y + ( bHorz ? 0 : m_nScrollPos )
//		);
CPoint ptClientTest(
		ptClient.x +
			( bHorz
				? (m_nScrollPos - m_rcTabItemsArea.left)
				: 0
			),
		ptClient.y +
			( bHorz
				? 0
				: (m_nScrollPos - m_rcTabItemsArea.top)
			)
		);

LONG nItemCount = ItemGetCount();
LONG nVisCount = ItemGetVisibleCount();
	if(		nItemCount > 0
		&&	nVisCount > 0
		&&	m_nIndexVisFirst >= 0
		&&	m_nIndexVisLast >= 0
		&&	m_nIndexVisFirst < nItemCount
		&&	m_nIndexVisLast < nItemCount
		)
	{
		ASSERT( m_nIndexVisFirst <= m_nIndexVisLast );
		ASSERT( 0 <= m_nIndexVisFirst && m_nIndexVisFirst < nItemCount );
		ASSERT( 0 <= m_nIndexVisLast && m_nIndexVisLast < nItemCount );
		for( LONG nIndex = m_nIndexVisFirst; nIndex <= m_nIndexVisLast; nIndex++ )
		{
			const TAB_ITEM_INFO * pTii = ItemGet( nIndex );
			ASSERT_VALID( pTii );
			if( !pTii->VisibleGet() )
				continue;
			if( pTii->HitTest( ptClientTest ) )
				return nIndex;
		} // for( LONG nIndex = m_nIndexVisFirst; nIndex <= m_nIndexVisLast; nIndex++ )
	} // if( nItemCount > 0 && nVisCount > 0 )

	return __ETWH_ITEMS_AREA;
}

bool CExtTabWnd::ItemEnsureVisible(
	INT nItemIndex,
	bool bUpdateTabWnd // = false
	)
{
	ASSERT_VALID( this );
	ASSERT( nItemIndex >= 0 && nItemIndex < ItemGetCount() );
bool bRetVal = false;
	if( _IsScrollAvail() )
	{
		TAB_ITEM_INFO * pTii = ItemGet( nItemIndex );
		ASSERT_VALID( pTii );
		if( pTii->VisibleGet() )
		{
			bool bHorz = OrientationIsHorizontal();
			
			CRect rcItem = pTii->ItemRectGet();
			rcItem.OffsetRect(
				bHorz ? (-m_nScrollPos) : 0,
				bHorz ? 0 : (-m_nScrollPos)
				);
			
			INT nItemExtentMinVal =
				bHorz ? rcItem.left : rcItem.top;
			
			INT nItemExtentMaxVal =
				bHorz ? rcItem.right : rcItem.bottom;
			
			INT nAreaExtentMinVal =
				0;
				// bHorz ? m_rcTabItemsArea.left : m_rcTabItemsArea.top;
			INT nAreaExtentMaxVal =
				bHorz
					? (m_rcTabItemsArea.right - m_rcTabItemsArea.left)
					: (m_rcTabItemsArea.bottom - m_rcTabItemsArea.top)
					;
				//bHorz ? m_rcTabItemsArea.right : m_rcTabItemsArea.bottom;
			nAreaExtentMaxVal += __EXTTAB_ADD_END_SCROLL_SPACE + 2;

			if( nItemExtentMaxVal > nAreaExtentMaxVal  )
			{
				INT nShift = nItemExtentMaxVal - nAreaExtentMaxVal;
				ASSERT( nShift > 0 );
				m_nScrollPos += nShift;
				
				INT nScrollRest = m_nScrollMaxPos - m_nScrollPos;
				nScrollRest = min( nScrollRest, __EXTTAB_ADD_END_SCROLL_SPACE );
				m_nScrollPos += nScrollRest;
				
				ASSERT( m_nScrollPos >= 0 && m_nScrollPos <= m_nScrollMaxPos );
			}
			if( nItemExtentMinVal < nAreaExtentMinVal  )
			{
				INT nShift = nAreaExtentMinVal - nItemExtentMinVal;
				ASSERT( nShift > 0 );
				m_nScrollPos -= nShift;

				INT nScrollRest = min( m_nScrollPos, __EXTTAB_ADD_END_SCROLL_SPACE );
				m_nScrollPos -= nScrollRest;
				
				ASSERT( m_nScrollPos >= 0 && m_nScrollPos <= m_nScrollMaxPos );
			}
			
			if( m_nScrollPos > m_nScrollMaxPos )
				m_nScrollPos = m_nScrollMaxPos;
			
			bRetVal = true;
		} // if( pTii->VisibleGet() )
	} // if( _IsScrollAvail() )
	else
		bRetVal = true;
	UpdateTabWnd( bUpdateTabWnd );
	return bRetVal;
}

bool CExtTabWnd::g_bTabWndClassRegistered = false;

bool CExtTabWnd::RegisterTabWndClass()
{
	if( g_bTabWndClassRegistered )
		return true;

WNDCLASS wndclass;
HINSTANCE hInst = AfxGetInstanceHandle();
	if( ! ::GetClassInfo(
			hInst,
			__EXT_TAB_WND_CLASS_NAME,
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
			( g_hCursor != NULL )
				? g_hCursor
				: ::LoadCursor(
					NULL, //hInst,
					IDC_ARROW
					)
				;
		ASSERT( wndclass.hCursor != NULL );
		wndclass.hbrBackground = NULL; 
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = __EXT_TAB_WND_CLASS_NAME;
		if( !::AfxRegisterClass( &wndclass ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}

	g_bTabWndClassRegistered = true;
	return true;
}

BOOL CExtTabWnd::Create(
	CWnd * pParentWnd,
	const RECT & rcWnd, // = CRect( 0, 0, 0, 0 )
	UINT nDlgCtrlID, // = UINT( IDC_STATIC )
	DWORD dwWindowStyle, // = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS
	DWORD dwTabStyle, // = __ETWS_ORIENT_TOP|__ETWS_ENABLED_BTN_HELP|__ETWS_ENABLED_BTN_CLOSE
	CCreateContext * pContext // = NULL
	)
{
	if( !RegisterTabWndClass() )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	m_bDirectCreateCall = true;
	m_dwTabWndStyle = dwTabStyle;
	if( ! CWnd::Create(
			__EXT_TAB_WND_CLASS_NAME,
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

bool CExtTabWnd::_CreateHelper()
{
	EnableToolTips( TRUE );

	if( OnTabWndToolTipQueryEnabled() )
	{ // tooltips enabled at all?
		if( ! m_wndToolTip.Create( this ) )
		{
			ASSERT( FALSE );
			return false;
		}
		m_wndToolTip.Activate( TRUE );
	}

	UpdateTabWnd( false );
	return true;
}

void CExtTabWnd::PreSubclassWindow() 
{
	CWnd::PreSubclassWindow();

	if( m_bDirectCreateCall )
		return;

DWORD dwStyle = ::GetWindowLong( m_hWnd, GWL_STYLE );
	m_dwTabWndStyle = dwStyle & __EXTMFC_ALL_FORM_MOVABLE_WND_STYLES;
	::SetWindowLong( m_hWnd, GWL_STYLE, dwStyle & (~__EXTMFC_ALL_FORM_MOVABLE_WND_STYLES) );
	
	if( !_CreateHelper() )
	{
		ASSERT( FALSE );
		AfxThrowMemoryException();
	} // if( !Create() )
}

LRESULT CExtTabWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
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
	return CWnd::WindowProc(message, wParam, lParam);
}

BOOL CExtTabWnd::PreTranslateMessage(MSG* pMsg) 
{
	if(		OnTabWndToolTipQueryEnabled()
		&&	m_wndToolTip.GetSafeHwnd() != NULL
		)
		m_wndToolTip.RelayEvent( pMsg );

	return CWnd::PreTranslateMessage(pMsg);
}

BOOL CExtTabWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	if( ( !RegisterTabWndClass() )
		|| ( !CWnd::PreCreateWindow(cs) )
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
	
	cs.lpszClass = __EXT_TAB_WND_CLASS_NAME;

	return TRUE;
}

bool CExtTabWnd::_ProcessMouseClick(
	CPoint point,
	bool bButtonPressed,
	INT nMouseButton, // MK_... values
	UINT nMouseEventFlags
	)
{
LONG nHitTest = ItemHitTest( point );
	switch( nHitTest )
	{
	case __ETWH_BUTTON_LEFTUP:
	case __ETWH_BUTTON_RIGHTDOWN:
	case __ETWH_BUTTON_HELP:
	case __ETWH_BUTTON_CLOSE:
	case __ETWH_BUTTON_TAB_LIST:
		return
			OnTabWndClickedButton(
				nHitTest,
				bButtonPressed,
				nMouseButton,
				nMouseEventFlags
				);
	default:
		if( nHitTest < __ETWH_TAB_FIRST )
		{
			OnTabWndMouseTrackingPushedStop();
			return false; //true;
		}
	break;
	}
	ASSERT( nHitTest >= 0 && nHitTest < ItemGetCount() );
	if( bButtonPressed )
		m_ptStartDrag = point;
	return
		OnTabWndClickedItem(
			nHitTest,
			bButtonPressed,
			nMouseButton,
			nMouseEventFlags
			);
}

void CExtTabWnd::UpdateTabWnd(
	bool bImmediate // = true
	)
{
	ASSERT_VALID( this );
	
	m_bDelayRecalcLayout = true;

	if(		GetSafeHwnd() == NULL
		||	( ! ::IsWindow(GetSafeHwnd()) )
		)
		return;

	if( bImmediate
		&& ( (GetStyle() & WS_VISIBLE) != 0 )
		)
	{
		_RecalcLayoutImpl();
		Invalidate();
		UpdateWindow();
	}
}

void CExtTabWnd::OnTabWndSyncVisibility()
{
}

void CExtTabWnd::_RecalcLayoutImpl()
{
	ASSERT_VALID( this );

	if(		GetSafeHwnd() == NULL
		||	( ! ::IsWindow(GetSafeHwnd()) )
		)
		return;

	if( ! m_bDelayRecalcLayout )
		return;
	m_bDelayRecalcLayout = false;

	m_rcTabItemsArea.SetRectEmpty();
	m_rcTabNearBorderArea.SetRectEmpty();
	m_rcBtnUp.SetRectEmpty();
	m_rcBtnDown.SetRectEmpty();
	m_rcBtnHelp.SetRectEmpty();
	m_rcBtnClose.SetRectEmpty();
	m_rcBtnTabList.SetRectEmpty();
	m_nIndexVisFirst = m_nIndexVisLast = -1;

CRect rcClient;
	_RecalcLayout_GetClientRect( rcClient );
	if( rcClient.IsRectEmpty()
		|| rcClient.right <= rcClient.left
		|| rcClient.bottom <=  rcClient.top
		)
		return;

	m_rcTabItemsArea = rcClient;
DWORD dwOrientation = OrientationGet();
bool bHorz = OrientationIsHorizontal();
CWindowDC dcMeasure( this );
LONG nTabAreaMetric = 0;
LONG nItemCount = ItemGetCount();
DWORD dwTabWndStyle = GetTabWndStyle();
bool bEqualWidth = (dwTabWndStyle&__ETWS_EQUAL_WIDTHS) ? true : false;
bool bGrouped = (dwTabWndStyle&__ETWS_GROUPED) ? true : false;
bool bGroupedExpandItems = (dwTabWndStyle&__ETWS_GROUPED_EXPAND_ITEMS) ? true : false;
bool bShowHelp = (dwTabWndStyle&__ETWS_SHOW_BTN_HELP) ? true : false;
bool bShowClose = (dwTabWndStyle&__ETWS_SHOW_BTN_CLOSE) ? true : false;
bool bShowTabList = (dwTabWndStyle&__ETWS_SHOW_BTN_TAB_LIST) ? true : false;
	if( bGrouped )
		m_nItemsExtent = m_nScrollMaxPos = 10;
	else
		m_nItemsExtent = m_nScrollMaxPos = 0;

	if( bGrouped )
		bEqualWidth = false;

INT nVisibleNo = 0;
	for( LONG nIndex = 0; nIndex < nItemCount; )
	{ // calc items extent
		TAB_ITEM_INFO * pTii = ItemGet( nIndex );
		ASSERT_VALID( pTii );
		if( !pTii->VisibleGet() )
		{
			nIndex++;
			continue;
		}
		
		if( bGrouped )
		{
			if( ! bGroupedExpandItems )
			{
				INT nMaxInGroupExtent = 0;
				INT nNormalGroupItemExtent = 0;
				INT nGroupStartIndex = nIndex;
				for( INT nCountInGroup = 0; true ; )
				{
					CSize _size = pTii->Measure( &dcMeasure );
					CSize _sizeText = pTii->GetLastMeasuredTextSize();
					CSize _sizeIcon = pTii->GetLastMeasuredIconSize();
					bool bGroupStart =
						(nVisibleNo == 0)
						||
						pTii->GroupStartGet();

					switch( dwOrientation )
					{
					case __ETWS_ORIENT_TOP:
					case __ETWS_ORIENT_BOTTOM:
					{
						nMaxInGroupExtent =
							max( nMaxInGroupExtent, _size.cx );
						INT nGroupItemExtent =
							_size.cx
							- _sizeText.cx
							- ((_sizeIcon.cx > 0) ? __EXTTAB_MARGIN_ICON2TEXT_X : 0)
							;
						nNormalGroupItemExtent =
							max( nNormalGroupItemExtent, nGroupItemExtent );
						//
						if( nVisibleNo != 0
							&& bGroupStart
							)
							m_nItemsExtent += __EXTTAB_BETWEEN_GROUP_GAP_DX;
						nTabAreaMetric =
							max( nTabAreaMetric, _size.cy );
					}
					break;
					case __ETWS_ORIENT_LEFT:
					case __ETWS_ORIENT_RIGHT:
					{
						nMaxInGroupExtent =
							max( nMaxInGroupExtent, _size.cy );
						INT nGroupItemExtent =
							_size.cy
							- _sizeText.cx
							- ((_sizeIcon.cy > 0) ? __EXTTAB_MARGIN_ICON2TEXT_Y : 0)
							;
						nNormalGroupItemExtent =
							max( nNormalGroupItemExtent, nGroupItemExtent );
						//
						if( nVisibleNo != 0
							&& bGroupStart
							)
							m_nItemsExtent += __EXTTAB_BETWEEN_GROUP_GAP_DY;
						nTabAreaMetric =
							max( nTabAreaMetric, _size.cx );
					}
					break;
				#ifdef _DEBUG
					default:
						ASSERT( FALSE );
					break;
				#endif // _DEBUG
					} // switch( dwOrientation )

					nIndex++;
					nVisibleNo++;
					nCountInGroup++;
					ASSERT( nIndex <= nItemCount );
					if( nIndex == nItemCount )
						break;
					pTii = ItemGet( nIndex );
					ASSERT_VALID( pTii );
					if( pTii->GroupStartGet() )
						break;
				} // for( INT nCountInGroup = 0; true ; )

				ASSERT( nCountInGroup >= 1 );
				INT nGroupExtent =
					nMaxInGroupExtent
					+ nNormalGroupItemExtent * (nCountInGroup-1);
				m_nItemsExtent += nGroupExtent;

				for( INT n = 0; n < nCountInGroup; n++ )
				{ // reset in group minimal sizes
					pTii = ItemGet( nGroupStartIndex + n );
					ASSERT_VALID( pTii );
					bool bInGroupActive = pTii->InGroupActiveGet();

					switch( dwOrientation )
					{
					case __ETWS_ORIENT_TOP:
					case __ETWS_ORIENT_BOTTOM:
						if( bInGroupActive )
							pTii->m_sizeLastMeasuredItem.cx = nMaxInGroupExtent;
						else
							pTii->m_sizeLastMeasuredItem.cx = nNormalGroupItemExtent;
					break;
					case __ETWS_ORIENT_LEFT:
					case __ETWS_ORIENT_RIGHT:
						if( bInGroupActive )
							pTii->m_sizeLastMeasuredItem.cy = nMaxInGroupExtent;
						else
							pTii->m_sizeLastMeasuredItem.cy = nNormalGroupItemExtent;
					break;
#ifdef _DEBUG
					default:
						ASSERT( FALSE );
					break;
#endif // _DEBUG
					} // switch( dwOrientation )
				} // reset in group minimal sizes

			} // if( ! bGroupedExpandItems )
			else
			{
				CSize _size = pTii->Measure( &dcMeasure );
				bool bGroupStart =
					(nVisibleNo == 0)
					||
					pTii->GroupStartGet();
				switch( dwOrientation )
				{
				case __ETWS_ORIENT_TOP:
				case __ETWS_ORIENT_BOTTOM:
					m_nItemsExtent += _size.cx;
					nTabAreaMetric =
						max( nTabAreaMetric, _size.cy );
					if( nVisibleNo != 0
						&& bGroupStart
						)
						m_nItemsExtent += __EXTTAB_BETWEEN_GROUP_GAP_DX;
				break;
				case __ETWS_ORIENT_LEFT:
				case __ETWS_ORIENT_RIGHT:
					m_nItemsExtent += _size.cy;
					nTabAreaMetric =
						max( nTabAreaMetric, _size.cx );
					if( nVisibleNo != 0
						&& bGroupStart
						)
						m_nItemsExtent += __EXTTAB_BETWEEN_GROUP_GAP_DY;
				break;
#ifdef _DEBUG
				default:
					ASSERT( FALSE );
				break;
#endif // _DEBUG
				} // switch( dwOrientation )

				nIndex++;
				nVisibleNo++;
			} // else from if( ! bGroupedExpandItems )
		} // if( bGrouped )
		else
		{
			CSize _size = pTii->Measure( &dcMeasure );
			switch( dwOrientation )
			{
			case __ETWS_ORIENT_TOP:
			case __ETWS_ORIENT_BOTTOM:
				m_nItemsExtent += _size.cx;
				nTabAreaMetric =
					max( nTabAreaMetric, _size.cy );
			break;
			case __ETWS_ORIENT_LEFT:
			case __ETWS_ORIENT_RIGHT:
				m_nItemsExtent += _size.cy;
				nTabAreaMetric =
					max( nTabAreaMetric, _size.cx );
			break;
#ifdef _DEBUG
			default:
				ASSERT( FALSE );
			break;
#endif // _DEBUG
			} // switch( dwOrientation )

			nIndex++;
			nVisibleNo++;
		} // else from if( bGrouped )

	} // calc items extent
	ASSERT( nVisibleNo == m_nVisibleItemCount );

LONG nSpaceBefore = 0, nSpaceAfter = 0, nSpaceOver = 0;
	OnTabWndMeasureItemAreaMargins(
		dcMeasure,
		nSpaceBefore,
		nSpaceAfter,
		nSpaceOver
		);
LONG nAddShiftForBtns = nSpaceAfter;
	// precalc button size
CSize _sizeButton =
		OnTabWndCalcButtonSize(
			dcMeasure,
			nTabAreaMetric
			);
	ASSERT( _sizeButton.cx > 0 && _sizeButton.cy > 0 );
INT nButtonExtent = bHorz ? _sizeButton.cx : _sizeButton.cy;
INT nBetweenButtonExtent = bHorz ? __EXTTAB_BETWEEN_BTN_GAP_DX : __EXTTAB_BETWEEN_BTN_GAP_DY;
bool bFirstShiftPassed = false;
	if( bShowClose )
	{
		if( !bFirstShiftPassed )
		{
			nSpaceAfter += nBetweenButtonExtent;
			bFirstShiftPassed = true;
		}
		nSpaceAfter += nButtonExtent + nBetweenButtonExtent;
	} // if( bShowClose )
	if( bShowHelp )
	{
		if( !bFirstShiftPassed )
		{
			nSpaceAfter += nBetweenButtonExtent;
			bFirstShiftPassed = true;
		}
		nSpaceAfter += nButtonExtent + nBetweenButtonExtent;
	} // if( bShowHelp )

	if( bShowTabList )
	{
		if( !bFirstShiftPassed )
		{
			nSpaceAfter += nBetweenButtonExtent;
			bFirstShiftPassed = true;
		}
		nSpaceAfter += nButtonExtent + nBetweenButtonExtent;
	} // if( bShowTabList )

bool bEnableScrollButtons = !bEqualWidth;

	if( bEnableScrollButtons )
	{
		if( !bFirstShiftPassed )
		{
			nSpaceAfter += nBetweenButtonExtent;
			bFirstShiftPassed = true;
		}
		nSpaceAfter += (nButtonExtent + nBetweenButtonExtent)*2;
	} // if( !bEqualWidth )

//LONG nItemRectStartPos = nSpaceBefore;
LONG nItemRectStartPos = 0;
LONG nItemRectOffs = 0;

	// calc tab wnd areas
	switch( dwOrientation )
	{
	case __ETWS_ORIENT_TOP:
		nTabAreaMetric =
			max( nTabAreaMetric, __EXTTAB_MIN_HORZ_HEIGHT );
		m_rcTabItemsArea.top += nSpaceOver;
		m_rcTabItemsArea.bottom =
			m_rcTabItemsArea.top + nTabAreaMetric;
		m_rcTabItemsArea.bottom =
			min( m_rcTabItemsArea.bottom, rcClient.bottom );
		if( m_rcTabItemsArea.bottom < rcClient.bottom )
		{
			m_rcTabNearBorderArea = rcClient;
			m_rcTabNearBorderArea.top = m_rcTabItemsArea.bottom;
		}
m_rcTabItemsArea.left += nSpaceBefore;
		m_rcTabItemsArea.right -= nSpaceAfter;
//		nItemRectStartPos += m_rcTabItemsArea.left;
		nItemRectOffs += m_rcTabItemsArea.top;

		m_nScrollMaxPos = m_nItemsExtent - m_rcTabItemsArea.Width();
		if( m_nScrollMaxPos < 0 )
			m_nScrollMaxPos = 0;
	break;
	case __ETWS_ORIENT_BOTTOM:
		nTabAreaMetric =
			max( nTabAreaMetric, __EXTTAB_MIN_HORZ_HEIGHT );
		m_rcTabItemsArea.bottom -= nSpaceOver;
		m_rcTabItemsArea.top =
			m_rcTabItemsArea.bottom - nTabAreaMetric;
		m_rcTabItemsArea.top =
			max( m_rcTabItemsArea.top, rcClient.top );
		if( m_rcTabItemsArea.top > rcClient.top )
		{
			m_rcTabNearBorderArea = rcClient;
			m_rcTabNearBorderArea.bottom = m_rcTabItemsArea.top;
		}
m_rcTabItemsArea.left += nSpaceBefore;
		m_rcTabItemsArea.right -= nSpaceAfter;
//		nItemRectStartPos += m_rcTabItemsArea.left;
		nItemRectOffs += m_rcTabItemsArea.top;

		m_nScrollMaxPos = m_nItemsExtent - m_rcTabItemsArea.Width();
		if( m_nScrollMaxPos < 0 )
			m_nScrollMaxPos = 0;
	break;
	case __ETWS_ORIENT_LEFT:
		nTabAreaMetric =
			max( nTabAreaMetric, __EXTTAB_MIN_VERT_WIDTH );
		m_rcTabItemsArea.left += nSpaceOver;
		m_rcTabItemsArea.right =
			m_rcTabItemsArea.left + nTabAreaMetric;
		m_rcTabItemsArea.right =
			min( m_rcTabItemsArea.right, rcClient.right );
		if( m_rcTabItemsArea.right < rcClient.right )
		{
			m_rcTabNearBorderArea = rcClient;
			m_rcTabNearBorderArea.left = m_rcTabItemsArea.right;
		}
m_rcTabItemsArea.top += nSpaceBefore;
		m_rcTabItemsArea.bottom -= nSpaceAfter;
//		nItemRectStartPos += m_rcTabItemsArea.top;
		nItemRectOffs += m_rcTabItemsArea.left;

		m_nScrollMaxPos = m_nItemsExtent - m_rcTabItemsArea.Height();
		if( m_nScrollMaxPos < 0 )
			m_nScrollMaxPos = 0;
	break;
	case __ETWS_ORIENT_RIGHT:
		nTabAreaMetric =
			max( nTabAreaMetric, __EXTTAB_MIN_VERT_WIDTH );
		m_rcTabItemsArea.right -= nSpaceOver;
		m_rcTabItemsArea.left =
			m_rcTabItemsArea.right - nTabAreaMetric;
		m_rcTabItemsArea.left =
			max( m_rcTabItemsArea.left, rcClient.left );
		if( m_rcTabItemsArea.left > rcClient.left )
		{
			m_rcTabNearBorderArea = rcClient;
			m_rcTabNearBorderArea.right = m_rcTabItemsArea.left;
		}
m_rcTabItemsArea.top += nSpaceBefore;
		m_rcTabItemsArea.bottom -= nSpaceAfter;
//		nItemRectStartPos += m_rcTabItemsArea.top;
		nItemRectOffs += m_rcTabItemsArea.left;

		m_nScrollMaxPos = m_nItemsExtent - m_rcTabItemsArea.Height();
		if( m_nScrollMaxPos < 0 )
			m_nScrollMaxPos = 0;
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( dwOrientation )

	ASSERT( m_nScrollPos >= 0 );
	ASSERT( m_nScrollMaxPos >= 0 );
	if( m_nScrollPos > m_nScrollMaxPos )
		m_nScrollPos = m_nScrollMaxPos;

LONG nEqualItemExtent = bHorz
		? __EXTTAB_MIN_HORZ_WIDTH
		: __EXTTAB_MIN_VERT_HEIGHT;
bool bCancelEqualWidth = false;
	if( bEqualWidth && m_nVisibleItemCount > 0 )
	{
		if( !_IsScrollAvail() )
		{
			if( (m_dwTabWndStyle & __ETWS_FULL_WIDTH) == 0 )
				bCancelEqualWidth = true;
		}
		if( !bCancelEqualWidth )
		{
			INT nTestExtent = bHorz
				? m_rcTabItemsArea.Width()
				: m_rcTabItemsArea.Height();
			nTestExtent /= m_nVisibleItemCount;
			nEqualItemExtent = max( nTestExtent, nEqualItemExtent );
		}
	} // if( bEqualWidth && m_nVisibleItemCount > 0 )

	if( !_IsScrollAvail()
		//&& bEnableScrollButtons
		&& (m_dwTabWndStyle & __ETWS_AUTOHIDE_SCROLL)
		)
	{
		bEnableScrollButtons = false;
		m_rcBtnUp.SetRectEmpty();
		m_rcBtnDown.SetRectEmpty();
	}

CRect rcButton( m_rcTabItemsArea );
	if( bHorz )
		rcButton.right = rcClient.right - nAddShiftForBtns;
	else
		rcButton.bottom = rcClient.bottom - nAddShiftForBtns;
	rcButton.DeflateRect(
		bHorz ? 0 : (rcButton.Width() - _sizeButton.cx) / 2,
		bHorz ? (rcButton.Height() - _sizeButton.cy) / 2 : 0
		);
	rcButton.OffsetRect(
		bHorz ? -nBetweenButtonExtent : 0,
		bHorz ? 0 : -nBetweenButtonExtent
		);
	if( bHorz )
	{
		rcButton.left = rcButton.right - _sizeButton.cx;
		rcButton.bottom = rcButton.top + _sizeButton.cy;
	}
	else
	{
		rcButton.right = rcButton.left + _sizeButton.cx;
		rcButton.top = rcButton.bottom - _sizeButton.cy;
	}
INT nButtonShift = - nBetweenButtonExtent - nButtonExtent;
	if( bShowClose )
	{
		m_rcBtnClose = rcButton;
		rcButton.OffsetRect(
			bHorz ? nButtonShift : 0,
			bHorz ? 0 : nButtonShift
			);
	} // if( bShowClose )
	if( bShowTabList )
	{
		m_rcBtnTabList = rcButton;
		rcButton.OffsetRect(
			bHorz ? nButtonShift : 0,
			bHorz ? 0 : nButtonShift
			);
	} // if( bShowTabList )
	if( bShowHelp )
	{
		m_rcBtnHelp = rcButton;
		rcButton.OffsetRect(
			bHorz ? nButtonShift : 0,
			bHorz ? 0 : nButtonShift
			);
	} // if( bShowHelp )
	if( bEnableScrollButtons )
	{
		m_rcBtnDown = rcButton;
		rcButton.OffsetRect(
			bHorz ? nButtonShift : 0,
			bHorz ? 0 : nButtonShift
			);
		m_rcBtnUp = rcButton;
	} // if( !bEqualWidth )

	m_nIndexVisFirst = -1;
	m_nIndexVisLast = -1;

	nVisibleNo = 0;
	m_bEnableTrackToolTips = false;
	for( nIndex = 0; nIndex < nItemCount; nIndex++ )
	{ // setup item rects
		TAB_ITEM_INFO * pTii = ItemGet( nIndex );
		ASSERT_VALID( pTii );
		if( !pTii->VisibleGet() )
			continue;

		pTii->m_bHelperToolTipAvail = false;
		if( bEqualWidth && !bCancelEqualWidth )
		{
			if( bHorz )
			{
				if( pTii->m_sizeLastMeasuredItem.cx > nEqualItemExtent )
				{
					m_bEnableTrackToolTips = true;
					pTii->m_bHelperToolTipAvail = true;
				}
				pTii->m_sizeLastMeasuredItem.cx = nEqualItemExtent;
			}
			else
			{
				if( pTii->m_sizeLastMeasuredItem.cy > nEqualItemExtent )
				{
					m_bEnableTrackToolTips = true;
					pTii->m_bHelperToolTipAvail = true;
				}
				pTii->m_sizeLastMeasuredItem.cy = nEqualItemExtent;
			}
		}
		
		const CSize & _size = pTii->GetLastMeasuredItemSize();
		CRect rcItem( 0, 0, _size.cx, _size.cy );

		bool bGroupStart = bGrouped && pTii->GroupStartGet();
//		bool bInGroupActive = bGrouped && pTii->InGroupActiveGet();
		
		switch( dwOrientation )
		{
		case __ETWS_ORIENT_TOP:
		case __ETWS_ORIENT_BOTTOM:
			if( nVisibleNo != 0
				&& bGroupStart
				)
				nItemRectStartPos += __EXTTAB_BETWEEN_GROUP_GAP_DX;

			rcItem.OffsetRect( nItemRectStartPos, nItemRectOffs );
			nItemRectStartPos += _size.cx;
		break;
		case __ETWS_ORIENT_LEFT:
		case __ETWS_ORIENT_RIGHT:
			if( nVisibleNo != 0
				&& bGroupStart
				)
				nItemRectStartPos += __EXTTAB_BETWEEN_GROUP_GAP_DY;

			rcItem.OffsetRect( nItemRectOffs, nItemRectStartPos );
			nItemRectStartPos += _size.cy;
		break;
#ifdef _DEBUG
		default:
			ASSERT( FALSE );
		break;
#endif // _DEBUG
		} // switch( dwOrientation )

		pTii->ItemRectSet( rcItem );

		rcItem.OffsetRect(
			bHorz
				? (m_rcTabItemsArea.left - m_nScrollPos)
				: 0
			,
			bHorz
				? 0
				: (m_rcTabItemsArea.top - m_nScrollPos)
			);

		INT nItemExtentMinVal =
			bHorz ? rcItem.left : rcItem.top;
		
		INT nItemExtentMaxVal =
			bHorz ? rcItem.right : rcItem.bottom;
		
		INT nAreaExtentMinVal =
			0;
			// bHorz ? m_rcTabItemsArea.left : m_rcTabItemsArea.top;
		INT nAreaExtentMaxVal =
			bHorz
				? (m_rcTabItemsArea.right - m_rcTabItemsArea.left)
				: (m_rcTabItemsArea.bottom - m_rcTabItemsArea.top)
				;
			//bHorz ? m_rcTabItemsArea.right : m_rcTabItemsArea.bottom;
		nAreaExtentMaxVal += __EXTTAB_ADD_END_SCROLL_SPACE + 2;
		if( m_nIndexVisFirst < 0 )
		{
			if( nItemExtentMaxVal >= nAreaExtentMinVal )
				m_nIndexVisFirst = nIndex;
		}
		if( nItemExtentMinVal <= nAreaExtentMaxVal )
			m_nIndexVisLast = nIndex;

		nVisibleNo++;
	} // setup item rects
	ASSERT( nVisibleNo == m_nVisibleItemCount );

//	if(		OnTabWndToolTipQueryEnabled()
//		&&	m_wndToolTip.GetSafeHwnd() != NULL
//		)
//	{
//		m_wndToolTip.Activate(
//			m_bEnableTrackToolTips ? TRUE : FALSE
//			);
//	}

//	if( m_nIndexVisFirst < 0 )
//		m_nIndexVisFirst = 0;
//	if( m_nIndexVisLast < 0 )
//		m_nIndexVisLast = 0;

	if( m_nIndexVisFirst < 0 || m_nIndexVisLast < 0 )
	{
		m_nIndexVisFirst = m_nIndexVisLast = -1;
		bEnableScrollButtons = false;
	}
	else
	{
		if( m_nIndexVisFirst > m_nIndexVisLast )
			m_nIndexVisLast = m_nIndexVisFirst;
		ASSERT( m_nIndexVisFirst <= m_nIndexVisLast );
		ASSERT( 0 <= m_nIndexVisFirst && m_nIndexVisFirst < nItemCount );
		ASSERT( 0 <= m_nIndexVisLast && m_nIndexVisLast < nItemCount );
	}


	// auto enable/disable scroll buttons
	m_dwTabWndStyle &= ~(__ETWS_ENABLED_BTN_UP|__ETWS_ENABLED_BTN_DOWN);

	if( bEnableScrollButtons )
	{
		if( _IsScrollAvail() && m_nScrollPos > 0 )
			m_dwTabWndStyle |= __ETWS_ENABLED_BTN_UP;
		if( _IsScrollAvail() && m_nScrollPos < m_nScrollMaxPos )
			m_dwTabWndStyle |= __ETWS_ENABLED_BTN_DOWN;
	}

	ASSERT_VALID( this );
}

void CExtTabWnd::_RecalcLayout_GetClientRect(
	CRect & rcClient
	)
{
	ASSERT_VALID( this );
	if( !m_rcRecalcLayout.IsRectEmpty() )
		rcClient = m_rcRecalcLayout;
	else
	{
		ASSERT( GetSafeHwnd() != NULL );
		ASSERT( ::IsWindow(GetSafeHwnd()) );
		GetClientRect( &rcClient );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CExtTabWnd message handlers

void CExtTabWnd::OnPaint() 
{
	ASSERT_VALID( this );

CRuntimeClass * pPaintManagerRTC =
		g_PaintManager->GetRuntimeClass();
	if( m_pHelperLastPaintManagerRTC != pPaintManagerRTC )
	{
		m_pHelperLastPaintManagerRTC = pPaintManagerRTC;
		m_bDelayRecalcLayout = true;
	} // if( m_pHelperLastPaintManagerRTC != pPaintManagerRTC )

	// recalc delayed layout first
	_RecalcLayoutImpl();

CPaintDC dcPaint( this );
CRect rcClient;
	GetClientRect( &rcClient );
	if( rcClient.IsRectEmpty() )
		return;

	{
		CExtMemoryDC dc( &dcPaint, &rcClient );
		OnTabWndDrawEntire( dc, rcClient );
	}
	g_PaintManager->OnPaintSessionComplete( this );
}


void CExtTabWnd::OnTabWndMeasureItemAreaMargins(
	CDC & dcMeasure,
	LONG & nSpaceBefore,
	LONG & nSpaceAfter,
	LONG & nSpaceOver
	)
{
	ASSERT_VALID( this );
	ASSERT( dcMeasure.GetSafeHdc() != NULL );

	g_PaintManager->TabWnd_MeasureItemAreaMargins(
		this,
		dcMeasure,
		nSpaceBefore,
		nSpaceAfter,
		nSpaceOver
		);
}

void CExtTabWnd::OnTabWndDrawEntire(
	CDC & dc,
	CRect & rcClient
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

bool bGroupedMode =
		(GetTabWndStyle() & __ETWS_GROUPED) ? true : false;

	OnTabWndEraseClientArea(
		dc,
		rcClient,
		m_rcTabItemsArea,
		m_rcTabNearBorderArea,
		OrientationGet(),
		bGroupedMode
		);

LONG nItemCount = ItemGetCount();
LONG nVisCount = ItemGetVisibleCount();
	if( nItemCount > 0 && nVisCount > 0 && m_nIndexVisFirst >= 0 )
	{
		ASSERT( m_nIndexVisFirst <= m_nIndexVisLast );
		ASSERT( 0 <= m_nIndexVisFirst && m_nIndexVisFirst < nItemCount );
		ASSERT( 0 <= m_nIndexVisLast && m_nIndexVisLast < nItemCount );

		bool bHorz = OrientationIsHorizontal();
		bool bTopLeft = OrientationIsTopLeft();
		bool bCenteredText =
			(GetTabWndStyle() & __ETWS_CENTERED_TEXT) ? true : false;
		bool bInvertedVerticalMode = (GetTabWndStyle() & __ETWS_INVERT_VERT_FONT) ? true : false;

		CRect rcSetMargins(
			( (!bHorz) && (!bTopLeft) ) ? 1 : 0,
			(   bHorz  && (!bTopLeft) ) ? 1 : 0,
			( (!bHorz) &&   bTopLeft  ) ? 1 : 0,
			(   bHorz  &&   bTopLeft  ) ? 1 : 0
			);
		CRect rcPaintItems( m_rcTabItemsArea );
		rcPaintItems.InflateRect(
			bHorz ? 1 : 0,
			bHorz ? 0 : 1
			);
		if( bGroupedMode )
			rcPaintItems.InflateRect(
				bHorz ? 0 : 1,
				bHorz ? 1 : 0
				);
		rcPaintItems.InflateRect(
			rcSetMargins.left,
			rcSetMargins.top,
			rcSetMargins.right,
			rcSetMargins.bottom
			);
		CRgn rgnPaint;
		if( !rgnPaint.CreateRectRgnIndirect(&rcPaintItems) )
		{
			ASSERT( FALSE );
			return;
		}
		dc.SelectClipRgn( &rgnPaint );

		for( LONG nIndex = m_nIndexVisFirst; nIndex <= m_nIndexVisLast; nIndex++ )
		{ 
			// paint items
			TAB_ITEM_INFO * pTii = ItemGet( nIndex );
			ASSERT_VALID( pTii );
			if( !pTii->VisibleGet() )
				continue;

			CRect rcItem = pTii->ItemRectGet();
			//rcItem.OffsetRect(
			//	bHorz ? -m_nScrollPos : 0,
			//	bHorz ? 0 : -m_nScrollPos
			//	);
			rcItem.OffsetRect(
				bHorz
					? (m_rcTabItemsArea.left - m_nScrollPos)
					: 0
				,
				bHorz
					? 0
					: (m_rcTabItemsArea.top - m_nScrollPos)
				);
			if( !dc.RectVisible( &rcItem ) )
				continue;

			CSize _sizeTextMeasured = pTii->GetLastMeasuredTextSize();
			bool bSelected = pTii->SelectedGet();
			CFont & font = _GetTabWndFont( bSelected );
			CFont * pFont = &font;
			CExtSafeString sText = pTii->TextGet();
			CExtCmdIcon * pIcon = pTii->IconGetPtr();
			
			bool bInGroupActive = pTii->InGroupActiveGet();
			OnTabWndDrawItem(
				dc,
				m_rcTabItemsArea,
				nIndex,
				pTii,
				bTopLeft,
				bHorz,
				bSelected,
				bCenteredText,
				bGroupedMode,
				bInGroupActive,
				bInvertedVerticalMode,
				rcItem,
				_sizeTextMeasured,
				pFont,
				sText,
				pIcon
				);
			
			dc.SelectClipRgn( &rgnPaint );

		} // paint items

		dc.SelectClipRgn( NULL );
	} // if( nItemCount > 0 && nVisCount > 0 && m_nIndexVisFirst >= 0 )

//bool bAnyButtonTracked =
//			m_nPushedTrackingButton == __ETWH_BUTTON_LEFTUP
//		||	m_nPushedTrackingButton == __ETWH_BUTTON_RIGHTDOWN
//		||	m_nPushedTrackingButton == __ETWH_BUTTON_HELP
//		||	m_nPushedTrackingButton == __ETWH_BUTTON_CLOSE
//		;
//bool bAnyMouseButtonPressed = false;
//	if( bAnyButtonTracked )
//		bAnyMouseButtonPressed =
//			CExtPopupMenuWnd::IsKeyPressed(VK_LBUTTON)
//		||	CExtPopupMenuWnd::IsKeyPressed(VK_MBUTTON)
//		||	CExtPopupMenuWnd::IsKeyPressed(VK_RBUTTON)
//		;

CPoint ptCursor( 0, 0 );
	::GetCursorPos( &ptCursor );
	ScreenToClient( &ptCursor );

bool bTopLeft = OrientationIsTopLeft();
bool bHorz = OrientationIsHorizontal();

	if( !m_rcBtnUp.IsRectEmpty()
		&& dc.RectVisible( &m_rcBtnUp )
		)
	{
//		bool bEnabled =
//			_IsScrollAvail() && m_nScrollPos > 0;
		bool bEnabled = (GetTabWndStyle() & __ETWS_ENABLED_BTN_UP) ? true : false;
		bool bHover = bEnabled && m_rcBtnUp.PtInRect(ptCursor) ? true : false;
		//bool bPushed = bEnabled && bHover && bAnyButtonTracked;
		OnTabWndDrawButton(
			dc,
			m_rcBtnUp,
			__ETWH_BUTTON_LEFTUP,
			bTopLeft,
			bHorz,
			bEnabled,
			bHover,
			m_bPushedUp,
			bGroupedMode
			);
	}
	if( !m_rcBtnDown.IsRectEmpty()
		&& dc.RectVisible( &m_rcBtnDown )
		)
	{
//		bool bEnabled =
//			_IsScrollAvail() && m_nScrollPos < m_nScrollMaxPos;
		bool bEnabled = (GetTabWndStyle() & __ETWS_ENABLED_BTN_DOWN) ? true : false;
		bool bHover = bEnabled && m_rcBtnDown.PtInRect(ptCursor) ? true : false;
		//bool bPushed = bEnabled && bHover && bAnyButtonTracked;
		OnTabWndDrawButton(
			dc,
			m_rcBtnDown,
			__ETWH_BUTTON_RIGHTDOWN,
			bTopLeft,
			bHorz,
			bEnabled,
			bHover,
			m_bPushedDown,
			bGroupedMode
			);
	}
	if( !m_rcBtnHelp.IsRectEmpty()
		&& dc.RectVisible( &m_rcBtnHelp )
		)
	{
		bool bEnabled = (GetTabWndStyle() & __ETWS_ENABLED_BTN_HELP) ? true : false;
		bool bHover = bEnabled && m_rcBtnHelp.PtInRect(ptCursor) ? true : false;
		//bool bPushed = bEnabled && bHover && bAnyButtonTracked;
		OnTabWndDrawButton(
			dc,
			m_rcBtnHelp,
			__ETWH_BUTTON_HELP,
			bTopLeft,
			bHorz,
			bEnabled,
			bHover,
			m_bPushedHelp,
			bGroupedMode
			);
	}
	if( !m_rcBtnClose.IsRectEmpty()
		&& dc.RectVisible( &m_rcBtnClose )
		)
	{
		bool bEnabled = (GetTabWndStyle() & __ETWS_ENABLED_BTN_CLOSE) ? true : false;
		bool bHover = bEnabled && m_rcBtnClose.PtInRect(ptCursor) ? true : false;
		//bool bPushed = bEnabled && bHover && bAnyButtonTracked;
		OnTabWndDrawButton(
			dc,
			m_rcBtnClose,
			__ETWH_BUTTON_CLOSE,
			bTopLeft,
			bHorz,
			bEnabled,
			bHover,
			m_bPushedClose,
			bGroupedMode
			);
	}
	if( !m_rcBtnTabList.IsRectEmpty()
		&& dc.RectVisible( &m_rcBtnTabList )
		)
	{
		bool bEnabled = (GetTabWndStyle() & __ETWS_ENABLED_BTN_TAB_LIST) ? true : false;
		bool bHover = bEnabled && m_rcBtnTabList.PtInRect(ptCursor) ? true : false;
		OnTabWndDrawButton(
			dc,
			m_rcBtnTabList,
			__ETWH_BUTTON_TAB_LIST,
			bTopLeft,
			bHorz,
			bEnabled,
			bHover,
			m_bPushedTabList,
			bGroupedMode
			);
	}
}

void CExtTabWnd::OnTabWndDrawButton(
	CDC & dc,
	CRect & rcButton,
	LONG nHitTest,
	bool bTopLeft,
	bool bHorz,
	bool bEnabled,
	bool bHover,
	bool bPushed,
	bool bGroupedMode
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	g_PaintManager->PaintTabButton(
		dc,
		rcButton,
		nHitTest,
		bTopLeft,
		bHorz,
		bEnabled,
		bHover,
		bPushed,
		bGroupedMode,
		this
		);
}

void CExtTabWnd::OnTabWndEraseClientArea(
	CDC & dc,
	CRect & rcClient,
	CRect & rcTabItemsArea,
	CRect & rcTabNearBorderArea,
	DWORD dwOrientation,
	bool bGroupedMode
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	g_PaintManager->PaintTabClientArea(
		dc,
		rcClient,
		rcTabItemsArea,
		rcTabNearBorderArea,
		dwOrientation,
		bGroupedMode,
		this
		);
}

void CExtTabWnd::OnTabWndDrawItem(
	CDC & dc,
	CRect & rcTabItemsArea,
	LONG nItemIndex,
	CExtTabWnd::TAB_ITEM_INFO * pTii,
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
	ASSERT_VALID( this );
	ASSERT_VALID( pTii );
	pTii;
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pFont != NULL );
	ASSERT( pFont->GetSafeHandle() != NULL );
	nItemIndex;

	g_PaintManager->PaintTabItem(
		dc,
		rcTabItemsArea,
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
		pIcon,
		this
		);
}

bool CExtTabWnd::OnTabWndMouseTrackingPushedStart(
	INT nMouseButton, // MK_... values
	LONG nHitTest
	)
{
	ASSERT_VALID( this );

	if(		m_nPushedTrackingHitTest != __ETWH_NOWHERE
		&&	m_nPushedTrackingButton != (-1L)
		)
	{
		if(		m_nPushedTrackingHitTest != nMouseButton
			||	m_nPushedTrackingButton != nHitTest
			)
			return false;
	}

	m_bPushedUp = false;
	m_bPushedDown = false;
	m_bPushedHelp = false;
	m_bPushedClose = false;
	m_bPushedTabList = false;

	switch( nHitTest )
	{
	case __ETWH_BUTTON_LEFTUP:
		if( nMouseButton != MK_LBUTTON )
			return false;
		if( (GetTabWndStyle() & __ETWS_ENABLED_BTN_UP) == 0 )
			return false;
		m_bPushedUp = true;
		UpdateTabWnd( true );
		break;
	case __ETWH_BUTTON_RIGHTDOWN:
		if( nMouseButton != MK_LBUTTON )
			return false;
		if( (GetTabWndStyle() & __ETWS_ENABLED_BTN_DOWN) == 0 )
			return false;
		m_bPushedDown = true;
		UpdateTabWnd( true );
		break;
	case __ETWH_BUTTON_HELP:
		if( nMouseButton != MK_LBUTTON )
			return false;
		if( (GetTabWndStyle() & __ETWS_ENABLED_BTN_HELP) == 0 )
			return false;
		m_bPushedHelp = true;
		UpdateTabWnd( true );
		break;
	case __ETWH_BUTTON_CLOSE:
		if( nMouseButton != MK_LBUTTON )
			return false;
		if( (GetTabWndStyle() & __ETWS_ENABLED_BTN_CLOSE) == 0 )
			return false;
		m_bPushedClose = true;
		UpdateTabWnd( true );
		break;
	case __ETWH_BUTTON_TAB_LIST:
		if( nMouseButton != MK_LBUTTON )
			return false;
		if( (GetTabWndStyle() & __ETWS_ENABLED_BTN_TAB_LIST) == 0 )
			return false;
		m_bPushedTabList = true;
		UpdateTabWnd( true );
		break;
	} // switch( nHitTest )

	m_nPushedTrackingHitTest = nMouseButton;
	m_nPushedTrackingButton = nHitTest;
	m_bTrackingButtonPushed = true;

	CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );

	return true;
}

void CExtTabWnd::OnTabWndMouseTrackingPushedStop(
	bool bEnableReleaseCapture // = true
	)
{
	ASSERT_VALID( this );

	if( (GetTabWndStyle() & __ETWS_SCROLL_BY_PAGE) == 0
		&&	m_nScrollDirectionRest != 0
		)
		SendMessage( WM_CANCELMODE );

	m_ptStartDrag.x = m_ptStartDrag.y = -1;
	switch( m_nPushedTrackingButton )
	{
	case __ETWH_BUTTON_LEFTUP:
		m_bPushedUp = false;
		UpdateTabWnd( true );
		break;
	case __ETWH_BUTTON_RIGHTDOWN:
		m_bPushedDown = false;
		UpdateTabWnd( true );
		break;
	case __ETWH_BUTTON_HELP:
		m_bPushedHelp = false;
		UpdateTabWnd( true );
		break;
	case __ETWH_BUTTON_CLOSE:
		m_bPushedClose = false;
		UpdateTabWnd( true );
		break;
	case __ETWH_BUTTON_TAB_LIST:
		m_bPushedTabList = false;
		UpdateTabWnd( true );
		break;
	} // switch( m_nPushedTrackingButton )

	m_nPushedTrackingHitTest = __ETWH_NOWHERE;
	m_nPushedTrackingButton = (-1L);
	m_bTrackingButtonPushed = false;
	if( bEnableReleaseCapture && CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
		CExtMouseCaptureSink::ReleaseCapture();
}

bool CExtTabWnd::OnTabWndStartDrag( LONG nIndex )
{
	ASSERT_VALID( this );
	ASSERT( ItemGetCount() > 0 );
	ASSERT(
		nIndex >= 0
		&& nIndex < ItemGetCount()
		);
	nIndex;
	return false;
}

bool CExtTabWnd::OnTabWndClickedButton(
	LONG nHitTest,
	bool bButtonPressed,
	INT nMouseButton, // MK_... values
	UINT nMouseEventFlags
	)
{
	ASSERT_VALID( this );
	nMouseEventFlags;

	if( bButtonPressed )
	{
		if( OnTabWndMouseTrackingPushedStart( nMouseButton, nHitTest ) )
		{
			Invalidate();
			UpdateWindow();
			if(		nHitTest == __ETWH_BUTTON_LEFTUP
				||	nHitTest == __ETWH_BUTTON_RIGHTDOWN
				)
			{
				OnTabWndDoScroll(
					(nHitTest == __ETWH_BUTTON_LEFTUP) ? -1 : +1,
					true
					);
			}
		}
	}
	else
	{
		OnTabWndMouseTrackingPushedStop();

		if(		nMouseButton == MK_LBUTTON 
			&&	nHitTest == __ETWH_BUTTON_TAB_LIST
			&&	(GetTabWndStyle() & __ETWS_ENABLED_BTN_TAB_LIST) != 0
			)
		{
			// show tab list popup menu
			CExtPopupMenuWnd * pPopup = new CExtPopupMenuWnd;
			if( pPopup->CreatePopupMenu( GetSafeHwnd() ) )
			{
				UINT nStartID = 0xffff;
				for( LONG nIndex = 0; nIndex < m_arrItems.GetSize(); nIndex++ )
				{
					TAB_ITEM_INFO * pTii = ItemGet( nIndex );
					if( pTii != NULL ) 
					{
						bool bGrouped = (GetTabWndStyle()&__ETWS_GROUPED) ? true : false;
						if(		bGrouped 
							&&	nIndex != 0 
							)
						{
							LONG nGroupStart;
							LONG nGroupEnd;
							ItemGetGroupRange(
								nIndex,
								nGroupStart,
								nGroupEnd
								);
							if( nGroupStart == nIndex )
								VERIFY( pPopup->ItemInsertCommand( CExtPopupMenuWnd::TYPE_SEPARATOR, -1 ) );
						}

						CExtCmdIcon* pCmdIcon = OnTabWndQueryItemIcon(pTii);
						HICON hIcon = NULL;
						if( pCmdIcon )
							hIcon = 
								pCmdIcon->IsEmpty() 
									? NULL 
									: pCmdIcon->GetIcon();
						VERIFY( 
							pPopup->ItemInsertCommand(
								nStartID + nIndex,
								-1,
								OnTabWndQueryItemText(pTii),
								NULL,
								hIcon,
								true,
								( SelectionGet() == nIndex )
								) 
							);
					}	
				}
				CPoint ptTrack( m_rcBtnTabList.left, m_rcBtnTabList.bottom );
				ClientToScreen( &ptTrack );
				CRect rcExcludeArea( m_rcBtnTabList );
				ClientToScreen( &rcExcludeArea );
				
				DWORD nAlign = TPMX_LEFTALIGN; 
				switch( __ETWS_ORIENT_MASK & OrientationGet() ) 
				{
				case __ETWS_ORIENT_TOP:
					nAlign = TPMX_TOPALIGN;
					break;
				case __ETWS_ORIENT_BOTTOM:
					nAlign = TPMX_BOTTOMALIGN;
					break;
				case __ETWS_ORIENT_LEFT:
					nAlign = TPMX_LEFTALIGN;
					break;
				case __ETWS_ORIENT_RIGHT:
					nAlign = TPMX_RIGHTALIGN;
					break;
				}

				UINT nCmdRetVal = 0;
				if(	pPopup->TrackPopupMenu(
						TPMX_OWNERDRAW_FIXED
							| nAlign
							| TPMX_COMBINE_DEFAULT
							| TPMX_DO_MESSAGE_LOOP
							| TPMX_NO_WM_COMMAND
							| TPMX_NO_CMD_UI
							| TPMX_NO_HIDE_RARELY,
						ptTrack.x, 
						ptTrack.y,
						&rcExcludeArea,
						NULL,
						_CbPaintCombinedTabListBtnContent,
						&nCmdRetVal
						)
					)
				{
					// user selected the nCmdRetVal item
					if( nCmdRetVal != 0 )
						SelectionSet( nCmdRetVal - nStartID , true, true );
					
					Invalidate();
				}
				else
				{
					ASSERT( FALSE );
					delete pPopup;
				}
			}
			else
			{
				ASSERT( FALSE );
				delete pPopup;
			}
		}
	}

	return true;
}

void CExtTabWnd::_CbPaintCombinedTabListBtnContent(
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
	ASSERT( eCombineAlign != CExtPopupMenuWnd::__CMBA_NONE );
	
	if( rcExcludeArea.IsRectEmpty() )
		return;
	
	CRect rcExcludeAreaX(rcExcludeArea);
	refWndMenu.ScreenToClient( &rcExcludeAreaX );

	g_PaintManager->PaintTabButton(
		dc,
		rcExcludeAreaX,
		__ETWH_BUTTON_TAB_LIST,
		true,	// bTopLeft
		false,	// bHorz
		true,	// bEnabled,
		false,	// bHover,
		false,	// bPushed,
		false,	// bGroupedMode
		(CObject*)&refWndMenu
		);
}

bool CExtTabWnd::OnTabWndClickedItem(
	LONG nItemIndex,
	bool bButtonPressed,
	INT nMouseButton, // MK_... values
	UINT nMouseEventFlags
	)
{
	ASSERT_VALID( this );
	nMouseEventFlags;

	if( bButtonPressed )
	{
		ASSERT( ItemGetCount() > 0 );
		ASSERT( nItemIndex >= 0 && nItemIndex < ItemGetCount() );
		if( OnTabWndMouseTrackingPushedStart( nMouseButton, nItemIndex ) )
		{
			SelectionSet( nItemIndex, true, true );
			return ( nMouseButton == MK_RBUTTON ) ? false : true;
		}
	}
	OnTabWndMouseTrackingPushedStop();
	return ( nMouseButton == MK_RBUTTON ) ? false : true;
}

void CExtTabWnd::OnTabWndDoScroll(
	LONG nStep,
	bool bSmoothScroll // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( nStep != 0 );
	
	if( !_IsScrollAvail() )
		return;
//	if( _IsScrolling() )
//		return;
	if(		m_rcTabItemsArea.left >= m_rcTabItemsArea.right
		||	m_rcTabItemsArea.top >= m_rcTabItemsArea.bottom
		||	ItemGetCount() == 0
		||	ItemGetVisibleCount() == 0
		)
		return;

bool bHorz = OrientationIsHorizontal();
INT nScrollRange = bHorz
		? m_rcTabItemsArea.right - m_rcTabItemsArea.left
		: m_rcTabItemsArea.bottom - m_rcTabItemsArea.top
		;
	ASSERT( nScrollRange > 0 );

	if( nStep < 0 )
	{ // up
		if( m_nScrollPos == 0 )
			return;
		ASSERT( (GetTabWndStyle() & __ETWS_ENABLED_BTN_UP) != 0 );

		if( bSmoothScroll )
		{
			if( GetTabWndStyle() & __ETWS_SCROLL_BY_PAGE )
				m_nScrollDirectionRest = -nScrollRange;
			else
				m_nScrollDirectionRest = -m_nScrollPos;
			SetTimer(
				__EXTTAB_SCROLL_TIMER_ID,
				__EXTTAB_SCROLL_TIMER_PERIOD,
				NULL
				);
			return;
		}

		//m_nScrollPos -= nScrollRange;
		//m_nScrollPos--;
		m_nScrollPos += nStep;
		if( m_nScrollPos < 0 )
			m_nScrollPos = 0;

	} // up
	else
	{ // down
		if( m_nScrollPos == m_nScrollMaxPos )
			return;
		ASSERT( (GetTabWndStyle() & __ETWS_ENABLED_BTN_DOWN) != 0 );

		if( bSmoothScroll )
		{
			if( GetTabWndStyle() & __ETWS_SCROLL_BY_PAGE )
				m_nScrollDirectionRest = nScrollRange;
			else
				m_nScrollDirectionRest = m_nScrollMaxPos - m_nScrollPos;
			SetTimer(
				__EXTTAB_SCROLL_TIMER_ID,
				__EXTTAB_SCROLL_TIMER_PERIOD,
				NULL
				);
			return;
		}

		//m_nScrollPos += nScrollRange;
		//m_nScrollPos++;
		m_nScrollPos += nStep;
		if( m_nScrollPos > m_nScrollMaxPos )
			m_nScrollPos = m_nScrollMaxPos;
	} // down

	UpdateTabWnd( true );
}

LONG CExtTabWnd::SelectionSet(
	LONG nSelIndex,
	bool bEnsureVisible, // = false
	bool bUpdateTabWnd // = false
	)
{
	ASSERT_VALID( this );
LONG nOldSelection = m_nSelIndex;
	if( !OnTabWndSelectionChange( m_nSelIndex, nSelIndex, true ) )
		return m_nSelIndex;
LONG nCount = ItemGetCount();
	if( nSelIndex < 0 )
		m_nSelIndex = -1;
	else
	{
		ASSERT( nSelIndex < nCount );
		m_nSelIndex = nSelIndex;
	} // else from if( nSelIndex < 0 )
	if( nOldSelection >= 0 )
	{
		ASSERT( nOldSelection < nCount );
		
		TAB_ITEM_INFO * pTii = ItemGet( nOldSelection );
		pTii->ModifyItemStyle(
			__ETWI_SELECTED,
			0
			);
	}
	if( m_nSelIndex >= 0 )
	{
		ASSERT( m_nSelIndex < nCount );

		TAB_ITEM_INFO * pTii = ItemGet( m_nSelIndex );
		ASSERT_VALID( pTii );
		//ASSERT( pTii->VisibleGet() );
		
		pTii->ModifyItemStyle(
			0,
			__ETWI_SELECTED
			);
	}
	if( !OnTabWndSelectionChange( nOldSelection, m_nSelIndex, false ) )
		return m_nSelIndex;

	if( bEnsureVisible && m_nSelIndex >= 0 && ItemGetCount() > 0 )
	{
		ASSERT( m_nSelIndex < ItemGetCount() );
		// item should not have "invisible" style
		VERIFY(
			ItemEnsureVisible(
				m_nSelIndex,
				bUpdateTabWnd
				)
			);
	} // if( bEnsureVisible )
	else
		UpdateTabWnd( bUpdateTabWnd );
	return nOldSelection;
}

bool CExtTabWnd::SelectionDelay(
	LONG nSelIndex, // = -1, // -1 - to cancel
	DWORD dwMilliseconds // = 0 // should be > 0 if nSelIndex >= 0
	)
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT( ::IsWindow(GetSafeHwnd()) );
	if( nSelIndex < 0 )
	{
		if( m_nDelayedSelIndex >= 0 )
		{
			KillTimer( __EXTTAB_SELECTION_DELAY_TIMER_ID );
			m_nDelayedSelIndex = -1;
		}
		return true;
	}

	if( m_nDelayedSelIndex >= 0 )
	{
		KillTimer( __EXTTAB_SELECTION_DELAY_TIMER_ID );
		m_nDelayedSelIndex = -1;
	}

INT nVisibleItemCount = ItemGetVisibleCount();
	if( nVisibleItemCount == 0 )
		return false;
INT nItemCount = ItemGetCount();
	if( nSelIndex >= nItemCount )
	{
		ASSERT( FALSE );
		return false;
	}
	if( ! (ItemGet( nSelIndex )->VisibleGet()) )
	{
		ASSERT( FALSE );
		return false;
	}
	ASSERT( dwMilliseconds > 0 );
	m_nDelayedSelIndex = nSelIndex;
	SetTimer(
		__EXTTAB_SELECTION_DELAY_TIMER_ID,
		dwMilliseconds,
		NULL
		);
	return true;
}

LONG CExtTabWnd::ItemFindByLParam(
	LPARAM lParam,
	LONG nIndexStartSearch, // = -1
	bool bIncludeVisible, // = true,
	bool bIncludeInvisible // = false
	) const
{
	ASSERT_VALID( this );
LONG nIndex = ( nIndexStartSearch < 0 )
		? 0
		: (nIndexStartSearch + 1)
		;
LONG nCount = ItemGetCount();
	for( ; nIndex < nCount; nIndex++ )
	{
		const TAB_ITEM_INFO * pTii = ItemGet( nIndex );
		ASSERT_VALID( pTii );
		bool bItemVisible = pTii->VisibleGet();
		if( bIncludeVisible  && !bIncludeInvisible && !bItemVisible )
			continue;
		if( !bIncludeVisible && bIncludeInvisible  && bItemVisible )
			continue;
		LPARAM lParam2 = pTii->LParamGet();
		if( lParam == lParam2 )
			return nIndex;
	} // for( ; nIndex < nCount; nIndex++ )
	return -1;
}

LONG CExtTabWnd::ItemFindByStyle(
	DWORD dwItemStyleInclude,
	DWORD dwItemStyleExclude, // = __ETWI_INVISIBLE
	LONG nIndexStartSearch // = -1
	) const
{
	ASSERT_VALID( this );
LONG nIndex = ( nIndexStartSearch < 0 )
		? 0
		: (nIndexStartSearch + 1)
		;
LONG nCount = ItemGetCount();
	for( ; nIndex < nCount; nIndex++ )
	{
		const TAB_ITEM_INFO * pTii = ItemGet( nIndex );
		ASSERT_VALID( pTii );
		DWORD dwItemStyle = pTii->GetItemStyle();
		if( dwItemStyleInclude != 0 )
		{
			DWORD dwItemStylesReallyIncluded = dwItemStyle & dwItemStyleInclude;
			if( dwItemStylesReallyIncluded == 0 )
				continue;
		}
		if( dwItemStyleExclude != 0 )
		{
			DWORD dwItemStylesReallyExculded = dwItemStyle & dwItemStyleExclude;
			if( dwItemStylesReallyExculded != 0 )
				continue;
		}
		return nIndex;
	} // for( ; nIndex < nCount; nIndex++ )
	return -1;
}

bool CExtTabWnd::OnTabWndSelectionChange(
	LONG nOldItemIndex,
	LONG nNewItemIndex,
	bool bPreSelectionTest
	)
{
	ASSERT_VALID( this );
	nOldItemIndex;
	nNewItemIndex;
	bPreSelectionTest;
	return true;
}

CExtCmdIcon * CExtTabWnd::OnTabWndQueryItemIcon(
	const CExtTabWnd::TAB_ITEM_INFO * pTii
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTii );
	ASSERT( pTii->GetTabWnd() == this );
	if( pTii != NULL ) 
		return (CExtCmdIcon*)&pTii->m_icon;
	return NULL;
}

__EXT_MFC_SAFE_LPCTSTR CExtTabWnd::OnTabWndQueryItemText(
	const CExtTabWnd::TAB_ITEM_INFO * pTii
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTii );
	ASSERT( pTii->GetTabWnd() == this );
	if( pTii != NULL ) 
		return pTii->m_sText;
	return NULL;
}

void CExtTabWnd::OnTabWndUpdateItemMeasure(
	CExtTabWnd::TAB_ITEM_INFO * pTii,
	CDC & dcMeasure,
	CSize & sizePreCalc
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTii );
	ASSERT( dcMeasure.GetSafeHdc() != NULL );
	ASSERT( pTii->GetTabWnd() == this );
	pTii;
	dcMeasure;
	sizePreCalc;
}

void CExtTabWnd::OnTabWndItemInsert(
	LONG nItemIndex,
	CExtTabWnd::TAB_ITEM_INFO * pTii
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTii );
	ASSERT( pTii->GetTabWnd() == this );
	ASSERT( ItemGetCount() > 0 );
	ASSERT( nItemIndex >= 0 && nItemIndex < ItemGetCount() );
	ASSERT( ItemGetIndexOf( pTii ) == nItemIndex );
	pTii;
	nItemIndex;
}

void CExtTabWnd::OnTabWndRemoveItem(
	LONG nItemIndex,
	LONG nCount,
	bool bPreRemove
	)
{
	ASSERT_VALID( this );
	nItemIndex;
	nCount;
	bPreRemove;
}

void CExtTabWnd::OnTabWndRemoveAllItems(
	bool bPreRemove
	)
{
	ASSERT_VALID( this );
	bPreRemove;
}

CSize CExtTabWnd::OnTabWndCalcButtonSize(
	CDC & dcMeasure,
	LONG nTabAreaMetric // vertical max width or horizontal max heights of all tabs
	)
{
	ASSERT_VALID( this );
	ASSERT( dcMeasure.GetSafeHdc() != NULL );
	dcMeasure;
	nTabAreaMetric;
CSize _sizeTmp( // (+ v.2.22)
		::GetSystemMetrics(SM_CXSMSIZE),
		::GetSystemMetrics(SM_CYSMSIZE)
		);
int nMetric =
	max( _sizeTmp.cx, _sizeTmp.cy ) + 1;
CSize _sizeButton(
		max( nMetric, __EXTTAB_BTN_MIN_DX ),
		max( nMetric, __EXTTAB_BTN_MIN_DY )
		);
	return _sizeButton;
}

void CExtTabWnd::OnSysColorChange() 
{
	ASSERT_VALID( this );
	CWnd::OnSysColorChange();
	g_PaintManager.OnSysColorChange( this );
	g_CmdManager.OnSysColorChange( this );
	Invalidate();
}

LRESULT CExtTabWnd::OnDisplayChange( WPARAM wParam, LPARAM lParam )
{
LRESULT lResult = CWnd::OnDisplayChange( wParam, lParam );
	g_PaintManager.OnDisplayChange( this, (INT)wParam, CPoint(lParam) );
	g_CmdManager.OnDisplayChange( this, (INT)wParam, CPoint(lParam) );
	return lResult;
}

LRESULT CExtTabWnd::OnThemeChanged( WPARAM wParam, LPARAM lParam )
{
LRESULT lResult = Default();
	g_PaintManager.OnThemeChanged( this, wParam, lParam );
	g_CmdManager.OnThemeChanged( this, wParam, lParam );
	return lResult;
}

void CExtTabWnd::OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection) 
{
	ASSERT_VALID( this );
	CWnd::OnSettingChange(uFlags, lpszSection);
	g_PaintManager.OnSettingChange( this, uFlags, lpszSection );
	g_CmdManager.OnSettingChange( this, uFlags, lpszSection );
	Invalidate();
}

LRESULT CExtTabWnd::_OnQueryRepositionCalcEffect(WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID( this );
	lParam;
	if( !m_bReflectQueryRepositionCalcEffect )
		return 0L;
CExtControlBar::QUERY_REPOSITION_CALC_EFFECT_DATA * p_qrced =
		(CExtControlBar::QUERY_REPOSITION_CALC_EFFECT_DATA *)
			(wParam);
	ASSERT( p_qrced != NULL );
	ASSERT_VALID( p_qrced->m_pWndToReposChilds );
	ASSERT( p_qrced->m_pWndToReposChilds->GetSafeHwnd() != NULL );
	ASSERT( ::IsWindow( p_qrced->m_pWndToReposChilds->GetSafeHwnd() ) );
	if( p_qrced->IsQueryReposQuery() )
	{
		if( p_qrced->m_pWndToReposChilds->IsKindOf(
				RUNTIME_CLASS(CFrameWnd)
				)
			)
			p_qrced->ExcludeFromCenterSet();
	} // if( p_qrced->IsQueryReposQuery() )
	return 0L;
}

void CExtTabWnd::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	ASSERT_VALID( this );
	pWnd;
	point;
}

void CExtTabWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

DWORD dwWndStyle = GetStyle();
	if( (dwWndStyle & WS_VISIBLE) == 0 )
		return;

	if(		m_wndToolTip.GetSafeHwnd() != NULL
		&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
		)
		m_wndToolTip.DelTool( this, 1 );
	CWnd::CancelToolTips();

	UpdateTabWnd( true );
}

void CExtTabWnd::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CWnd::OnWindowPosChanged(lpwndpos);

DWORD dwWndStyle = GetStyle();
	if( (dwWndStyle & WS_VISIBLE) == 0 )
		return;

	if(		m_wndToolTip.GetSafeHwnd() != NULL
		&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
		)
		m_wndToolTip.DelTool( this, 1 );
	CWnd::CancelToolTips();

	UpdateTabWnd( true );
}

void CExtTabWnd::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
	
DWORD dwWndStyle = GetStyle();
	if( (dwWndStyle & WS_VISIBLE) == 0 )
		return;

	UpdateTabWnd( true );
}

void CExtTabWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
//	CWnd::OnLButtonDblClk(nFlags, point);
	nFlags;
	point;
}
void CExtTabWnd::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
//	CWnd::OnRButtonDblClk(nFlags, point);
	nFlags;
	point;
}
void CExtTabWnd::OnMButtonDblClk(UINT nFlags, CPoint point) 
{
//	CWnd::OnMButtonDblClk(nFlags, point);
	nFlags;
	point;
}
int CExtTabWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
//	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
	pDesktopWnd;
	nHitTest;
	message;

bool bTopParentActive = false;
HWND hWndForeground = ::GetForegroundWindow();
CWnd * pWndForeground =
		(hWndForeground == NULL)
			? NULL
			: CWnd::FromHandlePermanent(hWndForeground)
			;
	if( pWndForeground != NULL )
	{
		CWnd * pWndTopParent =
			GetTopLevelParent();
		if( pWndTopParent != NULL )
		{
			HWND hWndLastActivePopup = ::GetLastActivePopup( pWndTopParent->m_hWnd );
			CWnd * pWndLastActivePopup =
				(hWndLastActivePopup == NULL)
					? NULL
					: CWnd::FromHandlePermanent(hWndLastActivePopup)
					;
			if( pWndForeground == pWndLastActivePopup )
				bTopParentActive = true;
		}
	}

	if( !bTopParentActive )
	{
		HWND hWndParent = ::GetParent( GetSafeHwnd() );
		if( hWndParent != NULL )
			::SetFocus( hWndParent );
	}
	return MA_NOACTIVATE;
}
void CExtTabWnd::OnSetFocus(CWnd* pOldWnd) 
{
//	CWnd::OnSetFocus(pOldWnd);
	pOldWnd;
HWND hWndParent = ::GetParent( GetSafeHwnd() );
	if( hWndParent != NULL )
		::SetFocus( hWndParent );
}

void CExtTabWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( !_ProcessMouseClick( point, true, MK_LBUTTON, nFlags ) )
		CWnd::OnLButtonDown(nFlags, point);
}
void CExtTabWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( !_ProcessMouseClick( point, false, MK_LBUTTON, nFlags ) )
		CWnd::OnLButtonUp(nFlags, point);
}
void CExtTabWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if( !_ProcessMouseClick( point, true, MK_RBUTTON, nFlags ) )
		CWnd::OnRButtonDown(nFlags, point);
}
void CExtTabWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if( !_ProcessMouseClick( point, false, MK_RBUTTON, nFlags ) )
		CWnd::OnRButtonUp(nFlags, point);
}
void CExtTabWnd::OnMButtonDown(UINT nFlags, CPoint point) 
{
	if( !_ProcessMouseClick( point, true, MK_MBUTTON, nFlags ) )
		CWnd::OnMButtonDown(nFlags, point);
}
void CExtTabWnd::OnMButtonUp(UINT nFlags, CPoint point) 
{
	if( !_ProcessMouseClick( point, false, MK_MBUTTON, nFlags ) )
		CWnd::OnMButtonUp(nFlags, point);
}

HCURSOR CExtTabWnd::g_hCursor = ::LoadCursor( NULL, IDC_ARROW );

BOOL CExtTabWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
		return FALSE;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( CExtControlBar::FindHelpMode(this) )
		return FALSE;
	if( g_hCursor != NULL )
	{
//		HCURSOR hCursor = GetCursor();
//		if( hCursor != g_hCursor )
//		{
//			CPoint ptRealCursorPos;
//			::GetCursorPos( &ptRealCursorPos );
//			ScreenToClient( &ptRealCursorPos );
//			CRect rcClient;
//			GetClientRect( &rcClient );
//			if( rcClient.PtInRect(ptRealCursorPos) )
				::SetCursor( g_hCursor );
//		} // if( hCursor != g_hCursor )
		return TRUE;
	} // if( g_hCursor != NULL )
	
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CExtTabWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
//	CWnd::OnMouseMove(nFlags, point);
	nFlags;

	if( m_nPushedTrackingHitTest >= 0 )
	{
		LONG nIndexToStartDrag = m_nPushedTrackingButton;
		LONG nItemCount = ItemGetCount();
		if(		nItemCount == 0
			||	nIndexToStartDrag < 0
			||	nIndexToStartDrag >= nItemCount
			)
		{
			if(		m_wndToolTip.GetSafeHwnd() != NULL
				&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
				)
				m_wndToolTip.DelTool( this, 1 );
			CWnd::CancelToolTips();
			bool bCancelMode = true;
			if(		m_bPushedUp
				||	m_bPushedDown
				||	m_bPushedHelp
				||	m_bPushedClose
				||	m_bPushedTabList
				|| _IsScrolling()
				)
				bCancelMode = false;
			if( bCancelMode )
				SendMessage( WM_CANCELMODE );
			return;
		}
		if(		(abs(point.x-m_ptStartDrag.x)) > __EXTTAB_DRAG_START_DX
			||	(abs(point.y-m_ptStartDrag.y)) > __EXTTAB_DRAG_START_DY
			)
		{
			if( OnTabWndStartDrag(nIndexToStartDrag) )
			{
				if(		m_wndToolTip.GetSafeHwnd() != NULL
					&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
					)
					m_wndToolTip.DelTool( this, 1 );
				CWnd::CancelToolTips();
				return;
			}
		}
	}

	if( m_bTrackingButtonPushed )
	{
		if(		m_wndToolTip.GetSafeHwnd() != NULL
			&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
			)
			m_wndToolTip.DelTool( this, 1 );
		CWnd::CancelToolTips();
		return;
	}

LONG nHitTest = ItemHitTest( point );
bool bCancelTips = true;
	switch( nHitTest )
	{
	case __ETWH_BUTTON_LEFTUP:
	case __ETWH_BUTTON_RIGHTDOWN:
	case __ETWH_BUTTON_HELP:
	case __ETWH_BUTTON_CLOSE:
	case __ETWH_BUTTON_TAB_LIST:
		if( OnTabWndMouseTrackingHoverStart( nHitTest ) )
			bCancelTips = false;
		break;
	default:
		OnTabWndMouseTrackingHoverStop( true );
		if( ItemGetVisibleCount() > 0
			&& nHitTest >= 0
			)
		{
			TAB_ITEM_INFO * pTii = ItemGet(nHitTest);
			ASSERT_VALID( pTii );
			ASSERT( pTii->VisibleGet() );

			if(		(GetTabWndStyle() & __ETWS_HOVER_FOCUS)
				&&	m_nDelayedSelIndex != nHitTest
				)
			{
				if( m_nDelayedSelIndex >= 0 )
				{
					VERIFY( SelectionDelay() );
				}
				VERIFY(
					SelectionDelay(
						nHitTest,
						__EXTTAB_SELECTION_DELAY_TIMER_PERIOD
						)
					);
				ASSERT( m_nDelayedSelIndex == nHitTest );
			}
			else if(
					OnTabWndToolTipQueryEnabled()
				&&	m_wndToolTip.GetSafeHwnd() != NULL
				&&	m_bEnableTrackToolTips
				)
			{
				ASSERT( GetTabWndStyle() & __ETWS_EQUAL_WIDTHS );
				if( pTii->IsToolTipAvailByExtent() )
				{
					bCancelTips = false;
					CRect rcItem = pTii->ItemRectGet();
					m_wndToolTip.AddTool(
						this,
						pTii->TextGet(),
						&rcItem,
						1
						);
				}
			}
		
		}
		break;
	} // switch( nHitTest )

	if( bCancelTips )
	{
		if(		m_wndToolTip.GetSafeHwnd() != NULL
			&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
			)
			m_wndToolTip.DelTool( this, 1 );
		CWnd::CancelToolTips();
	}
}

void CExtTabWnd::OnCaptureChanged(CWnd *pWnd) 
{
	CWnd::OnCaptureChanged(pWnd);
	if( pWnd != NULL && pWnd != this )
	{
		OnTabWndMouseTrackingPushedStop( false );
		OnTabWndMouseTrackingHoverStop( false );
	}
}

void CExtTabWnd::OnCancelMode() 
{
	CWnd::OnCancelMode();

	if( m_nScrollDirectionRest != 0 )
	{
		m_nScrollDirectionRest = 0;
		KillTimer( __EXTTAB_SCROLL_TIMER_ID );
	}

//	if(		m_wndToolTip.GetSafeHwnd() != NULL
//		&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
//		)
//		m_wndToolTip.DelTool( this, 1 );
//	CWnd::CancelToolTips();

	OnTabWndMouseTrackingPushedStop( true );
	OnTabWndMouseTrackingHoverStop( true );
}

bool CExtTabWnd::OnTabWndMouseTrackingHoverStart(
	LONG nHitTest
	)
{
	ASSERT_VALID( this );

	if( ! CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND( GetSafeHwnd() ) )
		return false;

	m_nHoverTrackingHitTest = nHitTest;
	m_bTrackingButtonHover = true;
	Invalidate();
	UpdateWindow();
	CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );

	if(		OnTabWndToolTipQueryEnabled()
		&&	m_wndToolTip.GetSafeHwnd() != NULL
		)
	{
		CRect rcTipTrack( 0, 0, 0, 0 );
		CExtSafeString sTooltipText;
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		CExtLocalResourceHelper _LRH;
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		switch( nHitTest )
		{
		case __ETWH_BUTTON_LEFTUP:
			if( ! g_ResourceManager->LoadString( sTooltipText, IDS_EXTTABWND_BTN_LEFTUP ) )
			{
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
				ASSERT( FALSE );
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
				sTooltipText = _T("Previous");
			}
			rcTipTrack = m_rcBtnUp;
			break;
		case __ETWH_BUTTON_RIGHTDOWN:
			if( ! g_ResourceManager->LoadString( sTooltipText, IDS_EXTTABWND_BTN_RIGHTDOWN ) )
			{
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
				ASSERT( FALSE );
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
				sTooltipText = _T("Next");
			}
			rcTipTrack = m_rcBtnDown;
			break;
		case __ETWH_BUTTON_HELP:
			if( ! g_ResourceManager->LoadString( sTooltipText, IDS_EXTTABWND_BTN_HELP ) )
			{
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
				ASSERT( FALSE );
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
				sTooltipText = _T("Help");
			}
			rcTipTrack = m_rcBtnHelp;
			break;
		case __ETWH_BUTTON_CLOSE:
			if( ! g_ResourceManager->LoadString( sTooltipText, IDS_EXTTABWND_BTN_CLOSE ) )
			{
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
				ASSERT( FALSE );
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
				sTooltipText = _T("");
			}
			rcTipTrack = m_rcBtnClose;
			break;
		case __ETWH_BUTTON_TAB_LIST:
			if( ! g_ResourceManager->LoadString( sTooltipText, IDS_EXTTABWND_BTN_TAB_LIST ) )
			{
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
				ASSERT( FALSE );
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
				sTooltipText = _T("");
			}
			rcTipTrack = m_rcBtnTabList;
			break;
		} // switch( nHitTest )
		if( !sTooltipText.IsEmpty() )
			m_wndToolTip.AddTool(
				this,
				sTooltipText,
				&rcTipTrack,
				1
				);
		else
		{
			m_wndToolTip.DelTool( this, 1 );
			CWnd::CancelToolTips();
		}
	} // if( OnTabWndToolTipQueryEnabled() ...

	return true;
}

void CExtTabWnd::OnTabWndMouseTrackingHoverStop(
	bool bEnableReleaseCapture // = true
	)
{
	ASSERT_VALID( this );

	if( !m_bTrackingButtonHover )
		return;
	m_bTrackingButtonHover = false;
	m_nHoverTrackingHitTest = __ETWH_NOWHERE;
	Invalidate();
	UpdateWindow();
	if( bEnableReleaseCapture )
		CExtMouseCaptureSink::ReleaseCapture();
}


void CExtTabWnd::OnTimer(UINT nIDEvent) 
{
	switch( nIDEvent )
	{
	case __EXTTAB_SCROLL_TIMER_ID:
	{
		if( m_nScrollDirectionRest != 0 )
		{
			OnTabWndDoScroll(
				(m_nScrollDirectionRest < 0)
					? - ( min(__EXTTAB_SCROLL_STEP,(-m_nScrollDirectionRest)) )
					: + ( min(__EXTTAB_SCROLL_STEP, m_nScrollDirectionRest  ) )
				,
				false
				);
			if( m_nScrollDirectionRest < 0 )
			{
				//m_nScrollDirectionRest++;
				m_nScrollDirectionRest += __EXTTAB_SCROLL_STEP;
				if( m_nScrollDirectionRest > 0 )
					m_nScrollDirectionRest = 0;
			}
			else
			{
				//m_nScrollDirectionRest--;
				m_nScrollDirectionRest -= __EXTTAB_SCROLL_STEP;
				if( m_nScrollDirectionRest < 0 )
					m_nScrollDirectionRest = 0;
			}
		} // if( m_nScrollDirectionRest != 0 )

		if( m_nScrollDirectionRest == 0 )
			KillTimer( __EXTTAB_SCROLL_TIMER_ID );
	}
	break; // case __EXTTAB_SCROLL_TIMER_ID
	case __EXTTAB_SELECTION_DELAY_TIMER_ID:
	{
		KillTimer( __EXTTAB_SELECTION_DELAY_TIMER_ID );
		if( m_nSelIndex != m_nDelayedSelIndex
			&& m_nDelayedSelIndex >= 0
			&& m_nDelayedSelIndex < ItemGetCount()
			&& ItemGetVisibleCount() > 0
			&& ItemGet(m_nDelayedSelIndex)->VisibleGet()
			)
		{
			CPoint ptCursor( 0, 0 );
			if( ! ::GetCursorPos( &ptCursor ) )
				return;
			ScreenToClient( &ptCursor );
			LONG nHitTest = ItemHitTest( ptCursor );
			if( nHitTest == m_nDelayedSelIndex )
				SelectionSet( m_nDelayedSelIndex, true, true );
		} // if( m_nSelIndex != m_nDelayedSelIndex )
		m_nDelayedSelIndex = -1;
	}
	break; // case __EXTTAB_SELECTION_DELAY_TIMER_ID
	default:
	{
		CWnd::OnTimer(nIDEvent);
	}
	break;
	} // switch( nIDEvent )
}

LRESULT CExtTabWnd::OnSizeParent( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );

	if( !m_bReflectParentSizing )
		return (LRESULT)0;

	wParam;
AFX_SIZEPARENTPARAMS * lpLayout = (AFX_SIZEPARENTPARAMS*)lParam;
	ASSERT( lpLayout != NULL );

//DWORD dwWndStyle = GetStyle();
//	if( (dwWndStyle & WS_VISIBLE) == 0 )
//		return 0;

CRect rcFrameRest = &lpLayout->rect;
	if( rcFrameRest.left >= rcFrameRest.right
		|| rcFrameRest.top >= rcFrameRest.bottom
		)
	{
		if( lpLayout->hDWP == NULL )
			return 0;
		::SetWindowPos(
			m_hWnd,
			NULL, 0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOOWNERZORDER
				|SWP_HIDEWINDOW
			);
		return 0;
	}

	OnTabWndSyncVisibility();
DWORD dwWndStyle = GetStyle();
	if( (dwWndStyle & WS_VISIBLE) == 0 )
		return 0;
		
	m_rcRecalcLayout = rcFrameRest;
	m_bDelayRecalcLayout = true;
	_RecalcLayoutImpl();
	m_rcRecalcLayout.SetRect( 0, 0, 0, 0 );
	
	if( m_rcTabItemsArea.IsRectEmpty() )
		return 0;

CSize _sizeNeeded = m_rcTabItemsArea.Size();
	if( _sizeNeeded.cx <= 0 || _sizeNeeded.cy <= 0 )
		return 0;

CRect rcOwnLayout( rcFrameRest );

DWORD dwOrientation = OrientationGet();
	switch( dwOrientation )
	{
	case __ETWS_ORIENT_TOP:
	{
		ASSERT( _sizeNeeded.cy > 0 );
		_sizeNeeded.cy += __EXTTAB_IN_FRAME_GAP_TOP_DY+3;
		lpLayout->rect.top += _sizeNeeded.cy;
		rcOwnLayout.bottom = rcOwnLayout.top + _sizeNeeded.cy;
lpLayout->sizeTotal.cy += _sizeNeeded.cy;
	}
	break;
	case __ETWS_ORIENT_BOTTOM:
	{
		ASSERT( _sizeNeeded.cy > 0 );
		_sizeNeeded.cy += __EXTTAB_IN_FRAME_GAP_BOTTOM_DY;
		lpLayout->rect.bottom -= _sizeNeeded.cy;
		rcOwnLayout.top = rcOwnLayout.bottom - _sizeNeeded.cy;
lpLayout->sizeTotal.cy += _sizeNeeded.cy;
	}
	break;
	case __ETWS_ORIENT_LEFT:
	{
		ASSERT( _sizeNeeded.cx > 0 );
		_sizeNeeded.cx += __EXTTAB_IN_FRAME_GAP_LEFT_DX+3;
		lpLayout->rect.left += _sizeNeeded.cx;
		rcOwnLayout.right = rcOwnLayout.left + _sizeNeeded.cx;
lpLayout->sizeTotal.cx += _sizeNeeded.cx;
	}
	break;
	case __ETWS_ORIENT_RIGHT:
	{
		ASSERT( _sizeNeeded.cx > 0 );
		_sizeNeeded.cx += __EXTTAB_IN_FRAME_GAP_RIGHT_DX;
		lpLayout->rect.right -= _sizeNeeded.cx;
		rcOwnLayout.left = rcOwnLayout.right - _sizeNeeded.cx;
lpLayout->sizeTotal.cx += _sizeNeeded.cx;
	}
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( dwOrientation )

	ASSERT( ! rcOwnLayout.IsRectEmpty() );
	if( lpLayout->hDWP != NULL )
	{
		::AfxRepositionWindow(
			lpLayout,
			m_hWnd,
			&rcOwnLayout
			);
		::SetWindowPos(
			m_hWnd,
			NULL, 0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOOWNERZORDER
				|SWP_FRAMECHANGED
			);
		UpdateTabWnd( true );
	} // if( lpLayout->hDWP != NULL )
	return 0L;
}

bool CExtTabWnd::OnTabWndToolTipQueryEnabled() const
{
	return true;
}

#endif // (!defined __EXT_MFC_NO_TAB_CTRL )

#if (!defined __EXT_MFC_NO_TABMDI_CTRL )

/////////////////////////////////////////////////////////////////////////////
// CExtTabMdiWnd window

IMPLEMENT_DYNCREATE( CExtTabMdiWnd, CExtTabWnd );

CExtTabMdiWnd::CExtTabMdiWnd()
	: m_bInSyncLayout( false )
{
	m_bReflectQueryRepositionCalcEffect = true;
}

CExtTabMdiWnd::~CExtTabMdiWnd()
{
}

BEGIN_MESSAGE_MAP(CExtTabMdiWnd, CExtTabWnd)
	//{{AFX_MSG_MAP(CExtTabMdiWnd)
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


#ifdef _DEBUG

void CExtTabMdiWnd::AssertValid() const
{
	CExtTabWnd::AssertValid();
}

void CExtTabMdiWnd::Dump(CDumpContext& dc) const
{
	CExtTabWnd::Dump( dc );
}

#endif

BOOL CExtTabMdiWnd::Create(
	CWnd * pParentWnd,
	const RECT & rcWnd, // = CRect( 0, 0, 0, 0 )
	UINT nDlgCtrlID, // = UINT( IDC_STATIC )
	DWORD dwWindowStyle, // = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS
	DWORD dwTabStyle, // = __ETWS_ORIENT_TOP|__ETWS_ENABLED_BTN_HELP|__ETWS_ENABLED_BTN_CLOSE
	CCreateContext * pContext // = NULL
	)
{
	ASSERT( pParentWnd != NULL );
	ASSERT( pParentWnd->GetSafeHwnd() != NULL );
	ASSERT( ::IsWindow(pParentWnd->GetSafeHwnd()) );
	// FrameFeatures does not use CMDIFrameWnd
	// ASSERT_KINDOF( CMDIFrameWnd, pParentWnd );
	
	if(	!CExtTabWnd::Create(
			pParentWnd,
			rcWnd,
			nDlgCtrlID,
			dwWindowStyle,
			dwTabStyle,
			pContext
			)
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}

	// FrameFeatures does not use CMDIFrameWnd
HWND hWndMdiArea = _GetHwndMdiArea();
	//ASSERT( hWndMdiArea != NULL );
	if( hWndMdiArea == NULL )
	{
		//return FALSE;
		return TRUE;
	} // if( hWndMdiArea == NULL )

	if( IsHookedWindow( hWndMdiArea ) )
		return TRUE;

	if( !SetupHookWndSink( hWndMdiArea, false, true ) )
	{
		ASSERT( FALSE );
		return FALSE;
	}

	// setup mdi update timer
	SetTimer(
		__EXTTAB_MDI_UPDATE_TIMER_ID,
		__EXTTAB_MDI_UPDATE_TIMER_PERIOD,
		NULL
		);

	return TRUE;
}


HWND CExtTabMdiWnd::_GetHwndMainFrame()
{
	ASSERT_VALID( this );
CWnd * pParentWnd = GetParent();
	ASSERT( pParentWnd != NULL );
HWND hWndParent = pParentWnd->GetSafeHwnd();
	ASSERT( hWndParent != NULL );
	ASSERT( ::IsWindow(hWndParent) );
	// FrameFeatures does not use CMDIFrameWnd
	// ASSERT_KINDOF( CMDIFrameWnd, pParentWnd );
	return hWndParent;
}

HWND CExtTabMdiWnd::_GetHwndMdiArea()
{
	ASSERT_VALID( this );
HWND hWndMdiFrame = _GetHwndMainFrame();
	ASSERT( hWndMdiFrame != NULL );
	if( hWndMdiFrame == NULL )
		return NULL;
CFrameWnd * pFrame =
		STATIC_DOWNCAST(
			CFrameWnd,
			CWnd::FromHandlePermanent( hWndMdiFrame )
			);
	ASSERT_VALID( pFrame );
	ASSERT( ::IsWindow(pFrame->GetSafeHwnd()) );

CMDIFrameWnd * pMdiFrame =
		DYNAMIC_DOWNCAST(
			CMDIFrameWnd,
			pFrame
			);
	if( pMdiFrame == NULL )
	{
		// FrameFeatures does not use CMDIFrameWnd
		for(	HWND hWnd = ::GetWindow( pFrame->GetSafeHwnd(), GW_CHILD );
				hWnd != NULL;
				hWnd = ::GetWindow( hWnd, GW_HWNDNEXT )
			)
		{
			ASSERT( ::IsWindow( hWnd ) );
			TCHAR strClassName[ 512 ];
			::memset( strClassName, 0, sizeof(strClassName) );
			::GetClassName( hWnd, strClassName, 512 );
			if( _tcslen( strClassName ) == 0 )
				continue;
			_tcslwr( strClassName );
			//if( _tcscmp( strClassName, _T("mdiclient") ) == 0 )
			if( _tcsstr( strClassName, _T("mdiclient") ) != NULL )
				return hWnd;
		}
		return NULL;
	} // if( pMdiFrame == NULL )
	//ASSERT_VALID( pMdiFrame );

	ASSERT( pMdiFrame->m_hWndMDIClient != NULL );
	ASSERT( ::IsWindow( pMdiFrame->m_hWndMDIClient ) );
	return pMdiFrame->m_hWndMDIClient;
}

bool CExtTabMdiWnd::OnHookWndMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	UINT nMessage,
	WPARAM & wParam,
	LPARAM & lParam
	)
{
	__PROF_UIS_MANAGE_STATE;

	ASSERT_VALID( this );

	if( CWnd::FromHandlePermanent(GetSafeHwnd()) == NULL )
		return
			CExtHookSink::OnHookWndMsg(
				lResult,
				hWndHooked,
				nMessage,
				wParam,
				lParam
				);

	if( hWndHooked == _GetHwndMdiArea() )
	{
		bool bDelayUpdateMdiTabCtrl = false;
		
		switch( nMessage )
		{
		case WM_WINDOWPOSCHANGING:
		{
			LPWINDOWPOS lpWindowPos = 
				reinterpret_cast < LPWINDOWPOS > (lParam);
			ASSERT( lpWindowPos != NULL );
			lpWindowPos->flags |= SWP_FRAMECHANGED;
			break;
		}
		break;
		case WM_NCCALCSIZE:
		{
			lResult = 0;

			NCCALCSIZE_PARAMS * pNCCSP =
				reinterpret_cast < NCCALCSIZE_PARAMS * > ( lParam );
			ASSERT( pNCCSP != NULL );
			
			DWORD dwWndStyle = GetStyle();
			bool bVisibleTab =
				( (dwWndStyle & WS_VISIBLE) != 0 ) ? true : false;

			CRect rcMdiTabWnd( pNCCSP->rgrc[0] );
			DWORD dwOrientation = OrientationGet();
			rcMdiTabWnd.DeflateRect(
				(bVisibleTab && dwOrientation == __ETWS_ORIENT_LEFT) ? 0: __EXTTAB_MDI_NC_AREA_GAP_DX_L,
				(bVisibleTab && dwOrientation == __ETWS_ORIENT_TOP) ? 0: __EXTTAB_MDI_NC_AREA_GAP_DY_T,
				(bVisibleTab && dwOrientation == __ETWS_ORIENT_RIGHT) ? 0: __EXTTAB_MDI_NC_AREA_GAP_DX_R,
				(bVisibleTab && dwOrientation == __ETWS_ORIENT_BOTTOM) ? 0: __EXTTAB_MDI_NC_AREA_GAP_DY_B
				);
			::CopyRect( &(pNCCSP->rgrc[0]), rcMdiTabWnd );

			return true;
		}
		break;
		case WM_NCPAINT:
		{
			lResult = 0;

			CWnd * pWndMdiArea = CWnd::FromHandle( _GetHwndMdiArea() );

			CRect rcMdiAreaWnd, rcMdiAreaClient;
			pWndMdiArea->GetWindowRect( &rcMdiAreaWnd );
			pWndMdiArea->GetClientRect( &rcMdiAreaClient );
			pWndMdiArea->ClientToScreen( &rcMdiAreaClient );
			if( rcMdiAreaWnd == rcMdiAreaClient )
				return true;
			CPoint ptDevOffset = -rcMdiAreaWnd.TopLeft();
			rcMdiAreaWnd.OffsetRect( ptDevOffset );
			rcMdiAreaClient.OffsetRect( ptDevOffset );

			CWindowDC dc( pWndMdiArea );
			ASSERT( dc.GetSafeHdc() != NULL );
			dc.ExcludeClipRect( &rcMdiAreaClient );

			dc.FillSolidRect(
				&rcMdiAreaWnd,
				g_PaintManager->GetColor(COLOR_3DFACE)
				);

			DWORD dwWndStyle = GetStyle();
			bool bInvisibleTab =
				( (dwWndStyle & WS_VISIBLE) == 0 ) ? true : false;

			DWORD dwOrientation = OrientationGet();
			rcMdiAreaWnd.InflateRect(
				(bInvisibleTab || dwOrientation != __ETWS_ORIENT_LEFT) ? 0: __EXTTAB_MDI_NC_AREA_GAP_DX_L,
				(bInvisibleTab || dwOrientation != __ETWS_ORIENT_TOP) ? 0: __EXTTAB_MDI_NC_AREA_GAP_DY_T,
				(bInvisibleTab || dwOrientation != __ETWS_ORIENT_RIGHT) ? 0: __EXTTAB_MDI_NC_AREA_GAP_DX_R,
				(bInvisibleTab || dwOrientation != __ETWS_ORIENT_BOTTOM) ? 0: __EXTTAB_MDI_NC_AREA_GAP_DY_B
				);
			//rcMdiAreaWnd.DeflateRect( 1, 1 );
			
			g_PaintManager->PaintTabNcAreaRect(
				dc,
				rcMdiAreaWnd,
				this
				);
			
			return true;
		}
		case WM_MDIDESTROY:
		{
			HWND hWndMdiChild = (HWND)wParam;
			if( hWndMdiChild != NULL )
			{
				LONG nIndex = ItemFindByHWND( hWndMdiChild, -1, true, true );
				if( nIndex >= 0 )
				{
					m_bInSyncLayout = true;
					ItemRemove( nIndex, 1, true );
					m_bInSyncLayout = false;
				}
			}
			bDelayUpdateMdiTabCtrl = true;
		} // if( nMessage == WM_MDIDESTROY )
		break;
		case WM_MDIACTIVATE:
		{
			HWND hWndMdiChild = (HWND)wParam;
			if( hWndMdiChild != NULL )
			{
				LONG nIndex = ItemFindByHWND( hWndMdiChild, -1, true, true );
				if( nIndex >= 0 )
				{
					m_bInSyncLayout = true;
					SelectionSet( nIndex, true, true );
					m_bInSyncLayout = false;
				}
			}
			bDelayUpdateMdiTabCtrl = true;
		} // else if( nMessage == WM_MDIACTIVATE )
		break;
		case WM_MDIREFRESHMENU:
		case WM_MDICREATE:
			bDelayUpdateMdiTabCtrl = true;
		break;
		} // switch( nMessage )
		

		if( bDelayUpdateMdiTabCtrl )
		{
			// setup mdi update timer
//			SetTimer(
//				__EXTTAB_MDI_UPDATE_TIMER_ID,
//				__EXTTAB_MDI_UPDATE_TIMER_PERIOD,
//				NULL
//				);
			PostMessage(
				WM_TIMER,
				__EXTTAB_MDI_UPDATE_TIMER_ID,
				0
				);
		}
	} // if( hWndHooked == _GetHwndMdiArea() )

	return
		CExtHookSink::OnHookWndMsg(
			lResult,
			hWndHooked,
			nMessage,
			wParam,
			lParam
			);
}

/////////////////////////////////////////////////////////////////////////////
// CExtTabMdiWnd message handlers

void CExtTabMdiWnd::OnTimer(UINT nIDEvent) 
{
	switch( nIDEvent )
	{
	case __EXTTAB_MDI_UPDATE_TIMER_ID:
	{
		KillTimer( __EXTTAB_MDI_UPDATE_TIMER_ID );
		if( m_bInSyncLayout )
		{
//			PostMessage(
//				WM_TIMER,
//				__EXTTAB_MDI_UPDATE_TIMER_ID,
//				0
//				);
			break;
		}
		_SyncAllItems();
	}
	break; // case __EXTTAB_MDI_CREATE_TIMER_ID
	default:
	{
		CExtTabWnd::OnTimer(nIDEvent);
	}
	break;
	} // switch( nIDEvent )
}

bool CExtTabMdiWnd::OnTabWndClickedButton(
	LONG nHitTest,
	bool bButtonPressed,
	INT nMouseButton, // MK_... values
	UINT nMouseEventFlags
	)
{
	if( nMouseButton != MK_LBUTTON )
		return false;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
		return false;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

bool bRetVal =
		CExtTabWnd::OnTabWndClickedButton(
			nHitTest,
			bButtonPressed,
			nMouseButton, // MK_... values
			nMouseEventFlags
			);

	if( !bButtonPressed
		&& (nHitTest == __ETWH_BUTTON_CLOSE || nHitTest == __ETWH_BUTTON_HELP)
		&& SelectionGet() >= 0
		)
	{ // if "X" or "?" pressed
		TAB_ITEM_INFO * pTii = SelectionGetPtr();
		ASSERT_VALID( pTii );
		HWND hWndMdiChild = (HWND)pTii->LParamGet();	
		if( hWndMdiChild != NULL && ::IsWindow(hWndMdiChild) )
		{
			PostMessage( WM_CANCELMODE );
			if( nHitTest == __ETWH_BUTTON_CLOSE )
			{ // if "X" pressed
				::PostMessage(
					hWndMdiChild,
					WM_CLOSE,
					0,
					0
					);
			} // if "X" pressed
			else
			{ // if "?" pressed
				CPoint ptCursor( 0, 0 );
				::GetCursorPos( &ptCursor );
				VERIFY( ::ClientToScreen( hWndMdiChild, &ptCursor ) );
				::PostMessage(
					hWndMdiChild,
					WM_SYSCOMMAND,
					SC_CONTEXTHELP,
					MAKELONG( ptCursor.x, ptCursor.y )
					);
			} // if "?" pressed
		} // if( hWndMdiChild != NULL && ::IsWindow(hWndMdiChild) )
	} // if "X" or "?" pressed

	return bRetVal;
}

bool CExtTabMdiWnd::OnTabWndClickedItem(
	LONG nItemIndex,
	bool bButtonPressed,
	INT nMouseButton, // MK_... values
	UINT nMouseEventFlags
	)
{
	if( nMouseButton != MK_LBUTTON )
		return false;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
		return false;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	return
		CExtTabWnd::OnTabWndClickedItem(
			nItemIndex,
			bButtonPressed,
			nMouseButton, // MK_... values
			nMouseEventFlags
			);
}

bool CExtTabMdiWnd::OnTabWndSelectionChange(
	LONG nOldItemIndex,
	LONG nNewItemIndex,
	bool bPreSelectionTest
	)
{
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
		return false;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

bool bRetVal =
		CExtTabWnd::OnTabWndSelectionChange(
			nOldItemIndex,
			nNewItemIndex,
			bPreSelectionTest
			);
	if( (!m_bInSyncLayout) && (!bPreSelectionTest) )
	{
		ASSERT( bRetVal );
		HWND hWnd = (HWND)ItemLParamGet( nNewItemIndex );
		if( hWnd != NULL && ::IsWindow(hWnd) )
		{
			// FrameFeatures does not use CMDIFrameWnd
//			CMDIFrameWnd * pMdiFrame =
//				STATIC_DOWNCAST(
//					CMDIFrameWnd,
//					CWnd::FromHandlePermanent( _GetHwndMainFrame() )
//					);
//			pMdiFrame->MDIActivate(
//				CWnd::FromHandle( hWnd )
//				);
			HWND hWndMdiArea = _GetHwndMdiArea();
			if( hWndMdiArea != NULL )
				::SendMessage(
					hWndMdiArea,
					WM_MDIACTIVATE,
					(WPARAM)hWnd,
					0
					);
		} // if( hWnd != NULL && ::IsWindow(hWnd) )
	} // if( (!m_bInSyncLayout) && (!bPreSelectionTest) )
	return bRetVal;
}

bool CExtTabMdiWnd::OnTabWndMouseTrackingPushedStart(
	INT nMouseButton, // MK_... values
	LONG nHitTest
	)
{
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
		return false;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	return CExtTabWnd::OnTabWndMouseTrackingPushedStart( nMouseButton, nHitTest );
}

bool CExtTabMdiWnd::OnTabWndMouseTrackingHoverStart(
	LONG nHitTest
	)
{
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
		return false;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	return CExtTabWnd::OnTabWndMouseTrackingHoverStart( nHitTest );
}

CExtCmdIcon * CExtTabMdiWnd::OnTabWndQueryItemIcon(
	const TAB_ITEM_INFO * pTii
	) const
{
	ASSERT_VALID( pTii );
HWND hWndMdiChild = (HWND)pTii->LParamGet();	
	if(		hWndMdiChild == NULL
		||	(! ::IsWindow( hWndMdiChild ) )
		)
		return NULL;
HICON hIcon = (HICON)
		::SendMessage(
			hWndMdiChild,
			WM_GETICON,
			WPARAM(ICON_SMALL),
			0
			);
	if( hIcon == NULL )
	{
		hIcon = (HICON)
			::SendMessage(
				hWndMdiChild,
				WM_GETICON,
				WPARAM(ICON_BIG),
				0
				);
		if( hIcon == NULL )
		{
			hIcon = (HICON)
				::GetClassLong(
					hWndMdiChild,
					GCL_HICONSM
					);
			if( hIcon == NULL )
			{
				hIcon = (HICON)
					::GetClassLong(
						hWndMdiChild,
						GCL_HICON
						);
				if( hIcon == NULL )
					return NULL;
			} // if( hIcon == NULL )
		} // if( hIcon == NULL )
	} // if( hIcon == NULL )
	ASSERT( hIcon != NULL );
	m_iconMdiChildCache.Empty();
CExtCmdIcon _iconBig( hIcon, false );
	VERIFY(
		m_iconMdiChildCache.
			CreateScaledCopy(
				_iconBig,
				CSize( 16, 16 )
				)
		);
	_iconBig.DetachAsHICON();
	if( ! m_iconMdiChildCache.IsEmpty() )
		return (&m_iconMdiChildCache);
	return NULL;
}

__EXT_MFC_SAFE_LPCTSTR CExtTabMdiWnd::OnTabWndQueryItemText(
	const TAB_ITEM_INFO * pTii
	) const
{
	ASSERT_VALID( pTii );
HWND hWndMdiChild = (HWND)pTii->LParamGet();	
	if( hWndMdiChild != NULL
		&& ::IsWindow(hWndMdiChild)
		)
	{
		CWnd * pWnd = CWnd::FromHandle( hWndMdiChild );
		ASSERT( pWnd != NULL );
		static CString sText( _T("") );

// (- 2.24)
//		if( pWnd->IsKindOf( RUNTIME_CLASS(CMDIChildWnd) ) )
//		{
//			CDocument * pActiveDoc =((CMDIChildWnd *)pWnd)->GetActiveDocument();
//			if( pActiveDoc != NULL )
//				sText = pActiveDoc->GetTitle();
//		}
//		else if( pWnd->IsKindOf( RUNTIME_CLASS(CFrameWnd) ) )
//			sText = ((CFrameWnd *)pWnd)->GetTitle();
// (+ 2.24)
		if( pWnd->IsKindOf( RUNTIME_CLASS(CFrameWnd) ) )
		{
			CDocument * pActiveDoc =
				((CFrameWnd*)pWnd)->GetActiveDocument();
			if( pActiveDoc != NULL )
				sText = pActiveDoc->GetTitle();
			if( sText.IsEmpty() )
				sText = ((CFrameWnd *)pWnd)->GetTitle();
			if( ! sText.IsEmpty() )
			{
				int nWindow = ((CFrameWnd*)pWnd)->m_nWindow;
				if( nWindow > 0 )
				{
					sText += _T(":");
					CString sNo;
					sNo.Format( _T("%d"), nWindow );
					sText += sNo;
				} // if( nWindow > 0 )
			} // if( ! sText.IsEmpty() )
		} // if( pWnd->IsKindOf( RUNTIME_CLASS(CFrameWnd) ) )
		else
			sText = _T("");

		if( sText.IsEmpty() )
			pWnd->GetWindowText( sText );
		
		return (sText.IsEmpty()) ? _T("") : ((LPCTSTR)sText);
		
	}
	return NULL;
}

void CExtTabMdiWnd::OnTabWndItemInsert(
	LONG nItemIndex,
	TAB_ITEM_INFO * pTii
	)
{
	CExtTabWnd::OnTabWndItemInsert(
		nItemIndex,
		pTii
		);

	if( m_bInSyncLayout )
		return;

bool bDoRecalcLayout =
		(ItemGetCount() == 1)
			? true
			: false
			;
	if( bDoRecalcLayout )
		_RecalcFrameLayout();
}

void CExtTabMdiWnd::OnTabWndRemoveItem(
	LONG nItemIndex,
	LONG nCount,
	bool bPreRemove
	)
{
	CExtTabWnd::OnTabWndRemoveItem(
		nItemIndex,
		nCount,
		bPreRemove
		);

	if( !bPreRemove || m_bInSyncLayout )
		return;

bool bDoRecalcLayout =
		(ItemGetCount() == 0)
			? true
			: false
			;
	if( bDoRecalcLayout )
		_RecalcFrameLayout();
}

void CExtTabMdiWnd::OnTabWndRemoveAllItems(
	bool bPreRemove
	)
{
	CExtTabWnd::OnTabWndRemoveAllItems(
		bPreRemove
		);

	if( !bPreRemove || m_bInSyncLayout )
		return;

	_RecalcFrameLayout();
}

void CExtTabMdiWnd::OnTabWndSyncVisibility()
{
LONG nItemCount = ItemGetCount();
DWORD dwWndStyle = GetStyle();
	if( nItemCount > 0
		&&	(!(		CExtControlBar::FindPrintPreviewMode(
						STATIC_DOWNCAST( CFrameWnd, GetParent() )
						)
				||	CExtControlBar::IsOleIpObjActive(
						STATIC_DOWNCAST( CFrameWnd, GetParent() )
						)
			) )
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
			HWND hWndMdiArea = _GetHwndMdiArea();
			if( hWndMdiArea != NULL )
				::SetWindowPos(
					hWndMdiArea,
					NULL, 0, 0, 0, 0,
					SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOOWNERZORDER
						|SWP_FRAMECHANGED
					);
		}
	} // if( nItemCount > 0 ...
	else
	{
		if( (dwWndStyle & WS_VISIBLE) != 0 )
			::SetWindowPos(
				m_hWnd,
				NULL, 0, 0, 0, 0,
				SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOOWNERZORDER
					|SWP_HIDEWINDOW
				);
	} // else from if( nItemCount > 0 ...

	dwWndStyle = GetStyle();
	if( (dwWndStyle & WS_VISIBLE) == 0 )
		m_nIndexVisFirst = m_nIndexVisLast = -1;
}

void CExtTabMdiWnd::_RecalcFrameLayout()
{
bool bWasVisible = (GetStyle() & WS_VISIBLE) ? true : false;
	OnTabWndSyncVisibility();
bool bNowVisible = (GetStyle() & WS_VISIBLE) ? true : false;
	if( bWasVisible != bNowVisible )
	{
		// FrameFeatures does not use CMDIFrameWnd
		CFrameWnd * pFrame =
			STATIC_DOWNCAST(
				CFrameWnd,
				CWnd::FromHandlePermanent( _GetHwndMainFrame() )
				);
		pFrame->DelayRecalcLayout();
	} // if( bWasVisible != bNowVisible )
	UpdateTabWnd( true );
}

void CExtTabMdiWnd::_SyncAllItems()
{
	if( m_bInSyncLayout )
		return;

	m_bInSyncLayout = true;

	// FrameFeatures does not use CMDIFrameWnd
//CMDIFrameWnd * pMdiFrame =
//		STATIC_DOWNCAST(
//			CMDIFrameWnd,
//			CWnd::FromHandlePermanent( _GetHwndMainFrame() )
//			);
//CWnd * pWndActiveMdiChild = pMdiFrame->MDIGetActive(NULL);
CWnd * pWndActiveMdiChild = NULL;
HWND hWndMainFrame = _GetHwndMainFrame();
HWND hWndMdiArea = _GetHwndMdiArea();
	if(		hWndMainFrame == NULL
		||	hWndMdiArea == NULL
		)
	{
		m_bInSyncLayout = false;
		return;
	}
CMDIFrameWnd * pMdiFrame =
		DYNAMIC_DOWNCAST(
			CMDIFrameWnd,
			CWnd::FromHandlePermanent(hWndMainFrame)
			);
	if( pMdiFrame != NULL )
	{
		pWndActiveMdiChild = pMdiFrame->MDIGetActive(NULL);
	} // if( pMdiFrame != NULL )
	else
	{
		HWND hWnd = (HWND)
			::SendMessage(
				hWndMdiArea,
				WM_MDIGETACTIVE,
				0,
				0
				);
		if( hWnd == NULL )
		{
			m_bInSyncLayout = false;
			return;
		}
		pWndActiveMdiChild = CWnd::FromHandle( hWnd );
	} // else from if( pMdiFrame != NULL )
	if( pWndActiveMdiChild == NULL )
	{
		m_bInSyncLayout = false;
		return;
	}

HWND hWndMdiChild = NULL;
	if( pWndActiveMdiChild != NULL )
		hWndMdiChild = pWndActiveMdiChild->GetWindow( GW_HWNDFIRST )->GetSafeHwnd();
	for( ; hWndMdiChild != NULL; hWndMdiChild = ::GetWindow( hWndMdiChild, GW_HWNDNEXT ) )
	{
		LONG nIndexExist = ItemFindByHWND( hWndMdiChild, -1, true, true );
		if( nIndexExist >= 0 )
		{
			if( hWndMdiChild == pWndActiveMdiChild->GetSafeHwnd() )
				SelectionSet( nIndexExist, true, false );
			continue;
		}

		ItemInsert(
			NULL,
			NULL, false,
			0,
			-1,
			LPARAM( hWndMdiChild ),
			false
			);
		if(  hWndMdiChild == pWndActiveMdiChild->GetSafeHwnd()
			)
			SelectionSet( ItemGetCount()-1, true, false );

    } // for( ; hWndMdiChild != NULL; hWndMdiChild = ::GetWindow( hWndMdiChild, GW_HWNDNEXT ) )

	for( INT nIndex = 0; nIndex < ItemGetCount(); )
	{
		TAB_ITEM_INFO * pTii = ItemGet( nIndex );
		ASSERT_VALID( pTii );
		HWND hWnd = (HWND)pTii->LParamGet();
		ASSERT( hWnd );
		if( ! ::IsWindow(hWnd) )
			ItemRemove( nIndex );
		else
			nIndex++;
	}

	m_bInSyncLayout = false;

	_RecalcFrameLayout();
}

LRESULT CExtTabMdiWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch( message )
	{
	case WM_WINDOWPOSCHANGING:
	{
		LPWINDOWPOS lpWindowPos = 
			reinterpret_cast < LPWINDOWPOS > (lParam);
		ASSERT( lpWindowPos != NULL );
		lpWindowPos->flags |= SWP_FRAMECHANGED;
		break;
	}
	break;
	case WM_NCCALCSIZE:
	{
		NCCALCSIZE_PARAMS * pNCCSP =
			reinterpret_cast < NCCALCSIZE_PARAMS * > ( lParam );
		ASSERT( pNCCSP != NULL );
		
		DWORD dwWndStyle = GetStyle();
		if( (dwWndStyle & WS_VISIBLE) != 0 )
		{
			CRect rcMdiTabWnd( pNCCSP->rgrc[0] );
			DWORD dwOrientation = OrientationGet();
			rcMdiTabWnd.DeflateRect(
				(dwOrientation == __ETWS_ORIENT_RIGHT) ? 0: __EXTTAB_MDI_NC_AREA_GAP_DX_L,
				(dwOrientation == __ETWS_ORIENT_BOTTOM) ? 0: __EXTTAB_MDI_NC_AREA_GAP_DY_T,
				(dwOrientation == __ETWS_ORIENT_LEFT) ? 0: __EXTTAB_MDI_NC_AREA_GAP_DX_R,
				(dwOrientation == __ETWS_ORIENT_TOP) ? 0: __EXTTAB_MDI_NC_AREA_GAP_DY_B
				);
			::CopyRect( &(pNCCSP->rgrc[0]), rcMdiTabWnd );
		}
		
		return 0;
	}
	break;
	case WM_NCPAINT:
	{
		CRect rcMdiTabWnd, rcMdiTabClient;
		GetWindowRect( &rcMdiTabWnd );
		GetClientRect( &rcMdiTabClient );
		ClientToScreen( &rcMdiTabClient );
		if( rcMdiTabWnd == rcMdiTabClient )
			return 0;
		CPoint ptDevOffset = -rcMdiTabWnd.TopLeft();
		rcMdiTabWnd.OffsetRect( ptDevOffset );
		rcMdiTabClient.OffsetRect( ptDevOffset );

		CWindowDC dc( this );
		ASSERT( dc.GetSafeHdc() != NULL );
		dc.ExcludeClipRect( &rcMdiTabClient );

		dc.FillSolidRect(
			&rcMdiTabWnd,
			g_PaintManager->GetColor( COLOR_3DFACE )
			);

		DWORD dwOrientation = OrientationGet();
		rcMdiTabWnd.InflateRect(
			(dwOrientation != __ETWS_ORIENT_RIGHT) ? 0: __EXTTAB_MDI_NC_AREA_GAP_DX_L,
			(dwOrientation != __ETWS_ORIENT_BOTTOM) ? 0: __EXTTAB_MDI_NC_AREA_GAP_DY_T,
			(dwOrientation != __ETWS_ORIENT_LEFT) ? 0: __EXTTAB_MDI_NC_AREA_GAP_DX_R,
			(dwOrientation != __ETWS_ORIENT_TOP) ? 0: __EXTTAB_MDI_NC_AREA_GAP_DY_B
			);
		//rcMdiTabWnd.DeflateRect( 1, 1 );

		g_PaintManager->PaintTabNcAreaRect(
			dc,
			rcMdiTabWnd,
			this
			);

		return 0;
	}
	break;
	} // switch( message )
	
	return CExtTabWnd::WindowProc(message, wParam, lParam);
}

void CExtTabMdiWnd::OnContextMenu(CWnd* pWnd, CPoint point)
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

HWND hWndTrack = _GetHwndMainFrame();
	ASSERT( hWndTrack != NULL && ::IsWindow(hWndTrack) );

CExtPopupMenuWnd * pPopup = new CExtPopupMenuWnd;
	VERIFY( pPopup->CreatePopupMenu(hWndTrack) );

CExtControlBar::POPUP_MENU_EVENT_DATA _pmed(
		CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_MDITABS_CTX,
		pPopup,
		this,
		point,
		WM_CONTEXTMENU,
		NULL
		);

	if( !_pmed.NotifyTarget( false ) )
	{
		delete pPopup;
		return;
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

#endif // (!defined __EXT_MFC_NO_TABMDI_CTRL )


