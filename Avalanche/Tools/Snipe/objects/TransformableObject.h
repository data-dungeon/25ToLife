#pragma once

#include "HierObject.h"
#include "../Properties/Property.h"
#include "../Utility/Matrix44.h"

class CGL2DView;
class PolyModel;

class TransformableObject : public HierObject
{
protected:
   static Property::Info            m_boneInfo;
     static ColorProperty::Info       m_cBoneColorInfo;
     static BoolProperty::Info        m_bChainedInfo;
     static BoolProperty::Info        m_bLockIKInfo;
     static TranslateProperty::Info   m_vStartInfo;
     static TranslateProperty::Info   m_vEndInfo;
     static RotateProperty::Info      m_vRotateInfo;
     static FloatProperty::Info       m_fLengthInfo;
     static TypeProperty::Info        m_nRollMethodInfo;
     static Property::Info            m_manipulatoroptionsInfo;
       static TypeProperty::Info        m_nManipulationMethodInfo;
       static BoolProperty::Info        m_bLimitManipulatorsInfo;
         static Property::Info            m_allowtranslateInfo;
           static BoolProperty::Info        m_bAllowXTranslateInfo;
           static BoolProperty::Info        m_bAllowYTranslateInfo;
           static BoolProperty::Info        m_bAllowZTranslateInfo;
         static Property::Info            m_allowscaleInfo;
           static BoolProperty::Info        m_bAllowXScaleInfo;
           static BoolProperty::Info        m_bAllowYScaleInfo;
           static BoolProperty::Info        m_bAllowZScaleInfo;
         static Property::Info            m_allowrotateInfo;
           static BoolProperty::Info        m_bAllowXRotateInfo;
           static BoolProperty::Info        m_bAllowYRotateInfo;
           static BoolProperty::Info        m_bAllowZRotateInfo;
   static TransformProperty::Info   m_transformInfo;
   static BoolProperty::Info        m_bVisibleInfo;

public:
   Property            m_bone;
     ColorProperty       m_cBoneColor;
     BoolProperty        m_bChained;
     BoolProperty        m_bLockIK;
     TranslateProperty   m_vStart;
     TranslateProperty   m_vEnd;
     RotateProperty      m_vRotate;
     FloatProperty       m_fLength;
     TypeProperty        m_nRollMethod; enum { RM_ZSINGULARITY, RM_YPOLESSINGULARITY, RM_ROLLHISTORY };
     Property            m_manipulatoroptions;
       TypeProperty        m_nManipulationMethod; enum { MM_STANDARD, MM_TRANSLATEONLY, MM_ROTATEONLY };
       BoolProperty        m_bLimitManipulators;
         Property            m_allowtranslate;
           BoolProperty        m_bAllowXTranslate;
           BoolProperty        m_bAllowYTranslate;
           BoolProperty        m_bAllowZTranslate;
         Property            m_allowscale;
           BoolProperty        m_bAllowXScale;
           BoolProperty        m_bAllowYScale;
           BoolProperty        m_bAllowZScale;
         Property            m_allowrotate;
           BoolProperty        m_bAllowXRotate;
           BoolProperty        m_bAllowYRotate;
           BoolProperty        m_bAllowZRotate;
   TransformProperty   m_transform;
   BoolProperty        m_bVisible;

   DECLARE_SNIPEOBJECT_PURE(TransformableObject, HierObject);

private:
   Matrix44 m_objecttobone,           m_bonetoobject;
   Matrix44 m_objecttoanimatedobject, m_animatedobjecttoobject;
   Matrix44 m_objecttoworld,          m_worldtoobject;
// Matrix44 m_objecttoscreen,         m_screentoobject;

   Time     m_tPrevBoneMatrixTime;
   Time     m_tPrevAnimatedObjectMatrixTime;
   Time     m_tPrevWorldMatrixTime;

public:
            TransformableObject();
   virtual ~TransformableObject();

// Operations
   TranslateProperty *GetStart() { return &m_vStart; }

   TransformProperty &GetTransform() { return m_transform; }
   void CalculateTransform( const Vector &vTarget, float fDistance, float fRotate, float fAngle );

   UINT GetNumTransformableObjects() { return (UINT)GetNumChildren(); }
   TransformableObject *GetTransformableObjectAt( UINT nIndex ) { return (TransformableObject *)GetChildAt( (int)nIndex ); }

   // bone matrices
   void SetBoneMatricesDirty() { m_tPrevBoneMatrixTime.SetTagged(); SetAnimatedObjectMatricesDirty(); }
   BOOL IsBoneMatricesDirty( const Time &time ) { return m_tPrevBoneMatrixTime != time; }
   void CalcBoneMatrices( const Time &time );
   const Matrix44 &GetObjectToBoneMatrix( const Time &time=GetTime() ) { CalcBoneMatrices( time ); return m_objecttobone; }
   const Matrix44 &GetBoneToObjectMatrix( const Time &time=GetTime() ) { CalcBoneMatrices( time ); return m_bonetoobject; }
   const Matrix44 GetBoneToWorldMatrix( const Time &time=GetTime() ) { return GetObjectToWorldMatrix( time ) * GetBoneToObjectMatrix( time ); }
   const Matrix44 GetBoneToScreenMatrix( CGL2DView *p2DView, const Time &time=GetTime() );

   // animated object matrices
   void SetAnimatedObjectMatricesDirty() { m_tPrevAnimatedObjectMatrixTime.SetTagged(); SetWorldMatricesDirty(); }
   BOOL IsAnimatedObjectMatricesDirty( const Time &time ) { return m_tPrevAnimatedObjectMatrixTime != time; }
   void CalcAnimatedObjectMatrices( const Time &time );
   const Matrix44 &GetObjectToAnimatedObjectMatrix( const Time &time=GetTime() ) { CalcAnimatedObjectMatrices( time ); return m_objecttoanimatedobject; }
   const Matrix44 &GetAnimatedObjectToObjectMatrix( const Time &time=GetTime() ) { CalcAnimatedObjectMatrices( time ); return m_animatedobjecttoobject; }

   // world matrices
   BOOL IsWorldMatricesDirty( const Time &time ) { return m_tPrevWorldMatrixTime != time; }
   void CalcWorldMatrices( const Time &time );

   // properties
   ColorProperty *GetBoneColor() { return &m_cBoneColor; }

// Overridables
   virt_base virtual void PositionVertices( PolyModel *pModel, BOOL bTessellated ) {}
   virt_base virtual BOOL IsBone() { return FALSE; }

// InstanceObject overrides
   virtual InstanceObject *CreateInstance();

// HierObject overrides
   virtual void OnParent( HierObject *pHierObject );

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual BOOL GetWorldPos( Vector &pos );
   virtual void Transform( const Matrix44 &matrix );
   virtual SnipeObject *FindMirrorObject();
   virtual BOOL IsPickable() { return TRUE; }
   virtual BOOL IsLockable() { return TRUE; }
   virtual void SetWorldMatricesDirty();
   virtual const Matrix44 &GetObjectToWorldMatrix( const Time &time=GetTime() ) { CalcWorldMatrices( time ); return m_objecttoworld; }
   virtual const Matrix44 &GetWorldToObjectMatrix( const Time &time=GetTime() ) { CalcWorldMatrices( time ); return m_worldtoobject; }
};

class TransformableObjectAttribsPusher
{
public:
   CGLBaseView *m_pView;
	GLdouble     m_GLModelViewMatrix[16];
   TransformableObject *m_pObject;

   TransformableObjectAttribsPusher( TransformableObject *pObj, CGLBaseView *pView );
   ~TransformableObjectAttribsPusher();
};
