#pragma once
#include "Plugin.h"
#include "SDK/Enum.h"

class SnipePlugin;
class PolyModel;
typedef void (* PFN_MODELIMPORT) (void *pPluginData, PolyModel *, const char *szFileName);
typedef void (* PFN_MODELEXPORT) (void *pPluginData, PolyModel *, const char *szFileName);

class ModelTranslatorPlugin : public Plugin
{
public:
   enum ModelType { MT_POLYMODEL, MT_PATCHMODEL, MT_SUBDMODEL };
private:
   String m_strFilterName; //LIKE TUPPERWARE
   String m_strExtension; //LIKE tup, or mdl
   ModelType m_nModelType;
   catch_msg PFN_MODELIMPORT m_pfnImport;
   catch_msg PFN_MODELEXPORT m_pfnExport;

public:

   ModelTranslatorPlugin( SnipePlugin *pSnipePlugin, void *pPluginData, PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData  ) : 
    Plugin(pSnipePlugin, pPluginData, pfnDestroy_PluginData),
    m_pfnImport(NULL),
    m_pfnExport(NULL)
   {
   }


   void SetFilterName(const String &strFilterName) { m_strFilterName = strFilterName; }
   void SetExtension(const String &strExtension) { m_strExtension = strExtension; }
   void SetModelType( ModelType nModelType ) { m_nModelType = nModelType; }
   void SetImportCallback(PFN_MODELIMPORT pfnImport) { m_pfnImport = pfnImport; } 
   void SetExportCallback(PFN_MODELEXPORT pfnExport) { m_pfnExport = pfnExport; } 
   
   String GetFilterName() const { return m_strFilterName; }
   String GetExtension() const { return m_strExtension; }
   ModelType GetModelType() const { return m_nModelType; }
   SnipeObjectType GetSOT() const;
   PFN_MODELIMPORT GetImportCallback() const { return m_pfnImport; }
   PFN_MODELEXPORT GetExportCallback() const { return m_pfnExport; }

   //Plugin Overrides
   virtual String GetClassType() const { return "Model Translator"; }
};

inline SnipeObjectType ModelTranslatorPlugin::GetSOT() const
{
   switch(m_nModelType) 
   {
      case MT_POLYMODEL:
         return SOT_PolyModel;
      case MT_PATCHMODEL:
         return SOT_PatchModel;
      case MT_SUBDMODEL:
         return SOT_SubDModel;
//      case MT_BEZIERSPLINEMODEL:
//         return SOT_BezierSplineModel;
      default:
         return SOT_Unknown;
   }
}

//inline ModelTranslatorPlugin::ModelType ModelTranslatorPlugin::GetModelType(SnipeObjectType nSOT)
//{
//   switch(nSOT) 
//   {
//      case SOT_PolyModel:
//         return MT_POLYMODEL;
//      case SOT_PatchModel:
//         return MT_PATCHMODEL;
//      case SOT_SubDModel:
//         return MT_SUBDMODEL;
//      case SOT_BezierSplineModel:
//         return MT_BEZIERSPLINEMODEL;
//      default: {
//         ASSERT(FALSE); // This should NEVER happen
//         return (ModelTranslatorPlugin::ModelType)-1;
//      }
//   }
//}
