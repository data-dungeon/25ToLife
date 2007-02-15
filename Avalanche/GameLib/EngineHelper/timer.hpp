/*
**
**  File:   timer.hpp
**  Date:   March 24, 2000
**  By:     Bryant Collard
**  Desc:   Implementation of inlined member functions of classes for
**          managing time.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#ifdef TIMER_HPP

#define DEFAULT_FRAME_MS  17
#define DEFAULT_FILTER    0.95f

/************************** CMasterTimer **************************/

inline void CMasterTimer::SetSmoothing(float i_smoothTC)
{
	d_smoothTC = i_smoothTC;
	if (d_smoothTC <= 0.0f)
		d_smoothing = false;
}

inline float CMasterTimer::SetFilter(float i_filter)
{
	if (i_filter > 1.0f)
		d_filter = 1.0f;
	else if (i_filter < 0.0f)
		d_filter = 0.0f;
	else
		d_filter = i_filter;

	d_filterInverse = 1.0f - d_filter;

	return (d_filter);
}

inline CMasterTimer::ETimerSpeed CMasterTimer::GetSpeed(void)
{
	return(GetSpeed(d_activeSpeedState));
}

inline CMasterTimer::ETimerSpeed CMasterTimer::GetSetSpeed(void)
{
	return(GetSpeed(d_speedState));
}

inline uint32 CMasterTimer::GetStartMS(void)
{
	return (d_startMS);
}

inline uint32 CMasterTimer::GetEndMS(void)
{
	return (d_endMS);
}

inline uint32 CMasterTimer::GetFrameMS(void)
{
	return (d_frameMS);
}

inline float CMasterTimer::GetStartSec(void)
{
	return (d_startSec);
}

inline float CMasterTimer::GetEndSec(void)
{
	return (d_endSec);
}

inline float CMasterTimer::GetFrameSec(void)
{
	return (d_frameSec);
}

inline float CMasterTimer::GetFilteredStartSec(void)
{
	return (d_filteredStartSec);
}

inline float CMasterTimer::GetFilteredEndSec(void)
{
	return (d_filteredEndSec);
}

inline float CMasterTimer::GetFilteredFrameSec(void)
{
	return (d_filteredFrameSec);
}

inline uint32 CMasterTimer::GetOSTime(void)
{
	return(EngineTimer__Time());
}

inline uint32 CMasterTimer::GetOSStartMS (void)
{
   return (d_osStartMS);
}

inline uint32 CMasterTimer::GetOSEndMS (void)
{
   return (d_osEndMS);
}

inline uint32 CMasterTimer::GetOSFrameMS (void)
{
   return (d_osFrameMS);
}

inline float CMasterTimer::GetOSStartSec (void)
{
   return (0.001f * (float)d_osStartMS);
}

inline float CMasterTimer::GetOSEndSec (void)
{
   return (0.001f * (float)d_osEndMS);
}

inline float CMasterTimer::GetOSFrameSec (void)
{
   return (0.001f * (float)d_osFrameMS);
}

inline uint32 CMasterTimer::GetFrameCount(void)
{
	return (d_frameCount);
}

// --------------------------------------- Save/Load Game Functions ----------------------------

inline const u32 CMasterTimer::GetSaveGameMemoryNeeded( ) const
{
	u32 size = sizeof( masterTimerSaveGameData );

	return( Align_RoundTo16( size ) );
}

#endif // TIMER_HPP

