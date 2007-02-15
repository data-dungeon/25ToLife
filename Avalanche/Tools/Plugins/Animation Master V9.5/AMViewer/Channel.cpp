//

#include "stdafx.h"
#include "Channel.h"
#include "Matrix34.h"
#include "AMViewerDoc.h"

void ChannelItem::CalcTangents(int index)
{
   Vector2 pp;

   BOOL  noit = FALSE;
   float magnitude, itmagnitude, otmagnitude;

   if (index && index < m_keyframes.GetSize()-1) {
      pp.Set( m_keyframes[index-1].m_time.GetSeconds()-m_keyframes[index+1].m_time.GetSeconds(),
       m_keyframes[index-1].m_value-m_keyframes[index+1].m_value );
      if ((magnitude = (float)fabs(pp.x)) == 0) {
         m_keyframes[index].m_it.Set( 0,0 );
         m_keyframes[index].m_ot.Set( 0,0 );
         return;
      }
      pp /= magnitude;
      itmagnitude = m_keyframes[index+1].m_time.GetSeconds() - m_keyframes[index].m_time.GetSeconds();
      otmagnitude = m_keyframes[index].m_time.GetSeconds() - m_keyframes[index-1].m_time.GetSeconds();
      m_keyframes[index].m_it = pp;
      m_keyframes[index].m_it *= itmagnitude;
      m_keyframes[index].m_ot = pp;
      m_keyframes[index].m_ot *= otmagnitude;
   }
   else {
      if (index < m_keyframes.GetSize()-1) {
         m_keyframes[index].m_it.Set( m_keyframes[index].m_time.GetSeconds()-m_keyframes[index+1].m_time.GetSeconds(), m_keyframes[index].m_value-m_keyframes[index+1].m_value );
         m_keyframes[index].m_it *= .5;
      } else 
         noit = TRUE;

      if (index) {
         m_keyframes[index].m_ot.Set( m_keyframes[index-1].m_time.GetSeconds()-m_keyframes[index].m_time.GetSeconds(), m_keyframes[index-1].m_value-m_keyframes[index].m_value );
         m_keyframes[index].m_ot *= .5;
         if (noit)
            m_keyframes[index].m_it = m_keyframes[index].m_ot;
      } else
         m_keyframes[index].m_ot = m_keyframes[index].m_it;
   }
}

float ChannelItem::InterpolateValue( const Time &time, int index)
{
   switch (m_keyframes[index].m_interpmethod) {
      case Keyframe::HOLD:
         return m_keyframes[index].m_value;
      case Keyframe::LINEAR:
         return m_keyframes[index].m_value + (time.GetSeconds() - m_keyframes[index].m_time.GetSeconds())
          /(m_keyframes[index+1].m_time.GetSeconds() - m_keyframes[index].m_time.GetSeconds()) 
          *(m_keyframes[index+1].m_value - m_keyframes[index].m_value);
      default: {
      case Keyframe::SPLINE:
         float evalx = time.GetSeconds();
         CubicCoefficients1D xcoeff( m_keyframes[index].m_time.GetSeconds(), m_keyframes[index+1].m_time.GetSeconds(), -m_keyframes[index].m_it.x, -m_keyframes[index+1].m_ot.x ),
                             ycoeff( m_keyframes[index].m_value, m_keyframes[index+1].m_value, -m_keyframes[index].m_it.y, -m_keyframes[index+1].m_ot.y );
         
         float xspan = m_keyframes[index+1].m_time.GetSeconds() - m_keyframes[index].m_time.GetSeconds();
         float guesst = (evalx - m_keyframes[index].m_time.GetSeconds()) / xspan;
         float diffx = evalx - xcoeff.Evaluate( guesst );
         float error = (float)fabs( diffx );
         float lasterror = error;
         float maxerror = MAXFRAMEERROR / CAMViewerDoc::m_fps;
         while (error > maxerror) {
            guesst = guesst + diffx / xcoeff.Derivative(guesst);
            diffx = evalx - xcoeff.Evaluate( guesst );

            error = (float)fabs( diffx );
            if ( error > lasterror ) {  // NOT CONVERGING, PROBABLY BOGUS CHANNEL DATA, WALK USING BUMP
               maxerror *= 10;  // DON'T BE AS ACCURATE BECAUSE THIS IS MUCH SLOWER
               int steps = (int)(xspan / maxerror);
               float deltat = 1.0f / steps;
               xcoeff.InitFD( steps );
               int i;
               diffx = error;
               for ( i=0, guesst=0.0; diffx>maxerror && i<steps; guesst+=deltat, i++ )
                  diffx = (float)fabs(evalx - xcoeff.BumpFD());
               break;
            }
            lasterror = error;
         }
         return ycoeff.Evaluate( guesst );
      }
   }
}

float ChannelItem::Interpolate(const Time &time)
{
   if (m_keyframes[m_keyframes.GetSize()-1].m_time <= time)
      return m_keyframes[m_keyframes.GetSize()-1].m_value;

   if (m_keyframes[0].m_time >= time)
      return m_keyframes[0].m_value;

   if (time == m_cachedtime)
      return m_cachedvalue;

   if (m_keyframes[m_cachedindex].m_time <= time) {
      for (int index = m_cachedindex; index < m_keyframes.GetSize()-1; index++) {
         if (m_keyframes[index].m_time == time) {
            m_cachedindex = index;
            m_cachedvalue = m_keyframes[index].m_value;
            break;
         } else if (m_keyframes[index+1].m_time == time ) {
            m_cachedindex = index;
            m_cachedvalue = m_keyframes[index+1].m_value;
            break;
         } else if (m_keyframes[index+1].m_time > time) {
            m_cachedindex = index;
            m_cachedvalue = InterpolateValue( time, index );
            break;
         }
      }
   }
   else {
      for (int index = m_cachedindex-1; index>=0; index--) {
         if (m_keyframes[index].m_time == time) {
            m_cachedindex = index;
            m_cachedvalue = m_keyframes[index].m_value;
            break;
         } else if (m_keyframes[index+1].m_time == time) {
            m_cachedindex = index;
            m_cachedvalue = m_keyframes[index+1].m_value;
            break;
         } else if (m_keyframes[index].m_time < time) {
            m_cachedindex = index;
            m_cachedvalue = InterpolateValue( time, index );
            break;
         }
      }
   }
   m_cachedtime = time;
   return m_cachedvalue;
}


TSQ Channel::GetTSQ(const Time &time)
{
   TSQ tsq;

   tsq.translate.Set(m_channelitem[XTRANS].Interpolate(time),
    m_channelitem[YTRANS].Interpolate(time),
    m_channelitem[ZTRANS].Interpolate(time));
   tsq.scale.Set(m_channelitem[XSCALE].Interpolate(time),
    m_channelitem[YSCALE].Interpolate(time),
    m_channelitem[ZSCALE].Interpolate(time));
   tsq.quat.Set(m_channelitem[XQUAT].Interpolate(time),
    m_channelitem[YQUAT].Interpolate(time),
    m_channelitem[ZQUAT].Interpolate(time),
    m_channelitem[WQUAT].Interpolate(time));
   tsq.quat.Normalize();
   return tsq;
}

void Channel::ComputeParentToBone(const Vector &bonedirection, const Vector &rolldirection,
 const Vector &parentbonedirection, const Vector &parentrolldirection)
{
   if ( parentbonedirection != Vector( 0 )) {
      m_bonetoparent = ToMatrix33(VectorsToStandard( parentbonedirection, parentrolldirection ))
       * ToMatrix33(VectorsToStandard( bonedirection, rolldirection )).Transpose();
   }
   else
      m_bonetoparent.SetIdentity();
}
