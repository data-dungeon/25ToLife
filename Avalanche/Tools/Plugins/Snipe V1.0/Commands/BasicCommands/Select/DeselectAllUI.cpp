//

#include "stdafx.h"
#include "DeselectAllUI.h"
#include "CommandUIAccel.h"
#include "HMainInterface.h"
#include "HSelectionList.h"
#include "HCommandPluginManager.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "../Resource.h"
#include "HCBaseDoc.h"

const char *DeselectAllCommandUIPlugin::GetCategory() const { return "Select"; }
const char *DeselectAllCommandUIPlugin::GetTitleName() const { return "Deselect All"; }
const char *DeselectAllCommandUIPlugin::GetDescription() const { return "Deselects everything currently selected."; }

UINT DeselectAllCommandUIPlugin::GetBitmapID() const { return IDB_DESELECTALL; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY|FCONTROL, 'D')
};

void DeselectAllCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void DeselectAllCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pDeselectAllCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "DeselectAll");
   if (m_pDeselectAllCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Deselect All ).");
}

void DeselectAllCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pToolBar = pMainFrame->AddToolBar(GetCategory());
   pToolBar->AddButton(GetPlugin());
}

// Message Maps
void DeselectAllCommandUIPlugin::OnUpdateCommandUIDoc(const HCBaseDoc *pDoc, CCmdUI *pCmdUI)
{
   HSelectionList *pList = pDoc->GetSelectionList();

   pCmdUI->Enable( pList && !pList->IsEmpty() );
}

void DeselectAllCommandUIPlugin::OnCommandDoc( HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pOutPlugin = m_pDeselectAllCommandPlugin;
}


