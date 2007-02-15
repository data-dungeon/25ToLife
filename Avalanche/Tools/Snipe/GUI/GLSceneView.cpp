//
#include "stdafx.h"
#include "GLSceneView.h"
#include "SceneDoc.h"
#include "Scene.h"
#include "ObjectExplorerBar.h"
#include "MouseOn.h"
#include "Snipe.h"
#include "MainFrm.h"

IMPLEMENT_DYNCREATE(CGLSceneView, CGLModelView)

BEGIN_MESSAGE_MAP(CGLSceneView, CGLModelView)
END_MESSAGE_MAP()

CGLSceneView::CGLSceneView()
{
}

CGLSceneView::~CGLSceneView()
{
   if (GetMainInterface()->GetCurrentView() == this)
   {
      GetMainFrame()->GetMainRenderWnd()->m_glTimeLineBar.m_glTransportControl.m_glStartFrameEdit.SetProperty( NULL );
      GetMainFrame()->GetMainRenderWnd()->m_glTimeLineBar.m_glTransportControl.m_glEndFrameEdit.SetProperty( NULL );
   }
}

void CGLSceneView::OnDragEnter( CPoint point, CGLControl *pDragControl )
{
   SetCursor( AfxGetApp()->LoadStandardCursor( IDC_ARROW ));
}

void CGLSceneView::OnDragMove( CPoint point, CGLControl *pDragControl )
{
}

void CGLSceneView::OnDragLeave( CPoint point, CGLControl *pDragControl )
{
}

void CGLSceneView::OnDragDrop( CPoint point, CGLControl *pDragControl )
{
   if (pDragControl->IsKindOf(RUNTIME_CLASS(CGLObjectTreeItem)))
   {
      CGLObjectTreeItem *pItem = (CGLObjectTreeItem *)pDragControl;
      SnipeObject *pObject = pItem->GetObject();
      if (pObject)
      {
         if (pObject->IsKindOf( SOT_TransformableObject ))
         {
            if (IsDownAlt())
            {
               TransformableObject *pCache = (TransformableObject *)pObject;

               for (int i=0; i<100; i++)
               {
                  TransformableObject *pInstance = (TransformableObject *)pCache->CreateInstance();
                  GetDocument()->m_pScene->AddChildAtTail( pInstance );
                  Vector pos( (Rnd()-0.5)*500.0f, (Rnd()-0.5)*500.0f, (Rnd()-0.5)*500.0f );
                  pInstance->GetTransform().m_vTranslate.StoreValue( pos );
               }
            }
            else
            {
               TransformableObject *pCache = (TransformableObject *)pObject;

               TransformableObject *pInstance = (TransformableObject*)GetDocument()->m_pScene->AddInstance(pCache);
               pInstance->Select();

               // position it
               MouseOnPlane mouseonplane;
               mouseonplane.Init( (HCGLBaseView *)this, Vector(0), Vector(0,1,0) );
               ClientToControl( point );
               Vector pos = mouseonplane.GetWorldPos( point );
               pInstance->GetTransform().m_vTranslate.StoreValue( pos );
            }
            GetApp().DrawInvalidControls();
         }
      }
   }
}

void CGLSceneView::OnActivate( ActivationMsg msg, CGLControl *pOther )
{
   CGLModelView::OnActivate( msg, pOther );

   switch (msg)
   {
      case ACTIVATION_ACTIVE:
         GetMainFrame()->GetMainRenderWnd()->m_glTimeLineBar.m_glTransportControl.m_glStartFrameEdit.SetProperty( GetDocument()->GetPlaybackStartProperty() );
         GetMainFrame()->GetMainRenderWnd()->m_glTimeLineBar.m_glTransportControl.m_glEndFrameEdit.SetProperty( GetDocument()->GetPlaybackEndProperty() );
         break;
   }
}

void CGLSceneView::DrawGrid()
{
   if (!m_bDisplayGrid)
      return;

   glPushMatrix();

   glRotatef( 90.0f, 1.0f, 0.0f, 0.0f );

   CGL2DView::DrawGrid(); // skip CGLModelView as we always want XZ grid

   glPopMatrix();
}
