#include "stdafx.h"
#if 0
#include "GLMotionFrame.h"
#include "GLMotionView.h"

IMPLEMENT_DYNCREATE(CGLMotionFrame, CGLSceneFrame)

CGLMotionFrame::CGLMotionFrame()
{
}

CGLMotionFrame::~CGLMotionFrame()
{
}

CGLBaseView *CGLMotionFrame::CreateView()
{
   return SNEW CGLMotionView;
}
#endif