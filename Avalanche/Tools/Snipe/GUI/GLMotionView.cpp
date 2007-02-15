//
#include "stdafx.h"
#include "GLMotionView.h"
#include "MotionDoc.h"
#include "Motion.h"
#include "ObjectExplorerBar.h"
#include "MouseOn.h"
#include "Snipe.h"
#include "Scene.h"

IMPLEMENT_DYNCREATE(CGLMotionView, CGLSceneView)

BEGIN_MESSAGE_MAP(CGLMotionView, CGLSceneView)
END_MESSAGE_MAP()

CGLMotionView::CGLMotionView()
{
}

CGLMotionView::~CGLMotionView()
{
}

void CGLMotionView::OnDragEnter( CPoint point, CGLControl *pDragControl )
{
   SetCursor( AfxGetApp()->LoadStandardCursor( IDC_ARROW ));
}

void CGLMotionView::OnDragMove( CPoint point, CGLControl *pDragControl )
{
}

void CGLMotionView::OnDragLeave( CPoint point, CGLControl *pDragControl )
{
}

void CGLMotionView::OnDragDrop( CPoint point, CGLControl *pDragControl )
{
   if (pDragControl->IsKindOf(RUNTIME_CLASS(CGLObjectTreeItem)))
   {
      CGLObjectTreeItem *pItem = (CGLObjectTreeItem *)pDragControl;
      SnipeObject *pObject = pItem->GetObject();
      if (pObject)
      {
         if (pObject->IsKindOf( SOT_TransformableObject ))
         {
            GetDocument()->ApplyModel((TransformableObject*)pObject);
            GetApp().DrawInvalidControls();
         }
      }
   }
}

