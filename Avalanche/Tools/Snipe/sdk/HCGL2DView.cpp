// 
#include "stdafx.h"
#include "HCGL2DView.h"
#include "../GUI/GL2DView.h"

HCamera *HCGL2DView::GetCurrentCamera()
{
   return (HCamera *) ((CGL2DView *)this)->GetCamera();
}
