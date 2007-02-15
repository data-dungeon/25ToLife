//
#include "stdafx.h"
#include "GLMotionSetView.h"
#include "MotionSetDoc.h"
#include "MotionSet.h"
#include "ObjectExplorerBar.h"
#include "MouseOn.h"
#include "Snipe.h"
#include "Scene.h"

IMPLEMENT_DYNCREATE(CGLMotionSetView, CGLSceneView)

BEGIN_MESSAGE_MAP(CGLMotionSetView, CGLSceneView)
END_MESSAGE_MAP()

static TypeProperty::Info::Node f_ViewModeInfos[] = {
   TypeProperty::Info::Node( "Original", "Original" ),
   TypeProperty::Info::Node( "Reduced", "Reduced" ),
   TypeProperty::Info::Node( "Composited", "Composited" ),
   TypeProperty::Info::Node( "Side-By-Side", "SideBySide" ),
};

TypeProperty::Info CGLMotionSetView::m_nViewModeInfo( "View Mode", "ViewMode", f_ViewModeInfos, sizeof(f_ViewModeInfos)/sizeof(TypeProperty::Info::Node), CGLMotionSetView::VIEWMODE_SIDEBYSIDE );

CGLMotionSetView::CGLMotionSetView()
{
   m_nViewMode.Init( &m_nViewModeInfo, this );
}

CGLMotionSetView::~CGLMotionSetView()
{
}

void CGLMotionSetView::OnDragEnter( CPoint point, CGLControl *pDragControl )
{
   SetCursor( AfxGetApp()->LoadStandardCursor( IDC_ARROW ));
}

void CGLMotionSetView::OnDragMove( CPoint point, CGLControl *pDragControl )
{
}

void CGLMotionSetView::OnDragLeave( CPoint point, CGLControl *pDragControl )
{
}

void CGLMotionSetView::OnDragDrop( CPoint point, CGLControl *pDragControl )
{
   if (pDragControl->IsKindOf(RUNTIME_CLASS(CGLObjectTreeItem)))
   {
      CGLObjectTreeItem *pItem = (CGLObjectTreeItem *)pDragControl;
      SnipeObject *pObject = pItem->GetObject();
      if (pObject)
      {
         if (pObject->IsKindOf( SOT_Motion ))
         {
            GetDocument()->AddMotion((Motion*)pObject);
            GetApp().DrawInvalidControls();
         }
         else if (pObject->IsKindOf( SOT_PolyModel ))
         {
            GetDocument()->GetMotionSet()->m_pModel.StoreValue( (PolyModel *)pObject );
            GetApp().DrawInvalidControls();
         }
      }
   }
}

void CGLMotionSetView::DrawContents()
{
   CMotionSetDoc *pDoc = GetDocument();
   MotionSet *pMotionSet = pDoc->GetMotionSet();

   switch (m_nViewMode)
   {
      case VIEWMODE_ORIGINAL:
      {
         pMotionSet->m_pCurModelInstance = pMotionSet->m_pOriginalModelInstance;
         CGLSceneView::DrawContents();
         if (!m_bDoPick)
         {
            glColor3f( 1,1,1 );
            DrawText( m_szControl.cx-55, 20, "Original" );
         }
         break;
      }
      case VIEWMODE_REDUCED:
      {
         pMotionSet->m_pCurModelInstance = pMotionSet->m_pReducedModelInstance;
         CGLSceneView::DrawContents();
         if (!m_bDoPick)
         {
            glColor3f( 1,1,1 );
            DrawText( m_szControl.cx-60, 20, "Reduced" );
         }
         break;
      }
      case VIEWMODE_COMPOSITED:
      {
         if (pMotionSet->m_pOriginalModelInstance)
            pMotionSet->m_pOriginalModelInstance->SetDrawOpacity( 0.5f );
         CGLSceneView::DrawContents();
         if (pMotionSet->m_pOriginalModelInstance)
            pMotionSet->m_pOriginalModelInstance->SetDrawOpacity( 1.0f );

         if (!m_bDoPick)
         {
            glColor3f( 1,1,1 );
            DrawText( m_szControl.cx-60, 20, "Reduced" );
            glColor3f( 0.5f, 0.5f, 0.5f );
            DrawText( m_szControl.cx-60, 33, "+ Original" );
         }
         break;
      }
      case VIEWMODE_SIDEBYSIDE:
      {
         GLint oldviewport[4];
         glGetIntegerv( GL_VIEWPORT, oldviewport );
         int nOldWidth = m_szControl.cx;
         m_szControl.cx /= 2;
         glViewport( oldviewport[0], oldviewport[1], oldviewport[2]/2, oldviewport[3] );
         pMotionSet->m_pCurModelInstance = pMotionSet->m_pOriginalModelInstance;
 
         CGLSceneView::DrawContents();

         if (!m_bDoPick)
         {
            glColor3f( 1,1,1 );
            DrawText( m_szControl.cx-55, 20, "Original" );
         }

         glViewport( oldviewport[0]+oldviewport[2]/2, oldviewport[1], oldviewport[2]/2, oldviewport[3] );

         pMotionSet->m_pCurModelInstance = pMotionSet->m_pReducedModelInstance;

         CGLSceneView::DrawContents();

         if (!m_bDoPick)
         {
            glColor3f( 1,1,1 );
            DrawText( m_szControl.cx-60, 20, "Reduced" );
         }

         glViewport( oldviewport[0], oldviewport[1], oldviewport[2], oldviewport[3] );
         m_szControl.cx = nOldWidth;

         SetWindowsProjectionMatrix();
         if (!m_bDoPick)
         {
            glDisable( GL_DEPTH_TEST );
            glColor3f( 0,0,0 );
            glBegin( GL_LINES );
            glVertex2f( m_szControl.cx/2+0.5f, 0 );
            glVertex2f( m_szControl.cx/2+0.5f, m_szControl.cy );
            glEnd();
         }

         break;
      }
   }
}

SnipeObjectTypeArray *CGLMotionSetView::GetSelectableObjectTypeArray() const
{
   return &GetDocument()->m_MotionSetSelectableObjectTypeArray;
}

void CGLMotionSetView::DoPick( const CRect &rcPick, PickHitArray &pickHitArray, BOOL *bOutSinglePick/*=NULL*/, SnipeObjectType kindof/*=SOT_Unknown*/, SnipeObjectList *pExcludeList/*=NULL*/ )
{
   CPoint ptControl = m_ptControl;
   CSize szControl = m_szControl;
   if (m_nViewMode == VIEWMODE_SIDEBYSIDE)
   {
      m_szControl.cx /= 2;
      if (rcPick.left > m_szControl.cx)
         m_ptControl.x += m_szControl.cx;
   }

   CGLSceneView::DoPick( rcPick, pickHitArray, bOutSinglePick, kindof, pExcludeList );

   m_ptControl = ptControl;
   m_szControl = szControl;
}
