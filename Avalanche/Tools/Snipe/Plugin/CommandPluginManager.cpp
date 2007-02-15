//
#include "stdafx.h"
#include "CommandPluginManager.h"
#include "SnipePlugin.h"
#include "CommandPlugin.h"

CommandPluginManager::CommandPluginManager()
{
}

// We don't need to delete the plugins, the SnipePlugin will 
// do it for us since it points to the same plugins
CommandPluginManager::~CommandPluginManager()
{
}

CommandPlugin *CommandPluginManager::FindCommandByClassName( const String &strPluginName, const String &strClassName )
{
   for (POSITION pos = m_list.GetHeadPosition(); pos; ) {
      CommandPlugin *pPlugin = (CommandPlugin *)m_list.GetNext(pos);
      if (pPlugin->GetClassName() == strClassName && pPlugin->GetSnipePlugin()->GetName() == strPluginName ) {
         if (pPlugin->IsValid())
            return pPlugin;
         else {
            try {
               Validate(pPlugin); // this could remove pPlugin from the CommandPluginManager
            }
            catch(const String &msg) {
               g_outputWindow << msg << "\n";
               return NULL;
            }
            return pPlugin;
         }
      }
   }
   return NULL;
}

CommandPlugin *CommandPluginManager::FindCommandByCLIName( const String &strPluginName, const String &strCLIName )
{
   for (POSITION pos = m_list.GetHeadPosition(); pos; ) {
      CommandPlugin *pPlugin = (CommandPlugin *)m_list.GetNext(pos);
      if (pPlugin->GetCLIName() == strCLIName && (strPluginName.IsEmpty() || pPlugin->GetSnipePlugin()->GetName() == strPluginName)) {
         if (pPlugin->IsValid())
            return pPlugin;
         else {
            try {
               Validate(pPlugin); // this could remove pPlugin from the CommandPluginManager
            }
            catch(const String &msg) {
               g_outputWindow << msg << "\n";
               return NULL;
            }
            return pPlugin;
         }
      }
   }
   return NULL;
}


BOOL CommandPluginManager::RemoveCommand( CommandPlugin *pPlugin )
{
   for (POSITION pos = m_list.GetHeadPosition(); pos; ) {
      POSITION oldpos = pos;
      CommandPlugin *pMatch = (CommandPlugin *)m_list.GetNext(pos);
      if (pMatch == pPlugin) {
         m_list.RemoveAt( oldpos );
         return TRUE;
      }
   }
   return FALSE;
}

void CommandPluginManager::Validate(CommandPlugin *pPlugin)
{
   ASSERT(pPlugin->IsValid()==FALSE);

   if (pPlugin->IsValidating()) { // Circular dependencies
      RemoveCommand(pPlugin); // it is still in the SnipePlugin, but who cares.
      SnipePlugin *pSnipePlugin = pPlugin->GetSnipePlugin();
      String msg("Circular dependencies in Command (%1 : %2).");
      msg.Replace("%1", pSnipePlugin->GetName());
      msg.Replace("%2", pPlugin->GetClassName());
      THROW_FATAL_ERROR(msg);
   }
   pPlugin->SetValidating(TRUE);

   //Validate function pointers
   try {
      pPlugin->Validate();
   }
   catch (const String &strReason) {
      RemoveCommand(pPlugin);
      RETHROW_FATAL_ERROR(pPlugin->FormatError("Validate", strReason));
   }

   // Validate Dependencies through InitCallback
   try {
      if (pPlugin->GetInitCommandPluginCallback())
         pPlugin->GetInitCommandPluginCallback()(pPlugin->GetPluginData(), this);
   }
   catch(const String &strReason) {
      RemoveCommand(pPlugin);
      RETHROW_FATAL_ERROR(pPlugin->FormatError("Init", strReason));
   }
   catch(...) {
      RemoveCommand(pPlugin);
      THROW_FATAL_ERROR(pPlugin->FormatError("Init", "Unknown"));
   }
   pPlugin->SetValid(TRUE);
   pPlugin->SetValidating(FALSE);
}

void CommandPluginManager::ValidateAll()
{
   for (POSITION pos = m_list.GetHeadPosition(); pos; ) {
      CommandPlugin *pPlugin = (CommandPlugin *)m_list.GetNext(pos);
      if (!pPlugin->IsValid()) {
         try {
            Validate(pPlugin); // This could remove pPlugin from the CommandPluginManager
         }
         catch(const String &msg) {
            g_outputWindow << msg << "\n";
         }
      }
   }
}

void CommandPluginManager::InitPlugins()
{
   ValidateAll();
}

