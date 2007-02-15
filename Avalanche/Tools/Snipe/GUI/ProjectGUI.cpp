
#include "stdafx.h"
#include "Project.h"
#include "MainFrm.h"

BOOL Project::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg) {
      case NOTIFYOBJ_ADDCHILD:
//         GetMainFrame()->m_wndProjectExplorerWnd.m_glTree.SetObjectHead( this );
         break;
      case NOTIFYOBJ_REMOVECHILD:
//         GetMainFrame()->m_wndProjectExplorerWnd.m_glTree.SetObjectHead( this );
         break;
   }
   return HierObject::OnObjectNotify( msg, wParam, lParam );
}
