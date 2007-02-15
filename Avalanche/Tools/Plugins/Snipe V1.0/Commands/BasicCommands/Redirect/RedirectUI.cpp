//

#include "stdafx.h"
#include "RedirectUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCGLBaseView.h"
#include "HCommandPluginManager.h"
#include "HSelectionList.h"
#include "HPatchVertex.h"
#include "HPatchEdge.h"
#include "HMainInterface.h"

const char *RedirectCommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *RedirectCommandUIPlugin::GetTitleName() const { return "Redirect"; }
const char *RedirectCommandUIPlugin::GetDescription() const { return "Redirect Continuity."; }

UINT RedirectCommandUIPlugin::GetBitmapID() const { return IDB_REDIRECT; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY|FCONTROL, 'R')
};

void RedirectCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void RedirectCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pRedirectCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Redirect");
   if (m_pRedirectCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Redirect ).");
}

void RedirectCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pModelToolBar = pMainFrame->AddToolBar(GetCategory());
   pModelToolBar->AddButton(NULL);//Separator
   pModelToolBar->AddButton(GetPlugin());
}

// Object Message Maps
SnipeObjectType RedirectCommandUIPlugin::GetCommandObjectType() { return SOT_PatchEdge; }

void RedirectCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   HPatchEdge *pEdge = (HPatchEdge *)pObject;
   BOOL bEnable = FALSE;

   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();

   if (pSelectionList->GetCount()==2)
      bEnable = TRUE;

   pCmdUI->Enable( bEnable );
}

void RedirectCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();
   HPatchEdge *pEdge[2];
   pEdge[0] = NULL;
   pEdge[1] = NULL;

   ASSERT( pSelectionList->GetCount()==2 );

   int i=0;
   for (POSITION pos = pSelectionList->GetHeadPosition(); pos; i++)
   {
      HSnipeObject *pObject = pSelectionList->GetNext(pos);
      if (pObject->IsKindOf(SOT_PatchEdge))
         pEdge[i] = (HPatchEdge *)pObject;
   }

   if (pEdge[0]==NULL || pEdge[1]==NULL)
      THROW_FATAL_ERROR("Two Edges not provided");

   String strFromEdge;
   String strToEdge;
   
   GetHMainInterface()->ObjectToDescription( pEdge[0], strFromEdge );
   GetHMainInterface()->ObjectToDescription( pEdge[1], strToEdge );

   strOutParameters = (String)"-From " + strFromEdge + " -To " + strToEdge;
   pOutPlugin = m_pRedirectCommandPlugin;
}
