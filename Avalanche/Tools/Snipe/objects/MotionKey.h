#pragma once

#include "Component.h"
#include "../GUI/GLBaseView.h"

class MotionCurve;

class MotionKey : public Component
{
private:
   Time       m_time;
   MotionKey *m_pPrevKey, *m_pNextKey;

public:
   class DeleteInfo : public Component::DeleteInfo
   {
   private:
      MotionKey *m_pOldPrevKey;
      MotionKey *m_pOldNextKey;
      MotionCurve *m_pParentMotionCurve;

   public:
      DeleteInfo( SnipeObject *pSnipeObject )
         : Component::DeleteInfo( pSnipeObject )
      {
         m_pOldPrevKey = NULL;
         m_pOldNextKey = NULL;
         m_pParentMotionCurve = NULL;
      }
      ~DeleteInfo();

      void SetOldPrevKey( MotionKey *pKey ) { AssignWithReference( (SnipeObject *&)m_pOldPrevKey, (SnipeObject *)pKey ); }
      void SetOldNextKey( MotionKey *pKey ) { AssignWithReference( (SnipeObject *&)m_pOldNextKey, (SnipeObject *)pKey ); }
      void SetParentMotionCurve( MotionCurve *pParentMotionCurve ) { m_pParentMotionCurve = pParentMotionCurve; }

      MotionKey *GetOldPrevKey() { return m_pOldPrevKey; }
      MotionKey *GetOldNextKey() { return m_pOldNextKey; }
      MotionCurve *GetParentMotionCurve() { return m_pParentMotionCurve; }

   };
   virtual SnipeObject::DeleteInfo *AllocDeleteInfo() { return SNEW DeleteInfo(this); }

   DECLARE_COMPONENT_PURE( MotionKey, Component )

   enum InterpolationMethod { LINEAR, SPLINE, HOLD };

   MotionKey()
   {
      m_time.SetTagged();
      m_pPrevKey = NULL;
      m_pNextKey = NULL;
   }
   MotionKey( UINT nID )
   {
      m_time.SetTagged();
      m_pPrevKey = NULL;
      m_pNextKey = NULL;
   }

   MotionKey *GetNextKey() { return m_pNextKey; }
   MotionKey *GetPrevKey() { return m_pPrevKey; }

   void InsertBefore(MotionKey *pKey);
   void InsertAfter(MotionKey *pKey);
   void Remove();

   const Time &GetTime() const { return m_time; }
   void SetTime(const Time &time) { m_time = time; OnPositionChanged(); }

   MotionCurve *GetParentCurve();

// Overridables
   virt_base virtual void OnPositionChanged();
   virt_base virtual void Draw() {}
   virt_base virtual void DrawPick() {}

// SnipeObject overrides
   virtual BOOL Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError );
   virtual BOOL UnDelete( SnipeObject::DeleteInfo *pDeleteInfo );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL Load( BaseStream &bs );
};

class FloatMotionKey : public MotionKey
{
public:
   DECLARE_COMPONENT_CREATE( FloatMotionKey, MotionKey )

   class BiasHandle : public Property
   {
   public:

      static FloatProperty::Info m_fMagnitudeInfo;
      static FloatProperty::Info m_fAlphaInfo;

      FloatProperty   m_fMagnitude;
      FloatProperty   m_fAlpha;

      BiasHandle::BiasHandle() { }

      FloatProperty *GetMagnitude() { return &m_fMagnitude; }
      FloatProperty *GetAlpha() { return &m_fAlpha; }
   };

   static FloatProperty::Info m_fValueInfo;
   static Property::Info m_inBiasInfo;
   static Property::Info m_outBiasInfo;
   static BoolProperty::Info m_bPeakedInfo;

private:
   enum SaveFlags { FMKF_NONE=0, FMKF_BIAS=1<<0, FMKF_PEEKED=1<<1 };

   FloatProperty m_fValue;
   BiasHandle m_inBias;
   BiasHandle m_outBias;
   BoolProperty m_bPeaked;

   Vector2 m_inTangent;
   Vector2 m_outTangent;

   int     m_nInterpolationMethod;
   UINT    m_bDirtyTangents : 1;

public:
   FloatMotionKey()
   {
      Init();
   }
   FloatMotionKey( UINT nID )
   {
      Init();
   }

   void Init()
   {
      m_fValue.Init( &m_fValueInfo, this );
      m_inBias.Init( &m_inBiasInfo, this );
      m_inBias.m_fMagnitude.Init( &BiasHandle::m_fMagnitudeInfo, &m_inBias );
      m_inBias.m_fAlpha.Init( &BiasHandle::m_fAlphaInfo, &m_inBias );
      m_outBias.Init( &m_outBiasInfo, this );
      m_outBias.m_fMagnitude.Init( &BiasHandle::m_fMagnitudeInfo, &m_outBias );
      m_outBias.m_fAlpha.Init( &BiasHandle::m_fAlphaInfo, &m_outBias );
      m_bPeaked.Init( &m_bPeakedInfo, this );
      m_nInterpolationMethod = SPLINE;
      m_bDirtyTangents = TRUE;
   }

   FloatMotionKey *GetNextKey() { return (FloatMotionKey *)MotionKey::GetNextKey(); }
   FloatMotionKey *GetPrevKey() { return (FloatMotionKey *)MotionKey::GetPrevKey(); }

   void SetValue( float fValue ) { m_fValue.StoreValue(fValue); OnPositionChanged(); }
   float GetValue() { return m_fValue.GetValue(); }

   void SetPeaked( BOOL bPeaked=TRUE ) { m_bPeaked.StoreValue( bPeaked); }
   BOOL IsPeaked() { return m_bPeaked.GetValue(); }

   BiasHandle *GetInBiasHandle() { return &m_inBias; }
   BiasHandle *GetOutBiasHandle() { return &m_outBias; }

   void SetInTangent(const Vector2 &v2Tangent);
   Vector2 GetInTangent();
   void SetOutTangent(const Vector2 &v2Tangent);
   Vector2 GetOutTangent();

   void SetInterpolationMethod(InterpolationMethod nInterpolationMethod) { m_nInterpolationMethod = nInterpolationMethod; }
   InterpolationMethod GetInterpolationMethod() { return (InterpolationMethod)m_nInterpolationMethod; }

   BOOL IsTangentsDirty(){ return m_bDirtyTangents; }
   void SetTangentsDirtyFlag( BOOL bFlag=TRUE ) { m_bDirtyTangents = bFlag; }
   void CalcTangents();

// MotionKey overrides
   virtual void OnPositionChanged();
   virtual void Draw();
   virtual void DrawPick();

// SnipeObject overrides
   virtual BOOL GetWorldPos( Vector &pos );
   virtual void Transform( const Matrix44 &matrix );
   virtual void PushBounds( BoundingBox &box, CGLBaseView *pView=NULL );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL Load( BaseStream &bs );
   virtual void OnFocusChanged( BOOL bFocus );
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};

class StringMotionKey : public MotionKey
{
public:
   DECLARE_COMPONENT_CREATE( StringMotionKey, MotionKey )

   static StringProperty::Info m_sValueInfo;

private:
   enum SaveFlags { FMKF_NONE=0, FMKF_BIAS=1<<0, FMKF_PEEKED=1<<1 };

   StringProperty m_sValue;

public:
   StringMotionKey()
   {
      Init();
   }
   StringMotionKey( UINT nID )
   {
      Init();
   }

   void Init()
   {
      m_sValue.Init( &m_sValueInfo, this );
   }

   StringMotionKey *GetNextKey() { return (StringMotionKey *)MotionKey::GetNextKey(); }
   StringMotionKey *GetPrevKey() { return (StringMotionKey *)MotionKey::GetPrevKey(); }

   void SetValue( String sValue ) { m_sValue.StoreValue(sValue); }
   String GetValue() { return m_sValue.GetValue(); }

   // MotionKey overrides
   virtual void OnPositionChanged();
   virtual void Draw();
   virtual void DrawPick();

   // SnipeObject overrides
   virtual BOOL GetWorldPos( Vector &pos );
   virtual void Transform( const Matrix44 &matrix );
   virtual void PushBounds( BoundingBox &box, CGLBaseView *pView=NULL );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL Load( BaseStream &bs );
   virtual void OnFocusChanged( BOOL bFocus );
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};
