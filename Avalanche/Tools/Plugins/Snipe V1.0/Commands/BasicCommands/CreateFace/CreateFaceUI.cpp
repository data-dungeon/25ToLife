//

#include "stdafx.h"
#include "CreateFaceUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCommandPluginManager.h"
#include "HMainInterface.h"

const char *CreateFaceCommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *CreateFaceCommandUIPlugin::GetTitleName() const { return "Create Face"; }
const char *CreateFaceCommandUIPlugin::GetDescription() const { return "Create Face."; }

UINT CreateFaceCommandUIPlugin::GetBitmapID() const { return IDB_CREATEFACE; }

//static CommandUIAccel f_pAccel [] = 
//{
//   CommandUIAccel(FVIRTKEY, 'P') // Already used for Peak
//};
//
//void CreateFaceCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
//{
//   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
//   pAccel = f_pAccel;
//}

void CreateFaceCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pCreateFaceCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "CreateFace");
   if (m_pCreateFaceCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : CreateFace ).");
}

void CreateFaceCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pModelToolBar = pMainFrame->AddToolBar(GetCategory());
   pModelToolBar->AddButton(NULL);//Separator
   pModelToolBar->AddButton(GetPlugin());
}

// Message Maps
void CreateFaceCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable(pObject->IsKindOf(SOT_Edge) || pObject->IsKindOf(SOT_Vertex));
}

void CreateFaceCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   GetHMainInterface()->ObjectToDescription( pObject, strOutParameters );
   pOutPlugin = m_pCreateFaceCommandPlugin;
}
