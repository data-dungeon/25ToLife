//
#pragma once

#include "GLTabControl.h"

class CGLBaseFrame : public CGLControl
{
   DECLARE_DYNCREATE( CGLBaseFrame )
public:
   CRuntimeClass *m_pViewClass; // class for creating new view controls
   CGLBaseView   *m_pView;
   BOOL           m_bMaximized;
   CGLControl    *m_pRestoreParent;
   CGLControl    *m_pRestoreControlReplaced;

            CGLBaseFrame();
   virtual ~CGLBaseFrame();

// Operations
   void CreateView();
   void OnMaximize();
   void OnRestore();

// Overridables
   virt_base virtual void UpdateHeader() {}

// CGLControl overrides
   virtual void OnSplit( CGLControl *pFirstCell );
};

class CBaseDoc;

class CGLDocTabControl : public CGLTabControl
{
   DECLARE_MESSAGE_MAP()
public:
   CGLDocTabControl() : CGLTabControl( TRUE ) {}

   CBaseDoc *GetCurrentDocument() const;

   afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
   afx_msg void OnFileSave();
   afx_msg void OnTabControlNewHorizontalTabGroup();
   afx_msg void OnUpdateTabControlNewHorizontalTabGroup(CCmdUI *pCmdUI);
   afx_msg void OnTabControlNewVerticalTabGroup();
   afx_msg void OnUpdateTabControlNewVerticalTabGroup(CCmdUI *pCmdUI);
   afx_msg void OnTabControlMoveToPreviousTabGroup();
   afx_msg void OnUpdateTabControlMoveToPreviousTabGroup(CCmdUI *pCmdUI);
   afx_msg void OnTabControlMoveToNextTabGroup();
   afx_msg void OnUpdateTabControlMoveToNextTabGroup(CCmdUI *pCmdUI);
   afx_msg void OnTabControlLayoutNew();
   afx_msg void OnUpdateTabControlLayoutNew(CCmdUI *pCmdUI);
   afx_msg void OnLoadLayout( UINT nID );

// GLTabControl overrides
   virtual UINT GetTabMenuID();
};

class CGLBaseDocFrame : public CGLControl
{
   DECLARE_DYNCREATE( CGLBaseDocFrame )
public:
   enum { TABHEIGHT=19 };

   CGLDocTabControl m_glTabControl;

            CGLBaseDocFrame();
   virtual ~CGLBaseDocFrame();

// Operations
	CBaseDoc* GetCurrentDocument() const { return m_glTabControl.GetCurrentDocument(); }

// CGLControl overrides
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};

