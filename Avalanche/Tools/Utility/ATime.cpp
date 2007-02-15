// SString.cpp : implementation file
//

#include "stdafx.h"
#include "ATime.h"
#include "AString.h"

static Time::TimeUnit f_timeunits = Time::SMPTE;

UTILITYEXPORT Time::TimeUnit GetTimeUnits()
{
   return f_timeunits;
}

UTILITYEXPORT void SetTimeUnits( Time::TimeUnit timeunits )
{
   f_timeunits = timeunits;
}

String Time::ToString( double fps, Time::TimeUnit timeunits, BOOL treatasdelta ) const
{
   static CString negstring = "-";
   double seconds = GetSeconds();
   CString framestring;

   switch (timeunits)
   {
      case SMPTE: 
      {
         BOOL negative = FALSE;
         if (seconds < 0.0f)
         {
            negative = TRUE;
            seconds = -seconds;
         }
         int intseconds = fast_ftol_unsigned((float)seconds);
         double frames = (seconds-intseconds) * fps;
         Round( frames, .01f );
         if (fps < 100)
            framestring.Format( "%02d:%02d:%02.4g", intseconds/60, intseconds%60, frames );
         else
            framestring.Format( "%02d:%02d:%03.5g", intseconds/60, intseconds%60, frames );
         if (negative)
            framestring = negstring + framestring;
         break;
      }
      case FRAME: 
      {
         double frames = seconds * fps;
         Round( frames, .01f );
         framestring.Format( "%g", frames );
         break;
      }
      case CEL: 
      {
         int offset = treatasdelta?0:1;
         double frames = seconds * fps;
         Round( frames, .01f );
         framestring.Format( "%g", frames+offset );
         break;
      }
   }

   return (LPCTSTR)framestring;
}

Time String::GetTime( double fps, Time::TimeUnit timeunits, BOOL treatasdelta/*=FALSE*/ ) const
{
   static String numstring;
   int minutes=0, seconds=0;
   float frames=0.0f;
   int len=Length(), colon, start=0;

   float framebase = 100.0f;
   if (fps >= 100)
      framebase = 1000.0f;

   for (int i=0; i<=len; i++)
   {
      if (i==len || m_pData[i]==':')
      {
         colon = i;
         numstring = Mid( start, colon-start );
         start = colon+1;

         minutes = seconds;
         seconds = (int)frames;
         frames = (float)atof( numstring.Get());

         if (timeunits == Time::SMPTE)
         {
            if (frames >= framebase)
            {
               seconds += (int)(frames / framebase);
               frames = fmod(frames, framebase);
            }

            if (seconds >= 100)
            {
               minutes += seconds / 100;
               seconds = seconds % 100;
            }
         }
      }
   }

   if (timeunits==Time::CEL && !treatasdelta)
      frames--;

   Time time;
   time.SetSeconds( minutes*60.0f + seconds + frames/fps );
   return time;
}
