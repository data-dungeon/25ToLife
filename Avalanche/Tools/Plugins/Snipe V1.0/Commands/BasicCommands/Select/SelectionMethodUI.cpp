//

#include "stdafx.h"
#include "SelectionMethodUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "SelectManipulator.h"
#include "HCGLBaseView.h"

const char *SelectionMethodCommandUIPlugin::GetTitleName() const { return "Selection Method"; }
const char *SelectionMethodCommandUIPlugin::GetDescription() const { return "Cycles through the selection methods."; }

UINT SelectionMethodCommandUIPlugin::GetBitmapID() const 
{
   if (SelectManipulatorPlugin::m_pMethod == NULL)
      return IDB_SINGLESELECT;

   switch (SelectManipulatorPlugin::m_pMethod->GetValue()) 
   {
      case SelectManipulatorPlugin::SM_SINGLE:
         return IDB_SINGLESELECT;
      case SelectManipulatorPlugin::SM_BOUND:
         return IDB_BOUNDSELECT;
      case SelectManipulatorPlugin::SM_LASSO:
         return IDB_LASSOSELECT;
      case SelectManipulatorPlugin::SM_POLYLASSO:
         return IDB_POLYLASSOSELECT;
      case SelectManipulatorPlugin::SM_PAINT:
         return IDB_PAINTSELECT;
      default: {
         ASSERT(FALSE);
         return IDB_SINGLESELECT;
      }
   }
}

void SelectionMethodCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pManipulatorToolBar = pMainFrame->AddToolBar(GetCategory());
   pManipulatorToolBar->AddButton(GetPlugin(), 0);
   pManipulatorToolBar->AddButton(NULL, 1);
}

// Message Maps
void SelectionMethodCommandUIPlugin::OnUpdateCommandUIDoc(const HCBaseDoc *pDoc, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( SelectManipulatorPlugin::m_pMethod != NULL );
}

void SelectionMethodCommandUIPlugin::OnCommandDoc( HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   UINT nMethod = SelectManipulatorPlugin::m_pMethod->GetValue();
   nMethod++;
   if (nMethod == SelectManipulatorPlugin::SM_NUMMETHODS)
      nMethod = 0;

   SelectManipulatorPlugin::m_pMethod->StoreValue( nMethod );
}

