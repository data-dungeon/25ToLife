#pragma once
#include "Plugin.h"
#include "SDK/Enum.h"

class SnipePlugin;
class Motion;
typedef void (* PFN_MOTIONIMPORT) (void *pPluginData, Motion *, const char *szFileName);
typedef void (* PFN_MOTIONEXPORT) (void *pPluginData, Motion *, const char *szFileName);

class MotionTranslatorPlugin : public Plugin
{
public:
private:
   String m_strFilterName; //LIKE TUPPERWARE
   String m_strExtension; //LIKE tup, or mdl
   catch_msg PFN_MOTIONIMPORT m_pfnImport;
   catch_msg PFN_MOTIONEXPORT m_pfnExport;

public:

   MotionTranslatorPlugin( SnipePlugin *pSnipePlugin, void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData  ) : 
    Plugin(pSnipePlugin, pPluginData, pfnDestroy_PluginData),
    m_pfnImport(NULL),
    m_pfnExport(NULL)
   {
   }


   void SetFilterName(const String &strFilterName) { m_strFilterName = strFilterName; }
   void SetExtension(const String &strExtension) { m_strExtension = strExtension; }
   void SetImportCallback(PFN_MOTIONIMPORT pfnImport) { m_pfnImport = pfnImport; } 
   void SetExportCallback(PFN_MOTIONEXPORT pfnExport) { m_pfnExport = pfnExport; } 
   
   String GetFilterName() const { return m_strFilterName; }
   String GetExtension() const { return m_strExtension; }
   SnipeObjectType GetSOT() const;
   PFN_MOTIONIMPORT GetImportCallback() const { return m_pfnImport; }
   PFN_MOTIONEXPORT GetExportCallback() const { return m_pfnExport; }

   //Plugin Overrides
   virtual String GetClassType() const { return "Motion Translator"; }
};

inline SnipeObjectType MotionTranslatorPlugin::GetSOT() const
{
   return SOT_Motion;
}
