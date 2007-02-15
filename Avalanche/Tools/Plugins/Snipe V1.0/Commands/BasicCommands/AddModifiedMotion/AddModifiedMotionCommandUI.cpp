//

#include "stdafx.h"
#include "AddModifiedMotionCommandUI.h"
#include "CommandUIAccel.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCGLBaseView.h"
#include "HCommandPluginManager.h"
#include "../Resource.h"
#include "HMainInterface.h"
//#include "AddModifiedMotionDlg.h"


const char *AddModifiedMotionCommandUIPlugin::GetCategory() const { return "Edit"; }
const char *AddModifiedMotionCommandUIPlugin::GetTitleName() const { return "Add Modified Motion Curves"; }
const char *AddModifiedMotionCommandUIPlugin::GetDescription() const { return "Add motion curve modifiers."; }

UINT AddModifiedMotionCommandUIPlugin::GetBitmapID() const { return IDB_DEFAULT; }

void AddModifiedMotionCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pAddModifiedMotionCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "AddModifiedMotion");
   if (m_pAddModifiedMotionCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : AddModifiedMotion ).");
}

void AddModifiedMotionCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
//   HCGLToolBarControl *pEditToolBar = pMainFrame->AddToolBar( GetCategory() );
 //  pEditToolBar->AddButton( GetPlugin() );
}

// Object Message Maps
SnipeObjectType AddModifiedMotionCommandUIPlugin::GetCommandObjectType() { return SOT_HierObject; }

void AddModifiedMotionCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   BOOL bEnable = FALSE;
   SnipeObjectType nSOT = pObject->GetSOT();

   if (pObject->IsKindOf(SOT_MotionCurveDriver)||pObject->IsKindOf(SOT_Motion))
      bEnable = TRUE;

   if (!bEnable && pCmdUI->m_pMenu)
      pCmdUI->m_pMenu->DeleteMenu( pCmdUI->m_nIndex, MF_BYPOSITION );
   else
      pCmdUI->Enable( bEnable );
}

void AddModifiedMotionCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   HMainInterface *pMainInterface = GetHMainInterface();
   String objectdescription;

//   AddModifiedMotionDlg dlg;
//   if (dlg.DoModal() != IDOK)
//      return;

//   strOutParameters += "-t ";
//   strOutParameters += String(dlg.m_fTolerance);

   if (pMainInterface->ObjectToDescription(pObject,objectdescription))
   {
      strOutParameters += "-m ";
      strOutParameters += objectdescription;
   }
   pOutPlugin = m_pAddModifiedMotionCommandPlugin;
}