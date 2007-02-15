#pragma once
#include "HPluginHelper.h"

/////////////////////////////////////////////////////////////////////////////
// C++ Helper class for HSceneTranslatorPlugin

class HSceneTranslatorPluginHelper : public HPluginHelper
{
public:
   HSceneTranslatorPlugin *GetPlugin() { return (HSceneTranslatorPlugin *)HPluginHelper::GetPlugin(); }

   virtual BOOL HasImporter() { return FALSE; }
   virtual BOOL HasExporter() { return FALSE; }

   // Pure
   virtual const char *GetFilterName() = 0;
   virtual const char *GetFilterExtension() = 0;

   // Callbacks
   catch_msg virtual void OnImport( HScene *pScene, const char *szFileName) {}
   catch_msg virtual void OnExport( HScene *pScene, const char *szFileName) {}
};

////////////////////////////////////////////////////////////////////////////
// Inlines

inline void OnImport_HSceneTranslatorPluginHelper(void *pPluginData, HScene *pScene, const char *szFileName)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   ((HSceneTranslatorPluginHelper *)pPluginData)->OnImport( pScene, szFileName );
}

inline void OnExport_HSceneTranslatorPluginHelper(void *pPluginData, HScene *pScene, const char *szFileName)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   ((HSceneTranslatorPluginHelper *)pPluginData)->OnExport( pScene, szFileName );
}

inline HSceneTranslatorPluginHelper *GetSceneTranslatorPluginHelper( HSceneTranslatorPlugin *pPlugin )
{
   ASSERT(pPlugin);
   return (HSceneTranslatorPluginHelper *)pPlugin->GetPluginData();\
}

/////////////////////////////////////////////////////////////////////////
// Macro

#define ADD_SNIPE_SCENE_TRANSLATOR_PLUGIN( pSnipePlugin, helper_class ) \
{ \
   AFX_MANAGE_STATE(AfxGetStaticModuleState()); \
   HSceneTranslatorPluginHelper *pHelper = new helper_class; \
   HSceneTranslatorPlugin *pPlugin = pSnipePlugin->AddSceneTranslatorPlugin(pHelper, Destroy_PluginData); \
   pHelper->SetPlugin(pPlugin); \
   pPlugin->SetClassName( pHelper->GetClassName() ); \
   pPlugin->SetTitleName( pHelper->GetTitleName() ); \
   pPlugin->SetFilterName( pHelper->GetFilterName() ); \
   pPlugin->SetExtension( pHelper->GetFilterExtension() ); \
   pPlugin->SetImportCallback( pHelper->HasImporter() ? OnImport_HSceneTranslatorPluginHelper : NULL ); \
   pPlugin->SetExportCallback( pHelper->HasExporter() ? OnExport_HSceneTranslatorPluginHelper : NULL ); \
}

#define DECLARE_SCENETRANSLATOR_PLUGINHELPER(class_name, baseclass_name) \
   DECLARE_PLUGINHELPER(class_name, baseclass_name )
