#if 0
#pragma once
#include "GL2DFrame.h"

class CGLImageFrame : public CGL2DFrame
{
   DECLARE_DYNCREATE( CGLImageFrame )
public:
// Attributes

            CGLImageFrame();
   virtual ~CGLImageFrame();

// CGLHeaderFrame
   virtual void UpdateHeader();
   virtual CGLBaseView *CreateView();

// CGLControl overrides
   virtual void PostInit();
};
#endif