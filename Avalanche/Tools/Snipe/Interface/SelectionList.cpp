#include "stdafx.h"
#include "SelectionList.h"
#include "SnipeObject.h"
#include "Poly.h"

SnipeObject *SelectionList::m_pMouseOverObject;
SnipeObject *SelectionList::m_pFocusObject;
SnipeObject *SelectionList::m_pNearestFocusObject;

SelectionList::SelectionList()
 : m_objectmap( 128 )
{
   m_bBoundsDirty = TRUE;
   m_bSelectionDirty = FALSE;
   m_bMapDirty = TRUE;
   m_objectmap.InitHashTable( 1019 );
}

// Static
void SelectionList::SetMouseOverObject( SnipeObject *pMouseOverObject )
{
   if (m_pMouseOverObject)
      m_pMouseOverObject->OnMouseOverChanged( FALSE );

   m_pMouseOverObject = pMouseOverObject;

   if (m_pMouseOverObject)
      m_pMouseOverObject->OnMouseOverChanged( TRUE );
}

// Static
BOOL SelectionList::SetFocusObject( SnipeObject *pFocusObject, SnipeObject *pNearestFocusObject )
{
   if (m_pFocusObject)
      m_pFocusObject->OnFocusChanged( FALSE );

   m_pFocusObject = pFocusObject;
   m_pNearestFocusObject = NULL;

   if (pNearestFocusObject)
   {
      if (m_pFocusObject->IsKindOf(SOT_Vertex))
      {
         Vertex *pFocusVertex = (Vertex *)m_pFocusObject;
         if (pNearestFocusObject->IsKindOf( SOT_Edge))
         {
            for (UINT i=0; i < pFocusVertex->GetNumSurroundingEdges(); i++)
            {
               Edge *pMatchEdge = pFocusVertex->GetSurroundingEdgeAt(i);
               if (pMatchEdge == pNearestFocusObject) {
                  m_pNearestFocusObject = pNearestFocusObject;
                  break;
               }
            }
         }
         else if (pNearestFocusObject->IsKindOf( SOT_Poly))
         {
            for (UINT i=0; i < pFocusVertex->GetNumSurroundingPolys(); i++)
            {
               Poly *pMatchFace = pFocusVertex->GetSurroundingPolyAt(i);
               if (pMatchFace == pNearestFocusObject)
               {
                  m_pNearestFocusObject = pNearestFocusObject;
                  break;
               }
            }
         }
         else
         {
            ASSERT(FALSE);
            return FALSE;
         }
      }
      else if (m_pFocusObject->IsKindOf(SOT_Edge))
      {
         Edge *pFocusEdge = (Edge *)m_pFocusObject;
         if (pNearestFocusObject->IsKindOf( SOT_Vertex))
         {
            if (pFocusEdge->GetFirstVertex()==pNearestFocusObject || pFocusEdge->GetSecondVertex() == pNearestFocusObject)
               m_pNearestFocusObject = pNearestFocusObject;
         }
         else if (pNearestFocusObject->IsKindOf( SOT_Poly))
         {
            if (pFocusEdge->GetFirstPoly()==pNearestFocusObject || pFocusEdge->GetSecondPoly()==pNearestFocusObject)
               m_pNearestFocusObject = pNearestFocusObject;
         }
         else
         {
            ASSERT(FALSE);
            return FALSE;
         }
      }
      else if (m_pFocusObject->IsKindOf(SOT_Poly))
      {
         Poly *pFocusFace = (Poly *)m_pFocusObject;
         if (pNearestFocusObject->IsKindOf( SOT_Vertex))
         {
            for (UINT i=0; i < pFocusFace->GetNumSides(); i++)
            {
               Vertex *pMatchVertex = pFocusFace->GetVertexAt(i);
               if (pMatchVertex == pNearestFocusObject)
               {
                  m_pNearestFocusObject = pNearestFocusObject;
                  break;
               }
            }
         }
         else if (pNearestFocusObject->IsKindOf( SOT_Edge))
         {
            for (UINT i=0; i < pFocusFace->GetNumSides(); i++)
            {
               Edge *pMatchEdge = pFocusFace->GetEdgeAt(i);
               if (pMatchEdge == pNearestFocusObject)
               {
                  m_pNearestFocusObject = pNearestFocusObject;
                  break;
               }
            }
         }
         else
         {
            ASSERT(FALSE);
            return FALSE;
         }
      }
      if (!m_pNearestFocusObject)
         return FALSE;
   }

   if (m_pFocusObject)
      m_pFocusObject->OnFocusChanged( TRUE );

   return TRUE;
}

void SelectionList::SetFocusObjectOnlyNoCheck( SnipeObject *pFocusObject )
{
   if (m_pFocusObject)
      m_pFocusObject->OnFocusChanged( FALSE );

   m_pFocusObject = pFocusObject;

   if (m_pFocusObject)
      m_pFocusObject->OnFocusChanged( TRUE );
}

void SelectionList::SetNearestFocusObjectOnlyNoCheck( SnipeObject *pNearestFocusObject )
{
   m_pNearestFocusObject = pNearestFocusObject;
}

SnipeObjectList *SelectionList::CreateSnipeObjectList()
{
   SnipeObjectList *pObjectList = SNEW SnipeObjectList;

   for (POSITION pos=GetHeadPosition(); pos; )
   {
      SnipeObject *pSnipeObject = GetNext( pos );
      pObjectList->AddTail(pSnipeObject);
   }

   return pObjectList;
}

void SelectionList::RemoveAll()
{
   while (!m_objectlist.IsEmpty())
   {
      SnipeObject *pSnipeObject = m_objectlist.RemoveHead();
      ASSERT( pSnipeObject->IsSelected() );
      pSnipeObject->SetSelected(FALSE);
   }

   m_objectmap.RemoveAll();
   m_bMapDirty = FALSE;

   SetDirty();
}

void SelectionList::Add(SnipeObjectList *pList)
{
   if (pList)
   {
      for (POSITION pos = pList->GetHeadPosition(); pos; )
         Add(pList->GetNext( pos ));
   }
}

void SelectionList::Remove(SnipeObjectList *pList)
{
    if (pList)
    {
       for (POSITION pos = pList->GetHeadPosition(); pos; )
          Remove(pList->GetNext( pos ));
    }
}

void SelectionList::Toggle(SnipeObjectList *pList)
{
   if (pList)
   {
      for (POSITION pos = pList->GetHeadPosition(); pos; )
         Toggle(pList->GetNext( pos ));
   }
}

void SelectionList::SetTo(SnipeObjectList *pList)
{
   RemoveAll();
   Add(pList);
}

const BoundingBox &SelectionList::GetBoundingBox()
{
   if (m_bBoundsDirty)
   {
      m_BoundingBox.Clear();

      m_objectlist.ComputeBoundingBox( m_BoundingBox );
//      Vector worldpos;
//      for (POSITION pos=GetHeadPosition(); pos; )
//      {
//         SnipeObject *pSnipeObject = GetNext( pos );
//         pSnipeObject->GetWorldPos( worldpos );
//         m_BoundingBox.Push( worldpos );
//      }
   }

   m_bBoundsDirty = FALSE;
   return m_BoundingBox;
}

void SelectionList::RebuildMap()
{
   ASSERT( m_bMapDirty );

   m_objectmap.RemoveAll();

   for (POSITION pos=GetHeadPosition(); pos; )
   {
      POSITION objpos = pos;
      SnipeObject *pSnipeObject = GetNext( pos );

      m_objectmap.SetAt( pSnipeObject, objpos );
   }

   m_bMapDirty = FALSE;
}
