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

#if (!defined __EXT_SCROLLWND_H)
#define __EXT_SCROLLWND_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __AFXTEMPL_H__)
	#include <AfxTempl.h>
#endif

#if (!defined __EXT_MOUSECAPTURESINK_H)
	#include <../Src/ExtMouseCaptureSink.h>
#endif

#if (!defined __EXT_MFC_NO_SCROLLWND)

#define __EXT_SCROLL_NUMERIC_MAX INT_MAX

/////////////////////////////////////////////////////////////////////////////
// CExtScrollBar window


class __PROF_UIS_API CExtScrollBar : public CScrollBar
{
// Construction
public:
	DECLARE_DYNCREATE( CExtScrollBar );
	CExtScrollBar();

// Attributes
public:
	enum eScrollerOrientation_t
	{
		__ESO_TOP = 0,
		__ESO_BOTTOM = 1,
		__ESO_LEFT = 2,
		__ESO_RIGHT = 3,
		__ESO_AUTO = 4,
		__ESO_NONE = 5,
		__ESO_MIN_VALUE = 0,
		__ESO_MAX_VALUE = 5,
	};
	eScrollerOrientation_t m_eSO;
	bool m_bAutoDeleteOnPostNcDestroy:1,m_bReflectParentSizing:1,
		m_bDisableAutoReserveSpace:1,
		m_bAutoReserveSpaceBefore:1, m_bAutoReserveSpaceAfter:1,
		m_bSmoothPainting:1,
		m_bHideHwndBefore:1, m_bHideHwndAfter:1;
	HWND m_hWndBefore, m_hWndAfter;

	virtual void SyncReservedSpace( CExtScrollBar * pOtherBar );
	virtual eScrollerOrientation_t GetScrollerOrientation() const;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtScrollBar)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtScrollBar();
	virtual void PostNcDestroy();
protected:
	INT m_nHorzBarHeight, m_nVertBarWidth;
	void _ScanSysMertics();

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtScrollBar)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg void OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection);
	afx_msg LRESULT OnSizeParent( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
}; // class CExtScrollBar

/////////////////////////////////////////////////////////////////////////////
// CExtScrollWnd window

// based on MFC source code in viewscrl.cpp
class __PROF_UIS_API CExtScrollWnd
	: public CWnd
	, public CExtMouseCaptureSink
{
// Construction
public:
	DECLARE_DYNCREATE( CExtScrollWnd );
	CExtScrollWnd();

// Attributes
public:
	bool m_bScrollPhysical:1, m_bEatErasing:1,
		m_bBufferedPainting:1,
		m_bScrollInvalidate:1, m_bScrollErase:1,
		m_bScrollUpdateWindow:1, m_bRedrawUpdateWindow:1,
		m_bRedrawInvalidate:1, m_bRedrawErase:1,
		m_bUse32BitScrollInfo:1;

protected:
	int m_nUpdateScrollBars;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtScrollWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtScrollWnd();

	virtual CSize OnSwGetTotalSize() const;
	virtual CSize OnSwGetPageSize( int nDirection ) const;
	virtual CSize OnSwGetLineSize( int nDirection ) const;
	virtual CRect OnSwRecalcLayout(
		bool bDoLayout,
		LPCRECT pRectClientSrc = NULL
		);
	virtual CRect OnSwGetClientRect() const;
	
	virtual bool OnSwCanAutoHideScrollBar( bool bHorz ) const;
	virtual bool OnSwHasScrollBar( bool bHorz ) const;
	virtual void OnSwEnableScrollBarCtrl( int nBar, bool bEnable );
	virtual bool OnSwScrollInfoAdjust(
		int nBar,
		SCROLLINFO & si,
		bool bRedraw = true
		);
	virtual void OnSwSetScrollRange(
		int nBar,
		LONG nMinPos,
		LONG nMaxPos,
		bool bRedraw = true
		);
	virtual bool OnSwQueryThumbTrackEnabled( bool bHorz ) const;

	virtual LONG ScrollLimit32Get( int nBar ) const;
	virtual void ScrollInfo32Get(
		int nBar,
		LONG * p_nMinPos,
		LONG * p_nMaxPos,
		LONG * p_nPageSize,
		LONG * p_nCurrPos,
		LONG * p_nTrackPos
		) const;
	virtual LONG ScrollPos32Get(
		int nBar,
		bool bTrackPos = false
		) const;
	virtual void ScrollPos32Set(
		int nBar,
		LONG nPos,
		bool bRedraw = true
		);

	virtual CPoint OnSwGetScrollPos() const;
	virtual CPoint OnSwGetScrollPaintPos() const;
	virtual void OnSwSetScrollPos( POINT pt );

	virtual void OnSwGetScrollBarState(
		CSize sizeClient,
		CSize & sizeNeedSB,
		CSize & sizeRange,
		CPoint & ptMove,
		bool bInsideClient
		) const;
	virtual CSize OnSwGetScrollBarSizes() const;
	virtual bool OnSwCalcClientSizes(
		CSize & size,
		CSize & sizeSb
		);
	
	virtual UINT OnSwCalcMouseScrollLines(
		UINT fFlags,
		short zDelta,
		CPoint point
		);
	virtual bool OnSwDoMouseWheel(
		UINT fFlags,
		short zDelta,
		CPoint point
		);

	virtual bool OnSwDoScroll(
		UINT nScrollCode,
		UINT nPos,
		bool bDoScroll = true
		);
	virtual bool OnSwDoScrollBy(
		CSize sizeScroll,
		bool bDoScroll = true
		);
	virtual void OnSwUpdateScrollBars();

	virtual void OnSwDoScrollWindow(
		int xAmount,
		int yAmount,
		LPCRECT lpRect = NULL,
		LPCRECT lpClipRect = NULL
		);

	virtual void OnSwEnsurePointAvail( CPoint point );

	virtual void OnSwInvalidate( bool bErase );
	virtual void OnSwDoRedraw();
	virtual void OnSwPaint( CDC & dc );

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtScrollWnd)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT fFlags, short zDelta, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg void OnSysColorChange();
	afx_msg void OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection);
	afx_msg LRESULT OnDisplayChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnThemeChanged( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
}; // class CExtScrollWnd

#if (!defined __EXT_MFC_NO_SCROLLITEMWND)

/////////////////////////////////////////////////////////////////////////////
// CExtScrollItemWnd window & CExtScrollItemCacheInfo helper

// item scrolling type
#define __ESIW_ST_NONE					0x00000000L
#define __ESIW_ST_PIXEL					0x00000001L
#define __ESIW_ST_ITEM					0x00000002L
#define __ESIW_ST_VIRTUAL				0x00000003L
#define __ESIW_ST_MASK					0x00000003L
#define __ESIW_ST_BIT_COUNT				2L

// item scroll window styles
#define __ESIS_STH_NONE					__ESIW_ST_NONE
#define __ESIS_STH_PIXEL				__ESIW_ST_PIXEL
#define __ESIS_STH_ITEM					__ESIW_ST_ITEM
#define __ESIS_STH_VIRTUAL				__ESIW_ST_VIRTUAL
#define __ESIS_STV_NONE					(__ESIW_ST_NONE << __ESIW_ST_BIT_COUNT)
#define __ESIS_STV_PIXEL				(__ESIW_ST_PIXEL << __ESIW_ST_BIT_COUNT)
#define __ESIS_STV_ITEM					(__ESIW_ST_ITEM << __ESIW_ST_BIT_COUNT)
#define __ESIS_STV_VIRTUAL				(__ESIW_ST_VIRTUAL << __ESIW_ST_BIT_COUNT)
#define __ESIS_STH_MASK					__ESIW_ST_MASK
#define __ESIS_STV_MASK					(__ESIW_ST_MASK << __ESIW_ST_BIT_COUNT)
#define __ESIS_ST_MASK					(__ESIS_STH_MASK|__ESIS_STV_MASK)
#define __ESIS_DISABLE_THUMBTRACK_H		0x00000010L
#define __ESIS_DISABLE_THUMBTRACK_V		0x00000020L
#define __ESIS_DISABLE_THUMBTRACK		(__ESIS_DISABLE_THUMBTRACK_H|__ESIS_DISABLE_THUMBTRACK_V)
#define __ESIS_DISABLE_AUTOHIDE_SB_H	0x00000040L
#define __ESIS_DISABLE_AUTOHIDE_SB_V	0x00000080L
#define __ESIS_DISABLE_AUTOHIDE_SB		(__ESIS_DISABLE_AUTOHIDE_SB_H|__ESIS_DISABLE_AUTOHIDE_SB_V)
#define __ESIS_PREFER_HORZ_WALK			0x00000100L

#if (!defined __EXT_MFC_NO_GRIDBASEWND)

// helper flags to optimize event processing when
// all rows/columns have equal sizes
#define __EGBS_FIXED_SIZE_COLUMNS		0x00000200L
#define __EGBS_FIXED_SIZE_ROWS			0x00000400L

// basic selection/focus type
#define __EGBS_SFB_NONE					0x00000000L
#define __EGBS_SFB_CELLS				0x00001000L
#define __EGBS_SFB_FULL_ROWS			0x00002000L
#define __EGBS_SFB_FULL_COLUMNS			0x00003000L
#define __EGBS_SFB_MASK					0x00003000L
// allow multiple row/column/cell selection
#define __EGBS_SFM_ROWS					0x00000800L
#define __EGBS_SFM_COLUMNS				0x00008000L
#define __EGBS_SFM_MASK					(__EGBS_SFM_ROWS|__EGBS_SFM_COLUMNS)
// enabled multiple selection types
#define __EGBS_SFM_CELLS_H				(__EGBS_SFB_CELLS|__EGBS_SFM_COLUMNS)
#define __EGBS_SFM_CELLS_V				(__EGBS_SFB_CELLS|__EGBS_SFM_ROWS)
#define __EGBS_SFM_CELLS_HV				(__EGBS_SFB_CELLS|__EGBS_SFM_COLUMNS|__EGBS_SFM_ROWS)
#define __EGBS_SFM_FULL_ROWS			(__EGBS_SFB_FULL_ROWS|__EGBS_SFM_ROWS)
#define __EGBS_SFM_FULL_COLUMNS			(__EGBS_SFB_FULL_COLUMNS|__EGBS_SFM_COLUMNS)
// any selection mask
#define __EGBS_SF_MASK					(__EGBS_SFB_MASK|__EGBS_SFM_MASK)
// allow selection via outer row/column clicks
#define __EGBS_SF_SELECT_OUTER_ROWS		0x00010000L
#define __EGBS_SF_SELECT_OUTER_COLUMNS	0x00020000L

// enable row/column resizing
#define __EGBS_RESIZING_CELLS_OUTER_H	0x00040000L
#define __EGBS_RESIZING_CELLS_OUTER_V	0x00100000L
#define __EGBS_RESIZING_CELLS_OUTER		(__EGBS_RESIZING_CELLS_OUTER_H|__EGBS_RESIZING_CELLS_OUTER_V)
#define __EGBS_RESIZING_CELLS_INNER_H	0x00200000L
#define __EGBS_RESIZING_CELLS_INNER_V	0x00400000L
#define __EGBS_RESIZING_CELLS_INNER		(__EGBS_RESIZING_CELLS_INNER_H|__EGBS_RESIZING_CELLS_INNER_V)
#define __EGBS_RESIZING_CELLS_MASK		(__EGBS_RESIZING_CELLS_OUTER|__EGBS_RESIZING_CELLS_INNER)

// auto move focused cell to bottom/right corner of selection
#define __EGBS_AUTO_FOCUS_BOTTOM_RIGHT	0x00800000L

// allow multiple cell ranges to be selected at a time
#define __EGBS_MULTI_AREA_SELECTION		0x01000000L

// no hide selection when not focused
#define __EGBS_NO_HIDE_SELECTION		0x02000000L

// draw grid lines
#define __EGBS_GRIDLINES_H				0x04000000L
#define __EGBS_GRIDLINES_V				0x08000000L
#define __EGBS_GRIDLINES				(__EGBS_GRIDLINES_H|__EGBS_GRIDLINES_V)

// mouse click + move selects cell range
#define __EGBS_LBEXT_SELECTION			0x10000000L

// subtract overlapped cell ranges in selection areas
#define __EGBS_SUBTRACT_SEL_AREAS		0x20000000L

// resize rows/columns on-the-fly (no half-tone tracker lines)
#define __EGBS_DYNAMIC_RESIZING_H		0x40000000L
#define __EGBS_DYNAMIC_RESIZING_V		0x80000000L
#define __EGBS_DYNAMIC_RESIZING			(__EGBS_DYNAMIC_RESIZING_H|__EGBS_DYNAMIC_RESIZING_V)

#endif // (!defined __EXT_MFC_NO_GRIDBASEWND)

// extended/behavior styles provided with
// CExtScrollItemWnd::SiwGetStyleEx()
// ( these styles are not intersected with
//   CExtScrollItemWnd::SiwGetStyle() style values )

#if (!defined __EGWS_EX_PM_COLORS)
	#define __EGWS_EX_PM_COLORS					0x80000000L
#endif


class __PROF_UIS_API CExtScrollItemCacheInfo
{
public:
	LONG m_nPosH;
	LONG m_nMaxH;
	LONG m_nPageSizeH;
	LONG m_nPosV;
	LONG m_nMaxV;
	LONG m_nPageSizeV;
	bool m_bBeginH:1;
	bool m_bEndH:1;
	bool m_bBeginV:1;
	bool m_bEndV:1;
	CExtScrollItemCacheInfo(
		LONG nPosH = 0L,
		LONG nMaxH = 0L,
		LONG nPageSizeH = 0L,
		LONG nPosV = 0L,
		LONG nMaxV = 0L,
		LONG nPageSizeV = 0L,
		bool bBeginH = false,
		bool bEndH = false,
		bool bBeginV = false,
		bool bEndV = false
		);
	CExtScrollItemCacheInfo(
		const CExtScrollItemCacheInfo & other
		);
	CExtScrollItemCacheInfo & operator = (
		const CExtScrollItemCacheInfo & other
		);
	bool operator == (
		const CExtScrollItemCacheInfo & other
		) const;
	bool operator != (
		const CExtScrollItemCacheInfo & other
		) const;
	void Empty(
		bool bEmptyH = true,
		bool bEmptyV = true
		);
	void EmptyH();
	void EmptyV();
	bool IsEmpty(
		bool bCheckEmptyH = true,
		bool bCheckEmptyV = true
		) const;
	bool IsEmptyH() const;
	bool IsEmptyV() const;
	bool IsValid() const;
	static bool IsEqual(
		const CExtScrollItemCacheInfo & left,
		const CExtScrollItemCacheInfo & right,
		bool bComparePosH = true,
		bool bComparePosV = true,
		bool bCompareMaxH = true,
		bool bCompareMaxV = true,
		bool bComparePageSizeH = true,
		bool bComparePageSizeV = true,
		bool bCompareBofH = false,
		bool bCompareBofV = false,
		bool bCompareEofH = false,
		bool bCompareEofV = false
		);
	bool IsEqual(
		const CExtScrollItemCacheInfo & other,
		bool bComparePosH = true,
		bool bComparePosV = true,
		bool bCompareMaxH = true,
		bool bCompareMaxV = true,
		bool bComparePageSizeH = true,
		bool bComparePageSizeV = true,
		bool bCompareBofH = false,
		bool bCompareBofV = false,
		bool bCompareEofH = false,
		bool bCompareEofV = false
		) const;
	LONG GetExtentH() const;
	LONG GetExtentV() const;
	LONG GetPosChangingH(
		const CExtScrollItemCacheInfo & _sciOld
		) const;
	LONG GetPosChangingV(
		const CExtScrollItemCacheInfo & _sciOld
		) const;
	LONG GetMaxChangingH(
		const CExtScrollItemCacheInfo & _sciOld
		) const;
	LONG GetMaxChangingV(
		const CExtScrollItemCacheInfo & _sciOld
		) const;
	LONG GetExtentChangingH(
		const CExtScrollItemCacheInfo & _sciOld
		) const;
	LONG GetExtentChangingV(
		const CExtScrollItemCacheInfo & _sciOld
		) const;
	LONG GetPageSizeChangingH(
		const CExtScrollItemCacheInfo & _sciOld
		) const;
	LONG GetPageSizeChangingV(
		const CExtScrollItemCacheInfo & _sciOld
		) const;
	bool IsShiftToBofH(
		const CExtScrollItemCacheInfo & _sciOld
		) const;
	bool IsShiftToBofV(
		const CExtScrollItemCacheInfo & _sciOld
		) const;
	bool IsShiftToEofH(
		const CExtScrollItemCacheInfo & _sciOld
		) const;
	bool IsShiftToEofV(
		const CExtScrollItemCacheInfo & _sciOld
		) const;
	void FlipHV();
	void Assign(
		const CExtScrollItemCacheInfo & other,
		bool bAssignH = true,
		bool bAssignV = true
		);
	void AssignH(
		const CExtScrollItemCacheInfo & other
		);
	void AssignV(
		const CExtScrollItemCacheInfo & other
		);
}; // class CExtScrollItemCacheInfo

class __PROF_UIS_API CExtScrollItemWnd : public CExtScrollWnd
{
// Construction
public:
	DECLARE_DYNCREATE( CExtScrollItemWnd );
	CExtScrollItemWnd();

// Attributes
public:
	bool m_bExtractResourceStyles:1;

protected:
	static bool g_bScrollItemWndClassRegistered;
	bool m_bDirectCreateCall:1;
	DWORD m_dwScrollItemWndStyle, m_dwScrollItemWndStyleEx;

// Operations
public:
	virtual DWORD SiwGetStyle() const;
	virtual DWORD SiwModifyStyle(
		DWORD dwStyleAdd,
		DWORD dwStyleRemove = 0L,
		bool bUpdateWnd = true
		);
	virtual DWORD SiwGetStyleEx() const;
	virtual DWORD SiwModifyStyleEx(
		DWORD dwStyleExAdd,
		DWORD dwStyleExRemove = 0L,
		bool bUpdateWnd = true
		);
	DWORD SiwScrollTypeHGet() const;
	DWORD SiwScrollTypeVGet() const;
	DWORD SiwScrollTypeHSet(
		DWORD dwScrollTypeNew,
		bool bUpdateWnd = true
		);
	DWORD SiwScrollTypeVSet(
		DWORD dwScrollTypeNew,
		bool bUpdateWnd = true
		);
	bool SiwPreferHorizontalWalkGet() const;
	bool SiwPreferHorizontalWalkSet(
		bool bPreferHorizontalWalk
		);
	bool SiwThumbTrackEnabledGet( bool bHorz ) const;
	bool SiwThumbTrackEnabledSet(
		bool bHorz,
		bool bEnabled = true
		);
	bool SiwAutohideScrollBarGet( bool bHorz ) const;
	bool SiwAutohideScrollBarSet(
		bool bHorz,
		bool bAutoHide = true
		);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtScrollItemWnd)
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtScrollItemWnd();

	BOOL Create(
		CWnd * pParentWnd,
		const RECT & rcWnd = CRect( 0, 0, 0, 0 ),
		UINT nDlgCtrlID = UINT( IDC_STATIC ),
		DWORD dwScrollItemWndStyle = 0L,
		DWORD dwWindowStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
		CCreateContext * pContext = NULL
		);
	virtual bool _CreateHelper();
	static bool RegisterScrollItemWndClass();

	virtual void OnSwGetScrollBarState(
		CSize sizeClient,
		CSize & sizeNeedSB,
		CSize & sizeRange,
		CPoint & ptMove,
		bool bInsideClient
		) const;
	virtual CSize OnSwGetLineSize( int nDirection ) const;
	virtual void OnSwUpdateScrollBars();
	virtual CSize OnSwGetPageSize( int nDirection ) const;
	virtual CSize OnSwGetTotalSize() const;
	virtual bool OnSwDoScroll(
		UINT nScrollCode,
		UINT nPos,
		bool bDoScroll = true
		);
	virtual bool OnSwDoScrollBy(
		CSize sizeScroll,
		bool bDoScroll = true
		);
	virtual bool SiwFireCacheChanging(
		LONG nScrollPosNewX = -1L, // if <0L - use current
		LONG nScrollPosNewY = -1L, // if <0L - use current
		bool bUpdate = true
		);
	virtual CPoint OnSwGetScrollPaintPos() const;
	virtual void OnSwPaint( CDC & dc );
	virtual bool OnSwQueryThumbTrackEnabled( bool bHorz ) const;
	virtual bool OnSwCanAutoHideScrollBar( bool bHorz ) const;

	virtual CSize OnSiwCalcItemSize() const;
	virtual CSize OnSiwCalcPageMetrics( int nDirection ) const;
	virtual bool OnSiwQueryVirtualBOF( bool bHorz ) const;
	virtual bool OnSiwQueryVirtualEOF( bool bHorz ) const;
	virtual CRect OnSiwGetVisibleRange() const;
	virtual INT OnSiwQueryItemExtentH(
		LONG nColNo,
		INT * p_nExtraSpaceBefore = NULL,
		INT * p_nExtraSpaceAfter = NULL
		) const;
	virtual INT OnSiwQueryItemExtentV(
		LONG nRowNo,
		INT * p_nExtraSpaceBefore = NULL,
		INT * p_nExtraSpaceAfter = NULL
		) const;
	virtual bool OnSiwVirtualEndTestH( LONG nColNo, LPARAM lParam ) const;
	virtual bool OnSiwVirtualEndTestV( LONG nRowNo, LPARAM lParam ) const;
	virtual bool OnSiwWalkCell(
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
		) const;
	virtual bool OnSiwWalkItemsH(
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
		) const;
	virtual bool OnSiwWalkItemsV(
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
		) const;
	virtual void OnSiwPaintBackground(
		CDC & dc,
		bool bFocusedControl
		) const;
	virtual void OnSiwPaintForeground(
		CDC & dc,
		bool bFocusedControl
		) const;
	virtual bool OnSiwQueryFocusedControlState() const;
	virtual COLORREF OnSiwGetSysColor( int nIndex ) const;
	// nLighterOrDarker is related to COLOR_3DFACE brush:
	// -3 - darkest, -2 - darker, -1 - dark,
	// 1 - light, 2 - lighter, 3 - lightest
	virtual CBrush & OnSiwGetLighterOrDarkerBrush( int nLighterOrDarker ) const;
	virtual CFont & OnSiwGetDefaultFont() const;

	virtual bool OnSiwCacheChanged(
		const CExtScrollItemCacheInfo & _sciNew,
		const CExtScrollItemCacheInfo & _sciOld
		);

protected:
	CExtScrollItemCacheInfo m_sciLast;
	virtual bool _NotifyCacheChanged(
		LONG nPosH,
		LONG nPosOldH,
		LONG nMaxH,
		LONG nPageSizeH,
		LONG nPosV,
		LONG nPosOldV,
		LONG nMaxV,
		LONG nPageSizeV,
		bool bBeginH,
		bool bEndH,
		bool bBeginV,
		bool bEndV
		);
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtScrollItemWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; // class CExtScrollItemWnd

#define __EXT_SCROLLITEMWND_CLASS_NAME _T("ProfUIS-ScrollItemWindow")

#endif // (!defined __EXT_MFC_NO_SCROLLITEMWND)

#endif // (!defined __EXT_MFC_NO_SCROLLWND)

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined __EXT_SCROLLWND_H
