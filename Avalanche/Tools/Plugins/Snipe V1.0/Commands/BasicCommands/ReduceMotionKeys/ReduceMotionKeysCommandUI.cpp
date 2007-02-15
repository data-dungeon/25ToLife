//

#include "stdafx.h"
#include "ReduceMotionKeysCommandUI.h"
#include "CommandUIAccel.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCGLBaseView.h"
#include "HCommandPluginManager.h"
#include "../Resource.h"
#include "HMainInterface.h"
#include "ReduceMotionKeysDlg.h"


const char *ReduceMotionKeysCommandUIPlugin::GetCategory() const { return "Edit"; }
const char *ReduceMotionKeysCommandUIPlugin::GetTitleName() const { return "Compress Motion"; }
const char *ReduceMotionKeysCommandUIPlugin::GetDescription() const { return "Compress Motion."; }

UINT ReduceMotionKeysCommandUIPlugin::GetBitmapID() const { return IDB_DEFAULT; }

void ReduceMotionKeysCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pReduceMotionKeysCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "ReduceMotionKeys");
   if (m_pReduceMotionKeysCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : ReduceMotionKeys ).");
}

void ReduceMotionKeysCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
//   HCGLToolBarControl *pEditToolBar = pMainFrame->AddToolBar( GetCategory() );
 //  pEditToolBar->AddButton( GetPlugin() );
}

// Object Message Maps
SnipeObjectType ReduceMotionKeysCommandUIPlugin::GetCommandObjectType() { return SOT_HierObject; }

void ReduceMotionKeysCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
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

void ReduceMotionKeysCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   HMainInterface *pMainInterface = GetHMainInterface();
   String objectdescription;

//   ReduceMotionKeysDlg dlg;
//   if (dlg.DoModal() != IDOK)
//      return;

//   strOutParameters += "-t ";
//   strOutParameters += String(dlg.m_fTolerance);

   if (pMainInterface->ObjectToDescription(pObject,objectdescription))
   {
      strOutParameters += "-m ";
      strOutParameters += objectdescription;
   }
   pOutPlugin = m_pReduceMotionKeysCommandPlugin;
}