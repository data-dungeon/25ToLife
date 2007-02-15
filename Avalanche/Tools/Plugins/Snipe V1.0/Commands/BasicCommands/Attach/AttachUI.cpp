//

#include "stdafx.h"
#include "AttachUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
//#include "HCGLBaseView.h"
#include "HCommandPluginManager.h"
//#include "HSelectionList.h"

const char *AttachCommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *AttachCommandUIPlugin::GetTitleName() const { return "Attach"; }
const char *AttachCommandUIPlugin::GetDescription() const { return "Attaches two components of same type together."; }

UINT AttachCommandUIPlugin::GetBitmapID() const { return IDB_ATTACH; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY, '`')
};

void AttachCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void AttachCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pAttachCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Attach");
   if (m_pAttachCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Attach ).");
}

void AttachCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pEditToolBar = pMainFrame->AddToolBar(GetCategory());
   pEditToolBar->AddButton(GetPlugin());
}

// Object Message Maps
SnipeObjectType AttachCommandUIPlugin::GetCommandObjectType() { return SOT_Component; }

void AttachCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   BOOL bEnable = FALSE;
//   if (pObject->IsKindOf(SOT_Edge))
//   {
//      HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();
//   }

   pCmdUI->Enable( bEnable );
}

void AttachCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pOutPlugin = m_pAttachCommandPlugin;
}
