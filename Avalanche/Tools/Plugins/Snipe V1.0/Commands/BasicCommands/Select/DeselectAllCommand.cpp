//

#include "stdafx.h"
#include "DeselectAllCommand.h"
#include "HSnipeObjectList.h"
#include "HSelectionList.h"
#include "../resource.h"
#include "HMainInterface.h"

CommandInstance *DeselectAllCommandPlugin::CreateCommandInstance() { return SNEW DeselectAllCommand(this); }


const char *DeselectAllCommandPlugin::GetCLIName() const { return "DeselectAll"; }; // Name used in scripts and commandline
const char *DeselectAllCommandPlugin::GetTitleName() const { return "Deselect All"; }

UINT DeselectAllCommandPlugin::GetBitmapID() const { return IDB_DESELECTALL; }

String DeselectAllCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Deselects all Objects and/or Components.\n";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
BOOL DeselectAllCommand::OnDo(StringList &strResults)
{
   SelectBaseCommand::OnDo(strResults);

   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();
   ASSERT(pSelectionList);

   pSelectionList->RemoveAll();
   pSelectionList->SetFocusObject(NULL, NULL);

   return TRUE;
}
