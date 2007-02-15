//

#include "stdafx.h"
#include "PeakCommand.h"
#include "HEdge.h"
#include "HPoly.h"
#include "HMainInterface.h"
#include "HSelectionList.h"
#include "HPolyModel.h"
#include "../Resource.h"

CommandInstance *PeakCommandPlugin::CreateCommandInstance() { return SNEW PeakCommand(this); }

const char *PeakCommandPlugin::GetCLIName() const { return "Peak"; }; // Name used in scripts and commandline
const char *PeakCommandPlugin::GetTitleName() const { return "Peak"; }
UINT PeakCommandPlugin::GetBitmapID() const { return IDB_PEAK; }

String PeakCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp  = "Peaks the vertices, edges, or faces provided, if none are provided then the\n";
   strHelp += "selected objects will be used.";
   strHelp += (String) "    " + GetCLIName() + "[-o PEAKTYPE] [-e EDGENAME] [-f FACENAME] [-v VERTEXNAME EDGENAME].\n";
   strHelp += (String) "      -o (PEAKTYPE can be one of the following.)\n";
   strHelp += (String) "         PEAK (Peaks the edges. This is default of no option is provided.)\n";
   strHelp += (String) "         SOFT (Makes all the edges soft.)\n";
   strHelp += (String) "         TOGGLE (Toggles the edge from peaked to soft.)\n";
   strHelp += (String) "      -e (EDGENAME - Will peak the entire edge.)\n";
   strHelp += (String) "      -f (FACENAME - Will peak all edges of the face.)\n";
   strHelp += (String) "      -v (VERTEXNAME - Will peak the vertex in the direction of EDGENAME.)\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName()+ "\n";
   strHelp += (String) "    " + GetCLIName()+ " -o SOFT\n";
   strHelp += (String) "    " + GetCLIName() + " -e Edge(5) -e Edge(7)\n";
   strHelp += (String) "    " + GetCLIName() + " -f Face(3)\n";
   strHelp += (String) "    " + GetCLIName() + " -v Vertex(6) Edge(2)\n";
   strHelp += (String) "    " + GetCLIName() + " -o TOGGLE -e Edge(5) -e Edge(7)\n";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

UndoType PeakCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

PeakCommand::~PeakCommand()
{
   for (POSITION pos=m_vertexedgemap.GetStartPosition(); pos;)
   {
      UINT nUnused;
      VertexEdge *pVertexEdge;
      m_vertexedgemap.GetNextAssoc(pos, pVertexEdge, nUnused);
      delete pVertexEdge;
   }

   for (POSITION pos=m_peakvertexinfolist.GetHeadPosition(); pos;)
      delete m_peakvertexinfolist.GetNext(pos);
}

BOOL PeakCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-o" ) == 0)
   {
      String type = tokenlist.GetNext();
      if (type == "PEAK")
         m_nPeakType = PT_PEAK;
      else if (type == "SMOOTH")
         m_nPeakType = PT_SMOOTH;
      else if (type == "TOGGLE")
         m_nPeakType = PT_TOGGLE;
      else
      {
         String strMsg = "Invalid option %1";
         strMsg.Replace("%1", type);
         THROW_FATAL_ERROR(strMsg);
      }
      return TRUE;
   }
   else if (stricmp( szToken, "-v") == 0)
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute Peak command, no vertex provided.");

      String strError;
      HVertex *pVertex = (HVertex *)GetHMainInterface()->DescriptionToObject(szNextToken, &strError);
      if (!pVertex || !pVertex->IsKindOf(SOT_Vertex))
      {
         String strMsg("Unable to execute Peak command, invalid vertex %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);
      }

      szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute Peak command, no edge provided.");

      HEdge *pEdge = (HEdge *)GetHMainInterface()->DescriptionToObject(szNextToken, &strError);
      if (!pEdge || !pEdge->IsKindOf(SOT_Edge))
      {
         String strMsg("Unable to execute Peak command, invalid edge %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);
      }
      VertexEdge *pVertexEdge = SNEW VertexEdge( pVertex, pEdge );
      Add( pVertexEdge );
      return TRUE;
   }
   else if (stricmp( szToken, "-e") == 0)
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute Peak command, no edge provided.");

      String strError;
      HEdge *pEdge = (HEdge *)GetHMainInterface()->DescriptionToObject(szNextToken, &strError);
      if (!pEdge || !pEdge->IsKindOf(SOT_Edge)/* || pEdge->IsDangle()*/)
      {
         String strMsg("Unable to execute Peak command, invalid edge %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);
      }
      AddVertexEdges(pEdge);
      return TRUE;
   }
   else if (stricmp( szToken, "-f") == 0)
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute Peak command, no face provided.");

      String strError;
      HPoly *pFace = (HPoly *)GetHMainInterface()->DescriptionToObject(szNextToken, &strError);
      if (!pFace || !pFace->IsKindOf(SOT_Poly))
      {
         String strMsg("Unable to execute Peak command, invalid face %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);
      }
      AddVertexEdges(pFace);
      return TRUE;
   }

   return FALSE;
}

void PeakCommand::Add(VertexEdge *pVertexEdge)
{
   UINT nUnused;
   if (!m_vertexedgemap.Lookup(pVertexEdge, nUnused))
      m_vertexedgemap.SetAt(pVertexEdge, 0);
   else
      delete pVertexEdge;
}

void PeakCommand::AddVertexEdges(HEdge *pEdge)
{
   HPoly *pPoly = pEdge->GetPoly();
   UINT nFaceRelativeIndex = pEdge->GetFaceRelativeIndex( pPoly );

   for (UINT i=0; i<2; i++)
   {
      UINT nIndex = i==0 ? pPoly->GetPrevIndex(nFaceRelativeIndex) : pPoly->GetNextIndex(nFaceRelativeIndex);
      HEdge *pBorderEdge = pPoly->GetEdgeAt( nIndex );
      HVertex *pVertex = pEdge->GetSharedVertex(pBorderEdge);
      VertexEdge *pVertexEdge = SNEW VertexEdge(pVertex, pBorderEdge);
      Add(pVertexEdge);
   }
}

void PeakCommand::AddVertexEdges(HVertex *pVertex, HEdge *pEdge)
{
   VertexEdge *pVertexEdge = SNEW VertexEdge(pVertex, pEdge);
   Add(pVertexEdge);
}

void PeakCommand::AddVertexEdges(HPoly *pFace)
{
   for (UINT i=0; i<pFace->GetNumSides(); i++)
   {
      HEdge *pEdge = pFace->GetEdgeAt(i);
      VertexEdge *pVertexEdge = SNEW VertexEdge(pEdge->GetFirstVertex(), pEdge);
      Add(pVertexEdge);
      pVertexEdge = SNEW VertexEdge(pEdge->GetSecondVertex(), pEdge);
      Add(pVertexEdge);
   }
}

BOOL PeakCommand::OnValidate( String &strOutError )
{
   if (!CommandInstance::OnValidate(strOutError))
      return FALSE;

   CMap<HVertex *, HVertex *, UINT, UINT> m_vertexmap;
   m_vertexmap.InitHashTable(GetPrimeLargerThan(100));

   if (m_vertexedgemap.IsEmpty())
   {
      // Lets use the current selectionlist
      HMainInterface *pMainInterface = GetHMainInterface();
      HSelectionList *pSelectionList = pMainInterface->GetCurrentSelectionList();
      
      for (POSITION pos = pSelectionList->GetHeadPosition(); pos; )
      {
         HSnipeObject *pObject = pSelectionList->GetNext(pos);
         if (pObject->IsKindOf(SOT_Edge))
            AddVertexEdges((HEdge *)pObject);
         else if (pObject->IsKindOf(SOT_Poly))
            AddVertexEdges((HPoly *)pObject);
         else if (pObject->IsKindOf(SOT_Vertex))
            m_vertexmap.SetAt((HVertex *)pObject, 0);
      }
   }

   if (!m_vertexmap.IsEmpty())
   {
      HPolyModel *pModel = (HPolyModel *)GetHMainInterface()->GetCurrentCommandObject();
      UINT nUnUsed;

      if (pModel && pModel->IsKindOf(SOT_PolyModel))
      {
         for (POSITION pos = pModel->GetHeadEdgePosition(); pos;)
         {
            HEdge *pEdge = pModel->GetNextEdge(pos);
            if (m_vertexmap.Lookup(pEdge->GetFirstVertex(), nUnUsed) && m_vertexmap.Lookup(pEdge->GetSecondVertex(), nUnUsed))
            {
               AddVertexEdges(pEdge->GetFirstVertex(), pEdge);
               AddVertexEdges(pEdge->GetSecondVertex(), pEdge);
            }
         }
      }
   }

   HSnipeObject *pFocusObject = HSelectionList::GetFocusObject();
   HSnipeObject *pNearObject = HSelectionList::GetNearestFocusObject();

   if (pFocusObject && pFocusObject->IsKindOf(SOT_Vertex) && pNearObject && pNearObject->IsKindOf(SOT_Edge))
      AddVertexEdges((HVertex *)pFocusObject, (HEdge *)pNearObject);

   if (m_vertexedgemap.IsEmpty())
   {
      strOutError = "No vertices, edges or faces provided.";
      return FALSE;
   }

   return TRUE;
}

MyVertexPeakInfo *PeakCommand::DoPeak( VertexEdge *pVertexEdge )
{
   BOOL bPeak;

   switch (m_nPeakType)
   {
   case PT_PEAK:
      bPeak = TRUE;
      break;
   case PT_SMOOTH:
      bPeak = FALSE;
      break;
   case PT_TOGGLE:
      bPeak = !pVertexEdge->m_pVertex->IsPeaked(pVertexEdge->m_pEdge);
      break;
   }

   HVertexPeakInfo *pInfo = pVertexEdge->m_pVertex->Peak(pVertexEdge->m_pEdge, bPeak);
   if (!pInfo)
      return NULL;

   return SNEW MyVertexPeakInfo(bPeak, pInfo);
}

BOOL PeakCommand::OnDo(StringList &strResults)
{
   for (POSITION pos = m_vertexedgemap.GetStartPosition(); pos;)
   {
      VertexEdge *pVertexEdge;
      UINT nUnused;

      m_vertexedgemap.GetNextAssoc(pos, pVertexEdge, nUnused);
      MyVertexPeakInfo *pMyInfo = DoPeak(pVertexEdge);
      if (pMyInfo)
         m_peakvertexinfolist.AddTail(pMyInfo);
   }

   return TRUE;
}

void PeakCommand::OnUndo()
{
   for (POSITION pos = m_peakvertexinfolist.GetTailPosition();pos;)
   {
      MyVertexPeakInfo *pMyInfo = m_peakvertexinfolist.GetPrev(pos);
      pMyInfo->Undo();
   }
}

void PeakCommand::OnRedo()
{
   for (POSITION pos = m_peakvertexinfolist.GetHeadPosition();pos;)
   {
      MyVertexPeakInfo *pMyInfo = m_peakvertexinfolist.GetNext(pos);
      pMyInfo->Redo();
   }
}