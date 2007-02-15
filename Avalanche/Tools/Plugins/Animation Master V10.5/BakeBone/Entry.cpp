// Dan  12/27/00  \Dan90\Plugin\HXT\Duplicator\Entry.cpp

#include "stdafx.h"
//#include "Resource.h"
#include "Hxt.h"
#include "StrClass.h"
#include "SDK\Entry.h"
#include "SDK\HBone.h"

enum {BAKE_MODELBONE, BAKE_LIGHT, BAKE_NULL, BAKE_CAMERA };

extern "C" __declspec(dllexport) const BOOL HxtLoadCommandEntry(UINT index, ObjectType &objtype)
{
   switch (index)
   {
   case BAKE_MODELBONE:
      objtype = HOT_MODEL;
      return TRUE;
   case BAKE_LIGHT:
      objtype = HOT_LIGHT;
      return TRUE;
   case BAKE_NULL:
      objtype = HOT_NULLOBJECT;
      return TRUE;
   case BAKE_CAMERA:
      objtype = HOT_CAMERA;
      return TRUE;
   }
   
   return FALSE;
}

extern "C" __declspec(dllexport) BOOL HxtOnAddCommandMenu(HTreeObject *htreeobject, UINT index, String &menuname, MenuCategory &mc, BOOL &disabled)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (((HBone *)htreeobject)->IsCache()) // Only allow on Instances
      return FALSE; 
   
   CString string;
   string.LoadString(IDS_MENUFILTER);
   menuname = (LPCTSTR)string;

   return TRUE;
}

extern "C" __declspec(dllexport) BOOL HxtOnCommand(HTreeObject *htreeobject, UINT index)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return ((CHxtApp *)AfxGetApp())->BakeBone((HObject *)htreeobject);
}

