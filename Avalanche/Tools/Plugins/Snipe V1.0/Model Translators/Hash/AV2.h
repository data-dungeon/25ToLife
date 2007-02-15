#pragma once

#include "HModelTranslatorPlugin.h" 
#include "HModelTranslatorPluginHelper.h" 

class Av2Translator : public HModelTranslatorPluginHelper
{
protected:
public:
   DECLARE_MODELTRANSLATOR_PLUGINHELPER( Av2Translator, HModelTranslatorPluginHelper )

   Av2Translator();

   // HModelTranslatorPluginHelper overrides
   virtual const char *GetFilterName() { return "Hash Avatar"; }
   virtual const char *GetFilterExtension() { return "av2"; }
   virtual HModelTranslatorPlugin::ModelType GetModelType() { return HModelTranslatorPlugin::MT_PATCHMODEL; }
   virtual BOOL HasImporter() { return TRUE; }
   virtual void OnImport( HPolyModel *pPolyModel, const char *szFileName);

   // HPluginHelper overrides
   virtual const char *GetTitleName() const { return "Hash Av2"; }
};