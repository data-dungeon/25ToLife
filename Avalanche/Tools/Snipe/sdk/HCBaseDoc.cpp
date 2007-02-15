// 
#include "stdafx.h"
#include "HCBaseDoc.h"
#include "../GUI/BaseDoc.h"

HSelectionList *HCBaseDoc::GetSelectionList() const
{
   return (HSelectionList *) ((const CBaseDoc *)this)->GetSelectionList();
}

SnipeDocType HCBaseDoc::GetDocType()
{
   return ((CBaseDoc*)this)->GetDocType();
}

HHierObject *HCBaseDoc::GetObjectHead()
{
   return (HHierObject*)((CBaseDoc*)this)->GetObjectHead();
}