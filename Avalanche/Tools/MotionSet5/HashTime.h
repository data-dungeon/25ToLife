// RC  4/29/2002  \HRandy95\Include\HashTime.h

#pragma once

#define HASH_TIME_H

#define MAX_TICKS   2147483647L
#define MAX_SECONDS MAX_TICKS/HashTime::TICKSPERSECOND

#define MINTIME     -HashTime( HashTime::TT_MAX )
#define MAXTIME     HashTime( HashTime::TT_MAX )
#define MINTIMESTEP HashTime( HashTime::TT_MINSTEP )

#define SIGN(x) ((x) >= 0 ? 1 : -1)

double RoundValue( double number, double modulo );
long RoundValue( double number );

class String;

class HashTime 
{
public:
   long m_ticks;

public:
   enum { TICKSPERSECOND=135000 };
   enum TimeType { TT_MINSTEP, TT_MAX };

   HashTime()
   {
      m_ticks = 0L;
   }

   HashTime( double seconds )
   {
      m_ticks = (long)(seconds * TICKSPERSECOND);
   }

   HashTime( double frame, double fps )
   {
      SetSeconds( frame / fps );
   }

   HashTime( TimeType rt )
   {
      switch (rt) {
         case TT_MINSTEP:
            SetMinStep();
            return;
         case TT_MAX:
            SetMax();
            return;
      }
   }

   HashTime &operator = (const double seconds )
   {
      m_ticks = (long)(seconds * TICKSPERSECOND);
      return *this;
   }

   HashTime &operator = ( HashTime other )
   {
      m_ticks = other.m_ticks;
      return *this;
   }

   bool operator == ( HashTime other ) const
   {
      return m_ticks == other.m_ticks;
   }

   bool operator != ( HashTime other ) const
   {
      return m_ticks != other.m_ticks;
   }

   bool operator > ( HashTime other ) const
   {
      return m_ticks > other.m_ticks;
   }

   bool operator < ( HashTime other ) const
   {
      return m_ticks < other.m_ticks;
   }

   bool operator >= ( HashTime other ) const
   {
      return m_ticks >= other.m_ticks;
   }

   bool operator <= ( HashTime other ) const
   {
      return m_ticks <= other.m_ticks;
   }

   HashTime operator + ( HashTime other ) const
   {
      HashTime result( *this );
      result.m_ticks += other.m_ticks;
      return result;
   }

   HashTime operator - ( HashTime other ) const
   {
      HashTime result( *this );
      result.m_ticks -= other.m_ticks;
      return result;
   }

   HashTime operator * ( HashTime other ) const
   {
      HashTime result( *this );
      result.m_ticks *= other.m_ticks;
      return result;
   }

   HashTime operator * ( double n ) const
   {
      HashTime result( *this );
      result.m_ticks = RoundValue(result.m_ticks * n);
      return result;
   }

   double operator / ( HashTime other ) const
   {
      return (double)m_ticks / other.m_ticks;
   }

   HashTime operator / ( double n ) const
   {
      HashTime result( *this );
      result.m_ticks = RoundValue(result.m_ticks / n);
      return result;
   }

   HashTime operator % ( HashTime other ) const
   {
      HashTime result( *this );
      if (other.m_ticks)
         result.m_ticks %= other.m_ticks;
      return result;
   }

   void operator += ( HashTime other )
   {
      m_ticks += other.m_ticks;
   }

   void operator -= ( HashTime other )
   {
      m_ticks -= other.m_ticks;
   }

   void operator *= ( HashTime other )
   {
      m_ticks *= other.m_ticks;
   }

   void operator *= ( double n )
   {
      m_ticks = RoundValue(m_ticks * n);
   }

   void operator /= ( HashTime other )
   {
      m_ticks /= other.m_ticks;
   }

   void operator /= ( double n )
   {
      m_ticks = RoundValue(m_ticks / n);
   }

   HashTime operator - () const
   {
      HashTime result( *this );
      result.m_ticks = -result.m_ticks;
      return result;
   }
/* USE GETSECONDS INSTEAD
   operator double() const
   {
      return (double)m_ticks/TICKSPERSECOND;
   }
*/
   double GetSeconds() const
   {
      return (double)((double)m_ticks/TICKSPERSECOND);
   }

   double GetSecondsDouble() const
   {
      return (double)m_ticks/TICKSPERSECOND;
   }

   void SetSeconds( double seconds )
   {
      m_ticks = RoundValue(seconds * TICKSPERSECOND);
   }

   long GetTicks() const
   {
      return m_ticks;
   }

   void SetTicks( long ticks )
   {
      m_ticks = ticks;
   }

   bool IsZero() const
   {
      return m_ticks == 0L;
   }

   double GetFrame( double fps ) const
   {
      return (double)(GetSeconds() * fps);
   }

   int GetFrameInt( double fps ) const
   {
      return (int)(GetSeconds() * fps + .5);
   }

   void SetFrame( double frame, double fps )
   {
      SetSeconds( frame / fps );
   }

   void StepFrame( double fps )
   {
      m_ticks += RoundValue(1.0/fps * TICKSPERSECOND);
   }

   void StepFrames( double frames, double fps )
   {
      m_ticks += RoundValue(1.0/fps * TICKSPERSECOND * frames);
   }

   void SnapToFrame( double fps )
   {
      long oneframe = RoundValue(1.0/fps * TICKSPERSECOND);
      m_ticks += (long)(SIGN(m_ticks) * 0.5 * oneframe);
      m_ticks -= m_ticks % oneframe;
   }

   void FloorToFrame( double fps )
   {
      long oneframe = RoundValue(1.0/fps * TICKSPERSECOND);
      m_ticks -= m_ticks % oneframe;
   }

   void CeilToFrame( double fps )
   {
      long oneframe = RoundValue(1.0/fps * TICKSPERSECOND);
      long modulus = m_ticks % oneframe;
      if ( modulus )
         m_ticks += oneframe - modulus;
   }

   double GetCell( double fps ) const
   {
      return GetFrame( fps ) + 1.0f;
   }

   void SetCell( double cell, double fps )
   {
      SetFrame( cell-1.0, fps );
   }

   String ToString(double fps, int timeunits, bool treatasdelta=false) const;

   friend HashTime operator * ( double n, HashTime right );
   
   void SetTagged( bool state=true )
   {
      SetTicks( state ? -1 : 0 );
   }

   bool IsTagged()
   {
      return GetTicks() == -1;
   }

   void SetMax()
   {
      SetTicks( MAX_TICKS );
   }

   void SetMin()
   {
      SetTicks( -MAX_TICKS );
   }
   
   void SetMinStep()
   {
      SetTicks( 1 );
   }

   bool IsMax()
   {
      return (m_ticks==MAX_TICKS);
   }

   bool IsMin()
   {
      return (m_ticks==-MAX_TICKS);
   }

   bool IsMinStep()
   {
      return (m_ticks==-1);
   }
};

inline HashTime operator * ( double n, HashTime right )
{
   HashTime result( right );
   result *= n;
   return result;
}

inline HashTime abs( HashTime time )
{
   HashTime result( time );
  // result.SetTicks(abs(result.GetTicks()));
   return result;
}

inline void Round( double &number, double modulo )
{
   number += SIGN(number) * .5f * modulo;
//   number -= (double)fmod(number, modulo);
   number -= (double)((int)number % (int)modulo);
}

inline double RoundValue( double number, double modulo )
{
   double rvalue( number );
   Round( rvalue, modulo );
   return rvalue;
}

inline long RoundValue( double number )
{
   return (long)(number + SIGN(number) * .5f);
}

