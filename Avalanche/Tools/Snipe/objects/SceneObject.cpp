#include "stdafx.h"
#include "SceneObject.h"
#include "Motion.h"
#include "../GUI/PropertyBar.h"

SceneObject *BasicProperty<SceneObject *>::m_prevvalue;

SceneObjectPointerProperty::Info SceneObject::m_pSceneObjectCacheInfo( "Cache", "Cache", PIF_HIDDEN );

IMPLEMENT_SNIPEOBJECT_PURE( SceneObject )

SceneObject::SceneObject()
{
   m_pSceneObjectCache.Init( &m_pSceneObjectCacheInfo, this );

   m_pMotionInstanceContainer = NULL;
}

SceneObject::~SceneObject()
{
}

MotionInstanceContainer *SceneObject::CreateMotionInstanceContainer()
{
   if (!m_pMotionInstanceContainer)
   {
      m_pMotionInstanceContainer = SNEW MotionInstanceContainer;
      AddChildAtTail( m_pMotionInstanceContainer );
   }

   return m_pMotionInstanceContainer;
}

void SceneObject::AddMotionInstance( MotionInstance *pMotionInstance )
{
   CreateMotionInstanceContainer()->AddChildAtTail( pMotionInstance );
}

static void CloneCacheHierarchy( HierObject *pCacheParent, HierObject *pInstanceParent )
{
   int nChildCount = pCacheParent->GetNumChildren();

   for (int nIndex=0; nIndex<nChildCount; nIndex++)
   {
      HierObject *pCacheChild = (HierObject *)pCacheParent->GetChildAt( nIndex )->IsKindOf( SOT_HierObject );
      if (pCacheChild)
      {
         HierObject *pInstanceChild = (HierObject *)pCacheChild->CreateInstance();
         pInstanceParent->AddChildAtTail( pInstanceChild );

         CloneCacheHierarchy( pCacheChild, pInstanceChild );
      }
   }
}

InstanceObject *SceneObject::CreateInstance()
{
   SceneObject *pInstance = (SceneObject *)TransformableObject::CreateInstance();
   pInstance->m_pSceneObjectCache.SetVisible( TRUE );
   pInstance->m_pSceneObjectCache.StoreValue( this, 0 );
   return pInstance;
}

BOOL SceneObject::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_ADDEDCHILD:
      {
         SnipeObject *pChild = (SnipeObject *)lParam;
         switch (pChild->GetSOT())
         {
            case SOT_MotionInstanceContainer:
               m_pMotionInstanceContainer = (MotionInstanceContainer *)pChild;
               break;
         }
         break;
      }
      case NOTIFYOBJ_REMOVEDCHILD:
      {
         SnipeObject *pChild = (SnipeObject *)lParam;
         if (pChild == GetMotionInstanceContainer())
            m_pMotionInstanceContainer = NULL;
         break;
      }
      case NOTIFYOBJ_ADDEDDESCENDANT:
      {
         HierObject *pChild = (HierObject *)lParam;
         switch (pChild->GetSOT())
         {
            case SOT_MotionInstance:
            {
               MotionInstance *pMotionInstance = (MotionInstance *)pChild;
               pMotionInstance->ApplyDrivers();
               break;
            }
         }
         break;
      }
      case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         if (pProperty == &m_pSceneObjectCache)
         {
            m_pCache = m_pSceneObjectCache;
            if (IsInstance())
            {
               m_pSceneObjectCache.SetVisible( TRUE );
               GetPropertyArray()->SetCachePointers( m_pCache->GetPropertyArray() );
               CloneCacheHierarchy( m_pSceneObjectCache, this );
            }
         }
         break;
      }
   }

   return TransformableObject::OnObjectNotify( msg, wParam, lParam );
}

void SceneObject::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   if (IsInstance())
   {
      for (int i=0; i<GetNumChildren(); i++)
      {
         SnipeObject *pSnipeObject = GetChildAt( i );
         pSnipeObject->m_bSaveDependent = FALSE;
      }

      if (m_pMotionInstanceContainer)
         m_pMotionInstanceContainer->m_bSaveDependent = TRUE;
   }

   TransformableObject::Save( bs, bWriteHeader );
}

BaseStream &operator << ( BaseStream &bs, const SceneObject *&value )
{
   return operator << ( bs, (const SnipeObject * &) value);
}

BaseStream &operator >> ( BaseStream &bs, SceneObject *&value )
{
   return operator >> (bs, (SnipeObject *&) value );
}
