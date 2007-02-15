//

#include "stdafx.h"
#include "SelectInverseCommand.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "HPoly.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "../Resource.h"

CommandInstance *SelectInverseCommandPlugin::CreateCommandInstance() { return SNEW SelectInverseCommand(this); }

const char *SelectInverseCommandPlugin::GetCLIName() const { return "SelectInverse"; }; // Name used in scripts and commandline
const char *SelectInverseCommandPlugin::GetTitleName() const { return "Select Inverse"; }

UINT SelectInverseCommandPlugin::GetBitmapID() const { return IDB_SELECTINVERSE; }

String SelectInverseCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Selects all objects or components that are not currently selected, and deselects those that are.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

SelectInverseCommand::SelectInverseCommand(HCommandPluginHelper *pPlugin) :
   SelectBaseCommand(pPlugin)
{
   m_nSOT = SOT_Unknown;
}

BOOL SelectInverseCommand::OnDo(StringList &strResults)
{
   if (!SelectBaseCommand::OnDo(strResults))
      return FALSE;

   HSnipeObject *pSnipeObject = GetHMainInterface()->GetCurrentCommandObject();
   if (m_nSOT == SOT_Unknown)
      m_nSOT = GetHMainInterface()->GetCurrentSelectionSOT();
   HSnipeObjectList *pObjectList = pSnipeObject->CreateListFromSOT( m_nSOT );

   for (POSITION pos=pObjectList->GetHeadPosition(); pos; )
   {
      POSITION oldpos = pos;
      HSnipeObject *pSubObject = pObjectList->GetNext( pos );
      if (!pSubObject->IsVisible() || pSubObject->IsSelected())
      {
         pObjectList->RemoveAt( oldpos );
         continue;
      }
   }
   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();
   pSelectionList->SetTo( pObjectList );
   pSelectionList->SetFocusObject(NULL, NULL);

   delete pObjectList;

   return TRUE;
}

