//

#include "stdafx.h"
#include "ShowAllCommand.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "HPoly.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "HideSelectionCommand.h"
#include "HPolyModel.h"
#include "../Resource.h"

CommandInstance *ShowAllCommandPlugin::CreateCommandInstance() { return SNEW ShowAllCommand(this); }

const char *ShowAllCommandPlugin::GetCLIName() const { return "ShowAll"; }; // Name used in scripts and commandline
const char *ShowAllCommandPlugin::GetTitleName() const { return "Show All"; }
UINT ShowAllCommandPlugin::GetBitmapID() const { return IDB_SHOWALL; }

String ShowAllCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Shows (unhides) all objects and components.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

ShowAllCommand::ShowAllCommand(HCommandPluginHelper *pPlugin) :
   CommandInstance(pPlugin)
{
   m_nSOT = SOT_Unknown;
}

ShowAllCommand::~ShowAllCommand()
{
}

UndoType ShowAllCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL ShowAllCommand::OnDo(StringList &strResults)
{
   HSnipeObject *pSnipeObject = GetHMainInterface()->GetCurrentCommandObject();
   if (m_nSOT == SOT_Unknown)
      m_nSOT = GetHMainInterface()->GetCurrentSelectionSOT();

   HSnipeObjectList *pObjectList = pSnipeObject->CreateListFromSOT( m_nSOT );
   BOOL bHasComponent = FALSE;
   for (POSITION pos=pObjectList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pSubObject = pObjectList->GetNext( pos );
      if (!pSubObject->IsVisible())
      {
         m_objectlist.AddTail( pSubObject );
         pSubObject->SetVisible( TRUE );
      }
      if (!bHasComponent && pSubObject->IsKindOf(SOT_Component))
         bHasComponent = TRUE;
   }
   delete pObjectList;

   HPolyModel *pModel = (HPolyModel *)pSnipeObject->IsKindOf(SOT_PolyModel);
   if (pModel && bHasComponent)
   {
      for (POSITION pos=pModel->GetHeadFacePosition(); pos; )
      {
         HPoly *pPoly = pModel->GetNextFace( pos );
         if (!pPoly->IsVisible())
         {
            m_objectlist.AddTail(pPoly);
            pPoly->SetVisible(TRUE);
         }
      }
      SetComponentsVisibiltyToParentsVisibility(&m_objectlist);
   }

   return TRUE;
}

void ShowAllCommand::OnRedo()
{
   for (POSITION pos=m_objectlist.GetHeadPosition(); pos; )
   {
      HSnipeObject *pSnipeObject = m_objectlist.GetNext(pos);
      pSnipeObject->SetVisible( TRUE );
   }
}

void ShowAllCommand::OnUndo()
{
   for (POSITION pos=m_objectlist.GetHeadPosition(); pos; )
   {
      HSnipeObject *pSnipeObject = m_objectlist.GetNext(pos);
      pSnipeObject->SetVisible( FALSE );
   }
}
