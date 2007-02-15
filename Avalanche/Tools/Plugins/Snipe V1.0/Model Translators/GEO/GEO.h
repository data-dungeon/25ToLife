#pragma once

#include "HModelTranslatorPlugin.h" 
#include "HModelTranslatorPluginHelper.h" 

class GEOTranslator : public HModelTranslatorPluginHelper
{
public:
   DECLARE_MODELTRANSLATOR_PLUGINHELPER( GEOTranslator, HModelTranslatorPluginHelper )
   GEOTranslator();

   // HModelTranslatorPluginHelper overrides
   virtual const char *GetFilterName() { return "Geometry Object"; }
   virtual const char *GetFilterExtension() { return "geo"; }
   virtual HModelTranslatorPlugin::ModelType GetModelType() { return HModelTranslatorPlugin::MT_POLYMODEL; }
   virtual BOOL HasExporter() { return TRUE; }
   virtual void OnExport( HPolyModel *pPolyModel, const char *szFileName);

   // HPluginHelper overrides
   virtual const char *GetTitleName() const { return "Geometry Object"; }
};

 