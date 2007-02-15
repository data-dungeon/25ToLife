//

#include "stdafx.h"
#include "GrowSelectionCommand.h"
#include "HSelectionList.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "HVertex.h"
#include "HEdge.h"
#include "HPoly.h"
#include "../Resource.h"

CommandInstance *GrowSelectionCommandPlugin::CreateCommandInstance() { return SNEW GrowSelectionCommand(this); }

const char *GrowSelectionCommandPlugin::GetCLIName() const { return "GrowSelection"; }; // Name used in scripts and commandline
const char *GrowSelectionCommandPlugin::GetTitleName() const { return "Grow Selection"; }

UINT GrowSelectionCommandPlugin::GetBitmapID() const { return IDB_GROWSELECTION; }

String GrowSelectionCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Grow the selection.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void GrowSelectionCommand::GrowVertex( HVertex *pVertex, HSnipeObjectList *pList )
{
   pList->AddTail(pVertex);

   for (UINT i=0; i<pVertex->GetNumSurroundingEdges(); i++)
   {
      HEdge *pEdge = pVertex->GetSurroundingEdgeAt(i);
      HVertex *pOther = pEdge->GetConnectedVertex(pVertex );
      if (pOther->IsVisible() && !pOther->IsSelected())
         pList->AddTail(pOther);
   }
}

void GrowSelectionCommand::GrowEdge( HEdge *pEdge, HSnipeObjectList *pList )
{
   pList->AddTail(pEdge);

   HVertex *pVertex = pEdge->GetFirstVertex();
   for (UINT i=0; i<pVertex->GetNumSurroundingEdges(); i++)
   {
      HEdge *pOther = pVertex->GetSurroundingEdgeAt(i);
      if (pOther->IsVisible() && !pOther->IsSelected())
         pList->AddTail(pOther);
   }

   pVertex = pEdge->GetSecondVertex();
   for (UINT i=0; i<pVertex->GetNumSurroundingEdges(); i++)
   {
      HEdge *pOther = pVertex->GetSurroundingEdgeAt(i);
      if (pOther->IsVisible() && !pOther->IsSelected())
         pList->AddTail(pOther);
   }
}

void GrowSelectionCommand::GrowFace( HPoly *pFace, HSnipeObjectList *pList )
{
   pList->AddTail(pFace);

   for (UINT nVert=0; nVert<pFace->GetNumSides(); nVert++)
   {
      HVertex *pVertex = pFace->GetVertexAt( nVert );

      for (UINT i=0; i<pVertex->GetNumSurroundingPolys(); i++)
      {
         HPoly *pOther = pVertex->GetSurroundingPolyAt(i);
         if (pOther->IsVisible() && !pOther->IsSelected())
            pList->AddTail(pOther);
      }
   }
}

BOOL GrowSelectionCommand::OnDo(StringList &strResults)
{
   SelectBaseCommand::OnDo(strResults);

   HSnipeObjectList *pList = HSnipeObjectList::New();
   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();

   for (POSITION pos = pSelectionList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pSnipeObject = pSelectionList->GetNext( pos );

      if (pSnipeObject->IsKindOf( SOT_Vertex ))
         GrowVertex((HVertex *)pSnipeObject, pList);
      else if (pSnipeObject->IsKindOf( SOT_Edge ))
         GrowEdge((HEdge *)pSnipeObject, pList);
      else if (pSnipeObject->IsKindOf( SOT_Poly ))
         GrowFace((HPoly *)pSnipeObject, pList);
      else
         ASSERT(FALSE);
   }

   if (!pList->IsEmpty())
      pSelectionList->Add( pList );

   delete pList;

   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////

CommandInstance *ShrinkSelectionCommandPlugin::CreateCommandInstance() { return SNEW ShrinkSelectionCommand(this); }

const char *ShrinkSelectionCommandPlugin::GetCLIName() const { return "ShrinkSelection"; }; // Name used in scripts and commandline
const char *ShrinkSelectionCommandPlugin::GetTitleName() const { return "Shrink Selection"; }

UINT ShrinkSelectionCommandPlugin::GetBitmapID() const { return IDB_SHRINKSELECTION; }

String ShrinkSelectionCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Shrink the selection.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void ShrinkSelectionCommand::ShrinkVertex( HVertex *pVertex, HSnipeObjectList *pList )
{
   for (UINT i=0; i<pVertex->GetNumSurroundingEdges(); i++)
   {
      HEdge *pEdge = pVertex->GetSurroundingEdgeAt(i);
      HVertex *pOther = pEdge->GetConnectedVertex( pVertex );
      if (!pOther->IsSelected() || pEdge->IsBorderEdge() )
      {
         pList->AddTail(pVertex);
         return;
      }
   }
}

void ShrinkSelectionCommand::ShrinkEdge( HEdge *pEdge, HSnipeObjectList *pList )
{
   if (pEdge->IsBorderEdge())
   {
      pList->AddTail( pEdge );
      return;
   }

   HVertex *pVertex = pEdge->GetFirstVertex();
   for (UINT i=0; i<pVertex->GetNumSurroundingEdges(); i++)
   {
      HEdge *pOther = pVertex->GetSurroundingEdgeAt(i);
      if (!pOther->IsSelected())
      {
         pList->AddTail(pEdge);
         return;
      }
   }

   pVertex = pEdge->GetSecondVertex();
   for (UINT i=0; i<pVertex->GetNumSurroundingEdges(); i++)
   {
      HEdge *pOther = pVertex->GetSurroundingEdgeAt(i);
      if (!pOther->IsSelected())
      {
         pList->AddTail(pEdge);
         return;
      }
   }
}

void ShrinkSelectionCommand::ShrinkFace( HPoly *pFace, HSnipeObjectList *pList )
{
   for (UINT nEdge=0; nEdge<pFace->GetNumSides(); nEdge++ )
   {
      HEdge *pEdge = pFace->GetEdgeAt( nEdge );
      if (pEdge->IsBorderEdge())
      {
         pList->AddTail(pFace);
         return;
      }
   }

   for (UINT nVert=0; nVert<pFace->GetNumSides(); nVert++)
   {
      HVertex *pVertex = pFace->GetVertexAt( nVert );

      for (UINT i=0; i<pVertex->GetNumSurroundingPolys(); i++)
      {
         HPoly *pOther = pVertex->GetSurroundingPolyAt(i);
         if (!pOther->IsSelected())
         {
            pList->AddTail(pFace);
            return;
         }
      }
   }
}

BOOL ShrinkSelectionCommand::OnDo(StringList &strResults)
{
   SelectBaseCommand::OnDo(strResults);

   HSnipeObjectList *pList = HSnipeObjectList::New();
   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();

   for (POSITION pos = pSelectionList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pSnipeObject = pSelectionList->GetNext( pos );

      if (pSnipeObject->IsKindOf( SOT_Vertex ))
         ShrinkVertex((HVertex *)pSnipeObject, pList);
      else if (pSnipeObject->IsKindOf( SOT_Edge ))
         ShrinkEdge((HEdge *)pSnipeObject, pList);
      else if (pSnipeObject->IsKindOf( SOT_Poly ))
         ShrinkFace((HPoly *)pSnipeObject, pList);
      else
         ASSERT(FALSE);
   }

   if (!pList->IsEmpty())
   {
      if (pSelectionList->GetCount() != pList->GetSize()) // Don't want to remove last one
         pSelectionList->Remove( pList );
   }

   delete pList;

   return TRUE;
}