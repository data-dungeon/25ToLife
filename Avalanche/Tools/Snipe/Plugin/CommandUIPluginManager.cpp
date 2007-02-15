//
#include "stdafx.h"
#include "resource.h"
#include "CommandUIPlugin.h"
#include "CommandUIPluginManager.h"
#include "SnipePlugin.h"
#include "../GUI/MainFrm.h"
#include "RegistryIDMap.h"

CArray<CommandUIPluginList, CommandUIPluginList *> CommandUIPluginManager::m_sotarray;
CArray<CommandUIPluginList, CommandUIPluginList *> CommandUIPluginManager::m_svtarray;
CArray<CommandUIPluginList, CommandUIPluginList *> CommandUIPluginManager::m_sdtarray;
CommandUIPluginList CommandUIPluginManager::m_mainframelist;
CommandUIPluginList CommandUIPluginManager::m_allcommanduilist;
BOOL CommandUIPluginManager::m_bHasInit;

CommandUIPluginManager::CommandUIPluginManager() 
{
   if (!m_bHasInit) {
      m_sotarray.SetSize(SOT_NUMOBJECTS);
      m_svtarray.SetSize(SVT_NUMVIEWS);
      m_sdtarray.SetSize(SDT_NUMDOCS);
      m_bHasInit = TRUE;
   }
}

// We don't need to delete the plugins, the SnipePlugin will 
// do it for us since it points to the same plugins
CommandUIPluginManager::~CommandUIPluginManager()
{
}

void CommandUIPluginManager::AddPlugin(Plugin *pPlugin)
{
   PluginManager::AddPlugin(pPlugin);

   CommandUIPlugin *pCommandUIPlugin = (CommandUIPlugin *)pPlugin;
   m_allcommanduilist.AddTail( pCommandUIPlugin ); // static list

   try {
      pCommandUIPlugin->Validate();
   }
   catch(const String &strReason) {
      g_outputWindow << pCommandUIPlugin->FormatError("Validate", strReason) << "\n";
      return;
   }

   if (pCommandUIPlugin->GetObjectType())
      m_sotarray[pCommandUIPlugin->GetObjectType()].AddTail(pCommandUIPlugin);

   if (pCommandUIPlugin->GetViewType())
      m_svtarray[pCommandUIPlugin->GetViewType()].AddTail(pCommandUIPlugin);

   if (pCommandUIPlugin->GetDocType())
      m_sdtarray[pCommandUIPlugin->GetDocType()].AddTail(pCommandUIPlugin);

   if (pCommandUIPlugin->GetOnCommandMainFrameCallback() && pCommandUIPlugin->GetOnUpdateCommandMainFrameCallback())
       m_mainframelist.AddTail(pCommandUIPlugin);
}

// this will remove it from all lists
void CommandUIPluginManager::Remove( CommandUIPlugin *pPlugin )
{
   POSITION pos = m_list.Find(pPlugin);
   if (pos)
      m_list.RemoveAt(pos);

   pos = m_allcommanduilist.Find(pPlugin);
   if (pos)
      m_allcommanduilist.RemoveAt(pos);

   CommandUIPluginList *pList;

   pList = &m_sotarray[pPlugin->GetObjectType()];
   if (pList) {
      pos = pList->Find(pPlugin);
      if (pos)
         pList->RemoveAt(pos);
   }

   pList = &m_svtarray[pPlugin->GetViewType()];
   if (pList) {
      pos = pList->Find(pPlugin);
      if (pos)
         pList->RemoveAt(pos);
   }

   pList = &m_sdtarray[pPlugin->GetDocType()];
   if (pList) {
      pos = pList->Find(pPlugin);
      if (pos)
         pList->RemoveAt(pos);
   }

   pos = m_mainframelist.Find(pPlugin);
   if (pos)
      m_mainframelist.RemoveAt(pos);
}

void CommandUIPluginManager::InitPlugins()
{
   RegistryIDMap idmap;// Loads on Construct, and Saves on Destruct

   CMainFrame *pMainFrame = GetMainFrame();
   
   for (POSITION pos = m_list.GetHeadPosition(); pos; ) {
      CommandUIPlugin *pPlugin = (CommandUIPlugin *)m_list.GetNext(pos);
      UINT nID = idmap.GetCmdID(pPlugin->GetSnipePlugin()->GetName(), pPlugin->Plugin::GetClassName());
      pPlugin->SetCmdID(nID);

      // Init Dependecies
      try {
         pPlugin->GetInitCallback()(pPlugin->GetPluginData(), GetMainInterface()->GetCommandPluginManager());
      }
      catch(const String &strReason) {
         AfxSetModuleState(AfxGetAppModuleState());
         g_outputWindow << pPlugin->FormatError("Init", strReason) << "\n";
         Remove(pPlugin);
         continue;
      }
      catch(...) {
         AfxSetModuleState(AfxGetAppModuleState());
         g_outputWindow << pPlugin->FormatError("Init", "Unknown") << "\n";
         Remove(pPlugin);
         continue;
      }
      //Add Command
      GetMainInterface()->GetCommandUIManager()->AddCommandUI(pPlugin);

      // Lets place the command now
      try {
         PFN_PLACE_COMMANDUIPLUGIN pfnPlace = pPlugin->GetPlaceCallback();
         if (pfnPlace)
            pfnPlace(pPlugin->GetPluginData(), pMainFrame);
      }
      catch(const String &strReason) {
         g_outputWindow << pPlugin->FormatError("PlaceCommand", strReason) << "\n";
      }
      catch(...) {
         g_outputWindow << pPlugin->FormatError("PlaceCommand", "Unknown") << "\n";
      }
   }
}
