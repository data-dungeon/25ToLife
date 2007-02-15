//

#include "stdafx.h"
#include "CommandUIAccel.h"
#include "HMainInterface.h"
#include "HSelectionList.h"
#include "HCommandPluginManager.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "SelectRingUI.h"
#include "../Resource.h"
#include "HMainInterface.h"
#include "HEdge.h"

const char *SelectRingCommandUIPlugin::GetCategory() const { return "Select"; }
const char *SelectRingCommandUIPlugin::GetTitleName() const { return "Select Ring"; }
const char *SelectRingCommandUIPlugin::GetDescription() const { return "Selected a ring of edges."; }

UINT SelectRingCommandUIPlugin::GetBitmapID() const { return IDB_SELECTRING; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY, 'G')
};

void SelectRingCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void SelectRingCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSelectRingCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "SelectRing");
   if (m_pSelectRingCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Select Ring ).");
}

void SelectRingCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pToolBar = pMainFrame->AddToolBar(GetCategory());
   pToolBar->AddButton(GetPlugin());
}

// Message Maps
void SelectRingCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( TRUE );
}

void SelectRingCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pOutPlugin = m_pSelectRingCommandPlugin;
}