
#include "stdafx.h"
#include "Resource.h"
#include "GLImageView.h"

IMPLEMENT_DYNCREATE(CGLImageView, CGL2DView)

BEGIN_MESSAGE_MAP(CGLImageView, CGL2DView)
END_MESSAGE_MAP()

CGLImageView::CGLImageView()
{
   m_nID = IDR_IMAGEVIEW;
}

CGLImageView::~CGLImageView()
{
}

#define PADDING 1.03f

void CGLImageView::DefaultZoomFit(SnipeObjectList *pList)
{
   float fHalfFOVy = m_pCurCamera->m_fFOV * 0.5f;
   float fHalfHeight = 1.0f * PADDING * 0.5f;
   float fDistanceY = fHalfHeight / tan( fHalfFOVy * DEGTORAD );

   float fAspect = (GLdouble)m_szControl.cx/(double)m_szControl.cy;
   float fHalfFOVx = fHalfFOVy * fAspect;
   float fHalfWidth = 1.0f * PADDING * 0.5f;
   float fDistanceX = fHalfWidth / tan( fHalfFOVx * DEGTORAD );

   float fDistance = max( fDistanceX, fDistanceY );
   if (!fDistance)
      fDistance = GetCamera()->m_fFocusDistance;

   Vector newtarget( 0.0f );
   ViewToWorld( newtarget );
   Vector vRotate = m_pCurCamera->m_transform.m_vRotate;
   m_pCurCamera->CalculateTransform( newtarget, fDistance, vRotate.y, vRotate.x );

   Invalidate();
}

void CGLImageView::DrawContents()
{
   SetProjectionMatrix();

   CGLBaseView::DrawContents();
}

