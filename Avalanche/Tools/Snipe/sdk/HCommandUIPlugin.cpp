//
#include "stdafx.h"
#include "CommandUIPlugin.h"
#include "HCommandUIPlugin.h"

void HCommandUIPlugin::SetInitCommandUIPluginCallback(SDK_PFN_INIT_COMMANDUIPLUGIN pfn)
{
   ((CommandUIPlugin *)this)->SetInitCallback((PFN_INIT_COMMANDUIPLUGIN)pfn);
}

void HCommandUIPlugin::SetGetBitmapIDCallback(SDK_PFN_GETBITMAPID_COMMANDUIPLUGIN pfn)
{
   ((CommandUIPlugin *)this)->SetGetBitmapIDCallback((PFN_GETBITMAPID_COMMANDUIPLUGIN)pfn);
}

void HCommandUIPlugin::SetPlaceCallback(SDK_PFN_PLACE_COMMANDUIPLUGIN pfn)
{
   ((CommandUIPlugin *)this)->SetPlaceCallback((PFN_PLACE_COMMANDUIPLUGIN)pfn);
}

void HCommandUIPlugin::SetOnObjectNotifyCallback(SDK_PFN_ONOBJECTNOTIFY_COMMANDUIPLUGIN pfn)
{
   ((CommandUIPlugin *)this)->SetOnObjectNotifyCallback((PFN_ONOBJECTNOTIFY_COMMANDUIPLUGIN)pfn);
}

void HCommandUIPlugin::SetDescription( const char *szDescription )
{
   ((CommandUIPlugin *)this)->SetDescription( szDescription );
}

void HCommandUIPlugin::SetCategory( const char *szCategory )
{
   ((CommandUIPlugin *)this)->SetCategory( szCategory );
}

void HCommandUIPlugin::SetAccelerators( UINT nCount, CommandUIAccel *pAccel )
{
   ((CommandUIPlugin *)this)->SetAccelerators( nCount, pAccel );
}

void HCommandUIPlugin::SetPropertyArray( HPropertyArray *pPropertyArray )
{
   ((CommandUIPlugin *)this)->SetPropertyArray( (PropertyArray *)pPropertyArray );
}

void HCommandUIPlugin::SetMessageMapForObject(SnipeObjectType nObjectType, 
 SDK_PFN_ON_COMMAND_OBJECT lpfnOnCommandObject,
 SDK_PFN_ON_UPDATE_COMMAND_UI_OBJECT lpfnOnUpdateCommandObject)
{
   ((CommandUIPlugin *)this)->SetMessageMapForObject(
     nObjectType,
     (PFN_ON_COMMAND_OBJECT)lpfnOnCommandObject,
     (PFN_ON_UPDATE_COMMAND_UI_OBJECT)lpfnOnUpdateCommandObject);
}

void HCommandUIPlugin::SetMessageMapForView(SnipeViewType nViewType, 
 SDK_PFN_ON_COMMAND_VIEW lpfnOnCommandView,
 SDK_PFN_ON_UPDATE_COMMAND_UI_VIEW lpfnOnUpdateCommandView)
{
   ((CommandUIPlugin *)this)->SetMessageMapForView(
     nViewType,
     (PFN_ON_COMMAND_VIEW)lpfnOnCommandView,
     (PFN_ON_UPDATE_COMMAND_UI_VIEW)lpfnOnUpdateCommandView);
}

void HCommandUIPlugin::SetMessageMapForDoc(SnipeDocType nDocType, 
 SDK_PFN_ON_COMMAND_DOC lpfnOnCommandDoc,
 SDK_PFN_ON_UPDATE_COMMAND_UI_DOC lpfnOnUpdateCommandDoc)
{
   ((CommandUIPlugin *)this)->SetMessageMapForDoc(
     nDocType,
     (PFN_ON_COMMAND_DOC)lpfnOnCommandDoc,
     (PFN_ON_UPDATE_COMMAND_UI_DOC)lpfnOnUpdateCommandDoc);
}

void HCommandUIPlugin::SetMessageMapForMainFrame( 
 SDK_PFN_ON_COMMAND_MAINFRAME lpfnOnCommandMainFrame,
 SDK_PFN_ON_UPDATE_COMMAND_UI_MAINFRAME lpfnOnUpdateCommandMainFrame)
{
   ((CommandUIPlugin *)this)->SetMessageMapForMainFrame(
     (PFN_ON_COMMAND_MAINFRAME)lpfnOnCommandMainFrame,
     (PFN_ON_UPDATE_COMMAND_UI_MAINFRAME)lpfnOnUpdateCommandMainFrame);
}

UINT HCommandUIPlugin::GetCmdID() const
{
   return ((CommandUIPlugin *)this)->GetCmdID();
}

HPropertyArray *HCommandUIPlugin::GetPropertyArray()
{
   return (HPropertyArray *) ((CommandUIPlugin *)this)->GetPropertyArray();
}
