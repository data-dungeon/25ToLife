#if 0
#pragma once
#include "GLHeaderFrame.h"

class CGL3DView;

class CGL3DFrame : public CGLHeaderFrame
{
   DECLARE_DYNAMIC( CGL3DFrame )
public:
// Attributes
   CGLComboControl  m_glViewAngleComboControl;

            CGL3DFrame();
   virtual ~CGL3DFrame();

   CGL3DView *GetView() { return (CGL3DView *)m_pView; }

// CGLHeaderFrame overrides
   virtual void UpdateHeader();

// CGLControl overrides
   virtual void PostInit();
};
#endif