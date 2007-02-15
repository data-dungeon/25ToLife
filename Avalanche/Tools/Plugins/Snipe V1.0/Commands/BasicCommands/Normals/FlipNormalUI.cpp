//

#include "stdafx.h"
#include "FlipNormalUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCommandPluginManager.h"

const char *FlipNormalCommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *FlipNormalCommandUIPlugin::GetTitleName() const { return "Flip Normal"; }
const char *FlipNormalCommandUIPlugin::GetDescription() const { return "Flip normals of selected faces."; }

UINT FlipNormalCommandUIPlugin::GetBitmapID() const { return IDB_FLIPNORMAL; }

void FlipNormalCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pFlipNormalCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "FlipNormal");
   if (m_pFlipNormalCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : FlipNormal ).");
}

void FlipNormalCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pModelToolBar = pMainFrame->AddToolBar(GetCategory());
   pModelToolBar->AddButton(NULL);//Separator
   pModelToolBar->AddButton(GetPlugin());
}

// Message Maps
void FlipNormalCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( TRUE );
}

void FlipNormalCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pOutPlugin = m_pFlipNormalCommandPlugin;
}