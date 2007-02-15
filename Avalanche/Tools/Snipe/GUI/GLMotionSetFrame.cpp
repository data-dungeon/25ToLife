#include "stdafx.h"
#if 0
#include "GLMotionSetFrame.h"
#include "GLMotionSetView.h"

IMPLEMENT_DYNCREATE(CGLMotionSetFrame, CGLSceneFrame)

CGLMotionSetFrame::CGLMotionSetFrame()
{
}

CGLMotionSetFrame::~CGLMotionSetFrame()
{
}

CGLBaseView *CGLMotionSetFrame::CreateView()
{
   return SNEW CGLMotionSetView;
}
#endif