#pragma once

#include "MotionCurveDriver.h"
#include "MotionCurve.h"

class FloatMotionCurveDriver : public MotionCurveDriver
{
public:
   DECLARE_SNIPEOBJECT_CREATE(FloatMotionCurveDriver, MotionCurveDriver)

   FloatMotionCurve m_motioncurve;

   FloatMotionCurveDriver()
   {
      m_motioncurve.SetMotionCurveDriver( this );
   }

   FloatProperty *GetProperty() { return (FloatProperty *)m_pProperty.GetValue(); }

   MotionKey *StoreValue( float value, const Time &time=GetTime(), BOOL bStoreUndo=TRUE)
   {
      return (MotionKey*)m_motioncurve.StoreValue(value,time,bStoreUndo);
   }

   float GetValue( const Time &time=GetTime() )
   {
      Validate();
      return m_motioncurve.GetValue( time );
   }

// MotionCurveDriver overrides
   virtual SnipeObjectType GetPropertyType() { return SOT_FloatProperty; }
   virtual UINT GetNumMotionCurves() { return 1; }
   virtual MotionCurve *GetMotionCurveAt( UINT nIndex ) { Validate(); return &m_motioncurve; }
   virtual MotionCurveDriver *CreateMotionCurveDriverBiasObject();

// PropertyDriver overrides
   virtual void OnPropertyValueStored( const Time &time, BOOL bStoreUndo );
   virtual void Apply( Time time )
   {
      Validate();
      GetProperty()->SetValue( GetValue( time ) );
   }
   virtual void Apply( Property *pProperty, Time time )
   {
      ASSERT( pProperty->IsKindOf( SOT_FloatProperty ) );

      Validate();
      ((FloatProperty *)pProperty)->SetValue( GetValue( time ) );
   }
};

// could go this route, or use a generic property driver that has a copy of the property at each key
//class BoolMotionCurveDriver : public MotionCurveDriver
//{
//public:
//   DECLARE_SNIPEOBJECT_CREATE(BoolMotionCurveDriver, MotionCurveDriver)
//
//   BoolMotionCurve m_motioncurve;
//
//   BoolProperty *GetProperty() { return (BoolProperty *)m_pProperty.GetValue(); }
//
//   MotionKey *StoreValue( BOOL value, const Time &time = GetTime(), BOOL bStoreUndo = TRUE)
//   {
//      return (MotionKey*)m_motioncurve.StoreValue(value,time,bStoreUndo);
//   }
//
//   BOOL GetValue( const Time &time = GetTime() )
//   {
//      return m_motioncurve.GetValue( time );
//   }
//
//// MotionCurveDriver overrides
//   virtual SnipeObjectType GetPropertyType() { return SOT_BoolProperty; }
//   virtual UINT GetNumMotionCurves() { return 1; }
//   virtual MotionCurve *GetMotionCurveAt( UINT nIndex ) { return &m_motioncurve; }
//   virtual void Apply( Time time )
//   {
//      GetProperty()->SetValue( GetValue( time ) );
//   }
//};

//class IntMotionCurveDriver : public MotionCurveDriver
//{
//public:
//   DECLARE_SNIPEOBJECT_CREATE(IntMotionCurveDriver, MotionCurveDriver)
//
//   IntMotionCurve m_motioncurve;
//
//   IntProperty *GetProperty() { return (IntProperty *)m_pProperty.GetValue(); }
//
//   MotionKey *StoreValue( int value, const Time &time = GetTime(), BOOL bStoreUndo = TRUE)
//   {
//      return (MotionKey*)m_motioncurve.StoreValue(value,time,bStoreUndo);
//   }
//
//   int GetValue( const Time &time = GetTime() )
//   {
//      return m_motioncurve.GetValue( time );
//   }
//
//// MotionCurveDriver overrides
//   virtual SnipeObjectType GetPropertyType() { return SOT_IntProperty; }
//   virtual UINT GetNumMotionCurves() { return 1; }
//   virtual MotionCurve *GetMotionCurveAt( UINT nIndex ) { return &m_motioncurve; }
//   virtual void Apply( Time time )
//   {
//      GetProperty()->SetValue( GetValue( time ) );
//   }
//};

class VectorMotionCurveDriver : public MotionCurveDriver
{
public:
   DECLARE_SNIPEOBJECT_CREATE(VectorMotionCurveDriver, MotionCurveDriver)

   FloatMotionCurve m_motioncurve[3];

   VectorMotionCurveDriver()
   {
      m_motioncurve[0].SetColor( RGBAFloat(1,0,0,1) );
      m_motioncurve[0].SetMotionCurveDriver( this );
      m_motioncurve[1].SetColor( RGBAFloat(0,1,0,1) );
      m_motioncurve[1].SetMotionCurveDriver( this );
      m_motioncurve[2].SetColor( RGBAFloat(0,0,1,1) );
      m_motioncurve[2].SetMotionCurveDriver( this );
   }

   VectorProperty *GetProperty() { return (VectorProperty *)m_pProperty.GetValue(); }

   void StoreValue( const Vector &value, const Time &time = GetTime(), BOOL bStoreUndo = TRUE)
   {
      m_motioncurve[0].StoreValue(value.x,time,bStoreUndo);
      m_motioncurve[1].StoreValue(value.y,time,bStoreUndo);
      m_motioncurve[2].StoreValue(value.z,time,bStoreUndo);
   }
   Vector GetValue( const Time &time = GetTime() )
   {
      Validate();
      return Vector( m_motioncurve[0].GetValue(time), m_motioncurve[1].GetValue(time), m_motioncurve[2].GetValue(time) );
   } 

// MotionCurveDriver overrides
   virtual SnipeObjectType GetPropertyType() { return SOT_VectorProperty; }
   virtual UINT GetNumMotionCurves() { return 3; }
   virtual MotionCurve *GetMotionCurveAt( UINT nIndex ) { Validate(); return &m_motioncurve[nIndex]; }
   virtual MotionCurveDriver *CreateMotionCurveDriverBiasObject();

// PropertyDriver overrides
   virtual void OnPropertyValueStored( const Time &time, BOOL bStoreUndo );
   virtual void Apply( Time time )
   {
      Validate();
      GetProperty()->SetValue( GetValue( time ) );
   }
   virtual void Apply( Property *pProperty, Time time )
   {
      ASSERT( pProperty->IsKindOf( SOT_VectorProperty ) );

      Validate();
      ((VectorProperty *)pProperty)->SetValue( GetValue( time ) );
   }
};

class Vector2MotionCurveDriver : public MotionCurveDriver
{
public:
   DECLARE_SNIPEOBJECT_CREATE(Vector2MotionCurveDriver, MotionCurveDriver)

   FloatMotionCurve m_motioncurve[2];

   Vector2MotionCurveDriver()
   {
      m_motioncurve[0].SetColor( RGBAFloat(1,0,0,1) );
      m_motioncurve[0].SetMotionCurveDriver( this );
      m_motioncurve[1].SetColor( RGBAFloat(0,1,0,1) );
      m_motioncurve[1].SetMotionCurveDriver( this );
   }

   Vector2Property *GetProperty() { return (Vector2Property *)m_pProperty.GetValue(); }

   void StoreValue( const Vector2 &value, const Time &time = GetTime(), BOOL bStoreUndo = TRUE)
   {
      m_motioncurve[0].StoreValue(value.x,time,bStoreUndo);
      m_motioncurve[1].StoreValue(value.y,time,bStoreUndo);
   }
   Vector2 GetValue( const Time &time = GetTime() )
   {
      Validate();
      return Vector2( m_motioncurve[0].GetValue(time), m_motioncurve[1].GetValue(time) );
   } 

// MotionCurveDriver overrides
   virtual SnipeObjectType GetPropertyType() { return SOT_Vector2Property; }
   virtual UINT GetNumMotionCurves() { return 2; }
   virtual MotionCurve *GetMotionCurveAt( UINT nIndex ) { Validate(); return &m_motioncurve[nIndex]; }
   virtual MotionCurveDriver *CreateMotionCurveDriverBiasObject();

// PropertyDriver overrides
   virtual void Apply( Time time )
   {
      Validate();
      GetProperty()->SetValue( GetValue( time ) );
   }
   virtual void Apply( Property *pProperty, Time time )
   {
      ASSERT( pProperty->IsKindOf( SOT_Vector2Property ) );

      Validate();
      ((Vector2Property *)pProperty)->SetValue( GetValue( time ) );
   }
};

class EulerMotionCurveDriver: public VectorMotionCurveDriver
{
public:
   DECLARE_SNIPEOBJECT_CREATE(EulerMotionCurveDriver, VectorMotionCurveDriver)

   RotateProperty *GetProperty() { return (RotateProperty *)m_pProperty.GetValue(); }

// MotionCurveDriver overrides
   virtual SnipeObjectType GetPropertyType() { return SOT_RotateProperty; }
   virtual MotionCurveDriver *CreateMotionCurveDriverBiasObject();
};

class QuaternionMotionCurveDriver: public MotionCurveDriver
{
public:
   DECLARE_SNIPEOBJECT_CREATE(QuaternionMotionCurveDriver, MotionCurveDriver)

   FloatMotionCurve m_motioncurve[4];

   QuaternionMotionCurveDriver()
   {
      m_motioncurve[0].SetColor( RGBAFloat(1,0,0,1) );
      m_motioncurve[0].SetMotionCurveDriver( this );
      m_motioncurve[1].SetColor( RGBAFloat(0,1,0,1) );
      m_motioncurve[1].SetMotionCurveDriver( this );
      m_motioncurve[2].SetColor( RGBAFloat(0,0,1,1) );
      m_motioncurve[2].SetMotionCurveDriver( this );
      m_motioncurve[3].SetColor( RGBAFloat(0,1,1,1) );
      m_motioncurve[3].SetMotionCurveDriver( this );
   }

   RotateProperty *GetProperty() { return (RotateProperty *)m_pProperty.GetValue(); }

   void StoreValue( const Quaternion &quat, const Time &time=GetTime(), BOOL bStoreUndo=TRUE )
   {
      m_motioncurve[0].StoreValue( quat.GetV().x, time, bStoreUndo );
      m_motioncurve[1].StoreValue( quat.GetV().y, time, bStoreUndo );
      m_motioncurve[2].StoreValue( quat.GetV().z, time, bStoreUndo );
      m_motioncurve[3].StoreValue( quat.GetW(), time, bStoreUndo );
   }
   Quaternion GetValue( const Time &time = GetTime() )
   {
      Validate();
      return Quaternion( m_motioncurve[0].GetValue(time), m_motioncurve[1].GetValue(time), m_motioncurve[2].GetValue(time), m_motioncurve[3].GetValue(time) );
   } 

// MotionCurveDriver overrides
   virtual SnipeObjectType GetPropertyType() { return SOT_RotateProperty; }
   virtual UINT GetNumMotionCurves() { return 4; }
   virtual MotionCurve *GetMotionCurveAt( UINT nIndex ) { Validate(); return &m_motioncurve[nIndex]; }
   virtual MotionCurveDriver *CreateMotionCurveDriverBiasObject();

// PropertyDriver overrides
   virtual void Apply( Time time )
   {
      Validate();
      GetProperty()->SetValue( GetValue( time ) );
   }
   virtual void Apply( Property *pProperty, Time time )
   {
      ASSERT( pProperty->IsKindOf( SOT_RotateProperty ) );

//      Quaternion quat( m_motioncurve[0].GetValue(time), m_motioncurve[1].GetValue(time), m_motioncurve[2].GetValue(time), m_motioncurve[3].GetValue(time) );
      Validate();
      ((RotateProperty *)pProperty)->SetValue( GetValue( time ) );
   }
};

class StringMotionCurveDriver : public MotionCurveDriver
{
public:
   DECLARE_SNIPEOBJECT_CREATE(StringMotionCurveDriver, MotionCurveDriver)

   StringMotionCurve m_motioncurve;

   StringMotionCurveDriver()
   {
      m_motioncurve.SetColor( RGBAFloat(1,1,0,1) );
      m_motioncurve.SetMotionCurveDriver( this );
   }

   StringProperty *GetProperty() { return (StringProperty *)m_pProperty.GetValue(); }

   void StoreValue( const String &value, const Time &time = GetTime(), BOOL bStoreUndo = TRUE)
   {
      m_motioncurve.StoreValue(value,time,bStoreUndo);
   }
   String GetValue( const Time &time = GetTime() )
   {
      Validate();
      return m_motioncurve.GetValue(time);
   } 

   // MotionCurveDriver overrides
   virtual SnipeObjectType GetPropertyType() { return SOT_StringProperty; }
   virtual UINT GetNumMotionCurves() { return 1; }
   virtual MotionCurve *GetMotionCurveAt( UINT nIndex ) { Validate(); return &m_motioncurve; }

   // PropertyDriver overrides
   virtual void Apply( Time time )
   {
      Validate();
      GetProperty()->SetValue( GetValue( time ) );
   }
   virtual void Apply( Property *pProperty, Time time )
   {
      ASSERT( pProperty->IsKindOf( SOT_StringProperty ) );

      Validate();
      ((StringProperty *)pProperty)->SetValue( GetValue( time ) );
   }
};
