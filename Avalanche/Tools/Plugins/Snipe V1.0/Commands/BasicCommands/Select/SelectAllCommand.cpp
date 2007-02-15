//

#include "stdafx.h"
#include "SelectAllCommand.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "HPoly.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "../Resource.h"

CommandInstance *SelectAllCommandPlugin::CreateCommandInstance() { return SNEW SelectAllCommand(this); }

const char *SelectAllCommandPlugin::GetCLIName() const { return "SelectAll"; }; // Name used in scripts and commandline
const char *SelectAllCommandPlugin::GetTitleName() const { return "Select All"; }

UINT SelectAllCommandPlugin::GetBitmapID() const { return IDB_SELECTALL; }

String SelectAllCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Selects all objects or components.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

SelectAllCommand::SelectAllCommand(HCommandPluginHelper *pPlugin) :
   SelectBaseCommand(pPlugin)
{
   m_nSOT = SOT_Unknown;
}

BOOL SelectAllCommand::OnDo(StringList &strResults)
{
   if (!SelectBaseCommand::OnDo(strResults))
      return FALSE;

   HSnipeObject *pSnipeObject = GetHMainInterface()->GetCurrentCommandObject();
   if (m_nSOT == SOT_Unknown)
      m_nSOT = GetHMainInterface()->GetCurrentSelectionSOT();
   HSnipeObjectList *pObjectList = pSnipeObject->CreateListFromSOT( m_nSOT );
   if (!pObjectList)
      return FALSE;

   for (POSITION pos = pObjectList->GetHeadPosition(); pos; )
   {
      POSITION oldpos = pos;
      HSnipeObject *pObject = pObjectList->GetNext(pos);
      if (!pObject->IsVisible())
         pObjectList->RemoveAt( oldpos );
   }

   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();
   pSelectionList->SetTo( pObjectList );
   pSelectionList->SetFocusObject(NULL, NULL);

   delete pObjectList;

   return TRUE;
}
