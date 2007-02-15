#include "stdafx.h"
#include "Resource.h"
#include "GLSchematicView.h"
#include "BaseDoc.h"
#include "Interface/SnipeNavigator.h"

IMPLEMENT_DYNCREATE(CGLSchematicView, CGL2DView)

BEGIN_MESSAGE_MAP(CGLSchematicView, CGL2DView)
END_MESSAGE_MAP()

CGLSchematicView::CGLSchematicView()
{
   m_nID = IDR_IMAGEVIEW;
}

CGLSchematicView::~CGLSchematicView()
{
}

#define PADDING 1.03f

void CGLSchematicView::DefaultZoomFit(SnipeObjectList *pList)
{
   if (((HierObject *)GetDocument()->GetObjectHead())->m_bSchematicPositionsDirty)
      ((HierObject *)GetDocument()->GetObjectHead())->ComputeSchematicPositions();

   int nLevels = 8; // TODO
   float fWidth = nLevels * 150.0f + 100.0f;
   float fHeight = 20 * 30.0f + 20.0f;

   float fHalfFOVy = m_pCurCamera->m_fFOV * 0.5f;
   float fHalfHeight = fHeight * PADDING * 0.5f;
   float fDistanceY = fHalfHeight / tan( fHalfFOVy * DEGTORAD );

   float fAspect = (GLdouble)m_szControl.cx/(double)m_szControl.cy;
   float fHalfFOVx = fHalfFOVy * fAspect;
   float fHalfWidth = fWidth * PADDING * 0.5f;
   float fDistanceX = fHalfWidth / tan( fHalfFOVx * DEGTORAD );

   float fDistance = max( fDistanceX, fDistanceY );
   if (!fDistance)
      fDistance = GetCamera()->m_fFocusDistance;

   Vector newtarget( fWidth / 2.0f, 0.0f, 0.0f );
   Vector vRotate = m_pCurCamera->m_transform.m_vRotate;
   m_pCurCamera->CalculateTransform( newtarget, fDistance, vRotate.y, vRotate.x );

   //Vector vCamera(0.0f);
   //vCamera *= m_pCurCamera->GetObjectToWorldMatrix();
   //fDistance = fabs((newtarget - vCamera).Length());
   //m_pCurCamera->m_fFocusDistance.StoreValue( fDistance );

   Invalidate();
}

void CGLSchematicView::DrawContents()
{
   SetProjectionMatrix();

   if (!m_bDoPick && GetManipulatorInstance())
      GetManipulatorInstance()->PreDraw(this);

   if (!m_bDoPick && GetNavigatorInstance())
      GetNavigatorInstance()->PreDraw(this);

   GetDocument()->GetObjectHead()->DrawSchematic( this );

   if (!m_bDoPick && GetNavigatorInstance())
      GetNavigatorInstance()->PostDraw(this);

   if (!m_bDoPick && GetManipulatorInstance())
      GetManipulatorInstance()->PostDraw(this);

   if (!m_bDoPick)
      DrawViewControls();
}
