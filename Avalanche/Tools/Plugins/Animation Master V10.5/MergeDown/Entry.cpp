// Dan  12/27/00  \Dan90\Plugin\HXT\Duplicator\Entry.cpp

#include "stdafx.h"
//#include "Resource.h"
#include "Hxt.h"
#include "StrClass.h"
#include "SDK\Entry.h"
#include "SDK\HAction.h"

enum {MERGE_ACTION, MERGE_CHORACTION };

extern "C" __declspec(dllexport) const BOOL HxtLoadCommandEntry(UINT index, ObjectType &objtype)
{
   switch (index)
   {
   case MERGE_ACTION:
      objtype = HOT_ACTION;
      return TRUE;
   case MERGE_CHORACTION:
      objtype = HOT_CHORACTION;
      return TRUE;
   }
   
   return FALSE;
}

extern "C" __declspec(dllexport) BOOL HxtOnAddCommandMenu(HTreeObject *htreeobject, UINT index, String &menuname, MenuCategory &mc, BOOL &disabled)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (index == MERGE_ACTION) // HOT_ACTIONS, do this because ChorActions are both cache and instance
      if (((HAction *)htreeobject)->IsCache()) // Only allow on Instances
         return FALSE; 

   CString string;
   string.LoadString(IDS_MENUFILTER);
   menuname = (LPCTSTR)string;

   disabled = TRUE;
   HTreeObject *sibling = htreeobject->GetTreeSibling();
   if (sibling) {
      int hot = sibling->GetObjectType();
      if (hot == HOT_CHORACTION)
         return TRUE;
   }
   return FALSE;
}

extern "C" __declspec(dllexport) BOOL HxtOnCommand(HTreeObject *htreeobject, UINT index)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return ((CHxtApp *)AfxGetApp())->MergeDown((HAction *)htreeobject);
}

