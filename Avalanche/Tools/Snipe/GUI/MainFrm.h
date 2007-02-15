// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__57DF813D_6BDB_4AA1_A29E_0AAA116BC4D5__INCLUDED_)
#define AFX_MAINFRM_H__57DF813D_6BDB_4AA1_A29E_0AAA116BC4D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GLMainRenderWnd.h"
//#include "GLStatusBarControl.h"

class CGLCustomizeDlg;

class CMainFrame : public CFrameWnd
{
protected: 
	DECLARE_DYNAMIC(CMainFrame)

   CGLMainRenderWnd m_glMainRenderWnd;
   MainInterface   *m_pMainInterface;
//   CGLStatusBarWnd  m_glStatusBarWnd;
   UINT             m_bFullScreen;
   WINDOWPLACEMENT  m_wpInitial;
   CGLCustomizeDlg *m_pCustomizeDlg;
   union {
      ULONG    m_mainframeflags;
      struct {
         ULONG m_bUnused : 1;
      };
   };

public:
   static CGLSharedRenderWnd m_glSharedRenderWnd;

            CMainFrame();
	virtual ~CMainFrame();

   MainInterface *GetMainInterface() { return m_pMainInterface; }
   CGLMainRenderWnd *GetMainRenderWnd() { return &m_glMainRenderWnd; }

   CGLToolBarControl *FindToolBar(const String &strName) const;
   // This will return an already existing toolbar if available
   CGLToolBarControl *AddToolBar( const String &strName );

   CommandUIPlugin *GetCommandUIPlugin( UINT nCmdID );

   GLuint CreateCallListFromGeoResource( UINT nResourceID );

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnClose();
   afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   afx_msg void OnFileNewModelPolygonal();
   afx_msg void OnFileNewModelPatch();
   afx_msg void OnFileNew();
   afx_msg void OnFileNewProject();
   afx_msg void OnFileOpen();
   afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
   afx_msg void OnFileSaveProjectAs();
   afx_msg void OnUpdateFileSaveProjectAs(CCmdUI *pCmdUI);
   afx_msg void OnSaveProject();
   afx_msg void OnUpdateSaveProject(CCmdUI* pCmdUI);
   afx_msg void OnImportModel();
   afx_msg void OnUpdateImportModel(CCmdUI* pCmdUI);
   afx_msg void OnImportScene();
   afx_msg void OnUpdateImportScene(CCmdUI* pCmdUI);
   afx_msg void OnImportMotion();
   afx_msg void OnUpdateImportMotion(CCmdUI* pCmdUI);
   afx_msg void OnImportMotionSet();
   afx_msg void OnUpdateImportMotionSet(CCmdUI* pCmdUI);
   afx_msg void OnImportImage();
   afx_msg void OnUpdateImportImage(CCmdUI* pCmdUI);
   afx_msg void OnUpdatePluginCommand( CCmdUI *pCmdUI );
   afx_msg void OnPluginCommand( UINT nID );
   afx_msg void OnUpdateHistoryControl(CCmdUI *pCmdUI);
   afx_msg void OnUpdateComponentCombo(CCmdUI *pCmdUI);
   afx_msg void OnUpdateManipulatorCombo(CCmdUI *pCmdUI);
   afx_msg void OnUpdateOutputControl( CCmdUI *pCmnUI );
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnFileNewScene();
   afx_msg void OnFileNewMotionSet();
   afx_msg void OnMirrorMode();
   afx_msg void OnUpdateMirrorMode(CCmdUI* pCmdUI);

   afx_msg void OnEditUndo();
   afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
   afx_msg void OnEditRedo();
   afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);
   afx_msg void OnGotoCommandLine();
   afx_msg void OnUpdateGotoCommandLine(CCmdUI *pCmdUI);

   DECLARE_MESSAGE_MAP()

// CFrameWnd override
   virtual HACCEL GetDefaultAccelerator();

// CWnd Overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};

CMainFrame *GetMainFrame();

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__57DF813D_6BDB_4AA1_A29E_0AAA116BC4D5__INCLUDED_)
	