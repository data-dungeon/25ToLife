//
#include "stdafx.h"
#include "SnipeCommand.h"
#include "CommandPlugin.h"
#include "Interface/MainInterface.h"
#include "Interface/SelectionList.hpp"
#include "GUI/GLBaseView.h"
#include "HierObject.h"

SnipeCommand::SnipeCommand(CommandPlugin *pPlugin, const String &strParameters, void *pPluginData) :
m_pPlugin(pPlugin),
m_strParameters(strParameters),
m_pPluginData(pPluginData)
{
   m_nSnipeCommandFlags = 0;
   m_bMirrorMode = SnipeObject::IsMirrorMode();
}

SnipeCommand::~SnipeCommand()
{
   if (m_pPlugin) {
      try {
         m_pPlugin->GetDestroyCommandInstanceCallback()(m_pPluginData);
      }
      catch (const String &strReason) {
         g_outputWindow << m_pPlugin->FormatError("Destroy Command Instance", strReason) << "\n";
      }
      catch (...) {
         g_outputWindow << m_pPlugin->FormatError("Destroy Command Instance", "Unknown") << "\n";
      }
   }
}

// doesn't include (MirrorMode On) string
String SnipeCommand::GetCommandInfo( BOOL bLocalizedName /*= TRUE*/, BOOL bAbbreviated/*=TRUE*/ ) const
{
   String strText = m_pPlugin->GetCommandInfo( m_strParameters, bLocalizedName, bAbbreviated );

   if (m_bMirrorMode)
      strText+= "  (Mirror)";

   return strText;   
}

BOOL SnipeCommand::Do(SnipeObject *pCommandObject, StringList &strResults, BOOL bTempCommand)
{
   try {
      BOOL bResult = m_pPlugin->GetDoCommandInstanceCallback()(m_pPluginData, strResults);
      pCommandObject->InvalidateDependantViews();
      if (pCommandObject->GetSelectionList()->IsSelectionChanged())
      {
         pCommandObject->GetSelectionList()->SetSelectionChangedOnTempCommand(bTempCommand);
         GetMainInterface()->NotifyNodes( NOTIFYOBJ_SELECTIONLISTCHANGED, (WPARAM)bTempCommand, (LPARAM)pCommandObject->GetSelectionList() );
      }
      return bResult;
   }
   catch (const String &strReason) {
      g_outputWindow << m_pPlugin->FormatError("Do", strReason) << "\n";
      return FALSE;
   }
   catch (...) {
      g_outputWindow << m_pPlugin->FormatError("Do", "Unknown") << "\n";
      return FALSE;
   }
}

void SnipeCommand::Redo(SnipeObject *pCommandObject, BOOL bTemp)
{
   try {
      MirrorModePusher pusher( m_bMirrorMode );
      m_pPlugin->GetRedoCommandInstanceCallback()(m_pPluginData);
      pCommandObject->InvalidateDependantViews();
      if (pCommandObject->GetSelectionList()->IsSelectionChanged())
      {
         pCommandObject->GetSelectionList()->SetSelectionChangedOnTempCommand(bTemp);
         GetMainInterface()->NotifyNodes( NOTIFYOBJ_SELECTIONLISTCHANGED, (WPARAM)bTemp, (LPARAM)pCommandObject->GetSelectionList() );
      }
   }
   catch (const String &strReason) {
      g_outputWindow << "Error in Redo: " << m_pPlugin->GetCommandInfo(FALSE) << "\n";
      g_outputWindow << m_pPlugin->FormatError("Redo", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << "Error in Redo: " << m_pPlugin->GetCommandInfo(FALSE) << "\n";
      g_outputWindow << m_pPlugin->FormatError("Redo", "Unknown") << "\n";
   }
}

void SnipeCommand::Undo(SnipeObject *pCommandObject, BOOL bTemp)
{
   try {
      MirrorModePusher pusher( m_bMirrorMode );
		m_pPlugin->GetUndoCommandInstanceCallback()(m_pPluginData);
      pCommandObject->InvalidateDependantViews();
      if (pCommandObject->GetSelectionList()->IsSelectionChanged())
      {
         pCommandObject->GetSelectionList()->SetSelectionChangedOnTempCommand(bTemp);
         GetMainInterface()->NotifyNodes( NOTIFYOBJ_SELECTIONLISTCHANGED, (WPARAM)bTemp, (LPARAM)pCommandObject->GetSelectionList() );
      }
   }
   catch (const String &strReason) {
      g_outputWindow << "Error in Undo: " << m_pPlugin->GetCommandInfo(FALSE) << "\n";
      g_outputWindow << m_pPlugin->FormatError("Undo", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << "Error in Undo: " << m_pPlugin->GetCommandInfo(FALSE) << "\n";
      g_outputWindow << m_pPlugin->FormatError("Undo", "Unknown") << "\n";
   }
}
