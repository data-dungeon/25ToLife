//

#include "stdafx.h"
#include "TerminateUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCGLBaseView.h"
#include "HCommandPluginManager.h"
#include "HSelectionList.h"
#include "HPatchEdge.h"
#include "HPatchVertex.h"
#include "HMainInterface.h"

const char *TerminateCommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *TerminateCommandUIPlugin::GetTitleName() const { return "Terminate"; }
const char *TerminateCommandUIPlugin::GetDescription() const { return "Terminate Spline continuity."; }

UINT TerminateCommandUIPlugin::GetBitmapID() const { return IDB_TERMINATE; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY|FCONTROL, 'T')
};

void TerminateCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void TerminateCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pRedirectCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Redirect");
   if (m_pRedirectCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Redirect ).");
}

void TerminateCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pModelToolBar = pMainFrame->AddToolBar(GetCategory());
   pModelToolBar->AddButton(GetPlugin());
}

// Object Message Maps
SnipeObjectType TerminateCommandUIPlugin::GetCommandObjectType() { return SOT_PatchVertex; }

void TerminateCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   HPatchVertex *pVertex = (HPatchVertex *)pObject;
   HSnipeObject *pNearest = HSelectionList::GetNearestFocusObject();
   HPatchEdge *pEdge = NULL;
   
   if (pNearest && pNearest->IsKindOf(SOT_PatchEdge))
      pEdge = (HPatchEdge *)pNearest;

   BOOL bEnable = FALSE;
   if (pEdge)
      bEnable = pEdge->GetContinuousEdge(pVertex)!=NULL;

   pCmdUI->Enable( bEnable );
}

void TerminateCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   HPatchVertex *pFromKnot = (HPatchVertex *)pObject;
   HPatchEdge *pFromEdge = (HPatchEdge *)HSelectionList::GetNearestFocusObject();

   String strFromEdge;
   String strFromVertex;
   
   GetHMainInterface()->ObjectToDescription( pFromEdge, strFromEdge );
   GetHMainInterface()->ObjectToDescription( pFromKnot, strFromVertex );

   strOutParameters = (String)"-From " + strFromEdge + " -Term " + strFromVertex;
   pOutPlugin = m_pRedirectCommandPlugin;
}
