//============================================================================
//= SpotProfile.h - allow profiling moments in a single routine
//============================================================================

#ifndef _SPOT_PROFILE_H
#define _SPOT_PROFILE_H

#if !defined(CONSUMER_BUILD)

#include "EngineHelper/Timer.h"

#define MAX_SPOTS 32

class SpotProfile
{
public:
	struct ProfileEntry
	{
		char name[32];
		u32  elapsed;
	};
	SpotProfile()
	{
		d_numSpots = 0;
		d_start = 0;
	}
	void Stop()
	{
		u32 end = g_timer.GetOSTime();
		if (d_start && d_numSpots < MAX_SPOTS)
		{
			d_spots[d_numSpots].elapsed = end - d_start;
			d_numSpots++;
		}
		d_start = 0;
	}
	void Start(const char* name)
	{
		if (d_start)
			Stop();
		if (d_numSpots < MAX_SPOTS)
		{
			strncpy(d_spots[d_numSpots].name, name, sizeof(d_spots[d_numSpots].name) - 1);
			d_spots[d_numSpots].name[sizeof(d_spots[d_numSpots].name) - 1] = 0;
			d_start = g_timer.GetOSTime();
		}
	}
	void Dump()
	{
		if (d_start)
			Stop();
		u32 total = 0;
		for (int i = 0; i < d_numSpots; i++)
		{
			dbgPrint("%2d. %8dms %s\n", i, d_spots[i].elapsed, d_spots[i].name);
			total += d_spots[i].elapsed;
		}
		dbgPrint("%8dms TOTAL\n", total);
	}
	int d_numSpots;
	ProfileEntry d_spots[MAX_SPOTS];
	u32 d_start;
};

#else //CONSUMER_BUILD

class SpotProfile
{
public:
	SpotProfile() {}
	void Start(const char* name) {}
	void Stop() {}
	void Dump() {}
};

#endif //CONSUMER_BUILD

#endif //_SPOT_PROFILE_H
