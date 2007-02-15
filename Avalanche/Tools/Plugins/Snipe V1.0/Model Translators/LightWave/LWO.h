#pragma once

#include "HModelTranslatorPlugin.h" 
#include "HModelTranslatorPluginHelper.h" 

class LWOTranslator : public HModelTranslatorPluginHelper
{
public:
   DECLARE_MODELTRANSLATOR_PLUGINHELPER( LWOTranslator, HModelTranslatorPluginHelper )
   LWOTranslator();

   // HModelTranslatorPluginHelper overrides
   virtual const char *GetFilterName() { return "LightWave Object"; }
   virtual const char *GetFilterExtension() { return "lwo"; }
   virtual HModelTranslatorPlugin::ModelType GetModelType() { return HModelTranslatorPlugin::MT_POLYMODEL; }
   virtual BOOL HasImporter() { return TRUE; }
   virtual void OnImport( HPolyModel *pPolyModel, const char *szFileName);

   // HPluginHelper overrides
   virtual const char *GetTitleName() const { return "LightWave Object"; }
};

 