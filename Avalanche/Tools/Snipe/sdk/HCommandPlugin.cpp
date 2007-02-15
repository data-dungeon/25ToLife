//
#include "stdafx.h"
#include "CommandPlugin.h"
#include "HCommandPlugin.h"
#include "SnipePlugin.h"

#include "HCBaseDoc.h"
#include "GUI\BaseDoc.h"

void HCommandPlugin::SetCLIName( const char *szCLIName )
{
   ((CommandPlugin *)this)->SetCLIName( szCLIName );
}

void HCommandPlugin::SetBitmapID(UINT nID)
{
   CommandPlugin *pPlugin = (CommandPlugin *)this;

   pPlugin->GetIconImageList()->Create(nID, 0, pPlugin->GetSnipePlugin()->GetHInstance());
}

void HCommandPlugin::SetInitCommandPluginCallback(SDK_PFN_INIT_COMMANDPLUGIN pfn)
{
   ((CommandPlugin *)this)->SetInitCommandPluginCallback((PFN_INIT_COMMANDPLUGIN)pfn);
}

void HCommandPlugin::SetCreateCommandInstanceCallback(SDK_PFN_CREATEINSTANCE_COMMANDPLUGIN pfn)
{
   ((CommandPlugin *)this)->SetCreateCommandInstanceCallback((PFN_CREATEINSTANCE_COMMANDPLUGIN)pfn);
}

void HCommandPlugin::SetDestroyCommandInstanceCallback(SDK_PFN_DESTROY_COMMANDINSTANCE pfn)
{
   ((CommandPlugin *)this)->SetDestroyCommandInstanceCallback((PFN_DESTROY_COMMANDINSTANCE)pfn);
}

void HCommandPlugin::SetValidateCommandInstanceCallback(SDK_PFN_VALIDATE_COMMANDINSTANCE pfn)
{
   ((CommandPlugin *)this)->SetValidateCommandInstanceCallback((PFN_VALIDATE_COMMANDINSTANCE)pfn);
}

void HCommandPlugin::SetSupportUndoCommandInstanceCallback(SDK_PFN_SUPPORTUNDO_COMMANDINSTANCE pfn)
{
   ((CommandPlugin *)this)->SetSupportUndoCommandInstanceCallback((PFN_SUPPORTUNDO_COMMANDINSTANCE)pfn);
}

void HCommandPlugin::SetDoCommandInstanceCallback(SDK_PFN_DO_COMMANDINSTANCE pfn)
{
   ((CommandPlugin *)this)->SetDoCommandInstanceCallback((PFN_DO_COMMANDINSTANCE)pfn);
}

void HCommandPlugin::SetRedoCommandInstanceCallback(SDK_PFN_REDO_COMMANDINSTANCE pfn)
{
   ((CommandPlugin *)this)->SetRedoCommandInstanceCallback((PFN_REDO_COMMANDINSTANCE)pfn);
}

void HCommandPlugin::SetUndoCommandInstanceCallback(SDK_PFN_UNDO_COMMANDINSTANCE pfn)
{
   ((CommandPlugin *)this)->SetUndoCommandInstanceCallback((PFN_UNDO_COMMANDINSTANCE)pfn);
}

void HCommandPlugin::SetHelp(const String &strHelp )
{
   ((CommandPlugin *)this)->SetHelp( strHelp );
}

SDK_PFN_INIT_COMMANDPLUGIN HCommandPlugin::GetInitCommandPluginCallback() const
{
   return (SDK_PFN_INIT_COMMANDPLUGIN) ((CommandPlugin *)this)->GetInitCommandPluginCallback();
}

SDK_PFN_CREATEINSTANCE_COMMANDPLUGIN HCommandPlugin::GetCreateCommandInstanceCallback() const
{
   return (SDK_PFN_CREATEINSTANCE_COMMANDPLUGIN) ((CommandPlugin *)this)->GetCreateCommandInstanceCallback();
}

SDK_PFN_DESTROY_COMMANDINSTANCE HCommandPlugin::GetDestroyCommandInstanceCallback() const
{
   return (SDK_PFN_DESTROY_COMMANDINSTANCE) ((CommandPlugin *)this)->GetDestroyCommandInstanceCallback();
}

SDK_PFN_VALIDATE_COMMANDINSTANCE HCommandPlugin::GetValidateCommandInstanceCallback() const
{
   return (SDK_PFN_VALIDATE_COMMANDINSTANCE) ((CommandPlugin *)this)->GetValidateCommandInstanceCallback();
}

SDK_PFN_SUPPORTUNDO_COMMANDINSTANCE HCommandPlugin::GetSupportUndoCommandInstanceCallback() const
{
   return (SDK_PFN_SUPPORTUNDO_COMMANDINSTANCE) ((CommandPlugin *)this)->GetSupportUndoCommandInstanceCallback();
}

SDK_PFN_DO_COMMANDINSTANCE HCommandPlugin::GetDoCommandInstanceCallback() const
{
   return (SDK_PFN_DO_COMMANDINSTANCE) ((CommandPlugin *)this)->GetDoCommandInstanceCallback();
}

SDK_PFN_REDO_COMMANDINSTANCE HCommandPlugin::GetRedoCommandInstanceCallback() const
{
   return (SDK_PFN_REDO_COMMANDINSTANCE) ((CommandPlugin *)this)->GetRedoCommandInstanceCallback();
}

SDK_PFN_UNDO_COMMANDINSTANCE HCommandPlugin::GetUndoCommandInstanceCallback() const
{
   return (SDK_PFN_UNDO_COMMANDINSTANCE) ((CommandPlugin *)this)->GetUndoCommandInstanceCallback();
}
