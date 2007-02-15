//

#include "stdafx.h"
#include "resource.h"
#include "Motion.h"
#include "MotionSet.h"
#include "Properties/ModelPropertyDriversContainer.h"
#include "Properties/BoneDriversContainer.h"
#include "Properties/CustomPropertyDriversContainer.h"
#include "Properties/VertexMotionDriversContainer.h"
#include "Properties/MotionChannel.h"
#include "Properties/MotionCurveDriver.h"
#include "../GUI/MainFrm.h"
#include "Project.h"
#include "MotionTranslatorPluginManager.h"
#include "Interface/Implement.h"
#include "MotionCurve.h"
#include "../GUI/SnipeDocTemplate.h"
#include "../GUI/MotionSetDoc.h"

Motion *BasicProperty<Motion *>::m_prevvalue;
MotionInstance *BasicProperty<MotionInstance *>::m_prevvalue;

/////////////////////////////////////////////////////
// Motion

IMPLEMENT_SNIPEOBJECT_CREATE( Motion )
IMPLEMENT_COMMANDOBJECT( Motion )

BEGIN_MESSAGE_MAP(Motion, Scene)
   SNIPEOBJECT_ON_UPDATE_COMMAND_UI(ID_EXPORT_MOTION, OnUpdateExportMotion)
   SNIPEOBJECT_ON_COMMAND(ID_EXPORT_MOTION, OnExportMotion)
END_MESSAGE_MAP()

TimeProperty::Info     Motion::m_tLengthInfo( "Length", "Length", 0, 0, MAXTIME );
FloatProperty::Info    Motion::m_fReductionToleranceInfo( "ReductionTolerance", "ReductionTolerance", 0.1f, 0.0f, 1.0f, 100.0f, FloatProperty::Info::FPT_PERCENT);

Motion::Motion()
{
   m_pModelPropertyDriversContainer = NULL;
   m_pBoneDriversContainer = NULL;
   m_pCustomPropertyDriversContainer = NULL;
   m_pVertexMotionDriversContainer = NULL;

   m_tLength.Init( &m_tLengthInfo, this );
   m_fReductionTolerance.Init( &m_fReductionToleranceInfo,this);
}

Motion::~Motion()
{
}

void Motion::PreDestruct()
{
   m_commandlist.Empty();
   HierObject::PreDestruct();
}

UINT Motion::GetDefaultMenuItemID()
{
   return ID_OBJECT_EDIT;
}

CSnipeDocTemplate *Motion::GetDocTemplate()
{
   return GetApp().GetMotionDocTemplate();
}

ModelPropertyDriversContainer *Motion::CreateModelPropertyDriversContainer()
{
   if (!m_pModelPropertyDriversContainer)
   {
      m_pModelPropertyDriversContainer = ModelPropertyDriversContainer::Create();
      AddChildAtTail( m_pModelPropertyDriversContainer );
   }
   return m_pModelPropertyDriversContainer;
}

ModelPropertyDriversContainer *Motion::GetModelPropertyDriversContainer()
{
   return m_pModelPropertyDriversContainer;
}

BoneDriversContainer *Motion::CreateBoneDriversContainer()
{
   if (!m_pBoneDriversContainer)
   {
      m_pBoneDriversContainer = BoneDriversContainer::Create();
      AddChildAtTail( m_pBoneDriversContainer );
   }
   return m_pBoneDriversContainer;
}

BoneDriversContainer *Motion::GetBoneDriversContainer()
{
   return m_pBoneDriversContainer;
}

CustomPropertyDriversContainer *Motion::CreateCustomPropertyDriversContainer()
{
   if (!m_pCustomPropertyDriversContainer)
   {
      m_pCustomPropertyDriversContainer = CustomPropertyDriversContainer::Create();
      AddChildAtTail( m_pCustomPropertyDriversContainer );
   }
   return m_pCustomPropertyDriversContainer;
}

CustomPropertyDriversContainer *Motion::GetCustomPropertyDriversContainer()
{
   return m_pCustomPropertyDriversContainer;
}

VertexMotionDriversContainer *Motion::CreateVertexMotionDriversContainer()
{
   if (!m_pVertexMotionDriversContainer)
   {
      m_pVertexMotionDriversContainer = VertexMotionDriversContainer::Create();
      AddChildAtTail( m_pVertexMotionDriversContainer );
   }
   return m_pVertexMotionDriversContainer;
}

VertexMotionDriversContainer *Motion::GetVertexMotionDriversContainer()
{
   return m_pVertexMotionDriversContainer;
}

MotionChannel *Motion::AddBone( const char *name /*=NULL*/ )
{
   MotionChannel *pMotionChannel = MotionChannel::Create();
   CreateBoneDriversContainer()->AddChildAtTail( pMotionChannel );
   if (name)
      pMotionChannel->SetName(name);
   else
      pMotionChannel->SetName("Bone");

   return pMotionChannel;
}

UINT Motion::GetNumMotionChannels()
{
   if (m_pBoneDriversContainer)
      return ((HierObject*)m_pBoneDriversContainer)->GetNumChildren();
   return 0;
}

MotionChannel *Motion::GetMotionChannel(const char *name)
{
   for (UINT nMotionChannelIndex = 0; nMotionChannelIndex < GetNumMotionChannels(); nMotionChannelIndex++)
   {
      MotionChannel *pMotionChannel = GetMotionChannelAt(nMotionChannelIndex);
      if (stricmp(pMotionChannel->GetMatchName().Get(),name)==0)
         return pMotionChannel;
   }
   return NULL;
}

InstanceObject *Motion::CreateInstance()
{
   MotionInstance *pMotionInstance = SNEW MotionInstance;

   pMotionInstance->m_pMotion.StoreValue( this, 0 );

   return (InstanceObject*)pMotionInstance;
}

void Motion::CalcLengthBranch( SnipeObject *pStartObject, Time &tLength )
{
   for (int i=0; i<pStartObject->GetNumChildren(); i++)
   {
      SnipeObject *pObject = pStartObject->GetChildAt( i );

      if (pObject->IsKindOf( SOT_MotionCurveDriver ))
      {
         MotionCurveDriver *pMotionCurveDriver = (MotionCurveDriver *)pObject;
         Time tCurveLength = pMotionCurveDriver->GetLength();
         if (tCurveLength > tLength)
            tLength = tCurveLength;
      }

      CalcLengthBranch( pObject, tLength );
   }
}

Time Motion::CalcLength()
{
   Time tLength;
   CalcLengthBranch( this, tLength );
   m_tLength.StoreValue( tLength );
   return tLength;
}

BOOL Motion::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         if (pProperty == &m_tPlaybackStart && m_tPlaybackStart.GetCurrentValue() > m_tPlaybackEnd.GetCurrentValue())
            m_tPlaybackEnd.StoreValue( m_tPlaybackStart, 0, FALSE );
         else if (pProperty == &m_tPlaybackEnd && m_tPlaybackEnd.GetCurrentValue() < m_tPlaybackStart.GetCurrentValue())
            m_tPlaybackStart.StoreValue( m_tPlaybackEnd, 0, FALSE );
         else if (pProperty == &m_fReductionTolerance)
         {
            DirtyModifiedMotionCurves();
            GetMainFrame()->m_glMainRenderWnd.m_glTimeLineBar.m_glTimeLineFrame.m_glTimeLineView.Invalidate();
         }
         break;
      }
      case NOTIFYOBJ_ADDEDDESCENDANT:
      case NOTIFYOBJ_REMOVEDDESCENDANT:
      {
         HierObject *pObj = (HierObject *)lParam;
         CalcLength();
         break;
      }
      case NOTIFYOBJ_ADDEDCHILD:
      {
         SnipeObject *pChild = (SnipeObject *)lParam;
         if (pChild->GetSOT() == SOT_ModelPropertyDriversContainer)
            m_pModelPropertyDriversContainer = (ModelPropertyDriversContainer *)pChild;
         else if (pChild->GetSOT() == SOT_BoneDriversContainer)
            m_pBoneDriversContainer = (BoneDriversContainer *)pChild;
         else if (pChild->GetSOT() == SOT_CustomPropertyDriversContainer)
            m_pCustomPropertyDriversContainer = (CustomPropertyDriversContainer *)pChild;
         else if (pChild->GetSOT() == SOT_VertexMotionDriversContainer)
            m_pVertexMotionDriversContainer = (VertexMotionDriversContainer *)pChild;
         break;
      }
      case NOTIFYOBJ_REMOVEDCHILD:
      {
         SnipeObject *pChild = (SnipeObject *)lParam;
         if (pChild == GetModelPropertyDriversContainer())
            m_pModelPropertyDriversContainer = NULL;
         else if (pChild == GetBoneDriversContainer())
            m_pBoneDriversContainer = NULL;
         else if (pChild == GetCustomPropertyDriversContainer())
            m_pCustomPropertyDriversContainer = NULL;
         else if (pChild == GetVertexMotionDriversContainer())
            m_pVertexMotionDriversContainer = NULL;
         break;
      }
   }

   return HierObject::OnObjectNotify( msg, wParam, lParam );
}

void Motion::DirtyModifiedMotionCurves()
{
   for (UINT nMotionChannelIndex = 0; nMotionChannelIndex < GetNumMotionChannels(); nMotionChannelIndex++)
   {
      MotionChannel *pMotionChannel = GetMotionChannelAt(nMotionChannelIndex);
      for (UINT nMotionCurveDriverIndex = 0; nMotionCurveDriverIndex < pMotionChannel->GetNumMotionCurveDrivers(); nMotionCurveDriverIndex++)
      {
         MotionCurveDriver *pMotionCurveDriver = pMotionChannel->GetMotionCurveDriverAt(nMotionCurveDriverIndex);
         for (UINT nMotionCurveIndex =0; nMotionCurveIndex < pMotionCurveDriver->GetNumMotionCurves(); nMotionCurveIndex++)
         {
            MotionCurve *pMotionCurve = pMotionCurveDriver->GetMotionCurveAt(nMotionCurveIndex);
            pMotionCurve->SetModifiedCurveDirty(TRUE);
         }
      }
   }
}

void Motion::OnUpdateExportMotion(CCmdUI* pCmdUI) 
{
   PluginList *pPluginList = GetMainInterface()->GetMotionTranslatorPluginManager()->GetPluginList();

   BOOL bEnable = FALSE;
   for (POSITION pos = pPluginList->GetHeadPosition(); pos; )
   {
      MotionTranslatorPlugin *pPlugin = (MotionTranslatorPlugin *)pPluginList->GetNext( pos );
      if (pPlugin->GetExportCallback()) {
         bEnable = TRUE;
         break;
      }
   }
   pCmdUI->Enable(bEnable);
}

void Motion::OnExportMotion() 
{
   GetMainInterface()->GetMotionTranslatorPluginManager()->Export(this);
}

Time Motion::GetStartTime()
{
   Time startTime;
   for (UINT nMotionChannelIndex = 0; nMotionChannelIndex < GetNumMotionChannels(); nMotionChannelIndex++)
   {
      MotionChannel *pMotionChannel = GetMotionChannelAt(nMotionChannelIndex);
      Time time = pMotionChannel->GetStartTime();
      if (time < startTime)
         startTime = time;
   }
   return startTime;
}

Time Motion::GetStopTime()
{
   Time stopTime;
   for (UINT nMotionChannelIndex = 0; nMotionChannelIndex < GetNumMotionChannels(); nMotionChannelIndex++)
   {
      MotionChannel *pMotionChannel = GetMotionChannelAt(nMotionChannelIndex);
      Time time = pMotionChannel->GetStopTime();
      if (time > stopTime)
         stopTime = time;
   }
   return stopTime;
}

void Motion::OnParent( HierObject *pHierObject )
{
   HierObject::OnParent(pHierObject);

   switch (pHierObject->GetSOT())
   {
      case SOT_MotionSet:
      {
         MotionSet *pMotionSet = (MotionSet*)pHierObject;
         pMotionSet->AddMotion( this );
         // Doesn't all this code below ultimately do the above line?  It looks like it would add it to all motionsetdocs.  Not just the motion set you dropped it on.

         //CSnipeDocTemplate *pTemplate = pHierObject->GetDocTemplate();
         //if (pTemplate)
         //{
         //   for (POSITION pos = pTemplate->GetFirstDocPosition(); pos; )
         //   {
         //      CBaseDoc *pDocument = (CBaseDoc*)pTemplate->GetNextDoc(pos);
         //      SnipeDocType nType = pDocument->GetDocType();
         //      if (nType == SDT_CMotionSetDoc)
         //      {
         //         CMotionSetDoc *pMotionSetDoc = (CMotionSetDoc*)pDocument;
         //         pMotionSetDoc->AddMotion(this);
         //      }
         //   }
         //}
         return;
      }
   }
}

/////////////////////////////////////////////////////
// MotionContainer

IMPLEMENT_SNIPEOBJECT_CREATE( MotionContainer )

MotionContainer::MotionContainer()
{
   m_strName = "Motions";
   GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glExplorerBar.m_glMotionTree.SetObjectHead( this );
}

MotionContainer::~MotionContainer()
{
   GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glExplorerBar.m_glMotionTree.SetObjectHead( NULL );
}


/////////////////////////////////////////////////////
// MotionInstance

IMPLEMENT_SNIPEOBJECT_CREATE( MotionInstance )

MotionPointerProperty::Info MotionInstance::m_pMotionInfo( "Motion", "Motion" );
   TimeProperty::Info          MotionInstance::m_tCropStartInfo( "Crop Start", "CropStart", 0, 0, MAXTIME );
   TimeProperty::Info          MotionInstance::m_tCropEndInfo( "Crop End", "CropEnd", 0, 0, MAXTIME );
TimeProperty::Info          MotionInstance::m_tStartInfo( "Start", "Start", 0, -MAXTIME, MAXTIME );
TimeProperty::Info          MotionInstance::m_tDurationInfo( "Duration", "Duration", 0, 0, MAXTIME );

MotionInstance::MotionInstance()
{
   m_pMotion.Init( &m_pMotionInfo, this );
      m_tCropStart.Init( &m_tCropStartInfo, &m_pMotion );
      m_tCropEnd.Init( &m_tCropEndInfo, &m_pMotion );
   m_tStart.Init( &m_tStartInfo, this );
   m_tDuration.Init( &m_tDurationInfo, this );
}

MotionInstance::~MotionInstance()
{
}

void MotionInstance::ApplyDrivers()
{
   if (!m_pMotion || !GetParent())
      return;

   ApplyDriversBranch( m_pMotion );
}

void MotionInstance::ApplyDriversBranch( SnipeObject *pParentObject )
{
   SceneObject *pSceneObject = GetSceneObject();
   ASSERT(pSceneObject);
   int nChildCount = pParentObject->GetNumChildren();

   for (int i=0; i<nChildCount; i++)
   {
      SnipeObject *pChild = pParentObject->GetChildAt( i );
      if (pChild->IsKindOf( SOT_MotionChannel ))
      {
         MotionChannel *pMotionChannel = (MotionChannel *)pChild;
         String strMatchName = pMotionChannel->GetMatchName();
         SnipeObject *pSnipeObject = pSceneObject->FindDescendant( strMatchName );
         if (pSnipeObject)
            pMotionChannel->ApplyDrivers( this, pSnipeObject );
      }
      else
         ApplyDriversBranch( pChild );
   }
}

BOOL MotionInstance::Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError )
{
   // if we are the last Motion, we won't detach ourself, we will detach the MotionContainer
   DeleteInfo *&pMotionInstanceDeleteInfo = (DeleteInfo *&)pDeleteInfo;
   if (pMotionInstanceDeleteInfo == NULL)
      pMotionInstanceDeleteInfo = SNEW DeleteInfo( this );

   if (GetParent() && GetParent()->GetNumChildren()==1)
   {
      pMotionInstanceDeleteInfo->SetOldMotionInstanceContainer((MotionInstanceContainer *)GetParent());
      pMotionInstanceDeleteInfo->GetOldMotionInstanceContainer()->Delete( (SnipeObject::DeleteInfo *)pMotionInstanceDeleteInfo->m_pMotionInstanceContainerDeleteInfo, strOutError );
      return SnipeObject::Delete( pDeleteInfo, strOutError );
   }
   else
      return HierObject::Delete( pDeleteInfo, strOutError );
}

BOOL MotionInstance::UnDelete( SnipeObject::DeleteInfo *pDeleteInfo )
{
   DeleteInfo *pMotionInstanceDeleteInfo = (DeleteInfo *)pDeleteInfo;
   if (pMotionInstanceDeleteInfo->GetOldMotionInstanceContainer())
   {
      pMotionInstanceDeleteInfo->GetOldMotionInstanceContainer()->UnDelete( (SnipeObject::DeleteInfo *)pMotionInstanceDeleteInfo->m_pMotionInstanceContainerDeleteInfo );
      pMotionInstanceDeleteInfo->GetOldMotionInstanceContainer()->FreeDeleteInfo( pMotionInstanceDeleteInfo->m_pMotionInstanceContainerDeleteInfo );
      pMotionInstanceDeleteInfo->m_pMotionInstanceContainerDeleteInfo = NULL;
      return TRUE;
   }
   else
      return HierObject::UnDelete( pDeleteInfo );
}

BOOL MotionInstance::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_VALUECHANGED:
      {
         int nStoreUndo = (int)wParam;
         Property *pProperty = (Property *)lParam;
         if (pProperty == &m_pMotion)
         {
            Motion *pMotion = m_pMotion;
            if (!m_bUserNamed && pMotion)
               SetName( pMotion->GetTitle() );
            ApplyDrivers();
         }
         break;
      }
   }

   return HierObject::OnObjectNotify( msg, wParam, lParam );
}

BOOL MotionInstance::GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList )
{
   if (pPointerProperty != &m_pMotion)
      return HierObject::GetPossiblePointerPropertyChoices( pPointerProperty, pSnipeObjectList );

   Project *pProject = GetMainInterface()->GetProject();
   if (!pProject)
   {
      ASSERT( FALSE );
      return TRUE;
   }

   MotionContainer *pMotionContainer = &pProject->m_motioncontainer;
   for (UINT i=0; i<pMotionContainer->GetNumMotions(); i++)
   {
      Motion *pMotion = pMotionContainer->GetMotionAt( i );
      pSnipeObjectList->AddTail( pMotion );
   }

   return TRUE;
}

/////////////////////////////////////////////////////
// MotionInstanceContainer

IMPLEMENT_SNIPEOBJECT_CREATE( MotionInstanceContainer )

MotionInstanceContainer::MotionInstanceContainer()
{
   m_strName = "Motions";
}

/////////////////////////

BaseStream &operator << ( BaseStream &bs, const Motion *&value )
{
   return operator << ( bs, (const SnipeObject * &) value);
}

BaseStream &operator >> ( BaseStream &bs, Motion *&value )
{
   return operator >> (bs, (SnipeObject *&) value );
}

BaseStream &operator << ( BaseStream &bs, const MotionInstance *&value )
{
   return operator << ( bs, (const SnipeObject * &) value);
}

BaseStream &operator >> ( BaseStream &bs, MotionInstance *&value )
{
   return operator >> (bs, (SnipeObject *&) value );
}
