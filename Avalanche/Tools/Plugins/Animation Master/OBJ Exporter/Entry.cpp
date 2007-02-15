// Dan  1/26/00
#include "stdafx.h"
#include "HashHand.h"
#include "Resource.h"
#include "IE.h"
#include "StrClass.h"
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

//****************************************************
//** ModelCacheHandle Export
//****************************************************
extern "C" __declspec(dllexport) const char *GetExportModelCacheHandleFilters()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   static CString filter;
   filter.LoadString(IDS_MENUFILTER);
   return (LPCTSTR)filter;
}

extern "C" __declspec(dllexport) BOOL OnExportModelCacheHandle(ModelCacheHandle *mch, const char *desc)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((CIEApp *)AfxGetApp())->OnExportModelCacheHandle(mch);
}

//****************************************************
//** ModelCacheHandle Import
//****************************************************
extern "C" __declspec(dllexport) const char *GetImportModelCacheHandleFilters()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   static CString filter;
   filter.LoadString(IDS_MENUFILTER);
   return (LPCTSTR)filter;
}

extern "C" __declspec(dllexport) BOOL OnImportModelCacheHandle(ModelCacheHandle *mch, const char *desc)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((CIEApp *)AfxGetApp())->OnImportModelCacheHandle(mch);
}

//****************************************************
//** ObjectCacheContainerHandle Export
//****************************************************
extern "C" __declspec(dllexport) const char *GetExportObjectCacheContainerHandleFilters()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   static CString filter;
   filter.LoadString(IDS_MENUFILTER);
   return (LPCTSTR)filter;
}

extern "C" __declspec(dllexport) BOOL OnExportObjectCacheContainerHandle(ObjectCacheContainerHandle *occh, const char *desc)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((CIEApp *)AfxGetApp())->OnExportObjectCacheContainerHandle(occh);
}

//****************************************************
//** ObjectCacheContainerHandle Import
//****************************************************
extern "C" __declspec(dllexport) const char *GetImportObjectCacheContainerHandleFilters()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   static CString filter;
   filter.LoadString(IDS_MENUFILTER);
   return (LPCTSTR)filter;
}

extern "C" __declspec(dllexport) BOOL OnImportObjectCacheContainerHandle(ObjectCacheContainerHandle *occh, const char *desc)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((CIEApp *)AfxGetApp())->OnImportObjectCacheContainerHandle(occh);
}

//****************************************************
//** ActionCacheHandle Export // Morph Targets
//****************************************************
extern "C" __declspec(dllexport) const char *GetExportActionCacheHandleFilters()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   static CString filter;
   filter.LoadString(IDS_MENUFILTER);
   return (LPCTSTR)filter;
}

extern "C" __declspec(dllexport) BOOL OnExportActionCacheHandle(ActionCacheHandle *ach, const char *desc)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((CIEApp *)AfxGetApp())->OnExportActionCacheHandle(ach);
}

//****************************************************
//** ActionCacheContainerHandle Export
//****************************************************
extern "C" __declspec(dllexport) const char *GetExportActionCacheContainerHandleFilters()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   static CString filter;
   filter.LoadString(IDS_MENUFILTER);
   return (LPCTSTR)filter;
}

extern "C" __declspec(dllexport) BOOL OnExportActionCacheContainerHandle(ActionCacheContainerHandle *acch, const char *desc)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((CIEApp *)AfxGetApp())->OnExportActionCacheContainerHandle(acch);
}

//****************************************************
//** Polygon Prop Import (IEPolyModel)
//****************************************************
extern "C" __declspec(dllexport) const char *GetImportPropModelFilters()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   static CString filter;
   filter.LoadString(IDS_FILEFILTERS);
   return (LPCTSTR)filter;
}

extern "C" __declspec(dllexport) IEPolyModel *OnImportPropModel(const char *filename, const char *desc)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return ((CIEApp *)AfxGetApp())->ImportPLY(filename);
}

extern "C" __declspec(dllexport) void DeletePropModel(IEPolyModel *iemodel)
{
   delete iemodel;
}


