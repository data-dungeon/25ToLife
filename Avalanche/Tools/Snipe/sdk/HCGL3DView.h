#pragma once
#include "HCGL2DView.h"

class HLight;
class HTypeProperty;

class SNIPEEXPORT HCGL3DView : public HCGL2DView
{
public:
   HLight *GetCurLight();

   ViewAngleIndex GetViewAngle() const;
   void SetViewAngle( ViewAngleIndex nIndex, BOOL bSnapToLastBirdsEye = TRUE );

   HTypeProperty *GetViewAngleProperty();

   void DefaultSetViewAngle( ViewAngleIndex nIndex, BOOL bSnapToLastBirdsEye = TRUE );
};