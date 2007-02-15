//
#include "stdafx.h"
#include "ModelDoc.h"
#include "Objects/Project.h"
#include "MainFrm.h"
#include "GLModelFrame.h"
#include "Resource.h"

IMPLEMENT_DYNCREATE(C3DDoc, CBaseDoc)

BEGIN_MESSAGE_MAP(C3DDoc, CBaseDoc)
END_MESSAGE_MAP()

C3DDoc::C3DDoc()
{
   m_fBoundingRadius = 1.0f;
   m_vBoundingCenter.Set( 0.0f );
}

C3DDoc::~C3DDoc()
{
}

void C3DDoc::RebuildLightArray( SnipeObject *pStartObj/*=NULL*/ )
{
   if (pStartObj==NULL) {
      pStartObj = m_pObjectHead;
      m_lightarray.RemoveAll();
   }

   for (int nChild=0; nChild<pStartObj->GetNumChildren(); nChild++) {
      SnipeObject *pObj = pStartObj->GetChildAt( nChild );
      if (pObj->GetSOT() == SOT_Light)
         m_lightarray.Add( (Light *)pObj );
      RebuildLightArray( pObj );
   }
}

void C3DDoc::RebuildCameraArray( SnipeObject *pStartObj/*=NULL*/ )
{
   BOOL bFillViewAngleCombos = FALSE;
   if (pStartObj==NULL)
   {
      bFillViewAngleCombos = TRUE;
      pStartObj = m_pObjectHead;
      m_cameraarray.RemoveAll();
   }

   for (int nChild=0; nChild<pStartObj->GetNumChildren(); nChild++)
   {
      SnipeObject *pObj = pStartObj->GetChildAt( nChild );
      if (pObj->GetSOT() == SOT_Camera)
         m_cameraarray.Add( (Camera *)pObj );
      RebuildCameraArray( pObj );
   }

   if (bFillViewAngleCombos)
   {
      for (POSITION pos=m_viewlist.GetHeadPosition(); pos; )
      {
         CGLBaseView *pView = m_viewlist.GetNext( pos );
// TODO         ((CGLModelFrame *)pView->GetParentFrame())->FillViewAngleCombo();
      }
   }
}

void C3DDoc::SetObjectHead( HierObject *pObjectHead )
{
   CBaseDoc::SetObjectHead( pObjectHead );

   RebuildCameraArray();
   RebuildLightArray();
}

BOOL C3DDoc::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg) {
      case NOTIFYOBJ_ADDEDDESCENDANT:
      case NOTIFYOBJ_REMOVEDDESCENDANT: {
         HierObject *pObj = (HierObject *)lParam;
         if (pObj->GetSOT() == SOT_Camera)
            RebuildCameraArray();
         if (pObj->GetSOT() == SOT_Light)
            RebuildLightArray();
         break;
      }
   }
   return CBaseDoc::OnObjectNotify( msg, wParam, lParam );
}
