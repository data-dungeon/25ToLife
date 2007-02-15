#pragma once
#include "Plugin.h"
#include "SDK/Enum.h"

class SnipePlugin;
class MotionSet;
typedef void (* PFN_MOTIONSETIMPORT) (void *pPluginData, MotionSet *, const char *szFileName);
typedef void (* PFN_MOTIONSETEXPORT) (void *pPluginData, MotionSet *, const char *szFileName);

class MotionSetTranslatorPlugin : public Plugin
{
public:
private:
   String m_strFilterName; //LIKE TUPPERWARE
   String m_strExtension; //LIKE tup, or mdl
   catch_msg PFN_MOTIONSETIMPORT m_pfnImport;
   catch_msg PFN_MOTIONSETEXPORT m_pfnExport;

public:

   MotionSetTranslatorPlugin( SnipePlugin *pSnipePlugin, void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData  ) : 
    Plugin(pSnipePlugin, pPluginData, pfnDestroy_PluginData),
    m_pfnImport(NULL),
    m_pfnExport(NULL)
   {
   }


   void SetFilterName(const String &strFilterName) { m_strFilterName = strFilterName; }
   void SetExtension(const String &strExtension) { m_strExtension = strExtension; }
   void SetImportCallback(PFN_MOTIONSETIMPORT pfnImport) { m_pfnImport = pfnImport; } 
   void SetExportCallback(PFN_MOTIONSETEXPORT pfnExport) { m_pfnExport = pfnExport; } 
   
   String GetFilterName() const { return m_strFilterName; }
   String GetExtension() const { return m_strExtension; }
   SnipeObjectType GetSOT() const;
   PFN_MOTIONSETIMPORT GetImportCallback() const { return m_pfnImport; }
   PFN_MOTIONSETEXPORT GetExportCallback() const { return m_pfnExport; }

   //Plugin Overrides
   virtual String GetClassType() const { return "MotionSet Translator"; }
};

inline SnipeObjectType MotionSetTranslatorPlugin::GetSOT() const
{
   return SOT_MotionSet;
}
