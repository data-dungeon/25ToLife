#if 0
#pragma once
#include "GLSceneFrame.h"

class CGLMotionSetFrame : public CGLSceneFrame
{
   DECLARE_DYNCREATE( CGLMotionSetFrame )
public:
            CGLMotionSetFrame();
   virtual ~CGLMotionSetFrame();

// CGLBaseFrame overrides
   virtual CGLBaseView *CreateView();
};
#endif