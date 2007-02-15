#if 0
#pragma once
#include "GL2DFrame.h"
#include "PropertyBar.h"

class CGLTexCoordView;

class CGLTexCoordFrame : public CGL2DFrame
{
   DECLARE_DYNCREATE( CGLTexCoordFrame )
public:
// Attributes
   CGLObjectCombo m_glContextCombo;
   UINT           m_bRebuildContextCombo : 1;

            CGLTexCoordFrame();
   virtual ~CGLTexCoordFrame();

// Operations
   CGLTexCoordView *GetView() { return (CGLTexCoordView *)m_pView; }
   void FillContextCombo();

// CGLBaseFrame overrides
   virtual void UpdateHeader();
   virtual CGLBaseView *CreateView();

// CGLControl overrides
   virtual void PostInit();
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};
#endif