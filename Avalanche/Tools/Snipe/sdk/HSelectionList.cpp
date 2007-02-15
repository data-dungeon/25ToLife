#include "stdafx.h"
#include "HSelectionList.h"
#include "Interface/SelectionList.hpp"

void HSelectionList::RemoveAll()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((SelectionList*)this)->RemoveAll();
}

BOOL HSelectionList::IsEmpty() const
{
   return ((SelectionList*)this)->IsEmpty();
}

UINT HSelectionList::GetCount() const
{
   return ((SelectionList*)this)->GetCount();
}

const BoundingBox &HSelectionList::GetBoundingBox()
{
   return ((SelectionList *)this)->GetBoundingBox();
}

void HSelectionList::SetDirty()
{
   ((SelectionList *)this)->SetDirty();
}

void HSelectionList::Add(HSnipeObject *pSnipeObject)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((SelectionList*)this)->Add((SnipeObject*)pSnipeObject);
}

void HSelectionList::Remove(HSnipeObject *pSnipeObject)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((SelectionList*)this)->Remove((SnipeObject*)pSnipeObject);
}

void HSelectionList::Toggle(HSnipeObject *pSnipeObject)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((SelectionList*)this)->Toggle((SnipeObject*)pSnipeObject);
}

void HSelectionList::Add(HSnipeObjectList *pList)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((SelectionList*)this)->Add((SnipeObjectList *)pList);
}

void HSelectionList::Remove(HSnipeObjectList *pList)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((SelectionList*)this)->Remove((SnipeObjectList *)pList);
}

void HSelectionList::Toggle(HSnipeObjectList *pList)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((SelectionList*)this)->Toggle((SnipeObjectList *)pList);
}

void HSelectionList::SetTo(HSnipeObjectList *pList)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((SelectionList*)this)->SetTo((SnipeObjectList *)pList);
}

//BOOL HSelectionList::Find(HSnipeObject *pSnipeObject)
//{
//   return ((SelectionList*)this)->Find((SnipeObject*)pSnipeObject);
//}

POSITION HSelectionList::GetHeadPosition()
{
   return ((SelectionList*)this)->GetHeadPosition();
}

HSnipeObject *HSelectionList::GetHead()
{
   return (HSnipeObject *)((SelectionList*)this)->GetHead();
}

HSnipeObject *HSelectionList::GetNext(POSITION &pos)
{
   return (HSnipeObject*)((SelectionList*)this)->GetNext(pos);
}

HSnipeObjectList *HSelectionList::CreateSnipeObjectList()
{
   return (HSnipeObjectList*)((SelectionList*)this)->CreateSnipeObjectList();
}

void HSelectionList::operator delete (void *pVoid)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   delete (SelectionList *)pVoid;
}

// Static 
void HSelectionList::SetMouseOverObject( HSnipeObject *pMouseOverObject )
{
   SelectionList::SetMouseOverObject( (SnipeObject*)pMouseOverObject );
}

BOOL HSelectionList::SetFocusObject(HSnipeObject *pFocusObject, HSnipeObject *pNearestFocusObject )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return SelectionList::SetFocusObject( (SnipeObject*)pFocusObject, (SnipeObject *)pNearestFocusObject );
}

HSnipeObject *HSelectionList::GetMouseOverObject()
{
   return (HSnipeObject *)SelectionList::GetMouseOverObject();
}

HSnipeObject *HSelectionList::GetFocusObject()
{
   return (HSnipeObject *)SelectionList::GetFocusObject();
}

HSnipeObject *HSelectionList::GetNearestFocusObject()
{
   return (HSnipeObject *)SelectionList::GetNearestFocusObject();
}

///////////////////////////////////////////////////