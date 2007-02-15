//

#include "stdafx.h"
#include "ShowSelectionCommand.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "HPoly.h"
#include "../Resource.h"
#include "HMainInterface.h"

CommandInstance *ShowSelectionCommandPlugin::CreateCommandInstance() { return SNEW ShowSelectionCommand(this); }

const char *ShowSelectionCommandPlugin::GetCLIName() const { return "Show"; }; // Name used in scripts and commandline
const char *ShowSelectionCommandPlugin::GetTitleName() const { return "Show Selection"; }

UINT ShowSelectionCommandPlugin::GetBitmapID() const { return IDB_SHOWSELECTION; }

String ShowSelectionCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Shows (unhides) selected objects and components.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

ShowSelectionCommand::~ShowSelectionCommand()
{
}

UndoType ShowSelectionCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL ShowSelectionCommand::OnDo(StringList &strResults)
{
   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();

   for (POSITION pos = pSelectionList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pSnipeObject = pSelectionList->GetNext(pos);
      switch (pSnipeObject->GetSOT())
      {
         case SOT_Poly:
            HPoly *pPoly = (HPoly*)pSnipeObject;
            if (!pPoly->IsVisible())
            {
               m_objectlist.AddTail(pPoly);
               pPoly->SetVisible( TRUE );
            }
            break;
      }
   }

   return TRUE;
}

void ShowSelectionCommand::OnUndo()
{
   for (POSITION pos = m_objectlist.GetHeadPosition(); pos; )
   {
      HSnipeObject *pSnipeObject = m_objectlist.GetNext(pos);
      pSnipeObject->SetVisible( FALSE );
   }
}

