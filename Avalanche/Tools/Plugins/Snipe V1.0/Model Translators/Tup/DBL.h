#pragma once

#include "HModelTranslatorPlugin.h" 
#include "HModelTranslatorPluginHelper.h" 

class DBLModelTranslator : public HModelTranslatorPluginHelper
{
public:
   DECLARE_MODELTRANSLATOR_PLUGINHELPER( DBLModelTranslator, HModelTranslatorPluginHelper )
   DBLModelTranslator();

   // HModelTranslatorPluginHelper overrides
   virtual const char *GetFilterName() { return "DBL Model"; }
   virtual const char *GetFilterExtension() { return "dbl"; }
   virtual HModelTranslatorPlugin::ModelType GetModelType() { return HModelTranslatorPlugin::MT_POLYMODEL; }
   virtual BOOL HasExporter() { return TRUE; }
   virtual void OnExport( HPolyModel *pPolyModel, const char *szFileName);

   // HPluginHelper overrides
   virtual const char *GetTitleName() const { return "DBL Model"; }
};

