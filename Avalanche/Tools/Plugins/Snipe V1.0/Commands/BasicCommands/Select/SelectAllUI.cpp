//

#include "stdafx.h"
#include "SelectAllUI.h"
#include "CommandUIAccel.h"
#include "HMainInterface.h"
#include "HSelectionList.h"
#include "HCommandPluginManager.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "../Resource.h"

const char *SelectAllCommandUIPlugin::GetCategory() const { return "Select"; }
const char *SelectAllCommandUIPlugin::GetTitleName() const { return "Select All"; }
const char *SelectAllCommandUIPlugin::GetDescription() const { return "Selects everything."; }

UINT SelectAllCommandUIPlugin::GetBitmapID() const { return IDB_SELECTALL; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY|FCONTROL, 'A')
};

void SelectAllCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void SelectAllCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSelectAllCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "SelectAll");
   if (m_pSelectAllCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Select All ).");
}

void SelectAllCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pToolBar = pMainFrame->AddToolBar(GetCategory());
   pToolBar->AddButton(GetPlugin());
}

// Message Maps
void SelectAllCommandUIPlugin::OnUpdateCommandUIDoc(const HCBaseDoc *pDoc, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( TRUE );
}

void SelectAllCommandUIPlugin::OnCommandDoc( HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pOutPlugin = m_pSelectAllCommandPlugin;
}


