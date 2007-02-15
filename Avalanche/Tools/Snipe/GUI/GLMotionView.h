#pragma once
#include "GLSceneView.h"

class CMotionDoc;

class CGLMotionView : public CGLSceneView
{
   DECLARE_BASEVIEW(CGLMotionView, CGLSceneView)

public:
            CGLMotionView();
   virtual ~CGLMotionView();

   CMotionDoc *GetDocument() const { return (CMotionDoc *)CGLBaseView::GetDocument(); }

// CGLControl overrides
   virtual void OnDragEnter( CPoint point, CGLControl *pDragControl );
   virtual void OnDragMove( CPoint point, CGLControl *pDragControl );
   virtual void OnDragLeave( CPoint point, CGLControl *pDragControl );
   virtual void OnDragDrop( CPoint point, CGLControl *pDragControl );
};
