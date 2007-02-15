//

#include "stdafx.h"
#include "HideSelectionUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCGLBaseView.h"
#include "HMainInterface.h"
#include "HSelectionList.h"
#include "HCommandPluginManager.h"
#include "HCBaseDoc.h"

const char *HideSelectionCommandUIPlugin::GetCategory() const { return "Edit"; }
const char *HideSelectionCommandUIPlugin::GetTitleName() const { return "Hide Selection"; }
const char *HideSelectionCommandUIPlugin::GetDescription() const { return "Hide the selected objects."; }

UINT HideSelectionCommandUIPlugin::GetBitmapID() const { return IDB_HIDESELECTION; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY|FSHIFT, 'H')
};

void HideSelectionCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void HideSelectionCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pHideSelectionCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Hide");
   if (m_pHideSelectionCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : HideSelection ).");
}

void HideSelectionCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pEditToolBar = pMainFrame->AddToolBar(GetCategory());
   pEditToolBar->AddButton(NULL);
   pEditToolBar->AddButton(GetPlugin());
}

// Message Maps
void HideSelectionCommandUIPlugin::OnUpdateCommandUIDoc(const HCBaseDoc *pDoc, CCmdUI *pCmdUI)
{
   BOOL bEnable = FALSE;
   HSelectionList *pList = pDoc->GetSelectionList();
   if (pList && !pList->IsEmpty() && pList->GetHead()->IsHidable())
      bEnable = TRUE;

   pCmdUI->Enable( bEnable );
}

void HideSelectionCommandUIPlugin::OnCommandDoc( HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pOutPlugin = m_pHideSelectionCommandPlugin;
}
