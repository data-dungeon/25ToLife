//

#include "stdafx.h"
#include "ImportMotionCommandUI.h"
#include "CommandUIAccel.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCGLBaseView.h"
#include "HCommandPluginManager.h"
#include "HMainInterface.h"
#include "../../Resource.h"

const char *ImportMotionCommandUIPlugin::GetCategory() const { return "File"; }
const char *ImportMotionCommandUIPlugin::GetTitleName() const { return "Import Motion..."; }
const char *ImportMotionCommandUIPlugin::GetDescription() const { return "Imports a motion file into the project."; }

UINT ImportMotionCommandUIPlugin::GetBitmapID() const { return IDB_DEFAULT; }

void ImportMotionCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pImportMotionCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "ImportMotion");
   if (m_pImportMotionCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : ImportMotion ).");
}

void ImportMotionCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pEditToolBar = pMainFrame->AddToolBar( GetCategory() );
   pEditToolBar->AddButton( GetPlugin() );
}

// Object Message Maps
SnipeObjectType ImportMotionCommandUIPlugin::GetCommandObjectType() { return SOT_MotionContainer; }

void ImportMotionCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( TRUE );
}

void ImportMotionCommandUIPlugin::OnCommandObject( const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   pOutPlugin = m_pImportMotionCommandPlugin;
   strOutParameters = "-e";
}

void ImportMotionCommandUIPlugin::OnCommandMainFrame( HCMainFrame *pMainFrame, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   pOutPlugin = m_pImportMotionCommandPlugin;
   strOutParameters = "-e";
}
