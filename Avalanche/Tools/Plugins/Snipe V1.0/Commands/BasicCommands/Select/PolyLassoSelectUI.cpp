//
#include "stdafx.h"
#include "HCMainFrame.h"
#include "PolyLassoSelectUI.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HManipulatorPlugin.h"
#include "HCGLBaseView.h"
#include "SelectManipulator.h"

const char *PolyLassoSelectCommandUIPlugin::GetTitleName() const { return "Poly Lasso Select"; }
const char *PolyLassoSelectCommandUIPlugin::GetDescription() const { return "Changed selection mode to poly lasso."; }

UINT PolyLassoSelectCommandUIPlugin::GetBitmapID() const { return IDB_POLYLASSOSELECT; }

void PolyLassoSelectCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pManipulatorToolBar = pMainFrame->AddToolBar(GetCategory());
   pManipulatorToolBar->AddButton(GetPlugin());
}

// Message Maps
void PolyLassoSelectCommandUIPlugin::OnUpdateCommandUIDoc(const HCBaseDoc *pDoc, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( SelectManipulatorPlugin::m_pMethod != NULL );

   if (SelectManipulatorPlugin::m_pMethod)
      pCmdUI->SetCheck( SelectManipulatorPlugin::m_pMethod->GetValue() == SelectManipulatorPlugin::SM_POLYLASSO );
   else
      pCmdUI->SetCheck( FALSE );
}

void PolyLassoSelectCommandUIPlugin::OnCommandDoc( HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   SelectManipulatorPlugin::m_pMethod->StoreValue( SelectManipulatorPlugin::SM_POLYLASSO );
}
