#include "stdafx.h"
#if 0
#include "GLSceneFrame.h"
#include "GLSceneView.h"

IMPLEMENT_DYNCREATE(CGLSceneFrame, CGLModelFrame)

CGLSceneFrame::CGLSceneFrame()
{
}

CGLSceneFrame::~CGLSceneFrame()
{
}

CGLBaseView *CGLSceneFrame::CreateView()
{
   return SNEW CGLSceneView;
}
#endif