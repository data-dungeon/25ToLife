// Dan  12/28/00  \Dan90\Plugin\HXT\Ply\Entry.cpp
#include "stdafx.h"
#include "BasicImagePlugin.h"
#include "ImageFormatEntry.h"

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

extern CItrApp g_theApp;

BOOL ImportImage(HBitmapObject *pBitmapObject, const char *strFileName) { 
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return g_theApp.ImportImage(pBitmapObject, strFileName); 
}

//Import BMP, GIF, JPEG, PCX, PGM, PICT, PNG, TGA, TIFF and WMF
//Export BMP, JPEG, PNG and TIFF

static ImageFormatEntry f_entries[] = 
{
   ImageFormatEntry("PAINTLIB_BMP",  "BMP" ,"BMP;RLE;DIB", (PFN_IMAGEIMPORT)ImportImage, (PFN_IMAGEEXPORT)NULL),
   ImageFormatEntry("PAINTLIB_GIF",  "GIF" ,"GIF", (PFN_IMAGEIMPORT)ImportImage, (PFN_IMAGEEXPORT)NULL),
   ImageFormatEntry("PAINTLIB_JPEG", "JPEG", "JPG;JPEG;JPE", (PFN_IMAGEIMPORT)ImportImage, (PFN_IMAGEEXPORT)NULL),
   ImageFormatEntry("PAINTLIB_PCX",  "PCX" ,"PCX", (PFN_IMAGEIMPORT)ImportImage, (PFN_IMAGEEXPORT)NULL),
   ImageFormatEntry("PAINTLIB_PGM",  "PGM" ,"PGM", (PFN_IMAGEIMPORT)ImportImage, (PFN_IMAGEEXPORT)NULL),
   ImageFormatEntry("PAINTLIB_PICT", "Pict File","PICT;PCT", (PFN_IMAGEIMPORT)ImportImage, (PFN_IMAGEEXPORT)NULL),
   ImageFormatEntry("PAINTLIB_PNG",  "PNG" ,"PNG", (PFN_IMAGEIMPORT)ImportImage, (PFN_IMAGEEXPORT)NULL),
   ImageFormatEntry("PAINTLIB_TGA",  "Targa","TGA", (PFN_IMAGEIMPORT)ImportImage, (PFN_IMAGEEXPORT)NULL),
   ImageFormatEntry("PAINTLIB_TIFF", "TIFF","TIFF;TIF", (PFN_IMAGEIMPORT)ImportImage, (PFN_IMAGEEXPORT)NULL),
   ImageFormatEntry("PAINTLIB_WMF",  "WMF" ,"WMF", (PFN_IMAGEIMPORT)ImportImage, (PFN_IMAGEEXPORT)NULL)
};

extern "C" __declspec(dllexport) void InitImageTranslator(char **strPluginName, ImageFormatEntry **entries, int &nCount)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
   *strPluginName = "PAINTLIB";
   *entries = f_entries;
   nCount = sizeof(f_entries)/sizeof(ImageFormatEntry); 
}
