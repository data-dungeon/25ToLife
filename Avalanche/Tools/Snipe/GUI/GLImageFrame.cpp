#include "stdafx.h"
#if 0
#include "GLImageFrame.h"
#include "GLImageView.h"

IMPLEMENT_DYNCREATE(CGLImageFrame, CGL2DFrame)

CGLImageFrame::CGLImageFrame()
{
}

CGLImageFrame::~CGLImageFrame()
{
}

CGLBaseView *CGLImageFrame::CreateView()
{
   return SNEW CGLImageView;
}

void CGLImageFrame::UpdateHeader()
{
   CGL2DFrame::UpdateHeader();
}

void CGLImageFrame::PostInit()
{
   CGL2DFrame::PostInit();
}
#endif