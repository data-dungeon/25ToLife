//

#include "stdafx.h"
#include "SelectConnectedUI.h"
#include "CommandUIAccel.h"
#include "HMainInterface.h"
#include "HSelectionList.h"
#include "HCommandPluginManager.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "../Resource.h"
#include "HCBaseDoc.h"

const char *SelectConnectedCommandUIPlugin::GetCategory() const { return "Select"; }
const char *SelectConnectedCommandUIPlugin::GetTitleName() const { return "Select Connected"; }
const char *SelectConnectedCommandUIPlugin::GetDescription() const { return "Selected connected components of the same type as the current selected component."; }

UINT SelectConnectedCommandUIPlugin::GetBitmapID() const { return IDB_SELECTCONNECTED; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FNOINVERT, '/')
};

void SelectConnectedCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void SelectConnectedCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSelectConnectedCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "SelectConnected");
   if (m_pSelectConnectedCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Select Connected ).");
}

void SelectConnectedCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pToolBar = pMainFrame->AddToolBar(GetCategory());
   pToolBar->AddButton(GetPlugin());
}

// Message Maps
void SelectConnectedCommandUIPlugin::OnUpdateCommandUIDoc(const HCBaseDoc *pDoc, CCmdUI *pCmdUI)
{
   HSelectionList *pList = pDoc->GetSelectionList();
   HSnipeObject *pObject = NULL;
   
   if (pList && !pList->IsEmpty())
      pObject = pList->GetHead();

   BOOL bEnable = pObject && (pObject->IsKindOf(SOT_Vertex) || pObject->IsKindOf(SOT_Edge) || pObject->IsKindOf(SOT_Poly));

   if (!bEnable && pCmdUI->m_pMenu)
      pCmdUI->m_pMenu->DeleteMenu( pCmdUI->m_nIndex, MF_BYPOSITION );

   pCmdUI->Enable( bEnable );
}

void SelectConnectedCommandUIPlugin::OnCommandDoc( HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pOutPlugin = m_pSelectConnectedCommandPlugin;
}


