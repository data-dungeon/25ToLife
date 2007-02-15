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

#if (!defined __EXT_MFC_NO_GRIDBASEWND)

#if (!defined __EXT_GRIDWND_H)
	#include <ExtGridWnd.h>
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

#if (!defined __EXT_POPUP_CTRL_MENU_H)
	#include <ExtPopupCtrlMenu.h>
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

#if (!defined __EXTCOLORDLG_H)
	#include "ExtColorDlg.h"
#endif

#include <../profuisdll/resource.h>
#include <limits.h>
#include <float.h>

/////////////////////////////////////////////////////////////////////////////
// CExtGridHitTestInfo

CExtGridHitTestInfo::CExtGridHitTestInfo(
	CPoint ptClient // = CPoint( -1, -1 )
	)
{
	Empty();
	m_ptClient = ptClient;
}

CExtGridHitTestInfo::CExtGridHitTestInfo(
	const CExtGridHitTestInfo & htInfo
	)
{
	_AssignFromOther( htInfo );
}

CExtGridHitTestInfo & CExtGridHitTestInfo::operator = (
	const CExtGridHitTestInfo & htInfo
	)
{
	_AssignFromOther( htInfo );
	return (*this);
}

void CExtGridHitTestInfo::_AssignFromOther(
	const CExtGridHitTestInfo & htInfo
	)
{
	m_ptClient = htInfo.m_ptClient;
	m_nRowNo = htInfo.m_nRowNo;
	m_nColNo = htInfo.m_nColNo;
	m_nVisibleRowNo = htInfo.m_nVisibleRowNo;
	m_nVisibleColNo = htInfo.m_nVisibleColNo;
	m_dwAreaFlags = htInfo.m_dwAreaFlags;
	m_rcItem = htInfo.m_rcItem;
	m_rcExtra = htInfo.m_rcExtra;
	m_rcPart = htInfo.m_rcPart;
	m_nButtonType = htInfo.m_nButtonType;
}

void CExtGridHitTestInfo::Empty()
{
	m_ptClient.x = m_ptClient.y = -1;
	m_nRowNo = m_nColNo = m_nVisibleRowNo = m_nVisibleColNo = -1L;
	m_dwAreaFlags = __EGBWA_NOWHERE;
	m_rcItem.SetRect( 0, 0, 0, 0 );
	m_rcPart.SetRect( 0, 0, 0, 0 );
	m_nButtonType = -1;
}

bool CExtGridHitTestInfo::IsHoverEmpty() const
{
	if( (m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0 )
	{
		if( m_nColNo >= 0L && m_nRowNo >= 0L )
			return false;
		return true;
	} // if( (m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0 )
	if( (m_dwAreaFlags & __EGBWA_OUTER_CELLS) != 0 )
	{
		if(		(m_dwAreaFlags & __EGBWA_OUTER_H) != 0
			&&	(m_dwAreaFlags & __EGBWA_OUTER_V) != 0
			)
			return true;
		if( (m_dwAreaFlags & __EGBWA_OUTER_H) != 0 )
		{
			if( m_nRowNo >= 0L )
				return false;
			return true;
		} // if( (m_dwAreaFlags & __EGBWA_OUTER_H) != 0 )
		if( (m_dwAreaFlags & __EGBWA_OUTER_V) != 0 )
		{
			if( m_nColNo >= 0L )
				return false;
			return true;
		} // if( (m_dwAreaFlags & __EGBWA_OUTER_V) != 0 )
	} // if( (m_dwAreaFlags & __EGBWA_OUTER_CELLS) != 0 )
	return true;
}

bool CExtGridHitTestInfo::IsHoverInner() const
{
	if( (m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0 )
	{
		if( m_nColNo >= 0L && m_nRowNo >= 0L )
			return true;
	} // if( (m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0 )
	return false;
}

bool CExtGridHitTestInfo::IsHoverOuter(
	bool * p_bOuterCellAtLeftRight // = NULL
	) const
{
	if( (m_dwAreaFlags & __EGBWA_OUTER_CELLS) != 0 )
	{
		if(		(m_dwAreaFlags & __EGBWA_OUTER_H) != 0
			&&	(m_dwAreaFlags & __EGBWA_OUTER_V) != 0
			)
			return false;
		if( (m_dwAreaFlags & __EGBWA_OUTER_H) != 0 )
		{
			if( m_nRowNo >= 0L )
			{
				if( p_bOuterCellAtLeftRight != NULL )
					*p_bOuterCellAtLeftRight = true;
				return true;
			} // if( m_nRowNo >= 0L )
			return false;
		} // if( (m_dwAreaFlags & __EGBWA_OUTER_H) != 0 )
		if( (m_dwAreaFlags & __EGBWA_OUTER_V) != 0 )
		{
			if( m_nColNo >= 0L )
			{
				if( p_bOuterCellAtLeftRight != NULL )
					*p_bOuterCellAtLeftRight = false;
				return true;
			} // if( m_nColNo >= 0L )
			return false;
		} // if( (m_dwAreaFlags & __EGBWA_OUTER_V) != 0 )
	} // if( (m_dwAreaFlags & __EGBWA_OUTER_CELLS) != 0 )
	return false;
}

bool CExtGridHitTestInfo::IsHoverEqual(
	const CExtGridHitTestInfo & htInfoLeft,
	const CExtGridHitTestInfo & htInfoRight,
	bool bCompareRects, // = true
	bool bValidtateRects, // = true
	bool bCompareOuterDropAfterState // = true
	)
{
	if( bCompareRects )
	{
		if( htInfoLeft.m_rcItem != htInfoRight.m_rcItem )
			return false;
		if( bValidtateRects )
		{
			if( ! htInfoLeft.IsValidRect() )
				return false;
		} // if( bValidtateRects )
	} // if( bCompareRects )
	if(		(htInfoLeft.m_dwAreaFlags & __EGBWA_INNER_CELLS)
		!=	(htInfoRight.m_dwAreaFlags & __EGBWA_INNER_CELLS)
		)
		return false;
	if(		(htInfoLeft.m_dwAreaFlags & __EGBWA_OUTER_CELLS)
		!=	(htInfoRight.m_dwAreaFlags & __EGBWA_OUTER_CELLS)
		)
		return false;
	if( (htInfoLeft.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0 )
	{
		if( (htInfoLeft.m_dwAreaFlags & __EGBWA_OUTER_CELLS) != 0 )
			return false;
		if(		htInfoLeft.m_nRowNo == htInfoRight.m_nRowNo
			&&	htInfoLeft.m_nColNo == htInfoRight.m_nColNo
			)
		{
			if( bCompareOuterDropAfterState )
			{
				bool bOuterDropAfterStateLeft =
					( (htInfoLeft.m_dwAreaFlags & __EGBWA_OUTER_DROP_AFTER) != 0 )
						? true : false;
				bool bOuterDropAfterStateRight =
					( (htInfoRight.m_dwAreaFlags & __EGBWA_OUTER_DROP_AFTER) != 0 )
						? true : false;
				if( bOuterDropAfterStateLeft != bOuterDropAfterStateRight )
					return false;
			} // if( bCompareOuterDropAfterState )
			return true;
		}
		return false;
	} // if( (htInfoLeft.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0 )
	if( (htInfoLeft.m_dwAreaFlags & __EGBWA_OUTER_CELLS) == 0 )
		return false;
	if(		(htInfoLeft.m_dwAreaFlags & __EGBWA_OUTER_H)
		!=	(htInfoRight.m_dwAreaFlags & __EGBWA_OUTER_H)
		)
		return false;
	if(		(htInfoLeft.m_dwAreaFlags & __EGBWA_OUTER_V)
		!=	(htInfoRight.m_dwAreaFlags & __EGBWA_OUTER_V)
		)
		return false;
	if( (htInfoLeft.m_dwAreaFlags & __EGBWA_OUTER_H) != 0 )
	{
		if( (htInfoLeft.m_dwAreaFlags & __EGBWA_OUTER_V) != 0 )
			return false;
		if( htInfoLeft.m_nRowNo == htInfoRight.m_nRowNo )
		{
			if( bCompareOuterDropAfterState )
			{
				bool bOuterDropAfterStateLeft =
					( (htInfoLeft.m_dwAreaFlags & __EGBWA_OUTER_DROP_AFTER) != 0 )
						? true : false;
				bool bOuterDropAfterStateRight =
					( (htInfoRight.m_dwAreaFlags & __EGBWA_OUTER_DROP_AFTER) != 0 )
						? true : false;
				if( bOuterDropAfterStateLeft != bOuterDropAfterStateRight )
					return false;
			} // if( bCompareOuterDropAfterState )
			return true;
		} // if( htInfoLeft.m_nRowNo == htInfoRight.m_nRowNo )
	} // if( (htInfoLeft.m_dwAreaFlags & __EGBWA_OUTER_H) != 0 )
	if( (htInfoLeft.m_dwAreaFlags & __EGBWA_OUTER_V) != 0 )
	{
		if( (htInfoLeft.m_dwAreaFlags & __EGBWA_OUTER_H) != 0 )
			return false;
		if( htInfoLeft.m_nColNo == htInfoRight.m_nColNo )
		{
			if( bCompareOuterDropAfterState )
			{
				bool bOuterDropAfterStateLeft =
					( (htInfoLeft.m_dwAreaFlags & __EGBWA_OUTER_DROP_AFTER) != 0 )
						? true : false;
				bool bOuterDropAfterStateRight =
					( (htInfoRight.m_dwAreaFlags & __EGBWA_OUTER_DROP_AFTER) != 0 )
						? true : false;
				if( bOuterDropAfterStateLeft != bOuterDropAfterStateRight )
					return false;
			} // if( bCompareOuterDropAfterState )
			return true;
		} // if( htInfoLeft.m_nColNo == htInfoRight.m_nColNo )
	} // if( (htInfoLeft.m_dwAreaFlags & __EGBWA_OUTER_V) != 0 )
	return false;
}

bool CExtGridHitTestInfo::IsHoverEqual(
	const CExtGridHitTestInfo & htInfo,
	bool bCompareRects, // = true
	bool bValidtateRects, // = true
	bool bCompareOuterDropAfterState // = true
	) const
{
	return
		IsHoverEqual(
			*this,
			htInfo,
			bCompareRects,
			bValidtateRects,
			bCompareOuterDropAfterState
			);
}

bool CExtGridHitTestInfo::IsValidRect(
	bool bEmptyIsInvalid // = true
	) const
{
	if( bEmptyIsInvalid )
	{
		if(		m_rcItem.left >= m_rcItem.right
			||	m_rcItem.top >= m_rcItem.bottom
			)
			return false;
	} // if( bEmptyIsInvalid )
	else
	{
		if(		m_rcItem.left > m_rcItem.right
			||	m_rcItem.top > m_rcItem.bottom
			)
			return false;
	} // else from if( bEmptyIsInvalid )
	return true;
}

INT CExtGridHitTestInfo::GetInnerOuterTypeOfColumn() const
{
	ASSERT( !IsHoverEmpty() );
	return GetInnerOuterTypeOfColumn( m_dwAreaFlags );
}

INT CExtGridHitTestInfo::GetInnerOuterTypeOfRow() const
{
	ASSERT( !IsHoverEmpty() );
	return GetInnerOuterTypeOfRow( m_dwAreaFlags );
}

INT CExtGridHitTestInfo::GetInnerOuterTypeOfColumn( DWORD dwAreaFlags )
{
INT nColType =
		( (dwAreaFlags & __EGBWA_OUTER_LEFT) != 0 )
				? -1
				: (	( (dwAreaFlags & __EGBWA_OUTER_RIGHT) != 0 )
						? 1
						: 0
					);
	return nColType;
}

INT CExtGridHitTestInfo::GetInnerOuterTypeOfRow( DWORD dwAreaFlags )
{
INT nRowType =
		( (dwAreaFlags & __EGBWA_OUTER_TOP) != 0 )
			? -1
			: (	( (dwAreaFlags & __EGBWA_OUTER_BOTTOM) != 0 )
					? 1
					: 0
				);
	return nRowType;
}

DWORD CExtGridHitTestInfo::CellTypesToAreaFlags(
	INT nColType,
	INT nRowType
	)
{
DWORD dwAreaFlags = 0L;

	if( nColType < 0 )
		dwAreaFlags |= __EGBWA_OUTER_LEFT|__EGBWA_OUTER_CELLS;
	else if( nColType > 0 )
		dwAreaFlags |= __EGBWA_OUTER_RIGHT|__EGBWA_OUTER_CELLS;

	if( nRowType < 0 )
		dwAreaFlags |= __EGBWA_OUTER_TOP|__EGBWA_OUTER_CELLS;
	else if( nRowType > 0 )
		dwAreaFlags |= __EGBWA_OUTER_BOTTOM|__EGBWA_OUTER_CELLS;

	if( nColType == 0 && nRowType == 0 )
		dwAreaFlags |= __EGBWA_INNER_CELLS;

	return dwAreaFlags;
}

/////////////////////////////////////////////////////////////////////////////
// CExtGridBaseWnd

IMPLEMENT_DYNCREATE( CExtGridBaseWnd, CExtScrollItemWnd );

CExtGridBaseWnd::CExtGridBaseWnd()
	: m_nMouseActivateCode( MA_ACTIVATE )
	, m_nCountOfRows( 0L )
	, m_nCountOfColumns( 0L )
	, m_nCountOfOuterRowsT( 0L )
	, m_nCountOfOuterRowsB( 0L )
	, m_nCountOfOuterColumnsL( 0L )
	, m_nCountOfOuterColumnsR( 0L )
	, m_dwDefaultScrollTypeH( __ESIW_ST_ITEM )
	, m_dwDefaultScrollTypeV( __ESIW_ST_ITEM )
	, m_sizeCellDefault( 60, GetSystemMetrics(SM_CYHSCROLL)+1 )
	, m_rcOuterT( 0, 0, 0, 0 )
	, m_rcOuterB( 0, 0, 0, 0 )
	, m_rcOuterL( 0, 0, 0, 0 )
	, m_rcOuterR( 0, 0, 0, 0 )
	, m_rcOuterLT( 0, 0, 0, 0 )
	, m_rcOuterRT( 0, 0, 0, 0 )
	, m_rcOuterLB( 0, 0, 0, 0 )
	, m_rcOuterRB( 0, 0, 0, 0 )
	, m_rcClient( 0, 0, 0, 0 )
	, m_rcDefaultCellBorderHtSpaces( 3, 3, 3, 3 )
	, m_ptFocusCell( -1, -1 )
	, m_ptTrackOffset( 0, 0 )
	, m_bProcessingCancelMode( false )
	, m_bLockVirtualCacheUpdateH( false )
	, m_bLockVirtualCacheUpdateV( false )
	, m_eMTT( CExtGridBaseWnd::__EMTT_NOTHING )
	, m_nTrackExtentMin( 0 )
	, m_nTrackExtentMax( 0 )
	, m_nTrackExtentCurrent( 0 )
	, m_nTimerIdPopupMenuWaiting( 654 )
	, m_nTimerIdDelayedExpand( 653 )
	, m_nTimerIdExpandWait( 652 )
	, m_nTimerIdPressedButton( 651 )
	, m_nSizeOfExpandShadow( 3 )
	, m_hCursorOuterSelectionTop( NULL )
	, m_hCursorOuterSelectionBottom( NULL )
	, m_hCursorOuterSelectionLeft( NULL )
	, m_hCursorOuterSelectionRight( NULL )
	, m_hCursorInnerCell( NULL )
	, m_hCursorCheckBox( NULL )
	, m_hCursorBtnUp( NULL )
	, m_hCursorBtnDown( NULL )
	, m_hCursorBtnDropdown( NULL )
	, m_hCursorBtnEllipsis( NULL )
	, m_hCursorFocusArrow( NULL )
	, m_hCursorSortArrow( NULL )
	, m_hCursorIcon( NULL )
	, m_hWndInplaceControl( NULL )
{
	m_hCursorDefault = ::LoadCursor( NULL, IDC_ARROW );
	ASSERT( m_hCursorDefault != NULL );
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		CExtLocalResourceHelper _LRH;
		CWinApp * pApp = ::AfxGetApp();
		ASSERT_VALID( pApp );
		m_hCursorResizingH = 
			pApp->LoadCursor( MAKEINTRESOURCE( IDC_EXT_RESIZE_H1 ) );
		ASSERT( m_hCursorResizingH != NULL );
		m_hCursorResizingV =
			pApp->LoadCursor( MAKEINTRESOURCE( IDC_EXT_RESIZE_V1 ) );
		ASSERT( m_hCursorResizingV != NULL );
#else
		m_hCursorResizingH = 
			::LoadCursor( NULL, IDC_SIZEWE );
		ASSERT( m_hCursorResizingH != NULL );
		m_hCursorResizingV =
			::LoadCursor( NULL, IDC_SIZENS );
		ASSERT( m_hCursorResizingV != NULL );
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
}

CExtGridBaseWnd::~CExtGridBaseWnd()
{
	if( m_hCursorDefault != NULL )
		::DestroyCursor( m_hCursorDefault );
	if( m_hCursorResizingH != NULL )
		::DestroyCursor( m_hCursorResizingH );
	if( m_hCursorResizingV != NULL )
		::DestroyCursor( m_hCursorResizingV );
	if( m_hCursorOuterSelectionTop != NULL )
		::DestroyCursor( m_hCursorOuterSelectionTop );
	if( m_hCursorOuterSelectionBottom != NULL )
		::DestroyCursor( m_hCursorOuterSelectionBottom );
	if( m_hCursorOuterSelectionLeft != NULL )
		::DestroyCursor( m_hCursorOuterSelectionLeft );
	if( m_hCursorOuterSelectionRight != NULL )
		::DestroyCursor( m_hCursorOuterSelectionRight );
	if( m_hCursorInnerCell != NULL )
		::DestroyCursor( m_hCursorInnerCell );
	if( m_hCursorCheckBox != NULL )
		::DestroyCursor( m_hCursorCheckBox );
	if( m_hCursorBtnUp != NULL )
		::DestroyCursor( m_hCursorBtnUp );
	if( m_hCursorBtnDown != NULL )
		::DestroyCursor( m_hCursorBtnDown );
	if( m_hCursorBtnDropdown != NULL )
		::DestroyCursor( m_hCursorBtnDropdown );
	if( m_hCursorBtnEllipsis != NULL )
		::DestroyCursor( m_hCursorBtnEllipsis );
	if( m_hCursorFocusArrow != NULL )
		::DestroyCursor( m_hCursorFocusArrow );
	if( m_hCursorSortArrow != NULL )
		::DestroyCursor( m_hCursorSortArrow );
	if( m_hCursorIcon != NULL )
		::DestroyCursor( m_hCursorIcon );
}

BEGIN_MESSAGE_MAP(CExtGridBaseWnd,CExtScrollItemWnd)
	//{{AFX_MSG_MAP(CExtGridBaseWnd)
	ON_WM_SETCURSOR()
	ON_WM_CANCELMODE()
	ON_WM_CAPTURECHANGED()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEACTIVATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_TIMER()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SHOWWINDOW()
	ON_WM_ACTIVATEAPP()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(
		CExtContentExpandWnd::g_nMsgPaintItemContent,
		_OnPaintExpandedItemContent
		)
END_MESSAGE_MAP()

#ifdef _DEBUG
void CExtGridBaseWnd::AssertValid() const
{
	CExtScrollItemWnd::AssertValid();
	ASSERT( m_nCountOfOuterRowsT >= 0L );
	ASSERT( m_nCountOfOuterRowsB >= 0L );
	ASSERT( m_nCountOfOuterColumnsL >= 0L );
	ASSERT( m_nCountOfOuterColumnsR >= 0L );
	ASSERT(
			m_rcOuterT.left <= m_rcOuterT.right
		&&	m_rcOuterT.top <= m_rcOuterT.bottom
		&&	m_rcOuterB.left <= m_rcOuterB.right
		&&	m_rcOuterB.top <= m_rcOuterB.bottom
		&&	m_rcOuterL.left <= m_rcOuterL.right
		&&	m_rcOuterL.top <= m_rcOuterL.bottom
		&&	m_rcOuterR.left <= m_rcOuterR.right
		&&	m_rcOuterR.top <= m_rcOuterR.bottom
		&&	m_rcOuterLT.left <= m_rcOuterLT.right
		&&	m_rcOuterLT.top <= m_rcOuterLT.bottom
		&&	m_rcOuterRT.left <= m_rcOuterRT.right
		&&	m_rcOuterRT.top <= m_rcOuterRT.bottom
		&&	m_rcOuterLB.left <= m_rcOuterLB.right
		&&	m_rcOuterLB.top <= m_rcOuterLB.bottom
		&&	m_rcOuterRB.left <= m_rcOuterRB.right
		&&	m_rcOuterRB.top <= m_rcOuterRB.bottom
		);
	ASSERT( m_dwDefaultScrollTypeH != __ESIW_ST_VIRTUAL );
	ASSERT( m_dwDefaultScrollTypeV != __ESIW_ST_VIRTUAL );
	if( m_hWnd != NULL && ::IsWindow(m_hWnd) )
	{
		DWORD dwScrollTypeH = m_dwScrollItemWndStyle&__ESIS_STH_MASK;
		DWORD dwScrollTypeV = ((m_dwScrollItemWndStyle&__ESIS_STV_MASK) >> __ESIW_ST_BIT_COUNT);
		ASSERT(
				( dwScrollTypeH == __ESIW_ST_VIRTUAL && m_nCountOfColumns == -1L )
			||	( dwScrollTypeH != __ESIW_ST_VIRTUAL && m_nCountOfColumns >= 0L )
			);
		ASSERT(
				( dwScrollTypeV == __ESIW_ST_VIRTUAL && m_nCountOfRows == -1L )
			||	( dwScrollTypeV != __ESIW_ST_VIRTUAL && m_nCountOfRows >= 0L )
			);
	} // if( m_hWnd != NULL && ::IsWindow(m_hWnd) )
	ASSERT( m_nSizeOfExpandShadow >= 0 );
}
void CExtGridBaseWnd::Dump( CDumpContext & dc ) const
{
	CExtScrollItemWnd::Dump( dc );
}
#endif // _DEBUG

void CExtGridBaseWnd::PreSubclassWindow() 
{
	ASSERT_VALID( this );
	m_rcClient.SetRect( 0, 0, 0, 0 );
	CExtScrollItemWnd::PreSubclassWindow();
DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	ColumnCountSet(
		( dwScrollTypeH == __ESIW_ST_VIRTUAL ) ? -1L : 0L,
		false
		);
	RowCountSet(
		( dwScrollTypeV == __ESIW_ST_VIRTUAL ) ? -1L : 0L,
		false
		);
}

CExtGridHitTestInfo & CExtGridBaseWnd::HitTest(
	CExtGridHitTestInfo & htInfo,
	bool bReAlignCellResizing,
	bool bSupressZeroTopCellResizing,
	bool bComputeOuterDropAfterState // = false
	) const
{
	ASSERT_VALID( this );
	htInfo.m_dwAreaFlags = __EGBWA_NOWHERE;
	if(	m_hWnd == NULL || (! ::IsWindow(m_hWnd) ) )
		return htInfo;
CDC dcEmpty;
	OnGbwWalkVisibleAreas(
		dcEmpty,
		false,
		&htInfo
		);
	if(		( htInfo.m_dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0
		&&	( htInfo.m_nColNo >= 0 || htInfo.m_nRowNo >= 0 )
		)
	{
		ASSERT( ! (htInfo.m_nColNo < 0 && htInfo.m_nRowNo < 0 ) );
		if( htInfo.m_nColNo < 0 )
			htInfo.m_nColNo = (-htInfo.m_nColNo) - 1;
		else
			htInfo.m_nRowNo = (-htInfo.m_nRowNo) - 1;
		ASSERT( htInfo.m_nColNo >= 0 );
		ASSERT( htInfo.m_nRowNo >= 0 );
	} // if( ( htInfo.m_dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0 ...
	if( ( htInfo.m_dwAreaFlags & (__EGBWA_OUTER_CELLS|__EGBWA_INNER_CELLS) ) != 0 )
	{
		if( bReAlignCellResizing )
		{
			if(		htInfo.m_nColNo > 0
				&&	( htInfo.m_dwAreaFlags & __EGBWA_NEAR_CELL_BORDER_LEFT ) != 0
				)
			{
				htInfo.m_nColNo --;
				htInfo.m_dwAreaFlags &= ~__EGBWA_NEAR_CELL_BORDER_LEFT;
				htInfo.m_dwAreaFlags |= __EGBWA_NEAR_CELL_BORDER_RIGHT;
				//htInfo.m_rcItem.right = htInfo.m_rcItem.left;
				//htInfo.m_rcItem.left -= OnSiwQueryItemExtentH( htInfo.m_nColNo );
				OnGbwCalcVisibleItemRect(
					htInfo.m_nColNo,
					htInfo.m_nRowNo,
					htInfo.GetInnerOuterTypeOfColumn(),
					htInfo.GetInnerOuterTypeOfRow(),
					htInfo.m_rcExtra,
					htInfo.m_rcItem,
					&htInfo.m_nVisibleColNo,
					&htInfo.m_nVisibleRowNo
					);
			}
			if(		htInfo.m_nRowNo > 0
				&&	( htInfo.m_dwAreaFlags & __EGBWA_NEAR_CELL_BORDER_TOP ) != 0
				)
			{
				htInfo.m_nRowNo --;
				htInfo.m_dwAreaFlags &= ~__EGBWA_NEAR_CELL_BORDER_TOP;
				htInfo.m_dwAreaFlags |= __EGBWA_NEAR_CELL_BORDER_BOTTOM;
				//htInfo.m_rcItem.bottom = htInfo.m_rcItem.top;
				//htInfo.m_rcItem.top -= OnSiwQueryItemExtentV( htInfo.m_nRowNo );
				OnGbwCalcVisibleItemRect(
					htInfo.m_nColNo,
					htInfo.m_nRowNo,
					htInfo.GetInnerOuterTypeOfColumn(),
					htInfo.GetInnerOuterTypeOfRow(),
					htInfo.m_rcExtra,
					htInfo.m_rcItem,
					&htInfo.m_nVisibleColNo,
					&htInfo.m_nVisibleRowNo
					);
			}
		} // if( bReAlignCellResizing )
		if( bSupressZeroTopCellResizing )
		{
			if(		htInfo.m_nColNo == 0
				&&	( htInfo.m_dwAreaFlags & __EGBWA_NEAR_CELL_BORDER_LEFT ) != 0
				)
				htInfo.m_dwAreaFlags &= ~__EGBWA_NEAR_CELL_BORDER_LEFT;
			if(		htInfo.m_nRowNo == 0
				&&	( htInfo.m_dwAreaFlags & __EGBWA_NEAR_CELL_BORDER_TOP ) != 0
				)
				htInfo.m_dwAreaFlags &= ~__EGBWA_NEAR_CELL_BORDER_TOP;
		} // if( bSupressZeroTopCellResizing )
	} // if( ( htInfo.m_dwAreaFlags & (__EGBWA_OUTER_CELLS|__EGBWA_INNER_CELLS) ) != 0 )
	if(		bComputeOuterDropAfterState
		&&	(htInfo.m_dwAreaFlags&__EGBWA_OUTER_CELLS) != 0
		&&	( ! htInfo.IsHoverEmpty() )
		)
	{
		CRect rcHalfAfter( htInfo.m_rcItem );
		if( htInfo.GetInnerOuterTypeOfColumn() != 0 )
			rcHalfAfter.top += rcHalfAfter.Height() / 2;
		else
			rcHalfAfter.left += rcHalfAfter.Width() / 2;
		if( rcHalfAfter.PtInRect(htInfo.m_ptClient) )
			htInfo.m_dwAreaFlags |= __EGBWA_OUTER_DROP_AFTER;
	} // if( bComputeOuterDropAfterState ...
	return htInfo;
}

LONG CExtGridBaseWnd::OuterColumnCountLeftGet() const
{
	ASSERT_VALID( this );
	return m_nCountOfOuterColumnsL;
}

LONG CExtGridBaseWnd::OuterColumnCountRightGet() const
{
	ASSERT_VALID( this );
	return m_nCountOfOuterColumnsR;
}

LONG CExtGridBaseWnd::OuterColumnCountLeftSet(
	LONG nCount, // = 0L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( nCount >= 0 );
LONG nCountOfOuterColumnsLeftOld = OuterColumnCountLeftGet();
	if( nCountOfOuterColumnsLeftOld != nCount )
	{
		m_nCountOfOuterColumnsL = nCount;
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
	} // if( nCountOfOuterColumnsLeftOld != nCount )
	return nCountOfOuterColumnsLeftOld;
}

LONG CExtGridBaseWnd::OuterColumnCountRightSet(
	LONG nCount, // = 0L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( nCount >= 0 );
LONG nCountOfOuterColumnsRightOld = OuterColumnCountRightGet();
	if( nCountOfOuterColumnsRightOld != nCount )
	{
		m_nCountOfOuterColumnsR = nCount;
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
	} // if( nCountOfOuterColumnsRightOld != nCount )
	return nCountOfOuterColumnsRightOld;
}

LONG CExtGridBaseWnd::OuterRowCountTopGet() const
{
	ASSERT_VALID( this );
	return m_nCountOfOuterRowsT;
}

LONG CExtGridBaseWnd::OuterRowCountBottomGet() const
{
	ASSERT_VALID( this );
	return m_nCountOfOuterRowsB;
}

LONG CExtGridBaseWnd::OuterRowCountTopSet(
	LONG nCount, // = 0L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( nCount >= 0 );
LONG nCountOfOuterRowsTopOld = OuterRowCountTopGet();
	if( nCountOfOuterRowsTopOld != nCount )
	{
		m_nCountOfOuterRowsT = nCount;
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
	} // if( nCountOfOuterRowsTopOld != nCount )
	return nCountOfOuterRowsTopOld;
}

LONG CExtGridBaseWnd::OuterRowCountBottomSet(
	LONG nCount, // = 0L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( nCount >= 0 );
LONG nCountOfOuterRowsBottomOld = OuterRowCountBottomGet();
	if( nCountOfOuterRowsBottomOld != nCount )
	{
		m_nCountOfOuterRowsB = nCount;
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
	} // if( nCountOfOuterRowsBottomOld != nCount )
	return nCountOfOuterRowsBottomOld;
}

LONG CExtGridBaseWnd::ColumnCountGet() const
{
	ASSERT_VALID( this );
	return m_nCountOfColumns;
}

LONG CExtGridBaseWnd::ColumnCountSet(
	LONG nCount, // = -1L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( nCount == -1L || nCount >= 0 );
LONG nCountOfColumnsOld = ColumnCountGet();
	if( nCountOfColumnsOld != nCount )
	{
		DWORD dwScrollTypeH = SiwScrollTypeHGet();
		m_nCountOfColumns = nCount;
		SiwScrollTypeHSet(
			( nCount == -1L )
				? __ESIW_ST_VIRTUAL
				: (		(dwScrollTypeH == __ESIW_ST_VIRTUAL)
							? m_dwDefaultScrollTypeH
							: dwScrollTypeH
					)
				,
			false
			);
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
	} // if( nCountOfColumnsOld != nCount )
	return nCountOfColumnsOld;
}

bool CExtGridBaseWnd::ColumnInsert(
	LONG nColNo, // -1 or greater than count - append
	LONG nColInsertCount, // = 1L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( nColInsertCount < 0 )
	{
		// invalid arg
		ASSERT( FALSE );
		return false;
	} // if( nColInsertCount < 0 )
DWORD dwScrollTypeH = SiwScrollTypeHGet();
	if( dwScrollTypeH == __ESIW_ST_VIRTUAL )
	{
		// virtual mode should not use this method
		ASSERT( FALSE );
		return false;
	} // if( dwScrollTypeH == __ESIW_ST_VIRTUAL )
LONG nColCount = ColumnCountGet();
	if( nColCount < 0 )
	{
		// virtual mode should not use this method
		ASSERT( FALSE );
		return false;
	} // if( nColCount < 0 )
	if( nColInsertCount == 0 )
	{
		// nothing to insert
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
		return true;
	} // if( nColInsertCount == 0 )
	OnGbwHoverRemove();
	if( nColNo < 0 || nColNo > nColCount )
		nColNo = nColCount;
LONG nColCountOld = nColCount;
	nColCount += nColInsertCount;
	ColumnCountSet( nColCount, false );
	if( nColNo < nColCountOld )
	{
		CPoint ptFocus = FocusGet();
		if( ptFocus.x >= nColNo )
		{
			ptFocus.x += nColInsertCount;
			FocusSet( ptFocus, false, false, false, false );
		} // if( ptFocus.x >= nColNo )
		INT nAreaCount = SelectionGetAreaCount();
		for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
		{
			CRect rcSelectionOld( SelectionGet( false, nAreaNumber ) );
			CRect rcSelectionNew( rcSelectionOld );
			if( rcSelectionNew.left >= nColNo )
				rcSelectionNew.left += nColInsertCount;
			if( rcSelectionNew.right >= nColNo )
				rcSelectionNew.right += nColInsertCount;
			if( rcSelectionOld != rcSelectionNew )
			{
				VERIFY(
					SelectionSetAt(
						nAreaNumber,
						rcSelectionNew,
						false
						)
					);
			} // if( rcSelectionOld != rcSelectionNew )
		} // for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
	} // if( nColNo < nColCountOld )
	if( bRedraw )
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	} // if( bRedraw )
	return true;
}

LONG CExtGridBaseWnd::ColumnRemove(
	LONG nColNo,
	LONG nColRemoveCount, // = 1L // -1 - remove up to end
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( nColNo < 0 )
	{
		// invalid arg
		ASSERT( FALSE );
		return 0L;
	} // if( nColNo < 0 )
DWORD dwScrollTypeH = SiwScrollTypeHGet();
	if( dwScrollTypeH == __ESIW_ST_VIRTUAL )
	{
		// virtual mode should not use this method
		ASSERT( FALSE );
		return 0L;
	} // if( dwScrollTypeH == __ESIW_ST_VIRTUAL )
LONG nColCount = ColumnCountGet();
	if( nColCount < 0 )
	{
		// virtual mode should not use this method
		ASSERT( FALSE );
		return 0L;
	} // if( nColCount < 0 )
	if( nColNo >= nColCount )
	{
		// invalid arg
		ASSERT( FALSE );
		return 0L;
	} // if( nColNo >= nColCount )
	OnGbwHoverRemove();
	if( nColRemoveCount < 0 )
		nColRemoveCount = nColCount;
LONG nAvailToRemove = nColCount - nColNo;
	if( nColRemoveCount > nAvailToRemove )
		nColRemoveCount = nAvailToRemove;
	if( nColRemoveCount == 0 )
	{
		// nothing to remove
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
		return 0L;
	} // if( nColRemoveCount == 0 )
	nColCount -= nColRemoveCount;
	ASSERT( nColCount >= 0 );
	ColumnCountSet( nColCount, false );
	if( nColCount == 0 )
	{
		FocusUnset( false );
		SelectionUnset( false, false );
	} // if( nColCount == 0 )
	else
	{
		LONG nColLast = nColNo + nColRemoveCount - 1;
		ASSERT( nColLast >= nColNo );
		CPoint ptFocus = FocusGet();
		if( ptFocus.x >= nColNo )
		{
			if( ptFocus.x <= nColLast )
			{
				FocusUnset( false );
			} // if( ptFocus.x <= nColLast )
			else
			{
				ptFocus.x -= nColRemoveCount;
				ASSERT( ptFocus.x >= 0 );
				ASSERT( ptFocus.x < nColCount );
				FocusSet( ptFocus, false, false, false, false );
			} // else from if( ptFocus.x <= nColLast )
		} // if( ptFocus.x >= nColNo )
		INT nAreaCount = SelectionGetAreaCount();
		for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; )
		{
			CRect rcSelectionOld( SelectionGet( false, nAreaNumber ) );
			if(		rcSelectionOld.left >= nColNo
				&&	rcSelectionOld.left <= nColLast
				&&	rcSelectionOld.right >= nColNo
				&&	rcSelectionOld.right <= nColLast
				)
			{
				VERIFY( SelectionRemoveAt( nAreaNumber, false ) );
				nAreaCount--;
				continue;
			}
			CRect rcSelectionNew( rcSelectionOld );
			if( rcSelectionNew.left >= nColNo )
			{
				if( rcSelectionNew.left <= nColLast )
				{
					ASSERT(
							rcSelectionOld.right < nColNo
						||	rcSelectionOld.right > nColLast
						);
					rcSelectionNew.left = nColNo;
					ASSERT( rcSelectionNew.left <= nColCount );
					if( rcSelectionNew.left == nColCount )
						rcSelectionNew.left = nColCount-1;
					ASSERT( rcSelectionNew.left >= 0 );
				} // if( rcSelectionNew.left <= nColLast )
				else
				{
					rcSelectionNew.left -= nColRemoveCount;
					ASSERT( rcSelectionNew.left >= 0 );
					ASSERT( rcSelectionNew.left < nColCount );
				} // else from if( rcSelectionNew.left <= nColLast )
			} // if( rcSelectionNew.left >= nColNo )
			if( rcSelectionNew.right >= nColNo )
			{
				if( rcSelectionNew.right <= nColLast )
				{
					ASSERT(
							rcSelectionOld.left < nColNo
						||	rcSelectionOld.left > nColLast
						);
					rcSelectionNew.right = nColNo;
					ASSERT( rcSelectionNew.right <= nColCount );
					if( rcSelectionNew.right == nColCount )
						rcSelectionNew.right = nColCount-1;
					ASSERT( rcSelectionNew.right >= 0 );
				} // if( rcSelectionNew.right <= nColLast )
				else
				{
					rcSelectionNew.right -= nColRemoveCount;
					ASSERT( rcSelectionNew.right >= 0 );
					ASSERT( rcSelectionNew.right < nColCount );
				} // else from if( rcSelectionNew.right <= nColLast )
			} // if( rcSelectionNew.right >= nColNo )
			if( rcSelectionOld != rcSelectionNew )
			{
				VERIFY(
					SelectionSetAt(
						nAreaNumber,
						rcSelectionNew,
						false
						)
					);
			} // if( rcSelectionOld != rcSelectionNew )
			nAreaNumber++;
		} // for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; )
	} // else from if( nColCount == 0 )
	if( bRedraw )
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	} // if( bRedraw )
	return nColRemoveCount;
}

LONG CExtGridBaseWnd::RowCountGet() const
{
	ASSERT_VALID( this );
	return m_nCountOfRows;
}

LONG CExtGridBaseWnd::RowCountSet(
	LONG nCount, // = -1L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( nCount == -1L || nCount >= 0 );
LONG nCountOfRowsOld = RowCountGet();
	if( nCountOfRowsOld != nCount )
	{
		DWORD dwScrollTypeV = SiwScrollTypeVGet();
		m_nCountOfRows = nCount;
		SiwScrollTypeVSet(
			( nCount == -1L )
				? __ESIW_ST_VIRTUAL
				: (		(dwScrollTypeV == __ESIW_ST_VIRTUAL)
							? m_dwDefaultScrollTypeV
							: dwScrollTypeV
					)
				,
			false
			);
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
	} // if( nCountOfRowsOld != nCount )
	return nCountOfRowsOld;
}

bool CExtGridBaseWnd::RowInsert(
	LONG nRowNo, // -1 or greater than count - append
	LONG nRowInsertCount, // = 1L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( nRowInsertCount < 0 )
	{
		// invalid arg
		ASSERT( FALSE );
		return false;
	} // if( nRowInsertCount < 0 )
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	if( dwScrollTypeV == __ESIW_ST_VIRTUAL )
	{
		// virtual mode should not use this method
		ASSERT( FALSE );
		return false;
	} // if( dwScrollTypeV == __ESIW_ST_VIRTUAL )
LONG nRowCount = RowCountGet();
	if( nRowCount < 0 )
	{
		// virtual mode should not use this method
		ASSERT( FALSE );
		return false;
	} // if( nRowCount < 0 )
	if( nRowInsertCount == 0 )
	{
		// nothing to insert
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
		return true;
	} // if( nRowInsertCount == 0 )
	OnGbwHoverRemove();
	if( nRowNo < 0 || nRowNo > nRowCount )
		nRowNo = nRowCount;
LONG nRowCountOld = nRowCount;
	nRowCount += nRowInsertCount;
	RowCountSet( nRowCount, false );
	if( nRowNo < nRowCountOld )
	{
		CPoint ptFocus = FocusGet();
		if( ptFocus.y >= nRowNo )
		{
			ptFocus.y += nRowInsertCount;
			FocusSet( ptFocus, false, false, false, false );
		} // if( ptFocus.y >= nRowNo )
		INT nAreaCount = SelectionGetAreaCount();
		for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
		{
			CRect rcSelectionOld( SelectionGet( false, nAreaNumber ) );
			CRect rcSelectionNew( rcSelectionOld );
			if( rcSelectionNew.top >= nRowNo )
				rcSelectionNew.top += nRowInsertCount;
			if( rcSelectionNew.bottom >= nRowNo )
				rcSelectionNew.bottom += nRowInsertCount;
			if( rcSelectionOld != rcSelectionNew )
			{
				VERIFY(
					SelectionSetAt(
						nAreaNumber,
						rcSelectionNew,
						false
						)
					);
			} // if( rcSelectionOld != rcSelectionNew )
		} // for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
	} // if( nRowNo < nRowCountOld )
	if( bRedraw )
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	} // if( bRedraw )
	return true;
}

LONG CExtGridBaseWnd::RowRemove(
	LONG nRowNo,
	LONG nRowRemoveCount, // = 1L // -1 - remove up to end
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( nRowNo < 0 )
	{
		// invalid arg
		ASSERT( FALSE );
		return 0L;
	} // if( nRowNo < 0 )
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	if( dwScrollTypeV == __ESIW_ST_VIRTUAL )
	{
		// virtual mode should not use this method
		ASSERT( FALSE );
		return 0L;
	} // if( dwScrollTypeV == __ESIW_ST_VIRTUAL )
LONG nRowCount = RowCountGet();
	if( nRowCount < 0 )
	{
		// virtual mode should not use this method
		ASSERT( FALSE );
		return 0L;
	} // if( nRowCount < 0 )
	if( nRowNo >= nRowCount )
	{
		// invalid arg
		ASSERT( FALSE );
		return 0L;
	} // if( nRowNo >= nRowCount )
	OnGbwHoverRemove();
	if( nRowRemoveCount < 0 )
		nRowRemoveCount = nRowCount;
LONG nAvailToRemove = nRowCount - nRowNo;
	if( nRowRemoveCount > nAvailToRemove )
		nRowRemoveCount = nAvailToRemove;
	if( nRowRemoveCount == 0 )
	{
		// nothing to remove
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
		return 0L;
	} // if( nRowRemoveCount == 0 )
	nRowCount -= nRowRemoveCount;
	ASSERT( nRowCount >= 0 );
	RowCountSet( nRowCount, false );
	if( nRowCount == 0 )
	{
		FocusUnset( false );
		SelectionUnset( false, false );
	} // if( nRowCount == 0 )
	else
	{
		LONG nRowLast = nRowNo + nRowRemoveCount - 1;
		ASSERT( nRowLast >= nRowNo );
		CPoint ptFocus = FocusGet();
		if( ptFocus.y >= nRowNo )
		{
			if( ptFocus.y <= nRowLast )
			{
				FocusUnset( false );
			} // if( ptFocus.y <= nRowLast )
			else
			{
				ptFocus.y -= nRowRemoveCount;
				ASSERT( ptFocus.y >= 0 );
				ASSERT( ptFocus.y < nRowCount );
				FocusSet( ptFocus, false, false, false, false );
			} // else from if( ptFocus.y <= nRowLast )
		} // if( ptFocus.y >= nRowNo )
		INT nAreaCount = SelectionGetAreaCount();
		for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; )
		{
			CRect rcSelectionOld( SelectionGet( false, nAreaNumber ) );
			if(		rcSelectionOld.top >= nRowNo
				&&	rcSelectionOld.top <= nRowLast
				&&	rcSelectionOld.bottom >= nRowNo
				&&	rcSelectionOld.bottom <= nRowLast
				)
			{
				VERIFY( SelectionRemoveAt( nAreaNumber, false ) );
				nAreaCount--;
				continue;
			}
			CRect rcSelectionNew( rcSelectionOld );
			if( rcSelectionNew.top >= nRowNo )
			{
				if( rcSelectionNew.top <= nRowLast )
				{
					ASSERT(
							rcSelectionOld.bottom < nRowNo
						||	rcSelectionOld.bottom > nRowLast
						);
					rcSelectionNew.top = nRowNo;
					ASSERT( rcSelectionNew.top <= nRowCount );
					if( rcSelectionNew.top == nRowCount )
						rcSelectionNew.top = nRowCount-1;
					ASSERT( rcSelectionNew.top >= 0 );
				} // if( rcSelectionNew.top <= nRowLast )
				else
				{
					rcSelectionNew.top -= nRowRemoveCount;
					ASSERT( rcSelectionNew.top >= 0 );
					ASSERT( rcSelectionNew.top < nRowCount );
				} // else from if( rcSelectionNew.top <= nRowLast )
			} // if( rcSelectionNew.top >= nRowNo )
			if( rcSelectionNew.bottom >= nRowNo )
			{
				if( rcSelectionNew.bottom <= nRowLast )
				{
					ASSERT(
							rcSelectionOld.top < nRowNo
						||	rcSelectionOld.top > nRowLast
						);
					rcSelectionNew.bottom = nRowNo;
					ASSERT( rcSelectionNew.bottom <= nRowCount );
					if( rcSelectionNew.bottom == nRowCount )
						rcSelectionNew.bottom = nRowCount-1;
					ASSERT( rcSelectionNew.bottom >= 0 );
				} // if( rcSelectionNew.bottom <= nRowLast )
				else
				{
					rcSelectionNew.bottom -= nRowRemoveCount;
					ASSERT( rcSelectionNew.bottom >= 0 );
					ASSERT( rcSelectionNew.bottom < nRowCount );
				} // else from if( rcSelectionNew.bottom <= nRowLast )
			} // if( rcSelectionNew.bottom >= nRowNo )
			if( rcSelectionOld != rcSelectionNew )
			{
				VERIFY(
					SelectionSetAt(
						nAreaNumber,
						rcSelectionNew,
						false
						)
					);
			} // if( rcSelectionOld != rcSelectionNew )
			nAreaNumber++;
		} // for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; )
	} // else from if( nRowCount == 0 )
	if( bRedraw )
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	} // if( bRedraw )
	return nRowRemoveCount;
}

INT CExtGridBaseWnd::DefaultColumnWidthGet() const
{
	ASSERT_VALID( this );
	return m_sizeCellDefault.cx;
}

INT CExtGridBaseWnd::DefaultColumnWidthSet(
	INT nColumnWidth,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
INT nColumnWidthOld = DefaultColumnWidthGet();
	if( nColumnWidthOld != nColumnWidth )
	{
		m_sizeCellDefault.cx = nColumnWidth;
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
	} // if( nColumnWidthOld != nColumnWidth )
	return nColumnWidthOld;
}

INT CExtGridBaseWnd::DefaultRowHeightGet() const
{
	ASSERT_VALID( this );
	return m_sizeCellDefault.cy;
}

INT CExtGridBaseWnd::DefaultRowHeightSet(
	INT nRowHeight,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
INT nRowHeightOld = DefaultRowHeightGet();
	if( nRowHeightOld != nRowHeight )
	{
		m_sizeCellDefault.cy = nRowHeight;
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
	} // if( nRowHeightOld != nRowHeight )
	return nRowHeightOld;
}

bool CExtGridBaseWnd::FixedSizeColumnsGet() const
{
	ASSERT_VALID( this );
bool bFixedSizeColumns =
		( SiwGetStyle() & __EGBS_FIXED_SIZE_COLUMNS )
			? true : false;
	return bFixedSizeColumns;
}

bool CExtGridBaseWnd::FixedSizeColumnsSet(
	bool bFixedSizeColumns,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
bool bFixedSizeColumnsOld = FixedSizeColumnsGet();
	if(	!(		(bFixedSizeColumnsOld && bFixedSizeColumns)
			||	((!bFixedSizeColumnsOld) && (!bFixedSizeColumns))
			)
		)
		SiwModifyStyle(
			bFixedSizeColumns ? __EGBS_FIXED_SIZE_COLUMNS : 0L,
			__EGBS_FIXED_SIZE_COLUMNS,
			bRedraw
			);
	return bFixedSizeColumnsOld;
}

bool CExtGridBaseWnd::FixedSizeRowsGet() const
{
	ASSERT_VALID( this );
bool bFixedSizeRows =
		( SiwGetStyle() & __EGBS_FIXED_SIZE_ROWS )
			? true : false;
	return bFixedSizeRows;
}

bool CExtGridBaseWnd::FixedSizeRowsSet(
	bool bFixedSizeRows,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
bool bFixedSizeRowsOld = FixedSizeRowsGet();
	if(	!(		(bFixedSizeRowsOld && bFixedSizeRows)
			||	((!bFixedSizeRowsOld) && (!bFixedSizeRows))
			)
		)
		SiwModifyStyle(
			bFixedSizeRows ? __EGBS_FIXED_SIZE_ROWS : 0L,
			__EGBS_FIXED_SIZE_ROWS,
			bRedraw
			);
	return bFixedSizeRowsOld;
}

void CExtGridBaseWnd::OnGbwEraseArea(
	CDC & dc,
	const RECT & rcArea,
	DWORD dwAreaFlags
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
//COLORREF clrDebugFill = COLORREF( -1L );
//	switch( dwAreaFlags )
//	{
//	case __EGBWA_OUTER_CELLS|__EGBWA_OUTER_TOP:							clrDebugFill = RGB( 255, 128, 128 ); break;
//	case __EGBWA_OUTER_CELLS|__EGBWA_OUTER_BOTTOM:						clrDebugFill = RGB( 128, 255, 128 ); break;
//	case __EGBWA_OUTER_CELLS|__EGBWA_OUTER_LEFT:						clrDebugFill = RGB( 128, 128, 255 ); break;
//	case __EGBWA_OUTER_CELLS|__EGBWA_OUTER_RIGHT:						clrDebugFill = RGB( 128, 255, 255 ); break;
//	case __EGBWA_OUTER_CELLS|__EGBWA_OUTER_LEFT|__EGBWA_OUTER_TOP:		clrDebugFill = RGB( 255, 128, 255 ); break;
//	case __EGBWA_OUTER_CELLS|__EGBWA_OUTER_LEFT|__EGBWA_OUTER_BOTTOM:	clrDebugFill = RGB( 255, 255, 128 ); break;
//	case __EGBWA_OUTER_CELLS|__EGBWA_OUTER_RIGHT|__EGBWA_OUTER_TOP:		clrDebugFill = RGB( 255, 255, 128 ); break;
//	case __EGBWA_OUTER_CELLS|__EGBWA_OUTER_RIGHT|__EGBWA_OUTER_BOTTOM:	clrDebugFill = RGB( 128, 128, 128 ); break;
//	case __EGBWA_INNER_CELLS:											clrDebugFill = RGB( 255, 255, 224 ); break;
//	} // switch( dwAreaFlags )
//	if( clrDebugFill != COLORREF( -1L ) )
//		dc.FillSolidRect( &rcArea, clrDebugFill );
	switch( dwAreaFlags )
	{
	case __EGBWA_OUTER_CELLS|__EGBWA_OUTER_LEFT|__EGBWA_OUTER_TOP:
	case __EGBWA_OUTER_CELLS|__EGBWA_OUTER_LEFT|__EGBWA_OUTER_BOTTOM:
	case __EGBWA_OUTER_CELLS|__EGBWA_OUTER_RIGHT|__EGBWA_OUTER_TOP:
	case __EGBWA_OUTER_CELLS|__EGBWA_OUTER_RIGHT|__EGBWA_OUTER_BOTTOM:
	{
		DWORD dwSiwStyleEx = SiwGetStyleEx();
		if( (dwSiwStyleEx&__EGBS_EX_CORNER_AREAS_3D) != 0 )
		{
			COLORREF clrLT = OnSiwGetSysColor( COLOR_3DHIGHLIGHT );
			COLORREF clrRB = OnSiwGetSysColor( COLOR_3DSHADOW );
			if( (dwSiwStyleEx&__EGBS_EX_CORNER_AREAS_CURVE) != 0 )
			{
				DWORD dwScrollTypeH = SiwScrollTypeHGet();
				DWORD dwScrollTypeV = SiwScrollTypeVGet();
				switch( dwAreaFlags )
				{
				case __EGBWA_OUTER_CELLS|__EGBWA_OUTER_LEFT|__EGBWA_OUTER_TOP:
				{
					CRect rcDrawArea( rcArea );
					dc.FillSolidRect(
						rcDrawArea.left, rcDrawArea.top,
						1, rcDrawArea.Height(),
						clrLT
						);
					dc.FillSolidRect(
						rcDrawArea.left, rcDrawArea.top,
						rcDrawArea.Width(), 1,
						clrLT
						);
					bool bPlainMarginH = true, bPlainMarginV = true;
					if( dwScrollTypeH == __ESIW_ST_PIXEL && ScrollPos32Get(SB_HORZ) > 0 )
						bPlainMarginH = false;
					if( dwScrollTypeV == __ESIW_ST_PIXEL && ScrollPos32Get(SB_VERT) > 0 )
						bPlainMarginV = false;
					if( bPlainMarginH )
						dc.FillSolidRect(
							rcDrawArea.right-1, rcDrawArea.top,
							1, rcDrawArea.Height(),
							clrRB
							);
					else
					{
						int nShift = 1;
						bool bAscending = true;
						for( int i = 0; i < rcDrawArea.Height(); i++ )
						{
							dc.SetPixel(
								rcDrawArea.right - 1 - nShift,
								rcDrawArea.top + i,
								clrRB
								);
							i++;
							if( i == rcDrawArea.Height() )
								break;
							dc.SetPixel(
								rcDrawArea.right - 1 - nShift,
								rcDrawArea.top + i,
								clrRB
								);
							if( bAscending )
								nShift ++;
							else
								nShift --;
							if( nShift == 2 || nShift < 0 )
							{
								nShift = bAscending ? (nShift-1) : (nShift+2);
								bAscending = ( ! bAscending );
							}
						} // for( int i = 0; i < rcDrawArea.Height(); i++ )
					} // else from if( bPlainMarginH )
					if( bPlainMarginV )
						dc.FillSolidRect(
							rcDrawArea.left, rcDrawArea.bottom-1,
							rcDrawArea.Width(), 1,
							clrRB
							);
					else
					{
						int nShift = 1;
						bool bAscending = true;
						for( int i = 0; i < rcDrawArea.Width(); i++ )
						{
							dc.SetPixel(
								rcDrawArea.left + i,
								rcDrawArea.bottom - 1 - nShift,
								clrRB
								);
							i++;
							if( i == rcDrawArea.Width() )
								break;
							dc.SetPixel(
								rcDrawArea.left + i,
								rcDrawArea.bottom - 1 - nShift,
								clrRB
								);
							if( bAscending )
								nShift ++;
							else
								nShift --;
							if( nShift == 2 || nShift < 0 )
							{
								nShift = bAscending ? (nShift-1) : (nShift+2);
								bAscending = ( ! bAscending );
							}
						} // for( int i = 0; i < rcDrawArea.Width(); i++ )
					} // else from if( bPlainMarginV )
				} // case __EGBWA_OUTER_CELLS|__EGBWA_OUTER_LEFT|__EGBWA_OUTER_TOP:
				break;
				case __EGBWA_OUTER_CELLS|__EGBWA_OUTER_RIGHT|__EGBWA_OUTER_TOP:
				{
					CRect rcDrawArea( rcArea );
					dc.FillSolidRect(
						rcDrawArea.left, rcDrawArea.top,
						rcDrawArea.Width(), 1,
						clrLT
						);
					dc.FillSolidRect(
						rcDrawArea.right-1, rcDrawArea.top,
						1, rcDrawArea.Height(),
						clrRB
						);
					bool bPlainMarginH = true, bPlainMarginV = true;
					if( dwScrollTypeH == __ESIW_ST_PIXEL && OnSwHasScrollBar(true) )
					{
						LONG nMaxPos = 0L, nPageSize = 0L, nCurrPos = 0L;
						ScrollInfo32Get(
							SB_HORZ,
							NULL,
							&nMaxPos,
							&nPageSize,
							&nCurrPos,
							NULL
							);
						if( (nCurrPos+nPageSize) < nMaxPos )
							bPlainMarginH = false;
					} // if( dwScrollTypeH == __ESIW_ST_PIXEL && OnSwHasScrollBar(true) )
					//else if(
					//		dwScrollTypeH == __ESIW_ST_NONE
					//	&&	OnSwGetClientRect().Width() < OnSwGetTotalSize().cx
					//	)
					//	bPlainMarginH = false;
					if( dwScrollTypeV == __ESIW_ST_PIXEL && ScrollPos32Get(SB_VERT) > 0 )
						bPlainMarginV = false;
					if( bPlainMarginH )
						dc.FillSolidRect(
							rcDrawArea.left, rcDrawArea.top,
							1, rcDrawArea.Height(),
							clrLT
							);
					else
					{
						int nShift = 1;
						bool bAscending = true;
						for( int i = 0; i < rcDrawArea.Height(); i++ )
						{
							dc.SetPixel(
								rcDrawArea.left - 1 + nShift,
								rcDrawArea.top + i,
								clrRB
								);
							i++;
							if( i == rcDrawArea.Height() )
								break;
							dc.SetPixel(
								rcDrawArea.left - 1 + nShift,
								rcDrawArea.top + i,
								clrRB
								);
							if( bAscending )
								nShift ++;
							else
								nShift --;
							if( nShift == 2 || nShift < 0 )
							{
								nShift = bAscending ? (nShift-1) : (nShift+2);
								bAscending = ( ! bAscending );
							}
						} // for( int i = 0; i < rcDrawArea.Height(); i++ )
					} // else from if( bPlainMarginH )
					if( bPlainMarginV )
						dc.FillSolidRect(
							rcDrawArea.left, rcDrawArea.bottom-1,
							rcDrawArea.Width(), 1,
							clrRB
							);
					else
					{
						int nShift = 1;
						bool bAscending = true;
						for( int i = 0; i < rcDrawArea.Width(); i++ )
						{
							dc.SetPixel(
								rcDrawArea.left + i,
								rcDrawArea.bottom - 1 - nShift,
								clrRB
								);
							i++;
							if( i == rcDrawArea.Width() )
								break;
							dc.SetPixel(
								rcDrawArea.left + i,
								rcDrawArea.bottom - 1 - nShift,
								clrRB
								);
							if( bAscending )
								nShift ++;
							else
								nShift --;
							if( nShift == 2 || nShift < 0 )
							{
								nShift = bAscending ? (nShift-1) : (nShift+2);
								bAscending = ( ! bAscending );
							}
						} // for( int i = 0; i < rcDrawArea.Width(); i++ )
					} // else from if( bPlainMarginV )
				}
				break;
				case __EGBWA_OUTER_CELLS|__EGBWA_OUTER_LEFT|__EGBWA_OUTER_BOTTOM:
				{
					CRect rcDrawArea( rcArea );
					dc.FillSolidRect(
						rcDrawArea.left, rcDrawArea.top,
						1, rcDrawArea.Height(),
						clrLT
						);
					dc.FillSolidRect(
						rcDrawArea.left, rcDrawArea.bottom-1,
						rcDrawArea.Width(), 1,
						clrRB
						);
					bool bPlainMarginH = true, bPlainMarginV = true;
					if( dwScrollTypeH == __ESIW_ST_PIXEL && ScrollPos32Get(SB_HORZ) > 0 )
						bPlainMarginH = false;
					if( dwScrollTypeV == __ESIW_ST_PIXEL && OnSwHasScrollBar(false) )
					{
						LONG nMaxPos = 0L, nPageSize = 0L, nCurrPos = 0L;
						ScrollInfo32Get(
							SB_VERT,
							NULL,
							&nMaxPos,
							&nPageSize,
							&nCurrPos,
							NULL
							);
						if( (nCurrPos+nPageSize) < nMaxPos )
							bPlainMarginV = false;
					} // if( dwScrollTypeV == __ESIW_ST_PIXEL && OnSwHasScrollBar(false) )
					//else if(
					//		dwScrollTypeV == __ESIW_ST_NONE
					//	&&	OnSwGetClientRect().Height() < OnSwGetTotalSize().cy
					//	)
					//	bPlainMarginV = false;
					if( bPlainMarginH )
						dc.FillSolidRect(
							rcDrawArea.right-1, rcDrawArea.top,
							1, rcDrawArea.Height(),
							clrRB
							);
					else
					{
						int nShift = 1;
						bool bAscending = true;
						for( int i = 0; i < rcDrawArea.Height(); i++ )
						{
							dc.SetPixel(
								rcDrawArea.right - 1 - nShift,
								rcDrawArea.top + i,
								clrRB
								);
							i++;
							if( i == rcDrawArea.Height() )
								break;
							dc.SetPixel(
								rcDrawArea.right - 1 - nShift,
								rcDrawArea.top + i,
								clrRB
								);
							if( bAscending )
								nShift ++;
							else
								nShift --;
							if( nShift == 2 || nShift < 0 )
							{
								nShift = bAscending ? (nShift-1) : (nShift+2);
								bAscending = ( ! bAscending );
							}
						} // for( int i = 0; i < rcDrawArea.Height(); i++ )
					} // else from if( bPlainMarginH )
					if( bPlainMarginV )
						dc.FillSolidRect(
							rcDrawArea.left, rcDrawArea.top,
							rcDrawArea.Width(), 1,
							clrLT
							);
					else
					{
						int nShift = 1;
						bool bAscending = true;
						for( int i = 0; i < rcDrawArea.Width(); i++ )
						{
							dc.SetPixel(
								rcDrawArea.left + i,
								rcDrawArea.top + nShift,
								clrRB
								);
							i++;
							if( i == rcDrawArea.Width() )
								break;
							dc.SetPixel(
								rcDrawArea.left + i,
								rcDrawArea.top + nShift,
								clrRB
								);
							if( bAscending )
								nShift ++;
							else
								nShift --;
							if( nShift == 2 || nShift < 0 )
							{
								nShift = bAscending ? (nShift-1) : (nShift+2);
								bAscending = ( ! bAscending );
							}
						} // for( int i = 0; i < rcDrawArea.Width(); i++ )
					} // else from if( bPlainMarginV )
				}
				break;
				case __EGBWA_OUTER_CELLS|__EGBWA_OUTER_RIGHT|__EGBWA_OUTER_BOTTOM:
				{
					CRect rcDrawArea( rcArea );
					dc.FillSolidRect(
						rcDrawArea.right-1, rcDrawArea.top,
						1, rcDrawArea.Height(),
						clrRB
						);
					dc.FillSolidRect(
						rcDrawArea.left, rcDrawArea.bottom-1,
						rcDrawArea.Width(), 1,
						clrRB
						);
					bool bPlainMarginH = true, bPlainMarginV = true;
					if( dwScrollTypeH == __ESIW_ST_PIXEL && OnSwHasScrollBar(true) )
					{
						LONG nMaxPos = 0L, nPageSize = 0L, nCurrPos = 0L;
						ScrollInfo32Get(
							SB_HORZ,
							NULL,
							&nMaxPos,
							&nPageSize,
							&nCurrPos,
							NULL
							);
						if( (nCurrPos+nPageSize) < nMaxPos )
							bPlainMarginH = false;
					} // if( dwScrollTypeH == __ESIW_ST_PIXEL && OnSwHasScrollBar(true) )
					//else if(
					//		dwScrollTypeH == __ESIW_ST_NONE
					//	&&	OnSwGetClientRect().Width() < OnSwGetTotalSize().cx
					//	)
					//	bPlainMarginH = false;
					if( dwScrollTypeV == __ESIW_ST_PIXEL && OnSwHasScrollBar(false) )
					{
						LONG nMaxPos = 0L, nPageSize = 0L, nCurrPos = 0L;
						ScrollInfo32Get(
							SB_VERT,
							NULL,
							&nMaxPos,
							&nPageSize,
							&nCurrPos,
							NULL
							);
						if( (nCurrPos+nPageSize) < nMaxPos )
							bPlainMarginV = false;
					} // if( dwScrollTypeV == __ESIW_ST_PIXEL && OnSwHasScrollBar(false) )
					//else if(
					//		dwScrollTypeV == __ESIW_ST_NONE
					//	&&	OnSwGetClientRect().Height() < OnSwGetTotalSize().cy
					//	)
					//	bPlainMarginV = false;
					if( bPlainMarginH )
						dc.FillSolidRect(
							rcDrawArea.left, rcDrawArea.top,
							1, rcDrawArea.Height(),
							clrLT
							);
					else
					{
						int nShift = 1;
						bool bAscending = true;
						for( int i = 0; i < rcDrawArea.Height(); i++ )
						{
							dc.SetPixel(
								rcDrawArea.left - 1 + nShift,
								rcDrawArea.top + i,
								clrRB
								);
							i++;
							if( i == rcDrawArea.Height() )
								break;
							dc.SetPixel(
								rcDrawArea.left - 1 + nShift,
								rcDrawArea.top + i,
								clrRB
								);
							if( bAscending )
								nShift ++;
							else
								nShift --;
							if( nShift == 2 || nShift < 0 )
							{
								nShift = bAscending ? (nShift-1) : (nShift+2);
								bAscending = ( ! bAscending );
							}
						} // for( int i = 0; i < rcDrawArea.Height(); i++ )
					} // else from if( bPlainMarginH )
					if( bPlainMarginV )
						dc.FillSolidRect(
							rcDrawArea.left, rcDrawArea.top,
							rcDrawArea.Width(), 1,
							clrLT
							);
					else
					{
						int nShift = 1;
						bool bAscending = true;
						for( int i = 0; i < rcDrawArea.Width(); i++ )
						{
							dc.SetPixel(
								rcDrawArea.left + i,
								rcDrawArea.top + nShift,
								clrRB
								);
							i++;
							if( i == rcDrawArea.Width() )
								break;
							dc.SetPixel(
								rcDrawArea.left + i,
								rcDrawArea.top + nShift,
								clrRB
								);
							if( bAscending )
								nShift ++;
							else
								nShift --;
							if( nShift == 2 || nShift < 0 )
							{
								nShift = bAscending ? (nShift-1) : (nShift+2);
								bAscending = ( ! bAscending );
							}
						} // for( int i = 0; i < rcDrawArea.Width(); i++ )
					} // else from if( bPlainMarginV )
				}
				break;
				} // switch( dwAreaFlags )
			} // if( (dwSiwStyleEx&__EGBS_EX_CORNER_AREAS_CURVE) != 0 )
			else
			{
				dc.Draw3dRect( &rcArea, clrLT, clrRB );
			} // else from if( (dwSiwStyleEx&__EGBS_EX_CORNER_AREAS_CURVE) != 0 )
		} // if( (dwSiwStyleEx&__EGBS_EX_CORNER_AREAS_3D) != 0 )
	}
	break;
	} // switch( dwAreaFlags )
}

void CExtGridBaseWnd::OnSiwPaintBackground(
	CDC & dc,
	bool bFocusedControl
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	CExtScrollItemWnd::OnSiwPaintBackground(
		dc,
		bFocusedControl
		);
}

void CExtGridBaseWnd::OnSiwPaintForeground(
	CDC & dc,
	bool bFocusedControl
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
COLORREF clrText = OnSiwGetSysColor( COLOR_BTNTEXT );
int nOldBkMode = dc.SetBkMode( TRANSPARENT );
COLORREF clrTextOld = dc.SetTextColor( clrText );
CFont * pOldFont = dc.SelectObject( &( OnSiwGetDefaultFont() ) );
	OnGbwWalkVisibleAreas( dc, bFocusedControl );
	dc.SelectObject( pOldFont );
	dc.SetTextColor( clrTextOld );
	dc.SetBkMode( nOldBkMode );
}

bool CExtGridBaseWnd::OnGbwCalcVisibleItemRect(
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	RECT & rcCellExtra,
	RECT & rcCell,
	LONG * p_nVisibleColNo, // = NULL
	LONG * p_nVisibleRowNo // = NULL
	) const
{
	ASSERT_VALID( this );
	rcCellExtra.left = rcCellExtra.right
		= rcCellExtra.top = rcCellExtra.bottom
		= rcCell.left = rcCell.right
		= rcCell.top = rcCell.bottom
		= 0;
	ASSERT( nColType == 0 || nRowType == 0 );
	if( nColType != 0 && nRowType != 0 )
		return false;
CRect rcVisibleRange = OnSiwGetVisibleRange();
	if(		(	nColType == 0
			&&	(rcVisibleRange.left > rcVisibleRange.right)
			)
		||	(	nRowType == 0
			&&	(rcVisibleRange.top > rcVisibleRange.bottom)
			)
		)
		return false;
	if( p_nVisibleColNo != NULL )
		*p_nVisibleColNo = 0L;
	if( p_nVisibleRowNo != NULL )
		*p_nVisibleRowNo = 0L;
CRect rcCalcItem( 0, 0, 0, 0 ), rcCalcExtra( 0, 0, 0, 0 );
LONG	nColFirst = 0L, nColLast = 0L, nColFirstOffset = 0L,
		nRowFirst = 0L, nRowLast = 0L, nRowFirstOffset = 0L;
	if( nColType < 0 )
	{
		if( m_rcOuterL.IsRectEmpty() )
			return false;
		rcCalcItem.OffsetRect( -rcCalcItem.left, 0 );
		nColFirstOffset = m_rcOuterL.left;
		LONG nOuterCount = OuterColumnCountLeftGet();
		if( nOuterCount == 0L )
			return false;
		nColFirst = 0L;
		nColLast = nOuterCount - 1L;
		//ASSERT( nColFirst <= nColLast );
		if( nColNo < nColFirst || nColNo > nColLast )
			return false;
		for( LONG nWalkNo = nColFirst; nWalkNo <= nColNo; nWalkNo++ )
		{
			INT nItemExtent = OuterColumnWidthGet( true, nWalkNo );
			if( nWalkNo < nColNo )
			{
				if( p_nVisibleColNo != NULL )
					(*p_nVisibleColNo) ++;
				rcCalcItem.OffsetRect( nItemExtent, 0 );
			} // if( nWalkNo < nColNo )
			else
				rcCalcItem.right += nItemExtent;
		} // for( LONG nWalkNo = nColFirst; nWalkNo <= nColNo; nWalkNo++ )
		rcCalcExtra.left = rcCalcItem.left; 
		rcCalcExtra.right = rcCalcItem.right; 
	} // if( nColType < 0 )
	else if( nColType > 0 )
	{
		if( m_rcOuterR.IsRectEmpty() )
			return false;
		rcCalcItem.OffsetRect( -rcCalcItem.left, 0 );
		nColFirstOffset = m_rcOuterR.left;
		LONG nOuterCount = OuterColumnCountRightGet();
		if( nOuterCount == 0L )
			return false;
		nColFirst = 0L;
		nColLast = nOuterCount - 1L;
		//ASSERT( nColFirst <= nColLast );
		if( nColNo < nColFirst || nColNo > nColLast )
			return false;
		for( LONG nWalkNo = nColFirst; nWalkNo <= nColNo; nWalkNo++ )
		{
			INT nItemExtent = OuterColumnWidthGet( false, nWalkNo );
			if( nWalkNo < nColNo )
			{
				if( p_nVisibleColNo != NULL )
					(*p_nVisibleColNo) ++;
				rcCalcItem.OffsetRect( nItemExtent, 0 );
			} // if( nWalkNo < nColNo )
			else
				rcCalcItem.right += nItemExtent;
		} // for( LONG nWalkNo = nColFirst; nWalkNo <= nColNo; nWalkNo++ )
		rcCalcExtra.left = rcCalcItem.left; 
		rcCalcExtra.right = rcCalcItem.right; 
	} // else if( nColType > 0 )
	else
	{ // if inner column type
		OnGbwQueryVisibleColumnRange(
			nColFirst,
			nColLast,
			nColFirstOffset
			);
		ASSERT( nColFirst <= nColLast );
		if( nColNo < nColFirst || nColNo > nColLast )
			return false;
		for( LONG nWalkNo = nColFirst; nWalkNo <= nColNo; nWalkNo++ )
		{
			INT nExtraSpaceBefore = 0, nExtraSpaceAfter = 0;
			INT nItemExtent =
				OnSiwQueryItemExtentH(
					nWalkNo,
					&nExtraSpaceBefore,
					&nExtraSpaceAfter
					);
			ASSERT( nExtraSpaceBefore >= 0 );
			ASSERT( nExtraSpaceAfter >= 0 );
			if( nWalkNo < nColNo )
			{
				if( p_nVisibleColNo != NULL )
					(*p_nVisibleColNo) ++;
				rcCalcItem.OffsetRect( nItemExtent, 0 );
			} // if( nWalkNo < nColNo )
			else
			{
				rcCalcItem.right += nItemExtent;
				rcCalcExtra = rcCalcItem;
				rcCalcItem.DeflateRect(
					nExtraSpaceBefore,
					0,
					nExtraSpaceAfter,
					0
					);
			} // else from if( nWalkNo < nColNo )
		} // for( LONG nWalkNo = nColFirst; nWalkNo <= nColNo; nWalkNo++ )
	} // if inner column type
	OnGbwQueryVisibleRowRange(
		nRowFirst,
		nRowLast,
		nRowFirstOffset
		);
	if( nRowType < 0 )
	{
		if( m_rcOuterT.IsRectEmpty() )
			return false;
		rcCalcItem.OffsetRect( 0, -rcCalcItem.top );
		nRowFirstOffset = m_rcOuterT.top;
		LONG nOuterCount = OuterRowCountTopGet();
		if( nOuterCount == 0L )
			return false;
		nRowFirst = 0L;
		nRowLast = nOuterCount - 1L;
		//ASSERT( nRowFirst <= nRowLast );
		if( nRowNo < nRowFirst || nRowNo > nRowLast )
			return false;
		for( LONG nWalkNo = nRowFirst; nWalkNo <= nRowNo; nWalkNo++ )
		{
			INT nItemExtent = OuterRowHeightGet( true, nWalkNo );
			if( nWalkNo < nRowNo )
			{
				if( p_nVisibleRowNo != NULL )
					(*p_nVisibleRowNo) ++;
				rcCalcItem.OffsetRect( 0, nItemExtent );
			} // if( nWalkNo < nRowNo )
			else
				rcCalcItem.bottom += nItemExtent;
		} // for( LONG nWalkNo = nRowFirst; nWalkNo <= nRowNo; nWalkNo++ )
		rcCalcExtra.top = rcCalcItem.top; 
		rcCalcExtra.bottom = rcCalcItem.bottom; 
	} // if( nRowType < 0 )
	else if( nRowType > 0 )
	{
		if( m_rcOuterB.IsRectEmpty() )
			return false;
		rcCalcItem.OffsetRect( 0, -rcCalcItem.top );
		nRowFirstOffset = m_rcOuterB.top;
		LONG nOuterCount = OuterRowCountBottomGet();
		if( nOuterCount == 0L )
			return false;
		nRowFirst = 0L;
		nRowLast = nOuterCount - 1L;
		//ASSERT( nRowFirst <= nRowLast );
		if( nRowNo < nRowFirst || nRowNo > nRowLast )
			return false;
		for( LONG nWalkNo = nRowFirst; nWalkNo <= nRowNo; nWalkNo++ )
		{
			INT nItemExtent = OuterRowHeightGet( false, nWalkNo );
			if( nWalkNo < nRowNo )
			{
				if( p_nVisibleRowNo != NULL )
					(*p_nVisibleRowNo) ++;
				rcCalcItem.OffsetRect( 0, nItemExtent );
			} // if( nWalkNo < nRowNo )
			else
				rcCalcItem.bottom += nItemExtent;
		} // for( LONG nWalkNo = nRowFirst; nWalkNo <= nRowNo; nWalkNo++ )
		rcCalcExtra.top = rcCalcItem.top; 
		rcCalcExtra.bottom = rcCalcItem.bottom; 
	} // else if( nRowType > 0 )
	else
	{ // if inner column type
		OnGbwQueryVisibleRowRange(
			nRowFirst,
			nRowLast,
			nRowFirstOffset
			);
		ASSERT( nRowFirst <= nRowLast );
		if( nRowNo < nRowFirst || nRowNo > nRowLast )
			return false;
		for( LONG nWalkNo = nRowFirst; nWalkNo <= nRowNo; nWalkNo++ )
		{
			INT nExtraSpaceBefore = 0, nExtraSpaceAfter = 0;
			INT nItemExtent =
				OnSiwQueryItemExtentV(
					nWalkNo,
					&nExtraSpaceBefore,
					&nExtraSpaceAfter
					);
			ASSERT( nExtraSpaceBefore >= 0 );
			ASSERT( nExtraSpaceAfter >= 0 );
			if( nWalkNo < nRowNo )
			{
				if( p_nVisibleRowNo != NULL )
					(*p_nVisibleRowNo) ++;
				rcCalcItem.OffsetRect( 0, nItemExtent );
			} // if( nWalkNo < nRowNo )
			else
			{
				rcCalcItem.bottom += nItemExtent;
				rcCalcExtra = rcCalcItem;
				rcCalcItem.DeflateRect(
					0,
					nExtraSpaceBefore,
					0,
					nExtraSpaceAfter
					);
			} // else from if( nWalkNo < nRowNo )
		} // for( LONG nWalkNo = nRowFirst; nWalkNo <= nRowNo; nWalkNo++ )
	} // if inner column type
	ASSERT( rcCalcItem.left <= rcCalcItem.right );
	ASSERT( rcCalcItem.top <= rcCalcItem.bottom );
	rcCalcItem.OffsetRect( nColFirstOffset, nRowFirstOffset );
	::CopyRect( &rcCell, &rcCalcItem );
	::CopyRect( &rcCellExtra, &rcCalcExtra );
	return true;
}

void CExtGridBaseWnd::OnGbwWalkVisibleAreas(
	CDC & dc,
	bool bFocusedControl,
	CExtGridHitTestInfo * pHT // = NULL
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL || pHT != NULL );

bool bPaintQuery = ( dc.GetSafeHdc() != NULL && pHT == NULL ) ? true : false;
	if( !bPaintQuery )
	{
		pHT->m_dwAreaFlags = __EGBWA_NOWHERE;
		CRect rcClientReal;
		CWnd::GetClientRect( &rcClientReal );
		if( !rcClientReal.PtInRect(pHT->m_ptClient) )
			return;
	} // if( !bPaintQuery )

CRect rcClient = OnSwGetClientRect();

	if( bPaintQuery )
	{
		if( (!m_rcOuterT.IsRectEmpty()) && dc.RectVisible(&m_rcOuterT) )
			OnGbwEraseArea( dc, m_rcOuterT, __EGBWA_OUTER_CELLS|__EGBWA_OUTER_TOP );
		if( (!m_rcOuterB.IsRectEmpty()) && dc.RectVisible(&m_rcOuterB) )
			OnGbwEraseArea( dc, m_rcOuterB, __EGBWA_OUTER_CELLS|__EGBWA_OUTER_BOTTOM );
		if( (!m_rcOuterL.IsRectEmpty()) && dc.RectVisible(&m_rcOuterL) )
			OnGbwEraseArea( dc, m_rcOuterL, __EGBWA_OUTER_CELLS|__EGBWA_OUTER_LEFT );
		if( (!m_rcOuterR.IsRectEmpty()) && dc.RectVisible(&m_rcOuterR) )
			OnGbwEraseArea( dc, m_rcOuterR, __EGBWA_OUTER_CELLS|__EGBWA_OUTER_RIGHT );
		if( (!m_rcOuterLT.IsRectEmpty()) && dc.RectVisible(&m_rcOuterLT) )
			OnGbwEraseArea( dc, m_rcOuterLT, __EGBWA_OUTER_CELLS|__EGBWA_OUTER_LEFT|__EGBWA_OUTER_TOP );
		if( (!m_rcOuterRT.IsRectEmpty()) && dc.RectVisible(&m_rcOuterRT) )
			OnGbwEraseArea( dc, m_rcOuterRT, __EGBWA_OUTER_CELLS|__EGBWA_OUTER_RIGHT|__EGBWA_OUTER_TOP );
		if( (!m_rcOuterLB.IsRectEmpty()) && dc.RectVisible(&m_rcOuterLB) )
			OnGbwEraseArea( dc, m_rcOuterLB, __EGBWA_OUTER_CELLS|__EGBWA_OUTER_LEFT|__EGBWA_OUTER_BOTTOM );
		if( (!m_rcOuterRB.IsRectEmpty()) && dc.RectVisible(&m_rcOuterRB) )
			OnGbwEraseArea( dc, m_rcOuterRB, __EGBWA_OUTER_CELLS|__EGBWA_OUTER_RIGHT|__EGBWA_OUTER_BOTTOM );
		if( (!rcClient.IsRectEmpty()) && dc.RectVisible(&rcClient) )
			OnGbwEraseArea( dc, rcClient, __EGBWA_INNER_CELLS );
	} // if( bPaintQuery )
	else
	{
		ASSERT( pHT != NULL );
		if( (!m_rcOuterLT.IsRectEmpty()) && m_rcOuterLT.PtInRect(pHT->m_ptClient) )
		{
			pHT->m_dwAreaFlags = __EGBWA_OUTER_CELLS|__EGBWA_OUTER_LEFT|__EGBWA_OUTER_TOP;
			return;
		}
		if( (!m_rcOuterRT.IsRectEmpty()) && m_rcOuterRT.PtInRect(pHT->m_ptClient) )
		{
			pHT->m_dwAreaFlags = __EGBWA_OUTER_CELLS|__EGBWA_OUTER_RIGHT|__EGBWA_OUTER_TOP;
			return;
		}
		if( (!m_rcOuterLB.IsRectEmpty()) && m_rcOuterLB.PtInRect(pHT->m_ptClient) )
		{
			pHT->m_dwAreaFlags = __EGBWA_OUTER_CELLS|__EGBWA_OUTER_LEFT|__EGBWA_OUTER_BOTTOM;
			return;
		}
		if( (!m_rcOuterRB.IsRectEmpty()) && m_rcOuterRB.PtInRect(pHT->m_ptClient) )
		{
			pHT->m_dwAreaFlags = __EGBWA_OUTER_CELLS|__EGBWA_OUTER_RIGHT|__EGBWA_OUTER_BOTTOM;
			return;
		}
	} // else from if( bPaintQuery )

CRect rcVisibleRange = OnSiwGetVisibleRange();
	if(		rcVisibleRange.left > rcVisibleRange.right
		&&	rcVisibleRange.top > rcVisibleRange.bottom
		)
		return;

CRgn rgnClientClip;
	if( bPaintQuery && rgnClientClip.CreateRectRgnIndirect(&rcClient) )
		dc.SelectClipRgn( &rgnClientClip );

DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();
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

LONG nOuterColumnCountR = OuterColumnCountRightGet();
	ASSERT( nOuterColumnCountR >= 0L );
LONG nOuterColumnCountL = OuterColumnCountLeftGet();
	ASSERT( nOuterColumnCountL >= 0L );
LONG nOuterRowCountB = OuterRowCountBottomGet();
	ASSERT( nOuterRowCountB >= 0L );
LONG nOuterRowCountT = OuterRowCountTopGet();
	ASSERT( nOuterRowCountT >= 0L );
	if( bPreferHorizontalWalk )
	{
		bool bVirtualRightReached = false;
		CRect rcColExtra(
			rcClient.left,
			rcClient.top,
			rcClient.left,
			rcClient.bottom
			);
		LONG nColNo = rcVisibleRange.left;
		LONG nColLast = rcVisibleRange.right;
		if(		dwScrollTypeH == __ESIW_ST_NONE
			||	dwScrollTypeH == __ESIW_ST_PIXEL
			)
		{
			LONG nColFirstOffset = rcColExtra.left;
			OnGbwQueryVisibleColumnRange(
				nColNo,
				nColLast,
				nColFirstOffset
				);
			rcColExtra.left = rcColExtra.right = nColFirstOffset;
		}
		CRect rcColSaved( rcColExtra );
		LONG nVisibleColNo;
		LONG nColNoSaved = nColNo;
		if(		(! rcClient.IsRectEmpty() )
			&&	( (!bPaintQuery) || dc.RectVisible(&rcClient) )
			&&	( pHT == NULL || rcClient.PtInRect(pHT->m_ptClient) )
			)
		{
			for( nVisibleColNo = 0; nColNo <= nColLast; nColNo++, nVisibleColNo++ )
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
				if(	OnSiwWalkItemsV(
						dc,
						(LPVOID)pHT,
						rcColExtra,
						rcCol,
						nVisibleColNo,
						nColNo,
						rcVisibleRange,
						bVirtualRightReached,
						__EGBWA_INNER_CELLS,
						bFocusedControl
						)
					)
				{
					ASSERT( dc.GetSafeHdc() == NULL );
					ASSERT( pHT != NULL );
					ASSERT( pHT->m_dwAreaFlags != __EGBWA_NOWHERE );
					return;
				}
				if( bVirtualRightReached )
					break;
				rcColExtra.left = rcColExtra.right;
			} // for( nVisibleColNo = 0; nColNo <= nColLast; nColNo++, nVisibleColNo++ )
			if( pHT != NULL )
			{
				pHT->m_dwAreaFlags = __EGBWA_INNER_CELLS;
				return; // no hit test at any inner cell
			}
		} // if( (!rcClient.IsRectEmpty()) ...

		if( bPaintQuery )
			dc.SelectClipRgn( NULL );
		if(		nOuterRowCountB > 0L
			&&	m_rcOuterB.left < m_rcOuterB.right
			&&	m_rcOuterB.top < m_rcOuterB.bottom
			&&	( (!bPaintQuery) || dc.RectVisible( &m_rcOuterB ) )
			&&	( pHT == NULL || m_rcOuterB.PtInRect(pHT->m_ptClient) )
			)
		{ // if need to paint fixed rows at bottom
			rcColExtra.SetRect(
				rcColSaved.left,
				m_rcOuterB.top,
				rcColSaved.right,
				m_rcOuterB.bottom
				);
			CRgn rgnOuterAreaClip;
			if( bPaintQuery && rgnOuterAreaClip.CreateRectRgnIndirect(&m_rcOuterB) )
				dc.SelectClipRgn( &rgnOuterAreaClip );
			for( nColNo = nColNoSaved, nVisibleColNo = 0; nColNo <= nColLast; nColNo++, nVisibleColNo++ )
			{
				bool bVirtualBottomReached = false, bVirtualRightReached = false;
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
				rcColExtra.right += nColWidth;
				if(		(!rcColExtra.IsRectEmpty())
					&&	( (!bPaintQuery) || dc.RectVisible( &rcColExtra ) )
					&&	( pHT == NULL || rcColExtra.PtInRect(pHT->m_ptClient) )
					)
				{
					LONG nRowNo = 0;
					CRect rcCellExtra( rcColExtra );
					rcCellExtra.bottom = rcCellExtra.top;
					for( ; nRowNo < nOuterRowCountB; nRowNo++ )
					{
						int nCellHeight = OuterRowHeightGet( false, nRowNo );
						rcCellExtra.bottom += nCellHeight;
						if(		(!rcCellExtra.IsRectEmpty())
							&&	( (!bPaintQuery) || dc.RectVisible( &rcCellExtra ) )
							&&	( pHT == NULL || rcCellExtra.PtInRect(pHT->m_ptClient) )
							)
						{
							// extra-space adjustment
							CRect rcCell(
								rcCellExtra.left + nExtraSpaceBefore,
								rcCellExtra.top,
								rcCellExtra.right - nExtraSpaceAfter,
								rcCellExtra.bottom
								);
							if(	OnSiwWalkCell(
									dc,
									(LPVOID)pHT,
									nVisibleColNo,
									nRowNo,
									nColNo,
									- nRowNo - 1, // indicate fixed type
									rcCellExtra,
									rcCell,
									rcVisibleRange,
									bVirtualRightReached,
									bVirtualBottomReached,
									__EGBWA_OUTER_CELLS|__EGBWA_OUTER_BOTTOM,
									bFocusedControl
									)
								)
							{
								ASSERT( pHT != NULL );
								ASSERT( pHT->m_dwAreaFlags != __EGBWA_NOWHERE );
								return;
							}
							if( pHT != NULL )
							{
								pHT->m_dwAreaFlags = __EGBWA_OUTER_CELLS|__EGBWA_OUTER_BOTTOM;
								return;
							}
							if( bVirtualBottomReached || bVirtualRightReached )
								break;
						} // if( (!rcCell.IsRectEmpty()) ...
						if( bVirtualRightReached )
							break;
						rcCellExtra.top = rcCellExtra.bottom;
					} // for( ; nRowNo < nOuterRowCountB; nRowNo++ )
					if( pHT != NULL )
					{
						pHT->m_dwAreaFlags = __EGBWA_OUTER_CELLS|__EGBWA_OUTER_BOTTOM;
						return;
					}
				} // if( (!rcCol.IsRectEmpty()) ...
				rcColExtra.left = rcColExtra.right;
			} // for( nColNo = nColNoSaved, nVisibleColNo = 0; nColNo <= nColLast; nColNo++, nVisibleColNo++ )
			if( pHT != NULL )
			{
				pHT->m_dwAreaFlags = __EGBWA_OUTER_CELLS|__EGBWA_OUTER_BOTTOM;
				return; // no hit test at any cell in this area
			}
		} // if need to paint fixed rows at bottom

		if( bPaintQuery )
			dc.SelectClipRgn( NULL );
		if(		nOuterRowCountT > 0L
			&&	m_rcOuterT.left < m_rcOuterT.right
			&&	m_rcOuterT.top < m_rcOuterT.bottom
			&&	( (!bPaintQuery) || dc.RectVisible( &m_rcOuterT ) )
			&&	( pHT == NULL || m_rcOuterT.PtInRect(pHT->m_ptClient) )
			)
		{ // if need to paint fixed rows at top
			rcColExtra.SetRect(
				rcColSaved.left,
				m_rcOuterT.top,
				rcColSaved.right,
				m_rcOuterT.bottom
				);
			CRgn rgnOuterAreaClip;
			if( bPaintQuery && rgnOuterAreaClip.CreateRectRgnIndirect(&m_rcOuterT) )
				dc.SelectClipRgn( &rgnOuterAreaClip );
			for( nColNo = nColNoSaved, nVisibleColNo = 0; nColNo <= nColLast; nColNo++, nVisibleColNo++ )
			{
				bool bVirtualBottomReached = false, bVirtualRightReached = false;
				INT nExtraSpaceBefore = 0, nExtraSpaceAfter = 0;
				INT nColWidth =
					OnSiwQueryItemExtentH(
						nColNo,
						&nExtraSpaceBefore,
						&nExtraSpaceAfter
						);
				ASSERT( nColWidth >= 0 );
				rcColExtra.right += nColWidth;
				if(		(!rcColExtra.IsRectEmpty())
					&&	( (!bPaintQuery) || dc.RectVisible( &rcColExtra ) )
					&&	( pHT == NULL || rcColExtra.PtInRect(pHT->m_ptClient) )
					)
				{
					LONG nRowNo = 0;
					CRect rcCellExtra( rcColExtra );
					rcCellExtra.bottom = rcCellExtra.top;
					for( ; nRowNo < nOuterRowCountT; nRowNo++ )
					{
						int nCellHeight = OuterRowHeightGet( true, nRowNo );
						rcCellExtra.bottom += nCellHeight;
						if(		(!rcCellExtra.IsRectEmpty())
							&&	( (!bPaintQuery) || dc.RectVisible( &rcCellExtra ) )
							&&	( pHT == NULL || rcCellExtra.PtInRect(pHT->m_ptClient) )
							)
						{
							// extra-space adjustment
							CRect rcCell(
								rcCellExtra.left + nExtraSpaceBefore,
								rcCellExtra.top,
								rcCellExtra.right - nExtraSpaceAfter,
								rcCellExtra.bottom
								);
							if(	OnSiwWalkCell(
									dc,
									(LPVOID)pHT,
									nVisibleColNo,
									nRowNo,
									nColNo,
									- nRowNo - 1, // indicate fixed type
									rcCellExtra,
									rcCell,
									rcVisibleRange,
									bVirtualRightReached,
									bVirtualBottomReached,
									__EGBWA_OUTER_CELLS|__EGBWA_OUTER_TOP,
									bFocusedControl
									)
								)
							{
								ASSERT( pHT != NULL );
								ASSERT( pHT->m_dwAreaFlags != __EGBWA_NOWHERE );
								return;
							}
							if( pHT != NULL )
							{
								pHT->m_dwAreaFlags = __EGBWA_OUTER_CELLS|__EGBWA_OUTER_TOP;
								return;
							}
							if( bVirtualBottomReached || bVirtualRightReached )
								break;
						} // if( (!rcCellExtra.IsRectEmpty()) ...
						if( bVirtualRightReached )
							break;
						rcCellExtra.top = rcCellExtra.bottom;
					} // for( ; nRowNo < nOuterRowCountT; nRowNo++ )
					if( pHT != NULL )
					{
						pHT->m_dwAreaFlags = __EGBWA_OUTER_CELLS|__EGBWA_OUTER_TOP;
						return;
					}
				} // if( (!rcCol.IsRectEmpty()) ...
				rcColExtra.left = rcColExtra.right;
			} // for( nColNo = nColNoSaved, nVisibleColNo = 0; nColNo <= nColLast; nColNo++, nVisibleColNo++ )
			if( pHT != NULL )
			{
				pHT->m_dwAreaFlags = __EGBWA_OUTER_CELLS|__EGBWA_OUTER_TOP;
				return; // no hit test at any cell in this area
			}
		} // if need to paint fixed rows at top
		
		if( bPaintQuery )
			dc.SelectClipRgn( NULL );
		if(		nOuterColumnCountR > 0L
			&&	m_rcOuterR.left < m_rcOuterR.right
			&&	m_rcOuterR.top < m_rcOuterR.bottom
			&&	( (!bPaintQuery) || dc.RectVisible( &m_rcOuterR ) )
			&&	( pHT == NULL || m_rcOuterR.PtInRect(pHT->m_ptClient) )
			)
		{ // if need to paint fixed columns at right
			bVirtualRightReached = false;
			rcColExtra.SetRect(
				m_rcOuterR.left,
				rcColSaved.top,
				m_rcOuterR.left,
				rcColSaved.bottom
				);
			CRgn rgnOuterAreaClip;
			if( bPaintQuery && rgnOuterAreaClip.CreateRectRgnIndirect(&m_rcOuterR) )
				dc.SelectClipRgn( &rgnOuterAreaClip );
			for( nColNo = 0, nVisibleColNo = 0; nColNo < nOuterColumnCountR; nColNo++, nVisibleColNo++ )
			{
				int nColWidth = OuterColumnWidthGet( false, nColNo );
				ASSERT( nColWidth >= 0 );
				if( nColWidth == 0 )
					continue;
				rcColExtra.right += nColWidth;
				// extra-space adjustment
				CRect rcCol( rcColExtra );
				if(	OnSiwWalkItemsV(
						dc,
						(LPVOID)pHT,
						rcColExtra,
						rcCol,
						nVisibleColNo,
						- nColNo - 1, // indicate fixed row type
						rcVisibleRange,
						bVirtualRightReached,
						__EGBWA_OUTER_CELLS|__EGBWA_OUTER_RIGHT,
						bFocusedControl
						)
					)
				{
					ASSERT( dc.GetSafeHdc() == NULL );
					ASSERT( pHT != NULL );
					ASSERT( pHT->m_dwAreaFlags != __EGBWA_NOWHERE );
					return;
				}
				if( bVirtualRightReached )
					break;
				rcColExtra.left = rcColExtra.right;
			} // for( nColNo = 0, nVisibleColNo = 0; nColNo < nOuterColumnCountR; nColNo++, nVisibleColNo++ )
			if( pHT != NULL )
			{
				pHT->m_dwAreaFlags = __EGBWA_OUTER_CELLS|__EGBWA_OUTER_RIGHT;
				return; // no hit test at any cell in this area
			}
		} // if need to paint fixed columns at right

		if( bPaintQuery )
			dc.SelectClipRgn( NULL );
		if(		nOuterColumnCountL > 0L
			&&	m_rcOuterL.left < m_rcOuterL.right
			&&	m_rcOuterL.top < m_rcOuterL.bottom
			&&	( (!bPaintQuery) || dc.RectVisible( &m_rcOuterL ) )
			&&	( pHT == NULL || m_rcOuterL.PtInRect(pHT->m_ptClient) )
			)
		{ // if need to paint fixed columns at left
			bVirtualRightReached = false;
			rcColExtra.SetRect(
				m_rcOuterL.left,
				rcColSaved.top,
				m_rcOuterL.left,
				rcColSaved.bottom
				);
			CRgn rgnOuterAreaClip;
			if( bPaintQuery && rgnOuterAreaClip.CreateRectRgnIndirect(&m_rcOuterL) )
				dc.SelectClipRgn( &rgnOuterAreaClip );
			for( nColNo = 0, nVisibleColNo = 0; nColNo < nOuterColumnCountL; nColNo++, nVisibleColNo++ )
			{
				int nColWidth = OuterColumnWidthGet( true, nColNo );
				ASSERT( nColWidth >= 0 );
				if( nColWidth == 0 )
					continue;
				rcColExtra.right += nColWidth;
				// extra-space adjustment
				CRect rcCol( rcColExtra );
				if(	OnSiwWalkItemsV(
						dc,
						(LPVOID)pHT,
						rcColExtra,
						rcCol,
						nVisibleColNo,
						- nColNo - 1, // indicate fixed row type
						rcVisibleRange,
						bVirtualRightReached,
						__EGBWA_OUTER_CELLS|__EGBWA_OUTER_LEFT,
						bFocusedControl
						)
					)
				{
					ASSERT( dc.GetSafeHdc() == NULL );
					ASSERT( pHT != NULL );
					ASSERT( pHT->m_dwAreaFlags != __EGBWA_NOWHERE );
					return;
				}
				if( bVirtualRightReached )
					break;
				rcColExtra.left = rcColExtra.right;
			}
			if( pHT != NULL )
			{
				pHT->m_dwAreaFlags = __EGBWA_OUTER_CELLS|__EGBWA_OUTER_LEFT;
				return; // no hit test at any cell in this area
			}
		} // if need to paint fixed columns at left
	} // if( bPreferHorizontalWalk )
	else
	{
		bool bVirtualBottomReached = false;
		CRect rcRowExtra(
			rcClient.left,
			rcClient.top,
			rcClient.right,
			rcClient.top
			);
		LONG nRowNo = rcVisibleRange.top;
		LONG nRowLast = rcVisibleRange.bottom;
		if(		dwScrollTypeV == __ESIW_ST_NONE
			||	dwScrollTypeV == __ESIW_ST_PIXEL
			)
		{
			LONG nRowFirstOffset = rcRowExtra.top;
			OnGbwQueryVisibleRowRange(
				nRowNo,
				nRowLast,
				nRowFirstOffset
				);
			rcRowExtra.top = rcRowExtra.bottom = nRowFirstOffset;
		}
		CRect rcRowSaved( rcRowExtra );
		LONG nRowNoSaved = nRowNo;
		LONG nVisibleRowNo;
		if(		(! rcClient.IsRectEmpty() )
			&&	( (!bPaintQuery) || dc.RectVisible(&rcClient) )
			&&	( pHT == NULL || rcClient.PtInRect(pHT->m_ptClient) )
			)
		{
			for( nVisibleRowNo = 0; nRowNo <= nRowLast; nRowNo++, nVisibleRowNo++ )
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
				if(	OnSiwWalkItemsH(
						dc,
						(LPVOID)pHT,
						rcRowExtra,
						rcRow,
						nVisibleRowNo,
						nRowNo,
						rcVisibleRange,
						bVirtualBottomReached,
						__EGBWA_INNER_CELLS,
						bFocusedControl
						)
					)
				{
					ASSERT( dc.GetSafeHdc() == NULL );
					ASSERT( pHT != NULL );
					ASSERT( pHT->m_dwAreaFlags != __EGBWA_NOWHERE );
					return;
				}
				if( bVirtualBottomReached )
					break;
				rcRowExtra.top = rcRowExtra.bottom;
			} // for( nVisibleRowNo = 0; nRowNo <= nRowLast; nRowNo++, nVisibleRowNo++ )
			if( pHT != NULL )
			{
				pHT->m_dwAreaFlags = __EGBWA_INNER_CELLS;
				return; // no hit test at any inner cell
			}
		} // if( (!rcClient.IsRectEmpty()) ...

		if( bPaintQuery )
			dc.SelectClipRgn( NULL );
		if(		nOuterColumnCountR > 0L
			&&	m_rcOuterR.left < m_rcOuterR.right
			&&	m_rcOuterR.top < m_rcOuterR.bottom
			&&	( (!bPaintQuery) || dc.RectVisible( &m_rcOuterR ) )
			&&	( pHT == NULL || m_rcOuterR.PtInRect(pHT->m_ptClient) )
			)
		{ // if need to paint fixed columns at right
			rcRowExtra.SetRect(
				m_rcOuterR.left,
				rcRowSaved.top,
				m_rcOuterR.right,
				rcRowSaved.bottom
				);
			CRgn rgnOuterAreaClip;
			if( bPaintQuery && rgnOuterAreaClip.CreateRectRgnIndirect(&m_rcOuterR) )
				dc.SelectClipRgn( &rgnOuterAreaClip );
			for( nRowNo = nRowNoSaved, nVisibleRowNo = 0; nRowNo <= nRowLast; nRowNo++, nVisibleRowNo++ )
			{
				bool bVirtualBottomReached = false, bVirtualRightReached = false;
				INT nExtraSpaceBefore = 0, nExtraSpaceAfter = 0;
				INT nRowHeight =
					OnSiwQueryItemExtentV(
						nRowNo,
						&nExtraSpaceBefore,
						&nExtraSpaceAfter
						);
				ASSERT( nRowHeight >= 0 );
				rcRowExtra.bottom += nRowHeight;
				if(		(!rcRowExtra.IsRectEmpty())
					&&	( (!bPaintQuery) || dc.RectVisible( &rcRowExtra ) )
					&&	( pHT == NULL || rcRowExtra.PtInRect(pHT->m_ptClient) )
					)
				{
					LONG nColNo = 0;
					CRect rcCellExtra( rcRowExtra );
					rcCellExtra.right = rcCellExtra.left;
					for( ; nColNo < nOuterColumnCountR; nColNo++ )
					{
						int nCellWidth = OuterColumnWidthGet( false, nColNo );
						rcCellExtra.right += nCellWidth;
						if(		(!rcCellExtra.IsRectEmpty())
							&&	( (!bPaintQuery) || dc.RectVisible( &rcCellExtra ) )
							&&	( pHT == NULL || rcCellExtra.PtInRect(pHT->m_ptClient) )
							)
						{
							// extra-space adjustment
							CRect rcCell(
								rcCellExtra.left,
								rcCellExtra.top + nExtraSpaceBefore,
								rcCellExtra.right,
								rcCellExtra.bottom - nExtraSpaceAfter
								);
							if(	OnSiwWalkCell(
									dc,
									(LPVOID)pHT,
									nColNo,
									nVisibleRowNo,
									- nColNo - 1, // indicate fixed row type
									nRowNo,
									rcCellExtra,
									rcCell,
									rcVisibleRange,
									bVirtualRightReached,
									bVirtualBottomReached,
									__EGBWA_OUTER_CELLS|__EGBWA_OUTER_RIGHT,
									bFocusedControl
									)
								)
							{
								ASSERT( dc.GetSafeHdc() == NULL );
								ASSERT( pHT != NULL );
								ASSERT( pHT->m_dwAreaFlags != __EGBWA_NOWHERE );
								return;
							}
							if( pHT != NULL )
							{
								pHT->m_dwAreaFlags = __EGBWA_OUTER_CELLS|__EGBWA_OUTER_RIGHT;
								return;
							}
							if( bVirtualBottomReached || bVirtualRightReached )
								break;
						} // if( (!rcCell.IsRectEmpty()) ...
						if( bVirtualBottomReached )
							break;
						rcCellExtra.left = rcCellExtra.right;
					} // for( ; nColNo < nOuterColumnCountR; nColNo++ )
				} // if( (!rcRowExtra.IsRectEmpty()) ...
				rcRowExtra.top = rcRowExtra.bottom;
			} // for( nRowNo = nRowNoSaved, nVisibleRowNo = 0; nRowNo <= nRowLast; nRowNo++, nVisibleRowNo++ )
			if( pHT != NULL )
			{
				pHT->m_dwAreaFlags = __EGBWA_OUTER_CELLS|__EGBWA_OUTER_RIGHT;
				return; // no hit test at any cell in this area
			}
		} // if need to paint fixed columns at right

		if( bPaintQuery )
			dc.SelectClipRgn( NULL );
		if(		nOuterColumnCountL > 0L
			&&	m_rcOuterL.left < m_rcOuterL.right
			&&	m_rcOuterL.top < m_rcOuterL.bottom
			&&	( (!bPaintQuery) || dc.RectVisible( &m_rcOuterL ) )
			&&	( pHT == NULL || m_rcOuterL.PtInRect(pHT->m_ptClient) )
			)
		{ // if need to paint fixed columns at left
			rcRowExtra.SetRect(
				m_rcOuterL.left,
				rcRowSaved.top,
				m_rcOuterL.right,
				rcRowSaved.bottom
				);
			CRgn rgnOuterAreaClip;
			if( bPaintQuery && rgnOuterAreaClip.CreateRectRgnIndirect(&m_rcOuterL) )
				dc.SelectClipRgn( &rgnOuterAreaClip );
			for( nRowNo = nRowNoSaved, nVisibleRowNo = 0; nRowNo <= nRowLast; nRowNo++, nVisibleRowNo++ )
			{
				bool bVirtualBottomReached = false, bVirtualRightReached = false;
				INT nExtraSpaceBefore = 0, nExtraSpaceAfter = 0;
				INT nRowHeight =
					OnSiwQueryItemExtentV(
						nRowNo,
						&nExtraSpaceBefore,
						&nExtraSpaceAfter
						);
				ASSERT( nRowHeight >= 0 );
				rcRowExtra.bottom += nRowHeight;
				if(		(!rcRowExtra.IsRectEmpty())
					&&	( (!bPaintQuery) || dc.RectVisible( &rcRowExtra ) )
					&&	( pHT == NULL || rcRowExtra.PtInRect(pHT->m_ptClient) )
					)
				{
					LONG nColNo = 0;
					CRect rcCellExtra( rcRowExtra );
					rcCellExtra.right = rcCellExtra.left;
					for( ; nColNo < nOuterColumnCountL; nColNo++ )
					{
						int nCellWidth = OuterColumnWidthGet( true, nColNo );
						rcCellExtra.right += nCellWidth;
						if(		(!rcCellExtra.IsRectEmpty())
							&&	( (!bPaintQuery) || dc.RectVisible( &rcCellExtra ) )
							&&	( pHT == NULL || rcCellExtra.PtInRect(pHT->m_ptClient) )
							)
						{
							// extra-space adjustment
							CRect rcCell(
								rcCellExtra.left,
								rcCellExtra.top + nExtraSpaceBefore,
								rcCellExtra.right,
								rcCellExtra.bottom - nExtraSpaceAfter
								);
							if(	OnSiwWalkCell(
									dc,
									(LPVOID)pHT,
									nColNo,
									nVisibleRowNo,
									- nColNo - 1, // indicate fixed row type
									nRowNo,
									rcCellExtra,
									rcCell,
									rcVisibleRange,
									bVirtualRightReached,
									bVirtualBottomReached,
									__EGBWA_OUTER_CELLS|__EGBWA_OUTER_LEFT,
									bFocusedControl
									)
								)
							{
								ASSERT( dc.GetSafeHdc() == NULL );
								ASSERT( pHT != NULL );
								ASSERT( pHT->m_dwAreaFlags != __EGBWA_NOWHERE );
								return;
							}
							if( pHT != NULL )
							{
								pHT->m_dwAreaFlags = __EGBWA_OUTER_CELLS|__EGBWA_OUTER_LEFT;
								return;
							}
							if( bVirtualBottomReached || bVirtualRightReached )
								break;
						} // if( (!rcCellExtra.IsRectEmpty()) ...
						if( bVirtualBottomReached )
							break;
						rcCellExtra.left = rcCellExtra.right;
					} // for( ; nColNo < nOuterColumnCountL; nColNo++ )
				} // if( (!rcRow.IsRectEmpty()) ...
				rcRowExtra.top = rcRowExtra.bottom;
			} // for( nRowNo = nRowNoSaved, nVisibleRowNo = 0; nRowNo <= nRowLast; nRowNo++, nVisibleRowNo++ )
			if( pHT != NULL )
			{
				pHT->m_dwAreaFlags = __EGBWA_OUTER_CELLS|__EGBWA_OUTER_LEFT;
				return; // no hit test at any cell in this area
			}
		} // if need to paint fixed columns at left

		if( bPaintQuery )
			dc.SelectClipRgn( NULL );
		if(		nOuterRowCountB > 0L
			&&	m_rcOuterB.left < m_rcOuterB.right
			&&	m_rcOuterB.top < m_rcOuterB.bottom
			&&	( (!bPaintQuery) || dc.RectVisible( &m_rcOuterB ) )
			&&	( pHT == NULL || m_rcOuterB.PtInRect(pHT->m_ptClient) )
			)
		{ // if need to paint fixed rows at bottom
			bVirtualBottomReached = false;
			rcRowExtra.SetRect(
				rcRowSaved.left,
				m_rcOuterB.top,
				rcRowSaved.right,
				m_rcOuterB.top
				);
			CRgn rgnOuterAreaClip;
			if( bPaintQuery && rgnOuterAreaClip.CreateRectRgnIndirect(&m_rcOuterB) )
				dc.SelectClipRgn( &rgnOuterAreaClip );
			for( nRowNo = 0, nVisibleRowNo = 0; nRowNo < nOuterRowCountB; nRowNo++, nVisibleRowNo++ )
			{
				int nRowHeight = OuterRowHeightGet( false, nRowNo );
				ASSERT( nRowHeight >= 0 );
				if( nRowHeight == 0 )
					continue;
				rcRowExtra.bottom += nRowHeight;
				// extra-space adjustment
				CRect rcRow( rcRowExtra );
				if(	OnSiwWalkItemsH(
						dc,
						(LPVOID)pHT,
						rcRowExtra,
						rcRow,
						nVisibleRowNo,
						- nRowNo - 1, // indicate fixed row type
						rcVisibleRange,
						bVirtualBottomReached,
						__EGBWA_OUTER_CELLS|__EGBWA_OUTER_BOTTOM,
						bFocusedControl
						)
					)
				{
					ASSERT( dc.GetSafeHdc() == NULL );
					ASSERT( pHT != NULL );
					ASSERT( pHT->m_dwAreaFlags != __EGBWA_NOWHERE );
					return;
				}
				if( bVirtualBottomReached )
					break;
				rcRowExtra.top = rcRowExtra.bottom;
			}
			if( pHT != NULL )
			{
				pHT->m_dwAreaFlags = __EGBWA_OUTER_CELLS|__EGBWA_OUTER_BOTTOM;
				return; // no hit test at any cell in this area
			}
		} // if need to paint fixed rows at bottom

		if( bPaintQuery )
			dc.SelectClipRgn( NULL );
		if(		nOuterRowCountT > 0L
			&&	m_rcOuterT.left < m_rcOuterT.right
			&&	m_rcOuterT.top < m_rcOuterT.bottom
			&&	( (!bPaintQuery) || dc.RectVisible( &m_rcOuterT ) )
			&&	( pHT == NULL || m_rcOuterT.PtInRect(pHT->m_ptClient) )
			)
		{ // if need to paint fixed rows at top
			bVirtualBottomReached = false;
			rcRowExtra.SetRect(
				rcRowSaved.left,
				m_rcOuterT.top,
				rcRowSaved.right,
				m_rcOuterT.top
				);
			CRgn rgnOuterAreaClip;
			if( bPaintQuery && rgnOuterAreaClip.CreateRectRgnIndirect(&m_rcOuterT) )
				dc.SelectClipRgn( &rgnOuterAreaClip );
			for( nRowNo = 0, nVisibleRowNo = 0; nRowNo < nOuterRowCountT; nRowNo++, nVisibleRowNo++ )
			{
				int nRowHeight = OuterRowHeightGet( true, nRowNo );
				ASSERT( nRowHeight >= 0 );
				if( nRowHeight == 0 )
					continue;
				rcRowExtra.bottom += nRowHeight;
				// extra-space adjustment
				CRect rcRow( rcRowExtra );
				if(	OnSiwWalkItemsH(
						dc,
						(LPVOID)pHT,
						rcRowExtra,
						rcRow,
						nVisibleRowNo,
						- nRowNo - 1, // indicate fixed row type
						rcVisibleRange,
						bVirtualBottomReached,
						__EGBWA_OUTER_CELLS|__EGBWA_OUTER_TOP,
						bFocusedControl
						)	
					)
				{
					ASSERT( dc.GetSafeHdc() == NULL );
					ASSERT( pHT != NULL );
					ASSERT( pHT->m_dwAreaFlags != __EGBWA_NOWHERE );
					return;
				}
				if( bVirtualBottomReached )
					break;
				rcRowExtra.top = rcRowExtra.bottom;
			}
			if( pHT != NULL )
			{
				pHT->m_dwAreaFlags = __EGBWA_OUTER_CELLS|__EGBWA_OUTER_TOP;
				return; // no hit test at any cell in this area
			}
		} // if need to paint fixed rows at top
	} // else from if( bPreferHorizontalWalk )

	if( bPaintQuery )
		dc.SelectClipRgn( NULL );
}

void CExtGridBaseWnd::OnGbwPaintCell(
	CDC & dc,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	const RECT & rcVisibleRange,
	DWORD dwAreaFlags,
	DWORD dwHelperPaintFlags
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
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
	nVisibleColNo;
	nVisibleRowNo;
	rcVisibleRange;
	rcCellExtra;

bool bHoverByColumn = ( (dwHelperPaintFlags&__EGCPF_HOVER_BY_COLUMN) != 0 ) ? true : false;
bool bHoverByRow = ( (dwHelperPaintFlags&__EGCPF_HOVER_BY_ROW) != 0 ) ? true : false;
bool bHighlightedBySelectedColumn = ( (dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_SELECTED_COLUMN) != 0 ) ? true : false;
bool bHighlightedBySelectedRow = ( (dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_SELECTED_ROW) != 0 ) ? true : false;
bool bHighlightedByFocusedColumn = ( (dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_FOCUSED_COLUMN) != 0 ) ? true : false;
bool bHighlightedByFocusedRow = ( (dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_FOCUSED_ROW) != 0 ) ? true : false;
bool bFocusedControl = ( (dwHelperPaintFlags&__EGCPF_FOCUSED_CONTROL) != 0 ) ? true : false;
bool bHighlightPressing = ( (dwHelperPaintFlags&(__EGCPF_HIGHLIGHTED_BY_PRESSED_COLUMN|__EGCPF_HIGHLIGHTED_BY_PRESSED_ROW)) != 0 ) ? true : false;
	if( ( dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0 )
	{

//		if( bHoverByColumn && bHoverByRow )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 196, 255, 196 )
//				);
//		else if( bHoverByColumn )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 196, 196, 255 )
//				);
//		else if( bHoverByRow )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 255, 196, 196 )
//				);

//		if( bHighlightPressing )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 196, 255, 196 )
//				);
//		else if( bHighlightedBySelectedColumn && bHighlightedBySelectedRow )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 255, 196, 255 )
//				);
//		else if( bHighlightedBySelectedColumn )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 255, 255, 196 )
//				);
//		else if( bHighlightedBySelectedRow )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 196, 255, 255 )
//				);

//		if( bHighlightedByFocusedColumn && bHighlightedByFocusedRow )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 164, 128, 164 )
//				);
//		else if( bHighlightedByFocusedColumn )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 164, 164, 128 )
//				);
//		else if( bHighlightedByFocusedRow )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 128, 164, 164 )
//				);

		if( bHighlightPressing )
		{
			CRect rcHighlightPressing( rcCell );
			if( (dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_PRESSED_COLUMN) != 0 )
				rcHighlightPressing.DeflateRect( 0, 0, 1, 0 );
			if( (dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_PRESSED_ROW) != 0 )
				rcHighlightPressing.DeflateRect( 0, 0, 0, 1 );
			dc.FillSolidRect(
				&rcHighlightPressing, // &rcCell
				OnSiwGetSysColor( COLOR_3DDKSHADOW )
				);
		} // if( bHighlightPressing )
		else if( bHighlightedByFocusedColumn || bHighlightedByFocusedRow )
			dc.FillRect(
				&rcCell,
				&( OnSiwGetLighterOrDarkerBrush( -3 ) )
				);
		else if( bHighlightedBySelectedColumn || bHighlightedBySelectedRow )
			dc.FillRect(
				&rcCell,
				&( OnSiwGetLighterOrDarkerBrush( -2 ) )
				);
		else if( bHoverByColumn || bHoverByRow )
			dc.FillRect(
				&rcCell,
				&( OnSiwGetLighterOrDarkerBrush( -1 ) )
				);
		else
		{
			COLORREF clrLT = OnSiwGetSysColor( COLOR_3DHIGHLIGHT );
			COLORREF clrRB = OnSiwGetSysColor( COLOR_3DSHADOW );
			dc.Draw3dRect( &rcCell, clrLT, clrRB );
		}
	} // if( ( dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0 )
	else
	{
		ASSERT( nRowNo >= 0 && nColNo >= 0 );

//		if( bHoverByColumn && bHoverByRow )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 128, 255, 128 )
//				);
//		else if( bHoverByColumn )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 128, 128, 255 )
//				);
//		else if( bHoverByRow )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 255, 128, 128 )
//				);

//		if( bHighlightPressing )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 128, 255, 128 )
//				);
//		else if( bHighlightedBySelectedColumn && bHighlightedBySelectedRow )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 255, 128, 255 )
//				);
//		else if( bHighlightedBySelectedColumn )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 255, 255, 128 )
//				);
//		else if( bHighlightedBySelectedRow )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 128, 255, 255 )
//				);

//		if( bHighlightedByFocusedColumn && bHighlightedByFocusedRow )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 128, 96, 128 )
//				);
//		else if( bHighlightedByFocusedColumn )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 128, 128, 96 )
//				);
//		else if( bHighlightedByFocusedRow )
//			dc.FillSolidRect(
//				&rcCell,
//				RGB( 96, 128, 128 )
//				);

		if( bHighlightPressing )
		{
			CRect rcHighlightPressing( rcCell );
			if( (dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_PRESSED_COLUMN) != 0 )
				rcHighlightPressing.DeflateRect( 0, 0, 1, 0 );
			if( (dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_PRESSED_ROW) != 0 )
				rcHighlightPressing.DeflateRect( 0, 0, 0, 1 );
			dc.FillSolidRect(
				&rcHighlightPressing, // &rcCell
				OnSiwGetSysColor( COLOR_3DDKSHADOW )
				);
		} // if( bHighlightPressing )
		else if( bHighlightedByFocusedColumn || bHighlightedByFocusedRow )
			dc.FillRect(
				&rcCell,
				&( OnSiwGetLighterOrDarkerBrush( 3 ) )
				);
		else if( bHighlightedBySelectedColumn || bHighlightedBySelectedRow )
			dc.FillRect(
				&rcCell,
				&( OnSiwGetLighterOrDarkerBrush( 2 ) )
				);
		else if( bHoverByColumn || bHoverByRow )
			dc.FillRect(
				&rcCell,
				&( OnSiwGetLighterOrDarkerBrush( 1 ) )
				);
		
		bool bGridLinesHorz = false, bGridLinesVert = false;
		if( ! bHighlightPressing )
		{
			bGridLinesHorz = GridLinesHorzGet();
			bGridLinesVert = GridLinesVertGet();
		} // if( ! bHighlightPressing )

		if( bFocusedControl || NoHideSelectionGet() )
		{
			if( (!bHighlightPressing) && SelectionGetForCell( nColNo, nRowNo ) )
			{
				COLORREF clrHighLight =
					OnSiwGetSysColor(
						bFocusedControl
							? COLOR_HIGHLIGHT
							: COLOR_3DFACE
						);
				dc.FillSolidRect(
					&rcCell,
					clrHighLight
					);
			} // if( (!bHighlightPressing) && SelectionGetForCell( nColNo, nRowNo ) )
			CPoint ptFocus = FocusGet();
			DWORD dwBasicSelectionType = SiwGetStyle() & __EGBS_SFB_MASK;
			if(		dwBasicSelectionType != __EGBS_SFB_NONE
				&&	( ptFocus.x == nColNo || ptFocus.y == nRowNo )
				)
			{
				ASSERT(
						dwBasicSelectionType == __EGBS_SFB_CELLS
					||	dwBasicSelectionType == __EGBS_SFB_FULL_COLUMNS
					||	dwBasicSelectionType == __EGBS_SFB_FULL_ROWS
					);
				DWORD dwScrollTypeH = SiwScrollTypeHGet();
				DWORD dwScrollTypeV = SiwScrollTypeVGet();
				LONG nColumnCount = ColumnCountGet();
				LONG nRowCount = RowCountGet();
				if( bFocusedControl )
				{
					if(		(dwBasicSelectionType == __EGBS_SFB_CELLS
//							||	(	dwScrollTypeH == __ESIW_ST_VIRTUAL
//								&&	dwScrollTypeV == __ESIW_ST_VIRTUAL
//								)
							)
						&&	ptFocus.x == nColNo
						&&	ptFocus.y == nRowNo
						)
					{
						COLORREF clrTextOld =
							dc.SetTextColor( OnSiwGetSysColor( COLOR_HIGHLIGHTTEXT ) );
						COLORREF clrBkOld =
							dc.SetBkColor( OnSiwGetSysColor( COLOR_HIGHLIGHT ) );
						CRect rcCellFocus( rcCell );
						rcCellFocus.DeflateRect(
							0,
							0,
							bGridLinesVert ? 1 : 0,
							bGridLinesHorz ? 1 : 0
							);
						dc.DrawFocusRect( &rcCellFocus );
						dc.SetBkColor( clrBkOld );
						dc.SetTextColor( clrTextOld );
					} // if( dwBasicSelectionType == __EGBS_SFB_CELLS ...
					else if(
							dwBasicSelectionType == __EGBS_SFB_FULL_COLUMNS
						//&&	dwScrollTypeH != __ESIW_ST_VIRTUAL
						&&	ptFocus.x == nColNo
						)
					{
						LONG nRowFirst = 0, nRowLast = 0, nRowFirstOffset = 0;
						OnGbwQueryVisibleRowRange(
							nRowFirst,
							nRowLast,
							nRowFirstOffset
							);
						ASSERT( nRowFirst <= nRowLast );
						COLORREF clrTextOld =
							dc.SetTextColor( OnSiwGetSysColor( COLOR_HIGHLIGHTTEXT ) );
						COLORREF clrBkOld =
							dc.SetBkColor( OnSiwGetSysColor( COLOR_HIGHLIGHT ) );
						CRect rcCellFocus( rcCell );
						rcCellFocus.DeflateRect(
							0,
							bGridLinesHorz ? -1 : 0,
							bGridLinesVert ? 1 : 0,
							0
							);
						CRect rc( &rcCellFocus );
						rc.right = rc.left;
						if( !bGridLinesHorz )
						{
							rc.bottom ++;
							rc.top --;
						}
						dc.DrawFocusRect( &rc );
						rc = rcCellFocus;
						rc.left = rc.right;
						if( !bGridLinesHorz )
						{
							rc.bottom ++;
							rc.top --;
						}
						dc.DrawFocusRect( &rc );
						//if( nRowNo == nRowFirst )
						if(		( dwScrollTypeV != __ESIW_ST_VIRTUAL && nRowNo == 0 )
							||	( dwScrollTypeV == __ESIW_ST_VIRTUAL && nRowNo == nRowFirst )
							)
						{
							rc = rcCellFocus;
							rc.top ++;
							rc.bottom = rc.top;
							dc.DrawFocusRect( &rc );
						}
						//if( nRowNo == nRowLast )
						if(		( dwScrollTypeV != __ESIW_ST_VIRTUAL && nRowNo == (nRowCount-1) )
							||	( dwScrollTypeV == __ESIW_ST_VIRTUAL && nRowNo == nRowLast )
							)
						{
							rc = rcCell;
							rc.bottom --;
							rc.top = rc.bottom;
							dc.DrawFocusRect( &rc );
						}
						dc.SetBkColor( clrBkOld );
						dc.SetTextColor( clrTextOld );
					} // else if( dwBasicSelectionType == __EGBS_SFB_FULL_COLUMNS ...
					else if(
							dwBasicSelectionType == __EGBS_SFB_FULL_ROWS
						//&&	dwScrollTypeV != __ESIW_ST_VIRTUAL
						&&	ptFocus.y == nRowNo
						)
					{
						LONG nColFirst = 0, nColLast = 0, nColFirstOffset = 0;
						OnGbwQueryVisibleColumnRange(
							nColFirst,
							nColLast,
							nColFirstOffset
							);
						ASSERT( nColFirst <= nColLast );
						COLORREF clrTextOld =
							dc.SetTextColor( OnSiwGetSysColor( COLOR_HIGHLIGHTTEXT ) );
						COLORREF clrBkOld =
							dc.SetBkColor( OnSiwGetSysColor( COLOR_HIGHLIGHT ) );
						CRect rcCellFocus( rcCell );
						rcCellFocus.DeflateRect(
							bGridLinesVert ? -1 : 0,
							0,
							0,
							bGridLinesHorz ? 1 : 0
							);
						CRect rc( &rcCellFocus );
						rc.bottom = rc.top;
						if( !bGridLinesVert )
						{
							rc.right ++;
							rc.left --;
						}
						dc.DrawFocusRect( &rc );
						rc = rcCellFocus;
						rc.top = rc.bottom;
						if( !bGridLinesVert )
						{
							rc.right ++;
							rc.left --;
						}
						dc.DrawFocusRect( &rc );
						//if( nColNo == nColFirst )
						if(		( dwScrollTypeH != __ESIW_ST_VIRTUAL && nColNo == 0 )
							||	( dwScrollTypeH == __ESIW_ST_VIRTUAL && nColNo == nColFirst )
							)
						{
							rc = rcCellFocus;
							rc.left ++;
							rc.right = rc.left;
							dc.DrawFocusRect( &rc );
						}
						//if( nColNo == nColLast )
						if(		( dwScrollTypeH != __ESIW_ST_VIRTUAL && nColNo == (nColumnCount-1) )
							||	( dwScrollTypeH == __ESIW_ST_VIRTUAL && nColNo == nColLast )
							)
						{
							rc = rcCell;
							rc.right --;
							rc.left = rc.right;
							dc.DrawFocusRect( &rc );
						}
						dc.SetBkColor( clrBkOld );
						dc.SetTextColor( clrTextOld );
					} // if( dwBasicSelectionType == __EGBS_SFB_FULL_ROWS ...
				} // if( bFocusedControl )
			} // if( dwBasicSelectionType != __EGBS_SFB_NONE ...
		} // if( bFocusedControl || NoHideSelectionGet() )

		if( bGridLinesHorz || bGridLinesVert )
		{
			COLORREF clrFace = OnSiwGetSysColor( COLOR_3DFACE );
			if( bGridLinesHorz )
			{
				dc.FillSolidRect(
					rcCell.left,
					rcCell.top-1,
					rcCell.right - rcCell.left,
					1,
					clrFace
					);
				dc.FillSolidRect(
					rcCell.left,
					rcCell.bottom-1,
					rcCell.right - rcCell.left,
					1,
					clrFace
					);
			} // if( bGridLinesHorz )
			if( bGridLinesVert )
			{
				dc.FillSolidRect(
					rcCell.left-1,
					rcCell.top,
					1,
					rcCell.bottom - rcCell.top,
					clrFace
					);
				dc.FillSolidRect(
					rcCell.right-1,
					rcCell.top,
					1,
					rcCell.bottom - rcCell.top,
					clrFace
					);
			} // if( bGridLinesVert )
		} // if( bGridLinesHorz || bGridLinesVert )

	} // else from if( ( dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0 )

//CString sCellText;
//	sCellText.Format( _T("%08d-%08d"), nColNo, nRowNo );
//	dc.DrawText(
//		sCellText,
//		(LPRECT)&rcCell,
//		DT_SINGLELINE|DT_CENTER|DT_VCENTER
//		);
//	dc.Draw3dRect( &rcCell, clrLT, clrRB );
}

CRect CExtGridBaseWnd::OnGbwQueryCellBorderHtSpaces(
	CExtGridHitTestInfo * pHT // = NULL // if NULL - get default resizing border spaces
	) const
{
	ASSERT_VALID( this );
	pHT;
	ASSERT( m_rcDefaultCellBorderHtSpaces.left >= 0 );
	ASSERT( m_rcDefaultCellBorderHtSpaces.right >= 0 );
	ASSERT( m_rcDefaultCellBorderHtSpaces.top >= 0 );
	ASSERT( m_rcDefaultCellBorderHtSpaces.bottom >= 0 );
	return m_rcDefaultCellBorderHtSpaces;
}

bool CExtGridBaseWnd::OnSiwWalkCell(
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
//	ASSERT( nColNo >= 0 );
//	ASSERT( nRowNo >= 0 );
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
	rcVisibleRange;
	dwAreaFlags;

	if(		rcCellExtra.right == rcCellExtra.left
		||	rcCellExtra.bottom == rcCellExtra.top
		)
		return false;
	if(		dc.GetSafeHdc() != NULL
		&&	(! dc.RectVisible( &rcCell ) )
		)
		return false;

	if( nColNo >= 0 )
	{
		bVirtualRightReached = OnSiwVirtualEndTestH( nColNo, LPARAM(dwAreaFlags) );
		if( bVirtualRightReached )
			return false;
	} // if( nColNo >= 0 )
	if( nRowNo >= 0 )
	{
		bVirtualBottomReached = OnSiwVirtualEndTestV( nRowNo, LPARAM(dwAreaFlags) );
		if( bVirtualBottomReached )
			return false;
	} // if( nRowNo >= 0 )

	if( pQueryData != NULL )
	{
		if( ::PtInRect( &rcCellExtra, ((CExtGridHitTestInfo *)pQueryData)->m_ptClient ) )
		{
			((CExtGridHitTestInfo *)pQueryData)->m_dwAreaFlags = dwAreaFlags;
			((CExtGridHitTestInfo *)pQueryData)->m_nRowNo = nRowNo;
			((CExtGridHitTestInfo *)pQueryData)->m_nColNo = nColNo;
			((CExtGridHitTestInfo *)pQueryData)->m_nVisibleRowNo = nVisibleRowNo;
			((CExtGridHitTestInfo *)pQueryData)->m_nVisibleColNo = nVisibleColNo;
			((CExtGridHitTestInfo *)pQueryData)->m_rcExtra = rcCellExtra;
			((CExtGridHitTestInfo *)pQueryData)->m_rcItem = rcCell;
			if( ::PtInRect( &rcCell, ((CExtGridHitTestInfo *)pQueryData)->m_ptClient ) )
			{
				CRect rcDefaultCellBorderHtSpaces =
					OnGbwQueryCellBorderHtSpaces(
						(CExtGridHitTestInfo *)pQueryData
						);
				
				CRect rcBorderTest( rcCell );
				rcBorderTest.right = rcBorderTest.left + rcDefaultCellBorderHtSpaces.left;
				if( rcBorderTest.PtInRect( ((CExtGridHitTestInfo *)pQueryData)->m_ptClient ) )
					((CExtGridHitTestInfo *)pQueryData)->m_dwAreaFlags |= __EGBWA_NEAR_CELL_BORDER_LEFT;
				
				rcBorderTest = rcCell;
				rcBorderTest.bottom = rcBorderTest.top + rcDefaultCellBorderHtSpaces.top;
				if( rcBorderTest.PtInRect( ((CExtGridHitTestInfo *)pQueryData)->m_ptClient ) )
					((CExtGridHitTestInfo *)pQueryData)->m_dwAreaFlags |= __EGBWA_NEAR_CELL_BORDER_TOP;
				
				rcBorderTest = rcCell;
				rcBorderTest.left = rcBorderTest.right - rcDefaultCellBorderHtSpaces.right;
				if( rcBorderTest.PtInRect( ((CExtGridHitTestInfo *)pQueryData)->m_ptClient ) )
					((CExtGridHitTestInfo *)pQueryData)->m_dwAreaFlags |= __EGBWA_NEAR_CELL_BORDER_RIGHT;
				
				rcBorderTest = rcCell;
				rcBorderTest.top = rcBorderTest.bottom - rcDefaultCellBorderHtSpaces.bottom;
				if( rcBorderTest.PtInRect( ((CExtGridHitTestInfo *)pQueryData)->m_ptClient ) )
					((CExtGridHitTestInfo *)pQueryData)->m_dwAreaFlags |= __EGBWA_NEAR_CELL_BORDER_BOTTOM;
			} // if( ::PtInRect( &rcCell, ((CExtGridHitTestInfo *)pQueryData)->m_ptClient ) )
			return true;
		} // if( ::PtInRect( &rcCellExtra, ((CExtGridHitTestInfo *)pQueryData)->m_ptClient ) )
		return false;
	} // if( pQueryData != NULL )

INT nFriendlyColNo = nColNo;
INT nFriendlyRowNo = nRowNo;
	if( ( dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0 )
	{
		ASSERT( nFriendlyColNo < 0 || nFriendlyRowNo < 0 );
		ASSERT( ! (nFriendlyColNo < 0 && nFriendlyRowNo < 0 ) );
		if( nFriendlyColNo < 0 )
			nFriendlyColNo = (-nFriendlyColNo) - 1;
		else
			nFriendlyRowNo = (-nFriendlyRowNo) - 1;
		ASSERT( nFriendlyColNo >= 0 );
		ASSERT( nFriendlyRowNo >= 0 );
	} // if( ( dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0 )
	else
	{
		ASSERT( nFriendlyColNo >= 0 );
		ASSERT( nFriendlyRowNo >= 0 );
	} // else from if( ( dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0 )
bool bHoverByColumn = false, bHoverByRow = false,
		bHighlightedBySelectedColumn = false, bHighlightedBySelectedRow = false,
		bHighlightedByFocusedColumn = false, bHighlightedByFocusedRow = false;
CPoint ptFocus = FocusGet();
	if( ( dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0 )
	{
		// compute hover flags
		if( HoverEventsOuterGet() && HoverHighlightCellOuterGet() )
		{
			bool bOuterCellAtLeftRight;
			if( m_htInfoHover.IsHoverOuter( &bOuterCellAtLeftRight ) )
			{
				if(			bOuterCellAtLeftRight
						&&	(dwAreaFlags &__EGBWA_OUTER_H) != 0
						&&	nRowNo == m_htInfoHover.m_nRowNo
					)
					bHoverByColumn = bHoverByRow = true;
				else if(	( ! bOuterCellAtLeftRight )
						&&	(dwAreaFlags &__EGBWA_OUTER_V) != 0
						&&	nColNo == m_htInfoHover.m_nColNo
					)
					bHoverByColumn = bHoverByRow = true;
			} // if( m_htInfoHover.IsHoverOuter( &bOuterCellAtLeftRight ) )
		} // if( HoverEventsOuterGet() && HoverHighlightCellOuterGet() )
		if(		( ! bHoverByColumn )
			&&	( ! bHoverByRow )
			&&	HoverEventsInnerGet()
			&&	m_htInfoHover.IsHoverInner()
			)
		{
				if(			(dwAreaFlags &__EGBWA_OUTER_H) != 0
						&&	nRowNo == m_htInfoHover.m_nRowNo
						&&  HoverHighlightRowOuterGet()
					)
					bHoverByRow = true;
				else if(	(dwAreaFlags &__EGBWA_OUTER_V) != 0
						&&	nColNo == m_htInfoHover.m_nColNo
						&&  HoverHighlightColumnOuterGet()
					)
					bHoverByColumn = true;
		} // if( ( ! bHoverByColumn ) ....
		// compute outer column selection highlight flags
		if(		(dwAreaFlags &__EGBWA_OUTER_V) != 0
			&&	SelectionHighlightColumnOuterGet()
			&&	SelectionGetForCell( nColNo, -1 )
			)
			bHighlightedBySelectedColumn = true;
		// compute outer row selection highlight flags
		if(		(dwAreaFlags &__EGBWA_OUTER_H) != 0
			&&	SelectionHighlightRowOuterGet()
			&&	SelectionGetForCell( -1, nRowNo )
			)
			bHighlightedBySelectedRow = true;
		// compute outer column focus highlight flags
		if(		(dwAreaFlags &__EGBWA_OUTER_V) != 0
			&&	ptFocus.x >= 0
			&&	FocusHighlightColumnOuterGet()
			&&	ptFocus.x == nColNo
			)
			bHighlightedByFocusedColumn = true;
		// compute outer row focus highlight flags
		if(		(dwAreaFlags &__EGBWA_OUTER_H) != 0
			&&	ptFocus.y >= 0
			&&	FocusHighlightRowOuterGet()
			&&	ptFocus.y == nRowNo
			)
			bHighlightedByFocusedRow = true;
	} // if( ( dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0 )
	else
	{
		// compute hover flags
		if(		HoverEventsInnerGet()
			&&	HoverHighlightCellInnerGet()
			&&	m_htInfoHover.IsHoverInner()
			&&	(	nColNo == m_htInfoHover.m_nColNo
				||	nRowNo == m_htInfoHover.m_nRowNo
				)
			)
		{
			if(			nColNo == m_htInfoHover.m_nColNo
					&&	nRowNo == m_htInfoHover.m_nRowNo
					)
					bHoverByColumn = bHoverByRow = true;
			else if(	nColNo == m_htInfoHover.m_nColNo
					&&	HoverHighlightColumnInnerGet()
					)
					bHoverByColumn = true;
			else if(	nRowNo == m_htInfoHover.m_nRowNo
					&&	HoverHighlightRowInnerGet()
					)
					bHoverByRow = true;
		} // if( HoverEventsInnerGet() ...
		if(		( ! bHoverByColumn )
			&&	( ! bHoverByRow )
			&&	HoverEventsOuterGet()
			&&	HoverHighlightCellOuterGet()
			)
		{
			bool bOuterCellAtLeftRight;
			if( m_htInfoHover.IsHoverOuter( &bOuterCellAtLeftRight ) )
			{
				if( bOuterCellAtLeftRight )
				{
					if(		nRowNo == m_htInfoHover.m_nRowNo
						&&  HoverHighlightRowInnerGet()
						)
						bHoverByRow = true;
				} // if( bOuterCellAtLeftRight )
				else
				{
					if(		nColNo == m_htInfoHover.m_nColNo
						&&  HoverHighlightColumnInnerGet()
						)
						bHoverByColumn = true;
				} // else from if( bOuterCellAtLeftRight )
			} // if( m_htInfoHover.IsHoverOuter( &bOuterCellAtLeftRight ) )
		} // if( ( ! bHoverByColumn )
		// compute inner column selection highlight flags
		if(		SelectionHighlightColumnInnerGet()
			&&	SelectionGetForCell( nColNo, -1 )
			)
			bHighlightedBySelectedColumn = true;
		// compute inner row selection highlight flags
		if(		SelectionHighlightRowInnerGet()
			&&	SelectionGetForCell( -1, nRowNo )
			)
			bHighlightedBySelectedRow = true;
		// compute inner column focus highlight flags
		if(		FocusHighlightColumnInnerGet()
			&&	ptFocus.x == nColNo
			)
			bHighlightedByFocusedColumn = true;
		// compute inner row focus highlight flags
		if(		FocusHighlightRowInnerGet()
			&&	ptFocus.y == nRowNo
			)
			bHighlightedByFocusedRow = true;
	} // else from if( ( dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0 )
DWORD dwHelperPaintFlags = 0L;
	if( bHoverByColumn )
		dwHelperPaintFlags |= __EGCPF_HOVER_BY_COLUMN;
	if( bHoverByRow )
		dwHelperPaintFlags |= __EGCPF_HOVER_BY_ROW;
	if( bHighlightedBySelectedColumn )
		dwHelperPaintFlags |= __EGCPF_HIGHLIGHTED_BY_SELECTED_COLUMN;
	if( bHighlightedBySelectedRow )
		dwHelperPaintFlags |= __EGCPF_HIGHLIGHTED_BY_SELECTED_ROW;
	if( bHighlightedByFocusedColumn )
		dwHelperPaintFlags |= __EGCPF_HIGHLIGHTED_BY_FOCUSED_COLUMN;
	if( bHighlightedByFocusedRow )
		dwHelperPaintFlags |= __EGCPF_HIGHLIGHTED_BY_FOCUSED_ROW;
	if( bFocusedControl )
		dwHelperPaintFlags |= __EGCPF_FOCUSED_CONTROL;
	OnGbwPaintCell(
		dc,
		nVisibleColNo,
		nVisibleRowNo,
		nFriendlyColNo,
		nFriendlyRowNo,
		rcCellExtra,
		rcCell,
		rcVisibleRange,
		dwAreaFlags,
		dwHelperPaintFlags
		);
	return false;
}

bool CExtGridBaseWnd::OnSiwWalkItemsH(
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
//	ASSERT( nRowNo >= 0 );
	ASSERT( rcRow.right >= rcRow.left );
	ASSERT( rcRow.bottom >= rcRow.top );
	ASSERT( rcRowExtra.right >= rcRowExtra.left );
	ASSERT( rcRowExtra.bottom >= rcRowExtra.top );
	ASSERT( rcRowExtra.left <= rcRow.left );
	ASSERT( rcRowExtra.top <= rcRow.top );
	ASSERT( rcRowExtra.right >= rcRow.right );
	ASSERT( rcRowExtra.bottom >= rcRow.bottom );
	if( nRowNo >= 0 )
	{
		bVirtualBottomReached = OnSiwVirtualEndTestV( nRowNo, LPARAM(dwAreaFlags) );
		if( bVirtualBottomReached )
			return false;
	}
	if(		rcRowExtra.right == rcRowExtra.left
		||	rcRowExtra.bottom == rcRowExtra.top
		)
		return false;
	if(		ColumnCountGet() == 0
		&&	RowCountGet() == 0
		)
		return false;
	if(		dc.GetSafeHdc() != NULL
		&&	(! dc.RectVisible( &rcRow ) )
		)
		return false;
	if(		pQueryData != NULL
		&&	(	rcRowExtra.left		> ((CExtGridHitTestInfo *)pQueryData)->m_ptClient.x
			||	rcRowExtra.right		< ((CExtGridHitTestInfo *)pQueryData)->m_ptClient.x
			||	rcRowExtra.top		> ((CExtGridHitTestInfo *)pQueryData)->m_ptClient.y
			||	rcRowExtra.bottom	< ((CExtGridHitTestInfo *)pQueryData)->m_ptClient.y
			)
		)
		return false;
CRect rcClient = OnSwGetClientRect();
LONG nVisibleColNo = 0;
bool bVirtualRightReached = false;
CRect rcCellExtra(
		rcClient.left,
		rcRowExtra.top,
		rcClient.left,
		rcRowExtra.bottom
		);
LONG nColNo = rcVisibleRange.left;
LONG nColLast = rcVisibleRange.right;
DWORD dwScrollTypeH = SiwScrollTypeHGet();
	if(		dwScrollTypeH == __ESIW_ST_NONE
		||	dwScrollTypeH == __ESIW_ST_PIXEL
		)
	{
		LONG nColFirstOffset = rcCellExtra.left;
		OnGbwQueryVisibleColumnRange(
			nColNo,
			nColLast,
			nColFirstOffset
			);
		rcCellExtra.left = rcCellExtra.right = nColFirstOffset;
	}
	if(		dc.GetSafeHdc() != NULL
		&&	(	rcRow.left != rcRowExtra.left
			||	rcRow.right != rcRowExtra.right
			||	rcRow.top != rcRowExtra.top
			||	rcRow.bottom != rcRowExtra.bottom
			)
		)
		OnGbwPaintExtraSpaceH(
			false,
			dc,
			rcRowExtra,
			rcRow,
			nVisibleColNo,
			nColNo,
			rcVisibleRange,
			dwAreaFlags,
			bFocusedControl
			);
	for( ; nColNo <= nColLast; nColNo++, nVisibleColNo++ )
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
		if(	!(		rcCellExtra.right < rcClient.left
				||	rcCellExtra.left > rcClient.right
				)
			)
		{
			CRect rcCell(
				rcCellExtra.left + nExtraSpaceBefore,
				rcRow.top,
				rcCellExtra.right - nExtraSpaceAfter,
				rcRow.bottom
				);
			if(	OnSiwWalkCell(
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
					)
				)
			{
				ASSERT( pQueryData != NULL );
				ASSERT( ((CExtGridHitTestInfo *)pQueryData)->m_dwAreaFlags != __EGBWA_NOWHERE );
				return true;
			}
			if( bVirtualRightReached || bVirtualBottomReached )
				break;
		}
		rcCellExtra.left = rcCellExtra.right;
	} // for( ; nColNo <= rcVisibleRange.right; nColNo++, nVisibleColNo++ )
	if(		dc.GetSafeHdc() != NULL
		&&	(	rcRow.left != rcRowExtra.left
			||	rcRow.right != rcRowExtra.right
			||	rcRow.top != rcRowExtra.top
			||	rcRow.bottom != rcRowExtra.bottom
			)
		)
		OnGbwPaintExtraSpaceH(
			true,
			dc,
			rcRowExtra,
			rcRow,
			nVisibleColNo,
			nColNo,
			rcVisibleRange,
			dwAreaFlags,
			bFocusedControl
			);
	return false;
}

bool CExtGridBaseWnd::OnSiwWalkItemsV(
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
//	ASSERT( nColNo >= 0 );
	ASSERT( rcCol.right >= rcCol.left );
	ASSERT( rcCol.bottom >= rcCol.top );
	ASSERT( rcColExtra.right >= rcColExtra.left );
	ASSERT( rcColExtra.bottom >= rcColExtra.top );
	ASSERT( rcColExtra.left <= rcCol.left );
	ASSERT( rcColExtra.top <= rcCol.top );
	ASSERT( rcColExtra.right >= rcCol.right );
	ASSERT( rcColExtra.bottom >= rcCol.bottom );
	if( nColNo >= 0 )
	{
		bVirtualRightReached = OnSiwVirtualEndTestH( nColNo, LPARAM(dwAreaFlags) );
		if( bVirtualRightReached )
			return false;
	}
	if(		rcColExtra.right == rcColExtra.left
		||	rcColExtra.bottom == rcColExtra.top
		)
		return false;
	if(		ColumnCountGet() == 0
		&&	RowCountGet() == 0
		)
		return false;
	if(		dc.GetSafeHdc() != NULL
		&&	(! dc.RectVisible( &rcCol ) )
		)
		return false;
	if(		pQueryData != NULL
		&&	(	rcColExtra.left		> ((CExtGridHitTestInfo *)pQueryData)->m_ptClient.x
			||	rcColExtra.right		< ((CExtGridHitTestInfo *)pQueryData)->m_ptClient.x
			||	rcColExtra.top		> ((CExtGridHitTestInfo *)pQueryData)->m_ptClient.y
			||	rcColExtra.bottom	< ((CExtGridHitTestInfo *)pQueryData)->m_ptClient.y
			)
		)
		return false;
CRect rcClient = OnSwGetClientRect();
LONG nVisibleRowNo = 0;
bool bVirtualBottomReached = false;
CRect rcCellExtra(
		rcColExtra.left,
		rcClient.top,
		rcColExtra.right,
		rcClient.top
		);
LONG nRowNo = rcVisibleRange.top;
LONG nRowLast = rcVisibleRange.bottom;
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	if(		dwScrollTypeV == __ESIW_ST_NONE
		||	dwScrollTypeV == __ESIW_ST_PIXEL
		)
	{
		LONG nRowFirstOffset = rcCellExtra.top;
		OnGbwQueryVisibleRowRange(
			nRowNo,
			nRowLast,
			nRowFirstOffset
			);
		rcCellExtra.top = rcCellExtra.bottom = nRowFirstOffset;
	}
	if(		dc.GetSafeHdc() != NULL
		&&	(	rcCol.left != rcColExtra.left
			||	rcCol.right != rcColExtra.right
			||	rcCol.top != rcColExtra.top
			||	rcCol.bottom != rcColExtra.bottom
			)
		)
		OnGbwPaintExtraSpaceV(
			false,
			dc,
			rcColExtra,
			rcCol,
			nVisibleColNo,
			nColNo,
			rcVisibleRange,
			dwAreaFlags,
			bFocusedControl
			);
	for( ; nRowNo <= nRowLast; nRowNo++, nVisibleRowNo++ )
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
		if(	!(	rcCellExtra.bottom < rcClient.top
				||	rcCellExtra.top > rcClient.bottom
				)
			)
		{
			CRect rcCell(
				rcCol.left,
				rcCellExtra.top + nExtraSpaceBefore,
				rcCol.right,
				rcCellExtra.bottom - nExtraSpaceAfter
				);
			if(	OnSiwWalkCell(
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
					)
				)
			{
				ASSERT( pQueryData != NULL );
				ASSERT( ((CExtGridHitTestInfo *)pQueryData)->m_dwAreaFlags != __EGBWA_NOWHERE );
				return true;
			}
			if( bVirtualRightReached || bVirtualBottomReached )
				break;
		}
		rcCellExtra.top = rcCellExtra.bottom;
	} // for( ; nRowNo <= nRowLast; nRowNo++, nVisibleRowNo++ )
	if(		dc.GetSafeHdc() != NULL
		&&	(	rcCol.left != rcColExtra.left
			||	rcCol.right != rcColExtra.right
			||	rcCol.top != rcColExtra.top
			||	rcCol.bottom != rcColExtra.bottom
			)
		)
		OnGbwPaintExtraSpaceV(
			true,
			dc,
			rcColExtra,
			rcCol,
			nVisibleColNo,
			nColNo,
			rcVisibleRange,
			dwAreaFlags,
			bFocusedControl
			);
	return false;
}

void CExtGridBaseWnd::OnGbwPaintExtraSpaceH(
	bool bPostPaint,
	CDC & dc,
	const RECT & rcRowExtra,
	const RECT & rcRow,
	LONG nVisibleRowNo,
	LONG nRowNo,
	const RECT & rcVisibleRange,
	DWORD dwAreaFlags,
	bool bFocusedControl
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL);
	ASSERT( nVisibleRowNo >= 0 );
	ASSERT( rcRow.right >= rcRow.left );
	ASSERT( rcRow.bottom >= rcRow.top );
	ASSERT( rcRowExtra.right >= rcRowExtra.left );
	ASSERT( rcRowExtra.bottom >= rcRowExtra.top );
	ASSERT( rcRowExtra.left <= rcRow.left );
	ASSERT( rcRowExtra.top <= rcRow.top );
	ASSERT( rcRowExtra.right >= rcRow.right );
	ASSERT( rcRowExtra.bottom >= rcRow.bottom );
	bPostPaint;
	dc;
	rcRowExtra;
	rcRow;
	nVisibleRowNo;
	nRowNo;
	rcVisibleRange;
	dwAreaFlags;
	bFocusedControl;
//CRect rcSpaceBefore(
//		rcRowExtra.left,
//		rcRowExtra.top,
//		rcRowExtra.right,
//		rcRow.top
//		);
//CRect rcSpaceAfter(
//		rcRowExtra.left,
//		rcRow.bottom,
//		rcRowExtra.right,
//		rcRowExtra.bottom
//		);
//	dc.FillSolidRect( &rcSpaceBefore, RGB(255,128,128) );
//	dc.FillSolidRect( &rcSpaceAfter, RGB(128,255,128) );
}

void CExtGridBaseWnd::OnGbwPaintExtraSpaceV(
	bool bPostPaint,
	CDC & dc,
	const RECT & rcColExtra,
	const RECT & rcCol,
	LONG nVisibleColNo,
	LONG nColNo,
	const RECT & rcVisibleRange,
	DWORD dwAreaFlags,
	bool bFocusedControl
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL);
	ASSERT( nVisibleColNo >= 0 );
	ASSERT( rcCol.right >= rcCol.left );
	ASSERT( rcCol.bottom >= rcCol.top );
	ASSERT( rcColExtra.right >= rcColExtra.left );
	ASSERT( rcColExtra.bottom >= rcColExtra.top );
	ASSERT( rcColExtra.left <= rcCol.left );
	ASSERT( rcColExtra.top <= rcCol.top );
	ASSERT( rcColExtra.right >= rcCol.right );
	ASSERT( rcColExtra.bottom >= rcCol.bottom );
	bPostPaint;
	dc;
	rcColExtra;
	rcCol;
	nVisibleColNo;
	nColNo;
	rcVisibleRange;
	dwAreaFlags;
	bFocusedControl;
}

INT CExtGridBaseWnd::OnSiwQueryItemExtentH(
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
	return DefaultColumnWidthGet();
}

INT CExtGridBaseWnd::OnSiwQueryItemExtentV(
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
	return DefaultRowHeightGet();
}

INT CExtGridBaseWnd::OuterColumnWidthGet(
	bool bLeft,
	LONG nColNo,
	bool bZeroIfNoRows // = true
	) const
{
	ASSERT_VALID( this );
	ASSERT( nColNo >= 0 );
	if( bZeroIfNoRows )
	{
		LONG nRowCount = RowCountGet();
		if( nRowCount == 0L )
			return 0;
	} // if( bZeroIfNoRows )
const index2extent_map_t & _map = bLeft
		? m_mapExtentsLeft : m_mapExtentsRight; 
INT nExtent = 0;
	if( ! _map.Lookup( nColNo, nExtent ) )
		nExtent = DefaultColumnWidthGet();
	return nExtent;
}

INT CExtGridBaseWnd::OuterRowHeightGet(
	bool bTop,
	LONG nRowNo,
	bool bZeroIfNoColumns // = true
	) const
{
	ASSERT_VALID( this );
	ASSERT( nRowNo >= 0 );
	if( bZeroIfNoColumns )
	{
		LONG nColCount = ColumnCountGet();
		if( nColCount == 0L )
			return 0;
	} // if( bZeroIfNoColumns )
const index2extent_map_t & _map = bTop
		? m_mapExtentsTop : m_mapExtentsBottom; 
INT nExtent = 0;
	if( ! _map.Lookup( nRowNo, nExtent ) )
		nExtent = DefaultRowHeightGet();
	return nExtent;
}

void CExtGridBaseWnd::OuterColumnWidthSet(
	bool bLeft,
	LONG nColNo,
	INT nWidth
	)
{
	ASSERT_VALID( this );
	ASSERT( nWidth >= 0 && nColNo >= 0 );
	ASSERT(
		nColNo <
			( bLeft
				? OuterColumnCountLeftGet()
				: OuterColumnCountRightGet()
			)
		);
index2extent_map_t & _map = bLeft
		? m_mapExtentsLeft : m_mapExtentsRight; 
	_map.SetAt( nColNo, nWidth );
}

void CExtGridBaseWnd::OuterRowHeightSet(
	bool bTop,
	LONG nRowNo,
	INT nHeight
	)
{
	ASSERT_VALID( this );
	ASSERT( nHeight >= 0 && nRowNo >= 0 );
	ASSERT(
		nRowNo <
			( bTop
				? OuterRowCountTopGet()
				: OuterRowCountBottomGet()
			)
		);
index2extent_map_t & _map = bTop
		? m_mapExtentsTop : m_mapExtentsBottom; 
	_map.SetAt( nRowNo, nHeight );
}

void CExtGridBaseWnd::OuterExtentsClearAll(
	bool bLeft, // = true
	bool bRight, // = true
	bool bTop, // = true
	bool bBottom // = true
	)
{
	ASSERT_VALID( this );
	if( bLeft )
		m_mapExtentsLeft.RemoveAll();
	if( bRight )
		m_mapExtentsRight.RemoveAll();
	if( bTop )
		m_mapExtentsTop.RemoveAll();
	if( bBottom )
		m_mapExtentsBottom.RemoveAll();
}

bool CExtGridBaseWnd::OnSwDoMouseWheel(
	UINT fFlags,
	short zDelta,
	CPoint point
	)
{
	ASSERT_VALID( this );
	if( m_hWndInplaceControl != NULL )
		return true;
	if( m_wndContentExpand.GetSafeHwnd() != NULL )
	{
		m_wndContentExpand.Deactivate();
		m_htInfoExpand.Empty();
	} // if( m_wndContentExpand.GetSafeHwnd() != NULL )
bool bRetVal =
		CExtScrollItemWnd::OnSwDoMouseWheel(
			fFlags,
			zDelta,
			point
			);
	ScreenToClient( &point );
	OnGbwHoverRecalc( &point );
	return bRetVal;
}

CRect CExtGridBaseWnd::OnSwRecalcLayout(
	bool bDoLayout,
	LPCRECT pRectClientSrc // = NULL
	)
{
	ASSERT_VALID( this );
CRect rcClient = CExtScrollItemWnd::OnSwRecalcLayout( bDoLayout, pRectClientSrc );
	m_rcOuterT.SetRect( 0, 0, 0, 0 );
	m_rcOuterB.SetRect( 0, 0, 0, 0 );
	m_rcOuterL.SetRect( 0, 0, 0, 0 );
	m_rcOuterR.SetRect( 0, 0, 0, 0 );
	m_rcOuterLT.SetRect( 0, 0, 0, 0 );
	m_rcOuterRT.SetRect( 0, 0, 0, 0 );
	m_rcOuterLB.SetRect( 0, 0, 0, 0 );
	m_rcOuterRB.SetRect( 0, 0, 0, 0 );
	m_rcClient.SetRect( 0, 0, 0, 0 );
INT nOuterMeric;
LONG i;
LONG nOuterRowCountT = OuterRowCountTopGet();
	for( i = 0, nOuterMeric = 0; i < nOuterRowCountT; i++ )
	{
		INT nMetric = OuterRowHeightGet( true, i );
		ASSERT( nMetric >= 0 );
		nOuterMeric += nMetric;
	}
	if( nOuterMeric > 0 )
	{
		m_rcOuterT = rcClient;
		m_rcOuterT.bottom = m_rcOuterT.top + nOuterMeric;
		rcClient.top = m_rcOuterT.bottom;
		if( rcClient.bottom < rcClient.top )
			rcClient.bottom = rcClient.top;
		m_rcOuterT.right = max( m_rcOuterT.left, m_rcOuterT.right );
	}
LONG nOuterRowCountB = OuterRowCountBottomGet();
	for( i = 0, nOuterMeric = 0; i < nOuterRowCountB; i++ )
	{
		INT nMetric = OuterRowHeightGet( false, i );
		ASSERT( nMetric >= 0 );
		nOuterMeric += nMetric;
	}
	if( nOuterMeric > 0 )
	{
		m_rcOuterB = rcClient;
		m_rcOuterB.top = m_rcOuterB.bottom - nOuterMeric;
		rcClient.bottom = m_rcOuterB.top;
		if( rcClient.bottom < rcClient.top )
			rcClient.bottom = rcClient.top;
		m_rcOuterB.right = max( m_rcOuterB.left, m_rcOuterB.right );
		if( m_rcOuterB.top < rcClient.bottom )
			m_rcOuterB.OffsetRect(
				0,
				rcClient.bottom - m_rcOuterB.top
				);
	}
LONG nOuterColumnCountL = OuterColumnCountLeftGet();
	for( i = 0, nOuterMeric = 0; i < nOuterColumnCountL; i++ )
	{
		INT nMetric = OuterColumnWidthGet( true, i );
		ASSERT( nMetric >= 0 );
		nOuterMeric += nMetric;
	}
	if( nOuterMeric > 0 )
	{
		m_rcOuterL = rcClient;
		m_rcOuterL.right = m_rcOuterL.left + nOuterMeric;
		rcClient.left = m_rcOuterL.right;
		if( rcClient.right < rcClient.left )
			rcClient.right = rcClient.left;
		m_rcOuterL.bottom = max( m_rcOuterL.top, m_rcOuterL.bottom );
		if( ! m_rcOuterT.IsRectEmpty() )
		{
			m_rcOuterT.left = max( m_rcOuterT.left, rcClient.left );
			m_rcOuterT.right = max( m_rcOuterT.left, m_rcOuterT.right );
			m_rcOuterLT.SetRect(
				m_rcOuterL.left, m_rcOuterT.top,
				m_rcOuterL.right, m_rcOuterT.bottom
				);
			m_rcOuterLT.left = min( m_rcOuterLT.left, m_rcOuterLT.right );
			m_rcOuterLT.top = min( m_rcOuterLT.top, m_rcOuterLT.bottom );
		}
		if( ! m_rcOuterB.IsRectEmpty() )
		{
			m_rcOuterB.left = max( m_rcOuterB.left, rcClient.left );
			m_rcOuterB.right = max( m_rcOuterB.left, m_rcOuterB.right );
			m_rcOuterLB.SetRect(
				m_rcOuterL.left, m_rcOuterB.top,
				m_rcOuterL.right, m_rcOuterB.bottom
				);
			m_rcOuterLB.left = min( m_rcOuterLB.left, m_rcOuterLB.right );
			m_rcOuterLB.top = min( m_rcOuterLB.top, m_rcOuterLB.bottom );
		}
	}
LONG nOuterColumnCountR = OuterColumnCountRightGet();
	for( i = 0, nOuterMeric = 0; i < nOuterColumnCountR; i++ )
	{
		INT nMetric = OuterColumnWidthGet( false, i );
		ASSERT( nMetric >= 0 );
		nOuterMeric += nMetric;
	}
	if( nOuterMeric > 0 )
	{
		m_rcOuterR = rcClient;
		m_rcOuterR.left = m_rcOuterR.right - nOuterMeric;
		rcClient.right = m_rcOuterR.left;
		if( rcClient.right < rcClient.left )
			rcClient.right = rcClient.left;
		m_rcOuterR.bottom = max( m_rcOuterR.top, m_rcOuterR.bottom );
		if( m_rcOuterR.left < rcClient.right )
			m_rcOuterR.OffsetRect(
				rcClient.right - m_rcOuterR.left,
				0
				);
		if( ! m_rcOuterT.IsRectEmpty() )
		{
			m_rcOuterT.right = min( m_rcOuterT.right, rcClient.right );
			m_rcOuterT.left = min( m_rcOuterT.left, m_rcOuterT.right );
			m_rcOuterRT.SetRect(
				m_rcOuterR.left, m_rcOuterT.top,
				m_rcOuterR.right, m_rcOuterT.bottom
				);
			m_rcOuterRT.left = min( m_rcOuterRT.left, m_rcOuterRT.right );
			m_rcOuterRT.top = min( m_rcOuterRT.top, m_rcOuterRT.bottom );
		}
		if( ! m_rcOuterB.IsRectEmpty() )
		{
			m_rcOuterB.right = min( m_rcOuterB.right, rcClient.right );
			m_rcOuterB.left = min( m_rcOuterB.left, m_rcOuterT.right );
			m_rcOuterRB.SetRect(
				m_rcOuterR.left, m_rcOuterB.top,
				m_rcOuterR.right, m_rcOuterB.bottom
				);
			m_rcOuterRB.left = min( m_rcOuterRB.left, m_rcOuterRB.right );
			m_rcOuterRB.top = min( m_rcOuterRB.top, m_rcOuterRB.bottom );
		}
	}
	return rcClient;
}

CSize CExtGridBaseWnd::OnSiwCalcItemSize() const
{
	ASSERT_VALID( this );
CSize sizeItem(
		DefaultColumnWidthGet(),
		DefaultRowHeightGet()
		);
	return sizeItem;
}

CSize CExtGridBaseWnd::OnSwGetTotalSize() const
{
	ASSERT_VALID( this );
LONG nCountOfColumns = ColumnCountGet();
LONG nCountOfRows = RowCountGet();
CSize sizeTotal( nCountOfColumns, nCountOfRows );
CSize sizeItem = OnSiwCalcItemSize();
CRect rcClient = OnSwGetClientRect();
CSize sizeClient = rcClient.Size();

DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	
	//if( dwScrollTypeH == __ESIW_ST_NONE )
	if( dwScrollTypeH == __ESIW_ST_NONE && dwScrollTypeV != __ESIW_ST_NONE )
		sizeTotal.cx = 0;
	else if( dwScrollTypeH == __ESIW_ST_PIXEL )
	{
		if( FixedSizeColumnsGet() )
			sizeTotal.cx *= sizeItem.cx;
		else
		{
			int nColCount = ColumnCountGet();
			ASSERT( nColCount >= 0 );
			sizeTotal.cx = 0;
			for( LONG nColNo = 0; nColNo < nColCount; nColNo ++ )
			{
				INT nItemExtent = OnSiwQueryItemExtentH( nColNo );
				ASSERT( nItemExtent >= 0 );
				sizeTotal.cx += nItemExtent;
			} // for( LONG nColNo = 0; nColNo < nColCount; nColNo ++ )
		} // else from if( FixedSizeColumnsGet() )
	} // else if( dwScrollTypeH == __ESIW_ST_PIXEL )
	else if( dwScrollTypeH == __ESIW_ST_VIRTUAL )
	{
		ASSERT( ColumnCountGet() == -1 );
		sizeTotal.cx = __EXT_SCROLL_NUMERIC_MAX;
	} // else if( dwScrollTypeH == __ESIW_ST_VIRTUAL )

	//if( dwScrollTypeV == __ESIW_ST_NONE )
	if( dwScrollTypeV == __ESIW_ST_NONE && dwScrollTypeH != __ESIW_ST_NONE )
		sizeTotal.cy = 0;
	else if( dwScrollTypeV == __ESIW_ST_PIXEL )
	{
		if( FixedSizeRowsGet() )
			sizeTotal.cy *= sizeItem.cy;
		else
		{
			int nRowCount = RowCountGet();
			ASSERT( nRowCount >= 0 );
			sizeTotal.cy = 0;
			for( LONG nRowNo = 0; nRowNo < nRowCount; nRowNo ++ )
			{
				INT nItemExtent = OnSiwQueryItemExtentV( nRowNo );
				ASSERT( nItemExtent >= 0 );
				sizeTotal.cy += nItemExtent;
			} // for( LONG nRowNo = 0; nRowNo < nRowCount; nRowNo ++ )
		} // else from if( FixedSizeRowsGet() )
	} // else if( dwScrollTypeV == __ESIW_ST_PIXEL )
	else if( dwScrollTypeV == __ESIW_ST_VIRTUAL )
	{
		ASSERT( RowCountGet() == -1 );
		sizeTotal.cy = __EXT_SCROLL_NUMERIC_MAX;
	} // else if( dwScrollTypeV == __ESIW_ST_VIRTUAL )

	return sizeTotal;
}

CSize CExtGridBaseWnd::OnSiwCalcPageMetrics( int nDirection ) const
{
	ASSERT_VALID( this );
	nDirection;

CSize _sizePageMetrics = CExtScrollItemWnd::OnSiwCalcPageMetrics( nDirection );
DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();

CRect rcClient = OnSwGetClientRect();
CSize _sizeClient = rcClient.Size();
	if( _sizeClient.cx < 0 )
		_sizeClient.cx = 0;
	if( _sizeClient.cy < 0 )
		_sizeClient.cy = 0;

	if( dwScrollTypeH == __ESIW_ST_PIXEL )
		_sizePageMetrics.cx = _sizeClient.cx;
	else if( dwScrollTypeH == __ESIW_ST_ITEM )
	{
		int nColCount = ColumnCountGet();
		ASSERT( nColCount >= 0 );
		CRect rcClient = OnSwGetClientRect();
		INT nClientWidth = rcClient.Width();
		INT nCalcWidth = 0;
		LONG nColNo = ScrollPos32Get( SB_HORZ, false );
		ASSERT( nColNo >= 0 );
		_sizePageMetrics.cx = 0;
		if( nDirection < 0 )
		{
			for( ; nColNo > 0; nColNo--, _sizePageMetrics.cx++ )
			{
				INT nItemExtent = OnSiwQueryItemExtentH( nColNo - 1 );
				nCalcWidth += nItemExtent;
				if( nCalcWidth >= nClientWidth )
					break;
			}
		} // if( nDirection < 0 )
		else
		{ // this page/next metric
			int nColStart = nColNo;
			for( ; nColNo < nColCount; nColNo++, _sizePageMetrics.cx++ )
			{
				INT nItemExtent = OnSiwQueryItemExtentH( nColNo );
				nCalcWidth += nItemExtent;
				if( nCalcWidth >= nClientWidth )
					break;
			}
			if(		nColNo == nColCount
				&&	nColStart > 0
				&&	nCalcWidth < nClientWidth
				)
			{
				nColNo = nColStart;
				for( ; nColNo > 0; nColNo--, _sizePageMetrics.cx++ )
				{
					INT nItemExtent = OnSiwQueryItemExtentH( nColNo - 1 );
					nCalcWidth += nItemExtent;
					if( nCalcWidth >= nClientWidth )
						break;
				}
			} // if( nColNo == nColCount && ...
		} // this page/next metric
	} // else if( dwScrollTypeH == __ESIW_ST_ITEM )
	
	if( dwScrollTypeV == __ESIW_ST_PIXEL )
		_sizePageMetrics.cy = _sizeClient.cy;
	else if( dwScrollTypeV == __ESIW_ST_ITEM )
	{
		int nRowCount = RowCountGet();
		ASSERT( nRowCount >= 0 );
		CRect rcClient = OnSwGetClientRect();
		INT nClientHeight = rcClient.Height();
		INT nCalcHeight = 0;
		LONG nRowNo = ScrollPos32Get( SB_VERT, false );
		ASSERT( nRowNo >= 0 );
		_sizePageMetrics.cy = 0;
		if( nDirection < 0 )
		{
			for( ; nRowNo > 0; nRowNo--, _sizePageMetrics.cy++ )
			{
				INT nItemExtent = OnSiwQueryItemExtentV( nRowNo - 1 );
				nCalcHeight += nItemExtent;
				if( nCalcHeight >= nClientHeight )
					break;
			}
		} // if( nDirection < 0 )
		else
		{ // this page/next metric
			int nRowStart = nRowNo;
			for( ; nRowNo < nRowCount; nRowNo++, _sizePageMetrics.cy++ )
			{
				INT nItemExtent = OnSiwQueryItemExtentV( nRowNo );
				nCalcHeight += nItemExtent;
				if( nCalcHeight >= nClientHeight )
					break;
			}
			if(		nRowNo == nRowCount
				&&	nRowStart > 0
				&&	nCalcHeight < nClientHeight )
			{
				nRowNo = nRowStart;
				for( ; nRowNo > 0; nRowNo--, _sizePageMetrics.cy++ )
				{
					INT nItemExtent = OnSiwQueryItemExtentV( nRowNo - 1 );
					nCalcHeight += nItemExtent;
					if( nCalcHeight >= nClientHeight )
						break;
				}
			} // if( nRowNo == nRowCount && ...
		} // this page/next metric
	} // else if( dwScrollTypeV == __ESIW_ST_ITEM )
	
	if( _sizePageMetrics.cx == 0 )
		_sizePageMetrics.cx = 1;
	if( _sizePageMetrics.cy == 0 )
		_sizePageMetrics.cy = 1;

	return _sizePageMetrics;
}

bool CExtGridBaseWnd::OnSwDoScrollBy(
	CSize sizeScroll,
	bool bDoScroll // = true
	)
{
	ASSERT_VALID( this );
//	OnGbwHoverRemove( false );
	if(		( sizeScroll.cx != 0 || sizeScroll.cy != 0 )
		&&	m_wndContentExpand.GetSafeHwnd() != NULL
		)
	{
		m_wndContentExpand.Deactivate();
		m_htInfoExpand.Empty();
	}
CPoint _ptScrollPosBefore = OnSwGetScrollPos();
CSize _sizePageBefore = OnSwGetPageSize( 0 );
bool bRetVal =
		CExtScrollItemWnd::OnSwDoScrollBy(
			sizeScroll,
			bDoScroll
			);
	if( ! bRetVal )
		return false;
CSize _sizePageAfter = OnSwGetPageSize( 0 );
//	if(		( _sizePageBefore.cx != _sizePageAfter.cx && (!FixedSizeColumnsGet()) )
//		||	( _sizePageBefore.cy != _sizePageAfter.cy && (!FixedSizeRowsGet()) )
//		)
	if( _sizePageBefore != _sizePageAfter )
	{
		OnSwUpdateScrollBars();
		CPoint _ptScrollPosNeeded = _ptScrollPosBefore + sizeScroll;
		CPoint _ptScrollPosAfter = OnSwGetScrollPos();
		if( _ptScrollPosNeeded != _ptScrollPosAfter )
		{
			CSize sizeScrollPlus = _ptScrollPosNeeded - _ptScrollPosAfter;
			bRetVal =
				CExtScrollItemWnd::OnSwDoScrollBy(
					sizeScrollPlus,
					bDoScroll
					);
		}
	} // if( _sizePageBefore != _sizePageAfter )
//	OnGbwHoverRecalc();
DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	if(		dwScrollTypeH == __ESIW_ST_VIRTUAL
		||	dwScrollTypeV == __ESIW_ST_VIRTUAL
		)
	{
		m_bLockVirtualCacheUpdateH =
			(dwScrollTypeH == __ESIW_ST_VIRTUAL) ? true : false;
		m_bLockVirtualCacheUpdateV =
			(dwScrollTypeV == __ESIW_ST_VIRTUAL) ? true : false;
		OnSwUpdateScrollBars();
		m_bLockVirtualCacheUpdateH = false;
		m_bLockVirtualCacheUpdateV = false;
	}
	return bRetVal;
}

void CExtGridBaseWnd::OnGbwQueryVisibleColumnRange(
	LONG & nColFirst,
	LONG & nColLast,
	LONG & nColFirstOffset
	) const
{
	ASSERT_VALID( this );
	nColFirst = 0;
	nColLast = -1;
	nColFirstOffset = 0;
CRect rcClient = OnSwGetClientRect();
	nColFirstOffset = rcClient.left;
DWORD dwScrollTypeH = SiwScrollTypeHGet();
//	if( dwScrollTypeH == __ESIW_ST_NONE )
//	{
//		nColFirst = 0L;
//		nColLast = ColumnCountGet() - 1;
//		return;
//	}
//	if( dwScrollTypeH == __ESIW_ST_PIXEL )
	if(		dwScrollTypeH == __ESIW_ST_NONE
		||	dwScrollTypeH == __ESIW_ST_PIXEL
		)
	{
		nColFirst = 0;
		nColLast = -1;
		LONG nColumnCount = ColumnCountGet();
		if( nColumnCount == 0 )
			return;
		int nScrollPos = 0;
		if( OnSwHasScrollBar(true) )
			nScrollPos = ScrollPos32Get( SB_HORZ );
		int nRangeMin = nScrollPos;
		int nRangeMax = nScrollPos + OnSwGetClientRect().Width();
		if( nRangeMax <= nRangeMin )
			return;
		int nOffset = 0;
		nColFirst = -1;
		for( LONG nColNo = 0; nColNo < nColumnCount; nColNo++ )
		{
			INT nItemExtent = OnSiwQueryItemExtentH( nColNo );
			INT nOffsetNext = nOffset + nItemExtent;
			if(		( nRangeMin <= nOffset && nOffset <= nRangeMax )
				||	( nRangeMin <= nOffsetNext && nOffsetNext <= nRangeMax )
				||	( nOffset <= nRangeMin && nRangeMax <= nOffsetNext )
				)
			{
				if( nColFirst == -1 )
				{
					nColFirst = nColNo;
					nColFirstOffset = nOffset + rcClient.left - nScrollPos;
				}
				nColLast = nColNo;
			}
			else
				if( nColLast >= 0 )
					break;
			nOffset = nOffsetNext;
		}
		if( nColFirst < 0 )
		{
			nColFirst = 0;
			nColLast = -1;
		}
		return;
	}

CRect rcSiwVisibleRange = OnSiwGetVisibleRange();
	nColFirst = rcSiwVisibleRange.left;
	nColLast = rcSiwVisibleRange.right;
}

void CExtGridBaseWnd::OnGbwQueryVisibleRowRange(
	LONG & nRowFirst,
	LONG & nRowLast,
	LONG & nRowFirstOffset
	) const
{
	ASSERT_VALID( this );
	nRowFirst = 0;
	nRowLast = -1;
	nRowFirstOffset = 0;
CRect rcClient = OnSwGetClientRect();
	nRowFirstOffset = rcClient.top;
DWORD dwScrollTypeV = SiwScrollTypeVGet();
//	if( dwScrollTypeV == __ESIW_ST_NONE )
//	{
//		nRowFirst = 0L;
//		nRowLast = RowCountGet() - 1;
//		return;
//	}
//	if( dwScrollTypeV == __ESIW_ST_PIXEL )
	if(		dwScrollTypeV == __ESIW_ST_NONE
		||	dwScrollTypeV == __ESIW_ST_PIXEL
		)
	{
		nRowFirst = 0;
		nRowLast = -1;
		LONG nRowCount = RowCountGet();
		if( nRowCount == 0 )
			return;
		int nScrollPos = 0;
		if( OnSwHasScrollBar(false) )
			nScrollPos = ScrollPos32Get( SB_VERT );
		int nRangeMin = nScrollPos;
		int nRangeMax = nScrollPos + OnSwGetClientRect().Height();
		if( nRangeMax <= nRangeMin )
			return;
		int nOffset = 0;
		nRowFirst = -1;
		for( LONG nRowNo = 0; nRowNo < nRowCount; nRowNo++ )
		{
			INT nItemExtent = OnSiwQueryItemExtentV( nRowNo );
			INT nOffsetNext = nOffset + nItemExtent;
			if(		( nRangeMin <= nOffset && nOffset <= nRangeMax )
				||	( nRangeMin <= nOffsetNext && nOffsetNext <= nRangeMax )
				||	( nOffset <= nRangeMin && nRangeMax <= nOffsetNext )
				)
			{
				if( nRowFirst == -1 )
				{
					nRowFirst = nRowNo;
					nRowFirstOffset = nOffset + rcClient.top - nScrollPos;
				}
				nRowLast = nRowNo;
			}
			else
				if( nRowLast >= 0 )
					break;
			nOffset = nOffsetNext;
		}
		if( nRowFirst < 0 )
		{
			nRowFirst = 0;
			nRowLast = -1;
		}
		return;
	}

CRect rcSiwVisibleRange = OnSiwGetVisibleRange();
	nRowFirst = rcSiwVisibleRange.top;
	nRowLast = rcSiwVisibleRange.bottom;
}

LONG CExtGridBaseWnd::OnGbwQueryPixelColumnOffset(
	LONG nColNo
	)
{
	ASSERT_VALID( this );
	if( nColNo < 0 )
		return 0L;
LONG nColCount = ColumnCountGet();
	if( nColNo > nColCount )
		return 0L;
	if( FixedSizeColumnsGet() )
	{
		CSize sizeItem = OnSiwCalcItemSize();
		return sizeItem.cx * nColNo;
	} // if( FixedSizeColsGet() )
LONG nOffset = 0L;
	for( LONG nColIdx = 0; nColIdx < nColNo; nColIdx++ )
	{
		INT nItemExtent = OnSiwQueryItemExtentH( nColIdx );
		nOffset += nItemExtent;
	}
	return nOffset;
}

LONG CExtGridBaseWnd::OnGbwQueryPixelRowOffset(
	LONG nRowNo
	)
{
	ASSERT_VALID( this );
	if( nRowNo < 0 )
		return 0L;
LONG nRowCount = RowCountGet();
	if( nRowNo > nRowCount )
		return 0L;
	if( FixedSizeRowsGet() )
	{
		CSize sizeItem = OnSiwCalcItemSize();
		return sizeItem.cy * nRowNo;
	} // if( FixedSizeRowsGet() )
LONG nOffset = 0L;
	for( LONG nRowIdx = 0; nRowIdx < nRowNo; nRowIdx++ )
	{
		INT nItemExtent = OnSiwQueryItemExtentV( nRowIdx );
		nOffset += nItemExtent;
	}
	return nOffset;
}

bool CExtGridBaseWnd::EnsureVisibleColumn(
	LONG nColNo,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( nColNo < 0 )
	{
		ASSERT( FALSE );
		return false;
	}
DWORD dwScrollTypeH = SiwScrollTypeHGet();
	if(		dwScrollTypeH == __ESIW_ST_NONE
		||	dwScrollTypeH == __ESIW_ST_VIRTUAL
		)
		return false;
LONG nColCount = ColumnCountGet();
	if( nColCount <= nColNo )
		return false;
	if( !OnSwHasScrollBar(true) )
	{
		if( bRedraw )
			OnSwDoRedraw();
		return true;
	}
int nScrollPos = ScrollPos32Get( SB_HORZ, false );
int nScrollLimit = ScrollLimit32Get( SB_HORZ );
	if( nScrollPos > nScrollLimit )
	{
		if( bRedraw )
			OnSwDoRedraw();
		return true;
	}
LONG nColFirst = 0, nColLast = 0, nColFirstOffset = 0;
	OnGbwQueryVisibleColumnRange(
		nColFirst,
		nColLast,
		nColFirstOffset
		);
	ASSERT( nColFirst <= nColLast );
LONG nScrollByOffset = 0L;
bool bFixedSizeColumns = FixedSizeColumnsGet();
	if(		nColNo < nColFirst
		||	( nColNo <= nColFirst && dwScrollTypeH == __ESIW_ST_PIXEL )
		)
	{
		if( dwScrollTypeH == __ESIW_ST_PIXEL )
		{
			if( bFixedSizeColumns )
			{
				CSize sizeItem = OnSiwCalcItemSize();
				LONG nTargetScrollPos =
					nColNo * sizeItem.cx;
				nScrollByOffset = nTargetScrollPos - nScrollPos;
			} // if( bFixedSizeColumns )
			else
			{
				LONG nTargetScrollPos =
					OnGbwQueryPixelColumnOffset( nColNo );
				nScrollByOffset = nTargetScrollPos - nScrollPos;
			} // else from if( bFixedSizeColumns )
		} // if( dwScrollTypeH == __ESIW_ST_PIXEL )
		else
		{
			ASSERT( dwScrollTypeH == __ESIW_ST_ITEM );
			if( bFixedSizeColumns )
			{
				nScrollByOffset = nColNo - nScrollPos;
			} // if( bFixedSizeColumns )
			else
			{
				ASSERT( nColNo < nColFirst );
				nScrollByOffset = nColNo - nScrollPos;
			} // else from if( bFixedSizeColumns )
		} // else from if( dwScrollTypeH == __ESIW_ST_PIXEL )
	} // if( nColNo < nColFirst ...
	else if( nColNo >= nColLast )
	{
		if( dwScrollTypeH == __ESIW_ST_PIXEL )
		{
			if( bFixedSizeColumns )
			{
				CSize sizeItem = OnSiwCalcItemSize();
				CRect rcClient = OnSwGetClientRect();
				int nClientWidth = rcClient.Width();
				if( nClientWidth <= sizeItem.cx )
				{
					LONG nTargetScrollPos =
						nColNo * sizeItem.cx;
					nScrollByOffset = nTargetScrollPos - nScrollPos;
				} // if( nClientWidth <= sizeItem.cx )
				else
				{
					LONG nTargetScrollPos =
						(	nColNo
						-	(nClientWidth / sizeItem.cx)
						+	( (nClientWidth % sizeItem.cx) == 0 ? 0 : 1 )
						)
						* sizeItem.cx;
					nScrollByOffset = nTargetScrollPos - nScrollPos;
				} // else from if( nClientWidth <= sizeItem.cx )
			} // if( bFixedSizeColumns )
			else
			{
				LONG nTargetScrollPos =
					OnGbwQueryPixelColumnOffset( nColNo + 1 );
				CRect rcClient = OnSwGetClientRect();
				int nClientWidth = rcClient.Width();
				nScrollByOffset =
					nTargetScrollPos
					- nScrollPos
					- nClientWidth
					;
			} // else from if( bFixedSizeColumns )
		} // if( dwScrollTypeH == __ESIW_ST_PIXEL )
		else
		{
			ASSERT( dwScrollTypeH == __ESIW_ST_ITEM );
			if( bFixedSizeColumns )
			{
				CSize sizeItem = OnSiwCalcItemSize();
				CRect rcClient = OnSwGetClientRect();
				int nClientWidth = rcClient.Width();
				if( nClientWidth <= sizeItem.cx )
				{
					nScrollByOffset = nColNo - nScrollPos;
				} // if( nClientWidth <= sizeItem.cx )
				else
				{
					LONG nTargetScrollPos =
						(	nColNo
						-	(nClientWidth / sizeItem.cx)
						+	( (nClientWidth % sizeItem.cx) == 0 ? 0 : 1 )
						);
					nScrollByOffset = nTargetScrollPos - nScrollPos;
				} // else from if( nClientWidth <= sizeItem.cx )
			} // if( bFixedSizeColumns )
			else
			{
				CRect rcClient = OnSwGetClientRect();
				int nClientWidth = rcClient.Width();
				LONG nVisibleExtent = 0L;
				for( LONG nColIdx = nColNo+1; nColIdx > 0; nColIdx-- )
				{
					INT nItemExtent = OnSiwQueryItemExtentH( nColIdx-1 );
					nVisibleExtent += nItemExtent;
					if( nVisibleExtent >= nClientWidth )
						break;
				}
				nScrollByOffset = nColIdx - nScrollPos;
			} // else from if( bFixedSizeColumns )
		} // else from if( dwScrollTypeH == __ESIW_ST_PIXEL )
	} // else if( nColNo >= nColLast )
bool bRetVal = true;
	if( nScrollByOffset != 0 )
		bRetVal = OnSwDoScrollBy( CSize(nScrollByOffset,0), bRedraw );
	else
	{
		if( bRedraw )
			OnSwDoRedraw();
		return bRetVal;
	} // else from if( nScrollByOffset != 0 )
	return bRetVal;
}

bool CExtGridBaseWnd::EnsureVisibleRow(
	LONG nRowNo,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( nRowNo < 0 )
	{
		ASSERT( FALSE );
		return false;
	}
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	if(		dwScrollTypeV == __ESIW_ST_NONE
		||	dwScrollTypeV == __ESIW_ST_VIRTUAL
		)
		return false;
LONG nRowCount = RowCountGet();
	if( nRowCount <= nRowNo )
		return false;
	if( !OnSwHasScrollBar(false) )
	{
		if( bRedraw )
			OnSwDoRedraw();
		return true;
	}
int nScrollPos = ScrollPos32Get( SB_VERT, false );
int nScrollLimit = ScrollLimit32Get( SB_VERT );
	if( nScrollPos > nScrollLimit )
	{
		if( bRedraw )
			OnSwDoRedraw();
		return true;
	}
LONG nRowFirst = 0, nRowLast = 0, nRowFirstOffset = 0;
	OnGbwQueryVisibleRowRange(
		nRowFirst,
		nRowLast,
		nRowFirstOffset
		);
	ASSERT( nRowFirst <= nRowLast );
LONG nScrollByOffset = 0L;
bool bFixedSizeRows = FixedSizeRowsGet();
	if(		nRowNo < nRowFirst
		||	( nRowNo <= nRowFirst && dwScrollTypeV == __ESIW_ST_PIXEL )
		)
	{
		if( dwScrollTypeV == __ESIW_ST_PIXEL )
		{
			if( bFixedSizeRows )
			{
				CSize sizeItem = OnSiwCalcItemSize();
				LONG nTargetScrollPos =
					nRowNo * sizeItem.cy;
				nScrollByOffset = nTargetScrollPos - nScrollPos;
			} // if( bFixedSizeRows )
			else
			{
				LONG nTargetScrollPos =
					OnGbwQueryPixelRowOffset( nRowNo );
				nScrollByOffset = nTargetScrollPos - nScrollPos;
			} // else from if( bFixedSizeRows )
		} // if( dwScrollTypeV == __ESIW_ST_PIXEL )
		else
		{
			ASSERT( dwScrollTypeV == __ESIW_ST_ITEM );
			if( bFixedSizeRows )
			{
				nScrollByOffset = nRowNo - nScrollPos;
			} // if( bFixedSizeRows )
			else
			{
				ASSERT( nRowNo < nRowFirst );
				nScrollByOffset = nRowNo - nScrollPos;
			} // else from if( bFixedSizeRows )
		} // else from if( dwScrollTypeV == __ESIW_ST_PIXEL )
	} // if( nRowNo < nRowFirst ...
	else if( nRowNo >= nRowLast )
	{
		if( dwScrollTypeV == __ESIW_ST_PIXEL )
		{
			if( bFixedSizeRows )
			{
				CSize sizeItem = OnSiwCalcItemSize();
				CRect rcClient = OnSwGetClientRect();
				int nClientHeight = rcClient.Height();
				if( nClientHeight <= sizeItem.cy )
				{
					LONG nTargetScrollPos =
						nRowNo * sizeItem.cy;
					nScrollByOffset = nTargetScrollPos - nScrollPos + 1;
				} // if( nClientHeight <= sizeItem.cy )
				else
				{
					LONG nTargetScrollPos =
						(	nRowNo
						-	(nClientHeight / sizeItem.cy)
						+	( (nClientHeight % sizeItem.cy) == 0 ? 0 : 1 )
						)
						* sizeItem.cy;
					nScrollByOffset = nTargetScrollPos - nScrollPos + 1;
				} // else from if( nClientHeight <= sizeItem.cy )
			} // if( bFixedSizeRows )
			else
			{
				LONG nTargetScrollPos =
					OnGbwQueryPixelRowOffset( nRowNo + 1 );
				CRect rcClient = OnSwGetClientRect();
				int nClientHeight = rcClient.Height();
				nScrollByOffset =
					nTargetScrollPos
					- nScrollPos
					- nClientHeight
					;
			} // else from if( bFixedSizeRows )
		} // if( dwScrollTypeV == __ESIW_ST_PIXEL )
		else
		{
			ASSERT( dwScrollTypeV == __ESIW_ST_ITEM );
			if( bFixedSizeRows )
			{
				CSize sizeItem = OnSiwCalcItemSize();
				CRect rcClient = OnSwGetClientRect();
				int nClientHeight = rcClient.Height();
				if( nClientHeight <= sizeItem.cy )
				{
					nScrollByOffset = nRowNo - nScrollPos + 1;
				} // if( nClientHeight <= sizeItem.cy )
				else
				{
					LONG nTargetScrollPos =
						(	nRowNo
						-	(nClientHeight / sizeItem.cy)
						+	( (nClientHeight % sizeItem.cy) == 0 ? 0 : 1 )
						);
					nScrollByOffset = nTargetScrollPos - nScrollPos + 1;
				} // else from if( nClientHeight <= sizeItem.cy )
			} // if( bFixedSizeRows )
			else
			{
				CRect rcClient = OnSwGetClientRect();
				int nClientHeight = rcClient.Height();
				LONG nVisibleExtent = 0L;
				for( LONG nRowIdx = nRowNo+1; nRowIdx > 0; nRowIdx-- )
				{
					INT nItemExtent = OnSiwQueryItemExtentV( nRowIdx-1 );
					nVisibleExtent += nItemExtent;
					if( nVisibleExtent >= nClientHeight )
						break;
				}
				nScrollByOffset = nRowIdx - nScrollPos;
			} // else from if( bFixedSizeRows )
		} // else from if( dwScrollTypeV == __ESIW_ST_PIXEL )
	} // else if( nRowNo >= nRowLast )
bool bRetVal = true;
	if( nScrollByOffset != 0 )
		bRetVal = OnSwDoScrollBy( CSize(0,nScrollByOffset), bRedraw );
	else
	{
		if( bRedraw )
			OnSwDoRedraw();
		return bRetVal;
	} // else from if( nScrollByOffset != 0 )
	return bRetVal;
}

INT CExtGridBaseWnd::SelectionGetHitCount(
	LONG nColNo,
	LONG nRowNo
	) const
{
	ASSERT_VALID( this );
	if( nColNo < 0L && nRowNo < 0L )
		return 0;
INT nAreaCount = m_arrSelAreas.GetSize();
	if( nAreaCount == 0 )
		return 0;
INT nHitCount = 0;
	for( int nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
	{
		CRect rcArea = m_arrSelAreas[ nAreaNumber ];
		rcArea.NormalizeRect();
		if( nColNo >= 0 )
		{
			if( nRowNo >= 0 )
			{
				if(		rcArea.left <= nColNo && nColNo <= rcArea.right
					&&	rcArea.top <= nRowNo && nRowNo <= rcArea.bottom
					)
						nHitCount ++;
			} // if( nRowNo >= 0 )
			else
			{
				if(		rcArea.left <= nColNo && nColNo <= rcArea.right )
						nHitCount ++;
			} // else from if( nRowNo >= 0 )
		} // if( nColNo >= 0 )
		else
		{
				ASSERT( nRowNo >= 0 );
				if(		rcArea.top <= nRowNo && nRowNo <= rcArea.bottom )
						nHitCount ++;
		} // else from if( nColNo >= 0 )
	} // for( int nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
	return nHitCount;
}

bool CExtGridBaseWnd::SelectionGetForCell(
	LONG nColNo,
	LONG nRowNo
	) const
{
	ASSERT_VALID( this );
	if( nColNo < 0L && nRowNo < 0L )
		return false;
INT nAreaCount = m_arrSelAreas.GetSize();
	if( nAreaCount == 0 )
		return false;
bool bSubtractSelectionAreas = SubtractSelectionAreasGet();
	if( bSubtractSelectionAreas && ( nColNo < 0L || nRowNo < 0L ) )
	{
		for( int nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
		{
			CRect rcArea = m_arrSelAreas[ nAreaNumber ];
			rcArea.NormalizeRect();
			if( nColNo < 0L )
			{
				ASSERT( nRowNo >= 0L );
				if( rcArea.top <= nRowNo && nRowNo <= rcArea.bottom )
				{
					for( LONG nIdx = rcArea.left; nIdx <= rcArea.right; nIdx++ )
					{
						if( SelectionGetForCell( nIdx, nRowNo ) )
							return true;
					} // for( LONG nIdx = rcArea.left; nIdx <= rcArea.right; nIdx++ )
				} // if( rcArea.top <= nRowNo && nRowNo <= rcArea.bottom )
			} // if( nColNo < 0L )
			else
			{
				ASSERT( nRowNo < 0L );
				if( rcArea.left <= nColNo && nColNo <= rcArea.right )
				{
					for( LONG nIdx = rcArea.top; nIdx <= rcArea.bottom; nIdx++ )
					{
						if( SelectionGetForCell( nColNo, nIdx ) )
							return true;
					} // for( LONG nIdx = rcArea.top; nIdx <= rcArea.bottom; nIdx++ )
				} // if( rcArea.left <= nColNo && nColNo <= rcArea.right )
			} // else from if( nColNo < 0L );
		} // for( int nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
		return false;
	} // if( bSubtractSelectionAreas && ( nColNo < 0L || nRowNo < 0L ) )
DWORD dwCounter = 0;
	for( int nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
	{
		CRect rcArea = m_arrSelAreas[ nAreaNumber ];
		rcArea.NormalizeRect();
		if(		(	nColNo < 0L
				||	(	rcArea.left <= nColNo
					&&	nColNo <= rcArea.right
					)
				)
			&&	(	nRowNo < 0L
				||	(	rcArea.top <= nRowNo
					&&	nRowNo <= rcArea.bottom
					)
				)
			)
		{
			if( !bSubtractSelectionAreas )
				return true;
			dwCounter++;
		}
	} // for( int nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
	if( !bSubtractSelectionAreas )
		return false;
	if( (dwCounter & 0x01L) != 0L )
		return true;
	return false;
}

INT CExtGridBaseWnd::SelectionGetAreaCount() const
{
	ASSERT_VALID( this );
INT nAreaCount = m_arrSelAreas.GetSize();
	return nAreaCount;
}

CRect CExtGridBaseWnd::SelectionGet(
	bool bNormalized,
	INT nAreaNumber // -1 - get normalized union of all areas
	) const
{
	ASSERT_VALID( this );
INT nAreaCount = m_arrSelAreas.GetSize();
	if( nAreaCount == 0 )
		return CRect( -1, -1, -1, -1 );
	if( nAreaNumber < 0 )
	{
		CRect rcUnion( 0, 0, 0, 0 );
		for( nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
		{
			CRect rcArea = m_arrSelAreas[ nAreaNumber ];
			rcArea.NormalizeRect();
			if( nAreaNumber == 0 )
			{
				rcUnion = rcArea;
				continue;
			} // if( nAreaNumber == 0 )
			rcUnion.left = min( rcUnion.left, rcArea.left );
			rcUnion.top = min( rcUnion.top, rcArea.top );
			rcUnion.right = max( rcUnion.right, rcArea.right );
			rcUnion.bottom = max( rcUnion.bottom, rcArea.bottom );
		} // for( nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
		return rcUnion;
	} // if( nAreaNumber < 0 )
	if( nAreaNumber >= nAreaCount )
		return CRect( -1, -1, -1, -1 );
CRect rcArea = m_arrSelAreas[ nAreaNumber ];
	if( bNormalized )
		rcArea.NormalizeRect();
	return rcArea;
}

void CExtGridBaseWnd::_AddToSelArrayImpl( CRect & rcApplyNew )
{
	ASSERT_VALID( this );
bool bSubtractSelectionAreas = SubtractSelectionAreasGet();
	if( bSubtractSelectionAreas )
	{
		m_arrSelAreas.Add( rcApplyNew );
		return;
	} // if( bSubtractSelectionAreas )
INT nAreaCount = m_arrSelAreas.GetSize();
	for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; )
	{
		CRect & rcRegisteredArea = m_arrSelAreas.ElementAt( nAreaNumber );
		if(		rcRegisteredArea.left == rcApplyNew.left
			&&	rcRegisteredArea.right == rcApplyNew.right
			&&	rcRegisteredArea.top == rcApplyNew.top
			&&	rcRegisteredArea.bottom == rcApplyNew.bottom
			)
		{
			m_arrSelAreas.RemoveAt( nAreaNumber );
			nAreaCount --;
		}
		else
			nAreaNumber ++;
	} // for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; )
	m_arrSelAreas.Add( rcApplyNew );
}

bool CExtGridBaseWnd::SelectionRemoveAt(
	INT nAreaNumber,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( nAreaNumber < 0 )
	{
		ASSERT( FALSE );
		return false;
	}
INT nAreaCount = SelectionGetAreaCount();
	if( nAreaNumber >= nAreaCount )
	{
		ASSERT( FALSE );
		return false;
	}
	if( ! OnGbwSelectionCanModify() )
		return false;
	m_arrSelAreas.RemoveAt( nAreaNumber, 1 );
	OnGbwSelectionChanged();
	if( bRedraw )
		OnSwDoRedraw();
	return true;
}

bool CExtGridBaseWnd::SelectionSetAt(
	INT nAreaNumber,
	const RECT & rcNewSelection,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( nAreaNumber < 0 )
	{
		ASSERT( FALSE );
		return false;
	}
INT nAreaCount = SelectionGetAreaCount();
	if( nAreaNumber >= nAreaCount )
	{
		ASSERT( FALSE );
		return false;
	}
	if( ! OnGbwSelectionCanModify() )
		return false;
CRect rcApplyNew( rcNewSelection );
	m_arrSelAreas.SetAt( nAreaNumber, rcApplyNew );
	OnGbwSelectionChanged();
	if( bRedraw )
		OnSwDoRedraw();
	return true;
}

bool CExtGridBaseWnd::SelectionInsertAt(
	INT nAreaNumber,
	const RECT & rcNewSelection,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( ! OnGbwSelectionCanModify() )
		return false;
INT nAreaCount = SelectionGetAreaCount();
	if( nAreaNumber < 0 || nAreaNumber > nAreaCount )
		nAreaNumber = nAreaCount;
CRect rcApplyNew( rcNewSelection );
	m_arrSelAreas.InsertAt( nAreaNumber, rcApplyNew, 1 );
	OnGbwSelectionChanged();
	if( bRedraw )
		OnSwDoRedraw();
	return true;
}

bool CExtGridBaseWnd::SelectionSet(
	const RECT & rcNewSelection,
	bool bReplaceOldAreas, // = true
	bool bReplaceLastArea, // = false
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( ! OnGbwSelectionCanModify() )
		return false;
	if( ! MultiAreaSelectionGet() )
		bReplaceOldAreas = true;
	if( bReplaceOldAreas )
	{
		m_arrSelAreas.RemoveAll();
		bReplaceOldAreas = false;
	}
INT nAreaCount = m_arrSelAreas.GetSize();
CRect rcApplyNew( rcNewSelection );
	if( bReplaceOldAreas )
	{
		ASSERT( nAreaCount == 0 );
		_AddToSelArrayImpl( rcApplyNew );
	} // if( bReplaceOldAreas )
	else
	{
		if( bReplaceLastArea )
		{
			if( nAreaCount == 0 )
				_AddToSelArrayImpl( rcApplyNew );
			else
				m_arrSelAreas.SetAt( nAreaCount - 1, rcApplyNew );
		} // if( bReplaceLastArea )
		else
			_AddToSelArrayImpl( rcApplyNew );
	} // else from if( bReplaceOldAreas )
CPoint ptFocus = FocusGet();
	if( AutoFocusBottomRightGet() )
	{
		ptFocus.x = rcNewSelection.right;
		ptFocus.y = rcNewSelection.bottom;
		FocusSet( ptFocus, bRedraw, bRedraw, false, bRedraw );
	} // AutoFocusBottomRightGet()
	if( ptFocus.x >= 0 )
		EnsureVisibleColumn( ptFocus.x, false );
	if( m_ptFocusCell.y >= 0 )
		EnsureVisibleRow( ptFocus.y, false );
	OnGbwSelectionChanged();
	if( bRedraw )
		OnSwDoRedraw();
	return true;
}

bool CExtGridBaseWnd::SelectionSet(
	const POINT & ptNewSelection,
	bool bReplaceOldAreas, // = true
	bool bReplaceLastArea, // = false
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
CRect rcNewSelection(
		ptNewSelection.x, ptNewSelection.y,
		ptNewSelection.x, ptNewSelection.y
		);
	DWORD dwBasicSelectionType = SiwGetStyle() & __EGBS_SFB_MASK;
	if( dwBasicSelectionType == __EGBS_SFB_NONE )
	{
		if( bRedraw )
			OnSwDoRedraw();
		return false;
	} // if( dwBasicSelectionType == __EGBS_SFB_NONE )
DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	if(		dwScrollTypeH != __ESIW_ST_VIRTUAL
		&&	dwScrollTypeV != __ESIW_ST_VIRTUAL
		)
	{
		if( dwBasicSelectionType == __EGBS_SFB_FULL_COLUMNS )
		{
			LONG nRowCount = RowCountGet();
			if( nRowCount == 0L )
			{
				if( bRedraw )
					OnSwDoRedraw();
				return false;
			} // if( nRowCount == 0L )
			ASSERT( nRowCount > 0L );
			rcNewSelection.top = 0L;
			rcNewSelection.bottom = nRowCount - 1L;
		} // if( dwBasicSelectionType == __EGBS_SFB_FULL_COLUMNS )
		else if( dwBasicSelectionType == __EGBS_SFB_FULL_ROWS )
		{
			LONG nColCount = ColumnCountGet();
			if( nColCount == 0L )
			{
				if( bRedraw )
					OnSwDoRedraw();
				return false;
			} // if( nColCount == 0L )
			ASSERT( nColCount > 0L );
			rcNewSelection.left = 0L;
			rcNewSelection.right = nColCount - 1L;
		} // else if( dwBasicSelectionType == __EGBS_SFB_FULL_ROWS )
	}
	return
		SelectionSet(
			rcNewSelection,
			bReplaceOldAreas,
			bReplaceLastArea,
			bRedraw
			);
}

bool CExtGridBaseWnd::SelectionIsEmpty() const
{
	ASSERT_VALID( this );
	return ( SelectionGetAreaCount() == 0) ? true : false;
}

bool CExtGridBaseWnd::SelectionUnset(
	bool bSetToFocus, // = true
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( ! OnGbwSelectionCanModify() )
		return false;
	m_arrSelAreas.RemoveAll();
bool bRetVal = true;
	if( bSetToFocus )
	{
		CPoint ptFocus = FocusGet();
		bRetVal = SelectionSet( ptFocus, true, false, false );
	} // if( bSetToFocus )
	else
		OnGbwSelectionChanged();
	if( bRedraw )
		OnSwDoRedraw();
	return bRetVal;
}

bool CExtGridBaseWnd::OnGbwSelectionCanModify()
{
	ASSERT_VALID( this );
	return true;
}

void CExtGridBaseWnd::OnGbwSelectionChanged()
{
	ASSERT_VALID( this );
}

LONG CExtGridBaseWnd::SelectionGetForCellNextPrev(
	bool bSearchHorz,
	bool bSearchNext,
	LONG nColNo,
	LONG nRowNo
	) const
{
	ASSERT_VALID( this );
LONG nColCount = ColumnCountGet();
	if( nColCount < 0L )
		return -1L;
LONG nRowCount = RowCountGet();
	if( nRowCount < 0L )
		return -1L;
LONG nCrossIdx = bSearchHorz ? nRowNo : nColNo;
	if( nCrossIdx < 0L )
	{
		ASSERT( FALSE );
		return -1L; // invalid arg
	}
LONG nCrossAfterEndIdx = bSearchHorz ? nRowCount : nColCount;
	if( nCrossIdx >= nCrossAfterEndIdx )
	{
		ASSERT( FALSE );
		return -1L; // invalid arg
	}
LONG nWalkStartIdx = bSearchHorz ? nColNo : nRowNo;
LONG nWalkStartAfterEndIdx = bSearchHorz ? nColCount : nRowCount;
	if(		nWalkStartIdx >= nWalkStartAfterEndIdx
		||	nWalkStartAfterEndIdx == 0L
		)
		return -1L; // end of columns/rows reached
LONG nWalkShift = bSearchNext ? (1L) : (-1L);
LONG nWalkLastIdx = bSearchNext ? (nWalkStartAfterEndIdx - 1L) : 0L;
	if( nWalkStartIdx < 0L )
		nWalkStartIdx = bSearchNext ? 0L : (nWalkStartAfterEndIdx - 1L);
	else
	{
		if( nWalkShift < 0L && nWalkStartIdx == 0L )
			return -1L; // end of columns/rows reached
		nWalkStartIdx += nWalkShift;
		if( nWalkShift > 0L && nWalkStartIdx >= nWalkStartAfterEndIdx )
			return -1L; // end of columns/rows reached
	}
	for( LONG nWalkIdx = nWalkStartIdx; true; nWalkIdx += nWalkShift )
	{
		ASSERT( 0 <= nWalkIdx && nWalkIdx < nWalkStartAfterEndIdx );
		if(	SelectionGetForCell(
				bSearchHorz ? nWalkIdx : nColNo,
				bSearchHorz ? nRowNo : nWalkIdx
				)
			)
			return nWalkIdx;
		if( nWalkIdx == nWalkLastIdx )
			break;
	} // for( LONG nWalkIdx = nWalkStartIdx; true; nWalkIdx += nWalkShift )
	return -1L; // end of columns/rows reached
}

LONG CExtGridBaseWnd::SelectionGetNextRowInColumn(
	LONG nColNo,
	LONG nRowNo
	) const
{
	ASSERT_VALID( this );
LONG nRetVal =
		SelectionGetForCellNextPrev(
			false, // bSearchHorz
			true, // bSearchNext
			nColNo,
			nRowNo
			);
	return nRetVal;
}

LONG CExtGridBaseWnd::SelectionGetPrevRowInColumn(
	LONG nColNo,
	LONG nRowNo
	) const
{
	ASSERT_VALID( this );
LONG nRetVal =
		SelectionGetForCellNextPrev(
			false, // bSearchHorz
			false, // bSearchNext
			nColNo,
			nRowNo
			);
	return nRetVal;
}

LONG CExtGridBaseWnd::SelectionGetNextColumnInRow(
	LONG nColNo,
	LONG nRowNo
	) const
{
	ASSERT_VALID( this );
LONG nRetVal =
		SelectionGetForCellNextPrev(
			true, // bSearchHorz
			true, // bSearchNext
			nColNo,
			nRowNo
			);
	return nRetVal;
}

LONG CExtGridBaseWnd::SelectionGetPrevColumnInRow(
	LONG nColNo,
	LONG nRowNo
	) const
{
	ASSERT_VALID( this );
LONG nRetVal =
		SelectionGetForCellNextPrev(
			true, // bSearchHorz
			false, // bSearchNext
			nColNo,
			nRowNo
			);
	return nRetVal;
}

LONG CExtGridBaseWnd::SelectionGetFirstRowInColumn(
	LONG nColNo
	) const
{
	ASSERT_VALID( this );
LONG nRetVal =
		SelectionGetNextRowInColumn(
			nColNo,
			-1L
			);
	return nRetVal;
}

LONG CExtGridBaseWnd::SelectionGetLastRowInColumn(
	LONG nColNo
	) const
{
	ASSERT_VALID( this );
LONG nRetVal =
		SelectionGetPrevRowInColumn(
			nColNo,
			-1L
			);
	return nRetVal;
}

LONG CExtGridBaseWnd::SelectionGetFirstColumnInRow(
	LONG nRowNo
	) const
{
	ASSERT_VALID( this );
LONG nRetVal =
		SelectionGetNextColumnInRow(
			-1L,
			nRowNo
			);
	return nRetVal;
}

LONG CExtGridBaseWnd::SelectionGetLastColumnInRow(
	LONG nRowNo
	) const
{
	ASSERT_VALID( this );
LONG nRetVal =
		SelectionGetPrevColumnInRow(
			-1L,
			nRowNo
			);
	return nRetVal;
}

CPoint CExtGridBaseWnd::FocusGet() const
{
	ASSERT_VALID( this );
	return m_ptFocusCell;
}

CPoint CExtGridBaseWnd::FocusSet(
	const POINT & ptNewFocus,
	bool bEnsureVisibleColumn, // = true
	bool bEnsureVisibleRow, // = true
	bool bResetSelectionToFocus, // = true
	bool bRedraw, // = true
	bool * p_bCanceled // = NULL
	)
{
	ASSERT_VALID( this );
CPoint ptOldFocus( FocusGet() );
	if( ! OnGbwFocusChanging(
			ptOldFocus,
			ptNewFocus,
			bEnsureVisibleColumn,
			bEnsureVisibleRow,
			bResetSelectionToFocus,
			bRedraw
			)
		)
	{
		if( p_bCanceled != NULL )
			(*p_bCanceled) = true;
		return ptOldFocus;
	}
	if( p_bCanceled != NULL )
		(*p_bCanceled) = false;
	m_ptFocusCell = ptNewFocus;
	if( bResetSelectionToFocus )
		SelectionSet( ptNewFocus, true, false, false );
	if( bEnsureVisibleColumn && ptNewFocus.x >= 0 )
		EnsureVisibleColumn( ptNewFocus.x, bRedraw );
	if( bEnsureVisibleRow && ptNewFocus.y >= 0 )
		EnsureVisibleRow( ptNewFocus.y, bRedraw );
	OnGbwFocusChanged(
		ptOldFocus,
		ptNewFocus
		);
	return ptOldFocus;
}

CPoint CExtGridBaseWnd::FocusUnset(
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
CPoint ptEmptyFocus( -1, -1 );
	return
		FocusSet(
			ptEmptyFocus,
			false,
			false,
			false,
			bRedraw
			);
}

bool CExtGridBaseWnd::OnGbwFocusChanging(
	const POINT & ptOldFocus,
	const POINT & ptNewFocus,
	bool & bEnsureVisibleColumn,
	bool & bEnsureVisibleRow,
	bool & bResetSelectionToFocus,
	bool & bRedraw
	)
{
	ASSERT_VALID( this );
	ptOldFocus;
	ptNewFocus;
	bEnsureVisibleColumn;
	bEnsureVisibleRow;
	bResetSelectionToFocus;
	bRedraw;
	return true;
}

void CExtGridBaseWnd::OnGbwFocusChanged(
	const POINT & ptOldFocus,
	const POINT & ptNewFocus
	)
{
	ASSERT_VALID( this );
	ptOldFocus;
	ptNewFocus;
}

bool CExtGridBaseWnd::AutoFocusBottomRightGet() const
{
	ASSERT_VALID( this );
bool bAutoFocusBottomRight = ( SiwGetStyle() & __EGBS_AUTO_FOCUS_BOTTOM_RIGHT ) ? true : false;
	return bAutoFocusBottomRight;
}

bool CExtGridBaseWnd::AutoFocusBottomRightSet(
	bool bAutoFocusBottomRight
	)
{
	ASSERT_VALID( this );
bool bAutoFocusBottomRightOld = AutoFocusBottomRightGet();
	if(		(bAutoFocusBottomRightOld && bAutoFocusBottomRight)
		||	((!bAutoFocusBottomRightOld) && (!bAutoFocusBottomRight))
		)
		return bAutoFocusBottomRightOld;
	SiwModifyStyle(
		bAutoFocusBottomRight ? __EGBS_AUTO_FOCUS_BOTTOM_RIGHT : 0L,
		__EGBS_AUTO_FOCUS_BOTTOM_RIGHT,
		false
		);
	return bAutoFocusBottomRightOld;
}

bool CExtGridBaseWnd::MultiAreaSelectionGet() const
{
	ASSERT_VALID( this );
bool bMultiAreaSelection = ( SiwGetStyle() & __EGBS_MULTI_AREA_SELECTION ) ? true : false;
	return bMultiAreaSelection;
}

bool CExtGridBaseWnd::MultiAreaSelectionSet(
	bool bMultiAreaSelection
	)
{
	ASSERT_VALID( this );
bool bMultiAreaSelectionOld = MultiAreaSelectionGet();
	if(		(bMultiAreaSelectionOld && bMultiAreaSelection)
		||	((!bMultiAreaSelectionOld) && (!bMultiAreaSelection))
		)
		return bMultiAreaSelectionOld;
	SiwModifyStyle(
		bMultiAreaSelection ? __EGBS_MULTI_AREA_SELECTION : 0L,
		__EGBS_MULTI_AREA_SELECTION,
		false
		);
	return bMultiAreaSelectionOld;
}

bool CExtGridBaseWnd::NoHideSelectionGet() const
{
	ASSERT_VALID( this );
bool bNoHideSelection = ( SiwGetStyle() & __EGBS_NO_HIDE_SELECTION ) ? true : false;
	return bNoHideSelection;
}

bool CExtGridBaseWnd::NoHideSelectionSet(
	bool bNoHideSelection,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
bool bNoHideSelectionOld = NoHideSelectionGet();
	if(		(bNoHideSelectionOld && bNoHideSelection)
		||	((!bNoHideSelectionOld) && (!bNoHideSelection))
		)
		return bNoHideSelectionOld;
	SiwModifyStyle(
		bNoHideSelection ? __EGBS_NO_HIDE_SELECTION : 0L,
		__EGBS_NO_HIDE_SELECTION,
		false
		);
	if( bRedraw )
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	} // if( bRedraw )
	return bNoHideSelectionOld;
}

bool CExtGridBaseWnd::GridLinesHorzGet() const
{
	ASSERT_VALID( this );
bool bGridLines = ( SiwGetStyle() & __EGBS_GRIDLINES_H ) ? true : false;
	return bGridLines;
}

bool CExtGridBaseWnd::GridLinesHorzSet(
	bool bGridLines,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
bool bGridLinesOld = GridLinesHorzGet();
	if(		(bGridLinesOld && bGridLines)
		||	((!bGridLinesOld) && (!bGridLines))
		)
		return bGridLinesOld;
	SiwModifyStyle(
		bGridLines ? __EGBS_GRIDLINES_H : 0L,
		__EGBS_GRIDLINES_H,
		false
		);
	if( bRedraw )
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	} // if( bRedraw )
	return bGridLinesOld;
}

bool CExtGridBaseWnd::GridLinesVertGet() const
{
	ASSERT_VALID( this );
bool bGridLines = ( SiwGetStyle() & __EGBS_GRIDLINES_V ) ? true : false;
	return bGridLines;
}

bool CExtGridBaseWnd::GridLinesVertSet(
	bool bGridLines,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
bool bGridLinesOld = GridLinesVertGet();
	if(		(bGridLinesOld && bGridLines)
		||	((!bGridLinesOld) && (!bGridLines))
		)
		return bGridLinesOld;
	SiwModifyStyle(
		bGridLines ? __EGBS_GRIDLINES_V : 0L,
		__EGBS_GRIDLINES_V,
		false
		);
	if( bRedraw )
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	} // if( bRedraw )
	return bGridLinesOld;
}

bool CExtGridBaseWnd::LbExtSelectionGet() const
{
	ASSERT_VALID( this );
bool bLbExtSelection = ( SiwGetStyle() & __EGBS_LBEXT_SELECTION ) ? true : false;
	return bLbExtSelection;
}

bool CExtGridBaseWnd::LbExtSelectionSet( bool bLbExtSelection )
{
	ASSERT_VALID( this );
bool bLbExtSelectionOld = LbExtSelectionGet();
	if(		(bLbExtSelectionOld && bLbExtSelection)
		||	((!bLbExtSelectionOld) && (!bLbExtSelection))
		)
		return bLbExtSelectionOld;
	SiwModifyStyle(
		bLbExtSelection ? __EGBS_LBEXT_SELECTION : 0L,
		__EGBS_LBEXT_SELECTION,
		false
		);
	return bLbExtSelectionOld;
}

bool CExtGridBaseWnd::SubtractSelectionAreasGet() const
{
	ASSERT_VALID( this );
bool bSubtractSelectionAreas = ( SiwGetStyle() & __EGBS_SUBTRACT_SEL_AREAS ) ? true : false;
	return bSubtractSelectionAreas;
}

bool CExtGridBaseWnd::SubtractSelectionAreasSet(
	bool bSubtractSelectionAreas,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
bool bSubtractSelectionAreasOld = SubtractSelectionAreasGet();
	if(		(bSubtractSelectionAreasOld && bSubtractSelectionAreas)
		||	((!bSubtractSelectionAreasOld) && (!bSubtractSelectionAreas))
		)
		return bSubtractSelectionAreasOld;
	SiwModifyStyle(
		bSubtractSelectionAreas ? __EGBS_SUBTRACT_SEL_AREAS : 0L,
		__EGBS_SUBTRACT_SEL_AREAS,
		false
		);
	if( bRedraw )
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	} // if( bRedraw )
	return bSubtractSelectionAreasOld;
}

void CExtGridBaseWnd::SelectionSplitAllAreas(
	bool bSplitByColumns,
	bool bSplitByRows
	)
{
	ASSERT_VALID( this );
	if( (!bSplitByColumns) && (!bSplitByRows) )
		return;
INT nAreaCount = SelectionGetAreaCount();
	ASSERT( nAreaCount >= 0L );
	if( nAreaCount == 0L )
		return;
	if( bSplitByColumns )
	{
		for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
		{
			INT nAreaPos = nAreaCount - nAreaNumber - 1;
			CRect rcSelectionOld( SelectionGet( true, nAreaPos ) );
			ASSERT( rcSelectionOld.left <= rcSelectionOld.right );
			if( rcSelectionOld.left == rcSelectionOld.right )
				continue;
			INT nNewCount = rcSelectionOld.right - rcSelectionOld.left + 1;
			ASSERT( nNewCount > 0 );
			for( INT nNewNumber = 0; nNewNumber < nNewCount; nNewNumber++ )
			{
				CRect rcSelectionNew(
					rcSelectionOld.left + nNewNumber,
					rcSelectionOld.top,
					rcSelectionOld.left + nNewNumber,
					rcSelectionOld.bottom
					);
				if( nNewNumber == 0 )
				{
					VERIFY( SelectionSetAt( nAreaPos, rcSelectionNew, false ) );
				} // if( nNewNumber == 0 )
				else
				{
					VERIFY( SelectionInsertAt( nAreaPos+nNewNumber, rcSelectionNew, false ) );
				} // else from if( nNewNumber == 0 )
			} // for( INT nNewNumber = 0; nNewNumber < nNewCount; nNewNumber++ )
		} // for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
	} // if( bSplitByColumns )
	if( bSplitByRows )
	{
		nAreaCount = SelectionGetAreaCount();
		for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
		{
			INT nAreaPos = nAreaCount - nAreaNumber - 1;
			CRect rcSelectionOld( SelectionGet( true, nAreaPos ) );
			ASSERT( rcSelectionOld.top <= rcSelectionOld.bottom );
			if( rcSelectionOld.top == rcSelectionOld.bottom )
				continue;
			INT nNewCount = rcSelectionOld.bottom - rcSelectionOld.top + 1;
			ASSERT( nNewCount > 0 );
			for( INT nNewNumber = 0; nNewNumber < nNewCount; nNewNumber++ )
			{
				CRect rcSelectionNew(
					rcSelectionOld.left,
					rcSelectionOld.top + nNewNumber,
					rcSelectionOld.right,
					rcSelectionOld.top + nNewNumber
					);
				if( nNewNumber == 0 )
				{
					VERIFY( SelectionSetAt( nAreaPos, rcSelectionNew, false ) );
				} // if( nNewNumber == 0 )
				else
				{
					VERIFY( SelectionInsertAt( nAreaPos+nNewNumber, rcSelectionNew, false ) );
				} // else from if( nNewNumber == 0 )
			} // for( INT nNewNumber = 0; nNewNumber < nNewCount; nNewNumber++ )
		} // for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
	} // if( bSplitByRows )
}

bool CExtGridBaseWnd::HoverEventsInnerGet() const
{
	ASSERT_VALID( this );
DWORD dwSiwStylesEx = SiwGetStyleEx();
bool bRetVal = (dwSiwStylesEx & __EGBS_EX_HVI_EVENT_CELLS) ? true : false;
	return bRetVal;
}

bool CExtGridBaseWnd::HoverEventsOuterGet() const
{
	ASSERT_VALID( this );
DWORD dwSiwStylesEx = SiwGetStyleEx();
bool bRetVal = (dwSiwStylesEx & __EGBS_EX_HVO_EVENT_CELLS) ? true : false;
	return bRetVal;
}

void CExtGridBaseWnd::HoverEventsSet(
	bool bHoverInnerCells, // = true
	bool bHoverOuterCells // = true
	)
{
	ASSERT_VALID( this );
DWORD dwStyleExAdd = 0L, dwStyleExRemove = 0L;
	if( bHoverInnerCells )
		dwStyleExAdd |= __EGBS_EX_HVI_EVENT_CELLS;
	else
		dwStyleExRemove |= __EGBS_EX_HVI_EVENT_CELLS;
	if( bHoverOuterCells )
		dwStyleExAdd |= __EGBS_EX_HVO_EVENT_CELLS;
	else
		dwStyleExRemove |= __EGBS_EX_HVO_EVENT_CELLS;
	SiwModifyStyleEx(
		dwStyleExAdd,
		dwStyleExRemove,
		false
		);
}

bool CExtGridBaseWnd::HoverHighlightCellInnerGet() const
{
	ASSERT_VALID( this );
DWORD dwSiwStylesEx = SiwGetStyleEx();
bool bRetVal = (dwSiwStylesEx & __EGBS_EX_HVI_HIGHLIGHT_CELL) ? true : false;
	return bRetVal;
}

bool CExtGridBaseWnd::HoverHighlightCellOuterGet() const
{
	ASSERT_VALID( this );
DWORD dwSiwStylesEx = SiwGetStyleEx();
bool bRetVal = (dwSiwStylesEx & __EGBS_EX_HVO_HIGHLIGHT_CELL) ? true : false;
	return bRetVal;
}

bool CExtGridBaseWnd::HoverHighlightRowInnerGet() const
{
	ASSERT_VALID( this );
DWORD dwSiwStylesEx = SiwGetStyleEx();
bool bRetVal = (dwSiwStylesEx & __EGBS_EX_HVI_HIGHLIGHT_ROWS) ? true : false;
	return bRetVal;
}

bool CExtGridBaseWnd::HoverHighlightRowOuterGet() const
{
	ASSERT_VALID( this );
DWORD dwSiwStylesEx = SiwGetStyleEx();
bool bRetVal = (dwSiwStylesEx & __EGBS_EX_HVO_HIGHLIGHT_ROWS) ? true : false;
	return bRetVal;
}

bool CExtGridBaseWnd::HoverHighlightColumnInnerGet() const
{
	ASSERT_VALID( this );
DWORD dwSiwStylesEx = SiwGetStyleEx();
bool bRetVal = (dwSiwStylesEx & __EGBS_EX_HVI_HIGHLIGHT_COLUMNS) ? true : false;
	return bRetVal;
}

bool CExtGridBaseWnd::HoverHighlightColumnOuterGet() const
{
	ASSERT_VALID( this );
DWORD dwSiwStylesEx = SiwGetStyleEx();
bool bRetVal = (dwSiwStylesEx & __EGBS_EX_HVO_HIGHLIGHT_COLUMNS) ? true : false;
	return bRetVal;
}

void CExtGridBaseWnd::HoverHighlightSet(
	bool bHighlightSingleCellsInner, // = true
	bool bHighlightSingleCellsOuter, // = true
	bool bHighlightColumnCellsInner, // = true
	bool bHighlightColumnCellsOuter, // = true
	bool bHighlightRowCellsInner, // = true
	bool bHighlightRowCellsOuter // = true
	)
{
	ASSERT_VALID( this );
DWORD dwStyleExAdd = 0L, dwStyleExRemove = 0L;
	if( bHighlightSingleCellsInner )
		dwStyleExAdd |= __EGBS_EX_HVI_HIGHLIGHT_CELL;
	else
		dwStyleExRemove |= __EGBS_EX_HVI_HIGHLIGHT_CELL;
	if( bHighlightSingleCellsOuter )
		dwStyleExAdd |= __EGBS_EX_HVO_HIGHLIGHT_CELL;
	else
		dwStyleExRemove |= __EGBS_EX_HVO_HIGHLIGHT_CELL;
	if( bHighlightColumnCellsInner )
		dwStyleExAdd |= __EGBS_EX_HVI_HIGHLIGHT_COLUMNS;
	else
		dwStyleExRemove |= __EGBS_EX_HVI_HIGHLIGHT_COLUMNS;
	if( bHighlightColumnCellsOuter )
		dwStyleExAdd |= __EGBS_EX_HVO_HIGHLIGHT_COLUMNS;
	else
		dwStyleExRemove |= __EGBS_EX_HVO_HIGHLIGHT_COLUMNS;
	if( bHighlightRowCellsInner )
		dwStyleExAdd |= __EGBS_EX_HVI_HIGHLIGHT_ROWS;
	else
		dwStyleExRemove |= __EGBS_EX_HVI_HIGHLIGHT_ROWS;
	if( bHighlightRowCellsOuter )
		dwStyleExAdd |= __EGBS_EX_HVO_HIGHLIGHT_ROWS;
	else
		dwStyleExRemove |= __EGBS_EX_HVO_HIGHLIGHT_ROWS;
	SiwModifyStyleEx(
		dwStyleExAdd,
		dwStyleExRemove,
		false
		);
}

CExtGridHitTestInfo & CExtGridBaseWnd::HoverInfoGet()
{
	ASSERT_VALID( this );
	return m_htInfoHover;
}

const CExtGridHitTestInfo & CExtGridBaseWnd::HoverInfoGet() const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridBaseWnd * > ( this ) )
			-> HoverInfoGet();
}

bool CExtGridBaseWnd::SelectionHighlightRowInnerGet() const
{
	ASSERT_VALID( this );
DWORD dwSiwStylesEx = SiwGetStyleEx();
bool bRetVal = (dwSiwStylesEx & __EGBS_EX_SI_HIGHLIGHT_ROWS) ? true : false;
	return bRetVal;
}

bool CExtGridBaseWnd::SelectionHighlightRowOuterGet() const
{
	ASSERT_VALID( this );
DWORD dwSiwStylesEx = SiwGetStyleEx();
bool bRetVal = (dwSiwStylesEx & __EGBS_EX_SO_HIGHLIGHT_ROWS) ? true : false;
	return bRetVal;
}

bool CExtGridBaseWnd::SelectionHighlightColumnInnerGet() const
{
	ASSERT_VALID( this );
DWORD dwSiwStylesEx = SiwGetStyleEx();
bool bRetVal = (dwSiwStylesEx & __EGBS_EX_SI_HIGHLIGHT_COLUMNS) ? true : false;
	return bRetVal;
}

bool CExtGridBaseWnd::SelectionHighlightColumnOuterGet() const
{
	ASSERT_VALID( this );
DWORD dwSiwStylesEx = SiwGetStyleEx();
bool bRetVal = (dwSiwStylesEx & __EGBS_EX_SO_HIGHLIGHT_COLUMNS) ? true : false;
	return bRetVal;
}

void CExtGridBaseWnd::SelectionHighlightSet(
	bool bHighlightColumnCellsInner, // = true
	bool bHighlightColumnCellsOuter, // = true
	bool bHighlightRowCellsInner, // = true
	bool bHighlightRowCellsOuter // = true
	)
{
	ASSERT_VALID( this );
DWORD dwStyleExAdd = 0L, dwStyleExRemove = 0L;
	if( bHighlightColumnCellsInner )
		dwStyleExAdd |= __EGBS_EX_SI_HIGHLIGHT_COLUMNS;
	else
		dwStyleExRemove |= __EGBS_EX_SI_HIGHLIGHT_COLUMNS;
	if( bHighlightColumnCellsOuter )
		dwStyleExAdd |= __EGBS_EX_SO_HIGHLIGHT_COLUMNS;
	else
		dwStyleExRemove |= __EGBS_EX_SO_HIGHLIGHT_COLUMNS;
	if( bHighlightRowCellsInner )
		dwStyleExAdd |= __EGBS_EX_SI_HIGHLIGHT_ROWS;
	else
		dwStyleExRemove |= __EGBS_EX_SI_HIGHLIGHT_ROWS;
	if( bHighlightRowCellsOuter )
		dwStyleExAdd |= __EGBS_EX_SO_HIGHLIGHT_ROWS;
	else
		dwStyleExRemove |= __EGBS_EX_SO_HIGHLIGHT_ROWS;
	SiwModifyStyleEx(
		dwStyleExAdd,
		dwStyleExRemove,
		false
		);
}

bool CExtGridBaseWnd::FocusHighlightRowInnerGet() const
{
	ASSERT_VALID( this );
DWORD dwSiwStylesEx = SiwGetStyleEx();
bool bRetVal = (dwSiwStylesEx & __EGBS_EX_FI_HIGHLIGHT_ROWS) ? true : false;
	return bRetVal;
}

bool CExtGridBaseWnd::FocusHighlightRowOuterGet() const
{
	ASSERT_VALID( this );
DWORD dwSiwStylesEx = SiwGetStyleEx();
bool bRetVal = (dwSiwStylesEx & __EGBS_EX_FO_HIGHLIGHT_ROWS) ? true : false;
	return bRetVal;
}

bool CExtGridBaseWnd::FocusHighlightColumnInnerGet() const
{
	ASSERT_VALID( this );
DWORD dwSiwStylesEx = SiwGetStyleEx();
bool bRetVal = (dwSiwStylesEx & __EGBS_EX_FI_HIGHLIGHT_COLUMNS) ? true : false;
	return bRetVal;
}

bool CExtGridBaseWnd::FocusHighlightColumnOuterGet() const
{
	ASSERT_VALID( this );
DWORD dwSiwStylesEx = SiwGetStyleEx();
bool bRetVal = (dwSiwStylesEx & __EGBS_EX_FO_HIGHLIGHT_COLUMNS) ? true : false;
	return bRetVal;
}

void CExtGridBaseWnd::FocusHighlightSet(
	bool bHighlightColumnCellsInner, // = true
	bool bHighlightColumnCellsOuter, // = true
	bool bHighlightRowCellsInner, // = true
	bool bHighlightRowCellsOuter // = true
	)
{
	ASSERT_VALID( this );
DWORD dwStyleExAdd = 0L, dwStyleExRemove = 0L;
	if( bHighlightColumnCellsInner )
		dwStyleExAdd |= __EGBS_EX_FI_HIGHLIGHT_COLUMNS;
	else
		dwStyleExRemove |= __EGBS_EX_FI_HIGHLIGHT_COLUMNS;
	if( bHighlightColumnCellsOuter )
		dwStyleExAdd |= __EGBS_EX_FO_HIGHLIGHT_COLUMNS;
	else
		dwStyleExRemove |= __EGBS_EX_FO_HIGHLIGHT_COLUMNS;
	if( bHighlightRowCellsInner )
		dwStyleExAdd |= __EGBS_EX_FI_HIGHLIGHT_ROWS;
	else
		dwStyleExRemove |= __EGBS_EX_FI_HIGHLIGHT_ROWS;
	if( bHighlightRowCellsOuter )
		dwStyleExAdd |= __EGBS_EX_FO_HIGHLIGHT_ROWS;
	else
		dwStyleExRemove |= __EGBS_EX_FO_HIGHLIGHT_ROWS;
	SiwModifyStyleEx(
		dwStyleExAdd,
		dwStyleExRemove,
		false
		);
}

DWORD CExtGridBaseWnd::SiwModifyStyle(
	DWORD dwStyleAdd,
	DWORD dwStyleRemove, // = 0L
	bool bUpdateWnd // = true
	)
{
	ASSERT_VALID( this );
DWORD dwRetVal = CExtScrollItemWnd::SiwModifyStyle(
		dwStyleAdd,
		dwStyleRemove,
		false
		);
DWORD dwScrollItemWndStyle = SiwGetStyle();
DWORD dwScrollTypeH = (dwScrollItemWndStyle&__ESIS_STH_MASK);
DWORD dwScrollTypeV = ((dwScrollItemWndStyle&__ESIS_STV_MASK) >> __ESIW_ST_BIT_COUNT);
	if( dwScrollTypeH == __ESIW_ST_VIRTUAL )
	{
		if( m_nCountOfColumns >= 0L )
			m_nCountOfColumns = -1L;
	} // if( dwScrollTypeH == __ESIW_ST_VIRTUAL )
	else
	{
		if( m_nCountOfColumns < 0L )
			m_nCountOfColumns = 0L;
	} // else from if( dwScrollTypeH == __ESIW_ST_VIRTUAL )
	if( dwScrollTypeV == __ESIW_ST_VIRTUAL )
	{
		if( m_nCountOfRows >= 0L )
			m_nCountOfRows = -1L;
	} // if( dwScrollTypeV == __ESIW_ST_VIRTUAL )
	else
	{
		if( m_nCountOfRows < 0L )
			m_nCountOfRows = 0L;
	} // else from if( dwScrollTypeV == __ESIW_ST_VIRTUAL )
	if( bUpdateWnd )
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	} // if( bUpdateWnd )
	return dwRetVal;
}

HWND CExtGridBaseWnd::GetSafeInplaceActiveHwnd()
{
	if( this == NULL )
		return NULL;
	ASSERT_VALID( this );
	if(		m_hWndInplaceControl == NULL
		||	( ! ::IsWindow( m_hWndInplaceControl ) )
		)
		return NULL;
	return m_hWndInplaceControl;
}

DWORD CExtGridBaseWnd::SelectionTypeGet() const
{
	ASSERT_VALID( this );
DWORD dwSelectionType = SiwGetStyle() & __EGBS_SF_MASK;
	return dwSelectionType;
}

DWORD CExtGridBaseWnd::SelectionTypeSet(
	DWORD dwNewSelectionType
	)
{
	ASSERT_VALID( this );
	dwNewSelectionType &= ~__EGBS_SF_MASK;
DWORD dwOldSelectionType = SelectionTypeGet();
	if( dwOldSelectionType != dwNewSelectionType )
		SiwModifyStyle( dwNewSelectionType, __EGBS_SF_MASK, false );
	return dwOldSelectionType;
}

bool CExtGridBaseWnd::OnGbwCanResizeColumn(
	LONG nColNo,
	INT * p_nExtentMin, //  = NULL
	INT * p_nExtentMax //  = NULL
	)
{
	ASSERT_VALID( this );
	nColNo;
	if( p_nExtentMin != NULL )
		(*p_nExtentMin) = 0;
	if( p_nExtentMax != NULL )
		(*p_nExtentMax) = 32767;
	return true;
}

bool CExtGridBaseWnd::OnGbwCanResizeRow(
	LONG nRowNo,
	INT * p_nExtentMin, //  = NULL
	INT * p_nExtentMax //  = NULL
	)
{
	ASSERT_VALID( this );
	nRowNo;
	if( p_nExtentMin != NULL )
		(*p_nExtentMin) = 0;
	if( p_nExtentMax != NULL )
		(*p_nExtentMax) = 32767;
	return true;
}

void CExtGridBaseWnd::OnGbwHoverCell(
	const CExtGridHitTestInfo & htInfoPrevious,
	const CExtGridHitTestInfo & htInfoCurrent,
	bool bHoverEnter,
	bool bHoverLeave
	)
{
	ASSERT_VALID( this );
	bHoverEnter;
	if( bHoverLeave )
	{
		if(		m_wndToolTip.GetSafeHwnd() != NULL
			&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
			)
			m_wndToolTip.DelTool( this, 1 );
		CWnd::CancelToolTips();
		m_wndContentExpand.Deactivate();
		m_htInfoExpand.Empty();
	} // if( bHoverLeave )

//	if( bHoverLeave )
//	{
//		TRACE2( " --> CExtGridBaseWnd::OnGbwHoverCell --> hover leave: %02d , %02d", htInfoPrevious.m_nColNo, htInfoPrevious.m_nRowNo );
//		TRACE2( "   ( %03d, %03d - ", htInfoPrevious.m_rcItem.left, htInfoPrevious.m_rcItem.right );
//		TRACE2( "%03d, %03d )", htInfoPrevious.m_rcItem.top, htInfoPrevious.m_rcItem.bottom );
//		TRACE2( "  -  ( %03d, %03d )\n", htInfoPrevious.m_ptClient.x, htInfoPrevious.m_ptClient.y );
//	}
//	if( bHoverEnter )
//	{
//		TRACE2( " --> CExtGridBaseWnd::OnGbwHoverCell --> hover enter: %02d , %02d", htInfoCurrent.m_nColNo, htInfoCurrent.m_nRowNo );
//		TRACE2( "   ( %03d, %03d - ", htInfoCurrent.m_rcItem.left, htInfoCurrent.m_rcItem.right );
//		TRACE2( "%03d, %03d )", htInfoCurrent.m_rcItem.top, htInfoCurrent.m_rcItem.bottom );
//		TRACE2( "  -  ( %03d, %03d )\n", htInfoCurrent.m_ptClient.x, htInfoCurrent.m_ptClient.y );
//	}
//	if( ( ! bHoverLeave ) && ( ! bHoverEnter ) )
//	{
//		TRACE2( " --> CExtGridBaseWnd::OnGbwHoverCell --> hover move : %02d , %02d", htInfoCurrent.m_nColNo, htInfoCurrent.m_nRowNo );
//		TRACE2( "   ( %03d, %03d - ", htInfoCurrent.m_rcItem.left, htInfoCurrent.m_rcItem.right );
//		TRACE2( "%03d, %03d )", htInfoCurrent.m_rcItem.top, htInfoCurrent.m_rcItem.bottom );
//		TRACE2( "  -  ( %03d, %03d )\n", htInfoCurrent.m_ptClient.x, htInfoCurrent.m_ptClient.y );
//	}
DWORD dwSiwStyleEx = SiwGetStyleEx();
	if( ( dwSiwStyleEx & (__EGBS_EX_HVI_EVENT_CELLS|__EGBS_EX_HVO_EVENT_CELLS) ) != 0 )
	{
		bool bHoverEmptyPrevious = htInfoPrevious.IsHoverEmpty();
		bool bHoverEmptyCurrent = htInfoCurrent.IsHoverEmpty();
		ASSERT( (!bHoverEmptyPrevious) || (!bHoverEmptyCurrent) );
		if( ! htInfoPrevious.IsHoverEqual( htInfoCurrent ) )
		{
			if(		( ! bHoverEmptyPrevious )
				&&	htInfoPrevious.IsValidRect()
				)
			{
				_InvalidateHoveredArea( htInfoPrevious );
				if( CExtMouseCaptureSink::GetCapture() == m_hWnd )
					CExtMouseCaptureSink::ReleaseCapture();
			}
			if(		( ! bHoverEmptyCurrent )
				&&	htInfoCurrent.IsValidRect()
				)
			{
				_InvalidateHoveredArea( htInfoCurrent );
				if( CExtMouseCaptureSink::GetCapture() != m_hWnd )
					CExtMouseCaptureSink::SetCapture( m_hWnd );
			}
		} // if( ! htInfoPrevious.IsHoverEqual( htInfoCurrent ) )
	} // if( ( dwSiwStyleEx & (__EGBS_EX_HVI_EVENT_CELLS|__EGBS_EX_HVO_EVENT_CELLS) ) != 0 )
}

void CExtGridBaseWnd::_InvalidateHoveredArea(
	const CExtGridHitTestInfo & htInfo
	)
{
	ASSERT_VALID( this );
	ASSERT( ! htInfo.IsHoverEmpty() );
	ASSERT( htInfo.IsValidRect() );
DWORD dwSiwStyleEx = SiwGetStyleEx();
	ASSERT( ( dwSiwStyleEx & (__EGBS_EX_HVI_EVENT_CELLS|__EGBS_EX_HVO_EVENT_CELLS) ) != 0 );
	if( (	dwSiwStyleEx &
				(__EGBS_EX_HVI_HIGHLIGHT_COLUMNS|__EGBS_EX_HVO_HIGHLIGHT_COLUMNS
				|__EGBS_EX_HVI_HIGHLIGHT_ROWS|__EGBS_EX_HVO_HIGHLIGHT_ROWS
				|__EGBS_EX_HVI_HIGHLIGHT_CELL|__EGBS_EX_HVO_HIGHLIGHT_CELL
				)
			) == 0
		)
		return;
	if( (	dwSiwStyleEx &
				(__EGBS_EX_HVI_HIGHLIGHT_COLUMNS|__EGBS_EX_HVO_HIGHLIGHT_COLUMNS
				|__EGBS_EX_HVI_HIGHLIGHT_ROWS|__EGBS_EX_HVO_HIGHLIGHT_ROWS
				)
			) != 0
		)
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		if( (	dwSiwStyleEx &
					(__EGBS_EX_HVI_HIGHLIGHT_COLUMNS|__EGBS_EX_HVO_HIGHLIGHT_COLUMNS)
				) != 0
			)
		{
			CRect rcCol( htInfo.m_rcItem );
			rcCol.top = rcClient.top;
			rcCol.bottom = rcClient.bottom;
			InvalidateRect( &rcCol );
		}
		if( (	dwSiwStyleEx &
					(__EGBS_EX_HVI_HIGHLIGHT_ROWS|__EGBS_EX_HVO_HIGHLIGHT_ROWS)
				) != 0
			)
		{
			CRect rcRow( htInfo.m_rcItem );
			rcRow.left = rcClient.left;
			rcRow.right = rcClient.right;
			InvalidateRect( &rcRow );
		}
	}
	else if( (	(dwSiwStyleEx&__EGBS_EX_HVI_HIGHLIGHT_CELL) != 0
				&&	(htInfo.m_dwAreaFlags&__EGBWA_INNER_CELLS) != 0
				)
			||	(	(dwSiwStyleEx&__EGBS_EX_HVO_HIGHLIGHT_CELL) != 0
				&&	(htInfo.m_dwAreaFlags&__EGBWA_OUTER_CELLS) != 0
				)
			)
			InvalidateRect( &htInfo.m_rcItem );

}

bool CExtGridBaseWnd::_DoSetCursor(
	const POINT & ptClient
	)
{
	ASSERT_VALID( this );
CExtGridHitTestInfo htInfo( ptClient );
	HitTest( htInfo, true, true );
	return OnGbwSetCursor( htInfo );
}

bool CExtGridBaseWnd::_DoSetCursor()
{
	ASSERT_VALID( this );
CExtGridHitTestInfo htInfo;
	HitTest( htInfo, true, true );
	return OnGbwSetCursor( htInfo );
}

bool CExtGridBaseWnd::OnGbwSetCursor(
	const CExtGridHitTestInfo & htInfo
	)
{
	ASSERT_VALID( this );
	if( m_eMTT == __EMTT_RESIZING_H )
	{
		HCURSOR hCursor =
			(m_hCursorResizingH == NULL)
				? ( ::LoadCursor(NULL,IDC_SIZEWE) )
				: m_hCursorResizingH;
		if( hCursor == NULL )
			hCursor = ::LoadCursor( NULL, IDC_ARROW );
		ASSERT( hCursor != NULL );
		SetCursor( hCursor );
		return true;
	} // if( m_eMTT == __EMTT_RESIZING_H )
	if( m_eMTT == __EMTT_RESIZING_V )
	{
		HCURSOR hCursor =
			(m_hCursorResizingV == NULL)
				? ( ::LoadCursor(NULL,IDC_SIZENS) )
				: m_hCursorResizingV;
		if( hCursor == NULL )
			hCursor = ::LoadCursor( NULL, IDC_ARROW );
		ASSERT( hCursor != NULL );
		SetCursor( hCursor );
		return true;
	} // if( m_eMTT == __EMTT_RESIZING_V )
	if( m_eMTT != __EMTT_NOTHING )
		return true;
HCURSOR hCursor = NULL;
DWORD dwSiwStyles = SiwGetStyle();
	if( (dwSiwStyles & __EGBS_RESIZING_CELLS_MASK) != 0 )
	{
		if(		htInfo.m_nColNo >= 0 && htInfo.m_nRowNo >= 0
			&&	(htInfo.m_dwAreaFlags & __EGBWA_NEAR_CELL_BORDER_SIDE_MASK) != 0
			)
		{
			if( (htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0 )
			{
				if(		(htInfo.m_dwAreaFlags & __EGBWA_NEAR_CELL_BORDER_V) != 0
					&&	(dwSiwStyles & __EGBS_RESIZING_CELLS_INNER_V) != 0
					&&	htInfo.m_nRowNo >= 0
					&&	OnGbwCanResizeRow( htInfo.m_nRowNo )
					)
				{
					hCursor = (m_hCursorResizingV == NULL)
						? ( ::LoadCursor(NULL,IDC_SIZENS) )
						: m_hCursorResizingV;
				}
				if(		hCursor == NULL
					&&	(htInfo.m_dwAreaFlags & __EGBWA_NEAR_CELL_BORDER_H) != 0
					&&	(dwSiwStyles & __EGBS_RESIZING_CELLS_INNER_H) != 0
					&&	htInfo.m_nColNo >= 0
					&&	OnGbwCanResizeColumn( htInfo.m_nColNo )
					)
				{
					hCursor = (m_hCursorResizingH == NULL)
						? ( ::LoadCursor(NULL,IDC_SIZEWE) )
						: m_hCursorResizingH;
				}
			} // if( (htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0 )
			else if( (htInfo.m_dwAreaFlags & __EGBWA_OUTER_CELLS) != 0 )
			{
				if(		(htInfo.m_dwAreaFlags & __EGBWA_NEAR_CELL_BORDER_V) != 0
					&&	(htInfo.m_dwAreaFlags & __EGBWA_OUTER_H	) != 0
					&&	(dwSiwStyles & __EGBS_RESIZING_CELLS_OUTER_V) != 0
					&&	htInfo.m_nRowNo >= 0
					&&	OnGbwCanResizeRow( htInfo.m_nRowNo )
					)
				{
					hCursor = (m_hCursorResizingV == NULL)
						? ( ::LoadCursor(NULL,IDC_SIZENS) )
						: m_hCursorResizingV;
				}
				if(		hCursor == NULL
					&&	(htInfo.m_dwAreaFlags & __EGBWA_NEAR_CELL_BORDER_H) != 0
					&&	(htInfo.m_dwAreaFlags & __EGBWA_OUTER_V	) != 0
					&&	(dwSiwStyles & __EGBS_RESIZING_CELLS_OUTER_H) != 0
					&&	htInfo.m_nColNo >= 0
					&&	OnGbwCanResizeColumn( htInfo.m_nColNo )
					)
				{
					hCursor = (m_hCursorResizingH == NULL)
						? ( ::LoadCursor(NULL,IDC_SIZEWE) )
						: m_hCursorResizingH;
				}
			} // else if( (htInfo.m_dwAreaFlags & __EGBWA_OUTER_CELLS) != 0 )
		} // if( htInfo.m_nColNo >= 0 && htInfo.m_nRowNo >= 0 ...
	} // if( (dwSiwStyles & __EGBS_RESIZING_CELLS_MASK) != 0 )
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	if( hCursor == NULL )
	{
		CExtLocalResourceHelper _LRH;
		CWinApp * pApp = ::AfxGetApp();
		ASSERT_VALID( pApp );
		if( (htInfo.m_dwAreaFlags&__EGBWA_CELL_FOCUS_ARROW) != 0 )
		{ // if focus arrow
			hCursor = m_hCursorFocusArrow;
		} // if focus arrow
		else if( (htInfo.m_dwAreaFlags&__EGBWA_CELL_SORT_ARROW) != 0 )
		{ // if sort arrow
			hCursor = m_hCursorSortArrow;
		} // if sort arrow
		else if( (htInfo.m_dwAreaFlags&__EGBWA_CELL_IMAGE) != 0 )
		{ // if sort arrow
			hCursor = m_hCursorIcon;
		} // if sort arrow
		else if(	(htInfo.m_dwAreaFlags&__EGBWA_CELL_BUTTON) != 0 )
		{ // if cell button
#if (!defined __EXT_MFC_NO_GRIDWND)
			if( htInfo.m_nButtonType == INT(CExtGridCell::__EBTT_ELLIPSIS) )
				hCursor = m_hCursorBtnEllipsis;
			else if( htInfo.m_nButtonType == INT(CExtGridCell::__EBTT_DROPDOWN) )
				hCursor = m_hCursorBtnDropdown;
			else if( htInfo.m_nButtonType == INT(CExtGridCell::__EBTT_UPDOWN_UP) )
				hCursor = m_hCursorBtnUp;
			else if( htInfo.m_nButtonType == INT(CExtGridCell::__EBTT_UPDOWN_DOWN) )
				hCursor = m_hCursorBtnDown;
#endif // (!defined __EXT_MFC_NO_GRIDWND)
		} // if cell button
		else if(	(htInfo.m_dwAreaFlags&__EGBWA_CELL_CHECKBOX) != 0 )
		{ // if check-box
			hCursor = m_hCursorCheckBox;
		} // if check-box
		else if(	(htInfo.m_dwAreaFlags&__EGBWA_INNER_CELLS) != 0
			&&		( htInfo.m_nColNo >= 0L || htInfo.m_nRowNo >= 0L )
			)
		{ // inner selection cursor
			hCursor = m_hCursorInnerCell;
		} // inner selection cursor
		else if(	(htInfo.m_dwAreaFlags&__EGBWA_OUTER_SIDE_MASK) != 0
				&&	(dwSiwStyles&(__EGBS_SF_SELECT_OUTER_ROWS|__EGBS_SF_SELECT_OUTER_COLUMNS)) != 0
				&&	( htInfo.m_nColNo >= 0L || htInfo.m_nRowNo >= 0L )
			)
		{ // outer full-row/column selection cursors
			if(		(htInfo.m_dwAreaFlags&__EGBWA_OUTER_TOP) != 0
				&&	htInfo.m_nColNo >= 0L
				&&	(dwSiwStyles&__EGBS_SF_SELECT_OUTER_COLUMNS) != 0
				)
			{
				if( m_hCursorOuterSelectionTop == NULL )
					m_hCursorOuterSelectionTop = 
						pApp->LoadCursor(
							MAKEINTRESOURCE( IDC_EXT_BLACK_ARROW_BOTTOM )
							);
				hCursor = m_hCursorOuterSelectionTop;
			} // if( (htInfo.m_dwAreaFlags&__EGBWA_OUTER_TOP) != 0 )
			else if(
					(htInfo.m_dwAreaFlags&__EGBWA_OUTER_BOTTOM) != 0
				&&	htInfo.m_nColNo >= 0L
				&&	(dwSiwStyles&__EGBS_SF_SELECT_OUTER_COLUMNS) != 0
				)
			{
				if( m_hCursorOuterSelectionBottom == NULL )
					m_hCursorOuterSelectionBottom = 
						pApp->LoadCursor(
							MAKEINTRESOURCE( IDC_EXT_BLACK_ARROW_TOP )
							);
				hCursor = m_hCursorOuterSelectionBottom;
			} // else if( (htInfo.m_dwAreaFlags&__EGBWA_OUTER_BOTTOM) != 0 )
			else if(
					(htInfo.m_dwAreaFlags&__EGBWA_OUTER_LEFT) != 0
				&&	htInfo.m_nRowNo >= 0L
				&&	(dwSiwStyles&__EGBS_SF_SELECT_OUTER_ROWS) != 0
				)
			{
				if( m_hCursorOuterSelectionLeft == NULL )
					m_hCursorOuterSelectionLeft = 
						pApp->LoadCursor(
							MAKEINTRESOURCE( IDC_EXT_BLACK_ARROW_RIGHT )
							);
				hCursor = m_hCursorOuterSelectionLeft;
			} // else if( (htInfo.m_dwAreaFlags&__EGBWA_OUTER_LEFT) != 0 )
			else if(
					(htInfo.m_dwAreaFlags&__EGBWA_OUTER_RIGHT) != 0
				&&	htInfo.m_nRowNo >= 0L
				&&	(dwSiwStyles&__EGBS_SF_SELECT_OUTER_ROWS) != 0
				)
			{
				if( m_hCursorOuterSelectionRight == NULL )
					m_hCursorOuterSelectionRight = 
						pApp->LoadCursor(
							MAKEINTRESOURCE( IDC_EXT_BLACK_ARROW_LEFT )
							);
				hCursor = m_hCursorOuterSelectionRight;
			} // else if( (htInfo.m_dwAreaFlags&__EGBWA_OUTER_RIGHT) != 0 )
		} // outer full-row/column selection cursors
	} // if( hCursor == NULL )
#endif // (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	if( hCursor == NULL )
		hCursor = m_hCursorDefault;
	if( hCursor == NULL )
	{
		hCursor = ::LoadCursor( NULL, IDC_ARROW );
		ASSERT( hCursor != NULL );
	} // hCursor
	SetCursor( hCursor );
	return true;
}

BOOL CExtGridBaseWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	ASSERT_VALID( this );
CPoint ptCursor;
	if( ! ::GetCursorPos(&ptCursor) )
		return CExtScrollItemWnd::OnSetCursor(pWnd,nHitTest,message);
	ScreenToClient( &ptCursor );
	if( _DoSetCursor( ptCursor ) )
		return TRUE;
	return CExtScrollItemWnd::OnSetCursor( pWnd, nHitTest, message );
}

void CExtGridBaseWnd::OnCaptureChanged(CWnd *pWnd) 
{
	ASSERT_VALID( this );
	CExtScrollItemWnd::OnCaptureChanged(pWnd);
	if(		m_eMTT != __EMTT_NOTHING
		&&	CExtMouseCaptureSink::GetCapture() != m_hWnd
		)
		SendMessage( WM_CANCELMODE );
}

void CExtGridBaseWnd::OnCancelMode() 
{
	ASSERT_VALID( this );
	CExtScrollItemWnd::OnCancelMode();
	if( m_bProcessingCancelMode )
		return;
	m_bProcessingCancelMode = true;

	if( m_hWndInplaceControl != NULL )
	{
		if( ::IsWindow(m_hWndInplaceControl) )
		{
			if( ::GetCapture() == m_hWndInplaceControl )
				::ReleaseCapture();
			::DestroyWindow(m_hWndInplaceControl);
		} // if( ::IsWindow(m_hWndInplaceControl) )
		m_hWndInplaceControl = NULL;
	} // if( m_hWndInplaceControl != NULL )
	if(		m_wndToolTip.GetSafeHwnd() != NULL
		&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
		)
		m_wndToolTip.DelTool( this, 1 );
	CWnd::CancelToolTips();
	m_wndContentExpand.Deactivate();
	m_htInfoExpand.Empty();

	// cancel any kind of clicked mouse tracking
	if( m_eMTT != __EMTT_NOTHING )
	{
		if(		m_eMTT == __EMTT_RESIZING_H
			||	m_eMTT == __EMTT_RESIZING_V
			)
			OnGbwResizingStateUpdate( false, true );
		m_eMTT = __EMTT_NOTHING;
		m_nTrackExtentMin
			= m_nTrackExtentMax
			= m_nTrackExtentCurrent
			= 0;
		m_htInfoResizing.Empty();
	} // if( m_eMTT != __EMTT_NOTHING )

	// cancel hover tracking
	OnGbwHoverRemove();

	if( CExtMouseCaptureSink::GetCapture() == m_hWnd )
		CExtMouseCaptureSink::ReleaseCapture();

	m_bProcessingCancelMode = false;
}

#if _MFC_VER < 0x700
void CExtGridBaseWnd::OnActivateApp(BOOL bActive, HTASK hTask) 
#else
void CExtGridBaseWnd::OnActivateApp(BOOL bActive, DWORD hTask) 
#endif
{
	ASSERT_VALID( this );
	hTask;
	if( !bActive )
		SendMessage( WM_CANCELMODE );
}

void CExtGridBaseWnd::OnSetFocus(CWnd* pOldWnd) 
{
	ASSERT_VALID( this );
	CExtScrollItemWnd::OnSetFocus(pOldWnd);
	OnGbwHoverRecalc();
	OnSwInvalidate( m_bRedrawErase );
}

void CExtGridBaseWnd::OnKillFocus(CWnd* pNewWnd) 
{
	ASSERT_VALID( this );
	if(		m_hWndInplaceControl != NULL
		&&	pNewWnd->GetSafeHwnd() == m_hWndInplaceControl
		)
		return;
	CExtScrollItemWnd::OnKillFocus(pNewWnd);
	SendMessage( WM_CANCELMODE );
	OnSwInvalidate( m_bRedrawErase );
}

void CExtGridBaseWnd::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	ASSERT_VALID( this );
	CExtScrollItemWnd::OnWindowPosChanged(lpwndpos);
	m_rcClient.SetRect( 0, 0, 0, 0 );
	if( (GetStyle() & WS_VISIBLE) == 0 )
		SendMessage( WM_CANCELMODE );
	_AnalyzeVirtualFocus();
}

void CExtGridBaseWnd::OnSize(UINT nType, int cx, int cy) 
{
	ASSERT_VALID( this );
	CExtScrollItemWnd::OnSize(nType, cx, cy);
	m_rcClient.SetRect( 0, 0, 0, 0 );
	if( (GetStyle() & WS_VISIBLE) == 0 )
		SendMessage( WM_CANCELMODE );
	_AnalyzeVirtualFocus();
}

void CExtGridBaseWnd::_AnalyzeVirtualFocus(
	bool bRedraw // = true
	)
{
CPoint ptFocusOld = FocusGet();
DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	if(	! (		(	dwScrollTypeH == __ESIW_ST_VIRTUAL
				&&	ptFocusOld.x >= 0
				&&	( ! OnSiwQueryVirtualBOF( true ) )
				&&	( ! OnSiwQueryVirtualEOF( true ) )
				)
			||	(	dwScrollTypeV == __ESIW_ST_VIRTUAL
				&&	ptFocusOld.y >= 0
				&&	( ! OnSiwQueryVirtualBOF( false ) )
				&&	( ! OnSiwQueryVirtualEOF( false ) )
				)
			)
		)
		return;
CPoint ptFocusNew = _AnalyzeVirtualFocus( ptFocusOld );
	if( ptFocusNew.x < 0 || ptFocusNew.y < 0 )
		FocusUnset( bRedraw );
	else
		FocusSet( ptFocusNew, false, false, true, bRedraw );
}

CPoint CExtGridBaseWnd::_AnalyzeVirtualFocus(
	const POINT & ptFocusOld
	)
{
	ASSERT_VALID( this );
DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	if(	! (		(	dwScrollTypeH == __ESIW_ST_VIRTUAL
				&&	ptFocusOld.x >= 0
				)
			||	(	dwScrollTypeV == __ESIW_ST_VIRTUAL
				&&	ptFocusOld.y >= 0
				)
			)
		)
		return CPoint( -1, -1 );
CPoint ptFocusNew( ptFocusOld );
bool bFocusInequality = false;
	if( dwScrollTypeH == __ESIW_ST_VIRTUAL )
	{
		ASSERT( ptFocusNew.x >= 0 );
		for(	bFocusInequality = true;
				OnSiwVirtualEndTestH(
					ptFocusNew.x,
					LPARAM(__EGBWA_INNER_CELLS)
					);
				ptFocusNew.x --
				)
		{
			if(  ptFocusNew.x == 0 )
				return CPoint( -1, -1 );
		}
		if( ptFocusNew.x > 0 )
		{
			CRect rcClient = OnSwGetClientRect();
			CExtGridHitTestInfo htInfo( CPoint( rcClient.right-1, rcClient.top+1 ) );
			HitTest( htInfo, false, true );
			if(		(htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0
				&&	(! htInfo.m_rcItem.IsRectEmpty() )
				&&	htInfo.m_nColNo == ptFocusNew.x
				)
			{
				ASSERT( htInfo.m_rcItem.left < rcClient.right );
				if( htInfo.m_rcItem.right > rcClient.right )
					ptFocusNew.x --;
			}
		} // if( ptFocusNew.x > 0 )
	} // if( dwScrollTypeH == __ESIW_ST_VIRTUAL );
	if( dwScrollTypeV == __ESIW_ST_VIRTUAL )
	{
		ASSERT( ptFocusNew.y >= 0 );
		for(	bFocusInequality = true;
				OnSiwVirtualEndTestV(
					ptFocusNew.y,
					LPARAM(__EGBWA_INNER_CELLS)
					);
				ptFocusNew.y --
				)
		{
			if(  ptFocusNew.y == 0 )
				return CPoint( -1, -1 );
		}
		if( ptFocusNew.y > 0 )
		{
			CRect rcClient = OnSwGetClientRect();
			CExtGridHitTestInfo htInfo( CPoint( rcClient.left+1, rcClient.bottom-1 ) );
			HitTest( htInfo, false, true );
			if(		(htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0
				&&	(! htInfo.m_rcItem.IsRectEmpty() )
				&&	htInfo.m_nRowNo == ptFocusNew.y
				)
			{
				ASSERT( htInfo.m_rcItem.top < rcClient.bottom );
				if( htInfo.m_rcItem.bottom > rcClient.bottom )
					ptFocusNew.y --;
			}
		} // if( ptFocusNew.y > 0 )
	} // if( dwScrollTypeV == __ESIW_ST_VIRTUAL );
	if( bFocusInequality )
		return ptFocusNew;
	else
		return ptFocusOld;
}

void CExtGridBaseWnd::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	ASSERT_VALID( this );
	CExtScrollItemWnd::OnShowWindow(bShow, nStatus);
	if( (GetStyle() & WS_VISIBLE) == 0 )
		SendMessage( WM_CANCELMODE );
}

int CExtGridBaseWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	ASSERT_VALID( this );
	pDesktopWnd;
	nHitTest;
	message;
	return m_nMouseActivateCode;
}

bool CExtGridBaseWnd::OnGbwDataDndIsAllowed() const
{
	ASSERT_VALID( this );
	return false;
}

CPoint CExtGridBaseWnd::OnGbwDataDndGetStartOffset() const
{
	ASSERT_VALID( this );
	return CPoint( 3, 3 );
}

bool CExtGridBaseWnd::OnGbwDataDndCanStart(
	const CExtGridHitTestInfo & htInfo
	)
{
	ASSERT_VALID( this );
bool bAlt = ( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 ) ? true : false;
bool bShift = ( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 ) ? true : false;
	if(		(!(	OnGbwDataDndIsAllowed()
			&&	htInfo.m_nColNo >= 0 && htInfo.m_nRowNo >= 0
			&&	(htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0
			))
		||	bShift
		||	bAlt
		)
		return false;
DWORD dwSelectionType = SelectionTypeGet();
	if( (dwSelectionType&__EGBS_SFB_MASK) == __EGBS_SFB_NONE )
		return false;
	m_htInfoDataDnd = htInfo;
CPoint ptNewFocus(htInfo.m_nColNo,htInfo.m_nRowNo);
	FocusSet( ptNewFocus, false, false, false, false );
	OnSwDoRedraw();
bool bCtrl = ( (::GetAsyncKeyState(VK_CONTROL)&0x8000) != 0 ) ? true : false;
bool bNoModifiers = (!bCtrl) && (!bAlt) && (!bShift);
bool bCtrlOnly = bCtrl && (!bAlt) && (!bShift);
bool bReplaceOldAreas =
		(	bCtrl
		&&	((dwSelectionType&__EGBS_SFM_MASK) != 0)
		)
		? false : true;
bool bCellSelected =
		SelectionGetForCell( ptNewFocus.x, ptNewFocus.y );
	if( bCellSelected && (bNoModifiers || bCtrlOnly) )
		return true;
	if( (! bCellSelected ) || bReplaceOldAreas )
	{
		SelectionSet( ptNewFocus, bReplaceOldAreas );
		if( (bNoModifiers /*|| bCtrlOnly*/) && (! SelectionIsEmpty() ) )
			return true;
		return false;
	} // if( (! bCellSelected ) || bReplaceOldAreas )
	if( SelectionIsEmpty() )
		return false;
	return true;
}

void CExtGridBaseWnd::OnGbwDataDndDo(
	const CExtGridHitTestInfo & htInfo
	)
{
	ASSERT_VALID( this );
	htInfo;
	TRACE2(
		"CExtGridBaseWnd::OnGbwDataDndDo(%d,%d)\n",
		htInfo.m_nColNo,
		htInfo.m_nRowNo
		);
}

void CExtGridBaseWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );

	switch( m_eMTT )
	{
	case __EMTT_SEL_OUTER_ROWS:
	case __EMTT_SEL_OUTER_COLUMNS:
		{
			DWORD dwSiwStyles = SiwGetStyle();
			CExtGridHitTestInfo htInfo( point );
			HitTest( htInfo, false, true );
			if( (htInfo.m_dwAreaFlags & __EGBWA_OUTER_CELLS) != 0 )
			{
				if(		m_eMTT == __EMTT_SEL_OUTER_ROWS
					&&	(htInfo.m_dwAreaFlags & __EGBWA_OUTER_H) != 0
					&&	(dwSiwStyles & __EGBS_SF_SELECT_OUTER_ROWS) != 0
					&&	htInfo.m_nRowNo >= 0
					&&	ColumnCountGet() > 0
					)
				{
					ASSERT( SelectionGetAreaCount() > 0 );
					CRect rcSelection( SelectionGet(false,SelectionGetAreaCount()-1) );
					if( rcSelection.bottom != htInfo.m_nRowNo )
					{
						rcSelection.bottom = htInfo.m_nRowNo;
						if( (dwSiwStyles & __EGBS_SFM_ROWS) == 0 )
							rcSelection.top = rcSelection.bottom;
						if( ! AutoFocusBottomRightGet() )
							FocusSet( CPoint(FocusGet().x, htInfo.m_nRowNo), false, false, false, false );
						SelectionSet( rcSelection, false, true, true );
					} // if( rcSelection.bottom != htInfo.m_nRowNo )
					return;
				}
				if(		m_eMTT == __EMTT_SEL_OUTER_COLUMNS
					&&	(htInfo.m_dwAreaFlags & __EGBWA_OUTER_V) != 0
					&&	(dwSiwStyles & __EGBS_SF_SELECT_OUTER_COLUMNS) != 0
					&&	htInfo.m_nColNo >= 0
					&&	RowCountGet() > 0
					)
				{
					ASSERT( SelectionGetAreaCount() > 0 );
					CRect rcSelection( SelectionGet(false,SelectionGetAreaCount()-1) );
					if( rcSelection.right != htInfo.m_nColNo )
					{
						rcSelection.right = htInfo.m_nColNo;
						if( (dwSiwStyles & __EGBS_SFM_COLUMNS) == 0 )
							rcSelection.left = rcSelection.right;
						if( ! AutoFocusBottomRightGet() )
							FocusSet( CPoint(htInfo.m_nColNo, FocusGet().y), false, false, false, false );
						SelectionSet( rcSelection, false, true, true );
					} // if( rcSelection.right != htInfo.m_nColNo )
					return;
				}
			} // if( (htInfo.m_dwAreaFlags & __EGBWA_OUTER_CELLS) != 0 ...
		} // cases __EMTT_SEL_OUTER_ROWS or __EMTT_SEL_OUTER_ROWS
		return;
	case __EMTT_SEL_INNER_ROWS:
	case __EMTT_SEL_INNER_COLUMNS:
		{
			DWORD dwSiwStyles = SiwGetStyle();
			CExtGridHitTestInfo htInfo( point );
			HitTest( htInfo, false, true );
			if( (htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0 )
			{
				if(		m_eMTT == __EMTT_SEL_INNER_ROWS
					&&	htInfo.m_nRowNo >= 0
					//&&	ColumnCountGet() > 0
					)
				{
					ASSERT( SelectionGetAreaCount() > 0 );
					CRect rcSelection( SelectionGet(false,SelectionGetAreaCount()-1) );
					if( rcSelection.bottom != htInfo.m_nRowNo )
					{
						rcSelection.bottom = htInfo.m_nRowNo;
						if( (dwSiwStyles & __EGBS_SFM_ROWS) == 0 )
							rcSelection.top = rcSelection.bottom;
						if( ! AutoFocusBottomRightGet() )
							FocusSet(
								CPoint(
									(htInfo.m_nColNo >= 0) ? htInfo.m_nColNo : FocusGet().x,
									htInfo.m_nRowNo
									),
								false, false, false, false
								);
						SelectionSet( rcSelection, false, true, true );
					} // if( rcSelection.bottom != htInfo.m_nRowNo )
					return;
				}
				if(		m_eMTT == __EMTT_SEL_INNER_COLUMNS
					&&	htInfo.m_nColNo >= 0
					//&&	RowCountGet() > 0
					)
				{
					ASSERT( SelectionGetAreaCount() > 0 );
					CRect rcSelection( SelectionGet(false,SelectionGetAreaCount()-1) );
					if( rcSelection.right != htInfo.m_nColNo )
					{
						rcSelection.right = htInfo.m_nColNo;
						if( (dwSiwStyles & __EGBS_SFM_COLUMNS) == 0 )
							rcSelection.left = rcSelection.right;
						if( ! AutoFocusBottomRightGet() )
							FocusSet(
								CPoint(
									htInfo.m_nColNo,
									(htInfo.m_nRowNo >= 0) ? htInfo.m_nRowNo : FocusGet().y
									),
								false, false, false, false
								);
						SelectionSet( rcSelection, false, true, true );
					} // if( rcSelection.right != htInfo.m_nColNo )
					return;
				}
			} // if( (htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0 )
		} // cases __EMTT_SEL_INNER_ROWS or __EMTT_SEL_INNER_COLUMNS
		return;
	case __EMTT_SEL_VAR_ROWS:
		{
			CExtGridHitTestInfo htInfo( point );
			HitTest( htInfo, false, true );
			if(		(	(htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0
					||	(	(htInfo.m_dwAreaFlags & __EGBWA_OUTER_CELLS) != 0
						&&	(htInfo.m_dwAreaFlags & __EGBWA_OUTER_H) != 0
						&&	(htInfo.m_dwAreaFlags & __EGBWA_OUTER_V) == 0
						)
					)
				&&	htInfo.m_nRowNo >= 0
				//&&	RowCountGet() > 0
				)
			{
				ASSERT( SelectionGetAreaCount() > 0 );
				CRect rcSelection( SelectionGet(false,SelectionGetAreaCount()-1) );
				if( rcSelection.bottom != htInfo.m_nRowNo )
				{
					rcSelection.bottom = htInfo.m_nRowNo;
					if( ! AutoFocusBottomRightGet() )
						FocusSet( CPoint(FocusGet().x, htInfo.m_nRowNo), false, false, false, false );
					SelectionSet( rcSelection, false, true, true );
				} // if( rcSelection.right != htInfo.m_nColNo )
			}
		} // case __EMTT_SEL_VAR_ROWS
		return;
	case __EMTT_SEL_VAR_COLUMNS:
		{
			CExtGridHitTestInfo htInfo( point );
			HitTest( htInfo, false, true );
			if(		(	(htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0
					||	(	(htInfo.m_dwAreaFlags & __EGBWA_OUTER_CELLS) != 0
						&&	(htInfo.m_dwAreaFlags & __EGBWA_OUTER_V) != 0
						&&	(htInfo.m_dwAreaFlags & __EGBWA_OUTER_H) == 0
						)
					)
				&&	htInfo.m_nColNo >= 0
				//&&	ColumnCountGet() > 0
				)
			{
				ASSERT( SelectionGetAreaCount() > 0 );
				CRect rcSelection( SelectionGet(false,SelectionGetAreaCount()-1) );
				if( rcSelection.right != htInfo.m_nColNo )
				{
					rcSelection.right = htInfo.m_nColNo;
					if( ! AutoFocusBottomRightGet() )
						FocusSet( CPoint(htInfo.m_nColNo, FocusGet().y), false, false, false, false );
					SelectionSet( rcSelection, false, true, true );
				} // if( rcSelection.right != htInfo.m_nColNo )
			}
		} // case __EMTT_SEL_VAR_COLUMNS
		return;
	case __EMTT_SEL_VAR_RANGE:
		{
			CExtGridHitTestInfo htInfo( point );
			HitTest( htInfo, false, true );
			if(		(htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0
				&&	htInfo.m_nColNo >= 0
				&&	htInfo.m_nRowNo >= 0
				//&&	ColumnCountGet() > 0
				//&&	RowCountGet() > 0
				)
			{
				ASSERT( SelectionGetAreaCount() > 0 );
				CRect rcSelection( SelectionGet(false,SelectionGetAreaCount()-1) );
				if(		rcSelection.right != htInfo.m_nColNo
					||	rcSelection.bottom != htInfo.m_nRowNo
					)
				{
					rcSelection.right = htInfo.m_nColNo;
					rcSelection.bottom = htInfo.m_nRowNo;
					if( ! AutoFocusBottomRightGet() )
						FocusSet( CPoint(htInfo.m_nColNo, htInfo.m_nRowNo), false, false, false, false );
					SelectionSet( rcSelection, false, true, true );
				} // if( rcSelection.right != htInfo.m_nColNo )
			}
		} // case __EMTT_SEL_VAR_RANGE
		return;
	case __EMTT_RESIZING_H:
	case __EMTT_RESIZING_V:
			OnGbwResizingStateUpdate( false, false, &point );
		return;
	case __EMTT_DATA_DND_STARTING:
		{
			CExtGridHitTestInfo htInfo( point );
			HitTest( htInfo, false, true );
			CPoint ptOffsetTest = OnGbwDataDndGetStartOffset();
			ASSERT( ptOffsetTest.x > 0 && ptOffsetTest.y > 0 );
			if(		(! m_htInfoDataDnd.IsHoverEqual(htInfo,false,false,false) )
				||	(abs((m_htInfoDataDnd.m_ptClient.x-htInfo.m_ptClient.x)) > ptOffsetTest.x)
				||	(abs((m_htInfoDataDnd.m_ptClient.y-htInfo.m_ptClient.y)) > ptOffsetTest.y)
				)
			{
				if( CExtMouseCaptureSink::GetCapture() == m_hWnd )
					CExtMouseCaptureSink::ReleaseCapture();
				OnGbwDataDndDo( m_htInfoDataDnd );
			}
		}
		return;
	} // switch( m_eMTT )

	if( CExtMouseCaptureSink::GetCapture() == m_hWnd )
	{
		CPoint ptScreen( point );
		ClientToScreen( &ptScreen );
		HWND hWndFromPoint = ::WindowFromPoint( ptScreen );
		if(		hWndFromPoint != m_hWnd
			&&	( hWndFromPoint != m_wndToolTip.GetSafeHwnd() || m_wndToolTip.GetSafeHwnd() == NULL )
			&&	( hWndFromPoint != m_wndContentExpand.GetSafeHwnd() || m_wndContentExpand.GetSafeHwnd() == NULL )
			)
		{
			OnGbwHoverRemove();
			return;
		} // if( hWndFromPoint != m_hWnd )
	} // if( CExtMouseCaptureSink::GetCapture() == m_hWnd )

	if( OnGbwHoverRecalc(&point) )
		return;

	CExtScrollItemWnd::OnMouseMove(nFlags, point);
}

CRect CExtGridBaseWnd::_ResizingTrackerRect( INT nExtentCalc )
{
	ASSERT_VALID( this );
	ASSERT( m_eMTT == __EMTT_RESIZING_H || m_eMTT == __EMTT_RESIZING_V );
	ASSERT(
			m_nTrackExtentMin >= 0
		&&	m_nTrackExtentMax >= 0
		&&	m_nTrackExtentMin < m_nTrackExtentMax
		&&	m_nTrackExtentCurrent >= 0
		&&	m_nTrackExtentMin <= m_nTrackExtentCurrent
		&&	m_nTrackExtentCurrent <= m_nTrackExtentMax
		&&	nExtentCalc >= 0
		&&	0 <= nExtentCalc // m_nTrackExtentMin <= nExtentCalc
		&&	nExtentCalc <= m_nTrackExtentMax
		);
CRect rcClient = OnSwGetClientRect();
	if( ! m_rcOuterT.IsRectEmpty() )
		rcClient.top = m_rcOuterT.top;
	if( ! m_rcOuterB.IsRectEmpty() )
		rcClient.bottom = m_rcOuterB.bottom;
	if( ! m_rcOuterL.IsRectEmpty() )
		rcClient.left = m_rcOuterL.left;
	if( ! m_rcOuterR.IsRectEmpty() )
		rcClient.right = m_rcOuterR.right;
CRect rcTracker( m_htInfoResizing.m_rcItem.TopLeft(), CSize(0,0) );
	if( m_eMTT == __EMTT_RESIZING_H )
	{ // if horizontal resizing
		rcTracker.top = rcClient.top;
		rcTracker.bottom = rcClient.bottom;
		if(		m_htInfoResizing.m_nColNo == 0
			&&	nExtentCalc == 0
			)
			rcTracker.right ++;
		else
			rcTracker.left --;
		rcTracker.OffsetRect( nExtentCalc, 0 );
	} // if horizontal resizing
	else
	{ // if vertical resizing
		rcTracker.left = rcClient.left;
		rcTracker.right = rcClient.right;
		if(		m_htInfoResizing.m_nRowNo == 0
			&&	nExtentCalc == 0
			)
			rcTracker.bottom ++;
		else
			rcTracker.top --;
		rcTracker.OffsetRect( 0, nExtentCalc );
	} // if vertical resizing
	return rcTracker;
}

void CExtGridBaseWnd::_ResizingTrackerDraw( const CRect & rcTracker )
{
	ASSERT_VALID( this );
	ASSERT( ! rcTracker.IsRectEmpty() );
CDC * pDC =
		GetDCEx(
			NULL,
			// DCX_WINDOW|
				DCX_CACHE|DCX_LOCKWINDOWUPDATE
				|DCX_CLIPSIBLINGS|DCX_CLIPCHILDREN
			);
CBrush * pBrushOld =
		pDC->SelectObject( CDC::GetHalftoneBrush() );
    pDC->PatBlt(
		rcTracker.left, rcTracker.top,
		rcTracker.Width(), rcTracker.Height(),
		PATINVERT
		);
    pDC->SelectObject( pBrushOld );
    ReleaseDC( pDC );
}

void CExtGridBaseWnd::OnGbwResizingStateUpdate(
	bool bInitial,
	bool bFinal,
	const CPoint * p_ptClient // = NULL
	)
{
	ASSERT_VALID( this );
	ASSERT( ! ( bInitial && bFinal ) );
	ASSERT(
			m_nTrackExtentMin >= 0
		&&	m_nTrackExtentMax >= 0
		&&	m_nTrackExtentMin < m_nTrackExtentMax
		&&	m_nTrackExtentCurrent >= 0
		&&	m_nTrackExtentMin <= m_nTrackExtentCurrent
		&&	m_nTrackExtentCurrent <= m_nTrackExtentMax
		);
	ASSERT( (m_htInfoResizing.m_dwAreaFlags&__EGBWA_NEAR_CELL_BORDER_LEFT) == 0 );
	ASSERT( (m_htInfoResizing.m_dwAreaFlags&__EGBWA_NEAR_CELL_BORDER_TOP) == 0 );
DWORD dwSiwStyle = SiwGetStyle();
DWORD dwDynamicTestMask =
		( m_eMTT == __EMTT_RESIZING_H )
			? __EGBS_DYNAMIC_RESIZING_H
			: __EGBS_DYNAMIC_RESIZING_V;
bool bDynamicResizing =
		( (dwSiwStyle&dwDynamicTestMask) != 0L )
			? true
			: false;
	if( bFinal )
	{
		if( ! bDynamicResizing )
		{
			// CRect rcTrackerMin = _ResizingTrackerRect( m_nTrackExtentMin );
			CRect rcTrackerMin = _ResizingTrackerRect( 0 );
			_ResizingTrackerDraw( rcTrackerMin );
			CRect rcTrackerOld = _ResizingTrackerRect( m_nTrackExtentCurrent );
			_ResizingTrackerDraw( rcTrackerOld );
		} // if( ! bDynamicResizing )
		INT nTrackExtentOld =
			( m_eMTT == __EMTT_RESIZING_H )
				? m_htInfoResizing.m_rcItem.Width()
				: m_htInfoResizing.m_rcItem.Height();
		if( nTrackExtentOld != m_nTrackExtentCurrent )
			OnGbwResizingStateApply(
				( m_eMTT == __EMTT_RESIZING_H )
					? true
					: false,
				( m_eMTT == __EMTT_RESIZING_H )
					? m_htInfoResizing.m_nColNo
					: m_htInfoResizing.m_nRowNo,
				m_nTrackExtentCurrent
				);
		return;
	} // if( bFinal )
CPoint point;
	if( p_ptClient != NULL )
		point = (*p_ptClient);
	else
	{
		if( ! ::GetCursorPos( &point ) )
			return;
		ScreenToClient( &point );
	} // else from if( p_ptClient != NULL )
	if( bInitial )
		m_ptTrackOffset =
			m_htInfoResizing.m_rcItem.BottomRight()
			- point
			;
INT nTrackExtentNew =
		( m_eMTT == __EMTT_RESIZING_H )
			? point.x
			: point.y;
	nTrackExtentNew -=
		( m_eMTT == __EMTT_RESIZING_H )
			? (m_htInfoResizing.m_rcItem.left - m_ptTrackOffset.x)
			: (m_htInfoResizing.m_rcItem.top - m_ptTrackOffset.y);
	if( nTrackExtentNew < m_nTrackExtentMin )
		nTrackExtentNew = m_nTrackExtentMin;
	if( nTrackExtentNew > m_nTrackExtentMax )
		nTrackExtentNew = m_nTrackExtentMax;
	if( bInitial )
	{
		OnGbwHoverRemove( false );
		UpdateWindow();
		_DoSetCursor();
		if( bDynamicResizing )
		{
			if( m_nTrackExtentCurrent != nTrackExtentNew )
			{
				m_nTrackExtentCurrent = nTrackExtentNew;
				OnGbwResizingStateApply(
					( m_eMTT == __EMTT_RESIZING_H )
						? true
						: false,
					( m_eMTT == __EMTT_RESIZING_H )
						? m_htInfoResizing.m_nColNo
						: m_htInfoResizing.m_nRowNo,
					m_nTrackExtentCurrent
					);
			} // if( m_nTrackExtentCurrent != nTrackExtentNew )
		} // if( bDynamicResizing )
		else
		{
			//CRect rcTrackerMin = _ResizingTrackerRect( m_nTrackExtentMin );
			CRect rcTrackerMin = _ResizingTrackerRect( 0 );
			_ResizingTrackerDraw( rcTrackerMin );
			m_nTrackExtentCurrent = nTrackExtentNew;
			CRect rcTrackerNew = _ResizingTrackerRect( nTrackExtentNew );
			_ResizingTrackerDraw( rcTrackerNew );
		} // else from if( bDynamicResizing )
		return;
	} // if( bInitial )
	if( nTrackExtentNew == m_nTrackExtentCurrent )
		return;
	if( bDynamicResizing )
	{
		if( m_nTrackExtentCurrent != nTrackExtentNew )
		{
			m_nTrackExtentCurrent = nTrackExtentNew;
			OnGbwResizingStateApply(
				( m_eMTT == __EMTT_RESIZING_H )
					? true
					: false,
				( m_eMTT == __EMTT_RESIZING_H )
					? m_htInfoResizing.m_nColNo
					: m_htInfoResizing.m_nRowNo,
				m_nTrackExtentCurrent
				);
		} // if( m_nTrackExtentCurrent != nTrackExtentNew )
	} // if( bDynamicResizing )
	else
	{
		CRect rcTrackerOld = _ResizingTrackerRect( m_nTrackExtentCurrent );
		CRect rcTrackerNew = _ResizingTrackerRect( nTrackExtentNew );
		m_nTrackExtentCurrent = nTrackExtentNew;
		_ResizingTrackerDraw( rcTrackerOld );
		_ResizingTrackerDraw( rcTrackerNew );
	} // else from if( bDynamicResizing )
}

void CExtGridBaseWnd::OnGbwResizingStateApply(
	bool bHorz,
	LONG nItemNo,
	INT nItemExtent
	)
{
	ASSERT_VALID( this );
	bHorz;
	nItemNo;
	nItemExtent;
	ASSERT( nItemNo >= 0 );
	ASSERT( nItemExtent >= 0 );
#ifdef _DEBUG
INT nDebugVerifyTrackExtentMin = 0, nDebugVerifyTrackExtentMax = 0;
	if( bHorz )
	{
		ASSERT( nItemNo < ColumnCountGet() );
		VERIFY(
			OnGbwCanResizeColumn(
				nItemNo,
				&nDebugVerifyTrackExtentMin,
				&nDebugVerifyTrackExtentMax
				)
			);
		ASSERT(
				nDebugVerifyTrackExtentMin < nDebugVerifyTrackExtentMax
			&&	nDebugVerifyTrackExtentMin <= nItemExtent
			&&	nItemExtent <= nDebugVerifyTrackExtentMax
			);
	} // if( bHorz )
	else
	{
		ASSERT( nItemNo < RowCountGet() );
		VERIFY(
			OnGbwCanResizeRow(
				nItemNo,
				&nDebugVerifyTrackExtentMin,
				&nDebugVerifyTrackExtentMax
				)
			);
		ASSERT(
				nDebugVerifyTrackExtentMin < nDebugVerifyTrackExtentMax
			&&	nDebugVerifyTrackExtentMin <= nItemExtent
			&&	nItemExtent <= nDebugVerifyTrackExtentMax
			);
	} // else from if( bHorz )
#endif // _DEBUG
}

bool CExtGridBaseWnd::OnGbwQueryTrackingCellMenu() const
{
	ASSERT_VALID( this );
	return false;
}

void CExtGridBaseWnd::OnTimer(UINT nIDEvent) 
{
	if( nIDEvent == m_nTimerIdPopupMenuWaiting )
	{
		if( CExtPopupMenuWnd::IsMenuTracking() )
			return;
		KillTimer( m_nTimerIdPopupMenuWaiting );
		OnGbwHoverRecalc();
		return;
	} // if( nIDEvent == m_nTimerIdPopupMenuWaiting )
	if( nIDEvent == m_nTimerIdDelayedExpand )
	{
		KillTimer( m_nTimerIdDelayedExpand );
		KillTimer( m_nTimerIdExpandWait );
		CPoint ptClient;
		DWORD dwSiwStyleEx = SiwGetStyleEx();
		bool bAlt = ( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 ) ? true : false;
		bool bCtrl = ( (::GetAsyncKeyState(VK_CONTROL)&0x8000) != 0 ) ? true : false;
		bool bShift = ( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 ) ? true : false;
		bool bMouseLeft = ( (::GetAsyncKeyState(VK_LBUTTON)&0x8000) != 0 ) ? true : false;
		bool bMouseRight = ( (::GetAsyncKeyState(VK_LBUTTON)&0x8000) != 0 ) ? true : false;
		bool bMouseMiddle = ( (::GetAsyncKeyState(VK_MBUTTON)&0x8000) != 0 ) ? true : false;
		if(		(dwSiwStyleEx&__EGBS_EX_CELL_TE_MASK) != 0
			&&	m_eMTT == __EMTT_NOTHING
			&&	( ! ( bAlt || bCtrl || bShift || bMouseLeft || bMouseRight || bMouseMiddle ) )
			&&	( ! OnGbwQueryTrackingCellMenu() )
			&&	( ! CExtPopupMenuWnd::IsMenuTracking() )
			&&	CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND( GetSafeHwnd() )
			&&	::GetCursorPos(&ptClient)
			)
		{
			ScreenToClient( &ptClient );
			CExtGridHitTestInfo htInfo( ptClient );
			HitTest( htInfo, false, true );
			if( m_htInfoExpand.IsHoverEqual( htInfo ) )
			{
				OnGbwExpand( htInfo, 0 );
				if( m_wndContentExpand.GetSafeHwnd() != NULL )
					SetTimer( m_nTimerIdExpandWait, 20, NULL );
				return;
			}
		}
		if( m_eMTT == __EMTT_NOTHING )
			OnGbwHoverRemove();
		return;
	} // if( nIDEvent == m_nTimerIdDelayedExpand )
	if( nIDEvent == m_nTimerIdExpandWait )
	{
		if( m_wndContentExpand.GetSafeHwnd() == NULL )
		{
			KillTimer( m_nTimerIdExpandWait );
			m_wndContentExpand.Deactivate();
			m_htInfoExpand.Empty();
			return;
		}
		CPoint point;
		if( ! ::GetCursorPos(&point) )
		{
			KillTimer( m_nTimerIdExpandWait );
			m_wndContentExpand.Deactivate();
			m_htInfoExpand.Empty();
			return;
		}
		HWND hWndFromPoint = ::WindowFromPoint( point );
		if(		hWndFromPoint != m_wndContentExpand.GetSafeHwnd()
			//&&	hWndFromPoint != m_hWnd
			)
		{
			KillTimer( m_nTimerIdExpandWait );
			m_wndContentExpand.Deactivate();
			m_htInfoExpand.Empty();
			return;
		}
		return;
	} // if( nIDEvent == m_nTimerIdExpandWait )
	if( nIDEvent == m_nTimerIdPressedButton )
		return;
	CExtScrollItemWnd::OnTimer(nIDEvent);
}

void CExtGridBaseWnd::OnGbwHoverRemove(
	bool bAllowReleaseCapture // = true
	)
{
	ASSERT_VALID( this );
	KillTimer( m_nTimerIdPopupMenuWaiting );
	KillTimer( m_nTimerIdDelayedExpand );
	KillTimer( m_nTimerIdExpandWait );
	KillTimer( m_nTimerIdPressedButton );

	if(		m_wndToolTip.GetSafeHwnd() != NULL
		&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
		)
		m_wndToolTip.DelTool( this, 1 );
	CWnd::CancelToolTips();
	m_wndContentExpand.Deactivate();
	m_htInfoExpand.Empty();

bool bHoverEmptyPrevious = m_htInfoHover.IsHoverEmpty();
	if( ! bHoverEmptyPrevious )
	{ // if previous cell has lost its hover
		OnGbwHoverCell(
			m_htInfoHover,
			m_htInfoHover,
			false,
			true
			);
		_InvalidateHoveredArea( m_htInfoHover );
		m_htInfoHover.Empty();
		OnGbwSetCursor( m_htInfoHover );
	} // if previous cell has lost its hover
	if( bAllowReleaseCapture && CExtMouseCaptureSink::GetCapture() == m_hWnd )
		CExtMouseCaptureSink::ReleaseCapture();
}

bool CExtGridBaseWnd::OnGbwHoverRecalc(
	const POINT * pPtClient // = NULL
	)
{
	ASSERT_VALID( this );
bool bHoverEmptyPrevious = m_htInfoHover.IsHoverEmpty();
bool bTrackingCellMenu = OnGbwQueryTrackingCellMenu();
	if( bTrackingCellMenu )
		return false;
	if( CExtPopupMenuWnd::IsMenuTracking() )
	{
		SetTimer( m_nTimerIdPopupMenuWaiting, 10, NULL );
		return false;
	}
	if( ! CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND(
			GetSafeHwnd()
			)
		)
	{ // if hover is disabled (grid is not inside active window or popup menu is tracking)
		OnGbwHoverRemove();
		return (!bHoverEmptyPrevious);
	} // if hover is disabled (grid is not inside active window or popup menu is tracking)
CPoint ptClient;
	if( pPtClient != NULL )
		ptClient = *pPtClient;
	else
	{
		if( ! ::GetCursorPos(&ptClient) )
			return false;
		ScreenToClient( &ptClient );
	} // if( pPtClient != NULL )
CPoint ptScreen( ptClient );
	ClientToScreen( &ptScreen );
HWND hWndFromPoint = ::WindowFromPoint( ptScreen );
	if(		hWndFromPoint != m_hWnd
		&&	( hWndFromPoint != m_wndToolTip.GetSafeHwnd() || m_wndToolTip.GetSafeHwnd() == NULL )
		&&	( hWndFromPoint != m_wndContentExpand.GetSafeHwnd() || m_wndContentExpand.GetSafeHwnd() == NULL )
		)
	{
		OnGbwHoverRemove();
		return (!bHoverEmptyPrevious);
	} // if( hWndFromPoint != m_hWnd )
DWORD dwSiwStyleEx = SiwGetStyleEx();
	if( ( dwSiwStyleEx &
			(__EGBS_EX_HVI_EVENT_CELLS
			|__EGBS_EX_HVO_EVENT_CELLS
			|__EGBS_EX_CELL_TE_MASK)
			) == 0
		)
		return false;
CExtGridHitTestInfo htInfo( ptClient );
	HitTest( htInfo, false, true );
bool bHoverEmptyCurrent = htInfo.IsHoverEmpty();
//bool bHoverEmptyPrevious = m_htInfoHover.IsHoverEmpty();
	if( bHoverEmptyPrevious && bHoverEmptyCurrent )
		return false;

	// first: analyze tooltips and expanding
	if(		(dwSiwStyleEx & __EGBS_EX_CELL_TE_MASK) != 0
		&&	m_eMTT == __EMTT_NOTHING
		&&	(! bHoverEmptyCurrent )
		&&	( ! htInfo.IsHoverEqual( m_htInfoHover ) )
		)
	{ // if tooltips/expanding allowed and new item is hovered
		bool bExpandItem = false, bTooltipItem = false;
		INT nColType = htInfo.GetInnerOuterTypeOfColumn();
		INT nRowType = htInfo.GetInnerOuterTypeOfRow();
		if( nColType < 0 )
		{
			if( nRowType == 0 )
			{
				if( (dwSiwStyleEx & __EGBS_EX_CELL_EXPANDING_OUTER_T) != 0 )
					bExpandItem = true;
				else if( (dwSiwStyleEx & __EGBS_EX_CELL_TOOLTIPS_OUTER_T) != 0 )
					bTooltipItem = true;
			} // if( nRowType == 0 )
		} // if( nColType < 0 )
		else if( nColType > 0 )
		{
			if( nRowType == 0 )
			{
				if( (dwSiwStyleEx & __EGBS_EX_CELL_EXPANDING_OUTER_B) != 0 )
					bExpandItem = true;
				else if( (dwSiwStyleEx & __EGBS_EX_CELL_TOOLTIPS_OUTER_B) != 0 )
					bTooltipItem = true;
			} // if( nRowType == 0 )
		} // else if( nColType > 0 )
		else
		{ // nColType is zero
			if( nRowType < 0 )
			{
				if( (dwSiwStyleEx & __EGBS_EX_CELL_EXPANDING_OUTER_L) != 0 )
					bExpandItem = true;
				else if( (dwSiwStyleEx & __EGBS_EX_CELL_TOOLTIPS_OUTER_L) != 0 )
					bTooltipItem = true;
			} // if( nRowType < 0 )
			else if( nRowType > 0 )
			{
				if( (dwSiwStyleEx & __EGBS_EX_CELL_EXPANDING_OUTER_R) != 0 )
					bExpandItem = true;
				else if( (dwSiwStyleEx & __EGBS_EX_CELL_TOOLTIPS_OUTER_R) != 0 )
					bTooltipItem = true;
			} // else if( nRowType > 0 )
			else
			{ // nRowType is zero
				if( (dwSiwStyleEx & __EGBS_EX_CELL_EXPANDING_INNER) != 0 )
					bExpandItem = true;
				else if( (dwSiwStyleEx & __EGBS_EX_CELL_TOOLTIPS_INNER) != 0 )
					bTooltipItem = true;
			} // nRowType is zero
		} // nColType is zero

		if( bExpandItem )
		{
			if(		m_wndToolTip.GetSafeHwnd() != NULL
				&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
				)
				m_wndToolTip.DelTool( this, 1 );
			CWnd::CancelToolTips();
			if( ! m_htInfoExpand.IsHoverEqual( htInfo ) )
			{
				m_htInfoExpand = htInfo;
				OnGbwExpand( htInfo, 200 );
			} // if( ! m_htInfoExpand.IsHoverEqual( htInfo ) )
		} // if( bExpandItem )
		else if( bTooltipItem )
		{
			m_wndContentExpand.Deactivate();
			m_htInfoExpand.Empty();
			OnGbwTooltip( htInfo );
		} // else if( bTooltipItem )
	} // if tooltips/expanding allowed and new item is hovered
	else if(	bHoverEmptyCurrent
			||	m_eMTT != __EMTT_NOTHING
//			||	(! bHoverEmptyCurrent
//				&&	(htInfo.m_dwAreaFlags & __EGBWA_NEAR_CELL_BORDER_SIDE_MASK) != 0
//				)
			)
	{
		if(		m_wndToolTip.GetSafeHwnd() != NULL
			&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
			)
			m_wndToolTip.DelTool( this, 1 );
		CWnd::CancelToolTips();
		m_wndContentExpand.Deactivate();
		m_htInfoExpand.Empty();
	} // else if( bHoverEmptyCurrent ...

	// second: continue only with the hover analysis
	if( ( dwSiwStyleEx & (__EGBS_EX_HVI_EVENT_CELLS|__EGBS_EX_HVO_EVENT_CELLS) ) == 0 )
		return false;

	if(		htInfo.IsHoverEqual( m_htInfoHover )
		&&	( ! bHoverEmptyCurrent )
		)
	{ // if hover on the same cell
		OnGbwHoverCell(
			m_htInfoHover,
			htInfo,
			false,
			false
			);
	} // if hover on the same cell
	else
	{ // if hover changed
		if( ! bHoverEmptyPrevious )
		{ // if previous cell has lost its hover
			OnGbwHoverCell(
				m_htInfoHover,
				htInfo,
				false,
				true
				);
		} // if previous cell has lost its hover
#ifdef _DEBUG
		bool bDebugOuterCellAtLeftRight;
#endif // _DEBUG
		if( ! bHoverEmptyCurrent )
		{ // if new hover gained
			if(		(dwSiwStyleEx & __EGBS_EX_HVI_EVENT_CELLS) != 0
				&&	htInfo.IsHoverInner()
				)
			{ // if hit test is on inner cell
				ASSERT( htInfo.m_nColNo >= 0 &&	htInfo.m_nRowNo >= 0 );
				OnGbwHoverCell(
					m_htInfoHover,
					htInfo,
					true,
					false
					);
			} // if hit test is on inner cell
			else if(	(dwSiwStyleEx & __EGBS_EX_HVO_EVENT_CELLS) != 0
						&&	htInfo.IsHoverOuter(
#ifdef _DEBUG
								& bDebugOuterCellAtLeftRight
#endif // _DEBUG
								)
					)
			{ // if hit test is on outer cell
				ASSERT(
						(	bDebugOuterCellAtLeftRight
						&&	(htInfo.m_dwAreaFlags & __EGBWA_OUTER_H) != 0
						&&	(htInfo.m_dwAreaFlags & __EGBWA_OUTER_V) == 0
						&&	htInfo.m_nRowNo >= 0
						)
					||	(	( ! bDebugOuterCellAtLeftRight )
						&&	(htInfo.m_dwAreaFlags & __EGBWA_OUTER_H) == 0
						&&	(htInfo.m_dwAreaFlags & __EGBWA_OUTER_V) != 0
						&&	htInfo.m_nColNo >= 0
						)
					);
				OnGbwHoverCell(
					m_htInfoHover,
					htInfo,
					true,
					false
					);
			} // if hit test is on outer cell
		} // if new hover gained
	} // if hover changed
	m_htInfoHover = htInfo;
	//OnGbwSetCursor( m_htInfoHover );
	HitTest( htInfo, true, true );
	OnGbwSetCursor( htInfo );
	return true;
}

void CExtGridBaseWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	if( OnGbwAnalyzeCellMouseClickEvent(VK_LBUTTON,2,nFlags,point) )
		return;
	CExtScrollItemWnd::OnLButtonDblClk(nFlags, point);
}
void CExtGridBaseWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	if( m_eMTT != __EMTT_NOTHING )
	{
		SendMessage( WM_CANCELMODE );
		if( m_eMTT != __EMTT_NOTHING )
			return;
	} // if( m_eMTT != __EMTT_NOTHING )
	if(		m_nMouseActivateCode == MA_ACTIVATE
		||	m_nMouseActivateCode == MA_ACTIVATEANDEAT
		)
	{
		if( GetFocus() != this )
			SetFocus();
	}
	if( OnGbwAnalyzeCellMouseClickEvent(VK_LBUTTON,1,nFlags,point) )
		return;
LONG nColumnCount = ColumnCountGet();
LONG nRowCount = RowCountGet();
CExtGridHitTestInfo htInfo( point );
	HitTest( htInfo, true, true );
DWORD dwSiwStyles = SiwGetStyle();
	if( (dwSiwStyles & __EGBS_RESIZING_CELLS_MASK) != 0 )
	{
		if(		htInfo.m_nColNo >= 0 && htInfo.m_nRowNo >= 0
			&&	(htInfo.m_dwAreaFlags & __EGBWA_NEAR_CELL_BORDER_SIDE_MASK) != 0
			)
		{
			m_nTrackExtentMin
				= m_nTrackExtentMax
				= m_nTrackExtentCurrent
				= 0;
			if( (htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0 )
			{
				if(		(htInfo.m_dwAreaFlags & __EGBWA_NEAR_CELL_BORDER_V) != 0
					&&	(dwSiwStyles & __EGBS_RESIZING_CELLS_INNER_V) != 0
					&&	htInfo.m_nRowNo >= 0
					&&	OnGbwCanResizeRow(
							htInfo.m_nRowNo,
							&m_nTrackExtentMin,
							&m_nTrackExtentMax
							)
					)
				{
					m_nTrackExtentCurrent = htInfo.m_rcItem.Height();
					ASSERT(
							m_nTrackExtentMin >= 0
						&&	m_nTrackExtentMax >= 0
						&&	m_nTrackExtentMin < m_nTrackExtentMax
						&&	m_nTrackExtentCurrent >= 0
						&&	m_nTrackExtentMin <= m_nTrackExtentCurrent
						&&	m_nTrackExtentCurrent <= m_nTrackExtentMax
						);
					// start row resizing
					_DoSetCursor();
					m_htInfoResizing = htInfo;
//					m_htInfoResizing.m_ptClient.y = htInfo.m_rcItem.top - 1;
//					HitTest( m_htInfoResizing, true, true );
					m_eMTT = __EMTT_RESIZING_V;
					CExtMouseCaptureSink::SetCapture( m_hWnd );
					OnGbwResizingStateUpdate( true, false, &point );
					return;
				}
				if(		(htInfo.m_dwAreaFlags & __EGBWA_NEAR_CELL_BORDER_H) != 0
					&&	(dwSiwStyles & __EGBS_RESIZING_CELLS_INNER_H) != 0
					&&	htInfo.m_nColNo >= 0
					&&	OnGbwCanResizeColumn(
							htInfo.m_nColNo,
							&m_nTrackExtentMin,
							&m_nTrackExtentMax
							)
					)
				{
					m_nTrackExtentCurrent = htInfo.m_rcItem.Width();
					ASSERT(
							m_nTrackExtentMin >= 0
						&&	m_nTrackExtentMax >= 0
						&&	m_nTrackExtentMin < m_nTrackExtentMax
						&&	m_nTrackExtentCurrent >= 0
						&&	m_nTrackExtentMin <= m_nTrackExtentCurrent
						&&	m_nTrackExtentCurrent <= m_nTrackExtentMax
						);
					// start column resizing
					_DoSetCursor();
					m_htInfoResizing = htInfo;
//					m_htInfoResizing.m_ptClient.x = htInfo.m_rcItem.left - 1;
//					HitTest( m_htInfoResizing, true, true );
					m_eMTT = __EMTT_RESIZING_H;
					CExtMouseCaptureSink::SetCapture( m_hWnd );
					OnGbwResizingStateUpdate( true, false, &point );
					return;
				}
			} // if( (htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0 )
			else if( (htInfo.m_dwAreaFlags & __EGBWA_OUTER_CELLS) != 0 )
			{
				if(		(htInfo.m_dwAreaFlags & __EGBWA_NEAR_CELL_BORDER_V) != 0
					&&	(htInfo.m_dwAreaFlags & __EGBWA_OUTER_H	) != 0
					&&	(dwSiwStyles & __EGBS_RESIZING_CELLS_OUTER_V) != 0
					&&	htInfo.m_nRowNo >= 0
					&&	OnGbwCanResizeRow(
							htInfo.m_nRowNo,
							&m_nTrackExtentMin,
							&m_nTrackExtentMax
							)
					)
				{
					m_nTrackExtentCurrent = htInfo.m_rcItem.Height();
					ASSERT(
							m_nTrackExtentMin >= 0
						&&	m_nTrackExtentMax >= 0
						&&	m_nTrackExtentMin < m_nTrackExtentMax
						&&	m_nTrackExtentCurrent >= 0
						&&	m_nTrackExtentMin <= m_nTrackExtentCurrent
						&&	m_nTrackExtentCurrent <= m_nTrackExtentMax
						);
					// start row resizing
					_DoSetCursor();
					m_htInfoResizing = htInfo;
//					m_htInfoResizing.m_ptClient.y = htInfo.m_rcItem.top - 1;
//					HitTest( m_htInfoResizing, true, true );
					m_eMTT = __EMTT_RESIZING_V;
					CExtMouseCaptureSink::SetCapture( m_hWnd );
					OnGbwResizingStateUpdate( true, false, &point );
					return;
				}
				if(		(htInfo.m_dwAreaFlags & __EGBWA_NEAR_CELL_BORDER_H) != 0
					&&	(htInfo.m_dwAreaFlags & __EGBWA_OUTER_V	) != 0
					&&	(dwSiwStyles & __EGBS_RESIZING_CELLS_OUTER_H) != 0
					&&	htInfo.m_nColNo >= 0
					&&	OnGbwCanResizeColumn(
							htInfo.m_nColNo,
							&m_nTrackExtentMin,
							&m_nTrackExtentMax
							)
					)
				{
					m_nTrackExtentCurrent = htInfo.m_rcItem.Width();
					ASSERT(
							m_nTrackExtentMin >= 0
						&&	m_nTrackExtentMax >= 0
						&&	m_nTrackExtentMin < m_nTrackExtentMax
						&&	m_nTrackExtentCurrent >= 0
						&&	m_nTrackExtentMin <= m_nTrackExtentCurrent
						&&	m_nTrackExtentCurrent <= m_nTrackExtentMax
						);
					// start column resizing
					_DoSetCursor();
					m_htInfoResizing = htInfo;
//					m_htInfoResizing.m_ptClient.x = htInfo.m_rcItem.left - 1;
//					HitTest( m_htInfoResizing, true, true );
					m_eMTT = __EMTT_RESIZING_H;
					CExtMouseCaptureSink::SetCapture( m_hWnd );
					OnGbwResizingStateUpdate( true, false, &point );
					return;
				}
			} // else if( (htInfo.m_dwAreaFlags & __EGBWA_OUTER_CELLS) != 0 )
		} // if( htInfo.m_nColNo >= 0 && htInfo.m_nRowNo >= 0 ...
	} // if( (dwSiwStyles & __EGBS_RESIZING_CELLS_MASK) != 0 )

	HitTest( htInfo, false, true );

	if( OnGbwDataDndCanStart( htInfo ) )
	{
		m_eMTT = __EMTT_DATA_DND_STARTING;
		CExtMouseCaptureSink::SetCapture( m_hWnd );
		return;
	} // if( OnGbwDataDndCanStart( htInfo ) )

	if(		(dwSiwStyles & __EGBS_SFB_MASK) != __EGBS_SFB_NONE
		&&	( htInfo.m_nColNo >= 0 || htInfo.m_nRowNo >= 0 )
		)
	{ // if any selection mode can be tracked
		INT nAreaCount = SelectionGetAreaCount();
		bool bAlt = ( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 ) ? true : false;
		bool bCtrl = ( (::GetAsyncKeyState(VK_CONTROL)&0x8000) != 0 ) ? true : false;
		bool bShift = ( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 ) ? true : false;
		bool bCtrlOnly = bCtrl && (!bAlt) && (!bShift);
		bool bShiftOnly = (!bCtrl) && (!bAlt) && bShift;
		bool bNoModifiers = (!bCtrl) && (!bAlt) && (!bShift);
		bool bMultiAreaSelection = MultiAreaSelectionGet();
		bool bReplaceOldSelection =
				(!bMultiAreaSelection)
			||	(!(bCtrlOnly || bShiftOnly));
		if( bNoModifiers )
		{
			if( OnGbwAnalyzeCellMouseClickEvent(VK_LBUTTON,3,nFlags,point) )
				return;
		}

		if( (htInfo.m_dwAreaFlags & __EGBWA_OUTER_CELLS) != 0 )
		{
			if(		(htInfo.m_dwAreaFlags & __EGBWA_OUTER_H) != 0
				&&	(dwSiwStyles & __EGBS_SF_SELECT_OUTER_ROWS) != 0
				&&	htInfo.m_nRowNo >= 0
				&&	nColumnCount > 0
				&&	(	(dwSiwStyles & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_ROWS
					||	(	(dwSiwStyles & __EGBS_SFB_MASK) == __EGBS_SFB_CELLS
						&&	(dwSiwStyles & __EGBS_SFM_ROWS) != 0
						)
					)
				)
			{
				CRect rcSelection( nColumnCount - 1, htInfo.m_nRowNo, 0, htInfo.m_nRowNo );
				if( (htInfo.m_dwAreaFlags & __EGBWA_OUTER_RIGHT ) != 0 )
				{
					LONG n = rcSelection.right;
					rcSelection.right = rcSelection.left;
					rcSelection.left = n;
				} // if( (htInfo.m_dwAreaFlags & __EGBWA_OUTER_RIGHT ) != 0 )
				if(		(dwSiwStyles & __EGBS_SFM_ROWS) != 0
					&&	bShiftOnly
					&&	nAreaCount > 0
					)
				{
					CRect rcSelection = SelectionGet( false, nAreaCount - 1 );
					if( rcSelection.bottom != htInfo.m_nRowNo )
					{
						rcSelection.bottom = htInfo.m_nRowNo;
						FocusSet( CPoint(FocusGet().x, htInfo.m_nRowNo), false, false, false, false );
						VERIFY( SelectionSetAt( nAreaCount - 1, rcSelection, false ) );
						EnsureVisibleRow( htInfo.m_nRowNo, true );
					}
					return;
				}
				if( ! AutoFocusBottomRightGet() )
					FocusSet( CPoint(FocusGet().x, htInfo.m_nRowNo), false, false, false, false );
				SelectionSet( rcSelection, bReplaceOldSelection, false, false );
				EnsureVisibleRow( htInfo.m_nRowNo, true );
				if( ! LbExtSelectionGet() )
					return;
				if( (dwSiwStyles & __EGBS_SFM_ROWS) != 0 )
				{
					m_eMTT = __EMTT_SEL_OUTER_ROWS;
					CExtMouseCaptureSink::SetCapture( m_hWnd );
					_DoSetCursor();
				}
				return;
			}
			if(		(htInfo.m_dwAreaFlags & __EGBWA_OUTER_V) != 0
				&&	(dwSiwStyles & __EGBS_SF_SELECT_OUTER_COLUMNS) != 0
				&&	htInfo.m_nColNo >= 0
				&&	nRowCount > 0
				&&	(	(dwSiwStyles & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_COLUMNS
					||	(	(dwSiwStyles & __EGBS_SFB_MASK) == __EGBS_SFB_CELLS
						&&	(dwSiwStyles & __EGBS_SFM_COLUMNS) != 0
						)
					)
				)
			{
				CRect rcSelection( htInfo.m_nColNo, nRowCount - 1, htInfo.m_nColNo, 0 );
				if( (htInfo.m_dwAreaFlags & __EGBWA_OUTER_BOTTOM ) != 0 )
				{
					LONG n = rcSelection.bottom;
					rcSelection.bottom = rcSelection.top;
					rcSelection.top = n;
				} // if( (htInfo.m_dwAreaFlags & __EGBWA_OUTER_BOTTOM ) != 0 )
				if(		(dwSiwStyles & __EGBS_SFM_COLUMNS) != 0
					&&	bShiftOnly
					&&	nAreaCount > 0
					)
				{
					CRect rcSelection = SelectionGet( false, nAreaCount - 1 );
					if( rcSelection.right != htInfo.m_nColNo )
					{
						rcSelection.right = htInfo.m_nColNo;
						FocusSet( CPoint(htInfo.m_nColNo, FocusGet().y), false, false, false, false );
						VERIFY( SelectionSetAt( nAreaCount - 1, rcSelection, false ) );
						EnsureVisibleColumn( htInfo.m_nColNo, true );
					}
					return;
				}
				if( ! AutoFocusBottomRightGet() )
					FocusSet( CPoint(htInfo.m_nColNo, FocusGet().y), false, false, false, false );
				SelectionSet( rcSelection, bReplaceOldSelection, false, false );
				EnsureVisibleColumn( htInfo.m_nColNo, true );
				if( ! LbExtSelectionGet() )
					return;
				if( (dwSiwStyles & __EGBS_SFM_COLUMNS) != 0 )
				{
					m_eMTT = __EMTT_SEL_OUTER_COLUMNS;
					CExtMouseCaptureSink::SetCapture( m_hWnd );
					_DoSetCursor();
				}
				return;
			}
		} // if( (htInfo.m_dwAreaFlags & __EGBWA_OUTER_CELLS) != 0 ...

		if( (htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0 )
		{
			DWORD dwScrollTypeH = SiwScrollTypeHGet();
			DWORD dwScrollTypeV = SiwScrollTypeVGet();
			if( (dwSiwStyles & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_COLUMNS )
			{
				if( htInfo.m_nColNo >= 0 /*&& nColumnCount > 0*/ )
				{
					if(		(dwSiwStyles & __EGBS_SFM_COLUMNS) != 0
						&&	OnGbwDataDndIsAllowed()
						&&	(	bNoModifiers
							||	(	bCtrlOnly
								&&	SelectionGetForCell( htInfo.m_nColNo, 0 )
								&&	(!bReplaceOldSelection)
								)
							)
						)
						return; // process in WM_LBUTTONUP
					if(		(dwSiwStyles & __EGBS_SFM_COLUMNS) != 0
						&&	bShiftOnly
						&&	nAreaCount > 0
						)
					{
						CRect rcSelection = SelectionGet( false, nAreaCount - 1 );
						if( rcSelection.right != htInfo.m_nColNo )
						{
							rcSelection.right = htInfo.m_nColNo;
							FocusSet( CPoint(htInfo.m_nColNo, htInfo.m_nRowNo), false, false, false, false );
							VERIFY( SelectionSetAt( nAreaCount - 1, rcSelection, false ) );
							EnsureVisibleColumn( htInfo.m_nColNo, true );
						}
						return;
					}
					if( ! AutoFocusBottomRightGet() )
						FocusSet( CPoint(htInfo.m_nColNo, htInfo.m_nRowNo), false, false, false, false );
					CRect rcSelection( htInfo.m_nColNo, 0, htInfo.m_nColNo, nRowCount-1 );
					SelectionSet( rcSelection, bReplaceOldSelection, false, false );
					EnsureVisibleColumn( htInfo.m_nColNo, true );
					if( ! LbExtSelectionGet() )
						return;
					if( (dwSiwStyles & __EGBS_SFM_COLUMNS) != 0 )
					{
						m_eMTT = __EMTT_SEL_INNER_COLUMNS;
						CExtMouseCaptureSink::SetCapture( m_hWnd );
						_DoSetCursor();
					}
				} // if( htInfo.m_nColNo >= 0 /*&& nColumnCount > 0*/ )
				return;
			} // if( (dwSiwStyles & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_COLUMNS )
			else if( (dwSiwStyles & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_ROWS )
			{
				if( htInfo.m_nRowNo >= 0 /*&& nRowCount > 0*/ )
				{
					if(		(dwSiwStyles & __EGBS_SFM_ROWS) != 0
						&&	OnGbwDataDndIsAllowed()
						&&	(	bNoModifiers
							||	(	bCtrlOnly
								&&	SelectionGetForCell( 0, htInfo.m_nRowNo )
								&&	(!bReplaceOldSelection)
								)
							)
						)
						return; // process in WM_LBUTTONUP
					if(		(dwSiwStyles & __EGBS_SFM_ROWS) != 0
						&&	bShiftOnly
						&&	nAreaCount > 0
						)
					{
						CRect rcSelection = SelectionGet( false, nAreaCount - 1 );
						if( rcSelection.bottom != htInfo.m_nRowNo )
						{
							rcSelection.bottom = htInfo.m_nRowNo;
							FocusSet( CPoint(htInfo.m_nColNo, htInfo.m_nRowNo), false, false, false, false );
							VERIFY( SelectionSetAt( nAreaCount - 1, rcSelection, false ) );
							EnsureVisibleRow( htInfo.m_nRowNo, true );
						}
						return;
					}
					if( ! AutoFocusBottomRightGet() )
						FocusSet( CPoint(htInfo.m_nColNo, htInfo.m_nRowNo), false, false, false, false );
					CRect rcSelection( 0, htInfo.m_nRowNo, nColumnCount-1, htInfo.m_nRowNo );
					SelectionSet( rcSelection, bReplaceOldSelection, false, false );
					EnsureVisibleRow( htInfo.m_nRowNo, true );
					if( ! LbExtSelectionGet() )
						return;
					if( (dwSiwStyles & __EGBS_SFM_ROWS) != 0 )
					{
						m_eMTT = __EMTT_SEL_INNER_ROWS;
						CExtMouseCaptureSink::SetCapture( m_hWnd );
						_DoSetCursor();
					}
				} // if( htInfo.m_nRowNo >= 0 /*&& nRowCount > 0*/ )
				return;
			} // else if( (dwSiwStyles & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_ROWS )
			else
			{
				ASSERT( (dwSiwStyles & __EGBS_SFB_MASK) == __EGBS_SFB_CELLS );
				if(		0 <= htInfo.m_nColNo && (htInfo.m_nColNo < nColumnCount || dwScrollTypeH == __ESIW_ST_VIRTUAL )
					&&	0 <= htInfo.m_nRowNo && (htInfo.m_nRowNo < nRowCount || dwScrollTypeV == __ESIW_ST_VIRTUAL )
					)
				{ // if column and row numbers specified
					ASSERT( m_eMTT == __EMTT_NOTHING );
					CPoint ptNewFocus( htInfo.m_nColNo, htInfo.m_nRowNo );
					if( bReplaceOldSelection )
						FocusSet( ptNewFocus, true, true, true, true );
					else
					{
						if(		(dwSiwStyles & (__EGBS_SFM_ROWS|__EGBS_SFM_COLUMNS)) != 0
							&&	OnGbwDataDndIsAllowed()
							&&	(	bNoModifiers
								||	(	bCtrlOnly
									&&	SelectionGetForCell( htInfo.m_nColNo, htInfo.m_nRowNo )
									&&	(!bReplaceOldSelection)
									)
								)
							)
							return; // process in WM_LBUTTONUP
						if(		(dwSiwStyles & (__EGBS_SFM_ROWS|__EGBS_SFM_COLUMNS)) != 0
							&&	bShiftOnly
							&&	nAreaCount > 0
							)
						{
							CRect rcSelection = SelectionGet( false, nAreaCount - 1 );
							if(		(	rcSelection.bottom != htInfo.m_nRowNo
									&&	(dwSiwStyles & __EGBS_SFM_ROWS) != 0
									)
								||	(	rcSelection.right != htInfo.m_nColNo
									&&	(dwSiwStyles & __EGBS_SFM_COLUMNS) != 0
									)
								)
							{
								CPoint ptFocus = FocusGet();
								if( (dwSiwStyles & __EGBS_SFM_ROWS) != 0 )
								{
									rcSelection.bottom = htInfo.m_nRowNo;
									ptFocus.y = htInfo.m_nRowNo;
								}
								if( (dwSiwStyles & __EGBS_SFM_COLUMNS) != 0 )
								{
									rcSelection.right = htInfo.m_nColNo;
									ptFocus.x = htInfo.m_nColNo;
								}
								FocusSet( ptFocus, false, false, false, false );
								VERIFY( SelectionSetAt( nAreaCount - 1, rcSelection, true ) );
							}
							return;
						}
						if( ! AutoFocusBottomRightGet() )
							FocusSet( ptNewFocus, true, true, false, false );
						SelectionSet( ptNewFocus, false, false, true );
					} // else from if( bReplaceOldSelection )
					if( ! LbExtSelectionGet() )
						return;
					switch( (dwSiwStyles & (__EGBS_SFM_COLUMNS|__EGBS_SFM_ROWS) ) )
					{
					case 0L:
						return;
					case __EGBS_SFM_COLUMNS:
						m_eMTT = __EMTT_SEL_VAR_COLUMNS;
						break;
					case __EGBS_SFM_ROWS:
						m_eMTT = __EMTT_SEL_VAR_ROWS;
						break;
					case __EGBS_SFM_COLUMNS|__EGBS_SFM_ROWS:
						m_eMTT = __EMTT_SEL_VAR_RANGE;
						break;
#ifdef _DEBUG
					default:
						ASSERT( FALSE );
						break;
#endif // _DEBUG
					} // switch( (dwSiwStyles & (__EGBS_SFM_COLUMNS|__EGBS_SFM_ROWS) ) )
					ASSERT( m_eMTT != __EMTT_NOTHING );
					CExtMouseCaptureSink::SetCapture( m_hWnd );
					_DoSetCursor();
					return;
				} // if column and row numbers specified
			} // else from else if( (dwSiwStyles & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_ROWS )
			return;
		} // if( (htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0 )

	} // if any selection mode can be tracked

	CExtScrollItemWnd::OnLButtonDown(nFlags, point);
}
void CExtGridBaseWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );

	if( OnGbwDataDndIsAllowed() && m_eMTT == __EMTT_DATA_DND_STARTING )
	{
		SendMessage( WM_CANCELMODE );
		bool bAlt = ( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 ) ? true : false;
		bool bCtrl = ( (::GetAsyncKeyState(VK_CONTROL)&0x8000) != 0 ) ? true : false;
		bool bShift = ( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 ) ? true : false;
		bool bCtrlOnly = bCtrl && (!bAlt) && (!bShift);
		bool bShiftOnly = (!bCtrl) && (!bAlt) && bShift;
		bool bNoModifiers = (!bCtrl) && (!bAlt) && (!bShift);
		bool bMultiAreaSelection = MultiAreaSelectionGet();
		bool bReplaceOldSelection =
				(!bMultiAreaSelection)
			||	(!(bCtrlOnly || bShiftOnly));
		if( ! ( bCtrlOnly || bNoModifiers ) )
			return;
		CExtGridHitTestInfo htInfo( point );
		HitTest( htInfo, false, true );
		DWORD dwSiwStyles = SiwGetStyle();
		if(		(dwSiwStyles & __EGBS_SFB_MASK) != __EGBS_SFB_NONE
			&&	( htInfo.m_nColNo >= 0 || htInfo.m_nRowNo >= 0 )
			)
		{ // if any selection mode can be tracked
			if( (htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0 )
			{
				LONG nColumnCount = ColumnCountGet();
				LONG nRowCount = RowCountGet();
				if( (dwSiwStyles & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_COLUMNS )
				{
					if( htInfo.m_nColNo >= 0 /*&& nColumnCount > 0*/ )
					{
						if( bNoModifiers )
						{
							CPoint ptNewFocus( htInfo.m_nColNo, 0 );
							SelectionSet( ptNewFocus, true, false, false );
							FocusSet( ptNewFocus, true, true, false, false );
							OnSwDoRedraw();
							return;
						} // if( bNoModifiers )
						ASSERT( bCtrlOnly );
						if(		(dwSiwStyles & __EGBS_SFM_COLUMNS) == 0
							||	(! SelectionGetForCell( htInfo.m_nColNo, 0 ) )
							||	bReplaceOldSelection
							)
							return; // processed in WM_LBUTTONDOWN
						CRect rcSelection( htInfo.m_nColNo, 0, htInfo.m_nColNo, nRowCount-1 );
						SelectionSet( rcSelection, bReplaceOldSelection, false, false );
						EnsureVisibleColumn( htInfo.m_nColNo, true );
					} // if( htInfo.m_nColNo >= 0 /*&& nColumnCount > 0*/ )
					return;
				} // if( (dwSiwStyles & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_COLUMNS )
				else if( (dwSiwStyles & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_ROWS )
				{
					if( htInfo.m_nRowNo >= 0 /*&& nRowCount > 0*/ )
					{
						if( bNoModifiers )
						{
							CPoint ptNewFocus( 0, htInfo.m_nRowNo );
							SelectionSet( ptNewFocus, true, false, false );
							FocusSet( ptNewFocus, true, true, false, false );
							OnSwDoRedraw();
							return;
						} // if( bNoModifiers )
						ASSERT( bCtrlOnly );
						if(		(dwSiwStyles & __EGBS_SFM_ROWS) == 0
							||	(! SelectionGetForCell( 0, htInfo.m_nRowNo ) )
							||	bReplaceOldSelection
							)
							return; // processed in WM_LBUTTONDOWN
						CRect rcSelection( 0, htInfo.m_nRowNo, nColumnCount-1, htInfo.m_nRowNo );
						SelectionSet( rcSelection, bReplaceOldSelection, false, false );
						EnsureVisibleRow( htInfo.m_nRowNo, true );
					} // if( htInfo.m_nRowNo >= 0 /*&& nRowCount > 0*/ )
					return;
				} // else if( (dwSiwStyles & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_ROWS )
				else
				{
					ASSERT( (dwSiwStyles & __EGBS_SFB_MASK) == __EGBS_SFB_CELLS );
					DWORD dwScrollTypeH = SiwScrollTypeHGet();
					DWORD dwScrollTypeV = SiwScrollTypeVGet();
					if(		0 <= htInfo.m_nColNo && (htInfo.m_nColNo < nColumnCount || dwScrollTypeH == __ESIW_ST_VIRTUAL )
						&&	0 <= htInfo.m_nRowNo && (htInfo.m_nRowNo < nRowCount || dwScrollTypeV == __ESIW_ST_VIRTUAL )
						)
					{ // if column and row numbers specified
						if( bNoModifiers )
						{
							CPoint ptNewFocus( htInfo.m_nColNo, htInfo.m_nRowNo );
							SelectionSet( ptNewFocus, true, false, false );
							FocusSet( ptNewFocus, true, true, false, false );
							OnSwDoRedraw();
							return;
						} // if( bNoModifiers )
						ASSERT( bCtrlOnly );
						if(		(dwSiwStyles&(__EGBS_SFM_COLUMNS|__EGBS_SFM_ROWS)) == 0
							||	(! SelectionGetForCell( htInfo.m_nColNo, htInfo.m_nRowNo ) )
							||	bReplaceOldSelection
							)
							return; // processed in WM_LBUTTONDOWN
						//ASSERT( m_eMTT == __EMTT_NOTHING );
						CPoint ptNewFocus( htInfo.m_nColNo, htInfo.m_nRowNo );
						if( ! AutoFocusBottomRightGet() )
							FocusSet( ptNewFocus, true, true, false, false );
						SelectionSet( ptNewFocus, false, false, true );
						return;
					} // if column and row numbers specified
				} // else from else if( (dwSiwStyles & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_ROWS )
			} // if( (htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0 )
		} // if any selection mode can be tracked
		return;
	} // if( OnGbwDataDndIsAllowed() && m_eMTT == __EMTT_DATA_DND_STARTING )

	if( m_eMTT != __EMTT_NOTHING )
	{
		SendMessage( WM_CANCELMODE );
		return;
	}

	if( OnGbwAnalyzeCellMouseClickEvent(VK_LBUTTON,0,nFlags,point) )
		return;

	CExtScrollItemWnd::OnLButtonUp(nFlags, point);
}

void CExtGridBaseWnd::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	if( m_eMTT != __EMTT_NOTHING )
	{
		SendMessage( WM_CANCELMODE );
		return;
	}
	if( OnGbwAnalyzeCellMouseClickEvent(VK_RBUTTON,2,nFlags,point) )
		return;
	CExtScrollItemWnd::OnRButtonDblClk(nFlags, point);
}
void CExtGridBaseWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	if( m_eMTT != __EMTT_NOTHING )
	{
		SendMessage( WM_CANCELMODE );
		return;
	}
	if( OnGbwAnalyzeCellMouseClickEvent(VK_RBUTTON,1,nFlags,point) )
		return;
	CExtScrollItemWnd::OnRButtonDown(nFlags, point);
}
void CExtGridBaseWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	if( m_eMTT != __EMTT_NOTHING )
	{
		SendMessage( WM_CANCELMODE );
		return;
	}
	if( OnGbwAnalyzeCellMouseClickEvent(VK_RBUTTON,0,nFlags,point) )
		return;
	CExtScrollItemWnd::OnRButtonUp(nFlags, point);
}

void CExtGridBaseWnd::OnMButtonDblClk(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	if( m_eMTT != __EMTT_NOTHING )
	{
		SendMessage( WM_CANCELMODE );
		return;
	}
	if( OnGbwAnalyzeCellMouseClickEvent(VK_MBUTTON,2,nFlags,point) )
		return;
	CExtScrollItemWnd::OnMButtonDblClk(nFlags, point);
}
void CExtGridBaseWnd::OnMButtonDown(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	if( m_eMTT != __EMTT_NOTHING )
	{
		SendMessage( WM_CANCELMODE );
		return;
	}
	if( OnGbwAnalyzeCellMouseClickEvent(VK_MBUTTON,1,nFlags,point) )
		return;
	CExtScrollItemWnd::OnMButtonDown(nFlags, point);
}
void CExtGridBaseWnd::OnMButtonUp(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	if( m_eMTT != __EMTT_NOTHING )
	{
		SendMessage( WM_CANCELMODE );
		return;
	}
	if( OnGbwAnalyzeCellMouseClickEvent(VK_MBUTTON,0,nFlags,point) )
		return;
	CExtScrollItemWnd::OnMButtonUp(nFlags, point);
}

bool CExtGridBaseWnd::OnGbwAnalyzeCellMouseClickEvent(
	UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
	UINT nRepCnt, // 0 - button up, 1 - signle click, 2 - double click, 3 - post single click & begin editing
	UINT nFlags, // mouse event flags
	CPoint point // mouse pointer in client coordinates
	)
{
	ASSERT_VALID( this );
	ASSERT( nChar == VK_LBUTTON || nChar == VK_RBUTTON || nChar == VK_MBUTTON );
	ASSERT( 0 <= nRepCnt && nRepCnt <= 3 );
	nChar;
	nFlags;
	point;
	if( nRepCnt == 1 || nRepCnt == 2 || nRepCnt == 3 )
	{
		if( m_wndContentExpand.GetSafeHwnd() != NULL )
			SendMessage( WM_CANCELMODE );
	} // if( nRepCnt == 1 || nRepCnt == 2 || nRepCnt == 3 )
//	if( nRepCnt == 2 )
//	{
//		CExtGridHitTestInfo htInfo( point );
//		HitTest( htInfo, false, true );
//		if( ( ! htInfo.IsHoverEmpty() ) && htInfo.IsValidRect() )
//		{
//			if( (htInfo.m_dwAreaFlags&__EGBWA_CELL_IMAGE) != 0 )
//				return false;
//			if( (htInfo.m_dwAreaFlags&__EGBWA_CELL_CHECKBOX) != 0 )
//				return false;
//			if( (htInfo.m_dwAreaFlags&__EGBWA_CELL_BUTTON) != 0 )
//				return false;
//			if( (htInfo.m_dwAreaFlags&__EGBWA_CELL_SORT_ARROW) != 0 )
//				return false;
//			if( (htInfo.m_dwAreaFlags&__EGBWA_CELL_FOCUS_ARROW) != 0 )
//				return false;
//			if( (htInfo.m_dwAreaFlags&(__EGBWA_CELL_TEXT|__EGBWA_CELL_TEXT_AREA)) != 0 )
//				return
//					OnGbwBeginEdit(
//						htInfo.m_nVisibleColNo,
//						htInfo.m_nVisibleRowNo,
//						htInfo.m_nColNo,
//						htInfo.m_nRowNo,
//						htInfo.GetInnerOuterTypeOfColumn(),
//						htInfo.GetInnerOuterTypeOfRow(),
//						htInfo.m_rcExtra,
//						htInfo.m_rcItem,
//						htInfo.m_rcPart
//						);
//		} // if( ( ! htInfo.IsHoverEmpty() ) && htInfo.IsValidRect() )
//	} // if( nRepCnt == 2 )
	return false;
}

bool CExtGridBaseWnd::OnGbwBeginEdit(
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	const RECT & rcInplaceControl,
	bool bContinueMsgLoop // = true
	)
{
	ASSERT_VALID( this );
	nVisibleColNo;
	nVisibleRowNo;
	nColNo;
	nRowNo;
	nColType;
	nRowType;
	rcCellExtra;
	rcCell;
	rcInplaceControl;
	bContinueMsgLoop;
	return false;
}

UINT CExtGridBaseWnd::OnGetDlgCode() 
{
	ASSERT_VALID( this );
	return DLGC_WANTARROWS|DLGC_WANTALLKEYS;
}

bool CExtGridBaseWnd::OnGbwAnalyzeCellKeyEvent(
	bool bKeyDownEvent, // true - key-down event, false - key-up event
	UINT nChar, // virtual key code
	UINT nRepCnt, // key-down/key-up press count
	UINT nFlags // key-down/key-up event flags
	)
{
	ASSERT_VALID( this );
	bKeyDownEvent;
	nChar;
	nRepCnt;
	nFlags;
	if( m_wndContentExpand.GetSafeHwnd() != NULL )
		SendMessage( WM_CANCELMODE );
	return false;
}

void CExtGridBaseWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	ASSERT_VALID( this );
	if( m_eMTT == __EMTT_DATA_DND_STARTING )
		return;
	if( m_eMTT != __EMTT_NOTHING )
	{
		SendMessage( WM_CANCELMODE );
		return;
	}
	if(	OnGbwAnalyzeCellKeyEvent(
			true,
			nChar,
			nRepCnt,
			nFlags
			)
		)
		return;
	if( nChar == VK_TAB )
	{
		HWND hWndParent = GetParent()->GetSafeHwnd();
		if( hWndParent == NULL )
			return;
		bool bShift = ( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 ) ? true : false;
		::SendMessage(
			hWndParent,
			WM_NEXTDLGCTL,
			bShift ? (!0) : 0,
			0
			);
		return;
	} // if( nChar == VK_TAB )
CRect rcClient = OnSwGetClientRect();
	if(		rcClient.right <= rcClient.left
		||	rcClient.bottom <= rcClient.top
		)
		return;
LONG nJump = LONG(nRepCnt);
	if( nJump == 0 )
		nJump = 1;
bool bAlt = ( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 ) ? true : false;
bool bCtrl = ( (::GetAsyncKeyState(VK_CONTROL)&0x8000) != 0 ) ? true : false;
bool bShift = ( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 ) ? true : false;
bool bCtrlOnly = bCtrl && (!bAlt) && (!bShift);
bool bShiftOnly = (!bCtrl) && (!bAlt) && bShift;
bool bCtrlShiftOnly = bCtrl && (!bAlt) && bShift;
bool bNoModifiers = (!bCtrl) && (!bAlt) && (!bShift);
DWORD dwSelectionType = SelectionTypeGet();
CPoint ptFocusOld = FocusGet();
LONG nColumnCount = ColumnCountGet();
LONG nRowCount = RowCountGet();
bool bFocusInequality = false;
	if( ptFocusOld.x < 0 )
	{
		ptFocusOld.x = 0;
		bFocusInequality = true;
	}
	if( ptFocusOld.y < 0 )
	{
		ptFocusOld.y = 0;
		bFocusInequality = true;
	}
CRect rcSelectionOld(
		0,
		0,
		(		nColumnCount > 0
			&&	(dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_ROWS // (dwSelectionType & __EGBS_SFM_ROWS) != 0
			) ? (nColumnCount-1) : 0,
		(		nRowCount > 0
			&&	(dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_COLUMNS //(dwSelectionType & __EGBS_SFM_COLUMNS) != 0
			)	? (nRowCount-1) : 0
		);
INT nAreaCount = SelectionGetAreaCount();
	if( nAreaCount > 0 )
		rcSelectionOld = SelectionGet( false, nAreaCount-1 );
	switch( nChar )
	{
	case VK_UP:
	case VK_DOWN:
		if(		(dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_NONE
			||	(dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_COLUMNS
			||	bCtrlOnly
			)
		{
			if( OnSwHasScrollBar( false ) )
			{
				SendMessage(
					WM_VSCROLL,
					(nChar == VK_UP) ? SB_LINEUP : SB_LINEDOWN,
					(LPARAM)(GetScrollBarCtrl( SB_VERT )->GetSafeHwnd())
					);
				_AnalyzeVirtualFocus();
				OnGbwHoverRecalc();
			} // if( OnSwHasScrollBar( false ) )
			return;
		}
		if( nRowCount == 0 )
			return;
		if(		(dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_CELLS
			||	(dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_ROWS
			)
		{
			DWORD dwScrollTypeV = SiwScrollTypeVGet();
			if( dwScrollTypeV == __ESIW_ST_VIRTUAL )
			{
				ASSERT( nJump != 0L );
				nJump = ( nJump < 0L ) ? -1L : 1L;
				if( nChar == VK_UP && ptFocusOld.y == 0 )
				{
					SendMessage(
						WM_VSCROLL,
						SB_LINEUP,
						(LPARAM)(GetScrollBarCtrl( SB_VERT )->GetSafeHwnd())
						);
					_AnalyzeVirtualFocus();
					OnGbwHoverRecalc();
					return;
				}
				else if( nChar == VK_DOWN )
				{
					if(	OnSiwVirtualEndTestV(
							ptFocusOld.y+1,
							LPARAM(__EGBWA_INNER_CELLS)
							)
						)
					{
						if( OnSiwQueryVirtualEOF(false) )
							return;
						SendMessage(
							WM_VSCROLL,
							SB_LINEDOWN,
							(LPARAM)(GetScrollBarCtrl( SB_VERT )->GetSafeHwnd())
							);
						_AnalyzeVirtualFocus();
						OnGbwHoverRecalc();
						return;
					}
					LONG nRowFirst = 0, nRowLast = 0, nRowFirstOffset = 0;
					OnGbwQueryVisibleRowRange(
						nRowFirst,
						nRowLast,
						nRowFirstOffset
						);
					ASSERT( ptFocusOld.y <= nRowLast );
					if( (ptFocusOld.y + 1) == nRowLast )
					{
						SendMessage(
							WM_VSCROLL,
							SB_LINEDOWN,
							(LPARAM)(GetScrollBarCtrl( SB_VERT )->GetSafeHwnd())
							);
						nJump = 0L;
					}
				}
			} // if( dwScrollTypeV == __ESIW_ST_VIRTUAL )
			CPoint ptFocusNew( ptFocusOld );
			ptFocusNew.y += nJump * ( (nChar == VK_UP) ? (-1) : 1 );
			if( nRowCount > 0 )
			{
				if( ptFocusNew.y < 0 )
					ptFocusNew.y = 0;
				if( ptFocusNew.y >= nRowCount )
					ptFocusNew.y = nRowCount-1;
			} // if( nRowCount > 0 )
			if( bFocusInequality || ptFocusNew.y != ptFocusOld.y )
			{
				CRect rcSelectionNew( rcSelectionOld );
				bool bResetFocus = false;
				if( bShiftOnly && (dwSelectionType & __EGBS_SFM_ROWS) != 0 && nAreaCount > 0 )
				{
					bResetFocus = true;
					rcSelectionNew.bottom = ptFocusNew.y;
					SelectionSet( rcSelectionNew, false, true, false );
				} // if( bShiftOnly && (dwSelectionType & __EGBS_SFM_ROWS) != 0 && nAreaCount > 0 )
				else if( bNoModifiers )
				{
					bResetFocus = true;
					rcSelectionNew.top = rcSelectionNew.bottom = ptFocusNew.y;
					if( (dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_CELLS )
						rcSelectionNew.left = rcSelectionNew.right;
					SelectionSet( rcSelectionNew, true, false, false );
				} // else if( bNoModifiers )
				if( bResetFocus )
				{
					FocusSet(
						ptFocusNew,
						((dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_CELLS) ? true : false,
						true,
						false,
						true
						);
					OnSwUpdateScrollBars();
				} // if( bResetFocus )
			} // if( bFocusInequality || ptFocusNew.y != ptFocusOld.y )
			OnGbwHoverRecalc();
		}
		return;
	case VK_LEFT:
	case VK_RIGHT:
		if(		(dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_NONE
			||	(dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_ROWS
			||	bCtrlOnly
			)
		{
			if( OnSwHasScrollBar( true ) )
			{
				SendMessage(
					WM_HSCROLL,
					(nChar == VK_LEFT) ? SB_LINELEFT : SB_LINERIGHT,
					(LPARAM)(GetScrollBarCtrl( SB_HORZ )->GetSafeHwnd())
					);
				_AnalyzeVirtualFocus();
				OnGbwHoverRecalc();
			} // if( OnSwHasScrollBar( true ) )
			return;
		}
		if( nColumnCount == 0 )
			return;
		if(		(dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_CELLS
			||	(dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_COLUMNS
			)
		{
			DWORD dwScrollTypeH = SiwScrollTypeHGet();
			if( dwScrollTypeH == __ESIW_ST_VIRTUAL )
			{
				ASSERT( nJump != 0L );
				nJump = ( nJump < 0L ) ? -1L : 1L;
				if( nChar == VK_LEFT && ptFocusOld.x == 0 )
				{
					SendMessage(
						WM_HSCROLL,
						SB_LINELEFT,
						(LPARAM)(GetScrollBarCtrl( SB_HORZ )->GetSafeHwnd())
						);
					_AnalyzeVirtualFocus();
					OnGbwHoverRecalc();
					return;
				}
				else if( nChar == VK_RIGHT )
				{
					if(	OnSiwVirtualEndTestH(
							ptFocusOld.x+1,
							LPARAM(__EGBWA_INNER_CELLS)
							)
						)
					{
						if( OnSiwQueryVirtualEOF(true) )
							return;
						SendMessage(
							WM_HSCROLL,
							SB_LINERIGHT,
							(LPARAM)(GetScrollBarCtrl( SB_HORZ )->GetSafeHwnd())
							);
						_AnalyzeVirtualFocus();
						OnGbwHoverRecalc();
						return;
					}
					LONG nColFirst = 0, nColLast = 0, nColFirstOffset = 0;
					OnGbwQueryVisibleColumnRange(
						nColFirst,
						nColLast,
						nColFirstOffset
						);
					ASSERT( ptFocusOld.x <= nColLast );
					if( (ptFocusOld.x + 1) == nColLast )
					{
						SendMessage(
							WM_HSCROLL,
							SB_LINERIGHT,
							(LPARAM)(GetScrollBarCtrl( SB_HORZ )->GetSafeHwnd())
							);
						nJump = 0L;
					}
				}
			} // if( dwScrollTypeH == __ESIW_ST_VIRTUAL )
			CPoint ptFocusNew( ptFocusOld );
			ptFocusNew.x += nJump * ( (nChar == VK_LEFT) ? (-1) : 1 );
			if( nColumnCount > 0 )
			{
				if( ptFocusNew.x < 0 )
					ptFocusNew.x = 0;
				if( ptFocusNew.x >= nColumnCount )
					ptFocusNew.x = nColumnCount-1;
			} // if( nColumnCount > 0 )
			if( bFocusInequality || ptFocusNew.x != ptFocusOld.x )
			{
				CRect rcSelectionNew( rcSelectionOld );
				bool bResetFocus = false;
				if( bShiftOnly && (dwSelectionType & __EGBS_SFM_COLUMNS) != 0 && nAreaCount > 0 )
				{
					bResetFocus = true;
					rcSelectionNew.right = ptFocusNew.x;
					SelectionSet( rcSelectionNew, false, true, false );
				} // if( bShiftOnly && (dwSelectionType & __EGBS_SFM_COLUMNS) != 0 && nAreaCount > 0 )
				else if( bNoModifiers )
				{
					bResetFocus = true;
					rcSelectionNew.left = rcSelectionNew.right = ptFocusNew.x;
					if( (dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_CELLS )
						rcSelectionNew.top = rcSelectionNew.bottom;
					SelectionSet( rcSelectionNew, true, false, false );
				} // else if( bNoModifiers )
				if( bResetFocus )
				{
					FocusSet(
						ptFocusNew,
						true,
						((dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_CELLS) ? true : false,
						false,
						true
						);
					OnSwUpdateScrollBars();
				} // if( bResetFocus )
			} // if( bFocusInequality || ptFocusNew.x != ptFocusOld.x )
			OnGbwHoverRecalc();
		}
		return;
	case VK_PRIOR:
	case VK_NEXT:
		if(	(	(dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_NONE
			||	(dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_COLUMNS
			)
			&&	( bNoModifiers || bShiftOnly )
			)
		{
			if( bShiftOnly && (dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_NONE )
			{
				if( OnSwHasScrollBar( true ) )
				{
					SendMessage(
						WM_HSCROLL,
						(nChar == VK_PRIOR) ? SB_PAGELEFT : SB_PAGERIGHT,
						(LPARAM)(GetScrollBarCtrl( SB_HORZ )->GetSafeHwnd())
						);
					_AnalyzeVirtualFocus();
					OnGbwHoverRecalc();
				} // if( OnSwHasScrollBar( true ) )
				return;
			} // if( bShiftOnly && (dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_NONE )
			else
			{
				if( OnSwHasScrollBar( false ) )
				{
					SendMessage(
						WM_VSCROLL,
						(nChar == VK_PRIOR) ? SB_PAGEUP : SB_PAGEDOWN,
						(LPARAM)(GetScrollBarCtrl( SB_VERT )->GetSafeHwnd())
						);
					_AnalyzeVirtualFocus();
					OnGbwHoverRecalc();
				} // if( OnSwHasScrollBar( false ) )
				return;
			} // else from if( bShiftOnly && (dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_NONE )
		}
		if( nRowCount == 0 )
			return;
		if(		(dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_CELLS
			||	(dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_ROWS
			)
		{
			DWORD dwScrollTypeV = SiwScrollTypeVGet();
			CPoint ptFocusNew( ptFocusOld );
			if( (bCtrlOnly || bCtrlShiftOnly) && nRowCount > 0 )
			{
				ptFocusNew.y = (nChar == VK_PRIOR) ? 0 : (nRowCount-1);
			} // if( (bCtrlOnly || bCtrlShiftOnly) && nRowCount > 0 )
			else
			{
				INT nPartialFirstRow = 0, nPartialLastRow = 0;
				CExtGridHitTestInfo htInfo( CPoint( rcClient.left+1, rcClient.bottom-1 ) );
				HitTest( htInfo, false, true );
				if(		(htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0
					&&	(! htInfo.m_rcItem.IsRectEmpty() )
					)
				{
					ASSERT( htInfo.m_rcItem.top < rcClient.bottom );
					if( htInfo.m_rcItem.bottom > rcClient.bottom )
						nPartialLastRow = 1;
				}
				if( dwScrollTypeV == __ESIW_ST_PIXEL )
				{
					CExtGridHitTestInfo htInfo( CPoint( rcClient.left+1, rcClient.top+1 ) );
					HitTest( htInfo, false, true );
					if(		(htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0
						&&	(! htInfo.m_rcItem.IsRectEmpty() )
						)
					{
						ASSERT( htInfo.m_rcItem.bottom > rcClient.top );
						if( htInfo.m_rcItem.top < rcClient.top )
							nPartialFirstRow = 1;
					}
				} // if( dwScrollTypeV == __ESIW_ST_PIXEL )
				LONG nRowFirst = 0, nRowLast = 0, nRowFirstOffset = 0;
				OnGbwQueryVisibleRowRange(
					nRowFirst,
					nRowLast,
					nRowFirstOffset
					);
				if( nRowFirst < (nRowLast-1) )
				{
					nRowLast -= nPartialLastRow;
					if( nRowLast > nRowFirst && nPartialFirstRow != 0 )
						nRowFirst += nPartialFirstRow;
				}
				bool bDoPreScroll = true;
				if( ptFocusNew.y < nRowFirst || nRowLast < ptFocusNew.y )
				{
					bDoPreScroll = false;
					ptFocusNew.y = (nChar == VK_PRIOR) ? nRowFirst : nRowLast;
				} // if( ptFocusNew.y < nRowFirst || nRowLast < ptFocusNew.y )
				else
				{
					if( nChar == VK_PRIOR )
					{
						if( ptFocusNew.y > nRowFirst )
						{
							ptFocusNew.y = nRowFirst;
							bDoPreScroll = false;
						}
					} // if( nChar == VK_PRIOR )
					else
					{
						ASSERT( nChar == VK_NEXT );
						if( ptFocusNew.y < (nRowLast-1) )
						{
							ptFocusNew.y = nRowLast;
							bDoPreScroll = false;
						}
					} // else from if( nChar == VK_PRIOR )
				} // else from if( ptFocusNew.y < nRowFirst || nRowLast < ptFocusNew.y )
				
				if( bDoPreScroll && OnSwHasScrollBar( false ) )
				{
					SendMessage(
						WM_VSCROLL,
						(nChar == VK_PRIOR) ? SB_PAGEUP : SB_PAGEDOWN,
						(LPARAM)(GetScrollBarCtrl( SB_VERT )->GetSafeHwnd())
						);
					OnGbwHoverRecalc();
					OnGbwQueryVisibleRowRange(
						nRowFirst,
						nRowLast,
						nRowFirstOffset
						);
					ASSERT( nRowFirst <= nRowLast );
					nPartialLastRow = 0;
					CExtGridHitTestInfo htInfo( CPoint( rcClient.left+1, rcClient.bottom-1 ) );
					HitTest( htInfo, false, true );
					if(		(htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0
						&&	(! htInfo.m_rcItem.IsRectEmpty() )
						)
					{
						ASSERT( htInfo.m_rcItem.top < rcClient.bottom );
						if( htInfo.m_rcItem.bottom > rcClient.bottom )
							nPartialLastRow = 1;
						if( nRowFirst < (nRowLast-1) )
							nRowLast -= nPartialLastRow;
					}
					ptFocusNew.y = (nChar == VK_PRIOR) ? nRowFirst : nRowLast;
				} // if( bDoPreScroll && OnSwHasScrollBar( false ) )
				else
				{
					ptFocusNew.y = (nChar == VK_PRIOR) ? nRowFirst : nRowLast;
				} // else from if( bDoPreScroll && OnSwHasScrollBar( true ) )
			} // else from if( (bCtrlOnly || bCtrlShiftOnly) && nRowCount > 0 )
			if( nRowCount > 0 )
			{
				if( ptFocusNew.y < 0 )
					ptFocusNew.y = 0;
				if( ptFocusNew.y >= nRowCount )
					ptFocusNew.y = nRowCount-1;
			} // if( nRowCount > 0 )
			//if( dwScrollTypeV == __ESIW_ST_VIRTUAL && bNoModifiers )
			if( dwScrollTypeV == __ESIW_ST_VIRTUAL )
			{
				for(	bFocusInequality = true;
						OnSiwVirtualEndTestV(
							ptFocusNew.y,
							LPARAM(__EGBWA_INNER_CELLS)
							);
						ptFocusNew.y--
						)
				{
					if(  ptFocusNew.y == 0 )
					{
						FocusUnset( true );
						return;
					}
				}
			} // if( dwScrollTypeV == __ESIW_ST_VIRTUAL )
			if( bFocusInequality || ptFocusNew.y != ptFocusOld.y )
			{
				CRect rcSelectionNew( rcSelectionOld );
				bool bResetFocus = false;
				if( (bShiftOnly || bCtrlShiftOnly) && (dwSelectionType & __EGBS_SFM_ROWS) != 0 && nAreaCount > 0 )
				{
					bResetFocus = true;
					rcSelectionNew.bottom = ptFocusNew.y;
					SelectionSet( rcSelectionNew, false, true, false );
				} // if( (bShiftOnly || bCtrlShiftOnly) && (dwSelectionType & __EGBS_SFM_ROWS) != 0 && nAreaCount > 0 )
				else if( bNoModifiers || bCtrlOnly || bCtrlShiftOnly )
				{
					bResetFocus = true;
					rcSelectionNew.top = rcSelectionNew.bottom = ptFocusNew.y;
					if( (dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_CELLS )
						rcSelectionNew.left = rcSelectionNew.right;
					SelectionSet( rcSelectionNew, true, false, false );
				} // else if( bNoModifiers || bCtrlOnly || bCtrlShiftOnly )
				if( bResetFocus )
				{
					FocusSet(
						ptFocusNew,
						((dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_CELLS) ? true : false,
						true,
						false,
						true
						);
					OnSwUpdateScrollBars();
				} // if( bResetFocus )
			} // if( bFocusInequality || ptFocusNew.y != ptFocusOld.y )
			OnGbwHoverRecalc();
		}
		return;
	case VK_HOME:
	case VK_END:
		{
			if(		(dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_NONE
				||	(	(dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_ROWS
					&&	bCtrlOnly
					)
				)
			{
				if(		(dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_NONE
					&&	bCtrlOnly
					&&	OnSwHasScrollBar( false )
					)
				{
					SendMessage(
						WM_VSCROLL,
						(nChar == VK_HOME) ? SB_TOP : SB_BOTTOM,
						(LPARAM)(GetScrollBarCtrl( SB_VERT )->GetSafeHwnd())
						);
					_AnalyzeVirtualFocus();
					OnGbwHoverRecalc();
					return;
				} // if( OnSwHasScrollBar( true ) )
				if( OnSwHasScrollBar( true ) )
				{
					SendMessage(
						WM_HSCROLL,
						(nChar == VK_HOME) ? SB_LEFT : SB_RIGHT,
						(LPARAM)(GetScrollBarCtrl( SB_HORZ )->GetSafeHwnd())
						);
					_AnalyzeVirtualFocus();
					OnGbwHoverRecalc();
				} // if( OnSwHasScrollBar( true ) )
				return;
			}
			ASSERT( (dwSelectionType & __EGBS_SFB_MASK) != __EGBS_SFB_NONE );
			if(		(dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_CELLS
				||	(dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_COLUMNS
				)
			{
				if( bCtrlOnly )
				{
					if( OnSwHasScrollBar( false ) )
					{
						SendMessage(
							WM_VSCROLL,
							(nChar == VK_HOME) ? SB_TOP : SB_BOTTOM,
							(LPARAM)(GetScrollBarCtrl( SB_VERT )->GetSafeHwnd())
							);
						_AnalyzeVirtualFocus();
						OnGbwHoverRecalc();
					} // if( OnSwHasScrollBar( false ) )
					return;
				}
				if( nColumnCount < 0 )
				{
					if( OnSwHasScrollBar( true ) )
					{
						SendMessage(
							WM_HSCROLL,
							(nChar == VK_HOME) ? SB_LEFT : SB_RIGHT,
							(LPARAM)(GetScrollBarCtrl( SB_HORZ )->GetSafeHwnd())
							);
						_AnalyzeVirtualFocus();
						OnGbwHoverRecalc();
					} // if( OnSwHasScrollBar( true ) )
					return;
				}
				if( nColumnCount == 0 )
					return;
				CPoint ptFocusNew( ptFocusOld );
				ptFocusNew.x = (nChar == VK_HOME) ? 0 : (nColumnCount-1);
				if( bFocusInequality || ptFocusNew.x != ptFocusOld.x )
				{
					CRect rcSelectionNew( rcSelectionOld );
					bool bResetFocus = false;
					if( bShiftOnly && (dwSelectionType & __EGBS_SFM_COLUMNS) != 0 && nAreaCount > 0 )
					{
						bResetFocus = true;
						rcSelectionNew.right = ptFocusNew.x;
						SelectionSet( rcSelectionNew, false, true, false );
					} // if( bShiftOnly && (dwSelectionType & __EGBS_SFM_COLUMNS) != 0 && nAreaCount > 0 )
					else if( bNoModifiers )
					{
						bResetFocus = true;
						rcSelectionNew.left = rcSelectionNew.right = ptFocusNew.x;
						if( (dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_CELLS )
							rcSelectionNew.top = rcSelectionNew.bottom;
						SelectionSet( rcSelectionNew, true, false, false );
					} // else if( bNoModifiers )
					if( bResetFocus )
					{
						FocusSet(
							ptFocusNew,
							true,
							((dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_CELLS) ? true : false,
							false,
							true
							);
						OnSwUpdateScrollBars();
					} // if( bResetFocus )
				} // if( bFocusInequality || ptFocusNew.x != ptFocusOld.x )
				OnGbwHoverRecalc();
				return;
			}
			ASSERT( (dwSelectionType & __EGBS_SFB_MASK) == __EGBS_SFB_FULL_ROWS );
			if( nRowCount < 0 )
			{
				if( OnSwHasScrollBar( false ) )
				{
					SendMessage(
						WM_VSCROLL,
						(nChar == VK_HOME) ? SB_TOP : SB_BOTTOM,
						(LPARAM)(GetScrollBarCtrl( SB_VERT )->GetSafeHwnd())
						);
					_AnalyzeVirtualFocus();
					OnGbwHoverRecalc();
				} // if( OnSwHasScrollBar( false ) )
				return;
			}
			if( nRowCount == 0 )
				return;
			CPoint ptFocusNew( ptFocusOld );
			ptFocusNew.y = (nChar == VK_HOME) ? 0 : (nRowCount-1);
			if( bFocusInequality || ptFocusNew.y != ptFocusOld.y )
			{
				CRect rcSelectionNew( rcSelectionOld );
				bool bResetFocus = false;
				if( bShiftOnly && (dwSelectionType & __EGBS_SFM_ROWS) != 0 && nAreaCount > 0 )
				{
					bResetFocus = true;
					rcSelectionNew.bottom = ptFocusNew.y;
					SelectionSet( rcSelectionNew, false, true, false );
				} // if( bShiftOnly && (dwSelectionType & __EGBS_SFM_ROWS) != 0 && nAreaCount > 0 )
				else if( bNoModifiers )
				{
					bResetFocus = true;
					rcSelectionNew.top = rcSelectionNew.bottom = ptFocusNew.y;
					SelectionSet( rcSelectionNew, true, false, false );
				} // else if( bNoModifiers )
				if( bResetFocus )
				{
					FocusSet(
						ptFocusNew,
						false,
						true,
						false,
						true
						);
					OnSwUpdateScrollBars();
				} // if( bResetFocus )
			} // if( bFocusInequality || ptFocusNew.y != ptFocusOld.y )
			OnGbwHoverRecalc();
		}
		return;
	case VK_RETURN:
		OnGbwProcessVkBeginEdit( VK_RETURN );
		return;
	} // switch( nChar )

	CExtScrollItemWnd::OnKeyDown( nChar,  nRepCnt,  nFlags );
}

bool CExtGridBaseWnd::OnGbwProcessVkBeginEdit( UINT nChar )
{
	ASSERT_VALID( this );
	nChar;
	return false;
}

bool CExtGridBaseWnd::EditCell( // default parameters will start to edit focused cell
	LONG nColNo, // = -1L
	LONG nRowNo, // = -1L
	INT nColType, // = 0,
	INT nRowType, // = 0
	bool bContinueMsgLoop // = true
	)
{
	ASSERT_VALID( this );
	if( nColType != 0 && nRowType != 0 )
		return false;
DWORD dwBasicSelectionType = SiwGetStyle() & __EGBS_SFB_MASK;
	if( dwBasicSelectionType != __EGBS_SFB_CELLS )
		return false;
	if( nColNo < 0L || nRowNo < 0L )
	{
		CPoint ptFocus = FocusGet();
		if( nColNo < 0L )
			nColNo = ptFocus.x;
		if( nRowNo < 0L )
			nRowNo = ptFocus.y;
	} // if( nColNo < 0L || nRowNo < 0L )
	if( nColNo < 0L || nRowNo < 0L )
		return false;
	if(		( ! EnsureVisibleColumn( nColNo ) )
		||	( ! EnsureVisibleRow( nRowNo ) )
		)
		return false;
LONG nVisibleColNo = 0L, nVisibleRowNo = 0L;
CRect rcCellExtra( 0, 0, 0, 0 ), rcCell( 0, 0, 0, 0 );
	if( ! OnGbwCalcVisibleItemRect(
			nColNo,
			nRowNo,
			0,
			0,
			rcCellExtra,
			rcCell,
			&nVisibleColNo,
			&nVisibleRowNo
			)
		)
		return false;
CRect rcInplaceControl(0,0,0,0);
	if( ! OnGbwBeginEdit(
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			0,
			0,
			rcCellExtra,
			rcCell,
			rcInplaceControl,
			bContinueMsgLoop
			)
		)
		return false;
	return true;
}

void CExtGridBaseWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(	OnGbwAnalyzeCellKeyEvent(
			false,
			nChar,
			nRepCnt,
			nFlags
			)
		)
		return;
	CExtScrollItemWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

CRect CExtGridBaseWnd::OnSwGetClientRect() const
{
	ASSERT_VALID( this );
	if(		m_rcClient.left >= m_rcClient.right
		||	m_rcClient.top >= m_rcClient.bottom
		)
		m_rcClient = CExtScrollItemWnd::OnSwGetClientRect();
	return m_rcClient;
}

void CExtGridBaseWnd::OnSwUpdateScrollBars()
{
	ASSERT_VALID( this );
	m_rcClient.SetRect( 0, 0, 0, 0 );
	CExtScrollItemWnd::OnSwUpdateScrollBars();
}

void CExtGridBaseWnd::EnableTooltips(
	bool bTooltipsOuterTop, // = true
	bool bTooltipsOuterBottom, // = true
	bool bTooltipsOuterLeft, // = true
	bool bTooltipsOuterRight, // = true
	bool bTooltipsInner // = false
	)
{
	ASSERT_VALID( this );
DWORD dwStyleAdd = 0;
	if( bTooltipsOuterTop )
		dwStyleAdd |= __EGBS_EX_CELL_TOOLTIPS_OUTER_T;
	if( bTooltipsOuterBottom )
		dwStyleAdd |= __EGBS_EX_CELL_TOOLTIPS_OUTER_B;
	if( bTooltipsOuterLeft )
		dwStyleAdd |= __EGBS_EX_CELL_TOOLTIPS_OUTER_L;
	if( bTooltipsOuterRight )
		dwStyleAdd |= __EGBS_EX_CELL_TOOLTIPS_OUTER_R;
	if( bTooltipsInner )
		dwStyleAdd |= __EGBS_EX_CELL_TOOLTIPS_INNER;
	SiwModifyStyleEx(
		dwStyleAdd,
		__EGBS_EX_CELL_TOOLTIPS_OUTER|__EGBS_EX_CELL_TOOLTIPS_INNER,
		false
		);
}

void CExtGridBaseWnd::EnableExpanding(
	bool bExpandingOuterTop, // = false
	bool bExpandingOuterBottom, // = false
	bool bExpandingOuterLeft, // = false
	bool bExpandingOuterRight, // = false
	bool bExpandingInner // = true
	)
{
	ASSERT_VALID( this );
DWORD dwStyleAdd = 0;
	if( bExpandingOuterTop )
		dwStyleAdd |= __EGBS_EX_CELL_EXPANDING_OUTER_T;
	if( bExpandingOuterBottom )
		dwStyleAdd |= __EGBS_EX_CELL_EXPANDING_OUTER_B;
	if( bExpandingOuterLeft )
		dwStyleAdd |= __EGBS_EX_CELL_EXPANDING_OUTER_L;
	if( bExpandingOuterRight )
		dwStyleAdd |= __EGBS_EX_CELL_EXPANDING_OUTER_R;
	if( bExpandingInner )
		dwStyleAdd |= __EGBS_EX_CELL_EXPANDING_INNER;
	SiwModifyStyleEx(
		dwStyleAdd,
		__EGBS_EX_CELL_EXPANDING_OUTER|__EGBS_EX_CELL_EXPANDING_INNER,
		false
		);
}

CToolTipCtrl & CExtGridBaseWnd::GetToolTipCtrl()
{
	ASSERT_VALID( this );
	return m_wndToolTip;
}

const CToolTipCtrl & CExtGridBaseWnd::GetToolTipCtrl() const
{
	ASSERT_VALID( this );
	return m_wndToolTip;
}

CExtContentExpandWnd & CExtGridBaseWnd::GetContentExpandWnd()
{
	ASSERT_VALID( this );
	return m_wndContentExpand;
}

const CExtContentExpandWnd & CExtGridBaseWnd::GetContentExpandWnd() const
{
	ASSERT_VALID( this );
	return m_wndContentExpand;
}

INT CExtGridBaseWnd::ExpandShadowSizeGet() const
{
	ASSERT_VALID( this );
	return m_nSizeOfExpandShadow;
}

INT CExtGridBaseWnd::ExpandShadowSizeSet( INT nSizeOfNewShadow )
{
	ASSERT_VALID( this );
	ASSERT( nSizeOfNewShadow >= 0 );
INT nSizeOfExpandShadowOld = m_nSizeOfExpandShadow;
	m_nSizeOfExpandShadow = nSizeOfNewShadow;
	return nSizeOfExpandShadowOld;
}

bool CExtGridBaseWnd::_CreateHelper()
{
	if( ! CExtScrollItemWnd::_CreateHelper() )
		return false;

	EnableToolTips( TRUE );

	if( ! m_wndToolTip.Create(this) )
	{
		ASSERT( FALSE );
		return false;
	}
	m_wndToolTip.Activate( TRUE );

	return true;
}

void CExtGridBaseWnd::PostNcDestroy() 
{
	m_wndContentExpand.Deactivate();
	m_htInfoExpand.Empty();
	CExtScrollItemWnd::PostNcDestroy();
}

LRESULT CExtGridBaseWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
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
	return CExtScrollItemWnd::WindowProc(message, wParam, lParam);
}

BOOL CExtGridBaseWnd::PreTranslateMessage(MSG* pMsg) 
{
	if(		m_wndToolTip.GetSafeHwnd() != NULL
		&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
		)
		m_wndToolTip.RelayEvent( pMsg );
	return CExtScrollItemWnd::PreTranslateMessage(pMsg);
}

bool CExtGridBaseWnd::OnGbwTooltip(
	const CExtGridHitTestInfo & htInfo
	)
{
	ASSERT_VALID( this );
	ASSERT( ! htInfo.IsHoverEmpty() );
	htInfo;
	return false;
}

bool CExtGridBaseWnd::OnGbwExpand(
	const CExtGridHitTestInfo & htInfo,
	UINT nDelayMilliseconds
	)
{
	ASSERT_VALID( this );
	ASSERT( ! htInfo.IsHoverEmpty() );
	htInfo;
	if( nDelayMilliseconds == 0 )
		return false;
	SetTimer( m_nTimerIdDelayedExpand, nDelayMilliseconds, NULL );
	return true;
}

LRESULT CExtGridBaseWnd::_OnPaintExpandedItemContent(WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID( this );
	lParam;
CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA * p_picd =
		(CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA *)wParam;
	ASSERT( p_picd != NULL );
	ASSERT( p_picd->m_dc.GetSafeHdc() != NULL );
	if( ! OnGbwPaintExpandedItemContent(*p_picd) )
		return 0;
	return (!0);
}

bool CExtGridBaseWnd::OnGbwPaintExpandedItemContent(
	CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA & picd
	) const
{
	ASSERT_VALID( this );
	ASSERT( picd.m_dc.GetSafeHdc() != NULL );
	picd;
	return false;
}

#if (!defined __EXT_MFC_NO_GRIDWND)

/////////////////////////////////////////////////////////////////////////////
// CExtAlloc

CExtAlloc::CExtAlloc(
	ULONG nCountInChunk,
	bool bHeapMinimizeOnTheFly,
	IUnknown * pUnkOuter,
	bool bZeroNewMemory
	)
	: m_pUnkOuter( pUnkOuter )
	, m_bZeroNewMemory( bZeroNewMemory )
	, m_nRefCount( 1L )
	, m_nCountInChunk( (nCountInChunk > __EXT_ALLOC_MAX_REASONABLE_COUNT_IN_CHUNK ) ? __EXT_ALLOC_MAX_REASONABLE_COUNT_IN_CHUNK : nCountInChunk )
	, m_nSizeFreeMap( 0L )
	, m_bHeapMinimizeOnTheFly( bHeapMinimizeOnTheFly )
#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	, m_nDebugHighLevelAllocNo( 0L )
	, m_nDebugHighLevelAllocCount( 0L )
	, m_nDebugHighLevelBreakAllocNo( 0xFFFFFFFF )
	, m_nDebugLowLevelAllocNo( 0L )
	, m_nDebugLowLevelBreakAllocNo( 0xFFFFFFFF )
#endif // __EXT_DEBUG_ALLOC_TRACK_NUMBERS
#ifdef _DEBUG
	, m_bDebugInDesructor( false )
#endif // _DEBUG
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	m_nSizeFreeMap = nCountInChunk >> 0x03L;
#ifdef __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
	__EXT_DEBUG_ALLOC_TRACE2(
		"     --> CExtAlloc::CExtAlloc(m_nCountInChunk=%lu) is object at %p\n",
		m_nCountInChunk,
		this
		);
#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	__EXT_DEBUG_ALLOC_TRACE1(
		"                        m_nDebugHighLevelBreakAllocNo = %lu\n",
		m_nDebugHighLevelBreakAllocNo
		);
#endif // !__EXT_DEBUG_ALLOC_TRACK_NUMBERS
#endif // __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
	if( m_nCountInChunk <= 1 )
		return;
	if( (nCountInChunk & 0x07L) > 0 )
		m_nSizeFreeMap ++;
	ASSERT( m_nSizeFreeMap >= 1 && m_nSizeFreeMap <= ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
#ifdef __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	__EXT_DEBUG_ALLOC_TRACE1(
		"                        m_nSizeFreeMap = %lu\n",
		m_nSizeFreeMap
		);
#endif // !__EXT_DEBUG_ALLOC_TRACK_NUMBERS
#endif // __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
}

CExtAlloc::~CExtAlloc()
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount == 0L );
#ifdef _DEBUG
	m_bDebugInDesructor = true;
#endif // _DEBUG
#ifdef __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
	__EXT_DEBUG_ALLOC_TRACE1(
		"     --> CExtAlloc::~CExtAlloc() is object at %p\n",
		this
		);
#endif // __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	ASSERT( m_nDebugHighLevelAllocCount == 0L );
#endif // __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	if( m_nCountInChunk <= 1 )
	{
#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
		_LowLevel_trace_leaks();
#endif // __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	} // if( m_nCountInChunk <= 1 )
	else
	{
		_HighLevel_NHeapMinimize();
#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
		_HighLevel_trace_leaks();
		_LowLevel_trace_leaks();
#endif // __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	} // else from if( m_nCountInChunk <= 1 )
#ifdef _DEBUG
	m_bDebugInDesructor = false;
#endif // _DEBUG
}

bool CExtAlloc::_LowLevel_IsValidPointer(
	LPCVOID pMemory,
	ULONG nByteCount,
	bool bCheckReadAccess, // = true
	bool bCheckWriteAccess // = true
	)
{
	if( pMemory == NULL )
		return false;
	if( bCheckReadAccess )
	{
		if(	::IsBadReadPtr(
				const_cast < LPVOID > ( pMemory ),
				nByteCount
				)
			)
			return false;
	} // if( bCheckReadAccess )
	if( bCheckWriteAccess )
	{
		if(	::IsBadWritePtr(
				const_cast < LPVOID > ( pMemory ),
				nByteCount
				)
			)
			return false;
	} // if( bCheckWriteAccess )
	return true;
}

bool CExtAlloc::_LowLevel_IsValidPointer(
	const CExtAlloc * pAlloc,
	bool bCheckReadAccess, // = true
	bool bCheckWriteAccess // = true
	)
{
	return
		_LowLevel_IsValidPointer(
			pAlloc,
			sizeof( CExtAlloc ),
			bCheckReadAccess,
			bCheckWriteAccess
			);
}

#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
void CExtAlloc::_HighLevel_trace_leaks()
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L || m_bDebugInDesructor );
	ASSERT( m_nCountInChunk > 1L );
	ASSERT( m_nSizeFreeMap >= 1 && m_nSizeFreeMap <= ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
POSITION pos = m_mapHighLevelChunks.GetStartPosition();
///	ASSERT( pos == NULL ); // assert no leaks
	for( ; pos != NULL; )
	{
		ULONG nItemSize = 0L;
		_N_header_block_t * pHdrN = NULL;
		m_mapHighLevelChunks.GetNextAssoc( pos, nItemSize, pHdrN );
		ASSERT( nItemSize > 0 );
		ASSERT( pHdrN != NULL);
		if( pHdrN == NULL )
			continue;
		
///#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
		for( _N_header_block_t * pWalk = pHdrN; pWalk != NULL; pWalk = pWalk->m_pNext )
		{
			ASSERT( pWalk->m_pMalloc == this );
			LPBYTE pFreeMap = (LPBYTE(pWalk)) + sizeof(_N_header_block_t);
			for( ULONG i = 0; i < m_nCountInChunk; i++ )
			{
				ULONG nChunk = i >> 0x03L;
				ASSERT( nChunk < ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
				ULONG nBit = i & 0x07L;
				ASSERT( nBit < 8L );
				if( ((pFreeMap[nChunk])&(BYTE(1L<<nBit))) != BYTE(0) )
				{
					LPBYTE pMemory = pFreeMap + m_nSizeFreeMap;
					pMemory += (pWalk->m_nItemSize + sizeof(_N_header_chunk_t)) * i;
					__EXT_DEBUG_ALLOC_TRACE3(
						"     --> CExtAlloc::_HighLevel_trace_leaks() leaks detected, leak ptr = %p, allocation No = %lu, item size = %lu\n",
						pMemory + sizeof(_N_header_chunk_t),
						((_N_header_chunk_t *)pMemory)->m_nDebugHighLevelAllocNo,
						pWalk->m_nItemSize
						);
				} // if( ((pFreeMap[nChunk])&(BYTE(1L<<nBit))) != BYTE(0) )
			} // for( ULONG i = 0; i < m_nCountInChunk; i++ )
		} // for( _N_header_block_t * pWalk = pHdrN; pWalk != NULL; pWalk = pWalk->m_pNext )
		for( pWalk = pHdrN->m_pPrev; pWalk != NULL; pWalk = pWalk->m_pPrev )
		{
			ASSERT( pWalk->m_pMalloc == this );
			LPBYTE pFreeMap = (LPBYTE(pWalk)) + sizeof(_N_header_block_t);
			for( ULONG i = 0; i < m_nCountInChunk; i++ )
			{
				ULONG nChunk = i >> 0x03L;
				ASSERT( nChunk < ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
				ULONG nBit = i & 0x07L;
				ASSERT( nBit < 8L );
				if( ((pFreeMap[nChunk])&(BYTE(1L<<nBit))) != BYTE(0) )
				{
					LPBYTE pMemory = pFreeMap + m_nSizeFreeMap;
					pMemory += (pWalk->m_nItemSize + sizeof(_N_header_chunk_t)) * i;
					__EXT_DEBUG_ALLOC_TRACE3(
						"     --> CExtAlloc::_HighLevel_trace_leaks() leaks detected, leak ptr = %p, allocation No = %lu, item size = %lu\n",
						pMemory + sizeof(_N_header_chunk_t),
						((_N_header_chunk_t *)pMemory)->m_nDebugHighLevelAllocNo,
						pWalk->m_nItemSize
						);
				} // if( ((pFreeMap[nChunk])&(BYTE(1L<<nBit))) != BYTE(0) )
			} // for( ULONG i = 0; i < m_nCountInChunk; i++ )
		} // for( pWalk = pHdrN->m_pPrev; pWalk != NULL; pWalk = pWalk->m_pPrev )
///#endif // __EXT_DEBUG_ALLOC_TRACK_NUMBERS

		ASSERT( pHdrN->m_pNext == NULL );
		ASSERT( pHdrN->m_pPrev == NULL );
///#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
		ASSERT( pHdrN->m_nDebugHighLevelAllocCount == 0L );
///#endif // __EXT_DEBUG_ALLOC_TRACK_NUMBERS
		_LowLevel_free( pHdrN );
	} // for( ; pos != NULL; )
	m_mapHighLevelChunks.RemoveAll();
}
#endif // __EXT_DEBUG_ALLOC_TRACK_NUMBERS

void * CExtAlloc::_LowLevel_alloc( ULONG nSize )
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( nSize > 0 );
#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	if( m_nDebugLowLevelAllocNo == m_nDebugLowLevelBreakAllocNo )
	{
		// break at m_nDebugLowLevelBreakAllocNo CRT library allocation
		ASSERT( FALSE );
	} // if( m_nDebugLowLevelAllocNo == m_nDebugLowLevelBreakAllocNo )
LPBYTE pMemory = (LPBYTE)::malloc( size_t(nSize)+sizeof(_CRT_header_chunk_t) );
	ASSERT( pMemory != NULL );
	((_CRT_header_chunk_t*)pMemory)->m_pMalloc = this;
	((_CRT_header_chunk_t*)pMemory)->m_nBlockSize = nSize;
	((_CRT_header_chunk_t*)pMemory)->m_nDebugLowLevelAllocNo = m_nDebugLowLevelAllocNo;
	m_nDebugLowLevelAllocNo ++;
	m_mapLowLevelAllocationsTrace.SetAt(
		( pMemory + sizeof(_CRT_header_chunk_t) ),
		nSize
		);
	return ( pMemory + sizeof(_CRT_header_chunk_t) );
#else // __EXT_DEBUG_ALLOC_TRACK_NUMBERS
void * pMemory = ::malloc( size_t(nSize) );
	ASSERT( pMemory != NULL );
	return pMemory;
#endif // !__EXT_DEBUG_ALLOC_TRACK_NUMBERS
}

void CExtAlloc::_LowLevel_free( void * pMemory )
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( pMemory != NULL );
#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
ULONG nAllcatedSize = 0L;
	VERIFY( m_mapLowLevelAllocationsTrace.Lookup(pMemory,nAllcatedSize) );
	ASSERT( nAllcatedSize > 0 );
	pMemory = LPBYTE(pMemory) - sizeof(_CRT_header_chunk_t);
	ASSERT( ((_CRT_header_chunk_t*)pMemory)->m_pMalloc == this );
	ASSERT( ((_CRT_header_chunk_t*)pMemory)->m_nBlockSize == nAllcatedSize );
	ASSERT(
		_LowLevel_IsValidPointer(
			pMemory,
			sizeof(_CRT_header_chunk_t)+nAllcatedSize
			)
		);
	VERIFY( m_mapLowLevelAllocationsTrace.RemoveKey( (LPBYTE(pMemory) + sizeof(_CRT_header_chunk_t)) ) );
	::free( pMemory );
#else // __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	ASSERT( _LowLevel_IsValidPointer( pMemory, 1 ) );
	::free( pMemory );
#endif // !__EXT_DEBUG_ALLOC_TRACK_NUMBERS
}

void * CExtAlloc::_LowLevel_realloc( void * pMemory, ULONG nSize )
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( pMemory != NULL );
	ASSERT( nSize > 0 );
#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
ULONG nAllcatedSize = 0L;
	VERIFY(
		m_mapLowLevelAllocationsTrace.Lookup(
			pMemory,
			nAllcatedSize
			)
		);
	ASSERT( nAllcatedSize > 0 );
	pMemory = LPBYTE(pMemory) - sizeof(_CRT_header_chunk_t);
	ASSERT( ((_CRT_header_chunk_t*)pMemory)->m_pMalloc == this );
	ASSERT( ((_CRT_header_chunk_t*)pMemory)->m_nBlockSize == nAllcatedSize );
	ASSERT(
		_LowLevel_IsValidPointer(
			pMemory,
			sizeof(_CRT_header_chunk_t)+nAllcatedSize
			)
		);
	if( m_nDebugLowLevelAllocNo == m_nDebugLowLevelBreakAllocNo )
	{
		// break at m_nDebugLowLevelBreakAllocNo CRT library allocation
		ASSERT( FALSE );
	} // if( m_nDebugLowLevelAllocNo == m_nDebugLowLevelBreakAllocNo )
LPBYTE pMemoryNew = (LPBYTE)::realloc( pMemory, size_t(nSize)+sizeof(_CRT_header_chunk_t) );
	ASSERT( pMemoryNew != NULL );
	if( pMemoryNew == NULL )
		return NULL;
	VERIFY(
		m_mapLowLevelAllocationsTrace.RemoveKey(
			LPBYTE(pMemory) + sizeof(_CRT_header_chunk_t)
			)
		);
	ASSERT( ((_CRT_header_chunk_t*)pMemoryNew)->m_pMalloc == this );
	((_CRT_header_chunk_t*)pMemoryNew)->m_nBlockSize = nSize;
	((_CRT_header_chunk_t*)pMemoryNew)->m_nDebugLowLevelAllocNo = m_nDebugLowLevelAllocNo;
	m_nDebugLowLevelAllocNo ++;
	m_mapLowLevelAllocationsTrace.SetAt(
		( pMemoryNew + sizeof(_CRT_header_chunk_t) ),
		nSize
		);
	return ( pMemoryNew + sizeof(_CRT_header_chunk_t) );
#else // __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	ASSERT( _LowLevel_IsValidPointer( pMemory, 1 ) );
	pMemory = ::realloc( pMemory, size_t(nSize) );
	ASSERT( pMemory != NULL );
	return pMemory;
#endif // !__EXT_DEBUG_ALLOC_TRACK_NUMBERS
}

#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
void CExtAlloc::_LowLevel_trace_leaks()
{
POSITION pos = m_mapLowLevelAllocationsTrace.GetStartPosition();
	ASSERT( pos == NULL ); // NULL if no CRT library leaks
	for( ; pos != NULL ; )
	{
		LPVOID pMemory = NULL;
		ULONG nAllocatedSize = 0L;
		m_mapLowLevelAllocationsTrace.GetNextAssoc( pos, pMemory, nAllocatedSize );
		ASSERT( pMemory != NULL && nAllocatedSize > 0 );
		_CRT_header_chunk_t * ptr = (_CRT_header_chunk_t*)(LPBYTE(pMemory)-sizeof(_CRT_header_chunk_t));
		ASSERT( ptr->m_pMalloc == this );
		ASSERT( ptr->m_nBlockSize == nAllocatedSize );
		__EXT_DEBUG_ALLOC_TRACE3(
			"     --> CExtAlloc::_LowLevel_trace_leaks() leak ptr = %p, allocation No = %lu, item size = %lu\n",
			pMemory,
			ptr->m_nDebugLowLevelAllocNo,
			ptr->m_nBlockSize
			);
	} // for( ; pos != NULL ; )
}
#endif // __EXT_DEBUG_ALLOC_TRACK_NUMBERS

IMalloc * CExtAlloc::CreateObject(
	ULONG nCountInChunk, // = 256
	bool bHeapMinimizeOnTheFly, // = false
	IUnknown * pUnkOuter, // = NULL
	bool bZeroNewMemory // = true
	)
{
	try
	{
		return new
			CExtAlloc(
				nCountInChunk,
				bHeapMinimizeOnTheFly,
				pUnkOuter,
				bZeroNewMemory
				);
	} // try
	catch( CException * pException )
	{
		__EXT_DEBUG_ALLOC_TRACE1(
			"     --> CExtAlloc::CreateObject(nCountInChunk=%lu) exception\n",
			nCountInChunk
			);
		ASSERT( FALSE );
#ifdef _DEBUG
		TCHAR strError[4096];
		if( pException->GetErrorMessage( strError, 4096-1 ) )
		{
			__EXT_DEBUG_ALLOC_TRACE1(
				"              Description = \"%s\"\n",
				strError
				);
		} // if( pException->GetErrorMessage( strError, 4096-1 ) )
#endif // _DEBUG
		pException->Delete();
	} // catch( CException * pException )
	catch( ... )
	{
		__EXT_DEBUG_ALLOC_TRACE1(
			"     --> CExtAlloc::CreateObject(nCountInChunk=%lu) unknown exception\n",
			nCountInChunk
			);
		ASSERT( FALSE );
	} // catch( ... )
	return NULL;
}

HRESULT STDMETHODCALLTYPE CExtAlloc::QueryInterface( 
    REFIID riid,
    void ** ppvObject
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L && (!m_bDebugInDesructor) );
	if( ppvObject == NULL )
	{
		ASSERT( FALSE );
		return E_NOINTERFACE;
	} // if( ppvObject == NULL )
	if(		riid == IID_IUnknown
		||	riid == IID_IMalloc
		)
	{
		if(		riid == IID_IUnknown
			&&	m_pUnkOuter != NULL
			)
			(*ppvObject) = m_pUnkOuter;
		else
			(*ppvObject) = this;
		((IUnknown *)(*ppvObject))->AddRef();
		return S_OK;
	}
	if( m_pUnkOuter != NULL )
		return m_pUnkOuter->QueryInterface( riid, ppvObject );
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CExtAlloc::AddRef(void)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L && (!m_bDebugInDesructor) );
	m_nRefCount ++;
	return m_nRefCount;
}

ULONG STDMETHODCALLTYPE CExtAlloc::Release(void)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L && (!m_bDebugInDesructor) );
	m_nRefCount --;
	if( m_nRefCount == 0L )
	{
		delete this;
		return 0L;
	}
	return m_nRefCount;
}

void * CExtAlloc::_HighLevel_1Alloc(
	ULONG cb
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L && (!m_bDebugInDesructor) );
	ASSERT( m_nCountInChunk <= 1L );
	ASSERT( cb > 0 );
ULONG cbAlloc = cb + sizeof(_1_header_chunk_t);
LPBYTE pMemory = (LPBYTE)_LowLevel_alloc( cbAlloc );
	if( pMemory == NULL )
	{
		ASSERT( FALSE );
		__EXT_DEBUG_ALLOC_TRACE1(
			"     --> CExtAlloc::_HighLevel_1Alloc(%lu) CRT allocation failed\n",
			cb
			);
		return NULL;
	} // if( pMemory == NULL )
	((_1_header_chunk_t*)pMemory)->m_pMalloc = this;
	((_1_header_chunk_t*)pMemory)->m_nBlockSize = cb;
#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	((_1_header_chunk_t*)pMemory)->m_nDebugHighLevelAllocNo = m_nDebugHighLevelAllocNo;
	if( m_nDebugHighLevelAllocNo == m_nDebugHighLevelBreakAllocNo )
	{
		// break at m_nDebugHighLevelBreakAllocNo allocation number
		ASSERT( FALSE );
		__EXT_DEBUG_ALLOC_TRACE2(
			"     --> CExtAlloc::_HighLevel_1Alloc(%lu) breaking at allocation No %lu\n",
			cb,
			m_nDebugHighLevelBreakAllocNo
			);
	} // if( m_nDebugHighLevelAllocNo == m_nDebugHighLevelBreakAllocNo )
#ifdef __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
	__EXT_DEBUG_ALLOC_TRACE3(
		"     --> CExtAlloc::_HighLevel_1Alloc(%lu) returned %p with allocation No %lu\n",
		cb,
		( pMemory + sizeof(_1_header_chunk_t) ),
		m_nDebugHighLevelAllocNo
		);
#endif // __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
	m_nDebugHighLevelAllocNo ++;
	m_nDebugHighLevelAllocCount ++;
#else // __EXT_DEBUG_ALLOC_TRACK_NUMBERS
#ifdef __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
	__EXT_DEBUG_ALLOC_TRACE2(
		"     --> CExtAlloc::_HighLevel_1Alloc(%lu) returned %p\n",
		cb,
		( pMemory + sizeof(_1_header_chunk_t) )
		);
#endif // __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
#endif // !__EXT_DEBUG_ALLOC_TRACK_NUMBERS
	pMemory += sizeof(_1_header_chunk_t);
	if( m_bZeroNewMemory )
		::memset( pMemory, 0, cb );
	return pMemory;
}

void * CExtAlloc::_HighLevel_NAlloc(
	ULONG cb
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L && (!m_bDebugInDesructor) );
	ASSERT( m_nCountInChunk > 1L );
	ASSERT( m_nSizeFreeMap >= 1 && m_nSizeFreeMap <= ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
	ASSERT( cb > 0 );
_N_header_block_t * pHdr = _HighLevel_N_BlockGetWithFI( cb );
	if( pHdr == NULL )
	{
		__EXT_DEBUG_ALLOC_TRACE1(
			"     --> CExtAlloc::_HighLevel_NAlloc(%lu) _HighLevel_N_BlockGetWithFI() allocation failed\n",
			cb
			);
		ASSERT( FALSE );
		return NULL;
	} // if( pHdr == NULL )
	ASSERT( pHdr->m_nItemSize == cb );
LPVOID pv = _HighLevel_N_BlockGetFirstFI( pHdr, true );
	ASSERT( pv != NULL );
#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	((_N_header_chunk_t*) (LPBYTE(pv)-sizeof(_N_header_chunk_t)) )->m_nDebugHighLevelAllocNo = m_nDebugHighLevelAllocNo;
	if( m_nDebugHighLevelAllocNo == m_nDebugHighLevelBreakAllocNo )
	{
		// break at m_nDebugHighLevelBreakAllocNo allocation number
		__EXT_DEBUG_ALLOC_TRACE2(
			"     --> CExtAlloc::_HighLevel_NAlloc(%lu) breaking at allocation No %lu\n",
			cb,
			m_nDebugHighLevelBreakAllocNo
			);
		ASSERT( FALSE );
	} // if( m_nDebugHighLevelAllocNo == m_nDebugHighLevelBreakAllocNo )
#ifdef __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
	__EXT_DEBUG_ALLOC_TRACE3(
		"     --> CExtAlloc::_HighLevel_NAlloc(%lu) returned %p with allocation No %lu\n",
		cb,
		pv,
		m_nDebugHighLevelAllocNo
		);
#endif // __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
	m_nDebugHighLevelAllocNo ++;
	m_nDebugHighLevelAllocCount ++;
#else // __EXT_DEBUG_ALLOC_TRACK_NUMBERS
#ifdef __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
	__EXT_DEBUG_ALLOC_TRACE2(
		"     --> CExtAlloc::_HighLevel_NAlloc(%lu) returned %p\n",
		cb,
		pv
		);
#endif // __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
#endif // !__EXT_DEBUG_ALLOC_TRACK_NUMBERS
	if( m_bZeroNewMemory )
		::memset( pv, 0, cb );
	return pv;
}

void * CExtAlloc::_HighLevel_N_BlockGetFirstFI(
	CExtAlloc::_N_header_block_t * pHdr,
	bool bMarkAllocated
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nCountInChunk > 1L );
	ASSERT( m_nSizeFreeMap >= 1 && m_nSizeFreeMap <= ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
	ASSERT( pHdr != NULL );
	ASSERT( pHdr->m_pMalloc == this );
	ASSERT( pHdr->m_nItemSize > 0 );
LPBYTE pFreeMap = (LPBYTE(pHdr)) + sizeof(_N_header_block_t);
	for( ULONG i = 0; i < m_nCountInChunk; i++ )
	{
		ULONG nChunk = i >> 0x03L;
		ASSERT( nChunk < ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
		ULONG nBit = i & 0x07L;
		ASSERT( nBit < 8L );
		if( ((pFreeMap[nChunk])&(BYTE(1L<<nBit))) != BYTE(0) )
			continue;
		if( bMarkAllocated )
			pFreeMap[nChunk] = BYTE( pFreeMap[nChunk] | (BYTE(1L<<nBit)) );
		LPBYTE pMemory = LPBYTE(pHdr) + sizeof(_N_header_block_t) + m_nSizeFreeMap;
		pMemory += (sizeof(_N_header_chunk_t) + pHdr->m_nItemSize) * i;
		return ( pMemory + sizeof(_N_header_chunk_t) );
	} // for( ULONG i = 0; i < m_nCountInChunk; i++ )
	return NULL;
}

bool CExtAlloc::_HighLevel_N_BlockIsFullyFree(
	CExtAlloc::_N_header_block_t * pHdr
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L || m_bDebugInDesructor );
	ASSERT( m_nCountInChunk > 1L );
	ASSERT( m_nSizeFreeMap >= 1 && m_nSizeFreeMap <= ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
	ASSERT( pHdr != NULL );
	ASSERT( pHdr->m_pMalloc == this );
	ASSERT( pHdr->m_nItemSize > 0 );
LPBYTE pFreeMap = (LPBYTE(pHdr)) + sizeof(_N_header_block_t);
	for( ULONG i = 0; i < m_nSizeFreeMap; i++ )
	{
		if( pFreeMap[i] != 0 )
			return false;
	} // for( ULONG i = 0; i < m_nSizeFreeMap; i++ )
	return true;
}

bool CExtAlloc::_HighLevel_N_BlockHasFI(
	CExtAlloc::_N_header_block_t * pHdr
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L || m_bDebugInDesructor );
	ASSERT( m_nCountInChunk > 1L );
	ASSERT( m_nSizeFreeMap >= 1 && m_nSizeFreeMap <= ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
	ASSERT( pHdr != NULL );
	ASSERT( pHdr->m_pMalloc == this );
	ASSERT( pHdr->m_nItemSize > 0 );
LPBYTE pFreeMap = (LPBYTE(pHdr)) + sizeof(_N_header_block_t);
	for( ULONG i = 0; i < m_nCountInChunk; i++ )
	{
		ULONG nChunk = i >> 0x03L;
		ASSERT( nChunk < ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
		ULONG nBit = i & 0x07L;
		ASSERT( nBit < 8L );
		if( ((pFreeMap[nChunk])&(BYTE(1L<<nBit))) == BYTE(0) )
			return true;
	} // for( ULONG i = 0; i < m_nCountInChunk; i++ )
	return false;
}

CExtAlloc::_N_header_block_t * CExtAlloc::_HighLevel_N_BlockGetWithFI(
	ULONG nSizeItem
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L || m_bDebugInDesructor );
	ASSERT( m_nCountInChunk > 1L );
	ASSERT( m_nSizeFreeMap >= 1 && m_nSizeFreeMap <= ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
_N_header_block_t * pOther = NULL;
	if( ! m_mapHighLevelChunks.Lookup( nSizeItem, pOther )	)
		pOther = NULL;
	if( pOther != NULL )
	{
		for( _N_header_block_t * pHdr = pOther; pHdr != NULL; pHdr = pHdr->m_pNext )
		{
			ASSERT( pHdr->m_pMalloc == this );
			ASSERT( pHdr->m_nItemSize == nSizeItem );
			if( _HighLevel_N_BlockHasFI(pHdr) )
			{
				if( pHdr != pOther )
					m_mapHighLevelChunks.SetAt( nSizeItem, pHdr );
				return pHdr;
			} // if( _HighLevel_N_BlockHasFI(pHdr) )
		} // for( _N_header_block_t * pHdr = pOther; pHdr != NULL; pHdr = pHdr->m_pNext )
		for( pHdr = pOther; pHdr != NULL; pHdr = pHdr->m_pPrev )
		{
			ASSERT( pHdr->m_pMalloc == this );
			ASSERT( pHdr->m_nItemSize == nSizeItem );
			if( _HighLevel_N_BlockHasFI(pHdr) )
			{
				if( pHdr != pOther )
					m_mapHighLevelChunks.SetAt( nSizeItem, pHdr );
				return pHdr;
			} // if( _HighLevel_N_BlockHasFI(pHdr) )
		} // for( pHdr = pOther; pHdr != NULL; pHdr = pHdr->m_pPrev )
	} // if( pOther != NULL )
ULONG nBlockSize = _HighLevel_N_BlockGetTotlalSize(nSizeItem);
_N_header_block_t * pHdr = (_N_header_block_t *) _LowLevel_alloc( nBlockSize );
	if( pHdr == NULL )
	{
		__EXT_DEBUG_ALLOC_TRACE1(
			"     --> CExtAlloc::_HighLevel_N_BlockGetWithFI(nSizeItem=%lu) CRT allocation failed\n",
			nSizeItem
			);
		ASSERT( FALSE );
		return NULL;
	} // if( pHdr == NULL )
	::memset( pHdr, 0, nBlockSize );
	pHdr->m_pMalloc = this;
	pHdr->m_nItemSize = nSizeItem;
LPBYTE pMemory = LPBYTE(pHdr) + sizeof(_N_header_block_t) + m_nSizeFreeMap;
	for(	ULONG i = 0;
			i < m_nCountInChunk;
			i++, pMemory += (sizeof(_N_header_chunk_t) + nSizeItem)
			)
	{
		((_N_header_chunk_t*)pMemory)->m_nHdrOffset =
			ULONG(pMemory) - ULONG(pHdr);
	}
	if( pOther == NULL )
	{
		m_mapHighLevelChunks.SetAt( nSizeItem, pHdr );
		return pHdr;
	} // if( pOther == NULL )
	ASSERT( pOther->m_pMalloc == this );
	pHdr->m_pNext = pOther->m_pNext;
	if( pHdr->m_pNext != NULL )
	{
		ASSERT( pHdr->m_pNext->m_pPrev->m_pMalloc == this );
		ASSERT( pHdr->m_pNext->m_pPrev == pOther );
		pHdr->m_pNext->m_pPrev = pHdr;
	} // if( pHdr->m_pNext != NULL )
	pHdr->m_pPrev = pOther;
	pOther->m_pNext = pHdr;
	return pHdr;
}

ULONG CExtAlloc::_HighLevel_N_BlockGetTotlalSize(
	ULONG nSizeItem
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L || m_bDebugInDesructor );
	ASSERT( m_nCountInChunk > 1L );
	ASSERT( m_nSizeFreeMap >= 1 && m_nSizeFreeMap <= ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
	ASSERT( nSizeItem > 0 );
ULONG nSize =
		sizeof(_N_header_block_t)
		+ m_nSizeFreeMap
		+ ( (nSizeItem + sizeof(_N_header_chunk_t)) * m_nCountInChunk )
		;
	return nSize;
}

void * STDMETHODCALLTYPE CExtAlloc::Alloc(
    ULONG cb
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L );
	if( cb == 0L )
	{
		ASSERT( FALSE );
		__EXT_DEBUG_ALLOC_TRACE0(
			"     --> CExtAlloc::Alloc(zero) invalid parameter\n"
			);
		return NULL;
	} // if( cb == 0L )
	if( m_nCountInChunk <= 1L )
		return _HighLevel_1Alloc( cb );
	return _HighLevel_NAlloc( cb );
}

void * CExtAlloc::_HighLevel_1Realloc( 
    void * pv,
    ULONG cb
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L );
	ASSERT( m_nCountInChunk <= 1L );
	ASSERT( pv != NULL );
	ASSERT( cb > 0 );
LPBYTE pMemory = (LPBYTE)pv;
	pMemory -= sizeof(_1_header_chunk_t);
	if(		((_1_header_chunk_t*)pMemory)->m_pMalloc != this
		||	((_1_header_chunk_t*)pMemory)->m_nBlockSize == 0L
		||	(! _LowLevel_IsValidPointer(
				pMemory,
				sizeof(_1_header_chunk_t)+((_1_header_chunk_t*)pMemory)->m_nBlockSize
				)
			)
		)
	{
		__EXT_DEBUG_ALLOC_TRACE2(
			"     --> CExtAlloc::_HighLevel_1Realloc(%p,%lu) invalid pointer param\n",
			pv,
			cb
			);
		ASSERT( FALSE );
		// not valid or not allocated by this IMalloc
		return NULL;
	} // if( ((_1_header_chunk_t*)pMemory)->m_pMalloc != this ) ...
ULONG cbRealloc = cb + sizeof(_1_header_chunk_t);
	pMemory = (LPBYTE)_LowLevel_realloc( pMemory, cbRealloc );
	if( pMemory == NULL )
	{
		__EXT_DEBUG_ALLOC_TRACE2(
			"     --> CExtAlloc::_HighLevel_1Realloc(%p,%lu) CRT reallocation failed\n",
			pv,
			cb
			);
		ASSERT( FALSE );
		return NULL;
	} // if( pMemory == NULL )
ULONG cbOld = ((_1_header_chunk_t*)pMemory)->m_nBlockSize;
	((_1_header_chunk_t*)pMemory)->m_pMalloc = this;
	((_1_header_chunk_t*)pMemory)->m_nBlockSize = cb;
	pMemory += sizeof(_1_header_chunk_t);
	if( m_bZeroNewMemory && cb > cbOld )
		::memset(
			pMemory + cbOld,
			0,
			cb - cbOld
			);
	return pMemory;
}

void * CExtAlloc::_HighLevel_NRealloc( 
    void * pv,
    ULONG cb
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L );
	ASSERT( m_nCountInChunk > 1L );
	ASSERT( m_nSizeFreeMap >= 1 && m_nSizeFreeMap <= ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
	ASSERT( pv != NULL );
	ASSERT( cb > 0 );
ULONG nOldSize = _HighLevel_NGetSize( pv ); 
	if( nOldSize == 0 )
	{
		__EXT_DEBUG_ALLOC_TRACE2(
			"     --> CExtAlloc::_HighLevel_NRealloc(%p,%lu) _HighLevel_NGetSize() failed\n",
			pv,
			cb
			);
		ASSERT( FALSE );
		return NULL;
	} // if( nOldSize == 0 )
	if( nOldSize == cb )
		return pv;
void * pvNew = _HighLevel_NAlloc( cb );
	if( pvNew == NULL )
	{
		__EXT_DEBUG_ALLOC_TRACE2(
			"     --> CExtAlloc::_HighLevel_NRealloc(%p,%lu) _HighLevel_NAlloc() failed\n",
			pv,
			cb
			);
		ASSERT( FALSE );
		return NULL;
	} // if( pvNew == NULL )
ULONG nCopySize = min( nOldSize, cb );
	::memcpy( pvNew, pv, size_t(nCopySize) );
	_HighLevel_NFree( pv );
	if( m_bZeroNewMemory && cb > nOldSize )
		::memset(
			(LPBYTE(pvNew)) + nOldSize,
			0,
			cb - nOldSize
			);
	return pvNew;
}

void * STDMETHODCALLTYPE CExtAlloc::Realloc( 
    void * pv,
    ULONG cb
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L );
	if( cb == 0 || pv == NULL )
	{
		__EXT_DEBUG_ALLOC_TRACE2(
			"     --> CExtAlloc::Realloc(%p,%lu) invalid parameters\n",
			pv,
			cb
			);
		ASSERT( FALSE );
		return NULL;
	} // if( cb == 0 || pv == NULL )
	if( m_nCountInChunk <= 1L )
		return _HighLevel_1Realloc( pv, cb );
	return _HighLevel_NRealloc( pv, cb );
}

void CExtAlloc::_HighLevel_1Free( 
	void * pv
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L || m_bDebugInDesructor );
	ASSERT( m_nCountInChunk <= 1L );
	ASSERT( pv != NULL );
LPBYTE pMemory = (LPBYTE)pv;
	pMemory -= sizeof(_1_header_chunk_t);
	if(		((_1_header_chunk_t*)pMemory)->m_pMalloc != this
		||	((_1_header_chunk_t*)pMemory)->m_nBlockSize == 0L
		||	(! _LowLevel_IsValidPointer(
				pMemory,
				sizeof(_1_header_chunk_t)+((_1_header_chunk_t*)pMemory)->m_nBlockSize
				)
			)
		)
	{
		__EXT_DEBUG_ALLOC_TRACE1(
			"     --> CExtAlloc::_HighLevel_1Free(%p) with invalid pointer\n",
			pv
			);
		ASSERT( FALSE );
		// not valid or not allocated by this IMalloc
		return;
	} // if( ((_1_header_chunk_t*)pMemory)->m_pMalloc != this ) ...
#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
#ifdef __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
	__EXT_DEBUG_ALLOC_TRACE3(
		"     --> CExtAlloc::_HighLevel_1Free(%p) with allocation No %lu (size = %lu)\n",
		pv,
		((_1_header_chunk_t*)pMemory)->m_nDebugHighLevelAllocNo,
		((_1_header_chunk_t*)pMemory)->m_nBlockSize
		);
#endif // __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
	ASSERT( m_nDebugHighLevelAllocCount > 0 );
	m_nDebugHighLevelAllocCount --;
#else // __EXT_DEBUG_ALLOC_TRACK_NUMBERS
#ifdef __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
	__EXT_DEBUG_ALLOC_TRACE2(
		"     --> CExtAlloc::_HighLevel_1Free(%p) succeeded (size = %lu)\n",
		pv,
		((_1_header_chunk_t*)pMemory)->m_nBlockSize
		);
#endif // __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
#endif // !__EXT_DEBUG_ALLOC_TRACK_NUMBERS
	_LowLevel_free( pMemory );
}

void CExtAlloc::_HighLevel_N_BlockOptimizeEmpty(
	CExtAlloc::_N_header_block_t * pHdr
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L || m_bDebugInDesructor );
	ASSERT( m_nCountInChunk > 1L );
	ASSERT( m_nSizeFreeMap >= 1 && m_nSizeFreeMap <= ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
	ASSERT( pHdr != NULL );
	ASSERT( pHdr->m_pMalloc == this );
	ASSERT( pHdr->m_nItemSize > 0 );
	if( pHdr->m_pPrev == NULL && pHdr->m_pNext == NULL )
		return;
	bool bResetHdrMap = false;
	for( _N_header_block_t * pWalk = pHdr; pWalk != NULL; )
	{
		ASSERT( pWalk->m_pMalloc == this );
		if( !_HighLevel_N_BlockIsFullyFree(pWalk) )
		{
			pWalk = pWalk->m_pNext;
			continue;
		}
		if( pWalk == pHdr && pWalk->m_pNext == NULL )
			break;
		pHdr = pWalk->m_pNext;
		bResetHdrMap = true;
		_HighLevel_N_BlockFree( pWalk );
		pWalk = pHdr;
	} // for( _N_header_block_t * pWalk = pHdr; pWalk != NULL; )
	for( pWalk = pHdr; pWalk != NULL; )
	{
		ASSERT( pWalk->m_pMalloc == this );
		if( !_HighLevel_N_BlockIsFullyFree(pWalk) )
		{
			pWalk = pWalk->m_pPrev;
			continue;
		}
		if( pWalk == pHdr && pWalk->m_pPrev == NULL )
			break;
		pHdr = pWalk->m_pPrev;
		bResetHdrMap = true;
		_HighLevel_N_BlockFree( pWalk );
		pWalk = pHdr;
	} // for( pWalk = pHdr; pWalk != NULL; )
	if( bResetHdrMap )
		m_mapHighLevelChunks.SetAt( pHdr->m_nItemSize, pHdr );
}

void CExtAlloc::_HighLevel_NFree( 
	void * pv
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L || m_bDebugInDesructor );
	ASSERT( m_nCountInChunk > 1L );
	ASSERT( m_nSizeFreeMap >= 1 && m_nSizeFreeMap <= ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
	ASSERT( pv != NULL );
LPBYTE pMemory = LPBYTE(pv) - sizeof(_N_header_chunk_t);
ULONG nHdrOffset = ((_N_header_chunk_t*)pMemory)->m_nHdrOffset;
	pMemory -= nHdrOffset;
	if(		((_N_header_block_t*)pMemory)->m_pMalloc != this
		||	((_N_header_block_t*)pMemory)->m_nItemSize == 0
		||	(!	_LowLevel_IsValidPointer(
					pMemory,
					sizeof(_N_header_block_t)
					+ m_nSizeFreeMap
					+ ( (sizeof(_N_header_chunk_t) + ((_N_header_block_t*)pMemory)->m_nItemSize) * m_nCountInChunk )
					)
				)
		)
	{
		__EXT_DEBUG_ALLOC_TRACE1(
			"     --> CExtAlloc::_HighLevel_NFree(%p) with invalid pointer\n",
			pv
			);
		ASSERT( FALSE );
		// not valid or not allocated by this IMalloc
		return;
	} // if( ((_N_header_block_t*)pMemory)->m_pMalloc != this )
	ASSERT( ((_N_header_block_t*)pMemory)->m_nItemSize > 0 );
ULONG nPureOffset = nHdrOffset - sizeof(_N_header_block_t) - m_nSizeFreeMap;
	ASSERT( ( nPureOffset % (sizeof(_N_header_chunk_t)+((_N_header_block_t*)pMemory)->m_nItemSize) ) == 0 );
ULONG nMapIdx = nPureOffset / (sizeof(_N_header_chunk_t)+((_N_header_block_t*)pMemory)->m_nItemSize);
	ASSERT( nMapIdx < m_nCountInChunk );
ULONG nChunk = nMapIdx >> 0x03L;
	ASSERT( nChunk < ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
ULONG nBit = nMapIdx & 0x07L;
	ASSERT( nBit < 8L );
LPBYTE pFreeMap = pMemory + sizeof(_N_header_block_t);
	pFreeMap[nChunk] = BYTE( pFreeMap[nChunk] & (~(BYTE(1L<<nBit))) );
#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
#ifdef __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
	__EXT_DEBUG_ALLOC_TRACE3(
		"     --> CExtAlloc::_HighLevel_NFree(%p) with allocation No %lu succeeded (size = %lu)\n",
		pv,
		((_N_header_chunk_t*) (LPBYTE(pv)-sizeof(_N_header_chunk_t)))->m_nDebugHighLevelAllocNo,
		((_N_header_block_t*)pMemory)->m_nItemSize
		);
#endif // __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
	ASSERT( m_nDebugHighLevelAllocCount > 0 );
	m_nDebugHighLevelAllocCount --;
#else // __EXT_DEBUG_ALLOC_TRACK_NUMBERS
#ifdef __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
	__EXT_DEBUG_ALLOC_TRACE2(
		"     --> CExtAlloc::_HighLevel_NFree(%p) succeeded (size = %lu)\n",
		pv,
		((_N_header_block_t*)pMemory)->m_nItemSize
		);
#endif // __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
#endif // !__EXT_DEBUG_ALLOC_TRACK_NUMBERS
	_HighLevel_N_BlockOptimizeEmpty( (_N_header_block_t *)pMemory );
	if(		m_bHeapMinimizeOnTheFly
		&&	_HighLevel_N_BlockIsFullyFree(((_N_header_block_t *)pMemory))
		&&	((_N_header_block_t *)pMemory)->m_pPrev == NULL
		&&	((_N_header_block_t *)pMemory)->m_pNext == NULL
		)
	{
		ULONG nItemSize = ((_N_header_block_t *)pMemory)->m_nItemSize;
		_HighLevel_N_BlockFree( ((_N_header_block_t *)pMemory) );
		VERIFY( m_mapHighLevelChunks.RemoveKey(nItemSize) );
	}
}

void CExtAlloc::_HighLevel_N_BlockFree(
	CExtAlloc::_N_header_block_t * pHdr
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L || m_bDebugInDesructor );
	ASSERT( m_nCountInChunk > 1L );
	ASSERT( m_nSizeFreeMap >= 1 && m_nSizeFreeMap <= ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
	ASSERT( pHdr != NULL );
	ASSERT( pHdr->m_pMalloc == this );
	ASSERT( pHdr->m_nItemSize > 0 );
	ASSERT( _HighLevel_N_BlockIsFullyFree(pHdr) );
_N_header_block_t * pPrev = pHdr->m_pPrev;
_N_header_block_t * pNext = pHdr->m_pNext;
	if( pPrev != NULL )
	{
		ASSERT( pPrev->m_pMalloc == this );
		ASSERT( pPrev->m_nItemSize == pHdr->m_nItemSize );
		pPrev->m_pNext = pNext;
	} // if( pPrev != NULL )
	if( pNext != NULL )
	{
		ASSERT( pNext->m_pMalloc == this );
		ASSERT( pNext->m_nItemSize == pHdr->m_nItemSize );
		pNext->m_pPrev = pPrev;
	} // if( pNext != NULL )
	_LowLevel_free( pHdr );
}

void STDMETHODCALLTYPE CExtAlloc::Free( 
    void * pv
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L || m_bDebugInDesructor );
	if( pv == NULL )
	{
		ASSERT( FALSE );
		__EXT_DEBUG_ALLOC_TRACE0(
			"     --> CExtAlloc::Free(NULL) with invalid pointer\n"
			);
		return;
	} // if( pv == NULL )
	if( m_nCountInChunk <= 1L )
		_HighLevel_1Free( pv );
	else
		_HighLevel_NFree( pv );
}

ULONG CExtAlloc::_HighLevel_1GetSize( 
	void * pv
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L );
	ASSERT( m_nCountInChunk <= 1L );
	ASSERT( pv != NULL );
LPBYTE pMemory = (LPBYTE)pv;
	pMemory -= sizeof(_1_header_chunk_t);
	if(		((_1_header_chunk_t*)pMemory)->m_pMalloc != this
		||	((_1_header_chunk_t*)pMemory)->m_nBlockSize == 0L
		)
	{
		__EXT_DEBUG_ALLOC_TRACE1(
			"     --> CExtAlloc::_HighLevel_1GetSize(%p) with invalid pointer\n",
			pv
			);
		ASSERT( FALSE );
		// not valid or not allocated by this IMalloc
		return 0L;
	} // if( ((_1_header_chunk_t*)pMemory)->m_pMalloc != this ) ...
ULONG nSize = ((_1_header_chunk_t*)pMemory)->m_nBlockSize;
	return nSize;
}

ULONG CExtAlloc::_HighLevel_NGetSize( 
	void * pv
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L );
	ASSERT( m_nCountInChunk > 1L );
	ASSERT( m_nSizeFreeMap >= 1 && m_nSizeFreeMap <= ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
	ASSERT( pv != NULL );
LPBYTE pMemory = LPBYTE(pv) - sizeof(_N_header_chunk_t);
ULONG nHdrOffset = ((_N_header_chunk_t*)pMemory)->m_nHdrOffset;
	pMemory -= nHdrOffset;
	if( ((_N_header_block_t*)pMemory)->m_pMalloc != this )
	{
		__EXT_DEBUG_ALLOC_TRACE1(
			"     --> CExtAlloc::_HighLevel_NGetSize(%p) with invalid pointer\n",
			pv
			);
		ASSERT( FALSE );
		// not valid or not allocated by this IMalloc
		return 0L;
	} // if( ((_N_header_block_t*)pMemory)->m_pMalloc != this )
ULONG nItemSize = ((_N_header_block_t*)pMemory)->m_nItemSize;
	ASSERT( nItemSize > 0 );
	return nItemSize;
}

ULONG STDMETHODCALLTYPE CExtAlloc::GetSize( 
    void * pv
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L );
	if( pv == NULL )
	{
		ASSERT( FALSE );
		__EXT_DEBUG_ALLOC_TRACE0(
			"     --> CExtAlloc::GetSize(NULL) with invalid pointer\n"
			);
		return ULONG(-1L);
	} // if( pv == NULL )
	if( m_nCountInChunk <= 1L )
		return _HighLevel_1GetSize( pv );
	return _HighLevel_NGetSize( pv );
}

int CExtAlloc::_HighLevel_1DidAlloc( 
	void * pv
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L || m_bDebugInDesructor );
	ASSERT( m_nCountInChunk <= 1L );
	ASSERT( pv != NULL );
#ifdef _DEBUG
	__try
	{
#endif // _DEBUG
		LPBYTE pMemory = (LPBYTE)pv;
		pMemory -= sizeof(_1_header_chunk_t);
		if( !_LowLevel_IsValidPointer(
				pMemory,
				sizeof(_1_header_chunk_t)+1L
				)
			)
			return 0;
		if( ((_1_header_chunk_t*)pMemory)->m_pMalloc == this )
		{
			ULONG nItemSize = ((_1_header_chunk_t*)pMemory)->m_nBlockSize;
			if( nItemSize == 0 )
				return 0;
			if( !_LowLevel_IsValidPointer(
					pMemory,
					sizeof(_1_header_chunk_t) + nItemSize
					)
				)
				return 0;
			return 1;
		} // if( ((_1_header_chunk_t*)pMemory)->m_pMalloc == this )
#ifdef _DEBUG
	} // __try
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		ASSERT( FALSE );
	} // __except( EXCEPTION_EXECUTE_HANDLER )
#endif // _DEBUG
	return 0;
}

int CExtAlloc::_HighLevel_NDidAlloc( 
	void * pv
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L || m_bDebugInDesructor );
	ASSERT( m_nCountInChunk > 1L );
	ASSERT( m_nSizeFreeMap >= 1 && m_nSizeFreeMap <= ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
	ASSERT( pv != NULL );
#ifdef _DEBUG
	__try
	{
#endif // _DEBUG
		LPBYTE pMemory = LPBYTE(pv) - sizeof(_N_header_chunk_t);
		if( !_LowLevel_IsValidPointer(
				pMemory,
				sizeof(_N_header_chunk_t) + 1L
				)
			)
			return 0;
		ULONG nHdrOffset = ((_N_header_chunk_t*)pMemory)->m_nHdrOffset;
		pMemory -= nHdrOffset;
		if( !_LowLevel_IsValidPointer(
				pMemory,
				sizeof(_N_header_block_t) + m_nSizeFreeMap + 1L
				)
			)
			return 0;
		if( ((_N_header_block_t*)pMemory)->m_pMalloc == this )
		{
			ULONG nItemSize = ((_N_header_block_t*)pMemory)->m_nItemSize;
			if( nItemSize == 0 )
				return 0;
			if( !_LowLevel_IsValidPointer(
					pMemory,
					sizeof(_N_header_block_t)
					+ m_nSizeFreeMap
					+ ( (sizeof(_N_header_chunk_t) + nItemSize) * m_nCountInChunk )
					)
				)
				return 0;
			return 1;
		} // if( ((_N_header_block_t*)pMemory)->m_pMalloc == this )
#ifdef _DEBUG
	} // __try
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		ASSERT( FALSE );
	} // __except( EXCEPTION_EXECUTE_HANDLER )
#endif // _DEBUG
	return 0;
}

int STDMETHODCALLTYPE CExtAlloc::DidAlloc( 
    void * pv
	)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L || m_bDebugInDesructor );
	if( pv == NULL )
	{
		ASSERT( FALSE );
		__EXT_DEBUG_ALLOC_TRACE0(
			"     --> CExtAlloc::DidAlloc(NULL) with invalid pointer\n"
			);
		return -1;
	} // if( pv == NULL )
	if( m_nCountInChunk <= 1L )
		return _HighLevel_1DidAlloc( pv );
	return _HighLevel_NDidAlloc( pv );
}

void CExtAlloc::_HighLevel_1HeapMinimize()
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L || m_bDebugInDesructor );
	ASSERT( m_nCountInChunk <= 1L );
	return;
}

void CExtAlloc::_HighLevel_NHeapMinimize()
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L || m_bDebugInDesructor );
	ASSERT( m_nCountInChunk > 1L );
	ASSERT( m_nSizeFreeMap >= 1 && m_nSizeFreeMap <= ((m_nCountInChunk>>0x03L)+(((m_nCountInChunk&0x07L)>0)?1:0)) );
CList < ULONG, ULONG > _listSizesToRemove;
CList < _N_header_block_t *, _N_header_block_t * > _listPointersToRemove;
POSITION pos = m_mapHighLevelChunks.GetStartPosition();
	for( ; pos != NULL; )
	{
		ULONG nItemSize = 0L;
		_N_header_block_t * pHdr = NULL;
		m_mapHighLevelChunks.GetNextAssoc( pos, nItemSize, pHdr );
		ASSERT( nItemSize > 0 );
		ASSERT( pHdr != NULL);
		if(		_HighLevel_N_BlockIsFullyFree(pHdr)
			&&	pHdr->m_pPrev == NULL
			&&	pHdr->m_pNext == NULL
			)
		{
			_listSizesToRemove.AddTail( nItemSize );
			_listPointersToRemove.AddTail( pHdr );
		}
	} // for( ; pos != NULL; )
	ASSERT( _listSizesToRemove.GetCount() == _listPointersToRemove.GetCount() );
POSITION pos2 = _listPointersToRemove.GetHeadPosition();
	for( pos = _listSizesToRemove.GetHeadPosition(); pos != NULL; )
	{
		ASSERT( pos2 != NULL );
		ULONG nItemSize = _listSizesToRemove.GetNext( pos );
		ASSERT( nItemSize > 0 );
		_N_header_block_t * pHdr = _listPointersToRemove.GetNext( pos2 );
		ASSERT( pHdr != NULL );
		ASSERT( pHdr->m_pMalloc == this );
		ASSERT( pHdr->m_nItemSize == nItemSize );
		ASSERT( pHdr->m_pPrev == NULL );
		ASSERT( pHdr->m_pNext == NULL );
		ASSERT( _HighLevel_N_BlockIsFullyFree(pHdr) );
		ASSERT( (pos != NULL && pos2 != NULL) || (pos == NULL && pos2 == NULL) );
		_HighLevel_N_BlockFree( pHdr );
		VERIFY( m_mapHighLevelChunks.RemoveKey(nItemSize) );
	} // for( pos = _listSizesToRemove.GetHeadPosition(); pos != NULL; )
	_listSizesToRemove.RemoveAll();
	_listPointersToRemove.RemoveAll();
	return;
}

void STDMETHODCALLTYPE CExtAlloc::HeapMinimize(void)
{
	ASSERT( this != NULL && _LowLevel_IsValidPointer(this) );
	ASSERT( m_nRefCount > 0L || m_bDebugInDesructor );
	if( m_nCountInChunk <= 1L )
	{
		_HighLevel_1HeapMinimize();
		return;
	} // if( m_nCountInChunk <= 1L )
	_HighLevel_NHeapMinimize();
	return;
}

/////////////////////////////////////////////////////////////////////////////
// CExtArchiveIStream

CExtArchiveIStream::CExtArchiveIStream(
	CArchive * pAr, // = NULL
	bool bAutoDestoyArchive // = false
	)
	: m_nRefCount( 1L )
	, m_pAr( pAr )
	, m_bAutoDestoyArchive( bAutoDestoyArchive )
{
}

CExtArchiveIStream::CExtArchiveIStream(
	CArchive & ar,
	bool bAutoDestoyArchive // = false
	)
	: m_nRefCount( 1L )
	, m_pAr( &ar )
	, m_bAutoDestoyArchive( bAutoDestoyArchive )
{
}

CExtArchiveIStream::~CExtArchiveIStream()
{
	if( m_bAutoDestoyArchive && m_pAr != NULL )
	{
		if( m_pAr->IsStoring() )
			m_pAr->Flush();
		m_pAr->Close();
		delete m_pAr;
	} // if( m_bAutoDestoyArchive && m_pAr != NULL )
}

CArchive * CExtArchiveIStream::ArchiveAttach(
	CArchive * pAr, // = NULL
	bool bAutoDestoyArchive // = false
	)
{
CArchive * pArOld = m_pAr;
	m_pAr = pAr;
	m_bAutoDestoyArchive = bAutoDestoyArchive;
	return pArOld;
}

CArchive * CExtArchiveIStream::ArchiveAttach(
	CArchive & ar,
	bool bAutoDestoyArchive // = false
	)
{
	return ArchiveAttach( &ar, bAutoDestoyArchive );
}

CArchive * CExtArchiveIStream::ArchiveDetach()
{
	return ArchiveAttach();
}

HRESULT STDMETHODCALLTYPE CExtArchiveIStream::QueryInterface( 
    REFIID riid,
    void ** ppvObject
	)
{
	if( ppvObject == NULL )
	{
		ASSERT( FALSE );
		return E_NOINTERFACE;
	} // if( ppvObject == NULL )
	if(		riid == IID_IUnknown
		||	riid == IID_ISequentialStream
		||	riid == IID_IStream
		)
	{
		(*ppvObject) = this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CExtArchiveIStream::AddRef(void)
{
	ASSERT( m_nRefCount > 0L );
	m_nRefCount ++;
	return m_nRefCount;
}

ULONG STDMETHODCALLTYPE CExtArchiveIStream::Release(void)
{
	ASSERT( m_nRefCount > 0L );
	m_nRefCount --;
	if( m_nRefCount == 0L )
	{
		delete this;
		return 0L;
	}
	return m_nRefCount;
}

HRESULT STDMETHODCALLTYPE CExtArchiveIStream::Read(
    void * pv,
    ULONG cb,
    ULONG * pcbRead
	)
{
	if( m_pAr == NULL )
	{
		ASSERT( FALSE );
		return S_FALSE;
	} // if( m_pAr == NULL )
	if( ! m_pAr->IsLoading() )
	{
		ASSERT( FALSE );
		return S_FALSE;
	} // if( ! m_pAr->IsLoading() )
	if( pcbRead != NULL )
		*pcbRead = 0L;
	if( cb == 0L )
		return S_OK;
	if( pv == NULL )
	{
		ASSERT( FALSE );
		return E_INVALIDARG;
	} // if( pv == NULL )
	try
	{
		ULONG nBytes = m_pAr->Read( pv, cb );
		if( pcbRead != NULL )
			*pcbRead = nBytes;
	} // try
	catch( CException * pException )
	{
		ASSERT( FALSE );
		pException->Delete();
		return STG_E_ACCESSDENIED;
	} // catch( CException * pException )
	catch( ... )
	{
		ASSERT( FALSE );
		return STG_E_ACCESSDENIED;
	} // catch( ... )
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CExtArchiveIStream::Write( 
    const void * pv,
    ULONG cb,
    ULONG * pcbWritten
	)
{
	if( m_pAr == NULL )
	{
		ASSERT( FALSE );
		return S_FALSE;
	} // if( m_pAr == NULL )
	if( ! m_pAr->IsStoring() )
	{
		ASSERT( FALSE );
		return S_FALSE;
	} // if( ! m_pAr->IsStoring() )
	if( pcbWritten != NULL )
		*pcbWritten = 0L;
	if( cb == 0L )
		return S_OK;
	if( pv == NULL )
	{
		ASSERT( FALSE );
		return E_INVALIDARG;
	} // if( pv == NULL )
	try
	{
		m_pAr->Write( pv, cb );
		if( pcbWritten != NULL )
			*pcbWritten = cb;
	} // try
	catch( CException * pException )
	{
		ASSERT( FALSE );
		pException->Delete();
		return STG_E_ACCESSDENIED;
	} // catch( CException * pException )
	catch( ... )
	{
		ASSERT( FALSE );
		return STG_E_ACCESSDENIED;
	} // catch( ... )
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CExtArchiveIStream::Seek( 
    LARGE_INTEGER dlibMove,
    DWORD dwOrigin,
    ULARGE_INTEGER * plibNewPosition
	)
{
	if( m_pAr == NULL )
	{
		ASSERT( FALSE );
		return S_FALSE;
	} // if( m_pAr == NULL )
	CFile * pFile = m_pAr->GetFile();
	if( pFile == NULL )
	{
		ASSERT( FALSE );
		return S_FALSE;
	} // if( pFile == NULL )
	try
	{
// disable warning 4244
#pragma warning( push )
#pragma warning ( disable : 4244 )
		LONG lNewPos =
			pFile->Seek(
				dlibMove.QuadPart, //LONG( dlibMove.QuadPart )
				(dwOrigin == STREAM_SEEK_SET)
					?	CFile::begin
					:	(	(dwOrigin == STREAM_SEEK_END)
							?	CFile::end
							:	CFile::current
						)
				);
// rollback warning 4244
#pragma warning( pop )
		if( plibNewPosition != NULL )
			plibNewPosition->QuadPart =
				__int64(lNewPos);
	} // try
	catch( CException * pException )
	{
		ASSERT( FALSE );
		pException->Delete();
		return STG_E_ACCESSDENIED;
	} // catch( CException * pException )
	catch( ... )
	{
		ASSERT( FALSE );
		return STG_E_ACCESSDENIED;
	} // catch( ... )
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CExtArchiveIStream::SetSize( 
    ULARGE_INTEGER libNewSize
	)
{
	libNewSize;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CExtArchiveIStream::CopyTo( 
    IStream * pstm,
    ULARGE_INTEGER cb,
    ULARGE_INTEGER * pcbRead,
    ULARGE_INTEGER * pcbWritten
	)
{
    pstm;
    cb;
    pcbRead;
    pcbWritten;
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CExtArchiveIStream::Commit( 
    DWORD grfCommitFlags
	)
{
	grfCommitFlags;
	if( m_pAr == NULL )
	{
		ASSERT( FALSE );
		return STG_E_ACCESSDENIED;
	} // if( m_pAr == NULL )
	try
	{
		if( m_pAr->IsLoading() )
			return S_OK;
		CFile * pFile = m_pAr->GetFile();
		if( pFile == NULL )
			return S_OK;
		pFile->Flush();
	} // try
	catch( CException * pException )
	{
		ASSERT( FALSE );
		pException->Delete();
		return STG_E_ACCESSDENIED;
	} // catch( CException * pException )
	catch( ... )
	{
		ASSERT( FALSE );
		return STG_E_ACCESSDENIED;
	} // catch( ... )
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CExtArchiveIStream::Revert(void)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CExtArchiveIStream::LockRegion( 
    ULARGE_INTEGER libOffset,
    ULARGE_INTEGER cb,
    DWORD dwLockType
	)
{
    dwLockType;
	if( m_pAr == NULL )
	{
		ASSERT( FALSE );
		return STG_E_INVALIDFUNCTION;
	} // if( m_pAr == NULL )
	try
	{
		CFile * pFile = m_pAr->GetFile();
		if( pFile == NULL )
			return STG_E_INVALIDFUNCTION;
		pFile->LockRange(
			ULONG(libOffset.QuadPart),
			ULONG(cb.QuadPart)
			);
	} // try
	catch( CException * pException )
	{
		ASSERT( FALSE );
		pException->Delete();
		return STG_E_INVALIDFUNCTION;
	} // catch( CException * pException )
	catch( ... )
	{
		ASSERT( FALSE );
		return STG_E_INVALIDFUNCTION;
	} // catch( ... )
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CExtArchiveIStream::UnlockRegion( 
    ULARGE_INTEGER libOffset,
    ULARGE_INTEGER cb,
    DWORD dwLockType
	)
{
    dwLockType;
	if( m_pAr == NULL )
	{
		ASSERT( FALSE );
		return STG_E_INVALIDFUNCTION;
	} // if( m_pAr == NULL )
	try
	{
		CFile * pFile = m_pAr->GetFile();
		if( pFile == NULL )
			return STG_E_INVALIDFUNCTION;
		pFile->UnlockRange(
			ULONG(libOffset.QuadPart),
			ULONG(cb.QuadPart)
			);
	} // try
	catch( CException * pException )
	{
		ASSERT( FALSE );
		pException->Delete();
		return STG_E_INVALIDFUNCTION;
	} // catch( CException * pException )
	catch( ... )
	{
		ASSERT( FALSE );
		return STG_E_INVALIDFUNCTION;
	} // catch( ... )
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CExtArchiveIStream::Stat( 
    STATSTG * pstatstg,
    DWORD grfStatFlag
	)
{
    grfStatFlag;
    if( pstatstg == NULL )
	{
		ASSERT( FALSE );
		return E_INVALIDARG;
	} // if( pstatstg == NULL )
	if( m_pAr == NULL )
	{
		ASSERT( FALSE );
		return STG_E_ACCESSDENIED;
	} // if( m_pAr == NULL )
	::memset( pstatstg, 0, sizeof(STATSTG) );
	pstatstg->type = STGTY_STREAM;
	pstatstg->grfMode = STATFLAG_NONAME;
	//pstatstg->grfLocksSupported = LOCK_EXCLUSIVE;
	//pstatstg->clsid = CLSID_NULL;
	try
	{
		CFile * pFile = m_pAr->GetFile();
		if( pFile == NULL )
			return S_OK;
		CFileStatus _status;
		if( ! pFile->GetStatus(_status) )
			return S_OK;
		SYSTEMTIME _system_time;
		FILETIME _file_time;
		if( _status.m_mtime.GetAsSystemTime( _system_time ) )
			if( ::SystemTimeToFileTime( &_system_time, &_file_time ) )
				::memcpy( &pstatstg->mtime, &_file_time, sizeof(FILETIME) );
		if( _status.m_ctime.GetAsSystemTime( _system_time ) )
			if( ::SystemTimeToFileTime( &_system_time, &_file_time ) )
				::memcpy( &pstatstg->ctime, &_file_time, sizeof(FILETIME) );
		if( _status.m_atime.GetAsSystemTime( _system_time ) )
			if( ::SystemTimeToFileTime( &_system_time, &_file_time ) )
				::memcpy( &pstatstg->atime, &_file_time, sizeof(FILETIME) );
	} // try
	catch( CException * pException )
	{
		ASSERT( FALSE );
		pException->Delete();
		return STG_E_ACCESSDENIED;
	} // catch( CException * pException )
	catch( ... )
	{
		ASSERT( FALSE );
		return STG_E_ACCESSDENIED;
	} // catch( ... )
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CExtArchiveIStream::Clone( 
    IStream * * ppstm
	)
{
	ppstm;
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CExtGridCell

IMPLEMENT_DYNCREATE( CExtGridCell, CObject );

CExtGridCell::CExtGridCell(
	CExtGridDataProvider * pDataProvider // = NULL
	)
{
	_InitM();
	m_pDataProvider = pDataProvider;
#ifdef _DEBUG
	if( m_pDataProvider != NULL )
	{
		ASSERT_VALID( m_pDataProvider );
	} // if( m_pDataProvider != NULL )
#endif // _DEBUG
}

CExtGridCell::CExtGridCell(
	const CExtGridCell & other
	)
{
	_InitM();
	(*this) = other;
}

CExtGridCell::~CExtGridCell()
{
}

#ifdef _DEBUG
void CExtGridCell::AssertValid() const
{
	CObject::AssertValid();
	if( m_pDataProvider != NULL )
	{
		ASSERT_VALID( m_pDataProvider );
	} // if( m_pDataProvider != NULL )
}
void CExtGridCell::Dump( CDumpContext & dc ) const
{
	CObject::Dump( dc );
}
#endif // _DEBUG

LPARAM CExtGridCell::LParamGet() const
{
	ASSERT_VALID( this );
	return m_lParam;
}

LPARAM CExtGridCell::LParamSet(
	LPARAM lParamNew // = 0L
	)
{
	ASSERT_VALID( this );
LPARAM lParamOld = m_lParam;
	m_lParam = lParamNew;
	return lParamOld;
}

DWORD CExtGridCell::GetStyle() const
{
	ASSERT_VALID( this );
	return m_dwStyle;
}

DWORD CExtGridCell::ModifyStyle(
	DWORD dwStyleAdd,
	DWORD dwStyleRemove // = 0L
	)
{
	ASSERT_VALID( this );
DWORD dwStyleOld = GetStyle();
	if( dwStyleAdd == 0 && dwStyleRemove == 0 )
		return dwStyleOld;
	m_dwStyle &= ~dwStyleRemove;
	m_dwStyle |= dwStyleAdd;
	return dwStyleOld;
}

DWORD CExtGridCell::GetStyleEx() const
{
	ASSERT_VALID( this );
	return m_dwStyleEx;
}

DWORD CExtGridCell::ModifyStyleEx(
	DWORD dwStyleExAdd,
	DWORD dwStyleExRemove // = 0L
	)
{
	ASSERT_VALID( this );
DWORD dwStyleExOld = GetStyleEx();
	if( dwStyleExAdd == 0 && dwStyleExRemove == 0 )
		return dwStyleExOld;
	m_dwStyleEx &= ~dwStyleExRemove;
	m_dwStyleEx |= dwStyleExAdd;
	return dwStyleExOld;
}

CExtGridDataProvider * CExtGridCell::DataProviderGet()
{
	ASSERT_VALID( this );
	return m_pDataProvider;
}

const CExtGridDataProvider * CExtGridCell::DataProviderGet() const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridCell * > ( this ) )
			-> DataProviderGet();
}

CExtGridDataProvider * CExtGridCell::DataProviderSet(
	CExtGridDataProvider * pDataProvider
	)
{
	ASSERT_VALID( this );
CExtGridDataProvider * pOldDS = DataProviderGet();
	m_pDataProvider = pDataProvider;
#ifdef _DEBUG
	if( m_pDataProvider != NULL )
	{
		ASSERT_VALID( m_pDataProvider );
	} // if( m_pDataProvider != NULL )
#endif // _DEBUG
	return pOldDS;
}

void CExtGridCell::Serialize( CArchive & ar )
{
	CObject::Serialize( ar );
	if( ar.IsStoring() )
	{
		ar.Write( &m_dwStyle, sizeof(m_dwStyle) );
		ar.Write( &m_dwStyleEx, sizeof(m_dwStyleEx) );
		ar.Write( &m_nIconIndex, sizeof(m_nIconIndex) );
		ar.Write( &m_lParam, sizeof(m_lParam) );
	} // if( ar.IsStoring() )
	else
	{
		ar.Read( &m_dwStyle, sizeof(m_dwStyle) );
		ar.Read( &m_dwStyleEx, sizeof(m_dwStyleEx) );
		ar.Read( &m_nIconIndex, sizeof(m_nIconIndex) );
		ar.Read( &m_lParam, sizeof(m_lParam) );
	} // else from if( ar.IsStoring() )
}

bool CExtGridCell::IsEmpty() const
{
	ASSERT_VALID( this );
	return true;
}

void CExtGridCell::Empty()
{
	ASSERT_VALID( this );
	_InitM();
}

void CExtGridCell::_AssignM( const CExtGridCell & other )
{
	ASSERT_VALID( this );
	m_dwStyle = other.m_dwStyle;
	m_dwStyleEx = other.m_dwStyleEx;
	m_nIconIndex = other.m_nIconIndex;
	m_lParam = other.m_lParam;
	DataProviderSet(
		const_cast < CExtGridDataProvider * >
			( other.DataProviderGet() )
		);
	ASSERT_VALID( this );
}

void CExtGridCell::_InitM()
{
	m_dwStyle = __EGCS_DEFAULT_STYLES;
	m_dwStyleEx = __EGCS_EX_DEFAULT_STYLES;
	m_nIconIndex = -1;
	m_lParam = 0L;
	m_pDataProvider = NULL;
	ASSERT_VALID( this );
}

void CExtGridCell::Assign( const CExtGridCell & other )
{
	ASSERT_VALID( this );
	_AssignM( other );
}

CExtGridCell & CExtGridCell::operator = (  const CExtGridCell & other )
{
	ASSERT_VALID( this );
	Assign( other );
	return (*this);
}

int CExtGridCell::Compare(
	const CExtGridCell & other,
	DWORD dwStyleMask, // = __EGCS_COMPARE_MASK
	DWORD dwStyleExMask // = __EGCS_EX_COMPARE_MASK
	) const
{
	ASSERT_VALID( this );
LPCTSTR sTextLeft = GetTextBuffer();
	if( sTextLeft != NULL )
	{
		LPCTSTR sTextRight = other.GetTextBuffer();
		if( sTextRight != NULL )
		{
			int nRetVal = _tcscmp( sTextLeft, sTextRight );
			if( nRetVal != 0 )
				return nRetVal;
			return CompareStyleOnly( other, dwStyleMask, dwStyleExMask );
		} // if( sTextRight != NULL )
	} // if( sTextLeft != NULL )
VARIANT varLeft, varRight;
	::VariantInit( &varLeft );
	::VariantInit( &varRight );
	GetVariant( varLeft );
	other.GetVariant( varRight );
int nRetVal = stat_VariantCompare( &varLeft, &varRight );
	::VariantClear( &varLeft );
	::VariantClear( &varRight );
	if( nRetVal != 0 )
		return nRetVal;
	return CompareStyleOnly( other, dwStyleMask, dwStyleExMask );
}

int CExtGridCell::CompareStyleOnly(
	const CExtGridCell & other,
	DWORD dwStyleMask,
	DWORD dwStyleExMask
	) const
{
	if( dwStyleMask == 0 )
		return 0;
DWORD dwFlagsLeft = GetStyle() & dwStyleMask;
DWORD dwFlagsRight = other.GetStyle() & dwStyleMask;
	if( dwFlagsLeft < dwFlagsRight )
		return -1;
	if( dwFlagsLeft > dwFlagsRight )
		return 1;
	dwFlagsLeft = GetStyleEx() & dwStyleExMask;
	dwFlagsRight = other.GetStyleEx() & dwStyleExMask;
	if( dwFlagsLeft < dwFlagsRight )
		return -1;
	if( dwFlagsLeft > dwFlagsRight )
		return 1;
	return 0;
}

void CExtGridCell::TextGet( CExtSafeString & strCopy ) const
{
	ASSERT_VALID( this );
	strCopy.Empty();
}

void CExtGridCell::TextSet(
	__EXT_MFC_SAFE_LPCTSTR str, // = __EXT_MFC_SAFE_LPCTSTR(NULL) // empty text
	bool bAllowChangeDataType // = false
	)
{
	ASSERT_VALID( this );
	str;
	bAllowChangeDataType;
}

void CExtGridCell::GetVariant( VARIANT & varCopy ) const
{
	ASSERT_VALID( this );
	::VariantClear( &varCopy );
CExtSafeString strCopy;
	TextGet( strCopy );
	if( strCopy.IsEmpty() )
		return;
	try
	{
		BSTR bstrCopy = strCopy.AllocSysString();
		if( bstrCopy == NULL )
			return;
		varCopy.vt = VT_BSTR;
		varCopy.bstrVal = bstrCopy;
	} // try
	catch( CException * pException )
	{
		ASSERT( FALSE );
		pException->Delete();
		return;
	} // catch( CException * pException )
	catch( ... )
	{
		ASSERT( FALSE );
		return;
	} // catch( ... )
}

bool CExtGridCell::operator == ( const CExtGridCell & other ) const
{
	ASSERT_VALID( this );
	return (Compare(other) == 0) ? true : false;
}

bool CExtGridCell::operator != ( const CExtGridCell & other ) const
{
	ASSERT_VALID( this );
	return (Compare(other) != 0) ? true : false;
}

bool CExtGridCell::operator < ( const CExtGridCell & other ) const
{
	ASSERT_VALID( this );
	return (Compare(other) < 0) ? true : false;
}

bool CExtGridCell::operator > ( const CExtGridCell & other ) const
{
	ASSERT_VALID( this );
	return (Compare(other) > 0) ? true : false;
}

bool CExtGridCell::operator <= ( const CExtGridCell & other ) const
{
	ASSERT_VALID( this );
	return (Compare(other) <= 0) ? true : false;
}

bool CExtGridCell::operator >= ( const CExtGridCell & other ) const
{
	ASSERT_VALID( this );
	return (Compare(other) >= 0) ? true : false;
}

HRESULT CExtGridCell::stat_VariantSerializeData(
	LPVARIANT pVariant,
	IStream * pStream,
	bool bStore,
	VARTYPE * pVarTypeSerialized
	)
{
	if( pVariant == NULL || pStream == NULL )
	{
		ASSERT( FALSE );
		return E_INVALIDARG;
	} // if( pVariant == NULL || pStream == NULL )
	if( bStore )
	{
		HRESULT hr = S_OK;
		if( pVarTypeSerialized == NULL )
		{
			hr =
				pStream->Write(
					(LPVOID)&pVariant->vt,
					sizeof(VARTYPE),
					NULL
					);
			if( FAILED( hr ) )
			{
				ASSERT( FALSE );
				return hr;
			} // if( FAILED( hr ) )
		} // if( pVarTypeSerialized == NULL )
#ifdef _DEBUG
		else
		{
			ASSERT( pVariant->vt == (*pVarTypeSerialized) );
		} // else from if( pVarTypeSerialized == NULL )
#endif // _DEBUG
		INT nBytesToWrite = 0;
		switch( pVariant->vt )
		{
		case VT_UNKNOWN:
		case VT_DISPATCH:
		{
			IPersistStream * pPersistStream = NULL;
			if( pVariant->punkVal != NULL )
			{
				hr =
					pVariant -> punkVal ->
						QueryInterface(
						IID_IPersistStream,
						(LPVOID*)&pPersistStream
						);
				if( FAILED( hr ) )
				{
					hr =
						pVariant -> punkVal ->
							QueryInterface(
								IID_IPersistStreamInit,
								(LPVOID*)&pPersistStream
								);
					if( FAILED( hr ) )
						return hr;
				} // if( FAILED( hr ) )
			} // if( pVariant->punkVal != NULL )
			if( pPersistStream != NULL )
			{
				hr =
					::OleSaveToStream(
						pPersistStream,
						pStream
						);
				ASSERT( SUCCEEDED( hr ) );
				pPersistStream->Release();
			}
			else
			{
				hr =
					::WriteClassStm(
						pStream,
						CLSID_NULL
						);
				ASSERT( SUCCEEDED( hr ) );
			}
			return hr;
		} // cases VT_UNKNOWN, VT_DISPATCH:
		case VT_I1:
		case VT_UI1:
			nBytesToWrite = sizeof( BYTE );
			break;
		case VT_BOOL:
		case VT_I2:
		case VT_UI2:
			nBytesToWrite = sizeof( SHORT );
			break;
		case VT_ERROR:
		case VT_INT:
		case VT_I4:
		case VT_UINT:
		case VT_UI4:
		case VT_R4:
			nBytesToWrite = sizeof( LONG );
			break;
		case VT_I8:
		case VT_UI8:
			nBytesToWrite = sizeof( __int64 );
			break;
		case VT_DATE:
		case VT_R8:
		case VT_CY:
			nBytesToWrite = sizeof( DOUBLE );
			break;
		} // switch( pVariant->vt )
		if( nBytesToWrite != 0 )
		{
			hr =
				pStream->Write(
					(LPVOID)&pVariant->bVal,
					nBytesToWrite,
					NULL
					);
			ASSERT( SUCCEEDED( hr ) );
			return hr;
		} // if( nBytesToWrite != 0 )
		BSTR bstrEffective = NULL;
		VARIANT variantHelper;
		::VariantInit( &variantHelper );
		if( pVariant->vt == VT_BSTR )
			bstrEffective = pVariant->bstrVal;
		else
		{
			hr =
				::VariantChangeType(
					&variantHelper,
					pVariant,
					VARIANT_NOVALUEPROP,
					VT_BSTR
					);
			if( FAILED( hr ) )
			{
				ASSERT( FALSE );
				::VariantClear( &variantHelper );
				return hr;
			} // if( FAILED( hr ) )
			bstrEffective = variantHelper.bstrVal;
		} // if( pVariant->vt == VT_BSTR )
		ULONG nEffectiveLen =
			ULONG(
				(bstrEffective != NULL )
					? (::SysStringByteLen(bstrEffective) + sizeof(OLECHAR))
					: 0
					);
		ULONG nWritten = 0;
		hr =
			pStream->Write(
				(LPVOID)&nEffectiveLen,
				sizeof(nEffectiveLen),
				&nWritten
				);
		if( SUCCEEDED( hr ) )
		{
			if( nEffectiveLen != 0 )
			{
				hr =
					pStream->Write(
						(LPVOID)bstrEffective,
						nEffectiveLen,
						&nWritten
						);
				ASSERT( SUCCEEDED( hr ) );
			} // if( nEffectiveLen != 0 )
		} // if( SUCCEEDED( hr ) )
#ifdef _DEBUG
		else
		{
			ASSERT( FALSE );
		} // else from if( SUCCEEDED( hr ) )
#endif // _DEBUG
		
		::VariantClear( &variantHelper );
		return hr;
	} // if( bStore )
	else
	{
		HRESULT hr = ::VariantClear( pVariant );
		if( FAILED( hr ) )
		{
			ASSERT( FALSE );
			return hr;
		} // if( FAILED( hr ) )
		ULONG nBytesToRead = 0;
		VARTYPE vtLoaded = VT_EMPTY;
		if( pVarTypeSerialized == NULL )
		{
			hr =
				pStream->Read(
					(LPVOID)&vtLoaded,
					sizeof(VARTYPE),
					&nBytesToRead
					);
			if( FAILED( hr ) )
			{
				ASSERT( FALSE );
				return hr;
			} // if( FAILED( hr ) )
			if( hr == S_FALSE )
			{
				ASSERT( FALSE );
				return E_FAIL;
			} // if( hr == S_FALSE )
			if( nBytesToRead != sizeof(VARTYPE) )
			{
				ASSERT( FALSE );
				return E_FAIL;
			} // if( nBytesToRead != sizeof(VARTYPE) )
		} // if( pVarTypeSerialized == NULL )
		else
		{
			vtLoaded = *pVarTypeSerialized;
		} // else from if( pVarTypeSerialized == NULL )
		pVariant->vt = vtLoaded;
		nBytesToRead = 0;
		switch( vtLoaded )
		{
		case VT_UNKNOWN:
		case VT_DISPATCH:
			{
				pVariant->punkVal = NULL;
				hr =
					::OleLoadFromStream(
						pStream,
						( vtLoaded == VT_UNKNOWN )
							? IID_IUnknown
							: IID_IDispatch,
						(LPVOID*)&pVariant->punkVal
						);
				// If IPictureDisp or IFontDisp property is not set, 
				// ::OleLoadFromStream() will return REGDB_E_CLASSNOTREG.
				if( hr == REGDB_E_CLASSNOTREG )
					hr = S_OK;
				return hr;
			}
		case VT_I1:
		case VT_UI1:
			nBytesToRead = sizeof( BYTE );
			break;
		case VT_BOOL:
		case VT_I2:
		case VT_UI2:
			nBytesToRead = sizeof( SHORT );
			break;
		case VT_ERROR:
		case VT_INT:
		case VT_I4:
		case VT_UINT:
		case VT_UI4:
		case VT_R4:
			nBytesToRead = sizeof( LONG );
			break;
		case VT_I8:
		case VT_UI8:
			nBytesToRead = sizeof( __int64 );
			break;
		case VT_DATE:
		case VT_R8:
		case VT_CY:
			nBytesToRead = sizeof( DOUBLE );
			break;
		} // switch( vtLoaded )
		if( nBytesToRead != 0 )
		{
			hr =
				pStream->Read(
					(LPVOID)&pVariant->bVal,
					nBytesToRead,
					NULL
					);
			if( hr == S_FALSE )
			{
				ASSERT( FALSE );
				return E_FAIL;
			} // if( hr == S_FALSE )
			ASSERT( SUCCEEDED( hr ) );
			return hr;
		} // if( nBytesToRead != 0 )
		ULONG nStrLenLoaded = 0;
		hr =
			pStream->Read(
				(LPVOID)&nStrLenLoaded,
				sizeof(nStrLenLoaded),
				NULL
				);
		if( FAILED( hr ) )
		{
			ASSERT( FALSE );
			return hr;
		} // if( FAILED( hr ) )
		if( hr == S_FALSE || nStrLenLoaded == 0 )
		{
			ASSERT( FALSE );
			return E_FAIL;
		} // if( hr == S_FALSE || nStrLenLoaded == 0 )
		BSTR bstrLoaded =
			::SysAllocStringByteLen(
				NULL,
				nStrLenLoaded - sizeof(OLECHAR)
				);
		if( bstrLoaded == NULL )
		{
			ASSERT( FALSE );
			return  E_OUTOFMEMORY;
		} // if( bstrLoaded == NULL )
		hr =
			pStream->Read(
				(LPVOID)bstrLoaded,
				nStrLenLoaded,
				NULL
				);
		if( FAILED( hr ) )
		{
			ASSERT( FALSE );
			::SysFreeString( bstrLoaded );
			return hr;
		} // if( FAILED( hr ) )
		if( hr == S_FALSE )
		{
			ASSERT( FALSE );
			::SysFreeString( bstrLoaded );
			return E_FAIL;
		} // if( hr == S_FALSE )
		pVariant->vt = VT_BSTR;
		pVariant->bstrVal = bstrLoaded;
		if( vtLoaded != VT_BSTR )
		{
			hr =
				::VariantChangeType(
					pVariant,
					pVariant,
					0,
					vtLoaded
					);
			if( FAILED( hr ) )
			{
				ASSERT( FALSE );
				::VariantClear( pVariant );
			} // if( FAILED( hr ) )
		} // if( vtLoaded != VT_BSTR )
		return hr;
	} // else from if( bStore )
}

HRESULT CExtGridCell::stat_VariantSerialize(
	LPVARIANT pVariant,
	IStream * pStream,
	bool bStore
	)
{
	return
		stat_VariantSerializeData(
			pVariant,
			pStream,
			bStore,
			(VARTYPE *)NULL
			);
}

HRESULT CExtGridCell::stat_VariantSerialize(
	LPVARIANT pVariant,
	CArchive & ar
	)
{
	if( pVariant == NULL )
	{
		ASSERT( FALSE );
		return E_INVALIDARG;
	} // if( pVariant == NULL )
CFile * pFile = ar.GetFile();
	if( pFile != NULL )
	{
		COleStreamFile * pOleFile =
			DYNAMIC_DOWNCAST( COleStreamFile, pFile );
		if( pOleFile != NULL )
		{
			IStream * pStream = pOleFile->GetStream();
			if( pStream == NULL )
			{
				ASSERT( FALSE );
				return E_FAIL;
			} // if( pStream == NULL )
			HRESULT hr =
				stat_VariantSerialize(
					pVariant,
					pStream,
					ar.IsStoring() ? true : false
					);
			ASSERT( SUCCEEDED( hr ) );
			return hr;
		} // if( pOleFile != NULL )
	} // if( pFile != NULL )
CExtArchiveIStream stm( ar, false );
HRESULT hr =
		stat_VariantSerialize(
			pVariant,
			stm,
			ar.IsStoring() ? true : false
			);
	ASSERT( SUCCEEDED( hr ) );
	return hr;
}

int CExtGridCell::stat_VariantCompare(
	LPVARIANT pVariantLeft,
	LPVARIANT pVariantRight,
	LCID lcid, // = LOCALE_USER_DEFAULT
	ULONG dwFlags // = 0L
	)
{
HRESULT hr =
	stat_VariantCompareHR(
		pVariantLeft,
		pVariantRight,
		lcid,
		dwFlags
		);
	ASSERT(
			hr == VARCMP_EQ
		||	hr == VARCMP_LT
		||	hr == VARCMP_GT
		);
	if( hr == VARCMP_LT )
		return -1;
	if( hr == VARCMP_GT )
		return 1;
	return 0;
}

typedef
	HRESULT ( __stdcall * p_VarCmp_t )(
		LPVARIANT pVariantLeft,
		LPVARIANT pVariantRight,
		LCID lcid,
		ULONG dwFlags
		);
typedef
	HRESULT ( __stdcall * p_SafeArrayGetVartype_t )(
		SAFEARRAY *  psa, 
		VARTYPE *  pvt 
		);
static p_VarCmp_t g_pVarCmp = NULL;
static p_SafeArrayGetVartype_t g_pSafeArrayGetVartype = NULL;

HRESULT CExtGridCell::stat_VariantCompareHR(
	LPVARIANT pVariantLeft,
	LPVARIANT pVariantRight,
	LCID lcid, // = LOCALE_USER_DEFAULT
	ULONG dwFlags, // = 0L
	bool bReturnOnlyValidResult // = true // return only VARCMP_EQ or VARCMP_GT or VARCMP_LT values
	)
{
	ASSERT( pVariantLeft != NULL );
	ASSERT( pVariantRight != NULL );
VARTYPE vtCmpLeft = pVariantLeft->vt;
	if(		vtCmpLeft == (VT_VARIANT|VT_BYREF)
		&&	pVariantLeft->pvarVal != NULL
		)
		return
			stat_VariantCompareHR(
				pVariantLeft->pvarVal,
				pVariantRight,
				lcid,
				dwFlags,
				bReturnOnlyValidResult
				);
VARTYPE vtCmpRight = pVariantRight->vt;
	if(		vtCmpRight == (VT_VARIANT|VT_BYREF)
		&&	pVariantRight->pvarVal != NULL
		)
		return
			stat_VariantCompareHR(
				pVariantLeft,
				pVariantRight->pvarVal,
				lcid,
				dwFlags,
				bReturnOnlyValidResult
				);
	if( (vtCmpLeft & VT_ARRAY) == 0 && (vtCmpRight & VT_ARRAY) == 0 )
	{
		vtCmpLeft &= ~(VT_BYREF);
		vtCmpRight &= ~(VT_BYREF);
		if( vtCmpLeft == vtCmpRight )
		{
			switch( vtCmpLeft )
			{
			case VT_EMPTY:
			case VT_NULL:
				return VARCMP_EQ;
			case VT_BOOL:
			{
				ASSERT( VARIANT_FALSE == 0 );
				VARIANT_BOOL boolLeft =
					( (pVariantLeft->vt & (VT_BYREF)) != 0 )
						? (	(pVariantLeft->pboolVal == NULL)
							? VARIANT_FALSE
							: (*pVariantLeft->pboolVal)
							)
						: pVariantLeft->boolVal;
				VARIANT_BOOL boolRight =
					( (pVariantRight->vt & (VT_BYREF)) != 0 )
						? (	(pVariantRight->pboolVal == NULL)
							? VARIANT_FALSE
							: (*pVariantRight->pboolVal)
							)
						: pVariantRight->boolVal;
				if(		(	boolLeft == VARIANT_FALSE
						&&	boolRight == VARIANT_FALSE
						)
					||	(	boolLeft != VARIANT_FALSE
						&&	boolRight != VARIANT_FALSE
						)
					)
					return VARCMP_EQ;
				if( boolLeft != VARIANT_FALSE )
					return VARCMP_GT;
				ASSERT( boolRight != VARIANT_FALSE );
				return VARCMP_LT;
			} // case VT_BOOL
			case VT_I1:
			{
				CHAR charLeft =
					( (pVariantLeft->vt & (VT_BYREF)) != 0 )
						? (	(pVariantLeft->pcVal == NULL)
							? ((CHAR)0)
							: (*pVariantLeft->pcVal)
							)
						: pVariantLeft->cVal;
				CHAR charRight =
					( (pVariantRight->vt & (VT_BYREF)) != 0 )
						? (	(pVariantRight->pcVal == NULL)
							? ((CHAR)0)
							: (*pVariantRight->pcVal)
							)
						: pVariantRight->cVal;
				if(	charLeft == charRight )
					return VARCMP_EQ;
				if(	charLeft > charRight )
					return VARCMP_GT;
				ASSERT(	charLeft < charRight );
				return VARCMP_LT;
			} // case VT_I1
			case VT_UI1:
			{
				BYTE byteLeft =
					( (pVariantLeft->vt & (VT_BYREF)) != 0 )
						? (	(pVariantLeft->pbVal == NULL)
							? ((BYTE)0)
							: (*pVariantLeft->pbVal)
							)
						: pVariantLeft->bVal;
				BYTE byteRight =
					( (pVariantRight->vt & (VT_BYREF)) != 0 )
						? (	(pVariantRight->pbVal == NULL)
							? ((BYTE)0)
							: (*pVariantRight->pbVal)
							)
						: pVariantRight->bVal;
				if(	byteLeft == byteRight )
					return VARCMP_EQ;
				if(	byteLeft > byteRight )
					return VARCMP_GT;
				ASSERT(	byteLeft < byteRight );
				return VARCMP_LT;
			} // case VT_UI1
			case VT_I2:
			{
				SHORT shortLeft =
					( (pVariantLeft->vt & (VT_BYREF)) != 0 )
						? (	(pVariantLeft->piVal == NULL)
							? ((SHORT)0)
							: (*pVariantLeft->piVal)
							)
						: pVariantLeft->iVal;
				SHORT shortRight =
					( (pVariantRight->vt & (VT_BYREF)) != 0 )
						? (	(pVariantRight->piVal == NULL)
							? ((SHORT)0)
							: (*pVariantRight->piVal)
							)
						: pVariantRight->iVal;
				if(	shortLeft == shortRight )
					return VARCMP_EQ;
				if(	shortLeft > shortRight )
					return VARCMP_GT;
				ASSERT(	shortLeft < shortRight );
				return VARCMP_LT;
			} // case VT_I2
			case VT_UI2:
			{
				USHORT ushortLeft =
					( (pVariantLeft->vt & (VT_BYREF)) != 0 )
						? (	(pVariantLeft->puiVal == NULL)
							? ((USHORT)0)
							: (*pVariantLeft->puiVal)
							)
						: pVariantLeft->uiVal;
				USHORT ushortRight =
					( (pVariantRight->vt & (VT_BYREF)) != 0 )
						? (	(pVariantRight->puiVal == NULL)
							? ((USHORT)0)
							: (*pVariantRight->puiVal)
							)
						: pVariantRight->uiVal;
				if(	ushortLeft == ushortRight )
					return VARCMP_EQ;
				if(	ushortLeft > ushortRight )
					return VARCMP_GT;
				ASSERT(	ushortLeft < ushortRight );
				return VARCMP_LT;
			} // case VT_UI2
			case VT_I4:
			case VT_INT:
			case VT_ERROR:
			{
				ASSERT( sizeof(pVariantLeft->lVal) == sizeof(pVariantLeft->scode) );
				LONG longLeft =
					( (pVariantLeft->vt & (VT_BYREF)) != 0 )
						? (	(pVariantLeft->plVal == NULL)
							? ((LONG)0)
							: (*pVariantLeft->plVal)
							)
						: pVariantLeft->lVal;
				LONG longRight =
					( (pVariantRight->vt & (VT_BYREF)) != 0 )
						? (	(pVariantRight->plVal == NULL)
							? ((LONG)0)
							: (*pVariantRight->plVal)
							)
						: pVariantRight->lVal;
				if(	longLeft == longRight )
					return VARCMP_EQ;
				if(	longLeft > longRight )
					return VARCMP_GT;
				ASSERT(	longLeft < longRight );
				return VARCMP_LT;
			} // cases VT_I4, VT_INT, VT_ERROR
			case VT_UI4:
			case VT_UINT:
			{
				ULONG ulongLeft =
					( (pVariantLeft->vt & (VT_BYREF)) != 0 )
						? (	(pVariantLeft->pulVal == NULL)
							? ((ULONG)0)
							: (*pVariantLeft->pulVal)
							)
						: pVariantLeft->ulVal;
				ULONG ulongRight =
					( (pVariantRight->vt & (VT_BYREF)) != 0 )
						? (	(pVariantRight->pulVal == NULL)
							? ((ULONG)0)
							: (*pVariantRight->pulVal)
							)
						: pVariantRight->ulVal;
				if(	ulongLeft == ulongRight )
					return VARCMP_EQ;
				if(	ulongLeft > ulongRight )
					return VARCMP_GT;
				ASSERT(	ulongLeft < ulongRight );
				return VARCMP_LT;
			} // cases VT_UI4, VT_UINT
			case VT_R4:
			{
				FLOAT floatLeft =
					( (pVariantLeft->vt & (VT_BYREF)) != 0 )
						? (	(pVariantLeft->pfltVal == NULL)
							? ((FLOAT)0.0)
							: (*pVariantLeft->pfltVal)
							)
						: pVariantLeft->fltVal;
				FLOAT floatRight =
					( (pVariantRight->vt & (VT_BYREF)) != 0 )
						? (	(pVariantRight->pfltVal == NULL)
							? ((FLOAT)0.0)
							: (*pVariantRight->pfltVal)
							)
						: pVariantRight->fltVal;
				if(	floatLeft == floatRight )
					return VARCMP_EQ;
				if(	floatLeft > floatRight )
					return VARCMP_GT;
				ASSERT(	floatLeft < floatRight );
				return VARCMP_LT;
			} // case VT_R4
			case VT_R8:
			{
				DOUBLE doubleLeft =
					( (pVariantLeft->vt & (VT_BYREF)) != 0 )
						? (	(pVariantLeft->pdblVal == NULL)
							? ((DOUBLE)0.0)
							: (*pVariantLeft->pdblVal)
							)
						: pVariantLeft->dblVal;
				DOUBLE doubleRight =
					( (pVariantRight->vt & (VT_BYREF)) != 0 )
						? (	(pVariantRight->pdblVal == NULL)
							? ((DOUBLE)0.0)
							: (*pVariantRight->pdblVal)
							)
						: pVariantRight->dblVal;
				if(	doubleLeft == doubleRight )
					return VARCMP_EQ;
				if(	doubleLeft > doubleRight )
					return VARCMP_GT;
				ASSERT(	doubleLeft < doubleRight );
				return VARCMP_LT;
			} // case VT_R8
			case VT_BSTR:
			{
				BSTR bstrLeft =
					( (pVariantLeft->vt & (VT_BYREF)) != 0 )
						? (	(pVariantLeft->pbstrVal == NULL)
							? ((BSTR)NULL)
							: (*pVariantLeft->pbstrVal)
							)
						: pVariantLeft->bstrVal;
				BSTR bstrRight =
					( (pVariantRight->vt & (VT_BYREF)) != 0 )
						? (	(pVariantRight->pbstrVal == NULL)
							? ((BSTR)NULL)
							: (*pVariantRight->pbstrVal)
							)
						: pVariantRight->bstrVal;
				if( bstrLeft == NULL || bstrRight == NULL )
				{
					if( bstrLeft == NULL && bstrRight == NULL )
						return VARCMP_EQ;
					if( bstrRight == NULL )
						return VARCMP_GT;
					ASSERT( bstrLeft == NULL );
					return VARCMP_LT;
				} // if( bstrLeft == NULL || bstrRight == NULL )
				ASSERT( bstrLeft != NULL );
				ASSERT( bstrRight != NULL );
				UINT nLenLeft = ::SysStringByteLen( bstrLeft );
				UINT nLenRight = ::SysStringByteLen( bstrRight );
				if( nLenLeft == 0 || nLenRight == 0 )
				{
					if( nLenLeft == 0 && nLenRight == 0 )
						return VARCMP_EQ;
					if( nLenRight == 0 )
						return VARCMP_GT;
					ASSERT( nLenLeft == 0 );
					return VARCMP_LT;
				} // if( nLenLeft == 0 || nLenRight == 0 )
				ASSERT( nLenLeft != 0 );
				ASSERT( nLenRight != 0 );
				break; // use ::VarCmp()
			} // case VT_BSTR
			case VT_DISPATCH:
			case VT_UNKNOWN:
			{
				IUnknown * pLeftSrc = NULL, * pRightSrc = NULL;
				if( (pVariantLeft->vt & (VT_BYREF)) != 0 )
				{
					if( pVariantLeft->ppunkVal != NULL )
						pLeftSrc = (*(pVariantLeft->ppunkVal));
				} // if( (pVariantLeft->vt & (VT_BYREF)) != 0 )
				else
					pLeftSrc = pVariantLeft->punkVal;
				if( (pVariantRight->vt & (VT_BYREF)) != 0 )
				{
					if( pVariantRight->ppunkVal != NULL )
						pRightSrc = (*(pVariantRight->ppunkVal));
				} // if( (pVariantRight->vt & (VT_BYREF)) != 0 )
				else
					pRightSrc = pVariantRight->punkVal;
				if( pLeftSrc != NULL && pRightSrc != NULL )
				{
					IUnknown * pLeftDst = NULL, * pRightDst = NULL;
					if(		SUCCEEDED(
								pLeftSrc->QueryInterface(
									IID_IUnknown,
									(LPVOID*)&pLeftDst
									)
								)
						&&	SUCCEEDED(
								pRightSrc->QueryInterface(
									IID_IUnknown,
									(LPVOID*)&pRightDst
									)
								)
						)
					{ // if both IUnknown queries succeeded
						ASSERT( pLeftDst != NULL );
						ASSERT( pRightDst != NULL );
						if( pLeftDst == pRightDst )
						{
							pLeftDst->Release();
							pRightDst->Release();
							return VARCMP_EQ;
						} // if( pLeftDst == pRightDst )
					} // if both IUnknown queries succeeded
					if( pLeftDst != NULL )
						pLeftDst->Release();
					if( pRightDst != NULL )
						pRightDst->Release();
				} // if( pLeftSrc != NULL && pRightSrc != NULL )
				break; // use ::VarCmp()
			} // cases VT_DISPATCH, VT_UNKNOWN
			} // switch( vtCmpLeft )
		} // if( vtCmpLeft == vtCmpRight )
	} // if( (vtCmpLeft & VT_ARRAY) == 0 && (vtCmpRight & VT_ARRAY) == 0 )
	if( g_pVarCmp == NULL )
	{
		HINSTANCE hOleAut32 =
			::GetModuleHandle( _T("OLEAUT32.DLL") );
		ASSERT( hOleAut32 != NULL );
		if( hOleAut32 == NULL )
			return bReturnOnlyValidResult ? VARCMP_EQ : VARCMP_NULL;
		g_pVarCmp = (p_VarCmp_t)
			::GetProcAddress(
				hOleAut32,
				"VarCmp"
				);
		ASSERT( g_pVarCmp != NULL );
		if( g_pVarCmp == NULL )
			return bReturnOnlyValidResult ? VARCMP_EQ : VARCMP_NULL;
	} // if( g_pVarCmp == NULL )
HRESULT hr =
		g_pVarCmp(
			pVariantLeft,
			pVariantRight,
			lcid,
			dwFlags
			);
	if( bReturnOnlyValidResult )
	{
		// convert VARCMP_NULL and unknown results
		// into VARCMP_EQ result code
		if(		hr != VARCMP_EQ
			&&	hr != VARCMP_GT
			&&	hr != VARCMP_LT
			)
			hr = VARCMP_EQ;
	} // if( bReturnOnlyValidResult )
	return hr;
}

__EXT_MFC_SAFE_LPCTSTR CExtGridCell::GetTextBuffer() const
{
	ASSERT_VALID( this );
	return NULL; // no text buffer
}

CExtGridCell * CExtGridCell::Clone(
	IMalloc * pMalloc // = NULL
	) const
{
	ASSERT_VALID( this );
	if( pMalloc == NULL )
	{
		try
		{
			CRuntimeClass * pRTC = GetRuntimeClass();
			ASSERT( pRTC != NULL );
			CExtGridCell * pOther = (CExtGridCell *)
				pRTC->CreateObject();
			ASSERT_VALID( pOther );
			ASSERT_KINDOF( CExtGridCell, pOther );
			pOther->Assign( *this );
			ASSERT( pOther->DataProviderGet() == DataProviderGet() );
			return pOther;
		} // try
		catch( CException * pException )
		{
			ASSERT( FALSE );
			pException->Delete();
		} // catch( CException * pException )
		catch( ... )
		{
			ASSERT( FALSE );
		} // catch( ... )
		return NULL;
	} // if( pMalloc == NULL )
CExtGridCell * pOther =
		new (pMalloc, false) CExtGridCell
			(	(	const_cast
					< CExtGridCell * >
					( this )
				)
				-> DataProviderGet()
			);
	ASSERT( pOther != NULL );
	if( pOther != NULL )
	{
		ASSERT_VALID( pOther );
		pOther->Assign( *this );
		ASSERT( pOther->DataProviderGet() == DataProviderGet() );
	} // if( pOther != NULL )
	return pOther;
}

void * CExtGridCell::operator new (
	size_t nSize,
	IMalloc * pMalloc,
	bool bThrowException
	)
{
	ASSERT( pMalloc != NULL );
	ASSERT( nSize > 0 );
	if( pMalloc == NULL || (! (nSize > 0) ) )
	{
		ASSERT( FALSE );
		if( bThrowException )
			::AfxThrowMemoryException();
		return NULL;
	} // if( pMalloc == NULL || (! (nSize > 0) ) )
void * pMemory =
		pMalloc->Alloc( ULONG(nSize) );
	ASSERT( pMemory != NULL );
	if( pMemory == NULL && bThrowException )
		::AfxThrowMemoryException();
	return pMemory;
}

void CExtGridCell::operator delete (
	void * pMemory,
	IMalloc * pMalloc,
	bool bThrowException
	)
{
	ASSERT( pMalloc != NULL );
	ASSERT( pMemory != NULL );
	if( pMalloc == NULL || pMemory == NULL )
	{
		ASSERT( FALSE );
		if( bThrowException )
			::AfxThrowMemoryException();
		return;
	} // if( pMalloc == NULL || (! (nSize > 0) ) )
	if( pMalloc->DidAlloc(pMemory) == 0 )
	{
		ASSERT( FALSE );
		if( bThrowException )
			::AfxThrowMemoryException();
		return;
	} // if( pMalloc->DidAlloc(pMemory) == 0 )
	pMalloc->Free( pMemory );
}

#ifdef _DEBUG
void * CExtGridCell::operator new [] (
	size_t nSize
	)
{
	ASSERT( FALSE ); // must never be used!
	nSize;
	return NULL;
}
void CExtGridCell::operator delete [] (
	void * pMemory
	)
{
	ASSERT( FALSE ); // must never be used!
	pMemory;
}
void * CExtGridCell::operator new [] (
	size_t nSize,
	IMalloc * pMalloc,
	bool bThrowException
	)
{
	ASSERT( FALSE ); // must never be used!
	nSize;
	pMalloc;
	bThrowException;
	return NULL;
}
void CExtGridCell::operator delete [] (
	void * pMemory,
	IMalloc * pMalloc,
	bool bThrowException
	)
{
	ASSERT( FALSE ); // must never be used!
	pMemory;
	pMalloc;
	bThrowException;
}
#endif // _DEBUG

CExtGridCell * CExtGridCell::InstanceCreate(
	CRuntimeClass * pRTC,
	IMalloc * pMalloc,
	bool bThrowException // = false
	)
{
	ASSERT( pRTC != NULL );
	ASSERT( pMalloc != NULL );
CExtGridCell * pCellType = NULL;
	try
	{
		pCellType =
			(CExtGridCell *)
				pRTC->CreateObject();
		ASSERT_VALID( pCellType );
		ASSERT_KINDOF( CExtGridCell, pCellType );
		CExtGridCell * pCell = pCellType->Clone( pMalloc );
		ASSERT( pCell != NULL );
		if( pCellType != NULL )
		{
			try
			{
				delete pCellType;
			} // try
			catch( CException * pLocalException )
			{
				ASSERT( FALSE );
				pLocalException->Delete();
			} // catch( CException * pLocalException )
			catch( ... )
			{
				ASSERT( FALSE );
			} // catch( ... )
			pCellType = NULL;
		} // if( pCellType != NULL )
		if( pCell == NULL && bThrowException )
			::AfxThrowMemoryException();
		return pCell;
	} // try
	catch( CException * pException )
	{
		ASSERT( FALSE );
		if( pCellType != NULL )
		{
			try
			{
				delete pCellType;
			} // try
			catch( CException * pLocalException )
			{
				ASSERT( FALSE );
				pLocalException->Delete();
			} // catch( CException * pLocalException )
			catch( ... )
			{
				ASSERT( FALSE );
			} // catch( ... )
			pCellType = NULL;
		} // if( pCellType != NULL )
		if( bThrowException )
			throw;
		pException->Delete();
	} // catch( CException * pException )
	catch( ... )
	{
		ASSERT( FALSE );
		if( pCellType != NULL )
		{
			try
			{
				delete pCellType;
			} // try
			catch( CException * pLocalException )
			{
				ASSERT( FALSE );
				pLocalException->Delete();
			} // catch( CException * pLocalException )
			catch( ... )
			{
				ASSERT( FALSE );
			} // catch( ... )
			pCellType = NULL;
		} // if( pCellType != NULL )
		if( bThrowException )
			throw;
	} // catch( ... )
	return NULL;
}

CExtGridCell * CExtGridCell::InstanceCreate(
	const CExtGridCell & cell,
	bool bCopyData,
	IMalloc * pMalloc,
	bool bThrowException // = false
	)
{
	ASSERT_VALID( (&cell) );
	ASSERT_KINDOF( CExtGridCell, (&cell) );
	ASSERT( pMalloc != NULL );
CExtGridCell * pCell =
		InstanceCreate(
			cell.GetRuntimeClass(),
			pMalloc,
			bThrowException
			);
	if( pCell == NULL )
	{
		ASSERT( !bThrowException );
		return NULL;
	} // bCopyData
	if( bCopyData )
		pCell->Assign( cell );
	ASSERT( pCell->DataProviderGet() == cell.DataProviderGet() );
	return pCell;
}

void CExtGridCell::InstanceDestroy(
	CExtGridCell * pCell,
	IMalloc * pMalloc,
	bool bThrowException // = false
	)
{
	ASSERT_VALID( pCell );
	ASSERT( pMalloc != NULL );
	pCell->~CExtGridCell();
	CExtGridCell::operator delete( pCell, pMalloc, bThrowException );
}

CExtGridCell * CExtGridCell::InstanceLoad(
	CArchive & ar,
	IMalloc * pMalloc,
	bool bThrowException // = false
	)
{
	ASSERT( ar.IsLoading() );
	ASSERT( pMalloc != NULL );
CExtGridCell * pCell = NULL;
	try
	{
		CRuntimeClass * pRTC =
			ar.ReadClass( RUNTIME_CLASS(CExtGridCell) );
		ASSERT( pRTC != NULL );
		if( pRTC == NULL )
		{
			if( bThrowException )
				::AfxThrowArchiveException(
					CArchiveException::badClass
					);
			return NULL;
		} // if( pRTC == NULL )
		pCell =
			InstanceCreate(
				pRTC,
				pMalloc,
				bThrowException
				);
		ASSERT( !bThrowException );
		ASSERT_VALID( pCell );
		ASSERT_KINDOF( CExtGridCell, pCell );
		ASSERT( pCell->IsKindOf(pRTC) );
		pCell->Serialize( ar );
		return pCell;
	} // try
	catch( CException * pException )
	{
		ASSERT( FALSE );
		if( bThrowException )
		{
			if( pCell != NULL )
				InstanceDestroy( pCell, pMalloc, false );
			throw;
		} // if( bThrowException )
		pException->Delete();
	} // catch( CException * pException )
	catch( ... )
	{
		ASSERT( FALSE );
		if( bThrowException )
		{
			if( pCell != NULL )
				InstanceDestroy( pCell, pMalloc, false );
			throw;
		} // if( bThrowException )
	} // catch( ... )
	return NULL;
}

bool CExtGridCell::InstanceSave(
	CExtGridCell & cell,
	CArchive & ar,
	bool bThrowException // = false
	)
{
	ASSERT_VALID( (&cell) );
	ASSERT_KINDOF( CExtGridCell, (&cell) );
	ASSERT( ar.IsStoring() );
	try
	{
		CRuntimeClass * pRTC = cell.GetRuntimeClass();
		ASSERT( pRTC != NULL );
		ar.WriteClass( pRTC );
		cell.Serialize( ar );
		return true;
	} // try
	catch( CException * pException )
	{
		ASSERT( FALSE );
		if( bThrowException )
			throw;
		pException->Delete();
	} // catch( CException * pException )
	catch( ... )
	{
		ASSERT( FALSE );
		if( bThrowException )
			throw;
	} // catch( ... )
	return false;
}

bool CExtGridCell::OnPaintBackground(
	const CExtGridWnd & wndGrid,
	CDC & dc,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	const RECT & rcVisibleRange,
	DWORD dwAreaFlags,
	DWORD dwHelperPaintFlags
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( nVisibleColNo >= 0 );
	ASSERT( nVisibleRowNo >= 0 );
	ASSERT( nColNo >= 0 );
	ASSERT( nRowNo >= 0 );
	ASSERT( rcCell.right >= rcCell.left );
	ASSERT( rcCell.bottom >= rcCell.top );
	nVisibleColNo;
	nVisibleRowNo;
	nColNo;
	nRowNo;
	nColType;
	nRowType;
	rcVisibleRange;
	dwAreaFlags;
	rcCellExtra;
	if( (dwHelperPaintFlags&__EGCPF_OUTER_DND) != 0 )
	{
		COLORREF clrFace = wndGrid.OnSiwGetSysColor( COLOR_3DFACE );
		COLORREF clrLight = wndGrid.OnSiwGetSysColor( COLOR_3DHILIGHT );
		COLORREF clrDark = wndGrid.OnSiwGetSysColor( COLOR_3DSHADOW );
		dc.FillSolidRect( &rcCell, clrFace );
		dc.Draw3dRect( &rcCell, clrLight, clrDark );
		return true;
	} // if( (dwHelperPaintFlags&__EGCPF_OUTER_DND) != 0 )
//bool bHoverByColumn = ( (dwHelperPaintFlags&__EGCPF_HOVER_BY_COLUMN) != 0 ) ? true : false;
//bool bHoverByRow = ( (dwHelperPaintFlags&__EGCPF_HOVER_BY_ROW) != 0 ) ? true : false;
//bool bHighlightedBySelectedColumn = ( (dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_SELECTED_COLUMN) != 0 ) ? true : false;
//bool bHighlightedBySelectedRow = ( (dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_SELECTED_ROW) != 0 ) ? true : false;
//bool bHighlightedByFocusedColumn = ( (dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_FOCUSED_COLUMN) != 0 ) ? true : false;
//bool bHighlightedByFocusedRow = ( (dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_FOCUSED_ROW) != 0 ) ? true : false;
//bool bFocusedControl = ( (dwHelperPaintFlags&__EGCPF_FOCUSED_CONTROL) != 0 ) ? true : false;
//bool bHighlightPressing = ( (dwHelperPaintFlags&(__EGCPF_HIGHLIGHTED_BY_PRESSED_COLUMN|__EGCPF_HIGHLIGHTED_BY_PRESSED_ROW)) != 0 ) ? true : false;
	return false;
}

#define __EXT_FOCUS_ARROW_GLYPH_EXTENT_HORZ 12
#define __EXT_FOCUS_ARROW_GLYPH_EXTENT_VERT 12
#define __EXT_SORT_ARROW_GLYPH_EXTENT_HORZ 14
#define __EXT_SORT_ARROW_GLYPH_EXTENT_VERT 14
#define __EXT_EXPANDED_TEXT_GAP_X 2
#define __EXT_EXPANDED_TEXT_GAP_Y 2

void CExtGridCell::OnPaintForeground(
	const CExtGridWnd & wndGrid,
	CDC & dc,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	const RECT & rcVisibleRange,
	DWORD dwAreaFlags,
	DWORD dwHelperPaintFlags
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( nVisibleColNo >= 0 );
	ASSERT( nVisibleRowNo >= 0 );
	ASSERT( nColNo >= 0 );
	ASSERT( nRowNo >= 0 );
	ASSERT( rcCell.right >= rcCell.left );
	ASSERT( rcCell.bottom >= rcCell.top );
	if( ! dc.RectVisible(&rcCell) )
		return;
CRect	rcCellRest( rcCell ),
		rcCellText,
		rcFocusArrow,
		rcIcon,
		rcCheck,
		rcSortArrow,
		rcUpdownButtonTop,
		rcUpdownButtonBottom,
		rcDropdownButton,
		rcEllipsisButton;
CSize	sizeIcon = IconGetSize(),
		sizeFocusArrow =
			OnCalcFocusArrowSize(
				wndGrid,
				dc,
				nVisibleColNo,
				nVisibleRowNo,
				nColNo,
				nRowNo,
				nColType,
				nRowType,
				dwAreaFlags,
				dwHelperPaintFlags
				),
		sizeSortArrow =
			OnCalcSortArrowSize(
				wndGrid,
				dc,
				nVisibleColNo,
				nVisibleRowNo,
				nColNo,
				nRowNo,
				nColType,
				nRowType,
				dwAreaFlags,
				dwHelperPaintFlags
				);
CSize sizeCheck = OnCalcCheckSize();
	ASSERT( sizeIcon.cx >= 0 && sizeIcon.cy >= 0 );
	ASSERT( sizeCheck.cx >= 0 && sizeCheck.cy >= 0 );
INT		nExtentUpdownButton =
			OnCalcButtonExtent(
				INT(__EBTT_UPDOWN_UP),
				wndGrid,
				dc,
				nVisibleColNo,
				nVisibleRowNo,
				nColNo,
				nRowNo,
				nColType,
				nRowType,
				dwAreaFlags,
				dwHelperPaintFlags
				),
		nExtentDropdownButton =
			OnCalcButtonExtent(
				INT(__EBTT_DROPDOWN),
				wndGrid,
				dc,
				nVisibleColNo,
				nVisibleRowNo,
				nColNo,
				nRowNo,
				nColType,
				nRowType,
				dwAreaFlags,
				dwHelperPaintFlags
				),
		nExtentEllipsisButton =
			OnCalcButtonExtent(
				INT(__EBTT_ELLIPSIS),
				wndGrid,
				dc,
				nVisibleColNo,
				nVisibleRowNo,
				nColNo,
				nRowNo,
				nColType,
				nRowType,
				dwAreaFlags,
				dwHelperPaintFlags
				),
		nFocusGlyphType = -1;
	ASSERT( nExtentUpdownButton >= 0 );
	ASSERT( nExtentDropdownButton >= 0 );
	ASSERT( nExtentEllipsisButton >= 0 );
DWORD dwCellStyle = GetStyle();
DWORD dwCellStyleEx = GetStyleEx();
	OnCalcLayout(
		dwCellStyle,
		dwCellStyleEx,
		rcCellRest,
		rcCellText,
		rcFocusArrow,
		rcIcon,
		rcCheck,
		rcSortArrow,
		rcUpdownButtonTop,
		rcUpdownButtonBottom,
		rcDropdownButton,
		rcEllipsisButton,
		sizeFocusArrow,
		sizeIcon,
		sizeCheck,
		sizeSortArrow,
		nExtentUpdownButton,
		nExtentDropdownButton,
		nExtentEllipsisButton,
		nFocusGlyphType,
		wndGrid,
		dc,
		nVisibleColNo,
		nVisibleRowNo,
		nColNo,
		nRowNo,
		nColType,
		nRowType,
		dwAreaFlags,
		dwHelperPaintFlags
		);
	if(		rcFocusArrow.left < rcFocusArrow.right
		&&	rcFocusArrow.top < rcFocusArrow.bottom
		&&	nFocusGlyphType >= 0
		)
	{ // if draw focus arrow glyph
		CExtPaintManager::glyph_t * pFocusArrowGlyph =
			CExtPaintManager::g_DockingCaptionGlyphs[ nFocusGlyphType ];
		ASSERT( pFocusArrowGlyph != NULL );
		COLORREF ColorValues[2] =
		{
			RGB(0,0,0),
			RGB(0,0,0),
		};
		ColorValues[1] = 
			OnQueryTextColor(
				wndGrid,
				dc,
				nVisibleColNo,
				nVisibleRowNo,
				nColNo,
				nRowNo,
				nColType,
				nRowType,
				dwAreaFlags,
				dwHelperPaintFlags
				);
		if( ColorValues[1] == COLORREF(-1) )
			ColorValues[1] = dc.GetTextColor();
		g_PaintManager->PaintGlyphCentered(
			dc,
			rcFocusArrow,
			*pFocusArrowGlyph,
			ColorValues
			);
	} // if draw focus arrow glyph
	if(		rcIcon.left < rcIcon.right
		&&	rcIcon.top < rcIcon.bottom
		)
	{
		const CExtCmdIcon * pIcon = IconGet();
		if(		pIcon != NULL
			&&	( pIcon->IsEmpty() )
			)
			pIcon = NULL;
		OnPaintIcon(
			pIcon,
			rcIcon,
			wndGrid,
			dc,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			nColType,
			nRowType,
			rcCellExtra,
			rcCell,
			rcVisibleRange,
			dwAreaFlags,
			dwHelperPaintFlags
			);
	}
	if(		rcCheck.left < rcCheck.right
		&&	rcCheck.top < rcCheck.bottom
		)
		OnPaintCheck(
			rcCheck,
			wndGrid,
			dc,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			nColType,
			nRowType,
			rcCellExtra,
			rcCell,
			rcVisibleRange,
			dwAreaFlags,
			dwHelperPaintFlags
			);

	if(		rcEllipsisButton.left < rcEllipsisButton.right
		&&	rcEllipsisButton.top < rcEllipsisButton.bottom
		)
	{ // if draw ellipsis button
		OnPaintButton(
			rcEllipsisButton,
			INT(__EBTT_ELLIPSIS),
			((dwCellStyle&__EGCS_PRESSED_ELLIPSIS)!=0L) ? true : false,
			IsButtonEnabled( INT(__EBTT_ELLIPSIS) ),
			wndGrid,
			dc,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			nColType,
			nRowType,
			rcCellExtra,
			rcCell,
			rcVisibleRange,
			dwAreaFlags,
			dwHelperPaintFlags
			);
	} // if draw ellipsis button
	if(		rcDropdownButton.left < rcDropdownButton.right
		&&	rcDropdownButton.top < rcDropdownButton.bottom
		)
	{ // if draw dropdown button
		OnPaintButton(
			rcDropdownButton,
			INT(__EBTT_DROPDOWN),
			((dwCellStyle&__EGCS_PRESSED_DROPDOWN)!=0L) ? true : false,
			IsButtonEnabled( INT(__EBTT_DROPDOWN) ),
			wndGrid,
			dc,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			nColType,
			nRowType,
			rcCellExtra,
			rcCell,
			rcVisibleRange,
			dwAreaFlags,
			dwHelperPaintFlags
			);
	} // if draw dropdown button
	if(		rcUpdownButtonTop.left < rcUpdownButtonTop.right
		&&	rcUpdownButtonTop.top < rcUpdownButtonTop.bottom
		)
	{ // if draw updown top-button
		OnPaintButton(
			rcUpdownButtonTop,
			INT(__EBTT_UPDOWN_UP),
			((dwCellStyle&__EGCS_PRESSED_UP)!=0L) ? true : false,
			IsButtonEnabled( INT(__EBTT_UPDOWN_UP) ),
			wndGrid,
			dc,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			nColType,
			nRowType,
			rcCellExtra,
			rcCell,
			rcVisibleRange,
			dwAreaFlags,
			dwHelperPaintFlags
			);
	} // if draw updown top-button
	if(		rcUpdownButtonBottom.left < rcUpdownButtonBottom.right
		&&	rcUpdownButtonBottom.top < rcUpdownButtonBottom.bottom
		)
	{ // if draw updown bottom-button
		OnPaintButton(
			rcUpdownButtonBottom,
			INT(__EBTT_UPDOWN_DOWN),
			((dwCellStyle&__EGCS_PRESSED_DOWN)!=0L) ? true : false,
			IsButtonEnabled( INT(__EBTT_UPDOWN_DOWN) ),
			wndGrid,
			dc,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			nColType,
			nRowType,
			rcCellExtra,
			rcCell,
			rcVisibleRange,
			dwAreaFlags,
			dwHelperPaintFlags
			);
	} // if draw updown bottom-button

	if(		rcSortArrow.left < rcSortArrow.right
		&&	rcSortArrow.top < rcSortArrow.bottom
		)
		OnPaintSortArrow(
			rcSortArrow,
			wndGrid,
			dc,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			nColType,
			nRowType,
			rcCellExtra,
			rcCell,
			rcVisibleRange,
			dwAreaFlags,
			dwHelperPaintFlags
			);
	if(		rcCellText.left < rcCellText.right
		&&	rcCellText.top < rcCellText.bottom
		)
		OnPaintText(
			rcCellText,
			wndGrid,
			dc,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			nColType,
			nRowType,
			rcCellExtra,
			rcCell,
			rcVisibleRange,
			dwAreaFlags,
			dwHelperPaintFlags
			);
}

void CExtGridCell::OnCalcLayout(
	DWORD dwCellStyle,
	DWORD dwCellStyleEx,
	CRect & rcCellRest,
	CRect & rcCellText,
	CRect & rcFocusArrow,
	CRect & rcIcon,
	CRect & rcCheck,
	CRect & rcSortArrow,
	CRect & rcUpdownButtonTop,
	CRect & rcUpdownButtonBottom,
	CRect & rcDropdownButton,
	CRect & rcEllipsisButton,
	CSize sizeFocusArrow,
	CSize sizeIcon,
	CSize sizeCheck,
	CSize sizeSortArrow,
	INT nExtentUpdownButton,
	INT nExtentDropdownButton,
	INT nExtentEllipsisButton,
	INT & nFocusGlyphType,
	const CExtGridWnd & wndGrid,
	CDC & dc,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	DWORD dwAreaFlags,
	DWORD dwHelperPaintFlags
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( sizeFocusArrow.cx >= 0 && sizeFocusArrow.cy >= 0 );
	ASSERT( sizeIcon.cx >= 0 && sizeIcon.cy >= 0 );
	ASSERT( sizeCheck.cx >= 0 && sizeCheck.cy >= 0 );
	ASSERT( sizeSortArrow.cx >= 0 && sizeSortArrow.cy >= 0 );
	ASSERT( nExtentUpdownButton >= 0 );
	ASSERT( nExtentDropdownButton >= 0 );
	ASSERT( nExtentEllipsisButton >= 0 );
	dc;
	dwCellStyleEx;
	dwHelperPaintFlags;
	nVisibleColNo;
	nVisibleRowNo;
	rcCellText.SetRect( 0, 0, 0, 0 );
	rcFocusArrow.SetRect( 0, 0, 0, 0 );
	rcIcon.SetRect( 0, 0, 0, 0 );
	rcCheck.SetRect( 0, 0, 0, 0 );
	rcSortArrow.SetRect( 0, 0, 0, 0 );
	rcUpdownButtonTop.SetRect( 0, 0, 0, 0 );
	rcUpdownButtonBottom.SetRect( 0, 0, 0, 0 );
	rcDropdownButton.SetRect( 0, 0, 0, 0 );
	rcEllipsisButton.SetRect( 0, 0, 0, 0 );
	nFocusGlyphType = -1;

	rcCellRest.DeflateRect( 1, 1 );
	if(		rcCellRest.left >= rcCellRest.right
		||	rcCellRest.top >= rcCellRest.bottom
		)
		return; // if rest space is empty
	
	if(		(dwCellStyle&(__EGCS_HDR_FOCUS_ARROW_RESERVE_SPACE|__EGCS_HDR_FOCUS_ARROW_DISPLAY)) != 0L
		&&	( dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0
		&&	(	(	nColType != 0
				&&	nRowType == 0
				)
			||	(	nRowType != 0
				&&	nColType == 0
				)
			)
		)
	{ // if draw focus arrow
		CPoint ptFocus = wndGrid.FocusGet();
		if(		(dwCellStyle&__EGCS_HDR_FOCUS_ARROW_RESERVE_SPACE) != 0
			||	(
						(dwCellStyle&__EGCS_HDR_FOCUS_ARROW_DISPLAY) != 0
					&&	(
							(	nColType != 0
							&&	ptFocus.y == nRowNo
							)
						||
							(	nRowType != 0
							&&	ptFocus.x == nColNo
							)
						)
				)
			)
		{
			rcFocusArrow = rcCellRest;
			if( nColType != 0 )
			{ // if focus arrow for row
				if( nColType < 0 )
				{
					rcFocusArrow.left = rcFocusArrow.right - sizeFocusArrow.cx;
					rcCellRest.right -= rcFocusArrow.Width();
					if(		ptFocus.y == nRowNo
						&&	(dwCellStyle&__EGCS_HDR_FOCUS_ARROW_DISPLAY) != 0L
						)
						nFocusGlyphType = (INT)CExtPaintManager::__DCBT_ARROW_RIGHT;
				} // if( nColType < 0 )
				else
				{
					rcFocusArrow.right = rcFocusArrow.left + sizeFocusArrow.cx;
					rcCellRest.left += rcFocusArrow.Width();
					if(		ptFocus.y == nRowNo
						&&	(dwCellStyle&__EGCS_HDR_FOCUS_ARROW_DISPLAY) != 0L
						)
						nFocusGlyphType = (INT) CExtPaintManager::__DCBT_ARROW_LEFT;
				} // else from if( nColType < 0 )
			} // if focus arrow for row
			if( nRowType != 0 )
			{ // if focus arrow for column
				rcFocusArrow.right = rcFocusArrow.left + sizeFocusArrow.cx;
				rcCellRest.left += rcFocusArrow.Width();
				if( nRowType < 0 )
				{
					if(		ptFocus.x == nColNo
						&&	(dwCellStyle&__EGCS_HDR_FOCUS_ARROW_DISPLAY) != 0L
						)
						nFocusGlyphType = (INT)CExtPaintManager::__DCBT_ARROW_DOWN;
				} // if( nRowType < 0 )
				else
				{
					if(		ptFocus.x == nColNo
						&&	(dwCellStyle&__EGCS_HDR_FOCUS_ARROW_DISPLAY) != 0L
						)
						nFocusGlyphType = (INT)CExtPaintManager::__DCBT_ARROW_UP;
				} // else from if( nRowType < 0 )
			} // if focus arrow for column
		}
	} // if draw focus arrow

	if(		rcCellRest.left >= rcCellRest.right
		||	rcCellRest.top >= rcCellRest.bottom
		)
		return; // if rest space is empty
	if( nExtentEllipsisButton > 0 )
	{
		rcEllipsisButton = rcCellRest;
		rcEllipsisButton.left
			= rcCellRest.right
			= rcEllipsisButton.right - nExtentEllipsisButton;
		//rcEllipsisButton.DeflateRect( 0, 0, 1, 1 );
	} // if( nExtentEllipsisButton > 0 )

	if(		rcCellRest.left >= rcCellRest.right
		||	rcCellRest.top >= rcCellRest.bottom
		)
		return; // if rest space is empty
	if( nExtentDropdownButton > 0 )
	{
		rcDropdownButton = rcCellRest;
		rcDropdownButton.left
			= rcCellRest.right
			= rcDropdownButton.right - nExtentDropdownButton;
		//rcDropdownButton.DeflateRect( 0, 0, 1, 1 );
	} // if( nExtentDropdownButton > 0 )

	if(		rcCellRest.left >= rcCellRest.right
		||	rcCellRest.top >= rcCellRest.bottom
		)
		return; // if rest space is empty
	if( nExtentUpdownButton > 0 )
	{
		rcUpdownButtonTop = rcCellRest;
		rcUpdownButtonTop.left
			= rcCellRest.right
			= rcUpdownButtonTop.right - nExtentUpdownButton;
		rcUpdownButtonBottom = rcUpdownButtonTop;
		rcUpdownButtonTop.bottom
			= rcUpdownButtonBottom.top
			= rcUpdownButtonTop.top + (rcUpdownButtonTop.Height()/2);
		//rcUpdownButtonTop.DeflateRect( 0, 0, 1, 1 );
		//rcUpdownButtonBottom.DeflateRect( 0, 0, 1, 1 );
	} // if( nExtentUpdownButton > 0 )

	if(		rcCellRest.left >= (rcCellRest.right-2)
		||	rcCellRest.top >= rcCellRest.bottom
		)
		return; // if rest space is empty
	if( sizeIcon.cx > 0 || sizeCheck.cx > 0 )
	{
		rcCellRest.left += 2;
		CSize sizeBoth(
			sizeIcon.cx + sizeCheck.cx,
			max( sizeIcon.cy, sizeCheck.cy )
			);
		if( sizeIcon.cx > 0 && sizeCheck.cx > 0 )
		{
			ASSERT( sizeIcon.cx > 0 && sizeIcon.cy > 0 );
			ASSERT( sizeCheck.cx > 0 && sizeCheck.cy > 0 );
			sizeBoth.cx += 2;
		} // if( sizeIcon.cx > 0 && sizeCheck.cx > 0 )
		CRect rcBoth( rcCellRest.TopLeft(), sizeBoth );
		DWORD dwCellStyle = GetStyle();
		switch( (dwCellStyle & __EGCS_ICA_VERT_MASK) )
		{
		case __EGCS_ICA_VERT_TOP:
		break;
		case __EGCS_ICA_VERT_BOTTOM:
			rcBoth.OffsetRect(
				0,
				rcCellRest.Height() - rcBoth.Height()
				);
		break;
		case __EGCS_ICA_VERT_BY_TYPE:
		case __EGCS_ICA_VERT_CENTER:
			rcBoth.OffsetRect(
				0,
				( rcCellRest.Height() - rcBoth.Height() ) / 2
				);
		break;
#ifdef _DEBUG
		default:
			ASSERT( FALSE );
		break;
#endif // _DEBUG
		} // switch( (dwCellStyle & __EGCS_ICA_VERT_MASK) )
		switch( (dwCellStyle & __EGCS_ICA_HORZ_MASK) )
		{
		case __EGCS_ICA_HORZ_BY_TYPE:
		case __EGCS_ICA_HORZ_LEFT:
			rcCellRest.left = rcBoth.right;
		break;
		case __EGCS_ICA_HORZ_RIGHT:
			rcBoth.OffsetRect(
				rcCellRest.Width() - rcBoth.Width(),
				0
				);
			rcCellRest.right = rcBoth.left;
		break;
		case __EGCS_ICA_HORZ_CENTER:
			{
				bool bTextIsEmpty = true;
				LPCTSTR strTextBuffer = LPCTSTR( GetTextBuffer() );
				if( strTextBuffer != NULL )
				{
					if( _tcslen( strTextBuffer ) > 0 )
						bTextIsEmpty = false;
				} // if( strTextBuffer != NULL )
				else
				{
					CExtSafeString strCopy;
					TextGet( strCopy );
					if( ! strCopy.IsEmpty() )
						bTextIsEmpty = false;
				} // else from if( strTextBuffer != NULL )
				if( bTextIsEmpty )
					rcBoth.OffsetRect(
						( rcCellRest.Width() - rcBoth.Width() ) / 2,
						0
						);
				else
					rcCellRest.left = rcBoth.right;
			}
		break;
#ifdef _DEBUG
		default:
			ASSERT( FALSE );
		break;
#endif // _DEBUG
		} // switch( (dwCellStyle & __EGCS_ICA_HORZ_MASK) )
		if( sizeIcon.cx > 0 )
		{
			ASSERT( sizeIcon.cx > 0 && sizeIcon.cy > 0 );
			rcIcon = rcBoth;
			rcIcon.right = rcIcon.left + sizeIcon.cx;
			if( sizeCheck.cx > 0 )
			{
				ASSERT( sizeCheck.cx > 0 && sizeCheck.cy > 0 );
				rcCheck = rcBoth;
				rcCheck.left = rcIcon.right + 2;
			} // if( sizeCheck.cx > 0 )
		} // if( sizeIcon.cx > 0 )
		else if( sizeCheck.cx > 0 )
		{
			ASSERT( sizeCheck.cx > 0 && sizeCheck.cy > 0 );
			rcCheck = rcBoth;
		} // else if( sizeCheck.cx > 0 )
	} // if( sizeIcon.cx > 0 || sizeCheck.cx > 0 )

	if(		rcCellRest.left >= rcCellRest.right
		||	rcCellRest.top >= rcCellRest.bottom
		)
		return; // if rest space is empty
	if( sizeSortArrow.cx > 0 && sizeSortArrow.cy > 0 )
	{
		rcSortArrow = rcCellRest;
		rcSortArrow.left = rcCellRest.right =
			rcCellRest.right - sizeSortArrow.cx;
		rcSortArrow.bottom = rcSortArrow.top + sizeSortArrow.cy;
		rcSortArrow.OffsetRect(
			0,
			( rcCellRest.Height() - rcSortArrow.Height() ) / 2
			);
	} // if( sizeSortArrow.cx > 0 && sizeSortArrow.cy > 0 )

	if(		rcCellRest.left >= (rcCellRest.right-4)
		||	rcCellRest.top >= (rcCellRest.bottom-2)
		)
		return; // if rest space is empty
	rcCellText = rcCellRest;
	rcCellText.DeflateRect( 2, 1 );
}

CSize CExtGridCell::OnCalcFocusArrowSize(
	const CExtGridWnd & wndGrid,
	CDC & dc,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	DWORD dwAreaFlags,
	DWORD dwHelperPaintFlags
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( dc.GetSafeHdc() != NULL );
	wndGrid;
	dc;
	nVisibleColNo;
	nVisibleRowNo;
	nColNo;
	nRowNo;
	dwAreaFlags;
	dwHelperPaintFlags;
	if( nColType == 0 && nRowType == 0 )
		return CSize(0,0);
DWORD dwCellStyle = GetStyle();
	if( (dwCellStyle&(__EGCS_HDR_FOCUS_ARROW_RESERVE_SPACE|__EGCS_HDR_FOCUS_ARROW_DISPLAY)) == 0 )
		return CSize(0,0);
	return
		CSize(
			__EXT_FOCUS_ARROW_GLYPH_EXTENT_HORZ,
			__EXT_FOCUS_ARROW_GLYPH_EXTENT_VERT
			);
}

CSize CExtGridCell::OnCalcSortArrowSize(
	const CExtGridWnd & wndGrid,
	CDC & dc,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	DWORD dwAreaFlags,
	DWORD dwHelperPaintFlags
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( dc.GetSafeHdc() != NULL );
	wndGrid;
	dc;
	nVisibleColNo;
	nVisibleRowNo;
	nColNo;
	nRowNo;
	dwAreaFlags;
	dwHelperPaintFlags;
	if( nColType == 0 && nRowType == 0 )
		return CSize(0,0);
DWORD dwCellStyle = GetStyle();
	if( (dwCellStyle&__EGCS_SORT_ARROW) == 0 )
		return CSize(0,0);
	return
		CSize(
			__EXT_SORT_ARROW_GLYPH_EXTENT_HORZ,
			__EXT_SORT_ARROW_GLYPH_EXTENT_VERT
			);
}

void CExtGridCell::OnPaintSortArrow(
	const RECT & rcSortArrow,
	const CExtGridWnd & wndGrid,
	CDC & dc,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	const RECT & rcVisibleRange,
	DWORD dwAreaFlags,
	DWORD dwHelperPaintFlags
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcSortArrow) )
		return;
	nVisibleColNo;
	nVisibleRowNo;
	nColNo;
	nRowNo;
	rcCellExtra;
	rcCell;
	rcVisibleRange;
	dwAreaFlags;
	dwHelperPaintFlags;
	if( nColType == 0 && nRowType == 0 )
		return;
DWORD dwCellStyle = GetStyle();
	if( (dwCellStyle&__EGCS_SORT_ARROW) == 0 )
		return;
CExtPaintManager::glyph_t * pSortArrowGlyph = NULL;
	if( (dwCellStyle&__EGCS_SORT_DESCENDING) != 0 )
	{
		if( nColType != 0 )
		{
			ASSERT( nRowType == 0 );
			pSortArrowGlyph =
				CExtPaintManager::g_DockingCaptionGlyphs[
					(INT) CExtPaintManager::__DCBT_ARROW_SORT_RIGHT
					];
		} // if( nColType != 0 )
		else
		{
			ASSERT( nRowType != 0 );
			pSortArrowGlyph =
				CExtPaintManager::g_DockingCaptionGlyphs[
					(INT) CExtPaintManager::__DCBT_ARROW_SORT_DOWN
					];
		} // else from if( nColType != 0 )
	} // if( (dwCellStyle&__EGCS_SORT_DESCENDING) != 0 )
	else
	{
		if( nColType != 0 )
		{
			ASSERT( nRowType == 0 );
			pSortArrowGlyph =
				CExtPaintManager::g_DockingCaptionGlyphs[
					(INT) CExtPaintManager::__DCBT_ARROW_SORT_LEFT
					];
		} // if( nColType != 0 )
		else
		{
			ASSERT( nRowType != 0 );
			pSortArrowGlyph =
				CExtPaintManager::g_DockingCaptionGlyphs[
					(INT) CExtPaintManager::__DCBT_ARROW_SORT_UP
					];
		} // else from if( nColType != 0 )
	} // else from if( (dwCellStyle&__EGCS_SORT_DESCENDING) != 0 )
	ASSERT( pSortArrowGlyph != NULL );
COLORREF ColorValues[4] =
{
	RGB(0,0,0),
	wndGrid.OnSiwGetSysColor( COLOR_3DSHADOW ),
	wndGrid.OnSiwGetSysColor( COLOR_3DHIGHLIGHT ),
	wndGrid.OnSiwGetSysColor( COLOR_3DDKSHADOW ),
};
	g_PaintManager->PaintGlyphCentered(
		dc,
		rcSortArrow,
		*pSortArrowGlyph,
		ColorValues
		);
}

void CExtGridCell::OnPaintText(
	const RECT & rcCellText,
	const CExtGridWnd & wndGrid,
	CDC & dc,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	const RECT & rcVisibleRange,
	DWORD dwAreaFlags,
	DWORD dwHelperPaintFlags
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcCellText) )
		return;
	rcCellExtra;
	rcCell;
	rcVisibleRange;
COLORREF clrTextOld = COLORREF(-1L);
HFONT hOldFont = NULL;
HFONT hCellFont =
		OnQueryCellFont(
			wndGrid,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			nColType,
			nRowType,
			dwAreaFlags
			);
	if( hCellFont != NULL )
		hOldFont = (HFONT)::SelectObject( dc, hCellFont );
	if(		rcCellText.left < rcCellText.right
		&&	rcCellText.top < rcCellText.bottom
		)
	{ // if text painting area is not empty
		UINT nDrawTextFlags =
			OnQueryDrawTextFlags(
				nVisibleColNo,
				nVisibleRowNo,
				nColNo,
				nRowNo,
				nColType,
				nRowType,
				dwAreaFlags
				);
		DWORD dwCellStyle = GetStyle();
		if(		( dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0
			&&	(dwCellStyle&__EGCS_HDR_ROW_COLUMN_NUMBER) != 0L
			&&	( nColType != 0 || nRowType != 0 )
			)
		{ // if draw row/column number text
			CExtSafeString strText;
			OnFormatHeaderNumberString(
				strText,
				wndGrid,
				nVisibleColNo,
				nVisibleRowNo,
				nColNo,
				nRowNo,
				nColType,
				nRowType,
				dwAreaFlags
				);
			if( ! strText.IsEmpty() )
			{
				COLORREF clrTextNew =
					OnQueryTextColor(
						wndGrid,
						dc,
						nVisibleColNo,
						nVisibleRowNo,
						nColNo,
						nRowNo,
						nColType,
						nRowType,
						dwAreaFlags,
						dwHelperPaintFlags
						);
				if( clrTextNew != COLORREF(-1L) )
				{
					COLORREF clrTextLast =
						dc.SetTextColor( clrTextNew );
					if( clrTextOld == COLORREF(-1L) )
						clrTextOld = clrTextLast;
				} // if( clrTextNew != COLORREF(-1L) )
				dc.DrawText(
					LPCTSTR( strText ),
					strText.GetLength(),
					(RECT *)&rcCellText,
					nDrawTextFlags
					);
			} // if( ! strText.IsEmpty() )
		} // if draw row/column number text
		else
		{ // if draw cell default text
			LPCTSTR strTextBuffer = LPCTSTR( GetTextBuffer() );
			int nTextBufferLen = 
				( strTextBuffer != NULL )
					? _tcslen( strTextBuffer )
					: 0;
			if( nTextBufferLen > 0 )
			{
					COLORREF clrTextNew =
						OnQueryTextColor(
							wndGrid,
							dc,
							nVisibleColNo,
							nVisibleRowNo,
							nColNo,
							nRowNo,
							nColType,
							nRowType,
							dwAreaFlags,
							dwHelperPaintFlags
							);
					if( clrTextNew != COLORREF(-1L) )
					{
						COLORREF clrTextLast =
							dc.SetTextColor( clrTextNew );
						if( clrTextOld == COLORREF(-1L) )
							clrTextOld = clrTextLast;
					} // if( clrTextNew != COLORREF(-1L) )
					dc.DrawText(
						strTextBuffer,
						nTextBufferLen,
						(RECT *)&rcCellText,
						nDrawTextFlags
						);
			} // if( nTextBufferLen > 0 )
			else
			{
				CExtSafeString strCopy;
				TextGet( strCopy );
				if( ! strCopy.IsEmpty() )
				{
					COLORREF clrTextNew =
						OnQueryTextColor(
							wndGrid,
							dc,
							nVisibleColNo,
							nVisibleRowNo,
							nColNo,
							nRowNo,
							nColType,
							nRowType,
							dwAreaFlags,
							dwHelperPaintFlags
							);
					if( clrTextNew != COLORREF(-1L) )
					{
						COLORREF clrTextLast =
							dc.SetTextColor( clrTextNew );
						if( clrTextOld == COLORREF(-1L) )
							clrTextOld = clrTextLast;
					} // if( clrTextNew != COLORREF(-1L) )
					dc.DrawText(
						LPCTSTR( strCopy ),
						strCopy.GetLength(),
						(RECT *)&rcCellText,
						nDrawTextFlags
						);
				} // if( ! strCopy.IsEmpty() )
			} // else from if( nTextBufferLen > 0 )
		} // if draw cell default text
	} // if text painting area is not empty
	if( clrTextOld != COLORREF(-1L) )
		dc.SetTextColor( clrTextOld );
	if( hOldFont != NULL )
		::SelectObject( dc, hOldFont );
}

HFONT CExtGridCell::OnQueryCellFont(
	const CExtGridWnd & wndGrid,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	DWORD dwAreaFlags
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	wndGrid;
	nVisibleColNo;
	nVisibleRowNo;
	nColNo;
	nRowNo;
	nColType;
	nRowType;
	dwAreaFlags;
	return ((HFONT)NULL);
}

CSize CExtGridCell::OnMeasureTextSize(
	const CExtGridWnd & wndGrid,
	CDC & dc,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	DWORD dwAreaFlags
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( dc.GetSafeHdc() != NULL );
HFONT hOldFont = NULL;
HFONT hCellFont =
		OnQueryCellFont(
			wndGrid,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			nColType,
			nRowType,
			dwAreaFlags
			);
	if( hCellFont == NULL )
		hCellFont = (HFONT)
			wndGrid.OnSiwGetDefaultFont().GetSafeHandle();
	if( hCellFont != NULL )
		hOldFont = (HFONT)::SelectObject( dc, hCellFont );
CRect rcCellTextMeasured( 0, 0, 0, 0 );
DWORD dwCellStyle = GetStyle();
const UINT nDrawTextFlags = DT_SINGLELINE|DT_LEFT|DT_TOP|DT_CALCRECT;
	if(		( dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0
		&&	(dwCellStyle&__EGCS_HDR_ROW_COLUMN_NUMBER) != 0L
		&&	( nColType != 0 || nRowType != 0 )
		)
	{ // if draw row/column number text
		CExtSafeString strText;
		OnFormatHeaderNumberString(
			strText,
			wndGrid,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			nColType,
			nRowType,
			dwAreaFlags
			);
		if( ! strText.IsEmpty() )
		{
			dc.DrawText(
				LPCTSTR( strText ),
				strText.GetLength(),
				(RECT *)&rcCellTextMeasured,
				nDrawTextFlags
				);
		} // if( ! strText.IsEmpty() )
	} // if draw row/column number text
	else
	{ // if draw cell default text
		LPCTSTR strTextBuffer = LPCTSTR( GetTextBuffer() );
		int nTextBufferLen = 
			( strTextBuffer != NULL )
				? _tcslen( strTextBuffer )
				: 0;
		if( nTextBufferLen > 0 )
		{
			dc.DrawText(
				strTextBuffer,
				nTextBufferLen,
				(RECT *)&rcCellTextMeasured,
				nDrawTextFlags
				);
		} // if( nTextBufferLen > 0 )
		else
		{
			CExtSafeString strCopy;
			TextGet( strCopy );
			if( ! strCopy.IsEmpty() )
			{
				dc.DrawText(
					LPCTSTR( strCopy ),
					strCopy.GetLength(),
					(RECT *)&rcCellTextMeasured,
					nDrawTextFlags
					);
			} // if( ! strCopy.IsEmpty() )
		} // else from if( nTextBufferLen > 0 )
	} // if draw cell default text
CRect rcCellTextMeasured2( 0, 0, 0, 0 );
static CExtSafeString g_sTestText( _T("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789;[]{}\\/=+-_*&^%$#@!~") );
	dc.DrawText(
		LPCTSTR( g_sTestText ),
		g_sTestText.GetLength(),
		(RECT *)&rcCellTextMeasured2,
		nDrawTextFlags
		);
INT nAlignHeight = rcCellTextMeasured2.Height();
	if( hOldFont != NULL )
		::SelectObject( dc, hOldFont );
CSize sizeText = rcCellTextMeasured.Size();
	sizeText.cy = max( sizeText.cy, nAlignHeight );
	return sizeText;
}

INT CExtGridCell::OnCalcButtonExtent(
	INT nButtonType, // e_button_type_t
	const CExtGridWnd & wndGrid,
	CDC & dc,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	DWORD dwAreaFlags,
	DWORD dwHelperPaintFlags
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
	nVisibleColNo;
	nVisibleRowNo;
	nColType;
	nRowType;
	dwAreaFlags;
DWORD dwBseStyle = wndGrid.BseGetStyle();
	if( (dwBseStyle&__EGWS_BSE_MASK) == 0 )
		return 0;
DWORD dwCellStyle = GetStyle();
	switch( nButtonType )
	{
	case __EBTT_ELLIPSIS:
		if( (dwCellStyle&__EGCS_BUTTON_ELLIPSIS) == 0 )
			return 0;
	break;
	case __EBTT_DROPDOWN:
		if( (dwCellStyle&__EGCS_BUTTON_DROPDOWN) == 0 )
			return 0;
	break;
	case __EBTT_UPDOWN_UP:
	case __EBTT_UPDOWN_DOWN:
		if( (dwCellStyle&__EGCS_BUTTON_UPDOWN) == 0 )
			return 0;
	break;
	} // switch( nButtonType )
	if( (dwBseStyle&__EGWS_BSE_BUTTONS_PERSISTENT) != 0 )
		return ( ::GetSystemMetrics( SM_CXVSCROLL ) );
	if(		(dwBseStyle&
				(__EGWS_BSE_BUTTONS_IN_FOCUSED_CELL
				|__EGWS_BSE_BUTTONS_IN_FOCUSED_ROW
				|__EGWS_BSE_BUTTONS_IN_FOCUSED_COLUMN)
			) != 0
		)
	{
		CPoint ptFocus = wndGrid.FocusGet();
		if(		(dwBseStyle&__EGWS_BSE_BUTTONS_IN_FOCUSED_CELL) != 0
			&&	ptFocus.x == nColNo
			&&	ptFocus.y == nRowNo
			)
			return ( ::GetSystemMetrics( SM_CXVSCROLL ) );
		if(		(dwBseStyle&__EGWS_BSE_BUTTONS_IN_FOCUSED_COLUMN) != 0
			&&	ptFocus.x == nColNo
			)
			return ( ::GetSystemMetrics( SM_CXVSCROLL ) );
		if(		(dwBseStyle&__EGWS_BSE_BUTTONS_IN_FOCUSED_ROW) != 0
			&&	ptFocus.y == nRowNo
			)
			return ( ::GetSystemMetrics( SM_CXVSCROLL ) );
	}
	if(		(dwBseStyle&
				(__EGWS_BSE_BUTTONS_IN_HOVERED_CELL
				|__EGWS_BSE_BUTTONS_IN_HOVERED_ROW
				|__EGWS_BSE_BUTTONS_IN_HOVERED_COLUMN)
			) != 0
		)
	{
		bool bHoverByColumn = ( (dwHelperPaintFlags&__EGCPF_HOVER_BY_COLUMN) != 0 ) ? true : false;
		bool bHoverByRow = ( (dwHelperPaintFlags&__EGCPF_HOVER_BY_ROW) != 0 ) ? true : false;
		if(		(dwBseStyle&__EGWS_BSE_BUTTONS_IN_HOVERED_CELL) != 0
			&&	bHoverByColumn
			&&	bHoverByRow
			)
			return ( ::GetSystemMetrics( SM_CXVSCROLL ) );
		if(		(dwBseStyle&__EGWS_BSE_BUTTONS_IN_HOVERED_COLUMN) != 0
			&&	bHoverByColumn
			)
			return ( ::GetSystemMetrics( SM_CXVSCROLL ) );
		if(		(dwBseStyle&__EGWS_BSE_BUTTONS_IN_HOVERED_ROW) != 0
			&&	bHoverByRow
			)
			return ( ::GetSystemMetrics( SM_CXVSCROLL ) );
	}	
	if(		(dwBseStyle&
				(__EGWS_BSE_BUTTONS_IN_SELECTED_CELLS
				|__EGWS_BSE_BUTTONS_IN_SELECTED_ROW
				|__EGWS_BSE_BUTTONS_IN_SELECTED_COLUMN)
			) != 0
		)
	{
		//bool bHighlightedBySelectedColumn = ( (dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_SELECTED_COLUMN) != 0 ) ? true : false;
		//bool bHighlightedBySelectedRow = ( (dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_SELECTED_ROW) != 0 ) ? true : false;
		if(		(dwBseStyle&__EGWS_BSE_BUTTONS_IN_SELECTED_CELLS) != 0
			//&&	bHighlightedBySelectedColumn
			//&&	bHighlightedBySelectedRow
			&&	wndGrid.SelectionGetForCell( nColNo, nRowNo )
			)
			return ( ::GetSystemMetrics( SM_CXVSCROLL ) );
		if(		(dwBseStyle&__EGWS_BSE_BUTTONS_IN_SELECTED_COLUMN) != 0
			//&&	bHighlightedBySelectedColumn
			//&&	wndGrid.SelectionGetFirstRowInColumn( nColNo ) >= 0L
			&&	(dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_SELECTED_COLUMN) != 0
			)
			return ( ::GetSystemMetrics( SM_CXVSCROLL ) );
		if(		(dwBseStyle&__EGWS_BSE_BUTTONS_IN_SELECTED_ROW) != 0
			//&&	bHighlightedBySelectedRow
			//&&	wndGrid.SelectionGetFirstColumnInRow( nRowNo ) >= 0L
			&&	(dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_SELECTED_ROW) != 0
			)
			return ( ::GetSystemMetrics( SM_CXVSCROLL ) );
	}	
	return 0;
}

bool CExtGridCell::IsButtonEnabled(
	INT nButtonType // e_button_type_t
	) const
{
	ASSERT_VALID( this );
bool bEnabled = false;
	if( ! OnQueryButtonInfo( nButtonType, &bEnabled ) )
		return false;
	return bEnabled;
}

bool CExtGridCell::IsButtonPressed(
	INT nButtonType // e_button_type_t
	) const
{
	ASSERT_VALID( this );
bool bPressed = false;
	if( ! OnQueryButtonInfo( nButtonType, NULL, &bPressed ) )
		return false;
	return bPressed;
}

bool CExtGridCell::IsButtonStayPressed(
	INT nButtonType // e_button_type_t
	) const
{
	ASSERT_VALID( this );
bool bStayPressed = false;
	if( ! OnQueryButtonInfo( nButtonType, NULL, NULL, &bStayPressed ) )
		return false;
	return bStayPressed;
}

void CExtGridCell::OnPaintButton(
	const RECT & rcButton,
	INT nButtonType, // e_button_type_t
	bool bPressed,
	bool bEnabled,
	const CExtGridWnd & wndGrid,
	CDC & dc,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	const RECT & rcVisibleRange,
	DWORD dwAreaFlags,
	DWORD dwHelperPaintFlags
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcButton) )
		return;
	nVisibleColNo;
	nVisibleRowNo;
	nColNo;
	nRowNo;
	nColType;
	nRowType;
	rcCellExtra;
	rcCell;
	rcVisibleRange;
	dwAreaFlags;
	dwHelperPaintFlags;
	if( !bEnabled )
		bPressed = false;
COLORREF clrFace =
		wndGrid.OnSiwGetSysColor( COLOR_3DFACE );
COLORREF clrLight = 
		wndGrid.OnSiwGetSysColor( COLOR_3DHIGHLIGHT );
COLORREF clrShadow = 
		wndGrid.OnSiwGetSysColor( COLOR_3DSHADOW );
COLORREF clrDkShadow = 
		wndGrid.OnSiwGetSysColor( COLOR_3DDKSHADOW );
COLORREF clrOuterLT =
		bPressed ? clrDkShadow : clrLight;
COLORREF clrInnerLT =
		bPressed ? clrShadow : clrFace;
COLORREF clrOuterRB =
		bPressed ? clrFace : clrDkShadow;
COLORREF clrInnerRB =
		bPressed ? clrLight : clrShadow;
CRect rc( rcButton );
	dc.FillSolidRect( &rc, clrFace );
	rc.DeflateRect( 2, 2 );
	if( bPressed )
		rc.OffsetRect( 0, 1 );
CExtPaintManager::glyph_t * pGlyph = NULL;
	if( nButtonType == INT(__EBTT_DROPDOWN) )
		pGlyph = 
			CExtPaintManager::g_DockingCaptionGlyphs[
				(INT) CExtPaintManager::__DCBT_ARROW_DOWN
				];
	else if( nButtonType == INT(__EBTT_ELLIPSIS) )
		pGlyph = 
			CExtPaintManager::g_DockingCaptionGlyphs[
				(INT) CExtPaintManager::__DCBT_ELLIPSIS
				];
	else if( nButtonType == INT(__EBTT_UPDOWN_UP) )
		pGlyph = 
			CExtPaintManager::g_DockingCaptionGlyphs[
				(INT) CExtPaintManager::__DCBT_ARROW_SMALL_UP
				];
	else if( nButtonType == INT(__EBTT_UPDOWN_DOWN) )
		pGlyph = 
			CExtPaintManager::g_DockingCaptionGlyphs[
				(INT) CExtPaintManager::__DCBT_ARROW_SMALL_DOWN
				];
	if( pGlyph != NULL )
	{
		COLORREF ColorValues[2] =
		{
			RGB(0,0,0),
			RGB(0,0,0),
		};
		ColorValues[1] = bEnabled
			? wndGrid.OnSiwGetSysColor( COLOR_BTNTEXT )
			: clrLight;
		if( ! bEnabled )
			rc.OffsetRect( 1, 1 );
		g_PaintManager->PaintGlyphCentered(
			dc,
			rc,
			*pGlyph,
			ColorValues
			);
		if( ! bEnabled )
		{
			rc.OffsetRect( -1, -1 );
			ColorValues[1] = clrShadow;
			g_PaintManager->PaintGlyphCentered(
				dc,
				rc,
				*pGlyph,
				ColorValues
				);
		} // if( ! bEnabled )
	} // if( pGlyph != NULL )
	rc = rcButton;
	dc.Draw3dRect( &rc, clrOuterLT, clrOuterRB );
	rc.DeflateRect( 1, 1 );
	dc.Draw3dRect( &rc, clrInnerLT, clrInnerRB );
}

void CExtGridCell::OnPaintIcon(
	const CExtCmdIcon * pIcon,
	const RECT & rcIcon,
	const CExtGridWnd & wndGrid,
	CDC & dc,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	const RECT & rcVisibleRange,
	DWORD dwAreaFlags,
	DWORD dwHelperPaintFlags
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( pIcon == NULL )
		return;
	ASSERT( ( ! pIcon->IsEmpty() ) );
	if( ! dc.RectVisible(&rcIcon) )
		return;
	wndGrid;
	nVisibleColNo;
	nVisibleRowNo;
	nColNo;
	nRowNo;
	nColType;
	nRowType;
	rcCell;
	rcVisibleRange;
	dwAreaFlags;
	dwHelperPaintFlags;
	rcCellExtra;
	pIcon->Paint( dc, rcIcon.left, rcIcon.top );
}

void CExtGridCell::OnPaintCheck(
	const RECT & rcCheck,
	const CExtGridWnd & wndGrid,
	CDC & dc,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	const RECT & rcVisibleRange,
	DWORD dwAreaFlags,
	DWORD dwHelperPaintFlags
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcCheck) )
		return;
	wndGrid;
	nVisibleColNo;
	nVisibleRowNo;
	nColNo;
	nRowNo;
	nColType;
	nRowType;
	rcCellExtra;
	rcCell;
	rcVisibleRange;
	dwAreaFlags;
	dwHelperPaintFlags;
UINT nState = DFCS_FLAT;
DWORD dwCellStyle = GetStyle();
	switch( (dwCellStyle&__EGCS_CHK_MASK) )
	{
	case __EGCS_CHK_NONE:
		return;
	case __EGCS_CHK_CHECK:
		nState |= DFCS_BUTTONCHECK;
	break;
	case __EGCS_CHK_RADIO:
		nState |= DFCS_BUTTONRADIO;
	break;
	case __EGCS_CHK_INDETERMINATE:
		nState |= DFCS_BUTTONCHECK|DFCS_BUTTON3STATE;
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	}; // switch( (dwCellStyle&__EGCS_CHK_MASK) )
	if( (dwCellStyle&__EGCS_CHECKED) != 0 )
		nState |= DFCS_CHECKED;
	dc.DrawFrameControl(
		(RECT *)&rcCheck,
		DFC_BUTTON,
		nState
		);
}

CSize CExtGridCell::OnCalcCheckSize() const
{
	ASSERT_VALID( this );
DWORD dwCellStyle = GetStyle();
	if( (dwCellStyle&__EGCS_CHK_MASK) == __EGCS_CHK_NONE )
		return CSize( 0, 0 );
	return CSize( 13, 13 );
}

void CExtGridCell::OnFormatHeaderNumberString(
	CExtSafeString & strText,
	const CExtGridWnd & wndGrid,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	DWORD dwAreaFlags
	) const
{
	ASSERT_VALID( this );
	ASSERT(
			(nColType != 0 && nRowType == 0)
		||	(nRowType != 0 && nColType == 0)
		);
	wndGrid;
	nVisibleColNo;
	nVisibleRowNo;
	nColType;
	nRowType;
	dwAreaFlags;
LONG nNumber = ( nColType == 0 ) ? nColNo : nRowNo;
	ASSERT( nNumber >= 0L );
	nNumber ++; // one based number
	strText.Format( _T("%ld"), nNumber );
}

COLORREF CExtGridCell::OnQueryTextColor(
	const CExtGridWnd & wndGrid,
	CDC & dc,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	DWORD dwAreaFlags,
	DWORD dwHelperPaintFlags
	) const
{
	ASSERT_VALID( this );
	dc;
	nVisibleColNo;
	nVisibleRowNo;
	nColType;
	nRowType;
COLORREF clrText = COLORREF(-1L); // use current DC text color
bool bFocusedControl = ( (dwHelperPaintFlags&__EGCPF_FOCUSED_CONTROL) != 0 ) ? true : false;
	if( bFocusedControl )
	{
		bool bHighlightPressing = ( (dwHelperPaintFlags&(__EGCPF_HIGHLIGHTED_BY_PRESSED_COLUMN|__EGCPF_HIGHLIGHTED_BY_PRESSED_ROW)) != 0 ) ? true : false;
		if( bHighlightPressing )
			clrText = wndGrid.OnSiwGetSysColor( COLOR_HIGHLIGHTTEXT );
		else
		{
			if( ( dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0 )
			{
				bool bHoverByColumn = ( (dwHelperPaintFlags&__EGCPF_HOVER_BY_COLUMN) != 0 ) ? true : false;
				bool bHoverByRow = ( (dwHelperPaintFlags&__EGCPF_HOVER_BY_ROW) != 0 ) ? true : false;
				bool bHighlightedBySelectedColumn = ( (dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_SELECTED_COLUMN) != 0 ) ? true : false;
				bool bHighlightedBySelectedRow = ( (dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_SELECTED_ROW) != 0 ) ? true : false;
				bool bHighlightedByFocusedColumn = ( (dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_FOCUSED_COLUMN) != 0 ) ? true : false;
				bool bHighlightedByFocusedRow = ( (dwHelperPaintFlags&__EGCPF_HIGHLIGHTED_BY_FOCUSED_ROW) != 0 ) ? true : false;
				if(		bHoverByColumn
					||	bHoverByRow
					||	bHighlightedBySelectedColumn
					||	bHighlightedBySelectedRow
					||	bHighlightedByFocusedColumn
					||	bHighlightedByFocusedRow
					)
					clrText = wndGrid.OnSiwGetSysColor( COLOR_HIGHLIGHTTEXT );
			} // if( ( dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0 )
			else
			{
				if( wndGrid.SelectionGetForCell( nColNo, nRowNo ) )
					clrText = wndGrid.OnSiwGetSysColor( COLOR_HIGHLIGHTTEXT );
			} // else from if( ( dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0 )
		} // else from if( bHighlightPressing )
	} // if( bFocusedControl )
	return clrText;
}

UINT CExtGridCell::OnQueryDrawTextFlags(
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	DWORD dwAreaFlags,
	bool bIncludeHorizontalFlags, // = true
	bool bIncludeVerticalFlags, // = true
	bool bIncludeOtherFlags // = true
	) const
{
	ASSERT_VALID( this );
	nVisibleColNo;
	nVisibleRowNo;
	nColNo;
	nRowNo;
	nColType;
	nRowType;
	dwAreaFlags;
UINT nFlags = bIncludeOtherFlags ? (DT_SINGLELINE|DT_NOPREFIX) : 0;
DWORD dwCellStyle = GetStyle();
	if(		bIncludeOtherFlags
		&&	(dwCellStyle&__EGCS_TEXT_ELLIPSIS) != 0
		)
		nFlags |= DT_END_ELLIPSIS;
	if( bIncludeHorizontalFlags )
	{
		switch( (dwCellStyle&__EGCS_TA_HORZ_MASK) )
		{
		case __EGCS_TA_HORZ_LEFT:
			nFlags |= DT_LEFT;
		break;
		case __EGCS_TA_HORZ_RIGHT:
			nFlags |= DT_RIGHT;
		break;
		case __EGCS_TA_HORZ_CENTER:
			nFlags |= DT_CENTER;
		break;
		case __EGCS_TA_HORZ_BY_TYPE:
			nFlags |= DT_LEFT;
		break;
#ifdef _DEBUG
		default:
			ASSERT( FALSE );
		break;
#endif // _DEBUG
		} // switch( (dwCellStyle&__EGCS_TA_HORZ_MASK) )
	} // if( bIncludeHorizontalFlags )
	if( bIncludeVerticalFlags )
	{
		switch( (dwCellStyle&__EGCS_TA_VERT_MASK) )
		{
		case __EGCS_TA_VERT_CENTER:
			nFlags |= DT_VCENTER;
		break;
		case __EGCS_TA_VERT_TOP:
			nFlags |= DT_TOP;
		break;
		case __EGCS_TA_VERT_BOTTOM:
			nFlags |= DT_BOTTOM;
		break;
		case __EGCS_TA_VERT_BY_TYPE:
			nFlags |= DT_VCENTER;
		break;
#ifdef _DEBUG
		default:
			ASSERT( FALSE );
		break;
#endif // _DEBUG
		} // switch( (dwCellStyle&__EGCS_TA_VERT_MASK) )
	} // if( bIncludeVerticalFlags )
	return nFlags;
}

void CExtGridCell::OnHitTestInfoAdjust(
	const CExtGridWnd & wndGrid,
	CExtGridHitTestInfo & htInfo,
	bool bComputeOuterDropAfterState
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( ! htInfo.IsHoverEmpty() );
	ASSERT( htInfo.IsValidRect( false ) );
	bComputeOuterDropAfterState;

bool bHoverByColumn = false, bHoverByRow = false,
		bHighlightedBySelectedColumn = false, bHighlightedBySelectedRow = false,
		bHighlightedByFocusedColumn = false, bHighlightedByFocusedRow = false;
CPoint ptFocus = wndGrid.FocusGet();
	if( ( htInfo.m_dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0 )
	{
		// compute hover flags
		if( wndGrid.HoverEventsOuterGet() && wndGrid.HoverHighlightCellOuterGet() )
		{
			bool bOuterCellAtLeftRight;
			if( htInfo.IsHoverOuter( &bOuterCellAtLeftRight ) )
			{
				if(			bOuterCellAtLeftRight
						&&	(htInfo.m_dwAreaFlags &__EGBWA_OUTER_H) != 0
						//&&	nRowNo == htInfo.m_nRowNo
					)
					bHoverByColumn = bHoverByRow = true;
				else if(	( ! bOuterCellAtLeftRight )
						&&	(htInfo.m_dwAreaFlags &__EGBWA_OUTER_V) != 0
						//&&	nColNo == htInfo.m_nColNo
					)
					bHoverByColumn = bHoverByRow = true;
			} // if( htInfo.IsHoverOuter( &bOuterCellAtLeftRight ) )
		} // if( wndGrid.HoverEventsOuterGet() && wndGrid.HoverHighlightCellOuterGet() )
		if(		( ! bHoverByColumn )
			&&	( ! bHoverByRow )
			&&	wndGrid.HoverEventsInnerGet()
			&&	htInfo.IsHoverInner()
			)
		{
				if(			(htInfo.m_dwAreaFlags &__EGBWA_OUTER_H) != 0
						//&&	nRowNo == htInfo.m_nRowNo
						&&  wndGrid.HoverHighlightRowOuterGet()
					)
					bHoverByRow = true;
				else if(	(htInfo.m_dwAreaFlags &__EGBWA_OUTER_V) != 0
						//&&	nColNo == htInfo.m_nColNo
						&&  wndGrid.HoverHighlightColumnOuterGet()
					)
					bHoverByColumn = true;
		} // if( ( ! bHoverByColumn ) ....
		// compute outer column selection highlight flags
		if(		(htInfo.m_dwAreaFlags &__EGBWA_OUTER_V) != 0
			&&	wndGrid.SelectionHighlightColumnOuterGet()
			&&	wndGrid.SelectionGetForCell( htInfo.m_nColNo, -1 )
			)
			bHighlightedBySelectedColumn = true;
		// compute outer row selection highlight flags
		if(		(htInfo.m_dwAreaFlags &__EGBWA_OUTER_H) != 0
			&&	wndGrid.SelectionHighlightRowOuterGet()
			&&	wndGrid.SelectionGetForCell( -1, htInfo.m_nRowNo )
			)
			bHighlightedBySelectedRow = true;
		// compute outer column focus highlight flags
		if(		(htInfo.m_dwAreaFlags &__EGBWA_OUTER_V) != 0
			&&	ptFocus.x >= 0
			&&	wndGrid.FocusHighlightColumnOuterGet()
			&&	ptFocus.x == htInfo.m_nColNo
			)
			bHighlightedByFocusedColumn = true;
		// compute outer row focus highlight flags
		if(		(htInfo.m_dwAreaFlags &__EGBWA_OUTER_H) != 0
			&&	ptFocus.y >= 0
			&&	wndGrid.FocusHighlightRowOuterGet()
			&&	ptFocus.y == htInfo.m_nRowNo
			)
			bHighlightedByFocusedRow = true;
	} // if( ( htInfo.m_dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0 )
	else
	{
		// compute hover flags
		if(		wndGrid.HoverEventsInnerGet()
			&&	wndGrid.HoverHighlightCellInnerGet()
			&&	htInfo.IsHoverInner()
			//&&	(	nColNo == htInfo.m_nColNo
			//	||	nRowNo == htInfo.m_nRowNo
			//	)
			)
		{
			//if(			nColNo == htInfo.m_nColNo
			//		&&	nRowNo == htInfo.m_nRowNo
			//		)
					bHoverByColumn = bHoverByRow = true;
			//else if(	nColNo == htInfo.m_nColNo
			//		&&	HoverHighlightColumnInnerGet()
			//		)
			//		bHoverByColumn = true;
			//else if(	nRowNo == htInfo.m_nRowNo
			//		&&	HoverHighlightRowInnerGet()
			//		)
			//		bHoverByRow = true;
		} // if( HoverEventsInnerGet() ...
		if(		( ! bHoverByColumn )
			&&	( ! bHoverByRow )
			&&	wndGrid.HoverEventsOuterGet()
			&&	wndGrid.HoverHighlightCellOuterGet()
			)
		{
			bool bOuterCellAtLeftRight;
			if( htInfo.IsHoverOuter( &bOuterCellAtLeftRight ) )
			{
				if( bOuterCellAtLeftRight )
				{
					if(		wndGrid.HoverHighlightRowInnerGet()
						//&&  nRowNo == htInfo.m_nRowNo
						)
						bHoverByRow = true;
				} // if( bOuterCellAtLeftRight )
				else
				{
					if(		wndGrid.HoverHighlightColumnInnerGet()
						//&&  nColNo == htInfo.m_nColNo
						)
						bHoverByColumn = true;
				} // else from if( bOuterCellAtLeftRight )
			} // if( htInfo.IsHoverOuter( &bOuterCellAtLeftRight ) )
		} // if( ( ! bHoverByColumn )
		// compute inner column selection highlight flags
		if(		wndGrid.SelectionHighlightColumnInnerGet()
			&&	wndGrid.SelectionGetForCell( htInfo.m_nColNo, -1 )
			)
			bHighlightedBySelectedColumn = true;
		// compute inner row selection highlight flags
		if(		wndGrid.SelectionHighlightRowInnerGet()
			&&	wndGrid.SelectionGetForCell( -1, htInfo.m_nRowNo )
			)
			bHighlightedBySelectedRow = true;
		// compute inner column focus highlight flags
		if(		wndGrid.FocusHighlightColumnInnerGet()
			&&	ptFocus.x == htInfo.m_nColNo
			)
			bHighlightedByFocusedColumn = true;
		// compute inner row focus highlight flags
		if(		wndGrid.FocusHighlightRowInnerGet()
			&&	ptFocus.y == htInfo.m_nRowNo
			)
			bHighlightedByFocusedRow = true;
	} // else from if( ( htInfo.m_dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0 )
DWORD dwHelperPaintFlags = 0L;
	if( bHoverByColumn )
		dwHelperPaintFlags |= __EGCPF_HOVER_BY_COLUMN;
	if( bHoverByRow )
		dwHelperPaintFlags |= __EGCPF_HOVER_BY_ROW;
	if( bHighlightedBySelectedColumn )
		dwHelperPaintFlags |= __EGCPF_HIGHLIGHTED_BY_SELECTED_COLUMN;
	if( bHighlightedBySelectedRow )
		dwHelperPaintFlags |= __EGCPF_HIGHLIGHTED_BY_SELECTED_ROW;
	if( bHighlightedByFocusedColumn )
		dwHelperPaintFlags |= __EGCPF_HIGHLIGHTED_BY_FOCUSED_COLUMN;
	if( bHighlightedByFocusedRow )
		dwHelperPaintFlags |= __EGCPF_HIGHLIGHTED_BY_FOCUSED_ROW;
bool bFocusedControl = wndGrid.OnSiwQueryFocusedControlState();
	if( bFocusedControl )
		dwHelperPaintFlags |= __EGCPF_FOCUSED_CONTROL;

CClientDC dc( (CWnd *)&wndGrid );
CRect	rcCellRest( htInfo.m_rcItem ),
		rcCellText,
		rcFocusArrow,
		rcIcon,
		rcCheck,
		rcSortArrow,
		rcUpdownButtonTop,
		rcUpdownButtonBottom,
		rcDropdownButton,
		rcEllipsisButton;
INT nColType = htInfo.GetInnerOuterTypeOfColumn();
INT nRowType = htInfo.GetInnerOuterTypeOfRow();
CSize	sizeIcon = IconGetSize(),
		sizeFocusArrow =
			OnCalcFocusArrowSize(
				wndGrid,
				dc,
				htInfo.m_nVisibleColNo,
				htInfo.m_nVisibleRowNo,
				htInfo.m_nColNo,
				htInfo.m_nRowNo,
				nColType,
				nRowType,
				htInfo.m_dwAreaFlags,
				dwHelperPaintFlags
				),
		sizeSortArrow =
			OnCalcSortArrowSize(
				wndGrid,
				dc,
				htInfo.m_nVisibleColNo,
				htInfo.m_nVisibleRowNo,
				htInfo.m_nColNo,
				htInfo.m_nRowNo,
				nColType,
				nRowType,
				htInfo.m_dwAreaFlags,
				dwHelperPaintFlags
				);
CSize sizeCheck = OnCalcCheckSize();
	ASSERT( sizeIcon.cx >= 0 && sizeIcon.cy >= 0 );
	ASSERT( sizeCheck.cx >= 0 && sizeCheck.cy >= 0 );
INT		nExtentUpdownButton =
			OnCalcButtonExtent(
				INT(__EBTT_UPDOWN_UP),
				wndGrid,
				dc,
				htInfo.m_nVisibleColNo,
				htInfo.m_nVisibleRowNo,
				htInfo.m_nColNo,
				htInfo.m_nRowNo,
				nColType,
				nRowType,
				htInfo.m_dwAreaFlags,
				dwHelperPaintFlags
				),
		nExtentDropdownButton =
			OnCalcButtonExtent(
				INT(__EBTT_DROPDOWN),
				wndGrid,
				dc,
				htInfo.m_nVisibleColNo,
				htInfo.m_nVisibleRowNo,
				htInfo.m_nColNo,
				htInfo.m_nRowNo,
				nColType,
				nRowType,
				htInfo.m_dwAreaFlags,
				dwHelperPaintFlags
				),
		nExtentEllipsisButton =
			OnCalcButtonExtent(
				INT(__EBTT_ELLIPSIS),
				wndGrid,
				dc,
				htInfo.m_nVisibleColNo,
				htInfo.m_nVisibleRowNo,
				htInfo.m_nColNo,
				htInfo.m_nRowNo,
				nColType,
				nRowType,
				htInfo.m_dwAreaFlags,
				dwHelperPaintFlags
				),
		nFocusGlyphType = -1;
	ASSERT( nExtentUpdownButton >= 0 );
	ASSERT( nExtentDropdownButton >= 0 );
	ASSERT( nExtentEllipsisButton >= 0 );
DWORD dwCellStyle = GetStyle();
DWORD dwCellStyleEx = GetStyleEx();
	OnCalcLayout(
		dwCellStyle,
		dwCellStyleEx,
		rcCellRest,
		rcCellText,
		rcFocusArrow,
		rcIcon,
		rcCheck,
		rcSortArrow,
		rcUpdownButtonTop,
		rcUpdownButtonBottom,
		rcDropdownButton,
		rcEllipsisButton,
		sizeFocusArrow,
		sizeIcon,
		sizeCheck,
		sizeSortArrow,
		nExtentUpdownButton,
		nExtentDropdownButton,
		nExtentEllipsisButton,
		nFocusGlyphType,
		wndGrid,
		dc,
		htInfo.m_nVisibleColNo,
		htInfo.m_nVisibleRowNo,
		htInfo.m_nColNo,
		htInfo.m_nRowNo,
		nColType,
		nRowType,
		htInfo.m_dwAreaFlags,
		dwHelperPaintFlags
		);
	htInfo.m_dwAreaFlags &= ~__EGBWA_CELL_MASK;
	htInfo.m_nButtonType = -1;
	if( rcEllipsisButton.PtInRect( htInfo.m_ptClient ) )
	{
		htInfo.m_dwAreaFlags |= __EGBWA_CELL_BUTTON;
		htInfo.m_nButtonType = INT(__EBTT_ELLIPSIS);
		htInfo.m_rcPart = rcEllipsisButton;
	} // if( rcEllipsisButton.PtInRect( htInfo.m_ptClient ) )
	else if( rcDropdownButton.PtInRect( htInfo.m_ptClient ) )
	{
		htInfo.m_dwAreaFlags |= __EGBWA_CELL_BUTTON;
		htInfo.m_nButtonType = INT(__EBTT_DROPDOWN);
		htInfo.m_rcPart = rcDropdownButton;
	} // if( rcDropdownButton.PtInRect( htInfo.m_ptClient ) )
	else if( rcUpdownButtonTop.PtInRect( htInfo.m_ptClient ) )
	{
		htInfo.m_dwAreaFlags |= __EGBWA_CELL_BUTTON;
		htInfo.m_nButtonType = INT(__EBTT_UPDOWN_UP);
		htInfo.m_rcPart = rcUpdownButtonTop;
	} // if( rcUpdownButtonTop.PtInRect( htInfo.m_ptClient ) )
	else if( rcUpdownButtonBottom.PtInRect( htInfo.m_ptClient ) )
	{
		htInfo.m_dwAreaFlags |= __EGBWA_CELL_BUTTON;
		htInfo.m_nButtonType = INT(__EBTT_UPDOWN_DOWN);
		htInfo.m_rcPart = rcUpdownButtonBottom;
	} // if( rcUpdownButtonBottom.PtInRect( htInfo.m_ptClient ) )
	else if( rcSortArrow.PtInRect( htInfo.m_ptClient ) )
	{
		htInfo.m_dwAreaFlags |= __EGBWA_CELL_SORT_ARROW;
		htInfo.m_rcPart = rcSortArrow;
	} // if( rcSortArrow.PtInRect( htInfo.m_ptClient ) )
	else if( rcCheck.PtInRect( htInfo.m_ptClient ) )
	{
		htInfo.m_dwAreaFlags |= __EGBWA_CELL_CHECKBOX;
		htInfo.m_rcPart = rcCheck;
	} // if( rcCheck.PtInRect( htInfo.m_ptClient ) )
	else if( rcIcon.PtInRect( htInfo.m_ptClient ) )
	{
		htInfo.m_dwAreaFlags |= __EGBWA_CELL_IMAGE;
		htInfo.m_rcPart = rcIcon;
	} // if( rcIcon.PtInRect( htInfo.m_ptClient ) )
	else if( rcFocusArrow.PtInRect( htInfo.m_ptClient ) )
	{
		htInfo.m_dwAreaFlags |= __EGBWA_CELL_FOCUS_ARROW;
		htInfo.m_rcPart = rcFocusArrow;
	} // if( rcFocusArrow.PtInRect( htInfo.m_ptClient ) )
	else if( rcCellText.PtInRect( htInfo.m_ptClient ) )
	{
		htInfo.m_dwAreaFlags |= __EGBWA_CELL_TEXT;
		htInfo.m_rcPart = rcCellText;
	} // if( rcCellText.PtInRect( htInfo.m_ptClient ) )
	else if( rcCellRest.PtInRect( htInfo.m_ptClient ) )
	{
		htInfo.m_dwAreaFlags |= __EGBWA_CELL_TEXT_AREA;
		htInfo.m_rcPart = rcCellRest;
	} // if( rcCellRest.PtInRect( htInfo.m_ptClient ) )
}

void CExtGridCell::OnHover(
	CExtGridWnd & wndGrid,
	const CExtGridHitTestInfo & htInfo,
	bool bHoverEnter,
	bool bHoverLeave
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( ! htInfo.IsHoverEmpty() );
	ASSERT( htInfo.IsValidRect() );
	ASSERT( ! (bHoverEnter && bHoverLeave) );
	wndGrid;
	htInfo;
	bHoverEnter;
	bHoverLeave;
}

bool CExtGridCell::OnClick(
	CExtGridWnd & wndGrid,
	const CExtGridHitTestInfo & htInfo,
	UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
	UINT nRepCnt, // 0 - button up, 1 - signle click, 2 - double click, 3 - post single click & begin editing
	UINT nFlags // mouse event flags
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( ! htInfo.IsHoverEmpty() );
	ASSERT( htInfo.IsValidRect() );
	ASSERT( nChar == VK_LBUTTON || nChar == VK_RBUTTON || nChar == VK_MBUTTON );
	ASSERT( 0 <= nRepCnt && nRepCnt <= 3 );
	htInfo;
	nFlags;
DWORD dwCellStyle = GetStyle();
	if( (dwCellStyle&__EGCS_READ_ONLY) != 0 )
		return false;
	if( nChar == VK_LBUTTON && nRepCnt == 1 )
	{ // if single left button click
		if( (nFlags&(MK_SHIFT|MK_CONTROL)) == 0 )
		{ // if neither Shift nor Ctrl keys was pressed
			if( (htInfo.m_dwAreaFlags&__EGBWA_CELL_BUTTON) != 0 )
			{
				CExtGridHitTestInfo htInfoTrack( htInfo );
				ASSERT( htInfoTrack.m_nButtonType >= 0 );
				return
					wndGrid.OnGridTrackCellButton(
						this,
						htInfoTrack
						);
			} // if( (htInfo.m_dwAreaFlags&__EGBWA_CELL_BUTTON) != 0 )
			if( (htInfo.m_dwAreaFlags&__EGBWA_CELL_CHECKBOX) != 0 )
			{
				DWORD dwCellStyle = GetStyle();
				if( (dwCellStyle&__EGCS_CHECKED) != 0 )
					OnSetCheck( false, wndGrid, htInfo.m_rcExtra, htInfo.m_rcItem );
				else
					OnSetCheck( true, wndGrid, htInfo.m_rcExtra, htInfo.m_rcItem );
				return true;
			} // if( (htInfo.m_dwAreaFlags&__EGBWA_CELL_CHECKBOX) != 0 )
		} // if neither Shift nor Ctrl keys was pressed
		if(		(nFlags&MK_CONTROL) == 0
			&&	(htInfo.m_dwAreaFlags&__EGBWA_OUTER_CELLS) != 0
			&&	(htInfo.m_dwAreaFlags&__EGBWA_NEAR_CELL_BORDER_SIDE_MASK) == 0
			&&	DataProviderGet() != NULL
			)
		{
			DWORD dwBseStyle = wndGrid.BseGetStyle();
			DWORD dwBseStyleEx = wndGrid.BseGetStyleEx();
			DWORD dwCellStyleEx = GetStyleEx();
			bool bAllowStartDragEvent =
				(	(dwBseStyleEx&__EGBS_BSE_EX_DRAG_START_ANY_MASK) != 0
				&&	(dwCellStyleEx&__EGCS_EX_DISABLE_START_DRAG) == 0
				) ? true : false;
			if( bAllowStartDragEvent )
			{
				int nColType = htInfo.GetInnerOuterTypeOfColumn();
				int nRowType = htInfo.GetInnerOuterTypeOfRow();
				if( nColType != 0 && nRowType == 0 )
				{
					if( (dwBseStyleEx&__EGBS_BSE_EX_DRAG_START_ROWS) == 0 )
						bAllowStartDragEvent = false;
				} // if( nColType != 0 && nRowType == 0 )
				else if( nColType == 0 && nRowType != 0 )
				{
					if( (dwBseStyleEx&__EGBS_BSE_EX_DRAG_START_COLUMNS) == 0 )
						bAllowStartDragEvent = false;
				} // else if( nColType == 0 && nRowType != 0 )
				else
					bAllowStartDragEvent = false;
			} // if( bAllowStartDragEvent )
			if(		(	(dwBseStyle&__EGWS_BSE_SORT_ANY_MASK) != 0
					&&	(dwCellStyleEx&__EGCS_EX_DISABLE_SORT) == 0
					)
				||	bAllowStartDragEvent
				)
			{ 
				ASSERT_VALID( DataProviderGet() );
				bool bAllowSort = false, bUpdateMultipleSort = false,
					bSortColumns = false;
				if( (dwCellStyleEx&__EGCS_EX_DISABLE_SORT) == 0 )
				{
					if( (htInfo.m_dwAreaFlags&__EGBWA_OUTER_TOP) != 0 )
					{
						if( (dwBseStyle&__EGWS_BSE_SORT_COLUMNS_T) != 0 )
						{
							bAllowSort = true;
							if(		(nFlags&MK_SHIFT) != 0
								&&	(dwBseStyle&__EGWS_BSE_SORT_COLUMNS_ALLOW_MULTIPLE) != 0
								)
								bUpdateMultipleSort = true;
						}
					} // if( (htInfo.m_dwAreaFlags&__EGBWA_OUTER_TOP) != 0 )
					else if( (htInfo.m_dwAreaFlags&__EGBWA_OUTER_BOTTOM) != 0 )
					{
						if( (dwBseStyle&__EGWS_BSE_SORT_COLUMNS_B) != 0 )
						{
							bAllowSort = true;
							if(		(nFlags&MK_SHIFT) != 0
								&&	(dwBseStyle&__EGWS_BSE_SORT_COLUMNS_ALLOW_MULTIPLE) != 0
								)
								bUpdateMultipleSort = true;
						}
					} // else if( (htInfo.m_dwAreaFlags&__EGBWA_OUTER_BOTTOM) != 0 )
					else if( (htInfo.m_dwAreaFlags&__EGBWA_OUTER_LEFT) != 0 )
					{
						if( (dwBseStyle&__EGWS_BSE_SORT_ROWS_L) != 0 )
						{
							bSortColumns = bAllowSort = true;
							if(		(nFlags&MK_SHIFT) != 0
								&&	(dwBseStyle&__EGWS_BSE_SORT_ROWS_ALLOW_MULTIPLE) != 0
								)
								bUpdateMultipleSort = true;
						}
					} // else if( (htInfo.m_dwAreaFlags&__EGBWA_OUTER_LEFT) != 0 )
					else if( (htInfo.m_dwAreaFlags&__EGBWA_OUTER_RIGHT) != 0 )
					{
						if( (dwBseStyle&__EGWS_BSE_SORT_ROWS_R) != 0 )
						{
							bSortColumns = bAllowSort = true;
							if(		(nFlags&MK_SHIFT) != 0
								&&	(dwBseStyle&__EGWS_BSE_SORT_ROWS_ALLOW_MULTIPLE) != 0
								)
								bUpdateMultipleSort = true;
						}
					} // else if( (htInfo.m_dwAreaFlags&__EGBWA_OUTER_RIGHT) != 0 )
				} // if( (dwCellStyleEx&__EGCS_EX_DISABLE_SORT) == 0 )
				if( bAllowSort || bAllowStartDragEvent )
				{
					CExtGridDataSortOrder _gdsoUpdate;
					if( bAllowSort )
					{
						CExtGridDataSortOrder::ITEM_INFO _soii(
							bSortColumns ? htInfo.m_nRowNo : htInfo.m_nColNo
							);
						_gdsoUpdate.m_arrItems.Add( _soii );
					} // if( bAllowSort )
					wndGrid.OnGridTrackCellMousePressing(
						this,
						htInfo,
						nChar,
						nFlags,
						bAllowStartDragEvent,
						bAllowSort ? (&_gdsoUpdate) : NULL,
						bUpdateMultipleSort,
						bSortColumns
						);
					return true;
				} // if( bAllowSort || bAllowStartDragEvent )
			}
		}
	} // if single left button click
	return false;
}

void CExtGridCell::OnSetCheck(
	bool bCheck,
	CExtGridWnd & wndGrid,
	const RECT & rcCellExtra,
	const RECT & rcCell
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	rcCell;
DWORD dwCellStyle = GetStyle();
	if( (dwCellStyle&__EGCS_READ_ONLY) != 0 )
		return;
bool bRedraw = false;
	if( (dwCellStyle&__EGCS_CHECKED) != 0 )
	{
		if( ! bCheck )
		{
			ModifyStyle( 0, __EGCS_CHECKED );
			bRedraw = true;
		} // if( ! bCheck )
	} // if( (dwCellStyle&__EGCS_CHECKED) != 0 )
	else
	{
		if( bCheck )
		{
			ModifyStyle( __EGCS_CHECKED, 0 );
			bRedraw = true;
		} // if( bCheck )
	} // else from if( (dwCellStyle&__EGCS_CHECKED) != 0 )
	if( bRedraw )
		wndGrid.InvalidateRect( &rcCellExtra );
}

#define __EXT_GRID_BUTTON_DEF_TIMER_ELAPSE_VALUE 300

bool CExtGridCell::OnQueryButtonInfo(
	INT nButtonType,
	bool * p_bEnabled,
	bool * p_bPressed, // = NULL
	bool * p_bStayPressed, // = NULL
	UINT * p_nTimerElapseValue // = NULL
	) const
{
	ASSERT_VALID( this );
	if( p_bEnabled != NULL )
		(*p_bEnabled) = false;
	if( p_bPressed != NULL )
		(*p_bPressed) = false;
	if( p_bStayPressed != NULL )
		(*p_bStayPressed) = false;
	if( p_nTimerElapseValue != NULL )
		(*p_nTimerElapseValue) = 0;
DWORD dwCellStyle = GetStyle();
	switch( nButtonType )
	{
	case __EBTT_ELLIPSIS:
		if( (dwCellStyle&__EGCS_BUTTON_ELLIPSIS) == 0 )
			return false;
		if( p_bStayPressed != NULL )
			(*p_bStayPressed) = true;
		if(		p_bEnabled != NULL
			&&	(dwCellStyle&__EGCS_READ_ONLY) == 0
			)
			(*p_bEnabled) = true;
		if(		p_bPressed != NULL
			&&	(dwCellStyle&__EGCS_PRESSED_ELLIPSIS) != 0
			)
			(*p_bPressed) = true;
		return true;
	case __EBTT_DROPDOWN:
		if( (dwCellStyle&__EGCS_BUTTON_DROPDOWN) == 0 )
			return false;
		if(		p_bEnabled != NULL
			&&	(dwCellStyle&__EGCS_READ_ONLY) == 0
			)
			(*p_bEnabled) = true;
		if(		p_bPressed != NULL
			&&	(dwCellStyle&__EGCS_PRESSED_DROPDOWN) != 0
			)
			(*p_bPressed) = true;
		return true;
	case __EBTT_UPDOWN_UP:
		if( (dwCellStyle&__EGCS_BUTTON_UPDOWN) == 0 )
			return false;
		if( p_bStayPressed != NULL )
			(*p_bStayPressed) = true;
		if( p_nTimerElapseValue != NULL )
			(*p_nTimerElapseValue) = __EXT_GRID_BUTTON_DEF_TIMER_ELAPSE_VALUE;
		if(		p_bEnabled != NULL
			&&	(dwCellStyle&__EGCS_READ_ONLY) == 0
			)
			(*p_bEnabled) = OnQueryEnabledIncrement();
		if(		p_bPressed != NULL
			&&	(dwCellStyle&__EGCS_PRESSED_UP) != 0
			)
			(*p_bPressed) = true;
		return true;
	case __EBTT_UPDOWN_DOWN:
		if( (dwCellStyle&__EGCS_BUTTON_UPDOWN) == 0 )
			return false;
		if( p_bStayPressed != NULL )
			(*p_bStayPressed) = true;
		if( p_nTimerElapseValue != NULL )
			(*p_nTimerElapseValue) = __EXT_GRID_BUTTON_DEF_TIMER_ELAPSE_VALUE;
		if(		p_bEnabled != NULL
			&&	(dwCellStyle&__EGCS_READ_ONLY) == 0
			)
			(*p_bEnabled) = OnQueryEnabledDecrement();
		if(		p_bPressed != NULL
			&&	(dwCellStyle&__EGCS_PRESSED_DOWN) != 0
			)
			(*p_bPressed) = true;
		return true;
	} // switch( nButtonType )
	return false;
}

void CExtGridCell::OnChangeButtonPressedState(
	INT nButtonType,
	bool bPressed
	)
{
	ASSERT_VALID( this );
DWORD dwCellStyle = GetStyle();
	switch( nButtonType )
	{
	case __EBTT_ELLIPSIS:
		if( (dwCellStyle&__EGCS_BUTTON_ELLIPSIS) == 0 )
			return;
		ModifyStyle(
			bPressed ? __EGCS_PRESSED_ELLIPSIS : 0,
			bPressed ? 0 : __EGCS_PRESSED_ELLIPSIS
			);
		return;
	case __EBTT_DROPDOWN:
		if( (dwCellStyle&__EGCS_BUTTON_DROPDOWN) == 0 )
			return;
		ModifyStyle(
			bPressed ? __EGCS_PRESSED_DROPDOWN : 0,
			bPressed ? 0 : __EGCS_PRESSED_DROPDOWN
			);
		return;
	case __EBTT_UPDOWN_UP:
		if( (dwCellStyle&__EGCS_BUTTON_UPDOWN) == 0 )
			return;
		ModifyStyle(
			bPressed ? __EGCS_PRESSED_UP : 0,
			bPressed ? 0 : __EGCS_PRESSED_UP
			);
		return;
	case __EBTT_UPDOWN_DOWN:
		if( (dwCellStyle&__EGCS_BUTTON_UPDOWN) == 0 )
			return;
		ModifyStyle(
			bPressed ? __EGCS_PRESSED_DOWN : 0,
			bPressed ? 0 : __EGCS_PRESSED_DOWN
			);
		return;
	} // switch( nButtonType )
}

void CExtGridCell::OnButtonPressing(
	CExtGridWnd & wndGrid,
	INT nButtonType,
	bool bMouseInsideButton,
	ULONG nPressedTimerCounter,
	const RECT & rcButton,
	const RECT & rcCellExtra,
	const RECT & rcCell
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	bMouseInsideButton;
	nPressedTimerCounter;
	rcButton;
	rcCell;
//	TRACE3(
//		"CExtGridCell::OnButtonPressing(button-type=%d,mouse-state=%s,counter=%lu)\n",
//		nButtonType,
//		bMouseInsideButton ? _T("inside") : _T("outside"),
//		nPressedTimerCounter
//		);
	if( ! bMouseInsideButton )
		return;
bool bRedrawCell = false;
	switch( nButtonType )
	{
	case __EBTT_UPDOWN_UP:
		if(		IsButtonEnabled(__EBTT_UPDOWN_UP)
			&&	OnQueryEnabledIncrement()
			)
		{
			if( OnValueIncrement() )
				bRedrawCell = true;
		}
	break;
	case __EBTT_UPDOWN_DOWN:
		if(		IsButtonEnabled(__EBTT_UPDOWN_DOWN)
			&&	OnQueryEnabledDecrement()
			)
		{
			if( OnValueDecrement() )
				bRedrawCell = true;
		}
	break;
	} // switch( nButtonType )
	if( bRedrawCell )
		wndGrid.InvalidateRect( &rcCellExtra );
}

void CExtGridCell::OnButtonPressed(
	CExtGridWnd & wndGrid,
	INT nButtonType,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
//	TRACE1(
//		"CExtGridCell::OnButtonPressed(button-type=%d)\n",
//		nButtonType
//		);
bool bRedrawCell = false;
bool bUpdateCell = false;
HWND hWndGrid = wndGrid.GetSafeHwnd();
	ASSERT( hWndGrid != NULL && ::IsWindow(hWndGrid) );
	switch( nButtonType )
	{
	case __EBTT_UPDOWN_UP:
		if(		IsButtonEnabled(__EBTT_UPDOWN_UP)
			&&	OnQueryEnabledIncrement()
			)
		{
			if( OnValueIncrement() )
				bRedrawCell = true;
		}
	break;
	case __EBTT_UPDOWN_DOWN:
		if(		IsButtonEnabled(__EBTT_UPDOWN_DOWN)
			&&	OnQueryEnabledDecrement()
			)
		{
			if( OnValueDecrement() )
				bRedrawCell = true;
		}
	break;
	case __EBTT_DROPDOWN:
		{
			wndGrid.InvalidateRect( &rcCell );
			wndGrid.UpdateWindow();
			TrackCellStateInfo_t _tcsi(
				(*this),
				wndGrid,
				nButtonType,
				rcCellExtra,
				rcCell,
				nVisibleColNo,
				nVisibleRowNo,
				nColNo,
				nRowNo,
				nColType,
				nRowType
				);
			OnButtonPopupMenuTrack( _tcsi );
			bRedrawCell = true;
			bUpdateCell = true;
		}
	break;
	} // switch( nButtonType )
	if( ! ::IsWindow(hWndGrid) )
		return;
	if( bRedrawCell )
		wndGrid.InvalidateRect( &rcCellExtra );
	if( bUpdateCell )
		wndGrid.UpdateWindow();
}

bool CExtGridCell::OnQueryEnabledIncrement() const
{
	ASSERT_VALID( this );
	return false;
}

bool CExtGridCell::OnQueryEnabledDecrement() const
{
	ASSERT_VALID( this );
	return false;
}

bool CExtGridCell::OnValueIncrement()
{
	ASSERT_VALID( this );
	return false;
}

bool CExtGridCell::OnValueDecrement()
{
	ASSERT_VALID( this );
	return false;
}

bool CExtGridCell::OnKey(
	CExtGridWnd & wndGrid,
	bool bKeyDownEvent, // true - key-down event, false - key-up event
	UINT nChar, // virtual key code
	UINT nRepCnt, // key-down/key-up press count
	UINT nFlags // key-down/key-up event flags
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	nRepCnt;
	nFlags;
	if( bKeyDownEvent )
	{
		if( nChar == VK_SPACE )
		{
			CPoint ptFocus = wndGrid.FocusGet();
			ASSERT( ptFocus.x >= 0 && ptFocus.y >= 0 );
			CRect rcCell( 0, 0, 0, 0 ), rcCellExtra( 0, 0, 0, 0 );
			wndGrid.GridCellRectsGet(
				ptFocus.x,
				ptFocus.y,
				0,
				0,
				&rcCell,
				&rcCellExtra
				);
			DWORD dwCellStyle = GetStyle();
			if( (dwCellStyle&__EGCS_CHECKED) != 0 )
				OnSetCheck( false, wndGrid, rcCellExtra, rcCell );
			else
				OnSetCheck( true, wndGrid, rcCellExtra, rcCell );
			return true;
		} // if( nChar == VK_SPACE )
	} // if( bKeyDownEvent )
	return false;
}

bool CExtGridCell::OnSetCursor(
	CExtGridWnd & wndGrid,
	const CExtGridHitTestInfo & htInfo
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( ! htInfo.IsHoverEmpty() );
	ASSERT( htInfo.IsValidRect() );
	wndGrid;
	htInfo;
	return false;
}

bool CExtGridCell::OnQueryExtent(
	INT & nItemExtent,
	bool bGet,
	INT nExtentType // = 0 // -1 - min, 0 - curr, 1 - max
	)
{
	ASSERT( this != NULL );
	ASSERT_KINDOF( CExtGridCell, this );
	nItemExtent;
	bGet;
	nExtentType;
	return false;
}

bool CExtGridCell::ExtentGet(
	INT & nItemExtent,
	INT nExtentType // = 0 // -1 - min, 0 - curr, 1 - max
	) const
{
	ASSERT( this != NULL );
	ASSERT_KINDOF( CExtGridCell, this );
	return
		( const_cast < CExtGridCell * > ( this ) )
			-> OnQueryExtent( nItemExtent, true, nExtentType );
}

bool CExtGridCell::ExtentSet(
	INT nItemExtent,
	INT nExtentType // = 0 // -1 - min, 0 - curr, 1 - max
	)
{
	ASSERT( this != NULL );
	ASSERT_KINDOF( CExtGridCell, this );
	return OnQueryExtent( nItemExtent, false, nExtentType );
}

bool CExtGridCell::OnQueryExtraSpace(
	INT & nItemExtraSpace,
	bool bGet,
	bool bAfter // true - extra space after, false - after before
	)
{
	ASSERT( this != NULL );
	ASSERT_KINDOF( CExtGridCell, this );
	nItemExtraSpace;
	bGet;
	bAfter;
	return false;
}

bool CExtGridCell::ExtraSpaceGet(
	INT & nItemExtraSpace,
	bool bAfter // true - extra space after, false - after before
	) const
{
	ASSERT( this != NULL );
	ASSERT_KINDOF( CExtGridCell, this );
	return
		( const_cast < CExtGridCell * > ( this ) )
			-> OnQueryExtraSpace( nItemExtraSpace, true, bAfter );
}

bool CExtGridCell::ExtraSpaceSet(
	INT nItemExtraSpace,
	bool bAfter // true - extra space after, false - after before
	)
{
	ASSERT( this != NULL );
	ASSERT_KINDOF( CExtGridCell, this );
	return OnQueryExtraSpace( nItemExtraSpace, false, bAfter );
}

bool CExtGridCell::OnInitToolTip(
	CExtGridWnd & wndGrid,
	const CExtGridHitTestInfo & htInfo,
	CToolTipCtrl & wndToolTip,
	UINT nToolNo,
	const RECT & rcTool
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( ! htInfo.IsHoverEmpty() );
	ASSERT( htInfo.IsValidRect() );
	htInfo;
bool bRetVal = false;
LPCTSTR strTextBuffer = (LPCTSTR)GetTextBuffer();
	if( strTextBuffer != NULL )
	{
		if( _tcslen(strTextBuffer) >= 0 )
		{
			wndToolTip.AddTool(
				&wndGrid,
				strTextBuffer,
				&rcTool,
				nToolNo
				);
			bRetVal = true;
		} // if( _tcslen(strTextBuffer) >= 0 )
	} // if( strTextBuffer != NULL )
	else
	{
		CExtSafeString strText;
		TextGet( strText );
		if( strText.GetLength() > 0 )
		{
			wndToolTip.AddTool(
				&wndGrid,
				(LPCTSTR)strText,
				&rcTool,
				nToolNo
				);
			bRetVal = true;
		} // if( strText.GetLength() > 0 )
	} // else from if( strTextBuffer != NULL )
	if( ! bRetVal )
	{
		wndToolTip.DelTool( &wndGrid, nToolNo );
		CWnd::CancelToolTips();
	} // if( ! bRetVal )
	return bRetVal;
}

bool CExtGridCell::OnInitExpandWnd(
	CExtGridWnd & wndGrid,
	const CExtGridHitTestInfo & htInfo,
	CExtContentExpandWnd & wndContentExpand,
	const RECT & rcExpand,
	INT nSizeOfExpandShadow
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( ! htInfo.IsHoverEmpty() );
	ASSERT( htInfo.IsValidRect() );
	ASSERT( nSizeOfExpandShadow >= 0 );
	rcExpand;
	if( wndGrid.GetSafeInplaceActiveHwnd() != NULL )
		return false;
	if(		(htInfo.m_dwAreaFlags&__EGBWA_CELL_TEXT) == 0
		||	htInfo.m_rcPart.IsRectEmpty()
		)
		return false;
CRect rcExpandInit( 0, 0, 0, 0 );
	{ // block for CDC
		INT nColType = htInfo.GetInnerOuterTypeOfColumn();
		INT nRowType = htInfo.GetInnerOuterTypeOfRow();
		CClientDC dc( (CWnd*)&wndGrid );
		CSize sizeReqiredForText =
			OnMeasureTextSize(
				wndGrid,
				dc,
				htInfo.m_nVisibleColNo,
				htInfo.m_nVisibleRowNo,
				htInfo.m_nColNo,
				htInfo.m_nRowNo,
				nColType,
				nRowType,
				htInfo.m_dwAreaFlags
				);
		CSize sizeCellText = htInfo.m_rcPart.Size();
		UINT nDrawTextFlagsReal =
			OnQueryDrawTextFlags(
				htInfo.m_nVisibleColNo,
				htInfo.m_nVisibleRowNo,
				htInfo.m_nColNo,
				htInfo.m_nRowNo,
				nColType,
				nRowType,
				htInfo.m_dwAreaFlags
				);
		
		if( (nDrawTextFlagsReal&DT_RIGHT) != 0 )
		{
			rcExpandInit.SetRect(
				htInfo.m_rcPart.right - sizeReqiredForText.cx,
				htInfo.m_rcPart.top,
				htInfo.m_rcPart.right,
				htInfo.m_rcPart.top + sizeReqiredForText.cy
				);
		} // if( (nDrawTextFlagsReal&DT_RIGHT) != 0 )
		else
		{
			rcExpandInit.SetRect(
				htInfo.m_rcPart.left,
				htInfo.m_rcPart.top,
				htInfo.m_rcPart.left + sizeReqiredForText.cx,
				htInfo.m_rcPart.top + sizeReqiredForText.cy
				);
			if( (nDrawTextFlagsReal&DT_CENTER) != 0 )
			{
				rcExpandInit.OffsetRect(
					( htInfo.m_rcPart.Width() - rcExpandInit.Width() ) / 2,
					0
					);
			} // if( (nDrawTextFlagsReal&DT_CENTER) != 0 )
		} // else from if( (nDrawTextFlagsReal&DT_RIGHT) != 0 )
		if( (nDrawTextFlagsReal&DT_BOTTOM) != 0 )
		{
			rcExpandInit.OffsetRect(
				0,
				( htInfo.m_rcPart.Height() - rcExpandInit.Height() )
				);
		} // if( (nDrawTextFlagsReal&DT_BOTTOM) != 0 )
		else if( (nDrawTextFlagsReal&DT_VCENTER) != 0 )
		{
			rcExpandInit.OffsetRect(
				0,
				( htInfo.m_rcPart.Height() - rcExpandInit.Height() ) / 2
				);
		} // else if( (nDrawTextFlagsReal&DT_VCENTER) != 0 )
		if(		sizeCellText.cx >= sizeReqiredForText.cx
			&&	sizeCellText.cy >= sizeReqiredForText.cy
			)
		{
			CRect rcClient = wndGrid.OnSwGetClientRect();
			if(		rcExpandInit.bottom <= rcClient.bottom
				&&	rcExpandInit.top >= rcClient.top
				&&	rcExpandInit.right <= rcClient.right
				&&	rcExpandInit.left >= rcClient.left
				)
				return false;
		}
//		rcExpandInit.SetRect(
//			htInfo.m_rcPart.left,
//			rcExpand.top,
//			htInfo.m_rcPart.left + sizeReqiredForText.cx,
//			rcExpand.bottom
//			);
//		if( rcExpandInit.Height() < sizeReqiredForText.cy )
//			rcExpandInit.bottom = rcExpandInit.top + sizeReqiredForText.cy;
		rcExpandInit.InflateRect(
			__EXT_EXPANDED_TEXT_GAP_X,
			__EXT_EXPANDED_TEXT_GAP_Y
			);
		rcExpandInit.InflateRect(
			0,
			0,
			nSizeOfExpandShadow,
			nSizeOfExpandShadow
			);
		wndGrid.ClientToScreen( &rcExpandInit );
		rcExpandInit =
			CExtPaintManager::stat_AlignWndRectToMonitor(
				rcExpandInit,
				false,
				true
				);
		wndGrid.ScreenToClient( &rcExpandInit );
	} // block for CDC
	ASSERT( !rcExpandInit.IsRectEmpty() );

bool bRetVal =
		wndContentExpand.Activate(
			rcExpandInit,
			&wndGrid,
			__ECWAF_DEF_EXPANDED_ITEM_PAINTER
				|__ECWAF_DRAW_SOURCE
				|__ECWAF_NO_CAPTURE
				|__ECWAF_REDIRECT_MOUSE
				|__ECWAF_REDIRECT_NO_DEACTIVATE
				|__ECWAF_REDIRECT_AND_HANDLE
				|__ECWAF_HANDLE_MOUSE_ACTIVATE
				|__ECWAF_MA_NOACTIVATE
			);
	return bRetVal;
}

bool CExtGridCell::OnPaintExpandedContent(
	CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA & picd,
	const CExtGridWnd & wndGrid,
	const CExtGridHitTestInfo & htInfo,
	CExtContentExpandWnd & wndContentExpand,
	INT nSizeOfExpandShadow
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( picd.m_dc.GetSafeHdc() != NULL );
	ASSERT( wndContentExpand.GetSafeHwnd() != NULL );
	ASSERT( ! htInfo.IsHoverEmpty() );
	ASSERT( htInfo.IsValidRect() );
	ASSERT( nSizeOfExpandShadow >= 0 );
	wndContentExpand;
COLORREF clrBk = wndGrid.OnSiwGetSysColor( COLOR_INFOBK );
COLORREF clrBorder = wndGrid.OnSiwGetSysColor( COLOR_3DDKSHADOW );
CRect rcItem( picd.m_rcItem );
	rcItem.DeflateRect(
		0,
		0,
		nSizeOfExpandShadow,
		nSizeOfExpandShadow
		);
CExtWndShadow _shadow;
	_shadow.Paint(
		picd.m_dc,
		rcItem,
		CRect(0,0,0,0),
		CRect(0,0,0,0),
		nSizeOfExpandShadow
		);
	picd.m_dc.FillSolidRect( &rcItem, clrBk );
	picd.m_dc.Draw3dRect( &rcItem, clrBorder, clrBorder );
CRect rcVisibleRange(
		htInfo.m_nColNo, htInfo.m_nRowNo,
		htInfo.m_nColNo, htInfo.m_nRowNo
		);
INT nOldBkMode = picd.m_dc.SetBkMode( TRANSPARENT );
CFont * pOldFont = picd.m_dc.SelectObject( &wndGrid.OnSiwGetDefaultFont() );
	rcItem.DeflateRect(
		__EXT_EXPANDED_TEXT_GAP_X,
		__EXT_EXPANDED_TEXT_GAP_Y
		);
	OnPaintText(
		rcItem,
		wndGrid,
		picd.m_dc,
		htInfo.m_nVisibleColNo,
		htInfo.m_nVisibleRowNo,
		htInfo.m_nColNo,
		htInfo.m_nRowNo,
		htInfo.GetInnerOuterTypeOfColumn(),
		htInfo.GetInnerOuterTypeOfRow(),
		htInfo.m_rcExtra,
		htInfo.m_rcItem,
		rcVisibleRange,
		htInfo.m_dwAreaFlags,
		0L //dwHelperPaintFlags
		);
	picd.m_dc.SelectObject( pOldFont );
	picd.m_dc.SetBkMode( nOldBkMode );
	return true;
}

INT CExtGridCell::IconIndexGet() const
{
	ASSERT_VALID( this );
	return m_nIconIndex;
}

INT CExtGridCell::IconIndexSet(
	INT nIconIndex // = -1 // -1 - remove icon
	)
{
	ASSERT_VALID( this );
INT nIconIndexOld = m_nIconIndex;
	m_nIconIndex = nIconIndex;
	return nIconIndexOld;
}

CExtCmdIcon * CExtGridCell::IconGet()
{
	ASSERT_VALID( this );
CExtGridDataProvider * pDataProvider = DataProviderGet();
	if( pDataProvider == NULL )
		return NULL;
	ASSERT_VALID( pDataProvider );
INT nIconIndex = IconIndexGet();
	if( nIconIndex < 0 )
		return NULL;
INT nDpIconCount = pDataProvider->IconGetCount();
	if( nIconIndex >= nDpIconCount )
	{
		//ASSERT( FALSE );
		return NULL;
	} // if( nIconIndex >= nDpIconCount )
	return pDataProvider->IconGetAt( nIconIndex );
}

const CExtCmdIcon * CExtGridCell::IconGet() const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridCell * > ( this ) )
			-> IconGet();
}

CSize CExtGridCell::IconGetSize() const
{
	ASSERT_VALID( this );
const CExtCmdIcon * pIcon = IconGet();
	if(		pIcon == NULL
		||	pIcon->IsEmpty()
		)
		return CSize( 0, 0 );
CSize _size = pIcon->GetSize();
	return _size;
}

HWND CExtGridCell::OnInplaceControlCreate(
	CExtGridWnd & wndGrid,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	const RECT & rcInplaceControl,
	LONG nLastEditedColNo,
	LONG nLastEditedRowNo
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
DWORD dwCellStyle = GetStyle();
	if( (dwCellStyle&__EGCS_NO_INPLACE_CONTROL) != 0 )
		return NULL;
CExtGridInplaceEdit * pEdit = NULL;
	try
	{
		pEdit =
			new CExtGridInplaceEdit(
				wndGrid,
				*this,
				nVisibleColNo,
				nVisibleRowNo,
				nColNo,
				nRowNo,
				nColType,
				nRowType,
				rcCellExtra,
				rcCell,
				rcInplaceControl,
				nLastEditedColNo,
				nLastEditedRowNo
				);
		ASSERT_VALID( pEdit );
		if( ! pEdit->Create() )
		{
			ASSERT( FALSE );
			return NULL;
		} // if( ! pEdit->Create() )
	} // try
	catch( CException * pXept )
	{
		ASSERT( FALSE );
		pXept->Delete();
		return NULL;
	} // catch( CException * pXept )
	catch( ... )
	{
		ASSERT( FALSE );
		return NULL;
	} // catch( ... )
	ASSERT_VALID( pEdit );
HWND hWnd = pEdit->GetSafeHwnd();
	ASSERT( hWnd != NULL && ::IsWindow(hWnd) );
	return hWnd;
}

bool CExtGridCell::OnInplaceControlWindowProc(
	LRESULT & lResult,
	UINT nMessage,
	WPARAM wParam,
	LPARAM lParam,
	HWND hWndInplaceControl,
	CExtGridWnd & wndGrid,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	const RECT & rcInplaceControl
	)
{
	ASSERT_VALID( this );
	ASSERT( hWndInplaceControl != NULL && ::IsWindow(hWndInplaceControl) );
	ASSERT_VALID( (&wndGrid) );
	lResult;
	nMessage;
	wParam;
	lParam;
	hWndInplaceControl;
	wndGrid;
	nVisibleColNo;
	nVisibleRowNo;
	nColNo;
	nRowNo;
	nColType;
	nRowType;
	rcCellExtra;
	rcCell;
	rcInplaceControl;
	return false;
}

bool CExtGridCell::OnInplaceControlPreTranslateMessage(
	MSG * pMsg,
	HWND hWndInplaceControl,
	CExtGridWnd & wndGrid,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	const RECT & rcInplaceControl
	)
{
	ASSERT_VALID( this );
	ASSERT( hWndInplaceControl != NULL && ::IsWindow(hWndInplaceControl) );
	ASSERT_VALID( (&wndGrid) );
	pMsg;
	hWndInplaceControl;
	wndGrid;
	nVisibleColNo;
	nVisibleRowNo;
	nColNo;
	nRowNo;
	nColType;
	nRowType;
	rcCellExtra;
	rcCell;
	rcInplaceControl;
	if(		(	pMsg->message == WM_KEYDOWN
			||	pMsg->message == WM_KEYUP
			)
		&&	pMsg->wParam == VK_MENU
		)
		return true;
	return false;
}

bool CExtGridCell::OnInplaceControlIdle(
	HWND hWndInplaceControl,
	CExtGridWnd & wndGrid,
	LONG nIdleCounter
	)
{
	ASSERT_VALID( this );
	ASSERT( hWndInplaceControl != NULL && ::IsWindow(hWndInplaceControl) );
	ASSERT_VALID( (&wndGrid) );
	hWndInplaceControl;
	wndGrid;
	nIdleCounter;
	return false;
}

bool CExtGridCell::OnInplaceControlTextInputVerify(
	HWND hWndInplaceControl,
	CExtGridWnd & wndGrid,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	__EXT_MFC_SAFE_LPCTSTR sTextInitial,
	__EXT_MFC_SAFE_LPCTSTR sTextPrevious,
	CExtSafeString & sTextNew,
	bool bEndEdit
	) const
{
	ASSERT_VALID( this );
	ASSERT( hWndInplaceControl != NULL && ::IsWindow(hWndInplaceControl) );
	ASSERT_VALID( (&wndGrid) );
	hWndInplaceControl;
	wndGrid;
	nVisibleColNo;
	nVisibleRowNo;
	nColNo;
	nRowNo;
	nColType;
	nRowType;
	sTextInitial;
	sTextPrevious;
	if( ! bEndEdit )
		return true;
HRESULT hr =
		OnParseText(
			sTextNew.IsEmpty()
				? _T("")
				: LPCTSTR(sTextNew)
			);
	if( FAILED(hr) )
		return false;
	return true;
}

HRESULT CExtGridCell::OnParseText(
	__EXT_MFC_SAFE_LPCTSTR sText
	) const
{
	ASSERT_VALID( this );
	ASSERT( sText != NULL );
	if( sText == NULL )
		return E_INVALIDARG;
	return S_OK;
}

void CExtGridCell::OnInplaceControlTextInputComplete(
	HWND hWndInplaceControl,
	CExtGridWnd & wndGrid,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	__EXT_MFC_SAFE_LPCTSTR sTextNew,
	bool bSaveChanges
	)
{
	ASSERT_VALID( this );
	ASSERT( hWndInplaceControl != NULL && ::IsWindow(hWndInplaceControl) );
	ASSERT_VALID( (&wndGrid) );
	hWndInplaceControl;
	wndGrid;
	nVisibleColNo;
	nVisibleRowNo;
	nColNo;
	nRowNo;
	nColType;
	nRowType;
	sTextNew;
	if( bSaveChanges )
	{
		TextSet( sTextNew );
		CExtGridDataProvider & _DataProvider = wndGrid.OnGridQueryDataProvider();
		_DataProvider.SortOrderUpdate( true, &wndGrid );
		_DataProvider.SortOrderUpdate( false, &wndGrid );
		CPoint ptFocus = wndGrid.FocusGet();
		if( ptFocus.x >= 0 )
			wndGrid.EnsureVisibleColumn( ptFocus.x );
		if( ptFocus.y >= 0 )
			wndGrid.EnsureVisibleRow( ptFocus.y );
	}
}

bool CExtGridCell::OnButtonPopupMenuTrack(
	CExtGridCell::TrackCellStateInfo_t & _tcsi
	)
{
	ASSERT_VALID( this );
	ASSERT( (&_tcsi.m_cell) == this );
	ASSERT_VALID( (&_tcsi.m_wndGrid) );
CPoint ptFocusOld = _tcsi.m_wndGrid.FocusGet();
CPoint ptFocusDesired( _tcsi.m_nColNo, _tcsi.m_nRowNo );
	if( ptFocusDesired != ptFocusOld )
	{
		_tcsi.m_wndGrid.FocusSet( ptFocusDesired );
		_tcsi.m_wndGrid.UpdateWindow();
	} // if( ptFocusDesired != ptFocusOld )
CExtPopupListBoxMenuWnd * pPopup =
		new CExtPopupListBoxMenuWnd(
			0L,
			_tcsi.m_dwListBoxStyles
			);
	if( ! pPopup->CreatePopupMenu( _tcsi.m_wndGrid.GetSafeHwnd() ) )
	{
		ASSERT( FALSE );
		return false;
	}
	pPopup->m_pCbListBoxSelection = 
		(CExtPopupInplaceListBox::pCbListBoxSelection)
			stat_CbListBoxSelection;
	pPopup->m_pSelectionCoockie = (LPVOID)(&_tcsi);
	
	pPopup->m_pCbInitListBoxContent =
		(CExtPopupInplaceListBox::pCbInitListBoxContent)
			stat_CbInitListBoxContent;
	pPopup->m_pInitListBoxCoockie = (LPVOID)(&_tcsi);

	pPopup->m_pCbListBoxItemDraw =
		(CExtPopupInplaceListBox::pCbListBoxItemDraw)
			stat_CbListBoxItemDraw;
	pPopup->m_pCbListBoxItemMeasure =
		(CExtPopupInplaceListBox::pCbListBoxItemMeasure)
			stat_CbListBoxItemMeasure;
	pPopup->m_pListBoxItemCoockie = (LPVOID)(&_tcsi);

int nMenuBorderSize = pPopup->OnQueryMenuBorderSize();
CRect wr( _tcsi.m_rcCell );
CSize _sizeMeasured =
		OnPopupListBoxMeasureTrackSize(
			_tcsi,
			pPopup
			);
	pPopup->m_sizeChildControl = _sizeMeasured;
	pPopup->m_sizeChildControl.cx = wr.Width() - nMenuBorderSize * 2;
	pPopup->m_sizeChildControl.cx = max( pPopup->m_sizeChildControl.cx, 100 );
	_tcsi.m_wndGrid.ClientToScreen( &wr );
CPoint pt( wr.CenterPoint() );
	if( !pPopup->TrackPopupMenu(
			TPMX_TOPALIGN
				|TPMX_DO_MESSAGE_LOOP
				|TPMX_COMBINE_NONE,
			pt.x,
			pt.y,
			&wr,
			NULL,
			NULL,
			NULL,
			false
			)
		)
	{
		delete pPopup;
		ASSERT( FALSE );
		return false;
	}
	if( ! _tcsi.m_bHelperSelChanged )
		return false;
	return OnButtonPopupMenuComplete( _tcsi );
}

bool CExtGridCell::OnButtonPopupMenuComplete(
	CExtGridCell::TrackCellStateInfo_t & _tcsi
	)
{
	ASSERT_VALID( this );
	ASSERT( (&_tcsi.m_cell) == this );
	ASSERT_VALID( (&_tcsi.m_wndGrid) );
	_tcsi;
	return true;
}

bool CExtGridCell::stat_CbInitListBoxContent(
	CExtPopupInplaceListBox & wndListBox,
	CExtGridCell::TrackCellStateInfo_t & _tcsi
	)
{
	ASSERT_VALID( (&wndListBox) );
	ASSERT_VALID( (&_tcsi.m_cell) );
	ASSERT_VALID( (&_tcsi.m_wndGrid) );
	return
		_tcsi.m_cell.OnPopupListBoxInitContent(
			wndListBox,
			_tcsi
			);
}

bool CExtGridCell::stat_CbListBoxSelection(
	CExtPopupInplaceListBox & wndListBox,
	CExtGridCell::TrackCellStateInfo_t & _tcsi,
	int eSAT // CExtPopupInplaceListBox::e_sel_action_t
	)
{
	ASSERT_VALID( (&wndListBox) );
	ASSERT_VALID( (&_tcsi.m_cell) );
	ASSERT_VALID( (&_tcsi.m_wndGrid) );
	switch( ((CExtPopupInplaceListBox::e_sel_action_t)eSAT) )
	{
	case CExtPopupInplaceListBox::__SAT_SELCHANGE:
		return
			_tcsi.m_cell.OnPopupListBoxSelChange(
				wndListBox,
				_tcsi
				);
	case CExtPopupInplaceListBox::__SAT_SELENDOK:
		return
			_tcsi.m_cell.OnPopupListBoxSelEndOK(
				wndListBox,
				_tcsi
				);
	case CExtPopupInplaceListBox::__SAT_SELENDCANCEL:
		return
			_tcsi.m_cell.OnPopupListBoxSelEndCancel(
				wndListBox,
				_tcsi
				);
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( ((CExtPopupInplaceListBox::e_sel_action_t)eSAT) )
	return false;
}

bool CExtGridCell::stat_CbListBoxItemDraw(
	CExtPopupInplaceListBox & wndListBox,
	CExtGridCell::TrackCellStateInfo_t & _tcsi,
	LPDRAWITEMSTRUCT pDIS
	)
{
	ASSERT_VALID( (&wndListBox) );
	ASSERT_VALID( (&_tcsi.m_cell) );
	ASSERT_VALID( (&_tcsi.m_wndGrid) );
	ASSERT( pDIS != NULL && pDIS->hDC != NULL );
	return
		_tcsi.m_cell.OnPopupListBoxItemDraw(
			wndListBox,
			_tcsi,
			pDIS
			);
}

bool CExtGridCell::stat_CbListBoxItemMeasure(
	CExtPopupInplaceListBox & wndListBox,
	CExtGridCell::TrackCellStateInfo_t & _tcsi,
	LPMEASUREITEMSTRUCT pMIS
	)
{
	ASSERT_VALID( (&wndListBox) );
	ASSERT_VALID( (&_tcsi.m_cell) );
	ASSERT_VALID( (&_tcsi.m_wndGrid) );
	ASSERT( pMIS != NULL );
	return
		_tcsi.m_cell.OnPopupListBoxItemMeasure(
			wndListBox,
			_tcsi,
			pMIS
			);
}

CSize CExtGridCell::OnPopupListBoxCalcItemExtraSizes(
	CExtGridCell::TrackCellStateInfo_t & _tcsi,
	LONG nItemNo
	) const
{
	ASSERT_VALID( this );
	ASSERT( (&_tcsi.m_cell) == this );
	ASSERT_VALID( (&_tcsi.m_wndGrid) );
	_tcsi;
	nItemNo;
	return CSize( 2, 1 );
}

CSize CExtGridCell::OnPopupListBoxMeasureTrackSize(
	CExtGridCell::TrackCellStateInfo_t & _tcsi,
	CExtPopupListBoxMenuWnd * pPopup
	)
{
	ASSERT_VALID( this );
	ASSERT( (&_tcsi.m_cell) == this );
	ASSERT_VALID( pPopup );
	ASSERT_VALID( (&_tcsi.m_wndGrid) );
	_tcsi;
	pPopup;
	return CSize( 150, 100 );
}

bool CExtGridCell::OnPopupListBoxInitContent(
	CExtPopupInplaceListBox & wndListBox,
	CExtGridCell::TrackCellStateInfo_t & _tcsi
	)
{
	ASSERT_VALID( this );
	ASSERT( (&_tcsi.m_cell) == this );
	ASSERT_VALID( (&wndListBox) );
	ASSERT_VALID( (&_tcsi.m_wndGrid) );
	_tcsi.m_nItemNoSelCancel = wndListBox.GetCurSel();
	return true;
}

bool CExtGridCell::OnPopupListBoxSelChange(
	CExtPopupInplaceListBox & wndListBox,
	CExtGridCell::TrackCellStateInfo_t & _tcsi
	)
{
	ASSERT_VALID( this );
	ASSERT( (&_tcsi.m_cell) == this );
	ASSERT_VALID( (&wndListBox) );
	ASSERT_VALID( (&_tcsi.m_wndGrid) );
	_tcsi.m_nItemNoSelChange = wndListBox.GetCurSel();
	return false;
}

bool CExtGridCell::OnPopupListBoxSelEndOK(
	CExtPopupInplaceListBox & wndListBox,
	CExtGridCell::TrackCellStateInfo_t & _tcsi
	)
{
	ASSERT_VALID( this );
	ASSERT( (&_tcsi.m_cell) == this );
	ASSERT_VALID( (&wndListBox) );
	ASSERT_VALID( (&_tcsi.m_wndGrid) );
	_tcsi.m_nItemNoSelEndOK = wndListBox.GetCurSel();
	_tcsi.m_bHelperSelChanged = true;
	if( (wndListBox.GetStyle()&LBS_HASSTRINGS) != 0 )
	{
		LONG nItemCount = wndListBox.GetCount();
		if( nItemCount > 0L )
		{
			LONG nCurSel = wndListBox.GetCurSel();
			if( 0L <= nCurSel && nCurSel < nItemCount )
			{
				CString strText;
				wndListBox.GetText( nCurSel, strText );
				if( ! strText.IsEmpty() )
				{
					HRESULT hr = OnParseText( LPCTSTR(strText) );
					if( hr == S_OK )
					{
						TextSet( LPCTSTR(strText) );
						_tcsi.m_wndGrid.InvalidateRect( &_tcsi.m_rcCellExtra );
						//_tcsi.m_wndGrid.UpdateWindow();
					} // if( hr == S_OK )
				} // if( ! strText.IsEmpty() )
			} // if( 0L <= nCurSel && nCurSel < nItemCount )
		} // if( nItemCount > 0L )
	} // if( (wndListBox.GetStyle()&LBS_HASSTRINGS) != 0 )
	return false;
}

bool CExtGridCell::OnPopupListBoxSelEndCancel(
	CExtPopupInplaceListBox & wndListBox,
	CExtGridCell::TrackCellStateInfo_t & _tcsi
	)
{
	ASSERT_VALID( this );
	ASSERT( (&_tcsi.m_cell) == this );
	ASSERT_VALID( (&wndListBox) );
	ASSERT_VALID( (&_tcsi.m_wndGrid) );
	wndListBox;
	_tcsi;
	return false;
}

bool CExtGridCell::OnPopupListBoxItemDraw(
	CExtPopupInplaceListBox & wndListBox,
	CExtGridCell::TrackCellStateInfo_t & _tcsi,
	LPDRAWITEMSTRUCT pDIS
	)
{
	ASSERT_VALID( this );
	ASSERT( (&_tcsi.m_cell) == this );
	ASSERT_VALID( (&wndListBox) );
	ASSERT_VALID( (&_tcsi.m_wndGrid) );
	ASSERT( pDIS != NULL && pDIS->hDC != NULL );
	_tcsi;
	if( (wndListBox.GetStyle() & LBS_HASSTRINGS) == 0 )
		return false;
	if( ((INT)pDIS->itemID) < 0 || ((INT)pDIS->itemID) >= wndListBox.GetCount() )
		return true;
CDC dc;
	dc.Attach( pDIS->hDC );
	dc.FillSolidRect(
		&pDIS->rcItem,
		_tcsi.m_wndGrid.OnSiwGetSysColor(
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
				_tcsi.m_wndGrid.OnSiwGetSysColor(
					( pDIS->itemState & ODS_SELECTED )
						? COLOR_HIGHLIGHTTEXT
						: COLOR_BTNTEXT
					)
				);
		int nOldBkMode = dc.SetBkMode( TRANSPARENT );
		CRect rcText( pDIS->rcItem );
		rcText.DeflateRect(
			OnPopupListBoxCalcItemExtraSizes(
				_tcsi,
				(LONG)pDIS->itemID
				)
			);
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

bool CExtGridCell::OnPopupListBoxItemMeasure(
	CExtPopupInplaceListBox & wndListBox,
	CExtGridCell::TrackCellStateInfo_t & _tcsi,
	LPMEASUREITEMSTRUCT pMIS
	)
{
	ASSERT_VALID( this );
	ASSERT( (&_tcsi.m_cell) == this );
	ASSERT_VALID( (&wndListBox) );
	ASSERT_VALID( (&_tcsi.m_wndGrid) );
	ASSERT( pMIS != NULL );
	_tcsi;
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
	rcMeasure.InflateRect(
		OnPopupListBoxCalcItemExtraSizes(
			_tcsi,
			(LONG)pMIS->itemID
			)
		);
	pMIS->itemWidth = rcMeasure.Width();
	pMIS->itemHeight = rcMeasure.Height();
	return true;
}

bool CExtGridCell::OnCellPressingEnter(
	CExtGridWnd & wndGrid,
	const CExtGridHitTestInfo & htInfo,
	UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
	UINT nFlags // mouse event flags
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	wndGrid;
	htInfo;
	nChar;
	nFlags;
	return false;
}

void CExtGridCell::OnCellPressingStateChanged(
	CExtGridWnd & wndGrid,
	const CExtGridHitTestInfo & htInfo,
	UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
	UINT nFlags, // mouse event flags
	bool bMouseInsideCell
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	wndGrid;
	htInfo;
	nChar;
	nFlags;
	bMouseInsideCell;
}

bool CExtGridCell::OnCellPressingLeave(
	CExtGridWnd & wndGrid,
	const CExtGridHitTestInfo & htInfo,
	UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
	UINT nFlags, // mouse event flags
	bool bPressedEvent,
	bool bStartDragEvent
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	wndGrid;
	htInfo;
	nChar;
	nFlags;
	bPressedEvent;
	bStartDragEvent;
	return false;
}

bool CExtGridCell::OnOuterDndTrack(
	CExtGridWnd & wndGrid,
	const CExtGridHitTestInfo & htInfo,
	UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
	UINT nFlags // mouse event flags
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	wndGrid;
	htInfo;
	nChar;
	nFlags;
	return false;
}

bool CExtGridCell::OnOuterDndComplete(
	CExtGridWnd & wndGrid,
	bool bDraggedCell, // true - dragged cell, false - drop-target cell
	const CExtGridHitTestInfo & htInfoDrag,
	const CExtGridHitTestInfo & htInfoDrop
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	wndGrid;
	bDraggedCell;
	htInfoDrag;
	htInfoDrop;
	return false;
}

bool CExtGridCell::OnOuterDndOut(
	CExtGridWnd & wndGrid,
	const CExtGridHitTestInfo & htInfo
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	wndGrid;
	htInfo;
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CExtGridInplaceEdit

IMPLEMENT_DYNAMIC( CExtGridInplaceEdit, CEdit );

CExtGridInplaceEdit::CExtGridInplaceEdit(
	CExtGridWnd & wndGrid,
	CExtGridCell & cell,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	const RECT & rcInplaceControl,
	LONG nLastEditedColNo,
	LONG nLastEditedRowNo
	)
	: m_wndGrid( wndGrid )
	, m_cell( cell )
	, m_nVisibleColNo( nVisibleColNo )
	, m_nVisibleRowNo( nVisibleRowNo )
	, m_nColNo( nColNo )
	, m_nRowNo( nRowNo )
	, m_nColType( nColType )
	, m_nRowType( nRowType )
	, m_rcCellExtra( rcCellExtra )
	, m_rcCell( rcCell )
	, m_rcInplaceControl( rcInplaceControl )
	, m_nLastEditedColNo( nLastEditedColNo )
	, m_nLastEditedRowNo( nLastEditedRowNo )
	, m_bInEndEdit( false )
	, m_bInValidation( false )
	, m_bInConstruction( false )
{
	m_rcInplaceControl.InflateRect( 1, 1, 0, 0 );
	m_rcCell.OffsetRect( -1, -1 );
	m_rcNonClientMargins.SetRect(
		1,
		m_rcInplaceControl.top - m_rcCell.top + 1,
		1+1,
		m_rcCell.bottom - m_rcInplaceControl.bottom + 1
		);
	m_rcInplaceControl.top = m_rcCell.top;
	if( m_rcCell.Height() >= m_rcInplaceControl.Height() )
		m_rcInplaceControl.bottom = m_rcCell.bottom;
	if( m_rcNonClientMargins.left < 0 )
		m_rcNonClientMargins.left = 0;
	if( m_rcNonClientMargins.right < 0 )
		m_rcNonClientMargins.right = 0;
	if( m_rcNonClientMargins.top < 0 )
		m_rcNonClientMargins.top = 0;
	if( m_rcNonClientMargins.bottom < 0 )
		m_rcNonClientMargins.bottom = 0;
}

CExtGridInplaceEdit::~CExtGridInplaceEdit()
{
}

#ifdef _DEBUG
void CExtGridInplaceEdit::AssertValid() const
{
	CEdit::AssertValid();
	ASSERT_VALID( (&m_wndGrid) );
	ASSERT_KINDOF( CExtGridWnd, (&m_wndGrid) );
	ASSERT_VALID( (&m_cell) );
	ASSERT_KINDOF( CExtGridCell, (&m_cell) );
}
void CExtGridInplaceEdit::Dump( CDumpContext & dc ) const
{
	CEdit::Dump( dc );
}
#endif // _DEBUG

bool CExtGridInplaceEdit::Create()
{
	ASSERT_VALID( this );
	ASSERT( m_hWnd == NULL );
	if( m_wndGrid.GetSafeHwnd() == NULL )
	{
		ASSERT( FALSE );
		delete this;
		return false;
	}
DWORD dwAreaFlags =
		CExtGridHitTestInfo::CellTypesToAreaFlags(
			m_nColType,
			m_nRowType
			);
HFONT hFont =
		m_cell.OnQueryCellFont(
			m_wndGrid,
			m_nVisibleColNo,
			m_nVisibleRowNo,
			m_nColNo,
			m_nRowNo,
			m_nColType,
			m_nRowType,
			dwAreaFlags
			);
	if( hFont == NULL )
	{
		hFont = (HFONT)
			m_wndGrid.OnSiwGetDefaultFont().GetSafeHandle();
		if( hFont == NULL )
			hFont = (HFONT)
				::GetStockObject(DEFAULT_GUI_FONT);
		if( hFont == NULL )
		{
			if( hFont == NULL )
				hFont = (HFONT)
					::GetStockObject(SYSTEM_FONT);
			if( hFont == NULL )
			{
				ASSERT( FALSE );
				delete this;
				return false;
			}
		}
	}
LOGFONT lf;
	VERIFY( ::GetObject(hFont,sizeof(LOGFONT),&lf) );
	if( ! m_font.CreateFontIndirect(&lf) )
	{
		ASSERT( FALSE );
		delete this;
		return false;
	}

CRect rcWnd( m_rcInplaceControl );
//	rcWnd.InflateRect(
//		(m_rcNonClientMargins.left == 0) ? 0 : (m_rcNonClientMargins.left-1),
//		m_rcNonClientMargins.top,
//		(m_rcNonClientMargins.right == 0) ? 0 : (m_rcNonClientMargins.right-1),
//		m_rcNonClientMargins.bottom
//		);
	rcWnd.OffsetRect( 0, 1 );
UINT nDrawTextFlags =
		m_cell.OnQueryDrawTextFlags(
			m_nVisibleColNo,
			m_nVisibleRowNo,
			m_nColNo,
			m_nRowNo,
			m_nColType,
			m_nRowType,
			dwAreaFlags
			);
DWORD dwStyle = WS_CHILD|ES_AUTOHSCROLL;
	if( nDrawTextFlags & DT_RIGHT )
		dwStyle |= ES_RIGHT;
	if( nDrawTextFlags & DT_CENTER )
		dwStyle |= ES_CENTER;
	else
		dwStyle |= ES_LEFT;
DWORD dwCellStyle = m_cell.GetStyle();
	if( (dwCellStyle&__EGCS_READ_ONLY) != 0 )
		dwStyle |= ES_READONLY;
	m_bInConstruction = true;
HWND hWnd =
		::CreateWindowEx(
			0L,
			_T("EDIT"),
			_T(""),
			dwStyle,
			rcWnd.left,
			rcWnd.top,
			rcWnd.Width(),
			rcWnd.Height(),
			m_wndGrid.GetSafeHwnd(),
			NULL,
			::AfxGetInstanceHandle(),
			0L
			);
	if( hWnd == NULL )
	{
		ASSERT( FALSE );
		delete this;
		return false;
	}
	if( ! SubclassWindow( hWnd ) )
	{
		ASSERT( FALSE );
		ASSERT( CWnd::FromHandlePermanent(hWnd) == NULL );
		::DestroyWindow( hWnd );
		delete this;
		return false;
	}
	m_cell.TextGet( m_strTextPrevious );
	m_strTextInitial = m_strTextPrevious;
	if( ! m_strTextInitial.IsEmpty() )
		SetWindowText( LPCTSTR(m_strTextInitial) );
	SetFont( &m_font );
	SetMargins( 2, 0 );
	if( m_nLastEditedRowNo == m_nRowNo )
	{
		if( m_nLastEditedColNo == (m_nColNo - 1L) )
			SetSel( 0, 0 );
		else if( m_nLastEditedColNo == (m_nColNo + 1L) )
			SetSel( -1, -1 );
		else
			SetSel( 0, -1 );
	} // if( m_nLastEditedRowNo == m_nRowNo )
	else
			SetSel( 0, -1 );
	SetWindowPos(
		&CWnd::wndTop,0,0,0,0,
		SWP_NOMOVE|SWP_NOSIZE
			|SWP_NOACTIVATE
			|SWP_FRAMECHANGED
			|SWP_SHOWWINDOW
		);
	SetFocus();
	m_bInConstruction = false;
	return true;
}

void CExtGridInplaceEdit::PostNcDestroy()
{
	ASSERT_VALID( this );
	delete this;
}

BOOL CExtGridInplaceEdit::PreTranslateMessage( MSG * pMsg )
{
	ASSERT_VALID( this );
	if( m_cell.OnInplaceControlPreTranslateMessage(
			pMsg,
			m_hWnd,
			m_wndGrid,
			m_nVisibleColNo,
			m_nVisibleRowNo,
			m_nColNo,
			m_nRowNo,
			m_nColType,
			m_nRowType,
			m_rcCellExtra,
			m_rcCell,
			m_rcInplaceControl
			)
		)
		return TRUE;
	return CEdit::PreTranslateMessage( pMsg );
}

LRESULT CExtGridInplaceEdit::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
LRESULT lResult = 0L;
	if( m_cell.OnInplaceControlWindowProc(
			lResult,
			message,
			wParam,
			lParam,
			m_hWnd,
			m_wndGrid,
			m_nVisibleColNo,
			m_nVisibleRowNo,
			m_nColNo,
			m_nRowNo,
			m_nColType,
			m_nRowType,
			m_rcCellExtra,
			m_rcCell,
			m_rcInplaceControl
			)
		)
		return lResult;
	if( message == WM_NCCALCSIZE )
	{
		NCCALCSIZE_PARAMS * pNCCSP =
			reinterpret_cast < NCCALCSIZE_PARAMS * > ( lParam );
		ASSERT( pNCCSP != NULL );
		CRect rcInBarWnd( pNCCSP->rgrc[0] );
		rcInBarWnd.DeflateRect(
			m_rcNonClientMargins.left,
			m_rcNonClientMargins.top,
			m_rcNonClientMargins.right,
			m_rcNonClientMargins.bottom
			);
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
		COLORREF clrBk =
			m_wndGrid.OnSiwGetSysColor(
				( (GetStyle()&ES_READONLY) != 0 )
					? COLOR_3DFACE
					: COLOR_WINDOW
				);
		COLORREF clrBorder =
			m_wndGrid.OnSiwGetSysColor(
				COLOR_3DDKSHADOW
				);
		dc.FillSolidRect( &rcInBarWnd, clrBk );
		dc.Draw3dRect( &rcInBarWnd, clrBorder, clrBorder );
		//rcInBarWnd.DeflateRect( 1, 1 );
		//dc.Draw3dRect( &rcInBarWnd, clrBorder, clrBorder );
		return 0;
	} // if( message == WM_NCPAINT )
	if( message == WM_GETDLGCODE )
		return DLGC_WANTALLKEYS|DLGC_WANTCHARS|DLGC_WANTTAB;
	if( message == WM_KILLFOCUS )
	{
//CWnd * pWndNewFocus = CWnd::FromHandlePermanent( (HWND)wParam );
//pWndNewFocus;
		_DoEndEdit( true );
		return 0;
	} // if( message == WM_KILLFOCUS )
	if( message == WM_ACTIVATEAPP )
	{
		if( !wParam )
		_DoEndEdit( false );
		return 0;
	} // if( message == WM_ACTIVATEAPP )
	if( message == WM_KEYDOWN )
	{
		switch( int(wParam) )
		{
		case VK_RETURN:
			_DoEndEdit( true );
			return 0;
		case VK_ESCAPE:
			_DoEndEdit( false );
			return 0;
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
//		case VK_PRIOR:
//		case VK_NEXT:
//		case VK_HOME:
//		case VK_END:
		case VK_LEFT:
		case VK_RIGHT:
		{
			if(		m_nColType != 0
				||	m_nRowType != 0
				)
				break;
			if(		CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL)
				||	CExtPopupMenuWnd::IsKeyPressed(VK_SHIFT)
				)
				break;
			bool bTranslate = true;
			DWORD dwBseStyle = m_wndGrid.BseGetStyle();
			if( (dwBseStyle&__EGWS_BSE_WALK) == 0 )
				bTranslate = false;
			else if(	int(wParam) == VK_UP
					||	int(wParam) == VK_DOWN
//					||	int(wParam) == VK_PRIOR
//					||	int(wParam) == VK_NEXT
//					||	int(wParam) == VK_HOME
//					||	int(wParam) == VK_END
				)
			{
				if(		(dwBseStyle&__EGWS_BSE_WALK_VERT) == 0
					||	m_wndGrid.SiwScrollTypeVGet() == __ESIW_ST_VIRTUAL
					)
					bTranslate = false;
			}
			else if(	int(wParam) == VK_LEFT
					||	int(wParam) == VK_RIGHT
				)
			{
				if(		(dwBseStyle&__EGWS_BSE_WALK_HORZ) == 0
					||	m_wndGrid.SiwScrollTypeHGet() == __ESIW_ST_VIRTUAL
					)
					bTranslate = false;
				else
				{
					int nChrStart = 0, nChrEnd = 0;
					GetSel( nChrStart, nChrEnd );
					if( nChrStart != nChrEnd )
						bTranslate = false;
					else if( int(wParam) == VK_LEFT )
					{
						if( nChrStart != 0 )
							bTranslate = false;
						else if( m_nColNo == 0L )
							bTranslate = false;
					} // else if( m_wndGrid.SiwScrollTypeHGet() != __ESIW_ST_VIRTUAL )
					else if( int(wParam) == VK_RIGHT )
					{
						if( nChrStart < GetWindowTextLength() )
							bTranslate = false;
						else
						{
							LONG nColCount =
								(m_nColType == 0)
									? m_wndGrid.ColumnCountGet()
									: (	(m_nColType < 0)
										? m_wndGrid.OuterColumnCountLeftGet()
										: m_wndGrid.OuterColumnCountRightGet()
										)
									;
							ASSERT( m_nColNo < nColCount );
							if( m_nColNo == (nColCount - 1) )
								bTranslate = false;
						} // else from if( nChrStart < GetWindowTextLength() )
					} // else if( int(wParam) == VK_RIGHT )
				} // else if( ...
			}
			if( bTranslate )
			{
				LRESULT lResult = 0L;
				HWND hWndParent = GetParent()->GetSafeHwnd();
				if( hWndParent != NULL )
				{
					HWND hWndOwn = m_hWnd;
					HWND hWndGrid = m_wndGrid.m_hWnd;
					CExtGridWnd * pWndGrid = &m_wndGrid;
					_DoEndEdit( true );
					lResult =
						::SendMessage(hWndParent,WM_KEYDOWN,wParam,lParam);
					if(		::IsWindow(hWndGrid)
						&&	( ! ::IsWindow(hWndOwn) )
						&&	CWnd::FromHandlePermanent(hWndGrid) == pWndGrid
						)
					{
						ASSERT_VALID( pWndGrid );
						pWndGrid->EditCell();
					}
				}
				return lResult;
			} // if( bTranslate )
		} // case VK_...
		} // switch( int(wParam) )
		m_bInValidation = true;
		CString sTextOld;
		GetWindowText( sTextOld );
		DWORD dwSelSaved = CEdit::GetSel();
		CEdit::SetRedraw( FALSE );
		LRESULT lResult = CEdit::WindowProc( message, wParam, lParam );
		if( ! _DoVerifyTextInput(false) )
		{
			CEdit::SetSel( 0, -1 );
			CEdit::ReplaceSel( sTextOld );
			CEdit::SetSel( dwSelSaved );
		} // if( ! _DoVerifyTextInput(false) )
		CEdit::SetRedraw( TRUE );
		Invalidate();
		UpdateWindow();
		m_bInValidation = false;
		return lResult;
	} // if( message == WM_KEYDOWN )
	if(		message == WM_CHAR
		||	(	(! (m_bInConstruction||m_bInValidation) )
			&&	(	message == WM_CUT
				||	message == WM_PASTE
				||	message == WM_UNDO
				||	message == WM_CLEAR
				||	message == WM_SETTEXT
				||	message == EM_REPLACESEL
				)
			)
		)
	{
		m_bInValidation = true;
		CString sTextOld;
		GetWindowText( sTextOld );
		DWORD dwSelSaved = CEdit::GetSel();
		CEdit::SetRedraw( FALSE );
		LRESULT lResult = CEdit::WindowProc( message, wParam, lParam );
		if( ! _DoVerifyTextInput(false) )
		{
			CEdit::SetSel( 0, -1 );
			CEdit::ReplaceSel( sTextOld );
			CEdit::SetSel( dwSelSaved );
		} // if( ! _DoVerifyTextInput(false) )
		CEdit::SetRedraw( TRUE );
		Invalidate();
		UpdateWindow();
		m_bInValidation = false;
		return lResult;
	} // if( message == WM_CHAR ...

	if( message == WM_CANCELMODE || message == WM_ACTIVATEAPP )
	{
		_DoEndEdit( false );
		return 0L;
	} // if( message == WM_CANCELMODE || message == WM_ACTIVATEAPP )

	return CEdit::WindowProc( message, wParam, lParam );
}

void CExtGridInplaceEdit::DoEndEdit( bool bSaveChanges )
{
	ASSERT_VALID( this );
	m_cell.OnInplaceControlTextInputComplete(
		m_hWnd,
		m_wndGrid,
		m_nVisibleColNo,
		m_nVisibleRowNo,
		m_nColNo,
		m_nRowNo,
		m_nColType,
		m_nRowType,
		LPCTSTR(m_strTextPrevious),
		bSaveChanges
		);
	DestroyWindow();
}

void CExtGridInplaceEdit::_DoEndEdit( bool bSaveChanges )
{
	ASSERT_VALID( this );
	if( m_bInEndEdit )
		return;
	m_bInEndEdit = true;
	if( bSaveChanges )
	{
		HWND hWndOwn = m_hWnd;
		ASSERT( hWndOwn != NULL && ::IsWindow(hWndOwn) );
		if( ! _DoVerifyTextInput(true) )
			bSaveChanges = false;
		if( ! ::IsWindow(hWndOwn) )
			return;
	} // if( bSaveChanges )
	DoEndEdit( bSaveChanges );
}

bool CExtGridInplaceEdit::DoVerifyTextInput(
	__EXT_MFC_SAFE_LPCTSTR sTextInitial,
	__EXT_MFC_SAFE_LPCTSTR sTextPrevious,
	CExtSafeString & sTextNew,
	bool bEndEdit
	)
{
	ASSERT_VALID( this );
	return
		m_cell.OnInplaceControlTextInputVerify(
			m_hWnd,
			m_wndGrid,
			m_nVisibleColNo,
			m_nVisibleRowNo,
			m_nColNo,
			m_nRowNo,
			m_nColType,
			m_nRowType,
			sTextInitial,
			sTextPrevious,
			sTextNew,
			bEndEdit
			);
}

bool CExtGridInplaceEdit::_DoVerifyTextInput( bool bEndEdit )
{
	ASSERT_VALID( this );
CExtSafeString sTextNew;
int nWindowTextLength = GetWindowTextLength();
	if( nWindowTextLength > 0 )
	{
		GetWindowText(
			sTextNew.GetBuffer( nWindowTextLength + 2 ),
			nWindowTextLength + 1
			);
		sTextNew.ReleaseBuffer();
	} // if( nWindowTextLength > 0 )
__EXT_MFC_SAFE_LPCTSTR sTextInitial =
		m_strTextInitial.IsEmpty()
			? _T("")
			: LPCTSTR(m_strTextInitial)
			;
__EXT_MFC_SAFE_LPCTSTR sTextPrevious =
		m_strTextPrevious.IsEmpty()
			? _T("")
			: LPCTSTR(m_strTextPrevious)
			;
bool bRetVal =
		DoVerifyTextInput(
			sTextInitial,
			sTextPrevious,
			sTextNew,
			bEndEdit
			);
	if( bRetVal )
		m_strTextPrevious = sTextNew;
	return bRetVal;
}


/////////////////////////////////////////////////////////////////////////////
// CExtGridCellStringDM

IMPLEMENT_DYNCREATE( CExtGridCellStringDM, CExtGridCell );

CExtGridCellStringDM::CExtGridCellStringDM(
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
	, m_strData( LPTSTR(NULL) )
{
}

CExtGridCellStringDM::CExtGridCellStringDM(
	CExtGridCellStringDM & other
	)
	: m_strData( LPTSTR(NULL) )
{
	(*this) = other;
}

CExtGridCellStringDM::CExtGridCellStringDM(
	CExtGridCell & other
	)
	: m_strData( LPTSTR(NULL) )
{
	//(*this) = other;
	Assign( other );
}

CExtGridCellStringDM::~CExtGridCellStringDM()
{
	_StringBufferFree();
}

#ifdef _DEBUG
void CExtGridCellStringDM::AssertValid() const
{
	CExtGridCell::AssertValid();
}
void CExtGridCellStringDM::Dump( CDumpContext & dc ) const
{
	CExtGridCell::Dump( dc );
}
#endif // _DEBUG

void CExtGridCellStringDM::Serialize( CArchive & ar )
{
	CExtGridCell::Serialize( ar );
	if( ar.IsStoring() )
	{
		CExtSafeString sTmp;
		TextGet( sTmp );
		ar << sTmp;
	} // if( ar.IsStoring() )
	else
	{
		CExtSafeString sTmp;
		ar >> sTmp;
		TextSet( sTmp );
	} // else from if( ar.IsStoring() )
}

bool CExtGridCellStringDM::IsEmpty() const
{
	ASSERT_VALID( this );
	if(		LPCTSTR(m_strData) != NULL
		&&	_tcslen( LPCTSTR(m_strData) ) > 0
		)
		return false;
	return CExtGridCell::IsEmpty();
}

void CExtGridCellStringDM::_StringBufferFree()
{
	ASSERT_VALID( this );
	if( LPCTSTR(m_strData) == NULL )
		return;
CExtGridDataProvider * pDP = DataProviderGet();
	if( pDP == NULL )
	{
		ASSERT( FALSE );
		::AfxThrowMemoryException();
	} // if( pDP == NULL )
	pDP->StringFree( m_strData );
	m_strData = LPTSTR(NULL);
}

void CExtGridCellStringDM::_StringBufferSet( __EXT_MFC_SAFE_LPCTSTR strBuf )
{
	ASSERT_VALID( this );
LPCTSTR strOwn = m_strData;
LPCTSTR strOther = strBuf;
	if( strOwn == strOther )
		return;
	_StringBufferFree();
	if( strOther == NULL )
		return;
CExtGridDataProvider * pDP = DataProviderGet();
	if( pDP == NULL )
	{
		ASSERT( FALSE );
		::AfxThrowMemoryException();
	} // if( pDP == NULL )
	m_strData = pDP->StringAlloc( strBuf );
	ASSERT( m_strData != NULL );
}

void CExtGridCellStringDM::Empty()
{
	ASSERT_VALID( this );
	_StringBufferFree();
	CExtGridCell::Empty();
}

void CExtGridCellStringDM::Assign( const CExtGridCell & other )
{
	ASSERT_VALID( this );
#ifdef _DEBUG
CExtGridDataProvider * pDebugOldDataProvider = DataProviderGet();
#endif // _DEBUG
	CExtGridCell::_AssignM( other );
#ifdef _DEBUG
CExtGridDataProvider * pDebugNewDataProvider = DataProviderGet();
	// data provider object must be the same
	// or changed from NULL to not-NULL
	ASSERT(
			pDebugOldDataProvider == pDebugNewDataProvider
		||	(	pDebugOldDataProvider == NULL
			&&	pDebugNewDataProvider != NULL
			)
		);
#endif // _DEBUG
__EXT_MFC_SAFE_LPTSTR strBuf = LPTSTR(LPCTSTR(other.GetTextBuffer()));
	if( strBuf != NULL )
	{
		if( strBuf[0] != _T('\0') )
			_StringBufferSet( LPCTSTR(strBuf) );
		return;
	} // if( strBuf != NULL )
CExtSafeString strCopy;
	other.TextGet( strCopy );
	if( ! strCopy.IsEmpty() )
		_StringBufferSet( LPCTSTR(strCopy) );
	else
		_StringBufferFree();
}

void CExtGridCellStringDM::TextGet( CExtSafeString & strCopy ) const
{
	ASSERT_VALID( this );
	if( LPCTSTR(m_strData) == NULL )
		strCopy.Empty();
	else
		strCopy = LPCTSTR(m_strData);
}

void CExtGridCellStringDM::TextSet(
	__EXT_MFC_SAFE_LPCTSTR str, // = __EXT_MFC_SAFE_LPCTSTR(NULL) // empty text
	bool bAllowChangeDataType // = false
	)
{
	ASSERT_VALID( this );
	bAllowChangeDataType;
	_StringBufferSet( str );
}

void CExtGridCellStringDM::GetVariant( VARIANT & varCopy ) const
{
	ASSERT_VALID( this );
	::VariantClear( &varCopy );
	if( LPCTSTR(m_strData) == NULL )
		return;
USES_CONVERSION;
BSTR bstrCopy = ::SysAllocString( T2COLE(LPCTSTR(m_strData)) );
	if( bstrCopy == NULL )
		return;
	varCopy.vt = VT_BSTR;
	varCopy.bstrVal = bstrCopy;
}

__EXT_MFC_SAFE_LPCTSTR CExtGridCellStringDM::GetTextBuffer() const
{
	ASSERT_VALID( this );
	if( LPCTSTR(m_strData) == NULL )
		return __EXT_MFC_SAFE_LPCTSTR( LPCTSTR(_T("")) );
	return __EXT_MFC_SAFE_LPCTSTR( LPCTSTR(m_strData) );
}

/////////////////////////////////////////////////////////////////////////////
// CExtGridCellHeader

IMPLEMENT_DYNCREATE( CExtGridCellHeader, CExtGridCellStringDM );

CExtGridCellHeader::CExtGridCellHeader(
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGCE < CExtGridCellStringDM >( pDataProvider )
{
	ModifyStyle( __EGCS_TEXT_ELLIPSIS );
}

CExtGridCellHeader::CExtGridCellHeader( CExtGridCell & other )
	: CExtGCE < CExtGridCellStringDM >( other )
{
}

/////////////////////////////////////////////////////////////////////////////
// CExtGridCellColor

IMPLEMENT_DYNCREATE( CExtGridCellColor, CExtGridCell );

CExtGridCellColor::CExtGridCellColor(
	CExtGridDataProvider * pDataProvider, // = NULL
	COLORREF clr, // = RGB(0,0,0)
	CSize sizeIcon, // = CSize( 32, 16 )
	bool bEnableButtonRandomColor, // = false
	bool bEnableButtonCustomColor // = true
	)
	: CExtGridCell( pDataProvider )
	, m_PackedColor( clr )
	, m_sizeIcon( sizeIcon )
	, m_bEnableButtonRandomColor( bEnableButtonRandomColor )
	, m_bEnableButtonCustomColor( bEnableButtonCustomColor )
{
}

CExtGridCellColor::CExtGridCellColor(
	CExtGridDataProvider * pDataProvider,
	INT nSysColorIdx,
	CSize sizeIcon, // = CSize( 32, 16 )
	bool bEnableButtonRandomColor, // = false
	bool bEnableButtonCustomColor // = true
	)
	: CExtGridCell( pDataProvider )
	, m_PackedColor( nSysColorIdx )
	, m_sizeIcon( sizeIcon )
	, m_bEnableButtonRandomColor( bEnableButtonRandomColor )
	, m_bEnableButtonCustomColor( bEnableButtonCustomColor )
{
}

CExtGridCellColor::CExtGridCellColor(
	CExtGridDataProvider * pDataProvider,
	CExtGridCellColor::CPackedColor & PackedColor,
	CSize sizeIcon, // = CSize( 32, 16 )
	bool bEnableButtonRandomColor, // = false
	bool bEnableButtonCustomColor // = true
	)
	: CExtGridCell( pDataProvider )
	, m_PackedColor( PackedColor )
	, m_sizeIcon( sizeIcon )
	, m_bEnableButtonRandomColor( bEnableButtonRandomColor )
	, m_bEnableButtonCustomColor( bEnableButtonCustomColor )
{
}

CExtGridCellColor::CExtGridCellColor( CExtGridCell & other )
	: CExtGridCell( other )
{
}

#ifdef _DEBUG
void CExtGridCellColor::AssertValid() const
{
	CExtGridCell::AssertValid();
}
#endif // _DEBUG

void CExtGridCellColor::Assign( const CExtGridCell & other )
{
	ASSERT_VALID( this );
	CExtGridCell::Assign( other );
CExtGridCellColor * pCell =
		DYNAMIC_DOWNCAST(
			CExtGridCellColor,
			( const_cast < CExtGridCell * > ( &other ) )
			);
	if( pCell != NULL )
	{
		m_PackedColor = pCell->m_PackedColor;
		m_sizeIcon = pCell->m_sizeIcon;
		m_bEnableButtonRandomColor = pCell->m_bEnableButtonRandomColor;
		m_bEnableButtonCustomColor = pCell->m_bEnableButtonCustomColor;
	} // if( pCell != NULL )
	else
	{
		m_PackedColor.Clear();
		m_sizeIcon = CSize( 32, 16 );
		m_bEnableButtonRandomColor = false;
		m_bEnableButtonCustomColor = true;
	} // else if( pCell != NULL )
}

void CExtGridCellColor::Serialize( CArchive & ar )
{
	ASSERT_VALID( this );
	CExtGridCell::Serialize( ar );
	m_PackedColor.Serialize( ar );
DWORD dwSerializeFlags = 0L;
	if( ar.IsStoring() )
	{
		ar << m_sizeIcon;
		if( m_bEnableButtonRandomColor )
			dwSerializeFlags |= 0x00000001L;
		if( m_bEnableButtonCustomColor )
			dwSerializeFlags |= 0x00000002L;
		ar << dwSerializeFlags;
	} // if( ar.IsStoring() )
	else
	{
		ar >> m_sizeIcon;
		ar >> dwSerializeFlags;
		if( (dwSerializeFlags&0x00000001L) != 0L )
			m_bEnableButtonRandomColor = true;
		else
			m_bEnableButtonRandomColor = false;
		if( (dwSerializeFlags&0x00000002L) != 0L )
			m_bEnableButtonCustomColor = true;
		else
			m_bEnableButtonCustomColor = false;
	} // else from if( ar.IsStoring() );
}

CSize CExtGridCellColor::IconGetSize() const
{
	ASSERT_VALID( this );
	return m_sizeIcon;
}

void CExtGridCellColor::OnPaintIcon(
	const CExtCmdIcon * pIcon,
	const RECT & rcIcon,
	const CExtGridWnd & wndGrid,
	CDC & dc,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	const RECT & rcVisibleRange,
	DWORD dwAreaFlags,
	DWORD dwHelperPaintFlags
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	ASSERT( dc.GetSafeHdc() != NULL );
	pIcon;
	wndGrid;
	nVisibleColNo;
	nVisibleRowNo;
	nColNo;
	nRowNo;
	nColType;
	nRowType;
	rcCellExtra;
	rcCell;
	rcVisibleRange;
	dwAreaFlags;
	dwHelperPaintFlags;
COLORREF clr = m_PackedColor.GetColor();
	dc.FillSolidRect( &rcIcon, clr );
}

void CExtGridCellColor::OnButtonPressed(
	CExtGridWnd & wndGrid,
	INT nButtonType,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&wndGrid) );
	if( nButtonType != __EBTT_DROPDOWN )
	{
		CExtGridCell::OnButtonPressed(
			wndGrid,
			nButtonType,
			rcCellExtra,
			rcCell,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			nColType,
			nRowType
			);
		return;
	} // if( nButtonType != __EBTT_DROPDOWN )
	wndGrid.InvalidateRect( &rcCellExtra );
	wndGrid.UpdateWindow();
CExtPopupColorMenuWnd * pPopup = new CExtPopupColorMenuWnd;
	if( ! pPopup->CreatePopupMenu( wndGrid.GetSafeHwnd() ) )
	{
		ASSERT( FALSE );
		return;
	}
	if( m_bEnableButtonRandomColor )
	{
		pPopup->m_sBtnTextColorDefault = _T("Random Color");
		pPopup->m_bEnableBtnColorDefault = true;
	}
	else
		pPopup->m_bEnableBtnColorDefault = false;
	pPopup->m_bEnableBtnColorCustom = m_bEnableButtonCustomColor;
	pPopup->m_clrDefault = COLORREF( -100L );
	pPopup->m_clrInitial = m_PackedColor.GetColor();
CRect wr( rcCell );
	wndGrid.ClientToScreen( &wr );
CPoint pt( wr.CenterPoint() );
COLORREF clrValueFinal = COLORREF( CExtPopupColorMenuWnd::__ECST_NONE );
	pPopup->m_pClrValueFinal = &clrValueFinal;
	if( !pPopup->TrackPopupMenu(
			TPMX_TOPALIGN
				|TPMX_DO_MESSAGE_LOOP
				|TPMX_COMBINE_NONE,
			pt.x,
			pt.y,
			&wr,
			NULL,
			NULL,
			NULL,
			false
			)
		)
	{
		delete pPopup;
		ASSERT( FALSE );
		return;
	}
	if( clrValueFinal == CExtPopupColorMenuWnd::__ECST_NONE )
		return;
	if( clrValueFinal == CExtPopupColorMenuWnd::__ECST_BUTTON_CUSTOM )
	{
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		clrValueFinal = m_PackedColor.GetColor();
CExtLocalResourceHelper _LRH;
		CExtColorDlg dlg( clrValueFinal, clrValueFinal, &wndGrid );
		if( dlg.DoModal() != IDOK )
			return;
		clrValueFinal = dlg.m_clrNew;
#else // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		CColorDialog dlg( clrValueFinal, CC_FULLOPEN, &wndGrid );
		if( dlg.DoModal() != IDOK )
			return;
		clrValueFinal = dlg.m_cc.rgbResult;
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
		m_PackedColor.SetColor( clrValueFinal );
	} // if( clrValueFinal == CExtPopupColorMenuWnd::__ECST_BUTTON_CUSTOM )
	else if( clrValueFinal == COLORREF( -100L ) )
	{
		m_PackedColor.SetRandomizedColor( 32, 244, 32, 244, 32, 244 );
	} // else if( clrValueFinal == COLORREF( -100L ) )
	else
		m_PackedColor.SetColor( clrValueFinal );
	wndGrid.InvalidateRect( &rcCellExtra );
}

/////////////////////////////////////////////////////////////////////////////
// CExtGridCellVariant

IMPLEMENT_DYNCREATE( CExtGridCellVariant, CExtGridCell );

#pragma message("   Automatically linking with oleaut32.lib")
#pragma message("      (OLE automation)")
#pragma comment( lib, "oleaut32.lib" ) 

CExtGridCellVariant::CExtGridCellVariant(
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
}

CExtGridCellVariant::CExtGridCellVariant(
	const CExtGridCellVariant & other
	)
{
	_VariantInit();
	_AssignA( other );
}

CExtGridCellVariant::CExtGridCellVariant(
	const CExtGridCell & other
	)
{
	_VariantInit();
	if( other.IsKindOf( RUNTIME_CLASS(CExtGridCellVariant) ) )
		_AssignA(
			static_cast < const CExtGridCellVariant & > ( other )
			);
	else
		CExtGridCell::Assign( other );
}

CExtGridCellVariant::CExtGridCellVariant(
	const VARIANT & other,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( other );
}

CExtGridCellVariant::CExtGridCellVariant(
	IDispatch * pInterface,
	bool bAllowNullInterface, // = true
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( pInterface, bAllowNullInterface );
}

CExtGridCellVariant::CExtGridCellVariant(
	IUnknown * pInterface,
	bool bAllowNullInterface, // = true
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( pInterface, bAllowNullInterface );
}

CExtGridCellVariant::CExtGridCellVariant(
	IDispatch ** ppInterface,
	bool bAllowNullInterface, // = true
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( ppInterface, bAllowNullInterface );
}

CExtGridCellVariant::CExtGridCellVariant(
	IUnknown ** ppInterface,
	bool bAllowNullInterface, // = true
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( ppInterface, bAllowNullInterface );
}

CExtGridCellVariant::CExtGridCellVariant(
	const SAFEARRAY * pSA,
	bool bAllowNullArray, // = true
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( pSA, bAllowNullArray );
}

CExtGridCellVariant::CExtGridCellVariant(
	const SAFEARRAY ** ppSA,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( ppSA );
}

CExtGridCellVariant::CExtGridCellVariant(
	const DECIMAL & decimalVal,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( decimalVal );
}

CExtGridCellVariant::CExtGridCellVariant(
	const DECIMAL ** decimalVal,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( decimalVal );
}

//CExtGridCellVariant::CExtGridCellVariant( BSTR bstrOther, CExtGridDataProvider * pDataProvider /* = NULL*/ )
//	: CExtGridCell( pDataProvider )
//{
//	_VariantInit();
//	VERIFY( _VariantAssign( bstrOther ) );
//}

//CExtGridCellVariant::CExtGridCellVariant( BSTR * bstrOther, CExtGridDataProvider * pDataProvider /* = NULL*/ )
//	: CExtGridCell( pDataProvider )
//{
//	_VariantInit();
//	VERIFY( _VariantAssign( bstrOther ) );
//}

CExtGridCellVariant::CExtGridCellVariant(
	const char * strOther,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	VERIFY( _VariantAssign( strOther ) );
}

CExtGridCellVariant::CExtGridCellVariant(
	const unsigned short * strOther,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	VERIFY( _VariantAssign( strOther ) );
}

#if _MFC_VER >= 0x700
#if (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
CExtGridCellVariant::CExtGridCellVariant(
	const __wchar_t * strOther,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	VERIFY( _VariantAssign( strOther ) );
}
#endif // (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
#endif // _MFC_VER >= 0x700

CExtGridCellVariant::CExtGridCellVariant(
	bool bSrc,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( bSrc );
}

CExtGridCellVariant::CExtGridCellVariant(
	signed char nSrc,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc );
}

CExtGridCellVariant::CExtGridCellVariant(
	signed char * nSrc,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc );
}

CExtGridCellVariant::CExtGridCellVariant(
	unsigned char nSrc,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc );
}

CExtGridCellVariant::CExtGridCellVariant(
	unsigned char * nSrc,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc );
}

CExtGridCellVariant::CExtGridCellVariant(
	signed short nSrc,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc );
}

CExtGridCellVariant::CExtGridCellVariant(
	signed short * nSrc,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc );
}

CExtGridCellVariant::CExtGridCellVariant(
	unsigned short nSrc,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc );
}

CExtGridCellVariant::CExtGridCellVariant(
	unsigned short * nSrc,
	VARTYPE nVariantType, // = VT_UI2 // VT_UI2 or VT_BSTR only (VT_BYREF for VT_UI2 will be added automatically)
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc, nVariantType );
}

#if _MFC_VER >= 0x700
#if (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
CExtGridCellVariant::CExtGridCellVariant(
	__wchar_t * nSrc,
	VARTYPE nVariantType, // = VT_UI2 // VT_UI2 or VT_BSTR only (VT_BYREF for VT_UI2 will be added automatically)
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc, nVariantType );
}
#endif // (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
#endif // _MFC_VER >= 0x700

CExtGridCellVariant::CExtGridCellVariant(
	signed int nSrc,
	VARTYPE nVariantType, // = VT_I4 // VT_I4 or VT_INT only
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc, nVariantType );
}

CExtGridCellVariant::CExtGridCellVariant(
	signed int * nSrc,
	VARTYPE nVariantType, // = VT_I4 // VT_I4 or VT_INT only (VT_BYREF will be added automatically)
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc, nVariantType );
}

CExtGridCellVariant::CExtGridCellVariant(
	unsigned int nSrc,
	VARTYPE nVariantType, // = VT_UI4 // VT_UI4 or VT_UINT only
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc, nVariantType );
}

CExtGridCellVariant::CExtGridCellVariant(
	unsigned int * nSrc,
	VARTYPE nVariantType, // = VT_UI4 // VT_UI4 or VT_UINT only (VT_BYREF will be added automatically)
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc, nVariantType );
}

CExtGridCellVariant::CExtGridCellVariant(
	signed long nSrc,
	VARTYPE nVariantType, // = VT_I4 // VT_I4 or VT_ERROR only
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc, nVariantType );
}

CExtGridCellVariant::CExtGridCellVariant(
	signed long * nSrc,
	VARTYPE nVariantType, // = VT_I4 // VT_I4 or VT_ERROR only (VT_BYREF will be added automatically)
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc, nVariantType );
}

CExtGridCellVariant::CExtGridCellVariant(
	unsigned long nSrc,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc );
}

CExtGridCellVariant::CExtGridCellVariant(
	unsigned long * nSrc,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc );
}

CExtGridCellVariant::CExtGridCellVariant(
	signed __int64 nSrc,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc );
}

CExtGridCellVariant::CExtGridCellVariant(
	signed __int64 * nSrc,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc );
}

CExtGridCellVariant::CExtGridCellVariant(
	unsigned __int64 nSrc,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc );
}

CExtGridCellVariant::CExtGridCellVariant(
	unsigned __int64 * nSrc,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc );
}

CExtGridCellVariant::CExtGridCellVariant(
	float nSrc,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc );
}

CExtGridCellVariant::CExtGridCellVariant(
	float * nSrc,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc );
}

CExtGridCellVariant::CExtGridCellVariant(
	double nSrc,
	VARTYPE nVariantType, // = VT_R8 // VT_R8 or VT_DATE only
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc, nVariantType );
}

CExtGridCellVariant::CExtGridCellVariant(
	double * nSrc,
	VARTYPE nVariantType, // = VT_R8 // VT_R8 or VT_DATE only (VT_BYREF will be added automatically)
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( nSrc, nVariantType );
}

CExtGridCellVariant::CExtGridCellVariant(
	CY cySrc,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( cySrc );
}

CExtGridCellVariant::CExtGridCellVariant(
	CY * cySrc,
	CExtGridDataProvider * pDataProvider // = NULL
	)
	: CExtGridCell( pDataProvider )
{
	_VariantInit();
	_VariantAssign( cySrc );
}

CExtGridCellVariant::~CExtGridCellVariant()
{
	_VariantClear( true );
}

void CExtGridCellVariant::Serialize( CArchive & ar )
{
	CExtGridCell::Serialize( ar );
	CExtGridCell::stat_VariantSerialize( this, ar );
}

bool CExtGridCellVariant::IsEmpty() const
{
	ASSERT_VALID( this );
	if( vt == VT_EMPTY )
		return true;
	return false;
}

void CExtGridCellVariant::Empty()
{
	ASSERT_VALID( this );
	CExtGridCell::Empty();
	_VariantClear( true );
}

signed __int64 & CExtGridCellVariant::_GetInt64Ref()
{
	ASSERT_VALID( this );
signed __int64 * ptr =
		reinterpret_cast < signed __int64 * > ( &lVal );
	return *ptr;
}

unsigned __int64 & CExtGridCellVariant::_GetUInt64Ref()
{
	ASSERT_VALID( this );
unsigned __int64 * ptr =
		reinterpret_cast < unsigned __int64 * > ( &ulVal );
	return *ptr;
}

signed __int64 *& CExtGridCellVariant::_GetInt64PtrRef()
{
	ASSERT_VALID( this );
signed __int64 *& ptr_ref =
		reinterpret_cast < signed __int64 *& > ( plVal );
	return ptr_ref;
}

unsigned __int64 *& CExtGridCellVariant::_GetUInt64PtrRef()
{
	ASSERT_VALID( this );
unsigned __int64 *& ptr_ref =
		reinterpret_cast < unsigned __int64 *& > ( pulVal );
	return ptr_ref;
}

void CExtGridCellVariant::_VariantInit()
{
	ASSERT_VALID( this );
	::VariantInit( this );
}

bool CExtGridCellVariant::_VariantClear(
	bool bConvertToErrorIfFailed // = true
	)
{
	ASSERT_VALID( this );
HRESULT hr =
		::VariantClear( this );
	if( FAILED( hr ) )
	{
		ASSERT( FALSE );
		if( bConvertToErrorIfFailed )
		{
			vt = VT_ERROR;
			scode = hr;
		} // bConvertToErrorIfFailed
		else
			vt = VT_EMPTY;
		return false;
	}
	return true;
}

bool CExtGridCellVariant::_VariantAssign(
	const VARIANT & other,
	bool bConvertToErrorIfFailed // = true
	)
{
	ASSERT_VALID( this );
VARIANT * pVariant1 = const_cast < VARIANT * > ( &other );
	if( this == pVariant1 )
		return true;
	_VariantClear( false );
HRESULT hr =
		::VariantCopy(
			this,
			pVariant1
			);
	if( FAILED( hr ) )
	{
		if( bConvertToErrorIfFailed )
		{
			_VariantClear( false );
			vt = VT_ERROR;
			scode = hr;
		} // if( bConvertToErrorIfFailed )
		return false;
	} // if( FAILED( hr ) )
	return true;
}

void CExtGridCellVariant::_VariantAssign(
	IDispatch * pInterface,
	bool bAllowNullInterface // = true
	)
{
	ASSERT_VALID( this );
	_VariantClear();
	if( pInterface != NULL || bAllowNullInterface )
	{
		if( pInterface == NULL )
		{
			ASSERT( bAllowNullInterface );
			vt = VT_DISPATCH;
			pdispVal = NULL;
		} // if( pInterface == NULL )
		else
		{
			IDispatch * _pdispVal = NULL;
			HRESULT hr =
				pInterface->QueryInterface(
					IID_IDispatch,
					(LPVOID*)&_pdispVal
					);
			ASSERT( SUCCEEDED( hr ) );
			if( SUCCEEDED( hr ) )
			{
				ASSERT( _pdispVal != NULL );
				vt = VT_DISPATCH;
				pdispVal = _pdispVal;
			} // if( SUCCEEDED( hr ) )
		} // else from if( pInterface == NULL )
	} // if( pInterface != NULL || bAllowNullInterface )
}

void CExtGridCellVariant::_VariantAssign(
	IUnknown * pInterface,
	bool bAllowNullInterface // = true
	)
{
	ASSERT_VALID( this );
	_VariantClear();
	if( pInterface != NULL || bAllowNullInterface )
	{
		if( pInterface == NULL )
		{
			ASSERT( bAllowNullInterface );
			vt = VT_UNKNOWN;
			punkVal = NULL;
		} // if( pInterface == NULL )
		else
		{
			IUnknown * _punkVal = NULL;
			HRESULT hr =
				pInterface->QueryInterface(
					IID_IUnknown,
					(LPVOID*)&_punkVal
					);
			ASSERT( SUCCEEDED( hr ) );
			if( SUCCEEDED( hr ) )
			{
				ASSERT( _punkVal != NULL );
				vt = VT_UNKNOWN;
				punkVal = _punkVal;
			} // if( SUCCEEDED( hr ) )
		} // else from if( pInterface == NULL )
	} // if( pInterface != NULL || bAllowNullInterface )
}

void CExtGridCellVariant::_VariantAssign(
	IDispatch ** ppInterface,
	bool bAllowNullInterface // = true
	)
{
	ASSERT_VALID( this );
	_VariantClear();
	if( ppInterface != NULL && ((*ppInterface) != NULL) || bAllowNullInterface )
	{
		vt = VT_DISPATCH|VT_BYREF;
		ppdispVal = ppInterface;
		if( ppInterface != NULL && (*ppdispVal) != NULL )
			(*ppdispVal)->AddRef();
	} // if( ppInterface != NULL && ((*ppInterface) != NULL) || bAllowNullInterface )
}

void CExtGridCellVariant::_VariantAssign(
	IUnknown ** ppInterface,
	bool bAllowNullInterface // = true
	)
{
	ASSERT_VALID( this );
	_VariantClear();
	if( ppInterface != NULL && ((*ppInterface) != NULL) || bAllowNullInterface )
	{
		vt = VT_UNKNOWN|VT_BYREF;
		ppunkVal = ppInterface;
		if( ppunkVal != NULL && (*ppunkVal) != NULL )
			(*ppunkVal)->AddRef();
	} // if( ppInterface != NULL && ((*ppInterface) != NULL) || bAllowNullInterface )
}

void CExtGridCellVariant::_VariantAssign(
	const SAFEARRAY * pSA,
	bool bAllowConvertToError // = true
	)
{
	ASSERT_VALID( this );
	_VariantClear();
	if( pSA == NULL)
		return;
LPSAFEARRAY _pSA = NULL;
HRESULT hr = ::SafeArrayCopy( (LPSAFEARRAY)pSA, &_pSA );
	if( SUCCEEDED( hr ) && _pSA != NULL )
	{
		if( g_pSafeArrayGetVartype == NULL )
		{
			HINSTANCE hOleAut32 =
				::GetModuleHandle( _T("OLEAUT32.DLL") );
			ASSERT( hOleAut32 != NULL );
			if( hOleAut32 == NULL )
			{
				if( bAllowConvertToError )
				{
					vt = VT_ERROR;
					scode = E_FAIL;
				} // if( bAllowConvertToError )
				return;
			}
			g_pSafeArrayGetVartype = (p_SafeArrayGetVartype_t)
				::GetProcAddress(
					hOleAut32,
					"SafeArrayGetVartype"
					);
			ASSERT( g_pSafeArrayGetVartype != NULL );
			if( g_pSafeArrayGetVartype == NULL )
			{
				if( bAllowConvertToError )
				{
					vt = VT_ERROR;
					scode = E_FAIL;
				} // if( bAllowConvertToError )
				return;
			}
		} // if( g_pSafeArrayGetVartype == NULL )
#ifdef _DEBUG
		hr =
#endif // _DEBUG
			g_pSafeArrayGetVartype( (LPSAFEARRAY)pSA, &vt );
		ASSERT( SUCCEEDED( hr ) );
		vt |= VT_ARRAY;
		parray = _pSA;
	} // if( SUCCEEDED( hr ) && _pSA != NULL )
	else
	{
		if( bAllowConvertToError )
		{
			vt = VT_ERROR;
			scode = hr;
		} // if( bAllowConvertToError )
	} // else from if( SUCCEEDED( hr ) && _pSA != NULL )
}

void CExtGridCellVariant::_VariantAssign(
	const SAFEARRAY ** ppSA
	)
{
	ASSERT_VALID( this );
	_VariantClear();
	if( ppSA == NULL || (*ppSA) == NULL )
		return;
	if( g_pSafeArrayGetVartype == NULL )
	{
		HINSTANCE hOleAut32 =
			::GetModuleHandle( _T("OLEAUT32.DLL") );
		ASSERT( hOleAut32 != NULL );
		if( hOleAut32 == NULL )
			return;
		g_pSafeArrayGetVartype = (p_SafeArrayGetVartype_t)
			::GetProcAddress(
				hOleAut32,
				"SafeArrayGetVartype"
				);
		ASSERT( g_pSafeArrayGetVartype != NULL );
		if( g_pSafeArrayGetVartype == NULL )
			return;
	} // if( g_pSafeArrayGetVartype == NULL )
LPSAFEARRAY * _ppSA = (LPSAFEARRAY*)ppSA;
#ifdef _DEBUG
HRESULT hr =
#endif // _DEBUG
		g_pSafeArrayGetVartype( *_ppSA, &vt );
	ASSERT( SUCCEEDED( hr ) );
	vt |= VT_ARRAY|VT_BYREF;
	pparray = _ppSA;
}

void CExtGridCellVariant::_VariantAssign( const DECIMAL & decimalVal )
{
	ASSERT_VALID( this );
	_VariantClear();
	vt = VT_DECIMAL;
	::memcpy( &decVal, &decimalVal, sizeof(DECIMAL) );
}

void CExtGridCellVariant::_VariantAssign( const DECIMAL ** decimalVal )
{
	ASSERT_VALID( this );
	_VariantClear();
	if( decimalVal == NULL || (*decimalVal) == NULL )
		return;
	vt = VT_DECIMAL|VT_BYREF;
	pdecVal = (DECIMAL*)(*decimalVal);
}

//bool CExtGridCellVariant::_VariantAssign( BSTR bstrOther )
//{
//	ASSERT_VALID( this );
//	_VariantClear();
//	vt = VT_BSTR;
//	bstrVal = ::SysAllocString( bstrOther );
//	if( bstrVal == NULL && bstrOther != NULL )
//	{
//		ASSERT( FALSE );
//		vt = VT_ERROR;
//		scode = E_OUTOFMEMORY;
//		return false;
//	} // if( bstrVal == NULL && bstrOther != NULL )
//	return true;
//}

//bool CExtGridCellVariant::_VariantAssign( BSTR * bstrOther )
//{
//	ASSERT_VALID( this );
//	_VariantClear();
//	vt = VT_BSTR|VT_BYREF;
//	pbstrVal = bstrOther;
//	return true;
//}

bool CExtGridCellVariant::_VariantAssign( const char * strOther )
{
	ASSERT_VALID( this );
	_VariantClear();
	vt = VT_BSTR;
USES_CONVERSION;
	bstrVal = ::SysAllocString( A2OLE( strOther ) );
	if( bstrVal == NULL && strOther != NULL )
	{
		ASSERT( FALSE );
		vt = VT_ERROR;
		scode = E_OUTOFMEMORY;
		return false;
	} // if( bstrVal == NULL && strOther != NULL )
	return true;
}

bool CExtGridCellVariant::_VariantAssign( const unsigned short * strOther )
{
	ASSERT_VALID( this );
	_VariantClear();
	vt = VT_BSTR;
USES_CONVERSION;
	bstrVal = ::SysAllocString( W2OLE( ((wchar_t *)strOther) ) );
	if( bstrVal == NULL && strOther != NULL )
	{
		ASSERT( FALSE );
		vt = VT_ERROR;
		scode = E_OUTOFMEMORY;
		return false;
	} // if( bstrVal == NULL && strOther != NULL )
	return true;
}

#if _MFC_VER >= 0x700
#if (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
bool CExtGridCellVariant::_VariantAssign( const __wchar_t * strOther )
{
	ASSERT_VALID( this );
	_VariantClear();
	vt = VT_BSTR;
USES_CONVERSION;
	bstrVal = ::SysAllocString( W2OLE( ((wchar_t *)strOther) ) );
	if( bstrVal == NULL && strOther != NULL )
	{
		ASSERT( FALSE );
		vt = VT_ERROR;
		scode = E_OUTOFMEMORY;
		return false;
	} // if( bstrVal == NULL && strOther != NULL )
	return true;
}
#endif // (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
#endif // _MFC_VER >= 0x700

void CExtGridCellVariant::_VariantAssign( bool bSrc )
{
	ASSERT_VALID( this );
	if( vt != VT_BOOL )
	{
		_VariantClear();
		vt = VT_BOOL;
	} // if( vt != VT_BOOL )
#pragma warning(disable: 4310)
	boolVal = bSrc ? VARIANT_TRUE : VARIANT_FALSE;
#pragma warning(default: 4310)
}

void CExtGridCellVariant::_VariantAssign( signed char nSrc )
{
	ASSERT_VALID( this );
	if( vt != VT_I1 )
	{
		_VariantClear();
		vt = VT_I1;
	} // if( vt != VT_I1 )
	cVal = (CHAR)nSrc;
}

void CExtGridCellVariant::_VariantAssign( signed char * nSrc )
{
	ASSERT_VALID( this );
	if( vt != (VT_I1|VT_BYREF) )
	{
		_VariantClear();
		vt = (VT_I1|VT_BYREF);
	} // if( vt != (VT_I1|VT_BYREF) )
	pcVal = (CHAR*)nSrc;
}

void CExtGridCellVariant::_VariantAssign( unsigned char nSrc )
{
	ASSERT_VALID( this );
	if( vt != VT_UI1 )
	{
		_VariantClear();
		vt = VT_UI1;
	} // if( vt != VT_UI1 )
	bVal = (BYTE)nSrc;
}

void CExtGridCellVariant::_VariantAssign( unsigned char * nSrc )
{
	ASSERT_VALID( this );
	if( vt != (VT_UI1|VT_BYREF) )
	{
		_VariantClear();
		vt = (VT_UI1|VT_BYREF);
	} // if( vt != (VT_UI1|VT_BYREF) )
	pbVal = (BYTE*)nSrc;
}

void CExtGridCellVariant::_VariantAssign( signed short nSrc )
{
	ASSERT_VALID( this );
	if( vt != VT_I2 )
	{
		_VariantClear();
		vt = VT_I2;
	} // if( vt != VT_I2 )
	iVal = nSrc;
}

void CExtGridCellVariant::_VariantAssign( signed short * nSrc )
{
	ASSERT_VALID( this );
	if( vt != (VT_I2|VT_BYREF) )
	{
		_VariantClear();
		vt = (VT_I2|VT_BYREF);
	} // if( vt != (VT_I2|VT_BYREF) )
	piVal = nSrc;
}

void CExtGridCellVariant::_VariantAssign( unsigned short nSrc )
{
	ASSERT_VALID( this );
	if( vt != VT_UI2 )
	{
		_VariantClear();
		vt = VT_UI2;
	} // if( vt != VT_UI2 )
	uiVal = nSrc;
}

void CExtGridCellVariant::_VariantAssign(
	unsigned short * nSrc,
	VARTYPE nVariantType // = VT_UI2 // VT_UI2 or VT_BSTR only (VT_BYREF for VT_UI2 will be added automatically)
	)
{
	ASSERT_VALID( this );
	ASSERT(
			nVariantType == VT_BSTR
		||	(nVariantType & (~(VT_BYREF))) == VT_UI2
		);
	if( nVariantType == VT_BSTR )
	{
		const unsigned short * strOther = nSrc;
		VERIFY( _VariantAssign( strOther ) );
		ASSERT( vt == VT_BSTR );
		return;
	} // if( nVariantType == VT_BSTR )
	if( vt != (VT_UI2|VT_BYREF) )
	{
		_VariantClear();
		vt = (VT_UI2|VT_BYREF);
	} // if( vt != (VT_UI2|VT_BYREF) )
	puiVal = nSrc;
}

#if _MFC_VER >= 0x700
#if (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
void CExtGridCellVariant::_VariantAssign(
	__wchar_t * nSrc,
	VARTYPE nVariantType // = VT_UI2 // VT_UI2 or VT_BSTR only (VT_BYREF for VT_UI2 will be added automatically)
	)
{
	ASSERT_VALID( this );
	ASSERT(
			nVariantType == VT_BSTR
		||	(nVariantType & (~(VT_BYREF))) == VT_UI2
		);
	if( nVariantType == VT_BSTR )
	{
		const unsigned short * strOther =
			(unsigned short *)nSrc;
		VERIFY( _VariantAssign( strOther ) );
		ASSERT( vt == VT_BSTR );
		return;
	} // if( nVariantType == VT_BSTR )
	if( vt != (VT_UI2|VT_BYREF) )
	{
		_VariantClear();
		vt = (VT_UI2|VT_BYREF);
	} // if( vt != (VT_UI2|VT_BYREF) )
	puiVal = (unsigned short *)nSrc;
}
#endif // (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
#endif // _MFC_VER >= 0x700

void CExtGridCellVariant::_VariantAssign(
	signed int nSrc,
	VARTYPE nVariantType // = VT_I4 // VT_I4 or VT_INT only
	)
{
	ASSERT_VALID( this );
	nVariantType &= ~(VT_BYREF);
	ASSERT(
			nVariantType == VT_I4
		||	nVariantType == VT_INT
		);
	if( vt != nVariantType )
	{
		_VariantClear();
		vt = nVariantType;
	} // if( vt != nVariantType )
	intVal = nSrc;
}

void CExtGridCellVariant::_VariantAssign(
	signed int * nSrc,
	VARTYPE nVariantType // = VT_I4 // VT_I4 or VT_INT only (VT_BYREF will be added automatically)
	)
{
	ASSERT_VALID( this );
	nVariantType |= VT_BYREF;
	ASSERT(
			nVariantType == (VT_I4|VT_BYREF)
		||	nVariantType == (VT_INT|VT_BYREF)
		);
	if( vt != nVariantType )
	{
		_VariantClear();
		vt = nVariantType;
	} // if( vt != nVariantType )
	pintVal = nSrc;
}

void CExtGridCellVariant::_VariantAssign(
	unsigned int nSrc,
	VARTYPE nVariantType // = VT_UI4 // VT_UI4 or VT_UINT only (VT_BYREF will be added automatically)
	)
{
	ASSERT_VALID( this );
	nVariantType &= ~(VT_BYREF);
	ASSERT(
			nVariantType == VT_UI4
		||	nVariantType == VT_UINT
		);
	if( vt != nVariantType )
	{
		_VariantClear();
		vt = nVariantType;
	} // if( vt != nVariantType )
	uintVal = nSrc;
}

void CExtGridCellVariant::_VariantAssign(
	unsigned int * nSrc,
	VARTYPE nVariantType // = VT_UI4 // VT_UI4 or VT_UINT only (VT_BYREF will be added automatically)
	)
{
	ASSERT_VALID( this );
	nVariantType |= VT_BYREF;
	ASSERT(
			nVariantType == (VT_UI4|VT_BYREF)
		||	nVariantType == (VT_UINT|VT_BYREF)
		);
	if( vt != nVariantType )
	{
		_VariantClear();
		vt = nVariantType;
	} // if( vt != nVariantType )
	puintVal = nSrc;
}

void CExtGridCellVariant::_VariantAssign(
	signed long nSrc,
	VARTYPE nVariantType // = VT_I4 // VT_I4 or VT_ERROR only
	)
{
	ASSERT_VALID( this );
	nVariantType &= ~(VT_BYREF);
	ASSERT(
			nVariantType == VT_I4
		||	nVariantType == VT_ERROR
		);
	if( vt != nVariantType )
	{
		_VariantClear();
		vt = nVariantType;
	} // if( vt != nVariantType )
	lVal = nSrc;
}

void CExtGridCellVariant::_VariantAssign(
	signed long * nSrc,
	VARTYPE nVariantType // = VT_I4 // VT_I4 or VT_ERROR only (VT_BYREF will be added automatically)
	)
{
	ASSERT_VALID( this );
	nVariantType |= VT_BYREF;
	ASSERT(
			nVariantType == (VT_I4|VT_BYREF)
		||	nVariantType == (VT_ERROR|VT_BYREF)
		);
	if( vt != nVariantType )
	{
		_VariantClear();
		vt = nVariantType;
	} // if( vt != nVariantType )
	plVal = nSrc;
}

void CExtGridCellVariant::_VariantAssign( unsigned long nSrc )
{
	ASSERT_VALID( this );
	if( vt != VT_UI4 )
	{
		_VariantClear();
		vt = VT_UI4;
	} // if( vt != VT_UI4 )
	ulVal = nSrc;
}

void CExtGridCellVariant::_VariantAssign( unsigned long * nSrc )
{
	ASSERT_VALID( this );
	if( vt != (VT_UI4|VT_BYREF) )
	{
		_VariantClear();
		vt = (VT_UI4|VT_BYREF);
	} // if( vt != (VT_UI4|VT_BYREF) )
	pulVal = nSrc;
}

void CExtGridCellVariant::_VariantAssign( signed __int64 nSrc )
{
	ASSERT_VALID( this );
	if( vt != VT_I8 )
	{
		_VariantClear();
		vt = VT_I8;
	} // if( vt != VT_I8 )
	_GetInt64Ref() = nSrc;
}

void CExtGridCellVariant::_VariantAssign( signed __int64 * nSrc )
{
	ASSERT_VALID( this );
	if( vt != (VT_I8|VT_BYREF) )
	{
		_VariantClear();
		vt = (VT_I8|VT_BYREF);
	} // if( vt != (VT_I8|VT_BYREF) )
	_GetInt64PtrRef() = nSrc;
}

void CExtGridCellVariant::_VariantAssign( unsigned __int64 nSrc )
{
	ASSERT_VALID( this );
	if( vt != VT_UI8 )
	{
		_VariantClear();
		vt = VT_UI8;
	} // if( vt != VT_UI8 )
	_GetUInt64Ref() = nSrc;
}

void CExtGridCellVariant::_VariantAssign( unsigned __int64 * nSrc )
{
	ASSERT_VALID( this );
	if( vt != (VT_UI8|VT_BYREF) )
	{
		_VariantClear();
		vt = (VT_UI8|VT_BYREF);
	} // if( vt != (VT_UI8|VT_BYREF) )
	_GetUInt64PtrRef() = nSrc;
}

void CExtGridCellVariant::_VariantAssign( float nSrc )
{
	ASSERT_VALID( this );
	if( vt != VT_R4 )
	{
		_VariantClear();
		vt = VT_R4;
	} // if( vt != VT_R4 )
	fltVal = nSrc;
}

void CExtGridCellVariant::_VariantAssign( float * nSrc )
{
	ASSERT_VALID( this );
	if( vt != (VT_R4|VT_BYREF) )
	{
		_VariantClear();
		vt = (VT_R4|VT_BYREF);
	} // if( vt != (VT_R4|VT_BYREF) )
	pfltVal = nSrc;
}

void CExtGridCellVariant::_VariantAssign(
	double nSrc,
	VARTYPE nVariantType // = VT_R8 // VT_R8 or VT_DATE only
	)
{
	ASSERT_VALID( this );
	nVariantType &= ~(VT_BYREF);
	ASSERT(
			nVariantType == VT_R8
		||	nVariantType == VT_DATE
		);
	if( vt != nVariantType )
	{
		_VariantClear();
		vt = nVariantType;
	} // if( vt != nVariantType )
	dblVal = nSrc;
}

void CExtGridCellVariant::_VariantAssign(
	double * nSrc,
	VARTYPE nVariantType // = VT_R8 // VT_R8 or VT_DATE only (VT_BYREF will be added automatically)
	)
{
	ASSERT_VALID( this );
	nVariantType |= VT_BYREF;
	ASSERT(
			nVariantType == (VT_R8|VT_BYREF)
		||	nVariantType == (VT_DATE|VT_BYREF)
		);
	if( vt != nVariantType )
	{
		_VariantClear();
		vt = nVariantType;
	} // if( vt != nVariantType )
	pdblVal = nSrc;
}

void CExtGridCellVariant::_VariantAssign( CY cySrc )
{
	ASSERT_VALID( this );
	if( vt != VT_CY )
	{
		_VariantClear();
		vt = VT_CY;
	} // if( vt != VT_CY )
	cyVal.Hi = cySrc.Hi;
	cyVal.Lo = cySrc.Lo;
}

void CExtGridCellVariant::_VariantAssign( CY * cySrc )
{
	ASSERT_VALID( this );
	if( vt != (VT_CY|VT_BYREF) )
	{
		_VariantClear();
		vt = (VT_CY|VT_BYREF);
	} // if( vt != (VT_CY|VT_BYREF) )
	pcyVal = cySrc;
}

bool CExtGridCellVariant::_VariantIsEqual(
	const VARIANT & other,
	LCID _lcid_, // = LOCALE_USER_DEFAULT
	ULONG nVarCmpFlagsFlags // = 0L
	) const
{
	ASSERT_VALID( this );
VARIANT * pVariant0 = const_cast < CExtGridCellVariant * > ( this );
VARIANT * pVariant1 = const_cast < VARIANT * > ( &other );
	if( pVariant0 == pVariant1 )
		return true;
	if(	stat_VariantCompare(
			pVariant0,
			pVariant1,
			_lcid_,
			nVarCmpFlagsFlags
			) == 0
		)
		return true;
	return false;
}

bool CExtGridCellVariant::_VariantIsInequal(
	const VARIANT & other,
	LCID _lcid_, // = LOCALE_USER_DEFAULT
	ULONG nVarCmpFlagsFlags // = 0L
	) const
{
	ASSERT_VALID( this );
	if( _VariantIsEqual( other, _lcid_, nVarCmpFlagsFlags ) )
		return false;
	return true;
}

bool CExtGridCellVariant::_VariantIsLess(
	const VARIANT & other,
	LCID _lcid_, // = LOCALE_USER_DEFAULT
	ULONG nVarCmpFlagsFlags // = 0L
	) const
{
	ASSERT_VALID( this );
VARIANT * pVariant0 = const_cast < CExtGridCellVariant * > ( this );
VARIANT * pVariant1 = const_cast < VARIANT * > ( &other );
	if( pVariant0 == pVariant1 )
		return false;
	if(	stat_VariantCompare(
			pVariant0,
			pVariant1,
			_lcid_,
			nVarCmpFlagsFlags
			) < 0
		)
		return true;
	return false;
}

bool CExtGridCellVariant::_VariantIsGreater(
	const VARIANT & other,
	LCID _lcid_, // = LOCALE_USER_DEFAULT
	ULONG nVarCmpFlagsFlags // = 0L
	) const
{
	ASSERT_VALID( this );
VARIANT * pVariant0 = const_cast < CExtGridCellVariant * > ( this );
VARIANT * pVariant1 = const_cast < VARIANT * > ( &other );
	if( pVariant0 == pVariant1 )
		return false;
	if(	stat_VariantCompare(
			pVariant0,
			pVariant1,
			_lcid_,
			nVarCmpFlagsFlags
			) > 0
		)
		return true;
	return false;
}

bool CExtGridCellVariant::_VariantIsLessOrEqual(
	const VARIANT & other,
	LCID _lcid_, // = LOCALE_USER_DEFAULT
	ULONG nVarCmpFlagsFlags // = 0L
	) const
{
	ASSERT_VALID( this );
	if( _VariantIsEqual( other, _lcid_, nVarCmpFlagsFlags ) )
		return true;
	if( _VariantIsLess( other, _lcid_, nVarCmpFlagsFlags ) )
		return true;
	return false;
}

bool CExtGridCellVariant::_VariantIsGreaterOrEqual(
	const VARIANT & other,
	LCID _lcid_, // = LOCALE_USER_DEFAULT
	ULONG nVarCmpFlagsFlags // = 0L
	) const
{
	ASSERT_VALID( this );
	if( _VariantIsEqual( other, _lcid_, nVarCmpFlagsFlags ) )
		return true;
	if( _VariantIsGreater( other, _lcid_, nVarCmpFlagsFlags ) )
		return true;
	return false;
}

int CExtGridCellVariant::_VariantCompare(
	const VARIANT & other,
	LCID _lcid_, // = LOCALE_USER_DEFAULT
	ULONG nVarCmpFlagsFlags // = 0L
	) const
{
	ASSERT_VALID( this );
int nRetVal =
		CExtGridCell::stat_VariantCompare(
			LPVARIANT(this),
			LPVARIANT(&other),
			_lcid_,
			nVarCmpFlagsFlags
			);
	return nRetVal;
}

bool CExtGridCellVariant::_VariantChangeType(
	VARTYPE vtNewType,
	const VARIANT * pVariantOther, // = NULL
	bool bConvertToErrorIfFailed // = true
	)
{
	ASSERT_VALID( this );
VARIANT * pVariantChangeType =
		const_cast < VARIANT * > ( pVariantOther );
	if( pVariantChangeType == NULL )
		pVariantChangeType = this;
HRESULT hr =
		::VariantChangeType(
			this,
			pVariantChangeType,
			0,
			vtNewType
			);
	if( FAILED( hr ) )
	{
		if( bConvertToErrorIfFailed )
		{
			_VariantClear( false );
			vt = VT_ERROR;
			scode = hr;
		} // if( bConvertToErrorIfFailed )
		return false;
	} // if( FAILED( hr ) )
	return true;
}

void CExtGridCellVariant::Assign( const CExtGridCell & other )
{
	ASSERT_VALID( this );
	if( other.IsKindOf( RUNTIME_CLASS(CExtGridCellVariant) ) )
	{
		_AssignA(
			static_cast < const CExtGridCellVariant & > ( other )
			);
		return;
	} // if( other.IsKindOf( RUNTIME_CLASS(CExtGridCellVariant) ) )
	_InitM();
	_VariantInit();
	CExtGridCell::Assign( other );
LPCTSTR strTextBuffer = LPCTSTR( other.GetTextBuffer() );
	if( strTextBuffer != NULL )
	{
		_VariantAssign( strTextBuffer );
		ASSERT( vt == VT_BSTR );
		return;
	} // if( strTextBuffer != NULL )
CExtSafeString strCopy;
	other.TextGet( strCopy );
	if( strCopy.IsEmpty() )
		return;
	strTextBuffer = LPCTSTR( strCopy );
	ASSERT( strTextBuffer != NULL );
	_VariantAssign( strTextBuffer );
	ASSERT( vt == VT_BSTR );
}

int CExtGridCellVariant::Compare(
	const CExtGridCell & other,
	DWORD dwStyleMask, // = __EGCS_COMPARE_MASK
	DWORD dwStyleExMask // = __EGCS_EX_COMPARE_MASK
	) const
{
	ASSERT_VALID( this );
	if( other.IsKindOf( RUNTIME_CLASS(CExtGridCellVariant) ) )
	{
		int nRetVal =
			_VariantCompare(
				static_cast < const CExtGridCellVariant & > ( other )
				);
		if( nRetVal != 0 )
			return nRetVal;
		return CompareStyleOnly( other, dwStyleMask, dwStyleExMask );
	} // if( other.IsKindOf( RUNTIME_CLASS(CExtGridCellVariant) ) )
	return CExtGridCell::Compare( other, dwStyleMask, dwStyleExMask );
}

void CExtGridCellVariant::TextGet( CExtSafeString & strCopy ) const
{
	ASSERT_VALID( this );
	strCopy.Empty();
	if( vt == VT_BSTR )
	{
		if( bstrVal == NULL )
			return;
		USES_CONVERSION;
		strCopy = OLE2T( bstrVal );
		return;
	} // if( vt == VT_BSTR )
	if( vt == (VT_BSTR|VT_BYREF) )
	{
		if( pbstrVal == NULL || (*pbstrVal) == NULL )
			return;
		USES_CONVERSION;
		strCopy = OLE2T( (*pbstrVal) );
		return;
	} // if( vt == (VT_BSTR|VT_BYREF) )
CExtGridCellVariant varCopy( *this );
	if( varCopy._VariantChangeType( VT_BSTR ) )
	{
		ASSERT( varCopy.vt == VT_BSTR );
		varCopy.TextGet( strCopy );
	} // if( varCopy._VariantChangeType( VT_BSTR ) )
}

void CExtGridCellVariant::TextSet(
	__EXT_MFC_SAFE_LPCTSTR str, // = __EXT_MFC_SAFE_LPCTSTR(NULL) // empty text
	bool bAllowChangeDataType // = false
	)
{
USES_CONVERSION;
	if( bAllowChangeDataType )
	{
		_VariantClear();
		_VariantAssign( LPCTSTR(str) );
		ASSERT( vt == VT_BSTR );
		return;
	} // if( bAllowChangeDataType )
VARIANT varSrc;
	varSrc.vt = VT_BSTR;
	varSrc.bstrVal =
		::SysAllocString(
			(str == NULL)
				? ( T2COLE(    _T("")    ) )
				: ( T2COLE( LPCTSTR(str) ) )
			);
	if( varSrc.bstrVal == NULL && LPCTSTR(str) != NULL )
	{
		ASSERT( FALSE );
		return;
	} // if( varSrc.bstrVal == NULL && LPCTSTR(str) != NULL )
HRESULT hr =
		::VariantChangeType(
			this,
			&varSrc,
			0,
			vt
			);
	ASSERT( SUCCEEDED( hr ) );
	hr;
//	if( SUCCEEDED( hr ) )
//	{
//		// detach/attach like operation
//		LPVARIANT pVariantDst = this;
//		LPCVARIANT pVariantSrc = &varSrc;
//		::VariantClear( pVariantDst );
//		::memcpy( pVariantDst, pVariantSrc, sizeof(VARIANT) );
//	} // if( SUCCEEDED( hr ) )
//	else
		::VariantClear( &varSrc );
}

UINT CExtGridCellVariant::OnQueryDrawTextFlags(
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	DWORD dwAreaFlags,
	bool bIncludeHorizontalFlags, // = true
	bool bIncludeVerticalFlags, // = true
	bool bIncludeOtherFlags // = true
	) const
{
	ASSERT_VALID( this );
UINT nFlags = 0;
	if( bIncludeVerticalFlags || bIncludeVerticalFlags )
	{
		DWORD dwCellStyle = GetStyle();
		if(		bIncludeHorizontalFlags
			&&	(dwCellStyle&__EGCS_TA_HORZ_MASK) == __EGCS_TA_HORZ_BY_TYPE
			)
		{
			VARTYPE _vt = VT_EMPTY;
			if(		vt == (VT_VARIANT|VT_BYREF)
				&&	pvarVal != NULL
				)
				_vt = pvarVal->vt;
			else
				_vt = vt;
			_vt &= ~(VT_BYREF|VT_ARRAY);
			switch( _vt )
			{
			case VT_I1:
			case VT_I2:
			case VT_I4:
			case VT_I8:
			case VT_INT:
			case VT_UI1:
			case VT_UI2:
			case VT_UI4:
			case VT_UI8:
			case VT_UINT:
			case VT_R4:
			case VT_R8:
			case VT_CY:
			case VT_ERROR:
			case VT_DECIMAL:
				bIncludeHorizontalFlags = false;
				nFlags |= DT_RIGHT;
				break;
			case VT_DATE:
			case VT_BOOL:
				bIncludeHorizontalFlags = false;
				nFlags |= DT_CENTER;
				break;
			case VT_BSTR:
			default:
				bIncludeHorizontalFlags = false;
				nFlags |= DT_LEFT;
				break;
			}; // switch( _vt )
		}
		if(		bIncludeVerticalFlags
			&&	(dwCellStyle&__EGCS_TA_VERT_MASK) == __EGCS_TA_VERT_BY_TYPE
			)
		{
			bIncludeVerticalFlags = false;
			nFlags |= DT_VCENTER;
		}
	} // if( bIncludeVerticalFlags || bIncludeVerticalFlags )
	nFlags |=
		CExtGridCell::OnQueryDrawTextFlags(
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			nColType,
			nRowType,
			dwAreaFlags,
			bIncludeHorizontalFlags,
			bIncludeVerticalFlags,
			bIncludeOtherFlags
			);
	return nFlags;
}

void CExtGridCellVariant::GetVariant( VARIANT & varCopy ) const
{
	ASSERT_VALID( this );
	::VariantClear( &varCopy );
	if( vt != VT_EMPTY )
	{
		HRESULT hr =
			::VariantCopy(
				&varCopy,
				LPVARIANT( LPCVARIANT( this ) )
				);
		if( FAILED(hr) )
		{
			ASSERT( FALSE );
			::VariantClear( &varCopy );
		} // if( FAILED(hr) )
	} // if( vt != VT_EMPTY )
}

CExtGridCellVariant & CExtGridCellVariant::operator = (  const CExtGridCell & other )
{
	ASSERT_VALID( this );
	Assign( other );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = (  const VARIANT & other )
{
	ASSERT_VALID( this );
//	_InitM();
	_VariantAssign( other );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( IDispatch * pInterface )
{
	ASSERT_VALID( this );
	_VariantAssign( pInterface );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( IUnknown * pInterface )
{
	ASSERT_VALID( this );
	_VariantAssign( pInterface );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( IDispatch ** ppInterface )
{
	ASSERT_VALID( this );
	_VariantAssign( ppInterface );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( IUnknown ** ppInterface )
{
	ASSERT_VALID( this );
	_VariantAssign( ppInterface );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( const SAFEARRAY * pSA )
{
	ASSERT_VALID( this );
	_VariantAssign( pSA );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( const SAFEARRAY ** ppSA )
{
	ASSERT_VALID( this );
	_VariantAssign( ppSA );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( const DECIMAL & decimalVal )
{
	ASSERT_VALID( this );
	_VariantAssign( decimalVal );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( const DECIMAL ** decimalVal )
{
	ASSERT_VALID( this );
	_VariantAssign( decimalVal );
	return (*this);
}

bool CExtGridCellVariant::operator == ( const CExtGridCellVariant & other ) const
{
	ASSERT_VALID( this );
	return ( Compare( other ) == 0 ) ? true : false;
}

bool CExtGridCellVariant::operator == ( const VARIANT & other ) const
{
	ASSERT_VALID( this );
	return _VariantIsEqual( other );
}

bool CExtGridCellVariant::operator != ( const CExtGridCellVariant & other ) const
{
	ASSERT_VALID( this );
	return ( Compare( other ) != 0 ) ? true : false;
}

bool CExtGridCellVariant::operator != ( const VARIANT & other ) const
{
	ASSERT_VALID( this );
	return _VariantIsInequal( other );
}

bool CExtGridCellVariant::operator < ( const CExtGridCellVariant & other ) const
{
	ASSERT_VALID( this );
	return ( Compare( other ) < 0 ) ? true : false;
}

bool CExtGridCellVariant::operator < ( const VARIANT & other ) const
{
	ASSERT_VALID( this );
	return _VariantIsLess( other );
}

bool CExtGridCellVariant::operator > ( const CExtGridCellVariant & other ) const
{
	ASSERT_VALID( this );
	return ( Compare( other ) > 0 ) ? true : false;
}

bool CExtGridCellVariant::operator > ( const VARIANT & other ) const
{
	ASSERT_VALID( this );
	return _VariantIsGreater( other );
}

bool CExtGridCellVariant::operator <= ( const CExtGridCellVariant & other ) const
{
	ASSERT_VALID( this );
	return ( Compare( other ) <= 0 ) ? true : false;
}

bool CExtGridCellVariant::operator <= ( const VARIANT & other ) const
{
	ASSERT_VALID( this );
	return _VariantIsLessOrEqual( other );
}

bool CExtGridCellVariant::operator >= ( const CExtGridCellVariant & other ) const
{
	ASSERT_VALID( this );
	return ( Compare( other ) >= 0 ) ? true : false;
}

bool CExtGridCellVariant::operator >= ( const VARIANT & other ) const
{
	ASSERT_VALID( this );
	return _VariantIsGreaterOrEqual( other );
}

//CExtGridCellVariant & CExtGridCellVariant::operator = ( BSTR bstrOther )
//{
//	ASSERT_VALID( this );
//	VERIFY( _VariantAssign( bstrOther ) );
//	return (*this);
//}

//CExtGridCellVariant & CExtGridCellVariant::operator = ( BSTR * bstrOther )
//{
//	ASSERT_VALID( this );
//	VERIFY( _VariantAssign( bstrOther ) );
//	return (*this);
//}

CExtGridCellVariant & CExtGridCellVariant::operator = ( const char * strOther )
{
	ASSERT_VALID( this );
	VERIFY( _VariantAssign( strOther ) );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( const unsigned short * strOther )
{
	ASSERT_VALID( this );
	VERIFY( _VariantAssign( strOther ) );
	return (*this);
}

#if _MFC_VER >= 0x700
#if (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
CExtGridCellVariant & CExtGridCellVariant::operator = ( const __wchar_t * strOther )
{
	ASSERT_VALID( this );
	VERIFY( _VariantAssign( strOther ) );
	return (*this);
}
#endif // (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
#endif // _MFC_VER >= 0x700

CExtGridCellVariant & CExtGridCellVariant::operator = ( bool bSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( bSrc );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( signed char nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( signed char * nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( unsigned char nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( unsigned char * nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( signed short nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( signed short * nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( unsigned short nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( unsigned short * nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc, VT_UI2 );
	return (*this);
}

#if _MFC_VER >= 0x700
#if (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
CExtGridCellVariant & CExtGridCellVariant::operator = ( __wchar_t * nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc, VT_UI2 );
	return (*this);
}
#endif // (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
#endif // _MFC_VER >= 0x700


CExtGridCellVariant & CExtGridCellVariant::operator = ( signed int nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc, VT_I4 );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( signed int * nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc, VT_I4 );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( unsigned int nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc, VT_UI4 );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( unsigned int * nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc, VT_UI4 );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( signed long nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc, VT_I4 );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( signed long * nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc, VT_I4 );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( unsigned long nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( unsigned long * nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( signed __int64 nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( signed __int64 * nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( unsigned __int64 nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( unsigned __int64 * nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( float nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( float * nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( double nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc, VT_R8 );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( double * nSrc )
{
	ASSERT_VALID( this );
	_VariantAssign( nSrc, VT_R8 );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( CY cySrc )
{
	ASSERT_VALID( this );
	_VariantAssign( cySrc );
	return (*this);
}

CExtGridCellVariant & CExtGridCellVariant::operator = ( CY * cySrc )
{
	ASSERT_VALID( this );
	_VariantAssign( cySrc );
	return (*this);
}

void CExtGridCellVariant::_AssignA( const CExtGridCellVariant & other )
{
	ASSERT_VALID( this );
	_AssignM( other );
	_VariantAssign( other );
}

#ifdef _DEBUG
void CExtGridCellVariant::AssertValid() const
{
	CExtGridCell::AssertValid();
}
void CExtGridCellVariant::Dump( CDumpContext & dc ) const
{
	CExtGridCell::Dump( dc );
}
#endif // _DEBUG

HRESULT CExtGridCellVariant::OnParseText(
	__EXT_MFC_SAFE_LPCTSTR sText
	) const
{
	ASSERT_VALID( this );
	ASSERT( sText != NULL );
	if( sText == NULL )
		return E_INVALIDARG;
	if( vt == VT_BSTR || vt == VT_EMPTY )
		return S_OK;
VARIANT varSrc;
	varSrc.vt = VT_BSTR;
USES_CONVERSION;
	varSrc.bstrVal = ::SysAllocString( T2COLE(sText) );
	if( varSrc.bstrVal == NULL )
	{
		ASSERT( FALSE );
		return E_OUTOFMEMORY;
	}
HRESULT hr =
		::VariantChangeType(
			&varSrc,
			&varSrc,
			0,
			vt
			);
	::VariantClear( &varSrc );
	return hr;
}

bool CExtGridCellVariant::OnQueryEnabledIncrement() const
{
	ASSERT_VALID( this );
DWORD dwCellStyle = GetStyle();
	if( (dwCellStyle&__EGCS_READ_ONLY) != 0 )
		return false;
LPCVARIANT pVariant = this;
VARTYPE varType = pVariant->vt;
	if( varType == VT_VARIANT )
	{
		pVariant = pVariant->pvarVal;
		if( pVariant == NULL )
			return false;
		varType = pVariant->vt;
	} // if( varType == VT_VARIANT )
	if( (varType&VT_ARRAY) != 0 )
		return false;
	switch( (varType&(~VT_BYREF)) )
	{
	case VT_I1:
		if( (varType&VT_BYREF) != 0 )
		{
			if( (*(pVariant->pcVal)) < SCHAR_MAX )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->cVal < SCHAR_MAX )
			return true;
		return false;
	case VT_UI1:
		if( (varType&VT_BYREF) != 0 )
		{
			if( (*(pVariant->pbVal)) < UCHAR_MAX )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->bVal < UCHAR_MAX )
			return true;
		return false;
	case VT_BOOL:
		return true;
	case VT_I2:
		if( (varType&VT_BYREF) != 0 )
		{
			if( (*(pVariant->piVal)) < SHRT_MAX )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->iVal < SHRT_MAX )
			return true;
		return false;
	case VT_UI2:
		if( (varType&VT_BYREF) != 0 )
		{
			if( (*(pVariant->puiVal)) < USHRT_MAX )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->uiVal < USHRT_MAX )
			return true;
		return false;
	case VT_ERROR:
	case VT_INT:
	case VT_I4:
		if( (varType&VT_BYREF) != 0 )
		{
			if( (*(pVariant->plVal)) < INT_MAX )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->lVal < INT_MAX )
			return true;
		return false;
	case VT_UINT:
	case VT_UI4:
		if( (varType&VT_BYREF) != 0 )
		{
			if( (*(pVariant->pulVal)) < UINT_MAX )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->ulVal < UINT_MAX )
			return true;
		return false;
	case VT_I8:
	{
		if( (varType&VT_BYREF) != 0 )
		{
			signed __int64 * ptr =
				reinterpret_cast
				< signed __int64 * >
				( LPVOID(pVariant->plVal) );
			if( (*ptr) < _I64_MAX )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		signed __int64 * ptr =
			reinterpret_cast
			< signed __int64 * >
			( LPVOID(&(pVariant->lVal)) );
		if( (*ptr) < _I64_MAX )
			return true;
		return false;
	}
	case VT_UI8:
	{
		if( (varType&VT_BYREF) != 0 )
		{
			unsigned __int64 * ptr =
				reinterpret_cast
				< unsigned __int64 * >
				( LPVOID(pVariant->plVal) );
			if( (*ptr) < _UI64_MAX )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		unsigned __int64 * ptr =
			reinterpret_cast
			< unsigned __int64 * >
			( LPVOID(&(pVariant->lVal)) );
		if( (*ptr) < _UI64_MAX )
			return true;
		return false;
	}
	case VT_R4:
		if( (varType&VT_BYREF) != 0 )
		{
			if( (*(pVariant->pfltVal)) < FLT_MAX )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->fltVal < FLT_MAX )
			return true;
		return false;
	case VT_R8:
	case VT_DATE:
		if( (varType&VT_BYREF) != 0 )
		{
			if( (*(pVariant->pdblVal)) < DBL_MAX )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->dblVal < DBL_MAX )
			return true;
		return false;
	case VT_CY:
		if( (varType&VT_BYREF) != 0 )
		{
			if( pVariant->pcyVal->int64 < _I64_MAX )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->cyVal.int64 < _I64_MAX )
			return true;
		return false;
	case VT_DECIMAL:
		if( (varType&VT_BYREF) != 0 )
		{
			if( pVariant->pdecVal->Hi32 < UINT_MAX )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->decVal.Hi32 < UINT_MAX )
			return true;
		return false;
	} // switch( (varType&(~VT_BYREF)) )
	return false;
}

bool CExtGridCellVariant::OnQueryEnabledDecrement() const
{
	ASSERT_VALID( this );
DWORD dwCellStyle = GetStyle();
	if( (dwCellStyle&__EGCS_READ_ONLY) != 0 )
		return false;
LPCVARIANT pVariant = this;
VARTYPE varType = pVariant->vt;
	if( varType == VT_VARIANT )
	{
		pVariant = pVariant->pvarVal;
		if( pVariant == NULL )
			return false;
		varType = pVariant->vt;
	} // if( varType == VT_VARIANT )
	if( (varType&VT_ARRAY) != 0 )
		return false;
	switch( (varType&(~VT_BYREF)) )
	{
	case VT_I1:
		if( (varType&VT_BYREF) != 0 )
		{
			if( (*(pVariant->pcVal)) > SCHAR_MIN )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->cVal > SCHAR_MIN )
			return true;
		return false;
	case VT_UI1:
		if( (varType&VT_BYREF) != 0 )
		{
			if( (*(pVariant->pbVal)) > UCHAR(0) )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->bVal > UCHAR(0) )
			return true;
		return false;
	case VT_BOOL:
		return true;
	case VT_I2:
		if( (varType&VT_BYREF) != 0 )
		{
			if( (*(pVariant->piVal)) > SHRT_MIN )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->iVal > SHRT_MIN )
			return true;
		return false;
	case VT_UI2:
		if( (varType&VT_BYREF) != 0 )
		{
			if( (*(pVariant->puiVal)) > USHORT(0) )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->uiVal > USHORT(0) )
			return true;
		return false;
	case VT_ERROR:
	case VT_INT:
	case VT_I4:
		if( (varType&VT_BYREF) != 0 )
		{
			if( (*(pVariant->plVal)) > INT_MIN )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->lVal > INT_MIN )
			return true;
		return false;
	case VT_UINT:
	case VT_UI4:
		if( (varType&VT_BYREF) != 0 )
		{
			if( (*(pVariant->pulVal)) > UINT(0) )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->ulVal > UINT(0) )
			return true;
		return false;
	case VT_I8:
	{
		if( (varType&VT_BYREF) != 0 )
		{
			signed __int64 * ptr =
				reinterpret_cast
				< signed __int64 * >
				( LPVOID(pVariant->plVal) );
			if( (*ptr) > __int64(0) )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		signed __int64 * ptr =
			reinterpret_cast
			< signed __int64 * >
			( LPVOID(&(pVariant->lVal)) );
		if( (*ptr) > __int64(0) )
			return true;
		return false;
	}
	case VT_UI8:
	{
		if( (varType&VT_BYREF) != 0 )
		{
			unsigned __int64 * ptr =
				reinterpret_cast
				< unsigned __int64 * >
				( LPVOID(pVariant->plVal) );
			if( (*ptr) > ((unsigned __int64)(0)) )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		unsigned __int64 * ptr =
			reinterpret_cast
			< unsigned __int64 * >
			( LPVOID(&(pVariant->lVal)) );
		if( (*ptr) > ((unsigned __int64)(0)) )
			return true;
		return false;
	}
	case VT_R4:
		if( (varType&VT_BYREF) != 0 )
		{
			if( (*(pVariant->pfltVal)) > FLT_MIN )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->fltVal > FLT_MIN )
			return true;
		return false;
	case VT_R8:
	case VT_DATE:
		if( (varType&VT_BYREF) != 0 )
		{
			if( (*(pVariant->pdblVal)) > DBL_MIN )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->dblVal > DBL_MIN )
			return true;
		return false;
	case VT_CY:
		if( (varType&VT_BYREF) != 0 )
		{
			if( pVariant->pcyVal->int64 > _I64_MIN )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->cyVal.int64 > _I64_MIN )
			return true;
		return false;
	case VT_DECIMAL:
		if( (varType&VT_BYREF) != 0 )
		{
			if( pVariant->pdecVal->Hi32 > UINT(0) )
				return true;
			return false;
		} // if( (varType&VT_BYREF) != 0 )
		if( pVariant->decVal.Hi32 > UINT(0) )
			return true;
		return false;
	} // switch( (varType&(~VT_BYREF)) )
	return false;
}

bool CExtGridCellVariant::OnValueIncrement()
{
	ASSERT_VALID( this );
LPVARIANT pVariant = this;
VARTYPE varType = pVariant->vt;
	if( varType == VT_VARIANT )
	{
		pVariant = pVariant->pvarVal;
		if( pVariant == NULL )
			return false;
		varType = pVariant->vt;
	} // if( varType == VT_VARIANT )
	if( (varType&VT_ARRAY) != 0 )
		return false;
	switch( (varType&(~VT_BYREF)) )
	{
	case VT_I1:
		if( (varType&VT_BYREF) != 0 )
		{
			(*(pVariant->pcVal)) ++;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->cVal ++;
		return true;
	case VT_UI1:
		if( (varType&VT_BYREF) != 0 )
		{
			(*(pVariant->pbVal)) ++;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->bVal ++;
		return true;
	case VT_BOOL:
		pVariant->boolVal = ( ! pVariant->boolVal );
		return true;
	case VT_I2:
		if( (varType&VT_BYREF) != 0 )
		{
			(*(pVariant->piVal)) ++;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->iVal ++;
		return true;
	case VT_UI2:
		if( (varType&VT_BYREF) != 0 )
		{
			(*(pVariant->puiVal)) ++;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->uiVal ++;
		return true;
	case VT_ERROR:
	case VT_INT:
	case VT_I4:
		if( (varType&VT_BYREF) != 0 )
		{
			(*(pVariant->plVal)) ++;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->lVal ++;
		return true;
	case VT_UINT:
	case VT_UI4:
		if( (varType&VT_BYREF) != 0 )
		{
			(*(pVariant->pulVal)) ++;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->ulVal ++;
		return true;
	case VT_I8:
	{
		if( (varType&VT_BYREF) != 0 )
		{
			signed __int64 * ptr =
				reinterpret_cast
				< signed __int64 * >
				( LPVOID(pVariant->plVal) );
			(*ptr) ++;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		signed __int64 * ptr =
			reinterpret_cast
			< signed __int64 * >
			( LPVOID(&(pVariant->lVal)) );
		(*ptr) ++;
		return true;
	}
	case VT_UI8:
	{
		if( (varType&VT_BYREF) != 0 )
		{
			unsigned __int64 * ptr =
				reinterpret_cast
				< unsigned __int64 * >
				( LPVOID(pVariant->plVal) );
			(*ptr) ++;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		unsigned __int64 * ptr =
			reinterpret_cast
			< unsigned __int64 * >
			( LPVOID(&(pVariant->lVal)) );
		(*ptr) ++;
		return true;
	}
	case VT_R4:
		if( (varType&VT_BYREF) != 0 )
		{
			(*(pVariant->pfltVal)) += 1.0f;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->fltVal += 1.0f;
		return true;
	case VT_R8:
	case VT_DATE:
		if( (varType&VT_BYREF) != 0 )
		{
			(*(pVariant->pdblVal)) += 1.0;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->dblVal += 1.0;
		return true;
	case VT_CY:
		if( (varType&VT_BYREF) != 0 )
		{
			pVariant->pcyVal->int64 ++;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->cyVal.int64 ++;
		return true;
	case VT_DECIMAL:
		if( (varType&VT_BYREF) != 0 )
		{
			pVariant->pdecVal->Hi32 ++;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->decVal.Hi32 ++;
		return true;
	} // switch( (varType&(~VT_BYREF)) )
	return false;
}

bool CExtGridCellVariant::OnValueDecrement()
{
	ASSERT_VALID( this );
LPVARIANT pVariant = this;
VARTYPE varType = pVariant->vt;
	if( varType == VT_VARIANT )
	{
		pVariant = pVariant->pvarVal;
		if( pVariant == NULL )
			return false;
		varType = pVariant->vt;
	} // if( varType == VT_VARIANT )
	if( (varType&VT_ARRAY) != 0 )
		return false;
	switch( (varType&(~VT_BYREF)) )
	{
	case VT_I1:
		if( (varType&VT_BYREF) != 0 )
		{
			(*(pVariant->pcVal)) --;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->cVal --;
		return true;
	case VT_UI1:
		if( (varType&VT_BYREF) != 0 )
		{
			(*(pVariant->pbVal)) --;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->bVal --;
		return true;
	case VT_BOOL:
		pVariant->boolVal = ( ! pVariant->boolVal );
		return true;
	case VT_I2:
		if( (varType&VT_BYREF) != 0 )
		{
			(*(pVariant->piVal)) --;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->iVal --;
		return true;
	case VT_UI2:
		if( (varType&VT_BYREF) != 0 )
		{
			(*(pVariant->puiVal)) --;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->uiVal --;
		return true;
	case VT_ERROR:
	case VT_INT:
	case VT_I4:
		if( (varType&VT_BYREF) != 0 )
		{
			(*(pVariant->plVal)) --;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->lVal --;
		return true;
	case VT_UINT:
	case VT_UI4:
		if( (varType&VT_BYREF) != 0 )
		{
			(*(pVariant->pulVal)) --;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->ulVal --;
		return true;
	case VT_I8:
	{
		if( (varType&VT_BYREF) != 0 )
		{
			signed __int64 * ptr =
				reinterpret_cast
				< signed __int64 * >
				( LPVOID(pVariant->plVal) );
			(*ptr) --;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		signed __int64 * ptr =
			reinterpret_cast
			< signed __int64 * >
			( LPVOID(&(pVariant->lVal)) );
		(*ptr) --;
		return true;
	}
	case VT_UI8:
	{
		if( (varType&VT_BYREF) != 0 )
		{
			unsigned __int64 * ptr =
				reinterpret_cast
				< unsigned __int64 * >
				( LPVOID(pVariant->plVal) );
			(*ptr) --;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		unsigned __int64 * ptr =
			reinterpret_cast
			< unsigned __int64 * >
			( LPVOID(&(pVariant->lVal)) );
		(*ptr) --;
		return true;
	}
	case VT_R4:
		if( (varType&VT_BYREF) != 0 )
		{
			(*(pVariant->pfltVal)) -= 1.0f;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->fltVal -= 1.0f;
		return true;
	case VT_R8:
	case VT_DATE:
		if( (varType&VT_BYREF) != 0 )
		{
			(*(pVariant->pdblVal)) -= 1.0;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->dblVal -= 1.0;
		return true;
	case VT_CY:
		if( (varType&VT_BYREF) != 0 )
		{
			pVariant->pcyVal->int64 --;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->cyVal.int64 --;
		return true;
	case VT_DECIMAL:
		if( (varType&VT_BYREF) != 0 )
		{
			pVariant->pdecVal->Hi32 --;
			return true;
		} // if( (varType&VT_BYREF) != 0 )
		pVariant->decVal.Hi32 --;
		return true;
	} // switch( (varType&(~VT_BYREF)) )
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CExtGridDataSortOrder

IMPLEMENT_SERIAL( CExtGridDataSortOrder, CObject, VERSIONABLE_SCHEMA|1 );

CExtGridDataSortOrder::CExtGridDataSortOrder()
{
}

CExtGridDataSortOrder::CExtGridDataSortOrder(
	const CExtGridDataSortOrder & other
	)
{
	_AssignFromOther( other );
}

CExtGridDataSortOrder::~CExtGridDataSortOrder()
{
	Empty();
}

void CExtGridDataSortOrder::_AssignFromOther(
	const CExtGridDataSortOrder & other
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&other) );
	Empty();
LONG nCount = other.m_arrItems.GetSize();
	for( LONG i = 0L; i < nCount; i++ )
	{
		ITEM_INFO _itemDst( other.m_arrItems[ i ] );
		m_arrItems.Add( _itemDst );
	}
	ASSERT( nCount == m_arrItems.GetSize() );
}

CExtGridDataSortOrder & CExtGridDataSortOrder::operator =(
	const CExtGridDataSortOrder & other
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&other) );
	_AssignFromOther( other );
	return (*this);
}

bool CExtGridDataSortOrder::operator ==(
	const CExtGridDataSortOrder & other
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&other) );
LONG nCount = m_arrItems.GetSize();
LONG i = other.m_arrItems.GetSize();
	if( nCount != i )
		return false;
	for( i = 0L; i < nCount; i++ )
	{
		const ITEM_INFO & _itemSrc = m_arrItems[ i ];
		const ITEM_INFO & _itemDst = other.m_arrItems[ i ];
		if( _itemSrc != _itemDst )
			return false;
	}
	return true;
}

bool CExtGridDataSortOrder::operator !=(
	const CExtGridDataSortOrder & other
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&other) );
	if( (*this) == other )
		return false;
	return true;
}

#ifdef _DEBUG
void CExtGridDataSortOrder::AssertValid() const
{
	CObject::AssertValid();
}
void CExtGridDataSortOrder::Dump( CDumpContext & dc ) const
{
	CObject::Dump( dc );
}
#endif // _DEBUG

void CExtGridDataSortOrder::Serialize( CArchive & ar )
{
	CObject::Serialize( ar );
LONG nCount;
	if( ar.IsStoring() )
	{
		nCount = m_arrItems.GetSize();
		ar << nCount;
		for( LONG i = 0L; i < nCount; i++ )
		{
			ITEM_INFO & _itemSrc = m_arrItems[ i ];
			_itemSrc.Serialize( ar );
		}
	} // if( ar.IsStoring() )
	else
	{
		m_arrItems.RemoveAll();
		ar >> nCount;
		for( LONG i = 0L; i < nCount; i++ )
		{
			ITEM_INFO _itemSrc;
			_itemSrc.Serialize( ar );
			m_arrItems.Add( _itemSrc );
		}
		ASSERT( m_arrItems.GetSize() == nCount );
	} // else from if( ar.IsStoring() )
}

bool CExtGridDataSortOrder::ItemsUnique() const
{
	ASSERT_VALID( this );
LONG nCount = m_arrItems.GetSize();
	for( LONG i = 0L; i < (nCount - 1L); i++ )
	{
		const ITEM_INFO & _itemSrc = m_arrItems[ i ];
		for( LONG j = (i + 1L); j < nCount; j++ )
		{
			const ITEM_INFO & _itemDst = m_arrItems[ j ];
			if( _itemSrc == _itemDst )
				return false;
		}
	}
	return true;
}

void CExtGridDataSortOrder::MakeItemsUnique()
{
	ASSERT_VALID( this );
LONG nCount = m_arrItems.GetSize();
	for( LONG i = 0L; i < (nCount - 1L); i++ )
	{
		const ITEM_INFO & _itemSrc = m_arrItems[ i ];
		for( LONG j = (i + 1L); j < nCount; )
		{
			const ITEM_INFO & _itemDst = m_arrItems[ j ];
			if( _itemSrc != _itemDst )
			{
				j++;
				continue;
			}
			nCount--;
			m_arrItems.RemoveAt( j, 1 );
		}
	}
}

bool CExtGridDataSortOrder::IsEmpty() const
{
	ASSERT_VALID( this );
	if( m_arrItems.GetSize() == 0L )
		return true;
	return false;
}

void CExtGridDataSortOrder::Empty()
{
	ASSERT_VALID( this );
	m_arrItems.RemoveAll();
}

LONG CExtGridDataSortOrder::GetRowColPos( LONG nRowColNo ) const
{
	ASSERT_VALID( this );
LONG nCount = m_arrItems.GetSize();
	for( LONG i = 0L; i < nCount; i++ )
	{
		const ITEM_INFO & _itemSrc = m_arrItems[ i ];
		if( _itemSrc.m_nRowColNo == nRowColNo )
			return i;
	}
	return -1L;
}

void CExtGridDataSortOrder::SetupOrder(
	const CExtGridDataSortOrder & _gdsoUpdate,
	bool bInvertIntersectionSortOrder
	)
{
	ASSERT_VALID( this );
	ASSERT( ItemsUnique() );
	ASSERT_VALID( (&_gdsoUpdate) );
	ASSERT( _gdsoUpdate.ItemsUnique() );
LONG nCount = m_arrItems.GetSize();
	if( nCount == 0 )
	{
		(*this) = _gdsoUpdate;
		return;
	} // if( nCount == 0 )
LONG nCountUpdate = _gdsoUpdate.m_arrItems.GetSize();
	for( LONG u = 0L; u < nCountUpdate; u++ )
	{
		const ITEM_INFO & _itemUpdate = _gdsoUpdate.m_arrItems[ u ];
		ITEM_INFO _itemSetup( _itemUpdate );
		LONG i = GetRowColPos( _itemUpdate.m_nRowColNo );
		if( i < 0L )
		{
			m_arrItems.Add( _itemSetup );
			continue;
		}
		ITEM_INFO & _itemDst = m_arrItems[ i ];
		ASSERT( _itemDst.m_nRowColNo == _itemUpdate.m_nRowColNo );
		if(		_itemDst == _itemUpdate
			&&	(! bInvertIntersectionSortOrder )
			)
			continue;
		if( bInvertIntersectionSortOrder )
			_itemSetup.m_bAscending = (! _itemDst.m_bAscending);
		m_arrItems.SetAt( i, _itemSetup );
	}
	ASSERT_VALID( this );
	ASSERT( ItemsUnique() );
}

void CExtGridDataSortOrder::UpdateIndices(
	LONG nRowColNo,
	LONG nInsertRemoveCount,
	bool bInsert
	)
{
	ASSERT_VALID( this );
	ASSERT( nRowColNo >= 0L );
	ASSERT( nInsertRemoveCount >= 0L );
	if( nInsertRemoveCount == 0L )
		return;
LONG nCount = m_arrItems.GetSize();
	for( LONG i = 0L; i < nCount; )
	{
		ITEM_INFO _itemUpdate( m_arrItems[ i ] );
		if( bInsert )
		{
			if( _itemUpdate.m_nRowColNo >= nRowColNo )
			{
				_itemUpdate.m_nRowColNo += nInsertRemoveCount;
				m_arrItems.SetAt( i, _itemUpdate );
			}
			i++;
		}
		else
		{
			if( _itemUpdate.m_nRowColNo >= (nRowColNo+nInsertRemoveCount) )
			{
				_itemUpdate.m_nRowColNo -= nInsertRemoveCount;
				m_arrItems.SetAt( i, _itemUpdate );
				i++;
			}
			else if( _itemUpdate.m_nRowColNo >= nRowColNo )
			{
				m_arrItems.RemoveAt( i, 1 );
				nCount--;
			}
			else
				i++;
		}
	}
}

void CExtGridDataSortOrder::SwapDroppedSeries(
	LONG nRowColNoSrc,
	LONG nRowColNoDropBefore
	)
{
	ASSERT_VALID( this );
	ASSERT( nRowColNoSrc >= 0L );
	ASSERT( nRowColNoDropBefore >= 0L );
	if(		nRowColNoSrc == nRowColNoDropBefore
		||	(nRowColNoSrc+1L) == nRowColNoDropBefore
		)
		return;
LONG nCount = m_arrItems.GetSize();
	for( LONG i = 0L; i < nCount; i++ )
	{
		ITEM_INFO _itemUpdate( m_arrItems[ i ] );
		if( nRowColNoSrc < nRowColNoDropBefore )
		{
			if(		_itemUpdate.m_nRowColNo > nRowColNoSrc
				&&	_itemUpdate.m_nRowColNo < nRowColNoDropBefore
				)
			{
				_itemUpdate.m_nRowColNo --;
				m_arrItems.SetAt( i, _itemUpdate );
			}
			else if( _itemUpdate.m_nRowColNo == nRowColNoSrc )
			{
				_itemUpdate.m_nRowColNo = nRowColNoDropBefore - 1L;
				m_arrItems.SetAt( i, _itemUpdate );
			}
		} // if( nRowColNoSrc < nRowColNoDropBefore )
		else
		{
			ASSERT( nRowColNoSrc > nRowColNoDropBefore );
			if(		_itemUpdate.m_nRowColNo < nRowColNoSrc
				&&	_itemUpdate.m_nRowColNo >= nRowColNoDropBefore
				)
			{
				_itemUpdate.m_nRowColNo ++;
				m_arrItems.SetAt( i, _itemUpdate );
			}
			else if( _itemUpdate.m_nRowColNo == nRowColNoSrc )
			{
				_itemUpdate.m_nRowColNo = nRowColNoDropBefore;
				m_arrItems.SetAt( i, _itemUpdate );
			}
		} // else from if( nRowColNoSrc < nRowColNoDropBefore )
	} // for( LONG i = 0L; i < nCount; i++ )
}

/////////////////////////////////////////////////////////////////////////////
// CExtGridDataProvider
// (abstract data provider for any grid window)

IMPLEMENT_DYNAMIC( CExtGridDataProvider, CObject );

CExtGridDataProvider::CExtGridDataProvider()
{
}

CExtGridDataProvider::~CExtGridDataProvider()
{
}

#ifdef _DEBUG
void CExtGridDataProvider::AssertValid() const
{
	CObject::AssertValid();
}
void CExtGridDataProvider::Dump( CDumpContext & dc ) const
{
	CObject::Dump( dc );
}
#endif // _DEBUG

__EXT_MFC_SAFE_LPTSTR CExtGridDataProvider::StringAlloc(
	const CExtSafeString & strSrc
	)
{
	ASSERT_VALID( this );
	return
		StringAlloc(
			( strSrc.IsEmpty() )
				? __EXT_MFC_SAFE_LPCTSTR( _T("") )
				: __EXT_MFC_SAFE_LPCTSTR( LPCTSTR(strSrc) )
			);
}

__EXT_MFC_SAFE_LPTSTR CExtGridDataProvider::StringAlloc(
	__EXT_MFC_SAFE_LPCTSTR strSrc
	)
{
	ASSERT_VALID( this );
	if( LPCTSTR(strSrc) == NULL )
	{
		ASSERT( FALSE );
		return __EXT_MFC_SAFE_LPTSTR( NULL );
	} // if( LPCTSTR(strSrc) == NULL )
INT nLength = _tcslen( strSrc );
	ASSERT( nLength >= 0 );
__EXT_MFC_SAFE_LPTSTR strRetVal = StringAlloc( nLength + 1 );
	if( LPTSTR(strRetVal) == NULL )
	{
		ASSERT( FALSE );
		return __EXT_MFC_SAFE_LPTSTR( NULL );
	} // if( LPTSTR(strRetVal) == NULL )
	_tcscpy(
		LPTSTR(strRetVal),
		( nLength > 0 ) ? LPCTSTR(strSrc) : _T("")
		);
	return strRetVal;
}

const CExtGridCell * CExtGridDataProvider::RowDefaultValueGet(
	ULONG nRowNo
	) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridDataProvider * > ( this ) )
			-> RowDefaultValueGet( nRowNo );
}

const CExtGridCell * CExtGridDataProvider::ColumnDefaultValueGet(
	ULONG nColNo
	) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridDataProvider * > ( this ) )
			-> ColumnDefaultValueGet( nColNo );
}

bool CExtGridDataProvider::RowDefaultValueUnbind(
	ULONG nRowNo // = (ULONG(-1)) // if (ULONG(-1)) - default value for all rows
	)
{
	ASSERT_VALID( this );
	return RowDefaultValueBind( nRowNo );
}

bool CExtGridDataProvider::ColumnDefaultValueUnbind(
	ULONG nColNo // = (ULONG(-1)) // if (ULONG(-1)) - default value for all columns
	)
{
	ASSERT_VALID( this );
	return ColumnDefaultValueBind( nColNo );
}

const CExtGridCell * CExtGridDataProvider::CellGet(
	ULONG nColNo,
	ULONG nRowNo,
	CRuntimeClass * pInitRTC, // = NULL
	bool bAutoFindValue, // = true // auto find row/column default value (only when pInitRTC=NULL)
	bool bUseColumnDefaultValue // = true // false - use row default value (only when bAutoFindValue=true)
	) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridDataProvider * > ( this ) )
			-> CellGet(
				nColNo,
				nRowNo,
				pInitRTC,
				bAutoFindValue,
				bUseColumnDefaultValue
				);
}

bool CExtGridDataProvider::CellRangeEmpty(
	ULONG nColNo,
	ULONG nRowNo,
	ULONG nColCount, // = 1L
	ULONG nRowCount, // = 1L
	ULONG * p_nUpdatedCellCount // = NULL // returns count of really erased cells (zero also may be treated as success)
	)
{
	ASSERT_VALID( this );
	return
		CellRangeSet(
			nColNo,
			nRowNo,
			nColCount,
			nRowCount,
			NULL,
			true,
			NULL,
			false,
			true,
			p_nUpdatedCellCount
			);
}

bool CExtGridDataProvider::CellRangeEmptyAll(
	ULONG * p_nUpdatedCellCount // = NULL // returns count of really erased cells (zero also may be treated as success)
	)
{
	ASSERT_VALID( this );
ULONG nColCount = ColumnCountGet();
ULONG nRowCount = RowCountGet();
	return
		CellRangeEmpty(
			0,
			0,
			nColCount,
			nRowCount,
			p_nUpdatedCellCount
			);
}

bool CExtGridDataProvider::CacheIsVisibleFirstRecord( bool bHorz ) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridDataProvider * > ( this ) )
			-> CacheIsVisibleFirstRecord( bHorz );
}

bool CExtGridDataProvider::CacheIsVisibleLastRecord( bool bHorz ) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridDataProvider * > ( this ) )
			-> CacheIsVisibleLastRecord( bHorz );
}

ULONG CExtGridDataProvider::CacheColumnCountGet() const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridDataProvider * > ( this ) )
			-> CacheColumnCountGet();
}

ULONG CExtGridDataProvider::CacheRowCountGet() const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridDataProvider * > ( this ) )
			-> CacheRowCountGet();
}

INT CExtGridDataProvider::IconGetCount() const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridDataProvider * > ( this ) )
			-> IconGetCount();
}

const CExtCmdIcon * CExtGridDataProvider::IconGetAt( INT nIdx ) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridDataProvider * > ( this ) )
			-> IconGetAt( nIdx );
}

INT CExtGridDataProvider::IconInsert( // returns index or -1
	const CExtCmdIcon * pIcon,
	INT nIdx, // = -1 // append
	bool bCopyIcon // = true
	)
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridDataProvider * > ( this ) )
			-> IconInsert(
					const_cast < CExtCmdIcon * > ( pIcon ),
					nIdx,
					bCopyIcon
					);
}

/////////////////////////////////////////////////////////////////////////////
// CExtGridDataProviderBL
// (basic layer data provider)

IMPLEMENT_DYNAMIC( CExtGridDataProviderBL, CExtGridDataProvider );

CExtGridDataProviderBL::CExtGridDataProviderBL()
	: m_nColCountReserved( 0L )
	, m_nRowCountReserved( 0L )
{
}

CExtGridDataProviderBL::~CExtGridDataProviderBL()
{
}

#ifdef _DEBUG
void CExtGridDataProviderBL::AssertValid() const
{
	CExtGDPI < CExtGridDataProvider > :: AssertValid();
}
void CExtGridDataProviderBL::Dump( CDumpContext & dc ) const
{
	CExtGDPI < CExtGridDataProvider > :: Dump( dc );
}
#endif // _DEBUG

CExtGridCell * CExtGridDataProviderBL::RowDefaultValueGet(
	ULONG nRowNo
	)
{
	ASSERT_VALID( this );
	return _DVM_Lookup( m_mapDVY, nRowNo );
}

CExtGridCell * CExtGridDataProviderBL::ColumnDefaultValueGet(
	ULONG nColNo
	)
{
	ASSERT_VALID( this );
	return _DVM_Lookup( m_mapDVX, nColNo );
}

bool CExtGridDataProviderBL::RowDefaultValueBind(
	ULONG nRowNo, // = (ULONG(-1)) // if (ULONG(-1)) - default value for all rows
	CExtGridCell * pCell // = NULL // if NULL - remove default value
	)
{
	ASSERT_VALID( this );
	if( ! _DVM_Setup( m_mapDVY, nRowNo, pCell ) )
		return false;
	return true;
}

bool CExtGridDataProviderBL::ColumnDefaultValueBind(
	ULONG nColNo, // = (ULONG(-1)) // if (ULONG(-1)) - default value for all columns
	CExtGridCell * pCell // = NULL // if NULL - remove default value
	)
{
	ASSERT_VALID( this );
	if( ! _DVM_Setup( m_mapDVX, nColNo, pCell ) )
		return false;
	return true;
}

void CExtGridDataProviderBL::RowDefaultValueUnbindAll()
{
	ASSERT_VALID( this );
	_DVM_RemoveAll( m_mapDVY );
}

void CExtGridDataProviderBL::ColumnDefaultValueUnbindAll()
{
	ASSERT_VALID( this );
	_DVM_RemoveAll( m_mapDVX );
}

void CExtGridDataProviderBL::_DVM_RemoveAll(
	CExtGridDataProviderBL::DefValueMap_t & _map
	)
{
	ASSERT_VALID( this );
POSITION pos = _map.GetStartPosition();
	for( ; pos != NULL; )
	{
		ULONG nIdx;
		CExtGridCell * pCell;
		_map.GetNextAssoc( pos, nIdx, pCell );
		ASSERT_VALID( pCell );
		ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
		ASSERT( pCell->DataProviderGet() == this );
		//ASSERT_VALID( pCell->DataProviderGet() );
		IMalloc * pMalloc = _DVM_GetCellAllocator();
		ASSERT( pMalloc != NULL );
		CExtGridCell::InstanceDestroy( pCell, pMalloc );
	} // for( ; pos != NULL; )
	_map.RemoveAll();
}

bool CExtGridDataProviderBL::_DVM_Setup(
	CExtGridDataProviderBL::DefValueMap_t & _map,
	ULONG nIdx,
	CExtGridCell * pCell
	)
{
	ASSERT_VALID( this );
CExtGridCell * pCellDV = NULL;
	if( !_map.Lookup(nIdx,pCellDV) )
		pCellDV = NULL;
	if( pCell == NULL )
	{
		if( pCellDV == NULL )
			return false;
		ASSERT_VALID( pCellDV );
		ASSERT( pCellDV->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
		ASSERT( pCellDV->DataProviderGet() == this );
		//ASSERT_VALID( pCellDV->DataProviderGet() );
		IMalloc * pMalloc = _DVM_GetCellAllocator();
		ASSERT( pMalloc != NULL );
		CExtGridCell::InstanceDestroy( pCellDV, pMalloc );
		VERIFY( _map.RemoveKey(nIdx) );
		return true;
	} // if( pCell == NULL )
	ASSERT_VALID( pCell );
	ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
	if( pCellDV == pCell )
		return true;
	if( pCellDV != NULL )
	{
		ASSERT_VALID( pCellDV );
		ASSERT( pCellDV->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
		ASSERT( pCellDV->DataProviderGet() == this );
		//ASSERT_VALID( pCellDV->DataProviderGet() );
		IMalloc * pMalloc = _DVM_GetCellAllocator();
		ASSERT( pMalloc != NULL );
		CExtGridCell::InstanceDestroy( pCellDV, pMalloc );
	} // if( pCellDV != NULL )
IMalloc * pMalloc = _DVM_GetCellAllocator();
	ASSERT( pMalloc != NULL );
CExtGridCell * pCellNew = pCell->Clone( pMalloc );
	if( pCellNew == NULL )
	{
		ASSERT( FALSE );
		return false;
	} // if( pCellNew == NULL )
	ASSERT_VALID( pCellNew );
	pCellNew->DataProviderSet( this );
	_map.SetAt( nIdx, pCellNew );
	return true;
}

CExtGridCell * CExtGridDataProviderBL::_DVM_Lookup(
	CExtGridDataProviderBL::DefValueMap_t & _map,
	ULONG nIdx
	)
{
CExtGridCell * pCellDV = NULL;
	if( !_map.Lookup(nIdx,pCellDV) )
		pCellDV = NULL;
#ifdef _DEBUG
	else
	{
		ASSERT_VALID( pCellDV );
		ASSERT( pCellDV->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
		ASSERT( pCellDV->DataProviderGet() == this );
		//ASSERT_VALID( pCellDV->DataProviderGet() );
	} // else from if( !_map.Lookup(nIdx,pCellDV) )
#endif // _DEBUG
	return pCellDV;
}

bool CExtGridDataProviderBL::CacheReserveForOuterCells(
	ULONG nColCount,
	ULONG nRowCount
	)
{
	ASSERT_VALID( this );
	m_nColCountReserved = nColCount;
	m_nRowCountReserved = nRowCount;
	return false;
}

void CExtGridDataProviderBL::CacheReservedCountsGet(
	ULONG * p_nColCount,
	ULONG * p_nRowCount
	) const
{
	ASSERT_VALID( this );
	if( p_nColCount != NULL )
		(*p_nColCount) = m_nColCountReserved;
	if( p_nRowCount != NULL )
		(*p_nRowCount) = m_nRowCountReserved;
}

bool CExtGridDataProviderBL::CacheData(
	const CExtScrollItemCacheInfo & _sciNew,
	const CExtScrollItemCacheInfo & _sciOld
	)
{
	ASSERT_VALID( this );
	_sciNew;
	_sciOld;

#ifdef _DEBUG
//CString strTraceMsg( _T("   ---> CExtGridDataProviderBL::CacheData()\n") );
//CString strLine;
//LONG nPosChangingH = _sciNew.GetPosChangingH(_sciOld);
//LONG nPosChangingV = _sciNew.GetPosChangingV(_sciOld);
//LONG nPageSizeChangingH = _sciNew.GetPageSizeChangingH(_sciOld);
//LONG nPageSizeChangingV = _sciNew.GetPageSizeChangingV(_sciOld);
//LONG nExtentChangingH = _sciNew.GetExtentChangingH(_sciOld);
//LONG nExtentChangingV = _sciNew.GetExtentChangingV(_sciOld);
//bool bNothingChanged = true;
//	if( nPosChangingH != NULL )
//	{
//		bNothingChanged = false;
//		strLine.Format(
//			_T("            nPosChangingH=%ld\n"),
//			nPosChangingH
//			);
//		strTraceMsg += strLine;
//	}
//	if( nPosChangingV != NULL )
//	{
//		bNothingChanged = false;
//		strLine.Format(
//			_T("            nPosChangingV=%ld\n"),
//			nPosChangingV
//			);
//		strTraceMsg += strLine;
//	}
//	if( nPageSizeChangingH != NULL )
//	{
//		bNothingChanged = false;
//		strLine.Format(
//			_T("            nPageSizeChangingH=%ld\n"),
//			nPageSizeChangingH
//			);
//		strTraceMsg += strLine;
//	}
//	if( nPageSizeChangingV != NULL )
//	{
//		bNothingChanged = false;
//		strLine.Format(
//			_T("            nPageSizeChangingV=%ld\n"),
//			nPageSizeChangingV
//			);
//		strTraceMsg += strLine;
//	}
//	if( nExtentChangingH != NULL )
//	{
//		bNothingChanged = false;
//		strLine.Format(
//			_T("            nExtentChangingH=%ld\n"),
//			nExtentChangingH
//			);
//		strTraceMsg += strLine;
//	}
//	if( nExtentChangingV != NULL )
//	{
//		bNothingChanged = false;
//		strLine.Format(
//			_T("            nExtentChangingV=%ld\n"),
//			nExtentChangingV
//			);
//		strTraceMsg += strLine;
//	}
//	if( bNothingChanged )
//		strTraceMsg += _T("            (nothing changed)\n");
//	strTraceMsg;
//	TRACE0( LPCTSTR(strTraceMsg) );
#endif // _DEBUG

	return true;
}

bool CExtGridDataProviderBL::CacheIsVisibleFirstRecord( bool bHorz )
{
	ASSERT_VALID( this );
	bHorz;
	return true;
}

bool CExtGridDataProviderBL::CacheIsVisibleLastRecord( bool bHorz )
{
	ASSERT_VALID( this );
	bHorz;
	return true;
}

ULONG CExtGridDataProviderBL::CacheColumnCountGet()
{
	ASSERT_VALID( this );
	return ColumnCountGet();
}

ULONG CExtGridDataProviderBL::CacheRowCountGet()
{
	ASSERT_VALID( this );
	return RowCountGet();
}

/////////////////////////////////////////////////////////////////////////////
// CExtGridDataProviderRT
// (data provider which retranslates method calls to other
// agregated data provider object with/without X/Y flipping)

IMPLEMENT_DYNAMIC( CExtGridDataProviderRT, CExtGridDataProvider );

CExtGridDataProviderRT::CExtGridDataProviderRT(
	CExtGridDataProvider * pOther, // = NULL
	bool bAutoDeleteOther, // = false
	bool bFlipHV // = false
	)
	: m_pOther( pOther )
	, m_bAutoDeleteOther( bAutoDeleteOther )
	, m_bFlipHV( bFlipHV )
{
#ifdef _DEBUG
	ASSERT( LPCVOID(m_pOther) != LPCVOID(this) );
	if( m_pOther != NULL )
	{
		ASSERT_VALID( m_pOther );
	}
#endif // _DEBUG
}

CExtGridDataProviderRT::~CExtGridDataProviderRT()
{
	if( m_bAutoDeleteOther && m_pOther != NULL )
	{
		ASSERT( LPCVOID(m_pOther) != LPCVOID(this) );
		delete m_pOther;
	}
}

#ifdef _DEBUG
void CExtGridDataProviderRT::AssertValid() const
{
	CExtGridDataProvider::AssertValid();
	ASSERT( m_pOther != NULL );
	ASSERT( LPCVOID(m_pOther) != LPCVOID(this) );
	ASSERT_VALID( m_pOther );
}
void CExtGridDataProviderRT::Dump( CDumpContext & dc ) const
{
	CExtGridDataProvider::Dump( dc );
}
#endif // _DEBUG

__EXT_MFC_SAFE_LPTSTR CExtGridDataProviderRT::StringAlloc(
	INT nCharCountIncZT
	)
{
	ASSERT_VALID( this );
	return m_pOther->StringAlloc( nCharCountIncZT );
}

void CExtGridDataProviderRT::StringFree(
	__EXT_MFC_SAFE_LPTSTR strToFree
	)
{
	ASSERT_VALID( this );
	m_pOther->StringFree( strToFree );
}

bool CExtGridDataProviderRT::ColumnInsert(
	ULONG nColNo,
	ULONG nInsertCount // = 1
	)
{
	ASSERT_VALID( this );
	if( m_bFlipHV )
		return m_pOther->RowInsert( nColNo, nInsertCount );
	return m_pOther->ColumnInsert( nColNo, nInsertCount );
}

bool CExtGridDataProviderRT::RowInsert(
	ULONG nRowNo,
	ULONG nInsertCount // = 1
	)
{
	ASSERT_VALID( this );
	if( m_bFlipHV )
		return m_pOther->ColumnInsert( nRowNo, nInsertCount );
	return m_pOther->RowInsert( nRowNo, nInsertCount );
}

ULONG CExtGridDataProviderRT::ColumnCountGet() const
{
	ASSERT_VALID( this );
	if( m_bFlipHV )
		return m_pOther->RowCountGet();
	return m_pOther->ColumnCountGet();
}

ULONG CExtGridDataProviderRT::RowCountGet() const
{
	ASSERT_VALID( this );
	if( m_bFlipHV )
		return m_pOther->ColumnCountGet();
	return m_pOther->RowCountGet();
}

bool CExtGridDataProviderRT::ColumnRemove(
	ULONG nColNo,
	ULONG nRemoveCount // = 1
	)
{
	ASSERT_VALID( this );
	if( m_bFlipHV )
		return m_pOther->RowRemove( nColNo, nRemoveCount );
	return m_pOther->ColumnRemove( nColNo, nRemoveCount );
}

bool CExtGridDataProviderRT::RowRemove(
	ULONG nRowNo,
	ULONG nRemoveCount // = 1
	)
{
	ASSERT_VALID( this );
	if( m_bFlipHV )
		return m_pOther->ColumnRemove( nRowNo, nRemoveCount );
	return m_pOther->RowRemove( nRowNo, nRemoveCount );
}

void CExtGridDataProviderRT::MinimizeMemoryUsage()
{
	ASSERT_VALID( this );
	m_pOther->MinimizeMemoryUsage();
}

bool CExtGridDataProviderRT::RowDefaultValueBind(
	ULONG nRowNo, // = (ULONG(-1)) // if (ULONG(-1)) - default value for all rows
	CExtGridCell * pCell // = NULL // if NULL - remove default value
	)
{
	ASSERT_VALID( this );
	if( m_bFlipHV )
		return m_pOther->ColumnDefaultValueBind( nRowNo, pCell );
	return m_pOther->RowDefaultValueBind( nRowNo, pCell );
}

bool CExtGridDataProviderRT::ColumnDefaultValueBind(
	ULONG nColNo, // = (ULONG(-1)) // if (ULONG(-1)) - default value for all columns
	CExtGridCell * pCell // = NULL // if NULL - remove default value
	)
{
	ASSERT_VALID( this );
	if( m_bFlipHV )
		return m_pOther->RowDefaultValueBind( nColNo, pCell );
	return m_pOther->ColumnDefaultValueBind( nColNo, pCell );
}

void CExtGridDataProviderRT::RowDefaultValueUnbindAll()
{
	ASSERT_VALID( this );
	if( m_bFlipHV )
		m_pOther->ColumnDefaultValueUnbindAll();
	else
		m_pOther->RowDefaultValueUnbindAll();
}

void CExtGridDataProviderRT::ColumnDefaultValueUnbindAll()
{
	ASSERT_VALID( this );
	if( m_bFlipHV )
		m_pOther->RowDefaultValueUnbindAll();
	else
		m_pOther->ColumnDefaultValueUnbindAll();
}

CExtGridCell * CExtGridDataProviderRT::RowDefaultValueGet(
	ULONG nRowNo
	)
{
	ASSERT_VALID( this );
	if( m_bFlipHV )
		return m_pOther->ColumnDefaultValueGet( nRowNo );
	return m_pOther->RowDefaultValueGet( nRowNo );
}

CExtGridCell * CExtGridDataProviderRT::ColumnDefaultValueGet(
	ULONG nColNo
	)
{
	ASSERT_VALID( this );
	if( m_bFlipHV )
		return m_pOther->RowDefaultValueGet( nColNo );
	return m_pOther->ColumnDefaultValueGet( nColNo );
}

CExtGridCell * CExtGridDataProviderRT::CellGet(
	ULONG nColNo,
	ULONG nRowNo,
	CRuntimeClass * pInitRTC, // = NULL
	bool bAutoFindValue, // = true // auto find row/column default value (only when pInitRTC=NULL)
	bool bUseColumnDefaultValue // = true // false - use row default value (only when bAutoFindValue=true)
	)
{
	ASSERT_VALID( this );
	return
		m_pOther->CellGet(
			m_bFlipHV ? nRowNo : nColNo,
			m_bFlipHV ? nColNo : nRowNo,
			pInitRTC,
			bAutoFindValue,
			m_bFlipHV ? (!bUseColumnDefaultValue) : bUseColumnDefaultValue
			);
}

bool CExtGridDataProviderRT::CellRangeSet(
	ULONG nColNo,
	ULONG nRowNo,
	ULONG nColCount, // = 1L
	ULONG nRowCount, // = 1L
	const CExtGridCell * pCellNewValue, // = NULL // if NULL - empty existing cell values
	bool bReplace, // = false // false - assign to existing cell instances or column/row type created cells, true - create new cloned copies of pCellNewValue
	CRuntimeClass * pInitRTC, // = NULL // runtime class for new cell instance (used if bReplace=false)
	bool bAutoFindValue, // = true // auto find row/column default value (only when pInitRTC=NULL)
	bool bUseColumnDefaultValue, // = true // false - use row default value (only when bAutoFindValue=true)
	ULONG * p_nUpdatedCellCount // = NULL // returns count of really updated cells (zero also may be treated as success)
	)
{
	ASSERT_VALID( this );
	return
		m_pOther->CellRangeSet(
			m_bFlipHV ? nRowNo : nColNo,
			m_bFlipHV ? nColNo : nRowNo,
			m_bFlipHV ? nRowCount : nColCount,
			m_bFlipHV ? nColCount : nRowCount,
			pCellNewValue,
			bReplace,
			pInitRTC,
			bAutoFindValue,
			m_bFlipHV ? (!bUseColumnDefaultValue) : bUseColumnDefaultValue,
			p_nUpdatedCellCount
			);
}

bool CExtGridDataProviderRT::CacheReserveForOuterCells(
	ULONG nColCount,
	ULONG nRowCount
	)
{
	ASSERT_VALID( this );
	return
		m_pOther->CacheReserveForOuterCells(
			m_bFlipHV ? nRowCount : nColCount,
			m_bFlipHV ? nColCount : nRowCount
			);
}

void CExtGridDataProviderRT::CacheReservedCountsGet(
	ULONG * p_nColCount,
	ULONG * p_nRowCount
	) const
{
	ASSERT_VALID( this );
	m_pOther->CacheReservedCountsGet(
		m_bFlipHV ? p_nRowCount : p_nColCount,
		m_bFlipHV ? p_nColCount : p_nRowCount
		);
}

bool CExtGridDataProviderRT::CacheData(
	const CExtScrollItemCacheInfo & _sciNew,
	const CExtScrollItemCacheInfo & _sciOld
	)
{
	ASSERT_VALID( this );
	if( m_bFlipHV )
	{
		CExtScrollItemCacheInfo _sciNewFlipped( _sciNew );
		CExtScrollItemCacheInfo _sciOldFlipped( _sciOld );
		_sciNewFlipped.FlipHV();
		_sciOldFlipped.FlipHV();
		return
			m_pOther->CacheData(
				_sciNewFlipped,
				_sciOldFlipped
				);
	} // if( m_bFlipHV )
	return
		m_pOther->CacheData(
			_sciNew,
			_sciOld
			);
}

bool CExtGridDataProviderRT::CacheIsVisibleFirstRecord( bool bHorz )
{
	ASSERT_VALID( this );
	return
		m_pOther->CacheIsVisibleFirstRecord(
			m_bFlipHV
				? ( ! bHorz )
				: bHorz
			);
}

bool CExtGridDataProviderRT::CacheIsVisibleLastRecord( bool bHorz )
{
	ASSERT_VALID( this );
	return
		m_pOther->CacheIsVisibleLastRecord(
			m_bFlipHV
				? ( ! bHorz )
				: bHorz
			);
}

ULONG CExtGridDataProviderRT::CacheColumnCountGet()
{
	ASSERT_VALID( this );
	if( m_bFlipHV )
		return m_pOther->CacheRowCountGet();
	return m_pOther->CacheColumnCountGet();
}

ULONG CExtGridDataProviderRT::CacheRowCountGet()
{
	ASSERT_VALID( this );
	if( m_bFlipHV )
		return m_pOther->CacheColumnCountGet();
	return m_pOther->CacheRowCountGet();
}

INT CExtGridDataProviderRT::IconGetCount()
{
	ASSERT_VALID( this );
	return m_pOther->IconGetCount();
}

CExtCmdIcon * CExtGridDataProviderRT::IconGetAt( INT nIdx )
{
	ASSERT_VALID( this );
	return m_pOther->IconGetAt( nIdx );
}

INT CExtGridDataProviderRT::IconInsert( // returns index or -1
	CExtCmdIcon * pIcon,
	INT nIdx, // = -1 // append
	bool bCopyIcon // = true
	)
{
	ASSERT_VALID( this );
	return m_pOther->IconInsert( pIcon, nIdx, bCopyIcon );
}

INT CExtGridDataProviderRT::IconRemove(
	INT nIdx, // = 0
	INT nCountToRemove // = -1 // all
	)
{
	ASSERT_VALID( this );
	return m_pOther->IconRemove( nIdx, nCountToRemove );
}

bool CExtGridDataProviderRT::SortOrderUpdate(
	bool bColumns, // true = sort order for columns, false - for rows
	CExtGridDataProvider::IDataProviderEvents * pDPE // = NULL
	)
{
	ASSERT_VALID( this );
	if( pDPE == NULL || (!m_bFlipHV) )
		return
			m_pOther->SortOrderUpdate(
				m_bFlipHV ? (!bColumns) : bColumns,
				pDPE
				);
	ASSERT( pDPE != NULL );
	ASSERT( m_bFlipHV );
IDataProviderEventsFlip _dpeFlip( pDPE );
	return
		m_pOther->SortOrderUpdate(
			(!bColumns),
			&_dpeFlip
			);
}

bool CExtGridDataProviderRT::SortOrderSet(
	const CExtGridDataSortOrder & _gdso,
	bool bColumns, // true = sort order for columns, false - for rows
	CExtGridDataProvider::IDataProviderEvents * pDPE // = NULL
	)
{
	ASSERT_VALID( this );
	ASSERT( _gdso.ItemsUnique() );
	if( pDPE == NULL || (!m_bFlipHV) )
		return
			m_pOther->SortOrderSet(
				_gdso,
				m_bFlipHV ? (!bColumns) : bColumns,
				pDPE
				);
	ASSERT( pDPE != NULL );
	ASSERT( m_bFlipHV );
IDataProviderEventsFlip _dpeFlip( pDPE );
	return
		m_pOther->SortOrderSet(
			_gdso,
			(!bColumns),
			&_dpeFlip
			);
}

bool CExtGridDataProviderRT::SortOrderGet(
	CExtGridDataSortOrder & _gdso,
	bool bColumns // true = sort order for columns, false - for rows
	) const
{
	ASSERT_VALID( this );
	return
		m_pOther->SortOrderGet(
			_gdso,
			m_bFlipHV ? (!bColumns) : bColumns
			);
}

bool CExtGridDataProviderRT::SwapDroppedSeries(
	bool bColumns, // true = swap columns, false - rows
	ULONG nRowColNoSrc,
	ULONG nRowColNoDropBefore,
	IDataProviderEvents * pDPE // = NULL
	)
{
	ASSERT_VALID( this );
	if( pDPE == NULL || (!m_bFlipHV) )
		return
			m_pOther->SwapDroppedSeries(
				m_bFlipHV ? (!bColumns) : bColumns,
				nRowColNoSrc,
				nRowColNoDropBefore,
				pDPE
				);
	ASSERT( pDPE != NULL );
	ASSERT( m_bFlipHV );
IDataProviderEventsFlip _dpeFlip( pDPE );
	return
		m_pOther->SwapDroppedSeries(
			(!bColumns),
			nRowColNoSrc,
			nRowColNoDropBefore,
			&_dpeFlip
			);
}

/////////////////////////////////////////////////////////////////////////////
// CExtGridDataProviderRecordset
// (cached data provider for vertical static/virtual modes)

IMPLEMENT_DYNAMIC( CExtGridDataProviderRecordset, CExtGridDataProviderRT );

CExtGridDataProviderRecordset::CExtGridDataProviderRecordset()
	: m_nRowNoHeader( 0L )
	, m_bHeaderInitialized( false )
	, m_bVisibleFirstRecord( true )
	, m_bVisibleLastRecord( false )
	, m_nVirtualPageSize( 0L )
	, m_nVirtualRsOffset( 0L )
{
}

CExtGridDataProviderRecordset::~CExtGridDataProviderRecordset()
{
}

void CExtGridDataProviderRecordset::_VCS_ToPageStart()
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pOther );
	for( ; m_nVirtualRsOffset != 0L; )
	{
		if( m_nVirtualRsOffset < 0L )
		{
			if( RsIsEOF() )
			{
				m_bVisibleLastRecord = true;
				m_nVirtualRsOffset = 0L;
				RsSeekToPrevious();
				break;
			}
			RsSeekToNext();
			m_nVirtualRsOffset ++;
			m_bVisibleLastRecord = false;
		} // if( m_nVirtualRsOffset < 0L )
		else
		{
			ASSERT( m_nVirtualRsOffset > 0L );
			if( RsIsBOF() )
			{
				m_nVirtualRsOffset = 0L;
				m_bVisibleFirstRecord = true;
				RsSeekToNext();
				break;
			}
			RsSeekToPrevious();
			m_nVirtualRsOffset --;
			m_bVisibleFirstRecord = false;
		} // else from if( m_nVirtualRsOffset < 0L )
	}
}

void CExtGridDataProviderRecordset::_VCS_FromCurrent( LONG nPosChangingV )
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pOther );
	for( ; nPosChangingV != 0L;  )
	{
		if( nPosChangingV > 0L )
		{
			if( RsIsEOF() )
			{
				nPosChangingV = 0L;
				m_bVisibleLastRecord = true;
				RsSeekToPrevious();
				break;
			}
			RsSeekToNext();
			nPosChangingV --;
			m_bVisibleLastRecord = false;
		} // if( nPosChangingV > 0L )
		else
		{
			ASSERT( nPosChangingV < 0L );
			if( RsIsBOF() )
			{
				nPosChangingV = 0L;
				m_bVisibleFirstRecord = true;
				RsSeekToNext();
				break;
			}
			RsSeekToPrevious();
			nPosChangingV ++;
			m_bVisibleFirstRecord = false;
		} // else from if( nPosChangingV > 0L )
	}
}

#ifdef _DEBUG
void CExtGridDataProviderRecordset::AssertValid() const
{
	CExtGridDataProviderRT::AssertValid();
	if( m_pOther != NULL )
	{
		ASSERT_VALID( m_pOther );
	}
	ASSERT( ! m_bFlipHV );
	ASSERT( ! m_bAutoDeleteOther );
}
#endif // _DEBUG

bool CExtGridDataProviderRecordset::CacheReserveForOuterCells(
	ULONG nColCount,
	ULONG nRowCount
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pOther );
ULONG nColCountReserved = 0L, nRowCountReserved = 0L;
	CacheReservedCountsGet(
		&nColCountReserved,
		&nRowCountReserved
		);
	m_pOther->CacheReserveForOuterCells(
		0L,
		0L
		);
	if( nColCountReserved > nColCount )
	{
		m_pOther->ColumnRemove(
			nColCount,
			nColCountReserved - nColCount
			);
		m_bHeaderInitialized = false;
	} // if( nColCountReserved > nColCount )
	else if( nColCountReserved < nColCount )
	{
		m_pOther->ColumnInsert(
			nColCountReserved,
			nColCount - nColCountReserved
			);
		m_bHeaderInitialized = false;
	} // else if( nColCountReserved < nColCount )
	if( nRowCountReserved > nRowCount )
	{
		m_pOther->RowRemove(
			nRowCount,
			nRowCountReserved - nRowCount
			);
		m_bHeaderInitialized = false;
	} // if( nRowCountReserved > nRowCount )
	else if( nRowCountReserved < nRowCount )
	{
		m_pOther->RowInsert(
			nRowCountReserved,
			nRowCount - nRowCountReserved
			);
		m_bHeaderInitialized = false;
	} // else if( nRowCountReserved < nRowCount )
	m_pOther->CacheReserveForOuterCells(
		nColCount,
		nRowCount
		);
	CacheReservedCountsGet(
		&nColCountReserved,
		&nRowCountReserved
		);
	if( m_nRowNoHeader > nRowCountReserved )
		m_bHeaderInitialized = false;
	if(		( ! m_bHeaderInitialized )
		&&	CacheColumnCountGet() > 0
		)
	{ // init header cells
		try
		{
			if( nRowCount > 0 )
			{
				RsCacheRow(
					nColCountReserved,
					m_nRowNoHeader,
					true
					);
			} // if( nRowCount > 0 )
			m_bHeaderInitialized = true;
		} // try
		catch( CException * pXept )
		{
#ifdef _DEBUG
			TCHAR strError[4096];
			if( pXept->GetErrorMessage(strError, 4096-1 ) )
			{
				TRACE1(
					"CExtGridDataProviderRecordset::CacheDataStatic() caught an exception:\n    \"%s\"\n",
					strError
					);
			} // if( pException->GetErrorMessage(strError, 4096-1 ) )
#endif // _DEBUG
			ASSERT( FALSE );
			pXept->Delete();
			return false;
		} // catch( CException * pXept )
		catch( ... )
		{
			ASSERT( FALSE );
			return false;
		} // catch( ... )
	} // init header cells
	return true;
}

bool CExtGridDataProviderRecordset::CacheDataStatic(
	const CExtScrollItemCacheInfo & _sciNew,
	const CExtScrollItemCacheInfo & _sciOld
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pOther );
	ASSERT( _sciNew.IsValid() );
	ASSERT( _sciOld.IsValid() );
	ASSERT( _sciNew != _sciOld );
ULONG nColCountReserved = 0L, nRowCountReserved = 0L;
	CacheReservedCountsGet(
		&nColCountReserved,
		&nRowCountReserved
		);
ULONG nCacheRowCount = m_pOther->RowCountGet();
	ASSERT( nCacheRowCount >= nRowCountReserved );
ULONG nCurrPageSize = nCacheRowCount - nRowCountReserved;
	if( _sciNew.m_nPageSizeV == 0L )
	{
		if( nCurrPageSize > 0L )
		{
			VERIFY(
				m_pOther->RowRemove(
					nRowCountReserved,
					nCurrPageSize
					)
				);
		} // if( nCurrPageSize > 0L )
		ColumnCountGet(); // sync col count
		m_sci = _sciNew;
		return true;
	} // if( _sciNew.m_nPageSizeV == 0L )
LONG nPosChangingV = _sciNew.GetPosChangingV( _sciOld );
LONG nPageSizeChangingV = _sciNew.GetPageSizeChangingV( _sciOld );
LONG nMaxChangingV = _sciNew.GetMaxChangingV( _sciOld );
	if(		nPosChangingV == 0L
		&&	nPageSizeChangingV == 0L
		&&	nMaxChangingV == 0L
		)
	{
		ColumnCountGet(); // sync col count
		m_sci = _sciNew;
		return true;
	} // if( nPosChangingV == 0L ...
ULONG nAbsShiftV = (ULONG) ( abs( nPosChangingV ) );
ULONG nRowLoadFirst = 0L;
	ASSERT( _sciNew.m_nPageSizeV > 0L );
ULONG nRowLoadCount = (ULONG)_sciNew.m_nPageSizeV;
ULONG nRowCountReal = (ULONG)RsRecordsCount();
	if( nRowLoadCount > nRowCountReal )
		nRowLoadCount = nRowCountReal;
	if( nPosChangingV != 0L && nCurrPageSize > 0 )
	{
		if( nAbsShiftV >= nCurrPageSize )
		{
			VERIFY(
				m_pOther->RowRemove(
					nRowCountReserved,
					nCurrPageSize
					)
				);
			nCurrPageSize = 0;
			ASSERT( m_pOther->RowCountGet() == nRowCountReserved );
		} // if( nAbsShiftV >= nCurrPageSize )
		else
		{
			if( nPosChangingV > 0L )
			{
				VERIFY(
					m_pOther->RowRemove(
						nRowCountReserved,
						nAbsShiftV
						)
					);
				nCurrPageSize -= nAbsShiftV;
				nRowLoadFirst += nCurrPageSize;
				nRowLoadCount = nAbsShiftV;
				ASSERT( m_pOther->RowCountGet() >= nRowCountReserved );
			} // if( nPosChangingV > 0L )
			else
			{
				ASSERT( nPosChangingV < 0L );
				VERIFY(
					m_pOther->RowInsert(
						nRowCountReserved,
						nAbsShiftV
						)
					);
				nCurrPageSize += nAbsShiftV;
				nRowLoadCount = nAbsShiftV;
			} // else from if( nPosChangingV > 0L )
		} // else from if( nAbsShiftV >= nCurrPageSize )
	} // if( nPosChangingV != 0L && nCurrPageSize > 0 )
	
	ASSERT( _sciNew.m_nPageSizeV > 0 );
	if( (ULONG)_sciNew.m_nPageSizeV > nCurrPageSize )
	{
		VERIFY(
			m_pOther->RowInsert(
				nRowCountReserved + nCurrPageSize,
				(ULONG)_sciNew.m_nPageSizeV - nCurrPageSize
				)
			);
		nCurrPageSize = (ULONG)_sciNew.m_nPageSizeV;
	} // if( (ULONG)_sciNew.m_nPageSizeV > nCurrPageSize )
	else if( (ULONG)_sciNew.m_nPageSizeV < nCurrPageSize )
	{
		VERIFY(
			m_pOther->RowRemove(
				nRowCountReserved + (ULONG)_sciNew.m_nPageSizeV,
				nCurrPageSize - (ULONG)_sciNew.m_nPageSizeV
				)
			);
		nCurrPageSize = (ULONG)_sciNew.m_nPageSizeV;
	} // else if( (ULONG)_sciNew.m_nPageSizeV < nCurrPageSize )

	if( nRowLoadFirst >= nCurrPageSize || nRowLoadCount == 0L )
	{
		ColumnCountGet(); // sync col count
		m_sci = _sciNew;
		return true;
	} // if( nRowLoadFirst >= nCurrPageSize || nRowLoadCount == 0L )
	if( (nRowLoadFirst + nRowLoadCount) > nCurrPageSize )
	{
		nRowLoadCount = nCurrPageSize - nRowLoadFirst;
		ASSERT( LONG(nRowLoadCount) > 0L );
	}
	ULONG nRowInPageShift = nRowLoadFirst;
	nRowLoadFirst += (ULONG)_sciNew.m_nPosV;

	try
	{
		RsSeekFromStart( nRowLoadFirst );
		LONG nColCount = RsFieldsCount();
		ColumnCountGet(); // sync col count
		ASSERT(
			m_pOther->ColumnCountGet() ==
				(nColCount + nColCountReserved )
			);
		for( LONG nRowNo = 0L; nRowNo < LONG(nRowLoadCount); nRowNo++ )
		{
			RsCacheRow(
				nColCountReserved,
				nRowNo + nRowInPageShift + nRowCountReserved,
				false
				);
			RsSeekToNext();
		}
		m_sci = _sciNew;
	} // try
	catch( CException * pXept )
	{
#ifdef _DEBUG
		TCHAR strError[4096];
		if( pXept->GetErrorMessage(strError, 4096-1 ) )
		{
			TRACE1(
				"CExtGridDataProviderRecordset::CacheDataStatic() caught an exception:\n    \"%s\"\n",
				strError
				);
		} // if( pException->GetErrorMessage(strError, 4096-1 ) )
#endif // _DEBUG
		ASSERT( FALSE );
		pXept->Delete();
		return false;
	} // catch( CException * pXept )
	catch( ... )
	{
		ASSERT( FALSE );
	} // catch( ... )
	return true;
}

bool CExtGridDataProviderRecordset::CacheDataVirtual(
	const CExtScrollItemCacheInfo & _sciNew,
	const CExtScrollItemCacheInfo & _sciOld
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pOther );
	ASSERT( _sciNew.IsValid() );
	ASSERT( _sciOld.IsValid() );
	ASSERT( _sciNew != _sciOld );
ULONG nColCountReserved = 0L, nRowCountReserved = 0L;
	CacheReservedCountsGet(
		&nColCountReserved,
		&nRowCountReserved
		);
LONG nColCount = ColumnCountGet() - nColCountReserved; // sync col count
	ASSERT( nColCount > 0L );
	nColCount;
ULONG nCacheRowCount = m_pOther->RowCountGet();
	ASSERT( nCacheRowCount >= nRowCountReserved );
	nCacheRowCount;
ULONG nCurrPageSize = nCacheRowCount - nRowCountReserved;
LONG nNewPageSize = _sciNew.m_nPageSizeV;
	if( nNewPageSize == 0L )
	{
		m_bVisibleFirstRecord = true;
		m_bVisibleLastRecord = true;
		if( nCurrPageSize > 0L )
		{
			VERIFY(
				m_pOther->RowRemove(
					nRowCountReserved,
					nCurrPageSize
					)
				);
		} // if( nCurrPageSize > 0L )
		m_sci = _sciNew;
		return true;
	} // if( nNewPageSize == 0L )

LONG nPosChangingV = _sciNew.GetPosChangingV( _sciOld );
	if(		abs(nPosChangingV) >= (max(nNewPageSize,_sciOld.m_nPageSizeV))
		||	_sciNew.m_bBeginV
		||	_sciNew.m_bEndV
		)
		nPosChangingV = 0L;
	if(		nPosChangingV == 0L
		&&	_sciNew.GetPageSizeChangingV( _sciOld ) == 0L
		&&	( ! ( _sciNew.m_bBeginV || _sciNew.m_bEndV ) )
		)
	{
		m_sci = _sciNew;
		return true;
	}

	if( nCurrPageSize > 0L )
	{
		VERIFY(
			m_pOther->RowRemove(
				nRowCountReserved,
				nCurrPageSize
				)
			);
		nCurrPageSize = 0L;
	} // if( nCurrPageSize > 0L )

	try
	{
		m_nVirtualPageSize = 0L;
		if( _sciNew.m_bBeginV )
			m_bVisibleFirstRecord = true;
		if( _sciNew.m_bEndV )
			m_bVisibleLastRecord = true;
		if( m_bVisibleFirstRecord )
		{
			RsSeekToFirst();
			m_nVirtualRsOffset = 0L;
		} // if( m_bVisibleFirstRecord )
		if( RsIsEOF() && RsIsBOF() )
		{
			m_sci = _sciNew;
			return true;
		}
		if( _sciNew.m_bEndV )
		{
			m_nVirtualRsOffset = 0L;
			RsSeekToLast();
		} // if( _sciNew.m_bEndV )
		else if( _sciNew.m_bBeginV )
		{
			m_nVirtualRsOffset = 0L;
			RsSeekToFirst();
		} // else if( _sciNew.m_bBeginV )
		else
		{
			_VCS_ToPageStart();
			_VCS_FromCurrent( nPosChangingV );
		}
		if( RsIsEOF() )
		{
			for(	ULONG nRowNo = 0L;
					nRowNo < ULONG(nNewPageSize);
					nRowNo++
					)
			{
				RsSeekToPrevious();
				if( RsIsBOF() )
					break;
			}
		}
		if( RsIsBOF() )
			RsSeekToNext();
		ASSERT( ! RsIsBOF() );
		ASSERT( ! RsIsEOF() );
		
		m_bVisibleFirstRecord = false;
		m_bVisibleLastRecord = false;
		RsSeekToPrevious();
		if( RsIsBOF() )
			m_bVisibleFirstRecord = true;
		RsSeekToNext();

		for(	ULONG nRowNo = 0L;
				nRowNo < ULONG(nNewPageSize);
				nRowNo++, m_nVirtualPageSize++, m_nVirtualRsOffset++
				)
		{
			if( RsIsEOF() )
			{
				RsSeekToPrevious();
				m_nVirtualRsOffset--;
				m_bVisibleLastRecord = true;
				if(		nNewPageSize > m_nVirtualPageSize
					&&	m_nVirtualPageSize > 0
					&&	( ! _sciNew.m_bEndV )
					)
					nNewPageSize--;
				break;
			}
			VERIFY(
				m_pOther->RowInsert(
					nRowNo + nRowCountReserved,
					1L
					)
				);
			RsCacheRow(
				nColCountReserved,
				nRowNo + nRowCountReserved,
				false
				);
			RsSeekToNext();
		}
		if( ! RsIsEOF() )
		{
			RsSeekToNext();
			if( RsIsEOF() )
				m_bVisibleLastRecord = true;
			RsSeekToPrevious();
		}
		ASSERT( m_nVirtualPageSize <= nNewPageSize );
		if( m_nVirtualPageSize < nNewPageSize )
		{
			_VCS_ToPageStart();
			ASSERT( ! RsIsEOF() );
			m_nVirtualRsOffset = 0L;
			for( ; m_nVirtualPageSize < nNewPageSize; )
			{
				RsSeekToPrevious();
				if( RsIsBOF() )
				{
					RsSeekToNext();
					m_bVisibleFirstRecord = true;
					break;
				}
				m_nVirtualPageSize++;
				VERIFY(
					m_pOther->RowInsert(
						nRowCountReserved,
						1L
						)
					);
				RsCacheRow(
					nColCountReserved,
					nRowCountReserved,
					false
					);
			} // for( ; m_nVirtualPageSize < nNewPageSize; )
		} // if( m_nVirtualPageSize < nNewPageSize )
	} // try
	catch( CException * pXept )
	{
#ifdef _DEBUG
		TCHAR strError[4096];
		if( pXept->GetErrorMessage(strError, 4096-1 ) )
		{
			TRACE1(
				"CExtGridDataProviderRecordset::CacheDataVirtual() caught an exception:\n    \"%s\"\n",
				strError
				);
		} // if( pException->GetErrorMessage(strError, 4096-1 ) )
#endif // _DEBUG
		ASSERT( FALSE );
		pXept->Delete();
		return false;
	} // catch( CException * pXept )
	catch( ... )
	{
		ASSERT( FALSE );
		return false;
	} // catch( ... )
	m_sci = _sciNew;
	return true;
}

bool CExtGridDataProviderRecordset::CacheData(
	const CExtScrollItemCacheInfo & _sciNew,
	const CExtScrollItemCacheInfo & _sciOld
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pOther );
	ASSERT( _sciNew.IsValid() );
	ASSERT( _sciOld.IsValid() );
	if( _sciNew == _sciOld )
		return true;
	if( RsIsVirtualMode() )
		return CacheDataVirtual( _sciNew, _sciOld );
	else
		return CacheDataStatic( _sciNew, _sciOld );
}

bool CExtGridDataProviderRecordset::CacheIsVisibleFirstRecord( bool bHorz )
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pOther );
	ASSERT( ( ! bHorz ) );
	bHorz;
	return m_bVisibleFirstRecord;
}

bool CExtGridDataProviderRecordset::CacheIsVisibleLastRecord( bool bHorz )
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pOther );
	ASSERT( ( ! bHorz ) );
	bHorz;
	if( m_bVisibleLastRecord && (m_nVirtualPageSize <= m_sci.m_nPageSizeV) )
		return true;
	return false;
}

ULONG CExtGridDataProviderRecordset::ColumnCountGet() const
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pOther );
ULONG nColCountReserved = 0L, nRowCountReserved = 0L;
	((CExtGridDataProvider *)(this)) ->
		CacheReservedCountsGet(
			&nColCountReserved,
			&nRowCountReserved
			);
ULONG nCount = (ULONG)RsFieldsCount();
	nCount += nColCountReserved;
ULONG nCachedCols = m_pOther->ColumnCountGet();
	if( nCount != nCachedCols )
	{
		if( nCachedCols < nCount  )
			m_pOther->ColumnInsert(
				0,
				nCount - nCachedCols
				);
		else
			m_pOther->ColumnRemove(
				nCount,
				nCachedCols - nCount
				);
		ASSERT( m_pOther->ColumnCountGet() == nCount );
	} // if( nCount != nCachedCols )
	return nCount;
}

ULONG CExtGridDataProviderRecordset::RowCountGet() const
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pOther );
	if( RsIsVirtualMode() )
		return m_pOther->RowCountGet();
ULONG nColCountReserved = 0L, nRowCountReserved = 0L;
	((CExtGridDataProvider *)(this)) ->
		CacheReservedCountsGet(
			&nColCountReserved,
			&nRowCountReserved
			);
LONG nCount = RsRecordsCount();
	ASSERT( nCount >= 0L );
	nCount += nRowCountReserved;
	ASSERT( nCount >= 0L );
	return (ULONG)nCount;
}

ULONG CExtGridDataProviderRecordset::CacheColumnCountGet()
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pOther );
ULONG nRetVal = ColumnCountGet();
	return nRetVal;
}

ULONG CExtGridDataProviderRecordset::CacheRowCountGet()
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pOther );
ULONG nColCountReserved = 0L, nRowCountReserved = 0L;
	CacheReservedCountsGet(
		&nColCountReserved,
		&nRowCountReserved
		);
ULONG nRetVal = m_nVirtualPageSize + nRowCountReserved;
	return nRetVal;
}

CExtGridCell * CExtGridDataProviderRecordset::CellGet(
	ULONG nColNo,
	ULONG nRowNo,
	CRuntimeClass * pInitRTC, // = NULL
	bool bAutoFindValue, // = true // auto find row/column default value (only when pInitRTC=NULL)
	bool bUseColumnDefaultValue // = true // false - use row default value (only when bAutoFindValue=true)
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pOther );
	if( RsIsVirtualMode() )
	{
		CExtGridCell * pCell =
			m_pOther->CellGet(
				nColNo,
				nRowNo,
				pInitRTC,
				bAutoFindValue,
				bUseColumnDefaultValue
				);
		return pCell;
	} // if( RsIsVirtualMode() )
ULONG nColCountReserved = 0L, nRowCountReserved = 0L;
	CacheReservedCountsGet(
		&nColCountReserved,
		&nRowCountReserved
		);
	if( nRowNo >= nRowCountReserved )
	{
		if( (nRowNo - nRowCountReserved) < ULONG(m_sci.m_nPosV) )
			return NULL;
		nRowNo -= ULONG(m_sci.m_nPosV);
	} // if( nRowNo >= nRowCountReserved )
	if(		nColNo >= m_pOther->ColumnCountGet()
		||	nRowNo >= m_pOther->RowCountGet()
		)
		return NULL; // out of cache
CExtGridCell * pCell =
		m_pOther->CellGet(
			nColNo,
			nRowNo,
			pInitRTC,
			bAutoFindValue,
			bUseColumnDefaultValue
			);
	return pCell;
}

bool CExtGridDataProviderRecordset::RsIsBOF() const
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pOther );
	return
		( const_cast < CExtGridDataProviderRecordset * > ( this ) )
			-> RsIsBOF();
}

bool CExtGridDataProviderRecordset::RsIsEOF() const
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pOther );
	return
		( const_cast < CExtGridDataProviderRecordset * > ( this ) )
			-> RsIsEOF();
}

bool CExtGridDataProviderRecordset::RsIsVirtualMode() const
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pOther );
	return
		( const_cast < CExtGridDataProviderRecordset * > ( this ) )
			-> RsIsVirtualMode();
}

LONG CExtGridDataProviderRecordset::RsFieldsCount() const
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pOther );
	return
		( const_cast < CExtGridDataProviderRecordset * > ( this ) )
			-> RsFieldsCount();
}

LONG CExtGridDataProviderRecordset::RsRecordsCount() const
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pOther );
	return
		( const_cast < CExtGridDataProviderRecordset * > ( this ) )
			-> RsRecordsCount();
}

bool CExtGridDataProviderRecordset::SortOrderUpdate(
	bool bColumns, // true = sort order for columns, false - for rows
	CExtGridDataProvider::IDataProviderEvents * pDPE // = NULL
	)
{
	ASSERT_VALID( this );
	bColumns;
	pDPE;
	return false; // not supported
}

bool CExtGridDataProviderRecordset::SortOrderSet(
	const CExtGridDataSortOrder & _gdso,
	bool bColumns, // true = sort order for columns, false - for rows
	CExtGridDataProvider::IDataProviderEvents * pDPE // = NULL
	)
{
	ASSERT_VALID( this );
	ASSERT( _gdso.ItemsUnique() );
	_gdso;
	bColumns;
	pDPE;
	return false; // not supported
}

bool CExtGridDataProviderRecordset::SortOrderGet(
	CExtGridDataSortOrder & _gdso,
	bool bColumns // true = sort order for columns, false - for rows
	) const
{
	ASSERT_VALID( this );
	_gdso;
	bColumns;
	return false; // not supported
}

bool CExtGridDataProviderRecordset::SwapDroppedSeries(
	bool bColumns, // true = swap columns, false - rows
	ULONG nRowColNoSrc,
	ULONG nRowColNoDropBefore,
	IDataProviderEvents * pDPE // = NULL
	)
{
	ASSERT_VALID( this );
	bColumns;
	nRowColNoSrc;
	nRowColNoDropBefore;
	pDPE;
	return false; // not supported
}

/////////////////////////////////////////////////////////////////////////////
// CExtGridDataProviderMemory
// (dynamic memory data provider based on different kinds of
// IMalloc objects, used as default data provider)

IMPLEMENT_DYNAMIC( CExtGridDataProviderMemory, CExtGridDataProviderBL );

CExtGridDataProviderMemory::CExtGridDataProviderMemory(
	IMalloc * pMallocV, // = NULL
	IMalloc * pMallocF, // = NULL
	IMalloc * pMallocD, // = NULL
	ULONG nStringAllocationAlign // = 32
	)
	: m_pMallocV( pMallocV )
	, m_pMallocF( pMallocF )
	, m_pMallocD( pMallocD )
	, m_nStringAllocationAlign( (nStringAllocationAlign > 1) ? nStringAllocationAlign : 1 )
	, m_nDX( 0L )
	, m_nDY( 0L )
	, m_pSections( NULL )
{
	ASSERT( sizeof(LPVOID) == sizeof(CExtGridCell*) );
	ASSERT( sizeof(LPVOID) == sizeof(CExtGridCell**) );
	ASSERT( sizeof(LPVOID) == sizeof(CExtGridCell***) );
	ASSERT( sizeof(LPVOID) == sizeof(CExtGridCell****) );
	if( m_pMallocF != NULL )
		m_pMallocF->AddRef();
	if( m_pMallocV != NULL )
		m_pMallocV->AddRef();
	if( m_pMallocD != NULL )
		m_pMallocD->AddRef();

	if( m_pMallocF == NULL )
	{
		m_pMallocF = CExtAlloc::CreateObject();
		if( m_pMallocF == NULL )
		{
			ASSERT( FALSE );
			::AfxThrowMemoryException();
		} // if( m_pMallocF == NULL )
	} // if( m_pMallocF == NULL )

bool bInternalVariableAllocNB = false;
	if( m_pMallocV == NULL )
	{
		if(		m_pMallocF != NULL
			&&	m_nStringAllocationAlign > 1
			)
		{ // if align specified - use the same allocator
			m_pMallocV = m_pMallocF;
			m_pMallocV->AddRef();
		} // if align specified - use the same allocator
		else
		{ // in other case - create separated non-blocked allocator
			m_pMallocV = CExtAlloc::CreateObject( 1 );
			if( m_pMallocV == NULL )
			{
				ASSERT( FALSE );
				::AfxThrowMemoryException();
			} // if( m_pMallocV == NULL )
			bInternalVariableAllocNB = true;
		} // in other case - create separated non-blocked allocator
	} // if( m_pMallocV == NULL )

	if( m_pMallocD == NULL )
	{
		if( bInternalVariableAllocNB )
		{ // if variable allocator is internal and non-blocked mode
			m_pMallocD = m_pMallocV;
			m_pMallocD->AddRef();
		} // if variable allocator is internal and non-blocked mode
		else
		{ // in other case - create separated non-blocked allocator
			m_pMallocD = CExtAlloc::CreateObject( 1 );
			if( m_pMallocD == NULL )
			{
				ASSERT( FALSE );
				::AfxThrowMemoryException();
			} // if( m_pMallocD == NULL )
		} // in other case - create separated non-blocked allocator
	} // if( m_pMallocD == NULL )
}

CExtGridDataProviderMemory::~CExtGridDataProviderMemory()
{
ULONG nColCount = ColumnCountGet();
ULONG nRowCount = RowCountGet();
	if( nColCount > 0 && nRowCount > 0 )
	{
		CellRangeEmpty(
			0,
			0,
			nColCount,
			nRowCount
			);
	} // if( nColCount > 0 && nRowCount > 0 )
	_SectionFreeAll( true );
	_DVM_RemoveAll( m_mapDVX );
	_DVM_RemoveAll( m_mapDVY );
	if( m_pMallocV != NULL )
		m_pMallocV->Release();
	if( m_pMallocF != NULL )
		m_pMallocF->Release();
	if( m_pMallocD != NULL )
		m_pMallocD->Release();
}

CExtGridCell ** CExtGridDataProviderMemory::_SectionGetAt(
	ULONG nX,
	bool bAllowDelayedAllocation
	)
{
	ASSERT_VALID( this );
	ASSERT( m_pSections != NULL );
	ASSERT( nX < m_nDX );
CExtGridCell ** pSection = m_pSections[ nX ];
	if( pSection != NULL )
		return pSection;
	if( !bAllowDelayedAllocation )
		return NULL;
	ASSERT( m_nDY >= 1 );
	pSection = (CExtGridCell **)
		m_pMallocD->Alloc( sizeof(LPVOID)*m_nDY );
	return pSection;
}

void CExtGridDataProviderMemory::_SectionFree( CExtGridCell ** pSection )
{
	ASSERT_VALID( this );
	ASSERT( pSection != NULL );
	ASSERT( m_nDY >= 1 );
	for( ULONG nIdx = 0; nIdx < m_nDY; nIdx++ )
	{
		CExtGridCell * pCell =
			pSection[ nIdx ];
		if( pCell == NULL )
			continue;
		ASSERT_VALID( pCell );
		ASSERT_KINDOF( CExtGridCell, pCell );
		ASSERT( pCell->DataProviderGet() == this );
		//ASSERT_VALID( pCell->DataProviderGet() );
		CExtGridCell::InstanceDestroy( pCell, m_pMallocF );
	} // for( ULONG nIdx = 0; nIdx < m_nDY; nIdx++ )
	m_pMallocD->Free( pSection );
}

void CExtGridDataProviderMemory::_SectionFreeAll( bool bReleaseSectionStack )
{
	ASSERT_VALID( this );
	if( m_pSections == NULL )
	{
		ASSERT( m_nDX == 0 );
		return;
	} // if( m_pSections == NULL )
	ASSERT( m_nDX > 0 );
	for( ULONG nX = 0; nX < m_nDX; nX++  )
	{
		CExtGridCell ** pSection =
			_SectionGetAt( nX, false );
		if( pSection != NULL )
		{
			ASSERT( m_nDY > 0 );
			_SectionFree( pSection );
			m_pSections[ nX ]  = NULL;
		} // if( pSection != NULL )
	} // for( ULONG nX = 0; nX < m_nDX; nX++  )
	if( !bReleaseSectionStack )
		return;
	m_pMallocD->Free( m_pSections );
	m_pSections = NULL;
	m_nDX = 0; // m_nDX = m_nDY = 0;
}

ULONG CExtGridDataProviderMemory::_SectionInsertAt(
	ULONG nX,
	ULONG nSectionCount,
	bool bForceDelayedRangeAllocation
	)
{
	ASSERT_VALID( this );
	ASSERT( nX <= m_nDX );
	ASSERT( nSectionCount >= 1 );
	if( m_nDY == 0 )
		bForceDelayedRangeAllocation = true;
	if( m_pSections == NULL )
	{
		ASSERT( m_nDX == 0 );
		ASSERT( nX == 0 );
		m_pSections = (CExtGridCell ***)
			m_pMallocD->Alloc(
				sizeof(LPVOID)*nSectionCount
				);
		if( m_pSections == NULL )
		{
			ASSERT( FALSE );
			return 0;
		} // if( m_pSections == NULL )
		m_nDX = nSectionCount;
		if( !bForceDelayedRangeAllocation )
		{
			ASSERT( m_nDY > 0 );
			for( ULONG nIdx = 0; nIdx < nSectionCount; nIdx++ )
			{
				ASSERT( m_pSections[nIdx] == NULL );
				m_pSections[nIdx] = (CExtGridCell **)
					m_pMallocD->Alloc( sizeof(LPVOID)*m_nDY );
				if( m_pSections[nIdx] == NULL )
					break; // delay rest of allocations
			} // for( ULONG nIdx = 0; nIdx < nSectionCount; nIdx++ )
		} // if( !bForceDelayedRangeAllocation )
	} // if( m_pSections == NULL )
	else
	{
		CExtGridCell *** pSectionsNew =
			(CExtGridCell ***)
				m_pMallocD->Realloc(
					m_pSections,
					sizeof(LPVOID)*(m_nDX + nSectionCount)
					);
		if( pSectionsNew == NULL )
		{
			ASSERT( FALSE );
			return 0;
		} // if( pSectionsNew == NULL )
		m_pSections = pSectionsNew;
		ULONG nOldDX = m_nDX;
		m_nDX += nSectionCount;
		if( nX < nOldDX )
		{
			ULONG nIdxDstSaved = m_nDX - 1;
			ULONG nIdxSrcSaved = nIdxDstSaved - nSectionCount;
			ULONG nIdxDst = nIdxDstSaved;
			ULONG nIdxSrc = nIdxSrcSaved;
			for(	ULONG nIdxMeasure = m_nDX - nSectionCount;
					nIdxMeasure > nX;
					nIdxMeasure--, nIdxSrc--, nIdxDst--
					)
			{
				m_pSections[nIdxDst] = m_pSections[nIdxSrc];
				m_pSections[nIdxSrc] = NULL;
			}
			if( !bForceDelayedRangeAllocation )
			{
				ASSERT( m_nDY > 0 );
				for(	nIdxSrc = nX, nIdxMeasure = 0L;
						nIdxMeasure < nSectionCount;
						nIdxMeasure++, nIdxSrc++
						)
				{
					ASSERT( m_pSections[nIdxSrc] == NULL );
					m_pSections[nIdxSrc] = (CExtGridCell **)
						m_pMallocD->Alloc( sizeof(LPVOID)*m_nDY );
					if( m_pSections[nIdxSrc] == NULL )
						break; // delay rest of allocations
				}
			} // if( !bForceDelayedRangeAllocation )
		} // if( nX < nOldDX )
		else
		{
			if( !bForceDelayedRangeAllocation )
			{
				ASSERT( m_nDY > 0 );
				ULONG nIdxDst = m_nDX - 1;
				ULONG nIdxMeasure = m_nDX;
				for( ; nIdxMeasure > nOldDX; nIdxMeasure--, nIdxDst-- )
				{
					ASSERT( m_pSections[nIdxDst] == NULL );
					m_pSections[nIdxDst] = (CExtGridCell **)
						m_pMallocD->Alloc( sizeof(LPVOID)*m_nDY );
					if( m_pSections[nIdxDst] == NULL )
						break; // delay rest of allocations
				} // for( ; nIdxMeasure > nOldDX; nIdxMeasure--, nIdxDst-- )
			} // if( !bForceDelayedRangeAllocation )
		} // else from if( nX < nOldDX )

	} // else from if( m_pSections == NULL )
	return nSectionCount;
}

bool CExtGridDataProviderMemory::_SectionEdject(
	ULONG nX,
	ULONG nDX,
	bool bDeallocateRest
	)
{
	ASSERT_VALID( this );
	ASSERT( m_nDX > 0 );
	ASSERT( nX < m_nDX );
	ASSERT( (nX+nDX) <= m_nDX );
	ASSERT( nDX > 0 );
	ASSERT( nDX < m_nDX );
	ASSERT( m_pSections != NULL );
ULONG nIdxSrc = nX;
	for( ULONG nIdx = 0; nIdx < nDX; nIdx++, nIdxSrc++ )
	{
		CExtGridCell ** pSection = m_pSections[ nIdxSrc ];
		if( pSection != NULL )
		{
			ASSERT( m_nDY > 0 );
			_SectionFree( pSection );
			m_pSections[ nIdxSrc ] = NULL;
		} // if( pSection != NULL )
	} // for( ULONG nIdx = 0; nIdx < nDX; nIdx++, nIdxSrc++ )
	ASSERT( nIdxSrc <= m_nDX );
	if( nIdxSrc < m_nDX )
	{
		ULONG nIdxDst = nX;
		nIdxSrc = nIdxDst + nDX;
		ULONG nMoveCount = m_nDX - nX - nDX;
		ASSERT( LONG(nMoveCount) > 0L );
		for( nIdx = 0; nIdx < nMoveCount; nIdx++, nIdxSrc++, nIdxDst++ )
		{
			m_pSections[ nIdxDst] = m_pSections[ nIdxSrc ];
			m_pSections[ nIdxSrc ] = NULL;
		} // for( nIdx = 0; nIdx < nMoveCount; nIdx++, nIdxSrc++, nIdxDst++ )
		ASSERT( nIdxSrc == m_nDX );
		ASSERT( nIdxDst == (nIdxSrc-nDX) );
	} // if( nIdxSrc < m_nDX )
	if( ! bDeallocateRest )
		return true;
CExtGridCell *** pNewSections = (CExtGridCell ***)
		m_pMallocD->Realloc(
			m_pSections,
			(m_nDX - nDX)*sizeof(LPVOID)
			);
	if( pNewSections == NULL )
		return false;
	m_pSections = pNewSections;
	m_nDX -= nDX;
	return true;
}

bool CExtGridDataProviderMemory::_SectionRealloc(
	ULONG nX,
	ULONG nDY
	)
{
	ASSERT_VALID( this );
	ASSERT( m_nDX > 0 );
	ASSERT( nX < m_nDX );
//	ASSERT( nDY >= 1 && nDY != m_nDY );
//	ASSERT( m_nDY >= 1 );
	ASSERT( m_pSections != NULL );
CExtGridCell ** pSection =
		_SectionGetAt( nX, false );
	if( pSection == NULL )
	{
		pSection = (CExtGridCell **)
			m_pMallocD->Alloc( sizeof(LPVOID)*nDY );
		if( pSection == NULL )
			return false; // reallocation failed
		ASSERT( m_pSections[nX] == NULL );
		m_pSections[nX] = pSection;
	} // if( pSection == NULL )
	else
	{
		if( nDY < m_nDY )
			_SectionRangeDestoy(
				m_pMallocF,
				pSection,
				m_nDY - nDY,
				nDY
				);
		CExtGridCell ** pSectionNew = (CExtGridCell **)
			m_pMallocD->Realloc( pSection, sizeof(LPVOID)*nDY );
		if( pSectionNew == NULL )
			return false; // reallocation failed
		m_pSections[nX] = pSectionNew;
	} // else from if( pSection == NULL )
	return true;
}

bool CExtGridDataProviderMemory::_SectionReallocAll(
	ULONG nDY
	)
{
	ASSERT_VALID( this );
	ASSERT( m_nDX > 0 );
	ASSERT( nDY >= 1); // ASSERT( nDY >= 1 && nDY != m_nDY );
	//ASSERT( m_nDY >= 1 );
	ASSERT( m_pSections != NULL );
	for( ULONG nX = m_nDX; nX > 0; nX-- )
	{
		if( !_SectionRealloc( nX-1, nDY ) )
		{
			for( ; nX <= m_nDX; nX++ )
			{
				if( _SectionGetAt(nX-1,false) == NULL )
					continue;
				// following line should allways succeed
				// with non-blocked allocator
				VERIFY( _SectionRealloc( nX-1, m_nDY ) );
			} // for( ; nX <= m_nDX; nX++ )
			return false;
		}
		ASSERT( _SectionGetAt(nX-1,false) != NULL );
	} // for( ULONG nX = m_nDX; nX > 0; nX-- )
	m_nDY = nDY;
	return true;
}

void CExtGridDataProviderMemory::_SectionRangeDestoy( // static
	IMalloc * pMalloc,
	CExtGridCell ** pSection,
	ULONG nY,
	ULONG nDY
	)
{
	ASSERT( pMalloc != NULL );
	ASSERT( pSection != NULL );
	ASSERT( nDY > 0 );
	for( ULONG nIdxMeasure = 0; nIdxMeasure < nDY; nIdxMeasure++, nY++ )
	{
		CExtGridCell * pCell = pSection[nY];
		if( pCell == NULL )
			continue;
		ASSERT_VALID( pCell );
		ASSERT_KINDOF( CExtGridCell, pCell );
		ASSERT( pCell->DataProviderGet() != NULL );
		//ASSERT_VALID( pCell->DataProviderGet() );
		CExtGridCell::InstanceDestroy( pCell, pMalloc );
		pSection[nY] = NULL;
	} // for( ULONG nIdxMeasure = 0; nIdxMeasure < nDY; nIdxMeasure++, nY++ )
}

bool CExtGridDataProviderMemory::_SectionRangeInjectAll(
	ULONG nY,
	ULONG nDY
	)
{
	ASSERT_VALID( this );
	ASSERT( m_nDX > 0 );
	ASSERT( nDY >= 1 );
	ASSERT( m_pSections != NULL );
	ASSERT( nY <= m_nDY );
ULONG nOldDY = m_nDY;
ULONG nNewDY = nOldDY + nDY;
	if( !_SectionReallocAll(nNewDY) )
	{
		ASSERT( m_nDY == nOldDY );
		return false;
	}
	ASSERT( m_nDY == nNewDY );
	_SectionRangeBottomToBottomAll( nY, nDY );
	return true;
}

void CExtGridDataProviderMemory::_SectionRangeBottomToBottom( // static
	CExtGridCell ** pSection,
	ULONG nY,
	ULONG nDY,
	ULONG nSectionSize
	)
{
	ASSERT( pSection != NULL );
	ASSERT( nDY > 0 );
	ASSERT( (nY+nDY) <= nSectionSize );
ULONG nIdxMeasure = 0;
ULONG nIdxDst = nSectionSize - 1;
ULONG nIdxSrc = nIdxDst - nDY;
	ASSERT( nIdxSrc < nIdxDst );
ULONG nCount = nSectionSize - nY - nDY;
	for(	;
			nIdxMeasure < nCount;
			nIdxMeasure++, nIdxSrc--, nIdxDst--
			)
	{
		ASSERT( pSection[nIdxDst] == NULL );
		pSection[nIdxDst] = pSection[nIdxSrc];
		pSection[nIdxSrc] = NULL;
	} // for ...
}

void CExtGridDataProviderMemory::_SectionRangeBottomToBottomAll(
	ULONG nY,
	ULONG nDY
	)
{
	ASSERT_VALID( this );
	ASSERT( m_nDX > 0 && m_nDY > 0 );
	ASSERT( nDY < m_nDY && nDY > 0 );
	ASSERT( m_pSections != NULL );
	for( ULONG nX = 0; nX < m_nDX; nX++ )
		_SectionRangeBottomToBottom(
			_SectionGetAt( nX, false ),
			nY,
			nDY,
			m_nDY
			);
}

void CExtGridDataProviderMemory::_SectionRangeEdject( // static
	IMalloc * pMalloc,
	CExtGridCell ** pSection,
	ULONG nY,
	ULONG nDY,
	ULONG nSectionSize
	)
{
	ASSERT( pMalloc != NULL );
	ASSERT( pSection != NULL );
	ASSERT( nSectionSize > 0 );
	ASSERT( nDY > 0 );
	ASSERT( nY <= (nSectionSize-1) );
	ASSERT( (nY+nDY) <= nSectionSize );
ULONG nIdxMeasure = 0;
	for( ; nIdxMeasure < nDY; nIdxMeasure++ )
	{
		ASSERT( (nY + nIdxMeasure) < nSectionSize );
		CExtGridCell * pCell =
			pSection[ nY + nIdxMeasure ];
		if( pCell != NULL )
		{
			ASSERT_VALID( pCell );
			ASSERT_KINDOF( CExtGridCell, pCell );
			ASSERT( pCell->DataProviderGet() != NULL );
			//ASSERT_VALID( pCell->DataProviderGet() );
			CExtGridCell::InstanceDestroy( pCell, pMalloc );
		} // if( pCell != NULL )
		pSection[ nY + nIdxMeasure ] = NULL;
	} // for( ; nIdxMeasure < nDY; nIdxMeasure++ )
	nIdxMeasure = 0;
ULONG nIdxSrc = nY + nDY;
ULONG nIdxDst = nY;
	ASSERT( nIdxDst < nIdxSrc );
ULONG nMoveCount = nSectionSize - nIdxSrc;
	ASSERT( LONG(nMoveCount) >= 0 );
	if( nMoveCount == 0 )
		return;
	for( ; nIdxMeasure < nMoveCount; nIdxMeasure++, nIdxSrc++, nIdxDst++ )
	{
		ASSERT( nIdxSrc < nSectionSize );
		ASSERT( nIdxDst < nSectionSize );
		ASSERT( pSection[ nIdxDst ] == NULL );
		pSection[ nIdxDst ] = pSection[ nIdxSrc ];
		pSection[ nIdxSrc ] = NULL;
	} // for( ; nIdxMeasure < nMoveCount; nIdxMeasure++, nIdxSrc++, nIdxDst++ )
}

void CExtGridDataProviderMemory::_SectionRangeEdjectAll(
	ULONG nY,
	ULONG nDY,
	bool bDeallocateRest
	)
{
	ASSERT_VALID( this );
	ASSERT( m_nDX > 0 && m_nDY > 0 );
	ASSERT( nDY > 0 );
	ASSERT( nY <= (m_nDY-1) );
	ASSERT( (nY+nDY) <= m_nDY );
	ASSERT( m_pSections != NULL );
ULONG nMoveCount = m_nDY - nDY - nY;
	ASSERT( LONG(nMoveCount) >= 0 );
//	if( nMoveCount == 0 )
	for( ULONG nX = 0; nX < m_nDX; nX++ )
	{
		CExtGridCell ** pSection =
			_SectionGetAt( nX, false );
		if( pSection == NULL )
			continue;
		_SectionRangeEdject(
			m_pMallocF,
			pSection,
			nY,
			nDY,
			m_nDY
			);
		if( ! bDeallocateRest )
			continue;
		CExtGridCell ** pNewSection = (CExtGridCell **)
			m_pMallocD->Realloc(
				pSection,
				(m_nDY - nDY)*sizeof(LPVOID)
				);
		ASSERT( pNewSection != NULL );
		m_pSections[ nX ]= pNewSection;
	} // for( ULONG nX = 0; nX < m_nDX; nX++ )
	if( bDeallocateRest )
	{
		m_nDY -= nDY;
		ASSERT( m_nDY > 0 );
	} // if( bDeallocateRest )
}

IMalloc * CExtGridDataProviderMemory::_DVM_GetCellAllocator()
{
	ASSERT_VALID( this );
	return m_pMallocF;
}

#ifdef _DEBUG
void CExtGridDataProviderMemory::AssertValid() const
{
	CExtGridDataProviderBL::AssertValid();
	ASSERT( m_pMallocV != NULL );
	ASSERT( m_pMallocF != NULL );
	ASSERT( m_pMallocD != NULL );
	ASSERT( m_nStringAllocationAlign >= 1 );
}
void CExtGridDataProviderMemory::Dump( CDumpContext & dc ) const
{
	CExtGridDataProviderBL::Dump( dc );
}
#endif // _DEBUG

__EXT_MFC_SAFE_LPTSTR CExtGridDataProviderMemory::StringAlloc(
	INT nCharCountIncZT
	)
{
	ASSERT_VALID( this );
	if( nCharCountIncZT < 1 )
	{
		ASSERT( FALSE );
		return NULL;
	} // if( nCharCountIncZT < 1 )
ULONG nSize = (ULONG)nCharCountIncZT;
	if( m_nStringAllocationAlign > 0 )
	{
		ULONG nSizeRest = nSize % m_nStringAllocationAlign;
		nSize /= m_nStringAllocationAlign;
		nSize *= m_nStringAllocationAlign;
		if( nSizeRest > 0 )
			nSize += m_nStringAllocationAlign;
	} // if( m_nStringAllocationAlign > 0 )
	ASSERT( nSize >= 1 );
LPVOID pMemory = m_pMallocV->Alloc( nSize*sizeof(TCHAR) );
	if( pMemory == NULL )
	{
		ASSERT( FALSE );
		return __EXT_MFC_SAFE_LPTSTR( LPTSTR(NULL) );
	} // if( pMemory == NULL )
	return __EXT_MFC_SAFE_LPTSTR( LPTSTR(pMemory) );
}

void CExtGridDataProviderMemory::StringFree(
	__EXT_MFC_SAFE_LPTSTR strToFree
	)
{
	ASSERT_VALID( this );
LPVOID pMemory = LPVOID(LPTSTR(strToFree));
	if( pMemory == NULL )
	{
		ASSERT( FALSE );
		return;
	} // if( pMemory == NULL )
	ASSERT( m_pMallocV->DidAlloc(pMemory) );
	ASSERT( m_pMallocV->GetSize(pMemory) >= 1 );
	m_pMallocV->Free( pMemory );
}

bool CExtGridDataProviderMemory::ColumnInsert(
	ULONG nColNo,
	ULONG nInsertCount // = 1
	)
{
	ASSERT_VALID( this );
	ASSERT( nInsertCount >= 0 );
	if( nInsertCount == 0 )
		return true;
	if( nColNo > m_nDX )
	{
		ASSERT( FALSE );
		return false;
	} // if( nColNo > m_nDX )
ULONG nInserted = _SectionInsertAt( nColNo, nInsertCount, false );
	ASSERT( nInserted == nInsertCount );
	if ( nInserted != nInsertCount )
		return false;
	m_gdsoRows.UpdateIndices(
		LONG(nColNo),
		LONG(nInsertCount),
		true
		);
	return true;
}

bool CExtGridDataProviderMemory::RowInsert(
	ULONG nRowNo,
	ULONG nInsertCount // = 1
	)
{
	ASSERT_VALID( this );
	ASSERT( nInsertCount >= 0 );
	if( nInsertCount == 0 )
		return true;
	if( m_nDX == 0 )
	{
		m_nDY += nInsertCount;
		return true;
	} // if( m_nDX == 0 )
	if( nRowNo > m_nDY )
	{
		ASSERT( FALSE );
		return false;
	} // if( nRowNo > m_nDY )
bool bRetVal = _SectionRangeInjectAll( nRowNo, nInsertCount );
	ASSERT( bRetVal );
	if( bRetVal )
	{
		m_gdsoColumns.UpdateIndices(
			LONG(nRowNo),
			LONG(nInsertCount),
			true
			);
	} // if( bRetVal )
	return bRetVal;
}

ULONG CExtGridDataProviderMemory::ColumnCountGet() const
{
	ASSERT_VALID( this );
	return m_nDX;
}

ULONG CExtGridDataProviderMemory::RowCountGet() const
{
	ASSERT_VALID( this );
	return m_nDY;
}

bool CExtGridDataProviderMemory::ColumnRemove(
	ULONG nColNo,
	ULONG nRemoveCount // = 1
	)
{
	ASSERT_VALID( this );
	if( nColNo >= m_nDX )
	{
		ASSERT( FALSE );
		return false;
	} // if( nColNo >= m_nDX )
	if( nRemoveCount == 0 )
		return true;
	if( (nColNo + nRemoveCount) > m_nDX )
	{
		ASSERT( FALSE );
		return false;
	} // if( (nColNo + nRemoveCount) > m_nDX )
	if( nColNo == 0 && nRemoveCount == m_nDX )
	{
		_SectionFreeAll( true );
		m_gdsoRows.Empty();
		return true;
	} // if( nColNo == 0 && nRemoveCount == m_nDX )
bool bRetVal =
		_SectionEdject(
			nColNo,
			nRemoveCount,
			true
			);
	ASSERT( bRetVal );
	if( bRetVal )
	{
		m_gdsoRows.UpdateIndices(
			LONG(nColNo),
			LONG(nRemoveCount),
			false
			);
	} // if( bRetVal )
	return bRetVal;
}

bool CExtGridDataProviderMemory::RowRemove(
	ULONG nRowNo,
	ULONG nRemoveCount // = 1
	)
{
	ASSERT_VALID( this );
	if( nRowNo >= m_nDY )
	{
		ASSERT( FALSE );
		return false;
	} // if( nRowNo >= m_nDY )
	if( nRemoveCount == 0 )
		return true;
	if( (nRowNo + nRemoveCount) > m_nDY )
	{
		ASSERT( FALSE );
		return false;
	} // if( (nRowNo + nRemoveCount) > m_nDY )
	if( nRowNo == 0 && nRemoveCount == m_nDY )
	{
		_SectionFreeAll( (m_nDX == 0) ? true : false );
		m_nDY = 0;
		m_gdsoColumns.Empty();
		return true;
	} // if( nRowNo == 0 && nRemoveCount == m_nDY )
	if( m_nDX == 0 )
	{
		m_nDY -= nRemoveCount;
		m_gdsoColumns.Empty();
		return true;
	} // if( m_nDX == 0 )
	_SectionRangeEdjectAll(
		nRowNo,
		nRemoveCount,
		true
		);
	m_gdsoColumns.UpdateIndices(
		LONG(nRowNo),
		LONG(nRemoveCount),
		true
		);
	return true;
}

void CExtGridDataProviderMemory::MinimizeMemoryUsage()
{
	ASSERT_VALID( this );
	m_pMallocV->HeapMinimize();
	m_pMallocF->HeapMinimize();
	m_pMallocD->HeapMinimize();
}

CExtGridCell * CExtGridDataProviderMemory::CellGet(
	ULONG nColNo,
	ULONG nRowNo,
	CRuntimeClass * pInitRTC, // = NULL
	bool bAutoFindValue, // = true // auto find row/column default value (only when pInitRTC=NULL)
	bool bUseColumnDefaultValue // = true // false - use row default value (only when bAutoFindValue=true)
	)
{
	ASSERT_VALID( this );
	if( nColNo >= m_nDX || nRowNo >= m_nDY )
	{
		// invalid position
		ASSERT( FALSE );
		return NULL;
	} // if( nColNo >= m_nDX || nRowNo >= m_nDY )
bool bAllowDelayedAllocation =
		( pInitRTC != NULL || bAutoFindValue )
			? true : false;
CExtGridCell ** pSection =
		_SectionGetAt( nColNo, bAllowDelayedAllocation );
	if( pSection == NULL )
	{
		// may be, delayed allocation failed
		ASSERT( !bAllowDelayedAllocation );
		return NULL;
	} // if( pSection == NULL )
CExtGridCell * pFoundCell = pSection[nRowNo];
	if( pFoundCell != NULL )
	{
		ASSERT_VALID( pFoundCell );
		ASSERT( pFoundCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
		ASSERT( pFoundCell->DataProviderGet() == this );
		//ASSERT_VALID( pFoundCell->DataProviderGet() );
		return pFoundCell;
	} // if( pFoundCell != NULL )
CExtGridCell * pInitCell = NULL;
	if( pInitRTC != NULL )
	{
		ASSERT( pInitRTC->IsDerivedFrom(RUNTIME_CLASS(CExtGridCell)) );
		pInitCell = 
			CExtGridCell::InstanceCreate(
				pInitRTC,
				m_pMallocF,
				false
				);
		if( pInitCell == NULL )
		{
			// if allocation/construction failed
			ASSERT( FALSE );
			return NULL;
		} // if( pInitCell == NULL )
		ASSERT_VALID( pInitCell );
		ASSERT( pInitCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
		pInitCell->DataProviderSet( this );
	} // if( pInitRTC != NULL )
	else if( bAutoFindValue )
	{
		CExtGridCell * pCellDV = ( bUseColumnDefaultValue )
			? ColumnDefaultValueGet( nColNo )
			: RowDefaultValueGet( nRowNo )
			;
		if( pCellDV == NULL )
		{
			// if no default value found
			return NULL;
		}
		ASSERT_VALID( pCellDV );
		ASSERT( pCellDV->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
		ASSERT( pCellDV->DataProviderGet() == this );
		//ASSERT_VALID( pCellDV->DataProviderGet() );
		pInitCell = pCellDV->Clone( m_pMallocF );
		if( pInitCell == NULL )
		{
			// if allocation/construction failed
			ASSERT( FALSE );
			return NULL;
		} // if( pInitCell == NULL )
		ASSERT_VALID( pInitCell );
		ASSERT( pInitCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
		ASSERT( pInitCell->DataProviderGet() == this );
		//ASSERT_VALID( pInitCell->DataProviderGet() );
	} // else if( bAutoFindValue )
	ASSERT( pSection[nRowNo] == NULL );
	pSection[nRowNo] = pInitCell;
	return pInitCell;
}

bool CExtGridDataProviderMemory::CellRangeSet(
	ULONG nColNo,
	ULONG nRowNo,
	ULONG nColCount, // = 1L
	ULONG nRowCount, // = 1L
	const CExtGridCell * pCellNewValue, // = NULL // if NULL - empty existing cell values
	bool bReplace, // false - assign to existing cell instances or column/row type created cells, true - create new cloned copies of pCellNewValue
	CRuntimeClass * pInitRTC, // = NULL
	bool bAutoFindValue, // = true // auto find row/column default value (only when pInitRTC=NULL)
	bool bUseColumnDefaultValue, // = true // false - use row default value (only when bAutoFindValue=true)
	ULONG * p_nUpdatedCellCount // = NULL // returns count of really updated cells (zero also may be treated as success)
	)
{
	ASSERT_VALID( this );
#ifdef _DEBUG
	if( pCellNewValue != NULL )
	{
		ASSERT_VALID( pCellNewValue );
		ASSERT( pCellNewValue->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
	} // if( pCellNewValue != NULL )
#endif // _DEBUG
	if( p_nUpdatedCellCount != NULL )
		*p_nUpdatedCellCount = 0L;
	if( nColCount == 0 || nRowCount == 0 )
		return false; // empty update range
ULONG nColAfterLast = nColNo + nColCount;
ULONG nRowAfterLast = nRowNo + nRowCount;
ULONG nRowNoStart = nRowNo;
	if( nColAfterLast > m_nDX || nRowAfterLast > m_nDY )
	{
		// invalid position/rane
		ASSERT( FALSE );
		return false;
	} // if( nColAfterLast > m_nDX || nRowAfterLast > m_nDY )
bool bAllowDelayedAllocation =
		( pCellNewValue != NULL )
			? true : false;
	try
	{
		for( ; nColNo < nColAfterLast; nColNo++ )
		{
			CExtGridCell ** pSection =
				_SectionGetAt( nColNo, bAllowDelayedAllocation );
			if( pSection == NULL )
			{
				if( !bAllowDelayedAllocation )
					continue;
				// delayed allocation failed
				ASSERT( FALSE );
				return false;
			} // if( pSection == NULL )
			for( nRowNo = nRowNoStart; nRowNo < nRowAfterLast; nRowNo++ )
			{
				CExtGridCell * pCell = pSection[nRowNo];
				if( pCell != NULL )
				{
					ASSERT_VALID( pCell );
					ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
					ASSERT( pCell->DataProviderGet() == this );
					//ASSERT_VALID( pCell->DataProviderGet() );
					if( pCellNewValue == NULL )
					{
						CExtGridCell::InstanceDestroy(
							pCell,
							m_pMallocF,
							false
							);
						pSection[nRowNo] = NULL;
						if( p_nUpdatedCellCount != NULL )
							(*p_nUpdatedCellCount) ++;
						continue;
					} // if( pCellNewValue == NULL )
					if( ! bReplace )
					{
						pCell->Assign( *pCellNewValue );
						ASSERT_VALID( pCell );
						if( p_nUpdatedCellCount != NULL )
							(*p_nUpdatedCellCount) ++;
						continue;
					} // if( ! bReplace )
					pSection[nRowNo] = NULL;
					CExtGridCell::InstanceDestroy(
						pCell,
						m_pMallocF,
						false
						);
				} // if( pCell != NULL )
				else
				{
					if( pCellNewValue == NULL )
						continue;
				} // else from if( pCell != NULL )
				ASSERT( pCellNewValue != NULL );
				// create new cell instace
				pCell = NULL;
				if( ! bReplace )
				{
					if( pInitRTC != NULL )
					{
						ASSERT( pInitRTC->IsDerivedFrom(RUNTIME_CLASS(CExtGridCell)) );
						pCell = 
							CExtGridCell::InstanceCreate(
								pInitRTC,
								m_pMallocF,
								false
								);
						if( pCell == NULL )
						{
							// if allocation/construction failed
							ASSERT( FALSE );
							return false;
						} // if( pInitCell == NULL )
						ASSERT_VALID( pCell );
						ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
						pCell->DataProviderSet( this );
						pCell->Assign( *pCellNewValue );
						ASSERT_VALID( pCell );
					} // if( pInitRTC != NULL )
					else if( bAutoFindValue )
					{
						CExtGridCell * pCellDV = ( bUseColumnDefaultValue )
							? ColumnDefaultValueGet( nColNo )
							: RowDefaultValueGet( nRowNo )
							;
						if( pCellDV != NULL )
						{ // if default value found
							ASSERT_VALID( pCellDV );
							ASSERT( pCellDV->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
							ASSERT( pCellDV->DataProviderGet() == this );
							//ASSERT_VALID( pCellDV->DataProviderGet() );
							pCell = pCellDV->Clone( m_pMallocF );
							if( pCell == NULL )
							{
								// if allocation/construction failed
								ASSERT( FALSE );
								return false;
							} // if( pCell == NULL )
							ASSERT_VALID( pCell );
							ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
							pCell->Assign( *pCellNewValue );
							ASSERT_VALID( pCell );
							pCell->DataProviderSet( this );
						} // if default value found
					} // else if( bAutoFindValue )
				} // if( ! bReplace )
				if( pCell == NULL )
				{
					pCell = pCellNewValue->Clone( m_pMallocF );
					if( pCell == NULL )
					{
						// if allocation/construction failed
						ASSERT( FALSE );
						return false;
					} // if( pCell == NULL )
					ASSERT_VALID( pCell );
					pCell->DataProviderSet( this );
				} // if( pCell == NULL )
				ASSERT_VALID( pCell );
				ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
				ASSERT( pCell->DataProviderGet() == this );
				//ASSERT_VALID( pCell->DataProviderGet() );
				ASSERT( pSection[nRowNo] == NULL );
				pSection[nRowNo] = pCell;
				if( p_nUpdatedCellCount != NULL )
					(*p_nUpdatedCellCount) ++;
			} // for( nRowNo = nRowNoStart; nRowNo < nRowAfterLast; nRowNo++ )
		} // for( ; nColNo < nColAfterLast; nColNo++ )
		return true;
	} // try
	catch( CException * pException )
	{
		ASSERT( FALSE );
		pException->Delete();
	} // catch( CException * pException )
	catch( ... )
	{
		ASSERT( FALSE );
	} // catch( ... )
	return false;
}

bool CExtGridDataProviderMemory::_SortOrderCheck(
	const CExtGridDataSortOrder & _gdso,
	bool bColumns
	)
{
	ASSERT_VALID( this );
LONG nCount = _gdso.m_arrItems.GetSize();
	if( nCount == 0L )
		return true;
LONG nSize = bColumns ? RowCountGet() : ColumnCountGet();
	ASSERT( nSize >= 0L );
	for( LONG i = 0L; i < nCount; i++ )
	{
		const CExtGridDataSortOrder::ITEM_INFO & _itemSrc =
			_gdso.m_arrItems[ i ];
		if(		_itemSrc.m_nRowColNo < 0L
			||	_itemSrc.m_nRowColNo >= nSize
			)
			return false;
	}
	return true;
}

int CExtGridDataProviderMemory::_SortCmpCells(
	CExtGridCell * pCell_1,
	CExtGridCell * pCell_2
	)
{
	ASSERT_VALID( this );
	if( pCell_1 == NULL )
	{
		if( pCell_2 == NULL )
			return 0;
		ASSERT_VALID( pCell_2 );
		return -1;
	} // if( pCell_1 == NULL )
	else
	{
		if( pCell_2 == NULL )
		{
			ASSERT_VALID( pCell_1 );
			return 1;
		} // if( pCell_2 == NULL )
	} // else from if( pCell_1 == NULL )
	ASSERT_VALID( pCell_1 );
	ASSERT_VALID( pCell_2 );
int nRetVal = pCell_1->Compare( *pCell_2 );
	return nRetVal;
}

int CExtGridDataProviderMemory::_SortCmpSeries(
	LONG nRowColNo1,
	LONG nRowColNo2,
	bool bColumns,
	const CExtGridDataSortOrder & _gdso
	)
{
	ASSERT_VALID( this );
	ASSERT( ! _gdso.IsEmpty() );
	ASSERT( nRowColNo1 >= 0L );
	ASSERT( nRowColNo2 >= 0L );
	if( nRowColNo1 == nRowColNo2 )
		return 0;
LONG nCount = _gdso.m_arrItems.GetSize();
	ASSERT( nCount > 0L );
int nPointerRetVal = 0;
	for( LONG i = 0L; i < nCount; i++ )
	{
		const CExtGridDataSortOrder::ITEM_INFO & _itemSrc =
			_gdso.m_arrItems[ i ];
		int nRetVal = 0;
		if( bColumns )
		{
			CExtGridCell * pCell_1 =
				CellGet(
					nRowColNo1,
					_itemSrc.m_nRowColNo
					);
			CExtGridCell * pCell_2 =
				CellGet(
					nRowColNo2,
					_itemSrc.m_nRowColNo
					);
			nRetVal =
				_itemSrc.m_bAscending
					? _SortCmpCells( pCell_1, pCell_2 )
					: _SortCmpCells( pCell_2, pCell_1 );
			if( i == 0L && nRetVal == 0 )
			{
				// the following statments will will provide a better
				// sort order behavior after double resortings
				if( LONG(pCell_1) < LONG(pCell_2) )
					nPointerRetVal = -1;
				else if( LONG(pCell_1) > LONG(pCell_2) )
					nPointerRetVal = 1;
			} // if( i == 0L && nRetVal == 0 )
		} // if( bColumns )
		else
		{
			CExtGridCell * pCell_1 =
				CellGet(
					_itemSrc.m_nRowColNo,
					nRowColNo1
					);
			CExtGridCell * pCell_2 =
				CellGet(
					_itemSrc.m_nRowColNo,
					nRowColNo2
					);
			nRetVal =
				_itemSrc.m_bAscending
					? _SortCmpCells( pCell_1, pCell_2 )
					: _SortCmpCells( pCell_2, pCell_1 );
			if( i == 0L && nRetVal == 0 )
			{
				// the following statments will will provide a better
				// sort order behavior after double resortings
				if( LONG(pCell_1) < LONG(pCell_2) )
					nPointerRetVal = -1;
				else if( LONG(pCell_1) > LONG(pCell_2) )
					nPointerRetVal = 1;
			} // if( i == 0L && nRetVal == 0 )
		} // else from if( bColumns )
		if( nRetVal != 0 )
			return nRetVal;
	} // for( LONG i = 0L; i < nCount; i++ )
	return nPointerRetVal;
}

bool CExtGridDataProviderMemory::_SortSwapSeries(
	LONG nRowColNo1,
	LONG nRowColNo2,
	LONG & nSwapCounter,
	CExtGridDataProvider::IDataProviderEvents * pDPE,
	bool bColumns
	)
{
	ASSERT_VALID( this );
	ASSERT( nRowColNo1 >= 0L );
	ASSERT( nRowColNo2 >= 0L );
	ASSERT( nRowColNo1 != nRowColNo2 );
	ASSERT( nSwapCounter >= 0L );
	if( m_pSections != NULL )
	{
		if( pDPE != NULL )
			pDPE->OnDataProviderSwapSeries(
				bColumns,
				nRowColNo1,
				nRowColNo2,
				nSwapCounter
				);
		if( bColumns )
		{
			ASSERT( ULONG(nRowColNo1) < m_nDX );
			ASSERT( ULONG(nRowColNo2) < m_nDX );
			CExtGridCell ** pSectionTmp = m_pSections[ nRowColNo1 ];
			m_pSections[ nRowColNo1 ] = m_pSections[ nRowColNo2 ];
			m_pSections[ nRowColNo2 ] = pSectionTmp;
			if( ! m_gdsoRows.IsEmpty() )
			{
				LONG nCount = m_gdsoRows.m_arrItems.GetSize();
				ASSERT( nCount > 0L );
				for( LONG i = 0; i < nCount; i++ )
				{
					const CExtGridDataSortOrder::ITEM_INFO & _soii =
						m_gdsoRows.m_arrItems[i];
					if( _soii.m_nRowColNo == nRowColNo1 )
					{
						CExtGridDataSortOrder::ITEM_INFO _soiiUpdate( _soii );
						_soiiUpdate.m_nRowColNo = nRowColNo2;
						m_gdsoRows.m_arrItems.SetAt( i, _soiiUpdate );
					}
					else if( _soii.m_nRowColNo == nRowColNo2 )
					{
						CExtGridDataSortOrder::ITEM_INFO _soiiUpdate( _soii );
						_soiiUpdate.m_nRowColNo = nRowColNo1;
						m_gdsoRows.m_arrItems.SetAt( i, _soiiUpdate );
					}
				}
			} // if( ! m_gdsoRows.IsEmpty() )
		} // if( bColumns )
		else
		{
			ASSERT( ULONG(nRowColNo1) < m_nDY );
			ASSERT( ULONG(nRowColNo2) < m_nDY );
			if( nSwapCounter == 0L )
			{ // ensure all the sections present
				for( ULONG i = 0L; i < m_nDX; i++ )
				{
					CExtGridCell ** pSection =
						_SectionGetAt( i, true );
					if( pSection == NULL )
						return false;
				}
			} // ensure all the sections present
			for( ULONG i = 0L; i < m_nDX; i++ )
			{
				CExtGridCell ** pSection =
					_SectionGetAt( i, false );
				ASSERT( pSection != NULL );
				CExtGridCell * pCellTmp = pSection[ nRowColNo1 ];
				pSection[ nRowColNo1 ] = pSection[ nRowColNo2 ];
				pSection[ nRowColNo2 ] = pCellTmp;
			}
			if( ! m_gdsoColumns.IsEmpty() )
			{
				LONG nCount = m_gdsoColumns.m_arrItems.GetSize();
				ASSERT( nCount > 0L );
				for( LONG i = 0; i < nCount; i++ )
				{
					const CExtGridDataSortOrder::ITEM_INFO & _soii =
						m_gdsoColumns.m_arrItems[i];
					if( _soii.m_nRowColNo == nRowColNo1 )
					{
						CExtGridDataSortOrder::ITEM_INFO _soiiUpdate( _soii );
						_soiiUpdate.m_nRowColNo = nRowColNo2;
						m_gdsoColumns.m_arrItems.SetAt( i, _soiiUpdate );
					}
					else if( _soii.m_nRowColNo == nRowColNo2 )
					{
						CExtGridDataSortOrder::ITEM_INFO _soiiUpdate( _soii );
						_soiiUpdate.m_nRowColNo = nRowColNo1;
						m_gdsoColumns.m_arrItems.SetAt( i, _soiiUpdate );
					}
				}
			} // if( ! m_gdsoColumns.IsEmpty() )
		} // else from if( bColumns )
	} // if( m_pSections != NULL )
	nSwapCounter ++;
	return true;
}

bool CExtGridDataProviderMemory::_SortStep(
	bool bColumns,
	CExtGridDataProvider::IDataProviderEvents * pDPE,
	CExtGridDataSortOrder & _gdso,
	LONG & nSwapCounter,
	LONG nLow,
	LONG nHigh
	)
{
	ASSERT_VALID( this );
	ASSERT( ! _gdso.IsEmpty() );
	ASSERT( nLow <= nHigh );
	if( nLow == nHigh )
		return true;
	if( nLow > nHigh )
	{
		ASSERT( FALSE );
		return false;
	} // if( nLow > nHigh )
//	for( LONG i = nLow; i < nHigh; i++ )
//	{
//		for( LONG j = nHigh; j > i; j-- )
//		{
//			if( _SortCmpSeries(
//					j - 1,
//					j,
//					bColumns,
//					_gdso
//					) <= 0
//				)
//				continue;
//			if(	!_SortSwapSeries(
//					j - 1,
//					j,
//					nSwapCounter,
//					pDPE,
//					bColumns
//					)
//				)
//			{
//				ASSERT( FALSE );
//				return false;
//			}
//		} // for( LONG j = nHigh; j > i; j-- )
//	} // for( LONG i = nLow; i < nHigh; i++ )
LONG nViewMin = nLow, nViewMax = nHigh, nViewMiddle = ( nLow + nHigh ) / 2L;
	for( ; nViewMin <= nViewMax; )
	{
		for( ;	( nViewMin < nHigh )
			&&	( _SortCmpSeries( nViewMin, nViewMiddle, bColumns, _gdso ) < 0 );
			++ nViewMin
			);
		for( ;	( nViewMax > nLow )
			&&	( _SortCmpSeries( nViewMax, nViewMiddle, bColumns, _gdso ) > 0 );
			-- nViewMax
			);
		if( nViewMin <= nViewMax )
		{
			if(		nViewMin != nViewMax
				&&	_SortCmpSeries(
						nViewMin,
						nViewMax,
						bColumns,
						_gdso
						) != 0
				)
			{
				if( nViewMiddle == nViewMin )
					nViewMiddle = nViewMax;
				else if( nViewMiddle == nViewMax )
					nViewMiddle = nViewMin;
				if( ! _SortSwapSeries(
						nViewMin,
						nViewMax,
						nSwapCounter,
						pDPE,
						bColumns
						)
					)
				{
					ASSERT( FALSE );
					return false;
				}
			} // if( nViewMin != nViewMax && ...
			++ nViewMin;
			-- nViewMax;
		}
	}
	if( nLow < nViewMax )
	{
		if( ! _SortStep(
				bColumns,
				pDPE,
				_gdso,
				nSwapCounter,
				nLow,
				nViewMax
				)
			)
		{
			ASSERT( FALSE );
			return false;
		}
	} // if( nLow < nViewMax )
	if( nViewMin < nHigh )
	{
		if( ! _SortStep(
				bColumns,
				pDPE,
				_gdso,
				nSwapCounter,
				nViewMin,
				nHigh
				)
			)
		{
			ASSERT( FALSE );
			return false;
		}
	} // if( nViewMin < nHigh )
	return true;
}

bool CExtGridDataProviderMemory::SortOrderUpdate(
	bool bColumns, // true = sort order for columns, false - for rows
	CExtGridDataProvider::IDataProviderEvents * pDPE // = NULL
	)
{
	ASSERT_VALID( this );
CExtGridDataSortOrder & _gdso =
		bColumns ? m_gdsoColumns : m_gdsoRows;
	if( _gdso.IsEmpty() )
		return true;
	if( ! _SortOrderCheck( _gdso, bColumns ) )
		return false;
ULONG nColCountReserved, nRowCountReserved;
	CacheReservedCountsGet(
		&nColCountReserved,
		&nRowCountReserved
		);
LONG nReserved = bColumns ? LONG(nColCountReserved) : LONG(nRowCountReserved);
LONG nSize = bColumns ? ColumnCountGet() : RowCountGet();
	ASSERT( nSize >= 0L );
	ASSERT( nSize >= 0L );
	if( nSize <= 1L )
		return true;
	ASSERT( nSize > 1L );
	if( pDPE != NULL )
		pDPE->OnDataProviderSortEnter( bColumns );
LONG nSwapCounter = 0L;
bool bRetVal =
		_SortStep(
			bColumns,
			pDPE,
			_gdso,
			nSwapCounter,
			nReserved,
			nSize - 1L
			);
	if( pDPE != NULL )
		pDPE->OnDataProviderSortLeave( bColumns );
	return bRetVal;

}

bool CExtGridDataProviderMemory::SortOrderSet(
	const CExtGridDataSortOrder & _gdso,
	bool bColumns, // true = sort order for columns, false - for rows
	CExtGridDataProvider::IDataProviderEvents * pDPE // = NULL
	)
{
	ASSERT_VALID( this );
	if( ! _gdso.ItemsUnique() )
	{
		// sort order must have unique row/column indices
		ASSERT( FALSE );
		return false;
	}
	if( ! _SortOrderCheck( _gdso, bColumns ) )
		return false;
	if( bColumns )
	{
		if( _gdso == m_gdsoColumns )
			return true;
		m_gdsoColumns = _gdso;
	} // if( bColumns )
	else
	{
		if( _gdso == m_gdsoRows )
			return true;
		m_gdsoRows = _gdso;
	} // else from if( bColumns )
	VERIFY( SortOrderUpdate( bColumns, pDPE ) );
	return true;
}

bool CExtGridDataProviderMemory::SortOrderGet(
	CExtGridDataSortOrder & _gdso,
	bool bColumns // true = sort order for columns, false - for rows
	) const
{
	ASSERT_VALID( this );
	if( bColumns )
		_gdso = m_gdsoColumns;
	else
		_gdso = m_gdsoRows;
	ASSERT( _gdso.ItemsUnique() );
	return true;
}

bool CExtGridDataProviderMemory::SwapDroppedSeries(
	bool bColumns, // true = swap columns, false - rows
	ULONG nRowColNoSrc,
	ULONG nRowColNoDropBefore,
	IDataProviderEvents * pDPE // = NULL
	)
{
	ASSERT_VALID( this );
	if(		nRowColNoSrc < 0
		||	nRowColNoDropBefore < 0
		)
	{
		ASSERT( FALSE );
		return false;
	}
//ULONG nOffset = 0L;
//	CacheReservedCountsGet(
//		bColumns ? (&nOffset) : NULL,
//		bColumns ? NULL : (&nOffset)
//		);
//	nRowColNoSrc += nOffset;
//	nRowColNoDropBefore += nOffset;
	if( bColumns )
	{
		if(		nRowColNoSrc >= m_nDX
			||	nRowColNoDropBefore > m_nDX
			)
		{
			ASSERT( FALSE );
			return false;
		}
		if( m_pSections == NULL )
			return TRUE;
		if(		nRowColNoSrc == nRowColNoDropBefore
			||	(nRowColNoSrc+1L) == nRowColNoDropBefore
			)
			return true;
		CExtGridCell ** pSectionSrc = m_pSections[ nRowColNoSrc ];
		if( nRowColNoSrc < nRowColNoDropBefore )
		{
			for( ULONG nIdx = nRowColNoSrc; nIdx < nRowColNoDropBefore; nIdx++ )
				m_pSections[ nIdx ] = m_pSections[ nIdx + 1L ];
			m_pSections[ nRowColNoDropBefore - 1L ] = pSectionSrc;
		} // if( nRowColNoSrc < nRowColNoDropBefore )
		else
		{
			ASSERT( nRowColNoSrc > nRowColNoDropBefore );
			for( ULONG nIdx = nRowColNoSrc; nIdx > nRowColNoDropBefore; nIdx-- )
				m_pSections[ nIdx ] = m_pSections[ nIdx - 1L ];
			m_pSections[ nRowColNoDropBefore ] = pSectionSrc;
		} // else from if( nRowColNoSrc < nRowColNoDropBefore )
		if( ! m_gdsoRows.IsEmpty() )
			m_gdsoRows.SwapDroppedSeries(
				LONG(nRowColNoSrc), // - LONG(nOffset)
				LONG(nRowColNoDropBefore) // - LONG(nOffset)
				);
	} // if( bColumns )
	else
	{
		if(		nRowColNoSrc >= m_nDY
			||	nRowColNoDropBefore > m_nDY
			)
		{
			ASSERT( FALSE );
			return false;
		}
		if( m_pSections == NULL )
			return TRUE;
		if(		nRowColNoSrc == nRowColNoDropBefore
			||	(nRowColNoSrc+1L) == nRowColNoDropBefore
			)
			return true;
		for( ULONG i = 0L; i < m_nDX; i++ )
		{
			CExtGridCell ** pSection =
				_SectionGetAt( i, false );
			if( pSection == NULL )
				continue;
			CExtGridCell * pCellSrc = pSection[ nRowColNoSrc ];
			if( nRowColNoSrc < nRowColNoDropBefore )
			{
				for( ULONG nIdx = nRowColNoSrc; nIdx < nRowColNoDropBefore; nIdx++ )
					pSection[ nIdx ] = pSection[ nIdx + 1L ];
				pSection[ nRowColNoDropBefore - 1L ] = pCellSrc;
			} // if( nRowColNoSrc < nRowColNoDropBefore )
			else
			{
				ASSERT( nRowColNoSrc > nRowColNoDropBefore );
				for( ULONG nIdx = nRowColNoSrc; nIdx > nRowColNoDropBefore; nIdx-- )
					pSection[ nIdx ] = pSection[ nIdx - 1L ];
				pSection[ nRowColNoDropBefore ] = pCellSrc;
			} // else from if( nRowColNoSrc < nRowColNoDropBefore )
		}
		if( ! m_gdsoColumns.IsEmpty() )
			m_gdsoColumns.SwapDroppedSeries(
				LONG(nRowColNoSrc), // - LONG(nOffset)
				LONG(nRowColNoDropBefore) // - LONG(nOffset)
				);
	} // else from if( bColumns )
	if( pDPE != NULL )
		pDPE->OnDataProviderSwapDroppedSeries(
			bColumns,
			LONG(nRowColNoSrc), // - LONG(nOffset)
			LONG(nRowColNoDropBefore) // - LONG(nOffset)
			);
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CExtGridWnd window

IMPLEMENT_DYNCREATE( CExtGridWnd, CExtGridBaseWnd );

CExtGridWnd::CExtGridWnd()
	: m_pDataProvider( NULL )
	, m_dwBseStyle( __EGWS_BSE_DEFAULT )
	, m_dwBseStyleEx( __EGWS_BSE_EX_DEFAULT )
#ifdef _DEBUG
	, m_bDebugModifyingColumns( false )
	, m_bDebugModifyingRows( false )
#endif // _DEBUG
	, m_nHelperLastEditedColNo( -1L )
	, m_nHelperLastEditedRowNo( -1L )
{
	m_hCursorOuterDragOK = 
		::LoadCursor( NULL, IDC_ARROW );
	ASSERT( m_hCursorOuterDragOK != NULL );
#if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
	CExtLocalResourceHelper _LRH;
	CWinApp * pApp = ::AfxGetApp();
	ASSERT_VALID( pApp );
	m_hCursorOuterDragCancel =
		pApp->LoadCursor( MAKEINTRESOURCE( IDC_EXT_BIG_X_CROSS ) );
	ASSERT( m_hCursorOuterDragCancel != NULL );
#else
	m_hCursorOuterDragCancel =
		::LoadCursor( NULL, IDC_NO );
	ASSERT( m_hCursorOuterDragCancel != NULL );
#endif // #if (defined _AFXDLL && !defined __STATPROFUIS_WITH_DLLMFC__)
}

CExtGridWnd::~CExtGridWnd()
{
	if( m_pDataProvider != NULL )
		delete m_pDataProvider;
	if( m_hCursorOuterDragOK != NULL )
		::DestroyCursor( m_hCursorOuterDragOK );
	if( m_hCursorOuterDragCancel != NULL )
		::DestroyCursor( m_hCursorOuterDragCancel );
}

BEGIN_MESSAGE_MAP(CExtGridWnd,CExtGridBaseWnd)
	//{{AFX_MSG_MAP(CExtGridWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#ifdef _DEBUG
void CExtGridWnd::AssertValid() const
{
	CExtGridBaseWnd::AssertValid();
//	ASSERT( m_nCountOfColumns >= 0 );
//	ASSERT( m_nCountOfRows >= 0 );
//DWORD dwScrollTypeH = m_dwScrollItemWndStyle&__ESIS_STH_MASK;
//DWORD dwScrollTypeV = ((m_dwScrollItemWndStyle&__ESIS_STV_MASK) >> __ESIW_ST_BIT_COUNT);
//	ASSERT( dwScrollTypeH != __ESIW_ST_VIRTUAL );
//	ASSERT( dwScrollTypeV != __ESIW_ST_VIRTUAL );

//	if( m_pDataProvider != NULL )
//	{
//		LONG nCountOfColumnsDP = (LONG)m_pDataProvider->ColumnCountGet();
//		LONG nCountOfRowsDP = (LONG)m_pDataProvider->RowCountGet();
//		ASSERT( m_nCountOfColumns == nCountOfColumnsDP );
//		ASSERT( m_nCountOfRows == nCountOfRowsDP );
//	} // if( m_pDataProvider != NULL )
}
void CExtGridWnd::Dump( CDumpContext & dc ) const
{
	CExtGridBaseWnd::Dump( dc );
}
#endif // _DEBUG

bool CExtGridWnd::ExternalDataGet() const
{
	ASSERT_VALID( this );
DWORD dwSiwStyleEx = SiwGetStyleEx();
bool bExternalData =
		( (dwSiwStyleEx&__EGWS_EX_EXTERNAL_DATA) != 0L )
			? true : false;
	return bExternalData;
}

void CExtGridWnd::ExternalDataSet( bool bExternalData )
{
	ASSERT_VALID( this );
	SiwModifyStyleEx(
		bExternalData ? __EGWS_EX_EXTERNAL_DATA : 0L,
		bExternalData ? 0L : __EGWS_EX_EXTERNAL_DATA,
		false
		);
}

CExtGridDataProvider & CExtGridWnd::OnGridQueryDataProvider()
{
	ASSERT_VALID( this );
	if( m_pDataProvider != NULL )
	{
		ASSERT_VALID( m_pDataProvider );
		return (*m_pDataProvider);
	} // if( m_pDataProvider != NULL )

//CExtGridDataProviderMemory * pDPMEM = new CExtGridDataProviderMemory;
//	ASSERT_VALID( pDPMEM );
//CExtGridDataProviderRT * pDPRT = new CExtGridDataProviderRT( pDPMEM, true, true );
//	ASSERT_VALID( pDPRT );
//	m_pDataProvider = pDPRT;

	m_pDataProvider = new CExtGridDataProviderMemory;
	ASSERT_VALID( m_pDataProvider );
	return (*m_pDataProvider);
}

const CExtGridDataProvider & CExtGridWnd::OnGridQueryDataProvider() const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridWnd * > ( this ) )
			-> OnGridQueryDataProvider();
}

LONG CExtGridWnd::OuterColumnCountLeftSet(
	LONG nCount, // = 0L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( nCount >= 0 );
LONG nOuterColCountLeft = CExtGridBaseWnd::OuterColumnCountLeftGet();
	if( nOuterColCountLeft == nCount )
	{
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
		return true;
	} // if( nOuterColCountLeft == nCount )
LONG nOuterColCountRight = CExtGridBaseWnd::OuterColumnCountRightGet();
LONG nOuterRowCountTop = CExtGridBaseWnd::OuterRowCountTopGet();
LONG nOuterRowCountBottom = CExtGridBaseWnd::OuterRowCountBottomGet();
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	if( ! _DataProvider.CacheReserveForOuterCells(
			nCount + nOuterColCountRight,
			nOuterRowCountTop + nOuterRowCountBottom
			)
		)
	{ // if data provider does not have own allocation strategy for outer cells
		if( nCount > nOuterColCountLeft )
		{
			if( ! _DataProvider.ColumnInsert(
					nOuterColCountLeft,
					nCount - nOuterColCountLeft
					)
				)
			{
				ASSERT( FALSE );
				return false;
			}
		} // if( nCount > nOuterColCountLeft )
		else
		{
			if( ! _DataProvider.ColumnRemove(
					nOuterColCountLeft - nCount,
					nOuterColCountLeft - nCount
					)
				)
			{
				ASSERT( FALSE );
				return false;
			}
		} // else from if( nCount > nOuterColCountLeft )
	} // if data provider does not have own allocation strategy for outer cells
	return
		CExtGridBaseWnd::OuterColumnCountLeftSet(
			nCount,
			bRedraw
			);
}

LONG CExtGridWnd::OuterColumnCountRightSet(
	LONG nCount, // = 0L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( nCount >= 0 );
LONG nOuterColCountRight = CExtGridBaseWnd::OuterColumnCountRightGet();
	if( nOuterColCountRight == nCount )
	{
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
		return true;
	} // if( nOuterColCountRight == nCount )
LONG nOuterColCountLeft = CExtGridBaseWnd::OuterColumnCountLeftGet();
LONG nOuterRowCountTop = CExtGridBaseWnd::OuterRowCountTopGet();
LONG nOuterRowCountBottom = CExtGridBaseWnd::OuterRowCountBottomGet();
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	if( ! _DataProvider.CacheReserveForOuterCells(
			nOuterColCountLeft + nCount,
			nOuterRowCountTop + nOuterRowCountBottom
			)
		)
	{ // if data provider does not have own allocation strategy for outer cells
		if( nCount > nOuterColCountRight )
		{
			if( ! _DataProvider.ColumnInsert(
					nOuterColCountRight + nOuterColCountLeft,
					nCount - nOuterColCountRight
					)
				)
			{
				ASSERT( FALSE );
				return false;
			}
		} // if( nCount > nOuterColCountRight )
		else
		{
			if( ! _DataProvider.ColumnRemove(
					nOuterColCountRight - nCount + nOuterColCountLeft,
					nOuterColCountRight - nCount
					)
				)
			{
				ASSERT( FALSE );
				return false;
			}
		} // else from if( nCount > nOuterColCountRight )
	} // if data provider does not have own allocation strategy for outer cells
	return
		CExtGridBaseWnd::OuterColumnCountRightSet(
			nCount,
			bRedraw
			);
}

LONG CExtGridWnd::OuterRowCountTopSet(
	LONG nCount, // = 0L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( nCount >= 0 );
LONG nOuterRowCountTop = CExtGridBaseWnd::OuterRowCountTopGet();
	if( nOuterRowCountTop == nCount )
	{
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
		return true;
	} // if( nOuterRowCountTop == nCount )
LONG nOuterColCountLeft = CExtGridBaseWnd::OuterColumnCountLeftGet();
LONG nOuterColCountRight = CExtGridBaseWnd::OuterColumnCountRightGet();
LONG nOuterRowCountBottom = CExtGridBaseWnd::OuterRowCountBottomGet();
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	if( ! _DataProvider.CacheReserveForOuterCells(
			nOuterColCountLeft + nOuterColCountRight,
			nCount + nOuterRowCountBottom
			)
		)
	{ // if data provider does not have own allocation strategy for outer cells
		if( nCount > nOuterRowCountTop )
		{
			if( ! _DataProvider.RowInsert(
					nOuterRowCountTop,
					nCount - nOuterRowCountTop
					)
				)
			{
				ASSERT( FALSE );
				return false;
			}
		} // if( nCount > nOuterRowCountTop )
		else
		{
			if( ! _DataProvider.RowRemove(
					nOuterRowCountTop - nCount,
					nOuterRowCountTop - nCount
					)
				)
			{
				ASSERT( FALSE );
				return false;
			}
		} // else from if( nCount > nOuterRowCountTop )
	} // if data provider does not have own allocation strategy for outer cells
	return
		CExtGridBaseWnd::OuterRowCountTopSet(
			nCount,
			bRedraw
			);
}

LONG CExtGridWnd::OuterRowCountBottomSet(
	LONG nCount, // = 0L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( nCount >= 0 );
LONG nOuterRowCountBottom = CExtGridBaseWnd::OuterRowCountBottomGet();
	if( nOuterRowCountBottom == nCount )
	{
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
		return true;
	} // if( nOuterRowCountBottom == nCount )
LONG nOuterColCountLeft = CExtGridBaseWnd::OuterColumnCountLeftGet();
LONG nOuterColCountRight = CExtGridBaseWnd::OuterColumnCountRightGet();
LONG nOuterRowCountTop = CExtGridBaseWnd::OuterRowCountTopGet();
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	if( ! _DataProvider.CacheReserveForOuterCells(
			nOuterColCountLeft + nOuterColCountRight,
			nOuterRowCountTop + nCount
			)
		)
	{ // if data provider does not have own allocation strategy for outer cells
		if( nCount > nOuterRowCountBottom )
		{
			if( ! _DataProvider.RowInsert(
					nOuterRowCountBottom + nOuterRowCountTop,
					nCount - nOuterRowCountBottom
					)
				)
			{
				ASSERT( FALSE );
				return false;
			}
		} // if( nCount > nOuterRowCountBottom )
		else
		{
			if( ! _DataProvider.RowRemove(
					nOuterRowCountBottom - nCount + nOuterRowCountTop,
					nOuterRowCountBottom - nCount
					)
				)
			{
				ASSERT( FALSE );
				return false;
			}
		} // else from if( nCount > nOuterRowCountBottom )
	} // if data provider does not have own allocation strategy for outer cells
	return
		CExtGridBaseWnd::OuterRowCountBottomSet(
			nCount,
			bRedraw
			);
}

bool CExtGridWnd::ColumnInsert(
	LONG nColNo, // -1 or greater than count - append
	LONG nColInsertCount, // = 1L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( ExternalDataGet() )
	{
		ASSERT( FALSE );
		return false;
	}
LONG nColCount = /*CExtGridBaseWnd::*/ ColumnCountGet();
	if( nColNo < 0 )
		nColNo = nColCount;
	if(		nColInsertCount < 0
		||	nColNo > nColCount
		)
	{
		ASSERT( FALSE );
		return false;
	} // if( nColNo < 0 ...
	if( nColInsertCount == 0 )
	{
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
		return true;
	} // if( nColInsertCount == 0 )
	OnGbwHoverRemove();
LONG nOuterColCountRight = CExtGridBaseWnd::OuterColumnCountRightGet();
LONG nOuterColCountLeft = CExtGridBaseWnd::OuterColumnCountLeftGet();
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	if( ! _DataProvider.ColumnInsert(
			nColNo + nOuterColCountLeft + nOuterColCountRight,
			nColInsertCount
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
#ifdef _DEBUG
	m_bDebugModifyingColumns = true;
#endif // _DEBUG
bool bRetVal =
		CExtGridBaseWnd::ColumnInsert(
			nColNo,
			nColInsertCount,
			bRedraw
			);
	ASSERT( bRetVal );
#ifdef _DEBUG
	m_bDebugModifyingColumns = false;
#endif // _DEBUG
	return bRetVal;
}

bool CExtGridWnd::RowInsert(
	LONG nRowNo, // -1 or greater than count - append
	LONG nRowInsertCount, // = 1L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( ExternalDataGet() )
	{
		ASSERT( FALSE );
		return false;
	}
LONG nRowCount = /*CExtGridBaseWnd::*/ RowCountGet();
	if( nRowNo < 0 )
		nRowNo = nRowCount;
	if(		nRowInsertCount < 0
		||	nRowNo > nRowCount
		)
	{
		ASSERT( FALSE );
		return false;
	} // if( nRowNo < 0 ...
	if( nRowInsertCount == 0 )
	{
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
		return true;
	} // if( nRowInsertCount == 0 )
	OnGbwHoverRemove();
LONG nOuterRowCountBottom = CExtGridBaseWnd::OuterRowCountBottomGet();
LONG nOuterRowCountTop = CExtGridBaseWnd::OuterRowCountTopGet();
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	if( ! _DataProvider.RowInsert(
			nRowNo + nOuterRowCountTop + nOuterRowCountBottom,
			nRowInsertCount
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
#ifdef _DEBUG
	m_bDebugModifyingRows = true;
#endif // _DEBUG
bool bRetVal =
		CExtGridBaseWnd::RowInsert(
			nRowNo,
			nRowInsertCount,
			bRedraw
			);
	ASSERT( bRetVal );
#ifdef _DEBUG
	m_bDebugModifyingRows = false;
#endif // _DEBUG
	return bRetVal;
}

LONG CExtGridWnd::ColumnRemove(
	LONG nColNo,
	LONG nColRemoveCount, // = 1L // -1 - remove up to end (truncate)
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( ExternalDataGet() )
	{
		ASSERT( FALSE );
		return false;
	}
LONG nColCount = /*CExtGridBaseWnd::*/ ColumnCountGet();
	if( nColRemoveCount < 0 )
		nColRemoveCount = nColCount;
	if(		nColNo < 0
		||	(nColNo+nColRemoveCount) > nColCount
		)
	{
		ASSERT( FALSE );
		return false;
	} // if( nColNo < 0 ...
	if( nColRemoveCount == 0 )
	{
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
		return true;
	} // if( nColRemoveCount == 0 )
	OnGbwHoverRemove();
LONG nOuterColCountRight = CExtGridBaseWnd::OuterColumnCountRightGet();
LONG nOuterColCountLeft = CExtGridBaseWnd::OuterColumnCountLeftGet();
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	if( ! _DataProvider.ColumnRemove(
			nColNo + nOuterColCountLeft + nOuterColCountRight,
			nColRemoveCount
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
#ifdef _DEBUG
	m_bDebugModifyingColumns = true;
#endif // _DEBUG
LONG nRetVal =
		CExtGridBaseWnd::ColumnRemove(
			nColNo,
			nColRemoveCount,
			bRedraw
			);
#ifdef _DEBUG
	m_bDebugModifyingColumns = false;
#endif // _DEBUG
	return nRetVal;
}

LONG CExtGridWnd::RowRemove(
	LONG nRowNo,
	LONG nRowRemoveCount, // = 1L // -1 - remove up to end (truncate)
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( ExternalDataGet() )
	{
		ASSERT( FALSE );
		return false;
	}
LONG nRowCount = /*CExtGridBaseWnd::*/ RowCountGet();
	if( nRowRemoveCount < 0 )
		nRowRemoveCount = nRowCount;
	if(		nRowNo < 0
		||	(nRowNo+nRowRemoveCount) > nRowCount
		)
	{
		ASSERT( FALSE );
		return false;
	} // if( nRowNo < 0 ...
	if( nRowRemoveCount == 0 )
	{
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
		return true;
	} // if( nRowRemoveCount == 0 )
	OnGbwHoverRemove();
LONG nOuterRowCountBottom = CExtGridBaseWnd::OuterRowCountBottomGet();
LONG nOuterRowCountTop = CExtGridBaseWnd::OuterRowCountTopGet();
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	if( ! _DataProvider.RowRemove(
			nRowNo + nOuterRowCountTop + nOuterRowCountBottom,
			nRowRemoveCount
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
#ifdef _DEBUG
	m_bDebugModifyingRows = true;
#endif // _DEBUG
LONG nRetVal =
		CExtGridBaseWnd::RowRemove(
			nRowNo,
			nRowRemoveCount,
			bRedraw
			);
#ifdef _DEBUG
	m_bDebugModifyingRows = false;
#endif // _DEBUG
	return nRetVal;
}

#ifdef _DEBUG
LONG CExtGridWnd::ColumnCountSet(
	LONG nCount, // = -1L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT(
			m_bDebugModifyingColumns
		||	nCount == /*CExtGridBaseWnd::*/ ColumnCountGet()
		||	nCount < 0L // switching to virtual mode
		);
	return
		CExtGridBaseWnd::ColumnCountSet(
			nCount,
			bRedraw
			);
}
LONG CExtGridWnd::RowCountSet(
	LONG nCount, // = -1L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT(
			m_bDebugModifyingRows
		||	nCount == /*CExtGridBaseWnd::*/ RowCountGet()
		||	nCount < 0L // switching to virtual mode
		);
	return
		CExtGridBaseWnd::RowCountSet(
			nCount,
			bRedraw
			);
}
#endif // _DEBUG

LONG CExtGridWnd::ColumnCountGet() const
{
	ASSERT_VALID( this );
DWORD dwScrollTypeH = SiwScrollTypeHGet();
	if(		dwScrollTypeH == __ESIW_ST_VIRTUAL
		||	( ! ExternalDataGet() )
		)	
		return CExtGridBaseWnd::ColumnCountGet();
const CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
LONG nCount = _DataProvider.ColumnCountGet();
LONG nOuterBefore = OuterColumnCountLeftGet();
LONG nOuterAfter = OuterColumnCountRightGet();
	nCount -= nOuterBefore + nOuterAfter;
	ASSERT( nCount >= 0L );
	return nCount;
}

LONG CExtGridWnd::RowCountGet() const
{
	ASSERT_VALID( this );
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	if(		dwScrollTypeV == __ESIW_ST_VIRTUAL
		||	( ! ExternalDataGet() )
		)	
		return CExtGridBaseWnd::RowCountGet();
const CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
LONG nCount = _DataProvider.RowCountGet();
LONG nOuterBefore = OuterRowCountTopGet();
LONG nOuterAfter = OuterRowCountBottomGet();
	nCount -= nOuterBefore + nOuterAfter;
	ASSERT( nCount >= 0L );
	return nCount;
}

CExtGridCell * CExtGridWnd::GridCellGet(
	const CExtGridHitTestInfo & htInfo,
	CRuntimeClass * pInitRTC, // = NULL
	bool bAutoFindValue, // = true // auto find row/column default value (only when pInitRTC=NULL)
	bool bUseColumnDefaultValue // = true // false - use row default value (only when bAutoFindValue=true)
	)
{
	ASSERT_VALID( this );
	if( htInfo.IsHoverEmpty() )
		return NULL;
	return
		GridCellGet(
			htInfo.m_nColNo,
			htInfo.m_nRowNo,
			htInfo.GetInnerOuterTypeOfColumn(),
			htInfo.GetInnerOuterTypeOfRow(),
			pInitRTC,
			bAutoFindValue,
			bUseColumnDefaultValue
			);
}

const CExtGridCell * CExtGridWnd::GridCellGet(
	const CExtGridHitTestInfo & htInfo,
	CRuntimeClass * pInitRTC, // = NULL
	bool bAutoFindValue, // = true // auto find row/column default value (only when pInitRTC=NULL)
	bool bUseColumnDefaultValue // = true // false - use row default value (only when bAutoFindValue=true)
	) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridWnd * > ( this ) )
			-> GridCellGet(
				htInfo,
				pInitRTC,
				bAutoFindValue,
				bUseColumnDefaultValue
				);
}

const CExtGridCell * CExtGridWnd::GridCellGet(
	LONG nColNo,
	LONG nRowNo,
	INT nColType, // = 0 // -1 - nColNo is fixed column at left, 1 - at right, 0 - data cell
	INT nRowType, // = 0 // -1 - nRowNo is fixed column at top, 1 - at bottom, 0 - data cell
	CRuntimeClass * pInitRTC, // = NULL
	bool bAutoFindValue, // = true // auto find row/column default value (only when pInitRTC=NULL)
	bool bUseColumnDefaultValue // = true // false - use row default value (only when bAutoFindValue=true)
	) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridWnd * > ( this ) )
			-> GridCellGet(
				nColNo,
				nRowNo,
				nColType,
				nRowType,
				pInitRTC,
				bAutoFindValue,
				bUseColumnDefaultValue
				);
}

CExtGridCell * CExtGridWnd::GridCellGetOuterAtTop(
	LONG nColNo,
	LONG nRowNo,
	CRuntimeClass * pInitRTC, // = NULL
	bool bAutoFindValue, // = true // auto find row/column default value (only when pInitRTC=NULL)
	bool bUseColumnDefaultValue // = true // false - use row default value (only when bAutoFindValue=true)
	)
{
	ASSERT_VALID( this );
	return
		GridCellGet(
			nColNo,
			nRowNo,
			0,
			-1,
			pInitRTC,
			bAutoFindValue,
			bUseColumnDefaultValue
			);
}

const CExtGridCell * CExtGridWnd::GridCellGetOuterAtTop(
	LONG nColNo,
	LONG nRowNo,
	CRuntimeClass * pInitRTC, // = NULL
	bool bAutoFindValue, // = true // auto find row/column default value (only when pInitRTC=NULL)
	bool bUseColumnDefaultValue // = true // false - use row default value (only when bAutoFindValue=true)
	) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridWnd * > ( this ) )
			-> GridCellGetOuterAtTop(
				nColNo,
				nRowNo,
				pInitRTC,
				bAutoFindValue,
				bUseColumnDefaultValue
				);
}

CExtGridCell * CExtGridWnd::GridCellGetOuterAtBottom(
	LONG nColNo,
	LONG nRowNo,
	CRuntimeClass * pInitRTC, // = NULL
	bool bAutoFindValue, // = true // auto find row/column default value (only when pInitRTC=NULL)
	bool bUseColumnDefaultValue // = true // false - use row default value (only when bAutoFindValue=true)
	)
{
	ASSERT_VALID( this );
	return
		GridCellGet(
			nColNo,
			nRowNo,
			0,
			1,
			pInitRTC,
			bAutoFindValue,
			bUseColumnDefaultValue
			);
}

const CExtGridCell * CExtGridWnd::GridCellGetOuterAtBottom(
	LONG nColNo,
	LONG nRowNo,
	CRuntimeClass * pInitRTC, // = NULL
	bool bAutoFindValue, // = true // auto find row/column default value (only when pInitRTC=NULL)
	bool bUseColumnDefaultValue // = true // false - use row default value (only when bAutoFindValue=true)
	) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridWnd * > ( this ) )
			-> GridCellGetOuterAtBottom(
				nColNo,
				nRowNo,
				pInitRTC,
				bAutoFindValue,
				bUseColumnDefaultValue
				);
}

CExtGridCell * CExtGridWnd::GridCellGetOuterAtLeft(
	LONG nColNo,
	LONG nRowNo,
	CRuntimeClass * pInitRTC, // = NULL
	bool bAutoFindValue, // = true // auto find row/column default value (only when pInitRTC=NULL)
	bool bUseColumnDefaultValue // = true // false - use row default value (only when bAutoFindValue=true)
	)
{
	ASSERT_VALID( this );
	return
		GridCellGet(
			nColNo,
			nRowNo,
			-1,
			0,
			pInitRTC,
			bAutoFindValue,
			bUseColumnDefaultValue
			);
}

const CExtGridCell * CExtGridWnd::GridCellGetOuterAtLeft(
	LONG nColNo,
	LONG nRowNo,
	CRuntimeClass * pInitRTC, // = NULL
	bool bAutoFindValue, // = true // auto find row/column default value (only when pInitRTC=NULL)
	bool bUseColumnDefaultValue // = true // false - use row default value (only when bAutoFindValue=true)
	) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridWnd * > ( this ) )
			-> GridCellGetOuterAtLeft(
				nColNo,
				nRowNo,
				pInitRTC,
				bAutoFindValue,
				bUseColumnDefaultValue
				);
}

CExtGridCell * CExtGridWnd::GridCellGetOuterAtRight(
	LONG nColNo,
	LONG nRowNo,
	CRuntimeClass * pInitRTC, // = NULL
	bool bAutoFindValue, // = true // auto find row/column default value (only when pInitRTC=NULL)
	bool bUseColumnDefaultValue // = true // false - use row default value (only when bAutoFindValue=true)
	)
{
	ASSERT_VALID( this );
	return
		GridCellGet(
			nColNo,
			nRowNo,
			1,
			0,
			pInitRTC,
			bAutoFindValue,
			bUseColumnDefaultValue
			);
}

const CExtGridCell * CExtGridWnd::GridCellGetOuterAtRight(
	LONG nColNo,
	LONG nRowNo,
	CRuntimeClass * pInitRTC, // = NULL
	bool bAutoFindValue, // = true // auto find row/column default value (only when pInitRTC=NULL)
	bool bUseColumnDefaultValue // = true // false - use row default value (only when bAutoFindValue=true)
	) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridWnd * > ( this ) )
			-> GridCellGetOuterAtRight(
				nColNo,
				nRowNo,
				pInitRTC,
				bAutoFindValue,
				bUseColumnDefaultValue
				);
}

bool CExtGridWnd::GridCellSetOuterAtTop(
	LONG nColNo,
	LONG nRowNo,
	const CExtGridCell * pCell, // = NULL // NULL - clear (make empty)
	LONG nColCount, // = 1L
	LONG nRowCount, // = 1L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	return GridCellSet( nColNo, nRowNo, pCell, nColCount, nRowCount, 0, -1, bRedraw );
}

bool CExtGridWnd::GridCellSetOuterAtBottom(
	LONG nColNo,
	LONG nRowNo,
	const CExtGridCell * pCell, // = NULL // NULL - clear (make empty)
	LONG nColCount, // = 1L
	LONG nRowCount, // = 1L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	return GridCellSet( nColNo, nRowNo, pCell, nColCount, nRowCount, 0, 1, bRedraw );
}

bool CExtGridWnd::GridCellSetOuterAtLeft(
	LONG nColNo,
	LONG nRowNo,
	const CExtGridCell * pCell, // = NULL // NULL - clear (make empty)
	LONG nColCount, // = 1L
	LONG nRowCount, // = 1L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	return GridCellSet( nColNo, nRowNo, pCell, nColCount, nRowCount, -1, 0, bRedraw );
}

bool CExtGridWnd::GridCellSetOuterAtRight(
	LONG nColNo,
	LONG nRowNo,
	const CExtGridCell * pCell, // = NULL // NULL - clear (make empty)
	LONG nColCount, // = 1L
	LONG nRowCount, // = 1L
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	return GridCellSet( nColNo, nRowNo, pCell, nColCount, nRowCount, 1, 0, bRedraw );
}

CExtGridCell * CExtGridWnd::GridCellGet(
	LONG nColNo,
	LONG nRowNo,
	INT nColType, // = 0 // -1 - nColNo is fixed column at left, 1 - at right, 0 - data cell
	INT nRowType, // = 0 // -1 - nRowNo is fixed column at top, 1 - at bottom, 0 - data cell
	CRuntimeClass * pInitRTC, // = NULL
	bool bAutoFindValue, // = true // auto find row/column default value (only when pInitRTC=NULL)
	bool bUseColumnDefaultValue // = true // false - use row default value (only when bAutoFindValue=true)
	)
{
	ASSERT_VALID( this );
	if( nColNo < 0L || nRowNo < 0L )
	{
		// invalid cell position
		ASSERT( FALSE );
		return NULL;
	} // if( nColNo < 0L || nRowNo < 0L )
LONG nEffectiveColNo = nColNo;
LONG nEffectiveRowNo = nRowNo;
	if( nColType < 0 )
	{
		if( nRowType != 0 )
		{
			// only one of nColType, nRowType may be non-zero
			ASSERT( FALSE );
			return NULL;
		} // if( nRowType != 0 )
		LONG nOuterColCountLeft = CExtGridBaseWnd::OuterColumnCountLeftGet();
		if( nColNo >= nOuterColCountLeft )
		{
			// invalid cell position
			ASSERT( FALSE );
			return NULL;
		}
	} // if( nColType < 0 )
	else if( nColType > 0 )
	{
		if( nRowType != 0 )
		{
			// only one of nColType, nRowType may be non-zero
			ASSERT( FALSE );
			return NULL;
		} // if( nRowType != 0 )
		LONG nOuterColCountRight = CExtGridBaseWnd::OuterColumnCountRightGet();
		if( nColNo >= nOuterColCountRight )
		{
			// invalid cell position
			ASSERT( FALSE );
			return NULL;
		}
		LONG nOuterColCountLeft = CExtGridBaseWnd::OuterColumnCountLeftGet();
		nEffectiveColNo += nOuterColCountLeft;
	} // else if( nColType > 0 )
	else
	{
		LONG nInnerColCount =
			(SiwScrollTypeVGet() == __ESIW_ST_VIRTUAL)
				? LONG( OnGridQueryDataProvider().CacheColumnCountGet() )
				: /*CExtGridBaseWnd::*/ ColumnCountGet()
				;
		if( nColNo >= nInnerColCount )
		{
			// invalid cell position
//			ASSERT( FALSE );
			return NULL;
		} // if( nColNo >= nInnerColCount )
		LONG nOuterColCountLeft = CExtGridBaseWnd::OuterColumnCountLeftGet();
		LONG nOuterColCountRight = CExtGridBaseWnd::OuterColumnCountRightGet();
		LONG nEffectiveShift = nOuterColCountLeft + nOuterColCountRight;
		nEffectiveColNo += nEffectiveShift;
	} // else from else if( nColType > 0 )
	if( nRowType < 0 )
	{
		if( nColType != 0 )
		{
			// only one of nRowType, nRowType may be non-zero
			ASSERT( FALSE );
			return NULL;
		} // if( nColType != 0 )
		LONG nOuterRowCountTop = CExtGridBaseWnd::OuterRowCountTopGet();
		if( nRowNo >= nOuterRowCountTop )
		{
			// invalid cell position
			ASSERT( FALSE );
			return NULL;
		}
	} // if( nRowType < 0 )
	else if( nRowType > 0 )
	{
		if( nColType != 0 )
		{
			// only one of nRowType, nRowType may be non-zero
			ASSERT( FALSE );
			return NULL;
		} // if( nColType != 0 )
		LONG nOuterRowCountBottom = CExtGridBaseWnd::OuterRowCountBottomGet();
		if( nRowNo >= nOuterRowCountBottom )
		{
			// invalid cell position
			ASSERT( FALSE );
			return NULL;
		}
		LONG nOuterRowCountTop = CExtGridBaseWnd::OuterRowCountTopGet();
		nEffectiveRowNo += nOuterRowCountTop;
	} // else if( nRowType > 0 )
	else
	{
		LONG nInnerRowCount =
			(SiwScrollTypeVGet() == __ESIW_ST_VIRTUAL)
				? LONG( OnGridQueryDataProvider().CacheRowCountGet() )
				: /*CExtGridBaseWnd::*/ RowCountGet()
				;
		if( nRowNo >= nInnerRowCount )
		{
			// invalid cell position
//			ASSERT( FALSE );
			return NULL;
		} // if( nRowNo >= nInnerRowCount )
		LONG nOuterRowCountTop = CExtGridBaseWnd::OuterRowCountTopGet();
		LONG nOuterRowCountBottom = CExtGridBaseWnd::OuterRowCountBottomGet();
		LONG nEffectiveShift = nOuterRowCountTop + nOuterRowCountBottom;
		nEffectiveRowNo += nEffectiveShift;
	} // else from else if( nRowType > 0 )
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
CExtGridCell * pCell =
		_DataProvider.CellGet(
			ULONG(nEffectiveColNo),
			ULONG(nEffectiveRowNo),
			pInitRTC,
			bAutoFindValue,
			bUseColumnDefaultValue
			);
#ifdef _DEBUG
	if( pCell != NULL )
	{
		ASSERT_VALID( pCell );
		ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
		//ASSERT( pCell->DataProviderGet() == (&_DataProvider) );
		ASSERT_VALID( pCell->DataProviderGet() );
	} // if( pCell != NULL )
#endif // _DEBUG
	return pCell;
}

bool CExtGridWnd::GridCellSet(
	LONG nColNo,
	LONG nRowNo,
	const CExtGridCell * pCell, // = NULL // NULL - clear (make empty)
	LONG nColCount, // = 1L
	LONG nRowCount, // = 1L
	INT nColType, // = 0 // -1 - nColNo is fixed column at left, 1 - at right, 0 - data cell
	INT nRowType, // = 0 // -1 - nRowNo is fixed column at top, 1 - at bottom, 0 - data cell
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( nColNo < 0L || nRowNo < 0L || nColCount < 0L || nRowCount < 0L )
	{
		// invalid cell position
		ASSERT( FALSE );
		return false;
	} // if( nColNo < 0L || nRowNo < 0L || nColCount < 0L || nRowCount < 0L )
	if( nColCount == 0L || nRowCount == 0L )
	{
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
		return true;
	} // if( nColCount == 0L || nRowCount == 0L )
LONG nEffectiveColNo = nColNo;
LONG nEffectiveRowNo = nRowNo;
	if( nColType < 0 )
	{
		if( nRowType != 0 )
		{
			// only one of nColType, nRowType may be non-zero
			ASSERT( FALSE );
			return false;
		} // if( nRowType != 0 )
		LONG nOuterColCountLeft = CExtGridBaseWnd::OuterColumnCountLeftGet();
		if(		nColNo >= nOuterColCountLeft
			||	(nColNo+nColCount) > nOuterColCountLeft
			)
		{
			// invalid cell position
			ASSERT( FALSE );
			return false;
		}
	} // if( nColType < 0 )
	else if( nColType > 0 )
	{
		if( nRowType != 0 )
		{
			// only one of nColType, nRowType may be non-zero
			ASSERT( FALSE );
			return false;
		} // if( nRowType != 0 )
		LONG nOuterColCountRight = CExtGridBaseWnd::OuterColumnCountRightGet();
		if(		nColNo >= nOuterColCountRight
			||	(nColNo+nColCount) > nOuterColCountRight
			)
		{
			// invalid cell position
			ASSERT( FALSE );
			return false;
		}
		LONG nOuterColCountLeft = CExtGridBaseWnd::OuterColumnCountLeftGet();
		nEffectiveColNo += nOuterColCountLeft;
	} // else if( nColType > 0 )
	else
	{
		LONG nInnerColCount = /*CExtGridBaseWnd::*/ ColumnCountGet();
		if(		nColNo >= nInnerColCount
			||	(nColNo+nColCount) > nInnerColCount
			)
		{
			// invalid cell position
			ASSERT( FALSE );
			return false;
		} // if( nColNo >= nInnerColCount )
		LONG nOuterColCountLeft = CExtGridBaseWnd::OuterColumnCountLeftGet();
		LONG nOuterColCountRight = CExtGridBaseWnd::OuterColumnCountRightGet();
		LONG nEffectiveShift = nOuterColCountLeft + nOuterColCountRight;
		nEffectiveColNo += nEffectiveShift;
	} // else from else if( nColType > 0 )
	if( nRowType < 0 )
	{
		if( nColType != 0 )
		{
			// only one of nRowType, nRowType may be non-zero
			ASSERT( FALSE );
			return false;
		} // if( nColType != 0 )
		LONG nOuterRowCountTop = CExtGridBaseWnd::OuterRowCountTopGet();
		if(		nRowNo >= nOuterRowCountTop
			||	(nRowNo+nRowCount) > nOuterRowCountTop
			)
		{
			// invalid cell position
			ASSERT( FALSE );
			return false;
		}
	} // if( nRowType < 0 )
	else if( nRowType > 0 )
	{
		if( nColType != 0 )
		{
			// only one of nRowType, nRowType may be non-zero
			ASSERT( FALSE );
			return false;
		} // if( nColType != 0 )
		LONG nOuterRowCountBottom = CExtGridBaseWnd::OuterRowCountBottomGet();
		if(		nRowNo >= nOuterRowCountBottom
			||	(nRowNo+nRowCount) > nOuterRowCountBottom
			)
		{
			// invalid cell position
			ASSERT( FALSE );
			return false;
		}
		LONG nOuterRowCountTop = CExtGridBaseWnd::OuterRowCountTopGet();
		nEffectiveRowNo += nOuterRowCountTop;
	} // else if( nRowType > 0 )
	else
	{
		LONG nInnerRowCount = /*CExtGridBaseWnd::*/ RowCountGet();
		if(		nRowNo >= nInnerRowCount
			||	(nRowNo+nRowCount) > nInnerRowCount
			)
		{
			// invalid cell position
			ASSERT( FALSE );
			return false;
		} // if( nRowNo >= nInnerRowCount )
		LONG nOuterRowCountTop = CExtGridBaseWnd::OuterRowCountTopGet();
		LONG nOuterRowCountBottom = CExtGridBaseWnd::OuterRowCountBottomGet();
		LONG nEffectiveShift = nOuterRowCountTop + nOuterRowCountBottom;
		nEffectiveRowNo += nEffectiveShift;
	} // else from else if( nRowType > 0 )
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
bool bRetVal =
		_DataProvider.CellRangeSet(
			ULONG(nEffectiveColNo),
			ULONG(nEffectiveRowNo),
			ULONG(nColCount),
			ULONG(nRowCount),
			pCell
			);
	ASSERT( bRetVal );
	if( bRedraw )
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	} // if( bRedraw )
	return bRetVal;
}

void CExtGridWnd::OnGridPaintDraggedCell(
	CDC & dc,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	const RECT & rcCell,
	DWORD dwAreaFlags
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( nVisibleColNo >= 0 );
	ASSERT( nVisibleRowNo >= 0 );
	ASSERT( nColNo >= 0 );
	ASSERT( nRowNo >= 0 );
	ASSERT( rcCell.right >= rcCell.left );
	ASSERT( rcCell.bottom >= rcCell.top );
INT nColType = CExtGridHitTestInfo::GetInnerOuterTypeOfColumn( dwAreaFlags );
INT nRowType = CExtGridHitTestInfo::GetInnerOuterTypeOfRow( dwAreaFlags );
DWORD dwHelperPaintFlags = __EGCPF_OUTER_DND;
CRect rcVisibleRange = OnSiwGetVisibleRange();
INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
CFont * pOldFont = dc.SelectObject( &OnSiwGetDefaultFont() );
const CExtGridCell * pCell =
		GridCellGet( nColNo, nRowNo, nColType, nRowType );
	if( pCell == NULL )
	{
		CExtGridBaseWnd::OnGbwPaintCell(
			dc,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			rcCell, // extra-space
			rcCell,
			rcVisibleRange,
			dwAreaFlags,
			dwHelperPaintFlags
			);
	} // if( pCell == NULL )
	else
	{
		if( ! pCell->OnPaintBackground(
				*this,
				dc,
				nVisibleColNo,
				nVisibleRowNo,
				nColNo,
				nRowNo,
				nColType,
				nRowType,
				rcCell, // extra-space
				rcCell,
				rcVisibleRange,
				dwAreaFlags,
				dwHelperPaintFlags
				)
			)
			CExtGridBaseWnd::OnGbwPaintCell(
				dc,
				nVisibleColNo,
				nVisibleRowNo,
				nColNo,
				nRowNo,
				rcCell, // extra-space
				rcCell,
				rcVisibleRange,
				dwAreaFlags,
				dwHelperPaintFlags
				);
		pCell->OnPaintForeground(
			*this,
			dc,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			nColType,
			nRowType,
			rcCell, // extra-space
			rcCell,
			rcVisibleRange,
			dwAreaFlags,
			dwHelperPaintFlags
			);
	} // else from if( pCell == NULL )
	dc.SelectObject( pOldFont );
	dc.SetBkMode( nOldBkMode );
}

void CExtGridWnd::OnGbwPaintCell(
	CDC & dc,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	const RECT & rcVisibleRange,
	DWORD dwAreaFlags,
	DWORD dwHelperPaintFlags
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( nVisibleColNo >= 0 );
	ASSERT( nVisibleRowNo >= 0 );
	ASSERT( nColNo >= 0 );
	ASSERT( nRowNo >= 0 );
	ASSERT( rcCell.right >= rcCell.left );
	ASSERT( rcCell.bottom >= rcCell.top );
INT nColType = CExtGridHitTestInfo::GetInnerOuterTypeOfColumn( dwAreaFlags );
INT nRowType = CExtGridHitTestInfo::GetInnerOuterTypeOfRow( dwAreaFlags );
DWORD dwBseStyleEx = BseGetStyleEx();
	if( (dwBseStyleEx&__EGBS_BSE_EX_HIGHLIGHT_PRESSING_ALL) != 0 )
	{
		const CExtGridHitTestInfo & htInfoPressing = OnGridQueryPressingCellInfo();
		if(		( ! htInfoPressing.IsHoverEmpty() )
			&&	(	( htInfoPressing.m_nColNo == nColNo && nColType == htInfoPressing.GetInnerOuterTypeOfColumn() )
				||	( htInfoPressing.m_nRowNo == nRowNo && nRowType == htInfoPressing.GetInnerOuterTypeOfRow() )
				)
			)
		{
			if( ( dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0 )
			{
				if(		htInfoPressing.m_nColNo == nColNo
					&&	nColType == 0
					&&	htInfoPressing.GetInnerOuterTypeOfColumn() == 0
					&&	(dwBseStyleEx&__EGBS_BSE_EX_HIGHLIGHT_PRESSING_COLUMNS_OUTER) != 0
					)
					dwHelperPaintFlags |= __EGCPF_HIGHLIGHTED_BY_PRESSED_COLUMN;
				if(	htInfoPressing.m_nRowNo == nRowNo
					&&	nRowType == 0
					&&	htInfoPressing.GetInnerOuterTypeOfRow() == 0
					&&	(dwBseStyleEx&__EGBS_BSE_EX_HIGHLIGHT_PRESSING_ROWS_OUTER) != 0
					)
					dwHelperPaintFlags |= __EGCPF_HIGHLIGHTED_BY_PRESSED_ROW;
			} // if( ( dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0 )
			else
			{
				if(		htInfoPressing.m_nColNo == nColNo
					&&	nColType == 0
					&&	htInfoPressing.GetInnerOuterTypeOfColumn() == 0
					&&	(dwBseStyleEx&__EGBS_BSE_EX_HIGHLIGHT_PRESSING_COLUMNS_INNER) != 0
					)
					dwHelperPaintFlags |= __EGCPF_HIGHLIGHTED_BY_PRESSED_COLUMN;
				if(	htInfoPressing.m_nRowNo == nRowNo
					&&	nRowType == 0
					&&	htInfoPressing.GetInnerOuterTypeOfRow() == 0
					&&	(dwBseStyleEx&__EGBS_BSE_EX_HIGHLIGHT_PRESSING_ROWS_INNER) != 0
					)
					dwHelperPaintFlags |= __EGCPF_HIGHLIGHTED_BY_PRESSED_ROW;
			} // else from if( ( dwAreaFlags & __EGBWA_OUTER_CELLS ) != 0 )
		} // if( ! htInfoPressing.IsHoverEmpty() ...
	} // if( (dwBseStyleEx&__EGBS_BSE_EX_HIGHLIGHT_PRESSING_ALL) != 0 )
const CExtGridCell * pCell =
		GridCellGet( nColNo, nRowNo, nColType, nRowType );
	if( pCell == NULL )
	{
		CExtGridBaseWnd::OnGbwPaintCell(
			dc,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			rcCellExtra,
			rcCell,
			rcVisibleRange,
			dwAreaFlags,
			dwHelperPaintFlags
			);
		return;
	} // if( pCell == NULL )
	if( ! pCell->OnPaintBackground(
			*this,
			dc,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			nColType,
			nRowType,
			rcCellExtra,
			rcCell,
			rcVisibleRange,
			dwAreaFlags,
			dwHelperPaintFlags
			)
		)
		CExtGridBaseWnd::OnGbwPaintCell(
			dc,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			rcCellExtra,
			rcCell,
			rcVisibleRange,
			dwAreaFlags,
			dwHelperPaintFlags
			);
	pCell->OnPaintForeground(
		*this,
		dc,
		nVisibleColNo,
		nVisibleRowNo,
		nColNo,
		nRowNo,
		nColType,
		nRowType,
		rcCellExtra,
		rcCell,
		rcVisibleRange,
		dwAreaFlags,
		dwHelperPaintFlags
		);
}

void CExtGridWnd::OnGbwHoverCell(
	const CExtGridHitTestInfo & htInfoPrevious,
	const CExtGridHitTestInfo & htInfoCurrent,
	bool bHoverEnter,
	bool bHoverLeave
	)
{
	ASSERT_VALID( this );
	ASSERT( ! (bHoverEnter && bHoverLeave) );
	// CExtGridBaseWnd::OnGbwHoverCell() performs region
	// invalidation for highligting rows/columns
	CExtGridBaseWnd::OnGbwHoverCell(
		htInfoPrevious,
		htInfoCurrent,
		bHoverEnter,
		bHoverLeave
		);
DWORD dwSiwStyleEx = SiwGetStyleEx();
	if( ( dwSiwStyleEx & (__EGBS_EX_HVI_EVENT_CELLS|__EGBS_EX_HVO_EVENT_CELLS) ) != 0 )
	{
		bool bHoverEmptyPrevious = htInfoPrevious.IsHoverEmpty();
		bool bHoverEmptyCurrent = htInfoCurrent.IsHoverEmpty();
		if(		htInfoPrevious.IsHoverEqual( htInfoCurrent )
			&&	( (!bHoverEnter) && (!bHoverLeave) )
			)
		{
			// hover is on the same cell
			ASSERT( !bHoverEmptyCurrent );
			ASSERT( (!bHoverEnter) && (!bHoverLeave) );
			INT nColType =
				htInfoCurrent.GetInnerOuterTypeOfColumn();
			INT nRowType =
				htInfoCurrent.GetInnerOuterTypeOfRow();
			CExtGridCell * pCell =
				GridCellGet(
					htInfoCurrent.m_nColNo,
					htInfoCurrent.m_nRowNo,
					nColType,
					nRowType
					);
			if( pCell != NULL )
			{
				ASSERT_VALID( pCell );
				ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
				//ASSERT( pCell->DataProviderGet() == (&OnGridQueryDataProvider()) );
				ASSERT_VALID( pCell->DataProviderGet() );
				pCell->OnHover(
					*this,
					htInfoCurrent,
					false,
					false
					);
			} // if( pCell != NULL )
		} // if( htInfoPrevious.IsHoverEqual( htInfoCurrent ) ...
		else
		{
			if(		bHoverLeave
				&&	( ! bHoverEmptyPrevious )
				&&	htInfoPrevious.IsValidRect()
				)
			{ // notify old cell about lost hover
				ASSERT( !bHoverEnter );
				INT nColType =
					htInfoPrevious.GetInnerOuterTypeOfColumn();
				INT nRowType =
					htInfoPrevious.GetInnerOuterTypeOfRow();
				CExtGridCell * pCell =
					GridCellGet(
						htInfoPrevious.m_nColNo,
						htInfoPrevious.m_nRowNo,
						nColType,
						nRowType
						);
				if( pCell != NULL )
				{
					ASSERT_VALID( pCell );
					ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
					//ASSERT( pCell->DataProviderGet() == (&OnGridQueryDataProvider()) );
					ASSERT_VALID( pCell->DataProviderGet() );
					pCell->OnHover(
						*this,
						htInfoPrevious,
						false,
						true
						);
				} // if( pCell != NULL )
			} // notify old cell about lost hover
			if(		bHoverEnter
				&&	( ! bHoverEmptyCurrent )
				&&	htInfoCurrent.IsValidRect()
				)
			{ // notify new cell about gained hover
				ASSERT( !bHoverLeave );
				INT nColType =
					htInfoCurrent.GetInnerOuterTypeOfColumn();
				INT nRowType =
					htInfoCurrent.GetInnerOuterTypeOfRow();
				CExtGridCell * pCell =
					GridCellGet(
						htInfoCurrent.m_nColNo,
						htInfoCurrent.m_nRowNo,
						nColType,
						nRowType
						);
				if( pCell != NULL )
				{
					ASSERT_VALID( pCell );
					ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
					//ASSERT( pCell->DataProviderGet() == (&OnGridQueryDataProvider()) );
					ASSERT_VALID( pCell->DataProviderGet() );
					pCell->OnHover(
						*this,
						htInfoCurrent,
						true,
						false
						);
				} // if( pCell != NULL )
			} // notify new cell about gained hover
		} // else from if( htInfoPrevious.IsHoverEqual( htInfoCurrent ) ...
	} // if( ( dwSiwStyleEx & (__EGBS_EX_HVI_EVENT_CELLS|__EGBS_EX_HVO_EVENT_CELLS) ) != 0 )
}

CExtGridHitTestInfo & CExtGridWnd::HitTest(
	CExtGridHitTestInfo & htInfo,
	bool bReAlignCellResizing,
	bool bSupressZeroTopCellResizing,
	bool bComputeOuterDropAfterState // = false
	) const
{
	if(	CExtGridBaseWnd::HitTest(
			htInfo,
			bReAlignCellResizing,
			bSupressZeroTopCellResizing,
			bComputeOuterDropAfterState
			).IsHoverEmpty()
		)
		return htInfo;
INT nColType = htInfo.GetInnerOuterTypeOfColumn();
INT nRowType = htInfo.GetInnerOuterTypeOfRow();
const CExtGridCell * pCell =
		GridCellGet(
			htInfo.m_nColNo,
			htInfo.m_nRowNo,
			nColType,
			nRowType
			);
	if( pCell != NULL )
	{
		ASSERT_VALID( pCell );
		ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
		//ASSERT( pCell->DataProviderGet() == (&OnGridQueryDataProvider()) );
		ASSERT_VALID( pCell->DataProviderGet() );
		pCell->OnHitTestInfoAdjust(
			*this,
			htInfo,
			bComputeOuterDropAfterState
			);
	} // if( pCell != NULL )
	return htInfo;
}

bool CExtGridWnd::OnGbwAnalyzeCellMouseClickEvent(
	UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
	UINT nRepCnt, // 0 - button up, 1 - signle click, 2 - double click, 3 - post single click & begin editing
	UINT nFlags, // mouse event flags
	CPoint point // mouse pointer in client coordinates
	)
{
	ASSERT_VALID( this );
	ASSERT( 0 <= nRepCnt && nRepCnt <= 3 );
	if( CExtGridBaseWnd::OnGbwAnalyzeCellMouseClickEvent(
			nChar,
			nRepCnt,
			nFlags,
			point
			)
		)
		return true;
CExtGridHitTestInfo htInfo( point );
	HitTest( htInfo, false, true );
	if(		htInfo.IsHoverEmpty()
		||	( ! htInfo.IsValidRect() )
		)
		return false;
INT nColType = htInfo.GetInnerOuterTypeOfColumn();
INT nRowType = htInfo.GetInnerOuterTypeOfRow();
CExtGridCell * pCell =
		GridCellGet(
			htInfo.m_nColNo,
			htInfo.m_nRowNo,
			nColType,
			nRowType
			);
	if( pCell != NULL )
	{
		ASSERT_VALID( pCell );
		ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
		//ASSERT( pCell->DataProviderGet() == (&OnGridQueryDataProvider()) );
		ASSERT_VALID( pCell->DataProviderGet() );
		bool bRetVal =
			pCell->OnClick(
				*this,
				htInfo,
				nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
				nRepCnt, // 0 - button up, 1 - signle click, 2 - double click, 3 - post single click & begin editing
				nFlags // mouse event flags
				);
		if( bRetVal )
			return true;
	} // if( pCell != NULL )
	
DWORD dwBseStyle = BseGetStyle();
	if(		nChar == VK_LBUTTON
		&&	(	( nRepCnt == 2 && (dwBseStyle&__EGWS_BSE_EDIT_DOUBLE_LCLICK) != 0 )
			||	( nRepCnt == 3 && (dwBseStyle&__EGWS_BSE_EDIT_SINGLE_LCLICK) != 0 )
			)
		)
	{ // try to activate inplace editor etc...
		CExtGridHitTestInfo htInfo( point );
		HitTest( htInfo, false, true );
		if( ( ! htInfo.IsHoverEmpty() ) && htInfo.IsValidRect() )
		{
			if( (htInfo.m_dwAreaFlags&__EGBWA_CELL_IMAGE) != 0 )
				return false;
			if( (htInfo.m_dwAreaFlags&__EGBWA_CELL_CHECKBOX) != 0 )
				return false;
			if( (htInfo.m_dwAreaFlags&__EGBWA_CELL_BUTTON) != 0 )
				return false;
			if( (htInfo.m_dwAreaFlags&__EGBWA_CELL_SORT_ARROW) != 0 )
				return false;
			if( (htInfo.m_dwAreaFlags&__EGBWA_CELL_FOCUS_ARROW) != 0 )
				return false;
			if( (htInfo.m_dwAreaFlags&(__EGBWA_CELL_TEXT|__EGBWA_CELL_TEXT_AREA)) != 0 )
			{
				INT nColType = htInfo.GetInnerOuterTypeOfColumn();
				INT nRowType = htInfo.GetInnerOuterTypeOfRow();
				if( nRepCnt == 3 && (dwBseStyle&__EGWS_BSE_EDIT_SINGLE_FOCUSED_ONLY) != 0 )
				{
					CPoint ptFocus = FocusGet();
					DWORD dwBasicSelectionType = SiwGetStyle() & __EGBS_SFB_MASK;
					switch( dwBasicSelectionType )
					{
					case __EGBS_SFB_NONE:
					break;
					case __EGBS_SFB_CELLS:
						if( (htInfo.m_dwAreaFlags&__EGBWA_INNER_CELLS) != 0 )
						{
							if( htInfo.m_nColNo != ptFocus.x || htInfo.m_nRowNo != ptFocus.y )
								return false;
						} // if( (htInfo.m_dwAreaFlags&__EGBWA_INNER_CELLS) != 0 )
						else if( (htInfo.m_dwAreaFlags&__EGBWA_OUTER_SIDE_MASK) != 0 )
						{
							if( nColType != 0 || nRowType != 0 )
								return false;
						} // else if( (htInfo.m_dwAreaFlags&__EGBWA_OUTER_SIDE_MASK) != 0 )
						else
							return false;
					break;
					case __EGBS_SFB_FULL_ROWS:
						if( htInfo.m_nRowNo != ptFocus.y )
							return false;
						if( nColType != 0 )
							return false;
					break;
					case __EGBS_SFB_FULL_COLUMNS:
						if( htInfo.m_nColNo != ptFocus.x )
							return false;
						if( nRowType != 0 )
							return false;
					break;
#ifdef _DEBUG
					default:
						ASSERT( FALSE );
					break;
#endif // _DEBUG
					} // switch( dwBasicSelectionType )
				} // if( nRepCnt == 3 && (dwBseStyle&__EGWS_BSE_EDIT_SINGLE_FOCUSED_ONLY) != 0 )
				else if( nRepCnt == 2 )
				{
					bool bAlt = ( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 ) ? true : false;
					bool bCtrl = ( (::GetAsyncKeyState(VK_CONTROL)&0x8000) != 0 ) ? true : false;
					bool bShift = ( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 ) ? true : false;
					bool bNoModifiers = (!bCtrl) && (!bAlt) && (!bShift);
					if( !bNoModifiers )
						return false;
				} // else if( nRepCnt == 2 )
				if( nColType < 0 && (dwBseStyle&__EGWS_BSE_EDIT_CELLS_OUTER_L) == 0L )
					return false;
				else if( nColType > 0 && (dwBseStyle&__EGWS_BSE_EDIT_CELLS_OUTER_R) == 0L )
					return false;
				else if( nRowType < 0 && (dwBseStyle&__EGWS_BSE_EDIT_CELLS_OUTER_T) == 0L )
					return false;
				else if( nRowType > 0 && (dwBseStyle&__EGWS_BSE_EDIT_CELLS_OUTER_B) == 0L )
					return false;
				return
					OnGbwBeginEdit(
						htInfo.m_nVisibleColNo,
						htInfo.m_nVisibleRowNo,
						htInfo.m_nColNo,
						htInfo.m_nRowNo,
						nColType,
						nRowType,
						htInfo.m_rcExtra,
						htInfo.m_rcItem,
						CRect(0,0,0,0) // htInfo.m_rcPart
						);
			}
		} // if( ( ! htInfo.IsHoverEmpty() ) && htInfo.IsValidRect() )
	} // try to activate inplace editor etc...
	return false;
}

bool CExtGridWnd::OnGbwAnalyzeCellKeyEvent(
	bool bKeyDownEvent, // true - key-down event, false - key-up event
	UINT nChar, // virtual key code
	UINT nRepCnt, // key-down/key-up press count
	UINT nFlags // key-down/key-up event flags
	)
{
	ASSERT_VALID( this );
	if( CExtGridBaseWnd::OnGbwAnalyzeCellKeyEvent(
			bKeyDownEvent,
			nChar,
			nRepCnt,
			nFlags
			)
		)
		return true;
CPoint ptFocus = FocusGet();
	if( ptFocus.x < 0 || ptFocus.y < 0 )
		return false;
CExtGridCell * pCell = GridCellGet( ptFocus.x, ptFocus.y );
	if( pCell == NULL )
		return false;
	ASSERT_VALID( pCell );
	ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
	//ASSERT( pCell->DataProviderGet() == (&OnGridQueryDataProvider()) );
	ASSERT_VALID( pCell->DataProviderGet() );
bool bRetVal =
		pCell->OnKey(
			*this,
			bKeyDownEvent, // true - key-down event, false - key-up event
			nChar, // virtual key code
			nRepCnt, // key-down/key-up press count
			nFlags // key-down/key-up event flags
			);
	return bRetVal;
}

bool CExtGridWnd::OnGbwSetCursor(
	const CExtGridHitTestInfo & htInfo
	)
{
	ASSERT_VALID( this );
	if(		// m_eMTT == __EMTT_NOTHING &&
			( ! htInfo.IsHoverEmpty() )
		&&	htInfo.IsValidRect()
		)
	{
		INT nColType = htInfo.GetInnerOuterTypeOfColumn();
		INT nRowType = htInfo.GetInnerOuterTypeOfRow();
		CExtGridCell * pCell =
			GridCellGet(
				htInfo.m_nColNo,
				htInfo.m_nRowNo,
				nColType,
				nRowType
				);
		if( pCell != NULL )
		{
			ASSERT_VALID( pCell );
			ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
			//ASSERT( pCell->DataProviderGet() == (&OnGridQueryDataProvider()) );
			ASSERT_VALID( pCell->DataProviderGet() );
			if( pCell->OnSetCursor(
					*this,
					htInfo
					)
				)
				return true;
		} // if( pCell != NULL )
	} // if( m_eMTT == __EMTT_NOTHING ...
	return CExtGridBaseWnd::OnGbwSetCursor( htInfo );
}

bool CExtGridWnd::OnSiwCacheChanged(
	const CExtScrollItemCacheInfo & _sciNew,
	const CExtScrollItemCacheInfo & _sciOld
	)
{
	ASSERT_VALID( this );
	ASSERT( _sciNew.IsValid() );
	ASSERT( _sciOld.IsValid() );

DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();

#ifdef _DEBUG
	ASSERT(
	   CExtGridBaseWnd::OnSiwCacheChanged(
			_sciNew,
			_sciOld
			)
		);
	if( dwScrollTypeH == __ESIW_ST_ITEM )
	{
		LONG nColFirst, nColLast, nColFirstOffset;
		OnGbwQueryVisibleColumnRange(
			nColFirst,
			nColLast,
			nColFirstOffset
			);
		ASSERT( nColFirst >= 0 );
		if( nColLast >= 0 )
		{
			INT nPageSizeH = nColLast - nColFirst; // + 1;
			ASSERT(
					_sciNew.m_nPageSizeH == nPageSizeH
				||	_sciNew.m_nPageSizeH == (nPageSizeH+1)
				||	_sciNew.m_nPageSizeH == (nPageSizeH+2)
				);
		} // if( nColLast >= 0 )
	} // if( dwScrollTypeH == __ESIW_ST_ITEM )
	if( dwScrollTypeV == __ESIW_ST_ITEM )
	{
		LONG nRowFirst, nRowLast, nRowFirstOffset;
		OnGbwQueryVisibleRowRange(
			nRowFirst,
			nRowLast,
			nRowFirstOffset
			);
		ASSERT( nRowFirst >= 0 );
		if( nRowLast >= 0 )
		{
			INT nPageSizeV = nRowLast - nRowFirst; // + 1;
			ASSERT(
					_sciNew.m_nPageSizeV == nPageSizeV
				||	_sciNew.m_nPageSizeV == (nPageSizeV+1)
				||	_sciNew.m_nPageSizeV == (nPageSizeV+2)
				);
		} // if( nRowLast >= 0 )
	} // if( dwScrollTypeV == __ESIW_ST_ITEM )
#endif // _DEBUG

CExtScrollItemCacheInfo _sciNewByItem( _sciNew );
CExtScrollItemCacheInfo _sciOldByItem( _sciOld );
	if(		dwScrollTypeH == __ESIW_ST_NONE
		||	dwScrollTypeH == __ESIW_ST_PIXEL
		)
	{
		_sciNewByItem.EmptyH();
		_sciOldByItem.EmptyH();
	}
	if(		dwScrollTypeV == __ESIW_ST_NONE
		||	dwScrollTypeV == __ESIW_ST_PIXEL
		)
	{
		_sciNewByItem.EmptyV();
		_sciOldByItem.EmptyV();
	}

	if( m_bLockVirtualCacheUpdateH )
	{
		ASSERT( dwScrollTypeH == __ESIW_ST_VIRTUAL );
		_sciNewByItem.AssignH( _sciOldByItem );
	} // if( m_bLockVirtualCacheUpdateH )
	if( m_bLockVirtualCacheUpdateV )
	{
		ASSERT( dwScrollTypeV == __ESIW_ST_VIRTUAL );
		_sciNewByItem.AssignV( _sciOldByItem );
	} // if( m_bLockVirtualCacheUpdateV )

CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
bool bRetVal =
		_DataProvider.CacheData(
			_sciNewByItem,
			_sciOldByItem
			);
	return bRetVal;
}

bool CExtGridWnd::OnSiwQueryVirtualBOF( bool bHorz ) const
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
const CExtGridDataProvider & _DataProvider =
		OnGridQueryDataProvider();
	if( ! _DataProvider.CacheIsVisibleFirstRecord( bHorz ) )
		return false;
	return true;
}

bool CExtGridWnd::OnSiwQueryVirtualEOF( bool bHorz ) const
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
const CExtGridDataProvider & _DataProvider =
		OnGridQueryDataProvider();
	if( ! _DataProvider.CacheIsVisibleLastRecord( bHorz ) )
		return false;
CRect rcClient = OnSwGetClientRect();
CExtGridHitTestInfo htInfo(
		bHorz
			? CPoint( rcClient.right-1, rcClient.top+1 )
			: CPoint( rcClient.left+1, rcClient.bottom-1 )
			);
	HitTest( htInfo, false, true );
	if(		(htInfo.m_dwAreaFlags & __EGBWA_INNER_CELLS) != 0
		&&	(! htInfo.m_rcItem.IsRectEmpty() )
		)
	{
		if( bHorz )
		{
			ASSERT( htInfo.m_rcItem.left < rcClient.right );
			if( htInfo.m_rcItem.right > rcClient.right )
				return false;
		} // if( bHorz )
		else
		{
			ASSERT( htInfo.m_rcItem.top < rcClient.bottom );
			if( htInfo.m_rcItem.bottom > rcClient.bottom )
				return false;
		} // else from if( bHorz )
	}
	return true;
}

bool CExtGridWnd::OnSiwVirtualEndTestH( LONG nColNo, LPARAM lParam ) const
{
	ASSERT_VALID( this );
	ASSERT( nColNo >= 0L );
DWORD dwScrollTypeH = SiwScrollTypeHGet();
	if( dwScrollTypeH != __ESIW_ST_VIRTUAL )
		return false;
INT nColType =
		CExtGridHitTestInfo::GetInnerOuterTypeOfColumn(
			DWORD(lParam) //dwAreaFlags
			);
	if( nColType != 0 )
		return false;
const CExtGridDataProvider & _DataProvider =
		OnGridQueryDataProvider();
LONG nCacheSizeX = (LONG)_DataProvider.CacheColumnCountGet();
	ASSERT( nCacheSizeX >= 0 );
LONG nReservedSizeX =
		OuterColumnCountLeftGet() + OuterColumnCountRightGet();
	ASSERT( nReservedSizeX >= 0L );
	ASSERT( nCacheSizeX >= nReservedSizeX );
	nCacheSizeX -= nReservedSizeX;
	if( nColNo >= nCacheSizeX )
		return true;
	return false;
}

bool CExtGridWnd::OnSiwVirtualEndTestV( LONG nRowNo, LPARAM lParam ) const
{
	ASSERT_VALID( this );
	ASSERT( nRowNo >= 0L );
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	if( dwScrollTypeV != __ESIW_ST_VIRTUAL )
		return false;
INT nRowType =
		CExtGridHitTestInfo::GetInnerOuterTypeOfRow(
			DWORD(lParam) //dwAreaFlags
			);
	if( nRowType != 0 )
		return false;
const CExtGridDataProvider & _DataProvider =
		OnGridQueryDataProvider();
LONG nCacheSizeY = (LONG)_DataProvider.CacheRowCountGet();
	ASSERT( nCacheSizeY >= 0 );
LONG nReservedSizeY =
		OuterRowCountTopGet() + OuterRowCountBottomGet();
	ASSERT( nReservedSizeY >= 0L );
	ASSERT( nCacheSizeY >= nReservedSizeY );
	nCacheSizeY -= nReservedSizeY;
	if( nRowNo >= nCacheSizeY )
		return true;
	return false;
}

void CExtGridWnd::OnGbwResizingStateApply(
	bool bHorz,
	LONG nItemNo,
	INT nItemExtent
	)
{
	ASSERT_VALID( this );
CExtGridCell * pCell = NULL;
#ifdef _DEBUG
	ASSERT( nItemNo >= 0 );
	ASSERT( nItemExtent >= 0 );
INT nDebugVerifyTrackExtentMin = 0, nDebugVerifyTrackExtentMax = 0;
#endif // _DEBUG
	if( bHorz )
	{
#ifdef _DEBUG
		ASSERT( nItemNo < ColumnCountGet() );
		VERIFY(
			OnGbwCanResizeColumn(
				nItemNo,
				&nDebugVerifyTrackExtentMin,
				&nDebugVerifyTrackExtentMax
				)
			);
		ASSERT(
				nDebugVerifyTrackExtentMin < nDebugVerifyTrackExtentMax
			&&	nDebugVerifyTrackExtentMin <= nItemExtent
			&&	nItemExtent <= nDebugVerifyTrackExtentMax
			);
#endif // _DEBUG
		if( FixedSizeColumnsGet() )
		{
			DefaultColumnWidthSet( nItemExtent, false );
			OnSwDoRedraw();
			OnSwUpdateScrollBars();
			return;
		} // if( FixedSizeColumnsGet() )
		LONG nRowCount = OuterRowCountTopGet();
		ASSERT( nRowCount >= 0L );
		INT nType = -1;
		if( nRowCount == 0L )
		{
			nRowCount = OuterRowCountBottomGet();
			ASSERT( nRowCount >= 0L );
			nType = 1;
			if( nRowCount == 0L )
			{
				nRowCount = RowCountGet();
				ASSERT( nRowCount >= 0L );
				if( nRowCount == 0L )
					return;
				nType = 0;
			} // if( nRowCount == 0L )
		} // if( nRowCount == 0L )
		pCell = GridCellGet( nItemNo, 0L, 0, nType );
		if( pCell == NULL )
			return;
	} // if( bHorz )
	else
	{
#ifdef _DEBUG
		ASSERT( nItemNo < RowCountGet() );
		VERIFY(
			OnGbwCanResizeRow(
				nItemNo,
				&nDebugVerifyTrackExtentMin,
				&nDebugVerifyTrackExtentMax
				)
			);
		ASSERT(
				nDebugVerifyTrackExtentMin < nDebugVerifyTrackExtentMax
			&&	nDebugVerifyTrackExtentMin <= nItemExtent
			&&	nItemExtent <= nDebugVerifyTrackExtentMax
			);
#endif // _DEBUG
		if( FixedSizeRowsGet() )
		{
			DefaultRowHeightSet( nItemExtent, false );
			OnSwDoRedraw();
			OnSwUpdateScrollBars();
			return;
		} // if( FixedSizeRowsGet() )
		LONG nColCount = OuterColumnCountLeftGet();
		ASSERT( nColCount >= 0L );
		INT nType = -1;
		if( nColCount == 0L )
		{
			nColCount = OuterColumnCountRightGet();
			ASSERT( nColCount >= 0L );
			nType = 1;
			if( nColCount == 0L )
			{
				nColCount = ColumnCountGet();
				ASSERT( nColCount >= 0L );
				if( nColCount == 0L )
					return;
				nType = 0;
			} // if( nColCount == 0L )
		} // if( nColCount == 0L )
		pCell = GridCellGet( 0L, nItemNo, nType, 0 );
		if( pCell == NULL )
			return;
	} // else from if( bHorz )
	ASSERT_VALID( pCell );
	if( ! pCell->ExtentSet( nItemExtent, 0 ) )
		return;
	OnSwDoRedraw();
	OnSwUpdateScrollBars();
}

INT CExtGridWnd::OnSiwQueryItemExtentH(
	LONG nColNo,
	INT * p_nExtraSpaceBefore, // = NULL
	INT * p_nExtraSpaceAfter // = NULL
	) const
{
	ASSERT_VALID( this );
	ASSERT( nColNo >= 0 );
	if( p_nExtraSpaceBefore != NULL )
		(*p_nExtraSpaceBefore) = 0;
	if( p_nExtraSpaceAfter != NULL )
		(*p_nExtraSpaceAfter) = 0;
	if( FixedSizeColumnsGet() )
		return DefaultColumnWidthGet();
LONG nRowCount = OuterRowCountTopGet();
	ASSERT( nRowCount >= 0L );
INT nType = -1;
	if( nRowCount == 0L )
	{
		nRowCount = OuterRowCountBottomGet();
		ASSERT( nRowCount >= 0L );
		nType = 1;
		if( nRowCount == 0L )
		{
			nRowCount = RowCountGet();
			ASSERT( nRowCount >= 0L );
			nType = 0;
		} // if( nRowCount == 0L )
	} // if( nRowCount == 0L )
	if( nRowCount >= 0L )
	{
		const CExtGridCell * pCell =
			GridCellGet( nColNo, 0L, 0, nType );
		if( pCell != NULL )
		{
			ASSERT_VALID( pCell );
			INT nItemExtent, nExtraSpaceAfter = 0, nExtraSpaceBefore = 0;
			if( pCell->ExtentGet( nItemExtent, 0 ) )
			{
				pCell->ExtraSpaceGet( nExtraSpaceAfter, true );
				pCell->ExtraSpaceGet( nExtraSpaceBefore, false );
				ASSERT( nItemExtent >= 0 );
				ASSERT( nExtraSpaceAfter >= 0 );
				ASSERT( nExtraSpaceBefore >= 0 );
				if( p_nExtraSpaceBefore != NULL )
					(*p_nExtraSpaceBefore) = nExtraSpaceBefore;
				if( p_nExtraSpaceAfter != NULL )
					(*p_nExtraSpaceAfter) = nExtraSpaceAfter;
				return (nItemExtent+nExtraSpaceAfter+nExtraSpaceBefore);
			} // if( pCell->ExtentGet( nItemExtent, 0 ) )
		} // if( pCell != NULL )
	} // if( nRowCount >= 0L )
	return
		CExtGridBaseWnd::OnSiwQueryItemExtentH(
			nColNo,
			p_nExtraSpaceBefore,
			p_nExtraSpaceAfter
			);
}

INT CExtGridWnd::OnSiwQueryItemExtentV(
	LONG nRowNo,
	INT * p_nExtraSpaceBefore, // = NULL
	INT * p_nExtraSpaceAfter // = NULL
	) const
{
	ASSERT_VALID( this );
	ASSERT( nRowNo >= 0 );
	if( p_nExtraSpaceBefore != NULL )
		(*p_nExtraSpaceBefore) = 0;
	if( p_nExtraSpaceAfter != NULL )
		(*p_nExtraSpaceAfter) = 0;
	if( FixedSizeRowsGet() )
		return DefaultRowHeightGet();
LONG nColCount = OuterColumnCountLeftGet();
	ASSERT( nColCount >= 0L );
INT nType = -1;
	if( nColCount == 0L )
	{
		nColCount = OuterColumnCountRightGet();
		ASSERT( nColCount >= 0L );
		nType = 1;
		if( nColCount == 0L )
		{
			nColCount = ColumnCountGet();
			ASSERT( nColCount >= 0L );
			nType = 0;
		} // if( nColCount == 0L )
	} // if( nColCount == 0L )
	if( nColCount >= 0L )
	{
		const CExtGridCell * pCell =
			GridCellGet( 0L, nRowNo, nType, 0 );
		if( pCell != NULL )
		{
			ASSERT_VALID( pCell );
			INT nItemExtent, nExtraSpaceAfter = 0, nExtraSpaceBefore = 0;
			if( pCell->ExtentGet( nItemExtent, 0 ) )
			{
				pCell->ExtraSpaceGet( nExtraSpaceAfter, true );
				pCell->ExtraSpaceGet( nExtraSpaceBefore, false );
				ASSERT( nItemExtent >= 0 );
				ASSERT( nExtraSpaceAfter >= 0 );
				ASSERT( nExtraSpaceBefore >= 0 );
				if( p_nExtraSpaceBefore != NULL )
					(*p_nExtraSpaceBefore) = nExtraSpaceBefore;
				if( p_nExtraSpaceAfter != NULL )
					(*p_nExtraSpaceAfter) = nExtraSpaceAfter;
				return (nItemExtent+nExtraSpaceAfter+nExtraSpaceBefore);
			} // if( pCell->ExtentGet( nItemExtent, 0 ) )
		} // if( pCell != NULL )
	} // if( nColCount >= 0L )
	return
		CExtGridBaseWnd::OnSiwQueryItemExtentV(
			nRowNo,
			p_nExtraSpaceBefore,
			p_nExtraSpaceAfter
			);
}

bool CExtGridWnd::OnGbwCanResizeColumn(
	LONG nColNo,
	INT * p_nExtentMin, //  = NULL
	INT * p_nExtentMax //  = NULL
	)
{
	ASSERT_VALID( this );
	if( FixedSizeColumnsGet() )
	{
		if( p_nExtentMin != NULL )
			(*p_nExtentMin) = 2;
		if( p_nExtentMax != NULL )
			(*p_nExtentMax) = 32767;
		return true;
	} // if( FixedSizeColumnsGet() )
INT nExtentMin = 0, nExtentMax = 32767;
	ASSERT( nColNo >= 0 );
LONG nRowCount = OuterRowCountTopGet();
	ASSERT( nRowCount >= 0L );
INT nType = -1;
	if( nRowCount == 0L )
	{
		nRowCount = OuterRowCountBottomGet();
		ASSERT( nRowCount >= 0L );
		nType = 1;
		if( nRowCount == 0L )
		{
			nRowCount = RowCountGet();
			ASSERT( nRowCount >= 0L );
			nType = 0;
		} // if( nRowCount == 0L )
	} // if( nRowCount == 0L )
	if( nRowCount >= 0L )
	{
		const CExtGridCell * pCell =
			GridCellGet( nColNo, 0L, 0, nType );
		if( pCell != NULL )
		{
			ASSERT_VALID( pCell );
			if( pCell->ExtentGet( nExtentMin, -1 ) )
			{
				VERIFY( pCell->ExtentGet( nExtentMax, 1 ) );
			}
		} // if( pCell != NULL )
	} // if( nRowCount >= 0L )
	ASSERT(
			nExtentMin >=  0
		&&	nExtentMax >= 0
		&&	nExtentMin <= nExtentMax
		);
	if( p_nExtentMin != NULL )
		(*p_nExtentMin) = nExtentMin;
	if( p_nExtentMax != NULL )
		(*p_nExtentMax) = nExtentMax;
bool bRetVal = (nExtentMin != nExtentMax) ? true : false;
	return bRetVal;
}

bool CExtGridWnd::OnGbwCanResizeRow(
	LONG nRowNo,
	INT * p_nExtentMin, //  = NULL
	INT * p_nExtentMax //  = NULL
	)
{
	ASSERT_VALID( this );
	if( FixedSizeRowsGet() )
	{
		if( p_nExtentMin != NULL )
			(*p_nExtentMin) = 2;
		if( p_nExtentMax != NULL )
			(*p_nExtentMax) = 32767;
		return true;
	} // if( FixedSizeRowsGet() )
INT nExtentMin = 0, nExtentMax = 32767;
	ASSERT( nRowNo >= 0 );
LONG nColCount = OuterColumnCountLeftGet();
	ASSERT( nColCount >= 0L );
INT nType = -1;
	if( nColCount == 0L )
	{
		nColCount = OuterColumnCountRightGet();
		ASSERT( nColCount >= 0L );
		nType = 1;
		if( nColCount == 0L )
		{
			nColCount = ColumnCountGet();
			ASSERT( nColCount >= 0L );
			nType = 0;
		} // if( nColCount == 0L )
	} // if( nColCount == 0L )
	if( nColCount >= 0L )
	{
		const CExtGridCell * pCell =
			GridCellGet( 0L, nRowNo, nType, 0 );
		if( pCell != NULL )
		{
			ASSERT_VALID( pCell );
			if( pCell->ExtentGet( nExtentMin, -1 ) )
			{
				VERIFY( pCell->ExtentGet( nExtentMax, 1 ) );
			}
		} // if( pCell != NULL )
	} // if( nColCount >= 0L )
	ASSERT(
			nExtentMin >=  0
		&&	nExtentMax >= 0
		&&	nExtentMin <= nExtentMax
		);
	if( p_nExtentMin != NULL )
		(*p_nExtentMin) = nExtentMin;
	if( p_nExtentMax != NULL )
		(*p_nExtentMax) = nExtentMax;
bool bRetVal = (nExtentMin != nExtentMax) ? true : false;
	return bRetVal;
}

bool CExtGridWnd::OnGbwTooltip(
	const CExtGridHitTestInfo & htInfo
	)
{
	ASSERT_VALID( this );
	ASSERT( ! htInfo.IsHoverEmpty() );
	if(		m_wndToolTip.GetSafeHwnd() == NULL
		||	( ! ::IsWindow( m_wndToolTip.GetSafeHwnd() ) )
		)
		return false;
INT nColType = htInfo.GetInnerOuterTypeOfColumn();
INT nRowType = htInfo.GetInnerOuterTypeOfRow();
CExtGridCell * pCell =
		GridCellGet(
			htInfo.m_nColNo,
			htInfo.m_nRowNo,
			nColType,
			nRowType
			);
	if( pCell == NULL )
		return false;
	ASSERT_VALID( pCell );
	ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
	//ASSERT( pCell->DataProviderGet() == (&OnGridQueryDataProvider()) );
	ASSERT_VALID( pCell->DataProviderGet() );
bool bRetVal =
		pCell->OnInitToolTip(
			*this,
			htInfo,
			m_wndToolTip,
			1,
			htInfo.m_rcItem
			);
	return bRetVal;
}

bool CExtGridWnd::OnGbwExpand(
	const CExtGridHitTestInfo & htInfo,
	UINT nDelayMilliseconds
	)
{
	ASSERT_VALID( this );
	ASSERT( ! htInfo.IsHoverEmpty() );
	if( nDelayMilliseconds != 0 )
		return
			CExtGridBaseWnd::OnGbwExpand(
				htInfo,
				nDelayMilliseconds
				);
INT nColType = htInfo.GetInnerOuterTypeOfColumn();
INT nRowType = htInfo.GetInnerOuterTypeOfRow();
CExtGridCell * pCell =
		GridCellGet(
			htInfo.m_nColNo,
			htInfo.m_nRowNo,
			nColType,
			nRowType
			);
	if( pCell == NULL )
		return false;
	ASSERT_VALID( pCell );
	ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
	//ASSERT( pCell->DataProviderGet() == (&OnGridQueryDataProvider()) );
	ASSERT_VALID( pCell->DataProviderGet() );
INT nSizeOfExpandShadow = ExpandShadowSizeGet();
bool bRetVal =
		pCell->OnInitExpandWnd(
			*this,
			htInfo,
			m_wndContentExpand,
			htInfo.m_rcItem,
			nSizeOfExpandShadow
			);
	if( ! bRetVal )
	{
		m_wndContentExpand.Deactivate();
		m_htInfoExpand.Empty();
	} // if( ! bRetVal )
	return bRetVal;
}

bool CExtGridWnd::OnGbwPaintExpandedItemContent(
	CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA & picd
	) const
{
	ASSERT_VALID( this );
	ASSERT( picd.m_dc.GetSafeHdc() != NULL );
	if( ! m_htInfoCellPressing.IsHoverEmpty() )
	{
		CRect rcVisibleRange = OnSiwGetVisibleRange();
		OnGridPaintDraggedCell(
			picd.m_dc,
			m_htInfoCellPressing.m_nVisibleColNo,
			m_htInfoCellPressing.m_nVisibleRowNo,
			m_htInfoCellPressing.m_nColNo,
			m_htInfoCellPressing.m_nRowNo,
			picd.m_rcItem,
			m_htInfoCellPressing.m_dwAreaFlags
			);
		return true;
	} // if( ! m_htInfoCellPressing.IsHoverEmpty() )
	if(		m_wndContentExpand.GetSafeHwnd() == NULL
		||	m_htInfoExpand.IsHoverEmpty()
		||	( ! m_htInfoExpand.IsValidRect() )
		)
		return false;
const CExtGridCell * pCell =
		GridCellGet(
			m_htInfoExpand.m_nColNo,
			m_htInfoExpand.m_nRowNo,
			m_htInfoExpand.GetInnerOuterTypeOfColumn(),
			m_htInfoExpand.GetInnerOuterTypeOfRow()
			);
	if( pCell == NULL )
		return false;
	ASSERT_VALID( pCell );
	ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
	//ASSERT( pCell->DataProviderGet() == (&OnGridQueryDataProvider()) );
	ASSERT_VALID( pCell->DataProviderGet() );
INT nSizeOfExpandShadow = ExpandShadowSizeGet();
bool bRetVal =
		pCell->OnPaintExpandedContent(
			picd,
			*this,
			m_htInfoExpand,
			m_wndContentExpand,
			nSizeOfExpandShadow
			);
	return bRetVal;
}

DWORD CExtGridWnd::BseGetStyle() const
{
	ASSERT( this != NULL );
	return m_dwBseStyle;
}

DWORD CExtGridWnd::BseModifyStyle(
	DWORD dwStyleAdd,
	DWORD dwStyleRemove, // = 0L
	bool bRedraw // = true
	)
{
	ASSERT( this != NULL );
	if( bRedraw )
	{
		if(		m_hWnd == NULL
			||	( ! ::IsWindow(m_hWnd) )
			)
			bRedraw = false;
	}
	if( bRedraw )
		SendMessage( WM_CANCELMODE );
DWORD dwBseStyleOld = m_dwBseStyle;
	m_dwBseStyle &= ~dwStyleRemove;
	m_dwBseStyle |= dwStyleAdd;
	if( bRedraw )
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	} // if( bRedraw )
	return dwBseStyleOld;
}

DWORD CExtGridWnd::BseGetStyleEx() const
{
	ASSERT( this != NULL );
	return m_dwBseStyleEx;
}

DWORD CExtGridWnd::BseModifyStyleEx(
	DWORD dwStyleExAdd,
	DWORD dwStyleExRemove, // = 0L
	bool bRedraw // = true
	)
{
	ASSERT( this != NULL );
	if( bRedraw )
	{
		if(		m_hWnd == NULL
			||	( ! ::IsWindow(m_hWnd) )
			)
			bRedraw = false;
	}
	if( bRedraw )
		SendMessage( WM_CANCELMODE );
DWORD dwBseStyleExOld = m_dwBseStyleEx;
	m_dwBseStyleEx &= ~dwStyleExRemove;
	m_dwBseStyleEx |= dwStyleExAdd;
	if( bRedraw )
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	} // if( bRedraw )
	return dwBseStyleExOld;
}

bool CExtGridWnd::GridCellRectsGet(
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	RECT * pRectCell,
	RECT * pRectCellExtra, // = NULL
	RECT * pRectText, // = NULL
	RECT * pRectTextArea, // = NULL
	RECT * pRectIcon, // = NULL
	RECT * pRectCheck, // = NULL
	RECT * pRectButtonEllipsis, // = NULL
	RECT * pRectButtonDropdown, // = NULL
	RECT * pRectButtonUp, // = NULL
	RECT * pRectButtonDown, // = NULL
	RECT * pRectFocusArrow, // = NULL
	RECT * pRectSortArrow // = NULL
	) const
{
	ASSERT_VALID( this );
	if( pRectCell != NULL )
		pRectCell->left = pRectCell->right
		= pRectCell->top = pRectCell->bottom
		= 0;
	if( pRectCellExtra != NULL )
		pRectCellExtra->left = pRectCellExtra->right
		= pRectCellExtra->top = pRectCellExtra->bottom
		= 0;
	if( pRectText != NULL )
		pRectText->left = pRectText->right
		= pRectText->top = pRectText->bottom
		= 0;
	if( pRectTextArea != NULL )
		pRectTextArea->left = pRectTextArea->right
		= pRectTextArea->top = pRectTextArea->bottom
		= 0;
	if( pRectIcon != NULL )
		pRectIcon->left = pRectIcon->right
		= pRectIcon->top = pRectIcon->bottom
		= 0;
	if( pRectCheck != NULL )
		pRectCheck->left = pRectCheck->right
		= pRectCheck->top = pRectCheck->bottom
		= 0;
	if( pRectButtonEllipsis != NULL )
		pRectButtonEllipsis->left = pRectButtonEllipsis->right
		= pRectButtonEllipsis->top = pRectButtonEllipsis->bottom
		= 0;
	if( pRectButtonDropdown != NULL )
		pRectButtonDropdown->left = pRectButtonDropdown->right
		= pRectButtonDropdown->top = pRectButtonDropdown->bottom
		= 0;
	if( pRectButtonUp != NULL )
		pRectButtonUp->left = pRectButtonUp->right
		= pRectButtonUp->top = pRectButtonUp->bottom
		= 0;
	if( pRectButtonDown != NULL )
		pRectButtonDown->left = pRectButtonDown->right
		= pRectButtonDown->top = pRectButtonDown->bottom
		= 0;
	if( pRectFocusArrow != NULL )
		pRectFocusArrow->left = pRectFocusArrow->right
		= pRectFocusArrow->top = pRectFocusArrow->bottom
		= 0;
	if( pRectSortArrow != NULL )
		pRectSortArrow->left = pRectSortArrow->right
		= pRectSortArrow->top = pRectSortArrow->bottom
		= 0;
LONG nVisibleColNo = 0L, nVisibleRowNo = 0L;
CRect rcCellExtra( 0, 0, 0, 0 ), rcCell( 0, 0, 0, 0 );;
	if( ! OnGbwCalcVisibleItemRect(
			nColNo,
			nRowNo,
			nColType,
			nRowType,
			rcCellExtra,
			rcCell,
			&nVisibleColNo,
			&nVisibleRowNo
			)
		)
		return false;
	if( pRectCell != NULL )
		::CopyRect( pRectCell, &rcCell );
	if( pRectCellExtra != NULL )
		::CopyRect( pRectCellExtra, &rcCell );
const CExtGridCell * pCell =
		GridCellGet(
			nColNo,
			nRowNo,
			nColType,
			nRowType
			);
	if( pCell == NULL )
		return true;
	ASSERT_VALID( pCell );
	ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
	//ASSERT( pCell->DataProviderGet() == (&OnGridQueryDataProvider()) );
	ASSERT_VALID( pCell->DataProviderGet() );
DWORD	dwAreaFlags =
			CExtGridHitTestInfo::CellTypesToAreaFlags(
				nColType,
				nRowType
				),
		dwHelperPaintFlags = 0L;
CClientDC dc( (CWnd*)this );
CRect	rcCellRest( rcCell ),
		rcCellText,
		rcFocusArrow,
		rcIcon,
		rcCheck,
		rcSortArrow,
		rcUpdownButtonTop,
		rcUpdownButtonBottom,
		rcDropdownButton,
		rcEllipsisButton;
CSize	sizeIcon(0,0),
		sizeFocusArrow =
			pCell->OnCalcFocusArrowSize(
				*this,
				dc,
				nVisibleColNo,
				nVisibleRowNo,
				nColNo,
				nRowNo,
				nColType,
				nRowType,
				dwAreaFlags,
				dwHelperPaintFlags
				),
		sizeSortArrow =
			pCell->OnCalcSortArrowSize(
				*this,
				dc,
				nVisibleColNo,
				nVisibleRowNo,
				nColNo,
				nRowNo,
				nColType,
				nRowType,
				dwAreaFlags,
				dwHelperPaintFlags
				);

//const CExtCmdIcon * pIcon = pCell->IconGet();
//	if(		pIcon != NULL
//		&&	( ! pIcon->IsEmpty() )
//		)
//	{ // if have icon
//		sizeIcon = pIcon->GetSize();
//		ASSERT( sizeIcon.cx > 0 && sizeIcon.cy > 0 );
//	} // if have icon
	sizeIcon = pCell->IconGetSize();

CSize sizeCheck = pCell->OnCalcCheckSize();
	ASSERT( sizeIcon.cx >= 0 && sizeIcon.cy >= 0 );
	ASSERT( sizeCheck.cx >= 0 && sizeCheck.cy >= 0 );
INT		nExtentUpdownButton =
			pCell->OnCalcButtonExtent(
				INT(CExtGridCell::__EBTT_UPDOWN_UP),
				*this,
				dc,
				nVisibleColNo,
				nVisibleRowNo,
				nColNo,
				nRowNo,
				nColType,
				nRowType,
				dwAreaFlags,
				dwHelperPaintFlags
				),
		nExtentDropdownButton =
			pCell->OnCalcButtonExtent(
				INT(CExtGridCell::__EBTT_DROPDOWN),
				*this,
				dc,
				nVisibleColNo,
				nVisibleRowNo,
				nColNo,
				nRowNo,
				nColType,
				nRowType,
				dwAreaFlags,
				dwHelperPaintFlags
				),
		nExtentEllipsisButton =
			pCell->OnCalcButtonExtent(
				INT(CExtGridCell::__EBTT_ELLIPSIS),
				*this,
				dc,
				nVisibleColNo,
				nVisibleRowNo,
				nColNo,
				nRowNo,
				nColType,
				nRowType,
				dwAreaFlags,
				dwHelperPaintFlags
				),
		nFocusGlyphType = -1;
	ASSERT( nExtentUpdownButton >= 0 );
	ASSERT( nExtentDropdownButton >= 0 );
	ASSERT( nExtentEllipsisButton >= 0 );
DWORD dwCellStyle = pCell->GetStyle();
DWORD dwCellStyleEx = pCell->GetStyleEx();
	pCell->OnCalcLayout(
		dwCellStyle,
		dwCellStyleEx,
		rcCellRest,
		rcCellText,
		rcFocusArrow,
		rcIcon,
		rcCheck,
		rcSortArrow,
		rcUpdownButtonTop,
		rcUpdownButtonBottom,
		rcDropdownButton,
		rcEllipsisButton,
		sizeFocusArrow,
		sizeIcon,
		sizeCheck,
		sizeSortArrow,
		nExtentUpdownButton,
		nExtentDropdownButton,
		nExtentEllipsisButton,
		nFocusGlyphType,
		*this,
		dc,
		nVisibleColNo,
		nVisibleRowNo,
		nColNo,
		nRowNo,
		nColType,
		nRowType,
		dwAreaFlags,
		dwHelperPaintFlags
		);
	if( pRectText != NULL )
		::CopyRect( pRectText, &rcCellText );
	if( pRectTextArea != NULL )
		::CopyRect( pRectTextArea, &rcCellRest );
	if( pRectIcon != NULL )
		::CopyRect( pRectIcon, &rcIcon );
	if( pRectCheck != NULL )
		::CopyRect( pRectCheck, &rcCheck );
	if( pRectButtonEllipsis != NULL )
		::CopyRect( pRectButtonEllipsis, &rcEllipsisButton );
	if( pRectButtonDropdown != NULL )
		::CopyRect( pRectButtonDropdown, &rcDropdownButton );
	if( pRectButtonUp != NULL )
		::CopyRect( pRectButtonUp, &rcUpdownButtonTop );
	if( pRectButtonDown != NULL )
		::CopyRect( pRectButtonDown, &rcUpdownButtonBottom );
	if( pRectFocusArrow != NULL )
		::CopyRect( pRectFocusArrow, &rcFocusArrow );
	if( pRectSortArrow != NULL )
		::CopyRect( pRectSortArrow, &rcSortArrow );
	return true;
}

bool CExtGridWnd::OnGbwBeginEdit(
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	INT nColType,
	INT nRowType,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	const RECT & rcInplaceControl,
	bool bContinueMsgLoop // = true
	)
{
	ASSERT_VALID( this );
	if(	CExtGridBaseWnd::OnGbwBeginEdit(
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			nColType,
			nRowType,
			rcCellExtra,
			rcCell,
			rcInplaceControl,
			bContinueMsgLoop
			)
		)
		return true;
static CExtGridCell * g_pEditCell = NULL;
bool bDoMsgLoop = true;
	if( m_hWndInplaceControl != NULL )
	{
		if( ! bContinueMsgLoop )
			return false;
		HWND hWndInplaceControl = m_hWndInplaceControl;
		m_hWndInplaceControl = NULL;
		if( hWndInplaceControl != NULL )
		{
			bDoMsgLoop = false;
			if( ::IsWindow(hWndInplaceControl) )
				::DestroyWindow( hWndInplaceControl );
		} // if( hWndInplaceControl != NULL )
	} // if( m_hWndInplaceControl != NULL )
CRect rcActivate( rcInplaceControl );
	if(		rcActivate.left >= rcActivate.right
		||	rcActivate.top >= rcActivate.bottom
		)
	{ // if invalid rect - recalc it
		if(	! GridCellRectsGet(
				nColNo,
				nRowNo,
				nColType,
				nRowType,
				NULL,
				NULL,
				NULL,
				&rcActivate // pRectTextArea
				)
			)
		{
			g_pEditCell = NULL;
			return false;
		}
		if( rcActivate.IsRectEmpty() )
		{
			g_pEditCell = NULL;
			return false;
		}
	} // if invalid rect - recalc it
//CClientDC dc( this );
//	dc.FillSolidRect( &rcActivate, RGB(255,0,0) );
//CString strMsg;
//strMsg.Format( _T("CExtGridWnd::OnGbwBeginEdit(%ld,%ld,%ld,%ld,%d,%d)"),nVisibleColNo,nVisibleRowNo,nColNo,nRowNo,nColType,nRowType);
//::AfxMessageBox( strMsg, MB_ICONINFORMATION );
	g_pEditCell =
		GridCellGet(
			nColNo,
			nRowNo,
			nColType,
			nRowType
			);
	if( g_pEditCell == NULL )
		return false;
	ASSERT_VALID( g_pEditCell );
	ASSERT( g_pEditCell->IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
	//ASSERT( g_pEditCell->DataProviderGet() == (&OnGridQueryDataProvider()) );
	ASSERT_VALID( g_pEditCell->DataProviderGet() );
	m_hWndInplaceControl =
		g_pEditCell->OnInplaceControlCreate(
			*this,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			nColType,
			nRowType,
			rcCellExtra,
			rcCell,
			rcActivate,
			m_nHelperLastEditedColNo,
			m_nHelperLastEditedRowNo
			);
	if(		m_hWndInplaceControl == NULL
		||	( ! ::IsWindow( m_hWndInplaceControl ) )
		)
	{
		m_hWndInplaceControl = NULL;
		g_pEditCell = NULL;
		return false;
	}
	m_nHelperLastEditedColNo = nColNo;
	m_nHelperLastEditedRowNo = nRowNo;
	if( ! bDoMsgLoop )
		return true;
HWND hWndGrid = m_hWnd;
bool bStopFlag = false;
	for(	MSG msg;
				m_hWndInplaceControl != NULL
			&&	::IsWindow( m_hWndInplaceControl )
			&&	::IsWindowVisible( m_hWndInplaceControl )
			&&	::IsWindow( hWndGrid )
			&&	(!bStopFlag)
			&&	(g_pEditCell != NULL)
			;
		)
	{ // in-place editor message loop
		if( !PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		{
			if(		m_hWndInplaceControl == NULL
				||	( ! ::IsWindow( m_hWndInplaceControl ) )
				||	( ! ::IsWindow( hWndGrid ) )
				||	bStopFlag
				|| g_pEditCell == NULL
				)
				break;
			for(	LONG nIdleCounter = 0L;
					g_pEditCell->OnInplaceControlIdle( m_hWndInplaceControl, *this, nIdleCounter );
					nIdleCounter ++
					)
			{
				ASSERT_VALID( g_pEditCell );
			}
			for(	nIdleCounter = 0L;
					::AfxGetThread()->OnIdle(nIdleCounter);
					nIdleCounter ++
					);
			::WaitMessage();
			continue;
		} // if( !PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		switch( msg.message )
		{
		case WM_KILLFOCUS:
			if( msg.hwnd == m_hWndInplaceControl )
				bStopFlag = true;
		break;
		case WM_CANCELMODE:
		case WM_ACTIVATEAPP:
		case WM_SYSCOMMAND:
		case WM_SETTINGCHANGE:
		case WM_SYSCOLORCHANGE:
			bStopFlag = true;
		break;
		case WM_COMMAND:
			if(		(HIWORD(msg.wParam)) == 0
				||	(HIWORD(msg.wParam)) == 1
				)
				bStopFlag = true;
		break;
		case WM_CAPTURECHANGED:
			if( (HWND)msg.wParam != m_hWndInplaceControl )
				bStopFlag = true;
		break;
		case WM_CONTEXTMENU:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_NCLBUTTONDBLCLK:
		case WM_NCLBUTTONDOWN:
		case WM_NCRBUTTONDBLCLK:
		case WM_NCRBUTTONDOWN:
		case WM_NCMBUTTONDBLCLK:
		case WM_NCMBUTTONDOWN:
			if( msg.hwnd != m_hWndInplaceControl )
				bStopFlag = true;
		break;
		default:
//			if(	   msg.message == WM_SYSKEYDOWN
//				|| msg.message == WM_SYSKEYUP
//				|| msg.message == WM_KEYDOWN
//				|| msg.message == WM_KEYUP
//				)
//			{
//				bool bKeyUp =
//					( msg.message == WM_KEYUP || msg.message == WM_SYSKEYUP )
//						? true : false;
//				BOOL bCtrl = GetKeyState(VK_CONTROL) & 0x80000000;
//				BOOL bShift = GetKeyState(VK_SHIFT) & 0x80000000;
//				//BOOL bAlt = HIWORD(msg.lParam) & KF_ALTDOWN;
//				TCHAR vkTCHAR = (TCHAR)msg.wParam;
//				if(			vkTCHAR == VK_MENU
//						&&	(!bCtrl) && (!bShift)
//						//&&	bKeyUp
//					)
//					bStopFlag = true;
//				//else if(	bAlt
//				//		&&	(	vkTCHAR == VK_SUBTRACT
//				//			||	vkTCHAR == VK_SPACE
//				//			)
//				//	)
//				//	bStopFlag = true;
//			}
			if(		( ! bStopFlag )
				&&	WM_KEYFIRST <= msg.message
				&&	msg.message <= WM_KEYLAST
				&&	msg.hwnd != m_hWndInplaceControl
				)
			{
				if( msg.hwnd != hWndGrid )
					bStopFlag = true;
				PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
			}
		break;
		} // switch( msg.message )
		if( bStopFlag )
			break;
		if( !AfxGetThread()->PumpMessage() )
			break;
	} // in-place editor message loop
	g_pEditCell = NULL;
	if( ! ::IsWindow( hWndGrid ) )
		return true;
	ASSERT( CWnd::FromHandlePermanent(hWndGrid) == this );
	ASSERT_VALID( this );
	m_nHelperLastEditedColNo = -1L;
	m_nHelperLastEditedRowNo = -1L;
	if( m_hWndInplaceControl != NULL )
	{
		if( ::IsWindow(m_hWndInplaceControl) )
		{
			if( ::GetCapture() == m_hWndInplaceControl )
				::ReleaseCapture();
			::DestroyWindow(m_hWndInplaceControl);
		} // if( ::IsWindow(m_hWndInplaceControl) )
		m_hWndInplaceControl = NULL;
	} // if( m_hWndInplaceControl != NULL )
	return true;
}

bool CExtGridWnd::OnGridTrackCellButton(
	CExtGridCell * pCell,
	const CExtGridHitTestInfo & htInfo
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pCell );
	ASSERT( (htInfo.m_dwAreaFlags&__EGBWA_CELL_BUTTON) != 0 );
	ASSERT( htInfo.m_nButtonType >= 0 );
static CExtGridCell * g_pTrackCell = NULL;
	if( g_pTrackCell != NULL )
		return false;
	if(		htInfo.m_rcPart.left >= htInfo.m_rcPart.right
		||	htInfo.m_rcPart.top >= htInfo.m_rcPart.bottom
		)
		return false;
bool bEnabled = false;
bool bStayPressed = false;
UINT nTimerElapseValue = 0;
	if( ! pCell->OnQueryButtonInfo(
			htInfo.m_nButtonType,
			&bEnabled,
			NULL,
			&bStayPressed,
			&nTimerElapseValue
			)
		)
		return false;
	if( ! bEnabled )
		return false;
	g_pTrackCell = pCell;
	g_pTrackCell->OnChangeButtonPressedState(
		htInfo.m_nButtonType,
		true
		);
	InvalidateRect( &htInfo.m_rcPart );
	//UpdateWindow();
ULONG nPressedTimerCounter = 0L;
	if( nTimerElapseValue > 0 )
		SetTimer( m_nTimerIdPressedButton, nTimerElapseValue, NULL );
HWND hWndGrid = m_hWnd;
	CExtMouseCaptureSink::SetCapture( hWndGrid );
bool bStopFlag = false;
bool bPressedEvent = false;
bool bMouseInsideButton = true;
	for(	MSG msg;
				::IsWindow( hWndGrid )
			&&	(!bStopFlag)
			&&	(g_pTrackCell != NULL)
			;
		)
	{ // editor message loop
		if( !PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		{
			if(		( ! ::IsWindow( hWndGrid ) )
				||	bStopFlag
				||	g_pTrackCell == NULL
				)
				break;
			for(	LONG nIdleCounter = 0L;
					::AfxGetThread()->OnIdle(nIdleCounter);
					nIdleCounter ++
					);
			::WaitMessage();
			continue;
		} // if( !PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		switch( msg.message )
		{
		case WM_KILLFOCUS:
			if( msg.hwnd == hWndGrid )
				bStopFlag = true;
		break;
		case WM_CANCELMODE:
		case WM_ACTIVATEAPP:
		case WM_SYSCOMMAND:
		case WM_SETTINGCHANGE:
		case WM_SYSCOLORCHANGE:
			bStopFlag = true;
		break;
		case WM_COMMAND:
			if(		(HIWORD(msg.wParam)) == 0
				||	(HIWORD(msg.wParam)) == 1
				)
				bStopFlag = true;
		break;
		case WM_CAPTURECHANGED:
			if( (HWND)msg.wParam != hWndGrid )
				bStopFlag = true;
		break;
		case WM_TIMER:
			if(		msg.hwnd == hWndGrid
				&&	UINT(msg.wParam) == m_nTimerIdPressedButton
				)
			{ // if pressed button timer event
				ASSERT_VALID( this );
				ASSERT_VALID( g_pTrackCell );
				PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
				g_pTrackCell->OnButtonPressing(
					*this,
					htInfo.m_nButtonType,
					bMouseInsideButton,
					nPressedTimerCounter,
					htInfo.m_rcPart,
					htInfo.m_rcExtra,
					htInfo.m_rcItem
					);
				nPressedTimerCounter ++;
				continue;
			} // if pressed button timer event
		break;
		case WM_MOUSEWHEEL:
			if( msg.hwnd != hWndGrid )
				bStopFlag = true;
			else
			{
				PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
				continue;
			} // else from if( msg.hwnd != hWndGrid )
		break;
		case WM_MOUSEMOVE:
			if( msg.hwnd != hWndGrid )
				bStopFlag = true;
			else
			{
				ASSERT_VALID( this );
				ASSERT_VALID( g_pTrackCell );
				PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
				CPoint point;
				point = DWORD( msg.lParam );
				bool bInside =
					htInfo.m_rcPart.PtInRect(point) ? true : false;
				if( bMouseInsideButton != bInside )
				{
					bMouseInsideButton = bInside;
					g_pTrackCell->OnChangeButtonPressedState(
						htInfo.m_nButtonType,
						bMouseInsideButton
						);
					InvalidateRect( &htInfo.m_rcPart );
					//UpdateWindow();
				} // if( bMouseInsideButton != bInside )
				continue;
			} // else from if( msg.hwnd != hWndGrid )
		break;
		case WM_LBUTTONUP:
			bStopFlag = true;
			if( msg.hwnd == hWndGrid )
			{
				ASSERT_VALID( this );
				ASSERT_VALID( g_pTrackCell );
				CPoint point;
				point = DWORD( msg.lParam );
				if( htInfo.m_rcPart.PtInRect(point) )
				{
					bPressedEvent = true;
					if( bMouseInsideButton != bPressedEvent )
					{
						bMouseInsideButton = bPressedEvent;
						if(		( ! bMouseInsideButton )
							&&	nTimerElapseValue > 0
							)
						{
							KillTimer( m_nTimerIdPressedButton );
							nPressedTimerCounter = 0L;
						}
						g_pTrackCell->OnChangeButtonPressedState(
							htInfo.m_nButtonType,
							bMouseInsideButton
							);
						InvalidateRect( &htInfo.m_rcPart );
						//UpdateWindow();
					} // if( bMouseInsideButton != bPressedEvent )
				} // if( htInfo.m_rcPart.PtInRect(point) )
				PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
			} // if( msg.hwnd == hWndGrid )
			break;
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_CONTEXTMENU:
		case WM_NCLBUTTONUP:
		case WM_NCLBUTTONDBLCLK:
		case WM_NCLBUTTONDOWN:
		case WM_NCRBUTTONUP:
		case WM_NCRBUTTONDBLCLK:
		case WM_NCRBUTTONDOWN:
		case WM_NCMBUTTONUP:
		case WM_NCMBUTTONDBLCLK:
		case WM_NCMBUTTONDOWN:
			bStopFlag = true;
		break;
		default:
			if(	   WM_KEYFIRST <= msg.message
				&& msg.message <= WM_KEYLAST
				)
				bStopFlag = true;
		break;
		} // switch( msg.message )
		if( bStopFlag )
			break;
		if( !AfxGetThread()->PumpMessage() )
			break;
	} // message loop
	g_pTrackCell = NULL;
	if( ! ::IsWindow( hWndGrid ) )
		return true;
	if( nTimerElapseValue > 0 )
	{
		KillTimer( m_nTimerIdPressedButton );
		nPressedTimerCounter = 0L;
	}
	if( CExtMouseCaptureSink::GetCapture() == hWndGrid )
		CExtMouseCaptureSink::ReleaseCapture();
	ASSERT( CWnd::FromHandlePermanent(hWndGrid) == this );
	ASSERT_VALID( this );
	ASSERT_VALID( pCell );
	if( bPressedEvent )
	{
		if( (!bStayPressed) && bMouseInsideButton )
		{
			bMouseInsideButton = false;
			pCell->OnChangeButtonPressedState(
				htInfo.m_nButtonType,
				false
				);
		} // if( (!bStayPressed) && bMouseInsideButton )
		pCell->OnButtonPressed(
			*this,
			htInfo.m_nButtonType,
			htInfo.m_rcExtra,
			htInfo.m_rcItem,
			htInfo.m_nVisibleColNo,
			htInfo.m_nVisibleRowNo,
			htInfo.m_nColNo,
			htInfo.m_nRowNo,
			htInfo.GetInnerOuterTypeOfColumn(),
			htInfo.GetInnerOuterTypeOfRow()
			);
	} // if( bPressedEvent )
	if( bMouseInsideButton )
		pCell->OnChangeButtonPressedState(
			htInfo.m_nButtonType,
			false
			);
	InvalidateRect( &htInfo.m_rcPart );
	//UpdateWindow();
	return true;
}

CSize CExtGridWnd::OnGridQueryStartDragOffset(
	const CExtGridHitTestInfo * pHtInfo // = NULL
	) const
{
	ASSERT_VALID( this );
	pHtInfo;
	return CSize( 4, 4 );
}

const CExtGridHitTestInfo & CExtGridWnd::OnGridQueryPressingCellInfo() const
{
	ASSERT_VALID( this );
	return m_htInfoCellPressing;
}

void CExtGridWnd::OnGridTrackCellMousePressing(
	CExtGridCell * pCell,
	const CExtGridHitTestInfo & htInfo,
	UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
	UINT nFlags, // mouse event flags
	bool bAllowStartDragEvent, // = true
	CExtGridDataSortOrder * p_gdsoUpdate, // = NULL
	bool bUpdateMultipleSort, // = false
	bool bSortColumns // = false
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pCell );
static CExtGridCell * g_pTrackCell = NULL;
	if( g_pTrackCell != NULL )
		return;
	g_pTrackCell = pCell;
	if( g_pTrackCell->OnCellPressingEnter(
			*this,
			htInfo,
			nChar,
			nFlags
			)
		)
		return;
	m_htInfoCellPressing = htInfo;
DWORD dwBseStyleEx = BseGetStyleEx();
	if( (dwBseStyleEx&__EGBS_BSE_EX_HIGHLIGHT_PRESSING_ALL) != 0 )
		OnSwDoRedraw();
HWND hWndGrid = m_hWnd;
	CExtMouseCaptureSink::SetCapture( hWndGrid );
bool bStopFlag = false;
bool bPressedEvent = false;
bool bStartDragEvent = false;
bool bMouseInsideCell = true;
bool bStayPressed = false;
CSize sizeStartDragOffset = OnGridQueryStartDragOffset( &htInfo );
	ASSERT( sizeStartDragOffset.cx >= 0 && sizeStartDragOffset.cy >= 0 );
	if(			htInfo.GetInnerOuterTypeOfColumn() == 0
			&&	(dwBseStyleEx&__EGBS_BSE_EX_HIGHLIGHT_PRESSING_STAY_COLUMNS) != 0
			)
			bStayPressed = true;
	else if(	htInfo.GetInnerOuterTypeOfRow() == 0
			&&	(dwBseStyleEx&__EGBS_BSE_EX_HIGHLIGHT_PRESSING_STAY_ROWS) != 0
			)
			bStayPressed = true;
	for(	MSG msg;
				::IsWindow( hWndGrid )
			&&	(!bStopFlag)
			&&	(g_pTrackCell != NULL)
			;
		)
	{ // editor message loop
		if( !PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		{
			if(		( ! ::IsWindow( hWndGrid ) )
				||	bStopFlag
				||	g_pTrackCell == NULL
				)
				break;
			for(	LONG nIdleCounter = 0L;
					::AfxGetThread()->OnIdle(nIdleCounter);
					nIdleCounter ++
					);
			::WaitMessage();
			continue;
		} // if( !PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		switch( msg.message )
		{
		case WM_KILLFOCUS:
			if( msg.hwnd == hWndGrid )
				bStopFlag = true;
		break;
		case WM_CANCELMODE:
		case WM_ACTIVATEAPP:
		case WM_SYSCOMMAND:
		case WM_SETTINGCHANGE:
		case WM_SYSCOLORCHANGE:
			bStopFlag = true;
		break;
		case WM_COMMAND:
			if(		(HIWORD(msg.wParam)) == 0
				||	(HIWORD(msg.wParam)) == 1
				)
				bStopFlag = true;
		break;
		case WM_CAPTURECHANGED:
			if( (HWND)msg.wParam != hWndGrid )
				bStopFlag = true;
		break;
		case WM_MOUSEWHEEL:
			if( msg.hwnd != hWndGrid )
				bStopFlag = true;
			else
			{
				PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
				continue;
			} // else from if( msg.hwnd != hWndGrid )
		break;
		case WM_MOUSEMOVE:
			if( msg.hwnd != hWndGrid )
				bStopFlag = true;
			else
			{
				ASSERT_VALID( this );
				ASSERT_VALID( g_pTrackCell );
				PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
				CPoint point;
				point = DWORD( msg.lParam );
				bool bInside =
					htInfo.m_rcItem.PtInRect(point) ? true : false;
				if( bMouseInsideCell != bInside )
				{
					bMouseInsideCell = bInside;
					if( ! bStayPressed )
					{
						if( bMouseInsideCell )
						{
							m_htInfoCellPressing = htInfo;
							m_htInfoCellPressing.m_ptClient = point;
						} // if( bMouseInsideCell )
						else
							m_htInfoCellPressing.Empty();
					} // if( ! bStayPressed )
					g_pTrackCell->OnCellPressingStateChanged(
						*this,
						htInfo,
						nChar,
						nFlags,
						bMouseInsideCell
						);
					if( (dwBseStyleEx&__EGBS_BSE_EX_HIGHLIGHT_PRESSING_ALL) != 0 )
						OnSwDoRedraw();
				} // if( bMouseInsideCell != bInside )
				if( bAllowStartDragEvent )
				{
					CSize sizeOffset(
						abs( point.x - htInfo.m_ptClient.x ),
						abs( point.y - htInfo.m_ptClient.y )
						);
					if(		sizeOffset.cx >= sizeStartDragOffset.cx
						||	sizeOffset.cy >= sizeStartDragOffset.cy
						)
					{
						bStartDragEvent = true;
						bStopFlag = true;
						break;
					}
				} // if( bAllowStartDragEvent )
				continue;
			} // else from if( msg.hwnd != hWndGrid )
		break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			bStopFlag = true;
			if(		msg.hwnd == hWndGrid
				&&	(	( nChar == VK_LBUTTON && msg.message == WM_LBUTTONUP )
					||	( nChar == VK_MBUTTON && msg.message == WM_MBUTTONUP )
					||	( nChar == VK_RBUTTON && msg.message == WM_RBUTTONUP )
					)
				)
			{
				ASSERT_VALID( this );
				ASSERT_VALID( g_pTrackCell );
				CPoint point;
				point = DWORD( msg.lParam );
				if( htInfo.m_rcItem.PtInRect(point) )
				{
					bPressedEvent = true;
					if( bMouseInsideCell != bPressedEvent )
					{
						if( ! bStayPressed )
						{
							bMouseInsideCell = bPressedEvent;
							if( bMouseInsideCell )
							{
								m_htInfoCellPressing = htInfo;
								m_htInfoCellPressing.m_ptClient = point;
							} // if( bMouseInsideCell )
							else
								m_htInfoCellPressing.Empty();
						} // if( ! bStayPressed )
						g_pTrackCell->OnCellPressingStateChanged(
							*this,
							htInfo,
							nChar,
							nFlags,
							bMouseInsideCell
							);
						if( (dwBseStyleEx&__EGBS_BSE_EX_HIGHLIGHT_PRESSING_ALL) != 0 )
							OnSwDoRedraw();
					} // if( bMouseInsideCell != bPressedEvent )
				} // if( htInfo.m_rcItem.PtInRect(point) )
				PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
			} // if( msg.hwnd == hWndGrid ...
			break;
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_CONTEXTMENU:
		case WM_NCLBUTTONUP:
		case WM_NCLBUTTONDBLCLK:
		case WM_NCLBUTTONDOWN:
		case WM_NCRBUTTONUP:
		case WM_NCRBUTTONDBLCLK:
		case WM_NCRBUTTONDOWN:
		case WM_NCMBUTTONUP:
		case WM_NCMBUTTONDBLCLK:
		case WM_NCMBUTTONDOWN:
			bStopFlag = true;
		break;
		default:
			if(	   WM_KEYFIRST <= msg.message
				&& msg.message <= WM_KEYLAST
				)
			{
				if( msg.message == WM_KEYDOWN )
				{
					if( (nFlags&MK_SHIFT) != 0 )
					{
						if( msg.wParam == VK_SHIFT )
						{
							PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
							continue;
						} // if( msg.wParam == VK_SHIFT )
					} // if( (nFlags&MK_SHIFT) != 0 )
					if( (nFlags&MK_CONTROL) != 0 )
					{
						if( msg.wParam == VK_CONTROL )
						{
							PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
							continue;
						} // if( msg.wParam == VK_CONTROL )
					} // if( (nFlags&MK_CONTROL) != 0 )
				} // if( msg.message == WM_KEYDOWN )
				bStopFlag = true;
			}
		break;
		} // switch( msg.message )
		if( bStopFlag )
			break;
		if( !AfxGetThread()->PumpMessage() )
			break;
	} // message loop
	g_pTrackCell = NULL;
	if( ! ::IsWindow( hWndGrid ) )
		return;
	if( ! bStayPressed )
	{
		m_htInfoCellPressing.Empty();
		if( (dwBseStyleEx&__EGBS_BSE_EX_HIGHLIGHT_PRESSING_ALL) != 0 )
			OnSwDoRedraw();
	} // if( ! bStayPressed )
	if( CExtMouseCaptureSink::GetCapture() == hWndGrid )
		CExtMouseCaptureSink::ReleaseCapture();
	ASSERT( CWnd::FromHandlePermanent(hWndGrid) == this );
	ASSERT_VALID( this );
	ASSERT_VALID( pCell );
	if( ! pCell->OnCellPressingLeave(
			*this,
			htInfo,
			nChar,
			nFlags,
			bPressedEvent,
			bStartDragEvent
			)
		)
	{ // if cell did not handle pressed/drag events
		if( bPressedEvent )
		{
			if( p_gdsoUpdate != NULL )
				GridSortOrderSetup(
					bSortColumns,
					*p_gdsoUpdate,
					bUpdateMultipleSort,
					true,
					true
					);
		} // if( bPressedEvent )
		else if( bStartDragEvent )
		{
			OnGridTrackOuterCellDND(
				pCell,
				htInfo,
				nChar,
				nFlags
				);
		} // else if( bStartDragEvent )
	} // if cell did not handle pressed/drag events
	m_htInfoCellPressing.Empty();
	if( (dwBseStyleEx&__EGBS_BSE_EX_HIGHLIGHT_PRESSING_ALL) != 0 )
		OnSwDoRedraw();
}

void CExtGridWnd::OnGridTrackOuterCellDND(
	CExtGridCell * pCell,
	const CExtGridHitTestInfo & htInfo,
	UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
	UINT nFlags // mouse event flags
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pCell );
static CExtGridCell * g_pTrackCell = NULL;
	if( g_pTrackCell != NULL )
		return;
	if( pCell->OnOuterDndTrack(
			*this,
			htInfo,
			nChar,
			nFlags
			)
		)
		return;
CRect rcDND( htInfo.m_rcItem );
CExtContentExpandWnd wndDND, wndArrows;
	if( ! wndDND.Activate(
			rcDND,
			this,
			__ECWAF_DEF_EXPANDED_ITEM_PAINTER
				|__ECWAF_NO_CAPTURE
				|__ECWAF_REDIRECT_MOUSE
				|__ECWAF_REDIRECT_NO_DEACTIVATE
				|__ECWAF_REDIRECT_AND_HANDLE
				|__ECWAF_HANDLE_MOUSE_ACTIVATE
				|__ECWAF_MA_NOACTIVATE
			)
		)
	{
		ASSERT( FALSE );
		return;
	}
CExtGridHitTestInfo htInfoDrop;
CPoint ptShiftLast( 0, 0 );
CRect rcInitialWndDND;
	wndDND.GetWindowRect( &rcInitialWndDND );
CRect rcVisibleRange = OnSiwGetVisibleRange();
HWND hWndGrid = m_hWnd;
	g_pTrackCell = pCell;
	CExtMouseCaptureSink::SetCapture( hWndGrid );
bool bStopFlag = false;
bool bEventDropIn = false, bEventDropOut = false;
INT nColType = htInfo.GetInnerOuterTypeOfColumn();
	ASSERT( m_hCursorOuterDragOK != NULL );
	ASSERT( m_hCursorOuterDragCancel != NULL );
	::SetCursor( m_hCursorOuterDragOK );
	for(	MSG msg;
				::IsWindow( hWndGrid )
			&&	(!bStopFlag)
			&&	(g_pTrackCell != NULL)
			;
		)
	{ // editor message loop
		if( !PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		{
			if(		( ! ::IsWindow( hWndGrid ) )
				||	bStopFlag
				||	g_pTrackCell == NULL
				)
				break;
			for(	LONG nIdleCounter = 0L;
					::AfxGetThread()->OnIdle(nIdleCounter);
					nIdleCounter ++
					);
			::WaitMessage();
			continue;
		} // if( !PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		switch( msg.message )
		{
		case WM_KILLFOCUS:
			if( msg.hwnd == hWndGrid )
				bStopFlag = true;
		break;
		case WM_CANCELMODE:
		case WM_ACTIVATEAPP:
		case WM_SYSCOMMAND:
		case WM_SETTINGCHANGE:
		case WM_SYSCOLORCHANGE:
			bStopFlag = true;
		break;
		case WM_COMMAND:
			if(		(HIWORD(msg.wParam)) == 0
				||	(HIWORD(msg.wParam)) == 1
				)
				bStopFlag = true;
		break;
		case WM_CAPTURECHANGED:
			if( (HWND)msg.wParam != hWndGrid )
				bStopFlag = true;
		break;
		case WM_MOUSEWHEEL:
			if( msg.hwnd != hWndGrid )
				bStopFlag = true;
			else
			{
				PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
				continue;
			} // else from if( msg.hwnd != hWndGrid )
		break;
		case WM_MOUSEMOVE:
			if( msg.hwnd != hWndGrid )
				bStopFlag = true;
			else
			{
				ASSERT_VALID( this );
				ASSERT_VALID( g_pTrackCell );
				PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
				CPoint point;
				point = DWORD( msg.lParam );
				CPoint ptShift = point - htInfo.m_ptClient;
				if( ptShift != ptShiftLast )
				{
					CExtGridHitTestInfo htInfoDropNew;
					OnGridCalcOuterDropTarget(
						htInfo,
						htInfoDropNew,
						point
						);
					bool bSelfCancelDropEffect = false;
					if( ! htInfoDropNew.IsHoverEmpty() )
					{
						if(		(! htInfoDropNew.IsHoverEqual(htInfoDrop) )
							||	wndArrows.GetSafeHwnd() == NULL
							)
						{
							bEventDropIn = false;
							bEventDropOut = true;
							htInfoDrop = htInfoDropNew;
							bool bOuterDropAfterState =
								( (htInfoDrop.m_dwAreaFlags&__EGBWA_OUTER_DROP_AFTER) != 0 )
									? true : false;
							if( nColType != 0 )
							{
								if( htInfoDrop.m_nRowNo == htInfo.m_nRowNo )
								{
										bSelfCancelDropEffect = true;
										bEventDropOut = false;
								}
								else if(	(! bOuterDropAfterState )
										&&	htInfoDrop.m_nRowNo == (htInfo.m_nRowNo+1L)
										)
								{
										bSelfCancelDropEffect = true;
										bEventDropOut = false;
								}
								else if(	bOuterDropAfterState
										&&	(htInfoDrop.m_nRowNo+1L) == htInfo.m_nRowNo
										)
								{
										bSelfCancelDropEffect = true;
										bEventDropOut = false;
								}
							} // if( nColType != 0 )
							else
							{
								if( htInfoDrop.m_nColNo == htInfo.m_nColNo )
								{
										bSelfCancelDropEffect = true;
										bEventDropOut = false;
								}
								else if(	(! bOuterDropAfterState )
										&&	htInfoDrop.m_nColNo == (htInfo.m_nColNo+1L)
										)
								{
										bSelfCancelDropEffect = true;
										bEventDropOut = false;
								}
								else if(	bOuterDropAfterState
										&&	(htInfoDrop.m_nColNo+1L) == htInfo.m_nColNo
										)
								{
										bSelfCancelDropEffect = true;
										bEventDropOut = false;
								}
							} // else from if( nColType != 0 )
							if( ! bSelfCancelDropEffect )
							{
								bEventDropIn = true;
								bEventDropOut = false;
								CRect rcArrows;
								if( nColType != 0 )
								{
									rcArrows.SetRect(
											htInfoDrop.m_rcItem.left,
											bOuterDropAfterState
												? htInfoDrop.m_rcItem.bottom
												: htInfoDrop.m_rcItem.top,
											htInfoDrop.m_rcItem.right,
											bOuterDropAfterState
												? htInfoDrop.m_rcItem.bottom
												: htInfoDrop.m_rcItem.top
											);
								} // if( nColType != 0 )
								else
								{
									rcArrows.SetRect(
											bOuterDropAfterState
												? htInfoDrop.m_rcItem.right
												: htInfoDrop.m_rcItem.left,
											htInfoDrop.m_rcItem.top,
											bOuterDropAfterState
												? htInfoDrop.m_rcItem.right
												: htInfoDrop.m_rcItem.left,
											htInfoDrop.m_rcItem.bottom
											);
								} // else from if( nColType != 0 )
								VERIFY(
									wndArrows.Activate(
										rcArrows,
										this,
										__ECWAF_DRAW_RED_ARROWS
											|__ECWAF_TRANSPARENT_ITEM
											|__ECWAF_NO_CAPTURE
											|__ECWAF_REDIRECT_MOUSE
											|__ECWAF_REDIRECT_NO_DEACTIVATE
											|__ECWAF_REDIRECT_AND_HANDLE
											|__ECWAF_HANDLE_MOUSE_ACTIVATE
											|__ECWAF_MA_NOACTIVATE
										)
									);
								if( wndArrows.GetSafeHwnd() != NULL )
									wndArrows.SetWindowPos(
										&wndDND, 0, 0, 0, 0,
										SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE
										);
							} // if( ! bSelfCancelDropEffect )
							if( bEventDropIn )
								bEventDropOut = false;
						} // if( ! htInfoDropNew.IsHoverEqual(htInfoDrop) ...
					} // if( ! htInfoDropNew.IsHoverEmpty() )
					else
					{
						bEventDropIn = false;
						bEventDropOut = true;
						bSelfCancelDropEffect = true;
					} // else from if( ! htInfoDropNew.IsHoverEmpty() )
					if( bSelfCancelDropEffect )
						wndArrows.Deactivate();
					if( bEventDropOut )
						::SetCursor( m_hCursorOuterDragCancel );
					else //if( bEventDropIn )
						::SetCursor( m_hCursorOuterDragOK );
					ptShiftLast = ptShift;
					CRect rcWnd( rcInitialWndDND );
					rcWnd.OffsetRect( ptShift );
					wndDND.MoveWindow( &rcWnd );
					wndDND.UpdateWindow();
					if( wndArrows.GetSafeHwnd() != NULL )
						wndArrows.UpdateWindow();
					UpdateWindow();
				} // if( ptShift != ptShiftLast )
				continue;
			} // else from if( msg.hwnd != hWndGrid )
		break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			bStopFlag = true;
			if(		msg.hwnd == hWndGrid
				&&	(	( nChar == VK_LBUTTON && msg.message == WM_LBUTTONUP )
					||	( nChar == VK_MBUTTON && msg.message == WM_MBUTTONUP )
					||	( nChar == VK_RBUTTON && msg.message == WM_RBUTTONUP )
					)
				)
			{
				ASSERT_VALID( this );
				ASSERT_VALID( g_pTrackCell );
//				CPoint point;
//				point = DWORD( msg.lParam );
				//if( ! htInfoDrop.IsHoverEmpty() )
				//	bEventDropIn = true;
				PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
			} // if( msg.hwnd == hWndGrid ...
			break;
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_CONTEXTMENU:
		case WM_NCLBUTTONUP:
		case WM_NCLBUTTONDBLCLK:
		case WM_NCLBUTTONDOWN:
		case WM_NCRBUTTONUP:
		case WM_NCRBUTTONDBLCLK:
		case WM_NCRBUTTONDOWN:
		case WM_NCMBUTTONUP:
		case WM_NCMBUTTONDBLCLK:
		case WM_NCMBUTTONDOWN:
			bStopFlag = true;
		break;
		default:
			if(	   WM_KEYFIRST <= msg.message
				&& msg.message <= WM_KEYLAST
				)
			{
				if( msg.message == WM_KEYDOWN || msg.message == WM_KEYUP )
				{
					if( (nFlags&MK_SHIFT) != 0 )
					{
						if( msg.wParam == VK_SHIFT )
						{
							PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
							continue;
						} // if( msg.wParam == VK_SHIFT )
					} // if( (nFlags&MK_SHIFT) != 0 )
					if( (nFlags&MK_CONTROL) != 0 )
					{
						if( msg.wParam == VK_CONTROL )
						{
							PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
							continue;
						} // if( msg.wParam == VK_CONTROL )
					} // if( (nFlags&MK_CONTROL) != 0 )
				} // if( msg.message == WM_KEYDOWN || msg.message == WM_KEYUP )
				bStopFlag = true;
			}
		break;
		} // switch( msg.message )
		if( bStopFlag )
			break;
		if( !AfxGetThread()->PumpMessage() )
			break;
//		PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
//		DispatchMessage(&msg);
	} // message loop
	g_pTrackCell = NULL;
	wndDND.Deactivate();
	wndArrows.Deactivate();
	if( ! ::IsWindow( hWndGrid ) )
		return;
	g_pTrackCell = NULL;
	if( bEventDropIn )
	{
		ASSERT( ! htInfoDrop.IsHoverEmpty() );
		OnGridOuterDragComplete( htInfo, htInfoDrop );
	} // if( bEventDropIn )
	else if( bEventDropOut )
	{
		OnGridOuterDragOut( htInfo );
	} // else if( bEventDropOut )
	OnGbwHoverRecalc();
	OnSwDoRedraw();
}

void CExtGridWnd::OnGridOuterDragComplete(
	const CExtGridHitTestInfo & htInfoDrag,
	const CExtGridHitTestInfo & htInfoDrop
	)
{
	ASSERT_VALID( this );
	ASSERT( ! htInfoDrag.IsHoverEmpty() );
	ASSERT( ! htInfoDrop.IsHoverEmpty() );
	ASSERT( htInfoDrag.GetInnerOuterTypeOfColumn() == htInfoDrop.GetInnerOuterTypeOfColumn() );
	ASSERT( htInfoDrag.GetInnerOuterTypeOfRow() == htInfoDrop.GetInnerOuterTypeOfRow() );
INT nColType = htInfoDrag.GetInnerOuterTypeOfColumn();
bool bOuterDropAfterState =
		( (htInfoDrop.m_dwAreaFlags&__EGBWA_OUTER_DROP_AFTER) != 0 )
			? true : false;
CExtGridCell * pCellDrag = GridCellGet( htInfoDrag );
CExtGridCell * pCellDrop = GridCellGet( htInfoDrop );
	if(		pCellDrag != NULL
		&&	pCellDrag->OnOuterDndComplete( *this, true, htInfoDrag, htInfoDrop )
		)
		return;
	if(		pCellDrop != NULL
		&&	pCellDrop->OnOuterDndComplete( *this, false, htInfoDrag, htInfoDrop )
		)
		return;
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	if( nColType != 0 )
	{
		LONG nDropBeforeRowNo = htInfoDrop.m_nRowNo;
		if( bOuterDropAfterState )
			nDropBeforeRowNo ++;
		ASSERT( 0 <= nDropBeforeRowNo );
		ASSERT( nDropBeforeRowNo <= RowCountGet() );
		ASSERT( 0 <= htInfoDrag.m_nRowNo );
		ASSERT( htInfoDrag.m_nRowNo < RowCountGet() );
		ULONG nOffset = 0L;
		_DataProvider.CacheReservedCountsGet( NULL, &nOffset );
		VERIFY(
			_DataProvider.SwapDroppedSeries(
				false,
				htInfoDrag.m_nRowNo + nOffset,
				nDropBeforeRowNo + nOffset,
				this
				)
			);
	} // if( nColType != 0 )
	else
	{
		LONG nDropBeforeColNo = htInfoDrop.m_nColNo;
		if( bOuterDropAfterState )
			nDropBeforeColNo ++;
		ASSERT( 0 <= nDropBeforeColNo );
		ASSERT( nDropBeforeColNo <= ColumnCountGet() );
		ASSERT( 0 <= htInfoDrag.m_nColNo );
		ASSERT( htInfoDrag.m_nColNo < ColumnCountGet() );
		ULONG nOffset = 0L;
		_DataProvider.CacheReservedCountsGet( &nOffset, NULL );
		VERIFY(
			_DataProvider.SwapDroppedSeries(
				true,
				htInfoDrag.m_nColNo + nOffset,
				nDropBeforeColNo + nOffset,
				this
				)
			);
	} // else from if( nColType != 0 )
}

void CExtGridWnd::OnGridOuterDragOut(
	const CExtGridHitTestInfo & htInfo
	)
{
	ASSERT_VALID( this );
	ASSERT( ! htInfo.IsHoverEmpty() );
CExtGridCell * pCell = GridCellGet( htInfo );
	if(		pCell != NULL
		&&	pCell->OnOuterDndOut( *this, htInfo )
		)
		return;
INT nColType = htInfo.GetInnerOuterTypeOfColumn();
DWORD dwBseStyleEx = BseGetStyleEx();
	if( nColType != 0 )
	{
		if( (dwBseStyleEx&__EGBS_BSE_EX_DRAG_REMOVE_ROWS) != 0 )
		{
			CExtGridDataSortOrder _gdso;
			GridSortOrderGet( true, _gdso );
			RowRemove( htInfo.m_nRowNo );
			if(		_gdso.GetRowColPos(htInfo.m_nRowNo) >= 0L
				&&	_gdso.m_arrItems.GetSize() > 1L
				)
			{
				OnGridQueryDataProvider().
					SortOrderUpdate( true, this );
				OnSwDoRedraw();
			} // if( _gdso.GetRowColPos(htInfo.m_nRowNo) >= 0L ...
		} // if( (dwBseStyleEx&__EGBS_BSE_EX_DRAG_REMOVE_ROWS) != 0 )
	} // if( nColType != 0 )
	else
	{
		if( (dwBseStyleEx&__EGBS_BSE_EX_DRAG_REMOVE_COLUMNS) != 0 )
		{
			CExtGridDataSortOrder _gdso;
			GridSortOrderGet( false, _gdso );
			ColumnRemove( htInfo.m_nColNo );
			if(		_gdso.GetRowColPos(htInfo.m_nColNo) >= 0L
				&&	_gdso.m_arrItems.GetSize() > 1L
				)
			{
				OnGridQueryDataProvider().
					SortOrderUpdate( false, this );
				OnSwDoRedraw();
			} // if( _gdso.GetRowColPos(htInfo.m_nColNo) >= 0L ...
		} // if( (dwBseStyleEx&__EGBS_BSE_EX_DRAG_REMOVE_COLUMNS) != 0 )
	} // else from if( nColType != 0 )
}

void CExtGridWnd::OnGridCalcOuterDropTarget(
	const CExtGridHitTestInfo & htInfoDrag,
	CExtGridHitTestInfo & htInfoDrop,
	CPoint point
	)
{
	ASSERT_VALID( this );
	htInfoDrop.Empty();
	if( htInfoDrag.IsHoverEmpty() )
		return;
	htInfoDrop.m_ptClient = point;
	HitTest( htInfoDrop, false, false, true );
	if( htInfoDrop.IsHoverEmpty() )
		return;
INT nDragColType = htInfoDrag.GetInnerOuterTypeOfColumn();
INT nDropColType = htInfoDrop.GetInnerOuterTypeOfColumn();
INT nDragRowType = htInfoDrag.GetInnerOuterTypeOfRow();
INT nDropRowType = htInfoDrop.GetInnerOuterTypeOfRow();
	if(	!(		nDragColType == nDropColType
			&&	nDragRowType == nDropRowType
			)
		)
	{
		htInfoDrop.Empty();
		return;
	}
	if( nDragColType == 0 )
	{
		if( htInfoDrag.m_nRowNo != htInfoDrop.m_nRowNo )
		{
			htInfoDrop.Empty();
			return;
		}
		return;
	} // if( nDragColType == 0 )
	if( nDragRowType == 0 )
	{
		if( htInfoDrag.m_nColNo != htInfoDrop.m_nColNo )
		{
			htInfoDrop.Empty();
			return;
		}
		return;
	} // if( nDragRowType == 0 )
}

bool CExtGridWnd::OnGbwProcessVkBeginEdit( UINT nChar )
{
	ASSERT_VALID( this );
	if( nChar != VK_RETURN )
		return false;
DWORD dwBseStyle = BseGetStyle();
	if( (dwBseStyle&__EGWS_BSE_EDIT_RETURN_CLICK) == 0 )
		return false;
	EditCell();
	return true;
}

void CExtGridWnd::OnDataProviderSortEnter(
	bool bColumns // true = sorting/swapping columns, false - rows
	)
{
	ASSERT_VALID( this );
	bColumns;
}

void CExtGridWnd::OnDataProviderSortLeave(
	bool bColumns // true = sorting/swapping columns, false - rows
	)
{
	ASSERT_VALID( this );
	bColumns;
}

void CExtGridWnd::OnDataProviderSwapSeries(
	bool bColumns, // true = sorting/swapping columns, false - rows
	LONG nRowColNo1,
	LONG nRowColNo2,
	LONG nSwapCounter
	)
{
	ASSERT_VALID( this );
INT nAreaCount = SelectionGetAreaCount();
	ASSERT( nAreaCount >= 0L );
CPoint ptFocus = FocusGet();
	if( bColumns )
	{
		ULONG nOffset = 0L;
		OnGridQueryDataProvider().CacheReservedCountsGet(
			&nOffset,
			NULL
			);
		nRowColNo1 -= LONG(nOffset);
		nRowColNo2 -= LONG(nOffset);
		if( ptFocus.x >= 0 )
		{
			if( ptFocus.x == nRowColNo1 )
			{
				ptFocus.x = nRowColNo2;
				FocusSet( ptFocus, false, false, false, false );
			} // if( ptFocus.x == nRowColNo1 )
			else if( ptFocus.x == nRowColNo2 )
			{
				ptFocus.x = nRowColNo1;
				FocusSet( ptFocus, false, false, false, false );
			} // else if( ptFocus.x == nRowColNo2 )
		} // if( ptFocus.x >= 0 )
		if( nSwapCounter <= 0L )
		{
			SelectionSplitAllAreas( true, false );
			nAreaCount = SelectionGetAreaCount(); // may be changed after split
			ASSERT( nAreaCount >= 0L );
		} // if( nSwapCounter <= 0L )
		if( nAreaCount > 0L )
		{
			for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
			{
				CRect rcSelectionOld( SelectionGet( false, nAreaNumber ) );
				if( rcSelectionOld.left != rcSelectionOld.right )
					continue;
				if( rcSelectionOld.left == nRowColNo1 )
				{
					CRect rcSelectionNew(
						nRowColNo2,
						rcSelectionOld.top,
						nRowColNo2,
						rcSelectionOld.bottom
						);
					VERIFY( SelectionSetAt( nAreaNumber, rcSelectionNew, false ) );
				} // if( rcSelectionOld.left == nRowColNo1 )
				else if( rcSelectionOld.left == nRowColNo2 )
				{
					CRect rcSelectionNew(
						nRowColNo1,
						rcSelectionOld.top,
						nRowColNo1,
						rcSelectionOld.bottom
						);
					VERIFY( SelectionSetAt( nAreaNumber, rcSelectionNew, false ) );
				} // else if( rcSelectionOld.left == nRowColNo2 )
			} // for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
		} // if( nAreaCount > 0L )
	} // if( bColumns )
	else
	{
		ULONG nOffset = 0L;
		OnGridQueryDataProvider().CacheReservedCountsGet(
			NULL,
			&nOffset
			);
		nRowColNo1 -= LONG(nOffset);
		nRowColNo2 -= LONG(nOffset);
		if( ptFocus.y >= 0 )
		{
			if( ptFocus.y == nRowColNo1 )
			{
				ptFocus.y = nRowColNo2;
				FocusSet( ptFocus, false, false, false, false );
			} // if( ptFocus.y == nRowColNo1 )
			else if( ptFocus.y == nRowColNo2 )
			{
				ptFocus.y = nRowColNo1;
				FocusSet( ptFocus, false, false, false, false );
			} // else if( ptFocus.y == nRowColNo2 )
		} // if( ptFocus.y >= 0 )
		if( nSwapCounter <= 0L )
		{
			SelectionSplitAllAreas( false, true );
			nAreaCount = SelectionGetAreaCount(); // may be changed after split
			ASSERT( nAreaCount >= 0L );
		} // if( nSwapCounter <= 0L )
		if( nAreaCount > 0L )
		{
			for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
			{
				CRect rcSelectionOld( SelectionGet( false, nAreaNumber ) );
				if( rcSelectionOld.top != rcSelectionOld.bottom )
					continue;
				if( rcSelectionOld.top == nRowColNo1 )
				{
					CRect rcSelectionNew(
						rcSelectionOld.left,
						nRowColNo2,
						rcSelectionOld.right,
						nRowColNo2
						);
					VERIFY( SelectionSetAt( nAreaNumber, rcSelectionNew, false ) );
				} // if( rcSelectionOld.top == nRowColNo1 )
				else if( rcSelectionOld.top == nRowColNo2 )
				{
					CRect rcSelectionNew(
						rcSelectionOld.left,
						nRowColNo1,
						rcSelectionOld.right,
						nRowColNo1
						);
					VERIFY( SelectionSetAt( nAreaNumber, rcSelectionNew, false ) );
				} // else if( rcSelectionOld.top == nRowColNo2 )
			} // for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
		} // if( nAreaCount > 0L )
	} // else from if( bColumns )
}

void CExtGridWnd::OnDataProviderSwapDroppedSeries(
	bool bColumns, // true = swapping columns, false - rows
	LONG nRowColNoSrc,
	LONG nRowColNoDropBefore
	)
{
	ASSERT_VALID( this );
	if(		nRowColNoSrc == nRowColNoDropBefore
		||	(nRowColNoSrc+1L) == nRowColNoDropBefore
		)
		return;
INT nAreaCount = SelectionGetAreaCount();
	ASSERT( nAreaCount >= 0L );
CPoint ptFocus = FocusGet();
	if( bColumns )
	{
		ULONG nOffset = 0L;
		OnGridQueryDataProvider().CacheReservedCountsGet(
			&nOffset,
			NULL
			);
		nRowColNoSrc -= LONG(nOffset);
		nRowColNoDropBefore -= LONG(nOffset);
		if( ptFocus.x >= 0 )
		{
			if( nRowColNoSrc < nRowColNoDropBefore )
			{
				if(		ptFocus.x > nRowColNoSrc
					&&	ptFocus.x < nRowColNoDropBefore
					)
				{
					ptFocus.x --;
					FocusSet( ptFocus, false, false, false, false );
				}
				else if( ptFocus.x == nRowColNoSrc )
				{
					ptFocus.x = nRowColNoDropBefore - 1L;
					FocusSet( ptFocus, false, false, false, false );
				}
			} // if( nRowColNoSrc < nRowColNoDropBefore )
			else
			{
				ASSERT( nRowColNoSrc > nRowColNoDropBefore );
				if(		ptFocus.x < nRowColNoSrc
					&&	ptFocus.x >= nRowColNoDropBefore
					)
				{
					ptFocus.x ++;
					FocusSet( ptFocus, false, false, false, false );
				}
				else if( ptFocus.x == nRowColNoSrc )
				{
					ptFocus.x = nRowColNoDropBefore;
					FocusSet( ptFocus, false, false, false, false );
				}
			} // else from if( nRowColNoSrc < nRowColNoDropBefore )
		} // if( ptFocus.x >= 0 )
		if( nAreaCount > 0L )
		{
			SelectionSplitAllAreas( true, false );
			nAreaCount = SelectionGetAreaCount(); // may be changed after split
			ASSERT( nAreaCount >= 0L );
			for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
			{
				CRect rcSelection( SelectionGet( false, nAreaNumber ) );
				if( rcSelection.left != rcSelection.right )
					continue;
				if( nRowColNoSrc < nRowColNoDropBefore )
				{
					if(		rcSelection.left > nRowColNoSrc
						&&	rcSelection.left < nRowColNoDropBefore
						)
					{
						rcSelection.left --;
						rcSelection.right --;
						VERIFY( SelectionSetAt( nAreaNumber, rcSelection, false ) );
					}
					else if( rcSelection.left == nRowColNoSrc )
					{
						rcSelection.left = rcSelection.right = nRowColNoDropBefore - 1L;
						VERIFY( SelectionSetAt( nAreaNumber, rcSelection, false ) );
					}
				} // if( nRowColNoSrc < nRowColNoDropBefore )
				else
				{
					ASSERT( nRowColNoSrc > nRowColNoDropBefore );
					if(		rcSelection.left < nRowColNoSrc
						&&	rcSelection.left >= nRowColNoDropBefore
						)
					{
						rcSelection.left ++;
						rcSelection.right ++;
						VERIFY( SelectionSetAt( nAreaNumber, rcSelection, false ) );
					}
					else if( rcSelection.left == nRowColNoSrc )
					{
						rcSelection.left = rcSelection.right = nRowColNoDropBefore;
						VERIFY( SelectionSetAt( nAreaNumber, rcSelection, false ) );
					}
				} // else from if( nRowColNoSrc < nRowColNoDropBefore )
			} // for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
		} // if( nAreaCount > 0L )
	} // if( bColumns )
	else
	{
		ULONG nOffset = 0L;
		OnGridQueryDataProvider().CacheReservedCountsGet(
			NULL,
			&nOffset
			);
		nRowColNoSrc -= LONG(nOffset);
		nRowColNoDropBefore -= LONG(nOffset);
		if( ptFocus.y >= 0 )
		{
			if( nRowColNoSrc < nRowColNoDropBefore )
			{
				if(		ptFocus.y > nRowColNoSrc
					&&	ptFocus.y < nRowColNoDropBefore
					)
				{
					ptFocus.y --;
					FocusSet( ptFocus, false, false, false, false );
				}
				else if( ptFocus.y == nRowColNoSrc )
				{
					ptFocus.y = nRowColNoDropBefore - 1L;
					FocusSet( ptFocus, false, false, false, false );
				}
			} // if( nRowColNoSrc < nRowColNoDropBefore )
			else
			{
				ASSERT( nRowColNoSrc > nRowColNoDropBefore );
				if(		ptFocus.y < nRowColNoSrc
					&&	ptFocus.y >= nRowColNoDropBefore
					)
				{
					ptFocus.y ++;
					FocusSet( ptFocus, false, false, false, false );
				}
				else if( ptFocus.y == nRowColNoSrc )
				{
					ptFocus.y = nRowColNoDropBefore;
					FocusSet( ptFocus, false, false, false, false );
				}
			} // else from if( nRowColNoSrc < nRowColNoDropBefore )
		} // if( ptFocus.y >= 0 )
		if( nAreaCount > 0L )
		{
			SelectionSplitAllAreas( false, true );
			nAreaCount = SelectionGetAreaCount(); // may be changed after split
			ASSERT( nAreaCount >= 0L );
			for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
			{
				CRect rcSelection( SelectionGet( false, nAreaNumber ) );
				if( rcSelection.top != rcSelection.bottom )
					continue;
				if( nRowColNoSrc < nRowColNoDropBefore )
				{
					if(		rcSelection.top > nRowColNoSrc
						&&	rcSelection.top < nRowColNoDropBefore
						)
					{
						rcSelection.top --;
						rcSelection.bottom --;
						VERIFY( SelectionSetAt( nAreaNumber, rcSelection, false ) );
					}
					else if( rcSelection.top == nRowColNoSrc )
					{
						rcSelection.top = rcSelection.bottom = nRowColNoDropBefore - 1L;
						VERIFY( SelectionSetAt( nAreaNumber, rcSelection, false ) );
					}
				} // if( nRowColNoSrc < nRowColNoDropBefore )
				else
				{
					ASSERT( nRowColNoSrc > nRowColNoDropBefore );
					if(		rcSelection.top < nRowColNoSrc
						&&	rcSelection.top >= nRowColNoDropBefore
						)
					{
						rcSelection.top ++;
						rcSelection.bottom ++;
						VERIFY( SelectionSetAt( nAreaNumber, rcSelection, false ) );
					}
					else if( rcSelection.top == nRowColNoSrc )
					{
						rcSelection.top = rcSelection.bottom = nRowColNoDropBefore;
						VERIFY( SelectionSetAt( nAreaNumber, rcSelection, false ) );
					}
				} // else from if( nRowColNoSrc < nRowColNoDropBefore )
			} // for( INT nAreaNumber = 0; nAreaNumber < nAreaCount; nAreaNumber++ )
		} // if( nAreaCount > 0L )
	} // else from if( bColumns )
}

void CExtGridWnd::OnGridSyncCellSortArrows(
	bool bSortColumns,
	const CExtGridDataSortOrder & _gdso,
	bool bSetup
	)
{
	ASSERT_VALID( this );
LONG nCount = _gdso.m_arrItems.GetSize();
	if( nCount == 0L )
		return;
LONG nOuterCountBefore = bSortColumns
		? OuterColumnCountLeftGet()
		: OuterRowCountTopGet();
LONG nOuterCountAfter = bSortColumns
		? OuterColumnCountRightGet()
		: OuterRowCountBottomGet();
	if( nOuterCountBefore == 0L && nOuterCountAfter == 0L )
		return;
	for( LONG i = 0; i < nCount; i++ )
	{
		const CExtGridDataSortOrder::ITEM_INFO & _soii =
			_gdso.m_arrItems[ i ];
		ASSERT( _soii.m_nRowColNo >= 0L );
		for( LONG nOuterIdx = 0L; nOuterIdx < nOuterCountBefore; nOuterIdx++ )
		{
			if( bSortColumns )
			{
				ASSERT( _soii.m_nRowColNo < RowCountGet() );
				CExtGridCell * pCell =
					GridCellGetOuterAtLeft(
						nOuterIdx,
						_soii.m_nRowColNo
						);
				if( pCell == NULL )
					continue;
				ASSERT_VALID( pCell );
				pCell->ModifyStyle( 0L, __EGCS_SORT_ARROW|__EGCS_SORT_DESCENDING );
				if( bSetup )
				{
					pCell->ModifyStyle( __EGCS_SORT_ARROW );
					if( ! _soii.m_bAscending )
						pCell->ModifyStyle( __EGCS_SORT_DESCENDING );
				} // if( bSetup )
			} // if( bSortColumns )
			else
			{
				ASSERT( _soii.m_nRowColNo < ColumnCountGet() );
				CExtGridCell * pCell =
					GridCellGetOuterAtTop(
						_soii.m_nRowColNo,
						nOuterIdx
						);
				if( pCell == NULL )
					continue;
				ASSERT_VALID( pCell );
				pCell->ModifyStyle( 0L, __EGCS_SORT_ARROW|__EGCS_SORT_DESCENDING );
				if( bSetup )
				{
					pCell->ModifyStyle( __EGCS_SORT_ARROW );
					if( ! _soii.m_bAscending )
						pCell->ModifyStyle( __EGCS_SORT_DESCENDING );
				} // if( bSetup )
			} // else from if( bSortColumns )
		} // for( LONG nOuterIdx = 0L; nOuterIdx < nOuterCountBefore; nOuterIdx++ )
		for( nOuterIdx = 0L; nOuterIdx < nOuterCountAfter; nOuterIdx++ )
		{
			if( bSortColumns )
			{
				ASSERT( _soii.m_nRowColNo < RowCountGet() );
				CExtGridCell * pCell =
					GridCellGetOuterAtRight(
						nOuterIdx,
						_soii.m_nRowColNo
						);
				if( pCell == NULL )
					continue;
				ASSERT_VALID( pCell );
				pCell->ModifyStyle( 0L, __EGCS_SORT_ARROW|__EGCS_SORT_DESCENDING );
				if( bSetup )
				{
					pCell->ModifyStyle( __EGCS_SORT_ARROW );
					if( ! _soii.m_bAscending )
						pCell->ModifyStyle( __EGCS_SORT_DESCENDING );
				} // if( bSetup )
			} // if( bSortColumns )
			else
			{
				ASSERT( _soii.m_nRowColNo < ColumnCountGet() );
				CExtGridCell * pCell =
					GridCellGetOuterAtBottom(
						_soii.m_nRowColNo,
						nOuterIdx
						);
				if( pCell == NULL )
					continue;
				ASSERT_VALID( pCell );
				pCell->ModifyStyle( 0L, __EGCS_SORT_ARROW|__EGCS_SORT_DESCENDING );
				if( bSetup )
				{
					pCell->ModifyStyle( __EGCS_SORT_ARROW );
					if( ! _soii.m_bAscending )
						pCell->ModifyStyle( __EGCS_SORT_DESCENDING );
				} // if( bSetup )
			} // else from if( bSortColumns )
		} // for( nOuterIdx = 0L; nOuterIdx < nOuterCountAfter; nOuterIdx++ )
	} // for( LONG i = 0; i < nCount; i++ )
}

bool CExtGridWnd::GridSortOrderGet(
	bool bSortColumns,
	CExtGridDataSortOrder & _gdsoCurrent
	) const
{
	ASSERT_VALID( this );
	_gdsoCurrent.Empty();
const CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	if( ! _DataProvider.SortOrderGet( _gdsoCurrent, bSortColumns ) )
	{
		_gdsoCurrent.Empty();
		return false;
	} // if( ! _DataProvider.SortOrderGet( _gdsoCurrent, bSortColumns ) )
ULONG nReserved = 0L;
	_DataProvider.CacheReservedCountsGet(
		bSortColumns ? NULL : (&nReserved),
		bSortColumns ? (&nReserved) : NULL
		);
	_gdsoCurrent.UpdateIndices( 0, nReserved, false );
	return true;
}

bool CExtGridWnd::GridSortOrderSetup(
	bool bSortColumns,
	const CExtGridDataSortOrder & _gdsoUpdate,
	bool bUpdateExistingSortOrder, // = false
	bool bInvertIntersectionSortOrder, // = true
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( ! _gdsoUpdate.ItemsUnique() )
	{
		// items inside sort order must be unique
		ASSERT( FALSE );
		return false;
	}
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
ULONG nReserved = 0L;
	_DataProvider.CacheReservedCountsGet(
		bSortColumns ? NULL : (&nReserved),
		bSortColumns ? (&nReserved) : NULL
		);
CExtGridDataSortOrder _gdsoDP;
	if( ! GridSortOrderGet(bSortColumns,_gdsoDP) )
		return false;
	ASSERT( _gdsoDP.ItemsUnique() );
	if( ! bUpdateExistingSortOrder )
	{
		CExtGridDataSortOrder _gdsoSetup( _gdsoUpdate );
		if(		bInvertIntersectionSortOrder
			&&	_gdsoDP.m_arrItems.GetSize() == 1
			&&	_gdsoSetup.m_arrItems.GetSize() == 1
			)
		{
			const CExtGridDataSortOrder::ITEM_INFO & _soiiOld =
				_gdsoDP.m_arrItems[ 0 ];
			CExtGridDataSortOrder::ITEM_INFO _soiiNew(
				_gdsoSetup.m_arrItems[ 0 ] );
			if( _soiiOld.m_nRowColNo == _soiiNew.m_nRowColNo )
			{
				_soiiNew.m_bAscending = (! _soiiOld.m_bAscending);
				_gdsoSetup.m_arrItems.SetAt( 0, _soiiNew );
			}
		}
		_gdsoSetup.UpdateIndices( 0, nReserved, true );
		bool bRetVal =
			_DataProvider.SortOrderSet(
				_gdsoSetup,
				bSortColumns,
				this
				);
		if( bRetVal )
		{
			_gdsoSetup.UpdateIndices( 0, nReserved, false );
			OnGridSyncCellSortArrows(
				bSortColumns,
				_gdsoDP,
				false
				);
			OnGridSyncCellSortArrows(
				bSortColumns,
				_gdsoSetup,
				true
				);
		} // if( bRetVal )
		if( bRedraw )
		{
			OnSwUpdateScrollBars();
			OnSwDoRedraw();
		} // if( bRedraw )
		return bRetVal;
	} // if( ! bUpdateExistingSortOrder )
CExtGridDataSortOrder _gdsoModified( _gdsoDP );
	_gdsoModified.SetupOrder(
		_gdsoUpdate,
		bInvertIntersectionSortOrder
		);
	ASSERT( _gdsoModified.ItemsUnique() );
	_gdsoModified.UpdateIndices( 0, nReserved, true );
bool bRetVal =
		_DataProvider.SortOrderSet(
			_gdsoModified,
			bSortColumns,
			this
			);
	if( bRetVal )
	{
		_gdsoModified.UpdateIndices( 0, nReserved, false );
		OnGridSyncCellSortArrows(
			bSortColumns,
			_gdsoDP,
			false
			);
		OnGridSyncCellSortArrows(
			bSortColumns,
			_gdsoModified,
			true
			);
	} // if( bRetVal )
	if( bRedraw )
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	} // if( bRedraw )
	return bRetVal;
}

bool CExtGridWnd::GridColumnDefaultValueBind(
	LONG nColNo, // = -1L // if -1L - default value for all columns
	INT nColType, // = 0
	CExtGridCell * pCell // = NULL // if NULL - remove default value
	)
{
	ASSERT_VALID( this );
LONG nColNoEffective = nColNo;
	if( nColNo >= 0L )
	{
		if( nColType < 0 )
		{
			LONG nCountBefore = OuterColumnCountLeftGet();
			if( nColNo >= nCountBefore )
			{
				ASSERT( FALSE );
				return false;
			} // if( nColNo >= nCountBefore )
		} // if( nColType < 0 )
		else if( nColType > 0 )
		{
			LONG nCountAfter = OuterColumnCountRightGet();
			if( nColNo >= nCountAfter )
			{
				ASSERT( FALSE );
				return false;
			} // if( nColNo >= nCountAfter )
			LONG nCountBefore = OuterColumnCountLeftGet();
			nColNoEffective += nCountBefore;
		} // else if( nColType > 0 )
		else
		{
			ASSERT( nColType == 0 );
			LONG nCount = ColumnCountGet();
			if( nColNo >= nCount )
			{
				ASSERT( FALSE );
				return false;
			} // if( nColNo >= nCount )
			LONG nCountBefore = OuterColumnCountLeftGet();
			LONG nCountAfter = OuterColumnCountRightGet();
			nColNoEffective += nCountBefore + nCountAfter;
		} // nColType == 0
	} // if( nColNo >= 0L )
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	return
		_DataProvider.ColumnDefaultValueBind(
			ULONG(nColNoEffective),
			pCell
			);
}

bool CExtGridWnd::GridRowDefaultValueBind(
	LONG nRowNo, // = -1L // if -1L - default value for all rows
	INT nRowType, // = 0
	CExtGridCell * pCell // = NULL // if NULL - remove default value
	)
{
	ASSERT_VALID( this );
LONG nRowNoEffective = nRowNo;
	if( nRowNo >= 0L )
	{
		if( nRowType < 0 )
		{
			LONG nCountBefore = OuterRowCountTopGet();
			if( nRowNo >= nCountBefore )
			{
				ASSERT( FALSE );
				return false;
			} // if( nRowNo >= nCountBefore )
		} // if( nRowType < 0 )
		else if( nRowType > 0 )
		{
			LONG nCountAfter = OuterRowCountBottomGet();
			if( nRowNo >= nCountAfter )
			{
				ASSERT( FALSE );
				return false;
			} // if( nRowNo >= nCountAfter )
			LONG nCountBefore = OuterRowCountTopGet();
			nRowNoEffective += nCountBefore;
		} // else if( nRowType > 0 )
		else
		{
			ASSERT( nRowType == 0 );
			LONG nCount = RowCountGet();
			if( nRowNo >= nCount )
			{
				ASSERT( FALSE );
				return false;
			} // if( nRowNo >= nCount )
			LONG nCountBefore = OuterRowCountTopGet();
			LONG nCountAfter = OuterRowCountBottomGet();
			nRowNoEffective += nCountBefore + nCountAfter;
		} // nRowType == 0
	} // if( nRowNo >= 0L )
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	return
		_DataProvider.RowDefaultValueBind(
			ULONG(nRowNoEffective),
			pCell
			);
}

bool CExtGridWnd::GridColumnDefaultValueUnbind(
	LONG nColNo, // = -1L // if -1L - default value for all columns
	INT nColType // = 0
	)
{
	ASSERT_VALID( this );
LONG nColNoEffective = nColNo;
	if( nColNo >= 0L )
	{
		if( nColType < 0 )
		{
			LONG nCountBefore = OuterColumnCountLeftGet();
			if( nColNo >= nCountBefore )
			{
				ASSERT( FALSE );
				return false;
			} // if( nColNo >= nCountBefore )
		} // if( nColType < 0 )
		else if( nColType > 0 )
		{
			LONG nCountAfter = OuterColumnCountRightGet();
			if( nColNo >= nCountAfter )
			{
				ASSERT( FALSE );
				return false;
			} // if( nColNo >= nCountAfter )
			LONG nCountBefore = OuterColumnCountLeftGet();
			nColNoEffective += nCountBefore;
		} // else if( nColType > 0 )
		else
		{
			ASSERT( nColType == 0 );
			LONG nCount = ColumnCountGet();
			if( nColNo >= nCount )
			{
				ASSERT( FALSE );
				return false;
			} // if( nColNo >= nCount )
			LONG nCountBefore = OuterColumnCountLeftGet();
			LONG nCountAfter = OuterColumnCountRightGet();
			nColNoEffective += nCountBefore + nCountAfter;
		} // nColType == 0
	} // if( nColNo >= 0L )
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	return
		_DataProvider.ColumnDefaultValueUnbind(
			ULONG(nColNoEffective)
			);
}

bool CExtGridWnd::GridRowDefaultValueUnbind(
	LONG nRowNo, // = -1L // if -1L - default value for all rows
	INT nRowType // = 0
	)
{
	ASSERT_VALID( this );
LONG nRowNoEffective = nRowNo;
	if( nRowNo >= 0L )
	{
		if( nRowType < 0 )
		{
			LONG nCountBefore = OuterRowCountTopGet();
			if( nRowNo >= nCountBefore )
			{
				ASSERT( FALSE );
				return false;
			} // if( nRowNo >= nCountBefore )
		} // if( nRowType < 0 )
		else if( nRowType > 0 )
		{
			LONG nCountAfter = OuterRowCountBottomGet();
			if( nRowNo >= nCountAfter )
			{
				ASSERT( FALSE );
				return false;
			} // if( nRowNo >= nCountAfter )
			LONG nCountBefore = OuterRowCountTopGet();
			nRowNoEffective += nCountBefore;
		} // else if( nRowType > 0 )
		else
		{
			ASSERT( nRowType == 0 );
			LONG nCount = RowCountGet();
			if( nRowNo >= nCount )
			{
				ASSERT( FALSE );
				return false;
			} // if( nRowNo >= nCount )
			LONG nCountBefore = OuterRowCountTopGet();
			LONG nCountAfter = OuterRowCountBottomGet();
			nRowNoEffective += nCountBefore + nCountAfter;
		} // nRowType == 0
	} // if( nRowNo >= 0L )
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	return
		_DataProvider.RowDefaultValueUnbind(
			ULONG(nRowNoEffective)
			);
}

void CExtGridWnd::GridColumnDefaultValueUnbindAll()
{
	ASSERT_VALID( this );
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	_DataProvider.ColumnDefaultValueUnbindAll();
}

void CExtGridWnd::GridRowDefaultValueUnbindAll()
{
	ASSERT_VALID( this );
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	_DataProvider.RowDefaultValueUnbindAll();
}

CExtGridCell * CExtGridWnd::GridColumnDefaultValueGet(
	LONG nColNo,
	INT nColType // = 0
	)
{
	ASSERT_VALID( this );
LONG nColNoEffective = nColNo;
	if( nColNo >= 0L )
	{
		if( nColType < 0 )
		{
			LONG nCountBefore = OuterColumnCountLeftGet();
			if( nColNo >= nCountBefore )
			{
				ASSERT( FALSE );
				return false;
			} // if( nColNo >= nCountBefore )
		} // if( nColType < 0 )
		else if( nColType > 0 )
		{
			LONG nCountAfter = OuterColumnCountRightGet();
			if( nColNo >= nCountAfter )
			{
				ASSERT( FALSE );
				return false;
			} // if( nColNo >= nCountAfter )
			LONG nCountBefore = OuterColumnCountLeftGet();
			nColNoEffective += nCountBefore;
		} // else if( nColType > 0 )
		else
		{
			ASSERT( nColType == 0 );
			LONG nCount = ColumnCountGet();
			if( nColNo >= nCount )
			{
				ASSERT( FALSE );
				return false;
			} // if( nColNo >= nCount )
			LONG nCountBefore = OuterColumnCountLeftGet();
			LONG nCountAfter = OuterColumnCountRightGet();
			nColNoEffective += nCountBefore + nCountAfter;
		} // nColType == 0
	} // if( nColNo >= 0L )
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	return
		_DataProvider.ColumnDefaultValueGet(
			ULONG(nColNoEffective)
			);
}

const CExtGridCell * CExtGridWnd::GridColumnDefaultValueGet(
	LONG nColNo,
	INT nColType // = 0
	) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridWnd * > ( this ) ) ->
			GridColumnDefaultValueGet( nColNo, nColType );
}

CExtGridCell * CExtGridWnd::GridRowDefaultValueGet(
	LONG nRowNo,
	INT nRowType // = 0
	)
{
	ASSERT_VALID( this );
LONG nRowNoEffective = nRowNo;
	if( nRowNo >= 0L )
	{
		if( nRowType < 0 )
		{
			LONG nCountBefore = OuterRowCountTopGet();
			if( nRowNo >= nCountBefore )
			{
				ASSERT( FALSE );
				return false;
			} // if( nRowNo >= nCountBefore )
		} // if( nRowType < 0 )
		else if( nRowType > 0 )
		{
			LONG nCountAfter = OuterRowCountBottomGet();
			if( nRowNo >= nCountAfter )
			{
				ASSERT( FALSE );
				return false;
			} // if( nRowNo >= nCountAfter )
			LONG nCountBefore = OuterRowCountTopGet();
			nRowNoEffective += nCountBefore;
		} // else if( nRowType > 0 )
		else
		{
			ASSERT( nRowType == 0 );
			LONG nCount = RowCountGet();
			if( nRowNo >= nCount )
			{
				ASSERT( FALSE );
				return false;
			} // if( nRowNo >= nCount )
			LONG nCountBefore = OuterRowCountTopGet();
			LONG nCountAfter = OuterRowCountBottomGet();
			nRowNoEffective += nCountBefore + nCountAfter;
		} // nRowType == 0
	} // if( nRowNo >= 0L )
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	return
		_DataProvider.RowDefaultValueGet(
			ULONG(nRowNoEffective)
			);
}

const CExtGridCell * CExtGridWnd::GridRowDefaultValueGet(
	LONG nRowNo,
	INT nRowType // = 0
	) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtGridWnd * > ( this ) ) ->
			GridRowDefaultValueGet( nRowNo, nRowType );
}

INT CExtGridWnd::GridIconGetCount() const
{
	ASSERT_VALID( this );
const CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	return _DataProvider.IconGetCount();
}

CExtCmdIcon * CExtGridWnd::GridIconGetAt( INT nIdx )
{
	ASSERT_VALID( this );
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	return _DataProvider.IconGetAt( nIdx );
}

const CExtCmdIcon * CExtGridWnd::GridIconGetAt( INT nIdx ) const
{
	ASSERT_VALID( this );
const CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	return _DataProvider.IconGetAt( nIdx );
}

INT CExtGridWnd::GridIconInsert( // returns index or -1
	CExtCmdIcon * pIcon,
	INT nIdx, // = -1 // append
	bool bCopyIcon // = true
	)
{
	ASSERT_VALID( this );
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	return _DataProvider.IconInsert( pIcon, nIdx, bCopyIcon );
}

INT CExtGridWnd::GridIconInsert( // returns index or -1
	const CExtCmdIcon * pIcon,
	INT nIdx, // = -1 // append
	bool bCopyIcon // = true
	)
{
	ASSERT_VALID( this );
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	return _DataProvider.IconInsert( pIcon, nIdx, bCopyIcon );
}

INT CExtGridWnd::GridIconRemove(
	INT nIdx, // = 0
	INT nCountToRemove // = -1 // all
	)
{
	ASSERT_VALID( this );
CExtGridDataProvider & _DataProvider = OnGridQueryDataProvider();
	return _DataProvider.IconRemove( nIdx, nCountToRemove );
}

#endif // (!defined __EXT_MFC_NO_GRIDWND)

#endif // (!defined __EXT_MFC_NO_GRIDBASEWND)
