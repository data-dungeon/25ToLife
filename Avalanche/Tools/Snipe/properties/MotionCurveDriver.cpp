#include "stdafx.h"
#include "Motion.h"
#include "MotionCurveDriver.h"
#include "MotionCurve.h"
#include "../GUI/MainFrm.h"
#include "resource.h"

IMPLEMENT_SNIPEOBJECT_PURE( MotionCurveDriver )

//FloatProperty::Info MotionCurveDriver::m_fReductionBiasInfo( "ReductionBias", "ReductionBias", 0.0f, -1.0f, 1.0f, 100.0f, FloatProperty::Info::FPT_PERCENT);

MotionCurveDriver::MotionCurveDriver()
{
   m_strName = "Motion Curve";
   m_motionkeylist.SetOwner( this );
   m_prevapplytime.SetTagged();
}

MotionCurveDriver::~MotionCurveDriver()
{
   if (GetMainFrame()->m_glMainRenderWnd.m_glTimeLineBar.m_glTimeLineFrame.m_glTimeLineView.m_timelinedoc.m_pObjectHead == this)
      GetMainFrame()->m_glMainRenderWnd.m_glTimeLineBar.m_glTimeLineFrame.m_glTimeLineView.m_timelinedoc.SetObjectHead( NULL );
}

void MotionCurveDriver::PostConstruct()
{
   PropertyDriver::PostConstruct();
//   m_fReductionBias.Init( &m_fReductionBiasInfo,this);
}

void MotionCurveDriver::AddKey( MotionKey *pKey )
{
   m_motionkeylist.Add( pKey );
}

void MotionCurveDriver::RemoveKey( MotionKey *pKey )
{
   m_motionkeylist.Remove(pKey);
}

void MotionCurveDriver::RemoveAll()
{
   for (UINT i=0; i<GetNumMotionCurves(); i++)
   {
      MotionCurve *pMotionCurve = GetMotionCurveAt( i );
      pMotionCurve->RemoveAll();
   }
   m_prevapplytime.SetTagged();
}

Time MotionCurveDriver::GetLength()
{
   Time tMaxLength( Time::TT_NOTSET );

   for (UINT i=0; i<GetNumMotionCurves(); i++)
   {
      MotionCurve *pMotionCurve = GetMotionCurveAt( i );
      Time tLength = pMotionCurve->GetLength();
      if (tLength > tMaxLength)
         tMaxLength = tLength;
   }

   return tMaxLength;
}

void MotionCurveDriver::GetPossiblePointerPropertyChoicesBranch( PropertyArray *pPropertyArray, SnipeObjectList *pSnipeObjectList, SnipeObjectType nSOT )
{
   for (int i=0; i<pPropertyArray->GetCount(); i++)
   {
      Property *pProperty = pPropertyArray->GetAt( i );
      if (pProperty->IsKindOf( nSOT ))
         pSnipeObjectList->AddTail( pPropertyArray->GetAt(i) );

      if (pProperty->GetPropertyArray())
         GetPossiblePointerPropertyChoicesBranch( pProperty->GetPropertyArray(), pSnipeObjectList, nSOT );
   }
}

BOOL MotionCurveDriver::GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList )
{
   if (pPointerProperty == &m_pProperty)
   {
      GetPossiblePointerPropertyChoicesBranch( GetParent()->GetPropertyArray(), pSnipeObjectList, GetPropertyType() );
      return TRUE;
   }

   return PropertyDriver::GetPossiblePointerPropertyChoices( pPointerProperty, pSnipeObjectList );
}

void MotionCurveDriver::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   SnipeObjectHeader header( this, bWriteHeader, bs );

   MotionKey *pKey[4] = { NULL, NULL, NULL, NULL }; // max motion curves is 4
   UINT nNumMotionCurves = GetNumMotionCurves();
   ASSERT( nNumMotionCurves <= 4 );

   BOOL bHasKeys = FALSE;
   for (UINT i=0; i<nNumMotionCurves; i++)
   {
      MotionCurve *pMotionCurve = GetMotionCurveAt( i );
      pKey[i] = pMotionCurve->GetHeadKey();
      if (pKey[i])
         bHasKeys = TRUE;
   }

   if (bHasKeys)
   {
      DataHeader dataheader( bs, "Keys" );

      UINT nKeyFlags;
      while (TRUE)
      {
         Time tNextKey( Time::MAXTIME );
         nKeyFlags = 0;

         for (UINT i=0; i<nNumMotionCurves; i++)
         {
            if (pKey[i])
            {
               Time tKeyTime = pKey[i]->GetTime();

               if (tKeyTime < tNextKey)
               {
                  nKeyFlags = 0;
                  tNextKey = pKey[i]->GetTime();
               }

               if (tKeyTime == tNextKey)
                  nKeyFlags |= 1<<i;
            }
         }

         if (nKeyFlags == 0)
            break;

         {
            bs << mar << tNextKey << sp << nKeyFlags;
               for (UINT i=0; i<nNumMotionCurves; i++)
               {
                  if (nKeyFlags & (1<<i))
                  {
                     pKey[i]->Save( bs, TRUE );
                     pKey[i] = pKey[i]->GetNextKey();
                  }
               }
            bs << nl;
         }
      }
   }

   PropertyDriver::Save( bs );
}

BOOL MotionCurveDriver::ParseData( BaseStream &bs, const DataHeader &dataheader )
{
   if (dataheader.m_strKey == "Keys")
   {
      MemoryStream ms( 1024 );
      UINT nNumMotionCurves = GetNumMotionCurves();
      Time time;
      UINT nKeyFlags;

      while (TRUE)
      {
         bs.ReadMargin();
         bs.ReadLine( ms );

         if (DataHeader::IsHeader( ms ))
            return TRUE;

         ms >> time;
         ms >> nKeyFlags;

         for (UINT i=0; i<nNumMotionCurves; i++)
         {
            if (nKeyFlags & (1<<i))
            {
               MotionCurve *pMotionCurve = GetMotionCurveAt( i );
               MotionKey *pKey = pMotionCurve->AllocateMotionKey();
               pKey->SetTime( time );
               pKey->Load( ms );
               pMotionCurve->AddKey( pKey );
            }
         }
      }
   }
   else
      return PropertyDriver::ParseData( bs, dataheader );
}

void MotionCurveDriver::Draw( CGLBaseView *pView )
{
   if (pView->GetSVT() != SVT_CGLTimeLineView)
      return;

   //if (MotionCurve::m_glImageListKey.IsEmpty())
   //   MotionCurve::m_glImageListKey.Create( IDB_KEY );

   //MotionCurve::m_glImageListKey.Bind();
   for (UINT i=0; i<GetNumMotionCurves(); i++)
   {
      MotionCurve *pMotionCurve = GetMotionCurveAt( i );
      pMotionCurve->Draw( pView );
   }
   //MotionCurve::m_glImageListKey.UnBind();
}

void MotionCurveDriver::PushBounds( BoundingBox &box, CGLBaseView *pView/*=NULL*/ )
{
   for (UINT i=0; i<GetNumMotionCurves(); i++)
   {
      MotionCurve *pMotionCurve = GetMotionCurveAt( i );
      pMotionCurve->PushBounds( box, pView );
   }
}

void MotionCurveDriver::OnSelectedChanged()
{
   PropertyDriver::OnSelectedChanged();

   if (IsSelected())
      GetMainFrame()->m_glMainRenderWnd.m_glTimeLineBar.m_glTimeLineFrame.m_glTimeLineView.m_timelinedoc.SetObjectHead( this );
}

void MotionCurveDriver::CountPickObjectType( CGLBaseView *pBaseView, UINT &nCount )
{
   PropertyDriver::CountPickObjectType( pBaseView, nCount );

   if (pBaseView->m_nPickObjectType == SOT_MotionKey)
      nCount += m_motionkeylist.GetCount();
}

SimpleComponentList *MotionCurveDriver::FindComponentList( const String &strComponentType )
{
   if (strComponentType == "key")
      return (SimpleComponentList *)&m_motionkeylist;

   return PropertyDriver::FindComponentList(strComponentType);
}

float MotionCurveDriver::GetMaxValue()
{
   float fMaxValue = GetMotionCurveAt(0)->GetMaxValue();
   for (UINT nMotionCurveIndex = 1; nMotionCurveIndex < GetNumMotionCurves(); nMotionCurveIndex++)
   {
      if (GetMotionCurveAt(nMotionCurveIndex)->GetMaxValue()>fMaxValue)
      {
         fMaxValue = GetMotionCurveAt(nMotionCurveIndex)->GetMaxValue();
      }
   }
   return fMaxValue;
}

float MotionCurveDriver::GetMinValue()
{
   float fMinValue = GetMotionCurveAt(0)->GetMinValue();
   for (UINT nMotionCurveIndex = 1; nMotionCurveIndex < GetNumMotionCurves(); nMotionCurveIndex++)
   {
      if (GetMotionCurveAt(nMotionCurveIndex)->GetMinValue()<fMinValue)
      {
         fMinValue = GetMotionCurveAt(nMotionCurveIndex)->GetMinValue();
      }
   }
   return fMinValue;
}

Time MotionCurveDriver::GetStartTime()
{
   Time startTime = GetMotionCurveAt(0)->GetStartTime();
   for (UINT nMotionCurveIndex = 1; nMotionCurveIndex < GetNumMotionCurves(); nMotionCurveIndex++)
   {
      if (GetMotionCurveAt(nMotionCurveIndex)->GetStartTime()<startTime)
      {
         startTime = GetMotionCurveAt(nMotionCurveIndex)->GetStartTime();
      }
   }
   return startTime;
}

Time MotionCurveDriver::GetStopTime()
{
   Time stopTime = GetMotionCurveAt(0)->GetStopTime();
   for (UINT nMotionCurveIndex = 1; nMotionCurveIndex < GetNumMotionCurves(); nMotionCurveIndex++)
   {
      if (GetMotionCurveAt(nMotionCurveIndex)->GetStopTime()>stopTime)
      {
         stopTime = GetMotionCurveAt(nMotionCurveIndex)->GetStopTime();
      }
   }
   return stopTime;
}

void MotionCurveDriver::AddModifiedMotionCurves()
{
   for (UINT nMotionCurveIndex = 0; nMotionCurveIndex < GetNumMotionCurves(); nMotionCurveIndex++)
   {
      MotionCurve *pMotionCurve = GetMotionCurveAt(nMotionCurveIndex);
      pMotionCurve->AddModifiedMotionCurve();
   }
}

void MotionCurveDriver::RemoveModifiedMotionCurves()
{
   for (UINT nMotionCurveIndex = 0; nMotionCurveIndex < GetNumMotionCurves(); nMotionCurveIndex++)
   {
      MotionCurve *pMotionCurve = GetMotionCurveAt(nMotionCurveIndex);
      pMotionCurve->RemoveModifiedMotionCurve();
   }
}

MotionCurve *MotionCurveDriver::FindMotionCurve( MotionKey *pKey )
{
   int numCurves = GetNumMotionCurves();
   for (int curveIndex = 0; curveIndex < numCurves; curveIndex++)
   {
      MotionCurve *pMotionCurve = GetMotionCurveAt(curveIndex);
      if (pMotionCurve->FindKeyIndex(pKey)!=-1)
      {
         return pMotionCurve;
      }
   }
   return NULL;
}


BOOL MotionCurveDriver::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   //switch (msg)
   //{
   //   case NOTIFYOBJ_VALUECHANGED:
   //   {
   //      Property *pProperty = (Property *)lParam;
   //      if (pProperty == &m_fReductionBias)
   //      {
   //         for (UINT nMotionCurveIndex = 0; nMotionCurveIndex < GetNumMotionCurves(); nMotionCurveIndex++)
   //         {
   //            MotionCurve *pMotionCurve = GetMotionCurveAt(nMotionCurveIndex);
   //            pMotionCurve->SetModifiedCurveDirty(TRUE);
   //         }
   //         GetMainFrame()->m_glMainRenderWnd.m_glTimeLineBar.m_glTimeLineFrame.m_glTimeLineView.Invalidate();
   //      }
   //      break;
   //   }
   //}

   return PropertyDriver::OnObjectNotify( msg, wParam, lParam );
}

MotionCurveDriver::RebuildMotionCurveDriverInfo *MotionCurveDriver::RebuildCurves( float fTolerance, MotionCurveDriver::RebuildMotionCurveDriverInfo::RebuildMotionCurveType nType )
{
   MainInterface *pMI = GetMainInterface();
   RebuildMotionCurveDriverInfo *pRebuildMoionCurveDriverInfo = SNEW RebuildMotionCurveDriverInfo;

   pRebuildMoionCurveDriverInfo->m_pRebuiltMotionCurveDriver = (MotionCurveDriver*)pMI->CreateObject(GetSOT());
   pRebuildMoionCurveDriverInfo->m_pMotionCurveDriver = this;

   for (UINT nMotionCurveIndex = 0; nMotionCurveIndex < GetNumMotionCurves(); nMotionCurveIndex++)
   {
      MotionCurve *pMotionCurve = GetMotionCurveAt(nMotionCurveIndex);
      MotionCurve *pDstMotionCurve = pRebuildMoionCurveDriverInfo->m_pRebuiltMotionCurveDriver->GetMotionCurveAt(nMotionCurveIndex);
      if (nType == RebuildMotionCurveDriverInfo::LINEAR)
         ((FloatMotionCurve*)pMotionCurve)->RebuildAsLinear(fTolerance,(FloatMotionCurve*)pDstMotionCurve);
      else
         ((FloatMotionCurve*)pMotionCurve)->RebuildAsCubic(fTolerance,(FloatMotionCurve*)pDstMotionCurve);
   }
   // insert new motion driver into tree
   int nIndex = ((HierObject*)GetParent())->FindChildIndex(this);
   ASSERT(nIndex>=0);
   ((HierObject*)GetParent())->InsertChildAt(nIndex,pRebuildMoionCurveDriverInfo->m_pRebuiltMotionCurveDriver);
 
   // set up redo and undo
   pRebuildMoionCurveDriverInfo->m_pMotionCurveDriver->Reference();
   pRebuildMoionCurveDriverInfo->m_pRebuiltMotionCurveDriver->Reference();
   pRebuildMoionCurveDriverInfo->m_name = pRebuildMoionCurveDriverInfo->m_pMotionCurveDriver->GetMatchName();
   String strerror;
   if (!pRebuildMoionCurveDriverInfo->m_pMotionCurveDriver->Delete((SnipeObject::DeleteInfo *&)pRebuildMoionCurveDriverInfo->m_pDeleteInfo,strerror))
      return FALSE;
   if (!pRebuildMoionCurveDriverInfo->m_pRebuiltMotionCurveDriver->Delete((SnipeObject::DeleteInfo *&)pRebuildMoionCurveDriverInfo->m_pRebuildDeleteInfo,strerror))
      return FALSE;
   pRebuildMoionCurveDriverInfo->m_pRebuildDeleteInfo->UnDelete();
   pRebuildMoionCurveDriverInfo->m_pRebuiltMotionCurveDriver->SetName(pRebuildMoionCurveDriverInfo->m_name);

   return pRebuildMoionCurveDriverInfo;
}

MotionCurveDriver::RebuildMotionCurveDriverInfo::RebuildMotionCurveDriverInfo()
{
   m_pMotionCurveDriver = NULL;
   m_pRebuiltMotionCurveDriver = NULL;
   m_pDeleteInfo = NULL;
   m_pRebuildDeleteInfo = NULL;
}

MotionCurveDriver::RebuildMotionCurveDriverInfo::~RebuildMotionCurveDriverInfo()
{
   m_pMotionCurveDriver->DeReference();
   m_pRebuiltMotionCurveDriver->DeReference();
   if (m_pDeleteInfo)
      delete m_pDeleteInfo;
   if (m_pRebuildDeleteInfo)
      delete m_pRebuildDeleteInfo;
}

void MotionCurveDriver::RebuildMotionCurveDriverInfo::Redo()
{
   m_pRebuildDeleteInfo->UnDelete();
   m_pDeleteInfo->Delete();
   m_pRebuiltMotionCurveDriver->SetName(m_name); 
}

void MotionCurveDriver::RebuildMotionCurveDriverInfo::Undo()
{
   m_pDeleteInfo->UnDelete();
   m_pRebuildDeleteInfo->Delete();
   m_pMotionCurveDriver->SetName(m_name);
}

/////////////////////////////////////////////
// MotionCurveInstanceDriver

void MotionCurveInstanceDriver::Apply( Time time )
{
   if (m_pMotionInstance)
      time -= m_pMotionInstance->m_tStart;

   if (m_pMotionCurveDriver && m_pProperty)
      m_pMotionCurveDriver->Apply( m_pProperty, time );
}


