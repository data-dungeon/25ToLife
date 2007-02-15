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

#if (!defined __EXT_GRIDWND_H)
#define __EXT_GRIDWND_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_SCROLLWND_H)
	#include <ExtScrollWnd.h>
#endif 

#if !defined( _OLEAUTO_H_ )
	#include <OleAuto.h>
#endif

#if (!defined __EXT_EXTCONTENTEXPANDWND_H)
	#include <ExtContentExpandWnd.h>
#endif

#if (!defined __EXT_CMD_ICON_H)
	#include <ExtCmdIcon.h>
#endif

#if (!defined __EXT_MFC_NO_GRIDBASEWND)

/////////////////////////////////////////////////////////////////////////////
// CExtGridHitTestInfo

// hit-testing area detection
// (CExtGridHitTestInfo::m_dwAreaFlags flags)
#define __EGBWA_NOWHERE							0x00000000L
#define __EGBWA_OUTER_TOP						0x00000001L
#define __EGBWA_OUTER_BOTTOM					0x00000002L
#define __EGBWA_OUTER_V							(__EGBWA_OUTER_TOP|__EGBWA_OUTER_BOTTOM)
#define __EGBWA_OUTER_LEFT						0x00000004L
#define __EGBWA_OUTER_RIGHT						0x00000008L
#define __EGBWA_OUTER_H							(__EGBWA_OUTER_LEFT|__EGBWA_OUTER_RIGHT)
#define __EGBWA_OUTER_SIDE_MASK					(__EGBWA_OUTER_H|__EGBWA_OUTER_V)
#define __EGBWA_NEAR_CELL_BORDER_TOP			0x00000010L
#define __EGBWA_NEAR_CELL_BORDER_BOTTOM			0x00000020L
#define __EGBWA_NEAR_CELL_BORDER_V				(__EGBWA_NEAR_CELL_BORDER_TOP|__EGBWA_NEAR_CELL_BORDER_BOTTOM)
#define __EGBWA_NEAR_CELL_BORDER_LEFT			0x00000040L
#define __EGBWA_NEAR_CELL_BORDER_RIGHT			0x00000080L
#define __EGBWA_NEAR_CELL_BORDER_H				(__EGBWA_NEAR_CELL_BORDER_LEFT|__EGBWA_NEAR_CELL_BORDER_RIGHT)
#define __EGBWA_NEAR_CELL_BORDER_SIDE_MASK		(__EGBWA_NEAR_CELL_BORDER_H|__EGBWA_NEAR_CELL_BORDER_V)
#define __EGBWA_OUTER_CELLS						0x00000100L
#define __EGBWA_INNER_CELLS						0x00000200L
// the CExtGridCell::OnHitTestInfoAdjust() method may fill
// m_nButtonType, m_rcPart and add following flags:
#define __EGBWA_CELL_CHECKBOX					0x00000400L
#define __EGBWA_CELL_IMAGE						0x00000800L
#define __EGBWA_CELL_BUTTON						0x00001000L
#define __EGBWA_CELL_TEXT						0x00002000L
#define __EGBWA_CELL_TEXT_AREA					0x00004000L
#define __EGBWA_CELL_SORT_ARROW					0x00008000L
#define __EGBWA_CELL_FOCUS_ARROW				0x00010000L
#define __EGBWA_CELL_MASK \
	(__EGBWA_CELL_CHECKBOX \
	|__EGBWA_CELL_IMAGE \
	|__EGBWA_CELL_BUTTON \
	|__EGBWA_CELL_TEXT \
	|__EGBWA_CELL_TEXT_AREA \
	|__EGBWA_CELL_SORT_ARROW \
	|__EGBWA_CELL_FOCUS_ARROW \
	)

// drop-after state
#define __EGBWA_OUTER_DROP_AFTER				0x00020000L

class __PROF_UIS_API CExtGridHitTestInfo
{
public:
	CPoint m_ptClient;
	LONG m_nRowNo, m_nColNo, m_nVisibleRowNo, m_nVisibleColNo;
	DWORD m_dwAreaFlags;
	CRect m_rcItem, m_rcExtra, m_rcPart;
	INT m_nButtonType;
	CExtGridHitTestInfo(
		CPoint ptClient = CPoint( -1, -1 )
		);
	CExtGridHitTestInfo(
		const CExtGridHitTestInfo & htInfo
		);
	CExtGridHitTestInfo & operator = (
		const CExtGridHitTestInfo & htInfo
		);
	void Empty();
	bool IsHoverEmpty() const;
	bool IsHoverInner() const;
	bool IsHoverOuter(
		bool * p_bOuterCellAtLeftRight = NULL
		) const;
	static bool IsHoverEqual(
		const CExtGridHitTestInfo & htInfoLeft,
		const CExtGridHitTestInfo & htInfoRight,
		bool bCompareRects = true,
		bool bValidtateRects = true,
		bool bCompareOuterDropAfterState = true
		);
	bool IsHoverEqual(
		const CExtGridHitTestInfo & htInfo,
		bool bCompareRects = true,
		bool bValidtateRects = true,
		bool bCompareOuterDropAfterState = true
		) const;
	bool IsValidRect(
		bool bEmptyIsInvalid = true
		) const;
	INT GetInnerOuterTypeOfColumn() const;
	INT GetInnerOuterTypeOfRow() const;
	static INT GetInnerOuterTypeOfColumn( DWORD dwAreaFlags );
	static INT GetInnerOuterTypeOfRow( DWORD dwAreaFlags );
	static DWORD CellTypesToAreaFlags(
		INT nColType,
		INT nRowType
		);
protected:
	void _AssignFromOther(
		const CExtGridHitTestInfo & htInfo
		);
}; // class CExtGridHitTestInfo

/////////////////////////////////////////////////////////////////////////////
// CExtGridBaseWnd

// extended/behavior styles provided with
// CExtGridBaseWnd::SiwGetStyleEx()
// ( these styles are not intersected with
//   CExtScrollItemWnd::SiwGetStyle() style values )

// allow inner/outer cell hover processing on WM_MOUSEMOVE
#define __EGBS_EX_HVI_EVENT_CELLS				0x00000001L
#define __EGBS_EX_HVO_EVENT_CELLS				0x00000002L
// highlight inner/outer cells in the hovered row
// ( __EGBS_EX_HVI_HIGHLIGHT_ROWS requires __EGBS_EX_HVI_EVENT_CELLS|__EGBS_EX_HVI_HIGHLIGHT_CELL,
//   __EGBS_EX_HVO_HIGHLIGHT_ROWS requires __EGBS_EX_HVO_EVENT_CELLS|__EGBS_EX_HVO_HIGHLIGHT_CELL )
#define __EGBS_EX_HVI_HIGHLIGHT_ROWS			0x00000004L
#define __EGBS_EX_HVO_HIGHLIGHT_ROWS			0x00000008L
// highlight inner/outer cells in the hovered column
// ( __EGBS_EX_HVI_HIGHLIGHT_COLUMNS requires __EGBS_EX_HVI_EVENT_CELLS|__EGBS_EX_HVI_HIGHLIGHT_CELL,
//   __EGBS_EX_HVO_HIGHLIGHT_COLUMNS requires __EGBS_EX_HVO_EVENT_CELLS|__EGBS_EX_HVO_HIGHLIGHT_CELL )
#define __EGBS_EX_HVI_HIGHLIGHT_COLUMNS			0x00000010L
#define __EGBS_EX_HVO_HIGHLIGHT_COLUMNS			0x00000020L
// highlight cell under hover
// ( __EGBS_EX_HVI_HIGHLIGHT_CELL requires __EGBS_EX_HVI_EVENT_CELLS,
//   __EGBS_EX_HVO_HIGHLIGHT_CELL requires __EGBS_EX_HVO_EVENT_CELLS )
#define __EGBS_EX_HVI_HIGHLIGHT_CELL			0x00000040L
#define __EGBS_EX_HVO_HIGHLIGHT_CELL			0x00000080L

// highlight inner/outer cells matching with the selection
// in the same row/column
#define __EGBS_EX_SI_HIGHLIGHT_COLUMNS			0x00000100L
#define __EGBS_EX_SO_HIGHLIGHT_COLUMNS			0x00000200L
#define __EGBS_EX_SI_HIGHLIGHT_ROWS				0x00000400L
#define __EGBS_EX_SO_HIGHLIGHT_ROWS				0x00000800L

// highlight inner/outer cells matching with the focus
// in the same row/column
#define __EGBS_EX_FI_HIGHLIGHT_COLUMNS			0x00001000L
#define __EGBS_EX_FO_HIGHLIGHT_COLUMNS			0x00002000L
#define __EGBS_EX_FI_HIGHLIGHT_ROWS				0x00004000L
#define __EGBS_EX_FO_HIGHLIGHT_ROWS				0x00008000L

// tooltip/expanding for inner/outer cells
// (if expanding is enabled - toolips are not used)
#define __EGBS_EX_CELL_TOOLTIPS_OUTER_L			0x00010000L
#define __EGBS_EX_CELL_TOOLTIPS_OUTER_R			0x00020000L
#define __EGBS_EX_CELL_TOOLTIPS_OUTER_T			0x00040000L
#define __EGBS_EX_CELL_TOOLTIPS_OUTER_B			0x00080000L
#define __EGBS_EX_CELL_TOOLTIPS_OUTER_HORZ		(__EGBS_EX_CELL_TOOLTIPS_OUTER_T|__EGBS_EX_CELL_TOOLTIPS_OUTER_B)
#define __EGBS_EX_CELL_TOOLTIPS_OUTER_VERT		(__EGBS_EX_CELL_TOOLTIPS_OUTER_L|__EGBS_EX_CELL_TOOLTIPS_OUTER_R)
#define __EGBS_EX_CELL_TOOLTIPS_OUTER			(__EGBS_EX_CELL_TOOLTIPS_OUTER_HORZ|__EGBS_EX_CELL_TOOLTIPS_OUTER_VERT)
#define __EGBS_EX_CELL_EXPANDING_OUTER_L		0x00100000L
#define __EGBS_EX_CELL_EXPANDING_OUTER_R		0x00200000L
#define __EGBS_EX_CELL_EXPANDING_OUTER_T		0x00400000L
#define __EGBS_EX_CELL_EXPANDING_OUTER_B		0x00800000L
#define __EGBS_EX_CELL_EXPANDING_OUTER_HORZ		(__EGBS_EX_CELL_EXPANDING_OUTER_T|__EGBS_EX_CELL_EXPANDING_OUTER_B)
#define __EGBS_EX_CELL_EXPANDING_OUTER_VERT		(__EGBS_EX_CELL_EXPANDING_OUTER_L|__EGBS_EX_CELL_EXPANDING_OUTER_R)
#define __EGBS_EX_CELL_EXPANDING_OUTER			(__EGBS_EX_CELL_EXPANDING_OUTER_HORZ|__EGBS_EX_CELL_EXPANDING_OUTER_VERT)
#define __EGBS_EX_CELL_TOOLTIPS_INNER			0x01000000L
#define __EGBS_EX_CELL_EXPANDING_INNER			0x02000000L
#define __EGBS_EX_CELL_TOOLTIPS					(__EGBS_EX_CELL_TOOLTIPS_INNER|__EGBS_EX_CELL_TOOLTIPS_OUTER)
#define __EGBS_EX_CELL_EXPANDING				(__EGBS_EX_CELL_EXPANDING_INNER|__EGBS_EX_CELL_EXPANDING_OUTER)
#define __EGBS_EX_CELL_TE_MASK					(__EGBS_EX_CELL_TOOLTIPS|__EGBS_EX_CELL_EXPANDING)

#define __EGBS_EX_CORNER_AREAS_3D				0x04000000L
#define __EGBS_EX_CORNER_AREAS_CURVE			0x08000000L

#if (!defined __EXT_MFC_NO_GRIDWND)

// use external data from data provider for
// determening grid dimensions etc.
#define __EGWS_EX_EXTERNAL_DATA					0x10000000L

#if (!defined __EGWS_EX_PM_COLORS)
	#define __EGWS_EX_PM_COLORS					0x80000000L
#endif

#endif // (!defined __EXT_MFC_NO_GRIDWND)

class __PROF_UIS_API CExtGridBaseWnd : public CExtScrollItemWnd
{
// Construction
public:
	DECLARE_DYNCREATE( CExtGridBaseWnd );
	CExtGridBaseWnd();

// Attributes
public:
	int m_nMouseActivateCode;
	DWORD m_dwDefaultScrollTypeH, m_dwDefaultScrollTypeV;
	CRect m_rcDefaultCellBorderHtSpaces;
	HCURSOR m_hCursorDefault,
		m_hCursorResizingH, m_hCursorResizingV,
		m_hCursorOuterSelectionTop, m_hCursorOuterSelectionBottom,
		m_hCursorOuterSelectionLeft, m_hCursorOuterSelectionRight,
		m_hCursorInnerCell, m_hCursorCheckBox, m_hCursorBtnUp,
		m_hCursorBtnDown, m_hCursorBtnDropdown, m_hCursorBtnEllipsis,
		m_hCursorFocusArrow, m_hCursorSortArrow, m_hCursorIcon;

protected:
	LONG m_nCountOfRows, m_nCountOfColumns,
		m_nCountOfOuterRowsT, m_nCountOfOuterColumnsL,
		m_nCountOfOuterRowsB, m_nCountOfOuterColumnsR;
	CSize m_sizeCellDefault;
	CRect m_rcOuterT,m_rcOuterB,m_rcOuterL,m_rcOuterR,
		m_rcOuterLT,m_rcOuterRT,m_rcOuterLB,m_rcOuterRB;
	mutable CRect m_rcClient;
	CPoint m_ptFocusCell, m_ptTrackOffset;
	bool m_bProcessingCancelMode:1,
		m_bLockVirtualCacheUpdateH:1,
		m_bLockVirtualCacheUpdateV:1;
	CArray < CRect, CRect & > m_arrSelAreas;
	CExtGridHitTestInfo m_htInfoHover, m_htInfoResizing,
		m_htInfoExpand, m_htInfoCellPressing, m_htInfoDataDnd;
	UINT m_nTimerIdPopupMenuWaiting, m_nTimerIdDelayedExpand,
		m_nTimerIdExpandWait, m_nTimerIdPressedButton;
	typedef
		CMap < LONG, LONG, INT, INT >
		index2extent_map_t;
	index2extent_map_t
		m_mapExtentsLeft,
		m_mapExtentsRight,
		m_mapExtentsTop,
		m_mapExtentsBottom;
	enum e_MouseTrackType_t
	{
		__EMTT_NOTHING = 0,
		__EMTT_SEL_OUTER_ROWS = 1,
		__EMTT_SEL_OUTER_COLUMNS = 2,
		__EMTT_SEL_INNER_ROWS = 3,
		__EMTT_SEL_INNER_COLUMNS = 4,
		__EMTT_SEL_VAR_ROWS = 5,
		__EMTT_SEL_VAR_COLUMNS = 6,
		__EMTT_SEL_VAR_RANGE = 7,
		__EMTT_RESIZING_H = 8,
		__EMTT_RESIZING_V = 9,
		__EMTT_DATA_DND_STARTING = 10,
	};
	e_MouseTrackType_t m_eMTT;
	INT m_nTrackExtentMin, m_nTrackExtentMax, m_nTrackExtentCurrent;
	HWND m_hWndInplaceControl;

// Operations
public:
	virtual DWORD SiwModifyStyle(
		DWORD dwStyleAdd,
		DWORD dwStyleRemove = 0L,
		bool bUpdateWnd = true
		);
	HWND GetSafeInplaceActiveHwnd();
	DWORD SelectionTypeGet() const;
	DWORD SelectionTypeSet(
		DWORD dwNewSelectionType
		);
	virtual CExtGridHitTestInfo & HitTest(
		CExtGridHitTestInfo & htInfo,
		bool bReAlignCellResizing,
		bool bSupressZeroTopCellResizing,
		bool bComputeOuterDropAfterState = false
		) const;
	virtual LONG OuterColumnCountLeftGet() const;
	virtual LONG OuterColumnCountRightGet() const;
	virtual LONG OuterColumnCountLeftSet(
		LONG nCount = 0L,
		bool bRedraw = true
		);
	virtual LONG OuterColumnCountRightSet(
		LONG nCount = 0L,
		bool bRedraw = true
		);
	virtual LONG OuterRowCountTopGet() const;
	virtual LONG OuterRowCountBottomGet() const;
	virtual LONG OuterRowCountTopSet(
		LONG nCount = 0L,
		bool bRedraw = true
		);
	virtual LONG OuterRowCountBottomSet(
		LONG nCount = 0L,
		bool bRedraw = true
		);
	virtual INT OuterColumnWidthGet(
		bool bLeft,
		LONG nColNo,
		bool bZeroIfNoRows = true
		) const;
	virtual INT OuterRowHeightGet(
		bool bTop,
		LONG nRowNo,
		bool bZeroIfNoColumns = true
		) const;
	virtual void OuterColumnWidthSet(
		bool bLeft,
		LONG nColNo,
		INT nWidth
		);
	virtual void OuterRowHeightSet(
		bool bTop,
		LONG nRowNo,
		INT nHeight
		);
	virtual void OuterExtentsClearAll(
		bool bLeft = true,
		bool bRight = true,
		bool bTop = true,
		bool bBottom = true
		);
	virtual LONG ColumnCountGet() const;
	virtual LONG ColumnCountSet(
		LONG nCount = -1L,
		bool bRedraw = true
		);
	virtual bool ColumnInsert(
		LONG nColNo, // -1 or greater than count - append
		LONG nColInsertCount = 1L,
		bool bRedraw = true
		);
	bool ColumnAdd(
		LONG nColInsertCount = 1L,
		bool bRedraw = true
		)
	{
		return
			ColumnInsert(
				-1,
				nColInsertCount,
				bRedraw
				);
	}
	virtual LONG ColumnRemove(
		LONG nColNo,
		LONG nColRemoveCount = 1L, // -1 - remove up to end (truncate)
		bool bRedraw = true
		);
	LONG ColumnTruncate(
		LONG nColNo,
		bool bRedraw = true
		)
	{
		return ColumnRemove( nColNo, -1L, bRedraw );
	}
	LONG ColumnRemoveAll(
		bool bRedraw = true
		)
	{
		return ColumnTruncate( 0L, bRedraw );
	}
	virtual LONG RowCountGet() const;
	virtual LONG RowCountSet(
		LONG nCount = -1L,
		bool bRedraw = true
		);
	virtual bool RowInsert(
		LONG nRowNo, // -1 or greater than count - append
		LONG nRowInsertCount = 1L,
		bool bRedraw = true
		);
	bool RowAdd(
		LONG nRowInsertCount = 1L,
		bool bRedraw = true
		)
	{
		return
			RowInsert(
				-1,
				nRowInsertCount,
				bRedraw
				);
	}
	virtual LONG RowRemove(
		LONG nRowNo,
		LONG nRowRemoveCount = 1L, // -1 - remove up to end (truncate)
		bool bRedraw = true
		);
	LONG RowTruncate(
		LONG nRowNo,
		bool bRedraw = true
		)
	{
		return RowRemove( nRowNo, -1L, bRedraw );
	}
	LONG RowRemoveAll(
		bool bRedraw = true
		)
	{
		return RowTruncate( 0L, bRedraw );
	}
	virtual INT DefaultColumnWidthGet() const;
	virtual INT DefaultColumnWidthSet(
		INT nColumnWidth,
		bool bRedraw = true
		);
	virtual INT DefaultRowHeightGet() const;
	virtual INT DefaultRowHeightSet(
		INT nRowHeight,
		bool bRedraw = true
		);
	bool FixedSizeColumnsGet() const;
	bool FixedSizeColumnsSet(
		bool bFixedSizeColumns,
		bool bRedraw = true
		);
	bool FixedSizeRowsGet() const;
	bool FixedSizeRowsSet(
		bool bFixedSizeRows,
		bool bRedraw = true
		);
	virtual bool EnsureVisibleColumn(
		LONG nColNo,
		bool bRedraw = true
		);
	virtual bool EnsureVisibleRow(
		LONG nRowNo,
		bool bRedraw = true
		);
	virtual INT SelectionGetHitCount(
		LONG nColNo,
		LONG nRowNo
		) const;
	virtual bool SelectionGetForCell(
		LONG nColNo,
		LONG nRowNo
		) const;
	virtual INT SelectionGetAreaCount() const;
	virtual CRect SelectionGet(
		bool bNormalized,
		INT nAreaNumber // -1 - get normalized union of all areas
		) const;
	virtual bool SelectionRemoveAt(
		INT nAreaNumber,
		bool bRedraw = true
		);
	virtual bool SelectionSetAt(
		INT nAreaNumber,
		const RECT & rcNewSelection,
		bool bRedraw = true
		);
	virtual bool SelectionInsertAt(
		INT nAreaNumber,
		const RECT & rcNewSelection,
		bool bRedraw = true
		);
	virtual bool SelectionSet(
		const RECT & rcNewSelection,
		bool bReplaceOldAreas = true,
		bool bReplaceLastArea = false,
		bool bRedraw = true
		);
	bool SelectionSet(
		const POINT & ptNewSelection,
		bool bReplaceOldAreas = true,
		bool bReplaceLastArea = false,
		bool bRedraw = true
		);
	bool SelectionIsEmpty() const;
	virtual bool SelectionUnset(
		bool bSetToFocus = true,
		bool bRedraw = true
		);
	virtual LONG SelectionGetForCellNextPrev(
		bool bSearchHorz,
		bool bSearchNext,
		LONG nColNo,
		LONG nRowNo
		) const;
	LONG SelectionGetNextRowInColumn(
		LONG nColNo,
		LONG nRowNo
		) const;
	LONG SelectionGetPrevRowInColumn(
		LONG nColNo,
		LONG nRowNo
		) const;
	LONG SelectionGetNextColumnInRow(
		LONG nColNo,
		LONG nRowNo
		) const;
	LONG SelectionGetPrevColumnInRow(
		LONG nColNo,
		LONG nRowNo
		) const;
	LONG SelectionGetFirstRowInColumn(
		LONG nColNo
		) const;
	LONG SelectionGetLastRowInColumn(
		LONG nColNo
		) const;
	LONG SelectionGetFirstColumnInRow(
		LONG nRowNo
		) const;
	LONG SelectionGetLastColumnInRow(
		LONG nRowNo
		) const;
	virtual CPoint FocusGet() const;
	virtual CPoint FocusSet(
		const POINT & ptNewFocus,
		bool bEnsureVisibleColumn = true,
		bool bEnsureVisibleRow = true,
		bool bResetSelectionToFocus = true,
		bool bRedraw = true,
		bool * p_bCanceled = NULL
		);
	CPoint FocusUnset(
		bool bRedraw = true
		);
	bool AutoFocusBottomRightGet() const;
	bool AutoFocusBottomRightSet(
		bool bAutoFocusBottomRight
		);
	bool MultiAreaSelectionGet() const;
	bool MultiAreaSelectionSet(
		bool bMultiAreaSelection
		);
	bool NoHideSelectionGet() const;
	bool NoHideSelectionSet(
		bool bNoHideSelection,
		bool bRedraw = true
		);
	bool GridLinesHorzGet() const;
	bool GridLinesHorzSet(
		bool bGridLines,
		bool bRedraw = true
		);
	bool GridLinesVertGet() const;
	bool GridLinesVertSet(
		bool bGridLines,
		bool bRedraw = true
		);
	bool LbExtSelectionGet() const;
	bool LbExtSelectionSet( bool bLbExtSelection );
	bool SubtractSelectionAreasGet() const;
	bool SubtractSelectionAreasSet(
		bool bSubtractSelectionAreas,
		bool bRedraw = true
		);
	virtual void SelectionSplitAllAreas(
		bool bSplitByColumns,
		bool bSplitByRows
		);
	bool HoverEventsInnerGet() const;
	bool HoverEventsOuterGet() const;
	void HoverEventsSet(
		bool bHoverInnerCells = true,
		bool bHoverOuterCells = true
		);
	bool HoverHighlightCellInnerGet() const;
	bool HoverHighlightCellOuterGet() const;
	bool HoverHighlightColumnInnerGet() const;
	bool HoverHighlightColumnOuterGet() const;
	bool HoverHighlightRowInnerGet() const;
	bool HoverHighlightRowOuterGet() const;
	void HoverHighlightSet(
		bool bHighlightSingleCellsInner = true,
		bool bHighlightSingleCellsOuter = true,
		bool bHighlightColumnCellsInner = true,
		bool bHighlightColumnCellsOuter = true,
		bool bHighlightRowCellsInner = true,
		bool bHighlightRowCellsOuter = true
		);
	CExtGridHitTestInfo & HoverInfoGet();
	const CExtGridHitTestInfo & HoverInfoGet() const;

	bool SelectionHighlightColumnInnerGet() const;
	bool SelectionHighlightColumnOuterGet() const;
	bool SelectionHighlightRowInnerGet() const;
	bool SelectionHighlightRowOuterGet() const;
	void SelectionHighlightSet(
		bool bHighlightColumnCellsInner = true,
		bool bHighlightColumnCellsOuter = true,
		bool bHighlightRowCellsInner = true,
		bool bHighlightRowCellsOuter = true
		);

	bool FocusHighlightColumnInnerGet() const;
	bool FocusHighlightColumnOuterGet() const;
	bool FocusHighlightRowInnerGet() const;
	bool FocusHighlightRowOuterGet() const;
	void FocusHighlightSet(
		bool bHighlightColumnCellsInner = true,
		bool bHighlightColumnCellsOuter = true,
		bool bHighlightRowCellsInner = true,
		bool bHighlightRowCellsOuter = true
		);
	bool EditCell( // default parameters will start to edit focused cell
		LONG nColNo = -1L,
		LONG nRowNo = -1L,
		INT nColType = 0,
		INT nRowType = 0,
		bool bContinueMsgLoop = true
		);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtGridBaseWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	virtual void PostNcDestroy();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtGridBaseWnd();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext & dc ) const;
#endif // _DEBUG

	virtual bool OnGbwCalcVisibleItemRect(
		LONG nColNo,
		LONG nRowNo,
		INT nColType,
		INT nRowType,
		RECT & rcCellExtra,
		RECT & rcCell,
		LONG * p_nVisibleColNo = NULL,
		LONG * p_nVisibleRowNo = NULL
		) const;
	virtual void OnGbwWalkVisibleAreas(
		CDC & dc,
		bool bFocusedControl,
		CExtGridHitTestInfo * pHT = NULL
		) const;

	virtual void OnSiwPaintBackground(
		CDC & dc,
		bool bFocusedControl
		) const;
	virtual void OnSiwPaintForeground(
		CDC & dc,
		bool bFocusedControl
		) const;
	virtual void OnGbwPaintCell(
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
		) const;
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
	virtual void OnGbwPaintExtraSpaceH(
		bool bPostPaint,
		CDC & dc,
		const RECT & rcRowExtra,
		const RECT & rcRow,
		LONG nVisibleRowNo,
		LONG nRowNo,
		const RECT & rcVisibleRange,
		DWORD dwAreaFlags,
		bool bFocusedControl
		) const;
	virtual void OnGbwPaintExtraSpaceV(
		bool bPostPaint,
		CDC & dc,
		const RECT & rcColExtra,
		const RECT & rcCol,
		LONG nVisibleColNo,
		LONG nColNo,
		const RECT & rcVisibleRange,
		DWORD dwAreaFlags,
		bool bFocusedControl
		) const;
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
	virtual CSize OnSiwCalcItemSize() const;
	virtual CSize OnSwGetTotalSize() const;
	virtual bool OnSwDoScrollBy(
		CSize sizeScroll,
		bool bDoScroll = true
		);
	virtual CSize OnSiwCalcPageMetrics( int nDirection ) const;
	virtual void OnGbwQueryVisibleColumnRange(
		LONG & nColFirst,
		LONG & nColLast,
		LONG & nColFirstOffset
		) const;
	virtual void OnGbwQueryVisibleRowRange(
		LONG & nRowFirst,
		LONG & nRowLast,
		LONG & nRowFirstOffset
		) const;
	virtual bool OnSwDoMouseWheel(
		UINT fFlags,
		short zDelta,
		CPoint point
		);
	virtual CRect OnSwRecalcLayout(
		bool bDoLayout,
		LPCRECT pRectClientSrc = NULL
		);
	virtual void OnGbwEraseArea(
		CDC & dc,
		const RECT & rcArea,
		DWORD dwAreaFlags
		) const;
	virtual CRect OnGbwQueryCellBorderHtSpaces(
		CExtGridHitTestInfo * pHT = NULL // if NULL - get default resizing border spaces
		) const;
	virtual LONG OnGbwQueryPixelColumnOffset(
		LONG nColNo
		);
	virtual LONG OnGbwQueryPixelRowOffset(
		LONG nRowNo
		);
	virtual bool OnGbwSetCursor(
		const CExtGridHitTestInfo & htInfo
		);
	virtual bool OnGbwCanResizeColumn(
		LONG nColNo,
		INT * p_nExtentMin = NULL,
		INT * p_nExtentMax = NULL
		);
	virtual bool OnGbwCanResizeRow(
		LONG nRowNo,
		INT * p_nExtentMin = NULL,
		INT * p_nExtentMax = NULL
		);
	virtual bool OnGbwQueryTrackingCellMenu() const;
	virtual void OnGbwHoverRemove(
		bool bAllowReleaseCapture = true
		);
	virtual bool OnGbwHoverRecalc(
		const POINT * pPtClient = NULL
		);
	virtual void OnGbwHoverCell(
		const CExtGridHitTestInfo & htInfoPrevious,
		const CExtGridHitTestInfo & htInfoCurrent,
		bool bHoverEnter,
		bool bHoverLeave
		);
	virtual bool OnGbwAnalyzeCellMouseClickEvent(
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nRepCnt, // 0 - button up, 1 - signle click, 2 - double click, 3 - post single click & begin editing
		UINT nFlags, // mouse event flags
		CPoint point // mouse pointer in client coordinates
		);
	virtual bool OnGbwAnalyzeCellKeyEvent(
		bool bKeyDownEvent, // true - key-down event, false - key-up event
		UINT nChar, // virtual key code
		UINT nRepCnt, // key-down/key-up press count
		UINT nFlags // key-down/key-up event flags
		);
	virtual bool OnGbwBeginEdit(
		LONG nVisibleColNo,
		LONG nVisibleRowNo,
		LONG nColNo,
		LONG nRowNo,
		INT nColType,
		INT nRowType,
		const RECT & rcCellExtra,
		const RECT & rcCell,
		const RECT & rcInplaceControl,
		bool bContinueMsgLoop = true
		);
	virtual bool OnGbwSelectionCanModify();
	virtual void OnGbwSelectionChanged();
	virtual bool OnGbwFocusChanging(
		const POINT & ptOldFocus,
		const POINT & ptNewFocus,
		bool & bEnsureVisibleColumn,
		bool & bEnsureVisibleRow,
		bool & bResetSelectionToFocus,
		bool & bRedraw
		);
	virtual void OnGbwFocusChanged(
		const POINT & ptOldFocus,
		const POINT & ptNewFocus
		);
	virtual void OnGbwResizingStateUpdate(
		bool bInitial,
		bool bFinal,
		const CPoint * p_ptClient = NULL
		);
	virtual void OnGbwResizingStateApply(
		bool bHorz,
		LONG nItemNo,
		INT nItemExtent
		);
	virtual CRect OnSwGetClientRect() const;
	virtual void OnSwUpdateScrollBars();

protected:
	void _AddToSelArrayImpl( CRect & rcApplyNew );
	virtual void _InvalidateHoveredArea(
		const CExtGridHitTestInfo & htInfo
		);
	bool _DoSetCursor(
		const POINT & ptClient
		);
	bool _DoSetCursor();
	void _AnalyzeVirtualFocus(
		bool bRedraw = true
		);
	CPoint _AnalyzeVirtualFocus(
		const POINT & ptFocusOld
		);
	CRect _ResizingTrackerRect( INT nExtentCalc );
	void _ResizingTrackerDraw( const CRect & rcTracker );

public:
	void EnableTooltips(
		bool bTooltipsOuterTop = true,
		bool bTooltipsOuterBottom = true,
		bool bTooltipsOuterLeft = true,
		bool bTooltipsOuterRight = true,
		bool bTooltipsInner = false
		);
	void EnableExpanding(
		bool bExpandingOuterTop = false,
		bool bExpandingOuterBottom = false,
		bool bExpandingOuterLeft = false,
		bool bExpandingOuterRight = false,
		bool bExpandingInner = true
		);
protected:
	CToolTipCtrl m_wndToolTip;
	mutable CExtContentExpandWnd m_wndContentExpand;
	INT m_nSizeOfExpandShadow;

public:
	CToolTipCtrl & GetToolTipCtrl();
	const CToolTipCtrl & GetToolTipCtrl() const;
	CExtContentExpandWnd & GetContentExpandWnd();
	const CExtContentExpandWnd & GetContentExpandWnd() const;
	INT ExpandShadowSizeGet() const;
	INT ExpandShadowSizeSet( INT nSizeOfNewShadow );

	virtual bool _CreateHelper();

	virtual bool OnGbwTooltip(
		const CExtGridHitTestInfo & htInfo
		);
	virtual bool OnGbwExpand(
		const CExtGridHitTestInfo & htInfo,
		UINT nDelayMilliseconds
		);
	virtual bool OnGbwPaintExpandedItemContent(
		CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA & picd
		) const;

protected:
	virtual bool OnGbwProcessVkBeginEdit( UINT nChar );
	
	virtual bool OnGbwDataDndIsAllowed() const;
	virtual CPoint OnGbwDataDndGetStartOffset() const;
	virtual bool OnGbwDataDndCanStart(
		const CExtGridHitTestInfo & htInfo
		);
	virtual void OnGbwDataDndDo(
		const CExtGridHitTestInfo & htInfo
		);

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtGridBaseWnd)
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnCancelMode();
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
#if _MFC_VER < 0x700
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#else
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
#endif
	afx_msg LRESULT _OnPaintExpandedItemContent(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
}; // class CExtGridBaseWnd

#if (!defined __EXT_MFC_NO_GRIDWND)

/////////////////////////////////////////////////////////////////////////////
// CExtAlloc

#ifndef __EXT_ALLOC_MAX_REASONABLE_COUNT_IN_CHUNK
	#define __EXT_ALLOC_MAX_REASONABLE_COUNT_IN_CHUNK (10L*1000L)
#endif

#ifdef _DEBUG
	#define __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	//#define __EXT_DEBUG_ALLOC_TRACE_NORMAL_EVENTS
#endif

#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	#define __EXT_DEBUG_ALLOC_TRACE0(__x__) TRACE0(__x__)
	#define __EXT_DEBUG_ALLOC_TRACE1(__x__,__y__) TRACE1(__x__,__y__)
	#define __EXT_DEBUG_ALLOC_TRACE2(__x__,__y__,__z__) TRACE2(__x__,__y__,__z__)
	#define __EXT_DEBUG_ALLOC_TRACE3(__x__,__y__,__z__,__w__) TRACE3(__x__,__y__,__z__,__w__)
#else // __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	#define __EXT_DEBUG_ALLOC_TRACE0(__x__)
	#define __EXT_DEBUG_ALLOC_TRACE1(__x__,__y__)
	#define __EXT_DEBUG_ALLOC_TRACE2(__x__,__y__,__z__) 
	#define __EXT_DEBUG_ALLOC_TRACE3(__x__,__y__,__z__,__w__)
#endif // !__EXT_DEBUG_ALLOC_TRACK_NUMBERS

class __PROF_UIS_API CExtAlloc : public IMalloc
{
protected:
	CExtAlloc(
		ULONG nCountInChunk,
		bool bHeapMinimizeOnTheFly,
		IUnknown * pUnkOuter,
		bool bZeroNewMemory
		);
	virtual ~CExtAlloc();

	IUnknown * m_pUnkOuter;
	ULONG m_nRefCount, m_nCountInChunk, m_nSizeFreeMap;
#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	ULONG m_nDebugHighLevelAllocNo,
		m_nDebugHighLevelBreakAllocNo,
		m_nDebugHighLevelAllocCount;
#endif // __EXT_DEBUG_ALLOC_TRACK_NUMBERS
#ifdef _DEBUG
	bool m_bDebugInDesructor:1;
#endif // _DEBUG
	bool m_bHeapMinimizeOnTheFly:1;
	bool m_bZeroNewMemory:1;

	struct AFX_NOVTABLE _1_header_chunk_t
	{
		ULONG m_nBlockSize;
		IMalloc * m_pMalloc;
#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
		ULONG m_nDebugHighLevelAllocNo;
#endif // __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	}; // struct _1_header_chunk_t
	struct AFX_NOVTABLE _N_header_chunk_t
	{
		ULONG m_nHdrOffset; // positive
#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
		ULONG m_nDebugHighLevelAllocNo;
#endif // __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	}; // struct _N_header_chunk_t
	struct AFX_NOVTABLE _N_header_block_t
	{
		IMalloc * m_pMalloc;
		_N_header_block_t * m_pNext, * m_pPrev;
		ULONG m_nItemSize;
#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
		ULONG m_nDebugHighLevelAllocCount;
#endif // __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	}; // struct _N_header_block_t
	typedef
		CMap < ULONG, ULONG, _N_header_block_t *, _N_header_block_t * >
		HighLevelChunksMap_t;
	HighLevelChunksMap_t m_mapHighLevelChunks;
	bool _HighLevel_N_BlockIsFullyFree( _N_header_block_t * pHdr );
	bool _HighLevel_N_BlockHasFI( _N_header_block_t * pHdr );
	_N_header_block_t * _HighLevel_N_BlockGetWithFI( ULONG nSizeItem );
	ULONG _HighLevel_N_BlockGetTotlalSize( ULONG nSizeItem );
	void * _HighLevel_N_BlockGetFirstFI( _N_header_block_t * pHdr, bool bMarkAllocated );
	void _HighLevel_N_BlockOptimizeEmpty( _N_header_block_t * pHdr );
	void _HighLevel_N_BlockFree( _N_header_block_t * pHdr );
	void * _HighLevel_1Alloc(
		ULONG cb
		);
	void * _HighLevel_NAlloc(
		ULONG cb
		);
	void * _HighLevel_1Realloc( 
		void * pv,
		ULONG cb
		);
	void * _HighLevel_NRealloc( 
		void * pv,
		ULONG cb
		);
	void _HighLevel_1Free( 
		void * pv
		);
	void _HighLevel_NFree( 
		void * pv
		);
	ULONG _HighLevel_1GetSize( 
		void * pv
		);
	ULONG _HighLevel_NGetSize( 
		void * pv
		);
	int _HighLevel_1DidAlloc( 
		void * pv
		);
	int _HighLevel_NDidAlloc( 
		void * pv
		);
	void _HighLevel_1HeapMinimize();
	void _HighLevel_NHeapMinimize();
	virtual void * _LowLevel_alloc( ULONG nSize );
	virtual void _LowLevel_free( void * pMemory );
	virtual void * _LowLevel_realloc( void * pMemory, ULONG nSize );
#ifdef __EXT_DEBUG_ALLOC_TRACK_NUMBERS
	void _HighLevel_trace_leaks();
	typedef
		CMap < LPVOID, LPVOID, ULONG, ULONG >
		CrtLibTraceMap_t;
	CrtLibTraceMap_t m_mapLowLevelAllocationsTrace;
	virtual void _LowLevel_trace_leaks();
	struct AFX_NOVTABLE _CRT_header_chunk_t
	{
		ULONG m_nBlockSize;
		IMalloc * m_pMalloc;
		ULONG m_nDebugLowLevelAllocNo;
	}; // struct _CRT_header_chunk_t
	ULONG m_nDebugLowLevelAllocNo, m_nDebugLowLevelBreakAllocNo;
#endif // __EXT_DEBUG_ALLOC_TRACK_NUMBERS

	static bool _LowLevel_IsValidPointer(
		const CExtAlloc * pAlloc,
		bool bCheckReadAccess = true,
		bool bCheckWriteAccess = true
		);
	static bool _LowLevel_IsValidPointer(
		LPCVOID pMemory,
		ULONG nByteCount,
		bool bCheckReadAccess = true,
		bool bCheckWriteAccess = true
		);

public:
	static IMalloc * CreateObject(
		ULONG nCountInChunk = 256,
		bool bHeapMinimizeOnTheFly = false,
		IUnknown * pUnkOuter = NULL,
		bool bZeroNewMemory = true
		);

	/////////////////////// IUnknown /////////////////////////
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        REFIID riid,
        void ** ppvObject
		);
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

	/////////////////////// IMalloc /////////////////////////
	virtual void * STDMETHODCALLTYPE Alloc(
		ULONG cb
		);
	virtual void * STDMETHODCALLTYPE Realloc( 
		void * pv,
		ULONG cb
		);
	virtual void STDMETHODCALLTYPE Free( 
		void * pv
		);
	virtual ULONG STDMETHODCALLTYPE GetSize( 
		void * pv
		);
	virtual int STDMETHODCALLTYPE DidAlloc( 
		void * pv
		);
	virtual void STDMETHODCALLTYPE HeapMinimize(void);
}; // class __PROF_UIS_API CExtAlloc

/////////////////////////////////////////////////////////////////////////////
// CExtArchiveIStream

class __PROF_UIS_API CExtArchiveIStream : public IStream
{
	ULONG m_nRefCount;
	CArchive * m_pAr;
	bool m_bAutoDestoyArchive:1;
public:
	CExtArchiveIStream(
		CArchive * pAr = NULL,
		bool bAutoDestoyArchive = false
		);
	CExtArchiveIStream(
		CArchive & ar,
		bool bAutoDestoyArchive = false
		);
	~CExtArchiveIStream();
	CArchive * ArchiveAttach(
		CArchive * pAr = NULL,
		bool bAutoDestoyArchive = false
		);
	CArchive * ArchiveAttach(
		CArchive & ar,
		bool bAutoDestoyArchive = false
		);
	CArchive * ArchiveDetach();
	operator IStream * ()
	{
		return this;
	}
public:

	/////////////////////// IUnknown /////////////////////////
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        REFIID riid,
        void ** ppvObject
		);
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);
	/////////////////////// ISequentialStream /////////////////////////
    virtual HRESULT STDMETHODCALLTYPE Read( 
        void * pv,
        ULONG cb,
        ULONG * pcbRead
		);
    virtual HRESULT STDMETHODCALLTYPE Write( 
        const void * pv,
        ULONG cb,
        ULONG * pcbWritten
		);
	
	/////////////////////// IStream /////////////////////////
    virtual HRESULT STDMETHODCALLTYPE Seek( 
        LARGE_INTEGER dlibMove,
        DWORD dwOrigin,
        ULARGE_INTEGER * plibNewPosition
		);
    virtual HRESULT STDMETHODCALLTYPE SetSize( 
        ULARGE_INTEGER libNewSize
		);
    virtual HRESULT STDMETHODCALLTYPE CopyTo( 
        IStream * pstm,
        ULARGE_INTEGER cb,
        ULARGE_INTEGER * pcbRead,
        ULARGE_INTEGER * pcbWritten
		);
    virtual HRESULT STDMETHODCALLTYPE Commit( 
        DWORD grfCommitFlags
		);
    virtual HRESULT STDMETHODCALLTYPE Revert(void);
    virtual HRESULT STDMETHODCALLTYPE LockRegion( 
        ULARGE_INTEGER libOffset,
        ULARGE_INTEGER cb,
        DWORD dwLockType
		);
    virtual HRESULT STDMETHODCALLTYPE UnlockRegion( 
        ULARGE_INTEGER libOffset,
        ULARGE_INTEGER cb,
        DWORD dwLockType
		);
    virtual HRESULT STDMETHODCALLTYPE Stat( 
        STATSTG * pstatstg,
        DWORD grfStatFlag
		);
    virtual HRESULT STDMETHODCALLTYPE Clone( 
        IStream * * ppstm
		);
}; // class CExtArchiveIStream

/////////////////////////////////////////////////////////////////////////////
// CExtGridCell

class CExtGridHitTestInfo;
class CExtGridWnd;
class CExtGridDataProvider;

// values for dwHelperPaintFlags parameter of:
//    CExtGridBaseWnd::OnGbwPaintCell()
//    CExtGridCell::OnPaintBackground()
//    CExtGridCell::OnPaintForeground()

// column of the cell is hovered
#define __EGCPF_HOVER_BY_COLUMN						0x00000001L
// row of the cell is hovered
#define __EGCPF_HOVER_BY_ROW						0x00000002L
// column of the cell contains at least one selected cell
#define __EGCPF_HIGHLIGHTED_BY_SELECTED_COLUMN		0x00000004L
// row of the cell contains at least one selected cell
#define __EGCPF_HIGHLIGHTED_BY_SELECTED_ROW			0x00000008L
// column of the cell contains focused cell
#define __EGCPF_HIGHLIGHTED_BY_FOCUSED_COLUMN		0x00000010L
// row of the cell contains focused cell
#define __EGCPF_HIGHLIGHTED_BY_FOCUSED_ROW			0x00000020L
// grid control or any kind of its child window is focused
#define __EGCPF_FOCUSED_CONTROL						0x00000040L
// cell is highlighted by pressed row/column
#define __EGCPF_HIGHLIGHTED_BY_PRESSED_COLUMN		0x00000080L
#define __EGCPF_HIGHLIGHTED_BY_PRESSED_ROW			0x00000100L
// painting dragged outer cell content
#define __EGCPF_OUTER_DND							0x00000200L

// values for CExtGridCell::m_dwStyle

// horizontal text align
#define __EGCS_TA_HORZ_BY_TYPE						0x00000000L
#define __EGCS_TA_HORZ_LEFT							0x00000001L
#define __EGCS_TA_HORZ_RIGHT						0x00000002L
#define __EGCS_TA_HORZ_CENTER						0x00000003L
#define __EGCS_TA_HORZ_MASK							0x00000003L
// vertical text align
#define __EGCS_TA_VERT_BY_TYPE						0x00000000L
#define __EGCS_TA_VERT_TOP							0x00000010L
#define __EGCS_TA_VERT_BOTTOM						0x00000020L
#define __EGCS_TA_VERT_CENTER						0x00000030L
#define __EGCS_TA_VERT_MASK							0x00000030L
// horizontal icon/check align ( __EGCS_ICA_HORZ_BY_TYPE by default
// is the same as __EGCS_ICA_HORZ_CENTER, __EGCS_ICA_HORZ_CENTER
// is used only when text is empty)
#define __EGCS_ICA_HORZ_BY_TYPE						0x00000000L
#define __EGCS_ICA_HORZ_LEFT						0x00000100L
#define __EGCS_ICA_HORZ_RIGHT						0x00000200L
#define __EGCS_ICA_HORZ_CENTER						0x00000300L
#define __EGCS_ICA_HORZ_MASK						0x00000300L
// vertical icon/check align ( __EGCS_ICA_VERT_BY_TYPE by default
// is the same as __EGCS_ICA_VERT_CENTER)
#define __EGCS_ICA_VERT_BY_TYPE						0x00000000L
#define __EGCS_ICA_VERT_TOP							0x00001000L
#define __EGCS_ICA_VERT_BOTTOM						0x00002000L
#define __EGCS_ICA_VERT_CENTER						0x00003000L
#define __EGCS_ICA_VERT_MASK						0x00003000L
// draw "..." if not enough room for cell's text
#define __EGCS_TEXT_ELLIPSIS						0x00000008L
// draw row/column number in header cell
#define __EGCS_HDR_ROW_COLUMN_NUMBER				0x00000080L
// draw focus arrow in header cell
#define __EGCS_HDR_FOCUS_ARROW_RESERVE_SPACE		0x00000800L
// draw focus arrow in header cell
#define __EGCS_HDR_FOCUS_ARROW_DISPLAY				0x00008000L
// checkmark type
#define __EGCS_CHK_NONE								0x00000000L
#define __EGCS_CHK_CHECK							0x00010000L
#define __EGCS_CHK_RADIO							0x00020000L
#define __EGCS_CHK_INDETERMINATE					0x00030000L
#define __EGCS_CHK_MASK								0x00030000L
// checkmark state
#define __EGCS_CHECKED								0x00080000L
// have sort-arrow
#define __EGCS_SORT_ARROW							0x00100000L
// have sort-arrow is descending
#define __EGCS_SORT_DESCENDING						0x00200000L
// have updown button
#define __EGCS_BUTTON_UPDOWN						0x00400000L
#define __EGCS_PRESSED_UP							0x00800000L
#define __EGCS_PRESSED_DOWN							0x01000000L
// have dropdown button
#define __EGCS_BUTTON_DROPDOWN						0x02000000L
#define __EGCS_PRESSED_DROPDOWN						0x04000000L
// have ellipsis button
#define __EGCS_BUTTON_ELLIPSIS						0x08000000L
#define __EGCS_PRESSED_ELLIPSIS						0x10000000L
// read-only cell mode
#define __EGCS_READ_ONLY							0x20000000L
// display dropdown listbox-menu when dropdown button clicked
#define __EGCS_TRACK_DROPDOWN_MENU					0x40000000L
// force disable inplace-condrol
#define __EGCS_NO_INPLACE_CONTROL					0x80000000L
// default style set for CExtGridCell::m_dwStyle
#define __EGCS_DEFAULT_STYLES						(__EGCS_TRACK_DROPDOWN_MENU)
// default value for dwStyleMask parameter
// of CExtGridCell::Compare()
#define __EGCS_COMPARE_MASK							(__EGCS_CHK_MASK|__EGCS_CHECKED)

// values for CExtGridCell::m_dwStyleEx

// disable column/row sorting on this cell click
#define __EGCS_EX_DISABLE_SORT						0x00000001L
// disable column/row dragging
#define __EGCS_EX_DISABLE_START_DRAG				0x00000002L
// default style set for CExtGridCell::m_dwStyleEx
#define __EGCS_EX_DEFAULT_STYLES					0x00000000L
// default value for dwStyleExMask parameter
// of CExtGridCell::Compare()
#define __EGCS_EX_COMPARE_MASK						0x00000000L

class CExtPopupInplaceListBox;
class CExtPopupListBoxMenuWnd;

class __PROF_UIS_API CExtGridCell
	: public CObject
{
public:
	DECLARE_DYNCREATE( CExtGridCell );
protected:
	CExtGridDataProvider * m_pDataProvider;
	DWORD m_dwStyle, m_dwStyleEx;
	INT m_nIconIndex;
	LPARAM m_lParam;
public:
	CExtGridCell(
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCell( const CExtGridCell & other );
	virtual ~CExtGridCell();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext & dc ) const;
#endif // _DEBUG
	virtual LPARAM LParamGet() const;
	virtual LPARAM LParamSet( LPARAM lParamNew = 0L );
	virtual DWORD GetStyle() const;
	virtual DWORD ModifyStyle(
		DWORD dwStyleAdd,
		DWORD dwStyleRemove = 0L
		);
	virtual DWORD GetStyleEx() const;
	virtual DWORD ModifyStyleEx(
		DWORD dwStyleExAdd,
		DWORD dwStyleExRemove = 0L
		);
protected:
	void _AssignM( const CExtGridCell & other );
	void _InitM();
public:
	virtual CExtGridDataProvider * DataProviderGet();
	const CExtGridDataProvider * DataProviderGet() const;
	virtual CExtGridDataProvider * DataProviderSet(
		CExtGridDataProvider * pDataProvider
		);
	virtual void Serialize( CArchive & ar );
	virtual bool IsEmpty() const;
	virtual void Empty();
	virtual void Assign( const CExtGridCell & other );
	CExtGridCell & operator = (  const CExtGridCell & other );
	virtual int Compare(
		const CExtGridCell & other,
		DWORD dwStyleMask = __EGCS_COMPARE_MASK,
		DWORD dwStyleExMask = __EGCS_EX_COMPARE_MASK
		) const;
	virtual int CompareStyleOnly(
		const CExtGridCell & other,
		DWORD dwStyleMask,
		DWORD dwStyleExMask
		) const;
	virtual void TextGet( CExtSafeString & strCopy ) const;
	virtual void TextSet(
		__EXT_MFC_SAFE_LPCTSTR str = __EXT_MFC_SAFE_LPCTSTR(NULL), // empty text
		bool bAllowChangeDataType = false
		);
	virtual void GetVariant( VARIANT & varCopy ) const;
	bool operator == ( const CExtGridCell & other ) const;
	bool operator != ( const CExtGridCell & other ) const;
	bool operator < ( const CExtGridCell & other ) const;
	bool operator > ( const CExtGridCell & other ) const;
	bool operator <= ( const CExtGridCell & other ) const;
	bool operator >= ( const CExtGridCell & other ) const;
	static HRESULT stat_VariantSerializeData(
		LPVARIANT pVariant,
		IStream * pStream,
		bool bStore,
		VARTYPE * pVarTypeSerialized
		);
	static HRESULT stat_VariantSerialize(
		LPVARIANT pVariant,
		IStream * pStream,
		bool bStore
		);
	static HRESULT stat_VariantSerialize(
		LPVARIANT pVariant,
		CArchive & ar
		);
	static int stat_VariantCompare(
		LPVARIANT pVariantLeft,
		LPVARIANT pVariantRight,
		LCID lcid = LOCALE_USER_DEFAULT,
		ULONG dwFlags = 0L
		);
	static HRESULT stat_VariantCompareHR(
		LPVARIANT pVariantLeft,
		LPVARIANT pVariantRight,
		LCID lcid = LOCALE_USER_DEFAULT,
		ULONG dwFlags = 0L,
		bool bReturnOnlyValidResult = true // return only VARCMP_EQ or VARCMP_GT or VARCMP_LT values
		);
	virtual __EXT_MFC_SAFE_LPCTSTR GetTextBuffer() const;
	virtual CExtGridCell * Clone(
		IMalloc * pMalloc = NULL
		) const;
	void * operator new (
		size_t nSize
		)
	{
		return CObject :: operator new ( nSize );
	}
	void operator delete (
		void * pMemory
		)
	{
		CObject :: operator delete ( pMemory );
	}
	void * operator new (
		size_t nSize,
		IMalloc * pMalloc,
		bool bThrowException
		);
	void operator delete (
		void * pMemory,
		IMalloc * pMalloc,
		bool bThrowException
		);
#ifdef _DEBUG
	void * operator new [] ( // must never be used!
		size_t nSize
		);
	void operator delete [] ( // must never be used!
		void * pMemory
		);
	void * operator new [] ( // must never be used!
		size_t nSize,
		IMalloc * pMalloc,
		bool bThrowException
		);
	void operator delete [] ( // must never be used!
		void * pMemory,
		IMalloc * pMalloc,
		bool bThrowException
		);
#endif // _DEBUG
	static CExtGridCell * InstanceCreate(
		CRuntimeClass * pRTC,
		IMalloc * pMalloc,
		bool bThrowException = false
		);
	static CExtGridCell * InstanceCreate(
		const CExtGridCell & cell,
		bool bCopyData,
		IMalloc * pMalloc,
		bool bThrowException = false
		);
	static void InstanceDestroy(
		CExtGridCell * pCell,
		IMalloc * pMalloc,
		bool bThrowException = false
		);
	static CExtGridCell * InstanceLoad(
		CArchive & ar,
		IMalloc * pMalloc,
		bool bThrowException = false
		);
	static bool InstanceSave(
		CExtGridCell & cell,
		CArchive & ar,
		bool bThrowException = false
		);

	virtual bool OnPaintBackground(
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
		) const;
	virtual void OnPaintForeground(
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
		) const;
	virtual void OnPaintText(
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
		) const;
	virtual HFONT OnQueryCellFont(
		const CExtGridWnd & wndGrid,
		LONG nVisibleColNo,
		LONG nVisibleRowNo,
		LONG nColNo,
		LONG nRowNo,
		INT nColType,
		INT nRowType,
		DWORD dwAreaFlags
		) const;
	virtual CSize OnMeasureTextSize(
		const CExtGridWnd & wndGrid,
		CDC & dc,
		LONG nVisibleColNo,
		LONG nVisibleRowNo,
		LONG nColNo,
		LONG nRowNo,
		INT nColType,
		INT nRowType,
		DWORD dwAreaFlags
		) const;
	virtual void OnPaintSortArrow(
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
		) const;
	enum e_button_type_t
	{
		__EBTT_ELLIPSIS = 0,
		__EBTT_DROPDOWN = 1,
		__EBTT_UPDOWN_UP = 2,
		__EBTT_UPDOWN_DOWN = 3,
	};
	bool IsButtonEnabled(
		INT nButtonType // e_button_type_t
		) const;
	bool IsButtonPressed(
		INT nButtonType // e_button_type_t
		) const;
	bool IsButtonStayPressed(
		INT nButtonType // e_button_type_t
		) const;
	virtual void OnPaintButton(
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
		) const;
	virtual void OnPaintIcon(
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
		) const;
	virtual void OnPaintCheck(
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
		) const;
	virtual CSize OnCalcCheckSize() const;
	virtual void OnCalcLayout(
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
		) const;
	virtual CSize OnCalcSortArrowSize(
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
		) const;
	virtual CSize OnCalcFocusArrowSize(
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
		) const;
	virtual INT OnCalcButtonExtent(
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
		) const;
	virtual void OnFormatHeaderNumberString(
		CExtSafeString & strText,
		const CExtGridWnd & wndGrid,
		LONG nVisibleColNo,
		LONG nVisibleRowNo,
		LONG nColNo,
		LONG nRowNo,
		INT nColType,
		INT nRowType,
		DWORD dwAreaFlags
		) const;
	virtual COLORREF OnQueryTextColor(
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
		) const;
	virtual UINT OnQueryDrawTextFlags(
		LONG nVisibleColNo,
		LONG nVisibleRowNo,
		LONG nColNo,
		LONG nRowNo,
		INT nColType,
		INT nRowType,
		DWORD dwAreaFlags,
		bool bIncludeHorizontalFlags = true,
		bool bIncludeVerticalFlags = true,
		bool bIncludeOtherFlags = true
		) const;
	virtual void OnHitTestInfoAdjust(
		const CExtGridWnd & wndGrid,
		CExtGridHitTestInfo & htInfo,
		bool bComputeOuterDropAfterState
		) const;
	virtual void OnHover(
		CExtGridWnd & wndGrid,
		const CExtGridHitTestInfo & htInfo,
		bool bHoverEnter,
		bool bHoverLeave
		);
	virtual bool OnClick(
		CExtGridWnd & wndGrid,
		const CExtGridHitTestInfo & htInfo,
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nRepCnt, // 0 - button up, 1 - signle click, 2 - double click, 3 - post single click & begin editing
		UINT nFlags // mouse event flags
		);
	virtual void OnSetCheck(
		bool bCheck,
		CExtGridWnd & wndGrid,
		const RECT & rcCellExtra,
		const RECT & rcCell
		);
	virtual bool OnQueryButtonInfo(
		INT nButtonType,
		bool * p_bEnabled,
		bool * p_bPressed = NULL,
		bool * p_bStayPressed = NULL,
		UINT * p_nTimerElapseValue = NULL
		) const;
	virtual void OnButtonPressing(
		CExtGridWnd & wndGrid,
		INT nButtonType,
		bool bMouseInsideButton,
		ULONG nPressedTimerCounter,
		const RECT & rcButton,
		const RECT & rcCellExtra,
		const RECT & rcCell
		);
	virtual void OnButtonPressed(
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
		);
	virtual void OnChangeButtonPressedState(
		INT nButtonType,
		bool bPressed
		);
	virtual bool OnQueryEnabledIncrement() const;
	virtual bool OnQueryEnabledDecrement() const;
	virtual bool OnValueIncrement();
	virtual bool OnValueDecrement();
	virtual bool OnKey(
		CExtGridWnd & wndGrid,
		bool bKeyDownEvent, // true - key-down event, false - key-up event
		UINT nChar, // virtual key code
		UINT nRepCnt, // key-down/key-up press count
		UINT nFlags // key-down/key-up event flags
		);
	virtual bool OnSetCursor(
		CExtGridWnd & wndGrid,
		const CExtGridHitTestInfo & htInfo
		);
	virtual bool OnQueryExtent(
		INT & nItemExtent,
		bool bGet,
		INT nExtentType = 0 // -1 - min, 0 - curr, 1 - max
		);
	bool ExtentGet(
		INT & nItemExtent,
		INT nExtentType = 0 // -1 - min, 0 - curr, 1 - max
		) const;
	bool ExtentSet(
		INT nItemExtent,
		INT nExtentType = 0 // -1 - min, 0 - curr, 1 - max
		);
	virtual bool OnQueryExtraSpace(
		INT & nItemExtraSpace,
		bool bGet,
		bool bAfter // true - extra space after, false - after before
		);
	bool ExtraSpaceGet(
		INT & nItemExtraSpace,
		bool bAfter // true - extra space after, false - after before
		) const;
	bool ExtraSpaceSet(
		INT nItemExtraSpace,
		bool bAfter // true - extra space after, false - after before
		);
	virtual bool OnInitToolTip(
		CExtGridWnd & wndGrid,
		const CExtGridHitTestInfo & htInfo,
		CToolTipCtrl & wndToolTip,
		UINT nToolNo,
		const RECT & rcTool
		);
	virtual bool OnInitExpandWnd(
		CExtGridWnd & wndGrid,
		const CExtGridHitTestInfo & htInfo,
		CExtContentExpandWnd & wndContentExpand,
		const RECT & rcExpand,
		INT nSizeOfExpandShadow
		);
	virtual bool OnPaintExpandedContent(
		CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA & picd,
		const CExtGridWnd & wndGrid,
		const CExtGridHitTestInfo & htInfo,
		CExtContentExpandWnd & wndContentExpand,
		INT nSizeOfExpandShadow
		) const;
	virtual INT IconIndexGet() const;
	virtual INT IconIndexSet(
		INT nIconIndex = -1 // -1 - remove icon
		);
	virtual CExtCmdIcon * IconGet();
	const CExtCmdIcon * IconGet() const;
	virtual CSize IconGetSize() const;
	virtual HWND OnInplaceControlCreate(
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
		);
	virtual bool OnInplaceControlWindowProc(
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
		);
	virtual bool OnInplaceControlPreTranslateMessage(
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
		);
	virtual bool OnInplaceControlIdle(
		HWND hWndInplaceControl,
		CExtGridWnd & wndGrid,
		LONG nIdleCounter
		);
	virtual bool OnInplaceControlTextInputVerify(
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
		) const;
	virtual void OnInplaceControlTextInputComplete(
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
		);
	virtual HRESULT OnParseText(
		__EXT_MFC_SAFE_LPCTSTR sText
		) const;
	struct __PROF_UIS_API TrackCellStateInfo_t
	{
		CExtGridCell & m_cell;
		CExtGridWnd & m_wndGrid;
		INT m_nButtonType;
		CRect m_rcCellExtra, m_rcCell;
		LONG m_nVisibleColNo, m_nVisibleRowNo;
		LONG m_nColNo, m_nRowNo;
		INT m_nColType, m_nRowType;
		DWORD m_dwListBoxStyles;
		bool m_bHelperSelChanged:1;
		LONG m_nItemNoSelEndOK, m_nItemNoSelChange, m_nItemNoSelCancel;
		TrackCellStateInfo_t(
			CExtGridCell & cell,
			CExtGridWnd & wndGrid,
			INT nButtonType,
			const RECT & rcCellExtra,
			const RECT & rcCell,
			LONG nVisibleColNo,
			LONG nVisibleRowNo,
			LONG nColNo,
			LONG nRowNo,
			INT nColType,
			INT nRowType,
			DWORD dwListBoxStyles = WS_CHILD|WS_VISIBLE|WS_VSCROLL|LBS_NOINTEGRALHEIGHT|LBS_HASSTRINGS|LBS_OWNERDRAWVARIABLE
			)
			: m_cell( cell )
			, m_wndGrid( wndGrid )
			, m_nButtonType( nButtonType )
			, m_rcCellExtra( rcCellExtra )
			, m_rcCell( rcCell )
			, m_nVisibleColNo( nVisibleColNo )
			, m_nVisibleRowNo( nVisibleRowNo )
			, m_nColNo( nColNo )
			, m_nRowNo( nRowNo )
			, m_nColType( nColType )
			, m_nRowType( nRowType )
			, m_dwListBoxStyles( dwListBoxStyles )
			, m_bHelperSelChanged( false )
			, m_nItemNoSelEndOK( -1L )
			, m_nItemNoSelChange( -1L )
			, m_nItemNoSelCancel( -1L )
		{
		}
		~TrackCellStateInfo_t()
		{
		}
	};
	virtual bool OnButtonPopupMenuTrack(
		CExtGridCell::TrackCellStateInfo_t & _tcsi
		);
	virtual bool OnButtonPopupMenuComplete(
		CExtGridCell::TrackCellStateInfo_t & _tcsi
		);
	static bool stat_CbInitListBoxContent(
		CExtPopupInplaceListBox & wndListBox,
		CExtGridCell::TrackCellStateInfo_t & _tcsi
		);
	static bool stat_CbListBoxSelection(
		CExtPopupInplaceListBox & wndListBox,
		CExtGridCell::TrackCellStateInfo_t & _tcsi,
		int eSAT // CExtPopupInplaceListBox::e_sel_action_t
		);
	static bool stat_CbListBoxItemDraw(
		CExtPopupInplaceListBox & wndListBox,
		CExtGridCell::TrackCellStateInfo_t & _tcsi,
		LPDRAWITEMSTRUCT pDIS
		);
	static bool stat_CbListBoxItemMeasure(
		CExtPopupInplaceListBox & wndListBox,
		CExtGridCell::TrackCellStateInfo_t & _tcsi,
		LPMEASUREITEMSTRUCT pMIS
		);
	virtual CSize OnPopupListBoxCalcItemExtraSizes(
		CExtGridCell::TrackCellStateInfo_t & _tcsi,
		LONG nItemNo
		) const;
	virtual CSize OnPopupListBoxMeasureTrackSize(
		CExtGridCell::TrackCellStateInfo_t & _tcsi,
		CExtPopupListBoxMenuWnd * pPopup
		);
	virtual bool OnPopupListBoxInitContent(
		CExtPopupInplaceListBox & wndListBox,
		CExtGridCell::TrackCellStateInfo_t & _tcsi
		);
	virtual bool OnPopupListBoxSelChange(
		CExtPopupInplaceListBox & wndListBox,
		CExtGridCell::TrackCellStateInfo_t & _tcsi
		);
	virtual bool OnPopupListBoxSelEndOK(
		CExtPopupInplaceListBox & wndListBox,
		CExtGridCell::TrackCellStateInfo_t & _tcsi
		);
	virtual bool OnPopupListBoxSelEndCancel(
		CExtPopupInplaceListBox & wndListBox,
		CExtGridCell::TrackCellStateInfo_t & _tcsi
		);
	virtual bool OnPopupListBoxItemDraw(
		CExtPopupInplaceListBox & wndListBox,
		CExtGridCell::TrackCellStateInfo_t & _tcsi,
		LPDRAWITEMSTRUCT pDIS
		);
	virtual bool OnPopupListBoxItemMeasure(
		CExtPopupInplaceListBox & wndListBox,
		CExtGridCell::TrackCellStateInfo_t & _tcsi,
		LPMEASUREITEMSTRUCT pMIS
		);
	virtual bool OnCellPressingEnter(
		CExtGridWnd & wndGrid,
		const CExtGridHitTestInfo & htInfo,
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nFlags // mouse event flags
		);
	virtual void OnCellPressingStateChanged(
		CExtGridWnd & wndGrid,
		const CExtGridHitTestInfo & htInfo,
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nFlags, // mouse event flags
		bool bMouseInsideCell
		);
	virtual bool OnCellPressingLeave(
		CExtGridWnd & wndGrid,
		const CExtGridHitTestInfo & htInfo,
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nFlags, // mouse event flags
		bool bPressedEvent,
		bool bStartDragEvent
		);
	virtual bool OnOuterDndTrack(
		CExtGridWnd & wndGrid,
		const CExtGridHitTestInfo & htInfo,
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nFlags // mouse event flags
		);
	virtual bool OnOuterDndComplete(
		CExtGridWnd & wndGrid,
		bool bDraggedCell, // true - dragged cell, false - drop-target cell
		const CExtGridHitTestInfo & htInfoDrag,
		const CExtGridHitTestInfo & htInfoDrop
		);
	virtual bool OnOuterDndOut(
		CExtGridWnd & wndGrid,
		const CExtGridHitTestInfo & htInfo
		);
}; // class CExtGridCell

#define IMPLEMENT_ExtGridCell_Clone(__derived__,__base__) \
	virtual CExtGridCell * Clone( \
		IMalloc * pMalloc = NULL \
		) const \
	{ \
		ASSERT_VALID( this ); \
		if( pMalloc == NULL ) \
			return __base__ :: Clone( pMalloc ); \
		CExtGridCell * pOther = \
				new ( pMalloc, false ) __derived__ \
				(	(	const_cast \
						< __derived__ * > \
						( this ) \
					) \
					-> DataProviderGet() \
				); \
		ASSERT( pOther != NULL ); \
		if( pOther != NULL ) \
		{ \
			ASSERT_VALID( pOther ); \
			pOther->Assign( *this ); \
			ASSERT( pOther->DataProviderGet() == DataProviderGet() ); \
		} \
		return pOther; \
	}

/////////////////////////////////////////////////////////////////////////////
// CExtGridInplaceEdit

class __PROF_UIS_API CExtGridInplaceEdit : public CEdit
{
protected:
	CExtGridWnd & m_wndGrid;
	CExtGridCell & m_cell;
	LONG m_nVisibleColNo, m_nVisibleRowNo,
		m_nColNo, m_nRowNo,
		m_nColType, m_nRowType,
		m_nLastEditedColNo, m_nLastEditedRowNo;
	CRect m_rcCellExtra, m_rcCell, m_rcInplaceControl, m_rcNonClientMargins;
	CFont m_font;
	bool m_bInEndEdit:1, m_bInValidation:1, m_bInConstruction:1;
	CExtSafeString m_strTextPrevious, m_strTextInitial;
public:
	DECLARE_DYNAMIC( CExtGridInplaceEdit );
	CExtGridInplaceEdit(
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
		);
	~CExtGridInplaceEdit();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext & dc ) const;
#endif // _DEBUG
	virtual bool Create();
	virtual BOOL PreTranslateMessage( MSG * pMsg );
	virtual void PostNcDestroy();
protected:
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	void _DoEndEdit( bool bSaveChanges );
	bool _DoVerifyTextInput( bool bEndEdit );
public:
	virtual void DoEndEdit( bool bSaveChanges );
	virtual bool DoVerifyTextInput(
		__EXT_MFC_SAFE_LPCTSTR sTextInitial,
		__EXT_MFC_SAFE_LPCTSTR sTextPrevious,
		CExtSafeString & sTextNew,
		bool bEndEdit
		);
}; // class __PROF_UIS_API CExtGridInplaceEdit

/////////////////////////////////////////////////////////////////////////////
// CExtGridCellStringDM

class __PROF_UIS_API CExtGridCellStringDM : public CExtGridCell
{
	void _StringBufferFree();
	void _StringBufferSet( __EXT_MFC_SAFE_LPCTSTR strBuf );
public:
	DECLARE_DYNCREATE( CExtGridCellStringDM );
	IMPLEMENT_ExtGridCell_Clone( CExtGridCellStringDM, CExtGridCell );
	__EXT_MFC_SAFE_LPTSTR m_strData;
	CExtGridCellStringDM(
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellStringDM( CExtGridCellStringDM & other );
	CExtGridCellStringDM( CExtGridCell & other );
	virtual ~CExtGridCellStringDM();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext & dc ) const;
#endif // _DEBUG
	virtual void Serialize( CArchive & ar );
	virtual bool IsEmpty() const;
	virtual void Empty();
	virtual void Assign( const CExtGridCell & other );
	virtual void TextGet( CExtSafeString & strCopy ) const;
	virtual void TextSet(
		__EXT_MFC_SAFE_LPCTSTR str = __EXT_MFC_SAFE_LPCTSTR(NULL), // empty text
		bool bAllowChangeDataType = false
		);
	virtual void GetVariant( VARIANT & varCopy ) const;
	virtual __EXT_MFC_SAFE_LPCTSTR GetTextBuffer() const;
}; // class CExtGridCellStringDM

/////////////////////////////////////////////////////////////////////////////
// CExtGridCellStringTI
// template for internal static text buffer cell type
// nCTBS - cell text buffer size (must be > 1)

template < UINT nCTBS = 64 >
class CExtGridCellStringTI : public CExtGridCell
{
public:
	BYTE m_strData[ sizeof(TCHAR)*nCTBS ];
	CExtGridCellStringTI(
		CExtGridDataProvider * pDataProvider = NULL
		)
		: CExtGridCell( pDataProvider )
	{
		ASSERT( nCTBS > 1 );
		::memset( m_strData, 0, sizeof(TCHAR)*nCTBS );
	}
	CExtGridCellStringTI( CExtGridCellStringTI & other )
	{
		ASSERT( nCTBS > 1 );
		(*this) = other;
	}
private:
	__EXT_MFC_SAFE_LPCTSTR _GetBufferPtr() const
	{
		ASSERT( nCTBS > 1 );
		return __EXT_MFC_SAFE_LPCTSTR( (LPCTSTR)(LPVOID)m_strData );
	}
	__EXT_MFC_SAFE_LPTSTR _GetBufferPtr()
	{
		ASSERT( nCTBS > 1 );
		return __EXT_MFC_SAFE_LPTSTR( (LPTSTR)(LPVOID)m_strData );
	}
public:
	virtual void Serialize( CArchive & ar )
	{
		ASSERT_VALID( this );
		ASSERT( nCTBS > 1 );
		CExtGridCell::Serialize( ar );
		if( ar.IsStoring() )
		{
			CExtSafeString sTmp( _GetBufferPtr() );
			ar << sTmp;
		} // if( ar.IsStoring() )
		else
		{
			::memset( m_strData, 0, sizeof(TCHAR)*nCTBS );
			CExtSafeString sTmp;
			ar >> sTmp;
			if( !sTmp.IsEmpty() )
				_tcsncpy(
					_GetBufferPtr(),
					sTmp,
					nCTBS - 1;
					);
		} // else from if( ar.IsStoring() )
	}
	virtual bool IsEmpty() const
	{
		ASSERT_VALID( this );
		ASSERT( nCTBS > 1 );
		if( (_GetBufferPtr()[0]) != _T('\0') )
			return false;
		return CExtGridCell::IsEmpty();
	}
	virtual void Empty()
	{
		ASSERT_VALID( this );
		ASSERT( nCTBS > 1 );
		CExtGridCell::Empty();
		::memset( m_strData, 0, sizeof(TCHAR)*nCTBS );
	}
	virtual void Assign( const CExtGridCell & other )
	{
		ASSERT_VALID( this );
		ASSERT( nCTBS > 1 );
		::memset( m_strData, 0, sizeof(TCHAR)*nCTBS );
		CExtSafeString sTmp;
		other.TextGet( sTmp );
		_tcsncpy(
			_GetBufferPtr(),
			sTmp,
			nCTBS - 1;
			);
	}
	virtual void TextGet( CExtSafeString & strCopy ) const
	{
		ASSERT_VALID( this );
		ASSERT( nCTBS > 1 );
		strCopy = _GetBufferPtr();
	}
	virtual void TextSet(
		__EXT_MFC_SAFE_LPCTSTR str = __EXT_MFC_SAFE_LPCTSTR(NULL), // empty text
		bool bAllowChangeDataType = false
		)
	{
		ASSERT_VALID( this );
		ASSERT( nCTBS > 1 );
		LPCTSTR strNew = LPCTSTR(str);
		LPCTSTR strOld = _GetBufferPtr();
		if( strNew == strOld )
			return;
		::memset( m_strData, 0, sizeof(TCHAR)*nCTBS );
		if( strNew[0] == _T('\0') )
			return;
		_tcsncpy(
			_GetBufferPtr(),
			strNew,
			nCTBS - 1;
			);
	}
	virtual void GetVariant( VARIANT & varCopy ) const
	{
		ASSERT_VALID( this );
		ASSERT( nCTBS > 1 );
		::VariantClear( &varCopy );
		USES_CONVERSION;
		BSTR bstrCopy = ::SysAllocString( T2OLE( _GetBufferPtr() ) );
		if( bstrCopy == NULL )
			return;
		varCopy.vt = VT_BSTR;
		varCopy.bstrVal = bstrCopy;
	}
	virtual __EXT_MFC_SAFE_LPCTSTR GetTextBuffer() const
	{
		ASSERT_VALID( this );
		ASSERT( nCTBS > 1 );
		return _GetBufferPtr();
	}
	virtual CExtGridCell * Clone(
		IMalloc * pMalloc = NULL
		) const
	{
		ASSERT_VALID( this );
		ASSERT( nCTBS > 1 );
		if( pMalloc == NULL )
		{
			try
			{
				CExtGridCell * pOther =
					new CExtGridCellStringTI < nCTBS >
						(	(	const_cast
								< CExtGridCellStringTI < nCTBS > * >
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
			new (pMalloc, false) CExtGridCellStringTI < nCTBS >
				(	(	const_cast
						< CExtGridCellStringTI < nCTBS > * >
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
}; // class CExtGridCellStringTI

/////////////////////////////////////////////////////////////////////////////
// CExtGridCellStringTE
// template for external static text buffer cell type
// p_CTBuff - pointer to external text buffer (must be not NULL)
// nCTBS - cell text buffer size (must be > 1)

template < LPVOID p_CTBuff, UINT nCTBS = 64 >
class CExtGridCellStringTE : public CExtGridCell
{
public:
	CExtGridCellStringTE(
		CExtGridDataProvider * pDataProvider = NULL
		)
		: CExtGridCell( pDataProvider )
	{
		ASSERT( p_CTBuff != NULL );
		ASSERT( nCTBS > 1 );
		::memset( p_CTBuff, 0, sizeof(TCHAR)*nCTBS );
	}
	CExtGridCellStringTE( CExtGridCellStringTE & other )
	{
		ASSERT( p_CTBuff != NULL );
		ASSERT( nCTBS > 1 );
		(*this) = other;
	}
private:
	__EXT_MFC_SAFE_LPCTSTR _GetBufferPtr() const
	{
		ASSERT( p_CTBuff != NULL );
		ASSERT( nCTBS > 1 );
		return __EXT_MFC_SAFE_LPCTSTR( (LPCTSTR)p_CTBuff );
	}
	__EXT_MFC_SAFE_LPTSTR _GetBufferPtr()
	{
		ASSERT( p_CTBuff != NULL );
		ASSERT( nCTBS > 1 );
		return __EXT_MFC_SAFE_LPTSTR( (LPTSTR)p_CTBuff );
	}
public:
	virtual void Serialize( CArchive & ar )
	{
		ASSERT_VALID( this );
		ASSERT( p_CTBuff != NULL );
		ASSERT( nCTBS > 1 );
		CExtGridCell::Serialize( ar );
		if( ar.IsStoring() )
		{
			CExtSafeString sTmp( _GetBufferPtr() );
			ar << sTmp;
		} // if( ar.IsStoring() )
		else
		{
			::memset( p_CTBuff, 0, sizeof(TCHAR)*nCTBS );
			CExtSafeString sTmp;
			ar >> sTmp;
			if( !sTmp.IsEmpty() )
				_tcsncpy(
					_GetBufferPtr(),
					sTmp,
					nCTBS - 1;
					);
		} // else from if( ar.IsStoring() )
	}
	virtual bool IsEmpty() const
	{
		ASSERT_VALID( this );
		ASSERT( p_CTBuff != NULL );
		ASSERT( nCTBS > 1 );
		if( (_GetBufferPtr()[0]) != _T('\0') )
			return false;
		return CExtGridCell::IsEmpty();
	}
	virtual void Empty()
	{
		ASSERT_VALID( this );
		ASSERT( p_CTBuff != NULL );
		ASSERT( nCTBS > 1 );
		CExtGridCell::Empty();
		::memset( p_CTBuff, 0, sizeof(TCHAR)*nCTBS );
	}
	virtual void Assign( const CExtGridCell & other )
	{
		ASSERT_VALID( this );
		ASSERT( p_CTBuff != NULL );
		ASSERT( nCTBS > 1 );
		::memset( p_CTBuff, 0, sizeof(TCHAR)*nCTBS );
		CExtSafeString sTmp;
		other.TextGet( sTmp );
		_tcsncpy(
			_GetBufferPtr(),
			sTmp,
			nCTBS - 1;
			);
	}
	virtual void TextGet( CExtSafeString & strCopy ) const
	{
		ASSERT_VALID( this );
		ASSERT( p_CTBuff != NULL );
		ASSERT( nCTBS > 1 );
		strCopy = _GetBufferPtr();
	}
	virtual void TextSet(
		__EXT_MFC_SAFE_LPCTSTR str = __EXT_MFC_SAFE_LPCTSTR(NULL), // empty text
		bool bAllowChangeDataType = false
		)
	{
		ASSERT_VALID( this );
		ASSERT( p_CTBuff != NULL );
		ASSERT( nCTBS > 1 );
		LPCTSTR strNew = LPCTSTR(str);
		LPCTSTR strOld = _GetBufferPtr();
		if( strNew == strOld )
			return;
		::memset( p_CTBuff, 0, sizeof(TCHAR)*nCTBS );
		if( strNew[0] == _T('\0') )
			return;
		_tcsncpy(
			_GetBufferPtr(),
			strNew,
			nCTBS - 1;
			);
	}
	virtual void GetVariant( VARIANT & varCopy ) const
	{
		ASSERT_VALID( this );
		ASSERT( p_CTBuff != NULL );
		ASSERT( nCTBS > 1 );
		::VariantClear( &varCopy );
		USES_CONVERSION;
		BSTR bstrCopy = ::SysAllocString( T2OLE( _GetBufferPtr() ) );
		if( bstrCopy == NULL )
			return;
		varCopy.vt = VT_BSTR;
		varCopy.bstrVal = bstrCopy;
	}
	virtual __EXT_MFC_SAFE_LPCTSTR GetTextBuffer() const
	{
		ASSERT_VALID( this );
		ASSERT( p_CTBuff != NULL );
		ASSERT( nCTBS > 1 );
		return _GetBufferPtr();
	}
	virtual CExtGridCell * Clone(
		IMalloc * pMalloc = NULL
		) const
	{
		ASSERT_VALID( this );
		ASSERT( nCTBS > 1 );
		if( pMalloc == NULL )
		{
			try
			{
				CExtGridCell * pOther =
					new CExtGridCellStringTE < p_CTBuff, nCTBS >
						(	(	const_cast
								< CExtGridCellStringTE < p_CTBuff, nCTBS > * >
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
			new (pMalloc, false) CExtGridCellStringTE < p_CTBuff, nCTBS >
				( ( ( const_cast < CExtGridCellStringTE < p_CTBuff, nCTBS > * > ( this ) ) -> DataProviderGet() ) );
		ASSERT( pOther != NULL );
		if( pOther != NULL )
		{
			ASSERT_VALID( pOther );
			pOther->Assign( *this );
		} // if( pOther != NULL )
		return pOther;
	}
}; // class CExtGridCellStringTE

/////////////////////////////////////////////////////////////////////////////
// CExtGCE
// template for adding row/column extent and extra-space
// information to other cell types (used for header cells)

template < class _BC >
class CExtGCE : public _BC
{
	INT m_nItemExtentCurrent, m_nItemExtentMin, m_nItemExtentMax,
		m_nExtraSpaceAfter, m_nExtraSpaceBefore;
public:
	CExtGCE(
		CExtGridDataProvider * pDataProvider = NULL,
		INT nItemExtentCurrent = GetSystemMetrics(SM_CYHSCROLL)+1,
		INT nItemExtentMin = 0,
		INT nItemExtentMax = 32767,
		INT nExtraSpaceAfter = 0,
		INT nExtraSpaceBefore = 0
		)
		: _BC( pDataProvider )
		, m_nItemExtentCurrent( nItemExtentCurrent )
		, m_nItemExtentMin( nItemExtentMin )
		, m_nItemExtentMax( nItemExtentMax )
		, m_nExtraSpaceAfter( nExtraSpaceAfter )
		, m_nExtraSpaceBefore( nExtraSpaceBefore )
	{
		ASSERT_VALID( this );
	}
	CExtGCE( const CExtGridCell & other )
		: m_nItemExtentCurrent( 16 )
		, m_nItemExtentMin( 0 )
		, m_nItemExtentMax( 32767 )
		, m_nExtraSpaceAfter( 0 )
		, m_nExtraSpaceBefore( 0 )
	{
		Assign( other );
	}
#ifdef _DEBUG
	virtual void AssertValid() const
	{
		ASSERT( this != NULL );
		ASSERT( IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
		_BC::AssertValid();
		ASSERT(
				m_nItemExtentCurrent >= 0
			&&	m_nItemExtentMin >= 0
			&&	m_nItemExtentMax >= 0
			&&	m_nItemExtentMin <= m_nItemExtentMax
			&&	m_nItemExtentMin <= m_nItemExtentCurrent
			&&	m_nItemExtentCurrent <= m_nItemExtentMax
			&&	m_nExtraSpaceAfter >= 0
			&&	m_nExtraSpaceBefore >= 0
			);
	}
#endif // _DEBUG
	virtual void Assign( const CExtGridCell & other )
	{
		ASSERT_VALID( this );
		_BC::Assign( other );
		INT nItemExtentCurrent, nItemExtentMin, nItemExtentMax,
			nExtraSpaceAfter, nExtraSpaceBefore;
		if(		other.ExtentGet( nItemExtentMin, -1 )
			&&	other.ExtentGet( nItemExtentCurrent, 0 )
			&&	other.ExtentGet( nItemExtentMax, 1 )
			)
		{
			ASSERT(
					nItemExtentCurrent >= 0
				&&	nItemExtentMin >= 0
				&&	nItemExtentMax >= 0
				&&	nItemExtentMin <= nItemExtentMax
				&&	nItemExtentMin <= nItemExtentCurrent
				&&	nItemExtentCurrent <= nItemExtentMax
				);
			VERIFY( ExtentSet( nItemExtentMin, -1 ) );
			VERIFY( ExtentSet( nItemExtentCurrent, 0 ) );
			VERIFY( ExtentSet( nItemExtentMax, 1 ) );
		}
		if( other.ExtraSpaceGet( nExtraSpaceAfter, true ) )
			ExtraSpaceSet( nExtraSpaceAfter, true );
		if( other.ExtraSpaceGet( nExtraSpaceBefore, false ) )
			ExtraSpaceSet( nExtraSpaceBefore, false );
	}
	virtual CExtGridCell * Clone(
		IMalloc * pMalloc = NULL
		) const
	{
		ASSERT_VALID( this );
		if( pMalloc == NULL )
		{
			try
			{
				CExtGridCell * pOther =
					new CExtGCE < _BC >
						(	(	const_cast
								< CExtGCE < _BC > * >
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
			new (pMalloc, false) CExtGCE < _BC >
				(	(	const_cast
						< CExtGCE < _BC > * >
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
	virtual void Serialize( CArchive & ar )
	{
		ASSERT_VALID( this );
		_BC::Serialize( ar );
		INT nItemExtentCurrent, nItemExtentMin, nItemExtentMax,
			nExtraSpaceAfter, nExtraSpaceBefore;
		if( ar.IsStoring() )
		{
			VERIFY( ExtentGet( nItemExtentMin, -1 ) );
			VERIFY( ExtentGet( nItemExtentCurrent, 0 ) );
			VERIFY( ExtentGet( nItemExtentMax, 1 ) );
			VERIFY( ExtraSpaceGet( nExtraSpaceAfter, true ) );
			VERIFY( ExtraSpaceGet( nExtraSpaceBefore, false ) );
			ASSERT(
					nItemExtentCurrent >= 0
				&&	nItemExtentMin >= 0
				&&	nItemExtentMax >= 0
				&&	nItemExtentMin <= nItemExtentMax
				&&	nItemExtentMin <= nItemExtentCurrent
				&&	nItemExtentCurrent <= nItemExtentMax
				&&	nExtraSpaceAfter >= 0
				&&	nExtraSpaceBefore >= 0
				);
			ar << nItemExtentCurrent;
			ar << nItemExtentMin;
			ar << nItemExtentMax;
			ar << nExtraSpaceAfter;
			ar << nExtraSpaceBefore;
		} // if( ar.IsStoring() )
		else
		{
			ar >> nItemExtentCurrent;
			ar >> nItemExtentMin;
			ar >> nItemExtentMax;
			ar >> nExtraSpaceAfter;
			ar >> nExtraSpaceBefore;
			ASSERT(
					nItemExtentCurrent >= 0
				&&	nItemExtentMin >= 0
				&&	nItemExtentMax >= 0
				&&	nItemExtentMin <= nItemExtentMax
				&&	nItemExtentMin <= nItemExtentCurrent
				&&	nItemExtentCurrent <= nItemExtentMax
				&&	nExtraSpaceAfter >= 0
				&&	nExtraSpaceBefore >= 0
				);
			VERIFY( ExtentSet( nItemExtentMin, -1 ) );
			VERIFY( ExtentSet( nItemExtentCurrent, 0 ) );
			VERIFY( ExtentSet( nItemExtentMax, 1 ) );
			VERIFY( ExtraSpaceSet( nExtraSpaceAfter, true ) );
			VERIFY( ExtraSpaceSet( nExtraSpaceBefore, false ) );
		} // else from if( ar.IsStoring() )
	}
	virtual bool OnQueryExtent(
		INT & nItemExtent,
		bool bGet,
		INT nExtentType = 0 // -1 - min, 0 - curr, 1 - max
		)
	{
		ASSERT( this != NULL );
		ASSERT( IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
		if( bGet )
		{
			if( nExtentType < 0 )
				nItemExtent = m_nItemExtentMin;
			else if( nExtentType > 0 )
				nItemExtent = m_nItemExtentMax;
			else
				nItemExtent = m_nItemExtentCurrent;
		} // if( bGet )
		else
		{
			if( nExtentType < 0 )
				m_nItemExtentMin = nItemExtent;
			else if( nExtentType > 0 )
				m_nItemExtentMax = nItemExtent;
			else
				m_nItemExtentCurrent = nItemExtent;
		} // else from if( bGet )
		return true;
	}
	virtual bool OnQueryExtraSpace(
		INT & nItemExtraSpace,
		bool bGet,
		bool bAfter // true - extra space after, false - after before
		)
	{
		ASSERT( this != NULL );
		ASSERT( IsKindOf(RUNTIME_CLASS(CExtGridCell)) );
		if( bGet )
		{
			if( bAfter )
				nItemExtraSpace = m_nExtraSpaceAfter;
			else
				nItemExtraSpace = m_nExtraSpaceBefore;
		} // if( bGet )
		else
		{
			if( bAfter )
				m_nExtraSpaceAfter = nItemExtraSpace;
			else
				m_nExtraSpaceBefore = nItemExtraSpace;
		} // else from if( bGet )
		return true;
	}
}; // class CExtGCE

/////////////////////////////////////////////////////////////////////////////
// CExtGridCellHeader

class __PROF_UIS_API CExtGridCellHeader
	: public CExtGCE < CExtGridCellStringDM >
{
public:
	DECLARE_DYNCREATE( CExtGridCellHeader );
	IMPLEMENT_ExtGridCell_Clone( CExtGridCellHeader, CExtGCE < CExtGridCellStringDM > );
	CExtGridCellHeader(
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellHeader( CExtGridCell & other );
}; // class CExtGridCellHeader

/////////////////////////////////////////////////////////////////////////////
// CExtGridCellColor

class __PROF_UIS_API CExtGridCellColor : public CExtGridCell
{
public:
	class __PROF_UIS_API CPackedColor
	{
	public:
		BYTE m_arrColorBytes[4];
		enum e_PackedFlags_t
		{
			__EPFT_DEFAULT_FLAGS = BYTE(0x00),
			__EPFT_SYSCOLOR = BYTE(0x01),
		};
		CPackedColor()
		{
			Clear();
		}
		CPackedColor( COLORREF clr )
		{
			Clear();
			SetColor( clr );
		}
		CPackedColor( INT nSysColorIdx )
		{
			Clear();
			SetColor( nSysColorIdx );
		}
		CPackedColor( const CPackedColor & other )
		{
			(*this) = other;
		}
		~CPackedColor()
		{
		}
		CPackedColor & operator =( const CPackedColor & other )
		{
			m_arrColorBytes[0] = other.m_arrColorBytes[0];
			m_arrColorBytes[1] = other.m_arrColorBytes[1];
			m_arrColorBytes[2] = other.m_arrColorBytes[2];
			m_arrColorBytes[3] = other.m_arrColorBytes[3];
			return (*this);
		}
		bool operator ==( const CPackedColor & other ) const
		{
			if(		m_arrColorBytes[0] == other.m_arrColorBytes[0]
				&&	m_arrColorBytes[1] == other.m_arrColorBytes[1]
				&&	m_arrColorBytes[2] == other.m_arrColorBytes[2]
				&&	m_arrColorBytes[3] == other.m_arrColorBytes[3]
				)
				return true;
			return false;
		}
		bool operator !=( const CPackedColor & other ) const
		{
			return ( (*this) == other ) ? false : true;
		}
		void Clear()
		{
			m_arrColorBytes[0] = BYTE(0);
			m_arrColorBytes[1] = BYTE(0);
			m_arrColorBytes[2] = BYTE(0);
			m_arrColorBytes[3] = __EPFT_DEFAULT_FLAGS;
		}
		bool IsSysColor() const
		{
			if( m_arrColorBytes[3] )
				return true;
			return false;
		}
		COLORREF GetRefColor() const
		{
			COLORREF clr = RGB( m_arrColorBytes[2], m_arrColorBytes[1], m_arrColorBytes[0] );
			return clr;
		}
		INT GetSysColorIdx() const
		{
			INT nSysColorIdx = (INT)GetRefColor();
			return nSysColorIdx;
		}
		COLORREF GetColor() const
		{
			COLORREF clr = GetRefColor();
			if( ! IsSysColor() )
				return clr;
			INT nSysColorIdx = INT(clr);
			clr = ::GetSysColor( nSysColorIdx );
			return clr;
		}
		void SetColor( COLORREF clr )
		{
			m_arrColorBytes[0] = GetBValue( clr );
			m_arrColorBytes[1] = GetGValue( clr );
			m_arrColorBytes[2] = GetRValue( clr );
			m_arrColorBytes[3] &= ~(__EPFT_SYSCOLOR);
		}
		void SetColor( INT nSysColorIdx )
		{
			m_arrColorBytes[0] = GetBValue( nSysColorIdx );
			m_arrColorBytes[1] = GetGValue( nSysColorIdx );
			m_arrColorBytes[2] = GetRValue( nSysColorIdx );
			m_arrColorBytes[3] |= __EPFT_SYSCOLOR;
		}
		void Serialize( CArchive & ar )
		{
			if( ar.IsStoring() )
				ar.Write( m_arrColorBytes, sizeof(m_arrColorBytes) );
			else
				ar.Read( m_arrColorBytes, sizeof(m_arrColorBytes) );
		}
		static COLORREF stat_GenRandomizedColor(
			DWORD nRedMin = DWORD(0),
			DWORD nRedMax = DWORD(255),
			DWORD nGreenMin = DWORD(0),
			DWORD nGreenMax = DWORD(255),
			DWORD nBlueMin = DWORD(0),
			DWORD nBlueMax = DWORD(255)
			)
		{
			ASSERT( DWORD(0) <= nRedMin && nRedMin <= DWORD(255) );
			ASSERT( DWORD(0) <= nRedMax && nRedMax <= DWORD(255) );
			ASSERT( DWORD(0) <= nGreenMin && nGreenMin <= DWORD(255) );
			ASSERT( DWORD(0) <= nGreenMax && nGreenMax <= DWORD(255) );
			ASSERT( DWORD(0) <= nBlueMin && nBlueMin <= DWORD(255) );
			ASSERT( DWORD(0) <= nBlueMax && nBlueMax <= DWORD(255) );
			ASSERT( nRedMin <= nRedMax );
			ASSERT( nGreenMin <= nGreenMax );
			ASSERT( nBlueMin <= nBlueMax );
			DWORD nRed = DWORD(0);
			if( nRedMin != nRedMax )
			{
				DWORD nRedRange = nRedMax - nRedMin;
				nRed += DWORD(rand()) % nRedRange;
			} // if( nRedMin != nRedMax )
			nRed += nRedMin;
			ASSERT( DWORD(0) <= nRed && nRed <= DWORD(255) );
			ASSERT( nRedMin <= nRed && nRed <= nRedMax );
			DWORD nGreen = DWORD(0);
			if( nGreenMin != nGreenMax )
			{
				DWORD nGreenRange = nGreenMax - nGreenMin;
				nGreen += DWORD(rand()) % nGreenRange;
			} // if( nGreenMin != nGreenMax )
			nGreen += nGreenMin;
			ASSERT( DWORD(0) <= nGreen && nGreen <= DWORD(255) );
			ASSERT( nGreenMin <= nGreen && nGreen <= nGreenMax );
			DWORD nBlue = DWORD(0);
			if( nBlueMin != nBlueMax )
			{
				DWORD nBlueRange = nBlueMax - nBlueMin;
				nBlue += DWORD(rand()) % nBlueRange;
			} // if( nBlueMin != nBlueMax )
			nBlue += nBlueMin;
			ASSERT( DWORD(0) <= nBlue && nBlue <= DWORD(255) );
			ASSERT( nBlueMin <= nBlue && nBlue <= nBlueMax );
			COLORREF clr = RGB( nRed, nGreen, nBlue );
			return clr;
		}
		void SetRandomizedColor(
			DWORD nRedMin = DWORD(0),
			DWORD nRedMax = DWORD(255),
			DWORD nGreenMin = DWORD(0),
			DWORD nGreenMax = DWORD(255),
			DWORD nBlueMin = DWORD(0),
			DWORD nBlueMax = DWORD(255)
			)
		{
			SetColor(
				stat_GenRandomizedColor(
					nRedMin,
					nRedMax,
					nGreenMin,
					nGreenMax,
					nBlueMin,
					nBlueMax
					)
				);
		}
	}; // class CPackedColor
	CPackedColor m_PackedColor;
	CSize m_sizeIcon;
	bool m_bEnableButtonRandomColor:1, m_bEnableButtonCustomColor:1;
	DECLARE_DYNCREATE( CExtGridCellColor );
	IMPLEMENT_ExtGridCell_Clone( CExtGridCellColor, CExtGridCell );
	CExtGridCellColor(
		CExtGridDataProvider * pDataProvider = NULL,
		COLORREF clr = RGB(0,0,0),
		CSize sizeIcon = CSize( 32, 16 ),
		bool bEnableButtonRandomColor = false,
		bool bEnableButtonCustomColor = true
		);
	CExtGridCellColor(
		CExtGridDataProvider * pDataProvider,
		INT nSysColorIdx,
		CSize sizeIcon = CSize( 32, 16 ),
		bool bEnableButtonRandomColor = false,
		bool bEnableButtonCustomColor = true
		);
	CExtGridCellColor(
		CExtGridDataProvider * pDataProvider,
		CPackedColor & PackedColor,
		CSize sizeIcon = CSize( 32, 16 ),
		bool bEnableButtonRandomColor = false,
		bool bEnableButtonCustomColor = true
		);
	CExtGridCellColor( CExtGridCell & other );
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif // _DEBUG
	virtual void Assign( const CExtGridCell & other );
	virtual void Serialize( CArchive & ar );
	virtual CSize IconGetSize() const;
	virtual void OnPaintIcon(
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
		) const;
	virtual void OnButtonPressed(
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
		);
}; // class CExtGridCellColor

/////////////////////////////////////////////////////////////////////////////
// CExtGridCellVariant

class __PROF_UIS_API CExtGridCellVariant
	: public CExtGridCell
	, public VARIANT
{
public:
	DECLARE_DYNCREATE( CExtGridCellVariant );
	IMPLEMENT_ExtGridCell_Clone( CExtGridCellVariant, CExtGridCell );
	signed __int64 & _GetInt64Ref();
	unsigned __int64 & _GetUInt64Ref();
	signed __int64 *& _GetInt64PtrRef();
	unsigned __int64 *& _GetUInt64PtrRef();
protected:
	void _VariantInit();
public:
	bool _VariantClear(
		bool bConvertToErrorIfFailed = true
		);
	bool _VariantAssign(
		const VARIANT & other,
		bool bConvertToErrorIfFailed = true
		);
	void _VariantAssign(
		IDispatch * pInterface,
		bool bAllowNullInterface = true
		);
	void _VariantAssign(
		IUnknown * pInterface,
		bool bAllowNullInterface = true
		);
	void _VariantAssign(
		IDispatch ** ppInterface,
		bool bAllowNullInterface = true
		);
	void _VariantAssign(
		IUnknown ** ppInterface,
		bool bAllowNullInterface = true
		);
	void _VariantAssign(
		const SAFEARRAY * pSA,
		bool bAllowConvertToError = true
		);
	void _VariantAssign( const SAFEARRAY ** ppSA );
	void _VariantAssign( const DECIMAL & decimalVal );
	void _VariantAssign( const DECIMAL ** decimalVal );
//	bool _VariantAssign( BSTR bstrOther );
//	bool _VariantAssign( BSTR * bstrOther );
	bool _VariantAssign( const char * strOther );
	bool _VariantAssign( const unsigned short * strOther );
#if _MFC_VER >= 0x700
#if (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
	bool _VariantAssign( const __wchar_t * strOther );
#endif // (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
#endif // _MFC_VER >= 0x700
	void _VariantAssign( bool bSrc );
	void _VariantAssign( signed char nSrc );
	void _VariantAssign( signed char * nSrc );
	void _VariantAssign( unsigned char nSrc );
	void _VariantAssign( unsigned char * nSrc );
	void _VariantAssign( signed short nSrc );
	void _VariantAssign( signed short * nSrc );
	void _VariantAssign( unsigned short nSrc );
	void _VariantAssign(
		unsigned short * nSrc,
		VARTYPE nVariantType = VT_UI2 // VT_UI2 or VT_BSTR only (VT_BYREF for VT_UI2 will be added automatically)
		);
#if _MFC_VER >= 0x700
#if (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
	void _VariantAssign(
		__wchar_t * nSrc,
		VARTYPE nVariantType = VT_UI2 // VT_UI2 or VT_BSTR only (VT_BYREF for VT_UI2 will be added automatically)
		);
#endif // (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
#endif // _MFC_VER >= 0x700
	void _VariantAssign(
		signed int nSrc,
		VARTYPE nVariantType = VT_I4 // VT_I4 or VT_INT only
		);
	void _VariantAssign(
		signed int * nSrc,
		VARTYPE nVariantType = VT_I4 // VT_I4 or VT_INT only (VT_BYREF will be added automatically)
		);
	void _VariantAssign(
		unsigned int nSrc,
		VARTYPE nVariantType = VT_UI4 // VT_UI4 or VT_UINT only
		);
	void _VariantAssign(
		unsigned int * nSrc,
		VARTYPE nVariantType = VT_UI4 // VT_UI4 or VT_UINT only (VT_BYREF will be added automatically)
		);
	void _VariantAssign(
		signed long nSrc,
		VARTYPE nVariantType = VT_I4 // VT_I4 or VT_ERROR only
		);
	void _VariantAssign(
		signed long * nSrc,
		VARTYPE nVariantType = VT_I4 // VT_I4 or VT_ERROR only (VT_BYREF will be added automatically)
		);
	void _VariantAssign( unsigned long nSrc );
	void _VariantAssign( unsigned long * nSrc );
	void _VariantAssign( signed __int64 nSrc );
	void _VariantAssign( signed __int64 * nSrc );
	void _VariantAssign( unsigned __int64 nSrc );
	void _VariantAssign( unsigned __int64 * nSrc );
	void _VariantAssign( float nSrc );
	void _VariantAssign( float * nSrc );
	void _VariantAssign(
		double nSrc,
		VARTYPE nVariantType = VT_R8 // VT_R8 or VT_DATE only
		);
	void _VariantAssign(
		double * nSrc,
		VARTYPE nVariantType = VT_R8 // VT_R8 or VT_DATE only (VT_BYREF will be added automatically)
		);
	void _VariantAssign( CY cySrc );
	void _VariantAssign( CY * cySrc );
	bool _VariantIsEqual(
		const VARIANT & other,
		LCID _lcid_ = LOCALE_USER_DEFAULT,
		ULONG nVarCmpFlagsFlags = 0L
		) const;
	bool _VariantIsInequal(
		const VARIANT & other,
		LCID _lcid_ = LOCALE_USER_DEFAULT,
		ULONG nVarCmpFlagsFlags = 0L
		) const;
	bool _VariantIsLess(
		const VARIANT & other,
		LCID _lcid_ = LOCALE_USER_DEFAULT,
		ULONG nVarCmpFlagsFlags = 0L
		) const;
	bool _VariantIsGreater(
		const VARIANT & other,
		LCID _lcid_ = LOCALE_USER_DEFAULT,
		ULONG nVarCmpFlagsFlags = 0L
		) const;
	bool _VariantIsLessOrEqual(
		const VARIANT & other,
		LCID _lcid_ = LOCALE_USER_DEFAULT,
		ULONG nVarCmpFlagsFlags = 0L
		) const;
	bool _VariantIsGreaterOrEqual(
		const VARIANT & other,
		LCID _lcid_ = LOCALE_USER_DEFAULT,
		ULONG nVarCmpFlagsFlags = 0L
		) const;
	int _VariantCompare(
		const VARIANT & other,
		LCID _lcid_ = LOCALE_USER_DEFAULT,
		ULONG nVarCmpFlagsFlags = 0L
		) const;
	bool _VariantChangeType(
		VARTYPE vtNewType,
		const VARIANT * pVariantOther = NULL,
		bool bConvertToErrorIfFailed = true
		);

protected:
	void _AssignA( const CExtGridCellVariant & other );
public:
	CExtGridCellVariant(
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant( const CExtGridCellVariant & other );
	CExtGridCellVariant( const CExtGridCell & other );
	CExtGridCellVariant(
		const VARIANT & other,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		IDispatch * pInterface,
		bool bAllowNullInterface = true,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		IUnknown * pInterface,
		bool bAllowNullInterface = true,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		IDispatch ** ppInterface,
		bool bAllowNullInterface = true,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		IUnknown ** ppInterface,
		bool bAllowNullInterface = true,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		const SAFEARRAY * pSA,
		bool bAllowConvertToError = true,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		const SAFEARRAY ** ppSA,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		const DECIMAL & decimalVal,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		const DECIMAL ** decimalVal,
		CExtGridDataProvider * pDataProvider = NULL
		);
//	CExtGridCellVariant( BSTR bstrOther, CExtGridDataProvider * pDataProvider = NULL );
//	CExtGridCellVariant( BSTR * bstrOther, CExtGridDataProvider * pDataProvider = NULL );
	CExtGridCellVariant(
		const char * strOther,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		const unsigned short * strOther,
		CExtGridDataProvider * pDataProvider = NULL
		);
#if _MFC_VER >= 0x700
#if (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
	CExtGridCellVariant(
		const __wchar_t * strOther,
		CExtGridDataProvider * pDataProvider = NULL
		);
#endif // (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
#endif // _MFC_VER >= 0x700
	CExtGridCellVariant(
		bool bSrc,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		signed char nSrc,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		signed char * nSrc,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		unsigned char nSrc,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		unsigned char * nSrc,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		signed short nSrc,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		signed short * nSrc,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		unsigned short nSrc,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		unsigned short * nSrc,
		VARTYPE nVariantType = VT_UI2, // VT_UI2 or VT_BSTR only (VT_BYREF for VT_UI2 will be added automatically)
		CExtGridDataProvider * pDataProvider = NULL
		);
#if _MFC_VER >= 0x700
#if (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
	CExtGridCellVariant(
		__wchar_t * nSrc,
		VARTYPE nVariantType = VT_UI2, // VT_UI2 or VT_BSTR only (VT_BYREF for VT_UI2 will be added automatically)
		CExtGridDataProvider * pDataProvider = NULL
		);
#endif // (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
#endif // _MFC_VER >= 0x700
	CExtGridCellVariant(
		signed int nSrc,
		VARTYPE nVariantType = VT_I4, // VT_I4 or VT_INT only
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		signed int * nSrc,
		VARTYPE nVariantType = VT_I4, // VT_I4 or VT_INT only (VT_BYREF will be added automatically)
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		unsigned int nSrc,
		VARTYPE nVariantType = VT_UI4, // VT_UI4 or VT_UINT only
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		unsigned int * nSrc,
		VARTYPE nVariantType = VT_UI4, // VT_UI4 or VT_UINT only (VT_BYREF will be added automatically)
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		signed long nSrc,
		VARTYPE nVariantType = VT_I4, // VT_I4 or VT_ERROR only
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		signed long * nSrc,
		VARTYPE nVariantType = VT_I4, // VT_I4 or VT_ERROR only (VT_BYREF will be added automatically)
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		unsigned long nSrc,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		unsigned long * nSrc,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		signed __int64 nSrc,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		signed __int64 * nSrc,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		unsigned __int64 nSrc,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		unsigned __int64 * nSrc,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		float nSrc,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		float * nSrc,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		double nSrc,
		VARTYPE nVariantType = VT_R8, // VT_R8 or VT_DATE only
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		double * nSrc,
		VARTYPE nVariantType = VT_R8, // VT_R8 or VT_DATE only (VT_BYREF will be added automatically)
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		CY cySrc,
		CExtGridDataProvider * pDataProvider = NULL
		);
	CExtGridCellVariant(
		CY * cySrc,
		CExtGridDataProvider * pDataProvider = NULL
		);
	virtual ~CExtGridCellVariant();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext & dc ) const;
#endif // _DEBUG

	virtual void Serialize( CArchive & ar );
	virtual bool IsEmpty() const;
	virtual void Empty();
	virtual void Assign( const CExtGridCell & other );
	virtual int Compare(
		const CExtGridCell & other,
		DWORD dwStyleMask = __EGCS_COMPARE_MASK,
		DWORD dwStyleExMask = __EGCS_EX_COMPARE_MASK
		) const;
	virtual void TextGet( CExtSafeString & strCopy ) const;
	virtual void TextSet(
		__EXT_MFC_SAFE_LPCTSTR str = __EXT_MFC_SAFE_LPCTSTR(NULL), // empty text
		bool bAllowChangeDataType = false
		);
	virtual UINT OnQueryDrawTextFlags(
		LONG nVisibleColNo,
		LONG nVisibleRowNo,
		LONG nColNo,
		LONG nRowNo,
		INT nColType,
		INT nRowType,
		DWORD dwAreaFlags,
		bool bIncludeHorizontalFlags = true,
		bool bIncludeVerticalFlags = true,
		bool bIncludeOtherFlags = true
		) const;
	virtual void GetVariant( VARIANT & varCopy ) const;
	CExtGridCellVariant & operator = (  const CExtGridCell & other );
	CExtGridCellVariant & operator = (  const VARIANT & other );
	CExtGridCellVariant & operator = ( IDispatch * pInterface );
	CExtGridCellVariant & operator = ( IUnknown * pInterface );
	CExtGridCellVariant & operator = ( IDispatch ** ppInterface );
	CExtGridCellVariant & operator = ( IUnknown ** ppInterface );
	CExtGridCellVariant & operator = ( const SAFEARRAY * pSA );
	CExtGridCellVariant & operator = ( const SAFEARRAY ** ppSA );
	CExtGridCellVariant & operator = ( const DECIMAL & decimalVal );
	CExtGridCellVariant & operator = ( const DECIMAL ** decimalVal );
	bool operator == ( const CExtGridCellVariant & other ) const;
	bool operator == ( const VARIANT & other ) const;
	bool operator != ( const CExtGridCellVariant & other ) const;
	bool operator != ( const VARIANT & other ) const;
	bool operator < ( const CExtGridCellVariant & other ) const;
	bool operator < ( const VARIANT & other ) const;
	bool operator > ( const CExtGridCellVariant & other ) const;
	bool operator > ( const VARIANT & other ) const;
	bool operator <= ( const CExtGridCellVariant & other ) const;
	bool operator <= ( const VARIANT & other ) const;
	bool operator >= ( const CExtGridCellVariant & other ) const;
	bool operator >= ( const VARIANT & other ) const;
//	CExtGridCellVariant & operator = ( BSTR bstrOther );
//	CExtGridCellVariant & operator = ( BSTR * bstrOther );
	CExtGridCellVariant & operator = ( const char * strOther );
	CExtGridCellVariant & operator = ( const unsigned short * strOther );
#if _MFC_VER >= 0x700
#if (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
	CExtGridCellVariant & operator = ( const __wchar_t * strOther );
#endif // (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
#endif // _MFC_VER >= 0x700
	CExtGridCellVariant & operator = ( bool bSrc );
	CExtGridCellVariant & operator = ( signed char nSrc );
	CExtGridCellVariant & operator = ( signed char * nSrc );
	CExtGridCellVariant & operator = ( unsigned char nSrc );
	CExtGridCellVariant & operator = ( unsigned char * nSrc );
	CExtGridCellVariant & operator = ( signed short nSrc );
	CExtGridCellVariant & operator = ( signed short * nSrc );
	CExtGridCellVariant & operator = ( unsigned short nSrc );
	CExtGridCellVariant & operator = ( unsigned short * nSrc );
#if _MFC_VER >= 0x700
#if (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
	CExtGridCellVariant & operator = ( __wchar_t * nSrc );
#endif // (defined _UNICODE) && (defined _WCHAR_T_DEFINED)
#endif // _MFC_VER >= 0x700
	CExtGridCellVariant & operator = ( signed int nSrc );
	CExtGridCellVariant & operator = ( signed int * nSrc );
	CExtGridCellVariant & operator = ( unsigned int nSrc );
	CExtGridCellVariant & operator = ( unsigned int * nSrc );
	CExtGridCellVariant & operator = ( signed long nSrc );
	CExtGridCellVariant & operator = ( signed long * nSrc );
	CExtGridCellVariant & operator = ( unsigned long nSrc );
	CExtGridCellVariant & operator = ( unsigned long * nSrc );
	CExtGridCellVariant & operator = ( signed __int64 nSrc );
	CExtGridCellVariant & operator = ( signed __int64 * nSrc );
	CExtGridCellVariant & operator = ( unsigned __int64 nSrc );
	CExtGridCellVariant & operator = ( unsigned __int64 * nSrc );
	CExtGridCellVariant & operator = ( float nSrc );
	CExtGridCellVariant & operator = ( float * nSrc );
	CExtGridCellVariant & operator = ( double nSrc );
	CExtGridCellVariant & operator = ( double * nSrc );
	CExtGridCellVariant & operator = ( CY cySrc );
	CExtGridCellVariant & operator = ( CY * cySrc );
	virtual HRESULT OnParseText(
		__EXT_MFC_SAFE_LPCTSTR sText
		) const;
	virtual bool OnQueryEnabledIncrement() const;
	virtual bool OnQueryEnabledDecrement() const;
	virtual bool OnValueIncrement();
	virtual bool OnValueDecrement();
}; // class CExtGridCellVariant

/////////////////////////////////////////////////////////////////////////////
// CExtGridDataSortOrder

class __PROF_UIS_API CExtGridDataSortOrder : public CObject
{
	void _AssignFromOther(
		const CExtGridDataSortOrder & other
		);
public:
	DECLARE_SERIAL( CExtGridDataSortOrder );
	struct __PROF_UIS_API ITEM_INFO
	{
		LONG m_nRowColNo;
		bool m_bAscending:1;
		ITEM_INFO(
			LONG nRowColNo = -1L,
			bool bAscending = true
			)
			: m_nRowColNo( nRowColNo )
			, m_bAscending( bAscending )
		{
		}
		ITEM_INFO( const ITEM_INFO & other )
			: m_nRowColNo( other.m_nRowColNo )
			, m_bAscending( other.m_bAscending )
		{
		}
		ITEM_INFO & operator = ( const ITEM_INFO & other )
		{
			m_nRowColNo = other.m_nRowColNo;
			m_bAscending = other.m_bAscending;
			return (*this);
		}
		void Serialize( CArchive & ar )
		{
			BYTE nFlags = 0;
			if( ar.IsStoring() )
			{
				if( m_bAscending )
					nFlags |= 0x01;
				ar.Write( &m_nRowColNo, sizeof(m_nRowColNo) );
				ar.Write( &nFlags, sizeof(nFlags) );
			} // if( ar.IsStoring() )
			else
			{
				ar.Read( &m_nRowColNo, sizeof(m_nRowColNo) );
				ar.Read( &nFlags, sizeof(nFlags) );
				if( (nFlags&0x01) != 0 )
					m_bAscending = true;
				else
					m_bAscending = false;
			} // else from if( ar.IsStoring() )
		}
		int Cmp( const ITEM_INFO & other ) const
		{
			if( m_nRowColNo < other.m_nRowColNo )
				return -1;
			if( m_nRowColNo > other.m_nRowColNo )
				return 1;
			if( m_bAscending )
			{
				if( ! other.m_bAscending )
					return 1;
			} // if( m_bAscending )
			else
			{
				if( other.m_bAscending )
					return -1;
			} // else from if( m_bAscending )
			return 0;
		}
		bool operator == ( const ITEM_INFO & other ) const
		{
			return ( (Cmp(other) == 0) ? true : false );
		}
		bool operator != ( const ITEM_INFO & other ) const
		{
			return ( (Cmp(other) != 0) ? true : false );
		}
		bool operator <= ( const ITEM_INFO & other ) const
		{
			return ( (Cmp(other) <= 0) ? true : false );
		}
		bool operator >= ( const ITEM_INFO & other ) const
		{
			return ( (Cmp(other) >= 0) ? true : false );
		}
		bool operator < ( const ITEM_INFO & other ) const
		{
			return ( (Cmp(other) < 0) ? true : false );
		}
		bool operator > ( const ITEM_INFO & other ) const
		{
			return ( (Cmp(other) > 0) ? true : false );
		}
	}; // struct ITEM_INFO
	CArray < ITEM_INFO, ITEM_INFO & > m_arrItems;
	CExtGridDataSortOrder();
	CExtGridDataSortOrder(
		const CExtGridDataSortOrder & other
		);
	~CExtGridDataSortOrder();
	CExtGridDataSortOrder & operator =(
		const CExtGridDataSortOrder & other
		);
	bool operator ==(
		const CExtGridDataSortOrder & other
		) const;
	bool operator !=(
		const CExtGridDataSortOrder & other
		) const;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext & dc ) const;
#endif // _DEBUG
	virtual void Serialize( CArchive & ar );
	bool ItemsUnique() const;
	void MakeItemsUnique();
	bool IsEmpty() const;
	void Empty();
	LONG GetRowColPos( LONG nRowColNo ) const;
	void SetupOrder(
		const CExtGridDataSortOrder & _gdsoUpdate,
		bool bInvertIntersectionSortOrder
		);
	void UpdateIndices(
		LONG nRowColNo,
		LONG nInsertRemoveCount,
		bool bInsert
		);
	void SwapDroppedSeries(
		LONG nRowColNoSrc,
		LONG nRowColNoDropBefore
		);
}; // class CExtGridDataSortOrder

/////////////////////////////////////////////////////////////////////////////
// CExtGridDataProvider
// (abstract data provider for any grid window)

class __PROF_UIS_API CExtGridDataProvider : public CObject
{
public:
	DECLARE_DYNAMIC( CExtGridDataProvider );

	struct __PROF_UIS_API IDataProviderEvents
	{
		virtual void OnDataProviderSortEnter(
			bool bColumns // true = sorting/swapping columns, false - rows
			) = 0;
		virtual void OnDataProviderSortLeave(
			bool bColumns // true = sorting/swapping columns, false - rows
			) = 0;
		virtual void OnDataProviderSwapSeries(
			bool bColumns, // true = sorting/swapping columns, false - rows
			LONG nRowColNo1,
			LONG nRowColNo2,
			LONG nSwapCounter
			) = 0;
		virtual void OnDataProviderSwapDroppedSeries(
			bool bColumns, // true = swapping columns, false - rows
			LONG nRowColNoSrc,
			LONG nRowColNoDropBefore
			) = 0;
	}; // struct IDataProviderEvents

	CExtGridDataProvider();
	virtual ~CExtGridDataProvider();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext & dc ) const;
#endif // _DEBUG
	virtual __EXT_MFC_SAFE_LPTSTR StringAlloc(
		INT nCharCountIncZT
		) = 0;
	__EXT_MFC_SAFE_LPTSTR StringAlloc(
		const CExtSafeString & strSrc
		);
	__EXT_MFC_SAFE_LPTSTR StringAlloc(
		__EXT_MFC_SAFE_LPCTSTR strSrc
		);
	virtual void StringFree(
		__EXT_MFC_SAFE_LPTSTR strToFree
		) = 0;
	virtual bool ColumnInsert(
		ULONG nColNo,
		ULONG nInsertCount = 1
		) = 0;
	virtual bool RowInsert(
		ULONG nRowNo,
		ULONG nInsertCount = 1
		) = 0;
	virtual ULONG ColumnCountGet() const = 0;
	virtual ULONG RowCountGet() const = 0;
	virtual bool ColumnRemove(
		ULONG nColNo,
		ULONG nRemoveCount = 1
		) = 0;
	virtual bool RowRemove(
		ULONG nRowNo,
		ULONG nRemoveCount = 1
		) = 0;
	virtual void MinimizeMemoryUsage() = 0;
	virtual bool RowDefaultValueBind(
		ULONG nRowNo = (ULONG(-1)), // if (ULONG(-1)) - default value for all rows
		CExtGridCell * pCell = NULL // if NULL - remove default value
		) = 0;
	virtual bool ColumnDefaultValueBind(
		ULONG nColNo = (ULONG(-1)), // if (ULONG(-1)) - default value for all columns
		CExtGridCell * pCell = NULL // if NULL - remove default value
		) = 0;
	bool RowDefaultValueUnbind(
		ULONG nRowNo = (ULONG(-1)) // if (ULONG(-1)) - default value for all rows
		);
	bool ColumnDefaultValueUnbind(
		ULONG nColNo = (ULONG(-1)) // if (ULONG(-1)) - default value for all columns
		);
	virtual void RowDefaultValueUnbindAll() = 0;
	virtual void ColumnDefaultValueUnbindAll() = 0;
	virtual CExtGridCell * RowDefaultValueGet(
		ULONG nRowNo
		) = 0;
	virtual CExtGridCell * ColumnDefaultValueGet(
		ULONG nColNo
		) = 0;
	const CExtGridCell * RowDefaultValueGet(
		ULONG nRowNo
		) const;
	const CExtGridCell * ColumnDefaultValueGet(
		ULONG nColNo
		) const;
	virtual CExtGridCell * CellGet(
		ULONG nColNo,
		ULONG nRowNo,
		CRuntimeClass * pInitRTC = NULL,
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true // false - use row default value (only when bAutoFindValue=true)
		) = 0;
	const CExtGridCell * CellGet(
		ULONG nColNo,
		ULONG nRowNo,
		CRuntimeClass * pInitRTC = NULL,
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true // false - use row default value (only when bAutoFindValue=true)
		) const;
	virtual bool CellRangeSet(
		ULONG nColNo,
		ULONG nRowNo,
		ULONG nColCount = 1L,
		ULONG nRowCount = 1L,
		const CExtGridCell * pCellNewValue = NULL, // if NULL - empty existing cell values
		bool bReplace = false, // false - assign to existing cell instances or column/row type created cells, true - create new cloned copies of pCellNewValue
		CRuntimeClass * pInitRTC = NULL, // runtime class for new cell instance (used if bReplace=false)
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true, // false - use row default value (only when bAutoFindValue=true)
		ULONG * p_nUpdatedCellCount = NULL // returns count of really updated cells (zero also may be treated as success)
		) = 0;
	bool CellRangeEmpty(
		ULONG nColNo,
		ULONG nRowNo,
		ULONG nColCount = 1L,
		ULONG nRowCount = 1L,
		ULONG * p_nUpdatedCellCount = NULL // returns count of really erased cells (zero also may be treated as success)
		);
	bool CellRangeEmptyAll(
		ULONG * p_nUpdatedCellCount = NULL // returns count of really erased cells (zero also may be treated as success)
		);
	virtual bool CacheReserveForOuterCells(
		ULONG nColCount,
		ULONG nRowCount
		) = 0;
	virtual void CacheReservedCountsGet(
		ULONG * p_nColCount,
		ULONG * p_nRowCount
		) const = 0;
	virtual bool CacheData(
		const CExtScrollItemCacheInfo & _sciNew,
		const CExtScrollItemCacheInfo & _sciOld
		) = 0;
	virtual bool CacheIsVisibleFirstRecord( bool bHorz ) = 0;
	virtual bool CacheIsVisibleLastRecord( bool bHorz ) = 0;
	virtual bool CacheIsVisibleFirstRecord( bool bHorz ) const;
	virtual bool CacheIsVisibleLastRecord( bool bHorz ) const;
	virtual ULONG CacheColumnCountGet() = 0;
	virtual ULONG CacheRowCountGet() = 0;
	ULONG CacheColumnCountGet() const;
	ULONG CacheRowCountGet() const;
	virtual INT IconGetCount() = 0;
	INT IconGetCount() const;
	virtual CExtCmdIcon * IconGetAt( INT nIdx ) = 0;
	const CExtCmdIcon * IconGetAt( INT nIdx ) const;
	virtual INT IconInsert( // returns index or -1
		CExtCmdIcon * pIcon,
		INT nIdx = -1, // append
		bool bCopyIcon = true
		) = 0;
	INT IconInsert( // returns index or -1
		const CExtCmdIcon * pIcon,
		INT nIdx = -1, // append
		bool bCopyIcon = true
		);
	virtual INT IconRemove(
		INT nIdx = 0,
		INT nCountToRemove = -1 // all
		) = 0;
	virtual bool SortOrderUpdate(
		bool bColumns, // true = sort order for columns, false - for rows
		IDataProviderEvents * pDPE = NULL
		) = 0;
	virtual bool SortOrderSet(
		const CExtGridDataSortOrder & _gdso,
		bool bColumns, // true = sort order for columns, false - for rows
		IDataProviderEvents * pDPE = NULL
		) = 0;
	virtual bool SortOrderGet(
		CExtGridDataSortOrder & _gdso,
		bool bColumns // true = sort order for columns, false - for rows
		) const = 0;
	virtual bool SwapDroppedSeries(
		bool bColumns, // true = swap columns, false - rows
		ULONG nRowColNoSrc,
		ULONG nRowColNoDropBefore,
		IDataProviderEvents * pDPE = NULL
		) = 0;
}; // class CExtGridDataProvider

/////////////////////////////////////////////////////////////////////////////
// CExtGDPI
// template for adding icon store to any data provider

template < class _BDP >
class CExtGDPI : public _BDP
{
	CTypedPtrArray < CPtrArray, CExtCmdIcon * > m_arrIcons;
	INT _IconRemoveImpl(
		INT nIdx = 0,
		INT nCountToRemove = -1 // all
		)
	{
		if( nIdx < 0 )
		{
			ASSERT( FALSE );
			return 0;
		} // if( nIdx < 0 )
		if( nCountToRemove == 0 )
			return 0;
		INT nCount = (INT)m_arrIcons.GetSize();
		if( nCount == 0 )
			return 0;
		if( nIdx >= nCount )
			return 0;
		if( nCountToRemove < 0 )
			nCountToRemove = nCount;
		if( (nIdx+nCountToRemove) > nCount )
			nCountToRemove = nCount-nIdx;
		ASSERT( nCountToRemove >= 0 );
		if( nCountToRemove == 0 )
			return 0;
		INT nPosCurrent = nIdx;
		INT nPosLast = nIdx + nCountToRemove - 1;
		ASSERT( nPosLast >= nPosCurrent );
		for( ; nPosCurrent <= nPosLast; nPosCurrent++ )
		{
			CExtCmdIcon * pIcon = m_arrIcons[ nPosCurrent ];
			ASSERT( pIcon != NULL );
			delete pIcon;
		}
		m_arrIcons.RemoveAt( nIdx, nCountToRemove );
		return nCountToRemove;
	}
public:
	CExtGDPI()
	{
	}
	virtual ~CExtGDPI()
	{
		_IconRemoveImpl();
		ASSERT( m_arrIcons.GetSize() == 0 );
	}
	virtual INT IconGetCount()
	{
		ASSERT_VALID( this );
		INT nCount = (INT)m_arrIcons.GetSize();
		return nCount;
	}
	virtual CExtCmdIcon * IconGetAt( INT nIdx )
	{
		ASSERT_VALID( this );
		INT nCount = (INT)m_arrIcons.GetSize();
		if( nIdx < 0 || nIdx >= nCount )
		{
			ASSERT( FALSE );
			return NULL;
		} // if( nIdx < 0 || nIdx >= nCount )
		CExtCmdIcon * pIcon = m_arrIcons[ nIdx ];
		ASSERT( pIcon != NULL );
		return pIcon;
	}
	virtual INT IconInsert( // returns index or -1
		CExtCmdIcon * pIcon,
		INT nIdx = -1, // append
		bool bCopyIcon = true
		)
	{
		ASSERT_VALID( this );
		if( pIcon == NULL )
		{
			ASSERT( FALSE );
			return -1;
		} // if( pIcon == NULL )
		INT nCount = (INT)m_arrIcons.GetSize();
		if( nIdx < 0 || nIdx > nCount )
			nIdx = nCount;
		if( bCopyIcon )
			pIcon = new CExtCmdIcon( *pIcon );
		m_arrIcons.InsertAt( nIdx, pIcon, 1 );
		return nIdx;
	}
	virtual INT IconRemove(
		INT nIdx = 0,
		INT nCountToRemove = -1 // all
		)
	{
		ASSERT_VALID( this );
		return _IconRemoveImpl( nIdx, nCountToRemove );
	}
}; // class CExtGDPI

/////////////////////////////////////////////////////////////////////////////
// CExtGridDataProviderBL
// (basic layer data provider)

class __PROF_UIS_API CExtGridDataProviderBL
	: public CExtGDPI < CExtGridDataProvider >
{
protected:
	ULONG m_nColCountReserved, m_nRowCountReserved;
	typedef
		CMap <
			ULONG,
			ULONG,
			CExtGridCell *,
			CExtGridCell *
			>
		DefValueMap_t;
	DefValueMap_t m_mapDVX, m_mapDVY;
	void _DVM_RemoveAll(
		CExtGridDataProviderBL::DefValueMap_t & _map
		);
	bool _DVM_Setup(
		CExtGridDataProviderBL::DefValueMap_t & _map,
		ULONG nIdx,
		CExtGridCell * pCell
		);
	CExtGridCell * _DVM_Lookup(
		CExtGridDataProviderBL::DefValueMap_t & _map,
		ULONG nIdx
		);
	virtual IMalloc * _DVM_GetCellAllocator() = 0;
public:
	DECLARE_DYNAMIC( CExtGridDataProviderBL );

	CExtGridDataProviderBL();
	virtual ~CExtGridDataProviderBL();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext & dc ) const;
#endif // _DEBUG

	virtual bool RowDefaultValueBind(
		ULONG nRowNo = (ULONG(-1)), // if (ULONG(-1)) - default value for all rows
		CExtGridCell * pCell = NULL // if NULL - remove default value
		);
	virtual bool ColumnDefaultValueBind(
		ULONG nColNo = (ULONG(-1)), // if (ULONG(-1)) - default value for all columns
		CExtGridCell * pCell = NULL // if NULL - remove default value
		);
	virtual void RowDefaultValueUnbindAll();
	virtual void ColumnDefaultValueUnbindAll();
	virtual CExtGridCell * RowDefaultValueGet(
		ULONG nRowNo
		);
	virtual CExtGridCell * ColumnDefaultValueGet(
		ULONG nColNo
		);
	virtual bool CacheReserveForOuterCells(
		ULONG nColCount,
		ULONG nRowCount
		);
	virtual void CacheReservedCountsGet(
		ULONG * p_nColCount,
		ULONG * p_nRowCount
		) const;
	virtual bool CacheData(
		const CExtScrollItemCacheInfo & _sciNew,
		const CExtScrollItemCacheInfo & _sciOld
		);
	virtual bool CacheIsVisibleFirstRecord( bool bHorz );
	virtual bool CacheIsVisibleLastRecord( bool bHorz );
	virtual ULONG CacheColumnCountGet();
	virtual ULONG CacheRowCountGet();
}; // class CExtGridDataProviderBL

/////////////////////////////////////////////////////////////////////////////
// CExtGridDataProviderRT
// (data provider which retranslates method calls to other
// agregated data provider object with/without X/Y flipping)

class __PROF_UIS_API CExtGridDataProviderRT : public CExtGridDataProvider
{
public:
	DECLARE_DYNCREATE( CExtGridDataProviderRT );
	struct __PROF_UIS_API IDataProviderEventsFlip
		: public CExtGridDataProvider::IDataProviderEvents
	{
		IDataProviderEvents * m_pOther;
		IDataProviderEventsFlip(
			IDataProviderEvents * pOther
			)
			: m_pOther( pOther )
		{
			ASSERT( m_pOther != NULL );
		}
		virtual void OnDataProviderSortEnter(
			bool bColumns // true = sorting/swapping columns, false - rows
			)
		{
			ASSERT( m_pOther != NULL );
			m_pOther->OnDataProviderSortEnter( ! bColumns );
		}
		virtual void OnDataProviderSortLeave(
			bool bColumns // true = sorting/swapping columns, false - rows
			)
		{
			ASSERT( m_pOther != NULL );
			m_pOther->OnDataProviderSortLeave( ! bColumns );
		}
		virtual void OnDataProviderSwapSeries(
			bool bColumns, // true = sorting/swapping columns, false - rows
			LONG nRowColNo1,
			LONG nRowColNo2,
			LONG nSwapCounter
			)
		{
			ASSERT( m_pOther != NULL );
			m_pOther->OnDataProviderSwapSeries(
				! bColumns,
				nRowColNo1,
				nRowColNo2,
				nSwapCounter
				);
		}
		virtual void OnDataProviderSwapDroppedSeries(
			bool bColumns, // true = swapping columns, false - rows
			LONG nRowColNoSrc,
			LONG nRowColNoDropBefore
			)
		{
			m_pOther->OnDataProviderSwapDroppedSeries(
				! bColumns,
				nRowColNoSrc,
				nRowColNoDropBefore
				);
		}
	}; // struct IDataProviderEventsFlip
	CExtGridDataProvider * m_pOther;
	bool m_bAutoDeleteOther:1;
	bool m_bFlipHV:1;
	CExtGridDataProviderRT(
		CExtGridDataProvider * pOther = NULL,
		bool bAutoDeleteOther = false,
		bool bFlipHV = false
		);
	~CExtGridDataProviderRT();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext & dc ) const;
#endif // _DEBUG
	virtual __EXT_MFC_SAFE_LPTSTR StringAlloc(
		INT nCharCountIncZT
		);
	virtual void StringFree(
		__EXT_MFC_SAFE_LPTSTR strToFree
		);
	virtual bool ColumnInsert(
		ULONG nColNo,
		ULONG nInsertCount = 1
		);
	virtual bool RowInsert(
		ULONG nRowNo,
		ULONG nInsertCount = 1
		);
	virtual ULONG ColumnCountGet() const;
	virtual ULONG RowCountGet() const;
	virtual bool ColumnRemove(
		ULONG nColNo,
		ULONG nRemoveCount = 1
		);
	virtual bool RowRemove(
		ULONG nRowNo,
		ULONG nRemoveCount = 1
		);
	virtual void MinimizeMemoryUsage();
	virtual bool RowDefaultValueBind(
		ULONG nRowNo = (ULONG(-1)), // if (ULONG(-1)) - default value for all rows
		CExtGridCell * pCell = NULL // if NULL - remove default value
		);
	virtual bool ColumnDefaultValueBind(
		ULONG nColNo = (ULONG(-1)), // if (ULONG(-1)) - default value for all columns
		CExtGridCell * pCell = NULL // if NULL - remove default value
		);
	virtual void RowDefaultValueUnbindAll();
	virtual void ColumnDefaultValueUnbindAll();
	virtual CExtGridCell * RowDefaultValueGet(
		ULONG nRowNo
		);
	virtual CExtGridCell * ColumnDefaultValueGet(
		ULONG nColNo
		);
	virtual CExtGridCell * CellGet(
		ULONG nColNo,
		ULONG nRowNo,
		CRuntimeClass * pInitRTC = NULL,
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true // false - use row default value (only when bAutoFindValue=true)
		);
	virtual bool CellRangeSet(
		ULONG nColNo,
		ULONG nRowNo,
		ULONG nColCount = 1L,
		ULONG nRowCount = 1L,
		const CExtGridCell * pCellNewValue = NULL, // if NULL - empty existing cell values
		bool bReplace = false, // false - assign to existing cell instances or column/row type created cells, true - create new cloned copies of pCellNewValue
		CRuntimeClass * pInitRTC = NULL, // runtime class for new cell instance (used if bReplace=false)
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true, // false - use row default value (only when bAutoFindValue=true)
		ULONG * p_nUpdatedCellCount = NULL // returns count of really updated cells (zero also may be treated as success)
		);
	virtual bool CacheReserveForOuterCells(
		ULONG nColCount,
		ULONG nRowCount
		);
	virtual void CacheReservedCountsGet(
		ULONG * p_nColCount,
		ULONG * p_nRowCount
		) const;
	virtual bool CacheData(
		const CExtScrollItemCacheInfo & _sciNew,
		const CExtScrollItemCacheInfo & _sciOld
		);
	virtual bool CacheIsVisibleFirstRecord( bool bHorz );
	virtual bool CacheIsVisibleLastRecord( bool bHorz );
	virtual ULONG CacheColumnCountGet();
	virtual ULONG CacheRowCountGet();
	virtual INT IconGetCount();
	virtual CExtCmdIcon * IconGetAt( INT nIdx );
	virtual INT IconInsert( // returns index or -1
		CExtCmdIcon * pIcon,
		INT nIdx = -1, // append
		bool bCopyIcon = true
		);
	virtual INT IconRemove(
		INT nIdx = 0,
		INT nCountToRemove = -1 // all
		);
	virtual bool SortOrderUpdate(
		bool bColumns, // true = sort order for columns, false - for rows
		IDataProviderEvents * pDPE = NULL
		);
	virtual bool SortOrderSet(
		const CExtGridDataSortOrder & _gdso,
		bool bColumns, // true = sort order for columns, false - for rows
		IDataProviderEvents * pDPE = NULL
		);
	virtual bool SortOrderGet(
		CExtGridDataSortOrder & _gdso,
		bool bColumns // true = sort order for columns, false - for rows
		) const;
	virtual bool SwapDroppedSeries(
		bool bColumns, // true = swap columns, false - rows
		ULONG nRowColNoSrc,
		ULONG nRowColNoDropBefore,
		IDataProviderEvents * pDPE = NULL
		);
}; // class CExtGridDataProviderRT

/////////////////////////////////////////////////////////////////////////////
// CExtGridDataProviderRecordset
// (cached data provider for vertical static/virtual modes)

class __PROF_UIS_API CExtGridDataProviderRecordset : public CExtGridDataProviderRT
{
protected:
	CExtScrollItemCacheInfo m_sci;
	ULONG m_nRowNoHeader;
	bool m_bHeaderInitialized:1,
		m_bVisibleFirstRecord:1, m_bVisibleLastRecord:1;
	LONG m_nVirtualPageSize, m_nVirtualRsOffset;
	// virtual cusor shifting
	void _VCS_ToPageStart();
	void _VCS_FromCurrent( LONG nPosChangingV );
public:
	DECLARE_DYNCREATE( CExtGridDataProviderRecordset );
	CExtGridDataProviderRecordset();
	~CExtGridDataProviderRecordset();
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif // _DEBUG
	virtual bool CacheReserveForOuterCells(
		ULONG nColCount,
		ULONG nRowCount
		);
	virtual bool CacheDataStatic(
		const CExtScrollItemCacheInfo & _sciNew,
		const CExtScrollItemCacheInfo & _sciOld
		);
	virtual bool CacheDataVirtual(
		const CExtScrollItemCacheInfo & _sciNew,
		const CExtScrollItemCacheInfo & _sciOld
		);
	virtual bool CacheData(
		const CExtScrollItemCacheInfo & _sciNew,
		const CExtScrollItemCacheInfo & _sciOld
		);
	virtual bool CacheIsVisibleFirstRecord( bool bHorz );
	virtual bool CacheIsVisibleLastRecord( bool bHorz );
	virtual ULONG ColumnCountGet() const;
	virtual ULONG RowCountGet() const;
	virtual ULONG CacheColumnCountGet();
	virtual ULONG CacheRowCountGet();
	virtual CExtGridCell * CellGet(
		ULONG nColNo,
		ULONG nRowNo,
		CRuntimeClass * pInitRTC = NULL,
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true // false - use row default value (only when bAutoFindValue=true)
		);
	virtual void RsSeekToFirst() = 0L;
	virtual void RsSeekToLast() = 0L;
	virtual void RsSeekToNext() = 0L;
	virtual void RsSeekToPrevious() = 0L;
	virtual void RsSeekFromStart( LONG nPos ) = 0L;
	virtual void RsSeekFromEnd( LONG nPos ) = 0L;
	virtual void RsSeekFromCurrent( LONG nPos ) = 0L;
	virtual bool RsIsBOF() = 0L;
	virtual bool RsIsEOF() = 0L;
	virtual bool RsIsVirtualMode() = 0L;
	virtual LONG RsFieldsCount() = 0;
	virtual LONG RsRecordsCount() = 0;
	virtual void RsCacheRow(
		ULONG nCacheColOffset,
		ULONG nCacheRowNo,
		bool bHeader
		) = 0;
	bool RsIsBOF() const;
	bool RsIsEOF() const;
	bool RsIsVirtualMode() const;
	LONG RsFieldsCount() const;
	LONG RsRecordsCount() const;
	virtual bool SortOrderUpdate(
		bool bColumns, // true = sort order for columns, false - for rows
		IDataProviderEvents * pDPE = NULL
		);
	virtual bool SortOrderSet(
		const CExtGridDataSortOrder & _gdso,
		bool bColumns, // true = sort order for columns, false - for rows
		IDataProviderEvents * pDPE = NULL
		);
	virtual bool SortOrderGet(
		CExtGridDataSortOrder & _gdso,
		bool bColumns // true = sort order for columns, false - for rows
		) const;
	virtual bool SwapDroppedSeries(
		bool bColumns, // true = swap columns, false - rows
		ULONG nRowColNoSrc,
		ULONG nRowColNoDropBefore,
		IDataProviderEvents * pDPE = NULL
		);
}; // class CExtGridDataProviderRecordset

/////////////////////////////////////////////////////////////////////////////
// CExtGridDataProviderMemory
// (dynamic memory data provider based on different kinds of
// IMalloc objects, used as default data provider)

class __PROF_UIS_API CExtGridDataProviderMemory : public CExtGridDataProviderBL
{
protected:
	IMalloc * m_pMallocV, * m_pMallocF, * m_pMallocD;
	ULONG m_nStringAllocationAlign;
	ULONG m_nDX, m_nDY;
	CExtGridCell *** m_pSections;
	CExtGridDataSortOrder m_gdsoColumns, m_gdsoRows;
	virtual bool _SortOrderCheck(
		const CExtGridDataSortOrder & _gdso,
		bool bColumns
		);
	virtual int _SortCmpCells(
		CExtGridCell * pCell_1,
		CExtGridCell * pCell_2
		);
	virtual int _SortCmpSeries(
		LONG nRowColNo1,
		LONG nRowColNo2,
		bool bColumns,
		const CExtGridDataSortOrder & _gdso
		);
	virtual bool _SortSwapSeries(
		LONG nRowColNo1,
		LONG nRowColNo2,
		LONG & nSwapCounter,
		CExtGridDataProvider::IDataProviderEvents * pDPE,
		bool bColumns
		);
	virtual bool _SortStep(
		bool bColumns,
		CExtGridDataProvider::IDataProviderEvents * pDPE,
		CExtGridDataSortOrder & _gdso,
		LONG & nSwapCounter,
		LONG nLow,
		LONG nHigh
		);
	CExtGridCell ** _SectionGetAt(
		ULONG nX,
		bool bAllowDelayedAllocation
		);
	void _SectionFree( CExtGridCell ** pSection );
	void _SectionFreeAll( bool bReleaseSectionStack );
	ULONG _SectionInsertAt(
		ULONG nX,
		ULONG nSectionCount,
		bool bForceDelayedRangeAllocation
		);
	bool _SectionEdject(
		ULONG nX,
		ULONG nDX,
		bool bDeallocateRest
		);
	bool _SectionRealloc(
		ULONG nX,
		ULONG nDY
		);
	bool _SectionReallocAll(
		ULONG nDY
		);
	static void _SectionRangeDestoy(
		IMalloc * pMalloc,
		CExtGridCell ** pSection,
		ULONG nY,
		ULONG nDY
		);
	bool _SectionRangeInjectAll(
		ULONG nY,
		ULONG nDY
		);
	static void _SectionRangeBottomToBottom(
		CExtGridCell ** pSection,
		ULONG nY,
		ULONG nDY,
		ULONG nSectionSize
		);
	void _SectionRangeBottomToBottomAll(
		ULONG nY,
		ULONG nDY
		);
	static void _SectionRangeEdject(
		IMalloc * pMalloc,
		CExtGridCell ** pSection,
		ULONG nY,
		ULONG nDY,
		ULONG nSectionSize
		);
	void _SectionRangeEdjectAll(
		ULONG nY,
		ULONG nDY,
		bool bDeallocateRest
		);
	virtual IMalloc * _DVM_GetCellAllocator();
public:
	DECLARE_DYNCREATE( CExtGridDataProviderMemory );
	CExtGridDataProviderMemory(
		IMalloc * pMallocV = NULL,
		IMalloc * pMallocF = NULL,
		IMalloc * pMallocD = NULL,
		ULONG nStringAllocationAlign = 32
		);
	virtual ~CExtGridDataProviderMemory();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext & dc ) const;
#endif // _DEBUG
	virtual __EXT_MFC_SAFE_LPTSTR StringAlloc(
		INT nCharCountIncZT
		);
	virtual void StringFree(
		__EXT_MFC_SAFE_LPTSTR strToFree
		);
	virtual bool ColumnInsert(
		ULONG nColNo,
		ULONG nInsertCount = 1
		);
	virtual bool RowInsert(
		ULONG nRowNo,
		ULONG nInsertCount = 1
		);
	virtual ULONG ColumnCountGet() const;
	virtual ULONG RowCountGet() const;
	virtual bool ColumnRemove(
		ULONG nColNo,
		ULONG nRemoveCount = 1
		);
	virtual bool RowRemove(
		ULONG nRowNo,
		ULONG nRemoveCount = 1
		);
public:
	virtual void MinimizeMemoryUsage();
	virtual CExtGridCell * CellGet(
		ULONG nColNo,
		ULONG nRowNo,
		CRuntimeClass * pInitRTC = NULL,
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true // false - use row default value (only when bAutoFindValue=true)
		);
	virtual bool CellRangeSet(
		ULONG nColNo,
		ULONG nRowNo,
		ULONG nColCount = 1L,
		ULONG nRowCount = 1L,
		const CExtGridCell * pCellNewValue = NULL, // if NULL - empty existing cell values
		bool bReplace = false, // false - assign to existing cell instances or column/row type created cells, true - create new cloned copies of pCellNewValue
		CRuntimeClass * pInitRTC = NULL, // runtime class for new cell instance (used if bReplace=false)
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true, // false - use row default value (only when bAutoFindValue=true)
		ULONG * p_nUpdatedCellCount = NULL // returns count of really updated cells (zero also may be treated as success)
		);
	virtual bool SortOrderUpdate(
		bool bColumns, // true = sort order for columns, false - for rows
		IDataProviderEvents * pDPE = NULL
		);
	virtual bool SortOrderSet(
		const CExtGridDataSortOrder & _gdso,
		bool bColumns, // true = sort order for columns, false - for rows
		IDataProviderEvents * pDPE = NULL
		);
	virtual bool SortOrderGet(
		CExtGridDataSortOrder & _gdso,
		bool bColumns // true = sort order for columns, false - for rows
		) const;
	virtual bool SwapDroppedSeries(
		bool bColumns, // true = swap columns, false - rows
		ULONG nRowColNoSrc,
		ULONG nRowColNoDropBefore,
		IDataProviderEvents * pDPE = NULL
		);
}; // class CExtGridDataProviderMemory

/////////////////////////////////////////////////////////////////////////////
// CExtGridWnd window

// grid window behavior/sort/edit/drag-n-drop styles provided with
// CExtGridWnd::BseGetStyle() and CExtGridWnd::BseModifyStyle()

// allow editing
#define __EGWS_BSE_EDIT_CELLS_OUTER_T			0x00000001L
#define __EGWS_BSE_EDIT_CELLS_OUTER_B			0x00000002L
#define __EGWS_BSE_EDIT_CELLS_OUTER_HORZ		(__EGWS_BSE_EDIT_CELLS_OUTER_T|__EGWS_BSE_EDIT_CELLS_OUTER_B)
#define __EGWS_BSE_EDIT_CELLS_OUTER_L			0x00000004L
#define __EGWS_BSE_EDIT_CELLS_OUTER_R			0x00000008L
#define __EGWS_BSE_EDIT_CELLS_OUTER_VERT		(__EGWS_BSE_EDIT_CELLS_OUTER_L|__EGWS_BSE_EDIT_CELLS_OUTER_R)
#define __EGWS_BSE_EDIT_CELLS_OUTER				(__EGWS_BSE_EDIT_CELLS_OUTER_HORZ|__EGWS_BSE_EDIT_CELLS_OUTER_VERT)
#define __EGWS_BSE_EDIT_CELLS_INNER				0x00000010L
// allow "walking" editors for inner cells
#define __EGWS_BSE_WALK_HORZ					0x00000020L
#define __EGWS_BSE_WALK_VERT					0x00000040L
#define __EGWS_BSE_WALK							(__EGWS_BSE_WALK_HORZ|__EGWS_BSE_WALK_VERT)

// where cell ellipsis/dropdown/updown buttons should appear?
// in any cell which have buttons
#define __EGWS_BSE_BUTTONS_PERSISTENT			0x00000080L
// in the focused cell only
#define __EGWS_BSE_BUTTONS_IN_FOCUSED_CELL		0x00000100L
// any cell in the focused row
#define __EGWS_BSE_BUTTONS_IN_FOCUSED_ROW		0x00000200L
// any cell in the focused column
#define __EGWS_BSE_BUTTONS_IN_FOCUSED_COLUMN	0x00000400L
// in the hovered cell
#define __EGWS_BSE_BUTTONS_IN_HOVERED_CELL		0x00000800L
// any cell in the hovered row
#define __EGWS_BSE_BUTTONS_IN_HOVERED_ROW		0x00001000L
// any cell in the hovered column
#define __EGWS_BSE_BUTTONS_IN_HOVERED_COLUMN	0x00002000L
// in any selected cell
#define __EGWS_BSE_BUTTONS_IN_SELECTED_CELLS	0x00004000L
// any cell with selection in the same row
#define __EGWS_BSE_BUTTONS_IN_SELECTED_ROW		0x00008000L
// any cell with selection in the same column
#define __EGWS_BSE_BUTTONS_IN_SELECTED_COLUMN	0x00010000L
#define __EGWS_BSE_MASK \
	(__EGWS_BSE_BUTTONS_PERSISTENT \
	|__EGWS_BSE_BUTTONS_IN_FOCUSED_CELL \
	|__EGWS_BSE_BUTTONS_IN_FOCUSED_ROW \
	|__EGWS_BSE_BUTTONS_IN_FOCUSED_COLUMN \
	|__EGWS_BSE_BUTTONS_IN_HOVERED_CELL \
	|__EGWS_BSE_BUTTONS_IN_HOVERED_ROW \
	|__EGWS_BSE_BUTTONS_IN_HOVERED_COLUMN \
	|__EGWS_BSE_BUTTONS_IN_SELECTED_CELLS \
	|__EGWS_BSE_BUTTONS_IN_SELECTED_ROW \
	|__EGWS_BSE_BUTTONS_IN_SELECTED_COLUMN \
	)

// activate inplace editor on single lbutton click
#define __EGWS_BSE_EDIT_SINGLE_LCLICK			0x00020000L
#define __EGWS_BSE_EDIT_SINGLE_FOCUSED_ONLY		0x00040000L
// activate inplace editor on double lbutton click
#define __EGWS_BSE_EDIT_DOUBLE_LCLICK			0x00080000L
// activate inplace editor on VK_RETURN key-down
#define __EGWS_BSE_EDIT_RETURN_CLICK			0x00100000L

// allow sorting by multiple rows/columns
#define __EGWS_BSE_SORT_COLUMNS_ALLOW_MULTIPLE	0x00200000L
#define __EGWS_BSE_SORT_ROWS_ALLOW_MULTIPLE		0x00400000L
// row/column sorting (click on outer cells)
#define __EGWS_BSE_SORT_COLUMNS_T				0x00800000L
#define __EGWS_BSE_SORT_COLUMNS_B				0x01000000L
#define __EGWS_BSE_SORT_COLUMNS					(__EGWS_BSE_SORT_COLUMNS_T|__EGWS_BSE_SORT_COLUMNS_B)
#define __EGWS_BSE_SORT_MULTIPLE_COLUMNS		(__EGWS_BSE_SORT_COLUMNS|__EGWS_BSE_SORT_COLUMNS_ALLOW_MULTIPLE)
#define __EGWS_BSE_SORT_ROWS_L					0x02000000L
#define __EGWS_BSE_SORT_ROWS_R					0x04000000L
#define __EGWS_BSE_SORT_ROWS					(__EGWS_BSE_SORT_ROWS_L|__EGWS_BSE_SORT_ROWS_R)
#define __EGWS_BSE_SORT_MULTIPLE_ROWS			(__EGWS_BSE_SORT_ROWS|__EGWS_BSE_SORT_ROWS_ALLOW_MULTIPLE)
#define __EGWS_BSE_SORT_ANY_MASK				(__EGWS_BSE_SORT_COLUMNS|__EGWS_BSE_SORT_ROWS)

// default bse styles
#define __EGWS_BSE_DEFAULT \
	(__EGWS_BSE_BUTTONS_PERSISTENT \
	|__EGWS_BSE_EDIT_SINGLE_LCLICK \
	|__EGWS_BSE_EDIT_SINGLE_FOCUSED_ONLY \
	|__EGWS_BSE_EDIT_DOUBLE_LCLICK \
	|__EGWS_BSE_EDIT_RETURN_CLICK \
	)

// grid window extended behavior/sort/edit/drag-n-drop styles provided with
// CExtGridWnd::BseGetStyleEx() and CExtGridWnd::BseModifyStyleEx()

// inner/outer row/column highlighting during sort/drag starting
#define __EGBS_BSE_EX_HIGHLIGHT_PRESSING_COLUMNS_INNER	0x00000001L
#define __EGBS_BSE_EX_HIGHLIGHT_PRESSING_COLUMNS_OUTER	0x00000002L
#define __EGBS_BSE_EX_HIGHLIGHT_PRESSING_COLUMNS		(__EGBS_BSE_EX_HIGHLIGHT_PRESSING_COLUMNS_INNER|__EGBS_BSE_EX_HIGHLIGHT_PRESSING_COLUMNS_OUTER)
#define __EGBS_BSE_EX_HIGHLIGHT_PRESSING_ROWS_INNER		0x00000004L
#define __EGBS_BSE_EX_HIGHLIGHT_PRESSING_ROWS_OUTER		0x00000008L
#define __EGBS_BSE_EX_HIGHLIGHT_PRESSING_ROWS			(__EGBS_BSE_EX_HIGHLIGHT_PRESSING_ROWS_INNER|__EGBS_BSE_EX_HIGHLIGHT_PRESSING_ROWS_OUTER)
#define __EGBS_BSE_EX_HIGHLIGHT_PRESSING_ALL				(__EGBS_BSE_EX_HIGHLIGHT_PRESSING_COLUMNS|__EGBS_BSE_EX_HIGHLIGHT_PRESSING_ROWS)
#define __EGBS_BSE_EX_HIGHLIGHT_PRESSING_STAY_COLUMNS	0x00000010L
#define __EGBS_BSE_EX_HIGHLIGHT_PRESSING_STAY_ROWS		0x00000020L
#define __EGBS_BSE_EX_HIGHLIGHT_PRESSING_STAY			(__EGBS_BSE_EX_HIGHLIGHT_PRESSING_STAY_COLUMNS|__EGBS_BSE_EX_HIGHLIGHT_PRESSING_STAY_ROWS)
// enable drag-n-drop starting events (for outer rows/columns),
// perform swapping on drag-n-drop
#define __EGBS_BSE_EX_DRAG_START_COLUMNS				0x00000040L
#define __EGBS_BSE_EX_DRAG_SWAP_COLUMNS					0x00000080L
#define __EGBS_BSE_EX_DRAG_START_ROWS					0x00000100L
#define __EGBS_BSE_EX_DRAG_SWAP_ROWS					0x00000200L
#define __EGBS_BSE_EX_DRAG_COLUMNS						(__EGBS_BSE_EX_DRAG_START_COLUMNS|__EGBS_BSE_EX_DRAG_SWAP_COLUMNS)
#define __EGBS_BSE_EX_DRAG_ROWS							(__EGBS_BSE_EX_DRAG_START_ROWS|__EGBS_BSE_EX_DRAG_SWAP_ROWS)
#define __EGBS_BSE_EX_DRAG_START_ANY_MASK				(__EGBS_BSE_EX_DRAG_START_COLUMNS|__EGBS_BSE_EX_DRAG_START_ROWS)
// perform removing on drag-n-drop
#define __EGBS_BSE_EX_DRAG_REMOVE_COLUMNS				0x00000400L
#define __EGBS_BSE_EX_DRAG_REMOVE_ROWS					0x00000800L

#define __EGWS_BSE_EX_DEFAULT 0

class __PROF_UIS_API CExtGridWnd
	: public CExtGridBaseWnd
	, public CExtGridDataProvider::IDataProviderEvents
{
// Construction
public:
	DECLARE_DYNCREATE( CExtGridWnd );
	CExtGridWnd();

// Attributes
public:
	HCURSOR m_hCursorOuterDragOK, m_hCursorOuterDragCancel;
protected:
	CExtGridDataProvider * m_pDataProvider;
	DWORD m_dwBseStyle, m_dwBseStyleEx;
#ifdef _DEBUG
	bool m_bDebugModifyingColumns:1, m_bDebugModifyingRows:1;
#endif // _DEBUG
	LONG m_nHelperLastEditedColNo, m_nHelperLastEditedRowNo;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtGridWnd)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtGridWnd();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext & dc ) const;
#endif // _DEBUG

	virtual DWORD BseGetStyle() const;
	virtual DWORD BseModifyStyle(
		DWORD dwStyleAdd,
		DWORD dwStyleRemove = 0L,
		bool bRedraw = true
		);
	virtual DWORD BseGetStyleEx() const;
	virtual DWORD BseModifyStyleEx(
		DWORD dwStyleExAdd,
		DWORD dwStyleExRemove = 0L,
		bool bRedraw = true
		);

	// just test/set __EGWS_EX_EXTERNAL_DATA ex-style
	bool ExternalDataGet() const;
	void ExternalDataSet( bool bExternalData );

	virtual CExtGridDataProvider & OnGridQueryDataProvider();
	const CExtGridDataProvider & OnGridQueryDataProvider() const;

	virtual LONG OuterColumnCountLeftSet(
		LONG nCount = 0L,
		bool bRedraw = true
		);
	virtual LONG OuterColumnCountRightSet(
		LONG nCount = 0L,
		bool bRedraw = true
		);
	virtual LONG OuterRowCountTopSet(
		LONG nCount = 0L,
		bool bRedraw = true
		);
	virtual LONG OuterRowCountBottomSet(
		LONG nCount = 0L,
		bool bRedraw = true
		);
	virtual bool ColumnInsert(
		LONG nColNo, // -1 or greater than count - append
		LONG nColInsertCount = 1L,
		bool bRedraw = true
		);
	virtual bool RowInsert(
		LONG nRowNo, // -1 or greater than count - append
		LONG nRowInsertCount = 1L,
		bool bRedraw = true
		);
	virtual LONG ColumnRemove(
		LONG nColNo,
		LONG nColRemoveCount = 1L, // -1 - remove up to end (truncate)
		bool bRedraw = true
		);
	virtual LONG RowRemove(
		LONG nRowNo,
		LONG nRowRemoveCount = 1L, // -1 - remove up to end (truncate)
		bool bRedraw = true
		);
#ifdef _DEBUG
	virtual LONG ColumnCountSet(
		LONG nCount = -1L,
		bool bRedraw = true
		);
	virtual LONG RowCountSet(
		LONG nCount = -1L,
		bool bRedraw = true
		);
#endif // _DEBUG
	virtual LONG ColumnCountGet() const;
	virtual LONG RowCountGet() const;

	bool GridColumnDefaultValueBind(
		LONG nColNo = -1L, // if -1L - default value for all columns
		INT nColType = 0,
		CExtGridCell * pCell = NULL // if NULL - remove default value
		);
	bool GridRowDefaultValueBind(
		LONG nRowNo = -1L, // if -1L - default value for all rows
		INT nRowType = 0,
		CExtGridCell * pCell = NULL // if NULL - remove default value
		);
	bool GridColumnDefaultValueUnbind(
		LONG nColNo = -1L, // if -1L - default value for all columns
		INT nColType = 0
		);
	bool GridRowDefaultValueUnbind(
		LONG nRowNo = -1L, // if -1L - default value for all rows
		INT nRowType = 0
		);
	void GridColumnDefaultValueUnbindAll();
	void GridRowDefaultValueUnbindAll();
	CExtGridCell * GridColumnDefaultValueGet(
		LONG nColNo,
		INT nColType = 0
		);
	const CExtGridCell * GridColumnDefaultValueGet(
		LONG nColNo,
		INT nColType = 0
		) const;
	CExtGridCell * GridRowDefaultValueGet(
		LONG nRowNo,
		INT nRowType = 0
		);
	const CExtGridCell * GridRowDefaultValueGet(
		LONG nRowNo,
		INT nRowType = 0
		) const;
	
	CExtGridCell * GridCellGet(
		const CExtGridHitTestInfo & htInfo,
		CRuntimeClass * pInitRTC = NULL,
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true // false - use row default value (only when bAutoFindValue=true)
		);
	const CExtGridCell * GridCellGet(
		const CExtGridHitTestInfo & htInfo,
		CRuntimeClass * pInitRTC = NULL,
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true // false - use row default value (only when bAutoFindValue=true)
		) const;
	virtual CExtGridCell * GridCellGet(
		LONG nColNo,
		LONG nRowNo,
		INT nColType = 0, // -1 - nColNo is fixed column at left, 1 - at right, 0 - data cell
		INT nRowType = 0, // -1 - nRowNo is fixed column at top, 1 - at bottom, 0 - data cell
		CRuntimeClass * pInitRTC = NULL,
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true // false - use row default value (only when bAutoFindValue=true)
		);
	virtual bool GridCellSet(
		LONG nColNo,
		LONG nRowNo,
		const CExtGridCell * pCell = NULL, // NULL - clear (make empty)
		LONG nColCount = 1L,
		LONG nRowCount = 1L,
		INT nColType = 0, // -1 - nColNo is fixed column at left, 1 - at right, 0 - data cell
		INT nRowType = 0, // -1 - nRowNo is fixed column at top, 1 - at bottom, 0 - data cell
		bool bRedraw = true
		);
	const CExtGridCell * GridCellGet(
		LONG nColNo,
		LONG nRowNo,
		INT nColType = 0, // -1 - nColNo is fixed column at left, 1 - at right, 0 - data cell
		INT nRowType = 0, // -1 - nRowNo is fixed column at top, 1 - at bottom, 0 - data cell
		CRuntimeClass * pInitRTC = NULL,
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true // false - use row default value (only when bAutoFindValue=true)
		) const;
	CExtGridCell * GridCellGetOuterAtTop(
		LONG nColNo,
		LONG nRowNo,
		CRuntimeClass * pInitRTC = NULL,
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true // false - use row default value (only when bAutoFindValue=true)
		);
	const CExtGridCell * GridCellGetOuterAtTop(
		LONG nColNo,
		LONG nRowNo,
		CRuntimeClass * pInitRTC = NULL,
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true // false - use row default value (only when bAutoFindValue=true)
		) const;
	CExtGridCell * GridCellGetOuterAtBottom(
		LONG nColNo,
		LONG nRowNo,
		CRuntimeClass * pInitRTC = NULL,
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true // false - use row default value (only when bAutoFindValue=true)
		);
	const CExtGridCell * GridCellGetOuterAtBottom(
		LONG nColNo,
		LONG nRowNo,
		CRuntimeClass * pInitRTC = NULL,
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true // false - use row default value (only when bAutoFindValue=true)
		) const;
	CExtGridCell * GridCellGetOuterAtLeft(
		LONG nColNo,
		LONG nRowNo,
		CRuntimeClass * pInitRTC = NULL,
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true // false - use row default value (only when bAutoFindValue=true)
		);
	const CExtGridCell * GridCellGetOuterAtLeft(
		LONG nColNo,
		LONG nRowNo,
		CRuntimeClass * pInitRTC = NULL,
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true // false - use row default value (only when bAutoFindValue=true)
		) const;
	CExtGridCell * GridCellGetOuterAtRight(
		LONG nColNo,
		LONG nRowNo,
		CRuntimeClass * pInitRTC = NULL,
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true // false - use row default value (only when bAutoFindValue=true)
		);
	const CExtGridCell * GridCellGetOuterAtRight(
		LONG nColNo,
		LONG nRowNo,
		CRuntimeClass * pInitRTC = NULL,
		bool bAutoFindValue = true, // auto find row/column default value (only when pInitRTC=NULL)
		bool bUseColumnDefaultValue = true // false - use row default value (only when bAutoFindValue=true)
		) const;
	bool GridCellSetOuterAtTop(
		LONG nColNo,
		LONG nRowNo,
		const CExtGridCell * pCell = NULL, // NULL - clear (make empty)
		LONG nColCount = 1L,
		LONG nRowCount = 1L,
		bool bRedraw = true
		);
	bool GridCellSetOuterAtBottom(
		LONG nColNo,
		LONG nRowNo,
		const CExtGridCell * pCell = NULL, // NULL - clear (make empty)
		LONG nColCount = 1L,
		LONG nRowCount = 1L,
		bool bRedraw = true
		);
	bool GridCellSetOuterAtLeft(
		LONG nColNo,
		LONG nRowNo,
		const CExtGridCell * pCell = NULL, // NULL - clear (make empty)
		LONG nColCount = 1L,
		LONG nRowCount = 1L,
		bool bRedraw = true
		);
	bool GridCellSetOuterAtRight(
		LONG nColNo,
		LONG nRowNo,
		const CExtGridCell * pCell = NULL, // NULL - clear (make empty)
		LONG nColCount = 1L,
		LONG nRowCount = 1L,
		bool bRedraw = true
		);
	virtual bool GridCellRectsGet(
		LONG nColNo,
		LONG nRowNo,
		INT nColType,
		INT nRowType,
		RECT * pRectCell,
		RECT * pRectCellExtra = NULL,
		RECT * pRectText = NULL,
		RECT * pRectTextArea = NULL,
		RECT * pRectIcon = NULL,
		RECT * pRectCheck = NULL,
		RECT * pRectButtonEllipsis = NULL,
		RECT * pRectButtonDropdown = NULL,
		RECT * pRectButtonUp = NULL,
		RECT * pRectButtonDown = NULL,
		RECT * pRectFocusArrow = NULL,
		RECT * pRectSortArrow = NULL
		) const;
protected:
	virtual void OnDataProviderSortEnter(
		bool bColumns // true = sorting/swapping columns, false - rows
		);
	virtual void OnDataProviderSortLeave(
		bool bColumns // true = sorting/swapping columns, false - rows
		);
	virtual void OnDataProviderSwapSeries(
		bool bColumns, // true = sorting/swapping columns, false - rows
		LONG nRowColNo1,
		LONG nRowColNo2,
		LONG nSwapCounter
		);
	virtual void OnDataProviderSwapDroppedSeries(
		bool bColumns, // true = swapping columns, false - rows
		LONG nRowColNoSrc,
		LONG nRowColNoDropBefore
		);
	virtual void OnGridSyncCellSortArrows(
		bool bSortColumns,
		const CExtGridDataSortOrder & _gdso,
		bool bSetup
		);
public:
	virtual bool GridSortOrderGet(
		bool bSortColumns,
		CExtGridDataSortOrder & _gdsoCurrent
		) const;
	virtual bool GridSortOrderSetup(
		bool bSortColumns,
		const CExtGridDataSortOrder & _gdsoUpdate,
		bool bUpdateExistingSortOrder = false,
		bool bInvertIntersectionSortOrder = true,
		bool bRedraw = true
		);
	virtual void OnGbwPaintCell(
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
		) const;
	virtual void OnGbwHoverCell(
		const CExtGridHitTestInfo & htInfoPrevious,
		const CExtGridHitTestInfo & htInfoCurrent,
		bool bHoverEnter,
		bool bHoverLeave
		);
	virtual CExtGridHitTestInfo & HitTest(
		CExtGridHitTestInfo & htInfo,
		bool bReAlignCellResizing,
		bool bSupressZeroTopCellResizing,
		bool bComputeOuterDropAfterState = false
		) const;
	virtual bool OnGbwAnalyzeCellMouseClickEvent(
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nRepCnt, // 0 - button up, 1 - signle click, 2 - double click, 3 - post single click & begin editing
		UINT nFlags, // mouse event flags
		CPoint point // mouse pointer in client coordinates
		);
	virtual bool OnGbwAnalyzeCellKeyEvent(
		bool bKeyDownEvent, // true - key-down event, false - key-up event
		UINT nChar, // virtual key code
		UINT nRepCnt, // key-down/key-up press count
		UINT nFlags // key-down/key-up event flags
		);
	virtual bool OnGbwBeginEdit(
		LONG nVisibleColNo,
		LONG nVisibleRowNo,
		LONG nColNo,
		LONG nRowNo,
		INT nColType,
		INT nRowType,
		const RECT & rcCellExtra,
		const RECT & rcCell,
		const RECT & rcInplaceControl,
		bool bContinueMsgLoop = true
		);
	virtual bool OnGbwSetCursor(
		const CExtGridHitTestInfo & htInfo
		);

	virtual bool OnSiwCacheChanged(
		const CExtScrollItemCacheInfo & _sciNew,
		const CExtScrollItemCacheInfo & _sciOld
		);
	virtual bool OnSiwQueryVirtualBOF( bool bHorz ) const;
	virtual bool OnSiwQueryVirtualEOF( bool bHorz ) const;
	virtual bool OnSiwVirtualEndTestH( LONG nColNo, LPARAM lParam ) const;
	virtual bool OnSiwVirtualEndTestV( LONG nRowNo, LPARAM lParam ) const;
	virtual void OnGbwResizingStateApply(
		bool bHorz,
		LONG nItemNo,
		INT nItemExtent
		);
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
	virtual bool OnGbwCanResizeColumn(
		LONG nColNo,
		INT * p_nExtentMin = NULL,
		INT * p_nExtentMax = NULL
		);
	virtual bool OnGbwCanResizeRow(
		LONG nRowNo,
		INT * p_nExtentMin = NULL,
		INT * p_nExtentMax = NULL
		);

	virtual bool OnGbwTooltip(
		const CExtGridHitTestInfo & htInfo
		);
	virtual bool OnGbwExpand(
		const CExtGridHitTestInfo & htInfo,
		UINT nDelayMilliseconds
		);
	virtual bool OnGbwPaintExpandedItemContent(
		CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA & picd
		) const;
	virtual bool OnGridTrackCellButton(
		CExtGridCell * pCell,
		const CExtGridHitTestInfo & htInfo
		);
	virtual CSize OnGridQueryStartDragOffset(
		const CExtGridHitTestInfo * pHtInfo = NULL
		) const;
	virtual const CExtGridHitTestInfo & OnGridQueryPressingCellInfo() const;
	virtual void OnGridTrackCellMousePressing(
		CExtGridCell * pCell,
		const CExtGridHitTestInfo & htInfo,
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nFlags, // mouse event flags
		bool bAllowStartDragEvent = true,
		CExtGridDataSortOrder * p_gdsoUpdate = NULL,
		bool bUpdateMultipleSort = false,
		bool bSortColumns = false
		);
	virtual void OnGridTrackOuterCellDND(
		CExtGridCell * pCell,
		const CExtGridHitTestInfo & htInfo,
		UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
		UINT nFlags // mouse event flags
		);
	virtual void OnGridOuterDragComplete(
		const CExtGridHitTestInfo & htInfoDrag,
		const CExtGridHitTestInfo & htInfoDrop
		);
	virtual void OnGridOuterDragOut(
		const CExtGridHitTestInfo & htInfo
		);
	virtual void OnGridCalcOuterDropTarget(
		const CExtGridHitTestInfo & htInfoDrag,
		CExtGridHitTestInfo & htInfoDrop,
		CPoint point
		);
	virtual void OnGridPaintDraggedCell(
		CDC & dc,
		LONG nVisibleColNo,
		LONG nVisibleRowNo,
		LONG nColNo,
		LONG nRowNo,
		const RECT & rcCell,
		DWORD dwAreaFlags
		) const;

protected:
	virtual bool OnGbwProcessVkBeginEdit( UINT nChar );

public:
	INT GridIconGetCount() const;
	CExtCmdIcon * GridIconGetAt( INT nIdx );
	const CExtCmdIcon * GridIconGetAt( INT nIdx ) const;
	INT GridIconInsert( // returns index or -1
		CExtCmdIcon * pIcon,
		INT nIdx = -1, // append
		bool bCopyIcon = true
		);
	INT GridIconInsert( // returns index or -1
		const CExtCmdIcon * pIcon,
		INT nIdx = -1, // append
		bool bCopyIcon = true
		);
	INT GridIconRemove(
		INT nIdx = 0,
		INT nCountToRemove = -1 // all
		);

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtGridWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; // class CExtGridWnd

#endif // (!defined __EXT_MFC_NO_GRIDWND)

#endif // (!defined __EXT_MFC_NO_GRIDBASEWND)

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined __EXT_GRIDWND_H

