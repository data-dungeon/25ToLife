#include "stdafx.h"
#include "MotionTranslatorPluginManager.h"
#include "Motion.h"
#include "../interface/MainInterface.h"
#include "../gui/Snipe.h"
#include "MotionTranslatorPlugin.h"
#include "../Utility/FileDialog.h"
#include "SnipePlugin.h"
#include "Project.h"

class HMotion;
MotionTranslatorPluginManager::MotionTranslatorPluginManager()
{
}

// We don't need to delete the plugins, the SnipePlugin will 
// do it for us since it points to the same plugins
MotionTranslatorPluginManager::~MotionTranslatorPluginManager()
{
}

BOOL MotionTranslatorPluginManager::QueryImportFormat( MotionTranslatorPlugin **pOutEntry, StringList &strOutFileNames )
{
   *pOutEntry = NULL;

   String strInitFileName = ::AfxGetApp()->GetProfileString("Import", "Motion Name", "BOGUS");
   if (strInitFileName == "BOGUS")
      strInitFileName.Empty();

   //Build filter string now
   String strAllFormats("All Formats|");
   String strFilters;
   BOOL bFirstFilter = TRUE;
   for (POSITION pos = m_list.GetHeadPosition(); pos; )
   {
      MotionTranslatorPlugin *entry = (MotionTranslatorPlugin *)m_list.GetNext(pos);
      if (entry->GetImportCallback() == NULL)
         continue;
      
      String strCurrFilter("%1 (%2)|%2|");
      strCurrFilter.Replace("%1", entry->GetFilterName());
      StringList *extlist = entry->GetExtension().Split(';');
      String allextensions;
      for (POSITION pos = extlist->GetHeadPosition(); pos; )
      {
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
   if (!GetOpenFileName( strInitFileName, strFilters, strOutFileNames, NULL, &nIndex ))
      return FALSE;
   AfxGetMainWnd()->UpdateWindow();

   if (strOutFileNames.IsEmpty())
      return FALSE;

   String &strFileName = strOutFileNames.GetHead();
   ::AfxGetApp()->WriteProfileString( "Import", "Motion Name", strFileName.Get() );

   if (nIndex > 1) // user chose a specific importer
   {
      int nCount = 2;
      for (POSITION pos = m_list.GetHeadPosition(); pos; )
      {
         MotionTranslatorPlugin *entry = (MotionTranslatorPlugin *)m_list.GetNext(pos);
         if (entry->GetImportCallback() == NULL)
            continue;
         if (nCount == nIndex)
         {
            *pOutEntry = entry;
            return TRUE;
         }
         nCount++;
      }
   }

   return TRUE;
}

MotionTranslatorPlugin *MotionTranslatorPluginManager::FindEntryFromSuffix( const String &strSuffix )
{
   for (POSITION pos = m_list.GetHeadPosition(); pos; )
   {
      MotionTranslatorPlugin *pEntry = (MotionTranslatorPlugin *)m_list.GetNext(pos);
      if (pEntry->GetImportCallback() == NULL)
         continue;

      if (((String)pEntry->GetExtension()).Find(strSuffix) != -1)
         return pEntry;
   }

   return NULL;
}

BOOL MotionTranslatorPluginManager::QueryExportFormat(Motion *pMotion, MotionTranslatorPlugin **pOutEntry, String &strOutFileName )
{
   String strInitialName = pMotion->GetMatchName().GetNameOnlyWithoutSuffix();
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
      MotionTranslatorPlugin *entry = (MotionTranslatorPlugin *)m_list.GetNext(pos);
      if (entry->GetExportCallback() == NULL)
         continue;
      
      if (!pMotion->IsKindOf(entry->GetSOT()))
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
      MotionTranslatorPlugin *entry = (MotionTranslatorPlugin *)m_list.GetNext(pos);
      if (entry->GetExportCallback() == NULL)
         continue;
      if (!pMotion->IsKindOf(entry->GetSOT()))
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

BOOL MotionTranslatorPluginManager::Export( Motion *pMotion )
{
   String strFileName;
   MotionTranslatorPlugin *entry;

   if (!QueryExportFormat(pMotion, &entry, strFileName))
      return TRUE;

   String strOldBasePath = SetBasePath( strFileName.GetPathOnly() );

   try {
      entry->GetExportCallback()(entry->GetPluginData(), pMotion, strFileName.Get());
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

SnipeObjectList *MotionTranslatorPluginManager::Import( StringList &strFileNames )
{
   MirrorModePusher pusher(FALSE);
   MotionTranslatorPlugin *pUserChosenEntry = NULL;

   if (strFileNames.IsEmpty())
   {
      if (!QueryImportFormat( &pUserChosenEntry, strFileNames ))
         return NULL;
   }

   CWaitCursor waitcursor;
   Project *pProject = GetMainInterface()->CreateProject();
   SnipeObjectList *pMotionList = SNEW SnipeObjectList;

   for (POSITION pos=strFileNames.GetHeadPosition(); pos; )
   {
      String &strFileName = strFileNames.GetNext( pos );

      MotionTranslatorPlugin *pEntry = pUserChosenEntry;
      if (!pEntry)
      {
         pEntry = FindEntryFromSuffix( strFileName.GetSuffixOnly() );
         if (!pEntry)
            continue;
      }

      SnipeObjectType nSOT = pEntry->GetSOT();
      ASSERT(nSOT!=SOT_Unknown);

      SnipeObjectInfo *pInfo = SnipeObjectInfo::GetBySOT( nSOT );

      if (pInfo==NULL)
      {
         ASSERT(FALSE);
         continue;
      }

      Motion *pMotion = pProject->AddMotion();
      pMotion->GetFileInfoProperty()->GetFileNameProperty()->StoreValue( strFileName );

      String strOldBasePath = SetBasePath( strFileName.GetPathOnly() );

      try
      {
         OutputWindowTimer timer( "Imported " + strFileName + " in %1." );
         pEntry->GetImportCallback()( pEntry->GetPluginData(), pMotion, strFileName.Get() );
         pMotion->SetName( strFileName.GetNameOnlyWithoutSuffix() );
         pMotionList->AddTail( pMotion );
      }
      catch(const String &strReason)
      {
         g_outputWindow << pEntry->FormatError("Import", strReason) << "\n";
         delete pMotion;
      }
      catch(...)
      {
         g_outputWindow << pEntry->FormatError("Import", "Unknown") << "\n";
         delete pMotion;
      }

      SetBasePath( strOldBasePath );
   }

   return pMotionList;
}
