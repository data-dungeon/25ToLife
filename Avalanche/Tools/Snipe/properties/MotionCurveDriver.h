#pragma once

#include "PropertyDriver.h"
#include "ComponentList.h"
#include "MotionKey.h"

class MotionCurve;
class ModifiedMotionCurve;

class MotionCurveDriver : public PropertyDriver
{
public:
   DECLARE_SNIPEOBJECT_PURE(MotionCurveDriver, PropertyDriver)

   class RebuildMotionCurveDriverInfo
   {
   public:
      enum RebuildMotionCurveType
      {
         LINEAR, CUBIC,
      };
   public:
      MotionCurveDriver *m_pMotionCurveDriver;
      MotionCurveDriver *m_pRebuiltMotionCurveDriver;
      DeleteInfo *m_pDeleteInfo;
      DeleteInfo *m_pRebuildDeleteInfo;
      String m_name;

      RebuildMotionCurveDriverInfo();
      virtual ~RebuildMotionCurveDriverInfo();
      void Redo();
      void Undo();
   };

public:
//   static FloatProperty::Info m_fReductionBiasInfo;
//   FloatProperty m_fReductionBias;

private:
   CComponentList< MotionKey > m_motionkeylist;
   Time     m_prevapplytime;

public:
            MotionCurveDriver();
   virtual ~MotionCurveDriver();

   void AddKey( MotionKey *pKey );
   void RemoveKey( MotionKey *pKey );
   void RemoveAll();

   void GetPossiblePointerPropertyChoicesBranch( PropertyArray *pPropertyArray, SnipeObjectList *pSnipeObjectList, SnipeObjectType nSOT );
   Time GetLength();

   Time GetStartTime();
   Time GetStopTime();
   float GetMaxValue();
   float GetMinValue();

   void AddModifiedMotionCurves();
   void RemoveModifiedMotionCurves();

   MotionCurve *FindMotionCurve( MotionKey *pKey );

// Overridables
   virt_base virtual SnipeObjectType GetPropertyType() = 0;
   virt_base virtual UINT GetNumMotionCurves() = 0;
   virt_base virtual MotionCurve *GetMotionCurveAt( UINT nIndex ) = 0;
   virt_base virtual RebuildMotionCurveDriverInfo *RebuildCurves( float fTolerance, RebuildMotionCurveDriverInfo::RebuildMotionCurveType nType );
   virt_base virtual MotionCurveDriver *CreateMotionCurveDriverBiasObject() { return NULL; }
   virt_base virtual void SetMotionCurveDriver( MotionCurveDriver *pMotionCurveDriver ) {} // only called for MotionCurveDriverBiasObjects
   virt_base virtual void SetReductionBias( float fBias ) {} // only called for MotionCurveDriverBiasObjects
//   virt_base virtual float GetReductionBias() { return 0.0f; }
   virt_base virtual void SetReductionToleranceParentCache( float fTolerance ) {} // only called for MotionCurveDriverBiasObjects
 //  virt_base virtual float GetReductionTolerance() { return 0.0f; } // only called for MotionCurveDriverBiasObjects
   virt_base virtual void Validate() {}
   virt_base virtual FloatProperty *GetReductionBiasProperty() { return NULL; }

// PropertyDriver overrides
   virtual BOOL IsApplyDirty( Time time ) { return time != m_prevapplytime; }

// SnipeObject overrides
   virtual UINT GetIconIndex() { return ICONINDEX_MOTIONCURVE; };
   virtual BOOL GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL ParseData( BaseStream &bs, const DataHeader &dataheader );
   virtual void Draw( CGLBaseView *pView );
   virtual void PushBounds( BoundingBox &box, CGLBaseView *pView=NULL );
   virtual void OnSelectedChanged();
   virtual void CountPickObjectType( CGLBaseView *pBaseView, UINT &nCount );
   virtual SimpleComponentList *FindComponentList( const String &strComponentType );
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual void PostConstruct();
};

class MotionInstance;

class MotionCurveInstanceDriver : public PropertyDriver
{
public:
   MotionInstance    *m_pMotionInstance;
   MotionCurveDriver *m_pMotionCurveDriver;
   Time               m_prevapplytime;

   MotionCurveInstanceDriver( MotionInstance *pMotionInstance, MotionCurveDriver *pMotionCurveDriver )
   {
      m_pMotionInstance = pMotionInstance;
      m_pMotionCurveDriver = pMotionCurveDriver;
      m_prevapplytime.SetTagged();
   }

// PropertyDriver overrides
   virtual BOOL IsApplyDirty( Time time ) { return time != m_prevapplytime; }
   virtual void Apply( Time time );

// SnipeObject overrides
   virtual BOOL GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList ) { return FALSE; } // anything goes
};
