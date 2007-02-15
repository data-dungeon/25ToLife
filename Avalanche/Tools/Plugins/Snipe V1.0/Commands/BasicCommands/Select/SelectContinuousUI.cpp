//

#include "stdafx.h"
#include "CommandUIAccel.h"
#include "HMainInterface.h"
#include "HSelectionList.h"
#include "HCommandPluginManager.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "SelectContinuousUI.h"
#include "../Resource.h"
#include "HMainInterface.h"
#include "HEdge.h"

const char *SelectContinuousCommandUIPlugin::GetCategory() const { return "Select"; }
const char *SelectContinuousCommandUIPlugin::GetTitleName() const { return "Select Continuous"; }
const char *SelectContinuousCommandUIPlugin::GetDescription() const { return "Selected continuous components of the same type as the current selected component."; }

UINT SelectContinuousCommandUIPlugin::GetBitmapID() const { return IDB_SELECTCONTINUOUS; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FNOINVERT, ',')
};

void SelectContinuousCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void SelectContinuousCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSelectContinuousCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "SelectContinuous");
   if (m_pSelectContinuousCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Select Continuous ).");
}

void SelectContinuousCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pToolBar = pMainFrame->AddToolBar(GetCategory());
   pToolBar->AddButton(GetPlugin());
   pToolBar->AddButton(NULL);//SEPARATOR
}

// Message Maps
void SelectContinuousCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   if (pObject->IsKindOf(SOT_Vertex))
   {
      HSnipeObject *pNearestFocusObject = HSelectionList::GetNearestFocusObject();
      pCmdUI->Enable( pNearestFocusObject && pNearestFocusObject->IsKindOf(SOT_Edge) );
      return;
   }
   else if (pObject->IsKindOf(SOT_Edge))
   {
      pCmdUI->Enable( TRUE );
      return;
   }
   else 
   {
      if (pCmdUI->m_pMenu)
         pCmdUI->m_pMenu->DeleteMenu( pCmdUI->m_nIndex, MF_BYPOSITION );
   }
   pCmdUI->Enable( FALSE );
}

void SelectContinuousCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pOutPlugin = m_pSelectContinuousCommandPlugin;
}