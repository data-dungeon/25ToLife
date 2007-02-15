// Dan  12/27/00  \Dan90\Plugin\HXT\Duplicator\Entry.cpp

#include "stdafx.h"
#include "F_Protos.h"
#include "Resource.h"
#include "Hxt.h"
#include "SDK\Entry.h"
#include "ObjType.h"

enum { CB_KEYPOSE };

extern CHxtApp theApp;

enum {MODEL};
extern "C" __declspec(dllexport) const BOOL HxtLoadPropertyEntry(UINT index, ObjectType &objtype)
{
   switch (index) {
   case MODEL:
      objtype = HOT_MODEL;
      return TRUE;
   }
   return FALSE;
}

extern "C" __declspec(dllexport) BOOL HxtOnAddProperties(HTreeObject *htreeobject, HCategoryProperty *pluginproperty, UINT index)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

   switch (index) {
   case MODEL:
      ((CHxtApp *)AfxGetApp())->AddModelProperties((HModel *)htreeobject, pluginproperty);
      return TRUE;
   }

   return FALSE;
}

extern "C" __declspec(dllexport) BOOL HxtLoadControlBarDescription(UINT nIndex, String &strBarname, String &strVendor)
{
 	AFX_MANAGE_STATE(AfxGetStaticModuleState());

   switch (nIndex)
   {
   case CB_KEYPOSE: {
         CString temp;
         temp.LoadString(IDS_BARNAME);
         strBarname = (LPCTSTR)temp;
         strVendor = "Avalanche";
         return TRUE;
      }
   }
   
   return FALSE;
}

extern "C" __declspec(dllexport) BOOL HxtCreateControlBar(UINT nIndex, HWND hwndParent)
{
 	AFX_MANAGE_STATE(AfxGetStaticModuleState());

   switch (nIndex)
   {
   case CB_KEYPOSE:
      return theApp.m_wndKeyPose.CWnd::Create(NULL, NULL, WS_VISIBLE|WS_CHILD, CRect(0,0,0,0), CWnd::FromHandle(hwndParent), AFX_IDW_PANE_FIRST);
   }
   
   return FALSE;
}

extern "C" __declspec(dllexport) void HxtSetPoseModel(HModel *model)
{
 	AFX_MANAGE_STATE(AfxGetStaticModuleState());
   theApp.m_wndKeyPose.SetModel(model);
}
