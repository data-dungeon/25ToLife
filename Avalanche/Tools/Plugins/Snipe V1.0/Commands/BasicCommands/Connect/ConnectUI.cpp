//

#include "stdafx.h"
#include "ConnectUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCommandPluginManager.h"
#include "HMainInterface.h"

const char *ConnectCommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *ConnectCommandUIPlugin::GetTitleName() const { return "Connect"; }
const char *ConnectCommandUIPlugin::GetDescription() const { return "Connect selected vertices on a common face together with an edge, or create an edge between the mid-points of selected edges on a common face."; }

UINT ConnectCommandUIPlugin::GetBitmapID() const { return IDB_CONNECT; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY, 'C')
};

void ConnectCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void ConnectCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pConnectCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Connect");
   if (m_pConnectCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Connect ).");
}

void ConnectCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pModelToolBar = pMainFrame->AddToolBar(GetCategory());
   pModelToolBar->AddButton(GetPlugin());
}

// Message Maps
void ConnectCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable(pObject->IsKindOf(SOT_Edge) || pObject->IsKindOf(SOT_Vertex));
}

void ConnectCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pOutPlugin = m_pConnectCommandPlugin;
}
