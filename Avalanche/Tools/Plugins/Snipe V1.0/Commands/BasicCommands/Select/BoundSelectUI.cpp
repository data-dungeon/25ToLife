//

#include "stdafx.h"
#include "BoundSelectUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "SelectManipulator.h"
#include "HCGLBaseView.h"

const char *BoundSelectCommandUIPlugin::GetTitleName() const { return "Bound Select"; }
const char *BoundSelectCommandUIPlugin::GetDescription() const { return "Changed selection mode to bound."; }

UINT BoundSelectCommandUIPlugin::GetBitmapID() const { return IDB_BOUNDSELECT; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY, 'B')
};

void BoundSelectCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void BoundSelectCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pManipulatorToolBar = pMainFrame->AddToolBar(GetCategory());
   pManipulatorToolBar->AddButton(GetPlugin());
}

// Message Maps
void BoundSelectCommandUIPlugin::OnUpdateCommandUIDoc(const HCBaseDoc *pDoc, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( SelectManipulatorPlugin::m_pMethod != NULL );

   if (SelectManipulatorPlugin::m_pMethod)
      pCmdUI->SetCheck( SelectManipulatorPlugin::m_pMethod->GetValue() == SelectManipulatorPlugin::SM_BOUND );
   else
      pCmdUI->SetCheck( FALSE );
}

void BoundSelectCommandUIPlugin::OnCommandDoc( HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   SelectManipulatorPlugin::m_pMethod->StoreValue( SelectManipulatorPlugin::SM_BOUND );
}

