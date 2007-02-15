//

#include "stdafx.h"
#include "SaveSelectionCommand.h"
#include "HSelection.h"
#include "HSelectionList.h"
#include "HSnipeObjectList.h"
#include "HMainInterface.h"
#include "../Resource.h"

CommandInstance *SaveSelectionCommandPlugin::CreateCommandInstance() { return SNEW SaveSelectionCommand(this); }

const char *SaveSelectionCommandPlugin::GetCLIName() const { return "SaveSelection"; }; // Name used in scripts and commandline
const char *SaveSelectionCommandPlugin::GetTitleName() const { return "Save Selection"; }

UINT SaveSelectionCommandPlugin::GetBitmapID() const { return IDB_SELECT; }

String SaveSelectionCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Saves the current selection for recall at a later time.";
   strHelp += (String) "  " + GetCLIName() + " [-o OPTIONNAME] -name SELECTIONNAME\n";
   strHelp += (String) "    -o NEW (Creates a new selection object and stores current selection in it. This option is default.)\n";
   strHelp += (String) "    -o REPLACE (Replace stored selection with current selection.)\n";
   strHelp += (String) "    -o ADD (Add current selection to stored selection.)\n";
   strHelp += (String) "    -o SUBTRACT (Subtract current selection from stored selection.)\n";
   strHelp += (String) "    -o TOGGLE (Toggle selects current selection with stored selection.)\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " -o New -name MySelection";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

SaveSelectionCommand::SaveSelectionCommand(HCommandPluginHelper *pPlugin) :
   CommandInstance(pPlugin)
{
   m_pSelection = NULL;
   m_operation = OP_NEW;
   m_pSelectionDeleteInfo = NULL;
}

SaveSelectionCommand::~SaveSelectionCommand()
{
   if (m_pSelectionDeleteInfo)
   {
      delete m_pSelection;
      delete m_pSelectionDeleteInfo;
   }
}

BOOL SaveSelectionCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-name" ) == 0)
   {
      m_strName = tokenlist.GetNext();
      m_strName.RemoveChar('\"');
      return TRUE;
   }
   if (stricmp( szToken, "-o" ) == 0)
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute SaveSelection Command, invalid option provided.");
      if (strcmp(szNextToken, "NEW")==0)
         m_operation = OP_NEW;
      else if (strcmp(szNextToken, "REPLACE")==0)
         m_operation = OP_REPLACE;
      else if (strcmp(szNextToken, "ADD")==0)
         m_operation = OP_ADD;
      else if (strcmp(szNextToken, "SUBTRACT")==0)
         m_operation = OP_SUBTRACT;
      else if (strcmp(szNextToken, "TOGGLE")==0)
         m_operation = OP_TOGGLE;
      else
      {
         String strMsg("Unable to execute SaveSelection Command, invalid option %1.");
         strMsg.Replace("%1", szNextToken );
         THROW_FATAL_ERROR(strMsg);
      }
      return TRUE;
   }

   return FALSE;
}

BOOL SaveSelectionCommand::OnValidate( String &strOutError )
{
   if (m_strName.IsEmpty())
      m_strName = "Untitled";

   HSnipeObject *pCommandObject = GetHMainInterface()->GetCurrentCommandObject();

   if (m_operation == OP_NEW)
   {
      m_pSelection = pCommandObject->AddSelection();
      if (!m_pSelection)
      {
         strOutError = "Error in SaveSelection command: Couldn't create new selection object.";
         return FALSE;
      }

      m_pSelection->SetName( m_strName );
   }
   else
   {
      HSelectionContainer *pSelectionContainer = pCommandObject->GetSelectionContainer();
      if (pSelectionContainer)
         m_pSelection = pSelectionContainer->FindSelection( m_strName );

      if (!m_pSelection)
      {
         strOutError = "Error in %1 command: Couldn't find \"%2\" selection.";
         strOutError.Replace( "%1", GetCommandPluginHelper()->GetCLIName() );
         strOutError.Replace( "%2", m_strName );
         return FALSE;
      }
   }

   return TRUE;
}

UndoType SaveSelectionCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL SaveSelectionCommand::OnDo(StringList &strResults)
{
   if (m_operation != OP_NEW)
      m_strOldDescription = m_pSelection->GetSelectionDescription();

   if (m_operation == OP_REPLACE)
      m_pSelection->RemoveAll();

   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();

   if (m_operation == OP_NEW || m_operation == OP_REPLACE || m_operation == OP_ADD)
   {
      HSnipeObjectList *pList = pSelectionList->CreateSnipeObjectList();
      m_pSelection->AddObjects( pList );
      delete pList;
   }
   else
   {
      for (POSITION pos=pSelectionList->GetHeadPosition(); pos; )
      {
         HSnipeObject *pSnipeObject = pSelectionList->GetNext( pos );
         switch (m_operation)
         {
         case OP_SUBTRACT:
            m_pSelection->RemoveObject( pSnipeObject );
            break;
         case OP_TOGGLE:
            m_pSelection->ToggleObject( pSnipeObject );
            break;
         }
      }
   }

   m_strNewDescription = m_pSelection->GetSelectionDescription();

   return TRUE;
}

void SaveSelectionCommand::OnRedo()
{
   if (m_operation == OP_NEW)
   {
      m_pSelectionDeleteInfo->UnDelete();
      delete m_pSelectionDeleteInfo;
      m_pSelectionDeleteInfo = NULL;
   }
   else
      m_pSelection->SetSelectionDescription( m_strNewDescription );
}

void SaveSelectionCommand::OnUndo()
{
   if (m_operation == OP_NEW)
   {
      // remove selection from hierarchy
      String strError;
      m_pSelection->Delete( m_pSelectionDeleteInfo, strError );
   }
   else
      m_pSelection->SetSelectionDescription( m_strOldDescription );
}
