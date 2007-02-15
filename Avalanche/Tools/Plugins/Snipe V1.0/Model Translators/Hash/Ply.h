#pragma once

#include "HModelTranslatorPlugin.h" 
#include "HModelTranslatorPluginHelper.h" 

class PlyTranslator : public HModelTranslatorPluginHelper
{
protected:
public:
   DECLARE_MODELTRANSLATOR_PLUGINHELPER( PlyTranslator, HModelTranslatorPluginHelper )

   PlyTranslator();

   // HModelTranslatorPluginHelper overrides
   virtual const char *GetFilterName() { return "Hash Poly Model"; }
   virtual const char *GetFilterExtension() { return "ply"; }
   virtual HModelTranslatorPlugin::ModelType GetModelType() { return HModelTranslatorPlugin::MT_POLYMODEL; }
   virtual BOOL HasImporter() { return TRUE; }
   virtual catch_msg void OnImport( HPolyModel *pPolyModel, const char *szFileName);

   // HPluginHelper overrides
   virtual const char *GetTitleName() const { return "Hash Ply"; }
};