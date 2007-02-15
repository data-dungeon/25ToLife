
#include "stdafx.h"

#include "HSnipeObject.h"
#include "SnipeObject.h"
#include "Component.h"
#include "..\GUI\GLBaseView.h"

HSnipeObject *HSnipeObject::HDeleteInfo::GetSnipeObject()
{
   return (HSnipeObject *) ((SnipeObject::DeleteInfo *)this)->GetSnipeObject();
}

void HSnipeObject::HDeleteInfo::UnDelete()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((SnipeObject::DeleteInfo *)this)->UnDelete();
}

void HSnipeObject::HDeleteInfo::Delete()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((SnipeObject::DeleteInfo *)this)->Delete();
}

void HSnipeObject::HDeleteInfo::operator delete (void *pVoid)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   SnipeObject::DeleteInfo *pDeleteInfo = (SnipeObject::DeleteInfo *) pVoid;
   pDeleteInfo->GetSnipeObject()->FreeDeleteInfo( pDeleteInfo );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HSnipeObject::operator delete (void *pVoid)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((SnipeObject *)pVoid)->Destroy();
}

String HSnipeObject::GetTitle() const
{
   return ((const SnipeObject *)this)->GetTitle();
}

BOOL HSnipeObject::IsComponent() const
{
   return ((SnipeObject *)(this))->IsComponent();
}

SnipeObjectType HSnipeObject::GetSOT() const
{
   return ((const SnipeObject *)this)->GetSOT();
}

const HSnipeObject *HSnipeObject::IsKindOf( SnipeObjectType nType ) const
{
   if (!this)
      return NULL;
   return (const HSnipeObject *)((SnipeObject *)(this))->IsKindOf( nType );
}

String HSnipeObject::GetMatchName() const
{
   return ((SnipeObject *)(this))->GetMatchName();
}

BOOL HSnipeObject::IsSelected() const
{
   return ((SnipeObject *)(this))->IsSelected();
}

BOOL HSnipeObject::IsDeletable() const
{
   return ((SnipeObject *)(this))->IsDeletable();
}

void HSnipeObject::SetVisible( BOOL bVisible )
{
   ((SnipeObject *)(this))->SetVisible( bVisible );
}

BOOL HSnipeObject::IsVisible() const
{
   return ((SnipeObject *)(this))->IsVisible();
}

BOOL HSnipeObject::IsHidable() const
{
   return ((SnipeObject *)(this))->IsHidable();
}

void HSnipeObject::SetLocked( BOOL bLocked )
{
   ((SnipeObject *)(this))->SetLocked( bLocked );
}

BOOL HSnipeObject::IsLocked() const
{
   return ((SnipeObject *)(this))->IsLocked();
}

BOOL HSnipeObject::IsLockable() const
{
   return ((SnipeObject *)(this))->IsLockable();
}

BOOL HSnipeObject::IsUsedBy( HSnipeObject *pObject ) const
{
   return ((SnipeObject *)(this))->IsUsedBy( (SnipeObject *)pObject );
}

BOOL HSnipeObject::IsInProject() const
{
   return ((SnipeObject *)(this))->IsInList();
}

int HSnipeObject::GetNumChildren() const
{
   return ((SnipeObject *)this)->GetNumChildren();
}

HSnipeObject *HSnipeObject::GetChildAt( int index )
{
   return (HSnipeObject *)((SnipeObject *)this)->GetChildAt( index );
}

HSnipeObjectList *HSnipeObject::CreateListFromSOT(SnipeObjectType nSOT)
{
   return (HSnipeObjectList *)((SnipeObject *)(this))->CreateListFromSOT( nSOT );
}

HSnipeObjectList *HSnipeObject::CreateListFromSOT(const String &strMatchName)
{
   return (HSnipeObjectList *)((SnipeObject *)(this))->CreateListFromSOT( strMatchName );
}

const Matrix44 &HSnipeObject::GetObjectToWorldMatrix()
{
   return ((SnipeObject *)(this))->GetObjectToWorldMatrix();
}

const Matrix44 &HSnipeObject::GetWorldToObjectMatrix()
{
   return ((SnipeObject *)(this))->GetWorldToObjectMatrix();
}

BOOL HSnipeObject::GetWorldPos( Vector &pos )
{
   return ((SnipeObject *)(this))->GetWorldPos( pos );
}

void HSnipeObject::PreTransform()
{
   ((SnipeObject *)(this))->PreTransform();
}

void HSnipeObject::Transform( const Matrix44 &matrix )
{
   ((SnipeObject *)(this))->Transform( matrix );
}

HSnipeObject *HSnipeObject::GetParent() const
{
   AFX_MANAGE_STATE(AfxGetAppModuleState())
   return (HSnipeObject *) ((SnipeObject *)this)->GetParent();
}

HSnipeObject *HSnipeObject::GetOwner()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState())
   return (HSnipeObject *) ((SnipeObject *)this)->GetOwner();
}

BOOL HSnipeObject::Delete( HSnipeObject::HDeleteInfo *&pDeleteInfo, String &strOutError )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState())
   return ((SnipeObject *)this)->Delete( (SnipeObject::DeleteInfo *&)pDeleteInfo, strOutError );
}

void HSnipeObject::DirtyDependentFaces()
{
   ((SnipeObject*)this)->DirtyDependentFaces();
}

HSelectionContainer *HSnipeObject::GetSelectionContainer()
{
   return (HSelectionContainer *)((SnipeObject *)this)->GetSelectionContainer();
}

HSelection *HSnipeObject::AddSelection()
{
   return (HSelection *)((SnipeObject *)this)->AddSelection();
}

void HSnipeObject::Copy( HSnipeObject *pSnipeObject )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState())
   ((SnipeObject*)this)->Copy((SnipeObject*)pSnipeObject);
}

HSnipeObject *HSnipeObject::Duplicate()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState())
   return (HSnipeObject*)((SnipeObject*)this)->Duplicate();
}

BOOL HSnipeObject:: AddCallback( UINT msgs, PFN_ONOBJECTNOTIFY pfnOnObjectNotifyCallback, void *pData )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState())
   return ((SnipeObject *)this)->AddCallback( msgs, pfnOnObjectNotifyCallback, pData );
}

BOOL HSnipeObject::RemoveCallback( PFN_ONOBJECTNOTIFY pfnOnObjectNotifyCallback, void *pData, UINT msgs/*=NOTIFYOBJ_ALL*/ )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState())
   return ((SnipeObject *)this)->RemoveCallback( pfnOnObjectNotifyCallback, pData, msgs );
}

void HSnipeObject::SetDrawColor(const RGBAFloat &color)
{
   ((SnipeObject*)this)->SetDrawColor(color);
}

void HSnipeObject::Draw(HCGLBaseView *pView)
{
   ((SnipeObject*)this)->Draw((CGLBaseView*)pView);
}
