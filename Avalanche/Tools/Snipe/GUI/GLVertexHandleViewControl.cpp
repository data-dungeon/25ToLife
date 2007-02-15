#include "stdafx.h"
#include "GLVertexHandleViewControl.h"
#include "PatchEdge.h"
#include "GL3DView.h"
#include "PatchVertex.h"


CGLVertexHandleViewControl::CGLVertexHandleViewControl( CGLBaseView *pBaseView, PatchEdge *pPatchEdge, PatchVertex *pPatchVertex  ) :
CGLViewControl( pBaseView )
{
   m_pPatchEdge = pPatchEdge;
   m_pPatchVertex = pPatchVertex;
   m_pVertexHandle = pPatchEdge->GetVertexHandle(pPatchVertex);
   // get opposite tangent
   m_pOppositeVertexHandle = NULL;
   m_pOppositeEdge = NULL;
   PatchEdge *pPrevContEdge = m_pPatchEdge->GetPrevContEdge(TRUE);
   PatchEdge *pNextContEdge = m_pPatchEdge->GetNextContEdge(TRUE);
   if ((pPrevContEdge)&&(pPrevContEdge->GetSharedVertex(m_pPatchEdge) == pPatchVertex))
   {
      m_pOppositeVertexHandle = pPrevContEdge->GetVertexHandle(m_pPatchVertex);
      m_pOppositeEdge = pPrevContEdge;
   }
   else if ((pNextContEdge)&&(pNextContEdge->GetSharedVertex(m_pPatchEdge) == pPatchVertex))
   {
      m_pOppositeVertexHandle = pNextContEdge->GetVertexHandle(m_pPatchVertex);
      m_pOppositeEdge = pNextContEdge;
   }

   m_bIsDragging = FALSE;
}

BOOL CGLVertexHandleViewControl::IsFor( void *pData )
{
   return (pData == m_pPatchEdge->GetVertexHandle(m_pPatchVertex));
}

void CGLVertexHandleViewControl::Draw()
{
#if 1
   if (!m_pBaseView->m_bDoPick)
   {
      Vector vAlphaPlane,vGammaPlane;
      Vector vUnModifiedTangent = m_pPatchEdge->CalculateTangent(m_pPatchVertex,vAlphaPlane,vGammaPlane); 

      const Vector &pV1 = m_pPatchEdge->GetPrevContVertex()->GetPos();
      const Vector &pV2 = m_pPatchEdge->GetNextContVertex()->GetPos();

      vUnModifiedTangent.Normalize();

      Vector vCenter;
      vCenter = pV2 + pV1;
      vCenter *= 0.5f;

      Vector vEnd;
      vEnd = vCenter + vAlphaPlane;

      glColor3f(0.0f,1.0f,0.0f);

      glBegin(GL_LINES);
      glVertex3fv(&vCenter.x);
      glVertex3fv(&vEnd.x);
      glEnd();

      vEnd = vCenter + vGammaPlane;
      glBegin(GL_LINES);
      glVertex3fv(&vCenter.x);
      glVertex3fv(&vEnd.x);
      glEnd();

   }

#endif




   // push 2d projection
   {
      glMatrixMode( GL_MODELVIEW );
      glPushMatrix();
      glLoadIdentity();
      glMatrixMode( GL_PROJECTION );
      glPushMatrix();
      if (!m_pBaseView->m_bDoPick)
         glLoadIdentity();
      CRect rcControl = m_pBaseView->GetViewportRect();
      glOrtho( 0, rcControl.Width(), rcControl.Height(), 0, -1, 1 );
   }

   if (m_pBaseView->m_bDoPick)
   {
      glLoadName( (GLuint)this );
      glPushName( (GLuint)1 ); // in tangent
   }

   const double PI = 3.141592653589793238462643383279502884197169399375105820974944592308;

   Vector knobpos;
   knobpos = m_pPatchVertex->GetPos();
   knobpos += m_pVertexHandle->GetTangent() * 0.3333f;

   Vector v0,v1;
   v0 = m_pPatchVertex->GetPos();
   v1 = knobpos;
   m_pBaseView->WorldToScreen(v0);
   m_pBaseView->WorldToScreen(v1);

   glColor3f(1.0f,1.0f,0.0f);
   if (!m_pBaseView->m_bDoPick)
   {
      glBegin(GL_LINES);
      glVertex3fv(&v0.x);
      glVertex3fv(&v1.x);
      glEnd();
   }

   float fRadius = 6.0f;
   int nSegments = 8;

   if (m_pBaseView->m_bDoPick||m_bIsDragging)
      glBegin(GL_POLYGON);
   else
      glBegin(GL_LINE_LOOP);

   Vector2 v;
   for(int i=0;i<nSegments;i++)
   {
      v.Set(fRadius * cos(i*2.0f*PI/nSegments)+v1.x,
         fRadius * sin(i*2.0f*PI/nSegments)+v1.y);
      glVertex2fv(&v.x);
   }
   glEnd();

   if (m_pBaseView->m_bDoPick)
      glLoadName( (GLuint)2 ); // out tangent

   if (m_pOppositeVertexHandle&&!m_pBaseView->m_bDoPick)
   {
      knobpos = m_pPatchVertex->GetPos();
      knobpos += m_pOppositeVertexHandle->GetTangent() * 0.3333f;

      v1 = knobpos;
      m_pBaseView->WorldToScreen(v1);

      if (!m_pBaseView->m_bDoPick)
      {
         glBegin(GL_LINES);
         glVertex3fv(&v0.x);
         glVertex3fv(&v1.x);
         glEnd();

         glBegin(GL_LINE_LOOP);
         for(int i=0;i<nSegments;i++)
         {
            v.Set(fRadius * cos(i*2.0f*PI/nSegments)+v1.x,
               fRadius * sin(i*2.0f*PI/nSegments)+v1.y);
            glVertex2fv(&v.x);
         }
         glEnd();
      }
   }

   if (m_pBaseView->m_bDoPick)
   {
      glPopName(); // pop off tangent sub
      glLoadName( NULL );
   }

   {
      glMatrixMode( GL_PROJECTION );
      glPopMatrix();
      glMatrixMode( GL_MODELVIEW );
      glPopMatrix();
   }
}

BOOL CGLVertexHandleViewControl::OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   m_bIsDragging = TRUE;
   m_mouseonline.Init( (HCGLBaseView*)m_pBaseView, m_pPatchVertex->GetPos(), m_pVertexHandle->GetTangent() );
   m_vStart = m_mouseonline.GetWorldPos( ptMouse );

   return TRUE;
}

BOOL CGLVertexHandleViewControl::OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{

   MouseOnLine mouseonline;
   Vector vPos;
   vPos = m_mouseonline.GetWorldPos( ptMouse );

   Vector vAlphaPlane,vGammaPlane;
   Vector vUnModifiedTangent = m_pPatchEdge->CalculateTangent(m_pPatchVertex,vAlphaPlane,vGammaPlane); 
   float fUnModifiedLength = vUnModifiedTangent.Length();

   Vector vMouseToVertex = vPos - m_pPatchVertex->GetPos();
   float fLength = vMouseToVertex.Length();
   vMouseToVertex.Normalize();
   vAlphaPlane.Normalize();
   double angle = (acos(Dot(vMouseToVertex,vAlphaPlane)))/2.0;

   g_outputWindow << String(angle) << "\n";

 //  m_pVertexHandle->GetMagnitude()->StoreValue(fLength*3.0f/fUnModifiedLength);
   m_pVertexHandle->GetAlpha()->StoreValue(RADTODEG * angle);
//   m_pVertexHandle->GetGamma()->StoreValue(angle);

   if ((m_pOppositeVertexHandle)&&(keymodifier.GetValue() != KEYMOD_CTRL))
   {
      m_pOppositeVertexHandle->GetMagnitude()->StoreValue(fLength*3.0f/fUnModifiedLength);

   }

   m_pBaseView->Invalidate();

   return TRUE;
}

BOOL CGLVertexHandleViewControl::OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   m_bIsDragging = FALSE;
   m_pBaseView->Invalidate();
   return TRUE;
}

KeyModifiers CGLVertexHandleViewControl::GetKeyModifiers( MouseStates mousestate, const PickHit &pickHit )
{
   switch (mousestate.GetValue())
   {
   case MOUSESTATE_LEFT:
      return KeyModifiers( KEYMOD_NONE | KEYMOD_CTRL );
   default:
      return KeyModifiers( KEYMOD_NOTSUPPORTED );
   }
}
