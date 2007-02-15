//

#include "stdafx.h"
#include "CollapseCommand.h"
#include "HSelectionList.h"
#include "HMainInterface.h"
#include "../resource.h"

CommandInstance *WeldCommandPlugin::CreateCommandInstance() { return SNEW WeldCommand(this); }

const char *WeldCommandPlugin::GetCLIName() const { return "Weld"; }; // Name used in scripts and commandline
const char *WeldCommandPlugin::GetTitleName() const { return "Weld"; }

UINT WeldCommandPlugin::GetBitmapID() const { return IDB_COLLAPSE; }

String WeldCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Welds two vertices on a common edge together removing the edge in between.\n";
   strHelp += "Vertices must be listed in pairs followed by a percentage along the edge to place the welded vertex.\n";
   strHelp += "All textures are preserved.  Multiple sets of vertices and percentages are allowed.\n";
   strHelp += (String) "  " + GetCLIName() + " FROM_VERTEX_NAME TO_VERTEX_NAME [ FROM_VERTEX_NAME TO_VERTEX_NAME ]...\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " Vertex(1) Vertex(2) Vertex(3) Vertex(4). This will attach Vertex 1 to Vertex 2 and leave\n";
   strHelp += (String) "    the resulting welded vertex at Vertex 2's position, and Vertex 3 to Vertex 4 will be welded together and the welded vertex\n";
   strHelp += (String) "    will be Vertex 4 posistion.\n";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

WeldCommand::~WeldCommand()
{
   for (POSITION pos = m_WeldPairs.GetHeadPosition(); pos;)
   {
      WeldPair *pPair = m_WeldPairs.GetNext(pos);
      delete pPair;
   }
}

BOOL WeldCommand::ParseToken(const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   HMainInterface *pMI = GetHMainInterface();
   ASSERT(pMI);

   String strError;
   HSnipeObject *pFrom = pMI->DescriptionToObject( szToken, &strError );

   if (!pFrom)
   {
      String strMsg("Unable to execute Weld command, Reason: %1.");
      strMsg.Replace("%1", strError);
      THROW_FATAL_ERROR(strMsg);
   }

   const char *szNextToken = tokenlist.GetNext();
   if (!szNextToken)
      THROW_FATAL_ERROR("Unable to execute Weld Command, no [to] vertex provided.");

   HSnipeObject *pTo = pMI->DescriptionToObject( szNextToken, &strError );

   if (!pTo)
   {
      String strMsg("Unable to execute Weld command, Reason: %1.");
      strMsg.Replace("%1", strError);
      THROW_FATAL_ERROR(strMsg);
   }

   if ( !pFrom->IsKindOf(SOT_Vertex) || !pTo->IsKindOf(SOT_Vertex))
      THROW_FATAL_ERROR("Unable to execute Weld command, no vertices provided.");

   WeldPair *pPair = SNEW WeldPair( (HVertex *)pFrom, (HVertex *)pTo );
   m_WeldPairs.AddTail(pPair);

   return TRUE;
}

BOOL WeldCommand::OnValidate( String &strOutError )
{
   if (!CommandInstance::OnValidate( strOutError ))
      return FALSE;

   if (m_WeldPairs.IsEmpty())
   {
      strOutError = "Unable to execute Weld command, no pairs of vertices provided.";
      return FALSE;
   }

   return TRUE;
}

UndoType WeldCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL WeldCommand::OnDo(StringList &strResults)
{
   for (POSITION pos = m_WeldPairs.GetHeadPosition(); pos;)
   {
      POSITION oldpos = pos;
      WeldPair *pPair = m_WeldPairs.GetNext( pos );

      String strError;
      if (!pPair->Do(strError))
         m_WeldPairs.RemoveAt( oldpos );

      if (!strError.IsEmpty()) // Because in mirror mode the error could be set but the Do was valid
         DisplayWarning( strError );
   }

   return TRUE;
}

void WeldCommand::OnRedo()
{
   for (POSITION pos = m_WeldPairs.GetHeadPosition(); pos;)
   {
      WeldPair *pPair = m_WeldPairs.GetNext( pos );
      pPair->Redo();
   }
}

void WeldCommand::OnUndo()
{
   for (POSITION pos = m_WeldPairs.GetTailPosition(); pos;)
   {
      WeldPair *pPair = m_WeldPairs.GetPrev( pos );
      pPair->Undo();
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

WeldPair::~WeldPair()
{
   if (m_pWeldInfo)
      delete m_pWeldInfo;
}

BOOL WeldPair::Do( String &strError )
{
   m_pWeldInfo = m_pFrom->WeldTo( m_pTo, &strError );
   return m_pWeldInfo!=NULL;
}

void WeldPair::Undo()
{
   m_pWeldInfo->UnWeld();
}

void WeldPair::Redo()
{
   m_pWeldInfo->ReWeld();
}