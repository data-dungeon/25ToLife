#pragma once

#include "SnipeObject.h"
#include "SelectionList.h"

inline void SelectionList::SetDirty()
{
   SetBoundsDirty();
   m_bSelectionDirty = TRUE;
}

inline void SelectionList::SetBoundsDirty()
{
   m_bBoundsDirty = TRUE;
}

inline BOOL SelectionList::IsSelectionChanged()
{
   BOOL bChanged = m_bSelectionDirty;
   m_bSelectionDirty = FALSE;
   return bChanged;
}

inline void SelectionList::Add(SnipeObject *pSnipeObject)
{
   ASSERT( pSnipeObject->m_bInList || !pSnipeObject->IsKindOf(SOT_Component) );

   if (pSnipeObject->IsSelected())
      return;

   m_objectlist.AddTail( pSnipeObject );
   pSnipeObject->SetSelected(TRUE);
   SetDirty();
   m_bMapDirty = TRUE;
}

inline void SelectionList::Remove(SnipeObject *pSnipeObject)
{
   ASSERT( pSnipeObject->IsSelected() );

   if (m_bMapDirty)
      RebuildMap();

   POSITION pos = 0;
   m_objectmap.Lookup( pSnipeObject, pos );
   ASSERT( pos );
   m_objectlist.RemoveAt( pos );
   pSnipeObject->SetSelected(FALSE);
   SetDirty();
}

inline void SelectionList::Toggle(SnipeObject *pSnipeObject)
{
   if (pSnipeObject->IsSelected())
      Remove( pSnipeObject );
   else
      Add( pSnipeObject );
}

inline POSITION SelectionList::GetHeadPosition()
{
   return m_objectlist.GetHeadPosition();
}

inline SnipeObject *SelectionList::GetHead()
{
   return m_objectlist.GetHead();
}

inline SnipeObject *SelectionList::GetNext(POSITION &pos)
{
   return m_objectlist.GetNext( pos );
}
