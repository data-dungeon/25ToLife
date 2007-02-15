// Dan  12/28/00  \Dan90\Plugin\HXT\KFM\Entry.cpp
#include "stdafx.h"
#include "Resource.h"
#include "Hxt.h"
#include "SDK\HHashObj.h"
#include "SDK\Entry.h"

#include "SDK\HAction.h"

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

enum {EXPORT_ACTIONCACHE, IMPORT_ACTIONCACHE, EXPORT_ACTIONCACHECONTAINER, IMPORT_ACTIONCACHECONTAINER };

extern "C" __declspec(dllexport) const BOOL HxtLoadCommandEntry(UINT index, ObjectType &objtype)
{
   switch (index) {
   case EXPORT_ACTIONCACHE:
      objtype = HOT_ACTION;
      return TRUE;
   case IMPORT_ACTIONCACHE:
      objtype = HOT_ACTION;
      return TRUE;
   case EXPORT_ACTIONCACHECONTAINER:
      objtype = HOT_ACTIONCACHECONTAINER;
      return TRUE;
   case IMPORT_ACTIONCACHECONTAINER:
      objtype = HOT_ACTIONCACHECONTAINER;
      return TRUE;
   }
   return FALSE;
}

extern "C" __declspec(dllexport) BOOL HxtOnAddCommandMenu(HTreeObject *htreeobject, UINT index, String &menuname, MenuCategory &mc, BOOL &disabled)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CString string;
   string.LoadString(IDS_MENUFILTER);
   menuname = (LPCTSTR)string;

   disabled = FALSE;

   switch (index) {
   case EXPORT_ACTIONCACHE:
      if (!((HAnimObject *)htreeobject)->IsCache()) // Actually a Model, not a ModelCache
         return FALSE; 
      mc = MC_EXPORT;
      return TRUE;
   case IMPORT_ACTIONCACHE:
      if (!((HAnimObject *)htreeobject)->IsCache()) // Actually a Model, not a ModelCache
         return FALSE; 
      mc = MC_IMPORT;
      return TRUE;
   case EXPORT_ACTIONCACHECONTAINER:
      mc = MC_EXPORT;
      return TRUE;
   case IMPORT_ACTIONCACHECONTAINER:
      mc = MC_IMPORT;
      return TRUE;
   }
   return FALSE;
}


extern "C" __declspec(dllexport) BOOL HxtOnCommand(HTreeObject *htreeobject, UINT index)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

   switch (index) {
   case EXPORT_ACTIONCACHE:
      return ((CHxtApp *)AfxGetApp())->OnExportActionCache((HActionCache *)htreeobject);
   case IMPORT_ACTIONCACHE:
      return ((CHxtApp *)AfxGetApp())->OnImportActionCache((HActionCache *)htreeobject);
   case EXPORT_ACTIONCACHECONTAINER:
      return ((CHxtApp *)AfxGetApp())->OnExportActionCacheContainer((HActionCacheContainer *)htreeobject);
   case IMPORT_ACTIONCACHECONTAINER:
      return ((CHxtApp *)AfxGetApp())->OnImportActionCacheContainer((HActionCacheContainer *)htreeobject);
   }
   return FALSE;
}
