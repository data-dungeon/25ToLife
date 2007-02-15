#pragma once

class HBitmapObject;

typedef BOOL (* PFN_IMAGEIMPORT) (HBitmapObject *, const char *szFileName);
typedef BOOL (* PFN_IMAGEEXPORT) (HBitmapObject *, const char *szFileName);

class ImageFormatEntry
{
public:
   char *m_szClassName; // Like LIKE TARGA, stored in files for recreation
   char *m_szFilterName; //LIKE Targa
   char *m_szExtension; //LIKE tga
   PFN_IMAGEIMPORT m_lpfnImport;
   PFN_IMAGEEXPORT m_lpfnExport;

   ImageFormatEntry(char *szClassName, char *szFilterName, char *szExtension, PFN_IMAGEIMPORT lpfnImport, PFN_IMAGEEXPORT lpfnExport) :
     m_szClassName(szClassName),
     m_szFilterName(szFilterName),
     m_szExtension(szExtension),
     m_lpfnImport(lpfnImport),
     m_lpfnExport(lpfnExport)
   {
   }
};