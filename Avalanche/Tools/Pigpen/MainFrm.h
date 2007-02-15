/************************************/
/*                                  */
/* MainFrm.h                        */
/* Pigpen particle tool  12/07/99   */
/* ned martin  avalanche software   */
/* visual c++ 6.0                   */
/* parent window for application    */
/*                                  */
/************************************/

#ifndef __MAINFRM_H
#define __MAINFRM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************* includes ****************************/

/******************* defines *****************************/

/******************* class *******************************/

// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/******************* child frame class *******************/

/* i have to derive my own child frame class from CMDIChildWnd so that i can clear the FWS_ADDTOTITLE flag,
which overwrites the window title with it's own title */

class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)

public:
	CChildFrame() {}
	~CChildFrame() {}

	/* override PreCreateWindow to clear FWS_ADDTOTITLE flag */

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) {cs.style &= ~FWS_ADDTOTITLE; return(CMDIChildWnd::PreCreateWindow(cs)); }
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.




#endif //__MAINFRM_H
