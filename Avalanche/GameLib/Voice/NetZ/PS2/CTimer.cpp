
#include "CTimer.h"
#include <stdio.h>
#define TICKS_PER_SECOND 294912000
#define TICKS_PER_MILLISECOND 294912

CTimer::CTimer()
{
    m_nStartTick = (int)GetTickCount();
    m_bStarted = 0;
}

CTimer::~CTimer()
{

}

// resets the timer and returns the number of milliseconds
// ellapsed
void CTimer::Start()
{
    m_bStarted = 1;
    m_nStartTick = GetTickCount();
}

// sets the stop tick and returns the ellapsed time
float CTimer::Stop()
{
    float ellapsed_time = GetEllapsedTime();

    m_bStarted = 0;
    
    return ellapsed_time;
}

// returns the tick difference between the start and stop
// note that the counter will wrap about every 14 seconds
int CTimer::GetTickDifference(unsigned tickStart, unsigned tickStop)
{
    if (tickStop < m_nStartTick)
    {
        // counter has looped
        return ( (TICKS_PER_SECOND - tickStart) + tickStop);
    }
    else
    {
        return (tickStop - tickStart);
    }
}

// returns the ellapsed time
float CTimer::GetEllapsedTime()
{
    if (m_bStarted == 0)
        return 0;
    
    int tick_difference = GetTickDifference(m_nStartTick, GetTickCount());
    return (float)tick_difference / (float)TICKS_PER_MILLISECOND;
}

// returns the tick counter in register 9
unsigned CTimer::GetTickCount()
{

    unsigned result;
    asm __volatile__("mfc0 %0,$9" : "=r" (result) : );
    
    return result;
}
