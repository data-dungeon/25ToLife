//
#include "stdafx.h"
#include "NavigatorPlugin.h"
#include "Interface/SnipeNavigator.h"
#include "GUI/MainFrm.h"

static void OnCommandMainFrame_NavigatorPlugin (void *pPluginData, UINT nCmdID, CMainFrame *pMainFrame, CommandPlugin *&pCommandPlugin, const String &strParameters)
{
   CommandUI *pCommandUI = pMainFrame->m_pMainInterface->GetCommandUIManager()->GetCommandUI(nCmdID);
   NavigatorPlugin *pPlugin = (NavigatorPlugin *)pCommandUI;

   ASSERT(pPlugin);

   // Set the active manipulator
   SnipeNavigator *pNavigator = (SnipeNavigator *)pMainFrame->m_pMainInterface->GetSnipeNavigatorManager()->Find( pPlugin->GetCmdID() );
   ASSERT( pNavigator );
   pMainFrame->m_pMainInterface->SetCurrentNavigator( pNavigator );
}

static void OnUpdateCommandMainFrame_NavigatorPlugin(void *pPluginData, const CMainFrame *pMainFrame, CCmdUI *pCmdUI)
{
   CommandUI *pCommandUI = ((CMainFrame *)pMainFrame)->m_pMainInterface->GetCommandUIManager()->GetCommandUI(pCmdUI->m_nID);
   NavigatorPlugin *pPlugin = (NavigatorPlugin *)pCommandUI;

   pCmdUI->Enable(TRUE);
   pCmdUI->SetCheck(pMainFrame->m_pMainInterface->GetCurrentNavigator()->m_pPlugin == pPlugin);
}

static void *CreateInstance_ManipulatorPlugin( void *PluginData )
{
   return NULL;
}

NavigatorPlugin::NavigatorPlugin( SnipePlugin *pSnipePlugin, void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData ) : 
   ManipulatorPlugin(pSnipePlugin, pPluginData, pfnDestroy_PluginData)
{

   //CommandUIPlugin Overrides
   m_pfnOnCommandView = NULL; // Set these back because ManipulatorPlugin set them;
   m_pfnOnUpdateCommandView = NULL;
   m_nViewType = SVT_Unknown;
   m_pfnOnCommandMainFrame = OnCommandMainFrame_NavigatorPlugin;
   m_pfnOnUpdateCommandMainFrame = OnUpdateCommandMainFrame_NavigatorPlugin;

   //ManipulatorPlugin Overrides
   ManipulatorPlugin::SetCreateInstanceCallback(CreateInstance_ManipulatorPlugin); // Do this so Validate passes, Navigators have there own CreateInstance that passes in the view

   // Plugin
   m_pfnCreateInstance_NavigatorPlugin = NULL;
}

String NavigatorPlugin::GetClassType() const
{
   return "NavigatorPlugin";
}

void NavigatorPlugin::Validate()
{
   ManipulatorPlugin::Validate();

   if (m_pfnCreateInstance_NavigatorPlugin==NULL)
      THROW_FATAL_ERROR("Invalid Create Navigator Instance Callback.");
}
