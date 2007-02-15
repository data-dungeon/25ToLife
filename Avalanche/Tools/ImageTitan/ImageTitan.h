// Image.h : main header file for the Image DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "Plugin.h"

// CImageApp
// See Image.cpp for the implementation of this class
//

class CImageTitanApp : public CWinApp
{
public:
   CList<ImagePluginReg *, ImagePluginReg *> m_registration;
	
   CImageTitanApp();
   ~CImageTitanApp();

   void RegisterPlugins();
   void FreeRegisteredPlugins();

   BOOL QueryFormat(ImageFormatEntry **pOutEntry, String &strOutFileName);

   ImageFormatEntry *FindEntry( const String &strSuffix );
   ImageFormatEntry *FindEntry( const String &strPluginClassname, const String &strFormatClassName );

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
