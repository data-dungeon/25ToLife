//

#include "stdafx.h"
#include "ConnectCommand.h"
#include "HMainInterface.h"
#include "HSelectionList.h"
#include "HEdge.h"
#include "HPoly.h"
#include "HPolyModel.h"
#include "../Resource.h"

CommandInstance *ConnectCommandPlugin::CreateCommandInstance() { return SNEW ConnectCommand(this); }

const char *ConnectCommandPlugin::GetCLIName() const { return "Connect"; }; // Name used in scripts and commandline
const char *ConnectCommandPlugin::GetTitleName() const { return "Connect"; }
UINT ConnectCommandPlugin::GetBitmapID() const { return IDB_CONNECT; }

String ConnectCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp  = "Connects edges between selected vertices on a face, or from the center of selected edges on a face.\n";
   strHelp += (String) "    " + GetCLIName() + "\n";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

UsedPoly::UsedPoly( HPoly *pPoly )
{
   m_pPoly = pPoly;
   m_usedvertexmap.InitHashTable(GetPrimeLargerThan((UINT)(m_pPoly->GetNumSides() * 1.2f)));
}

ConnectCommand::ConnectCommand(HCommandPluginHelper *pPlugin) :
CommandInstance(pPlugin)
{
   HMainInterface *pMainInterface = GetHMainInterface();
   HSelectionList *pSelectionList = pMainInterface->GetCurrentSelectionList();

   UINT nCount = pSelectionList->GetCount();
   m_allvertexmap.InitHashTable(GetPrimeLargerThan((UINT)(nCount * 1.2f)));
   m_usedpolymap.InitHashTable(GetPrimeLargerThan((UINT)(nCount * 1.2f)));
}

ConnectCommand::~ConnectCommand()
{
   for (POSITION pos = m_addededges.GetTailPosition(); pos;)
      delete m_addededges.GetPrev(pos);      

   for (POSITION pos = m_addedvertices.GetTailPosition(); pos;)
      delete m_addedvertices.GetPrev(pos);  

   for (POSITION pos = m_usedpolymap.GetStartPosition(); pos; )
   {
      UsedPoly *pUsedPoly;
      HPoly *pPoly;
      m_usedpolymap.GetNextAssoc(pos, pPoly, pUsedPoly);
      delete pUsedPoly;
   }
}

UndoType ConnectCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL ConnectCommand::OnValidate( String &strOutError )
{
   if (!CommandInstance::OnValidate(strOutError))
      return FALSE;

   // Lets use the current selectionlist
   HMainInterface *pMainInterface = GetHMainInterface();
   HSelectionList *pSelectionList = pMainInterface->GetCurrentSelectionList();

   for (POSITION pos = pSelectionList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pObject = pSelectionList->GetNext(pos);
      if (pObject->IsKindOf(SOT_Edge))
         m_edgelist.AddTail((HEdge *)pObject);
      if (pObject->IsKindOf(SOT_Vertex))
      {
         HVertex *pVertex = (HVertex *)pObject;
         m_allvertexmap.SetAt(pVertex->GetID(), pVertex );
      }
   }

   if (m_edgelist.IsEmpty() && m_allvertexmap.IsEmpty())
   {
      strOutError = "No Edges or Vertices selected to connect together.";
      return FALSE;
   }

   if (m_edgelist.GetCount()==1)
   {
      strOutError = "Only one edge provided, not enough to connect together.";
      return FALSE;
   }

   return TRUE;
}

BOOL ConnectCommand::IsValidSplit(HPoly *pPoly, HVertex *pV0, HVertex *pV1)
{
   // Test to see if they are connected on same spline
   HEdge *pEdge0, *pEdge1;
   pV0->GetEdgePair( pPoly, &pEdge0, &pEdge1 );

   HEdge *pPrevEdge = pEdge1;
   HEdge *pEdge = pEdge0;
   while (TRUE)
   {
      if (pEdge->GetFirstVertex()==pV1 || pEdge->GetSecondVertex()==pV1)
         return FALSE;

      HEdge *pNextEdge = pEdge->GetContinuousEdge(pPrevEdge);
      if (!pNextEdge)
         break;

      if (pNextEdge->GetFirstPoly() != pPoly && pNextEdge->GetSecondPoly()!= pPoly)
         break;

      pPrevEdge = pEdge;
      pEdge = pNextEdge;
   }

   pPrevEdge = pEdge0;
   pEdge = pEdge1;
   while (TRUE)
   {
      if (pEdge->GetFirstVertex()==pV1 || pEdge->GetSecondVertex()==pV1)
         return FALSE;

      HEdge *pNextEdge = pEdge->GetContinuousEdge(pPrevEdge);
      if (!pNextEdge)
         break;

      if (pNextEdge->GetFirstPoly() != pPoly && pNextEdge->GetSecondPoly()!= pPoly)
         break;

      pPrevEdge = pEdge;
      pEdge = pNextEdge;
   }

   return TRUE;
}

BOOL ConnectCommand::FindBestMatch( HPoly *pPoly, HVertex *pVertex, HVertex *&pOutFirstMatch, HVertex *&pOutSecondMatch )
{
   pOutFirstMatch = NULL;
   pOutSecondMatch = NULL;
   UINT nStartRelativeIndex = pPoly->FindVertexFaceRelativeIndex(pVertex);
   UINT nRelativeIndex = nStartRelativeIndex;
   ASSERT(nRelativeIndex!=-1);

   BOOL bFound = FALSE;
   UINT nNextNumSides = 0;
   HVertex *pNextVertex;

   UsedPoly *pUsedPoly=NULL;
   m_usedpolymap.Lookup( pPoly, pUsedPoly );
   ASSERT(pUsedPoly);

   for ( UINT i=0; i<pPoly->GetNumSides(); i++)
   {
      nNextNumSides++;
      nRelativeIndex = pPoly->GetNextIndex(nRelativeIndex);
      pNextVertex = pPoly->GetVertexAt(nRelativeIndex);

      HVertex *pDummy;
      if (pNextVertex != pVertex && m_allvertexmap.Lookup(pNextVertex->GetID(), pDummy) && !pUsedPoly->m_usedvertexmap.Lookup(pNextVertex->GetID(), pDummy))
      {
         if (IsValidSplit(pPoly, pVertex, pNextVertex))
         {
            bFound = TRUE;
            break;
         }
      }
   }

   if (!bFound)
   {
      pNextVertex = NULL;
      nNextNumSides = 0;
   }

   bFound = FALSE;
   UINT nPrevNumSides = 0;
   HVertex *pPrevVertex;
   nRelativeIndex = nStartRelativeIndex;
   for ( UINT i=0; i<pPoly->GetNumSides(); i++)
   {
      nPrevNumSides++;
      nRelativeIndex = pPoly->GetPrevIndex(nRelativeIndex);
      pPrevVertex = pPoly->GetVertexAt(nRelativeIndex);

      HVertex *pDummy;
      if (pPrevVertex != pVertex && m_allvertexmap.Lookup(pPrevVertex->GetID(), pDummy) && !pUsedPoly->m_usedvertexmap.Lookup(pPrevVertex->GetID(), pDummy))
      {
         if (IsValidSplit(pPoly, pVertex, pPrevVertex))
         {
            bFound = TRUE;
            break;
         }
      }
   }
   if (!bFound)
   {
      pPrevVertex = NULL;
      nPrevNumSides = 0;
   }

   if (!pPrevVertex && !pNextVertex)
      return FALSE;
   else if (pPrevVertex && !pNextVertex)
      pOutFirstMatch = pPrevVertex;
   else if (!pPrevVertex && pNextVertex)
      pOutFirstMatch = pNextVertex;
   else if (nPrevNumSides == nNextNumSides)
   {
      pOutFirstMatch = pPrevVertex;
      pOutSecondMatch = pNextVertex;
   }
   else
      pOutFirstMatch = (nPrevNumSides < nNextNumSides) ? pPrevVertex : pNextVertex;

   return TRUE;
}

BOOL ConnectCommand::OnDo(StringList &strResults)
{
   for (POSITION pos = m_edgelist.GetHeadPosition(); pos;)
   {
      HEdge *pEdge = m_edgelist.GetNext(pos);
      HVertex::HDeleteInfo *pInfo = pEdge->SplitEdge();
      if (pInfo)
      {
         m_addedvertices.AddTail( pInfo );
         HVertex *pVertex = (HVertex *)pInfo->GetSnipeObject();
         m_allvertexmap.SetAt(pVertex->GetID(), pVertex);
      }
      else
      {
         String strMsg("Unable to split Edge(%1)");
         strMsg.Replace("%1", (ULONG)pEdge->GetID());
         DisplayWarning(strMsg);
      }
   }

   BOOL bChanged = TRUE;
   while (bChanged)
   {
      bChanged = FALSE;
      for (POSITION pos = m_allvertexmap.GetStartPosition(); pos; )
      {
         HVertex *pVertex, *pDummy;
         UINT nID;
         m_allvertexmap.GetNextAssoc(pos, nID, pVertex);

         for (UINT i=0; i<pVertex->GetNumSurroundingPolys(); i++)
         {
            HPoly *pPoly = pVertex->GetSurroundingPolyAt(i);
            
            UsedPoly *pUsedPoly;

            if (!m_usedpolymap.Lookup( pPoly, pUsedPoly ))
            {
               pUsedPoly = SNEW UsedPoly(pPoly);
               m_usedpolymap.SetAt(pPoly, pUsedPoly);
            }
            else
            {
               if (pUsedPoly->m_usedvertexmap.Lookup( pVertex->GetID(), pDummy ))
                  continue;
            }

            HVertex *pBestMatch1;
            HVertex *pBestMatch2; 
            
            if (!FindBestMatch( pPoly, pVertex, pBestMatch1, pBestMatch2 ))
               continue;

            HVertex *pBackBestMatch1;
            HVertex *pBackBestMatch2; 

            if (!FindBestMatch(pPoly, pBestMatch1, pBackBestMatch1, pBackBestMatch2))
               continue;

            if (pVertex != pBackBestMatch1 &&  pVertex != pBackBestMatch2)
               continue;

            SplitEdge *pSplitEdge = SNEW SplitEdge( pVertex, pBestMatch1 );
            m_splitedgelist.AddTail(pSplitEdge);
            bChanged = TRUE;

            pUsedPoly->m_usedvertexmap.SetAt(pVertex->GetID(), pVertex);
            pUsedPoly->m_usedvertexmap.SetAt(pBestMatch1->GetID(), pBestMatch1);
         }
      }
   }
   
   HPolyModel *pModel = (HPolyModel *)GetHMainInterface()->GetCurrentCommandObject();
   for (POSITION pos = m_splitedgelist.GetHeadPosition(); pos;)
   {
      SplitEdge *pSplitEdge = m_splitedgelist.GetNext(pos);
      HEdge::HDeleteInfo *pInfo = pModel->AddEdge( pSplitEdge->GetV0(), pSplitEdge->GetV1(), HEDGE_FINDCONTINUOUS, HEDGE_FINDCONTINUOUS );
      ASSERT(pInfo);
      m_addededges.AddTail( pInfo );
   }

   return TRUE;
}

void ConnectCommand::OnUndo()
{
   for (POSITION pos = m_addededges.GetTailPosition(); pos;)
   {
      HEdge::HDeleteInfo *pInfo = m_addededges.GetPrev(pos);
      pInfo->Delete();
   }

   for (POSITION pos = m_addedvertices.GetTailPosition(); pos;)
   {
      HVertex::HDeleteInfo *pInfo = m_addedvertices.GetPrev(pos);
      pInfo->Delete();
   }
}


void ConnectCommand::OnRedo()
{
   for (POSITION pos = m_addedvertices.GetHeadPosition(); pos;)
   {
      HVertex::HDeleteInfo *pInfo = m_addedvertices.GetNext(pos);
      pInfo->UnDelete();
   }

   for (POSITION pos = m_addededges.GetHeadPosition(); pos;)
   {
      HEdge::HDeleteInfo *pInfo = m_addededges.GetNext(pos);
      pInfo->UnDelete();
   }
}