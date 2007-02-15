//
#include "stdafx.h"
#include "Plugin.h"
#include "SnipePlugin.h"

Plugin::~Plugin()
{
   try {
      if (m_pfnDestroy_PluginData && m_pPluginData)
         m_pfnDestroy_PluginData(m_pPluginData);
   }
   catch(const String &strReason)
   {
      g_outputWindow << FormatError("Destroy", strReason) << "\n";
   }
   catch(...) {
      g_outputWindow << FormatError("Destroy", "Unknown") << "\n";
   }
}

void Plugin::Validate()
{
   if (m_strClassName.IsEmpty())
      THROW_FATAL_ERROR("Invalid Class Name.");

   if (m_strTitleName.IsEmpty())
      THROW_FATAL_ERROR("Invalid Title Name.");

   if (m_pfnDestroy_PluginData==NULL)
      THROW_FATAL_ERROR("Invalid Destroy Plugin Data Callback.");
}

void Plugin::SetTitleName( const String &strTitleName)
{
#if 0
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   g_outputWindow << "Loaded plugin: " << strTitleName << "\n";
#endif // _DEBUG
   m_strTitleName = strTitleName;
}

String Plugin::FormatError(const String &strFunctionName, const String &strReason)
{
   SnipePlugin *pPlugin = GetSnipePlugin();

   String strMsg("%FUNCTIONNAME failed in %CLASSTYPE Plugin: (%PLUGINNAME : %CLASSNAME), Reason: %REASON.");
   strMsg.Replace("%FUNCTIONNAME", strFunctionName); 
   strMsg.Replace("%CLASSTYPE", GetClassType());
   strMsg.Replace("%PLUGINNAME", pPlugin->GetName());
   strMsg.Replace("%CLASSNAME", GetClassName());
   strMsg.Replace("%REASON", strReason);

   return strMsg;
}