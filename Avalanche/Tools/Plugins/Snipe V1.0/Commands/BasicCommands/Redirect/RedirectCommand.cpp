//

#include "stdafx.h"
#include "RedirectCommand.h"
#include "HMainInterface.h"
#include "HPatchVertex.h"
#include "../resource.h"

CommandInstance *RedirectCommandPlugin::CreateCommandInstance() { return SNEW RedirectCommand(this); }

const char *RedirectCommandPlugin::GetCLIName() const { return "Redirect"; }; // Name used in scripts and commandline
const char *RedirectCommandPlugin::GetTitleName() const { return "Redirect"; }

UINT RedirectCommandPlugin::GetBitmapID() const { return IDB_REDIRECT; }

String RedirectCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "This command can be used to break a spline into two splines that are connected, or connect two splines into one spline.\n";
   strHelp = "To connect two spline edges together thus redirecting the continuity you must provide a FROM edge and a TO edge in the parameters.\n";
   strHelp = "To connect break a single spline into two splines thus terminating the continuity you must provide a FROM edge and a TERMINATING vertex in the parameters.\n";
   strHelp += (String) "  " + GetCLIName() + " -From EDGE_NAME [-To EDGE_NAME] [-Term VERTEX_NAME]\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " -From Edge(1) -To Edge(2). This will connect splines of edge(1) and edge(2) making one continuous spline.\n";
   strHelp += (String) "    " + GetCLIName() + " -From Edge(1) -Term Vertex(1). This will split the spline of edge(1) at vertex(1).\n";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

RedirectCommand::~RedirectCommand()
{
   if (m_pRedirectInfo)
      delete m_pRedirectInfo;
}

BOOL RedirectCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{

   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-from" ) == 0)
   {
      const char *szNextToken = tokenlist.GetNext();

      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute Redirect command, no from edge provided.");

      String strError;
      m_pFromEdge = (HPatchEdge *)GetHMainInterface()->DescriptionToObject(szNextToken, &strError);

      if (!m_pFromEdge)
      {
         String strMsg("Unable to execute Redirect command, invalid from edge %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);
      }
         
      if (!m_pFromEdge->IsKindOf(SOT_PatchEdge))
      {
         String strMsg("Unable to execute Redirect command, invalid from edge %1.");
         strMsg.Replace("%1", szNextToken);
         THROW_FATAL_ERROR(strMsg);
      }
      
      return TRUE;
   }
   else if (stricmp( szToken, "-term" ) == 0)
   {
      const char *szNextToken = tokenlist.GetNext();

      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute Redirect command, no Term vertex provided.");

      String strError;
      m_pTermVertex = (HPatchVertex *)GetHMainInterface()->DescriptionToObject(szNextToken, &strError);

      if (!m_pTermVertex)
      {
         String strMsg("Unable to execute Redirect command, invalid Term vertex %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);
      }

      if (!m_pTermVertex->IsKindOf(SOT_PatchVertex))
      {
         String strMsg("Unable to execute Redirect command, invalid Term vertex %1.");
         strMsg.Replace("%1", szNextToken);
         THROW_FATAL_ERROR(strMsg);
      }

      return TRUE;
   }
   else if (stricmp( szToken, "-to" ) == 0)
   {
      const char *szNextToken = tokenlist.GetNext();

      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute Redirect command, no redirection to edge provided.");

      String strError;
      m_pToEdge = (HPatchEdge *)GetHMainInterface()->DescriptionToObject(szNextToken, &strError);

      if (!m_pToEdge)
      {
         String strMsg("Unable to execute Redirect command, invalid To edge %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);
      }

      if (!m_pToEdge->IsKindOf(SOT_PatchEdge))
      {
         String strMsg("Unable to execute Redirect command, invalid To edge %1.");
         strMsg.Replace("%1", szNextToken);
         THROW_FATAL_ERROR(strMsg);
      }

      return TRUE;
   }

   return FALSE;
}

BOOL RedirectCommand::OnValidate( String &strOutError )
{
   if (!m_pFromEdge)
   {
      strOutError = "Unable to execute Redirect command, no FROM edge provided.";
      return FALSE;
   }

   if (!m_pTermVertex && !m_pToEdge)
   {
      strOutError = "Unable to execute Redirect command, no TERM vertex or TO edge provided.";
      return FALSE;
   }

   if (m_pTermVertex && m_pToEdge)
   {
      strOutError = "Unable to execute Redirect command, both TERM vertex and TO edge provided, only one should be.";
      return FALSE;
   }

   return TRUE;
}

UndoType RedirectCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL RedirectCommand::OnDo(StringList &strResults)
{
   String strError;
   if (m_pTermVertex)
      m_pRedirectInfo = m_pFromEdge->Redirect( m_pTermVertex, &strError );
   else
   {
      ASSERT (m_pToEdge );
      m_pRedirectInfo = m_pFromEdge->Redirect( m_pToEdge, &strError );
   }

   if (!strError.IsEmpty()) // Its possible in mirror mode that the mirror had an error, but the original did not
   {
      if (strError.IsEmpty())
         DisplayWarning( "Error in Redirect Command.");
      else
      {
         String strMsg = "Error in Redirect, Reason: %1.";
         strMsg.Replace("%1", strError);
         DisplayWarning( strMsg );
      }
   }

   return TRUE;
}

void RedirectCommand::OnUndo()
{
   if (m_pRedirectInfo)
      m_pRedirectInfo->Undo();
}

void RedirectCommand::OnRedo()
{
   if (m_pRedirectInfo)
      m_pRedirectInfo->Redo();
}
