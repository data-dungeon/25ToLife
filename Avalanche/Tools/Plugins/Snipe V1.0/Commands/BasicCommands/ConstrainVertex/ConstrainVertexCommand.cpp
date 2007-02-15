//

#include "stdafx.h"
#include "ConstrainVertexCommand.h"
#include "../resource.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "HComponent.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "HPatchVertex.h"
#include "MouseOn.h"
#include "HEdge.h"

CommandInstance *ConstrainVertexCommandPlugin::CreateCommandInstance() { return SNEW ConstrainVertexCommand(this); }

const char *ConstrainVertexCommandPlugin::GetCLIName() const { return "ConstrainVertex"; }; // Name used in scripts and commandline
const char *ConstrainVertexCommandPlugin::GetTitleName() const { return "ConstrainVertex"; }

UINT ConstrainVertexCommandPlugin::GetBitmapID() const { return IDB_CONSTRAINVERTEX; }

String ConstrainVertexCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Constrains or Unconstrains a vertex.  A contrained vertex automatically calculates its position and tangents based on the next and prev continuous vertices.\n";
   strHelp = "This command is only available on Bezier Spline Models.\n";
   strHelp += (String) "  " + GetCLIName() + " [-o OPTIONNAME] -v VERTEXNAME -v EDGENAME\n";
   strHelp += (String) "    -o TOGGLE  (Toggles On and Off the constraint, if no option is specified this is default)\n";
   strHelp += (String) "    -o CONSTRAIN  (Constrains vertex)\n";
   strHelp += (String) "    -o UNCONSTRAIN  (Unconstrains vertex)\n";
   strHelp += (String) "    -v VERTEXNAME (Vertex to constrain)\n";
   strHelp += (String) "    -e EDGENAME (Edge to constrain along)\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " -o TOGGLE -v Vertex(102) -e Edge(5)\n";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

ConstrainVertexCommand::~ConstrainVertexCommand()
{
}

BOOL ConstrainVertexCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-o" ) == 0)
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute ConstrainVertex command, invalid option provided.");
      if (strcmp(szNextToken, "TOGGLE")==0)
         m_nConstrainVertexOption = ConstrainVertexCommand::TOGGLE;
      else if (strcmp(szNextToken, "CONSTRAIN")==0)
         m_nConstrainVertexOption = ConstrainVertexCommand::CONSTRAIN;
      else if (strcmp(szNextToken, "UNCONSTRAIN")==0)
         m_nConstrainVertexOption = ConstrainVertexCommand::UNCONSTRAIN;
      else
      {
         String strMsg("Unable to execute ConstrainVertex Command, invalid option %1.");
         strMsg.Replace("%1", szNextToken );
         THROW_FATAL_ERROR(strMsg);
      }
      return TRUE;
   }
   else if (stricmp( szToken, "-v" ) == 0)
   {
      HMainInterface *pMI = GetHMainInterface();
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute ConstrainVertex command, no vertex provided.");
      String error;
      HSnipeObject *pSnipeObject = pMI->DescriptionToObject( szNextToken, &error);
      if (!pSnipeObject)
      {
         String strMsg("Unable to execute ConstrainVertex command, unable to find [%1].");
         strMsg.Replace("%1", szNextToken );
         THROW_FATAL_ERROR(strMsg);
      }
      if (!pSnipeObject->IsKindOf(SOT_PatchVertex))
         THROW_FATAL_ERROR("Unable to execute ConstrainVertex command, must be a Patch Model.");
      m_pVertex = (HPatchVertex *)pSnipeObject;
      return TRUE;
   }
   else if (stricmp( szToken, "-e" ) == 0)
   {
      HMainInterface *pMI = GetHMainInterface();
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute ConstrainVertex command, no edge provided.");
      String error;
      HSnipeObject *pSnipeObject = pMI->DescriptionToObject( szNextToken, &error);
      if (!pSnipeObject)
      {
         String strMsg("Unable to execute ConstrainVertex command, unable to find [%1].");
         strMsg.Replace("%1", szNextToken );
         THROW_FATAL_ERROR(strMsg);
      }
      if (!pSnipeObject->IsKindOf(SOT_PatchEdge))
         THROW_FATAL_ERROR("Unable to execute ConstrainVertex command, must be a Patch Model.");
      m_pEdge = (HPatchEdge *)pSnipeObject;
      return TRUE;
   }

   return FALSE;
}

BOOL ConstrainVertexCommand::OnValidate( String &strOutError )
{
   if (m_pVertex==NULL)
   {
      strOutError = "Unable to execute ConstrainVertex command, no vertex provided.";
      return FALSE;
   }

   if (m_pEdge==NULL)
   {
      strOutError = "Unable to execute ConstrainVertex command, no edge provided.";
      return FALSE;
   }

   if (!m_pVertex->IsConstrained() && !m_pVertex->CanConstrain(m_pEdge))
   {
      strOutError = "Unable to execute ConstrainVertex command, no continuous edges through vertex.";
      return FALSE;
   }

   return TRUE;
}

UndoType ConstrainVertexCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL ConstrainVertexCommand::OnDo(StringList &strResults)
{
   m_bWasConstrained = m_pVertex->IsConstrained();
   if (m_bWasConstrained)
   {
      HPatchEdge *pTemp;
      m_pVertex->GetConstrainedBetween( m_pOldEdge, pTemp );
   }
   else
      m_vPos = m_pVertex->GetPos();

   switch (m_nConstrainVertexOption)
   {
   case ConstrainVertexCommand::TOGGLE:
      m_pVertex->SetConstrained(m_pVertex->IsConstrained()?NULL:m_pEdge);
      break;
   case ConstrainVertexCommand::CONSTRAIN:
      m_pVertex->SetConstrained(m_pEdge);
      break;
   case ConstrainVertexCommand::UNCONSTRAIN:
      m_pVertex->SetConstrained(NULL);
      break;
   }

   return TRUE;
}

void ConstrainVertexCommand::OnUndo()
{
   if (m_pVertex->IsConstrained() == m_bWasConstrained)
      return;

   m_pVertex->SetConstrained(m_pOldEdge);

   if (!m_bWasConstrained)
      m_pVertex->SetPos(m_vPos);
}
