//

#include "stdafx.h"
#include "LassoSelectUI.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCGLBaseView.h"
#include "SelectManipulator.h"
#include "CommandUIAccel.h"

const char *LassoSelectCommandUIPlugin::GetTitleName() const { return "Lasso Select"; }
const char *LassoSelectCommandUIPlugin::GetDescription() const { return "Changed selection mode to lasso."; }

UINT LassoSelectCommandUIPlugin::GetBitmapID() const { return IDB_LASSOSELECT; }

static CommandUIAccel f_pAccel [] = 
{
//   CommandUIAccel(FVIRTKEY|FCONTROL|FSHIFT, VK_F2),
   CommandUIAccel(FVIRTKEY|FSHIFT, 'G') // AM keystroke
};

void LassoSelectCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void LassoSelectCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pManipulatorToolBar = pMainFrame->AddToolBar(GetCategory());
   pManipulatorToolBar->AddButton(GetPlugin());
}

// Message Maps
void LassoSelectCommandUIPlugin::OnUpdateCommandUIDoc(const HCBaseDoc *pDoc, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( SelectManipulatorPlugin::m_pMethod != NULL );

   if (SelectManipulatorPlugin::m_pMethod)
      pCmdUI->SetCheck( SelectManipulatorPlugin::m_pMethod->GetValue() == SelectManipulatorPlugin::SM_LASSO );
   else
      pCmdUI->SetCheck( FALSE );
}

void LassoSelectCommandUIPlugin::OnCommandDoc( HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   SelectManipulatorPlugin::m_pMethod->StoreValue( SelectManipulatorPlugin::SM_LASSO );
}
