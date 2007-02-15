//

#include "stdafx.h"
#include "SelectRingCommand.h"
#include "HSelectionList.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "HPatchEdge.h"
#include "HPoly.h"
#include "HPatchVertex.h"
#include "../Resource.h"

CommandInstance *SelectRingCommandPlugin::CreateCommandInstance() { return SNEW SelectRingCommand(this); }

const char *SelectRingCommandPlugin::GetCLIName() const { return "SelectRing"; }; // Name used in scripts and commandline
const char *SelectRingCommandPlugin::GetTitleName() const { return "Select Ring"; }

UINT SelectRingCommandPlugin::GetBitmapID() const { return IDB_SELECTRING; }

String SelectRingCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Selects Ring of edges.";
   strHelp += (String) "  " + GetCLIName() + " EDGE_NAME [EDGE_NAME...]\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " Edge(1). This will select a ring of edges starting with Edge(1).\n";
   strHelp += (String) "    " + GetCLIName() + " Edge(1) Edge(5). This will select a two ring of edges, one starting with Edge(1) and the other with Edge(5).\n";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

BOOL SelectRingCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{

   if (SelectBaseCommand::ParseToken( szToken, tokenlist ))
      return TRUE;

   String strError;
   HMainInterface *pMainInterface = GetHMainInterface();
   HSnipeObject *pObject = pMainInterface->DescriptionToObject( szToken, &strError );
   if (!pObject)
   {
      String strMsg("Error in SelectRing, Reason: %1.");
      strMsg.Replace("%1", strError);

      THROW_FATAL_ERROR(strMsg);
   }

   if (pObject->IsKindOf(SOT_Edge))
      m_edgelist.AddTail((HEdge *)pObject);
   else
   {
      String strMsg("Error in SelectRing, %1 is not a face.");
      strMsg.Replace("%1", szToken);
      THROW_FATAL_ERROR(strMsg);
   }

   return TRUE;
}

BOOL SelectRingCommand::OnValidate( String &strOutError )
{
   if (m_edgelist.IsEmpty())
   {
      // Lets use the current selectionlist
      HMainInterface *pMainInterface = GetHMainInterface();
      HSelectionList *pSelectionList = pMainInterface->GetCurrentSelectionList();

      for (POSITION pos = pSelectionList->GetHeadPosition(); pos; )
      {
         HSnipeObject *pObject = pSelectionList->GetNext(pos);
         if (pObject->IsKindOf(SOT_Edge))
            m_edgelist.AddTail((HEdge *)pObject);
      }
   }

   if (m_edgelist.IsEmpty())
   {
      strOutError = "Unable to execute SelectRing command, no edge provided.";
      return FALSE;
   }

   return TRUE;
}

BOOL SelectRingCommand::OnDo(StringList &strResults)
{
   if (!SelectBaseCommand::OnDo(strResults))
      return FALSE;

   HSnipeObjectList *pList = HSnipeObjectList::New();
   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();

   for (POSITION pos = m_edgelist.GetHeadPosition(); pos;)
   {
      HEdge *pEdge = m_edgelist.GetNext(pos);

      if (pEdge->GetFirstPoly())
         SelectRing(pEdge->GetFirstPoly(), pEdge, pList);

      if (pEdge->GetSecondPoly())
         SelectRing(pEdge->GetSecondPoly(), pEdge, pList);
   }

   if (!pList->IsEmpty())
      pSelectionList->Add( pList );

   delete pList;

   return TRUE;
}


void SelectRingCommand::SelectRing( HPoly *pPoly, HEdge *pStartEdge, HSnipeObjectList *pList )
{
   HEdge *pEdge = pStartEdge;
   do 
   {
      pList->AddTail(pEdge);
      pEdge = pEdge->GetOppositeEdge(pPoly);
   } while(pEdge && pEdge!=pStartEdge);
}

