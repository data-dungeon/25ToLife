#pragma once
#include "Plugin.h"
#include "SDK/Enum.h"

class SnipePlugin;
class Scene;
typedef void (* PFN_SCENEIMPORT) (void *pPluginData, Scene *, const char *szFileName);
typedef void (* PFN_SCENEEXPORT) (void *pPluginData, Scene *, const char *szFileName);

class SceneTranslatorPlugin : public Plugin
{
public:
private:
   String m_strFilterName; //LIKE TUPPERWARE
   String m_strExtension; //LIKE tup, or mdl
   catch_msg PFN_SCENEIMPORT m_pfnImport;
   catch_msg PFN_SCENEEXPORT m_pfnExport;

public:

   SceneTranslatorPlugin( SnipePlugin *pSnipePlugin, void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData  ) : 
    Plugin(pSnipePlugin, pPluginData, pfnDestroy_PluginData),
    m_pfnImport(NULL),
    m_pfnExport(NULL)
   {
   }


   void SetFilterName(const String &strFilterName) { m_strFilterName = strFilterName; }
   void SetExtension(const String &strExtension) { m_strExtension = strExtension; }
   void SetImportCallback(PFN_SCENEIMPORT pfnImport) { m_pfnImport = pfnImport; } 
   void SetExportCallback(PFN_SCENEEXPORT pfnExport) { m_pfnExport = pfnExport; } 
   
   String GetFilterName() const { return m_strFilterName; }
   String GetExtension() const { return m_strExtension; }
   SnipeObjectType GetSOT() const;
   PFN_SCENEIMPORT GetImportCallback() const { return m_pfnImport; }
   PFN_SCENEEXPORT GetExportCallback() const { return m_pfnExport; }

   //Plugin Overrides
   virtual String GetClassType() const { return "Scene Translator"; }
};

inline SnipeObjectType SceneTranslatorPlugin::GetSOT() const
{
   return SOT_Scene;
}
