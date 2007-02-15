#pragma once

#include <ostream>

// Output Stream Wrapper Class
class OStreamBuf : public std::streambuf{};
class OStreamWrapper :
   private virtual OStreamBuf, // ascertain early construction
   public std::ostream
{
public:
   OStreamWrapper(): std::ostream(NULL) {}
};

class OutputWindowStream : public OStreamWrapper
{
};

extern OutputWindowStream g_outputWindow;

// Output Dialog Stream Overrides
extern OutputWindowStream &operator << (OutputWindowStream &os,const char *line);
extern OutputWindowStream &operator << (OutputWindowStream &os,const String &line);
extern OutputWindowStream &operator << (OutputWindowStream &os,const int &value);
extern OutputWindowStream &operator << (OutputWindowStream &os,const UINT &value);
extern OutputWindowStream &operator << (OutputWindowStream &os,const long &value);
extern OutputWindowStream &operator << (OutputWindowStream &os,const float &value);
extern OutputWindowStream &operator << (OutputWindowStream &os,const double &value);

// timer
extern void TimerStart(LARGE_INTEGER &starttime);
extern float TimerStop(LARGE_INTEGER &starttime);

class OutputWindowTimer
{
public:
   String m_strMessage;
   LARGE_INTEGER m_starttime;

   OutputWindowTimer( String strMessage )
   {
      m_strMessage = strMessage;
      TimerStart( m_starttime );
   }
   ~OutputWindowTimer()
   {
      DisplayCurrentElapsed( m_strMessage );
   }

   void DisplayCurrentElapsed( String strMessage )
   {
      if (strMessage.Find("%1") == -1)
         strMessage += ": %1";
      float fSecondsElapsed = TimerStop( m_starttime );
      strMessage.Replace( "%1", String(fSecondsElapsed) + " seconds" );
      g_outputWindow << strMessage << "\n";
   }
};

// misc
UINT GetHashKey(const char *str);

enum CoordinateSpace
{
   Space_World =         0,
   Space_Object =        1,
   Space_View =          2,
};

// progress bar
extern void					StartProgressBar( const String &strmessage,int totalitems);
extern void					StepProgressBar();
extern void					StopProgressBar();