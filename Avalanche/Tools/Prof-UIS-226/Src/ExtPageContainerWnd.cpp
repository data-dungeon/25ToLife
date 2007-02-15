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

#if (!defined __EXT_MFC_NO_PAGECONTAINER_CTRL)

#if (!defined __EXT_PAGECONTAINERWND_H)
	#include <ExtPageContainerWnd.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#if (!defined __EXT_RESIZABLE_DIALOG_H)
	#include <ExtResizableDialog.h>
#endif


#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtPageContainerWnd::PAGE_ITEM_INFO

CExtPageContainerWnd::PAGE_ITEM_INFO::PAGE_ITEM_INFO(
	HWND hWndPage,
	__EXT_MFC_SAFE_LPCTSTR sText, // = NULL
	LPARAM lParam, // = 0L
	bool bExpanded // = false
	)
	: m_hWndPage( hWndPage )
	, m_sText( (sText == NULL) ? _T("") : sText )
	, m_lParam( lParam )
	, m_bExpanded( bExpanded )
	, m_sizeCaptionLastMeasured( 0, 0 )
	, m_sizeTextLastMeasured( 0, 0 )
	, m_rcCaption( 0, 0, 0, 0 )
	, m_rcPage( 0, 0, 0, 0 )
	, m_bCaptionHover( false )
	, m_bCaptionPressed( false )
{
	ASSERT( m_hWndPage != NULL );
	ASSERT( ::IsWindow( m_hWndPage ) );
	
	::GetWindowRect( m_hWndPage, &m_rcWndInitial );

CExtPageContainerWnd * pWndPageContainer =
		GetPageContainerWnd();
	ASSERT_VALID( pWndPageContainer );
	pWndPageContainer->ScreenToClient(
		&m_rcWndInitial
		);
	m_rcWnd = m_rcWndInitial;

	m_bVisible =
		(
			( (DWORD)GetWindowLong(m_hWndPage, GWL_STYLE) )
			& WS_VISIBLE
		)
			? true : false;
}

CExtPageContainerWnd::PAGE_ITEM_INFO::~PAGE_ITEM_INFO()
{
}

CExtPageContainerWnd *
	CExtPageContainerWnd::PAGE_ITEM_INFO::GetPageContainerWnd()
{
	ASSERT( m_hWndPage != NULL );
	if( ! ::IsWindow( m_hWndPage ) )
	{
		ASSERT( FALSE );
		return NULL;
	}

HWND hWndPageContainer =
		::GetParent( m_hWndPage );
	if( hWndPageContainer == NULL
		|| ( ! ::IsWindow( hWndPageContainer ) )
		)
	{
		ASSERT( FALSE );
		return NULL;
	}

CWnd * pWndTestDowncast =
		CWnd::FromHandlePermanent( hWndPageContainer );
	ASSERT_VALID( pWndTestDowncast );
CExtPageContainerWnd * pWndPageContainer =
		DYNAMIC_DOWNCAST(
			CExtPageContainerWnd,
			pWndTestDowncast
			);
	ASSERT( pWndPageContainer != NULL );
	return pWndPageContainer;
}

HICON CExtPageContainerWnd::PAGE_ITEM_INFO::IconGet(
	CSize * pSize // = NULL
	)
{
CExtPageContainerWnd * pWndPageContainer =
		GetPageContainerWnd();
	ASSERT_VALID( pWndPageContainer );
CExtCmdIcon * pIcon =
		pWndPageContainer->OnPageContainerQueryItemIcon( this, pSize );
	if( pIcon == NULL || pIcon->IsEmpty() )
		return NULL;
	return pIcon->GetIcon();
}

void CExtPageContainerWnd::PAGE_ITEM_INFO::IconSet( HICON hIcon, bool bCopyIcon )
{
	m_icon.AssignFromHICON( hIcon, bCopyIcon );
CExtPageContainerWnd * pWndPageContainer =
		GetPageContainerWnd();
	ASSERT_VALID( pWndPageContainer );
	pWndPageContainer->UpdatePageContainerWnd( false );
}

__EXT_MFC_SAFE_LPCTSTR CExtPageContainerWnd::PAGE_ITEM_INFO::TextGet() const
{
CExtPageContainerWnd * pWndPageContainer =
		GetPageContainerWnd();
	ASSERT_VALID( pWndPageContainer );
__EXT_MFC_SAFE_LPCTSTR sItemText =
		pWndPageContainer->OnPageContainerQueryItemText( this );
	return sItemText;
}

void CExtPageContainerWnd::PAGE_ITEM_INFO::TextSet( __EXT_MFC_SAFE_LPCTSTR sText )
{
CExtPageContainerWnd * pWndPageContainer =
		GetPageContainerWnd();
	ASSERT_VALID( pWndPageContainer );
	pWndPageContainer->OnPageContainerDoItemTextChanging(
		this,
		m_sText,
		sText
		);
}

CFont * CExtPageContainerWnd::PAGE_ITEM_INFO::GetCaptionFont(
	bool bHorzLayout
	)
{
CExtPageContainerWnd * pWndPageContainer =
		GetPageContainerWnd();
	ASSERT_VALID( pWndPageContainer );
	return
		pWndPageContainer->OnPageContainerQueryItemFont(
			this,
			bHorzLayout
			);
}

CSize CExtPageContainerWnd::PAGE_ITEM_INFO::MeasureCaption(
	CDC & dcCalcLayout,
	bool bHorzLayout,
	PAGE_ITEM_INFO * pPageInfoPrev
	)
{
CExtPageContainerWnd * pWndPageContainer =
		GetPageContainerWnd();
	ASSERT_VALID( pWndPageContainer );
	m_sizeCaptionLastMeasured.cx
		= m_sizeCaptionLastMeasured.cy
		= m_sizeTextLastMeasured.cx
		= m_sizeTextLastMeasured.cy
		= 0;
	pWndPageContainer->OnPageContainerMeasureCaptionSize(
		dcCalcLayout,
		bHorzLayout,
		this,
		pPageInfoPrev,
		m_sizeCaptionLastMeasured,
		m_sizeTextLastMeasured
		);
	return m_sizeCaptionLastMeasured;
}

/////////////////////////////////////////////////////////////////////////////
// CExtPageContainerWnd::CItemTextEditWnd

// CEdit syntax of Create() method
BOOL CExtPageContainerWnd::CItemTextEditWnd::Create(
	DWORD dwStyle,
	const RECT & rect,
	CWnd * pParentWnd,
	UINT nID
	)
{
	ASSERT( m_pPageInfo != NULL );
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

void CExtPageContainerWnd::CItemTextEditWnd::PostNcDestroy()
{
	delete this;
}

void CExtPageContainerWnd::CItemTextEditWnd::DoEndEdit( bool bUpdateItemText )
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT( m_pPageInfo != NULL );

	if( m_bInEndEdit )
		return;
	m_bInEndEdit = true;

CExtPageContainerWnd * pWndParent =
		STATIC_DOWNCAST(
			CExtPageContainerWnd,
			GetParent()
			);
	ASSERT( pWndParent != NULL );
HWND hWndParent = pWndParent->GetSafeHwnd();
	ASSERT( hWndParent != NULL );
	if( pWndParent->OnPageContainerDeactivateEditor(
			m_pPageInfo,
			bUpdateItemText
			)
		&& bUpdateItemText
		)
	{
		CString sWindowText;
		GetWindowText( sWindowText );
		m_pPageInfo->TextSet( (LPCTSTR)sWindowText );
	}
	if( ::IsWindow(hWndParent) )
		::SetFocus( hWndParent );

	DestroyWindow();
}

LRESULT CExtPageContainerWnd::CItemTextEditWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID( this );
	ASSERT( m_pPageInfo != NULL );

	switch( message )
	{
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

//			CWnd * pWndParent = GetParent();
//			ASSERT( pWndParent != NULL );
//			pWndParent->SetFocus();
//			return 0;
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
// CExtPageContainerWnd

IMPLEMENT_DYNCREATE( CExtPageContainerWnd, CWnd );

bool CExtPageContainerWnd::g_bAnimationAccelerated = true;
LONG CExtPageContainerWnd::g_nAnimationStepCount = 4;
LONG CExtPageContainerWnd::g_ctAnimationStepClocks = 15;

HCURSOR CExtPageContainerWnd::g_hCursorDragScrollOnH = NULL;
HCURSOR CExtPageContainerWnd::g_hCursorDragScrollOnV = NULL;
HCURSOR CExtPageContainerWnd::g_hCursorDragScrollOff = NULL;
HCURSOR CExtPageContainerWnd::g_hCursorCaption = NULL;

UINT CExtPageContainerWnd::g_nMsgQueryCancelResetDragCursor =
	::RegisterWindowMessage(
		_T("CExtPageContainerWnd::g_nMsgQueryCancelResetDragCursor")
		);

CExtPageContainerWnd::CExtPageContainerWnd()
	: m_dwPageContainerStyle( __EPCWS_STYLES_DEFAULT )
	, m_bDirectCreateCall( false )
	, m_bDelayRecalcLayout( false )
	, m_bInLayoutRecalcProc( false )
	, m_bCancelingActionsMode( false )
	, m_bDragScrolling( false )
	, m_bInExpandingAnimationSF( false )
	, m_nScrollPos( 0L )
	, m_nScrollSize( 0L )
	, m_nScrollDragStart( 0L )
	, m_rcPageBorderSizes( 0, 0, 0, 0 )
	, m_sizeBorderBeforeAfterSpaces( 0, 0 )
	, m_nSpaceCaption2Wnd( 0L )
	, m_rcScrollPosArea( 0, 0, 0, 0 )
	, m_pPageInfoHover( NULL )
	, m_pPageInfoPressed( NULL )
	, m_pPageInfoEnsureVisible( NULL )
	, m_ptStartLeftBtnTrack( -1, -1 )
	, m_hWndEditor( NULL )
{
	VERIFY( RegisterPageContainerWndClass() );

	if( g_hCursorDragScrollOnH == NULL )
	{
		g_hCursorDragScrollOnH =
			::LoadCursor( NULL, IDC_SIZEWE );
		ASSERT( g_hCursorDragScrollOnH != NULL );
	}
	if( g_hCursorDragScrollOnV == NULL )
	{
		g_hCursorDragScrollOnV =
			::LoadCursor( NULL, IDC_SIZENS );
		ASSERT( g_hCursorDragScrollOnV != NULL );
	}
	if( g_hCursorDragScrollOff == NULL )
	{
		g_hCursorDragScrollOff =
			::LoadCursor( NULL, IDC_SIZEALL );
		ASSERT( g_hCursorDragScrollOff != NULL );
	}
	if( g_hCursorCaption == NULL )
	{
		__EXT_MFC_SAFE_LPCTSTR lpctstr_IDC_HAND = MAKEINTRESOURCE(32649);
#if (defined IDC_HAND)
		ASSERT( (IDC_HAND) == lpctstr_IDC_HAND );
#endif
		g_hCursorCaption = ::LoadCursor( NULL, lpctstr_IDC_HAND );
	}
}

CExtPageContainerWnd::~CExtPageContainerWnd()
{
}


BEGIN_MESSAGE_MAP(CExtPageContainerWnd, CWnd)
	//{{AFX_MSG_MAP(CExtPageContainerWnd)
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CANCELMODE()
	ON_WM_CAPTURECHANGED()
	ON_WM_SHOWWINDOW()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_KILLFOCUS()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATEAPP()
	ON_REGISTERED_MESSAGE(
		CExtContentExpandWnd::g_nMsgPaintItemContent,
		_OnPaintExpanedItemContent
		)
END_MESSAGE_MAP()


BOOL CExtPageContainerWnd::Create(
	CWnd * pWndParent,
	UINT nDlgCtrlID, // = (UINT)IDC_STATIC
	CRect rect, // = CRect( 0, 0, 0, 0 )
	DWORD dwPageContainerStyle, // = __EPCWS_STYLES_DEFAULT
	DWORD dwWindowStyle, // = WS_CHILD|WS_VISIBLE
	CCreateContext * pContext // = NULL
	)
{
	if( !RegisterPageContainerWndClass() )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	m_bDirectCreateCall = true;
	m_dwPageContainerStyle = dwPageContainerStyle;

	if( ! CWnd::Create(
			__EXT_PAGECONTAINERWND_CLASS_NAME,
			NULL,
			dwWindowStyle,
			rect,
			pWndParent,
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

bool CExtPageContainerWnd::_CreateHelper()
{
	EnableToolTips( TRUE );

	if( !m_wndToolTip.Create(this) )
	{
		ASSERT( FALSE );
		return false;
	}
	m_wndToolTip.Activate( TRUE );

	UpdatePageContainerWnd( false );
	return true;
}

bool CExtPageContainerWnd::g_bPageContainerWndClassRegistered = false;

bool CExtPageContainerWnd::RegisterPageContainerWndClass()
{
	if( g_bPageContainerWndClassRegistered )
		return true;

WNDCLASS wndclass;
HINSTANCE hInst = AfxGetInstanceHandle();
	if( ! ::GetClassInfo(
			hInst,
			__EXT_PAGECONTAINERWND_CLASS_NAME,
			&wndclass
			)
		)
	{
		// otherwise we need to register a new class
		wndclass.style = CS_GLOBALCLASS; //|CS_DBLCLKS;
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
		wndclass.lpszClassName = __EXT_PAGECONTAINERWND_CLASS_NAME;
		if( !::AfxRegisterClass( &wndclass ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}

	g_bPageContainerWndClassRegistered = true;
	return true;
}

void CExtPageContainerWnd::UpdatePageContainerWnd(
	bool bUpdateLayoutNow
	)
{
	ASSERT_VALID( this );

	m_bDelayRecalcLayout = true;

	if(		bUpdateLayoutNow
		&&	GetSafeHwnd() != NULL
		&&	::IsWindow( GetSafeHwnd() )
		)
	{
		_RecalcLayoutImpl();
		Invalidate();
		UpdateWindow();
	}
}

void CExtPageContainerWnd::_RecalcLayoutImpl()
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

	m_nScrollSize = 0L;
bool bCanDoLayoutMouseScrolling = CanDoLayoutMouseScrolling();
	if( !bCanDoLayoutMouseScrolling )
		m_nScrollPos = 0L;

	m_rcScrollPosArea.SetRect( 0, 0, 0, 0 );
LONG nScrollPosAreaMetric = 0L;

CRect rcClient;
	GetClientRect( &rcClient );
CList < HWND, HWND > listHwndToHide;
LONG nCalcOffset = 0L;
bool bHorzLayout = IsHorizontalLayout();

CClientDC dcCalcLayout( this );
	OnPageContainerMeasurePageBorderInfo( 
		dcCalcLayout,
		m_rcPageBorderSizes,
		m_sizeBorderBeforeAfterSpaces,
		m_nSpaceCaption2Wnd,
		nScrollPosAreaMetric,
		bHorzLayout
		);
LONG nRightOrBottomExtent =
		bHorzLayout
			? m_rcPageBorderSizes.right
			: m_rcPageBorderSizes.bottom
			;
LONG nMinBordersXE =
		bHorzLayout
			? (m_rcPageBorderSizes.Height() + __EXT_PAGECONTAINERWND_MINPAGEEXT_DY)
			: (m_rcPageBorderSizes.Width() + __EXT_PAGECONTAINERWND_MINPAGEEXT_DX)
			;
	
	if( bHorzLayout )
		rcClient.bottom -= nScrollPosAreaMetric;
	else
		rcClient.right -= nScrollPosAreaMetric;

LONG nClientXE =
		bHorzLayout
			? rcClient.Height()
			: rcClient.Width()
			;
	if( nClientXE < nMinBordersXE )
	{
		if( bHorzLayout )
			rcClient.bottom =
				rcClient.top
				+ nMinBordersXE
				;
		else
			rcClient.right =
				rcClient.left
				+ nMinBordersXE
				;
	}
CSize sizeClient = rcClient.Size();

LONG nPageXE =
		bHorzLayout
			? (sizeClient.cy - m_rcPageBorderSizes.Height())
			: (sizeClient.cx - m_rcPageBorderSizes.Width())
			;
	if( bHorzLayout )
		m_rcScrollPosArea.SetRect(
			rcClient.left,
			rcClient.bottom,
			rcClient.right,
			rcClient.bottom + nScrollPosAreaMetric
			);
	else
		m_rcScrollPosArea.SetRect(
			rcClient.right,
			rcClient.top,
			rcClient.right + nScrollPosAreaMetric,
			rcClient.bottom
			);

LONG nPageCount = PageGetCount();
	if( nPageCount == 0 )
		m_nScrollPos = 0L;

//LONG nHelperCaptionsExtent = 0L;
//LONG nPageExtentSF = 0L;
PAGE_ITEM_INFO * pPageSF = NULL;

	// first, calc pages layout
PAGE_ITEM_INFO * pPageInfoPrevVisible = NULL;
	for( LONG nIndex = 0; nIndex < nPageCount; nIndex++ )
	{
		PAGE_ITEM_INFO * pPageInfo = PageGetInfo( nIndex );
		ASSERT( pPageInfo != NULL );
		HWND hWndPage = pPageInfo->GetSafePageHwnd();
		ASSERT( hWndPage != NULL );
		ASSERT( ::IsWindow(hWndPage) );
		DWORD dwWndStyle =
			(DWORD)GetWindowLong(hWndPage, GWL_STYLE);
		if( !pPageInfo->IsVisible() )
		{
			if( dwWndStyle & WS_VISIBLE )
				listHwndToHide.AddTail( hWndPage );
			continue;
		}
		
		CSize sizeCaption =
			pPageInfo->MeasureCaption(
				dcCalcLayout,
				bHorzLayout,
				pPageInfoPrevVisible
				);
		ASSERT( sizeCaption == pPageInfo->m_sizeCaptionLastMeasured );
		LONG nCaptionExpent =
			bHorzLayout ? sizeCaption.cx : sizeCaption.cy;
		LONG nMinCaptionExtent = 
			bHorzLayout ? m_rcPageBorderSizes.left : m_rcPageBorderSizes.top;
		if( nCaptionExpent < nMinCaptionExtent )
		{
			nCaptionExpent = nMinCaptionExtent;
			if( bHorzLayout )
				pPageInfo->m_sizeCaptionLastMeasured.cx = nCaptionExpent;
			else
				pPageInfo->m_sizeCaptionLastMeasured.cy = nCaptionExpent;
		}

		LONG nPageWndExtend = 0L;
		pPageInfo->m_rcWnd = pPageInfo->m_rcWndInitial;
		pPageInfo->m_rcWnd.OffsetRect( -pPageInfo->m_rcWnd.TopLeft() );
		bool bPageIsExpanded = pPageInfo->IsExpanded();
		if( bPageIsExpanded )
		{
			if( bHorzLayout )
			{
				pPageInfo->m_rcWnd.bottom =
					pPageInfo->m_rcWnd.top
					+ nPageXE;
				nPageWndExtend =
					pPageInfo->m_rcWnd.Width()
					+ m_nSpaceCaption2Wnd;
				pPageInfo->m_rcWnd.OffsetRect(
					m_nSpaceCaption2Wnd,
					0
					);
				pPageInfo->m_rcWnd.top		+= m_rcPageBorderSizes.top;
				pPageInfo->m_rcWnd.bottom	-= m_rcPageBorderSizes.bottom;
			} // if( bHorzLayout )
			else
			{
				pPageInfo->m_rcWnd.right =
					pPageInfo->m_rcWnd.left
					+ nPageXE;
				nPageWndExtend =
					pPageInfo->m_rcWnd.Height()
					+ m_nSpaceCaption2Wnd;
				pPageInfo->m_rcWnd.OffsetRect(
					0,
					m_nSpaceCaption2Wnd
					);
				pPageInfo->m_rcWnd.left		+= m_rcPageBorderSizes.left;
				pPageInfo->m_rcWnd.right	-= m_rcPageBorderSizes.right;
			} // else from if( bHorzLayout )
		} // if( bPageIsExpanded )

		pPageInfo->m_rcCaption.SetRect(
			0,
			0,
			sizeCaption.cx,
			sizeCaption.cy
			);
		if( bHorzLayout )
		{
			pPageInfo->m_rcCaption.bottom = nPageXE;
			pPageInfo->m_rcCaption.DeflateRect(
				0,
				m_sizeBorderBeforeAfterSpaces.cy,
				0,
				m_sizeBorderBeforeAfterSpaces.cx
				);
			pPageInfo->m_rcCaption.OffsetRect(
				nCalcOffset,
				0
				);
			pPageInfo->m_rcPage = pPageInfo->m_rcCaption;
			pPageInfo->m_rcPage.top = rcClient.top;
			pPageInfo->m_rcPage.bottom = rcClient.bottom;
			pPageInfo->m_rcCaption.DeflateRect(
				0,
				m_rcPageBorderSizes.top,
				0,
				m_rcPageBorderSizes.bottom
				);
			if( bPageIsExpanded )
			{
				pPageInfo->m_rcWnd.OffsetRect(
					nCalcOffset + pPageInfo->m_rcCaption.Width(),
					0
					);
				pPageInfo->m_rcPage.right =
					pPageInfo->m_rcWnd.right
					+ nRightOrBottomExtent
					;
			} // if( bPageIsExpanded )
		} // if( bHorzLayout )
		else
		{
			pPageInfo->m_rcCaption.right = nPageXE;
			pPageInfo->m_rcCaption.DeflateRect(
				m_sizeBorderBeforeAfterSpaces.cx,
				0,
				m_sizeBorderBeforeAfterSpaces.cy,
				0
				);
			pPageInfo->m_rcCaption.OffsetRect(
				0,
				nCalcOffset
				);
			pPageInfo->m_rcPage = pPageInfo->m_rcCaption;
			pPageInfo->m_rcPage.left = rcClient.left;
			pPageInfo->m_rcPage.right = rcClient.right;
			pPageInfo->m_rcCaption.DeflateRect(
				m_rcPageBorderSizes.left,
				0,
				m_rcPageBorderSizes.right,
				0
				);
			if( bPageIsExpanded )
			{
				pPageInfo->m_rcWnd.OffsetRect(
					0,
					nCalcOffset + pPageInfo->m_rcCaption.Height()
					);
				pPageInfo->m_rcPage.bottom =
					pPageInfo->m_rcWnd.bottom
					+ nRightOrBottomExtent
					;
			} // if( bPageIsExpanded )
		} // else from if( bHorzLayout )
		if( !bPageIsExpanded )
		{ // move out of client area
			ASSERT( pPageInfo->m_rcWnd.left == 0 );
			ASSERT( pPageInfo->m_rcWnd.top == 0 );
			pPageInfo->m_rcWnd.OffsetRect(
				-( pPageInfo->m_rcWnd.Width() + 1 ),
				-( pPageInfo->m_rcWnd.Height() + 1 )
				);
		} // move out of client area

		if( (!bCanDoLayoutMouseScrolling)
			&& bPageIsExpanded
			&& (!m_bInExpandingAnimationSF)
			)
		{
			ASSERT( pPageSF == NULL );
			pPageSF = pPageInfo;
//			nPageExtentSF =
//				nPageWndExtend
//				+ nRightOrBottomExtent
//				;
		}

//		nHelperCaptionsExtent += nCaptionExpent;

		nCalcOffset +=
			nCaptionExpent
			+ nPageWndExtend
			+ nRightOrBottomExtent
			;
		pPageInfoPrevVisible = pPageInfo;
	} // for( LONG nIndex = 0; nIndex < nPageCount; nIndex++ )

	if( bCanDoLayoutMouseScrolling )
	{
		m_nScrollSize = nCalcOffset;
		if( bHorzLayout )
			m_nScrollSize -= rcClient.Width();
		else
			m_nScrollSize -= rcClient.Height();
	} // if( bCanDoLayoutMouseScrolling )

	if( m_nScrollSize < 0 )
		m_nScrollSize = 0;
	if( m_nScrollPos > m_nScrollSize )
		m_nScrollPos = m_nScrollSize;

	if( m_pPageInfoEnsureVisible != NULL )
	{
		ASSERT( nPageCount > 0 );
		ASSERT( PageGetIndexOf(m_pPageInfoEnsureVisible) >= 0 );
		ASSERT( PageGetIndexOf(m_pPageInfoEnsureVisible) < nPageCount );
		ASSERT( m_pPageInfoEnsureVisible->IsVisible() );

		CRect rcPageTest(
			m_pPageInfoEnsureVisible->m_rcPage
			);
		if( bHorzLayout )
		{
			rcPageTest.OffsetRect( -m_nScrollPos, 0 );
			if( rcPageTest.right > rcClient.right )
				m_nScrollPos +=
					rcPageTest.right - rcClient.right;
			if( rcPageTest.left < rcClient.left )
				m_nScrollPos +=
					rcPageTest.left - rcClient.left;
		} // if( bHorzLayout )
		else
		{
			rcPageTest.OffsetRect( 0, -m_nScrollPos );
			if( rcPageTest.bottom > rcClient.bottom )
				m_nScrollPos +=
					rcPageTest.bottom - rcClient.bottom;
			if( rcPageTest.top < rcClient.top )
				m_nScrollPos +=
					rcPageTest.top - rcClient.top;
		} // else from if( bHorzLayout )

		if( m_nScrollPos < 0 )
			m_nScrollPos = 0;
		else if( m_nScrollPos > m_nScrollSize )
			m_nScrollPos = m_nScrollSize;

		m_pPageInfoEnsureVisible = NULL;
	} // if( m_pPageInfoEnsureVisible != NULL )

	if( (!bCanDoLayoutMouseScrolling) && nPageCount > 0 && pPageSF != NULL )
	{
		ASSERT( !m_bInExpandingAnimationSF );
		ASSERT( pPageSF->IsVisible() );
		ASSERT( pPageSF->IsExpanded() );

		m_nScrollPos = m_nScrollSize = 0L;

#ifdef _DEBUG
		LONG nIndexSF = PageGetNext( -1L, true, true );
		ASSERT( nIndexSF >= 0 && nIndexSF < nPageCount );
		ASSERT( PageGetNext( nIndexSF, true, true ) < 0 );
		PAGE_ITEM_INFO * pDebugTestPageSF = PageGetInfo( nIndexSF );
		ASSERT( pPageSF == pDebugTestPageSF );
#endif // _DEBUG
		
		LONG nAdjustPos =
			bHorzLayout
				? rcClient.right
				: rcClient.bottom
				;
		for( nIndex = nPageCount -1; true; nIndex-- )
		{
			ASSERT( nIndex >= 0 );
			PAGE_ITEM_INFO * pPageInfo = PageGetInfo( nIndex );
			ASSERT( pPageInfo != NULL );
			if( !pPageInfo->IsVisible() )
				continue;
			if( pPageInfo == pPageSF )
				break;
			ASSERT( ! pPageInfo->IsExpanded() );
			
			if( bHorzLayout )
			{
				LONG nPageExtent =
					pPageInfo->m_rcPage.Width()
					//+ m_nSpaceCaption2Wnd
					+ m_rcPageBorderSizes.right
					;
				LONG nShift =
					nAdjustPos
					- pPageInfo->m_rcPage.right
					;
				nAdjustPos -= nPageExtent;
				pPageInfo->m_rcPage.OffsetRect(
					nShift,
					0
					);
				pPageInfo->m_rcCaption.OffsetRect(
					nShift,
					0
					);
				pPageInfo->m_rcWnd.OffsetRect(
					nShift,
					0
					);
			} // if( bHorzLayout )
			else
			{
				LONG nPageExtent =
					pPageInfo->m_rcPage.Height()
					//+ m_nSpaceCaption2Wnd
					+ m_rcPageBorderSizes.bottom
					;
				LONG nShift =
					nAdjustPos
					- pPageInfo->m_rcPage.bottom
					;
				nAdjustPos -= nPageExtent;
				pPageInfo->m_rcPage.OffsetRect(
					0,
					nShift
					);
				pPageInfo->m_rcCaption.OffsetRect(
					0,
					nShift
					);
				pPageInfo->m_rcWnd.OffsetRect(
					0,
					nShift
					);
			} // else from if( bHorzLayout )
		} // for( nIndex = nPageCount -1; true; nIndex-- )

		if( bHorzLayout )
		{
			LONG nDiff =
				pPageSF->m_rcPage.right
				- pPageSF->m_rcWnd.right
				;
			pPageSF->m_rcPage.right = nAdjustPos;
			pPageSF->m_rcWnd.right = nAdjustPos - nDiff;
		} // if( bHorzLayout )
		else
		{
			LONG nDiff =
				pPageSF->m_rcPage.bottom
				- pPageSF->m_rcWnd.bottom
				;
			pPageSF->m_rcPage.bottom = nAdjustPos;
			pPageSF->m_rcWnd.bottom = nAdjustPos - nDiff;
		} // else from if( bHorzLayout )

	} // if( (!bCanDoLayoutMouseScrolling) && nPageCount > 0 && pPageSF != NULL )

	// second, offset pages layout and find pages to move
CList < HWND, HWND > listHwndToMove;
	for( nIndex = 0; nIndex < nPageCount; nIndex++ )
	{
		PAGE_ITEM_INFO * pPageInfo = PageGetInfo( nIndex );
		ASSERT( pPageInfo != NULL );
		HWND hWndPage = pPageInfo->GetSafePageHwnd();
		ASSERT( hWndPage != NULL );
		ASSERT( ::IsWindow(hWndPage) );
		if( !pPageInfo->IsVisible() )
			continue;
		bool bPageIsExpanded = pPageInfo->IsExpanded();
		if( m_nScrollPos != 0 )
		{
			if( bHorzLayout )
			{
				pPageInfo->m_rcCaption.OffsetRect( -m_nScrollPos, 0 );
				pPageInfo->m_rcPage.OffsetRect( -m_nScrollPos, 0 );
				pPageInfo->m_rcWnd.OffsetRect( -m_nScrollPos, 0 );
			} // if( bHorzLayout )
			else
			{
				pPageInfo->m_rcCaption.OffsetRect( 0, -m_nScrollPos );
				pPageInfo->m_rcPage.OffsetRect( 0, -m_nScrollPos );
				pPageInfo->m_rcWnd.OffsetRect( 0, -m_nScrollPos );
			} // else from if( bHorzLayout )
		} // if( m_nScrollPos != 0 )

		DWORD dwWndStyle =
			(DWORD)GetWindowLong(hWndPage, GWL_STYLE);
		bool bPageWndIsVisible =
			( dwWndStyle & WS_VISIBLE )
				? true
				: false
				;
		if( bPageIsExpanded )
		{
			CRect rcWndPageReal;
			VERIFY( ::GetWindowRect( hWndPage, &rcWndPageReal) );
			ScreenToClient( &rcWndPageReal );
			if(		(!bPageWndIsVisible)
				||	pPageInfo->m_rcWnd != rcWndPageReal
				)
				listHwndToMove.AddTail( hWndPage );
		} // if( bPageIsExpanded )
		else
		{
			if( bPageWndIsVisible )
				listHwndToHide.AddTail( hWndPage );
		} // else from if( bPageIsExpanded )
	} // for( nIndex = 0; nIndex < nPageCount; nIndex++ )

LONG nCountToHide = listHwndToHide.GetCount();
LONG nCountToMove = listHwndToMove.GetCount();
LONG nCountToDefer = nCountToHide + nCountToMove; 
	if( nCountToDefer > 0 )
	{
//		CTypedPtrList < CPtrList, CWnd * > listExplicitUpdate;
		HANDLE hDWP = ::BeginDeferWindowPos( nCountToDefer );
		ASSERT( hDWP != NULL );
		for(	POSITION pos = listHwndToHide.GetHeadPosition();
				pos != NULL;
				)
		{
			if( hDWP == NULL )
				break;
			HWND hWndPage = listHwndToHide.GetNext( pos );
			ASSERT( hWndPage != NULL );
			ASSERT( ::IsWindow( hWndPage ) );
			ASSERT( listHwndToMove.Find(hWndPage) == NULL );
			hDWP = ::DeferWindowPos(
				hDWP, hWndPage, NULL, 0,0,0,0,
				SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOOWNERZORDER
					|SWP_NOSIZE|SWP_NOMOVE|SWP_HIDEWINDOW
				);
			ASSERT( hDWP != NULL );
		}
		for(	pos = listHwndToMove.GetHeadPosition();
				pos != NULL;
				)
		{
			if( hDWP == NULL )
				break;
			HWND hWndPage = listHwndToMove.GetNext( pos );
			ASSERT( hWndPage != NULL );
			ASSERT( ::IsWindow(hWndPage) );
			ASSERT( listHwndToHide.Find(hWndPage) == NULL );
			LONG nIndex = PageFind( hWndPage );
			ASSERT( nIndex >= 0 && nIndex < nPageCount );
			PAGE_ITEM_INFO * pPageInfo = PageGetInfo( nIndex );
			ASSERT( pPageInfo != NULL );
			ASSERT( pPageInfo->GetSafePageHwnd() == hWndPage );
			hDWP = ::DeferWindowPos(
				hDWP, hWndPage, NULL,
				pPageInfo->m_rcWnd.left,
				pPageInfo->m_rcWnd.top,
				pPageInfo->m_rcWnd.Width(),
				pPageInfo->m_rcWnd.Height(),
				SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOOWNERZORDER
					|SWP_SHOWWINDOW
				);
			ASSERT( hDWP != NULL );
//			CWnd * pWndPermanent =
//				CWnd::FromHandlePermanent( hWndPage );
//			if( pWndPermanent != NULL )
//			{
//				if( pWndPermanent->IsKindOf(RUNTIME_CLASS(CExtResizableDialog)) )
//				{
//					listExplicitUpdate.AddTail( pWndPermanent );
//					continue;
//				} // if( pWndPermanent->IsKindOf(RUNTIME_CLASS(CExtResizableDialog)) )
//			} // if( pWndPermanent != NULL )
		}
		if( hDWP != NULL )
		{
			VERIFY( ::EndDeferWindowPos( hDWP ) );
		}
//		for(	pos = listExplicitUpdate.GetHeadPosition();
//				pos != NULL;
//				)
//		{
//			CWnd * pWnd = listExplicitUpdate.GetNext( pos );
//			ASSERT_VALID( pWnd );
//			pWnd->RedrawWindow(
//				NULL, NULL,
//				RDW_INVALIDATE|RDW_UPDATENOW
//					|RDW_ERASE|RDW_ERASENOW
//					|RDW_ALLCHILDREN
//				);
//		}
	} // if( nCountToDefer > 0 )

	m_bInLayoutRecalcProc = false;
}

void CExtPageContainerWnd::_CancelActions()
{
	ASSERT_VALID( this );
	if(		m_bCancelingActionsMode
		||	GetSafeHwnd() == NULL
		||	( ! ::IsWindow(GetSafeHwnd()) )
		)
		return;
	if(		m_bDragScrolling
		||	m_pPageInfoHover != NULL
		||	m_pPageInfoPressed != NULL
		||	m_hWndEditor != NULL
		)
		SendMessage( WM_CANCELMODE );
}

DWORD CExtPageContainerWnd::GetPageContainerStyle() const
{
	ASSERT_VALID( this );
	return m_dwPageContainerStyle;
}

void CExtPageContainerWnd::_AdjustWithPageContainerStyle()
{
	ASSERT_VALID( this );

	if( m_dwPageContainerStyle & __EPCWS_SINGLE_EXPANDED )
	{ // if now is single-expanded mode
		bool bFirst = true;
		for(	LONG nIndex = PageGetNext( -1, true );
				nIndex >= 0;
				nIndex = PageGetNext( nIndex, true )
				)
		{
			if( bFirst )
			{
				bFirst = false;
				continue;
			}
			PAGE_ITEM_INFO * pPageInfo = PageGetInfo( nIndex );
			ASSERT( pPageInfo != NULL );
			ASSERT( pPageInfo->IsExpanded() );
			pPageInfo->m_bExpanded = false;
		}
	} // if now is single-expanded mode
	
	if( m_dwPageContainerStyle & __EPCWS_NOT_ALL_CONTRACTED
		&& PageGetCount() > 0
		)
	{ // if now is not-all-contracted mode
		LONG nIndex = PageGetNext( -1, true, true );
		if( nIndex < 0 )
		{
			nIndex = PageGetNext( -1, false, true );
			if( nIndex >= 0 )
			{
				PAGE_ITEM_INFO * pPageInfo = PageGetInfo( nIndex );
				ASSERT( pPageInfo != NULL );
				ASSERT( !pPageInfo->IsExpanded() );
				pPageInfo->m_bExpanded = true;
			}
		}
	} // if now is not-all-contracted mode
}

DWORD CExtPageContainerWnd::ModifyPageContainerStyle(
	DWORD dwAdd,
	DWORD dwRemove, // = 0L
	bool bUpdateLayoutNow // = false
	)
{
	ASSERT_VALID( this );
DWORD dwPageContainerStyleOld = m_dwPageContainerStyle;
	m_dwPageContainerStyle |= dwAdd;
	m_dwPageContainerStyle &= ~dwRemove;
	_AdjustWithPageContainerStyle();
	UpdatePageContainerWnd( bUpdateLayoutNow );
	return dwPageContainerStyleOld;
}

HWND CExtPageContainerWnd::OnPageContainerStartItemEditor(
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo
	)
{
	ASSERT_VALID( this );
	ASSERT( pPageInfo != NULL );
	ASSERT( PageGetCount() > 0 );
	_CancelActions();
	if( (m_dwPageContainerStyle & __EPCWS_CAPTION_EDIT) == 0 )
		return NULL;
	if( (m_dwPageContainerStyle & __EPCWS_HORIZONTAL) != 0 )
		return NULL;

	if( !pPageInfo->IsVisible() )
		return NULL;
LONG nIndex = PageGetIndexOf( pPageInfo );
	ASSERT( nIndex >= 0 && nIndex < PageGetCount() );
	if(	!PageEnsureVisible( nIndex, true ) )
		return NULL;

CRect rcItem( 0, 0, 0, 0 );
	 PageGetLayoutRectInfo( nIndex, NULL, &rcItem );
//	rcItem.DeflateRect( 3, 2 );

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
	if( rcItem.left < rcClient.left )
		rcItem.left = rcClient.left;
	if( rcItem.right > rcClient.right )
		rcItem.right = rcClient.right;
	if( (rcItem.right - rcItem.left) < __EXT_PAGECONTAINERWND_MIN_EDITOR_DX )
		return false;
	if( (rcItem.bottom - rcItem.top) < __EXT_PAGECONTAINERWND_MIN_EDITOR_DY )
		return false;

CItemTextEditWnd * pWndEdit =
		new CItemTextEditWnd( pPageInfo );
	if( !pWndEdit->Create(
			WS_CHILD|WS_BORDER|ES_AUTOHSCROLL
				|( (m_dwPageContainerStyle&__EPCWS_CAPTION_CENTER_TEXT) ? ES_CENTER : 0)
				,
			rcItem,
			this,
			(UINT)IDC_STATIC
			)
		)
		return NULL;

HWND hWndEditor = pWndEdit->GetSafeHwnd();
//bool bHorzLayout = IsHorizontalLayout();
//CFont * pItemFont = pPageInfo->GetCaptionFont( bHorzLayout );
CFont * pItemFont = pPageInfo->GetCaptionFont( false );
	if( pItemFont != NULL )
		pWndEdit->SetFont( pItemFont );
	else
		::SendMessage(
			hWndEditor,
			WM_SETFONT,
			(WPARAM)::GetStockObject( DEFAULT_GUI_FONT ),
			(LPARAM)TRUE
			);
__EXT_MFC_SAFE_LPCTSTR sItemText = pPageInfo->TextGet();
	if( sItemText == NULL )
		sItemText = _T("");
	pWndEdit->SetWindowText( sItemText );
	pWndEdit->ShowWindow( SW_SHOW );
	pWndEdit->SetFocus();
	
	return hWndEditor;
}

void CExtPageContainerWnd::OnPageContainerCancelItemEditor(
	HWND hWndEditor
	)
{
	ASSERT_VALID( this );
	ASSERT( hWndEditor != NULL );
	if( !::IsWindow(hWndEditor) )
		return;
	::DestroyWindow( hWndEditor );
}

bool CExtPageContainerWnd::OnPageContainerDeactivateEditor(
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo,
	bool bUpdateItemText
	)
{
	ASSERT_VALID( this );
	ASSERT( pPageInfo != NULL );
//	ASSERT( pPageInfo->IsVisible() );
	pPageInfo;
	bUpdateItemText;
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

CExtCmdIcon * CExtPageContainerWnd::OnPageContainerQueryItemIcon(
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo,
	CSize * pSize // = NULL
	)
{
	ASSERT_VALID( this );
	ASSERT( pPageInfo != NULL );
	if( pPageInfo->m_icon.IsEmpty() )
	{
		if( pSize != NULL )
			pSize->cx = pSize->cy = 0;
		return NULL;
	}
	if( pSize != NULL )
		*pSize = pPageInfo->m_icon.GetSize();
	return (&pPageInfo->m_icon);
}

__EXT_MFC_SAFE_LPCTSTR CExtPageContainerWnd::OnPageContainerQueryItemText(
	const CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo
	)
{
	ASSERT_VALID( this );
	ASSERT( pPageInfo != NULL );
	return pPageInfo->m_sText;
}

void CExtPageContainerWnd::OnPageContainerDoItemTextChanging(
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo,
	CExtSafeString & sItemText,
	__EXT_MFC_SAFE_LPCTSTR sNewText
	)
{
	ASSERT_VALID( this );
	ASSERT( pPageInfo != NULL );
	pPageInfo;
	sItemText = (sNewText == NULL) ? _T("") : sNewText;
	UpdatePageContainerWnd( false );
}

CFont * CExtPageContainerWnd::OnPageContainerQueryItemFont(
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo,
	bool bHorzLayout
	)
{
	ASSERT_VALID( this );
	ASSERT( pPageInfo != NULL );
	pPageInfo;
	if( bHorzLayout )
	{
		if( m_dwPageContainerStyle & __EPCWS_CAPTION_INV_VFONT )
			return &g_PaintManager->m_FontNormalVert;
		else
			return &g_PaintManager->m_FontNormalVertX;
	}
	else
		return &g_PaintManager->m_FontNormal;
}

void CExtPageContainerWnd::OnPageContainerItemInserted(
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo
	)
{
	ASSERT_VALID( this );
	ASSERT( pPageInfo != NULL );
HWND hWndPage = pPageInfo->GetSafePageHwnd();
	ASSERT( hWndPage != NULL );
	ASSERT( ::IsWindow( hWndPage ) );
	VERIFY(
		SetupHookWndSink(
			hWndPage,
			false,
			false
			)
		);
}

bool CExtPageContainerWnd::OnPageContainerItemRemoving(
	LONG nIndex,
	bool bDestroyPageWindow
	)
{
	ASSERT_VALID( this );
	ASSERT( PageGetCount() > 0 );
	ASSERT( nIndex >= 0 );
	ASSERT( nIndex < PageGetCount() );
	nIndex;
	bDestroyPageWindow;
	return true;
}

void CExtPageContainerWnd::OnPageContainerItemRemoved(
	LONG nIndex,
	bool bDestroyPageWindow,
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo
	)
{
	ASSERT_VALID( this );
	ASSERT( pPageInfo != NULL );
	nIndex;
	bDestroyPageWindow;
	pPageInfo;
}

void CExtPageContainerWnd::OnPageContainerMeasurePageBorderInfo(
	CDC & dcCalcLayout,
	CRect & rcPageBorderSizes,
	CSize & sizeBorderBeforeAfterSpaces,
	LONG & nSpaceCaption2Wnd,
	LONG & nScrollPosAreaMetric,
	bool bHorzLayout
	)
{
	ASSERT_VALID( this );
	dcCalcLayout;
	if( m_dwPageContainerStyle & __EPCWS_PAGE_BORDERS )
	{
		rcPageBorderSizes.SetRect(
			__EXT_PAGECONTAINERWND_DEFPGB_LEFT,
			__EXT_PAGECONTAINERWND_DEFPGB_TOP,
			__EXT_PAGECONTAINERWND_DEFPGB_RIGHT,
			__EXT_PAGECONTAINERWND_DEFPGB_BOTTOM
			);
		if( bHorzLayout )
		{
			sizeBorderBeforeAfterSpaces.cx =
				__EXT_PAGECONTAINERWND_DEFCAPT_DY_BEFORE;
			sizeBorderBeforeAfterSpaces.cy =
				__EXT_PAGECONTAINERWND_DEFCAPT_DY_AFTER;
		} // if( bHorzLayout )
		else
		{
			sizeBorderBeforeAfterSpaces.cx =
				__EXT_PAGECONTAINERWND_DEFCAPT_DX_BEFORE;
			sizeBorderBeforeAfterSpaces.cy =
				__EXT_PAGECONTAINERWND_DEFCAPT_DX_AFTER;
		} // else from if( bHorzLayout )
	} // if( m_dwPageContainerStyle & __EPCWS_PAGE_BORDERS )
	else
	{
			sizeBorderBeforeAfterSpaces.cx
			= sizeBorderBeforeAfterSpaces.cy
			= 0;
		rcPageBorderSizes.SetRect(
			0,
			0,
			bHorzLayout ? __EXT_PAGECONTAINERWND_CAPT2WND_DX : 0,
			bHorzLayout ? 0 : __EXT_PAGECONTAINERWND_CAPT2WND_DY
			);
	} // else from if( m_dwPageContainerStyle & __EPCWS_PAGE_BORDERS )
	
	nScrollPosAreaMetric = 0L;
bool bHaveScrollPosArea = HaveScrollPosArea();
	if( bHorzLayout )
	{
		nSpaceCaption2Wnd =
			__EXT_PAGECONTAINERWND_CAPT2WND_DX;
		if( bHaveScrollPosArea )
			nScrollPosAreaMetric =
				__EXT_PAGECONTAINERWND_SCROLLPOSAREA_DY;
	} // if( bHorzLayout )
	else
	{
		nSpaceCaption2Wnd =
			__EXT_PAGECONTAINERWND_CAPT2WND_DY;
		if( bHaveScrollPosArea )
			nScrollPosAreaMetric =
				__EXT_PAGECONTAINERWND_SCROLLPOSAREA_DX;
	} // else from if( bHorzLayout )
}

CExtPaintManager::glyph_t &
	CExtPageContainerWnd::OnPageContainerQueryItemGlyph(
		const CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo,
		bool bExpandedGlyph
		) const
{
	ASSERT_VALID( this );
	ASSERT( pPageInfo != NULL );
	pPageInfo;
CExtPaintManager::glyph_t * pGlyph =
		CExtPaintManager::g_DockingCaptionGlyphs[
			bExpandedGlyph //( pPageInfo->IsExpanded() )
				? CExtPaintManager::__DCBT_HOLLOW_MINUS
				: CExtPaintManager::__DCBT_HOLLOW_PLUS
			];
	ASSERT( pGlyph != NULL );
	return *pGlyph;
}


void CExtPageContainerWnd::OnPageContainerMeasureCaptionSize(
	CDC & dcCalcLayout,
	bool bHorzLayout,
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo,
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfoPrev,
	CSize & sizeCaption,
	CSize & sizeText
	)
{
	ASSERT_VALID( this );
	ASSERT( pPageInfo != NULL );
	pPageInfoPrev;
	
CSize sizeIcon( 0, 0 );
HICON hItemIcon = pPageInfo->IconGet( &sizeIcon );

__EXT_MFC_SAFE_LPCTSTR sItemText = pPageInfo->TextGet();
	if( sItemText == NULL )
		sItemText = _T("");
LONG nItemTextLen = _tcslen( sItemText );

	sizeText.cx = sizeText.cy = 0;
	if( nItemTextLen > 0 )
	{
		CFont * pFont = pPageInfo->GetCaptionFont( false );
		ASSERT( pFont != NULL );
		ASSERT( pFont->GetSafeHandle() != NULL );
		CRect rcMeasureText =
			CExtPaintManager::stat_CalcTextDimension(
				dcCalcLayout,
				*pFont,
				sItemText
				);
		sizeText = rcMeasureText.Size();
	} // if( nItemTextLen > 0 )

CSize sizeGlyph( 0, 0 );
	if( GetPageContainerStyle() & __EPCWS_CAPTION_EXPBTN )
	{
		CExtPaintManager::glyph_t & _glyph =
			OnPageContainerQueryItemGlyph(
				pPageInfo,
				pPageInfo->IsExpanded()
				);
		sizeGlyph = _glyph.Size();
	}

	sizeCaption.cx = sizeCaption.cy = 0;
	if( bHorzLayout )
	{
		sizeText.cx += 10;
		sizeText.cy += 2;

		sizeCaption.cy +=
			sizeText.cx
			+ __EXT_PAGECONTAINERWND_CAPT_BEFORE_DY
			+ __EXT_PAGECONTAINERWND_CAPT_AFTER_DY
			;
		if( sizeGlyph.cy > 0 )
			sizeCaption.cy +=
				sizeGlyph.cy
				+ __EXT_PAGECONTAINERWND_CAPT_AFTER_GLYPH_DY
				;
		sizeCaption.cx = max( sizeText.cy, sizeGlyph.cx );
		if( hItemIcon != NULL )
		{
			sizeCaption.cy +=
				sizeIcon.cy
				+ __EXT_PAGECONTAINERWND_CAPT_ICON2TEXT_DY
				;
			sizeCaption.cx = max( sizeIcon.cx, sizeCaption.cx );
		} // if( hItemIcon != NULL )
		if( sizeCaption.cx < __EXT_PAGECONTAINERWND_CAPT_INNER_MIN_DX )
			sizeCaption.cx = __EXT_PAGECONTAINERWND_CAPT_INNER_MIN_DX;
		sizeCaption.cx += __EXT_PAGECONTAINERWND_CAPT_HBORDER_DX*2;
		sizeCaption.cy += __EXT_PAGECONTAINERWND_CAPT_HBORDER_DY*2;
	} // if( bHorzLayout )
	else
	{
		sizeCaption.cx +=
			sizeText.cx
			+ __EXT_PAGECONTAINERWND_CAPT_BEFORE_DX
			+ __EXT_PAGECONTAINERWND_CAPT_AFTER_DX
			;
		if( sizeGlyph.cx > 0 )
			sizeCaption.cx +=
				sizeGlyph.cx
				+ __EXT_PAGECONTAINERWND_CAPT_AFTER_GLYPH_DX
				;
		sizeCaption.cy = max( sizeText.cy, sizeGlyph.cy );
		if( hItemIcon != NULL )
		{
			sizeCaption.cx +=
				sizeIcon.cx
				+ __EXT_PAGECONTAINERWND_CAPT_ICON2TEXT_DX
				;
			sizeCaption.cy = max( sizeIcon.cy, sizeCaption.cy );
		} // if( hItemIcon != NULL )
		if( sizeCaption.cy < __EXT_PAGECONTAINERWND_CAPT_INNER_MIN_DY )
			sizeCaption.cy = __EXT_PAGECONTAINERWND_CAPT_INNER_MIN_DY;
		sizeCaption.cx += __EXT_PAGECONTAINERWND_CAPT_VBORDER_DX*2;
		sizeCaption.cy += __EXT_PAGECONTAINERWND_CAPT_VBORDER_DY*2;
	} // if( bHorzLayout )
	
}

LONG CExtPageContainerWnd::PageInsert(
	HWND hWndPage,
	LONG nIndexInsertBefore, // = -1L // append
	__EXT_MFC_SAFE_LPCTSTR sPageCaptionText, // = NULL
	HICON hCaptionIcon, // = NULL
	bool bCopyCaptionIcon, // = false
	LPARAM lParam, // = 0L
	bool bExpanded, // = true
	bool bEnsureVisible, // = false
	bool bUpdateLayoutNow // = false
	)
{
	ASSERT_VALID( this );
	if( hWndPage == NULL
		|| ( ! ::IsWindow(hWndPage) )
		)
	{
		ASSERT( FALSE );
		return -1L;
	}
	if( PageFind( hWndPage ) >= 0 )
	{
		ASSERT( FALSE );
		return -1L;
	}

	_CancelActions();

LONG nPageCount = PageGetCount();
	if(		nIndexInsertBefore < 0
		||	nIndexInsertBefore > nPageCount
		)
		nIndexInsertBefore = nPageCount;

PAGE_ITEM_INFO * pPageInfo = new PAGE_ITEM_INFO(
		hWndPage,
		sPageCaptionText,
		lParam,
		false
		);

	if( hCaptionIcon != NULL )
		pPageInfo->IconSet( hCaptionIcon, bCopyCaptionIcon );

	m_vPages.InsertAt( nIndexInsertBefore, pPageInfo, 1 );

	if( pPageInfo->IsVisible() && bExpanded )
	{
		VERIFY(
			PageExpand(
				nIndexInsertBefore,
				true,
				bEnsureVisible,
				false
				)
			);
	} // if( pPageInfo->IsVisible() && bExpanded )
	
	OnPageContainerItemInserted( pPageInfo );
	UpdatePageContainerWnd( bUpdateLayoutNow );
	return nIndexInsertBefore;
}

LONG CExtPageContainerWnd::PageRemove(
	LONG nIndex,
	LONG nCount, // = 1L
	bool bDestroyPageWindow, // = true
	bool bUpdateLayoutNow // = false
	)
{
	ASSERT_VALID( this );
LONG nRemovedCount = 0;
LONG nPageCount = PageGetCount();
LONG nLastIndex = nIndex + nCount - 1;
	if(		nPageCount > 0
		&&	nIndex >= 0
		&&	nIndex <= (nPageCount-1)
		)
	{

		_CancelActions();

		// fixed by Alexey Ignatenko
		// after release of 2.25
		//if( nLastIndex > nCount )
		//	nLastIndex = nCount - 1;
		if( nLastIndex > nPageCount )
			nLastIndex = nPageCount - 1;


		ASSERT( nLastIndex >= nIndex );
		for( ; true ; )
		{
			PAGE_ITEM_INFO * pPageInfo =
				PageGetInfo( nLastIndex );
			ASSERT( pPageInfo != NULL );
			HWND hWndPage = pPageInfo->GetSafePageHwnd();
			if( OnPageContainerItemRemoving(
					nLastIndex,
					bDestroyPageWindow
					)
				)
			{
				if( pPageInfo == m_pPageInfoEnsureVisible )
					m_pPageInfoEnsureVisible = NULL;
				nRemovedCount++;
				m_vPages.RemoveAt( nLastIndex );
				if(		hWndPage != NULL
					&&	::IsWindow( hWndPage )
					)
				{
					if( IsHookedWindow( hWndPage ) )
					{
						VERIFY(
							SetupHookWndSink(
								hWndPage,
								true
								)
							);
					}
					if( bDestroyPageWindow )
						::DestroyWindow( hWndPage );
				}
				OnPageContainerItemRemoved(
					nLastIndex,
					bDestroyPageWindow,
					pPageInfo
					);
				delete pPageInfo;
			}
			if( nLastIndex == nIndex )
				break;
			nLastIndex--;
		}
		ASSERT( nRemovedCount <= nCount );
	}

	UpdatePageContainerWnd( bUpdateLayoutNow );
	return nRemovedCount;
}

void CExtPageContainerWnd::PageRemoveAll(
	bool bDestroyPageWindow, // = true
	bool bUpdateLayoutNow // = false
	)
{
	ASSERT_VALID( this );
	PageRemove(
		0,
		PageGetCount(),
		bDestroyPageWindow,
		bUpdateLayoutNow
		);
	ASSERT( PageGetCount() == 0 );
}

LONG CExtPageContainerWnd::PageGetCount() const
{
	ASSERT_VALID( this );
LONG nPageCount = m_vPages.GetSize();
	return nPageCount;
}

LONG CExtPageContainerWnd::PageGetIndexOf(
	const CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo
	) const
{
	ASSERT_VALID( this );
	if( pPageInfo == NULL )
		return -1L;
LONG nPageCount = PageGetCount();
	if( nPageCount == 0 )
		return -1L;
	for( LONG nIndex = 0; nIndex < nPageCount; nIndex++ )
	{
		const PAGE_ITEM_INFO * pPageInfo2 = PageGetInfo( nIndex );
		ASSERT( pPageInfo != NULL );
		if( pPageInfo == pPageInfo2 )
			return nIndex;
	}
	return -1L;
}

CExtPageContainerWnd::PAGE_ITEM_INFO * CExtPageContainerWnd::PageGetInfo(
	LONG nIndex
	)
{
	ASSERT_VALID( this );
	if( nIndex < 0 )
		return NULL;
LONG nPageCount = PageGetCount();
	if( nIndex >= nPageCount )
		return NULL;
PAGE_ITEM_INFO * pPageInfo = m_vPages[ nIndex ];
	ASSERT( pPageInfo != NULL );
	return pPageInfo;
}

const CExtPageContainerWnd::PAGE_ITEM_INFO * CExtPageContainerWnd::PageGetInfo(
	LONG nIndex
	) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtPageContainerWnd * > ( this ) )
			-> PageGetInfo( nIndex );
}

void CExtPageContainerWnd::PageGetLayoutRectInfo(
	LONG nIndex,
	RECT * pRectPage,
	RECT * pRectCaption, // = NULL
	RECT * pRectWnd // = NULL
	) const
{
	if(		pRectPage == NULL
		&&	pRectCaption == NULL
		&&	pRectWnd == NULL
		)
		return;

	if( pRectPage != NULL )
		::memset( pRectPage, 0, sizeof(RECT) );
	if( pRectCaption != NULL )
		::memset( pRectCaption, 0, sizeof(RECT) );
	if( pRectWnd != NULL )
		::memset( pRectWnd, 0, sizeof(RECT) );

	if( nIndex < 0 )
		return;
LONG nPageCount = PageGetCount();
	if( nIndex >= nPageCount )
		return;

const PAGE_ITEM_INFO * pPageInfo = m_vPages[ nIndex ];
	ASSERT( pPageInfo != NULL );
	if( !pPageInfo->IsVisible() )
		return;

//bool bHorzLayout = IsHorizontalLayout();
LONG nOffsetX = 0, nOffsetY = 0;
//	if( bHorzLayout )
//		nOffsetX = -m_nScrollPos;
//	else
//		nOffsetY = -m_nScrollPos;

	if( pRectPage != NULL )
	{
		::CopyRect( pRectPage, &pPageInfo->m_rcPage );
		VERIFY(
			::OffsetRect(
				pRectPage,
				nOffsetX,
				nOffsetY
				)
			);
	}
	if( pRectCaption != NULL )
	{
		::CopyRect( pRectCaption, &pPageInfo->m_rcCaption );
		VERIFY(
			::OffsetRect(
				pRectCaption,
				nOffsetX,
				nOffsetY
				)
			);
	}
	if( pRectWnd != NULL )
	{
		if( pPageInfo->IsExpanded() )
		{
			::CopyRect( pRectWnd, &pPageInfo->m_rcWnd );
			VERIFY(
				::OffsetRect(
					pRectWnd,
					nOffsetX,
					nOffsetY
					)
				);
		}
	}
}

LONG CExtPageContainerWnd::PageFind(
	HWND hWnd
	) const
{
	ASSERT_VALID( this );
	if( hWnd == NULL
		|| ( ! ::IsWindow(hWnd) )
		)
		return -1L;
LONG nPageCount = PageGetCount();
	if( nPageCount == 0 )
		return -1L;
	for( LONG nIndex = 0; nIndex < nPageCount; nIndex++ )
	{
		const PAGE_ITEM_INFO * pPageInfo = PageGetInfo( nIndex );
		ASSERT( pPageInfo != NULL );
		HWND hWndPage = pPageInfo->GetSafePageHwnd();
		if( hWndPage == hWnd )
			return nIndex;
	}
	return -1L;
}

bool CExtPageContainerWnd::PageIsVisible( LONG nIndex ) const
{
	ASSERT_VALID( this );
	if( nIndex < 0 )
		return false;
LONG nPageCount = PageGetCount();
	if( nIndex >= nPageCount )
		return false;
const PAGE_ITEM_INFO * pPageInfo = PageGetInfo( nIndex );
	ASSERT( pPageInfo != NULL );
	return pPageInfo->IsVisible();
}

bool CExtPageContainerWnd::PageShow(
	LONG nIndex,
	bool bShow, // = true
	bool bEnsureVisible, // = false
	bool bUpdateLayoutNow // = false
	)
{
	ASSERT_VALID( this );
	if( nIndex < 0 )
		return false;

	_CancelActions();

LONG nPageCount = PageGetCount();
	if( nIndex >= nPageCount )
		return false;

PAGE_ITEM_INFO * pPageInfo = PageGetInfo( nIndex );
	ASSERT( pPageInfo != NULL );

bool bPageVisible = pPageInfo->IsVisible();
	if( ((!bShow) && (!bPageVisible))
		|| (bShow && bPageVisible)
		)
		return true;

	pPageInfo->m_bVisible = bShow;
	if( bShow && bEnsureVisible )
		PageEnsureVisible( nIndex, false );
	else
	{
		if( (!bShow)
			&& pPageInfo == m_pPageInfoEnsureVisible
			)
			m_pPageInfoEnsureVisible = NULL;
	}

	_AdjustWithPageContainerStyle();

	UpdatePageContainerWnd( bUpdateLayoutNow );
	return true;
}

bool CExtPageContainerWnd::PageIsExpanded( LONG nIndex ) const
{
	ASSERT_VALID( this );
	if( nIndex < 0 )
		return false;
LONG nPageCount = PageGetCount();
	if( nIndex >= nPageCount )
		return false;
const PAGE_ITEM_INFO * pPageInfo = PageGetInfo( nIndex );
	ASSERT( pPageInfo != NULL );
	return pPageInfo->IsExpanded();
}

bool CExtPageContainerWnd::OnPageContainerQueryCancelExpandAnimation()
{
	ASSERT_VALID( this );
	return false; // by default - no reason to cancel expand animation
}

bool CExtPageContainerWnd::PageExpand(
	LONG nIndex,
	bool bExpand, // = true
	bool bEnsureVisible, // = false
	bool bUpdateLayoutNow // = false
	)
{
	ASSERT_VALID( this );
	ASSERT( !m_bInExpandingAnimationSF );

	if( m_hWndEditor != NULL )
	{
		OnPageContainerCancelItemEditor( m_hWndEditor );
		m_hWndEditor = NULL;
	}

	if( nIndex < 0 )
		return false;
LONG nPageCount = PageGetCount();
	if( nIndex >= nPageCount )
		return false;

PAGE_ITEM_INFO * pPageInfo = PageGetInfo( nIndex );
	ASSERT( pPageInfo != NULL );
	if( !pPageInfo->IsVisible() )
		return false;

//PAGE_ITEM_INFO * pPagePrevSF = NULL;
	if(		(!m_bDelayRecalcLayout)
		&&	bUpdateLayoutNow
		&&	bExpand
		&&	nPageCount > 0
		&&	(m_dwPageContainerStyle & __EPCWS_NOT_ALL_CONTRACTED) != 0
		&&	(m_dwPageContainerStyle & __EPCWS_NO_EXPAND_ANIMATION) == 0
		&&	(!CanDoLayoutMouseScrolling())
		&&	OnPageContainerCanDoHoverActions()
		&&	pPageInfo->m_rcPage.left < pPageInfo->m_rcPage.right
		&&	pPageInfo->m_rcPage.top < pPageInfo->m_rcPage.bottom
		)
	{
		LONG nAlreadyExpanded = PageGetNext( -1L, true, true );
		if(		nAlreadyExpanded >= 0
			&&	PageGetNext( nAlreadyExpanded, true, true ) < 0
			&&	( ! OnPageContainerQueryCancelExpandAnimation() )
			)
		{
			ASSERT( nAlreadyExpanded != nIndex );
			PAGE_ITEM_INFO * pPageExpPrev =
				PageGetInfo( nAlreadyExpanded );
			ASSERT( pPageExpPrev != NULL );
			ASSERT( pPageExpPrev->IsExpanded() );
			pPageInfo->m_bExpanded = true;
			
			m_bInExpandingAnimationSF = true;
			CRect rcInitialOld( pPageExpPrev->m_rcWndInitial );
			CRect rcInitialNew( pPageInfo->m_rcWndInitial );
			CRect rcAnimationSrc( pPageExpPrev->m_rcWnd );
			rcAnimationSrc.OffsetRect( -rcAnimationSrc.TopLeft() );

			bool bHorzLayout = IsHorizontalLayout();
			LONG nSpaceToAnimate =
				bHorzLayout
					? ( rcAnimationSrc.Width() + m_rcPageBorderSizes.right - m_nSpaceCaption2Wnd )
					: ( rcAnimationSrc.Height() + m_rcPageBorderSizes.bottom - m_nSpaceCaption2Wnd )
					;
			
			clock_t nLastAnimTime = clock();
			for( LONG nStep = 1; nStep <= g_nAnimationStepCount; nStep++ )
			{
				LONG nAnimationOffset =
					::MulDiv(
						g_bAnimationAccelerated
							? (nStep * nStep)
							: nStep
							,
						nSpaceToAnimate,
						g_bAnimationAccelerated
							? ( (g_nAnimationStepCount+1)*(g_nAnimationStepCount+1) )
							: (g_nAnimationStepCount+1)
						);
				pPageInfo->m_rcWndInitial = rcAnimationSrc;
				pPageExpPrev->m_rcWndInitial = rcAnimationSrc;
				if( bHorzLayout )
				{
					pPageInfo->m_rcWndInitial.right =
						pPageInfo->m_rcWndInitial.left
						+ nAnimationOffset
						;
					pPageExpPrev->m_rcWndInitial.right =
						pPageExpPrev->m_rcWndInitial.left
						+ nSpaceToAnimate
						- nAnimationOffset
						;
				} // if( bHorzLayout )
				else
				{
					pPageInfo->m_rcWndInitial.bottom =
						pPageInfo->m_rcWndInitial.top
						+ nAnimationOffset
						;
					pPageExpPrev->m_rcWndInitial.bottom =
						pPageExpPrev->m_rcWndInitial.top
						+ nSpaceToAnimate
						- nAnimationOffset
						;
				} // if( bHorzLayout )
				UpdatePageContainerWnd( true );
				clock_t nNewAnimTime = clock();
				clock_t nDuration =
					nLastAnimTime - nNewAnimTime;
				if( nDuration < ((clock_t)g_ctAnimationStepClocks) )
					::Sleep( g_ctAnimationStepClocks - nDuration );
				nLastAnimTime = clock();
			} // for( LONG nStep = 1; nStep <= g_nAnimationStepCount; nStep++ )

			pPageExpPrev->m_rcWndInitial = rcInitialOld;
			pPageInfo->m_rcWndInitial = rcInitialNew;
			pPageExpPrev->m_bExpanded = false;
			m_bInExpandingAnimationSF = false;
			UpdatePageContainerWnd( true );

			return true;
		}
	}

	if( (m_dwPageContainerStyle & __EPCWS_SINGLE_EXPANDED) != 0
		&& bExpand
		)
	{
		for(	LONG nAlreadyExpanded = PageGetNext( -1L, true );
				nAlreadyExpanded >= 0;
				nAlreadyExpanded = PageGetNext( nAlreadyExpanded, true )
				)
		{
			CExtPageContainerWnd::PAGE_ITEM_INFO *
				pPageInfoExpanded =
					PageGetInfo( nAlreadyExpanded );
			ASSERT( pPageInfoExpanded != NULL );
			ASSERT( pPageInfoExpanded->IsExpanded() );
			if( pPageInfo != pPageInfoExpanded )
				pPageInfoExpanded->m_bExpanded = false;
		}
	}

	if( (m_dwPageContainerStyle & __EPCWS_NOT_ALL_CONTRACTED) != 0
		&&	(!bExpand)
		)
	{
		LONG nAlreadyExpanded = PageGetNext( -1L, true );
		if( nAlreadyExpanded < 0 )
		{
			if( bEnsureVisible )
				PageEnsureVisible( nIndex, bUpdateLayoutNow );
			return false;
		}
		CExtPageContainerWnd::PAGE_ITEM_INFO *
			pPageInfoExpanded =
				PageGetInfo( nAlreadyExpanded );
		ASSERT( pPageInfoExpanded != NULL );
		ASSERT( pPageInfoExpanded->IsExpanded() );
		if( pPageInfo == pPageInfoExpanded )
		{
			nAlreadyExpanded = PageGetNext( nAlreadyExpanded, true );
			if( nAlreadyExpanded < 0 )
			{
				if( bEnsureVisible )
					PageEnsureVisible( nIndex, bUpdateLayoutNow );
				return false;
			}
		}
	}
	
bool bPageExpanded = pPageInfo->IsExpanded();
	if( ((!bExpand) && (!bPageExpanded))
		|| (bExpand && bPageExpanded)
		)
	{
		if( bEnsureVisible )
			PageEnsureVisible( nIndex, bUpdateLayoutNow );
		return true;
	}

	pPageInfo->m_bExpanded = bExpand;
	if( bEnsureVisible )
		PageEnsureVisible( nIndex, false );
	UpdatePageContainerWnd( bUpdateLayoutNow );
	return true;
}

bool CExtPageContainerWnd::PageEnsureVisible(
	LONG nIndex,
	bool bUpdateLayoutNow // = false
	)
{
	ASSERT_VALID( this );
	if( nIndex < 0 )
		return false;
LONG nPageCount = PageGetCount();
	if( nIndex >= nPageCount )
		return false;

PAGE_ITEM_INFO * pPageInfo = PageGetInfo( nIndex );
	ASSERT( pPageInfo != NULL );
	if( !pPageInfo->IsVisible() )
		return false;

	m_pPageInfoEnsureVisible = pPageInfo;

	UpdatePageContainerWnd( bUpdateLayoutNow );
	return true;
}

CExtPageContainerWnd::HIT_TEST_INFO &
	CExtPageContainerWnd::PageHitTest(
		CExtPageContainerWnd::HIT_TEST_INFO & _pht
		) const
{
	ASSERT_VALID( this );

	_pht.Clear( false );
	OnPageContainerHitTest( _pht );
	return _pht;
}

void CExtPageContainerWnd::OnPageContainerHitTest(
	CExtPageContainerWnd::HIT_TEST_INFO & _pht
	) const
{
	ASSERT_VALID( this );

	_pht.Clear( false );
DWORD dwWndStyle = GetStyle();
	if( (dwWndStyle & WS_VISIBLE) == 0 )
		return;
CRect rcClient;
	GetClientRect( &rcClient );
	if(		rcClient.left >= rcClient.right
		||	rcClient.top >= rcClient.bottom
		)
		return;
	if( !rcClient.PtInRect(_pht.m_ptClient) )
		return;

	if( HaveScrollPosArea()
		&& ( !m_rcScrollPosArea.IsRectEmpty() )
		&& m_rcScrollPosArea.PtInRect(_pht.m_ptClient)
		)
	{
		_pht.m_dwHitTestCode = __EPCHT_ON_SCROLL_POS_AREA;
		return;
	}

LONG nPageCount = PageGetCount();
CRect rcPage, rcCaption;
bool bHorzLayout = IsHorizontalLayout();
	for( LONG nIndex = 0; nIndex < nPageCount; nIndex++ )
	{
		const PAGE_ITEM_INFO * pPageInfo = PageGetInfo( nIndex );
		ASSERT( pPageInfo != NULL );
		if( !pPageInfo->IsVisible() )
			continue;
		PageGetLayoutRectInfo(
			nIndex,
			&rcPage,
			&rcCaption
			);
		if( !rcPage.PtInRect(_pht.m_ptClient) )
			continue;

		_pht.m_nItemIndex = nIndex;
		_pht.m_pPageInfo =
			const_cast < PAGE_ITEM_INFO * > ( pPageInfo );
		
		if( rcCaption.PtInRect(_pht.m_ptClient) )
		{
			_pht.m_dwHitTestCode = __EPCHT_ON_PAGE_CAPT_TEXT;

			if( bHorzLayout )
			{
				rcCaption.DeflateRect(
					__EXT_PAGECONTAINERWND_CAPT_HBORDER_DX,
					__EXT_PAGECONTAINERWND_CAPT_HBORDER_DY
					);
				rcCaption.bottom -= __EXT_PAGECONTAINERWND_CAPT_BEFORE_DY;
				rcCaption.top += __EXT_PAGECONTAINERWND_CAPT_AFTER_DY;
			} // if( bHorzLayout )
			else
			{
				rcCaption.DeflateRect(
					__EXT_PAGECONTAINERWND_CAPT_VBORDER_DX,
					__EXT_PAGECONTAINERWND_CAPT_VBORDER_DY
					);
				rcCaption.left += __EXT_PAGECONTAINERWND_CAPT_BEFORE_DX;
				rcCaption.right -= __EXT_PAGECONTAINERWND_CAPT_AFTER_DX;
			} // else from if( bHorzLayout )

			DWORD dwPageContainerStyle = GetPageContainerStyle();
			if( dwPageContainerStyle & __EPCWS_CAPTION_EXPBTN )
			{
				CExtPaintManager::glyph_t & _glyph =
					OnPageContainerQueryItemGlyph(
						pPageInfo,
						pPageInfo->IsExpanded()
						);
				CSize sizeGlyph = _glyph.Size();
				if( sizeGlyph.cx > 0 && sizeGlyph.cy > 0 )
				{
					CRect rcGlyph( rcCaption );
					if( bHorzLayout )
					{
						rcGlyph.left += (rcCaption.Width() - sizeGlyph.cx)/2;
						rcGlyph.top = rcGlyph.bottom - sizeGlyph.cy;
						rcGlyph.right = rcGlyph.left + sizeGlyph.cx;
						rcCaption.bottom = rcGlyph.top - __EXT_PAGECONTAINERWND_CAPT_ICON2TEXT_DY;
					} // if( bHorzLayout )
					else
					{
						rcGlyph.right = rcGlyph.left + sizeGlyph.cx;
						rcGlyph.top += (rcCaption.Height() - sizeGlyph.cy)/2;
						rcGlyph.bottom = rcGlyph.top + sizeGlyph.cy;
						rcCaption.left = rcGlyph.right + __EXT_PAGECONTAINERWND_CAPT_ICON2TEXT_DX;
					} // else from if( bHorzLayout )

					if( rcGlyph.PtInRect(_pht.m_ptClient) )
					{
						_pht.m_dwHitTestCode = __EPCHT_ON_PAGE_CAPT_GLYPH;
						return;
					}

				} // if( sizeGlyph.cx > 0 && sizeGlyph.cy > 0 )
			}

			CSize sizeIcon( 0, 0 );
			HICON hItemIcon =
				_pht.m_pPageInfo->IconGet( &sizeIcon );
			if( hItemIcon != NULL && sizeIcon.cx > 0 && sizeIcon.cy > 0 )
			{
				CRect rcIcon( rcCaption );
				if( bHorzLayout )
				{
					rcIcon.left += (rcCaption.Width() - sizeIcon.cx)/2;
					rcIcon.top = rcIcon.bottom - sizeIcon.cy;
					rcIcon.right = rcIcon.left + sizeIcon.cx;
					//rcCaption.bottom = rcIcon.top - __EXT_PAGECONTAINERWND_CAPT_ICON2TEXT_DY;
				} // if( bHorzLayout )
				else
				{
					rcIcon.right = rcIcon.left + sizeIcon.cx;
					rcIcon.top += (rcCaption.Height() - sizeIcon.cy)/2;
					rcIcon.bottom = rcIcon.top + sizeIcon.cy;
					//rcCaption.left = rcIcon.right + __EXT_PAGECONTAINERWND_CAPT_ICON2TEXT_DX;
				} // else from if( bHorzLayout )

				if(		rcIcon.right > rcIcon.left
					&&	rcIcon.bottom > rcIcon.top
					&&	rcIcon.PtInRect(_pht.m_ptClient)
					)
				{
					_pht.m_dwHitTestCode = __EPCHT_ON_PAGE_CAPT_ICON;
					return;
				}
			} // if( hItemIcon != NULL && sizeIcon.cx > 0 && sizeIcon.cy > 0 )

			return;
		} // if( rcCaption.PtInRect(_pht.m_ptClient) )
		
		bool bPageIsExpanded =
			pPageInfo->IsExpanded();
		if( !bPageIsExpanded )
		{
			if( bHorzLayout )
				_pht.m_dwHitTestCode = __EPCHT_ON_PAGE_BORDER_L;
			else
				_pht.m_dwHitTestCode = __EPCHT_ON_PAGE_BORDER_T;
			return;
		} // if( !bPageIsExpanded )

		if( bHorzLayout )
		{
			// test left border
			LONG nMargin =
				max(
					m_rcPageBorderSizes.left,
					rcCaption.Width()
					);
			if( nMargin > 0 )
			{
				CRect rcBorder( rcPage );
				rcBorder.right =
					rcBorder.left
					+ nMargin
					;
				if( rcBorder.PtInRect(_pht.m_ptClient) )
				{
					_pht.m_dwHitTestCode = __EPCHT_ON_PAGE_BORDER_L;
					return;
				}
			}
			// test top border
			if( m_rcPageBorderSizes.top > 0 )
			{
				CRect rcBorder( rcPage );
				rcBorder.bottom =
					rcBorder.top
					+ m_rcPageBorderSizes.top
					;
				if( rcBorder.PtInRect(_pht.m_ptClient) )
				{
					_pht.m_dwHitTestCode = __EPCHT_ON_PAGE_BORDER_T;
					return;
				}
			}
		} // if( bHorzLayout )
		else
		{
			// test top border
			LONG nMargin =
				max(
					m_rcPageBorderSizes.top,
					rcCaption.Height()
					);
			if( nMargin > 0 )
			{
				CRect rcBorder( rcPage );
				rcBorder.bottom =
					rcBorder.top
					+ nMargin
					;
				if( rcBorder.PtInRect(_pht.m_ptClient) )
				{
					_pht.m_dwHitTestCode = __EPCHT_ON_PAGE_BORDER_T;
					return;
				}
			}
			// test left border
			if( m_rcPageBorderSizes.left > 0 )
			{
				CRect rcBorder( rcPage );
				rcBorder.right =
					rcBorder.left
					+ m_rcPageBorderSizes.left
					;
				if( rcBorder.PtInRect(_pht.m_ptClient) )
				{
					_pht.m_dwHitTestCode = __EPCHT_ON_PAGE_BORDER_L;
					return;
				}
			}
		} // else from if( bHorzLayout )

		// test right border
		if( m_rcPageBorderSizes.right > 0 )
		{
			CRect rcBorder( rcPage );
			rcBorder.left =
				rcBorder.right
				- m_rcPageBorderSizes.right
				;
			if( rcBorder.PtInRect(_pht.m_ptClient) )
			{
				_pht.m_dwHitTestCode = __EPCHT_ON_PAGE_BORDER_R;
				return;
			}
		}
		// test bottom border
		if( m_rcPageBorderSizes.bottom > 0 )
		{
			CRect rcBorder( rcPage );
			rcBorder.top =
				rcBorder.bottom
				- m_rcPageBorderSizes.bottom
				;
			if( rcBorder.PtInRect(_pht.m_ptClient) )
			{
				_pht.m_dwHitTestCode = __EPCHT_ON_PAGE_BORDER_B;
				return;
			}
		}

		_pht.m_dwHitTestCode = __EPCHT_ON_PAGE_WND;
		return;
	}
}

LONG CExtPageContainerWnd::PageGetNext(
	LONG nStartIndex, // = -1L
	bool bOnlyExpanded, // = false
	bool bOnlyVisible, // = false
	bool bNext, // = true
	bool bOnlyEnabled // = false
	) const
{
	ASSERT_VALID( this );
LONG nPageCount = PageGetCount();
	if( nPageCount == 0 )
		return -1L;

LONG nStep = 1L, nIndex = nStartIndex, nEndIndex = nPageCount-1;
	if( bNext )
	{ // if next - with greater index
		if( nIndex < 0 )
			nIndex = 0;
		else
			nIndex++;
		if( nIndex > nEndIndex )
			return -1L;
	} // if next - with greater index
	else
	{ // if previous - with less index
		nStep = -1L;
		nEndIndex = 0;
		if( nIndex < 0 || nIndex >= nPageCount )
			nIndex = nPageCount-1;
		else
			nIndex--;
		if( nIndex < nEndIndex )
			return -1L;
	} // if previous - with less index

	for( ; true; )
	{
		ASSERT( nIndex >= 0 && nIndex < nPageCount );
		const PAGE_ITEM_INFO * pPageInfo = PageGetInfo( nIndex );
		ASSERT( pPageInfo != NULL );
		bool bPassed = true;
		
		if( bOnlyVisible && (!pPageInfo->IsVisible()) )
			bPassed = false;

		if( bPassed && bOnlyExpanded && (!pPageInfo->IsExpanded()) )
			bPassed = false;

		if( bPassed && bOnlyEnabled )
		{
			HWND hWndPage = pPageInfo->GetSafePageHwnd();
			ASSERT( hWndPage != NULL );
			ASSERT( ::IsWindow( hWndPage ) );
			bool bEnabled = ( ::IsWindowEnabled( hWndPage ) ) ? true : false;
			if( !bEnabled )
				bPassed = false;
		} // if( bPassed && bOnlyEnabled )

		if( bPassed )
			return nIndex;

		if( nIndex == nEndIndex )
			break;
		nIndex += nStep;
	}

	return -1L;
}

/////////////////////////////////////////////////////////////////////////////
// CExtPageContainerWnd message handlers

void CExtPageContainerWnd::PreSubclassWindow() 
{
	CWnd::PreSubclassWindow();

	if( m_bDirectCreateCall )
		return;

DWORD dwStyle = ::GetWindowLong( m_hWnd, GWL_STYLE );
	m_dwPageContainerStyle = dwStyle & __EXTMFC_ALL_FORM_MOVABLE_WND_STYLES;
	::SetWindowLong( m_hWnd, GWL_STYLE, dwStyle & (~__EXTMFC_ALL_FORM_MOVABLE_WND_STYLES) );
	
	if( !_CreateHelper() )
	{
		ASSERT( FALSE );
		AfxThrowMemoryException();
	} // if( !Create() )
}

BOOL CExtPageContainerWnd::PreTranslateMessage(MSG* pMsg) 
{
	if(		(m_dwPageContainerStyle & __EPCWS_CAPTION_TOOLTIPS) != 0
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

BOOL CExtPageContainerWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	if( ( !RegisterPageContainerWndClass() )
		|| ( !CWnd::PreCreateWindow(cs) )
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
	
	cs.lpszClass = __EXT_PAGECONTAINERWND_CLASS_NAME;

	return TRUE;
	
//	return CWnd::PreCreateWindow(cs);
}

void CExtPageContainerWnd::PostNcDestroy() 
{
	if( m_hWndEditor != NULL )
	{
		OnPageContainerCancelItemEditor( m_hWndEditor );
		m_hWndEditor = NULL;
	}

	m_wndContentExpand.Deactivate();
	PageRemoveAll( false, false );
	
	CWnd::PostNcDestroy();
}

LRESULT CExtPageContainerWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch( message )
	{
	case WM_DESTROY:
	case WM_NCDESTROY:
		PageRemoveAll( false, false );
	break;
	default:
	{
		if( m_bDelayRecalcLayout )
		{
			CWinApp * pApp = AfxGetApp();
				ASSERT( pApp != NULL );
			BOOL bIdleMsg =
				pApp->IsIdleMessage(
#if _MFC_VER < 0x700
					&pApp->m_msgCur
#else
					&(::AfxGetThreadState()->m_msgCur)
#endif
					);
			if( bIdleMsg )
				UpdatePageContainerWnd( true );
		} // if( m_bDelayRecalcLayout )
	}
	break;
	}
	
	return CWnd::WindowProc(message, wParam, lParam);
}

void CExtPageContainerWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
DWORD dwWndStyle = GetStyle();
	if( (dwWndStyle & WS_VISIBLE) == 0 )
		return;

	_CancelActions();

	UpdatePageContainerWnd( true );
}

void CExtPageContainerWnd::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CWnd::OnWindowPosChanged(lpwndpos);
	
DWORD dwWndStyle = GetStyle();
	if( (dwWndStyle & WS_VISIBLE) == 0 )
		return;

	_CancelActions();

	UpdatePageContainerWnd( true );
}

void CExtPageContainerWnd::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
	
DWORD dwWndStyle = GetStyle();
	if( (dwWndStyle & WS_VISIBLE) == 0 )
		return;

	_CancelActions();

	UpdatePageContainerWnd( true );
}

#if _MFC_VER < 0x700
void CExtPageContainerWnd::OnActivateApp(BOOL bActive, HTASK hTask) 
#else
void CExtPageContainerWnd::OnActivateApp(BOOL bActive, DWORD hTask) 
#endif
{
	CWnd::OnActivateApp(bActive, hTask);
	
	if( !bActive )
		_CancelActions();
}

LRESULT CExtPageContainerWnd::_OnPaintExpanedItemContent(WPARAM wParam, LPARAM lParam)
{
	lParam;
CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA * p_picd =
		(CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA *)wParam;
	ASSERT( p_picd != NULL );
	ASSERT( p_picd->m_dc.GetSafeHdc() != NULL );

	if( m_pPageInfoHover == NULL )
		return 0;
	OnPageContainerDrawCaption(
		p_picd->m_dc,
		m_pPageInfoHover,
		p_picd->m_rcItem
		);
	return (!0);
}

void CExtPageContainerWnd::OnPaint() 
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
	OnPageContainerDrawEntire(
		dc,
		rcClient
		);
}

BOOL CExtPageContainerWnd::OnEraseBkgnd(CDC* pDC) 
{
//	return CWnd::OnEraseBkgnd(pDC);
	pDC;
	return TRUE;
}

void CExtPageContainerWnd::OnPageContainerEraseEntire(
	CDC & dc,
	CRect rcClient
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	
	if( !dc.RectVisible( &rcClient ) )
		return;
	// - 2.26
//COLORREF clrBk =
//		g_PaintManager->GetColor(
//			CExtPaintManager::CLR_3DFACE_OUT // COLOR_3DFACE
//			);
//	dc.FillSolidRect( &rcClient, clrBk );
	// + 2.26
	if( g_PaintManager->GetCb2DbTransparentMode(this) )
		g_PaintManager->PaintDockerBkgnd( dc, this );
	else
		dc.FillSolidRect(
			&rcClient,
			g_PaintManager->GetColor(
				CExtPaintManager::CLR_3DFACE_OUT
				)
			);
}

void CExtPageContainerWnd::OnPageContainerDrawEntire(
	CDC & dc,
	CRect rcClient
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

	if( !dc.RectVisible( &rcClient ) )
		return;
	OnPageContainerEraseEntire( dc, rcClient );

LONG nPageCount = PageGetCount();
	for( LONG nIndex = 0; nIndex < nPageCount; nIndex++ )
	{
		PAGE_ITEM_INFO * pPageInfo = PageGetInfo( nIndex );
		ASSERT( pPageInfo != NULL );
		if( !pPageInfo->IsVisible() )
			continue;
		CRect rcPage, rcCaption, rcWnd;
		PageGetLayoutRectInfo(
			nIndex,
			&rcPage,
			&rcCaption,
			&rcWnd
			);
		OnPageContainerDrawItem(
			dc,
			pPageInfo,
			rcPage,
			rcCaption,
			rcWnd
			);
	} // for( LONG nIndex = 0; nIndex < nPageCount; nIndex++ )

	if( !m_rcScrollPosArea.IsRectEmpty() )
	{
		OnPageContainerEraseScrollPosArea(
			dc,
			m_rcScrollPosArea,
			m_nScrollPos,
			m_nScrollSize
			);
		OnPageContainerDrawScrollPosArea(
			dc,
			m_rcScrollPosArea,
			m_nScrollPos,
			m_nScrollSize
			);
	} // if( !m_rcScrollPosArea.IsRectEmpty() )
}

void CExtPageContainerWnd::OnPageContainerEraseScrollPosArea(
	CDC & dc,
	CRect rcScrollPosArea,
	LONG nScrollPos,
	LONG nScrollSize
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( nScrollPos <= nScrollSize );
	nScrollPos;
	nScrollSize;
	
	if(		(rcScrollPosArea.right - rcScrollPosArea.left) <= 1
		||	(rcScrollPosArea.bottom - rcScrollPosArea.top) <= 1
		)
		return;

	if( !dc.RectVisible(&rcScrollPosArea) )
		return;
	
COLORREF clrLight = g_PaintManager->GetColor( COLOR_3DHIGHLIGHT );
COLORREF clrShadow = g_PaintManager->GetColor( COLOR_3DDKSHADOW );
COLORREF clrFace = g_PaintManager->GetColor( COLOR_3DSHADOW );
	dc.FillSolidRect(
		&rcScrollPosArea,
		clrFace
		);
	dc.Draw3dRect(
		&rcScrollPosArea,
		clrShadow,
		clrLight
		);
}

void CExtPageContainerWnd::OnPageContainerDrawScrollPosArea(
	CDC & dc,
	CRect rcScrollPosArea,
	LONG nScrollPos,
	LONG nScrollSize
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( nScrollPos <= nScrollSize );
	 
	if(		(rcScrollPosArea.right - rcScrollPosArea.left) <= 1
		||	(rcScrollPosArea.bottom - rcScrollPosArea.top) <= 1
		)
		return;

	if( !dc.RectVisible(&rcScrollPosArea) )
		return;

CRect rcScrollBtn( rcScrollPosArea );
	rcScrollBtn.DeflateRect( 1, 1 );
	if(		(rcScrollBtn.right - rcScrollBtn.left) <= 1
		||	(rcScrollBtn.bottom - rcScrollBtn.top) <= 1
		)
		return;
	if( nScrollSize > 0 )
	{
		ASSERT( nScrollPos <= nScrollSize );
		bool bHorzLayout = IsHorizontalLayout();

		LONG nScrollPecrent =
			::MulDiv(
				nScrollPos,
				100,
				nScrollSize
				);
		LONG nWindowMetric =
			bHorzLayout
				? rcScrollPosArea.Width()
				: rcScrollPosArea.Height()
				;
		LONG nBtnMetric =
			::MulDiv(
				nWindowMetric,
				100,
				( nScrollSize + nWindowMetric )
				);
		nBtnMetric =
			::MulDiv(
				nBtnMetric,
				nWindowMetric,
				100
				);
		if( nBtnMetric < 10L )
			nBtnMetric = 10L;
		LONG nBtnMetricMax =
			bHorzLayout
				? rcScrollBtn.Width()
				: rcScrollBtn.Height()
				;
		LONG nBtnOffset = 0L;
		if( nBtnMetric >= nBtnMetricMax
			|| nBtnMetric >= ( nWindowMetric - 2 )
			)
			nBtnMetric =
				min(
					nBtnMetricMax,
					( nWindowMetric - 2 )
					);
		else
		{
			nBtnOffset =
				::MulDiv(
					nWindowMetric - nBtnMetric,
					nScrollPecrent,
					100
					);
		} // else from if( nBtnMetric >= nBtnMetricMax )

		if( bHorzLayout )
		{
			rcScrollBtn.left += nBtnOffset;
			rcScrollBtn.right =
				rcScrollBtn.left + nBtnMetric;
			if( (rcScrollBtn.right+1) >= rcScrollPosArea.right )
				rcScrollBtn.right =
					rcScrollPosArea.right - 1;
		} // if( bHorzLayout )
		else
		{
			rcScrollBtn.top += nBtnOffset;
			rcScrollBtn.bottom =
				rcScrollBtn.top + nBtnMetric;
			if( (rcScrollBtn.bottom+1) >= rcScrollPosArea.bottom )
				rcScrollBtn.bottom =
					rcScrollPosArea.bottom - 1;
		} // else from if( bHorzLayout )
	
	} // if( nScrollSize > 0 )

COLORREF clrLight = g_PaintManager->GetColor( COLOR_3DHIGHLIGHT );
COLORREF clrShadow = g_PaintManager->GetColor( COLOR_3DDKSHADOW );
COLORREF clrFace = g_PaintManager->GetColor( COLOR_3DFACE );
	dc.FillSolidRect(
		&rcScrollBtn,
		clrFace
		);
	dc.Draw3dRect(
		&rcScrollBtn,
		clrLight,
		clrShadow
		);
}

void CExtPageContainerWnd::OnPageContainerEraseItem(
	CDC & dc,
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo,
	CRect rcPage,
	CRect rcCaption,
	CRect rcWnd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pPageInfo != NULL );
	ASSERT( pPageInfo->IsVisible() );
	pPageInfo;
	rcCaption;

	if(		rcPage.left >= rcPage.right
		||	rcPage.top >= rcPage.bottom
		)
		return;

	if( !dc.RectVisible( &rcPage ) )
		return;
	// - 2.26
//COLORREF clrBk = g_PaintManager->GetColor( COLOR_3DFACE );
//	dc.FillSolidRect( &rcPage, clrBk );

DWORD dwPageContainerStyle = GetPageContainerStyle();
	if( (dwPageContainerStyle & __EPCWS_PAGE_BORDERS) == 0 )
		return;
bool bPageIsExpanded = pPageInfo->IsExpanded();
	if( !bPageIsExpanded )
		return;

	ASSERT(
			rcPage.left <= rcWnd.left
		&&	rcPage.top <= rcWnd.top
		&&	rcPage.right >= rcWnd.right
		&&	rcPage.bottom >= rcWnd.bottom
		);

CRect rcMiddle(
		(rcPage.left + rcWnd.left) / 2,
		(rcPage.top + rcWnd.top) / 2,
		(rcPage.right + rcWnd.right) / 2,
		(rcPage.bottom + rcWnd.bottom) / 2
	   );

	if(		rcMiddle.left < rcMiddle.right
		&&	rcMiddle.top < rcMiddle.bottom
		)
	{
		if( m_dwPageContainerStyle & __EPCWS_CAPTION_FLAT
			&& !pPageInfo->IsCaptionHover()
			&& !pPageInfo->IsCaptionPressed()
			)
		{
			HWND hWndPage = pPageInfo->GetSafePageHwnd();
			ASSERT( hWndPage != NULL );
			ASSERT( ::IsWindow( hWndPage ) );
			bool bEnabled = ( ::IsWindowEnabled( hWndPage ) ) ? true : false;

			COLORREF clrFrame =
				g_PaintManager->GetColor(
					bEnabled
						? COLOR_3DDKSHADOW
						: COLOR_3DSHADOW
					);
			dc.Draw3dRect(
				&rcMiddle,
				clrFrame,
				clrFrame
				);
		}
		else
		{
			COLORREF clrLight = g_PaintManager->GetColor( COLOR_3DHIGHLIGHT );
			COLORREF clrShadow = g_PaintManager->GetColor( COLOR_3DSHADOW );
			dc.Draw3dRect(
				&rcMiddle,
				clrShadow,
				clrLight
				);
			rcMiddle.DeflateRect( 1, 1 );
			dc.Draw3dRect(
				&rcMiddle,
				clrLight,
				clrShadow
				);
		}
	}

}

void CExtPageContainerWnd::OnPageContainerDrawItem(
	CDC & dc,
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo,
	CRect rcPage,
	CRect rcCaption,
	CRect rcWnd
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pPageInfo != NULL );
	ASSERT( pPageInfo->IsVisible() );
	rcWnd;

	if(		rcPage.left >= rcPage.right
		||	rcPage.top >= rcPage.bottom
		)
		return;

	if( !dc.RectVisible( &rcPage ) )
		return;

	OnPageContainerEraseItem(
		dc,
		pPageInfo,
		rcPage,
		rcCaption,
		rcWnd
		);

	OnPageContainerDrawCaption(
		dc,
		pPageInfo,
		rcCaption
		);
}

void CExtPageContainerWnd::OnPageContainerEraseCaption(
	CDC & dc,
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo,
	CRect rcCaption
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pPageInfo != NULL );
	ASSERT( pPageInfo->IsVisible() );
	pPageInfo;

	if(		(rcCaption.right - rcCaption.left) <= 4 
		||	(rcCaption.bottom - rcCaption.top) <= 4 
		)
		return;

	if( !dc.RectVisible( &rcCaption ) )
		return;

COLORREF clrFace = g_PaintManager->GetColor(
			CExtPaintManager::CLR_3DFACE_OUT // COLOR_3DFACE
		);

	dc.FillSolidRect(
		&rcCaption,
		clrFace
		);

//HWND hWndPage = pPageInfo->GetSafePageHwnd();
//	ASSERT( hWndPage != NULL );
//	ASSERT( ::IsWindow( hWndPage ) );
//bool bEnabled = ( ::IsWindowEnabled( hWndPage ) ) ? true : false;
//	g_PaintManager->PaintPushButton(
//		dc,
//		true, // bHorz
//		rcCaption,
//		NULL, // sText
//		NULL, // hIcon
//		(m_dwPageContainerStyle & __EPCWS_CAPTION_FLAT) ? true : false, //bool bFlat
//		pPageInfo->IsCaptionHover(),
//		pPageInfo->IsCaptionPressed(),
//		false, // bIndeterminate
//		bEnabled,
//		true, // bDrawBorder
//		false, // bDrawFocusRect
//		false,
//		CExtPaintManager::__ALIGN_HORIZ_CENTER,
//		NULL,
//		false,
//		0,
//		false
//		);
}

void CExtPageContainerWnd::OnPageContainerDrawCaption(
	CDC & dc,
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo,
	CRect rcCaption
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pPageInfo != NULL );
	ASSERT( pPageInfo->IsVisible() );

	if(		(rcCaption.right - rcCaption.left) <= 4 
		||	(rcCaption.bottom - rcCaption.top) <= 4 
		)
		return;

	if( !dc.RectVisible( &rcCaption ) )
		return;

	// erase caption
	OnPageContainerEraseCaption(
		dc,
		pPageInfo,
		rcCaption
		);

	// draw caption

CRgn rgnCaption;
	if( rgnCaption.CreateRectRgnIndirect(&rcCaption) )
	{
		ASSERT( rgnCaption.GetSafeHandle() != NULL );
		dc.SelectClipRgn( &rgnCaption );
	}

HWND hWndPage = pPageInfo->GetSafePageHwnd();
	ASSERT( hWndPage != NULL );
	ASSERT( ::IsWindow( hWndPage ) );
bool bEnabled = ( ::IsWindowEnabled( hWndPage ) ) ? true : false;

COLORREF clrLight = g_PaintManager->GetColor( COLOR_3DHIGHLIGHT );
COLORREF clrFace = g_PaintManager->GetColor(
		CExtPaintManager::CLR_3DFACE_OUT // COLOR_3DFACE
		);
COLORREF clrShadow = g_PaintManager->GetColor( COLOR_3DSHADOW );
COLORREF clrDkShadow = g_PaintManager->GetColor( COLOR_3DDKSHADOW );

//	dc.FillSolidRect(
//		&rcCaption,
//		clrFace
//		);

CRect rcCaptBorder( rcCaption );
bool bHorzLayout = IsHorizontalLayout();

	if( bHorzLayout )
	{
		rcCaption.DeflateRect(
			__EXT_PAGECONTAINERWND_CAPT_HBORDER_DX,
			__EXT_PAGECONTAINERWND_CAPT_HBORDER_DY
			);
		rcCaption.bottom -= __EXT_PAGECONTAINERWND_CAPT_BEFORE_DY;
		rcCaption.top += __EXT_PAGECONTAINERWND_CAPT_AFTER_DY;
	} // if( bHorzLayout )
	else
	{
		rcCaption.DeflateRect(
			__EXT_PAGECONTAINERWND_CAPT_VBORDER_DX,
			__EXT_PAGECONTAINERWND_CAPT_VBORDER_DY
			);
		rcCaption.left += __EXT_PAGECONTAINERWND_CAPT_BEFORE_DX;
		rcCaption.right -= __EXT_PAGECONTAINERWND_CAPT_AFTER_DX;
	} // else from if( bHorzLayout )

DWORD dwPageContainerStyle = GetPageContainerStyle();
	if( dwPageContainerStyle & __EPCWS_CAPTION_EXPBTN )
	{
		CExtPaintManager::glyph_t & _glyph =
			OnPageContainerQueryItemGlyph(
				pPageInfo,
				pPageInfo->IsExpanded()
				);
		CSize sizeGlyph = _glyph.Size();
		if( sizeGlyph.cx > 0 && sizeGlyph.cy > 0 )
		{
			CRect rcGlyph( rcCaption );
			if( bHorzLayout )
			{
				rcGlyph.left += (rcCaption.Width() - sizeGlyph.cx)/2;
				rcGlyph.top = rcGlyph.bottom - sizeGlyph.cy;
				rcGlyph.right = rcGlyph.left + sizeGlyph.cx;
				rcCaption.bottom = rcGlyph.top - __EXT_PAGECONTAINERWND_CAPT_ICON2TEXT_DY;
			} // if( bHorzLayout )
			else
			{
				rcGlyph.right = rcGlyph.left + sizeGlyph.cx;
				rcGlyph.top += (rcCaption.Height() - sizeGlyph.cy)/2;
				rcGlyph.bottom = rcGlyph.top + sizeGlyph.cy;
				rcCaption.left = rcGlyph.right + __EXT_PAGECONTAINERWND_CAPT_ICON2TEXT_DX;
			} // else from if( bHorzLayout )

			COLORREF ColorValues[] =
			{
				RGB(0,0,0),
				g_PaintManager->GetColor(
					bEnabled
						? COLOR_BTNTEXT
						: COLOR_3DSHADOW
					)
			};
			g_PaintManager->PaintGlyphCentered(
				dc,
				rcGlyph,
				_glyph,
				ColorValues
				);
		} // if( sizeGlyph.cx > 0 && sizeGlyph.cy > 0 )
//		if( bHorzLayout )
//		{
//			if( sizeGlyph.cy > 0 )
//				rcCaption.top +=
//					sizeGlyph.cy
//					+ __EXT_PAGECONTAINERWND_CAPT_AFTER_GLYPH_DY
//					;
//		} // if( bHorzLayout )
//		else
//		{
//			if( sizeGlyph.cx > 0 )
//				rcCaption.left +=
//					sizeGlyph.cx
//					+ __EXT_PAGECONTAINERWND_CAPT_AFTER_GLYPH_DX
//					;
//		} // else from if( bHorzLayout )
	}

CSize sizeIcon( 0, 0 );
HICON hItemIcon = pPageInfo->IconGet( &sizeIcon );
	if( hItemIcon != NULL && sizeIcon.cx > 0 && sizeIcon.cy > 0 )
	{
		CRect rcIcon( rcCaption );
		if( bHorzLayout )
		{
			rcIcon.left += (rcCaption.Width() - sizeIcon.cx)/2;
			rcIcon.top = rcIcon.bottom - sizeIcon.cy;
			rcIcon.right = rcIcon.left + sizeIcon.cx;
			rcCaption.bottom = rcIcon.top - __EXT_PAGECONTAINERWND_CAPT_ICON2TEXT_DY;
		} // if( bHorzLayout )
		else
		{
			rcIcon.right = rcIcon.left + sizeIcon.cx;
			rcIcon.top += (rcCaption.Height() - sizeIcon.cy)/2;
			rcIcon.bottom = rcIcon.top + sizeIcon.cy;
			rcCaption.left = rcIcon.right + __EXT_PAGECONTAINERWND_CAPT_ICON2TEXT_DX;
		} // else from if( bHorzLayout )

		if(		rcIcon.right > rcIcon.left
			&&	rcIcon.bottom > rcIcon.top
			)
			dc.DrawState(
				rcIcon.TopLeft(),
				rcIcon.Size(), 
				hItemIcon, 
				bEnabled ? DSS_NORMAL : DSS_DISABLED, 
				(CBrush*)NULL
				);
	} // if( hItemIcon != NULL && sizeIcon.cx > 0 && sizeIcon.cy > 0 )

bool bCenteredText =
		(dwPageContainerStyle & __EPCWS_CAPTION_CENTER_TEXT)
			? true :  false;

__EXT_MFC_SAFE_LPCTSTR sItemText = pPageInfo->TextGet();
LONG nItemTextLen = 0;
	if(		sItemText != NULL
		&&	( nItemTextLen = _tcslen( sItemText ) ) > 0
		&&	rcCaption.right > rcCaption.left
		&&	rcCaption.bottom > rcCaption.top
		)
	{ // if can draw text
		CFont * pItemFont = pPageInfo->GetCaptionFont( bHorzLayout );
		ASSERT( pItemFont != NULL );
		ASSERT( pItemFont->GetSafeHandle() != NULL );
		CFont * pOldFont = dc.SelectObject( pItemFont );
		COLORREF clrTextOld =
			dc.SetTextColor(
				g_PaintManager->GetColor(
					bEnabled ? COLOR_BTNTEXT : COLOR_3DHILIGHT
					)
				);
		int nOldBkMode = dc.SetBkMode( TRANSPARENT );
		if( bHorzLayout )
		{
			rcCaption.InflateRect( 3, 0 );
			CSize sizeTextMeasured = pPageInfo->GetLastMeasuredTextSize();
			if( bCenteredText )
			{
				UINT nOldTA = dc.SetTextAlign(
					TA_CENTER | TA_BASELINE
					);
				rcCaption.OffsetRect(
					( (dwPageContainerStyle & __EPCWS_CAPTION_INV_VFONT) == 0 )
						?   sizeTextMeasured.cy/2
						: - sizeTextMeasured.cy/2
						,
					0
					);
				CPoint ptCenter = rcCaption.CenterPoint();
				dc.ExtTextOut(
					ptCenter.x,
					ptCenter.y,
					ETO_CLIPPED,
					&rcCaption,
					sItemText,
					nItemTextLen,
					NULL
					);
				if( !bEnabled )
				{
					dc.SetTextColor(
						g_PaintManager->GetColor(COLOR_3DSHADOW)
						);
					ptCenter.x--;
					ptCenter.y--;
					rcCaption.OffsetRect( -1, -1 );
					dc.ExtTextOut(
						ptCenter.x,
						ptCenter.y,
						ETO_CLIPPED,
						&rcCaption,
						sItemText,
						nItemTextLen,
						NULL
						);
				} // if( !bEnabled )
				dc.SetTextAlign( nOldTA );
			} // if( bCenteredText )
			else
			{
				UINT nOldTA = dc.SetTextAlign(
					TA_TOP | TA_BASELINE
					);
				rcCaption.OffsetRect(
					( (dwPageContainerStyle & __EPCWS_CAPTION_INV_VFONT) == 0 )
						?   sizeTextMeasured.cy/2
						: - sizeTextMeasured.cy/2
						,
					0
					);
				CPoint ptCenter = rcCaption.CenterPoint();
				if( (dwPageContainerStyle & __EPCWS_CAPTION_INV_VFONT) == 0 )
					ptCenter.y =
						rcCaption.bottom - 4
						- (rcCaption.Height() - sizeTextMeasured.cx)
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
					nItemTextLen,
					NULL
					);
				if( !bEnabled )
				{
					dc.SetTextColor(
						g_PaintManager->GetColor(COLOR_3DSHADOW)
						);
					ptCenter.x--;
					ptCenter.y--;
					rcCaption.OffsetRect( -1, -1 );
					dc.ExtTextOut(
						ptCenter.x,
						ptCenter.y,
						ETO_CLIPPED,
						&rcCaption,
						sItemText,
						nItemTextLen,
						NULL
						);
				} // if( !bEnabled )
				dc.SetTextAlign( nOldTA );
			} // else from if( bCenteredText )
		
		} // if( bHorzLayout )
		else
		{
			UINT nFormat =
				DT_SINGLELINE|DT_END_ELLIPSIS;
			if( bCenteredText )
			{
				nFormat |= DT_CENTER|DT_VCENTER;
			} // if( bCenteredText )
			else
			{
				nFormat |= DT_VCENTER|DT_LEFT;
			} // else from if( bCenteredText )
			dc.DrawText(
				sItemText,
				nItemTextLen,
				&rcCaption,
				nFormat
				);
			if( !bEnabled )
			{
				dc.SetTextColor(
					g_PaintManager->GetColor(COLOR_3DSHADOW)
					);
				rcCaption.OffsetRect( -1, -1 );
				dc.DrawText(
					sItemText,
					nItemTextLen,
					&rcCaption,
					nFormat
					);
			} // if( !bEnabled )
		} // else from if( bHorzLayout )
		dc.SetBkMode( nOldBkMode );
		dc.SetTextColor( clrTextOld );
		dc.SelectObject( pOldFont );
	} // if can draw text

	if( m_dwPageContainerStyle & __EPCWS_CAPTION_FLAT )
	{ // if flat captions
		if( pPageInfo->IsCaptionPressed() )
		{ // if caption is pressed
			dc.Draw3dRect(
				&rcCaptBorder,
				clrShadow,
				clrLight
				);
		} // if caption is pressed
		else if( pPageInfo->IsCaptionHover() )
		{ // if caption is under mouse pointer
			dc.Draw3dRect(
				&rcCaptBorder,
				clrLight,
				clrShadow
				);
		} // if caption is under mouse pointer
		else
		{ // if caption is neither pressed nor under mouse pointer
			dc.Draw3dRect(
				&rcCaptBorder,
				bEnabled ? clrDkShadow : clrShadow,
				bEnabled ? clrDkShadow : clrShadow
				);
		} // if caption is neither pressed nor under mouse pointer
	} // if flat captions
	else
	{ // if semi-flat captions
		if( pPageInfo->IsCaptionPressed() )
		{ // if caption is pressed
			dc.Draw3dRect(
				&rcCaptBorder,
				clrDkShadow,
				clrLight
				);
			rcCaptBorder.DeflateRect( 1, 1 );
			dc.Draw3dRect(
				&rcCaptBorder,
				clrShadow,
				clrFace
				);
		} // if caption is pressed
		else if( pPageInfo->IsCaptionHover() )
		{ // if caption is under mouse pointer
			dc.Draw3dRect(
				&rcCaptBorder,
				clrLight,
				clrDkShadow
				);
			rcCaptBorder.DeflateRect( 1, 1 );
			dc.Draw3dRect(
				&rcCaptBorder,
				clrFace,
				clrShadow
				);
		} // if caption is under mouse pointer
		else
		{ // if caption is neither pressed nor under mouse pointer
			dc.Draw3dRect(
				&rcCaptBorder,
				clrLight,
				clrShadow
				);
		} // if caption is neither pressed nor under mouse pointer
	} // if semi-flat captions

	dc.SelectClipRgn( NULL );
}

void CExtPageContainerWnd::OnCancelMode() 
{
	CWnd::OnCancelMode();
	
	if( m_bCancelingActionsMode )
		return;

	m_bCancelingActionsMode = true;

	if( m_hWndEditor != NULL )
	{
		OnPageContainerCancelItemEditor( m_hWndEditor );
		m_hWndEditor = NULL;
		//UpdatePageContainerWnd( false );
		Invalidate();
	}

	if(		(m_dwPageContainerStyle & __EPCWS_CAPTION_TOOLTIPS) != 0
		&&	m_wndToolTip.GetSafeHwnd() != NULL
		&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
		)
		m_wndToolTip.DelTool( this, 1 );
	CWnd::CancelToolTips();

	m_wndContentExpand.Deactivate();

	if( m_pPageInfoHover != NULL )
	{
		OnPageContainerCaptionHoverStop(
			m_pPageInfoHover
			);
	}

	if( m_pPageInfoPressed != NULL )
	{
		OnPageContainerCaptionPressedStop(
			m_pPageInfoPressed
			);
	}
	
	m_ptStartLeftBtnTrack.x = m_ptStartLeftBtnTrack.y = -1;
	if( m_bDragScrolling )
	{
		m_bDragScrolling = false;
		m_nScrollDragStart = 0L;
		UpdatePageContainerWnd( false );
	}

	if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
		CExtMouseCaptureSink::ReleaseCapture();

	m_bCancelingActionsMode = false;

	if( m_bDelayRecalcLayout )
		UpdatePageContainerWnd( true );
	else
		UpdateWindow();
}

void CExtPageContainerWnd::OnPageContainerCaptionPressedStart(
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo
	)
{
	ASSERT_VALID( this );
	ASSERT( pPageInfo != NULL );
	ASSERT( pPageInfo->IsVisible() );

	_CancelActions();

	if( m_dwPageContainerStyle & __EPCWS_CAPTION_NO_CLICKS  )
		return; // if do not expand/contract pages by user actions

	m_pPageInfoPressed = pPageInfo;
	m_pPageInfoPressed->m_bCaptionPressed = true;
	CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );

	UpdatePageContainerWnd( true );
}

void CExtPageContainerWnd::OnPageContainerCaptionPressedStop(
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo
	)
{
	ASSERT_VALID( this );
	ASSERT( pPageInfo != NULL );
	//ASSERT( pPageInfo->IsVisible() );
	pPageInfo;

	if( m_pPageInfoPressed != NULL )
	{
		m_pPageInfoPressed->m_bCaptionPressed = false;
		m_pPageInfoPressed = NULL;
		UpdatePageContainerWnd( true );
		if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
			CExtMouseCaptureSink::ReleaseCapture();

		//UpdatePageContainerWnd( false );
		//_CancelActions();
		//ASSERT( m_pPageInfoPressed == NULL );
	}
}

bool CExtPageContainerWnd::OnPageContainerCanDoHoverActions()
{
	ASSERT_VALID( this );

	if( m_hWndEditor != NULL )
		return false;

	return
		CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND(
			GetSafeHwnd()
			);
}

void CExtPageContainerWnd::OnPageContainerActivateContentExpandWnd(
	CExtContentExpandWnd & wndContentExpand,
	const RECT & rcItemLess, // in client
	const RECT & rcItemGreater, // in client
	DWORD dwCoockieCmd, // values [0..1000] are reserved
	LPARAM lParam
	)
{
	dwCoockieCmd;
	lParam;
	rcItemLess;
	ASSERT(
			rcItemLess.left < rcItemLess.right
		&&	rcItemLess.top < rcItemLess.bottom
		);
	ASSERT(
			rcItemGreater.left < rcItemGreater.right
		&&	rcItemGreater.top < rcItemGreater.bottom
		);
	wndContentExpand.Activate(
		rcItemGreater,
		this
		);
}

void CExtPageContainerWnd::OnPageContainerCaptionHoverStart(
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo
	)
{
	ASSERT_VALID( this );
	ASSERT( pPageInfo != NULL );
	ASSERT( pPageInfo->IsVisible() );

	_CancelActions();

	m_pPageInfoHover = pPageInfo;

	if( m_dwPageContainerStyle & __EPCWS_CAPTION_HOVER  )
		m_pPageInfoHover->m_bCaptionHover = true;

	CRect rcCaptionInClient( 0,0,0,0 );
	PageGetLayoutRectInfo(
		PageGetIndexOf(m_pPageInfoHover),
		NULL,
		&rcCaptionInClient
		);
	InvalidateRect( &rcCaptionInClient );

	SetTimer(
		__EXT_PAGECONTAINERWND_HOVER_TIMER_ID,
		__EXT_PAGECONTAINERWND_HOVER_TIMER_PERIOD,
		NULL
		);

	if( m_dwPageContainerStyle & (__EPCWS_CAPTION_EXPAND|__EPCWS_CAPTION_TOOLTIPS) )
	{ // if expand captions or show tooltips
		bool bHorzLayout = IsHorizontalLayout();
		bool bContentVisiblePartially = false;
		const CRect & rcCaption =
			m_pPageInfoHover->GetRectCaption();
		CSize sizeCaptionReal =
			rcCaption.Size();
		CSize sizeCaptionLastMeasured =
			m_pPageInfoHover->GetLastMeasuredCaptionSize();	
		if( bHorzLayout )
		{
			if( sizeCaptionReal.cy < sizeCaptionLastMeasured.cy /*-2*/ )
				bContentVisiblePartially = true;
		} // if( bHorzLayout )
		else
		{
			if( sizeCaptionReal.cx < sizeCaptionLastMeasured.cx /*-4*/ )
				bContentVisiblePartially = true;
		} // else from if( bHorzLayout )

		if( bContentVisiblePartially && (!rcCaptionInClient.IsRectEmpty()) )
		{
			if( m_dwPageContainerStyle & __EPCWS_CAPTION_EXPAND )
			{ // if expand captions
				CRect rcItemGreater( rcCaptionInClient );
				if( bHorzLayout )
					rcItemGreater.bottom =
						rcItemGreater.top
						+ sizeCaptionLastMeasured.cy
						;
				else
					rcItemGreater.right =
						rcItemGreater.left
						+ sizeCaptionLastMeasured.cx
						;
				OnPageContainerActivateContentExpandWnd(
					m_wndContentExpand,
					rcCaptionInClient,
					rcItemGreater,
					0,
					0
					);
			} // if expand captions
			else if( m_dwPageContainerStyle & __EPCWS_CAPTION_TOOLTIPS  )
			{ // if show tooltip on captions captions
				__EXT_MFC_SAFE_LPCTSTR sCaptionText = m_pPageInfoHover->TextGet();
				if( sCaptionText != NULL )
					m_wndToolTip.AddTool(
						this,
						sCaptionText,
						&rcCaptionInClient,
						1
						);
			} // if show tooltip on captions captions
		} // if( bContentVisiblePartially && (!rcCaptionInClient.IsRectEmpty()) )
	} // if expand captions or show tooltips

	if( m_bDelayRecalcLayout )
		UpdatePageContainerWnd( true );
	else
	{
//		Invalidate();
		UpdateWindow();
	}
}

void CExtPageContainerWnd::OnPageContainerCaptionHoverStop(
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo
	)
{
	ASSERT_VALID( this );
	ASSERT( pPageInfo != NULL );
	//ASSERT( pPageInfo->IsVisible() );
	pPageInfo;

	KillTimer( __EXT_PAGECONTAINERWND_HOVER_TIMER_ID );

	if( m_pPageInfoHover != NULL )
	{
		if(		(m_dwPageContainerStyle & __EPCWS_CAPTION_TOOLTIPS) != 0
			&&	m_wndToolTip.GetSafeHwnd() != NULL
			&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
			)
			m_wndToolTip.DelTool( this, 1 );
		CWnd::CancelToolTips();
		m_wndContentExpand.Deactivate();

		CRect rcCaption( 0, 0, 0, 0 );
		PageGetLayoutRectInfo(
			PageGetIndexOf(m_pPageInfoHover),
			NULL,
			&rcCaption
			);
		InvalidateRect( &rcCaption );

		m_pPageInfoHover->m_bCaptionHover = false;
		m_pPageInfoHover = NULL;
		//UpdatePageContainerWnd( false );
		//Invalidate();
	} // if( m_pPageInfoHover != NULL )

CFrameWnd * pParentFrame = GetParentFrame();
	if( pParentFrame != NULL
		&& pParentFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd))
		)
		SetTimer(
			__EXT_PAGECONTAINERWND_DELAY_UPDATE_TIMER_ID,
			__EXT_PAGECONTAINERWND_DELAY_UPDATE_TIMER_PERIOD,
			NULL
			);
	else
		UpdateWindow();
}

void CExtPageContainerWnd::OnCaptureChanged(CWnd *pWnd) 
{
	CWnd::OnCaptureChanged(pWnd);

	if( CExtMouseCaptureSink::GetCapture() != GetSafeHwnd() )
		_CancelActions();
}

void CExtPageContainerWnd::ScrollPosAdjust(
	LONG zDelta,
	bool bUpdateLayoutNow // = true
	)
{
LONG nScrollPos = m_nScrollPos + zDelta;
	if( nScrollPos > m_nScrollSize )
		nScrollPos = m_nScrollSize;
	else if( nScrollPos < 0 )
		nScrollPos = 0;
	if( nScrollPos == m_nScrollPos )
		return;
	m_nScrollPos = nScrollPos;
	UpdatePageContainerWnd( bUpdateLayoutNow );
}

void CExtPageContainerWnd::OnPageContainerAdjustMouseWheelParms(
	LONG & zDelta
	)
{
	if( zDelta > 0 )
		zDelta = 16L;
	else
		zDelta = -16L;
}

bool CExtPageContainerWnd::_ProcessMouseWheel( short zDelta, bool bDirectMsg )
{
	ASSERT_VALID( this );
	if(		GetSafeHwnd() == NULL
		||	( ! ::IsWindow(GetSafeHwnd()) )
		||	(GetStyle() & WS_VISIBLE) == 0
		||	( ! IsWindowEnabled() )
		)
		return false;

	if(		m_bDragScrolling
		||	m_pPageInfoPressed != NULL
		||	m_nScrollSize == 0
		)
		return false;

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

LONG zDeltaAdjust = -zDelta;
	OnPageContainerAdjustMouseWheelParms( zDeltaAdjust );
	_CancelActions();
	ScrollPosAdjust( zDeltaAdjust, true );
	
	return true;
}

BOOL CExtPageContainerWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// return CWnd::OnMouseWheel(nFlags, zDelta, pt);
	pt;
	nFlags;
	return _ProcessMouseWheel( zDelta, true ) ? TRUE : FALSE;
}

void CExtPageContainerWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_hWndEditor != NULL )
		return;
		
CPoint ptScreen( point );
	ClientToScreen( &ptScreen );
	if( m_bDragScrolling )
	{
		bool bHorzLayout = IsHorizontalLayout();
		LONG nMouseDelta = 
			bHorzLayout
				? (ptScreen.x - m_ptStartLeftBtnTrack.x)
				: (ptScreen.y - m_ptStartLeftBtnTrack.y)
				;
		LONG zDelta =
			m_nScrollDragStart
			- m_nScrollPos
			- nMouseDelta
			;
		LONG nScrollPosOld = ScrollPosGet();
		ScrollPosAdjust( zDelta, true );
		LONG nScrollPosNew = ScrollPosGet();
		if( nScrollPosOld == nScrollPosNew )
		{
			m_ptStartLeftBtnTrack = ptScreen;
			m_nScrollDragStart = nScrollPosNew;
		}
		return;
	}

	if( m_pPageInfoPressed != NULL )
	{
		if( m_dwPageContainerStyle & __EPCWS_CAPTION_DRAG )
		{
			if(	OnPageContainerCanDoCaptionDrag(
					m_pPageInfoPressed,
					m_ptStartLeftBtnTrack,
					ptScreen,
					nFlags
					)
				)
			{
				PAGE_ITEM_INFO * pPageInfo = m_pPageInfoPressed;
				CPoint ptStartLeftBtnTrack( m_ptStartLeftBtnTrack );
				_CancelActions();
				OnPageContainerStartCaptionDrag(
					pPageInfo,
					ptStartLeftBtnTrack,
					ptScreen,
					nFlags
					);
			}
		}
		return;
	}

	if( OnPageContainerCanDoHoverActions() )
	{
		HIT_TEST_INFO _pht( point );
		PageHitTest( _pht );
		if( _pht.m_dwHitTestCode & __EPCHT_ON_PAGE_CAPT_ANY )
		{
			ASSERT( _pht.m_pPageInfo != NULL );
			if( _pht.m_pPageInfo != m_pPageInfoHover )
				OnPageContainerCaptionHoverStart(
					_pht.m_pPageInfo
					);
			return;
		}
	} // if( OnPageContainerCanDoHoverActions() )

/*
HIT_TEST_INFO _pht( point );
	PageHitTest( _pht );
__EXT_MFC_SAFE_LPCTSTR sHitText = _T("__EPCHT_NOWHERE");
	switch( _pht.m_dwHitTestCode )
	{
	case __EPCHT_ON_SCROLL_POS_AREA:
		sHitText = _T("__EPCHT_ON_SCROLL_POS_AREA"); break;
	case __EPCHT_ON_PAGE_CAPT_TEXT:
		sHitText = _T("__EPCHT_ON_PAGE_CAPT_TEXT"); break;
	case __EPCHT_ON_PAGE_CAPT_ICON:
		sHitText = _T("__EPCHT_ON_PAGE_CAPT_ICON"); break;
	case __EPCHT_ON_PAGE_CAPT_GLYPH:
		sHitText = _T("__EPCHT_ON_PAGE_CAPT_GLYPH"); break;
	case __EPCHT_ON_PAGE_BORDER_L:
		sHitText = _T("__EPCHT_ON_PAGE_BORDER_L"); break;
	case __EPCHT_ON_PAGE_BORDER_R:
		sHitText = _T("__EPCHT_ON_PAGE_BORDER_R"); break;
	case __EPCHT_ON_PAGE_BORDER_T:
		sHitText = _T("__EPCHT_ON_PAGE_BORDER_T"); break;
	case __EPCHT_ON_PAGE_BORDER_B:
		sHitText = _T("__EPCHT_ON_PAGE_BORDER_B"); break;
	case __EPCHT_ON_PAGE_WND:
		sHitText = _T("__EPCHT_ON_PAGE_WND"); break;
	}
CFrameWnd * pFrame =
		STATIC_DOWNCAST(
			CFrameWnd,
			AfxGetMainWnd()
			);
	ASSERT_VALID( pFrame );
CStatusBar * pStatusBar =
		STATIC_DOWNCAST(
			CStatusBar,
			pFrame->GetControlBar( AFX_IDW_STATUS_BAR )
			);
	ASSERT_VALID( pStatusBar );
	pStatusBar->SetPaneText( 0, sHitText );
	
	CWnd::OnMouseMove(nFlags, point);
*/

}

void CExtPageContainerWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
//	CWnd::OnLButtonDblClk(nFlags, point);
	nFlags;
	point;

//HIT_TEST_INFO _pht( point );
//	PageHitTest( _pht );
//	if( _pht.m_dwHitTestCode & __EPCHT_ON_PAGE_CAPT_ANY )
//	{
//		ASSERT( _pht.m_pPageInfo != NULL );
//		_CancelActions();
//		ASSERT( m_hWndEditor == NULL );
//		m_hWndEditor = OnPageContainerStartItemEditor( _pht.m_pPageInfo );
//		return;
//	}

}

void CExtPageContainerWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
//	CWnd::OnLButtonDown(nFlags, point);
	nFlags;
//	_CancelActions();
	
HIT_TEST_INFO _pht( point );
	PageHitTest( _pht );
	if( _pht.m_dwHitTestCode & __EPCHT_ON_PAGE_CAPT_ANY )
	{
		ASSERT( _pht.m_pPageInfo != NULL );
		_CancelActions();
		OnPageContainerCaptionPressedStart(
			_pht.m_pPageInfo
			);
		if( m_dwPageContainerStyle & __EPCWS_CAPTION_DRAG )
		{
			CPoint ptScreen;
			if( ! ::GetCursorPos( &ptScreen ) )
				return;
			m_ptStartLeftBtnTrack = ptScreen;
		}
		return;
	} // if( _pht.m_dwHitTestCode & __EPCHT_ON_PAGE_CAPT_ANY )

CPoint ptScreen;
	if( ! ::GetCursorPos( &ptScreen ) )
		return;
	if( OnPageContainerIsDragablePosition(ptScreen) )
	{
		if( m_dwPageContainerStyle & __EPCWS_NO_SCROLL )
			return;
		_CancelActions();
		m_ptStartLeftBtnTrack = ptScreen;
		m_nScrollDragStart = m_nScrollPos;
		m_bDragScrolling = true;
		CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
		OnPageContainerSetCursorOnDragableArea( true );
		return;
	}
}

void CExtPageContainerWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
//	CWnd::OnLButtonUp(nFlags, point);
	nFlags;

	if( m_bDragScrolling )
	{
		_CancelActions();
		return;
	}

	if( m_pPageInfoPressed != NULL )
	{
		PAGE_ITEM_INFO * pPageInfoToExpand = NULL;
		HIT_TEST_INFO _pht( point );
		PageHitTest( _pht );
		if( _pht.m_dwHitTestCode & __EPCHT_ON_PAGE_CAPT_ANY )
		{
			ASSERT( _pht.m_pPageInfo != NULL );
			if( _pht.m_pPageInfo == m_pPageInfoPressed )
				pPageInfoToExpand = m_pPageInfoPressed;
		}
		_CancelActions();

		if( CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
		{
			ASSERT( m_hWndEditor == NULL );
			m_hWndEditor = OnPageContainerStartItemEditor( _pht.m_pPageInfo );
			return;
		}

		if( pPageInfoToExpand != NULL )
			PageExpand(
				PageGetIndexOf( pPageInfoToExpand ),
				!pPageInfoToExpand->IsExpanded(),
				true,
				true
				);
		
		if( (m_dwPageContainerStyle & __EPCWS_NO_FOCUS_ON_CLICK) == 0 )
		{
			HWND hWndToSetFocus = GetSafeHwnd();
			if(		(m_dwPageContainerStyle & __EPCWS_CAPTION_FOCUS) != 0
				&&	pPageInfoToExpand != NULL
				&&	pPageInfoToExpand->IsExpanded()
				)
			{
				HWND hWndPage = pPageInfoToExpand->GetSafePageHwnd();
				ASSERT( hWndPage != NULL );
				ASSERT( ::IsWindow(hWndPage) );
//				PageEnsureVisible(
//					PageGetIndexOf(pPageInfoToExpand),
//					true
//					);
				if( ::IsWindowEnabled(hWndPage) )
					hWndToSetFocus = hWndPage;
			}
			::SetFocus( hWndToSetFocus );
		} // if( (m_dwPageContainerStyle & __EPCWS_NO_FOCUS_ON_CLICK) == 0 )
		
		return;
	}

	if( (m_dwPageContainerStyle & __EPCWS_NO_FOCUS_ON_CLICK) == 0 )
		SetFocus();

	if( m_dwPageContainerStyle & __EPCWS_CAPTION_DRAG )
		m_ptStartLeftBtnTrack.x = m_ptStartLeftBtnTrack.y = -1;
}

void CExtPageContainerWnd::OnTimer(UINT nIDEvent) 
{
	switch( nIDEvent )
	{
	case __EXT_PAGECONTAINERWND_HOVER_TIMER_ID:
	{
		if( m_pPageInfoHover != NULL )
		{
			CPoint point;
			if( ! ::GetCursorPos( &point ) )
				return;
			ScreenToClient( &point );
			HIT_TEST_INFO _pht( point );
			PageHitTest( _pht );
			if( _pht.m_dwHitTestCode & __EPCHT_ON_PAGE_CAPT_ANY )
			{
				ASSERT( _pht.m_pPageInfo != NULL );
				if( _pht.m_pPageInfo == m_pPageInfoHover )
					return;
			}
		}
		KillTimer( nIDEvent );
		_CancelActions();
		return;
	} // case __EXT_PAGECONTAINERWND_HOVER_TIMER_ID
	case __EXT_PAGECONTAINERWND_DELAY_UPDATE_TIMER_ID:
	{
		KillTimer( nIDEvent );
		if( m_bDelayRecalcLayout )
			UpdatePageContainerWnd( true );
		else
		{
			Invalidate();
			UpdateWindow();
		}
		return;
	} // case __EXT_PAGECONTAINERWND_DELAY_UPDATE_TIMER_ID
	} // switch( nIDEvent )

	CWnd::OnTimer(nIDEvent);
}

void CExtPageContainerWnd::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);
	if( pNewWnd != NULL )
	{
		if( ::IsChild( m_hWnd, pNewWnd->GetSafeHwnd() ) )
			return;
		if(		m_hWndEditor != NULL
			&&	::IsWindow( m_hWndEditor )
			)
		{
			if( ::IsChild( m_hWndEditor, pNewWnd->GetSafeHwnd() ) )
				return;
		}
	}
	_CancelActions();
}

bool CExtPageContainerWnd::_SetCursorDragScroll(
	bool bDragScrollingNow,
	bool bHorzLayout
	)
{
HCURSOR hCursorToSet =
		bDragScrollingNow
			? ( bHorzLayout ? g_hCursorDragScrollOnH : g_hCursorDragScrollOnV )
			: g_hCursorDragScrollOff
			;
	if( hCursorToSet == NULL )
		return false;
	SetCursor( hCursorToSet );
	return true;
}

bool CExtPageContainerWnd::_SetCursorCaption()
{
	if( g_hCursorCaption == NULL )
		return false;
	SetCursor( g_hCursorCaption );
	return true;
}

bool CExtPageContainerWnd::OnPageContainerIsDragablePosition(
	const POINT & ptScreen
	) const
{
	ASSERT_VALID( this );

	if(		GetSafeHwnd() == NULL
		||	( ! ::IsWindow(GetSafeHwnd()) )
		||	( GetStyle() & WS_VISIBLE ) == 0
		||	( ! ::IsWindowEnabled(GetSafeHwnd()) )
		||	ScrollSizeGet() == 0
		||	PageGetCount() == 0
		||	(! CanDoLayoutMouseScrolling() )
		) // if nothing to scroll or can not start drag/scroll
		return false;

	if( m_dwPageContainerStyle & __EPCWS_NO_SCROLL )
		return false;

CPoint ptClient( ptScreen );
	ScreenToClient( &ptClient );
HIT_TEST_INFO _pht( ptClient );
	PageHitTest( _pht );
	if( _pht.m_dwHitTestCode & __EPCHT_ON_PAGE_CAPT_ANY )
	{ // if on page caption
		ASSERT( _pht.m_pPageInfo != NULL );
		return false;
	} // if on page caption

	if( (_pht.m_dwHitTestCode & __EPCHT_ON_PAGE_WND) == 0 )
	{ // if not on page window
		return true;
	} // if not on page window
	
	// if on page window
	ASSERT( _pht.m_pPageInfo != NULL );
HWND hWndPage = _pht.m_pPageInfo->GetSafePageHwnd();
	ASSERT( hWndPage != NULL );
	ASSERT( ::IsWindow(hWndPage) );
	if( ::SendMessage(
			hWndPage,
			CExtPageContainerWnd::g_nMsgQueryCancelResetDragCursor,
			reinterpret_cast < WPARAM > ( &_pht ),
			0
			) != 0
		) // if page window enables us to start drag/scroll
		return true;

	// force enable start drag/scroll on dialog areas
CWnd * pWnd = CWnd::FromHandlePermanent(hWndPage);
	if(		pWnd == NULL
		||	(! pWnd->IsKindOf( RUNTIME_CLASS(CDialog) ) )
		)
		return false;
HWND hWndFromPoint = ::WindowFromPoint( ptScreen );
	if( hWndFromPoint == hWndPage )
		return true;

	return false;
}

bool CExtPageContainerWnd::OnPageContainerSetCursorOnDragableArea(
	bool bDragScrollingNow
	)
{
	ASSERT_VALID( this );
	return
		_SetCursorDragScroll(
			bDragScrollingNow,
			IsHorizontalLayout()
			);
}

bool CExtPageContainerWnd::OnPageContainerSetCursorOnCaption(
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo
	)
{
	ASSERT_VALID( this );
	ASSERT( pPageInfo != NULL );
	pPageInfo;
	return _SetCursorCaption();
}

BOOL CExtPageContainerWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if( m_hWndEditor != NULL )
		return CWnd::OnSetCursor(pWnd, nHitTest, message);

CPoint ptScreen;
	if( ! ::GetCursorPos( &ptScreen ) )
		return CWnd::OnSetCursor(pWnd, nHitTest, message);
	if( OnPageContainerIsDragablePosition(ptScreen) )
	{
		if( OnPageContainerSetCursorOnDragableArea(
				IsDragScrolling()
				)
			)
			return TRUE;
	} // if( OnPageContainerIsDragablePosition(ptScreen) )
	else
	{
		CPoint ptClient( ptScreen );
		ScreenToClient( &ptClient );
		HIT_TEST_INFO _pht( ptClient );
		PageHitTest( _pht );
		if( _pht.m_dwHitTestCode & __EPCHT_ON_PAGE_CAPT_ANY )
		{ // if on page caption
			ASSERT( _pht.m_pPageInfo != NULL );
			if( OnPageContainerSetCursorOnCaption(
					_pht.m_pPageInfo
					)
				)
				return TRUE;
		} // if on page caption
	} // else from if( OnPageContainerIsDragablePosition(ptScreen) )

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

bool CExtPageContainerWnd::OnHookWndMsg(
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
LONG nPageCount = PageGetCount();
	if( nPageCount > 0 )
	{
		LONG nIndex = PageFind( hWndHooked );
		if( nIndex >= 0 )
		{
			ASSERT( nIndex < nPageCount );
			PAGE_ITEM_INFO * pPageInfo = PageGetInfo( nIndex );
			ASSERT( pPageInfo != NULL );
			if(	OnPageContainerHookPageWndMsg(
					lResult,
					pPageInfo,
					nMessage,
					wParam,
					lParam
					)
				)
				return true;
		} // if( nIndex >= 0 )
	} // if( nPageCount > 0 )

	return
		CExtHookSink::OnHookWndMsg(
			lResult,
			hWndHooked,
			nMessage,
			wParam,
			lParam
			);
}

bool CExtPageContainerWnd::OnPageContainerHookPageWndMsg(
	LRESULT & lResult,
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo,
	UINT nMessage,
	WPARAM & wParam,
	LPARAM & lParam
	)
{
	ASSERT_VALID( this );
	ASSERT( pPageInfo != NULL );
	lResult;

	if(		( !pPageInfo->IsVisible() )
		||	( !pPageInfo->IsExpanded() )
		)
		return false;

bool bResendMsgToSelf = false;
	switch( nMessage )
	{
	case WM_MOUSEWHEEL:
	{
		CPoint point;
		if( ! ::GetCursorPos( &point ) )
			return true;
		if( OnPageContainerIsDragablePosition(point) )
		{
			ScreenToClient( &point );
			lParam = MAKELPARAM( point.x, point.y );
			bResendMsgToSelf = true;
		}
	}
	break; // case WM_MOUSEWHEEL
	case WM_LBUTTONDOWN:
	{
		CPoint point;
		if( ! ::GetCursorPos( &point ) )
			return true;
		if( OnPageContainerIsDragablePosition(point) )
		{
			ScreenToClient( &point );
			lParam = MAKELPARAM( point.x, point.y );
			bResendMsgToSelf = true;
			HWND hWndPage = pPageInfo->GetSafePageHwnd();
			ASSERT( hWndPage != NULL );
			ASSERT( ::IsWindow(hWndPage) );
			HWND hWndFocus = ::GetFocus();
			if( hWndFocus != hWndPage
				|| (! ::IsChild(hWndPage,hWndFocus) )
				)
			{
				if(		::IsWindowEnabled(hWndPage)
					&&	( m_dwPageContainerStyle & __EPCWS_CAPTION_FOCUS ) != 0
					)
					::SetFocus( hWndPage );
				else
				{
					if(! ::IsChild(GetSafeHwnd(),hWndFocus) )
						SetFocus();
				}
			}
		}
	}
	break; // case WM_LBUTTONDOWN
	case WM_LBUTTONUP:
	{
		if( m_bDragScrolling )
		{
			CPoint point;
			if( ! ::GetCursorPos( &point ) )
				return true;
			ScreenToClient( &point );
			lParam = MAKELPARAM( point.x, point.y );
			bResendMsgToSelf = true;
		}
	}
	break; // case WM_LBUTTONUP
	case WM_SETFOCUS:
	{
		if( m_dwPageContainerStyle & __EPCWS_CAPTION_FOCUS )
		{
			PageEnsureVisible(
				PageGetIndexOf(pPageInfo),
				true
				);
		} // if( m_dwPageContainerStyle & __EPCWS_CAPTION_FOCUS )
	}
	break; // case WM_SETFOCUS
	} // switch( nMessage )

	if( bResendMsgToSelf )
	{
		::SendMessage(
			GetSafeHwnd(),
			nMessage,
			wParam,
			lParam
			);
		return true;
	} // if( bResendMsgToSelf )

	return false;
}

bool CExtPageContainerWnd::OnPageContainerCanDoCaptionDrag(
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo,
	CPoint ptStartLeftBtnTrack,
	CPoint ptCurrentLeftBtn,
	UINT nMouseMoveFlags
	)
{
	ASSERT_VALID( this );
	ASSERT( pPageInfo != NULL );
	ASSERT( pPageInfo->IsVisible() );
	pPageInfo;
	nMouseMoveFlags;
	if( (GetPageContainerStyle() & __EPCWS_CAPTION_DRAG) == 0 )
		return false;
CPoint ptDiff(
		labs( ptStartLeftBtnTrack.x - ptCurrentLeftBtn.x ),
		labs( ptStartLeftBtnTrack.y - ptCurrentLeftBtn.y )
		);
	if( ptDiff.x >= 10 || ptDiff.y >= 10 )
		return true;
	return false;
}

void CExtPageContainerWnd::OnPageContainerStartCaptionDrag(
	CExtPageContainerWnd::PAGE_ITEM_INFO * pPageInfo,
	CPoint ptStartLeftBtnTrack,
	CPoint ptCurrentLeftBtn,
	UINT nMouseMoveFlags
	)
{
	ASSERT_VALID( this );
	ASSERT( pPageInfo != NULL );
	ASSERT( pPageInfo->IsVisible() );
	pPageInfo;
	ptStartLeftBtnTrack;
	ptCurrentLeftBtn;
	nMouseMoveFlags;
}

#endif //  (!defined __EXT_MFC_NO_PAGECONTAINER_CTRL)

