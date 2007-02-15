// Image.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ImageTitan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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

// CImageApp

BEGIN_MESSAGE_MAP(CImageTitanApp, CWinApp)
END_MESSAGE_MAP()


// CImageApp construction

CImageTitanApp::CImageTitanApp()
{
}


CImageTitanApp::~CImageTitanApp()
{
   FreeRegisteredPlugins();
}

// The one and only CImageApp object

CImageTitanApp g_theApp;


// CImageApp initialization

BOOL CImageTitanApp::InitInstance()
{
	CWinApp::InitInstance();

   RegisterPlugins();
	return TRUE;
}

ImageFormatEntry *CImageTitanApp::FindEntry(const String &strSuffix)
{
   for (POSITION pos = m_registration.GetHeadPosition(); pos; ) {
      ImagePluginReg *reg = m_registration.GetNext(pos);
      ImageFormatEntry *entry = reg->FindEntryBySuffix(strSuffix);
      if (entry)
         return entry;
   }
   return NULL;
}

ImageFormatEntry *CImageTitanApp::FindEntry(const String &strPluginClassName, const String &strFormatClassName)
{
   for (POSITION pos = m_registration.GetHeadPosition(); pos; ) {
      ImagePluginReg *reg = m_registration.GetNext(pos);
      if (reg->m_strPluginClassName == strPluginClassName) {
         ImageFormatEntry *entry = reg->FindEntryByClassName(strFormatClassName);
         if (entry)
            return entry;
      }
   }
   return NULL;
}
