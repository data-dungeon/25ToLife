// 
#include "stdafx.h"
#include "HCGL3DView.h"
#include "../GUI/GL3DView.h"

HLight *HCGL3DView::GetCurLight()
{
   return (HLight *)((CGL3DView *)this)->GetCurLight();
}

ViewAngleIndex HCGL3DView::GetViewAngle() const
{
   return ((CGL3DView *)this)->GetViewAngle();
}

void HCGL3DView::SetViewAngle( ViewAngleIndex nIndex, BOOL bSnapToLastBirdsEye/*=TRUE*/  )
{
   ((CGL3DView *)this)->SetViewAngle( nIndex, bSnapToLastBirdsEye );
}

void HCGL3DView::DefaultSetViewAngle( ViewAngleIndex nIndex, BOOL bSnapToLastBirdsEye /*= TRUE*/ )
{
   ((CGL3DView *)this)->DefaultSetViewAngle( nIndex, bSnapToLastBirdsEye );
}

HTypeProperty *HCGL3DView::GetViewAngleProperty()
{
   return (HTypeProperty *) ((CGL3DView *)this)->GetViewAngleProperty();
}
