#include "stdafx.h"
#include "MotionCurve.h"
#include "properties\MotionCurveDriver.h"
#include "gui\Mainfrm.h"
#include "GUI\GLModelView.h"
#include "GUI\GLMotionKeyHandleViewControl.h"

IMPLEMENT_COMPONENT_PURE( MotionKey )

extern CMainFrame *g_pMainFrame;

MotionKey::DeleteInfo::~DeleteInfo()
{
   // these will dereference
   SetOldPrevKey(NULL);
   SetOldNextKey(NULL);
   SetParentMotionCurve(NULL);
}

void MotionKey::InsertBefore(MotionKey *pKey)
{
   if (!pKey)
      return;
   m_pPrevKey = pKey->m_pPrevKey;
   if (pKey->m_pPrevKey)
      pKey->m_pPrevKey->m_pNextKey = this;
   pKey->m_pPrevKey = this;
   m_pNextKey = pKey;
}

void MotionKey::InsertAfter(MotionKey *pKey)
{
   if (!pKey)
      return;
   m_pNextKey = pKey->m_pNextKey;
   if (pKey->m_pNextKey)
      pKey->m_pNextKey->m_pPrevKey = this;
   pKey->m_pNextKey = this;
   m_pPrevKey = pKey;
}

void MotionKey::Remove()
{
   if (m_pPrevKey)
      m_pPrevKey->m_pNextKey = m_pNextKey;
   if (m_pNextKey)
      m_pNextKey->m_pPrevKey = m_pPrevKey;
   m_pPrevKey = NULL;
   m_pNextKey = NULL;
}

void MotionKey::OnPositionChanged()
{
   if (IsSelected())
      g_pMainFrame->GetMainInterface()->GetCurrentSelectionList()->SetBoundsDirty(); // faster to use global
}

MotionCurve *MotionKey::GetParentCurve()
{
   MotionCurveDriver *pMotionCurveDriver = (MotionCurveDriver *)GetOwner();
   return pMotionCurveDriver->FindMotionCurve(this);
}

BOOL MotionKey::Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError )
{
   DeleteInfo *&pMotionKeyDeleteInfo = (DeleteInfo *&)pDeleteInfo;

   if (pMotionKeyDeleteInfo) // Must be redo
   {
      MotionCurve *pParentCurve = pMotionKeyDeleteInfo->GetParentMotionCurve();
      pParentCurve->RemoveKey(this); // actually remove the curve
      return Component::Delete( pDeleteInfo, strOutError );
   }

   pMotionKeyDeleteInfo = SNEW DeleteInfo( this );

   pMotionKeyDeleteInfo->SetOldPrevKey( m_pPrevKey );
   pMotionKeyDeleteInfo->SetOldNextKey( m_pNextKey );
   MotionCurve *pParentMotionCurve = GetParentCurve();

   pMotionKeyDeleteInfo->SetParentMotionCurve( pParentMotionCurve );
   pParentMotionCurve->RemoveKey(this); // actually remove the key from the curve

   return Component::Delete( pDeleteInfo, strOutError );
}

BOOL MotionKey::UnDelete( SnipeObject::DeleteInfo *pDeleteInfo )
{
   DeleteInfo *pMotionKeyDeleteInfo = (DeleteInfo *)pDeleteInfo;

   MotionKey *pOldPrevKey = pMotionKeyDeleteInfo->GetOldPrevKey();
   MotionKey *pOldNextKey = pMotionKeyDeleteInfo->GetOldNextKey();
   MotionCurve *pParentCurve = pMotionKeyDeleteInfo->GetParentMotionCurve();
   if (pOldPrevKey)
   {
      pParentCurve->InsertKeyAfter(pOldPrevKey,this);
   }
   else if (pOldNextKey)
   {
      pParentCurve->InsertKeyBefore(pOldNextKey,this);
   }
   else
   {
      pParentCurve->AddKey(this);
   }
   return Component::UnDelete( pDeleteInfo );
}

void MotionKey::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   SnipeObject::Save( bs );
}

BOOL MotionKey::Load( BaseStream &bs )
{
   return FALSE;
}

//////////////////////////////////////////
// FloatMotionKey

FloatProperty::Info FloatMotionKey::BiasHandle::m_fMagnitudeInfo( "Mag", "Mag", 100.0f, 0.0f, MAX_FLOAT );
FloatProperty::Info FloatMotionKey::BiasHandle::m_fAlphaInfo( "Alpha", "Alpha", 0.0f, -100.0f, 100.0f );

FloatProperty::Info FloatMotionKey::m_fValueInfo( "Value", "Value", 0.0, -MAX_FLOAT, MAX_FLOAT, 0.1f, FloatProperty::Info::FPT_UNITS );
Property::Info FloatMotionKey::m_inBiasInfo( "In Bias", "InBias" );
Property::Info FloatMotionKey::m_outBiasInfo( "Out Bias", "OutBias" );
BoolProperty::Info FloatMotionKey::m_bPeakedInfo( "Peaked", "Peaked", FALSE );

IMPLEMENT_COMPONENT_CREATE( FloatMotionKey, Key )

void FloatMotionKey::SetInTangent(const Vector2 &v2Tangent) 
{ 
   FloatMotionKey *pPrevKey = GetPrevKey();
   FloatMotionKey *pNextKey = GetNextKey();

   Vector2 defaultTangent;
   float defaultMagnitude;

   if (pPrevKey && pNextKey)
   {
      Vector2 pp( pNextKey->GetTime().GetSeconds()-pPrevKey->GetTime().GetSeconds(), pNextKey->GetValue()-pPrevKey->GetValue() );
      float magnitude = fabs(pp.x);
      if (magnitude == 0) // if keys are on top of one another (no tangents)
      {
         m_inBias.GetAlpha()->StoreValue(0.0f);
         m_inBias.GetMagnitude()->StoreValue(100.0f);
         return;
      }
      pp /= magnitude;
      defaultMagnitude = GetTime().GetSeconds() - pPrevKey->GetTime().GetSeconds();
      defaultTangent = pp*defaultMagnitude;
   }
   else
   {
      if (pNextKey)
      {
         defaultTangent.Set( pNextKey->GetTime().GetSeconds()-GetTime().GetSeconds(), pNextKey->GetValue()-GetValue() );
      }

      if (pPrevKey)
      {
         defaultTangent.Set( GetTime().GetSeconds()-pPrevKey->GetTime().GetSeconds(), GetValue()-pPrevKey->GetValue() );
      }
   }

   float alpha;
   float magnitude = 100.0f * (v2Tangent.Length()/defaultMagnitude);
   float defaultAngle = atan2( defaultTangent.x, defaultTangent.y) * RADIANS;
   float desiredAngle = atan2( v2Tangent.x, v2Tangent.y) * RADIANS;
   if (desiredAngle < 0.0f)
   {
      if (desiredAngle > -90.0f)
         desiredAngle = 0.0f;
      else
         desiredAngle = 180.0f;
   }
   if (desiredAngle < defaultAngle)
   {
      alpha = 100.0f * (desiredAngle - defaultAngle) / defaultAngle;
   }
   else
   {
      alpha = 100.0f * (desiredAngle - defaultAngle) / (180.0f - defaultAngle); 
   }
   m_inBias.GetAlpha()->StoreValue(alpha);
   m_inBias.GetMagnitude()->StoreValue(magnitude);
   SetTangentsDirtyFlag();
}

Vector2 FloatMotionKey::GetInTangent() 
{ 
   if (IsTangentsDirty()) 
      CalcTangents(); 
   return m_inTangent;
}

void FloatMotionKey::SetOutTangent(const Vector2 &v2Tangent) 
{ 
   FloatMotionKey *pPrevKey = GetPrevKey();
   FloatMotionKey *pNextKey = GetNextKey();

   Vector2 defaultTangent;
   float defaultMagnitude;

   if (pPrevKey && pNextKey)
   {
      Vector2 pp( pNextKey->GetTime().GetSeconds()-pPrevKey->GetTime().GetSeconds(), pNextKey->GetValue()-pPrevKey->GetValue() );
      float magnitude = fabs(pp.x);
      if (magnitude == 0) // if keys are on top of one another (no tangents)
      {
         m_outBias.GetAlpha()->StoreValue(0.0f);
         m_outBias.GetMagnitude()->StoreValue(100.0f);
         return;
      }
      pp /= magnitude;
      defaultMagnitude = pNextKey->GetTime().GetSeconds() - GetTime().GetSeconds();
      defaultTangent = pp*defaultMagnitude;
   }
   else
   {
      if (pNextKey)
      {
         defaultTangent.Set( pNextKey->GetTime().GetSeconds()-GetTime().GetSeconds(), pNextKey->GetValue()-GetValue() );
      }

      if (pPrevKey)
      {
         defaultTangent.Set( GetTime().GetSeconds()-pPrevKey->GetTime().GetSeconds(), GetValue()-pPrevKey->GetValue() );
      }
   }

   float alpha;
   float magnitude = 100.0f * (v2Tangent.Length()/defaultMagnitude);
   float defaultAngle = atan2( defaultTangent.x, defaultTangent.y) * RADIANS;
   float desiredAngle = atan2( v2Tangent.x, v2Tangent.y) * RADIANS;
   if (desiredAngle < 0.0f)
   {
      if (desiredAngle > -90.0f)
         desiredAngle = 0.0f;
      else
         desiredAngle = 180.0f;
   }
   if (desiredAngle < defaultAngle)
   {
      alpha = 100.0f * (desiredAngle - defaultAngle) / defaultAngle;
   }
   else
   {
      alpha = 100.0f * (desiredAngle - defaultAngle) / (180.0f - defaultAngle); 
   }
   m_outBias.GetAlpha()->StoreValue(alpha);
   m_outBias.GetMagnitude()->StoreValue(magnitude);
   SetTangentsDirtyFlag();
}

Vector2 FloatMotionKey::GetOutTangent() 
{ 
   if (IsTangentsDirty()) 
      CalcTangents(); 
   return m_outTangent; 
}

void FloatMotionKey::CalcTangents()
{
   if (!IsTangentsDirty())
      return;

   SetTangentsDirtyFlag( FALSE );

   FloatMotionKey *pPrevKey = GetPrevKey();
   FloatMotionKey *pNextKey = GetNextKey();

   Vector2 defaultInTangent;
   Vector2 defaultOutTangent;
   float fInMagnitude;
   float fOutMagnitude;

   if (pPrevKey && pNextKey)
   {
      Vector2 pp( pNextKey->GetTime().GetSeconds()-pPrevKey->GetTime().GetSeconds(), pNextKey->GetValue()-pPrevKey->GetValue() );
      float magnitude = fabs(pp.x);
      if (magnitude == 0) // if keys are on top of one another (no tangents)
      {
         m_inTangent.Set(0.0f,0.0f);
         m_outTangent.Set(0.0f,0.0f);
         return;
      }
      pp /= magnitude;
      fInMagnitude = GetTime().GetSeconds() - pPrevKey->GetTime().GetSeconds();
      fOutMagnitude = pNextKey->GetTime().GetSeconds() - GetTime().GetSeconds();

      defaultInTangent = pp*fInMagnitude;
      defaultOutTangent = pp*fOutMagnitude;
   }
   else
   {
      if (pNextKey)
      {
         defaultInTangent.Set( pNextKey->GetTime().GetSeconds()-GetTime().GetSeconds(), pNextKey->GetValue()-GetValue() );
         defaultOutTangent = defaultInTangent;
      }

      if (pPrevKey)
      {
         defaultInTangent.Set( GetTime().GetSeconds()-pPrevKey->GetTime().GetSeconds(), GetValue()-pPrevKey->GetValue() );
         defaultOutTangent = defaultInTangent;
      }
   }

   float alpha = m_inBias.GetAlpha()->GetValue()/100.0f;
   float magnitude = m_inBias.GetMagnitude()->GetValue()/100.0f;
   if (alpha != 0.0)
   {
      float angle = atan2( defaultInTangent.x, defaultInTangent.y) * RADIANS;
      if (alpha < 0.0)
      {
         angle += angle * alpha; //go between 0.0 and angle
      }
      else
      {
         angle += (180.0f - angle) * alpha; // go between angle and 180.0
      }
      m_inTangent.x = (float)sin(angle / RADIANS)*fInMagnitude*magnitude;
      m_inTangent.y = (float)cos(angle / RADIANS)*fInMagnitude*magnitude;
   }
   else if (magnitude != 1.0f)
   {
      m_inTangent = defaultInTangent * magnitude;
   }
   else // use default tangent
   {
      m_inTangent = defaultInTangent;
   }

   if (!m_bPeaked.GetValue())
   {
      m_outBias.GetAlpha()->StoreValue(alpha*100.0f);
      m_outBias.GetMagnitude()->StoreValue(magnitude*100.0f);
   }
   else
   {
      alpha = m_outBias.GetAlpha()->GetValue()/100.0f;
      magnitude = m_outBias.GetMagnitude()->GetValue()/100.0f;
   }
   if (alpha != 0.0)
   {
      float angle = atan2( defaultOutTangent.x, defaultOutTangent.y) * RADIANS;
      if (alpha < 0.0)
      {
         angle += angle * alpha; //go between 0.0 and angle
      }
      else
      {
         angle += (180.0f - angle) * alpha; // go between angle and 180.0
      }
      m_outTangent.x = (float)sin(angle / RADIANS)*fOutMagnitude*magnitude;
      m_outTangent.y = (float)cos(angle / RADIANS)*fOutMagnitude*magnitude;
   }
   else if (magnitude != 1.0f)
   {
      m_outTangent = defaultInTangent * magnitude;
   }
   else // use default tangent
   {
      m_outTangent = defaultInTangent;
   }
}

void FloatMotionKey::OnPositionChanged()
{
   MotionKey::OnPositionChanged();

   SetTangentsDirtyFlag();
   if (GetPrevKey())
      GetPrevKey()->SetTangentsDirtyFlag();
   if (GetNextKey())
      GetNextKey()->SetTangentsDirtyFlag();
}

void FloatMotionKey::PushBounds( BoundingBox &box, CGLBaseView *pView/*=NULL*/ )
{
   Vector vPos( GetTime().GetSeconds(), m_fValue, 0.0f );

   if (pView)
      pView->WorldToView( vPos );

   box.Push( vPos );
}

BOOL FloatMotionKey::GetWorldPos( Vector &pos )
{
   pos.x = GetTime().GetSeconds();
   pos.y = m_fValue;
   pos.z = 0.0f;
   return TRUE;
}

void FloatMotionKey::Transform( const Matrix44 &matrix )
{
   if (m_bTransformed || IsLocked())
      return;

   Vector2 pos;
   pos.y = m_fValue.GetValue();
   pos *= matrix;
   m_fValue.StoreValue(pos.y);
   OnPositionChanged();

   m_bTransformed = TRUE;
}

void FloatMotionKey::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   MotionKey::Save( bs );

   UINT nFlags = FMKF_NONE;
   if (IsPeaked())
      nFlags |= FMKF_PEEKED;
   if (m_inBias.GetAlpha()->GetValue()!=0  || m_inBias.GetMagnitude()->GetValue()!=1.0f || m_outBias.GetAlpha()->GetValue()!=0 || m_outBias.GetMagnitude()->GetValue()!=1.0f)
      nFlags |= FMKF_BIAS;

   bs << sp << nFlags;
   bs << sp << m_fValue;

   if (nFlags & FMKF_BIAS)
   {
      bs << sp << m_inBias.GetAlpha()->GetValue() << sp << m_inBias.GetMagnitude()->GetValue();
      if (nFlags & FMKF_PEEKED)
         bs << sp << m_outBias.GetAlpha()->GetValue() << sp << m_outBias.GetMagnitude()->GetValue();
   }
}

BOOL FloatMotionKey::Load( BaseStream &bs )
{
   UINT nFlags;
   bs >> nFlags;

   float value;
   bs >> value;
   m_fValue.StoreValue(value);

   if (nFlags & FMKF_BIAS)
   {
      float alpha, magnitude;
      bs >> alpha >> magnitude;
      m_inBias.GetAlpha()->StoreValue(alpha);
      m_inBias.GetMagnitude()->StoreValue(magnitude);
      if (nFlags & FMKF_PEEKED)
      {
         bs >> alpha >> magnitude;
         m_bPeaked.SetValue(TRUE);
      }
      m_outBias.GetAlpha()->StoreValue(alpha);
      m_outBias.GetMagnitude()->StoreValue(magnitude);
   }
   SetTangentsDirtyFlag();

   return TRUE;
}

void FloatMotionKey::Draw()
{
   if (!IsDraw())
      return;

   //m_glImageListKey.DrawQuad( p[3], p[3]+50.0f );
   glBegin(GL_POINTS);
      glVertex2f( GetTime().GetSeconds(), GetValue() );
   glEnd();
}

void FloatMotionKey::DrawPick()
{
   glLoadName( (GLuint)this );

   //if (CGLModelView::m_bSinglePick)
   //{
   //   draw half curves in and out
   //}
   //else
      Draw();
}

void FloatMotionKey::OnFocusChanged( BOOL bFocus )
{
   MotionKey::OnFocusChanged( bFocus );

   for (POSITION docpos=GetApp().GetDocList().GetHeadPosition(); docpos; )
   {
      CBaseDoc *pDoc = GetApp().GetDocList().GetNext( docpos );

      if (pDoc->UsesObject( this ))
      {
         for (POSITION viewpos=pDoc->m_viewlist.GetHeadPosition(); viewpos; )
         {
            CGLBaseView *pView = pDoc->m_viewlist.GetNext( viewpos );

            if (pView->GetSVT() != SVT_CGLTimeLineView)
               continue;

            if (bFocus)
            {
               SNEW CGLMotionKeyHandleViewControl( pView, this );
            }
            else
            {
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
}

BOOL FloatMotionKey::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
   case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         if (GetOwner())
         {
            if ( pProperty==m_inBias.GetAlpha() ||
               pProperty==m_inBias.GetMagnitude() ||
               pProperty==m_outBias.GetAlpha() ||
               pProperty==m_outBias.GetMagnitude() ||
               pProperty==&m_bPeaked)
            {
               // if not peeked 
               if (!m_bPeaked.GetValue())
               {
                  // if the out bias property changed we need to keep them in sync
                  if (pProperty==m_outBias.GetAlpha())
                  {
                     m_inBias.GetAlpha()->StoreValue(m_outBias.GetAlpha()->GetValue());
                  }
                  if (pProperty==m_outBias.GetMagnitude())
                  {
                     m_inBias.GetMagnitude()->StoreValue(m_outBias.GetMagnitude()->GetValue());
                  }
               }
               SetTangentsDirtyFlag(TRUE);
            }
         }
         break;
      }
   }

   return Component::OnObjectNotify( msg, wParam, lParam );
}


//////////////////////////////////////////
// StringMotionKey

StringProperty::Info StringMotionKey::m_sValueInfo( "Value", "Value", "" , TRUE );

IMPLEMENT_COMPONENT_CREATE( StringMotionKey, Key )

void StringMotionKey::OnPositionChanged()
{
   MotionKey::OnPositionChanged();
}

void StringMotionKey::PushBounds( BoundingBox &box, CGLBaseView *pView/*=NULL*/ )
{
   Vector vPos( GetTime().GetSeconds(), 0.0, 0.0f );

   if (pView)
      pView->WorldToView( vPos );

   box.Push( vPos );
}

BOOL StringMotionKey::GetWorldPos( Vector &pos )
{
   pos.x = GetTime().GetSeconds();
   pos.y = 0.0f;
   pos.z = 0.0f;
   return TRUE;
}

void StringMotionKey::Transform( const Matrix44 &matrix )
{
   return;
}

void StringMotionKey::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   MotionKey::Save( bs );
   bs << sp << m_sValue;
}

BOOL StringMotionKey::Load( BaseStream &bs )
{
   UINT nFlags;
   bs >> nFlags;

   String value;
   bs >> value;
   m_sValue.StoreValue(value);

   return TRUE;
}

void StringMotionKey::Draw()
{
   if (!IsDraw())
      return;

   //m_glImageListKey.DrawQuad( p[3], p[3]+50.0f );
   glBegin(GL_POINTS);
   glVertex2f( GetTime().GetSeconds(), 0 );
   glEnd();
}

void StringMotionKey::DrawPick()
{
   glLoadName( (GLuint)this );

   //if (CGLModelView::m_bSinglePick)
   //{
   //   draw half curves in and out
   //}
   //else
   Draw();
}

void StringMotionKey::OnFocusChanged( BOOL bFocus )
{
   MotionKey::OnFocusChanged( bFocus );

   for (POSITION docpos=GetApp().GetDocList().GetHeadPosition(); docpos; )
   {
      CBaseDoc *pDoc = GetApp().GetDocList().GetNext( docpos );

      if (pDoc->UsesObject( this ))
      {
         for (POSITION viewpos=pDoc->m_viewlist.GetHeadPosition(); viewpos; )
         {
            CGLBaseView *pView = pDoc->m_viewlist.GetNext( viewpos );

            if (pView->GetSVT() != SVT_CGLTimeLineView)
               continue;

            if (bFocus)
            {
               SNEW CGLMotionKeyHandleViewControl( pView, this );
            }
            else
            {
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
}

BOOL StringMotionKey::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
   case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         if (GetOwner())
         {
         }
         break;
      }
   }

   return Component::OnObjectNotify( msg, wParam, lParam );
}