#pragma once
#include "GLButtonControl.h"

class CGLTitleBarControl : public CGLControl
{
public:
   static GLImageList m_glImageListBackground;
   static GLImageList m_glImageListButton;

   CGLButtonControl m_glCloseButton;

            CGLTitleBarControl();
   virtual ~CGLTitleBarControl();

// Operations
   void OnClose();

// CGLControl overrides
   virtual void PostInit();
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual BOOL OnLButtonDown( UINT nFlags, CPoint point );
};
