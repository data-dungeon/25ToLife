
#include "stdafx.h"
#include "HHierObject.h"
#include "HierObject.h"

void HHierObject::SetName(const String &name)
{
   ((HierObject *)this)->SetName(name);
}

String HHierObject::GetName() const
{
  return ((HierObject *)this)->GetMatchName();
}

HStringProperty *HHierObject::GetNotes()
{
   return (HStringProperty *)&((HierObject *)this)->m_strNotes;
}

HPlaceholderObject *HHierObject::AddPlaceholderObject( const String &strFileText )
{
   return (HPlaceholderObject *)((HierObject *)this)->AddPlaceholderObject( strFileText );
}

BOOL HHierObject::AddRotoscope( HRotoscope *pRotoscope )
{
   return ((HierObject *)this)->AddRotoscope( (Rotoscope *)pRotoscope );
}

HHierObject::HConvertInfo *HHierObject::ConvertTo(SnipeObjectType nSOT)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState())
   return (HConvertInfo*)((HierObject *)this)->ConvertTo(nSOT);
}

void HHierObject::HConvertInfo::ReConvert()
{
   ((HierObject::ConvertInfo *)this)->ReConvert();
}

void HHierObject::HConvertInfo::UnConvert()
{
   ((HierObject::ConvertInfo *)this)->UnConvert();
}

void HHierObject::HConvertInfo::operator delete (void *pVoid)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   HierObject::ConvertInfo *pConvertInfo = (HierObject::ConvertInfo *) pVoid;
   delete pConvertInfo;
}

void HHierObject::EditObject()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((HierObject *)this)->OnObjectEdit();
}

HHierObject::HParentInfo *HHierObject::Parent( HHierObject *pHierObject )
{
   return (HHierObject::HParentInfo*)((HierObject *)this)->Parent((HierObject*)pHierObject);
}

void HHierObject::EditDocument()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((HierObject*)this)->OnObjectEdit();
}

void HHierObject::HParentInfo::ReParent()
{
   ((HierObject::ParentInfo *)this)->ReParent();
}

void HHierObject::HParentInfo::UnParent()
{
   ((HierObject::ParentInfo *)this)->UnParent();
}

void HHierObject::HParentInfo::operator delete (void *pVoid)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   HierObject::ParentInfo *pParentInfo = (HierObject::ParentInfo *) pVoid;
   delete pParentInfo;
}

