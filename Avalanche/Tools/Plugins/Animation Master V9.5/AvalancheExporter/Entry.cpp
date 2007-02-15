// Dan  2/2/01  \Dan90\Plugin\HXT\MyProperties\Entry.cpp
#include "stdafx.h"
#include "Resource.h"
#include "Hxt.h"
#include "SDK\Entry.h"
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

enum {EXPORT_MODELCACHE, EXPORT_OBJECTCACHECONTAINER, EXPORT_ACTIONCACHE, EXPORT_ACTIONCACHECONTAINER, EXPORT_CHOR };

extern "C" __declspec(dllexport) const BOOL HxtLoadCommandEntry(UINT index, ObjectType &objtype)
{
   switch (index) {
   case EXPORT_MODELCACHE:
      objtype = HOT_MODEL;
      return TRUE;
   case EXPORT_OBJECTCACHECONTAINER:
      objtype = HOT_OBJECTCACHECONTAINER;
      return TRUE;
   case EXPORT_ACTIONCACHE:
      objtype = HOT_ACTION;
      return TRUE;
   case EXPORT_ACTIONCACHECONTAINER:
      objtype = HOT_ACTIONCACHECONTAINER;
      return TRUE;
   case EXPORT_CHOR:
      objtype = HOT_CHOR;
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
   case EXPORT_MODELCACHE:
      if (!((HAnimObject *)htreeobject)->IsCache()) // Actually a Model, not a ModelCache
         return FALSE; 
      mc = MC_EXPORT;
      return TRUE;
   case EXPORT_OBJECTCACHECONTAINER:
      mc = MC_EXPORT;
      return TRUE;
   case EXPORT_ACTIONCACHE:
      if (!((HAnimObject *)htreeobject)->IsCache()) // Actually a Action, not a ActionCache
         return FALSE; 
      mc = MC_EXPORT;
      return TRUE;
   case EXPORT_ACTIONCACHECONTAINER:
      mc = MC_EXPORT;
      return TRUE;
   case EXPORT_CHOR:
      mc = MC_EXPORT;
      return TRUE;
   }
   return FALSE;
}


extern "C" __declspec(dllexport) BOOL HxtOnCommand(HTreeObject *htreeobject, UINT index)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

   switch (index) {
   case EXPORT_MODELCACHE:
      return ((CHxtApp *)AfxGetApp())->OnExportModelCache((HModelCache *)htreeobject);
   case EXPORT_OBJECTCACHECONTAINER:
      return ((CHxtApp *)AfxGetApp())->OnExportObjectCacheContainer((HObjectCacheContainer *)htreeobject);
   case EXPORT_ACTIONCACHE:
      return ((CHxtApp *)AfxGetApp())->OnExportActionCache((HActionCache *)htreeobject);
   case EXPORT_ACTIONCACHECONTAINER:
      return ((CHxtApp *)AfxGetApp())->OnExportActionCacheContainer((HActionCacheContainer *)htreeobject);
   case EXPORT_CHOR:
      return ((CHxtApp *)AfxGetApp())->OnExportChor((HChor *)htreeobject);
   }
   return FALSE;
}


// ************************** This plugin has Properies also

enum {MODEL, GROUP};
extern "C" __declspec(dllexport) const BOOL HxtLoadPropertyEntry(UINT index, ObjectType &objtype)
{
   switch (index) {
   case MODEL:
      objtype = HOT_MODEL;
      return TRUE;
   case GROUP:
      objtype = HOT_GROUP;
      return TRUE;
   }
   return FALSE;
}

extern "C" __declspec(dllexport) BOOL HxtOnAddProperties(HTreeObject *htreeobject, HDynamicCategoryProperty *pluginproperty, UINT index)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

   switch (index) {
   case MODEL:
      ((CHxtApp *)AfxGetApp())->AddModelProperties((HModelCache *)htreeobject, pluginproperty);
      return TRUE;
   case GROUP:
      ((CHxtApp *)AfxGetApp())->AddGroupProperties((HGroup *)htreeobject, pluginproperty);
      return TRUE;
   }
   return FALSE;
}
