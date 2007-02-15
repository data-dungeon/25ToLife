/*
**
**  File:   timer.cpp
**  Date:   March 24, 2000
**  By:     Bryant Collard
**  Desc:   Classes for managing time.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

/******************* includes ****************************/

#include "EngineHelper/EngineHelperPCH.h"

#include "Game/Managers/RenderMgr.h" // for RenderMgr__GetNumVBlanksToSyncTo()
#include "Math/SFilter.h"

#include "EngineHelper/timer.h"

CMasterTimer g_timer;

/************************** CMasterTimer **************************/

CMasterTimer::CMasterTimer()
{
	d_filter = DEFAULT_FILTER;
	d_filterInverse = 1.0f - d_filter;
	d_capMS = TIMER_MAX_MS; // default cap
	d_smoothTC = 0.0f;//0.25f;
	Reset();
}

void CMasterTimer::Reset(void)
{
	d_frameCount = 0;
	d_osStartMS = 0;
	d_osEndMS = EngineTimer__Time();
	d_osFrameMS = DEFAULT_FRAME_MS;
	d_frameMS = DEFAULT_FRAME_MS;
	d_startMS = 0;
	d_endMS = 0;
	d_frameSec = ((float) d_frameMS) * 0.001f;
	d_startSec = 0.0f;
	d_endSec = 0.0f;
	d_filteredFrameSec = d_frameSec;
	d_filteredStartSec = 0.0f;
	d_filteredEndSec = 0.0f;
	d_speedState = NORMAL;
	d_activeSpeedState = NORMAL;

 	d_smoothing = false;
	d_smoothSpeed = 0.0f;
	d_smoothSpeedTarget = 0.0f;
}

void CMasterTimer::SetSpeed(ETimerSpeed i_speed, bool i_smooth)
{
	switch (d_speedState)
	{
	 case PAUSED:
		d_filteredFrameSec = ((float) DEFAULT_FRAME_MS) * 0.001f;
		break;
	 case SLOW:
	 {
		for (int i = 0; i < d_speedShift; i++)
			d_filteredFrameSec *= 2.0f;
		break;
	 }
	 case FAST:
	 {
		for (int i = 0; i < d_speedShift; i++)
			d_filteredFrameSec *= 0.5f;
		break;
	 }
	 case NORMAL:
	 default:
		break;
	}

	switch (i_speed)
	{
	 case PAUSE:
		d_speedState = PAUSED;
		break;
	 case ONE_SIXTEENTH:
		d_speedState = SLOW;
		d_speedShift = 4;
		break;
	 case ONE_EIGHTH:
		d_speedState = SLOW;
		d_speedShift = 3;
		break;
	 case ONE_FOURTH:
		d_speedState = SLOW;
		d_speedShift = 2;
		break;
	 case ONE_HALF:
		d_speedState = SLOW;
		d_speedShift = 1;
		break;
	 case TWO_X:
		d_speedState = FAST;
		d_speedShift = 1;
		break;
	 case FOUR_X:
		d_speedState = FAST;
		d_speedShift = 2;
		break;
	 case EIGHT_X:
		d_speedState = FAST;
		d_speedShift = 3;
		break;
	 case SIXTEEN_X:
		d_speedState = FAST;
		d_speedShift = 4;
		break;
	 case ONE_X:
	 default:
		d_speedState = NORMAL;
		break;
	}

	switch (d_speedState)
	{
	 case PAUSED:
		d_filteredFrameSec = 0.0f;
		break;
	 case SLOW:
	 {
		for (int i = 0; i < d_speedShift; i++)
			d_filteredFrameSec *= 0.5f;
		break;
	 }
	 case FAST:
	 {
		for (int i = 0; i < d_speedShift; i++)
			d_filteredFrameSec *= 2.0f;
		break;
	 }
	 case NORMAL:
	 default:
		break;
	}

	if (!i_smooth || Math::Zero(d_smoothTC))	
	{
		d_smoothing = false;
		//TRACE("Ending smoothing from %.2f to %.2f\n", d_smoothSpeed, d_smoothSpeedTarget);
	}
	else
	{
		d_smoothSpeed = GetSpeedFactor();
		d_smoothSpeedTarget = ConvertSpeed(GetSetSpeed());
		d_smoothing = true;
		//TRACE("Starting smoothing from %.2f to %.2f\n", d_smoothSpeed, d_smoothSpeedTarget);
	}
}

CMasterTimer::ETimerSpeed CMasterTimer::GetSpeed(ESpeedState i_speedState)
{
	switch (i_speedState)
	{
	 case PAUSED:
		return(PAUSE);
	 case SLOW:
		switch(d_speedShift)
		{
		 case 1:
			return(ONE_HALF);
		 case 2:
			return(ONE_FOURTH);
		 case 3:
			return(ONE_EIGHTH);
		}
		return(ONE_SIXTEENTH);
	 case FAST:
		switch(d_speedShift)
		{
		 case 1:
			return(TWO_X);
		 case 2:
			return(FOUR_X);
		 case 3:
			return(EIGHT_X);
		}
		return(SIXTEEN_X);
	}
	return(ONE_X);
}

float CMasterTimer::ConvertSpeed(ETimerSpeed i_speed)
{
	switch (i_speed)
	{
	 case PAUSE:
		return(0.0f);
	 case ONE_SIXTEENTH:
		return(0.0625f);
	 case ONE_EIGHTH:
		return(0.125f);
	 case ONE_FOURTH:
		return(0.25f);
	 case ONE_HALF:
		return(0.5f);
	 case TWO_X:
		return(2.0f);
	 case FOUR_X:
		return(4.0f);
	 case EIGHT_X:
		return(8.0f);
	 case SIXTEEN_X:
		return(16.0f);
	}
	return(1.0f);
}

CMasterTimer::ETimerSpeed CMasterTimer::ConvertSpeed(float i_speed)
{
	if (i_speed < 0.03125f)
		return(PAUSE);
	if (i_speed < 0.09375f)
		return(ONE_SIXTEENTH);
	if (i_speed < 0.1875f)
		return(ONE_EIGHTH);
	if (i_speed < 0.375f)
		return(ONE_FOURTH);
	if (i_speed < 0.75f)
		return(ONE_HALF);
	if (i_speed < 1.5f)
		return(ONE_X);
	if (i_speed < 3.0f)
		return(TWO_X);
	if (i_speed < 6.0f)
		return(FOUR_X);
	if (i_speed < 12.0f)
		return(EIGHT_X);
	return(SIXTEEN_X);
}

float CMasterTimer::ClampSpeed(float i_speed)
{
	if (i_speed < 0.03125f)
		return(0.0f);
	if (i_speed < 0.09375f)
		return(0.0625f);
	if (i_speed < 0.1875f)
		return(0.125f);
	if (i_speed < 0.375f)
		return(0.25f);
	if (i_speed < 0.75f)
		return(0.5f);
	if (i_speed < 1.5f)
		return(1.0f);
	if (i_speed < 3.0f)
		return(2.0f);
	if (i_speed < 6.0f)
		return(4.0f);
	if (i_speed < 12.0f)
		return(8.0f);
	return(16.0f);
}

float CMasterTimer::GetSpeedFactor(void)
{
	if (d_smoothing)
		return d_smoothSpeed;

	switch (GetSpeed(d_activeSpeedState))
	{
	 case PAUSE:
		return(0.0f);
	 case ONE_SIXTEENTH:
		return(0.0625f);
	 case ONE_EIGHTH:
		return(0.125f);
	 case ONE_FOURTH:
		return(0.25f);
	 case ONE_HALF:
		return(0.5f);
	 case TWO_X:
		return(2.0f);
	 case FOUR_X:
		return(4.0f);
	 case EIGHT_X:
		return(8.0f);
	 case SIXTEEN_X:
		return(16.0f);
	}
	return(1.0f);
}

void CMasterTimer::NewFrame( void)
{
	d_osStartMS = d_osEndMS;
	d_osEndMS = EngineTimer__Time();

	// If the new time is earlier than the old time, then time went backwards.
	// Something wrong. Fix by forcing a default frame.
	if(d_osEndMS <= d_osStartMS)
		d_osStartMS = d_osEndMS - DEFAULT_FRAME_MS;

	d_osFrameMS = d_osEndMS - d_osStartMS;
	uint32 cappedMS = d_osFrameMS;
	if (cappedMS > d_capMS)
		cappedMS = d_capMS;  // cap frame rate

	d_activeSpeedState = d_speedState;

	// Are we smoothing?
	if (d_smoothing)
	{
		d_smoothSpeed = FOFilterImplicit(d_smoothSpeed, d_smoothSpeedTarget, (float)d_osFrameMS * 0.001f, d_smoothTC);
		if (Math::Equal(d_smoothSpeed, d_smoothSpeedTarget))
			d_smoothing = false;
		else
		{
			//TRACE("Smoothing time from %.2f to %.2f\n", d_smoothSpeed, d_smoothSpeedTarget);
			cappedMS = (uint32)((float)cappedMS * d_smoothSpeed);	
			if (cappedMS == 0)
				cappedMS = 1; 
		}
	}
	else
	{	
		switch (d_activeSpeedState)
		{
		case PAUSED:
			d_frameMS = 0;
			if(d_advanceTick)
			{
				d_frameMS = (uint32)( ( int ) ( (float)RenderMgr__GetNumVBlanksToSyncTo() * 16.666667f) );
				d_advanceTick = false;
			}
			break;
		case SLOW:
			d_frameMS = cappedMS >> d_speedShift;
			if( d_frameMS == 0 )
			{
				// yes, this can really happen, if you're running 1/8 speed and 120+ fps. :/
				// give us at least one millisecond to work with, please.  :)
				d_frameMS = 1;
			}
			break;
		case FAST:
			d_frameMS = cappedMS << d_speedShift;
			break;
		case NORMAL:
		default:
			d_frameMS = cappedMS;
			break;
		}
	}

	d_startMS = d_endMS;
	d_endMS += d_frameMS;

	d_frameSec = ((float) d_frameMS) * 0.001f;
	d_startSec = d_endSec;
	d_endSec = d_startSec + d_frameSec;

	d_filteredFrameSec = d_filter * d_frameSec +
			d_filterInverse * d_filteredFrameSec;
	d_filteredStartSec = d_filteredEndSec;
	d_filteredEndSec += d_filteredFrameSec;

	d_frameCount++;
}

const u32 CMasterTimer::SerializeSaveGame( char *pBuf, bool bDirection )
{
	// --- Save Data?
	if( bDirection )
	{
		masterTimerSaveGameData *pSaveTo = ( masterTimerSaveGameData * )pBuf;

		pSaveTo->d_frameCount			= d_frameCount;
		pSaveTo->d_osEndMS				= d_osEndMS;
		pSaveTo->d_osStartMS				= d_osStartMS;
		pSaveTo->d_osFrameMS				= d_osFrameMS;
		pSaveTo->d_frameMS				= d_frameMS;
		pSaveTo->d_startMS				= d_startMS;
		pSaveTo->d_endMS					= d_endMS;
		pSaveTo->d_frameSec				= d_frameSec;
		pSaveTo->d_startSec				= d_startSec;
		pSaveTo->d_endSec					= d_endSec;
		pSaveTo->d_filteredFrameSec	= d_filteredFrameSec;
		pSaveTo->d_filteredStartSec	= d_filteredStartSec;
		pSaveTo->d_filteredEndSec		= d_filteredEndSec;
		pSaveTo->d_filter					= d_filter;
		pSaveTo->d_filterInverse		= d_filterInverse;
		pSaveTo->d_speedShift			= d_speedShift;
		pSaveTo->d_advanceTick			= d_advanceTick;
	}

	// --- Load Data
	else
	{
		masterTimerSaveGameData *pLoadFrom = ( masterTimerSaveGameData * )pBuf;

		d_frameCount						= pLoadFrom->d_frameCount;
		d_osEndMS							= pLoadFrom->d_osEndMS;
		d_osStartMS							= pLoadFrom->d_osStartMS;
		d_osFrameMS							= pLoadFrom->d_osFrameMS;
		d_frameMS							= pLoadFrom->d_frameMS;
		d_startMS							= pLoadFrom->d_startMS;
		d_endMS								= pLoadFrom->d_endMS;
		d_frameSec							= pLoadFrom->d_frameSec;
		d_startSec							= pLoadFrom->d_startSec;
		d_endSec								= pLoadFrom->d_endSec;
		d_filteredFrameSec				= pLoadFrom->d_filteredFrameSec;
		d_filteredStartSec				= pLoadFrom->d_filteredStartSec;
		d_filteredEndSec					= pLoadFrom->d_filteredEndSec;
		d_filter								= pLoadFrom->d_filter;
		d_filterInverse					= pLoadFrom->d_filterInverse;
		d_speedShift						= pLoadFrom->d_speedShift;
		d_advanceTick						= pLoadFrom->d_advanceTick;
	}

	return( CMasterTimer::GetSaveGameMemoryNeeded( ) );
}


