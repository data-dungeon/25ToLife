#if 0
#pragma once
#include "GLModelFrame.h"

class CGLSceneFrame : public CGLModelFrame
{
   DECLARE_DYNCREATE( CGLSceneFrame )
public:
            CGLSceneFrame();
   virtual ~CGLSceneFrame();

// CGLBaseFrame overrides
   virtual CGLBaseView *CreateView();
};
#endif