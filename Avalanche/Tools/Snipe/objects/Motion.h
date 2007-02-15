#pragma once
#include "ContainerObject.h"
#include "../Properties/Property.h"
#include "Scene.h"

class ModelPropertyDriversContainer;
class BoneDriversContainer;
class CustomPropertyDriversContainer;
class VertexMotionDriversContainer;
class ChannelDriver;
class MotionChannel;
class MotionInstance;
class PolyModel;
class Motion;

class Motion : public Scene
{
  DECLARE_MESSAGE_MAP()
public:
   static TimeProperty::Info     m_tLengthInfo;
   static FloatProperty::Info     m_fReductionToleranceInfo;

   TimeProperty     m_tLength;
   FloatProperty     m_fReductionTolerance;

private:
   ModelPropertyDriversContainer  *m_pModelPropertyDriversContainer;
   BoneDriversContainer           *m_pBoneDriversContainer;
   CustomPropertyDriversContainer *m_pCustomPropertyDriversContainer;
   VertexMotionDriversContainer   *m_pVertexMotionDriversContainer;

public:
   DECLARE_SNIPEOBJECT_CREATE(Motion, Scene)
   DECLARE_COMMANDOBJECT()

            Motion();
   virtual ~Motion();

   ModelPropertyDriversContainer *CreateModelPropertyDriversContainer();
   ModelPropertyDriversContainer *GetModelPropertyDriversContainer();

   BoneDriversContainer *CreateBoneDriversContainer();
   BoneDriversContainer *GetBoneDriversContainer();

   CustomPropertyDriversContainer *CreateCustomPropertyDriversContainer();
   CustomPropertyDriversContainer *GetCustomPropertyDriversContainer();

   VertexMotionDriversContainer *CreateVertexMotionDriversContainer();
   VertexMotionDriversContainer *GetVertexMotionDriversContainer();

   MotionChannel *AddBone( const char *name = NULL );

   UINT GetNumMotionChannels();
   MotionChannel *GetMotionChannelAt(UINT nIndex) { return (MotionChannel*)((HierObject*)m_pBoneDriversContainer)->GetChildAt(nIndex); }
   MotionChannel *GetMotionChannel(const char *name);

   void CalcLengthBranch( SnipeObject *pStartObject, Time &tLength );
   Time CalcLength();

   Time GetStartTime();
   Time GetStopTime();

   void DirtyModifiedMotionCurves();

   // property access functions
   TimeProperty *GetLength() { return &m_tLength; }
   TimeProperty *GetPlaybackStart() { return &m_tPlaybackStart; }
   TimeProperty *GetPlaybackStop() { return &m_tPlaybackEnd; }

   // Message Maps
   afx_msg void OnUpdateExportMotion(CCmdUI* pCmdUI);
   afx_msg void OnExportMotion();

   // HierObject overrides
   virtual CSnipeDocTemplate *GetDocTemplate();
   virtual void OnParent( HierObject *pHierObject );
   virtual BOOL IsDraggable() { return TRUE; }

   // InstanceObject overrides
   virtual InstanceObject *CreateInstance(); 

// SnipeObject overrides
   virtual UINT GetIconIndex() { return ICONINDEX_MOTION; };
   virtual UINT GetDefaultMenuItemID();
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};

class MotionContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(MotionContainer, ContainerObject)

   MotionContainer();
   virtual ~MotionContainer();

   UINT GetNumMotions() const { return GetNumChildren(); }
   Motion *GetMotionAt( UINT nIndex ) { return (Motion *)GetChildAt(nIndex); }

// SnipeObject overrides
   virtual BOOL IsDeletable() { return FALSE; }
};

class MotionPointerProperty : public PointerProperty<Motion *>
{
};

BaseStream &operator << (BaseStream &bs, const Motion *&value );
BaseStream &operator >> (BaseStream &bs, Motion *&value );

class SceneObject;

class MotionInstanceContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(MotionInstanceContainer, ContainerObject)

   MotionInstanceContainer();

   SceneObject *GetSceneObject() { return (SceneObject *)m_pParent; }

   UINT GetNumMotions() const { return GetNumChildren(); }
   MotionInstance *GetMotionAt( UINT nIndex ) { return (MotionInstance *)GetChildAt(nIndex); }
};

class MotionInstance : public HierObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(MotionInstance, HierObject)

   class DeleteInfo : public HierObject::DeleteInfo
   {
   private:
      MotionInstanceContainer *m_pMotionInstanceContainer;

   public:
      SnipeObject::DeleteInfo *m_pMotionInstanceContainerDeleteInfo;

      DeleteInfo( SnipeObject *pSnipeObject )
         : HierObject::DeleteInfo( pSnipeObject )
      {
         m_pMotionInstanceContainer = NULL;
         m_pMotionInstanceContainerDeleteInfo = NULL;
      }

      ~DeleteInfo()
      {
         if (m_pMotionInstanceContainerDeleteInfo)
            delete m_pMotionInstanceContainerDeleteInfo;
         SetOldMotionInstanceContainer(NULL);
      }
      void SetOldMotionInstanceContainer( MotionInstanceContainer *pContainer ) { AssignWithReference( (SnipeObject *&)m_pMotionInstanceContainer, (SnipeObject *)pContainer );  }
      MotionInstanceContainer *GetOldMotionInstanceContainer() const { return m_pMotionInstanceContainer; }
   };
   virtual SnipeObject::DeleteInfo *AllocDeleteInfo() { return SNEW DeleteInfo(this); }

private:
   static MotionPointerProperty::Info m_pMotionInfo;
      static TimeProperty::Info          m_tCropStartInfo;
      static TimeProperty::Info          m_tCropEndInfo;
   static TimeProperty::Info          m_tStartInfo;
   static TimeProperty::Info          m_tDurationInfo;

public:
   MotionPointerProperty m_pMotion;
      TimeProperty          m_tCropStart;
      TimeProperty          m_tCropEnd;
   TimeProperty          m_tStart;
   TimeProperty          m_tDuration;

            MotionInstance();
   virtual ~MotionInstance();

   MotionInstanceContainer *GetMotionInstanceContainer() { return (MotionInstanceContainer *)m_pParent; }
   Motion *GetCache() { return m_pMotion; }

   SceneObject *GetSceneObject() { if (GetMotionInstanceContainer()) return GetMotionInstanceContainer()->GetSceneObject(); return NULL; }

   void ApplyDrivers();
   void ApplyDriversBranch( SnipeObject *pParentObject );

// SnipeObject overrides
   virtual UINT GetIconIndex() { return ICONINDEX_MOTION; };
   virtual void FreeDeleteInfo( SnipeObject::DeleteInfo *pDeleteInfo ) { delete (DeleteInfo *)pDeleteInfo; }// DeleteInfo does not have a v-table, this must be here to get to the right destructor
   virtual BOOL Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError );
   virtual BOOL UnDelete( SnipeObject::DeleteInfo *pDeleteInfo );
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual BOOL GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList );
};

class MotionInstancePointerProperty : public PointerProperty<MotionInstance *>
{
};

BaseStream &operator << (BaseStream &bs, const MotionInstance *&value );
BaseStream &operator >> (BaseStream &bs, MotionInstance *&value );
