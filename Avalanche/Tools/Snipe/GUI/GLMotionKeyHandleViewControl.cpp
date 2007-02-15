#include "stdafx.h"
#include "GLMotionKeyHandleViewControl.h"

//#include "GL3DView.h"
#include "MotionKey.h"


CGLMotionKeyHandleViewControl::CGLMotionKeyHandleViewControl( CGLBaseView *pBaseView, MotionKey *pMotionKey  ) :
CGLViewControl( pBaseView )
{
   m_pMotionKey = pMotionKey;
   m_bIsDragging = FALSE;
}

BOOL CGLMotionKeyHandleViewControl::IsFor( void *pData )
{
   return (pData == m_pMotionKey);
}

void CGLMotionKeyHandleViewControl::Draw()
{
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

   FloatMotionKey *pFloatMotionKey = (FloatMotionKey*)m_pMotionKey;

   glColor3f(1.0f,1.0f,0.0f);

   Vector v0(pFloatMotionKey->GetTime().GetSeconds(),pFloatMotionKey->GetValue(),0.0f);
   Vector v1;
   Vector2 v2Tangent = pFloatMotionKey->GetInTangent() * 0.3333f;
   Vector vTangent(v2Tangent.x,v2Tangent.y,0.0f);
   v1 = -vTangent + v0;

   m_pBaseView->WorldToScreen(v0);
   m_pBaseView->WorldToScreen(v1);
   v0.z = 0.0f;
   v1.z = 0.0f;

   if (!m_pBaseView->m_bDoPick)
   {
      glBegin(GL_LINES);
      glVertex3fv(&v0.x);
      glVertex3fv(&v1.x);
      glEnd();
   }

   float fRadius = 5.0f;
   int nSegments = 8;

   if (m_pBaseView->m_bDoPick)
   {
      glLoadName( (GLuint)this );
      glPushName( (GLuint)1 ); // in tangent
   }


   const double PI = 3.141592653589793238462643383279502884197169399375105820974944592308;

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

   v0.Set(pFloatMotionKey->GetTime().GetSeconds(),pFloatMotionKey->GetValue(),0.0f);
   v2Tangent = pFloatMotionKey->GetOutTangent() * 0.3333f;
   vTangent.Set(v2Tangent.x,v2Tangent.y,0.0f);
   v1 = vTangent + v0;

   m_pBaseView->WorldToScreen(v0);
   m_pBaseView->WorldToScreen(v1);
   v0.z = 0.0f;
   v1.z = 0.0f;

   if (!m_pBaseView->m_bDoPick)
   {
      glBegin(GL_LINES);
      glVertex3fv(&v0.x);
      glVertex3fv(&v1.x);
      glEnd();
   }

   if (m_pBaseView->m_bDoPick||m_bIsDragging)
      glBegin(GL_POLYGON);
   else
      glBegin(GL_LINE_LOOP);

   for(int i=0;i<nSegments;i++)
   {
      v.Set(fRadius * cos(i*2.0f*PI/nSegments)+v1.x,
         fRadius * sin(i*2.0f*PI/nSegments)+v1.y);
      glVertex2fv(&v.x);
   }
   glEnd();

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

BOOL CGLMotionKeyHandleViewControl::OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   m_bIsDragging = TRUE;
   return TRUE;
}

BOOL CGLMotionKeyHandleViewControl::OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   FloatMotionKey *pFloatMotionKey = (FloatMotionKey*)m_pMotionKey;

   Vector vMotionKeyPos(pFloatMotionKey->GetTime().GetSeconds(),pFloatMotionKey->GetValue(),0.0f);
   Vector vMousePos(ptMouse.x,ptMouse.y,0.0f);
   m_pBaseView->ScreenToWorld(vMousePos);

   if (keymodifier == KEYMOD_CTRL)
   {
      pFloatMotionKey->SetPeaked(TRUE);
   }

   // picking in tangent
   if (pickHit[1]==1)
   {
      Vector2 vNewTangentPos(vMousePos.x - vMotionKeyPos.x,vMousePos.y - vMotionKeyPos.y);
      vNewTangentPos *= -3.0f;
      pFloatMotionKey->SetInTangent(vNewTangentPos);
      if (!pFloatMotionKey->IsPeaked())
      {
         float inAlpha = pFloatMotionKey->GetInBiasHandle()->GetAlpha()->GetValue();
         float inMagnitude = pFloatMotionKey->GetInBiasHandle()->GetMagnitude()->GetValue();
         pFloatMotionKey->GetOutBiasHandle()->GetAlpha()->StoreValue(inAlpha);
         pFloatMotionKey->GetOutBiasHandle()->GetMagnitude()->StoreValue(inMagnitude);
      }
   }
   // picking out tangent
   else
   {
      Vector2 vNewTangentPos(vMousePos.x - vMotionKeyPos.x,vMousePos.y - vMotionKeyPos.y);
      vNewTangentPos *= 3.0f;
      pFloatMotionKey->SetOutTangent(vNewTangentPos);
      if (!pFloatMotionKey->IsPeaked())
      {
         float outAlpha = pFloatMotionKey->GetOutBiasHandle()->GetAlpha()->GetValue();
         float outMagnitude = pFloatMotionKey->GetOutBiasHandle()->GetMagnitude()->GetValue();
         pFloatMotionKey->GetInBiasHandle()->GetAlpha()->StoreValue(outAlpha);
         pFloatMotionKey->GetInBiasHandle()->GetMagnitude()->StoreValue(outMagnitude);
      }
   }

   m_pBaseView->Invalidate();

   return TRUE;
}

BOOL CGLMotionKeyHandleViewControl::OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit )
{
   m_bIsDragging = FALSE;
   m_pBaseView->Invalidate();
   return TRUE;
}

KeyModifiers CGLMotionKeyHandleViewControl::GetKeyModifiers( MouseStates mousestate, const PickHit &pickHit )
{
   switch (mousestate.GetValue())
   {
      case MOUSESTATE_LEFT:
         return KeyModifiers( KEYMOD_NONE | KEYMOD_CTRL );
      default:
         return KeyModifiers( KEYMOD_NOTSUPPORTED );
   }
}
