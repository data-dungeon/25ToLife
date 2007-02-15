#include "stdafx.h"
#if 0
#include "GLTexCoordFrame.h"
#include "GLTexCoordView.h"
#include "TexCoords.h"
#include "Resource.h"

IMPLEMENT_DYNCREATE(CGLTexCoordFrame, CGL2DFrame)

CGLTexCoordFrame::CGLTexCoordFrame()
{
   m_bRebuildContextCombo = TRUE;
}

CGLTexCoordFrame::~CGLTexCoordFrame()
{
}

CGLBaseView *CGLTexCoordFrame::CreateView()
{
   return SNEW CGLTexCoordView;
}

void CGLTexCoordFrame::PostInit()
{
   CGL2DFrame::PostInit();

   m_glContextCombo.Init( &m_glHeaderControl, NULL, CPoint(5,0), CSize(144,HEADERHEIGHT), ID_TEXCOORDINSTANCE_COMBO );

   UpdateHeader();
}

void CGLTexCoordFrame::UpdateHeader()
{
   if (m_bRebuildContextCombo)
   {
      FillContextCombo();
      m_bRebuildContextCombo = FALSE;
   }

   CGL2DFrame::UpdateHeader();
}

void CGLTexCoordFrame::FillContextCombo()
{
   m_glContextCombo.ResetContent();

   CGLTexCoordView *pView = GetView();
   TexCoords *pTexCoords = NULL;// TODO pView->GetDocument()->GetTexCoords();
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
}

BOOL CGLTexCoordFrame::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYCTRL_CB_CLICKED:
      {
         UINT nID = LOWORD(wParam);
         int nIndex = HIWORD(wParam);
         switch (nID)
         {
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
            case ID_TEXCOORDINSTANCE_COMBO:
            {
               TexCoords *pTexCoords = NULL;//TODO:GetView()->GetDocument()->GetTexCoords();
               if (pTexCoords)
                  pTexCoords->m_pCurInstance = (SnipeObject *)m_glContextCombo.GetItemData( m_glContextCombo.GetCurSel());
               return TRUE;
            }
         }
         break;
      }
   }

   return CGL2DFrame::OnControlNotify( msg, wParam, lParam );
}
#endif