#pragma once
#include "Plugin.h"
#include "SDK/Enum.h"

class SnipePlugin;
class Action;
typedef void (* PFN_ACTIONIMPORT) (void *pPluginData, Action *, const char *szFileName);
typedef void (* PFN_ACTIONEXPORT) (void *pPluginData, Action *, const char *szFileName);

class ActionTranslatorPlugin : public Plugin
{
public:
private:
   String m_strFilterName; //LIKE TUPPERWARE
   String m_strExtension; //LIKE tup, or mdl
   catch_msg PFN_ACTIONIMPORT m_pfnImport;
   catch_msg PFN_ACTIONEXPORT m_pfnExport;

public:

   ActionTranslatorPlugin( SnipePlugin *pSnipePlugin, void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData  ) : 
    Plugin(pSnipePlugin, pPluginData, pfnDestroy_PluginData),
    m_pfnImport(NULL),
    m_pfnExport(NULL)
   {
   }


   void SetFilterName(const String &strFilterName) { m_strFilterName = strFilterName; }
   void SetExtension(const String &strExtension) { m_strExtension = strExtension; }
   void SetImportCallback(PFN_ACTIONIMPORT pfnImport) { m_pfnImport = pfnImport; } 
   void SetExportCallback(PFN_ACTIONEXPORT pfnExport) { m_pfnExport = pfnExport; } 
   
   String GetFilterName() const { return m_strFilterName; }
   String GetExtension() const { return m_strExtension; }
   SnipeObjectType GetSOT() const;
   PFN_ACTIONIMPORT GetImportCallback() const { return m_pfnImport; }
   PFN_ACTIONEXPORT GetExportCallback() const { return m_pfnExport; }

   //Plugin Overrides
   virtual String GetClassType() const { return "Action Translator"; }
};

inline SnipeObjectType ActionTranslatorPlugin::GetSOT() const
{
   return SOT_Action;
}
