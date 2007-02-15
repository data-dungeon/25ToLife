//
#include "stdafx.h"
#include "ManipulatorPlugin.h"
#include "SnipePlugin.h"
#include "GUI/GLBaseView.h" 
#include "GUI/BaseDoc.h"

static void OnCommandView_ManipulatorPlugin (void *pPluginData, UINT nCmdID, CGLBaseView *pView, CommandPlugin *&pCommandPlugin, const String &strParameters)
{
   CommandUI *pCommandUI = GetMainInterface()->GetCommandUIManager()->GetCommandUI(nCmdID);
   ManipulatorPlugin *pPlugin = (ManipulatorPlugin *)pCommandUI;

   ASSERT(pPlugin);

   // Set the active manipulator
   SnipeManipulator *pManipulator = GetMainInterface()->GetSnipeManipulatorManager()->Find( pPlugin->GetCmdID() );
   ASSERT( pManipulator );
   pView->SetManipulatorInstance( pManipulator );
   pView->GetDocument()->InvalidateViews(pView->GetDocument()->m_pObjectHead);
}

static void OnUpdateCommandView_ManipulatorPlugin(void *pPluginData, const CGLBaseView *pView, CCmdUI *pCmdUI)
{
   CommandUI *pCommandUI = GetMainInterface()->GetCommandUIManager()->GetCommandUI(pCmdUI->m_nID);
   ManipulatorPlugin *pPlugin = (ManipulatorPlugin *)pCommandUI;

   SnipeObjectTypeArray *pSOTArray = pView->GetSelectableObjectTypeArray();
   ASSERT(pSOTArray);

   BOOL bIsAvailableManipulator = FALSE;
   BOOL bIsActiveManipulator; 

   SnipeManipulatorInstance *pSnipeManipulatorInstance = pView->GetManipulatorInstance();
   if (pSnipeManipulatorInstance && pSnipeManipulatorInstance->m_pPlugin == pPlugin)
      bIsActiveManipulator = TRUE;
   else
      bIsActiveManipulator = FALSE;

   SnipeManipulatorList *pManipulatorList = pSOTArray->GetAvailableManipulatorList();

   SnipeManipulator *pSnipeManipulator;
   for (POSITION pos = pManipulatorList->GetHeadPosition(); pos; ) {
      pSnipeManipulator = pManipulatorList->GetNext(pos);
      if (pPlugin == pSnipeManipulator->m_pPlugin) {
         bIsAvailableManipulator = TRUE;
         break;
      }
   }

   pCmdUI->Enable(bIsAvailableManipulator);
   pCmdUI->SetCheck(bIsActiveManipulator);
}

ManipulatorPlugin::ManipulatorPlugin( SnipePlugin *pSnipePlugin, void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData ) : 
   CommandUIPlugin(pSnipePlugin, pPluginData, pfnDestroy_PluginData)
{
   //CommandUIPlugin Overrides
   m_pfnOnCommandView = OnCommandView_ManipulatorPlugin;
   m_pfnOnUpdateCommandView = OnUpdateCommandView_ManipulatorPlugin;
   m_nViewType = SVT_CGLBaseView;

   // Plugin
   m_pfnCreateInstance_ManipulatorPlugin = NULL;
   m_pfnSupportSOT_ManipulatorPlugin = NULL;

   // Instance
   m_pfnDestroy_ManipulatorInstance = NULL;
   m_pfnGetStatusbarInstructions_ManipulatorInstance = NULL;
   m_pfnDrawPick_ManipulatorInstance = NULL;
   m_pfnPreDraw_ManipulatorInstance = NULL;
   m_pfnPostDraw_ManipulatorInstance = NULL;
   m_pfnOnLButtonDown_ManipulatorInstance = NULL;
   m_pfnOnLButtonDblClk_ManipulatorInstance = NULL;
   m_pfnOnLButtonUp_ManipulatorInstance = NULL;
   m_pfnOnMButtonDown_ManipulatorInstance = NULL;
   m_pfnOnMButtonDblClk_ManipulatorInstance = NULL;
   m_pfnOnMButtonUp_ManipulatorInstance = NULL;
   m_pfnOnRButtonDown_ManipulatorInstance = NULL;
   m_pfnOnRButtonDblClk_ManipulatorInstance = NULL;
   m_pfnOnRButtonUp_ManipulatorInstance = NULL;
   m_pfnOnMouseMove_ManipulatorInstance = NULL;
   m_pfnOnMouseWheel_ManipulatorInstance = NULL;
   m_pfnOnSetCursor_ManipulatorInstance = NULL;
   m_pfnGetToolTipText_ManipulatorInstance = NULL;
   m_pfnOnApply_ManipulatorInstance = NULL;
   m_pfnOnCancel_ManipulatorInstance = NULL;
}

String ManipulatorPlugin::GetClassType() const
{
   return "ManipulatorPlugin";
}

void ManipulatorPlugin::Validate()
{
   CommandUIPlugin::Validate();

   if (m_pfnSupportSOT_ManipulatorPlugin==NULL)
      THROW_FATAL_ERROR("Invalid Support SOT Callback.");

   if (m_pfnCreateInstance_ManipulatorPlugin==NULL)
      THROW_FATAL_ERROR("Invalid Create Manipulator Instance Callback.");

   if (m_pfnDestroy_ManipulatorInstance==NULL)
      THROW_FATAL_ERROR("Invalid Destroy Callback.");

   if (m_pfnGetStatusbarInstructions_ManipulatorInstance==NULL)
      THROW_FATAL_ERROR("Invalid GetStatusbarInstructions Callback.");

   if (m_pfnOnApply_ManipulatorInstance==NULL)
      THROW_FATAL_ERROR("Invalid OnApply Callback.");

   if (m_pfnOnCancel_ManipulatorInstance==NULL)
      THROW_FATAL_ERROR("Invalid OnCancel Callback.");
}