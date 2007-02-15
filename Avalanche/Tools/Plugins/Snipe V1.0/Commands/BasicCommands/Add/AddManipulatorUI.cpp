//

#include "stdafx.h"
#include "AddManipulatorUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCommandPluginManager.h"

const char *AddManipulatorUIPlugin::GetCategory() const { return "Modeling"; }
const char *AddManipulatorUIPlugin::GetTitleName() const { return "Add"; }
const char *AddManipulatorUIPlugin::GetDescription() const
{
   HMainInterface *pMI = GetHMainInterface();
   if (pMI->GetCurrentSelectionSOT() == SOT_Bone)
   {
      return "Add bones.";
   }
   return "Add vertices.";
}

UINT AddManipulatorUIPlugin::GetBitmapID() const { return IDB_ADDVERTEX; }

void AddManipulatorUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
}

/*
static CommandUIAccel f_pAddManipulatorUpAccel[] = 
{
   CommandUIAccel(FVIRTKEY, VK_PRIOR)
};

void AddManipulatorUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAddManipulatorUpAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAddManipulatorUpAccel;
}
*/

void AddManipulatorUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pModelToolBar = pMainFrame->AddToolBar(GetCategory());
   pModelToolBar->AddButton(NULL);//Separator
   pModelToolBar->AddButton(GetPlugin());
}


void AddManipulatorUIPlugin::OnCommandView( HCGLBaseView *pView, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{

   
}

/*
// Message Maps
void AddManipulatorUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( TRUE );
}

void AddManipulatorUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pOutPlugin = m_pAddManipulatorPlugin;
}
*/