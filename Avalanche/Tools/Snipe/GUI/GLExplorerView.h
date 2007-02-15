#pragma once

#include "GLBaseView.h"
#include "ObjectExplorerBar.h"

class CGLExplorerView : public CGLBaseView
{
   DECLARE_BASEVIEW(CGLExplorerView, CGLBaseView)

public:
   CGLObjectTreeControl m_glTreeControl;

            CGLExplorerView();
   virtual ~CGLExplorerView();

// GLControl overrides
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};
