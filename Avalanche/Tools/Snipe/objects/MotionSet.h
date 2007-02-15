#pragma once
#include "ContainerObject.h"
#include "../Properties/Property.h"
#include "Scene.h"
#include "../Properties/MotionCurveDrivers.h"
#include "PolyModel.h"
#include "Motion.h"

class MotionChannelBiasObject;

class MotionPlaceholder : public HierObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(MotionPlaceholder, HierObject)

   static BoolProperty::Info m_bActiveInfo;

   BoolProperty m_bActive;

   Motion *m_pMotion;

   MotionPlaceholder();

// Operations
   Motion *GetMotion() { return m_pMotion; }
   void ApplyDrivers( SceneObject *pSceneObject );
   void ApplyDriversBranch( SnipeObject *pParentObject, SceneObject *pSceneObject );

   UINT GetNumMotionChannelBiasObjects() const { return GetNumChildren(); }
   MotionChannelBiasObject *GetMotionChannelBiasObjectAt( UINT nIndex ) { return (MotionChannelBiasObject *)GetChildAt(nIndex); }

// SnipeObject
   virtual void OnTreeItemCreated( CGLObjectTreeItem *pItem );
};

class MotionPlaceholderContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(MotionPlaceholderContainer, ContainerObject)

   MotionPlaceholderContainer();

   UINT GetNumMotionPlaceholders() const { return GetNumChildren(); }
   MotionPlaceholder *GetMotionPlaceholderAt( UINT nIndex ) { return (MotionPlaceholder *)GetChildAt(nIndex); }
};

class MotionPlaceholderPointerProperty : public PointerProperty<MotionPlaceholder *>
{
};

BaseStream &operator << (BaseStream &bs, const MotionPlaceholder *&value );
BaseStream &operator >> (BaseStream &bs, MotionPlaceholder *&value );

class MotionChannelBiasObject;

class MotionChannelBiasDriverContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(MotionChannelBiasDriverContainer, ContainerObject)

   MotionChannelBiasDriverContainer();

   UINT GetNumMotionChannelBiasDrivers() const { return GetNumChildren(); }
   MotionChannelBiasObject *GetMotionChannelBiasDriverAt( UINT nIndex ) { return (MotionChannelBiasObject *)GetChildAt(nIndex); }
};

class MotionSet;

class MotionChannelBiasObject : public HierObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(MotionChannelBiasObject, HierObject)

   static Property::Info m_ReductionBiasInfo;
   static FloatProperty::Info m_fTranslationBiasInfo;
   static FloatProperty::Info m_fRotationBiasInfo;
   static FloatProperty::Info m_fScaleBiasInfo;

   Property m_ReductionBias;
   FloatProperty m_fTranslationBias;
   FloatProperty m_fRotationBias;
   FloatProperty m_fScaleBias;

   float m_fTranslationReductionToleranceParentCache;
   float m_fRotationReductionToleranceParentCache;
   float m_fScaleReductionToleranceParentCache;

   TransformableObject *m_pOriginalTransformableObject;
   TransformableObject *m_pReducedTransformableObject;

   MotionChannelBiasObject();

   static int GetNumPickElements() { return 2; }

// Operations
   FloatProperty *GetTranslationBiasProperty() { return &m_fTranslationBias; }
   FloatProperty *GetRotationBiasProperty() { return &m_fRotationBias; }
   FloatProperty *GetScaleBiasProperty() { return &m_fScaleBias; }

   MotionSet *GetMotionSet() { return (MotionSet *)GetCommandObject(); }
   UINT GetNumMotionCurveDriverBiasObjects();
   MotionCurveDriver *GetMotionCurveDriverBiasObjectAt( UINT nIndex );
   void ApplyDrivers( MotionPlaceholder *pMotionPlaceholder, SnipeObject *pObject );
   void PropagateTolerances();

   void CreateViewControl();
   void DestroyViewControl(); 

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual void DrawTreeItem( CGLControl *pItem, float x=0.0f );
   virtual void Draw( CGLBaseView *pBaseView );
   virtual void DrawSelected( CGLBaseView *pView );
   virtual void DrawMirrored( CGLBaseView *pView );
   virtual void DrawFocused( CGLBaseView *pView );

};

template<class TYPE>
class MotionCurveDriverBiasObject : public TYPE
{
public:
//   static Property::Info m_fReductionBiasInfo;
   FloatProperty m_fReductionBias;
   float m_fReductionToleranceParentCache;
   float m_fReductionTolerance;

   TYPE *m_pMotionCurveDriver;
   UINT  m_bCurvesNeedRebuilt : 1;

   MotionCurveDriverBiasObject();
  
   void RebuildCurves();

// PropertyDriver
   virtual BOOL IsApplyDirty( Time time ) { return TRUE; }

// MotionCurveDriver overrides
   virtual void SetMotionCurveDriver( MotionCurveDriver *pMotionCurveDriver );
   virtual void SetReductionBias(  float fBias );
   virtual void SetReductionToleranceParentCache(  float fTolerance );
   virtual void Validate();
   virtual FloatProperty *GetReductionBiasProperty() { return &m_fReductionBias; }

// SnipeObject overrides
   virtual void Draw( CGLBaseView *pView );
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual BOOL GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList ) { return FALSE; } // anything goes
   virtual void DrawTreeItem( CGLControl *pItem, float x=0.0f );

};

class FloatMotionCurveDriverBiasObject : public MotionCurveDriverBiasObject<FloatMotionCurveDriver>
{
public:
   DECLARE_SNIPEOBJECT_CREATE(FloatMotionCurveDriverBiasObject, MotionCurveDriverBiasObject<FloatMotionCurveDriver>)
};

class Vector2MotionCurveDriverBiasObject : public MotionCurveDriverBiasObject<Vector2MotionCurveDriver>
{
public:
   DECLARE_SNIPEOBJECT_CREATE(Vector2MotionCurveDriverBiasObject, MotionCurveDriverBiasObject<Vector2MotionCurveDriver>)
};

class VectorMotionCurveDriverBiasObject : public MotionCurveDriverBiasObject<VectorMotionCurveDriver>
{
public:
   DECLARE_SNIPEOBJECT_CREATE(VectorMotionCurveDriverBiasObject, MotionCurveDriverBiasObject<VectorMotionCurveDriver>)
};

class EulerMotionCurveDriverBiasObject : public MotionCurveDriverBiasObject<EulerMotionCurveDriver>
{
public:
   DECLARE_SNIPEOBJECT_CREATE(EulerMotionCurveDriverBiasObject, MotionCurveDriverBiasObject<EulerMotionCurveDriver>)
};

class QuaternionMotionCurveDriverBiasObject : public MotionCurveDriverBiasObject<QuaternionMotionCurveDriver>
{
public:
   DECLARE_SNIPEOBJECT_CREATE(QuaternionMotionCurveDriverBiasObject, MotionCurveDriverBiasObject<QuaternionMotionCurveDriver>)
};

class MotionInstanceContainer;

class MotionSet : public Scene
{
  DECLARE_MESSAGE_MAP()
public:
   static ModelPointerProperty::Info             m_pModelInfo;
   static MotionPlaceholderPointerProperty::Info m_pCurrentMotionPlaceholderInfo;

   ModelPointerProperty             m_pModel;
   MotionPlaceholderPointerProperty m_pCurrentMotionPlaceholder;

   static Property::Info m_ReductionToleranceInfo;
   static FloatProperty::Info m_fTranslationToleranceInfo;
   static FloatProperty::Info m_fRotationToleranceInfo;
   static FloatProperty::Info m_fScaleToleranceInfo;

   Property m_ReductionTolerance;
   FloatProperty m_fTranslationTolerance;
   FloatProperty m_fRotationTolerance;
   FloatProperty m_fScaleTolerance;

private:
   MotionChannelBiasDriverContainer *m_pMotionChannelBiasDriversContainer;
   MotionPlaceholderContainer    *m_pMotionPlaceholderContainer;

public:
   PolyModel                     *m_pOriginalModelInstance;
   PolyModel                     *m_pReducedModelInstance;
   PolyModel                     *m_pCurModelInstance; // the one to draw (points to one of the above)

   DECLARE_SNIPEOBJECT_CREATE(MotionSet, Scene)
   DECLARE_COMMANDOBJECT()

            MotionSet();
   virtual ~MotionSet();

   FloatProperty *GetTranslationToleranceProperty() { return &m_fTranslationTolerance; }
   FloatProperty *GetRotationToleranceProperty() { return &m_fRotationTolerance; }
   FloatProperty *GetScaleToleranceProperty() { return &m_fScaleTolerance; }

   // motion channel bias defaults
   MotionChannelBiasDriverContainer *CreateMotionChannelBiasDriversContainer();
   MotionChannelBiasDriverContainer *GetMotionChannelBiasDriversContainer();
   UINT GetNumMotionChannelBiasObjects();
   MotionChannelBiasObject *GetMotionChannelBiasObjectAt( UINT nIndex ) { return m_pMotionChannelBiasDriversContainer->GetMotionChannelBiasDriverAt(nIndex); }
   MotionChannelBiasObject *GetMotionChannelBiasObject( const String &strName );
   MotionChannelBiasObject *AddMotionChannelBiasObject( TransformableObject *pOriginalTransformableObject, TransformableObject *pReducedTransformableObject );

   MotionPlaceholderContainer *CreateMotionPlaceholderContainer();
   MotionPlaceholderContainer *GetMotionPlaceholderContainer();
   UINT GetNumMotionPlaceholders();
   MotionPlaceholder *GetMotionPlaceholderAt(UINT nIndex) { return m_pMotionPlaceholderContainer->GetMotionPlaceholderAt(nIndex); }
   HierObject *AddMotion( Motion *pMotion );

   void AddMotionChannelBiasDrivers( TransformableObject *pOriginalTransformableObject, TransformableObject *pReducedTransformableObject );
   void PropagateTolerances();
   void ApplyMotionToModels();

//   void DirtyModifiedMotionCurves();

   // Message Maps
   afx_msg void OnUpdateExportMotionSet(CCmdUI* pCmdUI);
   afx_msg void OnExportMotionSet();

   //InstanceObject overrides
   virtual void Draw( CGLBaseView *pView );

   // HierObject overrides
   virtual CSnipeDocTemplate *GetDocTemplate();

// SnipeObject overrides
   virtual UINT GetIconIndex() { return ICONINDEX_MOTION; };
   virtual UINT GetDefaultMenuItemID();
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual BOOL GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList );
};

#include "MotionSet.hpp"

