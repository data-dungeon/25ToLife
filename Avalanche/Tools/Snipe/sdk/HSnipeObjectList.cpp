//
#include "stdafx.h"
#include "HSnipeObjectList.h"
#include "SnipeObject.h"

// Construction
HSnipeObjectList *HSnipeObjectList::New()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HSnipeObjectList *) SNEW SnipeObjectList;
}

// Head / Tail Access
HSnipeObject *HSnipeObjectList::GetHead() // Cannot be empty
{
   return (HSnipeObject *) ((SnipeObjectList *)this)->GetHead();
}

HSnipeObject *HSnipeObjectList::GetTail() // Cannot be empty
{
   return (HSnipeObject *) ((SnipeObjectList *)this)->GetTail();
}

// Operations
POSITION HSnipeObjectList::AddHead( HSnipeObject *pSnipeObject )
{
   return ((SnipeObjectList *)this)->AddHead( (SnipeObject *)pSnipeObject );
}

POSITION HSnipeObjectList::AddTail( HSnipeObject *pSnipeObject )
{
   return ((SnipeObjectList *)this)->AddTail( (SnipeObject *)pSnipeObject );
}

void HSnipeObjectList::AddTail( HSnipeObjectList *pSnipeObjectList )
{
   ((SnipeObjectList *)this)->AddTail( (SnipeObjectList *)pSnipeObjectList );
}

void HSnipeObjectList::RemoveAll()
{
   ((SnipeObjectList *)this)->RemoveAll();
}

void HSnipeObjectList::RemoveHead()
{
   ((SnipeObjectList *)this)->RemoveHead();
}

void HSnipeObjectList::RemoveTail()
{
   ((SnipeObjectList *)this)->RemoveTail();
}

//Iteration
POSITION HSnipeObjectList::GetHeadPosition() const
{
   return ((SnipeObjectList *)this)->GetHeadPosition();
}

HSnipeObject *HSnipeObjectList::GetNext(POSITION &pos)
{
   return (HSnipeObject *)((SnipeObjectList *)this)->GetNext( pos );
}

HSnipeObject *HSnipeObjectList::GetPrev(POSITION &pos)
{
   return (HSnipeObject *)((SnipeObjectList *)this)->GetPrev( pos );
}

POSITION HSnipeObjectList::GetTailPosition() const
{
   return ((SnipeObjectList *)this)->GetTailPosition();
}

//Retrieval/Modification
HSnipeObject *HSnipeObjectList::GetAt( POSITION pos ) const
{
   return (HSnipeObject *) ((SnipeObjectList *)this)->GetAt( pos );
}

void HSnipeObjectList::RemoveAt( POSITION pos )
{
   ((SnipeObjectList *)this)->RemoveAt( pos );
}

void HSnipeObjectList::SetAt( POSITION pos, HSnipeObject *pSnipeObject )
{
   ((SnipeObjectList *)this)->SetAt( pos, (SnipeObject *)pSnipeObject );
}

void HSnipeObjectList::ConvertCompatibleObjectsToSOT( SnipeObjectType nSOT )
{
   ((SnipeObjectList *)this)->ConvertCompatibleObjectsToSOT( nSOT );
}
   
//Insertion
POSITION HSnipeObjectList::InsertAfter( POSITION pos, HSnipeObject *pSnipeObject )
{
   return ((SnipeObjectList *)this)->InsertAfter( pos, (SnipeObject *)pSnipeObject );
}

POSITION HSnipeObjectList::InsertBefore( POSITION pos, HSnipeObject *pSnipeObject )
{
   return ((SnipeObjectList *)this)->InsertBefore( pos, (SnipeObject *)pSnipeObject );
}

//Searching
POSITION HSnipeObjectList::Find( HSnipeObject *pObject, POSITION startAfter/* = NULL */) const
{
   return ((SnipeObjectList *)this)->Find( (SnipeObject *)pObject, startAfter );
}

POSITION HSnipeObjectList::FindIndex( UINT nIndex ) const
{
   return ((SnipeObjectList *)this)->FindIndex( nIndex );
}

//Status
UINT HSnipeObjectList::GetSize() const
{
   return (UINT)((SnipeObjectList *)this)->GetSize();
}

BOOL HSnipeObjectList::IsEmpty() const
{
   return ((SnipeObjectList *)this)->IsEmpty();
}

void HSnipeObjectList::ComputeBoundingBox( BoundingBox &bbox, HCGLBaseView *pView/*=NULL*/ )
{
   ((SnipeObjectList *)this)->ComputeBoundingBox( bbox, (CGLBaseView *)pView );
}

void HSnipeObjectList::operator delete (void *pVoid)
{
 	AFX_MANAGE_STATE(AfxGetAppModuleState());
   delete (SnipeObjectList *) pVoid;
}

