// GLMainRenderWnd.h : interface of the CGLMainRenderWnd class
//

#pragma once

#include "GLRenderWnd.h"
#include "ObjectExplorerBar.h"
#include "PropertyBar.h"
#include "OutputView.h"
#include "GLViewSplitter.h"
#include "GLMenuBar.h"
#include "GLToolBar.h"
#include "GLHistoryListControl.h"
#include "GLComponentCombo.h"
#include "GLManipulatorCombo.h"
#include "PropertyBar.h"
#include "GLStatusBarControl.h"
#include "GLTimeLine.h"

#define USEFREETYPE
//#define USEFTGL

#ifdef USEFREETYPE
#include "FreeType.h"
#endif
#ifdef USEFTGL
#include "../FTGL/include/FTGLOutlineFont.h"
#include "../FTGL/include/FTGLPolygonFont.h"
#include "../FTGL/include/FTGLBitmapFont.h"
#include "../FTGL/include/FTGLTextureFont.h"
#include "../FTGL/include/FTGLPixmapFont.h"
#endif

class CGLExplorerBar : public CGLBorderedControl
{
public:
   enum { TABHEIGHT=20 };

   CGLTabControl             m_glTab;
   CGLObjectTreeControl       m_glProjectTree;
   CGLObjectTreeControl       m_glImageTree;
   CGLObjectTreeControl       m_glModelTree;
   CGLObjectTreeControl       m_glMotionTree;
   CGLObjectTreeControl       m_glMotionSetTree;

   CGLExplorerBar() :
      m_glTab( TRUE, TRUE )
   {
   }

// CGLControl overrides
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
};

class CGLPanelBar : public CGLTransparentControl
{
public:
   CGLSplitterControl m_glPropertySplitter;
   CGLExplorerBar      m_glExplorerBar;
   CGLPropertyFrame    m_glPropertyFrame;

// CGLControl overrides
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
};

#include "GLBaseFrame.h"
class CGLDocumentBar : public CGLBorderedControl
{
public:
   CGLSplitterControl       m_glDocFrameSplitter;
   CList<CGLBaseDocFrame *> m_frameList;
   CGLBaseDocFrame         *m_pCurrentFrame;

   CGLDocumentBar() :
    m_glDocFrameSplitter( RUNTIME_CLASS(CGLBaseDocFrame), 6 )
   {
      m_pCurrentFrame = NULL;
   }

// CGLControl overrides
   virtual void PostInit()
   {
      CGLControl::PostInit();

      m_glDocFrameSplitter.Init( this );
      m_glDocFrameSplitter.AddRow();
      m_glDocFrameSplitter.AddColumn();
   }
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE )
   {
      CGLControl::Move( ptControl, szControl, justification );
      m_glDocFrameSplitter.Move( CPoint(2,2), CSize(m_szControl.cx-4, m_szControl.cy-4) );
   }
};

#define MOUSESTATUSPANE_LEFT   0
#define MOUSESTATUSPANE_MIDDLE 1
#define MOUSESTATUSPANE_RIGHT  2
#define MOUSESTATUSPANE_WHEEL  3

class CGLMainRenderWnd : public CGLRenderWnd
{
// Attributes
public:
   CGLImageControl          m_glBackground;
    CGLMenuBarControl        m_glMenuBar;
    CGLToolBarControl        m_glToolBar;
    CGLComponentCombo        m_glComponentCombo;
    CGLManipulatorCombo      m_glManipulatorCombo;
    CGLToolBarShelf          m_glToolBarContainer;
    CGLSplitterControl       m_glExplorerSplitter;
     CGLPanelBar              m_glPanelBar;
     CGLSplitterControl       m_glTimeLineSplitter;
      CGLDocumentBar           m_glDocumentBar;
      CGLTimeLineBar           m_glTimeLineBar;
    CGLStatusBarControl      m_glStatusBarControl;
     CGLStatusPaneControl      m_glStatusPaneControl;
     CGLKeyModifierPaneControl m_glModStatusPaneControl;
     CGLStatusPaneControl      m_glMouseStatusPaneControl[4];

         	CGLMainRenderWnd();
	virtual ~CGLMainRenderWnd();

// Generated message map functions
	DECLARE_MESSAGE_MAP()
public:
   void RefreshStatusBarInstructions();

   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnIdleUpdateCmdUI();
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

   virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};

class CGLSharedRenderWnd : public CGLRenderWnd
{
// Attributes
public:
   enum { ALPHADISPLAYLISTBASE=10000 };
#ifdef USEFREETYPE
   freetype::font_data m_font;	
#endif
#ifdef USEFTGL
   FTFont *m_pFont;
#endif

            CGLSharedRenderWnd();
	virtual ~CGLSharedRenderWnd();

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnDestroy();
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};

