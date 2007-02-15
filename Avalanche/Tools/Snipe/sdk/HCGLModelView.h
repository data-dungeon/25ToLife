#pragma once
#include "HCGL3DView.h"

class HCModelDoc;

class SNIPEEXPORT HCGLModelView : public HCGL3DView
{
public:
   HCModelDoc *GetDocument() { return (HCModelDoc *)HCGL3DView::GetDocument(); }
   BOOL IsDisplayWireframe();
   BOOL IsDisplayShaded();
};