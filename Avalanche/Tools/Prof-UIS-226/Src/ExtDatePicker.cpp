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

#if (!defined __EXT_MFC_NO_DATE_PICKER)

#if (!defined __EXT_DATE_PICKER_H)
	#include <ExtDatePicker.h>
#endif // (!defined __EXT_DATE_PICKER_H)

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __ExtCmdManager_H)
	#include <ExtCmdManager.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#include <math.h>

#include <../profuisdll/resource.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CExtDatePickerHeaderPopupWnd
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CExtDatePickerHeaderPopupWnd, CWnd );

CExtDatePickerHeaderPopupWnd::CExtDatePickerHeaderPopupWnd(
	CExtDatePickerWnd * pDPW,
	bool bUseShadow
	)
	: m_pDPW( NULL )
	, m_nMonth( 0 )
	, m_nYear( 0 )
	, m_nItemsPerPage( 7 )
	, m_nSelMonth( 0 )
	, m_nSelYear( 0 )
	, m_sizeDesiredSize( CSize( 0, 0 ) )
	, m_sizeItemSize( CSize( 0, 0 ) )
	, m_bUseShadow( bUseShadow )
	, m_bScrollingUp( false )
	, m_bScrollingDown( false )
	, m_nLastElapseTimerUp( 0 )
	, m_nLastElapseTimerDown( 0 )
{
	ASSERT( pDPW != NULL );
	m_pDPW = pDPW;
	_CalcSize();
}

CExtDatePickerHeaderPopupWnd::~CExtDatePickerHeaderPopupWnd()
{
}

BOOL CExtDatePickerHeaderPopupWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	if( ! CWnd::PreCreateWindow(cs) )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	cs.cx = m_sizeDesiredSize.cx;
	cs.cy = m_sizeDesiredSize.cy;
	return TRUE;
}

void CExtDatePickerHeaderPopupWnd::PostNcDestroy()
{
	delete this;
}

void CExtDatePickerHeaderPopupWnd::SetInitialDate( INT nMonth, INT nYear )
{
	ASSERT_VALID( this );
	ASSERT(
			nYear >= 100
		&&	nYear <= 9999
		&&	nMonth >= 1
		&&	nMonth <= 12
		);
	if( nMonth >= 1 && nMonth <= 12 )
		m_nMonth = nMonth;
	if( nYear >= 100 && nYear <= 9999 )
		m_nYear = nYear;
}

bool CExtDatePickerHeaderPopupWnd::GetSelectedDate( INT & nMonth, INT & nYear ) const
{
	ASSERT_VALID( this );
	ASSERT( m_hWnd != NULL && ::IsWindow(m_hWnd) );
	if( m_nSelMonth == 0 || m_nSelYear == 0 )
		return false;
	nMonth = m_nSelMonth;
	nYear = m_nSelYear;
	return true;
}

void CExtDatePickerHeaderPopupWnd::SetItemsPerPage( INT nItemsPerPage )
{
	ASSERT_VALID( this );
	ASSERT(
			nItemsPerPage >= 3
		&&	fmod( double(nItemsPerPage), 2.0 ) != 0.0
		);
	if(		nItemsPerPage >= 3
		&&	fmod( double(nItemsPerPage), 2.0 ) != 0.0
		)
		m_nItemsPerPage = nItemsPerPage;
}

CSize CExtDatePickerHeaderPopupWnd::GetSize(
	bool bOnlyClientArea // = false
	) const
{
	ASSERT_VALID( this );
CSize _size(
		m_sizeDesiredSize.cx -
			(	(m_bUseShadow && bOnlyClientArea)
					? CExtWndShadow::DEF_SHADOW_SIZE
					: 0
			),
		m_sizeDesiredSize.cy -
			(	(m_bUseShadow && bOnlyClientArea)
					? CExtWndShadow::DEF_SHADOW_SIZE
					: 0
			)
		);
	return _size;
}

UINT CExtDatePickerHeaderPopupWnd::GetScrollingSpeed( int nPixels )
{
	ASSERT_VALID( this );
double dAcceleration = floor( double(nPixels)/double(__EDPW_SCROLL_MONTH_LIST_ACCELERATION_STEP) );
INT nAcceleration = (INT)dAcceleration;
UINT nElapse = 0;
	switch( nAcceleration ) 
	{
	case 0:  nElapse = __EDPW_SCROLL_MONTH_LIST_TIMER_ELAPSE1; break;
	case 1:  nElapse = __EDPW_SCROLL_MONTH_LIST_TIMER_ELAPSE2; break;
	case 2:  nElapse = __EDPW_SCROLL_MONTH_LIST_TIMER_ELAPSE3; break;
	case 3:  nElapse = __EDPW_SCROLL_MONTH_LIST_TIMER_ELAPSE4; break;
	case 4:  nElapse = __EDPW_SCROLL_MONTH_LIST_TIMER_ELAPSE5; break;
	default: nElapse = __EDPW_SCROLL_MONTH_LIST_TIMER_ELAPSE5; break;
	}
	return nElapse;
}

void CExtDatePickerHeaderPopupWnd::AnalyzeChangings()
{
	ASSERT_VALID( this );
	ASSERT( m_hWnd != NULL && ::IsWindow(m_hWnd) );
POINT point;
	if( ! ::GetCursorPos(&point) )
		return;
	ScreenToClient( &point );
	if( point.y > 0 && point.y < m_sizeDesiredSize.cy )
	{
		if( m_bScrollingDown )
		{
			KillTimer( __EDPW_SCROLL_MONTH_LIST_DOWN_TIMER_ID );
			m_bScrollingDown = false;
		}
		if( m_bScrollingUp )
		{
			KillTimer( __EDPW_SCROLL_MONTH_LIST_UP_TIMER_ID );
			m_bScrollingUp = false;
		}
	} // if( point.y > 0 && point.y < m_sizeDesiredSize.cy )
INT nRow = _HitTest( point );
	m_nSelMonth = m_nSelYear = 0;
CSize sizeDesiredSize = GetSize( true );
	if( nRow > -1 )
	{
		INT nHalfCount = m_nItemsPerPage / 2;
		if( (nHalfCount * 2) > m_nItemsPerPage )
			nHalfCount--;
		INT nMonth = m_nMonth;
		INT nYear = m_nYear;
		for( INT i = 0; i < nHalfCount; i++ )
		{
			nMonth--;
			if( nMonth < 1 )
			{
				nMonth = 12;
				nYear--;
			}
		} // for( INT i = 0; i < nHalfCount; i++ )
		for( i = 0; i < m_nItemsPerPage; i++ )
		{
			if( nRow == i )
			{
				m_nSelMonth = nMonth;
				m_nSelYear = nYear;
				break;
			}
			nMonth++;
			if( nMonth > 12 )
			{
				nMonth = 1;
				nYear++;
			}
		} // for( i = 0; i < m_nItemsPerPage; i++ )
	} // if( nRow > -1 )
	else if( !(point.y > 0 && point.y < sizeDesiredSize.cy) )
	{
		if( point.y < 0 )
		{
			UINT nElapse = GetScrollingSpeed( abs(0 - point.y) );
			bool bNeedToResetTimer =
				(m_nLastElapseTimerUp != (UINT)nElapse);
			if( (!m_bScrollingUp) || bNeedToResetTimer )
			{
//				TRACE1(" * * * nElapse = %d\n",nElapse);
				if( bNeedToResetTimer )
					KillTimer( __EDPW_SCROLL_MONTH_LIST_UP_TIMER_ID );
				SetTimer( 
					__EDPW_SCROLL_MONTH_LIST_UP_TIMER_ID, 
					nElapse, 
					NULL 
					);
				m_nLastElapseTimerUp = nElapse;
				m_bScrollingUp = true;
			} // if( (!m_bScrollingUp) || bNeedToResetTimer )
		} // if( point.y < 0 )
		if( point.y > 0 )
		{
			UINT nElapse = GetScrollingSpeed( abs( point.y - sizeDesiredSize.cy ) );
			bool bNeedToResetTimer = (m_nLastElapseTimerDown != (UINT)nElapse);
			if( (!m_bScrollingDown) || bNeedToResetTimer )
			{
//				TRACE1(" * * * nElapse = %d\n",nElapse);
				if( bNeedToResetTimer )
					KillTimer( __EDPW_SCROLL_MONTH_LIST_UP_TIMER_ID );
				SetTimer( 
					__EDPW_SCROLL_MONTH_LIST_DOWN_TIMER_ID, 
					nElapse, 
					NULL 
					);
				m_nLastElapseTimerDown = nElapse;
				m_bScrollingDown = true;
			} // if( (!m_bScrollingDown) || bNeedToResetTimer )
		} // if( point.y > 0 m_bScrollingDown)
	} // else if( !(point.y > 0 && point.y < sizeDesiredSize.cy) )
	Invalidate( FALSE );
}

void CExtDatePickerHeaderPopupWnd::_CalcSize()
{
	ASSERT_VALID( this );
	ASSERT( m_pDPW != NULL );
	// determine the maximal width and height 
CWindowDC dc( NULL );
	for( INT nMonth = 1; nMonth <= 12; nMonth++ )
	{
		CExtSafeString sMonthName =
			m_pDPW->OnDatePickerGetMonthName( nMonth );
		sMonthName += _T(" 0000");
		CRect rcText = 
			CExtPaintManager::stat_CalcTextDimension(
			dc,
			*(m_pDPW->OnDatePickerQueryHeaderPopupFont()),
			sMonthName
			);
		if( m_sizeItemSize.cx < rcText.Width() )
			m_sizeItemSize.cx = rcText.Width();
		if( m_sizeItemSize.cy < rcText.Height() )
			m_sizeItemSize.cy = rcText.Height();
	} // for( INT nMonth = 1; nMonth <= 12; nMonth++ )
	m_sizeItemSize.cy += 2;
	m_sizeItemSize.cx += 2*12;
	m_sizeDesiredSize =
		CSize(
			m_sizeItemSize.cx + 1
				+ (m_bUseShadow ? CExtWndShadow::DEF_SHADOW_SIZE : 0 ),
			m_sizeItemSize.cy*m_nItemsPerPage + 2
				+ (m_bUseShadow ? CExtWndShadow::DEF_SHADOW_SIZE : 0 )
			);
}

INT CExtDatePickerHeaderPopupWnd::_HitTest( CPoint pt )
{
	ASSERT_VALID( this );
	for( INT i = 0; i<m_nItemsPerPage; i++ )
	{
		CRect rcItem( 
			0, 
			i*m_sizeItemSize.cy, 
			m_sizeItemSize.cx, 
			i*m_sizeItemSize.cy + m_sizeItemSize.cy
			);
		if( rcItem.PtInRect( pt ) )
			return i;
	}
	return -1;
}

BEGIN_MESSAGE_MAP(CExtDatePickerHeaderPopupWnd, CWnd)
	//{{AFX_MSG_MAP(CExtDatePickerHeaderPopupWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CExtDatePickerHeaderPopupWnd::OnPaint() 
{
	ASSERT_VALID( this );
	ASSERT( m_pDPW != NULL );
CPaintDC dcPaint( this );
CRect rcClient;
	GetClientRect( &rcClient );
	if( rcClient.IsRectEmpty() )
		return;
CExtMemoryDC dc( &dcPaint, &rcClient );
INT nHalfCount = m_nItemsPerPage / 2;
	if( (nHalfCount * 2) > m_nItemsPerPage )
		nHalfCount--;
INT nMonth = m_nMonth;
INT nYear = m_nYear;
	for( INT i = 0; i < nHalfCount; i++ )
	{
		nMonth--;
		if( nMonth < 1 )
		{
			nMonth = 12;
			nYear--;
		}
	} // for( INT i = 0; i < nHalfCount; i++ )
COLORREF clrBackColor = g_PaintManager->GetColor(COLOR_WINDOW);
	dc.FillSolidRect( rcClient, clrBackColor );
INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
COLORREF clrText = g_PaintManager->GetColor( COLOR_BTNTEXT );
COLORREF clrTextOld = dc.SetTextColor( clrText );
CFont * pOldFont = dc.SelectObject( m_pDPW->OnDatePickerQueryHeaderPopupFont() );
	for( i = 0; i < m_nItemsPerPage; i++ )
	{
		CExtSafeString sText;
		sText.Format(
			_T("%s %d"),
			m_pDPW->OnDatePickerGetMonthName( nMonth ),
			nYear
			);
		CRect rcItem( 
			0, 
			i*m_sizeItemSize.cy, 
			m_sizeItemSize.cx, 
			i*m_sizeItemSize.cy + m_sizeItemSize.cy
			);
		CRect rcText( rcItem );
		rcText.OffsetRect( 0, -1 );
		if(		m_nSelMonth != 0
			&&	m_nSelYear != 0
			&&	m_nSelMonth == nMonth 
			&&	m_nSelYear == nYear
			)
		{
			dc.FillSolidRect( rcItem, RGB(0,0,0) );
			COLORREF clrTextOld = dc.SetTextColor( RGB(255,255,255) );
			dc.DrawText(
				sText, 
				rcText,
				DT_CENTER|DT_VCENTER|DT_SINGLELINE
				);
			dc.SetTextColor( clrTextOld );
		}
		else
		{
			dc.DrawText(
				sText, 
				rcText,
				DT_CENTER|DT_VCENTER|DT_SINGLELINE
				);
		}
		nMonth++;
		if( nMonth > 12 )
		{
			nMonth = 1;
			nYear++;
		}
	} // for( i = 0; i < m_nItemsPerPage; i++ )
	dc.SelectObject( pOldFont );
	dc.SetTextColor( clrTextOld );
	dc.SetBkMode( nOldBkMode );
	g_PaintManager->OnPaintSessionComplete( this );
}

void CExtDatePickerHeaderPopupWnd::OnTimer(UINT nIDEvent) 
{
	switch( nIDEvent ) 
	{
	case __EDPW_SCROLL_MONTH_LIST_UP_TIMER_ID:
		m_nMonth--;
		if( m_nMonth < 1 )
		{
			m_nMonth = 12;
			m_nYear--;
		}
		break;
	case __EDPW_SCROLL_MONTH_LIST_DOWN_TIMER_ID:
		m_nMonth++;
		if( m_nMonth > 12 )
		{
			m_nMonth = 1;
			m_nYear++;
		}
		break;
	default:
		CWnd::OnTimer(nIDEvent);
	}
	Invalidate( FALSE );
}

LRESULT CExtDatePickerHeaderPopupWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_MOUSEACTIVATE )
	{
		if( (GetStyle() & WS_TABSTOP) != 0 )
			return MA_ACTIVATE;
		return MA_NOACTIVATE;
	}
	if( message == WM_ERASEBKGND )
		return 0L;
	if( message == WM_NCCALCSIZE )
	{
		NCCALCSIZE_PARAMS * pNCCSP =
			reinterpret_cast < NCCALCSIZE_PARAMS * > ( lParam );
		ASSERT( pNCCSP != NULL );
		CRect rcInBarWnd( pNCCSP->rgrc[0] );
		rcInBarWnd.DeflateRect(
			1,
			1,
			1 + (m_bUseShadow ? CExtWndShadow::DEF_SHADOW_SIZE : 0 ),
			1 + (m_bUseShadow ? CExtWndShadow::DEF_SHADOW_SIZE : 0 )
			);
		::CopyRect( &(pNCCSP->rgrc[0]), rcInBarWnd );
		return 0;
	}
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
		CRect rcBorder( rcInBarClient );
		rcBorder.InflateRect( 1, 1 );
		if( m_bUseShadow )
			m_shadow.Paint( dc, rcBorder );
		dc.FillSolidRect( &rcBorder, RGB(0,0,0) );
		
		return 0;
	}
LRESULT lResult = CWnd::WindowProc( message, wParam, lParam );
	return lResult;
}

/////////////////////////////////////////////////////////////////////////////
// CExtDatePickerWnd::MONTH_INFO
/////////////////////////////////////////////////////////////////////////////

CExtDatePickerWnd::MONTH_INFO::MONTH_INFO( 
	INT nRow, 
	INT nCol, 
	INT nMonth, 
	INT nYear, 
	CRect rcClient,
	CExtDatePickerWnd * pDPW
	)
	: m_pDPW( pDPW )
	, m_nRow( nRow )
	, m_nCol( nCol )
	, m_nMonth( nMonth )
	, m_nYear( nYear )
	, m_rcClient( rcClient )
	, m_rcScrollBtnBackward( 0, 0, 0, 0 )
	, m_rcScrollBtnForward( 0, 0, 0, 0 )
	, m_rcHeader( 0, 0, 0, 0 )
	, m_rcHeaderDate( 0, 0, 0, 0 )
	, m_rcHeaderWithoutBorders( 0, 0, 0, 0 )
	, m_rcDaysOfWeek( 0, 0, 0, 0 )
	, m_rcDaysCells( 0, 0, 0, 0 )
{
	ASSERT( m_pDPW != NULL );
DWORD dwDatePickerStyle = m_pDPW->GetDatePickerStyle();
INT nFirstDayOfWeek = m_pDPW->OnDatePickerGetFirstDayOfWeek();
	for( INT i = 0; i < 2; i++ )
	{
		// 1=Sunday, 2=Monday, and so on.
		nFirstDayOfWeek += 1;
		if( nFirstDayOfWeek > 7 ) 
			nFirstDayOfWeek = 1;
	} // for( INT i = 0; i < 2; i++ )
	COleDateTime dtFirstDay =
		COleDateTime( m_nYear, m_nMonth, 1, 0, 0, 0 );
	COleDateTime dtStart = dtFirstDay;
	if( dtFirstDay.GetDayOfWeek() == nFirstDayOfWeek )
		dtFirstDay -= 7;
	else
	{
		while( dtFirstDay.GetDayOfWeek() != nFirstDayOfWeek )
			dtFirstDay -= 1;
	} // else from if( dtFirstDay.GetDayOfWeek() == nFirstDayOfWeek )
	for( INT nWeek = 0; nWeek < 6; nWeek ++ )
	{
		for( INT nDay = 0; nDay < 7; nDay ++ )
		{
			m_aDays[nWeek][nDay].SetDatePickerWnd( m_pDPW, this );
			CSize sizeCalendarDimensionsCurrent;
			m_pDPW->DimGet( 
				NULL,
				NULL,
				&sizeCalendarDimensionsCurrent
				);
			if(		(	dtStart.GetMonth() == dtFirstDay.GetMonth()
					&&	dtStart.GetYear() == dtFirstDay.GetYear()
					)
				||	(	sizeCalendarDimensionsCurrent.cy-1 == m_nRow 
					&&	sizeCalendarDimensionsCurrent.cx-1 == m_nCol 
					&&  dtStart < dtFirstDay
					)
				||	(	m_nRow == 0 
					&&  m_nCol == 0
					&&  dtStart > dtFirstDay
					)
				||	(dwDatePickerStyle & __EDPWS_HIDE_INNER_NON_MONTH_DAYS) == 0
				)
			{
				m_aDays[nWeek][nDay].SetDate( dtFirstDay );
			}
			dtFirstDay += 1;
		} // for( INT nDay = 0; nDay < 7; nDay ++ )
	} // for( INT nWeek = 0; nWeek < 6; nWeek ++ )
	_RecalcLayout();
}

#ifdef _DEBUG
void CExtDatePickerWnd::MONTH_INFO::AssertValid() const
{
	CObject::AssertValid();
	ASSERT(
			m_nYear >= 100
		&&	m_nYear <= 9999
		&&	m_nMonth >= 1
		&&	m_nMonth <= 12
		);
	ASSERT(
			m_pDPW != NULL
		//&& ::IsWindow(m_pDPW->m_hWnd)
		);
	ASSERT_VALID( m_pDPW );
}

void CExtDatePickerWnd::MONTH_INFO::Dump( CDumpContext & dc) const
{
	CObject::Dump( dc );
}
#endif // _DEBUG

void CExtDatePickerWnd::MONTH_INFO::_RecalcLayout()
{
	m_rcScrollBtnBackward.SetRectEmpty();
	m_rcScrollBtnForward.SetRectEmpty();
	m_rcHeader.SetRectEmpty();
	m_rcHeaderDate.SetRectEmpty();
	m_rcHeaderWithoutBorders.SetRectEmpty();
	m_rcDaysCells.SetRectEmpty();
	m_rcDaysOfWeek.SetRectEmpty();
	// header's client area for displaying sel
	m_rcHeader.CopyRect( m_rcClient );
INT nMonthHeaderHeight = m_pDPW->OnDatePickerQueryMonthHeaderHeight();
	m_rcHeader.bottom = m_rcHeader.top + nMonthHeaderHeight;
	// header, but excluding border area
	m_rcHeaderWithoutBorders.CopyRect( m_rcHeader );
	m_rcHeaderWithoutBorders.DeflateRect( 1, 1 );
	// backward button
CSize sizeScrollButton = m_pDPW->OnDatePickerQueryScrollButtonSize();
	m_rcScrollBtnBackward.SetRect( 
		m_rcHeaderWithoutBorders.left,
		m_rcHeaderWithoutBorders.top,
		m_rcHeaderWithoutBorders.left + sizeScrollButton.cx,
		m_rcHeaderWithoutBorders.top + sizeScrollButton.cy
		);
	// forward button
	m_rcScrollBtnForward.SetRect( 
		m_rcHeaderWithoutBorders.right - sizeScrollButton.cx + 2*2,
		m_rcHeaderWithoutBorders.top,
		m_rcHeaderWithoutBorders.right,
		m_rcHeaderWithoutBorders.top + sizeScrollButton.cy
		);
	// header area for drawing selected date text
	m_rcHeaderDate.SetRect(
		m_rcHeaderWithoutBorders.left + sizeScrollButton.cx,
		m_rcHeaderWithoutBorders.top,
		m_rcHeaderWithoutBorders.right - sizeScrollButton.cx + 2*2,
		m_rcHeaderWithoutBorders.bottom
		);
	// day cells
//CRect rcBorderMetrics = m_pDPW->OnDatePickerGetBorderMetrics();
INT nIndentSpace = m_pDPW->OnDatePickerQueryIndentSpace();
	m_rcDaysCells.SetRect(
		m_rcClient.left + nIndentSpace,
		m_rcHeader.bottom,
		m_rcClient.right - nIndentSpace - 2,
		m_rcClient.bottom //- rcBorderMetrics.bottom
		);
	// days of week
	m_rcDaysOfWeek.CopyRect( &m_rcDaysCells );
INT nDaysOfWeekHeight = m_pDPW->OnDatePickerQueryDaysOfWeekHeight();
	m_rcDaysOfWeek.bottom = m_rcDaysOfWeek.top + nDaysOfWeekHeight;
	// days cells (continue...)
	m_rcDaysCells.top += m_rcDaysOfWeek.Height();
	// day's cells
CSize sizeDateCell = m_pDPW->OnDatePickerQueryDateCellSize();
	for( INT nWeek = 0; nWeek < 6; nWeek ++)
	{
		for( INT nDay = 0; nDay < 7; nDay ++ )
		{
			CRect rcCell( 
				m_rcDaysCells.left + nDay*sizeDateCell.cx,
				m_rcDaysCells.top + nWeek*sizeDateCell.cy, 
				m_rcDaysCells.left + nDay*sizeDateCell.cx + sizeDateCell.cx,
				m_rcDaysCells.top + nWeek*sizeDateCell.cy + sizeDateCell.cy
				);
			m_aDays[nWeek][nDay].SetRect( rcCell );
		} // for( INT nDay = 0; nDay < 7; nDay ++ )
	} // for( INT nWeek = 0; nWeek < 6; nWeek ++)
}

CRect CExtDatePickerWnd::MONTH_INFO::GetRect() const
{
	ASSERT_VALID( this );
	return m_rcClient;
}

CRect CExtDatePickerWnd::MONTH_INFO::GetHeaderRect() const
{
	ASSERT_VALID( this );
	return m_rcHeader;
}

void CExtDatePickerWnd::MONTH_INFO::GetMonth( INT & nMonth, INT & nYear ) const
{
	ASSERT_VALID( this );
	nMonth = m_nMonth;
	nYear = m_nYear;
}

COleDateTime CExtDatePickerWnd::MONTH_INFO::GetMonthInfoDT() const
{
	ASSERT_VALID( this );
COleDateTime dt( m_nYear, m_nMonth, 1, 1, 1, 1 );
	return dt;
}

const CExtDatePickerWnd::MONTH_INFO::DATE_INFO * CExtDatePickerWnd::MONTH_INFO::HitTestDay( const POINT & ptClient ) const
{
	for( INT nWeek = 0; nWeek < 6; nWeek ++ )
	{
		for( INT nDay = 0; nDay < 7; nDay ++ )
		{
			const CExtDatePickerWnd::MONTH_INFO::DATE_INFO * pDay = &(m_aDays[nWeek][nDay]);
			if( pDay->GetRect().PtInRect( ptClient ) )
			{
				return pDay;
			}
		} // for( INT nWeek = 0; nWeek < 6; nWeek ++ )
	} // for( INT nWeek = 0; nWeek < 6; nWeek ++ )
	return NULL;
}

LONG CExtDatePickerWnd::MONTH_INFO::HitTest(
	const POINT & ptClient,
	COleDateTime * pDT // = NULL
	) const
{
	ASSERT_VALID( this );
	if( pDT != NULL )
		pDT->SetStatus( COleDateTime::invalid );
	if( m_rcClient.IsRectEmpty()
		|| (! m_rcClient.PtInRect(ptClient) )
		)
		return __EDPWH_NOWHERE;
CSize sizeCalendarDimensionsCurrent;
	m_pDPW->DimGet( 
		NULL,
		NULL,
		&sizeCalendarDimensionsCurrent
		);
	if(		(!m_rcScrollBtnBackward.IsRectEmpty())
		&&	m_rcScrollBtnBackward.PtInRect(ptClient)
		&&	m_nRow == 0 
		&&  m_nCol == 0
		)
		return __EDPWH_BTN_BACKWARD;
	if(		(!m_rcScrollBtnForward.IsRectEmpty())
		&&	m_rcScrollBtnForward.PtInRect(ptClient)
		&&	m_nRow == 0 
		&&	(sizeCalendarDimensionsCurrent.cx - 1) == m_nCol 
		)
		return __EDPWH_BTN_FORWARD;
	if(		(!m_rcHeaderDate.IsRectEmpty())
		&&	m_rcHeaderDate.PtInRect(ptClient)
		)
	{
		if( pDT != NULL )
			(*pDT) = GetMonthInfoDT();
		return __EDPWH_HEADER_DATE;
	}
	if(		(!m_rcDaysOfWeek.IsRectEmpty())
		&&	m_rcDaysOfWeek.PtInRect(ptClient)
		)
	{
		if( pDT != NULL )
			(*pDT) = GetMonthInfoDT();
		return __EDPWH_DAYS_OF_WEEK;
	}
	if(		(!m_rcDaysCells.IsRectEmpty())
		&&	m_rcDaysCells.PtInRect(ptClient)
		)
	{
		if( pDT != NULL )
		{
			const DATE_INFO * pDayCell = HitTestDay( ptClient );
			if( pDayCell != NULL )
				(*pDT) = pDayCell->GetDate();
			else
				(*pDT) = GetMonthInfoDT();
		} // if( pDT != NULL )
		DATE_INFO * HitTestDay( const POINT & ptClient );
		return __EDPWH_DAYS_AREA;
	}
	return __EDPWH_NOWHERE;
}

void CExtDatePickerWnd::MONTH_INFO::Draw( CDC & dc ) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( m_pDPW != NULL );
	// draw header
	m_pDPW->OnDatePickerDrawHeader(
		m_nCol,
		m_nRow,
		dc,
		m_nYear,
		m_nMonth,
		m_rcHeader,
		m_rcHeaderWithoutBorders,
		m_rcHeaderDate,
		m_rcScrollBtnBackward,
		m_rcScrollBtnForward
		);
	// draw days of week
	m_pDPW->OnDatePickerDrawDaysOfWeek(
		m_nYear,
		m_nMonth,
		dc,
		m_rcDaysOfWeek
		);
	// draw day cells
DWORD dwDatePickerStyle = m_pDPW->GetDatePickerStyle();
	for( INT nWeek = 0; nWeek < 6; nWeek ++ )
	{
		for( INT nDay = 0; nDay < 7; nDay ++ )
		{
			m_aDays[nWeek][nDay].Draw( 
				dc,
				((dwDatePickerStyle & __EDPWS_SHOW_NON_MONTH_DAYS) != 0 ) ? true : false,
				((dwDatePickerStyle & __EDPWS_SHOW_TODAY_HIGHLIGHT) != 0 ) ? true : false
				);
		} // for( INT nDay = 0; nDay < 7; nDay ++ )
	} // for( INT nWeek = 0; nWeek < 6; nWeek ++ )
}

/////////////////////////////////////////////////////////////////////////////
// CExtDatePickerWnd::MONTH_INFO::DATE_INFO
/////////////////////////////////////////////////////////////////////////////

CExtDatePickerWnd::MONTH_INFO::DATE_INFO::DATE_INFO(
	CExtDatePickerWnd * pDPW // = NULL
	)
	: m_pDPW( pDPW )
	, m_pMI( NULL )
	, m_rcCell( 0, 0, 0, 0 )
{
	m_dt.SetStatus( COleDateTime::invalid );
}

CExtDatePickerWnd::MONTH_INFO::DATE_INFO::~DATE_INFO()
{
}

#ifdef _DEBUG

void CExtDatePickerWnd::MONTH_INFO::DATE_INFO::AssertValid() const
{
	CObject::AssertValid();
	ASSERT(		
		m_dt.GetStatus() == COleDateTime::invalid
		||	(		m_dt.GetStatus() == COleDateTime::valid 
				&&	m_dt.GetHour() == 0
				&&	m_dt.GetMinute() == 0
				&&	m_dt.GetSecond() == 0
			)
		);
	ASSERT(
			m_pDPW != NULL
		//&& ::IsWindow( m_pDPW->m_hWnd )
		);
	ASSERT_VALID( m_pDPW );
	ASSERT_VALID( m_pMI );
}

void CExtDatePickerWnd::MONTH_INFO::DATE_INFO::Dump( CDumpContext & dc ) const
{
	CObject::Dump( dc );
}
#endif //_DEBUG

void CExtDatePickerWnd::MONTH_INFO::DATE_INFO::Clear()
{
	ASSERT_VALID( this );
	m_dt.SetStatus( COleDateTime::invalid );
	m_rcCell.SetRect( 0, 0, 0, 0 );
}

void CExtDatePickerWnd::MONTH_INFO::DATE_INFO::SetDatePickerWnd( 
	CExtDatePickerWnd * pDPW,
	CExtDatePickerWnd::MONTH_INFO * pMI
	)
{
	ASSERT(
			pDPW != NULL
		//&& ::IsWindow(pDPW->m_hWnd)
		);
	ASSERT_VALID( pDPW );
	m_pDPW = pDPW;
	ASSERT_VALID( pMI );
	m_pMI = pMI;
}

void CExtDatePickerWnd::MONTH_INFO::DATE_INFO::SetRect( const RECT & rcCell )
{
	ASSERT_VALID( this );
	m_rcCell = rcCell;
}

CRect CExtDatePickerWnd::MONTH_INFO::DATE_INFO::GetRect() const
{
	ASSERT_VALID( this );
	return m_rcCell;
}

void CExtDatePickerWnd::MONTH_INFO::DATE_INFO::SetDate( const COleDateTime & dt )
{
	ASSERT_VALID( this );
	m_dt = dt;
}

COleDateTime CExtDatePickerWnd::MONTH_INFO::DATE_INFO::GetDate() const
{
	ASSERT_VALID( this );
	return m_dt;
}

void CExtDatePickerWnd::MONTH_INFO::DATE_INFO::Draw( 
	CDC & dc,
	bool bShowNonMonthDays, // = true
	bool bShowTodayHighlight // = true
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( COleDateTime::invalid == m_dt.GetStatus() )
		return;
INT nMonth = 0, nYear = 0;
	m_pMI->GetMonth( nMonth, nYear );
bool bNonMonthDay = 
		( m_dt.GetMonth() == nMonth )
			? false
			: true;
bool bDrawSelected = m_pDPW->SelectionGetForDate( m_dt );
CRect rcCell( m_rcCell );
bool bToday = false;
COleDateTime dtNow = m_pDPW->OnDatePickerGetToday();
	if(		m_dt.GetYear() == dtNow.GetYear() 
		&&	m_dt.GetMonth() == dtNow.GetMonth() 
		&&	m_dt.GetDay() == dtNow.GetDay() 				
		)
		bToday = true;
	m_pDPW->OnDatePickerDrawDateCell(
		dc,
		bShowNonMonthDays,
		bShowTodayHighlight,
		bNonMonthDay,
		bToday,
		rcCell,
		m_dt,
		bDrawSelected
		);
}

/////////////////////////////////////////////////////////////////////////////
// CExtDatePickerWnd
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE( CExtDatePickerWnd, CWnd );

CExtDatePickerWnd::CExtDatePickerWnd()
	: m_bDirectCreateCall( false )
	, m_bCanceling( false )
	, m_bUpdatingLayout( false )
	, m_rcDatePickerClient( 0, 0, 0, 0 )
	, m_rcDatePickerInnerArea( 0, 0, 0, 0 )
	, m_rcBtnNone( 0, 0, 0, 0 )
	, m_rcBtnToday( 0, 0, 0, 0 )
	, m_dwDatePickerStyle( __EDPWS_DEFAULT )
	, m_eMTT( CExtDatePickerWnd::__EMTT_NOTHING )
	, m_bBtnTodayMouseHover( false )
	, m_bBtnNoneMouseHover( false )
	, m_sizeCalendarDimensionsMin( 1, 1 )
	, m_sizeCalendarDimensionsCurrent( 1, 1 )
	, m_sizeCalendarDimensionsMax( 1, 1 )
	, m_sizeMonthPicker( 0, 0 )
	, m_sizeDateCell( 0, 0 )
	, m_sizePushButton( 0, 0 )
	, m_sizeScrollButton( 0, 0 )
	, m_nMonthHeaderHeight( 0 )
	, m_nButtonAreaHeight( 0 )
	, m_nDaysOfWeekHeight( 0 )
	, m_nIndentSpace( 0 )
	, m_lParamCookie( 0 )
	, m_bAutoDeleteWindow( false )
	, m_hWndNotificationReceiver( NULL )
	, m_pExternalSelectionInfo( NULL )
{ 
	VERIFY( RegisterDatePickerWndClass() );
	SelectionClear();
	CurrentDateSet( COleDateTime::GetCurrentTime() );
}

CExtDatePickerWnd::~CExtDatePickerWnd()
{
	for( INT nMonth = 0; nMonth < m_arrMonths.GetSize(); nMonth++ )
	{
		MONTH_INFO * pMonth = m_arrMonths.GetAt(nMonth);
		ASSERT( pMonth );
		if( pMonth != NULL )
		{
			delete pMonth;
			pMonth = NULL;
		}
	}
}

BEGIN_MESSAGE_MAP(CExtDatePickerWnd, CWnd)
	//{{AFX_MSG_MAP(CExtDatePickerWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_CAPTURECHANGED()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_KILLFOCUS()
	ON_WM_TIMECHANGE()
	ON_WM_ACTIVATEAPP()
	ON_WM_MOUSEACTIVATE()
	//}}AFX_MSG_MAP
	ON_WM_SYSCOLORCHANGE()
	__EXT_MFC_SAFE_ON_WM_SETTINGCHANGE()
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_MESSAGE(__ExtMfc_WM_THEMECHANGED, OnThemeChanged)
END_MESSAGE_MAP()

#ifdef _DEBUG
void CExtDatePickerWnd::AssertValid() const
{
	CWnd::AssertValid();

	ASSERT( m_sizeCalendarDimensionsMin.cx >= 1 );
	ASSERT( m_sizeCalendarDimensionsMin.cy >= 1 );
	ASSERT( m_sizeCalendarDimensionsMax.cx >= 1 );
	ASSERT( m_sizeCalendarDimensionsMax.cy >= 1 );
	ASSERT( m_sizeCalendarDimensionsMax.cx >= m_sizeCalendarDimensionsMin.cx );
	ASSERT( m_sizeCalendarDimensionsMax.cy >= m_sizeCalendarDimensionsMin.cy );
	
	ASSERT(
			(	m_dtSelBegin.GetStatus() == COleDateTime::invalid
			&&	m_dtSelEnd.GetStatus() == COleDateTime::invalid
			)
		||	(	m_dtSelBegin.GetStatus() == COleDateTime::valid
			&&	m_dtSelEnd.GetStatus() == COleDateTime::valid
			)
		);
	ASSERT(		
			m_dtSelBegin.GetStatus() == COleDateTime::invalid
		||	(	m_dtSelBegin.GetStatus() == COleDateTime::valid 
			&&	m_dtSelBegin.GetHour() == 0
			&&	m_dtSelBegin.GetMinute() == 0
			&&	m_dtSelBegin.GetSecond() == 0
			)
		);
	ASSERT(		
			m_dtSelEnd.GetStatus() == COleDateTime::invalid
		||	(	m_dtSelEnd.GetStatus() == COleDateTime::valid 
			&&	m_dtSelEnd.GetHour() == 0
			&&	m_dtSelEnd.GetMinute() == 0
			&&	m_dtSelEnd.GetSecond() == 0
			)
		);
	ASSERT(		
			m_dtCurrentDate.GetStatus() == COleDateTime::invalid
		||	(	m_dtCurrentDate.GetStatus() == COleDateTime::valid 
			&&	m_dtCurrentDate.GetHour() == 0
			&&	m_dtCurrentDate.GetMinute() == 0
			&&	m_dtCurrentDate.GetSecond() == 0
			)
		);

	ASSERT(
			(	m_dtDisplayedBegin.GetStatus() == COleDateTime::invalid
			&&	m_dtDisplayedEnd.GetStatus() == COleDateTime::invalid
			)
		||	(	m_dtDisplayedBegin.GetStatus() == COleDateTime::valid
			&&	m_dtDisplayedEnd.GetStatus() == COleDateTime::valid
			)
		);
}

void CExtDatePickerWnd::Dump(CDumpContext& dc) const
{
	CWnd::Dump( dc );
}
#endif

CExtDatePickerWnd::SELECTION_NOTIFICATION::SELECTION_NOTIFICATION(
	CExtDatePickerWnd::SELECTION_INFO * pExternalSelectionInfo,
	HWND hWndDatePicker,
	LPARAM lParamCookie,
	bool bFinalSelectionChanging,
	const COleDateTime & dtBegin,
	const COleDateTime & dtEnd,
	bool bAllowFinalParentCancelMode // = true
	)
	: m_hWndDatePicker( hWndDatePicker )
	, m_bFinalSelectionChanging( bFinalSelectionChanging )
	, m_bAllowFinalParentCancelMode( bAllowFinalParentCancelMode )
	, m_pExternalSelectionInfo( pExternalSelectionInfo )
{
	m_lParamCookie = lParamCookie;
	m_dtBegin = dtBegin;
	m_dtEnd = dtEnd;
	ASSERT(
			m_hWndDatePicker != NULL
		&&	::IsWindow( m_hWndDatePicker )
		);
}

CExtDatePickerWnd::SELECTION_NOTIFICATION::SELECTION_NOTIFICATION(
	CExtDatePickerWnd::SELECTION_INFO * pExternalSelectionInfo,
	const CExtDatePickerWnd & wndDatePicker,
	bool bFinalSelectionChanging,
	bool bAllowFinalParentCancelMode // = true
	)
	: m_hWndDatePicker( wndDatePicker.GetSafeHwnd() )
	, m_bFinalSelectionChanging( bFinalSelectionChanging )
	, m_bAllowFinalParentCancelMode( bAllowFinalParentCancelMode )
	, m_pExternalSelectionInfo( pExternalSelectionInfo )
{
	m_lParamCookie = wndDatePicker.m_lParamCookie;
	ASSERT(
			m_hWndDatePicker != NULL
		&&	::IsWindow( m_hWndDatePicker )
		);
	wndDatePicker.SelectionGet( m_dtBegin, m_dtEnd );
}

CExtDatePickerWnd::SELECTION_NOTIFICATION::operator WPARAM() const
{
WPARAM wParam = reinterpret_cast < WPARAM > ( this );
	return wParam;
}

const CExtDatePickerWnd::SELECTION_NOTIFICATION *
	CExtDatePickerWnd::SELECTION_NOTIFICATION::FromWPARAM( WPARAM wParam )
{
CExtDatePickerWnd::SELECTION_NOTIFICATION * pSN =
		reinterpret_cast < CExtDatePickerWnd::SELECTION_NOTIFICATION * > ( wParam );
	ASSERT( pSN != NULL );
	ASSERT(
			pSN->m_hWndDatePicker != NULL
		&&	::IsWindow( pSN->m_hWndDatePicker )
		);
	return pSN;
}

void CExtDatePickerWnd::SELECTION_NOTIFICATION::Notify(
	HWND hWndNotify
	) const
{
	if(		m_bFinalSelectionChanging
		&&	m_pExternalSelectionInfo != NULL
		)
		(*m_pExternalSelectionInfo) = (*this);
	ASSERT(
			m_hWndDatePicker != NULL
		&&	::IsWindow( m_hWndDatePicker )
		&&	hWndNotify != NULL
		&&	::IsWindow( hWndNotify )
		);
	::SendMessage(
		hWndNotify,
		CExtDatePickerWnd::g_nMsgSelectionNotification,
		*this,
		m_lParamCookie
		);
	if( m_bFinalSelectionChanging && m_bAllowFinalParentCancelMode )
	{
		HWND hWndParent = ::GetParent( m_hWndDatePicker );
		if(		hWndParent != NULL
			&&	::IsWindow( hWndParent )
			)
			::SendMessage( hWndParent, WM_CANCELMODE, 0L, 0L );
	} // if( m_bFinalSelectionChanging && m_bAllowFinalParentCancelMode )
}

const UINT CExtDatePickerWnd::g_nMsgSelectionNotification =
	::RegisterWindowMessage(
		_T("CExtDatePickerWnd::g_nMsgSelectionNotification")
		);
bool CExtDatePickerWnd::g_bDatePickerWndClassRegistered = false;

bool CExtDatePickerWnd::RegisterDatePickerWndClass()
{
	if( g_bDatePickerWndClassRegistered )
		return true;
WNDCLASS wndclass;
HINSTANCE hInst = ::AfxGetInstanceHandle();
	if( ! ::GetClassInfo(
			hInst,
			__EXT_DATE_PICKER_CLASS_NAME,
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
		wndclass.hCursor = ::LoadCursor( NULL, IDC_ARROW );
		ASSERT( wndclass.hCursor != NULL );
		wndclass.hbrBackground = NULL; 
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = __EXT_DATE_PICKER_CLASS_NAME;
		if( !::AfxRegisterClass( &wndclass ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}
	g_bDatePickerWndClassRegistered = true;
	return true;
}

bool CExtDatePickerWnd::Create(
	CWnd * pParentWnd,
	const RECT & rcWnd, // = CRect( 0, 0, 0, 0 )
	UINT nDlgCtrlID, // = UINT( IDC_STATIC )
	CSize sizeCalendarDimensionsMin, // = CSize(1,1),
	CSize sizeCalendarDimensionsMax, // = CSize(1,1),
	DWORD dwWindowStyle, // = WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN
	DWORD dwDatePickerStyle, // = __EDPWS_DEFAULT
	CCreateContext * pContext // = NULL
	)
{
	if( ! RegisterDatePickerWndClass() )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	m_bDirectCreateCall = true;
	m_dwDatePickerStyle = dwDatePickerStyle;
	if( ! CWnd::Create(
			__EXT_DATE_PICKER_CLASS_NAME,
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
		return false;
	}
	DimSet(
		sizeCalendarDimensionsMin,
		sizeCalendarDimensionsMax
		);
	_RecalcMetrics();
	if( ! _CreateHelper() )
	{
		ASSERT( FALSE );
		return false;
	}
	return true;
}

void CExtDatePickerWnd::AdjustSize( const SIZE & sizeDim )
{
	ASSERT_VALID( this );
	ASSERT( sizeDim.cx >= 1 && sizeDim.cy >= 1 );
	if(		GetSafeHwnd() == NULL
		||	( ! ::IsWindow( GetSafeHwnd() ) )
		)
		return;
CSize sizeContent = OnDatePickerCalcContentSize( sizeDim );
CRect rcWnd;
	GetWindowRect( &rcWnd );
	if( rcWnd.Size() == sizeContent )
		return;
	rcWnd.right = rcWnd.left + sizeContent.cx;
	rcWnd.bottom = rcWnd.top + sizeContent.cy;
	if( (GetStyle() & WS_CHILD) )
		GetParent()->ScreenToClient( &rcWnd );
	MoveWindow( &rcWnd, FALSE );
	UpdateDatePickerWnd( false );
}

void CExtDatePickerWnd::AdjustSize( INT nDimCols, INT nDimRows )
{
	ASSERT_VALID( this );
	ASSERT( nDimCols >= 1 && nDimRows >= 1 );
CSize sizeDim( nDimCols, nDimRows );
	AdjustSize( sizeDim );
}

bool CExtDatePickerWnd::_CreateHelper()
{
DWORD dwDatePickerStyle = GetDatePickerStyle();
	if( (dwDatePickerStyle & __EDPWS_BUTTON_ANY) != 0 )
	{
		INT nButtonAreaHeight = OnDatePickerQueryButtonAreaHeight();
		m_rcDatePickerClient.bottom += nButtonAreaHeight;
	}
	_RecalcCalendar();
	return true;
}

void CExtDatePickerWnd::_RecalcLayout()
{
	ASSERT_VALID( this );
	if(		GetSafeHwnd() == NULL
		||	( ! ::IsWindow( GetSafeHwnd() ) )
		)
		return;
	m_sizeCalendarDimensionsCurrent = m_sizeCalendarDimensionsMin;
	m_rcDatePickerClient.SetRectEmpty();
	m_rcDatePickerInnerArea.SetRectEmpty();
	m_rcBtnNone.SetRectEmpty();
	m_rcBtnToday.SetRectEmpty();
DWORD dwDatePickerStyle = GetDatePickerStyle();
bool bBtnTodayVisible =
		((dwDatePickerStyle & __EDPWS_BUTTON_TODAY) != 0 )
			? true : false;
bool bBtnNoneVisible =
		((dwDatePickerStyle & __EDPWS_BUTTON_NONE) != 0 )
			? true : false;
CRect rcClient;
	GetClientRect( &rcClient );
CRect rcBorderMetrics = OnDatePickerGetBorderMetrics();
INT nButtonAreaHeight = OnDatePickerQueryButtonAreaHeight();
CSize sizeMonthPicker = OnDatePickerQueryMonthPickerSize();
	if( (dwDatePickerStyle & __EDPWS_AUTO_DIMENSION_MODE) != 0 )
	{
		CRect rcTmpClient(rcClient);
		rcTmpClient.DeflateRect( rcBorderMetrics );
		if( bBtnTodayVisible || bBtnNoneVisible )
			rcTmpClient.bottom -= nButtonAreaHeight;
		INT nDesiredCols =
			(INT)floor( double(rcTmpClient.Width())/double(sizeMonthPicker.cx) );
		INT nDesiredRows =
			(INT)floor( double(rcTmpClient.Height())/double(sizeMonthPicker.cy) );
		if( nDesiredCols > m_sizeCalendarDimensionsMax.cx )
			nDesiredCols = m_sizeCalendarDimensionsMax.cx;
		if( nDesiredRows > m_sizeCalendarDimensionsMax.cy )
			nDesiredRows = m_sizeCalendarDimensionsMax.cy;
		if(		m_sizeCalendarDimensionsCurrent.cx != nDesiredCols
			||	m_sizeCalendarDimensionsCurrent.cy != nDesiredRows
			)
		{
			m_sizeCalendarDimensionsCurrent.cx = nDesiredCols;
			m_sizeCalendarDimensionsCurrent.cy = nDesiredRows;
			_RecalcCalendar();
		}
	} // if( (dwDatePickerStyle & __EDPWS_AUTO_DIMENSION_MODE) != 0 )
	
	// inner client area including border area
	m_rcDatePickerClient.CopyRect( &rcClient );
	m_rcDatePickerClient.right =
		m_sizeCalendarDimensionsCurrent.cx*sizeMonthPicker.cx
		+ rcBorderMetrics.left + rcBorderMetrics.right;
	m_rcDatePickerClient.bottom =
		m_sizeCalendarDimensionsCurrent.cy*sizeMonthPicker.cy
		+ rcBorderMetrics.top + rcBorderMetrics.bottom;

	// inner client area excluding border area
	m_rcDatePickerInnerArea.CopyRect( &m_rcDatePickerClient );
	m_rcDatePickerInnerArea.DeflateRect( rcBorderMetrics );

	// increase client area if the bottom buttons are visible
	if(	bBtnTodayVisible ||  bBtnNoneVisible )
		m_rcDatePickerClient.bottom += nButtonAreaHeight;
	
	// buttons area
	if( bBtnTodayVisible || bBtnNoneVisible )
	{
		CRect rcButtons( &m_rcDatePickerClient );
		rcButtons.top = m_rcDatePickerInnerArea.bottom;
		rcButtons.bottom -= rcBorderMetrics.bottom;
		CSize sizePushButton = OnDatePickerQueryPushButtonSize();

		// center buttons vertically
		INT nBtnTop =
			rcButtons.top
			+ (rcButtons.Height() - sizePushButton.cy)/2;
		INT nBtnBottom =
			rcButtons.top
			+ (rcButtons.Height() + sizePushButton.cy)/2;
		// center buttons horizontally
		INT nBtnLeftNone =
			rcButtons.left
			+ ( rcButtons.Width() - sizePushButton.cx)/2;
		INT nBtnLeftToday =
			rcButtons.left
			+ ( rcButtons.Width() - sizePushButton.cx)/2;
		
		if( bBtnTodayVisible )
			nBtnLeftNone += sizePushButton.cx/2 + 5;
		if( bBtnNoneVisible )
			nBtnLeftToday -= sizePushButton.cx/2 + 5;
			
		if( bBtnNoneVisible )
			m_rcBtnNone.SetRect( 
				nBtnLeftNone, 
				nBtnTop, 
				nBtnLeftNone + sizePushButton.cx, 
				nBtnBottom 
				);
		if( bBtnTodayVisible )
			m_rcBtnToday.SetRect( 
				nBtnLeftToday, 
				nBtnTop, 
				nBtnLeftToday + sizePushButton.cx, 
				nBtnBottom  
				);
	} // if( bBtnTodayVisible || bBtnNoneVisible )
}

void CExtDatePickerWnd::_RecalcCalendar(  
	bool bUpdateNow // = false
	)
{
	ASSERT_VALID( this );
INT nMonthsCount = m_arrMonths.GetSize();
	for( INT nMonth = 0; nMonth < nMonthsCount; nMonth++ )
	{
		MONTH_INFO * pMonth = m_arrMonths.GetAt( nMonth );
		if( pMonth == NULL )
			continue;
		delete pMonth;
	} // for( INT nMonth = 0; nMonth < nMonthsCount; nMonth++ )
	m_arrMonths.RemoveAll();
COleDateTime dtCurrentDate = CurrentDateGet();
INT nStartMonth = dtCurrentDate.GetMonth();
INT nStartYear = dtCurrentDate.GetYear();
	nMonthsCount =
		m_sizeCalendarDimensionsCurrent.cy
		* m_sizeCalendarDimensionsCurrent.cx;
	m_dtDisplayedBegin.SetStatus( COleDateTime::invalid );
	m_dtDisplayedEnd.SetStatus( COleDateTime::invalid );
CRect rcBorderMetrics = OnDatePickerGetBorderMetrics();
CSize sizeMonthPicker = OnDatePickerQueryMonthPickerSize();
	for( nMonth = 0; nMonth < nMonthsCount; nMonth++ )
	{
		ldiv_t div_result;
		div_result = ldiv( nMonth, m_sizeCalendarDimensionsCurrent.cx );
		INT nRow = div_result.quot;
		INT nCol = div_result.rem;
		CRect rcMonth;
		rcMonth.SetRect(
			nCol*sizeMonthPicker.cx,
			nRow*sizeMonthPicker.cy,
			sizeMonthPicker.cx + nCol*sizeMonthPicker.cx,
			sizeMonthPicker.cy + nRow*sizeMonthPicker.cy
			);
		rcMonth.OffsetRect(
			rcBorderMetrics.left - m_rcDatePickerClient.left,
			rcBorderMetrics.top  - m_rcDatePickerClient.top
			);
		MONTH_INFO * pMonth = 
			new MONTH_INFO( 
				nRow, 
				nCol, 
				nStartMonth, 
				nStartYear, 
				rcMonth, 
				this 
			);
		m_arrMonths.Add( pMonth );
		if( m_dtDisplayedBegin.GetStatus() == COleDateTime::invalid )
			m_dtDisplayedBegin.SetDate( nStartYear, nStartMonth, 1 );
		if( nStartMonth == 12 )
			m_dtDisplayedEnd.SetDate( nStartYear+1, 1, 1 );
		else
			m_dtDisplayedEnd.SetDate( nStartYear, nStartMonth+1, 1 );
		nStartMonth += 1;
		if( nStartMonth > 12 )
		{
			nStartMonth = 1;
			nStartYear++;
		}
	} // for( nMonth = 0; nMonth < nMonthsCount; nMonth++ )
	if( m_dtDisplayedEnd.GetStatus() != COleDateTime::invalid )
		m_dtDisplayedEnd -= 1;
	UpdateDatePickerWnd( bUpdateNow );
}

CExtDatePickerWnd::MONTH_INFO * CExtDatePickerWnd::_HitTestMonth( const POINT & ptClient ) const
{
	ASSERT_VALID( this );
	for( INT nMonth = 0; nMonth < m_arrMonths.GetSize(); nMonth++ )
	{
		MONTH_INFO * pMonth = m_arrMonths.GetAt( nMonth );
		if( pMonth == NULL )
			continue;
		if( pMonth->GetRect().PtInRect( ptClient ) )
			return pMonth;
	}
	return NULL;
}

const CExtDatePickerWnd::MONTH_INFO * CExtDatePickerWnd::_FindMonth( INT nRow, INT nCol ) const
{
	ASSERT_VALID( this );
	for( INT nMonth = 0; nMonth < m_arrMonths.GetSize(); nMonth++ )
	{
		const MONTH_INFO * pMonth = m_arrMonths.GetAt( nMonth );
		if( pMonth == NULL )
			continue;
		if( pMonth->m_nRow == nRow && pMonth->m_nCol == nCol )
			return pMonth;
	}
	return NULL; 
}

BOOL CExtDatePickerWnd::OnEraseBkgnd(CDC* pDC) 
{
	pDC;
	return FALSE;
}

void CExtDatePickerWnd::OnPaint() 
{
	ASSERT_VALID( this );
	// recalc layout first
	_RecalcLayout();
CPaintDC dcPaint( this );
CRect rcClient;
	GetClientRect( &rcClient );
	if( rcClient.IsRectEmpty() )
		return;
CExtMemoryDC dc( &dcPaint, &rcClient );
	OnDatePickerDrawEntire( dc, rcClient );
	g_PaintManager->OnPaintSessionComplete( this );
}

LRESULT CExtDatePickerWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == CExtPopupControlMenuWnd::g_nMsgControlInputRetranslate )
	{
		ASSERT( wParam != 0 );
		CExtPopupControlMenuWnd::CONTROLINPUTRETRANSLATEINFO & _ciri =
			*((CExtPopupControlMenuWnd::CONTROLINPUTRETRANSLATEINFO*)wParam);
		if( _ciri.m_eRTT == CExtPopupControlMenuWnd::CONTROLINPUTRETRANSLATEINFO::__ERTT_KEYBOARD )
		{
			OnKeyDown( _ciri.m_nChar, _ciri.m_nRepCnt, _ciri.m_nFlags );
			_ciri.m_bNoEat = false;
			return (!0);
		} // if( _ciri.m_eRTT == CExtPopupControlMenuWnd::CONTROLINPUTRETRANSLATEINFO::__ERTT_KEYBOARD )
		return 0;
	} // if( message == CExtPopupControlMenuWnd::g_nMsgControlInputRetranslate )
	return CWnd::WindowProc(message, wParam, lParam);
}

void CExtDatePickerWnd::PreSubclassWindow() 
{
	CWnd::PreSubclassWindow();
	if( ! m_bDirectCreateCall )
	{
	ModifyStyle( 0, WS_CLIPCHILDREN );
	_RecalcMetrics();
	if( ! _CreateHelper() )
	{
		ASSERT( FALSE );
		AfxThrowMemoryException();
	}
	} // if( ! m_bDirectCreateCall )
	if( m_pExternalSelectionInfo != NULL )
	{
		SelectionSet(
			m_pExternalSelectionInfo->m_dtBegin,
			m_pExternalSelectionInfo->m_dtEnd
			);
		if( m_pExternalSelectionInfo->m_dtBegin.GetStatus() == COleDateTime::valid )
		{
			CurrentDateSet( m_pExternalSelectionInfo->m_dtBegin, true );
		}
		else if( m_pExternalSelectionInfo->m_dtEnd.GetStatus() == COleDateTime::valid )
		{
			CurrentDateSet( m_pExternalSelectionInfo->m_dtEnd, true );
		}
	} // if( m_pExternalSelectionInfo != NULL )
}

void CExtDatePickerWnd::PostNcDestroy()
{
	if( m_bAutoDeleteWindow )
		delete this;
}

DWORD CExtDatePickerWnd::GetDatePickerStyle() const
{
	ASSERT_VALID( this );
	return m_dwDatePickerStyle;
}

DWORD CExtDatePickerWnd::ModifyDatePickerStyle(
	DWORD dwRemove,
	DWORD dwAdd, // = 0
	bool bUpdateDatePickerWnd // = false
	)
{
	ASSERT_VALID( this );
DWORD dwOldStyle = m_dwDatePickerStyle;
	m_dwDatePickerStyle &= ~dwRemove;
	m_dwDatePickerStyle |= dwAdd;
	if( GetSafeHwnd() == NULL )
		return dwOldStyle;
bool bRecalcCalendar = false;
	if( (dwRemove & __EDPWS_AUTO_DIMENSION_MODE) != 0 )
	{
		m_sizeCalendarDimensionsCurrent = m_sizeCalendarDimensionsMin;
		bRecalcCalendar = true;
	}
	if( (dwRemove & __EDPWS_MULTIPLE_SELECTION) != 0 )
	{
		m_dtSelEnd = m_dtSelBegin;
	}
const DWORD dwRecalcStyles =
		__EDPWS_HIDE_INNER_NON_MONTH_DAYS
		| __EDPWS_BORDER_RESERVE
		| __EDPWS_BOLD_ANY;
	if(		(dwRemove & dwRecalcStyles) != 0
		||	(dwAdd & dwRecalcStyles) != 0
		)
		bRecalcCalendar = true;
	if( bRecalcCalendar )
	{
		_RecalcMetrics();
		_RecalcCalendar();
	}
	UpdateDatePickerWnd( bUpdateDatePickerWnd );
	return dwOldStyle;
}

void CExtDatePickerWnd::UpdateDatePickerWnd(
	bool bUpdateNow // = false
	)
{
	ASSERT_VALID( this );
	if(		GetSafeHwnd() == NULL
		||	( ! ::IsWindow(GetSafeHwnd()) )
		)
		return;
	if( m_bUpdatingLayout )
		return;
	m_bUpdatingLayout = true;
	_RecalcLayout();
	Invalidate();
	if(		bUpdateNow
		&&	(GetStyle() & WS_VISIBLE) != 0
		)
		UpdateWindow();
	m_bUpdatingLayout = false;
}

bool CExtDatePickerWnd::SelectionGetForDate( 
	const COleDateTime & dt 
	) const
{
	ASSERT_VALID( this );
	bool bReturn = false;
	if(		m_dtSelBegin.GetStatus() == COleDateTime::valid 
		&&	m_dtSelEnd.GetStatus() == COleDateTime::valid
		)
	{
		COleDateTime dtSelBegin, dtSelEnd;
		if( m_dtSelBegin > m_dtSelEnd )
		{
			dtSelBegin = m_dtSelEnd;
			dtSelEnd = m_dtSelBegin;
		}
		else
		{
			dtSelBegin = m_dtSelBegin;
			dtSelEnd = m_dtSelEnd;
		}

		if (dt >= dtSelBegin && dt <= dtSelEnd)
			bReturn = true;
	}
	return bReturn;
}

bool CExtDatePickerWnd::EnsureVisible(
	const COleDateTime & dt,
	bool bUpdateNow // = false
	)
{
	ASSERT_VALID( this );
	if( dt.GetStatus() != COleDateTime::valid )
		return false;
COleDateTime dtDisplayedBegin, dtDisplayedEnd;
	GetDisplayedRange( dtDisplayedBegin, dtDisplayedEnd );
//	CurrentDateSet( dt );
	if(		dtDisplayedBegin.GetStatus() == COleDateTime::invalid
		||	dtDisplayedEnd.GetStatus() == COleDateTime::invalid
		)
	{
		CurrentDateSet( dt );
		_RecalcCalendar( bUpdateNow );
	}
	else if(
			dt < dtDisplayedBegin
		||	dtDisplayedEnd < dt 
		)
	{
		CurrentDateSet( dt );
		_RecalcCalendar( bUpdateNow );
	}
	return true;
}

void CExtDatePickerWnd::SelectionClear( 
	bool bUpdateNow // = false
	)
{
	ASSERT_VALID( this );
COleDateTime dtSel;
	dtSel.SetStatus( COleDateTime::invalid );
	SelectionSet(
		dtSel, 
		dtSel, 
		false,
		bUpdateNow
		);
}

void CExtDatePickerWnd::SelectionSet(
	const COleDateTime & dtSelBegin, 
	const COleDateTime & dtSelEnd, 
	bool bNormalize, // = false
	bool bUpdateNow // = false
	)
{
	ASSERT_VALID( this );
	if(		dtSelBegin.GetStatus() == m_dtSelBegin.GetStatus() 
		&&	dtSelEnd.GetStatus() == m_dtSelEnd.GetStatus() 
		&&	dtSelBegin.GetYear() == m_dtSelBegin.GetYear()
		&&	dtSelBegin.GetMonth() == m_dtSelBegin.GetMonth()
		&&	dtSelBegin.GetDay() == m_dtSelBegin.GetDay()
		&&	dtSelEnd.GetYear() == m_dtSelEnd.GetYear()
		&&	dtSelEnd.GetMonth() == m_dtSelEnd.GetMonth()
		&&	dtSelEnd.GetDay() == m_dtSelEnd.GetDay()
		)
		return;
	if(		dtSelBegin.GetStatus() == COleDateTime::invalid
		||	dtSelEnd.GetStatus() == COleDateTime::invalid
		)
	{
		m_dtSelBegin.SetStatus( COleDateTime::invalid );
		m_dtSelEnd.SetStatus( COleDateTime::invalid );
	}
	else
	{
		m_dtSelBegin.SetDate(
			dtSelBegin.GetYear(),
			dtSelBegin.GetMonth(),
			dtSelBegin.GetDay()
			);
		m_dtSelEnd.SetDate(
			dtSelEnd.GetYear(),
			dtSelEnd.GetMonth(),
			dtSelEnd.GetDay()
			);
		if( bNormalize && m_dtSelBegin > m_dtSelEnd )
		{
			COleDateTime dtTemp = m_dtSelBegin;
			m_dtSelBegin = m_dtSelEnd;
			m_dtSelEnd = dtTemp;
		}
	}
	if( GetSafeHwnd() != NULL )
	{
		UpdateDatePickerWnd( bUpdateNow );
		HWND hWndNotificationReceiver =
			OnDatePickerGetNotificationReceiver();
		if( hWndNotificationReceiver != NULL )
		{
			SELECTION_NOTIFICATION _SN( m_pExternalSelectionInfo, *this, false );
			_SN.Notify( hWndNotificationReceiver );
		} // if( hWndNotificationReceiver != NULL )
	} // if( GetSafeHwnd() != NULL )
}

HWND CExtDatePickerWnd::OnDatePickerGetNotificationReceiver() const
{
	ASSERT_VALID( this );
	if(		m_hWndNotificationReceiver != NULL
		&&	::IsWindow( m_hWndNotificationReceiver )
		)
		return m_hWndNotificationReceiver;
	if( GetSafeHwnd() == NULL )
		return NULL;
HWND hWndNotificationReceiver = ::GetParent( m_hWnd );
	return hWndNotificationReceiver;
}

void CExtDatePickerWnd::SelectionGet(
	COleDateTime & dtSelBegin, 
	COleDateTime & dtSelEnd
	) const
{
	ASSERT_VALID( this );
	dtSelBegin = m_dtSelBegin;
	dtSelEnd = m_dtSelEnd;
}

void CExtDatePickerWnd::GetDisplayedRange(
	COleDateTime & dtBegin, 
	COleDateTime & dtEnd
	) const
{
	ASSERT_VALID( this );
	dtBegin = m_dtDisplayedBegin;
	dtEnd = m_dtDisplayedEnd;
}

void CExtDatePickerWnd::CurrentDateSet( 
	const COleDateTime & dtCurrentDate, 
	bool bUpdateNow // = false
	)
{
	ASSERT_VALID( this );
	ASSERT( dtCurrentDate.GetStatus() == COleDateTime::valid );
	m_dtCurrentDate.SetDate(
		dtCurrentDate.GetYear(),
		dtCurrentDate.GetMonth(),
		dtCurrentDate.GetDay()
		);
	_RecalcCalendar( false );
	UpdateDatePickerWnd( bUpdateNow );
}

void CExtDatePickerWnd::CurrentDateSet( 
	INT nYear,
	INT nMonth,
	INT nDay, // = 1
	bool bUpdateNow // = false
	)
{
	ASSERT_VALID( this );
COleDateTime dtCurrentDate;
	dtCurrentDate.SetDate(
		nYear,
		nMonth,
		nDay
		);
	CurrentDateSet( dtCurrentDate, bUpdateNow );
}

COleDateTime CExtDatePickerWnd::CurrentDateGet() const
{
	ASSERT_VALID( this );
	return m_dtCurrentDate;
}

void CExtDatePickerWnd::CurrentDateGet( 
	INT & nYear, 
	INT & nMonth, 
	INT & nDay 
	) const
{
	ASSERT_VALID( this );
	COleDateTime dtCurrentDate = CurrentDateGet();
	ASSERT( dtCurrentDate.GetStatus() == COleDateTime::valid );
	nYear = dtCurrentDate.GetYear();
	nMonth = dtCurrentDate.GetMonth();
	nDay = dtCurrentDate.GetDay();
}

void CExtDatePickerWnd::DimSet( 
	CSize sizeCalendarDimensionsMin, // = CSize(0,0)
	CSize sizeCalendarDimensionsMax, // = CSize(0,0)
	bool bUpdateNow // = false
	)
{
	ASSERT_VALID( this );
	if( sizeCalendarDimensionsMin != CSize(0,0) )
	{
		if( sizeCalendarDimensionsMin.cx < 1 )
			sizeCalendarDimensionsMin.cx = 1;
		if( sizeCalendarDimensionsMin.cy < 1 )
			sizeCalendarDimensionsMin.cy = 1;
		m_sizeCalendarDimensionsMin = sizeCalendarDimensionsMin;
	}
	if( sizeCalendarDimensionsMax != CSize(0,0) )
	{
		if( sizeCalendarDimensionsMax.cx < 1 )
			sizeCalendarDimensionsMax.cx = 1;
		if( sizeCalendarDimensionsMax.cy < 1 )
			sizeCalendarDimensionsMax.cy = 1;
		if( sizeCalendarDimensionsMax.cx < sizeCalendarDimensionsMin.cx )
			sizeCalendarDimensionsMax.cx = sizeCalendarDimensionsMin.cx; 
		if( sizeCalendarDimensionsMax.cy < sizeCalendarDimensionsMin.cy )
			sizeCalendarDimensionsMax.cy = sizeCalendarDimensionsMin.cy; 
		m_sizeCalendarDimensionsMax = sizeCalendarDimensionsMax;
	}
	if( GetSafeHwnd() == NULL )
		return;
	_RecalcLayout();
	_RecalcCalendar();
	UpdateDatePickerWnd( bUpdateNow );
}

void CExtDatePickerWnd::DimGet( 
	SIZE * pSizeCalendarDimensionsMin, // = NULL
	SIZE * pSizeCalendarDimensionsMax, // = NULL
	SIZE * pSizeCalendarDimensionsCurrent // = NULL
	) const
{
	ASSERT_VALID( this );
	if( pSizeCalendarDimensionsMin != NULL )
		(*pSizeCalendarDimensionsMin) = m_sizeCalendarDimensionsMin;
	if( pSizeCalendarDimensionsMax != NULL )
		(*pSizeCalendarDimensionsMax) = m_sizeCalendarDimensionsMax;
	if( pSizeCalendarDimensionsCurrent != NULL )
		(*pSizeCalendarDimensionsCurrent) = m_sizeCalendarDimensionsCurrent;
}

void CExtDatePickerWnd::OnTimeChange() 
{
	ASSERT_VALID( this );
	CWnd::OnTimeChange();
	_RecalcCalendar();
}

void CExtDatePickerWnd::OnSysColorChange() 
{
	ASSERT_VALID( this );
	CWnd::OnSysColorChange();
	g_PaintManager.OnSysColorChange( this );
	g_CmdManager.OnSysColorChange( this );
	UpdateDatePickerWnd();	
}

LRESULT CExtDatePickerWnd::OnDisplayChange( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	_RecalcMetrics();
	_RecalcCalendar();
LRESULT lResult = CWnd::OnDisplayChange( wParam, lParam );
	g_PaintManager.OnDisplayChange( this, (INT)wParam, CPoint(lParam) );
	g_CmdManager.OnDisplayChange( this, (INT)wParam, CPoint(lParam) );
	return lResult;
}

LRESULT CExtDatePickerWnd::OnThemeChanged( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	_RecalcMetrics();
	_RecalcCalendar();
LRESULT lResult = Default();
	g_PaintManager.OnThemeChanged( this, wParam, lParam );
	g_CmdManager.OnThemeChanged( this, wParam, lParam );
	return lResult;
}

void CExtDatePickerWnd::OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection) 
{
	ASSERT_VALID( this );
	_RecalcMetrics();
	_RecalcCalendar();
	CWnd::OnSettingChange(uFlags, lpszSection);
	g_PaintManager.OnSettingChange( this, uFlags, lpszSection );
	g_CmdManager.OnSettingChange( this, uFlags, lpszSection );
	_RecalcCalendar();
}

CExtSafeString CExtDatePickerWnd::OnDatePickerGetMonthName(
	INT nMonth,
	bool bMakeUpper // = false
	) const
{
INT nMonthNameLocale = LOCALE_SMONTHNAME1 + nMonth - 1;
LPTSTR lpMonthBuf = (LPTSTR)::LocalAlloc(LPTR, 200);
	VERIFY(
		g_ResourceManager->GetLocaleInfo(
			nMonthNameLocale,
			lpMonthBuf,
			200
			) != 0
		);
CExtSafeString sLongMonthName = (LPTSTR)lpMonthBuf;
	::LocalFree( lpMonthBuf );
	if( bMakeUpper )
		sLongMonthName.MakeUpper();
	return sLongMonthName;
}

INT CExtDatePickerWnd::OnDatePickerGetFirstDayOfWeek() const
{
LPTSTR lpFirstDayOfWeek = (LPTSTR)::LocalAlloc( LPTR, 24 );
	VERIFY(
		g_ResourceManager->GetLocaleInfo(
			LOCALE_IFIRSTDAYOFWEEK,
			lpFirstDayOfWeek,
			24
			) != 0
		);
	// 0 Monday
	// 1 Tuesday
	// 2 Wednesday
	// 3 Thursday
	// 4 Friday 
	// 5 Saturday
	// 6 Sunday
INT nFirstDayOfWeek = _ttoi( lpFirstDayOfWeek );
	::LocalFree( lpFirstDayOfWeek );
	return nFirstDayOfWeek;
}

CExtSafeString CExtDatePickerWnd::OnDatePickerGetShortDayName(
	INT nDayOfWeek,
	bool bMakeUpper // = false
	) const
{
INT nDayNameLocale = LOCALE_SABBREVDAYNAME1 + nDayOfWeek - 1;
LPTSTR lpDayOfWeekBuf = (LPTSTR)::LocalAlloc( LPTR, 100 );
	VERIFY(
		g_ResourceManager->GetLocaleInfo(
			nDayNameLocale,
			lpDayOfWeekBuf,
			100
			) != 0
		);
CExtSafeString sLongDayName = (LPTSTR)lpDayOfWeekBuf;
	::LocalFree( lpDayOfWeekBuf );
	if( bMakeUpper )
		sLongDayName.MakeUpper();
CExtSafeString sShortDayName;
	if( sLongDayName.GetLength() > 0 )
		sShortDayName = sLongDayName.GetAt(0);
	return sShortDayName;
}

COleDateTime CExtDatePickerWnd::OnDatePickerGetToday() const
{
	ASSERT_VALID( this );
COleDateTime dtNow = COleDateTime::GetCurrentTime();
	return dtNow;
}

CRect CExtDatePickerWnd::OnDatePickerGetBorderMetrics() const
{
	ASSERT_VALID( this );
DWORD dwDatePickerStyle = GetDatePickerStyle();
	if( (dwDatePickerStyle & __EDPWS_BORDER_RESERVE) == 0 )
		return CRect( 0, 0, 0, 0 );
	return CRect( 4, 4, 4, 4 );
}

CFont * CExtDatePickerWnd::OnDatePickerQueryHeaderPopupFont() const
{
	ASSERT_VALID( this );
DWORD dwDatePickerStyle = GetDatePickerStyle();
	CFont * pFont =
		((dwDatePickerStyle & __EDPWS_BOLD_HEADER_POPUP) != 0)
			? ( &g_PaintManager->m_FontBold )
			: ( &g_PaintManager->m_FontNormal )
			;
	return pFont;
}

CFont * CExtDatePickerWnd::OnDatePickerQueryDayCellFont(
	const COleDateTime & dt
	) const
{
	ASSERT_VALID( this );
	dt;
DWORD dwDatePickerStyle = GetDatePickerStyle();
	CFont * pFont =
		((dwDatePickerStyle & __EDPWS_BOLD_DAY_CELLS) != 0)
			? ( &g_PaintManager->m_FontBold )
			: ( &g_PaintManager->m_FontNormal )
			;
	return pFont;
}

CFont * CExtDatePickerWnd::OnDatePickerQueryMonthHeaderFont(
	INT nYear,
	INT nMonth
	) const
{
	ASSERT_VALID( this );
	nYear;
	nMonth;
DWORD dwDatePickerStyle = GetDatePickerStyle();
	CFont * pFont =
		((dwDatePickerStyle & __EDPWS_BOLD_HEADER) != 0)
			? ( &g_PaintManager->m_FontBold )
			: ( &g_PaintManager->m_FontNormal )
			;
	return pFont;
}

CFont * CExtDatePickerWnd::OnDatePickerQueryDaysOfWeekFont(
	INT nYear,
	INT nMonth
	) const
{
	ASSERT_VALID( this );
	nYear;
	nMonth;
DWORD dwDatePickerStyle = GetDatePickerStyle();
	CFont * pFont =
		((dwDatePickerStyle & __EDPWS_BOLD_WEEK_DAYS) != 0)
			? ( &g_PaintManager->m_FontBold )
			: ( &g_PaintManager->m_FontNormal )
			;
	return pFont;
}

INT CExtDatePickerWnd::OnDatePickerQueryIndentSpace() const
{
	ASSERT_VALID( this );
	return m_nIndentSpace;
}

CSize CExtDatePickerWnd::OnDatePickerQueryDateCellSize() const
{
	ASSERT_VALID( this );
	return m_sizeDateCell;
}

CSize CExtDatePickerWnd::OnDatePickerQueryPushButtonSize() const
{
	ASSERT_VALID( this );
	return m_sizePushButton;
}

CSize CExtDatePickerWnd::OnDatePickerQueryScrollButtonSize() const
{
	ASSERT_VALID( this );
	return m_sizeScrollButton;
}

INT CExtDatePickerWnd::OnDatePickerQueryMonthHeaderHeight() const
{
	ASSERT_VALID( this );
	return m_nMonthHeaderHeight;
}

INT CExtDatePickerWnd::OnDatePickerQueryDaysOfWeekHeight() const
{
	ASSERT_VALID( this );
	return m_nDaysOfWeekHeight;
}

INT CExtDatePickerWnd::OnDatePickerQueryButtonAreaHeight() const
{
	ASSERT_VALID( this );
	return m_nButtonAreaHeight;
}

CSize CExtDatePickerWnd::OnDatePickerQueryMonthPickerSize() const
{
	ASSERT_VALID( this );
	return m_sizeMonthPicker;
}

void CExtDatePickerWnd::OnDatePickerRecalcMetrics(
	CDC & dcMeasure,
	CSize & sizeMonthPicker,
	CSize & sizeDateCell,
	CSize & sizePushButton,
	CSize & sizeScrollButton,
	INT & nMonthHeaderHeight,
	INT & nButtonAreaHeight,
	INT & nDaysOfWeekHeight,
	INT & nIndentSpace
	)
{
	ASSERT_VALID( this );
	ASSERT( dcMeasure.GetSafeHdc() != NULL );

DWORD dwDatePickerStyle = GetDatePickerStyle();
CRect rcText;
CSize sizeText;
CFont * pOldFont =
		dcMeasure.SelectObject(
			( (dwDatePickerStyle & __EDPWS_BOLD_ANY) != 0 )
				? (&g_PaintManager->m_FontBold)
				: (&g_PaintManager->m_FontNormal)
			);

	nIndentSpace = 11;
	
	// measure week-day cell size
CSize sizeWD( 10, 10 );
	for( INT i = 0; i < 7; i++ )
	{
		CExtSafeString sWD =
			OnDatePickerGetShortDayName( i, true );
		rcText.SetRect( 0, 0, 0, 0 );
		dcMeasure.DrawText(
			sWD,
			rcText,
			DT_SINGLELINE|DT_LEFT|DT_TOP|DT_CALCRECT
			);
		sizeText = rcText.Size();
		sizeText.cx += 2;
		sizeText.cy += 2;
		if( sizeWD.cx < sizeText.cx )
			sizeWD.cx = sizeText.cx;
		if( sizeWD.cy < sizeText.cy )
			sizeWD.cy = sizeText.cy;
	} // for( INT i = 0; i < 7; i++ )

	// measure date cell size
	sizeDateCell.cx = 17;
	sizeDateCell.cy = 15;
	for( i = 1; i <= 31; i++ )
	{
		CString strMeasure;
		strMeasure.Format( _T("%02d"), i );
		rcText.SetRect( 0, 0, 0, 0 );
		dcMeasure.DrawText(
			strMeasure,
			strMeasure.GetLength() ,
			rcText,
			DT_SINGLELINE|DT_LEFT|DT_TOP|DT_CALCRECT
			);
		sizeText = rcText.Size();
		sizeText.cx += 2;
		sizeText.cy += 2;
		if( sizeDateCell.cx < sizeText.cx )
			sizeDateCell.cx = sizeText.cx;
		if( sizeDateCell.cy < sizeText.cy )
			sizeDateCell.cy = sizeText.cy;
	} // for( i = 1; i <= 31; i++ )

	// update week-day cell size
	if( sizeDateCell.cx < sizeWD.cx )
		sizeDateCell.cx = sizeWD.cx;
	else if( sizeDateCell.cx > sizeWD.cx )
		sizeWD.cx = sizeDateCell.cx;

	// measure push-button size
	sizePushButton.cx = 47;
	sizePushButton.cy = 18;
CExtSafeString strCaption = OnDatePickerQueryPushButtonCaptionText( __EDPWH_BTN_TODAY );
	rcText.SetRect( 0, 0, 0, 0 );
	dcMeasure.DrawText(
		strCaption,
		rcText,
		DT_SINGLELINE|DT_LEFT|DT_TOP|DT_CALCRECT
		);
	sizeText = rcText.Size();
	sizeText.cx += 10;
	sizeText.cy += 6;
	if( sizePushButton.cx < sizeText.cx )
		sizePushButton.cx = sizeText.cx;
	if( sizePushButton.cy < sizeText.cy )
		sizePushButton.cy = sizeText.cy;
	strCaption = OnDatePickerQueryPushButtonCaptionText( __EDPWH_BTN_NONE );
	rcText.SetRect( 0, 0, 0, 0 );
	dcMeasure.DrawText(
		strCaption,
		rcText,
		DT_SINGLELINE|DT_LEFT|DT_TOP|DT_CALCRECT
		);
	sizeText = rcText.Size();
	sizeText.cx += 10;
	sizeText.cy += 6;
	if( sizePushButton.cx < sizeText.cx )
		sizePushButton.cx = sizeText.cx;
	if( sizePushButton.cy < sizeText.cy )
		sizePushButton.cy = sizeText.cy;

	// measure scroll-button size
	sizeScrollButton.cx = sizeScrollButton.cy = 16;
CFont * pTempFont = dcMeasure.SelectObject( &g_PaintManager->m_FontMarlett );
	dcMeasure.SelectObject( &g_PaintManager->m_FontMarlett );
	rcText.SetRect( 0, 0, 0, 0 );
	dcMeasure.DrawText(
		_T("3"), // popup rightArrow
		1,
		&rcText,
		DT_SINGLELINE|DT_LEFT|DT_TOP|DT_CALCRECT
		);
	sizeText = rcText.Size();
	if( sizeScrollButton.cx < sizeText.cx )
		sizeScrollButton.cx = sizeText.cx;
	if( sizeScrollButton.cy < sizeText.cy )
		sizeScrollButton.cy = sizeText.cy;
	rcText.SetRect( 0, 0, 0, 0 );
	dcMeasure.DrawText(
		_T("4"), // popup rightArrow
		1,
		&rcText,
		DT_SINGLELINE|DT_LEFT|DT_TOP|DT_CALCRECT
		);
	sizeText = rcText.Size();
	if( sizeScrollButton.cx < sizeText.cx )
		sizeScrollButton.cx = sizeText.cx;
	if( sizeScrollButton.cy < sizeText.cy )
		sizeScrollButton.cy = sizeText.cy;
	dcMeasure.SelectObject( pTempFont );

	// measure header text
CSize sizeHeaderText(10,10);
	for( i = 1; i <= 12; i++ )
	{
		CExtSafeString sMonthName =
			OnDatePickerGetMonthName( i );
		sMonthName += _T(" 0000");
		rcText = 
			CExtPaintManager::stat_CalcTextDimension(
			dcMeasure,
			*(OnDatePickerQueryHeaderPopupFont()),
			sMonthName
			);
		sizeText = rcText.Size();
		if( sizeHeaderText.cx < sizeText.cx )
			sizeHeaderText.cx = sizeText.cx;
		if( sizeHeaderText.cy < sizeText.cy )
			sizeHeaderText.cy = sizeText.cy;
	} // for( i = 1; i <= 12; i++ )
	
	// measure header area
CSize sizeHeaderArea( sizeHeaderText );
	sizeHeaderArea.cx += sizeScrollButton.cx * 2 + 4;
	if( (sizeScrollButton.cy+2) > sizeHeaderArea.cy )
		sizeHeaderArea.cy = (sizeScrollButton.cy+2);

	if( sizeHeaderArea.cx < (sizeWD.cx*7) )
		sizeHeaderArea.cx = (sizeWD.cx*7);

	// put header's and weekdays heights
	nMonthHeaderHeight = sizeHeaderArea.cy;
	if( nMonthHeaderHeight < 18 )
		nMonthHeaderHeight = 18;
	nDaysOfWeekHeight = sizeWD.cy;
	nDaysOfWeekHeight += 2;
	if( nDaysOfWeekHeight < 16 )
		nDaysOfWeekHeight = 16;

	// put buttons-area height
	nButtonAreaHeight = sizePushButton.cy + 10;
	if( nButtonAreaHeight < 30 )
		nButtonAreaHeight = 30;

	sizeMonthPicker.cx = sizeDateCell.cx*7;
	sizeMonthPicker.cy = sizeDateCell.cy*6 + nDaysOfWeekHeight + nMonthHeaderHeight;
	if( sizeMonthPicker.cx < sizeWD.cx )
		sizeMonthPicker.cx = sizeWD.cx;
	if( sizeMonthPicker.cx < sizeHeaderArea.cx )
		sizeMonthPicker.cx = sizeHeaderArea.cx;
	sizeMonthPicker.cx += nIndentSpace*2;
	if( sizeMonthPicker.cx < 143 )
		sizeMonthPicker.cx = 143;
	if( sizeMonthPicker.cy < 125 )
		sizeMonthPicker.cy = 125;

	dcMeasure.SelectObject( pOldFont );
}

CSize CExtDatePickerWnd::OnDatePickerCalcContentSize(
	const SIZE & sizeDim
	) const
{
	ASSERT_VALID( this );
	ASSERT( sizeDim.cx >= 1 && sizeDim.cy >= 1 );
	if( GetSafeHwnd() == NULL )
		( const_cast < CExtDatePickerWnd * > ( this ) ) ->
			_RecalcMetrics();
CSize sizeMonthPicker = OnDatePickerQueryMonthPickerSize();
CRect rcBorderMetrics = OnDatePickerGetBorderMetrics();
CSize sizeContent(
		sizeMonthPicker.cx*sizeDim.cx
			+ rcBorderMetrics.left + rcBorderMetrics.right,
		sizeMonthPicker.cy*sizeDim.cy
			+ rcBorderMetrics.top + rcBorderMetrics.bottom
		);
DWORD dwDatePickerStyle = GetDatePickerStyle();
	if( (dwDatePickerStyle & __EDPWS_BUTTON_ANY ) != 0 )
		sizeContent.cy += OnDatePickerQueryButtonAreaHeight();
	return sizeContent;
}

void CExtDatePickerWnd::_RecalcMetrics()
{
	ASSERT_VALID( this );
CWindowDC dcMeasure( NULL );
	OnDatePickerRecalcMetrics(
		dcMeasure,
		m_sizeMonthPicker,
		m_sizeDateCell,
		m_sizePushButton,
		m_sizeScrollButton,
		m_nMonthHeaderHeight,
		m_nButtonAreaHeight,
		m_nDaysOfWeekHeight,
		m_nIndentSpace
		);
}

void CExtDatePickerWnd::OnDatePickerEraseClientArea(
	CDC & dc,
	const CRect & rcClient
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	g_PaintManager->PaintDatePickerClientArea(
		dc,
		rcClient,
		m_rcDatePickerClient,
		(CObject*)this
		);
}

void CExtDatePickerWnd::OnDatePickerDrawBorder(
	CDC & dc,
	const CRect & rcClient
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	g_PaintManager->PaintDatePickerBorder(
		dc,
		rcClient,
		(CObject*)this
		);
}

void CExtDatePickerWnd::OnDatePickerDrawTodayBorder(
	CDC & dc,
	bool bShowNonMonthDays,
	bool bShowTodayHighlight,
	bool bDrawNonMonthDay,
	bool bDrawToday,
	const CRect & rcCell,
	const COleDateTime & dt,
	bool bDrawSelected
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	bShowNonMonthDays;
	bDrawNonMonthDay;
	dt;
	bDrawSelected;
	if(		bShowTodayHighlight
		&&	bDrawToday
		)
	{
		COLORREF clrTodayBorder =
			dc.GetNearestColor( RGB(128,0,0) );
		dc.Draw3dRect(
			(LPRECT)&rcCell,
			clrTodayBorder,
			clrTodayBorder
			);
	}
}

void CExtDatePickerWnd::OnDatePickerDrawDateCell(
	CDC & dc,
	bool bShowNonMonthDays,
	bool bShowTodayHighlight,
	bool bDrawNonMonthDay,
	bool bDrawToday,
	const CRect & rcCell,
	const COleDateTime & dt,
	bool bDrawSelected
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if(	(!bShowNonMonthDays) && bDrawNonMonthDay )
		return;
COLORREF clrText =
		g_PaintManager->GetColor( COLOR_BTNTEXT );
COLORREF clrBk =
		g_PaintManager->GetColor( CExtPaintManager::CLR_3DFACE_IN );
	if( bDrawNonMonthDay && (!bDrawSelected) )
		clrText = ::GetSysColor( COLOR_3DSHADOW );
INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
COLORREF clrTextOld = dc.SetTextColor( clrText );
CFont * pOldFont = dc.SelectObject( OnDatePickerQueryDayCellFont(dt) );
CString strDay;
	strDay.Format( _T("%d"), dt.GetDay() );
	if( bDrawSelected )
		dc.FillSolidRect( &rcCell, clrBk );
CRect rcText( rcCell );
	rcText.right -= 2;
	dc.DrawText(
		strDay, 
		strDay.GetLength(),
		&rcText,
		DT_RIGHT|DT_VCENTER|DT_SINGLELINE
		);
	OnDatePickerDrawTodayBorder(
		dc,
		bShowNonMonthDays,
		bShowTodayHighlight,
		bDrawNonMonthDay,
		bDrawToday,
		rcCell,
		dt,
		bDrawSelected
		);
	dc.SelectObject( pOldFont );
	dc.SetTextColor( clrTextOld );
	dc.SetBkMode( nOldBkMode );
}

void CExtDatePickerWnd::OnDatePickerDrawButtonsSeparator(
	CDC & dc,
	const CRect & rcSeparator
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	g_PaintManager->PaintDatePickerButtonsSeparator(
		dc,
		rcSeparator,
		(CObject*)this
		);
}

CExtSafeString CExtDatePickerWnd::OnDatePickerQueryPushButtonCaptionText(
	LONG nButtonType // __EDPWH_BTN_NONE or __EDPWH_BTN_TODAY
	) const
{
CExtSafeString strButtonCaption( _T("") );;
	switch( nButtonType )
	{
	case __EDPWH_BTN_NONE:
		if( ! g_ResourceManager->LoadString( strButtonCaption, IDS_EXT_DP_BTN_NONE ) )
			strButtonCaption = _T("None");
		break;
	case __EDPWH_BTN_TODAY:
		if( ! g_ResourceManager->LoadString( strButtonCaption, IDS_EXT_DP_BTN_TODAY ) )
			strButtonCaption = _T("Today");
		break;
	} // switch( nButtonType )
	return strButtonCaption;
}

void CExtDatePickerWnd::OnDatePickerDrawPushButton(
	CDC & dc,
	const CRect & rcButton,
	LONG nButtonType, // __EDPWH_BTN_NONE or __EDPWH_BTN_TODAY
	__EXT_MFC_SAFE_LPCTSTR strCaption,
	bool bFlat,
	bool bDrawBorder,
	bool bPushed,
	bool bHover
	) const
{
	ASSERT_VALID( this );
	g_PaintManager->PaintDatePickerPushButton(
		dc,
		rcButton,
		nButtonType,
		strCaption,
		bFlat,
		bDrawBorder,
		bPushed,
		bHover,
		(CObject*)this
		);
}

void CExtDatePickerWnd::OnDatePickerEraseHeader(
	INT nCol,
	INT nRow,
	CDC & dc,
	INT nYear,
	INT nMonth,
	const CRect & rcHeader,
	const CRect & rcHeaderWithoutBorders,
	const CRect & rcHeaderDate,
	const CRect & rcScrollBtnBackward,
	const CRect & rcScrollBtnForward
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	g_PaintManager->PaintDatePickerHeaderBackground(
		nCol,
		nRow,
		dc,
		nYear,
		nMonth,
		rcHeader,
		rcHeaderWithoutBorders,
		rcHeaderDate,
		rcScrollBtnBackward,
		rcScrollBtnForward,
		(CObject*)this
		);
}

void CExtDatePickerWnd::OnDatePickerDrawHeader(
	INT nCol,
	INT nRow,
	CDC & dc,
	INT nYear,
	INT nMonth,
	const CRect & rcHeader,
	const CRect & rcHeaderWithoutBorders,
	const CRect & rcHeaderDate,
	const CRect & rcScrollBtnBackward,
	const CRect & rcScrollBtnForward
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
DWORD dwDatePickerStyle = GetDatePickerStyle();
	if( (dwDatePickerStyle & __EDPWS_NO_HEADER_BACKGROUND) == 0 )
		OnDatePickerEraseHeader(
			nCol,
			nRow,
			dc,
			nYear,
			nMonth,
			rcHeader,
			rcHeaderWithoutBorders,
			rcHeaderDate,
			rcScrollBtnBackward,
			rcScrollBtnForward
			);
CExtSafeString sCaption = OnDatePickerGetMonthName( nMonth );
	sCaption += _T(" ");
CExtSafeString sYear;
	sYear.Format( _T("%d"), nYear );
	sCaption += sYear;
INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
COLORREF clrText = g_PaintManager->GetColor( COLOR_BTNTEXT );
COLORREF clrTextOld = dc.SetTextColor( clrText );
CFont * pOldFont =
		dc.SelectObject(
			OnDatePickerQueryMonthHeaderFont(
				nYear,
				nMonth
				)
			);
	dc.DrawText(
		sCaption,
		(LPRECT)&rcHeaderDate,
		DT_CENTER | DT_VCENTER | DT_SINGLELINE
		);
	dc.SelectObject( pOldFont );
	// draw scrolling buttons
	pOldFont = dc.SelectObject( &g_PaintManager->m_FontMarlett );
	if(		(! rcScrollBtnBackward.IsRectEmpty() )
		&&	nRow == 0 
		&&  nCol == 0
		)
	{
		dc.DrawText(
			_T("3"), // popup rightArrow
			1,
			(LPRECT)&rcScrollBtnBackward,
			DT_CENTER|DT_VCENTER|DT_SINGLELINE
			);
	}
CSize sizeCalendarDimensionsCurrent;
	DimGet( 
		NULL,
		NULL,
		&sizeCalendarDimensionsCurrent
		);
	if(		(! rcScrollBtnForward.IsRectEmpty() ) 
		&&	nRow == 0 
		&&	(sizeCalendarDimensionsCurrent.cx - 1) == nCol 
		)
	{
		dc.DrawText(
			_T("4"), // popup rightArrow
			1,
			(LPRECT)&rcScrollBtnForward,
			DT_CENTER|DT_VCENTER|DT_SINGLELINE
			);
	}
	dc.SelectObject( pOldFont );
	dc.SetTextColor( clrTextOld );
	dc.SetBkMode( nOldBkMode );
}

void CExtDatePickerWnd::OnDatePickerDrawDaysOfWeek(
	INT nYear,
	INT nMonth,
	CDC & dc,
	const CRect & rcClient
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	// day headers
	INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
	COLORREF clrText = g_PaintManager->GetColor( COLOR_BTNTEXT );
	COLORREF clrTextOld = dc.SetTextColor( clrText );
CFont * pOldFont =
		dc.SelectObject(
			OnDatePickerQueryDaysOfWeekFont(
				nYear,
				nMonth
				)
			);
INT nFirstDayOfWeek = OnDatePickerGetFirstDayOfWeek();
CSize sizeDateCell = OnDatePickerQueryDateCellSize();
	for( INT i = 0; i < 7; i++ )
	{
		CRect rcCell( 
			rcClient.left + i*sizeDateCell.cx,
			rcClient.top, 
			rcClient.left + i*sizeDateCell.cx + sizeDateCell.cx,
			rcClient.top + sizeDateCell.cy
			);
		rcCell.right -= 2; // text indent from right border
		rcCell.OffsetRect(0,-1);
		INT nDay = i + 1;
		for( INT j = 0; j < nFirstDayOfWeek; j++ )
		{
			if( nDay == 7 )
				nDay = 1;
			else
				nDay++;
		}
		CExtSafeString sShortDayName = OnDatePickerGetShortDayName( nDay, true );
		dc.DrawText(
			sShortDayName, 
			rcCell,
			DT_RIGHT|DT_VCENTER|DT_SINGLELINE
			);
	} // pOldFont = dc.SelectObject
	dc.SelectObject( pOldFont );
	dc.SetTextColor( clrTextOld );
	dc.SetBkMode( nOldBkMode );
	// bottom line
	CRect rcBL(
		rcClient.left,
		rcClient.bottom - 2,
		rcClient.right,
		rcClient.bottom - 1
		);
	dc.FillSolidRect( 
		&rcBL, 
		::GetSysColor(COLOR_3DSHADOW)
		);
}

void CExtDatePickerWnd::OnDatePickerDrawEntire(
	CDC & dc,
	const CRect & rcClient
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	// draw background
	OnDatePickerEraseClientArea( dc, rcClient );
	// draw border
DWORD dwDatePickerStyle = GetDatePickerStyle();
	if( (dwDatePickerStyle&__EDPWS_BORDER) == __EDPWS_BORDER )
		OnDatePickerDrawBorder( dc, m_rcDatePickerClient );
	// draw dates
	for( INT nRow = 0; nRow < m_sizeCalendarDimensionsCurrent.cy; nRow++ )
	{
		for( INT nCol = 0; nCol < m_sizeCalendarDimensionsCurrent.cx; nCol++ )
		{
			const MONTH_INFO * pMonth = _FindMonth( nRow, nCol );	
			if( pMonth != NULL )
				pMonth->Draw( dc );
		}
	}
	// draw button's separator
	if(		(dwDatePickerStyle & __EDPWS_BUTTON_ANY ) != 0
		&&	(dwDatePickerStyle & __EDPWS_BUTTON_SEPARATOR) != 0
		)
	{
		INT nIndentSpace = OnDatePickerQueryIndentSpace();
		CRect rcSeparator(
			m_rcDatePickerInnerArea.left + nIndentSpace, 
			m_rcDatePickerInnerArea.bottom + 1, 
			m_rcDatePickerInnerArea.right - nIndentSpace, 
			m_rcDatePickerInnerArea.bottom + 2
			);
		OnDatePickerDrawButtonsSeparator(
			dc,
			rcSeparator
			);
	}
	// draw TODAY button
	if( ! m_rcBtnToday.IsRectEmpty() )
	{
		bool bPushed = ( m_eMTT == __EMTT_BTN_TODAY_PRESSED && m_bBtnTodayMouseHover );
		bool bFlat = false;
		bool bHover = 
				m_bBtnTodayMouseHover 
			&& (m_eMTT == __EMTT_NOTHING || m_eMTT == __EMTT_BTN_TODAY_PRESSED);
		bool bDrawBorder = true;
		CExtSafeString strCaption = OnDatePickerQueryPushButtonCaptionText( __EDPWH_BTN_TODAY );
		OnDatePickerDrawPushButton(
			dc,
			m_rcBtnToday,
			__EDPWH_BTN_TODAY,
			(LPCTSTR)strCaption,
			bFlat,
			bDrawBorder,
			bPushed,
			bHover
			);
	}
	// draw NONE button
	if( ! m_rcBtnNone.IsRectEmpty() )
	{
		bool bPushed = ( m_eMTT == __EMTT_BTN_NONE_PRESSED && m_bBtnNoneMouseHover );
		bool bFlat = false;
		bool bHover = 
				m_bBtnNoneMouseHover 
			&& (m_eMTT == __EMTT_NOTHING || m_eMTT == __EMTT_BTN_NONE_PRESSED);
		bool bDrawBorder = true;
		CExtSafeString strCaption = OnDatePickerQueryPushButtonCaptionText( __EDPWH_BTN_NONE );
		OnDatePickerDrawPushButton(
			dc,
			m_rcBtnNone,
			__EDPWH_BTN_NONE,
			(LPCTSTR)strCaption,
			bFlat,
			bDrawBorder,
			bPushed,
			bHover
			);
	}
}

void CExtDatePickerWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	if( !_ProcessMouseClick( point, true, MK_LBUTTON, nFlags ) )
		CWnd::OnLButtonDown(nFlags, point);
}

void CExtDatePickerWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	if( !_ProcessMouseClick( point, true, MK_LBUTTON, nFlags ) )
		CWnd::OnLButtonDblClk(nFlags, point);
}

void CExtDatePickerWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	if( !_ProcessMouseClick( point, false, MK_LBUTTON, nFlags ) )
		CWnd::OnLButtonUp(nFlags, point);
}

void CExtDatePickerWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	if( !_ProcessMouseMove( point, nFlags ) )
		CWnd::OnMouseMove(nFlags, point);
}

bool CExtDatePickerWnd::_ProcessMouseMove(
	CPoint point,
	UINT nMouseEventFlags
	)
{
	nMouseEventFlags;
bool bBtnTodayMouseHover = m_bBtnTodayMouseHover;
bool bBtnNoneMouseHover = m_bBtnNoneMouseHover;
LONG nHitTest = HitTest( point );
DWORD dwDatePickerStyle = GetDatePickerStyle();
	switch( nHitTest )
	{
	case __EDPWH_INNER_AREA:
		{
			MONTH_INFO * pMonth = _HitTestMonth( point );
			if( pMonth != NULL )
			{
				LONG nHitTestMonth = pMonth->HitTest( point );
				switch( nHitTestMonth )
				{
				case __EDPWH_DAYS_AREA:
					if( m_eMTT == __EMTT_DAYS_SELECTING )
					{
						const MONTH_INFO::DATE_INFO * pDayCell = pMonth->HitTestDay( point );
						if( pDayCell != NULL )
						{
							INT nMonth = 0, nYear = 0;
							pMonth->GetMonth( nMonth, nYear );
							COleDateTime dtCell = pDayCell->GetDate();
							if( dtCell.GetStatus() == COleDateTime::valid )
							{
								bool bNonMonthDay = 
									dtCell.GetMonth() == nMonth
									? false
									: true;
								
							if(		!(bNonMonthDay 
								&&	!(dwDatePickerStyle & __EDPWS_SHOW_NON_MONTH_DAYS)) 
								)
							{
									COleDateTime dt = pDayCell->GetDate();
									if( dt.GetStatus() == COleDateTime::valid )
									{
										if(	(dwDatePickerStyle & __EDPWS_MULTIPLE_SELECTION) != 0 )
										{
											COleDateTime dtSelBegin, dtSelEnd;
											SelectionGet( dtSelBegin, dtSelEnd );
											SelectionSet( dtSelBegin, pDayCell->GetDate() );
										}
										else
										{
											//EnsureVisible( dt );
											SelectionSet( dt, dt );
										}
									} // if( dt.GetStatus() == COleDateTime::valid )
								}
							}
						} // if( pDayCell != NULL )
					} // if( m_eMTT == __EMTT_DAYS_SELECTING )
					break;
				}
			} // if( pMonth != NULL )
			break;
		}
		
	case __EDPWH_BTN_TODAY:
		m_bBtnTodayMouseHover = true;
		break;
		
	case __EDPWH_BTN_NONE:
		m_bBtnNoneMouseHover = true;
		break;

	default:
		m_bBtnTodayMouseHover = false;
		m_bBtnNoneMouseHover = false;
	}
bool bNeedRedraw = 
			( m_bBtnTodayMouseHover != bBtnTodayMouseHover )
		||	( m_bBtnNoneMouseHover != bBtnNoneMouseHover );
	if( bNeedRedraw )
		Invalidate( FALSE );
	return true;
}

bool CExtDatePickerWnd::_ProcessMouseClick(
	CPoint point,
	bool bButtonPressed,
	INT nMouseButton, // MK_... values
	UINT nMouseEventFlags
	)
{
	nMouseEventFlags;
	ASSERT_VALID( this );

	if( (GetStyle() & WS_TABSTOP) != 0 )
		SetFocus();

	if( MK_LBUTTON != nMouseButton )
		return false;

	// cancel any kind of clicked mouse tracking
e_MouseTrackType_t eMTT = m_eMTT;
	m_eMTT = __EMTT_NOTHING;
	if( CExtMouseCaptureSink::GetCapture() == m_hWnd )
		CExtMouseCaptureSink::ReleaseCapture();

HWND hWndOwn = GetSafeHwnd();
	ASSERT( hWndOwn != NULL	&& ::IsWindow(hWndOwn) );
				
DWORD dwDatePickerStyle = GetDatePickerStyle();
LONG nHitTest = HitTest( point );
	switch( nHitTest )
	{
	case __EDPWH_INNER_AREA:
		{
			MONTH_INFO * pMonth = _HitTestMonth( point );
			if( pMonth != NULL )
			{
				LONG nHitTestMonth = pMonth->HitTest( point );
				switch( nHitTestMonth )
				{
				case __EDPWH_BTN_BACKWARD:
					if( bButtonPressed )
					{			
						CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
						m_eMTT = __EMTT_SCROLLING_BACKWARD;
						OnDatePickerDoScroll(
							1,
							true
							);
					}
					break;
					
				case __EDPWH_BTN_FORWARD:
					if( bButtonPressed )
					{			
						CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
						m_eMTT = __EMTT_SCROLLING_FORWARD;
						OnDatePickerDoScroll(
							1,
							true
							);
					}
					break;

				case __EDPWH_DAYS_AREA:
					{
						const MONTH_INFO::DATE_INFO * pDayCell = pMonth->HitTestDay( point );
						if( pDayCell != NULL )
						{
							INT nMonth = 0, nYear = 0;
							pMonth->GetMonth( nMonth, nYear );
							bool bNonMonthDay = 
								pDayCell->GetDate().GetMonth() == nMonth
								? false
								: true;
							if(	!(bNonMonthDay && !(dwDatePickerStyle & __EDPWS_SHOW_NON_MONTH_DAYS)) )
							{
								COleDateTime dt = pDayCell->GetDate();
								if( bButtonPressed )
								{
									m_eMTT = __EMTT_DAYS_SELECTING;
									CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
									if( dt.GetStatus() == COleDateTime::valid )
										SelectionSet( dt, dt );
								}
								if(		dt.GetStatus() == COleDateTime::valid
									&&	eMTT == __EMTT_DAYS_SELECTING
									)
								{
									if( !bButtonPressed )
										EnsureVisible( dt );
									COleDateTime dtBegin, dtEnd;
									SelectionGet( dtBegin, dtEnd );
									SelectionSet( dtBegin, dt );
									HWND hWndNotificationReceiver =
										OnDatePickerGetNotificationReceiver();
									if( hWndNotificationReceiver != NULL )
									{
										HWND hWndOwn = GetSafeHwnd();
										SELECTION_NOTIFICATION _SN( m_pExternalSelectionInfo, *this, true );
										_SN.Notify( hWndNotificationReceiver );
										if( ! ::IsWindow(hWndOwn) )
											return true;
									} // if( hWndNotificationReceiver != NULL )
								}
							}						
						} // if( pDayCell != NULL )
					} // if( bButtonPressed )
					break;
					
				case __EDPWH_HEADER_DATE:
					{
						if( bButtonPressed )
						{			
							// header popup list
							CExtDatePickerHeaderPopupWnd * pHeaderPopupWnd =
								new CExtDatePickerHeaderPopupWnd(
								this,
								((dwDatePickerStyle & __EDPWS_SHOW_MONTH_LIST_SHADOW) != 0 ) ? true : false
								);
							ASSERT_VALID( pHeaderPopupWnd );
							if( !pHeaderPopupWnd->CreateEx(
								WS_EX_TOPMOST|WS_EX_WINDOWEDGE,
								::AfxRegisterWndClass(CS_SAVEBITS,NULL,NULL,NULL),
								_T(""),
								WS_POPUP,
								0,0,100,100, 
								m_hWnd,
								NULL,
								NULL
								)
								)
							{
								ASSERT( FALSE );
								return true;
							}
							
							HWND hWndHdr = pHeaderPopupWnd->m_hWnd;
							ASSERT( hWndHdr != NULL && ::IsWindow(hWndHdr) );
							INT nMonth = 0, nYear = 0;
							pMonth->GetMonth( nMonth, nYear );
							pHeaderPopupWnd->SetInitialDate( 
								nMonth,
								nYear
								);
							CSize sizeHeaderPopupWnd = pHeaderPopupWnd->GetSize( false );
							CSize sizeHeaderPopupWndClient = pHeaderPopupWnd->GetSize( true );
							
							CRect rcHeader( pMonth->GetHeaderRect() );
							ClientToScreen( &rcHeader );
							pHeaderPopupWnd->SetWindowPos(
								&CWnd::wndTopMost,
								rcHeader.CenterPoint().x - sizeHeaderPopupWndClient.cx/2,
								rcHeader.CenterPoint().y - sizeHeaderPopupWndClient.cy/2,
								sizeHeaderPopupWnd.cx,
								sizeHeaderPopupWnd.cy,
								SWP_SHOWWINDOW|SWP_NOACTIVATE
								);
							
							CExtMouseCaptureSink::SetCapture( hWndHdr );
							CWinThread * pThread = ::AfxGetThread();
							ASSERT_VALID( pThread );
							bool bAnalyzeDataChanging = false;
							for( bool bStopFlag = false; (!bStopFlag) && ::IsWindow(hWndHdr) ; )
							{
								MSG msg;
								// Process all the messages in the message queue
								while( ::IsWindow(hWndHdr) && PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
								{
									switch( msg.message )
									{
									case WM_LBUTTONUP:
									case WM_LBUTTONDOWN:
									case WM_LBUTTONDBLCLK:
									case WM_RBUTTONUP:
									case WM_RBUTTONDOWN:
									case WM_RBUTTONDBLCLK:
									case WM_MBUTTONUP:
									case WM_MBUTTONDOWN:
									case WM_MBUTTONDBLCLK:
									case WM_NCLBUTTONUP:
									case WM_NCLBUTTONDOWN:
									case WM_NCLBUTTONDBLCLK:
									case WM_NCRBUTTONUP:
									case WM_NCRBUTTONDOWN:
									case WM_NCRBUTTONDBLCLK:
									case WM_NCMBUTTONUP:
									case WM_NCMBUTTONDOWN:
									case WM_NCMBUTTONDBLCLK:
									case WM_MOUSEWHEEL:
									case WM_CONTEXTMENU:
										bStopFlag = true;
										if( msg.message == WM_LBUTTONUP )
											bAnalyzeDataChanging = true;
										//::PeekMessage(&msg,msg.hwnd,msg.message,msg.message,PM_REMOVE);
										break;
									case WM_MOUSEMOVE:
										pHeaderPopupWnd->AnalyzeChangings();
										//::PeekMessage(&msg,msg.hwnd,msg.message,msg.message,PM_REMOVE);
										break;
									case WM_CAPTURECHANGED:
										if( (HWND)msg.wParam != hWndHdr )
											bStopFlag = true;
										break;
									case WM_CANCELMODE:
									case WM_ACTIVATEAPP:
									case WM_SETTINGCHANGE:
									case WM_SYSCOLORCHANGE:
										bStopFlag = true;
										break;
									default:
										if( WM_KEYFIRST <= msg.message && msg.message <= WM_KEYLAST )
											bStopFlag = true;
										break;
									}
									if( CExtMouseCaptureSink::GetCapture() != hWndHdr )
										bStopFlag = true;
									else if(( ! CExtPopupMenuWnd::IsKeyPressed(VK_LBUTTON) )
										||	CExtPopupMenuWnd::IsKeyPressed(VK_MBUTTON)
										||	CExtPopupMenuWnd::IsKeyPressed(VK_RBUTTON)
										||	(	CExtPopupMenuWnd::IsMenuTracking()
											&&	(!(	( (GetStyle()&(WS_POPUP|WS_CHILD|WS_TABSTOP)) == WS_CHILD )
												&&	( GetParent()->IsKindOf( RUNTIME_CLASS(CExtPopupMenuWnd) ) )
												))
											)
										)
										bStopFlag = true;
									if( bStopFlag )
										break;
									ASSERT_VALID( pThread );
									if( ! pThread->PumpMessage() )
										bStopFlag = true;
								} // while( ::IsWindow(hWndHdr) && PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
								if( bStopFlag )
									break;
								if( ! ::WaitMessage() )
									break;
							} // for( bool bStopFlag = false; (!bStopFlag) && ::IsWindow(hWndHdr) ; )
							if( ::IsWindow(hWndHdr) )
							{
								if( CExtMouseCaptureSink::GetCapture() == hWndHdr )
									CExtMouseCaptureSink::ReleaseCapture();
								if( bAnalyzeDataChanging )
								{
									pHeaderPopupWnd->AnalyzeChangings();
									INT	nSelMonth = 0, nSelYear = 0;
									if( pHeaderPopupWnd->GetSelectedDate(
										nSelMonth,
										nSelYear )
										)
									{
										CSize sizeCalendarDimensionsCurrent;
										DimGet( 
											NULL,
											NULL,
											&sizeCalendarDimensionsCurrent
											);

										INT nMonthsDiff = 
											(pMonth->m_nRow)*(sizeCalendarDimensionsCurrent.cx) + (pMonth->m_nCol);
										
										for( INT nMonth=0; nMonth<nMonthsDiff; nMonth++ ){
											nSelMonth -= 1;
											if (nSelMonth < 1) {
												nSelMonth = 12;
												nSelYear--;
											}
										}
										CurrentDateSet( nSelYear, nSelMonth, 1 );
										_RecalcCalendar();
									}
								} // if( bAnalyzeDataChanging )
								::DestroyWindow( hWndHdr );
							} // if( ::IsWindow(hWndHdr) )
						} // if( bButtonPressed )
					}
					break;					
				} // switch( nHitTestMonth )
			} // if( pMonth != NULL )
		} // case __EDPWH_INNER_AREA:
		break;
		
	case __EDPWH_BTN_NONE:
		if( bButtonPressed )
		{
			CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
			m_eMTT = __EMTT_BTN_NONE_PRESSED;
			UpdateDatePickerWnd();	
		}
		else if( eMTT == __EMTT_BTN_NONE_PRESSED )
		{
			SelectionClear();
			HWND hWndNotificationReceiver =
				OnDatePickerGetNotificationReceiver();
			if( hWndNotificationReceiver != NULL )
			{
				HWND hWndOwn = GetSafeHwnd();
				SELECTION_NOTIFICATION _SN( m_pExternalSelectionInfo, *this, true );
				_SN.Notify( hWndNotificationReceiver );
				if( ! ::IsWindow(hWndOwn) )
					return true;
			} // if( hWndNotificationReceiver != NULL )
		}
		break;
		
	case __EDPWH_BTN_TODAY:
		if( bButtonPressed )
		{
			CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
			m_eMTT = __EMTT_BTN_TODAY_PRESSED;
			UpdateDatePickerWnd();	
		}
		else if( eMTT == __EMTT_BTN_TODAY_PRESSED )
		{
			CurrentDateSet( OnDatePickerGetToday() );
			COleDateTime dt = CurrentDateGet();
			if( EnsureVisible( dt ) )
				SelectionSet( dt, dt );
			HWND hWndNotificationReceiver =
				OnDatePickerGetNotificationReceiver();
			if( hWndNotificationReceiver != NULL )
			{
				HWND hWndOwn = GetSafeHwnd();
				SELECTION_NOTIFICATION _SN( m_pExternalSelectionInfo, *this, true );
				_SN.Notify( hWndNotificationReceiver );
				if( ! ::IsWindow(hWndOwn) )
					return true;
			} // if( hWndNotificationReceiver != NULL )
		}
		break;

	default:
		return false; 
	}
	
	if( ::IsWindow(hWndOwn) )
		UpdateDatePickerWnd();

	return true;
}

LONG CExtDatePickerWnd::HitTest(
	const POINT & ptClient,
	COleDateTime * pDT // = NULL
	) const
{
	ASSERT_VALID( this );
	if( pDT != NULL )
		pDT->SetStatus( COleDateTime::invalid );
	if(		GetSafeHwnd() == NULL
		||	( ! ::IsWindow( GetSafeHwnd() ) )
		)
		return __EDPWH_NOWHERE;
CRect rcClient;
	GetClientRect( &rcClient );
	if(		rcClient.IsRectEmpty()
		||	(! rcClient.PtInRect(ptClient) )
		)
		return __EDPWH_NOWHERE;
	if(		(! m_rcBtnNone.IsRectEmpty() )
		&&	m_rcBtnNone.PtInRect(ptClient)
		)
		return __EDPWH_BTN_NONE;
	if(		(! m_rcBtnToday.IsRectEmpty() )
		&&	m_rcBtnToday.PtInRect(ptClient)
		)
		return __EDPWH_BTN_TODAY;
	if(		(! m_rcDatePickerInnerArea.IsRectEmpty() )
		&&	m_rcDatePickerInnerArea.PtInRect(ptClient)
		)
	{
		if( pDT == NULL )
			return __EDPWH_INNER_AREA;
		const MONTH_INFO * pMonth = _HitTestMonth( ptClient );
		if( pMonth == NULL )
			return __EDPWH_INNER_AREA;
		LONG nHitTestMonth = pMonth->HitTest( ptClient, pDT );
		if( nHitTestMonth == __EDPWH_NOWHERE )
			return __EDPWH_INNER_AREA;
		return nHitTestMonth;
	}
	return __EDPWH_NOWHERE;
}

void CExtDatePickerWnd::OnDatePickerDoScroll( 
	INT nMonthCount, // = 1,
	bool bSmoothScroll // = true
	)
{
	ASSERT_VALID( this );
COleDateTime dtCurrentDate = CurrentDateGet();
INT nCurrentMonth = dtCurrentDate.GetMonth();
INT nCurrentYear = dtCurrentDate.GetYear();
	if( m_eMTT == __EMTT_SCROLLING_BACKWARD )
	{ 
		nCurrentMonth -= nMonthCount;
		if( nCurrentMonth < 1 )
		{
			nCurrentMonth= 12;
			nCurrentYear--;
		}
		CurrentDateSet( nCurrentYear, nCurrentMonth, 1 );
		_RecalcCalendar();
		if( bSmoothScroll )	
		{
			::Sleep( __EDPW_SCROLL_TIMER_PERIOD );
			SetTimer(
				__EDPW_SCROLL_TIMER_ID,
				__EDPW_SCROLL_TIMER_PERIOD,
				NULL
				);
		}
	}
	else if( m_eMTT == __EMTT_SCROLLING_FORWARD )
	{ 
		nCurrentMonth += nMonthCount;
		if( nCurrentMonth > 12 )
		{
			nCurrentMonth = 1;
			nCurrentYear++;
		}
		CurrentDateSet( nCurrentYear, nCurrentMonth, 1 );
		_RecalcCalendar();
		if( bSmoothScroll ) 
		{
			::Sleep( __EDPW_SCROLL_TIMER_PERIOD );
			SetTimer(
				__EDPW_SCROLL_TIMER_ID,
				__EDPW_SCROLL_TIMER_PERIOD,
				NULL
				);
		}
	}
}

void CExtDatePickerWnd::OnTimer(UINT nIDEvent) 
{
	if( nIDEvent == __EDPW_SCROLL_TIMER_ID )
	{
		if( m_eMTT == __EMTT_NOTHING )
			KillTimer( __EDPW_SCROLL_TIMER_ID );
		else
			OnDatePickerDoScroll(
				1,
				false
				);
	}
	CWnd::OnTimer(nIDEvent);
}

void CExtDatePickerWnd::OnCancelMode() 
{
	CWnd::OnCancelMode();
	if( m_bCanceling )
		return;
	m_bCanceling = true;
	if( m_eMTT != __EMTT_NOTHING )
	{
		Invalidate( FALSE );
		// cancel any kind of clicked mouse tracking
		m_eMTT = __EMTT_NOTHING;
	}
	if( CExtMouseCaptureSink::GetCapture() == m_hWnd )
		CExtMouseCaptureSink::ReleaseCapture();
	m_bCanceling = false;
}

void CExtDatePickerWnd::OnCaptureChanged(CWnd *pWnd) 
{
	CWnd::OnCaptureChanged(pWnd);
	if(		m_eMTT != __EMTT_NOTHING
		&&	CExtMouseCaptureSink::GetCapture() != m_hWnd
		)
		SendMessage( WM_CANCELMODE );
}

UINT CExtDatePickerWnd::OnGetDlgCode() 
{
	if( (GetStyle() & WS_TABSTOP) != 0 )
		return DLGC_WANTALLKEYS;
	return CWnd::OnGetDlgCode();
}

void CExtDatePickerWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	nRepCnt;
	nFlags;
bool bPopupMode = false;
	if( GetParent()->IsKindOf( RUNTIME_CLASS(CExtPopupMenuWnd) ) )
		bPopupMode = true;
COleDateTime dtSelBegin, dtSelEnd;
	SelectionGet( dtSelBegin, dtSelEnd );
	if( bPopupMode )
	{
		if( dtSelBegin.GetStatus() == COleDateTime::invalid )
			dtSelBegin = COleDateTime::GetCurrentTime();
		if( dtSelEnd.GetStatus() == COleDateTime::invalid )
			dtSelEnd = COleDateTime::GetCurrentTime();
	} // if( bPopupMode )
DWORD dwDatePickerStyle = GetDatePickerStyle();
bool bEnableMultipeSelect =
		((dwDatePickerStyle & __EDPWS_MULTIPLE_SELECTION) != 0 )
			? true : false;
	if( bEnableMultipeSelect )
	{
		bool bAlt = ( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 ) ? true : false;
		bool bCtrl = ( (::GetAsyncKeyState(VK_CONTROL)&0x8000) != 0 ) ? true : false;
		bool bShift = ( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 ) ? true : false;
		bool bShiftOnly = (!bCtrl) && (!bAlt) && bShift;
		if( ! bShiftOnly )
			bEnableMultipeSelect = false;
	} // if( bEnableMultipeSelect )
	switch( nChar )
	{
	case VK_LEFT:
		dtSelEnd -= 1;
		if( ! bEnableMultipeSelect )
			dtSelBegin = dtSelEnd;
		break;
	case VK_RIGHT:
		dtSelEnd += 1;
		if( ! bEnableMultipeSelect )
			dtSelBegin = dtSelEnd;
		break;
	case VK_UP:
		dtSelEnd -= 7;
		if( ! bEnableMultipeSelect )
			dtSelBegin = dtSelEnd;
		break;
	case VK_DOWN:
		dtSelEnd += 7;
		if( ! bEnableMultipeSelect )
			dtSelBegin = dtSelEnd;
		break;
	case VK_TAB:
		if( (GetStyle() & WS_TABSTOP) != 0 )
		{
			HWND hWndParent = ::GetParent(m_hWnd);
			if( hWndParent != NULL )
			{
				bool bShift = ( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 ) ? true : false;
				::SendMessage(
					hWndParent,
					WM_NEXTDLGCTL,
					bShift ? (!0) : 0,
					0
					);
			}
		} // if( (GetStyle() & WS_TABSTOP) != 0 )
		return;
	case VK_RETURN:
		if( m_pExternalSelectionInfo == NULL )
			bPopupMode = false;
		else
			SelectionGet(
				m_pExternalSelectionInfo->m_dtBegin,
				m_pExternalSelectionInfo->m_dtEnd
				);
		GetParent()->PostMessage( WM_CANCELMODE );
		break;
	case VK_ESCAPE:
		if( ! bPopupMode )
			return;
		GetParent()->PostMessage( WM_CANCELMODE );
		break;
	default:
		return;
	} // switch( nChar )
/*
COleDateTime dtDisplayedBegin, dtDisplayedEnd;
	GetDisplayedRange( dtDisplayedBegin, dtDisplayedEnd );
	SelectionSet( dtSelBegin, dtSelEnd );
	CurrentDateSet( dtSelEnd );
	if(		dtDisplayedBegin.GetStatus() == COleDateTime::invalid
		||	dtDisplayedEnd.GetStatus() == COleDateTime::invalid
		)
		_RecalcCalendar();
	else if(
			dtSelEnd < dtDisplayedBegin
		||	dtDisplayedEnd < dtSelEnd 
		)
		_RecalcCalendar();
*/

	SelectionSet( dtSelBegin, dtSelEnd );
	CurrentDateSet( dtSelEnd );
	EnsureVisible( dtSelEnd );

HWND hWndNotificationReceiver =
		OnDatePickerGetNotificationReceiver();
	if( hWndNotificationReceiver != NULL )
	{
		HWND hWndOwn = GetSafeHwnd();
		SELECTION_NOTIFICATION _SN(
			m_pExternalSelectionInfo,
			*this,
			bPopupMode ? false : true // true
			);
		_SN.Notify( hWndNotificationReceiver );
		if( ! ::IsWindow(hWndOwn) )
			return;
	} // if( hWndNotificationReceiver != NULL )
}

void CExtDatePickerWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	UpdateDatePickerWnd();
}

void CExtDatePickerWnd::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);
	if( (GetStyle() & WS_TABSTOP) != 0 )
		SendMessage( WM_CANCELMODE );
}

#if _MFC_VER < 0x700
void CExtDatePickerWnd::OnActivateApp(BOOL bActive, HTASK hTask) 
#else
void CExtDatePickerWnd::OnActivateApp(BOOL bActive, DWORD hTask) 
#endif
{
	CWnd::OnActivateApp(bActive, hTask);
	if( ! bActive )
		SendMessage( WM_CANCELMODE );
}

int CExtDatePickerWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	if( (GetStyle()&WS_TABSTOP) == 0 )
		return MA_NOACTIVATE;
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

#if (!defined __EXT_MFC_NO_DATE_PICKER_POPUP)

/////////////////////////////////////////////////////////////////////////////
// CExtPopupDatePickerMenuWnd

IMPLEMENT_DYNCREATE(CExtPopupDatePickerMenuWnd, CExtPopupControlMenuWnd)

BEGIN_MESSAGE_MAP(CExtPopupDatePickerMenuWnd, CExtPopupControlMenuWnd)
	//{{AFX_MSG_MAP(CExtPopupDatePickerMenuWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CExtPopupDatePickerMenuWnd::CExtPopupDatePickerMenuWnd(
	LPARAM lParamCookie, // = 0L
	CSize sizeCalendarDimensions, // = CSize(1,1)
	DWORD dwDatePickerWindowStyle, // = WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN
	DWORD dwDatePickerStyle // = __EDPWS_DEFAULT
	)
	: m_lParamCookie( lParamCookie )
	, m_sizeCalendarDimensions( sizeCalendarDimensions )
	, m_dwDatePickerWindowStyle( dwDatePickerWindowStyle )
	, m_dwDatePickerStyle( dwDatePickerStyle )
	, m_pExternalSelectionInfo( NULL )
{
	ASSERT( (m_dwDatePickerWindowStyle&WS_CHILD) != 0 );
	ASSERT( (m_dwDatePickerWindowStyle&WS_POPUP) == 0 );
	ASSERT( (m_dwDatePickerWindowStyle&WS_TABSTOP) == 0 );
	ASSERT( m_sizeCalendarDimensions.cx >= 1 );
	ASSERT( m_sizeCalendarDimensions.cy >= 1 );
CExtDatePickerWnd wndTemp;
	wndTemp.DimSet( 
		m_sizeCalendarDimensions,
		m_sizeCalendarDimensions,
		false
		);
	wndTemp.ModifyDatePickerStyle(
		0xFFFFFFFF,
		m_dwDatePickerStyle,
		false
		);
	m_sizeChildControl =
		wndTemp.OnDatePickerCalcContentSize( m_sizeCalendarDimensions );
}

CExtPopupDatePickerMenuWnd::~CExtPopupDatePickerMenuWnd()
{
}

int CExtPopupDatePickerMenuWnd::OnQueryMenuBorderSize() const
{
	ASSERT_VALID( this );
	return 0;
}

HWND CExtPopupDatePickerMenuWnd::OnCreateChildControl(
	const RECT & rcChildControl
	)
{
	ASSERT_VALID( this );
	ASSERT( (m_dwDatePickerWindowStyle&WS_CHILD) != 0 );
	ASSERT( (m_dwDatePickerWindowStyle&WS_POPUP) == 0 );
	ASSERT( (m_dwDatePickerWindowStyle&WS_TABSTOP) == 0 );
	ASSERT( m_sizeCalendarDimensions.cx >= 1 );
	ASSERT( m_sizeCalendarDimensions.cy >= 1 );
CExtDatePickerWnd * pWnd = new CExtDatePickerWnd;
	pWnd->m_pExternalSelectionInfo = m_pExternalSelectionInfo;
	if( ! pWnd->Create(
			this,
			rcChildControl,
			UINT( IDC_STATIC ),
			m_sizeCalendarDimensions,
			m_sizeCalendarDimensions,
			m_dwDatePickerWindowStyle,
			m_dwDatePickerStyle,
			NULL
			)
		)
	{
		ASSERT( FALSE );
		return NULL;
	}
	pWnd->m_bAutoDeleteWindow = true;
	return pWnd->m_hWnd;
}

bool CExtPopupDatePickerMenuWnd::_OnMouseWheel(
	WPARAM wParam,
	LPARAM lParam,
	bool & bNoEat
	)
{
	ASSERT_VALID( this );
	wParam;
	lParam;
	bNoEat = false;
	return true;
}

bool CExtPopupDatePickerMenuWnd::_OnMouseMove(
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
	if( hWndFromPoint != NULL )
	{
		if( (::GetWindowLong(hWndFromPoint,GWL_STYLE)&WS_CHILD) != 0 )
		{
			if(		(	m_hWndChildControl == hWndFromPoint
					||	::IsChild( m_hWndChildControl, hWndFromPoint )
					)
				)
			{
				bNoEat = true;
				return false;
			}
		} // if( (::GetWindowLong(hWndFromPoint,GWL_STYLE)&WS_CHILD) != 0 )
		else
		{
			CWnd * pWnd = CWnd::FromHandlePermanent( hWndFromPoint );
			if(		pWnd != NULL
				&&	pWnd->IsKindOf( RUNTIME_CLASS(CExtDatePickerHeaderPopupWnd) )
				)
			{
				bNoEat = true;
				return false;
			}
		} // else from if( (::GetWindowLong(hWndFromPoint,GWL_STYLE)&WS_CHILD) != 0 )
	} // if( hWndFromPoint != NULL )

	return
		CExtPopupControlMenuWnd::_OnMouseMove(
			nFlags,
			point,
			bNoEat
			);
}

bool CExtPopupDatePickerMenuWnd::_OnMouseClick(
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
	if( hWndFromPoint != NULL )
	{
		if( (::GetWindowLong(hWndFromPoint,GWL_STYLE)&WS_CHILD) != 0 )
		{
			if(		(	m_hWndChildControl == hWndFromPoint
					||	::IsChild( m_hWndChildControl, hWndFromPoint )
					)
				)
			{
				bNoEat = true;
				return false;
			}
		} // if( (::GetWindowLong(hWndFromPoint,GWL_STYLE)&WS_CHILD) != 0 )
		else
		{
			CWnd * pWnd = CWnd::FromHandlePermanent( hWndFromPoint );
			if(		pWnd != NULL
				&&	pWnd->IsKindOf( RUNTIME_CLASS(CExtDatePickerHeaderPopupWnd) )
				)
			{
				bNoEat = true;
				return false;
			}
		} // else from if( (::GetWindowLong(hWndFromPoint,GWL_STYLE)&WS_CHILD) != 0 )
	} // if( hWndFromPoint != NULL )

	return
		CExtPopupControlMenuWnd::_OnMouseClick(
			nFlags,
			point,
			bNoEat
			);
}


LRESULT CExtPopupDatePickerMenuWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == CExtDatePickerWnd::g_nMsgSelectionNotification )
	{
		ASSERT(
				m_hWndCmdReceiver != NULL
				&&	::IsWindow( m_hWndCmdReceiver )
			);
		return
			::SendMessage(
				m_hWndCmdReceiver,
				message,
				wParam,
				lParam
				);
	} // if( message == CExtDatePickerWnd::g_nMsgSelectionNotification )
	return CExtPopupControlMenuWnd::WindowProc(message,wParam,lParam);
}

#endif // (!defined __EXT_MFC_NO_DATE_PICKER_POPUP)

#endif // (!defined __EXT_MFC_NO_DATE_PICKER)

