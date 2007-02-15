#pragma once

#include "HierObject.h"
#include "..\Utility\Matrix44.h"

class MotionCurveDriver;
class MotionInstance;

class MotionChannel : public HierObject
{
private:
   InstanceObject *m_pCache;

   Matrix44 m_parentToBoneMatrix;
   
public:
   DECLARE_SNIPEOBJECT_CREATE( MotionChannel, HierObject )

   UINT GetNumMotionCurveDrivers();
   MotionCurveDriver *GetMotionCurveDriverAt( UINT nIndex );
   MotionCurveDriver *GetMotionCurveDriver( Property *pProperty );
   MotionCurveDriver *GetMotionCurveDriver( const char *name );
   int AddMotionCurveDriver( MotionCurveDriver *pMotionCurveDriver );
   void ApplyDrivers( MotionInstance *pMotionInstance, SnipeObject *pObject );

   void ComputeParentToBone( Vector bonedirection, Vector rolldirection, Vector parentbonedirection, Vector parentrolldirection );

   Time GetStartTime();
   Time GetStopTime();

// SnipeObject overrides
   virtual UINT GetIconIndex() { return ICONINDEX_MOTIONCHANNEL; };
   virtual void DrawTreeItem( CGLControl *pItem, float x=0.0f );
};
