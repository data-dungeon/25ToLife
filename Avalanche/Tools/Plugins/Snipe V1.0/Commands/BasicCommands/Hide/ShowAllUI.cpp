//

#include "stdafx.h"
#include "ShowAllUI.h"
#include "CommandUIAccel.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCGLBaseView.h"
#include "HCommandPluginManager.h"
#include "../Resource.h"

const char *ShowAllCommandUIPlugin::GetCategory() const { return "Edit"; }
const char *ShowAllCommandUIPlugin::GetTitleName() const { return "Show All"; }
const char *ShowAllCommandUIPlugin::GetDescription() const { return "Show all objects."; }

UINT ShowAllCommandUIPlugin::GetBitmapID() const { return IDB_SHOWALL; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY|FCONTROL, 'H')
};

void ShowAllCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void ShowAllCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pShowAllCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "ShowAll");
   if (m_pShowAllCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : ShowAll ).");
}

void ShowAllCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pEditToolBar = pMainFrame->AddToolBar( GetCategory() );
   pEditToolBar->AddButton( GetPlugin() );

//   HCGLMenu *pEditMenu = pMainFrame->AddMenu( GetCategory() );
//   pEditMenu->AddMenu( GetPlugin() );
}

// View Message Maps
SnipeViewType ShowAllCommandUIPlugin::GetCommandViewType() { return SVT_CGLBaseView; }

void ShowAllCommandUIPlugin::OnUpdateCommandUIView(const HCGLBaseView *pView, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( TRUE );
}

void ShowAllCommandUIPlugin::OnCommandView( HCGLBaseView *pView, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pOutPlugin = m_pShowAllCommandPlugin;
}
