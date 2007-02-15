#define MAXFRAMEERROR      0.003f

template< typename TYPE >
inline void BasicMotionCurveDriver<TYPE>::CalcTangents(int index)
{
   if (m_keyframes[index].m_bUserDefinedTangents)
      return;

   // calculates default tangents
   for (int nChannelItemIndex = 0; nChannelItemIndex < GetNumChannelItems(); nChannelItemIndex++)
   {
      Vector2 pp;
      bool  noit = false;
      float magnitude, itmagnitude, otmagnitude;

      if (index && index < (int)m_keyframes.GetSize()-1) {
         pp.Set( m_keyframes[index-1].m_time.GetSeconds()-m_keyframes[index+1].m_time.GetSeconds(),
            GetKeyValueForChannelItem(index-1,nChannelItemIndex)-GetKeyValueForChannelItem(index+1,nChannelItemIndex));
         if ((magnitude = fabs(pp.x)) == 0) {
            m_keyframes[index].m_pIT[nChannelItemIndex].Set( 0,0 );
            m_keyframes[index].m_pOT[nChannelItemIndex].Set( 0,0 );
            return;
         }
         pp /= magnitude;
         itmagnitude = m_keyframes[index+1].m_time.GetSeconds() - m_keyframes[index].m_time.GetSeconds();
         otmagnitude = m_keyframes[index].m_time.GetSeconds() - m_keyframes[index-1].m_time.GetSeconds();
         m_keyframes[index].m_pIT[nChannelItemIndex] = pp;
         m_keyframes[index].m_pIT[nChannelItemIndex] *= itmagnitude;
         m_keyframes[index].m_pOT[nChannelItemIndex] = pp;
         m_keyframes[index].m_pOT[nChannelItemIndex] *= otmagnitude;
      }
      else {
         if (index < (int)m_keyframes.GetSize()-1) {
            m_keyframes[index].m_pIT[nChannelItemIndex].Set( m_keyframes[index].m_time.GetSeconds()-m_keyframes[index+1].m_time.GetSeconds(), GetKeyValueForChannelItem(index,nChannelItemIndex)-GetKeyValueForChannelItem(index+1,nChannelItemIndex) );
            m_keyframes[index].m_pIT[nChannelItemIndex] *= 0.5f;
         } else 
            noit = true;

         if (index) {
            m_keyframes[index].m_pOT[nChannelItemIndex].Set( m_keyframes[index-1].m_time.GetSeconds()-m_keyframes[index].m_time.GetSeconds(), GetKeyValueForChannelItem(index-1,nChannelItemIndex)-GetKeyValueForChannelItem(index,nChannelItemIndex) );
            m_keyframes[index].m_pOT[nChannelItemIndex] *= 0.5f;
            if (noit)
               m_keyframes[index].m_pIT[nChannelItemIndex] = m_keyframes[index].m_pOT[nChannelItemIndex];
         } else
            m_keyframes[index].m_pOT[nChannelItemIndex] = m_keyframes[index].m_pIT[nChannelItemIndex];
      }
   }
}


template< typename TYPE >
inline float BasicMotionCurveDriver<TYPE>::InterpolateChannelItemValue( const Time &time, int index, int nChannelItemIndex )
{
   switch (m_keyframes[index].m_nInterpMethod) 
   {
   case BasicMotionCurveDriver<TYPE>::HOLD:
      return GetKeyValueForChannelItem(index,nChannelItemIndex);

   case BasicMotionCurveDriver<TYPE>::LINEAR:
      return GetKeyValueForChannelItem(index,nChannelItemIndex) + (time.GetSeconds() - m_keyframes[index].m_time.GetSeconds())
         /(m_keyframes[index+1].m_time.GetSeconds() - m_keyframes[index].m_time.GetSeconds()) 
         *(GetKeyValueForChannelItem(index+1,nChannelItemIndex) - GetKeyValueForChannelItem(index,nChannelItemIndex));

   case BasicMotionCurveDriver<TYPE>::SPLINE:
   default: 
      {
         float evalx = time.GetSeconds();
         CubicCoefficients1D xcoeff(m_keyframes[index].m_time.GetSeconds(), m_keyframes[index+1].m_time.GetSeconds(),-m_keyframes[index].m_pIT[nChannelItemIndex].x,-m_keyframes[index+1].m_pOT[nChannelItemIndex].x ),
            ycoeff(GetKeyValueForChannelItem(index,nChannelItemIndex),GetKeyValueForChannelItem(index+1,nChannelItemIndex),-m_keyframes[index].m_pIT[nChannelItemIndex].y,-m_keyframes[index+1].m_pOT[nChannelItemIndex].y );

         float xspan = m_keyframes[index+1].m_time.GetSeconds() - m_keyframes[index].m_time.GetSeconds();
         float guesst = (evalx - m_keyframes[index].m_time.GetSeconds()) / xspan;
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
}

template< typename TYPE >
inline float BasicMotionCurveDriver<TYPE>::InterpolateChannelItem(const Time &time,int nChannelItemIndex)
{
   if (IsTangentsDirty())
   {
      SetTangentsDirtyFlag(FALSE);
      CalcTangents();
   }

   float fValue;

   int s = m_keyframes.GetSize();
   Time t = m_keyframes[s-1].m_time;

   if (m_keyframes[m_keyframes.GetSize()-1].m_time <= time)
      return GetKeyValueForChannelItem(m_keyframes.GetSize()-1,nChannelItemIndex);

   if (m_keyframes[0].m_time >= time)
      return GetKeyValueForChannelItem(0,nChannelItemIndex);

//   if (time == m_dtime)
//      return m_dvalue;

   if (m_keyframes[m_dindex].m_time <= time) 
   {
      for (int index = m_dindex; index < (int)m_keyframes.GetSize()-1; index++) 
      {
         if (m_keyframes[index].m_time == time) 
         {
            m_dindex = index;
            fValue = GetKeyValueForChannelItem(index,nChannelItemIndex);
            break;
         } 
         else if (m_keyframes[index+1].m_time == time ) 
         {
            m_dindex = index;
            fValue = GetKeyValueForChannelItem(index+1,nChannelItemIndex);
            break;
         } 
         else if (m_keyframes[index+1].m_time > time) 
         {
            m_dindex = index;
            fValue = InterpolateChannelItemValue( time, index, nChannelItemIndex );
            break;
         }
      }
   }
   else 
   {
      for (int index = m_dindex-1; index>=0; index--) 
      {
         if (m_keyframes[index].m_time == time) 
         {
            m_dindex = index;
            fValue = GetKeyValueForChannelItem(index,nChannelItemIndex);
            break;
         }
         else if (m_keyframes[index+1].m_time == time) 
         {
            m_dindex = index;
            fValue = GetKeyValueForChannelItem(index+1,nChannelItemIndex);
            break;
         }
         else if (m_keyframes[index].m_time < time) 
         {
            m_dindex = index;
            fValue = InterpolateChannelItemValue( time, index, nChannelItemIndex );
            break;
         }
      }
   }
   m_dtime = time;
   return fValue;
}

template< typename TYPE >
inline void BasicMotionCurveDriver<TYPE>::CalcTangents()
{
   for (int i = 0; i < m_keyframes.GetSize(); i++)
      CalcTangents(i);
}

template< typename TYPE >
inline Time BasicMotionCurveDriver<TYPE>::GetStartTime()
{
   Time time;
   if (m_keyframes.GetSize()>0)
      time = m_keyframes[0].m_time;
   return time;
}


template< typename TYPE >
inline Time BasicMotionCurveDriver<TYPE>::GetEndTime()
{
   Time time;
   if (m_keyframes.GetSize()>0)
      time = m_keyframes[m_keyframes.GetSize()-1].m_time;
   return time;
}

template< typename TYPE >
inline void BasicMotionCurveDriver<TYPE>::RemoveAll()
{
   m_dindex = 0;
   m_dtime.SetTagged(true);
   m_keyframes.RemoveAll();
}


template< typename TYPE >
inline int BasicMotionCurveDriver<TYPE>::FindClosestKey(Time time)
{
   if (m_keyframes.GetSize()==0)
      return -1;

   if (time <= m_keyframes[0].m_time)
      return 0;

   if (time >= m_keyframes[m_keyframes.GetSize()-1].m_time)
      return m_keyframes.GetSize()-1;

   int start = 0;
   int stop = m_keyframes.GetSize()-1;
   while (TRUE)
   {
      if (time == m_keyframes[start].m_time)
         return start;
      else if (time == m_keyframes[stop].m_time)
         return stop;

      if (start == stop)
         return start;

      if (((stop-start)*0.5f)<1.0f)
         return start;

      int half = RoundValue((stop-start)*0.5f);
      if (time < m_keyframes[half].m_time)
      {
         stop = half;
      }
      else if (time >= m_keyframes[half].m_time)
      {
         start = half;
      }
   }
}

template< typename TYPE >
inline int BasicMotionCurveDriver<TYPE>::StoreValue( const TYPE &value, const Time &time=GetTime(), BOOL bStoreUndo = TRUE)
{
   int nIndex = FindClosestKey(time);
   if (nIndex < 0)
   {
      m_keyframes.Add(BasicMotionCurveDriver::Keyframe<TYPE>(time,value,GetNumChannelItems()));
      return m_keyframes.GetSize()-1;
   }
   if ( time > m_keyframes[nIndex].m_time )
   {
      nIndex++;
      m_keyframes.InsertAt(nIndex,BasicMotionCurveDriver::Keyframe<TYPE>(time,value,GetNumChannelItems()));
   }
   else if ( time < m_keyframes[nIndex].m_time )
      m_keyframes.InsertAt(nIndex,BasicMotionCurveDriver::Keyframe<TYPE>(time,value,GetNumChannelItems()));
   else
      return nIndex;

   SetTangentsDirtyFlag(TRUE);

   return nIndex;
}

template< typename TYPE >
inline void BasicMotionCurveDriver<TYPE>::SetNumKeys( int nSize )
{
   m_keyframes.SetSize(nSize);
   for (int i = 0; i < nSize; i++)
   {
      m_keyframes[i].m_pIT = SNEW Vector2[GetNumChannelItems()];
      m_keyframes[i].m_pOT = SNEW Vector2[GetNumChannelItems()];
   }
}

template< typename TYPE >
inline void BasicMotionCurveDriver<TYPE>::SetKey(int nIndex, const TYPE &value, const Time &time)
{
   m_keyframes[nIndex].m_value = value;
   m_keyframes[nIndex].m_time = time;
}

template< typename TYPE >
inline void BasicMotionCurveDriver<TYPE>::SetKeyInTangent(int nIndex, Vector2 inTan, int nChannelItemIndex/*=0*/)
{
   m_keyframes[nIndex].m_pIT[nChannelItemIndex] = inTan;
   SetTangentsDirtyFlag(TRUE);
   m_keyframes[nIndex].m_bUserDefinedTangents = TRUE;
}

template< typename TYPE >
inline void BasicMotionCurveDriver<TYPE>::SetKeyOutTangent(int nIndex, Vector2 outTan, int nChannelItemIndex/*=0*/)
{
   m_keyframes[nIndex].m_pOT[nChannelItemIndex] = outTan;
   SetTangentsDirtyFlag(TRUE);
   m_keyframes[nIndex].m_bUserDefinedTangents = TRUE;
}

template< typename TYPE >
inline void BasicMotionCurveDriver<TYPE>::GetKey(int nIndex, TYPE &value, Time &time)
{
   value = m_keyframes[nIndex].m_value;
   time = m_keyframes[nIndex].m_time;
}

template< typename TYPE >
inline Vector2 BasicMotionCurveDriver<TYPE>::GetKeyInTangent(int nIndex, int nChannelItemIndex/*=0*/)
{
   return m_keyframes[nIndex].m_pIT[nChannelItemIndex];
}

template< typename TYPE >
inline Vector2 BasicMotionCurveDriver<TYPE>::GetKeyOutTangent(int nIndex, int nChannelItemIndex/*=0*/)
{
   return m_keyframes[nIndex].m_pOT[nChannelItemIndex];
}

template< typename TYPE >
inline void BasicMotionCurveDriver<TYPE>::SetKeyInterpolationMethod(int nIndex, InterpolationMethod nInterpolationMethod)
{
   m_keyframes[nIndex].m_nInterpMethod = nInterpolationMethod;
}

template< typename TYPE >
inline MotionCurveDriver::InterpolationMethod BasicMotionCurveDriver<TYPE>::GetKeyInterpolationMethod(int nIndex)
{
   return (MotionCurveDriver::InterpolationMethod)m_keyframes[nIndex].m_nInterpMethod;
}