//

#include "stdafx.h"
#include "SelectBaseCommand.h"
#include "../resource.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "HEdge.h"
#include "HPoly.h"

/////////////////////////////////////////////////////////////////////////////////////////////////

SelectBaseCommand::~SelectBaseCommand()
{
   if (m_prevselectedobjects)
      delete m_prevselectedobjects;
}

UndoType SelectBaseCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL SelectBaseCommand::OnDo(StringList &strResults)
{
   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();
   ASSERT(pSelectionList);

   m_pPrevFocusObject = HSelectionList::GetFocusObject();
   m_pPrevNearestFocusObject = HSelectionList::GetNearestFocusObject();

   if (m_prevselectedobjects==NULL) // Could be from a redo, in that case m_prevselectedobjects is valid
      m_prevselectedobjects = pSelectionList->CreateSnipeObjectList();

   return TRUE;
}

void SelectBaseCommand::OnUndo()
{
   ASSERT(m_prevselectedobjects);

   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();
   pSelectionList->SetTo(m_prevselectedobjects);
   pSelectionList->SetFocusObject(m_pPrevFocusObject, m_pPrevNearestFocusObject );
}
