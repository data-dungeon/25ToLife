#include "stdafx.h"
#include "SceneTranslatorPluginManager.h"
#include "Scene.h"
#include "../interface/MainInterface.h"
#include "../gui/Snipe.h"
#include "SceneTranslatorPlugin.h"
#include "../Utility/FileDialog.h"
#include "SnipePlugin.h"
#include "Project.h"

class HSceneModel;
SceneTranslatorPluginManager::SceneTranslatorPluginManager()
{
}

// We don't need to delete the plugins, the SnipePlugin will 
// do it for us since it points to the same plugins
SceneTranslatorPluginManager::~SceneTranslatorPluginManager()
{
}

BOOL SceneTranslatorPluginManager::QueryImportFormat(SceneTranslatorPlugin **pOutEntry, String &strOutFileName )
{
   String strInitFileName = ::AfxGetApp()->GetProfileString("Import", "Scene Name", "BOGUS");
   if (strInitFileName == "BOGUS")
      strInitFileName.Empty();

   //Build filter string now
   String strAllFormats("All Formats|");
   String strFilters;
   BOOL bFirstFilter = TRUE;
   for (POSITION pos = m_list.GetHeadPosition(); pos; ) {
      SceneTranslatorPlugin *entry = (SceneTranslatorPlugin *)m_list.GetNext(pos);
      if (entry->GetImportCallback() == NULL)
         continue;
      
      String strCurrFilter("%1 (%2)|%2|");
      strCurrFilter.Replace("%1", entry->GetFilterName());
      StringList *extlist = entry->GetExtension().Split(';');
      String allextensions;
      for (POSITION pos = extlist->GetHeadPosition(); pos; ) {
         if (!allextensions.IsEmpty())
            allextensions += ';';
         allextensions += "*."+extlist->GetNext(pos);
      }
      delete extlist;
      strCurrFilter.Replace("%2", allextensions);

      strFilters+=strCurrFilter;
      
      if (bFirstFilter)
         bFirstFilter = FALSE;
      else
         strAllFormats += ';';

      strAllFormats += allextensions;
   }
   strFilters = strAllFormats + '|' + strFilters + '|';

   int nIndex;
   if (!GetOpenFileName( strInitFileName, strFilters, strOutFileName, NULL, &nIndex ))
      return FALSE;

   ::AfxGetApp()->WriteProfileString("Import", "Scene Name", strOutFileName.Get());

   if (nIndex == 1) { // All Formats
      String strSuffix = strOutFileName.GetSuffixOnly();
      for (POSITION pos = m_list.GetHeadPosition(); pos; ) {
         SceneTranslatorPlugin *entry = (SceneTranslatorPlugin *)m_list.GetNext(pos);
         if (entry->GetImportCallback() == NULL)
            continue;

         if (((String)entry->GetExtension()).Find(strSuffix)!= -1) {
            *pOutEntry = entry;
            return TRUE;
         }
      }
      return FALSE;
   }
   else {
      int nCount = 2;
      for (POSITION pos = m_list.GetHeadPosition(); pos; ) {
         SceneTranslatorPlugin *entry = (SceneTranslatorPlugin *)m_list.GetNext(pos);
         if (entry->GetImportCallback() == NULL)
            continue;
         if (nCount == nIndex) {
            *pOutEntry = entry;
            return TRUE;
         }
         nCount++;
      }
      return FALSE;
   }
   return FALSE;
}

BOOL SceneTranslatorPluginManager::QueryExportFormat(Scene *pScene, SceneTranslatorPlugin **pOutEntry, String &strOutFileName )
{
   String strInitialName = pScene->GetMatchName().GetNameOnlyWithoutSuffix();
   String strLastPath = ::AfxGetApp()->GetProfileString("Export", "Path", "BOGUS");
   String strTemp = ::AfxGetApp()->GetProfileString("Export", "ClassName", "BOGUS");
   String strPluginName;
   String strClassName;

   if (strLastPath != "BOGUS")
      strInitialName = strLastPath + "\\" + strInitialName;

   if (strTemp != "BOGUS") 
   {
      int nIndex = strTemp.Find('|');
      if (nIndex != -1) 
      {
         strPluginName = strTemp.Left(nIndex);
         nIndex++;
         strClassName = strTemp.Right(strTemp.Length()-nIndex);
      }
   }

   //Build filter string now
   String strFilters;
   BOOL bFirstFilter = TRUE;

   int nInitFilterIndex = 1;
   int nCurrentFilterIndex = 1;
   for (POSITION pos = m_list.GetHeadPosition(); pos; ) {
      SceneTranslatorPlugin *entry = (SceneTranslatorPlugin *)m_list.GetNext(pos);
      if (entry->GetExportCallback() == NULL)
         continue;
      
      if (!pScene->IsKindOf(entry->GetSOT()))
         continue;

      if (entry->GetSnipePlugin()->GetName() == strPluginName && entry->GetClassName() == strClassName)
         nInitFilterIndex = nCurrentFilterIndex;

      String strCurrFilter("%1 (%2)|%2|");
      strCurrFilter.Replace("%1", entry->GetFilterName());
      StringList *extlist = entry->GetExtension().Split(';');
      String allextensions;
      for (POSITION pos = extlist->GetHeadPosition(); pos; ) {
         if (!allextensions.IsEmpty())
            allextensions += ';';
         allextensions += "*."+extlist->GetNext(pos);
      }
      delete extlist;
      strCurrFilter.Replace("%2", allextensions);
      strFilters+=strCurrFilter;
      nCurrentFilterIndex++;
   }
   strFilters += '|';

   int nIndex;
   if (!GetSaveFileName(strInitialName, strFilters, strOutFileName, NULL, nInitFilterIndex, &nIndex))
      return FALSE;

   ::AfxGetApp()->WriteProfileString("Export", "Path", strOutFileName.GetPathOnly().Get());

   int nCount = 1;
   for (POSITION pos = m_list.GetHeadPosition(); pos; ) {
      SceneTranslatorPlugin *entry = (SceneTranslatorPlugin *)m_list.GetNext(pos);
      if (entry->GetExportCallback() == NULL)
         continue;
      if (!pScene->IsKindOf(entry->GetSOT()))
         continue;
      if (nCount == nIndex) {
         *pOutEntry = entry;
         String strClassName = entry->GetSnipePlugin()->GetName() + "|" + entry->GetClassName();
         ::AfxGetApp()->WriteProfileString("Export", "ClassName", strClassName.Get());
         return TRUE;
      }
      nCount++;
   }
   return FALSE;
}

BOOL SceneTranslatorPluginManager::Export( Scene *pScene )
{
   String strFileName;
   SceneTranslatorPlugin *entry;

   if (!QueryExportFormat(pScene, &entry, strFileName))
      return TRUE;

   String strOldBasePath = SetBasePath( strFileName.GetPathOnly() );

   try {
      entry->GetExportCallback()(entry->GetPluginData(), pScene, strFileName.Get());
   }
   catch(const String &strReason)
   {
      g_outputWindow << entry->FormatError("Export", strReason) << "\n";
      SetBasePath( strOldBasePath );
      return FALSE;
   }
   catch(...) {
      g_outputWindow << entry->FormatError("Export", "Unknown") << "\n";
      SetBasePath( strOldBasePath );
      return FALSE;
   }

   SetBasePath( strOldBasePath );
   return TRUE;
}

Scene *SceneTranslatorPluginManager::Import()
{
   MirrorModePusher pusher(FALSE);

   String strFileName;
   SceneTranslatorPlugin *entry;

   if (!QueryImportFormat(&entry, strFileName))
      return NULL;

   AfxGetMainWnd()->UpdateWindow();

   SnipeObjectType nSOT = entry->GetSOT();
   ASSERT(nSOT!=SOT_Unknown);

   SnipeObjectInfo *pInfo = SnipeObjectInfo::GetBySOT(nSOT);

   if (pInfo==NULL)
   {
      ASSERT(pInfo);
      return NULL;
   }

   CWaitCursor waitcursor;
   Project *pProject = GetMainInterface()->CreateProject();
   Scene *pScene = pProject->AddScene();

   ASSERT(dynamic_cast<Scene *>(pScene));

   String strOldBasePath = SetBasePath( strFileName.GetPathOnly() );

   try
   {
      OutputWindowTimer timer( "Imported " + strFileName + " in %1." );
      entry->GetImportCallback()(entry->GetPluginData(), pScene, strFileName.Get());

      pScene->SetName( strFileName.GetNameOnlyWithoutSuffix());

      GetMainInterface()->GetProject()->NotifyEndProjectLoad();

      SetBasePath( strOldBasePath );
   }
   catch(const String &strReason)
   {
      g_outputWindow << entry->FormatError("Import", strReason) << "\n";
      delete pScene;
      SetBasePath( strOldBasePath );
      return NULL;
   }
   catch(...)
   {
      g_outputWindow << entry->FormatError("Import", "Unknown") << "\n";
      delete pScene;
      SetBasePath( strOldBasePath );
      return NULL;
   }

   return pScene;
}
