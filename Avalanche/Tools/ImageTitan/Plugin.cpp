// Plugin.cpp
#include "stdafx.h"
#include "ImageTitan.h"
#include "Plugin.h"

//*** PluginReg **********************************************************
ImagePluginReg::ImagePluginReg(HINSTANCE hinstance, const String &strFileName, const String &strPluginClassName ) :
  m_hinstance(hinstance),
  m_strFileName(strFileName),
  m_strPluginClassName(strPluginClassName)
{
}

ImagePluginReg::~ImagePluginReg()
{
//Don't delete ImagePluginReg's.  Most likely they are alocated on the stack
//in the plugin.  Regardless, it is the plugins job to dispose of them.
}

ImagePluginReg &ImagePluginReg::operator = (const ImagePluginReg &other)
{
   m_strFileName = other.m_strFileName;
   m_strPluginClassName = other.m_strPluginClassName;
   m_hinstance = other.m_hinstance;

   m_entries.RemoveAll();

   for (int i=0; i < other.m_entries.GetCount(); i++)
      m_entries.SetAt(i, other.m_entries[i]);
   return *this;
}

ImageFormatEntry *ImagePluginReg::FindEntryBySuffix(const String &strMatchSuffix)
{
   for ( int i=0; i < m_entries.GetCount(); i++) {
      ImageFormatEntry *entry = m_entries[i];
      String strSuffix(entry->m_szExtension);
      if (strSuffix.Find(strMatchSuffix))
         return entry;
   }
   return NULL;
}

ImageFormatEntry *ImagePluginReg::FindEntryByClassName(const String &strClassName)
{
   for ( int i=0; i < m_entries.GetCount(); i++) {
      ImageFormatEntry *entry = m_entries[i];
      if (strClassName == entry->m_szClassName)
         return entry;
   }
   return NULL;
}

//***********************************************************************************************************************

void CImageTitanApp::FreeRegisteredPlugins()
{
   for (POSITION pos = m_registration.GetHeadPosition(); pos; )
      delete m_registration.GetNext(pos);
}

void CImageTitanApp::RegisterPlugins()
{
   String pluginfolder;

   pluginfolder = String::GetAppFolder();
   pluginfolder.AppendName("Image Translators\\*.itr");

   StringList *list = pluginfolder.FindFiles();

   if (list) {
      for (POSITION pos = list->GetHeadPosition(); pos; ) {
         String strFileName = list->GetNext(pos);
         
         HINSTANCE hinst;
         if (hinst = LoadLibrary( strFileName.Get())) {
            void (*lpfnInitImageTranslator)(char **strPluginName, ImageFormatEntry **pEntries, int *nCount);
            (FARPROC &) lpfnInitImageTranslator  = GetProcAddress( hinst, "InitImageTranslator");

            if (lpfnInitImageTranslator) {
               int nCount;
               char *strPluginName;
               ImageFormatEntry *pEntries;
               lpfnInitImageTranslator(&strPluginName, &pEntries, &nCount);
               if (nCount) {
                  ImagePluginReg *reg = SNEW ImagePluginReg(hinst, strFileName, strPluginName);
                  reg->m_entries.SetSize(nCount);
                  for (int i=0; i < nCount; i++) {
                     ImageFormatEntry *entry = &pEntries[i];
                     reg->m_entries.SetAt(i, entry);
                  }
                  m_registration.AddTail(reg);
               }
            }
         }
        else {
//            g_outputWindow << "Fatal Error: Unable to load plugin [" << strFileName<< "].\n";
         }
       }
      delete list;
   }
}