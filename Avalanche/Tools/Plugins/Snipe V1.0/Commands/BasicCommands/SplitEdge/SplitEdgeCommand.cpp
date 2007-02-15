//

#include "stdafx.h"
#include "SplitEdgeCommand.h"
#include "HMainInterface.h"
#include "HSelectionList.h"
#include "HPolyModel.h"
#include "../Resource.h"

CommandInstance *SplitEdgeCommandPlugin::CreateCommandInstance() { return SNEW SplitEdgeCommand(this); }

const char *SplitEdgeCommandPlugin::GetCLIName() const { return "SplitEdge"; }; // Name used in scripts and commandline
const char *SplitEdgeCommandPlugin::GetTitleName() const { return "SplitEdge"; }
UINT SplitEdgeCommandPlugin::GetBitmapID() const { return IDB_SPLITEDGE; }

String SplitEdgeCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp  = "Splits the edge(s) into a given number of segments.\n";
   strHelp += "If an no edges are provided then the selected objects will be used.";
   strHelp += (String) "    " + GetCLIName() + " -s NUMBER OF SEGMENTS [-e EDGENAME]...\n";
   strHelp += (String) "      -s (number of segments to split all edges.  Valid values are between 2 and 100 inclusively.  Default value is 2)\n";
   strHelp += (String) "      -e (EDGENAME - Edge to split, multiple edges can be privided.)\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + "\n";
   strHelp += (String) "    " + GetCLIName() + " -s 3\n";
   strHelp += (String) "    " + GetCLIName() + " -s 4 -e Edge(5) -e Edge(7)\n";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

UndoType SplitEdgeCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

SplitEdgeCommand::~SplitEdgeCommand()
{
   for (POSITION pos=m_DeleteInfoList.GetHeadPosition(); pos;)
      delete m_DeleteInfoList.GetNext(pos);

   if (m_pNewSelectionList)
      delete m_pNewSelectionList;

   if (m_pOldSelectionList)
      delete m_pOldSelectionList;
}

BOOL SplitEdgeCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-s" ) == 0)
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute SplitEdge command, no NUMBER OF SEGMENTS provided.");

      m_nSegments = atoi(szNextToken);
      return TRUE;
   }
   else if (stricmp( szToken, "-e") == 0)
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute SplitEdge command, no edge provided.");

      String strError;
      HEdge *pEdge = (HEdge *)GetHMainInterface()->DescriptionToObject(szNextToken, &strError);
      if (!pEdge || !pEdge->IsKindOf(SOT_Edge))
      {
         String strMsg("Unable to execute SplitEdge command, invalid edge %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);
      }

      m_edges.AddTail(pEdge);
      return TRUE;
   }

   return FALSE;
}

BOOL SplitEdgeCommand::OnValidate( String &strOutError )
{
   if (!CommandInstance::OnValidate(strOutError))
      return FALSE;

   if (m_edges.IsEmpty())
   {
      // Lets use the current selectionlist
      HMainInterface *pMainInterface = GetHMainInterface();
      HSelectionList *pSelectionList = pMainInterface->GetCurrentSelectionList();
      
      for (POSITION pos = pSelectionList->GetHeadPosition(); pos; )
      {
         HSnipeObject *pObject = pSelectionList->GetNext(pos);
         if (pObject->IsKindOf(SOT_Edge))
            m_edges.AddTail((HEdge *)pObject);
      }
   }

   if (m_edges.IsEmpty())
   {
      strOutError = "No edges provided.";
      return FALSE;
   }

   if (m_nSegments < 2 || m_nSegments > 100)
   {
      strOutError = "No number of segments must be between 2 and 100.";
      return FALSE;
   }

   return TRUE;
}

BOOL SplitEdgeCommand::OnDo(StringList &strResults)
{
   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();
   m_pOldSelectionList = pSelectionList->CreateSnipeObjectList();
   ASSERT(m_pOldSelectionList);

   m_pOldFocusObject = HSelectionList::GetFocusObject();
   m_pOldNearFocusObject = HSelectionList::GetNearestFocusObject();

   for (POSITION pos = m_edges.GetHeadPosition(); pos;)
   {
      HEdge *pEdge = m_edges.GetNext(pos);
      for (UINT i=m_nSegments; i > 1; i--)
      {
         float fPos = 1.0f / i;
         HEdge *pEdge0 = NULL;
         HEdge *pEdge1 = NULL;
         HVertex::HDeleteInfo *pDeleteInfo = pEdge->SplitEdge(fPos, &pEdge0, &pEdge1);
         if (pDeleteInfo)
         {
            ASSERT(pEdge);
            m_DeleteInfoList.AddTail(pDeleteInfo);
            pEdge = pEdge1;

            HVertex *pVertex = pEdge0->GetSharedVertex(pEdge1);
            ASSERT(pVertex);
            m_pNewSelectionList->AddTail(pVertex);
         }
      }
   }

   pSelectionList->SetTo(m_pNewSelectionList);
   HSelectionList::SetFocusObject(NULL, NULL);

   return TRUE;
}

void SplitEdgeCommand::OnUndo()
{
   for (POSITION pos = m_DeleteInfoList.GetTailPosition(); pos;)
   {
      HVertex::HDeleteInfo *pDeleteInfo = m_DeleteInfoList.GetPrev(pos);
      pDeleteInfo->Delete();
   }

   if (m_pOldSelectionList)
   {
      HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();
      pSelectionList->SetTo(m_pOldSelectionList);
      HSelectionList::SetFocusObject(m_pOldFocusObject, m_pOldNearFocusObject);
   }
}

void SplitEdgeCommand::OnRedo()
{
   for (POSITION pos = m_DeleteInfoList.GetHeadPosition(); pos;)
   {
      HVertex::HDeleteInfo *pDeleteInfo = m_DeleteInfoList.GetNext(pos);
      pDeleteInfo->UnDelete();
   }

   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();
   pSelectionList->SetTo(m_pNewSelectionList);
   HSelectionList::SetFocusObject(NULL, NULL);
}