//

#include "stdafx.h"
#include "ShowOnlyUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCGLBaseView.h"
#include "HMainInterface.h"
#include "HSelectionList.h"
#include "HCommandPluginManager.h"
#include "HCBaseDoc.h"

const char *ShowOnlyCommandUIPlugin::GetCategory() const { return "Edit"; }
const char *ShowOnlyCommandUIPlugin::GetTitleName() const { return "Show Selection Only"; }
const char *ShowOnlyCommandUIPlugin::GetDescription() const { return "Show only the selected objects, and hide everything else."; }

UINT ShowOnlyCommandUIPlugin::GetBitmapID() const { return IDB_SHOWONLY; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY, 'H')
};

void ShowOnlyCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void ShowOnlyCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pShowOnlyCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "ShowOnly");
   if (m_pShowOnlyCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : ShowOnly ).");
}

void ShowOnlyCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pEditToolBar = pMainFrame->AddToolBar(GetCategory());
   pEditToolBar->AddButton(GetPlugin());
}

// Message Maps
void ShowOnlyCommandUIPlugin::OnUpdateCommandUIDoc(const HCBaseDoc *pDoc, CCmdUI *pCmdUI)
{
   BOOL bEnable = FALSE;
   HSelectionList *pList = pDoc->GetSelectionList();
   if (pList && !pList->IsEmpty() && pList->GetHead()->IsHidable())
      bEnable = TRUE;

   pCmdUI->Enable( bEnable );
}

void ShowOnlyCommandUIPlugin::OnCommandDoc( HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   pOutPlugin = m_pShowOnlyCommandPlugin;
}
