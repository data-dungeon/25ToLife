// RC  4/29/2002  \HRandy95\Include\HashTime.h

#define __HASHTIME_H

#define MAX_TICKS   2147483647L
#define MAX_SECONDS MAX_TICKS/Time::TICKSPERSECOND

#define MINTIME     -Time( Time::TT_MAX )
#define MAXTIME     Time( Time::TT_MAX )
#define MINTIMESTEP Time( Time::TT_MINSTEP )

#define SIGN(x) ((x) >= 0 ? 1 : -1)

float RoundValue( float number, float modulo );
long RoundValue( double number );

class String;

class Time {
public:
   long m_ticks;

public:
   enum { TICKSPERSECOND=135000 };
   enum TimeType { TT_MINSTEP, TT_MAX };

   Time()
   {
      m_ticks = 0L;
   }

   Time( double seconds )
   {
      m_ticks = (long)(seconds * TICKSPERSECOND);
   }

   Time( double frame, double fps )
   {
      SetSeconds( frame / fps );
   }

   Time( TimeType rt )
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

   Time &operator = (const double seconds )
   {
      m_ticks = (long)(seconds * TICKSPERSECOND);
      return *this;
   }

   Time &operator = ( Time other )
   {
      m_ticks = other.m_ticks;
      return *this;
   }

   bool operator == ( Time other ) const
   {
      return m_ticks == other.m_ticks;
   }

   bool operator != ( Time other ) const
   {
      return m_ticks != other.m_ticks;
   }

   bool operator > ( Time other ) const
   {
      return m_ticks > other.m_ticks;
   }

   bool operator < ( Time other ) const
   {
      return m_ticks < other.m_ticks;
   }

   bool operator >= ( Time other ) const
   {
      return m_ticks >= other.m_ticks;
   }

   bool operator <= ( Time other ) const
   {
      return m_ticks <= other.m_ticks;
   }

   Time operator + ( Time other ) const
   {
      Time result( *this );
      result.m_ticks += other.m_ticks;
      return result;
   }

   Time operator - ( Time other ) const
   {
      Time result( *this );
      result.m_ticks -= other.m_ticks;
      return result;
   }

   Time operator * ( Time other ) const
   {
      Time result( *this );
      result.m_ticks *= other.m_ticks;
      return result;
   }

   Time operator * ( double n ) const
   {
      Time result( *this );
      result.m_ticks = RoundValue(result.m_ticks * n);
      return result;
   }

   float operator / ( Time other ) const
   {
      return (float)m_ticks / other.m_ticks;
   }

   Time operator / ( double n ) const
   {
      Time result( *this );
      result.m_ticks = RoundValue(result.m_ticks / n);
      return result;
   }

   Time operator % ( Time other ) const
   {
      Time result( *this );
      if (other.m_ticks)
         result.m_ticks %= other.m_ticks;
      return result;
   }

   void operator += ( Time other )
   {
      m_ticks += other.m_ticks;
   }

   void operator -= ( Time other )
   {
      m_ticks -= other.m_ticks;
   }

   void operator *= ( Time other )
   {
      m_ticks *= other.m_ticks;
   }

   void operator *= ( double n )
   {
      m_ticks = RoundValue(m_ticks * n);
   }

   void operator /= ( Time other )
   {
      m_ticks /= other.m_ticks;
   }

   void operator /= ( double n )
   {
      m_ticks = RoundValue(m_ticks / n);
   }

   Time operator - () const
   {
      Time result( *this );
      result.m_ticks = -result.m_ticks;
      return result;
   }
/* USE GETSECONDS INSTEAD
   operator float() const
   {
      return (float)m_ticks/TICKSPERSECOND;
   }
*/
   float GetSeconds() const
   {
      return (float)((double)m_ticks/TICKSPERSECOND);
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

   float GetFrame( double fps ) const
   {
      return (float)(GetSeconds() * fps);
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

   float GetCell( double fps ) const
   {
      return GetFrame( fps ) + 1.0f;
   }

   void SetCell( double cell, double fps )
   {
      SetFrame( cell-1.0, fps );
   }

   String ToString(double fps, int timeunits, bool treatasdelta=false) const;

   friend Time operator * ( double n, Time right );
   
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

inline Time operator * ( double n, Time right )
{
   Time result( right );
   result *= n;
   return result;
}

inline Time abs( Time time )
{
   Time result( time );
  // result.SetTicks(abs(result.GetTicks()));
   return result;
}

inline void Round( float &number, float modulo )
{
   number += SIGN(number) * .5f * modulo;
//   number -= (float)fmod(number, modulo);
   number -= (float)((int)number % (int)modulo);
}

inline float RoundValue( float number, float modulo )
{
   float rvalue( number );
   Round( rvalue, modulo );
   return rvalue;
}

inline long RoundValue( double number )
{
   return (long)(number + SIGN(number) * .5f);
}
