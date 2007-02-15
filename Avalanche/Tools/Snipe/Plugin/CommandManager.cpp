//
#include "stdafx.h"
#include "CommandManager.h"
#include "SnipePlugin.h"

CommandManager::CommandManager()
{
}

// We don't need to delete the entries, the SnipePlugin will 
// do it for us since it points to the same entries
CommandManager::~CommandManager()
{
}

CommandEntry *CommandManager::FindCommand( const String &strPluginName, const String &strClassName )
{
   for (POSITION pos = GetHeadPosition(); pos; ) {
      CommandEntry *pEntry = GetNext(pos);
      if (pEntry->GetClassName() == strClassName && pEntry->GetSnipePlugin()->GetName() == strPluginName ) {
         if (pEntry->IsValid())
            return pEntry;
         else {
            try {
               Validate(pEntry); // this could remove pEntry from the CommandManager
            }
            catch(const String &msg) {
               g_outputWindow << msg << "\n";
               return NULL;
            }
            return pEntry;
         }
      }
   }
   return NULL;
}

BOOL CommandManager::RemoveCommand( CommandEntry *pEntry )
{
   for (POSITION pos = GetHeadPosition(); pos; ) {
      POSITION oldpos = pos;
      CommandEntry *pMatch = GetNext(pos);
      if (pMatch == pEntry) {
         RemoveAt( oldpos );
         return TRUE;
      }
   }
   return FALSE;
}

void CommandEntry::Validate()
{
   //Validate function pointers
   if (GetCreateCommandCallback()==NULL)
      THROW_FATAL_ERROR("Invalid Create Command Callback");

   if (GetDestroyCommandCallback()==NULL)
      THROW_FATAL_ERROR("Invalid Destroy Command Callback");

   if (GetDoCommandCallback()==NULL)
      THROW_FATAL_ERROR("Invalid Do Command Callback");

   if (GetUndoCommandCallback()==NULL)
      THROW_FATAL_ERROR("Invalid Undo Command Callback");
}

void CommandManager::Validate(CommandEntry *pEntry)
{
   ASSERT(pEntry->IsValid()==FALSE);

   if (pEntry->IsValidating()) { // Circular dependencies
      RemoveCommand(pEntry); // it is still in the SnipePlugin, but who cares.
      SnipePlugin *pPlugin = pEntry->GetSnipePlugin();
      String msg("Circular dependencies in Command (%1 : %2).");
      msg.Replace("%1", pPlugin->GetName());
      msg.Replace("%2", pEntry->GetClassName());
      THROW_FATAL_ERROR(msg);
   }
   pEntry->SetValidating(TRUE);

   //Validate function pointers
   try {
      pEntry->Validate();
   }
   catch (const String &strReason) {
      RemoveCommand(pEntry);
      THROW_FATAL_ERROR(pEntry->FormatError("Validate", strReason));
   }

   // Validate Dependencies through InitCallback
   try {
      if (pEntry->GetInitHelperCallback())
         pEntry->GetInitHelperCallback()(pEntry->GetPluginEntryHelper(), this);
   }
   catch(const String &strReason) {
      RemoveCommand(pEntry);
      THROW_FATAL_ERROR(pEntry->FormatError("Init", strReason));
   }
   catch(...) {
      RemoveCommand(pEntry);
      THROW_FATAL_ERROR(pEntry->FormatError("Init", "Unknown"));
   }
   pEntry->SetValid(TRUE);
   pEntry->SetValidating(FALSE);
}

void CommandManager::ValidateAll()
{
   for (POSITION pos = GetHeadPosition(); pos; ) {
      CommandEntry *pEntry = GetNext(pos);
      if (!pEntry->IsValid()) {
         try {
            Validate(pEntry); // This could remove pEntry from the CommandManager
         }
         catch(const String &msg) {
            g_outputWindow << msg << "\n";
         }
      }
   }
}