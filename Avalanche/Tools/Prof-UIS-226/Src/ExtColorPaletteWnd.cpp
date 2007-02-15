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

#if (!defined __EXT_MFC_NO_COLORPALETTE)

#if (!defined __EXT_COLORPALETTE_WND_H)
	#include <ExtColorPaletteWnd.h>
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
// CExtColorPaletteWnd

CExtColorPaletteWnd::CExtColorPaletteWnd()
	: m_dwColorPaletteWndStyle( __ECPWS_DEFAULT_STYLES )
	, m_nColumnCountMin( 1 )
	, m_nColumnCountMax( 32767 )
	, m_clrForeground( RGB(0,0,0) )
	, m_clrBackground( RGB(255,255,255) )
	, m_nTrackItem( __ECPW_CELL_INDEX_INVALID )
	, m_nTrackMouseFlags( 0 )
	, m_bPresubclassDialogMode( true )
{
	VERIFY( RegisterColorPaletteWndClass() );
}

CExtColorPaletteWnd::~CExtColorPaletteWnd()
{
}

bool CExtColorPaletteWnd::Create(
	CWnd * pWndParent,
	DWORD dwColorPaletteWndStyle, // = __ECPWS_DEFAULT_STYLES
	DWORD dwStyle, // = WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS
	DWORD dwExStyle, // = 0L
	UINT nDlgCtrlID, // = ((UINT)(IDC_STATIC))
	__EXT_MFC_SAFE_LPCTSTR strWindowText, // = NULL
	CRect rc // = CRect( 0, 0, 0, 0 )
	)
{
	if( !RegisterColorPaletteWndClass() )
	{
		ASSERT( FALSE );
		return false;
	}
	m_dwColorPaletteWndStyle = dwColorPaletteWndStyle;
	m_bPresubclassDialogMode = false;
	if( !CWnd::CreateEx(
			dwExStyle,
			__EXT_COLOR_PALETTE_WND_CLASS_NAME,
			(strWindowText == NULL) ? _T("") : strWindowText,
			dwStyle,
			rc,
			pWndParent,
			nDlgCtrlID
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	return true;
}

bool CExtColorPaletteWnd::g_bColorPaletteWndClassRegistered = false;
HCURSOR CExtColorPaletteWnd::g_hCursor = NULL;

bool CExtColorPaletteWnd::RegisterColorPaletteWndClass()
{
	if( g_bColorPaletteWndClassRegistered )
		return true;

WNDCLASS wndclass;
HINSTANCE hInst = AfxGetInstanceHandle();
	if( ! ::GetClassInfo(
			hInst,
			__EXT_COLOR_PALETTE_WND_CLASS_NAME,
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
		wndclass.lpszClassName = __EXT_COLOR_PALETTE_WND_CLASS_NAME;
		if( !::AfxRegisterClass( &wndclass ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}

	g_bColorPaletteWndClassRegistered = true;
	return true;
}

DWORD CExtColorPaletteWnd::CpwGetStyle() const
{
	ASSERT_VALID( this );
	return m_dwColorPaletteWndStyle;
}

DWORD CExtColorPaletteWnd::CpwModifyStyle(
	DWORD dwStyleRemove,
	DWORD dwStyleAdd,
	bool bRepaint // = true
	)
{
	ASSERT_VALID( this );
DWORD dwStyleOld = m_dwColorPaletteWndStyle;
	m_dwColorPaletteWndStyle &= ~dwStyleRemove;
	m_dwColorPaletteWndStyle |= dwStyleAdd;
	if( bRepaint )
		_RedrawWnd();
	return dwStyleOld;
}

COLORREF CExtColorPaletteWnd::CpwForegroundColorGet() const
{
	ASSERT_VALID( this );
	return m_clrForeground;
}

COLORREF CExtColorPaletteWnd::CpwForegroundColorSet(
	COLORREF clr,
	bool bRepaint // = true
	)
{
	ASSERT_VALID( this );
	if( m_clrForeground == clr )
		return clr;
COLORREF clrOld = m_clrForeground;
	OnCpwActiveColorChange(
		true,
		clrOld,
		clr,
		bRepaint
		);
	return clrOld;
}

COLORREF CExtColorPaletteWnd::CpwBackgroundColorGet() const
{
	ASSERT_VALID( this );
	return m_clrBackground;
}

COLORREF CExtColorPaletteWnd::CpwBackgroundColorSet(
	COLORREF clr,
	bool bRepaint // = true
	)
{
	ASSERT_VALID( this );
	if( m_clrBackground == clr )
		return clr;
COLORREF clrOld = m_clrBackground;
	OnCpwActiveColorChange(
		false,
		clrOld,
		clr,
		bRepaint
		);
	return clrOld;
}

CExtColorPaletteWnd::CColorArray & CExtColorPaletteWnd::CpwColorArrayGet()
{
	ASSERT_VALID( this );
	return m_arrColors;
}

const CExtColorPaletteWnd::CColorArray & CExtColorPaletteWnd::CpwColorArrayGet() const
{
	ASSERT_VALID( this );
	return m_arrColors;
}

INT CExtColorPaletteWnd::CpwColorArrayGetCount() const
{
	ASSERT_VALID( this );
INT nCount = m_arrColors.GetSize();
	return nCount;
}

COLORREF CExtColorPaletteWnd::CpwColorArrayGetAt( INT nPos ) const
{
	ASSERT_VALID( this );
	ASSERT( 0 <= nPos && nPos < m_arrColors.GetSize() );
COLORREF clr = m_arrColors.GetAt( nPos );
	return clr;
}

INT CExtColorPaletteWnd::CpwColorGetIndexOf( COLORREF clr ) const
{
	ASSERT_VALID( this );
INT nCount = m_arrColors.GetSize();
	for( INT i = 0; i < nCount; i++ )
	{
		COLORREF clrToCmp = m_arrColors[i];
		if( clr == clrToCmp )
			return i;
	}
	if( (CpwGetStyle() & __ECPWS_SHOW_ACTIVE_COLOR_AREA) != 0 )
	{
		if( clr == m_clrForeground )
			return __ECPW_CELL_INDEX_FOREGROUND;
		if( (CpwGetStyle() & __ECPWS_SHOW_TWO_COLORS) != 0 )
		{
			if( clr == m_clrBackground )
				return __ECPW_CELL_INDEX_BACKGROUND;
		} // if( (CpwGetStyle() & __ECPWS_SHOW_TWO_COLORS) != 0 )
	} // if( (CpwGetStyle() & __ECPWS_SHOW_ACTIVE_COLOR_AREA) != 0 )
	return __ECPW_CELL_INDEX_INVALID;
}

INT CExtColorPaletteWnd::CpwHitTest(
	POINT ptClient,
	LPRECT pRcItem, // = NULL
	COLORREF * pColor // = NULL
	) const
{
	ASSERT_VALID( this );
	if( pRcItem != NULL )
		::SetRectEmpty( pRcItem );
	if( pColor != NULL )
		*pColor = RGB( 0, 0, 0 );

INT nStartRow = 0, nEndRow = 0;
bool bActiveColorAreaVisible = false;
	if( !OnCpwCalcVisibleRowRange(
			nStartRow,
			nEndRow,
			bActiveColorAreaVisible
			)
		)
		return __ECPW_CELL_INDEX_INVALID;

CPoint _ptScroll( 0, 0 );
CSize _sizeScrollTotal = OnSwGetTotalSize();
	if( _sizeScrollTotal.cy >= 0 )
		_ptScroll = OnSwGetScrollPos();

	if( bActiveColorAreaVisible )
	{
		ASSERT( (CpwGetStyle() & __ECPWS_SHOW_ACTIVE_COLOR_AREA) != 0 );
		CRect rcAreaAll, rcItemForeground, rcItemBackground;
		OnCpwCalcActiveColorRects(
			rcAreaAll,
			rcItemForeground,
			rcItemBackground
			);
		rcAreaAll.OffsetRect( -_ptScroll );
		if( rcAreaAll.PtInRect(ptClient) )
		{
			rcItemForeground.OffsetRect( -_ptScroll );
			if( rcItemForeground.PtInRect(ptClient) )
			{
				if( pRcItem != NULL )
					::CopyRect( pRcItem, &rcItemForeground );
				if( pColor != NULL )
					*pColor = m_clrForeground;
				return __ECPW_CELL_INDEX_FOREGROUND;
			} // if( rcItemForeground.PtInRect(ptClient) )
			if( (CpwGetStyle() & __ECPWS_SHOW_TWO_COLORS) != 0 )
			{
				rcItemBackground.OffsetRect( -_ptScroll );
				if( rcItemBackground.PtInRect(ptClient) )
				{
					if( pRcItem != NULL )
						::CopyRect( pRcItem, &rcItemBackground );
					if( pColor != NULL )
						*pColor = m_clrBackground;
					return __ECPW_CELL_INDEX_BACKGROUND;
				} // if( rcItemBackground.PtInRect(ptClient) )
			} // if( (CpwGetStyle() & __ECPWS_SHOW_TWO_COLORS) != 0 )
		} // if( rcAreaAll.PtInRect(ptClient) )
	} // if( bActiveColorAreaVisible )

INT nColumnCount =
		OnCpwCalcColumnCount(
			CpwColumnCountMinGet(),
			CpwColumnCountMaxGet()
			);
CSize _sizeItem, _sizeBetweenItems, _sizeOuterDistance;
	OnCpwMeasureSizes(
		_sizeItem,
		_sizeBetweenItems,
		_sizeOuterDistance
		);

	for( INT nRow = nStartRow; nRow <= nEndRow; nRow++ )
	{
		INT nStartItem = 0, nEndItem = 0;
		if(	!OnCpwCalcRowRange(
				nRow,
				nStartItem,
				nEndItem
				)
			)
			continue;
		for( INT nItem = nStartItem; nItem <= nEndItem; nItem++ )
		{
			CRect rcItem;
			OnCpwCalcItemRect(
				nItem,
				rcItem,
				nColumnCount,
				_sizeItem,
				_sizeBetweenItems,
				_sizeOuterDistance
				);
			rcItem.OffsetRect( -_ptScroll );
			if(		ptClient.y < rcItem.top
				||	ptClient.y > rcItem.bottom
				)
				break;
			if( rcItem.PtInRect(ptClient) )
			{
				if( pRcItem != NULL )
					::CopyRect( pRcItem, &rcItem );
				if( pColor != NULL )
					*pColor = m_arrColors[nItem];
				return nItem;
			} // if( rcItem.PtInRect(ptClient) )
		} // for( INT nItem = nStartItem; nItem <= nEndItem; nItem++ )
	} // for( INT nRow = nStartRow; nRow <= nEndRow; nRow++ )

	return __ECPW_CELL_INDEX_INVALID;
}

void CExtColorPaletteWnd::CpwColorArraySet(
	const CExtColorPaletteWnd::CColorArray & arrColors,
	bool bReplaceExisting, // = true false - append to existing
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( bReplaceExisting )
		m_arrColors.RemoveAll();
	m_arrColors.Append( arrColors );
	if( (CpwGetStyle() & __ECPWS_UNIQUE_COLORS) != 0 )
		CpwEnsureUniqueColorValues( false );
	if( bRedraw )
		_RedrawWnd();
}

COLORREF CExtColorPaletteWnd::CpwColorArraySetAt(
	INT nPos,
	COLORREF clrNew,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( 0 <= nPos && nPos < m_arrColors.GetSize() );
COLORREF clrOld = m_arrColors.GetAt( nPos );
	if( (CpwGetStyle() & __ECPWS_UNIQUE_COLORS) != 0 )
		CpwEnsureUniqueColorValues( false );
	if( clrOld != clrNew )
	{
		m_arrColors.SetAt( nPos, clrNew );
		if( bRedraw )
			Invalidate();
	} // if( clrOld != clrNew )
	return clrOld;
}

void CExtColorPaletteWnd::CpwEnsureUniqueColorValues(
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
INT nCount = m_arrColors.GetSize();
	if( nCount < 2 )
		return;
bool bModified = false;
	for( INT i = 0; i < (nCount-1); i++ )
	{
		COLORREF clrTest = m_arrColors[i];
		for( INT j = i+1; j < nCount;  )
		{
			COLORREF clrToCmp = m_arrColors[j];
			if( clrTest == clrToCmp )
			{
				m_arrColors.RemoveAt( j, 1 );
				nCount --;
				bModified = true;
			} // if( clrTest == clrToCmp )
			else
				j++;
		} // for( INT j = i+1; j < nCount; )
	} // for( INT i = 0; i < (nCount-1); i++ )
	if( bRedraw && bModified )
		_RedrawWnd();
}

void CExtColorPaletteWnd::CpwRedrawWnd()
{
	ASSERT_VALID( this );
	_RedrawWnd();
}

INT CExtColorPaletteWnd::CpwColumnCountMinGet() const
{
	ASSERT_VALID( this );
	ASSERT( m_nColumnCountMin >= 1 );
	ASSERT( m_nColumnCountMin <= m_nColumnCountMax );
	if( (CpwGetStyle() & __ECPWS_SHOW_ACTIVE_COLOR_AREA) != 0 )
	{
		if( m_nColumnCountMin < 2 )
			return 2;
	}
	return m_nColumnCountMin;
}

INT CExtColorPaletteWnd::CpwColumnCountMinSet(
	INT nCount, // = 1
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( m_nColumnCountMin >= 1 );
	ASSERT( m_nColumnCountMin <= m_nColumnCountMax );
INT nCountOld = m_nColumnCountMin;
	m_nColumnCountMin = nCount;
	if( m_nColumnCountMin < 1 )
		m_nColumnCountMin = 1;
	if( m_nColumnCountMax < m_nColumnCountMin )
		m_nColumnCountMax = m_nColumnCountMin;
	if( bRedraw && nCountOld != m_nColumnCountMin )
		_RedrawWnd();
	return nCountOld;
}

INT CExtColorPaletteWnd::CpwColumnCountMaxGet() const
{
	ASSERT_VALID( this );
	ASSERT( m_nColumnCountMin >= 1 );
	ASSERT( m_nColumnCountMin <= m_nColumnCountMax );
	if( (CpwGetStyle() & __ECPWS_SHOW_ACTIVE_COLOR_AREA) != 0 )
	{
		if( m_nColumnCountMax < 2 )
			return 2;
	}
	return m_nColumnCountMax;
}

INT CExtColorPaletteWnd::CpwColumnCountMaxSet(
	INT nCount, // = 32767
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( m_nColumnCountMin >= 1 );
	ASSERT( m_nColumnCountMin <= m_nColumnCountMax );
INT nCountOld = m_nColumnCountMax;
	m_nColumnCountMax = nCount;
	if( m_nColumnCountMax < 1 )
		m_nColumnCountMax = 1;
	if( m_nColumnCountMax < m_nColumnCountMin )
		m_nColumnCountMin = m_nColumnCountMax;
	if( bRedraw && nCountOld != m_nColumnCountMax )
		_RedrawWnd();
	return nCountOld;
}

void CExtColorPaletteWnd::OnCpwMeasureSizes(
	CSize & _sizeItem,
	CSize & _sizeBetweenItems,
	CSize & _sizeOuterDistance
	) const
{
	ASSERT_VALID( this );
	_sizeItem.cx = _sizeItem.cy = 15;
	_sizeBetweenItems.cx = _sizeBetweenItems.cy
		= _sizeOuterDistance.cx = _sizeOuterDistance.cy
		= 3;
}

void CExtColorPaletteWnd::OnCpwCalcItemRect(
	INT nPos,
	CRect & rcItem,
	INT nColumnCount,
	const CSize & _sizeItem,
	const CSize & _sizeBetweenItems,
	const CSize & _sizeOuterDistance
	) const
{
	ASSERT_VALID( this );
	ASSERT( 0 <= nPos && nPos < m_arrColors.GetSize() );
	ASSERT( nColumnCount >= 1 );
	rcItem.SetRect( 0, 0, _sizeItem.cx, _sizeItem.cy );
	if( (CpwGetStyle() & __ECPWS_SHOW_ACTIVE_COLOR_AREA) != 0 )
	{
		if( nPos < nColumnCount-2 )
			nPos += 2;
		else
			nPos += 4;
	}
INT nRow = nPos / nColumnCount;
INT nColumn = nPos % nColumnCount;

	rcItem.OffsetRect(
		_sizeOuterDistance.cx
			+ nColumn * (_sizeItem.cx + _sizeBetweenItems.cx),
		_sizeOuterDistance.cy
			+ nRow * (_sizeItem.cy + _sizeBetweenItems.cy)
		);

	if( (CpwGetStyle() & __ECPWS_HCENTER) != 0 )
	{
		INT nHorzExtentRequired =
			_sizeOuterDistance.cx * 2
			+ nColumnCount * (_sizeItem.cx + _sizeBetweenItems.cx)
			- _sizeBetweenItems.cx
			;
		ASSERT( nHorzExtentRequired > 0 );
		CSize _sizeClient = OnSwGetClientRect().Size();
		if( _sizeClient.cx > nHorzExtentRequired )
		{
			INT nXOffset =
				( _sizeClient.cx - nHorzExtentRequired )
				/ 2
				;
			rcItem.OffsetRect( nXOffset, 0 );
		} // if( _sizeClient.cx > nHorzExtentRequired )
	} // if( (CpwGetStyle() & __ECPWS_HCENTER) != 0 )
}

void CExtColorPaletteWnd::OnCpwCalcActiveColorRects(
	CRect & rcAreaAll,
	CRect & rcItemForeground,
	CRect & rcItemBackground
	) const
{
	ASSERT_VALID( this );
	ASSERT( (CpwGetStyle() & __ECPWS_SHOW_ACTIVE_COLOR_AREA) != 0 );
CSize _sizeItem, _sizeBetweenItems, _sizeOuterDistance;
	OnCpwMeasureSizes(
		_sizeItem,
		_sizeBetweenItems,
		_sizeOuterDistance
		);
	rcAreaAll.SetRect(
		0,
		0,
		_sizeItem.cx*2 + _sizeBetweenItems.cx,
		_sizeItem.cy*2 + _sizeBetweenItems.cy
		);
	rcAreaAll.OffsetRect( _sizeOuterDistance );
	rcItemForeground =
		CRect(
			rcAreaAll.TopLeft(),
			_sizeItem
			);
CSize _sizeAll = rcAreaAll.Size();
	rcItemForeground.OffsetRect(
		(_sizeAll.cx - _sizeItem.cx) / 2,
		(_sizeAll.cy - _sizeItem.cy) / 2
		);
	rcItemBackground = rcItemForeground;
	if( (CpwGetStyle() & __ECPWS_SHOW_TWO_COLORS) != 0 )
	{
		CSize _size3(
			_sizeItem.cx / 3,
			_sizeItem.cy / 3
			);
		rcItemForeground.OffsetRect( -_size3.cx, -_size3.cx );
		rcItemBackground.OffsetRect( _size3.cx, _size3.cx );
	} // if( (CpwGetStyle() & __ECPWS_SHOW_TWO_COLORS) != 0 )
	if( (CpwGetStyle() & __ECPWS_HCENTER) != 0 )
	{
		INT nColumnCount =
			OnCpwCalcColumnCount(
				CpwColumnCountMinGet(),
				CpwColumnCountMaxGet()
				);
		INT nHorzExtentRequired =
			_sizeOuterDistance.cx * 2
			+ nColumnCount * (_sizeItem.cx + _sizeBetweenItems.cx)
			- _sizeBetweenItems.cx
			;
		ASSERT( nHorzExtentRequired > 0 );
		CSize _sizeClient = OnSwGetClientRect().Size();
		if( _sizeClient.cx > nHorzExtentRequired )
		{
			INT nXOffset =
				( _sizeClient.cx - nHorzExtentRequired )
				/ 2
				;
			rcAreaAll.OffsetRect( nXOffset, 0 );
			rcItemForeground.OffsetRect( nXOffset, 0 );
			rcItemBackground.OffsetRect( nXOffset, 0 );
		} // if( _sizeClient.cx > nHorzExtentRequired )
	} // if( (CpwGetStyle() & __ECPWS_HCENTER) != 0 )
}

INT CExtColorPaletteWnd::OnCpwCalcColumnCount(
	INT nMin,
	INT nMax
	) const
{
	ASSERT_VALID( this );
	ASSERT( nMin >= 1 );
	ASSERT( nMin <= nMax );
	if( (CpwGetStyle() & __ECPWS_SHOW_ACTIVE_COLOR_AREA) != 0 )
	{
		if( nMin < 2 )
			nMin = 2;
		if( nMax < 2 )
			nMax = 2;
	}
INT nColumnCount = nMin;
CSize _sizeItem, _sizeBetweenItems, _sizeOuterDistance;
	OnCpwMeasureSizes(
		_sizeItem,
		_sizeBetweenItems,
		_sizeOuterDistance
		);
CRect rcClient = OnSwGetClientRect();
INT nAvailWidth =
		rcClient.right
		- rcClient.left
		- _sizeOuterDistance.cx*2
		;
INT nColumnWidth = _sizeItem.cx + _sizeBetweenItems.cx;
	if( nAvailWidth <= nColumnWidth )	
		return nColumnCount; // minimal
	nAvailWidth += _sizeBetweenItems.cx;
	nColumnCount = nAvailWidth / nColumnWidth;
	if( nColumnCount < nMin )
		nColumnCount = nMin;
	else if( nColumnCount > nMax )
		nColumnCount = nMax;
	return nColumnCount;
}

INT CExtColorPaletteWnd::OnCpwCalcTotalHeight() const
{
	ASSERT_VALID( this );
CSize _sizeItem, _sizeBetweenItems, _sizeOuterDistance;
	OnCpwMeasureSizes(
		_sizeItem,
		_sizeBetweenItems,
		_sizeOuterDistance
		);
INT nColumnCount =
		OnCpwCalcColumnCount(
			CpwColumnCountMinGet(),
			CpwColumnCountMaxGet()
			);
INT nCount = m_arrColors.GetSize();
	if( (CpwGetStyle() & __ECPWS_SHOW_ACTIVE_COLOR_AREA) != 0 )
		nCount += 4;
INT nRowCount = nCount / nColumnCount;
	if( (nCount % nColumnCount) != 0 )
		nRowCount ++;
INT nHeight =
		_sizeOuterDistance.cy * 2
		+ nRowCount*(_sizeItem.cy + _sizeBetweenItems.cy)
		;
	if( (nCount % nColumnCount) != 0 )
		nHeight -= _sizeBetweenItems.cy;
	return nHeight;
}

bool CExtColorPaletteWnd::OnCpwCalcVisibleRowRange(
	INT & nStartRow,
	INT & nEndRow,
	bool & bActiveColorAreaVisible
	) const
{
	ASSERT_VALID( this );
	nStartRow = nEndRow = 0;
	bActiveColorAreaVisible = false;

INT nCount = m_arrColors.GetSize();
	if( nCount == 0 )
		return false;
CRect rcClient = OnSwGetClientRect();
	if(		rcClient.bottom < rcClient.top
		||	rcClient.right < rcClient.left
		)
		return false;

INT nColumnCount =
		OnCpwCalcColumnCount(
			CpwColumnCountMinGet(),
			CpwColumnCountMaxGet()
			);
	if( (CpwGetStyle() & __ECPWS_SHOW_ACTIVE_COLOR_AREA) != 0 )
		nCount += 4;
INT nRowCount = nCount / nColumnCount;
	if( (nCount % nColumnCount) != 0 )
		nRowCount ++;

CPoint _ptScroll( 0, 0 );
CSize _sizeScrollTotal = OnSwGetTotalSize();
	if( _sizeScrollTotal.cy >= 0 )
		_ptScroll = OnSwGetScrollPos();
CSize _sizeClient = rcClient.Size();

CSize _sizeItem, _sizeBetweenItems, _sizeOuterDistance;
	OnCpwMeasureSizes(
		_sizeItem,
		_sizeBetweenItems,
		_sizeOuterDistance
		);

	_sizeClient.cy -= _sizeOuterDistance.cy*2;
	if( _sizeClient.cy <= 0 )
		return false;

INT nVisRows = _sizeClient.cy / (_sizeItem.cy + _sizeBetweenItems.cy) + 1;

	nEndRow = nVisRows;

	if( _ptScroll.y > 0 )
	{
		INT nOffset = _ptScroll.y / (_sizeItem.cy + _sizeBetweenItems.cy);
		nStartRow += nOffset;
		nEndRow += nOffset;
	}

	if( nStartRow >= nRowCount )
		nStartRow = nRowCount - 1;
	if( nEndRow >= nRowCount )
		nEndRow = nRowCount - 1;
	ASSERT( nStartRow <= nEndRow );

	if( (CpwGetStyle() & __ECPWS_SHOW_ACTIVE_COLOR_AREA) != 0 )
	{
		if( nStartRow < 2 )
			bActiveColorAreaVisible = true;
	}

	return true;
}

bool CExtColorPaletteWnd::OnCpwCalcRowRange(
	INT nRow,
	INT & nStartItem,
	INT & nEndItem
	) const
{
	ASSERT_VALID( this );
	nStartItem = nEndItem = 0;
	if( nRow < 0 )
	{
		ASSERT( FALSE );
		return false;
	}

INT nCount = m_arrColors.GetSize();
	if( nCount == 0 )
		return false;

INT nColumnCount =
		OnCpwCalcColumnCount(
			CpwColumnCountMinGet(),
			CpwColumnCountMaxGet()
			);
	if( (CpwGetStyle() & __ECPWS_SHOW_ACTIVE_COLOR_AREA) != 0 )
		nCount += 4;
INT nRowCount = nCount / nColumnCount;
	if( (nCount % nColumnCount) != 0 )
		nRowCount ++;
	if( nRow > nRowCount )
	{
		ASSERT( FALSE );
		return false;
	}

	if( (CpwGetStyle() & __ECPWS_SHOW_ACTIVE_COLOR_AREA) != 0 )
	{
		ASSERT( nColumnCount >= 2 );
		if( nRow == 0 )
		{
			if( nColumnCount == 2 )
				return false;
			nEndItem = nColumnCount - 3;
		}
		else if( nRow == 1 )
		{
			if( nColumnCount == 2 )
				return false;
			nStartItem = nColumnCount - 2;
			nEndItem = nColumnCount*2 - 5;
		}
		else
		{
			nStartItem = nRow * nColumnCount;
			nStartItem -= 4;
			nEndItem = nStartItem + nColumnCount - 1;
		}
		nCount -= 4;
		if( nStartItem >= nCount )
			return false;
		ASSERT( 0 <= nEndItem );
		if( nEndItem >= nCount )
			nEndItem = nCount - 1;
		ASSERT( 0 <= nStartItem && nStartItem < nCount );
		ASSERT( 0 <= nStartItem && nStartItem <= nEndItem );
		ASSERT( 0 <= nEndItem && nEndItem < nCount );
		return true;
	} // if( (CpwGetStyle() & __ECPWS_SHOW_ACTIVE_COLOR_AREA) != 0 )

	nStartItem = nRow * nColumnCount;
	nEndItem = nStartItem + nColumnCount - 1;
	if( nEndItem >= nCount )
		nEndItem = nCount - 1;
	ASSERT( 0 <= nStartItem && nStartItem <= nEndItem );
	ASSERT( 0 <= nEndItem && nEndItem < nCount );
	return true;
}

CExtSafeString CExtColorPaletteWnd::OnCpwGetTooltipText(
	INT nItem,
	CPoint ptClient,
	const CRect & rcItem,
	COLORREF clr
	) const
{
	ASSERT_VALID( this );
	ptClient;
	rcItem;
CExtSafeString strTooltipText;
	strTooltipText.Format(
		_T("#%02X%02X%02X"),
		(INT)GetRValue(clr),
		(INT)GetGValue(clr),
		(INT)GetBValue(clr)
		);
	if( nItem >= 0 && m_arrColors.GetSize() > 0 )
	{
		CExtSafeString strPaletteInfo;
		strPaletteInfo.Format(
			_T(" (%d/%d)"),
			nItem,
			m_arrColors.GetSize()
			);
		strTooltipText += strPaletteInfo;
	}
	return strTooltipText;
}

CSize CExtColorPaletteWnd::OnSwGetTotalSize() const
{
	ASSERT_VALID( this );
INT nHeight = 0;
	if( (CpwGetStyle() & __ECPWS_NO_VSCROLLING) == 0 )
		nHeight = OnCpwCalcTotalHeight();
	return CSize( 0, nHeight );
}

CSize CExtColorPaletteWnd::OnSwGetPageSize( int nDirection ) const
{
	ASSERT_VALID( this );
	 nDirection;
CRect rcClient = OnSwGetClientRect();
CSize _sizePage = rcClient.Size();
CSize _sizeLine = OnSwGetLineSize( nDirection );
	if( _sizePage.cx > (_sizeLine.cx*3) )
		_sizePage.cx -= _sizeLine.cx;
	else if( _sizePage.cx < _sizeLine.cx )
		_sizePage.cx = _sizeLine.cx;
	if( _sizePage.cy > (_sizeLine.cy*3) )
		_sizePage.cy -= _sizeLine.cy;
	else if( _sizePage.cy < _sizeLine.cy )
		_sizePage.cy = _sizeLine.cy;
	return _sizePage;
}

CSize CExtColorPaletteWnd::OnSwGetLineSize( int nDirection ) const
{
	ASSERT_VALID( this );
	nDirection;
CSize _sizeItem, _sizeBetweenItems, _sizeOuterDistance;
	OnCpwMeasureSizes(
		_sizeItem,
		_sizeBetweenItems,
		_sizeOuterDistance
		);
	return _sizeItem + _sizeBetweenItems;
}

bool CExtColorPaletteWnd::OnSwDoMouseWheel(
	UINT fFlags,
	short zDelta,
	CPoint point
	)
{
	ASSERT_VALID( this );
	if( m_nTrackItem != __ECPW_CELL_INDEX_INVALID )
		return true;
	_CancelActions();
	return
		CExtScrollWnd::OnSwDoMouseWheel( fFlags, zDelta, point );
}

void CExtColorPaletteWnd::OnCpwPaintItem(
	CDC & dc,
	INT nItemID,
	const CRect & rcItem,
	COLORREF clrItem,
	COLORREF clrBorder
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( !rcItem.IsRectEmpty() );
	ASSERT(
			nItemID == __ECPW_CELL_INDEX_FOREGROUND
		||	nItemID == __ECPW_CELL_INDEX_BACKGROUND
		||	(	0 <= nItemID
			&&	nItemID < CpwColorArrayGetCount()
			)
		);
	nItemID;
	dc.FillSolidRect( &rcItem, clrItem );
	dc.Draw3dRect( &rcItem, clrBorder, clrBorder );
}

void CExtColorPaletteWnd::OnSwPaint( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

CRect rcClipBox;
	dc.GetClipBox( &rcClipBox );
//	dc.FillSolidRect(
//		&rcClipBox,
//		g_PaintManager->GetColor(
//			CExtPaintManager::CLR_3DFACE_OUT
//			)
//		);
	if( g_PaintManager->GetCb2DbTransparentMode(this) )
	{
		CExtPaintManager::stat_ExcludeChildAreas(
			dc,
			GetSafeHwnd(),
			CExtPaintManager::stat_DefExcludeChildAreaCallback
			);
		g_PaintManager->PaintDockerBkgnd( dc, this );
	} // if( g_PaintManager->GetCb2DbTransparentMode(this) )
	else
		dc.FillSolidRect( &rcClipBox, g_PaintManager->GetColor(CExtPaintManager::CLR_3DFACE_OUT) );

INT nStartRow = 0, nEndRow = 0;
bool bActiveColorAreaVisible = false;
	if( !OnCpwCalcVisibleRowRange(
			nStartRow,
			nEndRow,
			bActiveColorAreaVisible
			)
		)
		return;

CSize _sizeItem, _sizeBetweenItems, _sizeOuterDistance;
	OnCpwMeasureSizes(
		_sizeItem,
		_sizeBetweenItems,
		_sizeOuterDistance
		);
INT nColumnCount =
		OnCpwCalcColumnCount(
			CpwColumnCountMinGet(),
			CpwColumnCountMaxGet()
			);

COLORREF clrBorder =
		g_PaintManager->GetColor(
			COLOR_3DSHADOW
			);
	if( bActiveColorAreaVisible )
	{
		ASSERT( (CpwGetStyle() & __ECPWS_SHOW_ACTIVE_COLOR_AREA) != 0 );
		CRect rcAreaAll, rcItemForeground, rcItemBackground;
		OnCpwCalcActiveColorRects(
			rcAreaAll,
			rcItemForeground,
			rcItemBackground
			);
		dc.FillRect(
			&rcAreaAll,
			&g_PaintManager->m_brushLightDefault
			);
		dc.Draw3dRect( &rcAreaAll, clrBorder, clrBorder );
		if( (CpwGetStyle() & __ECPWS_SHOW_TWO_COLORS) != 0 )
		{
			OnCpwPaintItem(
				dc,
				__ECPW_CELL_INDEX_BACKGROUND,
				rcItemBackground,
				m_clrBackground,
				clrBorder
				);
		}
		OnCpwPaintItem(
			dc,
			__ECPW_CELL_INDEX_FOREGROUND,
			rcItemForeground,
			m_clrForeground,
			clrBorder
			);
	} // if( bActiveColorAreaVisible )

	for( INT nRow = nStartRow; nRow <= nEndRow; nRow++ )
	{
		INT nStartItem = 0, nEndItem = 0;
		if(	!OnCpwCalcRowRange(
				nRow,
				nStartItem,
				nEndItem
				)
			)
			continue;
		for( INT nItem = nStartItem; nItem <= nEndItem; nItem++ )
		{
			CRect rcItem;
			OnCpwCalcItemRect(
				nItem,
				rcItem,
				nColumnCount,
				_sizeItem,
				_sizeBetweenItems,
				_sizeOuterDistance
				);
			OnCpwPaintItem(
				dc,
				nItem,
				rcItem,
				m_arrColors[nItem],
				clrBorder
				);
		} // for( INT nItem = nStartItem; nItem <= nEndItem; nItem++ )
	} // for( INT nRow = nStartRow; nRow <= nEndRow; nRow++ )

}

BEGIN_MESSAGE_MAP(CExtColorPaletteWnd, CExtScrollWnd)
	//{{AFX_MSG_MAP(CExtColorPaletteWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_CAPTURECHANGED()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CExtColorPaletteWnd message handlers

void CExtColorPaletteWnd::_RequestFocus()
{
	ASSERT_VALID( this );
	if( (CpwGetStyle() & __ECPWS_REQUEST_FOCUS) == 0 )
		return;
HWND hWndFocus = ::GetFocus();
	if( hWndFocus == m_hWnd )
		return;
	if(		hWndFocus == NULL
		||	(! ::IsChild(m_hWnd,hWndFocus) )
		)
		SetFocus();
}

void CExtColorPaletteWnd::_RedrawWnd()
{
	ASSERT_VALID( this );
	OnSwUpdateScrollBars();
	OnSwDoRedraw();
}

BOOL CExtColorPaletteWnd::PreTranslateMessage(MSG* pMsg) 
{
	if(		(CpwGetStyle() & __ECPWS_TOOLTIPS) != 0
		&&	m_wndToolTip.GetSafeHwnd() != NULL
		)
		m_wndToolTip.RelayEvent( pMsg );
	
	return CExtScrollWnd::PreTranslateMessage(pMsg);
}

void CExtColorPaletteWnd::PreSubclassWindow() 
{
	CExtScrollWnd::PreSubclassWindow();

	if( m_bPresubclassDialogMode )
	{
		DWORD dwStyle = ::GetWindowLong( m_hWnd, GWL_STYLE );
		m_dwColorPaletteWndStyle = dwStyle & __EXTMFC_ALL_FORM_MOVABLE_WND_STYLES;
		::SetWindowLong( m_hWnd, GWL_STYLE, dwStyle & (~__EXTMFC_ALL_FORM_MOVABLE_WND_STYLES) );
	} // if( m_bPresubclassDialogMode )

	if(		(CpwGetStyle() & __ECPWS_REQUEST_FOCUS) == 0
		&&	( GetStyle() & WS_TABSTOP ) != 0
		)
		CpwModifyStyle( 0, __ECPWS_REQUEST_FOCUS, false );

	EnableToolTips( TRUE );
}

void CExtColorPaletteWnd::PostNcDestroy() 
{
	m_bPresubclassDialogMode = true;
	CExtScrollWnd::PostNcDestroy();
}

void CExtColorPaletteWnd::_CancelActions()
{
	ASSERT_VALID( this );
	m_nTrackItem = __ECPW_CELL_INDEX_INVALID;
	if( CExtMouseCaptureSink::GetCapture() == m_hWnd )
		CExtMouseCaptureSink::ReleaseCapture();
	if(		m_wndToolTip.GetSafeHwnd() != NULL
		&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
		)
		m_wndToolTip.DelTool( this, 1 );
	CWnd::CancelToolTips();
}

bool CExtColorPaletteWnd::OnCpwActiveColorChange(
	bool bForeground, // true - foreground color, false - background color
	COLORREF clrOld,
	COLORREF clrNew,
	bool bRepaint // = true
	)
{
	ASSERT_VALID( this );
	if( clrNew == clrOld )
		return true;
CRect rcInvalidate( 0, 0, 0, 0 );
CRect rcAreaAll, rcItemForeground, rcItemBackground;
	OnCpwCalcActiveColorRects(
		rcAreaAll,
		rcItemForeground,
		rcItemBackground
		);
CPoint _ptScroll( 0, 0 );
CSize _sizeScrollTotal = OnSwGetTotalSize();
	if( _sizeScrollTotal.cy >= 0 )
		_ptScroll = OnSwGetScrollPos();
	if( bForeground )
	{
		m_clrForeground = clrNew;
		rcInvalidate = rcItemForeground;
	} // if( bForeground )
	else
	{
		m_clrBackground = clrNew;
		rcInvalidate = rcItemBackground;
	} // else from if( bForeground )
	if( bRepaint )
	{
		rcInvalidate.OffsetRect( -_ptScroll );
		InvalidateRect( &rcInvalidate );
		UpdateWindow();
	}
	return true;
}

bool CExtColorPaletteWnd::OnCpwItemClicked(
	INT nItem,
	UINT nMouseFlags,
	bool bMouseButtonPressed,
	CPoint point,
	const CRect & rcItem,
	COLORREF clr
	)
{
	ASSERT_VALID( this );
	point;
	rcItem;
	if( bMouseButtonPressed )
		return true;
	if( nItem < 0 )
		return true;
	if( (nMouseFlags & MK_LBUTTON) != 0 )
	{
		if( m_clrForeground == clr )
			return true;
		if( !OnCpwActiveColorChange(
				true,
				m_clrForeground,
				clr,
				true
				)
			)
			return true;
		ASSERT( m_clrForeground == clr );
	}
	else if( (nMouseFlags & MK_RBUTTON) != 0 )
	{
		if( m_clrBackground == clr )
			return true;
		if( !OnCpwActiveColorChange(
				false,
				m_clrBackground,
				clr,
				true
				)
			)
			return true;
		ASSERT( m_clrBackground == clr );
	}
	return true;
}

void CExtColorPaletteWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	_RequestFocus();
//	CExtScrollWnd::OnLButtonDown(nFlags, point);
	nFlags;
	point;
	_CancelActions();
COLORREF clr;
CRect rcItem;
	m_nTrackItem = CpwHitTest( point, &rcItem, &clr );
	if( m_nTrackItem == __ECPW_CELL_INDEX_INVALID )
		return;
	m_nTrackMouseFlags = MK_LBUTTON | (nFlags & (MK_CONTROL|MK_SHIFT));
	if( !OnCpwItemClicked(
			m_nTrackItem,
			m_nTrackMouseFlags,
			true,
			point,
			rcItem,
			clr
			)
		)
	{ // if tracking enabled
		m_nTrackItem = __ECPW_CELL_INDEX_INVALID;
		return;
	} // if tracking enabled
	CExtMouseCaptureSink::SetCapture( m_hWnd );
}
void CExtColorPaletteWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
//	CExtScrollWnd::OnLButtonUp(nFlags, point);
	nFlags;
	point;
COLORREF clr;
CRect rcItem;
	if(		m_nTrackItem != __ECPW_CELL_INDEX_INVALID
		&&	m_nTrackItem == CpwHitTest( point, &rcItem, &clr )
		&&	(m_nTrackMouseFlags & MK_LBUTTON) != 0
		)
	{ // if item clicked
		INT nTrackItem = m_nTrackItem;
		UINT nTrackMouseFlags = m_nTrackMouseFlags;
		_CancelActions();
		OnCpwItemClicked(
			nTrackItem,
			nTrackMouseFlags,
			false,
			point,
			rcItem,
			clr
			);
		return;
	} // if item clicked
	_CancelActions();
}
void CExtColorPaletteWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
//	CExtScrollWnd::OnLButtonDblClk(nFlags, point);
	nFlags;
	point;
}

void CExtColorPaletteWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	_RequestFocus();
//	CExtScrollWnd::OnRButtonDown(nFlags, point);
	nFlags;
	point;
	_CancelActions();
COLORREF clr;
CRect rcItem;
	m_nTrackItem = CpwHitTest( point, &rcItem, &clr );
	if( m_nTrackItem == __ECPW_CELL_INDEX_INVALID )
		return;
	m_nTrackMouseFlags = MK_RBUTTON | (nFlags & (MK_CONTROL|MK_SHIFT));
	if( !OnCpwItemClicked(
			m_nTrackItem,
			m_nTrackMouseFlags,
			true,
			point,
			rcItem,
			clr
			)
		)
	{ // if tracking enabled
		m_nTrackItem = __ECPW_CELL_INDEX_INVALID;
		return;
	} // if tracking enabled
	CExtMouseCaptureSink::SetCapture( m_hWnd );
}
void CExtColorPaletteWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
//	CExtScrollWnd::OnRButtonUp(nFlags, point);
	nFlags;
	point;
COLORREF clr;
CRect rcItem;
	if(		m_nTrackItem != __ECPW_CELL_INDEX_INVALID
		&&	m_nTrackItem == CpwHitTest( point, &rcItem, &clr )
		&&	(m_nTrackMouseFlags & MK_RBUTTON) != 0
		)
	{ // if item clicked
		INT nTrackItem = m_nTrackItem;
		UINT nTrackMouseFlags = m_nTrackMouseFlags;
		_CancelActions();
		OnCpwItemClicked(
			nTrackItem,
			nTrackMouseFlags,
			false,
			point,
			rcItem,
			clr
			);
		return;
	} // if item clicked
	_CancelActions();
}
void CExtColorPaletteWnd::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
//	CExtScrollWnd::OnRButtonDblClk(nFlags, point);
	nFlags;
	point;
}

void CExtColorPaletteWnd::OnMButtonDown(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	_RequestFocus();
//	CExtScrollWnd::OnMButtonDown(nFlags, point);
	nFlags;
	point;
	_CancelActions();
COLORREF clr;
CRect rcItem;
	m_nTrackItem = CpwHitTest( point, &rcItem, &clr );
	if( m_nTrackItem == __ECPW_CELL_INDEX_INVALID )
		return;
	m_nTrackMouseFlags = MK_MBUTTON | (nFlags & (MK_CONTROL|MK_SHIFT));
	if( !OnCpwItemClicked(
			m_nTrackItem,
			m_nTrackMouseFlags,
			true,
			point,
			rcItem,
			clr
			)
		)
	{ // if tracking enabled
		m_nTrackItem = __ECPW_CELL_INDEX_INVALID;
		return;
	} // if tracking enabled
	CExtMouseCaptureSink::SetCapture( m_hWnd );
}
void CExtColorPaletteWnd::OnMButtonUp(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
//	CExtScrollWnd::OnMButtonUp(nFlags, point);
	nFlags;
	point;
COLORREF clr;
CRect rcItem;
	if(		m_nTrackItem != __ECPW_CELL_INDEX_INVALID
		&&	m_nTrackItem == CpwHitTest( point, &rcItem, &clr )
		&&	(m_nTrackMouseFlags & MK_MBUTTON) != 0
		)
	{ // if item clicked
		INT nTrackItem = m_nTrackItem;
		UINT nTrackMouseFlags = m_nTrackMouseFlags;
		_CancelActions();
		OnCpwItemClicked(
			nTrackItem,
			nTrackMouseFlags,
			false,
			point,
			rcItem,
			clr
			);
		return;
	} // if item clicked
	_CancelActions();
}
void CExtColorPaletteWnd::OnMButtonDblClk(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
//	CExtScrollWnd::OnMButtonDblClk(nFlags, point);
	nFlags;
	point;
}

void CExtColorPaletteWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
//	CExtScrollWnd::OnMouseMove(nFlags, point);

	if( (nFlags & (MK_LBUTTON|MK_RBUTTON|MK_MBUTTON)) != 0 )
		return;

	if( (CpwGetStyle() & __ECPWS_TOOLTIPS) != 0 )
	{
		if( m_wndToolTip.GetSafeHwnd() == NULL )
		{
			if( ! m_wndToolTip.Create( this ) )
			{
				ASSERT( FALSE );
				return;
			}
			m_wndToolTip.Activate( TRUE );
		} // if( m_wndToolTip.GetSafeHwnd() == NULL )
		
		if(		m_wndToolTip.GetSafeHwnd() != NULL
			&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
			)
		{ // if tooltip window is valid
			bool bCancelTips = true;
			COLORREF clr = RGB( 0, 0, 0 );
			CRect rcItem;
			INT nItem = CpwHitTest( point, &rcItem, &clr );
			if( nItem != __ECPW_CELL_INDEX_INVALID )
			{
				CExtSafeString strTooltipText =
					OnCpwGetTooltipText(
						nItem,
						point,
						rcItem,
						clr
						);
				if( !strTooltipText.IsEmpty() )
				{
					if(	m_wndToolTip.AddTool(
							this,
							strTooltipText,
							&rcItem,
							1
							)
						)
					{
						bCancelTips = false;
					}
				} // if( !strTooltipText.IsEmpty() )
			} // if( nItem != __ECPW_CELL_INDEX_INVALID )
			if( bCancelTips )
			{
				m_wndToolTip.DelTool( this, 1 );
				CWnd::CancelToolTips();
			} // if( bCancelTips )
		} // if tooltip window is valid
	} // if( (CpwGetStyle() & __ECPWS_TOOLTIPS) != 0 )
}

void CExtColorPaletteWnd::OnCancelMode() 
{
	CExtScrollWnd::OnCancelMode();
	_CancelActions();	
}

void CExtColorPaletteWnd::OnCaptureChanged(CWnd *pWnd) 
{
	CExtScrollWnd::OnCaptureChanged(pWnd);
	if( pWnd != this )
		_CancelActions();
}

void CExtColorPaletteWnd::OnSize(UINT nType, int cx, int cy) 
{
	CExtScrollWnd::OnSize(nType, cx, cy);
	_CancelActions();
DWORD dwWndStyle = GetStyle();
	if( (dwWndStyle & WS_VISIBLE) == 0 )
		return;
	if(		m_wndToolTip.GetSafeHwnd() != NULL
		&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
		)
		m_wndToolTip.DelTool( this, 1 );
	CWnd::CancelToolTips();
}

void CExtColorPaletteWnd::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CExtScrollWnd::OnWindowPosChanged(lpwndpos);
	_CancelActions();
DWORD dwWndStyle = GetStyle();
	if( (dwWndStyle & WS_VISIBLE) == 0 )
		return;
	if(		m_wndToolTip.GetSafeHwnd() != NULL
		&&	::IsWindow( m_wndToolTip.GetSafeHwnd() )
		)
	{
		m_wndToolTip.DelTool( this, 1 );
		m_wndToolTip.SendMessage( WM_CANCELMODE );
	}
	CWnd::CancelToolTips();
}

LRESULT CExtColorPaletteWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
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
	
	return CExtScrollWnd::WindowProc(message, wParam, lParam);
}

BOOL CExtColorPaletteWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	if(		(! RegisterColorPaletteWndClass() )
		||	(! CExtScrollWnd::PreCreateWindow(cs) )
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
	cs.lpszClass = __EXT_COLOR_PALETTE_WND_CLASS_NAME;
	return TRUE;
}

#endif // (!defined __EXT_MFC_NO_COLORPALETTE)
