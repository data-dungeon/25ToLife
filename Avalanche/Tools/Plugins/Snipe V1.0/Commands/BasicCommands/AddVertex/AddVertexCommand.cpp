//

#include "stdafx.h"
#include "AddVertexCommand.h"
#include "../resource.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "HComponent.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "HEdge.h"
#include "HPolyModel.h"
#include "HPoly.h"
#include "HPatchModel.h"
#include "HVertex.h"

CommandInstance *AddVertexCommandPlugin::CreateCommandInstance() { return SNEW AddVertexCommand(this); }

const char *AddVertexCommandPlugin::GetCLIName() const { return "AddVertex"; }; // Name used in scripts and commandline
const char *AddVertexCommandPlugin::GetTitleName() const { return "AddVertex"; }

UINT AddVertexCommandPlugin::GetBitmapID() const { return IDB_ADDVERTEX; }

String AddVertexCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Add a vertex to a model.\n";
   strHelp += (String) "  " + GetCLIName() + " [-fc] [-lc] [-fce EDGENAME] [-lce EDGENAME] [-nf] [-s SELECTTYPE] [-e  EDGENAME fPERCENTALONGEDGE] [-p vPOSITION] [-v  VERTEXNAME] ...\n";
   strHelp += (String) "    -e  Add vertex to EDGENAME at the floating point percentage fPERCENTAGEALONGEDGE of the edge.\n";
   strHelp += (String) "    -p  Add vertex at the vector position vPOSITION.\n";
   strHelp += (String) "    -v  Use the vertex VERTEXNAME.\n";
   strHelp += (String) "    -fce Make the first added edge continuous on a patch model to EDGENAME.\n";
   strHelp += (String) "    -lce Make the last added edge continuous on a patch model to EDGENAME.\n";
   strHelp += (String) "    -fc try to make the first added edge continuous on a patch model by finding best appropriate edge.\n";
   strHelp += (String) "    -lc try to make the first last edge continuous on a patch model by finding best appropriate edge.\n";
   strHelp += (String) "    -nf No auto find 3 and 4 point faces.\n";
   strHelp += (String) "    -al Auto loop.  If edges are split creating vertices that when connected could form a loop, automatically\n";
   strHelp += (String) "        add the additional edge to make the loop.\n";
   strHelp += (String) "    -s  By default selection isn't changed, SELECTIONTYPE can be one of the following.\n";
   strHelp += (String) "        ALL selects all vertices.\n";
   strHelp += (String) "        LAST selects last vertex.\n";
   strHelp += (String) "    When two or more vertices are added, edges between them will attempt to be created if valid.\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " -e Edge(3) .5 -e Edge(4) .5 -e Edge(5) .25\n";
   strHelp += (String) "    " + GetCLIName() + " -p 25.3 8.0 0.0 -p 235.0 8.0 0.0\n";
   strHelp += (String) "    " + GetCLIName() + " -p 25.3 8.0 0.0 -e Edge(3) .5\n";
   strHelp += (String) "    " + GetCLIName() + " -v Vertex(43) - p 25.3 8.0 0.0 -e Edge(3) .5\n";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

AddVertexCommand::~AddVertexCommand()
{
   for ( POSITION pos = m_newdeletefaceinfos.GetHeadPosition(); pos; )
   {
      HDeleteFaceInfo *pDeleteFaceInfo = m_newdeletefaceinfos.GetNext( pos );
      delete pDeleteFaceInfo;
   }

   for ( int i=0; i<m_nNumNewEdgeDeleteInfos; i++ )
   {
      HEdge::HDeleteInfo *pEdgeDeleteInfo = m_pNewEdgeDeleteInfos[i];
      delete pEdgeDeleteInfo;
   }

   if (m_pNewEdgeDeleteInfos)
      delete [] m_pNewEdgeDeleteInfos;

   for ( POSITION pos = m_newvertexdeleteinfos.GetHeadPosition(); pos; )
   {
      HVertex::HDeleteInfo *pVertexDeleteInfo = m_newvertexdeleteinfos.GetNext( pos );
      delete pVertexDeleteInfo;
   }

   if (m_pNewSelectedVerts)
      delete m_pNewSelectedVerts;
}

BOOL AddVertexCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{

   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-s" ) == 0)
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute AddVertex command, no selectiontype provided.");

      String strSelectionType(szNextToken);
      if (strSelectionType=="ALL")
         m_selectiontype = ST_ALL;
      else if (strSelectionType=="LAST")
         m_selectiontype = ST_LAST;
      else
      {
         String strMsg("Unable to execute AddVertex command, invalid selectiontype %1.");
         strMsg.Replace("%1", strSelectionType);
         THROW_FATAL_ERROR(strMsg);
      }
      return TRUE;
   }
   else if (stricmp( szToken, "-fc" ) == 0)
   {
      m_pPreviousContinuity = HEDGE_FINDCONTINUOUS;
      return TRUE;
   }
   else if (stricmp( szToken, "-lc" ) == 0)
   {
      m_pNextContinuity = HEDGE_FINDCONTINUOUS;
      return TRUE;
   }
   else if (stricmp( szToken, "-fce" ) == 0)
   {
      HMainInterface *pMI = GetHMainInterface();
      const char *szNextToken = tokenlist.GetNext();

      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute AddVertex command, no prev edge provided.");

      String error;
      m_pPreviousContinuity = (HEdge *)pMI->DescriptionToObject( szNextToken, &error);
      if (!m_pPreviousContinuity)
      {
         String strMsg("Unable to execute AddVertex command, Reason: %1.");
         strMsg.Replace("%1", error);
         THROW_FATAL_ERROR(strMsg);
      }
      return TRUE;
   }
   else if (stricmp( szToken, "-lce" ) == 0)
   {
      HMainInterface *pMI = GetHMainInterface();
      const char *szNextToken = tokenlist.GetNext();

      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute AddVertex command, no next edge provided.");

      String error;
      m_pNextContinuity = (HEdge *)pMI->DescriptionToObject( szNextToken, &error);
      if (!m_pNextContinuity)
      {
         String strMsg("Unable to execute AddVertex command, Reason: %1.");
         strMsg.Replace("%1", error);
         THROW_FATAL_ERROR(strMsg);
      }
      return TRUE;
   }
   else if (stricmp( szToken, "-nf" ) == 0)
   {
      m_bFindFaces = FALSE;
      return TRUE;
   }
   else if (stricmp( szToken, "-al" ) == 0)
   {
      m_bAutoLoop = TRUE;
      return TRUE;
   }
   else if (stricmp( szToken, "-e" ) == 0)
   {
      HMainInterface *pMI = GetHMainInterface();
      const char *szNextToken = tokenlist.GetNext();

      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute AddVertex command, no edge provided.");

      String error;
      HEdge *pEdge = (HEdge *)pMI->DescriptionToObject( szNextToken, &error);
      if (!pEdge)
      {
         String strMsg("Unable to execute AddVertex command, Reason: %1.");
         strMsg.Replace("%1", error);
         THROW_FATAL_ERROR(strMsg);
      }

      szNextToken = tokenlist.GetNext();
      if (!szNextToken)
      {
         String strMsg("Unable to execute AddVertex command, no pos along edge %1 provided.");
         strMsg.Replace("%1", (String)pEdge->GetID());
         THROW_FATAL_ERROR(strMsg);
      }

      float fPos = (float)atof( szNextToken );
      if (fPos > 1.0f || fPos < 0.0f)
      {
         String strMsg("Unable to execute AddVertex command, position along edge %1, was %2.  Position must be between 0 and 1.");
         strMsg.Replace("%1", (String)pEdge->GetID());
         strMsg.Replace("%2", (String)fPos);
         THROW_FATAL_ERROR(strMsg);
      }

      NewVertexAlongEdgeParameter *pNewVertexParameter = SNEW NewVertexAlongEdgeParameter( pEdge, fPos );
      m_parameterlist.AddTail( pNewVertexParameter );
      return TRUE;
   }
   if (stricmp( szToken, "-v" ) == 0)
   {
      HMainInterface *pMI = GetHMainInterface();
      const char *szNextToken = tokenlist.GetNext();

      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute AddVertex command, no vertex provided.");

      String error;
      HVertex *pVertex = (HVertex *)pMI->DescriptionToObject( szNextToken, &error);
      if (!pVertex)
      {
         String strMsg("Unable to execute AddVertex command, Reason: %1.");
         strMsg.Replace("%1", error);
         THROW_FATAL_ERROR(strMsg);
      }

      UseVertexParameter *pUseVertexParameter = SNEW UseVertexParameter( pVertex );
      m_parameterlist.AddTail( pUseVertexParameter );
      return TRUE;
   }
   else if (stricmp( szToken, "-p" ) == 0)
   {
      Vector vPos;

      for (int i=0; i<3; i++)
      {
         const char *szNextToken = tokenlist.GetNext();
         if (!szNextToken)
            THROW_FATAL_ERROR("Unable to execute SplitPoly command, incomplete or no position provided.");

         vPos[i] = (float)atof( szNextToken );
      }

      HMainInterface *pMI = GetHMainInterface();
      HPolyModel *pModel = (HPolyModel *)pMI->GetCurrentCommandObject();
      ASSERT(pModel && pModel->IsKindOf( SOT_PolyModel ));

      NewVertexAtPosParameter *pNewVertexParameter = SNEW NewVertexAtPosParameter( pModel, vPos );
      m_parameterlist.AddTail( pNewVertexParameter );
      return TRUE;
   }

   return FALSE;
}

UndoType AddVertexCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

// Dan you may want to reevaluate this stuff. You are using a VertexData in a map. That may not be necessary anymore.
BOOL AddVertexCommand::OnDo(StringList &strResults)
{
   if (m_selectiontype != ST_NONE)
      if (!SelectBaseCommand::OnDo(strResults))
         return FALSE;

   HSnipeObjectList *pVertices = HSnipeObjectList::New();
   HMainInterface *pMI = GetHMainInterface();

   for (POSITION pos = m_parameterlist.GetHeadPosition(); pos; )
   {
      NewVertexParameter *pNewVertexParameter = m_parameterlist.GetNext( pos );

      HVertex::HDeleteInfo *pVertexDeleteInfo;
      HVertex *pNewVertex;
      
      pNewVertexParameter->AddVertex(pNewVertex, pVertexDeleteInfo);
      ASSERT(pNewVertex);
      pVertices->AddTail( pNewVertex );

      if (pVertexDeleteInfo)
         m_newvertexdeleteinfos.AddTail( pVertexDeleteInfo );
   }

   if (m_bAutoLoop && pVertices->GetSize() > 2)
   {
      HVertex *pFirstVertex = (HVertex *)pVertices->GetHead(); 
      HVertex *pLastVertex = (HVertex *)pVertices->GetTail();

      for (UINT i=0; i < pFirstVertex->GetNumSurroundingPolys(); i++)
      {
         HPoly *pPoly = pFirstVertex->GetSurroundingPolyAt(i);
         if (pPoly->IsInFace(pLastVertex))
         {
            pVertices->AddTail(pFirstVertex);
            m_pPreviousContinuity = HEDGE_FINDCONTINUOUS;
            m_pNextContinuity = HEDGE_FINDCONTINUOUS;
            break;
         }
      }
   }

   HPolyModel *pModel = (HPolyModel *)GetHMainInterface()->GetCurrentCommandObject();
   m_nNumNewEdgeDeleteInfos = pModel->AddEdges( pVertices, m_pNewEdgeDeleteInfos, m_pPreviousContinuity, m_pNextContinuity );

   if (m_nNumNewEdgeDeleteInfos)
   {
      for ( int i=0; i<m_nNumNewEdgeDeleteInfos; i++ )
      {
         HDeleteFaceInfo **pDeleteFaceInfos = NULL;

         HEdge::HDeleteInfo *pEdgeDeleteInfo = m_pNewEdgeDeleteInfos[i];
         HEdge *pEdge = (HEdge *)pEdgeDeleteInfo->GetSnipeObject();
         int nFacesAdded = pEdge->FindAndCreateFaces( pDeleteFaceInfos );
         if (nFacesAdded)
         {
            for (int j=0; j<nFacesAdded; j++)
               m_newdeletefaceinfos.AddTail( pDeleteFaceInfos[j]);

            delete [] pDeleteFaceInfos;
         }
      }
   }

   for (POSITION pos = pVertices->GetHeadPosition(); pos; )
   {
      HVertex *pVertex = (HVertex *)pVertices->GetNext(pos);

      String strVertex;
      pMI->ObjectToDescription( pVertex, strVertex );
      strResults.AddTail( strVertex );
   }

   if (m_selectiontype == ST_ALL)
   {
      m_pNewSelectedVerts = pVertices;
      pVertices = NULL; // Steal the pointer for selectionlist
      pMI->GetCurrentSelectionList()->SetTo(m_pNewSelectedVerts);
   }
   else if (m_selectiontype == ST_LAST)
   {
      m_pNewSelectedVerts = HSnipeObjectList::New();

      HVertex *pLastVertex = NULL;
      HVertex *pPrevVertex = NULL;

      POSITION pos = pVertices->GetTailPosition();
      if (pos)
         pLastVertex = (HVertex *)pVertices->GetPrev(pos);

      if (pos)
         pPrevVertex = (HVertex *)pVertices->GetPrev(pos);

      if (pPrevVertex)
         m_pNearEdge = HEdge::FindEdge( pPrevVertex, pLastVertex );

      m_pNewSelectedVerts->AddTail(pLastVertex);
      pMI->GetCurrentSelectionList()->SetTo(m_pNewSelectedVerts);

      m_pFocusVertex = pLastVertex;
      HSelectionList::SetFocusObject(m_pFocusVertex, m_pNearEdge);
   }

   if (pVertices)
      delete pVertices;

   for (POSITION pos = m_parameterlist.GetHeadPosition(); pos; ) // These are no longer needed
      delete m_parameterlist.GetNext(pos);

   return TRUE;
}

void AddVertexCommand::OnUndo()
{
   if (m_selectiontype != ST_NONE)
      SelectBaseCommand::OnUndo();

   for (POSITION pos = m_newdeletefaceinfos.GetTailPosition(); pos; )
   {
      HDeleteFaceInfo *pDeleteFaceInfo = m_newdeletefaceinfos.GetPrev( pos );
      pDeleteFaceInfo->Delete();
   }

   for ( int i=m_nNumNewEdgeDeleteInfos-1; i>=0; i-- )
   {
      HEdge::HDeleteInfo *pEdgeDeleteInfo = m_pNewEdgeDeleteInfos[i];
      pEdgeDeleteInfo->Delete();
   }

   for (POSITION pos = m_newvertexdeleteinfos.GetTailPosition(); pos; )
   {
      HVertex::HDeleteInfo *pVertexDeleteInfo = m_newvertexdeleteinfos.GetPrev( pos );
      pVertexDeleteInfo->Delete();
   }
}

void AddVertexCommand::OnRedo()
{
   for ( POSITION pos = m_newvertexdeleteinfos.GetHeadPosition(); pos; )
   {
      HVertex::HDeleteInfo *pVertexDeleteInfo = m_newvertexdeleteinfos.GetNext( pos );
      pVertexDeleteInfo->UnDelete();
   }

   for ( int i=0; i<m_nNumNewEdgeDeleteInfos; i++ )
   {
      HEdge::HDeleteInfo *pEdgeDeleteInfo = m_pNewEdgeDeleteInfos[i];
      pEdgeDeleteInfo->UnDelete();
   }

   for (POSITION pos = m_newdeletefaceinfos.GetHeadPosition(); pos; )
   {
      HDeleteFaceInfo *pDeleteFaceInfo = m_newdeletefaceinfos.GetNext( pos );
      pDeleteFaceInfo->UnDelete();
   }

   if (m_pNewEdgeDeleteInfos)
   {
      GetHMainInterface()->GetCurrentSelectionList()->SetTo(m_pNewSelectedVerts);
      HSelectionList::SetFocusObject(m_pFocusVertex, m_pNearEdge);
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////
void NewVertexAlongEdgeParameter::AddVertex( HVertex *&pOutVertex, HVertex::HDeleteInfo *&pOutVertexDeleteInfo )
{
   if (m_fPos == 0.0f)
   {
      pOutVertexDeleteInfo = NULL;
      pOutVertex = m_pEdge->GetFirstVertex();
      return;
   }
   else if (m_fPos == 1.0f)
   {
      pOutVertexDeleteInfo = NULL;
      pOutVertex = m_pEdge->GetSecondVertex();
   }
   else
   {
      pOutVertexDeleteInfo = m_pEdge->SplitEdge( m_fPos );
      ASSERT( pOutVertexDeleteInfo );

      pOutVertex = (HVertex *)pOutVertexDeleteInfo->GetSnipeObject();
   }
}

void NewVertexAtPosParameter::AddVertex( HVertex *&pOutVertex, HVertex::HDeleteInfo *&pOutVertexDeleteInfo )
{
   pOutVertex = m_pModel->AddVertex( m_vPos, &pOutVertexDeleteInfo, TRUE );
}