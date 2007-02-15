#include "stdafx.h"
#include "MotionCurve.h"
#include "Properties/MotionCurveDriver.h"
#include "GUI/GLBaseView.h"
#include "ModifiedMotionCurve.h"
#include "GameEngineModifiedMotionCurve.h"

GLImageList MotionCurve::m_glImageListKey;

MotionCurve::~MotionCurve()
{
   if (m_pModifiedMotionCurve)
      delete m_pModifiedMotionCurve;
}

void MotionCurve::RemoveAll()
{
   while (m_pHeadKey)
   {
      RemoveKey(m_pHeadKey);
   }
}

void MotionCurve::RemoveKey( MotionKey *pKey )
{
   ASSERT(pKey);
   if (m_pLastKey==pKey)
   {
      m_pLastKey = pKey->GetPrevKey();
      m_nLastTimeValid = false;
   }
   if (pKey == m_pHeadKey)
      m_pHeadKey = pKey->GetNextKey();
   if (pKey == m_pTailKey)
      m_pTailKey = pKey->GetPrevKey();
   pKey->Remove();
   GetMotionCurveDriver()->RemoveKey(pKey);
}

void MotionCurve::InsertKeyBefore( MotionKey *pAt, MotionKey* pKey )
{
   if (GetMotionCurveDriver())
      GetMotionCurveDriver()->AddKey( pKey );

   if (pAt == m_pHeadKey)
   {
      m_pHeadKey = pKey;
      if (!m_pTailKey)
         m_pTailKey = m_pHeadKey;
   }

   m_pLastKey = pKey;
   m_nLastTimeValid = false;

   if (pAt)
   {
      ASSERT( pKey->GetTime() < pAt->GetTime() );
      pKey->InsertBefore( pAt );
   }

   pKey->OnPositionChanged();
}

void MotionCurve::InsertKeyAfter( MotionKey *pAt, MotionKey* pKey )
{
   if (GetMotionCurveDriver())
      GetMotionCurveDriver()->AddKey( pKey );

   if (pAt == m_pTailKey)
   {
      m_pTailKey = pKey;
      if (!m_pHeadKey)
         m_pHeadKey = m_pTailKey;
   }

   m_pLastKey = pKey;
   m_nLastTimeValid = false;

   if (pAt)
   {
      ASSERT( pKey->GetTime() > pAt->GetTime() );
      pKey->InsertAfter( pAt );
   }

   pKey->OnPositionChanged();
}

void MotionCurve::AddKey( MotionKey *pKey )
{
   InsertKeyAfter( m_pTailKey, pKey );
}

void MotionCurve::PushBounds( BoundingBox &boundingbox, CGLBaseView *pView/*=NULL*/ )
{
   for (MotionKey *pKey=GetHeadKey(); pKey; pKey=pKey->GetNextKey())
      pKey->PushBounds( boundingbox, pView );
}

Time MotionCurve::GetStartTime(BOOL bModified/* = TRUE*/)
{
   if (GetHeadKey(bModified))
      return GetHeadKey(bModified)->GetTime();
   else
      return Time::TT_NOTSET;
}

Time MotionCurve::GetStopTime(BOOL bModified/* = TRUE*/)
{
   if (GetTailKey(bModified))
      return GetTailKey(bModified)->GetTime();
   else
      return Time::TT_NOTSET;
}

MotionKey *MotionCurve::FindClosestKey( const Time &time )
{
   MotionKey *pKey;
   if (m_pLastKey)
      pKey = m_pLastKey;
   else
      pKey = m_pHeadKey;

   if (!pKey)
      return NULL;

   if (pKey->GetTime() != time)
   {
      if (pKey->GetTime() < time)
      {
         MotionKey *pNextKey = pKey->GetNextKey();
         while (pNextKey)
         {
            if (pNextKey->GetTime() > time)
               break;
            pKey = pNextKey;
            pNextKey = pKey->GetNextKey();
         }
      }
      else
      {
         MotionKey *pPrevKey = pKey->GetPrevKey();
         while (pPrevKey)
         {
            pKey = pPrevKey;
            if (pKey->GetTime() <= time)
               break;
            pPrevKey = pKey->GetPrevKey();
         }
      }
   }

   m_pLastKey = pKey;
   m_nLastTimeValid = false;

   return pKey;
}

MotionKey *MotionCurve::CreateMotionKey( const Time &time )
{
   MotionKey *pKey = FindClosestKey( time );
   MotionKey *pNewKey;

   if (pKey && pKey->GetTime() == time)
      pNewKey = pKey;
   else
   {
      pNewKey = AllocateMotionKey();
      pNewKey->SetTime( time );

      if (!pKey)
         AddKey( pNewKey );
      else if (pKey->GetTime() < time)
         InsertKeyAfter( pKey, pNewKey );
      else if (pKey->GetTime() > time)
         InsertKeyBefore( pKey, pNewKey );
   }

   m_pLastKey = pNewKey;
   m_nLastTimeValid = false;

   return pNewKey;
}

void MotionCurve::AddModifiedMotionCurve()
{
   if (m_pModifiedMotionCurve)
      delete m_pModifiedMotionCurve;
   m_pModifiedMotionCurve = SNEW GameEngineModifiedMotionCurve;
//   m_pModifiedMotionCurve = SNEW GemsModifiedMotionCurve;
   m_pModifiedMotionCurve->m_pMotionCurve = (FloatMotionCurve *)this;
   RGBAFloat color = GetColor();
   m_pModifiedMotionCurve->SetColor( color );
   color.SetAlpha( color.GetAlpha() * 0.25f );
   SetColor( color );
   SetModifiedCurveDirty(TRUE);
}

void MotionCurve::RemoveModifiedMotionCurve()
{
   delete m_pModifiedMotionCurve;
   m_pModifiedMotionCurve = NULL;
}

ModifiedMotionCurve *MotionCurve::GetModifiedMotionCurve()
{
   return m_pModifiedMotionCurve;
}

int MotionCurve::FindKeyIndex(MotionKey *pFindKey) 
{ 
   UINT nIndex = 0;
   for (MotionKey *pKey = m_pHeadKey; pKey; pKey = pKey->GetNextKey(), nIndex++)
   {
      if (pKey == pFindKey)
         return nIndex;
   }
   return -1;
}

MotionKey *MotionCurve::GetHeadKey(BOOL bModified/* = TRUE*/)
{
   if (bModified && m_pModifiedMotionCurve)
   {
      if (IsModifiedCurveDirty())
      {
         SetModifiedCurveDirty(FALSE);
         GetModifiedMotionCurve()->Rebuild();
      }
      return ((FloatMotionCurve*)m_pModifiedMotionCurve)->GetHeadKey(bModified);
   }
   return m_pHeadKey;
}

MotionKey *MotionCurve::GetTailKey(BOOL bModified/* = TRUE*/)
{
   if (bModified && m_pModifiedMotionCurve)
   {
      if (IsModifiedCurveDirty())
      {
         SetModifiedCurveDirty(FALSE);
         GetModifiedMotionCurve()->Rebuild();
      }
      return ((FloatMotionCurve*)m_pModifiedMotionCurve)->GetTailKey(bModified);
   }
   return m_pTailKey;
}

MotionKey *MotionCurve::GetLastKey(BOOL bModified/* = TRUE*/)
{
   if (bModified && m_pModifiedMotionCurve)
   {
      if (IsModifiedCurveDirty())
      {
         SetModifiedCurveDirty(FALSE);
         GetModifiedMotionCurve()->Rebuild();
      }
      return ((FloatMotionCurve*)m_pModifiedMotionCurve)->GetLastKey(bModified);
   }
   return m_pLastKey;
}


Time MotionCurve::GetLength(BOOL bModified/* = TRUE*/)
{
   if (bModified && m_pModifiedMotionCurve)
   {
      if (IsModifiedCurveDirty())
      {
         SetModifiedCurveDirty(FALSE);
         GetModifiedMotionCurve()->Rebuild();
      }
      return ((FloatMotionCurve*)m_pModifiedMotionCurve)->GetLength(bModified);
   }
   return m_pTailKey ? m_pTailKey->GetTime() : TAGGEDTIME;
}


/////////////////////////////////////
// FloatMotionCurve

BOOL FloatMotionCurve::m_bDrawInBackground;

void FloatMotionCurve::Draw( CGLBaseView *pView )
{
   if (GetModifiedMotionCurve())
   {
      if (IsModifiedCurveDirty())
      {
         SetModifiedCurveDirty(FALSE);
         GetModifiedMotionCurve()->Rebuild();
      }
      GetModifiedMotionCurve()->Draw(pView);
   }

   if (pView->m_bDoPick)
   {
      for (FloatMotionKey *pKey=GetHeadKey(FALSE); pKey; pKey=pKey->GetNextKey())
         pKey->DrawPick();
      return;
   }

   Vector topLeft(0.0f,0.0f,0.0f);
   pView->ScreenToWorld(topLeft);
   Vector bottomRight((float)pView->m_szControl.cx,(float)pView->m_szControl.cy,0.0f);
   pView->ScreenToWorld(bottomRight);
   Vector2 minView(min(topLeft.x,bottomRight.x),min(topLeft.y,bottomRight.y));
   Vector2 maxView(max(topLeft.x,bottomRight.x),max(topLeft.y,bottomRight.y));

   const int PIXELS_PER_SAMPLE = 8;
   float timePerSample = ((bottomRight.x-topLeft.x)*(float)PIXELS_PER_SAMPLE)/((float)pView->m_szControl.cx);

   Vector2 p[4];
   RGBAFloat color;
   FloatMotionKey *pPrevKey = NULL;

   color = GetColor();
   if (m_bDrawInBackground)
      color.SetAlpha( 0.3f );
   glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );

   glLineWidth( 2.0f );

   glBegin( GL_LINES );
   // draw the lines
   for (FloatMotionKey *pKey=GetHeadKey(FALSE); pKey; pKey=pKey->GetNextKey())
   {
      p[3].Set( pKey->GetTime().GetSeconds(), pKey->GetValue() );
      if (pPrevKey)
      {
         p[2] = p[3] - (pKey->GetInTangent() / 3.0f);

         Vector2 minSegment = p[0];
         Vector2 maxSegment = p[3];

         if (pPrevKey->GetInterpolationMethod()==MotionKey::LINEAR)
         {
            p[1] = p[0] + ((p[3]-p[0])/3.0f);
         }
         if (pKey->GetInterpolationMethod()==MotionKey::LINEAR)
         {
            p[2] = p[3] - ((p[0]-p[3])/3.0f);
         }

         for (int pointIndex = 0; pointIndex < 4; pointIndex++)
         {
            minSegment.y = min(p[pointIndex].y,minSegment.y);
            maxSegment.y = max(p[pointIndex].y,maxSegment.y);
         }

         // if bounding box of segment lies within the window
         if (minSegment.x <= maxView.x && 
             minSegment.y <= maxView.y &&
             minView.x <= maxSegment.x &&
             minView.y <= maxSegment.y)
         {
            float segmentTime = p[3].x-p[0].x;
            int numSubSegments = (int)(0.5f+(segmentTime/timePerSample));
            if (numSubSegments>1)
            {
               Time subSegmentTime((pKey->GetTime()-pPrevKey->GetTime())/numSubSegments);
               Time prevTime = pPrevKey->GetTime()+subSegmentTime;
               float prevValue = GetValue( prevTime, FALSE );
               // draw from first point to end of first sub segment
               glVertex2f( p[0].x,p[0].y);
               glVertex2f( prevTime.GetSeconds(), prevValue);
               // draw second through next to last sub segment
               for (int subSegmentIndex = 0; subSegmentIndex < numSubSegments-2; subSegmentIndex++)
               {
                  Time nextTime = prevTime + subSegmentTime;
                  float nextValue = GetValue( nextTime, FALSE );

                  glVertex2f( prevTime.GetSeconds(), prevValue);
                  glVertex2f( nextTime.GetSeconds(), nextValue);

                  prevTime = nextTime;
                  prevValue = nextValue;
               }
               // draw last sub segment
               glVertex2f( prevTime.GetSeconds(), prevValue);
               glVertex2f( p[3].x,p[3].y);
            }
            else // draw a line
            {
               glVertex2fv( &p[0].x );
               glVertex2fv( &p[3].x );
            }
         }
      }
      p[1] = p[3] + (pKey->GetOutTangent() / 3.0f);
      p[0] = p[3];

      pPrevKey = pKey;
   }
   glEnd();

   if (m_bDrawInBackground)
      return;

#if 0
   pPrevKey = NULL;
   glLineWidth( 1.0f );
   glColor3f( 5.0f, 1.0f, 0.5f );
   glBegin( GL_LINES );
   // draw the tangent lines
   for (FloatMotionKey *pKey=GetHeadKey(FALSE); pKey; pKey=pKey->GetNextKey())
   {
      p[3].Set( pKey->GetTime().GetSeconds(), pKey->GetValue() );
      if (pPrevKey)
      {
         p[2] = p[3] - (pKey->GetInTangent() / 3.0f);

         if (pPrevKey->GetInterpolationMethod()==MotionKey::LINEAR)
         {
            p[1] = p[0] + ((p[3]-p[0])/3.0f);
         }
         if (pKey->GetInterpolationMethod()==MotionKey::LINEAR)
         {
            p[2] = p[3] - ((p[0]-p[3])/3.0f);
         }

         // out
         glVertex2fv( &p[0].x );
         glVertex2fv( &p[1].x );
         // in
         glVertex2fv( &p[2].x );
         glVertex2fv( &p[3].x ); 
      }
      p[1] = p[3] + (pKey->GetOutTangent() / 3.0f);
      p[0] = p[3];
      pPrevKey = pKey;
   }
   glEnd();
#endif

   // draw the keys
   for (FloatMotionKey *pKey=GetHeadKey(FALSE); pKey; pKey=pKey->GetNextKey())
   {
      color = GetColor();
      pKey->GetDrawColor( color );
      glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );
      pKey->Draw();
   }

#if 0 // draw a point at each frame by calling GetValue
   float fps = GetFPS()*8;
   Time tTotalTime = GetLength( FALSE );
   int nFrames = tTotalTime.GetSeconds() * fps + 1;
   if (nFrames > 1)
   {
      color = GetColor();
      glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha()*0.25f );
      glBegin(GL_POINTS);
      for (Time tTime=0; tTime<=tTotalTime; tTime.StepFrame(fps))
         glVertex2f( tTime.GetSeconds(), GetValue( tTime, FALSE ) );
      glEnd();
   }
#endif
}

float FloatMotionCurve::InterpolateValue( const Time &time, FloatMotionKey *pKey )
{
   FloatMotionKey *pNextKey = pKey->GetNextKey();

   switch (pKey->GetInterpolationMethod())
   {
      case FloatMotionKey::HOLD:
         return pKey->GetValue();

      case FloatMotionKey::LINEAR:
         return pKey->GetValue() + (time.GetSeconds() - pKey->GetTime().GetSeconds())
            /(pNextKey->GetTime().GetSeconds() - pKey->GetTime().GetSeconds()) 
            *(pNextKey->GetValue() - pKey->GetValue());

      case FloatMotionKey::SPLINE:
      {
         float evalx = time.GetSeconds();
         float pointax = pKey->GetTime().GetSeconds();
         float pointbx = pNextKey->GetTime().GetSeconds();
         float pointcx = pKey->GetOutTangent().x;
         float pointdx = pNextKey->GetInTangent().x;
         float pointay = pKey->GetValue();
         float pointby = pNextKey->GetValue();
         float pointcy = pKey->GetOutTangent().y;
         float pointdy = pNextKey->GetInTangent().y;

         float xspan = pointbx - pointax;
         float xspan3 = xspan*3;

         // if c going beyond b limit
         if (pointcx > xspan3)
         {
            float ratio = xspan3 / pointcx;
            pointcx = xspan3;
            pointcy *= ratio;
         }

         // if d going beyond a limit
         if (pointdx > xspan3)
         {
            float ratio = xspan3 / pointdx;
            pointdx = xspan3;
            pointdy *= ratio;
         }

         CubicCoefficients1D xcoeff( pointax, pointbx, pointcx, pointdx ),
                             ycoeff( pointay, pointby, pointcy, pointdy );

         float guesst = (evalx - pKey->GetTime().GetSeconds()) / xspan;
         float diffx = evalx - xcoeff.Evaluate( guesst );
         float error = fabs( diffx );
         float lasterror = error;
         float maxerror = MAXFRAMEERROR / 300.0f;

         while (error > maxerror) 
         {
            guesst = guesst + diffx / xcoeff.Derivative(guesst);
            diffx = evalx - xcoeff.Evaluate( guesst );

            error = fabs( diffx );
            if ( error > lasterror ) 
            {  // NOT CONVERGING, PROBABLY BOGUS CHANNEL DATA, WALK USING BUMP
               maxerror *= 10;  // DON'T BE AS ACCURATE BECAUSE THIS IS MUCH SLOWER
               int steps = (int)(xspan / maxerror);
               float deltat = 1.0f / steps;
               xcoeff.InitFD( steps );
               int i;
               diffx = error;
               for ( i=0, guesst=0.0; diffx>maxerror && i<steps; guesst+=deltat, i++ )
                  diffx = fabs(evalx - xcoeff.BumpFD());
               break;
            }
            lasterror = error;
         }
         return ycoeff.Evaluate( guesst );
      }
   }

   return 0.0f;
}

float FloatMotionCurve::GetValue( const Time &time, BOOL bModified/* = TRUE*/ )
{
   if (bModified && GetModifiedMotionCurve())
   {
      if (IsModifiedCurveDirty())
      {
         SetModifiedCurveDirty(FALSE);
         GetModifiedMotionCurve()->Rebuild();
      }
      return ((FloatMotionCurve*)GetModifiedMotionCurve())->GetValue( time, bModified );
   }

   if (!GetHeadKey(FALSE))
      return 0.0f;

   if (GetTailKey(FALSE)->GetTime() <= time)
      return GetTailKey(FALSE)->GetValue();

   if (GetHeadKey(FALSE)->GetTime() >= time)
      return GetHeadKey(FALSE)->GetValue();

   if (GetLastKey(FALSE))
   {
      if (GetLastTimeValid() && time == GetLastTime())
         return m_dlastValue; // if same as last time
   }
   // if last key is not valid start from the beginning
   else
   {
      SetLastKey(GetHeadKey(FALSE));
   }

   if (GetLastKey(FALSE)->GetTime() <= time) 
   {
      for (FloatMotionKey *pKey=GetLastKey(FALSE); pKey; pKey=pKey->GetNextKey())
      {
         FloatMotionKey *pNextKey = pKey->GetNextKey();
         if (pKey->GetTime() == time) 
         {
            SetLastKey( pKey );
            m_dlastValue = pKey->GetValue();
            break;
         } 
         else if (pNextKey->GetTime() == time) 
         {
            SetLastKey( pKey );
            m_dlastValue = pNextKey->GetValue();
            break;
         } 
         else if (pNextKey->GetTime() > time) 
         {
            SetLastKey( pKey );
            m_dlastValue = InterpolateValue( time, pKey );
            break;
         }
      }
   }
   else 
   {
      for (FloatMotionKey *pKey=GetLastKey(FALSE)->GetPrevKey(); pKey; pKey=pKey->GetPrevKey())
      {
         FloatMotionKey *pNextKey = pKey->GetNextKey();
         if (pKey->GetTime() == time) 
         {
            SetLastKey( pKey );
            m_dlastValue = pKey->GetValue();
            break;
         }
         else if (pNextKey->GetTime() == time) 
         {
            SetLastKey( pKey );
            m_dlastValue = pNextKey->GetValue();
            break;
         }
         else if (pKey->GetTime() < time) 
         {
            SetLastKey( pKey );
            m_dlastValue = InterpolateValue( time, pKey );
            break;
         }
      }
   }

   SetLastTime(time);
   SetLastTimeValid(TRUE);
   return m_dlastValue;
}

FloatMotionKey *FloatMotionCurve::StoreValue( float fValue, const Time &time/*=GetTime()*/, BOOL bStoreUndo/*=NULL*/ )
{
   FloatMotionKey *pNewKey = (FloatMotionKey *)CreateMotionKey( time );
   pNewKey->SetValue( fValue );

   SetMinAndMaxValueDirty(TRUE);

   SetModifiedCurveDirty(TRUE);

   return pNewKey;
}

void FloatMotionCurve::CalcMinAndMaxValue()
{
   FloatMotionKey *pMotionKey;

   for (pMotionKey = GetHeadKey(); pMotionKey; pMotionKey = (FloatMotionKey*)pMotionKey->GetNextKey())
   {
      if (pMotionKey == GetHeadKey())
      {
         m_fMaxValue = pMotionKey->GetValue();
      }
      else if (pMotionKey->GetValue() > m_fMaxValue)
      {
         m_fMaxValue = pMotionKey->GetValue();
      }
   }

   for (pMotionKey = GetHeadKey(); pMotionKey; pMotionKey = (FloatMotionKey*)pMotionKey->GetNextKey())
   {
      if (pMotionKey == GetHeadKey())
      {
         m_fMinValue = pMotionKey->GetValue();
      }
      else if (pMotionKey->GetValue() < m_fMinValue)
      {
         m_fMinValue = pMotionKey->GetValue();
      }
   }
   SetMinAndMaxValueDirty(FALSE);
}

float FloatMotionCurve::GetMinValue()
{
   if (IsMinAndMaxValueDirty())
      CalcMinAndMaxValue();
   return m_fMinValue;
}

float FloatMotionCurve::GetMaxValue()
{
   if (IsMinAndMaxValueDirty())
      CalcMinAndMaxValue();
   return m_fMaxValue;
}


/////////////////////////////////////
// StringMotionCurve

BOOL StringMotionCurve::m_bDrawInBackground;

void StringMotionCurve::Draw( CGLBaseView *pView )
{
   if (pView->m_bDoPick)
   {
      for (StringMotionKey *pKey=GetHeadKey(FALSE); pKey; pKey=pKey->GetNextKey())
         pKey->DrawPick();
      return;
   }

   Vector topLeft(0.0f,0.0f,0.0f);
   pView->ScreenToWorld(topLeft);
   Vector bottomRight((float)pView->m_szControl.cx,(float)pView->m_szControl.cy,0.0f);
   pView->ScreenToWorld(bottomRight);
   Vector2 minView(min(topLeft.x,bottomRight.x),min(topLeft.y,bottomRight.y));
   Vector2 maxView(max(topLeft.x,bottomRight.x),max(topLeft.y,bottomRight.y));

   const int PIXELS_PER_SAMPLE = 8;
   float timePerSample = ((bottomRight.x-topLeft.x)*(float)PIXELS_PER_SAMPLE)/((float)pView->m_szControl.cx);

   Vector2 p[2];
   RGBAFloat color;
   StringMotionKey *pPrevKey = NULL;

   color = GetColor();
   if (m_bDrawInBackground)
      color.SetAlpha( 0.3f );
   glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );

   glLineWidth( 2.0f );

   glBegin( GL_LINES );
   // draw the lines
   for (StringMotionKey *pKey=GetHeadKey(FALSE); pKey; pKey=pKey->GetNextKey())
   {
      p[1].Set( pKey->GetTime().GetSeconds(), 0.0f );
      if (pPrevKey)
      {
         glVertex2fv( &p[0].x );
         glVertex2fv( &p[1].x );
      }
      p[0] = p[1];
      pPrevKey = pKey;
   }
   glEnd();

   if (m_bDrawInBackground)
      return;

   // draw the keys
   for (StringMotionKey *pKey=GetHeadKey(FALSE); pKey; pKey=pKey->GetNextKey())
   {
      color = GetColor();
      pKey->GetDrawColor( color );
      glColor4f( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );
      pKey->Draw();
   }
}


String StringMotionCurve::GetValue( const Time &time )
{
   if (!GetHeadKey(FALSE))
      return String("");

   if (GetTailKey(FALSE)->GetTime() <= time)
      return GetTailKey(FALSE)->GetValue();

   if (GetHeadKey(FALSE)->GetTime() >= time)
      return GetHeadKey(FALSE)->GetValue();

   if (GetLastKey(FALSE))
   {
      if (GetLastTimeValid() && time == GetLastTime())
         return m_slastValue; // if same as last time
   }
   // if last key is not valid start from the beginning
   else
   {
      SetLastKey(GetHeadKey(FALSE));
   }

   if (GetLastKey(FALSE)->GetTime() <= time) 
   {
      for (StringMotionKey *pKey=GetLastKey(FALSE); pKey; pKey=pKey->GetNextKey())
      {
         StringMotionKey *pNextKey = pKey->GetNextKey();
         if (pKey->GetTime() == time) 
         {
            SetLastKey( pKey );
            m_slastValue = pKey->GetValue();
            break;
         } 
         else if (pNextKey->GetTime() == time) 
         {
            SetLastKey( pKey );
            m_slastValue = pNextKey->GetValue();
            break;
         } 
         else if (pNextKey->GetTime() > time) 
         {
            SetLastKey( pKey );
            m_slastValue = pKey->GetValue();
            break;
         }
      }
   }
   else 
   {
      for (StringMotionKey *pKey=GetLastKey(FALSE)->GetPrevKey(); pKey; pKey=pKey->GetPrevKey())
      {
         StringMotionKey *pNextKey = pKey->GetNextKey();
         if (pKey->GetTime() == time) 
         {
            SetLastKey( pKey );
            m_slastValue = pKey->GetValue();
            break;
         }
         else if (pNextKey->GetTime() == time) 
         {
            SetLastKey( pKey );
            m_slastValue = pNextKey->GetValue();
            break;
         }
         else if (pKey->GetTime() < time) 
         {
            SetLastKey( pKey );
            m_slastValue = pKey->GetValue();
            break;
         }
      }
   }

   SetLastTime(time);
   SetLastTimeValid(TRUE);
   return m_slastValue;
}

StringMotionKey *StringMotionCurve::StoreValue( const String &sValue, const Time &time/*=GetTime()*/, BOOL bStoreUndo/*=NULL*/ )
{
   StringMotionKey *pNewKey = (StringMotionKey *)CreateMotionKey( time );
   pNewKey->SetValue( sValue );

   SetMinAndMaxValueDirty(TRUE);

   return pNewKey;
}
