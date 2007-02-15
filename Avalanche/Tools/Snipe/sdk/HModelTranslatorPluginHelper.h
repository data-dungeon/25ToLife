#pragma once
#include "HPluginHelper.h"

/////////////////////////////////////////////////////////////////////////////
// C++ Helper class for HModelTranslatorPlugin

class HModelTranslatorPluginHelper : public HPluginHelper
{
public:
   HModelTranslatorPlugin *GetPlugin() { return (HModelTranslatorPlugin *)HPluginHelper::GetPlugin(); }

   virtual BOOL HasImporter() { return FALSE; }
   virtual BOOL HasExporter() { return FALSE; }

   // Pure
   virtual const char *GetFilterName() = 0;
   virtual const char *GetFilterExtension() = 0;
   virtual HModelTranslatorPlugin::ModelType GetModelType() = 0;

   // Callbacks
   catch_msg virtual void OnImport( HPolyModel *pPolyModel, const char *szFileName) {}
   catch_msg virtual void OnExport( HPolyModel *pPolyModel, const char *szFileName) {}
};

////////////////////////////////////////////////////////////////////////////
// Inlines

inline void OnImport_HModelTranslatorPluginHelper(void *pPluginData, HPolyModel *pModel, const char *szFileName)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   ((HModelTranslatorPluginHelper *)pPluginData)->OnImport( pModel, szFileName );
}

inline void OnExport_HModelTranslatorPluginHelper(void *pPluginData, HPolyModel *pModel, const char *szFileName)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   ((HModelTranslatorPluginHelper *)pPluginData)->OnExport( pModel, szFileName );
}

inline HModelTranslatorPluginHelper *GetModelTranslatorPluginHelper( HModelTranslatorPlugin *pPlugin )
{
   ASSERT(pPlugin);
   return (HModelTranslatorPluginHelper *)pPlugin->GetPluginData();\
}

/////////////////////////////////////////////////////////////////////////
// Macro

#define ADD_SNIPE_MODEL_TRANSLATOR_PLUGIN( pSnipePlugin, helper_class ) \
{ \
   AFX_MANAGE_STATE(AfxGetStaticModuleState()); \
   HModelTranslatorPluginHelper *pHelper = new helper_class; \
   HModelTranslatorPlugin *pPlugin = pSnipePlugin->AddModelTranslatorPlugin(pHelper, Destroy_PluginData); \
   pHelper->SetPlugin(pPlugin); \
   pPlugin->SetClassName( pHelper->GetClassName() ); \
   pPlugin->SetTitleName( pHelper->GetTitleName() ); \
   pPlugin->SetFilterName( pHelper->GetFilterName() ); \
   pPlugin->SetExtension( pHelper->GetFilterExtension() ); \
   pPlugin->SetModelType( pHelper->GetModelType() ); \
   pPlugin->SetImportCallback( pHelper->HasImporter() ? OnImport_HModelTranslatorPluginHelper : NULL ); \
   pPlugin->SetExportCallback( pHelper->HasExporter() ? OnExport_HModelTranslatorPluginHelper : NULL ); \
}

#define DECLARE_MODELTRANSLATOR_PLUGINHELPER(class_name, baseclass_name) \
   DECLARE_PLUGINHELPER(class_name, baseclass_name )
