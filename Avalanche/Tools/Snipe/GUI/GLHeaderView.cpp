//
#include "stdafx.h"
#include "Snipe.h"
#include "GLHeaderFrame.h"
#include "GLHeaderView.h"
#include "resource.h"

IMPLEMENT_DYNAMIC(CGLHeaderView, CGLBaseView)

BEGIN_MESSAGE_MAP(CGLHeaderView, CGLBaseView)
END_MESSAGE_MAP()

CGLHeaderView::CGLHeaderView()
{
}

CGLHeaderView::~CGLHeaderView()
{
}

void CGLHeaderView::UpdateHeader()
{
   if (GetHeaderFrame())
      GetHeaderFrame()->UpdateHeader();
}

BOOL CGLHeaderView::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYCTRL_BN_CLICKED:
         return FALSE;
      case NOTIFYCTRL_CB_CHANGED:
         return FALSE;
   }
   
   return CGLBaseView::OnControlNotify( msg, wParam, lParam );
}
