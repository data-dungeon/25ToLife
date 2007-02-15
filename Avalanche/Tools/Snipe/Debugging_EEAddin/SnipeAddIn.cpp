
#include "SnipeAddIn.h"

// Helper routine that converts a system time into ASCII

ADDIN_API HRESULT WINAPI AddIn_HComponent( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t max, DWORD reserved )
{
	BYTE component[ sizeof(Component) ];
	DWORD nGot;

	// read system time from debuggee memory space
	if (pHelper->ReadDebuggeeMemoryEx(pHelper, pHelper->GetRealAddress(pHelper), sizeof(Component), component, &nGot)!=S_OK)
      return E_FAIL;
	if (nGot!=sizeof(Component))
      return E_FAIL;

   Component *pComponent = (Component *)component;
   sprintf( pResult, "id=%d", pComponent->GetID() );
   return S_OK;
}
