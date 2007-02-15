#pragma once
#include "GLModelView.h"

class CSceneDoc;

class CGLSceneView : public CGLModelView
{
   DECLARE_BASEVIEW(CGLSceneView, CGLModelView)

public:
            CGLSceneView();
   virtual ~CGLSceneView();

   CSceneDoc *GetDocument() const { return (CSceneDoc *)CGLBaseView::GetDocument(); }

// CGL2DView overrides
   virtual void DrawGrid();

// CGLControl overrides
   virtual void OnDragEnter( CPoint point, CGLControl *pDragControl );
   virtual void OnDragMove( CPoint point, CGLControl *pDragControl );
   virtual void OnDragLeave( CPoint point, CGLControl *pDragControl );
   virtual void OnDragDrop( CPoint point, CGLControl *pDragControl );
   virtual void OnActivate( ActivationMsg msg, CGLControl *pOther );
};
