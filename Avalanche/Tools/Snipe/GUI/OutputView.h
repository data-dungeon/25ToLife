#pragma once

#include "GLScrollBarControl.h"
#include "GLEditControl.h"

class HierObject;

class OutputLine
{
public:
   RGBAFloat m_color;
   String    m_strPath;
   String    m_strText;
};

class CGLOutputControl : public CGLScrollableControl
{
   DECLARE_DYNCREATE(CGLOutputControl)
	DECLARE_MESSAGE_MAP()
public:
   CList<OutputLine> m_linelist;

            CGLOutputControl();
   virtual ~CGLOutputControl();

	void AppendLine(const char *line);
   void SetCurrentWorkingObject( SnipeObject *pObject );
   void SetCurrentCommandObject( SnipeObject *pObject );

   //afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnClear();

// CGLControl overrides
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual UINT GetContextMenuID( const CPoint &point, UINT &nDefaultItemID );

// CCmdTarget overrides
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
};

class CGLOutputControlCmdUI : public CGLControlCmdUI
{
public:
   CGLOutputControlCmdUI ( CGLControl *pControl ) :
      CGLControlCmdUI ( pControl ) {}

   void SetCurrentWorkingObject( SnipeObject *pObject );
   void SetCurrentCommandObject( SnipeObject *pObject );
};

class CGLCommandLineControl : public CGLEditControl
{
public:
   CArray<String> m_commandhistory;
   int            m_nHistoryIndex;

   CGLCommandLineControl()
   {
      m_bKillFocusOnEnter = FALSE;
      m_nHistoryIndex = 0;
   }

// CGLControl overrides
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );

// CCmdTarget overrides
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
};

class CGLOutputFrame : public CGLTransparentControl
{
public:
   CGLOutputControl      m_glOutputControl;
   CGLCommandLineControl m_glCommandLineControl;

// CGLControl overrides
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};
