//

#include "stdafx.h"
#include "PaintSelectUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "SelectManipulator.h"
#include "HCGLBaseView.h"

const char *PaintSelectCommandUIPlugin::GetTitleName() const { return "Paint Select"; }
const char *PaintSelectCommandUIPlugin::GetDescription() const { return "Changed selection mode to paint."; }

UINT PaintSelectCommandUIPlugin::GetBitmapID() const { return IDB_PAINTSELECT; }

void PaintSelectCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pToolBar = pMainFrame->AddToolBar(GetCategory());
   pToolBar->AddButton(GetPlugin());
   pToolBar->AddButton(NULL);//Separator
}
// Message Maps
void PaintSelectCommandUIPlugin::OnUpdateCommandUIDoc(const HCBaseDoc *pDoc, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( SelectManipulatorPlugin::m_pMethod != NULL );

   if (SelectManipulatorPlugin::m_pMethod)
      pCmdUI->SetCheck( SelectManipulatorPlugin::m_pMethod->GetValue() == SelectManipulatorPlugin::SM_PAINT );
   else
      pCmdUI->SetCheck( FALSE );
}

void PaintSelectCommandUIPlugin::OnCommandDoc( HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   SelectManipulatorPlugin::m_pMethod->StoreValue( SelectManipulatorPlugin::SM_PAINT );
}

