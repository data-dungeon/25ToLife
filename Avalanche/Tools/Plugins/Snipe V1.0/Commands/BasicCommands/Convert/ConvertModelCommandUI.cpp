//

#include "stdafx.h"
#include "ConvertModelCommandUI.h"
#include "CommandUIAccel.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCGLBaseView.h"
#include "HCommandPluginManager.h"
#include "../Resource.h"
#include "HMainInterface.h"
#include "ConvertModelDlg.h"


const char *ConvertModelCommandUIPlugin::GetCategory() const { return "Edit"; }
const char *ConvertModelCommandUIPlugin::GetTitleName() const { return "Convert Model"; }
const char *ConvertModelCommandUIPlugin::GetDescription() const { return "Converts a model from one type to another."; }

UINT ConvertModelCommandUIPlugin::GetBitmapID() const { return IDB_CONVERT; }

void ConvertModelCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pConvertModelCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "ConvertModel");
   if (m_pConvertModelCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : ConvertModel ).");
}

void ConvertModelCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
//   HCGLToolBarControl *pEditToolBar = pMainFrame->AddToolBar( GetCategory() );
 //  pEditToolBar->AddButton( GetPlugin() );
}

// Object Message Maps
SnipeObjectType ConvertModelCommandUIPlugin::GetCommandObjectType() { return SOT_PolyModel; }

void ConvertModelCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( TRUE );
}

void ConvertModelCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   HMainInterface *pMainInterface = GetHMainInterface();

   ConvertModelDlg dlg;
   dlg.m_nFromModelType = pObject->GetSOT();
   if (dlg.DoModal() != IDOK)
      return;

   strOutParameters += "-totype ";
   switch(dlg.m_nToModelType)
   {
   case SOT_PolyModel :
      {
         strOutParameters += "PolyModel";
      }
      break;
   case SOT_PatchModel :
      {
         strOutParameters += "PatchModel";
      }
      break;
//   case SOT_BezierSplineModel :
//      {
//         strOutParameters += "BezierSplineModel";
//      }
//      break;
   }

   String objectdescription;
   if (pMainInterface->ObjectToDescription(pObject,objectdescription))
   {
      strOutParameters += " -from ";
      strOutParameters += objectdescription;
   }
   pOutPlugin = m_pConvertModelCommandPlugin;
}