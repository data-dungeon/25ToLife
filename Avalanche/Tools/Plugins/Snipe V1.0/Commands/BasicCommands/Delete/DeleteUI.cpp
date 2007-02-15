//

#include "stdafx.h"
#include "DeleteUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCGLBaseView.h"
#include "HCommandPluginManager.h"

const char *DeleteCommandUIPlugin::GetCategory() const { return "Edit"; }
const char *DeleteCommandUIPlugin::GetTitleName() const { return "Delete"; }
const char *DeleteCommandUIPlugin::GetDescription() const { return "Delete the selected objects."; }

UINT DeleteCommandUIPlugin::GetBitmapID() const { return IDB_DELETE; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY, VK_DELETE)
};

void DeleteCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void DeleteCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pDeleteCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Delete");
   if (m_pDeleteCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Delete ).");
}

void DeleteCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pEditToolBar = pMainFrame->AddToolBar(GetCategory());
   pEditToolBar->AddButton(GetPlugin());
}

// Object Message Maps
SnipeObjectType DeleteCommandUIPlugin::GetCommandObjectType() { return SOT_SnipeObject; }

void DeleteCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( pObject->IsDeletable() );
}

void DeleteCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pOutPlugin = m_pDeleteCommandPlugin;
}
