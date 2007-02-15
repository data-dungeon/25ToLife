//

#include "stdafx.h"
#include "ParentCommand.h"
#include "../resource.h"

CommandInstance *ParentCommandPlugin::CreateCommandInstance() { return SNEW ParentCommand(this); }

const char *ParentCommandPlugin::GetCLIName() const { return "Parent"; }; // Name used in scripts and commandline
const char *ParentCommandPlugin::GetTitleName() const { return "Parent"; }

UINT ParentCommandPlugin::GetBitmapID() const { return IDB_DEFAULT; }

String ParentCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Parent hierarchal objects.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

ParentCommand::~ParentCommand()
{
}

BOOL ParentCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   HMainInterface *pMI = GetHMainInterface();

   if (stricmp( szToken, "-p" ) == 0)// bone parent
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Missing argument.");

      String strError;
      HSnipeObject *pSnipeObject = pMI->DescriptionToObject( szNextToken, &strError );
      if (!pSnipeObject)
      {
         String strMsg("Invalid argument, Reason: %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);
      }
      m_pParent = (HHierObject*)pSnipeObject;

      return TRUE;
   }
   else if (stricmp( szToken, "-c" ) == 0)// bone parent
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Missing argument.");

      String strError;
      HSnipeObject *pSnipeObject = pMI->DescriptionToObject( szNextToken, &strError );
      if (!pSnipeObject)
      {
         String strMsg("Invalid argument, Reason: %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);

      }
      m_pChild = (HHierObject*)pSnipeObject;

      return TRUE;
   }
  
   return FALSE;
}

BOOL ParentCommand::OnValidate( String &strOutError )
{
   if (!CommandInstance::OnValidate( strOutError ))
      return FALSE;

   if (!m_pParent->IsKindOf(SOT_HierObject))
      return FALSE;

   if (!m_pChild->IsKindOf(SOT_HierObject))
      return FALSE;

   return TRUE;
}

UndoType ParentCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL ParentCommand::OnDo(StringList &strResults)
{
   m_pParentInfo = m_pChild->Parent(m_pParent);
   return TRUE;
}

void ParentCommand::OnRedo()
{
   m_pParentInfo->ReParent();
}

void ParentCommand::OnUndo()
{
   m_pParentInfo->UnParent();
}

