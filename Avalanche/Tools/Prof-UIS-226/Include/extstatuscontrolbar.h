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

#if (!defined __EXT_STATUSCONTROLBAR_H)
#define __EXT_STATUSCONTROLBAR_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExtStatusControlBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExtStatusControlBar window

class __PROF_UIS_API CExtStatusControlBar : public CStatusBar
{
// Construction
public:

	DECLARE_DYNCREATE( CExtStatusControlBar )

	CExtStatusControlBar();

// Attributes
public:

protected:
	bool m_bLockPainting:1;

	// Operations
public:
	
	int GetPaneCount() const
	{
		ASSERT_VALID( this );
		return m_nCount;
	}
	
	void SetPaneWidth(int nIndex, int nWidth)
	{
		ASSERT_VALID( this );
		_STATUSBAR_PANE_ pane;
		PaneInfoGet(nIndex, &pane);
		pane.cxText = nWidth;
		PaneInfoSet(nIndex, &pane);
	}

protected:
	struct _ED_
	{
		UINT m_nDTF;
		CExtSafeString m_strTipText;
		_ED_(
			UINT _nDTF = 0,
			__EXT_MFC_SAFE_LPCTSTR _strTipText = NULL
			)
			: m_nDTF( _nDTF )
			, m_strTipText( _strTipText == NULL ? _T("") : _strTipText )
		{
		}
		_ED_ & operator = (
			const _ED_ & other
			)
		{
			m_nDTF = other.m_nDTF;
			m_strTipText = other.m_strTipText;
			return (*this);
		}
		_ED_(
			const _ED_ & other
			)
		{
			(*this) = other;
		}
	};
	CMap < UINT, UINT, _ED_, _ED_ > m_mapIdToExtData;
public:
	UINT GetPaneDrawTextFlags( int nIndex ) const;
	void SetPaneDrawTextFlags( int nIndex, UINT _nDTF );
	
	bool AddPane(
		UINT nID,	// ID of the  pane
		int nIndex	// index of the pane
		);
	
protected:
	bool _RemovePaneImpl(
		UINT nID,	// ID of the pane
		bool bRefresh
		);
public:
	bool RemovePane(
		UINT nID	// ID of the pane
		);
protected:
	int _RemovePanesImpl(
		int nIndexToRemove,
		int nCountToRemove,
		bool bRefresh
		);
public:
	int RemovePanes(
		int nIndexToRemove,
		int nCountToRemove
		);

	bool SetPaneControl(CWnd* pWnd, UINT nID, bool bAutoDestroy)
	{
		ASSERT_VALID( this );
		return SetPaneControl( pWnd->GetSafeHwnd(), nID, bAutoDestroy);
	}
	
	bool SetPaneControl(HWND hWnd, UINT nID, bool bAutoDestroy);

	bool RemovePaneControl( UINT nID );
	
	void DrawPaneSeparatorsSet( bool bSet = true )
	{
		ASSERT_VALID( this );
		m_bDrawPaneSeparatorsInsteadOfBorders = bSet;
		RepositionControls();
		Refresh();
	}
	bool DrawPaneSeparatorsGet() const
	{
		ASSERT_VALID( this );
		return m_bDrawPaneSeparatorsInsteadOfBorders;
	}
	
	void OuterRectInFirstBandSet( bool bSet = true )
	{
		ASSERT_VALID( this );
		m_bOuterRectInFirstBand = bSet;
		Refresh();
	}
	bool OuterRectInFirstBandGet() const
	{
		ASSERT_VALID( this );
		return m_bOuterRectInFirstBand;
	}
	
	void HideTextOnDisabledPanesSet( bool bSet = true )
	{
		ASSERT_VALID( this );
		m_bHideTextOnDisabledPanes = bSet;
		Refresh();
	}
	bool HideTextOnDisabledPanesGet() const
	{
		ASSERT_VALID( this );
		return m_bHideTextOnDisabledPanes;
	}

	void EnableControl( int nIndex, bool bEnable = true )
	{
		ASSERT_VALID( this );
		UINT uItemID = GetItemID(nIndex);

		for ( int i = 0; i < m_arrPaneControls.GetSize(); i++ ){
			if( uItemID == m_arrPaneControls[i]->nID ){
				if( m_arrPaneControls[i]->hWnd && ::IsWindow(m_arrPaneControls[i]->hWnd) ) {
					::EnableWindow(m_arrPaneControls[i]->hWnd, bEnable); 
					Refresh();
				}
			}
		}
	}

	void SetPaneInfo(int nIndex, UINT nID, UINT nStyle, int cxWidth)
	{
		ASSERT_VALID( this );
		CStatusBar::SetPaneInfo(nIndex, nID, nStyle, cxWidth);
		bool bEnabled = ((nStyle&SBPS_DISABLED) == 0);
		EnableControl(nIndex, bEnabled);
	}

	void SetPaneStyle(int nIndex, UINT nStyle)
	{
		ASSERT_VALID( this );
		CStatusBar::SetPaneStyle(nIndex, nStyle);
		bool bEnabled = ((nStyle&SBPS_DISABLED) == 0);
		EnableControl(nIndex, bEnabled);
	}
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtStatusControlBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtStatusControlBar();

	UINT GetItemID(int nIndex) const;
	BOOL SetIndicators(
		const UINT * lpIDArray,
		int nIDCount
		);
	void DrawBorders( CDC * pDC, CRect & rc );

	virtual int OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		) const;
	virtual int HitTestStatusPane(
		CPoint ptClient
		) const;
	CExtSafeString GetTipText( int nPane ) const;
	void SetTipText( int nPane, __EXT_MFC_SAFE_LPCTSTR pszTipText );

	bool m_bOuterRectInFirstBand:1;
	bool m_bCompleteRepaint:1;
	bool m_bDrawPaneSeparatorsInsteadOfBorders:1;
	bool m_bHideTextOnDisabledPanes:1;
	bool m_bDoNotPaintIcons:1;
	UINT m_nDrawPaneTextFlags; // DT_SINGLELINE|DT_LEFT|DT_VCENTER
protected:
	COLORREF m_clrStatusBk, m_clrGripper, m_clrPaneRect, m_clrPaneTextNormal, m_clrPaneTextDisabled;

	void _SyncStatusBarColors();

	CFrameWnd * _GetDockingFrameImpl();

	virtual void OnPaintEntireBackground(
		CDC & dc,
		const CRect & rcBk
		);
	
	virtual void OnErasePaneBackground( // by default does nothing
		CDC & dc,
		int nPaneIdx,
		const CRect & rcPane
		);

	virtual void OnPaintPane(
		CDC & dc,
		int nPaneIdx,
		const CRect & rcPane,
		DWORD dwPaneStyle,
		HICON hIcon,
		__EXT_MFC_SAFE_LPCTSTR sPaneText, // by default it's text from the status bar control
		UINT nDrawTextFlags // by default m_nDrawPaneTextFlags is used
		);

	virtual COLORREF OnQueryPaneTextColor( // COLORREF(-1L) <- detect color automatically
		int nPaneIdx
		);

	virtual bool OnQueryGripperEnabledState() const;
	virtual void OnPaintGripper(
		CDC & dc,
		const CRect & rcGrip
		);
	virtual void OnPaintSeparator(
		CDC & dc,
		int nPaneIdxAfterSep,
		const CRect & rcPaneBefore,
		const CRect & rcPaneAfter
		);

	struct _STATUSBAR_PANE_
	{
		UINT    nID;        // IDC of indicator: 0 => normal text area
		int     cxText;     // width of string area in pixels
		//   on both sides there is a 3 pixel gap and
		//   a one pixel border, making a pane 6 pixels wider
		UINT    nStyle;     // style flags (SBPS_*)
		UINT    nFlags;     // state flags (SBPF_*)
		CExtSafeString strText;    // text in the pane
		_STATUSBAR_PANE_()
		{
			nID = cxText = nStyle = nFlags = 0;
		}
	};
	
	struct _STATUSBAR_PANE_CTRL_
	{
		HWND hWnd;
		UINT nID;
		bool bAutoDestroy;		
	};
	
	CArray < _STATUSBAR_PANE_CTRL_*, _STATUSBAR_PANE_CTRL_* > m_arrPaneControls; 
	
	_STATUSBAR_PANE_* GetPanePtr(int nIndex) const
	{
		ASSERT_VALID( this );
		ASSERT((nIndex >= 0 && nIndex < m_nCount) || m_nCount == 0);
		return ((_STATUSBAR_PANE_*)m_pData) + nIndex;
	}
	
	_STATUSBAR_PANE_ * _GetPanePtr( int nIndex ) const;
	bool PaneInfoGet(int nIndex, _STATUSBAR_PANE_* pPane) const;
	bool PaneInfoSet(int nIndex, _STATUSBAR_PANE_* pPane);
	void RepositionControls();
	
	void Refresh();

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtStatusControlBar)
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg UINT OnNcHitTest(CPoint point);
	//}}AFX_MSG
	afx_msg void OnSysColorChange();
	afx_msg void OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection);
	afx_msg LRESULT OnDisplayChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnThemeChanged( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __EXT_STATUSCONTROLBAR_H
