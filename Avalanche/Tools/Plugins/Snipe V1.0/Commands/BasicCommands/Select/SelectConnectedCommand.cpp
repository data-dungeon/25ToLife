//

#include "stdafx.h"
#include "SelectConnectedCommand.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "HPoly.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "HVertex.h"
#include "HEdge.h"
#include "../Resource.h"

CommandInstance *SelectConnectedCommandPlugin::CreateCommandInstance() { return SNEW SelectConnectedCommand(this); }

const char *SelectConnectedCommandPlugin::GetCLIName() const { return "SelectConnected"; }; // Name used in scripts and commandline
const char *SelectConnectedCommandPlugin::GetTitleName() const { return "Select Connected"; }

UINT SelectConnectedCommandPlugin::GetBitmapID() const { return IDB_SELECTCONNECTED; }

String SelectConnectedCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Selects all components of same type that are connected together.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

BOOL SelectConnectedCommand::OnDo(StringList &strResults)
{
   if (!SelectBaseCommand::OnDo(strResults))
      return FALSE;

   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();

   CEdgeMap edgemap;
   edgemap.InitHashTable( 12211 ); // Prime number
   CPolyMap polymap;
   polymap.InitHashTable( 12211 ); // Prime number

   HSnipeObjectList *pList = HSnipeObjectList::New();
   for (POSITION pos = pSelectionList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pStartObject = pSelectionList->GetNext(pos);

      if (pStartObject->IsKindOf(SOT_Vertex))
         SelectConnectedVertex(pList, edgemap, (HVertex *)pStartObject);
      else if (pStartObject->IsKindOf(SOT_Edge))
         SelectConnectedEdge(pList, edgemap, (HEdge *)pStartObject);
      else if (pStartObject->IsKindOf(SOT_Poly))
         SelectConnectedFace(pList, polymap, (HPoly *)pStartObject);
      else
         ASSERT(FALSE);
   }
   
   if (!pList->IsEmpty())
      pSelectionList->Add( pList );

   delete pList;

   return TRUE;
}

void SelectConnectedCommand::SelectConnectedVertex( HSnipeObjectList *pSelectionList, CEdgeMap &edgemap, HVertex *pStartVertex )
{
   ASSERT( pStartVertex );

   UINT nEdgeCount = pStartVertex->GetNumSurroundingEdges();
   HEdge *pTemp; 

   for (UINT nEdge=0; nEdge < nEdgeCount; nEdge++)
   {
      HEdge *pEdge = pStartVertex->GetSurroundingEdgeAt(nEdge);
      if (edgemap.Lookup(pEdge, pTemp))
         continue;
      
      edgemap.SetAt(pEdge, pEdge);
      
      HVertex *pVertex = pEdge->GetFirstVertex();
      if (pVertex->IsVisible())
         pSelectionList->AddTail( pVertex );
      SelectConnectedVertex( pSelectionList, edgemap, pVertex );
      
      pVertex = pEdge->GetSecondVertex();
      if (pVertex->IsVisible())
         pSelectionList->AddTail( pVertex );
      SelectConnectedVertex( pSelectionList, edgemap, pVertex );
   }
}

void SelectConnectedCommand::SelectConnectedEdge( HSnipeObjectList *pSelectionList, CEdgeMap &edgemap, HEdge *pStartEdge )
{
   ASSERT(pStartEdge);

   for (UINT nEdge=0; nEdge<2; nEdge++)
   {
      HVertex *pStartVertex = nEdge==0?pStartEdge->GetFirstVertex():pStartEdge->GetSecondVertex();
      UINT nEdgeCount = pStartVertex->GetNumSurroundingEdges();
      HEdge *pTemp; 
      for (UINT nEdge=0; nEdge < nEdgeCount; nEdge++)
      {
         HEdge *pEdge = pStartVertex->GetSurroundingEdgeAt(nEdge);
         if (edgemap.Lookup(pEdge, pTemp))
            continue;
         edgemap.SetAt(pEdge, pEdge);
         if (pEdge->IsVisible())
            pSelectionList->AddTail( pEdge );
         SelectConnectedEdge( pSelectionList, edgemap, pEdge );
      }
   }
}

void SelectConnectedCommand::SelectConnectedFace( HSnipeObjectList *pSelectionList, CPolyMap &polymap, HPoly *pStartPoly )
{
   ASSERT( pStartPoly );

   UINT nVertexCount = pStartPoly->GetNumSides();
   for (UINT nVertex=0; nVertex<nVertexCount; nVertex++)
   {
      HVertex *pVertex = pStartPoly->GetVertexAt(nVertex);
      UINT nFaceCount = pVertex->GetNumSurroundingPolys();
      HPoly *pTemp; 
      for (UINT nFace=0; nFace < nFaceCount; nFace++)
      {
         HPoly *pPoly = pVertex->GetSurroundingPolyAt(nFace);
         if (polymap.Lookup(pPoly, pTemp))
            continue;
         polymap.SetAt(pPoly, pPoly);
         if (pPoly->IsVisible())
            pSelectionList->AddTail( pPoly );
         SelectConnectedFace( pSelectionList, polymap, pPoly );
      }
   }
}
