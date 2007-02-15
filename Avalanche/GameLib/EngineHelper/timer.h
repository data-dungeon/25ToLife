/*
**
**  File:   timer.h
**  Date:   March 24, 2000
**  By:     Bryant Collard
**  Desc:   Classes for managing time.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#ifndef TIMER_H
#define TIMER_H

#include "platform/BaseType.h"
#include "Layers/Timing.h"

/*
 * Cap filtered clock at 10 hz so anim's play Ok and ai doesn't get way out
 * of whack.
 */
#define TIMER_MAX_MS  100
#define TIMER_NO_CAP  100000

#if defined(WIN32) && !defined(_XBOX)
#include <mmsystem.h>
#endif

/*
 * A CMasterTimer maintains both raw and filtered clocks based on
 * a system clock allowing time to be sped up and slowed down.
 */

class CMasterTimer
{
  public:
	enum ETimerSpeed
	{
		PAUSE,
		ONE_SIXTEENTH,
		ONE_EIGHTH,
		ONE_FOURTH,
		ONE_HALF,
		TWO_X,
		FOUR_X,
		EIGHT_X,
		SIXTEEN_X,
		ONE_X
	};

	enum ESpeedState
	{
		PAUSED,
		SLOW,
		FAST,
		NORMAL
	};

	CMasterTimer ();
	void Reset (void);
	void SetSmoothing (float i_smoothTC);
	void SetSpeed (ETimerSpeed i_speed, bool i_smooth = false);
	float SetFilter (float i_filter);
	ETimerSpeed GetSpeed(void);
	ETimerSpeed GetSetSpeed(void);
	ESpeedState GetSpeedState(void) {return(d_speedState);};
	bool IsPaused(void) {return(d_activeSpeedState == PAUSED);}
	float ConvertSpeed(ETimerSpeed i_speed);
	ETimerSpeed ConvertSpeed(float i_speed);
	float ClampSpeed(float i_speed);
	float GetSpeedFactor(void);
	void NewFrame (void);
	uint32 GetStartMS (void);
	uint32 GetEndMS (void);
	uint32 GetFrameMS (void);
	float GetStartSec (void);
	float GetEndSec (void);
	float GetFrameSec (void);
	float GetFilteredStartSec (void);
	float GetFilteredEndSec (void);
	float GetFilteredFrameSec (void);
	uint32 GetOSTime (void);
	uint32 GetOSStartMS (void);
	uint32 GetOSEndMS (void);
	uint32 GetOSFrameMS (void);
	float GetOSStartSec (void);
	float GetOSEndSec (void);
	float GetOSFrameSec (void);
	uint32 GetFrameCount(void);
	void AdvanceTick(void) {d_advanceTick = true;}
	void SetCap(uint32 capMS) {d_capMS = capMS;}

  private:
	ETimerSpeed GetSpeed(ESpeedState i_speedState);

	uint32 d_frameCount;
	uint32 d_osFrameMS;
	uint32 d_osStartMS;
	uint32 d_osEndMS;
	uint32 d_frameMS;
	uint32 d_startMS;
	uint32 d_endMS;
	float d_frameSec;
	float d_startSec;
	float d_endSec;
	float d_filteredFrameSec;
	float d_filteredStartSec;
	float d_filteredEndSec;
	float d_filter;
	float d_filterInverse;
	ESpeedState d_speedState;
	ESpeedState d_activeSpeedState;
	int d_speedShift;
	bool d_advanceTick;
	uint32 d_capMS;
	bool d_smoothing;
	float d_smoothTC;
	float d_smoothSpeed;
	float d_smoothSpeedTarget;

// ----------------------- Save/Load Game -----------------------
  public:
	struct masterTimerSaveGameData
	{
		uint32 d_frameCount;
		uint32 d_osEndMS;
		uint32 d_osStartMS;
		uint32 d_osFrameMS;
		uint32 d_frameMS;
		uint32 d_startMS;
		uint32 d_endMS;
		float d_frameSec;
		float d_startSec;
		float d_endSec;
		float d_filteredFrameSec;
		float d_filteredStartSec;
		float d_filteredEndSec;
		float d_filter;
		float d_filterInverse;
		int d_speedShift;
		bool d_advanceTick:1;
	};

	const u32 GetSaveGameMemoryNeeded(void) const;
	const u32 SerializeSaveGame(char *pBuf, bool bDirection);
};

#define TIMER_HPP
#include "EngineHelper/timer.hpp"
#undef TIMER_HPP

extern CMasterTimer g_timer;

#endif // TIMER_H
