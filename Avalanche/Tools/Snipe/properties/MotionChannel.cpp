#include "stdafx.h"
#include "MotionChannel.h"
#include "MotionCurveDriver.h"
#include "GUI/ObjectExplorerBar.h"

IMPLEMENT_SNIPEOBJECT_CREATE( MotionChannel )

inline UINT MotionChannel::GetNumMotionCurveDrivers()
{
   return GetNumChildren();
}

inline MotionCurveDriver *MotionChannel::GetMotionCurveDriverAt( UINT nIndex )
{
   return (MotionCurveDriver *)GetChildAt( nIndex );
}

int MotionChannel::AddMotionCurveDriver( MotionCurveDriver *pMotionCurveDriver )
{
   ASSERT( pMotionCurveDriver );
   return AddChildAtTail( (HierObject*)pMotionCurveDriver );
}

MotionCurveDriver *MotionChannel::GetMotionCurveDriver( Property *pProperty )
{
   for (UINT nSurfaceDriverIndex=0; nSurfaceDriverIndex<GetNumMotionCurveDrivers(); nSurfaceDriverIndex++)
   {
      MotionCurveDriver *pMotionCurveDriver = (MotionCurveDriver *)GetMotionCurveDriverAt(nSurfaceDriverIndex);
      if (pMotionCurveDriver->GetProperty() == pProperty)
         return pMotionCurveDriver;
   }

   return NULL;
}

MotionCurveDriver *MotionChannel::GetMotionCurveDriver( const char *name )
{
   for (UINT nMotionCurveIndex = 0; nMotionCurveIndex < GetNumMotionCurveDrivers(); nMotionCurveIndex++)
   {
      MotionCurveDriver *pMotionCurveDriver = GetMotionCurveDriverAt(nMotionCurveIndex);
      if (stricmp(pMotionCurveDriver->GetMatchName().Get(),name)==0)
         return pMotionCurveDriver;
   }
   return NULL;
}

void MotionChannel::ApplyDrivers( MotionInstance *pMotionInstance, SnipeObject *pObject )
{
   static String strError;
   SnipeObject *pOldWorkingObject = GetMainInterface()->SetCurrentWorkingObject( pObject );
   int nDriverCount = GetNumMotionCurveDrivers();

   for (int i=0; i<nDriverCount; i++)
   {
      MotionCurveDriver *pMotionCurveDriver = GetMotionCurveDriverAt( i );
      String strMatchName = pMotionCurveDriver->GetMatchName();
      Property *pProperty = (Property *)GetMainInterface()->DescriptionToObject( strMatchName, &strError );
      if (pProperty)
      {
         ASSERT( pProperty->IsKindOf( SOT_Property ));
         MotionCurveInstanceDriver *pMotionCurveInstanceDriver = SNEW MotionCurveInstanceDriver( pMotionInstance, pMotionCurveDriver );
         pMotionCurveInstanceDriver->m_pProperty.StoreValue( pProperty );
         pProperty->OnValueChanged();
      }
   }

   GetMainInterface()->SetCurrentWorkingObject( pOldWorkingObject );
}

void MotionChannel::ComputeParentToBone( Vector bonedirection, Vector rolldirection, Vector parentbonedirection, Vector parentrolldirection )
{
   if (parentbonedirection != Vector( 0 ))
   {
      Matrix44 matA = VectorsToStandard( parentbonedirection, parentrolldirection );
      Matrix44 matB = VectorsToStandard( bonedirection, rolldirection );
      matB = matB.Transpose();
      m_parentToBoneMatrix = matA * matB;
   }
   else
      m_parentToBoneMatrix.Identity();
}

void MotionChannel::DrawTreeItem( CGLControl *pItem, float x/*=0.0f*/ )
{
   CGLObjectTreeControl::m_glImageListTreeIcons.Draw( x, 0.0f, ICONINDEX_BONE );

   HierObject::DrawTreeItem( pItem, x );
}

Time MotionChannel::GetStartTime()
{
   Time startTime;
   for (UINT nMotionCurveDriverIndex = 0; nMotionCurveDriverIndex < GetNumMotionCurveDrivers(); nMotionCurveDriverIndex++)
   {
      MotionCurveDriver *pMotionCurveDriver = GetMotionCurveDriverAt(nMotionCurveDriverIndex);
      Time time = pMotionCurveDriver->GetStartTime();
      if (time < startTime)
         startTime = time;
   }
   return startTime;
}

Time MotionChannel::GetStopTime()
{
   Time stopTime;
   for (UINT nMotionCurveDriverIndex = 0; nMotionCurveDriverIndex < GetNumMotionCurveDrivers(); nMotionCurveDriverIndex++)
   {
      MotionCurveDriver *pMotionCurveDriver = GetMotionCurveDriverAt(nMotionCurveDriverIndex);
      Time time = pMotionCurveDriver->GetStopTime();
      if (time > stopTime)
         stopTime = time;
   }
   return stopTime;
}