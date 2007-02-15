//

#include "stdafx.h"
#include "SaveSelectionUI.h"
#include "HCommandPluginManager.h"
#include "HMainInterface.h"
#include "HSelection.h"
#include "SaveSelectionDlg.h"
#include "SaveSelectionCommand.h"

const char *SaveSelectionCommandUIPlugin::GetCategory() const { return "Select"; }
const char *SaveSelectionCommandUIPlugin::GetTitleName() const { return "Save Selection"; }
const char *SaveSelectionCommandUIPlugin::GetDescription() const { return "Saves the current selection for recall at a later time."; }

UINT SaveSelectionCommandUIPlugin::GetBitmapID() const { return IDB_SELECT; }

void SaveSelectionCommandUIPlugin::Init( HCommandPluginManager *pCommandManager )
{
   // Collect dependencies here
   m_pSaveSelectionCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "SaveSelection");
   if (m_pSaveSelectionCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : SaveSelection ).");
}

// Object Message Maps
SnipeObjectType SaveSelectionCommandUIPlugin::GetCommandObjectType() { return SOT_SnipeObject; }

void SaveSelectionCommandUIPlugin::OnUpdateCommandUIObject( const HSnipeObject *pObject, CCmdUI *pCmdUI )
{
   pCmdUI->Enable( TRUE );
}

void SaveSelectionCommandUIPlugin::OnCommandObject( const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   pOutPlugin = m_pSaveSelectionCommandPlugin;

   CSaveSelectionDlg dlg;
   if (dlg.DoModal() != IDOK)
      return;

   if (dlg.m_nSelection != 0)
   {
      switch (dlg.m_nOperation + 1)
      {
         case SaveSelectionCommand::OP_REPLACE:
            strOutParameters = "-o REPLACE ";
            break;
         case SaveSelectionCommand::OP_ADD:
            strOutParameters = "-o ADD ";
            break;
         case SaveSelectionCommand::OP_SUBTRACT:
            strOutParameters = "-o SUBTRACT ";
            break;
         case SaveSelectionCommand::OP_TOGGLE:
            strOutParameters = "-o TOGGLE ";
            break;
      }
   }

   if (!dlg.m_strName.IsEmpty())
   {
      strOutParameters += (String)"-name ";
      if (dlg.m_strName.Find(' ') == -1)
         strOutParameters += (LPCTSTR)dlg.m_strName;
      else
         strOutParameters += (String)"\"" + (LPCTSTR)dlg.m_strName + "\"";
   }
}
