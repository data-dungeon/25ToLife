#include "stdafx.h"
#include "ModelTranslatorPluginManager.h"
#include "PolyModel.h"
#include "../interface/MainInterface.h"
#include "../gui/Snipe.h"
#include "ModelTranslatorPlugin.h"
#include "../Utility/FileDialog.h"
#include "SnipePlugin.h"
#include "Project.h"

class HPolyModel;
ModelTranslatorPluginManager::ModelTranslatorPluginManager()
{
}

// We don't need to delete the plugins, the SnipePlugin will 
// do it for us since it points to the same plugins
ModelTranslatorPluginManager::~ModelTranslatorPluginManager()
{
}

BOOL ModelTranslatorPluginManager::QueryImportFormat(ModelTranslatorPlugin **pOutEntry, String &strOutFileName )
{
   String strInitFileName = ::AfxGetApp()->GetProfileString("Import", "Model Name", "BOGUS");
   if (strInitFileName == "BOGUS")
      strInitFileName.Empty();

   //Build filter string now
   String strAllFormats("All Formats|");
   String strFilters;
   BOOL bFirstFilter = TRUE;
   for (POSITION pos = m_list.GetHeadPosition(); pos; ) {
      ModelTranslatorPlugin *entry = (ModelTranslatorPlugin *)m_list.GetNext(pos);
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

   ::AfxGetApp()->WriteProfileString("Import", "Model Name", strOutFileName.Get());

   if (nIndex == 1) { // All Formats
      String strSuffix = strOutFileName.GetSuffixOnly();
      for (POSITION pos = m_list.GetHeadPosition(); pos; ) {
         ModelTranslatorPlugin *entry = (ModelTranslatorPlugin *)m_list.GetNext(pos);
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
         ModelTranslatorPlugin *entry = (ModelTranslatorPlugin *)m_list.GetNext(pos);
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

BOOL ModelTranslatorPluginManager::QueryExportFormat(PolyModel *pModel, ModelTranslatorPlugin **pOutEntry, String &strOutFileName )
{
   String strInitialName = pModel->GetMatchName().GetNameOnlyWithoutSuffix();
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
      ModelTranslatorPlugin *entry = (ModelTranslatorPlugin *)m_list.GetNext(pos);
      if (entry->GetExportCallback() == NULL)
         continue;
      
      if (!pModel->IsKindOf(entry->GetSOT()))
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
      ModelTranslatorPlugin *entry = (ModelTranslatorPlugin *)m_list.GetNext(pos);
      if (entry->GetExportCallback() == NULL)
         continue;
      if (!pModel->IsKindOf(entry->GetSOT()))
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

BOOL ModelTranslatorPluginManager::Export( PolyModel *pModel )
{
   String strFileName;
   ModelTranslatorPlugin *entry;

   if (!QueryExportFormat(pModel, &entry, strFileName))
      return TRUE;

   String strOldBasePath = SetBasePath( strFileName.GetPathOnly() );

   try {
      entry->GetExportCallback()(entry->GetPluginData(), pModel, strFileName.Get());
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

PolyModel *ModelTranslatorPluginManager::Import()
{
   MirrorModePusher pusher(FALSE);

   String strFileName;
   ModelTranslatorPlugin *entry;

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
   PolyModel *pModel = (PolyModel *)pInfo->CreateObject();// pProject->AddPolyModel();

   ASSERT(dynamic_cast<PolyModel *>(pModel));

   String strOldBasePath = SetBasePath( strFileName.GetPathOnly() );

   pProject->m_modelcontainer.AddChildAtTail( pModel );

   try
   {
      OutputWindowTimer timer( "Imported " + strFileName + " in %1." );
      entry->GetImportCallback()(entry->GetPluginData(), pModel, strFileName.Get());

      pModel->SetName( strFileName.GetNameOnlyWithoutSuffix());

      GetMainInterface()->GetProject()->NotifyEndProjectLoad();

      SetBasePath( strOldBasePath );
   }
   catch(const String &strReason)
   {
      g_outputWindow << entry->FormatError("Import", strReason) << "\n";
      delete pModel;
      SetBasePath( strOldBasePath );
      return NULL;
   }
   catch(...)
   {
      g_outputWindow << entry->FormatError("Import", "Unknown") << "\n";
      delete pModel;
      SetBasePath( strOldBasePath );
      return NULL;
   }


   return pModel;
}
