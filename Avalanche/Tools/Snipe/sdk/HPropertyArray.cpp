//

#include "stdafx.h"
#include "HPropertyArray.h"

#include "stdafx.h"
#include "HPropertyArray.h"
#include "Properties/PropertyArray.h"

HProperty *HPropertyArray::Find(const String &strMatchName, BOOL bRecursive/*=TRUE*/)
{
   return (HProperty *) ((PropertyArray *)this)->Find( strMatchName, bRecursive );
}

HPropertyArray *HPropertyArray::New()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HPropertyArray *)SNEW PropertyArray;
}

//Atributes
UINT HPropertyArray::GetSize() const
{
   return ((PropertyArray *)this)->GetSize();
}

BOOL HPropertyArray::IsEmpty() const
{
   return ((PropertyArray *)this)->IsEmpty();
}

void HPropertyArray::SetSize( UINT nSize, UINT nGrowBy /*=-1*/)
{
   ((PropertyArray *)this)->SetSize( nSize, nGrowBy );
}

//Operations
void HPropertyArray::RemoveAll()
{
   ((PropertyArray *)this)->RemoveAll();
}

void HPropertyArray::FreeExtra()
{
   ((PropertyArray *)this)->FreeExtra();
}

//Element Access
HProperty *&HPropertyArray::GetAt(UINT nIndex)
{
   return (HProperty *&)((PropertyArray *)this)->GetAt(nIndex);
}

const HProperty *&HPropertyArray::GetAt(UINT nIndex) const
{
   return (const HProperty *&)((PropertyArray *)this)->GetAt(nIndex);
}

void HPropertyArray::SetAt(UINT nIndex, HProperty *pProperty)
{
   ((PropertyArray *)this)->SetAt(nIndex, (Property *)pProperty);
}

//Growing the Array
UINT HPropertyArray::Add(HProperty *pProperty)
{
   return ((PropertyArray *)this)->Add((Property *)pProperty);
}

UINT HPropertyArray::Append( const HPropertyArray &properyarray )
{
   return ((PropertyArray *)this)->Append((const PropertyArray &)properyarray);
}

void HPropertyArray::Copy( const HPropertyArray &propertyarray )
{
   ((PropertyArray *)this)->Copy((const PropertyArray &)propertyarray);
}

void HPropertyArray::SetAtGrow( UINT nIndex, HProperty *pProperty)
{
   ((PropertyArray *)this)->SetAtGrow(nIndex, (Property *)pProperty);
}

//Insertion - Removal
void HPropertyArray::InsertAt( UINT nIndex, HProperty *pProperty, UINT nCount /*= 1*/ )
{
   ((PropertyArray *)this)->InsertAt(nIndex, (Property *)pProperty, nCount);
}

void HPropertyArray::RemoveAt( UINT nIndex, UINT nCount /*= 1*/ )
{
   ((PropertyArray *)this)->RemoveAt(nIndex, nCount);
}

//Operators
HProperty *&HPropertyArray::operator[] (int i )
{
   return (HProperty *&) (*((PropertyArray *)this))[i];
}

const HProperty *&HPropertyArray::operator[] (int i ) const
{
   return (const HProperty *&) (*((PropertyArray *)this))[i];
}

void HPropertyArray::operator delete (void *pVoid)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   delete (PropertyArray *)pVoid;
}
