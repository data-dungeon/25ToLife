//

#include "stdafx.h"
#include "SelectInverseUI.h"
#include "CommandUIAccel.h"
#include "HMainInterface.h"
#include "HSelectionList.h"
#include "HCommandPluginManager.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "../Resource.h"

const char *SelectInverseCommandUIPlugin::GetCategory() const { return "Select"; }
const char *SelectInverseCommandUIPlugin::GetTitleName() const { return "Select Inverse"; }
const char *SelectInverseCommandUIPlugin::GetDescription() const { return "Select the opposite of what is currently selected."; }

UINT SelectInverseCommandUIPlugin::GetBitmapID() const { return IDB_SELECTINVERSE; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY|FCONTROL, 'I'),
   CommandUIAccel(FNOINVERT, '.')
};

void SelectInverseCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void SelectInverseCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSelectInverseCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "SelectInverse");
   if (m_pSelectInverseCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Select Inverse ).");
}

void SelectInverseCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pToolBar = pMainFrame->AddToolBar(GetCategory());
   pToolBar->AddButton(GetPlugin());
}

// Message Maps
void SelectInverseCommandUIPlugin::OnUpdateCommandUIDoc(const HCBaseDoc *pDoc, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( TRUE );
}

void SelectInverseCommandUIPlugin::OnCommandDoc( HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pOutPlugin = m_pSelectInverseCommandPlugin;
}


