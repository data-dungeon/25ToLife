#pragma once

#include "GL2DView.h"

class CGLSchematicView : public CGL2DView
{
   DECLARE_BASEVIEW(CGLSchematicView, CGL2DView)

public:
            CGLSchematicView();
   virtual ~CGLSchematicView();

// CGLBaseView overrides
   virtual void DefaultZoomFit(SnipeObjectList *pList);
   virtual void DrawContents();
};
