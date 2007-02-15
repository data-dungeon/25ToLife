#include "stdafx.h"
#include "ModelTranslatorManager.h"
#include "PolyModel.h"
#include "../interface/MainInterface.h"
#include "../gui/Snipe.h"
#include "ModelTranslatorEntry.h"

class HPolyModel;
ModelTranslatorManager::ModelTranslatorManager()
{
}

// We don't need to delete the entries, the SnipePlugin will 
// do it for us since it points to the same entries
ModelTranslatorManager::~ModelTranslatorManager()
{
}

BOOL ModelTranslatorManager::QueryFormat(ModelTranslatorEntry **pOutEntry, String &strOutFileName )
{
   String strLastFile = ::AfxGetApp()->GetProfileString("Import", "Model Name", "BOGUS");
   if (strLastFile == "BOGUS")
      strLastFile.Empty();

   //Build filter string now
   String strAllFormats("All Formats|");
   String strFilters;
   BOOL bFirstFilter = TRUE;
   for (POSITION pos = GetHeadPosition(); pos; ) {
      ModelTranslatorEntry *entry = GetNext(pos);
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

      strAllFormats +=(String)"*." + entry->GetExtension();
   }
   strFilters = strAllFormats + '|' + strFilters + '|';

   //Open File Dialog
   CFileDialog dlg(TRUE, NULL, strLastFile.Get(), OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,
    strFilters.Get(), AfxGetMainWnd());

   if (dlg.DoModal()!=IDOK)
      return FALSE;

   strOutFileName = dlg.GetPathName();
   ::AfxGetApp()->WriteProfileString("Import", "Model Name", strOutFileName.Get());

   int nIndex = dlg.m_ofn.nFilterIndex;

   if (nIndex == 1) { // All Formats
      String strSuffix = strOutFileName.GetSuffixOnly();
      for (POSITION pos = GetHeadPosition(); pos; ) {
         ModelTranslatorEntry *entry = GetNext(pos);
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
      for (POSITION pos = GetHeadPosition(); pos; ) {
         ModelTranslatorEntry *entry = GetNext(pos);
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

PolyModel *ModelTranslatorManager::Import()
{
   String strFileName;
   ModelTranslatorEntry *entry;

   if (!QueryFormat(&entry, strFileName))
      return NULL;

   UINT id;
   switch (entry->GetModelType()) {
      case ModelTranslatorEntry::MT_POLYMODEL:
         id = SOT_PolyModel;
         break;
      //case ModelTranslatorEntry::MT_PATCHMODEL:
      //   id = SOT_PatchModel;
      //   break;
      case ModelTranslatorEntry::MT_SUBDMODEL:
         id = SOT_SubDModel;
         break;
      case ModelTranslatorEntry::MT_BEZIERSPLINEMODEL:
         id = SOT_BezierSplineModel;
         break;
      default:
         return NULL;
   }

   MainInterface *mi = GetMainInterface();
   PolyModel *pModel = (PolyModel *)mi->CreateObjectByID(id);

   ASSERT(dynamic_cast<PolyModel *>(pModel));

   try {
      entry->GetImportCallback()(entry->GetPluginEntryHelper(), pModel, strFileName.Get());
   }
   catch(const String &strReason)
   {
      g_outputWindow << entry->FormatError("Import", strReason) << "\n";
      delete pModel;
      return NULL;
   }
   catch(...) {
      g_outputWindow << entry->FormatError("Import", "Unknown") << "\n";
      delete pModel;
      return NULL;
   }

   pModel->m_strName = strFileName;

   return pModel;
}

String ModelTranslatorEntry::GetClassType() const
{
   return "Model Translator";
}
