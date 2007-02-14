///////////////////////////////////////////////////////////////////////////////
// Simple text based instrument
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "hud/itimer.h"
#include "interfacesTTL/healthIF.h"
#include "environ/ConfigMgr.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TimerInstrument::TimerInstrument() :
TextInstrument("timer")
{
}

///////////////////////////////////////////////////////////////////////////////
// The update (called once per frame)
///////////////////////////////////////////////////////////////////////////////
void TimerInstrument::Update(float dt)
{
	// Wee!
	if	(g_referee.GetPhase() != Referee::PLAYING || !g_referee.IsTimedPhase())
		SetText(NULL);
	else
	{
		int time = g_referee.GetPhaseCountdown();
		int minutes = time / 60;
		int seconds = time - (minutes * 60);

		char timeString[64];	
		if (seconds < 10)
			sprintf(timeString, "%d:0%d", minutes, seconds);
		else
			sprintf(timeString, "%d:%d", minutes, seconds);
		SetText(timeString);
	}

	// Do the rest of the wrok
	TextInstrument::Update(dt);
}

