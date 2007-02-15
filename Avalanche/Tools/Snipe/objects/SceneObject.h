#pragma once

#include "TransformableObject.h"
#include "ContainerObject.h"

class SceneObject;

class SceneObjectPointerProperty : public PointerProperty<SceneObject *>
{
};

class MotionInstance;
class MotionInstanceContainer;

class SceneObject : public TransformableObject
{
protected:
   static SceneObjectPointerProperty::Info m_pSceneObjectCacheInfo;

public:
   SceneObjectPointerProperty m_pSceneObjectCache;

   MotionInstanceContainer *m_pMotionInstanceContainer;

   DECLARE_SNIPEOBJECT_PURE(SceneObject, TransformableObject);

            SceneObject();
   virtual ~SceneObject();

// Operations
   MotionInstanceContainer *GetMotionInstanceContainer() { return m_pMotionInstanceContainer; }
   MotionInstanceContainer *CreateMotionInstanceContainer();
   void AddMotionInstance( MotionInstance *pMotionInstance );

// InstanceObject overrides
   virtual InstanceObject *CreateInstance();

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
};

BaseStream &operator << (BaseStream &bs, const SceneObject *&value );
BaseStream &operator >> (BaseStream &bs, SceneObject *&value );
