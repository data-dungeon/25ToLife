//

#include "stdafx.h"
#include "SingleSelectUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "SelectManipulator.h"
#include "HCGLBaseView.h"

const char *SingleSelectCommandUIPlugin::GetTitleName() const { return "Single Select"; }
const char *SingleSelectCommandUIPlugin::GetDescription() const { return "Changed selection mode to single pick."; }

UINT SingleSelectCommandUIPlugin::GetBitmapID() const { return IDB_SINGLESELECT; }

void SingleSelectCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pManipulatorToolBar = pMainFrame->AddToolBar(GetCategory());
   pManipulatorToolBar->AddButton(GetPlugin());
}

void SingleSelectCommandUIPlugin::Init( HCommandPluginManager *pManager )
{
}

// MainFrame Message Maps
void SingleSelectCommandUIPlugin::OnUpdateCommandUIDoc(const HCBaseDoc *pDoc, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( SelectManipulatorPlugin::m_pMethod != NULL );

   if (SelectManipulatorPlugin::m_pMethod)
      pCmdUI->SetCheck( SelectManipulatorPlugin::m_pMethod->GetValue() == SelectManipulatorPlugin::SM_SINGLE );
   else
      pCmdUI->SetCheck( FALSE );
}

void SingleSelectCommandUIPlugin::OnCommandDoc( HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   SelectManipulatorPlugin::m_pMethod->StoreValue( SelectManipulatorPlugin::SM_SINGLE );
}

