#pragma once

#include "GL2DView.h"

class CImageDoc;

class CGLImageView : public CGL2DView
{
   DECLARE_BASEVIEW(CGLImageView, CGL2DView)

public:
            CGLImageView();
   virtual ~CGLImageView();

// Operations
   CImageDoc *GetDocument() { return (CImageDoc *)CGLBaseView::GetDocument(); }

// CGLBaseView overrides
   virtual void DefaultZoomFit(SnipeObjectList *pList);
   virtual void DrawContents();
};
