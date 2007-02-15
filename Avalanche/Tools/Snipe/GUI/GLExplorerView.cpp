#include "stdafx.h"
#include "GLExplorerView.h"
#include "BaseDoc.h"
#include "GLBaseFrame.h"
#include "Resource.h"

IMPLEMENT_DYNCREATE(CGLExplorerView, CGLBaseView)

BEGIN_MESSAGE_MAP(CGLExplorerView, CGLBaseView)
END_MESSAGE_MAP()

CGLExplorerView::CGLExplorerView()
{
   m_nID = IDR_IMAGEVIEW;
   m_glTreeControl.m_bAutoDelete = FALSE;
}

CGLExplorerView::~CGLExplorerView()
{
}

void CGLExplorerView::PostInit()
{
   CGLControl::PostInit();

   CGLBaseDocFrame *pBaseDocFrame = FindDocFrame();
   if (pBaseDocFrame)
   {
      m_pDocument = pBaseDocFrame->GetCurrentDocument();
      m_pDocument->AddView( this );
   }

   m_glTreeControl.Init( this );

   if (m_pDocument)
      m_glTreeControl.SetObjectHead( m_pDocument->GetObjectHead());
}

void CGLExplorerView::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   CGLBaseView::Move( ptControl, szControl, justification );

   m_glTreeControl.Move( CPoint(0,0), CSize( m_szControl.cx, m_szControl.cy ));
}

void CGLExplorerView::Draw( UINT nDrawFlags/*=DRAW_CHILDREN|DRAW_PARENT*/ )
{
   if (!m_bDoPick)
      CGLControl::Draw( nDrawFlags );
}

BOOL CGLExplorerView::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYCTRL_SB_ENDSCROLL:
         return FALSE;
   }

   return CGLBaseView::OnControlNotify( msg, wParam, lParam );
}
