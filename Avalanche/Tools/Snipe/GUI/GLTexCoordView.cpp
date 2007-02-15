//
#include "stdafx.h"
#include "Snipe.h"
#include "TexCoords.h"
#include "GLTexCoordView.h"
#include "GLModelFrame.h"
#include "ModelDoc.h"
#include "PolyModel.h"
#include "Interface/SnipeNavigator.h"
#include "Resource.h"

IMPLEMENT_DYNCREATE(CGLTexCoordView, CGL2DView)

BEGIN_MESSAGE_MAP(CGLTexCoordView, CGL2DView)
END_MESSAGE_MAP()

BoolProperty::Info CGLTexCoordView::m_bDisplayHiddenEdgesInfo( "Hidden Edges", "Hidden Edges", false );

CGLTexCoordView::CGLTexCoordView()
{
   m_nID = IDR_TEXCOORDVIEW;
   m_pCurTexCoords = NULL;
   m_bRebuildTexCoordsCombo = TRUE;
   m_bRebuildContextCombo = TRUE;

   m_bDisplayHiddenEdges.Init( &m_bDisplayHiddenEdgesInfo, &m_categoryDisplay );
}

CGLTexCoordView::~CGLTexCoordView()
{
}

#define ZOOMPADDING 1.03f

void CGLTexCoordView::DefaultZoomFit(SnipeObjectList *pList)
{
   ASSERT(pList);

   BoundingBox bbox;
   pList->ComputeBoundingBox( bbox, this  );
   if (bbox.IsEmpty())
      return;

   float fHalfFOVy = m_pCurCamera->m_fFOV * 0.5f;
   float fHalfHeight = bbox.GetHeight() * ZOOMPADDING * 0.5f;
   float fDistanceY = fHalfHeight / tan( fHalfFOVy * DEGTORAD );
   fDistanceY += bbox.GetDepth() * 0.5f;

   float fAspect = (GLdouble)m_szControl.cx/(double)m_szControl.cy;
   float fHalfFOVx = fHalfFOVy * fAspect;
   float fHalfWidth = bbox.GetWidth() * ZOOMPADDING * 0.5f;
   float fDistanceX = fHalfWidth / tan( fHalfFOVx * DEGTORAD );
   fDistanceX += bbox.GetDepth() * 0.5f;

   float fDistance = max( fDistanceX, fDistanceY );
   if (!fDistance)
      fDistance = GetCamera()->m_fFocusDistance;

   Vector newtarget = bbox.CalculateCenterPoint();
   ViewToWorld( newtarget );
   Vector vRotate = m_pCurCamera->m_transform.m_vRotate;
   m_pCurCamera->CalculateTransform( newtarget, fDistance, vRotate.y, vRotate.x );
   Invalidate();
}

void CGLTexCoordView::SetProjectionMatrix()
{
   // clear projection matrix
   glMatrixMode(GL_PROJECTION);
   if (!m_bDoPick)
      glLoadIdentity();
   float fHeight = tan( m_pCurCamera->m_fFOV / 2.0f * DEGTORAD ) * m_pCurCamera->m_fFocusDistance;
   float fWidth = fHeight * ((float)m_szControl.cx / (float)m_szControl.cy);
   fHeight = -fHeight; // flip the Y because UVs have V's that go 0 to 1 from top to bottom
   glOrtho( -fWidth, fWidth, -fHeight, fHeight, m_pCurCamera->m_fFocusDistance+10.0f, m_pCurCamera->m_fFocusDistance-10.0f );
   glGetDoublev( GL_PROJECTION_MATRIX, m_GLProjectionMatrix );

   // setup view transform matrix
   glMatrixMode(GL_MODELVIEW);
   GLfloat glmatrix[16];
   GetCamera()->GetWorldToObjectMatrix().GetFloatMatrix( glmatrix, true );
   glLoadMatrixf( glmatrix );
   glGetDoublev( GL_MODELVIEW_MATRIX, m_GLViewTransformMatrix );
}

SnipeObject *CGLTexCoordView::GetObjectHead()
{
   return m_pCurTexCoords;
}

void CGLTexCoordView::DrawContents()
{
   TexCoords *pTexCoords = GetTexCoords();
   if (!pTexCoords)
      return;

   SetProjectionMatrix();

   if (!m_bDoPick && GetManipulatorInstance())
      GetManipulatorInstance()->PreDraw(this);

   if (!m_bDoPick && GetNavigatorInstance())
      GetNavigatorInstance()->PreDraw(this);

   float fAspect = 1.0f;
   if (pTexCoords->m_pCurInstance)
      fAspect = pTexCoords->m_pCurInstance->DrawUVExtras( this );

   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();
   glScalef( fAspect, 1.0f, 1.0f );

   pTexCoords->DrawUVs( this );

   if (!m_bDoPick)
      DrawSelectedObjects();
   //DrawStatistics();

   glPopMatrix();

   if (!m_bDoPick && GetNavigatorInstance())
      GetNavigatorInstance()->PostDraw(this);

   if (!m_bDoPick && GetManipulatorInstance())
      GetManipulatorInstance()->PostDraw(this);
}

SnipeObjectTypeArray *CGLTexCoordView::GetSelectableObjectTypeArray() const
{
   return &GetDocument()->m_TexCoordSelectableObjectTypeArray;
}

void CGLTexCoordView::PostInit()
{
   CGL2DView::PostInit();

   m_glTexCoordsCombo.Init( &GetHeaderFrame()->m_glHeaderControl, NULL, GetHeaderFrame()->m_glViewModeComboControl.m_ptControl+CPoint(GetHeaderFrame()->m_glViewModeComboControl.m_szControl.cx+5,0), CSize(144,CGLModelFrame::HEADERHEIGHT), ID_TEXCOORD_COMBO );
   m_glContextCombo.Init( &GetHeaderFrame()->m_glHeaderControl, NULL, m_glTexCoordsCombo.m_ptControl+CPoint(m_glTexCoordsCombo.m_szControl.cx+5,0), CSize(144,CGLModelFrame::HEADERHEIGHT), ID_TEXCOORDINSTANCE_COMBO );

   UpdateHeader();
   ZoomFitAll();
}

void CGLTexCoordView::UpdateHeader()
{
   if (m_bRebuildTexCoordsCombo)
      FillTexCoordsCombo();

   if (m_bRebuildContextCombo)
      FillContextCombo();

   CGL2DView::UpdateHeader();
}

void CGLTexCoordView::FillTexCoordsCombo()
{
   m_glTexCoordsCombo.ResetContent();

   PolyModel *pModel = GetDocument()->GetCurrentModel();
   TexCoordsContainer *pTexCoordsContainer = NULL;
   if (pModel)
      pTexCoordsContainer = pModel->GetTexCoordsContainer();

   if (pTexCoordsContainer)
   {
      UINT nNumTexCoords = pTexCoordsContainer->GetNumTexCoords();
      for (UINT i=0; i<nNumTexCoords; i++)
      {
         TexCoords *pTexCoords = pTexCoordsContainer->GetTexCoordsAt( i );
         int nIndex = m_glTexCoordsCombo.AddObject( pTexCoords );
         if (pTexCoords == m_pCurTexCoords)
            m_glTexCoordsCombo.SetCurSel( nIndex );
      }
   }

   if (!m_pCurTexCoords && m_glTexCoordsCombo.GetCount())
      m_glTexCoordsCombo.SetCurSel( 0 );

   m_bRebuildTexCoordsCombo = FALSE;
}

void CGLTexCoordView::FillContextCombo()
{
   m_glContextCombo.ResetContent();

   TexCoords *pTexCoords = GetTexCoords();
   if (pTexCoords)
   {
      for (NotifyNode *pNotifyNode=pTexCoords->m_pNotifyNodeHead; pNotifyNode; pNotifyNode=pNotifyNode->m_pNextNotifyNode)
      {
         SnipeObject *pObject = pNotifyNode->GetSnipeObject();
         if (pObject && pObject->GetSOT()==SOT_TexCoordsPointerProperty)
         {
            Property *pProperty = (Property *)pObject;
            SnipeObject *pObject = pProperty->GetObject();
            int nIndex = m_glContextCombo.AddObject( pObject );
            if (pObject == pTexCoords->m_pCurInstance)
               m_glContextCombo.SetCurSel( nIndex );
         }
      }

      if (!pTexCoords->m_pCurInstance && m_glContextCombo.GetCount())
         m_glContextCombo.SetCurSel( 0 );
   }

   m_bRebuildContextCombo = FALSE;
}

BOOL CGLTexCoordView::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYCTRL_CB_CLICKED:
      {
         UINT nID = LOWORD(wParam);
         int nIndex = HIWORD(wParam);
         switch (nID)
         {
            case ID_TEXCOORD_COMBO:
               FillTexCoordsCombo();
               return TRUE;
            case ID_TEXCOORDINSTANCE_COMBO:
               FillContextCombo();
               return TRUE;
         }
         break;
      }
      case NOTIFYCTRL_CB_CHANGED:
      {
         UINT nID = LOWORD(wParam);
         int nIndex = HIWORD(wParam);
         switch (nID)
         {
            case ID_TEXCOORD_COMBO:
            {
               m_pCurTexCoords = (TexCoords *)m_glTexCoordsCombo.GetItemData( m_glTexCoordsCombo.GetCurSel() );
               FillContextCombo();
               return TRUE;
            }
            case ID_TEXCOORDINSTANCE_COMBO:
            {
               TexCoords *pTexCoords = GetTexCoords();
               if (pTexCoords)
                  pTexCoords->m_pCurInstance = (SnipeObject *)m_glContextCombo.GetItemData( m_glContextCombo.GetCurSel() );
               return TRUE;
            }
         }
         break;
      }
   }

   return CGL2DView::OnControlNotify( msg, wParam, lParam );
}
