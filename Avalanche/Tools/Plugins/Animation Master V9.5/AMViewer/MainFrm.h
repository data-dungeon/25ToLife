// DAN  10/30/98  \DanB\AMViewer\MainFrm.h

#define __MAINFRM_H

#ifndef __AMVIEWERSPLITTER_H
#include "AMViewerSplitter.h"
#endif

class CAMViewerView;
class CScriptView;

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

public:
   CAMViewerView *m_modelview;
   CScriptView   *m_scriptview;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

	virtual ~CMainFrame();

public:
   enum {SI_RENDEREDFPS=1, SI_ACTIONFRAME, SI_SCRIPTFRAME, SI_SUBLEVEL, SI_PATCHCOUNT};
   void SetRenderedFPS(const CString &text, BOOL bupdate = FALSE);
   void SetActionFrame(const CString &text, BOOL bupdate = FALSE);
   void SetScriptFrame(const CString &text, BOOL bupdate = FALSE);
   void SetStatusMessage(const CString &text, BOOL bupdate = FALSE);
   void SetPatchPolyCount(const CString &text, BOOL bupdate = FALSE);
   void SetSubLevel(const CString &text, BOOL bupdate = FALSE);
   void UpdateStatusBar();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CToolBar     m_wndToolBar;
	CStatusBar   m_wndStatusBar;
   CAMViewerSplitter m_wndSplitter;

	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
