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

#if (!defined __EXTTOOLBOXWND_H)
#define __EXTTOOLBOXWND_H

#if (!defined __EXT_MFC_NO_TOOLBOX_CTRL)

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

//#ifndef __AFXTEMPL_H__
//	#include <AfxTempl.h>
//#endif

#if (!defined __ExtCmdManager_H)
	#include <ExtCmdManager.h>
#endif

#if (!defined __EXT_EXTCONTENTEXPANDWND_H)
	#include <ExtContentExpandWnd.h>
#endif

#if (!defined __EXT_MOUSECAPTURESINK_H)
	#include <../Src/ExtMouseCaptureSink.h>
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// toolbox item styles

#define __TBWI_VISIBLE						0x00000001L
#define __TBWI_ENABLED						0x00000002L
#define __TBWI_HOVER						0x00000004L // internal
#define __TBWI_SELECTED						0x00000008L // internal
#define __TBWI_ACTIVE						0x00000010L // internal (child has focus)
#define __TBWI_ACTIVE2						0x00000020L // internal (child was under focus)
#define __TBWI_PRESSED						0x00000040L // internal
#define __TBWI_HAS_BUTTON_UP				0x00000080L // internal
#define __TBWI_HAS_BUTTON_DOWN				0x00000100L // internal
#define __TBWI_LISTBOX_VIEW					0x00000200L
#define __TBWI_WRAP							0x00000400L // internal
#define __TBWI_DRAG_SOURCE					0x00000800L
#define __TBWI_DRAG_TARGET					0x00001000L
#define __TBWI_EDITABLE						0x00002000L
#define __TBWI_USER_STYLE_17				0x00004000L
#define __TBWI_USER_STYLE_16				0x00008000L
#define __TBWI_USER_STYLE_15				0x00010000L
#define __TBWI_USER_STYLE_14				0x00020000L
#define __TBWI_USER_STYLE_13				0x00040000L
#define __TBWI_USER_STYLE_12				0x00080000L
#define __TBWI_USER_STYLE_11				0x00100000L
#define __TBWI_USER_STYLE_10				0x00200000L
#define __TBWI_USER_STYLE_09				0x00400000L
#define __TBWI_USER_STYLE_08				0x00800000L
#define __TBWI_USER_STYLE_07				0x01000000L
#define __TBWI_USER_STYLE_06				0x02000000L
#define __TBWI_USER_STYLE_05				0x04000000L
#define __TBWI_USER_STYLE_04				0x08000000L
#define __TBWI_USER_STYLE_03				0x10000000L
#define __TBWI_USER_STYLE_02				0x20000000L
#define __TBWI_USER_STYLE_01				0x40000000L
#define __TBWI_USER_STYLE_00				0x80000000L

//#define __TBWI_DEF_STYLES (__TBWI_VISIBLE|__TBWI_ENABLED|__TBWI_LISTBOX_VIEW|__TBWI_EDITABLE)
#define __TBWI_DEF_STYLES (__TBWI_VISIBLE|__TBWI_ENABLED|__TBWI_LISTBOX_VIEW)
#define __TBWI_DEF_ANY_BUTTONS (__TBWI_HAS_BUTTON_UP|__TBWI_HAS_BUTTON_DOWN)
#define __TBWI_DRAG_ANY (__TBWI_DRAG_SOURCE|__TBWI_DRAG_TARGET)

// toolbox control styles

#define __TBWS_TOOLTIPS_IN_GROUPS			0x00000001L
#define	__TBWS_TOOLTIPS_IN_ICONS_VIEW		0x00000002L
#define __TBWS_TOOLTIPS_IN_LISTBOX_VIEW		0x00000004L
#define __TBWS_EXPAND_LISTBOX_ITEMS			0x00000008L
#define __TBWS_EXPAND_GROUPS				0x00000010L
#define __TBWS_DND_GROUPS					0x00000020L // can start drag-n-drop groups
#define __TBWS_DND_ITEMS					0x00000040L // can start drag-n-drop items
#define __TBWS_USER_STYLE_24				0x00000080L
#define __TBWS_USER_STYLE_23				0x00000100L
#define __TBWS_USER_STYLE_22				0x00000200L
#define __TBWS_USER_STYLE_21				0x00000400L
#define __TBWS_USER_STYLE_20				0x00000800L
#define __TBWS_USER_STYLE_19				0x00001000L
#define __TBWS_USER_STYLE_18				0x00002000L
#define __TBWS_USER_STYLE_17				0x00004000L
#define __TBWS_USER_STYLE_16				0x00008000L
#define __TBWS_USER_STYLE_15				0x00010000L
#define __TBWS_USER_STYLE_14				0x00020000L
#define __TBWS_USER_STYLE_13				0x00040000L
#define __TBWS_USER_STYLE_12				0x00080000L
#define __TBWS_USER_STYLE_11				0x00100000L
#define __TBWS_USER_STYLE_10				0x00200000L
#define __TBWS_USER_STYLE_09				0x00400000L
#define __TBWS_USER_STYLE_08				0x00800000L
#define __TBWS_USER_STYLE_07				0x01000000L
#define __TBWS_USER_STYLE_06				0x02000000L
#define __TBWS_USER_STYLE_05				0x04000000L
#define __TBWS_USER_STYLE_04				0x08000000L
#define __TBWS_USER_STYLE_03				0x10000000L
#define __TBWS_USER_STYLE_02				0x20000000L
#define __TBWS_USER_STYLE_01				0x40000000L
#define __TBWS_USER_STYLE_00				0x80000000L

#define __TBWS_TOOLTIPS_ALL \
	(	__TBWS_TOOLTIPS_IN_GROUPS \
	|	__TBWS_TOOLTIPS_IN_ICONS_VIEW \
	|	__TBWS_TOOLTIPS_IN_LISTBOX_VIEW \
	)

#define __TBWS_EXPAND_ALL \
	(	__TBWS_EXPAND_LISTBOX_ITEMS \
	|	__TBWS_EXPAND_GROUPS \
	)

#define __TBWS_DND_ANY \
	(	__TBWS_DND_GROUPS \
	|	__TBWS_DND_ITEMS \
	)

#define __TBWS_DEF_STYLES \
	(	__TBWS_TOOLTIPS_ALL \
	|	__TBWS_EXPAND_ALL \
	)

// get next item code

#define __TBCGN_SIBLING			1  // get sibling using positive or negative index
#define __TBCGN_CHILD			2  // get child using positive index
#define __TBCGN_PARENT			3  // get parent (index not used, bVisibleOnly is not used)
#define __TBCGN_FIRST_SIBLING	4  // get first sibling (index not used)
#define __TBCGN_LAST_SIBLING	5  // get last sibling (index not used)
#define __TBCGN_FIRST_CHILD		6  // get first child (index not used)
#define __TBCGN_LAST_CHILD		7  // get last child (index not used)
#define __TBCGN_SIBLING_NR		8  // get sibling for next/prev row (index greater or less zero)
#define __TBCGN_IN_ROW_FIRST	9  // get first in row (index not used)
#define __TBCGN_IN_ROW_LAST		10 // get last in row (index not used)
#define __TBCGN_PAGE_UP			11 // get for one page up (index not used, should be visible)
#define __TBCGN_PAGE_DOWN		12 // get for one page down (index not used, should be visible)

/////////////////////////////////////////////////////////////////////////////
// CExtToolBoxWnd window

class __PROF_UIS_API CExtToolBoxWnd
	: public CWnd
	, public CExtMouseCaptureSink
{
// Construction
public:
	CExtToolBoxWnd();

	DECLARE_DYNCREATE( CExtToolBoxWnd );

// Attributes
public:

	class __PROF_UIS_API TOOLBOX_ITEM_DATA
	{
	protected:
		CExtToolBoxWnd * m_pToolBoxWnd;
		
		TOOLBOX_ITEM_DATA
			  * m_pItemParent
			, * m_pItemChild
			, * m_pItemNext
			, * m_pItemPrev
			;
		
		CExtCmdIcon m_icon;
		CExtSafeString m_sText;
		LPARAM m_lParam;
		DWORD m_dwItemStyle;
		CSize m_sizeLastMeasured;
		LONG m_nHorizontalOffset, m_nVerticalOffset;

	public:
		TOOLBOX_ITEM_DATA( CExtToolBoxWnd * pToolBoxWnd )
		{
			ASSERT_VALID( pToolBoxWnd );
			m_pToolBoxWnd = pToolBoxWnd;
			m_pItemParent = NULL;
			m_pItemChild = NULL;
			m_pItemNext = NULL;
			m_pItemPrev = NULL;
			m_dwItemStyle = __TBWI_DEF_STYLES;
			m_sizeLastMeasured.cx = m_sizeLastMeasured.cy = 0;
			m_nHorizontalOffset = m_nVerticalOffset = 0L;
		};
		~TOOLBOX_ITEM_DATA()
		{
			DestroyChilds();
		};

		HICON IconGet(
			CSize * pSize = NULL
			);
		void IconSet( HICON hIcon, bool bCopyIcon );

		__EXT_MFC_SAFE_LPCTSTR TextGet() const;
		void TextSet( __EXT_MFC_SAFE_LPCTSTR sText );

		const TOOLBOX_ITEM_DATA * ItemGetNext(
			INT nGetCode,
			LONG nIndex = 1,
			bool bVisibleOnly = true
			) const;
		TOOLBOX_ITEM_DATA * ItemGetNext(
			INT nGetCode,
			LONG nIndex = 1,
			bool bVisibleOnly = true
			);
		LONG ItemGetIndexInRow(
			bool bVisibleOnly = true
			) const;
		LONG ItemGetSiblingIndex(
			bool bVisibleOnly = true
			) const;

		DWORD GetItemStyle() const;
		DWORD ModifyItemStyle(
			DWORD dwStylesAdd,
			DWORD dwStylesRemove = 0L
			);

		LONG GetChildCount(
			bool bVisibleOnly = true
			) const;
		LONG GetSiblingCount(
			bool bVisibleOnly = true
			) const;

		void DestroyChilds();
		void DestroyItem(
			bool bSyncPrevNext
			);

		CExtToolBoxWnd * GetToolBoxWnd()
		{
			ASSERT_VALID( m_pToolBoxWnd );
			return m_pToolBoxWnd;
		}
		const CExtToolBoxWnd * GetToolBoxWnd() const
		{
			ASSERT_VALID( m_pToolBoxWnd );
			return m_pToolBoxWnd;
		}

		CSize GetLastMeasuredSize() const
		{
			return m_sizeLastMeasured;
		};

		CSize MeasureItem( CDC & dc );
		
		void DrawItem(
			CDC & dc,
			const CRect & rcItem
			);
		
		CFont * ItemGetFont();

		INT ItemGetActiveDirCode() const
		{
			if( GetItemStyle() & __TBWI_ACTIVE )
				return 0;
			const TOOLBOX_ITEM_DATA * pTBCI = m_pItemPrev;
			for( ; pTBCI != NULL; pTBCI = pTBCI->m_pItemPrev )
			{
				if( pTBCI->GetItemStyle() & __TBWI_ACTIVE )
					return -1;
			}
			return 1;
		}

		LONG SetVerticalOffset( LONG nVerticalOffset )
		{
			LONG nVerticalOffsetOld = m_nVerticalOffset;
			m_nVerticalOffset = nVerticalOffset;
			return nVerticalOffsetOld;
		}
		LONG GetVerticalOffset() const
		{
			return m_nVerticalOffset;
		}
		LONG SetHorizontalOffset( LONG nHorizontalOffset )
		{
			LONG nHorizontalOffsetOld = m_nHorizontalOffset;
			m_nHorizontalOffset = nHorizontalOffset;
			return nHorizontalOffsetOld;
		}
		LONG GetHorizontalOffset() const
		{
			return m_nHorizontalOffset;
		}

		const TOOLBOX_ITEM_DATA * FindSelectedChild() const;
		TOOLBOX_ITEM_DATA * FindSelectedChild();

		bool CanExpandItem() const;
		bool CanShowToolTip() const;
		bool IsContentPartiallyVisible() const;
		bool ItemIsVisible() const;

		LPARAM LPARAM_Get() const
		{
			return m_lParam;
		}
		void LPARAM_Set( LPARAM lParam = 0L )
		{
			m_lParam = lParam;
		}

		friend class CExtToolBoxWnd;
	}; // class TOOLBOX_ITEM_DATA

	class __PROF_UIS_API CItemTextEditWnd : public CEdit
	{
		TOOLBOX_ITEM_DATA * m_pTBCI;
		bool m_bInEndEdit:1;
	public:

		CItemTextEditWnd(
			TOOLBOX_ITEM_DATA * pTBCI
			)
			: m_pTBCI( pTBCI )
			, m_bInEndEdit( false )
		{
			ASSERT( m_pTBCI != NULL );
		}
		
		// CEdit syntax of Create() method
		BOOL Create(
			DWORD dwStyle,
			const RECT & rect,
			CWnd * pParentWnd,
			UINT nID
			);
		virtual void PostNcDestroy();

	protected:

		void DoEndEdit( bool bUpdateItemText );
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	
	}; // class CItemTextEditWnd

private:
	CToolTipCtrl m_wndToolTip;
	CExtContentExpandWnd m_wndContentExpand;
	static bool g_bToolBoxWndClassRegistered;
	bool m_bDirectCreateCall:1;
	bool m_bDelayRecalcLayout:1;
	bool m_bPushedBtnUp:1, m_bPushedBtnDown:1;
	bool m_bTrackingBtnUp:1, m_bTrackingBtnDown:1;
	DWORD m_dwToolBoxWndStyle;
	TOOLBOX_ITEM_DATA
		* m_pItemRoot,
		* m_pItemActive, * m_pItemActive2,
		* m_pItemTrackPressed,
		* m_pItemTrackHover;
	LONG
		m_nHeightUpper, m_nHeightLower,
		m_nHeightChild, m_nHeightChild2,
		m_nSelRowHeight;
	LONG m_nWidth;
	LONG m_nScrollStepNo,
		m_nScrollPos, m_nScrollPos2,
		m_nScrollSize, m_nScrollSize2;
	LONG m_nOffsetLower;
	CRect m_rcActiveChildArea, m_rcActiveChildArea2;
	CRgn m_rgnActiveChildArea, m_rgnActiveChildArea2;
	CRect m_rcBtnUp, m_rcBtnUp2, m_rcBtnDown, m_rcBtnDown2;
	LONG m_nExpandAnimationPercent;
	HWND m_hWndEditor;
	CPoint m_ptStartLeftBtnTrack;

// Operations
public:

	bool RegisterToolBoxWndClass();
	
	void UpdateToolBoxWnd(
		bool bUpdateCtrlNow,
		bool bEnsureVisibleSelection = false
		);
	
	BOOL Create(
		CWnd * pParentWnd,
		const RECT & rcWnd = CRect( 0, 0, 0, 0 ),
		UINT nDlgCtrlID = UINT( IDC_STATIC ),
		DWORD dwWindowStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
		DWORD dwToolBoxWndStyle = __TBWS_DEF_STYLES,
		CCreateContext * pContext = NULL
		);
	
	DWORD GetToolBoxWndStyle() const;
	DWORD ModifyToolBoxWndStyle(
		DWORD dwStylesAdd,
		DWORD dwStylesRemove = 0L,
		bool bUpdateCtrlNow = false
		);

	CToolTipCtrl & GetToolTipCtrl()
	{
		return m_wndToolTip;
	}
	const CToolTipCtrl & GetToolTipCtrl() const
	{
		return m_wndToolTip;
	}
	CExtContentExpandWnd & GetContentExpandWnd()
	{
		return m_wndContentExpand;
	}
	const CExtContentExpandWnd & GetContentExpandWnd() const
	{
		return m_wndContentExpand;
	}
	
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * ActivateNextGroup(
		bool bPreviousGroup
		);
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * ActivateFirstGroup(
		bool bPreviousGroup
		);
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * ActivateNextItem(
		bool bPreviousItem,
		bool bJumpRow,
		bool bSkipInIconViewMode,
		bool bSkipInListViewMode
		);
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * ActivateFirstItem(
		bool bPreviousItem
		);
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * ActivateNextPage(
		bool bPreviousPage
		);

	HWND GetEditorHWND() const
	{
		return
			( const_cast
				< CExtToolBoxWnd * >
				( this )
			) -> m_hWndEditor;
	}

	CRect GetActiveChildArea()
	{
		return m_rcActiveChildArea;
	}
	void ScrollPosSet(
		LONG nScrollPos = 0L,
		bool bUpdateCtrlNow = false
		)
	{
		m_nScrollPos = nScrollPos;
		if( m_nScrollPos < 0 )
			m_nScrollPos = 0;
		else if( m_nScrollPos > m_nScrollSize )
			m_nScrollPos = m_nScrollSize;
		UpdateToolBoxWnd( bUpdateCtrlNow );
	}
	LONG ScrollPosGet() const
	{
		return m_nScrollPos;
	}
	LONG ScrollSizeGet() const
	{
		return m_nScrollSize;
	}

	bool ItemBeginEdit(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI
		);

protected:
	bool _CreateHelper();
	void _RecalcLayoutImpl(
		bool bEnsureVisibleSelection = false
		);
	bool _IsScrollEnabled(
		bool bByButtonUp
		) const;
	bool _IsScrollEnabled2(
		bool bByButtonUp
		) const;
	bool _ProcessMouseWheel( UINT nFlags, short zDelta, bool bRealOwnMsg );

	virtual void OnToolBoxWndEraseEntire(
		CDC & dc,
		const CRect & rcClient,
		const CRect & rcActiveChildArea,
		CRgn & rgnActiveChildArea,
		const CRect & rcActiveChildArea2,
		CRgn & rgnActiveChildArea2
		);
	virtual void OnToolBoxWndDrawEntire(
		CDC & dc,
		const CRect & rcClient,
		const CRect & rcActiveChildArea,
		CRgn & rgnActiveChildArea,
		const CRect & rcActiveChildArea2,
		CRgn & rgnActiveChildArea2
		);
	virtual HICON OnToolBoxWndQueryItemIcon(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
		CSize * pSize = NULL
		);
	virtual __EXT_MFC_SAFE_LPCTSTR OnToolBoxWndQueryItemText(
		const CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI
		);
	virtual CFont * OnToolBoxWndQueryItemFont(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI
		);
	virtual CSize OnToolBoxWndMasureItem(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
		CDC & dc
		);
	virtual void OnToolBoxWndEraseItem(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
		const CRect & rcItem,
		CDC & dc
		);
	virtual void OnToolBoxWndDrawItem(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
		const CRect & rcItem,
		CDC & dc
		);
	virtual void OnToolBoxWndEraseScrollButton(
		CDC & dc,
		const CRect & rcBtn,
		bool bUpButton,
		bool bEnabled,
		bool bPushed
		);
	virtual void OnToolBoxWndDrawScrollButton(
		CDC & dc,
		const CRect & rcBtn,
		bool bUpButton,
		bool bEnabled,
		bool bPushed
		);

	virtual void OnToolBoxWndItemInsert(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI
		);
	virtual void OnToolBoxWndItemRemove(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI
		);
	virtual void OnToolBoxWndItemActivate(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_Old,
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_New
		);
	virtual void OnToolBoxWndItemSelChange(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_Old,
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_New
		);
	virtual void OnToolBoxWndItemHoverChange(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_Old,
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_New
		);

	virtual LONG OnToolBoxWndMeasureScrollStepSize(
		bool bByUpButton,
		LONG nScrollStepNo
		);
	virtual UINT OnToolBoxWndMeasureScrollTimerPeriod(
		bool bByUpButton
		);

	virtual bool OnToolBoxWndCanHandleHover() const;

public:
	virtual bool OnToolBoxWndDeactivateEditor(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
		bool bUpdateDeactivation
		);

protected:
	virtual HWND OnToolBoxWndStartItemEditor(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI
		);
	virtual void OnToolBoxWndCancelItemEditor(
		HWND hWndEditor
		);

	virtual void OnToolBoxWndDoItemTextChanging(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
		CExtSafeString & sItemText,
		__EXT_MFC_SAFE_LPCTSTR sNewText
		);

	virtual void OnToolBoxWndStartItemDrag(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
		CPoint ptClientStart,
		CPoint ptClientCurrent,
		UINT nMouseMoveFlags
		);
	virtual bool OnToolBoxWndCanDoItemDrag(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
		CPoint ptClientStart,
		CPoint ptClientCurrent,
		UINT nMouseMoveFlags
		);

public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtToolBoxWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	virtual void PostNcDestroy();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtToolBoxWnd();

	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * ItemInsert(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI_Parent = NULL, // default - root
		__EXT_MFC_SAFE_LPCTSTR sText = NULL,
		HICON hIcon = NULL,
		bool bCopyIcon = true,
		LONG nIndexInsertBefore = -1, // default - append
		bool bUpdateCtrlNow = false
		);
	void ItemRemove(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
		bool bChildrenOnly = false,
		bool bUpdateCtrlNow = false
		);
	void ItemRemoveAll(
		bool bUpdateCtrlNow = false
		);

	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * ItemGetRoot();
	const CExtToolBoxWnd::TOOLBOX_ITEM_DATA * ItemGetRoot() const;
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * ItemGetActive();
	const CExtToolBoxWnd::TOOLBOX_ITEM_DATA * ItemGetActive() const;
	CExtToolBoxWnd::TOOLBOX_ITEM_DATA * ItemHitTest(
		CPoint ptClient
		);
	const CExtToolBoxWnd::TOOLBOX_ITEM_DATA * ItemHitTest(
		CPoint ptClient
		) const;
	bool ItemSetActive(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI,
		bool bUpdateCtrlNow = false,
		bool bEnsureVisibleSelection = false
		);

	CRect ItemGetRect(
		CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI
		);
	CRect ItemGetRect(
		const CExtToolBoxWnd::TOOLBOX_ITEM_DATA * pTBCI
		) const
	{
		return
			(	const_cast
					< CExtToolBoxWnd * >
					( this )
				) ->
			ItemGetRect(
				const_cast
					< CExtToolBoxWnd::TOOLBOX_ITEM_DATA * >
					( pTBCI )
				);
	}

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtToolBoxWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class TOOLBOX_ITEM_DATA;
	
	afx_msg LRESULT _OnPaintExpanedItemContent(WPARAM wParam, LPARAM lParam);

#if _MFC_VER < 0x700
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#else
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
#endif

};

/////////////////////////////////////////////////////////////////////////////

#define __EXT_TOOLBOXWND_CLASS_NAME _T("ProfUIS-ToolBoxWnd")

#define __EXT_TOOLBOXWND_ITEM_MARGIN_DX 3
#define __EXT_TOOLBOXWND_ITEM_MARGIN_DY 1

#define __EXT_TOOLBOXWND_NONLIST_MARGIN_DX 6
#define __EXT_TOOLBOXWND_NONLIST_MARGIN_DY 5

#define __EXT_TOOLBOXWND_ITEM_MIN_DY_TOPLEVEL 15
#define __EXT_TOOLBOXWND_ITEM_MIN_DY_BOTTOMLEVEL 16

#define __EXT_TOOLBOXWND_ICON2TEXT_DX 3
#define __EXT_TOOLBOXWND_BTN2TEXT_DX 2
#define __EXT_TOOLBOXWND_BTN_EXTRA_DX 1

#define __EXT_TOOLBOXWND_MARGIN_BOTTOM_TOPLEVEL 1
#define __EXT_TOOLBOXWND_MARGIN_BOTTOM_BOTTOMLEVEL 0

#define __EXT_TOOLBOXWND_TIMER_ID_HOVER 101
#define __EXT_TOOLBOXWND_TIMER_ID_SCROLL 102
#define __EXT_TOOLBOXWND_TIMER_ID_DELAY_UPDATE 103

#define __EXT_TOOLBOXWND_MIN_CHILD_AREA_DX 4
#define __EXT_TOOLBOXWND_MIN_CHILD_AREA_DY 4

#define __EXT_TOOLBOXWND_SCROLL_GAP 2

#define __EXT_TOOLBOXWND_EXPAND_STEP_TIME 10
#define __EXT_TOOLBOXWND_EXPAND_STEP_PERCENT_1 4
#define __EXT_TOOLBOXWND_EXPAND_STEP_PERCENT_2 8
#define __EXT_TOOLBOXWND_EXPAND_STEP_PERCENT_SW 32

#define __EXT_TOOLBOXWND_MIN_EDITOR_DX 10
#define __EXT_TOOLBOXWND_MIN_EDITOR_DY 10

#endif // (!defined __EXT_MFC_NO_TOOLBOX_CTRL)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif /// __EXTTOOLBOXWND_H
