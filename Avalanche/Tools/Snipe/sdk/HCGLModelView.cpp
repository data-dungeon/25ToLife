// 
#include "stdafx.h"
#include "HCGLModelView.h"
#include "../GUI/GLModelView.h"


BOOL HCGLModelView::IsDisplayWireframe()
{
   return ((CGLModelView *)this)->IsDisplayWireframe();
}

BOOL HCGLModelView::IsDisplayShaded()
{
   return ((CGLModelView *)this)->IsDisplayShaded();
}
