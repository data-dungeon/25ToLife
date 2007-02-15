//

#include "stdafx.h"
#include "DeleteFaceUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCommandPluginManager.h"

const char *DeleteFaceCommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *DeleteFaceCommandUIPlugin::GetTitleName() const { return "Delete Face"; }
const char *DeleteFaceCommandUIPlugin::GetDescription() const { return "Delete Face."; }

UINT DeleteFaceCommandUIPlugin::GetBitmapID() const { return IDB_DELETEFACE; }

void DeleteFaceCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pDeleteFaceCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "DeleteFace");
   if (m_pDeleteFaceCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : DeleteFace ).");
}

void DeleteFaceCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pModelToolBar = pMainFrame->AddToolBar(GetCategory());
   pModelToolBar->AddButton(GetPlugin());
}

// Message Maps
void DeleteFaceCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( TRUE );
}

void DeleteFaceCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pOutPlugin = m_pDeleteFaceCommandPlugin;
}