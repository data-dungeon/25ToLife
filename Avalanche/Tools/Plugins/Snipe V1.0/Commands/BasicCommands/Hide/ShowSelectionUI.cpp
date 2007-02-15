//

#include "stdafx.h"
#include "ShowSelectionUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCGLBaseView.h"
#include "HCommandPluginManager.h"
#include "HSelectionList.h"
#include "HMainInterface.h"
#include "HCBaseDoc.h"

const char *ShowSelectionCommandUIPlugin::GetCategory() const { return "Edit"; }
const char *ShowSelectionCommandUIPlugin::GetTitleName() const{ return "Show Selection"; }
const char *ShowSelectionCommandUIPlugin::GetDescription() const { return "Show the selected objects."; }

UINT ShowSelectionCommandUIPlugin::GetBitmapID() const { return IDB_SHOWSELECTION; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY|FCONTROL|FSHIFT, 'H')
};

void ShowSelectionCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void ShowSelectionCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pShowSelectionCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Show");
   if (m_pShowSelectionCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : ShowSelection ).");
}

void ShowSelectionCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pEditToolBar = pMainFrame->AddToolBar(GetCategory());
   pEditToolBar->AddButton(GetPlugin());
}

// Message Maps
void ShowSelectionCommandUIPlugin::OnUpdateCommandUIDoc(const HCBaseDoc *pDoc, CCmdUI *pCmdUI)
{
   BOOL bEnable = FALSE;
   HSelectionList *pList = pDoc->GetSelectionList();
   if (pList && !pList->IsEmpty() && pList->GetHead()->IsHidable())
      bEnable = TRUE;

   pCmdUI->Enable( bEnable );
}

void ShowSelectionCommandUIPlugin::OnCommandDoc( HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pOutPlugin = m_pShowSelectionCommandPlugin;
}
