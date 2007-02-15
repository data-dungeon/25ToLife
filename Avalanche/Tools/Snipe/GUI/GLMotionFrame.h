#if 0
#pragma once
#include "GLSceneFrame.h"

class CGLMotionFrame : public CGLSceneFrame
{
   DECLARE_DYNCREATE( CGLMotionFrame )
public:
            CGLMotionFrame();
   virtual ~CGLMotionFrame();

// CGLBaseFrame overrides
   virtual CGLBaseView *CreateView();
};
#endif