/*******************************************************************************
 HoldVC.cpp
 Contains the Dll Entry stuff

 Author   :   Dave Astle
 Created  :   1/7/2002
  
 Copyright (C) 2002 Avalanche Software 
*******************************************************************************/
#include "HoldVC.h"

extern ClassDesc2* GetHoldVCUtilDesc();
extern ClassDesc2* GetHoldVCModDesc();

HINSTANCE g_hInstance;
int g_controlsInit = FALSE;


BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved)
{
  g_hInstance = hinstDLL;           // Hang on to this DLL's instance handle.

  if (!g_controlsInit)
  {
    g_controlsInit = TRUE;
    InitCustomControls(g_hInstance);  // Initialize MAX's custom controls
    InitCommonControls();           // Initialize Win95 controls
  }
		  
  return (TRUE);
}


__declspec( dllexport ) const TCHAR* LibDescription()
{
  return GetString(IDS_LIBDESCRIPTION);
}


__declspec( dllexport ) int LibNumberClasses()
{
  return 2;
}


__declspec( dllexport ) ClassDesc* LibClassDesc(int i)
{
  switch(i)
  {
    case 0: return GetHoldVCUtilDesc();
    case 1: return GetHoldVCModDesc();
    default: return 0;
  }
}


__declspec( dllexport ) ULONG LibVersion()
{
  return VERSION_3DSMAX;
}


TCHAR *GetString(int id)
{
  static TCHAR buf[256];

  if (g_hInstance)
    return LoadString(g_hInstance, id, buf, sizeof(buf)) ? buf : NULL;
  return NULL;
}