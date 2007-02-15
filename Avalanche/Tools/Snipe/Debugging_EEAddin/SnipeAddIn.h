
#include "StdAfx.h"
#include "custview.h"
#include "../Objects/Component.h"

#define ADDIN_API __declspec(dllexport)

extern "C" ADDIN_API HRESULT WINAPI AddIn_HComponent( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t max, DWORD reserved );
extern "C" ADDIN_API HRESULT WINAPI AddIn_HVertex( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t max, DWORD reserved );
extern "C" ADDIN_API HRESULT WINAPI AddIn_HEdge( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t max, DWORD reserved );
extern "C" ADDIN_API HRESULT WINAPI AddIn_HPoly( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t max, DWORD reserved );
