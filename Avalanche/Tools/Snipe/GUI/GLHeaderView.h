#pragma once

#include "GLBaseView.h"

class CGLHeaderFrame;

class CGLHeaderView : public CGLBaseView
{
   DECLARE_BASEVIEW(CGLHeaderView, CGLBaseView)

public:
            CGLHeaderView();
   virtual ~CGLHeaderView();

   CGLHeaderFrame *GetHeaderFrame() { return (CGLHeaderFrame *)m_pBaseFrame; }

// Overridables
   virt_base virtual void UpdateHeader();

// CGLControl overrides
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};
