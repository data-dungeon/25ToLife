//
#include "stdafx.h"
#include "ImageTitan.h"
#include "BitmapObject.h"

extern CImageTitanApp g_theApp;

ImageFormatEntry *BitmapObject::FindEntry(const String &strFileName)
{
   return g_theApp.FindEntry(strFileName.GetSuffixOnly());
}

ImageFormatEntry *BitmapObject::FindEntry(const String &strPluginClassName, const String &FormatClassName)
{
   return g_theApp.FindEntry(strPluginClassName, FormatClassName);
}

void BitmapObject::LoadImage(ImageFormatEntry *pEntry, const String &strFileName)
{
   if (m_strFileName == strFileName)
      return;

   try {
      pEntry->m_lpfnImport((HBitmapObject *)this, strFileName.Get());
   }
   catch(String msg) {
      String strError("Import Failed: pluginclass: %1, Msg: %2");
      strError.Replace("%1", pEntry->m_szClassName);
      strError.Replace("%2", msg);
      THROW_FATAL_ERROR(strError);
   }
   catch(...) {
      String strError("Import Failed: pluginclass: %1, filename: %2");
      strError.Replace("%1", pEntry->m_szClassName);
      strError.Replace("%2", strFileName);
      THROW_FATAL_ERROR(strError);
   }
   m_strFileName = strFileName;
}

void BitmapObject::LoadImage(const String &strFileName )
{
   ImageFormatEntry *pEntry = FindEntry(strFileName);
   if (pEntry == NULL) {
      String msg("No available plugin found to import [%1].");
      msg.Replace("%1", strFileName);
      THROW_FATAL_ERROR(msg);
   }
   LoadImage(pEntry, strFileName);
}

void BitmapObject::LoadImage(const String &strPluginClassName, const String &strFormatClassName, const String &strFileName)
{
   ImageFormatEntry *pEntry;
   
   pEntry = FindEntry(strPluginClassName, strFormatClassName);
   if (pEntry == NULL) // this plugin wasn't installed, lets check for a differnt one that might work
      pEntry = FindEntry(strFileName); 

   if (pEntry == NULL) {
      String msg("No available plugin found to import [%1].");
      msg.Replace("%1", strFileName);
      THROW_FATAL_ERROR(msg);
   }

   LoadImage(pEntry, strFileName);
}

BOOL BitmapObject::QueryLoadImage()
{
   String strFileName;
   ImageFormatEntry *pEntry;

   if (!g_theApp.QueryFormat(&pEntry, strFileName))
      return FALSE; // Must have cancelled from the Dialog

   LoadImage(pEntry, strFileName);
   return TRUE;
}

BOOL CImageTitanApp::QueryFormat(ImageFormatEntry **pOutEntry, String &strOutFileName)
{
   String strLastFile = ::AfxGetApp()->GetProfileString("Import", "Image Name", "BOGUS");
   if (strLastFile == "BOGUS")
      strLastFile.Empty();

   //Lets build one list of all entries that will be in the filters
   CList <ImageFormatEntry *> entrylist;
   for (POSITION pos = m_registration.GetHeadPosition(); pos; ) {
      ImagePluginReg *reg = m_registration.GetNext(pos);
      for (int i=0; i < reg->m_entries.GetCount(); i++) {
         ImageFormatEntry *entry = (ImageFormatEntry *)reg->m_entries[i];
         if (entry->m_lpfnImport == NULL)
            continue;
         entrylist.AddTail(entry);
      }
   }

   //Build filter string now
   String strAllFormats("All Formats|");
   String strFilters;
   BOOL bFirstFilter = TRUE;
   for (POSITION pos = entrylist.GetHeadPosition(); pos; ) {
      ImageFormatEntry *entry = entrylist.GetNext(pos);
      
      String strCurrFilter("%1 (%2)|%2|");
      strCurrFilter.Replace("%1", (String)entry->m_szFilterName);
      StringList *extlist = String(entry->m_szExtension).Split(';');
      String allextensions;
      for (POSITION pos = extlist->GetHeadPosition(); pos; ) {
         if (!allextensions.IsEmpty())
            allextensions += ';';
         allextensions += "*." + extlist->GetNext(pos);
      }
      delete extlist;
      strCurrFilter.Replace("%2", allextensions);

      strFilters+=strCurrFilter;
      
      if (bFirstFilter)
         bFirstFilter = FALSE;
      else
         strAllFormats += ';';

      strAllFormats +=(String)"*." + entry->m_szExtension;
   }
   strFilters = strAllFormats + '|' + strFilters + '|';

   //Open File Dialog
   CFileDialog dlg(TRUE, NULL, strLastFile.Get(), OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,
    strFilters.Get(), AfxGetMainWnd());

   if (dlg.DoModal()!=IDOK)
      return FALSE;

   strOutFileName = dlg.GetPathName();
   ::AfxGetApp()->WriteProfileString("Import", "Image Name", strOutFileName.Get());

   int nIndex = dlg.m_ofn.nFilterIndex;

   if (nIndex == 1) { // All Formats
      String strSuffix = strOutFileName.GetSuffixOnly();
      for (POSITION pos = entrylist.GetHeadPosition(); pos; ) {
         ImageFormatEntry *entry = (ImageFormatEntry *)entrylist.GetNext(pos);
         if (((String)entry->m_szExtension).Find(strSuffix)!= -1) {
            *pOutEntry = entry;
            return TRUE;
         }
      }
      String strMsg("No plugin available to load [ %1 ]. Err #1");
      strMsg.Replace("%1", strOutFileName);
      THROW_FATAL_ERROR(strMsg);
   }
   else {
      int nCount = 2;
      for (POSITION pos = entrylist.GetHeadPosition(); pos; ) {
         ImageFormatEntry *entry = (ImageFormatEntry *)entrylist.GetNext(pos);
         if (nCount == nIndex) {
            *pOutEntry = entry;
            return TRUE;
         }
         nCount++;
      }
      String strMsg("No plugin available to load [ %1 ]. Err #2");
      strMsg.Replace("%1", strOutFileName);
      THROW_FATAL_ERROR(strMsg);
   }
   return FALSE;
}
