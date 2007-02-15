// Ketapi32.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Ketapi32.h"

#define APP_NAME "fake Ketapi32.dll"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
//			MessageBox( NULL, APP_NAME " DllMain, PROCESS_ATTACH", APP_NAME, MB_OK );
			break;
		case DLL_THREAD_ATTACH:
//			MessageBox( NULL, APP_NAME " DllMain, THREAD_ATTACH", APP_NAME, MB_OK );
			break;
		case DLL_THREAD_DETACH:
//			MessageBox( NULL, APP_NAME " DllMain, THREAD_DETATCH", APP_NAME, MB_OK );
			break;
		case DLL_PROCESS_DETACH:
//			MessageBox( NULL, APP_NAME " DllMain, PROCESS_DETATCH", APP_NAME, MB_OK );
			break;
	}
	return TRUE;
}