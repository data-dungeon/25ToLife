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

#if (! defined __EXT_TAB_PAGE_CONTAINER_WND_H)
#define __EXT_TAB_PAGE_CONTAINER_WND_H

#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_HOOK_H)
	#include "../Src/ExtHook.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExtTabCtrl.h : header file
//

#ifndef __AFXTEMPL_H__
	#include <AfxTempl.h>
#endif

#if (!defined __EXT_CMD_ICON_H)
	#include <ExtCmdIcon.h>
#endif

#if (!defined __EXT_TABWND_H)
	#include <ExtTabWnd.h>
#endif

#if (! defined __EXT_TAB_FLAT_WND_H)
	#include <ExtTabFlatWnd.h>
#endif

// tab wnd class name
#define __EXT_TAB_PAGE_CONTAINER_CLASS_NAME _T("ProfUIS-TabPageContainer")

template <class CTabImplWnd>
class CExtTabsWndImpl : public CTabImplWnd
{
public:
	CExtTabsWndImpl()
	{
		m_bReflectParentSizing = true;
	}

	~CExtTabsWndImpl()
	{
	}
	
	void OnTabWndSyncVisibility()
	{
		LONG nItemCount = CTabImplWnd::ItemGetCount();
		DWORD dwWndStyle = GetStyle();
		if( nItemCount > 0 )
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
		} // if( nItemCount > 0 )
		else
		{
			if( (dwWndStyle & WS_VISIBLE) != 0 )
				::SetWindowPos(
				m_hWnd,
				NULL, 0, 0, 0, 0,
				SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOOWNERZORDER
				|SWP_HIDEWINDOW
				);
		} // else from if( nItemCount > 0 )
	}
	
	virtual bool OnTabWndSelectionChange(
		LONG nOldItemIndex,
		LONG nNewItemIndex,
		bool bPreSelectionTest
		)
	{
		bool bRetVal =
			CTabImplWnd::OnTabWndSelectionChange(
			nOldItemIndex,
			nNewItemIndex,
			bPreSelectionTest
			);
		
		if( (!bPreSelectionTest) && nNewItemIndex >= 0 && CTabImplWnd::ItemGetCount() > 0 )
		{
			if( nOldItemIndex >= 0 )
				::ShowWindow(
				(HWND)CTabImplWnd::ItemLParamGet(nOldItemIndex),
				SW_HIDE
				);
			HWND hWndNew = (HWND)CTabImplWnd::ItemLParamGet(nNewItemIndex);
			::ShowWindow( hWndNew, SW_SHOW );
			::SetFocus( hWndNew );
			GetParent()->CWnd::RepositionBars( 0, 0xFFFF, 0x101 + nNewItemIndex );
		} // if( (!bPreSelectionTest) && nNewItemIndex >= 0 && CTabImplWnd::ItemGetCount() > 0 )

		((CExtTabPageContainerWnd*)GetParent())->OnTabWndSelectionChange(
			nOldItemIndex,
			nNewItemIndex,
			bPreSelectionTest
			);
		
		return bRetVal;
	}

	virtual bool OnTabWndClickedButton(
		LONG nHitTest,
		bool bButtonPressed,
		INT nMouseButton, // MK_... values
		UINT nMouseEventFlags
		)
	{
		bool bRetVal =
			CTabImplWnd::OnTabWndClickedButton(
			nHitTest,
			bButtonPressed,
			nMouseButton, // MK_... values
			nMouseEventFlags
			);
		((CExtTabPageContainerWnd*)GetParent())->OnTabWndClickedButton(
			nHitTest,
			bButtonPressed,
			nMouseButton, // MK_... values
			nMouseEventFlags
			);
		return bRetVal;
	}
	
	virtual void PostNcDestroy()
	{
		delete this;
	}
}; // template class CExtTabsWndImpl


/////////////////////////////////////////////////////////////////////////////
// CExtTabPageContainerWnd window

class __PROF_UIS_API CExtTabPageContainerWnd 
	: public CWnd
	, public CExtHookSink
{
	DECLARE_DYNCREATE( CExtTabPageContainerWnd )
	
	DWORD ModifyTabStyle( DWORD dwTabWndStyle, bool bSet)
	{
		ASSERT_VALID( this );
		DWORD dwTabWndStyleRemove = dwTabWndStyle;
		DWORD dwTabWndStyleAdd = dwTabWndStyle;
		if( bSet )	dwTabWndStyleRemove = 0;
		else		dwTabWndStyleAdd = 0;
		DWORD dwRet = m_pWndTab->ModifyTabWndStyle( dwTabWndStyleRemove, dwTabWndStyleAdd, true);
		_RepositionBarsImpl();
		return dwRet;
	}
	
// Construction
public:
	CExtTabPageContainerWnd();

// Attributes
public:

protected:
	CExtTabWnd *  m_pWndTab;
	
// Operations
public:
	static HCURSOR g_hCursor;
	
	DWORD CenterTextSet(bool bSet=true){
		return ModifyTabStyle(__ETWS_CENTERED_TEXT, bSet);
	}
	bool CenterTextGet() const{
		return (m_pWndTab->GetTabWndStyle() & __ETWS_CENTERED_TEXT) ? true : false;
	}
	
	DWORD AutoHideScrollSet(bool bSet=true){
		return ModifyTabStyle(__ETWS_AUTOHIDE_SCROLL, bSet);
	}
	bool AutoHideScrollGet() const{
		return (m_pWndTab->GetTabWndStyle() & __ETWS_AUTOHIDE_SCROLL) ? true : false;
	}
	
	DWORD ShowBtnCloseSet(bool bSet=true){
		return ModifyTabStyle(__ETWS_SHOW_BTN_CLOSE, bSet);
	}
	bool ShowBtnCloseGet() const{
		return (m_pWndTab->GetTabWndStyle() & __ETWS_SHOW_BTN_CLOSE) ? true : false;
	}
	
	DWORD EnabledBtnCloseSet(bool bSet=true){
		return ModifyTabStyle(__ETWS_ENABLED_BTN_CLOSE, bSet);
	}
	bool EnabledBtnCloseGet() const{
		return (m_pWndTab->GetTabWndStyle() & __ETWS_ENABLED_BTN_CLOSE) ? true : false;
	}
	
	DWORD ShowBtnHelpSet(bool bSet=true){
		return ModifyTabStyle(__ETWS_SHOW_BTN_HELP, bSet);
	}
	bool ShowBtnHelpGet() const{
		return (m_pWndTab->GetTabWndStyle() & __ETWS_SHOW_BTN_HELP) ? true : false;
	}
	
	DWORD ShowBtnTabListSet(bool bSet=true){
		return ModifyTabStyle(__ETWS_SHOW_BTN_TAB_LIST, bSet);
	}
	bool ShowBtnTabListGet() const{
		return (m_pWndTab->GetTabWndStyle() & __ETWS_SHOW_BTN_TAB_LIST) ? true : false;
	}

	DWORD EnabledBtnTabListSet(bool bSet=true){
		return ModifyTabStyle(__ETWS_ENABLED_BTN_TAB_LIST, bSet);
	}
	bool EnabledBtnTabListGet() const{
		return (m_pWndTab->GetTabWndStyle() & __ETWS_ENABLED_BTN_TAB_LIST) ? true : false;
	}

	DWORD EnabledBtnHelpSet(bool bSet=true){
		return ModifyTabStyle(__ETWS_ENABLED_BTN_HELP, bSet);
	}
	bool EnabledBtnHelpGet() const{
		return (m_pWndTab->GetTabWndStyle() & __ETWS_ENABLED_BTN_HELP) ? true : false;
	}
	
	DWORD EqualWidthsSet(bool bSet=true){
		return ModifyTabStyle(__ETWS_EQUAL_WIDTHS, bSet);
	}
	bool EqualWidthsGet() const{
		return (m_pWndTab->GetTabWndStyle() & __ETWS_EQUAL_WIDTHS) ? true : false;
	}
	
	DWORD FullWidthSet(bool bSet=true){
		return ModifyTabStyle(__ETWS_FULL_WIDTH, bSet);
	}
	bool FullWidthGet() const{
		return (m_pWndTab->GetTabWndStyle() & __ETWS_FULL_WIDTH) ? true : false;
	}
	
	DWORD HoverFocusSet(bool bSet=true){
		return ModifyTabStyle(__ETWS_HOVER_FOCUS, bSet);
	}
	bool HoverFocusGet() const{
		return (m_pWndTab->GetTabWndStyle() & __ETWS_HOVER_FOCUS) ? true : false;
	}
	
	DWORD InvertVertFontSet(bool bSet=true){
		return ModifyTabStyle(__ETWS_INVERT_VERT_FONT, bSet);
	}
	bool InvertVertFontGet() const{
		return (m_pWndTab->GetTabWndStyle() & __ETWS_INVERT_VERT_FONT) ? true : false;
	}


	DWORD OrientationGet() const{
		ASSERT_VALID( this );
		return m_pWndTab->OrientationGet();
	}
	DWORD OrientationSet(DWORD dwOrientation){
		ASSERT_VALID( this );
		DWORD dwRet = m_pWndTab->OrientationSet(dwOrientation,true);
		_RepositionBarsImpl();
		return dwRet;		
	}
	bool OrientationIsHorizontal() const{
		ASSERT_VALID( this );
		return m_pWndTab->OrientationIsHorizontal();
	}
	bool OrientationIsVertical() const{
		ASSERT_VALID( this );
		return m_pWndTab->OrientationIsVertical();
	}
	bool OrientationIsTopLeft() const{
		ASSERT_VALID( this );
		return m_pWndTab->OrientationIsTopLeft();
	}
	bool SelectionBoldGet() const
	{
		ASSERT_VALID( this );
		return m_pWndTab->SelectionBoldGet();
	}
	void SelectionBoldSet( bool bBold = true)
	{
		ASSERT_VALID( this );
		m_pWndTab->SelectionBoldSet(bBold);
		_RepositionBarsImpl();
	}

	virtual BOOL Create(
		CWnd * pParentWnd,
		const RECT & rcWnd = CRect( 0, 0, 0, 0 ),
		UINT nDlgCtrlID = UINT( IDC_STATIC ),
		DWORD dwWindowStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
		CCreateContext * pContext = NULL
		);

	BOOL PageInsert(
		HWND hWnd,
		__EXT_MFC_SAFE_LPCTSTR sItemText = NULL,
		HICON hIcon = NULL,
		bool bCopyIcon = true,
		int nPos = -1, // append
		bool bSelect = false
		);

	BOOL PageInsert(
		CWnd * pWnd,
		__EXT_MFC_SAFE_LPCTSTR sItemText = NULL,
		HICON hIcon = NULL,
		bool bCopyIcon = true,
		int nPos = -1, // append
		bool bSelect = false
		);

	LONG PageRemove( // returns count of removed items
		LONG nIndex,
		LONG nCountToRemove = 1,
		bool bDestroyPageWnd = true
		)
	{
		ASSERT_VALID( this );
		if( nIndex > m_pWndTab->ItemGetCount()-1)
		{
			return 0;
		}

		HWND hWnd = (HWND)m_pWndTab->ItemLParamGet(nIndex);
		::ShowWindow( hWnd, SW_HIDE );
		if(	bDestroyPageWnd )
			::DestroyWindow(hWnd);
		LONG lRet = m_pWndTab->ItemRemove(nIndex,nCountToRemove,true);

		int nCount = m_pWndTab->ItemGetCount();
		int i = 0;
		for( ; i < nCount; i++ )
			::SetWindowLong(
			(HWND)m_pWndTab->ItemLParamGet(i),
			GWL_ID,
			0x101+i
			);
		
		// select previous page
		if( nCount > 0 )
		{
			if(nIndex > 0)
				nIndex--;
			PageSelectionSet(nIndex);
		}

		RemoveAllWndHooks();
		_RepositionBarsImpl();
		return lRet;
	}

	LONG PageRemoveAll( bool bDestroyPageWnd = true )// returns count of removed items
	{
		ASSERT_VALID( this );
		int nRemovedCount = 0;
		while ( PageRemove(0,1,bDestroyPageWnd) > 0 ) 
			nRemovedCount++;
		return nRemovedCount;
	}
	
	int PageGetCount() const
	{
		ASSERT_VALID( this );
		return m_pWndTab->ItemGetCount();
	}

	int PageSelectionGet() const
	{
		ASSERT_VALID( this );
		return m_pWndTab->SelectionGet();
	}

	void PageSelectionSet( int nPos )
	{
		ASSERT_VALID( this );
		m_pWndTab->SelectionSet( nPos, true, true );
	}

	HICON PageIconGet( LONG nIndex ) const
	{
		ASSERT_VALID( this );
		return m_pWndTab->ItemIconGet( nIndex );
	}
	
	void PageIconSet(
		LONG nIndex,
		HICON hIcon = NULL,
		bool bCopyIcon = true,
		bool bUpdateTabWnd = false
		)
	{
		ASSERT_VALID( this );
		m_pWndTab->ItemIconSet( 
			nIndex,
			hIcon,
			bCopyIcon,
			bUpdateTabWnd
			);
	}

	__EXT_MFC_SAFE_LPCTSTR PageTextGet( LONG nIndex ) const
	{
		ASSERT_VALID( this );
		return m_pWndTab->ItemTextGet( nIndex );
	}
	void PageTextSet(
		LONG nIndex,
		__EXT_MFC_SAFE_LPCTSTR sText = NULL,
		bool bUpdateTabWnd = false
		)
	{
		ASSERT_VALID( this );
		m_pWndTab->ItemTextSet( nIndex, sText, bUpdateTabWnd );
	}

	HWND PageHwndSet(
		LONG nIndex,
		HWND hWndNew
		)
	{
		ASSERT_VALID( this );
		ASSERT( hWndNew != NULL && ::IsWindow(hWndNew) );
		HWND hWndOld = PageHwndGetSafe(nIndex);
		ASSERT( hWndOld != NULL && ::IsWindow(hWndOld) );
		if( hWndNew == hWndOld )
			return hWndNew;
		if( ::GetParent(hWndNew) != m_hWnd )
			::SetParent( hWndNew, m_hWnd );
		m_pWndTab->ItemLParamSet( nIndex, LPARAM(hWndNew) );
		int nSelIdx = PageSelectionGet();
		if( nSelIdx == nIndex )
		{
			if( ( ::GetWindowLong( hWndNew, GWL_STYLE ) & WS_VISIBLE ) == 0 )
				::ShowWindow( hWndNew, SW_SHOW );
		} // if( nSelIdx == nIndex )
		else
		{
			if( ( ::GetWindowLong( hWndNew, GWL_STYLE ) & WS_VISIBLE ) != 0 )
				::ShowWindow( hWndNew, SW_HIDE );
		} // else from if( nSelIdx == nIndex )
		::SetWindowLong(
			hWndNew,
			GWL_ID,
			::GetWindowLong( hWndOld, GWL_ID )
			);
		_RepositionBarsImpl();
		m_pWndTab->UpdateTabWnd( true );
		return hWndOld;
	}
	HWND PageHwndGet( LONG nIndex ) const
	{
		ASSERT_VALID( this );
		LPARAM lParam = m_pWndTab->ItemLParamGet( nIndex );
		HWND hWnd = (HWND)lParam;
		return hWnd;
	}
	HWND PageHwndGetSafe( LONG nIndex ) const
	{
		ASSERT_VALID( this );
		HWND hWnd = PageHwndGet(nIndex);
		if( hWnd != NULL )
		{
			if( ! ::IsWindow(hWnd) )
				hWnd = NULL;
		}
		return hWnd;
	}
	const CWnd * PagePermanentWndGet( LONG nIndex ) const
	{
		ASSERT_VALID( this );
		HWND hWnd = PageHwndGetSafe( nIndex );
		if( hWnd == NULL )
			return NULL;
		CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
		return pWnd;
	}
	CWnd * PagePermanentWndGet( LONG nIndex )
	{
		ASSERT_VALID( this );
		HWND hWnd = PageHwndGetSafe( nIndex );
		if( hWnd == NULL )
			return NULL;
		CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
		return pWnd;
	}
	
	static bool RegisterTabCtrlWndClass();
	
protected:
	static bool g_bTabCtrlWndClassRegistered;
	bool m_bDirectCreateCall:1;
	
	void _RepositionBarsImpl()
	{
		int nPos = 0;
		if( m_pWndTab->GetSafeHwnd() != NULL )
		{
			nPos = (int)m_pWndTab->SelectionGet();
			if( nPos < 0 )
				nPos = 0;
		} // if( m_pWndTab->GetSafeHwnd() != NULL )
		CWnd::RepositionBars( 0, 0xFFFF, 0x101 + nPos );
	}
	
	virtual bool OnHookWndMsg(
		LRESULT & lResult,
		HWND hWndHooked,
		UINT nMessage,
		WPARAM & wParam,
		LPARAM & lParam
		);

	virtual CExtTabWnd* OnTabWndGetTabImpl()
	{
		return new CExtTabsWndImpl < CExtTabWnd >;
	}


	ULONG SetupHookWndSinkToChilds(
		HWND hWnd,
		UINT * pDlgCtrlIDs = NULL,
		ULONG nCountOfDlgCtrlIDs = 0,
		bool bDeep = false
		);


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtTabPageContainerWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL PagesNavigate(BOOL bNext);
	virtual ~CExtTabPageContainerWnd();

	virtual bool OnTabWndClickedButton(
		LONG nHitTest,
		bool bButtonPressed,
		INT nMouseButton, // MK_... values
		UINT nMouseEventFlags
		)
	{
		ASSERT_VALID( this );
		bButtonPressed;
		nMouseButton;
		nMouseEventFlags;
		nHitTest;
		return true;
	}
	
	virtual bool OnTabWndSelectionChange(
		LONG nOldItemIndex,
		LONG nNewItemIndex,
		bool bPreSelectionTest
		)
	{
		ASSERT_VALID( this );
		nOldItemIndex;
		if(!bPreSelectionTest)
		{
			_RepositionBarsImpl();
			RemoveAllWndHooks();
			HWND hWndNew = (HWND)m_pWndTab->ItemLParamGet(nNewItemIndex);
			SetupHookWndSinkToChilds(
				hWndNew,
				NULL,
				0,
				true
				);
		}
		return true;
	}

// Generated message map functions
protected:
	//{{AFX_MSG(CExtTabPageContainerWnd)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_FLAT_CTRL)

/////////////////////////////////////////////////////////////////////////////
// CExtTabPageContainerFlatWnd window

class __PROF_UIS_API CExtTabPageContainerFlatWnd : public CExtTabPageContainerWnd
{
	DECLARE_DYNCREATE( CExtTabPageContainerFlatWnd )

// Construction
public:
	CExtTabPageContainerFlatWnd();

	bool ItemsHasInclineGet( bool bBefore ) const{
		ASSERT_VALID( this );
		ASSERT_VALID( m_pWndTab );
		return ((CExtTabFlatWnd*)m_pWndTab)->ItemsHasInclineGet(bBefore);
	}
	bool ItemsHasInclineBeforeGet() const{
		ASSERT_VALID( this );
		ASSERT_VALID( m_pWndTab );
		return ((CExtTabFlatWnd*)m_pWndTab)->ItemsHasInclineBeforeGet();
	}
	bool ItemsHasInclineAfterGet() const{
		ASSERT_VALID( this );
		ASSERT_VALID( m_pWndTab );
		return ((CExtTabFlatWnd*)m_pWndTab)->ItemsHasInclineAfterGet();
	}
	void ItemsHasInclineSet( bool bBefore, bool bSet = true ){
		ASSERT_VALID( this );
		ASSERT_VALID( m_pWndTab );
		((CExtTabFlatWnd*)m_pWndTab)->ItemsHasInclineSet( bBefore, bSet );
	}
	void ItemsHasInclineBeforeSet( bool bSet = true ){
		ASSERT_VALID( this );
		ASSERT_VALID( m_pWndTab );
		((CExtTabFlatWnd*)m_pWndTab)->ItemsHasInclineBeforeSet( bSet);
	}
	void ItemsHasInclineAfterSet( bool bSet = true ){
		ASSERT_VALID( this );
		ASSERT_VALID( m_pWndTab );
		((CExtTabFlatWnd*)m_pWndTab)->ItemsHasInclineAfterSet( bSet );
	}

// Attributes
public:

// Operations
public:

	virtual CExtTabWnd* OnTabWndGetTabImpl()
	{
		return new CExtTabsWndImpl < CExtTabFlatWnd >;
	}

	virtual BOOL Create(
		CWnd * pParentWnd,
		const RECT & rcWnd = CRect( 0, 0, 0, 0 ),
		UINT nDlgCtrlID = UINT( IDC_STATIC ),
		DWORD dwWindowStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
		CCreateContext * pContext = NULL
		);
	
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtTabPageContainerFlatWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtTabPageContainerFlatWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtTabPageContainerFlatWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_FLAT_CTRL)

#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__EXT_TAB_PAGE_CONTAINER_WND_H)
