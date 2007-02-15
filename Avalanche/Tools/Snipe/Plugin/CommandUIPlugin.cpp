//
#include "stdafx.h"
#include "CommandUIPlugin.h"
#include "SnipePlugin.h"
#include "Properties/Property.h"
#include "Properties/PropertyArray.h"

String CommandUIPlugin::GetClassType() const
{
   return "CommandUIPlugin";
}

void CommandUIPlugin::SetPropertyArray( PropertyArray *pPropertyArray )
{
   CommandUI::SetPropertyArray( pPropertyArray );

   if (pPropertyArray)
   {
      for (int i=0; i<pPropertyArray->GetCount(); i++)
      {
         Property *pProperty = pPropertyArray->GetAt( i );
         pProperty->m_pParent = this;
      }
   }
}

void CommandUIPlugin::Validate()
{
   Plugin::Validate();

   if (m_strDescription.IsEmpty())
      THROW_FATAL_ERROR("Invalid Description.");

   if (m_strCategory.IsEmpty())
      THROW_FATAL_ERROR("Invalid category.");

   if (m_pfnInit==NULL)
      THROW_FATAL_ERROR("Invalid Init Callback.");

   if (m_pfnGetBitmapID==NULL)
      THROW_FATAL_ERROR("Invalid GetBitmapID Callback.");

   if (m_nObjectType > SOT_NUMOBJECTS)
      THROW_FATAL_ERROR("Invalid objecttype in MessageMap.");

   if (m_nViewType > SVT_NUMVIEWS)
      THROW_FATAL_ERROR("Invalid viewtype in MessageMap.");

   if (m_nDocType > SDT_NUMDOCS)
      THROW_FATAL_ERROR("Invalid doctype in MessageMap.");

   CreateIcon();
}

// Return value is whether the imagelist changed
BOOL CommandUIPlugin::CreateIcon()
{
   UINT nBitmapID;
   try {
      nBitmapID = GetGetBitmapIDCallback()(GetPluginData());
   }
   catch (const String &strReason) {
      g_outputWindow << FormatError("GetBitmapID", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << FormatError("GetBitmapID", "Unknown") << "\n";
   }

   if (m_nCurrentBitmapID == nBitmapID)
      return FALSE;

   if (m_glImageListIcon.Create( nBitmapID, 0, GetSnipePlugin()->GetHInstance()))
      m_nCurrentBitmapID = nBitmapID;
   return TRUE;
}

void CommandUIPlugin::SetAccelerators( UINT nCount, CommandUIAccel *pAccel )
{
   try {
      for (int i=0; i < (int)nCount; i++ )
         m_defaultaccellist.AddTail(pAccel[i]);
   }
   catch (...) {
      g_outputWindow << FormatError("SetAccelerators", "Unknown") << "\n";
   }
}

BOOL CommandUIPlugin::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   try {
      if (GetOnObjectNotifyCallback())
         GetOnObjectNotifyCallback()(GetPluginData(), msg, wParam, lParam);
   }
   catch (const String &strReason) {
      g_outputWindow << FormatError("OnObjectNotify", strReason) << "\n";
   }
   catch (...) {
      g_outputWindow << FormatError("OnObjectNotify", "Unknown") << "\n";
   }

   return CommandUI::OnObjectNotify( msg, wParam, lParam );
}
