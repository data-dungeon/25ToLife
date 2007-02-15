#include "stdafx.h"
#include "TransformableObject.h"
#include "../GUI/GL2DView.h"
#include "Scene.h"
#include "MotionSet.h"

#include "GUI/BaseDoc.h"
#include "GUI/SnipeDocTemplate.h"
#include "GUI/MotionDoc.h"
#include "GUI/MotionSetDoc.h"

static TypeProperty::Info::Node f_rollmethodinfos[] = {
   TypeProperty::Info::Node( "Z Singularity", "ZSingularity" ),
   TypeProperty::Info::Node( "Y Poles Singularity", "YPolesSingularity" ),
   TypeProperty::Info::Node( "Roll History", "RollHistory" ),
};

static TypeProperty::Info::Node f_manipulationmethodinfos[] = {
   TypeProperty::Info::Node( "Standard", "Standard" ),
   TypeProperty::Info::Node( "Translate Only", "TranslateOnly" ),
   TypeProperty::Info::Node( "Rotate Only", "RotateOnly" ),
};

Property::Info              TransformableObject::m_boneInfo( "Bone", "Bone" );
   ColorProperty::Info         TransformableObject::m_cBoneColorInfo( "Color", "Color", RGBFloat(1.0), RGBFloat(0.0f), RGBFloat(1.0f) );
   BoolProperty::Info          TransformableObject::m_bChainedInfo( "Chained To Parent", "Chained", FALSE );
   BoolProperty::Info          TransformableObject::m_bLockIKInfo( "Lock IK", "LockIK", FALSE );
   TranslateProperty::Info     TransformableObject::m_vStartInfo( "Start", "Start", 0 );
   TranslateProperty::Info     TransformableObject::m_vEndInfo( "End", "End" );
   RotateProperty::Info        TransformableObject::m_vRotateInfo( "Rotate", "Rotate" );
   FloatProperty::Info         TransformableObject::m_fLengthInfo( "Length", "Length", 10.0f, 0.0f, MAX_FLOAT, 1.0f, FloatProperty::Info::FPT_UNITS );
   TypeProperty::Info          TransformableObject::m_nRollMethodInfo( "Roll Method", "RollMethod", f_rollmethodinfos, sizeof(f_rollmethodinfos)/sizeof(TypeProperty::Info::Node), RM_ZSINGULARITY );
   Property::Info              TransformableObject::m_manipulatoroptionsInfo( "Manipulator Options", "ManipulatorOptions" );
      TypeProperty::Info          TransformableObject::m_nManipulationMethodInfo( "Manipulation", "Manipulation", f_manipulationmethodinfos, sizeof(f_manipulationmethodinfos)/sizeof(TypeProperty::Info::Node), MM_STANDARD );
      BoolProperty::Info          TransformableObject::m_bLimitManipulatorsInfo( "Limit Manipulator", "LimitManipulator", FALSE );
         Property::Info              TransformableObject::m_allowtranslateInfo( "Translate", "Translate" );
            BoolProperty::Info          TransformableObject::m_bAllowXTranslateInfo( "Allow X", "AllowX", TRUE );
            BoolProperty::Info          TransformableObject::m_bAllowYTranslateInfo( "Allow Y", "AllowY", TRUE );
            BoolProperty::Info          TransformableObject::m_bAllowZTranslateInfo( "Allow Z", "AllowZ", TRUE );
         Property::Info              TransformableObject::m_allowscaleInfo( "Scale", "Scale" );
            BoolProperty::Info          TransformableObject::m_bAllowXScaleInfo( "Allow X", "AllowX", TRUE );
            BoolProperty::Info          TransformableObject::m_bAllowYScaleInfo( "Allow Y", "AllowY", TRUE );
            BoolProperty::Info          TransformableObject::m_bAllowZScaleInfo( "Allow Z", "AllowZ", TRUE );
         Property::Info              TransformableObject::m_allowrotateInfo( "Rotate", "Rotate" );
            BoolProperty::Info          TransformableObject::m_bAllowXRotateInfo( "Allow X", "AllowX", TRUE );
            BoolProperty::Info          TransformableObject::m_bAllowYRotateInfo( "Allow Y", "AllowY", TRUE );
            BoolProperty::Info          TransformableObject::m_bAllowZRotateInfo( "Allow Z", "AllowZ", TRUE );

TransformProperty::Info  TransformableObject::m_transformInfo( "Transform", "Transform" );
BoolProperty::Info       TransformableObject::m_bVisibleInfo( "Visible", "Visible", TRUE );

IMPLEMENT_SNIPEOBJECT_PURE( TransformableObject )

TransformableObject::TransformableObject()
{
   m_bone.Init( &m_boneInfo, this );
      m_cBoneColor.Init( &m_cBoneColorInfo, &m_bone );
      m_bChained.Init( &m_bChainedInfo, &m_bone );
      m_bLockIK.Init( &m_bLockIKInfo, &m_bone );
      m_vStart.Init( &m_vStartInfo, &m_bone );
      m_vEnd.Init( &m_vEndInfo, &m_bone );
      m_vRotate.Init( &m_vRotateInfo, &m_bone );
      m_fLength.Init( &m_fLengthInfo, &m_bone );
      m_nRollMethod.Init( &m_nRollMethodInfo, &m_bone );
      m_manipulatoroptions.Init( &m_manipulatoroptionsInfo, &m_bone );
        m_nManipulationMethod.Init( &m_nManipulationMethodInfo, &m_manipulatoroptions );
        m_bLimitManipulators.Init( &m_bLimitManipulatorsInfo, &m_manipulatoroptions );
          m_allowtranslate.Init( &m_allowtranslateInfo, &m_bLimitManipulators );
            m_bAllowXTranslate.Init( &m_bAllowXTranslateInfo, &m_allowtranslate );
            m_bAllowYTranslate.Init( &m_bAllowYTranslateInfo, &m_allowtranslate );
            m_bAllowZTranslate.Init( &m_bAllowZTranslateInfo, &m_allowtranslate );
          m_allowscale.Init( &m_allowscaleInfo, &m_bLimitManipulators );
            m_bAllowXScale.Init( &m_bAllowXScaleInfo, &m_allowscale );
            m_bAllowYScale.Init( &m_bAllowYScaleInfo, &m_allowscale );
            m_bAllowZScale.Init( &m_bAllowZScaleInfo, &m_allowscale );
          m_allowrotate.Init( &m_allowrotateInfo, &m_bLimitManipulators );
            m_bAllowXRotate.Init( &m_bAllowXRotateInfo, &m_allowrotate );
            m_bAllowYRotate.Init( &m_bAllowYRotateInfo, &m_allowrotate );
            m_bAllowZRotate.Init( &m_bAllowZRotateInfo, &m_allowrotate );
   m_transform.Init( &m_transformInfo, this );
   m_bVisible.Init( &m_bVisibleInfo, this );
   m_bVisible.SetVisible( FALSE );

   SetBoneMatricesDirty();
}

TransformableObject::~TransformableObject()
{
}

void TransformableObject::CalculateTransform( const Vector &vTarget, float fDistance, float fRotate, float fAngle )
{
   Matrix44 temp, cameramatrix;
   cameramatrix.Identity();

   temp.RotationMatrix(fRotate*(float)DEGTORAD,0.0f,1.0f,0.0f);
   cameramatrix *= temp;
   temp.RotationMatrix(fAngle*(float)DEGTORAD,1.0f,0.0f,0.0f);
   cameramatrix *= temp;

   Vector translate = Vector( 0.0f, 0.0f, fDistance );
   translate *= cameramatrix;
   translate += vTarget;
   m_transform.m_vTranslate.StoreValue( translate );

   Vector rotate( fAngle, fRotate, 0.0f );
   m_transform.m_vRotate.StoreValue( rotate );
}

static SnipeObject *FindMirrorBranch( SnipeObject *pObject, const Vector &vMatch  )
{
   Vector vDelta;
   for ( int i=0; i<pObject->GetNumChildren(); i++)
   {
      SnipeObject *pChild = pObject->GetChildAt(i);
      if (!pChild->IsKindOf(SOT_TransformableObject))
         continue;
      pChild->GetWorldPos(vDelta);
      vDelta-=vMatch;

      if (fabs(vDelta.x) < MIRRORTOLERANCE
       && fabs(vDelta.y) < MIRRORTOLERANCE
       && fabs(vDelta.z) < MIRRORTOLERANCE)
         return pChild;

      FindMirrorBranch( pChild, vMatch );
   }
   return NULL;
}

SnipeObject *TransformableObject::FindMirrorObject()
{
   SnipeObject *pCommandObject = GetCommandObject();
   if ( pCommandObject == NULL )
      return NULL;

   Vector vMatch;
   GetWorldPos( vMatch );
   vMatch.x = -vMatch.x;

   return FindMirrorBranch( pCommandObject, vMatch );
}

InstanceObject *TransformableObject::CreateInstance()
{
   TransformableObject *pInstance = (TransformableObject *)HierObject::CreateInstance();
   pInstance->m_bVisible.SetVisible( TRUE );
   return pInstance;
}

void TransformableObject::CalcBoneMatrices( const Time &time )
{
   if (!IsBoneMatricesDirty( time ))
      return;

   Vector vRotate = m_vRotate.GetValue( time );
   m_objecttobone.RotationMatrix( -vRotate.z * DEGTORAD, 0, 0, 1 );//z rotation
   Matrix44 temp;
   temp.RotationMatrix( -vRotate.x * DEGTORAD, 1, 0, 0 );//x rotation
   m_objecttobone *= temp;
   temp.RotationMatrix( -vRotate.y * DEGTORAD, 0, 1, 0 );//y rotation
   m_objecttobone *= temp;

   m_objecttobone.SetTranslation( m_objecttobone * -m_vStart.GetValue( time ) );

   m_bonetoobject.Invert( m_objecttobone );

   m_tPrevBoneMatrixTime = time;

   SetAnimatedObjectMatricesDirty();
}

void TransformableObject::CalcAnimatedObjectMatrices( const Time &time )
{
   if (!IsAnimatedObjectMatricesDirty( time ))
      return;

   CalcBoneMatrices( time );

   m_objecttoanimatedobject = m_bonetoobject * GetTransform().GetMatrix( time )        * m_objecttobone;
   m_animatedobjecttoobject = m_objecttobone * GetTransform().GetInverseMatrix( time ) * m_bonetoobject;

   m_tPrevAnimatedObjectMatrixTime = time;

   SetWorldMatricesDirty();
}

void TransformableObject::CalcWorldMatrices( const Time &time )
{
   if (!IsWorldMatricesDirty( time ))
      return;

   if (m_pParent)
   {
      m_objecttoworld = m_pParent->GetObjectToWorldMatrix( time ) * GetObjectToAnimatedObjectMatrix( time );
      m_worldtoobject = GetAnimatedObjectToObjectMatrix( time ) * m_pParent->GetWorldToObjectMatrix( time );
   }
   else
   {
      m_objecttoworld = GetObjectToAnimatedObjectMatrix( time );
      m_worldtoobject = GetAnimatedObjectToObjectMatrix( time );
   }

   m_tPrevWorldMatrixTime = time;
}

void TransformableObject::SetWorldMatricesDirty()
{
   if (m_tPrevWorldMatrixTime.IsTagged())
      return;

   m_tPrevWorldMatrixTime.SetTagged();

   for (int i=0; i<GetNumChildren(); i++)
      GetChildAt( i )->SetWorldMatricesDirty();
}

const Matrix44 TransformableObject::GetBoneToScreenMatrix( CGL2DView *p2DView, const Time &time/*=GetTime()*/ )
{
   return p2DView->GetWorldToViewMatrix() * GetObjectToWorldMatrix( time ) * GetBoneToObjectMatrix( time );
}

BOOL TransformableObject::GetWorldPos( Vector &pos )
{
   pos.Set(0);
   pos *= ((TransformableObject *)this)->GetObjectToWorldMatrix();

   return TRUE;
}

void TransformableObject::Transform( const Matrix44 &matrix )
{
   if (m_bTransformed || IsLocked())
      return;

   Matrix44 newobjecttoworld = matrix * GetObjectToWorldMatrix();
   Vector translate;
   newobjecttoworld.GetTranslation( translate );
   m_transform.m_vTranslate.StoreValue( translate );
   RotateEuler euler = RotateEuler( newobjecttoworld );
   m_transform.m_vRotate.StoreValue( Vector( euler.x, euler.y, euler.z ));

   m_bTransformed = TRUE;
}

BOOL TransformableObject::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         if (pProperty == &m_transform.m_vTranslate
          || pProperty == &m_transform.m_vScale
          || pProperty == &m_transform.m_vRotate)
            SetAnimatedObjectMatricesDirty();
         else if (pProperty == &m_vStart
          || pProperty == &m_vRotate
          || pProperty == &m_fLength)
            SetBoneMatricesDirty();
         break;
      }
   }

   return HierObject::OnObjectNotify( msg, wParam, lParam );
}

void TransformableObject::OnParent( HierObject *pHierObject )
{
   HierObject::OnParent(pHierObject);

   switch (pHierObject->GetSOT())
   {
      case SOT_Scene:
      {
         Scene *pScene = (Scene*)pHierObject;
         pScene->AddInstance(this);
         return;
      }
      case SOT_Motion:
      {
         CSnipeDocTemplate *pTemplate = pHierObject->GetDocTemplate();
         if (pTemplate)
         {
            for (POSITION pos = pTemplate->GetFirstDocPosition(); pos; )
            {
               CBaseDoc *pDocument = (CBaseDoc*)pTemplate->GetNextDoc(pos);
               SnipeDocType nType = pDocument->GetDocType();
               if (nType == SDT_CMotionDoc)
               {
                  CMotionDoc *pMotionDoc = (CMotionDoc*)pDocument;
                  pMotionDoc->ApplyModel(this);
               }
            }
         }
         return;
      }
      case SOT_MotionSet:
      {
         MotionSet *pMotionSet = (MotionSet*)pHierObject;
         if (IsKindOf(SOT_PolyModel))
         {
            PolyModel *pPolyModel = (PolyModel*)this;
            pMotionSet->m_pModel.StoreValue(pPolyModel);
         }
         return;
      }
   }

   if (pHierObject->IsKindOf(SOT_TransformableObject))
   {
        MainInterface *pMI = GetMainInterface();

        String strChildDescription,strParentDescription;
        pMI->ObjectToDescription( pHierObject, strParentDescription, TRUE );

        pMI->ObjectToDescription( this, strChildDescription, TRUE );

        char buffer[256];
        sprintf(buffer,"-p %s -c %s",strParentDescription.Get(),strChildDescription.Get());

        StringList strResults;
        pMI->DoCommand("Parent",String(buffer),strResults);
   }
}