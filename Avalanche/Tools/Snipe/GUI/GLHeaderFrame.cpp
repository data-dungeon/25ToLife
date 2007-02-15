#include "stdafx.h"
#include "GLHeaderFrame.h"
#include "GLHeaderView.h"
#include "Snipe.h"
#include "BaseDoc.h"
#include "resource.h"

IMPLEMENT_DYNCREATE(CGLHeaderFrame, CGLBaseFrame)

CGLHeaderFrame::CGLHeaderFrame()
{
   m_nNextControlPos = 5;
}

CGLHeaderFrame::~CGLHeaderFrame()
{
   for (int i=0; i<m_glHeaderControl.m_childcontrolarray.GetCount(); i++)
      m_glHeaderControl.m_childcontrolarray[i]->m_bAutoDelete = FALSE;
}

void CGLHeaderFrame::AddHeaderControl( CGLControl *pControl, const CPoint &ptControlOffset, const CSize &szControl, UINT nID )
{
   pControl->Init( &m_glHeaderControl, NULL, CPoint(m_nNextControlPos+ptControlOffset.x,ptControlOffset.y), szControl, nID );
   m_nNextControlPos += ptControlOffset.x + szControl.cx;
}

void CGLHeaderFrame::PostInitGuts()
{
   CGLBaseFrame::PostInit();

   m_glHeaderControl.Init( this );

   m_glMaximizeButton.Init( &m_glHeaderControl, NULL, CPoint(BUTTONWIDTH+5,0), CSize(BUTTONWIDTH,HEADERHEIGHT+1), ID_MAXIMIZE, JUST_RIGHT );
   m_glRestoreButton.Init( &m_glHeaderControl, NULL, CPoint(BUTTONWIDTH+5,0), CSize(BUTTONWIDTH,HEADERHEIGHT+1), ID_RESTORE, JUST_RIGHT );
}

void CGLHeaderFrame::PostInit()
{
   PostInitGuts();

   CreateView();
}

void CGLHeaderFrame::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   CGLBaseFrame::Move( ptControl, szControl, justification );

   m_glHeaderControl.Move( CPoint( 0, 0 ), CSize( szControl.cx, HEADERHEIGHT ));
   m_pView->Move( CPoint( 0, HEADERHEIGHT ), CSize( szControl.cx, szControl.cy-HEADERHEIGHT ));
}

BOOL CGLHeaderFrame::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   if (msg != NOTIFYCTRL_REMOVECHILD)
      if (m_pView->OnControlNotify( msg, wParam, lParam ))
         return TRUE;

   switch (msg)
   {
      case NOTIFYCTRL_BN_CLICKED:
      {
         UINT nID = LOWORD(wParam);
         switch (nID)
         {
            case ID_MAXIMIZE:
               OnMaximize();
               return TRUE;
            case ID_RESTORE:
               OnRestore();
               return TRUE;
         }
         return FALSE;
      }
      case NOTIFYCTRL_CB_CHANGED:
         return FALSE;
   }

   return CGLBaseFrame::OnControlNotify( msg, wParam, lParam );
}

void CGLHeaderFrame::UpdateHeader()
{
   static CRuntimeClass *pRuntimeClass = RUNTIME_CLASS(CGLBaseDocFrame);
   m_glMaximizeButton.SetVisible( !m_bMaximized && !GetParent()->IsKindOf( pRuntimeClass ) );
   m_glRestoreButton.SetVisible( m_bMaximized );
}

BOOL CGLHeaderFrame::OnLButtonDown( UINT nFlags, CPoint point )
{
   if (m_pView->MakeActive( TRUE ))
      GetApp().DrawInvalidControls();

   return CGLBaseFrame::OnLButtonDown( nFlags, point );
}
