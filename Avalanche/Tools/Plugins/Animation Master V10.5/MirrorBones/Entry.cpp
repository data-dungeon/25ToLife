// Dan  12/27/00  \Dan90\Plugin\HXT\Duplicator\Entry.cpp

#include "stdafx.h"
#include "F_Protos.h"
#include "Resource.h"
#include "Hxt.h"
#include "MirrorBonesDialog.h"
#include "SDK\Entry.h"

extern "C" __declspec(dllexport) const BOOL HxtLoadCommandEntry(UINT index, ObjectType &objtype)
{
   switch (index)
   {
   case 0:
      objtype = HOT_SEGMENT;
      return TRUE;
   }
   
   return FALSE;
}

extern "C" __declspec(dllexport) BOOL HxtOnAddCommandMenu(HTreeObject *htreeobject, UINT index, String &menuname, MenuCategory &mc, BOOL &disabled)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (!((HAnimObject *)htreeobject)->IsCache()) // Only allow on Caches
      return FALSE; 

   CString string;
   string.LoadString(IDS_MENUFILTER);
   menuname = (LPCTSTR)string;

   disabled = FALSE;
   mc = MC_WIZARD;
   return TRUE;
}

extern "C" __declspec(dllexport) BOOL HxtOnCommand(HTreeObject *htreeobject, UINT index)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return ((CHxtApp *)AfxGetApp())->OnMirrorBones(htreeobject);
}

