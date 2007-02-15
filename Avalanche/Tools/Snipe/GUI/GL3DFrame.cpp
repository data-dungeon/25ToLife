#include "stdafx.h"
#if 0
#include "GL3DFrame.h"
#include "GL3DView.h"
#include "Resource.h"

IMPLEMENT_DYNAMIC(CGL3DFrame, CGLHeaderFrame)

CGL3DFrame::CGL3DFrame()
{
}

CGL3DFrame::~CGL3DFrame()
{
}

void CGL3DFrame::UpdateHeader()
{
   CGL3DView *pGL3DView = (CGL3DView *)m_pView;

   m_glViewAngleComboControl.SetCurSel( pGL3DView->m_nViewAngle, FALSE );

   CGLHeaderFrame::UpdateHeader();
}

void CGL3DFrame::PostInit()
{
   CGLHeaderFrame::PostInit();

   m_glViewAngleComboControl.Init( &m_glHeaderControl, NULL, m_glViewModeComboControl.m_ptControl+CPoint(m_glViewModeComboControl.m_szControl.cx+5,0), CSize(94,HEADERHEIGHT), ID_VIEW_ANGLEPRESET_COMBO );

   CGL3DView *pGL3DView = GetView();
   if (pGL3DView)
   {
      int nCount = pGL3DView->m_viewanglepreset.GetCount();
      for (int i=0; i<nCount; i++)
         m_glViewAngleComboControl.AddString( GetView()->m_viewanglepreset[i].m_name, i );
      m_glViewAngleComboControl.SetCurSel( pGL3DView->m_nViewAngle );
   }
}
#endif