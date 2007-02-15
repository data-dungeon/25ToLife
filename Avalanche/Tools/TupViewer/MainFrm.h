// MainFrm.h : interface of the CMainFrame class
//


#pragma once

#if !defined(__EXT_TEMPL_H)
	#include <ExtTempl.h>
#endif

 //__PROFUISAPPWIZ_KEY_RCB2

#if !defined(__PROFUIS_DLL_RESOURCE_IDs_INCLUDED_)
#define __PROFUIS_DLL_RESOURCE_IDs_INCLUDED_
#include <../profuisdll/resource.h>
#endif

#include "TupTreeView.h"
#include "TupPropertyView.h"

class MdiTabSwitcher : public CExtTabMdiWnd
{
public:
	virtual void OnTabWndSyncVisibility();
}; // class MdiTabSwitcher


class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

	BOOL m_bStandardMDI;

	enum
	{
		WINDOW_TUP_TREE_VIEW =					0x00000001,
		WINDOW_PROPERTY_VIEW =					0x00000002,
		WINDOW_TUP_TREE_VIEW_ITEM =			0x00000004,
	};

// Attributes
protected:
	WINDOWPLACEMENT m_dataFrameWP;
 //__PROFUISAPPWIZ_KEY_PERS_POS_MAINFRM
public:

// Operations
public:

// Overrides
public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
 //__PROFUISAPPWIZ_KEY_USE_MCB
	virtual void ActivateFrame(int nCmdShow = -1);
 //__PROFUISAPPWIZ_KEY_PERS_POS_MAINFRM


	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

//	MdiTabSwitcher m_wndMdiTabs;

protected:  // control bar embedded members
	CExtMenuControlBar    m_wndMenuBar;

	CExtStatusControlBar  m_wndStatusBar;
	CExtToolControlBar    m_wndToolBar;
 
	CExtControlBar m_wndResizableBarFileTree;
	CExtWFF<CTreeCtrl> m_wndDockedCtrlFileTree;

	CExtControlBar m_wndResizableBarOutput;
	CExtEdit m_wndDockedCtrlOutput;

	CExtControlBar m_wndResizableBarProperties;
	TupPropertyView m_wndDockedResizableProperties;
	CExtEdit m_wndEditInDockedDlg;

	CExtControlBar m_wndTupTreeViewBar;
	CExtWFF < TupTreeView > m_wndTupTreeViewChild;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnUpdateControlBarMenu(CCmdUI* pCmdUI);
	afx_msg BOOL OnBarCheck(UINT nID);
public:
	void RefreshAll(void);
	void RefreshSelected(int windowFlags,void *pData);
};
