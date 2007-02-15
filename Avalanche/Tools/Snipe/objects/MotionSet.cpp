//

#include "stdafx.h"
#include "resource.h"
#include "MotionSet.h"
#include "ContainerObject.h"
#include "Properties/BoneDriversContainer.h"
#include "Properties/MotionChannel.h"
#include "../GUI/MainFrm.h"
#include "../GUI/GLMotionSetView.h"
#include "Interface/Implement.h"
#include "Motion.h"
#include "ModelContainer.h"
#include "Project.h"
#include "Bone.h"
#include "GameEngineModifiedMotionCurve.h"
#include "MotionSetTranslatorPluginManager.h"
#include "GUI/ModelDoc.h"
#include "GUI/GLMotionReductionViewControl.h"

/////////////////////////////////////////////////////
// BoneMotionBiasDriverContainer

IMPLEMENT_SNIPEOBJECT_CREATE( MotionChannelBiasDriverContainer )

MotionChannelBiasDriverContainer::MotionChannelBiasDriverContainer()
{
   m_strName = "Default Reduction Tolerances";
}

/////////////////////////////////////////////////////
// MotionChannelBiasObject

Property::Info MotionChannelBiasObject::m_ReductionBiasInfo( "Reduction Bias", "ReductionBias" );
FloatProperty::Info MotionChannelBiasObject::m_fTranslationBiasInfo( "Translation Bias", "TranslationBias", 0.0f, -1.0f, 1.0f, 100.0f, FloatProperty::Info::FPT_PERCENT, PIF_SHOWVALUEWHENNOTSET );
FloatProperty::Info MotionChannelBiasObject::m_fRotationBiasInfo( "Rotation Bias", "RotationBias", 0.0f, -1.0f, 1.0f, 100.0f, FloatProperty::Info::FPT_PERCENT, PIF_SHOWVALUEWHENNOTSET );
FloatProperty::Info MotionChannelBiasObject::m_fScaleBiasInfo( "Scale Bias", "ScaleBias", 0.0f, -1.0f, 1.0f, 100.0f, FloatProperty::Info::FPT_PERCENT, PIF_SHOWVALUEWHENNOTSET );

IMPLEMENT_SNIPEOBJECT_CREATE( MotionChannelBiasObject )

MotionChannelBiasObject::MotionChannelBiasObject()
{
   m_ReductionBias.Init( &m_ReductionBiasInfo, this );
    m_fTranslationBias.Init( &m_fTranslationBiasInfo, &m_ReductionBias );
    m_fRotationBias.Init( &m_fRotationBiasInfo, &m_ReductionBias );
    m_fScaleBias.Init( &m_fScaleBiasInfo, &m_ReductionBias );
   m_fTranslationBias.SetSet( FALSE );
   m_fScaleBias.SetSet( FALSE );
   m_fRotationBias.SetSet( FALSE );
   m_pOriginalTransformableObject = NULL;
   m_pReducedTransformableObject = NULL;
}

UINT MotionChannelBiasObject::GetNumMotionCurveDriverBiasObjects()
{
   return GetNumChildren();
}

MotionCurveDriver *MotionChannelBiasObject::GetMotionCurveDriverBiasObjectAt( UINT nIndex )
{
   return (MotionCurveDriver *)GetChildAt( nIndex );
}

void MotionChannelBiasObject::ApplyDrivers( MotionPlaceholder *pMotionPlaceholder, SnipeObject *pObject )
{
   static String strError;
   SnipeObject *pOldWorkingObject = GetMainInterface()->SetCurrentWorkingObject( pObject );
   int nDriverCount = GetNumMotionCurveDriverBiasObjects();

   for (int i=0; i<nDriverCount; i++)
   {
      MotionCurveDriver *pMotionCurveDriver = GetMotionCurveDriverBiasObjectAt( i );
      String strMatchName = pMotionCurveDriver->GetMatchName();
      Property *pProperty = (Property *)GetMainInterface()->DescriptionToObject( strMatchName, &strError );
      if (pProperty)
      {
         ASSERT( pProperty->IsKindOf( SOT_Property ));
         pMotionCurveDriver->m_pProperty.StoreValue( pProperty );
         pProperty->OnValueChanged();
      }
   }

   GetMainInterface()->SetCurrentWorkingObject( pOldWorkingObject );
}

void MotionChannelBiasObject::PropagateTolerances()
{
   // MotionChannelBiasObjects are used in two places, each needing to propagate differently
   if (GetParent() && GetParent()->GetSOT() == SOT_MotionPlaceholder) // in this case, just walk all child MotionCurveDrivers and set them
   {
      for (int i=0; i<GetNumChildren(); i++)
      {
         MotionCurveDriver *pMotionCurveDriver = (MotionCurveDriver *)GetChildAt( i );

         if (stricmp(pMotionCurveDriver->GetMatchName().Get(),"Transform.Translate")==0)
         { 
            pMotionCurveDriver->SetReductionToleranceParentCache(m_fTranslationReductionToleranceParentCache + m_fTranslationBias.GetValue());
         }
         else if (stricmp(pMotionCurveDriver->GetMatchName().Get(),"Transform.Rotate")==0)
         {
            pMotionCurveDriver->SetReductionToleranceParentCache(m_fRotationReductionToleranceParentCache + m_fRotationBias.GetValue());
         }
         else if (stricmp(pMotionCurveDriver->GetMatchName().Get(),"Transform.Scale")==0)
         {
            pMotionCurveDriver->SetReductionToleranceParentCache(m_fScaleReductionToleranceParentCache + m_fScaleBias.GetValue());
         }
       }
   }
   else // in this case, walk all MotionPlaceholders looking for matching channel names, and set them
   {
      MotionSet *pMotionSet = GetMotionSet();
      if (!pMotionSet)
         return;

      MotionPlaceholderContainer *pMotionPlaceholderContainer = pMotionSet->GetMotionPlaceholderContainer();
      if (pMotionPlaceholderContainer)
      {
         for (UINT i=0; i<pMotionPlaceholderContainer->GetNumMotionPlaceholders(); i++)
         {
            MotionPlaceholder *pMotionPlaceholder = pMotionPlaceholderContainer->GetMotionPlaceholderAt( i );
            MotionChannelBiasObject *pMotionChannelBiasObject = (MotionChannelBiasObject *)pMotionPlaceholder->FindObject( GetMatchName() );
            if (pMotionChannelBiasObject && !pMotionChannelBiasObject->m_fTranslationBias.IsSet() && pMotionChannelBiasObject->m_fTranslationBias!=m_fTranslationBias)
            {
               pMotionChannelBiasObject->m_fTranslationBias.SetValue( m_fTranslationBias.GetValue() );
               pMotionChannelBiasObject->m_fTranslationBias.OnValueChanged();
            }
            if (pMotionChannelBiasObject && !pMotionChannelBiasObject->m_fRotationBias.IsSet() && pMotionChannelBiasObject->m_fRotationBias!=m_fRotationBias)
            {
               pMotionChannelBiasObject->m_fRotationBias.SetValue( m_fRotationBias.GetValue() );
               pMotionChannelBiasObject->m_fRotationBias.OnValueChanged();
            }
            if (pMotionChannelBiasObject && !pMotionChannelBiasObject->m_fScaleBias.IsSet() && pMotionChannelBiasObject->m_fScaleBias!=m_fScaleBias)
            {
               pMotionChannelBiasObject->m_fScaleBias.SetValue( m_fScaleBias.GetValue() );
               pMotionChannelBiasObject->m_fScaleBias.OnValueChanged();
            }
         }
      }
   }
}

BOOL MotionChannelBiasObject::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         BOOL bStoreUndo = (BOOL)wParam;

         if ((pProperty == &m_fTranslationBias)
            ||(pProperty == &m_fRotationBias)
            ||(pProperty == &m_fScaleBias))
         {
            PropagateTolerances();
            NotifyNodes( NOTIFYOBJ_ICONCHANGED, 0, (LPARAM)this );
         }
         break;
      }
      case NOTIFYOBJ_SELECTIONCHANGED:
      {
         BOOL bSelect = (BOOL)wParam;
         if (bSelect)
         {
            SetVisibleInTree(TRUE);
            CreateViewControl();
         }
         else
         {
            SetVisibleInTree(FALSE);
            DestroyViewControl();
         }
      }
      break;
   }

   return HierObject::OnObjectNotify( msg, wParam, lParam );
}

void MotionChannelBiasObject::CreateViewControl()
{
   for (POSITION docpos=GetApp().GetDocList().GetHeadPosition(); docpos; )
   {
      CBaseDoc *pDoc = GetApp().GetDocList().GetNext( docpos );
      MotionSet *pMotionSet = (MotionSet*)GetParent()->GetParent();
      if (pDoc->UsesObject( pMotionSet ))
      {
         for (POSITION viewpos=pDoc->m_viewlist.GetHeadPosition(); viewpos; )
         {
            CGLBaseView *pView = pDoc->m_viewlist.GetNext( viewpos );
            SNEW CGLMotionChannelObjectBiasViewControl( pView, this );
         }
      }
   }
}

void MotionChannelBiasObject::DestroyViewControl()
{
   for (POSITION docpos=GetApp().GetDocList().GetHeadPosition(); docpos; )
   {
      CBaseDoc *pDoc = GetApp().GetDocList().GetNext( docpos );
      MotionSet *pMotionSet = (MotionSet*)GetParent()->GetParent();
      if (pDoc->UsesObject( this ))
      {
         for (POSITION viewpos=pDoc->m_viewlist.GetHeadPosition(); viewpos; )
         {
            CGLBaseView *pView = pDoc->m_viewlist.GetNext( viewpos );

            POSITION controlpos=pView->GetViewControlList().GetHeadPosition();
            while (controlpos)
            {
               for (controlpos=pView->GetViewControlList().GetHeadPosition(); controlpos; )
               {
                  CGLViewControl *pControl = pView->GetViewControlList().GetNext( controlpos );
                  if (pControl->IsFor( this ))
                  {
                     delete pControl;
                     break;
                  }
               }
            }
         }
      }
   }
}

void MotionChannelBiasObject::DrawTreeItem( CGLControl *pItem, float x/*=0.0f*/ )
{
   HierObject::DrawTreeItem( pItem, x );

   if (!m_ReductionBias.IsVisible())
      return;

   BOOL bIsReductionSet = m_fTranslationBias.IsSet()||m_fRotationBias.IsSet()||m_fScaleBias.IsSet();

   glColor4f( 0.1f, 0.1f, 0.1f, bIsReductionSet ? 1.0f : 0.3f );

   String strText,strTemp;
   m_fTranslationBias.GetValueString( strTemp );
   strText = strTemp + " ";
   m_fRotationBias.GetValueString( strTemp );
   strText += strTemp+ " ";
   m_fScaleBias.GetValueString( strTemp );
   strText += strTemp;

   CSize szText = pItem->GetTextExtent( strText );
   pItem->DrawText( pItem->m_szControl.cx - szText.cx - 8, 12, strText );

   if (bIsReductionSet)
   {
      glColor3f( 0.1f, 0.1f, 0.1f );
      pItem->DrawText( x+CGLObjectTreeControl::m_glImageListTreeIcons.GetWidthEach()+3, 12, GetTitle());
      pItem->DrawText( pItem->m_szControl.cx - szText.cx - 9, 12, strText );
   }
}

void MotionChannelBiasObject::Draw( CGLBaseView *pBaseView )
{
   if (pBaseView->GetCurrentSelectionSOT() != SOT_MotionChannelBiasObject)
      return; // to prevent traversing the entire bone hierarchy

   TransformableObject *pTransformableObject = m_pOriginalTransformableObject;
   
   while (TRUE)
   {
      if (!pTransformableObject->IsBone())
         return;

      Bone *pBone = (Bone*)pTransformableObject;

      BOOL bDrawBone = TRUE;
      BOOL bDrawEnd = TRUE;

      if (pBone->m_bVisible)
      {
         if (pBaseView->m_bDoPick && IsKindOf( pBaseView->m_nPickObjectType ))
         {
            glLoadName( (GLuint)this );
            switch(SnipeObject::GetCurrentPickElement())
            {
            case -1 :
            case 1 :
               {
                  bDrawEnd = FALSE;
                  bDrawBone = TRUE;
               }
               break;
            case 0 :
               {
                  bDrawEnd = TRUE;
                  bDrawBone = FALSE;
               }
               break;
            }
         }

         RGBAFloat color( pBone->GetBoneColor()->GetValue(), 0.5f );

         BOOL bOldDepthTest = glSet( GL_DEPTH_TEST, FALSE );
         pBone->Draw( pBaseView, color, bDrawBone, bDrawEnd );
         glSet( GL_DEPTH_TEST, bOldDepthTest );

         if (pBaseView->m_bDoPick && IsKindOf( pBaseView->m_nPickObjectType ))
            glLoadName( NULL );
      }
      if (pTransformableObject == m_pReducedTransformableObject)
         break;
      pTransformableObject = m_pReducedTransformableObject;
   }
}

void MotionChannelBiasObject::DrawFocused( CGLBaseView *pBaseView )
{
   TransformableObject *pTransformableObject = m_pOriginalTransformableObject;

   while (TRUE)
   {
      if (!pTransformableObject->IsBone())
         return;

      Bone *pBone = (Bone*)pTransformableObject;

      pBone->DrawFocused(pBaseView);

      if (pTransformableObject == m_pReducedTransformableObject)
         break;
      pTransformableObject = m_pReducedTransformableObject;
   }
}

void MotionChannelBiasObject::DrawSelected( CGLBaseView *pBaseView )
{
   TransformableObject *pTransformableObject = m_pOriginalTransformableObject;

   while (TRUE)
   {
      if (!pTransformableObject->IsBone())
         return;

      Bone *pBone = (Bone*)pTransformableObject;

      pBone->DrawSelected(pBaseView);

      if (pTransformableObject == m_pReducedTransformableObject)
         break;
      pTransformableObject = m_pReducedTransformableObject;
   }
}

void MotionChannelBiasObject::DrawMirrored( CGLBaseView *pBaseView )
{
   TransformableObject *pTransformableObject = m_pOriginalTransformableObject;

   while (TRUE)
   {

      if (!pTransformableObject->IsBone())
         return;

      Bone *pBone = (Bone*)pTransformableObject;

      pBone->DrawMirrored(pBaseView);

      if (pTransformableObject == m_pReducedTransformableObject)
         break;
      pTransformableObject = m_pReducedTransformableObject;
   }
}

/////////////////////////////////////////////////////
// MotionCurveDriverBiasObjects

IMPLEMENT_SNIPEOBJECT_CREATE( FloatMotionCurveDriverBiasObject )

MotionCurveDriver *FloatMotionCurveDriver::CreateMotionCurveDriverBiasObject()
{
   return (MotionCurveDriver *)FloatMotionCurveDriverBiasObject::Create();
}

IMPLEMENT_SNIPEOBJECT_CREATE( Vector2MotionCurveDriverBiasObject )

MotionCurveDriver *Vector2MotionCurveDriver::CreateMotionCurveDriverBiasObject()
{
   return (MotionCurveDriver *)Vector2MotionCurveDriverBiasObject::Create();
}

IMPLEMENT_SNIPEOBJECT_CREATE( VectorMotionCurveDriverBiasObject )

MotionCurveDriver *VectorMotionCurveDriver::CreateMotionCurveDriverBiasObject()
{
   return (MotionCurveDriver *)VectorMotionCurveDriverBiasObject::Create();
}

IMPLEMENT_SNIPEOBJECT_CREATE( EulerMotionCurveDriverBiasObject )

MotionCurveDriver *EulerMotionCurveDriver::CreateMotionCurveDriverBiasObject()
{
   return (MotionCurveDriver *)EulerMotionCurveDriverBiasObject::Create();
}

IMPLEMENT_SNIPEOBJECT_CREATE( QuaternionMotionCurveDriverBiasObject )

MotionCurveDriver *QuaternionMotionCurveDriver::CreateMotionCurveDriverBiasObject()
{
   return (MotionCurveDriver *)QuaternionMotionCurveDriverBiasObject::Create();
}

/////////////////////////////////////////////////////
// MotionSet

IMPLEMENT_SNIPEOBJECT_CREATE( MotionSet )
IMPLEMENT_COMMANDOBJECT( MotionSet )

BEGIN_MESSAGE_MAP(MotionSet, Scene)
   SNIPEOBJECT_ON_UPDATE_COMMAND_UI(ID_EXPORT_MOTIONSET, OnUpdateExportMotionSet)
   SNIPEOBJECT_ON_COMMAND(ID_EXPORT_MOTIONSET, OnExportMotionSet)
END_MESSAGE_MAP()

ModelPointerProperty::Info MotionSet::m_pModelInfo( "Model", "Model" );
MotionPlaceholderPointerProperty::Info MotionSet::m_pCurrentMotionPlaceholderInfo("Current Motion", "CurrentMotion");

Property::Info MotionSet::m_ReductionToleranceInfo( "Motion Reduction Tolerance", "MotionReductionTolerance" );
FloatProperty::Info MotionSet::m_fTranslationToleranceInfo( "Translation Tolerance", "TranslationTolerance", 0.1f, 0.0f, 1.0f, 100.0f, FloatProperty::Info::FPT_PERCENT);
FloatProperty::Info MotionSet::m_fRotationToleranceInfo( "Rotation Tolerance", "RotationTolerance", 0.1f, 0.0f, 1.0f, 100.0f, FloatProperty::Info::FPT_PERCENT);
FloatProperty::Info MotionSet::m_fScaleToleranceInfo( "Scale Tolerance", "ScaleTolerance", 0.1f, 0.0f, 1.0f, 100.0f, FloatProperty::Info::FPT_PERCENT);

MotionSet::MotionSet()
{
   m_strName = "MotionSet1";
   m_pMotionChannelBiasDriversContainer = NULL;
   m_pMotionPlaceholderContainer = NULL;
   m_pOriginalModelInstance = NULL;
   m_pReducedModelInstance = NULL;
   m_pCurModelInstance = NULL;

   m_pModel.Init( &m_pModelInfo, this );
   m_pCurrentMotionPlaceholder.Init( &m_pCurrentMotionPlaceholderInfo, this );

   m_ReductionTolerance.Init( &m_ReductionToleranceInfo, this );
    m_fTranslationTolerance.Init( &m_fTranslationToleranceInfo, &m_ReductionTolerance );
    m_fRotationTolerance.Init( &m_fRotationToleranceInfo, &m_ReductionTolerance );
    m_fScaleTolerance.Init( &m_fScaleToleranceInfo, &m_ReductionTolerance );
}

MotionSet::~MotionSet()
{
}

void MotionSet::PreDestruct()
{
   HierObject::PreDestruct();
}

UINT MotionSet::GetDefaultMenuItemID()
{
   return ID_OBJECT_EDIT;
}

CSnipeDocTemplate *MotionSet::GetDocTemplate()
{
   return GetApp().GetMotionSetDocTemplate();
}

MotionPlaceholderContainer *MotionSet::CreateMotionPlaceholderContainer()
{
   if (!m_pMotionPlaceholderContainer)
   {
      m_pMotionPlaceholderContainer = MotionPlaceholderContainer::Create();
      AddChildAtTail( m_pMotionPlaceholderContainer );
   }
   return m_pMotionPlaceholderContainer;
}

MotionPlaceholderContainer *MotionSet::GetMotionPlaceholderContainer()
{
   return m_pMotionPlaceholderContainer;
}

UINT MotionSet::GetNumMotionChannelBiasObjects()
{
   if (m_pMotionChannelBiasDriversContainer)
      return m_pMotionChannelBiasDriversContainer->GetNumChildren();
   return 0;
}

MotionChannelBiasDriverContainer *MotionSet::CreateMotionChannelBiasDriversContainer()
{
   if (!m_pMotionChannelBiasDriversContainer)
   {
      m_pMotionChannelBiasDriversContainer = MotionChannelBiasDriverContainer::Create();
      AddChildAtTail( m_pMotionChannelBiasDriversContainer );
   }
   return m_pMotionChannelBiasDriversContainer;
}

MotionChannelBiasDriverContainer *MotionSet::GetMotionChannelBiasDriversContainer()
{
   return m_pMotionChannelBiasDriversContainer;
}

MotionChannelBiasObject *MotionSet::GetMotionChannelBiasObject( const String &strName )
{
   for (UINT nDriverIndex = 0; nDriverIndex < GetNumMotionChannelBiasObjects(); nDriverIndex++)
   {
      MotionChannelBiasObject *pMotionChannelBiasObject = GetMotionChannelBiasObjectAt(nDriverIndex);
      if (pMotionChannelBiasObject->GetMatchName() == strName)
         return pMotionChannelBiasObject;
   }

   return NULL;
}

MotionChannelBiasObject *MotionSet::AddMotionChannelBiasObject( TransformableObject *pOriginalTransformableObject, TransformableObject *pReducedTransformableObject )
{
   MotionChannelBiasObject *pMotionChannelBiasObject = GetMotionChannelBiasObject( pOriginalTransformableObject->GetMatchName() );
   if (pMotionChannelBiasObject)
      return pMotionChannelBiasObject;

   pMotionChannelBiasObject = (MotionChannelBiasObject *)MotionChannelBiasObject::Create();
   CreateMotionChannelBiasDriversContainer()->AddChildAtTail( pMotionChannelBiasObject );
   String strName = pOriginalTransformableObject->GetMatchName();
   pMotionChannelBiasObject->SetName( pOriginalTransformableObject->GetMatchName() );
   pMotionChannelBiasObject->m_pOriginalTransformableObject = pOriginalTransformableObject;
   pMotionChannelBiasObject->m_pReducedTransformableObject = pReducedTransformableObject;

   return pMotionChannelBiasObject;
}

UINT MotionSet::GetNumMotionPlaceholders()
{
   if (m_pMotionPlaceholderContainer)
      return m_pMotionPlaceholderContainer->GetNumMotionPlaceholders();
   return 0;
}

HierObject *MotionSet::AddMotion( Motion *pMotion )
{
   CWaitCursor waitcursor;

   MotionPlaceholder *pMotionPlaceholder = (MotionPlaceholder *)MotionPlaceholder::Create();
   pMotionPlaceholder->m_pMotion = pMotion;

   MotionPlaceholderContainer *pMotionPlaceholderContainer = CreateMotionPlaceholderContainer();
   pMotionPlaceholderContainer->AddChildAtTail( pMotionPlaceholder );
   pMotionPlaceholder->SetName( pMotion->GetMatchName() );

   BoneDriversContainer *pBoneDriversContainer = pMotion->GetBoneDriversContainer();
   for (int i=0; i<pBoneDriversContainer->GetNumChildren(); i++)
   {
      SnipeObject *pChild = pBoneDriversContainer->GetChildAt( i );
      if (pChild->IsKindOf( SOT_MotionChannel ))
      {
         MotionChannel *pMotionChannel = (MotionChannel *)pChild;
         MotionChannelBiasObject *pMotionChannelBiasObject = (MotionChannelBiasObject *)MotionChannelBiasObject::Create();
         pMotionChannelBiasObject->m_ReductionBias.SetVisible(FALSE);
         pMotionPlaceholder->AddChildAtTail( pMotionChannelBiasObject );
         pMotionChannelBiasObject->SetName( pChild->GetMatchName() );
         for (UINT nDriver=0; nDriver<pMotionChannel->GetNumMotionCurveDrivers(); nDriver++)
         {
            MotionCurveDriver *pMotionCurveDriver = pMotionChannel->GetMotionCurveDriverAt( nDriver );
            MotionCurveDriver *pMotionCurveDriverBiasObject = pMotionCurveDriver->CreateMotionCurveDriverBiasObject();
            if (pMotionCurveDriverBiasObject)
            {
               pMotionChannelBiasObject->AddChildAtTail( pMotionCurveDriverBiasObject );
               pMotionCurveDriverBiasObject->SetName( pMotionCurveDriver->GetMatchName() );
               pMotionCurveDriverBiasObject->SetMotionCurveDriver( pMotionCurveDriver );
            }
         }
      }
   }

   PropagateTolerances();

   if (pMotionPlaceholderContainer->GetNumMotionPlaceholders()==1)
      m_pCurrentMotionPlaceholder.StoreValue( pMotionPlaceholder );

   return pMotionPlaceholder;
}

BOOL MotionSet::GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList )
{
   if (pPointerProperty == &m_pModel)
   {
      Project *pProject = GetMainInterface()->GetProject();
      if (!pProject)
      {
         ASSERT( FALSE );
         return TRUE;
      }

      ModelContainer *pModelContainer = &pProject->m_modelcontainer;
      for (UINT i=0; i<pModelContainer->GetNumModels(); i++)
      {
         PolyModel *pModel = pModelContainer->GetModelAt( i );
         pSnipeObjectList->AddTail( pModel );
      }
   }
   else if (pPointerProperty == &m_pCurrentMotionPlaceholder)
   {
      Project *pProject = GetMainInterface()->GetProject();
      if (!pProject)
      {
         ASSERT( FALSE );
         return TRUE;
      }

      MotionPlaceholderContainer *pModelPlaceholderContainer = GetMotionPlaceholderContainer();
      if (pModelPlaceholderContainer)
      {
         for (UINT i=0; i<pModelPlaceholderContainer->GetNumMotionPlaceholders(); i++)
         {
            MotionPlaceholder *pMotionPlaceholder = pModelPlaceholderContainer->GetMotionPlaceholderAt( i );
            pSnipeObjectList->AddTail( pMotionPlaceholder );
         }
      }
   }

   return TRUE;
}

void MotionSet::ApplyMotionToModels()
{
   MotionPlaceholder *pMotionPlaceholder = m_pCurrentMotionPlaceholder;
   if (pMotionPlaceholder && m_pReducedModelInstance)
   {
      pMotionPlaceholder->ApplyDrivers( m_pReducedModelInstance );

      Motion *pMotion = pMotionPlaceholder->m_pMotion;
      if (pMotion)
      {
         MotionInstance *pMotionInstance = (MotionInstance *)pMotion->CreateInstance();
         m_pOriginalModelInstance->AddMotionInstance( pMotionInstance );
      }
   }
}

BOOL MotionSet::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         BOOL bStoreUndo = (BOOL)wParam;
  
         if (pProperty == &m_pModel)
         {
            if (m_pOriginalModelInstance)
            {
               m_pOriginalModelInstance->Destroy();
               m_pReducedModelInstance->Destroy();
               m_pOriginalModelInstance = NULL;
               m_pReducedModelInstance = NULL;
            }

            //TODO: RemoveUnusedMotionChannelBiasDrivers();

            PolyModel *pPolyModel = m_pModel;
            if (pPolyModel)
            {
//               if (bStoreUndo)
               {
                  m_pOriginalModelInstance = (PolyModel *)pPolyModel->CreateInstance();
                  m_pReducedModelInstance = (PolyModel *)pPolyModel->CreateInstance();

                  BoneContainer *pOriginalBoneContainer = m_pOriginalModelInstance->GetBoneContainer();
                  BoneContainer *pReducedBoneContainer = m_pReducedModelInstance->GetBoneContainer();

                  for (UINT i=0; i<pOriginalBoneContainer->GetNumTransformableObjects(); i++)
                  {
                     TransformableObject *pOriginalTransformableObject = pOriginalBoneContainer->GetTransformableObjectAt( i );
                     TransformableObject *pReducedTransformableObject = pReducedBoneContainer->GetTransformableObjectAt( i );
                     AddMotionChannelBiasDrivers( pOriginalTransformableObject, pReducedTransformableObject );
                  }
               }

  
               if (m_pCurrentMotionPlaceholder.GetPropertyInfo())
                  ApplyMotionToModels();
            }

            PropagateTolerances();
         }
         else if (pProperty == &m_pCurrentMotionPlaceholder)
            ApplyMotionToModels();
         else if ((pProperty == &m_fTranslationTolerance)
            ||(pProperty == &m_fRotationTolerance)
            ||(pProperty == &m_fScaleTolerance))
            PropagateTolerances();
         break;
      }
      case NOTIFYOBJ_ADDEDCHILD:
      {
         SnipeObject *pChild = (SnipeObject *)lParam;
         if (pChild->GetSOT() == SOT_MotionPlaceholderContainer)
            m_pMotionPlaceholderContainer = (MotionPlaceholderContainer *)pChild;
         else if (pChild->GetSOT() == SOT_MotionChannelBiasDriverContainer)
            m_pMotionChannelBiasDriversContainer = (MotionChannelBiasDriverContainer *)pChild;
         break;
      }
      case NOTIFYOBJ_REMOVEDCHILD:
      {
         SnipeObject *pChild = (SnipeObject *)lParam;
         if (pChild == GetMotionPlaceholderContainer())
            m_pMotionPlaceholderContainer = NULL;
         else if (pChild == GetMotionChannelBiasDriversContainer())
            m_pMotionChannelBiasDriversContainer = NULL;
         break;
      }
   }

   return HierObject::OnObjectNotify( msg, wParam, lParam );
}


void MotionSet::PropagateTolerances()
{
   // first go through all of the MotionPlaceholders
   MotionPlaceholderContainer *pMotionPlaceholderContainer = GetMotionPlaceholderContainer();
   if (pMotionPlaceholderContainer)
   {
      for (UINT i=0; i<pMotionPlaceholderContainer->GetNumMotionPlaceholders(); i++)
      {
         MotionPlaceholder *pMotionPlaceholder = pMotionPlaceholderContainer->GetMotionPlaceholderAt( i );
         for (int j=0; j<pMotionPlaceholder->GetNumChildren(); j++)
         {
            MotionChannelBiasObject *pMotionChannelBiasObject = (MotionChannelBiasObject *)pMotionPlaceholder->GetChildAt( j );
            pMotionChannelBiasObject->m_fTranslationReductionToleranceParentCache = m_fTranslationTolerance.GetValue();
            pMotionChannelBiasObject->m_fRotationReductionToleranceParentCache = m_fRotationTolerance.GetValue();
            pMotionChannelBiasObject->m_fScaleReductionToleranceParentCache = m_fScaleTolerance.GetValue();
            pMotionChannelBiasObject->PropagateTolerances();
         }
      }
   }

   // then through the MotionChannelBiasObjects
   MotionChannelBiasDriverContainer *pMotionChannelBiasDriverContainer = GetMotionChannelBiasDriversContainer();
   if (pMotionChannelBiasDriverContainer)
   {
      for (UINT i=0; i<pMotionChannelBiasDriverContainer->GetNumMotionChannelBiasDrivers(); i++)
      {
         MotionChannelBiasObject *pMotionChannelBiasObject = pMotionChannelBiasDriverContainer->GetMotionChannelBiasDriverAt( i );
         pMotionChannelBiasObject->m_fTranslationReductionToleranceParentCache = m_fTranslationTolerance.GetValue();    
         pMotionChannelBiasObject->m_fRotationReductionToleranceParentCache = m_fRotationTolerance.GetValue();
         pMotionChannelBiasObject->m_fScaleReductionToleranceParentCache = m_fScaleTolerance.GetValue();
         pMotionChannelBiasObject->PropagateTolerances();
      }
   }
}


void MotionSet::AddMotionChannelBiasDrivers( TransformableObject *pOriginalTransformableObject, TransformableObject *pReducedTransformableObject )
{
   ASSERT( pOriginalTransformableObject );
   ASSERT( pReducedTransformableObject );

   if (!pOriginalTransformableObject->IsBone())
      return;
   if (!pReducedTransformableObject->IsBone())
      return;
   
   MotionChannelBiasObject *pMotionChannelBiasDriver = AddMotionChannelBiasObject( pOriginalTransformableObject, pReducedTransformableObject );
   pMotionChannelBiasDriver->SetVisibleInTree(FALSE);

   for (int nChildIndex=0; nChildIndex<pOriginalTransformableObject->GetNumChildren(); nChildIndex++)
   {
      TransformableObject *pOriginalChild = (TransformableObject*)pOriginalTransformableObject->GetChildAt(nChildIndex);
      TransformableObject *pReducedChild = (TransformableObject*)pReducedTransformableObject->GetChildAt(nChildIndex);

      AddMotionChannelBiasDrivers( pOriginalChild, pReducedChild );
   }
}

void MotionSet::Draw( CGLBaseView *pView )
{
   CGLMotionSetView *pMotionSetView = (CGLMotionSetView *)pView;

   if (pMotionSetView->m_nViewMode == CGLMotionSetView::VIEWMODE_COMPOSITED)
   {
      if (m_pReducedModelInstance)
         m_pReducedModelInstance->Draw( pView );
      if (m_pOriginalModelInstance)
         m_pOriginalModelInstance->Draw( pView );

      for (UINT nMotionChannelBoneBiasObjectIndex=0; nMotionChannelBoneBiasObjectIndex<GetNumMotionChannelBiasObjects(); nMotionChannelBoneBiasObjectIndex++)
      {
         MotionChannelBiasObject *pMotionChannelBiasObject = GetMotionChannelBiasObjectAt(nMotionChannelBoneBiasObjectIndex);
         pMotionChannelBiasObject->Draw(pView);
      }
   }
   else if (m_pCurModelInstance)
   {
      ((HierObject*)m_pCurModelInstance)->Draw( pView );

      for (UINT nMotionChannelBoneBiasObjectIndex=0; nMotionChannelBoneBiasObjectIndex<GetNumMotionChannelBiasObjects(); nMotionChannelBoneBiasObjectIndex++)
      {
         MotionChannelBiasObject *pMotionChannelBiasObject = GetMotionChannelBiasObjectAt(nMotionChannelBoneBiasObjectIndex);
         pMotionChannelBiasObject->Draw(pView);
      }
   }
}

void MotionSet::OnUpdateExportMotionSet(CCmdUI* pCmdUI)
{
   PluginList *pPluginList = GetMainInterface()->GetMotionSetTranslatorPluginManager()->GetPluginList();

   BOOL bEnable = FALSE;
   for (POSITION pos = pPluginList->GetHeadPosition(); pos; )
   {
      MotionSetTranslatorPlugin *pPlugin = (MotionSetTranslatorPlugin *)pPluginList->GetNext( pos );
      if (pPlugin->GetExportCallback()) {
         bEnable = TRUE;
         break;
      }
   }
   pCmdUI->Enable(bEnable);
}

void MotionSet::OnExportMotionSet()
{
  GetMainInterface()->GetMotionSetTranslatorPluginManager()->Export(this);
}

MotionPlaceholder *BasicProperty<MotionPlaceholder *>::m_prevvalue;

IMPLEMENT_SNIPEOBJECT_CREATE( MotionPlaceholder )

BaseStream &operator << ( BaseStream &bs, const MotionPlaceholder *&value )
{
   return operator << ( bs, (const SnipeObject * &) value);
}

BaseStream &operator >> ( BaseStream &bs, MotionPlaceholder *&value )
{
   return operator >> (bs, (SnipeObject *&) value );
}

/////////////////////////////////////////////////////
// MotionPlaceholderContainer

IMPLEMENT_SNIPEOBJECT_CREATE( MotionPlaceholderContainer )

MotionPlaceholderContainer::MotionPlaceholderContainer()
{
   m_strName = "Motions";
}

////////////////////////////////////////////////
// MotionPlaceholder

BoolProperty::Info MotionPlaceholder::m_bActiveInfo( "Active", "Active", TRUE );

MotionPlaceholder::MotionPlaceholder()
{
   m_pMotion = NULL;

   m_bActive.Init( &m_bActiveInfo, this );
}

void MotionPlaceholder::ApplyDrivers( SceneObject *pSceneObject )
{
   ApplyDriversBranch( this, pSceneObject );
}

void MotionPlaceholder::ApplyDriversBranch( SnipeObject *pParentObject, SceneObject *pSceneObject )
{
   int nChildCount = pParentObject->GetNumChildren();

   for (int i=0; i<nChildCount; i++)
   {
      SnipeObject *pChild = pParentObject->GetChildAt( i );
      if (pChild->IsKindOf( SOT_MotionChannelBiasObject ))
      {
         MotionChannelBiasObject *pMotionChannelBiasObject = (MotionChannelBiasObject *)pChild;
         String strMatchName = pMotionChannelBiasObject->GetMatchName();
         SnipeObject *pSnipeObject = pSceneObject->FindDescendant( strMatchName );
         if (pSnipeObject)
            pMotionChannelBiasObject->ApplyDrivers( this, pSnipeObject );
      }
      else
         ApplyDriversBranch( pChild, pSceneObject );
   }
}

void MotionPlaceholder::OnTreeItemCreated( CGLObjectTreeItem *pItem )
{
   CGLBoolPropertyCheckBox *pBtn = SNEW CGLBoolPropertyCheckBox( &m_bActive );
   pBtn->Init( pItem, NULL, CPoint(PADDING+16,0), CSize( 16, 17 ), -1, CGLControl::JUST_RIGHT );

   HierObject::OnTreeItemCreated( pItem );
}

////////////////////////////////////////////////
// MotionCurveDriverBiasObject

FloatProperty::Info g_fReductionBiasInfo( "Reduction Bias", "ReductionBias", 0.0f, -1.0f, 1.0f, 100.0f, FloatProperty::Info::FPT_PERCENT, PIF_SHOWVALUEWHENNOTSET );

template<class TYPE>
void MotionCurveDriverBiasObject<TYPE>::DrawTreeItem( CGLControl *pItem, float x=0.0f )
{
   TYPE::DrawTreeItem( pItem, x );

   glColor4f( 0.1f, 0.1f, 0.1f, m_fReductionBias.IsSet() ? 1.0f : 0.3f );

   String strText;
   m_fReductionBias.GetValueString( strText );
   CSize szText = pItem->GetTextExtent( strText );
   pItem->DrawText( pItem->m_szControl.cx - szText.cx - 8, 12, strText );

   if (m_fReductionBias.IsSet())
   {
      glColor3f( 0.1f, 0.1f, 0.1f );
      pItem->DrawText( x+/*CGLObjectTreeControl::m_glImageListTreeIcons.GetWidthEach()*/16+3, 12, GetTitle());
      pItem->DrawText( pItem->m_szControl.cx - szText.cx - 9, 12, strText );
   }
}
